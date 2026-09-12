/*
 * SPDX-FileCopyrightText: 2014 René Kijewski  <rene.kijewski@fu-berlin.de>
 * SPDX-License-Identifier: NONE
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
