/*
 * Copyright (C) 2015  INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_s_cosens  S-CoSenS MAC protocol for RIOT OS
 * @ingroup     net_gnrc
 * @brief       S-CoSenS MAC/radio duty cycling protocol;
 *              designed by Bilel Nefzi and Ye-Qiong Song, see:
 *              B. Nefzi PhD thesis: "Mécanismes auto-adaptatifs pour la
 *              gestion de la Qualité de Service dans les réseaux de capteurs
 *              sans-fil", chapter 5, Networking and Internet Architecture,
 *              Institut National Polytechnique de Lorraine (INPL), 2011.
 *              (document in French language)
 * @{
 *
 * @file
 * @brief       Interface definition for the S-CoSenS MAC/RDC layer
 *              for router/PAN-coordinator nodes
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef __GNRC_S_COSENS_ROUTER_H_
#define __GNRC_S_COSENS_ROUTER_H_

#include "kernel.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the default message queue size for S-CoSenS layers
 */
#ifndef GNRC_S_COSENS_ROUTER_MSG_QUEUE_SIZE
#define GNRC_S_COSENS_ROUTER_MSG_QUEUE_SIZE       (8U)
#endif

/* CSMA/CA parameters for S-CoSenS for router/PAN-coordinator nodes */
#define S_COSENS_ROUTER_MAC_MIN_BE                (2U)
#define S_COSENS_ROUTER_MAC_MAX_BE                (5U)
#define S_COSENS_ROUTER_MAC_MAX_CSMA_BACKOFFS     (5U)

/**
 * @brief   Initialize an instance of the S-CoSenS
 *          for router/PAN-coordinator nodes layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the S-CoSenS instance
 * @param[in] name          name of the thread housing the S-CoSenS instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of S-CoSenS thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t gnrc_s_cosens_router_init(char *stack,
                                       int stacksize,
                                       char priority,
                                       const char *name,
                                       gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __GNRC_S_COSENS_NODE_H_ */
/** @} */
