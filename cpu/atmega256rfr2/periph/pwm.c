/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *               2016 OTA keys S.A.
 *               2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 * @author 		Steffen Robertz <steffen.robertz@rwth-aachen.de>
 *
 * @}
 */
#define ENABLE_DEBUG    (0)

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"
#include "periph/gpio.h"
#include "debug.h"
#include "stdlib.h"
#include "math.h"

/**
 * @brief   We have 5 possible prescaler values
 */
#define PRESCALE_NUMOF          (5U)

/**
 * @brief   Possible prescaler values
 */
static const uint16_t prescalers[] = { 1, 8, 64, 256, 1024 };


uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
	/* verify data */
	assert(pwm < PWM_NUMOF);

	/* stop and reset timer */
	pwm_config[pwm].dev->CRA = 0;
	pwm_config[pwm].dev->CRB = 0;
	pwm_config[pwm].dev->CRC = 0;
	pwm_config[pwm].dev->CNT = 0;

	/* poweron timer */
	pwm_poweron(pwm);


	 /* configure the used pins */
	 unsigned i = 0;
	 while ((i < TIMER_CHAN) && (pwm_config[pwm].chan[i].pin != GPIO_UNDEF)) {
	      gpio_init(pwm_config[pwm].chan[i].pin, GPIO_OUT);
	      i++;
	 }

	 /* set PWM mode */
	 switch (mode) {
	 	 case PWM_LEFT:
	 		 /* fast PWM Mode */
	 		 if(pwm_config[pwm].chan[0].pin != GPIO_UNDEF)
	 				pwm_config[pwm].dev->CRA |= (1<<COM1A0) | (1<<COM1A1); /* Clear OCnA on compare */
	 		if(pwm_config[pwm].chan[0].pin != GPIO_UNDEF)
	 				pwm_config[pwm].dev->CRA |= (1<<COM1B0) | (1<<COM1B1); /* Clear OCnB on compare */
	 		if(pwm_config[pwm].chan[0].pin != GPIO_UNDEF)
	 				pwm_config[pwm].dev->CRA |= (1<<COM1C0) | (1<<COM1C1); /* Clear OCnA on compare */
	 		/* Fast PWM ICRn as TOP */
	 		 pwm_config[pwm].dev->CRA |= (1<<WGM11);
	 		 pwm_config[pwm].dev->CRB |= (1<<WGM12) | (1<<WGM13);
	 		 pwm_config[pwm].dev->ICR = res;
	         break;
	     case PWM_RIGHT:
	    	 return 0; /* not yet supported */
	         break;
	     case PWM_CENTER:
	    	 /* Phase correct PWM */
	         break;
	 }

	 /* Calculate Prescaler*/
	 uint32_t current_best_frequency = 0;
	 uint16_t current_best_scale = 1;
	 uint16_t current_best_prescaler = 0;
	 uint16_t max_value;
	 if(pwm_config[pwm].bits == 16) {
		 max_value = 65535;
	 }else{
		 max_value = 255;
	 }
	 for(uint32_t a = 1; (a*res) <= max_value; a++) {
		 uint32_t clock_res = CLOCK_CORECLOCK / (a*res);
		 uint32_t opt_prescaler = clock_res / (freq); /*best prescaler, might not be possible */
		 i = 0;
		 if(opt_prescaler >1) {
		 	 for(; i < (PRESCALE_NUMOF-1); i++) {
		 	 	 if(opt_prescaler >= prescalers[i] && opt_prescaler < prescalers[i+1]) {
		 	 	 	 /*chose bigger prescaler thus decreasing frequency */
		 	 	 	 i++;
		 	 	 	 /*save current best prescaler if it matches frequency better */
		 	 	 	 uint32_t current_frequency = clock_res/prescalers[i];
		 	 	 	 if(abs(current_best_frequency - freq) > abs(current_frequency-freq)){
		 	 	 		 current_best_frequency = current_frequency;
		 	 	 		 current_best_scale = a;
		 	 	 		 current_best_prescaler = i;
		 	 	 	 }
			 	 	 break;
	 	 	 	 }
	 	 	 }
	 	 }else{
	 		 //stop from searching with opt_prescaler 0
	 		 max_value =0;
	 	 }
	 }
	 /*Set prescaler */
	 pwm_config[pwm].dev->CRB |= (current_best_prescaler+1);
	 *(pwm_config[pwm].prescaler_pointer) = current_best_prescaler+1;
	 /*Save prescaler*/
	 *(pwm_config[pwm].scale_pointer) = current_best_scale;
	 /* return actual frequency */
	 DEBUG("Best Frequency %u \n",(unsigned int) current_best_frequency);
	 DEBUG("Best Scale %u \n", (unsigned int)current_best_scale);
	 DEBUG("Prescaler set to %u \n", (unsigned int)prescalers[current_best_prescaler]);
	 return (current_best_frequency);

}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);

    unsigned i = 0;
    while ((i < TIMER_CHAN) && (pwm_config[pwm].chan[i].pin != GPIO_UNDEF)) {
        i++;
    }
    return (uint8_t)i;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm < PWM_NUMOF) &&
           (channel < TIMER_CHAN) &&
           (pwm_config[pwm].chan[channel].pin != GPIO_UNDEF));
    /* norm value to maximum possible value */
    if (value > pwm_config[pwm].dev->ICR) {
        value = (uint16_t)pwm_config[pwm].dev->ICR;
    }
    pwm_config[pwm].dev->OCR[channel] = value*(*(pwm_config[pwm].scale_pointer));
    DEBUG("OCR%d %u\n",channel, (unsigned int)value*(*(pwm_config[pwm].scale_pointer)));
}

void pwm_start(pwm_t pwm)
{
	assert(*(pwm_config[pwm].prescaler_pointer));
	pwm_config[pwm].dev->CRB |= (*(pwm_config[pwm].prescaler_pointer));

}

void pwm_stop(pwm_t pwm)
{
	pwm_config[pwm].dev->CRB &= ~(0b00000111);

}

void pwm_poweron(pwm_t pwm)
{
	/* 0 in Powerregister enables timer */
	*(pwm_config[pwm].power_reg) &= ~(1<<pwm_config[pwm].power_reg_bit);
}

void pwm_poweroff(pwm_t pwm)
{
	/* 1 in Powerregister disables timer */
	*(pwm_config[pwm].power_reg) |= (1<<pwm_config[pwm].power_reg_bit);
}
