#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/****************************************************************************
 * 队列组件 - 通用环形缓冲区队列（嵌入式优化版本）
 * 
 * 功能特性：
 * - 支持任意数据类型
 * - 静态内存分配（适合嵌入式系统）
 * - 内存高效（环形缓冲区）
 * - 支持阻塞和非阻塞操作
 * - 支持队列状态查询
 * - 预定义队列实例数量
 ****************************************************************************/

/****************************************************************************
 * 队列配置定义
 ****************************************************************************/
#define QUEUE_MAX_INSTANCES     4    // 最大队列实例数（减少内存使用）
#define QUEUE_MAX_CAPACITY      16   // 单个队列最大容量（减少内存使用）
#define QUEUE_MAX_ELEMENT_SIZE  16   // 单个元素最大大小（字节）（减少内存使用）

/****************************************************************************
 * 队列状态定义
 ****************************************************************************/
typedef enum {
    QUEUE_STATUS_SUCCESS = 0,        // 操作成功
    QUEUE_STATUS_ERROR,              // 通用错误
    QUEUE_STATUS_FULL,               // 队列已满
    QUEUE_STATUS_EMPTY,              // 队列为空
    QUEUE_STATUS_INVALID_PARAM,      // 无效参数
    QUEUE_STATUS_NOT_INITIALIZED,    // 未初始化
    QUEUE_STATUS_NO_MEMORY,          // 内存不足
    QUEUE_STATUS_INVALID_HANDLE      // 无效句柄
} QueueStatus_e;

/****************************************************************************
 * 队列配置结构体
 ****************************************************************************/
typedef struct {
    uint32_t capacity;               // 队列容量（元素个数）
    uint32_t element_size;           // 单个元素大小（字节）
    bool enable_blocking;            // 是否启用阻塞模式
    uint32_t timeout_ms;             // 阻塞超时时间（毫秒）
} QueueConfig_t;

/****************************************************************************
 * 队列句柄类型
 ****************************************************************************/
typedef uint8_t QueueHandle_t;      // 队列句柄（索引）

/****************************************************************************
 * 队列统计信息结构体
 ****************************************************************************/
typedef struct {
    uint32_t current_count;          // 当前元素数量
    uint32_t max_count;              // 历史最大元素数量
    uint32_t total_enqueued;         // 总入队次数
    uint32_t total_dequeued;         // 总出队次数
    uint32_t overflow_count;         // 溢出次数
    uint32_t underflow_count;        // 下溢次数
} QueueStats_t;

/****************************************************************************
 * 函数声明
 ****************************************************************************/

/**
 * @name      Queue_Create
 * @brief     创建队列实例
 * @param     config - 队列配置参数
 * @retval    QueueHandle_t 队列句柄，失败返回NULL
 */
QueueHandle_t Queue_Create(const QueueConfig_t* config);

/**
 * @name      Queue_Destroy
 * @brief     销毁队列实例
 * @param     handle - 队列句柄
 * @retval    无
 */
void Queue_Destroy(QueueHandle_t handle);

/**
 * @name      Queue_Enqueue
 * @brief     入队操作（非阻塞）
 * @param     handle - 队列句柄
 * @param     data - 要入队的数据指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_Enqueue(QueueHandle_t handle, const void* data);

/**
 * @name      Queue_Dequeue
 * @brief     出队操作（非阻塞）
 * @param     handle - 队列句柄
 * @param     data - 存储出队数据的缓冲区指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_Dequeue(QueueHandle_t handle, void* data);

/**
 * @name      Queue_EnqueueBlocking
 * @brief     入队操作（阻塞）
 * @param     handle - 队列句柄
 * @param     data - 要入队的数据指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_EnqueueBlocking(QueueHandle_t handle, const void* data);

/**
 * @name      Queue_DequeueBlocking
 * @brief     出队操作（阻塞）
 * @param     handle - 队列句柄
 * @param     data - 存储出队数据的缓冲区指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_DequeueBlocking(QueueHandle_t handle, void* data);

/**
 * @name      Queue_Peek
 * @brief     查看队首元素（不移除）
 * @param     handle - 队列句柄
 * @param     data - 存储数据的缓冲区指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_Peek(QueueHandle_t handle, void* data);

/**
 * @name      Queue_IsEmpty
 * @brief     检查队列是否为空
 * @param     handle - 队列句柄
 * @retval    bool true-空, false-非空
 */
bool Queue_IsEmpty(QueueHandle_t handle);

/**
 * @name      Queue_IsFull
 * @brief     检查队列是否已满
 * @param     handle - 队列句柄
 * @retval    bool true-满, false-未满
 */
bool Queue_IsFull(QueueHandle_t handle);

/**
 * @name      Queue_GetCount
 * @brief     获取队列当前元素数量
 * @param     handle - 队列句柄
 * @retval    uint32_t 元素数量
 */
uint32_t Queue_GetCount(QueueHandle_t handle);

/**
 * @name      Queue_GetCapacity
 * @brief     获取队列容量
 * @param     handle - 队列句柄
 * @retval    uint32_t 队列容量
 */
uint32_t Queue_GetCapacity(QueueHandle_t handle);

/**
 * @name      Queue_Clear
 * @brief     清空队列
 * @param     handle - 队列句柄
 * @retval    无
 */
void Queue_Clear(QueueHandle_t handle);

/**
 * @name      Queue_GetStats
 * @brief     获取队列统计信息
 * @param     handle - 队列句柄
 * @param     stats - 统计信息结构体指针
 * @retval    QueueStatus_e 操作状态
 */
QueueStatus_e Queue_GetStats(QueueHandle_t handle, QueueStats_t* stats);

/**
 * @name      Queue_ResetStats
 * @brief     重置队列统计信息
 * @param     handle - 队列句柄
 * @retval    无
 */
void Queue_ResetStats(QueueHandle_t handle);

/**
 * @name      Queue_GetFreeSpace
 * @brief     获取队列剩余空间
 * @param     handle - 队列句柄
 * @retval    uint32_t 剩余空间（元素个数）
 */
uint32_t Queue_GetFreeSpace(QueueHandle_t handle);

/**
 * @name      Queue_GetUtilization
 * @brief     获取队列利用率
 * @param     handle - 队列句柄
 * @retval    uint32_t 利用率（百分比，0-100）
 */
uint32_t Queue_GetUtilization(QueueHandle_t handle);

#endif /* __QUEUE_H */
