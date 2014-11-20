/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _ARM_CPU_H
#define _ARM_CPU_H

#include <stdint.h>
#include "VIC.h"
#include "arm7_common.h"

#define NEW_TASK_CPSR 0x1F
#define WORDSIZE 32

extern void dINT(void);
extern void eINT(void);

uint32_t get_system_speed(void);
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

void arm_reset(void);
void stdio_flush(void);

#endif // _ARM_CPU_H
