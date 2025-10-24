#include "queue.h"
#include "../../Middleware/Inc/soft_timer.h"
#include <string.h>

/****************************************************************************
 * 队列内部结构体定义（优化版本）
 ****************************************************************************/
typedef struct {
    uint8_t* buffer;                   // 队列缓冲区指针
    uint32_t capacity;                // 队列容量
    uint32_t element_size;            // 元素大小
    uint32_t head;                    // 队首索引
    uint32_t tail;                    // 队尾索引
    uint32_t count;                   // 当前元素数量
    bool is_initialized;              // 初始化标志
    bool enable_blocking;             // 阻塞模式标志
    uint32_t timeout_ms;              // 阻塞超时时间
    QueueStats_t stats;               // 统计信息
} QueueInstance_t;

/****************************************************************************
 * 全局队列实例池和缓冲区池
 ****************************************************************************/
static QueueInstance_t g_queue_pool[QUEUE_MAX_INSTANCES];
static bool g_queue_used[QUEUE_MAX_INSTANCES] = {false};

// 全局缓冲区池 - 按需分配
static uint8_t g_queue_buffer_pool[QUEUE_MAX_INSTANCES * QUEUE_MAX_CAPACITY * QUEUE_MAX_ELEMENT_SIZE];
static bool g_buffer_used[QUEUE_MAX_INSTANCES] = {false};

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/
static bool Queue_IsValidHandle(QueueHandle_t handle);
static QueueInstance_t* Queue_GetInstance(QueueHandle_t handle);
static void Queue_UpdateStats(QueueInstance_t* instance, bool is_enqueue, bool success);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      Queue_Create
 * @brief     创建队列实例（优化版本 - 按需分配缓冲区）
 * @param     config - 队列配置参数
 * @retval    QueueHandle_t 队列句柄，失败返回0xFF
 */
QueueHandle_t Queue_Create(const QueueConfig_t* config)
{
    if (config == NULL || 
        config->capacity == 0 || 
        config->capacity > QUEUE_MAX_CAPACITY ||
        config->element_size == 0 || 
        config->element_size > QUEUE_MAX_ELEMENT_SIZE) {
        return 0xFF;  // 无效句柄
    }
    
    // 查找可用的队列实例
    QueueHandle_t handle = 0xFF;
    for (uint8_t i = 0; i < QUEUE_MAX_INSTANCES; i++) {
        if (!g_queue_used[i]) {
            handle = i;
            break;
        }
    }
    
    if (handle == 0xFF) {
        return 0xFF;  // 没有可用实例
    }
    
    // 查找可用的缓冲区
    uint8_t buffer_index = 0xFF;
    for (uint8_t i = 0; i < QUEUE_MAX_INSTANCES; i++) {
        if (!g_buffer_used[i]) {
            buffer_index = i;
            break;
        }
    }
    
    if (buffer_index == 0xFF) {
        return 0xFF;  // 没有可用缓冲区
    }
    
    QueueInstance_t* instance = &g_queue_pool[handle];
    
    // 分配缓冲区指针
    instance->buffer = &g_queue_buffer_pool[buffer_index * QUEUE_MAX_CAPACITY * QUEUE_MAX_ELEMENT_SIZE];
    
    // 初始化队列实例
    instance->capacity = config->capacity;
    instance->element_size = config->element_size;
    instance->head = 0;
    instance->tail = 0;
    instance->count = 0;
    instance->is_initialized = true;
    instance->enable_blocking = config->enable_blocking;
    instance->timeout_ms = config->timeout_ms;
    
    // 初始化统计信息
    memset(&instance->stats, 0, sizeof(QueueStats_t));
    
    // 标记为已使用
    g_queue_used[handle] = true;
    g_buffer_used[buffer_index] = true;
    
    return handle;
}

/**
 * @name      Queue_Destroy
 * @brief     销毁队列实例（优化版本 - 释放缓冲区）
 * @param     handle - 队列句柄
 * @retval    无
 */
void Queue_Destroy(QueueHandle_t handle)
{
    if (!Queue_IsValidHandle(handle)) {
        return;
    }
    
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance != NULL) {
        // 计算缓冲区索引
        uint32_t buffer_offset = instance->buffer - g_queue_buffer_pool;
        uint8_t buffer_index = buffer_offset / (QUEUE_MAX_CAPACITY * QUEUE_MAX_ELEMENT_SIZE);
        
        // 释放缓冲区
        if (buffer_index < QUEUE_MAX_INSTANCES) {
            g_buffer_used[buffer_index] = false;
        }
        
        // 重置实例
        memset(instance, 0, sizeof(QueueInstance_t));
        
        // 标记为未使用
        g_queue_used[handle] = false;
    }
}

