#include <stdio.h>
#include <stdint.h>
#include <sht11.h>
#include <hwtimer.h>
#include <swtimer.h>
#include <board.h>

int main(void)
{
    sht11_val_t sht11_val;
    uint8_t success = 0;

    hwtimer_init();
    swtimer_init();
    sht11_init();
    while (1) {
        success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
        if (!success) {
            printf("Error reading SHT11\n");
        }
        else {
            printf("%-6.2f°C %5.2f%% (%5.2f%%)\n", sht11_val.temperature, sht11_val.relhum, sht11_val.relhum_temp);
        }
        LED_RED_TOGGLE;
        swtimer_usleep(1000 * 1000);
    }
}
