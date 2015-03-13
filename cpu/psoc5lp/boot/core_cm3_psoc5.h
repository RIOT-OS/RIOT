/*******************************************************************************
* File Name: core_cm3_psoc5.h
* Version 4.20
*
*  Description:
*   Provides important type information for the PSoC5.  This includes types
*   necessary for core_cm3.h.
*
*  Note:
*   Documentation of the API's in this file is located in the
*   System Reference Guide provided with PSoC Creator.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/


#if !defined(__CORE_CM3_PSOC5_H__)
#define __CORE_CM3_PSOC5_H__

/** Interrupt Number Definition */
typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn   = -14,              /*!< 2 Non Maskable Interrupt                         */
  HardFault_IRQn        = -13,              /*!< 3 Cortex-M3 Hard Fault Interrupt                 */
  MemoryManagement_IRQn = -12,              /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn         = -11,              /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn       = -10,              /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn           = -5,               /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn     = -4,               /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn           = -2,               /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn          = -1                /*!< 15 Cortex-M3 System Tick Interrupt               */
/******  PSoC5 Peripheral Interrupt Numbers *******************************************************/
  /* Not relevant.  All peripheral interrupts are defined by the user */
} IRQn_Type;

#include <cytypes.h>

#define __CHECK_DEVICE_DEFINES

#define __CM3_REV                 0x0201

#define __MPU_PRESENT             0
#define __NVIC_PRIO_BITS          3
#define __Vendor_SysTickConfig    0

#include <core_cm3.h>


#endif /* __CORE_CM3_PSOC5_H__ */
