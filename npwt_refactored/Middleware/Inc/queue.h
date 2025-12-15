 /****************************************************************************
  * 文件名: queue.h
  * 功能: 循环队列
  * 作者: 韦睿医疗
  * 说明:
  *
  * 创建日期: 2025-11-07
  ****************************************************************************/
#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* 锁机制宏定义 */
#define QUEUE_API_CREATE_LOCK(v)
#define QUEUE_API_DELETE_LOCK(v)
#define QUEUE_API_LOCK(v)
#define QUEUE_API_UNLOCK(v)

/* 队列状态计算宏 */
#define QUEUE_USED_COUNT(v) ((((v)->front) <= ((v)->rear)) ? \
                            (((v)->rear) - ((v)->front)) : \
                            (((v)->len) - ((v)->front) + ((v)->rear)))
#define QUEUE_FREE_COUNT(v) ((((v)->front) <= ((v)->rear)) ? \
                            ((((v)->len) - 1) - (((v)->rear) - ((v)->front))) : \
                            (((v)->front) - ((v)->rear) - 1))

/* 队列结构体 */
typedef struct queue {
    void* buffer;              /* 数据缓冲区 */
    size_t len;                /* 队列长度(物理长度，容量=len-1) */
    size_t size;               /* 单个数据大小(字节) */
    size_t front;              /* 数据头,指向下一个空闲存放地址 */
    size_t rear;               /* 数据尾，指向第一个数据 */
    
    /* 函数指针 */
    void (*initialize)(struct queue*, size_t, size_t, void*);
    void (*configure)(struct queue*);
    void (*destroy)(struct queue*);
    
    /* 状态查询函数 */
    size_t (*capacity)(struct queue*);
    size_t (*used)(struct queue*);
    size_t (*surplus)(struct queue*);
    bool (*full)(struct queue*);
    bool (*empty)(struct queue*);
    
    /* 数据操作函数 */
    bool (*put)(struct queue*, void*, size_t);
    bool (*get)(struct queue*, void*, size_t);
    bool (*peek)(struct queue*, void*, size_t, size_t);
    bool (*clear)(struct queue*, size_t);
    bool (*clears)(struct queue*);
} st_queue, *st_queue_ptr;

/* API */
void lib_queue_create(st_queue_ptr ptr);

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */
