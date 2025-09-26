/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       Implementation specific CPU configuration options
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#include "cpu_conf_common.h"

#include "vendor/QN908XC.h"
#include "vendor/QN908XC_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO    (1U)
/**
 * NUMBER_OF_INT_VECTORS in the QN908XC.h is defined as including the standard
 * ARM interrupt vectors and headers, however CPU_IRQ_NUMOF does not include
 * the first 15 interrupt values and the stack pointer.
 */
#define CPU_IRQ_NUMOF           (NUMBER_OF_INT_VECTORS - 16)
/**
 * The flash is aliased at several addresses in the memory range. In particular,
 * address 0 can be mapped to RAM or flash, so it is possible to run from
 * address 0 from flash, or even turn off the flash altogether and run from RAM
 * to save power. This setting uses the ROM_START_ADDR value set in the
 * Makefile.
 */
#define CPU_FLASH_BASE          (QN908X_ROM_START_ADDR)
/**
 * @brief   Bit-Band configuration
 */
#define CPU_HAS_BITBAND 1
/** @} */

/**
 * @name   Clocks configuration
 * @{
 * @brief External and internal clocks configuration.
 *
 * The QN908x has an internal 32 MHz RCO for the high frequency clock source and
 * a 32 KHz RCO for the low frequency clock source, as well as external
 * connections for a crystal oscillator (XTAL) of either 16 MHz or 32 MHz for
 * the high frequency clock source and another connection for a 32.768 KHz XTAL
 * for the low frequency clock normally used for accurate Bluetooth timing.
 * Note that the "32 KHz" clock source is not exactly the same frequency whether
 * you use the internal or external one.
 */

/**
 * @brief Whether the board has a 32.768 KHz crystal in XTAL32_IN / XTAL32_OUT.
 **/
#if !defined(CONFIG_BOARD_HAS_XTAL32K) || DOXYGEN
#define CONFIG_BOARD_HAS_XTAL32K        0
#endif

/**
 * @name 32K low frequency clock selector
 * @{
 */
#ifdef DOXYGEN
/**
 * @brief Enabled when the 32K low frequency uses the external crystal.
 **/
#define CONFIG_CPU_CLK_32K_XTAL
/**
 * @brief Enabled when the 32K low frequency uses the internal oscillator.
 **/
#define CONFIG_CPU_CLK_32K_RCO
#endif  /* def DOXYGEN */
/** @} */

/* Default 32K clock selector config. */
#if !defined(CONFIG_CPU_CLK_32K_XTAL) && !defined(CONFIG_CPU_CLK_32K_RCO)
#if CONFIG_BOARD_HAS_XTAL32K
#define CONFIG_CPU_CLK_32K_XTAL         1
#else
#define CONFIG_CPU_CLK_32K_RCO          1
#endif
#endif

/**
 * @brief Whether the board has a 16 or 32 MHz crystal in XTAL_IN / XTAL_OUT.
 * @{
 **/
#ifndef CONFIG_BOARD_HAS_XTAL
#define CONFIG_BOARD_HAS_XTAL           0
#endif

/**
 * @name External high frequency "XTAL" crystal frequency
 */
#ifdef DOXYGEN
/**
 * @brief Enabled when the external XTAL is a 16 MHz one.
 **/
#define CONFIG_CPU_CLK_XTAL_16M
/**
 * @brief Enabled when the external XTAL is a 32 MHz one.
 **/
#define CONFIG_CPU_CLK_XTAL_32M
#endif  /* def DOXYGEN */
/** @} */

/* Default XTAL setting. */
#if CONFIG_BOARD_HAS_XTAL && \
    !defined(CONFIG_BOARD_HAS_XTAL_16M) && !defined(CONFIG_BOARD_HAS_XTAL_32M)
#define CONFIG_BOARD_HAS_XTAL_32M        1
#endif

/**
 * @brief Internal OSC32M clock input /2 divider enabled
 **/
#ifndef CONFIG_CPU_CLK_OSC32M_DIV
#define CONFIG_CPU_CLK_OSC32M_DIV    0
#endif

/**
 * @brief External XTAL 32 MHz clock input /2 divider enabled
 **/
#ifndef CONFIG_CPU_CLK_XTAL_DIV
#define CONFIG_CPU_CLK_XTAL_DIV      0
#endif

/**
 * @name System clock configuration selector
 * @{
 */
#ifdef DOXYGEN
/**
 * @brief System clock is external crystal source (including divider).
 **/
#define CONFIG_CPU_CLK_SYS_XTAL
/**
 * @brief System clock is internal 32 MHz oscillator source (including divider).
 **/
#define CONFIG_CPU_CLK_SYS_OSC32M
/**
 * @brief System clock is the low frequency clock (32 or 32.768 KHz)
 **/
#define CONFIG_CPU_CLK_SYS_32K
#endif  /* def DOXYGEN */
/** @} */

/* Default system clock configuration selector */
#if !defined(CONFIG_CPU_CLK_SYS_XTAL) && !defined(CONFIG_CPU_CLK_SYS_OSC32M) && \
    !defined(CONFIG_CPU_CLK_SYS_32K)
#if CONFIG_BOARD_HAS_XTAL
#define CONFIG_CPU_CLK_SYS_XTAL      1
#else
#define CONFIG_CPU_CLK_SYS_OSC32M       1
#endif
#endif

/**
 * @brief AHB clock divider
 *
 * The AHB clock is derived from the System clock using this divider value,
 * between 1 and 8192, and serves as a clock source for ARM core, FSP, SCT,
 * Quad-SPI, Flexcomm (UART, SPI, I2C), GPIO, BLE_AHB and DMA.
 * Note: When BLE is enabled, the AHB clock must be at least the BLE clock
 * (either 8 or 16 MHz) limiting the range of allowed values for this
 * divider so that the AHB clock is 8, 16 or 32 MHz.
 **/
