

#ifndef BOARD_H
#define BOARD_H

#include "em_device.h"
#include "em_gpio.h"

/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               (24000000UL)

/**
 * @name Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 */
#define STDIO               UART_0

/**
 * @name LED pin definitions
 * @{
 */

#define LED_0_PORT				gpioPortC
#define LED_0_PIN				0
#define LED_1_PORT				gpioPortC
#define LED_1_PIN				1
#define LED_2_PORT				gpioPortC
#define LED_2_PIN				2
#define LED_3_PORT				gpioPortC
#define LED_3_PIN				3


/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_GREEN_ON        GPIO->P[LED_0_PORT].DOUTSET = 1 << LED_0_PIN;
#define LED_GREEN_OFF       GPIO->P[LED_0_PORT].DOUTCLR = 1 << LED_0_PIN;
#define LED_GREEN_TOGGLE    GPIO->P[LED_0_PORT].DOUTTGL = 1 << LED_0_PIN;
#define LED_RED_ON          GPIO->P[LED_1_PORT].DOUTSET = 1 << LED_1_PIN;
#define LED_RED_OFF         GPIO->P[LED_1_PORT].DOUTCLR = 1 << LED_1_PIN;
#define LED_RED_TOGGLE      GPIO->P[LED_1_PORT].DOUTTGL = 1 << LED_1_PIN;
/** @} */

#define LED_2_ON        	GPIO->P[LED_2_PORT].DOUTSET = 1 << LED_2_PIN;
#define LED_2_OFF       	GPIO->P[LED_2_PORT].DOUTCLR = 1 << LED_2_PIN;
#define LED_2_TOGGLE    	GPIO->P[LED_2_PORT].DOUTTGL = 1 << LED_2_PIN;
#define LED_3_ON          	GPIO->P[LED_3_PORT].DOUTSET = 1 << LED_3_PIN;
#define LED_3_OFF         	GPIO->P[LED_3_PORT].DOUTCLR = 1 << LED_3_PIN;
#define LED_3_TOGGLE      	GPIO->P[LED_3_PORT].DOUTTGL = 1 << LED_3_PIN;

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif
