/**
 * Native Board config.h implementation
 *
 * No functionality implemented at the moment.
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "config.h"

#ifdef MODULE_NATIVENET
#include "nativenet.h"
#include "nativenet_internal.h"
#endif
#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * XXX: loading not implemented
 */
void config_load(void)
{
    DEBUG("config_load()\n");

    sysconfig.id = _native_id;

#ifdef MODULE_NATIVENET
    _nativenet_default_dev_more._radio_addr = _native_id;
#endif

    return;
}

/**
 * XXX: storing not implemented
 */
uint8_t config_save(void)
{
    printf("XXX: config_save(): not implemented - your config will vanish on process termination\n");
    return 1;
}
