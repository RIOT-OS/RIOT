/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Thread creation attributes.
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 * @}
 */

#include <string.h>

#include "pthread.h"

int pthread_attr_init(pthread_attr_t *attr)
{
    memset(attr, 0, sizeof (*attr));
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    (void) attr;
    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    *detachstate = attr->detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    switch (detachstate) {
        case PTHREAD_CREATE_DETACHED:
            attr->detached = 1;
            return 0;
        case PTHREAD_CREATE_JOINABLE:
            attr->detached = 0;
            return 0;
        default:
            return -1;
    }
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    // TODO
    (void) attr;
    (void) guardsize;
    return -1;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    // TODO
    (void) attr;
    (void) guardsize;
    return -1;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
    // TODO
    (void) attr;
    (void) param;
    return -1;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    // TODO
    (void) attr;
    (void) param;
    return -1;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    // TODO
    (void) attr;
    (void) policy;
    return -1;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    // TODO
    (void) attr;
    (void) policy;
    return -1;
}

int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit)
{
    // TODO
    (void) attr;
    (void) inherit;
    return -1;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit)
{
    // TODO
    (void) attr;
    (void) inherit;
    return -1;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *scope)
{
    // TODO
    (void) attr;
    (void) scope;
    return -1;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    // TODO
    (void) attr;
    (void) scope;
    return -1;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr)
{
    *stackaddr = attr->ss_sp;
    return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
    attr->ss_sp = (char *) stackaddr;
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    *stacksize = attr->ss_size;
    if (*stacksize == 0) {
        /* FIXME: the standard says that this function should return the default value if no explicit value was set. */
    }
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    attr->ss_size = stacksize;
    return 0;
}
