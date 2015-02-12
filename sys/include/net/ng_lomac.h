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
 * @details     This MAC layer triggers a receive command everytime a
 *              packet is send to it.
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
