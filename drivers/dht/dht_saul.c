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

static int read_temp(const void *dev, phydat_t *res)
{
    if (dht_read((dht_t *)dev, &res->val[0], NULL)) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -1;
    return 1;
}

static int read_hum(const void *dev, phydat_t *res)
{
    if (dht_read((dht_t *)dev, NULL, &res->val[0])) {
        return -ECANCELED;
    }
    res->unit = UNIT_PERCENT;
    res->scale = -1;
    return 1;
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
