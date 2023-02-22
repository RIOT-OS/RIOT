/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL wrapper for servo motors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <string.h>
#include <stdint.h>

#include "assert.h"
#include "phydat.h"
#include "saul.h"
#include "saul/periph.h"
#include "saul_reg.h"
#include "servo.h"

static int write(const void *dev, const phydat_t *state)
{
    servo_t *s = (void *)dev;
    int32_t num = state->val[0];
    switch (state->unit) {
    case UNIT_PERCENT:
        num *= 100;
        num >>= 8;
        break;
    case UNIT_PERMILL:
        num *= 1000;
        num >>= 8;
        break;
    case UNIT_BOOL:
        num = (num) ? 255 : 0;
        break;
    case UNIT_NONE:
    case UNIT_UNDEF:
        break;
    default:
        return -EINVAL;
    }

    if (num > UINT8_MAX) {
        num = UINT8_MAX;
    }

    if (num < 0) {
        num = 0;
    }

    servo_set(s, num);
    return 1;
}

const saul_driver_t servo_saul_driver = {
    .read = saul_read_notsup,
    .write = write,
    .type = SAUL_ACT_SERVO,
};
