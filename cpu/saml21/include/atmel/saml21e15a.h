/**
 * \file
 *
 * \brief Header file for SAML21E15A
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAML21E15A_
#define _SAML21E15A_

/**
 * \ingroup SAML21_definitions
 * \addtogroup SAML21E15A_definitions SAML21E15A definitions
 * This file defines all structures and symbols for SAML21E15A:
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
typedef volatile       uint32_t WoReg8;  /**< Write only  8-bit register (volatile unsigned int) */
typedef volatile       uint32_t RwReg;   /**< Read-Write 32-bit register (volatile unsigned int) */
typedef volatile       uint16_t RwReg16; /**< Read-Write 16-bit register (volatile unsigned int) */
typedef volatile       uint8_t  RwReg8;  /**< Read-Write  8-bit register (volatile unsigned int) */
#define CAST(type, value) ((type *)(value))
#define REG_ACCESS(type, address) (*(type*)(address)) /**< C code: Register value */
#else
#define CAST(type, value) (value)
#define REG_ACCESS(type, address) (address) /**< Assembly code: Register address */
#endif

/* ************************************************************************** */
/**  CMSIS DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */
/** \defgroup SAML21E15A_cmsis CMSIS Definitions */
/*@{*/

/** Interrupt Number Definition */
typedef enum IRQn
{
  /******  Cortex-M0+ Processor Exceptions Numbers ******************************/
  NonMaskableInt_IRQn      = -14,/**<  2 Non Maskable Interrupt                 */
  HardFault_IRQn           = -13,/**<  3 Cortex-M0+ Hard Fault Interrupt        */
  SVCall_IRQn              = -5, /**< 11 Cortex-M0+ SV Call Interrupt           */
  PendSV_IRQn              = -2, /**< 14 Cortex-M0+ Pend SV Interrupt           */
  SysTick_IRQn             = -1, /**< 15 Cortex-M0+ System Tick Interrupt       */
  /******  SAML21E15A-specific Interrupt Numbers ***********************/
  SYSTEM_IRQn              =  0, /**<  0 SAML21E15A System Interrupts */
  MCLK_IRQn                =  0, /**<  0 SAML21E15A Main Clock (MCLK) */
  OSCCTRL_IRQn             =  0, /**<  0 SAML21E15A Oscillators Control (OSCCTRL) */
  OSC32KCTRL_IRQn          =  0, /**<  0 SAML21E15A 32k Oscillators Control (OSC32KCTRL) */
  PAC_IRQn                 =  0, /**<  0 SAML21E15A Peripheral Access Controller (PAC) */
  PM_IRQn                  =  0, /**<  0 SAML21E15A Power Manager (PM) */
  SUPC_IRQn                =  0, /**<  0 SAML21E15A Supply Controller (SUPC) */
  TAL_IRQn                 =  0, /**<  0 SAML21E15A Trigger Allocator (TAL) */
  WDT_IRQn                 =  1, /**<  1 SAML21E15A Watchdog Timer (WDT) */
  RTC_IRQn                 =  2, /**<  2 SAML21E15A Real-Time Counter (RTC) */
  EIC_IRQn                 =  3, /**<  3 SAML21E15A External Interrupt Controller (EIC) */
  NVMCTRL_IRQn             =  4, /**<  4 SAML21E15A Non-Volatile Memory Controller (NVMCTRL) */
  DMAC_IRQn                =  5, /**<  5 SAML21E15A Direct Memory Access Controller (DMAC) */
  USB_IRQn                 =  6, /**<  6 SAML21E15A Universal Serial Bus (USB) */
  EVSYS_IRQn               =  7, /**<  7 SAML21E15A Event System Interface (EVSYS) */
  SERCOM0_IRQn             =  8, /**<  8 SAML21E15A Serial Communication Interface 0 (SERCOM0) */
  SERCOM1_IRQn             =  9, /**<  9 SAML21E15A Serial Communication Interface 1 (SERCOM1) */
  SERCOM2_IRQn             = 10, /**< 10 SAML21E15A Serial Communication Interface 2 (SERCOM2) */
  SERCOM3_IRQn             = 11, /**< 11 SAML21E15A Serial Communication Interface 3 (SERCOM3) */
  TCC0_IRQn                = 14, /**< 14 SAML21E15A Timer Counter Control 0 (TCC0) */
  TCC1_IRQn                = 15, /**< 15 SAML21E15A Timer Counter Control 1 (TCC1) */
  TCC2_IRQn                = 16, /**< 16 SAML21E15A Timer Counter Control 2 (TCC2) */
  TC0_IRQn                 = 17, /**< 17 SAML21E15A Basic Timer Counter 0 (TC0) */
  TC1_IRQn                 = 18, /**< 18 SAML21E15A Basic Timer Counter 1 (TC1) */
  TC4_IRQn                 = 21, /**< 21 SAML21E15A Basic Timer Counter 4 (TC4) */
  ADC_IRQn                 = 22, /**< 22 SAML21E15A Analog Digital Converter (ADC) */
  AC_IRQn                  = 23, /**< 23 SAML21E15A Analog Comparators (AC) */
  DAC_IRQn                 = 24, /**< 24 SAML21E15A Digital-to-Analog Converter (DAC) */
  PTC_IRQn                 = 25, /**< 25 SAML21E15A Peripheral Touch Controller (PTC) */
  AES_IRQn                 = 26, /**< 26 SAML21E15A Advanced Encryption Standard (AES) */
  TRNG_IRQn                = 27, /**< 27 SAML21E15A True Random Generator (TRNG) */
  PICOP_IRQn               = 28, /**< 28 SAML21E15A PicoProcessor (PICOP) */

  PERIPH_COUNT_IRQn        = 29  /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M handlers */
  void* pfnReset_Handler;
  void* pfnNMI_Handler;
  void* pfnHardFault_Handler;
  void* pfnReservedM12;
  void* pfnReservedM11;
  void* pfnReservedM10;
  void* pfnReservedM9;
  void* pfnReservedM8;
  void* pfnReservedM7;
  void* pfnReservedM6;
  void* pfnSVC_Handler;
  void* pfnReservedM4;
  void* pfnReservedM3;
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Peripheral handlers */
  void* pfnSYSTEM_Handler;                /*  0 Main Clock, Oscillators Control, 32k Oscillators Control, Peripheral Access Controller, Power Manager, Supply Controller, Trigger Allocator */
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
  void* pfnReserved12;
  void* pfnReserved13;
  void* pfnTCC0_Handler;                  /* 14 Timer Counter Control 0 */
  void* pfnTCC1_Handler;                  /* 15 Timer Counter Control 1 */
  void* pfnTCC2_Handler;                  /* 16 Timer Counter Control 2 */
  void* pfnTC0_Handler;                   /* 17 Basic Timer Counter 0 */
  void* pfnTC1_Handler;                   /* 18 Basic Timer Counter 1 */
  void* pfnReserved19;
  void* pfnReserved20;
  void* pfnTC4_Handler;                   /* 21 Basic Timer Counter 4 */
  void* pfnADC_Handler;                   /* 22 Analog Digital Converter */
  void* pfnAC_Handler;                    /* 23 Analog Comparators */
  void* pfnDAC_Handler;                   /* 24 Digital-to-Analog Converter */
  void* pfnPTC_Handler;                   /* 25 Peripheral Touch Controller */
  void* pfnAES_Handler;                   /* 26 Advanced Encryption Standard */
  void* pfnTRNG_Handler;                  /* 27 True Random Generator */
  void* pfnPICOP_Handler;                 /* 28 PicoProcessor */
} DeviceVectors;

