/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:31:44
 * @LastEditTime: 2020-05-28 17:22:53
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _PLATFORM_THREAD_H_
#define _PLATFORM_THREAD_H_

#include <pthread.h>

typedef struct platform_thread {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} platform_thread_t;

platform_thread_t *platform_thread_init( const char *name,
                                        void (*entry)(void *),
                                        void * const param,
                                        unsigned int stack_size,
                                        unsigned int priority,
                                        unsigned int tick);
void platform_thread_startup(platform_thread_t* thread);
void platform_thread_stop(platform_thread_t* thread);
void platform_thread_start(platform_thread_t* thread);
void platform_thread_destroy(platform_thread_t* thread);
void platform_thread_notice_enter(platform_thread_t* thread);
void platform_thread_notice_exit(platform_thread_t* thread);
void platform_thread_wait_exit(platform_thread_t* thread);

#endif
