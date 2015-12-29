/*
 * Copyright (C) 2015 AGH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_efm32wg
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Kubaszek Mateusz <mathir.km.riot@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF             (16U)

/**
 * @brief   Datastructure to hold an interrupt context
 */
typedef struct {
	void (*cb)(void *arg); /**< interrupt callback routine */
	void *arg; /**< optional argument */
} exti_ctx_t;

/**
 * @brief   Hold one callback function pointer for each interrupt line
 */
static exti_ctx_t exti_chan[GPIO_ISR_CHAN_NUMOF];

/**
 * @brief   Extract the port number from the given pin identifier
 */
static inline uint32_t _port(gpio_t gpio) {
	return (uint32_t) ((gpio & (0xff00)) >> 8);
}

/**
 * @brief   Extract the ports pin number form the given identifier
 */
static inline uint32_t _pin(gpio_t gpio) {
	return (uint32_t) (gpio & 0xff);
}

int gpio_init(gpio_t gpio, gpio_dir_t mode, gpio_pp_t pullup) {

	if (GPIO_PULLUP == pullup) {
		/* High */
		gpio_set(gpio);
		if(GPIO_DIR_IN == mode){
			mode = GPIO_DIR_INPUT_PULL;
		}
	} else if (GPIO_PULLDOWN == pullup){
		/* Low */
		gpio_clear(gpio);
		if(GPIO_DIR_IN == mode){
			mode = GPIO_DIR_INPUT_PULL;
		}
	}

	/* There are two registers controlling the pins for each port. The MODEL
	 * register controls pins 0-7 and MODEH controls pins 8-15. */
	if (_pin(gpio) < 8) {
		GPIO->P[_port(gpio)].MODEL = (GPIO->P[_port(gpio)].MODEL
				& ~(0xF << (_pin(gpio) * 4))) | (mode << (_pin(gpio) * 4));
	} else {
		GPIO->P[_port(gpio)].MODEH = (GPIO->P[_port(gpio)].MODEH
				& ~(0xF << ((_pin(gpio) - 8) * 4)))
				| (mode << ((_pin(gpio) - 8) * 4));
	}

	return 0;
}

int gpio_init_int(gpio_t gpio, gpio_pp_t pullup, gpio_flank_t flank,
		gpio_cb_t cb, void *arg) {
	uint32_t tmp = 0;

	gpio_init(gpio, GPIO_DIR_INPUT, pullup);

	/* configure and save exti configuration struct */
	exti_chan[_pin(gpio)].cb = cb;
	exti_chan[_pin(gpio)].arg = arg;
	/* There are two registers controlling the interrupt configuration:
	 * The EXTIPSELL register controls pins 0-7 and EXTIPSELH controls
	 * pins 8-15. */
	if (_pin(gpio) < 8) {
		GPIO->EXTIPSELL = (GPIO->EXTIPSELL & ~(0xF << (4 * _pin(gpio))))
				| (_port(gpio) << (4 * _pin(gpio)));
	} else {
		tmp = _pin(gpio) - 8;
		GPIO->EXTIPSELH = (GPIO->EXTIPSELH & ~(0xF << (4 * tmp)))
				| (_port(gpio) << (4 * tmp));
	}
	/* configure the active edge(s) */
	switch (flank) {
	case GPIO_RISING:
		GPIO->EXTIRISE |= 1 << _pin(gpio);
		break;
	case GPIO_FALLING:
		GPIO->EXTIFALL |= 1 << _pin(gpio);
		break;
	case GPIO_BOTH:
		GPIO->EXTIRISE |= 1 << _pin(gpio);
		GPIO->EXTIFALL |= 1 << _pin(gpio);
		break;
	}
	/* clear any pending requests */
	GPIO->IFC = 1 << _pin(gpio);
	/* enable interrupt for EXTI line */
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	NVIC_SetPriority(GPIO_ODD_IRQn, 0);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_SetPriority(GPIO_EVEN_IRQn, 0);

	return 0;
}

void gpio_init_af(gpio_t pin, gpio_af_t af) {

}

void gpio_irq_enable(gpio_t gpio) {
	GPIO->IFC |= (1 << _pin(gpio));
	GPIO->IEN |= (1 << _pin(gpio));
}

void gpio_irq_disable(gpio_t gpio) {
    GPIO->IFC |= (1 << _pin(gpio));
	GPIO->IEN &= ~(1 <<_pin(gpio));
}

int gpio_read(gpio_t gpio) {
	return GPIO->P[_port(gpio)].DIN & (1 << _pin(gpio));
}

void gpio_set(gpio_t gpio) {
	GPIO->P[_port(gpio)].DOUTSET = (1 << _pin(gpio));
}

void gpio_clear(gpio_t gpio) {
	GPIO->P[_port(gpio)].DOUTCLR = (1 << _pin(gpio));
}

void gpio_toggle(gpio_t gpio) {
	GPIO->P[_port(gpio)].DOUTTGL = (1 << _pin(gpio));
}

void gpio_write(gpio_t pin, int value) {
	if (value) {
		gpio_set(pin);
	} else {
		gpio_clear(pin);
	}
}

void gpio_drive(gpio_port port, gpio_drive_strength mode){
	GPIO->P[port].CTRL = (GPIO->P[port].CTRL & ~(_GPIO_P_CTRL_DRIVEMODE_MASK))
			  | (mode << _GPIO_P_CTRL_DRIVEMODE_SHIFT);
}

void GPIO_EVEN_IRQHandler(void) {
	uint32_t intReg = GPIO->IF & GPIO->IEN;
	for (int i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
		if (intReg & (1 << i)) {
			GPIO->IFC = 1 << i;
			exti_chan[i].cb(exti_chan[i].arg);
		}
	}
	if (sched_context_switch_request) {
		thread_yield();
	}
}

void GPIO_ODD_IRQHandler(void) {
	uint32_t intReg = GPIO->IF & GPIO->IEN;
	for (int i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
		if (intReg & (1 << i)) {
			GPIO->IFC = 1 << i;
			exti_chan[i].cb(exti_chan[i].arg);
		}
	}
	if (sched_context_switch_request) {
		thread_yield();
	}
}
