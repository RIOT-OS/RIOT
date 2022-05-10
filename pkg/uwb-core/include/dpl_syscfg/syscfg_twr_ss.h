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
 * @brief       uwb-core single-sided two-way ranging module configurations
 *              taken from decawave-uwb-core/lib/twr_ss/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_SYSCFG_SYSCFG_TWR_SS_H
#define DPL_SYSCFG_SYSCFG_TWR_SS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable ranging services
 */
#ifndef MYNEWT_VAL_TWR_SS_ENABLED
#define MYNEWT_VAL_TWR_SS_ENABLED (IS_USED(MODULE_UWB_CORE_TWR_SS))
#endif

/**
 * @brief TOA timeout delay for SS TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_SS_RX_TIMEOUT
#define MYNEWT_VAL_TWR_SS_RX_TIMEOUT (((uint16_t)0x30))
#endif

/**
 * @brief tx holdoff delay for SS TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_SS_TX_HOLDOFF
#define MYNEWT_VAL_TWR_SS_TX_HOLDOFF (((uint32_t)0x0300 + 0xA0 * IS_USED(MODULE_UWB_CORE_RNG_TRX_INFO)))
#endif

#ifdef __cplusplus
}
#endif

#endif /* DPL_SYSCFG_SYSCFG_TWR_SS_H */
