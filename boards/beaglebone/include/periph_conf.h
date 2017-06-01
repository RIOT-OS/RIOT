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
 * @file        periph_conf.h
 * @brief       Peripheral MCU configuration for the beaglebone board
 *
 * @author      Max Dieckmann <dieckmann@zedat.fu-berlin.de>
 * @author      Martin Kessel <martin.kessel@fu-berlin.de> 
 *
 */


#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_IRQ_PRIO       1

/*
 * @brief GPIO configuration
 */
#define GPIO_NUMOF          (15U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1



/* @brief GPIO Pin base addresses, offsets and pin number 
**	Pins on channel 0-10 lie on expansion header P8, connection 20-30, 
**  respectively. Pins on channel 11-15 correspond to the connections 
**  39-43.
*/

/* GPIO channel 0 config */
#define GPIO_0_DEV          SOC_GPIO_1_REGS
#define GPIO_0_OFFSET       GPIO_1_31
#define GPIO_0_PIN			31
/* GPIO channel 1 config */
#define GPIO_1_DEV          SOC_GPIO_1_REGS
#define GPIO_1_OFFSET       GPIO_1_30
#define GPIO_1_PIN			30
/* GPIO channel 2 config */
#define GPIO_2_DEV          SOC_GPIO_1_REGS
#define GPIO_2_OFFSET       GPIO_1_5
#define GPIO_2_PIN			5
/* GPIO channel 3 config */
#define GPIO_3_DEV          SOC_GPIO_1_REGS
#define GPIO_3_OFFSET       GPIO_1_4
#define GPIO_3_PIN			4
/* GPIO channel 4 config */
#define GPIO_4_DEV          SOC_GPIO_1_REGS
#define GPIO_4_OFFSET       GPIO_1_1
#define GPIO_4_PIN			1
/* GPIO channel 5 config */
#define GPIO_5_DEV          SOC_GPIO_1_REGS
#define GPIO_5_OFFSET       GPIO_1_0
#define GPIO_5_PIN			0
/* GPIO channel 6 config */
#define GPIO_6_DEV          SOC_GPIO_1_REGS
#define GPIO_6_OFFSET       GPIO_1_29
#define GPIO_6_PIN			29
/* GPIO channel 7 config */
#define GPIO_7_DEV          SOC_GPIO_2_REGS
#define GPIO_7_OFFSET       GPIO_2_22
#define GPIO_7_PIN			22
/* GPIO channel 8 config */
#define GPIO_8_DEV          SOC_GPIO_2_REGS
#define GPIO_8_OFFSET       GPIO_2_24
#define GPIO_8_PIN			24
/* GPIO channel 9 config */
#define GPIO_9_DEV          SOC_GPIO_2_REGS
#define GPIO_9_OFFSET       GPIO_2_23
#define GPIO_9_PIN			23
/* GPIO channel 10 config */
#define GPIO_10_DEV         SOC_GPIO_2_REGS
#define GPIO_10_OFFSET      GPIO_2_25
#define GPIO_10_PIN			25
/* GPIO channel 11 config */
#define GPIO_11_DEV         SOC_GPIO_2_REGS
#define GPIO_11_OFFSET      GPIO_2_12
#define GPIO_11_PIN			12
/* GPIO channel 12 config */
#define GPIO_12_DEV         SOC_GPIO_2_REGS
#define GPIO_12_OFFSET      GPIO_2_13
#define GPIO_12_PIN			13
/* GPIO channel 13 config */
#define GPIO_13_DEV         SOC_GPIO_2_REGS
#define GPIO_13_OFFSET      GPIO_2_10
#define GPIO_13_PIN			10
/* GPIO channel 14 config */
#define GPIO_14_DEV         SOC_GPIO_2_REGS
#define GPIO_14_OFFSET      GPIO_2_11
#define GPIO_14_PIN			11
/* GPIO channel 15 config */
#define GPIO_15_DEV         SOC_GPIO_2_REGS
#define GPIO_15_OFFSET      GPIO_2_8
#define GPIO_15_PIN			8



#endif /* __PERIPH_CONF_H */
