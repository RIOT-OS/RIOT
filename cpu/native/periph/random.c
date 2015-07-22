/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     native_cpu
 * @defgroup    native_rng
 * @{
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "cpu_conf.h"
#include "native_internal.h"

#include "periph/random.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int powered = 0;
static int initialized = 0;
static int dev_random = -1;

/**********************************************************************
 * internal API declaration
 **********************************************************************/

/**
 * seed host random module with @ref _native_rng_seed
 */
void _native_rng_init_det(void);
void _native_rng_init_hq(void);
unsigned _native_rng_read_det(char *buf, unsigned num);
unsigned _native_rng_read_hq(char *buf, unsigned num);

/**********************************************************************
 * public API implementation
 **********************************************************************/

void random_init(void)
{
    DEBUG("random_init: initializing\n");
    switch (_native_rng_mode) {
        case 0:
            _native_rng_init_hq();
            break;
        case 1:
            _native_rng_init_det();
            break;
        default:
            err(EXIT_FAILURE, "random_init: _native_rng_mode is in invalid state %i\n",
                   _native_rng_mode);
            break;
    }

    initialized = 1;

    DEBUG("random_init: powering on\n");
    random_poweron();
}

int random_read(char *buf, unsigned int num)
{
    if (!initialized) {
        warnx("random_read: random device not initialized, failing\n");
        return 0;
    }

    if (!powered) {
        warnx("random_read: random device not powered, failing\n");
        return 0;
    }

    DEBUG("random_read: writing %u bytes\n", num);
    switch (_native_rng_mode) {
        case 0:
            num = _native_rng_read_hq(buf, num);
            break;
        case 1:
            num = _native_rng_read_det(buf, num);
            break;
        default:
            err(EXIT_FAILURE, "random_read: _native_rng_mode is in invalid state %i\n",
                   _native_rng_mode);
            break;
    }
    return num;
}

void random_poweron(void)
{
    DEBUG("random_poweron: power on\n");

    if (!initialized) {
        warnx("random_poweron: not initialized.");
    }

    powered = 1;
}

void random_poweroff(void)
{
    DEBUG("random_poweroff: power off\n");

    if (!initialized) {
        warnx("random_poweroff: not initialized.");
    }

    powered = 0;
}

/**********************************************************************
 * internal API implementation
 **********************************************************************/

void _native_rng_init_det(void)
{
    DEBUG("_native_rng_init_det\n");
    _native_syscall_enter();
    real_srandom(_native_rng_seed);
    _native_syscall_leave();
}

void _native_rng_init_hq(void)
{
    DEBUG("_native_rng_init_hq\n");
    _native_syscall_enter();
    dev_random = real_open("/dev/random", O_RDONLY);
    if (dev_random == -1) {
        err(EXIT_FAILURE, "_native_rng_init_hq: open(/dev/random)");
    }
    _native_syscall_leave();
}

unsigned _native_rng_read_det(char *buf, unsigned num)
{
    DEBUG("_native_rng_read_det\n");
    for (unsigned i = 0; i < num; i++) {
        _native_syscall_enter();
        buf[i] = (char)real_random();
        _native_syscall_leave();
    }

    return num;
}

unsigned _native_rng_read_hq(char *buf, unsigned num)
{
    DEBUG("_native_rng_read_hq\n");
    unsigned offset = 0;

    while (num > 0) {
        _native_syscall_enter();
        int r = real_read(dev_random, (buf + offset), num);
        _native_syscall_leave();

        if (r == -1) {
            err(EXIT_FAILURE, "_native_rng_read_hq: read");
        }

        num -= r;
        offset += r;
    }

    return offset;
}

/**
 * @}
 */
