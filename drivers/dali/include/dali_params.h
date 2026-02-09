/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dali
 * @{
 *
 * @file
 * @brief       DALI driver configuration
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef DALI_PARAMS_H
#define DALI_PARAMS_H

#include "dali.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DALI_PARAM_RX
#define DALI_PARAM_RX          GPIO_PIN(0, 5)
#endif
#ifndef DALI_PARAM_TX
#define DALI_PARAM_TX          GPIO_PIN(0, 6)
#endif

#ifndef DALI_PARAMS
#define DALI_PARAMS     { .rx_pin = DALI_PARAM_RX,    \
                          .tx_pin = DALI_PARAM_TX,    \
                        }
#endif

/**
 * @brief   DALI port descriptor array
 */
static const dali_conf_t dali_config[] = {
    DALI_PARAMS,
};

#define DALI_NUMOF             ARRAY_SIZE(dali_config)

#ifdef __cplusplus
}
#endif

#endif /* DALI_PARAMS_H */
/** @} */
