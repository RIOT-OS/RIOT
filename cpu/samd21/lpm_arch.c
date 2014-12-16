/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "arch/lpm_arch.h"
#include "cpu.h"
#define ENABLE_DEBUG (1)
#include "debug.h"

#define DeepSleep (1UL << 2)

void lpm_arch_init(void)
{
    lpm_awake();
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
	 //DEBUG("LPM: %d \n", target);
	 switch(target)
	 {
		 case LPM_ON:
		  SCB->SCR &= ~DeepSleep;
		  PM->SLEEP.reg = 0;
		  break;
		 case LPM_IDLE:
		  SCB->SCR &= ~DeepSleep;
		  PM->SLEEP.reg = 2;
		  break;
		 case LPM_SLEEP:
		  DEBUG("\n\n\t LPM SleepWalking \n");	  
		  SCB->SCR |=  DeepSleep;
		  PM->SLEEP.reg = 3;
		  break;
		 case LPM_POWERDOWN:
		  break;
		 case LPM_OFF:
		  break;
		 default:
		 	DEBUG("LPM Unknown!");
		 	return 0;
	 }
	 __DSB();
	 __WFI(); //Wait for interrupt
	    return target;
}

enum lpm_mode lpm_arch_get(void)
{
    uint8_t mode = PM->SLEEP.bit.IDLE;
    switch(mode)
    {
 	 case 0:
      return LPM_ON;
     case 2:
      return LPM_IDLE;
     case 3:
      return LPM_SLEEP;
     default:
      return 0;
    }
}

void lpm_arch_awake(void)
{
	lpm_arch_set(LPM_ON);
}

void lpm_arch_begin_awake(void)
{
    // TODO
}

void lpm_arch_end_awake(void)
{
    // TODO
}
