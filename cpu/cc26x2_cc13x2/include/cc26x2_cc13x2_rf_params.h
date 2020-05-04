/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RF_PARAMS_H
#define CC26X2_CC13X2_RF_PARAMS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CC26X2_CC13X2_RF_PA_ENTRY(bias, gain, boost, coefficient) \
    ((bias) << 0) | ((gain) << 6) | ((boost) << 8) | ((coefficient) << 9)

/**
 * @brief   RF PA table entry
 */
typedef struct {
    int16_t dbm;  /**< PA entry value in dBm */
    uint16_t val; /**< Coded value for the PA entry */
} cc26x2_cc13x2_rf_pa_t;

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_PARAMS_H */
/** @} */
