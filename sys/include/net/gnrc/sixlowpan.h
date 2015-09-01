/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan    6LoWPAN
 * @ingroup     net_gnrc
 * @brief       GNRC's 6LoWPAN implementation
 * @{
 *
 * @file
 * @brief   Definitions for 6LoWPAN
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SIXLOWPAN_H_
#define GNRC_SIXLOWPAN_H_

#include <stdbool.h>

#include "kernel_types.h"

#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/iphc.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_STACK_SIZE
#define GNRC_SIXLOWPAN_STACK_SIZE       (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_PRIO
#define GNRC_SIXLOWPAN_PRIO             (THREAD_PRIORITY_MAIN - 4)
#endif

/**
 * @brief   Default message queue size to use for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_MSG_QUEUE_SIZE
#define GNRC_SIXLOWPAN_MSG_QUEUE_SIZE   (8U)
#endif

/**
 * @brief   Initialization of the 6LoWPAN thread.
 *
 * @details If 6LoWPAN was already initialized, it will just return the PID of
 *          the 6LoWPAN thread.
 *
 * @return  The PID to the 6LoWPAN thread, on success.
 * @return  -EINVAL, if @ref GNRC_SIXLOWPAN_PRIO was greater than or equal to
 *          @ref SCHED_PRIO_LEVELS
 * @return  -EOVERFLOW, if there are too many threads running already in general
 */
kernel_pid_t gnrc_sixlowpan_init(void);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_H_ */
/** @} */
