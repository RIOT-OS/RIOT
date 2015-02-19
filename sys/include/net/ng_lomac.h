/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    sys_net_ng_lomac    Loopback MAC layer
 * @ingroup     sys_net
 * @{
 *
 * @file
 * @brief       Interface definition for the loopback MAC layer
 *
 * @details     This MAC layer returns every packet that it is send through it.
 *              The forwarding table of a higher layer can refer to this layer,
 *              if it identifies the destination address to be of the node
 *              itself.
 *
 *              This layer only understands the NG_NETAPI_MSG_TYPE_SND command and
 *              NG_NETAPI_MSG_TYPE_GET with NETCONF_OPT_MAX_PACKET_SIZE.
 *              Everything else will be discarded or answered with
 *              NG_NETAPI_MSG_TYPE_ACK with -ENOTSUP.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_LOMAC_H_
#define NG_LOMAC_H_

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default message queue size for loopback MAC layer.
 */
#ifndef NG_LOMAC_MSG_QUEUE_SIZE
#define NG_LOMAC_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief   Initializes the loopback MAC layer.
 *
 * @details There can only be one loopback MAC layer.
 *
 * @param[in] priority The priority for the loopback MAC layer thread.
 *
 * @return  The PID to the loopback MAC layer thread.
 * @return  -EEXIST, if loopback MAC layer was already initialized.
 * @return  other negative value, if thread creation failed
 *          (see @ref thread_create())
 */
kernel_pid_t ng_lomac_init(char priority);

#ifdef __cplusplus
}
#endif

#endif /* NG_LOMAC_H_ */
/**
 * @}
 */
