/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        lpm_cpu.c
 * @brief       low-power mode implementation for LM4F120 MCU
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include "cpu.h"

#include "lpm.h"

void lpm_init(void)
{
    lpm_arch_init();
}

/* Change the current power-saving mode. */
enum lpm_mode lpm_set(enum lpm_mode target)
{
	return(lpm_arch_set(target));
}
	
/* Return the current LPM mode of the MSP430 MCU. */
enum lpm_mode lpm_get(void)
{
	return(lpm_arch_get());
    
}

/* resume the MSP430 MCU */
inline void lpm_awake(void)
{
    /* disable all power savings mechanisms */
	lpm_arch_awake();
}

/* the following two functions have no actual role to play MSP430s */
inline void lpm_begin_awake(void)
{
	lpm_arch_begin_awake();
}
inline void lpm_end_awake(void)
{
	lpm_arch_end_awake();
}
