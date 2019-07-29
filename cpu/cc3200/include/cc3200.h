/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200_regs
 * @{
 *
 * @file
 * @brief           cc3200 MCU interrupt and register definitions
 *
 * Header file with register and macro declarations for the cc3200 GPIO module
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @{
 */

#ifndef CC3200_H
#define CC3200_H

#include "vendor/hw_ints.h"
#include "vendor/hw_memmap.h"
#include "vendor/hw_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile uint32_t cc3200_reg_t; /**< 32bit CC3200 default register
                                           address type */

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR CC3200 */
/* ************************************************************************** */
/** @addtogroup CC3200_cmsis CMSIS Definitions */
/*@{*/

/** Interrupt Number Definition */
typedef enum IRQn {
    /******  Cortex-M4 Processor Exceptions Numbers ***************************/
    ResetHandler_IRQn       = -15,  /**<  1 Reset Handler                         */
    NonMaskableInt_IRQn     = -14,  /**<  2 Non Maskable Interrupt                */
    HardFault_IRQn          = -13,  /**<  3 Cortex-M4 Hard Fault Interrupt        */
    MemoryManagement_IRQn   = -12,  /**<  4 Cortex-M4 Memory Management Interrupt
                                     */
    BusFault_IRQn           = -11,  /**<  5 Cortex-M4 Bus Fault Interrupt         */
    UsageFault_IRQn         = -10,  /**<  6 Cortex-M4 Usage Fault Interrupt       */
    SVCall_IRQn             = -5,   /**< 11 Cortex-M4 SV Call Interrupt           */
    DebugMonitor_IRQn       = -4,   /**< 12 Cortex-M4 Debug Monitor Interrupt     */
    PendSV_IRQn             = -2,   /**< 14 Cortex-M4 Pend SV Interrupt           */
    SysTick_IRQn            = -1,   /**< 15 Cortex-M4 System Tick Interrupt       */

    /******  CC3200 specific Interrupt Numbers ********************************/
    GPIO_PORT_A0_IRQn       = 0,                /**<    GPIO port A                          */
    GPIO_PORT_A1_IRQn       = 1,                /**<    GPIO port B                          */
    GPIO_PORT_A2_IRQn       = 2,                /**<    GPIO port C                          */
    GPIO_PORT_A3_IRQn       = 3,                /**<    GPIO port D                          */
    UART0_IRQn              = 5,                /**<    UART0                                */
    UART1_IRQn              = 6,                /**<    UART1                                */
    I2C_IRQn                = 8,                /**<    I2C                                  */
    ADC0_IRQn               = 14,               /**<    ADC Sequence 0                       */
    ADC1_IRQn               = 15,               /**<    ADC Sequence 1                       */
    ADC2_IRQn               = 16,               /**<    ADC Sequence 2                       */
    ADC3_IRQn               = 17,               /**<    ADC Sequence 3                       */
    WDT_IRQn                = 18,               /**<    Watchdog Timer                       */
    GPTIMER_0A_IRQn         = 19,               /**<    GPTimer 0A                           */
    GPTIMER_0B_IRQn         = 20,               /**<    GPTimer 0B                           */
    GPTIMER_1A_IRQn         = 21,               /**<    GPTimer 1A                           */
    GPTIMER_1B_IRQn         = 22,               /**<    GPTimer 1B                           */
    GPTIMER_2A_IRQn         = 23,               /**<    GPTimer 2A                           */
    GPTIMER_2B_IRQn         = 24,               /**<    GPTimer 2B                           */
    FLASH_CTRL_IRQn         = 29,               /**<    Flash memory control                 */
    GPTIMER_3A_IRQn         = 35,               /**<    GPTimer 3A                           */
    GPTIMER_3B_IRQn         = 36,               /**<    GPTimer 3B                           */
    UDMA_IRQn               = 46,               /**<    uDMA software                        */
    UDMA_ERR_IRQn           = 47,               /**<    uDMA error                           */
    SHA_IRQn                = 148,              /**<    SHA                                  */
    AES_IRQn                = 151,              /**<    AES                                  */
    DES_IRQn                = 153,              /**<    DES                                  */
    MMCHS_IRQn              = 159,              /**<    SDIO                                */
    I2S_IRQn                = 161,              /**<    McAPS                                */
    CAMERA_IRQn             = 163,              /**<    Camera                               */
    NWPIC_IRQn              = 171,              /**<    AES                                  */
    PRCM_IRQn               = 172,              /**<    Power, Reset and Clock Module        */
    SSPI_IRQn               = 175,              /**<    Shared SPI                           */
    GSPI_IRQn               = 176,              /**<    Generic SPI                          */
    LSPI_IRQn               = 177,              /**<    Link SPI                             */
    IRQN_COUNT              = (LSPI_IRQn + 1)   /**< Number of peripheral IDs */
} IRQn_Type;
/** @} */

/**
 * @brief Configuration of the Cortex-M4 Processor and Core Peripherals
 * @{
 */
#define __CM4_REV                                                          \
    0x0001                      /**< CC2538 core revision number ([15:8] revision number, [7:0] \
                                   patch number) */
#define __MPU_PRESENT 0         /**< CC3200 does not provides an MPU */
#define __NVIC_PRIO_BITS \
    3                           /**< CC3200 uses 3 Bits for Interrupt Priority Levels */
#define __Vendor_SysTickConfig \
    0                           /**< Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT 0         /**< CC3200 has not FPU */

#define CC3200_VTOR_ALIGN 1024  /**< CC3200 Vector Table alignment */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#include "core_cm4.h"

#endif /* CC3200_H */
/** @} */