/* Cortex-M0+ processor handlers */
void Reset_Handler               ( void );
void NMI_Handler                 ( void );
void HardFault_Handler           ( void );
void SVC_Handler                 ( void );
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
void PICOP_Handler               ( void );

/*
 * \brief Configuration of the Cortex-M0+ Processor and Core Peripherals
 */

#define LITTLE_ENDIAN          1
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
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAML21E15A */
/* ************************************************************************** */
/** \defgroup SAML21E15A_api Peripheral Software API */
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
#include "component/tal.h"
#include "component/tc.h"
#include "component/tcc.h"
#include "component/trng.h"
#include "component/usb.h"
#include "component/wdt.h"
/*@}*/

/* ************************************************************************** */
/**  REGISTERS ACCESS DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */
/** \defgroup SAML21E15A_reg Registers Access Definitions */
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
#include "instance/rstc.h"
#include "instance/rtc.h"
#include "instance/sercom0.h"
#include "instance/sercom1.h"
#include "instance/sercom2.h"
#include "instance/sercom3.h"
#include "instance/supc.h"
#include "instance/tal.h"
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
/**  PERIPHERAL ID DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */
/** \defgroup SAML21E15A_id Peripheral Ids Definitions */
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
#define ID_TAL           11 /**< \brief Trigger Allocator (TAL) */

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
#define ID_TC4           98 /**< \brief Basic Timer Counter 4 (TC4) */
#define ID_ADC           99 /**< \brief Analog Digital Converter (ADC) */
#define ID_AC           100 /**< \brief Analog Comparators (AC) */
#define ID_PTC          101 /**< \brief Peripheral Touch Controller (PTC) */
#define ID_OPAMP        102 /**< \brief Operational Amplifier (OPAMP) */
#define ID_CCL          103 /**< \brief Configurable Custom Logic (CCL) */

