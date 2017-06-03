/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 * @brief           CPU specific definitions common to all Kinetis CPUs
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_KINETIS_H
#define CPU_CONF_KINETIS_H

#include "cpu_conf_common.h"

#if (__CORTEX_M < 3)
/*
 * Kinetis Cortex-M0+ devices have bit manipulation engine (BME) which provides
 * the same functionality (and some more) as the bitband aliased memory found in
 * Cortex-M3 and up
 */
#include "bme.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO    (1U)
#define CPU_IRQ_NUMOF           (NUMBER_OF_INT_VECTORS)
#define CPU_FLASH_BASE          (0x00000000)
/** @} */

/**
 * @name GPIO pin mux function numbers
 * @{
 */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO   1
/** @} */

/**
 * @name GPIO interrupt flank settings
 * @{
 */
#define PIN_INTERRUPT_RISING    0b1001
#define PIN_INTERRUPT_FALLING   0b1010
#define PIN_INTERRUPT_EDGE      0b1011
/** @} */

/**
 * @brief Mapping internal module interrupts to LLWU wake up sources
 *
 * @note Modules not listed here CAN NOT be used to wake the CPU from LLS or
 * VLLSx power modes.
 *
 * The numbers are hardware specific, but have the same values across all the
 * supported Kinetis CPUs
 */
typedef enum llwu_wakeup_module {
    LLWU_WAKEUP_MODULE_LPTMR0      = 0,
    LLWU_WAKEUP_MODULE_CMP0        = 1,
    LLWU_WAKEUP_MODULE_RADIO       = 2, /* KWx1Z devices */
    LLWU_WAKEUP_MODULE_CMP1        = 2, /* others */
    LLWU_WAKEUP_MODULE_DCDC        = 3, /* KWx1Z devices */
    LLWU_WAKEUP_MODULE_CMP2        = 3, /* others */
    LLWU_WAKEUP_MODULE_TSI         = 4,
    LLWU_WAKEUP_MODULE_RTC_ALARM   = 5,
    LLWU_WAKEUP_MODULE_RESERVED    = 6,
    LLWU_WAKEUP_MODULE_RTC_SECONDS = 7,
    LLWU_WAKEUP_MODULE_NUMOF
} llwu_wakeup_module_t;

/**
 * @brief Mapping physical pins to LLWU wakeup pin source numbers
 *
 * @note Pins not listed here CAN NOT be used to wake the CPU from LLS or
 * VLLSx power modes.
 *
 * The numbers are hardware specific, but have the same values across all the
 * supported Kinetis CPUs.
 */
#if defined(KINETIS_SERIES_W) && defined(KINETIS_CORE_Z) && (KINETIS_SUBFAMILY == 1)
/* KW41Z has different LLWU pins */
typedef enum llwu_wakeup_pin {
    LLWU_WAKEUP_PIN_PTC16 =  0,
    LLWU_WAKEUP_PIN_PTC17 =  1,
    LLWU_WAKEUP_PIN_PTC18 =  2,
    LLWU_WAKEUP_PIN_PTC19 =  3,
    LLWU_WAKEUP_PIN_PTA16 =  4,
    LLWU_WAKEUP_PIN_PTA17 =  5,
    LLWU_WAKEUP_PIN_PTA18 =  6,
    LLWU_WAKEUP_PIN_PTA19 =  7,
    LLWU_WAKEUP_PIN_PTB0  =  8,
    LLWU_WAKEUP_PIN_PTC0  =  9,
    LLWU_WAKEUP_PIN_PTC2  = 10,
    LLWU_WAKEUP_PIN_PTC3  = 11,
    LLWU_WAKEUP_PIN_PTC4  = 12,
    LLWU_WAKEUP_PIN_PTC5  = 13,
    LLWU_WAKEUP_PIN_PTC6  = 14,
    LLWU_WAKEUP_PIN_PTC7  = 15,
    LLWU_WAKEUP_PIN_NUMOF,
    LLWU_WAKEUP_PIN_UNDEF
} llwu_wakeup_pin_t;
#else
typedef enum llwu_wakeup_pin {
    LLWU_WAKEUP_PIN_PTE1  =  0,
    LLWU_WAKEUP_PIN_PTE2  =  1,
    LLWU_WAKEUP_PIN_PTE4  =  2,
    LLWU_WAKEUP_PIN_PTA4  =  3,
    LLWU_WAKEUP_PIN_PTA13 =  4,
    LLWU_WAKEUP_PIN_PTB0  =  5,
    LLWU_WAKEUP_PIN_PTC1  =  6,
    LLWU_WAKEUP_PIN_PTC3  =  7,
    LLWU_WAKEUP_PIN_PTC4  =  8,
    LLWU_WAKEUP_PIN_PTC5  =  9,
    LLWU_WAKEUP_PIN_PTC6  = 10,
    LLWU_WAKEUP_PIN_PTC11 = 11,
    LLWU_WAKEUP_PIN_PTD0  = 12,
    LLWU_WAKEUP_PIN_PTD2  = 13,
    LLWU_WAKEUP_PIN_PTD4  = 14,
    LLWU_WAKEUP_PIN_PTD6  = 15,
    LLWU_WAKEUP_PIN_NUMOF,
    LLWU_WAKEUP_PIN_UNDEF
} llwu_wakeup_pin_t;
#endif

/**
 * @brief LLWU wakeup pin edge settings
 */
typedef enum llwu_wakeup_edge {
    LLWU_WAKEUP_EDGE_NONE = 0,
    LLWU_WAKEUP_EDGE_RISING = 1,
    LLWU_WAKEUP_EDGE_FALLING = 2,
    LLWU_WAKEUP_EDGE_BOTH = 3,
} llwu_wakeup_edge_t;

