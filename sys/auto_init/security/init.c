/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       Initializes security modules
 *
 * @author      Bennet Blischke <bennet.blischke@haw-hamburg.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg,de>
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

void auto_init_security(void)
{
#if IS_USED(MODULE_CRYPTOAUTHLIB)
    extern void auto_init_atca(void);
    DEBUG("auto_init_security: atca\n");
    auto_init_atca();
#endif
}