// Peripheral instances on HPB4 bridge
#define ID_PAC          128 /**< \brief Peripheral Access Controller (PAC) */
#define ID_DMAC         129 /**< \brief Direct Memory Access Controller (DMAC) */

#define ID_PERIPH_COUNT 130 /**< \brief Number of peripheral IDs */
/*@}*/

/* ************************************************************************** */
/**  BASE ADDRESS DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */
/** \defgroup SAML21E15A_base Peripheral Base Address Definitions */
/*@{*/

#if defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)
#define AC                            (0x43001000UL) /**< \brief (AC) APB Base Address */
#define ADC                           (0x43000C00UL) /**< \brief (ADC) APB Base Address */
#define AES                           (0x42003400UL) /**< \brief (AES) APB Base Address */
#define CCL                           (0x43001C00UL) /**< \brief (CCL) APB Base Address */
#define DAC                           (0x42003000UL) /**< \brief (DAC) APB Base Address */
#define DMAC                          (0x44000400UL) /**< \brief (DMAC) APB Base Address */
#define DSU                           (0x41002000UL) /**< \brief (DSU) APB Base Address */
#define EIC                           (0x40002400UL) /**< \brief (EIC) APB Base Address */
#define EVSYS                         (0x43000000UL) /**< \brief (EVSYS) APB Base Address */
#define GCLK                          (0x40001800UL) /**< \brief (GCLK) APB Base Address */
#define MCLK                          (0x40000400UL) /**< \brief (MCLK) APB Base Address */
#define MTB                           (0x41006000UL) /**< \brief (MTB) APB Base Address */
#define NVMCTRL                       (0x41004000UL) /**< \brief (NVMCTRL) APB Base Address */
#define NVMCTRL_CAL                   (0x00800000UL) /**< \brief (NVMCTRL) CAL Base Address */
#define NVMCTRL_LOCKBIT               (0x00802000UL) /**< \brief (NVMCTRL) LOCKBIT Base Address */
#define NVMCTRL_OTP1                  (0x00806000UL) /**< \brief (NVMCTRL) OTP1 Base Address */
#define NVMCTRL_OTP2                  (0x00806008UL) /**< \brief (NVMCTRL) OTP2 Base Address */
#define NVMCTRL_OTP3                  (0x00806010UL) /**< \brief (NVMCTRL) OTP3 Base Address */
#define NVMCTRL_OTP4                  (0x00806018UL) /**< \brief (NVMCTRL) OTP4 Base Address */
#define NVMCTRL_OTP5                  (0x00806020UL) /**< \brief (NVMCTRL) OTP5 Base Address */
#define NVMCTRL_TEMP_LOG              (0x00806030UL) /**< \brief (NVMCTRL) TEMP_LOG Base Address */
#define NVMCTRL_USER                  (0x00804000UL) /**< \brief (NVMCTRL) USER Base Address */
#define OPAMP                         (0x43001800UL) /**< \brief (OPAMP) APB Base Address */
#define OSCCTRL                       (0x40000C00UL) /**< \brief (OSCCTRL) APB Base Address */
#define OSC32KCTRL                    (0x40001000UL) /**< \brief (OSC32KCTRL) APB Base Address */
#define PAC                           (0x44000000UL) /**< \brief (PAC) APB Base Address */
#define PM                            (0x40000000UL) /**< \brief (PM) APB Base Address */
#define PORT                          (0x40002800UL) /**< \brief (PORT) APB Base Address */
#define PORT_IOBUS                    (0x60000000UL) /**< \brief (PORT) IOBUS Base Address */
#define RSTC                          (0x40000800UL) /**< \brief (RSTC) APB Base Address */
#define RTC                           (0x40002000UL) /**< \brief (RTC) APB Base Address */
#define SERCOM0                       (0x42000000UL) /**< \brief (SERCOM0) APB Base Address */
#define SERCOM1                       (0x42000400UL) /**< \brief (SERCOM1) APB Base Address */
#define SERCOM2                       (0x42000800UL) /**< \brief (SERCOM2) APB Base Address */
#define SERCOM3                       (0x42000C00UL) /**< \brief (SERCOM3) APB Base Address */
#define SUPC                          (0x40001400UL) /**< \brief (SUPC) APB Base Address */
#define TAL                           (0x40002C00UL) /**< \brief (TAL) APB Base Address */
#define TC0                           (0x42002000UL) /**< \brief (TC0) APB Base Address */
#define TC1                           (0x42002400UL) /**< \brief (TC1) APB Base Address */
#define TC4                           (0x43000800UL) /**< \brief (TC4) APB Base Address */
#define TCC0                          (0x42001400UL) /**< \brief (TCC0) APB Base Address */
#define TCC1                          (0x42001800UL) /**< \brief (TCC1) APB Base Address */
#define TCC2                          (0x42001C00UL) /**< \brief (TCC2) APB Base Address */
#define TRNG                          (0x42003800UL) /**< \brief (TRNG) APB Base Address */
#define USB                           (0x41000000UL) /**< \brief (USB) APB Base Address */
#define WDT                           (0x40001C00UL) /**< \brief (WDT) APB Base Address */
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
#define SERCOM_INST_NUM   4                          /**< \brief (SERCOM) Number of instances */
#define SERCOM_INSTS      { SERCOM0, SERCOM1, SERCOM2, SERCOM3 } /**< \brief (SERCOM) Instances List */

