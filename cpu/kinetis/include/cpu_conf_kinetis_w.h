/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_kinetis
 * @brief           CPU specific implementations for the NXP Kinetis K series of
 *                  Cortex-M MCUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#if defined(KINETIS_CORE_D)
/* Kinetis KW2xD */
#if defined(CPU_MODEL_MKW21D256VHA5) || \
    defined(CPU_MODEL_MKW21D512VHA5)
#include "vendor/MKW21D5.h"
#elif defined(CPU_MODEL_MKW22D512VHA5)
#include "vendor/MKW22D5.h"
#elif defined(CPU_MODEL_MKW24D512VHA5)
#include "vendor/MKW24D5.h"
#endif

/**
 * @brief This CPU provides an additional ADC clock divider as CFG1[ADICLK]=1
 */
#define KINETIS_HAVE_ADICLK_BUS_DIV_2 1
/**
 * @name KW2XD SiP internal interconnects between MCU and radio
 *
 * @{
 */
#define KW2XDRF_PORT_DEV        PORTB /**< MCU Port connected to radio*/
#define KW2XDRF_PORT            PORT_B /**< MCU Port connected to radio*/
#define KW2XDRF_GPIO            GPIOB /**< GPIO Device connected to radio */
#define KW2XDRF_PORT_IRQn       PORTB_IRQn
/** Clock Enable for PORTB */
#define KW2XDRF_PORT_CLKEN()    (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define KW2XDRF_PIN_AF          2  /**< Pin Muxing Parameter for GPIO Device*/
#define KW2XDRF_PCS0_PIN        10 /**< Radio SPI chip select pin */
#define KW2XDRF_SCK_PIN         11 /**< Radio SPI clock pin */
#define KW2XDRF_SOUT_PIN        16 /**< Radio SPI MOSI pin */
#define KW2XDRF_SIN_PIN         17 /**< Radio SPI MISO pin */
#define KW2XDRF_RST_PIN         19 /**< Radio reset pin */
#define KW2XDRF_IRQ_PIN         3  /**< radio IRQ pin (active low) */
#define KW2XDRF_CLK_CTRL_PORT   PORT_C /**< CLK_OUT control pin port */
#define KW2XDRF_CLK_CTRL_PORT_DEV PORTC /**< CLK_OUT control pin PORT device */
#define KW2XDRF_CLK_CTRL_GPIO   GPIOC /**< CLK_OUT control pin GPIO device */
#define KW2XDRF_CLK_CTRL_CLKEN() (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define KW2XDRF_CLK_CTRL_PIN    0 /**< CLK_OUT control pin */
/** @} */

#elif defined(KINETIS_CORE_Z)
/* Kinetis KWxxZ */
#if defined(CPU_MODEL_MKW21Z256VHT4) || \
    defined(CPU_MODEL_MKW21Z512VHT4)
#include "vendor/MKW21Z4.h"
#elif defined(CPU_MODEL_MKW31Z256VHT4) || \
    defined(CPU_MODEL_MKW31Z512CAT4) || \
    defined(CPU_MODEL_MKW31Z512VHT4)
#include "vendor/MKW31Z4.h"
#elif defined(CPU_MODEL_MKW41Z256VHT4) || \
    defined(CPU_MODEL_MKW41Z512CAT4) || \
    defined(CPU_MODEL_MKW41Z512VHT4)
#include "vendor/MKW41Z4.h"
#endif
#endif /* KINETIS_CORE_x */

/**
 * @name   Flashpage configuration
 * @{
 */
#define FLASHPAGE_SIZE      (2048U)
#define FLASHPAGE_NUMOF     ((KINETIS_ROMSIZE * 1024) / FLASHPAGE_SIZE)

/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_WRITE_BLOCK_SIZE          (4U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT     (4U)
/* Section erase and programming must be 8 bytes aligned */
#define FLASHPAGE_BLOCK_SECTION_ALIGNMENT   (8U)
/** @} */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

/** @} */
