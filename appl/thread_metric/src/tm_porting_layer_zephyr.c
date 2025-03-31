/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2024 Intel Corporation
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** Thread-Metric Component                                               */
/**                                                                       */
/**   Porting Layer (Must be completed with RTOS specifics)               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary files.  */

#include "tm_api.h"

#include <zephyr/kernel.h>

#define TM_TEST_NUM_THREADS        10
#define TM_TEST_STACK_SIZE         1024
#define TM_TEST_NUM_SEMAPHORES     4
#define TM_TEST_NUM_MESSAGE_QUEUES 4
#define TM_TEST_NUM_SLABS          4

#if (CONFIG_MP_MAX_NUM_CPUS > 1)
#error "*** Tests are only designed for single processor systems! ***"
#endif

//static struct k_thread test_thread[TM_TEST_NUM_THREADS];
//static K_THREAD_STACK_ARRAY_DEFINE(test_stack, TM_TEST_NUM_THREADS, TM_TEST_STACK_SIZE);

//static struct k_sem test_sem[TM_TEST_NUM_SEMAPHORES];

//static struct k_msgq test_msgq[TM_TEST_NUM_MESSAGE_QUEUES];
//static char test_msgq_buffer[TM_TEST_NUM_MESSAGE_QUEUES][8][16];

//static struct k_mem_slab test_slab[TM_TEST_NUM_SLABS];
//static char __aligned(4) test_slab_buffer[TM_TEST_NUM_SLABS][8 * 128];


/*
 * This function relinquishes to other ready threads at the same
 * priority.
 */
void tm_thread_relinquish(void)
{
	k_yield();
}

/*
 * This function suspends the specified thread for the specified number
 * of seconds.
 */
void tm_thread_sleep(int seconds)
{
	k_sleep(K_SECONDS(seconds));
}

#if defined(CONFIG_TM_INTERRUPT) || defined(CONFIG_TM_INTERRUPT_PREEMPTION)
/* This function is defined by the benchmark. */
extern void tm_interrupt_handler(const void *);

void tm_cause_interrupt(void)
{
	IRQ_CONNECT(42, 1, tm_interrupt_handler, NULL, 0);
	irq_enable(42);
	irq_offload(tm_interrupt_handler, NULL);
	irq_disable(42);
}
#endif
