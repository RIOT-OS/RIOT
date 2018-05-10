/*
 * Copyright (C) 2017 Eistec AB
 * Copyright (C) 2018 Ishraq Ibne Ashraf
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
 * @author          Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
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

/**
 * @name MKE02Z Compatibility layer
 * @{
 */
#if defined(MKE02Z_H_)
#define PORT_PCR_MUX(n) 0
#define PORT_PCR_PE_MASK 0
#define PORT_PCR_PS_MASK 0
#define PORT_PCR_IRQC(n) 0
#define ADC_CFG1_MODE(n) 0
#define SPI_CTAR_CPHA_MASK 0
#define SPI_CTAR_CPOL_MASK 0
typedef struct {} LPTMR_Type;
#define MCG_C4_DRST_DRS(n) 0
#define MCG_C4_DMX32_MASK 0
#define MCG_C2_RANGE0(n) 0
#define PORTA_BASE 0
#define SIM_SCGC5_PORTA_SHIFT 0
#define PIT_CLKEN() (bit_set32(&SIM->SCGC, SIM_SCGC_PIT_SHIFT))
#endif /* MKE02Z_H_ */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_KINETIS_H */
/** @} */
