/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup pthread POSIX threads
 * POSIX conforming multi-threading features.
 * @ingroup posix
 * @{
 * @file
 * @brief   POSIX conforming multi-threading features.
 * @details Please see the transcluded `pthread_*.h` files for further information.
 * @see     [The Open Group Base Specifications Issue 7: pthread.h - threads](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
 */

#include <time.h>

/**
 * @cond INTERNAL
 */
#ifndef __WITH_AVRLIBC__
#define HAVE_MALLOC_H 1
#endif
/**
 * @endcond
 */

#include "mutex.h"
#include "sched.h"

#include "pthread_threading_attr.h"
#include "pthread_threading.h"
#include "pthread_mutex_attr.h"
#include "pthread_mutex.h"
#include "pthread_rwlock_attr.h"
#include "pthread_rwlock.h"
#include "pthread_spin.h"
#include "pthread_barrier.h"
#include "pthread_cleanup.h"
#include "pthread_once.h"
#include "pthread_scheduling.h"
#include "pthread_cancellation.h"
#include "pthread_cond.h"
#include "pthread_tls.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
