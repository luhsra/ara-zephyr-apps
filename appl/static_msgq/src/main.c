#include <zephyr/kernel.h>
#include <stdio.h>

#define STACKSIZE (1 << 10)
#define MAX_WORK (1 << 2)
#define PRIORITY 1

/*
 * High priority thread tries to get from a msgq
 * If msgq is empty then low-priority thread will be scheduled and puts to the msgq
 */

K_MSGQ_DEFINE(work, sizeof(int), MAX_WORK, 4);

void do_put(void* a, void* b, void* c) {
    int done = 0;
    while(true) {
        int w = 0;
        printk("p");
        k_msgq_put(&work, &w, K_NO_WAIT);
    }
}

void do_get(void* a, void* b, void* c) {
    int done = 0;
    while(true) {
        int w = 0;
        printk("g");
        k_msgq_get(&work, &w, K_FOREVER);
    }
}

K_THREAD_DEFINE(thread_a, STACKSIZE, do_put, NULL, NULL, NULL,
		PRIORITY+4, 0, 0);


K_THREAD_DEFINE(thread_b, STACKSIZE, do_get, NULL, NULL, NULL,
		PRIORITY, 0, 0);
