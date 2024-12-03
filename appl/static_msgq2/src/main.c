#include <zephyr/kernel.h>
#include <stdio.h>

#define STACKSIZE (1 << 10)
#define MAX_WORK (1 << 4)
#define PRIORITY 0

K_THREAD_STACK_DEFINE(worker_stack_area, STACKSIZE);
struct k_thread worker;

K_MSGQ_DEFINE(work, sizeof(int), MAX_WORK, 4);

void do_work(void* a, void* b, void* c) {
    int done = 0;
    while(true) {
        int w = 0;
        printf("do_work: k_msgq_get()\n");
        int ret = k_msgq_get(&work, &w, K_NO_WAIT);
        if(ret != 0) {
            printf("do_work: k_yield()\n");
            k_yield();
        }
        done += w;
    }
}

int main(void) {
    k_tid_t workerId = k_thread_create(&worker, worker_stack_area,
        STACKSIZE, do_work, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    int item = 0;
    while(true) {
        printf("main: k_msgq_put()\n");
        int ret = k_msgq_put(&work, &item, K_FOREVER);
    }
    return 0;
}

