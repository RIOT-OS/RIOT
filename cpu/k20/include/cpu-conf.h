/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k20 K20
 * @ingroup         cpu_k20
 * @brief           CPU specific implementations for the K20
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H


#ifdef __cplusplus
extern "C" {
#endif


#include "k20_interrupts.h"
#include "k20_memory.h"
#include "core_cm4.h"


/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (2048)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
/** @} */

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

/**
 * @name CC110X buffer size definitions for the k20
 * @{
 */
#ifdef MODULE_CC110X
#define TRANSCEIVER_BUFFER_SIZE         (10)
#define RX_BUF_SIZE                     (10)
#endif
/** @} */

/**
 * @name Cortex M4-common configuration
 * @{
 */
#define __CM4_REV                 0x0001  /*!< Core revision r0p1                                */
#define __MPU_PRESENT             1       /*!< K20 provides an MPU                               */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used      */
#define __FPU_PRESENT             1       /*!< FPU present                                       */
/** @{ */

/**
 * @name kinetis definitions
 * @{
 */

/**
 * @name lptmr definition
 */
#define LPTMR_NUM 1

typedef enum {
    KINETIS_LPTMR_CLOCK_MCGIRCLK = 0,
    KINETIS_LPTMR_CLOCK_LPO      = 1,
    KINETIS_LPTMR_CLOCK_ERCLK32K = 2,
    KINETIS_LPTMR_CLOCK_OSCERCLK = 3
} kinetis_lptmr_clock_t;
/** @} */

/**
 * @name Clock gate definition used by the kinetis SIM driver
 *
 * The values are calculated as follows:
 * clockgate / 0x20 = id of SCG register
 * clockgate % 0x20 = shift for value
 */
typedef enum {
    /* SCGC4 */
    K20_CGATE_VREF   = 0x80 + 20,
    K20_CGATE_CMP    = 0x80 + 19,
    K20_CGATE_USBOTG = 0x80 + 18,
    K20_CGATE_UART2  = 0x80 + 12,
    K20_CGATE_UART1  = 0x80 + 11,
    K20_CGATE_UART0  = 0x80 + 10,
    K20_CGATE_I2C0   = 0x80 + 6,
    K20_CGATE_CMT    = 0x80 + 2,
    K20_CGATE_EWM    = 0x80 + 1,

    /* SCGC5 */
    K20_CGATE_PORTE  = 0xA0 + 13,
    K20_CGATE_PORTD  = 0xA0 + 12,
    K20_CGATE_PORTC  = 0xA0 + 11,
    K20_CGATE_PORTB  = 0xA0 + 10,
    K20_CGATE_PORTA  = 0xA0 + 9,
    K20_CGATE_TSI    = 0xA0 + 5,
    K20_CGATE_LPTMR  = 0xA0 + 0,

    /* SCGC6 */
    K20_CGATE_RTC    = 0xC0 + 29,
    K20_CGATE_ADC0   = 0xC0 + 27,
    K20_CGATE_FTM1   = 0xC0 + 25,
    K20_CGATE_FTM0   = 0xC0 + 24,
    K20_CGATE_PIT    = 0xC0 + 23,
    K20_CGATE_PDB    = 0xC0 + 22,
    K20_CGATE_USBDCD = 0xC0 + 21,
    K20_CGATE_CRC    = 0xC0 + 18,
    K20_CGATE_I2S    = 0xC0 + 15,
    K20_CGATE_SPI0   = 0xC0 + 12,
    K20_CGATE_DMAMUX = 0xC0 + 1,
    K20_CGATE_FTFL   = 0xC0 + 0,

    /* SCGC7 */
    K20_CGATE_DMA    = 0xE0 + 01
} kinetis_clock_gate_t;
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
