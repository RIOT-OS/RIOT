

#ifndef BOARD_H
#define BOARD_H

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
#define LED_0_PIN				10
#define LED_1_PORT				gpioPortC
#define LED_1_PIN				11

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

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif
