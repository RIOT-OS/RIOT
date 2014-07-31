/**
 * MBED LPC1768 board hwtimer_arch.h implementation stub
 *
 * TODO: implement
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup hwtimer
 * @ingroup mbed_lpc1768
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @file
 * @}
 */

#include <inttypes.h>

#include "arch/hwtimer_arch.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

void hwtimer_arch_enable_interrupt(void)
{
    DEBUG("hwtimer_arch_enable_interrupt(): not implemented\n");
    return;
}

void hwtimer_arch_disable_interrupt(void)
{
    DEBUG("hwtimer_arch_disable_interrupt(): not implemented\n");
    return;
}

void hwtimer_arch_unset(short timer)
{
    DEBUG("hwtimer_arch_unset(%i): not implemented\n", timer);
    return;
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    DEBUG("hwtimer_arch_set(%lu, %i): not implemented\n", offset, timer);
    return;
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    DEBUG("hwtimer_arch_set_absolute(%lu, %i): not implemented\n", value, timer);
    return;
}

unsigned long hwtimer_arch_now(void)
{
    DEBUG("hwtimer_arch_now(): not implemented\n");
    return 0;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    DEBUG("hwtimer_arch_init(%p, %" PRIu32 "): not implemented\n", handler, fcpu);
    return;
}
