/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_sixlowpan_netif  6LoWPAN network interfaces
 * @ingroup     net_ng_sixlowpan
 * @brief       6LoWPAN specific information on @ref net_ng_netif
 * @{
 *
 * @file
 * @brief   Definitions for 6LoWPAN specific information of network interfaces.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_SIXLOWPAN_NETIF_H_
#define NG_SIXLOWPAN_NETIF_H_

#include <stdbool.h>

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of 6LoWPAN interface type.
 */
typedef struct {
    kernel_pid_t pid;       /**< PID of the interface */
    uint16_t max_frag_size; /**< Maximum fragment size for this interface */
#ifdef MODULE_NG_SIXLOWPAN_IPHC
    bool iphc_enabled;      /**< enable or disable IPHC */
#endif
} ng_sixlowpan_netif_t;

/**
 * @brief   Initializes the module
 */
void ng_sixlowpan_netif_init(void);

/**
 * @brief   Add interface to 6LoWPAN.
 *
 * @param[in] pid           The PID to the interface.
 * @param[in] max_frag_size The maximum fragment size for this interface.
 */
void ng_sixlowpan_netif_add(kernel_pid_t pid, uint16_t max_frag_size);

/**
 * @brief   Remove interface from 6LoWPAN.
 *
 * @param[in] pid   The PID to the interface.
 */
void ng_sixlowpan_netif_remove(kernel_pid_t pid);

/**
 * @brief   Get interface.
 *
 * @param[in] pid   The PID to the interface
 *
 * @return  The interface describing structure, on success.
 * @return  NULL, if there is no interface with PID @p pid.
 */
ng_sixlowpan_netif_t *ng_sixlowpan_netif_get(kernel_pid_t pid);

#ifdef __cplusplus
}
#endif

#endif /* NG_SIXLOWPAN_NETIF_H_ */
/** @} */
