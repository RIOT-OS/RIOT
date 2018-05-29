/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 *
 * @addtogroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       CC3200 MCU definitions
 *
 * @author      Attilio Dona' <attilio.dona>
 *
 */

#ifndef CPU_INCLUDE_CC3200_H_
#define CPU_INCLUDE_CC3200_H_

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin.h"
#include "driverlib/gpio.h"
#include "driverlib/spi.h"
#include "driverlib/prcm.h"
#include "nwp_conf.h"


#ifdef __cplusplus
extern "C" {
#endif

extern const uint32_t port_to_base[];
extern const uint8_t digital_pin_to_port[];
extern const uint16_t digital_pin_to_pin_num[];
extern const uint8_t digital_pin_to_bit_mask[];

#define digitalPinToPort(P)       ( digital_pin_to_port[P] )
#define digitalPinToPinNum(P)     ( digital_pin_to_pin_num[P] )
#define digitalPinToBitMask(P)    ( digital_pin_to_bit_mask[P] )
#define portBASERegister(P)       ((volatile uint32_t *) port_to_base[P])


#define __CM4_REV                 0x0001  /*! Core revision r0p1              */
#define __MPU_PRESENT             1       /*! CC3200 does not provides an MPU */
#define __NVIC_PRIO_BITS          3       /*! CC3200 uses 3 Bits              */
#define __Vendor_SysTickConfig    0       /*! Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             0       /*! FPU notpresent                  */

#define __ASM            __asm
#define __STATIC_INLINE  static inline

#define SW_TIMERS_EXAUSTED 111

#define SYSTEM_RESET  cc3200_reset()

typedef enum {
    /******  Cortex-M4 Processor Exceptions Numbers ***************************/
    NonMaskableInt_IRQn = -14, /*!< 2 Non Maskable Interrupt                  */
    MemoryManagement_IRQn = -12, /*!< 4 Cortex-M4 Memory Management Interrupt */
    BusFault_IRQn = -11, /*!< 5 Cortex-M4 Bus Fault Interrupt                 */
    UsageFault_IRQn = -10, /*!< 6 Cortex-M4 Usage Fault Interrupt             */
    SVCall_IRQn = -5, /*!< 11 Cortex-M4 SV Call Interrupt                     */
    DebugMonitor_IRQn = -4, /*!< 12 Cortex-M4 Debug Monitor Interrupt         */
    PendSV_IRQn = -2, /*!< 14 Cortex-M4 Pend SV Interrupt                     */
    SysTick_IRQn = -1, /*!< 15 Cortex-M4 System Tick Interrupt                */

    /******  CC3200 specific Interrupt Numbers ********************************/
    GPIO_PORT_A0_IRQn = 0, /**<    GPIO port A                          */
    GPIO_PORT_A1_IRQn = 1, /**<    GPIO port B                          */
    GPIO_PORT_A2_IRQn = 2, /**<    GPIO port C                          */
    GPIO_PORT_A3_IRQn = 3, /**<    GPIO port D                          */
    UART0_IRQn = 5, /**<    UART0                                */
    UART1_IRQn = 6, /**<    UART1                                */
    I2C_IRQn = 8, /**<    I2C                                  */
    ADC0_IRQn = 14, /**<    ADC Sequence 0                       */
    ADC1_IRQn = 15, /**<    ADC Sequence 1                       */
    ADC2_IRQn = 16, /**<    ADC Sequence 2                       */
    ADC3_IRQn = 17, /**<    ADC Sequence 3                       */
    WDT_IRQn = 18, /**<    Watchdog Timer                       */
    GPTIMER_0A_IRQn = 19, /**<    GPTimer 0A                           */
    GPTIMER_0B_IRQn = 20, /**<    GPTimer 0B                           */
    GPTIMER_1A_IRQn = 21, /**<    GPTimer 1A                           */
    GPTIMER_1B_IRQn = 22, /**<    GPTimer 1B                           */
    GPTIMER_2A_IRQn = 23, /**<    GPTimer 2A                           */
    GPTIMER_2B_IRQn = 24, /**<    GPTimer 2B                           */
    FLASH_CTRL_IRQn = 29, /**<    Flash memory control                 */
    GPTIMER_3A_IRQn = 35, /**<    GPTimer 3A                           */
    GPTIMER_3B_IRQn = 36, /**<    GPTimer 3B                           */
    UDMA_IRQn = 46, /**<    uDMA software                        */
    UDMA_ERR_IRQn = 47, /**<    uDMA error                           */
    SHA_IRQn = 148, /**<    SHA                                  */
    AES_IRQn = 151, /**<    AES                                  */
    DES_IRQn = 153, /**<    DES                                  */
    MMCHS_IRQn = 159, /**<    SDIO                                */
    I2S_IRQn = 161, /**<    McAPS                                */
    CAMERA_IRQn = 163, /**<    Camera                               */
    NWPIC_IRQn = 171, /**<    AES                                  */
    PRCM_IRQn = 172, /**<    Power, Reset and Clock Module        */
    SSPI_IRQn = 175, /**<    Shared SPI                           */
    GSPI_IRQn = 176, /**<    Generic SPI                          */
    LSPI_IRQn = 177, /**<    Link SPI                             */

    PERIPH_COUNT_IRQn = (LSPI_IRQn + 1) /**< Number of peripheral IDs */

} IRQn_Type;

#ifdef __cplusplus
} /* end extern "C" */
#endif

#include "core_cm4.h"

/** @addtogroup cpu_specific_Peripheral_memory_map
 * @{
 */

#define START_OF_RAM                0x20004000 /**< SRAM base address */
#define PERIPH_BASE                 0x40000000 /**< Peripheral base address */

/** @} */

/**
 * reset the chip
 */
void cc3200_reset(void);


#endif /* CPU_INCLUDE_CC3200_H_ */
/** @} */

