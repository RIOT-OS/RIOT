/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 *
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       CC3200 MCU definitions
 *
 * @author      Attilio Dona'
 * @}
 *
 */

#ifndef CPU_INCLUDE_CC3200_H_
#define CPU_INCLUDE_CC3200_H_

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "prcm.h"
#include "rom_map.h"
#include "rom.h"
#include "pin.h"
#include "gpio.h"
#include "interrupt.h"
#include "board.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define __CM4_REV                 0x0001  /*!< Core revision r0p1                              */
#define __MPU_PRESENT             1       /*!< LM4F120H5QR provides an MPU                     */
#define __NVIC_PRIO_BITS          3       /*!< LM4F120H5QR uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used    */
#define __FPU_PRESENT             1       /*!< FPU present                                     */

#define __ASM            __asm
#define __STATIC_INLINE  static inline

#define SW_TIMERS_EXAUSTED 111

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
} IRQn_Type;


 #ifdef __cplusplus
} /* end extern "C" */
#endif

#include "core_cm4.h"

#endif /* CPU_INCLUDE_CC3200_H_ */
