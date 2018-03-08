/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Default configuration for DHT devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DHT_PARAMS_H
#define DHT_PARAMS_H

#include "board.h"
#include "dht.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DHT devices
 * @{
 */
#ifndef DHT_PARAM_PIN
#define DHT_PARAM_PIN               (GPIO_PIN(0, 0))
#endif
#ifndef DHT_PARAM_TYPE
#define DHT_PARAM_TYPE              (DHT11)
#endif
#ifndef DHT_PARAM_PULL
#define DHT_PARAM_PULL              (GPIO_IN_PU)
#endif
#ifndef DHT_PARAMS
#define DHT_PARAMS                  { .pin     = DHT_PARAM_PIN,  \
                                      .type    = DHT_PARAM_TYPE, \
                                      .in_mode = DHT_PARAM_PULL }
#endif
#ifndef DHT_SAULINFO
#define DHT_SAULINFO                { .name = "dht" }
#endif
/**@}*/

/**
 * @brief   Configure DHT devices
 */
static const dht_params_t dht_params[] =
{
    DHT_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t dht_saul_info[] =
{
    DHT_SAULINFO
};

#ifdef __cplusplus
}
#endif

#endif /* DHT_PARAMS_H */
/** @} */
