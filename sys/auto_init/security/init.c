/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
