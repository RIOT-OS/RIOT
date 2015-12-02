/**
 * @defgroup    boards_rf_ism_mod_2_1 rf_ism_mod_2_1
 * @ingroup     boards
 * @brief       Board specific files for the rf_ism_mod_2_1 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the rf_ism_mod_2_1 evaluation board
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @brief xtimer configuration values
 * @{
 */
#define XTIMER              		TIMER_0
#define XTIMER_CHAN					(0)
#define XTIMER_SHIFT                (5)
#define XTIMER_SHIFT_ON_COMPARE     (0)
#define XTIMER_BACKOFF              (40)

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_2
#define STDIO_BAUDRATE      (9600)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Define CC1125 connection.
 * @{
 */

#define SPI_CC1125			SPI_2

/** @} */

/**
 * @name Define SPI connection.
 * @{
 */

#define SPI_EXT			SPI_0

/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_RED_PORT	GPIO_PORT_D
#define LED_GREEN_PORT	GPIO_PORT_A
#define LED_YELLOW_PORT	GPIO_PORT_B
#define LED_RED_PIN		14
#define LED_GREEN_PIN	7
#define LED_YELLOW_PIN	1

/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */


/* for compatability to other boards */
#define LED_RED_ON          {GPIO->P[LED_RED_PORT].DOUTSET = 1<<LED_RED_PIN;}
#define LED_RED_OFF         {GPIO->P[LED_RED_PORT].DOUTCLR = 1<<LED_RED_PIN;}
#define LED_RED_TOGGLE      {GPIO->P[LED_RED_PORT].DOUTTGL = 1<<LED_RED_PIN;}
#define LED_GREEN_ON        {GPIO->P[LED_GREEN_PORT].DOUTSET = 1<<LED_GREEN_PIN;}
#define LED_GREEN_OFF       {GPIO->P[LED_GREEN_PORT].DOUTCLR = 1<<LED_GREEN_PIN;}
#define LED_GREEN_TOGGLE    {GPIO->P[LED_GREEN_PORT].DOUTTGL = 1<<LED_GREEN_PIN;}
#define LED_YELLOW_ON		{GPIO->P[LED_YELLOW_PORT].DOUTSET = 1<<LED_YELLOW_PIN;}
#define LED_YELLOW_OFF		{GPIO->P[LED_YELLOW_PORT].DOUTCLR = 1<<LED_YELLOW_PIN;}
#define LED_YELLOW_TOGGLE	{GPIO->P[LED_YELLOW_PORT].DOUTTGL = 1<<LED_YELLOW_PIN;}

/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
