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
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    1024

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   1024
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      512
/** @} */

/**
 * @name UART0 buffer size definition for compatibility reasons
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   128
#endif
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
