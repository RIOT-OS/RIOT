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

#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"
#include "qma6100p.h"
#include "qma6100p_params.h"
#include "ztimer.h"

#define SLEEP_S (5U)

static qma6100p_t dev;

int main(void)
{
#ifdef T1000E_3V3_ACC_EN_PIN
    gpio_init(T1000E_3V3_ACC_EN_PIN, GPIO_OUT);
    gpio_set(T1000E_3V3_ACC_EN_PIN);
    ztimer_sleep(ZTIMER_MSEC, 10);
#endif

    ztimer_sleep(ZTIMER_SEC, SLEEP_S);
    puts("QMA6100P accelerometer driver test\n");
    printf("Initializing QMA6100P at I2C_DEV(%d)... ",
           (int)qma6100p_params[0].i2c);

    if (qma6100p_init(&dev, &qma6100p_params[0]) == QMA6100P_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[FAILED]");
        return -1;
    }
    return 0;
}
