/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_newlib
 * @{
 *
 * @file
 * @brief       Newlib system calls for thread safety
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <sys/reent.h>

#include "rmutex.h"

static rmutex_t _env = RMUTEX_INIT;
static rmutex_t _malloc = RMUTEX_INIT;

void __env_lock(struct _reent *_r)
{
    (void)_r;

    rmutex_lock(&_env);
}

void __env_unlock(struct _reent *_r)
{
    (void)_r;

    rmutex_unlock(&_env);
}

void __malloc_lock(struct _reent *_r)
{
    (void)_r;

    rmutex_lock(&_malloc);
}

void __malloc_unlock(struct _reent *_r)
{
    (void)_r;

    rmutex_unlock(&_malloc);
}
