#include <zephyr/kernel.h>
#include <zephyr/sys/sem.h>

#define STACKSIZE 1024

#define PRIORITY 7

void threadA(void *a, void *b, void *c);
void threadB(void *a, void *b, void *c);
void expiry_fn(struct k_timer *timer);
void stop_fn(struct k_timer *timer);

SYS_SEM_DEFINE(a_sem, 0, 1);
SYS_SEM_DEFINE(b_sem, 0, 1);

K_TIMER_DEFINE(timer, expiry_fn, stop_fn);

K_THREAD_DEFINE(thread_a, STACKSIZE, threadA, NULL, NULL, NULL,
		PRIORITY, 0, 0);

K_THREAD_DEFINE(thread_b, STACKSIZE, threadB, NULL, NULL, NULL,
		PRIORITY, 0, 0);

int i = 0;

void expiry_fn(struct k_timer *timer) {
	i++;
	sys_sem_give(&b_sem);
	i++;
	sys_sem_give(&a_sem);
	i++;
}


void stop_fn(struct k_timer *timer) {
}

void threadA(void *a, void *b, void *c) {
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);
	k_timer_init(&timer, expiry_fn, stop_fn); // overwrite
	k_timer_start(&timer, K_MSEC(100), K_MSEC(100));
	// k_timer_start(&timer, K_MSEC(100), K_NO_WAIT); // one-shot timer
	
	// k_timer_remaining_get(&timer);
	
	sys_sem_take(&a_sem, K_FOREVER);
	
	k_timer_stop(&timer);
}

void threadB(void *a, void *b, void *c) {
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);
	sys_sem_take(&b_sem, K_FOREVER);
}
