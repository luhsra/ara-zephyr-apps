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

static struct k_thread test_thread_0;
static struct k_thread test_thread_1;
static struct k_thread test_thread_2;
static struct k_thread test_thread_3;
static struct k_thread test_thread_4;
static K_THREAD_STACK_ARRAY_DEFINE(test_stack, TM_TEST_NUM_THREADS,
                                   TM_TEST_STACK_SIZE);

#define tm_thread_create(thread_id, priority, entry_function)                  \
  do {                                                                         \
    k_tid_t tid;                                                               \
                                                                               \
    tid = k_thread_create(&test_thread_##thread_id, test_stack[thread_id],     \
                          TM_TEST_STACK_SIZE, entry_function, NULL, NULL,      \
                          NULL, priority, 0, K_FOREVER);                       \
                                                                               \
    /* Thread started in sleeping state. Switch to suspended state */          \
                                                                               \
    k_thread_suspend(&test_thread_##thread_id);                                \
    k_wakeup(&test_thread_##thread_id);                                        \
                                                                               \
    /*return (tid == &test_thread##thread_id) ? TM_SUCCESS : TM_ERROR;  */     \
  } while (0)

#define tm_thread_resume(thread_id)                                            \
  do {                                                                         \
    k_thread_resume(&test_thread_##thread_id);                                 \
                                                                               \
    /*return TM_SUCCESS;*/                                                     \
  } while (0)

#define tm_thread_suspend(thread_id)                                           \
  do {                                                                         \
    k_thread_suspend(&test_thread_##thread_id);                                \
                                                                               \
    /*return TM_SUCCESS;*/                                                     \
  } while (0)

#define tm_initialize(test_initialization_function)                            \
  do {                                                                         \
    test_initialization_function();                                            \
  } while (0)

#endif
