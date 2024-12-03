#include <zephyr/kernel.h>
#include <string.h>

struct k_mutex m;
struct k_timer t;

void threadA() {}

K_THREAD_DEFINE(thread_a, 1024, threadA, NULL, NULL, NULL,
		0, 0, 0);

K_MSGQ_DEFINE(q_name, 16, 16, 4);
char buffer[16*16];

struct k_work_delayable sampling_work;
static void soc_debug_sampling_work_handler(struct k_work *work) {}

int main() {
	k_mutex_init(&m);
    k_thread_join(thread_a, K_FOREVER);
	char x[5];
	strncpy(x, x, 4);
	k_timer_init(&t, NULL, NULL);
	k_msgq_init(&q_name, buffer, 16, 16);
	k_work_init_delayable(&sampling_work, soc_debug_sampling_work_handler);
	k_work_schedule(&sampling_work, K_MSEC(100));
	return 0;
}
