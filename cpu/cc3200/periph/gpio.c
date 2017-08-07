/*
 * Copyright (C) 2015 Attilio Dona'
 *
 ***********************************************************************
 Derived from:

 WInterrupts.c Part of the Wiring project - http://wiring.uniandes.edu.co

 Original work Copyright (c) 2004-05 Hernando Barragan

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General
 Public License along with this library; if not, write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA  02111-1307  USA

 Modified 24 November 2006 by David A. Mellis
 Modified 1 August 2010 by Mark Sproul
 */

/**
 * @ingroup     driver_cc3200
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Attilio Dona' <attilio.dona>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "board.h"
#include "periph_conf.h"


/**
 * PIN value high IRQ trigger
 */
#define HIGH 1

/**
 * PIN value low IRQ trigger
 */
#define LOW  0

/**
 * @brief Generate a bit mask in which only the specified bit is high.
 *
 * @param[in] n Number of the bit to set high in the mask.
 *
 * @return A bit mask in which bit n is high.
 */
#ifndef BV
#define BV(x) (1 << (x))
#endif

#define NOT_A_PORT 0
#define NOT_A_PIN 0

#define S0 1
#define S1 2
#define S2 3
#define S3 4
#define S4 5

const uint32_t port_to_base[] = {
		NOT_A_PORT,
		(uint32_t) GPIOA0_BASE,
		(uint32_t) GPIOA1_BASE,
		(uint32_t) GPIOA2_BASE,
		(uint32_t) GPIOA3_BASE, };

const uint16_t digital_pin_to_pin_num[] = {
		NOT_A_PIN, /*  dummy */
		NOT_A_PIN, /*  1  - 3.3V */
		PIN_58, /*  2  - GPIO_03 */
		PIN_04, /*  3  - GPIO_13 */
		PIN_03, /*  4  - GPIO_12 */
		PIN_61, /*  5  - GPIO_06 */
		PIN_59, /*  6  - GPIO_04 */
		PIN_05, /*  7  - GPIO_14 */
		PIN_62, /*  8  - GPIO_07 */
		PIN_01, /*  9  - GPIO_10 */
		PIN_02, /*  10 - GPIO_11 */
		PIN_15, /*  11 - GPIO_22 */
		PIN_55, /*  12 - GPIO_01 */
		PIN_21, /*  13 - GPIO_25 */
		PIN_06, /*  14 - GPIO_15 */
		PIN_07, /*  15 - GPIO_16 */
		NOT_A_PIN, /*  16 - RESET */
		PIN_45, /*  17 - GPIO_31 */
		PIN_08, /*  18 - GPIO_17 */
		PIN_18, /*  19 - GPIO_28 */
		NOT_A_PIN, /*  20 - GND */
		NOT_A_PIN, /*  21 - 5V */
		NOT_A_PIN, /*  22 - GND */
		PIN_57, /*  23 - GPIO_02 */
		PIN_60, /*  24 - GPIO_05 */
		PIN_58, /*  25 - GPIO_03 */
		PIN_59, /*  26 - GPIO_04 */
		PIN_63, /*  27 - GPIO_08 */
		PIN_53, /*  28 - GPIO_30 */
		PIN_64, /*  29 - GPIO_09 */
		PIN_50, /*  30 - GPIO_00 */
		PIN_17, /*  31 - GPIO_24 */
		PIN_16, /*  32 - GPIO_23 */
		PIN_60, /*  33 - GPIO_05 */
		PIN_62, /*  34 - GPIO_07 */
		PIN_18, /*  35 - GPIO_28 */
		PIN_21, /*  36 - GPIO_25 */
		PIN_64, /*  37 - GPIO_09 */
		PIN_17, /*  38 - GPIO_24 */
		PIN_01, /*  39 - GPIO_10 */
		PIN_02 /*  40 - GPIO_11 */
};

const uint8_t digital_pin_to_port[] = {
		NOT_A_PIN, /*  dummy */
		NOT_A_PIN, /*  1  - 3.3V */
		S0, /*  2  - GPIO_03 */
		S1, /*  3  - GPIO_13 */
		S1, /*  4  - GPIO_12 */
		S0, /*  5  - GPIO_06 */
		S0, /*  6  - GPIO_04 */
		S1, /*  7  - GPIO_14 */
		S0, /*  8  - GPIO_07 */
		S1, /*  9  - GPIO_10 */
		S1, /*  10 - GPIO_11 */
		S2, /*  11 - GPIO_22 */
		S0, /*  12 - GPIO_01 */
		S3, /*  13 - GPIO_25 */
		S1, /*  14 - GPIO_15 */
		S2, /*  15 - GPIO_16 */
		NOT_A_PIN, /*  16 - RESET */
		S3, /*  17 - GPIO_31 */
		S2, /*  18 - GPIO_17 */
		S3, /*  19 - GPIO_28 */
		NOT_A_PIN, /*  20 - GND */
		NOT_A_PIN, /*  21 - 5V */
		NOT_A_PIN, /*  22 - GND */
		S0, /*  23 - GPIO_02 */
		S0, /*  24 - GPIO_05 */
		S0, /*  25 - GPIO_03 */
		S0, /*  26 - GPIO_04 */
		S1, /*  27 - GPIO_08 */
		S3, /*  28 - GPIO_30 */
		S1, /*  29 - GPIO_09 */
		S0, /*  30 - GPIO_00 */
		S3, /*  31 - GPIO_24 */
		S2, /*  32 - GPIO_23 */
		S0, /*  33 - GPIO_05 */
		S0, /*  34 - GPIO_07 */
		S3, /*  35 - GPIO_28 */
		S3, /*  36 - GPIO_25 */
		S1, /*  37 - GPIO_09 */
		S3, /*  38 - GPIO_24 */
		S1, /*  39 - GPIO_10 */
		S1 /*  40 - GPIO_11 */
};

