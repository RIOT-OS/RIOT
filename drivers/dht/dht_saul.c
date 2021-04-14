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
#include "dht_saul.h"

#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
#include "saul_observer.h"

static int check_temp(const void *dev) {
    dht_saul_t * saul_dev = (dht_saul_t*) dev;
    int16_t cur_val, diff;

    /* get current value */
    if (dht_read(&saul_dev->sensor, &cur_val, NULL)) {
        return 0;
    }

    /* get absolut diff to last reading */
    diff = cur_val - saul_dev->last_event_val_temp;
    if (diff < 0) {
        diff *= -1;
    }

    /* ignore if change is under threshold */
    if (diff < CONFIG_DHT_EVENT_THRESHOLD_TEMP) {
        return 0;
    }

    /* change is significant */
    saul_dev->last_event_val_temp = cur_val;
    return 1;
}

static int check_hum(const void *dev) {
    dht_saul_t * saul_dev = (dht_saul_t*) dev;
    int16_t cur_val, diff;

    /* get current value */
    if (dht_read(&saul_dev->sensor, NULL, &cur_val)) {
        return 0;
    }

    /* get absolut diff to last reading */
    diff = cur_val - saul_dev->last_event_val_hum;
    if (diff < 0) {
        diff *= -1;
    }

    /* ignore if change is under threshold */
    if (diff < CONFIG_DHT_EVENT_THRESHOLD_HUM) {
        return 0;
    }

    /* change is significant */
    saul_dev->last_event_val_hum = cur_val;
    return 1;
}
#endif /* IS_ACTIVE(MODULE_SAUL_OBSERVER) */

static int read_temp(const void *dev, phydat_t *res)
{
    dht_saul_t * saul_dev = (dht_saul_t*) dev;
    if (dht_read(&saul_dev->sensor, &res->val[0], NULL)) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -1;

    return 1 | SAUL_FLAG_SCHEDULE_CHECK;
}

static int read_hum(const void *dev, phydat_t *res)
{
    dht_saul_t * saul_dev = (dht_saul_t*) dev;
    if (dht_read(&saul_dev->sensor, NULL, &res->val[0])) {
        return -ECANCELED;
    }
    res->unit = UNIT_PERCENT;
    res->scale = -1;

    return 1 | SAUL_FLAG_SCHEDULE_CHECK;
}

const saul_driver_t dht_temp_saul_driver = {
    .read = read_temp,
    .write = saul_notsup,
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
    .check = check_temp,
#endif
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t dht_hum_saul_driver = {
    .read = read_hum,
    .write = saul_notsup,
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
    .check = check_hum,
#endif
    .type = SAUL_SENSE_HUM
};
