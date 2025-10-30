#include "queue.h"

/**
  ******************************************************************************
  * @file:    queue.c (renamed from lib_queue_circular.c)
  ******************************************************************************
  */

/**
  * 以下为原 lib_queue_circular.c 的实现，头文件改为 queue.h
  */

/**
  * @brief  销毁队列
  */
void lib_queue_destroy(st_queue_ptr queue) {
    if (queue == NULL) {
        return;
    }
    QUEUE_API_LOCK(queue);
    // free(queue->buffer);  // 暂不释放，由用户管理
    QUEUE_API_UNLOCK(queue);
    QUEUE_API_DELETE_LOCK(queue);
}

/** 获取已使用元素数 */
size_t lib_queue_used(st_queue_ptr queue) {
    if (queue == NULL) {
        return 0;
    }
    QUEUE_API_LOCK(queue);
    size_t num_in_queue = QUEUE_USED_COUNT(queue);
    QUEUE_API_UNLOCK(queue);
    return num_in_queue;
}

/** 获取剩余空间 */
size_t lib_queue_surplus(st_queue_ptr queue) {
    if (queue == NULL) {
        return 0;
    }
    QUEUE_API_LOCK(queue);
    size_t left_num_in_queue = QUEUE_FREE_COUNT(queue);
    QUEUE_API_UNLOCK(queue);
    return left_num_in_queue;
}

/** 获取容量(len-1) */
size_t lib_queue_capacity(st_queue_ptr queue) {
    return (queue == NULL) ? 0 : (queue->len - 1);
}

/** 是否满 */
bool lib_queue_full(st_queue_ptr queue) {
    if (queue == NULL) {
        return false;
    }
    return (lib_queue_used(queue) == (queue->len - 1));
}

/** 是否空 */
bool lib_queue_empty(st_queue_ptr queue) {
    return (queue == NULL) ? false : (lib_queue_used(queue) == 0);
}

static size_t lib_queue_minimum(size_t a, size_t b) {
    return (a < b) ? a : b;
}

static void* lib_queue_copy(void* dst, const void* src, size_t n) {
    return memcpy(dst, src, n);
}

/** 入队(支持批量) */
bool lib_queue_put(st_queue_ptr queue, void* buf, size_t num) {
    if ((queue == NULL) || (queue->buffer == NULL) || (buf == NULL) || (num == 0)) {
        return false;
    }
    bool result = false;
    QUEUE_API_LOCK(queue);
    size_t left_num_in_queue = QUEUE_FREE_COUNT(queue);
    if (num <= left_num_in_queue) {
        size_t temp_len = lib_queue_minimum((queue->len) - (queue->rear), num);
        if (temp_len > 0) {
            lib_queue_copy((char*)(queue->buffer) + (queue->rear) * (queue->size), buf, temp_len * (queue->size));
        }
        if (num > temp_len) {
            lib_queue_copy((char*)(queue->buffer), (char*)buf + temp_len * (queue->size), (num - temp_len) * (queue->size));
        }
        queue->rear = (queue->rear + num) % (queue->len);
        result = true;
    }
    QUEUE_API_UNLOCK(queue);
    return result;
}

/** 出队(支持批量) */
bool lib_queue_get(st_queue_ptr queue, void* buf, size_t num) {
    if ((queue == NULL) || (queue->buffer == NULL) || (buf == NULL) || (num == 0)) {
        return false;
    }
    bool result = false;
    QUEUE_API_LOCK(queue);
    size_t num_in_queue = QUEUE_USED_COUNT(queue);
    if (num <= num_in_queue) {
        size_t temp_len = lib_queue_minimum((queue->len) - (queue->front), num);
        if (temp_len > 0) {
            lib_queue_copy((char*)buf, (char*)(queue->buffer) + (queue->front) * (queue->size), temp_len * (queue->size));
        }
        if (num > temp_len) {
            lib_queue_copy((char*)buf + temp_len * (queue->size), queue->buffer, (num - temp_len) * (queue->size));
        }
        queue->front = (queue->front + num) % (queue->len);
        result = true;
    }
    QUEUE_API_UNLOCK(queue);
    return result;
}

/** 观察不移除 */
bool lib_queue_peek(st_queue_ptr queue, void* buf, size_t num, size_t offset) {
    if ((queue == NULL) || (queue->buffer == NULL) || (buf == NULL) || (num == 0)) {
        return false;
    }
    bool result = false;
    QUEUE_API_LOCK(queue);
    size_t num_in_queue = QUEUE_USED_COUNT(queue);
    if ((offset + num) <= num_in_queue) {
        size_t temp_front = (queue->front) + offset;
        size_t temp_len = lib_queue_minimum((queue->len) - temp_front, num);
        if (temp_len > 0) {
            lib_queue_copy((char*)buf, (char*)(queue->buffer) + temp_front * (queue->size), temp_len * (queue->size));
        }
        if (num > temp_len) {
            lib_queue_copy((char*)buf + temp_len * (queue->size), queue->buffer, (num - temp_len) * (queue->size));
        }
        result = true;
    }
    QUEUE_API_UNLOCK(queue);
    return result;
}

/** 清除指定数量 */
bool lib_queue_clear(st_queue_ptr queue, size_t num) {
    if ((queue == NULL) || (num == 0)) {
        return false;
    }
    bool result = true;
    QUEUE_API_LOCK(queue);
    size_t num_in_queue = QUEUE_USED_COUNT(queue);
    if (num <= num_in_queue) {
        queue->front = (queue->front + num) % (queue->len);
    } else {
        result = false;
    }
    QUEUE_API_UNLOCK(queue);
    return result;
}

/** 清空队列 */
bool lib_queue_clears(st_queue_ptr queue) {
    if (queue == NULL) {
        return false;
    }
    QUEUE_API_LOCK(queue);
    queue->front = queue->rear;
    QUEUE_API_UNLOCK(queue);
    return true;
}

/** 初始化参数 */
void lib_queue_initialize(st_queue_ptr ptr, size_t queue_len, size_t item_size, void* buffer) {
    ptr->size = item_size;
    ptr->len = queue_len;
    ptr->buffer = buffer;
    ptr->front = ptr->rear = 0;
}

/** 配置函数指针 */
void lib_queue_configure(st_queue_ptr ptr) {
    ptr->size = 0;
    ptr->len = 0;
    ptr->buffer = NULL;
    ptr->front = ptr->rear = 0;
    ptr->used = lib_queue_used;
    ptr->surplus = lib_queue_surplus;
    ptr->capacity = lib_queue_capacity;
    ptr->full = lib_queue_full;
    ptr->empty = lib_queue_empty;
    ptr->put = lib_queue_put;
    ptr->get = lib_queue_get;
    ptr->peek = lib_queue_peek;
    ptr->clear = lib_queue_clear;
    ptr->clears = lib_queue_clears;
    ptr->destroy = lib_queue_destroy;
}

/** 创建实例(设置函数指针) */
void lib_queue_create(st_queue_ptr ptr) {
    ptr->configure = lib_queue_configure;
    ptr->initialize = lib_queue_initialize;
}



