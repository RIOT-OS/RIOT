/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_kinetis_common
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

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
/* Each interrupt priority setting is 8 bits wide, for both CM4 and CM0+, but
 * they are laid out differently. CM0+ concatenates the settings into 32 bit
 * words, CM4 and CM7 uses direct 8 bit access */
#define CPU_IRQ_NUMOF                   (sizeof(NVIC->IP))
#define CPU_FLASH_BASE                  (0x00000000)
/** @} */

/**
 * @name GPIO pin mux function numbers
 * @{
 */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO 1
/** @} */

/**
 * @name GPIO interrupt flank settings
 * @{
 */
#define PIN_INTERRUPT_RISING 0b1001
#define PIN_INTERRUPT_FALLING 0b1010
#define PIN_INTERRUPT_EDGE 0b1011
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
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_KINETIS_H */
/** @} */
