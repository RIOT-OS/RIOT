/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_cxx
 * @{
 *
 * @file
 * @brief       C++ functions for guarded initialization of static variables
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This module implements very basic versions of the glibc++ functions
 * `__cxa_guard_acquire` and `__cxa_guard_release`. These functions operate
 * on guard variables that are used to realize thread-safe static object
 * initialization.
 *
 * The glibc++ built-in versions of these functions had to be replaced since
 * they use the `pthread_once` function from module `pthread` for singleton
 * objects initialization where the parameter `once` is of incompatible type.
 *
 * The functions implemented by this module don't use a global mutex to
 * avoid deadlocks. Instead, within the initialization of a static variable,
 * creating a thread that in turn tries to initialize the same static variable
 * is not allowed and results in an abort.
 */

#include <stdint.h>
#include <stdlib.h>
#include <cxxabi.h>

#include "assert.h"
#include "esp_common_log.h"
#include "irq.h"
#include "irq_arch.h"
#include "mutex.h"

/**
 * Structure of guard variable in glibc++
 *  ((char*)&__guard)[0] == 1: _GLIBCXX_GUARD_BIT (static variable already initialized)
 *  ((char*)&__guard)[1] == 1: _GLIBCXX_GUARD_PENDING_BIT (static variable is being initialized)
 *  ((char*)&__guard)[2] == 1: _GLIBCXX_GUARD_WAITING_BIT (other thread is waiting)
 */
typedef struct {
    char done;      /* initialization is already done */
    char pending;   /* initialization is in progress */
    char waiting;   /* another thread is waiting until initialization is finished */
} guard_t;

namespace __cxxabiv1
{
    extern "C"
    int __cxa_guard_acquire (__guard *g)
    {
        guard_t *_gt = (guard_t *)g;

        assert(_gt);

        /* return if the static object is already initialized */
        if (_gt->done) {
          return 0;
        }

        critical_enter();

        /* if another thread is already initializing the static object */
        if (_gt->pending) {
            critical_exit();
            LOG_ERROR("Recursive static object initialization\n");
            assert(0);
        }

        /* mark the initialization in process and acquire */
        _gt->pending = 1;

        critical_exit();
        return 1;
    }

    extern "C"
    void __cxa_guard_release (__guard *g)
    {
        guard_t *_gt = (guard_t *)g;

        assert(_gt);
        assert(_gt->pending);

        critical_enter();

        _gt->done = 1;
        _gt->pending = 0;

        critical_exit();
    }

    extern "C"
    void __cxa_guard_abort (__guard *g)
    {
        guard_t *_gt = (guard_t *)g;

        assert(_gt);
        assert(_gt->pending);

        _gt->pending = 0;
    }
}
