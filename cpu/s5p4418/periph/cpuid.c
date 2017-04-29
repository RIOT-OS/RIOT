/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Dang Minh Phuong <kamejoko80@yahoo.com>
 * @}
 */

#include <string.h>
#include <stdint.h>
#include <random.h>
#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif
#include "periph/cpuid.h"
#include "cpu_conf.h"

void cpuid_get(void *id)
{
    uint32_t randomize;
#ifdef MODULE_XTIMER
    random_init(_xtimer_now());
#else
    random_init(0x12345678);
#endif
    randomize = random_uint32();
    memcpy(id, &randomize, 4);
}
