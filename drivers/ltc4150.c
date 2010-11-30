/******************************************************************************
Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
    http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
    scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @ingroup ltc4150
 * @{
 */

/**
 * @file
 * @brief       LTC4150 Coulomb Counter
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 */

#include <hwtimer.h>
#include "ltc4150_arch.h"

static volatile unsigned long int_count;
static unsigned int last_int_time;
static unsigned int last_int_duration;
static unsigned int start_time;

static double int_to_coulomb(int ints) {
    return ((double)ints) / (_GFH * _R_SENSE);
}

static double coulomb_to_mA(double coulomb){
    return (coulomb * 1000) / 3600;
}

uint32_t ltc4150_get_last_int_duration_us() {
    return HWTIMER_TICKS_TO_US(last_int_duration);
}

double ltc4150_get_current_mA() {
    return 1000000000/(ltc4150_get_last_int_duration_us()*(_GFH * _R_SENSE));
}

double ltc4150_get_total_mAh() {
    return coulomb_to_mA(int_to_coulomb(int_count));
}

double ltc4150_get_avg_mA() {
    return (int_to_coulomb(int_count)*1000000000)/HWTIMER_TICKS_TO_US(last_int_time - start_time);
}

int ltc4150_get_interval() {
    return HWTIMER_TICKS_TO_US(last_int_time - start_time);
}

unsigned long ltc4150_get_intcount() {
    return int_count;
}

void ltc4150_init() {
    ltc4150_arch_init();
}

void ltc4150_start() {
    ltc4150_disable_int();
    int_count = 0;
    uint32_t now = hwtimer_now();
    ltc4150_sync_blocking();
    start_time = now;
    last_int_time = now;
    ltc4150_enable_int();
}

void ltc4150_stop() {
    ltc4150_disable_int();
}

void ltc4150_interrupt()
{
    uint32_t now = hwtimer_now();
    if (now >= last_int_time) {
        last_int_duration = now - last_int_time;
    } else {
        last_int_duration = (0-1) - last_int_time + now + 1;
    }

    last_int_time = now;
    int_count++;
}

/** @} */
