
#include <zephyr/kernel.h>

#define STACKSIZE 1024

#define PRIORITY 7

#define PIPE_SIZE 256

K_PIPE_DEFINE(work, PIPE_SIZE, 4);

void do_stuff(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);
        int items = 0;
        while(true){
            int item;
            size_t bytes_read;
            k_pipe_get(&work, &item, sizeof(int), &bytes_read, sizeof(int), K_FOREVER);
            items += item;
        }
}

K_THREAD_DEFINE(thread_a, STACKSIZE, do_stuff, NULL, NULL, NULL,
		PRIORITY, 0, 0);

void zephyr_dummy_syscall(){};

int main() {
    zephyr_dummy_syscall();

    int item = 0;
    while(true){
        size_t bytes_written;
        k_pipe_put(&work, &item, sizeof(int), &bytes_written, sizeof(int), K_FOREVER);
    }
    return 0;
}