/**
 * @name      Queue_Enqueue
 * @brief     入队操作（非阻塞）
 * @param     handle - 队列句柄
 * @param     data - 要入队的数据指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_Enqueue(QueueHandle_t handle, const void* data)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || data == NULL) {
        return QUEUE_STATUS_INVALID_PARAM;
    }
    
    if (instance->count >= instance->capacity) {
        Queue_UpdateStats(instance, true, false);
        return QUEUE_STATUS_FULL;
    }
    
    // 计算写入位置
    uint32_t write_pos = instance->tail * instance->element_size;
    
    // 复制数据到缓冲区
    memcpy(&instance->buffer[write_pos], data, instance->element_size);
    
    // 更新队尾索引
    instance->tail = (instance->tail + 1) % instance->capacity;
    instance->count++;
    
    // 更新统计信息
    Queue_UpdateStats(instance, true, true);
    
    return QUEUE_STATUS_SUCCESS;
}

/**
 * @name      Queue_Dequeue
 * @brief     出队操作（非阻塞）
 * @param     handle - 队列句柄
 * @param     data - 存储出队数据的缓冲区指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_Dequeue(QueueHandle_t handle, void* data)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || data == NULL) {
        return QUEUE_STATUS_INVALID_PARAM;
    }
    
    if (instance->count == 0) {
        Queue_UpdateStats(instance, false, false);
        return QUEUE_STATUS_EMPTY;
    }
    
    // 计算读取位置
    uint32_t read_pos = instance->head * instance->element_size;
    
    // 从缓冲区复制数据
    memcpy(data, &instance->buffer[read_pos], instance->element_size);
    
    // 更新队首索引
    instance->head = (instance->head + 1) % instance->capacity;
    instance->count--;
    
    // 更新统计信息
    Queue_UpdateStats(instance, false, true);
    
    return QUEUE_STATUS_SUCCESS;
}

/**
 * @name      Queue_EnqueueBlocking
 * @brief     入队操作（阻塞）
 * @param     handle - 队列句柄
 * @param     data - 要入队的数据指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_EnqueueBlocking(QueueHandle_t handle, const void* data)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || data == NULL) {
        return QUEUE_STATUS_INVALID_PARAM;
    }
    
    if (!instance->enable_blocking) {
        return Queue_Enqueue(handle, data);
    }
    
    uint32_t start_time = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
    
    while (instance->count >= instance->capacity) {
        uint32_t current_time = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
        
        // 检查超时
        if ((current_time - start_time) >= instance->timeout_ms) {
            Queue_UpdateStats(instance, true, false);
            return QUEUE_STATUS_FULL;
        }
        
        // 等待一段时间后重试
        // 注意：这里使用简单的循环等待，实际应用中可能需要更复杂的同步机制
    }
    
    return Queue_Enqueue(handle, data);
}

/**
 * @name      Queue_DequeueBlocking
 * @brief     出队操作（阻塞）
 * @param     handle - 队列句柄
 * @param     data - 存储出队数据的缓冲区指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_DequeueBlocking(QueueHandle_t handle, void* data)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || data == NULL) {
        return QUEUE_STATUS_INVALID_PARAM;
    }
    
    if (!instance->enable_blocking) {
        return Queue_Dequeue(handle, data);
    }
    
    uint32_t start_time = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
    
    while (instance->count == 0) {
        uint32_t current_time = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
        
        // 检查超时
        if ((current_time - start_time) >= instance->timeout_ms) {
            Queue_UpdateStats(instance, false, false);
            return QUEUE_STATUS_EMPTY;
        }
        
        // 等待一段时间后重试
        // 注意：这里使用简单的循环等待，实际应用中可能需要更复杂的同步机制
    }
    
    return Queue_Dequeue(handle, data);
}

/**
 * @name      Queue_Peek
 * @brief     查看队首元素（不移除）
 * @param     handle - 队列句柄
 * @param     data - 存储数据的缓冲区指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_Peek(QueueHandle_t handle, void* data)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || data == NULL) {
        return QUEUE_STATUS_INVALID_PARAM;
    }
    
    if (instance->count == 0) {
        return QUEUE_STATUS_EMPTY;
    }
    
    // 计算读取位置
    uint32_t read_pos = instance->head * instance->element_size;
    
    // 从缓冲区复制数据（不移除）
    memcpy(data, &instance->buffer[read_pos], instance->element_size);
    
    return QUEUE_STATUS_SUCCESS;
}

/**
 * @name      Queue_IsEmpty
 * @brief     检查队列是否为空
 * @param     handle - 队列句柄
 * @retval    bool true-空, false-非空
 */
