/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_beaglebone
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Arduino Due board
 *
 * @author      Max Dieckmann <dieckmann@zedat.fu-berlin.de>
 * @author      Martin Kessel <martin.kessel@fu-berlin.de> 
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "consoleUtils.h"

#include "soc_AM335x.h"
#include "watchdog.h"
#include "beaglebone.h"

#include "gpio_v2.h"
#include "periph/gpio.h"

#include "periph_conf.h"
#include "pin_mux.h"


/* user LED config */
#define LED_1_DEV          SOC_GPIO_1_REGS
#define LED_1_OFFSET       GPIO_1_21
#define LED_1_PIN			21

#define LED_2_DEV          SOC_GPIO_1_REGS
#define LED_2_OFFSET       GPIO_1_22
#define LED_2_PIN			22

#define LED_3_DEV          SOC_GPIO_1_REGS
#define LED_3_OFFSET       GPIO_1_23
#define LED_3_PIN			23

#define LED_4_DEV          SOC_GPIO_1_REGS
#define LED_4_OFFSET       GPIO_1_24
#define LED_4_PIN			24


void led_init(void);

extern void kernel_init(void); //to call kernel_init here

extern void __libc_init_array(void);

/**
 * @brief Initialize the boards and start the kernel
 * 
 */
void board_init(void)
{
    
   /* Disable Watchdog Timer to prevent reset after 60 sec */
    WatchdogTimerDisable(SOC_WDT_1_REGS);
  
  
    /* initialize the boards LEDs */
    led_init();

    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array(); // calls unter anderem the _init syscall - what else does it do and where is it defined?
    
    /* kernel initialiation */
    kernel_init();
}


/**
 * @brief Initialize the boards on-board user LEDs
 *
 * The LED initialization is hard-coded in this function. The function will 
 * toggle the LEDs once after their initilization.
 * 
 */
void led_init(void)
{
	/* Init GPIO clock */
	GPIO1ModuleClkConfig();
	
	/* Init GPIO as output pin */
	GpioPinMuxSetup(LED_1_OFFSET, PAD_SL_RXE_NA_PUPDD(7));
	GPIOModuleEnable(LED_1_DEV);
	GPIOModuleReset(LED_1_DEV);
	GPIODirModeSet(LED_1_DEV, LED_1_PIN, GPIO_DIR_OUTPUT);	

	/* Drive a logic high on the pin */
	GPIOPinWrite(LED_1_DEV, LED_1_PIN, GPIO_PIN_HIGH);
	Delay(0xFFFFFF);
	/* Drive a logic low on the pin */	
	GPIOPinWrite(LED_1_DEV, LED_1_PIN, GPIO_PIN_LOW);

	/* Init GPIO as output pin */
	GpioPinMuxSetup(LED_2_OFFSET, PAD_SL_RXE_NA_PUPDD(7));
	GPIOModuleEnable(LED_2_DEV);
	GPIOModuleReset(LED_2_DEV);
	GPIODirModeSet(LED_2_DEV, LED_2_PIN, GPIO_DIR_OUTPUT);	
	/* Drive a logic high on the pin */
	GPIOPinWrite(LED_2_DEV, LED_2_PIN, GPIO_PIN_HIGH);
	Delay(0xFFFFFF);
	/* Drive a logic low on the pin */	
	GPIOPinWrite(LED_2_DEV, LED_2_PIN, GPIO_PIN_LOW);


	/* Init GPIO as output pin */
	GpioPinMuxSetup(LED_3_OFFSET, PAD_SL_RXE_NA_PUPDD(7));
	GPIOModuleEnable(LED_3_DEV);
	GPIOModuleReset(LED_3_DEV);
	GPIODirModeSet(LED_3_DEV, LED_3_PIN, GPIO_DIR_OUTPUT);	
	/* Drive a logic high on the pin */
	GPIOPinWrite(LED_3_DEV, LED_3_PIN, GPIO_PIN_HIGH);
	Delay(0xFFFFFF);
	/* Drive a logic low on the pin */	
	GPIOPinWrite(LED_3_DEV, LED_3_PIN, GPIO_PIN_LOW);


	/* Init GPIO as output pin */
	GpioPinMuxSetup(LED_4_OFFSET, PAD_SL_RXE_NA_PUPDD(7));
	GPIOModuleEnable(LED_4_DEV);
	GPIOModuleReset(LED_4_DEV);
	GPIODirModeSet(LED_4_DEV, LED_4_PIN, GPIO_DIR_OUTPUT);	
	/* Drive a logic high on the pin */
	GPIOPinWrite(LED_4_DEV, LED_4_PIN, GPIO_PIN_HIGH);
	Delay(0xFFFFFF);
	/* Drive a logic low on the pin */	
	GPIOPinWrite(LED_4_DEV, LED_4_PIN, GPIO_PIN_LOW);	
}


