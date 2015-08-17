/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k64f Freescale K64F MCU
 * @ingroup         cpu
 * @brief           CPU specific implementations for the Freescale K64F
 *                  Kinetis Cortex-M4 MCU.
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author          Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef CPU_MODEL_MK64FN1M0VLL12
#include "MK64F12.h"
#else
#error "undefined CPU_MODEL"
#endif

#include "mcg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (86U)
#define CPU_FLASH_BASE                  (0x00000000)
/** @} */

/**
 * @brief Length for reading CPU_ID in octets
 */
#define CPUID_ID_LEN                     (16)

/**
 * @brief Pointer to CPU_ID
 */
#define CPUID_ID_PTR                     ((void *)(&(SIM_UIDH)))

/**
 * @name GPIO pin mux function numbers
 */
/** @{ */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO 1
/** @} */
/**
 * @name GPIO interrupt flank settings
 */
/** @{ */
#define PIN_INTERRUPT_RISING 0b1001
#define PIN_INTERRUPT_FALLING 0b1010
#define PIN_INTERRUPT_EDGE 0b1011
/** @} */

/** @name PORT module clock gates */
/** @{ */
#define PORTA_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTA_SHIFT))
#define PORTB_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT))
#define PORTC_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT))
#define PORTD_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT))
#define PORTE_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT))
/** @} */

/**
 * @brief MCU specific Low Power Timer settings.
 */
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_LPO
#define LPTIMER_DEV                      (LPTMR0) /**< LPTIMER hardware module */
#define LPTIMER_CLKEN()                  (SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK) /**< Enable LPTMR0 clock gate */
#define LPTIMER_CLKDIS()                 (SIM->SCGC5 &= ~SIM_SCGC5_PTMR_MASK) /**< Disable LPTMR0 clock gate */
#define LPTIMER_CNR_NEEDS_LATCHING       1 /**< LPTMR.CNR register do not need latching */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