bool Queue_IsEmpty(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return true;
    }
    
    return (instance->count == 0);
}

/**
 * @name      Queue_IsFull
 * @brief     检查队列是否已满
 * @param     handle - 队列句柄
 * @retval    bool true-满, false-未满
 */
bool Queue_IsFull(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return true;
    }
    
    return (instance->count >= instance->capacity);
}

/**
 * @name      Queue_GetCount
 * @brief     获取队列当前元素数量
 * @param     handle - 队列句柄
 * @retval    uint32_t 元素数量
 */
uint32_t Queue_GetCount(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return 0;
    }
    
    return instance->count;
}

/**
 * @name      Queue_GetCapacity
 * @brief     获取队列容量
 * @param     handle - 队列句柄
 * @retval    uint32_t 队列容量
 */
uint32_t Queue_GetCapacity(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return 0;
    }
    
    return instance->capacity;
}

/**
 * @name      Queue_Clear
 * @brief     清空队列
 * @param     handle - 队列句柄
 * @retval    无
 */
void Queue_Clear(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return;
    }
    
    instance->head = 0;
    instance->tail = 0;
    instance->count = 0;
}

/**
 * @name      Queue_GetStats
 * @brief     获取队列统计信息
 * @param     handle - 队列句柄
 * @param     stats - 统计信息结构体指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_GetStats(QueueHandle_t handle, QueueStats_t* stats)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || stats == NULL) {
        return QUEUE_STATUS_INVALID_PARAM;
    }
    
    *stats = instance->stats;
    stats->current_count = instance->count;
    
    return QUEUE_STATUS_SUCCESS;
}

/**
 * @name      Queue_ResetStats
 * @brief     重置队列统计信息
 * @param     handle - 队列句柄
 * @retval    无
 */
void Queue_ResetStats(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return;
    }
    
    memset(&instance->stats, 0, sizeof(QueueStats_t));
}

/**
 * @name      Queue_GetFreeSpace
 * @brief     获取队列剩余空间
 * @param     handle - 队列句柄
 * @retval    uint32_t 剩余空间（元素个数）
 */
uint32_t Queue_GetFreeSpace(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL) {
        return 0;
    }
    
    return instance->capacity - instance->count;
}

/**
 * @name      Queue_GetUtilization
 * @brief     获取队列利用率
 * @param     handle - 队列句柄
 * @retval    uint32_t 利用率（百分比，0-100）
 */
uint32_t Queue_GetUtilization(QueueHandle_t handle)
{
    QueueInstance_t* instance = Queue_GetInstance(handle);
    if (instance == NULL || instance->capacity == 0) {
        return 0;
    }
    
    return (instance->count * 100) / instance->capacity;
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * @name      Queue_IsValidHandle
 * @brief     检查队列句柄是否有效
 * @param     handle - 队列句柄
 * @retval    bool true-有效, false-无效
 */
static bool Queue_IsValidHandle(QueueHandle_t handle)
{
    return (handle < QUEUE_MAX_INSTANCES && g_queue_used[handle]);
}

/**
 * @name      Queue_GetInstance
 * @brief     获取队列实例指针
 * @param     handle - 队列句柄
 * @retval    QueueInstance_t* 实例指针，失败返回NULL
 */
static QueueInstance_t* Queue_GetInstance(QueueHandle_t handle)
{
    if (!Queue_IsValidHandle(handle)) {
        return NULL;
    }
    
    QueueInstance_t* instance = &g_queue_pool[handle];
    if (!instance->is_initialized) {
        return NULL;
    }
    
    return instance;
}

/**
 * @name      Queue_UpdateStats
 * @brief     更新队列统计信息
 * @param     instance - 队列实例指针
 * @param     is_enqueue - true-入队操作, false-出队操作
 * @param     success - true-操作成功, false-操作失败
 * @retval    无
 */
static void Queue_UpdateStats(QueueInstance_t* instance, bool is_enqueue, bool success)
{
    if (instance == NULL) {
        return;
    }
    
    if (is_enqueue) {
        if (success) {
            instance->stats.total_enqueued++;
        } else {
            instance->stats.overflow_count++;
        }
    } else {
        if (success) {
            instance->stats.total_dequeued++;
        } else {
            instance->stats.underflow_count++;
        }
    }
    
    // 更新最大元素数量
    if (instance->count > instance->stats.max_count) {
        instance->stats.max_count = instance->count;
    }
}