/*
 * Copyright (C) 2017 HAW Hamburg
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lowpan       LoWPAN
 * @ingroup     net_gnrc
 * @brief       GNRC's LoWPAN adaptation layer and frame format handling
 *
 * @{
 *
 * @file
 * @brief   Definitions for the LoWPAN adaptation layer and frame format handling
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Cenk Gündoğan <mail-github@cgundogan.de>
 */
#ifndef NET_GNRC_LOWPAN_H
#define NET_GNRC_LOWPAN_H

#include <stdbool.h>

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the LoWPAN thread.
 */
#ifndef GNRC_LOWPAN_STACK_SIZE
#define GNRC_LOWPAN_STACK_SIZE			(THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the LoWPAN thread.
 */
#ifndef GNRC_LOWPAN_PRIO
#define GNRC_LOWPAN_PRIO				(THREAD_PRIORITY_MAIN - 4)
#endif

/**
 * @brief   Default message queue size to use for the LoWPAN thread.
 */
#ifndef GNRC_LOWPAN_MSG_QUEUE_SIZE
#define GNRC_LOWPAN_MSG_QUEUE_SIZE		(8U)
#endif

/**
 * @brief   Initialization of the LoWPAN thread.
 *
 * @details If LoWPAN was already initialized, it will just return the PID of
 *          the LoWPAN thread.
 *
 * @return  The PID to the LoWPAN thread, on success.
 * @return  -EINVAL, if @ref GNRC_LOWPAN_PRIO was greater than or equal to
 *          @ref SCHED_PRIO_LEVELS
 * @return  -EOVERFLOW, if there are too many threads running already in general
 */
kernel_pid_t gnrc_lowpan_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LOWPAN_H */
/** @} */
