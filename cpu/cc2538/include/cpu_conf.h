/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "cc2538.h"
#include "cc2538-gpio.h"
#include "cc2538-uart.h"
#include "gptimer.h"
#include "ioc.h"
#include "soc-adc.h"
#include "rfcore.h"
#include "sys-ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   PERIPH_COUNT_IRQn
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief   length of CPU ID for @ref cpuid_get() in @ref periph/cpuid.h
 */
#ifndef CPUID_ID_LEN
#define CPUID_ID_LEN                    8
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __CPU_CONF_H */
/** @} */
