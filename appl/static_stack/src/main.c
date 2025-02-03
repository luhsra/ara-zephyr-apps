
#include <zephyr/kernel.h>

#define STACKSIZE (1 << 10)
#define MAX_WORK (1 << 8)
#define PRIORITY 1

K_STACK_DEFINE(work, MAX_WORK);

void do_put(void* a, void* b, void* c) {
    while(true) {
        printk("p");
        k_stack_push(&work, (stack_data_t)0);
    }
}

void do_get(void* a, void* b, void* c) {
    while(true) {
        int w = 0;
        printk("g");
        k_stack_pop(&work, (stack_data_t*)w, K_NO_WAIT);
    }
}

K_THREAD_DEFINE(thread_a, STACKSIZE, do_put, NULL, NULL, NULL,
		PRIORITY+4, 0, 0);


K_THREAD_DEFINE(thread_b, STACKSIZE, do_get, NULL, NULL, NULL,
		PRIORITY, 0, 0);
