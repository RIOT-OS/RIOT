/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_newlib
 * @{
 *
 * @file
 * @brief       Newlib thread-safe env implementation
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "newlib_lock.h"

#if USE_NEWLIB_THREAD_SAFE
static recursive_mutex_t _env = RECURSIVE_MUTEX_INIT;
#endif

/**
 * @brief   __env_lock needs to provide recursive mutex locking
 */
void __env_lock(struct _reent *_r)
{
    (void) _r;

    /* TODO another way would be to avoid rescheduling other tasks */
#if USE_NEWLIB_THREAD_SAFE
    __recursive_lock(&_env);
#endif
}

void __env_unlock(struct _reent *_r)
{
    (void) _r;

#if USE_NEWLIB_THREAD_SAFE
    __recursive_unlock(&_env);
#endif
}
