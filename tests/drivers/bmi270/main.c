#if defined(CPU_CORE_CORTEX_M0PLUS)
int main(void) { return 0; }
#else

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmi270_r.h"
#include "bmi2.h"
#include "bmi2_defs.h"
#include "periph/i2c.h"
#include "ztimer.h"

static bmi270_t _dev;

int main(void)
{
    int rc;

    puts("BMI270 driver test");

    i2c_init(0);

    rc = bmi270_init_i2c(&_dev, 0, BMI2_I2C_PRIM_ADDR);
    if (rc != 0) {
        printf("bmi init failed: %d\n", rc);
        return 1;
    }

    rc = bmi270_config_default(&_dev);
    if (rc != 0) {
        printf("bmi cfg failed: %d\n", rc);
        return 1;
    }

    while (1) {
        bmi270_data_t data;

        rc = bmi270_read(&_dev, &data);
        if (rc == 0) {
            printf("acc z: %ld.%03ld g\n",
                   (long)(data.sensor_data.acc.z / 4096),
                   (long)(abs(data.sensor_data.acc.z * 1000 / 4096) % 1000));
        }
        else {
            printf("bmi read failed: %d\n", rc);
        }

        ztimer_sleep(ZTIMER_USEC, 50000U);
    }
}

#endif
