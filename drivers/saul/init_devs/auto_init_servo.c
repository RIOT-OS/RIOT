/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_saul
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for servo motors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <string.h>
#include <stdint.h>

#include "assert.h"
#include "kernel_defines.h"
#include "log.h"
#include "phydat.h"
#include "saul.h"
#include "saul/periph.h"
#include "saul_reg.h"
#include "servo.h"
#include "servo_params.h"

#define SERVO_NUMOF ARRAY_SIZE(servo_params)

static servo_t servos[SERVO_NUMOF];
static saul_reg_t saul_entries[SERVO_NUMOF];

void auto_init_servo(void)
{
    for (unsigned i = 0; i < SERVO_NUMOF; i++) {
        LOG_DEBUG("[servo] auto-init servo #%u\n", i);
        int retval = servo_init(&servos[i], &servo_params[i]);
        if (retval != 0) {
            LOG_WARNING("[servo] auto-init of servo #%u failed: %d\n",
                        i, retval);
            continue;
        }
        saul_reg_t *e = &saul_entries[i];

        e->dev = &servos[i];
        e->name = servo_saul_info[i].name;
        e->driver = &servo_saul_driver;

        saul_reg_add(e);
    }
}
