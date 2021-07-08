/*
 * Copyright (C) 2021 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dht
 *
 * @{
 * @file
 * @brief       SAUL context
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef DHT_SAUL_H
#define DHT_SAUL_H

#include "saul.h"
#include "saul_reg.h"
#include "dht.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum temperature diff for a change to be significant
 */
#ifndef CONFIG_DHT_EVENT_THRESHOLD_TEMP
#define CONFIG_DHT_EVENT_THRESHOLD_TEMP 10 /* x 10^-1K */
#endif

/**
 * @brief   Minimum humidity diff for a change to be significant
 */
#ifndef CONFIG_DHT_EVENT_THRESHOLD_HUM
#define CONFIG_DHT_EVENT_THRESHOLD_HUM  50 /* x 10^-1% */
#endif

/**
 * @brief   Polling interval for reading the sensor; set to 0 for disabling polling
 */
#ifndef CONFIG_DHT_POLLING_INTERVAL
#define CONFIG_DHT_POLLING_INTERVAL   1000 /* ms */
#endif

/**
 * @brief   Driver context for DHT SAUL
 */
typedef struct {
    dht_t sensor;                     /**< Sensor driver handle */
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
    int16_t last_event_val_temp;      /**< Last event's temperature */
    int16_t last_event_val_hum;       /**< Last event's humidity */
# if CONFIG_DHT_POLLING_INTERVAL > 0
    ztimer_t polling;                 /**< Polling timer handle */
# endif /* CONFIG_DHT_POLLING_INTERVAL > 0 */
#endif /* IS_ACTIVE(MODULE_SAUL_OBSERVER) */
} dht_saul_t;

/**
 * @brief   SAUL endpoint for temperature
 */
extern const saul_driver_t dht_temp_saul_driver;

/**
 * @brief   SAUL endpoint for humidity
 */
extern const saul_driver_t dht_hum_saul_driver;

#ifdef __cplusplus
}
#endif
#endif /* DHT_SAUL_H */
/** @} */