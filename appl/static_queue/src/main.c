
#include <zephyr/kernel.h>
#include <stdio.h>

#define STACKSIZE (1 << 10)
#define PRIORITY 1 // preemptive scheduling

struct work_item {
    void* reserved; // Reserved for queue management
    int data;
};

/*
 * High priority thread tries to get from a queue
 * If queue is empty then low-priority thread will be scheduled and puts to the queue
 */

K_LIFO_DEFINE(work);

void do_put(void* a, void* b, void* c) {
    while(true) {
        struct work_item item = {NULL, 0};
        printk("p");
        k_lifo_put(&work, &item);
    }
}

void do_get(void* a, void* b, void* c) {
    while(true) {
        int w = 0;
        printk("g");
        struct work_item* item = k_lifo_get(&work, K_FOREVER);
    }
}

K_THREAD_DEFINE(thread_a, STACKSIZE, do_put, NULL, NULL, NULL,
		PRIORITY+4, 0, 0);


K_THREAD_DEFINE(thread_b, STACKSIZE, do_get, NULL, NULL, NULL,
		PRIORITY, 0, 0);
