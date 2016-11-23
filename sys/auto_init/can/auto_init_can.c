/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup auto_init
 * @{
 * @file
 * @brief       initializes can device init function
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @}
 */

#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "can/dll.h"

void auto_init_candev(void)
{
    DEBUG("auto_init_can: init dll\n");
    can_dll_init();
#ifdef MODULE_CAN_LINUX
    extern void auto_init_can_native(void);
    auto_init_can_native();
#endif
}
