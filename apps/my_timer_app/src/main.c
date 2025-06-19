#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

struct k_timer my_timer;

void timer_handler(struct k_timer *timer_id)
{
    printk("Timer expired\n");
}

int  main(void)
{
    k_timer_init(&my_timer, timer_handler, NULL);
    k_timer_start(&my_timer, K_MSEC(1000), K_MSEC(1000));

    while (1) {
        k_msleep(5000);
    }
 return(0);
}
