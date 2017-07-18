/*
 * Copyright(C) 2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup         cpu_mips_pic32_common
 * @brief           Shared CPU specific definitions for the MIPS family.
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the MIPS family.
 *
 * @author          Francois Berder <francois.berder@imgtec.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
/** @} */

/**
 * @brief   Length of the CPU_ID in bytes
 */
#define CPUID_LEN           (4U)

/**
 * @brief   Override GPIO pin selection macro
 */
#define GPIO_PIN(x,y)       ((x << 4) | (y & 0xf))

/**
 * @brief   Available ports on the PIC32 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
};

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Use some common SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ =   100000U, /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ =   400000U, /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   =  1000000U, /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   =  5000000U, /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000U  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   SPI device configuration
 */
typedef struct {
    volatile uint32_t *mosi_reg;    /**< Output pin mux register address */
    volatile uint32_t *miso_reg;    /**< MISO pin mux register address */
    uint8_t mosi_af;                /**< Specify function of output pin */
    uint8_t miso_af;                /**< Specify input pin for MISO */
    gpio_t mosi_pin;                /**< GPIO pin for MOSI */
    gpio_t miso_pin;                /**< GPIO pin for MISO */
} spi_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
