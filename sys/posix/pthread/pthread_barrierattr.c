/*
 * POSIX compatible implementation of barriers.
 *
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief Attributes for synchronization barriers.
 * @author René Kijewski <kijewski@inf.fu-berlin.de>
 * @}
 */

#include "pthread.h"

#include <stdbool.h>

int pthread_barrierattr_init(pthread_barrierattr_t *attr)
{
    attr->pshared = 0;
    return 0;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *attr)
{
    (void) attr;
    return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared)
{
    *pshared = attr->pshared;
    return 0;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared)
{
    attr->pshared = pshared;
    return 0;
}
