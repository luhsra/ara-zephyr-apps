#include <zephyr/kernel.h>
#include <stdio.h>
K_MUTEX_DEFINE(guard);

void thread_L0(void* a, void* b, void* c);
void thread_H0(void* a, void* b, void* c);
void thread_H1(void* a, void* b, void* c);

K_THREAD_DEFINE(tcb_L0,  128, thread_L0, NULL, NULL, NULL, /*prio:*/ 7, 0, 0);
K_THREAD_DEFINE(tcb_H0,  128, thread_H0, NULL, NULL, NULL, /*prio:*/ 1, 0, -1);
K_THREAD_DEFINE(tcb_H1,  128, thread_H1, NULL, NULL, NULL, /*prio:*/ 1, 0, -1);

void thread_H0(void* a, void* b, void* c) {
    k_yield(); // let H1 run
    printf("%s mutex will lock\n", __FUNCTION__);
    k_mutex_lock(&guard, K_FOREVER);
    printf("%s mutex locked\n", __FUNCTION__);
    k_mutex_unlock(&guard);
    printf("%s mutex unlocked\n", __FUNCTION__);
}

void thread_H1(void* a, void* b, void* c) {
    printf("%s running\n", __FUNCTION__);
    if(k_mutex_unlock(&guard)) {
        printf("%s failed to unlock mutex\n", __FUNCTION__);
    }
    int i = 6;
    while(--i) {
        printf("%s yield\n", __FUNCTION__);
        k_yield();
    }
    printf("%s returning\n", __FUNCTION__);
}

void thread_L0(void* a, void* b, void* c) {
    if(k_mutex_lock(&guard, K_NO_WAIT) != 0) {
        k_fatal_halt(__LINE__);
    }
    printf("%s mutex locked P:%d\n", __FUNCTION__,
           k_thread_priority_get(tcb_L0));
    k_sched_lock();
    k_thread_start(tcb_H0);
    k_thread_start(tcb_H1);
    k_sched_unlock();
    // at this point we reschedule, with a locked mutex, triggering priority
    // inversion for this thread
    // H0 becomes active but gets suspended due to the locked mutex.
    // H1 is needed to show that L0 is scheduled regardless of a higher
    // priority thread H1 beeing ready
    printf("%s mutex will unlock P:%d\n", __FUNCTION__,
           k_thread_priority_get(tcb_L0));
    k_mutex_unlock(&guard);
    printf("%s returning P:%d\n", __FUNCTION__,
           k_thread_priority_get(tcb_L0));
}
