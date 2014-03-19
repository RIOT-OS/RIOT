/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        debug.h
 * @brief       Debug-header
 *
 * #define ENABLE_DEBUG, include this and then use DEBUG as printf you can toggle.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>
#include "sched.h"

#if DEVELHELP
#include "cpu-conf.h"
#define DEBUG_PRINT(...) \
    do { \
        if ((active_thread == NULL) || (active_thread->stack_size > KERNEL_CONF_STACKSIZE_PRINTF)) { \
            printf(__VA_ARGS__); \
        } \
        else { \
            puts("Cannot debug, stack too small"); \
        } \
    } while (0)
#else
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#endif

#if ENABLE_DEBUG
#include "tcb.h"
#define DEBUG(...) DEBUG_PRINT(__VA_ARGS__)
#define DEBUGF(...) \
    do { \
        DEBUG_PRINT("DEBUG(%s): %s:%d in %s: ", \
                active_thread ? active_thread->name : "NO THREAD", \
                __FILE__, __LINE__, __func__); \
        DEBUG_PRINT(__VA_ARGS__); \
    } while (0)
#undef ENABLE_DEBUG
#else
#define DEBUG(...)
#define DEBUGF(...)
#endif

/** @} */
#endif /* __DEBUG_H */
