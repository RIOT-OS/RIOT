/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_schc_config GNRC SCHC compile configurations
 * @ingroup     net_gnrc_schc
 * @ingroup     net_gnrc_conf
 * @brief
 * @{
 *
 * @file
 * @brief   Configuration macros for @ref net_gnrc_sixlowpan
 *
 * @author  Bart Moons <bamoons.moons@ugent.be>
 */
#ifndef NET_GNRC_SCHC_CONFIG_H
#define NET_GNRC_SCHC_CONFIG_H

#include "kernel_defines.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the SCHC thread.
 */
#ifndef GNRC_SCHC_STACK_SIZE
#define GNRC_SCHC_STACK_SIZE           (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the 6LoWPAN thread.
 */
#ifndef GNRC_SCHC_PRIO
#define GNRC_SCHC_PRIO                 (THREAD_PRIORITY_MAIN - 4)
#endif

/**
 * @brief   Default message queue size to use for the SCHC thread (as
 *          exponent of 2^n).
 *
 * As the queue size ALWAYS needs to be power of two, this option represents the
 * exponent of 2^n, which will be used as the size of the queue.
 */
#ifndef CONFIG_GNRC_SCHC_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_SCHC_MSG_QUEUE_SIZE_EXP   (3U)
#endif

/**
 * @brief   Message queue size to use for the SCHC thread.
 */
#ifndef GNRC_SCHC_MSG_QUEUE_SIZE
#define GNRC_SCHC_MSG_QUEUE_SIZE    			(1 << CONFIG_GNRC_SCHC_MSG_QUEUE_SIZE_EXP)
#endif


#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SCHC_CONFIG_H */
/** @} */
