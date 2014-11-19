/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     native_cpu
 * @{
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <string.h>
#include <stdlib.h>
#include <err.h>

#include "cpu-conf.h"
#include "native_internal.h"

#include "periph/random.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int powered = 0;

void random_init(void)
{
    DEBUGF("random_init: powering on\n");
    random_poweron();
}

int random_read(char *buf, unsigned int num)
{
    if (!powered) {
        warnx("random_read: random device not powered, failing\n");
        return 0;
    }

    DEBUGF("random_read: writing %ui bytes\n", num);
    for (unsigned i = 0; i < num; i++) {
        _native_syscall_enter();
        buf[i] = (char)real_random();
        _native_syscall_leave();
    }
    return num;
}

/**
 * seed host random module with @ref _native_id
 */
void random_poweron(void)
{
    DEBUGF("random_poweron: power on\n");
    powered = 1;
    _native_syscall_enter();
    real_srandom((unsigned int)_native_id);
    _native_syscall_leave();
}

void random_poweroff(void)
{
    DEBUGF("random_poweroff: power off\n");
    powered = 0;
}

/**
 * @}
 */
