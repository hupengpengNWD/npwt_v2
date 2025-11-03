/**
 * @file    app_ui.c
 * @brief   应用层UI模块实现
 * @date    2025-01-27
 * 
 * 应用层UI模块，使用FSM组件实现按键触发UI界面切换功能
 */

#include "../Inc/app_ui.h"
#include "../Inc/app_button.h"    // 获取KeyEvent_t和队列接口
#include "../../Middleware/Inc/fsm.h"
#include "../../Middleware/Inc/display.h"
#include "../../Middleware/Inc/key_machine.h"
#include "../../Middleware/Inc/soft_timer.h"

/****************************************************************************
 * 私有变量
 ****************************************************************************/

/* FSM实例 */
static st_fsm g_ui_fsm;

/* UI上下文 */
static UIContext_t g_ui_context;

/* 按键事件队列指针（从app_button模块获取） */
static st_queue_ptr g_key_event_queue = NULL;

/* 初始化超时定时器句柄 */
static SoftTimerHandle_t g_init_timeout_timer = 0;

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

static void AppUI_StateEntry_SYS(void* arg, st_fsm_event event);
static void AppUI_StateEntry_WAT(void* arg, st_fsm_event event);
static void AppUI_StateEntry_LIX(void* arg, st_fsm_event event);
static void AppUI_StateEntry_JIX(void* arg, st_fsm_event event);
static void AppUI_StateEntry_ZHT(void* arg, st_fsm_event event);
static void AppUI_StateEntry_SET(void* arg, st_fsm_event event);
static void AppUI_SwitchWorkMode(void* arg, st_fsm_event event);

static void AppUI_Display_SYS(void);
static void AppUI_Display_WAT(void);
static void AppUI_Display_LIX(void);
static void AppUI_Display_JIX(void);
static void AppUI_Display_ZHT(void);
static void AppUI_Display_SET(void);

static UIEvent_e AppUI_ConvertKeyEvent(uint8_t key_id, KeyMachineEvent_e key_event);

/* 初始化超时定时器回调函数 */
static void AppUI_InitTimeoutCallback(void* user_data);

/****************************************************************************
 * FSM状态转换表（必须在函数声明之后定义）
 ****************************************************************************/

