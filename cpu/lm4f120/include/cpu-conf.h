/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_lm4f120
 * @ingroup         cpu
 * @brief           CPU specific implementations for the Stellaris Launchpad LM4F120 board
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *12
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/cpu.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/fpu.h"
#include "periph/uart.h"
#include "stdio.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Length for reading CPU_ID
 */
#define CPUID_ID_LEN                    (12)

#define CPUID_ADDR						NVIC_CPUID					

/**
 * @name Kernel configuration
 *
 * The absolute minimum stack size is 140 byte (68 byte for the tcb + 72 byte
 * for a complete context save).
 *
 * TODO: measure and adjust for the Cortex-M3
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (1024)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
/** @} */
extern int uart_init_testing(uart_t uart, uint32_t baudrate);
extern void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
