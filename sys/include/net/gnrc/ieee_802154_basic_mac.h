/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_802154_basic_mac  IEEE 802.15.4 basic
 *                                         (i.e.: non-slotted) MAC layer
 * @ingroup     net_gnrc
 * @brief       Basic MAC protocol of the IEEE 802.15.4 standard,
 *              that sends packets using only non slotted CSMA/CA procedure
 * @{
 *
 * @file
 * @brief       Interface definition for the IEEE 802.15.4 basic MAC layer
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef GNRC_802154_BASIC_MAC_H_
#define GNRC_802154_BASIC_MAC_H_

#include "kernel.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the default message queue size for 802.15.4 basic MAC layer
 */
#ifndef GNRC_802154_BASIC_MAC_QUEUE_SIZE
#define GNRC_802154_BASIC_MAC_QUEUE_SIZE       (8U)
#endif


/**
 * @brief   Initialize an instance of the 802.15.4 basic MAC layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the MAC layer
 * @param[in] name          name of the thread housing the MAC layer
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of the 802.15.4 MAC layer thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t gnrc_802154_basic_mac_init(char *stack,
                                        int stacksize,
                                        char priority,
                                        const char *name,
                                        gnrc_netdev_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* GNRC_802154_BASIC_MAC_H_ */
/** @} */
