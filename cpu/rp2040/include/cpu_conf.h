/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup cpu_rp2040 Raspberry Pi RP2040 MCU
 * @ingroup cpu
 * @ingroup cpu_rp2040
 * @{
 *
 * @file cpu_conf.h
 * @brief CPU specific configuration
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#define __CM0PLUS_REV (1U) // Core revision.
#define __MPU_PRESENT (0U) // MPU present or not.
#define __NVIC_PRIO_BITS (2U) // Number of bits used for priority levels.
#define __VTOR_PRESENT (1U) // VTOR present or not.
#define __Vendor_SysTickConfig (0U) // Set to 1 if different SysTick config is used.

#define __CHECK_DEVICE_DEFINES

#define CPU_DEFAULT_IRQ_PRIO (1U)
#define CPU_IRQ_NUMOF (26U)

typedef enum {
    // ARM Cortex-M0+ specific interrupt numbers.

    Reset_IRQn = -15, // -15 Reset vector, invoked on power-up and warm reset.
    NonMaskableInt_IRQn = -14, // -14  Non-maskable interrupt, cannot be stopped or preempted.
    HardFault_IRQn = -13, // -13  Hard Fault, all classes of Fault.
    SVCall_IRQn =  -5, // -5 System service call via SVC instruction.
    PendSV_IRQn =  -2, // -2 Pendable request for system service.
    SysTick_IRQn =  -1, // -1 System tick timer.

    // RP2040 specific interrupt numbers.

    TIMER_0_IRQn = 0,
    TIMER_1_IRQn = 1,
    TIMER_2_IRQn = 2,
    TIMER_3_IRQn = 3,
    PWM_WRAP_IRQn = 4,
    USBCTRL_IRQn = 5,
    XIP_IRQn = 6,
    PIO0_0_IRQn = 7,
    PIO0_1_IRQn = 8,
    PIO1_0_IRQn = 9,
    PIO1_1_IRQn = 10,
    DMA_0_IRQn = 11,
    DMA_1_IRQn = 12,
    IO_BANK0_IRQn = 13,
    IO_QSPI_IRQn = 14,
    SIO_PROC0_IRQn = 15,
    SIO_PROC1_IRQn = 16,
    CLOCKS_IRQn = 17,
    SPI0_IRQn = 18,
    SPI1_IRQn = 19,
    UART0_IRQn = 20,
    UART1_IRQn = 21,
    ADC0_FIFO_IRQn = 22,
    I2C0_IRQn = 23,
    I2C1_IRQn = 24,
    RTC_IRQn = 25,
} IRQn_Type;

#include "cpu_conf_common.h"
#include "core_cm0plus.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
