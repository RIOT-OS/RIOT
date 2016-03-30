/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dht
 * @{
 *
 * @file
 * @brief       SAUL adaption for DHT devices
 *
 * The values exported to SAUL for DHT devices are buffered, meaning that new
 * values will only be read from the device, if the buffered values are older
 * then 1 second (being exactly the maximum sampling time for DHT devices).
 *
 * This buffering does further introduce a coupling between both SAUL endpoints,
 * meaning that if you read from both endpoints after each other, both values
 * are from the same sensor reading.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "dht.h"
#include "xtimer.h"

#define DHT_SAUL_HOLD_TIME      (1000 * 1000U)  /* 1s */

static int16_t temp, hum;
static uint32_t last = 0;

static int check_and_read(void *dev, phydat_t *res, int16_t *val, uint8_t unit)
{
    dht_t *d = (dht_t *)dev;
    uint32_t now = xtimer_now();

    if ((now - last) > DHT_SAUL_HOLD_TIME) {
        dht_read(d, &temp, &hum);
        last = now;
    }
    else {
        (void)d;
    }

    res->val[0] = *val;
    memset(&res->val[1], 0, 2 * sizeof(int16_t));
    res->unit = unit;
    res->scale = -1;
    return 1;
}

static int read_temp(void *dev, phydat_t *res)
{
    return check_and_read(dev, res, &temp, UNIT_TEMP_C);
}

static int read_hum(void *dev, phydat_t *res)
{
    return check_and_read(dev, res, &hum, UNIT_PERCENT);
}

const saul_driver_t dht_temp_saul_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t dht_hum_saul_driver = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM
};
