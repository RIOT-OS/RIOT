/**
 * \file
 *
 * \brief Header file for SAML21E18B
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21E18B_
#define _SAML21E18B_

/**
 * \ingroup SAML21_definitions
 * \addtogroup SAML21E18B_definitions SAML21E18B definitions
 * This file defines all structures and symbols for SAML21E18B:
 *   - registers and bitfields
 *   - peripheral base address
 *   - peripheral ID
 *   - PIO definitions
*/
/*@{*/

#ifdef __cplusplus
 extern "C" {
#endif

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include <stdint.h>
#ifndef __cplusplus
typedef volatile const uint32_t RoReg;   /**< Read only 32-bit register (volatile const unsigned int) */
typedef volatile const uint16_t RoReg16; /**< Read only 16-bit register (volatile const unsigned int) */
typedef volatile const uint8_t  RoReg8;  /**< Read only  8-bit register (volatile const unsigned int) */
#else
typedef volatile       uint32_t RoReg;   /**< Read only 32-bit register (volatile const unsigned int) */
typedef volatile       uint16_t RoReg16; /**< Read only 16-bit register (volatile const unsigned int) */
typedef volatile       uint8_t  RoReg8;  /**< Read only  8-bit register (volatile const unsigned int) */
#endif
typedef volatile       uint32_t WoReg;   /**< Write only 32-bit register (volatile unsigned int) */
typedef volatile       uint16_t WoReg16; /**< Write only 16-bit register (volatile unsigned int) */
typedef volatile       uint8_t  WoReg8;  /**< Write only  8-bit register (volatile unsigned int) */
typedef volatile       uint32_t RwReg;   /**< Read-Write 32-bit register (volatile unsigned int) */
typedef volatile       uint16_t RwReg16; /**< Read-Write 16-bit register (volatile unsigned int) */
typedef volatile       uint8_t  RwReg8;  /**< Read-Write  8-bit register (volatile unsigned int) */
#endif

#if !defined(SKIP_INTEGER_LITERALS)
#if defined(_U_) || defined(_L_) || defined(_UL_)
  #error "Integer Literals macros already defined elsewhere"
#endif

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/* Macros that deal with adding suffixes to integer literal constants for C/C++ */
#define _U_(x)         x ## U            /**< C code: Unsigned integer literal constant value */
#define _L_(x)         x ## L            /**< C code: Long integer literal constant value */
#define _UL_(x)        x ## UL           /**< C code: Unsigned Long integer literal constant value */
#else /* Assembler */
#define _U_(x)         x                 /**< Assembler: Unsigned integer literal constant value */
#define _L_(x)         x                 /**< Assembler: Long integer literal constant value */
#define _UL_(x)        x                 /**< Assembler: Unsigned Long integer literal constant value */
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* SKIP_INTEGER_LITERALS */

/* ************************************************************************** */
/**  CMSIS DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */
/** \defgroup SAML21E18B_cmsis CMSIS Definitions */
/*@{*/

/** Interrupt Number Definition */
typedef enum IRQn
{
  /******  Cortex-M0+ Processor Exceptions Numbers *******************/
  NonMaskableInt_IRQn      = -14,/**<  2 Non Maskable Interrupt      */
  HardFault_IRQn           = -13,/**<  3 Hard Fault Interrupt        */
  SVCall_IRQn              = -5, /**< 11 SV Call Interrupt           */
  PendSV_IRQn              = -2, /**< 14 Pend SV Interrupt           */
  SysTick_IRQn             = -1, /**< 15 System Tick Interrupt       */
  /******  SAML21E18B-specific Interrupt Numbers *********************/
  SYSTEM_IRQn              =  0, /**<  0 SAML21E18B System Interrupts */
  WDT_IRQn                 =  1, /**<  1 SAML21E18B Watchdog Timer (WDT) */
  RTC_IRQn                 =  2, /**<  2 SAML21E18B Real-Time Counter (RTC) */
  EIC_IRQn                 =  3, /**<  3 SAML21E18B External Interrupt Controller (EIC) */
  NVMCTRL_IRQn             =  4, /**<  4 SAML21E18B Non-Volatile Memory Controller (NVMCTRL) */
  DMAC_IRQn                =  5, /**<  5 SAML21E18B Direct Memory Access Controller (DMAC) */
  USB_IRQn                 =  6, /**<  6 SAML21E18B Universal Serial Bus (USB) */
  EVSYS_IRQn               =  7, /**<  7 SAML21E18B Event System Interface (EVSYS) */
  SERCOM0_IRQn             =  8, /**<  8 SAML21E18B Serial Communication Interface 0 (SERCOM0) */
  SERCOM1_IRQn             =  9, /**<  9 SAML21E18B Serial Communication Interface 1 (SERCOM1) */
  SERCOM2_IRQn             = 10, /**< 10 SAML21E18B Serial Communication Interface 2 (SERCOM2) */
  SERCOM3_IRQn             = 11, /**< 11 SAML21E18B Serial Communication Interface 3 (SERCOM3) */
  SERCOM4_IRQn             = 12, /**< 12 SAML21E18B Serial Communication Interface 4 (SERCOM4) */
  SERCOM5_IRQn             = 13, /**< 13 SAML21E18B Serial Communication Interface 5 (SERCOM5) */
  TCC0_IRQn                = 14, /**< 14 SAML21E18B Timer Counter Control 0 (TCC0) */
  TCC1_IRQn                = 15, /**< 15 SAML21E18B Timer Counter Control 1 (TCC1) */
  TCC2_IRQn                = 16, /**< 16 SAML21E18B Timer Counter Control 2 (TCC2) */
  TC0_IRQn                 = 17, /**< 17 SAML21E18B Basic Timer Counter 0 (TC0) */
  TC1_IRQn                 = 18, /**< 18 SAML21E18B Basic Timer Counter 1 (TC1) */
  TC4_IRQn                 = 21, /**< 21 SAML21E18B Basic Timer Counter 4 (TC4) */
  ADC_IRQn                 = 22, /**< 22 SAML21E18B Analog Digital Converter (ADC) */
  AC_IRQn                  = 23, /**< 23 SAML21E18B Analog Comparators (AC) */
  DAC_IRQn                 = 24, /**< 24 SAML21E18B Digital-to-Analog Converter (DAC) */
  PTC_IRQn                 = 25, /**< 25 SAML21E18B Peripheral Touch Controller (PTC) */
  AES_IRQn                 = 26, /**< 26 SAML21E18B Advanced Encryption Standard (AES) */
  TRNG_IRQn                = 27, /**< 27 SAML21E18B True Random Generator (TRNG) */

  PERIPH_COUNT_IRQn        = 29  /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M handlers */
  void* pfnReset_Handler;
  void* pfnNonMaskableInt_Handler;
  void* pfnHardFault_Handler;
  void* pvReservedM12;
  void* pvReservedM11;
  void* pvReservedM10;
  void* pvReservedM9;
  void* pvReservedM8;
  void* pvReservedM7;
  void* pvReservedM6;
  void* pfnSVCall_Handler;
  void* pvReservedM4;
  void* pvReservedM3;
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Peripheral handlers */
  void* pfnSYSTEM_Handler;                /*  0 Main Clock, 32k Oscillators Control, Oscillators Control, Peripheral Access Controller, Power Manager, Supply Controller, Trigger Allocator */
  void* pfnWDT_Handler;                   /*  1 Watchdog Timer */
  void* pfnRTC_Handler;                   /*  2 Real-Time Counter */
  void* pfnEIC_Handler;                   /*  3 External Interrupt Controller */
  void* pfnNVMCTRL_Handler;               /*  4 Non-Volatile Memory Controller */
  void* pfnDMAC_Handler;                  /*  5 Direct Memory Access Controller */
  void* pfnUSB_Handler;                   /*  6 Universal Serial Bus */
  void* pfnEVSYS_Handler;                 /*  7 Event System Interface */
  void* pfnSERCOM0_Handler;               /*  8 Serial Communication Interface 0 */
  void* pfnSERCOM1_Handler;               /*  9 Serial Communication Interface 1 */
  void* pfnSERCOM2_Handler;               /* 10 Serial Communication Interface 2 */
  void* pfnSERCOM3_Handler;               /* 11 Serial Communication Interface 3 */
  void* pfnSERCOM4_Handler;               /* 12 Serial Communication Interface 4 */
  void* pfnSERCOM5_Handler;               /* 13 Serial Communication Interface 5 */
  void* pfnTCC0_Handler;                  /* 14 Timer Counter Control 0 */
  void* pfnTCC1_Handler;                  /* 15 Timer Counter Control 1 */
  void* pfnTCC2_Handler;                  /* 16 Timer Counter Control 2 */
  void* pfnTC0_Handler;                   /* 17 Basic Timer Counter 0 */
  void* pfnTC1_Handler;                   /* 18 Basic Timer Counter 1 */
  void* pvReserved19;
  void* pvReserved20;
  void* pfnTC4_Handler;                   /* 21 Basic Timer Counter 4 */
  void* pfnADC_Handler;                   /* 22 Analog Digital Converter */
  void* pfnAC_Handler;                    /* 23 Analog Comparators */
  void* pfnDAC_Handler;                   /* 24 Digital-to-Analog Converter */
  void* pfnPTC_Handler;                   /* 25 Peripheral Touch Controller */
  void* pfnAES_Handler;                   /* 26 Advanced Encryption Standard */
  void* pfnTRNG_Handler;                  /* 27 True Random Generator */
  void* pvReserved28;
} DeviceVectors;

/* Cortex-M0+ processor handlers */
void Reset_Handler               ( void );
void NonMaskableInt_Handler      ( void );
void HardFault_Handler           ( void );
void SVCall_Handler              ( void );
void PendSV_Handler              ( void );
void SysTick_Handler             ( void );

/* Peripherals handlers */
void SYSTEM_Handler              ( void );
void WDT_Handler                 ( void );
void RTC_Handler                 ( void );
void EIC_Handler                 ( void );
void NVMCTRL_Handler             ( void );
void DMAC_Handler                ( void );
void USB_Handler                 ( void );
void EVSYS_Handler               ( void );
void SERCOM0_Handler             ( void );
void SERCOM1_Handler             ( void );
void SERCOM2_Handler             ( void );
void SERCOM3_Handler             ( void );
void SERCOM4_Handler             ( void );
void SERCOM5_Handler             ( void );
void TCC0_Handler                ( void );
void TCC1_Handler                ( void );
void TCC2_Handler                ( void );
void TC0_Handler                 ( void );
void TC1_Handler                 ( void );
void TC4_Handler                 ( void );
void ADC_Handler                 ( void );
void AC_Handler                  ( void );
void DAC_Handler                 ( void );
void PTC_Handler                 ( void );
void AES_Handler                 ( void );
void TRNG_Handler                ( void );

/*
 * \brief Configuration of the Cortex-M0+ Processor and Core Peripherals
 */

#define __CM0PLUS_REV          1         /*!< Core revision r0p1 */
#define __MPU_PRESENT          0         /*!< MPU present or not */
#define __NVIC_PRIO_BITS       2         /*!< Number of bits used for Priority Levels */
#define __VTOR_PRESENT         1         /*!< VTOR present or not */
#define __Vendor_SysTickConfig 0         /*!< Set to 1 if different SysTick Config is used */

/**
 * \brief CMSIS includes
 */

#include <core_cm0plus.h>
#if !defined DONT_USE_CMSIS_INIT
#include "system_saml21.h"
#endif /* DONT_USE_CMSIS_INIT */

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAML21E18B */
/* ************************************************************************** */
/** \defgroup SAML21E18B_api Peripheral Software API */
/*@{*/

#include "component/ac.h"
#include "component/adc.h"
#include "component/aes.h"
#include "component/ccl.h"
#include "component/dac.h"
#include "component/dmac.h"
#include "component/dsu.h"
#include "component/eic.h"
#include "component/evsys.h"
#include "component/gclk.h"
#include "component/mclk.h"
#include "component/mtb.h"
#include "component/nvmctrl.h"
#include "component/opamp.h"
#include "component/oscctrl.h"
#include "component/osc32kctrl.h"
#include "component/pac.h"
#include "component/pm.h"
#include "component/port.h"
#include "component/rstc.h"
#include "component/rtc.h"
#include "component/sercom.h"
#include "component/supc.h"
#include "component/tc.h"
#include "component/tcc.h"
#include "component/trng.h"
#include "component/usb.h"
#include "component/wdt.h"
/*@}*/

/* ************************************************************************** */
/**  REGISTERS ACCESS DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */
/** \defgroup SAML21E18B_reg Registers Access Definitions */
/*@{*/

#include "instance/ac.h"
#include "instance/adc.h"
#include "instance/aes.h"
#include "instance/ccl.h"
#include "instance/dac.h"
#include "instance/dmac.h"
#include "instance/dsu.h"
#include "instance/eic.h"
#include "instance/evsys.h"
#include "instance/gclk.h"
#include "instance/mclk.h"
#include "instance/mtb.h"
#include "instance/nvmctrl.h"
#include "instance/opamp.h"
#include "instance/oscctrl.h"
#include "instance/osc32kctrl.h"
#include "instance/pac.h"
#include "instance/pm.h"
#include "instance/port.h"
#include "instance/ptc.h"
#include "instance/rstc.h"
#include "instance/rtc.h"
#include "instance/sercom0.h"
#include "instance/sercom1.h"
#include "instance/sercom2.h"
#include "instance/sercom3.h"
#include "instance/sercom4.h"
#include "instance/sercom5.h"
#include "instance/supc.h"
#include "instance/tc0.h"
#include "instance/tc1.h"
#include "instance/tc4.h"
#include "instance/tcc0.h"
#include "instance/tcc1.h"
#include "instance/tcc2.h"
#include "instance/trng.h"
#include "instance/usb.h"
#include "instance/wdt.h"
/*@}*/

/* ************************************************************************** */
/**  PERIPHERAL ID DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */
/** \defgroup SAML21E18B_id Peripheral Ids Definitions */
/*@{*/

// Peripheral instances on HPB0 bridge
#define ID_PM             0 /**< \brief Power Manager (PM) */
#define ID_MCLK           1 /**< \brief Main Clock (MCLK) */
#define ID_RSTC           2 /**< \brief Reset Controller (RSTC) */
#define ID_OSCCTRL        3 /**< \brief Oscillators Control (OSCCTRL) */
#define ID_OSC32KCTRL     4 /**< \brief 32k Oscillators Control (OSC32KCTRL) */
#define ID_SUPC           5 /**< \brief Supply Controller (SUPC) */
#define ID_GCLK           6 /**< \brief Generic Clock Generator (GCLK) */
#define ID_WDT            7 /**< \brief Watchdog Timer (WDT) */
#define ID_RTC            8 /**< \brief Real-Time Counter (RTC) */
#define ID_EIC            9 /**< \brief External Interrupt Controller (EIC) */
#define ID_PORT          10 /**< \brief Port Module (PORT) */

// Peripheral instances on HPB1 bridge
#define ID_USB           32 /**< \brief Universal Serial Bus (USB) */
#define ID_DSU           33 /**< \brief Device Service Unit (DSU) */
#define ID_NVMCTRL       34 /**< \brief Non-Volatile Memory Controller (NVMCTRL) */
#define ID_MTB           35 /**< \brief Cortex-M0+ Micro-Trace Buffer (MTB) */

// Peripheral instances on HPB2 bridge
#define ID_SERCOM0       64 /**< \brief Serial Communication Interface 0 (SERCOM0) */
#define ID_SERCOM1       65 /**< \brief Serial Communication Interface 1 (SERCOM1) */
#define ID_SERCOM2       66 /**< \brief Serial Communication Interface 2 (SERCOM2) */
#define ID_SERCOM3       67 /**< \brief Serial Communication Interface 3 (SERCOM3) */
#define ID_SERCOM4       68 /**< \brief Serial Communication Interface 4 (SERCOM4) */
#define ID_TCC0          69 /**< \brief Timer Counter Control 0 (TCC0) */
#define ID_TCC1          70 /**< \brief Timer Counter Control 1 (TCC1) */
#define ID_TCC2          71 /**< \brief Timer Counter Control 2 (TCC2) */
#define ID_TC0           72 /**< \brief Basic Timer Counter 0 (TC0) */
#define ID_TC1           73 /**< \brief Basic Timer Counter 1 (TC1) */
#define ID_DAC           76 /**< \brief Digital-to-Analog Converter (DAC) */
#define ID_AES           77 /**< \brief Advanced Encryption Standard (AES) */
#define ID_TRNG          78 /**< \brief True Random Generator (TRNG) */

// Peripheral instances on HPB3 bridge
#define ID_EVSYS         96 /**< \brief Event System Interface (EVSYS) */
#define ID_SERCOM5       97 /**< \brief Serial Communication Interface 5 (SERCOM5) */
#define ID_TC4           98 /**< \brief Basic Timer Counter 4 (TC4) */
#define ID_ADC           99 /**< \brief Analog Digital Converter (ADC) */
#define ID_AC           100 /**< \brief Analog Comparators (AC) */
#define ID_PTC          101 /**< \brief Peripheral Touch Controller (PTC) */
#define ID_OPAMP        102 /**< \brief Operational Amplifier (OPAMP) */
#define ID_CCL          103 /**< \brief Configurable Custom Logic (CCL) */

// Peripheral instances on HPB4 bridge
#define ID_PAC          128 /**< \brief Peripheral Access Controller (PAC) */
#define ID_DMAC         129 /**< \brief Direct Memory Access Controller (DMAC) */

#define ID_PERIPH_COUNT 130 /**< \brief Max number of peripheral IDs */
/*@}*/

/* ************************************************************************** */
/**  BASE ADDRESS DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */
/** \defgroup SAML21E18B_base Peripheral Base Address Definitions */
/*@{*/

#if defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)
#define AC                            (0x43001000) /**< \brief (AC) APB Base Address */
#define ADC                           (0x43000C00) /**< \brief (ADC) APB Base Address */
#define AES                           (0x42003400) /**< \brief (AES) APB Base Address */
#define CCL                           (0x43001C00) /**< \brief (CCL) APB Base Address */
#define DAC                           (0x42003000) /**< \brief (DAC) APB Base Address */
#define DMAC                          (0x44000400) /**< \brief (DMAC) APB Base Address */
#define DSU                           (0x41002000) /**< \brief (DSU) APB Base Address */
#define EIC                           (0x40002400) /**< \brief (EIC) APB Base Address */
#define EVSYS                         (0x43000000) /**< \brief (EVSYS) APB Base Address */
#define GCLK                          (0x40001800) /**< \brief (GCLK) APB Base Address */
#define MCLK                          (0x40000400) /**< \brief (MCLK) APB Base Address */
#define MTB                           (0x41006000) /**< \brief (MTB) APB Base Address */
#define NVMCTRL                       (0x41004000) /**< \brief (NVMCTRL) APB Base Address */
#define NVMCTRL_CAL                   (0x00800000) /**< \brief (NVMCTRL) CAL Base Address */
#define NVMCTRL_LOCKBIT               (0x00802000) /**< \brief (NVMCTRL) LOCKBIT Base Address */
#define NVMCTRL_OTP1                  (0x00806000) /**< \brief (NVMCTRL) OTP1 Base Address */
#define NVMCTRL_OTP2                  (0x00806008) /**< \brief (NVMCTRL) OTP2 Base Address */
#define NVMCTRL_OTP3                  (0x00806010) /**< \brief (NVMCTRL) OTP3 Base Address */
#define NVMCTRL_OTP4                  (0x00806018) /**< \brief (NVMCTRL) OTP4 Base Address */
#define NVMCTRL_OTP5                  (0x00806020) /**< \brief (NVMCTRL) OTP5 Base Address */
#define NVMCTRL_TEMP_LOG              (0x00806030) /**< \brief (NVMCTRL) TEMP_LOG Base Address */
#define NVMCTRL_USER                  (0x00804000) /**< \brief (NVMCTRL) USER Base Address */
#define OPAMP                         (0x43001800) /**< \brief (OPAMP) APB Base Address */
#define OSCCTRL                       (0x40000C00) /**< \brief (OSCCTRL) APB Base Address */
#define OSC32KCTRL                    (0x40001000) /**< \brief (OSC32KCTRL) APB Base Address */
#define PAC                           (0x44000000) /**< \brief (PAC) APB Base Address */
#define PM                            (0x40000000) /**< \brief (PM) APB Base Address */
#define PORT                          (0x40002800) /**< \brief (PORT) APB Base Address */
#define PORT_IOBUS                    (0x60000000) /**< \brief (PORT) IOBUS Base Address */
#define PTC                           (0x43001400) /**< \brief (PTC) APB Base Address */
#define RSTC                          (0x40000800) /**< \brief (RSTC) APB Base Address */
#define RTC                           (0x40002000) /**< \brief (RTC) APB Base Address */
#define SERCOM0                       (0x42000000) /**< \brief (SERCOM0) APB Base Address */
#define SERCOM1                       (0x42000400) /**< \brief (SERCOM1) APB Base Address */
#define SERCOM2                       (0x42000800) /**< \brief (SERCOM2) APB Base Address */
#define SERCOM3                       (0x42000C00) /**< \brief (SERCOM3) APB Base Address */
#define SERCOM4                       (0x42001000) /**< \brief (SERCOM4) APB Base Address */
#define SERCOM5                       (0x43000400) /**< \brief (SERCOM5) APB Base Address */
#define SUPC                          (0x40001400) /**< \brief (SUPC) APB Base Address */
#define TC0                           (0x42002000) /**< \brief (TC0) APB Base Address */
#define TC1                           (0x42002400) /**< \brief (TC1) APB Base Address */
#define TC4                           (0x43000800) /**< \brief (TC4) APB Base Address */
#define TCC0                          (0x42001400) /**< \brief (TCC0) APB Base Address */
#define TCC1                          (0x42001800) /**< \brief (TCC1) APB Base Address */
#define TCC2                          (0x42001C00) /**< \brief (TCC2) APB Base Address */
#define TRNG                          (0x42003800) /**< \brief (TRNG) APB Base Address */
#define USB                           (0x41000000) /**< \brief (USB) APB Base Address */
#define WDT                           (0x40001C00) /**< \brief (WDT) APB Base Address */
#else
#define AC                ((Ac       *)0x43001000UL) /**< \brief (AC) APB Base Address */
#define AC_INST_NUM       1                          /**< \brief (AC) Number of instances */
#define AC_INSTS          { AC }                     /**< \brief (AC) Instances List */

#define ADC               ((Adc      *)0x43000C00UL) /**< \brief (ADC) APB Base Address */
#define ADC_INST_NUM      1                          /**< \brief (ADC) Number of instances */
#define ADC_INSTS         { ADC }                    /**< \brief (ADC) Instances List */

#define AES               ((Aes      *)0x42003400UL) /**< \brief (AES) APB Base Address */
#define AES_INST_NUM      1                          /**< \brief (AES) Number of instances */
#define AES_INSTS         { AES }                    /**< \brief (AES) Instances List */

#define CCL               ((Ccl      *)0x43001C00UL) /**< \brief (CCL) APB Base Address */
#define CCL_INST_NUM      1                          /**< \brief (CCL) Number of instances */
#define CCL_INSTS         { CCL }                    /**< \brief (CCL) Instances List */

#define DAC               ((Dac      *)0x42003000UL) /**< \brief (DAC) APB Base Address */
#define DAC_INST_NUM      1                          /**< \brief (DAC) Number of instances */
#define DAC_INSTS         { DAC }                    /**< \brief (DAC) Instances List */

#define DMAC              ((Dmac     *)0x44000400UL) /**< \brief (DMAC) APB Base Address */
#define DMAC_INST_NUM     1                          /**< \brief (DMAC) Number of instances */
#define DMAC_INSTS        { DMAC }                   /**< \brief (DMAC) Instances List */

#define DSU               ((Dsu      *)0x41002000UL) /**< \brief (DSU) APB Base Address */
#define DSU_INST_NUM      1                          /**< \brief (DSU) Number of instances */
#define DSU_INSTS         { DSU }                    /**< \brief (DSU) Instances List */

#define EIC               ((Eic      *)0x40002400UL) /**< \brief (EIC) APB Base Address */
#define EIC_INST_NUM      1                          /**< \brief (EIC) Number of instances */
#define EIC_INSTS         { EIC }                    /**< \brief (EIC) Instances List */

#define EVSYS             ((Evsys    *)0x43000000UL) /**< \brief (EVSYS) APB Base Address */
#define EVSYS_INST_NUM    1                          /**< \brief (EVSYS) Number of instances */
#define EVSYS_INSTS       { EVSYS }                  /**< \brief (EVSYS) Instances List */

#define GCLK              ((Gclk     *)0x40001800UL) /**< \brief (GCLK) APB Base Address */
#define GCLK_INST_NUM     1                          /**< \brief (GCLK) Number of instances */
#define GCLK_INSTS        { GCLK }                   /**< \brief (GCLK) Instances List */

#define MCLK              ((Mclk     *)0x40000400UL) /**< \brief (MCLK) APB Base Address */
#define MCLK_INST_NUM     1                          /**< \brief (MCLK) Number of instances */
#define MCLK_INSTS        { MCLK }                   /**< \brief (MCLK) Instances List */

#define MTB               ((Mtb      *)0x41006000UL) /**< \brief (MTB) APB Base Address */
#define MTB_INST_NUM      1                          /**< \brief (MTB) Number of instances */
#define MTB_INSTS         { MTB }                    /**< \brief (MTB) Instances List */

#define NVMCTRL           ((Nvmctrl  *)0x41004000UL) /**< \brief (NVMCTRL) APB Base Address */
#define NVMCTRL_CAL                   (0x00800000UL) /**< \brief (NVMCTRL) CAL Base Address */
#define NVMCTRL_LOCKBIT               (0x00802000UL) /**< \brief (NVMCTRL) LOCKBIT Base Address */
#define NVMCTRL_OTP1                  (0x00806000UL) /**< \brief (NVMCTRL) OTP1 Base Address */
#define NVMCTRL_OTP2                  (0x00806008UL) /**< \brief (NVMCTRL) OTP2 Base Address */
#define NVMCTRL_OTP3                  (0x00806010UL) /**< \brief (NVMCTRL) OTP3 Base Address */
#define NVMCTRL_OTP4                  (0x00806018UL) /**< \brief (NVMCTRL) OTP4 Base Address */
#define NVMCTRL_OTP5                  (0x00806020UL) /**< \brief (NVMCTRL) OTP5 Base Address */
#define NVMCTRL_TEMP_LOG              (0x00806030UL) /**< \brief (NVMCTRL) TEMP_LOG Base Address */
#define NVMCTRL_USER                  (0x00804000UL) /**< \brief (NVMCTRL) USER Base Address */
#define NVMCTRL_INST_NUM  1                          /**< \brief (NVMCTRL) Number of instances */
#define NVMCTRL_INSTS     { NVMCTRL }                /**< \brief (NVMCTRL) Instances List */

#define OPAMP             ((Opamp    *)0x43001800UL) /**< \brief (OPAMP) APB Base Address */
#define OPAMP_INST_NUM    1                          /**< \brief (OPAMP) Number of instances */
#define OPAMP_INSTS       { OPAMP }                  /**< \brief (OPAMP) Instances List */

#define OSCCTRL           ((Oscctrl  *)0x40000C00UL) /**< \brief (OSCCTRL) APB Base Address */
#define OSCCTRL_INST_NUM  1                          /**< \brief (OSCCTRL) Number of instances */
#define OSCCTRL_INSTS     { OSCCTRL }                /**< \brief (OSCCTRL) Instances List */

#define OSC32KCTRL        ((Osc32kctrl *)0x40001000UL) /**< \brief (OSC32KCTRL) APB Base Address */
#define OSC32KCTRL_INST_NUM 1                          /**< \brief (OSC32KCTRL) Number of instances */
#define OSC32KCTRL_INSTS  { OSC32KCTRL }             /**< \brief (OSC32KCTRL) Instances List */

#define PAC               ((Pac      *)0x44000000UL) /**< \brief (PAC) APB Base Address */
#define PAC_INST_NUM      1                          /**< \brief (PAC) Number of instances */
#define PAC_INSTS         { PAC }                    /**< \brief (PAC) Instances List */

#define PM                ((Pm       *)0x40000000UL) /**< \brief (PM) APB Base Address */
#define PM_INST_NUM       1                          /**< \brief (PM) Number of instances */
#define PM_INSTS          { PM }                     /**< \brief (PM) Instances List */

#define PORT              ((Port     *)0x40002800UL) /**< \brief (PORT) APB Base Address */
#define PORT_IOBUS        ((Port     *)0x60000000UL) /**< \brief (PORT) IOBUS Base Address */
#define PORT_INST_NUM     1                          /**< \brief (PORT) Number of instances */
#define PORT_INSTS        { PORT }                   /**< \brief (PORT) Instances List */
#define PORT_IOBUS_INST_NUM 1                          /**< \brief (PORT) Number of instances */
#define PORT_IOBUS_INSTS  { PORT_IOBUS }             /**< \brief (PORT) Instances List */

#define PTC               ((void     *)0x43001400UL) /**< \brief (PTC) APB Base Address */
#define PTC_GCLK_ID       33
#define PTC_INST_NUM      1                          /**< \brief (PTC) Number of instances */
#define PTC_INSTS         { PTC }                    /**< \brief (PTC) Instances List */

#define RSTC              ((Rstc     *)0x40000800UL) /**< \brief (RSTC) APB Base Address */
#define RSTC_INST_NUM     1                          /**< \brief (RSTC) Number of instances */
#define RSTC_INSTS        { RSTC }                   /**< \brief (RSTC) Instances List */

#define RTC               ((Rtc      *)0x40002000UL) /**< \brief (RTC) APB Base Address */
#define RTC_INST_NUM      1                          /**< \brief (RTC) Number of instances */
#define RTC_INSTS         { RTC }                    /**< \brief (RTC) Instances List */

#define SERCOM0           ((Sercom   *)0x42000000UL) /**< \brief (SERCOM0) APB Base Address */
#define SERCOM1           ((Sercom   *)0x42000400UL) /**< \brief (SERCOM1) APB Base Address */
#define SERCOM2           ((Sercom   *)0x42000800UL) /**< \brief (SERCOM2) APB Base Address */
#define SERCOM3           ((Sercom   *)0x42000C00UL) /**< \brief (SERCOM3) APB Base Address */
#define SERCOM4           ((Sercom   *)0x42001000UL) /**< \brief (SERCOM4) APB Base Address */
#define SERCOM5           ((Sercom   *)0x43000400UL) /**< \brief (SERCOM5) APB Base Address */
#define SERCOM_INST_NUM   6                          /**< \brief (SERCOM) Number of instances */
#define SERCOM_INSTS      { SERCOM0, SERCOM1, SERCOM2, SERCOM3, SERCOM4, SERCOM5 } /**< \brief (SERCOM) Instances List */

#define SUPC              ((Supc     *)0x40001400UL) /**< \brief (SUPC) APB Base Address */
#define SUPC_INST_NUM     1                          /**< \brief (SUPC) Number of instances */
#define SUPC_INSTS        { SUPC }                   /**< \brief (SUPC) Instances List */

#define TC0               ((Tc       *)0x42002000UL) /**< \brief (TC0) APB Base Address */
#define TC1               ((Tc       *)0x42002400UL) /**< \brief (TC1) APB Base Address */
#define TC4               ((Tc       *)0x43000800UL) /**< \brief (TC4) APB Base Address */
#define TC_INST_NUM       3                          /**< \brief (TC) Number of instances */
#define TC_INSTS          { TC0, TC1, TC4 }          /**< \brief (TC) Instances List */

#define TCC0              ((Tcc      *)0x42001400UL) /**< \brief (TCC0) APB Base Address */
#define TCC1              ((Tcc      *)0x42001800UL) /**< \brief (TCC1) APB Base Address */
#define TCC2              ((Tcc      *)0x42001C00UL) /**< \brief (TCC2) APB Base Address */
#define TCC_INST_NUM      3                          /**< \brief (TCC) Number of instances */
#define TCC_INSTS         { TCC0, TCC1, TCC2 }       /**< \brief (TCC) Instances List */

#define TRNG              ((Trng     *)0x42003800UL) /**< \brief (TRNG) APB Base Address */
#define TRNG_INST_NUM     1                          /**< \brief (TRNG) Number of instances */
#define TRNG_INSTS        { TRNG }                   /**< \brief (TRNG) Instances List */

#define USB               ((Usb      *)0x41000000UL) /**< \brief (USB) APB Base Address */
#define USB_INST_NUM      1                          /**< \brief (USB) Number of instances */
#define USB_INSTS         { USB }                    /**< \brief (USB) Instances List */

#define WDT               ((Wdt      *)0x40001C00UL) /**< \brief (WDT) APB Base Address */
#define WDT_INST_NUM      1                          /**< \brief (WDT) Number of instances */
#define WDT_INSTS         { WDT }                    /**< \brief (WDT) Instances List */

#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/*@}*/

/* ************************************************************************** */
/**  PORT DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */
/** \defgroup SAML21E18B_port PORT Definitions */
/*@{*/

#include "pio/saml21e18b.h"
/*@}*/

/* ************************************************************************** */
/**  MEMORY MAPPING DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */

#define FLASH_SIZE            _UL_(0x00040000) /* 256 kB */
#define FLASH_PAGE_SIZE       64
#define FLASH_NB_OF_PAGES     4096
#define FLASH_USER_PAGE_SIZE  64
#define HSRAM_SIZE            _UL_(0x00008000) /* 32 kB */
#define LPRAM_SIZE            _UL_(0x00002000) /* 8 kB */

#define FLASH_ADDR            _UL_(0x00000000) /**< FLASH base address */
#define FLASH_USER_PAGE_ADDR  _UL_(0x00800000) /**< FLASH_USER_PAGE base address */
#define HSRAM_ADDR            _UL_(0x20000000) /**< HSRAM base address */
#define LPRAM_ADDR            _UL_(0x30000000) /**< LPRAM base address */
#define HPB0_ADDR             _UL_(0x40000000) /**< HPB0 base address */
#define HPB1_ADDR             _UL_(0x41000000) /**< HPB1 base address */
#define HPB2_ADDR             _UL_(0x42000000) /**< HPB2 base address */
#define HPB3_ADDR             _UL_(0x43000000) /**< HPB3 base address */
#define HPB4_ADDR             _UL_(0x44000000) /**< HPB4 base address */
#define PPB_ADDR              _UL_(0xE0000000) /**< PPB base address */

#define DSU_DID_RESETVALUE    _UL_(0x10810219)
#define NVMCTRL_RWW_EEPROM_SIZE _UL_(0x00002000) /* 8 kB */
#define PORT_GROUPS           1
#define USB_HOST_IMPLEMENTED  1

/* ************************************************************************** */
/**  ELECTRICAL DEFINITIONS FOR SAML21E18B */
/* ************************************************************************** */


#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* SAML21E18B_H */
