/**
 * riot cpu specific port of kernel functions
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 *
 */


#include <stdint.h>
#include "stm32f4xx_tim.h"

void stm32f4_pclk_scale(uint32_t source, uint32_t target, uint32_t* pclksel, uint32_t* prescale)
{
	uint32_t pclkdiv;
	*prescale = source / target;

	if( (*prescale % 4) == 0 ) {
		*pclksel = TIM_CKD_DIV4;
		pclkdiv = 4;
	} else if( (*prescale % 2) == 0 ) {
		*pclksel = TIM_CKD_DIV2;
		pclkdiv = 2;
	} else {
		*pclksel = TIM_CKD_DIV1;
		pclkdiv = 1;
	}
	*prescale /= pclkdiv;

	if( *prescale % 2 )
		(*prescale)++;
}


void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t* prescale) {
    uint32_t pclksel;

    stm32f4_pclk_scale(source, target, &pclksel, prescale);
}