#ifndef CONFIG_CPU_CLK_AHB_DIV
#define CONFIG_CPU_CLK_AHB_DIV       1u
#endif

/**
 * @brief APB clock divider
 *
 * The APB clock is derived from the AHB clock using this divide value,
 * between 1 and 16, and serves as the clock source for several
 * peripherals, such as the RTC, ADC, DAC, Capacitive Sense (CS) and
 * optionally the WDT.
 **/
#ifndef CONFIG_CPU_CLK_APB_DIV
#define CONFIG_CPU_CLK_APB_DIV       1u
#endif

/** @} */

/**
 * @name   Code Read Protection
 * @{
 * @brief Image "Code Read Protection" field definitions.
 *
 * The Code Read Protection (CRP) is a 32-bit field stored in one of the
 * reserved fields in the Cortex-M interrupt vector and therefore part of the
 * image. It allows to enable or disable access to the flash from the In-System
 * Programming (ISP) interface to read, erase or write flash pages, as well as
 * external SWD access for debugging or programming the flash. Not all the CRP
 * values are valid and an invalid value may render the flash inaccessible and
 * effectively brick the device.
 *
 * To select the access level define the @ref QN908X_CRP macro from the global
 * compile options, otherwise the default value in this module will be used
 * (allowing everything). The value of the uint32_t CRP field in the Image
 * vector table should be the "or" of the following QN908X_CRP_* macros. Every
 * field must be either enabled or disabled, otherwise it would result in an
 * invalid CRP value.
 */

/**
 * @brief Number of pages to protect (0 to 255).
 *
 * This defines the number of pages to protect starting from 0. A value of 0
 * in this macro means that no page is protected. The maximum number allowed to
 * be passed to this macro is 255, however there are 256 pages in the flash. The
 * last page is protected if any other page is protected.
 *
 * Protected pages can't be erased or written to by the ISP.
 */
#define QN908X_CRP_PROTECT_PAGES(X) (255 - (X))

/**
 * @brief Mass erase from ISP allowed.
 */
#define QN908X_CRP_MASS_ERASE_ALLOW     (0x800)
/**
 * @brief Mass erase from ISP not allowed.
 */
#define QN908X_CRP_MASS_ERASE_DISALLOW  (0x400)

/**
 * @brief Page erase/write from ISP (for unprotected pages) allowed.
 */
#define QN908X_CRP_PAGE_ERASE_WRITE_ALLOW     (0x2000)
/**
 * @brief Page erase/write from ISP (for unprotected pages) not allowed.
 */
#define QN908X_CRP_PAGE_ERASE_WRITE_DISALLOW  (0x1000)

/**
 * @brief Flash read (for unprotected pages) from ISP allowed or not.
 */
#define QN908X_CRP_FLASH_READ_ALLOW     (0x8000)
/**
 * @brief Flash read (for unprotected pages) from ISP not allowed.
 */
#define QN908X_CRP_FLASH_READ_DISALLOW  (0x4000)

/**
 * @brief ISP entry is allowed (via CHIP_MODE pin).
 */
#define QN908X_CRP_ISP_ENTRY_ALLOW     (0x20000)
/**
 * @brief ISP entry via CHIP_MODE pin is not allowed.
 */
#define QN908X_CRP_ISP_ENTRY_DISALLOW  (0x10000)

/**
 * @brief External access is allowed (including SWD interface).
 */
#define QN908X_CRP_EXTERNAL_ACCESS_ALLOW     (0x80000)
/**
 * @brief External access is not allowed (including SWD interface).
 */
#define QN908X_CRP_EXTERNAL_ACCESS_DISALLOW  (0x40000)

/** @} */

/**
 * @brief   Default "Code Read Protection" allows everything.
 */
#ifndef QN908X_CRP
#define QN908X_CRP \
    (QN908X_CRP_PROTECT_PAGES(0) \
     | QN908X_CRP_MASS_ERASE_ALLOW \
     | QN908X_CRP_PAGE_ERASE_WRITE_ALLOW \
     | QN908X_CRP_FLASH_READ_ALLOW \
     | QN908X_CRP_ISP_ENTRY_ALLOW \
     | QN908X_CRP_EXTERNAL_ACCESS_ALLOW)
#endif  /* QN908X_CRP */

/**
 * @brief   The "Code Read Protection" is stored at the offset 0x20.
 *
 * To modify the CRP field define the macro @ref QN908X_CRP.
 */
#define CORTEXM_VECTOR_RESERVED_0X20 QN908X_CRP

/* Safety checks that the QN908X_CRP value is valid. */
#if !(QN908X_CRP & QN908X_CRP_MASS_ERASE_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_MASS_ERASE_DISALLOW)
#error "Must select exactly one of QN908X_CRP_MASS_ERASE_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_PAGE_ERASE_WRITE_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_PAGE_ERASE_WRITE_DISALLOW)
#error \
    "Must select exactly one of QN908X_CRP_PAGE_ERASE_WRITE_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_FLASH_READ_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_FLASH_READ_DISALLOW)
#error "Must select exactly one of QN908X_CRP_FLASH_READ_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_ISP_ENTRY_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_ISP_ENTRY_DISALLOW)
#error "Must select exactly one of QN908X_CRP_ISP_ENTRY_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_EXTERNAL_ACCESS_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_EXTERNAL_ACCESS_DISALLOW)
#error \
    "Must select exactly one of QN908X_CRP_EXTERNAL_ACCESS_* in the QN908X_CRP"
#endif

#ifdef __cplusplus
}
#endif

/** @} */
