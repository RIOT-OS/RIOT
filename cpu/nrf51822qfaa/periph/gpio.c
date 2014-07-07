/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *
 * This file is subject to the terms and conditions of the GPLv2 License.
 * See the file LICENSE in the top level directory for more details.
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
#include "nrf51.h"
#include "nrf51_bitfields.h"

#define gpio NRF_GPIO

typedef struct {
    void (*cb)(void);
} gpio_state_t;

static gpio_state_t config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:

            //nrf_gpio_cfg_output(GPIO_0);
            GPIO_DEV->PIN_CNF[GPIO_0] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_DEV->PIN_CNF[GPIO_1] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_DEV->PIN_CNF[GPIO_2] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_DEV->PIN_CNF[GPIO_3] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_DEV->PIN_CNF[GPIO_4] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_DEV->PIN_CNF[GPIO_5] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_DEV->PIN_CNF[GPIO_6] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_DEV->PIN_CNF[GPIO_7] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_DEV->PIN_CNF[GPIO_8] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_DEV->PIN_CNF[GPIO_9] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_DEV->PIN_CNF[GPIO_10] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_DEV->PIN_CNF[GPIO_11] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_DEV->PIN_CNF[GPIO_12] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_DEV->PIN_CNF[GPIO_13] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_DEV->PIN_CNF[GPIO_14] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_DEV->PIN_CNF[GPIO_15] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif

        case GPIO_UNDEFINED:
        default:
            return -1;
    }
    switch (pullup) {
        case GPIO_PULLDOWN:
        	GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);
        	break;
            //return -2;
        case GPIO_PULLUP:
        	GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
            break;
        case GPIO_NOPULL:
        	GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
            break;
    }


    return 1;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_DEV->PIN_CNF[dev] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
            break;
#endif

     case GPIO_UNDEFINED:
        default:
            return -1;
    }
    //configure dev as input and pullup as state

    switch (pullup) {
        case GPIO_PULLDOWN:
        	GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);
        	break;
            //return -2;
        case GPIO_PULLUP:
        	GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
            break;
        case GPIO_NOPULL:
        	GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
            break;
    }
    return 1;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, void (*cb)(void))
{

	//Löse Interrupt aus, bei Erkennung der Flanke

	   int res;
	    uint32_t pin = 50;

	    /* configure pin as input */
	    res = gpio_init_in(dev, pullup);
	    if (res < 0) {
	        return res;
	    }


	    switch (dev) {
	#ifdef GPIO_0_EN
	        case GPIO_0:
	            pin = GPIO_0_PIN;
	            //GPIO_0_EXTI_CFG();
//	            NVIC_SetPriority(GPIO_0_IRQ, GPIO_IRQ_PRIO);
//	            NVIC_EnableIRQ(GPIO_0_IRQ);

	            break;
	#endif
	#ifdef GPIO_1_EN
	        case GPIO_1:
	            pin = GPIO_1_PIN;
	            break;
	#endif
	#ifdef GPIO_2_EN
	        case GPIO_2:
	            pin = GPIO_2_PIN;
	            break;
	#endif
	#ifdef GPIO_3_EN
	        case GPIO_3:
	            pin = GPIO_3_PIN;
	            break;
	#endif
	#ifdef GPIO_4_EN
	        case GPIO_4:
	            pin = GPIO_4_PIN;
	            break;
	#endif
	#ifdef GPIO_5_EN
	        case GPIO_5:
	            pin = GPIO_5_PIN;
	            break;
	#endif
	#ifdef GPIO_6_EN
	        case GPIO_6:
	            pin = GPIO_6_PIN;
	            break;
	#endif
	#ifdef GPIO_7_EN
	        case GPIO_7:
	            pin = GPIO_7_PIN;
	            break;
	#endif
	#ifdef GPIO_8_EN
	        case GPIO_8:
	            pin = GPIO_8_PIN;
	            break;
	#endif
	#ifdef GPIO_9_EN
	        case GPIO_9:
	            pin = GPIO_9_PIN;
	            break;
	#endif
	#ifdef GPIO_10_EN
	        case GPIO_10:
	            pin = GPIO_10_PIN;
	            break;
	#endif
	#ifdef GPIO_11_EN
	        case GPIO_11:
	            pin = GPIO_11_PIN;
	            break;
	#endif
	        case GPIO_UNDEFINED:
	        default:
	            return -1;
	    }


	    if(pin == 50){
	    	return -1;
	    }


	   NVIC_SetPriority(GPIOTE_IRQn, GPIO_IRQ_PRIO);
	   NVIC_EnableIRQ(GPIOTE_IRQn);
	    /* save callback */
	   config[dev].cb = cb;

	    gpio_init_in(pin,pullup);
	    GPIO_IRQ_DEV->CONFIG[0] =  (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos)
	                           | (pin << GPIOTE_CONFIG_PSEL_Pos)
	                           | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);

	    GPIO_IRQ_DEV->INTENSET  = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;



    return 1;
}