/* FSM状态转换表 */
//static const st_fsm_transition g_ui_transition_table[13] = {
 const st_fsm_transition g_ui_transition_table[13] = {
    /* 当前状态      触发事件          动作函数              下一状态 */
    
    /* MOD_SYS (初始化模式) 状态转换 */
    [0] =
    {
        .current_state = UI_STATE_SYS,  
        .trigger_event = UI_EVENT_TIMEOUT, 
        .action_func   = AppUI_StateEntry_WAT,  
        .next_state    = UI_STATE_WAT
    },
    
    /* MOD_WAT (待机模式) 状态转换 */
    [1] = 
    {       
        .current_state = UI_STATE_WAT, 
        .trigger_event = UI_EVENT_MENU_UP, 
        .action_func   = AppUI_StateEntry_SET,  
        .next_state    = UI_STATE_SET
    },  
    
    [2]=
    {
        .current_state = UI_STATE_WAT,  
        .trigger_event = UI_EVENT_START,   
        .action_func   = AppUI_StateEntry_LIX,  
        .next_state    = UI_STATE_LIX
    },   
    
    /* MOD_LIX (连续工作模式) 状态转换 */
    [3]=
    {
        
        .current_state = UI_STATE_LIX,  
        .trigger_event = UI_EVENT_CONFIRM, 
        .action_func   = AppUI_StateEntry_ZHT,  
        .next_state    = UI_STATE_ZHT
    },  

    [4]=
    {
        .current_state = UI_STATE_LIX,  
        .trigger_event = UI_EVENT_SETTINGS, 
        .action_func   = AppUI_StateEntry_SET, 
        .next_state    = UI_STATE_SET
    },   
    
    /* MOD_JIX (间歇工作模式) 状态转换 */
    [5]=
    {
        .current_state = UI_STATE_JIX,  
        .trigger_event = UI_EVENT_CONFIRM, 
        .action_func   = AppUI_StateEntry_ZHT,   
        .next_state    = UI_STATE_ZHT
    },  

    [6]=
    {
        .current_state = UI_STATE_JIX,  
        .trigger_event = UI_EVENT_SETTINGS, 
        .action_func   = AppUI_StateEntry_SET,  
        .next_state    = UI_STATE_SET
    }, 
    
    /* MOD_ZHT (暂停模式) 状态转换 */
    [7]=
    {
        .current_state = UI_STATE_ZHT,  
        .trigger_event = UI_EVENT_CONFIRM, 
        .action_func   = AppUI_StateEntry_WAT,   
        .next_state    = UI_STATE_WAT
    },   
    
    [8]=
    {
        .current_state = UI_STATE_ZHT,  
        .trigger_event = UI_EVENT_SETTINGS, 
        .action_func   = AppUI_StateEntry_SET,  
        .next_state    = UI_STATE_SET
    },  
    
    /* MOD_SET (设置模式) 状态转换 */
    [9]=
    {
        .current_state = UI_STATE_SET,  
        .trigger_event = UI_EVENT_SETTINGS, 
        .action_func   = AppUI_StateEntry_ZHT, 
        .next_state    = UI_STATE_ZHT
    },  
    
    [10]=
    {
        .current_state = UI_STATE_SET,  
        .trigger_event = UI_EVENT_CONFIRM, 
        .action_func   = AppUI_StateEntry_SET,  
        .next_state    = UI_STATE_SET
    },   
    
    [11]=
    {
        .current_state = UI_STATE_SET,  
        .trigger_event = UI_EVENT_MENU_UP, 
        .action_func   = AppUI_SwitchWorkMode,  
        .next_state    = UI_STATE_SET
    },   
    
    [12]=
    {
        .current_state = UI_STATE_SET,  
        .trigger_event = UI_EVENT_MENU_DOWN, 
        .action_func   = AppUI_SwitchWorkMode,  
        .next_state    = UI_STATE_SET
    },   
};

/****************************************************************************
 * 状态进入动作函数实现
 ****************************************************************************/

/**
 * @name      AppUI_StateEntry_SYS
 * @brief     初始化模式进入动作
 */
static void AppUI_StateEntry_SYS(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->current_state = UI_STATE_SYS;
    ctx->work_mode_backup = UI_STATE_LIX;  // 默认连续模式
    AppUI_Display_SYS();
}

/**
 * @name      AppUI_StateEntry_WAT
 * @brief     待机模式进入动作
 */
static void AppUI_StateEntry_WAT(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_WAT;
    Display_Clear();
    AppUI_Display_WAT();
}

/**
 * @name      AppUI_StateEntry_LIX
 * @brief     连续工作模式进入动作
 */
static void AppUI_StateEntry_LIX(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_LIX;
    ctx->work_mode_backup = UI_STATE_LIX;
    Display_Clear();
    AppUI_Display_LIX();
}

/**
 * @name      AppUI_StateEntry_JIX
 * @brief     间歇工作模式进入动作
 */
static void AppUI_StateEntry_JIX(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_JIX;
    ctx->work_mode_backup = UI_STATE_JIX;
    Display_Clear();
    AppUI_Display_JIX();
}

/**
 * @name      AppUI_StateEntry_ZHT
 * @brief     暂停模式进入动作
 */
static void AppUI_StateEntry_ZHT(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_ZHT;
    Display_Clear();
    AppUI_Display_ZHT();
}

/**
 * @name      AppUI_StateEntry_SET
 * @brief     设置模式进入动作
 */
static void AppUI_StateEntry_SET(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_SET;
    ctx->settings_sub_state = 0;  // 重置设置子状态
    Display_Clear();
    AppUI_Display_SET();
}

/**
 * @name      AppUI_SwitchWorkMode
 * @brief     切换工作模式（在设置界面中）
 */
static void AppUI_SwitchWorkMode(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    // 切换工作模式：连续 <-> 间歇
    if (ctx->work_mode_backup == UI_STATE_LIX) {
        ctx->work_mode_backup = UI_STATE_JIX;
    } else if (ctx->work_mode_backup == UI_STATE_JIX) {
        ctx->work_mode_backup = UI_STATE_LIX;
    }
    
    // 注意：显示刷新由AppUI_Process统一处理，这里只需要更新work_mode_backup
    // AppUI_Process会检测到work_mode_backup变化并自动刷新显示
}

/****************************************************************************
 * 显示函数实现
 ****************************************************************************/

/**
 * @name      AppUI_Display_SYS
 * @brief     显示初始化界面
 */
static void AppUI_Display_SYS(void)
{
    // 显示开机Logo（由display模块处理）
    Display_ShowStartupInterface();
}

/**
 * @name      AppUI_Display_WAT
 * @brief     显示待机界面（主菜单）
 */
static void AppUI_Display_WAT(void)
{

//    Display_ShowIcon(0, 0, ICON_INTERMITTENT);//显示图标时15才是0  
    
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        Display_ShowIcon(0, 0, ICON_CONTINUOUS);  // 连续模式图标
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        Display_ShowIcon(0, 0, ICON_INTERMITTENT); // 间歇模式图标
    }
    
    Display_ShowString(25, 0, "-135 mmHg", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    Display_ShowIcon(32, 2, ICON_KEY2);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(48, 2, "Settings", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
   
    Display_ShowIcon(35, 4, ICON_KEY1);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(48, 4, "Therapy", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    

//    if (g_ui_context.lock_flag) {
//        Display_ShowIcon(108, 6, ICON_LOCK);  // 锁定图标（8x16）
//    }
    
    // 显示静音图标（条件显示，根据未重构工程：DISP_Buz(6,25)，页6，列25）
//     if (mute_flg) {
//         Display_ShowIcon(25, 6, ICON_SILENT);  // 静音图标（16x16）
//     }
}

/**
 * @name      AppUI_Display_LIX
 * @brief     显示连续工作模式界面
 */
static void AppUI_Display_LIX(void)
{
    // 显示工作模式标识
    Display_ShowString(0, 0, "Continuous", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowString(0, 2, "Therapy On", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    // 显示压力值（占位，实际应从传感器读取）
    Display_ShowString(0, 4, "Pressure: -125 mmHg", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_Display_JIX
 * @brief     显示间歇工作模式界面
 */
static void AppUI_Display_JIX(void)
{
    // 显示工作模式标识
    Display_ShowString(0, 0, "Intermittent", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowString(0, 2, "Therapy On", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    // 显示压力值（占位，实际应从传感器读取）
    Display_ShowString(0, 4, "Pressure: -125 mmHg", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_Display_ZHT
 * @brief     显示暂停模式界面
 */
static void AppUI_Display_ZHT(void)
{
    // 显示暂停状态
    Display_ShowString(0, 1, "Therapy Off", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(0, 3, "| Therapy", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    
    // 显示目标压力
    Display_ShowString(0, 5, "-125 mmHg", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_Display_SET
 * @brief     显示设置模式界面
 */
static void AppUI_Display_SET(void)
{
    // 显示设置菜单
    Display_ShowString(2, 0, "Mode", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowIcon(66, 0, ICON_KEY2);  
    Display_ShowString(78, 0, "Switch", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(2, 2, "Continuous", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(1, 4, "Intermittent", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    // 根据当前选中的模式显示勾号
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        // 连续模式选中：在第2行显示勾号
        Display_ShowIcon(106, 2, ICON_TICK); 
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        // 间歇模式选中：在第4行显示勾号
        Display_ShowIcon(106, 4, ICON_TICK); 
    }
}

/****************************************************************************
 * 辅助函数实现
 ****************************************************************************/

/**
 * @name      AppUI_ConvertKeyEvent
 * @brief     将按键事件转换为UI事件
 */
static UIEvent_e AppUI_ConvertKeyEvent(uint8_t key_id, KeyMachineEvent_e key_event)
{
    /* key_id: 0=OK/START, 1=UP, 2=DN, 3=CANCEL */
    
    if (key_event == KEY_MACHINE_EVENT_LONG_PRESS) {
        switch (key_id) {
            case 1: return UI_EVENT_NONE;        // 上键长按 -> 不处理（已删除设置菜单功能）
            case 2: return UI_EVENT_QUICK_DOWN;  // 下键长按 -> 快速向下
            case 0: return UI_EVENT_START;       // 启动键长按 -> 启动治疗
            default: return UI_EVENT_NONE;
        }
    }
    else if (key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
        // 长按释放事件：等效于原来的短按/单击，用于菜单选择和确认操作
        switch (key_id) {
            case 0: return UI_EVENT_CONFIRM;     // OK键 -> 确认
            case 1: return UI_EVENT_MENU_UP;     // 上键 -> 菜单向上
            case 2: return UI_EVENT_MENU_DOWN;  // 下键 -> 菜单向下
            case 3: return UI_EVENT_CONFIRM;     // 取消/静音键 -> 确认
            default: return UI_EVENT_NONE;
        }
    }
    else if (key_event == KEY_MACHINE_EVENT_ULTRA_LONG_PRESS_RELEASE) {
        // 超长按释放事件：可用于特殊功能
        // 当前暂不处理，返回NONE
        return UI_EVENT_NONE;
    }
    
    return UI_EVENT_NONE;
}

/****************************************************************************
 * 公共接口实现
 ****************************************************************************/

/**
 * @name      AppUI_InitTimeoutCallback
 * @brief     初始化超时定时器回调函数（7秒后触发超时事件）
 * @param     user_data - 用户数据（未使用）
 * @retval    无
 */
static void AppUI_InitTimeoutCallback(void* user_data)
{
    (void)user_data;
    
    /* 在状态转换前清空显示队列，确保新状态的显示事件能全部入队 */
    Display_ClearQueue();
    
    /* 创建超时事件并放入FSM队列 */
    st_fsm_event fsm_event = {0};
    fsm_event.event_type = (uint8_t)UI_EVENT_TIMEOUT;
    fsm_event.event_user = 0;
    
    /* 将事件加入FSM队列 */
    g_ui_fsm.event_trigger_queue->put(g_ui_fsm.event_trigger_queue, &fsm_event, sizeof(fsm_event));
}

/**
 * @name      AppUI_Init
 * @brief     初始化UI模块
 */
void AppUI_Init(void)
{
    /* 初始化UI上下文 */
    g_ui_context.current_state = UI_STATE_SYS;
    g_ui_context.last_state = UI_STATE_SYS;
    g_ui_context.work_mode_backup = UI_STATE_LIX;
    g_ui_context.lock_flag = false;
    g_ui_context.settings_sub_state = 0;
    g_ui_context.user_data = NULL;
    
    /* 获取按键事件队列指针 */
    g_key_event_queue = AppButton_GetKeyEventQueue();
    
    /* 创建FSM实例 */
    g_ui_fsm.fsm_id = FSM_ID_0;
    g_ui_fsm.trans_table = g_ui_transition_table;
    g_ui_fsm.trans_size = (uint8_t)(sizeof(g_ui_transition_table) / sizeof(g_ui_transition_table[0]));
    g_ui_fsm.current_state = UI_STATE_SYS;
    g_ui_fsm.last_state = UI_STATE_SYS;
    g_ui_fsm.user_arg = &g_ui_context;
    
    /* 创建并初始化FSM */
    if (fsm_create(&g_ui_fsm) == 0) {
        g_ui_fsm.configure(&g_ui_fsm);
        g_ui_fsm.initialize(&g_ui_fsm);
    }
    
    /* 创建初始化超时定时器（7秒，单次模式） */
    /* 注意：SoftTimer_Create的参数单位是毫秒，所以7秒应该填写7000 */
    g_init_timeout_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE, 
                                            7000,  // 7秒 = 7000毫秒
                                            AppUI_InitTimeoutCallback, 
                                            NULL);
    
    if (g_init_timeout_timer != 0) {
        /* 启动定时器 */
        SoftTimer_Start(g_init_timeout_timer);
    }
    
    /* 进入初始化状态 */
    AppUI_StateEntry_SYS(&g_ui_context, (st_fsm_event){0});
}

/**
 * @name      AppUI_Process
 * @brief     处理UI状态机（每10ms调用一次）
 * 
 * 功能说明：
 * - 处理按键事件队列，转换为FSM事件
 * - 处理FSM事件队列，执行状态转换（状态转换时的显示由状态入口函数处理）
 * - 检测SET状态下work_mode_backup的变化，使用局部清除优化更新勾号位置
 */
void AppUI_Process(void)
{
    /* 第一步：从按键事件队列读取并转换为FSM事件 */
    if (g_key_event_queue != NULL) {
        KeyEvent_t key_event;
        /* 处理队列中的所有按键事件（批量处理） */
        while (g_key_event_queue->empty(g_key_event_queue) == false) {
            if (g_key_event_queue->get(g_key_event_queue, &key_event, sizeof(key_event))) {
                /* 转换按键事件为UI事件 */
                UIEvent_e ui_event = AppUI_ConvertKeyEvent(key_event.key_id, key_event.key_event);
                
                if (ui_event != UI_EVENT_NONE) {
                    /* 创建FSM事件并放入队列 */
                    st_fsm_event fsm_event = {0};
                    fsm_event.event_type = (uint8_t)ui_event;
                    fsm_event.event_user = key_event.key_id;
                    
                    /* 将事件加入FSM队列 */
                    g_ui_fsm.event_trigger_queue->put(g_ui_fsm.event_trigger_queue, &fsm_event, sizeof(fsm_event));
                }
            }
        }
    }
    
    /* 第二步：从FSM事件队列获取事件并处理 */
    st_fsm_event event;
    
    /* 检查FSM事件队列 */
    if (g_ui_fsm.event_trigger_queue->empty(g_ui_fsm.event_trigger_queue) == false) {
        if (g_ui_fsm.event_trigger_queue->get(g_ui_fsm.event_trigger_queue, &event, sizeof(event))) {
            /* 处理事件（如果未锁定） */
            if (g_ui_context.lock_flag == false) {
                g_ui_fsm.poll(&g_ui_fsm, event);
            }
        }
    }
    
    /* 第三步：检测SET状态下work_mode_backup的变化（状态转换时的显示由状态入口函数处理） */
    /* 使用静态变量保存上次的工作模式 */
    static UIState_e last_work_mode_backup = UI_STATE_COUNT;  // 保存上次的工作模式
    
    if (g_ui_context.current_state == UI_STATE_SET && 
        g_ui_context.work_mode_backup != last_work_mode_backup) {
        /* SET状态下，work_mode_backup变化时使用局部清除优化 */
        
        // 只显示新的勾号位置（避免重绘整个界面）
        if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
            // 连续模式选中：在第2行显示勾号
            Display_ClearRect(106, 4, 16, 16);
            Display_ShowIcon(106, 2, ICON_TICK);
            
        } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
            // 间歇模式选中：在第4行显示勾号
            Display_ShowIcon(106, 4, ICON_TICK);
            Display_ClearRect(106, 2, 16, 16);
        }
        
        last_work_mode_backup = g_ui_context.work_mode_backup;  // 更新备份
    }
    /* 注意：如果状态未变化且work_mode_backup也未变化，不执行显示刷新，避免不必要的重复渲染 */
}

/* 注意：AppUI_OnKeyEvent函数已移除，改为使用队列机制
 * 按键事件现在通过app_button模块的队列传递到app_ui模块
 * 在AppUI_Process中统一从队列读取并处理
 */

/**
 * @name      AppUI_GetCurrentState
 * @brief     获取当前UI状态
 */
UIState_e AppUI_GetCurrentState(void)
{
    return g_ui_context.current_state;
}

/**
 * @name      AppUI_SetLockFlag
 * @brief     设置锁定标志
 */
void AppUI_SetLockFlag(bool locked)
{
    g_ui_context.lock_flag = locked;
}

/**
 * @name      AppUI_GetLockFlag
 * @brief     获取锁定标志
 */
bool AppUI_GetLockFlag(void)
{
    return g_ui_context.lock_flag;
}
