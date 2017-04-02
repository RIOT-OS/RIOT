/**
  ******************************************************************************
  * @file    stm32f429xx.h
  * @author  MCD Application Team
  * @version V2.4.3
  * @date    22-January-2016
  * @brief   CMSIS STM32F429xx Device Peripheral Access Layer Header File.
  *
  *          This file contains:
  *           - Data structures and the address mapping for all peripherals
  *           - peripherals registers declarations and bits definition
  *           - Macros to access peripheral’s registers hardware
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS_Device
  * @{
  */

/** @addtogroup stm32f429xx
  * @{
  */

#ifndef __STM32F429xx_H
#define __STM32F429xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/** @addtogroup Configuration_section_for_CMSIS
  * @{
  */

/**
  * @brief Configuration of the Cortex-M4 Processor and Core Peripherals
  */
#define __CM4_REV                 0x0001U  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1U       /*!< STM32F4XX provides an MPU                     */
#define __NVIC_PRIO_BITS          4U       /*!< STM32F4XX uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0U       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1U       /*!< FPU present                                   */

/**
  * @}
  */

/** @addtogroup Peripheral_interrupt_number_definition
  * @{
  */

/**
 * @brief STM32F4XX Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum
{
/******  Cortex-M4 Processor Exceptions Numbers ****************************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M4 Memory Management Interrupt                           */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M4 Bus Fault Interrupt                                   */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M4 Usage Fault Interrupt                                 */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M4 SV Call Interrupt                                    */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M4 Debug Monitor Interrupt                              */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M4 Pend SV Interrupt                                    */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M4 System Tick Interrupt                                */
/******  STM32 specific Interrupt Numbers **********************************************************************/
  WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                                         */
  PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
  TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
  RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
  FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
  RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
  EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
  EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
  EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
  EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
  EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
  DMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
  DMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
  DMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
  DMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
  DMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
  DMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
  DMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
  ADC_IRQn                    = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
  CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
  CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
  CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
  CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
  EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
  TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
  I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
  I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
  I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
  I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
  SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
  SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
  USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
  USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
  USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
  EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
  RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
  TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
  TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
  TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
  TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
  DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  FMC_IRQn                    = 48,     /*!< FMC global Interrupt                                              */
  SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
  TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
  SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
  UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
  UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
  TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
  TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
  DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
  ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
  CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
  CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
  CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
  CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
  OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
  DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
  I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
  I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
  OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
  OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
  OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
  OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
  DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
  HASH_RNG_IRQn               = 80,     /*!< Hash and RNG global interrupt                                     */
  FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
  UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
  UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
  SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
  SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
  SPI6_IRQn                   = 86,     /*!< SPI6 global Interrupt                                             */
  SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
  LTDC_IRQn                   = 88,     /*!< LTDC global Interrupt                                              */
  LTDC_ER_IRQn                = 89,     /*!< LTDC Error global Interrupt                                        */
  DMA2D_IRQn                  = 90      /*!< DMA2D global Interrupt                                            */
} IRQn_Type;

/**
  * @}
  */

#include "core_cm4.h"             /* Cortex-M4 processor and core peripherals */

/** @addtogroup Peripheral_registers_structures
  * @{
  */

/**
  * @brief Analog to Digital Converter
  */

typedef struct
{
  __IO uint32_t SR;     /*!< ADC status register,                         Address offset: 0x00 */
  __IO uint32_t CR1;    /*!< ADC control register 1,                      Address offset: 0x04 */
  __IO uint32_t CR2;    /*!< ADC control register 2,                      Address offset: 0x08 */
  __IO uint32_t SMPR1;  /*!< ADC sample time register 1,                  Address offset: 0x0C */
  __IO uint32_t SMPR2;  /*!< ADC sample time register 2,                  Address offset: 0x10 */
  __IO uint32_t JOFR1;  /*!< ADC injected channel data offset register 1, Address offset: 0x14 */
  __IO uint32_t JOFR2;  /*!< ADC injected channel data offset register 2, Address offset: 0x18 */
  __IO uint32_t JOFR3;  /*!< ADC injected channel data offset register 3, Address offset: 0x1C */
  __IO uint32_t JOFR4;  /*!< ADC injected channel data offset register 4, Address offset: 0x20 */
  __IO uint32_t HTR;    /*!< ADC watchdog higher threshold register,      Address offset: 0x24 */
  __IO uint32_t LTR;    /*!< ADC watchdog lower threshold register,       Address offset: 0x28 */
  __IO uint32_t SQR1;   /*!< ADC regular sequence register 1,             Address offset: 0x2C */
  __IO uint32_t SQR2;   /*!< ADC regular sequence register 2,             Address offset: 0x30 */
  __IO uint32_t SQR3;   /*!< ADC regular sequence register 3,             Address offset: 0x34 */
  __IO uint32_t JSQR;   /*!< ADC injected sequence register,              Address offset: 0x38*/
  __IO uint32_t JDR1;   /*!< ADC injected data register 1,                Address offset: 0x3C */
  __IO uint32_t JDR2;   /*!< ADC injected data register 2,                Address offset: 0x40 */
  __IO uint32_t JDR3;   /*!< ADC injected data register 3,                Address offset: 0x44 */
  __IO uint32_t JDR4;   /*!< ADC injected data register 4,                Address offset: 0x48 */
  __IO uint32_t DR;     /*!< ADC regular data register,                   Address offset: 0x4C */
} ADC_TypeDef;

typedef struct
{
  __IO uint32_t CSR;    /*!< ADC Common status register,                  Address offset: ADC1 base address + 0x300 */
  __IO uint32_t CCR;    /*!< ADC common control register,                 Address offset: ADC1 base address + 0x304 */
  __IO uint32_t CDR;    /*!< ADC common regular data register for dual
                             AND triple modes,                            Address offset: ADC1 base address + 0x308 */
} ADC_Common_TypeDef;


/**
  * @brief Controller Area Network TxMailBox
  */

typedef struct
{
  __IO uint32_t TIR;  /*!< CAN TX mailbox identifier register */
  __IO uint32_t TDTR; /*!< CAN mailbox data length control and time stamp register */
  __IO uint32_t TDLR; /*!< CAN mailbox data low register */
  __IO uint32_t TDHR; /*!< CAN mailbox data high register */
} CAN_TxMailBox_TypeDef;

/**
  * @brief Controller Area Network FIFOMailBox
  */

typedef struct
{
  __IO uint32_t RIR;  /*!< CAN receive FIFO mailbox identifier register */
  __IO uint32_t RDTR; /*!< CAN receive FIFO mailbox data length control and time stamp register */
  __IO uint32_t RDLR; /*!< CAN receive FIFO mailbox data low register */
  __IO uint32_t RDHR; /*!< CAN receive FIFO mailbox data high register */
} CAN_FIFOMailBox_TypeDef;

/**
  * @brief Controller Area Network FilterRegister
  */

typedef struct
{
  __IO uint32_t FR1; /*!< CAN Filter bank register 1 */
  __IO uint32_t FR2; /*!< CAN Filter bank register 1 */
} CAN_FilterRegister_TypeDef;

/**
  * @brief Controller Area Network
  */

typedef struct
{
  __IO uint32_t              MCR;                 /*!< CAN master control register,         Address offset: 0x00          */
  __IO uint32_t              MSR;                 /*!< CAN master status register,          Address offset: 0x04          */
  __IO uint32_t              TSR;                 /*!< CAN transmit status register,        Address offset: 0x08          */
  __IO uint32_t              RF0R;                /*!< CAN receive FIFO 0 register,         Address offset: 0x0C          */
  __IO uint32_t              RF1R;                /*!< CAN receive FIFO 1 register,         Address offset: 0x10          */
  __IO uint32_t              IER;                 /*!< CAN interrupt enable register,       Address offset: 0x14          */
  __IO uint32_t              ESR;                 /*!< CAN error status register,           Address offset: 0x18          */
  __IO uint32_t              BTR;                 /*!< CAN bit timing register,             Address offset: 0x1C          */
  uint32_t                   RESERVED0[88];       /*!< Reserved, 0x020 - 0x17F                                            */
  CAN_TxMailBox_TypeDef      sTxMailBox[3];       /*!< CAN Tx MailBox,                      Address offset: 0x180 - 0x1AC */
  CAN_FIFOMailBox_TypeDef    sFIFOMailBox[2];     /*!< CAN FIFO MailBox,                    Address offset: 0x1B0 - 0x1CC */
  uint32_t                   RESERVED1[12];       /*!< Reserved, 0x1D0 - 0x1FF                                            */
  __IO uint32_t              FMR;                 /*!< CAN filter master register,          Address offset: 0x200         */
  __IO uint32_t              FM1R;                /*!< CAN filter mode register,            Address offset: 0x204         */
  uint32_t                   RESERVED2;           /*!< Reserved, 0x208                                                    */
  __IO uint32_t              FS1R;                /*!< CAN filter scale register,           Address offset: 0x20C         */
  uint32_t                   RESERVED3;           /*!< Reserved, 0x210                                                    */
  __IO uint32_t              FFA1R;               /*!< CAN filter FIFO assignment register, Address offset: 0x214         */
  uint32_t                   RESERVED4;           /*!< Reserved, 0x218                                                    */
  __IO uint32_t              FA1R;                /*!< CAN filter activation register,      Address offset: 0x21C         */
  uint32_t                   RESERVED5[8];        /*!< Reserved, 0x220-0x23F                                              */
  CAN_FilterRegister_TypeDef sFilterRegister[28]; /*!< CAN Filter Register,                 Address offset: 0x240-0x31C   */
} CAN_TypeDef;

/**
  * @brief CRC calculation unit
  */

typedef struct
{
  __IO uint32_t DR;         /*!< CRC Data register,             Address offset: 0x00 */
  __IO uint8_t  IDR;        /*!< CRC Independent data register, Address offset: 0x04 */
  uint8_t       RESERVED0;  /*!< Reserved, 0x05                                      */
  uint16_t      RESERVED1;  /*!< Reserved, 0x06                                      */
  __IO uint32_t CR;         /*!< CRC Control register,          Address offset: 0x08 */
} CRC_TypeDef;

/**
  * @brief Digital to Analog Converter
  */

typedef struct
{
  __IO uint32_t CR;       /*!< DAC control register,                                    Address offset: 0x00 */
  __IO uint32_t SWTRIGR;  /*!< DAC software trigger register,                           Address offset: 0x04 */
  __IO uint32_t DHR12R1;  /*!< DAC channel1 12-bit right-aligned data holding register, Address offset: 0x08 */
  __IO uint32_t DHR12L1;  /*!< DAC channel1 12-bit left aligned data holding register,  Address offset: 0x0C */
  __IO uint32_t DHR8R1;   /*!< DAC channel1 8-bit right aligned data holding register,  Address offset: 0x10 */
  __IO uint32_t DHR12R2;  /*!< DAC channel2 12-bit right aligned data holding register, Address offset: 0x14 */
  __IO uint32_t DHR12L2;  /*!< DAC channel2 12-bit left aligned data holding register,  Address offset: 0x18 */
  __IO uint32_t DHR8R2;   /*!< DAC channel2 8-bit right-aligned data holding register,  Address offset: 0x1C */
  __IO uint32_t DHR12RD;  /*!< Dual DAC 12-bit right-aligned data holding register,     Address offset: 0x20 */
  __IO uint32_t DHR12LD;  /*!< DUAL DAC 12-bit left aligned data holding register,      Address offset: 0x24 */
  __IO uint32_t DHR8RD;   /*!< DUAL DAC 8-bit right aligned data holding register,      Address offset: 0x28 */
  __IO uint32_t DOR1;     /*!< DAC channel1 data output register,                       Address offset: 0x2C */
  __IO uint32_t DOR2;     /*!< DAC channel2 data output register,                       Address offset: 0x30 */
  __IO uint32_t SR;       /*!< DAC status register,                                     Address offset: 0x34 */
} DAC_TypeDef;

/**
  * @brief Debug MCU
  */

typedef struct
{
  __IO uint32_t IDCODE;  /*!< MCU device ID code,               Address offset: 0x00 */
  __IO uint32_t CR;      /*!< Debug MCU configuration register, Address offset: 0x04 */
  __IO uint32_t APB1FZ;  /*!< Debug MCU APB1 freeze register,   Address offset: 0x08 */
  __IO uint32_t APB2FZ;  /*!< Debug MCU APB2 freeze register,   Address offset: 0x0C */
}DBGMCU_TypeDef;

/**
  * @brief DCMI
  */

typedef struct
{
  __IO uint32_t CR;       /*!< DCMI control register 1,                       Address offset: 0x00 */
  __IO uint32_t SR;       /*!< DCMI status register,                          Address offset: 0x04 */
  __IO uint32_t RISR;     /*!< DCMI raw interrupt status register,            Address offset: 0x08 */
  __IO uint32_t IER;      /*!< DCMI interrupt enable register,                Address offset: 0x0C */
  __IO uint32_t MISR;     /*!< DCMI masked interrupt status register,         Address offset: 0x10 */
  __IO uint32_t ICR;      /*!< DCMI interrupt clear register,                 Address offset: 0x14 */
  __IO uint32_t ESCR;     /*!< DCMI embedded synchronization code register,   Address offset: 0x18 */
  __IO uint32_t ESUR;     /*!< DCMI embedded synchronization unmask register, Address offset: 0x1C */
  __IO uint32_t CWSTRTR;  /*!< DCMI crop window start,                        Address offset: 0x20 */
  __IO uint32_t CWSIZER;  /*!< DCMI crop window size,                         Address offset: 0x24 */
  __IO uint32_t DR;       /*!< DCMI data register,                            Address offset: 0x28 */
} DCMI_TypeDef;

/**
  * @brief DMA Controller
  */

typedef struct
{
  __IO uint32_t CR;     /*!< DMA stream x configuration register      */
  __IO uint32_t NDTR;   /*!< DMA stream x number of data register     */
  __IO uint32_t PAR;    /*!< DMA stream x peripheral address register */
  __IO uint32_t M0AR;   /*!< DMA stream x memory 0 address register   */
  __IO uint32_t M1AR;   /*!< DMA stream x memory 1 address register   */
  __IO uint32_t FCR;    /*!< DMA stream x FIFO control register       */
} DMA_Stream_TypeDef;

typedef struct
{
  __IO uint32_t LISR;   /*!< DMA low interrupt status register,      Address offset: 0x00 */
  __IO uint32_t HISR;   /*!< DMA high interrupt status register,     Address offset: 0x04 */
  __IO uint32_t LIFCR;  /*!< DMA low interrupt flag clear register,  Address offset: 0x08 */
  __IO uint32_t HIFCR;  /*!< DMA high interrupt flag clear register, Address offset: 0x0C */
} DMA_TypeDef;

/**
  * @brief DMA2D Controller
  */

typedef struct
{
  __IO uint32_t CR;            /*!< DMA2D Control Register,                         Address offset: 0x00 */
  __IO uint32_t ISR;           /*!< DMA2D Interrupt Status Register,                Address offset: 0x04 */
  __IO uint32_t IFCR;          /*!< DMA2D Interrupt Flag Clear Register,            Address offset: 0x08 */
  __IO uint32_t FGMAR;         /*!< DMA2D Foreground Memory Address Register,       Address offset: 0x0C */
  __IO uint32_t FGOR;          /*!< DMA2D Foreground Offset Register,               Address offset: 0x10 */
  __IO uint32_t BGMAR;         /*!< DMA2D Background Memory Address Register,       Address offset: 0x14 */
  __IO uint32_t BGOR;          /*!< DMA2D Background Offset Register,               Address offset: 0x18 */
  __IO uint32_t FGPFCCR;       /*!< DMA2D Foreground PFC Control Register,          Address offset: 0x1C */
  __IO uint32_t FGCOLR;        /*!< DMA2D Foreground Color Register,                Address offset: 0x20 */
  __IO uint32_t BGPFCCR;       /*!< DMA2D Background PFC Control Register,          Address offset: 0x24 */
  __IO uint32_t BGCOLR;        /*!< DMA2D Background Color Register,                Address offset: 0x28 */
  __IO uint32_t FGCMAR;        /*!< DMA2D Foreground CLUT Memory Address Register,  Address offset: 0x2C */
  __IO uint32_t BGCMAR;        /*!< DMA2D Background CLUT Memory Address Register,  Address offset: 0x30 */
  __IO uint32_t OPFCCR;        /*!< DMA2D Output PFC Control Register,              Address offset: 0x34 */
  __IO uint32_t OCOLR;         /*!< DMA2D Output Color Register,                    Address offset: 0x38 */
  __IO uint32_t OMAR;          /*!< DMA2D Output Memory Address Register,           Address offset: 0x3C */
  __IO uint32_t OOR;           /*!< DMA2D Output Offset Register,                   Address offset: 0x40 */
  __IO uint32_t NLR;           /*!< DMA2D Number of Line Register,                  Address offset: 0x44 */
  __IO uint32_t LWR;           /*!< DMA2D Line Watermark Register,                  Address offset: 0x48 */
  __IO uint32_t AMTCR;         /*!< DMA2D AHB Master Timer Configuration Register,  Address offset: 0x4C */
  uint32_t      RESERVED[236]; /*!< Reserved, 0x50-0x3FF */
  __IO uint32_t FGCLUT[256];   /*!< DMA2D Foreground CLUT,                          Address offset:400-7FF */
  __IO uint32_t BGCLUT[256];   /*!< DMA2D Background CLUT,                          Address offset:800-BFF */
} DMA2D_TypeDef;

/**
  * @brief Ethernet MAC
  */

typedef struct
{
  __IO uint32_t MACCR;
  __IO uint32_t MACFFR;
  __IO uint32_t MACHTHR;
  __IO uint32_t MACHTLR;
  __IO uint32_t MACMIIAR;
  __IO uint32_t MACMIIDR;
  __IO uint32_t MACFCR;
  __IO uint32_t MACVLANTR;             /*    8 */
  uint32_t      RESERVED0[2];
  __IO uint32_t MACRWUFFR;             /*   11 */
  __IO uint32_t MACPMTCSR;
  uint32_t      RESERVED1[2];
  __IO uint32_t MACSR;                 /*   15 */
  __IO uint32_t MACIMR;
  __IO uint32_t MACA0HR;
  __IO uint32_t MACA0LR;
  __IO uint32_t MACA1HR;
  __IO uint32_t MACA1LR;
  __IO uint32_t MACA2HR;
  __IO uint32_t MACA2LR;
  __IO uint32_t MACA3HR;
  __IO uint32_t MACA3LR;               /*   24 */
  uint32_t      RESERVED2[40];
  __IO uint32_t MMCCR;                 /*   65 */
  __IO uint32_t MMCRIR;
  __IO uint32_t MMCTIR;
  __IO uint32_t MMCRIMR;
  __IO uint32_t MMCTIMR;               /*   69 */
  uint32_t      RESERVED3[14];
  __IO uint32_t MMCTGFSCCR;            /*   84 */
  __IO uint32_t MMCTGFMSCCR;
  uint32_t      RESERVED4[5];
  __IO uint32_t MMCTGFCR;
  uint32_t      RESERVED5[10];
  __IO uint32_t MMCRFCECR;
  __IO uint32_t MMCRFAECR;
  uint32_t      RESERVED6[10];
  __IO uint32_t MMCRGUFCR;
  uint32_t      RESERVED7[334];
  __IO uint32_t PTPTSCR;
  __IO uint32_t PTPSSIR;
  __IO uint32_t PTPTSHR;
  __IO uint32_t PTPTSLR;
  __IO uint32_t PTPTSHUR;
  __IO uint32_t PTPTSLUR;
  __IO uint32_t PTPTSAR;
  __IO uint32_t PTPTTHR;
  __IO uint32_t PTPTTLR;
  __IO uint32_t RESERVED8;
  __IO uint32_t PTPTSSR;
  uint32_t      RESERVED9[565];
  __IO uint32_t DMABMR;
  __IO uint32_t DMATPDR;
  __IO uint32_t DMARPDR;
  __IO uint32_t DMARDLAR;
  __IO uint32_t DMATDLAR;
  __IO uint32_t DMASR;
  __IO uint32_t DMAOMR;
  __IO uint32_t DMAIER;
  __IO uint32_t DMAMFBOCR;
  __IO uint32_t DMARSWTR;
  uint32_t      RESERVED10[8];
  __IO uint32_t DMACHTDR;
  __IO uint32_t DMACHRDR;
  __IO uint32_t DMACHTBAR;
  __IO uint32_t DMACHRBAR;
} ETH_TypeDef;

/**
  * @brief External Interrupt/Event Controller
  */

typedef struct
{
  __IO uint32_t IMR;    /*!< EXTI Interrupt mask register,            Address offset: 0x00 */
  __IO uint32_t EMR;    /*!< EXTI Event mask register,                Address offset: 0x04 */
  __IO uint32_t RTSR;   /*!< EXTI Rising trigger selection register,  Address offset: 0x08 */
  __IO uint32_t FTSR;   /*!< EXTI Falling trigger selection register, Address offset: 0x0C */
  __IO uint32_t SWIER;  /*!< EXTI Software interrupt event register,  Address offset: 0x10 */
  __IO uint32_t PR;     /*!< EXTI Pending register,                   Address offset: 0x14 */
} EXTI_TypeDef;

/**
  * @brief FLASH Registers
  */

typedef struct
{
  __IO uint32_t ACR;      /*!< FLASH access control register,   Address offset: 0x00 */
  __IO uint32_t KEYR;     /*!< FLASH key register,              Address offset: 0x04 */
  __IO uint32_t OPTKEYR;  /*!< FLASH option key register,       Address offset: 0x08 */
  __IO uint32_t SR;       /*!< FLASH status register,           Address offset: 0x0C */
  __IO uint32_t CR;       /*!< FLASH control register,          Address offset: 0x10 */
  __IO uint32_t OPTCR;    /*!< FLASH option control register ,  Address offset: 0x14 */
  __IO uint32_t OPTCR1;   /*!< FLASH option control register 1, Address offset: 0x18 */
} FLASH_TypeDef;

/**
  * @brief Flexible Memory Controller
  */

typedef struct
{
  __IO uint32_t BTCR[8];    /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */
} FMC_Bank1_TypeDef;

/**
  * @brief Flexible Memory Controller Bank1E
  */

typedef struct
{
  __IO uint32_t BWTR[7];    /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
} FMC_Bank1E_TypeDef;

/**
  * @brief Flexible Memory Controller Bank2
  */

typedef struct
{
  __IO uint32_t PCR2;       /*!< NAND Flash control register 2,                       Address offset: 0x60 */
  __IO uint32_t SR2;        /*!< NAND Flash FIFO status and interrupt register 2,     Address offset: 0x64 */
  __IO uint32_t PMEM2;      /*!< NAND Flash Common memory space timing register 2,    Address offset: 0x68 */
  __IO uint32_t PATT2;      /*!< NAND Flash Attribute memory space timing register 2, Address offset: 0x6C */
  uint32_t      RESERVED0;  /*!< Reserved, 0x70                                                            */
  __IO uint32_t ECCR2;      /*!< NAND Flash ECC result registers 2,                   Address offset: 0x74 */
  uint32_t      RESERVED1;  /*!< Reserved, 0x78                                                            */
  uint32_t      RESERVED2;  /*!< Reserved, 0x7C                                                            */
  __IO uint32_t PCR3;       /*!< NAND Flash control register 3,                       Address offset: 0x80 */
  __IO uint32_t SR3;        /*!< NAND Flash FIFO status and interrupt register 3,     Address offset: 0x84 */
  __IO uint32_t PMEM3;      /*!< NAND Flash Common memory space timing register 3,    Address offset: 0x88 */
  __IO uint32_t PATT3;      /*!< NAND Flash Attribute memory space timing register 3, Address offset: 0x8C */
  uint32_t      RESERVED3;  /*!< Reserved, 0x90                                                            */
  __IO uint32_t ECCR3;      /*!< NAND Flash ECC result registers 3,                   Address offset: 0x94 */
} FMC_Bank2_3_TypeDef;

/**
  * @brief Flexible Memory Controller Bank4
  */

typedef struct
{
  __IO uint32_t PCR4;       /*!< PC Card  control register 4,                       Address offset: 0xA0 */
  __IO uint32_t SR4;        /*!< PC Card  FIFO status and interrupt register 4,     Address offset: 0xA4 */
  __IO uint32_t PMEM4;      /*!< PC Card  Common memory space timing register 4,    Address offset: 0xA8 */
  __IO uint32_t PATT4;      /*!< PC Card  Attribute memory space timing register 4, Address offset: 0xAC */
  __IO uint32_t PIO4;       /*!< PC Card  I/O space timing register 4,              Address offset: 0xB0 */
} FMC_Bank4_TypeDef;

/**
  * @brief Flexible Memory Controller Bank5_6
  */

typedef struct
{
  __IO uint32_t SDCR[2];        /*!< SDRAM Control registers ,      Address offset: 0x140-0x144  */
  __IO uint32_t SDTR[2];        /*!< SDRAM Timing registers ,       Address offset: 0x148-0x14C  */
  __IO uint32_t SDCMR;       /*!< SDRAM Command Mode register,    Address offset: 0x150  */
  __IO uint32_t SDRTR;       /*!< SDRAM Refresh Timer register,   Address offset: 0x154  */
  __IO uint32_t SDSR;        /*!< SDRAM Status register,          Address offset: 0x158  */
} FMC_Bank5_6_TypeDef;

/**
  * @brief General Purpose I/O
  */

typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;

/**
  * @brief System configuration controller
  */

typedef struct
{
  __IO uint32_t MEMRMP;       /*!< SYSCFG memory remap register,                      Address offset: 0x00      */
  __IO uint32_t PMC;          /*!< SYSCFG peripheral mode configuration register,     Address offset: 0x04      */
  __IO uint32_t EXTICR[4];    /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
  uint32_t      RESERVED[2];  /*!< Reserved, 0x18-0x1C                                                          */
  __IO uint32_t CMPCR;        /*!< SYSCFG Compensation cell control register,         Address offset: 0x20      */
} SYSCFG_TypeDef;

/**
  * @brief Inter-integrated Circuit Interface
  */

typedef struct
{
  __IO uint32_t CR1;        /*!< I2C Control register 1,     Address offset: 0x00 */
  __IO uint32_t CR2;        /*!< I2C Control register 2,     Address offset: 0x04 */
  __IO uint32_t OAR1;       /*!< I2C Own address register 1, Address offset: 0x08 */
  __IO uint32_t OAR2;       /*!< I2C Own address register 2, Address offset: 0x0C */
  __IO uint32_t DR;         /*!< I2C Data register,          Address offset: 0x10 */
  __IO uint32_t SR1;        /*!< I2C Status register 1,      Address offset: 0x14 */
  __IO uint32_t SR2;        /*!< I2C Status register 2,      Address offset: 0x18 */
  __IO uint32_t CCR;        /*!< I2C Clock control register, Address offset: 0x1C */
  __IO uint32_t TRISE;      /*!< I2C TRISE register,         Address offset: 0x20 */
  __IO uint32_t FLTR;       /*!< I2C FLTR register,          Address offset: 0x24 */
} I2C_TypeDef;

/**
  * @brief Independent WATCHDOG
  */

typedef struct
{
  __IO uint32_t KR;   /*!< IWDG Key register,       Address offset: 0x00 */
  __IO uint32_t PR;   /*!< IWDG Prescaler register, Address offset: 0x04 */
  __IO uint32_t RLR;  /*!< IWDG Reload register,    Address offset: 0x08 */
  __IO uint32_t SR;   /*!< IWDG Status register,    Address offset: 0x0C */
} IWDG_TypeDef;

/**
  * @brief LCD-TFT Display Controller
  */

typedef struct
{
  uint32_t      RESERVED0[2];  /*!< Reserved, 0x00-0x04 */
  __IO uint32_t SSCR;          /*!< LTDC Synchronization Size Configuration Register,    Address offset: 0x08 */
  __IO uint32_t BPCR;          /*!< LTDC Back Porch Configuration Register,              Address offset: 0x0C */
  __IO uint32_t AWCR;          /*!< LTDC Active Width Configuration Register,            Address offset: 0x10 */
  __IO uint32_t TWCR;          /*!< LTDC Total Width Configuration Register,             Address offset: 0x14 */
  __IO uint32_t GCR;           /*!< LTDC Global Control Register,                        Address offset: 0x18 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x1C-0x20 */
  __IO uint32_t SRCR;          /*!< LTDC Shadow Reload Configuration Register,           Address offset: 0x24 */
  uint32_t      RESERVED2[1];  /*!< Reserved, 0x28 */
  __IO uint32_t BCCR;          /*!< LTDC Background Color Configuration Register,        Address offset: 0x2C */
  uint32_t      RESERVED3[1];  /*!< Reserved, 0x30 */
  __IO uint32_t IER;           /*!< LTDC Interrupt Enable Register,                      Address offset: 0x34 */
  __IO uint32_t ISR;           /*!< LTDC Interrupt Status Register,                      Address offset: 0x38 */
  __IO uint32_t ICR;           /*!< LTDC Interrupt Clear Register,                       Address offset: 0x3C */
  __IO uint32_t LIPCR;         /*!< LTDC Line Interrupt Position Configuration Register, Address offset: 0x40 */
  __IO uint32_t CPSR;          /*!< LTDC Current Position Status Register,               Address offset: 0x44 */
  __IO uint32_t CDSR;         /*!< LTDC Current Display Status Register,                       Address offset: 0x48 */
} LTDC_TypeDef;

/**
  * @brief LCD-TFT Display layer x Controller
  */

typedef struct
{
  __IO uint32_t CR;            /*!< LTDC Layerx Control Register                                  Address offset: 0x84 */
  __IO uint32_t WHPCR;         /*!< LTDC Layerx Window Horizontal Position Configuration Register Address offset: 0x88 */
  __IO uint32_t WVPCR;         /*!< LTDC Layerx Window Vertical Position Configuration Register   Address offset: 0x8C */
  __IO uint32_t CKCR;          /*!< LTDC Layerx Color Keying Configuration Register               Address offset: 0x90 */
  __IO uint32_t PFCR;          /*!< LTDC Layerx Pixel Format Configuration Register               Address offset: 0x94 */
  __IO uint32_t CACR;          /*!< LTDC Layerx Constant Alpha Configuration Register             Address offset: 0x98 */
  __IO uint32_t DCCR;          /*!< LTDC Layerx Default Color Configuration Register              Address offset: 0x9C */
  __IO uint32_t BFCR;          /*!< LTDC Layerx Blending Factors Configuration Register           Address offset: 0xA0 */
  uint32_t      RESERVED0[2];  /*!< Reserved */
  __IO uint32_t CFBAR;         /*!< LTDC Layerx Color Frame Buffer Address Register               Address offset: 0xAC */
  __IO uint32_t CFBLR;         /*!< LTDC Layerx Color Frame Buffer Length Register                Address offset: 0xB0 */
  __IO uint32_t CFBLNR;        /*!< LTDC Layerx ColorFrame Buffer Line Number Register            Address offset: 0xB4 */
  uint32_t      RESERVED1[3];  /*!< Reserved */
  __IO uint32_t CLUTWR;         /*!< LTDC Layerx CLUT Write Register                               Address offset: 0x144 */

} LTDC_Layer_TypeDef;

/**
  * @brief Power Control
  */

typedef struct
{
  __IO uint32_t CR;   /*!< PWR power control register,        Address offset: 0x00 */
  __IO uint32_t CSR;  /*!< PWR power control/status register, Address offset: 0x04 */
} PWR_TypeDef;

/**
  * @brief Reset and Clock Control
  */

typedef struct
{
  __IO uint32_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  __IO uint32_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  __IO uint32_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  __IO uint32_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  __IO uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  __IO uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  __IO uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  __IO uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  __IO uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  __IO uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  __IO uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  __IO uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  __IO uint32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  __IO uint32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  __IO uint32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  uint32_t      RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  __IO uint32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  __IO uint32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  __IO uint32_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  __IO uint32_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  __IO uint32_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  __IO uint32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
  __IO uint32_t PLLSAICFGR;    /*!< RCC PLLSAI configuration register,                           Address offset: 0x88 */
  __IO uint32_t DCKCFGR;       /*!< RCC Dedicated Clocks configuration register,                 Address offset: 0x8C */

} RCC_TypeDef;

/**
  * @brief Real-Time Clock
  */

typedef struct
{
  __IO uint32_t TR;      /*!< RTC time register,                                        Address offset: 0x00 */
  __IO uint32_t DR;      /*!< RTC date register,                                        Address offset: 0x04 */
  __IO uint32_t CR;      /*!< RTC control register,                                     Address offset: 0x08 */
  __IO uint32_t ISR;     /*!< RTC initialization and status register,                   Address offset: 0x0C */
  __IO uint32_t PRER;    /*!< RTC prescaler register,                                   Address offset: 0x10 */
  __IO uint32_t WUTR;    /*!< RTC wakeup timer register,                                Address offset: 0x14 */
  __IO uint32_t CALIBR;  /*!< RTC calibration register,                                 Address offset: 0x18 */
  __IO uint32_t ALRMAR;  /*!< RTC alarm A register,                                     Address offset: 0x1C */
  __IO uint32_t ALRMBR;  /*!< RTC alarm B register,                                     Address offset: 0x20 */
  __IO uint32_t WPR;     /*!< RTC write protection register,                            Address offset: 0x24 */
  __IO uint32_t SSR;     /*!< RTC sub second register,                                  Address offset: 0x28 */
  __IO uint32_t SHIFTR;  /*!< RTC shift control register,                               Address offset: 0x2C */
  __IO uint32_t TSTR;    /*!< RTC time stamp time register,                             Address offset: 0x30 */
  __IO uint32_t TSDR;    /*!< RTC time stamp date register,                             Address offset: 0x34 */
  __IO uint32_t TSSSR;   /*!< RTC time-stamp sub second register,                       Address offset: 0x38 */
  __IO uint32_t CALR;    /*!< RTC calibration register,                                 Address offset: 0x3C */
  __IO uint32_t TAFCR;   /*!< RTC tamper and alternate function configuration register, Address offset: 0x40 */
  __IO uint32_t ALRMASSR;/*!< RTC alarm A sub second register,                          Address offset: 0x44 */
  __IO uint32_t ALRMBSSR;/*!< RTC alarm B sub second register,                          Address offset: 0x48 */
  uint32_t RESERVED7;    /*!< Reserved, 0x4C                                                                 */
  __IO uint32_t BKP0R;   /*!< RTC backup register 1,                                    Address offset: 0x50 */
  __IO uint32_t BKP1R;   /*!< RTC backup register 1,                                    Address offset: 0x54 */
  __IO uint32_t BKP2R;   /*!< RTC backup register 2,                                    Address offset: 0x58 */
  __IO uint32_t BKP3R;   /*!< RTC backup register 3,                                    Address offset: 0x5C */
  __IO uint32_t BKP4R;   /*!< RTC backup register 4,                                    Address offset: 0x60 */
  __IO uint32_t BKP5R;   /*!< RTC backup register 5,                                    Address offset: 0x64 */
  __IO uint32_t BKP6R;   /*!< RTC backup register 6,                                    Address offset: 0x68 */
  __IO uint32_t BKP7R;   /*!< RTC backup register 7,                                    Address offset: 0x6C */
  __IO uint32_t BKP8R;   /*!< RTC backup register 8,                                    Address offset: 0x70 */
  __IO uint32_t BKP9R;   /*!< RTC backup register 9,                                    Address offset: 0x74 */
  __IO uint32_t BKP10R;  /*!< RTC backup register 10,                                   Address offset: 0x78 */
  __IO uint32_t BKP11R;  /*!< RTC backup register 11,                                   Address offset: 0x7C */
  __IO uint32_t BKP12R;  /*!< RTC backup register 12,                                   Address offset: 0x80 */
  __IO uint32_t BKP13R;  /*!< RTC backup register 13,                                   Address offset: 0x84 */
  __IO uint32_t BKP14R;  /*!< RTC backup register 14,                                   Address offset: 0x88 */
  __IO uint32_t BKP15R;  /*!< RTC backup register 15,                                   Address offset: 0x8C */
  __IO uint32_t BKP16R;  /*!< RTC backup register 16,                                   Address offset: 0x90 */
  __IO uint32_t BKP17R;  /*!< RTC backup register 17,                                   Address offset: 0x94 */
  __IO uint32_t BKP18R;  /*!< RTC backup register 18,                                   Address offset: 0x98 */
  __IO uint32_t BKP19R;  /*!< RTC backup register 19,                                   Address offset: 0x9C */
} RTC_TypeDef;

/**
  * @brief Serial Audio Interface
  */

typedef struct
{
  __IO uint32_t GCR;      /*!< SAI global configuration register,        Address offset: 0x00 */
} SAI_TypeDef;

typedef struct
{
  __IO uint32_t CR1;      /*!< SAI block x configuration register 1,     Address offset: 0x04 */
  __IO uint32_t CR2;      /*!< SAI block x configuration register 2,     Address offset: 0x08 */
  __IO uint32_t FRCR;     /*!< SAI block x frame configuration register, Address offset: 0x0C */
  __IO uint32_t SLOTR;    /*!< SAI block x slot register,                Address offset: 0x10 */
  __IO uint32_t IMR;      /*!< SAI block x interrupt mask register,      Address offset: 0x14 */
  __IO uint32_t SR;       /*!< SAI block x status register,              Address offset: 0x18 */
  __IO uint32_t CLRFR;    /*!< SAI block x clear flag register,          Address offset: 0x1C */
  __IO uint32_t DR;       /*!< SAI block x data register,                Address offset: 0x20 */
} SAI_Block_TypeDef;

/**
  * @brief SD host Interface
  */

typedef struct
{
  __IO uint32_t POWER;          /*!< SDIO power control register,    Address offset: 0x00 */
  __IO uint32_t CLKCR;          /*!< SDI clock control register,     Address offset: 0x04 */
  __IO uint32_t ARG;            /*!< SDIO argument register,         Address offset: 0x08 */
  __IO uint32_t CMD;            /*!< SDIO command register,          Address offset: 0x0C */
  __I uint32_t  RESPCMD;        /*!< SDIO command response register, Address offset: 0x10 */
  __I uint32_t  RESP1;          /*!< SDIO response 1 register,       Address offset: 0x14 */
  __I uint32_t  RESP2;          /*!< SDIO response 2 register,       Address offset: 0x18 */
  __I uint32_t  RESP3;          /*!< SDIO response 3 register,       Address offset: 0x1C */
  __I uint32_t  RESP4;          /*!< SDIO response 4 register,       Address offset: 0x20 */
  __IO uint32_t DTIMER;         /*!< SDIO data timer register,       Address offset: 0x24 */
  __IO uint32_t DLEN;           /*!< SDIO data length register,      Address offset: 0x28 */
  __IO uint32_t DCTRL;          /*!< SDIO data control register,     Address offset: 0x2C */
  __I uint32_t  DCOUNT;         /*!< SDIO data counter register,     Address offset: 0x30 */
  __I uint32_t  STA;            /*!< SDIO status register,           Address offset: 0x34 */
  __IO uint32_t ICR;            /*!< SDIO interrupt clear register,  Address offset: 0x38 */
  __IO uint32_t MASK;           /*!< SDIO mask register,             Address offset: 0x3C */
  uint32_t      RESERVED0[2];   /*!< Reserved, 0x40-0x44                                  */
  __I uint32_t  FIFOCNT;        /*!< SDIO FIFO counter register,     Address offset: 0x48 */
  uint32_t      RESERVED1[13];  /*!< Reserved, 0x4C-0x7C                                  */
  __IO uint32_t FIFO;           /*!< SDIO data FIFO register,        Address offset: 0x80 */
} SDIO_TypeDef;

/**
  * @brief Serial Peripheral Interface
  */

typedef struct
{
  __IO uint32_t CR1;        /*!< SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */
  __IO uint32_t CR2;        /*!< SPI control register 2,                             Address offset: 0x04 */
  __IO uint32_t SR;         /*!< SPI status register,                                Address offset: 0x08 */
  __IO uint32_t DR;         /*!< SPI data register,                                  Address offset: 0x0C */
  __IO uint32_t CRCPR;      /*!< SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
  __IO uint32_t RXCRCR;     /*!< SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
  __IO uint32_t TXCRCR;     /*!< SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
  __IO uint32_t I2SCFGR;    /*!< SPI_I2S configuration register,                     Address offset: 0x1C */
  __IO uint32_t I2SPR;      /*!< SPI_I2S prescaler register,                         Address offset: 0x20 */
} SPI_TypeDef;

/**
  * @brief TIM
  */

typedef struct
{
  __IO uint32_t CR1;         /*!< TIM control register 1,              Address offset: 0x00 */
  __IO uint32_t CR2;         /*!< TIM control register 2,              Address offset: 0x04 */
  __IO uint32_t SMCR;        /*!< TIM slave mode control register,     Address offset: 0x08 */
  __IO uint32_t DIER;        /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
  __IO uint32_t SR;          /*!< TIM status register,                 Address offset: 0x10 */
  __IO uint32_t EGR;         /*!< TIM event generation register,       Address offset: 0x14 */
  __IO uint32_t CCMR1;       /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
  __IO uint32_t CCMR2;       /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
  __IO uint32_t CCER;        /*!< TIM capture/compare enable register, Address offset: 0x20 */
  __IO uint32_t CNT;         /*!< TIM counter register,                Address offset: 0x24 */
  __IO uint32_t PSC;         /*!< TIM prescaler,                       Address offset: 0x28 */
  __IO uint32_t ARR;         /*!< TIM auto-reload register,            Address offset: 0x2C */
  __IO uint32_t RCR;         /*!< TIM repetition counter register,     Address offset: 0x30 */
  __IO uint32_t CCR[4];        /*!< TIM capture/compare register 1,      Address offset: 0x34 */
  __IO uint32_t BDTR;        /*!< TIM break and dead-time register,    Address offset: 0x44 */
  __IO uint32_t DCR;         /*!< TIM DMA control register,            Address offset: 0x48 */
  __IO uint32_t DMAR;        /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
  __IO uint32_t OR;          /*!< TIM option register,                 Address offset: 0x50 */
} TIM_TypeDef;

/**
  * @brief Universal Synchronous Asynchronous Receiver Transmitter
  */

typedef struct
{
  __IO uint32_t SR;         /*!< USART Status register,                   Address offset: 0x00 */
  __IO uint32_t DR;         /*!< USART Data register,                     Address offset: 0x04 */
  __IO uint32_t BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
  __IO uint32_t CR1;        /*!< USART Control register 1,                Address offset: 0x0C */
  __IO uint32_t CR2;        /*!< USART Control register 2,                Address offset: 0x10 */
  __IO uint32_t CR3;        /*!< USART Control register 3,                Address offset: 0x14 */
  __IO uint32_t GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
} USART_TypeDef;

/**
  * @brief Window WATCHDOG
  */

typedef struct
{
  __IO uint32_t CR;   /*!< WWDG Control register,       Address offset: 0x00 */
  __IO uint32_t CFR;  /*!< WWDG Configuration register, Address offset: 0x04 */
  __IO uint32_t SR;   /*!< WWDG Status register,        Address offset: 0x08 */
} WWDG_TypeDef;


/**
  * @brief RNG
  */

typedef struct
{
  __IO uint32_t CR;  /*!< RNG control register, Address offset: 0x00 */
  __IO uint32_t SR;  /*!< RNG status register,  Address offset: 0x04 */
  __IO uint32_t DR;  /*!< RNG data register,    Address offset: 0x08 */
} RNG_TypeDef;


/**
  * @brief __USB_OTG_Core_register
  */
typedef struct
{
  __IO uint32_t GOTGCTL;      /*!<  USB_OTG Control and Status Register    000h */
  __IO uint32_t GOTGINT;      /*!<  USB_OTG Interrupt Register             004h */
  __IO uint32_t GAHBCFG;      /*!<  Core AHB Configuration Register        008h */
  __IO uint32_t GUSBCFG;      /*!<  Core USB Configuration Register        00Ch */
  __IO uint32_t GRSTCTL;      /*!<  Core Reset Register                    010h */
  __IO uint32_t GINTSTS;      /*!<  Core Interrupt Register                014h */
  __IO uint32_t GINTMSK;      /*!<  Core Interrupt Mask Register           018h */
  __IO uint32_t GRXSTSR;      /*!<  Receive Sts Q Read Register            01Ch */
  __IO uint32_t GRXSTSP;      /*!<  Receive Sts Q Read & POP Register      020h */
  __IO uint32_t GRXFSIZ;      /* Receive FIFO Size Register                024h */
  __IO uint32_t DIEPTXF0_HNPTXFSIZ;   /*!<  EP0 / Non Periodic Tx FIFO Size Register 028h*/
  __IO uint32_t HNPTXSTS;     /*!<  Non Periodic Tx FIFO/Queue Sts reg     02Ch */
  uint32_t Reserved30[2];     /* Reserved                           030h*/
  __IO uint32_t GCCFG;        /* General Purpose IO Register        038h*/
  __IO uint32_t CID;          /* User ID Register                   03Ch*/
  uint32_t  Reserved40[48];   /* Reserved                      040h-0FFh*/
  __IO uint32_t HPTXFSIZ; /* Host Periodic Tx FIFO Size Reg     100h*/
  __IO uint32_t DIEPTXF[0x0F];/* dev Periodic Transmit FIFO */
}
USB_OTG_GlobalTypeDef;


/**
  * @brief __device_Registers
  */
typedef struct
{
  __IO uint32_t DCFG;         /* dev Configuration Register   800h*/
  __IO uint32_t DCTL;         /* dev Control Register         804h*/
  __IO uint32_t DSTS;         /* dev Status Register (RO)     808h*/
  uint32_t Reserved0C;           /* Reserved                     80Ch*/
  __IO uint32_t DIEPMSK;   /* dev IN Endpoint Mask         810h*/
  __IO uint32_t DOEPMSK;  /* dev OUT Endpoint Mask        814h*/
  __IO uint32_t DAINT;     /* dev All Endpoints Itr Reg    818h*/
  __IO uint32_t DAINTMSK; /* dev All Endpoints Itr Mask   81Ch*/
  uint32_t  Reserved20;          /* Reserved                     820h*/
  uint32_t Reserved9;       /* Reserved                     824h*/
  __IO uint32_t DVBUSDIS;    /* dev VBUS discharge Register  828h*/
  __IO uint32_t DVBUSPULSE;  /* dev VBUS Pulse Register      82Ch*/
  __IO uint32_t DTHRCTL;     /* dev thr                      830h*/
  __IO uint32_t DIEPEMPMSK; /* dev empty msk             834h*/
  __IO uint32_t DEACHINT;    /* dedicated EP interrupt       838h*/
  __IO uint32_t DEACHMSK;    /* dedicated EP msk             83Ch*/
  uint32_t Reserved40;      /* dedicated EP mask           840h*/
  __IO uint32_t DINEP1MSK;  /* dedicated EP mask           844h*/
  uint32_t  Reserved44[15];      /* Reserved                 844-87Ch*/
  __IO uint32_t DOUTEP1MSK; /* dedicated EP msk            884h*/
}
USB_OTG_DeviceTypeDef;


/**
  * @brief __IN_Endpoint-Specific_Register
  */
typedef struct
{
  __IO uint32_t DIEPCTL; /* dev IN Endpoint Control Reg 900h + (ep_num * 20h) + 00h*/
  uint32_t Reserved04;             /* Reserved                       900h + (ep_num * 20h) + 04h*/
  __IO uint32_t DIEPINT; /* dev IN Endpoint Itr Reg     900h + (ep_num * 20h) + 08h*/
  uint32_t Reserved0C;             /* Reserved                       900h + (ep_num * 20h) + 0Ch*/
  __IO uint32_t DIEPTSIZ; /* IN Endpoint Txfer Size   900h + (ep_num * 20h) + 10h*/
  __IO uint32_t DIEPDMA; /* IN Endpoint DMA Address Reg    900h + (ep_num * 20h) + 14h*/
  __IO uint32_t DTXFSTS;/*IN Endpoint Tx FIFO Status Reg 900h + (ep_num * 20h) + 18h*/
  uint32_t Reserved18;             /* Reserved  900h+(ep_num*20h)+1Ch-900h+ (ep_num * 20h) + 1Ch*/
}
USB_OTG_INEndpointTypeDef;


/**
  * @brief __OUT_Endpoint-Specific_Registers
  */
typedef struct
{
  __IO uint32_t DOEPCTL;       /* dev OUT Endpoint Control Reg  B00h + (ep_num * 20h) + 00h*/
  uint32_t Reserved04;         /* Reserved                      B00h + (ep_num * 20h) + 04h*/
  __IO uint32_t DOEPINT;       /* dev OUT Endpoint Itr Reg      B00h + (ep_num * 20h) + 08h*/
  uint32_t Reserved0C;         /* Reserved                      B00h + (ep_num * 20h) + 0Ch*/
  __IO uint32_t DOEPTSIZ;      /* dev OUT Endpoint Txfer Size   B00h + (ep_num * 20h) + 10h*/
  __IO uint32_t DOEPDMA;       /* dev OUT Endpoint DMA Address  B00h + (ep_num * 20h) + 14h*/
  uint32_t Reserved18[2];      /* Reserved B00h + (ep_num * 20h) + 18h - B00h + (ep_num * 20h) + 1Ch*/
}
USB_OTG_OUTEndpointTypeDef;


/**
  * @brief __Host_Mode_Register_Structures
  */
typedef struct
{
  __IO uint32_t HCFG;             /* Host Configuration Register    400h*/
  __IO uint32_t HFIR;      /* Host Frame Interval Register   404h*/
  __IO uint32_t HFNUM;         /* Host Frame Nbr/Frame Remaining 408h*/
  uint32_t Reserved40C;                   /* Reserved                       40Ch*/
  __IO uint32_t HPTXSTS;   /* Host Periodic Tx FIFO/ Queue Status 410h*/
  __IO uint32_t HAINT;   /* Host All Channels Interrupt Register 414h*/
  __IO uint32_t HAINTMSK;   /* Host All Channels Interrupt Mask 418h*/
}
USB_OTG_HostTypeDef;

/**
  * @brief __Host_Channel_Specific_Registers
  */
typedef struct
{
  __IO uint32_t HCCHAR;
  __IO uint32_t HCSPLT;
  __IO uint32_t HCINT;
  __IO uint32_t HCINTMSK;
  __IO uint32_t HCTSIZ;
  __IO uint32_t HCDMA;
  uint32_t Reserved[2];
}
USB_OTG_HostChannelTypeDef;
/**
  * @}
  */

/** @addtogroup Peripheral_memory_map
  * @{
  */
#define FLASH_BASE            ((uint32_t)0x08000000U) /*!< FLASH(up to 2 MB) base address in the alias region                         */
#define CCMDATARAM_BASE       ((uint32_t)0x10000000U) /*!< CCM(core coupled memory) data RAM(64 KB) base address in the alias region  */
#define SRAM1_BASE            ((uint32_t)0x20000000U) /*!< SRAM1(112 KB) base address in the alias region                             */
#define SRAM2_BASE            ((uint32_t)0x2001C000U) /*!< SRAM2(16 KB) base address in the alias region                              */
#define SRAM3_BASE            ((uint32_t)0x20020000U) /*!< SRAM3(64 KB) base address in the alias region                              */
#define PERIPH_BASE           ((uint32_t)0x40000000U) /*!< Peripheral base address in the alias region                                */
#define BKPSRAM_BASE          ((uint32_t)0x40024000U) /*!< Backup SRAM(4 KB) base address in the alias region                         */
#define FMC_R_BASE            ((uint32_t)0xA0000000U) /*!< FMC registers base address                                                 */
#define SRAM1_BB_BASE         ((uint32_t)0x22000000U) /*!< SRAM1(112 KB) base address in the bit-band region                             */
#define SRAM2_BB_BASE         ((uint32_t)0x22380000U) /*!< SRAM2(16 KB) base address in the bit-band region                              */
#define SRAM3_BB_BASE         ((uint32_t)0x22400000U) /*!< SRAM3(64 KB) base address in the bit-band region                              */
#define PERIPH_BB_BASE        ((uint32_t)0x42000000U) /*!< Peripheral base address in the bit-band region                                */
#define BKPSRAM_BB_BASE       ((uint32_t)0x42480000U) /*!< Backup SRAM(4 KB) base address in the bit-band region                         */
#define FLASH_END             ((uint32_t)0x081FFFFFU) /*!< FLASH end address */
#define CCMDATARAM_END        ((uint32_t)0x1000FFFFU) /*!< CCM data RAM end address */

/* Legacy defines */
#define SRAM_BASE             SRAM1_BASE
#define SRAM_BB_BASE          SRAM1_BB_BASE


/*!< Peripheral memory map */
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000U)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000U)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000U)

/*!< APB1 peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000U)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400U)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800U)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00U)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000U)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400U)
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800U)
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00U)
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000U)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800U)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00U)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000U)
#define I2S2ext_BASE          (APB1PERIPH_BASE + 0x3400U)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800U)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00U)
#define I2S3ext_BASE          (APB1PERIPH_BASE + 0x4000U)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400U)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800U)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00U)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000U)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400U)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800U)
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00U)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400U)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800U)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000U)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400U)
#define UART7_BASE            (APB1PERIPH_BASE + 0x7800U)
#define UART8_BASE            (APB1PERIPH_BASE + 0x7C00U)

/*!< APB2 peripherals */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000U)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400U)
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000U)
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400U)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000U)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100U)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200U)
#define ADC_BASE              (APB2PERIPH_BASE + 0x2300U)
#define SDIO_BASE             (APB2PERIPH_BASE + 0x2C00U)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000U)
#define SPI4_BASE             (APB2PERIPH_BASE + 0x3400U)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800U)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00U)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000U)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400U)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800U)
#define SPI5_BASE             (APB2PERIPH_BASE + 0x5000U)
#define SPI6_BASE             (APB2PERIPH_BASE + 0x5400U)
#define SAI1_BASE             (APB2PERIPH_BASE + 0x5800U)
#define SAI1_Block_A_BASE     (SAI1_BASE + 0x004U)
#define SAI1_Block_B_BASE     (SAI1_BASE + 0x024U)
#define LTDC_BASE             (APB2PERIPH_BASE + 0x6800U)
#define LTDC_Layer1_BASE      (LTDC_BASE + 0x84U)
#define LTDC_Layer2_BASE      (LTDC_BASE + 0x104U)

/*!< AHB1 peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000U)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400U)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800U)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00U)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000U)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400U)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800U)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00U)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000U)
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400U)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800U)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000U)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800U)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00U)
#define DMA1_BASE             (AHB1PERIPH_BASE + 0x6000U)
#define DMA1_Stream0_BASE     (DMA1_BASE + 0x010U)
#define DMA1_Stream1_BASE     (DMA1_BASE + 0x028U)
#define DMA1_Stream2_BASE     (DMA1_BASE + 0x040U)
#define DMA1_Stream3_BASE     (DMA1_BASE + 0x058U)
#define DMA1_Stream4_BASE     (DMA1_BASE + 0x070U)
#define DMA1_Stream5_BASE     (DMA1_BASE + 0x088U)
#define DMA1_Stream6_BASE     (DMA1_BASE + 0x0A0U)
#define DMA1_Stream7_BASE     (DMA1_BASE + 0x0B8U)
#define DMA2_BASE             (AHB1PERIPH_BASE + 0x6400U)
#define DMA2_Stream0_BASE     (DMA2_BASE + 0x010U)
#define DMA2_Stream1_BASE     (DMA2_BASE + 0x028U)
#define DMA2_Stream2_BASE     (DMA2_BASE + 0x040U)
#define DMA2_Stream3_BASE     (DMA2_BASE + 0x058U)
#define DMA2_Stream4_BASE     (DMA2_BASE + 0x070U)
#define DMA2_Stream5_BASE     (DMA2_BASE + 0x088U)
#define DMA2_Stream6_BASE     (DMA2_BASE + 0x0A0U)
#define DMA2_Stream7_BASE     (DMA2_BASE + 0x0B8U)
#define ETH_BASE              (AHB1PERIPH_BASE + 0x8000U)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100U)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700U)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000U)
#define DMA2D_BASE            (AHB1PERIPH_BASE + 0xB000U)

/*!< AHB2 peripherals */
#define DCMI_BASE             (AHB2PERIPH_BASE + 0x50000U)
#define RNG_BASE              (AHB2PERIPH_BASE + 0x60800U)

/*!< FMC Bankx registers base address */
#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000U)
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104U)
#define FMC_Bank2_3_R_BASE    (FMC_R_BASE + 0x0060U)
#define FMC_Bank4_R_BASE      (FMC_R_BASE + 0x00A0U)
#define FMC_Bank5_6_R_BASE    (FMC_R_BASE + 0x0140U)

/* Debug MCU registers base address */
#define DBGMCU_BASE           ((uint32_t )0xE0042000U)

/*!< USB registers base address */
#define USB_OTG_HS_PERIPH_BASE               ((uint32_t )0x40040000U)
#define USB_OTG_FS_PERIPH_BASE               ((uint32_t )0x50000000U)

#define USB_OTG_GLOBAL_BASE                  ((uint32_t )0x000U)
#define USB_OTG_DEVICE_BASE                  ((uint32_t )0x800U)
#define USB_OTG_IN_ENDPOINT_BASE             ((uint32_t )0x900U)
#define USB_OTG_OUT_ENDPOINT_BASE            ((uint32_t )0xB00U)
#define USB_OTG_EP_REG_SIZE                  ((uint32_t )0x20U)
#define USB_OTG_HOST_BASE                    ((uint32_t )0x400U)
#define USB_OTG_HOST_PORT_BASE               ((uint32_t )0x440U)
#define USB_OTG_HOST_CHANNEL_BASE            ((uint32_t )0x500U)
#define USB_OTG_HOST_CHANNEL_SIZE            ((uint32_t )0x20U)
#define USB_OTG_PCGCCTL_BASE                 ((uint32_t )0xE00U)
#define USB_OTG_FIFO_BASE                    ((uint32_t )0x1000U)
#define USB_OTG_FIFO_SIZE                    ((uint32_t )0x1000U)

/**
  * @}
  */

/** @addtogroup Peripheral_declaration
  * @{
  */
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)
#define TIM12               ((TIM_TypeDef *) TIM12_BASE)
#define TIM13               ((TIM_TypeDef *) TIM13_BASE)
#define TIM14               ((TIM_TypeDef *) TIM14_BASE)
#define RTC                 ((RTC_TypeDef *) RTC_BASE)
#define WWDG                ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define I2S2ext             ((SPI_TypeDef *) I2S2ext_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)
#define I2S3ext             ((SPI_TypeDef *) I2S3ext_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define USART3              ((USART_TypeDef *) USART3_BASE)
#define UART4               ((USART_TypeDef *) UART4_BASE)
#define UART5               ((USART_TypeDef *) UART5_BASE)
#define I2C1                ((I2C_TypeDef *) I2C1_BASE)
#define I2C2                ((I2C_TypeDef *) I2C2_BASE)
#define I2C3                ((I2C_TypeDef *) I2C3_BASE)
#define CAN1                ((CAN_TypeDef *) CAN1_BASE)
#define CAN2                ((CAN_TypeDef *) CAN2_BASE)
#define PWR                 ((PWR_TypeDef *) PWR_BASE)
#define DAC                 ((DAC_TypeDef *) DAC_BASE)
#define UART7               ((USART_TypeDef *) UART7_BASE)
#define UART8               ((USART_TypeDef *) UART8_BASE)
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define USART6              ((USART_TypeDef *) USART6_BASE)
#define ADC                 ((ADC_Common_TypeDef *) ADC_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)
#define ADC2                ((ADC_TypeDef *) ADC2_BASE)
#define ADC3                ((ADC_TypeDef *) ADC3_BASE)
#define SDIO                ((SDIO_TypeDef *) SDIO_BASE)
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define SPI4                ((SPI_TypeDef *) SPI4_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define TIM9                ((TIM_TypeDef *) TIM9_BASE)
#define TIM10               ((TIM_TypeDef *) TIM10_BASE)
#define TIM11               ((TIM_TypeDef *) TIM11_BASE)
#define SPI5                ((SPI_TypeDef *) SPI5_BASE)
#define SPI6                ((SPI_TypeDef *) SPI6_BASE)
#define SAI1                ((SAI_TypeDef *) SAI1_BASE)
#define SAI1_Block_A        ((SAI_Block_TypeDef *)SAI1_Block_A_BASE)
#define SAI1_Block_B        ((SAI_Block_TypeDef *)SAI1_Block_B_BASE)
#define LTDC                ((LTDC_TypeDef *)LTDC_BASE)
#define LTDC_Layer1         ((LTDC_Layer_TypeDef *)LTDC_Layer1_BASE)
#define LTDC_Layer2         ((LTDC_Layer_TypeDef *)LTDC_Layer2_BASE)

#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI               ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ               ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK               ((GPIO_TypeDef *) GPIOK_BASE)
#define CRC                 ((CRC_TypeDef *) CRC_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define DMA1                ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_Stream0        ((DMA_Stream_TypeDef *) DMA1_Stream0_BASE)
#define DMA1_Stream1        ((DMA_Stream_TypeDef *) DMA1_Stream1_BASE)
#define DMA1_Stream2        ((DMA_Stream_TypeDef *) DMA1_Stream2_BASE)
#define DMA1_Stream3        ((DMA_Stream_TypeDef *) DMA1_Stream3_BASE)
#define DMA1_Stream4        ((DMA_Stream_TypeDef *) DMA1_Stream4_BASE)
#define DMA1_Stream5        ((DMA_Stream_TypeDef *) DMA1_Stream5_BASE)
#define DMA1_Stream6        ((DMA_Stream_TypeDef *) DMA1_Stream6_BASE)
#define DMA1_Stream7        ((DMA_Stream_TypeDef *) DMA1_Stream7_BASE)
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)
#define DMA2_Stream0        ((DMA_Stream_TypeDef *) DMA2_Stream0_BASE)
#define DMA2_Stream1        ((DMA_Stream_TypeDef *) DMA2_Stream1_BASE)
#define DMA2_Stream2        ((DMA_Stream_TypeDef *) DMA2_Stream2_BASE)
#define DMA2_Stream3        ((DMA_Stream_TypeDef *) DMA2_Stream3_BASE)
#define DMA2_Stream4        ((DMA_Stream_TypeDef *) DMA2_Stream4_BASE)
#define DMA2_Stream5        ((DMA_Stream_TypeDef *) DMA2_Stream5_BASE)
#define DMA2_Stream6        ((DMA_Stream_TypeDef *) DMA2_Stream6_BASE)
#define DMA2_Stream7        ((DMA_Stream_TypeDef *) DMA2_Stream7_BASE)
#define ETH                 ((ETH_TypeDef *) ETH_BASE)
#define DMA2D               ((DMA2D_TypeDef *)DMA2D_BASE)
#define DCMI                ((DCMI_TypeDef *) DCMI_BASE)
#define RNG                 ((RNG_TypeDef *) RNG_BASE)
#define FMC_Bank1           ((FMC_Bank1_TypeDef *) FMC_Bank1_R_BASE)
#define FMC_Bank1E          ((FMC_Bank1E_TypeDef *) FMC_Bank1E_R_BASE)
#define FMC_Bank2_3         ((FMC_Bank2_3_TypeDef *) FMC_Bank2_3_R_BASE)
#define FMC_Bank4           ((FMC_Bank4_TypeDef *) FMC_Bank4_R_BASE)
#define FMC_Bank5_6         ((FMC_Bank5_6_TypeDef *) FMC_Bank5_6_R_BASE)

#define DBGMCU              ((DBGMCU_TypeDef *) DBGMCU_BASE)

#define USB_OTG_FS          ((USB_OTG_GlobalTypeDef *) USB_OTG_FS_PERIPH_BASE)
#define USB_OTG_HS          ((USB_OTG_GlobalTypeDef *) USB_OTG_HS_PERIPH_BASE)

/**
  * @}
  */

/** @addtogroup Exported_constants
  * @{
  */

  /** @addtogroup Peripheral_Registers_Bits_Definition
  * @{
  */

/******************************************************************************/
/*                         Peripheral Registers_Bits_Definition               */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                        Analog to Digital Converter                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for ADC_SR register  ********************/
#define  ADC_SR_AWD                          ((uint32_t)0x00000001U)        /*!<Analog watchdog flag */
#define  ADC_SR_EOC                          ((uint32_t)0x00000002U)        /*!<End of conversion */
#define  ADC_SR_JEOC                         ((uint32_t)0x00000004U)        /*!<Injected channel end of conversion */
#define  ADC_SR_JSTRT                        ((uint32_t)0x00000008U)        /*!<Injected channel Start flag */
#define  ADC_SR_STRT                         ((uint32_t)0x00000010U)        /*!<Regular channel Start flag */
#define  ADC_SR_OVR                          ((uint32_t)0x00000020U)        /*!<Overrun flag */

/*******************  Bit definition for ADC_CR1 register  ********************/
#define  ADC_CR1_AWDCH                       ((uint32_t)0x0000001FU)        /*!<AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define  ADC_CR1_AWDCH_0                     ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_CR1_AWDCH_1                     ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_CR1_AWDCH_2                     ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_CR1_AWDCH_3                     ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  ADC_CR1_AWDCH_4                     ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  ADC_CR1_EOCIE                       ((uint32_t)0x00000020U)        /*!<Interrupt enable for EOC */
#define  ADC_CR1_AWDIE                       ((uint32_t)0x00000040U)        /*!<AAnalog Watchdog interrupt enable */
#define  ADC_CR1_JEOCIE                      ((uint32_t)0x00000080U)        /*!<Interrupt enable for injected channels */
#define  ADC_CR1_SCAN                        ((uint32_t)0x00000100U)        /*!<Scan mode */
#define  ADC_CR1_AWDSGL                      ((uint32_t)0x00000200U)        /*!<Enable the watchdog on a single channel in scan mode */
#define  ADC_CR1_JAUTO                       ((uint32_t)0x00000400U)        /*!<Automatic injected group conversion */
#define  ADC_CR1_DISCEN                      ((uint32_t)0x00000800U)        /*!<Discontinuous mode on regular channels */
#define  ADC_CR1_JDISCEN                     ((uint32_t)0x00001000U)        /*!<Discontinuous mode on injected channels */
#define  ADC_CR1_DISCNUM                     ((uint32_t)0x0000E000U)        /*!<DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define  ADC_CR1_DISCNUM_0                   ((uint32_t)0x00002000U)        /*!<Bit 0 */
#define  ADC_CR1_DISCNUM_1                   ((uint32_t)0x00004000U)        /*!<Bit 1 */
#define  ADC_CR1_DISCNUM_2                   ((uint32_t)0x00008000U)        /*!<Bit 2 */
#define  ADC_CR1_JAWDEN                      ((uint32_t)0x00400000U)        /*!<Analog watchdog enable on injected channels */
#define  ADC_CR1_AWDEN                       ((uint32_t)0x00800000U)        /*!<Analog watchdog enable on regular channels */
#define  ADC_CR1_RES                         ((uint32_t)0x03000000U)        /*!<RES[2:0] bits (Resolution) */
#define  ADC_CR1_RES_0                       ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  ADC_CR1_RES_1                       ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  ADC_CR1_OVRIE                       ((uint32_t)0x04000000U)         /*!<overrun interrupt enable */

/*******************  Bit definition for ADC_CR2 register  ********************/
#define  ADC_CR2_ADON                        ((uint32_t)0x00000001U)        /*!<A/D Converter ON / OFF */
#define  ADC_CR2_CONT                        ((uint32_t)0x00000002U)        /*!<Continuous Conversion */
#define  ADC_CR2_DMA                         ((uint32_t)0x00000100U)        /*!<Direct Memory access mode */
#define  ADC_CR2_DDS                         ((uint32_t)0x00000200U)        /*!<DMA disable selection (Single ADC) */
#define  ADC_CR2_EOCS                        ((uint32_t)0x00000400U)        /*!<End of conversion selection */
#define  ADC_CR2_ALIGN                       ((uint32_t)0x00000800U)        /*!<Data Alignment */
#define  ADC_CR2_JEXTSEL                     ((uint32_t)0x000F0000U)        /*!<JEXTSEL[3:0] bits (External event select for injected group) */
#define  ADC_CR2_JEXTSEL_0                   ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  ADC_CR2_JEXTSEL_1                   ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  ADC_CR2_JEXTSEL_2                   ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  ADC_CR2_JEXTSEL_3                   ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  ADC_CR2_JEXTEN                      ((uint32_t)0x00300000U)        /*!<JEXTEN[1:0] bits (External Trigger Conversion mode for injected channelsp) */
#define  ADC_CR2_JEXTEN_0                    ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  ADC_CR2_JEXTEN_1                    ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  ADC_CR2_JSWSTART                    ((uint32_t)0x00400000U)        /*!<Start Conversion of injected channels */
#define  ADC_CR2_EXTSEL                      ((uint32_t)0x0F000000U)        /*!<EXTSEL[3:0] bits (External Event Select for regular group) */
#define  ADC_CR2_EXTSEL_0                    ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  ADC_CR2_EXTSEL_1                    ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  ADC_CR2_EXTSEL_2                    ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  ADC_CR2_EXTSEL_3                    ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  ADC_CR2_EXTEN                       ((uint32_t)0x30000000U)        /*!<EXTEN[1:0] bits (External Trigger Conversion mode for regular channelsp) */
#define  ADC_CR2_EXTEN_0                     ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  ADC_CR2_EXTEN_1                     ((uint32_t)0x20000000U)        /*!<Bit 1 */
#define  ADC_CR2_SWSTART                     ((uint32_t)0x40000000U)        /*!<Start Conversion of regular channels */

/******************  Bit definition for ADC_SMPR1 register  *******************/
#define  ADC_SMPR1_SMP10                     ((uint32_t)0x00000007U)        /*!<SMP10[2:0] bits (Channel 10 Sample time selection) */
#define  ADC_SMPR1_SMP10_0                   ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP10_1                   ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP10_2                   ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP11                     ((uint32_t)0x00000038U)        /*!<SMP11[2:0] bits (Channel 11 Sample time selection) */
#define  ADC_SMPR1_SMP11_0                   ((uint32_t)0x00000008U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP11_1                   ((uint32_t)0x00000010U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP11_2                   ((uint32_t)0x00000020U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP12                     ((uint32_t)0x000001C0U)        /*!<SMP12[2:0] bits (Channel 12 Sample time selection) */
#define  ADC_SMPR1_SMP12_0                   ((uint32_t)0x00000040U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP12_1                   ((uint32_t)0x00000080U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP12_2                   ((uint32_t)0x00000100U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP13                     ((uint32_t)0x00000E00U)        /*!<SMP13[2:0] bits (Channel 13 Sample time selection) */
#define  ADC_SMPR1_SMP13_0                   ((uint32_t)0x00000200U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP13_1                   ((uint32_t)0x00000400U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP13_2                   ((uint32_t)0x00000800U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP14                     ((uint32_t)0x00007000U)        /*!<SMP14[2:0] bits (Channel 14 Sample time selection) */
#define  ADC_SMPR1_SMP14_0                   ((uint32_t)0x00001000U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP14_1                   ((uint32_t)0x00002000U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP14_2                   ((uint32_t)0x00004000U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP15                     ((uint32_t)0x00038000U)        /*!<SMP15[2:0] bits (Channel 15 Sample time selection) */
#define  ADC_SMPR1_SMP15_0                   ((uint32_t)0x00008000U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP15_1                   ((uint32_t)0x00010000U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP15_2                   ((uint32_t)0x00020000U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP16                     ((uint32_t)0x001C0000U)        /*!<SMP16[2:0] bits (Channel 16 Sample time selection) */
#define  ADC_SMPR1_SMP16_0                   ((uint32_t)0x00040000U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP16_1                   ((uint32_t)0x00080000U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP16_2                   ((uint32_t)0x00100000U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP17                     ((uint32_t)0x00E00000U)        /*!<SMP17[2:0] bits (Channel 17 Sample time selection) */
#define  ADC_SMPR1_SMP17_0                   ((uint32_t)0x00200000U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP17_1                   ((uint32_t)0x00400000U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP17_2                   ((uint32_t)0x00800000U)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP18                     ((uint32_t)0x07000000U)        /*!<SMP18[2:0] bits (Channel 18 Sample time selection) */
#define  ADC_SMPR1_SMP18_0                   ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP18_1                   ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP18_2                   ((uint32_t)0x04000000U)        /*!<Bit 2 */

/******************  Bit definition for ADC_SMPR2 register  *******************/
#define  ADC_SMPR2_SMP0                      ((uint32_t)0x00000007U)        /*!<SMP0[2:0] bits (Channel 0 Sample time selection) */
#define  ADC_SMPR2_SMP0_0                    ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP0_1                    ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP0_2                    ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP1                      ((uint32_t)0x00000038U)        /*!<SMP1[2:0] bits (Channel 1 Sample time selection) */
#define  ADC_SMPR2_SMP1_0                    ((uint32_t)0x00000008U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP1_1                    ((uint32_t)0x00000010U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP1_2                    ((uint32_t)0x00000020U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP2                      ((uint32_t)0x000001C0U)        /*!<SMP2[2:0] bits (Channel 2 Sample time selection) */
#define  ADC_SMPR2_SMP2_0                    ((uint32_t)0x00000040U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP2_1                    ((uint32_t)0x00000080U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP2_2                    ((uint32_t)0x00000100U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP3                      ((uint32_t)0x00000E00U)        /*!<SMP3[2:0] bits (Channel 3 Sample time selection) */
#define  ADC_SMPR2_SMP3_0                    ((uint32_t)0x00000200U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP3_1                    ((uint32_t)0x00000400U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP3_2                    ((uint32_t)0x00000800U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP4                      ((uint32_t)0x00007000U)        /*!<SMP4[2:0] bits (Channel 4 Sample time selection) */
#define  ADC_SMPR2_SMP4_0                    ((uint32_t)0x00001000U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP4_1                    ((uint32_t)0x00002000U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP4_2                    ((uint32_t)0x00004000U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP5                      ((uint32_t)0x00038000U)        /*!<SMP5[2:0] bits (Channel 5 Sample time selection) */
#define  ADC_SMPR2_SMP5_0                    ((uint32_t)0x00008000U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP5_1                    ((uint32_t)0x00010000U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP5_2                    ((uint32_t)0x00020000U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP6                      ((uint32_t)0x001C0000U)        /*!<SMP6[2:0] bits (Channel 6 Sample time selection) */
#define  ADC_SMPR2_SMP6_0                    ((uint32_t)0x00040000U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP6_1                    ((uint32_t)0x00080000U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP6_2                    ((uint32_t)0x00100000U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP7                      ((uint32_t)0x00E00000U)        /*!<SMP7[2:0] bits (Channel 7 Sample time selection) */
#define  ADC_SMPR2_SMP7_0                    ((uint32_t)0x00200000U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP7_1                    ((uint32_t)0x00400000U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP7_2                    ((uint32_t)0x00800000U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP8                      ((uint32_t)0x07000000U)        /*!<SMP8[2:0] bits (Channel 8 Sample time selection) */
#define  ADC_SMPR2_SMP8_0                    ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP8_1                    ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP8_2                    ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP9                      ((uint32_t)0x38000000U)        /*!<SMP9[2:0] bits (Channel 9 Sample time selection) */
#define  ADC_SMPR2_SMP9_0                    ((uint32_t)0x08000000U)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP9_1                    ((uint32_t)0x10000000U)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP9_2                    ((uint32_t)0x20000000U)        /*!<Bit 2 */

/******************  Bit definition for ADC_JOFR1 register  *******************/
#define  ADC_JOFR1_JOFFSET1                  ((uint32_t)0x0FFFU)            /*!<Data offset for injected channel 1 */

/******************  Bit definition for ADC_JOFR2 register  *******************/
#define  ADC_JOFR2_JOFFSET2                  ((uint32_t)0x0FFFU)            /*!<Data offset for injected channel 2 */

/******************  Bit definition for ADC_JOFR3 register  *******************/
#define  ADC_JOFR3_JOFFSET3                  ((uint32_t)0x0FFFU)            /*!<Data offset for injected channel 3 */

/******************  Bit definition for ADC_JOFR4 register  *******************/
#define  ADC_JOFR4_JOFFSET4                  ((uint32_t)0x0FFFU)            /*!<Data offset for injected channel 4 */

/*******************  Bit definition for ADC_HTR register  ********************/
#define  ADC_HTR_HT                          ((uint32_t)0x0FFFU)            /*!<Analog watchdog high threshold */

/*******************  Bit definition for ADC_LTR register  ********************/
#define  ADC_LTR_LT                          ((uint32_t)0x0FFFU)            /*!<Analog watchdog low threshold */

/*******************  Bit definition for ADC_SQR1 register  *******************/
#define  ADC_SQR1_SQ13                       ((uint32_t)0x0000001FU)        /*!<SQ13[4:0] bits (13th conversion in regular sequence) */
#define  ADC_SQR1_SQ13_0                     ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_SQR1_SQ13_1                     ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_SQR1_SQ13_2                     ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_SQR1_SQ13_3                     ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  ADC_SQR1_SQ13_4                     ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  ADC_SQR1_SQ14                       ((uint32_t)0x000003E0U)        /*!<SQ14[4:0] bits (14th conversion in regular sequence) */
#define  ADC_SQR1_SQ14_0                     ((uint32_t)0x00000020U)        /*!<Bit 0 */
#define  ADC_SQR1_SQ14_1                     ((uint32_t)0x00000040U)        /*!<Bit 1 */
#define  ADC_SQR1_SQ14_2                     ((uint32_t)0x00000080U)        /*!<Bit 2 */
#define  ADC_SQR1_SQ14_3                     ((uint32_t)0x00000100U)        /*!<Bit 3 */
#define  ADC_SQR1_SQ14_4                     ((uint32_t)0x00000200U)        /*!<Bit 4 */
#define  ADC_SQR1_SQ15                       ((uint32_t)0x00007C00U)        /*!<SQ15[4:0] bits (15th conversion in regular sequence) */
#define  ADC_SQR1_SQ15_0                     ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  ADC_SQR1_SQ15_1                     ((uint32_t)0x00000800U)        /*!<Bit 1 */
#define  ADC_SQR1_SQ15_2                     ((uint32_t)0x00001000U)        /*!<Bit 2 */
#define  ADC_SQR1_SQ15_3                     ((uint32_t)0x00002000U)        /*!<Bit 3 */
#define  ADC_SQR1_SQ15_4                     ((uint32_t)0x00004000U)        /*!<Bit 4 */
#define  ADC_SQR1_SQ16                       ((uint32_t)0x000F8000U)        /*!<SQ16[4:0] bits (16th conversion in regular sequence) */
#define  ADC_SQR1_SQ16_0                     ((uint32_t)0x00008000U)        /*!<Bit 0 */
#define  ADC_SQR1_SQ16_1                     ((uint32_t)0x00010000U)        /*!<Bit 1 */
#define  ADC_SQR1_SQ16_2                     ((uint32_t)0x00020000U)        /*!<Bit 2 */
#define  ADC_SQR1_SQ16_3                     ((uint32_t)0x00040000U)        /*!<Bit 3 */
#define  ADC_SQR1_SQ16_4                     ((uint32_t)0x00080000U)        /*!<Bit 4 */
#define  ADC_SQR1_L                          ((uint32_t)0x00F00000U)        /*!<L[3:0] bits (Regular channel sequence length) */
#define  ADC_SQR1_L_0                        ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  ADC_SQR1_L_1                        ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  ADC_SQR1_L_2                        ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  ADC_SQR1_L_3                        ((uint32_t)0x00800000U)        /*!<Bit 3 */

/*******************  Bit definition for ADC_SQR2 register  *******************/
#define  ADC_SQR2_SQ7                        ((uint32_t)0x0000001FU)        /*!<SQ7[4:0] bits (7th conversion in regular sequence) */
#define  ADC_SQR2_SQ7_0                      ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_SQR2_SQ7_1                      ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_SQR2_SQ7_2                      ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_SQR2_SQ7_3                      ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  ADC_SQR2_SQ7_4                      ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  ADC_SQR2_SQ8                        ((uint32_t)0x000003E0U)        /*!<SQ8[4:0] bits (8th conversion in regular sequence) */
#define  ADC_SQR2_SQ8_0                      ((uint32_t)0x00000020U)        /*!<Bit 0 */
#define  ADC_SQR2_SQ8_1                      ((uint32_t)0x00000040U)        /*!<Bit 1 */
#define  ADC_SQR2_SQ8_2                      ((uint32_t)0x00000080U)        /*!<Bit 2 */
#define  ADC_SQR2_SQ8_3                      ((uint32_t)0x00000100U)        /*!<Bit 3 */
#define  ADC_SQR2_SQ8_4                      ((uint32_t)0x00000200U)        /*!<Bit 4 */
#define  ADC_SQR2_SQ9                        ((uint32_t)0x00007C00U)        /*!<SQ9[4:0] bits (9th conversion in regular sequence) */
#define  ADC_SQR2_SQ9_0                      ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  ADC_SQR2_SQ9_1                      ((uint32_t)0x00000800U)        /*!<Bit 1 */
#define  ADC_SQR2_SQ9_2                      ((uint32_t)0x00001000U)        /*!<Bit 2 */
#define  ADC_SQR2_SQ9_3                      ((uint32_t)0x00002000U)        /*!<Bit 3 */
#define  ADC_SQR2_SQ9_4                      ((uint32_t)0x00004000U)        /*!<Bit 4 */
#define  ADC_SQR2_SQ10                       ((uint32_t)0x000F8000U)        /*!<SQ10[4:0] bits (10th conversion in regular sequence) */
#define  ADC_SQR2_SQ10_0                     ((uint32_t)0x00008000U)        /*!<Bit 0 */
#define  ADC_SQR2_SQ10_1                     ((uint32_t)0x00010000U)        /*!<Bit 1 */
#define  ADC_SQR2_SQ10_2                     ((uint32_t)0x00020000U)        /*!<Bit 2 */
#define  ADC_SQR2_SQ10_3                     ((uint32_t)0x00040000U)        /*!<Bit 3 */
#define  ADC_SQR2_SQ10_4                     ((uint32_t)0x00080000U)        /*!<Bit 4 */
#define  ADC_SQR2_SQ11                       ((uint32_t)0x01F00000U)        /*!<SQ11[4:0] bits (11th conversion in regular sequence) */
#define  ADC_SQR2_SQ11_0                     ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  ADC_SQR2_SQ11_1                     ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  ADC_SQR2_SQ11_2                     ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  ADC_SQR2_SQ11_3                     ((uint32_t)0x00800000U)        /*!<Bit 3 */
#define  ADC_SQR2_SQ11_4                     ((uint32_t)0x01000000U)        /*!<Bit 4 */
#define  ADC_SQR2_SQ12                       ((uint32_t)0x3E000000U)        /*!<SQ12[4:0] bits (12th conversion in regular sequence) */
#define  ADC_SQR2_SQ12_0                     ((uint32_t)0x02000000U)        /*!<Bit 0 */
#define  ADC_SQR2_SQ12_1                     ((uint32_t)0x04000000U)        /*!<Bit 1 */
#define  ADC_SQR2_SQ12_2                     ((uint32_t)0x08000000U)        /*!<Bit 2 */
#define  ADC_SQR2_SQ12_3                     ((uint32_t)0x10000000U)        /*!<Bit 3 */
#define  ADC_SQR2_SQ12_4                     ((uint32_t)0x20000000U)        /*!<Bit 4 */

/*******************  Bit definition for ADC_SQR3 register  *******************/
#define  ADC_SQR3_SQ1                        ((uint32_t)0x0000001FU)        /*!<SQ1[4:0] bits (1st conversion in regular sequence) */
#define  ADC_SQR3_SQ1_0                      ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_SQR3_SQ1_1                      ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_SQR3_SQ1_2                      ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_SQR3_SQ1_3                      ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  ADC_SQR3_SQ1_4                      ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  ADC_SQR3_SQ2                        ((uint32_t)0x000003E0U)        /*!<SQ2[4:0] bits (2nd conversion in regular sequence) */
#define  ADC_SQR3_SQ2_0                      ((uint32_t)0x00000020U)        /*!<Bit 0 */
#define  ADC_SQR3_SQ2_1                      ((uint32_t)0x00000040U)        /*!<Bit 1 */
#define  ADC_SQR3_SQ2_2                      ((uint32_t)0x00000080U)        /*!<Bit 2 */
#define  ADC_SQR3_SQ2_3                      ((uint32_t)0x00000100U)        /*!<Bit 3 */
#define  ADC_SQR3_SQ2_4                      ((uint32_t)0x00000200U)        /*!<Bit 4 */
#define  ADC_SQR3_SQ3                        ((uint32_t)0x00007C00U)        /*!<SQ3[4:0] bits (3rd conversion in regular sequence) */
#define  ADC_SQR3_SQ3_0                      ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  ADC_SQR3_SQ3_1                      ((uint32_t)0x00000800U)        /*!<Bit 1 */
#define  ADC_SQR3_SQ3_2                      ((uint32_t)0x00001000U)        /*!<Bit 2 */
#define  ADC_SQR3_SQ3_3                      ((uint32_t)0x00002000U)        /*!<Bit 3 */
#define  ADC_SQR3_SQ3_4                      ((uint32_t)0x00004000U)        /*!<Bit 4 */
#define  ADC_SQR3_SQ4                        ((uint32_t)0x000F8000U)        /*!<SQ4[4:0] bits (4th conversion in regular sequence) */
#define  ADC_SQR3_SQ4_0                      ((uint32_t)0x00008000U)        /*!<Bit 0 */
#define  ADC_SQR3_SQ4_1                      ((uint32_t)0x00010000U)        /*!<Bit 1 */
#define  ADC_SQR3_SQ4_2                      ((uint32_t)0x00020000U)        /*!<Bit 2 */
#define  ADC_SQR3_SQ4_3                      ((uint32_t)0x00040000U)        /*!<Bit 3 */
#define  ADC_SQR3_SQ4_4                      ((uint32_t)0x00080000U)        /*!<Bit 4 */
#define  ADC_SQR3_SQ5                        ((uint32_t)0x01F00000U)        /*!<SQ5[4:0] bits (5th conversion in regular sequence) */
#define  ADC_SQR3_SQ5_0                      ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  ADC_SQR3_SQ5_1                      ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  ADC_SQR3_SQ5_2                      ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  ADC_SQR3_SQ5_3                      ((uint32_t)0x00800000U)        /*!<Bit 3 */
#define  ADC_SQR3_SQ5_4                      ((uint32_t)0x01000000U)        /*!<Bit 4 */
#define  ADC_SQR3_SQ6                        ((uint32_t)0x3E000000U)        /*!<SQ6[4:0] bits (6th conversion in regular sequence) */
#define  ADC_SQR3_SQ6_0                      ((uint32_t)0x02000000U)        /*!<Bit 0 */
#define  ADC_SQR3_SQ6_1                      ((uint32_t)0x04000000U)        /*!<Bit 1 */
#define  ADC_SQR3_SQ6_2                      ((uint32_t)0x08000000U)        /*!<Bit 2 */
#define  ADC_SQR3_SQ6_3                      ((uint32_t)0x10000000U)        /*!<Bit 3 */
#define  ADC_SQR3_SQ6_4                      ((uint32_t)0x20000000U)        /*!<Bit 4 */

/*******************  Bit definition for ADC_JSQR register  *******************/
#define  ADC_JSQR_JSQ1                       ((uint32_t)0x0000001FU)        /*!<JSQ1[4:0] bits (1st conversion in injected sequence) */
#define  ADC_JSQR_JSQ1_0                     ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ1_1                     ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ1_2                     ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ1_3                     ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ1_4                     ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  ADC_JSQR_JSQ2                       ((uint32_t)0x000003E0U)        /*!<JSQ2[4:0] bits (2nd conversion in injected sequence) */
#define  ADC_JSQR_JSQ2_0                     ((uint32_t)0x00000020U)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ2_1                     ((uint32_t)0x00000040U)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ2_2                     ((uint32_t)0x00000080U)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ2_3                     ((uint32_t)0x00000100U)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ2_4                     ((uint32_t)0x00000200U)        /*!<Bit 4 */
#define  ADC_JSQR_JSQ3                       ((uint32_t)0x00007C00U)        /*!<JSQ3[4:0] bits (3rd conversion in injected sequence) */
#define  ADC_JSQR_JSQ3_0                     ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ3_1                     ((uint32_t)0x00000800U)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ3_2                     ((uint32_t)0x00001000U)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ3_3                     ((uint32_t)0x00002000U)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ3_4                     ((uint32_t)0x00004000U)        /*!<Bit 4 */
#define  ADC_JSQR_JSQ4                       ((uint32_t)0x000F8000U)        /*!<JSQ4[4:0] bits (4th conversion in injected sequence) */
#define  ADC_JSQR_JSQ4_0                     ((uint32_t)0x00008000U)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ4_1                     ((uint32_t)0x00010000U)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ4_2                     ((uint32_t)0x00020000U)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ4_3                     ((uint32_t)0x00040000U)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ4_4                     ((uint32_t)0x00080000U)        /*!<Bit 4 */
#define  ADC_JSQR_JL                         ((uint32_t)0x00300000U)        /*!<JL[1:0] bits (Injected Sequence length) */
#define  ADC_JSQR_JL_0                       ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  ADC_JSQR_JL_1                       ((uint32_t)0x00200000U)        /*!<Bit 1 */

/*******************  Bit definition for ADC_JDR1 register  *******************/
#define  ADC_JDR1_JDATA                      ((uint32_t)0xFFFFU)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR2 register  *******************/
#define  ADC_JDR2_JDATA                      ((uint32_t)0xFFFFU)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR3 register  *******************/
#define  ADC_JDR3_JDATA                      ((uint32_t)0xFFFFU)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR4 register  *******************/
#define  ADC_JDR4_JDATA                      ((uint32_t)0xFFFFU)            /*!<Injected data */

/********************  Bit definition for ADC_DR register  ********************/
#define  ADC_DR_DATA                         ((uint32_t)0x0000FFFFU)        /*!<Regular data */
#define  ADC_DR_ADC2DATA                     ((uint32_t)0xFFFF0000U)        /*!<ADC2 data */

/*******************  Bit definition for ADC_CSR register  ********************/
#define  ADC_CSR_AWD1                        ((uint32_t)0x00000001U)        /*!<ADC1 Analog watchdog flag */
#define  ADC_CSR_EOC1                        ((uint32_t)0x00000002U)        /*!<ADC1 End of conversion */
#define  ADC_CSR_JEOC1                       ((uint32_t)0x00000004U)        /*!<ADC1 Injected channel end of conversion */
#define  ADC_CSR_JSTRT1                      ((uint32_t)0x00000008U)        /*!<ADC1 Injected channel Start flag */
#define  ADC_CSR_STRT1                       ((uint32_t)0x00000010U)        /*!<ADC1 Regular channel Start flag */
#define  ADC_CSR_OVR1                        ((uint32_t)0x00000020U)        /*!<ADC1 DMA overrun  flag */
#define  ADC_CSR_AWD2                        ((uint32_t)0x00000100U)        /*!<ADC2 Analog watchdog flag */
#define  ADC_CSR_EOC2                        ((uint32_t)0x00000200U)        /*!<ADC2 End of conversion */
#define  ADC_CSR_JEOC2                       ((uint32_t)0x00000400U)        /*!<ADC2 Injected channel end of conversion */
#define  ADC_CSR_JSTRT2                      ((uint32_t)0x00000800U)        /*!<ADC2 Injected channel Start flag */
#define  ADC_CSR_STRT2                       ((uint32_t)0x00001000U)        /*!<ADC2 Regular channel Start flag */
#define  ADC_CSR_OVR2                        ((uint32_t)0x00002000U)        /*!<ADC2 DMA overrun  flag */
#define  ADC_CSR_AWD3                        ((uint32_t)0x00010000U)        /*!<ADC3 Analog watchdog flag */
#define  ADC_CSR_EOC3                        ((uint32_t)0x00020000U)        /*!<ADC3 End of conversion */
#define  ADC_CSR_JEOC3                       ((uint32_t)0x00040000U)        /*!<ADC3 Injected channel end of conversion */
#define  ADC_CSR_JSTRT3                      ((uint32_t)0x00080000U)        /*!<ADC3 Injected channel Start flag */
#define  ADC_CSR_STRT3                       ((uint32_t)0x00100000U)        /*!<ADC3 Regular channel Start flag */
#define  ADC_CSR_OVR3                        ((uint32_t)0x00200000U)        /*!<ADC3 DMA overrun  flag */

/* Legacy defines */
#define  ADC_CSR_DOVR1                        ADC_CSR_OVR1
#define  ADC_CSR_DOVR2                        ADC_CSR_OVR2
#define  ADC_CSR_DOVR3                        ADC_CSR_OVR3

/*******************  Bit definition for ADC_CCR register  ********************/
#define  ADC_CCR_MULTI                       ((uint32_t)0x0000001FU)        /*!<MULTI[4:0] bits (Multi-ADC mode selection) */
#define  ADC_CCR_MULTI_0                     ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  ADC_CCR_MULTI_1                     ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  ADC_CCR_MULTI_2                     ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  ADC_CCR_MULTI_3                     ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  ADC_CCR_MULTI_4                     ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  ADC_CCR_DELAY                       ((uint32_t)0x00000F00U)        /*!<DELAY[3:0] bits (Delay between 2 sampling phases) */
#define  ADC_CCR_DELAY_0                     ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  ADC_CCR_DELAY_1                     ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  ADC_CCR_DELAY_2                     ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  ADC_CCR_DELAY_3                     ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  ADC_CCR_DDS                         ((uint32_t)0x00002000U)        /*!<DMA disable selection (Multi-ADC mode) */
#define  ADC_CCR_DMA                         ((uint32_t)0x0000C000U)        /*!<DMA[1:0] bits (Direct Memory Access mode for multimode) */
#define  ADC_CCR_DMA_0                       ((uint32_t)0x00004000U)        /*!<Bit 0 */
#define  ADC_CCR_DMA_1                       ((uint32_t)0x00008000U)        /*!<Bit 1 */
#define  ADC_CCR_ADCPRE                      ((uint32_t)0x00030000U)        /*!<ADCPRE[1:0] bits (ADC prescaler) */
#define  ADC_CCR_ADCPRE_0                    ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  ADC_CCR_ADCPRE_1                    ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  ADC_CCR_VBATE                       ((uint32_t)0x00400000U)        /*!<VBAT Enable */
#define  ADC_CCR_TSVREFE                     ((uint32_t)0x00800000U)        /*!<Temperature Sensor and VREFINT Enable */

/*******************  Bit definition for ADC_CDR register  ********************/
#define  ADC_CDR_DATA1                      ((uint32_t)0x0000FFFFU)         /*!<1st data of a pair of regular conversions */
#define  ADC_CDR_DATA2                      ((uint32_t)0xFFFF0000U)         /*!<2nd data of a pair of regular conversions */

/******************************************************************************/
/*                                                                            */
/*                         Controller Area Network                            */
/*                                                                            */
/******************************************************************************/
/*!<CAN control and status registers */
/*******************  Bit definition for CAN_MCR register  ********************/
#define  CAN_MCR_INRQ                        ((uint32_t)0x00000001U)        /*!<Initialization Request */
#define  CAN_MCR_SLEEP                       ((uint32_t)0x00000002U)        /*!<Sleep Mode Request */
#define  CAN_MCR_TXFP                        ((uint32_t)0x00000004U)        /*!<Transmit FIFO Priority */
#define  CAN_MCR_RFLM                        ((uint32_t)0x00000008U)        /*!<Receive FIFO Locked Mode */
#define  CAN_MCR_NART                        ((uint32_t)0x00000010U)        /*!<No Automatic Retransmission */
#define  CAN_MCR_AWUM                        ((uint32_t)0x00000020U)        /*!<Automatic Wakeup Mode */
#define  CAN_MCR_ABOM                        ((uint32_t)0x00000040U)        /*!<Automatic Bus-Off Management */
#define  CAN_MCR_TTCM                        ((uint32_t)0x00000080U)        /*!<Time Triggered Communication Mode */
#define  CAN_MCR_RESET                       ((uint32_t)0x00008000U)        /*!<bxCAN software master reset */
#define  CAN_MCR_DBF                         ((uint32_t)0x00010000U)        /*!<bxCAN Debug freeze */
/*******************  Bit definition for CAN_MSR register  ********************/
#define  CAN_MSR_INAK                        ((uint32_t)0x0001U)            /*!<Initialization Acknowledge */
#define  CAN_MSR_SLAK                        ((uint32_t)0x0002U)            /*!<Sleep Acknowledge */
#define  CAN_MSR_ERRI                        ((uint32_t)0x0004U)            /*!<Error Interrupt */
#define  CAN_MSR_WKUI                        ((uint32_t)0x0008U)            /*!<Wakeup Interrupt */
#define  CAN_MSR_SLAKI                       ((uint32_t)0x0010U)            /*!<Sleep Acknowledge Interrupt */
#define  CAN_MSR_TXM                         ((uint32_t)0x0100U)            /*!<Transmit Mode */
#define  CAN_MSR_RXM                         ((uint32_t)0x0200U)            /*!<Receive Mode */
#define  CAN_MSR_SAMP                        ((uint32_t)0x0400U)            /*!<Last Sample Point */
#define  CAN_MSR_RX                          ((uint32_t)0x0800U)            /*!<CAN Rx Signal */

/*******************  Bit definition for CAN_TSR register  ********************/
#define  CAN_TSR_RQCP0                       ((uint32_t)0x00000001U)        /*!<Request Completed Mailbox0 */
#define  CAN_TSR_TXOK0                       ((uint32_t)0x00000002U)        /*!<Transmission OK of Mailbox0 */
#define  CAN_TSR_ALST0                       ((uint32_t)0x00000004U)        /*!<Arbitration Lost for Mailbox0 */
#define  CAN_TSR_TERR0                       ((uint32_t)0x00000008U)        /*!<Transmission Error of Mailbox0 */
#define  CAN_TSR_ABRQ0                       ((uint32_t)0x00000080U)        /*!<Abort Request for Mailbox0 */
#define  CAN_TSR_RQCP1                       ((uint32_t)0x00000100U)        /*!<Request Completed Mailbox1 */
#define  CAN_TSR_TXOK1                       ((uint32_t)0x00000200U)        /*!<Transmission OK of Mailbox1 */
#define  CAN_TSR_ALST1                       ((uint32_t)0x00000400U)        /*!<Arbitration Lost for Mailbox1 */
#define  CAN_TSR_TERR1                       ((uint32_t)0x00000800U)        /*!<Transmission Error of Mailbox1 */
#define  CAN_TSR_ABRQ1                       ((uint32_t)0x00008000U)        /*!<Abort Request for Mailbox 1 */
#define  CAN_TSR_RQCP2                       ((uint32_t)0x00010000U)        /*!<Request Completed Mailbox2 */
#define  CAN_TSR_TXOK2                       ((uint32_t)0x00020000U)        /*!<Transmission OK of Mailbox 2 */
#define  CAN_TSR_ALST2                       ((uint32_t)0x00040000U)        /*!<Arbitration Lost for mailbox 2 */
#define  CAN_TSR_TERR2                       ((uint32_t)0x00080000U)        /*!<Transmission Error of Mailbox 2 */
#define  CAN_TSR_ABRQ2                       ((uint32_t)0x00800000U)        /*!<Abort Request for Mailbox 2 */
#define  CAN_TSR_CODE                        ((uint32_t)0x03000000U)        /*!<Mailbox Code */

#define  CAN_TSR_TME                         ((uint32_t)0x1C000000U)        /*!<TME[2:0] bits */
#define  CAN_TSR_TME0                        ((uint32_t)0x04000000U)        /*!<Transmit Mailbox 0 Empty */
#define  CAN_TSR_TME1                        ((uint32_t)0x08000000U)        /*!<Transmit Mailbox 1 Empty */
#define  CAN_TSR_TME2                        ((uint32_t)0x10000000U)        /*!<Transmit Mailbox 2 Empty */

#define  CAN_TSR_LOW                         ((uint32_t)0xE0000000U)        /*!<LOW[2:0] bits */
#define  CAN_TSR_LOW0                        ((uint32_t)0x20000000U)        /*!<Lowest Priority Flag for Mailbox 0 */
#define  CAN_TSR_LOW1                        ((uint32_t)0x40000000U)        /*!<Lowest Priority Flag for Mailbox 1 */
#define  CAN_TSR_LOW2                        ((uint32_t)0x80000000U)        /*!<Lowest Priority Flag for Mailbox 2 */

/*******************  Bit definition for CAN_RF0R register  *******************/
#define  CAN_RF0R_FMP0                       ((uint32_t)0x03U)               /*!<FIFO 0 Message Pending */
#define  CAN_RF0R_FULL0                      ((uint32_t)0x08U)               /*!<FIFO 0 Full */
#define  CAN_RF0R_FOVR0                      ((uint32_t)0x10U)               /*!<FIFO 0 Overrun */
#define  CAN_RF0R_RFOM0                      ((uint32_t)0x20U)               /*!<Release FIFO 0 Output Mailbox */

/*******************  Bit definition for CAN_RF1R register  *******************/
#define  CAN_RF1R_FMP1                       ((uint32_t)0x03U)               /*!<FIFO 1 Message Pending */
#define  CAN_RF1R_FULL1                      ((uint32_t)0x08U)               /*!<FIFO 1 Full */
#define  CAN_RF1R_FOVR1                      ((uint32_t)0x10U)               /*!<FIFO 1 Overrun */
#define  CAN_RF1R_RFOM1                      ((uint32_t)0x20U)               /*!<Release FIFO 1 Output Mailbox */

/********************  Bit definition for CAN_IER register  *******************/
#define  CAN_IER_TMEIE                       ((uint32_t)0x00000001U)        /*!<Transmit Mailbox Empty Interrupt Enable */
#define  CAN_IER_FMPIE0                      ((uint32_t)0x00000002U)        /*!<FIFO Message Pending Interrupt Enable */
#define  CAN_IER_FFIE0                       ((uint32_t)0x00000004U)        /*!<FIFO Full Interrupt Enable */
#define  CAN_IER_FOVIE0                      ((uint32_t)0x00000008U)        /*!<FIFO Overrun Interrupt Enable */
#define  CAN_IER_FMPIE1                      ((uint32_t)0x00000010U)        /*!<FIFO Message Pending Interrupt Enable */
#define  CAN_IER_FFIE1                       ((uint32_t)0x00000020U)        /*!<FIFO Full Interrupt Enable */
#define  CAN_IER_FOVIE1                      ((uint32_t)0x00000040U)        /*!<FIFO Overrun Interrupt Enable */
#define  CAN_IER_EWGIE                       ((uint32_t)0x00000100U)        /*!<Error Warning Interrupt Enable */
#define  CAN_IER_EPVIE                       ((uint32_t)0x00000200U)        /*!<Error Passive Interrupt Enable */
#define  CAN_IER_BOFIE                       ((uint32_t)0x00000400U)        /*!<Bus-Off Interrupt Enable */
#define  CAN_IER_LECIE                       ((uint32_t)0x00000800U)        /*!<Last Error Code Interrupt Enable */
#define  CAN_IER_ERRIE                       ((uint32_t)0x00008000U)        /*!<Error Interrupt Enable */
#define  CAN_IER_WKUIE                       ((uint32_t)0x00010000U)        /*!<Wakeup Interrupt Enable */
#define  CAN_IER_SLKIE                       ((uint32_t)0x00020000U)        /*!<Sleep Interrupt Enable */
#define  CAN_IER_EWGIE                       ((uint32_t)0x00000100U)        /*!<Error warning interrupt enable */
#define  CAN_IER_EPVIE                       ((uint32_t)0x00000200U)        /*!<Error passive interrupt enable */
#define  CAN_IER_BOFIE                       ((uint32_t)0x00000400U)        /*!<Bus-off interrupt enable */
#define  CAN_IER_LECIE                       ((uint32_t)0x00000800U)        /*!<Last error code interrupt enable */
#define  CAN_IER_ERRIE                       ((uint32_t)0x00008000U)        /*!<Error interrupt enable */


/********************  Bit definition for CAN_ESR register  *******************/
#define  CAN_ESR_EWGF                        ((uint32_t)0x00000001U)        /*!<Error Warning Flag */
#define  CAN_ESR_EPVF                        ((uint32_t)0x00000002U)        /*!<Error Passive Flag */
#define  CAN_ESR_BOFF                        ((uint32_t)0x00000004U)        /*!<Bus-Off Flag */

#define  CAN_ESR_LEC                         ((uint32_t)0x00000070U)        /*!<LEC[2:0] bits (Last Error Code) */
#define  CAN_ESR_LEC_0                       ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  CAN_ESR_LEC_1                       ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  CAN_ESR_LEC_2                       ((uint32_t)0x00000040U)        /*!<Bit 2 */

#define  CAN_ESR_TEC                         ((uint32_t)0x00FF0000U)        /*!<Least significant byte of the 9-bit Transmit Error Counter */
#define  CAN_ESR_REC                         ((uint32_t)0xFF000000U)        /*!<Receive Error Counter */

/*******************  Bit definition for CAN_BTR register  ********************/
#define  CAN_BTR_BRP                         ((uint32_t)0x000003FFU)        /*!<Baud Rate Prescaler */
#define  CAN_BTR_TS1                         ((uint32_t)0x000F0000U)        /*!<Time Segment 1 */
#define  CAN_BTR_TS1_0                       ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  CAN_BTR_TS1_1                       ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  CAN_BTR_TS1_2                       ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  CAN_BTR_TS1_3                       ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  CAN_BTR_TS2                         ((uint32_t)0x00700000U)        /*!<Time Segment 2 */
#define  CAN_BTR_TS2_0                       ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  CAN_BTR_TS2_1                       ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  CAN_BTR_TS2_2                       ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  CAN_BTR_SJW                         ((uint32_t)0x03000000U)        /*!<Resynchronization Jump Width */
#define  CAN_BTR_SJW_0                       ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  CAN_BTR_SJW_1                       ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  CAN_BTR_LBKM                        ((uint32_t)0x40000000U)        /*!<Loop Back Mode (Debug) */
#define  CAN_BTR_SILM                        ((uint32_t)0x80000000U)        /*!<Silent Mode */


/*!<Mailbox registers */
/******************  Bit definition for CAN_TI0R register  ********************/
#define  CAN_TI0R_TXRQ                       ((uint32_t)0x00000001U)        /*!<Transmit Mailbox Request */
#define  CAN_TI0R_RTR                        ((uint32_t)0x00000002U)        /*!<Remote Transmission Request */
#define  CAN_TI0R_IDE                        ((uint32_t)0x00000004U)        /*!<Identifier Extension */
#define  CAN_TI0R_EXID                       ((uint32_t)0x001FFFF8U)        /*!<Extended Identifier */
#define  CAN_TI0R_STID                       ((uint32_t)0xFFE00000U)        /*!<Standard Identifier or Extended Identifier */

/******************  Bit definition for CAN_TDT0R register  *******************/
#define  CAN_TDT0R_DLC                       ((uint32_t)0x0000000FU)        /*!<Data Length Code */
#define  CAN_TDT0R_TGT                       ((uint32_t)0x00000100U)        /*!<Transmit Global Time */
#define  CAN_TDT0R_TIME                      ((uint32_t)0xFFFF0000U)        /*!<Message Time Stamp */

/******************  Bit definition for CAN_TDL0R register  *******************/
#define  CAN_TDL0R_DATA0                     ((uint32_t)0x000000FFU)        /*!<Data byte 0 */
#define  CAN_TDL0R_DATA1                     ((uint32_t)0x0000FF00U)        /*!<Data byte 1 */
#define  CAN_TDL0R_DATA2                     ((uint32_t)0x00FF0000U)        /*!<Data byte 2 */
#define  CAN_TDL0R_DATA3                     ((uint32_t)0xFF000000U)        /*!<Data byte 3 */

/******************  Bit definition for CAN_TDH0R register  *******************/
#define  CAN_TDH0R_DATA4                     ((uint32_t)0x000000FFU)        /*!<Data byte 4 */
#define  CAN_TDH0R_DATA5                     ((uint32_t)0x0000FF00U)        /*!<Data byte 5 */
#define  CAN_TDH0R_DATA6                     ((uint32_t)0x00FF0000U)        /*!<Data byte 6 */
#define  CAN_TDH0R_DATA7                     ((uint32_t)0xFF000000U)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_TI1R register  *******************/
#define  CAN_TI1R_TXRQ                       ((uint32_t)0x00000001U)        /*!<Transmit Mailbox Request */
#define  CAN_TI1R_RTR                        ((uint32_t)0x00000002U)        /*!<Remote Transmission Request */
#define  CAN_TI1R_IDE                        ((uint32_t)0x00000004U)        /*!<Identifier Extension */
#define  CAN_TI1R_EXID                       ((uint32_t)0x001FFFF8U)        /*!<Extended Identifier */
#define  CAN_TI1R_STID                       ((uint32_t)0xFFE00000U)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TDT1R register  ******************/
#define  CAN_TDT1R_DLC                       ((uint32_t)0x0000000FU)        /*!<Data Length Code */
#define  CAN_TDT1R_TGT                       ((uint32_t)0x00000100U)        /*!<Transmit Global Time */
#define  CAN_TDT1R_TIME                      ((uint32_t)0xFFFF0000U)        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_TDL1R register  ******************/
#define  CAN_TDL1R_DATA0                     ((uint32_t)0x000000FFU)        /*!<Data byte 0 */
#define  CAN_TDL1R_DATA1                     ((uint32_t)0x0000FF00U)        /*!<Data byte 1 */
#define  CAN_TDL1R_DATA2                     ((uint32_t)0x00FF0000U)        /*!<Data byte 2 */
#define  CAN_TDL1R_DATA3                     ((uint32_t)0xFF000000U)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_TDH1R register  ******************/
#define  CAN_TDH1R_DATA4                     ((uint32_t)0x000000FFU)        /*!<Data byte 4 */
#define  CAN_TDH1R_DATA5                     ((uint32_t)0x0000FF00U)        /*!<Data byte 5 */
#define  CAN_TDH1R_DATA6                     ((uint32_t)0x00FF0000U)        /*!<Data byte 6 */
#define  CAN_TDH1R_DATA7                     ((uint32_t)0xFF000000U)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_TI2R register  *******************/
#define  CAN_TI2R_TXRQ                       ((uint32_t)0x00000001U)        /*!<Transmit Mailbox Request */
#define  CAN_TI2R_RTR                        ((uint32_t)0x00000002U)        /*!<Remote Transmission Request */
#define  CAN_TI2R_IDE                        ((uint32_t)0x00000004U)        /*!<Identifier Extension */
#define  CAN_TI2R_EXID                       ((uint32_t)0x001FFFF8U)        /*!<Extended identifier */
#define  CAN_TI2R_STID                       ((uint32_t)0xFFE00000U)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TDT2R register  ******************/
#define  CAN_TDT2R_DLC                       ((uint32_t)0x0000000FU)        /*!<Data Length Code */
#define  CAN_TDT2R_TGT                       ((uint32_t)0x00000100U)        /*!<Transmit Global Time */
#define  CAN_TDT2R_TIME                      ((uint32_t)0xFFFF0000U)        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_TDL2R register  ******************/
#define  CAN_TDL2R_DATA0                     ((uint32_t)0x000000FFU)        /*!<Data byte 0 */
#define  CAN_TDL2R_DATA1                     ((uint32_t)0x0000FF00U)        /*!<Data byte 1 */
#define  CAN_TDL2R_DATA2                     ((uint32_t)0x00FF0000U)        /*!<Data byte 2 */
#define  CAN_TDL2R_DATA3                     ((uint32_t)0xFF000000U)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_TDH2R register  ******************/
#define  CAN_TDH2R_DATA4                     ((uint32_t)0x000000FFU)        /*!<Data byte 4 */
#define  CAN_TDH2R_DATA5                     ((uint32_t)0x0000FF00U)        /*!<Data byte 5 */
#define  CAN_TDH2R_DATA6                     ((uint32_t)0x00FF0000U)        /*!<Data byte 6 */
#define  CAN_TDH2R_DATA7                     ((uint32_t)0xFF000000U)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_RI0R register  *******************/
#define  CAN_RI0R_RTR                        ((uint32_t)0x00000002U)        /*!<Remote Transmission Request */
#define  CAN_RI0R_IDE                        ((uint32_t)0x00000004U)        /*!<Identifier Extension */
#define  CAN_RI0R_EXID                       ((uint32_t)0x001FFFF8U)        /*!<Extended Identifier */
#define  CAN_RI0R_STID                       ((uint32_t)0xFFE00000U)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RDT0R register  ******************/
#define  CAN_RDT0R_DLC                       ((uint32_t)0x0000000FU)        /*!<Data Length Code */
#define  CAN_RDT0R_FMI                       ((uint32_t)0x0000FF00U)        /*!<Filter Match Index */
#define  CAN_RDT0R_TIME                      ((uint32_t)0xFFFF0000U)        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_RDL0R register  ******************/
#define  CAN_RDL0R_DATA0                     ((uint32_t)0x000000FFU)        /*!<Data byte 0 */
#define  CAN_RDL0R_DATA1                     ((uint32_t)0x0000FF00U)        /*!<Data byte 1 */
#define  CAN_RDL0R_DATA2                     ((uint32_t)0x00FF0000U)        /*!<Data byte 2 */
#define  CAN_RDL0R_DATA3                     ((uint32_t)0xFF000000U)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_RDH0R register  ******************/
#define  CAN_RDH0R_DATA4                     ((uint32_t)0x000000FFU)        /*!<Data byte 4 */
#define  CAN_RDH0R_DATA5                     ((uint32_t)0x0000FF00U)        /*!<Data byte 5 */
#define  CAN_RDH0R_DATA6                     ((uint32_t)0x00FF0000U)        /*!<Data byte 6 */
#define  CAN_RDH0R_DATA7                     ((uint32_t)0xFF000000U)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_RI1R register  *******************/
#define  CAN_RI1R_RTR                        ((uint32_t)0x00000002U)        /*!<Remote Transmission Request */
#define  CAN_RI1R_IDE                        ((uint32_t)0x00000004U)        /*!<Identifier Extension */
#define  CAN_RI1R_EXID                       ((uint32_t)0x001FFFF8U)        /*!<Extended identifier */
#define  CAN_RI1R_STID                       ((uint32_t)0xFFE00000U)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RDT1R register  ******************/
#define  CAN_RDT1R_DLC                       ((uint32_t)0x0000000FU)        /*!<Data Length Code */
#define  CAN_RDT1R_FMI                       ((uint32_t)0x0000FF00U)        /*!<Filter Match Index */
#define  CAN_RDT1R_TIME                      ((uint32_t)0xFFFF0000U)        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_RDL1R register  ******************/
#define  CAN_RDL1R_DATA0                     ((uint32_t)0x000000FFU)        /*!<Data byte 0 */
#define  CAN_RDL1R_DATA1                     ((uint32_t)0x0000FF00U)        /*!<Data byte 1 */
#define  CAN_RDL1R_DATA2                     ((uint32_t)0x00FF0000U)        /*!<Data byte 2 */
#define  CAN_RDL1R_DATA3                     ((uint32_t)0xFF000000U)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_RDH1R register  ******************/
#define  CAN_RDH1R_DATA4                     ((uint32_t)0x000000FFU)        /*!<Data byte 4 */
#define  CAN_RDH1R_DATA5                     ((uint32_t)0x0000FF00U)        /*!<Data byte 5 */
#define  CAN_RDH1R_DATA6                     ((uint32_t)0x00FF0000U)        /*!<Data byte 6 */
#define  CAN_RDH1R_DATA7                     ((uint32_t)0xFF000000U)        /*!<Data byte 7 */

/*!<CAN filter registers */
/*******************  Bit definition for CAN_FMR register  ********************/
#define  CAN_FMR_FINIT                       ((uint32_t)0x01U)               /*!<Filter Init Mode */
#define  CAN_FMR_CAN2SB                      ((uint32_t)0x00003F00U)        /*!<CAN2 start bank */

/*******************  Bit definition for CAN_FM1R register  *******************/
#define  CAN_FM1R_FBM                        ((uint32_t)0x0FFFFFFFU)        /*!<Filter Mode */
#define  CAN_FM1R_FBM0                       ((uint32_t)0x00000001U)        /*!<Filter Init Mode bit 0 */
#define  CAN_FM1R_FBM1                       ((uint32_t)0x00000002U)        /*!<Filter Init Mode bit 1 */
#define  CAN_FM1R_FBM2                       ((uint32_t)0x00000004U)        /*!<Filter Init Mode bit 2 */
#define  CAN_FM1R_FBM3                       ((uint32_t)0x00000008U)        /*!<Filter Init Mode bit 3 */
#define  CAN_FM1R_FBM4                       ((uint32_t)0x00000010U)        /*!<Filter Init Mode bit 4 */
#define  CAN_FM1R_FBM5                       ((uint32_t)0x00000020U)        /*!<Filter Init Mode bit 5 */
#define  CAN_FM1R_FBM6                       ((uint32_t)0x00000040U)        /*!<Filter Init Mode bit 6 */
#define  CAN_FM1R_FBM7                       ((uint32_t)0x00000080U)        /*!<Filter Init Mode bit 7 */
#define  CAN_FM1R_FBM8                       ((uint32_t)0x00000100U)        /*!<Filter Init Mode bit 8 */
#define  CAN_FM1R_FBM9                       ((uint32_t)0x00000200U)        /*!<Filter Init Mode bit 9 */
#define  CAN_FM1R_FBM10                      ((uint32_t)0x00000400U)        /*!<Filter Init Mode bit 10 */
#define  CAN_FM1R_FBM11                      ((uint32_t)0x00000800U)        /*!<Filter Init Mode bit 11 */
#define  CAN_FM1R_FBM12                      ((uint32_t)0x00001000U)        /*!<Filter Init Mode bit 12 */
#define  CAN_FM1R_FBM13                      ((uint32_t)0x00002000U)        /*!<Filter Init Mode bit 13 */
#define  CAN_FM1R_FBM14                      ((uint32_t)0x00004000U)        /*!<Filter Init Mode bit 14 */
#define  CAN_FM1R_FBM15                      ((uint32_t)0x00008000U)        /*!<Filter Init Mode bit 15 */
#define  CAN_FM1R_FBM16                      ((uint32_t)0x00010000U)        /*!<Filter Init Mode bit 16 */
#define  CAN_FM1R_FBM17                      ((uint32_t)0x00020000U)        /*!<Filter Init Mode bit 17 */
#define  CAN_FM1R_FBM18                      ((uint32_t)0x00040000U)        /*!<Filter Init Mode bit 18 */
#define  CAN_FM1R_FBM19                      ((uint32_t)0x00080000U)        /*!<Filter Init Mode bit 19 */
#define  CAN_FM1R_FBM20                      ((uint32_t)0x00100000U)        /*!<Filter Init Mode bit 20 */
#define  CAN_FM1R_FBM21                      ((uint32_t)0x00200000U)        /*!<Filter Init Mode bit 21 */
#define  CAN_FM1R_FBM22                      ((uint32_t)0x00400000U)        /*!<Filter Init Mode bit 22 */
#define  CAN_FM1R_FBM23                      ((uint32_t)0x00800000U)        /*!<Filter Init Mode bit 23 */
#define  CAN_FM1R_FBM24                      ((uint32_t)0x01000000U)        /*!<Filter Init Mode bit 24 */
#define  CAN_FM1R_FBM25                      ((uint32_t)0x02000000U)        /*!<Filter Init Mode bit 25 */
#define  CAN_FM1R_FBM26                      ((uint32_t)0x04000000U)        /*!<Filter Init Mode bit 26 */
#define  CAN_FM1R_FBM27                      ((uint32_t)0x08000000U)        /*!<Filter Init Mode bit 27 */

/*******************  Bit definition for CAN_FS1R register  *******************/
#define  CAN_FS1R_FSC                        ((uint32_t)0x0FFFFFFFU)        /*!<Filter Scale Configuration */
#define  CAN_FS1R_FSC0                       ((uint32_t)0x00000001U)        /*!<Filter Scale Configuration bit 0 */
#define  CAN_FS1R_FSC1                       ((uint32_t)0x00000002U)        /*!<Filter Scale Configuration bit 1 */
#define  CAN_FS1R_FSC2                       ((uint32_t)0x00000004U)        /*!<Filter Scale Configuration bit 2 */
#define  CAN_FS1R_FSC3                       ((uint32_t)0x00000008U)        /*!<Filter Scale Configuration bit 3 */
#define  CAN_FS1R_FSC4                       ((uint32_t)0x00000010U)        /*!<Filter Scale Configuration bit 4 */
#define  CAN_FS1R_FSC5                       ((uint32_t)0x00000020U)        /*!<Filter Scale Configuration bit 5 */
#define  CAN_FS1R_FSC6                       ((uint32_t)0x00000040U)        /*!<Filter Scale Configuration bit 6 */
#define  CAN_FS1R_FSC7                       ((uint32_t)0x00000080U)        /*!<Filter Scale Configuration bit 7 */
#define  CAN_FS1R_FSC8                       ((uint32_t)0x00000100U)        /*!<Filter Scale Configuration bit 8 */
#define  CAN_FS1R_FSC9                       ((uint32_t)0x00000200U)        /*!<Filter Scale Configuration bit 9 */
#define  CAN_FS1R_FSC10                      ((uint32_t)0x00000400U)        /*!<Filter Scale Configuration bit 10 */
#define  CAN_FS1R_FSC11                      ((uint32_t)0x00000800U)        /*!<Filter Scale Configuration bit 11 */
#define  CAN_FS1R_FSC12                      ((uint32_t)0x00001000U)        /*!<Filter Scale Configuration bit 12 */
#define  CAN_FS1R_FSC13                      ((uint32_t)0x00002000U)        /*!<Filter Scale Configuration bit 13 */
#define  CAN_FS1R_FSC14                      ((uint32_t)0x00004000U)        /*!<Filter Scale Configuration bit 14 */
#define  CAN_FS1R_FSC15                      ((uint32_t)0x00008000U)        /*!<Filter Scale Configuration bit 15 */
#define  CAN_FS1R_FSC16                      ((uint32_t)0x00010000U)        /*!<Filter Scale Configuration bit 16 */
#define  CAN_FS1R_FSC17                      ((uint32_t)0x00020000U)        /*!<Filter Scale Configuration bit 17 */
#define  CAN_FS1R_FSC18                      ((uint32_t)0x00040000U)        /*!<Filter Scale Configuration bit 18 */
#define  CAN_FS1R_FSC19                      ((uint32_t)0x00080000U)        /*!<Filter Scale Configuration bit 19 */
#define  CAN_FS1R_FSC20                      ((uint32_t)0x00100000U)        /*!<Filter Scale Configuration bit 20 */
#define  CAN_FS1R_FSC21                      ((uint32_t)0x00200000U)        /*!<Filter Scale Configuration bit 21 */
#define  CAN_FS1R_FSC22                      ((uint32_t)0x00400000U)        /*!<Filter Scale Configuration bit 22 */
#define  CAN_FS1R_FSC23                      ((uint32_t)0x00800000U)        /*!<Filter Scale Configuration bit 23 */
#define  CAN_FS1R_FSC24                      ((uint32_t)0x01000000U)        /*!<Filter Scale Configuration bit 24 */
#define  CAN_FS1R_FSC25                      ((uint32_t)0x02000000U)        /*!<Filter Scale Configuration bit 25 */
#define  CAN_FS1R_FSC26                      ((uint32_t)0x04000000U)        /*!<Filter Scale Configuration bit 26 */
#define  CAN_FS1R_FSC27                      ((uint32_t)0x08000000U)        /*!<Filter Scale Configuration bit 27 */

/******************  Bit definition for CAN_FFA1R register  *******************/
#define  CAN_FFA1R_FFA                        ((uint32_t)0x0FFFFFFFU)        /*!<Filter FIFO Assignment */
#define  CAN_FFA1R_FFA0                       ((uint32_t)0x00000001U)        /*!<Filter FIFO Assignment bit 0 */
#define  CAN_FFA1R_FFA1                       ((uint32_t)0x00000002U)        /*!<Filter FIFO Assignment bit 1 */
#define  CAN_FFA1R_FFA2                       ((uint32_t)0x00000004U)        /*!<Filter FIFO Assignment bit 2 */
#define  CAN_FFA1R_FFA3                       ((uint32_t)0x00000008U)        /*!<Filter FIFO Assignment bit 3 */
#define  CAN_FFA1R_FFA4                       ((uint32_t)0x00000010U)        /*!<Filter FIFO Assignment bit 4 */
#define  CAN_FFA1R_FFA5                       ((uint32_t)0x00000020U)        /*!<Filter FIFO Assignment bit 5 */
#define  CAN_FFA1R_FFA6                       ((uint32_t)0x00000040U)        /*!<Filter FIFO Assignment bit 6 */
#define  CAN_FFA1R_FFA7                       ((uint32_t)0x00000080U)        /*!<Filter FIFO Assignment bit 7 */
#define  CAN_FFA1R_FFA8                       ((uint32_t)0x00000100U)        /*!<Filter FIFO Assignment bit 8 */
#define  CAN_FFA1R_FFA9                       ((uint32_t)0x00000200U)        /*!<Filter FIFO Assignment bit 9 */
#define  CAN_FFA1R_FFA10                      ((uint32_t)0x00000400U)        /*!<Filter FIFO Assignment bit 10 */
#define  CAN_FFA1R_FFA11                      ((uint32_t)0x00000800U)        /*!<Filter FIFO Assignment bit 11 */
#define  CAN_FFA1R_FFA12                      ((uint32_t)0x00001000U)        /*!<Filter FIFO Assignment bit 12 */
#define  CAN_FFA1R_FFA13                      ((uint32_t)0x00002000U)        /*!<Filter FIFO Assignment bit 13 */
#define  CAN_FFA1R_FFA14                      ((uint32_t)0x00004000U)        /*!<Filter FIFO Assignment bit 14 */
#define  CAN_FFA1R_FFA15                      ((uint32_t)0x00008000U)        /*!<Filter FIFO Assignment bit 15 */
#define  CAN_FFA1R_FFA16                      ((uint32_t)0x00010000U)        /*!<Filter FIFO Assignment bit 16 */
#define  CAN_FFA1R_FFA17                      ((uint32_t)0x00020000U)        /*!<Filter FIFO Assignment bit 17 */
#define  CAN_FFA1R_FFA18                      ((uint32_t)0x00040000U)        /*!<Filter FIFO Assignment bit 18 */
#define  CAN_FFA1R_FFA19                      ((uint32_t)0x00080000U)        /*!<Filter FIFO Assignment bit 19 */
#define  CAN_FFA1R_FFA20                      ((uint32_t)0x00100000U)        /*!<Filter FIFO Assignment bit 20 */
#define  CAN_FFA1R_FFA21                      ((uint32_t)0x00200000U)        /*!<Filter FIFO Assignment bit 21 */
#define  CAN_FFA1R_FFA22                      ((uint32_t)0x00400000U)        /*!<Filter FIFO Assignment bit 22 */
#define  CAN_FFA1R_FFA23                      ((uint32_t)0x00800000U)        /*!<Filter FIFO Assignment bit 23 */
#define  CAN_FFA1R_FFA24                      ((uint32_t)0x01000000U)        /*!<Filter FIFO Assignment bit 24 */
#define  CAN_FFA1R_FFA25                      ((uint32_t)0x02000000U)        /*!<Filter FIFO Assignment bit 25 */
#define  CAN_FFA1R_FFA26                      ((uint32_t)0x04000000U)        /*!<Filter FIFO Assignment bit 26 */
#define  CAN_FFA1R_FFA27                      ((uint32_t)0x08000000U)        /*!<Filter FIFO Assignment bit 27 */

/*******************  Bit definition for CAN_FA1R register  *******************/
#define  CAN_FA1R_FACT                        ((uint32_t)0x0FFFFFFFU)        /*!<Filter Active */
#define  CAN_FA1R_FACT0                       ((uint32_t)0x00000001U)        /*!<Filter Active bit 0 */
#define  CAN_FA1R_FACT1                       ((uint32_t)0x00000002U)        /*!<Filter Active bit 1 */
#define  CAN_FA1R_FACT2                       ((uint32_t)0x00000004U)        /*!<Filter Active bit 2 */
#define  CAN_FA1R_FACT3                       ((uint32_t)0x00000008U)        /*!<Filter Active bit 3 */
#define  CAN_FA1R_FACT4                       ((uint32_t)0x00000010U)        /*!<Filter Active bit 4 */
#define  CAN_FA1R_FACT5                       ((uint32_t)0x00000020U)        /*!<Filter Active bit 5 */
#define  CAN_FA1R_FACT6                       ((uint32_t)0x00000040U)        /*!<Filter Active bit 6 */
#define  CAN_FA1R_FACT7                       ((uint32_t)0x00000080U)        /*!<Filter Active bit 7 */
#define  CAN_FA1R_FACT8                       ((uint32_t)0x00000100U)        /*!<Filter Active bit 8 */
#define  CAN_FA1R_FACT9                       ((uint32_t)0x00000200U)        /*!<Filter Active bit 9 */
#define  CAN_FA1R_FACT10                      ((uint32_t)0x00000400U)        /*!<Filter Active bit 10 */
#define  CAN_FA1R_FACT11                      ((uint32_t)0x00000800U)        /*!<Filter Active bit 11 */
#define  CAN_FA1R_FACT12                      ((uint32_t)0x00001000U)        /*!<Filter Active bit 12 */
#define  CAN_FA1R_FACT13                      ((uint32_t)0x00002000U)        /*!<Filter Active bit 13 */
#define  CAN_FA1R_FACT14                      ((uint32_t)0x00004000U)        /*!<Filter Active bit 14 */
#define  CAN_FA1R_FACT15                      ((uint32_t)0x00008000U)        /*!<Filter Active bit 15 */
#define  CAN_FA1R_FACT16                      ((uint32_t)0x00010000U)        /*!<Filter Active bit 16 */
#define  CAN_FA1R_FACT17                      ((uint32_t)0x00020000U)        /*!<Filter Active bit 17 */
#define  CAN_FA1R_FACT18                      ((uint32_t)0x00040000U)        /*!<Filter Active bit 18 */
#define  CAN_FA1R_FACT19                      ((uint32_t)0x00080000U)        /*!<Filter Active bit 19 */
#define  CAN_FA1R_FACT20                      ((uint32_t)0x00100000U)        /*!<Filter Active bit 20 */
#define  CAN_FA1R_FACT21                      ((uint32_t)0x00200000U)        /*!<Filter Active bit 21 */
#define  CAN_FA1R_FACT22                      ((uint32_t)0x00400000U)        /*!<Filter Active bit 22 */
#define  CAN_FA1R_FACT23                      ((uint32_t)0x00800000U)        /*!<Filter Active bit 23 */
#define  CAN_FA1R_FACT24                      ((uint32_t)0x01000000U)        /*!<Filter Active bit 24 */
#define  CAN_FA1R_FACT25                      ((uint32_t)0x02000000U)        /*!<Filter Active bit 25 */
#define  CAN_FA1R_FACT26                      ((uint32_t)0x04000000U)        /*!<Filter Active bit 26 */
#define  CAN_FA1R_FACT27                      ((uint32_t)0x08000000U)        /*!<Filter Active bit 27 */

/*******************  Bit definition for CAN_F0R1 register  *******************/
#define  CAN_F0R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F0R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F0R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F0R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F0R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F0R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F0R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F0R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F0R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F0R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F0R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F0R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F0R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F0R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F0R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F0R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F0R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F0R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F0R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F0R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F0R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F0R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F0R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F0R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F0R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F0R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F0R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F0R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F0R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F0R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F0R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F0R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R1 register  *******************/
#define  CAN_F1R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F1R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F1R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F1R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F1R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F1R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F1R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F1R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F1R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F1R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F1R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F1R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F1R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F1R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F1R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F1R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F1R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F1R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F1R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F1R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F1R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F1R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F1R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F1R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F1R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F1R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F1R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F1R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F1R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F1R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F1R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F1R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R1 register  *******************/
#define  CAN_F2R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F2R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F2R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F2R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F2R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F2R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F2R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F2R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F2R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F2R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F2R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F2R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F2R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F2R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F2R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F2R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F2R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F2R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F2R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F2R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F2R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F2R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F2R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F2R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F2R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F2R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F2R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F2R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F2R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F2R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F2R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F2R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R1 register  *******************/
#define  CAN_F3R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F3R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F3R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F3R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F3R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F3R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F3R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F3R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F3R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F3R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F3R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F3R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F3R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F3R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F3R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F3R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F3R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F3R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F3R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F3R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F3R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F3R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F3R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F3R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F3R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F3R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F3R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F3R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F3R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F3R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F3R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F3R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R1 register  *******************/
#define  CAN_F4R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F4R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F4R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F4R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F4R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F4R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F4R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F4R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F4R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F4R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F4R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F4R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F4R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F4R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F4R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F4R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F4R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F4R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F4R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F4R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F4R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F4R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F4R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F4R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F4R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F4R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F4R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F4R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F4R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F4R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F4R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F4R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R1 register  *******************/
#define  CAN_F5R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F5R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F5R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F5R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F5R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F5R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F5R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F5R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F5R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F5R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F5R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F5R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F5R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F5R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F5R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F5R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F5R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F5R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F5R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F5R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F5R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F5R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F5R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F5R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F5R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F5R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F5R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F5R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F5R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F5R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F5R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F5R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R1 register  *******************/
#define  CAN_F6R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F6R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F6R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F6R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F6R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F6R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F6R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F6R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F6R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F6R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F6R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F6R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F6R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F6R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F6R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F6R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F6R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F6R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F6R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F6R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F6R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F6R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F6R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F6R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F6R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F6R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F6R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F6R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F6R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F6R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F6R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F6R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R1 register  *******************/
#define  CAN_F7R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F7R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F7R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F7R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F7R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F7R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F7R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F7R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F7R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F7R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F7R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F7R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F7R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F7R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F7R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F7R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F7R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F7R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F7R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F7R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F7R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F7R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F7R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F7R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F7R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F7R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F7R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F7R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F7R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F7R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F7R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F7R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R1 register  *******************/
#define  CAN_F8R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F8R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F8R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F8R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F8R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F8R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F8R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F8R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F8R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F8R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F8R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F8R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F8R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F8R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F8R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F8R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F8R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F8R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F8R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F8R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F8R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F8R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F8R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F8R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F8R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F8R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F8R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F8R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F8R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F8R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F8R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F8R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R1 register  *******************/
#define  CAN_F9R1_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F9R1_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F9R1_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F9R1_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F9R1_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F9R1_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F9R1_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F9R1_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F9R1_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F9R1_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F9R1_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F9R1_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F9R1_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F9R1_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F9R1_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F9R1_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F9R1_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F9R1_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F9R1_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F9R1_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F9R1_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F9R1_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F9R1_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F9R1_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F9R1_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F9R1_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F9R1_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F9R1_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F9R1_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F9R1_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F9R1_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F9R1_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R1 register  ******************/
#define  CAN_F10R1_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F10R1_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F10R1_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F10R1_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F10R1_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F10R1_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F10R1_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F10R1_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F10R1_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F10R1_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F10R1_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F10R1_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F10R1_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F10R1_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F10R1_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F10R1_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F10R1_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F10R1_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F10R1_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F10R1_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F10R1_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F10R1_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F10R1_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F10R1_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F10R1_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F10R1_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F10R1_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F10R1_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F10R1_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F10R1_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F10R1_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F10R1_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R1 register  ******************/
#define  CAN_F11R1_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F11R1_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F11R1_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F11R1_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F11R1_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F11R1_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F11R1_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F11R1_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F11R1_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F11R1_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F11R1_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F11R1_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F11R1_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F11R1_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F11R1_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F11R1_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F11R1_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F11R1_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F11R1_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F11R1_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F11R1_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F11R1_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F11R1_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F11R1_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F11R1_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F11R1_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F11R1_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F11R1_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F11R1_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F11R1_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F11R1_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F11R1_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R1 register  ******************/
#define  CAN_F12R1_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F12R1_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F12R1_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F12R1_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F12R1_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F12R1_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F12R1_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F12R1_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F12R1_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F12R1_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F12R1_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F12R1_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F12R1_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F12R1_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F12R1_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F12R1_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F12R1_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F12R1_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F12R1_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F12R1_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F12R1_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F12R1_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F12R1_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F12R1_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F12R1_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F12R1_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F12R1_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F12R1_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F12R1_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F12R1_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F12R1_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F12R1_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R1 register  ******************/
#define  CAN_F13R1_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F13R1_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F13R1_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F13R1_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F13R1_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F13R1_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F13R1_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F13R1_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F13R1_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F13R1_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F13R1_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F13R1_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F13R1_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F13R1_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F13R1_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F13R1_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F13R1_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F13R1_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F13R1_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F13R1_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F13R1_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F13R1_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F13R1_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F13R1_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F13R1_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F13R1_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F13R1_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F13R1_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F13R1_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F13R1_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F13R1_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F13R1_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F0R2 register  *******************/
#define  CAN_F0R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F0R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F0R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F0R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F0R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F0R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F0R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F0R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F0R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F0R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F0R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F0R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F0R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F0R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F0R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F0R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F0R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F0R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F0R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F0R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F0R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F0R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F0R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F0R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F0R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F0R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F0R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F0R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F0R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F0R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F0R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F0R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R2 register  *******************/
#define  CAN_F1R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F1R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F1R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F1R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F1R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F1R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F1R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F1R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F1R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F1R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F1R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F1R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F1R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F1R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F1R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F1R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F1R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F1R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F1R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F1R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F1R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F1R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F1R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F1R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F1R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F1R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F1R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F1R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F1R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F1R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F1R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F1R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R2 register  *******************/
#define  CAN_F2R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F2R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F2R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F2R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F2R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F2R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F2R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F2R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F2R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F2R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F2R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F2R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F2R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F2R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F2R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F2R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F2R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F2R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F2R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F2R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F2R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F2R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F2R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F2R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F2R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F2R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F2R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F2R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F2R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F2R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F2R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F2R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R2 register  *******************/
#define  CAN_F3R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F3R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F3R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F3R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F3R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F3R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F3R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F3R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F3R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F3R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F3R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F3R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F3R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F3R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F3R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F3R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F3R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F3R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F3R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F3R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F3R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F3R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F3R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F3R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F3R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F3R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F3R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F3R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F3R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F3R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F3R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F3R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R2 register  *******************/
#define  CAN_F4R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F4R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F4R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F4R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F4R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F4R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F4R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F4R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F4R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F4R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F4R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F4R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F4R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F4R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F4R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F4R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F4R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F4R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F4R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F4R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F4R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F4R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F4R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F4R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F4R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F4R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F4R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F4R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F4R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F4R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F4R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F4R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R2 register  *******************/
#define  CAN_F5R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F5R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F5R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F5R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F5R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F5R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F5R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F5R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F5R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F5R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F5R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F5R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F5R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F5R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F5R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F5R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F5R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F5R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F5R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F5R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F5R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F5R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F5R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F5R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F5R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F5R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F5R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F5R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F5R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F5R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F5R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F5R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R2 register  *******************/
#define  CAN_F6R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F6R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F6R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F6R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F6R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F6R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F6R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F6R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F6R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F6R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F6R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F6R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F6R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F6R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F6R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F6R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F6R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F6R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F6R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F6R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F6R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F6R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F6R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F6R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F6R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F6R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F6R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F6R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F6R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F6R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F6R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F6R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R2 register  *******************/
#define  CAN_F7R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F7R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F7R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F7R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F7R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F7R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F7R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F7R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F7R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F7R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F7R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F7R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F7R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F7R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F7R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F7R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F7R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F7R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F7R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F7R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F7R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F7R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F7R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F7R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F7R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F7R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F7R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F7R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F7R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F7R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F7R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F7R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R2 register  *******************/
#define  CAN_F8R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F8R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F8R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F8R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F8R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F8R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F8R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F8R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F8R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F8R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F8R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F8R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F8R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F8R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F8R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F8R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F8R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F8R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F8R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F8R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F8R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F8R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F8R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F8R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F8R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F8R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F8R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F8R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F8R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F8R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F8R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F8R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R2 register  *******************/
#define  CAN_F9R2_FB0                        ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F9R2_FB1                        ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F9R2_FB2                        ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F9R2_FB3                        ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F9R2_FB4                        ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F9R2_FB5                        ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F9R2_FB6                        ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F9R2_FB7                        ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F9R2_FB8                        ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F9R2_FB9                        ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F9R2_FB10                       ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F9R2_FB11                       ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F9R2_FB12                       ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F9R2_FB13                       ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F9R2_FB14                       ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F9R2_FB15                       ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F9R2_FB16                       ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F9R2_FB17                       ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F9R2_FB18                       ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F9R2_FB19                       ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F9R2_FB20                       ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F9R2_FB21                       ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F9R2_FB22                       ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F9R2_FB23                       ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F9R2_FB24                       ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F9R2_FB25                       ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F9R2_FB26                       ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F9R2_FB27                       ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F9R2_FB28                       ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F9R2_FB29                       ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F9R2_FB30                       ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F9R2_FB31                       ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R2 register  ******************/
#define  CAN_F10R2_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F10R2_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F10R2_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F10R2_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F10R2_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F10R2_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F10R2_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F10R2_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F10R2_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F10R2_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F10R2_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F10R2_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F10R2_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F10R2_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F10R2_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F10R2_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F10R2_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F10R2_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F10R2_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F10R2_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F10R2_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F10R2_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F10R2_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F10R2_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F10R2_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F10R2_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F10R2_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F10R2_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F10R2_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F10R2_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F10R2_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F10R2_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R2 register  ******************/
#define  CAN_F11R2_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F11R2_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F11R2_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F11R2_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F11R2_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F11R2_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F11R2_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F11R2_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F11R2_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F11R2_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F11R2_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F11R2_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F11R2_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F11R2_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F11R2_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F11R2_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F11R2_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F11R2_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F11R2_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F11R2_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F11R2_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F11R2_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F11R2_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F11R2_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F11R2_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F11R2_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F11R2_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F11R2_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F11R2_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F11R2_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F11R2_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F11R2_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R2 register  ******************/
#define  CAN_F12R2_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F12R2_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F12R2_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F12R2_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F12R2_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F12R2_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F12R2_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F12R2_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F12R2_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F12R2_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F12R2_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F12R2_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F12R2_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F12R2_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F12R2_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F12R2_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F12R2_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F12R2_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F12R2_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F12R2_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F12R2_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F12R2_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F12R2_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F12R2_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F12R2_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F12R2_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F12R2_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F12R2_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F12R2_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F12R2_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F12R2_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F12R2_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R2 register  ******************/
#define  CAN_F13R2_FB0                       ((uint32_t)0x00000001U)        /*!<Filter bit 0 */
#define  CAN_F13R2_FB1                       ((uint32_t)0x00000002U)        /*!<Filter bit 1 */
#define  CAN_F13R2_FB2                       ((uint32_t)0x00000004U)        /*!<Filter bit 2 */
#define  CAN_F13R2_FB3                       ((uint32_t)0x00000008U)        /*!<Filter bit 3 */
#define  CAN_F13R2_FB4                       ((uint32_t)0x00000010U)        /*!<Filter bit 4 */
#define  CAN_F13R2_FB5                       ((uint32_t)0x00000020U)        /*!<Filter bit 5 */
#define  CAN_F13R2_FB6                       ((uint32_t)0x00000040U)        /*!<Filter bit 6 */
#define  CAN_F13R2_FB7                       ((uint32_t)0x00000080U)        /*!<Filter bit 7 */
#define  CAN_F13R2_FB8                       ((uint32_t)0x00000100U)        /*!<Filter bit 8 */
#define  CAN_F13R2_FB9                       ((uint32_t)0x00000200U)        /*!<Filter bit 9 */
#define  CAN_F13R2_FB10                      ((uint32_t)0x00000400U)        /*!<Filter bit 10 */
#define  CAN_F13R2_FB11                      ((uint32_t)0x00000800U)        /*!<Filter bit 11 */
#define  CAN_F13R2_FB12                      ((uint32_t)0x00001000U)        /*!<Filter bit 12 */
#define  CAN_F13R2_FB13                      ((uint32_t)0x00002000U)        /*!<Filter bit 13 */
#define  CAN_F13R2_FB14                      ((uint32_t)0x00004000U)        /*!<Filter bit 14 */
#define  CAN_F13R2_FB15                      ((uint32_t)0x00008000U)        /*!<Filter bit 15 */
#define  CAN_F13R2_FB16                      ((uint32_t)0x00010000U)        /*!<Filter bit 16 */
#define  CAN_F13R2_FB17                      ((uint32_t)0x00020000U)        /*!<Filter bit 17 */
#define  CAN_F13R2_FB18                      ((uint32_t)0x00040000U)        /*!<Filter bit 18 */
#define  CAN_F13R2_FB19                      ((uint32_t)0x00080000U)        /*!<Filter bit 19 */
#define  CAN_F13R2_FB20                      ((uint32_t)0x00100000U)        /*!<Filter bit 20 */
#define  CAN_F13R2_FB21                      ((uint32_t)0x00200000U)        /*!<Filter bit 21 */
#define  CAN_F13R2_FB22                      ((uint32_t)0x00400000U)        /*!<Filter bit 22 */
#define  CAN_F13R2_FB23                      ((uint32_t)0x00800000U)        /*!<Filter bit 23 */
#define  CAN_F13R2_FB24                      ((uint32_t)0x01000000U)        /*!<Filter bit 24 */
#define  CAN_F13R2_FB25                      ((uint32_t)0x02000000U)        /*!<Filter bit 25 */
#define  CAN_F13R2_FB26                      ((uint32_t)0x04000000U)        /*!<Filter bit 26 */
#define  CAN_F13R2_FB27                      ((uint32_t)0x08000000U)        /*!<Filter bit 27 */
#define  CAN_F13R2_FB28                      ((uint32_t)0x10000000U)        /*!<Filter bit 28 */
#define  CAN_F13R2_FB29                      ((uint32_t)0x20000000U)        /*!<Filter bit 29 */
#define  CAN_F13R2_FB30                      ((uint32_t)0x40000000U)        /*!<Filter bit 30 */
#define  CAN_F13R2_FB31                      ((uint32_t)0x80000000U)        /*!<Filter bit 31 */

/******************************************************************************/
/*                                                                            */
/*                          CRC calculation unit                              */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for CRC_DR register  *********************/
#define  CRC_DR_DR                           ((uint32_t)0xFFFFFFFFU) /*!< Data register bits */


/*******************  Bit definition for CRC_IDR register  ********************/
#define  CRC_IDR_IDR                         ((uint32_t)0xFFU)        /*!< General-purpose 8-bit data register bits */


/********************  Bit definition for CRC_CR register  ********************/
#define  CRC_CR_RESET                        ((uint32_t)0x01U)        /*!< RESET bit */

/******************************************************************************/
/*                                                                            */
/*                      Digital to Analog Converter                           */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DAC_CR register  ********************/
#define  DAC_CR_EN1                          ((uint32_t)0x00000001U)        /*!<DAC channel1 enable */
#define  DAC_CR_BOFF1                        ((uint32_t)0x00000002U)        /*!<DAC channel1 output buffer disable */
#define  DAC_CR_TEN1                         ((uint32_t)0x00000004U)        /*!<DAC channel1 Trigger enable */

#define  DAC_CR_TSEL1                        ((uint32_t)0x00000038U)        /*!<TSEL1[2:0] (DAC channel1 Trigger selection) */
#define  DAC_CR_TSEL1_0                      ((uint32_t)0x00000008U)        /*!<Bit 0 */
#define  DAC_CR_TSEL1_1                      ((uint32_t)0x00000010U)        /*!<Bit 1 */
#define  DAC_CR_TSEL1_2                      ((uint32_t)0x00000020U)        /*!<Bit 2 */

#define  DAC_CR_WAVE1                        ((uint32_t)0x000000C0U)        /*!<WAVE1[1:0] (DAC channel1 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE1_0                      ((uint32_t)0x00000040U)        /*!<Bit 0 */
#define  DAC_CR_WAVE1_1                      ((uint32_t)0x00000080U)        /*!<Bit 1 */

#define  DAC_CR_MAMP1                        ((uint32_t)0x00000F00U)        /*!<MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define  DAC_CR_MAMP1_0                      ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  DAC_CR_MAMP1_1                      ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  DAC_CR_MAMP1_2                      ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  DAC_CR_MAMP1_3                      ((uint32_t)0x00000800U)        /*!<Bit 3 */

#define  DAC_CR_DMAEN1                       ((uint32_t)0x00001000U)        /*!<DAC channel1 DMA enable */
#define  DAC_CR_DMAUDRIE1                    ((uint32_t)0x00002000U)        /*!<DAC channel1 DMA underrun interrupt enable*/
#define  DAC_CR_EN2                          ((uint32_t)0x00010000U)        /*!<DAC channel2 enable */
#define  DAC_CR_BOFF2                        ((uint32_t)0x00020000U)        /*!<DAC channel2 output buffer disable */
#define  DAC_CR_TEN2                         ((uint32_t)0x00040000U)        /*!<DAC channel2 Trigger enable */

#define  DAC_CR_TSEL2                        ((uint32_t)0x00380000U)        /*!<TSEL2[2:0] (DAC channel2 Trigger selection) */
#define  DAC_CR_TSEL2_0                      ((uint32_t)0x00080000U)        /*!<Bit 0 */
#define  DAC_CR_TSEL2_1                      ((uint32_t)0x00100000U)        /*!<Bit 1 */
#define  DAC_CR_TSEL2_2                      ((uint32_t)0x00200000U)        /*!<Bit 2 */

#define  DAC_CR_WAVE2                        ((uint32_t)0x00C00000U)        /*!<WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE2_0                      ((uint32_t)0x00400000U)        /*!<Bit 0 */
#define  DAC_CR_WAVE2_1                      ((uint32_t)0x00800000U)        /*!<Bit 1 */

#define  DAC_CR_MAMP2                        ((uint32_t)0x0F000000U)        /*!<MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define  DAC_CR_MAMP2_0                      ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  DAC_CR_MAMP2_1                      ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  DAC_CR_MAMP2_2                      ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  DAC_CR_MAMP2_3                      ((uint32_t)0x08000000U)        /*!<Bit 3 */

#define  DAC_CR_DMAEN2                       ((uint32_t)0x10000000U)        /*!<DAC channel2 DMA enabled */
#define  DAC_CR_DMAUDRIE2                    ((uint32_t)0x20000000U)        /*!<DAC channel2 DMA underrun interrupt enable*/

/*****************  Bit definition for DAC_SWTRIGR register  ******************/
#define  DAC_SWTRIGR_SWTRIG1                 ((uint32_t)0x01U)               /*!<DAC channel1 software trigger */
#define  DAC_SWTRIGR_SWTRIG2                 ((uint32_t)0x02U)               /*!<DAC channel2 software trigger */

/*****************  Bit definition for DAC_DHR12R1 register  ******************/
#define  DAC_DHR12R1_DACC1DHR                ((uint32_t)0x0FFFU)            /*!<DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L1 register  ******************/
#define  DAC_DHR12L1_DACC1DHR                ((uint32_t)0xFFF0U)            /*!<DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R1 register  ******************/
#define  DAC_DHR8R1_DACC1DHR                 ((uint32_t)0xFFU)               /*!<DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12R2 register  ******************/
#define  DAC_DHR12R2_DACC2DHR                ((uint32_t)0x0FFFU)            /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L2 register  ******************/
#define  DAC_DHR12L2_DACC2DHR                ((uint32_t)0xFFF0U)            /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R2 register  ******************/
#define  DAC_DHR8R2_DACC2DHR                 ((uint32_t)0xFFU)               /*!<DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12RD register  ******************/
#define  DAC_DHR12RD_DACC1DHR                ((uint32_t)0x00000FFFU)        /*!<DAC channel1 12-bit Right aligned data */
#define  DAC_DHR12RD_DACC2DHR                ((uint32_t)0x0FFF0000U)        /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12LD register  ******************/
#define  DAC_DHR12LD_DACC1DHR                ((uint32_t)0x0000FFF0U)        /*!<DAC channel1 12-bit Left aligned data */
#define  DAC_DHR12LD_DACC2DHR                ((uint32_t)0xFFF00000U)        /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8RD register  ******************/
#define  DAC_DHR8RD_DACC1DHR                 ((uint32_t)0x00FFU)            /*!<DAC channel1 8-bit Right aligned data */
#define  DAC_DHR8RD_DACC2DHR                 ((uint32_t)0xFF00U)            /*!<DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define  DAC_DOR1_DACC1DOR                   ((uint32_t)0x0FFFU)            /*!<DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define  DAC_DOR2_DACC2DOR                   ((uint32_t)0x0FFFU)            /*!<DAC channel2 data output */

/********************  Bit definition for DAC_SR register  ********************/
#define  DAC_SR_DMAUDR1                      ((uint32_t)0x00002000U)        /*!<DAC channel1 DMA underrun flag */
#define  DAC_SR_DMAUDR2                      ((uint32_t)0x20000000U)        /*!<DAC channel2 DMA underrun flag */

/******************************************************************************/
/*                                                                            */
/*                                 Debug MCU                                  */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                    DCMI                                    */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DCMI_CR register  ******************/
#define DCMI_CR_CAPTURE                      ((uint32_t)0x00000001U)
#define DCMI_CR_CM                           ((uint32_t)0x00000002U)
#define DCMI_CR_CROP                         ((uint32_t)0x00000004U)
#define DCMI_CR_JPEG                         ((uint32_t)0x00000008U)
#define DCMI_CR_ESS                          ((uint32_t)0x00000010U)
#define DCMI_CR_PCKPOL                       ((uint32_t)0x00000020U)
#define DCMI_CR_HSPOL                        ((uint32_t)0x00000040U)
#define DCMI_CR_VSPOL                        ((uint32_t)0x00000080U)
#define DCMI_CR_FCRC_0                       ((uint32_t)0x00000100U)
#define DCMI_CR_FCRC_1                       ((uint32_t)0x00000200U)
#define DCMI_CR_EDM_0                        ((uint32_t)0x00000400U)
#define DCMI_CR_EDM_1                        ((uint32_t)0x00000800U)
#define DCMI_CR_CRE                          ((uint32_t)0x00001000U)
#define DCMI_CR_ENABLE                       ((uint32_t)0x00004000U)

/********************  Bits definition for DCMI_SR register  ******************/
#define DCMI_SR_HSYNC                        ((uint32_t)0x00000001U)
#define DCMI_SR_VSYNC                        ((uint32_t)0x00000002U)
#define DCMI_SR_FNE                          ((uint32_t)0x00000004U)

/********************  Bits definition for DCMI_RISR register  ****************/
#define DCMI_RISR_FRAME_RIS                  ((uint32_t)0x00000001U)
#define DCMI_RISR_OVF_RIS                    ((uint32_t)0x00000002U)
#define DCMI_RISR_ERR_RIS                    ((uint32_t)0x00000004U)
#define DCMI_RISR_VSYNC_RIS                  ((uint32_t)0x00000008U)
#define DCMI_RISR_LINE_RIS                   ((uint32_t)0x00000010U)

/********************  Bits definition for DCMI_IER register  *****************/
#define DCMI_IER_FRAME_IE                    ((uint32_t)0x00000001U)
#define DCMI_IER_OVF_IE                      ((uint32_t)0x00000002U)
#define DCMI_IER_ERR_IE                      ((uint32_t)0x00000004U)
#define DCMI_IER_VSYNC_IE                    ((uint32_t)0x00000008U)
#define DCMI_IER_LINE_IE                     ((uint32_t)0x00000010U)

/********************  Bits definition for DCMI_MIS register  *****************/
#define DCMI_MIS_FRAME_MIS                   ((uint32_t)0x00000001U)
#define DCMI_MIS_OVR_MIS                     ((uint32_t)0x00000002U)
#define DCMI_MIS_ERR_MIS                     ((uint32_t)0x00000004U)
#define DCMI_MIS_VSYNC_MIS                   ((uint32_t)0x00000008U)
#define DCMI_MIS_LINE_MIS                    ((uint32_t)0x00000010U)

/* Legacy defines */
#define DCMI_MISR_FRAME_MIS                  DCMI_MIS_FRAME_MIS
#define DCMI_MISR_OVF_MIS                    DCMI_MIS_OVR_MIS
#define DCMI_MISR_ERR_MIS                    DCMI_MIS_ERR_MIS
#define DCMI_MISR_VSYNC_MIS                  DCMI_MIS_VSYNC_MIS
#define DCMI_MISR_LINE_MIS                   DCMI_MIS_LINE_MIS

/********************  Bits definition for DCMI_ICR register  *****************/
#define DCMI_ICR_FRAME_ISC                   ((uint32_t)0x00000001U)
#define DCMI_ICR_OVR_ISC                     ((uint32_t)0x00000002U)
#define DCMI_ICR_ERR_ISC                     ((uint32_t)0x00000004U)
#define DCMI_ICR_VSYNC_ISC                   ((uint32_t)0x00000008U)
#define DCMI_ICR_LINE_ISC                    ((uint32_t)0x00000010U)

/* Legacy defines */
#define DCMI_ICR_OVF_ISC                     DCMI_ICR_OVR_ISC

/********************  Bits definition for DCMI_ESCR register  ******************/
#define DCMI_ESCR_FSC                        ((uint32_t)0x000000FFU)
#define DCMI_ESCR_LSC                        ((uint32_t)0x0000FF00U)
#define DCMI_ESCR_LEC                        ((uint32_t)0x00FF0000U)
#define DCMI_ESCR_FEC                        ((uint32_t)0xFF000000U)

/********************  Bits definition for DCMI_ESUR register  ******************/
#define DCMI_ESUR_FSU                        ((uint32_t)0x000000FFU)
#define DCMI_ESUR_LSU                        ((uint32_t)0x0000FF00U)
#define DCMI_ESUR_LEU                        ((uint32_t)0x00FF0000U)
#define DCMI_ESUR_FEU                        ((uint32_t)0xFF000000U)

/********************  Bits definition for DCMI_CWSTRT register  ******************/
#define DCMI_CWSTRT_HOFFCNT                  ((uint32_t)0x00003FFFU)
#define DCMI_CWSTRT_VST                      ((uint32_t)0x1FFF0000U)

/********************  Bits definition for DCMI_CWSIZE register  ******************/
#define DCMI_CWSIZE_CAPCNT                   ((uint32_t)0x00003FFFU)
#define DCMI_CWSIZE_VLINE                    ((uint32_t)0x3FFF0000U)

/********************  Bits definition for DCMI_DR register  ******************/
#define DCMI_DR_BYTE0                        ((uint32_t)0x000000FFU)
#define DCMI_DR_BYTE1                        ((uint32_t)0x0000FF00U)
#define DCMI_DR_BYTE2                        ((uint32_t)0x00FF0000U)
#define DCMI_DR_BYTE3                        ((uint32_t)0xFF000000U)

/******************************************************************************/
/*                                                                            */
/*                             DMA Controller                                 */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DMA_SxCR register  *****************/
#define DMA_SxCR_CHSEL                       ((uint32_t)0x0E000000U)
#define DMA_SxCR_CHSEL_0                     ((uint32_t)0x02000000U)
#define DMA_SxCR_CHSEL_1                     ((uint32_t)0x04000000U)
#define DMA_SxCR_CHSEL_2                     ((uint32_t)0x08000000U)
#define DMA_SxCR_MBURST                      ((uint32_t)0x01800000U)
#define DMA_SxCR_MBURST_0                    ((uint32_t)0x00800000U)
#define DMA_SxCR_MBURST_1                    ((uint32_t)0x01000000U)
#define DMA_SxCR_PBURST                      ((uint32_t)0x00600000U)
#define DMA_SxCR_PBURST_0                    ((uint32_t)0x00200000U)
#define DMA_SxCR_PBURST_1                    ((uint32_t)0x00400000U)
#define DMA_SxCR_CT                          ((uint32_t)0x00080000U)
#define DMA_SxCR_DBM                         ((uint32_t)0x00040000U)
#define DMA_SxCR_PL                          ((uint32_t)0x00030000U)
#define DMA_SxCR_PL_0                        ((uint32_t)0x00010000U)
#define DMA_SxCR_PL_1                        ((uint32_t)0x00020000U)
#define DMA_SxCR_PINCOS                      ((uint32_t)0x00008000U)
#define DMA_SxCR_MSIZE                       ((uint32_t)0x00006000U)
#define DMA_SxCR_MSIZE_0                     ((uint32_t)0x00002000U)
#define DMA_SxCR_MSIZE_1                     ((uint32_t)0x00004000U)
#define DMA_SxCR_PSIZE                       ((uint32_t)0x00001800U)
#define DMA_SxCR_PSIZE_0                     ((uint32_t)0x00000800U)
#define DMA_SxCR_PSIZE_1                     ((uint32_t)0x00001000U)
#define DMA_SxCR_MINC                        ((uint32_t)0x00000400U)
#define DMA_SxCR_PINC                        ((uint32_t)0x00000200U)
#define DMA_SxCR_CIRC                        ((uint32_t)0x00000100U)
#define DMA_SxCR_DIR                         ((uint32_t)0x000000C0U)
#define DMA_SxCR_DIR_0                       ((uint32_t)0x00000040U)
#define DMA_SxCR_DIR_1                       ((uint32_t)0x00000080U)
#define DMA_SxCR_PFCTRL                      ((uint32_t)0x00000020U)
#define DMA_SxCR_TCIE                        ((uint32_t)0x00000010U)
#define DMA_SxCR_HTIE                        ((uint32_t)0x00000008U)
#define DMA_SxCR_TEIE                        ((uint32_t)0x00000004U)
#define DMA_SxCR_DMEIE                       ((uint32_t)0x00000002U)
#define DMA_SxCR_EN                          ((uint32_t)0x00000001U)

/* Legacy defines */
#define DMA_SxCR_ACK                         ((uint32_t)0x00100000U)

/********************  Bits definition for DMA_SxCNDTR register  **************/
#define DMA_SxNDT                            ((uint32_t)0x0000FFFFU)
#define DMA_SxNDT_0                          ((uint32_t)0x00000001U)
#define DMA_SxNDT_1                          ((uint32_t)0x00000002U)
#define DMA_SxNDT_2                          ((uint32_t)0x00000004U)
#define DMA_SxNDT_3                          ((uint32_t)0x00000008U)
#define DMA_SxNDT_4                          ((uint32_t)0x00000010U)
#define DMA_SxNDT_5                          ((uint32_t)0x00000020U)
#define DMA_SxNDT_6                          ((uint32_t)0x00000040U)
#define DMA_SxNDT_7                          ((uint32_t)0x00000080U)
#define DMA_SxNDT_8                          ((uint32_t)0x00000100U)
#define DMA_SxNDT_9                          ((uint32_t)0x00000200U)
#define DMA_SxNDT_10                         ((uint32_t)0x00000400U)
#define DMA_SxNDT_11                         ((uint32_t)0x00000800U)
#define DMA_SxNDT_12                         ((uint32_t)0x00001000U)
#define DMA_SxNDT_13                         ((uint32_t)0x00002000U)
#define DMA_SxNDT_14                         ((uint32_t)0x00004000U)
#define DMA_SxNDT_15                         ((uint32_t)0x00008000U)

/********************  Bits definition for DMA_SxFCR register  ****************/
#define DMA_SxFCR_FEIE                       ((uint32_t)0x00000080U)
#define DMA_SxFCR_FS                         ((uint32_t)0x00000038U)
#define DMA_SxFCR_FS_0                       ((uint32_t)0x00000008U)
#define DMA_SxFCR_FS_1                       ((uint32_t)0x00000010U)
#define DMA_SxFCR_FS_2                       ((uint32_t)0x00000020U)
#define DMA_SxFCR_DMDIS                      ((uint32_t)0x00000004U)
#define DMA_SxFCR_FTH                        ((uint32_t)0x00000003U)
#define DMA_SxFCR_FTH_0                      ((uint32_t)0x00000001U)
#define DMA_SxFCR_FTH_1                      ((uint32_t)0x00000002U)

/********************  Bits definition for DMA_LISR register  *****************/
#define DMA_LISR_TCIF3                       ((uint32_t)0x08000000U)
#define DMA_LISR_HTIF3                       ((uint32_t)0x04000000U)
#define DMA_LISR_TEIF3                       ((uint32_t)0x02000000U)
#define DMA_LISR_DMEIF3                      ((uint32_t)0x01000000U)
#define DMA_LISR_FEIF3                       ((uint32_t)0x00400000U)
#define DMA_LISR_TCIF2                       ((uint32_t)0x00200000U)
#define DMA_LISR_HTIF2                       ((uint32_t)0x00100000U)
#define DMA_LISR_TEIF2                       ((uint32_t)0x00080000U)
#define DMA_LISR_DMEIF2                      ((uint32_t)0x00040000U)
#define DMA_LISR_FEIF2                       ((uint32_t)0x00010000U)
#define DMA_LISR_TCIF1                       ((uint32_t)0x00000800U)
#define DMA_LISR_HTIF1                       ((uint32_t)0x00000400U)
#define DMA_LISR_TEIF1                       ((uint32_t)0x00000200U)
#define DMA_LISR_DMEIF1                      ((uint32_t)0x00000100U)
#define DMA_LISR_FEIF1                       ((uint32_t)0x00000040U)
#define DMA_LISR_TCIF0                       ((uint32_t)0x00000020U)
#define DMA_LISR_HTIF0                       ((uint32_t)0x00000010U)
#define DMA_LISR_TEIF0                       ((uint32_t)0x00000008U)
#define DMA_LISR_DMEIF0                      ((uint32_t)0x00000004U)
#define DMA_LISR_FEIF0                       ((uint32_t)0x00000001U)

/********************  Bits definition for DMA_HISR register  *****************/
#define DMA_HISR_TCIF7                       ((uint32_t)0x08000000U)
#define DMA_HISR_HTIF7                       ((uint32_t)0x04000000U)
#define DMA_HISR_TEIF7                       ((uint32_t)0x02000000U)
#define DMA_HISR_DMEIF7                      ((uint32_t)0x01000000U)
#define DMA_HISR_FEIF7                       ((uint32_t)0x00400000U)
#define DMA_HISR_TCIF6                       ((uint32_t)0x00200000U)
#define DMA_HISR_HTIF6                       ((uint32_t)0x00100000U)
#define DMA_HISR_TEIF6                       ((uint32_t)0x00080000U)
#define DMA_HISR_DMEIF6                      ((uint32_t)0x00040000U)
#define DMA_HISR_FEIF6                       ((uint32_t)0x00010000U)
#define DMA_HISR_TCIF5                       ((uint32_t)0x00000800U)
#define DMA_HISR_HTIF5                       ((uint32_t)0x00000400U)
#define DMA_HISR_TEIF5                       ((uint32_t)0x00000200U)
#define DMA_HISR_DMEIF5                      ((uint32_t)0x00000100U)
#define DMA_HISR_FEIF5                       ((uint32_t)0x00000040U)
#define DMA_HISR_TCIF4                       ((uint32_t)0x00000020U)
#define DMA_HISR_HTIF4                       ((uint32_t)0x00000010U)
#define DMA_HISR_TEIF4                       ((uint32_t)0x00000008U)
#define DMA_HISR_DMEIF4                      ((uint32_t)0x00000004U)
#define DMA_HISR_FEIF4                       ((uint32_t)0x00000001U)

/********************  Bits definition for DMA_LIFCR register  ****************/
#define DMA_LIFCR_CTCIF3                     ((uint32_t)0x08000000U)
#define DMA_LIFCR_CHTIF3                     ((uint32_t)0x04000000U)
#define DMA_LIFCR_CTEIF3                     ((uint32_t)0x02000000U)
#define DMA_LIFCR_CDMEIF3                    ((uint32_t)0x01000000U)
#define DMA_LIFCR_CFEIF3                     ((uint32_t)0x00400000U)
#define DMA_LIFCR_CTCIF2                     ((uint32_t)0x00200000U)
#define DMA_LIFCR_CHTIF2                     ((uint32_t)0x00100000U)
#define DMA_LIFCR_CTEIF2                     ((uint32_t)0x00080000U)
#define DMA_LIFCR_CDMEIF2                    ((uint32_t)0x00040000U)
#define DMA_LIFCR_CFEIF2                     ((uint32_t)0x00010000U)
#define DMA_LIFCR_CTCIF1                     ((uint32_t)0x00000800U)
#define DMA_LIFCR_CHTIF1                     ((uint32_t)0x00000400U)
#define DMA_LIFCR_CTEIF1                     ((uint32_t)0x00000200U)
#define DMA_LIFCR_CDMEIF1                    ((uint32_t)0x00000100U)
#define DMA_LIFCR_CFEIF1                     ((uint32_t)0x00000040U)
#define DMA_LIFCR_CTCIF0                     ((uint32_t)0x00000020U)
#define DMA_LIFCR_CHTIF0                     ((uint32_t)0x00000010U)
#define DMA_LIFCR_CTEIF0                     ((uint32_t)0x00000008U)
#define DMA_LIFCR_CDMEIF0                    ((uint32_t)0x00000004U)
#define DMA_LIFCR_CFEIF0                     ((uint32_t)0x00000001U)

/********************  Bits definition for DMA_HIFCR  register  ****************/
#define DMA_HIFCR_CTCIF7                     ((uint32_t)0x08000000U)
#define DMA_HIFCR_CHTIF7                     ((uint32_t)0x04000000U)
#define DMA_HIFCR_CTEIF7                     ((uint32_t)0x02000000U)
#define DMA_HIFCR_CDMEIF7                    ((uint32_t)0x01000000U)
#define DMA_HIFCR_CFEIF7                     ((uint32_t)0x00400000U)
#define DMA_HIFCR_CTCIF6                     ((uint32_t)0x00200000U)
#define DMA_HIFCR_CHTIF6                     ((uint32_t)0x00100000U)
#define DMA_HIFCR_CTEIF6                     ((uint32_t)0x00080000U)
#define DMA_HIFCR_CDMEIF6                    ((uint32_t)0x00040000U)
#define DMA_HIFCR_CFEIF6                     ((uint32_t)0x00010000U)
#define DMA_HIFCR_CTCIF5                     ((uint32_t)0x00000800U)
#define DMA_HIFCR_CHTIF5                     ((uint32_t)0x00000400U)
#define DMA_HIFCR_CTEIF5                     ((uint32_t)0x00000200U)
#define DMA_HIFCR_CDMEIF5                    ((uint32_t)0x00000100U)
#define DMA_HIFCR_CFEIF5                     ((uint32_t)0x00000040U)
#define DMA_HIFCR_CTCIF4                     ((uint32_t)0x00000020U)
#define DMA_HIFCR_CHTIF4                     ((uint32_t)0x00000010U)
#define DMA_HIFCR_CTEIF4                     ((uint32_t)0x00000008U)
#define DMA_HIFCR_CDMEIF4                    ((uint32_t)0x00000004U)
#define DMA_HIFCR_CFEIF4                     ((uint32_t)0x00000001U)


/******************************************************************************/
/*                                                                            */
/*                         AHB Master DMA2D Controller (DMA2D)                */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for DMA2D_CR register  ******************/

#define DMA2D_CR_START                     ((uint32_t)0x00000001U)               /*!< Start transfer */
#define DMA2D_CR_SUSP                      ((uint32_t)0x00000002U)               /*!< Suspend transfer */
#define DMA2D_CR_ABORT                     ((uint32_t)0x00000004U)               /*!< Abort transfer */
#define DMA2D_CR_TEIE                      ((uint32_t)0x00000100U)               /*!< Transfer Error Interrupt Enable */
#define DMA2D_CR_TCIE                      ((uint32_t)0x00000200U)               /*!< Transfer Complete Interrupt Enable */
#define DMA2D_CR_TWIE                      ((uint32_t)0x00000400U)               /*!< Transfer Watermark Interrupt Enable */
#define DMA2D_CR_CAEIE                     ((uint32_t)0x00000800U)               /*!< CLUT Access Error Interrupt Enable */
#define DMA2D_CR_CTCIE                     ((uint32_t)0x00001000U)               /*!< CLUT Transfer Complete Interrupt Enable */
#define DMA2D_CR_CEIE                      ((uint32_t)0x00002000U)               /*!< Configuration Error Interrupt Enable */
#define DMA2D_CR_MODE                      ((uint32_t)0x00030000U)               /*!< DMA2D Mode */

/********************  Bit definition for DMA2D_ISR register  *****************/

#define DMA2D_ISR_TEIF                     ((uint32_t)0x00000001U)               /*!< Transfer Error Interrupt Flag */
#define DMA2D_ISR_TCIF                     ((uint32_t)0x00000002U)               /*!< Transfer Complete Interrupt Flag */
#define DMA2D_ISR_TWIF                     ((uint32_t)0x00000004U)               /*!< Transfer Watermark Interrupt Flag */
#define DMA2D_ISR_CAEIF                    ((uint32_t)0x00000008U)               /*!< CLUT Access Error Interrupt Flag */
#define DMA2D_ISR_CTCIF                    ((uint32_t)0x00000010U)               /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_ISR_CEIF                     ((uint32_t)0x00000020U)               /*!< Configuration Error Interrupt Flag */

/********************  Bit definition for DMA2D_IFCR register  ****************/

#define DMA2D_IFCR_CTEIF                   ((uint32_t)0x00000001U)               /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFCR_CTCIF                   ((uint32_t)0x00000002U)               /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFCR_CTWIF                   ((uint32_t)0x00000004U)               /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFCR_CAECIF                  ((uint32_t)0x00000008U)               /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFCR_CCTCIF                  ((uint32_t)0x00000010U)               /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFCR_CCEIF                   ((uint32_t)0x00000020U)               /*!< Clears Configuration Error Interrupt Flag    */

/* Legacy defines */
#define DMA2D_IFSR_CTEIF                   DMA2D_IFCR_CTEIF                     /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFSR_CTCIF                   DMA2D_IFCR_CTCIF                     /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFSR_CTWIF                   DMA2D_IFCR_CTWIF                     /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFSR_CCAEIF                  DMA2D_IFCR_CAECIF                    /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFSR_CCTCIF                  DMA2D_IFCR_CCTCIF                    /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFSR_CCEIF                   DMA2D_IFCR_CCEIF                     /*!< Clears Configuration Error Interrupt Flag    */

/********************  Bit definition for DMA2D_FGMAR register  ***************/

#define DMA2D_FGMAR_MA                     ((uint32_t)0xFFFFFFFFU)               /*!< Memory Address */

/********************  Bit definition for DMA2D_FGOR register  ****************/

#define DMA2D_FGOR_LO                      ((uint32_t)0x00003FFFU)               /*!< Line Offset */

/********************  Bit definition for DMA2D_BGMAR register  ***************/

#define DMA2D_BGMAR_MA                     ((uint32_t)0xFFFFFFFFU)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGOR register  ****************/

#define DMA2D_BGOR_LO                      ((uint32_t)0x00003FFFU)               /*!< Line Offset */

/********************  Bit definition for DMA2D_FGPFCCR register  *************/

#define DMA2D_FGPFCCR_CM                   ((uint32_t)0x0000000FU)               /*!< Color mode */
#define DMA2D_FGPFCCR_CCM                  ((uint32_t)0x00000010U)               /*!< CLUT Color mode */
#define DMA2D_FGPFCCR_START                ((uint32_t)0x00000020U)               /*!< Start */
#define DMA2D_FGPFCCR_CS                   ((uint32_t)0x0000FF00U)               /*!< CLUT size */
#define DMA2D_FGPFCCR_AM                   ((uint32_t)0x00030000U)               /*!< Alpha mode */
#define DMA2D_FGPFCCR_ALPHA                ((uint32_t)0xFF000000U)               /*!< Alpha value */

/********************  Bit definition for DMA2D_FGCOLR register  **************/

#define DMA2D_FGCOLR_BLUE                  ((uint32_t)0x000000FFU)               /*!< Blue Value */
#define DMA2D_FGCOLR_GREEN                 ((uint32_t)0x0000FF00U)               /*!< Green Value */
#define DMA2D_FGCOLR_RED                   ((uint32_t)0x00FF0000U)               /*!< Red Value */

/********************  Bit definition for DMA2D_BGPFCCR register  *************/

#define DMA2D_BGPFCCR_CM                   ((uint32_t)0x0000000FU)               /*!< Color mode */
#define DMA2D_BGPFCCR_CCM                  ((uint32_t)0x00000010U)               /*!< CLUT Color mode */
#define DMA2D_BGPFCCR_START                ((uint32_t)0x00000020U)               /*!< Start */
#define DMA2D_BGPFCCR_CS                   ((uint32_t)0x0000FF00U)               /*!< CLUT size */
#define DMA2D_BGPFCCR_AM                   ((uint32_t)0x00030000U)               /*!< Alpha Mode */
#define DMA2D_BGPFCCR_ALPHA                ((uint32_t)0xFF000000U)               /*!< Alpha value */

/********************  Bit definition for DMA2D_BGCOLR register  **************/

#define DMA2D_BGCOLR_BLUE                  ((uint32_t)0x000000FFU)               /*!< Blue Value */
#define DMA2D_BGCOLR_GREEN                 ((uint32_t)0x0000FF00U)               /*!< Green Value */
#define DMA2D_BGCOLR_RED                   ((uint32_t)0x00FF0000U)               /*!< Red Value */

/********************  Bit definition for DMA2D_FGCMAR register  **************/

#define DMA2D_FGCMAR_MA                    ((uint32_t)0xFFFFFFFFU)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGCMAR register  **************/

#define DMA2D_BGCMAR_MA                    ((uint32_t)0xFFFFFFFFU)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OPFCCR register  **************/

#define DMA2D_OPFCCR_CM                    ((uint32_t)0x00000007U)               /*!< Color mode */

/********************  Bit definition for DMA2D_OCOLR register  ***************/

/*!<Mode_ARGB8888/RGB888 */

#define DMA2D_OCOLR_BLUE_1                 ((uint32_t)0x000000FFU)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_1                ((uint32_t)0x0000FF00U)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_1                  ((uint32_t)0x00FF0000U)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_1                ((uint32_t)0xFF000000U)               /*!< Alpha Channel Value */

/*!<Mode_RGB565 */
#define DMA2D_OCOLR_BLUE_2                 ((uint32_t)0x0000001FU)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_2                ((uint32_t)0x000007E0U)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_2                  ((uint32_t)0x0000F800U)               /*!< Red Value */

/*!<Mode_ARGB1555 */
#define DMA2D_OCOLR_BLUE_3                 ((uint32_t)0x0000001FU)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_3                ((uint32_t)0x000003E0U)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_3                  ((uint32_t)0x00007C00U)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_3                ((uint32_t)0x00008000U)               /*!< Alpha Channel Value */

/*!<Mode_ARGB4444 */
#define DMA2D_OCOLR_BLUE_4                 ((uint32_t)0x0000000FU)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_4                ((uint32_t)0x000000F0U)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_4                  ((uint32_t)0x00000F00U)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_4                ((uint32_t)0x0000F000U)               /*!< Alpha Channel Value */

/********************  Bit definition for DMA2D_OMAR register  ****************/

#define DMA2D_OMAR_MA                      ((uint32_t)0xFFFFFFFFU)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OOR register  *****************/

#define DMA2D_OOR_LO                       ((uint32_t)0x00003FFFU)               /*!< Line Offset */

/********************  Bit definition for DMA2D_NLR register  *****************/

#define DMA2D_NLR_NL                       ((uint32_t)0x0000FFFFU)               /*!< Number of Lines */
#define DMA2D_NLR_PL                       ((uint32_t)0x3FFF0000U)               /*!< Pixel per Lines */

/********************  Bit definition for DMA2D_LWR register  *****************/

#define DMA2D_LWR_LW                       ((uint32_t)0x0000FFFFU)               /*!< Line Watermark */

/********************  Bit definition for DMA2D_AMTCR register  ***************/

#define DMA2D_AMTCR_EN                     ((uint32_t)0x00000001U)               /*!< Enable */
#define DMA2D_AMTCR_DT                     ((uint32_t)0x0000FF00U)               /*!< Dead Time */


/********************  Bit definition for DMA2D_FGCLUT register  **************/

/********************  Bit definition for DMA2D_BGCLUT register  **************/



/******************************************************************************/
/*                                                                            */
/*                    External Interrupt/Event Controller                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for EXTI_IMR register  *******************/
#define  EXTI_IMR_MR0                        ((uint32_t)0x00000001U)        /*!< Interrupt Mask on line 0 */
#define  EXTI_IMR_MR1                        ((uint32_t)0x00000002U)        /*!< Interrupt Mask on line 1 */
#define  EXTI_IMR_MR2                        ((uint32_t)0x00000004U)        /*!< Interrupt Mask on line 2 */
#define  EXTI_IMR_MR3                        ((uint32_t)0x00000008U)        /*!< Interrupt Mask on line 3 */
#define  EXTI_IMR_MR4                        ((uint32_t)0x00000010U)        /*!< Interrupt Mask on line 4 */
#define  EXTI_IMR_MR5                        ((uint32_t)0x00000020U)        /*!< Interrupt Mask on line 5 */
#define  EXTI_IMR_MR6                        ((uint32_t)0x00000040U)        /*!< Interrupt Mask on line 6 */
#define  EXTI_IMR_MR7                        ((uint32_t)0x00000080U)        /*!< Interrupt Mask on line 7 */
#define  EXTI_IMR_MR8                        ((uint32_t)0x00000100U)        /*!< Interrupt Mask on line 8 */
#define  EXTI_IMR_MR9                        ((uint32_t)0x00000200U)        /*!< Interrupt Mask on line 9 */
#define  EXTI_IMR_MR10                       ((uint32_t)0x00000400U)        /*!< Interrupt Mask on line 10 */
#define  EXTI_IMR_MR11                       ((uint32_t)0x00000800U)        /*!< Interrupt Mask on line 11 */
#define  EXTI_IMR_MR12                       ((uint32_t)0x00001000U)        /*!< Interrupt Mask on line 12 */
#define  EXTI_IMR_MR13                       ((uint32_t)0x00002000U)        /*!< Interrupt Mask on line 13 */
#define  EXTI_IMR_MR14                       ((uint32_t)0x00004000U)        /*!< Interrupt Mask on line 14 */
#define  EXTI_IMR_MR15                       ((uint32_t)0x00008000U)        /*!< Interrupt Mask on line 15 */
#define  EXTI_IMR_MR16                       ((uint32_t)0x00010000U)        /*!< Interrupt Mask on line 16 */
#define  EXTI_IMR_MR17                       ((uint32_t)0x00020000U)        /*!< Interrupt Mask on line 17 */
#define  EXTI_IMR_MR18                       ((uint32_t)0x00040000U)        /*!< Interrupt Mask on line 18 */
#define  EXTI_IMR_MR19                       ((uint32_t)0x00080000U)        /*!< Interrupt Mask on line 19 */
#define  EXTI_IMR_MR20                       ((uint32_t)0x00100000U)        /*!< Interrupt Mask on line 20 */
#define  EXTI_IMR_MR21                       ((uint32_t)0x00200000U)        /*!< Interrupt Mask on line 21 */
#define  EXTI_IMR_MR22                       ((uint32_t)0x00400000U)        /*!< Interrupt Mask on line 22 */

/*******************  Bit definition for EXTI_EMR register  *******************/
#define  EXTI_EMR_MR0                        ((uint32_t)0x00000001U)        /*!< Event Mask on line 0 */
#define  EXTI_EMR_MR1                        ((uint32_t)0x00000002U)        /*!< Event Mask on line 1 */
#define  EXTI_EMR_MR2                        ((uint32_t)0x00000004U)        /*!< Event Mask on line 2 */
#define  EXTI_EMR_MR3                        ((uint32_t)0x00000008U)        /*!< Event Mask on line 3 */
#define  EXTI_EMR_MR4                        ((uint32_t)0x00000010U)        /*!< Event Mask on line 4 */
#define  EXTI_EMR_MR5                        ((uint32_t)0x00000020U)        /*!< Event Mask on line 5 */
#define  EXTI_EMR_MR6                        ((uint32_t)0x00000040U)        /*!< Event Mask on line 6 */
#define  EXTI_EMR_MR7                        ((uint32_t)0x00000080U)        /*!< Event Mask on line 7 */
#define  EXTI_EMR_MR8                        ((uint32_t)0x00000100U)        /*!< Event Mask on line 8 */
#define  EXTI_EMR_MR9                        ((uint32_t)0x00000200U)        /*!< Event Mask on line 9 */
#define  EXTI_EMR_MR10                       ((uint32_t)0x00000400U)        /*!< Event Mask on line 10 */
#define  EXTI_EMR_MR11                       ((uint32_t)0x00000800U)        /*!< Event Mask on line 11 */
#define  EXTI_EMR_MR12                       ((uint32_t)0x00001000U)        /*!< Event Mask on line 12 */
#define  EXTI_EMR_MR13                       ((uint32_t)0x00002000U)        /*!< Event Mask on line 13 */
#define  EXTI_EMR_MR14                       ((uint32_t)0x00004000U)        /*!< Event Mask on line 14 */
#define  EXTI_EMR_MR15                       ((uint32_t)0x00008000U)        /*!< Event Mask on line 15 */
#define  EXTI_EMR_MR16                       ((uint32_t)0x00010000U)        /*!< Event Mask on line 16 */
#define  EXTI_EMR_MR17                       ((uint32_t)0x00020000U)        /*!< Event Mask on line 17 */
#define  EXTI_EMR_MR18                       ((uint32_t)0x00040000U)        /*!< Event Mask on line 18 */
#define  EXTI_EMR_MR19                       ((uint32_t)0x00080000U)        /*!< Event Mask on line 19 */
#define  EXTI_EMR_MR20                       ((uint32_t)0x00100000U)        /*!< Event Mask on line 20 */
#define  EXTI_EMR_MR21                       ((uint32_t)0x00200000U)        /*!< Event Mask on line 21 */
#define  EXTI_EMR_MR22                       ((uint32_t)0x00400000U)        /*!< Event Mask on line 22 */

/******************  Bit definition for EXTI_RTSR register  *******************/
#define  EXTI_RTSR_TR0                       ((uint32_t)0x00000001U)        /*!< Rising trigger event configuration bit of line 0 */
#define  EXTI_RTSR_TR1                       ((uint32_t)0x00000002U)        /*!< Rising trigger event configuration bit of line 1 */
#define  EXTI_RTSR_TR2                       ((uint32_t)0x00000004U)        /*!< Rising trigger event configuration bit of line 2 */
#define  EXTI_RTSR_TR3                       ((uint32_t)0x00000008U)        /*!< Rising trigger event configuration bit of line 3 */
#define  EXTI_RTSR_TR4                       ((uint32_t)0x00000010U)        /*!< Rising trigger event configuration bit of line 4 */
#define  EXTI_RTSR_TR5                       ((uint32_t)0x00000020U)        /*!< Rising trigger event configuration bit of line 5 */
#define  EXTI_RTSR_TR6                       ((uint32_t)0x00000040U)        /*!< Rising trigger event configuration bit of line 6 */
#define  EXTI_RTSR_TR7                       ((uint32_t)0x00000080U)        /*!< Rising trigger event configuration bit of line 7 */
#define  EXTI_RTSR_TR8                       ((uint32_t)0x00000100U)        /*!< Rising trigger event configuration bit of line 8 */
#define  EXTI_RTSR_TR9                       ((uint32_t)0x00000200U)        /*!< Rising trigger event configuration bit of line 9 */
#define  EXTI_RTSR_TR10                      ((uint32_t)0x00000400U)        /*!< Rising trigger event configuration bit of line 10 */
#define  EXTI_RTSR_TR11                      ((uint32_t)0x00000800U)        /*!< Rising trigger event configuration bit of line 11 */
#define  EXTI_RTSR_TR12                      ((uint32_t)0x00001000U)        /*!< Rising trigger event configuration bit of line 12 */
#define  EXTI_RTSR_TR13                      ((uint32_t)0x00002000U)        /*!< Rising trigger event configuration bit of line 13 */
#define  EXTI_RTSR_TR14                      ((uint32_t)0x00004000U)        /*!< Rising trigger event configuration bit of line 14 */
#define  EXTI_RTSR_TR15                      ((uint32_t)0x00008000U)        /*!< Rising trigger event configuration bit of line 15 */
#define  EXTI_RTSR_TR16                      ((uint32_t)0x00010000U)        /*!< Rising trigger event configuration bit of line 16 */
#define  EXTI_RTSR_TR17                      ((uint32_t)0x00020000U)        /*!< Rising trigger event configuration bit of line 17 */
#define  EXTI_RTSR_TR18                      ((uint32_t)0x00040000U)        /*!< Rising trigger event configuration bit of line 18 */
#define  EXTI_RTSR_TR19                      ((uint32_t)0x00080000U)        /*!< Rising trigger event configuration bit of line 19 */
#define  EXTI_RTSR_TR20                      ((uint32_t)0x00100000U)        /*!< Rising trigger event configuration bit of line 20 */
#define  EXTI_RTSR_TR21                      ((uint32_t)0x00200000U)        /*!< Rising trigger event configuration bit of line 21 */
#define  EXTI_RTSR_TR22                      ((uint32_t)0x00400000U)        /*!< Rising trigger event configuration bit of line 22 */

/******************  Bit definition for EXTI_FTSR register  *******************/
#define  EXTI_FTSR_TR0                       ((uint32_t)0x00000001U)        /*!< Falling trigger event configuration bit of line 0 */
#define  EXTI_FTSR_TR1                       ((uint32_t)0x00000002U)        /*!< Falling trigger event configuration bit of line 1 */
#define  EXTI_FTSR_TR2                       ((uint32_t)0x00000004U)        /*!< Falling trigger event configuration bit of line 2 */
#define  EXTI_FTSR_TR3                       ((uint32_t)0x00000008U)        /*!< Falling trigger event configuration bit of line 3 */
#define  EXTI_FTSR_TR4                       ((uint32_t)0x00000010U)        /*!< Falling trigger event configuration bit of line 4 */
#define  EXTI_FTSR_TR5                       ((uint32_t)0x00000020U)        /*!< Falling trigger event configuration bit of line 5 */
#define  EXTI_FTSR_TR6                       ((uint32_t)0x00000040U)        /*!< Falling trigger event configuration bit of line 6 */
#define  EXTI_FTSR_TR7                       ((uint32_t)0x00000080U)        /*!< Falling trigger event configuration bit of line 7 */
#define  EXTI_FTSR_TR8                       ((uint32_t)0x00000100U)        /*!< Falling trigger event configuration bit of line 8 */
#define  EXTI_FTSR_TR9                       ((uint32_t)0x00000200U)        /*!< Falling trigger event configuration bit of line 9 */
#define  EXTI_FTSR_TR10                      ((uint32_t)0x00000400U)        /*!< Falling trigger event configuration bit of line 10 */
#define  EXTI_FTSR_TR11                      ((uint32_t)0x00000800U)        /*!< Falling trigger event configuration bit of line 11 */
#define  EXTI_FTSR_TR12                      ((uint32_t)0x00001000U)        /*!< Falling trigger event configuration bit of line 12 */
#define  EXTI_FTSR_TR13                      ((uint32_t)0x00002000U)        /*!< Falling trigger event configuration bit of line 13 */
#define  EXTI_FTSR_TR14                      ((uint32_t)0x00004000U)        /*!< Falling trigger event configuration bit of line 14 */
#define  EXTI_FTSR_TR15                      ((uint32_t)0x00008000U)        /*!< Falling trigger event configuration bit of line 15 */
#define  EXTI_FTSR_TR16                      ((uint32_t)0x00010000U)        /*!< Falling trigger event configuration bit of line 16 */
#define  EXTI_FTSR_TR17                      ((uint32_t)0x00020000U)        /*!< Falling trigger event configuration bit of line 17 */
#define  EXTI_FTSR_TR18                      ((uint32_t)0x00040000U)        /*!< Falling trigger event configuration bit of line 18 */
#define  EXTI_FTSR_TR19                      ((uint32_t)0x00080000U)        /*!< Falling trigger event configuration bit of line 19 */
#define  EXTI_FTSR_TR20                      ((uint32_t)0x00100000U)        /*!< Falling trigger event configuration bit of line 20 */
#define  EXTI_FTSR_TR21                      ((uint32_t)0x00200000U)        /*!< Falling trigger event configuration bit of line 21 */
#define  EXTI_FTSR_TR22                      ((uint32_t)0x00400000U)        /*!< Falling trigger event configuration bit of line 22 */

/******************  Bit definition for EXTI_SWIER register  ******************/
#define  EXTI_SWIER_SWIER0                   ((uint32_t)0x00000001U)        /*!< Software Interrupt on line 0 */
#define  EXTI_SWIER_SWIER1                   ((uint32_t)0x00000002U)        /*!< Software Interrupt on line 1 */
#define  EXTI_SWIER_SWIER2                   ((uint32_t)0x00000004U)        /*!< Software Interrupt on line 2 */
#define  EXTI_SWIER_SWIER3                   ((uint32_t)0x00000008U)        /*!< Software Interrupt on line 3 */
#define  EXTI_SWIER_SWIER4                   ((uint32_t)0x00000010U)        /*!< Software Interrupt on line 4 */
#define  EXTI_SWIER_SWIER5                   ((uint32_t)0x00000020U)        /*!< Software Interrupt on line 5 */
#define  EXTI_SWIER_SWIER6                   ((uint32_t)0x00000040U)        /*!< Software Interrupt on line 6 */
#define  EXTI_SWIER_SWIER7                   ((uint32_t)0x00000080U)        /*!< Software Interrupt on line 7 */
#define  EXTI_SWIER_SWIER8                   ((uint32_t)0x00000100U)        /*!< Software Interrupt on line 8 */
#define  EXTI_SWIER_SWIER9                   ((uint32_t)0x00000200U)        /*!< Software Interrupt on line 9 */
#define  EXTI_SWIER_SWIER10                  ((uint32_t)0x00000400U)        /*!< Software Interrupt on line 10 */
#define  EXTI_SWIER_SWIER11                  ((uint32_t)0x00000800U)        /*!< Software Interrupt on line 11 */
#define  EXTI_SWIER_SWIER12                  ((uint32_t)0x00001000U)        /*!< Software Interrupt on line 12 */
#define  EXTI_SWIER_SWIER13                  ((uint32_t)0x00002000U)        /*!< Software Interrupt on line 13 */
#define  EXTI_SWIER_SWIER14                  ((uint32_t)0x00004000U)        /*!< Software Interrupt on line 14 */
#define  EXTI_SWIER_SWIER15                  ((uint32_t)0x00008000U)        /*!< Software Interrupt on line 15 */
#define  EXTI_SWIER_SWIER16                  ((uint32_t)0x00010000U)        /*!< Software Interrupt on line 16 */
#define  EXTI_SWIER_SWIER17                  ((uint32_t)0x00020000U)        /*!< Software Interrupt on line 17 */
#define  EXTI_SWIER_SWIER18                  ((uint32_t)0x00040000U)        /*!< Software Interrupt on line 18 */
#define  EXTI_SWIER_SWIER19                  ((uint32_t)0x00080000U)        /*!< Software Interrupt on line 19 */
#define  EXTI_SWIER_SWIER20                  ((uint32_t)0x00100000U)        /*!< Software Interrupt on line 20 */
#define  EXTI_SWIER_SWIER21                  ((uint32_t)0x00200000U)        /*!< Software Interrupt on line 21 */
#define  EXTI_SWIER_SWIER22                  ((uint32_t)0x00400000U)        /*!< Software Interrupt on line 22 */

/*******************  Bit definition for EXTI_PR register  ********************/
#define  EXTI_PR_PR0                         ((uint32_t)0x00000001U)        /*!< Pending bit for line 0 */
#define  EXTI_PR_PR1                         ((uint32_t)0x00000002U)        /*!< Pending bit for line 1 */
#define  EXTI_PR_PR2                         ((uint32_t)0x00000004U)        /*!< Pending bit for line 2 */
#define  EXTI_PR_PR3                         ((uint32_t)0x00000008U)        /*!< Pending bit for line 3 */
#define  EXTI_PR_PR4                         ((uint32_t)0x00000010U)        /*!< Pending bit for line 4 */
#define  EXTI_PR_PR5                         ((uint32_t)0x00000020U)        /*!< Pending bit for line 5 */
#define  EXTI_PR_PR6                         ((uint32_t)0x00000040U)        /*!< Pending bit for line 6 */
#define  EXTI_PR_PR7                         ((uint32_t)0x00000080U)        /*!< Pending bit for line 7 */
#define  EXTI_PR_PR8                         ((uint32_t)0x00000100U)        /*!< Pending bit for line 8 */
#define  EXTI_PR_PR9                         ((uint32_t)0x00000200U)        /*!< Pending bit for line 9 */
#define  EXTI_PR_PR10                        ((uint32_t)0x00000400U)        /*!< Pending bit for line 10 */
#define  EXTI_PR_PR11                        ((uint32_t)0x00000800U)        /*!< Pending bit for line 11 */
#define  EXTI_PR_PR12                        ((uint32_t)0x00001000U)        /*!< Pending bit for line 12 */
#define  EXTI_PR_PR13                        ((uint32_t)0x00002000U)        /*!< Pending bit for line 13 */
#define  EXTI_PR_PR14                        ((uint32_t)0x00004000U)        /*!< Pending bit for line 14 */
#define  EXTI_PR_PR15                        ((uint32_t)0x00008000U)        /*!< Pending bit for line 15 */
#define  EXTI_PR_PR16                        ((uint32_t)0x00010000U)        /*!< Pending bit for line 16 */
#define  EXTI_PR_PR17                        ((uint32_t)0x00020000U)        /*!< Pending bit for line 17 */
#define  EXTI_PR_PR18                        ((uint32_t)0x00040000U)        /*!< Pending bit for line 18 */
#define  EXTI_PR_PR19                        ((uint32_t)0x00080000U)        /*!< Pending bit for line 19 */
#define  EXTI_PR_PR20                        ((uint32_t)0x00100000U)        /*!< Pending bit for line 20 */
#define  EXTI_PR_PR21                        ((uint32_t)0x00200000U)        /*!< Pending bit for line 21 */
#define  EXTI_PR_PR22                        ((uint32_t)0x00400000U)        /*!< Pending bit for line 22 */

/******************************************************************************/
/*                                                                            */
/*                                    FLASH                                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bits definition for FLASH_ACR register  *****************/
#define FLASH_ACR_LATENCY                    ((uint32_t)0x0000000FU)
#define FLASH_ACR_LATENCY_0WS                ((uint32_t)0x00000000U)
#define FLASH_ACR_LATENCY_1WS                ((uint32_t)0x00000001U)
#define FLASH_ACR_LATENCY_2WS                ((uint32_t)0x00000002U)
#define FLASH_ACR_LATENCY_3WS                ((uint32_t)0x00000003U)
#define FLASH_ACR_LATENCY_4WS                ((uint32_t)0x00000004U)
#define FLASH_ACR_LATENCY_5WS                ((uint32_t)0x00000005U)
#define FLASH_ACR_LATENCY_6WS                ((uint32_t)0x00000006U)
#define FLASH_ACR_LATENCY_7WS                ((uint32_t)0x00000007U)
#define FLASH_ACR_LATENCY_8WS                ((uint32_t)0x00000008U)
#define FLASH_ACR_LATENCY_9WS                ((uint32_t)0x00000009U)
#define FLASH_ACR_LATENCY_10WS               ((uint32_t)0x0000000AU)
#define FLASH_ACR_LATENCY_11WS               ((uint32_t)0x0000000BU)
#define FLASH_ACR_LATENCY_12WS               ((uint32_t)0x0000000CU)
#define FLASH_ACR_LATENCY_13WS               ((uint32_t)0x0000000DU)
#define FLASH_ACR_LATENCY_14WS               ((uint32_t)0x0000000EU)
#define FLASH_ACR_LATENCY_15WS               ((uint32_t)0x0000000FU)
#define FLASH_ACR_PRFTEN                     ((uint32_t)0x00000100U)
#define FLASH_ACR_ICEN                       ((uint32_t)0x00000200U)
#define FLASH_ACR_DCEN                       ((uint32_t)0x00000400U)
#define FLASH_ACR_ICRST                      ((uint32_t)0x00000800U)
#define FLASH_ACR_DCRST                      ((uint32_t)0x00001000U)
#define FLASH_ACR_BYTE0_ADDRESS              ((uint32_t)0x40023C00U)
#define FLASH_ACR_BYTE2_ADDRESS              ((uint32_t)0x40023C03U)

/*******************  Bits definition for FLASH_SR register  ******************/
#define FLASH_SR_EOP                         ((uint32_t)0x00000001U)
#define FLASH_SR_SOP                         ((uint32_t)0x00000002U)
#define FLASH_SR_WRPERR                      ((uint32_t)0x00000010U)
#define FLASH_SR_PGAERR                      ((uint32_t)0x00000020U)
#define FLASH_SR_PGPERR                      ((uint32_t)0x00000040U)
#define FLASH_SR_PGSERR                      ((uint32_t)0x00000080U)
#define FLASH_SR_BSY                         ((uint32_t)0x00010000U)

/*******************  Bits definition for FLASH_CR register  ******************/
#define FLASH_CR_PG                          ((uint32_t)0x00000001U)
#define FLASH_CR_SER                         ((uint32_t)0x00000002U)
#define FLASH_CR_MER                         ((uint32_t)0x00000004U)
#define FLASH_CR_MER1                        FLASH_CR_MER
#define FLASH_CR_SNB                         ((uint32_t)0x000000F8U)
#define FLASH_CR_SNB_0                       ((uint32_t)0x00000008U)
#define FLASH_CR_SNB_1                       ((uint32_t)0x00000010U)
#define FLASH_CR_SNB_2                       ((uint32_t)0x00000020U)
#define FLASH_CR_SNB_3                       ((uint32_t)0x00000040U)
#define FLASH_CR_SNB_4                       ((uint32_t)0x00000080U)
#define FLASH_CR_PSIZE                       ((uint32_t)0x00000300U)
#define FLASH_CR_PSIZE_0                     ((uint32_t)0x00000100U)
#define FLASH_CR_PSIZE_1                     ((uint32_t)0x00000200U)
#define FLASH_CR_MER2                        ((uint32_t)0x00008000U)
#define FLASH_CR_STRT                        ((uint32_t)0x00010000U)
#define FLASH_CR_EOPIE                       ((uint32_t)0x01000000U)
#define FLASH_CR_LOCK                        ((uint32_t)0x80000000U)

/*******************  Bits definition for FLASH_OPTCR register  ***************/
#define FLASH_OPTCR_OPTLOCK                 ((uint32_t)0x00000001U)
#define FLASH_OPTCR_OPTSTRT                 ((uint32_t)0x00000002U)
#define FLASH_OPTCR_BOR_LEV_0               ((uint32_t)0x00000004U)
#define FLASH_OPTCR_BOR_LEV_1               ((uint32_t)0x00000008U)
#define FLASH_OPTCR_BOR_LEV                 ((uint32_t)0x0000000CU)
#define FLASH_OPTCR_BFB2                    ((uint32_t)0x00000010U)
#define FLASH_OPTCR_WDG_SW                  ((uint32_t)0x00000020U)
#define FLASH_OPTCR_nRST_STOP               ((uint32_t)0x00000040U)
#define FLASH_OPTCR_nRST_STDBY              ((uint32_t)0x00000080U)
#define FLASH_OPTCR_RDP                     ((uint32_t)0x0000FF00U)
#define FLASH_OPTCR_RDP_0                   ((uint32_t)0x00000100U)
#define FLASH_OPTCR_RDP_1                   ((uint32_t)0x00000200U)
#define FLASH_OPTCR_RDP_2                   ((uint32_t)0x00000400U)
#define FLASH_OPTCR_RDP_3                   ((uint32_t)0x00000800U)
#define FLASH_OPTCR_RDP_4                   ((uint32_t)0x00001000U)
#define FLASH_OPTCR_RDP_5                   ((uint32_t)0x00002000U)
#define FLASH_OPTCR_RDP_6                   ((uint32_t)0x00004000U)
#define FLASH_OPTCR_RDP_7                   ((uint32_t)0x00008000U)
#define FLASH_OPTCR_nWRP                    ((uint32_t)0x0FFF0000U)
#define FLASH_OPTCR_nWRP_0                  ((uint32_t)0x00010000U)
#define FLASH_OPTCR_nWRP_1                  ((uint32_t)0x00020000U)
#define FLASH_OPTCR_nWRP_2                  ((uint32_t)0x00040000U)
#define FLASH_OPTCR_nWRP_3                  ((uint32_t)0x00080000U)
#define FLASH_OPTCR_nWRP_4                  ((uint32_t)0x00100000U)
#define FLASH_OPTCR_nWRP_5                  ((uint32_t)0x00200000U)
#define FLASH_OPTCR_nWRP_6                  ((uint32_t)0x00400000U)
#define FLASH_OPTCR_nWRP_7                  ((uint32_t)0x00800000U)
#define FLASH_OPTCR_nWRP_8                  ((uint32_t)0x01000000U)
#define FLASH_OPTCR_nWRP_9                  ((uint32_t)0x02000000U)
#define FLASH_OPTCR_nWRP_10                 ((uint32_t)0x04000000U)
#define FLASH_OPTCR_nWRP_11                 ((uint32_t)0x08000000U)
#define FLASH_OPTCR_DB1M                    ((uint32_t)0x40000000U)
#define FLASH_OPTCR_SPRMOD                  ((uint32_t)0x80000000U)

/******************  Bits definition for FLASH_OPTCR1 register  ***************/
#define FLASH_OPTCR1_nWRP                    ((uint32_t)0x0FFF0000U)
#define FLASH_OPTCR1_nWRP_0                  ((uint32_t)0x00010000U)
#define FLASH_OPTCR1_nWRP_1                  ((uint32_t)0x00020000U)
#define FLASH_OPTCR1_nWRP_2                  ((uint32_t)0x00040000U)
#define FLASH_OPTCR1_nWRP_3                  ((uint32_t)0x00080000U)
#define FLASH_OPTCR1_nWRP_4                  ((uint32_t)0x00100000U)
#define FLASH_OPTCR1_nWRP_5                  ((uint32_t)0x00200000U)
#define FLASH_OPTCR1_nWRP_6                  ((uint32_t)0x00400000U)
#define FLASH_OPTCR1_nWRP_7                  ((uint32_t)0x00800000U)
#define FLASH_OPTCR1_nWRP_8                  ((uint32_t)0x01000000U)
#define FLASH_OPTCR1_nWRP_9                  ((uint32_t)0x02000000U)
#define FLASH_OPTCR1_nWRP_10                 ((uint32_t)0x04000000U)
#define FLASH_OPTCR1_nWRP_11                 ((uint32_t)0x08000000U)

/******************************************************************************/
/*                                                                            */
/*                          Flexible Memory Controller                        */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for FMC_BCR1 register  *******************/
#define  FMC_BCR1_MBKEN                     ((uint32_t)0x00000001U)        /*!<Memory bank enable bit                 */
#define  FMC_BCR1_MUXEN                     ((uint32_t)0x00000002U)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR1_MTYP                      ((uint32_t)0x0000000CU)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR1_MTYP_0                    ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  FMC_BCR1_MTYP_1                    ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  FMC_BCR1_MWID                      ((uint32_t)0x00000030U)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR1_MWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BCR1_MWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_BCR1_FACCEN                    ((uint32_t)0x00000040U)        /*!<Flash access enable        */
#define  FMC_BCR1_BURSTEN                   ((uint32_t)0x00000100U)        /*!<Burst enable bit           */
#define  FMC_BCR1_WAITPOL                   ((uint32_t)0x00000200U)        /*!<Wait signal polarity bit   */
#define  FMC_BCR1_WRAPMOD                   ((uint32_t)0x00000400U)        /*!<Wrapped burst mode support */
#define  FMC_BCR1_WAITCFG                   ((uint32_t)0x00000800U)        /*!<Wait timing configuration  */
#define  FMC_BCR1_WREN                      ((uint32_t)0x00001000U)        /*!<Write enable bit           */
#define  FMC_BCR1_WAITEN                    ((uint32_t)0x00002000U)        /*!<Wait enable bit            */
#define  FMC_BCR1_EXTMOD                    ((uint32_t)0x00004000U)        /*!<Extended mode enable       */
#define  FMC_BCR1_ASYNCWAIT                 ((uint32_t)0x00008000U)        /*!<Asynchronous wait          */
#define  FMC_BCR1_CPSIZE                    ((uint32_t)0x00070000U)        /*!<CRAM page size */
#define  FMC_BCR1_CPSIZE_0                  ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BCR1_CPSIZE_1                  ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BCR1_CPSIZE_2                  ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BCR1_CBURSTRW                  ((uint32_t)0x00080000U)        /*!<Write burst enable         */
#define  FMC_BCR1_CCLKEN                    ((uint32_t)0x00100000U)        /*!<Continous clock enable     */

/******************  Bit definition for FMC_BCR2 register  *******************/
#define  FMC_BCR2_MBKEN                     ((uint32_t)0x00000001U)        /*!<Memory bank enable bit                 */
#define  FMC_BCR2_MUXEN                     ((uint32_t)0x00000002U)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR2_MTYP                      ((uint32_t)0x0000000CU)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR2_MTYP_0                    ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  FMC_BCR2_MTYP_1                    ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  FMC_BCR2_MWID                      ((uint32_t)0x00000030U)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR2_MWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BCR2_MWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_BCR2_FACCEN                    ((uint32_t)0x00000040U)        /*!<Flash access enable        */
#define  FMC_BCR2_BURSTEN                   ((uint32_t)0x00000100U)        /*!<Burst enable bit           */
#define  FMC_BCR2_WAITPOL                   ((uint32_t)0x00000200U)        /*!<Wait signal polarity bit   */
#define  FMC_BCR2_WRAPMOD                   ((uint32_t)0x00000400U)        /*!<Wrapped burst mode support */
#define  FMC_BCR2_WAITCFG                   ((uint32_t)0x00000800U)        /*!<Wait timing configuration  */
#define  FMC_BCR2_WREN                      ((uint32_t)0x00001000U)        /*!<Write enable bit           */
#define  FMC_BCR2_WAITEN                    ((uint32_t)0x00002000U)        /*!<Wait enable bit            */
#define  FMC_BCR2_EXTMOD                    ((uint32_t)0x00004000U)        /*!<Extended mode enable       */
#define  FMC_BCR2_ASYNCWAIT                 ((uint32_t)0x00008000U)        /*!<Asynchronous wait          */
#define  FMC_BCR2_CPSIZE                    ((uint32_t)0x00070000U)        /*!<CRAM page size */
#define  FMC_BCR2_CPSIZE_0                  ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BCR2_CPSIZE_1                  ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BCR2_CPSIZE_2                  ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BCR2_CBURSTRW                  ((uint32_t)0x00080000U)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR3 register  *******************/
#define  FMC_BCR3_MBKEN                     ((uint32_t)0x00000001U)        /*!<Memory bank enable bit                 */
#define  FMC_BCR3_MUXEN                     ((uint32_t)0x00000002U)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR3_MTYP                      ((uint32_t)0x0000000CU)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR3_MTYP_0                    ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  FMC_BCR3_MTYP_1                    ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  FMC_BCR3_MWID                      ((uint32_t)0x00000030U)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR3_MWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BCR3_MWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_BCR3_FACCEN                    ((uint32_t)0x00000040U)        /*!<Flash access enable        */
#define  FMC_BCR3_BURSTEN                   ((uint32_t)0x00000100U)        /*!<Burst enable bit           */
#define  FMC_BCR3_WAITPOL                   ((uint32_t)0x00000200U)        /*!<Wait signal polarity bit   */
#define  FMC_BCR3_WRAPMOD                   ((uint32_t)0x00000400U)        /*!<Wrapped burst mode support */
#define  FMC_BCR3_WAITCFG                   ((uint32_t)0x00000800U)        /*!<Wait timing configuration  */
#define  FMC_BCR3_WREN                      ((uint32_t)0x00001000U)        /*!<Write enable bit           */
#define  FMC_BCR3_WAITEN                    ((uint32_t)0x00002000U)        /*!<Wait enable bit            */
#define  FMC_BCR3_EXTMOD                    ((uint32_t)0x00004000U)        /*!<Extended mode enable       */
#define  FMC_BCR3_ASYNCWAIT                 ((uint32_t)0x00008000U)        /*!<Asynchronous wait          */
#define  FMC_BCR3_CPSIZE                    ((uint32_t)0x00070000U)        /*!<CRAM page size */
#define  FMC_BCR3_CPSIZE_0                  ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BCR3_CPSIZE_1                  ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BCR3_CPSIZE_2                  ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BCR3_CBURSTRW                  ((uint32_t)0x00080000U)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR4 register  *******************/
#define  FMC_BCR4_MBKEN                     ((uint32_t)0x00000001U)        /*!<Memory bank enable bit                 */
#define  FMC_BCR4_MUXEN                     ((uint32_t)0x00000002U)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR4_MTYP                      ((uint32_t)0x0000000CU)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR4_MTYP_0                    ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  FMC_BCR4_MTYP_1                    ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  FMC_BCR4_MWID                      ((uint32_t)0x00000030U)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR4_MWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BCR4_MWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_BCR4_FACCEN                    ((uint32_t)0x00000040U)        /*!<Flash access enable        */
#define  FMC_BCR4_BURSTEN                   ((uint32_t)0x00000100U)        /*!<Burst enable bit           */
#define  FMC_BCR4_WAITPOL                   ((uint32_t)0x00000200U)        /*!<Wait signal polarity bit   */
#define  FMC_BCR4_WRAPMOD                   ((uint32_t)0x00000400U)        /*!<Wrapped burst mode support */
#define  FMC_BCR4_WAITCFG                   ((uint32_t)0x00000800U)        /*!<Wait timing configuration  */
#define  FMC_BCR4_WREN                      ((uint32_t)0x00001000U)        /*!<Write enable bit           */
#define  FMC_BCR4_WAITEN                    ((uint32_t)0x00002000U)        /*!<Wait enable bit            */
#define  FMC_BCR4_EXTMOD                    ((uint32_t)0x00004000U)        /*!<Extended mode enable       */
#define  FMC_BCR4_ASYNCWAIT                 ((uint32_t)0x00008000U)        /*!<Asynchronous wait          */
#define  FMC_BCR4_CPSIZE                    ((uint32_t)0x00070000U)        /*!<CRAM page size */
#define  FMC_BCR4_CPSIZE_0                  ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BCR4_CPSIZE_1                  ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BCR4_CPSIZE_2                  ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BCR4_CBURSTRW                  ((uint32_t)0x00080000U)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BTR1 register  ******************/
#define  FMC_BTR1_ADDSET                    ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR1_ADDSET_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BTR1_ADDSET_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BTR1_ADDSET_2                  ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BTR1_ADDSET_3                  ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BTR1_ADDHLD                    ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration)  */
#define  FMC_BTR1_ADDHLD_0                  ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BTR1_ADDHLD_1                  ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BTR1_ADDHLD_2                  ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BTR1_ADDHLD_3                  ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BTR1_DATAST                    ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR1_DATAST_0                  ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BTR1_DATAST_1                  ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BTR1_DATAST_2                  ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BTR1_DATAST_3                  ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BTR1_DATAST_4                  ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BTR1_DATAST_5                  ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BTR1_DATAST_6                  ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BTR1_DATAST_7                  ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BTR1_BUSTURN                   ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR1_BUSTURN_0                 ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BTR1_BUSTURN_1                 ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BTR1_BUSTURN_2                 ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BTR1_BUSTURN_3                 ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BTR1_CLKDIV                    ((uint32_t)0x00F00000U)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR1_CLKDIV_0                  ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  FMC_BTR1_CLKDIV_1                  ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  FMC_BTR1_CLKDIV_2                  ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  FMC_BTR1_CLKDIV_3                  ((uint32_t)0x00800000U)        /*!<Bit 3 */

#define  FMC_BTR1_DATLAT                    ((uint32_t)0x0F000000U)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR1_DATLAT_0                  ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_BTR1_DATLAT_1                  ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_BTR1_DATLAT_2                  ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_BTR1_DATLAT_3                  ((uint32_t)0x08000000U)        /*!<Bit 3 */

#define  FMC_BTR1_ACCMOD                    ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR1_ACCMOD_0                  ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BTR1_ACCMOD_1                  ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR2 register  *******************/
#define  FMC_BTR2_ADDSET                    ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR2_ADDSET_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BTR2_ADDSET_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BTR2_ADDSET_2                  ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BTR2_ADDSET_3                  ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BTR2_ADDHLD                    ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR2_ADDHLD_0                  ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BTR2_ADDHLD_1                  ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BTR2_ADDHLD_2                  ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BTR2_ADDHLD_3                  ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BTR2_DATAST                    ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR2_DATAST_0                  ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BTR2_DATAST_1                  ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BTR2_DATAST_2                  ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BTR2_DATAST_3                  ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BTR2_DATAST_4                  ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BTR2_DATAST_5                  ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BTR2_DATAST_6                  ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BTR2_DATAST_7                  ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BTR2_BUSTURN                   ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR2_BUSTURN_0                 ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BTR2_BUSTURN_1                 ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BTR2_BUSTURN_2                 ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BTR2_BUSTURN_3                 ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BTR2_CLKDIV                    ((uint32_t)0x00F00000U)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR2_CLKDIV_0                  ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  FMC_BTR2_CLKDIV_1                  ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  FMC_BTR2_CLKDIV_2                  ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  FMC_BTR2_CLKDIV_3                  ((uint32_t)0x00800000U)        /*!<Bit 3 */

#define  FMC_BTR2_DATLAT                    ((uint32_t)0x0F000000U)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR2_DATLAT_0                  ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_BTR2_DATLAT_1                  ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_BTR2_DATLAT_2                  ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_BTR2_DATLAT_3                  ((uint32_t)0x08000000U)        /*!<Bit 3 */

#define  FMC_BTR2_ACCMOD                    ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR2_ACCMOD_0                  ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BTR2_ACCMOD_1                  ((uint32_t)0x20000000U)        /*!<Bit 1 */

/*******************  Bit definition for FMC_BTR3 register  *******************/
#define  FMC_BTR3_ADDSET                    ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR3_ADDSET_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BTR3_ADDSET_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BTR3_ADDSET_2                  ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BTR3_ADDSET_3                  ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BTR3_ADDHLD                    ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR3_ADDHLD_0                  ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BTR3_ADDHLD_1                  ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BTR3_ADDHLD_2                  ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BTR3_ADDHLD_3                  ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BTR3_DATAST                    ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR3_DATAST_0                  ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BTR3_DATAST_1                  ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BTR3_DATAST_2                  ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BTR3_DATAST_3                  ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BTR3_DATAST_4                  ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BTR3_DATAST_5                  ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BTR3_DATAST_6                  ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BTR3_DATAST_7                  ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BTR3_BUSTURN                   ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR3_BUSTURN_0                 ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BTR3_BUSTURN_1                 ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BTR3_BUSTURN_2                 ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BTR3_BUSTURN_3                 ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BTR3_CLKDIV                    ((uint32_t)0x00F00000U)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR3_CLKDIV_0                  ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  FMC_BTR3_CLKDIV_1                  ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  FMC_BTR3_CLKDIV_2                  ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  FMC_BTR3_CLKDIV_3                  ((uint32_t)0x00800000U)        /*!<Bit 3 */

#define  FMC_BTR3_DATLAT                    ((uint32_t)0x0F000000U)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR3_DATLAT_0                  ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_BTR3_DATLAT_1                  ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_BTR3_DATLAT_2                  ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_BTR3_DATLAT_3                  ((uint32_t)0x08000000U)        /*!<Bit 3 */

#define  FMC_BTR3_ACCMOD                    ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR3_ACCMOD_0                  ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BTR3_ACCMOD_1                  ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR4 register  *******************/
#define  FMC_BTR4_ADDSET                    ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR4_ADDSET_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BTR4_ADDSET_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BTR4_ADDSET_2                  ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BTR4_ADDSET_3                  ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BTR4_ADDHLD                    ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR4_ADDHLD_0                  ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BTR4_ADDHLD_1                  ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BTR4_ADDHLD_2                  ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BTR4_ADDHLD_3                  ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BTR4_DATAST                    ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR4_DATAST_0                  ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BTR4_DATAST_1                  ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BTR4_DATAST_2                  ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BTR4_DATAST_3                  ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BTR4_DATAST_4                  ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BTR4_DATAST_5                  ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BTR4_DATAST_6                  ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BTR4_DATAST_7                  ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BTR4_BUSTURN                   ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR4_BUSTURN_0                 ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BTR4_BUSTURN_1                 ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BTR4_BUSTURN_2                 ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BTR4_BUSTURN_3                 ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BTR4_CLKDIV                    ((uint32_t)0x00F00000U)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR4_CLKDIV_0                  ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  FMC_BTR4_CLKDIV_1                  ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  FMC_BTR4_CLKDIV_2                  ((uint32_t)0x00400000U)        /*!<Bit 2 */
#define  FMC_BTR4_CLKDIV_3                  ((uint32_t)0x00800000U)        /*!<Bit 3 */

#define  FMC_BTR4_DATLAT                    ((uint32_t)0x0F000000U)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR4_DATLAT_0                  ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_BTR4_DATLAT_1                  ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_BTR4_DATLAT_2                  ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_BTR4_DATLAT_3                  ((uint32_t)0x08000000U)        /*!<Bit 3 */

#define  FMC_BTR4_ACCMOD                    ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR4_ACCMOD_0                  ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BTR4_ACCMOD_1                  ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR1 register  ******************/
#define  FMC_BWTR1_ADDSET                   ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR1_ADDSET_0                 ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BWTR1_ADDSET_1                 ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BWTR1_ADDSET_2                 ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BWTR1_ADDSET_3                 ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BWTR1_ADDHLD                   ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR1_ADDHLD_0                 ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BWTR1_ADDHLD_1                 ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BWTR1_ADDHLD_2                 ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BWTR1_ADDHLD_3                 ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BWTR1_DATAST                   ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR1_DATAST_0                 ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BWTR1_DATAST_1                 ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BWTR1_DATAST_2                 ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BWTR1_DATAST_3                 ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BWTR1_DATAST_4                 ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BWTR1_DATAST_5                 ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BWTR1_DATAST_6                 ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BWTR1_DATAST_7                 ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BWTR1_BUSTURN                  ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define  FMC_BWTR1_BUSTURN_0                ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BWTR1_BUSTURN_1                ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BWTR1_BUSTURN_2                ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BWTR1_BUSTURN_3                ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BWTR1_ACCMOD                   ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR1_ACCMOD_0                 ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BWTR1_ACCMOD_1                 ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR2 register  ******************/
#define  FMC_BWTR2_ADDSET                   ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR2_ADDSET_0                 ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BWTR2_ADDSET_1                 ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BWTR2_ADDSET_2                 ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BWTR2_ADDSET_3                 ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BWTR2_ADDHLD                   ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR2_ADDHLD_0                 ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BWTR2_ADDHLD_1                 ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BWTR2_ADDHLD_2                 ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BWTR2_ADDHLD_3                 ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BWTR2_DATAST                   ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR2_DATAST_0                 ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BWTR2_DATAST_1                 ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BWTR2_DATAST_2                 ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BWTR2_DATAST_3                 ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BWTR2_DATAST_4                 ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BWTR2_DATAST_5                 ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BWTR2_DATAST_6                 ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BWTR2_DATAST_7                 ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BWTR2_BUSTURN                  ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define  FMC_BWTR2_BUSTURN_0                ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BWTR2_BUSTURN_1                ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BWTR2_BUSTURN_2                ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BWTR2_BUSTURN_3                ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BWTR2_ACCMOD                   ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR2_ACCMOD_0                 ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BWTR2_ACCMOD_1                 ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR3 register  ******************/
#define  FMC_BWTR3_ADDSET                   ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR3_ADDSET_0                 ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BWTR3_ADDSET_1                 ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BWTR3_ADDSET_2                 ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BWTR3_ADDSET_3                 ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BWTR3_ADDHLD                   ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR3_ADDHLD_0                 ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BWTR3_ADDHLD_1                 ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BWTR3_ADDHLD_2                 ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BWTR3_ADDHLD_3                 ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BWTR3_DATAST                   ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR3_DATAST_0                 ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BWTR3_DATAST_1                 ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BWTR3_DATAST_2                 ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BWTR3_DATAST_3                 ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BWTR3_DATAST_4                 ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BWTR3_DATAST_5                 ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BWTR3_DATAST_6                 ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BWTR3_DATAST_7                 ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BWTR3_BUSTURN                  ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define  FMC_BWTR3_BUSTURN_0                ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BWTR3_BUSTURN_1                ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BWTR3_BUSTURN_2                ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BWTR3_BUSTURN_3                ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BWTR3_ACCMOD                   ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR3_ACCMOD_0                 ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BWTR3_ACCMOD_1                 ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR4 register  ******************/
#define  FMC_BWTR4_ADDSET                   ((uint32_t)0x0000000FU)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR4_ADDSET_0                 ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_BWTR4_ADDSET_1                 ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_BWTR4_ADDSET_2                 ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_BWTR4_ADDSET_3                 ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_BWTR4_ADDHLD                   ((uint32_t)0x000000F0U)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR4_ADDHLD_0                 ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_BWTR4_ADDHLD_1                 ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_BWTR4_ADDHLD_2                 ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_BWTR4_ADDHLD_3                 ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_BWTR4_DATAST                   ((uint32_t)0x0000FF00U)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR4_DATAST_0                 ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_BWTR4_DATAST_1                 ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_BWTR4_DATAST_2                 ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_BWTR4_DATAST_3                 ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_BWTR4_DATAST_4                 ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_BWTR4_DATAST_5                 ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_BWTR4_DATAST_6                 ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_BWTR4_DATAST_7                 ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_BWTR4_BUSTURN                  ((uint32_t)0x000F0000U)        /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define  FMC_BWTR4_BUSTURN_0                ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_BWTR4_BUSTURN_1                ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_BWTR4_BUSTURN_2                ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_BWTR4_BUSTURN_3                ((uint32_t)0x00080000U)        /*!<Bit 3 */

#define  FMC_BWTR4_ACCMOD                   ((uint32_t)0x30000000U)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR4_ACCMOD_0                 ((uint32_t)0x10000000U)        /*!<Bit 0 */
#define  FMC_BWTR4_ACCMOD_1                 ((uint32_t)0x20000000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_PCR2 register  *******************/
#define  FMC_PCR2_PWAITEN                   ((uint32_t)0x00000002U)        /*!<Wait feature enable bit                   */
#define  FMC_PCR2_PBKEN                     ((uint32_t)0x00000004U)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR2_PTYP                      ((uint32_t)0x00000008U)        /*!<Memory type                               */

#define  FMC_PCR2_PWID                      ((uint32_t)0x00000030U)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR2_PWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_PCR2_PWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_PCR2_ECCEN                     ((uint32_t)0x00000040U)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR2_TCLR                      ((uint32_t)0x00001E00U)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR2_TCLR_0                    ((uint32_t)0x00000200U)        /*!<Bit 0 */
#define  FMC_PCR2_TCLR_1                    ((uint32_t)0x00000400U)        /*!<Bit 1 */
#define  FMC_PCR2_TCLR_2                    ((uint32_t)0x00000800U)        /*!<Bit 2 */
#define  FMC_PCR2_TCLR_3                    ((uint32_t)0x00001000U)        /*!<Bit 3 */

#define  FMC_PCR2_TAR                       ((uint32_t)0x0001E000U)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR2_TAR_0                     ((uint32_t)0x00002000U)        /*!<Bit 0 */
#define  FMC_PCR2_TAR_1                     ((uint32_t)0x00004000U)        /*!<Bit 1 */
#define  FMC_PCR2_TAR_2                     ((uint32_t)0x00008000U)        /*!<Bit 2 */
#define  FMC_PCR2_TAR_3                     ((uint32_t)0x00010000U)        /*!<Bit 3 */

#define  FMC_PCR2_ECCPS                     ((uint32_t)0x000E0000U)        /*!<ECCPS[1:0] bits (ECC page size)           */
#define  FMC_PCR2_ECCPS_0                   ((uint32_t)0x00020000U)        /*!<Bit 0 */
#define  FMC_PCR2_ECCPS_1                   ((uint32_t)0x00040000U)        /*!<Bit 1 */
#define  FMC_PCR2_ECCPS_2                   ((uint32_t)0x00080000U)        /*!<Bit 2 */

/******************  Bit definition for FMC_PCR3 register  *******************/
#define  FMC_PCR3_PWAITEN                   ((uint32_t)0x00000002U)        /*!<Wait feature enable bit                   */
#define  FMC_PCR3_PBKEN                     ((uint32_t)0x00000004U)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR3_PTYP                      ((uint32_t)0x00000008U)        /*!<Memory type                               */

#define  FMC_PCR3_PWID                      ((uint32_t)0x00000030U)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR3_PWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_PCR3_PWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_PCR3_ECCEN                     ((uint32_t)0x00000040U)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR3_TCLR                      ((uint32_t)0x00001E00U)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR3_TCLR_0                    ((uint32_t)0x00000200U)        /*!<Bit 0 */
#define  FMC_PCR3_TCLR_1                    ((uint32_t)0x00000400U)        /*!<Bit 1 */
#define  FMC_PCR3_TCLR_2                    ((uint32_t)0x00000800U)        /*!<Bit 2 */
#define  FMC_PCR3_TCLR_3                    ((uint32_t)0x00001000U)        /*!<Bit 3 */

#define  FMC_PCR3_TAR                       ((uint32_t)0x0001E000U)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR3_TAR_0                     ((uint32_t)0x00002000U)        /*!<Bit 0 */
#define  FMC_PCR3_TAR_1                     ((uint32_t)0x00004000U)        /*!<Bit 1 */
#define  FMC_PCR3_TAR_2                     ((uint32_t)0x00008000U)        /*!<Bit 2 */
#define  FMC_PCR3_TAR_3                     ((uint32_t)0x00010000U)        /*!<Bit 3 */

#define  FMC_PCR3_ECCPS                     ((uint32_t)0x000E0000U)        /*!<ECCPS[2:0] bits (ECC page size)           */
#define  FMC_PCR3_ECCPS_0                   ((uint32_t)0x00020000U)        /*!<Bit 0 */
#define  FMC_PCR3_ECCPS_1                   ((uint32_t)0x00040000U)        /*!<Bit 1 */
#define  FMC_PCR3_ECCPS_2                   ((uint32_t)0x00080000U)        /*!<Bit 2 */

/******************  Bit definition for FMC_PCR4 register  *******************/
#define  FMC_PCR4_PWAITEN                   ((uint32_t)0x00000002U)        /*!<Wait feature enable bit                   */
#define  FMC_PCR4_PBKEN                     ((uint32_t)0x00000004U)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR4_PTYP                      ((uint32_t)0x00000008U)        /*!<Memory type                               */

#define  FMC_PCR4_PWID                      ((uint32_t)0x00000030U)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR4_PWID_0                    ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_PCR4_PWID_1                    ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_PCR4_ECCEN                     ((uint32_t)0x00000040U)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR4_TCLR                      ((uint32_t)0x00001E00U)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR4_TCLR_0                    ((uint32_t)0x00000200U)        /*!<Bit 0 */
#define  FMC_PCR4_TCLR_1                    ((uint32_t)0x00000400U)        /*!<Bit 1 */
#define  FMC_PCR4_TCLR_2                    ((uint32_t)0x00000800U)        /*!<Bit 2 */
#define  FMC_PCR4_TCLR_3                    ((uint32_t)0x00001000U)        /*!<Bit 3 */

#define  FMC_PCR4_TAR                       ((uint32_t)0x0001E000U)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR4_TAR_0                     ((uint32_t)0x00002000U)        /*!<Bit 0 */
#define  FMC_PCR4_TAR_1                     ((uint32_t)0x00004000U)        /*!<Bit 1 */
#define  FMC_PCR4_TAR_2                     ((uint32_t)0x00008000U)        /*!<Bit 2 */
#define  FMC_PCR4_TAR_3                     ((uint32_t)0x00010000U)        /*!<Bit 3 */

#define  FMC_PCR4_ECCPS                     ((uint32_t)0x000E0000U)        /*!<ECCPS[2:0] bits (ECC page size)           */
#define  FMC_PCR4_ECCPS_0                   ((uint32_t)0x00020000U)        /*!<Bit 0 */
#define  FMC_PCR4_ECCPS_1                   ((uint32_t)0x00040000U)        /*!<Bit 1 */
#define  FMC_PCR4_ECCPS_2                   ((uint32_t)0x00080000U)        /*!<Bit 2 */

/*******************  Bit definition for FMC_SR2 register  *******************/
#define  FMC_SR2_IRS                        ((uint32_t)0x01U)               /*!<Interrupt Rising Edge status                */
#define  FMC_SR2_ILS                        ((uint32_t)0x02U)               /*!<Interrupt Level status                      */
#define  FMC_SR2_IFS                        ((uint32_t)0x04U)               /*!<Interrupt Falling Edge status               */
#define  FMC_SR2_IREN                       ((uint32_t)0x08U)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR2_ILEN                       ((uint32_t)0x10U)               /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR2_IFEN                       ((uint32_t)0x20U)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR2_FEMPT                      ((uint32_t)0x40U)               /*!<FIFO empty                                  */

/*******************  Bit definition for FMC_SR3 register  *******************/
#define  FMC_SR3_IRS                        ((uint32_t)0x01U)               /*!<Interrupt Rising Edge status                */
#define  FMC_SR3_ILS                        ((uint32_t)0x02U)               /*!<Interrupt Level status                      */
#define  FMC_SR3_IFS                        ((uint32_t)0x04U)               /*!<Interrupt Falling Edge status               */
#define  FMC_SR3_IREN                       ((uint32_t)0x08U)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR3_ILEN                       ((uint32_t)0x10U)               /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR3_IFEN                       ((uint32_t)0x20U)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR3_FEMPT                      ((uint32_t)0x40U)               /*!<FIFO empty                                  */

/*******************  Bit definition for FMC_SR4 register  *******************/
#define  FMC_SR4_IRS                        ((uint32_t)0x01U)               /*!<Interrupt Rising Edge status                */
#define  FMC_SR4_ILS                        ((uint32_t)0x02U)               /*!<Interrupt Level status                      */
#define  FMC_SR4_IFS                        ((uint32_t)0x04U)               /*!<Interrupt Falling Edge status               */
#define  FMC_SR4_IREN                       ((uint32_t)0x08U)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR4_ILEN                       ((uint32_t)0x10U)               /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR4_IFEN                       ((uint32_t)0x20U)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR4_FEMPT                      ((uint32_t)0x40U)               /*!<FIFO empty                                  */

/******************  Bit definition for FMC_PMEM2 register  ******************/
#define  FMC_PMEM2_MEMSET2                  ((uint32_t)0x000000FFU)        /*!<MEMSET2[7:0] bits (Common memory 2 setup time) */
#define  FMC_PMEM2_MEMSET2_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMSET2_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMSET2_2                ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMSET2_3                ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMSET2_4                ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMSET2_5                ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMSET2_6                ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMSET2_7                ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PMEM2_MEMWAIT2                 ((uint32_t)0x0000FF00U)        /*!<MEMWAIT2[7:0] bits (Common memory 2 wait time) */
#define  FMC_PMEM2_MEMWAIT2_0               ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMWAIT2_1               ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMWAIT2_2               ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMWAIT2_3               ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMWAIT2_4               ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMWAIT2_5               ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMWAIT2_6               ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMWAIT2_7               ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PMEM2_MEMHOLD2                 ((uint32_t)0x00FF0000U)        /*!<MEMHOLD2[7:0] bits (Common memory 2 hold time) */
#define  FMC_PMEM2_MEMHOLD2_0               ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMHOLD2_1               ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMHOLD2_2               ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMHOLD2_3               ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMHOLD2_4               ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMHOLD2_5               ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMHOLD2_6               ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMHOLD2_7               ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PMEM2_MEMHIZ2                  ((uint32_t)0xFF000000U)        /*!<MEMHIZ2[7:0] bits (Common memory 2 databus HiZ time) */
#define  FMC_PMEM2_MEMHIZ2_0                ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMHIZ2_1                ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMHIZ2_2                ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMHIZ2_3                ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMHIZ2_4                ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMHIZ2_5                ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMHIZ2_6                ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMHIZ2_7                ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_PMEM3 register  ******************/
#define  FMC_PMEM3_MEMSET3                  ((uint32_t)0x000000FFU)        /*!<MEMSET3[7:0] bits (Common memory 3 setup time) */
#define  FMC_PMEM3_MEMSET3_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMSET3_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMSET3_2                ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMSET3_3                ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMSET3_4                ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMSET3_5                ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMSET3_6                ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMSET3_7                ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PMEM3_MEMWAIT3                 ((uint32_t)0x0000FF00U)        /*!<MEMWAIT3[7:0] bits (Common memory 3 wait time) */
#define  FMC_PMEM3_MEMWAIT3_0               ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMWAIT3_1               ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMWAIT3_2               ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMWAIT3_3               ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMWAIT3_4               ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMWAIT3_5               ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMWAIT3_6               ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMWAIT3_7               ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PMEM3_MEMHOLD3                 ((uint32_t)0x00FF0000U)        /*!<MEMHOLD3[7:0] bits (Common memory 3 hold time) */
#define  FMC_PMEM3_MEMHOLD3_0               ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMHOLD3_1               ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMHOLD3_2               ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMHOLD3_3               ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMHOLD3_4               ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMHOLD3_5               ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMHOLD3_6               ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMHOLD3_7               ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PMEM3_MEMHIZ3                  ((uint32_t)0xFF000000U)        /*!<MEMHIZ3[7:0] bits (Common memory 3 databus HiZ time) */
#define  FMC_PMEM3_MEMHIZ3_0                ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMHIZ3_1                ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMHIZ3_2                ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMHIZ3_3                ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMHIZ3_4                ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMHIZ3_5                ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMHIZ3_6                ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMHIZ3_7                ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_PMEM4 register  ******************/
#define  FMC_PMEM4_MEMSET4                  ((uint32_t)0x000000FFU)        /*!<MEMSET4[7:0] bits (Common memory 4 setup time) */
#define  FMC_PMEM4_MEMSET4_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMSET4_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMSET4_2                ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMSET4_3                ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMSET4_4                ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMSET4_5                ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMSET4_6                ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMSET4_7                ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PMEM4_MEMWAIT4                 ((uint32_t)0x0000FF00U)        /*!<MEMWAIT4[7:0] bits (Common memory 4 wait time) */
#define  FMC_PMEM4_MEMWAIT4_0               ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMWAIT4_1               ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMWAIT4_2               ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMWAIT4_3               ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMWAIT4_4               ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMWAIT4_5               ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMWAIT4_6               ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMWAIT4_7               ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PMEM4_MEMHOLD4                 ((uint32_t)0x00FF0000U)        /*!<MEMHOLD4[7:0] bits (Common memory 4 hold time) */
#define  FMC_PMEM4_MEMHOLD4_0               ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMHOLD4_1               ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMHOLD4_2               ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMHOLD4_3               ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMHOLD4_4               ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMHOLD4_5               ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMHOLD4_6               ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMHOLD4_7               ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PMEM4_MEMHIZ4                  ((uint32_t)0xFF000000U)        /*!<MEMHIZ4[7:0] bits (Common memory 4 databus HiZ time) */
#define  FMC_PMEM4_MEMHIZ4_0                ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMHIZ4_1                ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMHIZ4_2                ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMHIZ4_3                ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMHIZ4_4                ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMHIZ4_5                ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMHIZ4_6                ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMHIZ4_7                ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT2 register  ******************/
#define  FMC_PATT2_ATTSET2                  ((uint32_t)0x000000FFU)        /*!<ATTSET2[7:0] bits (Attribute memory 2 setup time) */
#define  FMC_PATT2_ATTSET2_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PATT2_ATTSET2_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PATT2_ATTSET2_2                ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PATT2_ATTSET2_3                ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PATT2_ATTSET2_4                ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PATT2_ATTSET2_5                ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PATT2_ATTSET2_6                ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PATT2_ATTSET2_7                ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PATT2_ATTWAIT2                 ((uint32_t)0x0000FF00U)        /*!<ATTWAIT2[7:0] bits (Attribute memory 2 wait time) */
#define  FMC_PATT2_ATTWAIT2_0               ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PATT2_ATTWAIT2_1               ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PATT2_ATTWAIT2_2               ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PATT2_ATTWAIT2_3               ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PATT2_ATTWAIT2_4               ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PATT2_ATTWAIT2_5               ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PATT2_ATTWAIT2_6               ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PATT2_ATTWAIT2_7               ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PATT2_ATTHOLD2                 ((uint32_t)0x00FF0000U)        /*!<ATTHOLD2[7:0] bits (Attribute memory 2 hold time) */
#define  FMC_PATT2_ATTHOLD2_0               ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PATT2_ATTHOLD2_1               ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PATT2_ATTHOLD2_2               ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PATT2_ATTHOLD2_3               ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PATT2_ATTHOLD2_4               ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PATT2_ATTHOLD2_5               ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PATT2_ATTHOLD2_6               ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PATT2_ATTHOLD2_7               ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PATT2_ATTHIZ2                  ((uint32_t)0xFF000000U)        /*!<ATTHIZ2[7:0] bits (Attribute memory 2 databus HiZ time) */
#define  FMC_PATT2_ATTHIZ2_0                ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PATT2_ATTHIZ2_1                ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PATT2_ATTHIZ2_2                ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PATT2_ATTHIZ2_3                ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PATT2_ATTHIZ2_4                ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PATT2_ATTHIZ2_5                ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PATT2_ATTHIZ2_6                ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PATT2_ATTHIZ2_7                ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT3 register  ******************/
#define  FMC_PATT3_ATTSET3                  ((uint32_t)0x000000FFU)        /*!<ATTSET3[7:0] bits (Attribute memory 3 setup time) */
#define  FMC_PATT3_ATTSET3_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PATT3_ATTSET3_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PATT3_ATTSET3_2                ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PATT3_ATTSET3_3                ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PATT3_ATTSET3_4                ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PATT3_ATTSET3_5                ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PATT3_ATTSET3_6                ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PATT3_ATTSET3_7                ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PATT3_ATTWAIT3                 ((uint32_t)0x0000FF00U)        /*!<ATTWAIT3[7:0] bits (Attribute memory 3 wait time) */
#define  FMC_PATT3_ATTWAIT3_0               ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PATT3_ATTWAIT3_1               ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PATT3_ATTWAIT3_2               ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PATT3_ATTWAIT3_3               ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PATT3_ATTWAIT3_4               ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PATT3_ATTWAIT3_5               ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PATT3_ATTWAIT3_6               ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PATT3_ATTWAIT3_7               ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PATT3_ATTHOLD3                 ((uint32_t)0x00FF0000U)        /*!<ATTHOLD3[7:0] bits (Attribute memory 3 hold time) */
#define  FMC_PATT3_ATTHOLD3_0               ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PATT3_ATTHOLD3_1               ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PATT3_ATTHOLD3_2               ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PATT3_ATTHOLD3_3               ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PATT3_ATTHOLD3_4               ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PATT3_ATTHOLD3_5               ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PATT3_ATTHOLD3_6               ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PATT3_ATTHOLD3_7               ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PATT3_ATTHIZ3                  ((uint32_t)0xFF000000U)        /*!<ATTHIZ3[7:0] bits (Attribute memory 3 databus HiZ time) */
#define  FMC_PATT3_ATTHIZ3_0                ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PATT3_ATTHIZ3_1                ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PATT3_ATTHIZ3_2                ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PATT3_ATTHIZ3_3                ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PATT3_ATTHIZ3_4                ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PATT3_ATTHIZ3_5                ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PATT3_ATTHIZ3_6                ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PATT3_ATTHIZ3_7                ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT4 register  ******************/
#define  FMC_PATT4_ATTSET4                  ((uint32_t)0x000000FFU)        /*!<ATTSET4[7:0] bits (Attribute memory 4 setup time) */
#define  FMC_PATT4_ATTSET4_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PATT4_ATTSET4_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PATT4_ATTSET4_2                ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PATT4_ATTSET4_3                ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PATT4_ATTSET4_4                ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PATT4_ATTSET4_5                ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PATT4_ATTSET4_6                ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PATT4_ATTSET4_7                ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PATT4_ATTWAIT4                 ((uint32_t)0x0000FF00U)        /*!<ATTWAIT4[7:0] bits (Attribute memory 4 wait time) */
#define  FMC_PATT4_ATTWAIT4_0               ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PATT4_ATTWAIT4_1               ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PATT4_ATTWAIT4_2               ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PATT4_ATTWAIT4_3               ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PATT4_ATTWAIT4_4               ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PATT4_ATTWAIT4_5               ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PATT4_ATTWAIT4_6               ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PATT4_ATTWAIT4_7               ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PATT4_ATTHOLD4                 ((uint32_t)0x00FF0000U)        /*!<ATTHOLD4[7:0] bits (Attribute memory 4 hold time) */
#define  FMC_PATT4_ATTHOLD4_0               ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PATT4_ATTHOLD4_1               ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PATT4_ATTHOLD4_2               ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PATT4_ATTHOLD4_3               ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PATT4_ATTHOLD4_4               ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PATT4_ATTHOLD4_5               ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PATT4_ATTHOLD4_6               ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PATT4_ATTHOLD4_7               ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PATT4_ATTHIZ4                  ((uint32_t)0xFF000000U)        /*!<ATTHIZ4[7:0] bits (Attribute memory 4 databus HiZ time) */
#define  FMC_PATT4_ATTHIZ4_0                ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PATT4_ATTHIZ4_1                ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PATT4_ATTHIZ4_2                ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PATT4_ATTHIZ4_3                ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PATT4_ATTHIZ4_4                ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PATT4_ATTHIZ4_5                ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PATT4_ATTHIZ4_6                ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PATT4_ATTHIZ4_7                ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_PIO4 register  *******************/
#define  FMC_PIO4_IOSET4                    ((uint32_t)0x000000FFU)        /*!<IOSET4[7:0] bits (I/O 4 setup time) */
#define  FMC_PIO4_IOSET4_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_PIO4_IOSET4_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_PIO4_IOSET4_2                  ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_PIO4_IOSET4_3                  ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  FMC_PIO4_IOSET4_4                  ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  FMC_PIO4_IOSET4_5                  ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  FMC_PIO4_IOSET4_6                  ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  FMC_PIO4_IOSET4_7                  ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  FMC_PIO4_IOWAIT4                   ((uint32_t)0x0000FF00U)        /*!<IOWAIT4[7:0] bits (I/O 4 wait time) */
#define  FMC_PIO4_IOWAIT4_0                 ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_PIO4_IOWAIT4_1                 ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_PIO4_IOWAIT4_2                 ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_PIO4_IOWAIT4_3                 ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  FMC_PIO4_IOWAIT4_4                 ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  FMC_PIO4_IOWAIT4_5                 ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  FMC_PIO4_IOWAIT4_6                 ((uint32_t)0x00004000U)        /*!<Bit 6 */
#define  FMC_PIO4_IOWAIT4_7                 ((uint32_t)0x00008000U)        /*!<Bit 7 */

#define  FMC_PIO4_IOHOLD4                   ((uint32_t)0x00FF0000U)        /*!<IOHOLD4[7:0] bits (I/O 4 hold time) */
#define  FMC_PIO4_IOHOLD4_0                 ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_PIO4_IOHOLD4_1                 ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_PIO4_IOHOLD4_2                 ((uint32_t)0x00040000U)        /*!<Bit 2 */
#define  FMC_PIO4_IOHOLD4_3                 ((uint32_t)0x00080000U)        /*!<Bit 3 */
#define  FMC_PIO4_IOHOLD4_4                 ((uint32_t)0x00100000U)        /*!<Bit 4 */
#define  FMC_PIO4_IOHOLD4_5                 ((uint32_t)0x00200000U)        /*!<Bit 5 */
#define  FMC_PIO4_IOHOLD4_6                 ((uint32_t)0x00400000U)        /*!<Bit 6 */
#define  FMC_PIO4_IOHOLD4_7                 ((uint32_t)0x00800000U)        /*!<Bit 7 */

#define  FMC_PIO4_IOHIZ4                    ((uint32_t)0xFF000000U)        /*!<IOHIZ4[7:0] bits (I/O 4 databus HiZ time) */
#define  FMC_PIO4_IOHIZ4_0                  ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_PIO4_IOHIZ4_1                  ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_PIO4_IOHIZ4_2                  ((uint32_t)0x04000000U)        /*!<Bit 2 */
#define  FMC_PIO4_IOHIZ4_3                  ((uint32_t)0x08000000U)        /*!<Bit 3 */
#define  FMC_PIO4_IOHIZ4_4                  ((uint32_t)0x10000000U)        /*!<Bit 4 */
#define  FMC_PIO4_IOHIZ4_5                  ((uint32_t)0x20000000U)        /*!<Bit 5 */
#define  FMC_PIO4_IOHIZ4_6                  ((uint32_t)0x40000000U)        /*!<Bit 6 */
#define  FMC_PIO4_IOHIZ4_7                  ((uint32_t)0x80000000U)        /*!<Bit 7 */

/******************  Bit definition for FMC_ECCR2 register  ******************/
#define  FMC_ECCR2_ECC2                     ((uint32_t)0xFFFFFFFFU)        /*!<ECC result */

/******************  Bit definition for FMC_ECCR3 register  ******************/
#define  FMC_ECCR3_ECC3                     ((uint32_t)0xFFFFFFFFU)        /*!<ECC result */

/******************  Bit definition for FMC_SDCR1 register  ******************/
#define  FMC_SDCR1_NC                       ((uint32_t)0x00000003U)        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR1_NC_0                     ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_SDCR1_NC_1                     ((uint32_t)0x00000002U)        /*!<Bit 1 */

#define  FMC_SDCR1_NR                       ((uint32_t)0x0000000CU)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_NR_0                     ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  FMC_SDCR1_NR_1                     ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  FMC_SDCR1_MWID                     ((uint32_t)0x00000030U)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_MWID_0                   ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_SDCR1_MWID_1                   ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_SDCR1_NB                       ((uint32_t)0x00000040U)        /*!<Number of internal bank */

#define  FMC_SDCR1_CAS                      ((uint32_t)0x00000180U)        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR1_CAS_0                    ((uint32_t)0x00000080U)        /*!<Bit 0 */
#define  FMC_SDCR1_CAS_1                    ((uint32_t)0x00000100U)        /*!<Bit 1 */

#define  FMC_SDCR1_WP                       ((uint32_t)0x00000200U)        /*!<Write protection */

#define  FMC_SDCR1_SDCLK                    ((uint32_t)0x00000C00U)        /*!<SDRAM clock configuration */
#define  FMC_SDCR1_SDCLK_0                  ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  FMC_SDCR1_SDCLK_1                  ((uint32_t)0x00000800U)        /*!<Bit 1 */

#define  FMC_SDCR1_RBURST                   ((uint32_t)0x00001000U)        /*!<Read burst */

#define  FMC_SDCR1_RPIPE                    ((uint32_t)0x00006000U)        /*!<Write protection */
#define  FMC_SDCR1_RPIPE_0                  ((uint32_t)0x00002000U)        /*!<Bit 0 */
#define  FMC_SDCR1_RPIPE_1                  ((uint32_t)0x00004000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_SDCR2 register  ******************/
#define  FMC_SDCR2_NC                       ((uint32_t)0x00000003U)        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR2_NC_0                     ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_SDCR2_NC_1                     ((uint32_t)0x00000002U)        /*!<Bit 1 */

#define  FMC_SDCR2_NR                       ((uint32_t)0x0000000CU)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_NR_0                     ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  FMC_SDCR2_NR_1                     ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  FMC_SDCR2_MWID                     ((uint32_t)0x00000030U)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_MWID_0                   ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_SDCR2_MWID_1                   ((uint32_t)0x00000020U)        /*!<Bit 1 */

#define  FMC_SDCR2_NB                       ((uint32_t)0x00000040U)        /*!<Number of internal bank */

#define  FMC_SDCR2_CAS                      ((uint32_t)0x00000180U)        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR2_CAS_0                    ((uint32_t)0x00000080U)        /*!<Bit 0 */
#define  FMC_SDCR2_CAS_1                    ((uint32_t)0x00000100U)        /*!<Bit 1 */

#define  FMC_SDCR2_WP                       ((uint32_t)0x00000200U)        /*!<Write protection */

#define  FMC_SDCR2_SDCLK                    ((uint32_t)0x00000C00U)        /*!<SDCLK[1:0] (SDRAM clock configuration) */
#define  FMC_SDCR2_SDCLK_0                  ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  FMC_SDCR2_SDCLK_1                  ((uint32_t)0x00000800U)        /*!<Bit 1 */

#define  FMC_SDCR2_RBURST                   ((uint32_t)0x00001000U)        /*!<Read burst */

#define  FMC_SDCR2_RPIPE                    ((uint32_t)0x00006000U)        /*!<RPIPE[1:0](Read pipe) */
#define  FMC_SDCR2_RPIPE_0                  ((uint32_t)0x00002000U)        /*!<Bit 0 */
#define  FMC_SDCR2_RPIPE_1                  ((uint32_t)0x00004000U)        /*!<Bit 1 */

/******************  Bit definition for FMC_SDTR1 register  ******************/
#define  FMC_SDTR1_TMRD                     ((uint32_t)0x0000000FU)        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR1_TMRD_0                   ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_SDTR1_TMRD_1                   ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_SDTR1_TMRD_2                   ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_SDTR1_TMRD_3                   ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_SDTR1_TXSR                     ((uint32_t)0x000000F0U)        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR1_TXSR_0                   ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_SDTR1_TXSR_1                   ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_SDTR1_TXSR_2                   ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_SDTR1_TXSR_3                   ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_SDTR1_TRAS                     ((uint32_t)0x00000F00U)        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR1_TRAS_0                   ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_SDTR1_TRAS_1                   ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_SDTR1_TRAS_2                   ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_SDTR1_TRAS_3                   ((uint32_t)0x00000800U)        /*!<Bit 3 */

#define  FMC_SDTR1_TRC                      ((uint32_t)0x0000F000U)        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR1_TRC_0                    ((uint32_t)0x00001000U)        /*!<Bit 0 */
#define  FMC_SDTR1_TRC_1                    ((uint32_t)0x00002000U)        /*!<Bit 1 */
#define  FMC_SDTR1_TRC_2                    ((uint32_t)0x00004000U)        /*!<Bit 2 */

#define  FMC_SDTR1_TWR                      ((uint32_t)0x000F0000U)        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR1_TWR_0                    ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_SDTR1_TWR_1                    ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_SDTR1_TWR_2                    ((uint32_t)0x00040000U)        /*!<Bit 2 */

#define  FMC_SDTR1_TRP                      ((uint32_t)0x00F00000U)        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR1_TRP_0                    ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  FMC_SDTR1_TRP_1                    ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  FMC_SDTR1_TRP_2                    ((uint32_t)0x00400000U)        /*!<Bit 2 */

#define  FMC_SDTR1_TRCD                     ((uint32_t)0x0F000000U)        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR1_TRCD_0                   ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_SDTR1_TRCD_1                   ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_SDTR1_TRCD_2                   ((uint32_t)0x04000000U)        /*!<Bit 2 */

/******************  Bit definition for FMC_SDTR2 register  ******************/
#define  FMC_SDTR2_TMRD                     ((uint32_t)0x0000000FU)        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR2_TMRD_0                   ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_SDTR2_TMRD_1                   ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_SDTR2_TMRD_2                   ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  FMC_SDTR2_TMRD_3                   ((uint32_t)0x00000008U)        /*!<Bit 3 */

#define  FMC_SDTR2_TXSR                     ((uint32_t)0x000000F0U)        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR2_TXSR_0                   ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  FMC_SDTR2_TXSR_1                   ((uint32_t)0x00000020U)        /*!<Bit 1 */
#define  FMC_SDTR2_TXSR_2                   ((uint32_t)0x00000040U)        /*!<Bit 2 */
#define  FMC_SDTR2_TXSR_3                   ((uint32_t)0x00000080U)        /*!<Bit 3 */

#define  FMC_SDTR2_TRAS                     ((uint32_t)0x00000F00U)        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR2_TRAS_0                   ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  FMC_SDTR2_TRAS_1                   ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  FMC_SDTR2_TRAS_2                   ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  FMC_SDTR2_TRAS_3                   ((uint32_t)0x00000800U)        /*!<Bit 3 */

#define  FMC_SDTR2_TRC                      ((uint32_t)0x0000F000U)        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR2_TRC_0                    ((uint32_t)0x00001000U)        /*!<Bit 0 */
#define  FMC_SDTR2_TRC_1                    ((uint32_t)0x00002000U)        /*!<Bit 1 */
#define  FMC_SDTR2_TRC_2                    ((uint32_t)0x00004000U)        /*!<Bit 2 */

#define  FMC_SDTR2_TWR                      ((uint32_t)0x000F0000U)        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR2_TWR_0                    ((uint32_t)0x00010000U)        /*!<Bit 0 */
#define  FMC_SDTR2_TWR_1                    ((uint32_t)0x00020000U)        /*!<Bit 1 */
#define  FMC_SDTR2_TWR_2                    ((uint32_t)0x00040000U)        /*!<Bit 2 */

#define  FMC_SDTR2_TRP                      ((uint32_t)0x00F00000U)        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR2_TRP_0                    ((uint32_t)0x00100000U)        /*!<Bit 0 */
#define  FMC_SDTR2_TRP_1                    ((uint32_t)0x00200000U)        /*!<Bit 1 */
#define  FMC_SDTR2_TRP_2                    ((uint32_t)0x00400000U)        /*!<Bit 2 */

#define  FMC_SDTR2_TRCD                     ((uint32_t)0x0F000000U)        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR2_TRCD_0                   ((uint32_t)0x01000000U)        /*!<Bit 0 */
#define  FMC_SDTR2_TRCD_1                   ((uint32_t)0x02000000U)        /*!<Bit 1 */
#define  FMC_SDTR2_TRCD_2                   ((uint32_t)0x04000000U)        /*!<Bit 2 */

/******************  Bit definition for FMC_SDCMR register  ******************/
#define  FMC_SDCMR_MODE                     ((uint32_t)0x00000007U)        /*!<MODE[2:0] bits (Command mode) */
#define  FMC_SDCMR_MODE_0                   ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  FMC_SDCMR_MODE_1                   ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  FMC_SDCMR_MODE_2                   ((uint32_t)0x00000004U)        /*!<Bit 2 */

#define  FMC_SDCMR_CTB2                     ((uint32_t)0x00000008U)        /*!<Command target 2 */

#define  FMC_SDCMR_CTB1                     ((uint32_t)0x00000010U)        /*!<Command target 1 */

#define  FMC_SDCMR_NRFS                     ((uint32_t)0x000001E0U)        /*!<NRFS[3:0] bits (Number of auto-refresh) */
#define  FMC_SDCMR_NRFS_0                   ((uint32_t)0x00000020U)        /*!<Bit 0 */
#define  FMC_SDCMR_NRFS_1                   ((uint32_t)0x00000040U)        /*!<Bit 1 */
#define  FMC_SDCMR_NRFS_2                   ((uint32_t)0x00000080U)        /*!<Bit 2 */
#define  FMC_SDCMR_NRFS_3                   ((uint32_t)0x00000100U)        /*!<Bit 3 */

#define  FMC_SDCMR_MRD                      ((uint32_t)0x003FFE00U)        /*!<MRD[12:0] bits (Mode register definition) */

/******************  Bit definition for FMC_SDRTR register  ******************/
#define  FMC_SDRTR_CRE                      ((uint32_t)0x00000001U)        /*!<Clear refresh error flag */

#define  FMC_SDRTR_COUNT                    ((uint32_t)0x00003FFEU)        /*!<COUNT[12:0] bits (Refresh timer count) */

#define  FMC_SDRTR_REIE                     ((uint32_t)0x00004000U)        /*!<RES interupt enable */

/******************  Bit definition for FMC_SDSR register  ******************/
#define  FMC_SDSR_RE                        ((uint32_t)0x00000001U)        /*!<Refresh error flag */

#define  FMC_SDSR_MODES1                    ((uint32_t)0x00000006U)        /*!<MODES1[1:0]bits (Status mode for bank 1) */
#define  FMC_SDSR_MODES1_0                  ((uint32_t)0x00000002U)        /*!<Bit 0 */
#define  FMC_SDSR_MODES1_1                  ((uint32_t)0x00000004U)        /*!<Bit 1 */

#define  FMC_SDSR_MODES2                    ((uint32_t)0x00000018U)        /*!<MODES2[1:0]bits (Status mode for bank 2) */
#define  FMC_SDSR_MODES2_0                  ((uint32_t)0x00000008U)        /*!<Bit 0 */
#define  FMC_SDSR_MODES2_1                  ((uint32_t)0x00000010U)        /*!<Bit 1 */
#define  FMC_SDSR_BUSY                      ((uint32_t)0x00000020U)        /*!<Busy status */



/******************************************************************************/
/*                                                                            */
/*                            General Purpose I/O                             */
/*                                                                            */
/******************************************************************************/
/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0                    ((uint32_t)0x00000003U)
#define GPIO_MODER_MODER0_0                  ((uint32_t)0x00000001U)
#define GPIO_MODER_MODER0_1                  ((uint32_t)0x00000002U)

#define GPIO_MODER_MODER1                    ((uint32_t)0x0000000CU)
#define GPIO_MODER_MODER1_0                  ((uint32_t)0x00000004U)
#define GPIO_MODER_MODER1_1                  ((uint32_t)0x00000008U)

#define GPIO_MODER_MODER2                    ((uint32_t)0x00000030U)
#define GPIO_MODER_MODER2_0                  ((uint32_t)0x00000010U)
#define GPIO_MODER_MODER2_1                  ((uint32_t)0x00000020U)

#define GPIO_MODER_MODER3                    ((uint32_t)0x000000C0U)
#define GPIO_MODER_MODER3_0                  ((uint32_t)0x00000040U)
#define GPIO_MODER_MODER3_1                  ((uint32_t)0x00000080U)

#define GPIO_MODER_MODER4                    ((uint32_t)0x00000300U)
#define GPIO_MODER_MODER4_0                  ((uint32_t)0x00000100U)
#define GPIO_MODER_MODER4_1                  ((uint32_t)0x00000200U)

#define GPIO_MODER_MODER5                    ((uint32_t)0x00000C00U)
#define GPIO_MODER_MODER5_0                  ((uint32_t)0x00000400U)
#define GPIO_MODER_MODER5_1                  ((uint32_t)0x00000800U)

#define GPIO_MODER_MODER6                    ((uint32_t)0x00003000U)
#define GPIO_MODER_MODER6_0                  ((uint32_t)0x00001000U)
#define GPIO_MODER_MODER6_1                  ((uint32_t)0x00002000U)

#define GPIO_MODER_MODER7                    ((uint32_t)0x0000C000U)
#define GPIO_MODER_MODER7_0                  ((uint32_t)0x00004000U)
#define GPIO_MODER_MODER7_1                  ((uint32_t)0x00008000U)

#define GPIO_MODER_MODER8                    ((uint32_t)0x00030000U)
#define GPIO_MODER_MODER8_0                  ((uint32_t)0x00010000U)
#define GPIO_MODER_MODER8_1                  ((uint32_t)0x00020000U)

#define GPIO_MODER_MODER9                    ((uint32_t)0x000C0000U)
#define GPIO_MODER_MODER9_0                  ((uint32_t)0x00040000U)
#define GPIO_MODER_MODER9_1                  ((uint32_t)0x00080000U)

#define GPIO_MODER_MODER10                   ((uint32_t)0x00300000U)
#define GPIO_MODER_MODER10_0                 ((uint32_t)0x00100000U)
#define GPIO_MODER_MODER10_1                 ((uint32_t)0x00200000U)

#define GPIO_MODER_MODER11                   ((uint32_t)0x00C00000U)
#define GPIO_MODER_MODER11_0                 ((uint32_t)0x00400000U)
#define GPIO_MODER_MODER11_1                 ((uint32_t)0x00800000U)

#define GPIO_MODER_MODER12                   ((uint32_t)0x03000000U)
#define GPIO_MODER_MODER12_0                 ((uint32_t)0x01000000U)
#define GPIO_MODER_MODER12_1                 ((uint32_t)0x02000000U)

#define GPIO_MODER_MODER13                   ((uint32_t)0x0C000000U)
#define GPIO_MODER_MODER13_0                 ((uint32_t)0x04000000U)
#define GPIO_MODER_MODER13_1                 ((uint32_t)0x08000000U)

#define GPIO_MODER_MODER14                   ((uint32_t)0x30000000U)
#define GPIO_MODER_MODER14_0                 ((uint32_t)0x10000000U)
#define GPIO_MODER_MODER14_1                 ((uint32_t)0x20000000U)

#define GPIO_MODER_MODER15                   ((uint32_t)0xC0000000U)
#define GPIO_MODER_MODER15_0                 ((uint32_t)0x40000000U)
#define GPIO_MODER_MODER15_1                 ((uint32_t)0x80000000U)

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT_0                     ((uint32_t)0x00000001U)
#define GPIO_OTYPER_OT_1                     ((uint32_t)0x00000002U)
#define GPIO_OTYPER_OT_2                     ((uint32_t)0x00000004U)
#define GPIO_OTYPER_OT_3                     ((uint32_t)0x00000008U)
#define GPIO_OTYPER_OT_4                     ((uint32_t)0x00000010U)
#define GPIO_OTYPER_OT_5                     ((uint32_t)0x00000020U)
#define GPIO_OTYPER_OT_6                     ((uint32_t)0x00000040U)
#define GPIO_OTYPER_OT_7                     ((uint32_t)0x00000080U)
#define GPIO_OTYPER_OT_8                     ((uint32_t)0x00000100U)
#define GPIO_OTYPER_OT_9                     ((uint32_t)0x00000200U)
#define GPIO_OTYPER_OT_10                    ((uint32_t)0x00000400U)
#define GPIO_OTYPER_OT_11                    ((uint32_t)0x00000800U)
#define GPIO_OTYPER_OT_12                    ((uint32_t)0x00001000U)
#define GPIO_OTYPER_OT_13                    ((uint32_t)0x00002000U)
#define GPIO_OTYPER_OT_14                    ((uint32_t)0x00004000U)
#define GPIO_OTYPER_OT_15                    ((uint32_t)0x00008000U)

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDER_OSPEEDR0               ((uint32_t)0x00000003U)
#define GPIO_OSPEEDER_OSPEEDR0_0             ((uint32_t)0x00000001U)
#define GPIO_OSPEEDER_OSPEEDR0_1             ((uint32_t)0x00000002U)

#define GPIO_OSPEEDER_OSPEEDR1               ((uint32_t)0x0000000CU)
#define GPIO_OSPEEDER_OSPEEDR1_0             ((uint32_t)0x00000004U)
#define GPIO_OSPEEDER_OSPEEDR1_1             ((uint32_t)0x00000008U)

#define GPIO_OSPEEDER_OSPEEDR2               ((uint32_t)0x00000030U)
#define GPIO_OSPEEDER_OSPEEDR2_0             ((uint32_t)0x00000010U)
#define GPIO_OSPEEDER_OSPEEDR2_1             ((uint32_t)0x00000020U)

#define GPIO_OSPEEDER_OSPEEDR3               ((uint32_t)0x000000C0U)
#define GPIO_OSPEEDER_OSPEEDR3_0             ((uint32_t)0x00000040U)
#define GPIO_OSPEEDER_OSPEEDR3_1             ((uint32_t)0x00000080U)

#define GPIO_OSPEEDER_OSPEEDR4               ((uint32_t)0x00000300U)
#define GPIO_OSPEEDER_OSPEEDR4_0             ((uint32_t)0x00000100U)
#define GPIO_OSPEEDER_OSPEEDR4_1             ((uint32_t)0x00000200U)

#define GPIO_OSPEEDER_OSPEEDR5               ((uint32_t)0x00000C00U)
#define GPIO_OSPEEDER_OSPEEDR5_0             ((uint32_t)0x00000400U)
#define GPIO_OSPEEDER_OSPEEDR5_1             ((uint32_t)0x00000800U)

#define GPIO_OSPEEDER_OSPEEDR6               ((uint32_t)0x00003000U)
#define GPIO_OSPEEDER_OSPEEDR6_0             ((uint32_t)0x00001000U)
#define GPIO_OSPEEDER_OSPEEDR6_1             ((uint32_t)0x00002000U)

#define GPIO_OSPEEDER_OSPEEDR7               ((uint32_t)0x0000C000U)
#define GPIO_OSPEEDER_OSPEEDR7_0             ((uint32_t)0x00004000U)
#define GPIO_OSPEEDER_OSPEEDR7_1             ((uint32_t)0x00008000U)

#define GPIO_OSPEEDER_OSPEEDR8               ((uint32_t)0x00030000U)
#define GPIO_OSPEEDER_OSPEEDR8_0             ((uint32_t)0x00010000U)
#define GPIO_OSPEEDER_OSPEEDR8_1             ((uint32_t)0x00020000U)

#define GPIO_OSPEEDER_OSPEEDR9               ((uint32_t)0x000C0000U)
#define GPIO_OSPEEDER_OSPEEDR9_0             ((uint32_t)0x00040000U)
#define GPIO_OSPEEDER_OSPEEDR9_1             ((uint32_t)0x00080000U)

#define GPIO_OSPEEDER_OSPEEDR10              ((uint32_t)0x00300000U)
#define GPIO_OSPEEDER_OSPEEDR10_0            ((uint32_t)0x00100000U)
#define GPIO_OSPEEDER_OSPEEDR10_1            ((uint32_t)0x00200000U)

#define GPIO_OSPEEDER_OSPEEDR11              ((uint32_t)0x00C00000U)
#define GPIO_OSPEEDER_OSPEEDR11_0            ((uint32_t)0x00400000U)
#define GPIO_OSPEEDER_OSPEEDR11_1            ((uint32_t)0x00800000U)

#define GPIO_OSPEEDER_OSPEEDR12              ((uint32_t)0x03000000U)
#define GPIO_OSPEEDER_OSPEEDR12_0            ((uint32_t)0x01000000U)
#define GPIO_OSPEEDER_OSPEEDR12_1            ((uint32_t)0x02000000U)

#define GPIO_OSPEEDER_OSPEEDR13              ((uint32_t)0x0C000000U)
#define GPIO_OSPEEDER_OSPEEDR13_0            ((uint32_t)0x04000000U)
#define GPIO_OSPEEDER_OSPEEDR13_1            ((uint32_t)0x08000000U)

#define GPIO_OSPEEDER_OSPEEDR14              ((uint32_t)0x30000000U)
#define GPIO_OSPEEDER_OSPEEDR14_0            ((uint32_t)0x10000000U)
#define GPIO_OSPEEDER_OSPEEDR14_1            ((uint32_t)0x20000000U)

#define GPIO_OSPEEDER_OSPEEDR15              ((uint32_t)0xC0000000U)
#define GPIO_OSPEEDER_OSPEEDR15_0            ((uint32_t)0x40000000U)
#define GPIO_OSPEEDER_OSPEEDR15_1            ((uint32_t)0x80000000U)

/******************  Bits definition for GPIO_PUPDR register  *****************/
#define GPIO_PUPDR_PUPDR0                    ((uint32_t)0x00000003U)
#define GPIO_PUPDR_PUPDR0_0                  ((uint32_t)0x00000001U)
#define GPIO_PUPDR_PUPDR0_1                  ((uint32_t)0x00000002U)

#define GPIO_PUPDR_PUPDR1                    ((uint32_t)0x0000000CU)
#define GPIO_PUPDR_PUPDR1_0                  ((uint32_t)0x00000004U)
#define GPIO_PUPDR_PUPDR1_1                  ((uint32_t)0x00000008U)

#define GPIO_PUPDR_PUPDR2                    ((uint32_t)0x00000030U)
#define GPIO_PUPDR_PUPDR2_0                  ((uint32_t)0x00000010U)
#define GPIO_PUPDR_PUPDR2_1                  ((uint32_t)0x00000020U)

#define GPIO_PUPDR_PUPDR3                    ((uint32_t)0x000000C0U)
#define GPIO_PUPDR_PUPDR3_0                  ((uint32_t)0x00000040U)
#define GPIO_PUPDR_PUPDR3_1                  ((uint32_t)0x00000080U)

#define GPIO_PUPDR_PUPDR4                    ((uint32_t)0x00000300U)
#define GPIO_PUPDR_PUPDR4_0                  ((uint32_t)0x00000100U)
#define GPIO_PUPDR_PUPDR4_1                  ((uint32_t)0x00000200U)

#define GPIO_PUPDR_PUPDR5                    ((uint32_t)0x00000C00U)
#define GPIO_PUPDR_PUPDR5_0                  ((uint32_t)0x00000400U)
#define GPIO_PUPDR_PUPDR5_1                  ((uint32_t)0x00000800U)

#define GPIO_PUPDR_PUPDR6                    ((uint32_t)0x00003000U)
#define GPIO_PUPDR_PUPDR6_0                  ((uint32_t)0x00001000U)
#define GPIO_PUPDR_PUPDR6_1                  ((uint32_t)0x00002000U)

#define GPIO_PUPDR_PUPDR7                    ((uint32_t)0x0000C000U)
#define GPIO_PUPDR_PUPDR7_0                  ((uint32_t)0x00004000U)
#define GPIO_PUPDR_PUPDR7_1                  ((uint32_t)0x00008000U)

#define GPIO_PUPDR_PUPDR8                    ((uint32_t)0x00030000U)
#define GPIO_PUPDR_PUPDR8_0                  ((uint32_t)0x00010000U)
#define GPIO_PUPDR_PUPDR8_1                  ((uint32_t)0x00020000U)

#define GPIO_PUPDR_PUPDR9                    ((uint32_t)0x000C0000U)
#define GPIO_PUPDR_PUPDR9_0                  ((uint32_t)0x00040000U)
#define GPIO_PUPDR_PUPDR9_1                  ((uint32_t)0x00080000U)

#define GPIO_PUPDR_PUPDR10                   ((uint32_t)0x00300000U)
#define GPIO_PUPDR_PUPDR10_0                 ((uint32_t)0x00100000U)
#define GPIO_PUPDR_PUPDR10_1                 ((uint32_t)0x00200000U)

#define GPIO_PUPDR_PUPDR11                   ((uint32_t)0x00C00000U)
#define GPIO_PUPDR_PUPDR11_0                 ((uint32_t)0x00400000U)
#define GPIO_PUPDR_PUPDR11_1                 ((uint32_t)0x00800000U)

#define GPIO_PUPDR_PUPDR12                   ((uint32_t)0x03000000U)
#define GPIO_PUPDR_PUPDR12_0                 ((uint32_t)0x01000000U)
#define GPIO_PUPDR_PUPDR12_1                 ((uint32_t)0x02000000U)

#define GPIO_PUPDR_PUPDR13                   ((uint32_t)0x0C000000U)
#define GPIO_PUPDR_PUPDR13_0                 ((uint32_t)0x04000000U)
#define GPIO_PUPDR_PUPDR13_1                 ((uint32_t)0x08000000U)

#define GPIO_PUPDR_PUPDR14                   ((uint32_t)0x30000000U)
#define GPIO_PUPDR_PUPDR14_0                 ((uint32_t)0x10000000U)
#define GPIO_PUPDR_PUPDR14_1                 ((uint32_t)0x20000000U)

#define GPIO_PUPDR_PUPDR15                   ((uint32_t)0xC0000000U)
#define GPIO_PUPDR_PUPDR15_0                 ((uint32_t)0x40000000U)
#define GPIO_PUPDR_PUPDR15_1                 ((uint32_t)0x80000000U)

/******************  Bits definition for GPIO_IDR register  *******************/
#define GPIO_IDR_IDR_0                       ((uint32_t)0x00000001U)
#define GPIO_IDR_IDR_1                       ((uint32_t)0x00000002U)
#define GPIO_IDR_IDR_2                       ((uint32_t)0x00000004U)
#define GPIO_IDR_IDR_3                       ((uint32_t)0x00000008U)
#define GPIO_IDR_IDR_4                       ((uint32_t)0x00000010U)
#define GPIO_IDR_IDR_5                       ((uint32_t)0x00000020U)
#define GPIO_IDR_IDR_6                       ((uint32_t)0x00000040U)
#define GPIO_IDR_IDR_7                       ((uint32_t)0x00000080U)
#define GPIO_IDR_IDR_8                       ((uint32_t)0x00000100U)
#define GPIO_IDR_IDR_9                       ((uint32_t)0x00000200U)
#define GPIO_IDR_IDR_10                      ((uint32_t)0x00000400U)
#define GPIO_IDR_IDR_11                      ((uint32_t)0x00000800U)
#define GPIO_IDR_IDR_12                      ((uint32_t)0x00001000U)
#define GPIO_IDR_IDR_13                      ((uint32_t)0x00002000U)
#define GPIO_IDR_IDR_14                      ((uint32_t)0x00004000U)
#define GPIO_IDR_IDR_15                      ((uint32_t)0x00008000U)
/* Old GPIO_IDR register bits definition, maintained for legacy purpose */
#define GPIO_OTYPER_IDR_0                    GPIO_IDR_IDR_0
#define GPIO_OTYPER_IDR_1                    GPIO_IDR_IDR_1
#define GPIO_OTYPER_IDR_2                    GPIO_IDR_IDR_2
#define GPIO_OTYPER_IDR_3                    GPIO_IDR_IDR_3
#define GPIO_OTYPER_IDR_4                    GPIO_IDR_IDR_4
#define GPIO_OTYPER_IDR_5                    GPIO_IDR_IDR_5
#define GPIO_OTYPER_IDR_6                    GPIO_IDR_IDR_6
#define GPIO_OTYPER_IDR_7                    GPIO_IDR_IDR_7
#define GPIO_OTYPER_IDR_8                    GPIO_IDR_IDR_8
#define GPIO_OTYPER_IDR_9                    GPIO_IDR_IDR_9
#define GPIO_OTYPER_IDR_10                   GPIO_IDR_IDR_10
#define GPIO_OTYPER_IDR_11                   GPIO_IDR_IDR_11
#define GPIO_OTYPER_IDR_12                   GPIO_IDR_IDR_12
#define GPIO_OTYPER_IDR_13                   GPIO_IDR_IDR_13
#define GPIO_OTYPER_IDR_14                   GPIO_IDR_IDR_14
#define GPIO_OTYPER_IDR_15                   GPIO_IDR_IDR_15

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_ODR_0                       ((uint32_t)0x00000001U)
#define GPIO_ODR_ODR_1                       ((uint32_t)0x00000002U)
#define GPIO_ODR_ODR_2                       ((uint32_t)0x00000004U)
#define GPIO_ODR_ODR_3                       ((uint32_t)0x00000008U)
#define GPIO_ODR_ODR_4                       ((uint32_t)0x00000010U)
#define GPIO_ODR_ODR_5                       ((uint32_t)0x00000020U)
#define GPIO_ODR_ODR_6                       ((uint32_t)0x00000040U)
#define GPIO_ODR_ODR_7                       ((uint32_t)0x00000080U)
#define GPIO_ODR_ODR_8                       ((uint32_t)0x00000100U)
#define GPIO_ODR_ODR_9                       ((uint32_t)0x00000200U)
#define GPIO_ODR_ODR_10                      ((uint32_t)0x00000400U)
#define GPIO_ODR_ODR_11                      ((uint32_t)0x00000800U)
#define GPIO_ODR_ODR_12                      ((uint32_t)0x00001000U)
#define GPIO_ODR_ODR_13                      ((uint32_t)0x00002000U)
#define GPIO_ODR_ODR_14                      ((uint32_t)0x00004000U)
#define GPIO_ODR_ODR_15                      ((uint32_t)0x00008000U)
/* Old GPIO_ODR register bits definition, maintained for legacy purpose */
#define GPIO_OTYPER_ODR_0                    GPIO_ODR_ODR_0
#define GPIO_OTYPER_ODR_1                    GPIO_ODR_ODR_1
#define GPIO_OTYPER_ODR_2                    GPIO_ODR_ODR_2
#define GPIO_OTYPER_ODR_3                    GPIO_ODR_ODR_3
#define GPIO_OTYPER_ODR_4                    GPIO_ODR_ODR_4
#define GPIO_OTYPER_ODR_5                    GPIO_ODR_ODR_5
#define GPIO_OTYPER_ODR_6                    GPIO_ODR_ODR_6
#define GPIO_OTYPER_ODR_7                    GPIO_ODR_ODR_7
#define GPIO_OTYPER_ODR_8                    GPIO_ODR_ODR_8
#define GPIO_OTYPER_ODR_9                    GPIO_ODR_ODR_9
#define GPIO_OTYPER_ODR_10                   GPIO_ODR_ODR_10
#define GPIO_OTYPER_ODR_11                   GPIO_ODR_ODR_11
#define GPIO_OTYPER_ODR_12                   GPIO_ODR_ODR_12
#define GPIO_OTYPER_ODR_13                   GPIO_ODR_ODR_13
#define GPIO_OTYPER_ODR_14                   GPIO_ODR_ODR_14
#define GPIO_OTYPER_ODR_15                   GPIO_ODR_ODR_15

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS_0                       ((uint32_t)0x00000001U)
#define GPIO_BSRR_BS_1                       ((uint32_t)0x00000002U)
#define GPIO_BSRR_BS_2                       ((uint32_t)0x00000004U)
#define GPIO_BSRR_BS_3                       ((uint32_t)0x00000008U)
#define GPIO_BSRR_BS_4                       ((uint32_t)0x00000010U)
#define GPIO_BSRR_BS_5                       ((uint32_t)0x00000020U)
#define GPIO_BSRR_BS_6                       ((uint32_t)0x00000040U)
#define GPIO_BSRR_BS_7                       ((uint32_t)0x00000080U)
#define GPIO_BSRR_BS_8                       ((uint32_t)0x00000100U)
#define GPIO_BSRR_BS_9                       ((uint32_t)0x00000200U)
#define GPIO_BSRR_BS_10                      ((uint32_t)0x00000400U)
#define GPIO_BSRR_BS_11                      ((uint32_t)0x00000800U)
#define GPIO_BSRR_BS_12                      ((uint32_t)0x00001000U)
#define GPIO_BSRR_BS_13                      ((uint32_t)0x00002000U)
#define GPIO_BSRR_BS_14                      ((uint32_t)0x00004000U)
#define GPIO_BSRR_BS_15                      ((uint32_t)0x00008000U)
#define GPIO_BSRR_BR_0                       ((uint32_t)0x00010000U)
#define GPIO_BSRR_BR_1                       ((uint32_t)0x00020000U)
#define GPIO_BSRR_BR_2                       ((uint32_t)0x00040000U)
#define GPIO_BSRR_BR_3                       ((uint32_t)0x00080000U)
#define GPIO_BSRR_BR_4                       ((uint32_t)0x00100000U)
#define GPIO_BSRR_BR_5                       ((uint32_t)0x00200000U)
#define GPIO_BSRR_BR_6                       ((uint32_t)0x00400000U)
#define GPIO_BSRR_BR_7                       ((uint32_t)0x00800000U)
#define GPIO_BSRR_BR_8                       ((uint32_t)0x01000000U)
#define GPIO_BSRR_BR_9                       ((uint32_t)0x02000000U)
#define GPIO_BSRR_BR_10                      ((uint32_t)0x04000000U)
#define GPIO_BSRR_BR_11                      ((uint32_t)0x08000000U)
#define GPIO_BSRR_BR_12                      ((uint32_t)0x10000000U)
#define GPIO_BSRR_BR_13                      ((uint32_t)0x20000000U)
#define GPIO_BSRR_BR_14                      ((uint32_t)0x40000000U)
#define GPIO_BSRR_BR_15                      ((uint32_t)0x80000000U)

/****************** Bit definition for GPIO_LCKR register *********************/
#define GPIO_LCKR_LCK0                       ((uint32_t)0x00000001U)
#define GPIO_LCKR_LCK1                       ((uint32_t)0x00000002U)
#define GPIO_LCKR_LCK2                       ((uint32_t)0x00000004U)
#define GPIO_LCKR_LCK3                       ((uint32_t)0x00000008U)
#define GPIO_LCKR_LCK4                       ((uint32_t)0x00000010U)
#define GPIO_LCKR_LCK5                       ((uint32_t)0x00000020U)
#define GPIO_LCKR_LCK6                       ((uint32_t)0x00000040U)
#define GPIO_LCKR_LCK7                       ((uint32_t)0x00000080U)
#define GPIO_LCKR_LCK8                       ((uint32_t)0x00000100U)
#define GPIO_LCKR_LCK9                       ((uint32_t)0x00000200U)
#define GPIO_LCKR_LCK10                      ((uint32_t)0x00000400U)
#define GPIO_LCKR_LCK11                      ((uint32_t)0x00000800U)
#define GPIO_LCKR_LCK12                      ((uint32_t)0x00001000U)
#define GPIO_LCKR_LCK13                      ((uint32_t)0x00002000U)
#define GPIO_LCKR_LCK14                      ((uint32_t)0x00004000U)
#define GPIO_LCKR_LCK15                      ((uint32_t)0x00008000U)
#define GPIO_LCKR_LCKK                       ((uint32_t)0x00010000U)

/******************************************************************************/
/*                                                                            */
/*                      Inter-integrated Circuit Interface                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for I2C_CR1 register  ********************/
#define  I2C_CR1_PE                          ((uint32_t)0x00000001U)     /*!<Peripheral Enable                             */
#define  I2C_CR1_SMBUS                       ((uint32_t)0x00000002U)     /*!<SMBus Mode                                    */
#define  I2C_CR1_SMBTYPE                     ((uint32_t)0x00000008U)     /*!<SMBus Type                                    */
#define  I2C_CR1_ENARP                       ((uint32_t)0x00000010U)     /*!<ARP Enable                                    */
#define  I2C_CR1_ENPEC                       ((uint32_t)0x00000020U)     /*!<PEC Enable                                    */
#define  I2C_CR1_ENGC                        ((uint32_t)0x00000040U)     /*!<General Call Enable                           */
#define  I2C_CR1_NOSTRETCH                   ((uint32_t)0x00000080U)     /*!<Clock Stretching Disable (Slave mode)  */
#define  I2C_CR1_START                       ((uint32_t)0x00000100U)     /*!<Start Generation                              */
#define  I2C_CR1_STOP                        ((uint32_t)0x00000200U)     /*!<Stop Generation                               */
#define  I2C_CR1_ACK                         ((uint32_t)0x00000400U)     /*!<Acknowledge Enable                            */
#define  I2C_CR1_POS                         ((uint32_t)0x00000800U)     /*!<Acknowledge/PEC Position (for data reception) */
#define  I2C_CR1_PEC                         ((uint32_t)0x00001000U)     /*!<Packet Error Checking                         */
#define  I2C_CR1_ALERT                       ((uint32_t)0x00002000U)     /*!<SMBus Alert                                   */
#define  I2C_CR1_SWRST                       ((uint32_t)0x00008000U)     /*!<Software Reset                                */

/*******************  Bit definition for I2C_CR2 register  ********************/
#define  I2C_CR2_FREQ                        ((uint32_t)0x0000003FU)     /*!<FREQ[5:0] bits (Peripheral Clock Frequency)   */
#define  I2C_CR2_FREQ_0                      ((uint32_t)0x00000001U)     /*!<Bit 0 */
#define  I2C_CR2_FREQ_1                      ((uint32_t)0x00000002U)     /*!<Bit 1 */
#define  I2C_CR2_FREQ_2                      ((uint32_t)0x00000004U)     /*!<Bit 2 */
#define  I2C_CR2_FREQ_3                      ((uint32_t)0x00000008U)     /*!<Bit 3 */
#define  I2C_CR2_FREQ_4                      ((uint32_t)0x00000010U)     /*!<Bit 4 */
#define  I2C_CR2_FREQ_5                      ((uint32_t)0x00000020U)     /*!<Bit 5 */

#define  I2C_CR2_ITERREN                     ((uint32_t)0x00000100U)     /*!<Error Interrupt Enable  */
#define  I2C_CR2_ITEVTEN                     ((uint32_t)0x00000200U)     /*!<Event Interrupt Enable  */
#define  I2C_CR2_ITBUFEN                     ((uint32_t)0x00000400U)     /*!<Buffer Interrupt Enable */
#define  I2C_CR2_DMAEN                       ((uint32_t)0x00000800U)     /*!<DMA Requests Enable     */
#define  I2C_CR2_LAST                        ((uint32_t)0x00001000U)     /*!<DMA Last Transfer       */

/*******************  Bit definition for I2C_OAR1 register  *******************/
#define  I2C_OAR1_ADD1_7                     ((uint32_t)0x000000FEU)     /*!<Interface Address */
#define  I2C_OAR1_ADD8_9                     ((uint32_t)0x00000300U)     /*!<Interface Address */

#define  I2C_OAR1_ADD0                       ((uint32_t)0x00000001U)     /*!<Bit 0 */
#define  I2C_OAR1_ADD1                       ((uint32_t)0x00000002U)     /*!<Bit 1 */
#define  I2C_OAR1_ADD2                       ((uint32_t)0x00000004U)     /*!<Bit 2 */
#define  I2C_OAR1_ADD3                       ((uint32_t)0x00000008U)     /*!<Bit 3 */
#define  I2C_OAR1_ADD4                       ((uint32_t)0x00000010U)     /*!<Bit 4 */
#define  I2C_OAR1_ADD5                       ((uint32_t)0x00000020U)     /*!<Bit 5 */
#define  I2C_OAR1_ADD6                       ((uint32_t)0x00000040U)     /*!<Bit 6 */
#define  I2C_OAR1_ADD7                       ((uint32_t)0x00000080U)     /*!<Bit 7 */
#define  I2C_OAR1_ADD8                       ((uint32_t)0x00000100U)     /*!<Bit 8 */
#define  I2C_OAR1_ADD9                       ((uint32_t)0x00000200U)     /*!<Bit 9 */

#define  I2C_OAR1_ADDMODE                    ((uint32_t)0x00008000U)     /*!<Addressing Mode (Slave mode) */

/*******************  Bit definition for I2C_OAR2 register  *******************/
#define  I2C_OAR2_ENDUAL                     ((uint32_t)0x00000001U)        /*!<Dual addressing mode enable */
#define  I2C_OAR2_ADD2                       ((uint32_t)0x000000FEU)        /*!<Interface address           */

/********************  Bit definition for I2C_DR register  ********************/
#define  I2C_DR_DR                           ((uint32_t)0x000000FFU)        /*!<8-bit Data Register         */

/*******************  Bit definition for I2C_SR1 register  ********************/
#define  I2C_SR1_SB                          ((uint32_t)0x00000001U)     /*!<Start Bit (Master mode)                  */
#define  I2C_SR1_ADDR                        ((uint32_t)0x00000002U)     /*!<Address sent (master mode)/matched (slave mode) */
#define  I2C_SR1_BTF                         ((uint32_t)0x00000004U)     /*!<Byte Transfer Finished                          */
#define  I2C_SR1_ADD10                       ((uint32_t)0x00000008U)     /*!<10-bit header sent (Master mode)         */
#define  I2C_SR1_STOPF                       ((uint32_t)0x00000010U)     /*!<Stop detection (Slave mode)              */
#define  I2C_SR1_RXNE                        ((uint32_t)0x00000040U)     /*!<Data Register not Empty (receivers)      */
#define  I2C_SR1_TXE                         ((uint32_t)0x00000080U)     /*!<Data Register Empty (transmitters)       */
#define  I2C_SR1_BERR                        ((uint32_t)0x00000100U)     /*!<Bus Error                                       */
#define  I2C_SR1_ARLO                        ((uint32_t)0x00000200U)     /*!<Arbitration Lost (master mode)           */
#define  I2C_SR1_AF                          ((uint32_t)0x00000400U)     /*!<Acknowledge Failure                             */
#define  I2C_SR1_OVR                         ((uint32_t)0x00000800U)     /*!<Overrun/Underrun                                */
#define  I2C_SR1_PECERR                      ((uint32_t)0x00001000U)     /*!<PEC Error in reception                          */
#define  I2C_SR1_TIMEOUT                     ((uint32_t)0x00004000U)     /*!<Timeout or Tlow Error                           */
#define  I2C_SR1_SMBALERT                    ((uint32_t)0x00008000U)     /*!<SMBus Alert                                     */

/*******************  Bit definition for I2C_SR2 register  ********************/
#define  I2C_SR2_MSL                         ((uint32_t)0x00000001U)     /*!<Master/Slave                              */
#define  I2C_SR2_BUSY                        ((uint32_t)0x00000002U)     /*!<Bus Busy                                  */
#define  I2C_SR2_TRA                         ((uint32_t)0x00000004U)     /*!<Transmitter/Receiver                      */
#define  I2C_SR2_GENCALL                     ((uint32_t)0x00000010U)     /*!<General Call Address (Slave mode)  */
#define  I2C_SR2_SMBDEFAULT                  ((uint32_t)0x00000020U)     /*!<SMBus Device Default Address (Slave mode) */
#define  I2C_SR2_SMBHOST                     ((uint32_t)0x00000040U)     /*!<SMBus Host Header (Slave mode)     */
#define  I2C_SR2_DUALF                       ((uint32_t)0x00000080U)     /*!<Dual Flag (Slave mode)             */
#define  I2C_SR2_PEC                         ((uint32_t)0x0000FF00U)     /*!<Packet Error Checking Register            */

/*******************  Bit definition for I2C_CCR register  ********************/
#define  I2C_CCR_CCR                         ((uint32_t)0x00000FFFU)     /*!<Clock Control Register in Fast/Standard mode (Master mode) */
#define  I2C_CCR_DUTY                        ((uint32_t)0x00004000U)     /*!<Fast Mode Duty Cycle                                       */
#define  I2C_CCR_FS                          ((uint32_t)0x00008000U)     /*!<I2C Master Mode Selection                                  */

/******************  Bit definition for I2C_TRISE register  *******************/
#define  I2C_TRISE_TRISE                     ((uint32_t)0x0000003FU)     /*!<Maximum Rise Time in Fast/Standard mode (Master mode) */

/******************  Bit definition for I2C_FLTR register  *******************/
#define  I2C_FLTR_DNF                        ((uint32_t)0x0000000FU)     /*!<Digital Noise Filter */
#define  I2C_FLTR_ANOFF                      ((uint32_t)0x00000010U)     /*!<Analog Noise Filter OFF */

/******************************************************************************/
/*                                                                            */
/*                           Independent WATCHDOG                             */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for IWDG_KR register  ********************/
#define  IWDG_KR_KEY                         ((uint32_t)0xFFFFU)            /*!<Key value (write only, read 0000h)  */

/*******************  Bit definition for IWDG_PR register  ********************/
#define  IWDG_PR_PR                          ((uint32_t)0x07U)               /*!<PR[2:0] (Prescaler divider)         */
#define  IWDG_PR_PR_0                        ((uint32_t)0x01U)               /*!<Bit 0 */
#define  IWDG_PR_PR_1                        ((uint32_t)0x02U)               /*!<Bit 1 */
#define  IWDG_PR_PR_2                        ((uint32_t)0x04U)               /*!<Bit 2 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define  IWDG_RLR_RL                         ((uint32_t)0x0FFFU)            /*!<Watchdog counter reload value        */

/*******************  Bit definition for IWDG_SR register  ********************/
#define  IWDG_SR_PVU                         ((uint32_t)0x01U)               /*!<Watchdog prescaler value update      */
#define  IWDG_SR_RVU                         ((uint32_t)0x02U)               /*!<Watchdog counter reload value update */


/******************************************************************************/
/*                                                                            */
/*                      LCD-TFT Display Controller (LTDC)                     */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for LTDC_SSCR register  *****************/

#define LTDC_SSCR_VSH                       ((uint32_t)0x000007FFU)              /*!< Vertical Synchronization Height */
#define LTDC_SSCR_HSW                       ((uint32_t)0x0FFF0000U)              /*!< Horizontal Synchronization Width */

/********************  Bit definition for LTDC_BPCR register  *****************/

#define LTDC_BPCR_AVBP                      ((uint32_t)0x000007FFU)              /*!< Accumulated Vertical Back Porch */
#define LTDC_BPCR_AHBP                      ((uint32_t)0x0FFF0000U)              /*!< Accumulated Horizontal Back Porch */

/********************  Bit definition for LTDC_AWCR register  *****************/

#define LTDC_AWCR_AAH                       ((uint32_t)0x000007FFU)              /*!< Accumulated Active heigh */
#define LTDC_AWCR_AAW                       ((uint32_t)0x0FFF0000U)              /*!< Accumulated Active Width */

/********************  Bit definition for LTDC_TWCR register  *****************/

#define LTDC_TWCR_TOTALH                    ((uint32_t)0x000007FFU)              /*!< Total Heigh */
#define LTDC_TWCR_TOTALW                    ((uint32_t)0x0FFF0000U)              /*!< Total Width */

/********************  Bit definition for LTDC_GCR register  ******************/

#define LTDC_GCR_LTDCEN                     ((uint32_t)0x00000001U)              /*!< LCD-TFT controller enable bit */
#define LTDC_GCR_DBW                        ((uint32_t)0x00000070U)              /*!< Dither Blue Width */
#define LTDC_GCR_DGW                        ((uint32_t)0x00000700U)              /*!< Dither Green Width */
#define LTDC_GCR_DRW                        ((uint32_t)0x00007000U)              /*!< Dither Red Width */
#define LTDC_GCR_DEN                        ((uint32_t)0x00010000U)              /*!< Dither Enable */
#define LTDC_GCR_PCPOL                      ((uint32_t)0x10000000U)              /*!< Pixel Clock Polarity */
#define LTDC_GCR_DEPOL                      ((uint32_t)0x20000000U)              /*!< Data Enable Polarity */
#define LTDC_GCR_VSPOL                      ((uint32_t)0x40000000U)              /*!< Vertical Synchronization Polarity */
#define LTDC_GCR_HSPOL                      ((uint32_t)0x80000000U)              /*!< Horizontal Synchronization Polarity */

/* Legacy defines */
#define LTDC_GCR_DTEN                       LTDC_GCR_DEN

/********************  Bit definition for LTDC_SRCR register  *****************/

#define LTDC_SRCR_IMR                      ((uint32_t)0x00000001U)               /*!< Immediate Reload */
#define LTDC_SRCR_VBR                      ((uint32_t)0x00000002U)               /*!< Vertical Blanking Reload */

/********************  Bit definition for LTDC_BCCR register  *****************/

#define LTDC_BCCR_BCBLUE                    ((uint32_t)0x000000FFU)              /*!< Background Blue value */
#define LTDC_BCCR_BCGREEN                   ((uint32_t)0x0000FF00U)              /*!< Background Green value */
#define LTDC_BCCR_BCRED                     ((uint32_t)0x00FF0000U)              /*!< Background Red value */

/********************  Bit definition for LTDC_IER register  ******************/

#define LTDC_IER_LIE                        ((uint32_t)0x00000001U)              /*!< Line Interrupt Enable */
#define LTDC_IER_FUIE                       ((uint32_t)0x00000002U)              /*!< FIFO Underrun Interrupt Enable */
#define LTDC_IER_TERRIE                     ((uint32_t)0x00000004U)              /*!< Transfer Error Interrupt Enable */
#define LTDC_IER_RRIE                       ((uint32_t)0x00000008U)              /*!< Register Reload interrupt enable */

/********************  Bit definition for LTDC_ISR register  ******************/

#define LTDC_ISR_LIF                        ((uint32_t)0x00000001U)              /*!< Line Interrupt Flag */
#define LTDC_ISR_FUIF                       ((uint32_t)0x00000002U)              /*!< FIFO Underrun Interrupt Flag */
#define LTDC_ISR_TERRIF                     ((uint32_t)0x00000004U)              /*!< Transfer Error Interrupt Flag */
#define LTDC_ISR_RRIF                       ((uint32_t)0x00000008U)              /*!< Register Reload interrupt Flag */

/********************  Bit definition for LTDC_ICR register  ******************/

#define LTDC_ICR_CLIF                       ((uint32_t)0x00000001U)              /*!< Clears the Line Interrupt Flag */
#define LTDC_ICR_CFUIF                      ((uint32_t)0x00000002U)              /*!< Clears the FIFO Underrun Interrupt Flag */
#define LTDC_ICR_CTERRIF                    ((uint32_t)0x00000004U)              /*!< Clears the Transfer Error Interrupt Flag */
#define LTDC_ICR_CRRIF                      ((uint32_t)0x00000008U)              /*!< Clears Register Reload interrupt Flag */

/********************  Bit definition for LTDC_LIPCR register  ****************/

#define LTDC_LIPCR_LIPOS                    ((uint32_t)0x000007FFU)              /*!< Line Interrupt Position */

/********************  Bit definition for LTDC_CPSR register  *****************/

#define LTDC_CPSR_CYPOS                     ((uint32_t)0x0000FFFFU)              /*!< Current Y Position */
#define LTDC_CPSR_CXPOS                     ((uint32_t)0xFFFF0000U)              /*!< Current X Position */

/********************  Bit definition for LTDC_CDSR register  *****************/

#define LTDC_CDSR_VDES                      ((uint32_t)0x00000001U)              /*!< Vertical Data Enable Status */
#define LTDC_CDSR_HDES                      ((uint32_t)0x00000002U)              /*!< Horizontal Data Enable Status */
#define LTDC_CDSR_VSYNCS                    ((uint32_t)0x00000004U)              /*!< Vertical Synchronization Status */
#define LTDC_CDSR_HSYNCS                    ((uint32_t)0x00000008U)              /*!< Horizontal Synchronization Status */

/********************  Bit definition for LTDC_LxCR register  *****************/

#define LTDC_LxCR_LEN                       ((uint32_t)0x00000001U)              /*!< Layer Enable */
#define LTDC_LxCR_COLKEN                    ((uint32_t)0x00000002U)              /*!< Color Keying Enable */
#define LTDC_LxCR_CLUTEN                    ((uint32_t)0x00000010U)              /*!< Color Lockup Table Enable */

/********************  Bit definition for LTDC_LxWHPCR register  **************/

#define LTDC_LxWHPCR_WHSTPOS                ((uint32_t)0x00000FFFU)              /*!< Window Horizontal Start Position */
#define LTDC_LxWHPCR_WHSPPOS                ((uint32_t)0xFFFF0000U)              /*!< Window Horizontal Stop Position */

/********************  Bit definition for LTDC_LxWVPCR register  **************/

#define LTDC_LxWVPCR_WVSTPOS                ((uint32_t)0x00000FFFU)              /*!< Window Vertical Start Position */
#define LTDC_LxWVPCR_WVSPPOS                ((uint32_t)0xFFFF0000U)              /*!< Window Vertical Stop Position */

/********************  Bit definition for LTDC_LxCKCR register  ***************/

#define LTDC_LxCKCR_CKBLUE                  ((uint32_t)0x000000FFU)              /*!< Color Key Blue value */
#define LTDC_LxCKCR_CKGREEN                 ((uint32_t)0x0000FF00U)              /*!< Color Key Green value */
#define LTDC_LxCKCR_CKRED                   ((uint32_t)0x00FF0000U)              /*!< Color Key Red value */

/********************  Bit definition for LTDC_LxPFCR register  ***************/

#define LTDC_LxPFCR_PF                      ((uint32_t)0x00000007U)              /*!< Pixel Format */

/********************  Bit definition for LTDC_LxCACR register  ***************/

#define LTDC_LxCACR_CONSTA                  ((uint32_t)0x000000FFU)              /*!< Constant Alpha */

/********************  Bit definition for LTDC_LxDCCR register  ***************/

#define LTDC_LxDCCR_DCBLUE                  ((uint32_t)0x000000FFU)              /*!< Default Color Blue */
#define LTDC_LxDCCR_DCGREEN                 ((uint32_t)0x0000FF00U)              /*!< Default Color Green */
#define LTDC_LxDCCR_DCRED                   ((uint32_t)0x00FF0000U)              /*!< Default Color Red */
#define LTDC_LxDCCR_DCALPHA                 ((uint32_t)0xFF000000U)              /*!< Default Color Alpha */

/********************  Bit definition for LTDC_LxBFCR register  ***************/

#define LTDC_LxBFCR_BF2                     ((uint32_t)0x00000007U)              /*!< Blending Factor 2 */
#define LTDC_LxBFCR_BF1                     ((uint32_t)0x00000700U)              /*!< Blending Factor 1 */

/********************  Bit definition for LTDC_LxCFBAR register  **************/

#define LTDC_LxCFBAR_CFBADD                 ((uint32_t)0xFFFFFFFFU)              /*!< Color Frame Buffer Start Address */

/********************  Bit definition for LTDC_LxCFBLR register  **************/

#define LTDC_LxCFBLR_CFBLL                  ((uint32_t)0x00001FFFU)              /*!< Color Frame Buffer Line Length */
#define LTDC_LxCFBLR_CFBP                   ((uint32_t)0x1FFF0000U)              /*!< Color Frame Buffer Pitch in bytes */

/********************  Bit definition for LTDC_LxCFBLNR register  *************/

#define LTDC_LxCFBLNR_CFBLNBR               ((uint32_t)0x000007FFU)              /*!< Frame Buffer Line Number */

/********************  Bit definition for LTDC_LxCLUTWR register  *************/

#define LTDC_LxCLUTWR_BLUE                  ((uint32_t)0x000000FFU)              /*!< Blue value */
#define LTDC_LxCLUTWR_GREEN                 ((uint32_t)0x0000FF00U)              /*!< Green value */
#define LTDC_LxCLUTWR_RED                   ((uint32_t)0x00FF0000U)              /*!< Red value */
#define LTDC_LxCLUTWR_CLUTADD               ((uint32_t)0xFF000000U)              /*!< CLUT address */


/******************************************************************************/
/*                                                                            */
/*                             Power Control                                  */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for PWR_CR register  ********************/
#define  PWR_CR_LPDS                         ((uint32_t)0x00000001U)     /*!< Low-Power Deepsleep                 */
#define  PWR_CR_PDDS                         ((uint32_t)0x00000002U)     /*!< Power Down Deepsleep                */
#define  PWR_CR_CWUF                         ((uint32_t)0x00000004U)     /*!< Clear Wakeup Flag                   */
#define  PWR_CR_CSBF                         ((uint32_t)0x00000008U)     /*!< Clear Standby Flag                  */
#define  PWR_CR_PVDE                         ((uint32_t)0x00000010U)     /*!< Power Voltage Detector Enable       */

#define  PWR_CR_PLS                          ((uint32_t)0x000000E0U)     /*!< PLS[2:0] bits (PVD Level Selection) */
#define  PWR_CR_PLS_0                        ((uint32_t)0x00000020U)     /*!< Bit 0 */
#define  PWR_CR_PLS_1                        ((uint32_t)0x00000040U)     /*!< Bit 1 */
#define  PWR_CR_PLS_2                        ((uint32_t)0x00000080U)     /*!< Bit 2 */

/*!< PVD level configuration */
#define  PWR_CR_PLS_LEV0                     ((uint32_t)0x00000000U)     /*!< PVD level 0 */
#define  PWR_CR_PLS_LEV1                     ((uint32_t)0x00000020U)     /*!< PVD level 1 */
#define  PWR_CR_PLS_LEV2                     ((uint32_t)0x00000040U)     /*!< PVD level 2 */
#define  PWR_CR_PLS_LEV3                     ((uint32_t)0x00000060U)     /*!< PVD level 3 */
#define  PWR_CR_PLS_LEV4                     ((uint32_t)0x00000080U)     /*!< PVD level 4 */
#define  PWR_CR_PLS_LEV5                     ((uint32_t)0x000000A0U)     /*!< PVD level 5 */
#define  PWR_CR_PLS_LEV6                     ((uint32_t)0x000000C0U)     /*!< PVD level 6 */
#define  PWR_CR_PLS_LEV7                     ((uint32_t)0x000000E0U)     /*!< PVD level 7 */
#define  PWR_CR_DBP                          ((uint32_t)0x00000100U)     /*!< Disable Backup Domain write protection                     */
#define  PWR_CR_FPDS                         ((uint32_t)0x00000200U)     /*!< Flash power down in Stop mode                              */
#define  PWR_CR_LPLVDS                       ((uint32_t)0x00000400U)     /*!< Low-Power Regulator Low Voltage Scaling in Stop mode       */
#define  PWR_CR_MRLVDS                       ((uint32_t)0x00000800U)     /*!< Main regulator Low Voltage Scaling in Stop mode            */
#define  PWR_CR_ADCDC1                       ((uint32_t)0x00002000U)     /*!< Refer to AN4073 on how to use this bit */
#define  PWR_CR_VOS                          ((uint32_t)0x0000C000U)     /*!< VOS[1:0] bits (Regulator voltage scaling output selection) */
#define  PWR_CR_VOS_0                        ((uint32_t)0x00004000U)     /*!< Bit 0 */
#define  PWR_CR_VOS_1                        ((uint32_t)0x00008000U)     /*!< Bit 1 */
#define  PWR_CR_ODEN                         ((uint32_t)0x00010000U)     /*!< Over Drive enable                   */
#define  PWR_CR_ODSWEN                       ((uint32_t)0x00020000U)     /*!< Over Drive switch enabled           */
#define  PWR_CR_UDEN                         ((uint32_t)0x000C0000U)     /*!< Under Drive enable in stop mode     */
#define  PWR_CR_UDEN_0                       ((uint32_t)0x00040000U)     /*!< Bit 0                               */
#define  PWR_CR_UDEN_1                       ((uint32_t)0x00080000U)     /*!< Bit 1                               */

/* Legacy define */
#define  PWR_CR_PMODE                        PWR_CR_VOS
#define  PWR_CR_LPUDS                        PWR_CR_LPLVDS     /*!< Low-Power Regulator in deepsleep under-drive mode          */
#define  PWR_CR_MRUDS                        PWR_CR_MRLVDS     /*!< Main regulator in deepsleep under-drive mode               */

/*******************  Bit definition for PWR_CSR register  ********************/
#define  PWR_CSR_WUF                         ((uint32_t)0x00000001U)     /*!< Wakeup Flag                                      */
#define  PWR_CSR_SBF                         ((uint32_t)0x00000002U)     /*!< Standby Flag                                     */
#define  PWR_CSR_PVDO                        ((uint32_t)0x00000004U)     /*!< PVD Output                                       */
#define  PWR_CSR_BRR                         ((uint32_t)0x00000008U)     /*!< Backup regulator ready                           */
#define  PWR_CSR_EWUP                        ((uint32_t)0x00000100U)     /*!< Enable WKUP pin                                  */
#define  PWR_CSR_BRE                         ((uint32_t)0x00000200U)     /*!< Backup regulator enable                          */
#define  PWR_CSR_VOSRDY                      ((uint32_t)0x00004000U)     /*!< Regulator voltage scaling output selection ready */
#define  PWR_CSR_ODRDY                       ((uint32_t)0x00010000U)     /*!< Over Drive generator ready                       */
#define  PWR_CSR_ODSWRDY                     ((uint32_t)0x00020000U)     /*!< Over Drive Switch ready                          */
#define  PWR_CSR_UDSWRDY                     ((uint32_t)0x000C0000U)     /*!< Under Drive ready                                */

/* Legacy define */
#define  PWR_CSR_REGRDY                      PWR_CSR_VOSRDY

/******************************************************************************/
/*                                                                            */
/*                         Reset and Clock Control                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for RCC_CR register  ********************/
#define  RCC_CR_HSION                        ((uint32_t)0x00000001U)
#define  RCC_CR_HSIRDY                       ((uint32_t)0x00000002U)

#define  RCC_CR_HSITRIM                      ((uint32_t)0x000000F8U)
#define  RCC_CR_HSITRIM_0                    ((uint32_t)0x00000008U)/*!<Bit 0 */
#define  RCC_CR_HSITRIM_1                    ((uint32_t)0x00000010U)/*!<Bit 1 */
#define  RCC_CR_HSITRIM_2                    ((uint32_t)0x00000020U)/*!<Bit 2 */
#define  RCC_CR_HSITRIM_3                    ((uint32_t)0x00000040U)/*!<Bit 3 */
#define  RCC_CR_HSITRIM_4                    ((uint32_t)0x00000080U)/*!<Bit 4 */

#define  RCC_CR_HSICAL                       ((uint32_t)0x0000FF00U)
#define  RCC_CR_HSICAL_0                     ((uint32_t)0x00000100U)/*!<Bit 0 */
#define  RCC_CR_HSICAL_1                     ((uint32_t)0x00000200U)/*!<Bit 1 */
#define  RCC_CR_HSICAL_2                     ((uint32_t)0x00000400U)/*!<Bit 2 */
#define  RCC_CR_HSICAL_3                     ((uint32_t)0x00000800U)/*!<Bit 3 */
#define  RCC_CR_HSICAL_4                     ((uint32_t)0x00001000U)/*!<Bit 4 */
#define  RCC_CR_HSICAL_5                     ((uint32_t)0x00002000U)/*!<Bit 5 */
#define  RCC_CR_HSICAL_6                     ((uint32_t)0x00004000U)/*!<Bit 6 */
#define  RCC_CR_HSICAL_7                     ((uint32_t)0x00008000U)/*!<Bit 7 */

#define  RCC_CR_HSEON                        ((uint32_t)0x00010000U)
#define  RCC_CR_HSERDY                       ((uint32_t)0x00020000U)
#define  RCC_CR_HSEBYP                       ((uint32_t)0x00040000U)
#define  RCC_CR_CSSON                        ((uint32_t)0x00080000U)
#define  RCC_CR_PLLON                        ((uint32_t)0x01000000U)
#define  RCC_CR_PLLRDY                       ((uint32_t)0x02000000U)
#define  RCC_CR_PLLI2SON                     ((uint32_t)0x04000000U)
#define  RCC_CR_PLLI2SRDY                    ((uint32_t)0x08000000U)
#define  RCC_CR_PLLSAION                     ((uint32_t)0x10000000U)
#define  RCC_CR_PLLSAIRDY                    ((uint32_t)0x20000000U)

/********************  Bit definition for RCC_PLLCFGR register  ***************/
#define  RCC_PLLCFGR_PLLM                    ((uint32_t)0x0000003FU)
#define  RCC_PLLCFGR_PLLM_0                  ((uint32_t)0x00000001U)
#define  RCC_PLLCFGR_PLLM_1                  ((uint32_t)0x00000002U)
#define  RCC_PLLCFGR_PLLM_2                  ((uint32_t)0x00000004U)
#define  RCC_PLLCFGR_PLLM_3                  ((uint32_t)0x00000008U)
#define  RCC_PLLCFGR_PLLM_4                  ((uint32_t)0x00000010U)
#define  RCC_PLLCFGR_PLLM_5                  ((uint32_t)0x00000020U)

#define  RCC_PLLCFGR_PLLN                     ((uint32_t)0x00007FC0U)
#define  RCC_PLLCFGR_PLLN_0                   ((uint32_t)0x00000040U)
#define  RCC_PLLCFGR_PLLN_1                   ((uint32_t)0x00000080U)
#define  RCC_PLLCFGR_PLLN_2                   ((uint32_t)0x00000100U)
#define  RCC_PLLCFGR_PLLN_3                   ((uint32_t)0x00000200U)
#define  RCC_PLLCFGR_PLLN_4                   ((uint32_t)0x00000400U)
#define  RCC_PLLCFGR_PLLN_5                   ((uint32_t)0x00000800U)
#define  RCC_PLLCFGR_PLLN_6                   ((uint32_t)0x00001000U)
#define  RCC_PLLCFGR_PLLN_7                   ((uint32_t)0x00002000U)
#define  RCC_PLLCFGR_PLLN_8                   ((uint32_t)0x00004000U)

#define  RCC_PLLCFGR_PLLP                    ((uint32_t)0x00030000U)
#define  RCC_PLLCFGR_PLLP_0                  ((uint32_t)0x00010000U)
#define  RCC_PLLCFGR_PLLP_1                  ((uint32_t)0x00020000U)

#define  RCC_PLLCFGR_PLLSRC                  ((uint32_t)0x00400000U)
#define  RCC_PLLCFGR_PLLSRC_HSE              ((uint32_t)0x00400000U)
#define  RCC_PLLCFGR_PLLSRC_HSI              ((uint32_t)0x00000000U)

#define  RCC_PLLCFGR_PLLQ                    ((uint32_t)0x0F000000U)
#define  RCC_PLLCFGR_PLLQ_0                  ((uint32_t)0x01000000U)
#define  RCC_PLLCFGR_PLLQ_1                  ((uint32_t)0x02000000U)
#define  RCC_PLLCFGR_PLLQ_2                  ((uint32_t)0x04000000U)
#define  RCC_PLLCFGR_PLLQ_3                  ((uint32_t)0x08000000U)

/********************  Bit definition for RCC_CFGR register  ******************/
/*!< SW configuration */
#define  RCC_CFGR_SW                         ((uint32_t)0x00000003U)        /*!< SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_0                       ((uint32_t)0x00000001U)        /*!< Bit 0 */
#define  RCC_CFGR_SW_1                       ((uint32_t)0x00000002U)        /*!< Bit 1 */

#define  RCC_CFGR_SW_HSI                     ((uint32_t)0x00000000U)        /*!< HSI selected as system clock */
#define  RCC_CFGR_SW_HSE                     ((uint32_t)0x00000001U)        /*!< HSE selected as system clock */
#define  RCC_CFGR_SW_PLL                     ((uint32_t)0x00000002U)        /*!< PLL selected as system clock */

/*!< SWS configuration */
#define  RCC_CFGR_SWS                        ((uint32_t)0x0000000CU)        /*!< SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_0                      ((uint32_t)0x00000004U)        /*!< Bit 0 */
#define  RCC_CFGR_SWS_1                      ((uint32_t)0x00000008U)        /*!< Bit 1 */

#define  RCC_CFGR_SWS_HSI                    ((uint32_t)0x00000000U)        /*!< HSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSE                    ((uint32_t)0x00000004U)        /*!< HSE oscillator used as system clock */
#define  RCC_CFGR_SWS_PLL                    ((uint32_t)0x00000008U)        /*!< PLL used as system clock */

/*!< HPRE configuration */
#define  RCC_CFGR_HPRE                       ((uint32_t)0x000000F0U)        /*!< HPRE[3:0] bits (AHB prescaler) */
#define  RCC_CFGR_HPRE_0                     ((uint32_t)0x00000010U)        /*!< Bit 0 */
#define  RCC_CFGR_HPRE_1                     ((uint32_t)0x00000020U)        /*!< Bit 1 */
#define  RCC_CFGR_HPRE_2                     ((uint32_t)0x00000040U)        /*!< Bit 2 */
#define  RCC_CFGR_HPRE_3                     ((uint32_t)0x00000080U)        /*!< Bit 3 */

#define  RCC_CFGR_HPRE_DIV1                  ((uint32_t)0x00000000U)        /*!< SYSCLK not divided */
#define  RCC_CFGR_HPRE_DIV2                  ((uint32_t)0x00000080U)        /*!< SYSCLK divided by 2 */
#define  RCC_CFGR_HPRE_DIV4                  ((uint32_t)0x00000090U)        /*!< SYSCLK divided by 4 */
#define  RCC_CFGR_HPRE_DIV8                  ((uint32_t)0x000000A0U)        /*!< SYSCLK divided by 8 */
#define  RCC_CFGR_HPRE_DIV16                 ((uint32_t)0x000000B0U)        /*!< SYSCLK divided by 16 */
#define  RCC_CFGR_HPRE_DIV64                 ((uint32_t)0x000000C0U)        /*!< SYSCLK divided by 64 */
#define  RCC_CFGR_HPRE_DIV128                ((uint32_t)0x000000D0U)        /*!< SYSCLK divided by 128 */
#define  RCC_CFGR_HPRE_DIV256                ((uint32_t)0x000000E0U)        /*!< SYSCLK divided by 256 */
#define  RCC_CFGR_HPRE_DIV512                ((uint32_t)0x000000F0U)        /*!< SYSCLK divided by 512 */

/*!< PPRE1 configuration */
#define  RCC_CFGR_PPRE1                      ((uint32_t)0x00001C00U)        /*!< PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_CFGR_PPRE1_0                    ((uint32_t)0x00000400U)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE1_1                    ((uint32_t)0x00000800U)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE1_2                    ((uint32_t)0x00001000U)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE1_DIV1                 ((uint32_t)0x00000000U)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 ((uint32_t)0x00001000U)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 ((uint32_t)0x00001400U)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE1_DIV8                 ((uint32_t)0x00001800U)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE1_DIV16                ((uint32_t)0x00001C00U)        /*!< HCLK divided by 16 */

/*!< PPRE2 configuration */
#define  RCC_CFGR_PPRE2                      ((uint32_t)0x0000E000U)        /*!< PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_CFGR_PPRE2_0                    ((uint32_t)0x00002000U)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE2_1                    ((uint32_t)0x00004000U)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE2_2                    ((uint32_t)0x00008000U)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE2_DIV1                 ((uint32_t)0x00000000U)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 ((uint32_t)0x00008000U)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE2_DIV4                 ((uint32_t)0x0000A000U)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV8                 ((uint32_t)0x0000C000U)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE2_DIV16                ((uint32_t)0x0000E000U)        /*!< HCLK divided by 16 */

/*!< RTCPRE configuration */
#define  RCC_CFGR_RTCPRE                     ((uint32_t)0x001F0000U)
#define  RCC_CFGR_RTCPRE_0                   ((uint32_t)0x00010000U)
#define  RCC_CFGR_RTCPRE_1                   ((uint32_t)0x00020000U)
#define  RCC_CFGR_RTCPRE_2                   ((uint32_t)0x00040000U)
#define  RCC_CFGR_RTCPRE_3                   ((uint32_t)0x00080000U)
#define  RCC_CFGR_RTCPRE_4                   ((uint32_t)0x00100000U)

/*!< MCO1 configuration */
#define  RCC_CFGR_MCO1                       ((uint32_t)0x00600000U)
#define  RCC_CFGR_MCO1_0                     ((uint32_t)0x00200000U)
#define  RCC_CFGR_MCO1_1                     ((uint32_t)0x00400000U)

#define  RCC_CFGR_I2SSRC                     ((uint32_t)0x00800000U)

#define  RCC_CFGR_MCO1PRE                    ((uint32_t)0x07000000U)
#define  RCC_CFGR_MCO1PRE_0                  ((uint32_t)0x01000000U)
#define  RCC_CFGR_MCO1PRE_1                  ((uint32_t)0x02000000U)
#define  RCC_CFGR_MCO1PRE_2                  ((uint32_t)0x04000000U)

#define  RCC_CFGR_MCO2PRE                    ((uint32_t)0x38000000U)
#define  RCC_CFGR_MCO2PRE_0                  ((uint32_t)0x08000000U)
#define  RCC_CFGR_MCO2PRE_1                  ((uint32_t)0x10000000U)
#define  RCC_CFGR_MCO2PRE_2                  ((uint32_t)0x20000000U)

#define  RCC_CFGR_MCO2                       ((uint32_t)0xC0000000U)
#define  RCC_CFGR_MCO2_0                     ((uint32_t)0x40000000U)
#define  RCC_CFGR_MCO2_1                     ((uint32_t)0x80000000U)

/********************  Bit definition for RCC_CIR register  *******************/
#define  RCC_CIR_LSIRDYF                     ((uint32_t)0x00000001U)
#define  RCC_CIR_LSERDYF                     ((uint32_t)0x00000002U)
#define  RCC_CIR_HSIRDYF                     ((uint32_t)0x00000004U)
#define  RCC_CIR_HSERDYF                     ((uint32_t)0x00000008U)
#define  RCC_CIR_PLLRDYF                     ((uint32_t)0x00000010U)
#define  RCC_CIR_PLLI2SRDYF                  ((uint32_t)0x00000020U)
#define  RCC_CIR_PLLSAIRDYF                  ((uint32_t)0x00000040U)
#define  RCC_CIR_CSSF                        ((uint32_t)0x00000080U)
#define  RCC_CIR_LSIRDYIE                    ((uint32_t)0x00000100U)
#define  RCC_CIR_LSERDYIE                    ((uint32_t)0x00000200U)
#define  RCC_CIR_HSIRDYIE                    ((uint32_t)0x00000400U)
#define  RCC_CIR_HSERDYIE                    ((uint32_t)0x00000800U)
#define  RCC_CIR_PLLRDYIE                    ((uint32_t)0x00001000U)
#define  RCC_CIR_PLLI2SRDYIE                 ((uint32_t)0x00002000U)
#define  RCC_CIR_PLLSAIRDYIE                 ((uint32_t)0x00004000U)
#define  RCC_CIR_LSIRDYC                     ((uint32_t)0x00010000U)
#define  RCC_CIR_LSERDYC                     ((uint32_t)0x00020000U)
#define  RCC_CIR_HSIRDYC                     ((uint32_t)0x00040000U)
#define  RCC_CIR_HSERDYC                     ((uint32_t)0x00080000U)
#define  RCC_CIR_PLLRDYC                     ((uint32_t)0x00100000U)
#define  RCC_CIR_PLLI2SRDYC                  ((uint32_t)0x00200000U)
#define  RCC_CIR_PLLSAIRDYC                  ((uint32_t)0x00400000U)
#define  RCC_CIR_CSSC                        ((uint32_t)0x00800000U)

/********************  Bit definition for RCC_AHB1RSTR register  **************/
#define  RCC_AHB1RSTR_GPIOARST               ((uint32_t)0x00000001U)
#define  RCC_AHB1RSTR_GPIOBRST               ((uint32_t)0x00000002U)
#define  RCC_AHB1RSTR_GPIOCRST               ((uint32_t)0x00000004U)
#define  RCC_AHB1RSTR_GPIODRST               ((uint32_t)0x00000008U)
#define  RCC_AHB1RSTR_GPIOERST               ((uint32_t)0x00000010U)
#define  RCC_AHB1RSTR_GPIOFRST               ((uint32_t)0x00000020U)
#define  RCC_AHB1RSTR_GPIOGRST               ((uint32_t)0x00000040U)
#define  RCC_AHB1RSTR_GPIOHRST               ((uint32_t)0x00000080U)
#define  RCC_AHB1RSTR_GPIOIRST               ((uint32_t)0x00000100U)
#define  RCC_AHB1RSTR_GPIOJRST               ((uint32_t)0x00000200U)
#define  RCC_AHB1RSTR_GPIOKRST               ((uint32_t)0x00000400U)
#define  RCC_AHB1RSTR_CRCRST                 ((uint32_t)0x00001000U)
#define  RCC_AHB1RSTR_DMA1RST                ((uint32_t)0x00200000U)
#define  RCC_AHB1RSTR_DMA2RST                ((uint32_t)0x00400000U)
#define  RCC_AHB1RSTR_DMA2DRST               ((uint32_t)0x00800000U)
#define  RCC_AHB1RSTR_ETHMACRST              ((uint32_t)0x02000000U)
#define  RCC_AHB1RSTR_OTGHRST                ((uint32_t)0x20000000U)

/********************  Bit definition for RCC_AHB2RSTR register  **************/
#define  RCC_AHB2RSTR_DCMIRST                ((uint32_t)0x00000001U)
#define  RCC_AHB2RSTR_RNGRST                 ((uint32_t)0x00000040U)
#define  RCC_AHB2RSTR_OTGFSRST               ((uint32_t)0x00000080U)

/********************  Bit definition for RCC_AHB3RSTR register  **************/
#define  RCC_AHB3RSTR_FMCRST                ((uint32_t)0x00000001U)

/********************  Bit definition for RCC_APB1RSTR register  **************/
#define  RCC_APB1RSTR_TIM2RST                ((uint32_t)0x00000001U)
#define  RCC_APB1RSTR_TIM3RST                ((uint32_t)0x00000002U)
#define  RCC_APB1RSTR_TIM4RST                ((uint32_t)0x00000004U)
#define  RCC_APB1RSTR_TIM5RST                ((uint32_t)0x00000008U)
#define  RCC_APB1RSTR_TIM6RST                ((uint32_t)0x00000010U)
#define  RCC_APB1RSTR_TIM7RST                ((uint32_t)0x00000020U)
#define  RCC_APB1RSTR_TIM12RST               ((uint32_t)0x00000040U)
#define  RCC_APB1RSTR_TIM13RST               ((uint32_t)0x00000080U)
#define  RCC_APB1RSTR_TIM14RST               ((uint32_t)0x00000100U)
#define  RCC_APB1RSTR_WWDGRST                ((uint32_t)0x00000800U)
#define  RCC_APB1RSTR_SPI2RST                ((uint32_t)0x00004000U)
#define  RCC_APB1RSTR_SPI3RST                ((uint32_t)0x00008000U)
#define  RCC_APB1RSTR_USART2RST              ((uint32_t)0x00020000U)
#define  RCC_APB1RSTR_USART3RST              ((uint32_t)0x00040000U)
#define  RCC_APB1RSTR_UART4RST               ((uint32_t)0x00080000U)
#define  RCC_APB1RSTR_UART5RST               ((uint32_t)0x00100000U)
#define  RCC_APB1RSTR_I2C1RST                ((uint32_t)0x00200000U)
#define  RCC_APB1RSTR_I2C2RST                ((uint32_t)0x00400000U)
#define  RCC_APB1RSTR_I2C3RST                ((uint32_t)0x00800000U)
#define  RCC_APB1RSTR_CAN1RST                ((uint32_t)0x02000000U)
#define  RCC_APB1RSTR_CAN2RST                ((uint32_t)0x04000000U)
#define  RCC_APB1RSTR_PWRRST                 ((uint32_t)0x10000000U)
#define  RCC_APB1RSTR_DACRST                 ((uint32_t)0x20000000U)
#define  RCC_APB1RSTR_UART7RST               ((uint32_t)0x40000000U)
#define  RCC_APB1RSTR_UART8RST               ((uint32_t)0x80000000U)

/********************  Bit definition for RCC_APB2RSTR register  **************/
#define  RCC_APB2RSTR_TIM1RST                ((uint32_t)0x00000001U)
#define  RCC_APB2RSTR_TIM8RST                ((uint32_t)0x00000002U)
#define  RCC_APB2RSTR_USART1RST              ((uint32_t)0x00000010U)
#define  RCC_APB2RSTR_USART6RST              ((uint32_t)0x00000020U)
#define  RCC_APB2RSTR_ADCRST                 ((uint32_t)0x00000100U)
#define  RCC_APB2RSTR_SDIORST                ((uint32_t)0x00000800U)
#define  RCC_APB2RSTR_SPI1RST                ((uint32_t)0x00001000U)
#define  RCC_APB2RSTR_SPI4RST                ((uint32_t)0x00002000U)
#define  RCC_APB2RSTR_SYSCFGRST              ((uint32_t)0x00004000U)
#define  RCC_APB2RSTR_TIM9RST                ((uint32_t)0x00010000U)
#define  RCC_APB2RSTR_TIM10RST               ((uint32_t)0x00020000U)
#define  RCC_APB2RSTR_TIM11RST               ((uint32_t)0x00040000U)
#define  RCC_APB2RSTR_SPI5RST                ((uint32_t)0x00100000U)
#define  RCC_APB2RSTR_SPI6RST                ((uint32_t)0x00200000U)
#define  RCC_APB2RSTR_SAI1RST                ((uint32_t)0x00400000U)
#define  RCC_APB2RSTR_LTDCRST                ((uint32_t)0x04000000U)

/* Old SPI1RST bit definition, maintained for legacy purpose */
#define  RCC_APB2RSTR_SPI1                   RCC_APB2RSTR_SPI1RST

/********************  Bit definition for RCC_AHB1ENR register  ***************/
#define  RCC_AHB1ENR_GPIOAEN                 ((uint32_t)0x00000001U)
#define  RCC_AHB1ENR_GPIOBEN                 ((uint32_t)0x00000002U)
#define  RCC_AHB1ENR_GPIOCEN                 ((uint32_t)0x00000004U)
#define  RCC_AHB1ENR_GPIODEN                 ((uint32_t)0x00000008U)
#define  RCC_AHB1ENR_GPIOEEN                 ((uint32_t)0x00000010U)
#define  RCC_AHB1ENR_GPIOFEN                 ((uint32_t)0x00000020U)
#define  RCC_AHB1ENR_GPIOGEN                 ((uint32_t)0x00000040U)
#define  RCC_AHB1ENR_GPIOHEN                 ((uint32_t)0x00000080U)
#define  RCC_AHB1ENR_GPIOIEN                 ((uint32_t)0x00000100U)
#define  RCC_AHB1ENR_GPIOJEN                 ((uint32_t)0x00000200U)
#define  RCC_AHB1ENR_GPIOKEN                 ((uint32_t)0x00000400U)

#define  RCC_AHB1ENR_CRCEN                   ((uint32_t)0x00001000U)
#define  RCC_AHB1ENR_BKPSRAMEN               ((uint32_t)0x00040000U)
#define  RCC_AHB1ENR_CCMDATARAMEN            ((uint32_t)0x00100000U)
#define  RCC_AHB1ENR_DMA1EN                  ((uint32_t)0x00200000U)
#define  RCC_AHB1ENR_DMA2EN                  ((uint32_t)0x00400000U)
#define  RCC_AHB1ENR_DMA2DEN                 ((uint32_t)0x00800000U)

#define  RCC_AHB1ENR_ETHMACEN                ((uint32_t)0x02000000U)
#define  RCC_AHB1ENR_ETHMACTXEN              ((uint32_t)0x04000000U)
#define  RCC_AHB1ENR_ETHMACRXEN              ((uint32_t)0x08000000U)
#define  RCC_AHB1ENR_ETHMACPTPEN             ((uint32_t)0x10000000U)
#define  RCC_AHB1ENR_OTGHSEN                 ((uint32_t)0x20000000U)
#define  RCC_AHB1ENR_OTGHSULPIEN             ((uint32_t)0x40000000U)

/********************  Bit definition for RCC_AHB2ENR register  ***************/
#define  RCC_AHB2ENR_DCMIEN                  ((uint32_t)0x00000001U)
#define  RCC_AHB2ENR_RNGEN                   ((uint32_t)0x00000040U)
#define  RCC_AHB2ENR_OTGFSEN                 ((uint32_t)0x00000080U)

/********************  Bit definition for RCC_AHB3ENR register  ***************/
#define  RCC_AHB3ENR_FMCEN                  ((uint32_t)0x00000001U)

/********************  Bit definition for RCC_APB1ENR register  ***************/
#define  RCC_APB1ENR_TIM2EN                  ((uint32_t)0x00000001U)
#define  RCC_APB1ENR_TIM3EN                  ((uint32_t)0x00000002U)
#define  RCC_APB1ENR_TIM4EN                  ((uint32_t)0x00000004U)
#define  RCC_APB1ENR_TIM5EN                  ((uint32_t)0x00000008U)
#define  RCC_APB1ENR_TIM6EN                  ((uint32_t)0x00000010U)
#define  RCC_APB1ENR_TIM7EN                  ((uint32_t)0x00000020U)
#define  RCC_APB1ENR_TIM12EN                 ((uint32_t)0x00000040U)
#define  RCC_APB1ENR_TIM13EN                 ((uint32_t)0x00000080U)
#define  RCC_APB1ENR_TIM14EN                 ((uint32_t)0x00000100U)
#define  RCC_APB1ENR_WWDGEN                  ((uint32_t)0x00000800U)
#define  RCC_APB1ENR_SPI2EN                  ((uint32_t)0x00004000U)
#define  RCC_APB1ENR_SPI3EN                  ((uint32_t)0x00008000U)
#define  RCC_APB1ENR_USART2EN                ((uint32_t)0x00020000U)
#define  RCC_APB1ENR_USART3EN                ((uint32_t)0x00040000U)
#define  RCC_APB1ENR_UART4EN                 ((uint32_t)0x00080000U)
#define  RCC_APB1ENR_UART5EN                 ((uint32_t)0x00100000U)
#define  RCC_APB1ENR_I2C1EN                  ((uint32_t)0x00200000U)
#define  RCC_APB1ENR_I2C2EN                  ((uint32_t)0x00400000U)
#define  RCC_APB1ENR_I2C3EN                  ((uint32_t)0x00800000U)
#define  RCC_APB1ENR_CAN1EN                  ((uint32_t)0x02000000U)
#define  RCC_APB1ENR_CAN2EN                  ((uint32_t)0x04000000U)
#define  RCC_APB1ENR_PWREN                   ((uint32_t)0x10000000U)
#define  RCC_APB1ENR_DACEN                   ((uint32_t)0x20000000U)
#define  RCC_APB1ENR_UART7EN                 ((uint32_t)0x40000000U)
#define  RCC_APB1ENR_UART8EN                 ((uint32_t)0x80000000U)

/********************  Bit definition for RCC_APB2ENR register  ***************/
#define  RCC_APB2ENR_TIM1EN                  ((uint32_t)0x00000001U)
#define  RCC_APB2ENR_TIM8EN                  ((uint32_t)0x00000002U)
#define  RCC_APB2ENR_USART1EN                ((uint32_t)0x00000010U)
#define  RCC_APB2ENR_USART6EN                ((uint32_t)0x00000020U)
#define  RCC_APB2ENR_ADC1EN                  ((uint32_t)0x00000100U)
#define  RCC_APB2ENR_ADC2EN                  ((uint32_t)0x00000200U)
#define  RCC_APB2ENR_ADC3EN                  ((uint32_t)0x00000400U)
#define  RCC_APB2ENR_SDIOEN                  ((uint32_t)0x00000800U)
#define  RCC_APB2ENR_SPI1EN                  ((uint32_t)0x00001000U)
#define  RCC_APB2ENR_SPI4EN                  ((uint32_t)0x00002000U)
#define  RCC_APB2ENR_SYSCFGEN                ((uint32_t)0x00004000U)
#define  RCC_APB2ENR_TIM9EN                  ((uint32_t)0x00010000U)
#define  RCC_APB2ENR_TIM10EN                 ((uint32_t)0x00020000U)
#define  RCC_APB2ENR_TIM11EN                 ((uint32_t)0x00040000U)
#define  RCC_APB2ENR_SPI5EN                  ((uint32_t)0x00100000U)
#define  RCC_APB2ENR_SPI6EN                  ((uint32_t)0x00200000U)
#define  RCC_APB2ENR_SAI1EN                  ((uint32_t)0x00400000U)
#define  RCC_APB2ENR_LTDCEN                  ((uint32_t)0x04000000U)

/********************  Bit definition for RCC_AHB1LPENR register  *************/
#define  RCC_AHB1LPENR_GPIOALPEN             ((uint32_t)0x00000001U)
#define  RCC_AHB1LPENR_GPIOBLPEN             ((uint32_t)0x00000002U)
#define  RCC_AHB1LPENR_GPIOCLPEN             ((uint32_t)0x00000004U)
#define  RCC_AHB1LPENR_GPIODLPEN             ((uint32_t)0x00000008U)
#define  RCC_AHB1LPENR_GPIOELPEN             ((uint32_t)0x00000010U)
#define  RCC_AHB1LPENR_GPIOFLPEN             ((uint32_t)0x00000020U)
#define  RCC_AHB1LPENR_GPIOGLPEN             ((uint32_t)0x00000040U)
#define  RCC_AHB1LPENR_GPIOHLPEN             ((uint32_t)0x00000080U)
#define  RCC_AHB1LPENR_GPIOILPEN             ((uint32_t)0x00000100U)
#define  RCC_AHB1LPENR_GPIOJLPEN             ((uint32_t)0x00000200U)
#define  RCC_AHB1LPENR_GPIOKLPEN             ((uint32_t)0x00000400U)

#define  RCC_AHB1LPENR_CRCLPEN               ((uint32_t)0x00001000U)
#define  RCC_AHB1LPENR_FLITFLPEN             ((uint32_t)0x00008000U)
#define  RCC_AHB1LPENR_SRAM1LPEN             ((uint32_t)0x00010000U)
#define  RCC_AHB1LPENR_SRAM2LPEN             ((uint32_t)0x00020000U)
#define  RCC_AHB1LPENR_BKPSRAMLPEN           ((uint32_t)0x00040000U)
#define  RCC_AHB1LPENR_SRAM3LPEN             ((uint32_t)0x00080000U)
#define  RCC_AHB1LPENR_DMA1LPEN              ((uint32_t)0x00200000U)
#define  RCC_AHB1LPENR_DMA2LPEN              ((uint32_t)0x00400000U)
#define  RCC_AHB1LPENR_DMA2DLPEN             ((uint32_t)0x00800000U)

#define  RCC_AHB1LPENR_ETHMACLPEN            ((uint32_t)0x02000000U)
#define  RCC_AHB1LPENR_ETHMACTXLPEN          ((uint32_t)0x04000000U)
#define  RCC_AHB1LPENR_ETHMACRXLPEN          ((uint32_t)0x08000000U)
#define  RCC_AHB1LPENR_ETHMACPTPLPEN         ((uint32_t)0x10000000U)
#define  RCC_AHB1LPENR_OTGHSLPEN             ((uint32_t)0x20000000U)
#define  RCC_AHB1LPENR_OTGHSULPILPEN         ((uint32_t)0x40000000U)

/********************  Bit definition for RCC_AHB2LPENR register  *************/
#define  RCC_AHB2LPENR_DCMILPEN              ((uint32_t)0x00000001U)
#define  RCC_AHB2LPENR_RNGLPEN               ((uint32_t)0x00000040U)
#define  RCC_AHB2LPENR_OTGFSLPEN             ((uint32_t)0x00000080U)

/********************  Bit definition for RCC_AHB3LPENR register  *************/
#define  RCC_AHB3LPENR_FMCLPEN              ((uint32_t)0x00000001U)

/********************  Bit definition for RCC_APB1LPENR register  *************/
#define  RCC_APB1LPENR_TIM2LPEN              ((uint32_t)0x00000001U)
#define  RCC_APB1LPENR_TIM3LPEN              ((uint32_t)0x00000002U)
#define  RCC_APB1LPENR_TIM4LPEN              ((uint32_t)0x00000004U)
#define  RCC_APB1LPENR_TIM5LPEN              ((uint32_t)0x00000008U)
#define  RCC_APB1LPENR_TIM6LPEN              ((uint32_t)0x00000010U)
#define  RCC_APB1LPENR_TIM7LPEN              ((uint32_t)0x00000020U)
#define  RCC_APB1LPENR_TIM12LPEN             ((uint32_t)0x00000040U)
#define  RCC_APB1LPENR_TIM13LPEN             ((uint32_t)0x00000080U)
#define  RCC_APB1LPENR_TIM14LPEN             ((uint32_t)0x00000100U)
#define  RCC_APB1LPENR_WWDGLPEN              ((uint32_t)0x00000800U)
#define  RCC_APB1LPENR_SPI2LPEN              ((uint32_t)0x00004000U)
#define  RCC_APB1LPENR_SPI3LPEN              ((uint32_t)0x00008000U)
#define  RCC_APB1LPENR_USART2LPEN            ((uint32_t)0x00020000U)
#define  RCC_APB1LPENR_USART3LPEN            ((uint32_t)0x00040000U)
#define  RCC_APB1LPENR_UART4LPEN             ((uint32_t)0x00080000U)
#define  RCC_APB1LPENR_UART5LPEN             ((uint32_t)0x00100000U)
#define  RCC_APB1LPENR_I2C1LPEN              ((uint32_t)0x00200000U)
#define  RCC_APB1LPENR_I2C2LPEN              ((uint32_t)0x00400000U)
#define  RCC_APB1LPENR_I2C3LPEN              ((uint32_t)0x00800000U)
#define  RCC_APB1LPENR_CAN1LPEN              ((uint32_t)0x02000000U)
#define  RCC_APB1LPENR_CAN2LPEN              ((uint32_t)0x04000000U)
#define  RCC_APB1LPENR_PWRLPEN               ((uint32_t)0x10000000U)
#define  RCC_APB1LPENR_DACLPEN               ((uint32_t)0x20000000U)
#define  RCC_APB1LPENR_UART7LPEN             ((uint32_t)0x40000000U)
#define  RCC_APB1LPENR_UART8LPEN             ((uint32_t)0x80000000U)

/********************  Bit definition for RCC_APB2LPENR register  *************/
#define  RCC_APB2LPENR_TIM1LPEN              ((uint32_t)0x00000001U)
#define  RCC_APB2LPENR_TIM8LPEN              ((uint32_t)0x00000002U)
#define  RCC_APB2LPENR_USART1LPEN            ((uint32_t)0x00000010U)
#define  RCC_APB2LPENR_USART6LPEN            ((uint32_t)0x00000020U)
#define  RCC_APB2LPENR_ADC1LPEN              ((uint32_t)0x00000100U)
#define  RCC_APB2LPENR_ADC2LPEN              ((uint32_t)0x00000200U)
#define  RCC_APB2LPENR_ADC3LPEN              ((uint32_t)0x00000400U)
#define  RCC_APB2LPENR_SDIOLPEN              ((uint32_t)0x00000800U)
#define  RCC_APB2LPENR_SPI1LPEN              ((uint32_t)0x00001000U)
#define  RCC_APB2LPENR_SPI4LPEN              ((uint32_t)0x00002000U)
#define  RCC_APB2LPENR_SYSCFGLPEN            ((uint32_t)0x00004000U)
#define  RCC_APB2LPENR_TIM9LPEN              ((uint32_t)0x00010000U)
#define  RCC_APB2LPENR_TIM10LPEN             ((uint32_t)0x00020000U)
#define  RCC_APB2LPENR_TIM11LPEN             ((uint32_t)0x00040000U)
#define  RCC_APB2LPENR_SPI5LPEN              ((uint32_t)0x00100000U)
#define  RCC_APB2LPENR_SPI6LPEN              ((uint32_t)0x00200000U)
#define  RCC_APB2LPENR_SAI1LPEN              ((uint32_t)0x00400000U)
#define  RCC_APB2LPENR_LTDCLPEN              ((uint32_t)0x04000000U)

/********************  Bit definition for RCC_BDCR register  ******************/
#define  RCC_BDCR_LSEON                      ((uint32_t)0x00000001U)
#define  RCC_BDCR_LSERDY                     ((uint32_t)0x00000002U)
#define  RCC_BDCR_LSEBYP                     ((uint32_t)0x00000004U)

#define  RCC_BDCR_RTCSEL                    ((uint32_t)0x00000300U)
#define  RCC_BDCR_RTCSEL_0                  ((uint32_t)0x00000100U)
#define  RCC_BDCR_RTCSEL_1                  ((uint32_t)0x00000200U)

#define  RCC_BDCR_RTCEN                      ((uint32_t)0x00008000U)
#define  RCC_BDCR_BDRST                      ((uint32_t)0x00010000U)

/********************  Bit definition for RCC_CSR register  *******************/
#define  RCC_CSR_LSION                       ((uint32_t)0x00000001U)
#define  RCC_CSR_LSIRDY                      ((uint32_t)0x00000002U)
#define  RCC_CSR_RMVF                        ((uint32_t)0x01000000U)
#define  RCC_CSR_BORRSTF                     ((uint32_t)0x02000000U)
#define  RCC_CSR_PADRSTF                     ((uint32_t)0x04000000U)
#define  RCC_CSR_PORRSTF                     ((uint32_t)0x08000000U)
#define  RCC_CSR_SFTRSTF                     ((uint32_t)0x10000000U)
#define  RCC_CSR_WDGRSTF                     ((uint32_t)0x20000000U)
#define  RCC_CSR_WWDGRSTF                    ((uint32_t)0x40000000U)
#define  RCC_CSR_LPWRRSTF                    ((uint32_t)0x80000000U)

/********************  Bit definition for RCC_SSCGR register  *****************/
#define  RCC_SSCGR_MODPER                    ((uint32_t)0x00001FFFU)
#define  RCC_SSCGR_INCSTEP                   ((uint32_t)0x0FFFE000U)
#define  RCC_SSCGR_SPREADSEL                 ((uint32_t)0x40000000U)
#define  RCC_SSCGR_SSCGEN                    ((uint32_t)0x80000000U)

/********************  Bit definition for RCC_PLLI2SCFGR register  ************/
#define  RCC_PLLI2SCFGR_PLLI2SN              ((uint32_t)0x00007FC0U)
#define  RCC_PLLI2SCFGR_PLLI2SN_0            ((uint32_t)0x00000040U)
#define  RCC_PLLI2SCFGR_PLLI2SN_1            ((uint32_t)0x00000080U)
#define  RCC_PLLI2SCFGR_PLLI2SN_2            ((uint32_t)0x00000100U)
#define  RCC_PLLI2SCFGR_PLLI2SN_3            ((uint32_t)0x00000200U)
#define  RCC_PLLI2SCFGR_PLLI2SN_4            ((uint32_t)0x00000400U)
#define  RCC_PLLI2SCFGR_PLLI2SN_5            ((uint32_t)0x00000800U)
#define  RCC_PLLI2SCFGR_PLLI2SN_6            ((uint32_t)0x00001000U)
#define  RCC_PLLI2SCFGR_PLLI2SN_7            ((uint32_t)0x00002000U)
#define  RCC_PLLI2SCFGR_PLLI2SN_8            ((uint32_t)0x00004000U)

#define  RCC_PLLI2SCFGR_PLLI2SQ              ((uint32_t)0x0F000000U)
#define  RCC_PLLI2SCFGR_PLLI2SQ_0            ((uint32_t)0x01000000U)
#define  RCC_PLLI2SCFGR_PLLI2SQ_1            ((uint32_t)0x02000000U)
#define  RCC_PLLI2SCFGR_PLLI2SQ_2            ((uint32_t)0x04000000U)
#define  RCC_PLLI2SCFGR_PLLI2SQ_3            ((uint32_t)0x08000000U)

#define  RCC_PLLI2SCFGR_PLLI2SR              ((uint32_t)0x70000000U)
#define  RCC_PLLI2SCFGR_PLLI2SR_0            ((uint32_t)0x10000000U)
#define  RCC_PLLI2SCFGR_PLLI2SR_1            ((uint32_t)0x20000000U)
#define  RCC_PLLI2SCFGR_PLLI2SR_2            ((uint32_t)0x40000000U)


/********************  Bit definition for RCC_PLLSAICFGR register  ************/
#define  RCC_PLLSAICFGR_PLLSAIN              ((uint32_t)0x00007FC0U)
#define  RCC_PLLSAICFGR_PLLSAIN_0            ((uint32_t)0x00000040U)
#define  RCC_PLLSAICFGR_PLLSAIN_1            ((uint32_t)0x00000080U)
#define  RCC_PLLSAICFGR_PLLSAIN_2            ((uint32_t)0x00000100U)
#define  RCC_PLLSAICFGR_PLLSAIN_3            ((uint32_t)0x00000200U)
#define  RCC_PLLSAICFGR_PLLSAIN_4            ((uint32_t)0x00000400U)
#define  RCC_PLLSAICFGR_PLLSAIN_5            ((uint32_t)0x00000800U)
#define  RCC_PLLSAICFGR_PLLSAIN_6            ((uint32_t)0x00001000U)
#define  RCC_PLLSAICFGR_PLLSAIN_7            ((uint32_t)0x00002000U)
#define  RCC_PLLSAICFGR_PLLSAIN_8            ((uint32_t)0x00004000U)

#define  RCC_PLLSAICFGR_PLLSAIQ              ((uint32_t)0x0F000000U)
#define  RCC_PLLSAICFGR_PLLSAIQ_0            ((uint32_t)0x01000000U)
#define  RCC_PLLSAICFGR_PLLSAIQ_1            ((uint32_t)0x02000000U)
#define  RCC_PLLSAICFGR_PLLSAIQ_2            ((uint32_t)0x04000000U)
#define  RCC_PLLSAICFGR_PLLSAIQ_3            ((uint32_t)0x08000000U)

#define  RCC_PLLSAICFGR_PLLSAIR              ((uint32_t)0x70000000U)
#define  RCC_PLLSAICFGR_PLLSAIR_0            ((uint32_t)0x10000000U)
#define  RCC_PLLSAICFGR_PLLSAIR_1            ((uint32_t)0x20000000U)
#define  RCC_PLLSAICFGR_PLLSAIR_2            ((uint32_t)0x40000000U)

/********************  Bit definition for RCC_DCKCFGR register  ***************/
#define  RCC_DCKCFGR_PLLI2SDIVQ              ((uint32_t)0x0000001FU)
#define  RCC_DCKCFGR_PLLSAIDIVQ              ((uint32_t)0x00001F00U)
#define  RCC_DCKCFGR_PLLSAIDIVR              ((uint32_t)0x00030000U)
#define  RCC_DCKCFGR_SAI1ASRC                ((uint32_t)0x00300000U)
#define  RCC_DCKCFGR_SAI1ASRC_0              ((uint32_t)0x00100000U)
#define  RCC_DCKCFGR_SAI1ASRC_1              ((uint32_t)0x00200000U)
#define  RCC_DCKCFGR_SAI1BSRC                ((uint32_t)0x00C00000U)
#define  RCC_DCKCFGR_SAI1BSRC_0              ((uint32_t)0x00400000U)
#define  RCC_DCKCFGR_SAI1BSRC_1              ((uint32_t)0x00800000U)
#define  RCC_DCKCFGR_TIMPRE                  ((uint32_t)0x01000000U)


/******************************************************************************/
/*                                                                            */
/*                                    RNG                                     */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RNG_CR register  *******************/
#define RNG_CR_RNGEN                         ((uint32_t)0x00000004U)
#define RNG_CR_IE                            ((uint32_t)0x00000008U)

/********************  Bits definition for RNG_SR register  *******************/
#define RNG_SR_DRDY                          ((uint32_t)0x00000001U)
#define RNG_SR_CECS                          ((uint32_t)0x00000002U)
#define RNG_SR_SECS                          ((uint32_t)0x00000004U)
#define RNG_SR_CEIS                          ((uint32_t)0x00000020U)
#define RNG_SR_SEIS                          ((uint32_t)0x00000040U)

/******************************************************************************/
/*                                                                            */
/*                           Real-Time Clock (RTC)                            */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RTC_TR register  *******************/
#define RTC_TR_PM                            ((uint32_t)0x00400000U)
#define RTC_TR_HT                            ((uint32_t)0x00300000U)
#define RTC_TR_HT_0                          ((uint32_t)0x00100000U)
#define RTC_TR_HT_1                          ((uint32_t)0x00200000U)
#define RTC_TR_HU                            ((uint32_t)0x000F0000U)
#define RTC_TR_HU_0                          ((uint32_t)0x00010000U)
#define RTC_TR_HU_1                          ((uint32_t)0x00020000U)
#define RTC_TR_HU_2                          ((uint32_t)0x00040000U)
#define RTC_TR_HU_3                          ((uint32_t)0x00080000U)
#define RTC_TR_MNT                           ((uint32_t)0x00007000U)
#define RTC_TR_MNT_0                         ((uint32_t)0x00001000U)
#define RTC_TR_MNT_1                         ((uint32_t)0x00002000U)
#define RTC_TR_MNT_2                         ((uint32_t)0x00004000U)
#define RTC_TR_MNU                           ((uint32_t)0x00000F00U)
#define RTC_TR_MNU_0                         ((uint32_t)0x00000100U)
#define RTC_TR_MNU_1                         ((uint32_t)0x00000200U)
#define RTC_TR_MNU_2                         ((uint32_t)0x00000400U)
#define RTC_TR_MNU_3                         ((uint32_t)0x00000800U)
#define RTC_TR_ST                            ((uint32_t)0x00000070U)
#define RTC_TR_ST_0                          ((uint32_t)0x00000010U)
#define RTC_TR_ST_1                          ((uint32_t)0x00000020U)
#define RTC_TR_ST_2                          ((uint32_t)0x00000040U)
#define RTC_TR_SU                            ((uint32_t)0x0000000FU)
#define RTC_TR_SU_0                          ((uint32_t)0x00000001U)
#define RTC_TR_SU_1                          ((uint32_t)0x00000002U)
#define RTC_TR_SU_2                          ((uint32_t)0x00000004U)
#define RTC_TR_SU_3                          ((uint32_t)0x00000008U)

/********************  Bits definition for RTC_DR register  *******************/
#define RTC_DR_YT                            ((uint32_t)0x00F00000U)
#define RTC_DR_YT_0                          ((uint32_t)0x00100000U)
#define RTC_DR_YT_1                          ((uint32_t)0x00200000U)
#define RTC_DR_YT_2                          ((uint32_t)0x00400000U)
#define RTC_DR_YT_3                          ((uint32_t)0x00800000U)
#define RTC_DR_YU                            ((uint32_t)0x000F0000U)
#define RTC_DR_YU_0                          ((uint32_t)0x00010000U)
#define RTC_DR_YU_1                          ((uint32_t)0x00020000U)
#define RTC_DR_YU_2                          ((uint32_t)0x00040000U)
#define RTC_DR_YU_3                          ((uint32_t)0x00080000U)
#define RTC_DR_WDU                           ((uint32_t)0x0000E000U)
#define RTC_DR_WDU_0                         ((uint32_t)0x00002000U)
#define RTC_DR_WDU_1                         ((uint32_t)0x00004000U)
#define RTC_DR_WDU_2                         ((uint32_t)0x00008000U)
#define RTC_DR_MT                            ((uint32_t)0x00001000U)
#define RTC_DR_MU                            ((uint32_t)0x00000F00U)
#define RTC_DR_MU_0                          ((uint32_t)0x00000100U)
#define RTC_DR_MU_1                          ((uint32_t)0x00000200U)
#define RTC_DR_MU_2                          ((uint32_t)0x00000400U)
#define RTC_DR_MU_3                          ((uint32_t)0x00000800U)
#define RTC_DR_DT                            ((uint32_t)0x00000030U)
#define RTC_DR_DT_0                          ((uint32_t)0x00000010U)
#define RTC_DR_DT_1                          ((uint32_t)0x00000020U)
#define RTC_DR_DU                            ((uint32_t)0x0000000FU)
#define RTC_DR_DU_0                          ((uint32_t)0x00000001U)
#define RTC_DR_DU_1                          ((uint32_t)0x00000002U)
#define RTC_DR_DU_2                          ((uint32_t)0x00000004U)
#define RTC_DR_DU_3                          ((uint32_t)0x00000008U)

/********************  Bits definition for RTC_CR register  *******************/
#define RTC_CR_COE                           ((uint32_t)0x00800000U)
#define RTC_CR_OSEL                          ((uint32_t)0x00600000U)
#define RTC_CR_OSEL_0                        ((uint32_t)0x00200000U)
#define RTC_CR_OSEL_1                        ((uint32_t)0x00400000U)
#define RTC_CR_POL                           ((uint32_t)0x00100000U)
#define RTC_CR_COSEL                         ((uint32_t)0x00080000U)
#define RTC_CR_BCK                           ((uint32_t)0x00040000U)
#define RTC_CR_SUB1H                         ((uint32_t)0x00020000U)
#define RTC_CR_ADD1H                         ((uint32_t)0x00010000U)
#define RTC_CR_TSIE                          ((uint32_t)0x00008000U)
#define RTC_CR_WUTIE                         ((uint32_t)0x00004000U)
#define RTC_CR_ALRBIE                        ((uint32_t)0x00002000U)
#define RTC_CR_ALRAIE                        ((uint32_t)0x00001000U)
#define RTC_CR_TSE                           ((uint32_t)0x00000800U)
#define RTC_CR_WUTE                          ((uint32_t)0x00000400U)
#define RTC_CR_ALRBE                         ((uint32_t)0x00000200U)
#define RTC_CR_ALRAE                         ((uint32_t)0x00000100U)
#define RTC_CR_DCE                           ((uint32_t)0x00000080U)
#define RTC_CR_FMT                           ((uint32_t)0x00000040U)
#define RTC_CR_BYPSHAD                       ((uint32_t)0x00000020U)
#define RTC_CR_REFCKON                       ((uint32_t)0x00000010U)
#define RTC_CR_TSEDGE                        ((uint32_t)0x00000008U)
#define RTC_CR_WUCKSEL                       ((uint32_t)0x00000007U)
#define RTC_CR_WUCKSEL_0                     ((uint32_t)0x00000001U)
#define RTC_CR_WUCKSEL_1                     ((uint32_t)0x00000002U)
#define RTC_CR_WUCKSEL_2                     ((uint32_t)0x00000004U)

/********************  Bits definition for RTC_ISR register  ******************/
#define RTC_ISR_RECALPF                      ((uint32_t)0x00010000U)
#define RTC_ISR_TAMP1F                       ((uint32_t)0x00002000U)
#define RTC_ISR_TAMP2F                       ((uint32_t)0x00004000U)
#define RTC_ISR_TSOVF                        ((uint32_t)0x00001000U)
#define RTC_ISR_TSF                          ((uint32_t)0x00000800U)
#define RTC_ISR_WUTF                         ((uint32_t)0x00000400U)
#define RTC_ISR_ALRBF                        ((uint32_t)0x00000200U)
#define RTC_ISR_ALRAF                        ((uint32_t)0x00000100U)
#define RTC_ISR_INIT                         ((uint32_t)0x00000080U)
#define RTC_ISR_INITF                        ((uint32_t)0x00000040U)
#define RTC_ISR_RSF                          ((uint32_t)0x00000020U)
#define RTC_ISR_INITS                        ((uint32_t)0x00000010U)
#define RTC_ISR_SHPF                         ((uint32_t)0x00000008U)
#define RTC_ISR_WUTWF                        ((uint32_t)0x00000004U)
#define RTC_ISR_ALRBWF                       ((uint32_t)0x00000002U)
#define RTC_ISR_ALRAWF                       ((uint32_t)0x00000001U)

/********************  Bits definition for RTC_PRER register  *****************/
#define RTC_PRER_PREDIV_A                    ((uint32_t)0x007F0000U)
#define RTC_PRER_PREDIV_S                    ((uint32_t)0x00007FFFU)

/********************  Bits definition for RTC_WUTR register  *****************/
#define RTC_WUTR_WUT                         ((uint32_t)0x0000FFFFU)

/********************  Bits definition for RTC_CALIBR register  ***************/
#define RTC_CALIBR_DCS                       ((uint32_t)0x00000080U)
#define RTC_CALIBR_DC                        ((uint32_t)0x0000001FU)

/********************  Bits definition for RTC_ALRMAR register  ***************/
#define RTC_ALRMAR_MSK4                      ((uint32_t)0x80000000U)
#define RTC_ALRMAR_WDSEL                     ((uint32_t)0x40000000U)
#define RTC_ALRMAR_DT                        ((uint32_t)0x30000000U)
#define RTC_ALRMAR_DT_0                      ((uint32_t)0x10000000U)
#define RTC_ALRMAR_DT_1                      ((uint32_t)0x20000000U)
#define RTC_ALRMAR_DU                        ((uint32_t)0x0F000000U)
#define RTC_ALRMAR_DU_0                      ((uint32_t)0x01000000U)
#define RTC_ALRMAR_DU_1                      ((uint32_t)0x02000000U)
#define RTC_ALRMAR_DU_2                      ((uint32_t)0x04000000U)
#define RTC_ALRMAR_DU_3                      ((uint32_t)0x08000000U)
#define RTC_ALRMAR_MSK3                      ((uint32_t)0x00800000U)
#define RTC_ALRMAR_PM                        ((uint32_t)0x00400000U)
#define RTC_ALRMAR_HT                        ((uint32_t)0x00300000U)
#define RTC_ALRMAR_HT_0                      ((uint32_t)0x00100000U)
#define RTC_ALRMAR_HT_1                      ((uint32_t)0x00200000U)
#define RTC_ALRMAR_HU                        ((uint32_t)0x000F0000U)
#define RTC_ALRMAR_HU_0                      ((uint32_t)0x00010000U)
#define RTC_ALRMAR_HU_1                      ((uint32_t)0x00020000U)
#define RTC_ALRMAR_HU_2                      ((uint32_t)0x00040000U)
#define RTC_ALRMAR_HU_3                      ((uint32_t)0x00080000U)
#define RTC_ALRMAR_MSK2                      ((uint32_t)0x00008000U)
#define RTC_ALRMAR_MNT                       ((uint32_t)0x00007000U)
#define RTC_ALRMAR_MNT_0                     ((uint32_t)0x00001000U)
#define RTC_ALRMAR_MNT_1                     ((uint32_t)0x00002000U)
#define RTC_ALRMAR_MNT_2                     ((uint32_t)0x00004000U)
#define RTC_ALRMAR_MNU                       ((uint32_t)0x00000F00U)
#define RTC_ALRMAR_MNU_0                     ((uint32_t)0x00000100U)
#define RTC_ALRMAR_MNU_1                     ((uint32_t)0x00000200U)
#define RTC_ALRMAR_MNU_2                     ((uint32_t)0x00000400U)
#define RTC_ALRMAR_MNU_3                     ((uint32_t)0x00000800U)
#define RTC_ALRMAR_MSK1                      ((uint32_t)0x00000080U)
#define RTC_ALRMAR_ST                        ((uint32_t)0x00000070U)
#define RTC_ALRMAR_ST_0                      ((uint32_t)0x00000010U)
#define RTC_ALRMAR_ST_1                      ((uint32_t)0x00000020U)
#define RTC_ALRMAR_ST_2                      ((uint32_t)0x00000040U)
#define RTC_ALRMAR_SU                        ((uint32_t)0x0000000FU)
#define RTC_ALRMAR_SU_0                      ((uint32_t)0x00000001U)
#define RTC_ALRMAR_SU_1                      ((uint32_t)0x00000002U)
#define RTC_ALRMAR_SU_2                      ((uint32_t)0x00000004U)
#define RTC_ALRMAR_SU_3                      ((uint32_t)0x00000008U)

/********************  Bits definition for RTC_ALRMBR register  ***************/
#define RTC_ALRMBR_MSK4                      ((uint32_t)0x80000000U)
#define RTC_ALRMBR_WDSEL                     ((uint32_t)0x40000000U)
#define RTC_ALRMBR_DT                        ((uint32_t)0x30000000U)
#define RTC_ALRMBR_DT_0                      ((uint32_t)0x10000000U)
#define RTC_ALRMBR_DT_1                      ((uint32_t)0x20000000U)
#define RTC_ALRMBR_DU                        ((uint32_t)0x0F000000U)
#define RTC_ALRMBR_DU_0                      ((uint32_t)0x01000000U)
#define RTC_ALRMBR_DU_1                      ((uint32_t)0x02000000U)
#define RTC_ALRMBR_DU_2                      ((uint32_t)0x04000000U)
#define RTC_ALRMBR_DU_3                      ((uint32_t)0x08000000U)
#define RTC_ALRMBR_MSK3                      ((uint32_t)0x00800000U)
#define RTC_ALRMBR_PM                        ((uint32_t)0x00400000U)
#define RTC_ALRMBR_HT                        ((uint32_t)0x00300000U)
#define RTC_ALRMBR_HT_0                      ((uint32_t)0x00100000U)
#define RTC_ALRMBR_HT_1                      ((uint32_t)0x00200000U)
#define RTC_ALRMBR_HU                        ((uint32_t)0x000F0000U)
#define RTC_ALRMBR_HU_0                      ((uint32_t)0x00010000U)
#define RTC_ALRMBR_HU_1                      ((uint32_t)0x00020000U)
#define RTC_ALRMBR_HU_2                      ((uint32_t)0x00040000U)
#define RTC_ALRMBR_HU_3                      ((uint32_t)0x00080000U)
#define RTC_ALRMBR_MSK2                      ((uint32_t)0x00008000U)
#define RTC_ALRMBR_MNT                       ((uint32_t)0x00007000U)
#define RTC_ALRMBR_MNT_0                     ((uint32_t)0x00001000U)
#define RTC_ALRMBR_MNT_1                     ((uint32_t)0x00002000U)
#define RTC_ALRMBR_MNT_2                     ((uint32_t)0x00004000U)
#define RTC_ALRMBR_MNU                       ((uint32_t)0x00000F00U)
#define RTC_ALRMBR_MNU_0                     ((uint32_t)0x00000100U)
#define RTC_ALRMBR_MNU_1                     ((uint32_t)0x00000200U)
#define RTC_ALRMBR_MNU_2                     ((uint32_t)0x00000400U)
#define RTC_ALRMBR_MNU_3                     ((uint32_t)0x00000800U)
#define RTC_ALRMBR_MSK1                      ((uint32_t)0x00000080U)
#define RTC_ALRMBR_ST                        ((uint32_t)0x00000070U)
#define RTC_ALRMBR_ST_0                      ((uint32_t)0x00000010U)
#define RTC_ALRMBR_ST_1                      ((uint32_t)0x00000020U)
#define RTC_ALRMBR_ST_2                      ((uint32_t)0x00000040U)
#define RTC_ALRMBR_SU                        ((uint32_t)0x0000000FU)
#define RTC_ALRMBR_SU_0                      ((uint32_t)0x00000001U)
#define RTC_ALRMBR_SU_1                      ((uint32_t)0x00000002U)
#define RTC_ALRMBR_SU_2                      ((uint32_t)0x00000004U)
#define RTC_ALRMBR_SU_3                      ((uint32_t)0x00000008U)

/********************  Bits definition for RTC_WPR register  ******************/
#define RTC_WPR_KEY                          ((uint32_t)0x000000FFU)

/********************  Bits definition for RTC_SSR register  ******************/
#define RTC_SSR_SS                           ((uint32_t)0x0000FFFFU)

/********************  Bits definition for RTC_SHIFTR register  ***************/
#define RTC_SHIFTR_SUBFS                     ((uint32_t)0x00007FFFU)
#define RTC_SHIFTR_ADD1S                     ((uint32_t)0x80000000U)

/********************  Bits definition for RTC_TSTR register  *****************/
#define RTC_TSTR_PM                          ((uint32_t)0x00400000U)
#define RTC_TSTR_HT                          ((uint32_t)0x00300000U)
#define RTC_TSTR_HT_0                        ((uint32_t)0x00100000U)
#define RTC_TSTR_HT_1                        ((uint32_t)0x00200000U)
#define RTC_TSTR_HU                          ((uint32_t)0x000F0000U)
#define RTC_TSTR_HU_0                        ((uint32_t)0x00010000U)
#define RTC_TSTR_HU_1                        ((uint32_t)0x00020000U)
#define RTC_TSTR_HU_2                        ((uint32_t)0x00040000U)
#define RTC_TSTR_HU_3                        ((uint32_t)0x00080000U)
#define RTC_TSTR_MNT                         ((uint32_t)0x00007000U)
#define RTC_TSTR_MNT_0                       ((uint32_t)0x00001000U)
#define RTC_TSTR_MNT_1                       ((uint32_t)0x00002000U)
#define RTC_TSTR_MNT_2                       ((uint32_t)0x00004000U)
#define RTC_TSTR_MNU                         ((uint32_t)0x00000F00U)
#define RTC_TSTR_MNU_0                       ((uint32_t)0x00000100U)
#define RTC_TSTR_MNU_1                       ((uint32_t)0x00000200U)
#define RTC_TSTR_MNU_2                       ((uint32_t)0x00000400U)
#define RTC_TSTR_MNU_3                       ((uint32_t)0x00000800U)
#define RTC_TSTR_ST                          ((uint32_t)0x00000070U)
#define RTC_TSTR_ST_0                        ((uint32_t)0x00000010U)
#define RTC_TSTR_ST_1                        ((uint32_t)0x00000020U)
#define RTC_TSTR_ST_2                        ((uint32_t)0x00000040U)
#define RTC_TSTR_SU                          ((uint32_t)0x0000000FU)
#define RTC_TSTR_SU_0                        ((uint32_t)0x00000001U)
#define RTC_TSTR_SU_1                        ((uint32_t)0x00000002U)
#define RTC_TSTR_SU_2                        ((uint32_t)0x00000004U)
#define RTC_TSTR_SU_3                        ((uint32_t)0x00000008U)

/********************  Bits definition for RTC_TSDR register  *****************/
#define RTC_TSDR_WDU                         ((uint32_t)0x0000E000U)
#define RTC_TSDR_WDU_0                       ((uint32_t)0x00002000U)
#define RTC_TSDR_WDU_1                       ((uint32_t)0x00004000U)
#define RTC_TSDR_WDU_2                       ((uint32_t)0x00008000U)
#define RTC_TSDR_MT                          ((uint32_t)0x00001000U)
#define RTC_TSDR_MU                          ((uint32_t)0x00000F00U)
#define RTC_TSDR_MU_0                        ((uint32_t)0x00000100U)
#define RTC_TSDR_MU_1                        ((uint32_t)0x00000200U)
#define RTC_TSDR_MU_2                        ((uint32_t)0x00000400U)
#define RTC_TSDR_MU_3                        ((uint32_t)0x00000800U)
#define RTC_TSDR_DT                          ((uint32_t)0x00000030U)
#define RTC_TSDR_DT_0                        ((uint32_t)0x00000010U)
#define RTC_TSDR_DT_1                        ((uint32_t)0x00000020U)
#define RTC_TSDR_DU                          ((uint32_t)0x0000000FU)
#define RTC_TSDR_DU_0                        ((uint32_t)0x00000001U)
#define RTC_TSDR_DU_1                        ((uint32_t)0x00000002U)
#define RTC_TSDR_DU_2                        ((uint32_t)0x00000004U)
#define RTC_TSDR_DU_3                        ((uint32_t)0x00000008U)

/********************  Bits definition for RTC_TSSSR register  ****************/
#define RTC_TSSSR_SS                         ((uint32_t)0x0000FFFFU)

/********************  Bits definition for RTC_CAL register  *****************/
#define RTC_CALR_CALP                        ((uint32_t)0x00008000U)
#define RTC_CALR_CALW8                       ((uint32_t)0x00004000U)
#define RTC_CALR_CALW16                      ((uint32_t)0x00002000U)
#define RTC_CALR_CALM                        ((uint32_t)0x000001FFU)
#define RTC_CALR_CALM_0                      ((uint32_t)0x00000001U)
#define RTC_CALR_CALM_1                      ((uint32_t)0x00000002U)
#define RTC_CALR_CALM_2                      ((uint32_t)0x00000004U)
#define RTC_CALR_CALM_3                      ((uint32_t)0x00000008U)
#define RTC_CALR_CALM_4                      ((uint32_t)0x00000010U)
#define RTC_CALR_CALM_5                      ((uint32_t)0x00000020U)
#define RTC_CALR_CALM_6                      ((uint32_t)0x00000040U)
#define RTC_CALR_CALM_7                      ((uint32_t)0x00000080U)
#define RTC_CALR_CALM_8                      ((uint32_t)0x00000100U)

/********************  Bits definition for RTC_TAFCR register  ****************/
#define RTC_TAFCR_ALARMOUTTYPE               ((uint32_t)0x00040000U)
#define RTC_TAFCR_TSINSEL                    ((uint32_t)0x00020000U)
#define RTC_TAFCR_TAMPINSEL                  ((uint32_t)0x00010000U)
#define RTC_TAFCR_TAMPPUDIS                  ((uint32_t)0x00008000U)
#define RTC_TAFCR_TAMPPRCH                   ((uint32_t)0x00006000U)
#define RTC_TAFCR_TAMPPRCH_0                 ((uint32_t)0x00002000U)
#define RTC_TAFCR_TAMPPRCH_1                 ((uint32_t)0x00004000U)
#define RTC_TAFCR_TAMPFLT                    ((uint32_t)0x00001800U)
#define RTC_TAFCR_TAMPFLT_0                  ((uint32_t)0x00000800U)
#define RTC_TAFCR_TAMPFLT_1                  ((uint32_t)0x00001000U)
#define RTC_TAFCR_TAMPFREQ                   ((uint32_t)0x00000700U)
#define RTC_TAFCR_TAMPFREQ_0                 ((uint32_t)0x00000100U)
#define RTC_TAFCR_TAMPFREQ_1                 ((uint32_t)0x00000200U)
#define RTC_TAFCR_TAMPFREQ_2                 ((uint32_t)0x00000400U)
#define RTC_TAFCR_TAMPTS                     ((uint32_t)0x00000080U)
#define RTC_TAFCR_TAMP2TRG                   ((uint32_t)0x00000010U)
#define RTC_TAFCR_TAMP2E                     ((uint32_t)0x00000008U)
#define RTC_TAFCR_TAMPIE                     ((uint32_t)0x00000004U)
#define RTC_TAFCR_TAMP1TRG                   ((uint32_t)0x00000002U)
#define RTC_TAFCR_TAMP1E                     ((uint32_t)0x00000001U)

/********************  Bits definition for RTC_ALRMASSR register  *************/
#define RTC_ALRMASSR_MASKSS                  ((uint32_t)0x0F000000U)
#define RTC_ALRMASSR_MASKSS_0                ((uint32_t)0x01000000U)
#define RTC_ALRMASSR_MASKSS_1                ((uint32_t)0x02000000U)
#define RTC_ALRMASSR_MASKSS_2                ((uint32_t)0x04000000U)
#define RTC_ALRMASSR_MASKSS_3                ((uint32_t)0x08000000U)
#define RTC_ALRMASSR_SS                      ((uint32_t)0x00007FFFU)

/********************  Bits definition for RTC_ALRMBSSR register  *************/
#define RTC_ALRMBSSR_MASKSS                  ((uint32_t)0x0F000000U)
#define RTC_ALRMBSSR_MASKSS_0                ((uint32_t)0x01000000U)
#define RTC_ALRMBSSR_MASKSS_1                ((uint32_t)0x02000000U)
#define RTC_ALRMBSSR_MASKSS_2                ((uint32_t)0x04000000U)
#define RTC_ALRMBSSR_MASKSS_3                ((uint32_t)0x08000000U)
#define RTC_ALRMBSSR_SS                      ((uint32_t)0x00007FFFU)

/********************  Bits definition for RTC_BKP0R register  ****************/
#define RTC_BKP0R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP1R register  ****************/
#define RTC_BKP1R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP2R register  ****************/
#define RTC_BKP2R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP3R register  ****************/
#define RTC_BKP3R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP4R register  ****************/
#define RTC_BKP4R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP5R register  ****************/
#define RTC_BKP5R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP6R register  ****************/
#define RTC_BKP6R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP7R register  ****************/
#define RTC_BKP7R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP8R register  ****************/
#define RTC_BKP8R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP9R register  ****************/
#define RTC_BKP9R                            ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP10R register  ***************/
#define RTC_BKP10R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP11R register  ***************/
#define RTC_BKP11R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP12R register  ***************/
#define RTC_BKP12R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP13R register  ***************/
#define RTC_BKP13R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP14R register  ***************/
#define RTC_BKP14R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP15R register  ***************/
#define RTC_BKP15R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP16R register  ***************/
#define RTC_BKP16R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP17R register  ***************/
#define RTC_BKP17R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP18R register  ***************/
#define RTC_BKP18R                           ((uint32_t)0xFFFFFFFFU)

/********************  Bits definition for RTC_BKP19R register  ***************/
#define RTC_BKP19R                           ((uint32_t)0xFFFFFFFFU)

/******************************************************************************/
/*                                                                            */
/*                          Serial Audio Interface                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for SAI_GCR register  *******************/
#define  SAI_GCR_SYNCIN                  ((uint32_t)0x00000003U)        /*!<SYNCIN[1:0] bits (Synchronization Inputs)   */
#define  SAI_GCR_SYNCIN_0                ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  SAI_GCR_SYNCIN_1                ((uint32_t)0x00000002U)        /*!<Bit 1 */

#define  SAI_GCR_SYNCOUT                 ((uint32_t)0x00000030U)        /*!<SYNCOUT[1:0] bits (Synchronization Outputs) */
#define  SAI_GCR_SYNCOUT_0               ((uint32_t)0x00000010U)        /*!<Bit 0 */
#define  SAI_GCR_SYNCOUT_1               ((uint32_t)0x00000020U)        /*!<Bit 1 */

/*******************  Bit definition for SAI_xCR1 register  *******************/
#define  SAI_xCR1_MODE                    ((uint32_t)0x00000003U)        /*!<MODE[1:0] bits (Audio Block Mode)           */
#define  SAI_xCR1_MODE_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  SAI_xCR1_MODE_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */

#define  SAI_xCR1_PRTCFG                  ((uint32_t)0x0000000CU)        /*!<PRTCFG[1:0] bits (Protocol Configuration)   */
#define  SAI_xCR1_PRTCFG_0                ((uint32_t)0x00000004U)        /*!<Bit 0 */
#define  SAI_xCR1_PRTCFG_1                ((uint32_t)0x00000008U)        /*!<Bit 1 */

#define  SAI_xCR1_DS                      ((uint32_t)0x000000E0U)        /*!<DS[1:0] bits (Data Size) */
#define  SAI_xCR1_DS_0                    ((uint32_t)0x00000020U)        /*!<Bit 0 */
#define  SAI_xCR1_DS_1                    ((uint32_t)0x00000040U)        /*!<Bit 1 */
#define  SAI_xCR1_DS_2                    ((uint32_t)0x00000080U)        /*!<Bit 2 */

#define  SAI_xCR1_LSBFIRST                ((uint32_t)0x00000100U)        /*!<LSB First Configuration  */
#define  SAI_xCR1_CKSTR                   ((uint32_t)0x00000200U)        /*!<ClocK STRobing edge      */

#define  SAI_xCR1_SYNCEN                  ((uint32_t)0x00000C00U)        /*!<SYNCEN[1:0](SYNChronization ENable) */
#define  SAI_xCR1_SYNCEN_0                ((uint32_t)0x00000400U)        /*!<Bit 0 */
#define  SAI_xCR1_SYNCEN_1                ((uint32_t)0x00000800U)        /*!<Bit 1 */

#define  SAI_xCR1_MONO                    ((uint32_t)0x00001000U)        /*!<Mono mode                  */
#define  SAI_xCR1_OUTDRIV                 ((uint32_t)0x00002000U)        /*!<Output Drive               */
#define  SAI_xCR1_SAIEN                   ((uint32_t)0x00010000U)        /*!<Audio Block enable         */
#define  SAI_xCR1_DMAEN                   ((uint32_t)0x00020000U)        /*!<DMA enable                 */
#define  SAI_xCR1_NODIV                   ((uint32_t)0x00080000U)        /*!<No Divider Configuration   */

#define  SAI_xCR1_MCKDIV                  ((uint32_t)0x00F00000U)        /*!<MCKDIV[3:0] (Master ClocK Divider)  */
#define  SAI_xCR1_MCKDIV_0                ((uint32_t)0x00100000U)        /*!<Bit 0  */
#define  SAI_xCR1_MCKDIV_1                ((uint32_t)0x00200000U)        /*!<Bit 1  */
#define  SAI_xCR1_MCKDIV_2                ((uint32_t)0x00400000U)        /*!<Bit 2  */
#define  SAI_xCR1_MCKDIV_3                ((uint32_t)0x00800000U)        /*!<Bit 3  */

/*******************  Bit definition for SAI_xCR2 register  *******************/
#define  SAI_xCR2_FTH                     ((uint32_t)0x00000007U)        /*!<FTH[2:0](Fifo THreshold)  */
#define  SAI_xCR2_FTH_0                   ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  SAI_xCR2_FTH_1                   ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  SAI_xCR2_FTH_2                   ((uint32_t)0x00000004U)        /*!<Bit 2 */

#define  SAI_xCR2_FFLUSH                  ((uint32_t)0x00000008U)        /*!<Fifo FLUSH                       */
#define  SAI_xCR2_TRIS                    ((uint32_t)0x00000010U)        /*!<TRIState Management on data line */
#define  SAI_xCR2_MUTE                    ((uint32_t)0x00000020U)        /*!<Mute mode                        */
#define  SAI_xCR2_MUTEVAL                 ((uint32_t)0x00000040U)        /*!<Muate value                      */

#define  SAI_xCR2_MUTECNT                  ((uint32_t)0x00001F80U)       /*!<MUTECNT[5:0] (MUTE counter) */
#define  SAI_xCR2_MUTECNT_0               ((uint32_t)0x00000080U)        /*!<Bit 0 */
#define  SAI_xCR2_MUTECNT_1               ((uint32_t)0x00000100U)        /*!<Bit 1 */
#define  SAI_xCR2_MUTECNT_2               ((uint32_t)0x00000200U)        /*!<Bit 2 */
#define  SAI_xCR2_MUTECNT_3               ((uint32_t)0x00000400U)        /*!<Bit 3 */
#define  SAI_xCR2_MUTECNT_4               ((uint32_t)0x00000800U)        /*!<Bit 4 */
#define  SAI_xCR2_MUTECNT_5               ((uint32_t)0x00001000U)        /*!<Bit 5 */

#define  SAI_xCR2_CPL                     ((uint32_t)0x00002000U)        /*!< Complement Bit             */

#define  SAI_xCR2_COMP                    ((uint32_t)0x0000C000U)        /*!<COMP[1:0] (Companding mode) */
#define  SAI_xCR2_COMP_0                  ((uint32_t)0x00004000U)        /*!<Bit 0 */
#define  SAI_xCR2_COMP_1                  ((uint32_t)0x00008000U)        /*!<Bit 1 */

/******************  Bit definition for SAI_xFRCR register  *******************/
#define  SAI_xFRCR_FRL                    ((uint32_t)0x000000FFU)        /*!<FRL[1:0](Frame length)  */
#define  SAI_xFRCR_FRL_0                  ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  SAI_xFRCR_FRL_1                  ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  SAI_xFRCR_FRL_2                  ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  SAI_xFRCR_FRL_3                  ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  SAI_xFRCR_FRL_4                  ((uint32_t)0x00000010U)        /*!<Bit 4 */
#define  SAI_xFRCR_FRL_5                  ((uint32_t)0x00000020U)        /*!<Bit 5 */
#define  SAI_xFRCR_FRL_6                  ((uint32_t)0x00000040U)        /*!<Bit 6 */
#define  SAI_xFRCR_FRL_7                  ((uint32_t)0x00000080U)        /*!<Bit 7 */

#define  SAI_xFRCR_FSALL                  ((uint32_t)0x00007F00U)        /*!<FRL[1:0] (Frame synchronization active level length)  */
#define  SAI_xFRCR_FSALL_0                ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  SAI_xFRCR_FSALL_1                ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  SAI_xFRCR_FSALL_2                ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  SAI_xFRCR_FSALL_3                ((uint32_t)0x00000800U)        /*!<Bit 3 */
#define  SAI_xFRCR_FSALL_4                ((uint32_t)0x00001000U)        /*!<Bit 4 */
#define  SAI_xFRCR_FSALL_5                ((uint32_t)0x00002000U)        /*!<Bit 5 */
#define  SAI_xFRCR_FSALL_6                ((uint32_t)0x00004000U)        /*!<Bit 6 */

#define  SAI_xFRCR_FSDEF                  ((uint32_t)0x00010000U)        /*!< Frame Synchronization Definition */
#define  SAI_xFRCR_FSPO                   ((uint32_t)0x00020000U)        /*!<Frame Synchronization POLarity    */
#define  SAI_xFRCR_FSOFF                  ((uint32_t)0x00040000U)        /*!<Frame Synchronization OFFset      */

/******************  Bit definition for SAI_xSLOTR register  *******************/
#define  SAI_xSLOTR_FBOFF                 ((uint32_t)0x0000001FU)        /*!<FRL[4:0](First Bit Offset)  */
#define  SAI_xSLOTR_FBOFF_0               ((uint32_t)0x00000001U)        /*!<Bit 0 */
#define  SAI_xSLOTR_FBOFF_1               ((uint32_t)0x00000002U)        /*!<Bit 1 */
#define  SAI_xSLOTR_FBOFF_2               ((uint32_t)0x00000004U)        /*!<Bit 2 */
#define  SAI_xSLOTR_FBOFF_3               ((uint32_t)0x00000008U)        /*!<Bit 3 */
#define  SAI_xSLOTR_FBOFF_4               ((uint32_t)0x00000010U)        /*!<Bit 4 */

#define  SAI_xSLOTR_SLOTSZ                ((uint32_t)0x000000C0U)        /*!<SLOTSZ[1:0] (Slot size)  */
#define  SAI_xSLOTR_SLOTSZ_0              ((uint32_t)0x00000040U)        /*!<Bit 0 */
#define  SAI_xSLOTR_SLOTSZ_1              ((uint32_t)0x00000080U)        /*!<Bit 1 */

#define  SAI_xSLOTR_NBSLOT                ((uint32_t)0x00000F00U)        /*!<NBSLOT[3:0] (Number of Slot in audio Frame)  */
#define  SAI_xSLOTR_NBSLOT_0              ((uint32_t)0x00000100U)        /*!<Bit 0 */
#define  SAI_xSLOTR_NBSLOT_1              ((uint32_t)0x00000200U)        /*!<Bit 1 */
#define  SAI_xSLOTR_NBSLOT_2              ((uint32_t)0x00000400U)        /*!<Bit 2 */
#define  SAI_xSLOTR_NBSLOT_3              ((uint32_t)0x00000800U)        /*!<Bit 3 */

#define  SAI_xSLOTR_SLOTEN                ((uint32_t)0xFFFF0000U)        /*!<SLOTEN[15:0] (Slot Enable)  */

/*******************  Bit definition for SAI_xIMR register  *******************/
#define  SAI_xIMR_OVRUDRIE                ((uint32_t)0x00000001U)        /*!<Overrun underrun interrupt enable                              */
#define  SAI_xIMR_MUTEDETIE               ((uint32_t)0x00000002U)        /*!<Mute detection interrupt enable                                */
#define  SAI_xIMR_WCKCFGIE                ((uint32_t)0x00000004U)        /*!<Wrong Clock Configuration interrupt enable                     */
#define  SAI_xIMR_FREQIE                  ((uint32_t)0x00000008U)        /*!<FIFO request interrupt enable                                  */
#define  SAI_xIMR_CNRDYIE                 ((uint32_t)0x00000010U)        /*!<Codec not ready interrupt enable                               */
#define  SAI_xIMR_AFSDETIE                ((uint32_t)0x00000020U)        /*!<Anticipated frame synchronization detection interrupt enable   */
#define  SAI_xIMR_LFSDETIE                ((uint32_t)0x00000040U)        /*!<Late frame synchronization detection interrupt enable          */

/********************  Bit definition for SAI_xSR register  *******************/
#define  SAI_xSR_OVRUDR                   ((uint32_t)0x00000001U)         /*!<Overrun underrun                               */
#define  SAI_xSR_MUTEDET                  ((uint32_t)0x00000002U)         /*!<Mute detection                                 */
#define  SAI_xSR_WCKCFG                   ((uint32_t)0x00000004U)         /*!<Wrong Clock Configuration                      */
#define  SAI_xSR_FREQ                     ((uint32_t)0x00000008U)         /*!<FIFO request                                   */
#define  SAI_xSR_CNRDY                    ((uint32_t)0x00000010U)         /*!<Codec not ready                                */
#define  SAI_xSR_AFSDET                   ((uint32_t)0x00000020U)         /*!<Anticipated frame synchronization detection    */
#define  SAI_xSR_LFSDET                   ((uint32_t)0x00000040U)         /*!<Late frame synchronization detection           */

#define  SAI_xSR_FLVL                     ((uint32_t)0x00070000U)         /*!<FLVL[2:0] (FIFO Level Threshold)               */
#define  SAI_xSR_FLVL_0                   ((uint32_t)0x00010000U)         /*!<Bit 0 */
#define  SAI_xSR_FLVL_1                   ((uint32_t)0x00020000U)         /*!<Bit 1 */
#define  SAI_xSR_FLVL_2                   ((uint32_t)0x00040000U)         /*!<Bit 2 */

/******************  Bit definition for SAI_xCLRFR register  ******************/
#define  SAI_xCLRFR_COVRUDR               ((uint32_t)0x00000001U)        /*!<Clear Overrun underrun                               */
#define  SAI_xCLRFR_CMUTEDET              ((uint32_t)0x00000002U)        /*!<Clear Mute detection                                 */
#define  SAI_xCLRFR_CWCKCFG               ((uint32_t)0x00000004U)        /*!<Clear Wrong Clock Configuration                      */
#define  SAI_xCLRFR_CFREQ                 ((uint32_t)0x00000008U)        /*!<Clear FIFO request                                   */
#define  SAI_xCLRFR_CCNRDY                ((uint32_t)0x00000010U)        /*!<Clear Codec not ready                                */
#define  SAI_xCLRFR_CAFSDET               ((uint32_t)0x00000020U)        /*!<Clear Anticipated frame synchronization detection    */
#define  SAI_xCLRFR_CLFSDET               ((uint32_t)0x00000040U)        /*!<Clear Late frame synchronization detection           */

/******************  Bit definition for SAI_xDR register  ******************/
#define  SAI_xDR_DATA                     ((uint32_t)0xFFFFFFFFU)


/******************************************************************************/
/*                                                                            */
/*                          SD host Interface                                 */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SDIO_POWER register  ******************/
#define  SDIO_POWER_PWRCTRL                  ((uint32_t)0x03U)               /*!<PWRCTRL[1:0] bits (Power supply control bits) */
#define  SDIO_POWER_PWRCTRL_0                ((uint32_t)0x01U)               /*!<Bit 0 */
#define  SDIO_POWER_PWRCTRL_1                ((uint32_t)0x02U)               /*!<Bit 1 */

/******************  Bit definition for SDIO_CLKCR register  ******************/
#define  SDIO_CLKCR_CLKDIV                   ((uint32_t)0x00FFU)            /*!<Clock divide factor             */
#define  SDIO_CLKCR_CLKEN                    ((uint32_t)0x0100U)            /*!<Clock enable bit                */
#define  SDIO_CLKCR_PWRSAV                   ((uint32_t)0x0200U)            /*!<Power saving configuration bit  */
#define  SDIO_CLKCR_BYPASS                   ((uint32_t)0x0400U)            /*!<Clock divider bypass enable bit */

#define  SDIO_CLKCR_WIDBUS                   ((uint32_t)0x1800U)            /*!<WIDBUS[1:0] bits (Wide bus mode enable bit) */
#define  SDIO_CLKCR_WIDBUS_0                 ((uint32_t)0x0800U)            /*!<Bit 0 */
#define  SDIO_CLKCR_WIDBUS_1                 ((uint32_t)0x1000U)            /*!<Bit 1 */

#define  SDIO_CLKCR_NEGEDGE                  ((uint32_t)0x2000U)            /*!<SDIO_CK dephasing selection bit */
#define  SDIO_CLKCR_HWFC_EN                  ((uint32_t)0x4000U)            /*!<HW Flow Control enable          */

/*******************  Bit definition for SDIO_ARG register  *******************/
#define  SDIO_ARG_CMDARG                     ((uint32_t)0xFFFFFFFFU)            /*!<Command argument */

/*******************  Bit definition for SDIO_CMD register  *******************/
#define  SDIO_CMD_CMDINDEX                   ((uint32_t)0x003FU)            /*!<Command Index                               */

#define  SDIO_CMD_WAITRESP                   ((uint32_t)0x00C0U)            /*!<WAITRESP[1:0] bits (Wait for response bits) */
#define  SDIO_CMD_WAITRESP_0                 ((uint32_t)0x0040U)            /*!< Bit 0 */
#define  SDIO_CMD_WAITRESP_1                 ((uint32_t)0x0080U)            /*!< Bit 1 */

#define  SDIO_CMD_WAITINT                    ((uint32_t)0x0100U)            /*!<CPSM Waits for Interrupt Request                               */
#define  SDIO_CMD_WAITPEND                   ((uint32_t)0x0200U)            /*!<CPSM Waits for ends of data transfer (CmdPend internal signal) */
#define  SDIO_CMD_CPSMEN                     ((uint32_t)0x0400U)            /*!<Command path state machine (CPSM) Enable bit                   */
#define  SDIO_CMD_SDIOSUSPEND                ((uint32_t)0x0800U)            /*!<SD I/O suspend command                                         */
#define  SDIO_CMD_ENCMDCOMPL                 ((uint32_t)0x1000U)            /*!<Enable CMD completion                                          */
#define  SDIO_CMD_NIEN                       ((uint32_t)0x2000U)            /*!<Not Interrupt Enable */
#define  SDIO_CMD_CEATACMD                   ((uint32_t)0x4000U)            /*!<CE-ATA command       */

/*****************  Bit definition for SDIO_RESPCMD register  *****************/
#define  SDIO_RESPCMD_RESPCMD                ((uint32_t)0x3FU)               /*!<Response command index */

/******************  Bit definition for SDIO_RESP0 register  ******************/
#define  SDIO_RESP0_CARDSTATUS0              ((uint32_t)0xFFFFFFFFU)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP1 register  ******************/
#define  SDIO_RESP1_CARDSTATUS1              ((uint32_t)0xFFFFFFFFU)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP2 register  ******************/
#define  SDIO_RESP2_CARDSTATUS2              ((uint32_t)0xFFFFFFFFU)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP3 register  ******************/
#define  SDIO_RESP3_CARDSTATUS3              ((uint32_t)0xFFFFFFFFU)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP4 register  ******************/
#define  SDIO_RESP4_CARDSTATUS4              ((uint32_t)0xFFFFFFFFU)        /*!<Card Status */

/******************  Bit definition for SDIO_DTIMER register  *****************/
#define  SDIO_DTIMER_DATATIME                ((uint32_t)0xFFFFFFFFU)        /*!<Data timeout period. */

/******************  Bit definition for SDIO_DLEN register  *******************/
#define  SDIO_DLEN_DATALENGTH                ((uint32_t)0x01FFFFFFU)        /*!<Data length value    */

/******************  Bit definition for SDIO_DCTRL register  ******************/
#define  SDIO_DCTRL_DTEN                     ((uint32_t)0x0001U)            /*!<Data transfer enabled bit         */
#define  SDIO_DCTRL_DTDIR                    ((uint32_t)0x0002U)            /*!<Data transfer direction selection */
#define  SDIO_DCTRL_DTMODE                   ((uint32_t)0x0004U)            /*!<Data transfer mode selection      */
#define  SDIO_DCTRL_DMAEN                    ((uint32_t)0x0008U)            /*!<DMA enabled bit                   */

#define  SDIO_DCTRL_DBLOCKSIZE               ((uint32_t)0x00F0U)            /*!<DBLOCKSIZE[3:0] bits (Data block size) */
#define  SDIO_DCTRL_DBLOCKSIZE_0             ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  SDIO_DCTRL_DBLOCKSIZE_1             ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  SDIO_DCTRL_DBLOCKSIZE_2             ((uint32_t)0x0040U)            /*!<Bit 2 */
#define  SDIO_DCTRL_DBLOCKSIZE_3             ((uint32_t)0x0080U)            /*!<Bit 3 */

#define  SDIO_DCTRL_RWSTART                  ((uint32_t)0x0100U)            /*!<Read wait start         */
#define  SDIO_DCTRL_RWSTOP                   ((uint32_t)0x0200U)            /*!<Read wait stop          */
#define  SDIO_DCTRL_RWMOD                    ((uint32_t)0x0400U)            /*!<Read wait mode          */
#define  SDIO_DCTRL_SDIOEN                   ((uint32_t)0x0800U)            /*!<SD I/O enable functions */

/******************  Bit definition for SDIO_DCOUNT register  *****************/
#define  SDIO_DCOUNT_DATACOUNT               ((uint32_t)0x01FFFFFFU)        /*!<Data count value */

/******************  Bit definition for SDIO_STA register  ********************/
#define  SDIO_STA_CCRCFAIL                   ((uint32_t)0x00000001U)        /*!<Command response received (CRC check failed)  */
#define  SDIO_STA_DCRCFAIL                   ((uint32_t)0x00000002U)        /*!<Data block sent/received (CRC check failed)   */
#define  SDIO_STA_CTIMEOUT                   ((uint32_t)0x00000004U)        /*!<Command response timeout                      */
#define  SDIO_STA_DTIMEOUT                   ((uint32_t)0x00000008U)        /*!<Data timeout                                  */
#define  SDIO_STA_TXUNDERR                   ((uint32_t)0x00000010U)        /*!<Transmit FIFO underrun error                  */
#define  SDIO_STA_RXOVERR                    ((uint32_t)0x00000020U)        /*!<Received FIFO overrun error                   */
#define  SDIO_STA_CMDREND                    ((uint32_t)0x00000040U)        /*!<Command response received (CRC check passed)  */
#define  SDIO_STA_CMDSENT                    ((uint32_t)0x00000080U)        /*!<Command sent (no response required)           */
#define  SDIO_STA_DATAEND                    ((uint32_t)0x00000100U)        /*!<Data end (data counter, SDIDCOUNT, is zero)   */
#define  SDIO_STA_STBITERR                   ((uint32_t)0x00000200U)        /*!<Start bit not detected on all data signals in wide bus mode */
#define  SDIO_STA_DBCKEND                    ((uint32_t)0x00000400U)        /*!<Data block sent/received (CRC check passed)   */
#define  SDIO_STA_CMDACT                     ((uint32_t)0x00000800U)        /*!<Command transfer in progress                  */
#define  SDIO_STA_TXACT                      ((uint32_t)0x00001000U)        /*!<Data transmit in progress                     */
#define  SDIO_STA_RXACT                      ((uint32_t)0x00002000U)        /*!<Data receive in progress                      */
#define  SDIO_STA_TXFIFOHE                   ((uint32_t)0x00004000U)        /*!<Transmit FIFO Half Empty: at least 8 words can be written into the FIFO */
#define  SDIO_STA_RXFIFOHF                   ((uint32_t)0x00008000U)        /*!<Receive FIFO Half Full: there are at least 8 words in the FIFO */
#define  SDIO_STA_TXFIFOF                    ((uint32_t)0x00010000U)        /*!<Transmit FIFO full                            */
#define  SDIO_STA_RXFIFOF                    ((uint32_t)0x00020000U)        /*!<Receive FIFO full                             */
#define  SDIO_STA_TXFIFOE                    ((uint32_t)0x00040000U)        /*!<Transmit FIFO empty                           */
#define  SDIO_STA_RXFIFOE                    ((uint32_t)0x00080000U)        /*!<Receive FIFO empty                            */
#define  SDIO_STA_TXDAVL                     ((uint32_t)0x00100000U)        /*!<Data available in transmit FIFO               */
#define  SDIO_STA_RXDAVL                     ((uint32_t)0x00200000U)        /*!<Data available in receive FIFO                */
#define  SDIO_STA_SDIOIT                     ((uint32_t)0x00400000U)        /*!<SDIO interrupt received                       */
#define  SDIO_STA_CEATAEND                   ((uint32_t)0x00800000U)        /*!<CE-ATA command completion signal received for CMD61 */

/*******************  Bit definition for SDIO_ICR register  *******************/
#define  SDIO_ICR_CCRCFAILC                  ((uint32_t)0x00000001U)        /*!<CCRCFAIL flag clear bit */
#define  SDIO_ICR_DCRCFAILC                  ((uint32_t)0x00000002U)        /*!<DCRCFAIL flag clear bit */
#define  SDIO_ICR_CTIMEOUTC                  ((uint32_t)0x00000004U)        /*!<CTIMEOUT flag clear bit */
#define  SDIO_ICR_DTIMEOUTC                  ((uint32_t)0x00000008U)        /*!<DTIMEOUT flag clear bit */
#define  SDIO_ICR_TXUNDERRC                  ((uint32_t)0x00000010U)        /*!<TXUNDERR flag clear bit */
#define  SDIO_ICR_RXOVERRC                   ((uint32_t)0x00000020U)        /*!<RXOVERR flag clear bit  */
#define  SDIO_ICR_CMDRENDC                   ((uint32_t)0x00000040U)        /*!<CMDREND flag clear bit  */
#define  SDIO_ICR_CMDSENTC                   ((uint32_t)0x00000080U)        /*!<CMDSENT flag clear bit  */
#define  SDIO_ICR_DATAENDC                   ((uint32_t)0x00000100U)        /*!<DATAEND flag clear bit  */
#define  SDIO_ICR_STBITERRC                  ((uint32_t)0x00000200U)        /*!<STBITERR flag clear bit */
#define  SDIO_ICR_DBCKENDC                   ((uint32_t)0x00000400U)        /*!<DBCKEND flag clear bit  */
#define  SDIO_ICR_SDIOITC                    ((uint32_t)0x00400000U)        /*!<SDIOIT flag clear bit   */
#define  SDIO_ICR_CEATAENDC                  ((uint32_t)0x00800000U)        /*!<CEATAEND flag clear bit */

/******************  Bit definition for SDIO_MASK register  *******************/
#define  SDIO_MASK_CCRCFAILIE                ((uint32_t)0x00000001U)        /*!<Command CRC Fail Interrupt Enable          */
#define  SDIO_MASK_DCRCFAILIE                ((uint32_t)0x00000002U)        /*!<Data CRC Fail Interrupt Enable             */
#define  SDIO_MASK_CTIMEOUTIE                ((uint32_t)0x00000004U)        /*!<Command TimeOut Interrupt Enable           */
#define  SDIO_MASK_DTIMEOUTIE                ((uint32_t)0x00000008U)        /*!<Data TimeOut Interrupt Enable              */
#define  SDIO_MASK_TXUNDERRIE                ((uint32_t)0x00000010U)        /*!<Tx FIFO UnderRun Error Interrupt Enable    */
#define  SDIO_MASK_RXOVERRIE                 ((uint32_t)0x00000020U)        /*!<Rx FIFO OverRun Error Interrupt Enable     */
#define  SDIO_MASK_CMDRENDIE                 ((uint32_t)0x00000040U)        /*!<Command Response Received Interrupt Enable */
#define  SDIO_MASK_CMDSENTIE                 ((uint32_t)0x00000080U)        /*!<Command Sent Interrupt Enable              */
#define  SDIO_MASK_DATAENDIE                 ((uint32_t)0x00000100U)        /*!<Data End Interrupt Enable                  */
#define  SDIO_MASK_STBITERRIE                ((uint32_t)0x00000200U)        /*!<Start Bit Error Interrupt Enable           */
#define  SDIO_MASK_DBCKENDIE                 ((uint32_t)0x00000400U)        /*!<Data Block End Interrupt Enable            */
#define  SDIO_MASK_CMDACTIE                  ((uint32_t)0x00000800U)        /*!<CCommand Acting Interrupt Enable           */
#define  SDIO_MASK_TXACTIE                   ((uint32_t)0x00001000U)        /*!<Data Transmit Acting Interrupt Enable      */
#define  SDIO_MASK_RXACTIE                   ((uint32_t)0x00002000U)        /*!<Data receive acting interrupt enabled      */
#define  SDIO_MASK_TXFIFOHEIE                ((uint32_t)0x00004000U)        /*!<Tx FIFO Half Empty interrupt Enable        */
#define  SDIO_MASK_RXFIFOHFIE                ((uint32_t)0x00008000U)        /*!<Rx FIFO Half Full interrupt Enable         */
#define  SDIO_MASK_TXFIFOFIE                 ((uint32_t)0x00010000U)        /*!<Tx FIFO Full interrupt Enable              */
#define  SDIO_MASK_RXFIFOFIE                 ((uint32_t)0x00020000U)        /*!<Rx FIFO Full interrupt Enable              */
#define  SDIO_MASK_TXFIFOEIE                 ((uint32_t)0x00040000U)        /*!<Tx FIFO Empty interrupt Enable             */
#define  SDIO_MASK_RXFIFOEIE                 ((uint32_t)0x00080000U)        /*!<Rx FIFO Empty interrupt Enable             */
#define  SDIO_MASK_TXDAVLIE                  ((uint32_t)0x00100000U)        /*!<Data available in Tx FIFO interrupt Enable */
#define  SDIO_MASK_RXDAVLIE                  ((uint32_t)0x00200000U)        /*!<Data available in Rx FIFO interrupt Enable */
#define  SDIO_MASK_SDIOITIE                  ((uint32_t)0x00400000U)        /*!<SDIO Mode Interrupt Received interrupt Enable */
#define  SDIO_MASK_CEATAENDIE                ((uint32_t)0x00800000U)        /*!<CE-ATA command completion signal received Interrupt Enable */

/*****************  Bit definition for SDIO_FIFOCNT register  *****************/
#define  SDIO_FIFOCNT_FIFOCOUNT              ((uint32_t)0x00FFFFFFU)        /*!<Remaining number of words to be written to or read from the FIFO */

/******************  Bit definition for SDIO_FIFO register  *******************/
#define  SDIO_FIFO_FIFODATA                  ((uint32_t)0xFFFFFFFFU)        /*!<Receive and transmit FIFO data */

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface                         */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for SPI_CR1 register  ********************/
#define  SPI_CR1_CPHA                        ((uint32_t)0x00000001U)            /*!<Clock Phase      */
#define  SPI_CR1_CPOL                        ((uint32_t)0x00000002U)            /*!<Clock Polarity   */
#define  SPI_CR1_MSTR                        ((uint32_t)0x00000004U)            /*!<Master Selection */

#define  SPI_CR1_BR                          ((uint32_t)0x00000038U)            /*!<BR[2:0] bits (Baud Rate Control) */
#define  SPI_CR1_BR_0                        ((uint32_t)0x00000008U)            /*!<Bit 0 */
#define  SPI_CR1_BR_1                        ((uint32_t)0x00000010U)            /*!<Bit 1 */
#define  SPI_CR1_BR_2                        ((uint32_t)0x00000020U)            /*!<Bit 2 */

#define  SPI_CR1_SPE                         ((uint32_t)0x00000040U)            /*!<SPI Enable                          */
#define  SPI_CR1_LSBFIRST                    ((uint32_t)0x00000080U)            /*!<Frame Format                        */
#define  SPI_CR1_SSI                         ((uint32_t)0x00000100U)            /*!<Internal slave select               */
#define  SPI_CR1_SSM                         ((uint32_t)0x00000200U)            /*!<Software slave management           */
#define  SPI_CR1_RXONLY                      ((uint32_t)0x00000400U)            /*!<Receive only                        */
#define  SPI_CR1_DFF                         ((uint32_t)0x00000800U)            /*!<Data Frame Format                   */
#define  SPI_CR1_CRCNEXT                     ((uint32_t)0x00001000U)            /*!<Transmit CRC next                   */
#define  SPI_CR1_CRCEN                       ((uint32_t)0x00002000U)            /*!<Hardware CRC calculation enable     */
#define  SPI_CR1_BIDIOE                      ((uint32_t)0x00004000U)            /*!<Output enable in bidirectional mode */
#define  SPI_CR1_BIDIMODE                    ((uint32_t)0x00008000U)            /*!<Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define  SPI_CR2_RXDMAEN                     ((uint32_t)0x00000001U)               /*!<Rx Buffer DMA Enable                 */
#define  SPI_CR2_TXDMAEN                     ((uint32_t)0x00000002U)               /*!<Tx Buffer DMA Enable                 */
#define  SPI_CR2_SSOE                        ((uint32_t)0x00000004U)               /*!<SS Output Enable                     */
#define  SPI_CR2_FRF                         ((uint32_t)0x00000010U)               /*!<Frame Format                         */
#define  SPI_CR2_ERRIE                       ((uint32_t)0x00000020U)               /*!<Error Interrupt Enable               */
#define  SPI_CR2_RXNEIE                      ((uint32_t)0x00000040U)               /*!<RX buffer Not Empty Interrupt Enable */
#define  SPI_CR2_TXEIE                       ((uint32_t)0x00000080U)               /*!<Tx buffer Empty Interrupt Enable     */

/********************  Bit definition for SPI_SR register  ********************/
#define  SPI_SR_RXNE                         ((uint32_t)0x00000001U)               /*!<Receive buffer Not Empty */
#define  SPI_SR_TXE                          ((uint32_t)0x00000002U)               /*!<Transmit buffer Empty    */
#define  SPI_SR_CHSIDE                       ((uint32_t)0x00000004U)               /*!<Channel side             */
#define  SPI_SR_UDR                          ((uint32_t)0x00000008U)               /*!<Underrun flag            */
#define  SPI_SR_CRCERR                       ((uint32_t)0x00000010U)               /*!<CRC Error flag           */
#define  SPI_SR_MODF                         ((uint32_t)0x00000020U)               /*!<Mode fault               */
#define  SPI_SR_OVR                          ((uint32_t)0x00000040U)               /*!<Overrun flag             */
#define  SPI_SR_BSY                          ((uint32_t)0x00000080U)               /*!<Busy flag                */
#define  SPI_SR_FRE                          ((uint32_t)0x00000100U)               /*!<Frame format error flag  */

/********************  Bit definition for SPI_DR register  ********************/
#define  SPI_DR_DR                           ((uint32_t)0x0000FFFFU)            /*!<Data Register           */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define  SPI_CRCPR_CRCPOLY                   ((uint32_t)0x0000FFFFU)            /*!<CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define  SPI_RXCRCR_RXCRC                    ((uint32_t)0x0000FFFFU)            /*!<Rx CRC Register         */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define  SPI_TXCRCR_TXCRC                    ((uint32_t)0x0000FFFFU)            /*!<Tx CRC Register         */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define  SPI_I2SCFGR_CHLEN                   ((uint32_t)0x00000001U)            /*!<Channel length (number of bits per audio channel) */

#define  SPI_I2SCFGR_DATLEN                  ((uint32_t)0x00000006U)            /*!<DATLEN[1:0] bits (Data length to be transferred)  */
#define  SPI_I2SCFGR_DATLEN_0                ((uint32_t)0x00000002U)            /*!<Bit 0 */
#define  SPI_I2SCFGR_DATLEN_1                ((uint32_t)0x00000004U)            /*!<Bit 1 */

#define  SPI_I2SCFGR_CKPOL                   ((uint32_t)0x00000008U)            /*!<steady state clock polarity               */

#define  SPI_I2SCFGR_I2SSTD                  ((uint32_t)0x00000030U)            /*!<I2SSTD[1:0] bits (I2S standard selection) */
#define  SPI_I2SCFGR_I2SSTD_0                ((uint32_t)0x00000010U)            /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SSTD_1                ((uint32_t)0x00000020U)            /*!<Bit 1 */

#define  SPI_I2SCFGR_PCMSYNC                 ((uint32_t)0x00000080U)            /*!<PCM frame synchronization                 */

#define  SPI_I2SCFGR_I2SCFG                  ((uint32_t)0x00000300U)            /*!<I2SCFG[1:0] bits (I2S configuration mode) */
#define  SPI_I2SCFGR_I2SCFG_0                ((uint32_t)0x00000100U)            /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SCFG_1                ((uint32_t)0x00000200U)            /*!<Bit 1 */

#define  SPI_I2SCFGR_I2SE                    ((uint32_t)0x00000400U)            /*!<I2S Enable         */
#define  SPI_I2SCFGR_I2SMOD                  ((uint32_t)0x00000800U)            /*!<I2S mode selection */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define  SPI_I2SPR_I2SDIV                    ((uint32_t)0x000000FFU)            /*!<I2S Linear prescaler         */
#define  SPI_I2SPR_ODD                       ((uint32_t)0x00000100U)            /*!<Odd factor for the prescaler */
#define  SPI_I2SPR_MCKOE                     ((uint32_t)0x00000200U)            /*!<Master Clock Output Enable   */

/******************************************************************************/
/*                                                                            */
/*                                 SYSCFG                                     */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SYSCFG_MEMRMP register  ***************/
#define SYSCFG_MEMRMP_MEM_MODE          ((uint32_t)0x00000007U) /*!< SYSCFG_Memory Remap Config */
#define SYSCFG_MEMRMP_MEM_MODE_0        ((uint32_t)0x00000001U)
#define SYSCFG_MEMRMP_MEM_MODE_1        ((uint32_t)0x00000002U)
#define SYSCFG_MEMRMP_MEM_MODE_2        ((uint32_t)0x00000004U)

#define SYSCFG_MEMRMP_UFB_MODE          ((uint32_t)0x00000100U) /*!< User Flash Bank mode    */
#define SYSCFG_SWP_FMC                  ((uint32_t)0x00000C00U) /*!< FMC memory mapping swap */

/******************  Bit definition for SYSCFG_PMC register  ******************/
#define SYSCFG_PMC_ADCxDC2              ((uint32_t)0x00070000U) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC1DC2              ((uint32_t)0x00010000U) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC2DC2              ((uint32_t)0x00020000U) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC3DC2              ((uint32_t)0x00040000U) /*!< Refer to AN4073 on how to use this bit  */

#define SYSCFG_PMC_MII_RMII_SEL         ((uint32_t)0x00800000U) /*!<Ethernet PHY interface selection */
/* Old MII_RMII_SEL bit definition, maintained for legacy purpose */
#define SYSCFG_PMC_MII_RMII             SYSCFG_PMC_MII_RMII_SEL

/*****************  Bit definition for SYSCFG_EXTICR1 register  ***************/
#define SYSCFG_EXTICR1_EXTI0            ((uint32_t)0x000FU) /*!<EXTI 0 configuration */
#define SYSCFG_EXTICR1_EXTI1            ((uint32_t)0x00F0U) /*!<EXTI 1 configuration */
#define SYSCFG_EXTICR1_EXTI2            ((uint32_t)0x0F00U) /*!<EXTI 2 configuration */
#define SYSCFG_EXTICR1_EXTI3            ((uint32_t)0xF000U) /*!<EXTI 3 configuration */
/**
  * @brief   EXTI0 configuration
  */
#define SYSCFG_EXTICR1_EXTI0_PA         ((uint32_t)0x0000U) /*!<PA[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PB         ((uint32_t)0x0001U) /*!<PB[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PC         ((uint32_t)0x0002U) /*!<PC[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PD         ((uint32_t)0x0003U) /*!<PD[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PE         ((uint32_t)0x0004U) /*!<PE[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PF         ((uint32_t)0x0005U) /*!<PF[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PG         ((uint32_t)0x0006U) /*!<PG[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PH         ((uint32_t)0x0007U) /*!<PH[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PI         ((uint32_t)0x0008U) /*!<PI[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PJ         ((uint32_t)0x0009U) /*!<PJ[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PK         ((uint32_t)0x000AU) /*!<PK[0] pin */

/**
  * @brief   EXTI1 configuration
  */
#define SYSCFG_EXTICR1_EXTI1_PA         ((uint32_t)0x0000U) /*!<PA[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PB         ((uint32_t)0x0010U) /*!<PB[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PC         ((uint32_t)0x0020U) /*!<PC[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PD         ((uint32_t)0x0030U) /*!<PD[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PE         ((uint32_t)0x0040U) /*!<PE[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PF         ((uint32_t)0x0050U) /*!<PF[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PG         ((uint32_t)0x0060U) /*!<PG[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PH         ((uint32_t)0x0070U) /*!<PH[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PI         ((uint32_t)0x0080U) /*!<PI[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PJ         ((uint32_t)0x0090U) /*!<PJ[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PK         ((uint32_t)0x00A0U) /*!<PK[1] pin */


/**
  * @brief   EXTI2 configuration
  */
#define SYSCFG_EXTICR1_EXTI2_PA         ((uint32_t)0x0000U) /*!<PA[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PB         ((uint32_t)0x0100U) /*!<PB[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PC         ((uint32_t)0x0200U) /*!<PC[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PD         ((uint32_t)0x0300U) /*!<PD[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PE         ((uint32_t)0x0400U) /*!<PE[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PF         ((uint32_t)0x0500U) /*!<PF[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PG         ((uint32_t)0x0600U) /*!<PG[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PH         ((uint32_t)0x0700U) /*!<PH[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PI         ((uint32_t)0x0800U) /*!<PI[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PJ         ((uint32_t)0x0900U) /*!<PJ[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PK         ((uint32_t)0x0A00U) /*!<PK[2] pin */


/**
  * @brief   EXTI3 configuration
  */
#define SYSCFG_EXTICR1_EXTI3_PA         ((uint32_t)0x0000U) /*!<PA[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PB         ((uint32_t)0x1000U) /*!<PB[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PC         ((uint32_t)0x2000U) /*!<PC[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PD         ((uint32_t)0x3000U) /*!<PD[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PE         ((uint32_t)0x4000U) /*!<PE[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PF         ((uint32_t)0x5000U) /*!<PF[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PG         ((uint32_t)0x6000U) /*!<PG[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PH         ((uint32_t)0x7000U) /*!<PH[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PI         ((uint32_t)0x8000U) /*!<PI[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PJ         ((uint32_t)0x9000U) /*!<PJ[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PK         ((uint32_t)0xA000U) /*!<PK[3] pin */


/*****************  Bit definition for SYSCFG_EXTICR2 register  ***************/
#define SYSCFG_EXTICR2_EXTI4            ((uint32_t)0x000FU) /*!<EXTI 4 configuration */
#define SYSCFG_EXTICR2_EXTI5            ((uint32_t)0x00F0U) /*!<EXTI 5 configuration */
#define SYSCFG_EXTICR2_EXTI6            ((uint32_t)0x0F00U) /*!<EXTI 6 configuration */
#define SYSCFG_EXTICR2_EXTI7            ((uint32_t)0xF000U) /*!<EXTI 7 configuration */
/**
  * @brief   EXTI4 configuration
  */
#define SYSCFG_EXTICR2_EXTI4_PA         ((uint32_t)0x0000U) /*!<PA[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PB         ((uint32_t)0x0001U) /*!<PB[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PC         ((uint32_t)0x0002U) /*!<PC[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PD         ((uint32_t)0x0003U) /*!<PD[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PE         ((uint32_t)0x0004U) /*!<PE[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PF         ((uint32_t)0x0005U) /*!<PF[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PG         ((uint32_t)0x0006U) /*!<PG[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PH         ((uint32_t)0x0007U) /*!<PH[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PI         ((uint32_t)0x0008U) /*!<PI[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PJ         ((uint32_t)0x0009U) /*!<PJ[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PK         ((uint32_t)0x000AU) /*!<PK[4] pin */

/**
  * @brief   EXTI5 configuration
  */
#define SYSCFG_EXTICR2_EXTI5_PA         ((uint32_t)0x0000U) /*!<PA[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PB         ((uint32_t)0x0010U) /*!<PB[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PC         ((uint32_t)0x0020U) /*!<PC[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PD         ((uint32_t)0x0030U) /*!<PD[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PE         ((uint32_t)0x0040U) /*!<PE[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PF         ((uint32_t)0x0050U) /*!<PF[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PG         ((uint32_t)0x0060U) /*!<PG[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PH         ((uint32_t)0x0070U) /*!<PH[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PI         ((uint32_t)0x0080U) /*!<PI[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PJ         ((uint32_t)0x0090U) /*!<PJ[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PK         ((uint32_t)0x00A0U) /*!<PK[5] pin */

/**
  * @brief   EXTI6 configuration
  */
#define SYSCFG_EXTICR2_EXTI6_PA         ((uint32_t)0x0000U) /*!<PA[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PB         ((uint32_t)0x0100U) /*!<PB[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PC         ((uint32_t)0x0200U) /*!<PC[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PD         ((uint32_t)0x0300U) /*!<PD[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PE         ((uint32_t)0x0400U) /*!<PE[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PF         ((uint32_t)0x0500U) /*!<PF[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PG         ((uint32_t)0x0600U) /*!<PG[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PH         ((uint32_t)0x0700U) /*!<PH[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PI         ((uint32_t)0x0800U) /*!<PI[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PJ         ((uint32_t)0x0900U) /*!<PJ[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PK         ((uint32_t)0x0A00U) /*!<PK[6] pin */


/**
  * @brief   EXTI7 configuration
  */
#define SYSCFG_EXTICR2_EXTI7_PA         ((uint32_t)0x0000U) /*!<PA[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PB         ((uint32_t)0x1000U) /*!<PB[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PC         ((uint32_t)0x2000U) /*!<PC[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PD         ((uint32_t)0x3000U) /*!<PD[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PE         ((uint32_t)0x4000U) /*!<PE[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PF         ((uint32_t)0x5000U) /*!<PF[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PG         ((uint32_t)0x6000U) /*!<PG[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PH         ((uint32_t)0x7000U) /*!<PH[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PI         ((uint32_t)0x8000U) /*!<PI[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PJ         ((uint32_t)0x9000U) /*!<PJ[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PK         ((uint32_t)0xA000U) /*!<PK[7] pin */

/*****************  Bit definition for SYSCFG_EXTICR3 register  ***************/
#define SYSCFG_EXTICR3_EXTI8            ((uint32_t)0x000FU) /*!<EXTI 8 configuration */
#define SYSCFG_EXTICR3_EXTI9            ((uint32_t)0x00F0U) /*!<EXTI 9 configuration */
#define SYSCFG_EXTICR3_EXTI10           ((uint32_t)0x0F00U) /*!<EXTI 10 configuration */
#define SYSCFG_EXTICR3_EXTI11           ((uint32_t)0xF000U) /*!<EXTI 11 configuration */

/**
  * @brief   EXTI8 configuration
  */
#define SYSCFG_EXTICR3_EXTI8_PA         ((uint32_t)0x0000U) /*!<PA[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PB         ((uint32_t)0x0001U) /*!<PB[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PC         ((uint32_t)0x0002U) /*!<PC[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PD         ((uint32_t)0x0003U) /*!<PD[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PE         ((uint32_t)0x0004U) /*!<PE[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PF         ((uint32_t)0x0005U) /*!<PF[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PG         ((uint32_t)0x0006U) /*!<PG[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PH         ((uint32_t)0x0007U) /*!<PH[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PI         ((uint32_t)0x0008U) /*!<PI[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PJ         ((uint32_t)0x0009U) /*!<PJ[8] pin */

/**
  * @brief   EXTI9 configuration
  */
#define SYSCFG_EXTICR3_EXTI9_PA         ((uint32_t)0x0000U) /*!<PA[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PB         ((uint32_t)0x0010U) /*!<PB[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PC         ((uint32_t)0x0020U) /*!<PC[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PD         ((uint32_t)0x0030U) /*!<PD[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PE         ((uint32_t)0x0040U) /*!<PE[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PF         ((uint32_t)0x0050U) /*!<PF[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PG         ((uint32_t)0x0060U) /*!<PG[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PH         ((uint32_t)0x0070U) /*!<PH[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PI         ((uint32_t)0x0080U) /*!<PI[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PJ         ((uint32_t)0x0090U) /*!<PJ[9] pin */


/**
  * @brief   EXTI10 configuration
  */
#define SYSCFG_EXTICR3_EXTI10_PA        ((uint32_t)0x0000U) /*!<PA[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PB        ((uint32_t)0x0100U) /*!<PB[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PC        ((uint32_t)0x0200U) /*!<PC[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PD        ((uint32_t)0x0300U) /*!<PD[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PE        ((uint32_t)0x0400U) /*!<PE[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PF        ((uint32_t)0x0500U) /*!<PF[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PG        ((uint32_t)0x0600U) /*!<PG[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PH        ((uint32_t)0x0700U) /*!<PH[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PI        ((uint32_t)0x0800U) /*!<PI[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PJ        ((uint32_t)0x0900U) /*!<PJ[10] pin */


/**
  * @brief   EXTI11 configuration
  */
#define SYSCFG_EXTICR3_EXTI11_PA        ((uint32_t)0x0000U) /*!<PA[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PB        ((uint32_t)0x1000U) /*!<PB[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PC        ((uint32_t)0x2000U) /*!<PC[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PD        ((uint32_t)0x3000U) /*!<PD[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PE        ((uint32_t)0x4000U) /*!<PE[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PF        ((uint32_t)0x5000U) /*!<PF[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PG        ((uint32_t)0x6000U) /*!<PG[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PH        ((uint32_t)0x7000U) /*!<PH[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PI        ((uint32_t)0x8000U) /*!<PI[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PJ        ((uint32_t)0x9000U) /*!<PJ[11] pin */


/*****************  Bit definition for SYSCFG_EXTICR4 register  ***************/
#define SYSCFG_EXTICR4_EXTI12           ((uint32_t)0x000FU) /*!<EXTI 12 configuration */
#define SYSCFG_EXTICR4_EXTI13           ((uint32_t)0x00F0U) /*!<EXTI 13 configuration */
#define SYSCFG_EXTICR4_EXTI14           ((uint32_t)0x0F00U) /*!<EXTI 14 configuration */
#define SYSCFG_EXTICR4_EXTI15           ((uint32_t)0xF000U) /*!<EXTI 15 configuration */
/**
  * @brief   EXTI12 configuration
  */
#define SYSCFG_EXTICR4_EXTI12_PA        ((uint32_t)0x0000U) /*!<PA[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PB        ((uint32_t)0x0001U) /*!<PB[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PC        ((uint32_t)0x0002U) /*!<PC[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PD        ((uint32_t)0x0003U) /*!<PD[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PE        ((uint32_t)0x0004U) /*!<PE[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PF        ((uint32_t)0x0005U) /*!<PF[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PG        ((uint32_t)0x0006U) /*!<PG[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PH        ((uint32_t)0x0007U) /*!<PH[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PI        ((uint32_t)0x0008U) /*!<PI[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PJ        ((uint32_t)0x0009U) /*!<PJ[12] pin */


/**
  * @brief   EXTI13 configuration
  */
#define SYSCFG_EXTICR4_EXTI13_PA        ((uint32_t)0x0000U) /*!<PA[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PB        ((uint32_t)0x0010U) /*!<PB[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PC        ((uint32_t)0x0020U) /*!<PC[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PD        ((uint32_t)0x0030U) /*!<PD[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PE        ((uint32_t)0x0040U) /*!<PE[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PF        ((uint32_t)0x0050U) /*!<PF[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PG        ((uint32_t)0x0060U) /*!<PG[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PH        ((uint32_t)0x0070U) /*!<PH[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PI        ((uint32_t)0x0008U) /*!<PI[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PJ        ((uint32_t)0x0009U) /*!<PJ[13] pin */


/**
  * @brief   EXTI14 configuration
  */
#define SYSCFG_EXTICR4_EXTI14_PA        ((uint32_t)0x0000U) /*!<PA[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PB        ((uint32_t)0x0100U) /*!<PB[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PC        ((uint32_t)0x0200U) /*!<PC[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PD        ((uint32_t)0x0300U) /*!<PD[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PE        ((uint32_t)0x0400U) /*!<PE[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PF        ((uint32_t)0x0500U) /*!<PF[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PG        ((uint32_t)0x0600U) /*!<PG[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PH        ((uint32_t)0x0700U) /*!<PH[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PI        ((uint32_t)0x0800U) /*!<PI[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PJ        ((uint32_t)0x0900U) /*!<PJ[14] pin */


/**
  * @brief   EXTI15 configuration
  */
#define SYSCFG_EXTICR4_EXTI15_PA        ((uint32_t)0x0000U) /*!<PA[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PB        ((uint32_t)0x1000U) /*!<PB[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PC        ((uint32_t)0x2000U) /*!<PC[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PD        ((uint32_t)0x3000U) /*!<PD[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PE        ((uint32_t)0x4000U) /*!<PE[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PF        ((uint32_t)0x5000U) /*!<PF[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PG        ((uint32_t)0x6000U) /*!<PG[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PH        ((uint32_t)0x7000U) /*!<PH[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PI        ((uint32_t)0x8000U) /*!<PI[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PJ        ((uint32_t)0x9000U) /*!<PJ[15] pin */

/******************  Bit definition for SYSCFG_CMPCR register  ****************/
#define SYSCFG_CMPCR_CMP_PD             ((uint32_t)0x00000001U) /*!<Compensation cell ready flag */
#define SYSCFG_CMPCR_READY              ((uint32_t)0x00000100U) /*!<Compensation cell power-down */

/******************************************************************************/
/*                                                                            */
/*                                    TIM                                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for TIM_CR1 register  ********************/
#define  TIM_CR1_CEN                         ((uint32_t)0x0001U)            /*!<Counter enable        */
#define  TIM_CR1_UDIS                        ((uint32_t)0x0002U)            /*!<Update disable        */
#define  TIM_CR1_URS                         ((uint32_t)0x0004U)            /*!<Update request source */
#define  TIM_CR1_OPM                         ((uint32_t)0x0008U)            /*!<One pulse mode        */
#define  TIM_CR1_DIR                         ((uint32_t)0x0010U)            /*!<Direction             */

#define  TIM_CR1_CMS                         ((uint32_t)0x0060U)            /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CR1_CMS_0                       ((uint32_t)0x0020U)            /*!<Bit 0 */
#define  TIM_CR1_CMS_1                       ((uint32_t)0x0040U)            /*!<Bit 1 */

#define  TIM_CR1_ARPE                        ((uint32_t)0x0080U)            /*!<Auto-reload preload enable     */

#define  TIM_CR1_CKD                         ((uint32_t)0x0300U)            /*!<CKD[1:0] bits (clock division) */
#define  TIM_CR1_CKD_0                       ((uint32_t)0x0100U)            /*!<Bit 0 */
#define  TIM_CR1_CKD_1                       ((uint32_t)0x0200U)            /*!<Bit 1 */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define  TIM_CR2_CCPC                        ((uint32_t)0x0001U)            /*!<Capture/Compare Preloaded Control        */
#define  TIM_CR2_CCUS                        ((uint32_t)0x0004U)            /*!<Capture/Compare Control Update Selection */
#define  TIM_CR2_CCDS                        ((uint32_t)0x0008U)            /*!<Capture/Compare DMA Selection            */

#define  TIM_CR2_MMS                         ((uint32_t)0x0070U)            /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS_0                       ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  TIM_CR2_MMS_1                       ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  TIM_CR2_MMS_2                       ((uint32_t)0x0040U)            /*!<Bit 2 */

#define  TIM_CR2_TI1S                        ((uint32_t)0x0080U)            /*!<TI1 Selection */
#define  TIM_CR2_OIS1                        ((uint32_t)0x0100U)            /*!<Output Idle state 1 (OC1 output)  */
#define  TIM_CR2_OIS1N                       ((uint32_t)0x0200U)            /*!<Output Idle state 1 (OC1N output) */
#define  TIM_CR2_OIS2                        ((uint32_t)0x0400U)            /*!<Output Idle state 2 (OC2 output)  */
#define  TIM_CR2_OIS2N                       ((uint32_t)0x0800U)            /*!<Output Idle state 2 (OC2N output) */
#define  TIM_CR2_OIS3                        ((uint32_t)0x1000U)            /*!<Output Idle state 3 (OC3 output)  */
#define  TIM_CR2_OIS3N                       ((uint32_t)0x2000U)            /*!<Output Idle state 3 (OC3N output) */
#define  TIM_CR2_OIS4                        ((uint32_t)0x4000U)            /*!<Output Idle state 4 (OC4 output)  */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define  TIM_SMCR_SMS                        ((uint32_t)0x0007U)            /*!<SMS[2:0] bits (Slave mode selection)    */
#define  TIM_SMCR_SMS_0                      ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  TIM_SMCR_SMS_1                      ((uint32_t)0x0002U)            /*!<Bit 1 */
#define  TIM_SMCR_SMS_2                      ((uint32_t)0x0004U)            /*!<Bit 2 */

#define  TIM_SMCR_TS                         ((uint32_t)0x0070U)            /*!<TS[2:0] bits (Trigger selection)        */
#define  TIM_SMCR_TS_0                       ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  TIM_SMCR_TS_1                       ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  TIM_SMCR_TS_2                       ((uint32_t)0x0040U)            /*!<Bit 2 */

#define  TIM_SMCR_MSM                        ((uint32_t)0x0080U)            /*!<Master/slave mode                       */

#define  TIM_SMCR_ETF                        ((uint32_t)0x0F00U)            /*!<ETF[3:0] bits (External trigger filter) */
#define  TIM_SMCR_ETF_0                      ((uint32_t)0x0100U)            /*!<Bit 0 */
#define  TIM_SMCR_ETF_1                      ((uint32_t)0x0200U)            /*!<Bit 1 */
#define  TIM_SMCR_ETF_2                      ((uint32_t)0x0400U)            /*!<Bit 2 */
#define  TIM_SMCR_ETF_3                      ((uint32_t)0x0800U)            /*!<Bit 3 */

#define  TIM_SMCR_ETPS                       ((uint32_t)0x3000U)            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_SMCR_ETPS_0                     ((uint32_t)0x1000U)            /*!<Bit 0 */
#define  TIM_SMCR_ETPS_1                     ((uint32_t)0x2000U)            /*!<Bit 1 */

#define  TIM_SMCR_ECE                        ((uint32_t)0x4000U)            /*!<External clock enable     */
#define  TIM_SMCR_ETP                        ((uint32_t)0x8000U)            /*!<External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define  TIM_DIER_UIE                        ((uint32_t)0x0001U)            /*!<Update interrupt enable */
#define  TIM_DIER_CC1IE                      ((uint32_t)0x0002U)            /*!<Capture/Compare 1 interrupt enable   */
#define  TIM_DIER_CC2IE                      ((uint32_t)0x0004U)            /*!<Capture/Compare 2 interrupt enable   */
#define  TIM_DIER_CC3IE                      ((uint32_t)0x0008U)            /*!<Capture/Compare 3 interrupt enable   */
#define  TIM_DIER_CC4IE                      ((uint32_t)0x0010U)            /*!<Capture/Compare 4 interrupt enable   */
#define  TIM_DIER_COMIE                      ((uint32_t)0x0020U)            /*!<COM interrupt enable                 */
#define  TIM_DIER_TIE                        ((uint32_t)0x0040U)            /*!<Trigger interrupt enable             */
#define  TIM_DIER_BIE                        ((uint32_t)0x0080U)            /*!<Break interrupt enable               */
#define  TIM_DIER_UDE                        ((uint32_t)0x0100U)            /*!<Update DMA request enable            */
#define  TIM_DIER_CC1DE                      ((uint32_t)0x0200U)            /*!<Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE                      ((uint32_t)0x0400U)            /*!<Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE                      ((uint32_t)0x0800U)            /*!<Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE                      ((uint32_t)0x1000U)            /*!<Capture/Compare 4 DMA request enable */
#define  TIM_DIER_COMDE                      ((uint32_t)0x2000U)            /*!<COM DMA request enable               */
#define  TIM_DIER_TDE                        ((uint32_t)0x4000U)            /*!<Trigger DMA request enable           */

/********************  Bit definition for TIM_SR register  ********************/
#define  TIM_SR_UIF                          ((uint32_t)0x0001U)            /*!<Update interrupt Flag              */
#define  TIM_SR_CC1IF                        ((uint32_t)0x0002U)            /*!<Capture/Compare 1 interrupt Flag   */
#define  TIM_SR_CC2IF                        ((uint32_t)0x0004U)            /*!<Capture/Compare 2 interrupt Flag   */
#define  TIM_SR_CC3IF                        ((uint32_t)0x0008U)            /*!<Capture/Compare 3 interrupt Flag   */
#define  TIM_SR_CC4IF                        ((uint32_t)0x0010U)            /*!<Capture/Compare 4 interrupt Flag   */
#define  TIM_SR_COMIF                        ((uint32_t)0x0020U)            /*!<COM interrupt Flag                 */
#define  TIM_SR_TIF                          ((uint32_t)0x0040U)            /*!<Trigger interrupt Flag             */
#define  TIM_SR_BIF                          ((uint32_t)0x0080U)            /*!<Break interrupt Flag               */
#define  TIM_SR_CC1OF                        ((uint32_t)0x0200U)            /*!<Capture/Compare 1 Overcapture Flag */
#define  TIM_SR_CC2OF                        ((uint32_t)0x0400U)            /*!<Capture/Compare 2 Overcapture Flag */
#define  TIM_SR_CC3OF                        ((uint32_t)0x0800U)            /*!<Capture/Compare 3 Overcapture Flag */
#define  TIM_SR_CC4OF                        ((uint32_t)0x1000U)            /*!<Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_EGR register  ********************/
#define  TIM_EGR_UG                          ((uint32_t)0x01U)               /*!<Update Generation                         */
#define  TIM_EGR_CC1G                        ((uint32_t)0x02U)               /*!<Capture/Compare 1 Generation              */
#define  TIM_EGR_CC2G                        ((uint32_t)0x04U)               /*!<Capture/Compare 2 Generation              */
#define  TIM_EGR_CC3G                        ((uint32_t)0x08U)               /*!<Capture/Compare 3 Generation              */
#define  TIM_EGR_CC4G                        ((uint32_t)0x10U)               /*!<Capture/Compare 4 Generation              */
#define  TIM_EGR_COMG                        ((uint32_t)0x20U)               /*!<Capture/Compare Control Update Generation */
#define  TIM_EGR_TG                          ((uint32_t)0x40U)               /*!<Trigger Generation                        */
#define  TIM_EGR_BG                          ((uint32_t)0x80U)               /*!<Break Generation                          */

/******************  Bit definition for TIM_CCMR1 register  *******************/
#define  TIM_CCMR1_CC1S                      ((uint32_t)0x0003U)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CCMR1_CC1S_0                    ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  TIM_CCMR1_CC1S_1                    ((uint32_t)0x0002U)            /*!<Bit 1 */

#define  TIM_CCMR1_OC1FE                     ((uint32_t)0x0004U)            /*!<Output Compare 1 Fast enable                 */
#define  TIM_CCMR1_OC1PE                     ((uint32_t)0x0008U)            /*!<Output Compare 1 Preload enable              */

#define  TIM_CCMR1_OC1M                      ((uint32_t)0x0070U)            /*!<OC1M[2:0] bits (Output Compare 1 Mode)       */
#define  TIM_CCMR1_OC1M_0                    ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  TIM_CCMR1_OC1M_1                    ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  TIM_CCMR1_OC1M_2                    ((uint32_t)0x0040U)            /*!<Bit 2 */

#define  TIM_CCMR1_OC1CE                     ((uint32_t)0x0080U)            /*!<Output Compare 1Clear Enable                 */

#define  TIM_CCMR1_CC2S                      ((uint32_t)0x0300U)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CCMR1_CC2S_0                    ((uint32_t)0x0100U)            /*!<Bit 0 */
#define  TIM_CCMR1_CC2S_1                    ((uint32_t)0x0200U)            /*!<Bit 1 */

#define  TIM_CCMR1_OC2FE                     ((uint32_t)0x0400U)            /*!<Output Compare 2 Fast enable                 */
#define  TIM_CCMR1_OC2PE                     ((uint32_t)0x0800U)            /*!<Output Compare 2 Preload enable              */

#define  TIM_CCMR1_OC2M                      ((uint32_t)0x7000U)            /*!<OC2M[2:0] bits (Output Compare 2 Mode)       */
#define  TIM_CCMR1_OC2M_0                    ((uint32_t)0x1000U)            /*!<Bit 0 */
#define  TIM_CCMR1_OC2M_1                    ((uint32_t)0x2000U)            /*!<Bit 1 */
#define  TIM_CCMR1_OC2M_2                    ((uint32_t)0x4000U)            /*!<Bit 2 */

#define  TIM_CCMR1_OC2CE                     ((uint32_t)0x8000U)            /*!<Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    ((uint32_t)0x000CU)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  ((uint32_t)0x0004U)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  ((uint32_t)0x0008U)            /*!<Bit 1 */

#define  TIM_CCMR1_IC1F                      ((uint32_t)0x00F0U)            /*!<IC1F[3:0] bits (Input Capture 1 Filter)      */
#define  TIM_CCMR1_IC1F_0                    ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1F_1                    ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  TIM_CCMR1_IC1F_2                    ((uint32_t)0x0040U)            /*!<Bit 2 */
#define  TIM_CCMR1_IC1F_3                    ((uint32_t)0x0080U)            /*!<Bit 3 */

#define  TIM_CCMR1_IC2PSC                    ((uint32_t)0x0C00U)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler)  */
#define  TIM_CCMR1_IC2PSC_0                  ((uint32_t)0x0400U)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  ((uint32_t)0x0800U)            /*!<Bit 1 */

#define  TIM_CCMR1_IC2F                      ((uint32_t)0xF000U)            /*!<IC2F[3:0] bits (Input Capture 2 Filter)       */
#define  TIM_CCMR1_IC2F_0                    ((uint32_t)0x1000U)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2F_1                    ((uint32_t)0x2000U)            /*!<Bit 1 */
#define  TIM_CCMR1_IC2F_2                    ((uint32_t)0x4000U)            /*!<Bit 2 */
#define  TIM_CCMR1_IC2F_3                    ((uint32_t)0x8000U)            /*!<Bit 3 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      ((uint32_t)0x0003U)            /*!<CC3S[1:0] bits (Capture/Compare 3 Selection)  */
#define  TIM_CCMR2_CC3S_0                    ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  TIM_CCMR2_CC3S_1                    ((uint32_t)0x0002U)            /*!<Bit 1 */

#define  TIM_CCMR2_OC3FE                     ((uint32_t)0x0004U)            /*!<Output Compare 3 Fast enable           */
#define  TIM_CCMR2_OC3PE                     ((uint32_t)0x0008U)            /*!<Output Compare 3 Preload enable        */

#define  TIM_CCMR2_OC3M                      ((uint32_t)0x0070U)            /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  TIM_CCMR2_OC3M_1                    ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  TIM_CCMR2_OC3M_2                    ((uint32_t)0x0040U)            /*!<Bit 2 */

#define  TIM_CCMR2_OC3CE                     ((uint32_t)0x0080U)            /*!<Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      ((uint32_t)0x0300U)            /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    ((uint32_t)0x0100U)            /*!<Bit 0 */
#define  TIM_CCMR2_CC4S_1                    ((uint32_t)0x0200U)            /*!<Bit 1 */

#define  TIM_CCMR2_OC4FE                     ((uint32_t)0x0400U)            /*!<Output Compare 4 Fast enable    */
#define  TIM_CCMR2_OC4PE                     ((uint32_t)0x0800U)            /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      ((uint32_t)0x7000U)            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    ((uint32_t)0x1000U)            /*!<Bit 0 */
#define  TIM_CCMR2_OC4M_1                    ((uint32_t)0x2000U)            /*!<Bit 1 */
#define  TIM_CCMR2_OC4M_2                    ((uint32_t)0x4000U)            /*!<Bit 2 */

#define  TIM_CCMR2_OC4CE                     ((uint32_t)0x8000U)            /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    ((uint32_t)0x000CU)            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  ((uint32_t)0x0004U)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  ((uint32_t)0x0008U)            /*!<Bit 1 */

#define  TIM_CCMR2_IC3F                      ((uint32_t)0x00F0U)            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    ((uint32_t)0x0010U)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3F_1                    ((uint32_t)0x0020U)            /*!<Bit 1 */
#define  TIM_CCMR2_IC3F_2                    ((uint32_t)0x0040U)            /*!<Bit 2 */
#define  TIM_CCMR2_IC3F_3                    ((uint32_t)0x0080U)            /*!<Bit 3 */

#define  TIM_CCMR2_IC4PSC                    ((uint32_t)0x0C00U)            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  ((uint32_t)0x0400U)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  ((uint32_t)0x0800U)            /*!<Bit 1 */

#define  TIM_CCMR2_IC4F                      ((uint32_t)0xF000U)            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    ((uint32_t)0x1000U)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4F_1                    ((uint32_t)0x2000U)            /*!<Bit 1 */
#define  TIM_CCMR2_IC4F_2                    ((uint32_t)0x4000U)            /*!<Bit 2 */
#define  TIM_CCMR2_IC4F_3                    ((uint32_t)0x8000U)            /*!<Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       ((uint32_t)0x0001U)            /*!<Capture/Compare 1 output enable                 */
#define  TIM_CCER_CC1P                       ((uint32_t)0x0002U)            /*!<Capture/Compare 1 output Polarity               */
#define  TIM_CCER_CC1NE                      ((uint32_t)0x0004U)            /*!<Capture/Compare 1 Complementary output enable   */
#define  TIM_CCER_CC1NP                      ((uint32_t)0x0008U)            /*!<Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       ((uint32_t)0x0010U)            /*!<Capture/Compare 2 output enable                 */
#define  TIM_CCER_CC2P                       ((uint32_t)0x0020U)            /*!<Capture/Compare 2 output Polarity               */
#define  TIM_CCER_CC2NE                      ((uint32_t)0x0040U)            /*!<Capture/Compare 2 Complementary output enable   */
#define  TIM_CCER_CC2NP                      ((uint32_t)0x0080U)            /*!<Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       ((uint32_t)0x0100U)            /*!<Capture/Compare 3 output enable                 */
#define  TIM_CCER_CC3P                       ((uint32_t)0x0200U)            /*!<Capture/Compare 3 output Polarity               */
#define  TIM_CCER_CC3NE                      ((uint32_t)0x0400U)            /*!<Capture/Compare 3 Complementary output enable   */
#define  TIM_CCER_CC3NP                      ((uint32_t)0x0800U)            /*!<Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       ((uint32_t)0x1000U)            /*!<Capture/Compare 4 output enable                 */
#define  TIM_CCER_CC4P                       ((uint32_t)0x2000U)            /*!<Capture/Compare 4 output Polarity               */
#define  TIM_CCER_CC4NP                      ((uint32_t)0x8000U)            /*!<Capture/Compare 4 Complementary output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         ((uint32_t)0xFFFFU)            /*!<Counter Value            */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         ((uint32_t)0xFFFFU)            /*!<Prescaler Value          */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         ((uint32_t)0xFFFFU)            /*!<actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint32_t)0xFFU)               /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       ((uint32_t)0xFFFFU)            /*!<Capture/Compare 1 Value  */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define  TIM_CCR2_CCR2                       ((uint32_t)0xFFFFU)            /*!<Capture/Compare 2 Value  */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define  TIM_CCR3_CCR3                       ((uint32_t)0xFFFFU)            /*!<Capture/Compare 3 Value  */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define  TIM_CCR4_CCR4                       ((uint32_t)0xFFFFU)            /*!<Capture/Compare 4 Value  */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_BDTR_DTG                        ((uint32_t)0x00FFU)            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_BDTR_DTG_0                      ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  TIM_BDTR_DTG_1                      ((uint32_t)0x0002U)            /*!<Bit 1 */
#define  TIM_BDTR_DTG_2                      ((uint32_t)0x0004U)            /*!<Bit 2 */
#define  TIM_BDTR_DTG_3                      ((uint32_t)0x0008U)            /*!<Bit 3 */
#define  TIM_BDTR_DTG_4                      ((uint32_t)0x0010U)            /*!<Bit 4 */
#define  TIM_BDTR_DTG_5                      ((uint32_t)0x0020U)            /*!<Bit 5 */
#define  TIM_BDTR_DTG_6                      ((uint32_t)0x0040U)            /*!<Bit 6 */
#define  TIM_BDTR_DTG_7                      ((uint32_t)0x0080U)            /*!<Bit 7 */

#define  TIM_BDTR_LOCK                       ((uint32_t)0x0300U)            /*!<LOCK[1:0] bits (Lock Configuration) */
#define  TIM_BDTR_LOCK_0                     ((uint32_t)0x0100U)            /*!<Bit 0 */
#define  TIM_BDTR_LOCK_1                     ((uint32_t)0x0200U)            /*!<Bit 1 */

#define  TIM_BDTR_OSSI                       ((uint32_t)0x0400U)            /*!<Off-State Selection for Idle mode */
#define  TIM_BDTR_OSSR                       ((uint32_t)0x0800U)            /*!<Off-State Selection for Run mode  */
#define  TIM_BDTR_BKE                        ((uint32_t)0x1000U)            /*!<Break enable                      */
#define  TIM_BDTR_BKP                        ((uint32_t)0x2000U)            /*!<Break Polarity                    */
#define  TIM_BDTR_AOE                        ((uint32_t)0x4000U)            /*!<Automatic Output enable           */
#define  TIM_BDTR_MOE                        ((uint32_t)0x8000U)            /*!<Main Output enable                */

/*******************  Bit definition for TIM_DCR register  ********************/
#define  TIM_DCR_DBA                         ((uint32_t)0x001FU)            /*!<DBA[4:0] bits (DMA Base Address) */
#define  TIM_DCR_DBA_0                       ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  TIM_DCR_DBA_1                       ((uint32_t)0x0002U)            /*!<Bit 1 */
#define  TIM_DCR_DBA_2                       ((uint32_t)0x0004U)            /*!<Bit 2 */
#define  TIM_DCR_DBA_3                       ((uint32_t)0x0008U)            /*!<Bit 3 */
#define  TIM_DCR_DBA_4                       ((uint32_t)0x0010U)            /*!<Bit 4 */

#define  TIM_DCR_DBL                         ((uint32_t)0x1F00U)            /*!<DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DCR_DBL_0                       ((uint32_t)0x0100U)            /*!<Bit 0 */
#define  TIM_DCR_DBL_1                       ((uint32_t)0x0200U)            /*!<Bit 1 */
#define  TIM_DCR_DBL_2                       ((uint32_t)0x0400U)            /*!<Bit 2 */
#define  TIM_DCR_DBL_3                       ((uint32_t)0x0800U)            /*!<Bit 3 */
#define  TIM_DCR_DBL_4                       ((uint32_t)0x1000U)            /*!<Bit 4 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define  TIM_DMAR_DMAB                       ((uint32_t)0xFFFFU)            /*!<DMA register for burst accesses                    */

/*******************  Bit definition for TIM_OR register  *********************/
#define TIM_OR_TI4_RMP                       ((uint32_t)0x00C0U)            /*!<TI4_RMP[1:0] bits (TIM5 Input 4 remap)             */
#define TIM_OR_TI4_RMP_0                     ((uint32_t)0x0040U)            /*!<Bit 0 */
#define TIM_OR_TI4_RMP_1                     ((uint32_t)0x0080U)            /*!<Bit 1 */
#define TIM_OR_ITR1_RMP                      ((uint32_t)0x0C00U)            /*!<ITR1_RMP[1:0] bits (TIM2 Internal trigger 1 remap) */
#define TIM_OR_ITR1_RMP_0                    ((uint32_t)0x0400U)            /*!<Bit 0 */
#define TIM_OR_ITR1_RMP_1                    ((uint32_t)0x0800U)            /*!<Bit 1 */


/******************************************************************************/
/*                                                                            */
/*         Universal Synchronous Asynchronous Receiver Transmitter            */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for USART_SR register  *******************/
#define  USART_SR_PE                         ((uint32_t)0x0001U)            /*!<Parity Error                 */
#define  USART_SR_FE                         ((uint32_t)0x0002U)            /*!<Framing Error                */
#define  USART_SR_NE                         ((uint32_t)0x0004U)            /*!<Noise Error Flag             */
#define  USART_SR_ORE                        ((uint32_t)0x0008U)            /*!<OverRun Error                */
#define  USART_SR_IDLE                       ((uint32_t)0x0010U)            /*!<IDLE line detected           */
#define  USART_SR_RXNE                       ((uint32_t)0x0020U)            /*!<Read Data Register Not Empty */
#define  USART_SR_TC                         ((uint32_t)0x0040U)            /*!<Transmission Complete        */
#define  USART_SR_TXE                        ((uint32_t)0x0080U)            /*!<Transmit Data Register Empty */
#define  USART_SR_LBD                        ((uint32_t)0x0100U)            /*!<LIN Break Detection Flag     */
#define  USART_SR_CTS                        ((uint32_t)0x0200U)            /*!<CTS Flag                     */

/*******************  Bit definition for USART_DR register  *******************/
#define  USART_DR_DR                         ((uint32_t)0x01FFU)            /*!<Data value */

/******************  Bit definition for USART_BRR register  *******************/
#define  USART_BRR_DIV_Fraction              ((uint32_t)0x000FU)            /*!<Fraction of USARTDIV */
#define  USART_BRR_DIV_Mantissa              ((uint32_t)0xFFF0U)            /*!<Mantissa of USARTDIV */

/******************  Bit definition for USART_CR1 register  *******************/
#define  USART_CR1_SBK                       ((uint32_t)0x0001U)            /*!<Send Break                             */
#define  USART_CR1_RWU                       ((uint32_t)0x0002U)            /*!<Receiver wakeup                        */
#define  USART_CR1_RE                        ((uint32_t)0x0004U)            /*!<Receiver Enable                        */
#define  USART_CR1_TE                        ((uint32_t)0x0008U)            /*!<Transmitter Enable                     */
#define  USART_CR1_IDLEIE                    ((uint32_t)0x0010U)            /*!<IDLE Interrupt Enable                  */
#define  USART_CR1_RXNEIE                    ((uint32_t)0x0020U)            /*!<RXNE Interrupt Enable                  */
#define  USART_CR1_TCIE                      ((uint32_t)0x0040U)            /*!<Transmission Complete Interrupt Enable */
#define  USART_CR1_TXEIE                     ((uint32_t)0x0080U)            /*!<PE Interrupt Enable                    */
#define  USART_CR1_PEIE                      ((uint32_t)0x0100U)            /*!<PE Interrupt Enable                    */
#define  USART_CR1_PS                        ((uint32_t)0x0200U)            /*!<Parity Selection                       */
#define  USART_CR1_PCE                       ((uint32_t)0x0400U)            /*!<Parity Control Enable                  */
#define  USART_CR1_WAKE                      ((uint32_t)0x0800U)            /*!<Wakeup method                          */
#define  USART_CR1_M                         ((uint32_t)0x1000U)            /*!<Word length                            */
#define  USART_CR1_UE                        ((uint32_t)0x2000U)            /*!<USART Enable                           */
#define  USART_CR1_OVER8                     ((uint32_t)0x8000U)            /*!<USART Oversampling by 8 enable         */

/******************  Bit definition for USART_CR2 register  *******************/
#define  USART_CR2_ADD                       ((uint32_t)0x000FU)            /*!<Address of the USART node            */
#define  USART_CR2_LBDL                      ((uint32_t)0x0020U)            /*!<LIN Break Detection Length           */
#define  USART_CR2_LBDIE                     ((uint32_t)0x0040U)            /*!<LIN Break Detection Interrupt Enable */
#define  USART_CR2_LBCL                      ((uint32_t)0x0100U)            /*!<Last Bit Clock pulse                 */
#define  USART_CR2_CPHA                      ((uint32_t)0x0200U)            /*!<Clock Phase                          */
#define  USART_CR2_CPOL                      ((uint32_t)0x0400U)            /*!<Clock Polarity                       */
#define  USART_CR2_CLKEN                     ((uint32_t)0x0800U)            /*!<Clock Enable                         */

#define  USART_CR2_STOP                      ((uint32_t)0x3000U)            /*!<STOP[1:0] bits (STOP bits) */
#define  USART_CR2_STOP_0                    ((uint32_t)0x1000U)            /*!<Bit 0 */
#define  USART_CR2_STOP_1                    ((uint32_t)0x2000U)            /*!<Bit 1 */

#define  USART_CR2_LINEN                     ((uint32_t)0x4000U)            /*!<LIN mode enable */

/******************  Bit definition for USART_CR3 register  *******************/
#define  USART_CR3_EIE                       ((uint32_t)0x0001U)            /*!<Error Interrupt Enable      */
#define  USART_CR3_IREN                      ((uint32_t)0x0002U)            /*!<IrDA mode Enable            */
#define  USART_CR3_IRLP                      ((uint32_t)0x0004U)            /*!<IrDA Low-Power              */
#define  USART_CR3_HDSEL                     ((uint32_t)0x0008U)            /*!<Half-Duplex Selection       */
#define  USART_CR3_NACK                      ((uint32_t)0x0010U)            /*!<Smartcard NACK enable       */
#define  USART_CR3_SCEN                      ((uint32_t)0x0020U)            /*!<Smartcard mode enable       */
#define  USART_CR3_DMAR                      ((uint32_t)0x0040U)            /*!<DMA Enable Receiver         */
#define  USART_CR3_DMAT                      ((uint32_t)0x0080U)            /*!<DMA Enable Transmitter      */
#define  USART_CR3_RTSE                      ((uint32_t)0x0100U)            /*!<RTS Enable                  */
#define  USART_CR3_CTSE                      ((uint32_t)0x0200U)            /*!<CTS Enable                  */
#define  USART_CR3_CTSIE                     ((uint32_t)0x0400U)            /*!<CTS Interrupt Enable        */
#define  USART_CR3_ONEBIT                    ((uint32_t)0x0800U)            /*!<USART One bit method enable */

/******************  Bit definition for USART_GTPR register  ******************/
#define  USART_GTPR_PSC                      ((uint32_t)0x00FFU)            /*!<PSC[7:0] bits (Prescaler value) */
#define  USART_GTPR_PSC_0                    ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  USART_GTPR_PSC_1                    ((uint32_t)0x0002U)            /*!<Bit 1 */
#define  USART_GTPR_PSC_2                    ((uint32_t)0x0004U)            /*!<Bit 2 */
#define  USART_GTPR_PSC_3                    ((uint32_t)0x0008U)            /*!<Bit 3 */
#define  USART_GTPR_PSC_4                    ((uint32_t)0x0010U)            /*!<Bit 4 */
#define  USART_GTPR_PSC_5                    ((uint32_t)0x0020U)            /*!<Bit 5 */
#define  USART_GTPR_PSC_6                    ((uint32_t)0x0040U)            /*!<Bit 6 */
#define  USART_GTPR_PSC_7                    ((uint32_t)0x0080U)            /*!<Bit 7 */

#define  USART_GTPR_GT                       ((uint32_t)0xFF00U)            /*!<Guard time value */

/******************************************************************************/
/*                                                                            */
/*                            Window WATCHDOG                                 */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for WWDG_CR register  ********************/
#define  WWDG_CR_T                           ((uint32_t)0x7FU)               /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CR_T0                          ((uint32_t)0x01U)               /*!<Bit 0 */
#define  WWDG_CR_T1                          ((uint32_t)0x02U)               /*!<Bit 1 */
#define  WWDG_CR_T2                          ((uint32_t)0x04U)               /*!<Bit 2 */
#define  WWDG_CR_T3                          ((uint32_t)0x08U)               /*!<Bit 3 */
#define  WWDG_CR_T4                          ((uint32_t)0x10U)               /*!<Bit 4 */
#define  WWDG_CR_T5                          ((uint32_t)0x20U)               /*!<Bit 5 */
#define  WWDG_CR_T6                          ((uint32_t)0x40U)               /*!<Bit 6 */

#define  WWDG_CR_WDGA                        ((uint32_t)0x80U)               /*!<Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define  WWDG_CFR_W                          ((uint32_t)0x007FU)            /*!<W[6:0] bits (7-bit window value) */
#define  WWDG_CFR_W0                         ((uint32_t)0x0001U)            /*!<Bit 0 */
#define  WWDG_CFR_W1                         ((uint32_t)0x0002U)            /*!<Bit 1 */
#define  WWDG_CFR_W2                         ((uint32_t)0x0004U)            /*!<Bit 2 */
#define  WWDG_CFR_W3                         ((uint32_t)0x0008U)            /*!<Bit 3 */
#define  WWDG_CFR_W4                         ((uint32_t)0x0010U)            /*!<Bit 4 */
#define  WWDG_CFR_W5                         ((uint32_t)0x0020U)            /*!<Bit 5 */
#define  WWDG_CFR_W6                         ((uint32_t)0x0040U)            /*!<Bit 6 */

#define  WWDG_CFR_WDGTB                      ((uint32_t)0x0180U)            /*!<WDGTB[1:0] bits (Timer Base) */
#define  WWDG_CFR_WDGTB0                     ((uint32_t)0x0080U)            /*!<Bit 0 */
#define  WWDG_CFR_WDGTB1                     ((uint32_t)0x0100U)            /*!<Bit 1 */

#define  WWDG_CFR_EWI                        ((uint32_t)0x0200U)            /*!<Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_SR register  ********************/
#define  WWDG_SR_EWIF                        ((uint32_t)0x01U)               /*!<Early Wakeup Interrupt Flag */


/******************************************************************************/
/*                                                                            */
/*                                DBG                                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DBGMCU_IDCODE register  *************/
#define  DBGMCU_IDCODE_DEV_ID                ((uint32_t)0x00000FFFU)
#define  DBGMCU_IDCODE_REV_ID                ((uint32_t)0xFFFF0000U)

/********************  Bit definition for DBGMCU_CR register  *****************/
#define  DBGMCU_CR_DBG_SLEEP                 ((uint32_t)0x00000001U)
#define  DBGMCU_CR_DBG_STOP                  ((uint32_t)0x00000002U)
#define  DBGMCU_CR_DBG_STANDBY               ((uint32_t)0x00000004U)
#define  DBGMCU_CR_TRACE_IOEN                ((uint32_t)0x00000020U)

#define  DBGMCU_CR_TRACE_MODE                ((uint32_t)0x000000C0U)
#define  DBGMCU_CR_TRACE_MODE_0              ((uint32_t)0x00000040U)/*!<Bit 0 */
#define  DBGMCU_CR_TRACE_MODE_1              ((uint32_t)0x00000080U)/*!<Bit 1 */

/********************  Bit definition for DBGMCU_APB1_FZ register  ************/
#define  DBGMCU_APB1_FZ_DBG_TIM2_STOP            ((uint32_t)0x00000001U)
#define  DBGMCU_APB1_FZ_DBG_TIM3_STOP            ((uint32_t)0x00000002U)
#define  DBGMCU_APB1_FZ_DBG_TIM4_STOP            ((uint32_t)0x00000004U)
#define  DBGMCU_APB1_FZ_DBG_TIM5_STOP            ((uint32_t)0x00000008U)
#define  DBGMCU_APB1_FZ_DBG_TIM6_STOP            ((uint32_t)0x00000010U)
#define  DBGMCU_APB1_FZ_DBG_TIM7_STOP            ((uint32_t)0x00000020U)
#define  DBGMCU_APB1_FZ_DBG_TIM12_STOP           ((uint32_t)0x00000040U)
#define  DBGMCU_APB1_FZ_DBG_TIM13_STOP           ((uint32_t)0x00000080U)
#define  DBGMCU_APB1_FZ_DBG_TIM14_STOP           ((uint32_t)0x00000100U)
#define  DBGMCU_APB1_FZ_DBG_RTC_STOP             ((uint32_t)0x00000400U)
#define  DBGMCU_APB1_FZ_DBG_WWDG_STOP            ((uint32_t)0x00000800U)
#define  DBGMCU_APB1_FZ_DBG_IWDG_STOP            ((uint32_t)0x00001000U)
#define  DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT   ((uint32_t)0x00200000U)
#define  DBGMCU_APB1_FZ_DBG_I2C2_SMBUS_TIMEOUT   ((uint32_t)0x00400000U)
#define  DBGMCU_APB1_FZ_DBG_I2C3_SMBUS_TIMEOUT   ((uint32_t)0x00800000U)
#define  DBGMCU_APB1_FZ_DBG_CAN1_STOP            ((uint32_t)0x02000000U)
#define  DBGMCU_APB1_FZ_DBG_CAN2_STOP            ((uint32_t)0x04000000U)
/* Old IWDGSTOP bit definition, maintained for legacy purpose */
#define  DBGMCU_APB1_FZ_DBG_IWDEG_STOP           DBGMCU_APB1_FZ_DBG_IWDG_STOP

/********************  Bit definition for DBGMCU_APB2_FZ register  ************/
#define  DBGMCU_APB2_FZ_DBG_TIM1_STOP        ((uint32_t)0x00000001U)
#define  DBGMCU_APB2_FZ_DBG_TIM8_STOP        ((uint32_t)0x00000002U)
#define  DBGMCU_APB2_FZ_DBG_TIM9_STOP        ((uint32_t)0x00010000U)
#define  DBGMCU_APB2_FZ_DBG_TIM10_STOP       ((uint32_t)0x00020000U)
#define  DBGMCU_APB2_FZ_DBG_TIM11_STOP       ((uint32_t)0x00040000U)

/******************************************************************************/
/*                                                                            */
/*                Ethernet MAC Registers bits definitions                     */
/*                                                                            */
/******************************************************************************/
/* Bit definition for Ethernet MAC Control Register register */
#define ETH_MACCR_WD      ((uint32_t)0x00800000U)  /* Watchdog disable */
#define ETH_MACCR_JD      ((uint32_t)0x00400000U)  /* Jabber disable */
#define ETH_MACCR_IFG     ((uint32_t)0x000E0000U)  /* Inter-frame gap */
#define ETH_MACCR_IFG_96Bit     ((uint32_t)0x00000000U)  /* Minimum IFG between frames during transmission is 96Bit */
  #define ETH_MACCR_IFG_88Bit     ((uint32_t)0x00020000U)  /* Minimum IFG between frames during transmission is 88Bit */
  #define ETH_MACCR_IFG_80Bit     ((uint32_t)0x00040000U)  /* Minimum IFG between frames during transmission is 80Bit */
  #define ETH_MACCR_IFG_72Bit     ((uint32_t)0x00060000U)  /* Minimum IFG between frames during transmission is 72Bit */
  #define ETH_MACCR_IFG_64Bit     ((uint32_t)0x00080000U)  /* Minimum IFG between frames during transmission is 64Bit */
  #define ETH_MACCR_IFG_56Bit     ((uint32_t)0x000A0000U)  /* Minimum IFG between frames during transmission is 56Bit */
  #define ETH_MACCR_IFG_48Bit     ((uint32_t)0x000C0000U)  /* Minimum IFG between frames during transmission is 48Bit */
  #define ETH_MACCR_IFG_40Bit     ((uint32_t)0x000E0000U)  /* Minimum IFG between frames during transmission is 40Bit */
#define ETH_MACCR_CSD     ((uint32_t)0x00010000U)  /* Carrier sense disable (during transmission) */
#define ETH_MACCR_FES     ((uint32_t)0x00004000U)  /* Fast ethernet speed */
#define ETH_MACCR_ROD     ((uint32_t)0x00002000U)  /* Receive own disable */
#define ETH_MACCR_LM      ((uint32_t)0x00001000U)  /* loopback mode */
#define ETH_MACCR_DM      ((uint32_t)0x00000800U)  /* Duplex mode */
#define ETH_MACCR_IPCO    ((uint32_t)0x00000400U)  /* IP Checksum offload */
#define ETH_MACCR_RD      ((uint32_t)0x00000200U)  /* Retry disable */
#define ETH_MACCR_APCS    ((uint32_t)0x00000080U)  /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL      ((uint32_t)0x00000060U)  /* Back-off limit: random integer number (r) of slot time delays before rescheduling
                                                       a transmission attempt during retries after a collision: 0 =< r <2^k */
  #define ETH_MACCR_BL_10    ((uint32_t)0x00000000U)  /* k = min (n, 10) */
  #define ETH_MACCR_BL_8     ((uint32_t)0x00000020U)  /* k = min (n, 8) */
  #define ETH_MACCR_BL_4     ((uint32_t)0x00000040U)  /* k = min (n, 4) */
  #define ETH_MACCR_BL_1     ((uint32_t)0x00000060U)  /* k = min (n, 1) */
#define ETH_MACCR_DC      ((uint32_t)0x00000010U)  /* Defferal check */
#define ETH_MACCR_TE      ((uint32_t)0x00000008U)  /* Transmitter enable */
#define ETH_MACCR_RE      ((uint32_t)0x00000004U)  /* Receiver enable */

/* Bit definition for Ethernet MAC Frame Filter Register */
#define ETH_MACFFR_RA     ((uint32_t)0x80000000U)  /* Receive all */
#define ETH_MACFFR_HPF    ((uint32_t)0x00000400U)  /* Hash or perfect filter */
#define ETH_MACFFR_SAF    ((uint32_t)0x00000200U)  /* Source address filter enable */
#define ETH_MACFFR_SAIF   ((uint32_t)0x00000100U)  /* SA inverse filtering */
#define ETH_MACFFR_PCF    ((uint32_t)0x000000C0U)  /* Pass control frames: 3 cases */
  #define ETH_MACFFR_PCF_BlockAll                ((uint32_t)0x00000040U)  /* MAC filters all control frames from reaching the application */
  #define ETH_MACFFR_PCF_ForwardAll              ((uint32_t)0x00000080U)  /* MAC forwards all control frames to application even if they fail the Address Filter */
  #define ETH_MACFFR_PCF_ForwardPassedAddrFilter ((uint32_t)0x000000C0U)  /* MAC forwards control frames that pass the Address Filter. */
#define ETH_MACFFR_BFD    ((uint32_t)0x00000020U)  /* Broadcast frame disable */
#define ETH_MACFFR_PAM    ((uint32_t)0x00000010U)  /* Pass all mutlicast */
#define ETH_MACFFR_DAIF   ((uint32_t)0x00000008U)  /* DA Inverse filtering */
#define ETH_MACFFR_HM     ((uint32_t)0x00000004U)  /* Hash multicast */
#define ETH_MACFFR_HU     ((uint32_t)0x00000002U)  /* Hash unicast */
#define ETH_MACFFR_PM     ((uint32_t)0x00000001U)  /* Promiscuous mode */

/* Bit definition for Ethernet MAC Hash Table High Register */
#define ETH_MACHTHR_HTH   ((uint32_t)0xFFFFFFFFU)  /* Hash table high */

/* Bit definition for Ethernet MAC Hash Table Low Register */
#define ETH_MACHTLR_HTL   ((uint32_t)0xFFFFFFFFU)  /* Hash table low */

/* Bit definition for Ethernet MAC MII Address Register */
#define ETH_MACMIIAR_PA   ((uint32_t)0x0000F800U)  /* Physical layer address */
#define ETH_MACMIIAR_MR   ((uint32_t)0x000007C0U)  /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR   ((uint32_t)0x0000001CU)  /* CR clock range: 6 cases */
  #define ETH_MACMIIAR_CR_Div42   ((uint32_t)0x00000000U)  /* HCLK:60-100 MHz; MDC clock= HCLK/42 */
  #define ETH_MACMIIAR_CR_Div62   ((uint32_t)0x00000004U)  /* HCLK:100-150 MHz; MDC clock= HCLK/62 */
  #define ETH_MACMIIAR_CR_Div16   ((uint32_t)0x00000008U)  /* HCLK:20-35 MHz; MDC clock= HCLK/16 */
  #define ETH_MACMIIAR_CR_Div26   ((uint32_t)0x0000000CU)  /* HCLK:35-60 MHz; MDC clock= HCLK/26 */
  #define ETH_MACMIIAR_CR_Div102  ((uint32_t)0x00000010U)  /* HCLK:150-168 MHz; MDC clock= HCLK/102 */
#define ETH_MACMIIAR_MW   ((uint32_t)0x00000002U)  /* MII write */
#define ETH_MACMIIAR_MB   ((uint32_t)0x00000001U)  /* MII busy */

/* Bit definition for Ethernet MAC MII Data Register */
#define ETH_MACMIIDR_MD   ((uint32_t)0x0000FFFFU)  /* MII data: read/write data from/to PHY */

/* Bit definition for Ethernet MAC Flow Control Register */
#define ETH_MACFCR_PT     ((uint32_t)0xFFFF0000U)  /* Pause time */
#define ETH_MACFCR_ZQPD   ((uint32_t)0x00000080U)  /* Zero-quanta pause disable */
#define ETH_MACFCR_PLT    ((uint32_t)0x00000030U)  /* Pause low threshold: 4 cases */
  #define ETH_MACFCR_PLT_Minus4   ((uint32_t)0x00000000U)  /* Pause time minus 4 slot times */
  #define ETH_MACFCR_PLT_Minus28  ((uint32_t)0x00000010U)  /* Pause time minus 28 slot times */
  #define ETH_MACFCR_PLT_Minus144 ((uint32_t)0x00000020U)  /* Pause time minus 144 slot times */
  #define ETH_MACFCR_PLT_Minus256 ((uint32_t)0x00000030U)  /* Pause time minus 256 slot times */
#define ETH_MACFCR_UPFD   ((uint32_t)0x00000008U)  /* Unicast pause frame detect */
#define ETH_MACFCR_RFCE   ((uint32_t)0x00000004U)  /* Receive flow control enable */
#define ETH_MACFCR_TFCE   ((uint32_t)0x00000002U)  /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA ((uint32_t)0x00000001U)  /* Flow control busy/backpressure activate */

/* Bit definition for Ethernet MAC VLAN Tag Register */
#define ETH_MACVLANTR_VLANTC ((uint32_t)0x00010000U)  /* 12-bit VLAN tag comparison */
#define ETH_MACVLANTR_VLANTI ((uint32_t)0x0000FFFFU)  /* VLAN tag identifier (for receive frames) */

/* Bit definition for Ethernet MAC Remote Wake-UpFrame Filter Register */
#define ETH_MACRWUFFR_D   ((uint32_t)0xFFFFFFFFU)  /* Wake-up frame filter register data */
/* Eight sequential Writes to this address (offset 0x28) will write all Wake-UpFrame Filter Registers.
   Eight sequential Reads from this address (offset 0x28) will read all Wake-UpFrame Filter Registers. */
/* Wake-UpFrame Filter Reg0 : Filter 0 Byte Mask
   Wake-UpFrame Filter Reg1 : Filter 1 Byte Mask
   Wake-UpFrame Filter Reg2 : Filter 2 Byte Mask
   Wake-UpFrame Filter Reg3 : Filter 3 Byte Mask
   Wake-UpFrame Filter Reg4 : RSVD - Filter3 Command - RSVD - Filter2 Command -
                              RSVD - Filter1 Command - RSVD - Filter0 Command
   Wake-UpFrame Filter Re5 : Filter3 Offset - Filter2 Offset - Filter1 Offset - Filter0 Offset
   Wake-UpFrame Filter Re6 : Filter1 CRC16 - Filter0 CRC16
   Wake-UpFrame Filter Re7 : Filter3 CRC16 - Filter2 CRC16 */

/* Bit definition for Ethernet MAC PMT Control and Status Register */
#define ETH_MACPMTCSR_WFFRPR ((uint32_t)0x80000000U)  /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU     ((uint32_t)0x00000200U)  /* Global Unicast */
#define ETH_MACPMTCSR_WFR    ((uint32_t)0x00000040U)  /* Wake-Up Frame Received */
#define ETH_MACPMTCSR_MPR    ((uint32_t)0x00000020U)  /* Magic Packet Received */
#define ETH_MACPMTCSR_WFE    ((uint32_t)0x00000004U)  /* Wake-Up Frame Enable */
#define ETH_MACPMTCSR_MPE    ((uint32_t)0x00000002U)  /* Magic Packet Enable */
#define ETH_MACPMTCSR_PD     ((uint32_t)0x00000001U)  /* Power Down */

/* Bit definition for Ethernet MAC Status Register */
#define ETH_MACSR_TSTS      ((uint32_t)0x00000200U)  /* Time stamp trigger status */
#define ETH_MACSR_MMCTS     ((uint32_t)0x00000040U)  /* MMC transmit status */
#define ETH_MACSR_MMMCRS    ((uint32_t)0x00000020U)  /* MMC receive status */
#define ETH_MACSR_MMCS      ((uint32_t)0x00000010U)  /* MMC status */
#define ETH_MACSR_PMTS      ((uint32_t)0x00000008U)  /* PMT status */

/* Bit definition for Ethernet MAC Interrupt Mask Register */
#define ETH_MACIMR_TSTIM     ((uint32_t)0x00000200U)  /* Time stamp trigger interrupt mask */
#define ETH_MACIMR_PMTIM     ((uint32_t)0x00000008U)  /* PMT interrupt mask */

/* Bit definition for Ethernet MAC Address0 High Register */
#define ETH_MACA0HR_MACA0H   ((uint32_t)0x0000FFFFU)  /* MAC address0 high */

/* Bit definition for Ethernet MAC Address0 Low Register */
#define ETH_MACA0LR_MACA0L   ((uint32_t)0xFFFFFFFFU)  /* MAC address0 low */

/* Bit definition for Ethernet MAC Address1 High Register */
#define ETH_MACA1HR_AE       ((uint32_t)0x80000000U)  /* Address enable */
#define ETH_MACA1HR_SA       ((uint32_t)0x40000000U)  /* Source address */
#define ETH_MACA1HR_MBC      ((uint32_t)0x3F000000U)  /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
  #define ETH_MACA1HR_MBC_HBits15_8    ((uint32_t)0x20000000U)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA1HR_MBC_HBits7_0     ((uint32_t)0x10000000U)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA1HR_MBC_LBits31_24   ((uint32_t)0x08000000U)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA1HR_MBC_LBits23_16   ((uint32_t)0x04000000U)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA1HR_MBC_LBits15_8    ((uint32_t)0x02000000U)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA1HR_MBC_LBits7_0     ((uint32_t)0x01000000U)  /* Mask MAC Address low reg bits [7:0] */
#define ETH_MACA1HR_MACA1H   ((uint32_t)0x0000FFFFU)  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address1 Low Register */
#define ETH_MACA1LR_MACA1L   ((uint32_t)0xFFFFFFFFU)  /* MAC address1 low */

/* Bit definition for Ethernet MAC Address2 High Register */
#define ETH_MACA2HR_AE       ((uint32_t)0x80000000U)  /* Address enable */
#define ETH_MACA2HR_SA       ((uint32_t)0x40000000U)  /* Source address */
#define ETH_MACA2HR_MBC      ((uint32_t)0x3F000000U)  /* Mask byte control */
  #define ETH_MACA2HR_MBC_HBits15_8    ((uint32_t)0x20000000U)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA2HR_MBC_HBits7_0     ((uint32_t)0x10000000U)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA2HR_MBC_LBits31_24   ((uint32_t)0x08000000U)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA2HR_MBC_LBits23_16   ((uint32_t)0x04000000U)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA2HR_MBC_LBits15_8    ((uint32_t)0x02000000U)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA2HR_MBC_LBits7_0     ((uint32_t)0x01000000U)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA2HR_MACA2H   ((uint32_t)0x0000FFFFU)  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address2 Low Register */
#define ETH_MACA2LR_MACA2L   ((uint32_t)0xFFFFFFFFU)  /* MAC address2 low */

/* Bit definition for Ethernet MAC Address3 High Register */
#define ETH_MACA3HR_AE       ((uint32_t)0x80000000U)  /* Address enable */
#define ETH_MACA3HR_SA       ((uint32_t)0x40000000U)  /* Source address */
#define ETH_MACA3HR_MBC      ((uint32_t)0x3F000000U)  /* Mask byte control */
  #define ETH_MACA3HR_MBC_HBits15_8    ((uint32_t)0x20000000U)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA3HR_MBC_HBits7_0     ((uint32_t)0x10000000U)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA3HR_MBC_LBits31_24   ((uint32_t)0x08000000U)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA3HR_MBC_LBits23_16   ((uint32_t)0x04000000U)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA3HR_MBC_LBits15_8    ((uint32_t)0x02000000U)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA3HR_MBC_LBits7_0     ((uint32_t)0x01000000U)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA3HR_MACA3H   ((uint32_t)0x0000FFFFU)  /* MAC address3 high */

/* Bit definition for Ethernet MAC Address3 Low Register */
#define ETH_MACA3LR_MACA3L   ((uint32_t)0xFFFFFFFFU)  /* MAC address3 low */

/******************************************************************************/
/*                Ethernet MMC Registers bits definition                      */
/******************************************************************************/

/* Bit definition for Ethernet MMC Contol Register */
#define ETH_MMCCR_MCFHP      ((uint32_t)0x00000020U)  /* MMC counter Full-Half preset */
#define ETH_MMCCR_MCP        ((uint32_t)0x00000010U)  /* MMC counter preset */
#define ETH_MMCCR_MCF        ((uint32_t)0x00000008U)  /* MMC Counter Freeze */
#define ETH_MMCCR_ROR        ((uint32_t)0x00000004U)  /* Reset on Read */
#define ETH_MMCCR_CSR        ((uint32_t)0x00000002U)  /* Counter Stop Rollover */
#define ETH_MMCCR_CR         ((uint32_t)0x00000001U)  /* Counters Reset */

/* Bit definition for Ethernet MMC Receive Interrupt Register */
#define ETH_MMCRIR_RGUFS     ((uint32_t)0x00020000U)  /* Set when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIR_RFAES     ((uint32_t)0x00000040U)  /* Set when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIR_RFCES     ((uint32_t)0x00000020U)  /* Set when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Register */
#define ETH_MMCTIR_TGFS      ((uint32_t)0x00200000U)  /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFMSCS   ((uint32_t)0x00008000U)  /* Set when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS    ((uint32_t)0x00004000U)  /* Set when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Receive Interrupt Mask Register */
#define ETH_MMCRIMR_RGUFM    ((uint32_t)0x00020000U)  /* Mask the interrupt when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIMR_RFAEM    ((uint32_t)0x00000040U)  /* Mask the interrupt when when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIMR_RFCEM    ((uint32_t)0x00000020U)  /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Mask Register */
#define ETH_MMCTIMR_TGFM     ((uint32_t)0x00200000U)  /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFMSCM  ((uint32_t)0x00008000U)  /* Mask the interrupt when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM   ((uint32_t)0x00004000U)  /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmitted Good Frames after Single Collision Counter Register */
#define ETH_MMCTGFSCCR_TGFSCC     ((uint32_t)0xFFFFFFFFU)  /* Number of successfully transmitted frames after a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames after More than a Single Collision Counter Register */
#define ETH_MMCTGFMSCCR_TGFMSCC   ((uint32_t)0xFFFFFFFFU)  /* Number of successfully transmitted frames after more than a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames Counter Register */
#define ETH_MMCTGFCR_TGFC    ((uint32_t)0xFFFFFFFFU)  /* Number of good frames transmitted. */

/* Bit definition for Ethernet MMC Received Frames with CRC Error Counter Register */
#define ETH_MMCRFCECR_RFCEC  ((uint32_t)0xFFFFFFFFU)  /* Number of frames received with CRC error. */

/* Bit definition for Ethernet MMC Received Frames with Alignement Error Counter Register */
#define ETH_MMCRFAECR_RFAEC  ((uint32_t)0xFFFFFFFFU)  /* Number of frames received with alignment (dribble) error */

/* Bit definition for Ethernet MMC Received Good Unicast Frames Counter Register */
#define ETH_MMCRGUFCR_RGUFC  ((uint32_t)0xFFFFFFFFU)  /* Number of good unicast frames received. */

/******************************************************************************/
/*               Ethernet PTP Registers bits definition                       */
/******************************************************************************/

/* Bit definition for Ethernet PTP Time Stamp Contol Register */
#define ETH_PTPTSCR_TSCNT       ((uint32_t)0x00030000U)  /* Time stamp clock node type */
#define ETH_PTPTSSR_TSSMRME     ((uint32_t)0x00008000U)  /* Time stamp snapshot for message relevant to master enable */
#define ETH_PTPTSSR_TSSEME      ((uint32_t)0x00004000U)  /* Time stamp snapshot for event message enable */
#define ETH_PTPTSSR_TSSIPV4FE   ((uint32_t)0x00002000U)  /* Time stamp snapshot for IPv4 frames enable */
#define ETH_PTPTSSR_TSSIPV6FE   ((uint32_t)0x00001000U)  /* Time stamp snapshot for IPv6 frames enable */
#define ETH_PTPTSSR_TSSPTPOEFE  ((uint32_t)0x00000800U)  /* Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_PTPTSSR_TSPTPPSV2E  ((uint32_t)0x00000400U)  /* Time stamp PTP packet snooping for version2 format enable */
#define ETH_PTPTSSR_TSSSR       ((uint32_t)0x00000200U)  /* Time stamp Sub-seconds rollover */
#define ETH_PTPTSSR_TSSARFE     ((uint32_t)0x00000100U)  /* Time stamp snapshot for all received frames enable */

#define ETH_PTPTSCR_TSARU    ((uint32_t)0x00000020U)  /* Addend register update */
#define ETH_PTPTSCR_TSITE    ((uint32_t)0x00000010U)  /* Time stamp interrupt trigger enable */
#define ETH_PTPTSCR_TSSTU    ((uint32_t)0x00000008U)  /* Time stamp update */
#define ETH_PTPTSCR_TSSTI    ((uint32_t)0x00000004U)  /* Time stamp initialize */
#define ETH_PTPTSCR_TSFCU    ((uint32_t)0x00000002U)  /* Time stamp fine or coarse update */
#define ETH_PTPTSCR_TSE      ((uint32_t)0x00000001U)  /* Time stamp enable */

/* Bit definition for Ethernet PTP Sub-Second Increment Register */
#define ETH_PTPSSIR_STSSI    ((uint32_t)0x000000FFU)  /* System time Sub-second increment value */

/* Bit definition for Ethernet PTP Time Stamp High Register */
#define ETH_PTPTSHR_STS      ((uint32_t)0xFFFFFFFFU)  /* System Time second */

/* Bit definition for Ethernet PTP Time Stamp Low Register */
#define ETH_PTPTSLR_STPNS    ((uint32_t)0x80000000U)  /* System Time Positive or negative time */
#define ETH_PTPTSLR_STSS     ((uint32_t)0x7FFFFFFFU)  /* System Time sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp High Update Register */
#define ETH_PTPTSHUR_TSUS    ((uint32_t)0xFFFFFFFFU)  /* Time stamp update seconds */

/* Bit definition for Ethernet PTP Time Stamp Low Update Register */
#define ETH_PTPTSLUR_TSUPNS  ((uint32_t)0x80000000U)  /* Time stamp update Positive or negative time */
#define ETH_PTPTSLUR_TSUSS   ((uint32_t)0x7FFFFFFFU)  /* Time stamp update sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp Addend Register */
#define ETH_PTPTSAR_TSA      ((uint32_t)0xFFFFFFFFU)  /* Time stamp addend */

/* Bit definition for Ethernet PTP Target Time High Register */
#define ETH_PTPTTHR_TTSH     ((uint32_t)0xFFFFFFFFU)  /* Target time stamp high */

/* Bit definition for Ethernet PTP Target Time Low Register */
#define ETH_PTPTTLR_TTSL     ((uint32_t)0xFFFFFFFFU)  /* Target time stamp low */

/* Bit definition for Ethernet PTP Time Stamp Status Register */
#define ETH_PTPTSSR_TSTTR    ((uint32_t)0x00000020U)  /* Time stamp target time reached */
#define ETH_PTPTSSR_TSSO     ((uint32_t)0x00000010U)  /* Time stamp seconds overflow */

/******************************************************************************/
/*                 Ethernet DMA Registers bits definition                     */
/******************************************************************************/

/* Bit definition for Ethernet DMA Bus Mode Register */
#define ETH_DMABMR_AAB       ((uint32_t)0x02000000U)  /* Address-Aligned beats */
#define ETH_DMABMR_FPM        ((uint32_t)0x01000000U)  /* 4xPBL mode */
#define ETH_DMABMR_USP       ((uint32_t)0x00800000U)  /* Use separate PBL */
#define ETH_DMABMR_RDP       ((uint32_t)0x007E0000U)  /* RxDMA PBL */
  #define ETH_DMABMR_RDP_1Beat    ((uint32_t)0x00020000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
  #define ETH_DMABMR_RDP_2Beat    ((uint32_t)0x00040000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
  #define ETH_DMABMR_RDP_4Beat    ((uint32_t)0x00080000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_8Beat    ((uint32_t)0x00100000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_16Beat   ((uint32_t)0x00200000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_32Beat   ((uint32_t)0x00400000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_4Beat   ((uint32_t)0x01020000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_4xPBL_8Beat   ((uint32_t)0x01040000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_4xPBL_16Beat  ((uint32_t)0x01080000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_4xPBL_32Beat  ((uint32_t)0x01100000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_64Beat  ((uint32_t)0x01200000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
  #define ETH_DMABMR_RDP_4xPBL_128Beat ((uint32_t)0x01400000U)  /* maximum number of beats to be transferred in one RxDMA transaction is 128 */
#define ETH_DMABMR_FB        ((uint32_t)0x00010000U)  /* Fixed Burst */
#define ETH_DMABMR_RTPR      ((uint32_t)0x0000C000U)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_1_1     ((uint32_t)0x00000000U)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_2_1     ((uint32_t)0x00004000U)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_3_1     ((uint32_t)0x00008000U)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_4_1     ((uint32_t)0x0000C000U)  /* Rx Tx priority ratio */
#define ETH_DMABMR_PBL    ((uint32_t)0x00003F00U)  /* Programmable burst length */
  #define ETH_DMABMR_PBL_1Beat    ((uint32_t)0x00000100U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
  #define ETH_DMABMR_PBL_2Beat    ((uint32_t)0x00000200U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
  #define ETH_DMABMR_PBL_4Beat    ((uint32_t)0x00000400U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_8Beat    ((uint32_t)0x00000800U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_16Beat   ((uint32_t)0x00001000U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_32Beat   ((uint32_t)0x00002000U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_4Beat   ((uint32_t)0x01000100U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_4xPBL_8Beat   ((uint32_t)0x01000200U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_4xPBL_16Beat  ((uint32_t)0x01000400U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_4xPBL_32Beat  ((uint32_t)0x01000800U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_64Beat  ((uint32_t)0x01001000U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
  #define ETH_DMABMR_PBL_4xPBL_128Beat ((uint32_t)0x01002000U)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_EDE       ((uint32_t)0x00000080U)  /* Enhanced Descriptor Enable */
#define ETH_DMABMR_DSL       ((uint32_t)0x0000007CU)  /* Descriptor Skip Length */
#define ETH_DMABMR_DA        ((uint32_t)0x00000002U)  /* DMA arbitration scheme */
#define ETH_DMABMR_SR        ((uint32_t)0x00000001U)  /* Software reset */

/* Bit definition for Ethernet DMA Transmit Poll Demand Register */
#define ETH_DMATPDR_TPD      ((uint32_t)0xFFFFFFFFU)  /* Transmit poll demand */

/* Bit definition for Ethernet DMA Receive Poll Demand Register */
#define ETH_DMARPDR_RPD      ((uint32_t)0xFFFFFFFFU)  /* Receive poll demand  */

/* Bit definition for Ethernet DMA Receive Descriptor List Address Register */
#define ETH_DMARDLAR_SRL     ((uint32_t)0xFFFFFFFFU)  /* Start of receive list */

/* Bit definition for Ethernet DMA Transmit Descriptor List Address Register */
#define ETH_DMATDLAR_STL     ((uint32_t)0xFFFFFFFFU)  /* Start of transmit list */

/* Bit definition for Ethernet DMA Status Register */
#define ETH_DMASR_TSTS       ((uint32_t)0x20000000U)  /* Time-stamp trigger status */
#define ETH_DMASR_PMTS       ((uint32_t)0x10000000U)  /* PMT status */
#define ETH_DMASR_MMCS       ((uint32_t)0x08000000U)  /* MMC status */
#define ETH_DMASR_EBS        ((uint32_t)0x03800000U)  /* Error bits status */
  /* combination with EBS[2:0] for GetFlagStatus function */
  #define ETH_DMASR_EBS_DescAccess      ((uint32_t)0x02000000U)  /* Error bits 0-data buffer, 1-desc. access */
  #define ETH_DMASR_EBS_ReadTransf      ((uint32_t)0x01000000U)  /* Error bits 0-write trnsf, 1-read transfr */
  #define ETH_DMASR_EBS_DataTransfTx    ((uint32_t)0x00800000U)  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((uint32_t)0x00700000U)  /* Transmit process state */
  #define ETH_DMASR_TPS_Stopped         ((uint32_t)0x00000000U)  /* Stopped - Reset or Stop Tx Command issued  */
  #define ETH_DMASR_TPS_Fetching        ((uint32_t)0x00100000U)  /* Running - fetching the Tx descriptor */
  #define ETH_DMASR_TPS_Waiting         ((uint32_t)0x00200000U)  /* Running - waiting for status */
  #define ETH_DMASR_TPS_Reading         ((uint32_t)0x00300000U)  /* Running - reading the data from host memory */
  #define ETH_DMASR_TPS_Suspended       ((uint32_t)0x00600000U)  /* Suspended - Tx Descriptor unavailabe */
  #define ETH_DMASR_TPS_Closing         ((uint32_t)0x00700000U)  /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS         ((uint32_t)0x000E0000U)  /* Receive process state */
  #define ETH_DMASR_RPS_Stopped         ((uint32_t)0x00000000U)  /* Stopped - Reset or Stop Rx Command issued */
  #define ETH_DMASR_RPS_Fetching        ((uint32_t)0x00020000U)  /* Running - fetching the Rx descriptor */
  #define ETH_DMASR_RPS_Waiting         ((uint32_t)0x00060000U)  /* Running - waiting for packet */
  #define ETH_DMASR_RPS_Suspended       ((uint32_t)0x00080000U)  /* Suspended - Rx Descriptor unavailable */
  #define ETH_DMASR_RPS_Closing         ((uint32_t)0x000A0000U)  /* Running - closing descriptor */
  #define ETH_DMASR_RPS_Queuing         ((uint32_t)0x000E0000U)  /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS        ((uint32_t)0x00010000U)  /* Normal interrupt summary */
#define ETH_DMASR_AIS        ((uint32_t)0x00008000U)  /* Abnormal interrupt summary */
#define ETH_DMASR_ERS        ((uint32_t)0x00004000U)  /* Early receive status */
#define ETH_DMASR_FBES       ((uint32_t)0x00002000U)  /* Fatal bus error status */
#define ETH_DMASR_ETS        ((uint32_t)0x00000400U)  /* Early transmit status */
#define ETH_DMASR_RWTS       ((uint32_t)0x00000200U)  /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS       ((uint32_t)0x00000100U)  /* Receive process stopped status */
#define ETH_DMASR_RBUS       ((uint32_t)0x00000080U)  /* Receive buffer unavailable status */
#define ETH_DMASR_RS         ((uint32_t)0x00000040U)  /* Receive status */
#define ETH_DMASR_TUS        ((uint32_t)0x00000020U)  /* Transmit underflow status */
#define ETH_DMASR_ROS        ((uint32_t)0x00000010U)  /* Receive overflow status */
#define ETH_DMASR_TJTS       ((uint32_t)0x00000008U)  /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS       ((uint32_t)0x00000004U)  /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS       ((uint32_t)0x00000002U)  /* Transmit process stopped status */
#define ETH_DMASR_TS         ((uint32_t)0x00000001U)  /* Transmit status */

/* Bit definition for Ethernet DMA Operation Mode Register */
#define ETH_DMAOMR_DTCEFD    ((uint32_t)0x04000000U)  /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF       ((uint32_t)0x02000000U)  /* Receive store and forward */
#define ETH_DMAOMR_DFRF      ((uint32_t)0x01000000U)  /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF       ((uint32_t)0x00200000U)  /* Transmit store and forward */
#define ETH_DMAOMR_FTF       ((uint32_t)0x00100000U)  /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC       ((uint32_t)0x0001C000U)  /* Transmit threshold control */
  #define ETH_DMAOMR_TTC_64Bytes       ((uint32_t)0x00000000U)  /* threshold level of the MTL Transmit FIFO is 64 Bytes */
  #define ETH_DMAOMR_TTC_128Bytes      ((uint32_t)0x00004000U)  /* threshold level of the MTL Transmit FIFO is 128 Bytes */
  #define ETH_DMAOMR_TTC_192Bytes      ((uint32_t)0x00008000U)  /* threshold level of the MTL Transmit FIFO is 192 Bytes */
  #define ETH_DMAOMR_TTC_256Bytes      ((uint32_t)0x0000C000U)  /* threshold level of the MTL Transmit FIFO is 256 Bytes */
  #define ETH_DMAOMR_TTC_40Bytes       ((uint32_t)0x00010000U)  /* threshold level of the MTL Transmit FIFO is 40 Bytes */
  #define ETH_DMAOMR_TTC_32Bytes       ((uint32_t)0x00014000U)  /* threshold level of the MTL Transmit FIFO is 32 Bytes */
  #define ETH_DMAOMR_TTC_24Bytes       ((uint32_t)0x00018000U)  /* threshold level of the MTL Transmit FIFO is 24 Bytes */
  #define ETH_DMAOMR_TTC_16Bytes       ((uint32_t)0x0001C000U)  /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST        ((uint32_t)0x00002000U)  /* Start/stop transmission command */
#define ETH_DMAOMR_FEF       ((uint32_t)0x00000080U)  /* Forward error frames */
#define ETH_DMAOMR_FUGF      ((uint32_t)0x00000040U)  /* Forward undersized good frames */
#define ETH_DMAOMR_RTC       ((uint32_t)0x00000018U)  /* receive threshold control */
  #define ETH_DMAOMR_RTC_64Bytes       ((uint32_t)0x00000000U)  /* threshold level of the MTL Receive FIFO is 64 Bytes */
  #define ETH_DMAOMR_RTC_32Bytes       ((uint32_t)0x00000008U)  /* threshold level of the MTL Receive FIFO is 32 Bytes */
  #define ETH_DMAOMR_RTC_96Bytes       ((uint32_t)0x00000010U)  /* threshold level of the MTL Receive FIFO is 96 Bytes */
  #define ETH_DMAOMR_RTC_128Bytes      ((uint32_t)0x00000018U)  /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF       ((uint32_t)0x00000004U)  /* operate on second frame */
#define ETH_DMAOMR_SR        ((uint32_t)0x00000002U)  /* Start/stop receive */

/* Bit definition for Ethernet DMA Interrupt Enable Register */
#define ETH_DMAIER_NISE      ((uint32_t)0x00010000U)  /* Normal interrupt summary enable */
#define ETH_DMAIER_AISE      ((uint32_t)0x00008000U)  /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE      ((uint32_t)0x00004000U)  /* Early receive interrupt enable */
#define ETH_DMAIER_FBEIE     ((uint32_t)0x00002000U)  /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE      ((uint32_t)0x00000400U)  /* Early transmit interrupt enable */
#define ETH_DMAIER_RWTIE     ((uint32_t)0x00000200U)  /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RPSIE     ((uint32_t)0x00000100U)  /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RBUIE     ((uint32_t)0x00000080U)  /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE       ((uint32_t)0x00000040U)  /* Receive interrupt enable */
#define ETH_DMAIER_TUIE      ((uint32_t)0x00000020U)  /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE      ((uint32_t)0x00000010U)  /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJTIE     ((uint32_t)0x00000008U)  /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE     ((uint32_t)0x00000004U)  /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TPSIE     ((uint32_t)0x00000002U)  /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE       ((uint32_t)0x00000001U)  /* Transmit interrupt enable */

/* Bit definition for Ethernet DMA Missed Frame and Buffer Overflow Counter Register */
#define ETH_DMAMFBOCR_OFOC   ((uint32_t)0x10000000U)  /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA    ((uint32_t)0x0FFE0000U)  /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC   ((uint32_t)0x00010000U)  /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC    ((uint32_t)0x0000FFFFU)  /* Number of frames missed by the controller */

/* Bit definition for Ethernet DMA Current Host Transmit Descriptor Register */
#define ETH_DMACHTDR_HTDAP   ((uint32_t)0xFFFFFFFFU)  /* Host transmit descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Descriptor Register */
#define ETH_DMACHRDR_HRDAP   ((uint32_t)0xFFFFFFFFU)  /* Host receive descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Transmit Buffer Address Register */
#define ETH_DMACHTBAR_HTBAP  ((uint32_t)0xFFFFFFFFU)  /* Host transmit buffer address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Buffer Address Register */
#define ETH_DMACHRBAR_HRBAP  ((uint32_t)0xFFFFFFFFU)  /* Host receive buffer address pointer */

/******************************************************************************/
/*                                                                            */
/*                                       USB_OTG			                        */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition forUSB_OTG_GOTGCTL register  ********************/
#define USB_OTG_GOTGCTL_SRQSCS                  ((uint32_t)0x00000001U)            /*!< Session request success  */
#define USB_OTG_GOTGCTL_SRQ                     ((uint32_t)0x00000002U)            /*!< Session request          */
#define USB_OTG_GOTGCTL_HNGSCS                  ((uint32_t)0x00000100U)            /*!< Host negotiation success */
#define USB_OTG_GOTGCTL_HNPRQ                   ((uint32_t)0x00000200U)            /*!< HNP request              */
#define USB_OTG_GOTGCTL_HSHNPEN                 ((uint32_t)0x00000400U)            /*!< Host set HNP enable      */
#define USB_OTG_GOTGCTL_DHNPEN                  ((uint32_t)0x00000800U)            /*!< Device HNP enabled       */
#define USB_OTG_GOTGCTL_CIDSTS                  ((uint32_t)0x00010000U)            /*!< Connector ID status      */
#define USB_OTG_GOTGCTL_DBCT                    ((uint32_t)0x00020000U)            /*!< Long/short debounce time */
#define USB_OTG_GOTGCTL_ASVLD                   ((uint32_t)0x00040000U)            /*!< A-session valid          */
#define USB_OTG_GOTGCTL_BSVLD                   ((uint32_t)0x00080000U)            /*!< B-session valid          */

/********************  Bit definition forUSB_OTG_HCFG register  ********************/

#define USB_OTG_HCFG_FSLSPCS                 ((uint32_t)0x00000003U)            /*!< FS/LS PHY clock select  */
#define USB_OTG_HCFG_FSLSPCS_0               ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_HCFG_FSLSPCS_1               ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_HCFG_FSLSS                   ((uint32_t)0x00000004U)            /*!< FS- and LS-only support */

/********************  Bit definition forUSB_OTG_DCFG register  ********************/

#define USB_OTG_DCFG_DSPD                    ((uint32_t)0x00000003U)            /*!< Device speed */
#define USB_OTG_DCFG_DSPD_0                  ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_DCFG_DSPD_1                  ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_DCFG_NZLSOHSK                ((uint32_t)0x00000004U)            /*!< Nonzero-length status OUT handshake */

#define USB_OTG_DCFG_DAD                     ((uint32_t)0x000007F0U)            /*!< Device address */
#define USB_OTG_DCFG_DAD_0                   ((uint32_t)0x00000010U)            /*!<Bit 0 */
#define USB_OTG_DCFG_DAD_1                   ((uint32_t)0x00000020U)            /*!<Bit 1 */
#define USB_OTG_DCFG_DAD_2                   ((uint32_t)0x00000040U)            /*!<Bit 2 */
#define USB_OTG_DCFG_DAD_3                   ((uint32_t)0x00000080U)            /*!<Bit 3 */
#define USB_OTG_DCFG_DAD_4                   ((uint32_t)0x00000100U)            /*!<Bit 4 */
#define USB_OTG_DCFG_DAD_5                   ((uint32_t)0x00000200U)            /*!<Bit 5 */
#define USB_OTG_DCFG_DAD_6                   ((uint32_t)0x00000400U)            /*!<Bit 6 */

#define USB_OTG_DCFG_PFIVL                   ((uint32_t)0x00001800U)            /*!< Periodic (micro)frame interval */
#define USB_OTG_DCFG_PFIVL_0                 ((uint32_t)0x00000800U)            /*!<Bit 0 */
#define USB_OTG_DCFG_PFIVL_1                 ((uint32_t)0x00001000U)            /*!<Bit 1 */

#define USB_OTG_DCFG_PERSCHIVL               ((uint32_t)0x03000000U)            /*!< Periodic scheduling interval */
#define USB_OTG_DCFG_PERSCHIVL_0             ((uint32_t)0x01000000U)            /*!<Bit 0 */
#define USB_OTG_DCFG_PERSCHIVL_1             ((uint32_t)0x02000000U)            /*!<Bit 1 */

/********************  Bit definition forUSB_OTG_PCGCR register  ********************/
#define USB_OTG_PCGCR_STPPCLK                 ((uint32_t)0x00000001U)            /*!< Stop PHY clock */
#define USB_OTG_PCGCR_GATEHCLK                ((uint32_t)0x00000002U)            /*!< Gate HCLK */
#define USB_OTG_PCGCR_PHYSUSP                 ((uint32_t)0x00000010U)            /*!< PHY suspended */

/********************  Bit definition forUSB_OTG_GOTGINT register  ********************/
#define USB_OTG_GOTGINT_SEDET                   ((uint32_t)0x00000004U)            /*!< Session end detected                   */
#define USB_OTG_GOTGINT_SRSSCHG                 ((uint32_t)0x00000100U)            /*!< Session request success status change  */
#define USB_OTG_GOTGINT_HNSSCHG                 ((uint32_t)0x00000200U)            /*!< Host negotiation success status change */
#define USB_OTG_GOTGINT_HNGDET                  ((uint32_t)0x00020000U)            /*!< Host negotiation detected              */
#define USB_OTG_GOTGINT_ADTOCHG                 ((uint32_t)0x00040000U)            /*!< A-device timeout change                */
#define USB_OTG_GOTGINT_DBCDNE                  ((uint32_t)0x00080000U)            /*!< Debounce done                          */

/********************  Bit definition forUSB_OTG_DCTL register  ********************/
#define USB_OTG_DCTL_RWUSIG                  ((uint32_t)0x00000001U)            /*!< Remote wakeup signaling */
#define USB_OTG_DCTL_SDIS                    ((uint32_t)0x00000002U)            /*!< Soft disconnect         */
#define USB_OTG_DCTL_GINSTS                  ((uint32_t)0x00000004U)            /*!< Global IN NAK status    */
#define USB_OTG_DCTL_GONSTS                  ((uint32_t)0x00000008U)            /*!< Global OUT NAK status   */

#define USB_OTG_DCTL_TCTL                    ((uint32_t)0x00000070U)            /*!< Test control */
#define USB_OTG_DCTL_TCTL_0                  ((uint32_t)0x00000010U)            /*!<Bit 0 */
#define USB_OTG_DCTL_TCTL_1                  ((uint32_t)0x00000020U)            /*!<Bit 1 */
#define USB_OTG_DCTL_TCTL_2                  ((uint32_t)0x00000040U)            /*!<Bit 2 */
#define USB_OTG_DCTL_SGINAK                  ((uint32_t)0x00000080U)            /*!< Set global IN NAK         */
#define USB_OTG_DCTL_CGINAK                  ((uint32_t)0x00000100U)            /*!< Clear global IN NAK       */
#define USB_OTG_DCTL_SGONAK                  ((uint32_t)0x00000200U)            /*!< Set global OUT NAK        */
#define USB_OTG_DCTL_CGONAK                  ((uint32_t)0x00000400U)            /*!< Clear global OUT NAK      */
#define USB_OTG_DCTL_POPRGDNE                ((uint32_t)0x00000800U)            /*!< Power-on programming done */

/********************  Bit definition forUSB_OTG_HFIR register  ********************/
#define USB_OTG_HFIR_FRIVL                   ((uint32_t)0x0000FFFFU)            /*!< Frame interval */

/********************  Bit definition forUSB_OTG_HFNUM register  ********************/
#define USB_OTG_HFNUM_FRNUM                   ((uint32_t)0x0000FFFFU)            /*!< Frame number         */
#define USB_OTG_HFNUM_FTREM                   ((uint32_t)0xFFFF0000U)            /*!< Frame time remaining */

/********************  Bit definition forUSB_OTG_DSTS register  ********************/
#define USB_OTG_DSTS_SUSPSTS                 ((uint32_t)0x00000001U)            /*!< Suspend status   */

#define USB_OTG_DSTS_ENUMSPD                 ((uint32_t)0x00000006U)            /*!< Enumerated speed */
#define USB_OTG_DSTS_ENUMSPD_0               ((uint32_t)0x00000002U)            /*!<Bit 0 */
#define USB_OTG_DSTS_ENUMSPD_1               ((uint32_t)0x00000004U)            /*!<Bit 1 */
#define USB_OTG_DSTS_EERR                    ((uint32_t)0x00000008U)            /*!< Erratic error     */
#define USB_OTG_DSTS_FNSOF                   ((uint32_t)0x003FFF00U)            /*!< Frame number of the received SOF */

/********************  Bit definition forUSB_OTG_GAHBCFG register  ********************/
#define USB_OTG_GAHBCFG_GINT                    ((uint32_t)0x00000001U)            /*!< Global interrupt mask */

#define USB_OTG_GAHBCFG_HBSTLEN                 ((uint32_t)0x0000001EU)            /*!< Burst length/type */
#define USB_OTG_GAHBCFG_HBSTLEN_0               ((uint32_t)0x00000002U)            /*!<Bit 0 */
#define USB_OTG_GAHBCFG_HBSTLEN_1               ((uint32_t)0x00000004U)            /*!<Bit 1 */
#define USB_OTG_GAHBCFG_HBSTLEN_2               ((uint32_t)0x00000008U)            /*!<Bit 2 */
#define USB_OTG_GAHBCFG_HBSTLEN_3               ((uint32_t)0x00000010U)            /*!<Bit 3 */
#define USB_OTG_GAHBCFG_DMAEN                   ((uint32_t)0x00000020U)            /*!< DMA enable */
#define USB_OTG_GAHBCFG_TXFELVL                 ((uint32_t)0x00000080U)            /*!< TxFIFO empty level */
#define USB_OTG_GAHBCFG_PTXFELVL                ((uint32_t)0x00000100U)            /*!< Periodic TxFIFO empty level */

/********************  Bit definition forUSB_OTG_GUSBCFG register  ********************/

#define USB_OTG_GUSBCFG_TOCAL                   ((uint32_t)0x00000007U)            /*!< FS timeout calibration */
#define USB_OTG_GUSBCFG_TOCAL_0                 ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TOCAL_1                 ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TOCAL_2                 ((uint32_t)0x00000004U)            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_PHYSEL                  ((uint32_t)0x00000040U)            /*!< USB 2.0 high-speed ULPI PHY or USB 1.1 full-speed serial transceiver select */
#define USB_OTG_GUSBCFG_SRPCAP                  ((uint32_t)0x00000100U)            /*!< SRP-capable */
#define USB_OTG_GUSBCFG_HNPCAP                  ((uint32_t)0x00000200U)            /*!< HNP-capable */

#define USB_OTG_GUSBCFG_TRDT                    ((uint32_t)0x00003C00U)            /*!< USB turnaround time */
#define USB_OTG_GUSBCFG_TRDT_0                  ((uint32_t)0x00000400U)            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TRDT_1                  ((uint32_t)0x00000800U)            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TRDT_2                  ((uint32_t)0x00001000U)            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_TRDT_3                  ((uint32_t)0x00002000U)            /*!<Bit 3 */
#define USB_OTG_GUSBCFG_PHYLPCS                 ((uint32_t)0x00008000U)            /*!< PHY Low-power clock select */
#define USB_OTG_GUSBCFG_ULPIFSLS                ((uint32_t)0x00020000U)            /*!< ULPI FS/LS select               */
#define USB_OTG_GUSBCFG_ULPIAR                  ((uint32_t)0x00040000U)            /*!< ULPI Auto-resume                */
#define USB_OTG_GUSBCFG_ULPICSM                 ((uint32_t)0x00080000U)            /*!< ULPI Clock SuspendM             */
#define USB_OTG_GUSBCFG_ULPIEVBUSD              ((uint32_t)0x00100000U)            /*!< ULPI External VBUS Drive        */
#define USB_OTG_GUSBCFG_ULPIEVBUSI              ((uint32_t)0x00200000U)            /*!< ULPI external VBUS indicator    */
#define USB_OTG_GUSBCFG_TSDPS                   ((uint32_t)0x00400000U)            /*!< TermSel DLine pulsing selection */
#define USB_OTG_GUSBCFG_PCCI                    ((uint32_t)0x00800000U)            /*!< Indicator complement            */
#define USB_OTG_GUSBCFG_PTCI                    ((uint32_t)0x01000000U)            /*!< Indicator pass through          */
#define USB_OTG_GUSBCFG_ULPIIPD                 ((uint32_t)0x02000000U)            /*!< ULPI interface protect disable  */
#define USB_OTG_GUSBCFG_FHMOD                   ((uint32_t)0x20000000U)            /*!< Forced host mode                */
#define USB_OTG_GUSBCFG_FDMOD                   ((uint32_t)0x40000000U)            /*!< Forced peripheral mode          */
#define USB_OTG_GUSBCFG_CTXPKT                  ((uint32_t)0x80000000U)            /*!< Corrupt Tx packet               */

/********************  Bit definition forUSB_OTG_GRSTCTL register  ********************/
#define USB_OTG_GRSTCTL_CSRST                   ((uint32_t)0x00000001U)            /*!< Core soft reset          */
#define USB_OTG_GRSTCTL_HSRST                   ((uint32_t)0x00000002U)            /*!< HCLK soft reset          */
#define USB_OTG_GRSTCTL_FCRST                   ((uint32_t)0x00000004U)            /*!< Host frame counter reset */
#define USB_OTG_GRSTCTL_RXFFLSH                 ((uint32_t)0x00000010U)            /*!< RxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFFLSH                 ((uint32_t)0x00000020U)            /*!< TxFIFO flush             */

#define USB_OTG_GRSTCTL_TXFNUM                  ((uint32_t)0x000007C0U)            /*!< TxFIFO number */
#define USB_OTG_GRSTCTL_TXFNUM_0                ((uint32_t)0x00000040U)            /*!<Bit 0 */
#define USB_OTG_GRSTCTL_TXFNUM_1                ((uint32_t)0x00000080U)            /*!<Bit 1 */
#define USB_OTG_GRSTCTL_TXFNUM_2                ((uint32_t)0x00000100U)            /*!<Bit 2 */
#define USB_OTG_GRSTCTL_TXFNUM_3                ((uint32_t)0x00000200U)            /*!<Bit 3 */
#define USB_OTG_GRSTCTL_TXFNUM_4                ((uint32_t)0x00000400U)            /*!<Bit 4 */
#define USB_OTG_GRSTCTL_DMAREQ                  ((uint32_t)0x40000000U)            /*!< DMA request signal */
#define USB_OTG_GRSTCTL_AHBIDL                  ((uint32_t)0x80000000U)            /*!< AHB master idle */

/********************  Bit definition forUSB_OTG_DIEPMSK register  ********************/
#define USB_OTG_DIEPMSK_XFRCM                   ((uint32_t)0x00000001U)            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPMSK_EPDM                    ((uint32_t)0x00000002U)            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPMSK_TOM                     ((uint32_t)0x00000008U)            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPMSK_ITTXFEMSK               ((uint32_t)0x00000010U)            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPMSK_INEPNMM                 ((uint32_t)0x00000020U)            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPMSK_INEPNEM                 ((uint32_t)0x00000040U)            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPMSK_TXFURM                  ((uint32_t)0x00000100U)            /*!< FIFO underrun mask                                */
#define USB_OTG_DIEPMSK_BIM                     ((uint32_t)0x00000200U)            /*!< BNA interrupt mask                                */

/********************  Bit definition forUSB_OTG_HPTXSTS register  ********************/
#define USB_OTG_HPTXSTS_PTXFSAVL                ((uint32_t)0x0000FFFFU)            /*!< Periodic transmit data FIFO space available     */

#define USB_OTG_HPTXSTS_PTXQSAV                 ((uint32_t)0x00FF0000U)            /*!< Periodic transmit request queue space available */
#define USB_OTG_HPTXSTS_PTXQSAV_0               ((uint32_t)0x00010000U)            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQSAV_1               ((uint32_t)0x00020000U)            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQSAV_2               ((uint32_t)0x00040000U)            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQSAV_3               ((uint32_t)0x00080000U)            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQSAV_4               ((uint32_t)0x00100000U)            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQSAV_5               ((uint32_t)0x00200000U)            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQSAV_6               ((uint32_t)0x00400000U)            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQSAV_7               ((uint32_t)0x00800000U)            /*!<Bit 7 */

#define USB_OTG_HPTXSTS_PTXQTOP                 ((uint32_t)0xFF000000U)            /*!< Top of the periodic transmit request queue */
#define USB_OTG_HPTXSTS_PTXQTOP_0               ((uint32_t)0x01000000U)            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQTOP_1               ((uint32_t)0x02000000U)            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQTOP_2               ((uint32_t)0x04000000U)            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQTOP_3               ((uint32_t)0x08000000U)            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQTOP_4               ((uint32_t)0x10000000U)            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQTOP_5               ((uint32_t)0x20000000U)            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQTOP_6               ((uint32_t)0x40000000U)            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQTOP_7               ((uint32_t)0x80000000U)            /*!<Bit 7 */

/********************  Bit definition forUSB_OTG_HAINT register  ********************/
#define USB_OTG_HAINT_HAINT                   ((uint32_t)0x0000FFFFU)            /*!< Channel interrupts */

/********************  Bit definition forUSB_OTG_DOEPMSK register  ********************/
#define USB_OTG_DOEPMSK_XFRCM                   ((uint32_t)0x00000001U)            /*!< Transfer completed interrupt mask */
#define USB_OTG_DOEPMSK_EPDM                    ((uint32_t)0x00000002U)            /*!< Endpoint disabled interrupt mask               */
#define USB_OTG_DOEPMSK_STUPM                   ((uint32_t)0x00000008U)            /*!< SETUP phase done mask                          */
#define USB_OTG_DOEPMSK_OTEPDM                  ((uint32_t)0x00000010U)            /*!< OUT token received when endpoint disabled mask */
#define USB_OTG_DOEPMSK_B2BSTUP                 ((uint32_t)0x00000040U)            /*!< Back-to-back SETUP packets received mask       */
#define USB_OTG_DOEPMSK_OPEM                    ((uint32_t)0x00000100U)            /*!< OUT packet error mask                          */
#define USB_OTG_DOEPMSK_BOIM                    ((uint32_t)0x00000200U)            /*!< BNA interrupt mask                             */

/********************  Bit definition forUSB_OTG_GINTSTS register  ********************/
#define USB_OTG_GINTSTS_CMOD                    ((uint32_t)0x00000001U)            /*!< Current mode of operation           */
#define USB_OTG_GINTSTS_MMIS                    ((uint32_t)0x00000002U)            /*!< Mode mismatch interrupt             */
#define USB_OTG_GINTSTS_OTGINT                  ((uint32_t)0x00000004U)            /*!< OTG interrupt                       */
#define USB_OTG_GINTSTS_SOF                     ((uint32_t)0x00000008U)            /*!< Start of frame                      */
#define USB_OTG_GINTSTS_RXFLVL                  ((uint32_t)0x00000010U)            /*!< RxFIFO nonempty                     */
#define USB_OTG_GINTSTS_NPTXFE                  ((uint32_t)0x00000020U)            /*!< Nonperiodic TxFIFO empty            */
#define USB_OTG_GINTSTS_GINAKEFF                ((uint32_t)0x00000040U)            /*!< Global IN nonperiodic NAK effective */
#define USB_OTG_GINTSTS_BOUTNAKEFF              ((uint32_t)0x00000080U)            /*!< Global OUT NAK effective            */
#define USB_OTG_GINTSTS_ESUSP                   ((uint32_t)0x00000400U)            /*!< Early suspend                            */
#define USB_OTG_GINTSTS_USBSUSP                 ((uint32_t)0x00000800U)            /*!< USB suspend                              */
#define USB_OTG_GINTSTS_USBRST                  ((uint32_t)0x00001000U)            /*!< USB reset                                */
#define USB_OTG_GINTSTS_ENUMDNE                 ((uint32_t)0x00002000U)            /*!< Enumeration done                         */
#define USB_OTG_GINTSTS_ISOODRP                 ((uint32_t)0x00004000U)            /*!< Isochronous OUT packet dropped interrupt */
#define USB_OTG_GINTSTS_EOPF                    ((uint32_t)0x00008000U)            /*!< End of periodic frame interrupt          */
#define USB_OTG_GINTSTS_IEPINT                  ((uint32_t)0x00040000U)            /*!< IN endpoint interrupt                          */
#define USB_OTG_GINTSTS_OEPINT                  ((uint32_t)0x00080000U)            /*!< OUT endpoint interrupt                         */
#define USB_OTG_GINTSTS_IISOIXFR                ((uint32_t)0x00100000U)            /*!< Incomplete isochronous IN transfer             */
#define USB_OTG_GINTSTS_PXFR_INCOMPISOOUT       ((uint32_t)0x00200000U)            /*!< Incomplete periodic transfer                   */
#define USB_OTG_GINTSTS_DATAFSUSP               ((uint32_t)0x00400000U)            /*!< Data fetch suspended                           */
#define USB_OTG_GINTSTS_HPRTINT                 ((uint32_t)0x01000000U)            /*!< Host port interrupt                            */
#define USB_OTG_GINTSTS_HCINT                   ((uint32_t)0x02000000U)            /*!< Host channels interrupt                        */
#define USB_OTG_GINTSTS_PTXFE                   ((uint32_t)0x04000000U)            /*!< Periodic TxFIFO empty                          */
#define USB_OTG_GINTSTS_CIDSCHG                 ((uint32_t)0x10000000U)            /*!< Connector ID status change                     */
#define USB_OTG_GINTSTS_DISCINT                 ((uint32_t)0x20000000U)            /*!< Disconnect detected interrupt                  */
#define USB_OTG_GINTSTS_SRQINT                  ((uint32_t)0x40000000U)            /*!< Session request/new session detected interrupt */
#define USB_OTG_GINTSTS_WKUINT                  ((uint32_t)0x80000000U)            /*!< Resume/remote wakeup detected interrupt        */

/********************  Bit definition forUSB_OTG_GINTMSK register  ********************/
#define USB_OTG_GINTMSK_MMISM                   ((uint32_t)0x00000002U)            /*!< Mode mismatch interrupt mask                        */
#define USB_OTG_GINTMSK_OTGINT                  ((uint32_t)0x00000004U)            /*!< OTG interrupt mask                                  */
#define USB_OTG_GINTMSK_SOFM                    ((uint32_t)0x00000008U)            /*!< Start of frame mask                                 */
#define USB_OTG_GINTMSK_RXFLVLM                 ((uint32_t)0x00000010U)            /*!< Receive FIFO nonempty mask                          */
#define USB_OTG_GINTMSK_NPTXFEM                 ((uint32_t)0x00000020U)            /*!< Nonperiodic TxFIFO empty mask                       */
#define USB_OTG_GINTMSK_GINAKEFFM               ((uint32_t)0x00000040U)            /*!< Global nonperiodic IN NAK effective mask            */
#define USB_OTG_GINTMSK_GONAKEFFM               ((uint32_t)0x00000080U)            /*!< Global OUT NAK effective mask                       */
#define USB_OTG_GINTMSK_ESUSPM                  ((uint32_t)0x00000400U)            /*!< Early suspend mask                                  */
#define USB_OTG_GINTMSK_USBSUSPM                ((uint32_t)0x00000800U)            /*!< USB suspend mask                                    */
#define USB_OTG_GINTMSK_USBRST                  ((uint32_t)0x00001000U)            /*!< USB reset mask                                      */
#define USB_OTG_GINTMSK_ENUMDNEM                ((uint32_t)0x00002000U)            /*!< Enumeration done mask                               */
#define USB_OTG_GINTMSK_ISOODRPM                ((uint32_t)0x00004000U)            /*!< Isochronous OUT packet dropped interrupt mask       */
#define USB_OTG_GINTMSK_EOPFM                   ((uint32_t)0x00008000U)            /*!< End of periodic frame interrupt mask                */
#define USB_OTG_GINTMSK_EPMISM                  ((uint32_t)0x00020000U)            /*!< Endpoint mismatch interrupt mask                    */
#define USB_OTG_GINTMSK_IEPINT                  ((uint32_t)0x00040000U)            /*!< IN endpoints interrupt mask                         */
#define USB_OTG_GINTMSK_OEPINT                  ((uint32_t)0x00080000U)            /*!< OUT endpoints interrupt mask                        */
#define USB_OTG_GINTMSK_IISOIXFRM               ((uint32_t)0x00100000U)            /*!< Incomplete isochronous IN transfer mask             */
#define USB_OTG_GINTMSK_PXFRM_IISOOXFRM         ((uint32_t)0x00200000U)            /*!< Incomplete periodic transfer mask                   */
#define USB_OTG_GINTMSK_FSUSPM                  ((uint32_t)0x00400000U)            /*!< Data fetch suspended mask                           */
#define USB_OTG_GINTMSK_PRTIM                   ((uint32_t)0x01000000U)            /*!< Host port interrupt mask                            */
#define USB_OTG_GINTMSK_HCIM                    ((uint32_t)0x02000000U)            /*!< Host channels interrupt mask                        */
#define USB_OTG_GINTMSK_PTXFEM                  ((uint32_t)0x04000000U)            /*!< Periodic TxFIFO empty mask                          */
#define USB_OTG_GINTMSK_CIDSCHGM                ((uint32_t)0x10000000U)            /*!< Connector ID status change mask                     */
#define USB_OTG_GINTMSK_DISCINT                 ((uint32_t)0x20000000U)            /*!< Disconnect detected interrupt mask                  */
#define USB_OTG_GINTMSK_SRQIM                   ((uint32_t)0x40000000U)            /*!< Session request/new session detected interrupt mask */
#define USB_OTG_GINTMSK_WUIM                    ((uint32_t)0x80000000U)            /*!< Resume/remote wakeup detected interrupt mask        */

/********************  Bit definition forUSB_OTG_DAINT register  ********************/
#define USB_OTG_DAINT_IEPINT                  ((uint32_t)0x0000FFFFU)            /*!< IN endpoint interrupt bits  */
#define USB_OTG_DAINT_OEPINT                  ((uint32_t)0xFFFF0000U)            /*!< OUT endpoint interrupt bits */

/********************  Bit definition forUSB_OTG_HAINTMSK register  ********************/
#define USB_OTG_HAINTMSK_HAINTM                  ((uint32_t)0x0000FFFFU)            /*!< Channel interrupt mask */

/********************  Bit definition for USB_OTG_GRXSTSP register  ********************/
#define USB_OTG_GRXSTSP_EPNUM                    ((uint32_t)0x0000000FU)            /*!< IN EP interrupt mask bits  */
#define USB_OTG_GRXSTSP_BCNT                     ((uint32_t)0x00007FF0U)            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_DPID                     ((uint32_t)0x00018000U)            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_PKTSTS                   ((uint32_t)0x001E0000U)            /*!< OUT EP interrupt mask bits */

/********************  Bit definition forUSB_OTG_DAINTMSK register  ********************/
#define USB_OTG_DAINTMSK_IEPM                    ((uint32_t)0x0000FFFFU)            /*!< IN EP interrupt mask bits */
#define USB_OTG_DAINTMSK_OEPM                    ((uint32_t)0xFFFF0000U)            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for OTG register  ********************/

#define USB_OTG_CHNUM                   ((uint32_t)0x0000000FU)            /*!< Channel number */
#define USB_OTG_CHNUM_0                 ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_CHNUM_1                 ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_CHNUM_2                 ((uint32_t)0x00000004U)            /*!<Bit 2 */
#define USB_OTG_CHNUM_3                 ((uint32_t)0x00000008U)            /*!<Bit 3 */
#define USB_OTG_BCNT                    ((uint32_t)0x00007FF0U)            /*!< Byte count */

#define USB_OTG_DPID                    ((uint32_t)0x00018000U)            /*!< Data PID */
#define USB_OTG_DPID_0                  ((uint32_t)0x00008000U)            /*!<Bit 0 */
#define USB_OTG_DPID_1                  ((uint32_t)0x00010000U)            /*!<Bit 1 */

#define USB_OTG_PKTSTS                  ((uint32_t)0x001E0000U)            /*!< Packet status */
#define USB_OTG_PKTSTS_0                ((uint32_t)0x00020000U)            /*!<Bit 0 */
#define USB_OTG_PKTSTS_1                ((uint32_t)0x00040000U)            /*!<Bit 1 */
#define USB_OTG_PKTSTS_2                ((uint32_t)0x00080000U)            /*!<Bit 2 */
#define USB_OTG_PKTSTS_3                ((uint32_t)0x00100000U)            /*!<Bit 3 */

#define USB_OTG_EPNUM                   ((uint32_t)0x0000000FU)            /*!< Endpoint number */
#define USB_OTG_EPNUM_0                 ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_EPNUM_1                 ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_EPNUM_2                 ((uint32_t)0x00000004U)            /*!<Bit 2 */
#define USB_OTG_EPNUM_3                 ((uint32_t)0x00000008U)            /*!<Bit 3 */

#define USB_OTG_FRMNUM                  ((uint32_t)0x01E00000U)            /*!< Frame number */
#define USB_OTG_FRMNUM_0                ((uint32_t)0x00200000U)            /*!<Bit 0 */
#define USB_OTG_FRMNUM_1                ((uint32_t)0x00400000U)            /*!<Bit 1 */
#define USB_OTG_FRMNUM_2                ((uint32_t)0x00800000U)            /*!<Bit 2 */
#define USB_OTG_FRMNUM_3                ((uint32_t)0x01000000U)            /*!<Bit 3 */

/********************  Bit definition for OTG register  ********************/

#define USB_OTG_CHNUM                   ((uint32_t)0x0000000FU)            /*!< Channel number */
#define USB_OTG_CHNUM_0                 ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_CHNUM_1                 ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_CHNUM_2                 ((uint32_t)0x00000004U)            /*!<Bit 2 */
#define USB_OTG_CHNUM_3                 ((uint32_t)0x00000008U)            /*!<Bit 3 */
#define USB_OTG_BCNT                    ((uint32_t)0x00007FF0U)            /*!< Byte count */

#define USB_OTG_DPID                    ((uint32_t)0x00018000U)            /*!< Data PID */
#define USB_OTG_DPID_0                  ((uint32_t)0x00008000U)            /*!<Bit 0 */
#define USB_OTG_DPID_1                  ((uint32_t)0x00010000U)            /*!<Bit 1 */

#define USB_OTG_PKTSTS                  ((uint32_t)0x001E0000U)            /*!< Packet status */
#define USB_OTG_PKTSTS_0                ((uint32_t)0x00020000U)            /*!<Bit 0 */
#define USB_OTG_PKTSTS_1                ((uint32_t)0x00040000U)            /*!<Bit 1 */
#define USB_OTG_PKTSTS_2                ((uint32_t)0x00080000U)            /*!<Bit 2 */
#define USB_OTG_PKTSTS_3                ((uint32_t)0x00100000U)            /*!<Bit 3 */

#define USB_OTG_EPNUM                   ((uint32_t)0x0000000FU)            /*!< Endpoint number */
#define USB_OTG_EPNUM_0                 ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_EPNUM_1                 ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_EPNUM_2                 ((uint32_t)0x00000004U)            /*!<Bit 2 */
#define USB_OTG_EPNUM_3                 ((uint32_t)0x00000008U)            /*!<Bit 3 */

#define USB_OTG_FRMNUM                  ((uint32_t)0x01E00000U)            /*!< Frame number */
#define USB_OTG_FRMNUM_0                ((uint32_t)0x00200000U)            /*!<Bit 0 */
#define USB_OTG_FRMNUM_1                ((uint32_t)0x00400000U)            /*!<Bit 1 */
#define USB_OTG_FRMNUM_2                ((uint32_t)0x00800000U)            /*!<Bit 2 */
#define USB_OTG_FRMNUM_3                ((uint32_t)0x01000000U)            /*!<Bit 3 */

/********************  Bit definition forUSB_OTG_GRXFSIZ register  ********************/
#define USB_OTG_GRXFSIZ_RXFD            ((uint32_t)0x0000FFFFU)            /*!< RxFIFO depth */

/********************  Bit definition forUSB_OTG_DVBUSDIS register  ********************/
#define USB_OTG_DVBUSDIS_VBUSDT         ((uint32_t)0x0000FFFFU)            /*!< Device VBUS discharge time */

/********************  Bit definition for OTG register  ********************/
#define USB_OTG_NPTXFSA                 ((uint32_t)0x0000FFFFU)            /*!< Nonperiodic transmit RAM start address */
#define USB_OTG_NPTXFD                  ((uint32_t)0xFFFF0000U)            /*!< Nonperiodic TxFIFO depth               */
#define USB_OTG_TX0FSA                  ((uint32_t)0x0000FFFFU)            /*!< Endpoint 0 transmit RAM start address  */
#define USB_OTG_TX0FD                   ((uint32_t)0xFFFF0000U)            /*!< Endpoint 0 TxFIFO depth                */

/********************  Bit definition forUSB_OTG_DVBUSPULSE register  ********************/
#define USB_OTG_DVBUSPULSE_DVBUSP                  ((uint32_t)0x00000FFFU)            /*!< Device VBUS pulsing time */

/********************  Bit definition forUSB_OTG_GNPTXSTS register  ********************/
#define USB_OTG_GNPTXSTS_NPTXFSAV                ((uint32_t)0x0000FFFFU)            /*!< Nonperiodic TxFIFO space available */

#define USB_OTG_GNPTXSTS_NPTQXSAV                ((uint32_t)0x00FF0000U)            /*!< Nonperiodic transmit request queue space available */
#define USB_OTG_GNPTXSTS_NPTQXSAV_0              ((uint32_t)0x00010000U)            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_1              ((uint32_t)0x00020000U)            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_2              ((uint32_t)0x00040000U)            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_3              ((uint32_t)0x00080000U)            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_4              ((uint32_t)0x00100000U)            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_5              ((uint32_t)0x00200000U)            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_6              ((uint32_t)0x00400000U)            /*!<Bit 6 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_7              ((uint32_t)0x00800000U)            /*!<Bit 7 */

#define USB_OTG_GNPTXSTS_NPTXQTOP                ((uint32_t)0x7F000000U)            /*!< Top of the nonperiodic transmit request queue */
#define USB_OTG_GNPTXSTS_NPTXQTOP_0              ((uint32_t)0x01000000U)            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_1              ((uint32_t)0x02000000U)            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_2              ((uint32_t)0x04000000U)            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_3              ((uint32_t)0x08000000U)            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_4              ((uint32_t)0x10000000U)            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_5              ((uint32_t)0x20000000U)            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_6              ((uint32_t)0x40000000U)            /*!<Bit 6 */

/********************  Bit definition forUSB_OTG_DTHRCTL register  ********************/
#define USB_OTG_DTHRCTL_NONISOTHREN             ((uint32_t)0x00000001U)            /*!< Nonisochronous IN endpoints threshold enable */
#define USB_OTG_DTHRCTL_ISOTHREN                ((uint32_t)0x00000002U)            /*!< ISO IN endpoint threshold enable */

#define USB_OTG_DTHRCTL_TXTHRLEN                ((uint32_t)0x000007FCU)            /*!< Transmit threshold length */
#define USB_OTG_DTHRCTL_TXTHRLEN_0              ((uint32_t)0x00000004U)            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_TXTHRLEN_1              ((uint32_t)0x00000008U)            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_TXTHRLEN_2              ((uint32_t)0x00000010U)            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_TXTHRLEN_3              ((uint32_t)0x00000020U)            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_TXTHRLEN_4              ((uint32_t)0x00000040U)            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_TXTHRLEN_5              ((uint32_t)0x00000080U)            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_TXTHRLEN_6              ((uint32_t)0x00000100U)            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_TXTHRLEN_7              ((uint32_t)0x00000200U)            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_TXTHRLEN_8              ((uint32_t)0x00000400U)            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_RXTHREN                 ((uint32_t)0x00010000U)            /*!< Receive threshold enable */

#define USB_OTG_DTHRCTL_RXTHRLEN                ((uint32_t)0x03FE0000U)            /*!< Receive threshold length */
#define USB_OTG_DTHRCTL_RXTHRLEN_0              ((uint32_t)0x00020000U)            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_RXTHRLEN_1              ((uint32_t)0x00040000U)            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_RXTHRLEN_2              ((uint32_t)0x00080000U)            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_RXTHRLEN_3              ((uint32_t)0x00100000U)            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_RXTHRLEN_4              ((uint32_t)0x00200000U)            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_RXTHRLEN_5              ((uint32_t)0x00400000U)            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_RXTHRLEN_6              ((uint32_t)0x00800000U)            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_RXTHRLEN_7              ((uint32_t)0x01000000U)            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_RXTHRLEN_8              ((uint32_t)0x02000000U)            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_ARPEN                   ((uint32_t)0x08000000U)            /*!< Arbiter parking enable */

/********************  Bit definition forUSB_OTG_DIEPEMPMSK register  ********************/
#define USB_OTG_DIEPEMPMSK_INEPTXFEM               ((uint32_t)0x0000FFFFU)            /*!< IN EP Tx FIFO empty interrupt mask bits */

/********************  Bit definition forUSB_OTG_DEACHINT register  ********************/
#define USB_OTG_DEACHINT_IEP1INT                 ((uint32_t)0x00000002U)            /*!< IN endpoint 1interrupt bit   */
#define USB_OTG_DEACHINT_OEP1INT                 ((uint32_t)0x00020000U)            /*!< OUT endpoint 1 interrupt bit */

/********************  Bit definition forUSB_OTG_GCCFG register  ********************/
#define USB_OTG_GCCFG_PWRDWN                  ((uint32_t)0x00010000U)            /*!< Power down */
#define USB_OTG_GCCFG_I2CPADEN                ((uint32_t)0x00020000U)            /*!< Enable I2C bus connection for the external I2C PHY interface */
#define USB_OTG_GCCFG_VBUSASEN                ((uint32_t)0x00040000U)            /*!< Enable the VBUS sensing device                               */
#define USB_OTG_GCCFG_VBUSBSEN                ((uint32_t)0x00080000U)            /*!< Enable the VBUS sensing device                               */
#define USB_OTG_GCCFG_SOFOUTEN                ((uint32_t)0x00100000U)            /*!< SOF output enable                                            */
#define USB_OTG_GCCFG_NOVBUSSENS              ((uint32_t)0x00200000U)            /*!< VBUS sensing disable option                                  */

/********************  Bit definition forUSB_OTG_DEACHINTMSK register  ********************/
#define USB_OTG_DEACHINTMSK_IEP1INTM                ((uint32_t)0x00000002U)            /*!< IN Endpoint 1 interrupt mask bit  */
#define USB_OTG_DEACHINTMSK_OEP1INTM                ((uint32_t)0x00020000U)            /*!< OUT Endpoint 1 interrupt mask bit */

/********************  Bit definition forUSB_OTG_CID register  ********************/
#define USB_OTG_CID_PRODUCT_ID              ((uint32_t)0xFFFFFFFFU)            /*!< Product ID field */

/********************  Bit definition forUSB_OTG_DIEPEACHMSK1 register  ********************/
#define USB_OTG_DIEPEACHMSK1_XFRCM                   ((uint32_t)0x00000001U)            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPEACHMSK1_EPDM                    ((uint32_t)0x00000002U)            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPEACHMSK1_TOM                     ((uint32_t)0x00000008U)            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPEACHMSK1_ITTXFEMSK               ((uint32_t)0x00000010U)            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPEACHMSK1_INEPNMM                 ((uint32_t)0x00000020U)            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPEACHMSK1_INEPNEM                 ((uint32_t)0x00000040U)            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPEACHMSK1_TXFURM                  ((uint32_t)0x00000100U)            /*!< FIFO underrun mask */
#define USB_OTG_DIEPEACHMSK1_BIM                     ((uint32_t)0x00000200U)            /*!< BNA interrupt mask */
#define USB_OTG_DIEPEACHMSK1_NAKM                    ((uint32_t)0x00002000U)            /*!< NAK interrupt mask */

/********************  Bit definition forUSB_OTG_HPRT register  ********************/
#define USB_OTG_HPRT_PCSTS                   ((uint32_t)0x00000001U)            /*!< Port connect status        */
#define USB_OTG_HPRT_PCDET                   ((uint32_t)0x00000002U)            /*!< Port connect detected      */
#define USB_OTG_HPRT_PENA                    ((uint32_t)0x00000004U)            /*!< Port enable                */
#define USB_OTG_HPRT_PENCHNG                 ((uint32_t)0x00000008U)            /*!< Port enable/disable change */
#define USB_OTG_HPRT_POCA                    ((uint32_t)0x00000010U)            /*!< Port overcurrent active    */
#define USB_OTG_HPRT_POCCHNG                 ((uint32_t)0x00000020U)            /*!< Port overcurrent change    */
#define USB_OTG_HPRT_PRES                    ((uint32_t)0x00000040U)            /*!< Port resume   */
#define USB_OTG_HPRT_PSUSP                   ((uint32_t)0x00000080U)            /*!< Port suspend  */
#define USB_OTG_HPRT_PRST                    ((uint32_t)0x00000100U)            /*!< Port reset    */

#define USB_OTG_HPRT_PLSTS                   ((uint32_t)0x00000C00U)            /*!< Port line status */
#define USB_OTG_HPRT_PLSTS_0                 ((uint32_t)0x00000400U)            /*!<Bit 0 */
#define USB_OTG_HPRT_PLSTS_1                 ((uint32_t)0x00000800U)            /*!<Bit 1 */
#define USB_OTG_HPRT_PPWR                    ((uint32_t)0x00001000U)            /*!< Port power */

#define USB_OTG_HPRT_PTCTL                   ((uint32_t)0x0001E000U)            /*!< Port test control */
#define USB_OTG_HPRT_PTCTL_0                 ((uint32_t)0x00002000U)            /*!<Bit 0 */
#define USB_OTG_HPRT_PTCTL_1                 ((uint32_t)0x00004000U)            /*!<Bit 1 */
#define USB_OTG_HPRT_PTCTL_2                 ((uint32_t)0x00008000U)            /*!<Bit 2 */
#define USB_OTG_HPRT_PTCTL_3                 ((uint32_t)0x00010000U)            /*!<Bit 3 */

#define USB_OTG_HPRT_PSPD                    ((uint32_t)0x00060000U)            /*!< Port speed */
#define USB_OTG_HPRT_PSPD_0                  ((uint32_t)0x00020000U)            /*!<Bit 0 */
#define USB_OTG_HPRT_PSPD_1                  ((uint32_t)0x00040000U)            /*!<Bit 1 */

/********************  Bit definition forUSB_OTG_DOEPEACHMSK1 register  ********************/
#define USB_OTG_DOEPEACHMSK1_XFRCM                   ((uint32_t)0x00000001U)            /*!< Transfer completed interrupt mask */
#define USB_OTG_DOEPEACHMSK1_EPDM                    ((uint32_t)0x00000002U)            /*!< Endpoint disabled interrupt mask */
#define USB_OTG_DOEPEACHMSK1_TOM                     ((uint32_t)0x00000008U)            /*!< Timeout condition mask */
#define USB_OTG_DOEPEACHMSK1_ITTXFEMSK               ((uint32_t)0x00000010U)            /*!< IN token received when TxFIFO empty mask */
#define USB_OTG_DOEPEACHMSK1_INEPNMM                 ((uint32_t)0x00000020U)            /*!< IN token received with EP mismatch mask */
#define USB_OTG_DOEPEACHMSK1_INEPNEM                 ((uint32_t)0x00000040U)            /*!< IN endpoint NAK effective mask */
#define USB_OTG_DOEPEACHMSK1_TXFURM                  ((uint32_t)0x00000100U)            /*!< OUT packet error mask */
#define USB_OTG_DOEPEACHMSK1_BIM                     ((uint32_t)0x00000200U)            /*!< BNA interrupt mask */
#define USB_OTG_DOEPEACHMSK1_BERRM                   ((uint32_t)0x00001000U)            /*!< Bubble error interrupt mask */
#define USB_OTG_DOEPEACHMSK1_NAKM                    ((uint32_t)0x00002000U)            /*!< NAK interrupt mask */
#define USB_OTG_DOEPEACHMSK1_NYETM                   ((uint32_t)0x00004000U)            /*!< NYET interrupt mask */

/********************  Bit definition forUSB_OTG_HPTXFSIZ register  ********************/
#define USB_OTG_HPTXFSIZ_PTXSA                   ((uint32_t)0x0000FFFFU)            /*!< Host periodic TxFIFO start address */
#define USB_OTG_HPTXFSIZ_PTXFD                   ((uint32_t)0xFFFF0000U)            /*!< Host periodic TxFIFO depth */

/********************  Bit definition forUSB_OTG_DIEPCTL register  ********************/
#define USB_OTG_DIEPCTL_MPSIZ                   ((uint32_t)0x000007FFU)            /*!< Maximum packet size */
#define USB_OTG_DIEPCTL_USBAEP                  ((uint32_t)0x00008000U)            /*!< USB active endpoint */
#define USB_OTG_DIEPCTL_EONUM_DPID              ((uint32_t)0x00010000U)            /*!< Even/odd frame */
#define USB_OTG_DIEPCTL_NAKSTS                  ((uint32_t)0x00020000U)            /*!< NAK status */

#define USB_OTG_DIEPCTL_EPTYP                   ((uint32_t)0x000C0000U)            /*!< Endpoint type */
#define USB_OTG_DIEPCTL_EPTYP_0                 ((uint32_t)0x00040000U)            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_EPTYP_1                 ((uint32_t)0x00080000U)            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_STALL                   ((uint32_t)0x00200000U)            /*!< STALL handshake */

#define USB_OTG_DIEPCTL_TXFNUM                  ((uint32_t)0x03C00000U)            /*!< TxFIFO number */
#define USB_OTG_DIEPCTL_TXFNUM_0                ((uint32_t)0x00400000U)            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_TXFNUM_1                ((uint32_t)0x00800000U)            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_TXFNUM_2                ((uint32_t)0x01000000U)            /*!<Bit 2 */
#define USB_OTG_DIEPCTL_TXFNUM_3                ((uint32_t)0x02000000U)            /*!<Bit 3 */
#define USB_OTG_DIEPCTL_CNAK                    ((uint32_t)0x04000000U)            /*!< Clear NAK */
#define USB_OTG_DIEPCTL_SNAK                    ((uint32_t)0x08000000U)            /*!< Set NAK */
#define USB_OTG_DIEPCTL_SD0PID_SEVNFRM          ((uint32_t)0x10000000U)            /*!< Set DATA0 PID */
#define USB_OTG_DIEPCTL_SODDFRM                 ((uint32_t)0x20000000U)            /*!< Set odd frame */
#define USB_OTG_DIEPCTL_EPDIS                   ((uint32_t)0x40000000U)            /*!< Endpoint disable */
#define USB_OTG_DIEPCTL_EPENA                   ((uint32_t)0x80000000U)            /*!< Endpoint enable */

/********************  Bit definition forUSB_OTG_HCCHAR register  ********************/
#define USB_OTG_HCCHAR_MPSIZ                   ((uint32_t)0x000007FFU)            /*!< Maximum packet size */

#define USB_OTG_HCCHAR_EPNUM                   ((uint32_t)0x00007800U)            /*!< Endpoint number */
#define USB_OTG_HCCHAR_EPNUM_0                 ((uint32_t)0x00000800U)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPNUM_1                 ((uint32_t)0x00001000U)            /*!<Bit 1 */
#define USB_OTG_HCCHAR_EPNUM_2                 ((uint32_t)0x00002000U)            /*!<Bit 2 */
#define USB_OTG_HCCHAR_EPNUM_3                 ((uint32_t)0x00004000U)            /*!<Bit 3 */
#define USB_OTG_HCCHAR_EPDIR                   ((uint32_t)0x00008000U)            /*!< Endpoint direction */
#define USB_OTG_HCCHAR_LSDEV                   ((uint32_t)0x00020000U)            /*!< Low-speed device */

#define USB_OTG_HCCHAR_EPTYP                   ((uint32_t)0x000C0000U)            /*!< Endpoint type */
#define USB_OTG_HCCHAR_EPTYP_0                 ((uint32_t)0x00040000U)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPTYP_1                 ((uint32_t)0x00080000U)            /*!<Bit 1 */

#define USB_OTG_HCCHAR_MC                      ((uint32_t)0x00300000U)            /*!< Multi Count (MC) / Error Count (EC) */
#define USB_OTG_HCCHAR_MC_0                    ((uint32_t)0x00100000U)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_MC_1                    ((uint32_t)0x00200000U)            /*!<Bit 1 */

#define USB_OTG_HCCHAR_DAD                     ((uint32_t)0x1FC00000U)            /*!< Device address */
#define USB_OTG_HCCHAR_DAD_0                   ((uint32_t)0x00400000U)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_DAD_1                   ((uint32_t)0x00800000U)            /*!<Bit 1 */
#define USB_OTG_HCCHAR_DAD_2                   ((uint32_t)0x01000000U)            /*!<Bit 2 */
#define USB_OTG_HCCHAR_DAD_3                   ((uint32_t)0x02000000U)            /*!<Bit 3 */
#define USB_OTG_HCCHAR_DAD_4                   ((uint32_t)0x04000000U)            /*!<Bit 4 */
#define USB_OTG_HCCHAR_DAD_5                   ((uint32_t)0x08000000U)            /*!<Bit 5 */
#define USB_OTG_HCCHAR_DAD_6                   ((uint32_t)0x10000000U)            /*!<Bit 6 */
#define USB_OTG_HCCHAR_ODDFRM                  ((uint32_t)0x20000000U)            /*!< Odd frame */
#define USB_OTG_HCCHAR_CHDIS                   ((uint32_t)0x40000000U)            /*!< Channel disable */
#define USB_OTG_HCCHAR_CHENA                   ((uint32_t)0x80000000U)            /*!< Channel enable */

/********************  Bit definition forUSB_OTG_HCSPLT register  ********************/

#define USB_OTG_HCSPLT_PRTADDR                 ((uint32_t)0x0000007FU)            /*!< Port address */
#define USB_OTG_HCSPLT_PRTADDR_0               ((uint32_t)0x00000001U)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_PRTADDR_1               ((uint32_t)0x00000002U)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_PRTADDR_2               ((uint32_t)0x00000004U)            /*!<Bit 2 */
#define USB_OTG_HCSPLT_PRTADDR_3               ((uint32_t)0x00000008U)            /*!<Bit 3 */
#define USB_OTG_HCSPLT_PRTADDR_4               ((uint32_t)0x00000010U)            /*!<Bit 4 */
#define USB_OTG_HCSPLT_PRTADDR_5               ((uint32_t)0x00000020U)            /*!<Bit 5 */
#define USB_OTG_HCSPLT_PRTADDR_6               ((uint32_t)0x00000040U)            /*!<Bit 6 */

#define USB_OTG_HCSPLT_HUBADDR                 ((uint32_t)0x00003F80U)            /*!< Hub address */
#define USB_OTG_HCSPLT_HUBADDR_0               ((uint32_t)0x00000080U)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_HUBADDR_1               ((uint32_t)0x00000100U)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_HUBADDR_2               ((uint32_t)0x00000200U)            /*!<Bit 2 */
#define USB_OTG_HCSPLT_HUBADDR_3               ((uint32_t)0x00000400U)            /*!<Bit 3 */
#define USB_OTG_HCSPLT_HUBADDR_4               ((uint32_t)0x00000800U)            /*!<Bit 4 */
#define USB_OTG_HCSPLT_HUBADDR_5               ((uint32_t)0x00001000U)            /*!<Bit 5 */
#define USB_OTG_HCSPLT_HUBADDR_6               ((uint32_t)0x00002000U)            /*!<Bit 6 */

#define USB_OTG_HCSPLT_XACTPOS                 ((uint32_t)0x0000C000U)            /*!< XACTPOS */
#define USB_OTG_HCSPLT_XACTPOS_0               ((uint32_t)0x00004000U)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_XACTPOS_1               ((uint32_t)0x00008000U)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_COMPLSPLT               ((uint32_t)0x00010000U)            /*!< Do complete split */
#define USB_OTG_HCSPLT_SPLITEN                 ((uint32_t)0x80000000U)            /*!< Split enable */

/********************  Bit definition forUSB_OTG_HCINT register  ********************/
#define USB_OTG_HCINT_XFRC                    ((uint32_t)0x00000001U)            /*!< Transfer completed */
#define USB_OTG_HCINT_CHH                     ((uint32_t)0x00000002U)            /*!< Channel halted */
#define USB_OTG_HCINT_AHBERR                  ((uint32_t)0x00000004U)            /*!< AHB error */
#define USB_OTG_HCINT_STALL                   ((uint32_t)0x00000008U)            /*!< STALL response received interrupt */
#define USB_OTG_HCINT_NAK                     ((uint32_t)0x00000010U)            /*!< NAK response received interrupt */
#define USB_OTG_HCINT_ACK                     ((uint32_t)0x00000020U)            /*!< ACK response received/transmitted interrupt */
#define USB_OTG_HCINT_NYET                    ((uint32_t)0x00000040U)            /*!< Response received interrupt */
#define USB_OTG_HCINT_TXERR                   ((uint32_t)0x00000080U)            /*!< Transaction error */
#define USB_OTG_HCINT_BBERR                   ((uint32_t)0x00000100U)            /*!< Babble error */
#define USB_OTG_HCINT_FRMOR                   ((uint32_t)0x00000200U)            /*!< Frame overrun */
#define USB_OTG_HCINT_DTERR                   ((uint32_t)0x00000400U)            /*!< Data toggle error */

/********************  Bit definition forUSB_OTG_DIEPINT register  ********************/
#define USB_OTG_DIEPINT_XFRC                    ((uint32_t)0x00000001U)            /*!< Transfer completed interrupt */
#define USB_OTG_DIEPINT_EPDISD                  ((uint32_t)0x00000002U)            /*!< Endpoint disabled interrupt */
#define USB_OTG_DIEPINT_TOC                     ((uint32_t)0x00000008U)            /*!< Timeout condition */
#define USB_OTG_DIEPINT_ITTXFE                  ((uint32_t)0x00000010U)            /*!< IN token received when TxFIFO is empty */
#define USB_OTG_DIEPINT_INEPNE                  ((uint32_t)0x00000040U)            /*!< IN endpoint NAK effective */
#define USB_OTG_DIEPINT_TXFE                    ((uint32_t)0x00000080U)            /*!< Transmit FIFO empty */
#define USB_OTG_DIEPINT_TXFIFOUDRN              ((uint32_t)0x00000100U)            /*!< Transmit Fifo Underrun */
#define USB_OTG_DIEPINT_BNA                     ((uint32_t)0x00000200U)            /*!< Buffer not available interrupt */
#define USB_OTG_DIEPINT_PKTDRPSTS               ((uint32_t)0x00000800U)            /*!< Packet dropped status */
#define USB_OTG_DIEPINT_BERR                    ((uint32_t)0x00001000U)            /*!< Babble error interrupt */
#define USB_OTG_DIEPINT_NAK                     ((uint32_t)0x00002000U)            /*!< NAK interrupt */

/********************  Bit definition forUSB_OTG_HCINTMSK register  ********************/
#define USB_OTG_HCINTMSK_XFRCM                   ((uint32_t)0x00000001U)            /*!< Transfer completed mask */
#define USB_OTG_HCINTMSK_CHHM                    ((uint32_t)0x00000002U)            /*!< Channel halted mask */
#define USB_OTG_HCINTMSK_AHBERR                  ((uint32_t)0x00000004U)            /*!< AHB error */
#define USB_OTG_HCINTMSK_STALLM                  ((uint32_t)0x00000008U)            /*!< STALL response received interrupt mask */
#define USB_OTG_HCINTMSK_NAKM                    ((uint32_t)0x00000010U)            /*!< NAK response received interrupt mask */
#define USB_OTG_HCINTMSK_ACKM                    ((uint32_t)0x00000020U)            /*!< ACK response received/transmitted interrupt mask */
#define USB_OTG_HCINTMSK_NYET                    ((uint32_t)0x00000040U)            /*!< response received interrupt mask */
#define USB_OTG_HCINTMSK_TXERRM                  ((uint32_t)0x00000080U)            /*!< Transaction error mask */
#define USB_OTG_HCINTMSK_BBERRM                  ((uint32_t)0x00000100U)            /*!< Babble error mask */
#define USB_OTG_HCINTMSK_FRMORM                  ((uint32_t)0x00000200U)            /*!< Frame overrun mask */
#define USB_OTG_HCINTMSK_DTERRM                  ((uint32_t)0x00000400U)            /*!< Data toggle error mask */

/********************  Bit definition for USB_OTG_DIEPTSIZ register  ********************/

#define USB_OTG_DIEPTSIZ_XFRSIZ                  ((uint32_t)0x0007FFFFU)            /*!< Transfer size */
#define USB_OTG_DIEPTSIZ_PKTCNT                  ((uint32_t)0x1FF80000U)            /*!< Packet count */
#define USB_OTG_DIEPTSIZ_MULCNT                  ((uint32_t)0x60000000U)            /*!< Packet count */
/********************  Bit definition forUSB_OTG_HCTSIZ register  ********************/
#define USB_OTG_HCTSIZ_XFRSIZ                    ((uint32_t)0x0007FFFFU)            /*!< Transfer size */
#define USB_OTG_HCTSIZ_PKTCNT                    ((uint32_t)0x1FF80000U)            /*!< Packet count */
#define USB_OTG_HCTSIZ_DOPING                    ((uint32_t)0x80000000U)            /*!< Do PING */
#define USB_OTG_HCTSIZ_DPID                      ((uint32_t)0x60000000U)            /*!< Data PID */
#define USB_OTG_HCTSIZ_DPID_0                    ((uint32_t)0x20000000U)            /*!<Bit 0 */
#define USB_OTG_HCTSIZ_DPID_1                    ((uint32_t)0x40000000U)            /*!<Bit 1 */

/********************  Bit definition forUSB_OTG_DIEPDMA register  ********************/
#define USB_OTG_DIEPDMA_DMAADDR                  ((uint32_t)0xFFFFFFFFU)            /*!< DMA address */

/********************  Bit definition forUSB_OTG_HCDMA register  ********************/
#define USB_OTG_HCDMA_DMAADDR                    ((uint32_t)0xFFFFFFFFU)            /*!< DMA address */

/********************  Bit definition forUSB_OTG_DTXFSTS register  ********************/
#define USB_OTG_DTXFSTS_INEPTFSAV                ((uint32_t)0x0000FFFFU)            /*!< IN endpoint TxFIFO space available */

/********************  Bit definition forUSB_OTG_DIEPTXF register  ********************/
#define USB_OTG_DIEPTXF_INEPTXSA                 ((uint32_t)0x0000FFFFU)            /*!< IN endpoint FIFOx transmit RAM start address */
#define USB_OTG_DIEPTXF_INEPTXFD                 ((uint32_t)0xFFFF0000U)            /*!< IN endpoint TxFIFO depth */

/********************  Bit definition forUSB_OTG_DOEPCTL register  ********************/

#define USB_OTG_DOEPCTL_MPSIZ                     ((uint32_t)0x000007FFU)            /*!< Maximum packet size */          /*!<Bit 1 */
#define USB_OTG_DOEPCTL_USBAEP                    ((uint32_t)0x00008000U)            /*!< USB active endpoint */
#define USB_OTG_DOEPCTL_NAKSTS                    ((uint32_t)0x00020000U)            /*!< NAK status */
#define USB_OTG_DOEPCTL_SD0PID_SEVNFRM            ((uint32_t)0x10000000U)            /*!< Set DATA0 PID */
#define USB_OTG_DOEPCTL_SODDFRM                   ((uint32_t)0x20000000U)            /*!< Set odd frame */
#define USB_OTG_DOEPCTL_EPTYP                     ((uint32_t)0x000C0000U)            /*!< Endpoint type */
#define USB_OTG_DOEPCTL_EPTYP_0                   ((uint32_t)0x00040000U)            /*!<Bit 0 */
#define USB_OTG_DOEPCTL_EPTYP_1                   ((uint32_t)0x00080000U)            /*!<Bit 1 */
#define USB_OTG_DOEPCTL_SNPM                      ((uint32_t)0x00100000U)            /*!< Snoop mode */
#define USB_OTG_DOEPCTL_STALL                     ((uint32_t)0x00200000U)            /*!< STALL handshake */
#define USB_OTG_DOEPCTL_CNAK                      ((uint32_t)0x04000000U)            /*!< Clear NAK */
#define USB_OTG_DOEPCTL_SNAK                      ((uint32_t)0x08000000U)            /*!< Set NAK */
#define USB_OTG_DOEPCTL_EPDIS                     ((uint32_t)0x40000000U)            /*!< Endpoint disable */
#define USB_OTG_DOEPCTL_EPENA                     ((uint32_t)0x80000000U)            /*!< Endpoint enable */

/********************  Bit definition forUSB_OTG_DOEPINT register  ********************/
#define USB_OTG_DOEPINT_XFRC                    ((uint32_t)0x00000001U)            /*!< Transfer completed interrupt */
#define USB_OTG_DOEPINT_EPDISD                  ((uint32_t)0x00000002U)            /*!< Endpoint disabled interrupt */
#define USB_OTG_DOEPINT_STUP                    ((uint32_t)0x00000008U)            /*!< SETUP phase done */
#define USB_OTG_DOEPINT_OTEPDIS                 ((uint32_t)0x00000010U)            /*!< OUT token received when endpoint disabled */
#define USB_OTG_DOEPINT_B2BSTUP                 ((uint32_t)0x00000040U)            /*!< Back-to-back SETUP packets received */
#define USB_OTG_DOEPINT_NYET                    ((uint32_t)0x00004000U)            /*!< NYET interrupt */

/********************  Bit definition forUSB_OTG_DOEPTSIZ register  ********************/

#define USB_OTG_DOEPTSIZ_XFRSIZ                  ((uint32_t)0x0007FFFFU)            /*!< Transfer size */
#define USB_OTG_DOEPTSIZ_PKTCNT                  ((uint32_t)0x1FF80000U)            /*!< Packet count */

#define USB_OTG_DOEPTSIZ_STUPCNT                 ((uint32_t)0x60000000U)            /*!< SETUP packet count */
#define USB_OTG_DOEPTSIZ_STUPCNT_0               ((uint32_t)0x20000000U)            /*!<Bit 0 */
#define USB_OTG_DOEPTSIZ_STUPCNT_1               ((uint32_t)0x40000000U)            /*!<Bit 1 */

/********************  Bit definition for PCGCCTL register  ********************/
#define USB_OTG_PCGCCTL_STOPCLK                 ((uint32_t)0x00000001U)            /*!< SETUP packet count */
#define USB_OTG_PCGCCTL_GATECLK                 ((uint32_t)0x00000002U)            /*!<Bit 0 */
#define USB_OTG_PCGCCTL_PHYSUSP                 ((uint32_t)0x00000010U)            /*!<Bit 1 */


/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup Exported_macros
  * @{
  */

/******************************* ADC Instances ********************************/
#define IS_ADC_ALL_INSTANCE(INSTANCE) (((INSTANCE) == ADC1) || \
                                       ((INSTANCE) == ADC2) || \
                                       ((INSTANCE) == ADC3))

/******************************* CAN Instances ********************************/
#define IS_CAN_ALL_INSTANCE(INSTANCE) (((INSTANCE) == CAN1) || \
                                       ((INSTANCE) == CAN2))

/******************************* CRC Instances ********************************/
#define IS_CRC_ALL_INSTANCE(INSTANCE) ((INSTANCE) == CRC)

/******************************* DAC Instances ********************************/
#define IS_DAC_ALL_INSTANCE(INSTANCE) ((INSTANCE) == DAC)

/******************************* DCMI Instances *******************************/
#define IS_DCMI_ALL_INSTANCE(INSTANCE) ((INSTANCE) == DCMI)

/******************************* DMA2D Instances *******************************/
#define IS_DMA2D_ALL_INSTANCE(INSTANCE) ((INSTANCE) == DMA2D)

/******************************** DMA Instances *******************************/
#define IS_DMA_STREAM_ALL_INSTANCE(INSTANCE) (((INSTANCE) == DMA1_Stream0) || \
                                              ((INSTANCE) == DMA1_Stream1) || \
                                              ((INSTANCE) == DMA1_Stream2) || \
                                              ((INSTANCE) == DMA1_Stream3) || \
                                              ((INSTANCE) == DMA1_Stream4) || \
                                              ((INSTANCE) == DMA1_Stream5) || \
                                              ((INSTANCE) == DMA1_Stream6) || \
                                              ((INSTANCE) == DMA1_Stream7) || \
                                              ((INSTANCE) == DMA2_Stream0) || \
                                              ((INSTANCE) == DMA2_Stream1) || \
                                              ((INSTANCE) == DMA2_Stream2) || \
                                              ((INSTANCE) == DMA2_Stream3) || \
                                              ((INSTANCE) == DMA2_Stream4) || \
                                              ((INSTANCE) == DMA2_Stream5) || \
                                              ((INSTANCE) == DMA2_Stream6) || \
                                              ((INSTANCE) == DMA2_Stream7))

/******************************* GPIO Instances *******************************/
#define IS_GPIO_ALL_INSTANCE(INSTANCE) (((INSTANCE) == GPIOA) || \
                                        ((INSTANCE) == GPIOB) || \
                                        ((INSTANCE) == GPIOC) || \
                                        ((INSTANCE) == GPIOD) || \
                                        ((INSTANCE) == GPIOE) || \
                                        ((INSTANCE) == GPIOF) || \
                                        ((INSTANCE) == GPIOG) || \
                                        ((INSTANCE) == GPIOH) || \
                                        ((INSTANCE) == GPIOI) || \
                                        ((INSTANCE) == GPIOJ) || \
                                        ((INSTANCE) == GPIOK))

/******************************** I2C Instances *******************************/
#define IS_I2C_ALL_INSTANCE(INSTANCE) (((INSTANCE) == I2C1) || \
                                       ((INSTANCE) == I2C2) || \
                                       ((INSTANCE) == I2C3))

/******************************** I2S Instances *******************************/
#define IS_I2S_ALL_INSTANCE(INSTANCE)  (((INSTANCE) == SPI2) || \
                                        ((INSTANCE) == SPI3))

/*************************** I2S Extended Instances ***************************/
#define IS_I2S_ALL_INSTANCE_EXT(PERIPH)  (((INSTANCE) == SPI2)    || \
                                          ((INSTANCE) == SPI3)    || \
                                          ((INSTANCE) == I2S2ext) || \
                                          ((INSTANCE) == I2S3ext))

/****************************** LTDC Instances ********************************/
#define IS_LTDC_ALL_INSTANCE(INSTANCE)  ((INSTANCE) == LTDC)

/******************************* RNG Instances ********************************/
#define IS_RNG_ALL_INSTANCE(INSTANCE)  ((INSTANCE) == RNG)

/****************************** RTC Instances *********************************/
#define IS_RTC_ALL_INSTANCE(INSTANCE)  ((INSTANCE) == RTC)

/******************************* SAI Instances ********************************/
#define IS_SAI_BLOCK_PERIPH(PERIPH) (((PERIPH) == SAI1_Block_A) || \
                                     ((PERIPH) == SAI1_Block_B))

/******************************** SPI Instances *******************************/
#define IS_SPI_ALL_INSTANCE(INSTANCE) (((INSTANCE) == SPI1) || \
                                       ((INSTANCE) == SPI2) || \
                                       ((INSTANCE) == SPI3) || \
                                       ((INSTANCE) == SPI4) || \
                                       ((INSTANCE) == SPI5) || \
                                       ((INSTANCE) == SPI6))

/*************************** SPI Extended Instances ***************************/
#define IS_SPI_ALL_INSTANCE_EXT(INSTANCE) (((INSTANCE) == SPI1)    || \
                                           ((INSTANCE) == SPI2)    || \
                                           ((INSTANCE) == SPI3)    || \
                                           ((INSTANCE) == SPI4)    || \
                                           ((INSTANCE) == SPI5)    || \
                                           ((INSTANCE) == SPI6)    || \
                                           ((INSTANCE) == I2S2ext) || \
                                           ((INSTANCE) == I2S3ext))

/****************** TIM Instances : All supported instances *******************/
#define IS_TIM_INSTANCE(INSTANCE) (((INSTANCE) == TIM1)   || \
                                   ((INSTANCE) == TIM2)   || \
                                   ((INSTANCE) == TIM3)   || \
                                   ((INSTANCE) == TIM4)   || \
                                   ((INSTANCE) == TIM5)   || \
                                   ((INSTANCE) == TIM6)   || \
                                   ((INSTANCE) == TIM7)   || \
                                   ((INSTANCE) == TIM8)   || \
                                   ((INSTANCE) == TIM9)   || \
                                   ((INSTANCE) == TIM10)  || \
                                   ((INSTANCE) == TIM11)  || \
                                   ((INSTANCE) == TIM12)  || \
                                   ((INSTANCE) == TIM13)  || \
                                   ((INSTANCE) == TIM14))

/************* TIM Instances : at least 1 capture/compare channel *************/
#define IS_TIM_CC1_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1)  || \
                                         ((INSTANCE) == TIM2)  || \
                                         ((INSTANCE) == TIM3)  || \
                                         ((INSTANCE) == TIM4)  || \
                                         ((INSTANCE) == TIM5)  || \
                                         ((INSTANCE) == TIM8)  || \
                                         ((INSTANCE) == TIM9)  || \
                                         ((INSTANCE) == TIM10) || \
                                         ((INSTANCE) == TIM11) || \
                                         ((INSTANCE) == TIM12) || \
                                         ((INSTANCE) == TIM13) || \
                                         ((INSTANCE) == TIM14))

/************ TIM Instances : at least 2 capture/compare channels *************/
#define IS_TIM_CC2_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                       ((INSTANCE) == TIM2) || \
                                       ((INSTANCE) == TIM3) || \
                                       ((INSTANCE) == TIM4) || \
                                       ((INSTANCE) == TIM5) || \
                                       ((INSTANCE) == TIM8) || \
                                       ((INSTANCE) == TIM9) || \
                                       ((INSTANCE) == TIM12))

/************ TIM Instances : at least 3 capture/compare channels *************/
#define IS_TIM_CC3_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1) || \
                                         ((INSTANCE) == TIM2) || \
                                         ((INSTANCE) == TIM3) || \
                                         ((INSTANCE) == TIM4) || \
                                         ((INSTANCE) == TIM5) || \
                                         ((INSTANCE) == TIM8))

/************ TIM Instances : at least 4 capture/compare channels *************/
#define IS_TIM_CC4_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                       ((INSTANCE) == TIM2) || \
                                       ((INSTANCE) == TIM3) || \
                                       ((INSTANCE) == TIM4) || \
                                       ((INSTANCE) == TIM5) || \
                                       ((INSTANCE) == TIM8))

/******************** TIM Instances : Advanced-control timers *****************/
#define IS_TIM_ADVANCED_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                            ((INSTANCE) == TIM8))

/******************* TIM Instances : Timer input XOR function *****************/
#define IS_TIM_XOR_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1) || \
                                         ((INSTANCE) == TIM2) || \
                                         ((INSTANCE) == TIM3) || \
                                         ((INSTANCE) == TIM4) || \
                                         ((INSTANCE) == TIM5) || \
                                         ((INSTANCE) == TIM8))

/****************** TIM Instances : DMA requests generation (UDE) *************/
#define IS_TIM_DMA_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                       ((INSTANCE) == TIM2) || \
                                       ((INSTANCE) == TIM3) || \
                                       ((INSTANCE) == TIM4) || \
                                       ((INSTANCE) == TIM5) || \
                                       ((INSTANCE) == TIM6) || \
                                       ((INSTANCE) == TIM7) || \
                                       ((INSTANCE) == TIM8))

/************ TIM Instances : DMA requests generation (CCxDE) *****************/
#define IS_TIM_DMA_CC_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                          ((INSTANCE) == TIM2) || \
                                          ((INSTANCE) == TIM3) || \
                                          ((INSTANCE) == TIM4) || \
                                          ((INSTANCE) == TIM5) || \
                                          ((INSTANCE) == TIM8))

/************ TIM Instances : DMA requests generation (COMDE) *****************/
#define IS_TIM_CCDMA_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
                                          ((INSTANCE) == TIM2) || \
                                          ((INSTANCE) == TIM3) || \
                                          ((INSTANCE) == TIM4) || \
                                          ((INSTANCE) == TIM5) || \
                                          ((INSTANCE) == TIM8))

/******************** TIM Instances : DMA burst feature ***********************/
#define IS_TIM_DMABURST_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
                                             ((INSTANCE) == TIM2) || \
                                             ((INSTANCE) == TIM3) || \
                                             ((INSTANCE) == TIM4) || \
                                             ((INSTANCE) == TIM5) || \
                                             ((INSTANCE) == TIM8))

/****** TIM Instances : master mode available (TIMx_CR2.MMS available )********/
#define IS_TIM_MASTER_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                          ((INSTANCE) == TIM2) || \
                                          ((INSTANCE) == TIM3) || \
                                          ((INSTANCE) == TIM4) || \
                                          ((INSTANCE) == TIM5) || \
                                          ((INSTANCE) == TIM6) || \
                                          ((INSTANCE) == TIM7) || \
                                          ((INSTANCE) == TIM8) || \
                                          ((INSTANCE) == TIM9) || \
                                          ((INSTANCE) == TIM12))

/*********** TIM Instances : Slave mode available (TIMx_SMCR available )*******/
#define IS_TIM_SLAVE_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                         ((INSTANCE) == TIM2) || \
                                         ((INSTANCE) == TIM3) || \
                                         ((INSTANCE) == TIM4) || \
                                         ((INSTANCE) == TIM5) || \
                                         ((INSTANCE) == TIM8) || \
                                         ((INSTANCE) == TIM9) || \
                                         ((INSTANCE) == TIM12))

/********************** TIM Instances : 32 bit Counter ************************/
#define IS_TIM_32B_COUNTER_INSTANCE(INSTANCE)(((INSTANCE) == TIM2) || \
                                              ((INSTANCE) == TIM5))

/***************** TIM Instances : external trigger input availabe ************/
#define IS_TIM_ETR_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
                                        ((INSTANCE) == TIM2) || \
                                        ((INSTANCE) == TIM3) || \
                                        ((INSTANCE) == TIM4) || \
                                        ((INSTANCE) == TIM5) || \
                                        ((INSTANCE) == TIM8))

/****************** TIM Instances : remapping capability **********************/
#define IS_TIM_REMAP_INSTANCE(INSTANCE) (((INSTANCE) == TIM2)  || \
                                         ((INSTANCE) == TIM5)  || \
                                         ((INSTANCE) == TIM11))

/******************* TIM Instances : output(s) available **********************/
#define IS_TIM_CCX_INSTANCE(INSTANCE, CHANNEL) \
    ((((INSTANCE) == TIM1) &&                  \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2) ||          \
      ((CHANNEL) == TIM_CHANNEL_3) ||          \
      ((CHANNEL) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((INSTANCE) == TIM2) &&                   \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2) ||          \
      ((CHANNEL) == TIM_CHANNEL_3) ||          \
      ((CHANNEL) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((INSTANCE) == TIM3) &&                   \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2) ||          \
      ((CHANNEL) == TIM_CHANNEL_3) ||          \
      ((CHANNEL) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((INSTANCE) == TIM4) &&                   \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2) ||          \
      ((CHANNEL) == TIM_CHANNEL_3) ||          \
      ((CHANNEL) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((INSTANCE) == TIM5) &&                   \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2) ||          \
      ((CHANNEL) == TIM_CHANNEL_3) ||          \
      ((CHANNEL) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((INSTANCE) == TIM8) &&                   \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2) ||          \
      ((CHANNEL) == TIM_CHANNEL_3) ||          \
      ((CHANNEL) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((INSTANCE) == TIM9) &&                   \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2)))           \
    ||                                         \
    (((INSTANCE) == TIM10) &&                  \
     (((CHANNEL) == TIM_CHANNEL_1)))           \
    ||                                         \
    (((INSTANCE) == TIM11) &&                  \
     (((CHANNEL) == TIM_CHANNEL_1)))           \
    ||                                         \
    (((INSTANCE) == TIM12) &&                  \
     (((CHANNEL) == TIM_CHANNEL_1) ||          \
      ((CHANNEL) == TIM_CHANNEL_2)))           \
    ||                                         \
    (((INSTANCE) == TIM13) &&                  \
     (((CHANNEL) == TIM_CHANNEL_1)))           \
    ||                                         \
    (((INSTANCE) == TIM14) &&                  \
     (((CHANNEL) == TIM_CHANNEL_1))))

/************ TIM Instances : complementary output(s) available ***************/
#define IS_TIM_CCXN_INSTANCE(INSTANCE, CHANNEL) \
   ((((INSTANCE) == TIM1) &&                    \
     (((CHANNEL) == TIM_CHANNEL_1) ||           \
      ((CHANNEL) == TIM_CHANNEL_2) ||           \
      ((CHANNEL) == TIM_CHANNEL_3)))            \
    ||                                          \
    (((INSTANCE) == TIM8) &&                    \
     (((CHANNEL) == TIM_CHANNEL_1) ||           \
      ((CHANNEL) == TIM_CHANNEL_2) ||           \
      ((CHANNEL) == TIM_CHANNEL_3))))

/******************** USART Instances : Synchronous mode **********************/
#define IS_USART_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                     ((INSTANCE) == USART2) || \
                                     ((INSTANCE) == USART3) || \
                                     ((INSTANCE) == USART6))

/******************** UART Instances : Asynchronous mode **********************/
#define IS_UART_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                    ((INSTANCE) == USART2) || \
                                    ((INSTANCE) == USART3) || \
                                    ((INSTANCE) == UART4)  || \
                                    ((INSTANCE) == UART5)  || \
                                    ((INSTANCE) == USART6) || \
                                    ((INSTANCE) == UART7)  || \
                                    ((INSTANCE) == UART8))

/****************** UART Instances : Hardware Flow control ********************/
#define IS_UART_HWFLOW_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                           ((INSTANCE) == USART2) || \
                                           ((INSTANCE) == USART3) || \
                                           ((INSTANCE) == USART6))

/********************* UART Instances : Smard card mode ***********************/
#define IS_SMARTCARD_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                         ((INSTANCE) == USART2) || \
                                         ((INSTANCE) == USART3) || \
                                         ((INSTANCE) == USART6))

/*********************** UART Instances : IRDA mode ***************************/
#define IS_IRDA_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                    ((INSTANCE) == USART2) || \
                                    ((INSTANCE) == USART3) || \
                                    ((INSTANCE) == UART4)  || \
                                    ((INSTANCE) == UART5)  || \
                                    ((INSTANCE) == USART6) || \
                                    ((INSTANCE) == UART7)  || \
                                    ((INSTANCE) == UART8))

/*********************** PCD Instances ****************************************/
#define IS_PCD_ALL_INSTANCE(INSTANCE) (((INSTANCE) == USB_OTG_FS) || \
                                        ((INSTANCE) == USB_OTG_HS))

/*********************** HCD Instances ****************************************/
#define IS_HCD_ALL_INSTANCE(INSTANCE) (((INSTANCE) == USB_OTG_FS) || \
                                       ((INSTANCE) == USB_OTG_HS))


/****************************** IWDG Instances ********************************/
#define IS_IWDG_ALL_INSTANCE(INSTANCE)  ((INSTANCE) == IWDG)

/****************************** WWDG Instances ********************************/
#define IS_WWDG_ALL_INSTANCE(INSTANCE)  ((INSTANCE) == WWDG)

/****************************** SDIO Instances ********************************/
#define IS_SDIO_ALL_INSTANCE(INSTANCE) ((INSTANCE) == SDIO)

/****************************** USB Exported Constants ************************/
#define USB_OTG_FS_HOST_MAX_CHANNEL_NBR                8U
#define USB_OTG_FS_MAX_IN_ENDPOINTS                    4U    /* Including EP0 */
#define USB_OTG_FS_MAX_OUT_ENDPOINTS                   4U    /* Including EP0 */
#define USB_OTG_FS_TOTAL_FIFO_SIZE                     1280U /* in Bytes */

#define USB_OTG_HS_HOST_MAX_CHANNEL_NBR                12U
#define USB_OTG_HS_MAX_IN_ENDPOINTS                    6U    /* Including EP0 */
#define USB_OTG_HS_MAX_OUT_ENDPOINTS                   6U    /* Including EP0 */
#define USB_OTG_HS_TOTAL_FIFO_SIZE                     4096U /* in Bytes */

/******************************************************************************/
/*  For a painless codes migration between the STM32F4xx device product       */
/*  lines, the aliases defined below are put in place to overcome the         */
/*  differences in the interrupt handlers and IRQn definitions.               */
/*  No need to update developed interrupt code when moving across             */
/*  product lines within the same STM32F4 Family                              */
/******************************************************************************/

/* Aliases for __IRQn */
#define FSMC_IRQn              FMC_IRQn

/* Aliases for __IRQHandler */
#define FSMC_IRQHandler        FMC_IRQHandler

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F429xx_H */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
