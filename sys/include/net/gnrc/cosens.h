/*
 * Copyright (C) 2015  INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_cosens  CoSenS MAC protocol for RIOT OS
 * @ingroup     net_gnrc
 * @brief       CoSenS MAC protocol;
 *              designed by Bilel Nefzi and Ye-Qiong Song, see:
 *              Ad Hoc Networks 10 (2012) 680-695.
 * @{
 *
 * @file
 * @brief       Interface definition for the CoSenS MAC layer
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef __GNRC_COSENS_H_
#define __GNRC_COSENS_H_

#include "kernel.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the default message queue size for CoSenS layers
 */
#ifndef GNRC_COSENS_MSG_QUEUE_SIZE
#define GNRC_COSENS_MSG_QUEUE_SIZE       (8U)
#endif

/* CSMA/CA parameters for CoSenS */
#define COSENS_MAC_MIN_BE              (2U)
#define COSENS_MAC_MAX_BE              (5U)
#define COSENS_MAC_MAX_CSMA_BACKOFFS   (5U)

/**
 * @brief   Initialize an instance of the CoSenS layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the CoSenS instance
 * @param[in] name          name of the thread housing the CoSenS instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of CoSenS thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t gnrc_cosens_init(char *stack, int stacksize, char priority,
                              const char *name, gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __GNRC_COSENS_H_ */
/** @} */
