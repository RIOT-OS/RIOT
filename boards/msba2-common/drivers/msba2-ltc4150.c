/*
 * Copyright (C) 2008-2009, Freie Universitaet Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     msba2
 * @ingroup     ltc4150
 * @{
 */

/**
 * @file
 * @brief       LTC4150 MSB-A2 specific implemetation
 *
 * @author      Heiko Will
 * @author      Michael Baar
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include "lpc2387.h"
#include "ltc4150_arch.h"
#include "gpioint.h"

void __attribute__((__no_instrument_function__)) ltc4150_disable_int(void)
{
    gpioint_set(0, BIT4, GPIOINT_DISABLE, NULL);
}

void __attribute__((__no_instrument_function__)) ltc4150_enable_int(void)
{
    gpioint_set(0, BIT4, GPIOINT_FALLING_EDGE, &ltc4150_interrupt);
}

void __attribute__((__no_instrument_function__)) ltc4150_sync_blocking(void)
{
    while (!(FIO0PIN & BIT4)) {};
}

void __attribute__((__no_instrument_function__)) ltc4150_arch_init(void)
{
    FIO0DIR |= BIT5;
    FIO0SET = BIT5;
}

/** @} */
