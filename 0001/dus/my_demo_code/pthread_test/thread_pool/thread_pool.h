#ifndef __THREAD_POOL_H_INC__
#define __THREAD_POOL_H_INC__
#include <stdio.h>
#include "types.h"
#include "Simplelist.h"
#include <pthread.h>
#define FUNC_ERR -1
#define FUNC_OK 0
typedef void*(*thread_pool_callback_func)(void* arg);
struct thread_pool {
    int thread_num;             /**< �߳���*/
    
    pthread_mutex_t task_lock;  /**< ���������*/
    struct list_head tasks;     /**< �������*/
    int task_num;               /**< ��������е�������*/
    
    pthread_mutex_t curr_running_task_num_lock;      /**<  ���ڴ������������*/
    int curr_running_task_num;           /**< ���ڴ����������*/
    
    pthread_t* threads;     /**< �߳�ID*/
    int* started;          /**< */
    BOOL exit;              /**< �̳߳��˳���־*/

    pthread_mutex_t task_cond_lock;     /**< ��������������*/
    pthread_cond_t task_cond;           /**< ������������*/
};
struct thread_pool* thread_pool_create(int thread_num);
void thread_pool_release(struct thread_pool* pool);
int thread_pool_add_task(struct thread_pool* pool, thread_pool_callback_func cb, void* arg);
#endif

