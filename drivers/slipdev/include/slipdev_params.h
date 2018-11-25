/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SLIPDEV_PARAMS_H
#define SLIPDEV_PARAMS_H

#include "board.h"
#include "slipdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for slipdev driver
 * @{
 */
#ifndef SLIPDEV_PARAM_UART
#define SLIPDEV_PARAM_UART      (UART_DEV(0))
#endif
#ifndef SLIPDEV_PARAM_BAUDRATE
#define SLIPDEV_PARAM_BAUDRATE  (115200U)
#endif

#ifndef SLIPDEV_PARAMS
#define SLIPDEV_PARAMS          { .uart = SLIPDEV_PARAM_UART,   \
                                  .baudrate = SLIPDEV_PARAM_BAUDRATE }
#endif
/** @} */

/**
 * @brief   slipdev configuration
 */
static const slipdev_params_t slipdev_params[] = {
    SLIPDEV_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* SLIPDEV_PARAMS_H */
/** @} */
