
#include <zephyr/kernel.h>
#include "stuff.h"

int main() {
    do_stuff();
    k_msleep(900);
    return 0;
}
