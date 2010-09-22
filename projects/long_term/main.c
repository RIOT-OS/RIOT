#include <stdio.h>
#include <stdint.h>
#include <hwtimer.h>
#include <swtimer.h>
#include <board.h>

int main(void)
{
    hwtimer_init();
    swtimer_init();
    uint32_t i = 0;

    while (1) {
        printf("(%10lu) Still running...\n", i++);
        LED_RED_TOGGLE;
        swtimer_usleep(10000);
    }
}
