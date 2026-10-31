#include <stdio.h>

#include "ztimer.h"
#include "lis331dlh.h"
#include "lis331dlh_params.h"

int main(void)
{
    lis331dlh_t dev;
    lis331dlh_data_t data;

    puts("LIS331DLH driver test");

    if (lis331dlh_init(&dev, &lis331dlh_params[0]) != 0) {
        puts("LIS331DLH init failed");
        return 1;
    }

    puts("LIS331DLH init OK");

    while (1) {
        if (lis331dlh_read_xyz(&dev, &data) == 0) {
            printf("X=%d Y=%d Z=%d\n", data.x, data.y, data.z);
        }
        else {
            puts("read failed");
        }

        ztimer_sleep(ZTIMER_MSEC, 500);
    }

    return 0;
}
