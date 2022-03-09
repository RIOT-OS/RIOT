/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538_regs
 * @{
 *
 * @file
 * @brief           CC2538 MCU interrupt and register definitions
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

#ifndef CC2538_H
#define CC2538_H

#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR CC2538 */
/* ************************************************************************** */
/**
 * @addtogroup CC2538_cmsis CMSIS Definitions
 * @{
 */
/** Interrupt Number Definition */
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

    /******  CC2538 specific Interrupt Numbers *********************************/
    GPIO_PORT_A_IRQn      =   0, /**<    GPIO port A                           */
    GPIO_PORT_B_IRQn      =   1, /**<    GPIO port B                           */
    GPIO_PORT_C_IRQn      =   2, /**<    GPIO port C                           */
    GPIO_PORT_D_IRQn      =   3, /**<    GPIO port D                           */
    UART0_IRQn            =   5, /**<    UART0                                 */
    UART1_IRQn            =   6, /**<    UART1                                 */
    SSI0_IRQn             =   7, /**<    SSI0                                  */
    I2C_IRQn              =   8, /**<    I2C                                   */
    ADC_IRQn              =  14, /**<    ADC                                   */
    WDT_IRQn              =  18, /**<    Watchdog Timer                        */
    GPTIMER_0A_IRQn       =  19, /**<    GPTimer 0A                            */
    GPTIMER_0B_IRQn       =  20, /**<    GPTimer 0B                            */
    GPTIMER_1A_IRQn       =  21, /**<    GPTimer 1A                            */
    GPTIMER_1B_IRQn       =  22, /**<    GPTimer 1B                            */
    GPTIMER_2A_IRQn       =  23, /**<    GPTimer 2A                            */
    GPTIMER_2B_IRQn       =  24, /**<    GPTimer 2B                            */
    ADC_CMP_IRQn          =  25, /**<    Analog Comparator                     */
    RF_RXTX_ALT_IRQn      =  26, /**<    RF TX/RX (Alternate)                  */
    RF_ERR_ALT_IRQn       =  27, /**<    RF Error (Alternate)                  */
    SYS_CTRL_IRQn         =  28, /**<    System Control                        */
    FLASH_CTRL_IRQn       =  29, /**<    Flash memory control                  */
    AES_ALT_IRQn          =  30, /**<    AES (Alternate)                       */
    PKA_ALT_IRQn          =  31, /**<    PKA (Alternate)                       */
    SM_TIMER_ALT_IRQn     =  32, /**<    SM Timer (Alternate)                  */
    MAC_TIMER_ALT_IRQn    =  33, /**<    MAC Timer (Alternate)                 */
    SSI1_IRQn             =  34, /**<    SSI1                                  */
    GPTIMER_3A_IRQn       =  35, /**<    GPTimer 3A                            */
    GPTIMER_3B_IRQn       =  36, /**<    GPTimer 3B                            */
    UDMA_IRQn             =  46, /**<    uDMA software                         */
    UDMA_ERR_IRQn         =  47, /**<    uDMA error                            */
    USB_IRQn              = 140, /**<    USB                                   */
    RF_RXTX_IRQn          = 141, /**<    RF Core Rx/Tx                         */
    RF_ERR_IRQn           = 142, /**<    RF Core Error                         */
    AES_IRQn              = 143, /**<    AES                                   */
    PKA_IRQn              = 144, /**<    PKA                                   */
    SM_TIMER_IRQn         = 145, /**<    SM Timer                              */
    MACTIMER_IRQn         = 146, /**<    MAC Timer                             */

    PERIPH_COUNT_IRQn     = (MACTIMER_IRQn + 1) /**< Number of peripheral IDs */
} IRQn_Type;

/**
 * @name Cortex-M3 core interrupt handlers
 * @{
 */
void Reset_Handler(void);        /**< Reset handler */
void NMI_Handler(void);          /**< NMI handler */
void HardFault_Handler(void);    /**< Hard fault handler */
void MemManage_Handler(void);    /**< Memory management handler */
void BusFault_Handler(void);     /**< Bus fault handler */
void UsageFault_Handler(void);   /**< Usage fault handler */
void SVC_Handler(void);          /**< SVC handler */
void DebugMon_Handler(void);     /**< Debug monitor handler */
void PendSV_Handler(void);       /**< PendSV handler */
void SysTick_Handler(void);      /**< SysTick handler */
/** @} */

/**
 * @brief Configuration of the Cortex-M3 Processor and Core Peripherals
 */

#define __CM3_REV              0x0200 /**< CC2538 core revision number ([15:8] revision number, [7:0] patch number) */
#define __MPU_PRESENT          1      /**< CC2538 does provide a MPU */
#define __NVIC_PRIO_BITS       3      /**< CC2538 uses 3 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick Config is used */

/**
 * @brief CMSIS includes
 */
#include <core_cm3.h>
/** @} */

#define IEEE_ADDR_MSWORD            ( *(const uint32_t*)0x00280028 ) /**< Most-significant 32 bits of the IEEE address */
#define IEEE_ADDR_LSWORD            ( *(const uint32_t*)0x0028002c ) /**< Least-significant 32 bits of the IEEE address */

typedef volatile uint32_t cc2538_reg_t; /**< Least-significant 32 bits of the IEEE address */

/**
 * @addtogroup cpu_specific_Peripheral_memory_map
 * @{
 */
#define FLASH_BASE                  0x00200000 /**< FLASH base address */
#define SRAM_BASE                   0x20000000 /**< SRAM base address */
#define PERIPH_BASE                 0x40000000 /**< Peripheral base address */

#define SRAM_BB_BASE                0x22000000 /**< SRAM base address in the bit-band region */
/** @} */

/** @name CC2538 Special Function Registers
 * @{
 */
