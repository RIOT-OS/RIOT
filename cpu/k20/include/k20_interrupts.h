/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       Interrupt configuration and definitions
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#ifndef __K20_INTERRUPTS_H
#define __K20_INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name General configuration
 */
#define __NVIC_PRIO_BITS          4       /*!< K20 uses 4 Bits for the Interrupt Priority Levels */

/**
 * @name Exception Numbers.
 *
 * These contain the exception numbers of the K20 as documented in the
 * K20 Sub-Familiy Reference Manual, Rev. 2, Feb 2012 page 63ff.
 *
 * @{
 */
typedef enum
{
/******  Cortex-M4 Processor Exceptions Numbers ****************************************************************/
/********* Name *************** IRQn ** Vector * Source Module - Description ***********************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Cortex-M4 - Non Maskable Interrupt                              */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M4 - Memory Management Interrupt                         */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M4 - Bus Fault Interrupt                                 */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M4 - Usage Fault Interrupt                               */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M4 - SV Call Interrupt                                  */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M4 - Debug Monitor Interrupt                            */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M4 - Pend SV Interrupt                                  */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M4 - System Tick Interrupt                              */
/******  K20 specific Interrupt Numbers ************************************************************************/
/********* Name *************** IRQn ** Vector * Source Module - Description ***********************************/
  DMA0_IRQn                   = 0,      /*!< 16 DMA - Channel 0 Transfer Complete                              */
  DMA1_IRQn                   = 1,      /*!< 17 DMA - Channel 1 Transfer Complete                              */
  DMA2_IRQn                   = 2,      /*!< 18 DMA - Channel 2 Transfer Complete                              */
  DMA3_IRQn                   = 3,      /*!< 19 DMA - Channel 3 Transfer Complete                              */
  DMA_Error_IRQn              = 4,      /*!< 20 DMA - Error                                                    */
  DMA_Unused_IRQn             = 5,      /*!< 21 DMA                                                            */
  Flash_Cmd_Complete_IRQn     = 6,      /*!< 22 Flash Memory - Command Complete                                */
  Flash_Read_Collision_IRQn   = 7,      /*!< 23 Flash Memory - Read Collision                                  */
  Mode_Low_Voltage_IRQn       = 8,      /*!< 24 Flash Memory - Low-Voltage Detect, Low-Voltage Warning         */
  Low_Leakage_Wakeup_IRQn     = 9,      /*!< 25 LLWU - Low Leakage Wakeup (NOTE: must not be masked)           */
  WDOG_IRQn                   = 10,     /*!< 26 WDOG - Both EWM and WDOG interrupt sources set this IRQ        */
  I2C0_IRQn                   = 11,     /*!< 27 I2C0                                                           */
  SPI0_IRQn                   = 12,     /*!< 28 SPI0 - Single interrupt vector for all sources                 */
  I2S0_IRQn                   = 13,     /*!< 29 I2S0 - Transmit                                                */
  I2S1_IRQn                   = 14,     /*!< 30 I2S1 - Receive                                                 */
  UART0_LON_Status_IRQn       = 15,     /*!< 31 UART0 - Single interrupt vector for CEA709.1-B status sources  */
  UART0_Status_IRQn           = 16,     /*!< 32 UART0 - Single interupt vector for UART0 status sources        */
  UART0_Error_IRQn            = 17,     /*!< 33 UART0 - Single interupt vector for UART0 error sources         */
  UART1_Status_IRQn           = 18,     /*!< 34 UART0 - Single interupt vector for UART1 status sources        */
  UART1_Error_IRQn            = 19,     /*!< 35 UART0 - Single interupt vector for UART1 error sources         */
  UART2_Status_IRQn           = 20,     /*!< 36 UART0 - Single interupt vector for UART1 status sources        */
  UART2_Error_IRQn            = 21,     /*!< 37 UART0 - Single interupt vector for UART1 error sources         */
  ADC0_IRQn                   = 22,     /*!< 38 ADC0                                                           */
  CMP0_IRQn                   = 23,     /*!< 39 CMP0                                                           */
  CMP1_IRQn                   = 24,     /*!< 40 CMP1                                                           */
  FTM0_IRQn                   = 25,     /*!< 41 FTM0                                                           */
  FTM1_IRQn                   = 26,     /*!< 42 FTM1                                                           */
  CMT_IRQn                    = 27,     /*!< 43 CMC                                                            */
  RTC_Alarm_IRQn              = 28,     /*!< 44 RTC - Alarm                                                    */
  RTC_Seconds_IRQn            = 29,     /*!< 45 RTC - Seconds                                                  */
  PIT_Channel_0_IRQn          = 30,     /*!< 46 PIT - Channel 0                                                */
  PIT_Channel_1_IRQn          = 31,     /*!< 47 PIT - Channel 1                                                */
  PIT_Channel_2_IRQn          = 32,     /*!< 48 PIT - Channel 2                                                */
  PIT_Channel_3_IRQn          = 33,     /*!< 49 PIT - Channel 3                                                */
  PDB_IRQn                    = 34,     /*!< 50 PDB                                                            */
  USB_OTG_IRQn                = 35,     /*!< 51 USB OTG                                                        */
  USB_Charger_Detect_IRQn     = 36,     /*!< 52 USB Charger Detect                                             */
  TSI_IRQn                    = 37,     /*!< 53 TSI                                                            */
  MCG_IRQn                    = 38,     /*!< 54 MCG                                                            */
  Low_Power_Timer_IRQn        = 39,     /*!< 55 Low Power Timer                                                */
  Port_A_IRQn                 = 40,     /*!< 56 Port control Module - Pin detect (Port A)                      */
  Port_B_IRQn                 = 41,     /*!< 57 Port control Module - Pin detect (Port B)                      */
  Port_C_IRQn                 = 42,     /*!< 58 Port control Module - Pin detect (Port C)                      */
  Port_D_IRQn                 = 43,     /*!< 59 Port control Module - Pin detect (Port D)                      */
  Port_E_IRQn                 = 44,     /*!< 60 Port control Module - Pin detect (Port E)                      */
  Software_Interrupt_IRQn     = 45      /*!< 61 Software initiated interrupt                                   */
/***************************************************************************************************************/
} IRQn_Type;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __K20_INTERRUPTS_H */
