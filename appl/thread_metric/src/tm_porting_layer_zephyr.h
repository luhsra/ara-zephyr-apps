/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef TM_PORTING_LAYER_ZEPHYR_H
#define TM_PORTING_LAYER_ZEPHYR_H

#include <zephyr/kernel.h>
#define TM_TEST_NUM_THREADS 10
#define TM_TEST_STACK_SIZE 1024
#define TM_TEST_NUM_SEMAPHORES 4
#define TM_TEST_NUM_MESSAGE_QUEUES 4
#define TM_TEST_NUM_SLABS 4

static struct k_thread test_thread_0;
static struct k_thread test_thread_1;
static struct k_thread test_thread_2;
static struct k_thread test_thread_3;
static struct k_thread test_thread_4;
static K_THREAD_STACK_ARRAY_DEFINE(test_stack, TM_TEST_NUM_THREADS,
                                   TM_TEST_STACK_SIZE);

static struct k_sem test_sem_0;
static struct k_sem test_sem_1;
static struct k_sem test_sem_2;
static struct k_sem test_sem_3;

static struct k_msgq test_msgq_0;
static struct k_msgq test_msgq_1;
static struct k_msgq test_msgq_2;
static struct k_msgq test_msgq_3;

static char test_msgq_buffer_0[8][16];
static char test_msgq_buffer_1[8][16];
static char test_msgq_buffer_2[8][16];
static char test_msgq_buffer_3[8][16];

static struct k_mem_slab test_slab_0;
static struct k_mem_slab test_slab_1;
static struct k_mem_slab test_slab_2;
static struct k_mem_slab test_slab_3;
static char __aligned(4) test_slab_buffer_0[8 * 128];
static char __aligned(4) test_slab_buffer_1[8 * 128];
static char __aligned(4) test_slab_buffer_2[8 * 128];
static char __aligned(4) test_slab_buffer_3[8 * 128];

/*
 * This function called from main performs basic RTOS initialization,
 * calls the test initialization function, and then starts the RTOS function.
 */
#define tm_initialize(test_initialization_function)                            \
  do {                                                                         \
    test_initialization_function();                                            \
  } while (0)

/*
 * This function takes a thread ID and priority and attempts to create the
 * file in the underlying RTOS.  Valid priorities range from 1 through 31,
 * where 1 is the highest priority and 31 is the lowest. If successful,
 * the function should return TM_SUCCESS. Otherwise, TM_ERROR should be
 * returned.
 */
#define tm_thread_create(thread_id, priority, entry_function)                  \
  do {                                                                         \
    k_tid_t tid;                                                               \
                                                                               \
    tid = k_thread_create(&test_thread_##thread_id, test_stack[thread_id],     \
                          TM_TEST_STACK_SIZE, entry_function, NULL, NULL,      \
                          NULL, priority, 0, K_NO_WAIT);                       \
                                                                               \
    /* Thread started in sleeping state. Switch to suspended state */          \
                                                                               \
    k_thread_suspend(&test_thread_##thread_id);                                \
    k_wakeup(&test_thread_##thread_id);                                        \
                                                                               \
    /*return (tid == &test_thread##thread_id) ? TM_SUCCESS : TM_ERROR;  */     \
  } while (0)

/*
 * This function resumes the specified thread.  If successful, the function
 * should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_thread_resume(thread_id)                                            \
  do {                                                                         \
    k_thread_resume(&test_thread_##thread_id);                                 \
                                                                               \
    /*return TM_SUCCESS;*/                                                     \
  } while (0)

/*
 * This function suspends the specified thread.  If successful, the function
 * should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_thread_suspend(thread_id)                                           \
  do {                                                                         \
    k_thread_suspend(&test_thread_##thread_id);                                \
    /*return */                                                                \
  } while (0)

/*
 * This function creates the specified queue.  If successful, the function
 * should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_queue_create(queue_id)                                              \
  do {                                                                         \
    k_msgq_init(&test_msgq_##queue_id, &test_msgq_buffer_##queue_id[0][0], 16, \
                8);                                                            \
                                                                               \
    /*return TM_SUCCESS;*/                                                     \
  } while (0)

/*
 * This function sends a 16-byte message to the specified queue.  If successful,
 * the function should return TM_SUCCESS. Otherwise, TM_ERROR should be
 * returned.
 */
#define tm_queue_send(queue_id, message_ptr)                                   \
  do {                                                                         \
    k_msgq_put(&test_msgq_##queue_id, message_ptr, K_FOREVER);                 \
  } while (0)

/*
 * This function receives a 16-byte message from the specified queue.  If
 * successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR should
 * be returned.
 */
#define tm_queue_receive(queue_id, message_ptr)                                \
  do {                                                                         \
    k_msgq_get(&test_msgq_##queue_id, message_ptr, K_FOREVER);                 \
  } while (0)

/*
 * This function creates the specified semaphore.  If successful, the function
 * should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_semaphore_create(semaphore_id)                                      \
  do {                                                                         \
    k_sem_init(&test_sem_##semaphore_id, 1, 1);                                \
  } while (0)

/*
 * This function gets the specified semaphore.  If successful, the function
 * should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_semaphore_get(semaphore_id)                                         \
  TM_SUCCESS;                                                                  \
  do {                                                                         \
    k_sem_take(&test_sem_##semaphore_id, K_FOREVER);                           \
    /*return */                                                                \
  } while (0)

/*
 * This function puts the specified semaphore.  If successful, the function
 * should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_semaphore_put(semaphore_id)                                         \
  TM_SUCCESS;                                                                  \
  do {                                                                         \
    k_sem_give(&test_sem_##semaphore_id);                                      \
    /*return TM_SUCCESS;*/                                                     \
  } while (0)

/*
 * This function creates the specified memory pool that can support one or more
 * allocations of 128 bytes.  If successful, the function should
 * return TM_SUCCESS. Otherwise, TM_ERROR should be returned.
 */
#define tm_memory_pool_create(pool_id)                                         \
  do {                                                                         \
    k_mem_slab_init(&test_slab_##pool_id, test_slab_buffer_##pool_id, 128, 8); \
  } while (0)

/*
 * This function allocates a 128 byte block from the specified memory pool.
 * If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR
 * should be returned.
 */
#define tm_memory_pool_allocate(pool_id, memory_ptr)                           \
  do {                                                                         \
    k_mem_slab_alloc(&test_slab_##pool_id, memory_ptr, K_NO_WAIT);             \
  } while (0)

/*
 * This function releases a previously allocated 128 byte block from the
 * specified memory pool. If successful, the function should return TM_SUCCESS.
 * Otherwise, TM_ERROR should be returned.
 */
#define tm_memory_pool_deallocate(pool_id, memory_ptr)                         \
  TM_SUCCESS;                                                                  \
  do {                                                                         \
    k_mem_slab_free(&test_slab_##pool_id, memory_ptr);                         \
  } while (0)

#endif
