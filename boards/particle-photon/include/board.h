#ifndef BOARD_H_
#define BOARD_H_

#include "periph_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name AT86RF233 configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAMS_BOARD    {.spi = SPI_DEV(0), \
                                   .spi_clk = SPI_CLK_5MHZ, \
                                   .cs_pin = GPIO_PIN(PORT_C, 2), \
                                   .int_pin = GPIO_PIN(PORT_A, 13), \
                                   .sleep_pin = GPIO_PIN(PORT_A, 15), \
                                   .reset_pin = GPIO_PIN(PORT_A, 14)}

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED1_PIN       GPIO_PIN(PORT_A, 1)
#define LED1_MASK      (1 << 1)
#define LED2_PIN       GPIO_PIN(PORT_A, 2)
#define LED2_MASK      (1 << 2)
#define LED3_PIN       GPIO_PIN(PORT_A, 3)
#define LED3_MASK      (1 << 3)

#define LED1_ON        (GPIOA->BSRR = (LED1_MASK << 16))
#define LED1_OFF       (GPIOA->BSRR = LED1_MASK)
#define LED1_TOGGLE    (GPIOA->ODR ^= LED1_MASK)

#define LED2_ON        (GPIOA->BSRR = (LED2_MASK << 16))
#define LED2_OFF       (GPIOA->BSRR = LED2_MASK)
#define LED2_TOGGLE    (GPIOA->ODR ^= LED2_MASK)

#define LED3_ON        (GPIOA->BSRR = (LED3_MASK << 16))
#define LED3_OFF       (GPIOA->BSRR = LED3_MASK)
#define LED3_TOGGLE    (GPIOA->ODR ^= LED3_MASK)
/** @} */

/**
 * @brief Use the 1st UART for STDIO on this board
 */
#define UART_STDIO_DEV      UART_DEV(0)

/**
 * @brief Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H_ */
