/*
** ###################################################################
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    KEA128RM, Rev. 22, Jun 2014
**     Version:             rev. 1.4, 2015-07-24
**     Build:               b150730
**
**     Abstract:
**         CMSIS Peripheral Access Layer for SKEAZ1284
**
**     Copyright (c) 1997 - 2015 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2013-07-30)
**         Initial version.
**     - rev. 1.1 (2013-10-29)
**         Definition of BITBAND macros updated to support peripherals with 32-bit acces disabled.
**     - rev. 1.2 (2014-01-10)
**         CAN - Corrected address of TSIDR1 register.
**         CAN - Corrected name of MSCAN_TDLR bit DLC to TDLC.
**         FTM0 - Added access macro for EXTTRIG register.
**         NVIC - Registers access macros improved.
**         SCB - Unused bits removed, mask, shift macros improved.
**         Defines of interrupt vectors aligned to RM.
**     - rev. 1.3 (2014-06-18)
**         The declaration of clock configurations has been moved to separate header file system_MKE02Z2.h
**         Module access macro {module}_BASES replaced by {module}_BASE_PTRS.
**         I2C - Renamed status register S to S1 to match RM naming.
**         Renamed interrupts: INT_PMC to INT_LVD_LVW, INT_ADC to ADC0,INT_WDOG to INT_Watchdog.
**     - rev. 1.4 (2015-07-24)
**         Correction of backward compatibility.
**
** ###################################################################
*/

/*!
 * @file SKEAZ1284.h
 * @version 1.4
 * @date 2015-07-24
 * @brief CMSIS Peripheral Access Layer for SKEAZ1284
 *
 * CMSIS Peripheral Access Layer for SKEAZ1284
 */


/* ----------------------------------------------------------------------------
   -- MCU activation
   ---------------------------------------------------------------------------- */

/* Prevention from multiple including the same memory map */
#if !defined(SKEAZ1284_H_)  /* Check if memory map has not been already included */
#define SKEAZ1284_H_
#define MCU_SKEAZ1284

/* Check if another memory map has not been also included */
#if (defined(MCU_ACTIVE))
  #error SKEAZ1284 memory map: There is already included another memory map. Only one memory map can be included.
#endif /* (defined(MCU_ACTIVE)) */
#define MCU_ACTIVE

#include <stdint.h>

/** Memory map major version (memory maps with equal major version number are
 * compatible) */
#define MCU_MEM_MAP_VERSION 0x0100u
/** Memory map minor version */
#define MCU_MEM_MAP_VERSION_MINOR 0x0004u


/* ----------------------------------------------------------------------------
   -- Interrupt vector numbers
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Interrupt_vector_numbers Interrupt vector numbers
 * @{
 */

/** Interrupt Number Definitions */
#define NUMBER_OF_INT_VECTORS 48                 /**< Number of interrupts in the Vector table */

typedef enum IRQn {
  /* Core interrupts */
  NonMaskableInt_IRQn          = -14,              /**< Non Maskable Interrupt */
  HardFault_IRQn               = -13,              /**< Cortex-M0 SV Hard Fault Interrupt */
  SVCall_IRQn                  = -5,               /**< Cortex-M0 SV Call Interrupt */
  PendSV_IRQn                  = -2,               /**< Cortex-M0 Pend SV Interrupt */
  SysTick_IRQn                 = -1,               /**< Cortex-M0 System Tick Interrupt */

  /* Device specific interrupts */
  Reserved16_IRQn              = 0,                /**< Reserved interrupt */
  Reserved17_IRQn              = 1,                /**< Reserved interrupt */
  Reserved18_IRQn              = 2,                /**< Reserved interrupt */
  Reserved19_IRQn              = 3,                /**< Reserved interrupt */
  Reserved20_IRQn              = 4,                /**< Reserved interrupt */
  FTMRE_IRQn                   = 5,                /**< FTMRE command complete */
  LVD_LVW_IRQn                 = 6,                /**< Low-voltage warning */
  IRQ_IRQn                     = 7,                /**< External interrupt */
  I2C0_IRQn                    = 8,                /**< I2C0 single interrupt vector for all sources */
  I2C1_IRQn                    = 9,                /**< I2C1 single interrupt vector for all sources */
  SPI0_IRQn                    = 10,               /**< SPI0 single interrupt vector for all sources */
  SPI1_IRQn                    = 11,               /**< SPI1 single interrupt vector for all sources */
  UART0_IRQn                   = 12,               /**< UART0 status and error */
  UART1_IRQn                   = 13,               /**< UART1 status and error */
  UART2_IRQn                   = 14,               /**< UART2 status and error */
  ADC0_IRQn                    = 15,               /**< ADC conversion complete interrupt */
  ACMP0_IRQn                   = 16,               /**< ACMP0 interrupt */
  FTM0_IRQn                    = 17,               /**< FTM0 single interrupt vector for all sources */
  FTM1_IRQn                    = 18,               /**< FTM1 single interrupt vector for all sources */
  FTM2_IRQn                    = 19,               /**< FTM2 single interrupt vector for all sources */
  RTC_IRQn                     = 20,               /**< RTC overflow */
  ACMP1_IRQn                   = 21,               /**< ACMP1 interrupt */
  PIT0_IRQn                 = 22,               /**< PIT CH0 overflow */
  PIT1_IRQn                 = 23,               /**< PIT CH1 overflow */
  KBI0_IRQn                    = 24,               /**< KBI0 interrupt */
  KBI1_IRQn                    = 25,               /**< KBI1 interrupt */
  Reserved42_IRQn              = 26,               /**< Reserved interrupt */
  ICS_IRQn                     = 27,               /**< Clock loss of lock */
  WDOG_EWM_IRQn                = 28,               /**< Watchdog timeout */
  PWT_IRQn                     = 29,               /**< PWT interrupt */
  MSCAN_RX_IRQn                = 30,               /**< MSCAN Rx interrupt */
  MSCAN_TX_IRQn                = 31                /**< MSCAN Tx, Err and Wake-up interrupt */
} IRQn_Type;

/*!
 * @}
 */ /* end of group Interrupt_vector_numbers */


/* ----------------------------------------------------------------------------
   -- Cortex M0 Core Configuration
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Cortex_Core_Configuration Cortex M0 Core Configuration
 * @{
 */

#define __CM0PLUS_REV                  0x0000    /**< Core revision r0p0 */
#define __MPU_PRESENT                  0         /**< Defines if an MPU is present or not */
#define __VTOR_PRESENT                 1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               2         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */

#include "core_cm0plus.h"              /* Core Peripheral Access Layer */
#include "system_SKEAZ1284.h"          /* Device specific configuration file */

/*!
 * @}
 */ /* end of group Cortex_Core_Configuration */


/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Peripheral_access_layer Device Peripheral Access Layer
 * @{
 */


/*
** Start of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #pragma push
  #pragma anon_unions
#elif defined(__CWCC__)
  #pragma push
  #pragma cpp_extensions on
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#elif defined(__ghs__) /* GreenHills */
  #else
  #error Not supported compiler type
#endif

/* ----------------------------------------------------------------------------
   -- ACMP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ACMP_Peripheral_Access_Layer ACMP Peripheral Access Layer
 * @{
 */

/** ACMP - Register Layout Typedef */
typedef struct {
  __IO uint8_t CS;                                 /**< ACMP Control and Status Register, offset: 0x0 */
  __IO uint8_t C0;                                 /**< ACMP Control Register 0, offset: 0x1 */
  __IO uint8_t C1;                                 /**< ACMP Control Register 1, offset: 0x2 */
  __IO uint8_t C2;                                 /**< ACMP Control Register 2, offset: 0x3 */
} ACMP_Type, *ACMP_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- ACMP - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ACMP_Register_Accessor_Macros ACMP - Register accessor macros
 * @{
 */


/* ACMP - Register accessors */
#define ACMP_CS_REG(base)                        ((base)->CS)
#define ACMP_C0_REG(base)                        ((base)->C0)
#define ACMP_C1_REG(base)                        ((base)->C1)
#define ACMP_C2_REG(base)                        ((base)->C2)

/*!
 * @}
 */ /* end of group ACMP_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- ACMP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ACMP_Register_Masks ACMP Register Masks
 * @{
 */

/* CS Bit Fields */
#define ACMP_CS_ACMOD_MASK                       0x3u
#define ACMP_CS_ACMOD_SHIFT                      0
#define ACMP_CS_ACMOD_WIDTH                      2
#define ACMP_CS_ACMOD(x)                         (((uint8_t)(((uint8_t)(x))<<ACMP_CS_ACMOD_SHIFT))&ACMP_CS_ACMOD_MASK)
#define ACMP_CS_ACOPE_MASK                       0x4u
#define ACMP_CS_ACOPE_SHIFT                      2
#define ACMP_CS_ACOPE_WIDTH                      1
#define ACMP_CS_ACOPE(x)                         (((uint8_t)(((uint8_t)(x))<<ACMP_CS_ACOPE_SHIFT))&ACMP_CS_ACOPE_MASK)
#define ACMP_CS_ACO_MASK                         0x8u
#define ACMP_CS_ACO_SHIFT                        3
#define ACMP_CS_ACO_WIDTH                        1
#define ACMP_CS_ACO(x)                           (((uint8_t)(((uint8_t)(x))<<ACMP_CS_ACO_SHIFT))&ACMP_CS_ACO_MASK)
#define ACMP_CS_ACIE_MASK                        0x10u
#define ACMP_CS_ACIE_SHIFT                       4
#define ACMP_CS_ACIE_WIDTH                       1
#define ACMP_CS_ACIE(x)                          (((uint8_t)(((uint8_t)(x))<<ACMP_CS_ACIE_SHIFT))&ACMP_CS_ACIE_MASK)
#define ACMP_CS_ACF_MASK                         0x20u
#define ACMP_CS_ACF_SHIFT                        5
#define ACMP_CS_ACF_WIDTH                        1
#define ACMP_CS_ACF(x)                           (((uint8_t)(((uint8_t)(x))<<ACMP_CS_ACF_SHIFT))&ACMP_CS_ACF_MASK)
#define ACMP_CS_HYST_MASK                        0x40u
#define ACMP_CS_HYST_SHIFT                       6
#define ACMP_CS_HYST_WIDTH                       1
#define ACMP_CS_HYST(x)                          (((uint8_t)(((uint8_t)(x))<<ACMP_CS_HYST_SHIFT))&ACMP_CS_HYST_MASK)
#define ACMP_CS_ACE_MASK                         0x80u
#define ACMP_CS_ACE_SHIFT                        7
#define ACMP_CS_ACE_WIDTH                        1
#define ACMP_CS_ACE(x)                           (((uint8_t)(((uint8_t)(x))<<ACMP_CS_ACE_SHIFT))&ACMP_CS_ACE_MASK)
/* C0 Bit Fields */
#define ACMP_C0_ACNSEL_MASK                      0x3u
#define ACMP_C0_ACNSEL_SHIFT                     0
#define ACMP_C0_ACNSEL_WIDTH                     2
#define ACMP_C0_ACNSEL(x)                        (((uint8_t)(((uint8_t)(x))<<ACMP_C0_ACNSEL_SHIFT))&ACMP_C0_ACNSEL_MASK)
#define ACMP_C0_ACPSEL_MASK                      0x30u
#define ACMP_C0_ACPSEL_SHIFT                     4
#define ACMP_C0_ACPSEL_WIDTH                     2
#define ACMP_C0_ACPSEL(x)                        (((uint8_t)(((uint8_t)(x))<<ACMP_C0_ACPSEL_SHIFT))&ACMP_C0_ACPSEL_MASK)
/* C1 Bit Fields */
#define ACMP_C1_DACVAL_MASK                      0x3Fu
#define ACMP_C1_DACVAL_SHIFT                     0
#define ACMP_C1_DACVAL_WIDTH                     6
#define ACMP_C1_DACVAL(x)                        (((uint8_t)(((uint8_t)(x))<<ACMP_C1_DACVAL_SHIFT))&ACMP_C1_DACVAL_MASK)
#define ACMP_C1_DACREF_MASK                      0x40u
#define ACMP_C1_DACREF_SHIFT                     6
#define ACMP_C1_DACREF_WIDTH                     1
#define ACMP_C1_DACREF(x)                        (((uint8_t)(((uint8_t)(x))<<ACMP_C1_DACREF_SHIFT))&ACMP_C1_DACREF_MASK)
#define ACMP_C1_DACEN_MASK                       0x80u
#define ACMP_C1_DACEN_SHIFT                      7
#define ACMP_C1_DACEN_WIDTH                      1
#define ACMP_C1_DACEN(x)                         (((uint8_t)(((uint8_t)(x))<<ACMP_C1_DACEN_SHIFT))&ACMP_C1_DACEN_MASK)
/* C2 Bit Fields */
#define ACMP_C2_ACIPE_MASK                       0x7u
#define ACMP_C2_ACIPE_SHIFT                      0
#define ACMP_C2_ACIPE_WIDTH                      3
#define ACMP_C2_ACIPE(x)                         (((uint8_t)(((uint8_t)(x))<<ACMP_C2_ACIPE_SHIFT))&ACMP_C2_ACIPE_MASK)

/*!
 * @}
 */ /* end of group ACMP_Register_Masks */


/* ACMP - Peripheral instance base addresses */
/** Peripheral ACMP0 base address */
#define ACMP0_BASE                               (0x40073000u)
/** Peripheral ACMP0 base pointer */
#define ACMP0                                    ((ACMP_Type *)ACMP0_BASE)
#define ACMP0_BASE_PTR                           (ACMP0)
/** Peripheral ACMP1 base address */
#define ACMP1_BASE                               (0x40074000u)
/** Peripheral ACMP1 base pointer */
#define ACMP1                                    ((ACMP_Type *)ACMP1_BASE)
#define ACMP1_BASE_PTR                           (ACMP1)
/** Array initializer of ACMP peripheral base addresses */
#define ACMP_BASE_ADDRS                          { ACMP0_BASE, ACMP1_BASE }
/** Array initializer of ACMP peripheral base pointers */
#define ACMP_BASE_PTRS                           { ACMP0, ACMP1 }

/* ----------------------------------------------------------------------------
   -- ACMP - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ACMP_Register_Accessor_Macros ACMP - Register accessor macros
 * @{
 */


/* ACMP - Register instance definitions */
/* ACMP0 */
#define ACMP0_CS                                 ACMP_CS_REG(ACMP0)
#define ACMP0_C0                                 ACMP_C0_REG(ACMP0)
#define ACMP0_C1                                 ACMP_C1_REG(ACMP0)
#define ACMP0_C2                                 ACMP_C2_REG(ACMP0)
/* ACMP1 */
#define ACMP1_CS                                 ACMP_CS_REG(ACMP1)
#define ACMP1_C0                                 ACMP_C0_REG(ACMP1)
#define ACMP1_C1                                 ACMP_C1_REG(ACMP1)
#define ACMP1_C2                                 ACMP_C2_REG(ACMP1)

/*!
 * @}
 */ /* end of group ACMP_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group ACMP_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- ADC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Peripheral_Access_Layer ADC Peripheral Access Layer
 * @{
 */

/** ADC - Register Layout Typedef */
typedef struct {
  __IO uint32_t SC1;                               /**< Status and Control Register 1, offset: 0x0 */
  __IO uint32_t SC2;                               /**< Status and Control Register 2, offset: 0x4 */
  __IO uint32_t SC3;                               /**< Status and Control Register 3, offset: 0x8 */
  __IO uint32_t SC4;                               /**< Status and Control Register 4, offset: 0xC */
  __I  uint32_t R;                                 /**< Conversion Result Register, offset: 0x10 */
  __IO uint32_t CV;                                /**< Compare Value Register, offset: 0x14 */
  __IO uint32_t APCTL1;                            /**< Pin Control 1 Register, offset: 0x18 */
  __IO uint32_t SC5;                               /**< Status and Control Register 5, offset: 0x1C */
} ADC_Type, *ADC_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- ADC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Register_Accessor_Macros ADC - Register accessor macros
 * @{
 */


/* ADC - Register accessors */
#define ADC_SC1_REG(base)                        ((base)->SC1)
#define ADC_SC2_REG(base)                        ((base)->SC2)
#define ADC_SC3_REG(base)                        ((base)->SC3)
#define ADC_SC4_REG(base)                        ((base)->SC4)
#define ADC_R_REG(base)                          ((base)->R)
#define ADC_CV_REG(base)                         ((base)->CV)
#define ADC_APCTL1_REG(base)                     ((base)->APCTL1)
#define ADC_SC5_REG(base)                        ((base)->SC5)

/*!
 * @}
 */ /* end of group ADC_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- ADC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Register_Masks ADC Register Masks
 * @{
 */

/* SC1 Bit Fields */
#define ADC_SC1_ADCH_MASK                        0x1Fu
#define ADC_SC1_ADCH_SHIFT                       0
#define ADC_SC1_ADCH_WIDTH                       5
#define ADC_SC1_ADCH(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC1_ADCH_SHIFT))&ADC_SC1_ADCH_MASK)
#define ADC_SC1_ADCO_MASK                        0x20u
#define ADC_SC1_ADCO_SHIFT                       5
#define ADC_SC1_ADCO_WIDTH                       1
#define ADC_SC1_ADCO(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC1_ADCO_SHIFT))&ADC_SC1_ADCO_MASK)
#define ADC_SC1_AIEN_MASK                        0x40u
#define ADC_SC1_AIEN_SHIFT                       6
#define ADC_SC1_AIEN_WIDTH                       1
#define ADC_SC1_AIEN(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC1_AIEN_SHIFT))&ADC_SC1_AIEN_MASK)
#define ADC_SC1_COCO_MASK                        0x80u
#define ADC_SC1_COCO_SHIFT                       7
#define ADC_SC1_COCO_WIDTH                       1
#define ADC_SC1_COCO(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC1_COCO_SHIFT))&ADC_SC1_COCO_MASK)
/* SC2 Bit Fields */
#define ADC_SC2_REFSEL_MASK                      0x3u
#define ADC_SC2_REFSEL_SHIFT                     0
#define ADC_SC2_REFSEL_WIDTH                     2
#define ADC_SC2_REFSEL(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC2_REFSEL_SHIFT))&ADC_SC2_REFSEL_MASK)
#define ADC_SC2_FFULL_MASK                       0x4u
#define ADC_SC2_FFULL_SHIFT                      2
#define ADC_SC2_FFULL_WIDTH                      1
#define ADC_SC2_FFULL(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_SC2_FFULL_SHIFT))&ADC_SC2_FFULL_MASK)
#define ADC_SC2_FEMPTY_MASK                      0x8u
#define ADC_SC2_FEMPTY_SHIFT                     3
#define ADC_SC2_FEMPTY_WIDTH                     1
#define ADC_SC2_FEMPTY(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC2_FEMPTY_SHIFT))&ADC_SC2_FEMPTY_MASK)
#define ADC_SC2_ACFGT_MASK                       0x10u
#define ADC_SC2_ACFGT_SHIFT                      4
#define ADC_SC2_ACFGT_WIDTH                      1
#define ADC_SC2_ACFGT(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_SC2_ACFGT_SHIFT))&ADC_SC2_ACFGT_MASK)
#define ADC_SC2_ACFE_MASK                        0x20u
#define ADC_SC2_ACFE_SHIFT                       5
#define ADC_SC2_ACFE_WIDTH                       1
#define ADC_SC2_ACFE(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC2_ACFE_SHIFT))&ADC_SC2_ACFE_MASK)
#define ADC_SC2_ADTRG_MASK                       0x40u
#define ADC_SC2_ADTRG_SHIFT                      6
#define ADC_SC2_ADTRG_WIDTH                      1
#define ADC_SC2_ADTRG(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_SC2_ADTRG_SHIFT))&ADC_SC2_ADTRG_MASK)
#define ADC_SC2_ADACT_MASK                       0x80u
#define ADC_SC2_ADACT_SHIFT                      7
#define ADC_SC2_ADACT_WIDTH                      1
#define ADC_SC2_ADACT(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_SC2_ADACT_SHIFT))&ADC_SC2_ADACT_MASK)
/* SC3 Bit Fields */
#define ADC_SC3_ADICLK_MASK                      0x3u
#define ADC_SC3_ADICLK_SHIFT                     0
#define ADC_SC3_ADICLK_WIDTH                     2
#define ADC_SC3_ADICLK(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC3_ADICLK_SHIFT))&ADC_SC3_ADICLK_MASK)
#define ADC_SC3_MODE_MASK                        0xCu
#define ADC_SC3_MODE_SHIFT                       2
#define ADC_SC3_MODE_WIDTH                       2
#define ADC_SC3_MODE(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC3_MODE_SHIFT))&ADC_SC3_MODE_MASK)
#define ADC_SC3_ADLSMP_MASK                      0x10u
#define ADC_SC3_ADLSMP_SHIFT                     4
#define ADC_SC3_ADLSMP_WIDTH                     1
#define ADC_SC3_ADLSMP(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC3_ADLSMP_SHIFT))&ADC_SC3_ADLSMP_MASK)
#define ADC_SC3_ADIV_MASK                        0x60u
#define ADC_SC3_ADIV_SHIFT                       5
#define ADC_SC3_ADIV_WIDTH                       2
#define ADC_SC3_ADIV(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC3_ADIV_SHIFT))&ADC_SC3_ADIV_MASK)
#define ADC_SC3_ADLPC_MASK                       0x80u
#define ADC_SC3_ADLPC_SHIFT                      7
#define ADC_SC3_ADLPC_WIDTH                      1
#define ADC_SC3_ADLPC(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_SC3_ADLPC_SHIFT))&ADC_SC3_ADLPC_MASK)
/* SC4 Bit Fields */
#define ADC_SC4_AFDEP_MASK                       0x7u
#define ADC_SC4_AFDEP_SHIFT                      0
#define ADC_SC4_AFDEP_WIDTH                      3
#define ADC_SC4_AFDEP(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_SC4_AFDEP_SHIFT))&ADC_SC4_AFDEP_MASK)
#define ADC_SC4_ACFSEL_MASK                      0x20u
#define ADC_SC4_ACFSEL_SHIFT                     5
#define ADC_SC4_ACFSEL_WIDTH                     1
#define ADC_SC4_ACFSEL(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC4_ACFSEL_SHIFT))&ADC_SC4_ACFSEL_MASK)
#define ADC_SC4_ASCANE_MASK                      0x40u
#define ADC_SC4_ASCANE_SHIFT                     6
#define ADC_SC4_ASCANE_WIDTH                     1
#define ADC_SC4_ASCANE(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC4_ASCANE_SHIFT))&ADC_SC4_ASCANE_MASK)
#define ADC_SC4_HTRGME_MASK                      0x100u
#define ADC_SC4_HTRGME_SHIFT                     8
#define ADC_SC4_HTRGME_WIDTH                     1
#define ADC_SC4_HTRGME(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC4_HTRGME_SHIFT))&ADC_SC4_HTRGME_MASK)
/* R Bit Fields */
#define ADC_R_ADR_MASK                           0xFFFu
#define ADC_R_ADR_SHIFT                          0
#define ADC_R_ADR_WIDTH                          12
#define ADC_R_ADR(x)                             (((uint32_t)(((uint32_t)(x))<<ADC_R_ADR_SHIFT))&ADC_R_ADR_MASK)
/* CV Bit Fields */
#define ADC_CV_CV_MASK                           0xFFFu
#define ADC_CV_CV_SHIFT                          0
#define ADC_CV_CV_WIDTH                          12
#define ADC_CV_CV(x)                             (((uint32_t)(((uint32_t)(x))<<ADC_CV_CV_SHIFT))&ADC_CV_CV_MASK)
/* APCTL1 Bit Fields */
#define ADC_APCTL1_ADPC_MASK                     0xFFFFu
#define ADC_APCTL1_ADPC_SHIFT                    0
#define ADC_APCTL1_ADPC_WIDTH                    16
#define ADC_APCTL1_ADPC(x)                       (((uint32_t)(((uint32_t)(x))<<ADC_APCTL1_ADPC_SHIFT))&ADC_APCTL1_ADPC_MASK)
/* SC5 Bit Fields */
#define ADC_SC5_HTRGMASKSEL_MASK                 0x1u
#define ADC_SC5_HTRGMASKSEL_SHIFT                0
#define ADC_SC5_HTRGMASKSEL_WIDTH                1
#define ADC_SC5_HTRGMASKSEL(x)                   (((uint32_t)(((uint32_t)(x))<<ADC_SC5_HTRGMASKSEL_SHIFT))&ADC_SC5_HTRGMASKSEL_MASK)
#define ADC_SC5_HTRGMASKE_MASK                   0x2u
#define ADC_SC5_HTRGMASKE_SHIFT                  1
#define ADC_SC5_HTRGMASKE_WIDTH                  1
#define ADC_SC5_HTRGMASKE(x)                     (((uint32_t)(((uint32_t)(x))<<ADC_SC5_HTRGMASKE_SHIFT))&ADC_SC5_HTRGMASKE_MASK)

/*!
 * @}
 */ /* end of group ADC_Register_Masks */


/* ADC - Peripheral instance base addresses */
/** Peripheral ADC base address */
#define ADC_BASE                                 (0x4003B000u)
/** Peripheral ADC base pointer */
#define ADC                                      ((ADC_Type *)ADC_BASE)
#define ADC_BASE_PTR                             (ADC)
/** Array initializer of ADC peripheral base addresses */
#define ADC_BASE_ADDRS                           { ADC_BASE }
/** Array initializer of ADC peripheral base pointers */
#define ADC_BASE_PTRS                            { ADC }

/* ----------------------------------------------------------------------------
   -- ADC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Register_Accessor_Macros ADC - Register accessor macros
 * @{
 */


/* ADC - Register instance definitions */
/* ADC */
#define ADC_SC1                                  ADC_SC1_REG(ADC)
#define ADC_SC2                                  ADC_SC2_REG(ADC)
#define ADC_SC3                                  ADC_SC3_REG(ADC)
#define ADC_SC4                                  ADC_SC4_REG(ADC)
#define ADC_R                                    ADC_R_REG(ADC)
#define ADC_CV                                   ADC_CV_REG(ADC)
#define ADC_APCTL1                               ADC_APCTL1_REG(ADC)
#define ADC_SC5                                  ADC_SC5_REG(ADC)

/*!
 * @}
 */ /* end of group ADC_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group ADC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CRC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Peripheral_Access_Layer CRC Peripheral Access Layer
 * @{
 */

/** CRC - Register Layout Typedef */
typedef struct {
  union {                                          /* offset: 0x0 */
    struct {                                         /* offset: 0x0 */
      __IO uint16_t DATAL;                             /**< CRC_DATAL register., offset: 0x0 */
      __IO uint16_t DATAH;                             /**< CRC_DATAH register., offset: 0x2 */
    } ACCESS16BIT;
    __IO uint32_t DATA;                              /**< CRC Data register, offset: 0x0 */
    struct {                                         /* offset: 0x0 */
      __IO uint8_t DATALL;                             /**< CRC_DATALL register., offset: 0x0 */
      __IO uint8_t DATALU;                             /**< CRC_DATALU register., offset: 0x1 */
      __IO uint8_t DATAHL;                             /**< CRC_DATAHL register., offset: 0x2 */
      __IO uint8_t DATAHU;                             /**< CRC_DATAHU register., offset: 0x3 */
    } ACCESS8BIT;
  };
  union {                                          /* offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint16_t GPOLYL;                            /**< CRC_GPOLYL register., offset: 0x4 */
      __IO uint16_t GPOLYH;                            /**< CRC_GPOLYH register., offset: 0x6 */
    } GPOLY_ACCESS16BIT;
    __IO uint32_t GPOLY;                             /**< CRC Polynomial register, offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint8_t GPOLYLL;                            /**< CRC_GPOLYLL register., offset: 0x4 */
      __IO uint8_t GPOLYLU;                            /**< CRC_GPOLYLU register., offset: 0x5 */
      __IO uint8_t GPOLYHL;                            /**< CRC_GPOLYHL register., offset: 0x6 */
      __IO uint8_t GPOLYHU;                            /**< CRC_GPOLYHU register., offset: 0x7 */
    } GPOLY_ACCESS8BIT;
  };
  union {                                          /* offset: 0x8 */
    __IO uint32_t CTRL;                              /**< CRC Control register, offset: 0x8 */
    struct {                                         /* offset: 0x8 */
           uint8_t RESERVED_0[3];
      __IO uint8_t CTRLHU;                             /**< CRC_CTRLHU register., offset: 0xB */
    } CTRL_ACCESS8BIT;
  };
} CRC_Type, *CRC_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- CRC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Register_Accessor_Macros CRC - Register accessor macros
 * @{
 */


/* CRC - Register accessors */
#define CRC_DATAL_REG(base)                      ((base)->ACCESS16BIT.DATAL)
#define CRC_DATAH_REG(base)                      ((base)->ACCESS16BIT.DATAH)
#define CRC_DATA_REG(base)                       ((base)->DATA)
#define CRC_DATALL_REG(base)                     ((base)->ACCESS8BIT.DATALL)
#define CRC_DATALU_REG(base)                     ((base)->ACCESS8BIT.DATALU)
#define CRC_DATAHL_REG(base)                     ((base)->ACCESS8BIT.DATAHL)
#define CRC_DATAHU_REG(base)                     ((base)->ACCESS8BIT.DATAHU)
#define CRC_GPOLYL_REG(base)                     ((base)->GPOLY_ACCESS16BIT.GPOLYL)
#define CRC_GPOLYH_REG(base)                     ((base)->GPOLY_ACCESS16BIT.GPOLYH)
#define CRC_GPOLY_REG(base)                      ((base)->GPOLY)
#define CRC_GPOLYLL_REG(base)                    ((base)->GPOLY_ACCESS8BIT.GPOLYLL)
#define CRC_GPOLYLU_REG(base)                    ((base)->GPOLY_ACCESS8BIT.GPOLYLU)
#define CRC_GPOLYHL_REG(base)                    ((base)->GPOLY_ACCESS8BIT.GPOLYHL)
#define CRC_GPOLYHU_REG(base)                    ((base)->GPOLY_ACCESS8BIT.GPOLYHU)
#define CRC_CTRL_REG(base)                       ((base)->CTRL)
#define CRC_CTRLHU_REG(base)                     ((base)->CTRL_ACCESS8BIT.CTRLHU)

/*!
 * @}
 */ /* end of group CRC_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- CRC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Register_Masks CRC Register Masks
 * @{
 */

/* DATAL Bit Fields */
#define CRC_DATAL_DATAL_MASK                     0xFFFFu
#define CRC_DATAL_DATAL_SHIFT                    0
#define CRC_DATAL_DATAL_WIDTH                    16
#define CRC_DATAL_DATAL(x)                       (((uint16_t)(((uint16_t)(x))<<CRC_DATAL_DATAL_SHIFT))&CRC_DATAL_DATAL_MASK)
/* DATAH Bit Fields */
#define CRC_DATAH_DATAH_MASK                     0xFFFFu
#define CRC_DATAH_DATAH_SHIFT                    0
#define CRC_DATAH_DATAH_WIDTH                    16
#define CRC_DATAH_DATAH(x)                       (((uint16_t)(((uint16_t)(x))<<CRC_DATAH_DATAH_SHIFT))&CRC_DATAH_DATAH_MASK)
/* DATA Bit Fields */
#define CRC_DATA_LL_MASK                         0xFFu
#define CRC_DATA_LL_SHIFT                        0
#define CRC_DATA_LL_WIDTH                        8
#define CRC_DATA_LL(x)                           (((uint32_t)(((uint32_t)(x))<<CRC_DATA_LL_SHIFT))&CRC_DATA_LL_MASK)
#define CRC_DATA_LU_MASK                         0xFF00u
#define CRC_DATA_LU_SHIFT                        8
#define CRC_DATA_LU_WIDTH                        8
#define CRC_DATA_LU(x)                           (((uint32_t)(((uint32_t)(x))<<CRC_DATA_LU_SHIFT))&CRC_DATA_LU_MASK)
#define CRC_DATA_HL_MASK                         0xFF0000u
#define CRC_DATA_HL_SHIFT                        16
#define CRC_DATA_HL_WIDTH                        8
#define CRC_DATA_HL(x)                           (((uint32_t)(((uint32_t)(x))<<CRC_DATA_HL_SHIFT))&CRC_DATA_HL_MASK)
#define CRC_DATA_HU_MASK                         0xFF000000u
#define CRC_DATA_HU_SHIFT                        24
#define CRC_DATA_HU_WIDTH                        8
#define CRC_DATA_HU(x)                           (((uint32_t)(((uint32_t)(x))<<CRC_DATA_HU_SHIFT))&CRC_DATA_HU_MASK)
/* DATALL Bit Fields */
#define CRC_DATALL_DATALL_MASK                   0xFFu
#define CRC_DATALL_DATALL_SHIFT                  0
#define CRC_DATALL_DATALL_WIDTH                  8
#define CRC_DATALL_DATALL(x)                     (((uint8_t)(((uint8_t)(x))<<CRC_DATALL_DATALL_SHIFT))&CRC_DATALL_DATALL_MASK)
/* DATALU Bit Fields */
#define CRC_DATALU_DATALU_MASK                   0xFFu
#define CRC_DATALU_DATALU_SHIFT                  0
#define CRC_DATALU_DATALU_WIDTH                  8
#define CRC_DATALU_DATALU(x)                     (((uint8_t)(((uint8_t)(x))<<CRC_DATALU_DATALU_SHIFT))&CRC_DATALU_DATALU_MASK)
/* DATAHL Bit Fields */
#define CRC_DATAHL_DATAHL_MASK                   0xFFu
#define CRC_DATAHL_DATAHL_SHIFT                  0
#define CRC_DATAHL_DATAHL_WIDTH                  8
#define CRC_DATAHL_DATAHL(x)                     (((uint8_t)(((uint8_t)(x))<<CRC_DATAHL_DATAHL_SHIFT))&CRC_DATAHL_DATAHL_MASK)
/* DATAHU Bit Fields */
#define CRC_DATAHU_DATAHU_MASK                   0xFFu
#define CRC_DATAHU_DATAHU_SHIFT                  0
#define CRC_DATAHU_DATAHU_WIDTH                  8
#define CRC_DATAHU_DATAHU(x)                     (((uint8_t)(((uint8_t)(x))<<CRC_DATAHU_DATAHU_SHIFT))&CRC_DATAHU_DATAHU_MASK)
/* GPOLYL Bit Fields */
#define CRC_GPOLYL_GPOLYL_MASK                   0xFFFFu
#define CRC_GPOLYL_GPOLYL_SHIFT                  0
#define CRC_GPOLYL_GPOLYL_WIDTH                  16
#define CRC_GPOLYL_GPOLYL(x)                     (((uint16_t)(((uint16_t)(x))<<CRC_GPOLYL_GPOLYL_SHIFT))&CRC_GPOLYL_GPOLYL_MASK)
/* GPOLYH Bit Fields */
#define CRC_GPOLYH_GPOLYH_MASK                   0xFFFFu
#define CRC_GPOLYH_GPOLYH_SHIFT                  0
#define CRC_GPOLYH_GPOLYH_WIDTH                  16
#define CRC_GPOLYH_GPOLYH(x)                     (((uint16_t)(((uint16_t)(x))<<CRC_GPOLYH_GPOLYH_SHIFT))&CRC_GPOLYH_GPOLYH_MASK)
/* GPOLY Bit Fields */
#define CRC_GPOLY_LOW_MASK                       0xFFFFu
#define CRC_GPOLY_LOW_SHIFT                      0
#define CRC_GPOLY_LOW_WIDTH                      16
#define CRC_GPOLY_LOW(x)                         (((uint32_t)(((uint32_t)(x))<<CRC_GPOLY_LOW_SHIFT))&CRC_GPOLY_LOW_MASK)
#define CRC_GPOLY_HIGH_MASK                      0xFFFF0000u
#define CRC_GPOLY_HIGH_SHIFT                     16
#define CRC_GPOLY_HIGH_WIDTH                     16
#define CRC_GPOLY_HIGH(x)                        (((uint32_t)(((uint32_t)(x))<<CRC_GPOLY_HIGH_SHIFT))&CRC_GPOLY_HIGH_MASK)
/* GPOLYLL Bit Fields */
#define CRC_GPOLYLL_GPOLYLL_MASK                 0xFFu
#define CRC_GPOLYLL_GPOLYLL_SHIFT                0
#define CRC_GPOLYLL_GPOLYLL_WIDTH                8
#define CRC_GPOLYLL_GPOLYLL(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYLL_GPOLYLL_SHIFT))&CRC_GPOLYLL_GPOLYLL_MASK)
/* GPOLYLU Bit Fields */
#define CRC_GPOLYLU_GPOLYLU_MASK                 0xFFu
#define CRC_GPOLYLU_GPOLYLU_SHIFT                0
#define CRC_GPOLYLU_GPOLYLU_WIDTH                8
#define CRC_GPOLYLU_GPOLYLU(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYLU_GPOLYLU_SHIFT))&CRC_GPOLYLU_GPOLYLU_MASK)
/* GPOLYHL Bit Fields */
#define CRC_GPOLYHL_GPOLYHL_MASK                 0xFFu
#define CRC_GPOLYHL_GPOLYHL_SHIFT                0
#define CRC_GPOLYHL_GPOLYHL_WIDTH                8
#define CRC_GPOLYHL_GPOLYHL(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYHL_GPOLYHL_SHIFT))&CRC_GPOLYHL_GPOLYHL_MASK)
/* GPOLYHU Bit Fields */
#define CRC_GPOLYHU_GPOLYHU_MASK                 0xFFu
#define CRC_GPOLYHU_GPOLYHU_SHIFT                0
#define CRC_GPOLYHU_GPOLYHU_WIDTH                8
#define CRC_GPOLYHU_GPOLYHU(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYHU_GPOLYHU_SHIFT))&CRC_GPOLYHU_GPOLYHU_MASK)
/* CTRL Bit Fields */
#define CRC_CTRL_TCRC_MASK                       0x1000000u
#define CRC_CTRL_TCRC_SHIFT                      24
#define CRC_CTRL_TCRC_WIDTH                      1
#define CRC_CTRL_TCRC(x)                         (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_TCRC_SHIFT))&CRC_CTRL_TCRC_MASK)
#define CRC_CTRL_WAS_MASK                        0x2000000u
#define CRC_CTRL_WAS_SHIFT                       25
#define CRC_CTRL_WAS_WIDTH                       1
#define CRC_CTRL_WAS(x)                          (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_WAS_SHIFT))&CRC_CTRL_WAS_MASK)
#define CRC_CTRL_FXOR_MASK                       0x4000000u
#define CRC_CTRL_FXOR_SHIFT                      26
#define CRC_CTRL_FXOR_WIDTH                      1
#define CRC_CTRL_FXOR(x)                         (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_FXOR_SHIFT))&CRC_CTRL_FXOR_MASK)
#define CRC_CTRL_TOTR_MASK                       0x30000000u
#define CRC_CTRL_TOTR_SHIFT                      28
#define CRC_CTRL_TOTR_WIDTH                      2
#define CRC_CTRL_TOTR(x)                         (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_TOTR_SHIFT))&CRC_CTRL_TOTR_MASK)
#define CRC_CTRL_TOT_MASK                        0xC0000000u
#define CRC_CTRL_TOT_SHIFT                       30
#define CRC_CTRL_TOT_WIDTH                       2
#define CRC_CTRL_TOT(x)                          (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_TOT_SHIFT))&CRC_CTRL_TOT_MASK)
/* CTRLHU Bit Fields */
#define CRC_CTRLHU_TCRC_MASK                     0x1u
#define CRC_CTRLHU_TCRC_SHIFT                    0
#define CRC_CTRLHU_TCRC_WIDTH                    1
#define CRC_CTRLHU_TCRC(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_TCRC_SHIFT))&CRC_CTRLHU_TCRC_MASK)
#define CRC_CTRLHU_WAS_MASK                      0x2u
#define CRC_CTRLHU_WAS_SHIFT                     1
#define CRC_CTRLHU_WAS_WIDTH                     1
#define CRC_CTRLHU_WAS(x)                        (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_WAS_SHIFT))&CRC_CTRLHU_WAS_MASK)
#define CRC_CTRLHU_FXOR_MASK                     0x4u
#define CRC_CTRLHU_FXOR_SHIFT                    2
#define CRC_CTRLHU_FXOR_WIDTH                    1
#define CRC_CTRLHU_FXOR(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_FXOR_SHIFT))&CRC_CTRLHU_FXOR_MASK)
#define CRC_CTRLHU_TOTR_MASK                     0x30u
#define CRC_CTRLHU_TOTR_SHIFT                    4
#define CRC_CTRLHU_TOTR_WIDTH                    2
#define CRC_CTRLHU_TOTR(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_TOTR_SHIFT))&CRC_CTRLHU_TOTR_MASK)
#define CRC_CTRLHU_TOT_MASK                      0xC0u
#define CRC_CTRLHU_TOT_SHIFT                     6
#define CRC_CTRLHU_TOT_WIDTH                     2
#define CRC_CTRLHU_TOT(x)                        (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_TOT_SHIFT))&CRC_CTRLHU_TOT_MASK)

/*!
 * @}
 */ /* end of group CRC_Register_Masks */


/* CRC - Peripheral instance base addresses */
/** Peripheral CRC base address */
#define CRC_BASE                                 (0x40032000u)
/** Peripheral CRC base pointer */
#define CRC0                                     ((CRC_Type *)CRC_BASE)
#define CRC_BASE_PTR                             (CRC0)
/** Array initializer of CRC peripheral base addresses */
#define CRC_BASE_ADDRS                           { CRC_BASE }
/** Array initializer of CRC peripheral base pointers */
#define CRC_BASE_PTRS                            { CRC0 }

/* ----------------------------------------------------------------------------
   -- CRC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Register_Accessor_Macros CRC - Register accessor macros
 * @{
 */


/* CRC - Register instance definitions */
/* CRC */
#define CRC_DATA                                 CRC_DATA_REG(CRC0)
#define CRC_DATAL                                CRC_DATAL_REG(CRC0)
#define CRC_DATALL                               CRC_DATALL_REG(CRC0)
#define CRC_DATALU                               CRC_DATALU_REG(CRC0)
#define CRC_DATAH                                CRC_DATAH_REG(CRC0)
#define CRC_DATAHL                               CRC_DATAHL_REG(CRC0)
#define CRC_DATAHU                               CRC_DATAHU_REG(CRC0)
#define CRC_GPOLY                                CRC_GPOLY_REG(CRC0)
#define CRC_GPOLYL                               CRC_GPOLYL_REG(CRC0)
#define CRC_GPOLYLL                              CRC_GPOLYLL_REG(CRC0)
#define CRC_GPOLYLU                              CRC_GPOLYLU_REG(CRC0)
#define CRC_GPOLYH                               CRC_GPOLYH_REG(CRC0)
#define CRC_GPOLYHL                              CRC_GPOLYHL_REG(CRC0)
#define CRC_GPOLYHU                              CRC_GPOLYHU_REG(CRC0)
#define CRC_CTRL                                 CRC_CTRL_REG(CRC0)
#define CRC_CTRLHU                               CRC_CTRLHU_REG(CRC0)

/*!
 * @}
 */ /* end of group CRC_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group CRC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FGPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Peripheral_Access_Layer FGPIO Peripheral Access Layer
 * @{
 */

/** FGPIO - Register Layout Typedef */
typedef struct {
  __IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
  __O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
  __O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
  __IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
  __IO uint32_t PIDR;                              /**< Port Input Disable Register, offset: 0x18 */
} FGPIO_Type, *FGPIO_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- FGPIO - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Accessor_Macros FGPIO - Register accessor macros
 * @{
 */


/* FGPIO - Register accessors */
#define FGPIO_PDOR_REG(base)                     ((base)->PDOR)
#define FGPIO_PSOR_REG(base)                     ((base)->PSOR)
#define FGPIO_PCOR_REG(base)                     ((base)->PCOR)
#define FGPIO_PTOR_REG(base)                     ((base)->PTOR)
#define FGPIO_PDIR_REG(base)                     ((base)->PDIR)
#define FGPIO_PDDR_REG(base)                     ((base)->PDDR)
#define FGPIO_PIDR_REG(base)                     ((base)->PIDR)

/*!
 * @}
 */ /* end of group FGPIO_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- FGPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Masks FGPIO Register Masks
 * @{
 */

/* PDOR Bit Fields */
#define FGPIO_PDOR_PDO_MASK                      0xFFFFFFFFu
#define FGPIO_PDOR_PDO_SHIFT                     0
#define FGPIO_PDOR_PDO_WIDTH                     32
#define FGPIO_PDOR_PDO(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PDOR_PDO_SHIFT))&FGPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define FGPIO_PSOR_PTSO_MASK                     0xFFFFFFFFu
#define FGPIO_PSOR_PTSO_SHIFT                    0
#define FGPIO_PSOR_PTSO_WIDTH                    32
#define FGPIO_PSOR_PTSO(x)                       (((uint32_t)(((uint32_t)(x))<<FGPIO_PSOR_PTSO_SHIFT))&FGPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define FGPIO_PCOR_PTCO_MASK                     0xFFFFFFFFu
#define FGPIO_PCOR_PTCO_SHIFT                    0
#define FGPIO_PCOR_PTCO_WIDTH                    32
#define FGPIO_PCOR_PTCO(x)                       (((uint32_t)(((uint32_t)(x))<<FGPIO_PCOR_PTCO_SHIFT))&FGPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define FGPIO_PTOR_PTTO_MASK                     0xFFFFFFFFu
#define FGPIO_PTOR_PTTO_SHIFT                    0
#define FGPIO_PTOR_PTTO_WIDTH                    32
#define FGPIO_PTOR_PTTO(x)                       (((uint32_t)(((uint32_t)(x))<<FGPIO_PTOR_PTTO_SHIFT))&FGPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define FGPIO_PDIR_PDI_MASK                      0xFFFFFFFFu
#define FGPIO_PDIR_PDI_SHIFT                     0
#define FGPIO_PDIR_PDI_WIDTH                     32
#define FGPIO_PDIR_PDI(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PDIR_PDI_SHIFT))&FGPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define FGPIO_PDDR_PDD_MASK                      0xFFFFFFFFu
#define FGPIO_PDDR_PDD_SHIFT                     0
#define FGPIO_PDDR_PDD_WIDTH                     32
#define FGPIO_PDDR_PDD(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PDDR_PDD_SHIFT))&FGPIO_PDDR_PDD_MASK)
/* PIDR Bit Fields */
#define FGPIO_PIDR_PID_MASK                      0xFFFFFFFFu
#define FGPIO_PIDR_PID_SHIFT                     0
#define FGPIO_PIDR_PID_WIDTH                     32
#define FGPIO_PIDR_PID(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PIDR_PID_SHIFT))&FGPIO_PIDR_PID_MASK)

/*!
 * @}
 */ /* end of group FGPIO_Register_Masks */


/* FGPIO - Peripheral instance base addresses */
/** Peripheral FGPIOA base address */
#define FGPIOA_BASE                              (0xF8000000u)
/** Peripheral FGPIOA base pointer */
#define FGPIOA                                   ((FGPIO_Type *)FGPIOA_BASE)
#define FGPIOA_BASE_PTR                          (FGPIOA)
/** Peripheral FGPIOB base address */
#define FGPIOB_BASE                              (0xF8000040u)
/** Peripheral FGPIOB base pointer */
#define FGPIOB                                   ((FGPIO_Type *)FGPIOB_BASE)
#define FGPIOB_BASE_PTR                          (FGPIOB)
/** Peripheral FGPIOC base address */
#define FGPIOC_BASE                              (0xF8000080u)
/** Peripheral FGPIOC base pointer */
#define FGPIOC                                   ((FGPIO_Type *)FGPIOC_BASE)
#define FGPIOC_BASE_PTR                          (FGPIOC)
/** Array initializer of FGPIO peripheral base addresses */
#define FGPIO_BASE_ADDRS                         { FGPIOA_BASE, FGPIOB_BASE, FGPIOC_BASE }
/** Array initializer of FGPIO peripheral base pointers */
#define FGPIO_BASE_PTRS                          { FGPIOA, FGPIOB, FGPIOC }

/* ----------------------------------------------------------------------------
   -- FGPIO - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Accessor_Macros FGPIO - Register accessor macros
 * @{
 */


/* FGPIO - Register instance definitions */
/* FGPIOA */
#define FGPIOA_PDOR                              FGPIO_PDOR_REG(FGPIOA)
#define FGPIOA_PSOR                              FGPIO_PSOR_REG(FGPIOA)
#define FGPIOA_PCOR                              FGPIO_PCOR_REG(FGPIOA)
#define FGPIOA_PTOR                              FGPIO_PTOR_REG(FGPIOA)
#define FGPIOA_PDIR                              FGPIO_PDIR_REG(FGPIOA)
#define FGPIOA_PDDR                              FGPIO_PDDR_REG(FGPIOA)
#define FGPIOA_PIDR                              FGPIO_PIDR_REG(FGPIOA)
/* FGPIOB */
#define FGPIOB_PDOR                              FGPIO_PDOR_REG(FGPIOB)
#define FGPIOB_PSOR                              FGPIO_PSOR_REG(FGPIOB)
#define FGPIOB_PCOR                              FGPIO_PCOR_REG(FGPIOB)
#define FGPIOB_PTOR                              FGPIO_PTOR_REG(FGPIOB)
#define FGPIOB_PDIR                              FGPIO_PDIR_REG(FGPIOB)
#define FGPIOB_PDDR                              FGPIO_PDDR_REG(FGPIOB)
#define FGPIOB_PIDR                              FGPIO_PIDR_REG(FGPIOB)
/* FGPIOC */
#define FGPIOC_PDOR                              FGPIO_PDOR_REG(FGPIOC)
#define FGPIOC_PSOR                              FGPIO_PSOR_REG(FGPIOC)
#define FGPIOC_PCOR                              FGPIO_PCOR_REG(FGPIOC)
#define FGPIOC_PTOR                              FGPIO_PTOR_REG(FGPIOC)
#define FGPIOC_PDIR                              FGPIO_PDIR_REG(FGPIOC)
#define FGPIOC_PDDR                              FGPIO_PDDR_REG(FGPIOC)
#define FGPIOC_PIDR                              FGPIO_PIDR_REG(FGPIOC)

/*!
 * @}
 */ /* end of group FGPIO_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group FGPIO_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FTM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTM_Peripheral_Access_Layer FTM Peripheral Access Layer
 * @{
 */

/** FTM - Register Layout Typedef */
typedef struct {
  __IO uint32_t SC;                                /**< Status And Control, offset: 0x0 */
  __IO uint32_t CNT;                               /**< Counter, offset: 0x4 */
  __IO uint32_t MOD;                               /**< Modulo, offset: 0x8 */
  struct {                                         /* offset: 0xC, array step: 0x8 */
    __IO uint32_t CnSC;                              /**< Channel (n) Status And Control, array offset: 0xC, array step: 0x8 */
    __IO uint32_t CnV;                               /**< Channel (n) Value, array offset: 0x10, array step: 0x8 */
  } CONTROLS[6];
       uint8_t RESERVED_0[16];
  __IO uint32_t CNTIN;                             /**< Counter Initial Value, offset: 0x4C */
  __IO uint32_t STATUS;                            /**< Capture And Compare Status, offset: 0x50 */
  __IO uint32_t MODE;                              /**< Features Mode Selection, offset: 0x54 */
  __IO uint32_t SYNC;                              /**< Synchronization, offset: 0x58 */
  __IO uint32_t OUTINIT;                           /**< Initial State For Channels Output, offset: 0x5C */
  __IO uint32_t OUTMASK;                           /**< Output Mask, offset: 0x60 */
  __IO uint32_t COMBINE;                           /**< Function For Linked Channels, offset: 0x64 */
  __IO uint32_t DEADTIME;                          /**< Deadtime Insertion Control, offset: 0x68 */
  __IO uint32_t EXTTRIG;                           /**< FTM External Trigger, offset: 0x6C */
  __IO uint32_t POL;                               /**< Channels Polarity, offset: 0x70 */
  __IO uint32_t FMS;                               /**< Fault Mode Status, offset: 0x74 */
  __IO uint32_t FILTER;                            /**< Input Capture Filter Control, offset: 0x78 */
  __IO uint32_t FLTCTRL;                           /**< Fault Control, offset: 0x7C */
       uint8_t RESERVED_1[4];
  __IO uint32_t CONF;                              /**< Configuration, offset: 0x84 */
  __IO uint32_t FLTPOL;                            /**< FTM Fault Input Polarity, offset: 0x88 */
  __IO uint32_t SYNCONF;                           /**< Synchronization Configuration, offset: 0x8C */
  __IO uint32_t INVCTRL;                           /**< FTM Inverting Control, offset: 0x90 */
  __IO uint32_t SWOCTRL;                           /**< FTM Software Output Control, offset: 0x94 */
  __IO uint32_t PWMLOAD;                           /**< FTM PWM Load, offset: 0x98 */
} FTM_Type, *FTM_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- FTM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTM_Register_Accessor_Macros FTM - Register accessor macros
 * @{
 */


/* FTM - Register accessors */
#define FTM_SC_REG(base)                         ((base)->SC)
#define FTM_CNT_REG(base)                        ((base)->CNT)
#define FTM_MOD_REG(base)                        ((base)->MOD)
#define FTM_CnSC_REG(base,index)                 ((base)->CONTROLS[index].CnSC)
#define FTM_CnSC_COUNT                           6
#define FTM_CnV_REG(base,index)                  ((base)->CONTROLS[index].CnV)
#define FTM_CnV_COUNT                            6
#define FTM_CNTIN_REG(base)                      ((base)->CNTIN)
#define FTM_STATUS_REG(base)                     ((base)->STATUS)
#define FTM_MODE_REG(base)                       ((base)->MODE)
#define FTM_SYNC_REG(base)                       ((base)->SYNC)
#define FTM_OUTINIT_REG(base)                    ((base)->OUTINIT)
#define FTM_OUTMASK_REG(base)                    ((base)->OUTMASK)
#define FTM_COMBINE_REG(base)                    ((base)->COMBINE)
#define FTM_DEADTIME_REG(base)                   ((base)->DEADTIME)
#define FTM_EXTTRIG_REG(base)                    ((base)->EXTTRIG)
#define FTM_POL_REG(base)                        ((base)->POL)
#define FTM_FMS_REG(base)                        ((base)->FMS)
#define FTM_FILTER_REG(base)                     ((base)->FILTER)
#define FTM_FLTCTRL_REG(base)                    ((base)->FLTCTRL)
#define FTM_CONF_REG(base)                       ((base)->CONF)
#define FTM_FLTPOL_REG(base)                     ((base)->FLTPOL)
#define FTM_SYNCONF_REG(base)                    ((base)->SYNCONF)
#define FTM_INVCTRL_REG(base)                    ((base)->INVCTRL)
#define FTM_SWOCTRL_REG(base)                    ((base)->SWOCTRL)
#define FTM_PWMLOAD_REG(base)                    ((base)->PWMLOAD)

/*!
 * @}
 */ /* end of group FTM_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- FTM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTM_Register_Masks FTM Register Masks
 * @{
 */

/* SC Bit Fields */
#define FTM_SC_PS_MASK                           0x7u
#define FTM_SC_PS_SHIFT                          0
#define FTM_SC_PS_WIDTH                          3
#define FTM_SC_PS(x)                             (((uint32_t)(((uint32_t)(x))<<FTM_SC_PS_SHIFT))&FTM_SC_PS_MASK)
#define FTM_SC_CLKS_MASK                         0x18u
#define FTM_SC_CLKS_SHIFT                        3
#define FTM_SC_CLKS_WIDTH                        2
#define FTM_SC_CLKS(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_SC_CLKS_SHIFT))&FTM_SC_CLKS_MASK)
#define FTM_SC_CPWMS_MASK                        0x20u
#define FTM_SC_CPWMS_SHIFT                       5
#define FTM_SC_CPWMS_WIDTH                       1
#define FTM_SC_CPWMS(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_SC_CPWMS_SHIFT))&FTM_SC_CPWMS_MASK)
#define FTM_SC_TOIE_MASK                         0x40u
#define FTM_SC_TOIE_SHIFT                        6
#define FTM_SC_TOIE_WIDTH                        1
#define FTM_SC_TOIE(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_SC_TOIE_SHIFT))&FTM_SC_TOIE_MASK)
#define FTM_SC_TOF_MASK                          0x80u
#define FTM_SC_TOF_SHIFT                         7
#define FTM_SC_TOF_WIDTH                         1
#define FTM_SC_TOF(x)                            (((uint32_t)(((uint32_t)(x))<<FTM_SC_TOF_SHIFT))&FTM_SC_TOF_MASK)
/* CNT Bit Fields */
#define FTM_CNT_COUNT_MASK                       0xFFFFu
#define FTM_CNT_COUNT_SHIFT                      0
#define FTM_CNT_COUNT_WIDTH                      16
#define FTM_CNT_COUNT(x)                         (((uint32_t)(((uint32_t)(x))<<FTM_CNT_COUNT_SHIFT))&FTM_CNT_COUNT_MASK)
/* MOD Bit Fields */
#define FTM_MOD_MOD_MASK                         0xFFFFu
#define FTM_MOD_MOD_SHIFT                        0
#define FTM_MOD_MOD_WIDTH                        16
#define FTM_MOD_MOD(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_MOD_MOD_SHIFT))&FTM_MOD_MOD_MASK)
/* CnSC Bit Fields */
#define FTM_CnSC_ELSA_MASK                       0x4u
#define FTM_CnSC_ELSA_SHIFT                      2
#define FTM_CnSC_ELSA_WIDTH                      1
#define FTM_CnSC_ELSA(x)                         (((uint32_t)(((uint32_t)(x))<<FTM_CnSC_ELSA_SHIFT))&FTM_CnSC_ELSA_MASK)
#define FTM_CnSC_ELSB_MASK                       0x8u
#define FTM_CnSC_ELSB_SHIFT                      3
#define FTM_CnSC_ELSB_WIDTH                      1
#define FTM_CnSC_ELSB(x)                         (((uint32_t)(((uint32_t)(x))<<FTM_CnSC_ELSB_SHIFT))&FTM_CnSC_ELSB_MASK)
#define FTM_CnSC_MSA_MASK                        0x10u
#define FTM_CnSC_MSA_SHIFT                       4
#define FTM_CnSC_MSA_WIDTH                       1
#define FTM_CnSC_MSA(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_CnSC_MSA_SHIFT))&FTM_CnSC_MSA_MASK)
#define FTM_CnSC_MSB_MASK                        0x20u
#define FTM_CnSC_MSB_SHIFT                       5
#define FTM_CnSC_MSB_WIDTH                       1
#define FTM_CnSC_MSB(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_CnSC_MSB_SHIFT))&FTM_CnSC_MSB_MASK)
#define FTM_CnSC_CHIE_MASK                       0x40u
#define FTM_CnSC_CHIE_SHIFT                      6
#define FTM_CnSC_CHIE_WIDTH                      1
#define FTM_CnSC_CHIE(x)                         (((uint32_t)(((uint32_t)(x))<<FTM_CnSC_CHIE_SHIFT))&FTM_CnSC_CHIE_MASK)
#define FTM_CnSC_CHF_MASK                        0x80u
#define FTM_CnSC_CHF_SHIFT                       7
#define FTM_CnSC_CHF_WIDTH                       1
#define FTM_CnSC_CHF(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_CnSC_CHF_SHIFT))&FTM_CnSC_CHF_MASK)
/* CnV Bit Fields */
#define FTM_CnV_VAL_MASK                         0xFFFFu
#define FTM_CnV_VAL_SHIFT                        0
#define FTM_CnV_VAL_WIDTH                        16
#define FTM_CnV_VAL(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_CnV_VAL_SHIFT))&FTM_CnV_VAL_MASK)
/* CNTIN Bit Fields */
#define FTM_CNTIN_INIT_MASK                      0xFFFFu
#define FTM_CNTIN_INIT_SHIFT                     0
#define FTM_CNTIN_INIT_WIDTH                     16
#define FTM_CNTIN_INIT(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_CNTIN_INIT_SHIFT))&FTM_CNTIN_INIT_MASK)
/* STATUS Bit Fields */
#define FTM_STATUS_CH0F_MASK                     0x1u
#define FTM_STATUS_CH0F_SHIFT                    0
#define FTM_STATUS_CH0F_WIDTH                    1
#define FTM_STATUS_CH0F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH0F_SHIFT))&FTM_STATUS_CH0F_MASK)
#define FTM_STATUS_CH1F_MASK                     0x2u
#define FTM_STATUS_CH1F_SHIFT                    1
#define FTM_STATUS_CH1F_WIDTH                    1
#define FTM_STATUS_CH1F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH1F_SHIFT))&FTM_STATUS_CH1F_MASK)
#define FTM_STATUS_CH2F_MASK                     0x4u
#define FTM_STATUS_CH2F_SHIFT                    2
#define FTM_STATUS_CH2F_WIDTH                    1
#define FTM_STATUS_CH2F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH2F_SHIFT))&FTM_STATUS_CH2F_MASK)
#define FTM_STATUS_CH3F_MASK                     0x8u
#define FTM_STATUS_CH3F_SHIFT                    3
#define FTM_STATUS_CH3F_WIDTH                    1
#define FTM_STATUS_CH3F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH3F_SHIFT))&FTM_STATUS_CH3F_MASK)
#define FTM_STATUS_CH4F_MASK                     0x10u
#define FTM_STATUS_CH4F_SHIFT                    4
#define FTM_STATUS_CH4F_WIDTH                    1
#define FTM_STATUS_CH4F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH4F_SHIFT))&FTM_STATUS_CH4F_MASK)
#define FTM_STATUS_CH5F_MASK                     0x20u
#define FTM_STATUS_CH5F_SHIFT                    5
#define FTM_STATUS_CH5F_WIDTH                    1
#define FTM_STATUS_CH5F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH5F_SHIFT))&FTM_STATUS_CH5F_MASK)
#define FTM_STATUS_CH6F_MASK                     0x40u
#define FTM_STATUS_CH6F_SHIFT                    6
#define FTM_STATUS_CH6F_WIDTH                    1
#define FTM_STATUS_CH6F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH6F_SHIFT))&FTM_STATUS_CH6F_MASK)
#define FTM_STATUS_CH7F_MASK                     0x80u
#define FTM_STATUS_CH7F_SHIFT                    7
#define FTM_STATUS_CH7F_WIDTH                    1
#define FTM_STATUS_CH7F(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_STATUS_CH7F_SHIFT))&FTM_STATUS_CH7F_MASK)
/* MODE Bit Fields */
#define FTM_MODE_FTMEN_MASK                      0x1u
#define FTM_MODE_FTMEN_SHIFT                     0
#define FTM_MODE_FTMEN_WIDTH                     1
#define FTM_MODE_FTMEN(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_MODE_FTMEN_SHIFT))&FTM_MODE_FTMEN_MASK)
#define FTM_MODE_INIT_MASK                       0x2u
#define FTM_MODE_INIT_SHIFT                      1
#define FTM_MODE_INIT_WIDTH                      1
#define FTM_MODE_INIT(x)                         (((uint32_t)(((uint32_t)(x))<<FTM_MODE_INIT_SHIFT))&FTM_MODE_INIT_MASK)
#define FTM_MODE_WPDIS_MASK                      0x4u
#define FTM_MODE_WPDIS_SHIFT                     2
#define FTM_MODE_WPDIS_WIDTH                     1
#define FTM_MODE_WPDIS(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_MODE_WPDIS_SHIFT))&FTM_MODE_WPDIS_MASK)
#define FTM_MODE_PWMSYNC_MASK                    0x8u
#define FTM_MODE_PWMSYNC_SHIFT                   3
#define FTM_MODE_PWMSYNC_WIDTH                   1
#define FTM_MODE_PWMSYNC(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_MODE_PWMSYNC_SHIFT))&FTM_MODE_PWMSYNC_MASK)
#define FTM_MODE_CAPTEST_MASK                    0x10u
#define FTM_MODE_CAPTEST_SHIFT                   4
#define FTM_MODE_CAPTEST_WIDTH                   1
#define FTM_MODE_CAPTEST(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_MODE_CAPTEST_SHIFT))&FTM_MODE_CAPTEST_MASK)
#define FTM_MODE_FAULTM_MASK                     0x60u
#define FTM_MODE_FAULTM_SHIFT                    5
#define FTM_MODE_FAULTM_WIDTH                    2
#define FTM_MODE_FAULTM(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_MODE_FAULTM_SHIFT))&FTM_MODE_FAULTM_MASK)
#define FTM_MODE_FAULTIE_MASK                    0x80u
#define FTM_MODE_FAULTIE_SHIFT                   7
#define FTM_MODE_FAULTIE_WIDTH                   1
#define FTM_MODE_FAULTIE(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_MODE_FAULTIE_SHIFT))&FTM_MODE_FAULTIE_MASK)
/* SYNC Bit Fields */
#define FTM_SYNC_CNTMIN_MASK                     0x1u
#define FTM_SYNC_CNTMIN_SHIFT                    0
#define FTM_SYNC_CNTMIN_WIDTH                    1
#define FTM_SYNC_CNTMIN(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_CNTMIN_SHIFT))&FTM_SYNC_CNTMIN_MASK)
#define FTM_SYNC_CNTMAX_MASK                     0x2u
#define FTM_SYNC_CNTMAX_SHIFT                    1
#define FTM_SYNC_CNTMAX_WIDTH                    1
#define FTM_SYNC_CNTMAX(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_CNTMAX_SHIFT))&FTM_SYNC_CNTMAX_MASK)
#define FTM_SYNC_REINIT_MASK                     0x4u
#define FTM_SYNC_REINIT_SHIFT                    2
#define FTM_SYNC_REINIT_WIDTH                    1
#define FTM_SYNC_REINIT(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_REINIT_SHIFT))&FTM_SYNC_REINIT_MASK)
#define FTM_SYNC_SYNCHOM_MASK                    0x8u
#define FTM_SYNC_SYNCHOM_SHIFT                   3
#define FTM_SYNC_SYNCHOM_WIDTH                   1
#define FTM_SYNC_SYNCHOM(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_SYNCHOM_SHIFT))&FTM_SYNC_SYNCHOM_MASK)
#define FTM_SYNC_TRIG0_MASK                      0x10u
#define FTM_SYNC_TRIG0_SHIFT                     4
#define FTM_SYNC_TRIG0_WIDTH                     1
#define FTM_SYNC_TRIG0(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_TRIG0_SHIFT))&FTM_SYNC_TRIG0_MASK)
#define FTM_SYNC_TRIG1_MASK                      0x20u
#define FTM_SYNC_TRIG1_SHIFT                     5
#define FTM_SYNC_TRIG1_WIDTH                     1
#define FTM_SYNC_TRIG1(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_TRIG1_SHIFT))&FTM_SYNC_TRIG1_MASK)
#define FTM_SYNC_TRIG2_MASK                      0x40u
#define FTM_SYNC_TRIG2_SHIFT                     6
#define FTM_SYNC_TRIG2_WIDTH                     1
#define FTM_SYNC_TRIG2(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_TRIG2_SHIFT))&FTM_SYNC_TRIG2_MASK)
#define FTM_SYNC_SWSYNC_MASK                     0x80u
#define FTM_SYNC_SWSYNC_SHIFT                    7
#define FTM_SYNC_SWSYNC_WIDTH                    1
#define FTM_SYNC_SWSYNC(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_SYNC_SWSYNC_SHIFT))&FTM_SYNC_SWSYNC_MASK)
/* OUTINIT Bit Fields */
#define FTM_OUTINIT_CH0OI_MASK                   0x1u
#define FTM_OUTINIT_CH0OI_SHIFT                  0
#define FTM_OUTINIT_CH0OI_WIDTH                  1
#define FTM_OUTINIT_CH0OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH0OI_SHIFT))&FTM_OUTINIT_CH0OI_MASK)
#define FTM_OUTINIT_CH1OI_MASK                   0x2u
#define FTM_OUTINIT_CH1OI_SHIFT                  1
#define FTM_OUTINIT_CH1OI_WIDTH                  1
#define FTM_OUTINIT_CH1OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH1OI_SHIFT))&FTM_OUTINIT_CH1OI_MASK)
#define FTM_OUTINIT_CH2OI_MASK                   0x4u
#define FTM_OUTINIT_CH2OI_SHIFT                  2
#define FTM_OUTINIT_CH2OI_WIDTH                  1
#define FTM_OUTINIT_CH2OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH2OI_SHIFT))&FTM_OUTINIT_CH2OI_MASK)
#define FTM_OUTINIT_CH3OI_MASK                   0x8u
#define FTM_OUTINIT_CH3OI_SHIFT                  3
#define FTM_OUTINIT_CH3OI_WIDTH                  1
#define FTM_OUTINIT_CH3OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH3OI_SHIFT))&FTM_OUTINIT_CH3OI_MASK)
#define FTM_OUTINIT_CH4OI_MASK                   0x10u
#define FTM_OUTINIT_CH4OI_SHIFT                  4
#define FTM_OUTINIT_CH4OI_WIDTH                  1
#define FTM_OUTINIT_CH4OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH4OI_SHIFT))&FTM_OUTINIT_CH4OI_MASK)
#define FTM_OUTINIT_CH5OI_MASK                   0x20u
#define FTM_OUTINIT_CH5OI_SHIFT                  5
#define FTM_OUTINIT_CH5OI_WIDTH                  1
#define FTM_OUTINIT_CH5OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH5OI_SHIFT))&FTM_OUTINIT_CH5OI_MASK)
#define FTM_OUTINIT_CH6OI_MASK                   0x40u
#define FTM_OUTINIT_CH6OI_SHIFT                  6
#define FTM_OUTINIT_CH6OI_WIDTH                  1
#define FTM_OUTINIT_CH6OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH6OI_SHIFT))&FTM_OUTINIT_CH6OI_MASK)
#define FTM_OUTINIT_CH7OI_MASK                   0x80u
#define FTM_OUTINIT_CH7OI_SHIFT                  7
#define FTM_OUTINIT_CH7OI_WIDTH                  1
#define FTM_OUTINIT_CH7OI(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTINIT_CH7OI_SHIFT))&FTM_OUTINIT_CH7OI_MASK)
/* OUTMASK Bit Fields */
#define FTM_OUTMASK_CH0OM_MASK                   0x1u
#define FTM_OUTMASK_CH0OM_SHIFT                  0
#define FTM_OUTMASK_CH0OM_WIDTH                  1
#define FTM_OUTMASK_CH0OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH0OM_SHIFT))&FTM_OUTMASK_CH0OM_MASK)
#define FTM_OUTMASK_CH1OM_MASK                   0x2u
#define FTM_OUTMASK_CH1OM_SHIFT                  1
#define FTM_OUTMASK_CH1OM_WIDTH                  1
#define FTM_OUTMASK_CH1OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH1OM_SHIFT))&FTM_OUTMASK_CH1OM_MASK)
#define FTM_OUTMASK_CH2OM_MASK                   0x4u
#define FTM_OUTMASK_CH2OM_SHIFT                  2
#define FTM_OUTMASK_CH2OM_WIDTH                  1
#define FTM_OUTMASK_CH2OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH2OM_SHIFT))&FTM_OUTMASK_CH2OM_MASK)
#define FTM_OUTMASK_CH3OM_MASK                   0x8u
#define FTM_OUTMASK_CH3OM_SHIFT                  3
#define FTM_OUTMASK_CH3OM_WIDTH                  1
#define FTM_OUTMASK_CH3OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH3OM_SHIFT))&FTM_OUTMASK_CH3OM_MASK)
#define FTM_OUTMASK_CH4OM_MASK                   0x10u
#define FTM_OUTMASK_CH4OM_SHIFT                  4
#define FTM_OUTMASK_CH4OM_WIDTH                  1
#define FTM_OUTMASK_CH4OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH4OM_SHIFT))&FTM_OUTMASK_CH4OM_MASK)
#define FTM_OUTMASK_CH5OM_MASK                   0x20u
#define FTM_OUTMASK_CH5OM_SHIFT                  5
#define FTM_OUTMASK_CH5OM_WIDTH                  1
#define FTM_OUTMASK_CH5OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH5OM_SHIFT))&FTM_OUTMASK_CH5OM_MASK)
#define FTM_OUTMASK_CH6OM_MASK                   0x40u
#define FTM_OUTMASK_CH6OM_SHIFT                  6
#define FTM_OUTMASK_CH6OM_WIDTH                  1
#define FTM_OUTMASK_CH6OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH6OM_SHIFT))&FTM_OUTMASK_CH6OM_MASK)
#define FTM_OUTMASK_CH7OM_MASK                   0x80u
#define FTM_OUTMASK_CH7OM_SHIFT                  7
#define FTM_OUTMASK_CH7OM_WIDTH                  1
#define FTM_OUTMASK_CH7OM(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_OUTMASK_CH7OM_SHIFT))&FTM_OUTMASK_CH7OM_MASK)
/* COMBINE Bit Fields */
#define FTM_COMBINE_COMBINE0_MASK                0x1u
#define FTM_COMBINE_COMBINE0_SHIFT               0
#define FTM_COMBINE_COMBINE0_WIDTH               1
#define FTM_COMBINE_COMBINE0(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMBINE0_SHIFT))&FTM_COMBINE_COMBINE0_MASK)
#define FTM_COMBINE_COMP0_MASK                   0x2u
#define FTM_COMBINE_COMP0_SHIFT                  1
#define FTM_COMBINE_COMP0_WIDTH                  1
#define FTM_COMBINE_COMP0(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMP0_SHIFT))&FTM_COMBINE_COMP0_MASK)
#define FTM_COMBINE_DECAPEN0_MASK                0x4u
#define FTM_COMBINE_DECAPEN0_SHIFT               2
#define FTM_COMBINE_DECAPEN0_WIDTH               1
#define FTM_COMBINE_DECAPEN0(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAPEN0_SHIFT))&FTM_COMBINE_DECAPEN0_MASK)
#define FTM_COMBINE_DECAP0_MASK                  0x8u
#define FTM_COMBINE_DECAP0_SHIFT                 3
#define FTM_COMBINE_DECAP0_WIDTH                 1
#define FTM_COMBINE_DECAP0(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAP0_SHIFT))&FTM_COMBINE_DECAP0_MASK)
#define FTM_COMBINE_DTEN0_MASK                   0x10u
#define FTM_COMBINE_DTEN0_SHIFT                  4
#define FTM_COMBINE_DTEN0_WIDTH                  1
#define FTM_COMBINE_DTEN0(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DTEN0_SHIFT))&FTM_COMBINE_DTEN0_MASK)
#define FTM_COMBINE_SYNCEN0_MASK                 0x20u
#define FTM_COMBINE_SYNCEN0_SHIFT                5
#define FTM_COMBINE_SYNCEN0_WIDTH                1
#define FTM_COMBINE_SYNCEN0(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_SYNCEN0_SHIFT))&FTM_COMBINE_SYNCEN0_MASK)
#define FTM_COMBINE_FAULTEN0_MASK                0x40u
#define FTM_COMBINE_FAULTEN0_SHIFT               6
#define FTM_COMBINE_FAULTEN0_WIDTH               1
#define FTM_COMBINE_FAULTEN0(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_FAULTEN0_SHIFT))&FTM_COMBINE_FAULTEN0_MASK)
#define FTM_COMBINE_COMBINE1_MASK                0x100u
#define FTM_COMBINE_COMBINE1_SHIFT               8
#define FTM_COMBINE_COMBINE1_WIDTH               1
#define FTM_COMBINE_COMBINE1(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMBINE1_SHIFT))&FTM_COMBINE_COMBINE1_MASK)
#define FTM_COMBINE_COMP1_MASK                   0x200u
#define FTM_COMBINE_COMP1_SHIFT                  9
#define FTM_COMBINE_COMP1_WIDTH                  1
#define FTM_COMBINE_COMP1(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMP1_SHIFT))&FTM_COMBINE_COMP1_MASK)
#define FTM_COMBINE_DECAPEN1_MASK                0x400u
#define FTM_COMBINE_DECAPEN1_SHIFT               10
#define FTM_COMBINE_DECAPEN1_WIDTH               1
#define FTM_COMBINE_DECAPEN1(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAPEN1_SHIFT))&FTM_COMBINE_DECAPEN1_MASK)
#define FTM_COMBINE_DECAP1_MASK                  0x800u
#define FTM_COMBINE_DECAP1_SHIFT                 11
#define FTM_COMBINE_DECAP1_WIDTH                 1
#define FTM_COMBINE_DECAP1(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAP1_SHIFT))&FTM_COMBINE_DECAP1_MASK)
#define FTM_COMBINE_DTEN1_MASK                   0x1000u
#define FTM_COMBINE_DTEN1_SHIFT                  12
#define FTM_COMBINE_DTEN1_WIDTH                  1
#define FTM_COMBINE_DTEN1(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DTEN1_SHIFT))&FTM_COMBINE_DTEN1_MASK)
#define FTM_COMBINE_SYNCEN1_MASK                 0x2000u
#define FTM_COMBINE_SYNCEN1_SHIFT                13
#define FTM_COMBINE_SYNCEN1_WIDTH                1
#define FTM_COMBINE_SYNCEN1(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_SYNCEN1_SHIFT))&FTM_COMBINE_SYNCEN1_MASK)
#define FTM_COMBINE_FAULTEN1_MASK                0x4000u
#define FTM_COMBINE_FAULTEN1_SHIFT               14
#define FTM_COMBINE_FAULTEN1_WIDTH               1
#define FTM_COMBINE_FAULTEN1(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_FAULTEN1_SHIFT))&FTM_COMBINE_FAULTEN1_MASK)
#define FTM_COMBINE_COMBINE2_MASK                0x10000u
#define FTM_COMBINE_COMBINE2_SHIFT               16
#define FTM_COMBINE_COMBINE2_WIDTH               1
#define FTM_COMBINE_COMBINE2(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMBINE2_SHIFT))&FTM_COMBINE_COMBINE2_MASK)
#define FTM_COMBINE_COMP2_MASK                   0x20000u
#define FTM_COMBINE_COMP2_SHIFT                  17
#define FTM_COMBINE_COMP2_WIDTH                  1
#define FTM_COMBINE_COMP2(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMP2_SHIFT))&FTM_COMBINE_COMP2_MASK)
#define FTM_COMBINE_DECAPEN2_MASK                0x40000u
#define FTM_COMBINE_DECAPEN2_SHIFT               18
#define FTM_COMBINE_DECAPEN2_WIDTH               1
#define FTM_COMBINE_DECAPEN2(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAPEN2_SHIFT))&FTM_COMBINE_DECAPEN2_MASK)
#define FTM_COMBINE_DECAP2_MASK                  0x80000u
#define FTM_COMBINE_DECAP2_SHIFT                 19
#define FTM_COMBINE_DECAP2_WIDTH                 1
#define FTM_COMBINE_DECAP2(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAP2_SHIFT))&FTM_COMBINE_DECAP2_MASK)
#define FTM_COMBINE_DTEN2_MASK                   0x100000u
#define FTM_COMBINE_DTEN2_SHIFT                  20
#define FTM_COMBINE_DTEN2_WIDTH                  1
#define FTM_COMBINE_DTEN2(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DTEN2_SHIFT))&FTM_COMBINE_DTEN2_MASK)
#define FTM_COMBINE_SYNCEN2_MASK                 0x200000u
#define FTM_COMBINE_SYNCEN2_SHIFT                21
#define FTM_COMBINE_SYNCEN2_WIDTH                1
#define FTM_COMBINE_SYNCEN2(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_SYNCEN2_SHIFT))&FTM_COMBINE_SYNCEN2_MASK)
#define FTM_COMBINE_FAULTEN2_MASK                0x400000u
#define FTM_COMBINE_FAULTEN2_SHIFT               22
#define FTM_COMBINE_FAULTEN2_WIDTH               1
#define FTM_COMBINE_FAULTEN2(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_FAULTEN2_SHIFT))&FTM_COMBINE_FAULTEN2_MASK)
#define FTM_COMBINE_COMBINE3_MASK                0x1000000u
#define FTM_COMBINE_COMBINE3_SHIFT               24
#define FTM_COMBINE_COMBINE3_WIDTH               1
#define FTM_COMBINE_COMBINE3(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMBINE3_SHIFT))&FTM_COMBINE_COMBINE3_MASK)
#define FTM_COMBINE_COMP3_MASK                   0x2000000u
#define FTM_COMBINE_COMP3_SHIFT                  25
#define FTM_COMBINE_COMP3_WIDTH                  1
#define FTM_COMBINE_COMP3(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_COMP3_SHIFT))&FTM_COMBINE_COMP3_MASK)
#define FTM_COMBINE_DECAPEN3_MASK                0x4000000u
#define FTM_COMBINE_DECAPEN3_SHIFT               26
#define FTM_COMBINE_DECAPEN3_WIDTH               1
#define FTM_COMBINE_DECAPEN3(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAPEN3_SHIFT))&FTM_COMBINE_DECAPEN3_MASK)
#define FTM_COMBINE_DECAP3_MASK                  0x8000000u
#define FTM_COMBINE_DECAP3_SHIFT                 27
#define FTM_COMBINE_DECAP3_WIDTH                 1
#define FTM_COMBINE_DECAP3(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DECAP3_SHIFT))&FTM_COMBINE_DECAP3_MASK)
#define FTM_COMBINE_DTEN3_MASK                   0x10000000u
#define FTM_COMBINE_DTEN3_SHIFT                  28
#define FTM_COMBINE_DTEN3_WIDTH                  1
#define FTM_COMBINE_DTEN3(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_DTEN3_SHIFT))&FTM_COMBINE_DTEN3_MASK)
#define FTM_COMBINE_SYNCEN3_MASK                 0x20000000u
#define FTM_COMBINE_SYNCEN3_SHIFT                29
#define FTM_COMBINE_SYNCEN3_WIDTH                1
#define FTM_COMBINE_SYNCEN3(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_SYNCEN3_SHIFT))&FTM_COMBINE_SYNCEN3_MASK)
#define FTM_COMBINE_FAULTEN3_MASK                0x40000000u
#define FTM_COMBINE_FAULTEN3_SHIFT               30
#define FTM_COMBINE_FAULTEN3_WIDTH               1
#define FTM_COMBINE_FAULTEN3(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_COMBINE_FAULTEN3_SHIFT))&FTM_COMBINE_FAULTEN3_MASK)
/* DEADTIME Bit Fields */
#define FTM_DEADTIME_DTVAL_MASK                  0x3Fu
#define FTM_DEADTIME_DTVAL_SHIFT                 0
#define FTM_DEADTIME_DTVAL_WIDTH                 6
#define FTM_DEADTIME_DTVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_DEADTIME_DTVAL_SHIFT))&FTM_DEADTIME_DTVAL_MASK)
#define FTM_DEADTIME_DTPS_MASK                   0xC0u
#define FTM_DEADTIME_DTPS_SHIFT                  6
#define FTM_DEADTIME_DTPS_WIDTH                  2
#define FTM_DEADTIME_DTPS(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_DEADTIME_DTPS_SHIFT))&FTM_DEADTIME_DTPS_MASK)
/* EXTTRIG Bit Fields */
#define FTM_EXTTRIG_CH2TRIG_MASK                 0x1u
#define FTM_EXTTRIG_CH2TRIG_SHIFT                0
#define FTM_EXTTRIG_CH2TRIG_WIDTH                1
#define FTM_EXTTRIG_CH2TRIG(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_CH2TRIG_SHIFT))&FTM_EXTTRIG_CH2TRIG_MASK)
#define FTM_EXTTRIG_CH3TRIG_MASK                 0x2u
#define FTM_EXTTRIG_CH3TRIG_SHIFT                1
#define FTM_EXTTRIG_CH3TRIG_WIDTH                1
#define FTM_EXTTRIG_CH3TRIG(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_CH3TRIG_SHIFT))&FTM_EXTTRIG_CH3TRIG_MASK)
#define FTM_EXTTRIG_CH4TRIG_MASK                 0x4u
#define FTM_EXTTRIG_CH4TRIG_SHIFT                2
#define FTM_EXTTRIG_CH4TRIG_WIDTH                1
#define FTM_EXTTRIG_CH4TRIG(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_CH4TRIG_SHIFT))&FTM_EXTTRIG_CH4TRIG_MASK)
#define FTM_EXTTRIG_CH5TRIG_MASK                 0x8u
#define FTM_EXTTRIG_CH5TRIG_SHIFT                3
#define FTM_EXTTRIG_CH5TRIG_WIDTH                1
#define FTM_EXTTRIG_CH5TRIG(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_CH5TRIG_SHIFT))&FTM_EXTTRIG_CH5TRIG_MASK)
#define FTM_EXTTRIG_CH0TRIG_MASK                 0x10u
#define FTM_EXTTRIG_CH0TRIG_SHIFT                4
#define FTM_EXTTRIG_CH0TRIG_WIDTH                1
#define FTM_EXTTRIG_CH0TRIG(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_CH0TRIG_SHIFT))&FTM_EXTTRIG_CH0TRIG_MASK)
#define FTM_EXTTRIG_CH1TRIG_MASK                 0x20u
#define FTM_EXTTRIG_CH1TRIG_SHIFT                5
#define FTM_EXTTRIG_CH1TRIG_WIDTH                1
#define FTM_EXTTRIG_CH1TRIG(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_CH1TRIG_SHIFT))&FTM_EXTTRIG_CH1TRIG_MASK)
#define FTM_EXTTRIG_INITTRIGEN_MASK              0x40u
#define FTM_EXTTRIG_INITTRIGEN_SHIFT             6
#define FTM_EXTTRIG_INITTRIGEN_WIDTH             1
#define FTM_EXTTRIG_INITTRIGEN(x)                (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_INITTRIGEN_SHIFT))&FTM_EXTTRIG_INITTRIGEN_MASK)
#define FTM_EXTTRIG_TRIGF_MASK                   0x80u
#define FTM_EXTTRIG_TRIGF_SHIFT                  7
#define FTM_EXTTRIG_TRIGF_WIDTH                  1
#define FTM_EXTTRIG_TRIGF(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_EXTTRIG_TRIGF_SHIFT))&FTM_EXTTRIG_TRIGF_MASK)
/* POL Bit Fields */
#define FTM_POL_POL0_MASK                        0x1u
#define FTM_POL_POL0_SHIFT                       0
#define FTM_POL_POL0_WIDTH                       1
#define FTM_POL_POL0(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL0_SHIFT))&FTM_POL_POL0_MASK)
#define FTM_POL_POL1_MASK                        0x2u
#define FTM_POL_POL1_SHIFT                       1
#define FTM_POL_POL1_WIDTH                       1
#define FTM_POL_POL1(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL1_SHIFT))&FTM_POL_POL1_MASK)
#define FTM_POL_POL2_MASK                        0x4u
#define FTM_POL_POL2_SHIFT                       2
#define FTM_POL_POL2_WIDTH                       1
#define FTM_POL_POL2(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL2_SHIFT))&FTM_POL_POL2_MASK)
#define FTM_POL_POL3_MASK                        0x8u
#define FTM_POL_POL3_SHIFT                       3
#define FTM_POL_POL3_WIDTH                       1
#define FTM_POL_POL3(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL3_SHIFT))&FTM_POL_POL3_MASK)
#define FTM_POL_POL4_MASK                        0x10u
#define FTM_POL_POL4_SHIFT                       4
#define FTM_POL_POL4_WIDTH                       1
#define FTM_POL_POL4(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL4_SHIFT))&FTM_POL_POL4_MASK)
#define FTM_POL_POL5_MASK                        0x20u
#define FTM_POL_POL5_SHIFT                       5
#define FTM_POL_POL5_WIDTH                       1
#define FTM_POL_POL5(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL5_SHIFT))&FTM_POL_POL5_MASK)
#define FTM_POL_POL6_MASK                        0x40u
#define FTM_POL_POL6_SHIFT                       6
#define FTM_POL_POL6_WIDTH                       1
#define FTM_POL_POL6(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL6_SHIFT))&FTM_POL_POL6_MASK)
#define FTM_POL_POL7_MASK                        0x80u
#define FTM_POL_POL7_SHIFT                       7
#define FTM_POL_POL7_WIDTH                       1
#define FTM_POL_POL7(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_POL_POL7_SHIFT))&FTM_POL_POL7_MASK)
/* FMS Bit Fields */
#define FTM_FMS_FAULTF0_MASK                     0x1u
#define FTM_FMS_FAULTF0_SHIFT                    0
#define FTM_FMS_FAULTF0_WIDTH                    1
#define FTM_FMS_FAULTF0(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_FMS_FAULTF0_SHIFT))&FTM_FMS_FAULTF0_MASK)
#define FTM_FMS_FAULTF1_MASK                     0x2u
#define FTM_FMS_FAULTF1_SHIFT                    1
#define FTM_FMS_FAULTF1_WIDTH                    1
#define FTM_FMS_FAULTF1(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_FMS_FAULTF1_SHIFT))&FTM_FMS_FAULTF1_MASK)
#define FTM_FMS_FAULTF2_MASK                     0x4u
#define FTM_FMS_FAULTF2_SHIFT                    2
#define FTM_FMS_FAULTF2_WIDTH                    1
#define FTM_FMS_FAULTF2(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_FMS_FAULTF2_SHIFT))&FTM_FMS_FAULTF2_MASK)
#define FTM_FMS_FAULTF3_MASK                     0x8u
#define FTM_FMS_FAULTF3_SHIFT                    3
#define FTM_FMS_FAULTF3_WIDTH                    1
#define FTM_FMS_FAULTF3(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_FMS_FAULTF3_SHIFT))&FTM_FMS_FAULTF3_MASK)
#define FTM_FMS_FAULTIN_MASK                     0x20u
#define FTM_FMS_FAULTIN_SHIFT                    5
#define FTM_FMS_FAULTIN_WIDTH                    1
#define FTM_FMS_FAULTIN(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_FMS_FAULTIN_SHIFT))&FTM_FMS_FAULTIN_MASK)
#define FTM_FMS_WPEN_MASK                        0x40u
#define FTM_FMS_WPEN_SHIFT                       6
#define FTM_FMS_WPEN_WIDTH                       1
#define FTM_FMS_WPEN(x)                          (((uint32_t)(((uint32_t)(x))<<FTM_FMS_WPEN_SHIFT))&FTM_FMS_WPEN_MASK)
#define FTM_FMS_FAULTF_MASK                      0x80u
#define FTM_FMS_FAULTF_SHIFT                     7
#define FTM_FMS_FAULTF_WIDTH                     1
#define FTM_FMS_FAULTF(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_FMS_FAULTF_SHIFT))&FTM_FMS_FAULTF_MASK)
/* FILTER Bit Fields */
#define FTM_FILTER_CH0FVAL_MASK                  0xFu
#define FTM_FILTER_CH0FVAL_SHIFT                 0
#define FTM_FILTER_CH0FVAL_WIDTH                 4
#define FTM_FILTER_CH0FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH0FVAL_SHIFT))&FTM_FILTER_CH0FVAL_MASK)
#define FTM_FILTER_CH1FVAL_MASK                  0xF0u
#define FTM_FILTER_CH1FVAL_SHIFT                 4
#define FTM_FILTER_CH1FVAL_WIDTH                 4
#define FTM_FILTER_CH1FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH1FVAL_SHIFT))&FTM_FILTER_CH1FVAL_MASK)
#define FTM_FILTER_CH2FVAL_MASK                  0xF00u
#define FTM_FILTER_CH2FVAL_SHIFT                 8
#define FTM_FILTER_CH2FVAL_WIDTH                 4
#define FTM_FILTER_CH2FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH2FVAL_SHIFT))&FTM_FILTER_CH2FVAL_MASK)
#define FTM_FILTER_CH3FVAL_MASK                  0xF000u
#define FTM_FILTER_CH3FVAL_SHIFT                 12
#define FTM_FILTER_CH3FVAL_WIDTH                 4
#define FTM_FILTER_CH3FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH3FVAL_SHIFT))&FTM_FILTER_CH3FVAL_MASK)
/* FLTCTRL Bit Fields */
#define FTM_FLTCTRL_FAULT0EN_MASK                0x1u
#define FTM_FLTCTRL_FAULT0EN_SHIFT               0
#define FTM_FLTCTRL_FAULT0EN_WIDTH               1
#define FTM_FLTCTRL_FAULT0EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FAULT0EN_SHIFT))&FTM_FLTCTRL_FAULT0EN_MASK)
#define FTM_FLTCTRL_FAULT1EN_MASK                0x2u
#define FTM_FLTCTRL_FAULT1EN_SHIFT               1
#define FTM_FLTCTRL_FAULT1EN_WIDTH               1
#define FTM_FLTCTRL_FAULT1EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FAULT1EN_SHIFT))&FTM_FLTCTRL_FAULT1EN_MASK)
#define FTM_FLTCTRL_FAULT2EN_MASK                0x4u
#define FTM_FLTCTRL_FAULT2EN_SHIFT               2
#define FTM_FLTCTRL_FAULT2EN_WIDTH               1
#define FTM_FLTCTRL_FAULT2EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FAULT2EN_SHIFT))&FTM_FLTCTRL_FAULT2EN_MASK)
#define FTM_FLTCTRL_FAULT3EN_MASK                0x8u
#define FTM_FLTCTRL_FAULT3EN_SHIFT               3
#define FTM_FLTCTRL_FAULT3EN_WIDTH               1
#define FTM_FLTCTRL_FAULT3EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FAULT3EN_SHIFT))&FTM_FLTCTRL_FAULT3EN_MASK)
#define FTM_FLTCTRL_FFLTR0EN_MASK                0x10u
#define FTM_FLTCTRL_FFLTR0EN_SHIFT               4
#define FTM_FLTCTRL_FFLTR0EN_WIDTH               1
#define FTM_FLTCTRL_FFLTR0EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FFLTR0EN_SHIFT))&FTM_FLTCTRL_FFLTR0EN_MASK)
#define FTM_FLTCTRL_FFLTR1EN_MASK                0x20u
#define FTM_FLTCTRL_FFLTR1EN_SHIFT               5
#define FTM_FLTCTRL_FFLTR1EN_WIDTH               1
#define FTM_FLTCTRL_FFLTR1EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FFLTR1EN_SHIFT))&FTM_FLTCTRL_FFLTR1EN_MASK)
#define FTM_FLTCTRL_FFLTR2EN_MASK                0x40u
#define FTM_FLTCTRL_FFLTR2EN_SHIFT               6
#define FTM_FLTCTRL_FFLTR2EN_WIDTH               1
#define FTM_FLTCTRL_FFLTR2EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FFLTR2EN_SHIFT))&FTM_FLTCTRL_FFLTR2EN_MASK)
#define FTM_FLTCTRL_FFLTR3EN_MASK                0x80u
#define FTM_FLTCTRL_FFLTR3EN_SHIFT               7
#define FTM_FLTCTRL_FFLTR3EN_WIDTH               1
#define FTM_FLTCTRL_FFLTR3EN(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FFLTR3EN_SHIFT))&FTM_FLTCTRL_FFLTR3EN_MASK)
#define FTM_FLTCTRL_FFVAL_MASK                   0xF00u
#define FTM_FLTCTRL_FFVAL_SHIFT                  8
#define FTM_FLTCTRL_FFVAL_WIDTH                  4
#define FTM_FLTCTRL_FFVAL(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FFVAL_SHIFT))&FTM_FLTCTRL_FFVAL_MASK)
/* CONF Bit Fields */
#define FTM_CONF_NUMTOF_MASK                     0x1Fu
#define FTM_CONF_NUMTOF_SHIFT                    0
#define FTM_CONF_NUMTOF_WIDTH                    5
#define FTM_CONF_NUMTOF(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_CONF_NUMTOF_SHIFT))&FTM_CONF_NUMTOF_MASK)
#define FTM_CONF_BDMMODE_MASK                    0xC0u
#define FTM_CONF_BDMMODE_SHIFT                   6
#define FTM_CONF_BDMMODE_WIDTH                   2
#define FTM_CONF_BDMMODE(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_CONF_BDMMODE_SHIFT))&FTM_CONF_BDMMODE_MASK)
#define FTM_CONF_GTBEEN_MASK                     0x200u
#define FTM_CONF_GTBEEN_SHIFT                    9
#define FTM_CONF_GTBEEN_WIDTH                    1
#define FTM_CONF_GTBEEN(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_CONF_GTBEEN_SHIFT))&FTM_CONF_GTBEEN_MASK)
#define FTM_CONF_GTBEOUT_MASK                    0x400u
#define FTM_CONF_GTBEOUT_SHIFT                   10
#define FTM_CONF_GTBEOUT_WIDTH                   1
#define FTM_CONF_GTBEOUT(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_CONF_GTBEOUT_SHIFT))&FTM_CONF_GTBEOUT_MASK)
/* FLTPOL Bit Fields */
#define FTM_FLTPOL_FLT0POL_MASK                  0x1u
#define FTM_FLTPOL_FLT0POL_SHIFT                 0
#define FTM_FLTPOL_FLT0POL_WIDTH                 1
#define FTM_FLTPOL_FLT0POL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FLTPOL_FLT0POL_SHIFT))&FTM_FLTPOL_FLT0POL_MASK)
#define FTM_FLTPOL_FLT1POL_MASK                  0x2u
#define FTM_FLTPOL_FLT1POL_SHIFT                 1
#define FTM_FLTPOL_FLT1POL_WIDTH                 1
#define FTM_FLTPOL_FLT1POL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FLTPOL_FLT1POL_SHIFT))&FTM_FLTPOL_FLT1POL_MASK)
#define FTM_FLTPOL_FLT2POL_MASK                  0x4u
#define FTM_FLTPOL_FLT2POL_SHIFT                 2
#define FTM_FLTPOL_FLT2POL_WIDTH                 1
#define FTM_FLTPOL_FLT2POL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FLTPOL_FLT2POL_SHIFT))&FTM_FLTPOL_FLT2POL_MASK)
#define FTM_FLTPOL_FLT3POL_MASK                  0x8u
#define FTM_FLTPOL_FLT3POL_SHIFT                 3
#define FTM_FLTPOL_FLT3POL_WIDTH                 1
#define FTM_FLTPOL_FLT3POL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FLTPOL_FLT3POL_SHIFT))&FTM_FLTPOL_FLT3POL_MASK)
/* SYNCONF Bit Fields */
#define FTM_SYNCONF_HWTRIGMODE_MASK              0x1u
#define FTM_SYNCONF_HWTRIGMODE_SHIFT             0
#define FTM_SYNCONF_HWTRIGMODE_WIDTH             1
#define FTM_SYNCONF_HWTRIGMODE(x)                (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_HWTRIGMODE_SHIFT))&FTM_SYNCONF_HWTRIGMODE_MASK)
#define FTM_SYNCONF_CNTINC_MASK                  0x4u
#define FTM_SYNCONF_CNTINC_SHIFT                 2
#define FTM_SYNCONF_CNTINC_WIDTH                 1
#define FTM_SYNCONF_CNTINC(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_CNTINC_SHIFT))&FTM_SYNCONF_CNTINC_MASK)
#define FTM_SYNCONF_INVC_MASK                    0x10u
#define FTM_SYNCONF_INVC_SHIFT                   4
#define FTM_SYNCONF_INVC_WIDTH                   1
#define FTM_SYNCONF_INVC(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_INVC_SHIFT))&FTM_SYNCONF_INVC_MASK)
#define FTM_SYNCONF_SWOC_MASK                    0x20u
#define FTM_SYNCONF_SWOC_SHIFT                   5
#define FTM_SYNCONF_SWOC_WIDTH                   1
#define FTM_SYNCONF_SWOC(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SWOC_SHIFT))&FTM_SYNCONF_SWOC_MASK)
#define FTM_SYNCONF_SYNCMODE_MASK                0x80u
#define FTM_SYNCONF_SYNCMODE_SHIFT               7
#define FTM_SYNCONF_SYNCMODE_WIDTH               1
#define FTM_SYNCONF_SYNCMODE(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SYNCMODE_SHIFT))&FTM_SYNCONF_SYNCMODE_MASK)
#define FTM_SYNCONF_SWRSTCNT_MASK                0x100u
#define FTM_SYNCONF_SWRSTCNT_SHIFT               8
#define FTM_SYNCONF_SWRSTCNT_WIDTH               1
#define FTM_SYNCONF_SWRSTCNT(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SWRSTCNT_SHIFT))&FTM_SYNCONF_SWRSTCNT_MASK)
#define FTM_SYNCONF_SWWRBUF_MASK                 0x200u
#define FTM_SYNCONF_SWWRBUF_SHIFT                9
#define FTM_SYNCONF_SWWRBUF_WIDTH                1
#define FTM_SYNCONF_SWWRBUF(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SWWRBUF_SHIFT))&FTM_SYNCONF_SWWRBUF_MASK)
#define FTM_SYNCONF_SWOM_MASK                    0x400u
#define FTM_SYNCONF_SWOM_SHIFT                   10
#define FTM_SYNCONF_SWOM_WIDTH                   1
#define FTM_SYNCONF_SWOM(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SWOM_SHIFT))&FTM_SYNCONF_SWOM_MASK)
#define FTM_SYNCONF_SWINVC_MASK                  0x800u
#define FTM_SYNCONF_SWINVC_SHIFT                 11
#define FTM_SYNCONF_SWINVC_WIDTH                 1
#define FTM_SYNCONF_SWINVC(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SWINVC_SHIFT))&FTM_SYNCONF_SWINVC_MASK)
#define FTM_SYNCONF_SWSOC_MASK                   0x1000u
#define FTM_SYNCONF_SWSOC_SHIFT                  12
#define FTM_SYNCONF_SWSOC_WIDTH                  1
#define FTM_SYNCONF_SWSOC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_SWSOC_SHIFT))&FTM_SYNCONF_SWSOC_MASK)
#define FTM_SYNCONF_HWRSTCNT_MASK                0x10000u
#define FTM_SYNCONF_HWRSTCNT_SHIFT               16
#define FTM_SYNCONF_HWRSTCNT_WIDTH               1
#define FTM_SYNCONF_HWRSTCNT(x)                  (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_HWRSTCNT_SHIFT))&FTM_SYNCONF_HWRSTCNT_MASK)
#define FTM_SYNCONF_HWWRBUF_MASK                 0x20000u
#define FTM_SYNCONF_HWWRBUF_SHIFT                17
#define FTM_SYNCONF_HWWRBUF_WIDTH                1
#define FTM_SYNCONF_HWWRBUF(x)                   (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_HWWRBUF_SHIFT))&FTM_SYNCONF_HWWRBUF_MASK)
#define FTM_SYNCONF_HWOM_MASK                    0x40000u
#define FTM_SYNCONF_HWOM_SHIFT                   18
#define FTM_SYNCONF_HWOM_WIDTH                   1
#define FTM_SYNCONF_HWOM(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_HWOM_SHIFT))&FTM_SYNCONF_HWOM_MASK)
#define FTM_SYNCONF_HWINVC_MASK                  0x80000u
#define FTM_SYNCONF_HWINVC_SHIFT                 19
#define FTM_SYNCONF_HWINVC_WIDTH                 1
#define FTM_SYNCONF_HWINVC(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_HWINVC_SHIFT))&FTM_SYNCONF_HWINVC_MASK)
#define FTM_SYNCONF_HWSOC_MASK                   0x100000u
#define FTM_SYNCONF_HWSOC_SHIFT                  20
#define FTM_SYNCONF_HWSOC_WIDTH                  1
#define FTM_SYNCONF_HWSOC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SYNCONF_HWSOC_SHIFT))&FTM_SYNCONF_HWSOC_MASK)
/* INVCTRL Bit Fields */
#define FTM_INVCTRL_INV0EN_MASK                  0x1u
#define FTM_INVCTRL_INV0EN_SHIFT                 0
#define FTM_INVCTRL_INV0EN_WIDTH                 1
#define FTM_INVCTRL_INV0EN(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_INVCTRL_INV0EN_SHIFT))&FTM_INVCTRL_INV0EN_MASK)
#define FTM_INVCTRL_INV1EN_MASK                  0x2u
#define FTM_INVCTRL_INV1EN_SHIFT                 1
#define FTM_INVCTRL_INV1EN_WIDTH                 1
#define FTM_INVCTRL_INV1EN(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_INVCTRL_INV1EN_SHIFT))&FTM_INVCTRL_INV1EN_MASK)
#define FTM_INVCTRL_INV2EN_MASK                  0x4u
#define FTM_INVCTRL_INV2EN_SHIFT                 2
#define FTM_INVCTRL_INV2EN_WIDTH                 1
#define FTM_INVCTRL_INV2EN(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_INVCTRL_INV2EN_SHIFT))&FTM_INVCTRL_INV2EN_MASK)
#define FTM_INVCTRL_INV3EN_MASK                  0x8u
#define FTM_INVCTRL_INV3EN_SHIFT                 3
#define FTM_INVCTRL_INV3EN_WIDTH                 1
#define FTM_INVCTRL_INV3EN(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_INVCTRL_INV3EN_SHIFT))&FTM_INVCTRL_INV3EN_MASK)
/* SWOCTRL Bit Fields */
#define FTM_SWOCTRL_CH0OC_MASK                   0x1u
#define FTM_SWOCTRL_CH0OC_SHIFT                  0
#define FTM_SWOCTRL_CH0OC_WIDTH                  1
#define FTM_SWOCTRL_CH0OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH0OC_SHIFT))&FTM_SWOCTRL_CH0OC_MASK)
#define FTM_SWOCTRL_CH1OC_MASK                   0x2u
#define FTM_SWOCTRL_CH1OC_SHIFT                  1
#define FTM_SWOCTRL_CH1OC_WIDTH                  1
#define FTM_SWOCTRL_CH1OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH1OC_SHIFT))&FTM_SWOCTRL_CH1OC_MASK)
#define FTM_SWOCTRL_CH2OC_MASK                   0x4u
#define FTM_SWOCTRL_CH2OC_SHIFT                  2
#define FTM_SWOCTRL_CH2OC_WIDTH                  1
#define FTM_SWOCTRL_CH2OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH2OC_SHIFT))&FTM_SWOCTRL_CH2OC_MASK)
#define FTM_SWOCTRL_CH3OC_MASK                   0x8u
#define FTM_SWOCTRL_CH3OC_SHIFT                  3
#define FTM_SWOCTRL_CH3OC_WIDTH                  1
#define FTM_SWOCTRL_CH3OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH3OC_SHIFT))&FTM_SWOCTRL_CH3OC_MASK)
#define FTM_SWOCTRL_CH4OC_MASK                   0x10u
#define FTM_SWOCTRL_CH4OC_SHIFT                  4
#define FTM_SWOCTRL_CH4OC_WIDTH                  1
#define FTM_SWOCTRL_CH4OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH4OC_SHIFT))&FTM_SWOCTRL_CH4OC_MASK)
#define FTM_SWOCTRL_CH5OC_MASK                   0x20u
#define FTM_SWOCTRL_CH5OC_SHIFT                  5
#define FTM_SWOCTRL_CH5OC_WIDTH                  1
#define FTM_SWOCTRL_CH5OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH5OC_SHIFT))&FTM_SWOCTRL_CH5OC_MASK)
#define FTM_SWOCTRL_CH6OC_MASK                   0x40u
#define FTM_SWOCTRL_CH6OC_SHIFT                  6
#define FTM_SWOCTRL_CH6OC_WIDTH                  1
#define FTM_SWOCTRL_CH6OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH6OC_SHIFT))&FTM_SWOCTRL_CH6OC_MASK)
#define FTM_SWOCTRL_CH7OC_MASK                   0x80u
#define FTM_SWOCTRL_CH7OC_SHIFT                  7
#define FTM_SWOCTRL_CH7OC_WIDTH                  1
#define FTM_SWOCTRL_CH7OC(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH7OC_SHIFT))&FTM_SWOCTRL_CH7OC_MASK)
#define FTM_SWOCTRL_CH0OCV_MASK                  0x100u
#define FTM_SWOCTRL_CH0OCV_SHIFT                 8
#define FTM_SWOCTRL_CH0OCV_WIDTH                 1
#define FTM_SWOCTRL_CH0OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH0OCV_SHIFT))&FTM_SWOCTRL_CH0OCV_MASK)
#define FTM_SWOCTRL_CH1OCV_MASK                  0x200u
#define FTM_SWOCTRL_CH1OCV_SHIFT                 9
#define FTM_SWOCTRL_CH1OCV_WIDTH                 1
#define FTM_SWOCTRL_CH1OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH1OCV_SHIFT))&FTM_SWOCTRL_CH1OCV_MASK)
#define FTM_SWOCTRL_CH2OCV_MASK                  0x400u
#define FTM_SWOCTRL_CH2OCV_SHIFT                 10
#define FTM_SWOCTRL_CH2OCV_WIDTH                 1
#define FTM_SWOCTRL_CH2OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH2OCV_SHIFT))&FTM_SWOCTRL_CH2OCV_MASK)
#define FTM_SWOCTRL_CH3OCV_MASK                  0x800u
#define FTM_SWOCTRL_CH3OCV_SHIFT                 11
#define FTM_SWOCTRL_CH3OCV_WIDTH                 1
#define FTM_SWOCTRL_CH3OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH3OCV_SHIFT))&FTM_SWOCTRL_CH3OCV_MASK)
#define FTM_SWOCTRL_CH4OCV_MASK                  0x1000u
#define FTM_SWOCTRL_CH4OCV_SHIFT                 12
#define FTM_SWOCTRL_CH4OCV_WIDTH                 1
#define FTM_SWOCTRL_CH4OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH4OCV_SHIFT))&FTM_SWOCTRL_CH4OCV_MASK)
#define FTM_SWOCTRL_CH5OCV_MASK                  0x2000u
#define FTM_SWOCTRL_CH5OCV_SHIFT                 13
#define FTM_SWOCTRL_CH5OCV_WIDTH                 1
#define FTM_SWOCTRL_CH5OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH5OCV_SHIFT))&FTM_SWOCTRL_CH5OCV_MASK)
#define FTM_SWOCTRL_CH6OCV_MASK                  0x4000u
#define FTM_SWOCTRL_CH6OCV_SHIFT                 14
#define FTM_SWOCTRL_CH6OCV_WIDTH                 1
#define FTM_SWOCTRL_CH6OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH6OCV_SHIFT))&FTM_SWOCTRL_CH6OCV_MASK)
#define FTM_SWOCTRL_CH7OCV_MASK                  0x8000u
#define FTM_SWOCTRL_CH7OCV_SHIFT                 15
#define FTM_SWOCTRL_CH7OCV_WIDTH                 1
#define FTM_SWOCTRL_CH7OCV(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_SWOCTRL_CH7OCV_SHIFT))&FTM_SWOCTRL_CH7OCV_MASK)
/* PWMLOAD Bit Fields */
#define FTM_PWMLOAD_CH0SEL_MASK                  0x1u
#define FTM_PWMLOAD_CH0SEL_SHIFT                 0
#define FTM_PWMLOAD_CH0SEL_WIDTH                 1
#define FTM_PWMLOAD_CH0SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH0SEL_SHIFT))&FTM_PWMLOAD_CH0SEL_MASK)
#define FTM_PWMLOAD_CH1SEL_MASK                  0x2u
#define FTM_PWMLOAD_CH1SEL_SHIFT                 1
#define FTM_PWMLOAD_CH1SEL_WIDTH                 1
#define FTM_PWMLOAD_CH1SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH1SEL_SHIFT))&FTM_PWMLOAD_CH1SEL_MASK)
#define FTM_PWMLOAD_CH2SEL_MASK                  0x4u
#define FTM_PWMLOAD_CH2SEL_SHIFT                 2
#define FTM_PWMLOAD_CH2SEL_WIDTH                 1
#define FTM_PWMLOAD_CH2SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH2SEL_SHIFT))&FTM_PWMLOAD_CH2SEL_MASK)
#define FTM_PWMLOAD_CH3SEL_MASK                  0x8u
#define FTM_PWMLOAD_CH3SEL_SHIFT                 3
#define FTM_PWMLOAD_CH3SEL_WIDTH                 1
#define FTM_PWMLOAD_CH3SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH3SEL_SHIFT))&FTM_PWMLOAD_CH3SEL_MASK)
#define FTM_PWMLOAD_CH4SEL_MASK                  0x10u
#define FTM_PWMLOAD_CH4SEL_SHIFT                 4
#define FTM_PWMLOAD_CH4SEL_WIDTH                 1
#define FTM_PWMLOAD_CH4SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH4SEL_SHIFT))&FTM_PWMLOAD_CH4SEL_MASK)
#define FTM_PWMLOAD_CH5SEL_MASK                  0x20u
#define FTM_PWMLOAD_CH5SEL_SHIFT                 5
#define FTM_PWMLOAD_CH5SEL_WIDTH                 1
#define FTM_PWMLOAD_CH5SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH5SEL_SHIFT))&FTM_PWMLOAD_CH5SEL_MASK)
#define FTM_PWMLOAD_CH6SEL_MASK                  0x40u
#define FTM_PWMLOAD_CH6SEL_SHIFT                 6
#define FTM_PWMLOAD_CH6SEL_WIDTH                 1
#define FTM_PWMLOAD_CH6SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH6SEL_SHIFT))&FTM_PWMLOAD_CH6SEL_MASK)
#define FTM_PWMLOAD_CH7SEL_MASK                  0x80u
#define FTM_PWMLOAD_CH7SEL_SHIFT                 7
#define FTM_PWMLOAD_CH7SEL_WIDTH                 1
#define FTM_PWMLOAD_CH7SEL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_CH7SEL_SHIFT))&FTM_PWMLOAD_CH7SEL_MASK)
#define FTM_PWMLOAD_LDOK_MASK                    0x200u
#define FTM_PWMLOAD_LDOK_SHIFT                   9
#define FTM_PWMLOAD_LDOK_WIDTH                   1
#define FTM_PWMLOAD_LDOK(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_PWMLOAD_LDOK_SHIFT))&FTM_PWMLOAD_LDOK_MASK)

/*!
 * @}
 */ /* end of group FTM_Register_Masks */


/* FTM - Peripheral instance base addresses */
/** Peripheral FTM0 base address */
#define FTM0_BASE                                (0x40038000u)
/** Peripheral FTM0 base pointer */
#define FTM0                                     ((FTM_Type *)FTM0_BASE)
#define FTM0_BASE_PTR                            (FTM0)
/** Peripheral FTM1 base address */
#define FTM1_BASE                                (0x40039000u)
/** Peripheral FTM1 base pointer */
#define FTM1                                     ((FTM_Type *)FTM1_BASE)
#define FTM1_BASE_PTR                            (FTM1)
/** Peripheral FTM2 base address */
#define FTM2_BASE                                (0x4003A000u)
/** Peripheral FTM2 base pointer */
#define FTM2                                     ((FTM_Type *)FTM2_BASE)
#define FTM2_BASE_PTR                            (FTM2)
/** Array initializer of FTM peripheral base addresses */
#define FTM_BASE_ADDRS                           { FTM0_BASE, FTM1_BASE, FTM2_BASE }
/** Array initializer of FTM peripheral base pointers */
#define FTM_BASE_PTRS                            { FTM0, FTM1, FTM2 }

/* ----------------------------------------------------------------------------
   -- FTM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTM_Register_Accessor_Macros FTM - Register accessor macros
 * @{
 */


/* FTM - Register instance definitions */
/* FTM0 */
#define FTM0_SC                                  FTM_SC_REG(FTM0)
#define FTM0_CNT                                 FTM_CNT_REG(FTM0)
#define FTM0_MOD                                 FTM_MOD_REG(FTM0)
#define FTM0_C0SC                                FTM_CnSC_REG(FTM0,0)
#define FTM0_C0V                                 FTM_CnV_REG(FTM0,0)
#define FTM0_C1SC                                FTM_CnSC_REG(FTM0,1)
#define FTM0_C1V                                 FTM_CnV_REG(FTM0,1)
#define FTM0_EXTTRIG                             FTM_EXTTRIG_REG(FTM0)
/* FTM1 */
#define FTM1_SC                                  FTM_SC_REG(FTM1)
#define FTM1_CNT                                 FTM_CNT_REG(FTM1)
#define FTM1_MOD                                 FTM_MOD_REG(FTM1)
#define FTM1_C0SC                                FTM_CnSC_REG(FTM1,0)
#define FTM1_C0V                                 FTM_CnV_REG(FTM1,0)
#define FTM1_C1SC                                FTM_CnSC_REG(FTM1,1)
#define FTM1_C1V                                 FTM_CnV_REG(FTM1,1)
/* FTM2 */
#define FTM2_SC                                  FTM_SC_REG(FTM2)
#define FTM2_CNT                                 FTM_CNT_REG(FTM2)
#define FTM2_MOD                                 FTM_MOD_REG(FTM2)
#define FTM2_C0SC                                FTM_CnSC_REG(FTM2,0)
#define FTM2_C0V                                 FTM_CnV_REG(FTM2,0)
#define FTM2_C1SC                                FTM_CnSC_REG(FTM2,1)
#define FTM2_C1V                                 FTM_CnV_REG(FTM2,1)
#define FTM2_C2SC                                FTM_CnSC_REG(FTM2,2)
#define FTM2_C2V                                 FTM_CnV_REG(FTM2,2)
#define FTM2_C3SC                                FTM_CnSC_REG(FTM2,3)
#define FTM2_C3V                                 FTM_CnV_REG(FTM2,3)
#define FTM2_C4SC                                FTM_CnSC_REG(FTM2,4)
#define FTM2_C4V                                 FTM_CnV_REG(FTM2,4)
#define FTM2_C5SC                                FTM_CnSC_REG(FTM2,5)
#define FTM2_C5V                                 FTM_CnV_REG(FTM2,5)
#define FTM2_CNTIN                               FTM_CNTIN_REG(FTM2)
#define FTM2_STATUS                              FTM_STATUS_REG(FTM2)
#define FTM2_MODE                                FTM_MODE_REG(FTM2)
#define FTM2_SYNC                                FTM_SYNC_REG(FTM2)
#define FTM2_OUTINIT                             FTM_OUTINIT_REG(FTM2)
#define FTM2_OUTMASK                             FTM_OUTMASK_REG(FTM2)
#define FTM2_COMBINE                             FTM_COMBINE_REG(FTM2)
#define FTM2_DEADTIME                            FTM_DEADTIME_REG(FTM2)
#define FTM2_EXTTRIG                             FTM_EXTTRIG_REG(FTM2)
#define FTM2_POL                                 FTM_POL_REG(FTM2)
#define FTM2_FMS                                 FTM_FMS_REG(FTM2)
#define FTM2_FILTER                              FTM_FILTER_REG(FTM2)
#define FTM2_FLTCTRL                             FTM_FLTCTRL_REG(FTM2)
#define FTM2_CONF                                FTM_CONF_REG(FTM2)
#define FTM2_FLTPOL                              FTM_FLTPOL_REG(FTM2)
#define FTM2_SYNCONF                             FTM_SYNCONF_REG(FTM2)
#define FTM2_INVCTRL                             FTM_INVCTRL_REG(FTM2)
#define FTM2_SWOCTRL                             FTM_SWOCTRL_REG(FTM2)
#define FTM2_PWMLOAD                             FTM_PWMLOAD_REG(FTM2)

/* FTM - Register array accessors */
#define FTM0_CnSC(index)                         FTM_CnSC_REG(FTM0,index)
#define FTM1_CnSC(index)                         FTM_CnSC_REG(FTM1,index)
#define FTM2_CnSC(index)                         FTM_CnSC_REG(FTM2,index)
#define FTM0_CnV(index)                          FTM_CnV_REG(FTM0,index)
#define FTM1_CnV(index)                          FTM_CnV_REG(FTM1,index)
#define FTM2_CnV(index)                          FTM_CnV_REG(FTM2,index)

/*!
 * @}
 */ /* end of group FTM_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group FTM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FTMRE Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTMRE_Peripheral_Access_Layer FTMRE Peripheral Access Layer
 * @{
 */

/** FTMRE - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[1];
  __IO uint8_t FCCOBIX;                            /**< Flash CCOB Index Register, offset: 0x1 */
  __I  uint8_t FSEC;                               /**< Flash Security Register, offset: 0x2 */
  __IO uint8_t FCLKDIV;                            /**< Flash Clock Divider Register, offset: 0x3 */
       uint8_t RESERVED_1[1];
  __IO uint8_t FSTAT;                              /**< Flash Status Register, offset: 0x5 */
       uint8_t RESERVED_2[1];
  __IO uint8_t FCNFG;                              /**< Flash Configuration Register, offset: 0x7 */
  __IO uint8_t FCCOBLO;                            /**< Flash Common Command Object Register: Low, offset: 0x8 */
  __IO uint8_t FCCOBHI;                            /**< Flash Common Command Object Register:High, offset: 0x9 */
       uint8_t RESERVED_3[1];
  __IO uint8_t FPROT;                              /**< Flash Protection Register, offset: 0xB */
       uint8_t RESERVED_4[3];
  __I  uint8_t FOPT;                               /**< Flash Option Register, offset: 0xF */
} FTMRE_Type, *FTMRE_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- FTMRE - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTMRE_Register_Accessor_Macros FTMRE - Register accessor macros
 * @{
 */


/* FTMRE - Register accessors */
#define FTMRE_FCCOBIX_REG(base)                  ((base)->FCCOBIX)
#define FTMRE_FSEC_REG(base)                     ((base)->FSEC)
#define FTMRE_FCLKDIV_REG(base)                  ((base)->FCLKDIV)
#define FTMRE_FSTAT_REG(base)                    ((base)->FSTAT)
#define FTMRE_FCNFG_REG(base)                    ((base)->FCNFG)
#define FTMRE_FCCOBLO_REG(base)                  ((base)->FCCOBLO)
#define FTMRE_FCCOBHI_REG(base)                  ((base)->FCCOBHI)
#define FTMRE_FPROT_REG(base)                    ((base)->FPROT)
#define FTMRE_FOPT_REG(base)                     ((base)->FOPT)

/*!
 * @}
 */ /* end of group FTMRE_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- FTMRE Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTMRE_Register_Masks FTMRE Register Masks
 * @{
 */

/* FCCOBIX Bit Fields */
#define FTMRE_FCCOBIX_CCOBIX_MASK                0x7u
#define FTMRE_FCCOBIX_CCOBIX_SHIFT               0
#define FTMRE_FCCOBIX_CCOBIX_WIDTH               3
#define FTMRE_FCCOBIX_CCOBIX(x)                  (((uint8_t)(((uint8_t)(x))<<FTMRE_FCCOBIX_CCOBIX_SHIFT))&FTMRE_FCCOBIX_CCOBIX_MASK)
/* FSEC Bit Fields */
#define FTMRE_FSEC_SEC_MASK                      0x3u
#define FTMRE_FSEC_SEC_SHIFT                     0
#define FTMRE_FSEC_SEC_WIDTH                     2
#define FTMRE_FSEC_SEC(x)                        (((uint8_t)(((uint8_t)(x))<<FTMRE_FSEC_SEC_SHIFT))&FTMRE_FSEC_SEC_MASK)
#define FTMRE_FSEC_KEYEN_MASK                    0xC0u
#define FTMRE_FSEC_KEYEN_SHIFT                   6
#define FTMRE_FSEC_KEYEN_WIDTH                   2
#define FTMRE_FSEC_KEYEN(x)                      (((uint8_t)(((uint8_t)(x))<<FTMRE_FSEC_KEYEN_SHIFT))&FTMRE_FSEC_KEYEN_MASK)
/* FCLKDIV Bit Fields */
#define FTMRE_FCLKDIV_FDIV_MASK                  0x3Fu
#define FTMRE_FCLKDIV_FDIV_SHIFT                 0
#define FTMRE_FCLKDIV_FDIV_WIDTH                 6
#define FTMRE_FCLKDIV_FDIV(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FCLKDIV_FDIV_SHIFT))&FTMRE_FCLKDIV_FDIV_MASK)
#define FTMRE_FCLKDIV_FDIVLCK_MASK               0x40u
#define FTMRE_FCLKDIV_FDIVLCK_SHIFT              6
#define FTMRE_FCLKDIV_FDIVLCK_WIDTH              1
#define FTMRE_FCLKDIV_FDIVLCK(x)                 (((uint8_t)(((uint8_t)(x))<<FTMRE_FCLKDIV_FDIVLCK_SHIFT))&FTMRE_FCLKDIV_FDIVLCK_MASK)
#define FTMRE_FCLKDIV_FDIVLD_MASK                0x80u
#define FTMRE_FCLKDIV_FDIVLD_SHIFT               7
#define FTMRE_FCLKDIV_FDIVLD_WIDTH               1
#define FTMRE_FCLKDIV_FDIVLD(x)                  (((uint8_t)(((uint8_t)(x))<<FTMRE_FCLKDIV_FDIVLD_SHIFT))&FTMRE_FCLKDIV_FDIVLD_MASK)
/* FSTAT Bit Fields */
#define FTMRE_FSTAT_MGSTAT_MASK                  0x3u
#define FTMRE_FSTAT_MGSTAT_SHIFT                 0
#define FTMRE_FSTAT_MGSTAT_WIDTH                 2
#define FTMRE_FSTAT_MGSTAT(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FSTAT_MGSTAT_SHIFT))&FTMRE_FSTAT_MGSTAT_MASK)
#define FTMRE_FSTAT_MGBUSY_MASK                  0x8u
#define FTMRE_FSTAT_MGBUSY_SHIFT                 3
#define FTMRE_FSTAT_MGBUSY_WIDTH                 1
#define FTMRE_FSTAT_MGBUSY(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FSTAT_MGBUSY_SHIFT))&FTMRE_FSTAT_MGBUSY_MASK)
#define FTMRE_FSTAT_FPVIOL_MASK                  0x10u
#define FTMRE_FSTAT_FPVIOL_SHIFT                 4
#define FTMRE_FSTAT_FPVIOL_WIDTH                 1
#define FTMRE_FSTAT_FPVIOL(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FSTAT_FPVIOL_SHIFT))&FTMRE_FSTAT_FPVIOL_MASK)
#define FTMRE_FSTAT_ACCERR_MASK                  0x20u
#define FTMRE_FSTAT_ACCERR_SHIFT                 5
#define FTMRE_FSTAT_ACCERR_WIDTH                 1
#define FTMRE_FSTAT_ACCERR(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FSTAT_ACCERR_SHIFT))&FTMRE_FSTAT_ACCERR_MASK)
#define FTMRE_FSTAT_CCIF_MASK                    0x80u
#define FTMRE_FSTAT_CCIF_SHIFT                   7
#define FTMRE_FSTAT_CCIF_WIDTH                   1
#define FTMRE_FSTAT_CCIF(x)                      (((uint8_t)(((uint8_t)(x))<<FTMRE_FSTAT_CCIF_SHIFT))&FTMRE_FSTAT_CCIF_MASK)
/* FCNFG Bit Fields */
#define FTMRE_FCNFG_ERSAREQ_MASK                 0x20u
#define FTMRE_FCNFG_ERSAREQ_SHIFT                5
#define FTMRE_FCNFG_ERSAREQ_WIDTH                1
#define FTMRE_FCNFG_ERSAREQ(x)                   (((uint8_t)(((uint8_t)(x))<<FTMRE_FCNFG_ERSAREQ_SHIFT))&FTMRE_FCNFG_ERSAREQ_MASK)
#define FTMRE_FCNFG_CCIE_MASK                    0x80u
#define FTMRE_FCNFG_CCIE_SHIFT                   7
#define FTMRE_FCNFG_CCIE_WIDTH                   1
#define FTMRE_FCNFG_CCIE(x)                      (((uint8_t)(((uint8_t)(x))<<FTMRE_FCNFG_CCIE_SHIFT))&FTMRE_FCNFG_CCIE_MASK)
/* FCCOBLO Bit Fields */
#define FTMRE_FCCOBLO_CCOB_MASK                  0xFFu
#define FTMRE_FCCOBLO_CCOB_SHIFT                 0
#define FTMRE_FCCOBLO_CCOB_WIDTH                 8
#define FTMRE_FCCOBLO_CCOB(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FCCOBLO_CCOB_SHIFT))&FTMRE_FCCOBLO_CCOB_MASK)
/* FCCOBHI Bit Fields */
#define FTMRE_FCCOBHI_CCOB_MASK                  0xFFu
#define FTMRE_FCCOBHI_CCOB_SHIFT                 0
#define FTMRE_FCCOBHI_CCOB_WIDTH                 8
#define FTMRE_FCCOBHI_CCOB(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FCCOBHI_CCOB_SHIFT))&FTMRE_FCCOBHI_CCOB_MASK)
/* FPROT Bit Fields */
#define FTMRE_FPROT_FPLS_MASK                    0x3u
#define FTMRE_FPROT_FPLS_SHIFT                   0
#define FTMRE_FPROT_FPLS_WIDTH                   2
#define FTMRE_FPROT_FPLS(x)                      (((uint8_t)(((uint8_t)(x))<<FTMRE_FPROT_FPLS_SHIFT))&FTMRE_FPROT_FPLS_MASK)
#define FTMRE_FPROT_FPLDIS_MASK                  0x4u
#define FTMRE_FPROT_FPLDIS_SHIFT                 2
#define FTMRE_FPROT_FPLDIS_WIDTH                 1
#define FTMRE_FPROT_FPLDIS(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FPROT_FPLDIS_SHIFT))&FTMRE_FPROT_FPLDIS_MASK)
#define FTMRE_FPROT_FPHS_MASK                    0x18u
#define FTMRE_FPROT_FPHS_SHIFT                   3
#define FTMRE_FPROT_FPHS_WIDTH                   2
#define FTMRE_FPROT_FPHS(x)                      (((uint8_t)(((uint8_t)(x))<<FTMRE_FPROT_FPHS_SHIFT))&FTMRE_FPROT_FPHS_MASK)
#define FTMRE_FPROT_FPHDIS_MASK                  0x20u
#define FTMRE_FPROT_FPHDIS_SHIFT                 5
#define FTMRE_FPROT_FPHDIS_WIDTH                 1
#define FTMRE_FPROT_FPHDIS(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FPROT_FPHDIS_SHIFT))&FTMRE_FPROT_FPHDIS_MASK)
#define FTMRE_FPROT_RNV6_MASK                    0x40u
#define FTMRE_FPROT_RNV6_SHIFT                   6
#define FTMRE_FPROT_RNV6_WIDTH                   1
#define FTMRE_FPROT_RNV6(x)                      (((uint8_t)(((uint8_t)(x))<<FTMRE_FPROT_RNV6_SHIFT))&FTMRE_FPROT_RNV6_MASK)
#define FTMRE_FPROT_FPOPEN_MASK                  0x80u
#define FTMRE_FPROT_FPOPEN_SHIFT                 7
#define FTMRE_FPROT_FPOPEN_WIDTH                 1
#define FTMRE_FPROT_FPOPEN(x)                    (((uint8_t)(((uint8_t)(x))<<FTMRE_FPROT_FPOPEN_SHIFT))&FTMRE_FPROT_FPOPEN_MASK)
/* FOPT Bit Fields */
#define FTMRE_FOPT_NV_MASK                       0xFFu
#define FTMRE_FOPT_NV_SHIFT                      0
#define FTMRE_FOPT_NV_WIDTH                      8
#define FTMRE_FOPT_NV(x)                         (((uint8_t)(((uint8_t)(x))<<FTMRE_FOPT_NV_SHIFT))&FTMRE_FOPT_NV_MASK)

/*!
 * @}
 */ /* end of group FTMRE_Register_Masks */


/* FTMRE - Peripheral instance base addresses */
/** Peripheral FTMRE base address */
#define FTMRE_BASE                               (0x40020000u)
/** Peripheral FTMRE base pointer */
#define FTMRE                                    ((FTMRE_Type *)FTMRE_BASE)
#define FTMRE_BASE_PTR                           (FTMRE)
/** Array initializer of FTMRE peripheral base addresses */
#define FTMRE_BASE_ADDRS                         { FTMRE_BASE }
/** Array initializer of FTMRE peripheral base pointers */
#define FTMRE_BASE_PTRS                          { FTMRE }

/* ----------------------------------------------------------------------------
   -- FTMRE - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTMRE_Register_Accessor_Macros FTMRE - Register accessor macros
 * @{
 */


/* FTMRE - Register instance definitions */
/* FTMRE */
#define FTMRE_FCCOBIX                            FTMRE_FCCOBIX_REG(FTMRE)
#define FTMRE_FSEC                               FTMRE_FSEC_REG(FTMRE)
#define FTMRE_FCLKDIV                            FTMRE_FCLKDIV_REG(FTMRE)
#define FTMRE_FSTAT                              FTMRE_FSTAT_REG(FTMRE)
#define FTMRE_FCNFG                              FTMRE_FCNFG_REG(FTMRE)
#define FTMRE_FCCOBLO                            FTMRE_FCCOBLO_REG(FTMRE)
#define FTMRE_FCCOBHI                            FTMRE_FCCOBHI_REG(FTMRE)
#define FTMRE_FPROT                              FTMRE_FPROT_REG(FTMRE)
#define FTMRE_FOPT                               FTMRE_FOPT_REG(FTMRE)

/*!
 * @}
 */ /* end of group FTMRE_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group FTMRE_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Peripheral_Access_Layer GPIO Peripheral Access Layer
 * @{
 */

/** GPIO - Register Layout Typedef */
typedef struct {
  __IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
  __O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
  __O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
  __IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
  __IO uint32_t PIDR;                              /**< Port Input Disable Register, offset: 0x18 */
} GPIO_Type, *GPIO_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- GPIO - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Accessor_Macros GPIO - Register accessor macros
 * @{
 */


/* GPIO - Register accessors */
#define GPIO_PDOR_REG(base)                      ((base)->PDOR)
#define GPIO_PSOR_REG(base)                      ((base)->PSOR)
#define GPIO_PCOR_REG(base)                      ((base)->PCOR)
#define GPIO_PTOR_REG(base)                      ((base)->PTOR)
#define GPIO_PDIR_REG(base)                      ((base)->PDIR)
#define GPIO_PDDR_REG(base)                      ((base)->PDDR)
#define GPIO_PIDR_REG(base)                      ((base)->PIDR)

/*!
 * @}
 */ /* end of group GPIO_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- GPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Masks GPIO Register Masks
 * @{
 */

/* PDOR Bit Fields */
#define GPIO_PDOR_PDO_MASK                       0xFFFFFFFFu
#define GPIO_PDOR_PDO_SHIFT                      0
#define GPIO_PDOR_PDO_WIDTH                      32
#define GPIO_PDOR_PDO(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDOR_PDO_SHIFT))&GPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define GPIO_PSOR_PTSO_MASK                      0xFFFFFFFFu
#define GPIO_PSOR_PTSO_SHIFT                     0
#define GPIO_PSOR_PTSO_WIDTH                     32
#define GPIO_PSOR_PTSO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PSOR_PTSO_SHIFT))&GPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define GPIO_PCOR_PTCO_MASK                      0xFFFFFFFFu
#define GPIO_PCOR_PTCO_SHIFT                     0
#define GPIO_PCOR_PTCO_WIDTH                     32
#define GPIO_PCOR_PTCO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PCOR_PTCO_SHIFT))&GPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define GPIO_PTOR_PTTO_MASK                      0xFFFFFFFFu
#define GPIO_PTOR_PTTO_SHIFT                     0
#define GPIO_PTOR_PTTO_WIDTH                     32
#define GPIO_PTOR_PTTO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PTOR_PTTO_SHIFT))&GPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define GPIO_PDIR_PDI_MASK                       0xFFFFFFFFu
#define GPIO_PDIR_PDI_SHIFT                      0
#define GPIO_PDIR_PDI_WIDTH                      32
#define GPIO_PDIR_PDI(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDIR_PDI_SHIFT))&GPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define GPIO_PDDR_PDD_MASK                       0xFFFFFFFFu
#define GPIO_PDDR_PDD_SHIFT                      0
#define GPIO_PDDR_PDD_WIDTH                      32
#define GPIO_PDDR_PDD(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDDR_PDD_SHIFT))&GPIO_PDDR_PDD_MASK)
/* PIDR Bit Fields */
#define GPIO_PIDR_PID_MASK                       0xFFFFFFFFu
#define GPIO_PIDR_PID_SHIFT                      0
#define GPIO_PIDR_PID_WIDTH                      32
#define GPIO_PIDR_PID(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PIDR_PID_SHIFT))&GPIO_PIDR_PID_MASK)

/*!
 * @}
 */ /* end of group GPIO_Register_Masks */


/* GPIO - Peripheral instance base addresses */
/** Peripheral GPIOA base address */
#define GPIOA_BASE                               (0x400FF000u)
/** Peripheral GPIOA base pointer */
#define GPIOA                                    ((GPIO_Type *)GPIOA_BASE)
#define GPIOA_BASE_PTR                           (GPIOA)
/** Peripheral GPIOB base address */
#define GPIOB_BASE                               (0x400FF040u)
/** Peripheral GPIOB base pointer */
#define GPIOB                                    ((GPIO_Type *)GPIOB_BASE)
#define GPIOB_BASE_PTR                           (GPIOB)
/** Peripheral GPIOC base address */
#define GPIOC_BASE                               (0x400FF080u)
/** Peripheral GPIOC base pointer */
#define GPIOC                                    ((GPIO_Type *)GPIOC_BASE)
#define GPIOC_BASE_PTR                           (GPIOC)
/** Array initializer of GPIO peripheral base addresses */
#define GPIO_BASE_ADDRS                          { GPIOA_BASE, GPIOB_BASE, GPIOC_BASE }
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASE_PTRS                           { GPIOA, GPIOB, GPIOC }

/* ----------------------------------------------------------------------------
   -- GPIO - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Accessor_Macros GPIO - Register accessor macros
 * @{
 */


/* GPIO - Register instance definitions */
/* GPIOA */
#define GPIOA_PDOR                               GPIO_PDOR_REG(GPIOA)
#define GPIOA_PSOR                               GPIO_PSOR_REG(GPIOA)
#define GPIOA_PCOR                               GPIO_PCOR_REG(GPIOA)
#define GPIOA_PTOR                               GPIO_PTOR_REG(GPIOA)
#define GPIOA_PDIR                               GPIO_PDIR_REG(GPIOA)
#define GPIOA_PDDR                               GPIO_PDDR_REG(GPIOA)
#define GPIOA_PIDR                               GPIO_PIDR_REG(GPIOA)
/* GPIOB */
#define GPIOB_PDOR                               GPIO_PDOR_REG(GPIOB)
#define GPIOB_PSOR                               GPIO_PSOR_REG(GPIOB)
#define GPIOB_PCOR                               GPIO_PCOR_REG(GPIOB)
#define GPIOB_PTOR                               GPIO_PTOR_REG(GPIOB)
#define GPIOB_PDIR                               GPIO_PDIR_REG(GPIOB)
#define GPIOB_PDDR                               GPIO_PDDR_REG(GPIOB)
#define GPIOB_PIDR                               GPIO_PIDR_REG(GPIOB)
/* GPIOC */
#define GPIOC_PDOR                               GPIO_PDOR_REG(GPIOC)
#define GPIOC_PSOR                               GPIO_PSOR_REG(GPIOC)
#define GPIOC_PCOR                               GPIO_PCOR_REG(GPIOC)
#define GPIOC_PTOR                               GPIO_PTOR_REG(GPIOC)
#define GPIOC_PDIR                               GPIO_PDIR_REG(GPIOC)
#define GPIOC_PDDR                               GPIO_PDDR_REG(GPIOC)
#define GPIOC_PIDR                               GPIO_PIDR_REG(GPIOC)

/*!
 * @}
 */ /* end of group GPIO_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group GPIO_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Peripheral_Access_Layer I2C Peripheral Access Layer
 * @{
 */

/** I2C - Register Layout Typedef */
typedef struct {
  __IO uint8_t A1;                                 /**< I2C Address Register 1, offset: 0x0 */
  __IO uint8_t F;                                  /**< I2C Frequency Divider register, offset: 0x1 */
  __IO uint8_t C1;                                 /**< I2C Control Register 1, offset: 0x2 */
  __IO uint8_t S1;                                 /**< I2C Status register 1, offset: 0x3 */
  __IO uint8_t D;                                  /**< I2C Data I/O register, offset: 0x4 */
  __IO uint8_t C2;                                 /**< I2C Control Register 2, offset: 0x5 */
  __IO uint8_t FLT;                                /**< I2C Programmable Input Glitch Filter register, offset: 0x6 */
  __IO uint8_t RA;                                 /**< I2C Range Address register, offset: 0x7 */
  __IO uint8_t SMB;                                /**< I2C SMBus Control and Status register, offset: 0x8 */
  __IO uint8_t A2;                                 /**< I2C Address Register 2, offset: 0x9 */
  __IO uint8_t SLTH;                               /**< I2C SCL Low Timeout Register High, offset: 0xA */
  __IO uint8_t SLTL;                               /**< I2C SCL Low Timeout Register Low, offset: 0xB */
} I2C_Type, *I2C_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- I2C - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Register_Accessor_Macros I2C - Register accessor macros
 * @{
 */


/* I2C - Register accessors */
#define I2C_A1_REG(base)                         ((base)->A1)
#define I2C_F_REG(base)                          ((base)->F)
#define I2C_C1_REG(base)                         ((base)->C1)
#define I2C_S1_REG(base)                         ((base)->S1)
#define I2C_D_REG(base)                          ((base)->D)
#define I2C_C2_REG(base)                         ((base)->C2)
#define I2C_FLT_REG(base)                        ((base)->FLT)
#define I2C_RA_REG(base)                         ((base)->RA)
#define I2C_SMB_REG(base)                        ((base)->SMB)
#define I2C_A2_REG(base)                         ((base)->A2)
#define I2C_SLTH_REG(base)                       ((base)->SLTH)
#define I2C_SLTL_REG(base)                       ((base)->SLTL)

/*!
 * @}
 */ /* end of group I2C_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- I2C Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Register_Masks I2C Register Masks
 * @{
 */

/* A1 Bit Fields */
#define I2C_A1_AD_MASK                           0xFEu
#define I2C_A1_AD_SHIFT                          1
#define I2C_A1_AD_WIDTH                          7
#define I2C_A1_AD(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_A1_AD_SHIFT))&I2C_A1_AD_MASK)
/* F Bit Fields */
#define I2C_F_ICR_MASK                           0x3Fu
#define I2C_F_ICR_SHIFT                          0
#define I2C_F_ICR_WIDTH                          6
#define I2C_F_ICR(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_F_ICR_SHIFT))&I2C_F_ICR_MASK)
#define I2C_F_MULT_MASK                          0xC0u
#define I2C_F_MULT_SHIFT                         6
#define I2C_F_MULT_WIDTH                         2
#define I2C_F_MULT(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_F_MULT_SHIFT))&I2C_F_MULT_MASK)
/* C1 Bit Fields */
#define I2C_C1_WUEN_MASK                         0x2u
#define I2C_C1_WUEN_SHIFT                        1
#define I2C_C1_WUEN_WIDTH                        1
#define I2C_C1_WUEN(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_C1_WUEN_SHIFT))&I2C_C1_WUEN_MASK)
#define I2C_C1_RSTA_MASK                         0x4u
#define I2C_C1_RSTA_SHIFT                        2
#define I2C_C1_RSTA_WIDTH                        1
#define I2C_C1_RSTA(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_C1_RSTA_SHIFT))&I2C_C1_RSTA_MASK)
#define I2C_C1_TXAK_MASK                         0x8u
#define I2C_C1_TXAK_SHIFT                        3
#define I2C_C1_TXAK_WIDTH                        1
#define I2C_C1_TXAK(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_C1_TXAK_SHIFT))&I2C_C1_TXAK_MASK)
#define I2C_C1_TX_MASK                           0x10u
#define I2C_C1_TX_SHIFT                          4
#define I2C_C1_TX_WIDTH                          1
#define I2C_C1_TX(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_C1_TX_SHIFT))&I2C_C1_TX_MASK)
#define I2C_C1_MST_MASK                          0x20u
#define I2C_C1_MST_SHIFT                         5
#define I2C_C1_MST_WIDTH                         1
#define I2C_C1_MST(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_C1_MST_SHIFT))&I2C_C1_MST_MASK)
#define I2C_C1_IICIE_MASK                        0x40u
#define I2C_C1_IICIE_SHIFT                       6
#define I2C_C1_IICIE_WIDTH                       1
#define I2C_C1_IICIE(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_C1_IICIE_SHIFT))&I2C_C1_IICIE_MASK)
#define I2C_C1_IICEN_MASK                        0x80u
#define I2C_C1_IICEN_SHIFT                       7
#define I2C_C1_IICEN_WIDTH                       1
#define I2C_C1_IICEN(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_C1_IICEN_SHIFT))&I2C_C1_IICEN_MASK)
/* S1 Bit Fields */
#define I2C_S1_RXAK_MASK                         0x1u
#define I2C_S1_RXAK_SHIFT                        0
#define I2C_S1_RXAK_WIDTH                        1
#define I2C_S1_RXAK(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_S1_RXAK_SHIFT))&I2C_S1_RXAK_MASK)
#define I2C_S1_IICIF_MASK                        0x2u
#define I2C_S1_IICIF_SHIFT                       1
#define I2C_S1_IICIF_WIDTH                       1
#define I2C_S1_IICIF(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_S1_IICIF_SHIFT))&I2C_S1_IICIF_MASK)
#define I2C_S1_SRW_MASK                          0x4u
#define I2C_S1_SRW_SHIFT                         2
#define I2C_S1_SRW_WIDTH                         1
#define I2C_S1_SRW(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_S1_SRW_SHIFT))&I2C_S1_SRW_MASK)
#define I2C_S1_RAM_MASK                          0x8u
#define I2C_S1_RAM_SHIFT                         3
#define I2C_S1_RAM_WIDTH                         1
#define I2C_S1_RAM(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_S1_RAM_SHIFT))&I2C_S1_RAM_MASK)
#define I2C_S1_ARBL_MASK                         0x10u
#define I2C_S1_ARBL_SHIFT                        4
#define I2C_S1_ARBL_WIDTH                        1
#define I2C_S1_ARBL(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_S1_ARBL_SHIFT))&I2C_S1_ARBL_MASK)
#define I2C_S1_BUSY_MASK                         0x20u
#define I2C_S1_BUSY_SHIFT                        5
#define I2C_S1_BUSY_WIDTH                        1
#define I2C_S1_BUSY(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_S1_BUSY_SHIFT))&I2C_S1_BUSY_MASK)
#define I2C_S1_IAAS_MASK                         0x40u
#define I2C_S1_IAAS_SHIFT                        6
#define I2C_S1_IAAS_WIDTH                        1
#define I2C_S1_IAAS(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_S1_IAAS_SHIFT))&I2C_S1_IAAS_MASK)
#define I2C_S1_TCF_MASK                          0x80u
#define I2C_S1_TCF_SHIFT                         7
#define I2C_S1_TCF_WIDTH                         1
#define I2C_S1_TCF(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_S1_TCF_SHIFT))&I2C_S1_TCF_MASK)
/* D Bit Fields */
#define I2C_D_DATA_MASK                          0xFFu
#define I2C_D_DATA_SHIFT                         0
#define I2C_D_DATA_WIDTH                         8
#define I2C_D_DATA(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_D_DATA_SHIFT))&I2C_D_DATA_MASK)
/* C2 Bit Fields */
#define I2C_C2_AD_MASK                           0x7u
#define I2C_C2_AD_SHIFT                          0
#define I2C_C2_AD_WIDTH                          3
#define I2C_C2_AD(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_C2_AD_SHIFT))&I2C_C2_AD_MASK)
#define I2C_C2_RMEN_MASK                         0x8u
#define I2C_C2_RMEN_SHIFT                        3
#define I2C_C2_RMEN_WIDTH                        1
#define I2C_C2_RMEN(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_C2_RMEN_SHIFT))&I2C_C2_RMEN_MASK)
#define I2C_C2_SBRC_MASK                         0x10u
#define I2C_C2_SBRC_SHIFT                        4
#define I2C_C2_SBRC_WIDTH                        1
#define I2C_C2_SBRC(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_C2_SBRC_SHIFT))&I2C_C2_SBRC_MASK)
#define I2C_C2_ADEXT_MASK                        0x40u
#define I2C_C2_ADEXT_SHIFT                       6
#define I2C_C2_ADEXT_WIDTH                       1
#define I2C_C2_ADEXT(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_C2_ADEXT_SHIFT))&I2C_C2_ADEXT_MASK)
#define I2C_C2_GCAEN_MASK                        0x80u
#define I2C_C2_GCAEN_SHIFT                       7
#define I2C_C2_GCAEN_WIDTH                       1
#define I2C_C2_GCAEN(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_C2_GCAEN_SHIFT))&I2C_C2_GCAEN_MASK)
/* FLT Bit Fields */
#define I2C_FLT_FLT_MASK                         0xFu
#define I2C_FLT_FLT_SHIFT                        0
#define I2C_FLT_FLT_WIDTH                        4
#define I2C_FLT_FLT(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_FLT_FLT_SHIFT))&I2C_FLT_FLT_MASK)
#define I2C_FLT_STARTF_MASK                      0x10u
#define I2C_FLT_STARTF_SHIFT                     4
#define I2C_FLT_STARTF_WIDTH                     1
#define I2C_FLT_STARTF(x)                        (((uint8_t)(((uint8_t)(x))<<I2C_FLT_STARTF_SHIFT))&I2C_FLT_STARTF_MASK)
#define I2C_FLT_SSIE_MASK                        0x20u
#define I2C_FLT_SSIE_SHIFT                       5
#define I2C_FLT_SSIE_WIDTH                       1
#define I2C_FLT_SSIE(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_FLT_SSIE_SHIFT))&I2C_FLT_SSIE_MASK)
#define I2C_FLT_STOPF_MASK                       0x40u
#define I2C_FLT_STOPF_SHIFT                      6
#define I2C_FLT_STOPF_WIDTH                      1
#define I2C_FLT_STOPF(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_FLT_STOPF_SHIFT))&I2C_FLT_STOPF_MASK)
#define I2C_FLT_SHEN_MASK                        0x80u
#define I2C_FLT_SHEN_SHIFT                       7
#define I2C_FLT_SHEN_WIDTH                       1
#define I2C_FLT_SHEN(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_FLT_SHEN_SHIFT))&I2C_FLT_SHEN_MASK)
/* RA Bit Fields */
#define I2C_RA_RAD_MASK                          0xFEu
#define I2C_RA_RAD_SHIFT                         1
#define I2C_RA_RAD_WIDTH                         7
#define I2C_RA_RAD(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_RA_RAD_SHIFT))&I2C_RA_RAD_MASK)
/* SMB Bit Fields */
#define I2C_SMB_SHTF2IE_MASK                     0x1u
#define I2C_SMB_SHTF2IE_SHIFT                    0
#define I2C_SMB_SHTF2IE_WIDTH                    1
#define I2C_SMB_SHTF2IE(x)                       (((uint8_t)(((uint8_t)(x))<<I2C_SMB_SHTF2IE_SHIFT))&I2C_SMB_SHTF2IE_MASK)
#define I2C_SMB_SHTF2_MASK                       0x2u
#define I2C_SMB_SHTF2_SHIFT                      1
#define I2C_SMB_SHTF2_WIDTH                      1
#define I2C_SMB_SHTF2(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_SMB_SHTF2_SHIFT))&I2C_SMB_SHTF2_MASK)
#define I2C_SMB_SHTF1_MASK                       0x4u
#define I2C_SMB_SHTF1_SHIFT                      2
#define I2C_SMB_SHTF1_WIDTH                      1
#define I2C_SMB_SHTF1(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_SMB_SHTF1_SHIFT))&I2C_SMB_SHTF1_MASK)
#define I2C_SMB_SLTF_MASK                        0x8u
#define I2C_SMB_SLTF_SHIFT                       3
#define I2C_SMB_SLTF_WIDTH                       1
#define I2C_SMB_SLTF(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_SMB_SLTF_SHIFT))&I2C_SMB_SLTF_MASK)
#define I2C_SMB_TCKSEL_MASK                      0x10u
#define I2C_SMB_TCKSEL_SHIFT                     4
#define I2C_SMB_TCKSEL_WIDTH                     1
#define I2C_SMB_TCKSEL(x)                        (((uint8_t)(((uint8_t)(x))<<I2C_SMB_TCKSEL_SHIFT))&I2C_SMB_TCKSEL_MASK)
#define I2C_SMB_SIICAEN_MASK                     0x20u
#define I2C_SMB_SIICAEN_SHIFT                    5
#define I2C_SMB_SIICAEN_WIDTH                    1
#define I2C_SMB_SIICAEN(x)                       (((uint8_t)(((uint8_t)(x))<<I2C_SMB_SIICAEN_SHIFT))&I2C_SMB_SIICAEN_MASK)
#define I2C_SMB_ALERTEN_MASK                     0x40u
#define I2C_SMB_ALERTEN_SHIFT                    6
#define I2C_SMB_ALERTEN_WIDTH                    1
#define I2C_SMB_ALERTEN(x)                       (((uint8_t)(((uint8_t)(x))<<I2C_SMB_ALERTEN_SHIFT))&I2C_SMB_ALERTEN_MASK)
#define I2C_SMB_FACK_MASK                        0x80u
#define I2C_SMB_FACK_SHIFT                       7
#define I2C_SMB_FACK_WIDTH                       1
#define I2C_SMB_FACK(x)                          (((uint8_t)(((uint8_t)(x))<<I2C_SMB_FACK_SHIFT))&I2C_SMB_FACK_MASK)
/* A2 Bit Fields */
#define I2C_A2_SAD_MASK                          0xFEu
#define I2C_A2_SAD_SHIFT                         1
#define I2C_A2_SAD_WIDTH                         7
#define I2C_A2_SAD(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_A2_SAD_SHIFT))&I2C_A2_SAD_MASK)
/* SLTH Bit Fields */
#define I2C_SLTH_SSLT_MASK                       0xFFu
#define I2C_SLTH_SSLT_SHIFT                      0
#define I2C_SLTH_SSLT_WIDTH                      8
#define I2C_SLTH_SSLT(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_SLTH_SSLT_SHIFT))&I2C_SLTH_SSLT_MASK)
/* SLTL Bit Fields */
#define I2C_SLTL_SSLT_MASK                       0xFFu
#define I2C_SLTL_SSLT_SHIFT                      0
#define I2C_SLTL_SSLT_WIDTH                      8
#define I2C_SLTL_SSLT(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_SLTL_SSLT_SHIFT))&I2C_SLTL_SSLT_MASK)

/*!
 * @}
 */ /* end of group I2C_Register_Masks */


/* I2C - Peripheral instance base addresses */
/** Peripheral I2C0 base address */
#define I2C0_BASE                                (0x40066000u)
/** Peripheral I2C0 base pointer */
#define I2C0                                     ((I2C_Type *)I2C0_BASE)
#define I2C0_BASE_PTR                            (I2C0)
/** Peripheral I2C1 base address */
#define I2C1_BASE                                (0x40067000u)
/** Peripheral I2C1 base pointer */
#define I2C1                                     ((I2C_Type *)I2C1_BASE)
#define I2C1_BASE_PTR                            (I2C1)
/** Array initializer of I2C peripheral base addresses */
#define I2C_BASE_ADDRS                           { I2C0_BASE, I2C1_BASE }
/** Array initializer of I2C peripheral base pointers */
#define I2C_BASE_PTRS                            { I2C0, I2C1 }

/* ----------------------------------------------------------------------------
   -- I2C - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Register_Accessor_Macros I2C - Register accessor macros
 * @{
 */


/* I2C - Register instance definitions */
/* I2C0 */
#define I2C0_A1                                  I2C_A1_REG(I2C0)
#define I2C0_F                                   I2C_F_REG(I2C0)
#define I2C0_C1                                  I2C_C1_REG(I2C0)
#define I2C0_S1                                  I2C_S1_REG(I2C0)
#define I2C0_D                                   I2C_D_REG(I2C0)
#define I2C0_C2                                  I2C_C2_REG(I2C0)
#define I2C0_FLT                                 I2C_FLT_REG(I2C0)
#define I2C0_RA                                  I2C_RA_REG(I2C0)
#define I2C0_SMB                                 I2C_SMB_REG(I2C0)
#define I2C0_A2                                  I2C_A2_REG(I2C0)
#define I2C0_SLTH                                I2C_SLTH_REG(I2C0)
#define I2C0_SLTL                                I2C_SLTL_REG(I2C0)
/* I2C1 */
#define I2C1_A1                                  I2C_A1_REG(I2C1)
#define I2C1_F                                   I2C_F_REG(I2C1)
#define I2C1_C1                                  I2C_C1_REG(I2C1)
#define I2C1_S1                                  I2C_S1_REG(I2C1)
#define I2C1_D                                   I2C_D_REG(I2C1)
#define I2C1_C2                                  I2C_C2_REG(I2C1)
#define I2C1_FLT                                 I2C_FLT_REG(I2C1)
#define I2C1_RA                                  I2C_RA_REG(I2C1)
#define I2C1_SMB                                 I2C_SMB_REG(I2C1)
#define I2C1_A2                                  I2C_A2_REG(I2C1)
#define I2C1_SLTH                                I2C_SLTH_REG(I2C1)
#define I2C1_SLTL                                I2C_SLTL_REG(I2C1)

/*!
 * @}
 */ /* end of group I2C_Register_Accessor_Macros */

/* Backward compatibility */
#define I2C_S_RXAK_MASK         (I2C_S1_RXAK_MASK)
#define I2C_S_RXAK_SHIFT        (I2C_S1_RXAK_SHIFT)
#define I2C_S_RXAK_WIDTH        (I2C_S1_RXAK_WIDTH)
#define I2C_S_RXAK(x)           (I2C_S1_RXAK(x))
#define I2C_S_IICIF_MASK        (I2C_S1_IICIF_MASK)
#define I2C_S_IICIF_SHIFT       (I2C_S1_IICIF_SHIFT)
#define I2C_S_IICIF_WIDTH       (I2C_S1_IICIF_WIDTH)
#define I2C_S_IICIF(x)          (I2C_S1_IICIF(x))
#define I2C_S_SRW_MASK          (I2C_S1_SRW_MASK)
#define I2C_S_SRW_SHIFT         (I2C_S1_SRW_SHIFT)
#define I2C_S_SRW_WIDTH         (I2C_S1_SRW_WIDTH)
#define I2C_S_SRW(x)            (I2C_S1_SRW(x))
#define I2C_S_RAM_MASK          (I2C_S1_RAM_MASK)
#define I2C_S_RAM_SHIFT         (I2C_S1_RAM_SHIFT)
#define I2C_S_RAM_WIDTH         (I2C_S1_RAM_WIDTH)
#define I2C_S_RAM(x)            (I2C_S1_RAM(x))
#define I2C_S_ARBL_MASK         (I2C_S1_ARBL_MASK)
#define I2C_S_ARBL_SHIFT        (I2C_S1_ARBL_SHIFT)
#define I2C_S_ARBL_WIDTH        (I2C_S1_ARBL_WIDTH)
#define I2C_S_ARBL(x)           (I2C_S1_ARBL(x))
#define I2C_S_BUSY_MASK         (I2C_S1_BUSY_MASK)
#define I2C_S_BUSY_SHIFT        (I2C_S1_BUSY_SHIFT)
#define I2C_S_BUSY_WIDTH        (I2C_S1_BUSY_WIDTH)
#define I2C_S_BUSY(x)           (I2C_S1_BUSY(x))
#define I2C_S_IAAS_MASK         (I2C_S1_IAAS_MASK)
#define I2C_S_IAAS_SHIFT        (I2C_S1_IAAS_SHIFT)
#define I2C_S_IAAS_WIDTH        (I2C_S1_IAAS_WIDTH)
#define I2C_S_IAAS(x)           (I2C_S1_IAAS(x))
#define I2C_S_TCF_MASK          (I2C_S1_TCF_MASK)
#define I2C_S_TCF_SHIFT         (I2C_S1_TCF_SHIFT)
#define I2C_S_TCF_WIDTH         (I2C_S1_TCF_WIDTH)
#define I2C_S_TCF(x)            (I2C_S1_TCF(x))


/*!
 * @}
 */ /* end of group I2C_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- ICS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ICS_Peripheral_Access_Layer ICS Peripheral Access Layer
 * @{
 */

/** ICS - Register Layout Typedef */
typedef struct {
  __IO uint8_t C1;                                 /**< ICS Control Register 1, offset: 0x0 */
  __IO uint8_t C2;                                 /**< ICS Control Register 2, offset: 0x1 */
  __IO uint8_t C3;                                 /**< ICS Control Register 3, offset: 0x2 */
  __IO uint8_t C4;                                 /**< ICS Control Register 4, offset: 0x3 */
  __IO uint8_t S;                                  /**< ICS Status Register, offset: 0x4 */
} ICS_Type, *ICS_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- ICS - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ICS_Register_Accessor_Macros ICS - Register accessor macros
 * @{
 */


/* ICS - Register accessors */
#define ICS_C1_REG(base)                         ((base)->C1)
#define ICS_C2_REG(base)                         ((base)->C2)
#define ICS_C3_REG(base)                         ((base)->C3)
#define ICS_C4_REG(base)                         ((base)->C4)
#define ICS_S_REG(base)                          ((base)->S)

/*!
 * @}
 */ /* end of group ICS_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- ICS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ICS_Register_Masks ICS Register Masks
 * @{
 */

/* C1 Bit Fields */
#define ICS_C1_IREFSTEN_MASK                     0x1u
#define ICS_C1_IREFSTEN_SHIFT                    0
#define ICS_C1_IREFSTEN_WIDTH                    1
#define ICS_C1_IREFSTEN(x)                       (((uint8_t)(((uint8_t)(x))<<ICS_C1_IREFSTEN_SHIFT))&ICS_C1_IREFSTEN_MASK)
#define ICS_C1_IRCLKEN_MASK                      0x2u
#define ICS_C1_IRCLKEN_SHIFT                     1
#define ICS_C1_IRCLKEN_WIDTH                     1
#define ICS_C1_IRCLKEN(x)                        (((uint8_t)(((uint8_t)(x))<<ICS_C1_IRCLKEN_SHIFT))&ICS_C1_IRCLKEN_MASK)
#define ICS_C1_IREFS_MASK                        0x4u
#define ICS_C1_IREFS_SHIFT                       2
#define ICS_C1_IREFS_WIDTH                       1
#define ICS_C1_IREFS(x)                          (((uint8_t)(((uint8_t)(x))<<ICS_C1_IREFS_SHIFT))&ICS_C1_IREFS_MASK)
#define ICS_C1_RDIV_MASK                         0x38u
#define ICS_C1_RDIV_SHIFT                        3
#define ICS_C1_RDIV_WIDTH                        3
#define ICS_C1_RDIV(x)                           (((uint8_t)(((uint8_t)(x))<<ICS_C1_RDIV_SHIFT))&ICS_C1_RDIV_MASK)
#define ICS_C1_CLKS_MASK                         0xC0u
#define ICS_C1_CLKS_SHIFT                        6
#define ICS_C1_CLKS_WIDTH                        2
#define ICS_C1_CLKS(x)                           (((uint8_t)(((uint8_t)(x))<<ICS_C1_CLKS_SHIFT))&ICS_C1_CLKS_MASK)
/* C2 Bit Fields */
#define ICS_C2_LP_MASK                           0x10u
#define ICS_C2_LP_SHIFT                          4
#define ICS_C2_LP_WIDTH                          1
#define ICS_C2_LP(x)                             (((uint8_t)(((uint8_t)(x))<<ICS_C2_LP_SHIFT))&ICS_C2_LP_MASK)
#define ICS_C2_BDIV_MASK                         0xE0u
#define ICS_C2_BDIV_SHIFT                        5
#define ICS_C2_BDIV_WIDTH                        3
#define ICS_C2_BDIV(x)                           (((uint8_t)(((uint8_t)(x))<<ICS_C2_BDIV_SHIFT))&ICS_C2_BDIV_MASK)
/* C3 Bit Fields */
#define ICS_C3_SCTRIM_MASK                       0xFFu
#define ICS_C3_SCTRIM_SHIFT                      0
#define ICS_C3_SCTRIM_WIDTH                      8
#define ICS_C3_SCTRIM(x)                         (((uint8_t)(((uint8_t)(x))<<ICS_C3_SCTRIM_SHIFT))&ICS_C3_SCTRIM_MASK)
/* C4 Bit Fields */
#define ICS_C4_SCFTRIM_MASK                      0x1u
#define ICS_C4_SCFTRIM_SHIFT                     0
#define ICS_C4_SCFTRIM_WIDTH                     1
#define ICS_C4_SCFTRIM(x)                        (((uint8_t)(((uint8_t)(x))<<ICS_C4_SCFTRIM_SHIFT))&ICS_C4_SCFTRIM_MASK)
#define ICS_C4_CME_MASK                          0x20u
#define ICS_C4_CME_SHIFT                         5
#define ICS_C4_CME_WIDTH                         1
#define ICS_C4_CME(x)                            (((uint8_t)(((uint8_t)(x))<<ICS_C4_CME_SHIFT))&ICS_C4_CME_MASK)
#define ICS_C4_LOLIE_MASK                        0x80u
#define ICS_C4_LOLIE_SHIFT                       7
#define ICS_C4_LOLIE_WIDTH                       1
#define ICS_C4_LOLIE(x)                          (((uint8_t)(((uint8_t)(x))<<ICS_C4_LOLIE_SHIFT))&ICS_C4_LOLIE_MASK)
/* S Bit Fields */
#define ICS_S_CLKST_MASK                         0xCu
#define ICS_S_CLKST_SHIFT                        2
#define ICS_S_CLKST_WIDTH                        2
#define ICS_S_CLKST(x)                           (((uint8_t)(((uint8_t)(x))<<ICS_S_CLKST_SHIFT))&ICS_S_CLKST_MASK)
#define ICS_S_IREFST_MASK                        0x10u
#define ICS_S_IREFST_SHIFT                       4
#define ICS_S_IREFST_WIDTH                       1
#define ICS_S_IREFST(x)                          (((uint8_t)(((uint8_t)(x))<<ICS_S_IREFST_SHIFT))&ICS_S_IREFST_MASK)
#define ICS_S_LOCK_MASK                          0x40u
#define ICS_S_LOCK_SHIFT                         6
#define ICS_S_LOCK_WIDTH                         1
#define ICS_S_LOCK(x)                            (((uint8_t)(((uint8_t)(x))<<ICS_S_LOCK_SHIFT))&ICS_S_LOCK_MASK)
#define ICS_S_LOLS_MASK                          0x80u
#define ICS_S_LOLS_SHIFT                         7
#define ICS_S_LOLS_WIDTH                         1
#define ICS_S_LOLS(x)                            (((uint8_t)(((uint8_t)(x))<<ICS_S_LOLS_SHIFT))&ICS_S_LOLS_MASK)

/*!
 * @}
 */ /* end of group ICS_Register_Masks */


/* ICS - Peripheral instance base addresses */
/** Peripheral ICS base address */
#define ICS_BASE                                 (0x40064000u)
/** Peripheral ICS base pointer */
#define ICS                                      ((ICS_Type *)ICS_BASE)
#define ICS_BASE_PTR                             (ICS)
/** Array initializer of ICS peripheral base addresses */
#define ICS_BASE_ADDRS                           { ICS_BASE }
/** Array initializer of ICS peripheral base pointers */
#define ICS_BASE_PTRS                            { ICS }

/* ----------------------------------------------------------------------------
   -- ICS - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ICS_Register_Accessor_Macros ICS - Register accessor macros
 * @{
 */


/* ICS - Register instance definitions */
/* ICS */
#define ICS_C1                                   ICS_C1_REG(ICS)
#define ICS_C2                                   ICS_C2_REG(ICS)
#define ICS_C3                                   ICS_C3_REG(ICS)
#define ICS_C4                                   ICS_C4_REG(ICS)
#define ICS_S                                    ICS_S_REG(ICS)

/*!
 * @}
 */ /* end of group ICS_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group ICS_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- IRQ Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IRQ_Peripheral_Access_Layer IRQ Peripheral Access Layer
 * @{
 */

/** IRQ - Register Layout Typedef */
typedef struct {
  __IO uint8_t SC;                                 /**< Interrupt Pin Request Status and Control Register, offset: 0x0 */
} IRQ_Type, *IRQ_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- IRQ - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IRQ_Register_Accessor_Macros IRQ - Register accessor macros
 * @{
 */


/* IRQ - Register accessors */
#define IRQ_SC_REG(base)                         ((base)->SC)

/*!
 * @}
 */ /* end of group IRQ_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- IRQ Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IRQ_Register_Masks IRQ Register Masks
 * @{
 */

/* SC Bit Fields */
#define IRQ_SC_IRQMOD_MASK                       0x1u
#define IRQ_SC_IRQMOD_SHIFT                      0
#define IRQ_SC_IRQMOD_WIDTH                      1
#define IRQ_SC_IRQMOD(x)                         (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQMOD_SHIFT))&IRQ_SC_IRQMOD_MASK)
#define IRQ_SC_IRQIE_MASK                        0x2u
#define IRQ_SC_IRQIE_SHIFT                       1
#define IRQ_SC_IRQIE_WIDTH                       1
#define IRQ_SC_IRQIE(x)                          (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQIE_SHIFT))&IRQ_SC_IRQIE_MASK)
#define IRQ_SC_IRQACK_MASK                       0x4u
#define IRQ_SC_IRQACK_SHIFT                      2
#define IRQ_SC_IRQACK_WIDTH                      1
#define IRQ_SC_IRQACK(x)                         (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQACK_SHIFT))&IRQ_SC_IRQACK_MASK)
#define IRQ_SC_IRQF_MASK                         0x8u
#define IRQ_SC_IRQF_SHIFT                        3
#define IRQ_SC_IRQF_WIDTH                        1
#define IRQ_SC_IRQF(x)                           (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQF_SHIFT))&IRQ_SC_IRQF_MASK)
#define IRQ_SC_IRQPE_MASK                        0x10u
#define IRQ_SC_IRQPE_SHIFT                       4
#define IRQ_SC_IRQPE_WIDTH                       1
#define IRQ_SC_IRQPE(x)                          (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQPE_SHIFT))&IRQ_SC_IRQPE_MASK)
#define IRQ_SC_IRQEDG_MASK                       0x20u
#define IRQ_SC_IRQEDG_SHIFT                      5
#define IRQ_SC_IRQEDG_WIDTH                      1
#define IRQ_SC_IRQEDG(x)                         (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQEDG_SHIFT))&IRQ_SC_IRQEDG_MASK)
#define IRQ_SC_IRQPDD_MASK                       0x40u
#define IRQ_SC_IRQPDD_SHIFT                      6
#define IRQ_SC_IRQPDD_WIDTH                      1
#define IRQ_SC_IRQPDD(x)                         (((uint8_t)(((uint8_t)(x))<<IRQ_SC_IRQPDD_SHIFT))&IRQ_SC_IRQPDD_MASK)

/*!
 * @}
 */ /* end of group IRQ_Register_Masks */


/* IRQ - Peripheral instance base addresses */
/** Peripheral IRQ base address */
#define IRQ_BASE                                 (0x40031000u)
/** Peripheral IRQ base pointer */
#define IRQ                                      ((IRQ_Type *)IRQ_BASE)
#define IRQ_BASE_PTR                             (IRQ)
/** Array initializer of IRQ peripheral base addresses */
#define IRQ_BASE_ADDRS                           { IRQ_BASE }
/** Array initializer of IRQ peripheral base pointers */
#define IRQ_BASE_PTRS                            { IRQ }

/* ----------------------------------------------------------------------------
   -- IRQ - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IRQ_Register_Accessor_Macros IRQ - Register accessor macros
 * @{
 */


/* IRQ - Register instance definitions */
/* IRQ */
#define IRQ_SC                                   IRQ_SC_REG(IRQ)

/*!
 * @}
 */ /* end of group IRQ_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group IRQ_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- KBI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup KBI_Peripheral_Access_Layer KBI Peripheral Access Layer
 * @{
 */

/** KBI - Register Layout Typedef */
typedef struct {
  __IO uint32_t PE;                                /**< KBI Pin Enable Register, offset: 0x0 */
  __IO uint32_t ES;                                /**< KBI Edge Select Register, offset: 0x4 */
  __IO uint32_t SC;                                /**< KBI Status and Control Register, offset: 0x8 */
  __I  uint32_t SP;                                /**< KBI Source Pin Register, offset: 0xC */
} KBI_Type, *KBI_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- KBI - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup KBI_Register_Accessor_Macros KBI - Register accessor macros
 * @{
 */


/* KBI - Register accessors */
#define KBI_PE_REG(base)                         ((base)->PE)
#define KBI_ES_REG(base)                         ((base)->ES)
#define KBI_SC_REG(base)                         ((base)->SC)
#define KBI_SP_REG(base)                         ((base)->SP)

/*!
 * @}
 */ /* end of group KBI_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- KBI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup KBI_Register_Masks KBI Register Masks
 * @{
 */

/* PE Bit Fields */
#define KBI_PE_KBIPE_MASK                        0xFFFFFFFFu
#define KBI_PE_KBIPE_SHIFT                       0
#define KBI_PE_KBIPE_WIDTH                       32
#define KBI_PE_KBIPE(x)                          (((uint32_t)(((uint32_t)(x))<<KBI_PE_KBIPE_SHIFT))&KBI_PE_KBIPE_MASK)
/* ES Bit Fields */
#define KBI_ES_KBEDG_MASK                        0xFFFFFFFFu
#define KBI_ES_KBEDG_SHIFT                       0
#define KBI_ES_KBEDG_WIDTH                       32
#define KBI_ES_KBEDG(x)                          (((uint32_t)(((uint32_t)(x))<<KBI_ES_KBEDG_SHIFT))&KBI_ES_KBEDG_MASK)
/* SC Bit Fields */
#define KBI_SC_KBMOD_MASK                        0x1u
#define KBI_SC_KBMOD_SHIFT                       0
#define KBI_SC_KBMOD_WIDTH                       1
#define KBI_SC_KBMOD(x)                          (((uint32_t)(((uint32_t)(x))<<KBI_SC_KBMOD_SHIFT))&KBI_SC_KBMOD_MASK)
#define KBI_SC_KBIE_MASK                         0x2u
#define KBI_SC_KBIE_SHIFT                        1
#define KBI_SC_KBIE_WIDTH                        1
#define KBI_SC_KBIE(x)                           (((uint32_t)(((uint32_t)(x))<<KBI_SC_KBIE_SHIFT))&KBI_SC_KBIE_MASK)
#define KBI_SC_KBACK_MASK                        0x4u
#define KBI_SC_KBACK_SHIFT                       2
#define KBI_SC_KBACK_WIDTH                       1
#define KBI_SC_KBACK(x)                          (((uint32_t)(((uint32_t)(x))<<KBI_SC_KBACK_SHIFT))&KBI_SC_KBACK_MASK)
#define KBI_SC_KBF_MASK                          0x8u
#define KBI_SC_KBF_SHIFT                         3
#define KBI_SC_KBF_WIDTH                         1
#define KBI_SC_KBF(x)                            (((uint32_t)(((uint32_t)(x))<<KBI_SC_KBF_SHIFT))&KBI_SC_KBF_MASK)
#define KBI_SC_KBSPEN_MASK                       0x10u
#define KBI_SC_KBSPEN_SHIFT                      4
#define KBI_SC_KBSPEN_WIDTH                      1
#define KBI_SC_KBSPEN(x)                         (((uint32_t)(((uint32_t)(x))<<KBI_SC_KBSPEN_SHIFT))&KBI_SC_KBSPEN_MASK)
#define KBI_SC_RSTKBSP_MASK                      0x20u
#define KBI_SC_RSTKBSP_SHIFT                     5
#define KBI_SC_RSTKBSP_WIDTH                     1
#define KBI_SC_RSTKBSP(x)                        (((uint32_t)(((uint32_t)(x))<<KBI_SC_RSTKBSP_SHIFT))&KBI_SC_RSTKBSP_MASK)
/* SP Bit Fields */
#define KBI_SP_SP_MASK                           0xFFFFFFFFu
#define KBI_SP_SP_SHIFT                          0
#define KBI_SP_SP_WIDTH                          32
#define KBI_SP_SP(x)                             (((uint32_t)(((uint32_t)(x))<<KBI_SP_SP_SHIFT))&KBI_SP_SP_MASK)

/*!
 * @}
 */ /* end of group KBI_Register_Masks */


/* KBI - Peripheral instance base addresses */
/** Peripheral KBI0 base address */
#define KBI0_BASE                                (0x40079000u)
/** Peripheral KBI0 base pointer */
#define KBI0                                     ((KBI_Type *)KBI0_BASE)
#define KBI0_BASE_PTR                            (KBI0)
/** Peripheral KBI1 base address */
#define KBI1_BASE                                (0x4007A000u)
/** Peripheral KBI1 base pointer */
#define KBI1                                     ((KBI_Type *)KBI1_BASE)
#define KBI1_BASE_PTR                            (KBI1)
/** Array initializer of KBI peripheral base addresses */
#define KBI_BASE_ADDRS                           { KBI0_BASE, KBI1_BASE }
/** Array initializer of KBI peripheral base pointers */
#define KBI_BASE_PTRS                            { KBI0, KBI1 }

/* ----------------------------------------------------------------------------
   -- KBI - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup KBI_Register_Accessor_Macros KBI - Register accessor macros
 * @{
 */


/* KBI - Register instance definitions */
/* KBI0 */
#define KBI0_PE                                  KBI_PE_REG(KBI0)
#define KBI0_ES                                  KBI_ES_REG(KBI0)
#define KBI0_SC                                  KBI_SC_REG(KBI0)
#define KBI0_SP                                  KBI_SP_REG(KBI0)
/* KBI1 */
#define KBI1_PE                                  KBI_PE_REG(KBI1)
#define KBI1_ES                                  KBI_ES_REG(KBI1)
#define KBI1_SC                                  KBI_SC_REG(KBI1)
#define KBI1_SP                                  KBI_SP_REG(KBI1)

/*!
 * @}
 */ /* end of group KBI_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group KBI_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- MCM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MCM_Peripheral_Access_Layer MCM Peripheral Access Layer
 * @{
 */

/** MCM - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[8];
  __I  uint16_t PLASC;                             /**< Crossbar Switch (AXBS) Slave Configuration, offset: 0x8 */
  __I  uint16_t PLAMC;                             /**< Crossbar Switch (AXBS) Master Configuration, offset: 0xA */
  __IO uint32_t PLACR;                             /**< Platform Control Register, offset: 0xC */
} MCM_Type, *MCM_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- MCM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MCM_Register_Accessor_Macros MCM - Register accessor macros
 * @{
 */


/* MCM - Register accessors */
#define MCM_PLASC_REG(base)                      ((base)->PLASC)
#define MCM_PLAMC_REG(base)                      ((base)->PLAMC)
#define MCM_PLACR_REG(base)                      ((base)->PLACR)

/*!
 * @}
 */ /* end of group MCM_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- MCM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MCM_Register_Masks MCM Register Masks
 * @{
 */

/* PLASC Bit Fields */
#define MCM_PLASC_ASC_MASK                       0xFFu
#define MCM_PLASC_ASC_SHIFT                      0
#define MCM_PLASC_ASC_WIDTH                      8
#define MCM_PLASC_ASC(x)                         (((uint16_t)(((uint16_t)(x))<<MCM_PLASC_ASC_SHIFT))&MCM_PLASC_ASC_MASK)
/* PLAMC Bit Fields */
#define MCM_PLAMC_AMC_MASK                       0xFFu
#define MCM_PLAMC_AMC_SHIFT                      0
#define MCM_PLAMC_AMC_WIDTH                      8
#define MCM_PLAMC_AMC(x)                         (((uint16_t)(((uint16_t)(x))<<MCM_PLAMC_AMC_SHIFT))&MCM_PLAMC_AMC_MASK)
/* PLACR Bit Fields */
#define MCM_PLACR_CFCC_MASK                      0x400u
#define MCM_PLACR_CFCC_SHIFT                     10
#define MCM_PLACR_CFCC_WIDTH                     1
#define MCM_PLACR_CFCC(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_CFCC_SHIFT))&MCM_PLACR_CFCC_MASK)
#define MCM_PLACR_DFCDA_MASK                     0x800u
#define MCM_PLACR_DFCDA_SHIFT                    11
#define MCM_PLACR_DFCDA_WIDTH                    1
#define MCM_PLACR_DFCDA(x)                       (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_DFCDA_SHIFT))&MCM_PLACR_DFCDA_MASK)
#define MCM_PLACR_DFCIC_MASK                     0x1000u
#define MCM_PLACR_DFCIC_SHIFT                    12
#define MCM_PLACR_DFCIC_WIDTH                    1
#define MCM_PLACR_DFCIC(x)                       (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_DFCIC_SHIFT))&MCM_PLACR_DFCIC_MASK)
#define MCM_PLACR_DFCC_MASK                      0x2000u
#define MCM_PLACR_DFCC_SHIFT                     13
#define MCM_PLACR_DFCC_WIDTH                     1
#define MCM_PLACR_DFCC(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_DFCC_SHIFT))&MCM_PLACR_DFCC_MASK)
#define MCM_PLACR_EFDS_MASK                      0x4000u
#define MCM_PLACR_EFDS_SHIFT                     14
#define MCM_PLACR_EFDS_WIDTH                     1
#define MCM_PLACR_EFDS(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_EFDS_SHIFT))&MCM_PLACR_EFDS_MASK)
#define MCM_PLACR_DFCS_MASK                      0x8000u
#define MCM_PLACR_DFCS_SHIFT                     15
#define MCM_PLACR_DFCS_WIDTH                     1
#define MCM_PLACR_DFCS(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_DFCS_SHIFT))&MCM_PLACR_DFCS_MASK)
#define MCM_PLACR_ESFC_MASK                      0x10000u
#define MCM_PLACR_ESFC_SHIFT                     16
#define MCM_PLACR_ESFC_WIDTH                     1
#define MCM_PLACR_ESFC(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_PLACR_ESFC_SHIFT))&MCM_PLACR_ESFC_MASK)

/*!
 * @}
 */ /* end of group MCM_Register_Masks */


/* MCM - Peripheral instance base addresses */
/** Peripheral MCM base address */
#define MCM_BASE                                 (0xF0003000u)
/** Peripheral MCM base pointer */
#define MCM                                      ((MCM_Type *)MCM_BASE)
#define MCM_BASE_PTR                             (MCM)
/** Array initializer of MCM peripheral base addresses */
#define MCM_BASE_ADDRS                           { MCM_BASE }
/** Array initializer of MCM peripheral base pointers */
#define MCM_BASE_PTRS                            { MCM }

/* ----------------------------------------------------------------------------
   -- MCM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MCM_Register_Accessor_Macros MCM - Register accessor macros
 * @{
 */


/* MCM - Register instance definitions */
/* MCM */
#define MCM_PLASC                                MCM_PLASC_REG(MCM)
#define MCM_PLAMC                                MCM_PLAMC_REG(MCM)
#define MCM_PLACR                                MCM_PLACR_REG(MCM)

/*!
 * @}
 */ /* end of group MCM_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group MCM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- MSCAN Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MSCAN_Peripheral_Access_Layer MSCAN Peripheral Access Layer
 * @{
 */

/** MSCAN - Register Layout Typedef */
typedef struct {
  __IO uint8_t CANCTL0;                            /**< MSCAN Control Register 0, offset: 0x0 */
  __IO uint8_t CANCTL1;                            /**< MSCAN Control Register 1, offset: 0x1 */
  __IO uint8_t CANBTR0;                            /**< MSCAN Bus Timing Register 0, offset: 0x2 */
  __IO uint8_t CANBTR1;                            /**< MSCAN Bus Timing Register 1, offset: 0x3 */
  __IO uint8_t CANRFLG;                            /**< MSCAN Receiver Flag Register, offset: 0x4 */
  __IO uint8_t CANRIER;                            /**< MSCAN Receiver Interrupt Enable Register, offset: 0x5 */
  __IO uint8_t CANTFLG;                            /**< MSCAN Transmitter Flag Register, offset: 0x6 */
  __IO uint8_t CANTIER;                            /**< MSCAN Transmitter Interrupt Enable Register, offset: 0x7 */
  __IO uint8_t CANTARQ;                            /**< MSCAN Transmitter Message Abort Request Register, offset: 0x8 */
  __I  uint8_t CANTAAK;                            /**< MSCAN Transmitter Message Abort Acknowledge Register, offset: 0x9 */
  __IO uint8_t CANTBSEL;                           /**< MSCAN Transmit Buffer Selection Register, offset: 0xA */
  __IO uint8_t CANIDAC;                            /**< MSCAN Identifier Acceptance Control Register, offset: 0xB */
       uint8_t RESERVED_0[1];
  __IO uint8_t CANMISC;                            /**< MSCAN Miscellaneous Register, offset: 0xD */
  __I  uint8_t CANRXERR;                           /**< MSCAN Receive Error Counter, offset: 0xE */
  __I  uint8_t CANTXERR;                           /**< MSCAN Transmit Error Counter, offset: 0xF */
  __IO uint8_t CANIDAR_BANK_1[4];                  /**< MSCAN Identifier Acceptance Register n of First Bank, array offset: 0x10, array step: 0x1 */
  __IO uint8_t CANIDMR_BANK_1[4];                  /**< MSCAN Identifier Mask Register n of First Bank, array offset: 0x14, array step: 0x1 */
  __IO uint8_t CANIDAR_BANK_2[4];                  /**< MSCAN Identifier Acceptance Register n of Second Bank, array offset: 0x18, array step: 0x1 */
  __IO uint8_t CANIDMR_BANK_2[4];                  /**< MSCAN Identifier Mask Register n of Second Bank, array offset: 0x1C, array step: 0x1 */
  union {                                          /* offset: 0x20 */
    __IO uint8_t REIDR0;                             /**< Receive Extended Identifier Register 0, offset: 0x20 */
    __IO uint8_t RSIDR0;                             /**< Receive Standard Identifier Register 0, offset: 0x20 */
  };
  union {                                          /* offset: 0x21 */
    __IO uint8_t REIDR1;                             /**< Receive Extended Identifier Register 1, offset: 0x21 */
    __IO uint8_t RSIDR1;                             /**< Standard Identifier Register 1, offset: 0x21 */
  };
  __IO uint8_t REIDR2;                             /**< Receive Extended Identifier Register 2, offset: 0x22 */
  __IO uint8_t REIDR3;                             /**< Receive Extended Identifier Register 3, offset: 0x23 */
  __IO uint8_t REDSR[8];                           /**< Receive Extended Data Segment Register N, array offset: 0x24, array step: 0x1 */
  __IO uint8_t RDLR;                               /**< Receive Data Length Register, offset: 0x2C */
       uint8_t RESERVED_1[1];
  __IO uint8_t RTSRH;                              /**< Receive Time Stamp Register High, offset: 0x2E */
  __IO uint8_t RTSRL;                              /**< Receive Time Stamp Register Low, offset: 0x2F */
  union {                                          /* offset: 0x30 */
    __IO uint8_t TEIDR0;                             /**< Transmit Extended Identifier Register 0, offset: 0x30 */
    __IO uint8_t TSIDR0;                             /**< Transmit Standard Identifier Register 0, offset: 0x30 */
  };
  union {                                          /* offset: 0x31 */
    __IO uint8_t TEIDR1;                             /**< Transmit Extended Identifier Register 1, offset: 0x31 */
    __IO uint8_t TSIDR1;                             /**< Transmit Standard Identifier Register 1, offset: 0x31 */
  };
  __IO uint8_t TEIDR2;                             /**< Transmit Extended Identifier Register 2, offset: 0x32 */
  __IO uint8_t TEIDR3;                             /**< Transmit Extended Identifier Register 3, offset: 0x33 */
  __IO uint8_t TEDSR[8];                           /**< Transmit Extended Data Segment Register N, array offset: 0x34, array step: 0x1 */
  __IO uint8_t TDLR;                               /**< This register keeps the data length field of the CAN frame., offset: 0x3C */
  __IO uint8_t TBPR;                               /**< Transmit Buffer Priority Register, offset: 0x3D */
  __IO uint8_t TTSRH;                              /**< Transmit Time Stamp Register High, offset: 0x3E */
  __IO uint8_t TTSRL;                              /**< Transmit Time Stamp Register Low, offset: 0x3F */
} MSCAN_Type, *MSCAN_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- MSCAN - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MSCAN_Register_Accessor_Macros MSCAN - Register accessor macros
 * @{
 */


/* MSCAN - Register accessors */
#define MSCAN_CANCTL0_REG(base)                  ((base)->CANCTL0)
#define MSCAN_CANCTL1_REG(base)                  ((base)->CANCTL1)
#define MSCAN_CANBTR0_REG(base)                  ((base)->CANBTR0)
#define MSCAN_CANBTR1_REG(base)                  ((base)->CANBTR1)
#define MSCAN_CANRFLG_REG(base)                  ((base)->CANRFLG)
#define MSCAN_CANRIER_REG(base)                  ((base)->CANRIER)
#define MSCAN_CANTFLG_REG(base)                  ((base)->CANTFLG)
#define MSCAN_CANTIER_REG(base)                  ((base)->CANTIER)
#define MSCAN_CANTARQ_REG(base)                  ((base)->CANTARQ)
#define MSCAN_CANTAAK_REG(base)                  ((base)->CANTAAK)
#define MSCAN_CANTBSEL_REG(base)                 ((base)->CANTBSEL)
#define MSCAN_CANIDAC_REG(base)                  ((base)->CANIDAC)
#define MSCAN_CANMISC_REG(base)                  ((base)->CANMISC)
#define MSCAN_CANRXERR_REG(base)                 ((base)->CANRXERR)
#define MSCAN_CANTXERR_REG(base)                 ((base)->CANTXERR)
#define MSCAN_CANIDAR_BANK_1_REG(base,index)     ((base)->CANIDAR_BANK_1[index])
#define MSCAN_CANIDAR_BANK_1_COUNT               4
#define MSCAN_CANIDMR_BANK_1_REG(base,index)     ((base)->CANIDMR_BANK_1[index])
#define MSCAN_CANIDMR_BANK_1_COUNT               4
#define MSCAN_CANIDAR_BANK_2_REG(base,index)     ((base)->CANIDAR_BANK_2[index])
#define MSCAN_CANIDAR_BANK_2_COUNT               4
#define MSCAN_CANIDMR_BANK_2_REG(base,index)     ((base)->CANIDMR_BANK_2[index])
#define MSCAN_CANIDMR_BANK_2_COUNT               4
#define MSCAN_REIDR0_REG(base)                   ((base)->REIDR0)
#define MSCAN_RSIDR0_REG(base)                   ((base)->RSIDR0)
#define MSCAN_REIDR1_REG(base)                   ((base)->REIDR1)
#define MSCAN_RSIDR1_REG(base)                   ((base)->RSIDR1)
#define MSCAN_REIDR2_REG(base)                   ((base)->REIDR2)
#define MSCAN_REIDR3_REG(base)                   ((base)->REIDR3)
#define MSCAN_REDSR_REG(base,index)              ((base)->REDSR[index])
#define MSCAN_REDSR_COUNT                        8
#define MSCAN_RDLR_REG(base)                     ((base)->RDLR)
#define MSCAN_RTSRH_REG(base)                    ((base)->RTSRH)
#define MSCAN_RTSRL_REG(base)                    ((base)->RTSRL)
#define MSCAN_TEIDR0_REG(base)                   ((base)->TEIDR0)
#define MSCAN_TSIDR0_REG(base)                   ((base)->TSIDR0)
#define MSCAN_TEIDR1_REG(base)                   ((base)->TEIDR1)
#define MSCAN_TSIDR1_REG(base)                   ((base)->TSIDR1)
#define MSCAN_TEIDR2_REG(base)                   ((base)->TEIDR2)
#define MSCAN_TEIDR3_REG(base)                   ((base)->TEIDR3)
#define MSCAN_TEDSR_REG(base,index)              ((base)->TEDSR[index])
#define MSCAN_TEDSR_COUNT                        8
#define MSCAN_TDLR_REG(base)                     ((base)->TDLR)
#define MSCAN_TBPR_REG(base)                     ((base)->TBPR)
#define MSCAN_TTSRH_REG(base)                    ((base)->TTSRH)
#define MSCAN_TTSRL_REG(base)                    ((base)->TTSRL)

/*!
 * @}
 */ /* end of group MSCAN_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- MSCAN Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MSCAN_Register_Masks MSCAN Register Masks
 * @{
 */

/* CANCTL0 Bit Fields */
#define MSCAN_CANCTL0_INITRQ_MASK                0x1u
#define MSCAN_CANCTL0_INITRQ_SHIFT               0
#define MSCAN_CANCTL0_INITRQ_WIDTH               1
#define MSCAN_CANCTL0_INITRQ(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_INITRQ_SHIFT))&MSCAN_CANCTL0_INITRQ_MASK)
#define MSCAN_CANCTL0_SLPRQ_MASK                 0x2u
#define MSCAN_CANCTL0_SLPRQ_SHIFT                1
#define MSCAN_CANCTL0_SLPRQ_WIDTH                1
#define MSCAN_CANCTL0_SLPRQ(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_SLPRQ_SHIFT))&MSCAN_CANCTL0_SLPRQ_MASK)
#define MSCAN_CANCTL0_WUPE_MASK                  0x4u
#define MSCAN_CANCTL0_WUPE_SHIFT                 2
#define MSCAN_CANCTL0_WUPE_WIDTH                 1
#define MSCAN_CANCTL0_WUPE(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_WUPE_SHIFT))&MSCAN_CANCTL0_WUPE_MASK)
#define MSCAN_CANCTL0_TIME_MASK                  0x8u
#define MSCAN_CANCTL0_TIME_SHIFT                 3
#define MSCAN_CANCTL0_TIME_WIDTH                 1
#define MSCAN_CANCTL0_TIME(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_TIME_SHIFT))&MSCAN_CANCTL0_TIME_MASK)
#define MSCAN_CANCTL0_SYNCH_MASK                 0x10u
#define MSCAN_CANCTL0_SYNCH_SHIFT                4
#define MSCAN_CANCTL0_SYNCH_WIDTH                1
#define MSCAN_CANCTL0_SYNCH(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_SYNCH_SHIFT))&MSCAN_CANCTL0_SYNCH_MASK)
#define MSCAN_CANCTL0_CSWAI_MASK                 0x20u
#define MSCAN_CANCTL0_CSWAI_SHIFT                5
#define MSCAN_CANCTL0_CSWAI_WIDTH                1
#define MSCAN_CANCTL0_CSWAI(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_CSWAI_SHIFT))&MSCAN_CANCTL0_CSWAI_MASK)
#define MSCAN_CANCTL0_RXACT_MASK                 0x40u
#define MSCAN_CANCTL0_RXACT_SHIFT                6
#define MSCAN_CANCTL0_RXACT_WIDTH                1
#define MSCAN_CANCTL0_RXACT(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_RXACT_SHIFT))&MSCAN_CANCTL0_RXACT_MASK)
#define MSCAN_CANCTL0_RXFRM_MASK                 0x80u
#define MSCAN_CANCTL0_RXFRM_SHIFT                7
#define MSCAN_CANCTL0_RXFRM_WIDTH                1
#define MSCAN_CANCTL0_RXFRM(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL0_RXFRM_SHIFT))&MSCAN_CANCTL0_RXFRM_MASK)
/* CANCTL1 Bit Fields */
#define MSCAN_CANCTL1_INITAK_MASK                0x1u
#define MSCAN_CANCTL1_INITAK_SHIFT               0
#define MSCAN_CANCTL1_INITAK_WIDTH               1
#define MSCAN_CANCTL1_INITAK(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_INITAK_SHIFT))&MSCAN_CANCTL1_INITAK_MASK)
#define MSCAN_CANCTL1_SLPAK_MASK                 0x2u
#define MSCAN_CANCTL1_SLPAK_SHIFT                1
#define MSCAN_CANCTL1_SLPAK_WIDTH                1
#define MSCAN_CANCTL1_SLPAK(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_SLPAK_SHIFT))&MSCAN_CANCTL1_SLPAK_MASK)
#define MSCAN_CANCTL1_WUPM_MASK                  0x4u
#define MSCAN_CANCTL1_WUPM_SHIFT                 2
#define MSCAN_CANCTL1_WUPM_WIDTH                 1
#define MSCAN_CANCTL1_WUPM(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_WUPM_SHIFT))&MSCAN_CANCTL1_WUPM_MASK)
#define MSCAN_CANCTL1_BORM_MASK                  0x8u
#define MSCAN_CANCTL1_BORM_SHIFT                 3
#define MSCAN_CANCTL1_BORM_WIDTH                 1
#define MSCAN_CANCTL1_BORM(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_BORM_SHIFT))&MSCAN_CANCTL1_BORM_MASK)
#define MSCAN_CANCTL1_LISTEN_MASK                0x10u
#define MSCAN_CANCTL1_LISTEN_SHIFT               4
#define MSCAN_CANCTL1_LISTEN_WIDTH               1
#define MSCAN_CANCTL1_LISTEN(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_LISTEN_SHIFT))&MSCAN_CANCTL1_LISTEN_MASK)
#define MSCAN_CANCTL1_LOOPB_MASK                 0x20u
#define MSCAN_CANCTL1_LOOPB_SHIFT                5
#define MSCAN_CANCTL1_LOOPB_WIDTH                1
#define MSCAN_CANCTL1_LOOPB(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_LOOPB_SHIFT))&MSCAN_CANCTL1_LOOPB_MASK)
#define MSCAN_CANCTL1_CLKSRC_MASK                0x40u
#define MSCAN_CANCTL1_CLKSRC_SHIFT               6
#define MSCAN_CANCTL1_CLKSRC_WIDTH               1
#define MSCAN_CANCTL1_CLKSRC(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_CLKSRC_SHIFT))&MSCAN_CANCTL1_CLKSRC_MASK)
#define MSCAN_CANCTL1_CANE_MASK                  0x80u
#define MSCAN_CANCTL1_CANE_SHIFT                 7
#define MSCAN_CANCTL1_CANE_WIDTH                 1
#define MSCAN_CANCTL1_CANE(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANCTL1_CANE_SHIFT))&MSCAN_CANCTL1_CANE_MASK)
/* CANBTR0 Bit Fields */
#define MSCAN_CANBTR0_BRP_MASK                   0x3Fu
#define MSCAN_CANBTR0_BRP_SHIFT                  0
#define MSCAN_CANBTR0_BRP_WIDTH                  6
#define MSCAN_CANBTR0_BRP(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_CANBTR0_BRP_SHIFT))&MSCAN_CANBTR0_BRP_MASK)
#define MSCAN_CANBTR0_SJW_MASK                   0xC0u
#define MSCAN_CANBTR0_SJW_SHIFT                  6
#define MSCAN_CANBTR0_SJW_WIDTH                  2
#define MSCAN_CANBTR0_SJW(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_CANBTR0_SJW_SHIFT))&MSCAN_CANBTR0_SJW_MASK)
/* CANBTR1 Bit Fields */
#define MSCAN_CANBTR1_TSEG1_MASK                 0xFu
#define MSCAN_CANBTR1_TSEG1_SHIFT                0
#define MSCAN_CANBTR1_TSEG1_WIDTH                4
#define MSCAN_CANBTR1_TSEG1(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANBTR1_TSEG1_SHIFT))&MSCAN_CANBTR1_TSEG1_MASK)
#define MSCAN_CANBTR1_TSEG2_MASK                 0x70u
#define MSCAN_CANBTR1_TSEG2_SHIFT                4
#define MSCAN_CANBTR1_TSEG2_WIDTH                3
#define MSCAN_CANBTR1_TSEG2(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANBTR1_TSEG2_SHIFT))&MSCAN_CANBTR1_TSEG2_MASK)
#define MSCAN_CANBTR1_SAMP_MASK                  0x80u
#define MSCAN_CANBTR1_SAMP_SHIFT                 7
#define MSCAN_CANBTR1_SAMP_WIDTH                 1
#define MSCAN_CANBTR1_SAMP(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANBTR1_SAMP_SHIFT))&MSCAN_CANBTR1_SAMP_MASK)
/* CANRFLG Bit Fields */
#define MSCAN_CANRFLG_RXF_MASK                   0x1u
#define MSCAN_CANRFLG_RXF_SHIFT                  0
#define MSCAN_CANRFLG_RXF_WIDTH                  1
#define MSCAN_CANRFLG_RXF(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRFLG_RXF_SHIFT))&MSCAN_CANRFLG_RXF_MASK)
#define MSCAN_CANRFLG_OVRIF_MASK                 0x2u
#define MSCAN_CANRFLG_OVRIF_SHIFT                1
#define MSCAN_CANRFLG_OVRIF_WIDTH                1
#define MSCAN_CANRFLG_OVRIF(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRFLG_OVRIF_SHIFT))&MSCAN_CANRFLG_OVRIF_MASK)
#define MSCAN_CANRFLG_TSTAT_MASK                 0xCu
#define MSCAN_CANRFLG_TSTAT_SHIFT                2
#define MSCAN_CANRFLG_TSTAT_WIDTH                2
#define MSCAN_CANRFLG_TSTAT(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRFLG_TSTAT_SHIFT))&MSCAN_CANRFLG_TSTAT_MASK)
#define MSCAN_CANRFLG_RSTAT_MASK                 0x30u
#define MSCAN_CANRFLG_RSTAT_SHIFT                4
#define MSCAN_CANRFLG_RSTAT_WIDTH                2
#define MSCAN_CANRFLG_RSTAT(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRFLG_RSTAT_SHIFT))&MSCAN_CANRFLG_RSTAT_MASK)
#define MSCAN_CANRFLG_CSCIF_MASK                 0x40u
#define MSCAN_CANRFLG_CSCIF_SHIFT                6
#define MSCAN_CANRFLG_CSCIF_WIDTH                1
#define MSCAN_CANRFLG_CSCIF(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRFLG_CSCIF_SHIFT))&MSCAN_CANRFLG_CSCIF_MASK)
#define MSCAN_CANRFLG_WUPIF_MASK                 0x80u
#define MSCAN_CANRFLG_WUPIF_SHIFT                7
#define MSCAN_CANRFLG_WUPIF_WIDTH                1
#define MSCAN_CANRFLG_WUPIF(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRFLG_WUPIF_SHIFT))&MSCAN_CANRFLG_WUPIF_MASK)
/* CANRIER Bit Fields */
#define MSCAN_CANRIER_RXFIE_MASK                 0x1u
#define MSCAN_CANRIER_RXFIE_SHIFT                0
#define MSCAN_CANRIER_RXFIE_WIDTH                1
#define MSCAN_CANRIER_RXFIE(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRIER_RXFIE_SHIFT))&MSCAN_CANRIER_RXFIE_MASK)
#define MSCAN_CANRIER_OVRIE_MASK                 0x2u
#define MSCAN_CANRIER_OVRIE_SHIFT                1
#define MSCAN_CANRIER_OVRIE_WIDTH                1
#define MSCAN_CANRIER_OVRIE(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRIER_OVRIE_SHIFT))&MSCAN_CANRIER_OVRIE_MASK)
#define MSCAN_CANRIER_TSTATE_MASK                0xCu
#define MSCAN_CANRIER_TSTATE_SHIFT               2
#define MSCAN_CANRIER_TSTATE_WIDTH               2
#define MSCAN_CANRIER_TSTATE(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRIER_TSTATE_SHIFT))&MSCAN_CANRIER_TSTATE_MASK)
#define MSCAN_CANRIER_RSTATE_MASK                0x30u
#define MSCAN_CANRIER_RSTATE_SHIFT               4
#define MSCAN_CANRIER_RSTATE_WIDTH               2
#define MSCAN_CANRIER_RSTATE(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRIER_RSTATE_SHIFT))&MSCAN_CANRIER_RSTATE_MASK)
#define MSCAN_CANRIER_CSCIE_MASK                 0x40u
#define MSCAN_CANRIER_CSCIE_SHIFT                6
#define MSCAN_CANRIER_CSCIE_WIDTH                1
#define MSCAN_CANRIER_CSCIE(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRIER_CSCIE_SHIFT))&MSCAN_CANRIER_CSCIE_MASK)
#define MSCAN_CANRIER_WUPIE_MASK                 0x80u
#define MSCAN_CANRIER_WUPIE_SHIFT                7
#define MSCAN_CANRIER_WUPIE_WIDTH                1
#define MSCAN_CANRIER_WUPIE(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRIER_WUPIE_SHIFT))&MSCAN_CANRIER_WUPIE_MASK)
/* CANTFLG Bit Fields */
#define MSCAN_CANTFLG_TXE_MASK                   0x7u
#define MSCAN_CANTFLG_TXE_SHIFT                  0
#define MSCAN_CANTFLG_TXE_WIDTH                  3
#define MSCAN_CANTFLG_TXE(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_CANTFLG_TXE_SHIFT))&MSCAN_CANTFLG_TXE_MASK)
/* CANTIER Bit Fields */
#define MSCAN_CANTIER_TXEIE_MASK                 0x7u
#define MSCAN_CANTIER_TXEIE_SHIFT                0
#define MSCAN_CANTIER_TXEIE_WIDTH                3
#define MSCAN_CANTIER_TXEIE(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANTIER_TXEIE_SHIFT))&MSCAN_CANTIER_TXEIE_MASK)
/* CANTARQ Bit Fields */
#define MSCAN_CANTARQ_ABTRQ_MASK                 0x7u
#define MSCAN_CANTARQ_ABTRQ_SHIFT                0
#define MSCAN_CANTARQ_ABTRQ_WIDTH                3
#define MSCAN_CANTARQ_ABTRQ(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANTARQ_ABTRQ_SHIFT))&MSCAN_CANTARQ_ABTRQ_MASK)
/* CANTAAK Bit Fields */
#define MSCAN_CANTAAK_ABTAK_MASK                 0x7u
#define MSCAN_CANTAAK_ABTAK_SHIFT                0
#define MSCAN_CANTAAK_ABTAK_WIDTH                3
#define MSCAN_CANTAAK_ABTAK(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANTAAK_ABTAK_SHIFT))&MSCAN_CANTAAK_ABTAK_MASK)
/* CANTBSEL Bit Fields */
#define MSCAN_CANTBSEL_TX_MASK                   0x7u
#define MSCAN_CANTBSEL_TX_SHIFT                  0
#define MSCAN_CANTBSEL_TX_WIDTH                  3
#define MSCAN_CANTBSEL_TX(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_CANTBSEL_TX_SHIFT))&MSCAN_CANTBSEL_TX_MASK)
/* CANIDAC Bit Fields */
#define MSCAN_CANIDAC_IDHIT_MASK                 0x7u
#define MSCAN_CANIDAC_IDHIT_SHIFT                0
#define MSCAN_CANIDAC_IDHIT_WIDTH                3
#define MSCAN_CANIDAC_IDHIT(x)                   (((uint8_t)(((uint8_t)(x))<<MSCAN_CANIDAC_IDHIT_SHIFT))&MSCAN_CANIDAC_IDHIT_MASK)
#define MSCAN_CANIDAC_IDAM_MASK                  0x30u
#define MSCAN_CANIDAC_IDAM_SHIFT                 4
#define MSCAN_CANIDAC_IDAM_WIDTH                 2
#define MSCAN_CANIDAC_IDAM(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_CANIDAC_IDAM_SHIFT))&MSCAN_CANIDAC_IDAM_MASK)
/* CANMISC Bit Fields */
#define MSCAN_CANMISC_BOHOLD_MASK                0x1u
#define MSCAN_CANMISC_BOHOLD_SHIFT               0
#define MSCAN_CANMISC_BOHOLD_WIDTH               1
#define MSCAN_CANMISC_BOHOLD(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANMISC_BOHOLD_SHIFT))&MSCAN_CANMISC_BOHOLD_MASK)
/* CANRXERR Bit Fields */
#define MSCAN_CANRXERR_RXERR_MASK                0xFFu
#define MSCAN_CANRXERR_RXERR_SHIFT               0
#define MSCAN_CANRXERR_RXERR_WIDTH               8
#define MSCAN_CANRXERR_RXERR(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANRXERR_RXERR_SHIFT))&MSCAN_CANRXERR_RXERR_MASK)
/* CANTXERR Bit Fields */
#define MSCAN_CANTXERR_TXERR_MASK                0xFFu
#define MSCAN_CANTXERR_TXERR_SHIFT               0
#define MSCAN_CANTXERR_TXERR_WIDTH               8
#define MSCAN_CANTXERR_TXERR(x)                  (((uint8_t)(((uint8_t)(x))<<MSCAN_CANTXERR_TXERR_SHIFT))&MSCAN_CANTXERR_TXERR_MASK)
/* CANIDAR_BANK_1 Bit Fields */
#define MSCAN_CANIDAR_BANK_1_AC_MASK             0xFFu
#define MSCAN_CANIDAR_BANK_1_AC_SHIFT            0
#define MSCAN_CANIDAR_BANK_1_AC_WIDTH            8
#define MSCAN_CANIDAR_BANK_1_AC(x)               (((uint8_t)(((uint8_t)(x))<<MSCAN_CANIDAR_BANK_1_AC_SHIFT))&MSCAN_CANIDAR_BANK_1_AC_MASK)
/* CANIDMR_BANK_1 Bit Fields */
#define MSCAN_CANIDMR_BANK_1_AM_MASK             0xFFu
#define MSCAN_CANIDMR_BANK_1_AM_SHIFT            0
#define MSCAN_CANIDMR_BANK_1_AM_WIDTH            8
#define MSCAN_CANIDMR_BANK_1_AM(x)               (((uint8_t)(((uint8_t)(x))<<MSCAN_CANIDMR_BANK_1_AM_SHIFT))&MSCAN_CANIDMR_BANK_1_AM_MASK)
/* CANIDAR_BANK_2 Bit Fields */
#define MSCAN_CANIDAR_BANK_2_AC_MASK             0xFFu
#define MSCAN_CANIDAR_BANK_2_AC_SHIFT            0
#define MSCAN_CANIDAR_BANK_2_AC_WIDTH            8
#define MSCAN_CANIDAR_BANK_2_AC(x)               (((uint8_t)(((uint8_t)(x))<<MSCAN_CANIDAR_BANK_2_AC_SHIFT))&MSCAN_CANIDAR_BANK_2_AC_MASK)
/* CANIDMR_BANK_2 Bit Fields */
#define MSCAN_CANIDMR_BANK_2_AM_MASK             0xFFu
#define MSCAN_CANIDMR_BANK_2_AM_SHIFT            0
#define MSCAN_CANIDMR_BANK_2_AM_WIDTH            8
#define MSCAN_CANIDMR_BANK_2_AM(x)               (((uint8_t)(((uint8_t)(x))<<MSCAN_CANIDMR_BANK_2_AM_SHIFT))&MSCAN_CANIDMR_BANK_2_AM_MASK)
/* REIDR0 Bit Fields */
#define MSCAN_REIDR0_REID28_REID21_MASK          0xFFu
#define MSCAN_REIDR0_REID28_REID21_SHIFT         0
#define MSCAN_REIDR0_REID28_REID21_WIDTH         8
#define MSCAN_REIDR0_REID28_REID21(x)            (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR0_REID28_REID21_SHIFT))&MSCAN_REIDR0_REID28_REID21_MASK)
/* RSIDR0 Bit Fields */
#define MSCAN_RSIDR0_RSID10_RSID3_MASK           0xFFu
#define MSCAN_RSIDR0_RSID10_RSID3_SHIFT          0
#define MSCAN_RSIDR0_RSID10_RSID3_WIDTH          8
#define MSCAN_RSIDR0_RSID10_RSID3(x)             (((uint8_t)(((uint8_t)(x))<<MSCAN_RSIDR0_RSID10_RSID3_SHIFT))&MSCAN_RSIDR0_RSID10_RSID3_MASK)
/* REIDR1 Bit Fields */
#define MSCAN_REIDR1_REID17_REID15_MASK          0x7u
#define MSCAN_REIDR1_REID17_REID15_SHIFT         0
#define MSCAN_REIDR1_REID17_REID15_WIDTH         3
#define MSCAN_REIDR1_REID17_REID15(x)            (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR1_REID17_REID15_SHIFT))&MSCAN_REIDR1_REID17_REID15_MASK)
#define MSCAN_REIDR1_REIDE_MASK                  0x8u
#define MSCAN_REIDR1_REIDE_SHIFT                 3
#define MSCAN_REIDR1_REIDE_WIDTH                 1
#define MSCAN_REIDR1_REIDE(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR1_REIDE_SHIFT))&MSCAN_REIDR1_REIDE_MASK)
#define MSCAN_REIDR1_RSRR_MASK                   0x10u
#define MSCAN_REIDR1_RSRR_SHIFT                  4
#define MSCAN_REIDR1_RSRR_WIDTH                  1
#define MSCAN_REIDR1_RSRR(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR1_RSRR_SHIFT))&MSCAN_REIDR1_RSRR_MASK)
#define MSCAN_REIDR1_REID20_REID18_MASK          0xE0u
#define MSCAN_REIDR1_REID20_REID18_SHIFT         5
#define MSCAN_REIDR1_REID20_REID18_WIDTH         3
#define MSCAN_REIDR1_REID20_REID18(x)            (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR1_REID20_REID18_SHIFT))&MSCAN_REIDR1_REID20_REID18_MASK)
/* RSIDR1 Bit Fields */
#define MSCAN_RSIDR1_RSIDE_MASK                  0x8u
#define MSCAN_RSIDR1_RSIDE_SHIFT                 3
#define MSCAN_RSIDR1_RSIDE_WIDTH                 1
#define MSCAN_RSIDR1_RSIDE(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_RSIDR1_RSIDE_SHIFT))&MSCAN_RSIDR1_RSIDE_MASK)
#define MSCAN_RSIDR1_RSRTR_MASK                  0x10u
#define MSCAN_RSIDR1_RSRTR_SHIFT                 4
#define MSCAN_RSIDR1_RSRTR_WIDTH                 1
#define MSCAN_RSIDR1_RSRTR(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_RSIDR1_RSRTR_SHIFT))&MSCAN_RSIDR1_RSRTR_MASK)
#define MSCAN_RSIDR1_RSID2_RSID0_MASK            0xE0u
#define MSCAN_RSIDR1_RSID2_RSID0_SHIFT           5
#define MSCAN_RSIDR1_RSID2_RSID0_WIDTH           3
#define MSCAN_RSIDR1_RSID2_RSID0(x)              (((uint8_t)(((uint8_t)(x))<<MSCAN_RSIDR1_RSID2_RSID0_SHIFT))&MSCAN_RSIDR1_RSID2_RSID0_MASK)
/* REIDR2 Bit Fields */
#define MSCAN_REIDR2_REID14_REID7_MASK           0xFFu
#define MSCAN_REIDR2_REID14_REID7_SHIFT          0
#define MSCAN_REIDR2_REID14_REID7_WIDTH          8
#define MSCAN_REIDR2_REID14_REID7(x)             (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR2_REID14_REID7_SHIFT))&MSCAN_REIDR2_REID14_REID7_MASK)
/* REIDR3 Bit Fields */
#define MSCAN_REIDR3_RERTR_MASK                  0x1u
#define MSCAN_REIDR3_RERTR_SHIFT                 0
#define MSCAN_REIDR3_RERTR_WIDTH                 1
#define MSCAN_REIDR3_RERTR(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR3_RERTR_SHIFT))&MSCAN_REIDR3_RERTR_MASK)
#define MSCAN_REIDR3_REID6_REID0_MASK            0xFEu
#define MSCAN_REIDR3_REID6_REID0_SHIFT           1
#define MSCAN_REIDR3_REID6_REID0_WIDTH           7
#define MSCAN_REIDR3_REID6_REID0(x)              (((uint8_t)(((uint8_t)(x))<<MSCAN_REIDR3_REID6_REID0_SHIFT))&MSCAN_REIDR3_REID6_REID0_MASK)
/* REDSR Bit Fields */
#define MSCAN_REDSR_RDB_MASK                     0xFFu
#define MSCAN_REDSR_RDB_SHIFT                    0
#define MSCAN_REDSR_RDB_WIDTH                    8
#define MSCAN_REDSR_RDB(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_REDSR_RDB_SHIFT))&MSCAN_REDSR_RDB_MASK)
/* RDLR Bit Fields */
#define MSCAN_RDLR_RDLC_MASK                     0xFu
#define MSCAN_RDLR_RDLC_SHIFT                    0
#define MSCAN_RDLR_RDLC_WIDTH                    4
#define MSCAN_RDLR_RDLC(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_RDLR_RDLC_SHIFT))&MSCAN_RDLR_RDLC_MASK)
/* RTSRH Bit Fields */
#define MSCAN_RTSRH_RTS_MASK                     0xFFu
#define MSCAN_RTSRH_RTS_SHIFT                    0
#define MSCAN_RTSRH_RTS_WIDTH                    8
#define MSCAN_RTSRH_RTS(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_RTSRH_RTS_SHIFT))&MSCAN_RTSRH_RTS_MASK)
/* RTSRL Bit Fields */
#define MSCAN_RTSRL_RTS_MASK                     0xFFu
#define MSCAN_RTSRL_RTS_SHIFT                    0
#define MSCAN_RTSRL_RTS_WIDTH                    8
#define MSCAN_RTSRL_RTS(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_RTSRL_RTS_SHIFT))&MSCAN_RTSRL_RTS_MASK)
/* TEIDR0 Bit Fields */
#define MSCAN_TEIDR0_TEID28_TEID21_MASK          0xFFu
#define MSCAN_TEIDR0_TEID28_TEID21_SHIFT         0
#define MSCAN_TEIDR0_TEID28_TEID21_WIDTH         8
#define MSCAN_TEIDR0_TEID28_TEID21(x)            (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR0_TEID28_TEID21_SHIFT))&MSCAN_TEIDR0_TEID28_TEID21_MASK)
/* TSIDR0 Bit Fields */
#define MSCAN_TSIDR0_TSID10_TSID3_MASK           0xFFu
#define MSCAN_TSIDR0_TSID10_TSID3_SHIFT          0
#define MSCAN_TSIDR0_TSID10_TSID3_WIDTH          8
#define MSCAN_TSIDR0_TSID10_TSID3(x)             (((uint8_t)(((uint8_t)(x))<<MSCAN_TSIDR0_TSID10_TSID3_SHIFT))&MSCAN_TSIDR0_TSID10_TSID3_MASK)
/* TEIDR1 Bit Fields */
#define MSCAN_TEIDR1_TEID17_TEID15_MASK          0x7u
#define MSCAN_TEIDR1_TEID17_TEID15_SHIFT         0
#define MSCAN_TEIDR1_TEID17_TEID15_WIDTH         3
#define MSCAN_TEIDR1_TEID17_TEID15(x)            (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR1_TEID17_TEID15_SHIFT))&MSCAN_TEIDR1_TEID17_TEID15_MASK)
#define MSCAN_TEIDR1_TEIDE_MASK                  0x8u
#define MSCAN_TEIDR1_TEIDE_SHIFT                 3
#define MSCAN_TEIDR1_TEIDE_WIDTH                 1
#define MSCAN_TEIDR1_TEIDE(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR1_TEIDE_SHIFT))&MSCAN_TEIDR1_TEIDE_MASK)
#define MSCAN_TEIDR1_TSRR_MASK                   0x10u
#define MSCAN_TEIDR1_TSRR_SHIFT                  4
#define MSCAN_TEIDR1_TSRR_WIDTH                  1
#define MSCAN_TEIDR1_TSRR(x)                     (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR1_TSRR_SHIFT))&MSCAN_TEIDR1_TSRR_MASK)
#define MSCAN_TEIDR1_TEID20_TEID18_MASK          0xE0u
#define MSCAN_TEIDR1_TEID20_TEID18_SHIFT         5
#define MSCAN_TEIDR1_TEID20_TEID18_WIDTH         3
#define MSCAN_TEIDR1_TEID20_TEID18(x)            (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR1_TEID20_TEID18_SHIFT))&MSCAN_TEIDR1_TEID20_TEID18_MASK)
/* TSIDR1 Bit Fields */
#define MSCAN_TSIDR1_TSIDE_MASK                  0x8u
#define MSCAN_TSIDR1_TSIDE_SHIFT                 3
#define MSCAN_TSIDR1_TSIDE_WIDTH                 1
#define MSCAN_TSIDR1_TSIDE(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_TSIDR1_TSIDE_SHIFT))&MSCAN_TSIDR1_TSIDE_MASK)
#define MSCAN_TSIDR1_TSRTR_MASK                  0x10u
#define MSCAN_TSIDR1_TSRTR_SHIFT                 4
#define MSCAN_TSIDR1_TSRTR_WIDTH                 1
#define MSCAN_TSIDR1_TSRTR(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_TSIDR1_TSRTR_SHIFT))&MSCAN_TSIDR1_TSRTR_MASK)
#define MSCAN_TSIDR1_TSID2_TSID0_MASK            0xE0u
#define MSCAN_TSIDR1_TSID2_TSID0_SHIFT           5
#define MSCAN_TSIDR1_TSID2_TSID0_WIDTH           3
#define MSCAN_TSIDR1_TSID2_TSID0(x)              (((uint8_t)(((uint8_t)(x))<<MSCAN_TSIDR1_TSID2_TSID0_SHIFT))&MSCAN_TSIDR1_TSID2_TSID0_MASK)
/* TEIDR2 Bit Fields */
#define MSCAN_TEIDR2_TEID14_TEID7_MASK           0xFFu
#define MSCAN_TEIDR2_TEID14_TEID7_SHIFT          0
#define MSCAN_TEIDR2_TEID14_TEID7_WIDTH          8
#define MSCAN_TEIDR2_TEID14_TEID7(x)             (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR2_TEID14_TEID7_SHIFT))&MSCAN_TEIDR2_TEID14_TEID7_MASK)
/* TEIDR3 Bit Fields */
#define MSCAN_TEIDR3_TERTR_MASK                  0x1u
#define MSCAN_TEIDR3_TERTR_SHIFT                 0
#define MSCAN_TEIDR3_TERTR_WIDTH                 1
#define MSCAN_TEIDR3_TERTR(x)                    (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR3_TERTR_SHIFT))&MSCAN_TEIDR3_TERTR_MASK)
#define MSCAN_TEIDR3_TEID6_TEID0_MASK            0xFEu
#define MSCAN_TEIDR3_TEID6_TEID0_SHIFT           1
#define MSCAN_TEIDR3_TEID6_TEID0_WIDTH           7
#define MSCAN_TEIDR3_TEID6_TEID0(x)              (((uint8_t)(((uint8_t)(x))<<MSCAN_TEIDR3_TEID6_TEID0_SHIFT))&MSCAN_TEIDR3_TEID6_TEID0_MASK)
/* TEDSR Bit Fields */
#define MSCAN_TEDSR_TDB_MASK                     0xFFu
#define MSCAN_TEDSR_TDB_SHIFT                    0
#define MSCAN_TEDSR_TDB_WIDTH                    8
#define MSCAN_TEDSR_TDB(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_TEDSR_TDB_SHIFT))&MSCAN_TEDSR_TDB_MASK)
/* TDLR Bit Fields */
#define MSCAN_TDLR_TDLC_MASK                     0xFu
#define MSCAN_TDLR_TDLC_SHIFT                    0
#define MSCAN_TDLR_TDLC_WIDTH                    4
#define MSCAN_TDLR_TDLC(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_TDLR_TDLC_SHIFT))&MSCAN_TDLR_TDLC_MASK)
/* TBPR Bit Fields */
#define MSCAN_TBPR_PRIO_MASK                     0xFFu
#define MSCAN_TBPR_PRIO_SHIFT                    0
#define MSCAN_TBPR_PRIO_WIDTH                    8
#define MSCAN_TBPR_PRIO(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_TBPR_PRIO_SHIFT))&MSCAN_TBPR_PRIO_MASK)
/* TTSRH Bit Fields */
#define MSCAN_TTSRH_TTS_MASK                     0xFFu
#define MSCAN_TTSRH_TTS_SHIFT                    0
#define MSCAN_TTSRH_TTS_WIDTH                    8
#define MSCAN_TTSRH_TTS(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_TTSRH_TTS_SHIFT))&MSCAN_TTSRH_TTS_MASK)
/* TTSRL Bit Fields */
#define MSCAN_TTSRL_TTS_MASK                     0xFFu
#define MSCAN_TTSRL_TTS_SHIFT                    0
#define MSCAN_TTSRL_TTS_WIDTH                    8
#define MSCAN_TTSRL_TTS(x)                       (((uint8_t)(((uint8_t)(x))<<MSCAN_TTSRL_TTS_SHIFT))&MSCAN_TTSRL_TTS_MASK)

/*!
 * @}
 */ /* end of group MSCAN_Register_Masks */


/* MSCAN - Peripheral instance base addresses */
/** Peripheral MSCAN base address */
#define MSCAN_BASE                               (0x40024000u)
/** Peripheral MSCAN base pointer */
#define MSCAN                                    ((MSCAN_Type *)MSCAN_BASE)
#define MSCAN_BASE_PTR                           (MSCAN)
/** Array initializer of MSCAN peripheral base addresses */
#define MSCAN_BASE_ADDRS                         { MSCAN_BASE }
/** Array initializer of MSCAN peripheral base pointers */
#define MSCAN_BASE_PTRS                          { MSCAN }

/* ----------------------------------------------------------------------------
   -- MSCAN - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MSCAN_Register_Accessor_Macros MSCAN - Register accessor macros
 * @{
 */


/* MSCAN - Register instance definitions */
/* MSCAN */
#define MSCAN_CANCTL0                            MSCAN_CANCTL0_REG(MSCAN)
#define MSCAN_CANCTL1                            MSCAN_CANCTL1_REG(MSCAN)
#define MSCAN_CANBTR0                            MSCAN_CANBTR0_REG(MSCAN)
#define MSCAN_CANBTR1                            MSCAN_CANBTR1_REG(MSCAN)
#define MSCAN_CANRFLG                            MSCAN_CANRFLG_REG(MSCAN)
#define MSCAN_CANRIER                            MSCAN_CANRIER_REG(MSCAN)
#define MSCAN_CANTFLG                            MSCAN_CANTFLG_REG(MSCAN)
#define MSCAN_CANTIER                            MSCAN_CANTIER_REG(MSCAN)
#define MSCAN_CANTARQ                            MSCAN_CANTARQ_REG(MSCAN)
#define MSCAN_CANTAAK                            MSCAN_CANTAAK_REG(MSCAN)
#define MSCAN_CANTBSEL                           MSCAN_CANTBSEL_REG(MSCAN)
#define MSCAN_CANIDAC                            MSCAN_CANIDAC_REG(MSCAN)
#define MSCAN_CANMISC                            MSCAN_CANMISC_REG(MSCAN)
#define MSCAN_CANRXERR                           MSCAN_CANRXERR_REG(MSCAN)
#define MSCAN_CANTXERR                           MSCAN_CANTXERR_REG(MSCAN)
#define MSCAN_CANIDAR0                           MSCAN_CANIDAR_BANK_1_REG(MSCAN,0)
#define MSCAN_CANIDAR1                           MSCAN_CANIDAR_BANK_1_REG(MSCAN,1)
#define MSCAN_CANIDAR2                           MSCAN_CANIDAR_BANK_1_REG(MSCAN,2)
#define MSCAN_CANIDAR3                           MSCAN_CANIDAR_BANK_1_REG(MSCAN,3)
#define MSCAN_CANIDMR0                           MSCAN_CANIDMR_BANK_1_REG(MSCAN,0)
#define MSCAN_CANIDMR1                           MSCAN_CANIDMR_BANK_1_REG(MSCAN,1)
#define MSCAN_CANIDMR2                           MSCAN_CANIDMR_BANK_1_REG(MSCAN,2)
#define MSCAN_CANIDMR3                           MSCAN_CANIDMR_BANK_1_REG(MSCAN,3)
#define MSCAN_CANIDAR4                           MSCAN_CANIDAR_BANK_2_REG(MSCAN,0)
#define MSCAN_CANIDAR5                           MSCAN_CANIDAR_BANK_2_REG(MSCAN,1)
#define MSCAN_CANIDAR6                           MSCAN_CANIDAR_BANK_2_REG(MSCAN,2)
#define MSCAN_CANIDAR7                           MSCAN_CANIDAR_BANK_2_REG(MSCAN,3)
#define MSCAN_CANIDMR4                           MSCAN_CANIDMR_BANK_2_REG(MSCAN,0)
#define MSCAN_CANIDMR5                           MSCAN_CANIDMR_BANK_2_REG(MSCAN,1)
#define MSCAN_CANIDMR6                           MSCAN_CANIDMR_BANK_2_REG(MSCAN,2)
#define MSCAN_CANIDMR7                           MSCAN_CANIDMR_BANK_2_REG(MSCAN,3)
#define MSCAN_REIDR0                             MSCAN_REIDR0_REG(MSCAN)
#define MSCAN_RSIDR0                             MSCAN_RSIDR0_REG(MSCAN)
#define MSCAN_REIDR1                             MSCAN_REIDR1_REG(MSCAN)
#define MSCAN_RSIDR1                             MSCAN_RSIDR1_REG(MSCAN)
#define MSCAN_REIDR2                             MSCAN_REIDR2_REG(MSCAN)
#define MSCAN_REIDR3                             MSCAN_REIDR3_REG(MSCAN)
#define MSCAN_REDSR0                             MSCAN_REDSR_REG(MSCAN,0)
#define MSCAN_REDSR1                             MSCAN_REDSR_REG(MSCAN,1)
#define MSCAN_REDSR2                             MSCAN_REDSR_REG(MSCAN,2)
#define MSCAN_REDSR3                             MSCAN_REDSR_REG(MSCAN,3)
#define MSCAN_REDSR4                             MSCAN_REDSR_REG(MSCAN,4)
#define MSCAN_REDSR5                             MSCAN_REDSR_REG(MSCAN,5)
#define MSCAN_REDSR6                             MSCAN_REDSR_REG(MSCAN,6)
#define MSCAN_REDSR7                             MSCAN_REDSR_REG(MSCAN,7)
#define MSCAN_RDLR                               MSCAN_RDLR_REG(MSCAN)
#define MSCAN_RTSRH                              MSCAN_RTSRH_REG(MSCAN)
#define MSCAN_RTSRL                              MSCAN_RTSRL_REG(MSCAN)
#define MSCAN_TEIDR0                             MSCAN_TEIDR0_REG(MSCAN)
#define MSCAN_TSIDR0                             MSCAN_TSIDR0_REG(MSCAN)
#define MSCAN_TEIDR1                             MSCAN_TEIDR1_REG(MSCAN)
#define MSCAN_TSIDR1                             MSCAN_TSIDR1_REG(MSCAN)
#define MSCAN_TEIDR2                             MSCAN_TEIDR2_REG(MSCAN)
#define MSCAN_TEIDR3                             MSCAN_TEIDR3_REG(MSCAN)
#define MSCAN_TEDSR0                             MSCAN_TEDSR_REG(MSCAN,0)
#define MSCAN_TEDSR1                             MSCAN_TEDSR_REG(MSCAN,1)
#define MSCAN_TEDSR2                             MSCAN_TEDSR_REG(MSCAN,2)
#define MSCAN_TEDSR3                             MSCAN_TEDSR_REG(MSCAN,3)
#define MSCAN_TEDSR4                             MSCAN_TEDSR_REG(MSCAN,4)
#define MSCAN_TEDSR5                             MSCAN_TEDSR_REG(MSCAN,5)
#define MSCAN_TEDSR6                             MSCAN_TEDSR_REG(MSCAN,6)
#define MSCAN_TEDSR7                             MSCAN_TEDSR_REG(MSCAN,7)
#define MSCAN_TDLR                               MSCAN_TDLR_REG(MSCAN)
#define MSCAN_TBPR                               MSCAN_TBPR_REG(MSCAN)
#define MSCAN_TTSRH                              MSCAN_TTSRH_REG(MSCAN)
#define MSCAN_TTSRL                              MSCAN_TTSRL_REG(MSCAN)

/* MSCAN - Register array accessors */
#define MSCAN_CANIDAR_BANK_1(index)              MSCAN_CANIDAR_BANK_1_REG(MSCAN,index)
#define MSCAN_CANIDMR_BANK_1(index)              MSCAN_CANIDMR_BANK_1_REG(MSCAN,index)
#define MSCAN_CANIDAR_BANK_2(index)              MSCAN_CANIDAR_BANK_2_REG(MSCAN,index)
#define MSCAN_CANIDMR_BANK_2(index)              MSCAN_CANIDMR_BANK_2_REG(MSCAN,index)
#define MSCAN_REDSR(index)                       MSCAN_REDSR_REG(MSCAN,index)
#define MSCAN_TEDSR(index)                       MSCAN_TEDSR_REG(MSCAN,index)

/*!
 * @}
 */ /* end of group MSCAN_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group MSCAN_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- NV Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup NV_Peripheral_Access_Layer NV Peripheral Access Layer
 * @{
 */

/** NV - Register Layout Typedef */
typedef struct {
  __I  uint8_t BACKKEY0;                           /**< Backdoor Comparison Key 0, offset: 0x0 */
  __I  uint8_t BACKKEY1;                           /**< Backdoor Comparison Key 1, offset: 0x1 */
  __I  uint8_t BACKKEY2;                           /**< Backdoor Comparison Key 2, offset: 0x2 */
  __I  uint8_t BACKKEY3;                           /**< Backdoor Comparison Key 3, offset: 0x3 */
  __I  uint8_t BACKKEY4;                           /**< Backdoor Comparison Key 4, offset: 0x4 */
  __I  uint8_t BACKKEY5;                           /**< Backdoor Comparison Key 5, offset: 0x5 */
  __I  uint8_t BACKKEY6;                           /**< Backdoor Comparison Key 6, offset: 0x6 */
  __I  uint8_t BACKKEY7;                           /**< Backdoor Comparison Key 7, offset: 0x7 */
       uint8_t RESERVED_0[5];
  __I  uint8_t FPROT;                              /**< Non-volatile P-Flash Protection Register, offset: 0xD */
  __I  uint8_t FSEC;                               /**< Non-volatile Flash Security Register, offset: 0xE */
  __I  uint8_t FOPT;                               /**< Non-volatile Flash Option Register, offset: 0xF */
} NV_Type, *NV_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- NV - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup NV_Register_Accessor_Macros NV - Register accessor macros
 * @{
 */


/* NV - Register accessors */
#define NV_BACKKEY0_REG(base)                    ((base)->BACKKEY0)
#define NV_BACKKEY1_REG(base)                    ((base)->BACKKEY1)
#define NV_BACKKEY2_REG(base)                    ((base)->BACKKEY2)
#define NV_BACKKEY3_REG(base)                    ((base)->BACKKEY3)
#define NV_BACKKEY4_REG(base)                    ((base)->BACKKEY4)
#define NV_BACKKEY5_REG(base)                    ((base)->BACKKEY5)
#define NV_BACKKEY6_REG(base)                    ((base)->BACKKEY6)
#define NV_BACKKEY7_REG(base)                    ((base)->BACKKEY7)
#define NV_FPROT_REG(base)                       ((base)->FPROT)
#define NV_FSEC_REG(base)                        ((base)->FSEC)
#define NV_FOPT_REG(base)                        ((base)->FOPT)

/*!
 * @}
 */ /* end of group NV_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- NV Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup NV_Register_Masks NV Register Masks
 * @{
 */

/* BACKKEY0 Bit Fields */
#define NV_BACKKEY0_KEY_MASK                     0xFFu
#define NV_BACKKEY0_KEY_SHIFT                    0
#define NV_BACKKEY0_KEY_WIDTH                    8
#define NV_BACKKEY0_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY0_KEY_SHIFT))&NV_BACKKEY0_KEY_MASK)
/* BACKKEY1 Bit Fields */
#define NV_BACKKEY1_KEY_MASK                     0xFFu
#define NV_BACKKEY1_KEY_SHIFT                    0
#define NV_BACKKEY1_KEY_WIDTH                    8
#define NV_BACKKEY1_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY1_KEY_SHIFT))&NV_BACKKEY1_KEY_MASK)
/* BACKKEY2 Bit Fields */
#define NV_BACKKEY2_KEY_MASK                     0xFFu
#define NV_BACKKEY2_KEY_SHIFT                    0
#define NV_BACKKEY2_KEY_WIDTH                    8
#define NV_BACKKEY2_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY2_KEY_SHIFT))&NV_BACKKEY2_KEY_MASK)
/* BACKKEY3 Bit Fields */
#define NV_BACKKEY3_KEY_MASK                     0xFFu
#define NV_BACKKEY3_KEY_SHIFT                    0
#define NV_BACKKEY3_KEY_WIDTH                    8
#define NV_BACKKEY3_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY3_KEY_SHIFT))&NV_BACKKEY3_KEY_MASK)
/* BACKKEY4 Bit Fields */
#define NV_BACKKEY4_KEY_MASK                     0xFFu
#define NV_BACKKEY4_KEY_SHIFT                    0
#define NV_BACKKEY4_KEY_WIDTH                    8
#define NV_BACKKEY4_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY4_KEY_SHIFT))&NV_BACKKEY4_KEY_MASK)
/* BACKKEY5 Bit Fields */
#define NV_BACKKEY5_KEY_MASK                     0xFFu
#define NV_BACKKEY5_KEY_SHIFT                    0
#define NV_BACKKEY5_KEY_WIDTH                    8
#define NV_BACKKEY5_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY5_KEY_SHIFT))&NV_BACKKEY5_KEY_MASK)
/* BACKKEY6 Bit Fields */
#define NV_BACKKEY6_KEY_MASK                     0xFFu
#define NV_BACKKEY6_KEY_SHIFT                    0
#define NV_BACKKEY6_KEY_WIDTH                    8
#define NV_BACKKEY6_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY6_KEY_SHIFT))&NV_BACKKEY6_KEY_MASK)
/* BACKKEY7 Bit Fields */
#define NV_BACKKEY7_KEY_MASK                     0xFFu
#define NV_BACKKEY7_KEY_SHIFT                    0
#define NV_BACKKEY7_KEY_WIDTH                    8
#define NV_BACKKEY7_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY7_KEY_SHIFT))&NV_BACKKEY7_KEY_MASK)
/* FPROT Bit Fields */
#define NV_FPROT_FPLS_MASK                       0x3u
#define NV_FPROT_FPLS_SHIFT                      0
#define NV_FPROT_FPLS_WIDTH                      2
#define NV_FPROT_FPLS(x)                         (((uint8_t)(((uint8_t)(x))<<NV_FPROT_FPLS_SHIFT))&NV_FPROT_FPLS_MASK)
#define NV_FPROT_FPLDIS_MASK                     0x4u
#define NV_FPROT_FPLDIS_SHIFT                    2
#define NV_FPROT_FPLDIS_WIDTH                    1
#define NV_FPROT_FPLDIS(x)                       (((uint8_t)(((uint8_t)(x))<<NV_FPROT_FPLDIS_SHIFT))&NV_FPROT_FPLDIS_MASK)
#define NV_FPROT_FPHS_MASK                       0x18u
#define NV_FPROT_FPHS_SHIFT                      3
#define NV_FPROT_FPHS_WIDTH                      2
#define NV_FPROT_FPHS(x)                         (((uint8_t)(((uint8_t)(x))<<NV_FPROT_FPHS_SHIFT))&NV_FPROT_FPHS_MASK)
#define NV_FPROT_FPHDIS_MASK                     0x20u
#define NV_FPROT_FPHDIS_SHIFT                    5
#define NV_FPROT_FPHDIS_WIDTH                    1
#define NV_FPROT_FPHDIS(x)                       (((uint8_t)(((uint8_t)(x))<<NV_FPROT_FPHDIS_SHIFT))&NV_FPROT_FPHDIS_MASK)
#define NV_FPROT_FPOPEN_MASK                     0x80u
#define NV_FPROT_FPOPEN_SHIFT                    7
#define NV_FPROT_FPOPEN_WIDTH                    1
#define NV_FPROT_FPOPEN(x)                       (((uint8_t)(((uint8_t)(x))<<NV_FPROT_FPOPEN_SHIFT))&NV_FPROT_FPOPEN_MASK)
/* FSEC Bit Fields */
#define NV_FSEC_SEC_MASK                         0x3u
#define NV_FSEC_SEC_SHIFT                        0
#define NV_FSEC_SEC_WIDTH                        2
#define NV_FSEC_SEC(x)                           (((uint8_t)(((uint8_t)(x))<<NV_FSEC_SEC_SHIFT))&NV_FSEC_SEC_MASK)
#define NV_FSEC_KEYEN_MASK                       0xC0u
#define NV_FSEC_KEYEN_SHIFT                      6
#define NV_FSEC_KEYEN_WIDTH                      2
#define NV_FSEC_KEYEN(x)                         (((uint8_t)(((uint8_t)(x))<<NV_FSEC_KEYEN_SHIFT))&NV_FSEC_KEYEN_MASK)

/*!
 * @}
 */ /* end of group NV_Register_Masks */


/* NV - Peripheral instance base addresses */
/** Peripheral FTMRE_FlashConfig base address */
#define FTMRE_FlashConfig_BASE                   (0x400u)
/** Peripheral FTMRE_FlashConfig base pointer */
#define FTMRE_FlashConfig                        ((NV_Type *)FTMRE_FlashConfig_BASE)
#define FTMRE_FlashConfig_BASE_PTR               (FTMRE_FlashConfig)
/** Array initializer of NV peripheral base addresses */
#define NV_BASE_ADDRS                            { FTMRE_FlashConfig_BASE }
/** Array initializer of NV peripheral base pointers */
#define NV_BASE_PTRS                             { FTMRE_FlashConfig }

/* ----------------------------------------------------------------------------
   -- NV - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup NV_Register_Accessor_Macros NV - Register accessor macros
 * @{
 */


/* NV - Register instance definitions */
/* FTMRE_FlashConfig */
#define NV_BACKKEY0                              NV_BACKKEY0_REG(FTMRE_FlashConfig)
#define NV_BACKKEY1                              NV_BACKKEY1_REG(FTMRE_FlashConfig)
#define NV_BACKKEY2                              NV_BACKKEY2_REG(FTMRE_FlashConfig)
#define NV_BACKKEY3                              NV_BACKKEY3_REG(FTMRE_FlashConfig)
#define NV_BACKKEY4                              NV_BACKKEY4_REG(FTMRE_FlashConfig)
#define NV_BACKKEY5                              NV_BACKKEY5_REG(FTMRE_FlashConfig)
#define NV_BACKKEY6                              NV_BACKKEY6_REG(FTMRE_FlashConfig)
#define NV_BACKKEY7                              NV_BACKKEY7_REG(FTMRE_FlashConfig)
#define NV_FPROT                                 NV_FPROT_REG(FTMRE_FlashConfig)
#define NV_FSEC                                  NV_FSEC_REG(FTMRE_FlashConfig)
#define NV_FOPT                                  NV_FOPT_REG(FTMRE_FlashConfig)

/*!
 * @}
 */ /* end of group NV_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group NV_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- OSC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup OSC_Peripheral_Access_Layer OSC Peripheral Access Layer
 * @{
 */

/** OSC - Register Layout Typedef */
typedef struct {
  __IO uint8_t CR;                                 /**< OSC Control Register, offset: 0x0 */
} OSC_Type, *OSC_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- OSC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup OSC_Register_Accessor_Macros OSC - Register accessor macros
 * @{
 */


/* OSC - Register accessors */
#define OSC_CR_REG(base)                         ((base)->CR)

/*!
 * @}
 */ /* end of group OSC_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- OSC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup OSC_Register_Masks OSC Register Masks
 * @{
 */

/* CR Bit Fields */
#define OSC_CR_OSCINIT_MASK                      0x1u
#define OSC_CR_OSCINIT_SHIFT                     0
#define OSC_CR_OSCINIT_WIDTH                     1
#define OSC_CR_OSCINIT(x)                        (((uint8_t)(((uint8_t)(x))<<OSC_CR_OSCINIT_SHIFT))&OSC_CR_OSCINIT_MASK)
#define OSC_CR_HGO_MASK                          0x2u
#define OSC_CR_HGO_SHIFT                         1
#define OSC_CR_HGO_WIDTH                         1
#define OSC_CR_HGO(x)                            (((uint8_t)(((uint8_t)(x))<<OSC_CR_HGO_SHIFT))&OSC_CR_HGO_MASK)
#define OSC_CR_RANGE_MASK                        0x4u
#define OSC_CR_RANGE_SHIFT                       2
#define OSC_CR_RANGE_WIDTH                       1
#define OSC_CR_RANGE(x)                          (((uint8_t)(((uint8_t)(x))<<OSC_CR_RANGE_SHIFT))&OSC_CR_RANGE_MASK)
#define OSC_CR_OSCOS_MASK                        0x10u
#define OSC_CR_OSCOS_SHIFT                       4
#define OSC_CR_OSCOS_WIDTH                       1
#define OSC_CR_OSCOS(x)                          (((uint8_t)(((uint8_t)(x))<<OSC_CR_OSCOS_SHIFT))&OSC_CR_OSCOS_MASK)
#define OSC_CR_OSCSTEN_MASK                      0x20u
#define OSC_CR_OSCSTEN_SHIFT                     5
#define OSC_CR_OSCSTEN_WIDTH                     1
#define OSC_CR_OSCSTEN(x)                        (((uint8_t)(((uint8_t)(x))<<OSC_CR_OSCSTEN_SHIFT))&OSC_CR_OSCSTEN_MASK)
#define OSC_CR_OSCEN_MASK                        0x80u
#define OSC_CR_OSCEN_SHIFT                       7
#define OSC_CR_OSCEN_WIDTH                       1
#define OSC_CR_OSCEN(x)                          (((uint8_t)(((uint8_t)(x))<<OSC_CR_OSCEN_SHIFT))&OSC_CR_OSCEN_MASK)

/*!
 * @}
 */ /* end of group OSC_Register_Masks */


/* OSC - Peripheral instance base addresses */
/** Peripheral OSC base address */
#define OSC_BASE                                 (0x40065000u)
/** Peripheral OSC base pointer */
#define OSC                                      ((OSC_Type *)OSC_BASE)
#define OSC_BASE_PTR                             (OSC)
/** Array initializer of OSC peripheral base addresses */
#define OSC_BASE_ADDRS                           { OSC_BASE }
/** Array initializer of OSC peripheral base pointers */
#define OSC_BASE_PTRS                            { OSC }

/* ----------------------------------------------------------------------------
   -- OSC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup OSC_Register_Accessor_Macros OSC - Register accessor macros
 * @{
 */


/* OSC - Register instance definitions */
/* OSC */
#define OSC_CR                                   OSC_CR_REG(OSC)

/*!
 * @}
 */ /* end of group OSC_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group OSC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Peripheral_Access_Layer PIT Peripheral Access Layer
 * @{
 */

/** PIT - Register Layout Typedef */
typedef struct {
  __IO uint32_t MCR;                               /**< PIT Module Control Register, offset: 0x0 */
       uint8_t RESERVED_0[252];
  struct {                                         /* offset: 0x100, array step: 0x10 */
    __IO uint32_t LDVAL;                             /**< Timer Load Value Register, array offset: 0x100, array step: 0x10 */
    __I  uint32_t CVAL;                              /**< Current Timer Value Register, array offset: 0x104, array step: 0x10 */
    __IO uint32_t TCTRL;                             /**< Timer Control Register, array offset: 0x108, array step: 0x10 */
    __IO uint32_t TFLG;                              /**< Timer Flag Register, array offset: 0x10C, array step: 0x10 */
  } CHANNEL[2];
} PIT_Type, *PIT_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- PIT - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Register_Accessor_Macros PIT - Register accessor macros
 * @{
 */


/* PIT - Register accessors */
#define PIT_MCR_REG(base)                        ((base)->MCR)
#define PIT_LDVAL_REG(base,index)                ((base)->CHANNEL[index].LDVAL)
#define PIT_LDVAL_COUNT                          2
#define PIT_CVAL_REG(base,index)                 ((base)->CHANNEL[index].CVAL)
#define PIT_CVAL_COUNT                           2
#define PIT_TCTRL_REG(base,index)                ((base)->CHANNEL[index].TCTRL)
#define PIT_TCTRL_COUNT                          2
#define PIT_TFLG_REG(base,index)                 ((base)->CHANNEL[index].TFLG)
#define PIT_TFLG_COUNT                           2

/*!
 * @}
 */ /* end of group PIT_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- PIT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Register_Masks PIT Register Masks
 * @{
 */

/* MCR Bit Fields */
#define PIT_MCR_FRZ_MASK                         0x1u
#define PIT_MCR_FRZ_SHIFT                        0
#define PIT_MCR_FRZ_WIDTH                        1
#define PIT_MCR_FRZ(x)                           (((uint32_t)(((uint32_t)(x))<<PIT_MCR_FRZ_SHIFT))&PIT_MCR_FRZ_MASK)
#define PIT_MCR_MDIS_MASK                        0x2u
#define PIT_MCR_MDIS_SHIFT                       1
#define PIT_MCR_MDIS_WIDTH                       1
#define PIT_MCR_MDIS(x)                          (((uint32_t)(((uint32_t)(x))<<PIT_MCR_MDIS_SHIFT))&PIT_MCR_MDIS_MASK)
/* LDVAL Bit Fields */
#define PIT_LDVAL_TSV_MASK                       0xFFFFFFFFu
#define PIT_LDVAL_TSV_SHIFT                      0
#define PIT_LDVAL_TSV_WIDTH                      32
#define PIT_LDVAL_TSV(x)                         (((uint32_t)(((uint32_t)(x))<<PIT_LDVAL_TSV_SHIFT))&PIT_LDVAL_TSV_MASK)
/* CVAL Bit Fields */
#define PIT_CVAL_TVL_MASK                        0xFFFFFFFFu
#define PIT_CVAL_TVL_SHIFT                       0
#define PIT_CVAL_TVL_WIDTH                       32
#define PIT_CVAL_TVL(x)                          (((uint32_t)(((uint32_t)(x))<<PIT_CVAL_TVL_SHIFT))&PIT_CVAL_TVL_MASK)
/* TCTRL Bit Fields */
#define PIT_TCTRL_TEN_MASK                       0x1u
#define PIT_TCTRL_TEN_SHIFT                      0
#define PIT_TCTRL_TEN_WIDTH                      1
#define PIT_TCTRL_TEN(x)                         (((uint32_t)(((uint32_t)(x))<<PIT_TCTRL_TEN_SHIFT))&PIT_TCTRL_TEN_MASK)
#define PIT_TCTRL_TIE_MASK                       0x2u
#define PIT_TCTRL_TIE_SHIFT                      1
#define PIT_TCTRL_TIE_WIDTH                      1
#define PIT_TCTRL_TIE(x)                         (((uint32_t)(((uint32_t)(x))<<PIT_TCTRL_TIE_SHIFT))&PIT_TCTRL_TIE_MASK)
#define PIT_TCTRL_CHN_MASK                       0x4u
#define PIT_TCTRL_CHN_SHIFT                      2
#define PIT_TCTRL_CHN_WIDTH                      1
#define PIT_TCTRL_CHN(x)                         (((uint32_t)(((uint32_t)(x))<<PIT_TCTRL_CHN_SHIFT))&PIT_TCTRL_CHN_MASK)
/* TFLG Bit Fields */
#define PIT_TFLG_TIF_MASK                        0x1u
#define PIT_TFLG_TIF_SHIFT                       0
#define PIT_TFLG_TIF_WIDTH                       1
#define PIT_TFLG_TIF(x)                          (((uint32_t)(((uint32_t)(x))<<PIT_TFLG_TIF_SHIFT))&PIT_TFLG_TIF_MASK)

/*!
 * @}
 */ /* end of group PIT_Register_Masks */


/* PIT - Peripheral instance base addresses */
/** Peripheral PIT base address */
#define PIT_BASE                                 (0x40037000u)
/** Peripheral PIT base pointer */
#define PIT                                      ((PIT_Type *)PIT_BASE)
#define PIT_BASE_PTR                             (PIT)
/** Array initializer of PIT peripheral base addresses */
#define PIT_BASE_ADDRS                           { PIT_BASE }
/** Array initializer of PIT peripheral base pointers */
#define PIT_BASE_PTRS                            { PIT }

/* ----------------------------------------------------------------------------
   -- PIT - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Register_Accessor_Macros PIT - Register accessor macros
 * @{
 */


/* PIT - Register instance definitions */
/* PIT */
#define PIT_MCR                                  PIT_MCR_REG(PIT)
#define PIT_LDVAL0                               PIT_LDVAL_REG(PIT,0)
#define PIT_CVAL0                                PIT_CVAL_REG(PIT,0)
#define PIT_TCTRL0                               PIT_TCTRL_REG(PIT,0)
#define PIT_TFLG0                                PIT_TFLG_REG(PIT,0)
#define PIT_LDVAL1                               PIT_LDVAL_REG(PIT,1)
#define PIT_CVAL1                                PIT_CVAL_REG(PIT,1)
#define PIT_TCTRL1                               PIT_TCTRL_REG(PIT,1)
#define PIT_TFLG1                                PIT_TFLG_REG(PIT,1)

/* PIT - Register array accessors */
#define PIT_LDVAL(index)                         PIT_LDVAL_REG(PIT,index)
#define PIT_CVAL(index)                          PIT_CVAL_REG(PIT,index)
#define PIT_TCTRL(index)                         PIT_TCTRL_REG(PIT,index)
#define PIT_TFLG(index)                          PIT_TFLG_REG(PIT,index)

/*!
 * @}
 */ /* end of group PIT_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group PIT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PMC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMC_Peripheral_Access_Layer PMC Peripheral Access Layer
 * @{
 */

/** PMC - Register Layout Typedef */
typedef struct {
  __IO uint8_t SPMSC1;                             /**< System Power Management Status and Control 1 Register, offset: 0x0 */
  __IO uint8_t SPMSC2;                             /**< System Power Management Status and Control 2 Register, offset: 0x1 */
} PMC_Type, *PMC_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- PMC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMC_Register_Accessor_Macros PMC - Register accessor macros
 * @{
 */


/* PMC - Register accessors */
#define PMC_SPMSC1_REG(base)                     ((base)->SPMSC1)
#define PMC_SPMSC2_REG(base)                     ((base)->SPMSC2)

/*!
 * @}
 */ /* end of group PMC_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- PMC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMC_Register_Masks PMC Register Masks
 * @{
 */

/* SPMSC1 Bit Fields */
#define PMC_SPMSC1_BGBE_MASK                     0x1u
#define PMC_SPMSC1_BGBE_SHIFT                    0
#define PMC_SPMSC1_BGBE_WIDTH                    1
#define PMC_SPMSC1_BGBE(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_BGBE_SHIFT))&PMC_SPMSC1_BGBE_MASK)
#define PMC_SPMSC1_LVDE_MASK                     0x4u
#define PMC_SPMSC1_LVDE_SHIFT                    2
#define PMC_SPMSC1_LVDE_WIDTH                    1
#define PMC_SPMSC1_LVDE(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_LVDE_SHIFT))&PMC_SPMSC1_LVDE_MASK)
#define PMC_SPMSC1_LVDSE_MASK                    0x8u
#define PMC_SPMSC1_LVDSE_SHIFT                   3
#define PMC_SPMSC1_LVDSE_WIDTH                   1
#define PMC_SPMSC1_LVDSE(x)                      (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_LVDSE_SHIFT))&PMC_SPMSC1_LVDSE_MASK)
#define PMC_SPMSC1_LVDRE_MASK                    0x10u
#define PMC_SPMSC1_LVDRE_SHIFT                   4
#define PMC_SPMSC1_LVDRE_WIDTH                   1
#define PMC_SPMSC1_LVDRE(x)                      (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_LVDRE_SHIFT))&PMC_SPMSC1_LVDRE_MASK)
#define PMC_SPMSC1_LVWIE_MASK                    0x20u
#define PMC_SPMSC1_LVWIE_SHIFT                   5
#define PMC_SPMSC1_LVWIE_WIDTH                   1
#define PMC_SPMSC1_LVWIE(x)                      (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_LVWIE_SHIFT))&PMC_SPMSC1_LVWIE_MASK)
#define PMC_SPMSC1_LVWACK_MASK                   0x40u
#define PMC_SPMSC1_LVWACK_SHIFT                  6
#define PMC_SPMSC1_LVWACK_WIDTH                  1
#define PMC_SPMSC1_LVWACK(x)                     (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_LVWACK_SHIFT))&PMC_SPMSC1_LVWACK_MASK)
#define PMC_SPMSC1_LVWF_MASK                     0x80u
#define PMC_SPMSC1_LVWF_SHIFT                    7
#define PMC_SPMSC1_LVWF_WIDTH                    1
#define PMC_SPMSC1_LVWF(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC1_LVWF_SHIFT))&PMC_SPMSC1_LVWF_MASK)
/* SPMSC2 Bit Fields */
#define PMC_SPMSC2_LVWV_MASK                     0x30u
#define PMC_SPMSC2_LVWV_SHIFT                    4
#define PMC_SPMSC2_LVWV_WIDTH                    2
#define PMC_SPMSC2_LVWV(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC2_LVWV_SHIFT))&PMC_SPMSC2_LVWV_MASK)
#define PMC_SPMSC2_LVDV_MASK                     0x40u
#define PMC_SPMSC2_LVDV_SHIFT                    6
#define PMC_SPMSC2_LVDV_WIDTH                    1
#define PMC_SPMSC2_LVDV(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_SPMSC2_LVDV_SHIFT))&PMC_SPMSC2_LVDV_MASK)

/*!
 * @}
 */ /* end of group PMC_Register_Masks */


/* PMC - Peripheral instance base addresses */
/** Peripheral PMC base address */
#define PMC_BASE                                 (0x4007D000u)
/** Peripheral PMC base pointer */
#define PMC                                      ((PMC_Type *)PMC_BASE)
#define PMC_BASE_PTR                             (PMC)
/** Array initializer of PMC peripheral base addresses */
#define PMC_BASE_ADDRS                           { PMC_BASE }
/** Array initializer of PMC peripheral base pointers */
#define PMC_BASE_PTRS                            { PMC }

/* ----------------------------------------------------------------------------
   -- PMC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMC_Register_Accessor_Macros PMC - Register accessor macros
 * @{
 */


/* PMC - Register instance definitions */
/* PMC */
#define PMC_SPMSC1                               PMC_SPMSC1_REG(PMC)
#define PMC_SPMSC2                               PMC_SPMSC2_REG(PMC)

/*!
 * @}
 */ /* end of group PMC_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group PMC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PORT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Peripheral_Access_Layer PORT Peripheral Access Layer
 * @{
 */

/** PORT - Register Layout Typedef */
typedef struct {
  __IO uint32_t IOFLT0;                            /**< Port Filter Register 0, offset: 0x0 */
  __IO uint32_t IOFLT1;                            /**< Port Filter Register 1, offset: 0x4 */
  __IO uint32_t PUE0;                              /**< Port Pullup Enable Register 0, offset: 0x8 */
  __IO uint32_t PUE1;                              /**< Port Pullup Enable Register 1, offset: 0xC */
  __IO uint32_t PUE2;                              /**< Port Pullup Enable Register 2, offset: 0x10 */
  __IO uint32_t HDRVE;                             /**< Port High Drive Enable Register, offset: 0x14 */
} PORT_Type, *PORT_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- PORT - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Accessor_Macros PORT - Register accessor macros
 * @{
 */


/* PORT - Register accessors */
#define PORT_IOFLT0_REG(base)                    ((base)->IOFLT0)
#define PORT_IOFLT1_REG(base)                    ((base)->IOFLT1)
#define PORT_PUE0_REG(base)                      ((base)->PUE0)
#define PORT_PUE1_REG(base)                      ((base)->PUE1)
#define PORT_PUE2_REG(base)                      ((base)->PUE2)
#define PORT_HDRVE_REG(base)                     ((base)->HDRVE)

/*!
 * @}
 */ /* end of group PORT_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- PORT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Masks PORT Register Masks
 * @{
 */

/* IOFLT0 Bit Fields */
#define PORT_IOFLT0_FLTA_MASK                    0x3u
#define PORT_IOFLT0_FLTA_SHIFT                   0
#define PORT_IOFLT0_FLTA_WIDTH                   2
#define PORT_IOFLT0_FLTA(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTA_SHIFT))&PORT_IOFLT0_FLTA_MASK)
#define PORT_IOFLT0_FLTB_MASK                    0xCu
#define PORT_IOFLT0_FLTB_SHIFT                   2
#define PORT_IOFLT0_FLTB_WIDTH                   2
#define PORT_IOFLT0_FLTB(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTB_SHIFT))&PORT_IOFLT0_FLTB_MASK)
#define PORT_IOFLT0_FLTC_MASK                    0x30u
#define PORT_IOFLT0_FLTC_SHIFT                   4
#define PORT_IOFLT0_FLTC_WIDTH                   2
#define PORT_IOFLT0_FLTC(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTC_SHIFT))&PORT_IOFLT0_FLTC_MASK)
#define PORT_IOFLT0_FLTD_MASK                    0xC0u
#define PORT_IOFLT0_FLTD_SHIFT                   6
#define PORT_IOFLT0_FLTD_WIDTH                   2
#define PORT_IOFLT0_FLTD(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTD_SHIFT))&PORT_IOFLT0_FLTD_MASK)
#define PORT_IOFLT0_FLTE_MASK                    0x300u
#define PORT_IOFLT0_FLTE_SHIFT                   8
#define PORT_IOFLT0_FLTE_WIDTH                   2
#define PORT_IOFLT0_FLTE(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTE_SHIFT))&PORT_IOFLT0_FLTE_MASK)
#define PORT_IOFLT0_FLTF_MASK                    0xC00u
#define PORT_IOFLT0_FLTF_SHIFT                   10
#define PORT_IOFLT0_FLTF_WIDTH                   2
#define PORT_IOFLT0_FLTF(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTF_SHIFT))&PORT_IOFLT0_FLTF_MASK)
#define PORT_IOFLT0_FLTG_MASK                    0x3000u
#define PORT_IOFLT0_FLTG_SHIFT                   12
#define PORT_IOFLT0_FLTG_WIDTH                   2
#define PORT_IOFLT0_FLTG(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTG_SHIFT))&PORT_IOFLT0_FLTG_MASK)
#define PORT_IOFLT0_FLTH_MASK                    0xC000u
#define PORT_IOFLT0_FLTH_SHIFT                   14
#define PORT_IOFLT0_FLTH_WIDTH                   2
#define PORT_IOFLT0_FLTH(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTH_SHIFT))&PORT_IOFLT0_FLTH_MASK)
#define PORT_IOFLT0_FLTRST_MASK                  0x30000u
#define PORT_IOFLT0_FLTRST_SHIFT                 16
#define PORT_IOFLT0_FLTRST_WIDTH                 2
#define PORT_IOFLT0_FLTRST(x)                    (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTRST_SHIFT))&PORT_IOFLT0_FLTRST_MASK)
#define PORT_IOFLT0_FLTKBI0_MASK                 0xC0000u
#define PORT_IOFLT0_FLTKBI0_SHIFT                18
#define PORT_IOFLT0_FLTKBI0_WIDTH                2
#define PORT_IOFLT0_FLTKBI0(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTKBI0_SHIFT))&PORT_IOFLT0_FLTKBI0_MASK)
#define PORT_IOFLT0_FLTKBI1_MASK                 0x300000u
#define PORT_IOFLT0_FLTKBI1_SHIFT                20
#define PORT_IOFLT0_FLTKBI1_WIDTH                2
#define PORT_IOFLT0_FLTKBI1(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTKBI1_SHIFT))&PORT_IOFLT0_FLTKBI1_MASK)
#define PORT_IOFLT0_FLTNMI_MASK                  0xC00000u
#define PORT_IOFLT0_FLTNMI_SHIFT                 22
#define PORT_IOFLT0_FLTNMI_WIDTH                 2
#define PORT_IOFLT0_FLTNMI(x)                    (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTNMI_SHIFT))&PORT_IOFLT0_FLTNMI_MASK)
#define PORT_IOFLT0_FLTDIV1_MASK                 0x3000000u
#define PORT_IOFLT0_FLTDIV1_SHIFT                24
#define PORT_IOFLT0_FLTDIV1_WIDTH                2
#define PORT_IOFLT0_FLTDIV1(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTDIV1_SHIFT))&PORT_IOFLT0_FLTDIV1_MASK)
#define PORT_IOFLT0_FLTDIV2_MASK                 0x1C000000u
#define PORT_IOFLT0_FLTDIV2_SHIFT                26
#define PORT_IOFLT0_FLTDIV2_WIDTH                3
#define PORT_IOFLT0_FLTDIV2(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTDIV2_SHIFT))&PORT_IOFLT0_FLTDIV2_MASK)
#define PORT_IOFLT0_FLTDIV3_MASK                 0xE0000000u
#define PORT_IOFLT0_FLTDIV3_SHIFT                29
#define PORT_IOFLT0_FLTDIV3_WIDTH                3
#define PORT_IOFLT0_FLTDIV3(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT0_FLTDIV3_SHIFT))&PORT_IOFLT0_FLTDIV3_MASK)
/* IOFLT1 Bit Fields */
#define PORT_IOFLT1_FLTI_MASK                    0x3u
#define PORT_IOFLT1_FLTI_SHIFT                   0
#define PORT_IOFLT1_FLTI_WIDTH                   2
#define PORT_IOFLT1_FLTI(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTI_SHIFT))&PORT_IOFLT1_FLTI_MASK)
#define PORT_IOFLT1_FLTIRQ_MASK                  0x30u
#define PORT_IOFLT1_FLTIRQ_SHIFT                 4
#define PORT_IOFLT1_FLTIRQ_WIDTH                 2
#define PORT_IOFLT1_FLTIRQ(x)                    (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTIRQ_SHIFT))&PORT_IOFLT1_FLTIRQ_MASK)
#define PORT_IOFLT1_FLTFTM0_MASK                 0xC0u
#define PORT_IOFLT1_FLTFTM0_SHIFT                6
#define PORT_IOFLT1_FLTFTM0_WIDTH                2
#define PORT_IOFLT1_FLTFTM0(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTFTM0_SHIFT))&PORT_IOFLT1_FLTFTM0_MASK)
#define PORT_IOFLT1_FLTFTM1_MASK                 0x300u
#define PORT_IOFLT1_FLTFTM1_SHIFT                8
#define PORT_IOFLT1_FLTFTM1_WIDTH                2
#define PORT_IOFLT1_FLTFTM1(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTFTM1_SHIFT))&PORT_IOFLT1_FLTFTM1_MASK)
#define PORT_IOFLT1_FLTPWT_MASK                  0xC00u
#define PORT_IOFLT1_FLTPWT_SHIFT                 10
#define PORT_IOFLT1_FLTPWT_WIDTH                 2
#define PORT_IOFLT1_FLTPWT(x)                    (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTPWT_SHIFT))&PORT_IOFLT1_FLTPWT_MASK)
#define PORT_IOFLT1_FLTI2C0_MASK                 0x3000u
#define PORT_IOFLT1_FLTI2C0_SHIFT                12
#define PORT_IOFLT1_FLTI2C0_WIDTH                2
#define PORT_IOFLT1_FLTI2C0(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTI2C0_SHIFT))&PORT_IOFLT1_FLTI2C0_MASK)
#define PORT_IOFLT1_FLTI2C1_MASK                 0xC000u
#define PORT_IOFLT1_FLTI2C1_SHIFT                14
#define PORT_IOFLT1_FLTI2C1_WIDTH                2
#define PORT_IOFLT1_FLTI2C1(x)                   (((uint32_t)(((uint32_t)(x))<<PORT_IOFLT1_FLTI2C1_SHIFT))&PORT_IOFLT1_FLTI2C1_MASK)
/* PUE0 Bit Fields */
#define PORT_PUE0_PTAPE0_MASK                    0x1u
#define PORT_PUE0_PTAPE0_SHIFT                   0
#define PORT_PUE0_PTAPE0_WIDTH                   1
#define PORT_PUE0_PTAPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE0_SHIFT))&PORT_PUE0_PTAPE0_MASK)
#define PORT_PUE0_PTAPE1_MASK                    0x2u
#define PORT_PUE0_PTAPE1_SHIFT                   1
#define PORT_PUE0_PTAPE1_WIDTH                   1
#define PORT_PUE0_PTAPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE1_SHIFT))&PORT_PUE0_PTAPE1_MASK)
#define PORT_PUE0_PTAPE2_MASK                    0x4u
#define PORT_PUE0_PTAPE2_SHIFT                   2
#define PORT_PUE0_PTAPE2_WIDTH                   1
#define PORT_PUE0_PTAPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE2_SHIFT))&PORT_PUE0_PTAPE2_MASK)
#define PORT_PUE0_PTAPE3_MASK                    0x8u
#define PORT_PUE0_PTAPE3_SHIFT                   3
#define PORT_PUE0_PTAPE3_WIDTH                   1
#define PORT_PUE0_PTAPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE3_SHIFT))&PORT_PUE0_PTAPE3_MASK)
#define PORT_PUE0_PTAPE4_MASK                    0x10u
#define PORT_PUE0_PTAPE4_SHIFT                   4
#define PORT_PUE0_PTAPE4_WIDTH                   1
#define PORT_PUE0_PTAPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE4_SHIFT))&PORT_PUE0_PTAPE4_MASK)
#define PORT_PUE0_PTAPE5_MASK                    0x20u
#define PORT_PUE0_PTAPE5_SHIFT                   5
#define PORT_PUE0_PTAPE5_WIDTH                   1
#define PORT_PUE0_PTAPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE5_SHIFT))&PORT_PUE0_PTAPE5_MASK)
#define PORT_PUE0_PTAPE6_MASK                    0x40u
#define PORT_PUE0_PTAPE6_SHIFT                   6
#define PORT_PUE0_PTAPE6_WIDTH                   1
#define PORT_PUE0_PTAPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE6_SHIFT))&PORT_PUE0_PTAPE6_MASK)
#define PORT_PUE0_PTAPE7_MASK                    0x80u
#define PORT_PUE0_PTAPE7_SHIFT                   7
#define PORT_PUE0_PTAPE7_WIDTH                   1
#define PORT_PUE0_PTAPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTAPE7_SHIFT))&PORT_PUE0_PTAPE7_MASK)
#define PORT_PUE0_PTBPE0_MASK                    0x100u
#define PORT_PUE0_PTBPE0_SHIFT                   8
#define PORT_PUE0_PTBPE0_WIDTH                   1
#define PORT_PUE0_PTBPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE0_SHIFT))&PORT_PUE0_PTBPE0_MASK)
#define PORT_PUE0_PTBPE1_MASK                    0x200u
#define PORT_PUE0_PTBPE1_SHIFT                   9
#define PORT_PUE0_PTBPE1_WIDTH                   1
#define PORT_PUE0_PTBPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE1_SHIFT))&PORT_PUE0_PTBPE1_MASK)
#define PORT_PUE0_PTBPE2_MASK                    0x400u
#define PORT_PUE0_PTBPE2_SHIFT                   10
#define PORT_PUE0_PTBPE2_WIDTH                   1
#define PORT_PUE0_PTBPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE2_SHIFT))&PORT_PUE0_PTBPE2_MASK)
#define PORT_PUE0_PTBPE3_MASK                    0x800u
#define PORT_PUE0_PTBPE3_SHIFT                   11
#define PORT_PUE0_PTBPE3_WIDTH                   1
#define PORT_PUE0_PTBPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE3_SHIFT))&PORT_PUE0_PTBPE3_MASK)
#define PORT_PUE0_PTBPE4_MASK                    0x1000u
#define PORT_PUE0_PTBPE4_SHIFT                   12
#define PORT_PUE0_PTBPE4_WIDTH                   1
#define PORT_PUE0_PTBPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE4_SHIFT))&PORT_PUE0_PTBPE4_MASK)
#define PORT_PUE0_PTBPE5_MASK                    0x2000u
#define PORT_PUE0_PTBPE5_SHIFT                   13
#define PORT_PUE0_PTBPE5_WIDTH                   1
#define PORT_PUE0_PTBPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE5_SHIFT))&PORT_PUE0_PTBPE5_MASK)
#define PORT_PUE0_PTBPE6_MASK                    0x4000u
#define PORT_PUE0_PTBPE6_SHIFT                   14
#define PORT_PUE0_PTBPE6_WIDTH                   1
#define PORT_PUE0_PTBPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE6_SHIFT))&PORT_PUE0_PTBPE6_MASK)
#define PORT_PUE0_PTBPE7_MASK                    0x8000u
#define PORT_PUE0_PTBPE7_SHIFT                   15
#define PORT_PUE0_PTBPE7_WIDTH                   1
#define PORT_PUE0_PTBPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTBPE7_SHIFT))&PORT_PUE0_PTBPE7_MASK)
#define PORT_PUE0_PTCPE0_MASK                    0x10000u
#define PORT_PUE0_PTCPE0_SHIFT                   16
#define PORT_PUE0_PTCPE0_WIDTH                   1
#define PORT_PUE0_PTCPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE0_SHIFT))&PORT_PUE0_PTCPE0_MASK)
#define PORT_PUE0_PTCPE1_MASK                    0x20000u
#define PORT_PUE0_PTCPE1_SHIFT                   17
#define PORT_PUE0_PTCPE1_WIDTH                   1
#define PORT_PUE0_PTCPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE1_SHIFT))&PORT_PUE0_PTCPE1_MASK)
#define PORT_PUE0_PTCPE2_MASK                    0x40000u
#define PORT_PUE0_PTCPE2_SHIFT                   18
#define PORT_PUE0_PTCPE2_WIDTH                   1
#define PORT_PUE0_PTCPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE2_SHIFT))&PORT_PUE0_PTCPE2_MASK)
#define PORT_PUE0_PTCPE3_MASK                    0x80000u
#define PORT_PUE0_PTCPE3_SHIFT                   19
#define PORT_PUE0_PTCPE3_WIDTH                   1
#define PORT_PUE0_PTCPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE3_SHIFT))&PORT_PUE0_PTCPE3_MASK)
#define PORT_PUE0_PTCPE4_MASK                    0x100000u
#define PORT_PUE0_PTCPE4_SHIFT                   20
#define PORT_PUE0_PTCPE4_WIDTH                   1
#define PORT_PUE0_PTCPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE4_SHIFT))&PORT_PUE0_PTCPE4_MASK)
#define PORT_PUE0_PTCPE5_MASK                    0x200000u
#define PORT_PUE0_PTCPE5_SHIFT                   21
#define PORT_PUE0_PTCPE5_WIDTH                   1
#define PORT_PUE0_PTCPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE5_SHIFT))&PORT_PUE0_PTCPE5_MASK)
#define PORT_PUE0_PTCPE6_MASK                    0x400000u
#define PORT_PUE0_PTCPE6_SHIFT                   22
#define PORT_PUE0_PTCPE6_WIDTH                   1
#define PORT_PUE0_PTCPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE6_SHIFT))&PORT_PUE0_PTCPE6_MASK)
#define PORT_PUE0_PTCPE7_MASK                    0x800000u
#define PORT_PUE0_PTCPE7_SHIFT                   23
#define PORT_PUE0_PTCPE7_WIDTH                   1
#define PORT_PUE0_PTCPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTCPE7_SHIFT))&PORT_PUE0_PTCPE7_MASK)
#define PORT_PUE0_PTDPE0_MASK                    0x1000000u
#define PORT_PUE0_PTDPE0_SHIFT                   24
#define PORT_PUE0_PTDPE0_WIDTH                   1
#define PORT_PUE0_PTDPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE0_SHIFT))&PORT_PUE0_PTDPE0_MASK)
#define PORT_PUE0_PTDPE1_MASK                    0x2000000u
#define PORT_PUE0_PTDPE1_SHIFT                   25
#define PORT_PUE0_PTDPE1_WIDTH                   1
#define PORT_PUE0_PTDPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE1_SHIFT))&PORT_PUE0_PTDPE1_MASK)
#define PORT_PUE0_PTDPE2_MASK                    0x4000000u
#define PORT_PUE0_PTDPE2_SHIFT                   26
#define PORT_PUE0_PTDPE2_WIDTH                   1
#define PORT_PUE0_PTDPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE2_SHIFT))&PORT_PUE0_PTDPE2_MASK)
#define PORT_PUE0_PTDPE3_MASK                    0x8000000u
#define PORT_PUE0_PTDPE3_SHIFT                   27
#define PORT_PUE0_PTDPE3_WIDTH                   1
#define PORT_PUE0_PTDPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE3_SHIFT))&PORT_PUE0_PTDPE3_MASK)
#define PORT_PUE0_PTDPE4_MASK                    0x10000000u
#define PORT_PUE0_PTDPE4_SHIFT                   28
#define PORT_PUE0_PTDPE4_WIDTH                   1
#define PORT_PUE0_PTDPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE4_SHIFT))&PORT_PUE0_PTDPE4_MASK)
#define PORT_PUE0_PTDPE5_MASK                    0x20000000u
#define PORT_PUE0_PTDPE5_SHIFT                   29
#define PORT_PUE0_PTDPE5_WIDTH                   1
#define PORT_PUE0_PTDPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE5_SHIFT))&PORT_PUE0_PTDPE5_MASK)
#define PORT_PUE0_PTDPE6_MASK                    0x40000000u
#define PORT_PUE0_PTDPE6_SHIFT                   30
#define PORT_PUE0_PTDPE6_WIDTH                   1
#define PORT_PUE0_PTDPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE6_SHIFT))&PORT_PUE0_PTDPE6_MASK)
#define PORT_PUE0_PTDPE7_MASK                    0x80000000u
#define PORT_PUE0_PTDPE7_SHIFT                   31
#define PORT_PUE0_PTDPE7_WIDTH                   1
#define PORT_PUE0_PTDPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE0_PTDPE7_SHIFT))&PORT_PUE0_PTDPE7_MASK)
/* PUE1 Bit Fields */
#define PORT_PUE1_PTEPE0_MASK                    0x1u
#define PORT_PUE1_PTEPE0_SHIFT                   0
#define PORT_PUE1_PTEPE0_WIDTH                   1
#define PORT_PUE1_PTEPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE0_SHIFT))&PORT_PUE1_PTEPE0_MASK)
#define PORT_PUE1_PTEPE1_MASK                    0x2u
#define PORT_PUE1_PTEPE1_SHIFT                   1
#define PORT_PUE1_PTEPE1_WIDTH                   1
#define PORT_PUE1_PTEPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE1_SHIFT))&PORT_PUE1_PTEPE1_MASK)
#define PORT_PUE1_PTEPE2_MASK                    0x4u
#define PORT_PUE1_PTEPE2_SHIFT                   2
#define PORT_PUE1_PTEPE2_WIDTH                   1
#define PORT_PUE1_PTEPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE2_SHIFT))&PORT_PUE1_PTEPE2_MASK)
#define PORT_PUE1_PTEPE3_MASK                    0x8u
#define PORT_PUE1_PTEPE3_SHIFT                   3
#define PORT_PUE1_PTEPE3_WIDTH                   1
#define PORT_PUE1_PTEPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE3_SHIFT))&PORT_PUE1_PTEPE3_MASK)
#define PORT_PUE1_PTEPE4_MASK                    0x10u
#define PORT_PUE1_PTEPE4_SHIFT                   4
#define PORT_PUE1_PTEPE4_WIDTH                   1
#define PORT_PUE1_PTEPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE4_SHIFT))&PORT_PUE1_PTEPE4_MASK)
#define PORT_PUE1_PTEPE5_MASK                    0x20u
#define PORT_PUE1_PTEPE5_SHIFT                   5
#define PORT_PUE1_PTEPE5_WIDTH                   1
#define PORT_PUE1_PTEPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE5_SHIFT))&PORT_PUE1_PTEPE5_MASK)
#define PORT_PUE1_PTEPE6_MASK                    0x40u
#define PORT_PUE1_PTEPE6_SHIFT                   6
#define PORT_PUE1_PTEPE6_WIDTH                   1
#define PORT_PUE1_PTEPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE6_SHIFT))&PORT_PUE1_PTEPE6_MASK)
#define PORT_PUE1_PTEPE7_MASK                    0x80u
#define PORT_PUE1_PTEPE7_SHIFT                   7
#define PORT_PUE1_PTEPE7_WIDTH                   1
#define PORT_PUE1_PTEPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTEPE7_SHIFT))&PORT_PUE1_PTEPE7_MASK)
#define PORT_PUE1_PTFPE0_MASK                    0x100u
#define PORT_PUE1_PTFPE0_SHIFT                   8
#define PORT_PUE1_PTFPE0_WIDTH                   1
#define PORT_PUE1_PTFPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE0_SHIFT))&PORT_PUE1_PTFPE0_MASK)
#define PORT_PUE1_PTFPE1_MASK                    0x200u
#define PORT_PUE1_PTFPE1_SHIFT                   9
#define PORT_PUE1_PTFPE1_WIDTH                   1
#define PORT_PUE1_PTFPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE1_SHIFT))&PORT_PUE1_PTFPE1_MASK)
#define PORT_PUE1_PTFPE2_MASK                    0x400u
#define PORT_PUE1_PTFPE2_SHIFT                   10
#define PORT_PUE1_PTFPE2_WIDTH                   1
#define PORT_PUE1_PTFPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE2_SHIFT))&PORT_PUE1_PTFPE2_MASK)
#define PORT_PUE1_PTFPE3_MASK                    0x800u
#define PORT_PUE1_PTFPE3_SHIFT                   11
#define PORT_PUE1_PTFPE3_WIDTH                   1
#define PORT_PUE1_PTFPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE3_SHIFT))&PORT_PUE1_PTFPE3_MASK)
#define PORT_PUE1_PTFPE4_MASK                    0x1000u
#define PORT_PUE1_PTFPE4_SHIFT                   12
#define PORT_PUE1_PTFPE4_WIDTH                   1
#define PORT_PUE1_PTFPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE4_SHIFT))&PORT_PUE1_PTFPE4_MASK)
#define PORT_PUE1_PTFPE5_MASK                    0x2000u
#define PORT_PUE1_PTFPE5_SHIFT                   13
#define PORT_PUE1_PTFPE5_WIDTH                   1
#define PORT_PUE1_PTFPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE5_SHIFT))&PORT_PUE1_PTFPE5_MASK)
#define PORT_PUE1_PTFPE6_MASK                    0x4000u
#define PORT_PUE1_PTFPE6_SHIFT                   14
#define PORT_PUE1_PTFPE6_WIDTH                   1
#define PORT_PUE1_PTFPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE6_SHIFT))&PORT_PUE1_PTFPE6_MASK)
#define PORT_PUE1_PTFPE7_MASK                    0x8000u
#define PORT_PUE1_PTFPE7_SHIFT                   15
#define PORT_PUE1_PTFPE7_WIDTH                   1
#define PORT_PUE1_PTFPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTFPE7_SHIFT))&PORT_PUE1_PTFPE7_MASK)
#define PORT_PUE1_PTGPE0_MASK                    0x10000u
#define PORT_PUE1_PTGPE0_SHIFT                   16
#define PORT_PUE1_PTGPE0_WIDTH                   1
#define PORT_PUE1_PTGPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE0_SHIFT))&PORT_PUE1_PTGPE0_MASK)
#define PORT_PUE1_PTGPE1_MASK                    0x20000u
#define PORT_PUE1_PTGPE1_SHIFT                   17
#define PORT_PUE1_PTGPE1_WIDTH                   1
#define PORT_PUE1_PTGPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE1_SHIFT))&PORT_PUE1_PTGPE1_MASK)
#define PORT_PUE1_PTGPE2_MASK                    0x40000u
#define PORT_PUE1_PTGPE2_SHIFT                   18
#define PORT_PUE1_PTGPE2_WIDTH                   1
#define PORT_PUE1_PTGPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE2_SHIFT))&PORT_PUE1_PTGPE2_MASK)
#define PORT_PUE1_PTGPE3_MASK                    0x80000u
#define PORT_PUE1_PTGPE3_SHIFT                   19
#define PORT_PUE1_PTGPE3_WIDTH                   1
#define PORT_PUE1_PTGPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE3_SHIFT))&PORT_PUE1_PTGPE3_MASK)
#define PORT_PUE1_PTGPE4_MASK                    0x100000u
#define PORT_PUE1_PTGPE4_SHIFT                   20
#define PORT_PUE1_PTGPE4_WIDTH                   1
#define PORT_PUE1_PTGPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE4_SHIFT))&PORT_PUE1_PTGPE4_MASK)
#define PORT_PUE1_PTGPE5_MASK                    0x200000u
#define PORT_PUE1_PTGPE5_SHIFT                   21
#define PORT_PUE1_PTGPE5_WIDTH                   1
#define PORT_PUE1_PTGPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE5_SHIFT))&PORT_PUE1_PTGPE5_MASK)
#define PORT_PUE1_PTGPE6_MASK                    0x400000u
#define PORT_PUE1_PTGPE6_SHIFT                   22
#define PORT_PUE1_PTGPE6_WIDTH                   1
#define PORT_PUE1_PTGPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE6_SHIFT))&PORT_PUE1_PTGPE6_MASK)
#define PORT_PUE1_PTGPE7_MASK                    0x800000u
#define PORT_PUE1_PTGPE7_SHIFT                   23
#define PORT_PUE1_PTGPE7_WIDTH                   1
#define PORT_PUE1_PTGPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTGPE7_SHIFT))&PORT_PUE1_PTGPE7_MASK)
#define PORT_PUE1_PTHPE0_MASK                    0x1000000u
#define PORT_PUE1_PTHPE0_SHIFT                   24
#define PORT_PUE1_PTHPE0_WIDTH                   1
#define PORT_PUE1_PTHPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE0_SHIFT))&PORT_PUE1_PTHPE0_MASK)
#define PORT_PUE1_PTHPE1_MASK                    0x2000000u
#define PORT_PUE1_PTHPE1_SHIFT                   25
#define PORT_PUE1_PTHPE1_WIDTH                   1
#define PORT_PUE1_PTHPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE1_SHIFT))&PORT_PUE1_PTHPE1_MASK)
#define PORT_PUE1_PTHPE2_MASK                    0x4000000u
#define PORT_PUE1_PTHPE2_SHIFT                   26
#define PORT_PUE1_PTHPE2_WIDTH                   1
#define PORT_PUE1_PTHPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE2_SHIFT))&PORT_PUE1_PTHPE2_MASK)
#define PORT_PUE1_PTHPE3_MASK                    0x8000000u
#define PORT_PUE1_PTHPE3_SHIFT                   27
#define PORT_PUE1_PTHPE3_WIDTH                   1
#define PORT_PUE1_PTHPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE3_SHIFT))&PORT_PUE1_PTHPE3_MASK)
#define PORT_PUE1_PTHPE4_MASK                    0x10000000u
#define PORT_PUE1_PTHPE4_SHIFT                   28
#define PORT_PUE1_PTHPE4_WIDTH                   1
#define PORT_PUE1_PTHPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE4_SHIFT))&PORT_PUE1_PTHPE4_MASK)
#define PORT_PUE1_PTHPE5_MASK                    0x20000000u
#define PORT_PUE1_PTHPE5_SHIFT                   29
#define PORT_PUE1_PTHPE5_WIDTH                   1
#define PORT_PUE1_PTHPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE5_SHIFT))&PORT_PUE1_PTHPE5_MASK)
#define PORT_PUE1_PTHPE6_MASK                    0x40000000u
#define PORT_PUE1_PTHPE6_SHIFT                   30
#define PORT_PUE1_PTHPE6_WIDTH                   1
#define PORT_PUE1_PTHPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE6_SHIFT))&PORT_PUE1_PTHPE6_MASK)
#define PORT_PUE1_PTHPE7_MASK                    0x80000000u
#define PORT_PUE1_PTHPE7_SHIFT                   31
#define PORT_PUE1_PTHPE7_WIDTH                   1
#define PORT_PUE1_PTHPE7(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE1_PTHPE7_SHIFT))&PORT_PUE1_PTHPE7_MASK)
/* PUE2 Bit Fields */
#define PORT_PUE2_PTIPE0_MASK                    0x1u
#define PORT_PUE2_PTIPE0_SHIFT                   0
#define PORT_PUE2_PTIPE0_WIDTH                   1
#define PORT_PUE2_PTIPE0(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE0_SHIFT))&PORT_PUE2_PTIPE0_MASK)
#define PORT_PUE2_PTIPE1_MASK                    0x2u
#define PORT_PUE2_PTIPE1_SHIFT                   1
#define PORT_PUE2_PTIPE1_WIDTH                   1
#define PORT_PUE2_PTIPE1(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE1_SHIFT))&PORT_PUE2_PTIPE1_MASK)
#define PORT_PUE2_PTIPE2_MASK                    0x4u
#define PORT_PUE2_PTIPE2_SHIFT                   2
#define PORT_PUE2_PTIPE2_WIDTH                   1
#define PORT_PUE2_PTIPE2(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE2_SHIFT))&PORT_PUE2_PTIPE2_MASK)
#define PORT_PUE2_PTIPE3_MASK                    0x8u
#define PORT_PUE2_PTIPE3_SHIFT                   3
#define PORT_PUE2_PTIPE3_WIDTH                   1
#define PORT_PUE2_PTIPE3(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE3_SHIFT))&PORT_PUE2_PTIPE3_MASK)
#define PORT_PUE2_PTIPE4_MASK                    0x10u
#define PORT_PUE2_PTIPE4_SHIFT                   4
#define PORT_PUE2_PTIPE4_WIDTH                   1
#define PORT_PUE2_PTIPE4(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE4_SHIFT))&PORT_PUE2_PTIPE4_MASK)
#define PORT_PUE2_PTIPE5_MASK                    0x20u
#define PORT_PUE2_PTIPE5_SHIFT                   5
#define PORT_PUE2_PTIPE5_WIDTH                   1
#define PORT_PUE2_PTIPE5(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE5_SHIFT))&PORT_PUE2_PTIPE5_MASK)
#define PORT_PUE2_PTIPE6_MASK                    0x40u
#define PORT_PUE2_PTIPE6_SHIFT                   6
#define PORT_PUE2_PTIPE6_WIDTH                   1
#define PORT_PUE2_PTIPE6(x)                      (((uint32_t)(((uint32_t)(x))<<PORT_PUE2_PTIPE6_SHIFT))&PORT_PUE2_PTIPE6_MASK)
/* HDRVE Bit Fields */
#define PORT_HDRVE_PTB4_MASK                     0x1u
#define PORT_HDRVE_PTB4_SHIFT                    0
#define PORT_HDRVE_PTB4_WIDTH                    1
#define PORT_HDRVE_PTB4(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTB4_SHIFT))&PORT_HDRVE_PTB4_MASK)
#define PORT_HDRVE_PTB5_MASK                     0x2u
#define PORT_HDRVE_PTB5_SHIFT                    1
#define PORT_HDRVE_PTB5_WIDTH                    1
#define PORT_HDRVE_PTB5(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTB5_SHIFT))&PORT_HDRVE_PTB5_MASK)
#define PORT_HDRVE_PTD0_MASK                     0x4u
#define PORT_HDRVE_PTD0_SHIFT                    2
#define PORT_HDRVE_PTD0_WIDTH                    1
#define PORT_HDRVE_PTD0(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTD0_SHIFT))&PORT_HDRVE_PTD0_MASK)
#define PORT_HDRVE_PTD1_MASK                     0x8u
#define PORT_HDRVE_PTD1_SHIFT                    3
#define PORT_HDRVE_PTD1_WIDTH                    1
#define PORT_HDRVE_PTD1(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTD1_SHIFT))&PORT_HDRVE_PTD1_MASK)
#define PORT_HDRVE_PTE0_MASK                     0x10u
#define PORT_HDRVE_PTE0_SHIFT                    4
#define PORT_HDRVE_PTE0_WIDTH                    1
#define PORT_HDRVE_PTE0(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTE0_SHIFT))&PORT_HDRVE_PTE0_MASK)
#define PORT_HDRVE_PTE1_MASK                     0x20u
#define PORT_HDRVE_PTE1_SHIFT                    5
#define PORT_HDRVE_PTE1_WIDTH                    1
#define PORT_HDRVE_PTE1(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTE1_SHIFT))&PORT_HDRVE_PTE1_MASK)
#define PORT_HDRVE_PTH0_MASK                     0x40u
#define PORT_HDRVE_PTH0_SHIFT                    6
#define PORT_HDRVE_PTH0_WIDTH                    1
#define PORT_HDRVE_PTH0(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTH0_SHIFT))&PORT_HDRVE_PTH0_MASK)
#define PORT_HDRVE_PTH1_MASK                     0x80u
#define PORT_HDRVE_PTH1_SHIFT                    7
#define PORT_HDRVE_PTH1_WIDTH                    1
#define PORT_HDRVE_PTH1(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_HDRVE_PTH1_SHIFT))&PORT_HDRVE_PTH1_MASK)

/*!
 * @}
 */ /* end of group PORT_Register_Masks */


/* PORT - Peripheral instance base addresses */
/** Peripheral PORT base address */
#define PORT_BASE                                (0x40049000u)
/** Peripheral PORT base pointer */
#define PORT                                     ((PORT_Type *)PORT_BASE)
#define PORT_BASE_PTR                            (PORT)
/** Array initializer of PORT peripheral base addresses */
#define PORT_BASE_ADDRS                          { PORT_BASE }
/** Array initializer of PORT peripheral base pointers */
#define PORT_BASE_PTRS                           { PORT }
/** Array initializer of PORT interrupt numbers */
#define PORT_IRQS                                {}

/* ----------------------------------------------------------------------------
   -- PORT - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Accessor_Macros PORT - Register accessor macros
 * @{
 */


/* PORT - Register instance definitions */
/* PORT */
#define PORT_IOFLT0                              PORT_IOFLT0_REG(PORT)
#define PORT_IOFLT1                              PORT_IOFLT1_REG(PORT)
#define PORT_PUE0                                PORT_PUE0_REG(PORT)
#define PORT_PUE1                                PORT_PUE1_REG(PORT)
#define PORT_PUE2                                PORT_PUE2_REG(PORT)
#define PORT_HDRVE                               PORT_HDRVE_REG(PORT)

/*!
 * @}
 */ /* end of group PORT_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group PORT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PWT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWT_Peripheral_Access_Layer PWT Peripheral Access Layer
 * @{
 */

/** PWT - Register Layout Typedef */
typedef struct {
  __IO uint32_t R1;                                /**< Pulse Width Timer Register 1, offset: 0x0 */
  __I  uint32_t R2;                                /**< Pulse Width Timer Register 2, offset: 0x4 */
} PWT_Type, *PWT_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- PWT - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWT_Register_Accessor_Macros PWT - Register accessor macros
 * @{
 */


/* PWT - Register accessors */
#define PWT_R1_REG(base)                         ((base)->R1)
#define PWT_R2_REG(base)                         ((base)->R2)

/*!
 * @}
 */ /* end of group PWT_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- PWT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWT_Register_Masks PWT Register Masks
 * @{
 */

/* R1 Bit Fields */
#define PWT_R1_PWTOV_MASK                        0x1u
#define PWT_R1_PWTOV_SHIFT                       0
#define PWT_R1_PWTOV_WIDTH                       1
#define PWT_R1_PWTOV(x)                          (((uint32_t)(((uint32_t)(x))<<PWT_R1_PWTOV_SHIFT))&PWT_R1_PWTOV_MASK)
#define PWT_R1_PWTRDY_MASK                       0x2u
#define PWT_R1_PWTRDY_SHIFT                      1
#define PWT_R1_PWTRDY_WIDTH                      1
#define PWT_R1_PWTRDY(x)                         (((uint32_t)(((uint32_t)(x))<<PWT_R1_PWTRDY_SHIFT))&PWT_R1_PWTRDY_MASK)
#define PWT_R1_PWTSR_MASK                        0x8u
#define PWT_R1_PWTSR_SHIFT                       3
#define PWT_R1_PWTSR_WIDTH                       1
#define PWT_R1_PWTSR(x)                          (((uint32_t)(((uint32_t)(x))<<PWT_R1_PWTSR_SHIFT))&PWT_R1_PWTSR_MASK)
#define PWT_R1_POVIE_MASK                        0x10u
#define PWT_R1_POVIE_SHIFT                       4
#define PWT_R1_POVIE_WIDTH                       1
#define PWT_R1_POVIE(x)                          (((uint32_t)(((uint32_t)(x))<<PWT_R1_POVIE_SHIFT))&PWT_R1_POVIE_MASK)
#define PWT_R1_PRDYIE_MASK                       0x20u
#define PWT_R1_PRDYIE_SHIFT                      5
#define PWT_R1_PRDYIE_WIDTH                      1
#define PWT_R1_PRDYIE(x)                         (((uint32_t)(((uint32_t)(x))<<PWT_R1_PRDYIE_SHIFT))&PWT_R1_PRDYIE_MASK)
#define PWT_R1_PWTIE_MASK                        0x40u
#define PWT_R1_PWTIE_SHIFT                       6
#define PWT_R1_PWTIE_WIDTH                       1
#define PWT_R1_PWTIE(x)                          (((uint32_t)(((uint32_t)(x))<<PWT_R1_PWTIE_SHIFT))&PWT_R1_PWTIE_MASK)
#define PWT_R1_PWTEN_MASK                        0x80u
#define PWT_R1_PWTEN_SHIFT                       7
#define PWT_R1_PWTEN_WIDTH                       1
#define PWT_R1_PWTEN(x)                          (((uint32_t)(((uint32_t)(x))<<PWT_R1_PWTEN_SHIFT))&PWT_R1_PWTEN_MASK)
#define PWT_R1_PRE_MASK                          0x700u
#define PWT_R1_PRE_SHIFT                         8
#define PWT_R1_PRE_WIDTH                         3
#define PWT_R1_PRE(x)                            (((uint32_t)(((uint32_t)(x))<<PWT_R1_PRE_SHIFT))&PWT_R1_PRE_MASK)
#define PWT_R1_EDGE_MASK                         0x1800u
#define PWT_R1_EDGE_SHIFT                        11
#define PWT_R1_EDGE_WIDTH                        2
#define PWT_R1_EDGE(x)                           (((uint32_t)(((uint32_t)(x))<<PWT_R1_EDGE_SHIFT))&PWT_R1_EDGE_MASK)
#define PWT_R1_PINSEL_MASK                       0x6000u
#define PWT_R1_PINSEL_SHIFT                      13
#define PWT_R1_PINSEL_WIDTH                      2
#define PWT_R1_PINSEL(x)                         (((uint32_t)(((uint32_t)(x))<<PWT_R1_PINSEL_SHIFT))&PWT_R1_PINSEL_MASK)
#define PWT_R1_PCLKS_MASK                        0x8000u
#define PWT_R1_PCLKS_SHIFT                       15
#define PWT_R1_PCLKS_WIDTH                       1
#define PWT_R1_PCLKS(x)                          (((uint32_t)(((uint32_t)(x))<<PWT_R1_PCLKS_SHIFT))&PWT_R1_PCLKS_MASK)
#define PWT_R1_PPW_MASK                          0xFFFF0000u
#define PWT_R1_PPW_SHIFT                         16
#define PWT_R1_PPW_WIDTH                         16
#define PWT_R1_PPW(x)                            (((uint32_t)(((uint32_t)(x))<<PWT_R1_PPW_SHIFT))&PWT_R1_PPW_MASK)
/* R2 Bit Fields */
#define PWT_R2_NPW_MASK                          0xFFFFu
#define PWT_R2_NPW_SHIFT                         0
#define PWT_R2_NPW_WIDTH                         16
#define PWT_R2_NPW(x)                            (((uint32_t)(((uint32_t)(x))<<PWT_R2_NPW_SHIFT))&PWT_R2_NPW_MASK)
#define PWT_R2_PWTC_MASK                         0xFFFF0000u
#define PWT_R2_PWTC_SHIFT                        16
#define PWT_R2_PWTC_WIDTH                        16
#define PWT_R2_PWTC(x)                           (((uint32_t)(((uint32_t)(x))<<PWT_R2_PWTC_SHIFT))&PWT_R2_PWTC_MASK)

/*!
 * @}
 */ /* end of group PWT_Register_Masks */


/* PWT - Peripheral instance base addresses */
/** Peripheral PWT base address */
#define PWT_BASE                                 (0x40033000u)
/** Peripheral PWT base pointer */
#define PWT                                      ((PWT_Type *)PWT_BASE)
#define PWT_BASE_PTR                             (PWT)
/** Array initializer of PWT peripheral base addresses */
#define PWT_BASE_ADDRS                           { PWT_BASE }
/** Array initializer of PWT peripheral base pointers */
#define PWT_BASE_PTRS                            { PWT }

/* ----------------------------------------------------------------------------
   -- PWT - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWT_Register_Accessor_Macros PWT - Register accessor macros
 * @{
 */


/* PWT - Register instance definitions */
/* PWT */
#define PWT_R1                                   PWT_R1_REG(PWT)
#define PWT_R2                                   PWT_R2_REG(PWT)

/*!
 * @}
 */ /* end of group PWT_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group PWT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- ROM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ROM_Peripheral_Access_Layer ROM Peripheral Access Layer
 * @{
 */

/** ROM - Register Layout Typedef */
typedef struct {
  __I  uint32_t ENTRY[1];                          /**< Entry, array offset: 0x0, array step: 0x4 */
  __I  uint32_t TABLEMARK;                         /**< End of Table Marker Register, offset: 0x4 */
       uint8_t RESERVED_0[4036];
  __I  uint32_t SYSACCESS;                         /**< System Access Register, offset: 0xFCC */
  __I  uint32_t PERIPHID4;                         /**< Peripheral ID Register, offset: 0xFD0 */
  __I  uint32_t PERIPHID5;                         /**< Peripheral ID Register, offset: 0xFD4 */
  __I  uint32_t PERIPHID6;                         /**< Peripheral ID Register, offset: 0xFD8 */
  __I  uint32_t PERIPHID7;                         /**< Peripheral ID Register, offset: 0xFDC */
  __I  uint32_t PERIPHID0;                         /**< Peripheral ID Register, offset: 0xFE0 */
  __I  uint32_t PERIPHID1;                         /**< Peripheral ID Register, offset: 0xFE4 */
  __I  uint32_t PERIPHID2;                         /**< Peripheral ID Register, offset: 0xFE8 */
  __I  uint32_t PERIPHID3;                         /**< Peripheral ID Register, offset: 0xFEC */
  __I  uint32_t COMPID[4];                         /**< Component ID Register, array offset: 0xFF0, array step: 0x4 */
} ROM_Type, *ROM_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- ROM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ROM_Register_Accessor_Macros ROM - Register accessor macros
 * @{
 */


/* ROM - Register accessors */
#define ROM_ENTRY_REG(base,index)                ((base)->ENTRY[index])
#define ROM_ENTRY_COUNT                          1
#define ROM_TABLEMARK_REG(base)                  ((base)->TABLEMARK)
#define ROM_SYSACCESS_REG(base)                  ((base)->SYSACCESS)
#define ROM_PERIPHID4_REG(base)                  ((base)->PERIPHID4)
#define ROM_PERIPHID5_REG(base)                  ((base)->PERIPHID5)
#define ROM_PERIPHID6_REG(base)                  ((base)->PERIPHID6)
#define ROM_PERIPHID7_REG(base)                  ((base)->PERIPHID7)
#define ROM_PERIPHID0_REG(base)                  ((base)->PERIPHID0)
#define ROM_PERIPHID1_REG(base)                  ((base)->PERIPHID1)
#define ROM_PERIPHID2_REG(base)                  ((base)->PERIPHID2)
#define ROM_PERIPHID3_REG(base)                  ((base)->PERIPHID3)
#define ROM_COMPID_REG(base,index)               ((base)->COMPID[index])
#define ROM_COMPID_COUNT                         4

/*!
 * @}
 */ /* end of group ROM_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- ROM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ROM_Register_Masks ROM Register Masks
 * @{
 */

/* ENTRY Bit Fields */
#define ROM_ENTRY_ENTRY_MASK                     0xFFFFFFFFu
#define ROM_ENTRY_ENTRY_SHIFT                    0
#define ROM_ENTRY_ENTRY_WIDTH                    32
#define ROM_ENTRY_ENTRY(x)                       (((uint32_t)(((uint32_t)(x))<<ROM_ENTRY_ENTRY_SHIFT))&ROM_ENTRY_ENTRY_MASK)
/* TABLEMARK Bit Fields */
#define ROM_TABLEMARK_MARK_MASK                  0xFFFFFFFFu
#define ROM_TABLEMARK_MARK_SHIFT                 0
#define ROM_TABLEMARK_MARK_WIDTH                 32
#define ROM_TABLEMARK_MARK(x)                    (((uint32_t)(((uint32_t)(x))<<ROM_TABLEMARK_MARK_SHIFT))&ROM_TABLEMARK_MARK_MASK)
/* SYSACCESS Bit Fields */
#define ROM_SYSACCESS_SYSACCESS_MASK             0xFFFFFFFFu
#define ROM_SYSACCESS_SYSACCESS_SHIFT            0
#define ROM_SYSACCESS_SYSACCESS_WIDTH            32
#define ROM_SYSACCESS_SYSACCESS(x)               (((uint32_t)(((uint32_t)(x))<<ROM_SYSACCESS_SYSACCESS_SHIFT))&ROM_SYSACCESS_SYSACCESS_MASK)
/* PERIPHID4 Bit Fields */
#define ROM_PERIPHID4_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID4_PERIPHID_SHIFT             0
#define ROM_PERIPHID4_PERIPHID_WIDTH             32
#define ROM_PERIPHID4_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID4_PERIPHID_SHIFT))&ROM_PERIPHID4_PERIPHID_MASK)
/* PERIPHID5 Bit Fields */
#define ROM_PERIPHID5_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID5_PERIPHID_SHIFT             0
#define ROM_PERIPHID5_PERIPHID_WIDTH             32
#define ROM_PERIPHID5_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID5_PERIPHID_SHIFT))&ROM_PERIPHID5_PERIPHID_MASK)
/* PERIPHID6 Bit Fields */
#define ROM_PERIPHID6_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID6_PERIPHID_SHIFT             0
#define ROM_PERIPHID6_PERIPHID_WIDTH             32
#define ROM_PERIPHID6_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID6_PERIPHID_SHIFT))&ROM_PERIPHID6_PERIPHID_MASK)
/* PERIPHID7 Bit Fields */
#define ROM_PERIPHID7_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID7_PERIPHID_SHIFT             0
#define ROM_PERIPHID7_PERIPHID_WIDTH             32
#define ROM_PERIPHID7_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID7_PERIPHID_SHIFT))&ROM_PERIPHID7_PERIPHID_MASK)
/* PERIPHID0 Bit Fields */
#define ROM_PERIPHID0_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID0_PERIPHID_SHIFT             0
#define ROM_PERIPHID0_PERIPHID_WIDTH             32
#define ROM_PERIPHID0_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID0_PERIPHID_SHIFT))&ROM_PERIPHID0_PERIPHID_MASK)
/* PERIPHID1 Bit Fields */
#define ROM_PERIPHID1_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID1_PERIPHID_SHIFT             0
#define ROM_PERIPHID1_PERIPHID_WIDTH             32
#define ROM_PERIPHID1_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID1_PERIPHID_SHIFT))&ROM_PERIPHID1_PERIPHID_MASK)
/* PERIPHID2 Bit Fields */
#define ROM_PERIPHID2_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID2_PERIPHID_SHIFT             0
#define ROM_PERIPHID2_PERIPHID_WIDTH             32
#define ROM_PERIPHID2_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID2_PERIPHID_SHIFT))&ROM_PERIPHID2_PERIPHID_MASK)
/* PERIPHID3 Bit Fields */
#define ROM_PERIPHID3_PERIPHID_MASK              0xFFFFFFFFu
#define ROM_PERIPHID3_PERIPHID_SHIFT             0
#define ROM_PERIPHID3_PERIPHID_WIDTH             32
#define ROM_PERIPHID3_PERIPHID(x)                (((uint32_t)(((uint32_t)(x))<<ROM_PERIPHID3_PERIPHID_SHIFT))&ROM_PERIPHID3_PERIPHID_MASK)
/* COMPID Bit Fields */
#define ROM_COMPID_COMPID_MASK                   0xFFFFFFFFu
#define ROM_COMPID_COMPID_SHIFT                  0
#define ROM_COMPID_COMPID_WIDTH                  32
#define ROM_COMPID_COMPID(x)                     (((uint32_t)(((uint32_t)(x))<<ROM_COMPID_COMPID_SHIFT))&ROM_COMPID_COMPID_MASK)

/*!
 * @}
 */ /* end of group ROM_Register_Masks */


/* ROM - Peripheral instance base addresses */
/** Peripheral ROM base address */
#define ROM_BASE                                 (0xF0002000u)
/** Peripheral ROM base pointer */
#define ROM                                      ((ROM_Type *)ROM_BASE)
#define ROM_BASE_PTR                             (ROM)
/** Array initializer of ROM peripheral base addresses */
#define ROM_BASE_ADDRS                           { ROM_BASE }
/** Array initializer of ROM peripheral base pointers */
#define ROM_BASE_PTRS                            { ROM }

/* ----------------------------------------------------------------------------
   -- ROM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ROM_Register_Accessor_Macros ROM - Register accessor macros
 * @{
 */


/* ROM - Register instance definitions */
/* ROM */
#define ROM_ENTRY0                               ROM_ENTRY_REG(ROM,0)
#define ROM_TABLEMARK                            ROM_TABLEMARK_REG(ROM)
#define ROM_SYSACCESS                            ROM_SYSACCESS_REG(ROM)
#define ROM_PERIPHID4                            ROM_PERIPHID4_REG(ROM)
#define ROM_PERIPHID5                            ROM_PERIPHID5_REG(ROM)
#define ROM_PERIPHID6                            ROM_PERIPHID6_REG(ROM)
#define ROM_PERIPHID7                            ROM_PERIPHID7_REG(ROM)
#define ROM_PERIPHID0                            ROM_PERIPHID0_REG(ROM)
#define ROM_PERIPHID1                            ROM_PERIPHID1_REG(ROM)
#define ROM_PERIPHID2                            ROM_PERIPHID2_REG(ROM)
#define ROM_PERIPHID3                            ROM_PERIPHID3_REG(ROM)
#define ROM_COMPID0                              ROM_COMPID_REG(ROM,0)
#define ROM_COMPID1                              ROM_COMPID_REG(ROM,1)
#define ROM_COMPID2                              ROM_COMPID_REG(ROM,2)
#define ROM_COMPID3                              ROM_COMPID_REG(ROM,3)

/* ROM - Register array accessors */
#define ROM_ENTRY(index)                         ROM_ENTRY_REG(ROM,index)
#define ROM_COMPID(index)                        ROM_COMPID_REG(ROM,index)

/*!
 * @}
 */ /* end of group ROM_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group ROM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RTC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Peripheral_Access_Layer RTC Peripheral Access Layer
 * @{
 */

/** RTC - Register Layout Typedef */
typedef struct {
  __IO uint32_t SC;                                /**< RTC Status and Control Register, offset: 0x0 */
  __IO uint32_t MOD;                               /**< RTC Modulo Register, offset: 0x4 */
  __I  uint32_t CNT;                               /**< RTC Counter Register, offset: 0x8 */
} RTC_Type, *RTC_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- RTC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Register_Accessor_Macros RTC - Register accessor macros
 * @{
 */


/* RTC - Register accessors */
#define RTC_SC_REG(base)                         ((base)->SC)
#define RTC_MOD_REG(base)                        ((base)->MOD)
#define RTC_CNT_REG(base)                        ((base)->CNT)

/*!
 * @}
 */ /* end of group RTC_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- RTC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Register_Masks RTC Register Masks
 * @{
 */

/* SC Bit Fields */
#define RTC_SC_RTCO_MASK                         0x10u
#define RTC_SC_RTCO_SHIFT                        4
#define RTC_SC_RTCO_WIDTH                        1
#define RTC_SC_RTCO(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_SC_RTCO_SHIFT))&RTC_SC_RTCO_MASK)
#define RTC_SC_RTIE_MASK                         0x40u
#define RTC_SC_RTIE_SHIFT                        6
#define RTC_SC_RTIE_WIDTH                        1
#define RTC_SC_RTIE(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_SC_RTIE_SHIFT))&RTC_SC_RTIE_MASK)
#define RTC_SC_RTIF_MASK                         0x80u
#define RTC_SC_RTIF_SHIFT                        7
#define RTC_SC_RTIF_WIDTH                        1
#define RTC_SC_RTIF(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_SC_RTIF_SHIFT))&RTC_SC_RTIF_MASK)
#define RTC_SC_RTCPS_MASK                        0x700u
#define RTC_SC_RTCPS_SHIFT                       8
#define RTC_SC_RTCPS_WIDTH                       3
#define RTC_SC_RTCPS(x)                          (((uint32_t)(((uint32_t)(x))<<RTC_SC_RTCPS_SHIFT))&RTC_SC_RTCPS_MASK)
#define RTC_SC_RTCLKS_MASK                       0xC000u
#define RTC_SC_RTCLKS_SHIFT                      14
#define RTC_SC_RTCLKS_WIDTH                      2
#define RTC_SC_RTCLKS(x)                         (((uint32_t)(((uint32_t)(x))<<RTC_SC_RTCLKS_SHIFT))&RTC_SC_RTCLKS_MASK)
/* MOD Bit Fields */
#define RTC_MOD_MOD_MASK                         0xFFFFu
#define RTC_MOD_MOD_SHIFT                        0
#define RTC_MOD_MOD_WIDTH                        16
#define RTC_MOD_MOD(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_MOD_MOD_SHIFT))&RTC_MOD_MOD_MASK)
/* CNT Bit Fields */
#define RTC_CNT_CNT_MASK                         0xFFFFu
#define RTC_CNT_CNT_SHIFT                        0
#define RTC_CNT_CNT_WIDTH                        16
#define RTC_CNT_CNT(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_CNT_CNT_SHIFT))&RTC_CNT_CNT_MASK)

/*!
 * @}
 */ /* end of group RTC_Register_Masks */


/* RTC - Peripheral instance base addresses */
/** Peripheral RTC base address */
#define RTC_BASE                                 (0x4003D000u)
/** Peripheral RTC base pointer */
#define RTC                                      ((RTC_Type *)RTC_BASE)
#define RTC_BASE_PTR                             (RTC)
/** Array initializer of RTC peripheral base addresses */
#define RTC_BASE_ADDRS                           { RTC_BASE }
/** Array initializer of RTC peripheral base pointers */
#define RTC_BASE_PTRS                            { RTC }

/* ----------------------------------------------------------------------------
   -- RTC - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Register_Accessor_Macros RTC - Register accessor macros
 * @{
 */


/* RTC - Register instance definitions */
/* RTC */
#define RTC_SC                                   RTC_SC_REG(RTC)
#define RTC_MOD                                  RTC_MOD_REG(RTC)
#define RTC_CNT                                  RTC_CNT_REG(RTC)

/*!
 * @}
 */ /* end of group RTC_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group RTC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SIM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SIM_Peripheral_Access_Layer SIM Peripheral Access Layer
 * @{
 */

/** SIM - Register Layout Typedef */
typedef struct {
  __I  uint32_t SRSID;                             /**< System Reset Status and ID Register, offset: 0x0 */
  __IO uint32_t SOPT0;                             /**< System Options Register 0, offset: 0x4 */
  __IO uint32_t SOPT1;                             /**< System Options Register, offset: 0x8 */
  __IO uint32_t PINSEL;                            /**< Pin Selection Register 0, offset: 0xC */
  __IO uint32_t PINSEL1;                           /**< Pin Selection Register 1, offset: 0x10 */
  __IO uint32_t SCGC;                              /**< System Clock Gating Control Register, offset: 0x14 */
  __I  uint32_t UIDL;                             /**< Universally Unique Identifier Low Register, offset: 0x18 */
  __I  uint32_t UIDML;                            /**< Universally Unique Identifier Middle Low Register, offset: 0x1C */
  __I  uint32_t UIDMH;                            /**< Universally Unique Identifier Middle High Register, offset: 0x20 */
  __IO uint32_t CLKDIV;                            /**< Clock Divider Register, offset: 0x24 */
} SIM_Type, *SIM_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- SIM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SIM_Register_Accessor_Macros SIM - Register accessor macros
 * @{
 */


/* SIM - Register accessors */
#define SIM_SRSID_REG(base)                      ((base)->SRSID)
#define SIM_SOPT0_REG(base)                      ((base)->SOPT0)
#define SIM_SOPT1_REG(base)                      ((base)->SOPT1)
#define SIM_PINSEL_REG(base)                     ((base)->PINSEL)
#define SIM_PINSEL1_REG(base)                    ((base)->PINSEL1)
#define SIM_SCGC_REG(base)                       ((base)->SCGC)
#define SIM_UUIDL_REG(base)                      ((base)->UUIDL)
#define SIM_UUIDML_REG(base)                     ((base)->UUIDML)
#define SIM_UUIDMH_REG(base)                     ((base)->UUIDMH)
#define SIM_CLKDIV_REG(base)                     ((base)->CLKDIV)

/*!
 * @}
 */ /* end of group SIM_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- SIM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SIM_Register_Masks SIM Register Masks
 * @{
 */

/* SRSID Bit Fields */
#define SIM_SRSID_LVD_MASK                       0x2u
#define SIM_SRSID_LVD_SHIFT                      1
#define SIM_SRSID_LVD_WIDTH                      1
#define SIM_SRSID_LVD(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_LVD_SHIFT))&SIM_SRSID_LVD_MASK)
#define SIM_SRSID_LOC_MASK                       0x4u
#define SIM_SRSID_LOC_SHIFT                      2
#define SIM_SRSID_LOC_WIDTH                      1
#define SIM_SRSID_LOC(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_LOC_SHIFT))&SIM_SRSID_LOC_MASK)
#define SIM_SRSID_WDOG_MASK                      0x20u
#define SIM_SRSID_WDOG_SHIFT                     5
#define SIM_SRSID_WDOG_WIDTH                     1
#define SIM_SRSID_WDOG(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_WDOG_SHIFT))&SIM_SRSID_WDOG_MASK)
#define SIM_SRSID_PIN_MASK                       0x40u
#define SIM_SRSID_PIN_SHIFT                      6
#define SIM_SRSID_PIN_WIDTH                      1
#define SIM_SRSID_PIN(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_PIN_SHIFT))&SIM_SRSID_PIN_MASK)
#define SIM_SRSID_POR_MASK                       0x80u
#define SIM_SRSID_POR_SHIFT                      7
#define SIM_SRSID_POR_WIDTH                      1
#define SIM_SRSID_POR(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_POR_SHIFT))&SIM_SRSID_POR_MASK)
#define SIM_SRSID_LOCKUP_MASK                    0x200u
#define SIM_SRSID_LOCKUP_SHIFT                   9
#define SIM_SRSID_LOCKUP_WIDTH                   1
#define SIM_SRSID_LOCKUP(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_LOCKUP_SHIFT))&SIM_SRSID_LOCKUP_MASK)
#define SIM_SRSID_SW_MASK                        0x400u
#define SIM_SRSID_SW_SHIFT                       10
#define SIM_SRSID_SW_WIDTH                       1
#define SIM_SRSID_SW(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_SW_SHIFT))&SIM_SRSID_SW_MASK)
#define SIM_SRSID_MDMAP_MASK                     0x800u
#define SIM_SRSID_MDMAP_SHIFT                    11
#define SIM_SRSID_MDMAP_WIDTH                    1
#define SIM_SRSID_MDMAP(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_MDMAP_SHIFT))&SIM_SRSID_MDMAP_MASK)
#define SIM_SRSID_SACKERR_MASK                   0x2000u
#define SIM_SRSID_SACKERR_SHIFT                  13
#define SIM_SRSID_SACKERR_WIDTH                  1
#define SIM_SRSID_SACKERR(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_SACKERR_SHIFT))&SIM_SRSID_SACKERR_MASK)
#define SIM_SRSID_PINID_MASK                     0xF0000u
#define SIM_SRSID_PINID_SHIFT                    16
#define SIM_SRSID_PINID_WIDTH                    4
#define SIM_SRSID_PINID(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_PINID_SHIFT))&SIM_SRSID_PINID_MASK)
#define SIM_SRSID_RevID_MASK                     0xF00000u
#define SIM_SRSID_RevID_SHIFT                    20
#define SIM_SRSID_RevID_WIDTH                    4
#define SIM_SRSID_RevID(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_RevID_SHIFT))&SIM_SRSID_RevID_MASK)
#define SIM_SRSID_SUBFAMID_MASK                  0xF000000u
#define SIM_SRSID_SUBFAMID_SHIFT                 24
#define SIM_SRSID_SUBFAMID_WIDTH                 4
#define SIM_SRSID_SUBFAMID(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_SUBFAMID_SHIFT))&SIM_SRSID_SUBFAMID_MASK)
#define SIM_SRSID_FAMID_MASK                     0xF0000000u
#define SIM_SRSID_FAMID_SHIFT                    28
#define SIM_SRSID_FAMID_WIDTH                    4
#define SIM_SRSID_FAMID(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SRSID_FAMID_SHIFT))&SIM_SRSID_FAMID_MASK)
/* SOPT0 Bit Fields */
#define SIM_SOPT0_NMIE_MASK                      0x2u
#define SIM_SOPT0_NMIE_SHIFT                     1
#define SIM_SOPT0_NMIE_WIDTH                     1
#define SIM_SOPT0_NMIE(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_NMIE_SHIFT))&SIM_SOPT0_NMIE_MASK)
#define SIM_SOPT0_RSTPE_MASK                     0x4u
#define SIM_SOPT0_RSTPE_SHIFT                    2
#define SIM_SOPT0_RSTPE_WIDTH                    1
#define SIM_SOPT0_RSTPE(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_RSTPE_SHIFT))&SIM_SOPT0_RSTPE_MASK)
#define SIM_SOPT0_SWDE_MASK                      0x8u
#define SIM_SOPT0_SWDE_SHIFT                     3
#define SIM_SOPT0_SWDE_WIDTH                     1
#define SIM_SOPT0_SWDE(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_SWDE_SHIFT))&SIM_SOPT0_SWDE_MASK)
#define SIM_SOPT0_ACTRG_MASK                     0x20u
#define SIM_SOPT0_ACTRG_SHIFT                    5
#define SIM_SOPT0_ACTRG_WIDTH                    1
#define SIM_SOPT0_ACTRG(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_ACTRG_SHIFT))&SIM_SOPT0_ACTRG_MASK)
#define SIM_SOPT0_RXDFE_MASK                     0x300u
#define SIM_SOPT0_RXDFE_SHIFT                    8
#define SIM_SOPT0_RXDFE_WIDTH                    2
#define SIM_SOPT0_RXDFE(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_RXDFE_SHIFT))&SIM_SOPT0_RXDFE_MASK)
#define SIM_SOPT0_RTCC_MASK                      0x400u
#define SIM_SOPT0_RTCC_SHIFT                     10
#define SIM_SOPT0_RTCC_WIDTH                     1
#define SIM_SOPT0_RTCC(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_RTCC_SHIFT))&SIM_SOPT0_RTCC_MASK)
#define SIM_SOPT0_ACIC_MASK                      0x800u
#define SIM_SOPT0_ACIC_SHIFT                     11
#define SIM_SOPT0_ACIC_WIDTH                     1
#define SIM_SOPT0_ACIC(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_ACIC_SHIFT))&SIM_SOPT0_ACIC_MASK)
#define SIM_SOPT0_RXDCE_MASK                     0x1000u
#define SIM_SOPT0_RXDCE_SHIFT                    12
#define SIM_SOPT0_RXDCE_WIDTH                    1
#define SIM_SOPT0_RXDCE(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_RXDCE_SHIFT))&SIM_SOPT0_RXDCE_MASK)
#define SIM_SOPT0_FTMSYNC_MASK                   0x4000u
#define SIM_SOPT0_FTMSYNC_SHIFT                  14
#define SIM_SOPT0_FTMSYNC_WIDTH                  1
#define SIM_SOPT0_FTMSYNC(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_FTMSYNC_SHIFT))&SIM_SOPT0_FTMSYNC_MASK)
#define SIM_SOPT0_TXDME_MASK                     0x8000u
#define SIM_SOPT0_TXDME_SHIFT                    15
#define SIM_SOPT0_TXDME_WIDTH                    1
#define SIM_SOPT0_TXDME(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_TXDME_SHIFT))&SIM_SOPT0_TXDME_MASK)
#define SIM_SOPT0_BUSREF_MASK                    0x70000u
#define SIM_SOPT0_BUSREF_SHIFT                   16
#define SIM_SOPT0_BUSREF_WIDTH                   3
#define SIM_SOPT0_BUSREF(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_BUSREF_SHIFT))&SIM_SOPT0_BUSREF_MASK)
#define SIM_SOPT0_CLKOE_MASK                     0x80000u
#define SIM_SOPT0_CLKOE_SHIFT                    19
#define SIM_SOPT0_CLKOE_WIDTH                    1
#define SIM_SOPT0_CLKOE(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_CLKOE_SHIFT))&SIM_SOPT0_CLKOE_MASK)
#define SIM_SOPT0_ADHWT_MASK                     0x700000u
#define SIM_SOPT0_ADHWT_SHIFT                    20
#define SIM_SOPT0_ADHWT_WIDTH                    3
#define SIM_SOPT0_ADHWT(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_ADHWT_SHIFT))&SIM_SOPT0_ADHWT_MASK)
#define SIM_SOPT0_DLYACT_MASK                    0x800000u
#define SIM_SOPT0_DLYACT_SHIFT                   23
#define SIM_SOPT0_DLYACT_WIDTH                   1
#define SIM_SOPT0_DLYACT(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_DLYACT_SHIFT))&SIM_SOPT0_DLYACT_MASK)
#define SIM_SOPT0_DELAY_MASK                     0xFF000000u
#define SIM_SOPT0_DELAY_SHIFT                    24
#define SIM_SOPT0_DELAY_WIDTH                    8
#define SIM_SOPT0_DELAY(x)                       (((uint32_t)(((uint32_t)(x))<<SIM_SOPT0_DELAY_SHIFT))&SIM_SOPT0_DELAY_MASK)
/* SOPT1 Bit Fields */
#define SIM_SOPT1_I2C04WEN_MASK                  0x1u
#define SIM_SOPT1_I2C04WEN_SHIFT                 0
#define SIM_SOPT1_I2C04WEN_WIDTH                 1
#define SIM_SOPT1_I2C04WEN(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_SOPT1_I2C04WEN_SHIFT))&SIM_SOPT1_I2C04WEN_MASK)
#define SIM_SOPT1_I2C0OINV_MASK                  0x2u
#define SIM_SOPT1_I2C0OINV_SHIFT                 1
#define SIM_SOPT1_I2C0OINV_WIDTH                 1
#define SIM_SOPT1_I2C0OINV(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_SOPT1_I2C0OINV_SHIFT))&SIM_SOPT1_I2C0OINV_MASK)
#define SIM_SOPT1_ACPWTS_MASK                    0x8u
#define SIM_SOPT1_ACPWTS_SHIFT                   3
#define SIM_SOPT1_ACPWTS_WIDTH                   1
#define SIM_SOPT1_ACPWTS(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_SOPT1_ACPWTS_SHIFT))&SIM_SOPT1_ACPWTS_MASK)
#define SIM_SOPT1_UARTPWTS_MASK                  0x30u
#define SIM_SOPT1_UARTPWTS_SHIFT                 4
#define SIM_SOPT1_UARTPWTS_WIDTH                 2
#define SIM_SOPT1_UARTPWTS(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_SOPT1_UARTPWTS_SHIFT))&SIM_SOPT1_UARTPWTS_MASK)
/* PINSEL Bit Fields */
#define SIM_PINSEL_IRQPS_MASK                    0x7u
#define SIM_PINSEL_IRQPS_SHIFT                   0
#define SIM_PINSEL_IRQPS_WIDTH                   3
#define SIM_PINSEL_IRQPS(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_IRQPS_SHIFT))&SIM_PINSEL_IRQPS_MASK)
#define SIM_PINSEL_RTCPS_MASK                    0x10u
#define SIM_PINSEL_RTCPS_SHIFT                   4
#define SIM_PINSEL_RTCPS_WIDTH                   1
#define SIM_PINSEL_RTCPS(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_RTCPS_SHIFT))&SIM_PINSEL_RTCPS_MASK)
#define SIM_PINSEL_I2C0PS_MASK                   0x20u
#define SIM_PINSEL_I2C0PS_SHIFT                  5
#define SIM_PINSEL_I2C0PS_WIDTH                  1
#define SIM_PINSEL_I2C0PS(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_I2C0PS_SHIFT))&SIM_PINSEL_I2C0PS_MASK)
#define SIM_PINSEL_SPI0PS_MASK                   0x40u
#define SIM_PINSEL_SPI0PS_SHIFT                  6
#define SIM_PINSEL_SPI0PS_WIDTH                  1
#define SIM_PINSEL_SPI0PS(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_SPI0PS_SHIFT))&SIM_PINSEL_SPI0PS_MASK)
#define SIM_PINSEL_UART0PS_MASK                  0x80u
#define SIM_PINSEL_UART0PS_SHIFT                 7
#define SIM_PINSEL_UART0PS_WIDTH                 1
#define SIM_PINSEL_UART0PS(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_UART0PS_SHIFT))&SIM_PINSEL_UART0PS_MASK)
#define SIM_PINSEL_FTM0PS0_MASK                  0x100u
#define SIM_PINSEL_FTM0PS0_SHIFT                 8
#define SIM_PINSEL_FTM0PS0_WIDTH                 1
#define SIM_PINSEL_FTM0PS0(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM0PS0_SHIFT))&SIM_PINSEL_FTM0PS0_MASK)
#define SIM_PINSEL_FTM0PS1_MASK                  0x200u
#define SIM_PINSEL_FTM0PS1_SHIFT                 9
#define SIM_PINSEL_FTM0PS1_WIDTH                 1
#define SIM_PINSEL_FTM0PS1(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM0PS1_SHIFT))&SIM_PINSEL_FTM0PS1_MASK)
#define SIM_PINSEL_FTM1PS0_MASK                  0x400u
#define SIM_PINSEL_FTM1PS0_SHIFT                 10
#define SIM_PINSEL_FTM1PS0_WIDTH                 1
#define SIM_PINSEL_FTM1PS0(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM1PS0_SHIFT))&SIM_PINSEL_FTM1PS0_MASK)
#define SIM_PINSEL_FTM1PS1_MASK                  0x800u
#define SIM_PINSEL_FTM1PS1_SHIFT                 11
#define SIM_PINSEL_FTM1PS1_WIDTH                 1
#define SIM_PINSEL_FTM1PS1(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM1PS1_SHIFT))&SIM_PINSEL_FTM1PS1_MASK)
#define SIM_PINSEL_FTM0CLKPS_MASK                0x3000000u
#define SIM_PINSEL_FTM0CLKPS_SHIFT               24
#define SIM_PINSEL_FTM0CLKPS_WIDTH               2
#define SIM_PINSEL_FTM0CLKPS(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM0CLKPS_SHIFT))&SIM_PINSEL_FTM0CLKPS_MASK)
#define SIM_PINSEL_FTM1CLKPS_MASK                0xC000000u
#define SIM_PINSEL_FTM1CLKPS_SHIFT               26
#define SIM_PINSEL_FTM1CLKPS_WIDTH               2
#define SIM_PINSEL_FTM1CLKPS(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM1CLKPS_SHIFT))&SIM_PINSEL_FTM1CLKPS_MASK)
#define SIM_PINSEL_FTM2CLKPS_MASK                0x30000000u
#define SIM_PINSEL_FTM2CLKPS_SHIFT               28
#define SIM_PINSEL_FTM2CLKPS_WIDTH               2
#define SIM_PINSEL_FTM2CLKPS(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_FTM2CLKPS_SHIFT))&SIM_PINSEL_FTM2CLKPS_MASK)
#define SIM_PINSEL_PWTCLKPS_MASK                 0xC0000000u
#define SIM_PINSEL_PWTCLKPS_SHIFT                30
#define SIM_PINSEL_PWTCLKPS_WIDTH                2
#define SIM_PINSEL_PWTCLKPS(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL_PWTCLKPS_SHIFT))&SIM_PINSEL_PWTCLKPS_MASK)
/* PINSEL1 Bit Fields */
#define SIM_PINSEL1_FTM2PS0_MASK                 0x3u
#define SIM_PINSEL1_FTM2PS0_SHIFT                0
#define SIM_PINSEL1_FTM2PS0_WIDTH                2
#define SIM_PINSEL1_FTM2PS0(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_FTM2PS0_SHIFT))&SIM_PINSEL1_FTM2PS0_MASK)
#define SIM_PINSEL1_FTM2PS1_MASK                 0xCu
#define SIM_PINSEL1_FTM2PS1_SHIFT                2
#define SIM_PINSEL1_FTM2PS1_WIDTH                2
#define SIM_PINSEL1_FTM2PS1(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_FTM2PS1_SHIFT))&SIM_PINSEL1_FTM2PS1_MASK)
#define SIM_PINSEL1_FTM2PS2_MASK                 0x30u
#define SIM_PINSEL1_FTM2PS2_SHIFT                4
#define SIM_PINSEL1_FTM2PS2_WIDTH                2
#define SIM_PINSEL1_FTM2PS2(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_FTM2PS2_SHIFT))&SIM_PINSEL1_FTM2PS2_MASK)
#define SIM_PINSEL1_FTM2PS3_MASK                 0xC0u
#define SIM_PINSEL1_FTM2PS3_SHIFT                6
#define SIM_PINSEL1_FTM2PS3_WIDTH                2
#define SIM_PINSEL1_FTM2PS3(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_FTM2PS3_SHIFT))&SIM_PINSEL1_FTM2PS3_MASK)
#define SIM_PINSEL1_FTM2PS4_MASK                 0x100u
#define SIM_PINSEL1_FTM2PS4_SHIFT                8
#define SIM_PINSEL1_FTM2PS4_WIDTH                1
#define SIM_PINSEL1_FTM2PS4(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_FTM2PS4_SHIFT))&SIM_PINSEL1_FTM2PS4_MASK)
#define SIM_PINSEL1_FTM2PS5_MASK                 0x200u
#define SIM_PINSEL1_FTM2PS5_SHIFT                9
#define SIM_PINSEL1_FTM2PS5_WIDTH                1
#define SIM_PINSEL1_FTM2PS5(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_FTM2PS5_SHIFT))&SIM_PINSEL1_FTM2PS5_MASK)
#define SIM_PINSEL1_I2C1PS_MASK                  0x400u
#define SIM_PINSEL1_I2C1PS_SHIFT                 10
#define SIM_PINSEL1_I2C1PS_WIDTH                 1
#define SIM_PINSEL1_I2C1PS(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_I2C1PS_SHIFT))&SIM_PINSEL1_I2C1PS_MASK)
#define SIM_PINSEL1_SPI1PS_MASK                  0x800u
#define SIM_PINSEL1_SPI1PS_SHIFT                 11
#define SIM_PINSEL1_SPI1PS_WIDTH                 1
#define SIM_PINSEL1_SPI1PS(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_SPI1PS_SHIFT))&SIM_PINSEL1_SPI1PS_MASK)
#define SIM_PINSEL1_UART1PS_MASK                 0x1000u
#define SIM_PINSEL1_UART1PS_SHIFT                12
#define SIM_PINSEL1_UART1PS_WIDTH                1
#define SIM_PINSEL1_UART1PS(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_UART1PS_SHIFT))&SIM_PINSEL1_UART1PS_MASK)
#define SIM_PINSEL1_UART2PS_MASK                 0x2000u
#define SIM_PINSEL1_UART2PS_SHIFT                13
#define SIM_PINSEL1_UART2PS_WIDTH                1
#define SIM_PINSEL1_UART2PS(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_UART2PS_SHIFT))&SIM_PINSEL1_UART2PS_MASK)
#define SIM_PINSEL1_PWTIN0PS_MASK                0x4000u
#define SIM_PINSEL1_PWTIN0PS_SHIFT               14
#define SIM_PINSEL1_PWTIN0PS_WIDTH               1
#define SIM_PINSEL1_PWTIN0PS(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_PWTIN0PS_SHIFT))&SIM_PINSEL1_PWTIN0PS_MASK)
#define SIM_PINSEL1_PWTIN1PS_MASK                0x8000u
#define SIM_PINSEL1_PWTIN1PS_SHIFT               15
#define SIM_PINSEL1_PWTIN1PS_WIDTH               1
#define SIM_PINSEL1_PWTIN1PS(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_PWTIN1PS_SHIFT))&SIM_PINSEL1_PWTIN1PS_MASK)
#define SIM_PINSEL1_MSCANPS_MASK                 0x10000u
#define SIM_PINSEL1_MSCANPS_SHIFT                16
#define SIM_PINSEL1_MSCANPS_WIDTH                1
#define SIM_PINSEL1_MSCANPS(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_PINSEL1_MSCANPS_SHIFT))&SIM_PINSEL1_MSCANPS_MASK)
/* SCGC Bit Fields */
#define SIM_SCGC_RTC_MASK                        0x1u
#define SIM_SCGC_RTC_SHIFT                       0
#define SIM_SCGC_RTC_WIDTH                       1
#define SIM_SCGC_RTC(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_RTC_SHIFT))&SIM_SCGC_RTC_MASK)
#define SIM_SCGC_PIT_MASK                        0x2u
#define SIM_SCGC_PIT_SHIFT                       1
#define SIM_SCGC_PIT_WIDTH                       1
#define SIM_SCGC_PIT(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_PIT_SHIFT))&SIM_SCGC_PIT_MASK)
#define SIM_SCGC_PWT_MASK                        0x10u
#define SIM_SCGC_PWT_SHIFT                       4
#define SIM_SCGC_PWT_WIDTH                       1
#define SIM_SCGC_PWT(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_PWT_SHIFT))&SIM_SCGC_PWT_MASK)
#define SIM_SCGC_FTM0_MASK                       0x20u
#define SIM_SCGC_FTM0_SHIFT                      5
#define SIM_SCGC_FTM0_WIDTH                      1
#define SIM_SCGC_FTM0(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_FTM0_SHIFT))&SIM_SCGC_FTM0_MASK)
#define SIM_SCGC_FTM1_MASK                       0x40u
#define SIM_SCGC_FTM1_SHIFT                      6
#define SIM_SCGC_FTM1_WIDTH                      1
#define SIM_SCGC_FTM1(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_FTM1_SHIFT))&SIM_SCGC_FTM1_MASK)
#define SIM_SCGC_FTM2_MASK                       0x80u
#define SIM_SCGC_FTM2_SHIFT                      7
#define SIM_SCGC_FTM2_WIDTH                      1
#define SIM_SCGC_FTM2(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_FTM2_SHIFT))&SIM_SCGC_FTM2_MASK)
#define SIM_SCGC_CRC_MASK                        0x400u
#define SIM_SCGC_CRC_SHIFT                       10
#define SIM_SCGC_CRC_WIDTH                       1
#define SIM_SCGC_CRC(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_CRC_SHIFT))&SIM_SCGC_CRC_MASK)
#define SIM_SCGC_FLASH_MASK                      0x1000u
#define SIM_SCGC_FLASH_SHIFT                     12
#define SIM_SCGC_FLASH_WIDTH                     1
#define SIM_SCGC_FLASH(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_FLASH_SHIFT))&SIM_SCGC_FLASH_MASK)
#define SIM_SCGC_SWD_MASK                        0x2000u
#define SIM_SCGC_SWD_SHIFT                       13
#define SIM_SCGC_SWD_WIDTH                       1
#define SIM_SCGC_SWD(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_SWD_SHIFT))&SIM_SCGC_SWD_MASK)
#define SIM_SCGC_MSCAN_MASK                      0x8000u
#define SIM_SCGC_MSCAN_SHIFT                     15
#define SIM_SCGC_MSCAN_WIDTH                     1
#define SIM_SCGC_MSCAN(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_MSCAN_SHIFT))&SIM_SCGC_MSCAN_MASK)
#define SIM_SCGC_I2C0_MASK                       0x10000u
#define SIM_SCGC_I2C0_SHIFT                      16
#define SIM_SCGC_I2C0_WIDTH                      1
#define SIM_SCGC_I2C0(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_I2C0_SHIFT))&SIM_SCGC_I2C0_MASK)
#define SIM_SCGC_I2C1_MASK                       0x20000u
#define SIM_SCGC_I2C1_SHIFT                      17
#define SIM_SCGC_I2C1_WIDTH                      1
#define SIM_SCGC_I2C1(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_I2C1_SHIFT))&SIM_SCGC_I2C1_MASK)
#define SIM_SCGC_SPI0_MASK                       0x40000u
#define SIM_SCGC_SPI0_SHIFT                      18
#define SIM_SCGC_SPI0_WIDTH                      1
#define SIM_SCGC_SPI0(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_SPI0_SHIFT))&SIM_SCGC_SPI0_MASK)
#define SIM_SCGC_SPI1_MASK                       0x80000u
#define SIM_SCGC_SPI1_SHIFT                      19
#define SIM_SCGC_SPI1_WIDTH                      1
#define SIM_SCGC_SPI1(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_SPI1_SHIFT))&SIM_SCGC_SPI1_MASK)
#define SIM_SCGC_UART0_MASK                      0x100000u
#define SIM_SCGC_UART0_SHIFT                     20
#define SIM_SCGC_UART0_WIDTH                     1
#define SIM_SCGC_UART0(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_UART0_SHIFT))&SIM_SCGC_UART0_MASK)
#define SIM_SCGC_UART1_MASK                      0x200000u
#define SIM_SCGC_UART1_SHIFT                     21
#define SIM_SCGC_UART1_WIDTH                     1
#define SIM_SCGC_UART1(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_UART1_SHIFT))&SIM_SCGC_UART1_MASK)
#define SIM_SCGC_UART2_MASK                      0x400000u
#define SIM_SCGC_UART2_SHIFT                     22
#define SIM_SCGC_UART2_WIDTH                     1
#define SIM_SCGC_UART2(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_UART2_SHIFT))&SIM_SCGC_UART2_MASK)
#define SIM_SCGC_KBI0_MASK                       0x1000000u
#define SIM_SCGC_KBI0_SHIFT                      24
#define SIM_SCGC_KBI0_WIDTH                      1
#define SIM_SCGC_KBI0(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_KBI0_SHIFT))&SIM_SCGC_KBI0_MASK)
#define SIM_SCGC_KBI1_MASK                       0x2000000u
#define SIM_SCGC_KBI1_SHIFT                      25
#define SIM_SCGC_KBI1_WIDTH                      1
#define SIM_SCGC_KBI1(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_KBI1_SHIFT))&SIM_SCGC_KBI1_MASK)
#define SIM_SCGC_IRQ_MASK                        0x8000000u
#define SIM_SCGC_IRQ_SHIFT                       27
#define SIM_SCGC_IRQ_WIDTH                       1
#define SIM_SCGC_IRQ(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_IRQ_SHIFT))&SIM_SCGC_IRQ_MASK)
#define SIM_SCGC_ADC_MASK                        0x20000000u
#define SIM_SCGC_ADC_SHIFT                       29
#define SIM_SCGC_ADC_WIDTH                       1
#define SIM_SCGC_ADC(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_ADC_SHIFT))&SIM_SCGC_ADC_MASK)
#define SIM_SCGC_ACMP0_MASK                      0x40000000u
#define SIM_SCGC_ACMP0_SHIFT                     30
#define SIM_SCGC_ACMP0_WIDTH                     1
#define SIM_SCGC_ACMP0(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_ACMP0_SHIFT))&SIM_SCGC_ACMP0_MASK)
#define SIM_SCGC_ACMP1_MASK                      0x80000000u
#define SIM_SCGC_ACMP1_SHIFT                     31
#define SIM_SCGC_ACMP1_WIDTH                     1
#define SIM_SCGC_ACMP1(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SCGC_ACMP1_SHIFT))&SIM_SCGC_ACMP1_MASK)
/* UUIDL Bit Fields */
#define SIM_UUIDL_ID_MASK                        0xFFFFFFFFu
#define SIM_UUIDL_ID_SHIFT                       0
#define SIM_UUIDL_ID_WIDTH                       32
#define SIM_UUIDL_ID(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_UUIDL_ID_SHIFT))&SIM_UUIDL_ID_MASK)
/* UUIDML Bit Fields */
#define SIM_UUIDML_ID_MASK                       0xFFFFFFFFu
#define SIM_UUIDML_ID_SHIFT                      0
#define SIM_UUIDML_ID_WIDTH                      32
#define SIM_UUIDML_ID(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_UUIDML_ID_SHIFT))&SIM_UUIDML_ID_MASK)
/* UUIDMH Bit Fields */
#define SIM_UUIDMH_ID_MASK                       0xFFFFu
#define SIM_UUIDMH_ID_SHIFT                      0
#define SIM_UUIDMH_ID_WIDTH                      16
#define SIM_UUIDMH_ID(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_UUIDMH_ID_SHIFT))&SIM_UUIDMH_ID_MASK)
/* CLKDIV Bit Fields */
#define SIM_CLKDIV_OUTDIV3_MASK                  0x100000u
#define SIM_CLKDIV_OUTDIV3_SHIFT                 20
#define SIM_CLKDIV_OUTDIV3_WIDTH                 1
#define SIM_CLKDIV_OUTDIV3(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV_OUTDIV3_SHIFT))&SIM_CLKDIV_OUTDIV3_MASK)
#define SIM_CLKDIV_OUTDIV2_MASK                  0x1000000u
#define SIM_CLKDIV_OUTDIV2_SHIFT                 24
#define SIM_CLKDIV_OUTDIV2_WIDTH                 1
#define SIM_CLKDIV_OUTDIV2(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV_OUTDIV2_SHIFT))&SIM_CLKDIV_OUTDIV2_MASK)
#define SIM_CLKDIV_OUTDIV1_MASK                  0x30000000u
#define SIM_CLKDIV_OUTDIV1_SHIFT                 28
#define SIM_CLKDIV_OUTDIV1_WIDTH                 2
#define SIM_CLKDIV_OUTDIV1(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV_OUTDIV1_SHIFT))&SIM_CLKDIV_OUTDIV1_MASK)

/*!
 * @}
 */ /* end of group SIM_Register_Masks */


/* SIM - Peripheral instance base addresses */
/** Peripheral SIM base address */
#define SIM_BASE                                 (0x40048000u)
/** Peripheral SIM base pointer */
#define SIM                                      ((SIM_Type *)SIM_BASE)
#define SIM_BASE_PTR                             (SIM)
/** Array initializer of SIM peripheral base addresses */
#define SIM_BASE_ADDRS                           { SIM_BASE }
/** Array initializer of SIM peripheral base pointers */
#define SIM_BASE_PTRS                            { SIM }

/* ----------------------------------------------------------------------------
   -- SIM - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SIM_Register_Accessor_Macros SIM - Register accessor macros
 * @{
 */


/* SIM - Register instance definitions */
/* SIM */
#define SIM_SRSID                                SIM_SRSID_REG(SIM)
#define SIM_SOPT0                                SIM_SOPT0_REG(SIM)
#define SIM_SOPT1                                SIM_SOPT1_REG(SIM)
#define SIM_PINSEL0                              SIM_PINSEL_REG(SIM)
#define SIM_PINSEL1                              SIM_PINSEL1_REG(SIM)
#define SIM_SCGC                                 SIM_SCGC_REG(SIM)
#define SIM_UUIDL                                SIM_UUIDL_REG(SIM)
#define SIM_UUIDML                               SIM_UUIDML_REG(SIM)
#define SIM_UUIDMH                               SIM_UUIDMH_REG(SIM)
#define SIM_CLKDIV                               SIM_CLKDIV_REG(SIM)

/*!
 * @}
 */ /* end of group SIM_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group SIM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SPI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Peripheral_Access_Layer SPI Peripheral Access Layer
 * @{
 */

/** SPI - Register Layout Typedef */
typedef struct {
  __IO uint8_t C1;                                 /**< SPI Control Register 1, offset: 0x0 */
  __IO uint8_t C2;                                 /**< SPI Control Register 2, offset: 0x1 */
  __IO uint8_t BR;                                 /**< SPI Baud Rate Register, offset: 0x2 */
  __IO uint8_t S;                                  /**< SPI Status Register, offset: 0x3 */
       uint8_t RESERVED_0[1];
  __IO uint8_t D;                                  /**< SPI Data Register, offset: 0x5 */
       uint8_t RESERVED_1[1];
  __IO uint8_t M;                                  /**< SPI Match Register, offset: 0x7 */
} SPI_Type, *SPI_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- SPI - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Register_Accessor_Macros SPI - Register accessor macros
 * @{
 */


/* SPI - Register accessors */
#define SPI_C1_REG(base)                         ((base)->C1)
#define SPI_C2_REG(base)                         ((base)->C2)
#define SPI_BR_REG(base)                         ((base)->BR)
#define SPI_S_REG(base)                          ((base)->S)
#define SPI_D_REG(base)                          ((base)->D)
#define SPI_M_REG(base)                          ((base)->M)

/*!
 * @}
 */ /* end of group SPI_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- SPI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Register_Masks SPI Register Masks
 * @{
 */

/* C1 Bit Fields */
#define SPI_C1_LSBFE_MASK                        0x1u
#define SPI_C1_LSBFE_SHIFT                       0
#define SPI_C1_LSBFE_WIDTH                       1
#define SPI_C1_LSBFE(x)                          (((uint8_t)(((uint8_t)(x))<<SPI_C1_LSBFE_SHIFT))&SPI_C1_LSBFE_MASK)
#define SPI_C1_SSOE_MASK                         0x2u
#define SPI_C1_SSOE_SHIFT                        1
#define SPI_C1_SSOE_WIDTH                        1
#define SPI_C1_SSOE(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_C1_SSOE_SHIFT))&SPI_C1_SSOE_MASK)
#define SPI_C1_CPHA_MASK                         0x4u
#define SPI_C1_CPHA_SHIFT                        2
#define SPI_C1_CPHA_WIDTH                        1
#define SPI_C1_CPHA(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_C1_CPHA_SHIFT))&SPI_C1_CPHA_MASK)
#define SPI_C1_CPOL_MASK                         0x8u
#define SPI_C1_CPOL_SHIFT                        3
#define SPI_C1_CPOL_WIDTH                        1
#define SPI_C1_CPOL(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_C1_CPOL_SHIFT))&SPI_C1_CPOL_MASK)
#define SPI_C1_MSTR_MASK                         0x10u
#define SPI_C1_MSTR_SHIFT                        4
#define SPI_C1_MSTR_WIDTH                        1
#define SPI_C1_MSTR(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_C1_MSTR_SHIFT))&SPI_C1_MSTR_MASK)
#define SPI_C1_SPTIE_MASK                        0x20u
#define SPI_C1_SPTIE_SHIFT                       5
#define SPI_C1_SPTIE_WIDTH                       1
#define SPI_C1_SPTIE(x)                          (((uint8_t)(((uint8_t)(x))<<SPI_C1_SPTIE_SHIFT))&SPI_C1_SPTIE_MASK)
#define SPI_C1_SPE_MASK                          0x40u
#define SPI_C1_SPE_SHIFT                         6
#define SPI_C1_SPE_WIDTH                         1
#define SPI_C1_SPE(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_C1_SPE_SHIFT))&SPI_C1_SPE_MASK)
#define SPI_C1_SPIE_MASK                         0x80u
#define SPI_C1_SPIE_SHIFT                        7
#define SPI_C1_SPIE_WIDTH                        1
#define SPI_C1_SPIE(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_C1_SPIE_SHIFT))&SPI_C1_SPIE_MASK)
/* C2 Bit Fields */
#define SPI_C2_SPC0_MASK                         0x1u
#define SPI_C2_SPC0_SHIFT                        0
#define SPI_C2_SPC0_WIDTH                        1
#define SPI_C2_SPC0(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_C2_SPC0_SHIFT))&SPI_C2_SPC0_MASK)
#define SPI_C2_SPISWAI_MASK                      0x2u
#define SPI_C2_SPISWAI_SHIFT                     1
#define SPI_C2_SPISWAI_WIDTH                     1
#define SPI_C2_SPISWAI(x)                        (((uint8_t)(((uint8_t)(x))<<SPI_C2_SPISWAI_SHIFT))&SPI_C2_SPISWAI_MASK)
#define SPI_C2_BIDIROE_MASK                      0x8u
#define SPI_C2_BIDIROE_SHIFT                     3
#define SPI_C2_BIDIROE_WIDTH                     1
#define SPI_C2_BIDIROE(x)                        (((uint8_t)(((uint8_t)(x))<<SPI_C2_BIDIROE_SHIFT))&SPI_C2_BIDIROE_MASK)
#define SPI_C2_MODFEN_MASK                       0x10u
#define SPI_C2_MODFEN_SHIFT                      4
#define SPI_C2_MODFEN_WIDTH                      1
#define SPI_C2_MODFEN(x)                         (((uint8_t)(((uint8_t)(x))<<SPI_C2_MODFEN_SHIFT))&SPI_C2_MODFEN_MASK)
#define SPI_C2_SPMIE_MASK                        0x80u
#define SPI_C2_SPMIE_SHIFT                       7
#define SPI_C2_SPMIE_WIDTH                       1
#define SPI_C2_SPMIE(x)                          (((uint8_t)(((uint8_t)(x))<<SPI_C2_SPMIE_SHIFT))&SPI_C2_SPMIE_MASK)
/* BR Bit Fields */
#define SPI_BR_SPR_MASK                          0xFu
#define SPI_BR_SPR_SHIFT                         0
#define SPI_BR_SPR_WIDTH                         4
#define SPI_BR_SPR(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_BR_SPR_SHIFT))&SPI_BR_SPR_MASK)
#define SPI_BR_SPPR_MASK                         0x70u
#define SPI_BR_SPPR_SHIFT                        4
#define SPI_BR_SPPR_WIDTH                        3
#define SPI_BR_SPPR(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_BR_SPPR_SHIFT))&SPI_BR_SPPR_MASK)
/* S Bit Fields */
#define SPI_S_MODF_MASK                          0x10u
#define SPI_S_MODF_SHIFT                         4
#define SPI_S_MODF_WIDTH                         1
#define SPI_S_MODF(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_S_MODF_SHIFT))&SPI_S_MODF_MASK)
#define SPI_S_SPTEF_MASK                         0x20u
#define SPI_S_SPTEF_SHIFT                        5
#define SPI_S_SPTEF_WIDTH                        1
#define SPI_S_SPTEF(x)                           (((uint8_t)(((uint8_t)(x))<<SPI_S_SPTEF_SHIFT))&SPI_S_SPTEF_MASK)
#define SPI_S_SPMF_MASK                          0x40u
#define SPI_S_SPMF_SHIFT                         6
#define SPI_S_SPMF_WIDTH                         1
#define SPI_S_SPMF(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_S_SPMF_SHIFT))&SPI_S_SPMF_MASK)
#define SPI_S_SPRF_MASK                          0x80u
#define SPI_S_SPRF_SHIFT                         7
#define SPI_S_SPRF_WIDTH                         1
#define SPI_S_SPRF(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_S_SPRF_SHIFT))&SPI_S_SPRF_MASK)
/* D Bit Fields */
#define SPI_D_Bits_MASK                          0xFFu
#define SPI_D_Bits_SHIFT                         0
#define SPI_D_Bits_WIDTH                         8
#define SPI_D_Bits(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_D_Bits_SHIFT))&SPI_D_Bits_MASK)
/* M Bit Fields */
#define SPI_M_Bits_MASK                          0xFFu
#define SPI_M_Bits_SHIFT                         0
#define SPI_M_Bits_WIDTH                         8
#define SPI_M_Bits(x)                            (((uint8_t)(((uint8_t)(x))<<SPI_M_Bits_SHIFT))&SPI_M_Bits_MASK)

/*!
 * @}
 */ /* end of group SPI_Register_Masks */


/* SPI - Peripheral instance base addresses */
/** Peripheral SPI0 base address */
#define SPI0_BASE                                (0x40076000u)
/** Peripheral SPI0 base pointer */
#define SPI0                                     ((SPI_Type *)SPI0_BASE)
#define SPI0_BASE_PTR                            (SPI0)
/** Peripheral SPI1 base address */
#define SPI1_BASE                                (0x40077000u)
/** Peripheral SPI1 base pointer */
#define SPI1                                     ((SPI_Type *)SPI1_BASE)
#define SPI1_BASE_PTR                            (SPI1)
/** Array initializer of SPI peripheral base addresses */
#define SPI_BASE_ADDRS                           { SPI0_BASE, SPI1_BASE }
/** Array initializer of SPI peripheral base pointers */
#define SPI_BASE_PTRS                            { SPI0, SPI1 }

/* ----------------------------------------------------------------------------
   -- SPI - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Register_Accessor_Macros SPI - Register accessor macros
 * @{
 */


/* SPI - Register instance definitions */
/* SPI0 */
#define SPI0_C1                                  SPI_C1_REG(SPI0)
#define SPI0_C2                                  SPI_C2_REG(SPI0)
#define SPI0_BR                                  SPI_BR_REG(SPI0)
#define SPI0_S                                   SPI_S_REG(SPI0)
#define SPI0_D                                   SPI_D_REG(SPI0)
#define SPI0_M                                   SPI_M_REG(SPI0)
/* SPI1 */
#define SPI1_C1                                  SPI_C1_REG(SPI1)
#define SPI1_C2                                  SPI_C2_REG(SPI1)
#define SPI1_BR                                  SPI_BR_REG(SPI1)
#define SPI1_S                                   SPI_S_REG(SPI1)
#define SPI1_D                                   SPI_D_REG(SPI1)
#define SPI1_M                                   SPI_M_REG(SPI1)

/*!
 * @}
 */ /* end of group SPI_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group SPI_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- UART Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Peripheral_Access_Layer UART Peripheral Access Layer
 * @{
 */

/** UART - Register Layout Typedef */
typedef struct {
  __IO uint8_t BDH;                                /**< UART Baud Rate Register: High, offset: 0x0 */
  __IO uint8_t BDL;                                /**< UART Baud Rate Register: Low, offset: 0x1 */
  __IO uint8_t C1;                                 /**< UART Control Register 1, offset: 0x2 */
  __IO uint8_t C2;                                 /**< UART Control Register 2, offset: 0x3 */
  __I  uint8_t S1;                                 /**< UART Status Register 1, offset: 0x4 */
  __IO uint8_t S2;                                 /**< UART Status Register 2, offset: 0x5 */
  __IO uint8_t C3;                                 /**< UART Control Register 3, offset: 0x6 */
  __IO uint8_t D;                                  /**< UART Data Register, offset: 0x7 */
} UART_Type, *UART_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- UART - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Register_Accessor_Macros UART - Register accessor macros
 * @{
 */


/* UART - Register accessors */
#define UART_BDH_REG(base)                       ((base)->BDH)
#define UART_BDL_REG(base)                       ((base)->BDL)
#define UART_C1_REG(base)                        ((base)->C1)
#define UART_C2_REG(base)                        ((base)->C2)
#define UART_S1_REG(base)                        ((base)->S1)
#define UART_S2_REG(base)                        ((base)->S2)
#define UART_C3_REG(base)                        ((base)->C3)
#define UART_D_REG(base)                         ((base)->D)

/*!
 * @}
 */ /* end of group UART_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- UART Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Register_Masks UART Register Masks
 * @{
 */

/* BDH Bit Fields */
#define UART_BDH_SBR_MASK                        0x1Fu
#define UART_BDH_SBR_SHIFT                       0
#define UART_BDH_SBR_WIDTH                       5
#define UART_BDH_SBR(x)                          (((uint8_t)(((uint8_t)(x))<<UART_BDH_SBR_SHIFT))&UART_BDH_SBR_MASK)
#define UART_BDH_SBNS_MASK                       0x20u
#define UART_BDH_SBNS_SHIFT                      5
#define UART_BDH_SBNS_WIDTH                      1
#define UART_BDH_SBNS(x)                         (((uint8_t)(((uint8_t)(x))<<UART_BDH_SBNS_SHIFT))&UART_BDH_SBNS_MASK)
#define UART_BDH_RXEDGIE_MASK                    0x40u
#define UART_BDH_RXEDGIE_SHIFT                   6
#define UART_BDH_RXEDGIE_WIDTH                   1
#define UART_BDH_RXEDGIE(x)                      (((uint8_t)(((uint8_t)(x))<<UART_BDH_RXEDGIE_SHIFT))&UART_BDH_RXEDGIE_MASK)
#define UART_BDH_LBKDIE_MASK                     0x80u
#define UART_BDH_LBKDIE_SHIFT                    7
#define UART_BDH_LBKDIE_WIDTH                    1
#define UART_BDH_LBKDIE(x)                       (((uint8_t)(((uint8_t)(x))<<UART_BDH_LBKDIE_SHIFT))&UART_BDH_LBKDIE_MASK)
/* BDL Bit Fields */
#define UART_BDL_SBR_MASK                        0xFFu
#define UART_BDL_SBR_SHIFT                       0
#define UART_BDL_SBR_WIDTH                       8
#define UART_BDL_SBR(x)                          (((uint8_t)(((uint8_t)(x))<<UART_BDL_SBR_SHIFT))&UART_BDL_SBR_MASK)
/* C1 Bit Fields */
#define UART_C1_PT_MASK                          0x1u
#define UART_C1_PT_SHIFT                         0
#define UART_C1_PT_WIDTH                         1
#define UART_C1_PT(x)                            (((uint8_t)(((uint8_t)(x))<<UART_C1_PT_SHIFT))&UART_C1_PT_MASK)
#define UART_C1_PE_MASK                          0x2u
#define UART_C1_PE_SHIFT                         1
#define UART_C1_PE_WIDTH                         1
#define UART_C1_PE(x)                            (((uint8_t)(((uint8_t)(x))<<UART_C1_PE_SHIFT))&UART_C1_PE_MASK)
#define UART_C1_ILT_MASK                         0x4u
#define UART_C1_ILT_SHIFT                        2
#define UART_C1_ILT_WIDTH                        1
#define UART_C1_ILT(x)                           (((uint8_t)(((uint8_t)(x))<<UART_C1_ILT_SHIFT))&UART_C1_ILT_MASK)
#define UART_C1_WAKE_MASK                        0x8u
#define UART_C1_WAKE_SHIFT                       3
#define UART_C1_WAKE_WIDTH                       1
#define UART_C1_WAKE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C1_WAKE_SHIFT))&UART_C1_WAKE_MASK)
#define UART_C1_M_MASK                           0x10u
#define UART_C1_M_SHIFT                          4
#define UART_C1_M_WIDTH                          1
#define UART_C1_M(x)                             (((uint8_t)(((uint8_t)(x))<<UART_C1_M_SHIFT))&UART_C1_M_MASK)
#define UART_C1_RSRC_MASK                        0x20u
#define UART_C1_RSRC_SHIFT                       5
#define UART_C1_RSRC_WIDTH                       1
#define UART_C1_RSRC(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C1_RSRC_SHIFT))&UART_C1_RSRC_MASK)
#define UART_C1_UARTSWAI_MASK                    0x40u
#define UART_C1_UARTSWAI_SHIFT                   6
#define UART_C1_UARTSWAI_WIDTH                   1
#define UART_C1_UARTSWAI(x)                      (((uint8_t)(((uint8_t)(x))<<UART_C1_UARTSWAI_SHIFT))&UART_C1_UARTSWAI_MASK)
#define UART_C1_LOOPS_MASK                       0x80u
#define UART_C1_LOOPS_SHIFT                      7
#define UART_C1_LOOPS_WIDTH                      1
#define UART_C1_LOOPS(x)                         (((uint8_t)(((uint8_t)(x))<<UART_C1_LOOPS_SHIFT))&UART_C1_LOOPS_MASK)
/* C2 Bit Fields */
#define UART_C2_SBK_MASK                         0x1u
#define UART_C2_SBK_SHIFT                        0
#define UART_C2_SBK_WIDTH                        1
#define UART_C2_SBK(x)                           (((uint8_t)(((uint8_t)(x))<<UART_C2_SBK_SHIFT))&UART_C2_SBK_MASK)
#define UART_C2_RWU_MASK                         0x2u
#define UART_C2_RWU_SHIFT                        1
#define UART_C2_RWU_WIDTH                        1
#define UART_C2_RWU(x)                           (((uint8_t)(((uint8_t)(x))<<UART_C2_RWU_SHIFT))&UART_C2_RWU_MASK)
#define UART_C2_RE_MASK                          0x4u
#define UART_C2_RE_SHIFT                         2
#define UART_C2_RE_WIDTH                         1
#define UART_C2_RE(x)                            (((uint8_t)(((uint8_t)(x))<<UART_C2_RE_SHIFT))&UART_C2_RE_MASK)
#define UART_C2_TE_MASK                          0x8u
#define UART_C2_TE_SHIFT                         3
#define UART_C2_TE_WIDTH                         1
#define UART_C2_TE(x)                            (((uint8_t)(((uint8_t)(x))<<UART_C2_TE_SHIFT))&UART_C2_TE_MASK)
#define UART_C2_ILIE_MASK                        0x10u
#define UART_C2_ILIE_SHIFT                       4
#define UART_C2_ILIE_WIDTH                       1
#define UART_C2_ILIE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C2_ILIE_SHIFT))&UART_C2_ILIE_MASK)
#define UART_C2_RIE_MASK                         0x20u
#define UART_C2_RIE_SHIFT                        5
#define UART_C2_RIE_WIDTH                        1
#define UART_C2_RIE(x)                           (((uint8_t)(((uint8_t)(x))<<UART_C2_RIE_SHIFT))&UART_C2_RIE_MASK)
#define UART_C2_TCIE_MASK                        0x40u
#define UART_C2_TCIE_SHIFT                       6
#define UART_C2_TCIE_WIDTH                       1
#define UART_C2_TCIE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C2_TCIE_SHIFT))&UART_C2_TCIE_MASK)
#define UART_C2_TIE_MASK                         0x80u
#define UART_C2_TIE_SHIFT                        7
#define UART_C2_TIE_WIDTH                        1
#define UART_C2_TIE(x)                           (((uint8_t)(((uint8_t)(x))<<UART_C2_TIE_SHIFT))&UART_C2_TIE_MASK)
/* S1 Bit Fields */
#define UART_S1_PF_MASK                          0x1u
#define UART_S1_PF_SHIFT                         0
#define UART_S1_PF_WIDTH                         1
#define UART_S1_PF(x)                            (((uint8_t)(((uint8_t)(x))<<UART_S1_PF_SHIFT))&UART_S1_PF_MASK)
#define UART_S1_FE_MASK                          0x2u
#define UART_S1_FE_SHIFT                         1
#define UART_S1_FE_WIDTH                         1
#define UART_S1_FE(x)                            (((uint8_t)(((uint8_t)(x))<<UART_S1_FE_SHIFT))&UART_S1_FE_MASK)
#define UART_S1_NF_MASK                          0x4u
#define UART_S1_NF_SHIFT                         2
#define UART_S1_NF_WIDTH                         1
#define UART_S1_NF(x)                            (((uint8_t)(((uint8_t)(x))<<UART_S1_NF_SHIFT))&UART_S1_NF_MASK)
#define UART_S1_OR_MASK                          0x8u
#define UART_S1_OR_SHIFT                         3
#define UART_S1_OR_WIDTH                         1
#define UART_S1_OR(x)                            (((uint8_t)(((uint8_t)(x))<<UART_S1_OR_SHIFT))&UART_S1_OR_MASK)
#define UART_S1_IDLE_MASK                        0x10u
#define UART_S1_IDLE_SHIFT                       4
#define UART_S1_IDLE_WIDTH                       1
#define UART_S1_IDLE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_S1_IDLE_SHIFT))&UART_S1_IDLE_MASK)
#define UART_S1_RDRF_MASK                        0x20u
#define UART_S1_RDRF_SHIFT                       5
#define UART_S1_RDRF_WIDTH                       1
#define UART_S1_RDRF(x)                          (((uint8_t)(((uint8_t)(x))<<UART_S1_RDRF_SHIFT))&UART_S1_RDRF_MASK)
#define UART_S1_TC_MASK                          0x40u
#define UART_S1_TC_SHIFT                         6
#define UART_S1_TC_WIDTH                         1
#define UART_S1_TC(x)                            (((uint8_t)(((uint8_t)(x))<<UART_S1_TC_SHIFT))&UART_S1_TC_MASK)
#define UART_S1_TDRE_MASK                        0x80u
#define UART_S1_TDRE_SHIFT                       7
#define UART_S1_TDRE_WIDTH                       1
#define UART_S1_TDRE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_S1_TDRE_SHIFT))&UART_S1_TDRE_MASK)
/* S2 Bit Fields */
#define UART_S2_RAF_MASK                         0x1u
#define UART_S2_RAF_SHIFT                        0
#define UART_S2_RAF_WIDTH                        1
#define UART_S2_RAF(x)                           (((uint8_t)(((uint8_t)(x))<<UART_S2_RAF_SHIFT))&UART_S2_RAF_MASK)
#define UART_S2_LBKDE_MASK                       0x2u
#define UART_S2_LBKDE_SHIFT                      1
#define UART_S2_LBKDE_WIDTH                      1
#define UART_S2_LBKDE(x)                         (((uint8_t)(((uint8_t)(x))<<UART_S2_LBKDE_SHIFT))&UART_S2_LBKDE_MASK)
#define UART_S2_BRK13_MASK                       0x4u
#define UART_S2_BRK13_SHIFT                      2
#define UART_S2_BRK13_WIDTH                      1
#define UART_S2_BRK13(x)                         (((uint8_t)(((uint8_t)(x))<<UART_S2_BRK13_SHIFT))&UART_S2_BRK13_MASK)
#define UART_S2_RWUID_MASK                       0x8u
#define UART_S2_RWUID_SHIFT                      3
#define UART_S2_RWUID_WIDTH                      1
#define UART_S2_RWUID(x)                         (((uint8_t)(((uint8_t)(x))<<UART_S2_RWUID_SHIFT))&UART_S2_RWUID_MASK)
#define UART_S2_RXINV_MASK                       0x10u
#define UART_S2_RXINV_SHIFT                      4
#define UART_S2_RXINV_WIDTH                      1
#define UART_S2_RXINV(x)                         (((uint8_t)(((uint8_t)(x))<<UART_S2_RXINV_SHIFT))&UART_S2_RXINV_MASK)
#define UART_S2_RXEDGIF_MASK                     0x40u
#define UART_S2_RXEDGIF_SHIFT                    6
#define UART_S2_RXEDGIF_WIDTH                    1
#define UART_S2_RXEDGIF(x)                       (((uint8_t)(((uint8_t)(x))<<UART_S2_RXEDGIF_SHIFT))&UART_S2_RXEDGIF_MASK)
#define UART_S2_LBKDIF_MASK                      0x80u
#define UART_S2_LBKDIF_SHIFT                     7
#define UART_S2_LBKDIF_WIDTH                     1
#define UART_S2_LBKDIF(x)                        (((uint8_t)(((uint8_t)(x))<<UART_S2_LBKDIF_SHIFT))&UART_S2_LBKDIF_MASK)
/* C3 Bit Fields */
#define UART_C3_PEIE_MASK                        0x1u
#define UART_C3_PEIE_SHIFT                       0
#define UART_C3_PEIE_WIDTH                       1
#define UART_C3_PEIE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C3_PEIE_SHIFT))&UART_C3_PEIE_MASK)
#define UART_C3_FEIE_MASK                        0x2u
#define UART_C3_FEIE_SHIFT                       1
#define UART_C3_FEIE_WIDTH                       1
#define UART_C3_FEIE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C3_FEIE_SHIFT))&UART_C3_FEIE_MASK)
#define UART_C3_NEIE_MASK                        0x4u
#define UART_C3_NEIE_SHIFT                       2
#define UART_C3_NEIE_WIDTH                       1
#define UART_C3_NEIE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C3_NEIE_SHIFT))&UART_C3_NEIE_MASK)
#define UART_C3_ORIE_MASK                        0x8u
#define UART_C3_ORIE_SHIFT                       3
#define UART_C3_ORIE_WIDTH                       1
#define UART_C3_ORIE(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C3_ORIE_SHIFT))&UART_C3_ORIE_MASK)
#define UART_C3_TXINV_MASK                       0x10u
#define UART_C3_TXINV_SHIFT                      4
#define UART_C3_TXINV_WIDTH                      1
#define UART_C3_TXINV(x)                         (((uint8_t)(((uint8_t)(x))<<UART_C3_TXINV_SHIFT))&UART_C3_TXINV_MASK)
#define UART_C3_TXDIR_MASK                       0x20u
#define UART_C3_TXDIR_SHIFT                      5
#define UART_C3_TXDIR_WIDTH                      1
#define UART_C3_TXDIR(x)                         (((uint8_t)(((uint8_t)(x))<<UART_C3_TXDIR_SHIFT))&UART_C3_TXDIR_MASK)
#define UART_C3_T8_MASK                          0x40u
#define UART_C3_T8_SHIFT                         6
#define UART_C3_T8_WIDTH                         1
#define UART_C3_T8(x)                            (((uint8_t)(((uint8_t)(x))<<UART_C3_T8_SHIFT))&UART_C3_T8_MASK)
#define UART_C3_R8_MASK                          0x80u
#define UART_C3_R8_SHIFT                         7
#define UART_C3_R8_WIDTH                         1
#define UART_C3_R8(x)                            (((uint8_t)(((uint8_t)(x))<<UART_C3_R8_SHIFT))&UART_C3_R8_MASK)
/* D Bit Fields */
#define UART_D_R0T0_MASK                         0x1u
#define UART_D_R0T0_SHIFT                        0
#define UART_D_R0T0_WIDTH                        1
#define UART_D_R0T0(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R0T0_SHIFT))&UART_D_R0T0_MASK)
#define UART_D_R1T1_MASK                         0x2u
#define UART_D_R1T1_SHIFT                        1
#define UART_D_R1T1_WIDTH                        1
#define UART_D_R1T1(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R1T1_SHIFT))&UART_D_R1T1_MASK)
#define UART_D_R2T2_MASK                         0x4u
#define UART_D_R2T2_SHIFT                        2
#define UART_D_R2T2_WIDTH                        1
#define UART_D_R2T2(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R2T2_SHIFT))&UART_D_R2T2_MASK)
#define UART_D_R3T3_MASK                         0x8u
#define UART_D_R3T3_SHIFT                        3
#define UART_D_R3T3_WIDTH                        1
#define UART_D_R3T3(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R3T3_SHIFT))&UART_D_R3T3_MASK)
#define UART_D_R4T4_MASK                         0x10u
#define UART_D_R4T4_SHIFT                        4
#define UART_D_R4T4_WIDTH                        1
#define UART_D_R4T4(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R4T4_SHIFT))&UART_D_R4T4_MASK)
#define UART_D_R5T5_MASK                         0x20u
#define UART_D_R5T5_SHIFT                        5
#define UART_D_R5T5_WIDTH                        1
#define UART_D_R5T5(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R5T5_SHIFT))&UART_D_R5T5_MASK)
#define UART_D_R6T6_MASK                         0x40u
#define UART_D_R6T6_SHIFT                        6
#define UART_D_R6T6_WIDTH                        1
#define UART_D_R6T6(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R6T6_SHIFT))&UART_D_R6T6_MASK)
#define UART_D_R7T7_MASK                         0x80u
#define UART_D_R7T7_SHIFT                        7
#define UART_D_R7T7_WIDTH                        1
#define UART_D_R7T7(x)                           (((uint8_t)(((uint8_t)(x))<<UART_D_R7T7_SHIFT))&UART_D_R7T7_MASK)

/*!
 * @}
 */ /* end of group UART_Register_Masks */


/* UART - Peripheral instance base addresses */
/** Peripheral UART0 base address */
#define UART0_BASE                               (0x4006A000u)
/** Peripheral UART0 base pointer */
#define UART0                                    ((UART_Type *)UART0_BASE)
#define UART0_BASE_PTR                           (UART0)
/** Peripheral UART1 base address */
#define UART1_BASE                               (0x4006B000u)
/** Peripheral UART1 base pointer */
#define UART1                                    ((UART_Type *)UART1_BASE)
#define UART1_BASE_PTR                           (UART1)
/** Peripheral UART2 base address */
#define UART2_BASE                               (0x4006C000u)
/** Peripheral UART2 base pointer */
#define UART2                                    ((UART_Type *)UART2_BASE)
#define UART2_BASE_PTR                           (UART2)
/** Array initializer of UART peripheral base addresses */
#define UART_BASE_ADDRS                          { UART0_BASE, UART1_BASE, UART2_BASE }
/** Array initializer of UART peripheral base pointers */
#define UART_BASE_PTRS                           { UART0, UART1, UART2 }

/* ----------------------------------------------------------------------------
   -- UART - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Register_Accessor_Macros UART - Register accessor macros
 * @{
 */


/* UART - Register instance definitions */
/* UART0 */
#define UART0_BDH                                UART_BDH_REG(UART0)
#define UART0_BDL                                UART_BDL_REG(UART0)
#define UART0_C1                                 UART_C1_REG(UART0)
#define UART0_C2                                 UART_C2_REG(UART0)
#define UART0_S1                                 UART_S1_REG(UART0)
#define UART0_S2                                 UART_S2_REG(UART0)
#define UART0_C3                                 UART_C3_REG(UART0)
#define UART0_D                                  UART_D_REG(UART0)
/* UART1 */
#define UART1_BDH                                UART_BDH_REG(UART1)
#define UART1_BDL                                UART_BDL_REG(UART1)
#define UART1_C1                                 UART_C1_REG(UART1)
#define UART1_C2                                 UART_C2_REG(UART1)
#define UART1_S1                                 UART_S1_REG(UART1)
#define UART1_S2                                 UART_S2_REG(UART1)
#define UART1_C3                                 UART_C3_REG(UART1)
#define UART1_D                                  UART_D_REG(UART1)
/* UART2 */
#define UART2_BDH                                UART_BDH_REG(UART2)
#define UART2_BDL                                UART_BDL_REG(UART2)
#define UART2_C1                                 UART_C1_REG(UART2)
#define UART2_C2                                 UART_C2_REG(UART2)
#define UART2_S1                                 UART_S1_REG(UART2)
#define UART2_S2                                 UART_S2_REG(UART2)
#define UART2_C3                                 UART_C3_REG(UART2)
#define UART2_D                                  UART_D_REG(UART2)

/*!
 * @}
 */ /* end of group UART_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group UART_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- WDOG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDOG_Peripheral_Access_Layer WDOG Peripheral Access Layer
 * @{
 */

/** WDOG - Register Layout Typedef */
typedef struct {
  __IO uint8_t CS1;                                /**< Watchdog Control and Status Register 1, offset: 0x0 */
  __IO uint8_t CS2;                                /**< Watchdog Control and Status Register 2, offset: 0x1 */
  union {                                          /* offset: 0x2 */
    __IO uint16_t CNT;                               /**< Watchdog Counter Register., offset: 0x2 */
    struct {                                         /* offset: 0x2 */
      __I  uint8_t CNTH;                               /**< Watchdog Counter Register: High, offset: 0x2 */
      __I  uint8_t CNTL;                               /**< Watchdog Counter Register: Low, offset: 0x3 */
    } CNT8B;
  };
  union {                                          /* offset: 0x4 */
    __IO uint16_t TOVAL;                             /**< Watchdog Timeout Value Register., offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint8_t TOVALH;                             /**< Watchdog Timeout Value Register: High, offset: 0x4 */
      __IO uint8_t TOVALL;                             /**< Watchdog Timeout Value Register: Low, offset: 0x5 */
    } TOVAL8B;
  };
  union {                                          /* offset: 0x6 */
    __IO uint16_t WIN;                               /**< Watchdog Window Register., offset: 0x6 */
    struct {                                         /* offset: 0x6 */
      __IO uint8_t WINH;                               /**< Watchdog Window Register: High, offset: 0x6 */
      __IO uint8_t WINL;                               /**< Watchdog Window Register: Low, offset: 0x7 */
    } WIN8B;
  };
} WDOG_Type, *WDOG_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- WDOG - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDOG_Register_Accessor_Macros WDOG - Register accessor macros
 * @{
 */


/* WDOG - Register accessors */
#define WDOG_CS1_REG(base)                       ((base)->CS1)
#define WDOG_CS2_REG(base)                       ((base)->CS2)
#define WDOG_CNT_REG(base)                       ((base)->CNT)
#define WDOG_CNTH_REG(base)                      ((base)->CNT8B.CNTH)
#define WDOG_CNTL_REG(base)                      ((base)->CNT8B.CNTL)
#define WDOG_TOVAL_REG(base)                     ((base)->TOVAL)
#define WDOG_TOVALH_REG(base)                    ((base)->TOVAL8B.TOVALH)
#define WDOG_TOVALL_REG(base)                    ((base)->TOVAL8B.TOVALL)
#define WDOG_WIN_REG(base)                       ((base)->WIN)
#define WDOG_WINH_REG(base)                      ((base)->WIN8B.WINH)
#define WDOG_WINL_REG(base)                      ((base)->WIN8B.WINL)

/*!
 * @}
 */ /* end of group WDOG_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- WDOG Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDOG_Register_Masks WDOG Register Masks
 * @{
 */

/* CS1 Bit Fields */
#define WDOG_CS1_STOP_MASK                       0x1u
#define WDOG_CS1_STOP_SHIFT                      0
#define WDOG_CS1_STOP_WIDTH                      1
#define WDOG_CS1_STOP(x)                         (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_STOP_SHIFT))&WDOG_CS1_STOP_MASK)
#define WDOG_CS1_WAIT_MASK                       0x2u
#define WDOG_CS1_WAIT_SHIFT                      1
#define WDOG_CS1_WAIT_WIDTH                      1
#define WDOG_CS1_WAIT(x)                         (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_WAIT_SHIFT))&WDOG_CS1_WAIT_MASK)
#define WDOG_CS1_DBG_MASK                        0x4u
#define WDOG_CS1_DBG_SHIFT                       2
#define WDOG_CS1_DBG_WIDTH                       1
#define WDOG_CS1_DBG(x)                          (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_DBG_SHIFT))&WDOG_CS1_DBG_MASK)
#define WDOG_CS1_TST_MASK                        0x18u
#define WDOG_CS1_TST_SHIFT                       3
#define WDOG_CS1_TST_WIDTH                       2
#define WDOG_CS1_TST(x)                          (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_TST_SHIFT))&WDOG_CS1_TST_MASK)
#define WDOG_CS1_UPDATE_MASK                     0x20u
#define WDOG_CS1_UPDATE_SHIFT                    5
#define WDOG_CS1_UPDATE_WIDTH                    1
#define WDOG_CS1_UPDATE(x)                       (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_UPDATE_SHIFT))&WDOG_CS1_UPDATE_MASK)
#define WDOG_CS1_INT_MASK                        0x40u
#define WDOG_CS1_INT_SHIFT                       6
#define WDOG_CS1_INT_WIDTH                       1
#define WDOG_CS1_INT(x)                          (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_INT_SHIFT))&WDOG_CS1_INT_MASK)
#define WDOG_CS1_EN_MASK                         0x80u
#define WDOG_CS1_EN_SHIFT                        7
#define WDOG_CS1_EN_WIDTH                        1
#define WDOG_CS1_EN(x)                           (((uint8_t)(((uint8_t)(x))<<WDOG_CS1_EN_SHIFT))&WDOG_CS1_EN_MASK)
/* CS2 Bit Fields */
#define WDOG_CS2_CLK_MASK                        0x3u
#define WDOG_CS2_CLK_SHIFT                       0
#define WDOG_CS2_CLK_WIDTH                       2
#define WDOG_CS2_CLK(x)                          (((uint8_t)(((uint8_t)(x))<<WDOG_CS2_CLK_SHIFT))&WDOG_CS2_CLK_MASK)
#define WDOG_CS2_PRES_MASK                       0x10u
#define WDOG_CS2_PRES_SHIFT                      4
#define WDOG_CS2_PRES_WIDTH                      1
#define WDOG_CS2_PRES(x)                         (((uint8_t)(((uint8_t)(x))<<WDOG_CS2_PRES_SHIFT))&WDOG_CS2_PRES_MASK)
#define WDOG_CS2_FLG_MASK                        0x40u
#define WDOG_CS2_FLG_SHIFT                       6
#define WDOG_CS2_FLG_WIDTH                       1
#define WDOG_CS2_FLG(x)                          (((uint8_t)(((uint8_t)(x))<<WDOG_CS2_FLG_SHIFT))&WDOG_CS2_FLG_MASK)
#define WDOG_CS2_WIN_MASK                        0x80u
#define WDOG_CS2_WIN_SHIFT                       7
#define WDOG_CS2_WIN_WIDTH                       1
#define WDOG_CS2_WIN(x)                          (((uint8_t)(((uint8_t)(x))<<WDOG_CS2_WIN_SHIFT))&WDOG_CS2_WIN_MASK)
/* CNT Bit Fields */
#define WDOG_CNT_CNT_MASK                        0xFFFFu
#define WDOG_CNT_CNT_SHIFT                       0
#define WDOG_CNT_CNT_WIDTH                       16
#define WDOG_CNT_CNT(x)                          (((uint16_t)(((uint16_t)(x))<<WDOG_CNT_CNT_SHIFT))&WDOG_CNT_CNT_MASK)
/* CNTH Bit Fields */
#define WDOG_CNTH_CNTHIGH_MASK                   0xFFu
#define WDOG_CNTH_CNTHIGH_SHIFT                  0
#define WDOG_CNTH_CNTHIGH_WIDTH                  8
#define WDOG_CNTH_CNTHIGH(x)                     (((uint8_t)(((uint8_t)(x))<<WDOG_CNTH_CNTHIGH_SHIFT))&WDOG_CNTH_CNTHIGH_MASK)
/* CNTL Bit Fields */
#define WDOG_CNTL_CNTLOW_MASK                    0xFFu
#define WDOG_CNTL_CNTLOW_SHIFT                   0
#define WDOG_CNTL_CNTLOW_WIDTH                   8
#define WDOG_CNTL_CNTLOW(x)                      (((uint8_t)(((uint8_t)(x))<<WDOG_CNTL_CNTLOW_SHIFT))&WDOG_CNTL_CNTLOW_MASK)
/* TOVAL Bit Fields */
#define WDOG_TOVAL_TOVAL_MASK                    0xFFFFu
#define WDOG_TOVAL_TOVAL_SHIFT                   0
#define WDOG_TOVAL_TOVAL_WIDTH                   16
#define WDOG_TOVAL_TOVAL(x)                      (((uint16_t)(((uint16_t)(x))<<WDOG_TOVAL_TOVAL_SHIFT))&WDOG_TOVAL_TOVAL_MASK)
/* TOVALH Bit Fields */
#define WDOG_TOVALH_TOVALHIGH_MASK               0xFFu
#define WDOG_TOVALH_TOVALHIGH_SHIFT              0
#define WDOG_TOVALH_TOVALHIGH_WIDTH              8
#define WDOG_TOVALH_TOVALHIGH(x)                 (((uint8_t)(((uint8_t)(x))<<WDOG_TOVALH_TOVALHIGH_SHIFT))&WDOG_TOVALH_TOVALHIGH_MASK)
/* TOVALL Bit Fields */
#define WDOG_TOVALL_TOVALLOW_MASK                0xFFu
#define WDOG_TOVALL_TOVALLOW_SHIFT               0
#define WDOG_TOVALL_TOVALLOW_WIDTH               8
#define WDOG_TOVALL_TOVALLOW(x)                  (((uint8_t)(((uint8_t)(x))<<WDOG_TOVALL_TOVALLOW_SHIFT))&WDOG_TOVALL_TOVALLOW_MASK)
/* WIN Bit Fields */
#define WDOG_WIN_WIN_MASK                        0xFFFFu
#define WDOG_WIN_WIN_SHIFT                       0
#define WDOG_WIN_WIN_WIDTH                       16
#define WDOG_WIN_WIN(x)                          (((uint16_t)(((uint16_t)(x))<<WDOG_WIN_WIN_SHIFT))&WDOG_WIN_WIN_MASK)
/* WINH Bit Fields */
#define WDOG_WINH_WINHIGH_MASK                   0xFFu
#define WDOG_WINH_WINHIGH_SHIFT                  0
#define WDOG_WINH_WINHIGH_WIDTH                  8
#define WDOG_WINH_WINHIGH(x)                     (((uint8_t)(((uint8_t)(x))<<WDOG_WINH_WINHIGH_SHIFT))&WDOG_WINH_WINHIGH_MASK)
/* WINL Bit Fields */
#define WDOG_WINL_WINLOW_MASK                    0xFFu
#define WDOG_WINL_WINLOW_SHIFT                   0
#define WDOG_WINL_WINLOW_WIDTH                   8
#define WDOG_WINL_WINLOW(x)                      (((uint8_t)(((uint8_t)(x))<<WDOG_WINL_WINLOW_SHIFT))&WDOG_WINL_WINLOW_MASK)

/*!
 * @}
 */ /* end of group WDOG_Register_Masks */


/* WDOG - Peripheral instance base addresses */
/** Peripheral WDOG base address */
#define WDOG_BASE                                (0x40052000u)
/** Peripheral WDOG base pointer */
#define WDOG                                     ((WDOG_Type *)WDOG_BASE)
#define WDOG_BASE_PTR                            (WDOG)
/** Array initializer of WDOG peripheral base addresses */
#define WDOG_BASE_ADDRS                          { WDOG_BASE }
/** Array initializer of WDOG peripheral base pointers */
#define WDOG_BASE_PTRS                           { WDOG }

/* ----------------------------------------------------------------------------
   -- WDOG - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDOG_Register_Accessor_Macros WDOG - Register accessor macros
 * @{
 */


/* WDOG - Register instance definitions */
/* WDOG */
#define WDOG_CS1                                 WDOG_CS1_REG(WDOG)
#define WDOG_CS2                                 WDOG_CS2_REG(WDOG)
#define WDOG_CNT                                 WDOG_CNT_REG(WDOG)
#define WDOG_CNTH                                WDOG_CNTH_REG(WDOG)
#define WDOG_CNTL                                WDOG_CNTL_REG(WDOG)
#define WDOG_TOVAL                               WDOG_TOVAL_REG(WDOG)
#define WDOG_TOVALH                              WDOG_TOVALH_REG(WDOG)
#define WDOG_TOVALL                              WDOG_TOVALL_REG(WDOG)
#define WDOG_WIN                                 WDOG_WIN_REG(WDOG)
#define WDOG_WINH                                WDOG_WINH_REG(WDOG)
#define WDOG_WINL                                WDOG_WINL_REG(WDOG)

/*!
 * @}
 */ /* end of group WDOG_Register_Accessor_Macros */


/*!
 * @}
 */ /* end of group WDOG_Peripheral_Access_Layer */


/*
** End of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #pragma pop
#elif defined(__CWCC__)
  #pragma pop
#elif defined(__GNUC__)
  /* leave anonymous unions enabled */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=default
#elif defined(__ghs__) /* GreenHills */
#else
  #error Not supported compiler type
#endif

/*!
 * @}
 */ /* end of group Peripheral_access_layer */


/* ----------------------------------------------------------------------------
   -- Backward Compatibility
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Backward_Compatibility_Symbols Backward Compatibility
 * @{
 */

#define MSCAN_TDLR_DLC_MASK          MSCAN_TDLR_TDLC_MASK
#define MSCAN_TDLR_DLC_SHIFT         MSCAN_TDLR_TDLC_SHIFT
#define MSCAN_TDLR_DLC(x)            MSCAN_TDLR_TDLC(x)
#define I2C_S_REG(base)              I2C_S1_REG(base)
#define ACMP_BASES                   ACMP_BASE_PTRS
#define ADC_BASES                    ADC_BASE_PTRS
#define CRC_BASES                    CRC_BASE_PTRS
#define FGPIO_BASES                  FGPIO_BASE_PTRS
#define FTM_BASES                    FTM_BASE_PTRS
#define FTMRE_BASES                  FTMRE_BASE_PTRS
#define GPIO_BASES                   GPIO_BASE_PTRS
#define I2C_BASES                    I2C_BASE_PTRS
#define ICS_BASES                    ICS_BASE_PTRS
#define IRQ_BASES                    IRQ_BASE_PTRS
#define KBI_BASES                    KBI_BASE_PTRS
#define MCM_BASES                    MCM_BASE_PTRS
#define MSCAN_BASES                  MSCAN_BASE_PTRS
#define NV_BASES                     NV_BASE_PTRS
#define OSC_BASES                    OSC_BASE_PTRS
#define PIT_BASES                    PIT_BASE_PTRS
#define PMC_BASES                    PMC_BASE_PTRS
#define PORT_BASES                   PORT_BASE_PTRS
#define PWT_BASES                    PWT_BASE_PTRS
#define ROM_BASES                    ROM_BASE_PTRS
#define RTC_BASES                    RTC_BASE_PTRS
#define SIM_BASES                    SIM_BASE_PTRS
#define SPI_BASES                    SPI_BASE_PTRS
#define UART_BASES                   UART_BASE_PTRS
#define WDOG_BASES                   WDOG_BASE_PTRS
#define ADC_IRQn                     ADC0_IRQn
#define ADC_IRQHandler               ADC0_IRQHandler
#define PMC_IRQn                     LVD_LVW_IRQn
#define PMC_IRQHandler               LVD_LVW_IRQHandler
#define Watchdog_IRQn                WDOG_EWM_IRQn
#define Watchdog_IRQHandler          WDOG_EWM_IRQHandler

/*!
 * @}
 */ /* end of group Backward_Compatibility_Symbols */


#else /* #if !defined(SKEAZ1284_H_) */
  /* There is already included the same memory map. Check if it is compatible (has the same major version) */
  #if (MCU_MEM_MAP_VERSION != 0x0100u)
    #if (!defined(MCU_MEM_MAP_SUPPRESS_VERSION_WARNING))
      #warning There are included two not compatible versions of memory maps. Please check possible differences.
    #endif /* (!defined(MCU_MEM_MAP_SUPPRESS_VERSION_WARNING)) */
  #endif /* (MCU_MEM_MAP_VERSION != 0x0100u) */
#endif  /* #if !defined(SKEAZ1284_H_) */

/* SKEAZ1284.h, eof. */
