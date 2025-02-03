
#include <zephyr/kernel.h>

#define STACKSIZE 1024

#define PRIORITY 1

#define PIPE_SIZE 4

/*
 * High priority thread tries to get from a pipe
 * If pipe is empty then low-priority thread will be scheduled and puts to the queue
 */

K_PIPE_DEFINE(work, PIPE_SIZE, 4);

void do_put(void* a, void* b, void* c) {
    while(true) {
        printk("p");
        int item = 0;
        size_t bytes_written;
        k_pipe_put(&work, &item, sizeof(int), &bytes_written, sizeof(int), K_NO_WAIT);
    }
}

void do_get(void* a, void* b, void* c) {
    while(true) {
        printk("g");
        int item;
        size_t bytes_read;
        k_pipe_get(&work, &item, sizeof(int), &bytes_read, sizeof(int), K_FOREVER);
    }
}

K_THREAD_DEFINE(thread_a, STACKSIZE, do_put, NULL, NULL, NULL,
		PRIORITY+4, 0, 0);


K_THREAD_DEFINE(thread_b, STACKSIZE, do_get, NULL, NULL, NULL,
		PRIORITY, 0, 0);