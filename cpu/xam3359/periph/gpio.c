/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "pin_mux.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"

typedef struct {
    void (*cb)(void);
} gpio_state_t;

// static gpio_state_t config[GPIO_NUMOF];



int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
	
	unsigned int pad_conf_value;
	switch (pushpull) {
        case GPIO_PULLDOWN:
            return -2;
        case GPIO_PULLUP:
            pad_conf_value = PAD_SL_RXE_PU_PUPDE(7);
            break;
        case GPIO_NOPULL:
            pad_conf_value = PAD_SL_RXE_NA_PUPDD(7);
            break;
    }

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
			GpioPinMuxSetup(GPIO_0_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_0_DEV);
			GPIOModuleReset(GPIO_0_DEV);
			GPIODirModeSet(GPIO_0_DEV, GPIO_0_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
			GpioPinMuxSetup(GPIO_1_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_1_DEV);
			GPIOModuleReset(GPIO_1_DEV);
			GPIODirModeSet(GPIO_1_DEV, GPIO_1_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
			GpioPinMuxSetup(GPIO_2_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_2_DEV);
			GPIOModuleReset(GPIO_2_DEV);
			GPIODirModeSet(GPIO_2_DEV, GPIO_2_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
			GpioPinMuxSetup(GPIO_3_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_3_DEV);
			GPIOModuleReset(GPIO_3_DEV);
			GPIODirModeSet(GPIO_3_DEV, GPIO_3_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
			GpioPinMuxSetup(GPIO_4_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_4_DEV);
			GPIOModuleReset(GPIO_4_DEV);
			GPIODirModeSet(GPIO_4_DEV, GPIO_4_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
			GpioPinMuxSetup(GPIO_5_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_5_DEV);
			GPIOModuleReset(GPIO_5_DEV);
			GPIODirModeSet(GPIO_5_DEV, GPIO_5_PIN, GPIO_DIR_OUTPUT);			
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
			GpioPinMuxSetup(GPIO_6_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_6_DEV);
			GPIOModuleReset(GPIO_6_DEV);
			GPIODirModeSet(GPIO_6_DEV, GPIO_6_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
			GpioPinMuxSetup(GPIO_7_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_7_DEV);
			GPIOModuleReset(GPIO_7_DEV);
			GPIODirModeSet(GPIO_7_DEV, GPIO_7_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
			GpioPinMuxSetup(GPIO_8_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_8_DEV);
			GPIOModuleReset(GPIO_8_DEV);
			GPIODirModeSet(GPIO_8_DEV, GPIO_8_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
			GpioPinMuxSetup(GPIO_9_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_9_DEV);
			GPIOModuleReset(GPIO_9_DEV);
			GPIODirModeSet(GPIO_9_DEV, GPIO_9_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
			GpioPinMuxSetup(GPIO_10_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_10_DEV);
			GPIOModuleReset(GPIO_10_DEV);
			GPIODirModeSet(GPIO_10_DEV, GPIO_10_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
			GpioPinMuxSetup(GPIO_11_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_11_DEV);
			GPIOModuleReset(GPIO_11_DEV);
			GPIODirModeSet(GPIO_11_DEV, GPIO_11_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
			GpioPinMuxSetup(GPIO_12_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_12_DEV);
			GPIOModuleReset(GPIO_12_DEV);
			GPIODirModeSet(GPIO_12_DEV, GPIO_12_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
			GpioPinMuxSetup(GPIO_13_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_13_DEV);
			GPIOModuleReset(GPIO_13_DEV);
			GPIODirModeSet(GPIO_13_DEV, GPIO_13_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
			GpioPinMuxSetup(GPIO_14_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_14_DEV);
			GPIOModuleReset(GPIO_14_DEV);
			GPIODirModeSet(GPIO_14_DEV, GPIO_14_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
			GpioPinMuxSetup(GPIO_15_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_15_DEV);
			GPIOModuleReset(GPIO_15_DEV);
			GPIODirModeSet(GPIO_15_DEV, GPIO_15_PIN, GPIO_DIR_OUTPUT);	
            break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
    return 1;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
	
	unsigned int pad_conf_value;
	switch (pushpull) {
        case GPIO_PULLDOWN:
            return -2;
        case GPIO_PULLUP:
            pad_conf_value = PAD_SL_RXE_PU_PUPDE(7);
            break;
        case GPIO_NOPULL:
            pad_conf_value = PAD_SL_RXE_NA_PUPDD(7);
            break;
    }

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
			GpioPinMuxSetup(GPIO_0_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_0_DEV);
			GPIOModuleReset(GPIO_0_DEV);
			GPIODirModeSet(GPIO_0_DEV, GPIO_0_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
			GpioPinMuxSetup(GPIO_1_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_1_DEV);
			GPIOModuleReset(GPIO_1_DEV);
			GPIODirModeSet(GPIO_1_DEV, GPIO_1_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
			GpioPinMuxSetup(GPIO_2_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_2_DEV);
			GPIOModuleReset(GPIO_2_DEV);
			GPIODirModeSet(GPIO_2_DEV, GPIO_2_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
			GpioPinMuxSetup(GPIO_3_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_3_DEV);
			GPIOModuleReset(GPIO_3_DEV);
			GPIODirModeSet(GPIO_3_DEV, GPIO_3_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
			GpioPinMuxSetup(GPIO_4_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_4_DEV);
			GPIOModuleReset(GPIO_4_DEV);
			GPIODirModeSet(GPIO_4_DEV, GPIO_4_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
			GpioPinMuxSetup(GPIO_5_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_5_DEV);
			GPIOModuleReset(GPIO_5_DEV);
			GPIODirModeSet(GPIO_5_DEV, GPIO_5_PIN, GPIO_DIR_INPUT);			
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
			GpioPinMuxSetup(GPIO_6_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_6_DEV);
			GPIOModuleReset(GPIO_6_DEV);
			GPIODirModeSet(GPIO_6_DEV, GPIO_6_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
			GpioPinMuxSetup(GPIO_7_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_7_DEV);
			GPIOModuleReset(GPIO_7_DEV);
			GPIODirModeSet(GPIO_7_DEV, GPIO_7_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
			GpioPinMuxSetup(GPIO_8_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_8_DEV);
			GPIOModuleReset(GPIO_8_DEV);
			GPIODirModeSet(GPIO_8_DEV, GPIO_8_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
			GpioPinMuxSetup(GPIO_9_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_9_DEV);
			GPIOModuleReset(GPIO_9_DEV);
			GPIODirModeSet(GPIO_9_DEV, GPIO_9_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
			GpioPinMuxSetup(GPIO_10_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_10_DEV);
			GPIOModuleReset(GPIO_10_DEV);
			GPIODirModeSet(GPIO_10_DEV, GPIO_10_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
			GpioPinMuxSetup(GPIO_11_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_11_DEV);
			GPIOModuleReset(GPIO_11_DEV);
			GPIODirModeSet(GPIO_11_DEV, GPIO_11_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
			GpioPinMuxSetup(GPIO_12_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_12_DEV);
			GPIOModuleReset(GPIO_12_DEV);
			GPIODirModeSet(GPIO_12_DEV, GPIO_12_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
			GpioPinMuxSetup(GPIO_13_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_13_DEV);
			GPIOModuleReset(GPIO_13_DEV);
			GPIODirModeSet(GPIO_13_DEV, GPIO_13_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
			GpioPinMuxSetup(GPIO_14_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_14_DEV);
			GPIOModuleReset(GPIO_14_DEV);
			GPIODirModeSet(GPIO_14_DEV, GPIO_14_PIN, GPIO_DIR_INPUT);	
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
			GpioPinMuxSetup(GPIO_15_OFFSET, pad_conf_value);
            GPIOModuleEnable(GPIO_15_DEV);
			GPIOModuleReset(GPIO_15_DEV);
			GPIODirModeSet(GPIO_15_DEV, GPIO_15_PIN, GPIO_DIR_INPUT);	
            break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
    return 1;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pushpull, gpio_flank_t flank, void (*cb)(void))
{
    // TODO implement
    return -3;
}

int gpio_read(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            return GPIOPinRead(GPIO_0_DEV, GPIO_0_PIN);
#endif
#if GPIO_1_EN
        case GPIO_1:
            return GPIOPinRead(GPIO_1_DEV, GPIO_1_PIN);
#endif
#if GPIO_2_EN
        case GPIO_2:
            return GPIOPinRead(GPIO_2_DEV, GPIO_2_PIN);
#endif
#if GPIO_3_EN
        case GPIO_3:
            return GPIOPinRead(GPIO_3_DEV, GPIO_3_PIN);
#endif
#if GPIO_4_EN
        case GPIO_4:
            return GPIOPinRead(GPIO_4_DEV, GPIO_4_PIN);
#endif
#if GPIO_5_EN
        case GPIO_5:
            return GPIOPinRead(GPIO_5_DEV, GPIO_5_PIN);			
#endif
#if GPIO_6_EN
        case GPIO_6:
            return GPIOPinRead(GPIO_6_DEV, GPIO_6_PIN);			
#endif
#if GPIO_7_EN
        case GPIO_7:
            return GPIOPinRead(GPIO_7_DEV, GPIO_7_PIN);			
#endif
#if GPIO_8_EN
        case GPIO_8:
            return GPIOPinRead(GPIO_8_DEV, GPIO_8_PIN);			
#endif
#if GPIO_9_EN
        case GPIO_9:
            return GPIOPinRead(GPIO_9_DEV, GPIO_9_PIN);			
#endif
#if GPIO_10_EN
        case GPIO_10:
            return GPIOPinRead(GPIO_10_DEV, GPIO_10_PIN);			
#endif
#if GPIO_11_EN
        case GPIO_11:
            return GPIOPinRead(GPIO_11_DEV, GPIO_11_PIN);			
#endif
#if GPIO_12_EN
        case GPIO_12:
            return GPIOPinRead(GPIO_12_DEV, GPIO_12_PIN);			
#endif
#if GPIO_13_EN
        case GPIO_13:
            return GPIOPinRead(GPIO_13_DEV, GPIO_13_PIN);			
#endif
#if GPIO_14_EN
        case GPIO_14:
            return GPIOPinRead(GPIO_14_DEV, GPIO_14_PIN);			
#endif
#if GPIO_15_EN
        case GPIO_15:
            return GPIOPinRead(GPIO_15_DEV, GPIO_15_PIN);			
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
}

int gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIOPinWrite(GPIO_0_DEV, GPIO_0_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIOPinWrite(GPIO_1_DEV, GPIO_1_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIOPinWrite(GPIO_2_DEV, GPIO_2_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIOPinWrite(GPIO_3_DEV, GPIO_3_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIOPinWrite(GPIO_4_DEV, GPIO_4_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIOPinWrite(GPIO_5_DEV, GPIO_5_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIOPinWrite(GPIO_6_DEV, GPIO_6_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIOPinWrite(GPIO_7_DEV, GPIO_7_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIOPinWrite(GPIO_8_DEV, GPIO_8_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIOPinWrite(GPIO_9_DEV, GPIO_9_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIOPinWrite(GPIO_10_DEV, GPIO_10_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIOPinWrite(GPIO_11_DEV, GPIO_11_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIOPinWrite(GPIO_12_DEV, GPIO_12_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIOPinWrite(GPIO_13_DEV, GPIO_13_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIOPinWrite(GPIO_14_DEV, GPIO_14_PIN, GPIO_PIN_HIGH);
			break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIOPinWrite(GPIO_15_DEV, GPIO_15_PIN, GPIO_PIN_HIGH);
			break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
	return 0;
}

int gpio_clear(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIOPinWrite(GPIO_0_DEV, GPIO_0_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIOPinWrite(GPIO_1_DEV, GPIO_1_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIOPinWrite(GPIO_2_DEV, GPIO_2_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIOPinWrite(GPIO_3_DEV, GPIO_3_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIOPinWrite(GPIO_4_DEV, GPIO_4_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIOPinWrite(GPIO_5_DEV, GPIO_5_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIOPinWrite(GPIO_6_DEV, GPIO_6_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIOPinWrite(GPIO_7_DEV, GPIO_7_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIOPinWrite(GPIO_8_DEV, GPIO_8_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIOPinWrite(GPIO_9_DEV, GPIO_9_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIOPinWrite(GPIO_10_DEV, GPIO_10_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIOPinWrite(GPIO_11_DEV, GPIO_11_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIOPinWrite(GPIO_12_DEV, GPIO_12_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIOPinWrite(GPIO_13_DEV, GPIO_13_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIOPinWrite(GPIO_14_DEV, GPIO_14_PIN, GPIO_PIN_LOW);
			break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIOPinWrite(GPIO_15_DEV, GPIO_15_PIN, GPIO_PIN_LOW);
			break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
	return 0;
}

int gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        return gpio_clear(dev);
    } else {
        return gpio_set(dev);
    }
}

int gpio_write(gpio_t dev, int value)
{
    if (value) {
        return gpio_set(dev);
    } else {
        return gpio_clear(dev);
    }
}
