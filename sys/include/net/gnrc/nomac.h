/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_nomac  Simplest possible MAC layer
 * @ingroup     net_gnrc
 * @brief       Simplest possible MAC protocol that sends without considering
 *              the state of the medium
 * @{
 *
 * @file
 * @brief       Interface definition for the NOMAC MAC layer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef GNRC_NOMAC_H_
#define GNRC_NOMAC_H_

#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the default message queue size for NOMAC layers
 */
#ifndef GNRC_NOMAC_MSG_QUEUE_SIZE
#define GNRC_NOMAC_MSG_QUEUE_SIZE       (8U)
#endif

/**
 * @brief   Initialize an instance of the NOMAC layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the NOMAC instance
 * @param[in] name          name of the thread housing the NOMAC instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of NOMAC thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t gnrc_nomac_init(char *stack, int stacksize, char priority,
                             const char *name, gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __NOMAC_H_ */
/** @} */
