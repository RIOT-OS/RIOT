/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the QMA6100P accelerometer driver
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "qma6100p.h"
#include "qma6100p_params.h"
#include "ztimer.h"

#define SLEEP_S (5U)

static qma6100p_t dev;

int main(void)
{
    qma6100p_data_t data;
    int res;

    ztimer_sleep(ZTIMER_SEC, SLEEP_S);
    puts("QMA6100P accelerometer driver test\n");
    printf("Initializing QMA6100P at I2C_DEV(%d)... ",
           (int)qma6100p_params[0].i2c);

    res = qma6100p_init(&dev, &qma6100p_params[0]);
    if (res < 0) {
        printf("Init FAILED: %d \n", res);
        return 1;
    }
    puts("[OK]\n");

    for (;;) {
        res = qma6100p_read(&dev, &data);
        if (res == QMA6100P_DATA_READY) {
            printf("X: %" PRId32 " ug, Y: %" PRId32 " ug, Z: %" PRId32 " ug\n",
                   data.x, data.y, data.z);
        }
        else if (res == QMA6100P_NO_NEW_DATA) {
            printf("No new data\n");
        }
        ztimer_sleep(ZTIMER_MSEC, 100);
    }
    return 0;
}
