/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_lm4f120 LM4F
 * @ingroup         cpu
 * @brief           CPU specific implementations for LM4F120
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CPU_MODEL_TM4C1294NCPDT
#include "hw/tm4c1294ncpdt.h"
#endif

//#include "hw_ints.h"
#include "hw/hw_memmap.h"
//#include "hw_nvic.h"
//#include "hw_sysctl.h"
//#include "hw_types.h"
//#include "cortex-m4-def.h"
#include "periph_ti/cpu.h"
#include "periph_ti/debug.h"
#include "periph_ti/interrupt.h"
#include "periph_ti/sysctl.h"
#include "periph_ti/adc.h"
#include "periph_ti/gpio.h"
#include "periph_ti/timer.h"
#include "periph_ti/pin_map.h"
#include "periph_ti/uart.h"
#include "periph_ti/fpu.h"
#include "periph_ti/rom.h"
#include "periph_ti/rom_map.h"
#include "hwtimer_cpu.h"
#include "periph/uart.h"



/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (48U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief Length for reading CPU_ID
 * @{
 */
#define CPUID_ID_LEN                    (11)
#define CPUID_ADDR                      NVIC_CPUID_R
/** @} */

/**
 * @name cpu functions
 * @{
 */

extern void setup_fpu(void);
extern void cpu_clock_init(int);
extern uint32_t cpu_getClock(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