const uint8_t digital_pin_to_bit_mask[] = {
		NOT_A_PIN, /*  dummy */
		NOT_A_PIN, /*  1  - 3.3V */
		BV(3), /*  2  - GPIO_03 */
		BV(5), /*  3  - GPIO_13 */
		BV(4), /*  4  - GPIO_12 */
		BV(6), /*  5  - GPIO_06 */
		BV(4), /*  6  - GPIO_04 */
		BV(6), /*  7  - GPIO_14 */
		BV(7), /*  8  - GPIO_07 */
		BV(2), /*  9  - GPIO_10 */
		BV(3), /*  10 - GPIO_11 */
		BV(6), /*  11 - GPIO_22 */
		BV(1), /*  12 - GPIO_01 */
		BV(1), /*  13 - GPIO_25 */
		BV(7), /*  14 - GPIO_15 */
		BV(0), /*  15 - GPIO_16 */
		NOT_A_PIN, /*  16 - RESET */
		BV(7), /*  17 - GPIO_31 */
		BV(1), /*  18 - GPIO_17 */
		BV(4), /*  19 - GPIO_28 */
		NOT_A_PIN, /*  20 - GND */
		NOT_A_PIN, /*  21 - 5V */
		NOT_A_PIN, /*  22 - GND */
		BV(2), /*  23 - GPIO_02 */
		BV(5), /*  24 - GPIO_05 */
		BV(3), /*  25 - GPIO_03 */
		BV(4), /*  26 - GPIO_04 */
		BV(0), /*  27 - GPIO_08 */
		BV(6), /*  28 - GPIO_30 */
		BV(1), /*  29 - GPIO_09 */
		BV(0), /*  30 - GPIO_00 */
		BV(0), /*  31 - GPIO_24 */
		BV(7), /*  32 - GPIO_23 */
		BV(5), /*  33 - GPIO_05 */
		BV(7), /*  34 - GPIO_07 */
		BV(4), /*  35 - GPIO_28 */
		BV(1), /*  36 - GPIO_25 */
		BV(1), /*  37 - GPIO_09 */
		BV(0), /*  38 - GPIO_24 */
		BV(2), /*  39 - GPIO_10 */
		BV(3) /*  40 - GPIO_11 */
};

int gpio_init(gpio_t dev, gpio_mode_t mode) {
	uint8_t bit = digitalPinToBitMask(dev);
	uint8_t port = digitalPinToPort(dev);

	if (port == NOT_A_PORT)
		return -1;

	uint32_t portBase = (uint32_t) portBASERegister(port);
	uint16_t pinNum = digitalPinToPinNum(dev);

	MAP_PinTypeGPIO(pinNum, PIN_MODE_0, false);

	if (mode == GPIO_OUT) {
		MAP_GPIODirModeSet(portBase, bit, GPIO_DIR_MODE_OUT);
	} else {
		MAP_GPIODirModeSet(portBase, bit, GPIO_DIR_MODE_IN);
	}

	switch (mode) {
	case GPIO_IN:
	case GPIO_OUT:
	case GPIO_OD:
		MAP_PinConfigSet(pinNum, PIN_STRENGTH_2MA, PIN_TYPE_STD);
		break;
	case GPIO_OD_PU:
	case GPIO_IN_PU:
		MAP_PinConfigSet(pinNum, PIN_STRENGTH_2MA, PIN_TYPE_STD_PU);
		break;
	case GPIO_IN_PD:
		MAP_PinConfigSet(pinNum, PIN_STRENGTH_2MA, PIN_TYPE_STD_PD);
		break;
	}

	return 0;
}

static void (*cbFuncsA0[8])(void* arg);
static void (*cbFuncsA1[8])(void* arg);
static void (*cbFuncsA2[8])(void* arg);
static void (*cbFuncsA3[8])(void* arg);

static void* cbArgA0[8];
static void* cbArgA1[8];
static void* cbArgA2[8];
static void* cbArgA3[8];

void GPIOXIntHandler(uint32_t base, void (**funcs)(void*), void **arg) {
	uint32_t i;
	uint32_t isr = MAP_GPIOIntStatus(base, true);

	MAP_GPIOIntClear(base, isr);

	for (i = 0; i < 8; i++, isr >>= 1) {
		if ((isr & 0x1) == 0)
			continue;
		if (funcs[i]) {
			funcs[i](arg[i]);
			if (sched_context_switch_request) {
				thread_yield();
			}
		}
	}
}

