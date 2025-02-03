
#include <zephyr/kernel.h>

#define STACKSIZE (1 << 10)
#define MAX_WORK (1 << 8)
#define WORK_IRQ 0
#define WORK_IRQ_PRIORITY 1

K_STACK_DEFINE(work, MAX_WORK);

void produce_work_isr(const void* args) {
    printk("p");
    k_stack_push(&work, (stack_data_t)0);
}

int main(void) {
    IRQ_CONNECT(WORK_IRQ, WORK_IRQ_PRIORITY, produce_work_isr, NULL, 0);
    irq_enable(WORK_IRQ);
    return 0;
}

void do_consume(void* a, void* b, void* c) {
    while(true) {
        int w = 0;
        printk("c");
        k_stack_pop(&work, (stack_data_t*)w, K_FOREVER);
    }
}

K_THREAD_DEFINE(thread_a, STACKSIZE, do_consume, NULL, NULL, NULL, NULL, 0, 0);