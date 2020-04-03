/*
 * Copyright (C) 2017 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pulse_counter
 * @{
 *
 * @file
 * @brief       PULSE_COUNTER adaption to the RIOT actuator/sensor interface
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "pulse_counter.h"

static int read_pulse_counter(const void *dev, phydat_t *res)
{
    /* Using non-const dev !! */
    pulse_counter_t *mydev = (pulse_counter_t *)dev;
    res->val[0] = pulse_counter_read_with_reset(mydev);
    res->unit  = UNIT_NONE;
    res->scale = 0;
    return 1;
}

static int write_pulse_counter(const void *dev, phydat_t *data)
{
    /* Using non-const dev !! */
    pulse_counter_t *mydev = (pulse_counter_t *)dev;
    pulse_counter_reset(mydev);
    (void) data;
    return 1;
}

const saul_driver_t pulse_counter_saul_driver = {
    .read  = read_pulse_counter,
    .write = write_pulse_counter,
    .type  = SAUL_SENSE_COUNT,
};