void GPIOA0IntHandler(void) {
	GPIOXIntHandler(GPIOA0_BASE, cbFuncsA0, cbArgA0);
}

void GPIOA1IntHandler(void) {
	GPIOXIntHandler(GPIOA1_BASE, cbFuncsA1, cbArgA1);
}

void GPIOA2IntHandler(void) {
	GPIOXIntHandler(GPIOA2_BASE, cbFuncsA2, cbArgA2);
}

void GPIOA3IntHandler(void) {
	GPIOXIntHandler(GPIOA3_BASE, cbFuncsA3, cbArgA3);
}

int gpio_init_int(gpio_t dev, gpio_mode_t mode, gpio_flank_t flank,
		gpio_cb_t cb, void *arg) {
	uint32_t cc3200Mode, res, i;

	uint8_t bit = digitalPinToBitMask(dev);
	uint8_t port = digitalPinToPort(dev);
	uint32_t portBase = (uint32_t) portBASERegister(port);

	/* Note: gpio_init() also checks if the gpio is enabled. */
	res = gpio_init(dev, mode);
	if (res < 0) {
		return res;
	}

	switch (flank) {
#if 0
	// not supported by RIOT
	case LOW:
	cc3200Mode = GPIO_LOW_LEVEL;
	break;
#endif
	case GPIO_BOTH:
		cc3200Mode = GPIO_BOTH_EDGES;
		break;
	case GPIO_RISING:
		cc3200Mode = GPIO_RISING_EDGE;
		break;
	case GPIO_FALLING:
		cc3200Mode = GPIO_FALLING_EDGE;
		break;
	default:
		return -1;
	}

	MAP_IntMasterDisable();
	MAP_GPIOIntClear(portBase, bit);
	MAP_GPIOIntTypeSet(portBase, bit, cc3200Mode);
	MAP_GPIOIntEnable(portBase, bit);

	for (i = 0; i < 8; i++, bit >>= 1) {
		if ((bit & 0x1) == 1)
			break;
	}

	switch (portBase) {
	case GPIOA0_BASE:
		cbFuncsA0[i] = cb;
		cbArgA0[i] = arg;
		MAP_GPIOIntRegister(GPIOA0_BASE, GPIOA0IntHandler);
		MAP_IntEnable(INT_GPIOA0);
		break;
	case GPIOA1_BASE:
		cbFuncsA1[i] = cb;
		cbArgA1[i] = arg;
		MAP_GPIOIntRegister(GPIOA1_BASE, GPIOA1IntHandler);
		MAP_IntEnable(INT_GPIOA1);
		break;
	case GPIOA2_BASE:
		cbFuncsA2[i] = cb;
		cbArgA2[i] = arg;
		MAP_GPIOIntRegister(GPIOA2_BASE, GPIOA2IntHandler);
		MAP_IntEnable(INT_GPIOA2);
		break;
	case GPIOA3_BASE:
		cbFuncsA3[i] = cb;
		cbArgA3[i] = arg;
		MAP_GPIOIntRegister(GPIOA3_BASE, GPIOA3IntHandler);
		MAP_IntEnable(INT_GPIOA3);
		break;
	}

	MAP_IntMasterEnable();

	return 0;
}

void gpio_irq_enable(gpio_t dev) {
	uint8_t bit = digitalPinToBitMask(dev);
	uint8_t port = digitalPinToPort(dev);
	uint32_t portBase = (uint32_t) portBASERegister(port);

	if (port == NOT_A_PIN)
		return;

	MAP_GPIOIntEnable(portBase, bit);
}

void gpio_irq_disable(gpio_t dev) {
	uint8_t bit = digitalPinToBitMask(dev);
	uint8_t port = digitalPinToPort(dev);
	uint32_t portBase = (uint32_t) portBASERegister(port);

	if (port == NOT_A_PIN)
		return;

	MAP_GPIOIntDisable(portBase, bit);
}

int gpio_read(gpio_t dev) {
	uint8_t bit = digitalPinToBitMask(dev);
	uint8_t port = digitalPinToPort(dev);
	uint32_t portBase = (uint32_t) portBASERegister(port);

	if (port == NOT_A_PORT)
		return LOW;

	if (MAP_GPIOPinRead(portBase, bit)) {
		return HIGH;
	}

	return LOW;
}

void gpio_set(gpio_t dev) {
	gpio_write(dev, HIGH);
}

void gpio_clear(gpio_t dev) {
	gpio_write(dev, LOW);
}

void gpio_toggle(gpio_t dev) {
	if (gpio_read(dev)) {
		gpio_clear(dev);
	} else {
		gpio_set(dev);
	}
}

void gpio_write(gpio_t dev, int value) {
	uint8_t bit = digitalPinToBitMask(dev);
	uint8_t mask = value ? bit : 0;
	uint8_t port = digitalPinToPort(dev);

	if (port == NOT_A_PORT)
		return;

	uint32_t portBase = (uint32_t) portBASERegister(port);

	MAP_GPIOPinWrite(portBase, bit, mask);
}
