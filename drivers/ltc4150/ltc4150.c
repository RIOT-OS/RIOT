/******************************************************************************
 * Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @ingroup ltc4150
 * @{
 */

/**
 * @file
 * @brief       LTC4150 Coulomb Counter
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <hwtimer.h>
#include "ltc4150_arch.h"

static volatile unsigned long int_count;
static unsigned int last_int_time;
static unsigned int last_int_duration;
static unsigned int start_time;

static double __attribute__((__no_instrument_function__)) int_to_coulomb(int ints)
{
    return ((double)ints) / (_GFH * _R_SENSE);
}

static double __attribute__((__no_instrument_function__)) coulomb_to_mA(double coulomb)
{
    return (coulomb * 1000) / 3600;
}

static double mAh_to_Joule(double mAh)
{
    return (SUPPLY_VOLTAGE * mAh * 3600);
}

uint32_t ltc4150_get_last_int_duration_us(void)
{
    return HWTIMER_TICKS_TO_US(last_int_duration);
}

double ltc4150_get_current_mA(void)
{
    return 1000000000 / (ltc4150_get_last_int_duration_us() * (_GFH * _R_SENSE));
}

double __attribute__((__no_instrument_function__)) ltc4150_get_total_mAh(void)
{
    return coulomb_to_mA(int_to_coulomb(int_count));
}

double ltc4150_get_total_Joule(void)
{
    return mAh_to_Joule(ltc4150_get_total_mAh());
}

double ltc4150_get_avg_mA(void)
{
    return (int_to_coulomb(int_count) * 1000000000) / HWTIMER_TICKS_TO_US(last_int_time - start_time);
}

int ltc4150_get_interval(void)
{
    return HWTIMER_TICKS_TO_US(last_int_time - start_time);
}

unsigned long __attribute__((__no_instrument_function__)) ltc4150_get_intcount(void)
{
    return int_count;
}

void ltc4150_init(void)
{
    ltc4150_arch_init();
}

void ltc4150_start(void)
{
    ltc4150_disable_int();
    int_count = 0;
    uint32_t now = hwtimer_now();
    ltc4150_sync_blocking();
    start_time = now;
    last_int_time = now;
    ltc4150_enable_int();
}

void ltc4150_stop(void)
{
    ltc4150_disable_int();
}

void __attribute__((__no_instrument_function__)) ltc4150_interrupt(void)
{
    uint32_t now = hwtimer_now();

    if (now >= last_int_time) {
        last_int_duration = now - last_int_time;
    }
    else {
        last_int_duration = (0 - 1) - last_int_time + now + 1;
    }

    last_int_time = now;
    int_count++;
}

/** @} */
