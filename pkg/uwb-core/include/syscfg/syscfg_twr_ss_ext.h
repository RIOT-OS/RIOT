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
 *              taken from decawave-uwb-core/lib/twr_ss_ext/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef SYSCFG_SYSCFG_TWR_SS_EXT_H
#define SYSCFG_SYSCFG_TWR_SS_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable Single Sided extended ranging services
 */
#ifndef MYNEWT_VAL_TWR_SS_EXT_ENABLED
#define MYNEWT_VAL_TWR_SS_EXT_ENABLED (IS_ACTIVE(MODULE_UWB_CORE_TWR_SS_EXT))
#endif

/**
 * @brief TOA timeout delay for SS EXT TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_SS_EXT_RX_TIMEOUT
#define MYNEWT_VAL_TWR_SS_EXT_RX_TIMEOUT (((uint16_t)0x40))
#endif

/**
 * @brief tx holdoff delay for SS EXT TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_SS_EXT_TX_HOLDOFF
#define MYNEWT_VAL_TWR_SS_EXT_TX_HOLDOFF (((uint32_t)0x0400))
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCFG_SYSCFG_TWR_SS_EXT_H */
