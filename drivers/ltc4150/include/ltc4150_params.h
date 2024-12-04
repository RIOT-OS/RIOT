/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ltc4150
 *
 * @{
 * @file
 * @brief       Default configuration for LTC4150 coulomb counters
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef LTC4150_PARAMS_H
#define LTC4150_PARAMS_H

#include "board.h"
#include "ltc4150.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the LTC4150
 * @{
 */
#ifndef LTC4150_PARAM_INT
#define LTC4150_PARAM_INT             (GPIO_PIN(0, 4))
#endif
#ifndef LTC4150_PARAM_POL
#define LTC4150_PARAM_POL             (GPIO_UNDEF)
#endif
#ifndef LTC4150_PARAM_SHUTDOWN
#define LTC4150_PARAM_SHUTDOWN        (GPIO_PIN(0, 5))
#endif
#ifndef LTC4150_PARAM_PULSES
#define LTC4150_PARAM_PULSES          (45700U)
#endif
#ifndef LTC4150_PARAM_FLAGS
#define LTC4150_PARAM_FLAGS           LTC4150_EXT_PULL_UP
#endif
#ifndef LTC4150_PARAM_RECS
#define LTC4150_PARAM_RECS            NULL
#define LTC4150_PARAM_RECDATA         NULL
#endif
#ifndef LTC4150_PARAMS
#define LTC4150_PARAMS                { .interrupt = LTC4150_PARAM_INT,  \
                                        .polarity = LTC4150_PARAM_POL, \
                                        .shutdown = LTC4150_PARAM_SHUTDOWN, \
                                        .pulses_per_ah = LTC4150_PARAM_PULSES, \
                                        .flags = LTC4150_PARAM_FLAGS, \
                                        .recorders = LTC4150_PARAM_RECS, \
                                        .recorder_data = LTC4150_PARAM_RECDATA }
#endif
/**@}*/

/**
 * @name    Set default SAUL info text for the LTC4150
 * @{
 */
#ifndef LTC4150_SAULINFO
#define LTC4150_SAULINFO              { .name = "LTC4150 charge" }, \
                                      { .name = "LTC4150 average current" }
#endif

/**@}*/

/**
 * @brief   Configure LTC4150 devices
 */
static const ltc4150_params_t ltc4150_params[] =
{
    LTC4150_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t ltc4150_saul_info[] =
{
    LTC4150_SAULINFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LTC4150_PARAMS_H */
