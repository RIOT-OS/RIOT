/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_lm4f120
 * @ingroup         cpu cpu_lm4f120
 * @{
 *
 * @file            cpu_conf.h
 * @brief           CPU specific implementations for LM4F120
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include <stdio.h>
#include <stdlib.h>
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_nvic.h"
#include "hw_sysctl.h"
#include "hw_types.h"
#include "lm4f120h5qr.h"
#include "cortex-m4-def.h"
#include "stellaris_periph/cpu.h"
#include "stellaris_periph/debug.h"
#include "stellaris_periph/interrupt.h"
#include "stellaris_periph/sysctl.h"
#include "stellaris_periph/adc.h"
#include "stellaris_periph/gpio.h"
#include "stellaris_periph/timer.h"
#include "stellaris_periph/pin_map.h"
#include "stellaris_periph/uart.h"
#include "stellaris_periph/fpu.h"
#include "stellaris_periph/rom.h"
#include "hwtimer_cpu.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#define CPUID_ID_LEN                    (12)
#define CPUID_ADDR                      NVIC_CPUID
/** @} */

/**
 * @name CC110X buffer size definitions for the stm32f4
 * @{
 */
#ifdef MODULE_CC110X
#define TRANSCEIVER_BUFFER_SIZE         (10)
#define RX_BUF_SIZE                     (10)
#endif
/** @} */

/**
 * @name cpu functions
 * @{
 */
extern void setup_fpu(void);
extern void cpu_clock_init(int);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
