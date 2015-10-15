/*
 * Copyright (C) 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Reboot function
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de
 *
 * @}
 */

#include "kernel.h"
#include "arch/reboot_arch.h"

int reboot(int mode)
{
    if (mode != RB_AUTOBOOT) {
        return -1;
    }

    return reboot_arch(mode);
}
