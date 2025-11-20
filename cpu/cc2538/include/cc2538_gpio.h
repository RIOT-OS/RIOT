/*
 * SPDX-FileCopyrightText: 2014 Loci Controls Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup        cpu_cc2538_gpio CC2538 General-Purpose I/O
 * @ingroup         cpu_cc2538_regs
 * @{
 *
 * @file
 * @brief           Driver for the cc2538 GPIO controller
 *
 * Header file with register and macro declarations for the cc2538 GPIO module
 *
 * @author          Ian Martin <ian@locicontrols.com>
 *
 * @{
 */

#include <stdint.h>

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Numeric representation of the four GPIO ports
 * @{
 */
enum {
    PORT_A = 0,
    PORT_B = 1,
    PORT_C = 2,
    PORT_D = 3,
};
/** @} */

#define GPIO_PORT_SHIFT 3                              /**< Right-shift amount to obtain the port number from a GPIO number */
#define GPIO_BITS_PER_PORT ( 1 << GPIO_PORT_SHIFT )    /**< Number of bits per GPIO port (8) */
#define GPIO_BIT_MASK      ( GPIO_BITS_PER_PORT - 1 )  /**< Mask to obtain the bit number from a GPIO number */

/**
 * @brief Generate a bit mask in which only the specified bit is high.
 *
 * @param[in] n Number of the bit to set high in the mask.
 *
 * @return A bit mask in which bit n is high.
*/
#define PIN_MASK(n) ( 1 << (n) )

/**
 * @brief Extract the GPIO port number (0-3) from a GPIO number (0-31)
 *
 * @param[in] gpio_num GPIO number (0-31)
 *
 * @return Corresponding GPIO port number (0-3)
*/
#define GPIO_NUM_TO_PORT_NUM(gpio_num) ( (gpio_num) >> GPIO_PORT_SHIFT )

/**
 * @brief Extract the GPIO port bit number (0-7) from a GPIO number (0-31)
 *
 * @param[in] gpio_num GPIO number (0-31)
 *
 * @return Corresponding GPIO port bit number (0-7)
*/
#define GPIO_BIT_NUM(gpio_num) ( (gpio_num) & GPIO_BIT_MASK )

/**
 * @brief Generate a GPIO number given a port and bit number
 *
 * @param[in] port_num GPIO port (PORT_A, PORT_B, PORT_C, or PORT_D)
 * @param[in] bit_num  GPIO bit number (0-7)
 *
 * @return Corresponding GPIO number (0-31)
*/
#define GPIO_PXX_TO_NUM(port_num, bit_num) ( ((port_num) << GPIO_PORT_SHIFT) | (bit_num) )

/**
 * @brief Obtain the GPIO port instance given a GPIO number (0-31)
 *
 * @param[in] gpio_num GPIO number (0-31)
 *
 * @return Corresponding GPIO port instance
*/
#define GPIO_NUM_TO_DEV(gpio_num) ( GPIO_A + GPIO_NUM_TO_PORT_NUM(gpio_num) )

/**
 * @brief Enable hardware (peripheral) control for a given GPIO pin number
 *
 * @param[in] gpio_num GPIO number (0-31)
*/
#define gpio_hardware_control(gpio_num) ( GPIO_NUM_TO_DEV(gpio_num)->AFSEL |= PIN_MASK(GPIO_BIT_NUM(gpio_num)) )

/**
 * @brief Enable software control for a given GPIO pin number
 *
 * @param[in] gpio_num GPIO number (0-31)
*/
#define gpio_software_control(gpio_num) ( GPIO_NUM_TO_DEV(gpio_num)->AFSEL &= ~PIN_MASK(GPIO_BIT_NUM(gpio_num)) )

/**
 * @brief Configure the given GPIO as an output
 *
 * @param[in] gpio_num GPIO number (0-31)
*/
#define gpio_dir_output(gpio_num) ( GPIO_NUM_TO_DEV(gpio_num)->DIR |= PIN_MASK(GPIO_BIT_NUM(gpio_num)) )

/**
 * @brief Configure the given GPIO as an input
 *
 * @param[in] gpio_num GPIO number (0-31)
*/
#define gpio_dir_input(gpio_num) ( GPIO_NUM_TO_DEV(gpio_num)->DIR &= ~PIN_MASK(GPIO_BIT_NUM(gpio_num)) )

/**
 * @brief Read the value of the given pin
 *
 * @param[in] gpio_num GPIO number (0-31)
 */
#define cc2538_gpio_read(gpio_num) ( (GPIO_NUM_TO_DEV(gpio_num)->DATA >> GPIO_BIT_NUM(gpio_num)) & 1 )

/**
 * @brief Set a specific GPIO output pin low
 *
 * @param[in] gpio_num GPIO number (0-31)
*/
#define cc2538_gpio_clear(gpio_num) ( GPIO_NUM_TO_DEV(gpio_num)->DATA &= ~PIN_MASK(GPIO_BIT_NUM(gpio_num)) )

/**
 * @name Unique names for each GPIO port/pin combination
 * @deprecated will be removed after adaption of periph drivers,
 *             use GPIO_PIN macro instead
 * @{
 */
enum {
    GPIO_PA0 = GPIO_PXX_TO_NUM(PORT_A, 0),          /**< PA0 */
    GPIO_PA1 = GPIO_PXX_TO_NUM(PORT_A, 1),          /**< PA1 */
    GPIO_PA2 = GPIO_PXX_TO_NUM(PORT_A, 2),          /**< PA2 */
    GPIO_PA3 = GPIO_PXX_TO_NUM(PORT_A, 3),          /**< PA3 */
    GPIO_PA4 = GPIO_PXX_TO_NUM(PORT_A, 4),          /**< PA4 */
    GPIO_PA5 = GPIO_PXX_TO_NUM(PORT_A, 5),          /**< PA5 */
    GPIO_PA6 = GPIO_PXX_TO_NUM(PORT_A, 6),          /**< PA6 */
    GPIO_PA7 = GPIO_PXX_TO_NUM(PORT_A, 7),          /**< PA7 */
    GPIO_PB0 = GPIO_PXX_TO_NUM(PORT_B, 0),          /**< PB0 */
    GPIO_PB1 = GPIO_PXX_TO_NUM(PORT_B, 1),          /**< PB1 */
    GPIO_PB2 = GPIO_PXX_TO_NUM(PORT_B, 2),          /**< PB2 */
    GPIO_PB3 = GPIO_PXX_TO_NUM(PORT_B, 3),          /**< PB3 */
    GPIO_PB4 = GPIO_PXX_TO_NUM(PORT_B, 4),          /**< PB4 */
    GPIO_PB5 = GPIO_PXX_TO_NUM(PORT_B, 5),          /**< PB5 */
    GPIO_PB6 = GPIO_PXX_TO_NUM(PORT_B, 6),          /**< PB6 */
    GPIO_PB7 = GPIO_PXX_TO_NUM(PORT_B, 7),          /**< PB7 */
    GPIO_PC0 = GPIO_PXX_TO_NUM(PORT_C, 0),          /**< PC0 */
    GPIO_PC1 = GPIO_PXX_TO_NUM(PORT_C, 1),          /**< PC1 */
    GPIO_PC2 = GPIO_PXX_TO_NUM(PORT_C, 2),          /**< PC2 */
    GPIO_PC3 = GPIO_PXX_TO_NUM(PORT_C, 3),          /**< PC3 */
    GPIO_PC4 = GPIO_PXX_TO_NUM(PORT_C, 4),          /**< PC4 */
    GPIO_PC5 = GPIO_PXX_TO_NUM(PORT_C, 5),          /**< PC5 */
    GPIO_PC6 = GPIO_PXX_TO_NUM(PORT_C, 6),          /**< PC6 */
    GPIO_PC7 = GPIO_PXX_TO_NUM(PORT_C, 7),          /**< PC7 */
    GPIO_PD0 = GPIO_PXX_TO_NUM(PORT_D, 0),          /**< PD0 */
    GPIO_PD1 = GPIO_PXX_TO_NUM(PORT_D, 1),          /**< PD1 */
    GPIO_PD2 = GPIO_PXX_TO_NUM(PORT_D, 2),          /**< PD2 */
    GPIO_PD3 = GPIO_PXX_TO_NUM(PORT_D, 3),          /**< PD3 */
    GPIO_PD4 = GPIO_PXX_TO_NUM(PORT_D, 4),          /**< PD4 */
    GPIO_PD5 = GPIO_PXX_TO_NUM(PORT_D, 5),          /**< PD5 */
    GPIO_PD6 = GPIO_PXX_TO_NUM(PORT_D, 6),          /**< PD6 */
    GPIO_PD7 = GPIO_PXX_TO_NUM(PORT_D, 7),          /**< PD7 */
};
/** @} */

/**
 * @brief GPIO port component registers
 */
typedef struct {
    cc2538_reg_t RESERVED1[255];                    /**< Reserved addresses */
    cc2538_reg_t DATA;                              /**< GPIO_A Data Register */
    cc2538_reg_t DIR;                               /**< GPIO_A data direction register */
    cc2538_reg_t IS;                                /**< GPIO_A Interrupt Sense register */
    cc2538_reg_t IBE;                               /**< GPIO_A Interrupt Both-Edges register */
    cc2538_reg_t IEV;                               /**< GPIO_A Interrupt Event Register */
    cc2538_reg_t IE;                                /**< GPIO_A Interrupt mask register */
    cc2538_reg_t RIS;                               /**< GPIO_A Raw Interrupt Status register */
    cc2538_reg_t MIS;                               /**< GPIO_A Masked Interrupt Status register */
    cc2538_reg_t IC;                                /**< GPIO_A Interrupt Clear register */
    cc2538_reg_t AFSEL;                             /**< GPIO_A Alternate Function / mode control select register */
    cc2538_reg_t RESERVED2[63];                     /**< Reserved addresses */
    cc2538_reg_t GPIOLOCK;                          /**< GPIO_A Lock register */
    cc2538_reg_t GPIOCR;                            /**< GPIO_A Commit Register */
    cc2538_reg_t RESERVED3[118];                    /**< Reserved addresses */
    cc2538_reg_t PMUX;                              /**< GPIO_A The PMUX register */
    cc2538_reg_t P_EDGE_CTRL;                       /**< GPIO_A The Port Edge Control register */
    cc2538_reg_t RESERVED4[2];                      /**< Reserved addresses */
    cc2538_reg_t PI_IEN;                            /**< GPIO_A The Power-up Interrupt Enable register */
    cc2538_reg_t RESERVED5[1];                      /**< Reserved addresses */
    cc2538_reg_t IRQ_DETECT_ACK;                    /**< GPIO_A IRQ Detect ACK register */
    cc2538_reg_t USB_IRQ_ACK;                       /**< GPIO_A IRQ Detect ACK for USB */
    cc2538_reg_t IRQ_DETECT_UNMASK;                 /**< GPIO_A IRQ Detect ACK for masked interrupts */
    cc2538_reg_t RESERVED6[567];                    /**< Reserved addresses */
} cc2538_gpio_t;

/**
 * @brief   GPIO port instance base address
 */
#define GPIO_BASE           (0x400d9000)

/**
 * @name    GPIO shift and masking
 * @{
 */
#define GPIO_PORTNUM_SHIFT  (12U)           /**< bit shift for GPIO port      */
#define GPIO_PORTNUM_MASK   (0x00007000)    /**< bit mask for GPIO port [0-3] */
#define GPIO_PIN_MASK       (0x00000007)    /**< bit mask for GPIO pin [0-7]  */
#define GPIO_PORT_MASK      (0xfffff000)    /**< bit mask for GPIO port addr  */
/** @} */

/**
 * @name    GPIO instance definitions
 * @deprecated will be removed after adaption of periph drivers
 * @{
 */
#define GPIO_A ((cc2538_gpio_t *)0x400d9000)    /**< GPIO Port A instance */
#define GPIO_B ((cc2538_gpio_t *)0x400da000)    /**< GPIO Port B instance */
#define GPIO_C ((cc2538_gpio_t *)0x400db000)    /**< GPIO Port C instance */
#define GPIO_D ((cc2538_gpio_t *)0x400dc000)    /**< GPIO Port D instance */
/** @} */

/**
 * @brief IOC port component registers
 */
 typedef struct {
     cc2538_reg_t SEL[32];      /**< select special function for output pin*/
     cc2538_reg_t OVER[32];     /**< override pin mode, enable alternate mode */
     cc2538_reg_t PINS[21];     /**< select input pin for special functions */
 } cc2538_ioc_t;

 /**
  * @name Peripheral Signal Select Values (for IOC_Pxx_SEL registers)
  * @{
  */
 typedef enum {
     UART0_TXD = 0,      /**< UART0 TXD */
     UART1_RTS,          /**< UART1 RTS */
     UART1_TXD,          /**< UART1 TXD */
     SSI0_TXD,           /**< SSI0 TXD */
     SSI0_CLK_OUT,       /**< SSI0 CLKOUT */
     SSI0_FSS_OUT,       /**< SSI0 FSSOUT */
     SSI0_TX_SER,        /**< SSI0 STXSER EN */
     SSI1_TXD,           /**< SSI1 TXD */
     SSI1_CLK_OUT,       /**< SSI1 CLKOUT */
     SSI1_FSS_OUT,       /**< SSI1 FSSOUT */
     SSI1_TX_SER,        /**< SSI1 STXSER EN */
     I2C_SDA_OUT,        /**< I2C CMSSDA */
     I2C_SCL_OUT,        /**< I2C CMSSCL */
     GPT0_ICP1,          /**< GPT0 ICP1 */
     GPT0_ICP2,          /**< GPT0 ICP2 */
     GPT1_ICP1,          /**< GPT1 ICP1 */
     GPT1_ICP2,          /**< GPT1 ICP2 */
     GPT2_ICP1,          /**< GPT2 ICP1 */
     GPT2_ICP2,          /**< GPT2 ICP2 */
     GPT3_ICP1,          /**< GPT3 ICP1 */
     GPT3_ICP2 ,         /**< GPT3 ICP2 */
 } cc2538_ioc_sel_t;
 /** @} */

 /**
  * @name Pin select for periphical functions
  * @{
  */
 typedef enum {
     UART0_RXD = 0,      /**< UART0 RXD */
     UART1_CTS,          /**< UART1 CTS */
     UART1_RXD,          /**< UART1 RXD */
     SSI0_CLK,           /**< SSI0 CLK */
     SSI0_RXD,           /**< SSI0 RXD */
     SSI0_FSS_IN,        /**< SSI0 FSS IN */
     SSI0_CLK_IN,        /**< SSI0 CLK IN */
     SSI1_CLK,           /**< SSI1 CLK */
     SSI1_RXD,           /**< SSI1 RXD */
     SSI1_FSS_IN,        /**< SSI1 FSS IN */
     SSI1_CLK_IN,        /**< SSI1 CLK IN */
     I2C_SDA_IN,         /**< I2C SDA IN */
     I2C_SCL_IN,         /**< I2C SCL IN */
     GPT0_OCP1,          /**< GPT0 OCP1 */
     GPT0_OCP2,          /**< GPT0 OCP2 */
     GPT1_OCP1,          /**< GPT1 OCP1 */
     GPT1_OCP2,          /**< GPT1 OCP2 */
     GPT2_OCP1,          /**< GPT2 OCP1 */
     GPT2_OCP2,          /**< GPT2 OCP2 */
     GPT3_OCP1,          /**< GPT3 OCP1 */
     GPT3_OCP2,          /**< GPT3 OCP2 */
 } cc2538_ioc_pin_t;
 /** @} */

 /**
  * @brief Values to override pin configuration
  */
 typedef enum {
     OVERRIDE_DISABLE    = 0x0,
     OVERRIDE_ANALOG     = 0x1,
     OVERRIDE_PULLDOWN   = 0x2,
     OVERRIDE_PULLUP     = 0x4,
     OVERRIDE_ENABLE     = 0x8,
 } cc2538_ioc_over_t;

/**
 * @name Values for IOC_PXX_OVER
 * @deprecated will be removed after adaption of periph drivers,
 *             use cc2538_ioc_over_t instead
 * @{
 */
#define IOC_OVERRIDE_OE   0x00000008    /**< Output Enable */
#define IOC_OVERRIDE_PUE  0x00000004    /**< Pull Up Enable */
#define IOC_OVERRIDE_PDE  0x00000002    /**< Pull Down Enable */
#define IOC_OVERRIDE_ANA  0x00000001    /**< Analog Enable */
#define IOC_OVERRIDE_DIS  0x00000000    /**< Override Disabled */
/** @} */

/**
 * @brief   IOC instance definition
 */
#define IOC    ((cc2538_ioc_t *)0x400d4000)

/**
 * @name Port control register addresses
 * @deprecated will be removed after adaption of periph drivers,
 *             use IOC->OVER and IOC->SEL instead
 * @{
 */
#define IOC_PXX_OVER    (IOC->OVER)
#define IOC_PXX_SEL     (IOC->SEL)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
/** @} */
