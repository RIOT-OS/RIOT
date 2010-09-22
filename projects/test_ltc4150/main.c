#include <stdio.h>
#include <ltc4150.h>
#include <swtimer.h>

void busy_wait(unsigned int n) {
    for (volatile unsigned int i = 0; i < n; i++) {}
}

int main(void)
{
    ltc4150_start();
    while(1) {            
            printf("Power usage: %.4f mA (%.4f mA avg)\n", ltc4150_get_current_mA(), ltc4150_get_avg_mA());
            swtimer_usleep(100000);
	}

	return 0;
}
