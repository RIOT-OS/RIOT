/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       Initializes the CryptoService
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 * @}
 */

#include "log.h"
#include "kernel_defines.h"
#include "CYS/os_mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void auto_init_cryptoservice(void)
{
    os_init_mutex();
    DEBUG("[auto_init_cryptoservice] CryptoService initialized\n");
}
