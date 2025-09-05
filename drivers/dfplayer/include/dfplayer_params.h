/*
 * SPDX-FileCopyrightText: 2019 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_dfplayer
 * @{
 *
 * @file
 * @brief       Default configuration for the DFPlayer Mini driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "board.h"
#include "dfplayer_types.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for the DFPlayer Mini driver
 * @{
 */
#ifndef DFPLAYER_PARAM_UART
#define DFPLAYER_PARAM_UART     (UART_DEV(0))   /**< The UART device connected to the DFPlayer Mini */
#endif

#ifndef DFPLAYER_PARAM_BUSY_PIN
#define DFPLAYER_PARAM_BUSY_PIN (GPIO_UNDEF)    /**< The GPIO connected to the busy pin of the DFPlayer Mini */
#endif

#ifndef DFPLAYER_PARAM_VOLUME
#define DFPLAYER_PARAM_VOLUME   (15)            /**< The volume to set during initialization */
#endif

#ifndef DFPLAYER_PARAMS
/**
 * @brief   Default initialization parameters of the DFPlayer Mini driver
 */
#define DFPLAYER_PARAMS         {\
                                    .uart       = DFPLAYER_PARAM_UART, \
                                    .busy_pin   = DFPLAYER_PARAM_BUSY_PIN, \
                                    .volume     = DFPLAYER_PARAM_VOLUME, \
                                }
#endif
/**@}*/

/**
 * @brief   DFPlayer Mini configuration
 */
static const dfplayer_params_t dfplayer_params[] =
{
    DFPLAYER_PARAMS
};

/**
 * @brief   Number of DFPlayer descriptors present
 */
#define DFPLAYER_NUMOF          ARRAY_SIZE(dfplayer_params)

#ifdef __cplusplus
}
#endif

/** @} */
