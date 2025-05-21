/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx, CC13xx definitions
 *
 * @author          Leon M. George <leon@georgemail.eu>
 * @author          Anton Gerasimov <tossel@gmail.com>
 */


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Unsigned 8-bit register type.
 */
typedef volatile uint8_t reg8_t;
/**
 * @brief Unsigned 16-bit register type.
 */
typedef volatile uint16_t reg16_t;
/**
 * @brief Unsigned 32-bit register type.
 */
typedef volatile uint32_t reg32_t;

/**
 * @brief   Masked 8-bit register
 */
typedef struct {
    reg8_t LOW; /**< Low 4-bit half */
    reg8_t HIGH; /**< High 4-bit half */
} reg8_m4_t;

/**
 * @brief   Masked 8-bit register
 */
typedef reg16_t reg8_m8_t;

/**
 * @brief   Masked 32-bit register
 */
typedef struct {
    reg32_t LOW; /**< Low 16-bit half */
    reg32_t HIGH; /**< High 16-bit half */
} reg32_m16_t;

/**
 * @addtogroup CC13x2_cmsis CMSIS Definitions
 * @{
 */
/**
 * @brief Interrupt number definition
 */
typedef enum IRQn {
    /******  Cortex-M4 Processor Exceptions Numbers ****************************/
    ResetHandler_IRQn     = -15, /**<  1 Reset Handler                         */
    NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
    HardFault_IRQn        = -13, /**<  3 Cortex-M4 Hard Fault Interrupt        */
    MemoryManagement_IRQn = -12, /**<  4 Cortex-M4 Memory Management Interrupt */
    BusFault_IRQn         = -11, /**<  5 Cortex-M4 Bus Fault Interrupt         */
    UsageFault_IRQn       = -10, /**<  6 Cortex-M4 Usage Fault Interrupt       */
    SVCall_IRQn           = - 5, /**< 11 Cortex-M4 SV Call Interrupt           */
    DebugMonitor_IRQn     = - 4, /**< 12 Cortex-M4 Debug Monitor Interrupt     */
    PendSV_IRQn           = - 2, /**< 14 Cortex-M4 Pend SV Interrupt           */
    SysTick_IRQn          = - 1, /**< 15 Cortex-M4 System Tick Interrupt       */

    /******  CC13x2 specific Interrupt Numbers *********************************/
    EDGE_DETECT_IRQN      =   0, /**< 16 AON edge detect                        */
    I2C_IRQN              =   1, /**< 17 I2C                                    */
    RF_CPE1_IRQN          =   2, /**< 18 RF Command and Packet Engine 1         */
    PKA_IRQN              =   3, /**< 19 PKA interrupt                          */
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
#ifdef CPU_VARIANT_X2
    OSC_IRQN              =  34, /**< 50 Combined event from oscillator control */
    AUX_TIMER2_IRQN       =  35, /**< 51 AUX Timer 2 event 0                    */
    UART1_IRQN            =  36, /**< 52 UART1 combined interrupt               */
    BATMON_IRQN           =  37, /**< 53 UART1 combined interrupt               */

    IRQN_COUNT            = (BATMON_IRQN + 1) /**< Number of peripheral IDs */
#else
    IRQN_COUNT            = (TRNG_IRQN + 1) /**< Number of peripheral IDs */
#endif

} IRQn_Type;

/**
 * @brief Configuration of the Cortex-M4 processor and core peripherals
 * @{
 */
#define __MPU_PRESENT          1      /**< CC13x2 does provide a MPU */
#define __NVIC_PRIO_BITS       3      /**< CC13x2 offers priority levels from 0..7 */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick config is used */
/** @} */

#define RCOSC48M_FREQ               48000000 /**< 48 MHz */
#define RCOSC24M_FREQ               24000000 /**< 24 MHz */

/**
 * @brief CMSIS includes
 */
#ifdef CPU_VARIANT_X2
#include <core_cm4.h>
#else
#include <core_cm3.h>
#endif
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define FLASH_BASE         0x00000000 /**< FLASH base address */
#define PERIPH_BASE        0x40000000 /**< Peripheral base address */
#define PERIPH_BASE_NONBUF 0x60000000 /**< Peripheral base address (nonbuf) */
#define ROM_HARD_API_BASE  0x10000048 /**< ROM Hard-API base address */
#define ROM_API_TABLE      ((uint32_t *) 0x10000180) /**< ROM API table */
/** @} */

/**
 * @brief   ADI master instruction offsets
 * @{
 */
#define ADI_DIR            0x00000000
#define ADI_SET            0x00000010
#define ADI_CLR            0x00000020
#define ADI_MASK4B         0x00000040
#define ADI_MASK8B         0x00000060
#define ADI_MASK16B        0x00000080
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
