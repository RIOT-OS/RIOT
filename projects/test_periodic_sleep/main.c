#include <stdio.h>
#include <hwtimer.h>
#include <swtimer.h>

#define PERIOD (1000000L)

int main(void)
{
    hwtimer_init();
    swtimer_init();
    puts("Software timer periodic wakeup test.\n");

    int64_t diff = 0;
    int64_t before = 0;
    int64_t new_period = 0;

    while(1) {
        before = swtimer_now();
        new_period = PERIOD - diff ;
        printf("%lli\n", new_period);
        swtimer_usleep(new_period);
        printf("%llu \t %lli\n", swtimer_now(), diff);
        diff = (swtimer_now() - before) - PERIOD + diff + 2;
    }
}

