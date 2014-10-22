/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Optional flags for controlling a threads initial state.
 * @{
 */
#define CREATE_SLEEPING     (1)     /**< set the new thread to sleeping */
#define AUTO_FREE           (2)     /**< currently not implemented */
#define CREATE_WOUT_YIELD   (4)     /**< do not automatically call thread_yield() after creation */
#define CREATE_STACKTEST    (8)     /**< write markers into the thread's stack to measure stack
                                         usage (for debugging) */
#define CREATE_NOMSG        (16)    /**< Create thread without messaging capabilities */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _FLAGS_H */
/** @} */
