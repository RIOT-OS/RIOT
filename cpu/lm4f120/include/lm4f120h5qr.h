/**************************************************************************//**
 * @file     lm4f120h5qr.h
 * @brief    LM4F120H5QR Core Peripheral Access Layer Header File
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2009 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#ifndef __LM4F120H5QR_H__
#define __LM4F120H5QR_H__

#ifdef __cplusplus
 extern "C" {
#endif

#define __CM4_REV                 0x0001  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1       /*!< LM4F120H5QR provides an MPU                     */
#define __NVIC_PRIO_BITS          3       /*!< LM4F120H5QR uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1       /*!< FPU present                                   */


/**
 * @brief LM4F120H5QR Interrupt Number Definition, according to the selected device
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
/******  LM4F specific Interrupt Numbers **********************************************************************/
  GPIOPortA_IRQn              = 0,
  GPIOPortB_IRQn              = 1,
  GPIOPortC_IRQn              = 2,
  GPIOPortD_IRQn              = 3,
  GPIOPortE_IRQn              = 4,
  UART0_IRQn                  = 5,
  UART1_IRQn                  = 6,
  SSI0_IRQn                   = 7,
  I2C0_IRQn                   = 8,
  PWMFault_IRQn               = 9,
  PWM0_IRQn                   = 10,
  PWM1_IRQn                   = 11,
  PWM2_IRQn                   = 12,
  Quadrature0_IRQn            = 13,
  ADC0_IRQn                   = 14,
  ADC1_IRQn                   = 15,
  ADC2_IRQn                   = 16,
  ADC3_IRQn                   = 17,
  WDT_IRQn                    = 18,
  Timer0A_IRQn                = 19,
  Timer0B_IRQn                = 20,
  Timer1A_IRQn                = 21,
  Timer1B_IRQn                = 22,
  Timer2A_IRQn                = 23,
  Timer2B_IRQn                = 24,
  Comp0_IRQn                  = 25,
  Comp1_IRQn                  = 26,
  Comp2_IRQn                  = 27,
  SysCtl_IRQn                 = 28,
  FlashCtl_IRQn               = 29,
  GPIOPortF_IRQn              = 30,
  GPIOPortG_IRQn              = 31,
  GPIOPortH_IRQn              = 32,
  UART2_IRQn                  = 33,
  SSI1_IRQn                   = 34,
  Timer3A_IRQn                = 35,
  Timer3B_IRQn                = 36,
  I2C1_IRQn                   = 37,
  Quadrature1_IRQn            = 38,
  CAN0_IRQn                   = 39,
  CAN1_IRQn                   = 40,
  CAN2_IRQn                   = 41,
  Ethernet_IRQn               = 42,
  Hibernate_IRQn              = 43,
  USB0_IRQn                   = 44,
  PWM3_IRQn                   = 45,
  uDMA_IRQn                   = 46,
  uDMA_Error_IRQn             = 47,
} IRQn_Type;


#include <stdint.h>               /* standard types definitions              */
#include "core_cm4.h"             /* Cortex-M4 processor and core peripherals */

#ifdef __cplusplus
}
#endif

#endif /* __LM4F120H5QR_H__*/


