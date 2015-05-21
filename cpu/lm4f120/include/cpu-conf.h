/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
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

#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_nvic.h"
#include "hw_sysctl.h"
#include "hw_types.h"
#include "lm4f120h5qr.h"
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
extern unsigned long CPUipsrGet(void);
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

/** __FPU_USED indicates whether an FPU is used or not.
    For this, __FPU_PRESENT has to be checked prior to making use of FPU specific registers and functions.
*/

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
