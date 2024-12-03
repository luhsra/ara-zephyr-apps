#include <zephyr/kernel.h>

#define STACKSIZE 1024

#define PRIORITY 7

void threadA(void *, void *, void *);
void threadB(void *, void *, void *);

K_THREAD_DEFINE(thread_a, STACKSIZE, threadA, NULL, NULL, NULL,
		PRIORITY, 0, 0);


K_THREAD_DEFINE(thread_b, STACKSIZE, threadB, NULL, NULL, NULL,
		PRIORITY + 4, 0, 0);

void threadA(void *dummy1, void *dummy2, void *dummy3) {
    k_thread_abort(k_current_get());
}

void threadB(void *dummy1, void *dummy2, void *dummy3) {
    k_thread_join(thread_a, K_FOREVER);
	k_current_get();
}
