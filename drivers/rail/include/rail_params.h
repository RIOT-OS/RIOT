/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rail
 * @{
 * @file
 * @brief       Default configuration for the Silabs EFR32 radio driver
 *
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 */

#ifndef RAIL_PARAMS_H
#define RAIL_PARAMS_H

#include "board.h"
#include "rail_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT86RF2xx driver
 * @{
 */
#ifndef RAIL_PARAMS
#define RAIL_PARAMS { .freq = RAIL_TRANSCEIVER_FREQUENCY_2P4GHZ }
#endif
/**@}*/

/**
 * @brief   EFR32 rail configuration
 */
static const rail_params_t rail_params[] = {
    RAIL_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* RAIL_PARAMS_H */
/** @} */
