/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file        flags.h
 * @brief       Misc flag definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _FLAGS_H
#define _FLAGS_H

/**
 * @def       DAEMON_THREAD
 * @brief     Create thread as a daemon.
 * @details   The new thread won't contribute to num_tasks.
 *            If the system was built to shut down after the last thread was finished,
 *            e.g. with `NATIVE_AUTO_EXIT`, this thread won't prevent it.
 */

/**
 * @name Optional flags for controlling a threads initial state.
 * @{
 */
#define CREATE_SLEEPING     (1)     /**< set the new thread to sleeping */
#define AUTO_FREE           (2)     /**< currently not implemented */
#define CREATE_WOUT_YIELD   (4)     /**< do not automatically call thread_yield() after creation */
#define CREATE_STACKTEST    (8)     /**< write markers into the thread's stack to measure stack
                                         usage (for debugging) */
#define DAEMON_THREAD      (16)
/** @} */

#endif /* _FLAGS_H */
/** @} */
