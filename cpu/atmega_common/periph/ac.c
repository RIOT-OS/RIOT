/*
 * Copyright (C) 2017 RWTH-Aachen, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level Analog Comparator driver implementation
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen-de>
 *
 * @}
 */

#include "periph/ac.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "cpu.h"
#include "thread.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static ac_isr_ctx_t ctx[AC_NUMOF];

int ac_init(ac_t dev, ac_output_mode_t output_mode, ac_irq_mode_t irq_mode, ac_isr_ctx_t received_ctx)
{

	ctx[dev] = received_ctx;

	 gpio_init(ac_config[dev].in1, GPIO_IN);
	 gpio_init(ac_config[dev].in2, GPIO_IN);

	 /* configure comparator */
	 /*delete all settings but keep comparator powered off */
	 *(ac_config[dev].acsr) = 0x80;
	 if(output_mode == AC_INPUT_CAPTURE) {
		 *(ac_config[dev].acsr) |= (1<<ACIC);
	 }
	 switch(irq_mode){
	 case AC_IRQ_DISABLED:
		 break;
	 case AC_IRQ_TOGGLE:
		 *(ac_config[dev].acsr) &= ~((1<<ACIS0)|(1<<ACIS1));
		 ac_irq_enable(dev);
		 break;
	 case AC_IRQ_FALLING_EDGE:
		 *(ac_config[dev].acsr) |= ((1<<ACIS0));
		 ac_irq_enable(dev);
		 break;
	 case AC_IRQ_RISING_EDGE:
		 *(ac_config[dev].acsr) |= ((1<<ACIS0)|(1<<ACIS1));
		 ac_irq_enable(dev);
		 break;
	 }
	 return 0;
}

void ac_poweron(ac_t dev)
{
	uint8_t is_irq_disabled = *(ac_config[dev].acsr) & (1<<ACIE);
	if(!is_irq_disabled)
		*(ac_config[dev].acsr) &= ~(1<<ACIE);
	*(ac_config[dev].acsr) &=  ~(1<<ACD);
	/*turn on interrupts again */
	if(!is_irq_disabled)
		*(ac_config[dev].acsr) |= (1<<ACIE);
}

void ac_poweroff(ac_t dev)
{
	uint8_t is_irq_disabled = *(ac_config[dev].acsr) & (1<<ACIE);
	if(!is_irq_disabled)
		*(ac_config[dev].acsr) &= ~(1<<ACIE);
	*(ac_config[dev].acsr) |=  (1<<ACD);
	/*turn on interrupts again */
	if(!is_irq_disabled)
		*(ac_config[dev].acsr) |= (1<<ACIE);
}

void ac_irq_enable(ac_t dev)
{
	*(ac_config[dev].acsr) |= (1<<ACIE);
	sei();
}

void ac_irq_disable(ac_t dev)
{
	*(ac_config[dev].acsr) &= ~(1<<ACIE);
}

uint8_t ac_read(ac_t dev)
{
	uint8_t compare_value = (*(ac_config[dev].acsr) & (1<<ACO))>>ACO;
	return compare_value;
}

#ifdef AC_NUMOF
static inline void _isr(ac_t dev)
{
    __enter_isr();
    ctx[dev].cb(ctx[dev].arg);

    if (sched_context_switch_request) {
        thread_yield();
    }

    __exit_isr();
}
#endif

#ifdef AC_0
ISR(ANALOG_COMP_vect, ISR_BLOCK)
{
    _isr(0);
}
#endif

