/*
 * Copyright (C) 2021  Lokotius Filzer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_bastwan BastWAN board
 * @ingroup     boards
 * @brief       Support for the BASTWAN SAM R34 board.
 * @{
 *
 * @file
 * @brief       Support for the BASTWAN SAM R34 board.
 *              Based on the work from
 *              Dylan Laduranty <dylan.laduranty@mesotic.com> for the  Microchip
 *              SAM R34 Xplained Pro board.
 *
 * @author      Lokotius Filzer <h-filzer@pixelsalat.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/pm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Semtech SX1276 configuration
 * @{
 */
#define SX127X_PARAM_SPI (SPI_DEV(1))
#define SX127X_PARAM_SPI_NSS GPIO_PIN(PB, 31)   /* D10 */
#define SX127X_PARAM_RESET GPIO_PIN(PB, 15)     /* A0 */
#define SX127X_PARAM_DIO0 GPIO_PIN(PB, 16)      /* D2 */
#define SX127X_PARAM_DIO1 GPIO_PIN(PA, 11)      /* D3 */
#define SX127X_PARAM_DIO2 GPIO_PIN(PA, 12)      /* D4 */
#define SX127X_PARAM_DIO3 GPIO_PIN(PB, 17)      /* D5 */
#define SX127X_PARAM_PASELECT (SX127X_PA_RFO)
/** @}*/

/**
 * @name Board specific configuration
 *  @{
 */
#define TCXO_PWR_PIN GPIO_PIN(PB, 3)
#define TX_OUTPUT_SEL_PIN GPIO_PIN(PA, 13)
#define TX_SWITCH_PWR_PIN GPIO_PIN(PA, 28)
/** @}*/

/**
 * @name LED pin definitions and handlers
 * @{
 */
#define LED_PORT PORT->Group[0]
#define LED0_PIN GPIO_PIN(PA, 22)
#define LED0_MASK (1 << 22)
#define LED0_ON (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @}*/

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 *
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
