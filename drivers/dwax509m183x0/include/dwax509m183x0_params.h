/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dwax509m183x0
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Jan Thies <jan.thies@haw-hamburg.de>
 */

#ifndef DWAX509M183X0_PARAMS_H
#define DWAX509M183X0_PARAMS_H

#include "board.h"
#include "dwax509m183x0.h"
#include "dwax509m183x0_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef DWAX509M183X0_PARAM_PARAM1
#define DWAX509M183X0_PARAM_PARAM1
#endif

#ifndef DWAX509M183X0_PARAMS
#define DWAX509M183X0_PARAMS
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const dwax509m183x0_params_t dwax509m183x0_params[] =
{
    DWAX509M183X0_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* DWAX509M183X0_PARAMS_H */
/** @} */