int gpio_read(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
        	return  ((GPIO_DEV->IN >> GPIO_0) & 1UL);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
        	return  ((GPIO_DEV->IN >> GPIO_1) & 1UL);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
        	return  ((GPIO_DEV->IN >> GPIO_2) & 1UL);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
        	return  ((GPIO_DEV->IN >> GPIO_3) & 1UL);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
        	return  ((GPIO_DEV->IN >> GPIO_4) & 1UL);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
        	return  ((GPIO_DEV->IN >> GPIO_5) & 1UL);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
        	return  ((GPIO_DEV->IN >> GPIO_6) & 1UL);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
        	return  ((GPIO_DEV->IN >> GPIO_7) & 1UL);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
        	return  ((GPIO_DEV->IN >> GPIO_8) & 1UL);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
        	return  ((GPIO_DEV->IN >> GPIO_9) & 1UL);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
        	return  ((GPIO_DEV->IN >> GPIO_10) & 1UL);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
        	return  ((GPIO_DEV->IN >> GPIO_11) & 1UL);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
        	return  ((GPIO_DEV->IN >> GPIO_12) & 1UL);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
        	return  ((GPIO_DEV->IN >> GPIO_13) & 1UL);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
        	return  ((GPIO_DEV->IN >> GPIO_14) & 1UL);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
        	return  ((GPIO_DEV->IN >> GPIO_15) & 1UL);
            break;
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
        	GPIO_DEV->OUTSET = (1UL << GPIO_0);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
        	GPIO_DEV->OUTSET = (1UL << GPIO_1);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
        	GPIO_DEV->OUTSET = (1UL << GPIO_2);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
        	GPIO_DEV->OUTSET = (1UL << GPIO_3);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
        	GPIO_DEV->OUTSET = (1UL << GPIO_4);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
        	GPIO_DEV->OUTSET = (1UL << GPIO_5);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
        	GPIO_DEV->OUTSET = (1UL << GPIO_6);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
        	GPIO_DEV->OUTSET = (1UL << GPIO_7);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
        	GPIO_DEV->OUTSET = (1UL << GPIO_8);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
        	GPIO_DEV->OUTSET = (1UL << GPIO_9);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
        	GPIO_DEV->OUTSET = (1UL << GPIO_10);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
        	GPIO_DEV->OUTSET = (1UL << GPIO_11);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
        	GPIO_DEV->OUTSET = (1UL << GPIO_12);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
        	GPIO_DEV->OUTSET = (1UL << GPIO_13);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
        	GPIO_DEV->OUTSET = (1UL << GPIO_14);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
        	GPIO_DEV->OUTSET = (1UL << GPIO_15);
            break;
#endif

        case GPIO_UNDEFINED:
        default:
            return -1;
    }
    return 1;
}

int gpio_clear(gpio_t dev)
{
       switch (dev) {
    #if GPIO_0_EN
            case GPIO_0:
                GPIO_DEV->OUTCLR = (1UL << GPIO_0);
                break;
    #endif
    #if GPIO_1_EN
            case GPIO_1:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_1);
                break;
    #endif
    #if GPIO_2_EN
            case GPIO_2:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_2);
                break;
    #endif
    #if GPIO_3_EN
            case GPIO_3:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_3);
                break;
    #endif
    #if GPIO_4_EN
            case GPIO_4:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_4);
                break;
    #endif
    #if GPIO_5_EN
            case GPIO_5:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_5);
                break;
    #endif
    #if GPIO_6_EN
            case GPIO_6:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_6);
                break;
    #endif
    #if GPIO_7_EN
            case GPIO_7:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_7);
                break;
    #endif
    #if GPIO_8_EN
            case GPIO_8:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_8);
                break;
    #endif
    #if GPIO_9_EN
            case GPIO_9:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_9);
                break;
    #endif
    #if GPIO_10_EN
            case GPIO_10:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_10);
                break;
    #endif
    #if GPIO_11_EN
            case GPIO_11:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_11);
                break;
    #endif
    #if GPIO_12_EN
            case GPIO_12:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_12);
                break;
    #endif
    #if GPIO_13_EN
            case GPIO_13:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_13);
                break;
    #endif
    #if GPIO_14_EN
            case GPIO_14:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_14);
                break;
    #endif
    #if GPIO_15_EN
            case GPIO_15:
            	GPIO_DEV->OUTCLR = (1UL << GPIO_15);
                break;
    #endif

            case GPIO_UNDEFINED:
            default:
                return -1;
        }
    return 1;
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


__attribute__((naked)) void EXTI0_0_IRQn(void)
{
    ISR_ENTER();
	    // Event causing the interrupt must be cleared.
	    if ((GPIO_IRQ_DEV->EVENTS_IN[0] == 1) &&
	        (GPIO_IRQ_DEV->INTENSET & GPIOTE_INTENSET_IN0_Msk))
	    {
	    	GPIO_IRQ_DEV->EVENTS_IN[0] = 0;
	    }

	    config[GPIO_IRQ_0].cb();
	    ISR_EXIT();
}

__attribute__((naked)) void EXTI0_1_IRQn(void)
{
    ISR_ENTER();
	    // Event causing the interrupt must be cleared.
	    if ((GPIO_IRQ_DEV->EVENTS_IN[0] == 1) &&
	        (GPIO_IRQ_DEV->INTENSET & GPIOTE_INTENSET_IN0_Msk))
	    {
	    	GPIO_IRQ_DEV->EVENTS_IN[0] = 0;
	    }

	    config[GPIO_IRQ_1].cb();
	    ISR_EXIT();
}


