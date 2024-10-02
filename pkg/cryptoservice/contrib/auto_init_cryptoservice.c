/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
