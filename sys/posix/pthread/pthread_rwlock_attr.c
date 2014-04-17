/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     pthread
 * @{
 *
 * @file
 * @brief       Implementation of a fair, POSIX conforming reader/writer lock (attribute set).
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "pthread.h"

#include <string.h>

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0, sizeof (*attr));
    return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    (void) attr;
    return 0;
}

/* Return current setting of process-shared attribute of ATTR in PSHARED.  */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared)
{
    if (attr == NULL || pshared == NULL) {
        return EINVAL;
    }

    *pshared = attr->pshared;
    return 0;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared)
{
    if (attr == NULL || (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE)) {
        return EINVAL;
    }

    attr->pshared = pshared;
    return 0;
}