/**
 * @name Compatibility definitions between vendor headers
 * @{
 */
/* Different versions of vendor headers use some variations of register names.
 * This section aims to eliminate these differences in the few places where it
 * matters for the RIOT driver implementations.
 */
#if !defined(MCG_C2_RANGE0) && defined(MCG_C2_RANGE)
#define MCG_C2_RANGE0(x)        MCG_C2_RANGE(x)
#endif
#if !defined(MCG_C2_RANGE0_MASK) && defined(MCG_C2_RANGE_MASK)
#define MCG_C2_RANGE0_MASK      MCG_C2_RANGE_MASK
#endif
#if !defined(MCG_C7_OSCSEL) && defined(MCG_C7_OSCSEL_SHIFT)
#define MCG_C7_OSCSEL(x)        (((uint32_t)(x) << MCG_C7_OSCSEL_SHIFT) & MCG_C7_OSCSEL_MASK)
#endif
#if !defined(OSC0) && defined(OSC)
#define OSC0 OSC
#endif
#if !defined(SIM_SOPT2_LPUART0SRC_MASK) && defined(SIM_SOPT2_LPUARTSRC_MASK)
#define SIM_SOPT2_LPUART0SRC_MASK   SIM_SOPT2_LPUARTSRC_MASK
#define SIM_SOPT2_LPUART0SRC_SHIFT  SIM_SOPT2_LPUARTSRC_SHIFT
#define SIM_SOPT2_LPUART0SRC        SIM_SOPT2_LPUARTSRC
#endif
#if !defined(SIM_SCGC5_LPTMR_SHIFT) && defined(SIM_SCGC5_LPTIMER_SHIFT)
#define SIM_SCGC5_LPTMR_SHIFT   SIM_SCGC5_LPTIMER_SHIFT
#endif
#if !defined(SIM_SCGC5_LPTMR_MASK) && defined(SIM_SCGC5_LPTIMER_MASK)
#define SIM_SCGC5_LPTMR_MASK    SIM_SCGC5_LPTIMER_MASK
#endif
#if !defined(GPIOA_BASE) && defined(PTA_BASE)
#define GPIOA_BASE PTA_BASE
#define GPIOB_BASE PTB_BASE
#define GPIOC_BASE PTC_BASE
#define GPIOD_BASE PTD_BASE
#define GPIOE_BASE PTE_BASE
#define GPIOF_BASE PTF_BASE
#define GPIOG_BASE PTG_BASE
#endif
#if !defined(GPIOA) && defined(PTA)
#define GPIOA PTA
#define GPIOB PTB
#define GPIOC PTC
#define GPIOD PTD
#define GPIOE PTE
#define GPIOF PTF
#define GPIOG PTG
#endif
/** @} */

/**
 * @name Timer hardware information
 * @{
 */
#ifdef SIM_SCGC5_LPTMR_SHIFT
/** Enable LPTMR clock gate */
#define LPTMR_CLKEN()  (bit_set32(&SIM->SCGC5, SIM_SCGC5_LPTMR_SHIFT))
#endif
#ifdef SIM_SCGC6_PIT_SHIFT
/** Enable PIT clock gate */
#define PIT_CLKEN()    (bit_set32(&SIM->SCGC6, SIM_SCGC6_PIT_SHIFT))
#endif
#ifdef SIM_SCGC6_RTC_SHIFT
/** Enable RTC clock gate */
#define RTC_CLKEN()    (bit_set32(&SIM->SCGC6, SIM_SCGC6_RTC_SHIFT))
#endif
/** @} */

/**
 * @name    I2C hardware information
 * @{
 */
#ifdef SIM_SCGC4_I2C0_SHIFT
/** Enable I2C0 clock gate */
#define I2C0_CLKEN()    (bit_set32(&SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT))
#endif
#ifdef SIM_SCGC4_I2C1_SHIFT
/** Enable I2C1 clock gate */
#define I2C1_CLKEN()    (bit_set32(&SIM->SCGC4, SIM_SCGC4_I2C1_SHIFT))
#endif
#ifdef SIM_SCGC1_I2C2_SHIFT
/** Enable I2C2 clock gate */
#define I2C2_CLKEN()    (bit_set32(&SIM->SCGC1, SIM_SCGC1_I2C2_SHIFT))
#endif
/** @} */

/**
 * @name Hardware random number generator module configuration
 * @{
 */
#if !defined(HWRNG_CLKEN) && defined(RNG) && !defined(RNG_CMD_ST_MASK)
#define KINETIS_RNGA RNG
#if defined(SIM_SCGC3_RNGA_SHIFT)
#define HWRNG_CLKEN()       (bit_set32(&SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT))
#define HWRNG_CLKDIS()      (bit_clear32(&SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT))
#elif defined(SIM_SCGC6_RNGA_SHIFT)
#define HWRNG_CLKEN()       (bit_set32(&SIM->SCGC6, SIM_SCGC6_RNGA_SHIFT))
#define HWRNG_CLKDIS()      (bit_clear32(&SIM->SCGC6, SIM_SCGC6_RNGA_SHIFT))
#endif
#endif /* KINETIS_RNGA */
/** @} */

/**
 * @name MK20D7 Compatibility layer
 * @{
 */
#if defined(MK20D7_H_)
#define LLWU_IRQn LLW_IRQn
#define LPTMR0_IRQn LPTimer_IRQn
#define PIT_TCTRL_CHN_MASK   (0x4u)
#define PIT_TCTRL_CHN_SHIFT  (2)
#define PORT_IRQS   { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn, PORTD_IRQn, PORTE_IRQn }
#endif /* MK20D7_H_ */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_KINETIS_H */
/** @} */
