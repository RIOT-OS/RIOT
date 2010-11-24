#include <stdio.h>
#include <sht11.h>
#include <board.h>
#include <swtimer.h>
#include <ltc4150.h>

int main(void)
{
    sht11_val_t sht11_val;
    uint8_t success = 0;
    
    puts("");
    puts("LaSeR: Longterm Sensor Reader initialized.");
    puts("Printing \"temperature in °C;relative humidity;temperature compensated relative humidity\".");
    puts("");
    
    ltc4150_start();

    while (1) {
        success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
        if (!success) {
            printf("error;error;error\n");
        }
        else {
            printf("%.2f;%.2f;%.2f\n", sht11_val.temperature, sht11_val.relhum, sht11_val.relhum_temp);
        }
        LED_RED_TOGGLE;
        swtimer_usleep(60 * 1000*1000);
    }
}
