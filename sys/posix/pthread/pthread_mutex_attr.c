/*
 * Copyright (C) 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief Attributes for pthread mutexes.
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include "pthread.h"

#include <string.h>

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0, sizeof (*attr));
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    (void) attr;
    return 0;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared)
{
    if (attr == NULL || pshared == NULL) {
        return EINVAL;
    }

    *pshared = attr->pshared;
    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared)
{
    if (attr == NULL || (pshared != PTHREAD_PROCESS_SHARED &&
                         pshared != PTHREAD_PROCESS_PRIVATE)) {
        return EINVAL;
    }

    attr->pshared = pshared;
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *kind)
{
    if (attr == NULL || kind == NULL) {
        return EINVAL;
    }

    *kind = attr->kind;
    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind)
{
    if (attr == NULL || (kind != PTHREAD_MUTEX_NORMAL &&
                         kind != PTHREAD_MUTEX_RECURSIVE &&
                         kind != PTHREAD_MUTEX_ERRORCHECK)) {
        return EINVAL;
    }

    if (kind != PTHREAD_MUTEX_NORMAL) {
        /* only "normal" mutexes are implemented, yet */
        return EINVAL;
    }

    attr->kind = kind;
    return 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol)
{
    if (attr == NULL || protocol == NULL) {
        return EINVAL;
    }

    *protocol = attr->protocol;
    return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol)
{
    if (attr == NULL || (protocol != PTHREAD_PRIO_NONE &&
                         protocol != PTHREAD_PRIO_INHERIT &&
                         protocol != PTHREAD_PRIO_PROTECT)) {
        return EINVAL;
    }

    if (protocol != PTHREAD_PRIO_NONE) {
        /* priority inheritance is not supported, yet */
        return EINVAL;
    }

    attr->protocol = protocol;
    return 0;
}

int pthread_mutexattr_getrobust(const pthread_mutexattr_t *attr, int *robustness)
{
    if (attr == NULL || robustness == NULL) {
        return EINVAL;
    }

    *robustness = attr->robustness;
    return 0;
}

int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robustness)
{
    if (attr == NULL || (robustness != PTHREAD_MUTEX_STALLED &&
                         robustness != PTHREAD_MUTEX_ROBUST)) {
        return EINVAL;
    }

    if (robustness != PTHREAD_MUTEX_STALLED) {
        /* robust mutexes are not supported, yet */
        return EINVAL;
    }

    attr->robustness = robustness;
    return 0;
}
