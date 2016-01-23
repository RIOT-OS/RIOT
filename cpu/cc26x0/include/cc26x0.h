/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 MCU interrupt definitions
 *
 * @author          Leon M. George <leon@georgemail.eu>
 */

#ifndef CC26x0_H
#define CC26x0_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile uint8_t reg8_t;
typedef volatile uint32_t reg32_t;

/** @addtogroup CC26x0_cmsis CMSIS Definitions */
/*@{*/

/** interrupt number definition */
typedef enum IRQn
{
    /******  Cortex-M3 Processor Exceptions Numbers ****************************/
    ResetHandler_IRQn     = -15, /**<  1 Reset Handler                         */
    NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
    HardFault_IRQn        = -13, /**<  3 Cortex-M3 Hard Fault Interrupt        */
    MemoryManagement_IRQn = -12, /**<  4 Cortex-M3 Memory Management Interrupt */
    BusFault_IRQn         = -11, /**<  5 Cortex-M3 Bus Fault Interrupt         */
    UsageFault_IRQn       = -10, /**<  6 Cortex-M3 Usage Fault Interrupt       */
    SVCall_IRQn           = - 5, /**< 11 Cortex-M3 SV Call Interrupt           */
    DebugMonitor_IRQn     = - 4, /**< 12 Cortex-M3 Debug Monitor Interrupt     */
    PendSV_IRQn           = - 2, /**< 14 Cortex-M3 Pend SV Interrupt           */
    SysTick_IRQn          = - 1, /**< 15 Cortex-M3 System Tick Interrupt       */

    /******  CC26x0 specific Interrupt Numbers *********************************/
    EDGE_DETECT_IRQN      =   0, /**< 16 AON edge detect                        */
    I2C_IRQN              =   1, /**< 17 I2C                                    */
    RF_CPE1_IRQN          =   2, /**< 18 RF Command and Packet Engine 1         */
    SPIS_IRQN             =   3, /**< 19 AON SpiSplave Rx, Tx and CS            */
    AON_RTC_IRQN          =   4, /**< 20 AON RTC                                */
    UART0_IRQN            =   5, /**< 21 UART0 Rx and Tx                        */
    AON_AUX_SWEV0_IRQN    =   6, /**< 22 Sensor Controller software event 0, through AON domain*/
    SSI0_IRQN             =   7, /**< 23 SSI0 Rx and Tx                         */
    SSI1_IRQN             =   8, /**< 24 SSI1 Rx and Tx                         */
    RF_CPE0_IRQN          =   9, /**< 25 RF Command and Packet Engine 0         */
    RF_HW_IRQN            =  10, /**< 26 RF Core Hardware                       */
    RF_CMD_ACK_IRQN       =  11, /**< 27 RF Core Command Acknowledge            */
    I2S_IRQN              =  12, /**< 28 I2S                                    */
    AON_AUX_SWEV1_IRQN    =  13, /**< 29 Sensor Controller software event 1, through AON domain*/
    WATCHDOG_IRQN         =  14, /**< 30 Watchdog timer                         */
    GPTIMER_0A_IRQN       =  15, /**< 31 Timer 0 subtimer A                     */
    GPTIMER_0B_IRQN       =  16, /**< 32 Timer 0 subtimer B                     */
    GPTIMER_1A_IRQN       =  17, /**< 33 Timer 1 subtimer A                     */
    GPTIMER_1B_IRQN       =  18, /**< 34 Timer 1 subtimer B                     */
    GPTIMER_2A_IRQN       =  19, /**< 35 Timer 2 subtimer A                     */
    GPTIMER_2B_IRQN       =  20, /**< 36 Timer 2 subtimer B                     */
    GPTIMER_3A_IRQN       =  21, /**< 37 Timer 3 subtimer A                     */
    GPTIMER_3B_IRQN       =  22, /**< 38 Timer 3 subtimer B                     */
    CRYPTO_IRQN           =  23, /**< 39 Crypto Core Result available           */
    UDMA_IRQN             =  24, /**< 40 uDMA Software                          */
    UDMA_ERR_IRQN         =  25, /**< 41 uDMA Error                             */
    FLASH_CTRL_IRQN       =  26, /**< 42 Flash controller                       */
    SW0_IRQN              =  27, /**< 43 Software Event 0                       */
    AUX_COMBO_IRQN        =  28, /**< 44 AUX combined event, directly to MCU domain*/
    AON_PRG0_IRQN         =  29, /**< 45 AON programmable 0                     */
    PROG_IRQN             =  30, /**< 46 Dynamic Programmable interrupt (default source: PRCM)*/
    AUX_COMPA_IRQN        =  31, /**< 47 AUX Comparator A                       */
    AUX_ADC_IRQN          =  32, /**< 48 AUX ADC IRQ                            */
    TRNG_IRQN             =  33, /**< 49 TRNG event                             */

    IRQN_COUNT            = (TRNG_IRQN + 1) /**< Number of peripheral IDs */
} IRQn_Type;

/**
 * @brief Configuration of the Cortex-M3 processor and core peripherals
 */

#define __MPU_PRESENT          1      /**< CC26x0 does provide a MPU */
#define __NVIC_PRIO_BITS       3      /**< CC26x0 offers priority levels from 0..7 */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick config is used */

#define RCOSC48M_FREQ               48000000 /**< 48 MHz */
#define RCOSC24M_FREQ               24000000 /**< 24 MHz */

/**
 * @brief CMSIS includes
 */
#include <core_cm3.h>
/*@}*/

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define FLASH_BASE                  0x00000000 /**< FLASH base address */
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* CC26x0_H */

/*@}*/
