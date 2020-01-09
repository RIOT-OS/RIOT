/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_native
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#ifndef CPUID_LEN
#define CPUID_LEN           (4U)
#endif

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_OFF
#define PROVIDES_PM_SET_LOWEST
/** @} */

/* Configuration for the wrapper around the Linux SPI API (periph_spidev_linux)
 *
 * Needs to go here, otherwise the SPI_NEEDS_ are defined after inclusion of
 * spi.h.
 */
#if defined(MODULE_PERIPH_SPIDEV_LINUX) || defined(DOXYGEN)

/**
 * @name SPI Configuration
 */

/**
 * @brief   Use the common `transfer_byte` SPI function
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
/**
 * @brief   Use the common `transfer_reg` SPI function
 */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
/**
 * @brief   Use the common `transfer_regs` SPI function
 */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS

#ifndef DOXYGEN
/**
 * @brief   Use a custom clock speed type
 */
#define HAVE_SPI_CLK_T
/**
 * @brief   SPI clock speed values
 *
 * The Linux userspace driver takes values in Hertz, which values are available
 * can only be determined at runtime.
 * @{
 */
typedef enum {
    SPI_CLK_100KHZ = (100000U),
    SPI_CLK_400KHZ = (400000U),
    SPI_CLK_1MHZ   = (1000000U),
    SPI_CLK_5MHZ   = (5000000U),
    SPI_CLK_10MHZ  = (10000000U)
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */
#endif /* MODULE_PERIPH_SPI | DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
