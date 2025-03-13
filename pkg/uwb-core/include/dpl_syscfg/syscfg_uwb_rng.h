/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core uwb_rng module configurations
 *              taken from decawave-uwb-core/lib/uwb_rng/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_SYSCFG_SYSCFG_UWB_RNG_H
#define DPL_SYSCFG_SYSCFG_UWB_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief uwb-core uwb_rng module is enabled
 */
#ifndef MYNEWT_VAL_UWB_RNG_ENABLED
#define MYNEWT_VAL_UWB_RNG_ENABLED ((IS_USED(MODULE_UWB_CORE_RNG)))
#endif

/**
 * @brief TOA timeout delay for TWR (usec)
 */
#ifndef MYNEWT_VAL_RNG_RX_TIMEOUT
#define MYNEWT_VAL_RNG_RX_TIMEOUT (((uint16_t)0x20))
#endif

/**
 * @brief worst case tx holdoff delay for all TWR modes (usec)
 */
#ifndef MYNEWT_VAL_RNG_TX_HOLDOFF
#define MYNEWT_VAL_RNG_TX_HOLDOFF (((uint32_t)0x0320 + 0xA0 * IS_USED(MODULE_UWB_CORE_RNG_TRX_INFO)))
#endif

/**
 * @brief Show debug output from postprocess
 */
#ifndef MYNEWT_VAL_RNG_VERBOSE
#define MYNEWT_VAL_RNG_VERBOSE (0)
#endif

/**
 * @brief JSON buffer size
 */
#ifndef MYNEWT_VAL_UWB_RNG_JSON_BUFSIZE
#define MYNEWT_VAL_UWB_RNG_JSON_BUFSIZE (256)
#endif

#ifdef __cplusplus
}
#endif

#endif /* DPL_SYSCFG_SYSCFG_UWB_RNG_H */
