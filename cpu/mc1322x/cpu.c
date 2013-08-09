/*
 * cpu.c - MC1322X architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 *
 */

#include "mc1322x.h"
#include "lpm.h"
#include "arm_cpu.h"

__attribute__((naked, noreturn)) void arm_reset(void)
{
    dINT();
    CRM->SW_RST = SW_RST_VAL;

    while (1);
}

enum lpm_mode lpm_set(enum lpm_mode target) {
	return LPM_ON;
}