#define SSI0_CR0                    ( *(cc2538_reg_t*)0x40008000 ) /**< SSI0 Control Register 0 */
#define SSI0_CR1                    ( *(cc2538_reg_t*)0x40008004 ) /**< SSI0 Control Register 1 */
#define SSI0_DR                     ( *(cc2538_reg_t*)0x40008008 ) /**< SSI0 Data register */
#define SSI0_SR                     ( *(cc2538_reg_t*)0x4000800c ) /**< SSI0 FIFO/busy Status Register */
#define SSI0_CPSR                   ( *(cc2538_reg_t*)0x40008010 ) /**< SSI0 Clock Register */
#define SSI0_IM                     ( *(cc2538_reg_t*)0x40008014 ) /**< SSI0 Interrupt Mask register */
#define SSI0_RIS                    ( *(cc2538_reg_t*)0x40008018 ) /**< SSI0 Raw Interrupt Status register */
#define SSI0_MIS                    ( *(cc2538_reg_t*)0x4000801c ) /**< SSI0 Masked Interrupt Status register */
#define SSI0_ICR                    ( *(cc2538_reg_t*)0x40008020 ) /**< SSI0 Interrupt Clear Register */
#define SSI0_DMACTL                 ( *(cc2538_reg_t*)0x40008024 ) /**< SSI0 uDMA Control Register. */
#define SSI0_CC                     ( *(cc2538_reg_t*)0x40008fc8 ) /**< SSI0 clock configuration */
#define SSI1_CR0                    ( *(cc2538_reg_t*)0x40009000 ) /**< SSI1 Control Register 0 */
#define SSI1_CR1                    ( *(cc2538_reg_t*)0x40009004 ) /**< SSI1 Control Register 1 */
#define SSI1_DR                     ( *(cc2538_reg_t*)0x40009008 ) /**< SSI1 Data register */
#define SSI1_SR                     ( *(cc2538_reg_t*)0x4000900c ) /**< SSI1 FIFO/busy Status Register */
#define SSI1_CPSR                   ( *(cc2538_reg_t*)0x40009010 ) /**< SSI1 Clock Register */
#define SSI1_IM                     ( *(cc2538_reg_t*)0x40009014 ) /**< SSI1 Interrupt Mask register */
#define SSI1_RIS                    ( *(cc2538_reg_t*)0x40009018 ) /**< SSI1 Raw Interrupt Status register */
#define SSI1_MIS                    ( *(cc2538_reg_t*)0x4000901c ) /**< SSI1 Masked Interrupt Status register */
#define SSI1_ICR                    ( *(cc2538_reg_t*)0x40009020 ) /**< SSI1 Interrupt Clear Register */
#define SSI1_DMACTL                 ( *(cc2538_reg_t*)0x40009024 ) /**< SSI1 uDMA Control Register. */
#define SSI1_CC                     ( *(cc2538_reg_t*)0x40009fc8 ) /**< SSI1 clock configuration */
#define UART0_DR                    ( *(cc2538_reg_t*)0x4000c000 ) /**< UART0 Data Register */
#define UART0_ECR                   ( *(cc2538_reg_t*)0x4000c004 ) /**< UART0 receive status and error clear */
#define UART0_RSR                   ( *(cc2538_reg_t*)0x4000c004 ) /**< UART0 receive status and error clear */
#define UART0_FR                    ( *(cc2538_reg_t*)0x4000c018 ) /**< UART0 flag */
#define UART0_ILPR                  ( *(cc2538_reg_t*)0x4000c020 ) /**< UART0 IrDA low-power register */
#define UART0_IBRD                  ( *(cc2538_reg_t*)0x4000c024 ) /**< UART0 integer baud-rate divisor */
#define UART0_FBRD                  ( *(cc2538_reg_t*)0x4000c028 ) /**< UART0 fractional baud-rate divisor */
#define UART0_LCRH                  ( *(cc2538_reg_t*)0x4000c02c ) /**< UART0 line control */
#define UART0_CTL                   ( *(cc2538_reg_t*)0x4000c030 ) /**< UART0 control */
#define UART0_IFLS                  ( *(cc2538_reg_t*)0x4000c034 ) /**< UART0 interrupt FIFO level select */
#define UART0_IM                    ( *(cc2538_reg_t*)0x4000c038 ) /**< UART0 interrupt mask */
#define UART0_RIS                   ( *(cc2538_reg_t*)0x4000c03c ) /**< UART0 raw interrupt status */
#define UART0_MIS                   ( *(cc2538_reg_t*)0x4000c040 ) /**< UART0 masked interrupt status */
#define UART0_ICR                   ( *(cc2538_reg_t*)0x4000c044 ) /**< UART0 interrupt clear */
#define UART0_DMACTL                ( *(cc2538_reg_t*)0x4000c048 ) /**< UART0 DMA control */
#define UART0_LCTL                  ( *(cc2538_reg_t*)0x4000c090 ) /**< UART0 LIN control */
#define UART0_LSS                   ( *(cc2538_reg_t*)0x4000c094 ) /**< UART0 LIN snap shot */
#define UART0_LTIM                  ( *(cc2538_reg_t*)0x4000c098 ) /**< UART0 LIN timer */
#define UART0_NINEBITADDR           ( *(cc2538_reg_t*)0x4000c0a4 ) /**< UART0 9-bit self address */
#define UART0_NINEBITAMASK          ( *(cc2538_reg_t*)0x4000c0a8 ) /**< UART0 9-bit self address mask */
#define UART0_PP                    ( *(cc2538_reg_t*)0x4000cfc0 ) /**< UART0 peripheral properties */
#define UART0_CC                    ( *(cc2538_reg_t*)0x4000cfc8 ) /**< UART0 clock configuration */
#define UART1_DR                    ( *(cc2538_reg_t*)0x4000d000 ) /**< UART1 Data Register */
#define UART1_ECR                   ( *(cc2538_reg_t*)0x4000d004 ) /**< UART1 receive status and error clear */
#define UART1_RSR                   ( *(cc2538_reg_t*)0x4000d004 ) /**< UART1 receive status and error clear */
#define UART1_FR                    ( *(cc2538_reg_t*)0x4000d018 ) /**< UART1 flag */
#define UART1_ILPR                  ( *(cc2538_reg_t*)0x4000d020 ) /**< UART1 IrDA low-power register */
#define UART1_IBRD                  ( *(cc2538_reg_t*)0x4000d024 ) /**< UART1 integer baud-rate divisor */
#define UART1_FBRD                  ( *(cc2538_reg_t*)0x4000d028 ) /**< UART1 fractional baud-rate divisor */
#define UART1_LCRH                  ( *(cc2538_reg_t*)0x4000d02c ) /**< UART1 line control */
#define UART1_CTL                   ( *(cc2538_reg_t*)0x4000d030 ) /**< UART1 control */
#define UART1_IFLS                  ( *(cc2538_reg_t*)0x4000d034 ) /**< UART1 interrupt FIFO level select */
#define UART1_IM                    ( *(cc2538_reg_t*)0x4000d038 ) /**< UART1 interrupt mask */
#define UART1_RIS                   ( *(cc2538_reg_t*)0x4000d03c ) /**< UART1 raw interrupt status */
#define UART1_MIS                   ( *(cc2538_reg_t*)0x4000d040 ) /**< UART1 masked interrupt status */
#define UART1_ICR                   ( *(cc2538_reg_t*)0x4000d044 ) /**< UART1 interrupt clear */
#define UART1_DMACTL                ( *(cc2538_reg_t*)0x4000d048 ) /**< UART1 DMA control */
#define UART1_LCTL                  ( *(cc2538_reg_t*)0x4000d090 ) /**< UART1 LIN control */
#define UART1_LSS                   ( *(cc2538_reg_t*)0x4000d094 ) /**< UART1 LIN snap shot */
#define UART1_LTIM                  ( *(cc2538_reg_t*)0x4000d098 ) /**< UART1 LIN timer */
#define UART1_NINEBITADDR           ( *(cc2538_reg_t*)0x4000d0a4 ) /**< UART1 9-bit self address */
#define UART1_NINEBITAMASK          ( *(cc2538_reg_t*)0x4000d0a8 ) /**< UART1 9-bit self address mask */
#define UART1_PP                    ( *(cc2538_reg_t*)0x4000dfc0 ) /**< UART1 peripheral properties */
#define UART1_CC                    ( *(cc2538_reg_t*)0x4000dfc8 ) /**< UART1 clock configuration */
#define I2CM_SA                     ( *(cc2538_reg_t*)0x40020000 ) /**< I2C Master Slave address */
#define I2CM_CTRL                   ( *(cc2538_reg_t*)0x40020004 ) /**< I2C Master Control and status */
#define I2CM_STAT                   ( *(cc2538_reg_t*)0x40020004 ) /**< I2C Master Control and status */
#define I2CM_DR                     ( *(cc2538_reg_t*)0x40020008 ) /**< I2C Master Data */
#define I2CM_TPR                    ( *(cc2538_reg_t*)0x4002000c ) /**< I2C Master Timer period */
#define I2CM_IMR                    ( *(cc2538_reg_t*)0x40020010 ) /**< I2C Master Interrupt mask */
#define I2CM_RIS                    ( *(cc2538_reg_t*)0x40020014 ) /**< I2C Master Raw interrupt status */
#define I2CM_MIS                    ( *(cc2538_reg_t*)0x40020018 ) /**< I2C Master Masked interrupt status */
#define I2CM_ICR                    ( *(cc2538_reg_t*)0x4002001c ) /**< I2C Master Interrupt clear */
#define I2CM_CR                     ( *(cc2538_reg_t*)0x40020020 ) /**< I2C Master Configuration */
#define I2CS_OAR                    ( *(cc2538_reg_t*)0x40020800 ) /**< I2C Slave own address */
#define I2CS_CTRL                   ( *(cc2538_reg_t*)0x40020804 ) /**< I2C Slave Control and status */
#define I2CS_STAT                   ( *(cc2538_reg_t*)0x40020804 ) /**< I2C Slave Control and status */
#define I2CS_DR                     ( *(cc2538_reg_t*)0x40020808 ) /**< I2C Slave Data */
#define I2CS_IMR                    ( *(cc2538_reg_t*)0x4002080c ) /**< I2C Slave Interrupt mask */
#define I2CS_RIS                    ( *(cc2538_reg_t*)0x40020810 ) /**< I2C Slave Raw interrupt status */
#define I2CS_MIS                    ( *(cc2538_reg_t*)0x40020814 ) /**< I2C Slave Masked interrupt status */
#define I2CS_ICR                    ( *(cc2538_reg_t*)0x40020818 ) /**< I2C Slave Interrupt clear */
#define GPTIMER0_CFG                ( *(cc2538_reg_t*)0x40030000 ) /**< GPTM0 configuration */
#define GPTIMER0_TAMR               ( *(cc2538_reg_t*)0x40030004 ) /**< GPTM0 Timer A mode */
#define GPTIMER0_TBMR               ( *(cc2538_reg_t*)0x40030008 ) /**< GPTM0 Timer B mode */
#define GPTIMER0_CTL                ( *(cc2538_reg_t*)0x4003000c ) /**< GPTM0 control */
#define GPTIMER0_SYNC               ( *(cc2538_reg_t*)0x40030010 ) /**< GPTM0 synchronize */
#define GPTIMER0_IMR                ( *(cc2538_reg_t*)0x40030018 ) /**< GPTM0 interrupt mask */
#define GPTIMER0_RIS                ( *(cc2538_reg_t*)0x4003001c ) /**< GPTM0 raw interrupt status */
#define GPTIMER0_MIS                ( *(cc2538_reg_t*)0x40030020 ) /**< GPTM0 masked interrupt status */
#define GPTIMER0_ICR                ( *(cc2538_reg_t*)0x40030024 ) /**< GPTM0 interrupt clear */
#define GPTIMER0_TAILR              ( *(cc2538_reg_t*)0x40030028 ) /**< GPTM0 Timer A interval load */
#define GPTIMER0_TBILR              ( *(cc2538_reg_t*)0x4003002c ) /**< GPTM0 Timer B interval load */
#define GPTIMER0_TAMATCHR           ( *(cc2538_reg_t*)0x40030030 ) /**< GPTM0 Timer A match */
#define GPTIMER0_TBMATCHR           ( *(cc2538_reg_t*)0x40030034 ) /**< GPTM0 Timer B match */
#define GPTIMER0_TAPR               ( *(cc2538_reg_t*)0x40030038 ) /**< GPTM0 Timer A prescale */
#define GPTIMER0_TBPR               ( *(cc2538_reg_t*)0x4003003c ) /**< GPTM0 Timer B prescale */
#define GPTIMER0_TAPMR              ( *(cc2538_reg_t*)0x40030040 ) /**< GPTM0 Timer A prescale match */
#define GPTIMER0_TBPMR              ( *(cc2538_reg_t*)0x40030044 ) /**< GPTM0 Timer B prescale match */
#define GPTIMER0_TAR                ( *(cc2538_reg_t*)0x40030048 ) /**< GPTM0 Timer A */
#define GPTIMER0_TBR                ( *(cc2538_reg_t*)0x4003004c ) /**< GPTM0 Timer B */
#define GPTIMER0_TAV                ( *(cc2538_reg_t*)0x40030050 ) /**< GPTM0 Timer A value */
#define GPTIMER0_TBV                ( *(cc2538_reg_t*)0x40030054 ) /**< GPTM0 Timer B value */
#define GPTIMER0_TAPS               ( *(cc2538_reg_t*)0x4003005c ) /**< GPTM0 Timer A prescale snapshot */
#define GPTIMER0_TBPS               ( *(cc2538_reg_t*)0x40030060 ) /**< GPTM0 Timer B prescale snapshot */
#define GPTIMER0_TAPV               ( *(cc2538_reg_t*)0x40030064 ) /**< GPTM0 Timer A prescale value */
#define GPTIMER0_TBPV               ( *(cc2538_reg_t*)0x40030068 ) /**< GPTM0 Timer B prescale value */
#define GPTIMER0_PP                 ( *(cc2538_reg_t*)0x40030fc0 ) /**< GPTM0 peripheral properties */
#define GPTIMER1_CFG                ( *(cc2538_reg_t*)0x40031000 ) /**< GPTM1 configuration */
#define GPTIMER1_TAMR               ( *(cc2538_reg_t*)0x40031004 ) /**< GPTM1 Timer A mode */
#define GPTIMER1_TBMR               ( *(cc2538_reg_t*)0x40031008 ) /**< GPTM1 Timer B mode */
#define GPTIMER1_CTL                ( *(cc2538_reg_t*)0x4003100c ) /**< GPTM1 control */
#define GPTIMER1_SYNC               ( *(cc2538_reg_t*)0x40031010 ) /**< GPTM1 synchronize */
#define GPTIMER1_IMR                ( *(cc2538_reg_t*)0x40031018 ) /**< GPTM1 interrupt mask */
#define GPTIMER1_RIS                ( *(cc2538_reg_t*)0x4003101c ) /**< GPTM1 raw interrupt status */
#define GPTIMER1_MIS                ( *(cc2538_reg_t*)0x40031020 ) /**< GPTM1 masked interrupt status */
#define GPTIMER1_ICR                ( *(cc2538_reg_t*)0x40031024 ) /**< GPTM1 interrupt clear */
#define GPTIMER1_TAILR              ( *(cc2538_reg_t*)0x40031028 ) /**< GPTM1 Timer A interval load */
#define GPTIMER1_TBILR              ( *(cc2538_reg_t*)0x4003102c ) /**< GPTM1 Timer B interval load */
#define GPTIMER1_TAMATCHR           ( *(cc2538_reg_t*)0x40031030 ) /**< GPTM1 Timer A match */
#define GPTIMER1_TBMATCHR           ( *(cc2538_reg_t*)0x40031034 ) /**< GPTM1 Timer B match */
#define GPTIMER1_TAPR               ( *(cc2538_reg_t*)0x40031038 ) /**< GPTM1 Timer A prescale */
#define GPTIMER1_TBPR               ( *(cc2538_reg_t*)0x4003103c ) /**< GPTM1 Timer B prescale */
#define GPTIMER1_TAPMR              ( *(cc2538_reg_t*)0x40031040 ) /**< GPTM1 Timer A prescale match */
#define GPTIMER1_TBPMR              ( *(cc2538_reg_t*)0x40031044 ) /**< GPTM1 Timer B prescale match */
#define GPTIMER1_TAR                ( *(cc2538_reg_t*)0x40031048 ) /**< GPTM1 Timer A */
#define GPTIMER1_TBR                ( *(cc2538_reg_t*)0x4003104c ) /**< GPTM1 Timer B */
#define GPTIMER1_TAV                ( *(cc2538_reg_t*)0x40031050 ) /**< GPTM1 Timer A value */
#define GPTIMER1_TBV                ( *(cc2538_reg_t*)0x40031054 ) /**< GPTM1 Timer B value */
#define GPTIMER1_TAPS               ( *(cc2538_reg_t*)0x4003105c ) /**< GPTM1 Timer A prescale snapshot */
#define GPTIMER1_TBPS               ( *(cc2538_reg_t*)0x40031060 ) /**< GPTM1 Timer B prescale snapshot */
#define GPTIMER1_TAPV               ( *(cc2538_reg_t*)0x40031064 ) /**< GPTM1 Timer A prescale value */
#define GPTIMER1_TBPV               ( *(cc2538_reg_t*)0x40031068 ) /**< GPTM1 Timer B prescale value */
#define GPTIMER1_PP                 ( *(cc2538_reg_t*)0x40031fc0 ) /**< GPTM1 peripheral properties */
#define GPTIMER2_CFG                ( *(cc2538_reg_t*)0x40032000 ) /**< GPTM2 configuration */
#define GPTIMER2_TAMR               ( *(cc2538_reg_t*)0x40032004 ) /**< GPTM2 Timer A mode */
#define GPTIMER2_TBMR               ( *(cc2538_reg_t*)0x40032008 ) /**< GPTM2 Timer B mode */
#define GPTIMER2_CTL                ( *(cc2538_reg_t*)0x4003200c ) /**< GPTM2 control */
#define GPTIMER2_SYNC               ( *(cc2538_reg_t*)0x40032010 ) /**< GPTM2 synchronize */
#define GPTIMER2_IMR                ( *(cc2538_reg_t*)0x40032018 ) /**< GPTM2 interrupt mask */
#define GPTIMER2_RIS                ( *(cc2538_reg_t*)0x4003201c ) /**< GPTM2 raw interrupt status */
#define GPTIMER2_MIS                ( *(cc2538_reg_t*)0x40032020 ) /**< GPTM2 masked interrupt status */
#define GPTIMER2_ICR                ( *(cc2538_reg_t*)0x40032024 ) /**< GPTM2 interrupt clear */
#define GPTIMER2_TAILR              ( *(cc2538_reg_t*)0x40032028 ) /**< GPTM2 Timer A interval load */
#define GPTIMER2_TBILR              ( *(cc2538_reg_t*)0x4003202c ) /**< GPTM2 Timer B interval load */
#define GPTIMER2_TAMATCHR           ( *(cc2538_reg_t*)0x40032030 ) /**< GPTM2 Timer A match */
#define GPTIMER2_TBMATCHR           ( *(cc2538_reg_t*)0x40032034 ) /**< GPTM2 Timer B match */
#define GPTIMER2_TAPR               ( *(cc2538_reg_t*)0x40032038 ) /**< GPTM2 Timer A prescale */
#define GPTIMER2_TBPR               ( *(cc2538_reg_t*)0x4003203c ) /**< GPTM2 Timer B prescale */
#define GPTIMER2_TAPMR              ( *(cc2538_reg_t*)0x40032040 ) /**< GPTM2 Timer A prescale match */
#define GPTIMER2_TBPMR              ( *(cc2538_reg_t*)0x40032044 ) /**< GPTM2 Timer B prescale match */
#define GPTIMER2_TAR                ( *(cc2538_reg_t*)0x40032048 ) /**< GPTM2 Timer A */
#define GPTIMER2_TBR                ( *(cc2538_reg_t*)0x4003204c ) /**< GPTM2 Timer B */
#define GPTIMER2_TAV                ( *(cc2538_reg_t*)0x40032050 ) /**< GPTM2 Timer A value */
#define GPTIMER2_TBV                ( *(cc2538_reg_t*)0x40032054 ) /**< GPTM2 Timer B value */
#define GPTIMER2_TAPS               ( *(cc2538_reg_t*)0x4003205c ) /**< GPTM2 Timer A prescale snapshot */
#define GPTIMER2_TBPS               ( *(cc2538_reg_t*)0x40032060 ) /**< GPTM2 Timer B prescale snapshot */
#define GPTIMER2_TAPV               ( *(cc2538_reg_t*)0x40032064 ) /**< GPTM2 Timer A prescale value */
#define GPTIMER2_TBPV               ( *(cc2538_reg_t*)0x40032068 ) /**< GPTM2 Timer B prescale value */
#define GPTIMER2_PP                 ( *(cc2538_reg_t*)0x40032fc0 ) /**< GPTM2 peripheral properties */
#define GPTIMER3_CFG                ( *(cc2538_reg_t*)0x40033000 ) /**< GPTM3 configuration */
#define GPTIMER3_TAMR               ( *(cc2538_reg_t*)0x40033004 ) /**< GPTM3 Timer A mode */
#define GPTIMER3_TBMR               ( *(cc2538_reg_t*)0x40033008 ) /**< GPTM3 Timer B mode */
#define GPTIMER3_CTL                ( *(cc2538_reg_t*)0x4003300c ) /**< GPTM3 control */
#define GPTIMER3_SYNC               ( *(cc2538_reg_t*)0x40033010 ) /**< GPTM3 synchronize */
#define GPTIMER3_IMR                ( *(cc2538_reg_t*)0x40033018 ) /**< GPTM3 interrupt mask */
#define GPTIMER3_RIS                ( *(cc2538_reg_t*)0x4003301c ) /**< GPTM3 raw interrupt status */
#define GPTIMER3_MIS                ( *(cc2538_reg_t*)0x40033020 ) /**< GPTM3 masked interrupt status */
#define GPTIMER3_ICR                ( *(cc2538_reg_t*)0x40033024 ) /**< GPTM3 interrupt clear */
#define GPTIMER3_TAILR              ( *(cc2538_reg_t*)0x40033028 ) /**< GPTM3 Timer A interval load */
#define GPTIMER3_TBILR              ( *(cc2538_reg_t*)0x4003302c ) /**< GPTM3 Timer B interval load */
#define GPTIMER3_TAMATCHR           ( *(cc2538_reg_t*)0x40033030 ) /**< GPTM3 Timer A match */
#define GPTIMER3_TBMATCHR           ( *(cc2538_reg_t*)0x40033034 ) /**< GPTM3 Timer B match */
#define GPTIMER3_TAPR               ( *(cc2538_reg_t*)0x40033038 ) /**< GPTM3 Timer A prescale */
#define GPTIMER3_TBPR               ( *(cc2538_reg_t*)0x4003303c ) /**< GPTM3 Timer B prescale */
#define GPTIMER3_TAPMR              ( *(cc2538_reg_t*)0x40033040 ) /**< GPTM3 Timer A prescale match */
#define GPTIMER3_TBPMR              ( *(cc2538_reg_t*)0x40033044 ) /**< GPTM3 Timer B prescale match */
#define GPTIMER3_TAR                ( *(cc2538_reg_t*)0x40033048 ) /**< GPTM3 Timer A */
#define GPTIMER3_TBR                ( *(cc2538_reg_t*)0x4003304c ) /**< GPTM3 Timer B */
#define GPTIMER3_TAV                ( *(cc2538_reg_t*)0x40033050 ) /**< GPTM3 Timer A value */
#define GPTIMER3_TBV                ( *(cc2538_reg_t*)0x40033054 ) /**< GPTM3 Timer B value */
#define GPTIMER3_TAPS               ( *(cc2538_reg_t*)0x4003305c ) /**< GPTM3 Timer A prescale snapshot */
#define GPTIMER3_TBPS               ( *(cc2538_reg_t*)0x40033060 ) /**< GPTM3 Timer B prescale snapshot */
#define GPTIMER3_TAPV               ( *(cc2538_reg_t*)0x40033064 ) /**< GPTM3 Timer A prescale value */
#define GPTIMER3_TBPV               ( *(cc2538_reg_t*)0x40033068 ) /**< GPTM3 Timer B prescale value */
#define GPTIMER3_PP                 ( *(cc2538_reg_t*)0x40033fc0 ) /**< GPTM3 peripheral properties */
#define RFCORE_FFSM_SRCRESMASK0     ( *(cc2538_reg_t*)0x40088580 ) /**< RF Source address matching result */
#define RFCORE_FFSM_SRCRESMASK1     ( *(cc2538_reg_t*)0x40088584 ) /**< RF Source address matching result */
#define RFCORE_FFSM_SRCRESMASK2     ( *(cc2538_reg_t*)0x40088588 ) /**< RF Source address matching result */
#define RFCORE_FFSM_SRCRESINDEX     ( *(cc2538_reg_t*)0x4008858c ) /**< RF Source address matching result */
#define RFCORE_FFSM_SRCEXTPENDEN0   ( *(cc2538_reg_t*)0x40088590 ) /**< RF Source address matching control */
#define RFCORE_FFSM_SRCEXTPENDEN1   ( *(cc2538_reg_t*)0x40088594 ) /**< RF Source address matching control */
#define RFCORE_FFSM_SRCEXTPENDEN2   ( *(cc2538_reg_t*)0x40088598 ) /**< RF Source address matching control */
#define RFCORE_FFSM_SRCSHORTPENDEN0 ( *(cc2538_reg_t*)0x4008859c ) /**< RF Source address matching control */
#define RFCORE_FFSM_SRCSHORTPENDEN1 ( *(cc2538_reg_t*)0x400885a0 ) /**< RF Source address matching control */
#define RFCORE_FFSM_SRCSHORTPENDEN2 ( *(cc2538_reg_t*)0x400885a4 ) /**< RF Source address matching control */
#define RFCORE_FFSM_EXT_ADDR0       ( *(cc2538_reg_t*)0x400885a8 ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR1       ( *(cc2538_reg_t*)0x400885ac ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR2       ( *(cc2538_reg_t*)0x400885b0 ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR3       ( *(cc2538_reg_t*)0x400885b4 ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR4       ( *(cc2538_reg_t*)0x400885b8 ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR5       ( *(cc2538_reg_t*)0x400885bc ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR6       ( *(cc2538_reg_t*)0x400885c0 ) /**< RF Local address information */
#define RFCORE_FFSM_EXT_ADDR7       ( *(cc2538_reg_t*)0x400885c4 ) /**< RF Local address information */
#define RFCORE_FFSM_PAN_ID0         ( *(cc2538_reg_t*)0x400885c8 ) /**< RF Local address information */
#define RFCORE_FFSM_PAN_ID1         ( *(cc2538_reg_t*)0x400885cc ) /**< RF Local address information */
#define RFCORE_FFSM_SHORT_ADDR0     ( *(cc2538_reg_t*)0x400885d0 ) /**< RF Local address information */
#define RFCORE_FFSM_SHORT_ADDR1     ( *(cc2538_reg_t*)0x400885d4 ) /**< RF Local address information */
#define RFCORE_XREG_FRMFILT0        ( *(cc2538_reg_t*)0x40088600 ) /**< RF Frame Filter 0 */
#define RFCORE_XREG_FRMFILT1        ( *(cc2538_reg_t*)0x40088604 ) /**< RF Frame Filter 1 */
#define RFCORE_XREG_SRCMATCH        ( *(cc2538_reg_t*)0x40088608 ) /**< RF Source address matching and pending bits */
#define RFCORE_XREG_SRCSHORTEN0     ( *(cc2538_reg_t*)0x4008860c ) /**< RF Short address matching */
#define RFCORE_XREG_SRCSHORTEN1     ( *(cc2538_reg_t*)0x40088610 ) /**< RF Short address matching */
#define RFCORE_XREG_SRCSHORTEN2     ( *(cc2538_reg_t*)0x40088614 ) /**< RF Short address matching */
#define RFCORE_XREG_SRCEXTEN0       ( *(cc2538_reg_t*)0x40088618 ) /**< RF Extended address matching */
#define RFCORE_XREG_SRCEXTEN1       ( *(cc2538_reg_t*)0x4008861c ) /**< RF Extended address matching */
#define RFCORE_XREG_SRCEXTEN2       ( *(cc2538_reg_t*)0x40088620 ) /**< RF Extended address matching */
#define RFCORE_XREG_FRMCTRL0        ( *(cc2538_reg_t*)0x40088624 ) /**< RF Frame handling */
#define RFCORE_XREG_FRMCTRL1        ( *(cc2538_reg_t*)0x40088628 ) /**< RF Frame handling */
#define RFCORE_XREG_RXENABLE        ( *(cc2538_reg_t*)0x4008862c ) /**< RF RX enabling */
#define RFCORE_XREG_RXMASKSET       ( *(cc2538_reg_t*)0x40088630 ) /**< RF RX enabling */
#define RFCORE_XREG_RXMASKCLR       ( *(cc2538_reg_t*)0x40088634 ) /**< RF RX disabling */
#define RFCORE_XREG_FREQTUNE        ( *(cc2538_reg_t*)0x40088638 ) /**< RF Crystal oscillator frequency tuning */
#define RFCORE_XREG_FREQCTRL        ( *(cc2538_reg_t*)0x4008863c ) /**< RF Controls the RF frequency */
#define RFCORE_XREG_TXPOWER         ( *(cc2538_reg_t*)0x40088640 ) /**< RF Controls the output power */
#define RFCORE_XREG_TXCTRL          ( *(cc2538_reg_t*)0x40088644 ) /**< RF Controls the TX settings */
#define RFCORE_XREG_FSMSTAT0        ( *(cc2538_reg_t*)0x40088648 ) /**< RF Radio status register */
#define RFCORE_XREG_FSMSTAT1        ( *(cc2538_reg_t*)0x4008864c ) /**< RF Radio status register */
#define RFCORE_XREG_FIFOPCTRL       ( *(cc2538_reg_t*)0x40088650 ) /**< RF FIFOP threshold */
#define RFCORE_XREG_FSMCTRL         ( *(cc2538_reg_t*)0x40088654 ) /**< RF FSM options */
#define RFCORE_XREG_CCACTRL0        ( *(cc2538_reg_t*)0x40088658 ) /**< RF CCA threshold */
#define RFCORE_XREG_CCACTRL1        ( *(cc2538_reg_t*)0x4008865c ) /**< RF Other CCA Options */
#define RFCORE_XREG_RSSI            ( *(cc2538_reg_t*)0x40088660 ) /**< RF RSSI status register */
#define RFCORE_XREG_RSSISTAT        ( *(cc2538_reg_t*)0x40088664 ) /**< RF RSSI valid status register */
#define RFCORE_XREG_RXFIRST         ( *(cc2538_reg_t*)0x40088668 ) /**< RF First byte in RX FIFO */
#define RFCORE_XREG_RXFIFOCNT       ( *(cc2538_reg_t*)0x4008866c ) /**< RF Number of bytes in RX FIFO */
#define RFCORE_XREG_TXFIFOCNT       ( *(cc2538_reg_t*)0x40088670 ) /**< RF Number of bytes in TX FIFO */
#define RFCORE_XREG_RXFIRST_PTR     ( *(cc2538_reg_t*)0x40088674 ) /**< RF RX FIFO pointer */
#define RFCORE_XREG_RXLAST_PTR      ( *(cc2538_reg_t*)0x40088678 ) /**< RF RX FIFO pointer */
#define RFCORE_XREG_RXP1_PTR        ( *(cc2538_reg_t*)0x4008867c ) /**< RF RX FIFO pointer */
#define RFCORE_XREG_TXFIRST_PTR     ( *(cc2538_reg_t*)0x40088684 ) /**< RF TX FIFO pointer */
#define RFCORE_XREG_TXLAST_PTR      ( *(cc2538_reg_t*)0x40088688 ) /**< RF TX FIFO pointer */
#define RFCORE_XREG_RFIRQM0         ( *(cc2538_reg_t*)0x4008868c ) /**< RF interrupt masks */
#define RFCORE_XREG_RFIRQM1         ( *(cc2538_reg_t*)0x40088690 ) /**< RF interrupt masks */
#define RFCORE_XREG_RFERRM          ( *(cc2538_reg_t*)0x40088694 ) /**< RF error interrupt mask */
#define RFCORE_XREG_RFRND           ( *(cc2538_reg_t*)0x4008869c ) /**< RF Random data */
#define RFCORE_XREG_MDMCTRL0        ( *(cc2538_reg_t*)0x400886a0 ) /**< RF Controls modem */
#define RFCORE_XREG_MDMCTRL1        ( *(cc2538_reg_t*)0x400886a4 ) /**< RF Controls modem */
#define RFCORE_XREG_FREQEST         ( *(cc2538_reg_t*)0x400886a8 ) /**< RF Estimated RF frequency offset */
#define RFCORE_XREG_RXCTRL          ( *(cc2538_reg_t*)0x400886ac ) /**< RF Tune receive section */
#define RFCORE_XREG_FSCTRL          ( *(cc2538_reg_t*)0x400886b0 ) /**< RF Tune frequency synthesizer */
#define RFCORE_XREG_FSCAL0          ( *(cc2538_reg_t*)0x400886b4 ) /**< RF Tune frequency calibration */
#define RFCORE_XREG_FSCAL1          ( *(cc2538_reg_t*)0x400886b8 ) /**< RF Tune frequency calibration */
#define RFCORE_XREG_FSCAL2          ( *(cc2538_reg_t*)0x400886bc ) /**< RF Tune frequency calibration */
#define RFCORE_XREG_FSCAL3          ( *(cc2538_reg_t*)0x400886c0 ) /**< RF Tune frequency calibration */
#define RFCORE_XREG_AGCCTRL0        ( *(cc2538_reg_t*)0x400886c4 ) /**< RF AGC dynamic range control */
#define RFCORE_XREG_AGCCTRL1        ( *(cc2538_reg_t*)0x400886c8 ) /**< RF AGC reference level */
#define RFCORE_XREG_AGCCTRL2        ( *(cc2538_reg_t*)0x400886cc ) /**< RF AGC gain override */
#define RFCORE_XREG_AGCCTRL3        ( *(cc2538_reg_t*)0x400886d0 ) /**< RF AGC control */
#define RFCORE_XREG_ADCTEST0        ( *(cc2538_reg_t*)0x400886d4 ) /**< RF ADC tuning */
#define RFCORE_XREG_ADCTEST1        ( *(cc2538_reg_t*)0x400886d8 ) /**< RF ADC tuning */
#define RFCORE_XREG_ADCTEST2        ( *(cc2538_reg_t*)0x400886dc ) /**< RF ADC tuning */
#define RFCORE_XREG_MDMTEST0        ( *(cc2538_reg_t*)0x400886e0 ) /**< RF Test register for modem */
#define RFCORE_XREG_MDMTEST1        ( *(cc2538_reg_t*)0x400886e4 ) /**< RF Test Register for Modem */
#define RFCORE_XREG_DACTEST0        ( *(cc2538_reg_t*)0x400886e8 ) /**< RF DAC override value */
#define RFCORE_XREG_DACTEST1        ( *(cc2538_reg_t*)0x400886ec ) /**< RF DAC override value */
#define RFCORE_XREG_DACTEST2        ( *(cc2538_reg_t*)0x400886f0 ) /**< RF DAC test setting */
#define RFCORE_XREG_ATEST           ( *(cc2538_reg_t*)0x400886f4 ) /**< RF Analog test control */
#define RFCORE_XREG_PTEST0          ( *(cc2538_reg_t*)0x400886f8 ) /**< RF Override power-down register */
#define RFCORE_XREG_PTEST1          ( *(cc2538_reg_t*)0x400886fc ) /**< RF Override power-down register */
#define RFCORE_XREG_CSPCTRL         ( *(cc2538_reg_t*)0x40088780 ) /**< RF CSP control bit */
#define RFCORE_XREG_CSPSTAT         ( *(cc2538_reg_t*)0x40088784 ) /**< RF CSP status register */
#define RFCORE_XREG_CSPX            ( *(cc2538_reg_t*)0x40088788 ) /**< RF CSP X data register */
#define RFCORE_XREG_CSPY            ( *(cc2538_reg_t*)0x4008878c ) /**< RF CSP Y data register */
#define RFCORE_XREG_CSPZ            ( *(cc2538_reg_t*)0x40088790 ) /**< RF CSP Z data register */
#define RFCORE_XREG_CSPT            ( *(cc2538_reg_t*)0x40088794 ) /**< RF CSP T data register */
#define RFCORE_XREG_RFC_OBS_CTRL0   ( *(cc2538_reg_t*)0x400887ac ) /**< RF observation mux control */
#define RFCORE_XREG_RFC_OBS_CTRL1   ( *(cc2538_reg_t*)0x400887b0 ) /**< RF observation mux control */
#define RFCORE_XREG_RFC_OBS_CTRL2   ( *(cc2538_reg_t*)0x400887b4 ) /**< RF observation mux control */
#define RFCORE_XREG_TXFILTCFG       ( *(cc2538_reg_t*)0x400887e8 ) /**< RF TX filter configuration */
#define RFCORE_SFR_MTCSPCFG         ( *(cc2538_reg_t*)0x40088800 ) /**< RF MAC Timer event configuration */
#define RFCORE_SFR_MTCTRL           ( *(cc2538_reg_t*)0x40088804 ) /**< RF MAC Timer control register */
#define RFCORE_SFR_MTIRQM           ( *(cc2538_reg_t*)0x40088808 ) /**< RF MAC Timer interrupt mask */
#define RFCORE_SFR_MTIRQF           ( *(cc2538_reg_t*)0x4008880c ) /**< RF MAC Timer interrupt flags */
#define RFCORE_SFR_MTMSEL           ( *(cc2538_reg_t*)0x40088810 ) /**< RF MAC Timer multiplex select */
#define RFCORE_SFR_MTM0             ( *(cc2538_reg_t*)0x40088814 ) /**< RF MAC Timer multiplexed register 0 */
#define RFCORE_SFR_MTM1             ( *(cc2538_reg_t*)0x40088818 ) /**< RF MAC Timer multiplexed register 1 */
#define RFCORE_SFR_MTMOVF2          ( *(cc2538_reg_t*)0x4008881c ) /**< RF MAC Timer multiplexed overflow register 2 */
#define RFCORE_SFR_MTMOVF1          ( *(cc2538_reg_t*)0x40088820 ) /**< RF MAC Timer multiplexed overflow register 1 */
#define RFCORE_SFR_MTMOVF0          ( *(cc2538_reg_t*)0x40088824 ) /**< RF MAC Timer multiplexed overflow register 0 */
#define RFCORE_SFR_RFDATA           ( *(cc2538_reg_t*)0x40088828 ) /**< RF Tx/Rx FIFO */
#define RFCORE_SFR_RFERRF           ( *(cc2538_reg_t*)0x4008882c ) /**< RF error interrupt flags */
#define RFCORE_SFR_RFIRQF1          ( *(cc2538_reg_t*)0x40088830 ) /**< RF interrupt flags */
#define RFCORE_SFR_RFIRQF0          ( *(cc2538_reg_t*)0x40088834 ) /**< RF interrupt flags */
#define RFCORE_SFR_RFST             ( *(cc2538_reg_t*)0x40088838 ) /**< RF CSMA-CA/strobe processor */
#define USB_ADDR                    ( *(cc2538_reg_t*)0x40089000 ) /**< USB Function address */
#define USB_POW                     ( *(cc2538_reg_t*)0x40089004 ) /**< USB Power management and control register */
#define USB_IIF                     ( *(cc2538_reg_t*)0x40089008 ) /**< USB Interrupt flags for endpoint 0 and IN endpoints 1-5 */
#define USB_OIF                     ( *(cc2538_reg_t*)0x40089010 ) /**< USB Interrupt flags for OUT endpoints 1-5 */
#define USB_CIF                     ( *(cc2538_reg_t*)0x40089018 ) /**< USB Common USB interrupt flags */
#define USB_IIE                     ( *(cc2538_reg_t*)0x4008901c ) /**< USB Interrupt enable mask for IN endpoints 1-5 and endpoint 0 */
#define USB_OIE                     ( *(cc2538_reg_t*)0x40089024 ) /**< USB Interrupt enable mask for OUT endpoints 1-5 */
#define USB_CIE                     ( *(cc2538_reg_t*)0x4008902c ) /**< USB Common USB interrupt enable mask */
#define USB_FRML                    ( *(cc2538_reg_t*)0x40089030 ) /**< USB Frame number (low byte) */
#define USB_FRMH                    ( *(cc2538_reg_t*)0x40089034 ) /**< USB Frame number (high byte) */
#define USB_INDEX                   ( *(cc2538_reg_t*)0x40089038 ) /**< USB Index register for selecting the endpoint status and control registers */
#define USB_CTRL                    ( *(cc2538_reg_t*)0x4008903c ) /**< USB USB peripheral control register */
#define USB_MAXI                    ( *(cc2538_reg_t*)0x40089040 ) /**< USB Indexed register: */
#define USB_CS0_CSIL                ( *(cc2538_reg_t*)0x40089044 ) /**< USB Indexed register: */
#define USB_CSIH                    ( *(cc2538_reg_t*)0x40089048 ) /**< USB Indexed register: */
#define USB_MAXO                    ( *(cc2538_reg_t*)0x4008904c ) /**< USB Indexed register: */
#define USB_CSOL                    ( *(cc2538_reg_t*)0x40089050 ) /**< USB Indexed register: */
#define USB_CSOH                    ( *(cc2538_reg_t*)0x40089054 ) /**< USB Indexed register: */
#define USB_CNT0_CNTL               ( *(cc2538_reg_t*)0x40089058 ) /**< USB Indexed register: */
#define USB_CNTH                    ( *(cc2538_reg_t*)0x4008905c ) /**< USB Indexed register: */
#define USB_F0                      ( *(cc2538_reg_t*)0x40089080 ) /**< USB Endpoint 0 FIFO */
#define USB_F1                      ( *(cc2538_reg_t*)0x40089088 ) /**< USB IN/OUT endpoint 1 FIFO */
#define USB_F2                      ( *(cc2538_reg_t*)0x40089090 ) /**< USB IN/OUT endpoint 2 FIFO */
#define USB_F3                      ( *(cc2538_reg_t*)0x40089098 ) /**< USB IN/OUT endpoint 3 FIFO */
#define USB_F4                      ( *(cc2538_reg_t*)0x400890a0 ) /**< USB IN/OUT endpoint 4 FIFO */
#define USB_F5                      ( *(cc2538_reg_t*)0x400890a8 ) /**< USB IN/OUT endpoint 5 FIFO */
#define AES_DMAC_CH0_CTRL           ( *(cc2538_reg_t*)0x4008b000 ) /**< AES Channel control */
#define AES_DMAC_CH0_EXTADDR        ( *(cc2538_reg_t*)0x4008b004 ) /**< AES Channel external address */
#define AES_DMAC_CH0_DMALENGTH      ( *(cc2538_reg_t*)0x4008b00c ) /**< AES Channel DMA length */
#define AES_DMAC_STATUS             ( *(cc2538_reg_t*)0x4008b018 ) /**< AES DMAC status */
#define AES_DMAC_SWRES              ( *(cc2538_reg_t*)0x4008b01c ) /**< AES DMAC software reset register */
#define AES_DMAC_CH1_CTRL           ( *(cc2538_reg_t*)0x4008b020 ) /**< AES Channel control */
#define AES_DMAC_CH1_EXTADDR        ( *(cc2538_reg_t*)0x4008b024 ) /**< AES Channel external address */
#define AES_DMAC_CH1_DMALENGTH      ( *(cc2538_reg_t*)0x4008b02c ) /**< AES Channel DMA length */
#define AES_DMAC_MST_RUNPARAMS      ( *(cc2538_reg_t*)0x4008b078 ) /**< AES DMAC master run-time parameters */
#define AES_DMAC_PERSR              ( *(cc2538_reg_t*)0x4008b07c ) /**< AES DMAC port error raw status register */
#define AES_DMAC_OPTIONS            ( *(cc2538_reg_t*)0x4008b0f8 ) /**< AES DMAC options register */
#define AES_DMAC_VERSION            ( *(cc2538_reg_t*)0x4008b0fc ) /**< AES DMAC version register */
#define AES_KEY_STORE_WRITE_AREA    ( *(cc2538_reg_t*)0x4008b400 ) /**< AES Key store write area register */
#define AES_KEY_STORE_WRITTEN_AREA  ( *(cc2538_reg_t*)0x4008b404 ) /**< AES Key store written area register */
#define AES_KEY_STORE_SIZE          ( *(cc2538_reg_t*)0x4008b408 ) /**< AES Key store size register */
#define AES_KEY_STORE_READ_AREA     ( *(cc2538_reg_t*)0x4008b40c ) /**< AES Key store read area register */
#define AES_AES_KEY2_0              ( *(cc2538_reg_t*)0x4008b500 ) /**< AES_KEY2_0 / AES_GHASH_H_IN_0 */
#define AES_AES_KEY2_1              ( *(cc2538_reg_t*)0x4008b504 ) /**< AES_KEY2_1 / AES_GHASH_H_IN_1 */
#define AES_AES_KEY2_2              ( *(cc2538_reg_t*)0x4008b508 ) /**< AES_KEY2_2 / AES_GHASH_H_IN_2 */
#define AES_AES_KEY2_3              ( *(cc2538_reg_t*)0x4008b50c ) /**< AES_KEY2_3 / AES_GHASH_H_IN_3 */
#define AES_AES_KEY3_0              ( *(cc2538_reg_t*)0x4008b510 ) /**< AES_KEY3_0 / AES_KEY2_4 */
#define AES_AES_KEY3_1              ( *(cc2538_reg_t*)0x4008b514 ) /**< AES_KEY3_1 / AES_KEY2_5 */
#define AES_AES_KEY3_2              ( *(cc2538_reg_t*)0x4008b518 ) /**< AES_KEY3_2 / AES_KEY2_6 */
#define AES_AES_KEY3_3              ( *(cc2538_reg_t*)0x4008b51c ) /**< AES_KEY3_3 / AES_KEY2_7 */
#define AES_AES_IV_0                ( *(cc2538_reg_t*)0x4008b540 ) /**< AES initialization vector registers */
#define AES_AES_IV_1                ( *(cc2538_reg_t*)0x4008b544 ) /**< AES initialization vector registers */
#define AES_AES_IV_2                ( *(cc2538_reg_t*)0x4008b548 ) /**< AES initialization vector registers */
#define AES_AES_IV_3                ( *(cc2538_reg_t*)0x4008b54c ) /**< AES initialization vector registers */
#define AES_AES_CTRL                ( *(cc2538_reg_t*)0x4008b550 ) /**< AES input/output buffer control and mode register */
#define AES_AES_C_LENGTH_0          ( *(cc2538_reg_t*)0x4008b554 ) /**< AES crypto length registers (LSW) */
#define AES_AES_C_LENGTH_1          ( *(cc2538_reg_t*)0x4008b558 ) /**< AES crypto length registers (MSW) */
#define AES_AES_AUTH_LENGTH         ( *(cc2538_reg_t*)0x4008b55c ) /**< AES Authentication length register */
#define AES_AES_DATA_IN_OUT_0       ( *(cc2538_reg_t*)0x4008b560 ) /**< AES Data input/output registers */
#define AES_AES_DATA_IN_OUT_1       ( *(cc2538_reg_t*)0x4008b564 ) /**< AES Data Input/Output Registers */
#define AES_AES_DATA_IN_OUT_2       ( *(cc2538_reg_t*)0x4008b568 ) /**< AES Data Input/Output Registers */
#define AES_AES_DATA_IN_OUT_3       ( *(cc2538_reg_t*)0x4008b56c ) /**< AES Data Input/Output Registers */
#define AES_AES_TAG_OUT_0           ( *(cc2538_reg_t*)0x4008b570 ) /**< AES TAG register 0 */
#define AES_AES_TAG_OUT_1           ( *(cc2538_reg_t*)0x4008b574 ) /**< AES TAG register 1 */
#define AES_AES_TAG_OUT_2           ( *(cc2538_reg_t*)0x4008b578 ) /**< AES TAG register 2 */
#define AES_AES_TAG_OUT_3           ( *(cc2538_reg_t*)0x4008b57c ) /**< AES TAG register 3 */
#define AES_HASH_DATA_IN_0          ( *(cc2538_reg_t*)0x4008b600 ) /**< AES HASH data input register 0  */
#define AES_HASH_DATA_IN_1          ( *(cc2538_reg_t*)0x4008b604 ) /**< AES HASH data input register 1  */
#define AES_HASH_DATA_IN_2          ( *(cc2538_reg_t*)0x4008b608 ) /**< AES HASH data input register 2  */
#define AES_HASH_DATA_IN_3          ( *(cc2538_reg_t*)0x4008b60c ) /**< AES HASH data input register 3  */
#define AES_HASH_DATA_IN_4          ( *(cc2538_reg_t*)0x4008b610 ) /**< AES HASH data input register 4  */
#define AES_HASH_DATA_IN_5          ( *(cc2538_reg_t*)0x4008b614 ) /**< AES HASH data input register 5  */
#define AES_HASH_DATA_IN_6          ( *(cc2538_reg_t*)0x4008b618 ) /**< AES HASH data input register 6  */
#define AES_HASH_DATA_IN_7          ( *(cc2538_reg_t*)0x4008b61c ) /**< AES HASH data input register 7  */
#define AES_HASH_DATA_IN_8          ( *(cc2538_reg_t*)0x4008b620 ) /**< AES HASH data input register 8  */
#define AES_HASH_DATA_IN_9          ( *(cc2538_reg_t*)0x4008b624 ) /**< AES HASH data input register 9  */
#define AES_HASH_DATA_IN_10         ( *(cc2538_reg_t*)0x4008b628 ) /**< AES HASH data input register 10 */
#define AES_HASH_DATA_IN_11         ( *(cc2538_reg_t*)0x4008b62c ) /**< AES HASH data input register 11 */
#define AES_HASH_DATA_IN_12         ( *(cc2538_reg_t*)0x4008b630 ) /**< AES HASH data input register 12 */
#define AES_HASH_DATA_IN_13         ( *(cc2538_reg_t*)0x4008b634 ) /**< AES HASH data input register 13 */
#define AES_HASH_DATA_IN_14         ( *(cc2538_reg_t*)0x4008b638 ) /**< AES HASH data input register 14 */
#define AES_HASH_DATA_IN_15         ( *(cc2538_reg_t*)0x4008b63c ) /**< AES HASH data input register 15 */
#define AES_HASH_IO_BUF_CTRL        ( *(cc2538_reg_t*)0x4008b640 ) /**< AES Input/output buffer control and status register */
#define AES_HASH_MODE_IN            ( *(cc2538_reg_t*)0x4008b644 ) /**< AES Hash mode register */
#define AES_HASH_LENGTH_IN_L        ( *(cc2538_reg_t*)0x4008b648 ) /**< AES Hash length register */
#define AES_HASH_LENGTH_IN_H        ( *(cc2538_reg_t*)0x4008b64c ) /**< AES Hash length register */
#define AES_HASH_DIGEST_A           ( *(cc2538_reg_t*)0x4008b650 ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_B           ( *(cc2538_reg_t*)0x4008b654 ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_C           ( *(cc2538_reg_t*)0x4008b658 ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_D           ( *(cc2538_reg_t*)0x4008b65c ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_E           ( *(cc2538_reg_t*)0x4008b660 ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_F           ( *(cc2538_reg_t*)0x4008b664 ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_G           ( *(cc2538_reg_t*)0x4008b668 ) /**< AES Hash digest registers */
#define AES_HASH_DIGEST_H           ( *(cc2538_reg_t*)0x4008b66c ) /**< AES Hash digest registers */
#define AES_CTRL_ALG_SEL            ( *(cc2538_reg_t*)0x4008b700 ) /**< AES Algorithm select */
#define AES_CTRL_PROT_EN            ( *(cc2538_reg_t*)0x4008b704 ) /**< AES Master PROT privileged access enable */
#define AES_CTRL_SW_RESET           ( *(cc2538_reg_t*)0x4008b740 ) /**< AES Software reset */
#define AES_CTRL_INT_CFG            ( *(cc2538_reg_t*)0x4008b780 ) /**< AES Interrupt configuration */
#define AES_CTRL_INT_EN             ( *(cc2538_reg_t*)0x4008b784 ) /**< AES Interrupt enable */
#define AES_CTRL_INT_CLR            ( *(cc2538_reg_t*)0x4008b788 ) /**< AES Interrupt clear */
#define AES_CTRL_INT_SET            ( *(cc2538_reg_t*)0x4008b78c ) /**< AES Interrupt set */
#define AES_CTRL_INT_STAT           ( *(cc2538_reg_t*)0x4008b790 ) /**< AES Interrupt status */
#define AES_CTRL_OPTIONS            ( *(cc2538_reg_t*)0x4008b7f8 ) /**< AES Options register */
#define AES_CTRL_VERSION            ( *(cc2538_reg_t*)0x4008b7fc ) /**< AES Version register */
#define SYS_CTRL_CLOCK_CTRL         ( *(cc2538_reg_t*)0x400d2000 ) /**< Clock control register */
#define SYS_CTRL_CLOCK_STA          ( *(cc2538_reg_t*)0x400d2004 ) /**< Clock status register */
#define SYS_CTRL_RCGCGPT            ( *(cc2538_reg_t*)0x400d2008 ) /**< Module clocks for GPT[3:0] when the CPU is in active (run) mode */
#define SYS_CTRL_SCGCGPT            ( *(cc2538_reg_t*)0x400d200c ) /**< Module clocks for GPT[3:0] when the CPU is in sleep mode */
#define SYS_CTRL_DCGCGPT            ( *(cc2538_reg_t*)0x400d2010 ) /**< Module clocks for GPT[3:0] when the CPU is in PM0 */
#define SYS_CTRL_SRGPT              ( *(cc2538_reg_t*)0x400d2014 ) /**< Reset for GPT[3:0]. */
#define SYS_CTRL_RCGCSSI            ( *(cc2538_reg_t*)0x400d2018 ) /**< Module clocks for SSI[1:0] when the CPU is in active (run) mode */
#define SYS_CTRL_SCGCSSI            ( *(cc2538_reg_t*)0x400d201c ) /**< Module clocks for SSI[1:0] when the CPU is insSleep mode */
#define SYS_CTRL_DCGCSSI            ( *(cc2538_reg_t*)0x400d2020 ) /**< Module clocks for SSI[1:0] when the CPU is in PM0 */
#define SYS_CTRL_SRSSI              ( *(cc2538_reg_t*)0x400d2024 ) /**< Reset for SSI[1:0]. */
#define SYS_CTRL_RCGCUART           ( *(cc2538_reg_t*)0x400d2028 ) /**< Module clocks for UART[1:0] when the CPU is in active (run) mode */
#define SYS_CTRL_SCGCUART           ( *(cc2538_reg_t*)0x400d202c ) /**< Module clocks for UART[1:0] when the CPU is in sleep mode */
#define SYS_CTRL_DCGCUART           ( *(cc2538_reg_t*)0x400d2030 ) /**< Module clocks for UART[1:0] when the CPU is in PM0 */
#define SYS_CTRL_SRUART             ( *(cc2538_reg_t*)0x400d2034 ) /**< Reset for UART[1:0]. */
#define SYS_CTRL_RCGCI2C            ( *(cc2538_reg_t*)0x400d2038 ) /**< Module clocks for I2C when the CPU is in active (run) mode */
#define SYS_CTRL_SCGCI2C            ( *(cc2538_reg_t*)0x400d203c ) /**< Module clocks for I2C when the CPU is in sleep mode */
#define SYS_CTRL_DCGCI2C            ( *(cc2538_reg_t*)0x400d2040 ) /**< Module clocks for I2C when the CPU is in PM0 */
#define SYS_CTRL_SRI2C              ( *(cc2538_reg_t*)0x400d2044 ) /**< Reset for I2C. */
#define SYS_CTRL_RCGCSEC            ( *(cc2538_reg_t*)0x400d2048 ) /**< Module clocks for the security module when the CPU is in active (run) mode */
#define SYS_CTRL_SCGCSEC            ( *(cc2538_reg_t*)0x400d204c ) /**< Module clocks for the security module when the CPU is in sleep mode */
#define SYS_CTRL_DCGCSEC            ( *(cc2538_reg_t*)0x400d2050 ) /**< Module clocks for the security module when the CPU is in PM0 */
#define SYS_CTRL_SRSEC              ( *(cc2538_reg_t*)0x400d2054 ) /**< Reset for the security module. */
#define SYS_CTRL_PMCTL              ( *(cc2538_reg_t*)0x400d2058 ) /**< Power mode. */
#define SYS_CTRL_SRCRC              ( *(cc2538_reg_t*)0x400d205c ) /**< CRC on state retention. */
#define SYS_CTRL_PWRDBG             ( *(cc2538_reg_t*)0x400d2074 ) /**< Power debug register */
#define SYS_CTRL_CLD                ( *(cc2538_reg_t*)0x400d2080 ) /**< This register controls the clock loss detection feature. */
#define SYS_CTRL_IWE                ( *(cc2538_reg_t*)0x400d2094 ) /**< This register controls interrupt wake-up. */
#define SYS_CTRL_I_MAP              ( *(cc2538_reg_t*)0x400d2098 ) /**< This register selects which interrupt map to be used. */
#define SYS_CTRL_RCGCRFC            ( *(cc2538_reg_t*)0x400d20a8 ) /**< This register defines the module clocks for RF CORE when the CPU is in active (run) mode */
#define SYS_CTRL_SCGCRFC            ( *(cc2538_reg_t*)0x400d20ac ) /**< This register defines the module clocks for RF CORE when the CPU is in sleep mode */
#define SYS_CTRL_DCGCRFC            ( *(cc2538_reg_t*)0x400d20b0 ) /**< This register defines the module clocks for RF CORE when the CPU is in PM0 */
#define SYS_CTRL_EMUOVR             ( *(cc2538_reg_t*)0x400d20b4 ) /**< This register defines the emulator override controls for power mode and peripheral clock gate. */
#define FLASH_CTRL_FCTL             ( *(cc2538_reg_t*)0x400d3008 ) /**< Flash control */
#define FLASH_CTRL_FADDR            ( *(cc2538_reg_t*)0x400d300c ) /**< Flash address */
#define FLASH_CTRL_FWDATA           ( *(cc2538_reg_t*)0x400d3010 ) /**< Flash data */
#define FLASH_CTRL_DIECFG0          ( *(cc2538_reg_t*)0x400d3014 ) /**< Flash Die Configuration 0 */
#define FLASH_CTRL_DIECFG1          ( *(cc2538_reg_t*)0x400d3018 ) /**< Flash Die Configuration 1 */
#define FLASH_CTRL_DIECFG2          ( *(cc2538_reg_t*)0x400d301c ) /**< Flash Die Configuration 2 */
#define IOC_PA0_SEL                 ( *(cc2538_reg_t*)0x400d4000 ) /**< Peripheral select control for PA0 */
#define IOC_PA1_SEL                 ( *(cc2538_reg_t*)0x400d4004 ) /**< Peripheral select control for PA1 */
#define IOC_PA2_SEL                 ( *(cc2538_reg_t*)0x400d4008 ) /**< Peripheral select control for PA2 */
#define IOC_PA3_SEL                 ( *(cc2538_reg_t*)0x400d400c ) /**< Peripheral select control for PA3 */
#define IOC_PA4_SEL                 ( *(cc2538_reg_t*)0x400d4010 ) /**< Peripheral select control for PA4 */
#define IOC_PA5_SEL                 ( *(cc2538_reg_t*)0x400d4014 ) /**< Peripheral select control for PA5 */
#define IOC_PA6_SEL                 ( *(cc2538_reg_t*)0x400d4018 ) /**< Peripheral select control for PA6 */
#define IOC_PA7_SEL                 ( *(cc2538_reg_t*)0x400d401c ) /**< Peripheral select control for PA7 */
#define IOC_PB0_SEL                 ( *(cc2538_reg_t*)0x400d4020 ) /**< Peripheral select control for PB0 */
#define IOC_PB1_SEL                 ( *(cc2538_reg_t*)0x400d4024 ) /**< Peripheral select control for PB1 */
#define IOC_PB2_SEL                 ( *(cc2538_reg_t*)0x400d4028 ) /**< Peripheral select control for PB2 */
#define IOC_PB3_SEL                 ( *(cc2538_reg_t*)0x400d402c ) /**< Peripheral select control for PB3 */
#define IOC_PB4_SEL                 ( *(cc2538_reg_t*)0x400d4030 ) /**< Peripheral select control for PB4 */
#define IOC_PB5_SEL                 ( *(cc2538_reg_t*)0x400d4034 ) /**< Peripheral select control for PB5 */
#define IOC_PB6_SEL                 ( *(cc2538_reg_t*)0x400d4038 ) /**< Peripheral select control for PB6 */
#define IOC_PB7_SEL                 ( *(cc2538_reg_t*)0x400d403c ) /**< Peripheral select control for PB7 */
#define IOC_PC0_SEL                 ( *(cc2538_reg_t*)0x400d4040 ) /**< Peripheral select control for PC0 */
#define IOC_PC1_SEL                 ( *(cc2538_reg_t*)0x400d4044 ) /**< Peripheral select control for PC1 */
#define IOC_PC2_SEL                 ( *(cc2538_reg_t*)0x400d4048 ) /**< Peripheral select control for PC2 */
#define IOC_PC3_SEL                 ( *(cc2538_reg_t*)0x400d404c ) /**< Peripheral select control for PC3 */
#define IOC_PC4_SEL                 ( *(cc2538_reg_t*)0x400d4050 ) /**< Peripheral select control for PC4 */
#define IOC_PC5_SEL                 ( *(cc2538_reg_t*)0x400d4054 ) /**< Peripheral select control for PC5 */
#define IOC_PC6_SEL                 ( *(cc2538_reg_t*)0x400d4058 ) /**< Peripheral select control for PC6 */
#define IOC_PC7_SEL                 ( *(cc2538_reg_t*)0x400d405c ) /**< Peripheral select control for PC7 */
#define IOC_PD0_SEL                 ( *(cc2538_reg_t*)0x400d4060 ) /**< Peripheral select control for PD0 */
#define IOC_PD1_SEL                 ( *(cc2538_reg_t*)0x400d4064 ) /**< Peripheral select control for PD1 */
#define IOC_PD2_SEL                 ( *(cc2538_reg_t*)0x400d4068 ) /**< Peripheral select control for PD2 */
#define IOC_PD3_SEL                 ( *(cc2538_reg_t*)0x400d406c ) /**< Peripheral select control for PD3 */
#define IOC_PD4_SEL                 ( *(cc2538_reg_t*)0x400d4070 ) /**< Peripheral select control for PD4 */
#define IOC_PD5_SEL                 ( *(cc2538_reg_t*)0x400d4074 ) /**< Peripheral select control for PD5 */
#define IOC_PD6_SEL                 ( *(cc2538_reg_t*)0x400d4078 ) /**< Peripheral select control for PD6 */
#define IOC_PD7_SEL                 ( *(cc2538_reg_t*)0x400d407c ) /**< Peripheral select control for PD7 */
#define IOC_PA0_OVER                ( *(cc2538_reg_t*)0x400d4080 ) /**< Override configuration register for PA0. */
#define IOC_PA1_OVER                ( *(cc2538_reg_t*)0x400d4084 ) /**< Override configuration register for PA1. */
#define IOC_PA2_OVER                ( *(cc2538_reg_t*)0x400d4088 ) /**< Override configuration register for PA2. */
#define IOC_PA3_OVER                ( *(cc2538_reg_t*)0x400d408c ) /**< Override configuration register for PA3. */
#define IOC_PA4_OVER                ( *(cc2538_reg_t*)0x400d4090 ) /**< Override configuration register for PA4. */
#define IOC_PA5_OVER                ( *(cc2538_reg_t*)0x400d4094 ) /**< Override configuration register for PA5. */
#define IOC_PA6_OVER                ( *(cc2538_reg_t*)0x400d4098 ) /**< Override configuration register for PA6. */
#define IOC_PA7_OVER                ( *(cc2538_reg_t*)0x400d409c ) /**< Override configuration register for PA7. */
#define IOC_PB0_OVER                ( *(cc2538_reg_t*)0x400d40a0 ) /**< Override configuration register for PB0. */
#define IOC_PB1_OVER                ( *(cc2538_reg_t*)0x400d40a4 ) /**< Override configuration register for PB1. */
#define IOC_PB2_OVER                ( *(cc2538_reg_t*)0x400d40a8 ) /**< Override configuration register for PB2. */
#define IOC_PB3_OVER                ( *(cc2538_reg_t*)0x400d40ac ) /**< Override configuration register for PB3. */
#define IOC_PB4_OVER                ( *(cc2538_reg_t*)0x400d40b0 ) /**< Override configuration register for PB4. */
#define IOC_PB5_OVER                ( *(cc2538_reg_t*)0x400d40b4 ) /**< Override configuration register for PB5. */
#define IOC_PB6_OVER                ( *(cc2538_reg_t*)0x400d40b8 ) /**< Override configuration register for PB6. */
#define IOC_PB7_OVER                ( *(cc2538_reg_t*)0x400d40bc ) /**< Override configuration register for PB7. */
#define IOC_PC0_OVER                ( *(cc2538_reg_t*)0x400d40c0 ) /**< Override configuration register for PC0. PC0 has high drive capability. */
#define IOC_PC1_OVER                ( *(cc2538_reg_t*)0x400d40c4 ) /**< Override configuration register for PC1. PC1 has high drive capability. */
#define IOC_PC2_OVER                ( *(cc2538_reg_t*)0x400d40c8 ) /**< Override configuration register for PC2. PC2 has high drive capability. */
#define IOC_PC3_OVER                ( *(cc2538_reg_t*)0x400d40cc ) /**< Override configuration register for PC3. PC3 has high drive capability. */
#define IOC_PC4_OVER                ( *(cc2538_reg_t*)0x400d40d0 ) /**< Override configuration register for PC4. */
#define IOC_PC5_OVER                ( *(cc2538_reg_t*)0x400d40d4 ) /**< Override configuration register for PC5. */
#define IOC_PC6_OVER                ( *(cc2538_reg_t*)0x400d40d8 ) /**< Override configuration register for PC6. */
#define IOC_PC7_OVER                ( *(cc2538_reg_t*)0x400d40dc ) /**< Override configuration register for PC7. */
#define IOC_PD0_OVER                ( *(cc2538_reg_t*)0x400d40e0 ) /**< Override configuration register for PD0. */
#define IOC_PD1_OVER                ( *(cc2538_reg_t*)0x400d40e4 ) /**< Override configuration register for PD1. */
#define IOC_PD2_OVER                ( *(cc2538_reg_t*)0x400d40e8 ) /**< Override configuration register for PD2. */
#define IOC_PD3_OVER                ( *(cc2538_reg_t*)0x400d40ec ) /**< Override configuration register for PD3. */
#define IOC_PD4_OVER                ( *(cc2538_reg_t*)0x400d40f0 ) /**< Override configuration register for PD4. */
#define IOC_PD5_OVER                ( *(cc2538_reg_t*)0x400d40f4 ) /**< Override configuration register for PD5. */
#define IOC_PD6_OVER                ( *(cc2538_reg_t*)0x400d40f8 ) /**< Override configuration register for PD6. */
#define IOC_PD7_OVER                ( *(cc2538_reg_t*)0x400d40fc ) /**< Override configuration register for PD7. */
#define IOC_UARTRXD_UART0           ( *(cc2538_reg_t*)0x400d4100 ) /**< Pin selection for UART0 RX. */
#define IOC_UARTCTS_UART1           ( *(cc2538_reg_t*)0x400d4104 ) /**< Pin selection for UART1 CTS. */
#define IOC_UARTRXD_UART1           ( *(cc2538_reg_t*)0x400d4108 ) /**< Pin selection for UART1 RX. */
#define IOC_CLK_SSI_SSI0            ( *(cc2538_reg_t*)0x400d410c ) /**< Pin selection for SSI0 CLK. */
#define IOC_SSIRXD_SSI0             ( *(cc2538_reg_t*)0x400d4110 ) /**< Pin selection for SSI0 RX. */
#define IOC_SSIFSSIN_SSI0           ( *(cc2538_reg_t*)0x400d4114 ) /**< Pin selection for SSI0 FSSIN. */
#define IOC_CLK_SSIIN_SSI0          ( *(cc2538_reg_t*)0x400d4118 ) /**< Pin selection for SSI0 CLK_SSIN. */
#define IOC_CLK_SSI_SSI1            ( *(cc2538_reg_t*)0x400d411c ) /**< Pin selection for SSI1 CLK. */
#define IOC_SSIRXD_SSI1             ( *(cc2538_reg_t*)0x400d4120 ) /**< Pin selection for SSI1 RX. */
#define IOC_SSIFSSIN_SSI1           ( *(cc2538_reg_t*)0x400d4124 ) /**< Pin selection for SSI1 FSSIN. */
#define IOC_CLK_SSIIN_SSI1          ( *(cc2538_reg_t*)0x400d4128 ) /**< Pin selection for SSI1 CLK_SSIN. */
#define IOC_I2CMSSDA                ( *(cc2538_reg_t*)0x400d412c ) /**< Pin selection for I2C SDA. */
#define IOC_I2CMSSCL                ( *(cc2538_reg_t*)0x400d4130 ) /**< Pin selection for I2C SCL. */
#define IOC_GPT0OCP1                ( *(cc2538_reg_t*)0x400d4134 ) /**< Pin selection for GPT0OCP1. */
#define IOC_GPT0OCP2                ( *(cc2538_reg_t*)0x400d4138 ) /**< Pin selection for GPT0OCP2. */
#define IOC_GPT1OCP1                ( *(cc2538_reg_t*)0x400d413c ) /**< Pin selection for GPT1OCP1. */
#define IOC_GPT1OCP2                ( *(cc2538_reg_t*)0x400d4140 ) /**< Pin selection for GPT1OCP2. */
#define IOC_GPT2OCP1                ( *(cc2538_reg_t*)0x400d4144 ) /**< Pin selection for GPT2OCP1. */
#define IOC_GPT2OCP2                ( *(cc2538_reg_t*)0x400d4148 ) /**< Pin selection for GPT2OCP2. */
#define IOC_GPT3OCP1                ( *(cc2538_reg_t*)0x400d414c ) /**< Pin selection for GPT3OCP1. */
#define IOC_GPT3OCP2                ( *(cc2538_reg_t*)0x400d4150 ) /**< Pin selection for GPT3OCP2. */
#define SMWDTHROSC_WDCTL            ( *(cc2538_reg_t*)0x400d5000 ) /**< Watchdog Timer Control */
#define SMWDTHROSC_ST0              ( *(cc2538_reg_t*)0x400d5040 ) /**< Sleep Timer 0 count and compare */
#define SMWDTHROSC_ST1              ( *(cc2538_reg_t*)0x400d5044 ) /**< Sleep Timer 1 count and compare */
#define SMWDTHROSC_ST2              ( *(cc2538_reg_t*)0x400d5048 ) /**< Sleep Timer 2 count and compare */
#define SMWDTHROSC_ST3              ( *(cc2538_reg_t*)0x400d504c ) /**< Sleep Timer 3 count and compare */
#define SMWDTHROSC_STLOAD           ( *(cc2538_reg_t*)0x400d5050 ) /**< Sleep Timer load status */
#define SMWDTHROSC_STCC             ( *(cc2538_reg_t*)0x400d5054 ) /**< Sleep Timer Capture control */
#define SMWDTHROSC_STCS             ( *(cc2538_reg_t*)0x400d5058 ) /**< Sleep Timer Capture status */
#define SMWDTHROSC_STCV0            ( *(cc2538_reg_t*)0x400d505c ) /**< Sleep Timer Capture value byte 0 */
#define SMWDTHROSC_STCV1            ( *(cc2538_reg_t*)0x400d5060 ) /**< Sleep Timer Capture value byte 1 */
#define SMWDTHROSC_STCV2            ( *(cc2538_reg_t*)0x400d5064 ) /**< Sleep Timer Capture value byte 2 */
#define SMWDTHROSC_STCV3            ( *(cc2538_reg_t*)0x400d5068 ) /**< Sleep Timer Capture value byte 3 */
#define ANA_REGS_IVCTRL             ( *(cc2538_reg_t*)0x400d6004 ) /**< Analog control register */
#define GPIO_A_DATA                 ( *(cc2538_reg_t*)0x400d9000 ) /**< GPIO_A Data Register */
#define GPIO_A_DIR                  ( *(cc2538_reg_t*)0x400d9400 ) /**< GPIO_A data direction register */
#define GPIO_A_IS                   ( *(cc2538_reg_t*)0x400d9404 ) /**< GPIO_A Interrupt Sense register */
#define GPIO_A_IBE                  ( *(cc2538_reg_t*)0x400d9408 ) /**< GPIO_A Interrupt Both-Edges register */
#define GPIO_A_IEV                  ( *(cc2538_reg_t*)0x400d940c ) /**< GPIO_A Interrupt Event Register */
#define GPIO_A_IE                   ( *(cc2538_reg_t*)0x400d9410 ) /**< GPIO_A Interrupt mask register */
#define GPIO_A_RIS                  ( *(cc2538_reg_t*)0x400d9414 ) /**< GPIO_A Raw Interrupt Status register */
#define GPIO_A_MIS                  ( *(cc2538_reg_t*)0x400d9418 ) /**< GPIO_A Masked Interrupt Status register */
#define GPIO_A_IC                   ( *(cc2538_reg_t*)0x400d941c ) /**< GPIO_A Interrupt Clear register */
#define GPIO_A_AFSEL                ( *(cc2538_reg_t*)0x400d9420 ) /**< GPIO_A Alternate Function / mode control select register */
#define GPIO_A_GPIOLOCK             ( *(cc2538_reg_t*)0x400d9520 ) /**< GPIO_A Lock register */
#define GPIO_A_GPIOCR               ( *(cc2538_reg_t*)0x400d9524 ) /**< GPIO_A Commit Register */
#define GPIO_A_PMUX                 ( *(cc2538_reg_t*)0x400d9700 ) /**< GPIO_A The PMUX register */
#define GPIO_A_P_EDGE_CTRL          ( *(cc2538_reg_t*)0x400d9704 ) /**< GPIO_A The Port Edge Control register */
#define GPIO_A_PI_IEN               ( *(cc2538_reg_t*)0x400d9710 ) /**< GPIO_A The Power-up Interrupt Enable register */
#define GPIO_A_IRQ_DETECT_ACK       ( *(cc2538_reg_t*)0x400d9718 ) /**< GPIO_A IRQ Detect ACK register */
#define GPIO_A_USB_IRQ_ACK          ( *(cc2538_reg_t*)0x400d971c ) /**< GPIO_A IRQ Detect ACK for USB */
#define GPIO_A_IRQ_DETECT_UNMASK    ( *(cc2538_reg_t*)0x400d9720 ) /**< GPIO_A IRQ Detect ACK for masked interrupts */
#define GPIO_B_DATA                 ( *(cc2538_reg_t*)0x400da000 ) /**< GPIO Data Register */
#define GPIO_B_DIR                  ( *(cc2538_reg_t*)0x400da400 ) /**< GPIO_B data direction register */
#define GPIO_B_IS                   ( *(cc2538_reg_t*)0x400da404 ) /**< GPIO_B Interrupt Sense register */
#define GPIO_B_IBE                  ( *(cc2538_reg_t*)0x400da408 ) /**< GPIO_B Interrupt Both-Edges register */
#define GPIO_B_IEV                  ( *(cc2538_reg_t*)0x400da40c ) /**< GPIO_B Interrupt Event Register */
#define GPIO_B_IE                   ( *(cc2538_reg_t*)0x400da410 ) /**< GPIO_B Interrupt mask register */
#define GPIO_B_RIS                  ( *(cc2538_reg_t*)0x400da414 ) /**< GPIO_B Raw Interrupt Status register */
#define GPIO_B_MIS                  ( *(cc2538_reg_t*)0x400da418 ) /**< GPIO_B Masked Interrupt Status register */
#define GPIO_B_IC                   ( *(cc2538_reg_t*)0x400da41c ) /**< GPIO_B Interrupt Clear register */
#define GPIO_B_AFSEL                ( *(cc2538_reg_t*)0x400da420 ) /**< GPIO_B Alternate Function / mode control select register */
#define GPIO_B_GPIOLOCK             ( *(cc2538_reg_t*)0x400da520 ) /**< GPIO_B Lock register */
#define GPIO_B_GPIOCR               ( *(cc2538_reg_t*)0x400da524 ) /**< GPIO_B Commit Register */
#define GPIO_B_PMUX                 ( *(cc2538_reg_t*)0x400da700 ) /**< GPIO_B The PMUX register */
#define GPIO_B_P_EDGE_CTRL          ( *(cc2538_reg_t*)0x400da704 ) /**< GPIO_B The Port Edge Control register */
#define GPIO_B_PI_IEN               ( *(cc2538_reg_t*)0x400da710 ) /**< GPIO_B The Power-up Interrupt Enable register */
#define GPIO_B_IRQ_DETECT_ACK       ( *(cc2538_reg_t*)0x400da718 ) /**< GPIO_B IRQ Detect ACK register */
#define GPIO_B_USB_IRQ_ACK          ( *(cc2538_reg_t*)0x400da71c ) /**< GPIO_B IRQ Detect ACK for USB */
#define GPIO_B_IRQ_DETECT_UNMASK    ( *(cc2538_reg_t*)0x400da720 ) /**< GPIO_B IRQ Detect ACK for masked interrupts */
#define GPIO_C_DATA                 ( *(cc2538_reg_t*)0x400db000 ) /**< GPIO_C Data Register */
#define GPIO_C_DIR                  ( *(cc2538_reg_t*)0x400db400 ) /**< GPIO_C data direction register */
#define GPIO_C_IS                   ( *(cc2538_reg_t*)0x400db404 ) /**< GPIO_C Interrupt Sense register */
#define GPIO_C_IBE                  ( *(cc2538_reg_t*)0x400db408 ) /**< GPIO_C Interrupt Both-Edges register */
#define GPIO_C_IEV                  ( *(cc2538_reg_t*)0x400db40c ) /**< GPIO_C Interrupt Event Register */
#define GPIO_C_IE                   ( *(cc2538_reg_t*)0x400db410 ) /**< GPIO_C Interrupt mask register */
#define GPIO_C_RIS                  ( *(cc2538_reg_t*)0x400db414 ) /**< GPIO_C Raw Interrupt Status register */
#define GPIO_C_MIS                  ( *(cc2538_reg_t*)0x400db418 ) /**< GPIO_C Masked Interrupt Status register */
#define GPIO_C_IC                   ( *(cc2538_reg_t*)0x400db41c ) /**< GPIO_C Interrupt Clear register */
#define GPIO_C_AFSEL                ( *(cc2538_reg_t*)0x400db420 ) /**< GPIO_C Alternate Function / mode control select register */
#define GPIO_C_GPIOLOCK             ( *(cc2538_reg_t*)0x400db520 ) /**< GPIO_C Lock register */
#define GPIO_C_GPIOCR               ( *(cc2538_reg_t*)0x400db524 ) /**< GPIO_C Commit Register */
#define GPIO_C_PMUX                 ( *(cc2538_reg_t*)0x400db700 ) /**< GPIO_C The PMUX register */
#define GPIO_C_P_EDGE_CTRL          ( *(cc2538_reg_t*)0x400db704 ) /**< GPIO_C The Port Edge Control register */
#define GPIO_C_PI_IEN               ( *(cc2538_reg_t*)0x400db710 ) /**< GPIO_C The Power-up Interrupt Enable register */
#define GPIO_C_IRQ_DETECT_ACK       ( *(cc2538_reg_t*)0x400db718 ) /**< GPIO_C IRQ Detect ACK register */
#define GPIO_C_USB_IRQ_ACK          ( *(cc2538_reg_t*)0x400db71c ) /**< GPIO_C IRQ Detect ACK for USB */
#define GPIO_C_IRQ_DETECT_UNMASK    ( *(cc2538_reg_t*)0x400db720 ) /**< GPIO_C IRQ Detect ACK for masked interrupts */
#define GPIO_D_DATA                 ( *(cc2538_reg_t*)0x400dc000 ) /**< GPIO_D Data Register */
#define GPIO_D_DIR                  ( *(cc2538_reg_t*)0x400dc400 ) /**< GPIO_D data direction register */
#define GPIO_D_IS                   ( *(cc2538_reg_t*)0x400dc404 ) /**< GPIO_D Interrupt Sense register */
#define GPIO_D_IBE                  ( *(cc2538_reg_t*)0x400dc408 ) /**< GPIO_D Interrupt Both-Edges register */
#define GPIO_D_IEV                  ( *(cc2538_reg_t*)0x400dc40c ) /**< GPIO_D Interrupt Event Register */
#define GPIO_D_IE                   ( *(cc2538_reg_t*)0x400dc410 ) /**< GPIO_D Interrupt mask register */
#define GPIO_D_RIS                  ( *(cc2538_reg_t*)0x400dc414 ) /**< GPIO_D Raw Interrupt Status register */
#define GPIO_D_MIS                  ( *(cc2538_reg_t*)0x400dc418 ) /**< GPIO_D Masked Interrupt Status register */
#define GPIO_D_IC                   ( *(cc2538_reg_t*)0x400dc41c ) /**< GPIO_D Interrupt Clear register */
#define GPIO_D_AFSEL                ( *(cc2538_reg_t*)0x400dc420 ) /**< GPIO_D Alternate Function / mode control select register */
#define GPIO_D_GPIOLOCK             ( *(cc2538_reg_t*)0x400dc520 ) /**< GPIO_D Lock register */
#define GPIO_D_GPIOCR               ( *(cc2538_reg_t*)0x400dc524 ) /**< GPIO_D Commit Register */
#define GPIO_D_PMUX                 ( *(cc2538_reg_t*)0x400dc700 ) /**< GPIO_D The PMUX register */
#define GPIO_D_P_EDGE_CTRL          ( *(cc2538_reg_t*)0x400dc704 ) /**< GPIO_D The Port Edge Control register */
#define GPIO_D_PI_IEN               ( *(cc2538_reg_t*)0x400dc710 ) /**< GPIO_D The Power-up Interrupt Enable register */
#define GPIO_D_IRQ_DETECT_ACK       ( *(cc2538_reg_t*)0x400dc718 ) /**< GPIO_D IRQ Detect ACK register */
#define GPIO_D_USB_IRQ_ACK          ( *(cc2538_reg_t*)0x400dc71c ) /**< GPIO_D IRQ Detect ACK for USB */
#define GPIO_D_IRQ_DETECT_UNMASK    ( *(cc2538_reg_t*)0x400dc720 ) /**< GPIO_D IRQ Detect ACK for masked interrupts */
#define UDMA_STAT                   ( *(cc2538_reg_t*)0x400ff000 ) /**< DMA status */
#define UDMA_CFG                    ( *(cc2538_reg_t*)0x400ff004 ) /**< DMA configuration */
#define UDMA_CTLBASE                ( *(cc2538_reg_t*)0x400ff008 ) /**< DMA channel control base pointer */
#define UDMA_ALTBASE                ( *(cc2538_reg_t*)0x400ff00c ) /**< DMA alternate channel control base pointer */
#define UDMA_WAITSTAT               ( *(cc2538_reg_t*)0x400ff010 ) /**< DMA channel wait-on-request status */
#define UDMA_SWREQ                  ( *(cc2538_reg_t*)0x400ff014 ) /**< DMA channel software request */
#define UDMA_USEBURSTSET            ( *(cc2538_reg_t*)0x400ff018 ) /**< DMA channel useburst set */
#define UDMA_USEBURSTCLR            ( *(cc2538_reg_t*)0x400ff01c ) /**< DMA channel useburst clear */
#define UDMA_REQMASKSET             ( *(cc2538_reg_t*)0x400ff020 ) /**< DMA channel request mask set */
#define UDMA_REQMASKCLR             ( *(cc2538_reg_t*)0x400ff024 ) /**< DMA channel request mask clear */
#define UDMA_ENASET                 ( *(cc2538_reg_t*)0x400ff028 ) /**< DMA channel enable set */
#define UDMA_ENACLR                 ( *(cc2538_reg_t*)0x400ff02c ) /**< DMA channel enable clear */
#define UDMA_ALTSET                 ( *(cc2538_reg_t*)0x400ff030 ) /**< DMA channel primary alternate set */
#define UDMA_ALTCLR                 ( *(cc2538_reg_t*)0x400ff034 ) /**< DMA channel primary alternate clear */
#define UDMA_PRIOSET                ( *(cc2538_reg_t*)0x400ff038 ) /**< DMA channel priority set */
#define UDMA_PRIOCLR                ( *(cc2538_reg_t*)0x400ff03c ) /**< DMA channel priority clear */
#define UDMA_ERRCLR                 ( *(cc2538_reg_t*)0x400ff04c ) /**< DMA bus error clear */
#define UDMA_CHASGN                 ( *(cc2538_reg_t*)0x400ff500 ) /**< DMA channel assignment */
#define UDMA_CHIS                   ( *(cc2538_reg_t*)0x400ff504 ) /**< DMA channel interrupt status */
#define UDMA_CHMAP0                 ( *(cc2538_reg_t*)0x400ff510 ) /**< DMA channel map select 0 */
#define UDMA_CHMAP1                 ( *(cc2538_reg_t*)0x400ff514 ) /**< DMA channel map select 1 */
#define UDMA_CHMAP2                 ( *(cc2538_reg_t*)0x400ff518 ) /**< DMA channel map select 2 */
#define UDMA_CHMAP3                 ( *(cc2538_reg_t*)0x400ff51c ) /**< DMA channel map select 3 */
#define PKA_APTR                    ( *(cc2538_reg_t*)0x44004000 ) /**< PKA vector A address */
#define PKA_BPTR                    ( *(cc2538_reg_t*)0x44004004 ) /**< PKA vector B address */
#define PKA_CPTR                    ( *(cc2538_reg_t*)0x44004008 ) /**< PKA vector C address */
#define PKA_DPTR                    ( *(cc2538_reg_t*)0x4400400c ) /**< PKA vector D address */
#define PKA_ALENGTH                 ( *(cc2538_reg_t*)0x44004010 ) /**< PKA vector A length */
#define PKA_BLENGTH                 ( *(cc2538_reg_t*)0x44004014 ) /**< PKA vector B length */
#define PKA_SHIFT                   ( *(cc2538_reg_t*)0x44004018 ) /**< PKA bit shift value */
#define PKA_FUNCTION                ( *(cc2538_reg_t*)0x4400401c ) /**< PKA function */
#define PKA_COMPARE                 ( *(cc2538_reg_t*)0x44004020 ) /**< PKA compare result */
#define PKA_MSW                     ( *(cc2538_reg_t*)0x44004024 ) /**< PKA most-significant-word of result vector */
#define PKA_DIVMSW                  ( *(cc2538_reg_t*)0x44004028 ) /**< PKA most-significant-word of divide remainder */
#define PKA_SEQ_CTRL                ( *(cc2538_reg_t*)0x440040c8 ) /**< PKA sequencer control and status register */
#define PKA_OPTIONS                 ( *(cc2538_reg_t*)0x440040f4 ) /**< PKA hardware options register */
#define PKA_SW_REV                  ( *(cc2538_reg_t*)0x440040f8 ) /**< PKA firmware revision and capabilities register */
#define PKA_REVISION                ( *(cc2538_reg_t*)0x440040fc ) /**< PKA hardware revision register */
#define CCTEST_IO                   ( *(cc2538_reg_t*)0x44010000 ) /**< Output strength control */
#define CCTEST_OBSSEL0              ( *(cc2538_reg_t*)0x44010014 ) /**< CCTEST Select output signal on observation output 0 */
#define CCTEST_OBSSEL1              ( *(cc2538_reg_t*)0x44010018 ) /**< CCTEST Select output signal on observation output 1 */
#define CCTEST_OBSSEL2              ( *(cc2538_reg_t*)0x4401001c ) /**< CCTEST Select output signal on observation output 2 */
#define CCTEST_OBSSEL3              ( *(cc2538_reg_t*)0x44010020 ) /**< CCTEST Select output signal on observation output 3 */
#define CCTEST_OBSSEL4              ( *(cc2538_reg_t*)0x44010024 ) /**< CCTEST Select output signal on observation output 4 */
#define CCTEST_OBSSEL5              ( *(cc2538_reg_t*)0x44010028 ) /**< CCTEST Select output signal on observation output 5 */
#define CCTEST_OBSSEL6              ( *(cc2538_reg_t*)0x4401002c ) /**< CCTEST Select output signal on observation output 6 */
#define CCTEST_OBSSEL7              ( *(cc2538_reg_t*)0x44010030 ) /**< CCTEST Select output signal on observation output 7 */
#define CCTEST_TR0                  ( *(cc2538_reg_t*)0x44010034 ) /**< CCTEST Test register 0 */
#define CCTEST_USBCTRL              ( *(cc2538_reg_t*)0x44010050 ) /**< CCTEST USB PHY stand-by control */
/** @} */

#define XOSC32M_FREQ                32000000U /**< 32 MHz external oscillator/clock frequency */
#define RCOSC16M_FREQ               16000000U /**< 16 MHz internal RC oscillator frequency */

#define XOSC32K_FREQ                   32768U /**< 32 KHz external oscillator/clock frequency */
#define RCOSC32K_FREQ                  32753U /**< 32 KHz internal RC oscillator frequency */

#define CC2538_VTOR_ALIGN                 512 /**< CC2538 Vector Table alignment */

#ifdef __cplusplus
}
#endif

#endif /* CC2538_H */
/** @} */
