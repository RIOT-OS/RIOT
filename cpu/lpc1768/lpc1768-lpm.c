/**
 * LPM dummys for NXP LPC1768
 *
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup  lpc1768
 * @{
 * @file    lpc1768-lpm.c
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include "lpm.h"

/* lpm is accessed before memory init and initialized separately through code */
__attribute__((section(".noinit")))
static enum lpm_mode lpm;

// TODO
enum lpm_mode lpm_set(enum lpm_mode target) {
    enum lpm_mode last_lpm = lpm;

    lpm = target;

    return last_lpm;
}

void lpm_awake(void) {
    lpm = LPM_ON;
}