#define SUPC              ((Supc     *)0x40001400UL) /**< \brief (SUPC) APB Base Address */
#define SUPC_INST_NUM     1                          /**< \brief (SUPC) Number of instances */
#define SUPC_INSTS        { SUPC }                   /**< \brief (SUPC) Instances List */

#define TAL               ((Tal      *)0x40002C00UL) /**< \brief (TAL) APB Base Address */
#define TAL_INST_NUM      1                          /**< \brief (TAL) Number of instances */
#define TAL_INSTS         { TAL }                    /**< \brief (TAL) Instances List */

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
/**  PORT DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */
/** \defgroup SAML21E15A_port PORT Definitions */
/*@{*/

#include "pio/saml21e15a.h"
/*@}*/

/* ************************************************************************** */
/**  MEMORY MAPPING DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */

#define FLASH_SIZE            0x8000UL /* 32 kB */
#define FLASH_PAGE_SIZE       64
#define FLASH_NB_OF_PAGES     512
#define FLASH_USER_PAGE_SIZE  64
#define HSRAM_SIZE            0x1000UL /* 4 kB */
#define LPRAM_SIZE            0x800UL /* 2 kB */
#define PICOPRAM_SIZE         0x1000UL /* 4 kB */
#define FLASH_ADDR            (0x00000000UL) /**< FLASH base address */
#define FLASH_USER_PAGE_ADDR  (0x00800000UL) /**< FLASH_USER_PAGE base address */
#define HSRAM_ADDR            (0x20000000UL) /**< HSRAM base address */
#define LPRAM_ADDR            (0x30000000UL) /**< LPRAM base address */
#define PICOPRAM_ADDR         (0x50000000UL) /**< PICOPRAM base address */

#define DSU_DID_RESETVALUE    0x1081000DUL
#define NVMCTRL_RWW_EEPROM_SIZE 0x400UL /* 1 kB */
#define PORT_GROUPS           1
#define USB_HOST_IMPLEMENTED  1

/* ************************************************************************** */
/**  ELECTRICAL DEFINITIONS FOR SAML21E15A */
/* ************************************************************************** */


#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* SAML21E15A_H */
