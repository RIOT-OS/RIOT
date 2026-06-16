/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "mutex.h"
#include "CYS/os_mutex.h"

mutex_t cryptoservice_mutex;

void os_init_mutex(void)
{
    mutex_init(&cryptoservice_mutex);
}

CYS_error_t os_get_mutex(void)
{
    return mutex_trylock(&cryptoservice_mutex) ? CYS_SUCCESS : CYS_ERROR_BAD_STATE;
}

void os_release_mutex(void)
{
    mutex_unlock(&cryptoservice_mutex);
}
