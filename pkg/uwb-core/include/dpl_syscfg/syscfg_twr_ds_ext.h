/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core double side extended two-way ranging module configurations
 *              taken from decawave-uwb-core/lib/twr_ds_ext/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable double sided extended two way ranging
 */
#ifndef MYNEWT_VAL_TWR_DS_EXT_ENABLED
#define MYNEWT_VAL_TWR_DS_EXT_ENABLED IS_USED(MODULE_UWB_CORE_TWR_DS_EXT)
#endif

/**
 * @brief Enable double sided extended two way ranging
 */
#ifndef MYNEWT_VAL_TWR_DS_EXT_RX_TIMEOUT
#define MYNEWT_VAL_TWR_DS_EXT_RX_TIMEOUT (((uint16_t)0x40))
#endif

/**
 * @brief tx holdoff delay for DS TWR extended frame (usec)
 */
#ifndef MYNEWT_VAL_TWR_DS_EXT_TX_HOLDOFF
#define MYNEWT_VAL_TWR_DS_EXT_TX_HOLDOFF (((uint32_t)0x0400 + 0xA0 * IS_USED(MODULE_UWB_CORE_RNG_TRX_INFO)))
#endif

#ifdef __cplusplus
}
#endif
