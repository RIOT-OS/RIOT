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
 * @brief       Single sided ranging using a hw generated ack module configurations
 *              taken from decawave-uwb-core/lib/twr_ss_ack/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef SYSCFG_SYSCFG_TWR_SS_ACK_H
#define SYSCFG_SYSCFG_TWR_SS_ACK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable toplevel ranging services
 */
#ifndef MYNEWT_VAL_TWR_SS_ACK_ENABLED
#define MYNEWT_VAL_TWR_SS_ACK_ENABLED (IS_ACTIVE(MODULE_UWB_CORE_TWR_SS_ACK))
#endif

/**
 * @brief tx holdoff delay used to know how long to wait for SS TWR ACK message (usec)
 */
#ifndef MYNEWT_VAL_TWR_SS_ACK_RX_TIMEOUT
#define MYNEWT_VAL_TWR_SS_ACK_RX_TIMEOUT (((uint16_t)0x100))
#endif

/**
 * @brief TOA timeout delay for SS TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_SS_ACK_TX_HOLDOFF
#define MYNEWT_VAL_TWR_SS_ACK_TX_HOLDOFF (((uint32_t)0x800))
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCFG_SYSCFG_TWR_SS_ACK_H */
