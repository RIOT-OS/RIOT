/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup lpc2387
 * @ingroup lpm
 * @{
 */

/**
 * @file
 * @brief       LPC2387 Low-Power management
 * @ingroup     lpc2387
 *
 * @author      Heiko Will
 * @version     $Revision$
 *
 * @note        $Id$
 */

#include <stdio.h>
#include <stdint.h>
#include "lpc23xx.h"
#include "lpc2387.h"
#include "lpm.h"

/* lpm is accessed before memory init and initialized separately through code */
__attribute__((section(".noinit")))
static enum lpm_mode lpm;

extern void init_clks1(void);
extern void init_clks2(void);

#define ENABLE_DEBUG (0)
#include "debug.h"

void lpm_init(void)
{
    lpm = LPM_ON;
}

#define LPM_DEBUG (1)

void lpm_begin_awake(void)
{
    if (lpm >= LPM_SLEEP) {                                 // wake up from deep sleep
        init_clks1();
    }
}

void lpm_end_awake(void)
{
    if (lpm >= LPM_SLEEP) {                                 // wake up from deep sleep
        init_clks2();
    }

    lpm = LPM_ON;
}

void lpm_awake(void)
{
#if LPM_DEBUG
    unsigned long usec = RTC_CTC;
#endif

    if (lpm >= LPM_SLEEP) {                                 // wake up from deep sleep
        /* benchmark */
        init_clks1();
        init_clks2();
        /* Debug tests */
#if LPM_DEBUG
        usec = RTC_CTC - usec;
        DEBUG("Wakeup in %lu usecs\n", usec * 31);
#endif
    }

    lpm = LPM_ON;
}

enum lpm_mode lpm_set(enum lpm_mode target)
{
    unsigned target_flags;
    enum lpm_mode last_lpm = lpm;

    /* calculate target mcu power mode */
    if (target == LPM_IDLE) {
        target_flags = PM_IDLE;
    }
    else if (target == LPM_SLEEP) {
        target_flags = PM_SLEEP;
    }
    else if (target == LPM_MT_TRANSCEIVER_POWERDOWN) {
        target_flags = PM_MT_TRANSCEIVER_POWERDOWN;
    }
    else {
        target_flags = 0;
    }

    lpm = target;

    DEBUG("# LPM power down %u -> %u", lpm, target);

    PCON |= target_flags;                                       // set target power mode
    return last_lpm;
}
/*---------------------------------------------------------------------------*/
enum lpm_mode
lpm_get(void)
{
    return lpm;
}
/*---------------------------------------------------------------------------*/
/** @} */
