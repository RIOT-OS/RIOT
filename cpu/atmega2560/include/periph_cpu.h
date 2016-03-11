/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega2560
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)          ((x << 4) | y)

/**
 * @brief   Available ports on the ATmega2560 family
 */
enum {
    PORT_A = 0,       /**< port A */
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    PORT_G = 6,       /**< port G */
    PORT_H = 7,       /**< port H */
    PORT_J = 8,       /**< port J */
    PORT_K = 9,       /**< port K */
    PORT_L = 10       /**< port L */
};

/**
 * @brief   SPI mode select macro
 *
 * The polarity is determined by bit 3 in the configuration register, the phase
 * by bit 2.
 */
#define SPI_MODE_SEL(pol, pha)          ((pol << 3) | (pha << 2))

/**
 * @brief   Override the SPI mode values
 *
 * As the mode is set in bit 3 and 2 of the configuration register, we put the
 * correct configuration there
 * @{
 */
#define HAVE_SPI_CONF_T
typedef enum {
    SPI_CONF_FIRST_RISING   = SPI_MODE_SEL(0, 0),   /**< mode 0 */
    SPI_CONF_SECOND_RISING  = SPI_MODE_SEL(0, 1),   /**< mode 1 */
    SPI_CONF_FIRST_FALLING  = SPI_MODE_SEL(1, 0),   /**< mode 2 */
    SPI_CONF_SECOND_FALLING = SPI_MODE_SEL(1, 1)    /**< mode 3 */
} spi_conf_t;
/** @} */

/**
 * @brief   SPI speed selection macro
 *
 * We encode the speed in bits 2, 1, and 0, where bit0 and bit1 hold the SPCR
 * prescaler bits, while bit2 holds the SPI2X bit.
 */
#define SPI_SPEED_SEL(s2x, pr1, pr0)    ((s2x << 2) | (pr1 << 1) | pr0)

/**
 * @brief   Override SPI speed values
 *
 * We assume a master clock speed of 16MHz here.
 * @{
 */
#define HAVE_SPI_SPEED_T
typedef enum {
    SPI_SPEED_100KHZ = SPI_SPEED_SEL(0, 1, 1),      /**< 16/128 -> 125KHz */
    SPI_SPEED_400KHZ = SPI_SPEED_SEL(1, 1, 0),      /**< 16/32  -> 500KHz */
    SPI_SPEED_1MHZ   = SPI_SPEED_SEL(0, 0, 1),      /**< 16/16  -> 1MHz */
    SPI_SPEED_5MHZ   = SPI_SPEED_SEL(0, 0, 0),      /**< 16/4   -> 4MHz */
    SPI_SPEED_10MHZ  = SPI_SPEED_SEL(1, 0, 0)       /**< 16/2   -> 8MHz */
} spi_speed_t;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
