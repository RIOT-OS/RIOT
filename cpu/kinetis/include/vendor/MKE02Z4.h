/*
** ###################################################################
**     Processors:          MKE02Z16VFM4
**                          MKE02Z16VLC4
**                          MKE02Z16VLD4
**                          MKE02Z32VFM4
**                          MKE02Z32VLC4
**                          MKE02Z32VLD4
**                          MKE02Z32VLH4
**                          MKE02Z32VQH4
**                          MKE02Z64VFM4
**                          MKE02Z64VLC4
**                          MKE02Z64VLD4
**                          MKE02Z64VLH4
**                          MKE02Z64VQH4
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          MCUXpresso Compiler
**
**     Reference manual:    MKE02P64M40SF0RM Rev 4
**     Version:             rev. 1.0, 2017-05-19
**     Build:               b170918
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MKE02Z4
**
**     The Clear BSD License
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2017 NXP
**     All rights reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted (subject to the limitations in the disclaimer below) provided
**      that the following conditions are met:
**
**     1. Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     3. Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2017-05-19)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file MKE02Z4.h
 * @version 1.0
 * @date 2017-05-19
 * @brief CMSIS Peripheral Access Layer for MKE02Z4
 *
 * CMSIS Peripheral Access Layer for MKE02Z4
 */

#ifndef _MKE02Z4_H_
#define _MKE02Z4_H_                              /**< Symbol preventing repeated inclusion */

/** Memory map major version (memory maps with equal major version number are
 * compatible) */
#define MCU_MEM_MAP_VERSION 0x0100U
/** Memory map minor version */
#define MCU_MEM_MAP_VERSION_MINOR 0x0000U


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
  /* Auxiliary constants */
  NotAvail_IRQn                = -128,             /**< Not available device specific interrupt */

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
  FTMRH_IRQn                   = 5,                /**< Command complete and error interrupt */
  PMC_IRQn                     = 6,                /**< Low-voltage warning */
  IRQ_IRQn                     = 7,                /**< External interrupt */
  I2C0_IRQn                    = 8,                /**< Single interrupt vector for all sources */
  Reserved25_IRQn              = 9,                /**< Reserved interrupt */
  SPI0_IRQn                    = 10,               /**< Single interrupt vector for all sources */
  SPI1_IRQn                    = 11,               /**< Single interrupt vector for all sources */
  UART0_IRQn                   = 12,               /**< Status and error */
  UART1_IRQn                   = 13,               /**< Status and error */
  UART2_IRQn                   = 14,               /**< Status and error */
  ADC_IRQn                     = 15,               /**< ADC conversion complete interrupt */
  ACMP0_IRQn                   = 16,               /**< Analog comparator 0 interrupt */
  FTM0_IRQn                    = 17,               /**< FTM0 single interrupt vector for all sources */
  FTM1_IRQn                    = 18,               /**< FTM1 single interrupt vector for all sources */
  FTM2_IRQn                    = 19,               /**< FTM2 single interrupt vector for all sources */
  RTC_IRQn                     = 20,               /**< RTC overflow */
  ACMP1_IRQn                   = 21,               /**< Analog comparator 1 interrupt */
  PIT_CH0_IRQn                 = 22,               /**< PIT CH0 overflow */
  PIT_CH1_IRQn                 = 23,               /**< PIT CH1 overflow */
  KBI0_IRQn                    = 24,               /**< Keyboard interrupt0 */
  KBI1_IRQn                    = 25,               /**< Keyboard interrupt1 */
  Reserved42_IRQn              = 26,               /**< Reserved interrupt */
  ICS_IRQn                     = 27,               /**< Clock loss of lock */
  WDOG_IRQn                    = 28,               /**< Watchdog timeout */
  Reserved45_IRQn              = 29,               /**< Reserved interrupt */
  Reserved46_IRQn              = 30,               /**< Reserved interrupt */
  Reserved47_IRQn              = 31                /**< Reserved interrupt */
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
#define __VTOR_PRESENT                 1         /**< Defines if VTOR is present or not */
#define __NVIC_PRIO_BITS               2         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */

#include "core_cm0plus.h"              /* Core Peripheral Access Layer */
//#include "system_MKE02Z4.h"            /* Device specific configuration file */

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
} ACMP_Type;

/* ----------------------------------------------------------------------------
   -- ACMP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ACMP_Register_Masks ACMP Register Masks
 * @{
 */

/*! @name CS - ACMP Control and Status Register */
#define ACMP_CS_ACMOD_MASK                       (0x3U)
#define ACMP_CS_ACMOD_SHIFT                      (0U)
#define ACMP_CS_ACMOD(x)                         (((uint8_t)(((uint8_t)(x)) << ACMP_CS_ACMOD_SHIFT)) & ACMP_CS_ACMOD_MASK)
#define ACMP_CS_ACOPE_MASK                       (0x4U)
#define ACMP_CS_ACOPE_SHIFT                      (2U)
#define ACMP_CS_ACOPE(x)                         (((uint8_t)(((uint8_t)(x)) << ACMP_CS_ACOPE_SHIFT)) & ACMP_CS_ACOPE_MASK)
#define ACMP_CS_ACO_MASK                         (0x8U)
#define ACMP_CS_ACO_SHIFT                        (3U)
#define ACMP_CS_ACO(x)                           (((uint8_t)(((uint8_t)(x)) << ACMP_CS_ACO_SHIFT)) & ACMP_CS_ACO_MASK)
#define ACMP_CS_ACIE_MASK                        (0x10U)
#define ACMP_CS_ACIE_SHIFT                       (4U)
#define ACMP_CS_ACIE(x)                          (((uint8_t)(((uint8_t)(x)) << ACMP_CS_ACIE_SHIFT)) & ACMP_CS_ACIE_MASK)
#define ACMP_CS_ACF_MASK                         (0x20U)
#define ACMP_CS_ACF_SHIFT                        (5U)
#define ACMP_CS_ACF(x)                           (((uint8_t)(((uint8_t)(x)) << ACMP_CS_ACF_SHIFT)) & ACMP_CS_ACF_MASK)
#define ACMP_CS_HYST_MASK                        (0x40U)
#define ACMP_CS_HYST_SHIFT                       (6U)
#define ACMP_CS_HYST(x)                          (((uint8_t)(((uint8_t)(x)) << ACMP_CS_HYST_SHIFT)) & ACMP_CS_HYST_MASK)
#define ACMP_CS_ACE_MASK                         (0x80U)
#define ACMP_CS_ACE_SHIFT                        (7U)
#define ACMP_CS_ACE(x)                           (((uint8_t)(((uint8_t)(x)) << ACMP_CS_ACE_SHIFT)) & ACMP_CS_ACE_MASK)

/*! @name C0 - ACMP Control Register 0 */
#define ACMP_C0_ACNSEL_MASK                      (0x3U)
#define ACMP_C0_ACNSEL_SHIFT                     (0U)
#define ACMP_C0_ACNSEL(x)                        (((uint8_t)(((uint8_t)(x)) << ACMP_C0_ACNSEL_SHIFT)) & ACMP_C0_ACNSEL_MASK)
#define ACMP_C0_ACPSEL_MASK                      (0x30U)
#define ACMP_C0_ACPSEL_SHIFT                     (4U)
#define ACMP_C0_ACPSEL(x)                        (((uint8_t)(((uint8_t)(x)) << ACMP_C0_ACPSEL_SHIFT)) & ACMP_C0_ACPSEL_MASK)

/*! @name C1 - ACMP Control Register 1 */
#define ACMP_C1_DACVAL_MASK                      (0x3FU)
#define ACMP_C1_DACVAL_SHIFT                     (0U)
#define ACMP_C1_DACVAL(x)                        (((uint8_t)(((uint8_t)(x)) << ACMP_C1_DACVAL_SHIFT)) & ACMP_C1_DACVAL_MASK)
#define ACMP_C1_DACREF_MASK                      (0x40U)
#define ACMP_C1_DACREF_SHIFT                     (6U)
#define ACMP_C1_DACREF(x)                        (((uint8_t)(((uint8_t)(x)) << ACMP_C1_DACREF_SHIFT)) & ACMP_C1_DACREF_MASK)
#define ACMP_C1_DACEN_MASK                       (0x80U)
#define ACMP_C1_DACEN_SHIFT                      (7U)
#define ACMP_C1_DACEN(x)                         (((uint8_t)(((uint8_t)(x)) << ACMP_C1_DACEN_SHIFT)) & ACMP_C1_DACEN_MASK)

/*! @name C2 - ACMP Control Register 2 */
#define ACMP_C2_ACIPE_MASK                       (0x7U)
#define ACMP_C2_ACIPE_SHIFT                      (0U)
#define ACMP_C2_ACIPE(x)                         (((uint8_t)(((uint8_t)(x)) << ACMP_C2_ACIPE_SHIFT)) & ACMP_C2_ACIPE_MASK)


/*!
 * @}
 */ /* end of group ACMP_Register_Masks */


/* ACMP - Peripheral instance base addresses */
/** Peripheral ACMP0 base address */
#define ACMP0_BASE                               (0x40073000u)
/** Peripheral ACMP0 base pointer */
#define ACMP0                                    ((ACMP_Type *)ACMP0_BASE)
/** Peripheral ACMP1 base address */
#define ACMP1_BASE                               (0x40074000u)
/** Peripheral ACMP1 base pointer */
#define ACMP1                                    ((ACMP_Type *)ACMP1_BASE)
/** Array initializer of ACMP peripheral base addresses */
#define ACMP_BASE_ADDRS                          { ACMP0_BASE, ACMP1_BASE }
/** Array initializer of ACMP peripheral base pointers */
#define ACMP_BASE_PTRS                           { ACMP0, ACMP1 }
/** Interrupt vectors for the ACMP peripheral type */
#define ACMP_IRQS                                { ACMP0_IRQn, ACMP1_IRQn }

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
} ADC_Type;

/* ----------------------------------------------------------------------------
   -- ADC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Register_Masks ADC Register Masks
 * @{
 */

/*! @name SC1 - Status and Control Register 1 */
#define ADC_SC1_ADCH_MASK                        (0x1FU)
#define ADC_SC1_ADCH_SHIFT                       (0U)
#define ADC_SC1_ADCH(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC1_ADCH_SHIFT)) & ADC_SC1_ADCH_MASK)
#define ADC_SC1_ADCO_MASK                        (0x20U)
#define ADC_SC1_ADCO_SHIFT                       (5U)
#define ADC_SC1_ADCO(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC1_ADCO_SHIFT)) & ADC_SC1_ADCO_MASK)
#define ADC_SC1_AIEN_MASK                        (0x40U)
#define ADC_SC1_AIEN_SHIFT                       (6U)
#define ADC_SC1_AIEN(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC1_AIEN_SHIFT)) & ADC_SC1_AIEN_MASK)
#define ADC_SC1_COCO_MASK                        (0x80U)
#define ADC_SC1_COCO_SHIFT                       (7U)
#define ADC_SC1_COCO(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC1_COCO_SHIFT)) & ADC_SC1_COCO_MASK)

/*! @name SC2 - Status and Control Register 2 */
#define ADC_SC2_REFSEL_MASK                      (0x3U)
#define ADC_SC2_REFSEL_SHIFT                     (0U)
#define ADC_SC2_REFSEL(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_REFSEL_SHIFT)) & ADC_SC2_REFSEL_MASK)
#define ADC_SC2_FFULL_MASK                       (0x4U)
#define ADC_SC2_FFULL_SHIFT                      (2U)
#define ADC_SC2_FFULL(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_SC2_FFULL_SHIFT)) & ADC_SC2_FFULL_MASK)
#define ADC_SC2_FEMPTY_MASK                      (0x8U)
#define ADC_SC2_FEMPTY_SHIFT                     (3U)
#define ADC_SC2_FEMPTY(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_FEMPTY_SHIFT)) & ADC_SC2_FEMPTY_MASK)
#define ADC_SC2_ACFGT_MASK                       (0x10U)
#define ADC_SC2_ACFGT_SHIFT                      (4U)
#define ADC_SC2_ACFGT(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACFGT_SHIFT)) & ADC_SC2_ACFGT_MASK)
#define ADC_SC2_ACFE_MASK                        (0x20U)
#define ADC_SC2_ACFE_SHIFT                       (5U)
#define ADC_SC2_ACFE(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACFE_SHIFT)) & ADC_SC2_ACFE_MASK)
#define ADC_SC2_ADTRG_MASK                       (0x40U)
#define ADC_SC2_ADTRG_SHIFT                      (6U)
#define ADC_SC2_ADTRG(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ADTRG_SHIFT)) & ADC_SC2_ADTRG_MASK)
#define ADC_SC2_ADACT_MASK                       (0x80U)
#define ADC_SC2_ADACT_SHIFT                      (7U)
#define ADC_SC2_ADACT(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ADACT_SHIFT)) & ADC_SC2_ADACT_MASK)

/*! @name SC3 - Status and Control Register 3 */
#define ADC_SC3_ADICLK_MASK                      (0x3U)
#define ADC_SC3_ADICLK_SHIFT                     (0U)
#define ADC_SC3_ADICLK(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADICLK_SHIFT)) & ADC_SC3_ADICLK_MASK)
#define ADC_SC3_MODE_MASK                        (0xCU)
#define ADC_SC3_MODE_SHIFT                       (2U)
#define ADC_SC3_MODE(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC3_MODE_SHIFT)) & ADC_SC3_MODE_MASK)
#define ADC_SC3_ADLSMP_MASK                      (0x10U)
#define ADC_SC3_ADLSMP_SHIFT                     (4U)
#define ADC_SC3_ADLSMP(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADLSMP_SHIFT)) & ADC_SC3_ADLSMP_MASK)
#define ADC_SC3_ADIV_MASK                        (0x60U)
#define ADC_SC3_ADIV_SHIFT                       (5U)
#define ADC_SC3_ADIV(x)                          (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADIV_SHIFT)) & ADC_SC3_ADIV_MASK)
#define ADC_SC3_ADLPC_MASK                       (0x80U)
#define ADC_SC3_ADLPC_SHIFT                      (7U)
#define ADC_SC3_ADLPC(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADLPC_SHIFT)) & ADC_SC3_ADLPC_MASK)

/*! @name SC4 - Status and Control Register 4 */
#define ADC_SC4_AFDEP_MASK                       (0x7U)
#define ADC_SC4_AFDEP_SHIFT                      (0U)
#define ADC_SC4_AFDEP(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_SC4_AFDEP_SHIFT)) & ADC_SC4_AFDEP_MASK)
#define ADC_SC4_ACFSEL_MASK                      (0x20U)
#define ADC_SC4_ACFSEL_SHIFT                     (5U)
#define ADC_SC4_ACFSEL(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_SC4_ACFSEL_SHIFT)) & ADC_SC4_ACFSEL_MASK)
#define ADC_SC4_ASCANE_MASK                      (0x40U)
#define ADC_SC4_ASCANE_SHIFT                     (6U)
#define ADC_SC4_ASCANE(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_SC4_ASCANE_SHIFT)) & ADC_SC4_ASCANE_MASK)

/*! @name R - Conversion Result Register */
#define ADC_R_ADR_MASK                           (0xFFFU)
#define ADC_R_ADR_SHIFT                          (0U)
#define ADC_R_ADR(x)                             (((uint32_t)(((uint32_t)(x)) << ADC_R_ADR_SHIFT)) & ADC_R_ADR_MASK)

/*! @name CV - Compare Value Register */
#define ADC_CV_CV_MASK                           (0xFFFU)
#define ADC_CV_CV_SHIFT                          (0U)
#define ADC_CV_CV(x)                             (((uint32_t)(((uint32_t)(x)) << ADC_CV_CV_SHIFT)) & ADC_CV_CV_MASK)

/*! @name APCTL1 - Pin Control 1 Register */
#define ADC_APCTL1_ADPC_MASK                     (0xFFFFU)
#define ADC_APCTL1_ADPC_SHIFT                    (0U)
#define ADC_APCTL1_ADPC(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_APCTL1_ADPC_SHIFT)) & ADC_APCTL1_ADPC_MASK)


/*!
 * @}
 */ /* end of group ADC_Register_Masks */


/* ADC - Peripheral instance base addresses */
/** Peripheral ADC base address */
#define ADC_BASE                                 (0x4003B000u)
/** Peripheral ADC base pointer */
#define ADC                                      ((ADC_Type *)ADC_BASE)
/** Array initializer of ADC peripheral base addresses */
#define ADC_BASE_ADDRS                           { ADC_BASE }
/** Array initializer of ADC peripheral base pointers */
#define ADC_BASE_PTRS                            { ADC }
/** Interrupt vectors for the ADC peripheral type */
#define ADC_IRQS                                 { ADC_IRQn }

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
} CRC_Type;

/* ----------------------------------------------------------------------------
   -- CRC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Register_Masks CRC Register Masks
 * @{
 */

/*! @name DATAL - CRC_DATAL register. */
#define CRC_DATAL_DATAL_MASK                     (0xFFFFU)
#define CRC_DATAL_DATAL_SHIFT                    (0U)
#define CRC_DATAL_DATAL(x)                       (((uint16_t)(((uint16_t)(x)) << CRC_DATAL_DATAL_SHIFT)) & CRC_DATAL_DATAL_MASK)

/*! @name DATAH - CRC_DATAH register. */
#define CRC_DATAH_DATAH_MASK                     (0xFFFFU)
#define CRC_DATAH_DATAH_SHIFT                    (0U)
#define CRC_DATAH_DATAH(x)                       (((uint16_t)(((uint16_t)(x)) << CRC_DATAH_DATAH_SHIFT)) & CRC_DATAH_DATAH_MASK)

/*! @name DATA - CRC Data register */
#define CRC_DATA_LL_MASK                         (0xFFU)
#define CRC_DATA_LL_SHIFT                        (0U)
#define CRC_DATA_LL(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_DATA_LL_SHIFT)) & CRC_DATA_LL_MASK)
#define CRC_DATA_LU_MASK                         (0xFF00U)
#define CRC_DATA_LU_SHIFT                        (8U)
#define CRC_DATA_LU(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_DATA_LU_SHIFT)) & CRC_DATA_LU_MASK)
#define CRC_DATA_HL_MASK                         (0xFF0000U)
#define CRC_DATA_HL_SHIFT                        (16U)
#define CRC_DATA_HL(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_DATA_HL_SHIFT)) & CRC_DATA_HL_MASK)
#define CRC_DATA_HU_MASK                         (0xFF000000U)
#define CRC_DATA_HU_SHIFT                        (24U)
#define CRC_DATA_HU(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_DATA_HU_SHIFT)) & CRC_DATA_HU_MASK)

/*! @name DATALL - CRC_DATALL register. */
#define CRC_DATALL_DATALL_MASK                   (0xFFU)
#define CRC_DATALL_DATALL_SHIFT                  (0U)
#define CRC_DATALL_DATALL(x)                     (((uint8_t)(((uint8_t)(x)) << CRC_DATALL_DATALL_SHIFT)) & CRC_DATALL_DATALL_MASK)

/*! @name DATALU - CRC_DATALU register. */
#define CRC_DATALU_DATALU_MASK                   (0xFFU)
#define CRC_DATALU_DATALU_SHIFT                  (0U)
#define CRC_DATALU_DATALU(x)                     (((uint8_t)(((uint8_t)(x)) << CRC_DATALU_DATALU_SHIFT)) & CRC_DATALU_DATALU_MASK)

/*! @name DATAHL - CRC_DATAHL register. */
#define CRC_DATAHL_DATAHL_MASK                   (0xFFU)
#define CRC_DATAHL_DATAHL_SHIFT                  (0U)
#define CRC_DATAHL_DATAHL(x)                     (((uint8_t)(((uint8_t)(x)) << CRC_DATAHL_DATAHL_SHIFT)) & CRC_DATAHL_DATAHL_MASK)

/*! @name DATAHU - CRC_DATAHU register. */
#define CRC_DATAHU_DATAHU_MASK                   (0xFFU)
#define CRC_DATAHU_DATAHU_SHIFT                  (0U)
#define CRC_DATAHU_DATAHU(x)                     (((uint8_t)(((uint8_t)(x)) << CRC_DATAHU_DATAHU_SHIFT)) & CRC_DATAHU_DATAHU_MASK)

/*! @name GPOLYL - CRC_GPOLYL register. */
#define CRC_GPOLYL_GPOLYL_MASK                   (0xFFFFU)
#define CRC_GPOLYL_GPOLYL_SHIFT                  (0U)
#define CRC_GPOLYL_GPOLYL(x)                     (((uint16_t)(((uint16_t)(x)) << CRC_GPOLYL_GPOLYL_SHIFT)) & CRC_GPOLYL_GPOLYL_MASK)

/*! @name GPOLYH - CRC_GPOLYH register. */
#define CRC_GPOLYH_GPOLYH_MASK                   (0xFFFFU)
#define CRC_GPOLYH_GPOLYH_SHIFT                  (0U)
#define CRC_GPOLYH_GPOLYH(x)                     (((uint16_t)(((uint16_t)(x)) << CRC_GPOLYH_GPOLYH_SHIFT)) & CRC_GPOLYH_GPOLYH_MASK)

/*! @name GPOLY - CRC Polynomial register */
#define CRC_GPOLY_LOW_MASK                       (0xFFFFU)
#define CRC_GPOLY_LOW_SHIFT                      (0U)
#define CRC_GPOLY_LOW(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_GPOLY_LOW_SHIFT)) & CRC_GPOLY_LOW_MASK)
#define CRC_GPOLY_HIGH_MASK                      (0xFFFF0000U)
#define CRC_GPOLY_HIGH_SHIFT                     (16U)
#define CRC_GPOLY_HIGH(x)                        (((uint32_t)(((uint32_t)(x)) << CRC_GPOLY_HIGH_SHIFT)) & CRC_GPOLY_HIGH_MASK)

/*! @name GPOLYLL - CRC_GPOLYLL register. */
#define CRC_GPOLYLL_GPOLYLL_MASK                 (0xFFU)
#define CRC_GPOLYLL_GPOLYLL_SHIFT                (0U)
#define CRC_GPOLYLL_GPOLYLL(x)                   (((uint8_t)(((uint8_t)(x)) << CRC_GPOLYLL_GPOLYLL_SHIFT)) & CRC_GPOLYLL_GPOLYLL_MASK)

/*! @name GPOLYLU - CRC_GPOLYLU register. */
#define CRC_GPOLYLU_GPOLYLU_MASK                 (0xFFU)
#define CRC_GPOLYLU_GPOLYLU_SHIFT                (0U)
#define CRC_GPOLYLU_GPOLYLU(x)                   (((uint8_t)(((uint8_t)(x)) << CRC_GPOLYLU_GPOLYLU_SHIFT)) & CRC_GPOLYLU_GPOLYLU_MASK)

/*! @name GPOLYHL - CRC_GPOLYHL register. */
#define CRC_GPOLYHL_GPOLYHL_MASK                 (0xFFU)
#define CRC_GPOLYHL_GPOLYHL_SHIFT                (0U)
#define CRC_GPOLYHL_GPOLYHL(x)                   (((uint8_t)(((uint8_t)(x)) << CRC_GPOLYHL_GPOLYHL_SHIFT)) & CRC_GPOLYHL_GPOLYHL_MASK)

/*! @name GPOLYHU - CRC_GPOLYHU register. */
#define CRC_GPOLYHU_GPOLYHU_MASK                 (0xFFU)
#define CRC_GPOLYHU_GPOLYHU_SHIFT                (0U)
#define CRC_GPOLYHU_GPOLYHU(x)                   (((uint8_t)(((uint8_t)(x)) << CRC_GPOLYHU_GPOLYHU_SHIFT)) & CRC_GPOLYHU_GPOLYHU_MASK)

/*! @name CTRL - CRC Control register */
#define CRC_CTRL_TCRC_MASK                       (0x1000000U)
#define CRC_CTRL_TCRC_SHIFT                      (24U)
#define CRC_CTRL_TCRC(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_TCRC_SHIFT)) & CRC_CTRL_TCRC_MASK)
#define CRC_CTRL_WAS_MASK                        (0x2000000U)
#define CRC_CTRL_WAS_SHIFT                       (25U)
#define CRC_CTRL_WAS(x)                          (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_WAS_SHIFT)) & CRC_CTRL_WAS_MASK)
#define CRC_CTRL_FXOR_MASK                       (0x4000000U)
#define CRC_CTRL_FXOR_SHIFT                      (26U)
#define CRC_CTRL_FXOR(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_FXOR_SHIFT)) & CRC_CTRL_FXOR_MASK)
#define CRC_CTRL_TOTR_MASK                       (0x30000000U)
#define CRC_CTRL_TOTR_SHIFT                      (28U)
#define CRC_CTRL_TOTR(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_TOTR_SHIFT)) & CRC_CTRL_TOTR_MASK)
#define CRC_CTRL_TOT_MASK                        (0xC0000000U)
#define CRC_CTRL_TOT_SHIFT                       (30U)
#define CRC_CTRL_TOT(x)                          (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_TOT_SHIFT)) & CRC_CTRL_TOT_MASK)

/*! @name CTRLHU - CRC_CTRLHU register. */
#define CRC_CTRLHU_TCRC_MASK                     (0x1U)
#define CRC_CTRLHU_TCRC_SHIFT                    (0U)
#define CRC_CTRLHU_TCRC(x)                       (((uint8_t)(((uint8_t)(x)) << CRC_CTRLHU_TCRC_SHIFT)) & CRC_CTRLHU_TCRC_MASK)
#define CRC_CTRLHU_WAS_MASK                      (0x2U)
#define CRC_CTRLHU_WAS_SHIFT                     (1U)
#define CRC_CTRLHU_WAS(x)                        (((uint8_t)(((uint8_t)(x)) << CRC_CTRLHU_WAS_SHIFT)) & CRC_CTRLHU_WAS_MASK)
#define CRC_CTRLHU_FXOR_MASK                     (0x4U)
#define CRC_CTRLHU_FXOR_SHIFT                    (2U)
#define CRC_CTRLHU_FXOR(x)                       (((uint8_t)(((uint8_t)(x)) << CRC_CTRLHU_FXOR_SHIFT)) & CRC_CTRLHU_FXOR_MASK)
#define CRC_CTRLHU_TOTR_MASK                     (0x30U)
#define CRC_CTRLHU_TOTR_SHIFT                    (4U)
#define CRC_CTRLHU_TOTR(x)                       (((uint8_t)(((uint8_t)(x)) << CRC_CTRLHU_TOTR_SHIFT)) & CRC_CTRLHU_TOTR_MASK)
#define CRC_CTRLHU_TOT_MASK                      (0xC0U)
#define CRC_CTRLHU_TOT_SHIFT                     (6U)
#define CRC_CTRLHU_TOT(x)                        (((uint8_t)(((uint8_t)(x)) << CRC_CTRLHU_TOT_SHIFT)) & CRC_CTRLHU_TOT_MASK)


/*!
 * @}
 */ /* end of group CRC_Register_Masks */


/* CRC - Peripheral instance base addresses */
/** Peripheral CRC base address */
#define CRC_BASE                                 (0x40032000u)
/** Peripheral CRC base pointer */
#define CRC0                                     ((CRC_Type *)CRC_BASE)
/** Array initializer of CRC peripheral base addresses */
#define CRC_BASE_ADDRS                           { CRC_BASE }
/** Array initializer of CRC peripheral base pointers */
#define CRC_BASE_PTRS                            { CRC0 }

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
} FGPIO_Type;

/* ----------------------------------------------------------------------------
   -- FGPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Masks FGPIO Register Masks
 * @{
 */

/*! @name PDOR - Port Data Output Register */
#define FGPIO_PDOR_PDO_MASK                      (0xFFFFFFFFU)
#define FGPIO_PDOR_PDO_SHIFT                     (0U)
#define FGPIO_PDOR_PDO(x)                        (((uint32_t)(((uint32_t)(x)) << FGPIO_PDOR_PDO_SHIFT)) & FGPIO_PDOR_PDO_MASK)

/*! @name PSOR - Port Set Output Register */
#define FGPIO_PSOR_PTSO_MASK                     (0xFFFFFFFFU)
#define FGPIO_PSOR_PTSO_SHIFT                    (0U)
#define FGPIO_PSOR_PTSO(x)                       (((uint32_t)(((uint32_t)(x)) << FGPIO_PSOR_PTSO_SHIFT)) & FGPIO_PSOR_PTSO_MASK)

/*! @name PCOR - Port Clear Output Register */
#define FGPIO_PCOR_PTCO_MASK                     (0xFFFFFFFFU)
#define FGPIO_PCOR_PTCO_SHIFT                    (0U)
#define FGPIO_PCOR_PTCO(x)                       (((uint32_t)(((uint32_t)(x)) << FGPIO_PCOR_PTCO_SHIFT)) & FGPIO_PCOR_PTCO_MASK)

/*! @name PTOR - Port Toggle Output Register */
#define FGPIO_PTOR_PTTO_MASK                     (0xFFFFFFFFU)
#define FGPIO_PTOR_PTTO_SHIFT                    (0U)
#define FGPIO_PTOR_PTTO(x)                       (((uint32_t)(((uint32_t)(x)) << FGPIO_PTOR_PTTO_SHIFT)) & FGPIO_PTOR_PTTO_MASK)

/*! @name PDIR - Port Data Input Register */
#define FGPIO_PDIR_PDI_MASK                      (0xFFFFFFFFU)
#define FGPIO_PDIR_PDI_SHIFT                     (0U)
#define FGPIO_PDIR_PDI(x)                        (((uint32_t)(((uint32_t)(x)) << FGPIO_PDIR_PDI_SHIFT)) & FGPIO_PDIR_PDI_MASK)

/*! @name PDDR - Port Data Direction Register */
#define FGPIO_PDDR_PDD_MASK                      (0xFFFFFFFFU)
#define FGPIO_PDDR_PDD_SHIFT                     (0U)
#define FGPIO_PDDR_PDD(x)                        (((uint32_t)(((uint32_t)(x)) << FGPIO_PDDR_PDD_SHIFT)) & FGPIO_PDDR_PDD_MASK)

/*! @name PIDR - Port Input Disable Register */
#define FGPIO_PIDR_PID_MASK                      (0xFFFFFFFFU)
#define FGPIO_PIDR_PID_SHIFT                     (0U)
#define FGPIO_PIDR_PID(x)                        (((uint32_t)(((uint32_t)(x)) << FGPIO_PIDR_PID_SHIFT)) & FGPIO_PIDR_PID_MASK)


/*!
 * @}
 */ /* end of group FGPIO_Register_Masks */


/* FGPIO - Peripheral instance base addresses */
/** Peripheral FGPIOA base address */
#define FGPIOA_BASE                              (0xF8000000u)
/** Peripheral FGPIOA base pointer */
#define FGPIOA                                   ((FGPIO_Type *)FGPIOA_BASE)
/** Peripheral FGPIOB base address */
#define FGPIOB_BASE                              (0xF8000040u)
/** Peripheral FGPIOB base pointer */
#define FGPIOB                                   ((FGPIO_Type *)FGPIOB_BASE)
/** Array initializer of FGPIO peripheral base addresses */
#define FGPIO_BASE_ADDRS                         { FGPIOA_BASE, FGPIOB_BASE }
/** Array initializer of FGPIO peripheral base pointers */
#define FGPIO_BASE_PTRS                          { FGPIOA, FGPIOB }

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
  } CONTROLS[8];
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
       uint8_t RESERVED_0[4];
  __IO uint32_t CONF;                              /**< Configuration, offset: 0x84 */
  __IO uint32_t FLTPOL;                            /**< FTM Fault Input Polarity, offset: 0x88 */
  __IO uint32_t SYNCONF;                           /**< Synchronization Configuration, offset: 0x8C */
  __IO uint32_t INVCTRL;                           /**< FTM Inverting Control, offset: 0x90 */
  __IO uint32_t SWOCTRL;                           /**< FTM Software Output Control, offset: 0x94 */
  __IO uint32_t PWMLOAD;                           /**< FTM PWM Load, offset: 0x98 */
} FTM_Type;

/* ----------------------------------------------------------------------------
   -- FTM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTM_Register_Masks FTM Register Masks
 * @{
 */

/*! @name SC - Status And Control */
#define FTM_SC_PS_MASK                           (0x7U)
#define FTM_SC_PS_SHIFT                          (0U)
#define FTM_SC_PS(x)                             (((uint32_t)(((uint32_t)(x)) << FTM_SC_PS_SHIFT)) & FTM_SC_PS_MASK)
#define FTM_SC_CLKS_MASK                         (0x18U)
#define FTM_SC_CLKS_SHIFT                        (3U)
#define FTM_SC_CLKS(x)                           (((uint32_t)(((uint32_t)(x)) << FTM_SC_CLKS_SHIFT)) & FTM_SC_CLKS_MASK)
#define FTM_SC_CPWMS_MASK                        (0x20U)
#define FTM_SC_CPWMS_SHIFT                       (5U)
#define FTM_SC_CPWMS(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_SC_CPWMS_SHIFT)) & FTM_SC_CPWMS_MASK)
#define FTM_SC_TOIE_MASK                         (0x40U)
#define FTM_SC_TOIE_SHIFT                        (6U)
#define FTM_SC_TOIE(x)                           (((uint32_t)(((uint32_t)(x)) << FTM_SC_TOIE_SHIFT)) & FTM_SC_TOIE_MASK)
#define FTM_SC_TOF_MASK                          (0x80U)
#define FTM_SC_TOF_SHIFT                         (7U)
#define FTM_SC_TOF(x)                            (((uint32_t)(((uint32_t)(x)) << FTM_SC_TOF_SHIFT)) & FTM_SC_TOF_MASK)

/*! @name CNT - Counter */
#define FTM_CNT_COUNT_MASK                       (0xFFFFU)
#define FTM_CNT_COUNT_SHIFT                      (0U)
#define FTM_CNT_COUNT(x)                         (((uint32_t)(((uint32_t)(x)) << FTM_CNT_COUNT_SHIFT)) & FTM_CNT_COUNT_MASK)

/*! @name MOD - Modulo */
#define FTM_MOD_MOD_MASK                         (0xFFFFU)
#define FTM_MOD_MOD_SHIFT                        (0U)
#define FTM_MOD_MOD(x)                           (((uint32_t)(((uint32_t)(x)) << FTM_MOD_MOD_SHIFT)) & FTM_MOD_MOD_MASK)

/*! @name CnSC - Channel (n) Status And Control */
#define FTM_CnSC_ELSA_MASK                       (0x4U)
#define FTM_CnSC_ELSA_SHIFT                      (2U)
#define FTM_CnSC_ELSA(x)                         (((uint32_t)(((uint32_t)(x)) << FTM_CnSC_ELSA_SHIFT)) & FTM_CnSC_ELSA_MASK)
#define FTM_CnSC_ELSB_MASK                       (0x8U)
#define FTM_CnSC_ELSB_SHIFT                      (3U)
#define FTM_CnSC_ELSB(x)                         (((uint32_t)(((uint32_t)(x)) << FTM_CnSC_ELSB_SHIFT)) & FTM_CnSC_ELSB_MASK)
#define FTM_CnSC_MSA_MASK                        (0x10U)
#define FTM_CnSC_MSA_SHIFT                       (4U)
#define FTM_CnSC_MSA(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_CnSC_MSA_SHIFT)) & FTM_CnSC_MSA_MASK)
#define FTM_CnSC_MSB_MASK                        (0x20U)
#define FTM_CnSC_MSB_SHIFT                       (5U)
#define FTM_CnSC_MSB(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_CnSC_MSB_SHIFT)) & FTM_CnSC_MSB_MASK)
#define FTM_CnSC_CHIE_MASK                       (0x40U)
#define FTM_CnSC_CHIE_SHIFT                      (6U)
#define FTM_CnSC_CHIE(x)                         (((uint32_t)(((uint32_t)(x)) << FTM_CnSC_CHIE_SHIFT)) & FTM_CnSC_CHIE_MASK)
#define FTM_CnSC_CHF_MASK                        (0x80U)
#define FTM_CnSC_CHF_SHIFT                       (7U)
#define FTM_CnSC_CHF(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_CnSC_CHF_SHIFT)) & FTM_CnSC_CHF_MASK)

/* The count of FTM_CnSC */
#define FTM_CnSC_COUNT                           (8U)

/*! @name CnV - Channel (n) Value */
#define FTM_CnV_VAL_MASK                         (0xFFFFU)
#define FTM_CnV_VAL_SHIFT                        (0U)
#define FTM_CnV_VAL(x)                           (((uint32_t)(((uint32_t)(x)) << FTM_CnV_VAL_SHIFT)) & FTM_CnV_VAL_MASK)

/* The count of FTM_CnV */
#define FTM_CnV_COUNT                            (8U)

/*! @name CNTIN - Counter Initial Value */
#define FTM_CNTIN_INIT_MASK                      (0xFFFFU)
#define FTM_CNTIN_INIT_SHIFT                     (0U)
#define FTM_CNTIN_INIT(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_CNTIN_INIT_SHIFT)) & FTM_CNTIN_INIT_MASK)

/*! @name STATUS - Capture And Compare Status */
#define FTM_STATUS_CH0F_MASK                     (0x1U)
#define FTM_STATUS_CH0F_SHIFT                    (0U)
#define FTM_STATUS_CH0F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH0F_SHIFT)) & FTM_STATUS_CH0F_MASK)
#define FTM_STATUS_CH1F_MASK                     (0x2U)
#define FTM_STATUS_CH1F_SHIFT                    (1U)
#define FTM_STATUS_CH1F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH1F_SHIFT)) & FTM_STATUS_CH1F_MASK)
#define FTM_STATUS_CH2F_MASK                     (0x4U)
#define FTM_STATUS_CH2F_SHIFT                    (2U)
#define FTM_STATUS_CH2F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH2F_SHIFT)) & FTM_STATUS_CH2F_MASK)
#define FTM_STATUS_CH3F_MASK                     (0x8U)
#define FTM_STATUS_CH3F_SHIFT                    (3U)
#define FTM_STATUS_CH3F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH3F_SHIFT)) & FTM_STATUS_CH3F_MASK)
#define FTM_STATUS_CH4F_MASK                     (0x10U)
#define FTM_STATUS_CH4F_SHIFT                    (4U)
#define FTM_STATUS_CH4F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH4F_SHIFT)) & FTM_STATUS_CH4F_MASK)
#define FTM_STATUS_CH5F_MASK                     (0x20U)
#define FTM_STATUS_CH5F_SHIFT                    (5U)
#define FTM_STATUS_CH5F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH5F_SHIFT)) & FTM_STATUS_CH5F_MASK)
#define FTM_STATUS_CH6F_MASK                     (0x40U)
#define FTM_STATUS_CH6F_SHIFT                    (6U)
#define FTM_STATUS_CH6F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH6F_SHIFT)) & FTM_STATUS_CH6F_MASK)
#define FTM_STATUS_CH7F_MASK                     (0x80U)
#define FTM_STATUS_CH7F_SHIFT                    (7U)
#define FTM_STATUS_CH7F(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_STATUS_CH7F_SHIFT)) & FTM_STATUS_CH7F_MASK)

/*! @name MODE - Features Mode Selection */
#define FTM_MODE_FTMEN_MASK                      (0x1U)
#define FTM_MODE_FTMEN_SHIFT                     (0U)
#define FTM_MODE_FTMEN(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_MODE_FTMEN_SHIFT)) & FTM_MODE_FTMEN_MASK)
#define FTM_MODE_INIT_MASK                       (0x2U)
#define FTM_MODE_INIT_SHIFT                      (1U)
#define FTM_MODE_INIT(x)                         (((uint32_t)(((uint32_t)(x)) << FTM_MODE_INIT_SHIFT)) & FTM_MODE_INIT_MASK)
#define FTM_MODE_WPDIS_MASK                      (0x4U)
#define FTM_MODE_WPDIS_SHIFT                     (2U)
#define FTM_MODE_WPDIS(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_MODE_WPDIS_SHIFT)) & FTM_MODE_WPDIS_MASK)
#define FTM_MODE_PWMSYNC_MASK                    (0x8U)
#define FTM_MODE_PWMSYNC_SHIFT                   (3U)
#define FTM_MODE_PWMSYNC(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_MODE_PWMSYNC_SHIFT)) & FTM_MODE_PWMSYNC_MASK)
#define FTM_MODE_CAPTEST_MASK                    (0x10U)
#define FTM_MODE_CAPTEST_SHIFT                   (4U)
#define FTM_MODE_CAPTEST(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_MODE_CAPTEST_SHIFT)) & FTM_MODE_CAPTEST_MASK)
#define FTM_MODE_FAULTM_MASK                     (0x60U)
#define FTM_MODE_FAULTM_SHIFT                    (5U)
#define FTM_MODE_FAULTM(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_MODE_FAULTM_SHIFT)) & FTM_MODE_FAULTM_MASK)
#define FTM_MODE_FAULTIE_MASK                    (0x80U)
#define FTM_MODE_FAULTIE_SHIFT                   (7U)
#define FTM_MODE_FAULTIE(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_MODE_FAULTIE_SHIFT)) & FTM_MODE_FAULTIE_MASK)

/*! @name SYNC - Synchronization */
#define FTM_SYNC_CNTMIN_MASK                     (0x1U)
#define FTM_SYNC_CNTMIN_SHIFT                    (0U)
#define FTM_SYNC_CNTMIN(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_CNTMIN_SHIFT)) & FTM_SYNC_CNTMIN_MASK)
#define FTM_SYNC_CNTMAX_MASK                     (0x2U)
#define FTM_SYNC_CNTMAX_SHIFT                    (1U)
#define FTM_SYNC_CNTMAX(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_CNTMAX_SHIFT)) & FTM_SYNC_CNTMAX_MASK)
#define FTM_SYNC_REINIT_MASK                     (0x4U)
#define FTM_SYNC_REINIT_SHIFT                    (2U)
#define FTM_SYNC_REINIT(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_REINIT_SHIFT)) & FTM_SYNC_REINIT_MASK)
#define FTM_SYNC_SYNCHOM_MASK                    (0x8U)
#define FTM_SYNC_SYNCHOM_SHIFT                   (3U)
#define FTM_SYNC_SYNCHOM(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_SYNCHOM_SHIFT)) & FTM_SYNC_SYNCHOM_MASK)
#define FTM_SYNC_TRIG0_MASK                      (0x10U)
#define FTM_SYNC_TRIG0_SHIFT                     (4U)
#define FTM_SYNC_TRIG0(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_TRIG0_SHIFT)) & FTM_SYNC_TRIG0_MASK)
#define FTM_SYNC_TRIG1_MASK                      (0x20U)
#define FTM_SYNC_TRIG1_SHIFT                     (5U)
#define FTM_SYNC_TRIG1(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_TRIG1_SHIFT)) & FTM_SYNC_TRIG1_MASK)
#define FTM_SYNC_TRIG2_MASK                      (0x40U)
#define FTM_SYNC_TRIG2_SHIFT                     (6U)
#define FTM_SYNC_TRIG2(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_TRIG2_SHIFT)) & FTM_SYNC_TRIG2_MASK)
#define FTM_SYNC_SWSYNC_MASK                     (0x80U)
#define FTM_SYNC_SWSYNC_SHIFT                    (7U)
#define FTM_SYNC_SWSYNC(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_SYNC_SWSYNC_SHIFT)) & FTM_SYNC_SWSYNC_MASK)

/*! @name OUTINIT - Initial State For Channels Output */
#define FTM_OUTINIT_CH0OI_MASK                   (0x1U)
#define FTM_OUTINIT_CH0OI_SHIFT                  (0U)
#define FTM_OUTINIT_CH0OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH0OI_SHIFT)) & FTM_OUTINIT_CH0OI_MASK)
#define FTM_OUTINIT_CH1OI_MASK                   (0x2U)
#define FTM_OUTINIT_CH1OI_SHIFT                  (1U)
#define FTM_OUTINIT_CH1OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH1OI_SHIFT)) & FTM_OUTINIT_CH1OI_MASK)
#define FTM_OUTINIT_CH2OI_MASK                   (0x4U)
#define FTM_OUTINIT_CH2OI_SHIFT                  (2U)
#define FTM_OUTINIT_CH2OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH2OI_SHIFT)) & FTM_OUTINIT_CH2OI_MASK)
#define FTM_OUTINIT_CH3OI_MASK                   (0x8U)
#define FTM_OUTINIT_CH3OI_SHIFT                  (3U)
#define FTM_OUTINIT_CH3OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH3OI_SHIFT)) & FTM_OUTINIT_CH3OI_MASK)
#define FTM_OUTINIT_CH4OI_MASK                   (0x10U)
#define FTM_OUTINIT_CH4OI_SHIFT                  (4U)
#define FTM_OUTINIT_CH4OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH4OI_SHIFT)) & FTM_OUTINIT_CH4OI_MASK)
#define FTM_OUTINIT_CH5OI_MASK                   (0x20U)
#define FTM_OUTINIT_CH5OI_SHIFT                  (5U)
#define FTM_OUTINIT_CH5OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH5OI_SHIFT)) & FTM_OUTINIT_CH5OI_MASK)
#define FTM_OUTINIT_CH6OI_MASK                   (0x40U)
#define FTM_OUTINIT_CH6OI_SHIFT                  (6U)
#define FTM_OUTINIT_CH6OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH6OI_SHIFT)) & FTM_OUTINIT_CH6OI_MASK)
#define FTM_OUTINIT_CH7OI_MASK                   (0x80U)
#define FTM_OUTINIT_CH7OI_SHIFT                  (7U)
#define FTM_OUTINIT_CH7OI(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTINIT_CH7OI_SHIFT)) & FTM_OUTINIT_CH7OI_MASK)

/*! @name OUTMASK - Output Mask */
#define FTM_OUTMASK_CH0OM_MASK                   (0x1U)
#define FTM_OUTMASK_CH0OM_SHIFT                  (0U)
#define FTM_OUTMASK_CH0OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH0OM_SHIFT)) & FTM_OUTMASK_CH0OM_MASK)
#define FTM_OUTMASK_CH1OM_MASK                   (0x2U)
#define FTM_OUTMASK_CH1OM_SHIFT                  (1U)
#define FTM_OUTMASK_CH1OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH1OM_SHIFT)) & FTM_OUTMASK_CH1OM_MASK)
#define FTM_OUTMASK_CH2OM_MASK                   (0x4U)
#define FTM_OUTMASK_CH2OM_SHIFT                  (2U)
#define FTM_OUTMASK_CH2OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH2OM_SHIFT)) & FTM_OUTMASK_CH2OM_MASK)
#define FTM_OUTMASK_CH3OM_MASK                   (0x8U)
#define FTM_OUTMASK_CH3OM_SHIFT                  (3U)
#define FTM_OUTMASK_CH3OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH3OM_SHIFT)) & FTM_OUTMASK_CH3OM_MASK)
#define FTM_OUTMASK_CH4OM_MASK                   (0x10U)
#define FTM_OUTMASK_CH4OM_SHIFT                  (4U)
#define FTM_OUTMASK_CH4OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH4OM_SHIFT)) & FTM_OUTMASK_CH4OM_MASK)
#define FTM_OUTMASK_CH5OM_MASK                   (0x20U)
#define FTM_OUTMASK_CH5OM_SHIFT                  (5U)
#define FTM_OUTMASK_CH5OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH5OM_SHIFT)) & FTM_OUTMASK_CH5OM_MASK)
#define FTM_OUTMASK_CH6OM_MASK                   (0x40U)
#define FTM_OUTMASK_CH6OM_SHIFT                  (6U)
#define FTM_OUTMASK_CH6OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH6OM_SHIFT)) & FTM_OUTMASK_CH6OM_MASK)
#define FTM_OUTMASK_CH7OM_MASK                   (0x80U)
#define FTM_OUTMASK_CH7OM_SHIFT                  (7U)
#define FTM_OUTMASK_CH7OM(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_OUTMASK_CH7OM_SHIFT)) & FTM_OUTMASK_CH7OM_MASK)

/*! @name COMBINE - Function For Linked Channels */
#define FTM_COMBINE_COMBINE0_MASK                (0x1U)
#define FTM_COMBINE_COMBINE0_SHIFT               (0U)
#define FTM_COMBINE_COMBINE0(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMBINE0_SHIFT)) & FTM_COMBINE_COMBINE0_MASK)
#define FTM_COMBINE_COMP0_MASK                   (0x2U)
#define FTM_COMBINE_COMP0_SHIFT                  (1U)
#define FTM_COMBINE_COMP0(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMP0_SHIFT)) & FTM_COMBINE_COMP0_MASK)
#define FTM_COMBINE_DECAPEN0_MASK                (0x4U)
#define FTM_COMBINE_DECAPEN0_SHIFT               (2U)
#define FTM_COMBINE_DECAPEN0(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAPEN0_SHIFT)) & FTM_COMBINE_DECAPEN0_MASK)
#define FTM_COMBINE_DECAP0_MASK                  (0x8U)
#define FTM_COMBINE_DECAP0_SHIFT                 (3U)
#define FTM_COMBINE_DECAP0(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAP0_SHIFT)) & FTM_COMBINE_DECAP0_MASK)
#define FTM_COMBINE_DTEN0_MASK                   (0x10U)
#define FTM_COMBINE_DTEN0_SHIFT                  (4U)
#define FTM_COMBINE_DTEN0(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DTEN0_SHIFT)) & FTM_COMBINE_DTEN0_MASK)
#define FTM_COMBINE_SYNCEN0_MASK                 (0x20U)
#define FTM_COMBINE_SYNCEN0_SHIFT                (5U)
#define FTM_COMBINE_SYNCEN0(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_SYNCEN0_SHIFT)) & FTM_COMBINE_SYNCEN0_MASK)
#define FTM_COMBINE_FAULTEN0_MASK                (0x40U)
#define FTM_COMBINE_FAULTEN0_SHIFT               (6U)
#define FTM_COMBINE_FAULTEN0(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_FAULTEN0_SHIFT)) & FTM_COMBINE_FAULTEN0_MASK)
#define FTM_COMBINE_COMBINE1_MASK                (0x100U)
#define FTM_COMBINE_COMBINE1_SHIFT               (8U)
#define FTM_COMBINE_COMBINE1(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMBINE1_SHIFT)) & FTM_COMBINE_COMBINE1_MASK)
#define FTM_COMBINE_COMP1_MASK                   (0x200U)
#define FTM_COMBINE_COMP1_SHIFT                  (9U)
#define FTM_COMBINE_COMP1(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMP1_SHIFT)) & FTM_COMBINE_COMP1_MASK)
#define FTM_COMBINE_DECAPEN1_MASK                (0x400U)
#define FTM_COMBINE_DECAPEN1_SHIFT               (10U)
#define FTM_COMBINE_DECAPEN1(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAPEN1_SHIFT)) & FTM_COMBINE_DECAPEN1_MASK)
#define FTM_COMBINE_DECAP1_MASK                  (0x800U)
#define FTM_COMBINE_DECAP1_SHIFT                 (11U)
#define FTM_COMBINE_DECAP1(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAP1_SHIFT)) & FTM_COMBINE_DECAP1_MASK)
#define FTM_COMBINE_DTEN1_MASK                   (0x1000U)
#define FTM_COMBINE_DTEN1_SHIFT                  (12U)
#define FTM_COMBINE_DTEN1(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DTEN1_SHIFT)) & FTM_COMBINE_DTEN1_MASK)
#define FTM_COMBINE_SYNCEN1_MASK                 (0x2000U)
#define FTM_COMBINE_SYNCEN1_SHIFT                (13U)
#define FTM_COMBINE_SYNCEN1(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_SYNCEN1_SHIFT)) & FTM_COMBINE_SYNCEN1_MASK)
#define FTM_COMBINE_FAULTEN1_MASK                (0x4000U)
#define FTM_COMBINE_FAULTEN1_SHIFT               (14U)
#define FTM_COMBINE_FAULTEN1(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_FAULTEN1_SHIFT)) & FTM_COMBINE_FAULTEN1_MASK)
#define FTM_COMBINE_COMBINE2_MASK                (0x10000U)
#define FTM_COMBINE_COMBINE2_SHIFT               (16U)
#define FTM_COMBINE_COMBINE2(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMBINE2_SHIFT)) & FTM_COMBINE_COMBINE2_MASK)
#define FTM_COMBINE_COMP2_MASK                   (0x20000U)
#define FTM_COMBINE_COMP2_SHIFT                  (17U)
#define FTM_COMBINE_COMP2(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMP2_SHIFT)) & FTM_COMBINE_COMP2_MASK)
#define FTM_COMBINE_DECAPEN2_MASK                (0x40000U)
#define FTM_COMBINE_DECAPEN2_SHIFT               (18U)
#define FTM_COMBINE_DECAPEN2(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAPEN2_SHIFT)) & FTM_COMBINE_DECAPEN2_MASK)
#define FTM_COMBINE_DECAP2_MASK                  (0x80000U)
#define FTM_COMBINE_DECAP2_SHIFT                 (19U)
#define FTM_COMBINE_DECAP2(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAP2_SHIFT)) & FTM_COMBINE_DECAP2_MASK)
#define FTM_COMBINE_DTEN2_MASK                   (0x100000U)
#define FTM_COMBINE_DTEN2_SHIFT                  (20U)
#define FTM_COMBINE_DTEN2(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DTEN2_SHIFT)) & FTM_COMBINE_DTEN2_MASK)
#define FTM_COMBINE_SYNCEN2_MASK                 (0x200000U)
#define FTM_COMBINE_SYNCEN2_SHIFT                (21U)
#define FTM_COMBINE_SYNCEN2(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_SYNCEN2_SHIFT)) & FTM_COMBINE_SYNCEN2_MASK)
#define FTM_COMBINE_FAULTEN2_MASK                (0x400000U)
#define FTM_COMBINE_FAULTEN2_SHIFT               (22U)
#define FTM_COMBINE_FAULTEN2(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_FAULTEN2_SHIFT)) & FTM_COMBINE_FAULTEN2_MASK)
#define FTM_COMBINE_COMBINE3_MASK                (0x1000000U)
#define FTM_COMBINE_COMBINE3_SHIFT               (24U)
#define FTM_COMBINE_COMBINE3(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMBINE3_SHIFT)) & FTM_COMBINE_COMBINE3_MASK)
#define FTM_COMBINE_COMP3_MASK                   (0x2000000U)
#define FTM_COMBINE_COMP3_SHIFT                  (25U)
#define FTM_COMBINE_COMP3(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_COMP3_SHIFT)) & FTM_COMBINE_COMP3_MASK)
#define FTM_COMBINE_DECAPEN3_MASK                (0x4000000U)
#define FTM_COMBINE_DECAPEN3_SHIFT               (26U)
#define FTM_COMBINE_DECAPEN3(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAPEN3_SHIFT)) & FTM_COMBINE_DECAPEN3_MASK)
#define FTM_COMBINE_DECAP3_MASK                  (0x8000000U)
#define FTM_COMBINE_DECAP3_SHIFT                 (27U)
#define FTM_COMBINE_DECAP3(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DECAP3_SHIFT)) & FTM_COMBINE_DECAP3_MASK)
#define FTM_COMBINE_DTEN3_MASK                   (0x10000000U)
#define FTM_COMBINE_DTEN3_SHIFT                  (28U)
#define FTM_COMBINE_DTEN3(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_DTEN3_SHIFT)) & FTM_COMBINE_DTEN3_MASK)
#define FTM_COMBINE_SYNCEN3_MASK                 (0x20000000U)
#define FTM_COMBINE_SYNCEN3_SHIFT                (29U)
#define FTM_COMBINE_SYNCEN3(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_SYNCEN3_SHIFT)) & FTM_COMBINE_SYNCEN3_MASK)
#define FTM_COMBINE_FAULTEN3_MASK                (0x40000000U)
#define FTM_COMBINE_FAULTEN3_SHIFT               (30U)
#define FTM_COMBINE_FAULTEN3(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_COMBINE_FAULTEN3_SHIFT)) & FTM_COMBINE_FAULTEN3_MASK)

/*! @name DEADTIME - Deadtime Insertion Control */
#define FTM_DEADTIME_DTVAL_MASK                  (0x3FU)
#define FTM_DEADTIME_DTVAL_SHIFT                 (0U)
#define FTM_DEADTIME_DTVAL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_DEADTIME_DTVAL_SHIFT)) & FTM_DEADTIME_DTVAL_MASK)
#define FTM_DEADTIME_DTPS_MASK                   (0xC0U)
#define FTM_DEADTIME_DTPS_SHIFT                  (6U)
#define FTM_DEADTIME_DTPS(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_DEADTIME_DTPS_SHIFT)) & FTM_DEADTIME_DTPS_MASK)

/*! @name EXTTRIG - FTM External Trigger */
#define FTM_EXTTRIG_CH2TRIG_MASK                 (0x1U)
#define FTM_EXTTRIG_CH2TRIG_SHIFT                (0U)
#define FTM_EXTTRIG_CH2TRIG(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_CH2TRIG_SHIFT)) & FTM_EXTTRIG_CH2TRIG_MASK)
#define FTM_EXTTRIG_CH3TRIG_MASK                 (0x2U)
#define FTM_EXTTRIG_CH3TRIG_SHIFT                (1U)
#define FTM_EXTTRIG_CH3TRIG(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_CH3TRIG_SHIFT)) & FTM_EXTTRIG_CH3TRIG_MASK)
#define FTM_EXTTRIG_CH4TRIG_MASK                 (0x4U)
#define FTM_EXTTRIG_CH4TRIG_SHIFT                (2U)
#define FTM_EXTTRIG_CH4TRIG(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_CH4TRIG_SHIFT)) & FTM_EXTTRIG_CH4TRIG_MASK)
#define FTM_EXTTRIG_CH5TRIG_MASK                 (0x8U)
#define FTM_EXTTRIG_CH5TRIG_SHIFT                (3U)
#define FTM_EXTTRIG_CH5TRIG(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_CH5TRIG_SHIFT)) & FTM_EXTTRIG_CH5TRIG_MASK)
#define FTM_EXTTRIG_CH0TRIG_MASK                 (0x10U)
#define FTM_EXTTRIG_CH0TRIG_SHIFT                (4U)
#define FTM_EXTTRIG_CH0TRIG(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_CH0TRIG_SHIFT)) & FTM_EXTTRIG_CH0TRIG_MASK)
#define FTM_EXTTRIG_CH1TRIG_MASK                 (0x20U)
#define FTM_EXTTRIG_CH1TRIG_SHIFT                (5U)
#define FTM_EXTTRIG_CH1TRIG(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_CH1TRIG_SHIFT)) & FTM_EXTTRIG_CH1TRIG_MASK)
#define FTM_EXTTRIG_INITTRIGEN_MASK              (0x40U)
#define FTM_EXTTRIG_INITTRIGEN_SHIFT             (6U)
#define FTM_EXTTRIG_INITTRIGEN(x)                (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_INITTRIGEN_SHIFT)) & FTM_EXTTRIG_INITTRIGEN_MASK)
#define FTM_EXTTRIG_TRIGF_MASK                   (0x80U)
#define FTM_EXTTRIG_TRIGF_SHIFT                  (7U)
#define FTM_EXTTRIG_TRIGF(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_EXTTRIG_TRIGF_SHIFT)) & FTM_EXTTRIG_TRIGF_MASK)

/*! @name POL - Channels Polarity */
#define FTM_POL_POL0_MASK                        (0x1U)
#define FTM_POL_POL0_SHIFT                       (0U)
#define FTM_POL_POL0(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL0_SHIFT)) & FTM_POL_POL0_MASK)
#define FTM_POL_POL1_MASK                        (0x2U)
#define FTM_POL_POL1_SHIFT                       (1U)
#define FTM_POL_POL1(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL1_SHIFT)) & FTM_POL_POL1_MASK)
#define FTM_POL_POL2_MASK                        (0x4U)
#define FTM_POL_POL2_SHIFT                       (2U)
#define FTM_POL_POL2(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL2_SHIFT)) & FTM_POL_POL2_MASK)
#define FTM_POL_POL3_MASK                        (0x8U)
#define FTM_POL_POL3_SHIFT                       (3U)
#define FTM_POL_POL3(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL3_SHIFT)) & FTM_POL_POL3_MASK)
#define FTM_POL_POL4_MASK                        (0x10U)
#define FTM_POL_POL4_SHIFT                       (4U)
#define FTM_POL_POL4(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL4_SHIFT)) & FTM_POL_POL4_MASK)
#define FTM_POL_POL5_MASK                        (0x20U)
#define FTM_POL_POL5_SHIFT                       (5U)
#define FTM_POL_POL5(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL5_SHIFT)) & FTM_POL_POL5_MASK)
#define FTM_POL_POL6_MASK                        (0x40U)
#define FTM_POL_POL6_SHIFT                       (6U)
#define FTM_POL_POL6(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL6_SHIFT)) & FTM_POL_POL6_MASK)
#define FTM_POL_POL7_MASK                        (0x80U)
#define FTM_POL_POL7_SHIFT                       (7U)
#define FTM_POL_POL7(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_POL_POL7_SHIFT)) & FTM_POL_POL7_MASK)

/*! @name FMS - Fault Mode Status */
#define FTM_FMS_FAULTF0_MASK                     (0x1U)
#define FTM_FMS_FAULTF0_SHIFT                    (0U)
#define FTM_FMS_FAULTF0(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_FMS_FAULTF0_SHIFT)) & FTM_FMS_FAULTF0_MASK)
#define FTM_FMS_FAULTF1_MASK                     (0x2U)
#define FTM_FMS_FAULTF1_SHIFT                    (1U)
#define FTM_FMS_FAULTF1(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_FMS_FAULTF1_SHIFT)) & FTM_FMS_FAULTF1_MASK)
#define FTM_FMS_FAULTF2_MASK                     (0x4U)
#define FTM_FMS_FAULTF2_SHIFT                    (2U)
#define FTM_FMS_FAULTF2(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_FMS_FAULTF2_SHIFT)) & FTM_FMS_FAULTF2_MASK)
#define FTM_FMS_FAULTF3_MASK                     (0x8U)
#define FTM_FMS_FAULTF3_SHIFT                    (3U)
#define FTM_FMS_FAULTF3(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_FMS_FAULTF3_SHIFT)) & FTM_FMS_FAULTF3_MASK)
#define FTM_FMS_FAULTIN_MASK                     (0x20U)
#define FTM_FMS_FAULTIN_SHIFT                    (5U)
#define FTM_FMS_FAULTIN(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_FMS_FAULTIN_SHIFT)) & FTM_FMS_FAULTIN_MASK)
#define FTM_FMS_WPEN_MASK                        (0x40U)
#define FTM_FMS_WPEN_SHIFT                       (6U)
#define FTM_FMS_WPEN(x)                          (((uint32_t)(((uint32_t)(x)) << FTM_FMS_WPEN_SHIFT)) & FTM_FMS_WPEN_MASK)
#define FTM_FMS_FAULTF_MASK                      (0x80U)
#define FTM_FMS_FAULTF_SHIFT                     (7U)
#define FTM_FMS_FAULTF(x)                        (((uint32_t)(((uint32_t)(x)) << FTM_FMS_FAULTF_SHIFT)) & FTM_FMS_FAULTF_MASK)

/*! @name FILTER - Input Capture Filter Control */
#define FTM_FILTER_CH0FVAL_MASK                  (0xFU)
#define FTM_FILTER_CH0FVAL_SHIFT                 (0U)
#define FTM_FILTER_CH0FVAL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FILTER_CH0FVAL_SHIFT)) & FTM_FILTER_CH0FVAL_MASK)
#define FTM_FILTER_CH1FVAL_MASK                  (0xF0U)
#define FTM_FILTER_CH1FVAL_SHIFT                 (4U)
#define FTM_FILTER_CH1FVAL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FILTER_CH1FVAL_SHIFT)) & FTM_FILTER_CH1FVAL_MASK)
#define FTM_FILTER_CH2FVAL_MASK                  (0xF00U)
#define FTM_FILTER_CH2FVAL_SHIFT                 (8U)
#define FTM_FILTER_CH2FVAL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FILTER_CH2FVAL_SHIFT)) & FTM_FILTER_CH2FVAL_MASK)
#define FTM_FILTER_CH3FVAL_MASK                  (0xF000U)
#define FTM_FILTER_CH3FVAL_SHIFT                 (12U)
#define FTM_FILTER_CH3FVAL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FILTER_CH3FVAL_SHIFT)) & FTM_FILTER_CH3FVAL_MASK)

/*! @name FLTCTRL - Fault Control */
#define FTM_FLTCTRL_FAULT0EN_MASK                (0x1U)
#define FTM_FLTCTRL_FAULT0EN_SHIFT               (0U)
#define FTM_FLTCTRL_FAULT0EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FAULT0EN_SHIFT)) & FTM_FLTCTRL_FAULT0EN_MASK)
#define FTM_FLTCTRL_FAULT1EN_MASK                (0x2U)
#define FTM_FLTCTRL_FAULT1EN_SHIFT               (1U)
#define FTM_FLTCTRL_FAULT1EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FAULT1EN_SHIFT)) & FTM_FLTCTRL_FAULT1EN_MASK)
#define FTM_FLTCTRL_FAULT2EN_MASK                (0x4U)
#define FTM_FLTCTRL_FAULT2EN_SHIFT               (2U)
#define FTM_FLTCTRL_FAULT2EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FAULT2EN_SHIFT)) & FTM_FLTCTRL_FAULT2EN_MASK)
#define FTM_FLTCTRL_FAULT3EN_MASK                (0x8U)
#define FTM_FLTCTRL_FAULT3EN_SHIFT               (3U)
#define FTM_FLTCTRL_FAULT3EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FAULT3EN_SHIFT)) & FTM_FLTCTRL_FAULT3EN_MASK)
#define FTM_FLTCTRL_FFLTR0EN_MASK                (0x10U)
#define FTM_FLTCTRL_FFLTR0EN_SHIFT               (4U)
#define FTM_FLTCTRL_FFLTR0EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FFLTR0EN_SHIFT)) & FTM_FLTCTRL_FFLTR0EN_MASK)
#define FTM_FLTCTRL_FFLTR1EN_MASK                (0x20U)
#define FTM_FLTCTRL_FFLTR1EN_SHIFT               (5U)
#define FTM_FLTCTRL_FFLTR1EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FFLTR1EN_SHIFT)) & FTM_FLTCTRL_FFLTR1EN_MASK)
#define FTM_FLTCTRL_FFLTR2EN_MASK                (0x40U)
#define FTM_FLTCTRL_FFLTR2EN_SHIFT               (6U)
#define FTM_FLTCTRL_FFLTR2EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FFLTR2EN_SHIFT)) & FTM_FLTCTRL_FFLTR2EN_MASK)
#define FTM_FLTCTRL_FFLTR3EN_MASK                (0x80U)
#define FTM_FLTCTRL_FFLTR3EN_SHIFT               (7U)
#define FTM_FLTCTRL_FFLTR3EN(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FFLTR3EN_SHIFT)) & FTM_FLTCTRL_FFLTR3EN_MASK)
#define FTM_FLTCTRL_FFVAL_MASK                   (0xF00U)
#define FTM_FLTCTRL_FFVAL_SHIFT                  (8U)
#define FTM_FLTCTRL_FFVAL(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_FLTCTRL_FFVAL_SHIFT)) & FTM_FLTCTRL_FFVAL_MASK)

/*! @name CONF - Configuration */
#define FTM_CONF_NUMTOF_MASK                     (0x1FU)
#define FTM_CONF_NUMTOF_SHIFT                    (0U)
#define FTM_CONF_NUMTOF(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_CONF_NUMTOF_SHIFT)) & FTM_CONF_NUMTOF_MASK)
#define FTM_CONF_BDMMODE_MASK                    (0xC0U)
#define FTM_CONF_BDMMODE_SHIFT                   (6U)
#define FTM_CONF_BDMMODE(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_CONF_BDMMODE_SHIFT)) & FTM_CONF_BDMMODE_MASK)
#define FTM_CONF_GTBEEN_MASK                     (0x200U)
#define FTM_CONF_GTBEEN_SHIFT                    (9U)
#define FTM_CONF_GTBEEN(x)                       (((uint32_t)(((uint32_t)(x)) << FTM_CONF_GTBEEN_SHIFT)) & FTM_CONF_GTBEEN_MASK)
#define FTM_CONF_GTBEOUT_MASK                    (0x400U)
#define FTM_CONF_GTBEOUT_SHIFT                   (10U)
#define FTM_CONF_GTBEOUT(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_CONF_GTBEOUT_SHIFT)) & FTM_CONF_GTBEOUT_MASK)

/*! @name FLTPOL - FTM Fault Input Polarity */
#define FTM_FLTPOL_FLT0POL_MASK                  (0x1U)
#define FTM_FLTPOL_FLT0POL_SHIFT                 (0U)
#define FTM_FLTPOL_FLT0POL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FLTPOL_FLT0POL_SHIFT)) & FTM_FLTPOL_FLT0POL_MASK)
#define FTM_FLTPOL_FLT1POL_MASK                  (0x2U)
#define FTM_FLTPOL_FLT1POL_SHIFT                 (1U)
#define FTM_FLTPOL_FLT1POL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FLTPOL_FLT1POL_SHIFT)) & FTM_FLTPOL_FLT1POL_MASK)
#define FTM_FLTPOL_FLT2POL_MASK                  (0x4U)
#define FTM_FLTPOL_FLT2POL_SHIFT                 (2U)
#define FTM_FLTPOL_FLT2POL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FLTPOL_FLT2POL_SHIFT)) & FTM_FLTPOL_FLT2POL_MASK)
#define FTM_FLTPOL_FLT3POL_MASK                  (0x8U)
#define FTM_FLTPOL_FLT3POL_SHIFT                 (3U)
#define FTM_FLTPOL_FLT3POL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_FLTPOL_FLT3POL_SHIFT)) & FTM_FLTPOL_FLT3POL_MASK)

/*! @name SYNCONF - Synchronization Configuration */
#define FTM_SYNCONF_HWTRIGMODE_MASK              (0x1U)
#define FTM_SYNCONF_HWTRIGMODE_SHIFT             (0U)
#define FTM_SYNCONF_HWTRIGMODE(x)                (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_HWTRIGMODE_SHIFT)) & FTM_SYNCONF_HWTRIGMODE_MASK)
#define FTM_SYNCONF_CNTINC_MASK                  (0x4U)
#define FTM_SYNCONF_CNTINC_SHIFT                 (2U)
#define FTM_SYNCONF_CNTINC(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_CNTINC_SHIFT)) & FTM_SYNCONF_CNTINC_MASK)
#define FTM_SYNCONF_INVC_MASK                    (0x10U)
#define FTM_SYNCONF_INVC_SHIFT                   (4U)
#define FTM_SYNCONF_INVC(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_INVC_SHIFT)) & FTM_SYNCONF_INVC_MASK)
#define FTM_SYNCONF_SWOC_MASK                    (0x20U)
#define FTM_SYNCONF_SWOC_SHIFT                   (5U)
#define FTM_SYNCONF_SWOC(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SWOC_SHIFT)) & FTM_SYNCONF_SWOC_MASK)
#define FTM_SYNCONF_SYNCMODE_MASK                (0x80U)
#define FTM_SYNCONF_SYNCMODE_SHIFT               (7U)
#define FTM_SYNCONF_SYNCMODE(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SYNCMODE_SHIFT)) & FTM_SYNCONF_SYNCMODE_MASK)
#define FTM_SYNCONF_SWRSTCNT_MASK                (0x100U)
#define FTM_SYNCONF_SWRSTCNT_SHIFT               (8U)
#define FTM_SYNCONF_SWRSTCNT(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SWRSTCNT_SHIFT)) & FTM_SYNCONF_SWRSTCNT_MASK)
#define FTM_SYNCONF_SWWRBUF_MASK                 (0x200U)
#define FTM_SYNCONF_SWWRBUF_SHIFT                (9U)
#define FTM_SYNCONF_SWWRBUF(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SWWRBUF_SHIFT)) & FTM_SYNCONF_SWWRBUF_MASK)
#define FTM_SYNCONF_SWOM_MASK                    (0x400U)
#define FTM_SYNCONF_SWOM_SHIFT                   (10U)
#define FTM_SYNCONF_SWOM(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SWOM_SHIFT)) & FTM_SYNCONF_SWOM_MASK)
#define FTM_SYNCONF_SWINVC_MASK                  (0x800U)
#define FTM_SYNCONF_SWINVC_SHIFT                 (11U)
#define FTM_SYNCONF_SWINVC(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SWINVC_SHIFT)) & FTM_SYNCONF_SWINVC_MASK)
#define FTM_SYNCONF_SWSOC_MASK                   (0x1000U)
#define FTM_SYNCONF_SWSOC_SHIFT                  (12U)
#define FTM_SYNCONF_SWSOC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_SWSOC_SHIFT)) & FTM_SYNCONF_SWSOC_MASK)
#define FTM_SYNCONF_HWRSTCNT_MASK                (0x10000U)
#define FTM_SYNCONF_HWRSTCNT_SHIFT               (16U)
#define FTM_SYNCONF_HWRSTCNT(x)                  (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_HWRSTCNT_SHIFT)) & FTM_SYNCONF_HWRSTCNT_MASK)
#define FTM_SYNCONF_HWWRBUF_MASK                 (0x20000U)
#define FTM_SYNCONF_HWWRBUF_SHIFT                (17U)
#define FTM_SYNCONF_HWWRBUF(x)                   (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_HWWRBUF_SHIFT)) & FTM_SYNCONF_HWWRBUF_MASK)
#define FTM_SYNCONF_HWOM_MASK                    (0x40000U)
#define FTM_SYNCONF_HWOM_SHIFT                   (18U)
#define FTM_SYNCONF_HWOM(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_HWOM_SHIFT)) & FTM_SYNCONF_HWOM_MASK)
#define FTM_SYNCONF_HWINVC_MASK                  (0x80000U)
#define FTM_SYNCONF_HWINVC_SHIFT                 (19U)
#define FTM_SYNCONF_HWINVC(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_HWINVC_SHIFT)) & FTM_SYNCONF_HWINVC_MASK)
#define FTM_SYNCONF_HWSOC_MASK                   (0x100000U)
#define FTM_SYNCONF_HWSOC_SHIFT                  (20U)
#define FTM_SYNCONF_HWSOC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SYNCONF_HWSOC_SHIFT)) & FTM_SYNCONF_HWSOC_MASK)

/*! @name INVCTRL - FTM Inverting Control */
#define FTM_INVCTRL_INV0EN_MASK                  (0x1U)
#define FTM_INVCTRL_INV0EN_SHIFT                 (0U)
#define FTM_INVCTRL_INV0EN(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_INVCTRL_INV0EN_SHIFT)) & FTM_INVCTRL_INV0EN_MASK)
#define FTM_INVCTRL_INV1EN_MASK                  (0x2U)
#define FTM_INVCTRL_INV1EN_SHIFT                 (1U)
#define FTM_INVCTRL_INV1EN(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_INVCTRL_INV1EN_SHIFT)) & FTM_INVCTRL_INV1EN_MASK)
#define FTM_INVCTRL_INV2EN_MASK                  (0x4U)
#define FTM_INVCTRL_INV2EN_SHIFT                 (2U)
#define FTM_INVCTRL_INV2EN(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_INVCTRL_INV2EN_SHIFT)) & FTM_INVCTRL_INV2EN_MASK)
#define FTM_INVCTRL_INV3EN_MASK                  (0x8U)
#define FTM_INVCTRL_INV3EN_SHIFT                 (3U)
#define FTM_INVCTRL_INV3EN(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_INVCTRL_INV3EN_SHIFT)) & FTM_INVCTRL_INV3EN_MASK)

/*! @name SWOCTRL - FTM Software Output Control */
#define FTM_SWOCTRL_CH0OC_MASK                   (0x1U)
#define FTM_SWOCTRL_CH0OC_SHIFT                  (0U)
#define FTM_SWOCTRL_CH0OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH0OC_SHIFT)) & FTM_SWOCTRL_CH0OC_MASK)
#define FTM_SWOCTRL_CH1OC_MASK                   (0x2U)
#define FTM_SWOCTRL_CH1OC_SHIFT                  (1U)
#define FTM_SWOCTRL_CH1OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH1OC_SHIFT)) & FTM_SWOCTRL_CH1OC_MASK)
#define FTM_SWOCTRL_CH2OC_MASK                   (0x4U)
#define FTM_SWOCTRL_CH2OC_SHIFT                  (2U)
#define FTM_SWOCTRL_CH2OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH2OC_SHIFT)) & FTM_SWOCTRL_CH2OC_MASK)
#define FTM_SWOCTRL_CH3OC_MASK                   (0x8U)
#define FTM_SWOCTRL_CH3OC_SHIFT                  (3U)
#define FTM_SWOCTRL_CH3OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH3OC_SHIFT)) & FTM_SWOCTRL_CH3OC_MASK)
#define FTM_SWOCTRL_CH4OC_MASK                   (0x10U)
#define FTM_SWOCTRL_CH4OC_SHIFT                  (4U)
#define FTM_SWOCTRL_CH4OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH4OC_SHIFT)) & FTM_SWOCTRL_CH4OC_MASK)
#define FTM_SWOCTRL_CH5OC_MASK                   (0x20U)
#define FTM_SWOCTRL_CH5OC_SHIFT                  (5U)
#define FTM_SWOCTRL_CH5OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH5OC_SHIFT)) & FTM_SWOCTRL_CH5OC_MASK)
#define FTM_SWOCTRL_CH6OC_MASK                   (0x40U)
#define FTM_SWOCTRL_CH6OC_SHIFT                  (6U)
#define FTM_SWOCTRL_CH6OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH6OC_SHIFT)) & FTM_SWOCTRL_CH6OC_MASK)
#define FTM_SWOCTRL_CH7OC_MASK                   (0x80U)
#define FTM_SWOCTRL_CH7OC_SHIFT                  (7U)
#define FTM_SWOCTRL_CH7OC(x)                     (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH7OC_SHIFT)) & FTM_SWOCTRL_CH7OC_MASK)
#define FTM_SWOCTRL_CH0OCV_MASK                  (0x100U)
#define FTM_SWOCTRL_CH0OCV_SHIFT                 (8U)
#define FTM_SWOCTRL_CH0OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH0OCV_SHIFT)) & FTM_SWOCTRL_CH0OCV_MASK)
#define FTM_SWOCTRL_CH1OCV_MASK                  (0x200U)
#define FTM_SWOCTRL_CH1OCV_SHIFT                 (9U)
#define FTM_SWOCTRL_CH1OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH1OCV_SHIFT)) & FTM_SWOCTRL_CH1OCV_MASK)
#define FTM_SWOCTRL_CH2OCV_MASK                  (0x400U)
#define FTM_SWOCTRL_CH2OCV_SHIFT                 (10U)
#define FTM_SWOCTRL_CH2OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH2OCV_SHIFT)) & FTM_SWOCTRL_CH2OCV_MASK)
#define FTM_SWOCTRL_CH3OCV_MASK                  (0x800U)
#define FTM_SWOCTRL_CH3OCV_SHIFT                 (11U)
#define FTM_SWOCTRL_CH3OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH3OCV_SHIFT)) & FTM_SWOCTRL_CH3OCV_MASK)
#define FTM_SWOCTRL_CH4OCV_MASK                  (0x1000U)
#define FTM_SWOCTRL_CH4OCV_SHIFT                 (12U)
#define FTM_SWOCTRL_CH4OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH4OCV_SHIFT)) & FTM_SWOCTRL_CH4OCV_MASK)
#define FTM_SWOCTRL_CH5OCV_MASK                  (0x2000U)
#define FTM_SWOCTRL_CH5OCV_SHIFT                 (13U)
#define FTM_SWOCTRL_CH5OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH5OCV_SHIFT)) & FTM_SWOCTRL_CH5OCV_MASK)
#define FTM_SWOCTRL_CH6OCV_MASK                  (0x4000U)
#define FTM_SWOCTRL_CH6OCV_SHIFT                 (14U)
#define FTM_SWOCTRL_CH6OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH6OCV_SHIFT)) & FTM_SWOCTRL_CH6OCV_MASK)
#define FTM_SWOCTRL_CH7OCV_MASK                  (0x8000U)
#define FTM_SWOCTRL_CH7OCV_SHIFT                 (15U)
#define FTM_SWOCTRL_CH7OCV(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_SWOCTRL_CH7OCV_SHIFT)) & FTM_SWOCTRL_CH7OCV_MASK)

/*! @name PWMLOAD - FTM PWM Load */
#define FTM_PWMLOAD_CH0SEL_MASK                  (0x1U)
#define FTM_PWMLOAD_CH0SEL_SHIFT                 (0U)
#define FTM_PWMLOAD_CH0SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH0SEL_SHIFT)) & FTM_PWMLOAD_CH0SEL_MASK)
#define FTM_PWMLOAD_CH1SEL_MASK                  (0x2U)
#define FTM_PWMLOAD_CH1SEL_SHIFT                 (1U)
#define FTM_PWMLOAD_CH1SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH1SEL_SHIFT)) & FTM_PWMLOAD_CH1SEL_MASK)
#define FTM_PWMLOAD_CH2SEL_MASK                  (0x4U)
#define FTM_PWMLOAD_CH2SEL_SHIFT                 (2U)
#define FTM_PWMLOAD_CH2SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH2SEL_SHIFT)) & FTM_PWMLOAD_CH2SEL_MASK)
#define FTM_PWMLOAD_CH3SEL_MASK                  (0x8U)
#define FTM_PWMLOAD_CH3SEL_SHIFT                 (3U)
#define FTM_PWMLOAD_CH3SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH3SEL_SHIFT)) & FTM_PWMLOAD_CH3SEL_MASK)
#define FTM_PWMLOAD_CH4SEL_MASK                  (0x10U)
#define FTM_PWMLOAD_CH4SEL_SHIFT                 (4U)
#define FTM_PWMLOAD_CH4SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH4SEL_SHIFT)) & FTM_PWMLOAD_CH4SEL_MASK)
#define FTM_PWMLOAD_CH5SEL_MASK                  (0x20U)
#define FTM_PWMLOAD_CH5SEL_SHIFT                 (5U)
#define FTM_PWMLOAD_CH5SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH5SEL_SHIFT)) & FTM_PWMLOAD_CH5SEL_MASK)
#define FTM_PWMLOAD_CH6SEL_MASK                  (0x40U)
#define FTM_PWMLOAD_CH6SEL_SHIFT                 (6U)
#define FTM_PWMLOAD_CH6SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH6SEL_SHIFT)) & FTM_PWMLOAD_CH6SEL_MASK)
#define FTM_PWMLOAD_CH7SEL_MASK                  (0x80U)
#define FTM_PWMLOAD_CH7SEL_SHIFT                 (7U)
#define FTM_PWMLOAD_CH7SEL(x)                    (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_CH7SEL_SHIFT)) & FTM_PWMLOAD_CH7SEL_MASK)
#define FTM_PWMLOAD_LDOK_MASK                    (0x200U)
#define FTM_PWMLOAD_LDOK_SHIFT                   (9U)
#define FTM_PWMLOAD_LDOK(x)                      (((uint32_t)(((uint32_t)(x)) << FTM_PWMLOAD_LDOK_SHIFT)) & FTM_PWMLOAD_LDOK_MASK)


/*!
 * @}
 */ /* end of group FTM_Register_Masks */


/* FTM - Peripheral instance base addresses */
/** Peripheral FTM0 base address */
#define FTM0_BASE                                (0x40038000u)
/** Peripheral FTM0 base pointer */
#define FTM0                                     ((FTM_Type *)FTM0_BASE)
/** Peripheral FTM1 base address */
#define FTM1_BASE                                (0x40039000u)
/** Peripheral FTM1 base pointer */
#define FTM1                                     ((FTM_Type *)FTM1_BASE)
/** Peripheral FTM2 base address */
#define FTM2_BASE                                (0x4003A000u)
/** Peripheral FTM2 base pointer */
#define FTM2                                     ((FTM_Type *)FTM2_BASE)
/** Array initializer of FTM peripheral base addresses */
#define FTM_BASE_ADDRS                           { FTM0_BASE, FTM1_BASE, FTM2_BASE }
/** Array initializer of FTM peripheral base pointers */
#define FTM_BASE_PTRS                            { FTM0, FTM1, FTM2 }
/** Interrupt vectors for the FTM peripheral type */
#define FTM_IRQS                                 { FTM0_IRQn, FTM1_IRQn, FTM2_IRQn }
/* Backward compatibility */
/*! @name SC - Status And Control */
#define TPM_SC_PS_MASK                           FTM_SC_PS_MASK
#define TPM_SC_PS_SHIFT                          FTM_SC_PS_SHIFT
#define TPM_SC_PS(x)                             FTM_SC_PS(x)
#define TPM_SC_CLKS_MASK                         FTM_SC_CLKS_MASK
#define TPM_SC_CLKS_SHIFT                        FTM_SC_CLKS_SHIFT
#define TPM_SC_CLKS(x)                           FTM_SC_CLKS(x)
#define TPM_SC_CPWMS_MASK                        FTM_SC_CPWMS_MASK
#define TPM_SC_CPWMS_SHIFT                       FTM_SC_CPWMS_SHIFT
#define TPM_SC_CPWMS(x)                          FTM_SC_CPWMS(x)
#define TPM_SC_TOIE_MASK                         FTM_SC_TOIE_MASK
#define TPM_SC_TOIE_SHIFT                        FTM_SC_TOIE_SHIFT
#define TPM_SC_TOIE(x)                           FTM_SC_TOIE(x)
#define TPM_SC_TOF_MASK                          FTM_SC_TOF_MASK
#define TPM_SC_TOF_SHIFT                         FTM_SC_TOF_SHIFT
#define TPM_SC_TOF(x)                            FTM_SC_TOF(x)
/*! @name CNT - Counter */
#define TPM_CNT_COUNT_MASK                       FTM_CNT_COUNT_MASK
#define TPM_CNT_COUNT_SHIFT                      FTM_CNT_COUNT_SHIFT
#define TPM_CNT_COUNT(x)                         FTM_CNT_COUNT(x)
/*! @name MOD - Modulo */
#define TPM_MOD_MOD_MASK                         FTM_MOD_MOD_MASK
#define TPM_MOD_MOD_SHIFT                        FTM_MOD_MOD_SHIFT
#define TPM_MOD_MOD(x)                           FTM_MOD_MOD(x)
/*! @name CnSC - Channel (n) Status And Control */
#define TPM_CnSC_ELSA_MASK                       FTM_CnSC_ELSA_MASK
#define TPM_CnSC_ELSA_SHIFT                      FTM_CnSC_ELSA_SHIFT
#define TPM_CnSC_ELSA(x)                         FTM_CnSC_ELSA(x)
#define TPM_CnSC_ELSB_MASK                       FTM_CnSC_ELSB_MASK
#define TPM_CnSC_ELSB_SHIFT                      FTM_CnSC_ELSB_SHIFT
#define TPM_CnSC_ELSB(x)                         FTM_CnSC_ELSB(x)
#define TPM_CnSC_MSA_MASK                        FTM_CnSC_MSA_MASK
#define TPM_CnSC_MSA_SHIFT                       FTM_CnSC_MSA_SHIFT
#define TPM_CnSC_MSA(x)                          FTM_CnSC_MSA(x)
#define TPM_CnSC_MSB_MASK                        FTM_CnSC_MSB_MASK
#define TPM_CnSC_MSB_SHIFT                       FTM_CnSC_MSB_SHIFT
#define TPM_CnSC_MSB(x)                          FTM_CnSC_MSB(x)
#define TPM_CnSC_CHIE_MASK                       FTM_CnSC_CHIE_MASK
#define TPM_CnSC_CHIE_SHIFT                      FTM_CnSC_CHIE_SHIFT
#define TPM_CnSC_CHIE(x)                         FTM_CnSC_CHIE(x)
#define TPM_CnSC_CHF_MASK                        FTM_CnSC_CHF_MASK
#define TPM_CnSC_CHF_SHIFT                       FTM_CnSC_CHF_SHIFT
#define TPM_CnSC_CHF(x)                          FTM_CnSC_CHF(x)
/* The count of FTM_CnSC */
#define TPM_CnSC_COUNT                          (2U)
/*! @name CnV - Channel (n) Value */
#define TPM_CnV_VAL_MASK                         FTM_CnV_VAL_MASK
#define TPM_CnV_VAL_SHIFT                        FTM_CnV_VAL_SHIFT
#define TPM_CnV_VAL(x)                           FTM_CnV_VAL(x)
/* The count of FTM_CnSC */
#define TPM_CnV_COUNT                           (2U)
/** TPM - Register Layout Typedef */
typedef FTM_Type  TPM_Type;
#define TPM0_IRQn                                FTM0_IRQn
#define TPM0_IRQHandler                          FTM0_IRQHandler
#define TPM1_IRQn                                FTM1_IRQn
#define TPM1_IRQHandler                          FTM1_IRQHandler
#define TPM_CLOCKS                               FTM_CLOCKS
/* TPM - Peripheral instance base addresses */
/** Peripheral TPM0 base address */
#define TPM0_BASE                                FTM0_BASE
/** Peripheral TPM0 base pointer */
#define TPM0                                   ((TPM_Type *)TPM0_BASE)
/** Peripheral TPM1 base address */
#define TPM1_BASE                                FTM1_BASE
/** Peripheral TPM1 base pointer */
#define TPM1                                   ((TPM_Type *)TPM1_BASE)
/** Array initializer of TPM peripheral base addresses */
#define TPM_BASE_ADDRS                         { TPM0_BASE, TPM1_BASE }
/** Array initializer of TPM peripheral base pointers */
#define TPM_BASE_PTRS                          { TPM0, TPM1 }
/** Interrupt vectors for the TPM peripheral type */
#define TPM_IRQS                               { TPM0_IRQn, TPM1_IRQn }


/*!
 * @}
 */ /* end of group FTM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FTMRH Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTMRH_Peripheral_Access_Layer FTMRH Peripheral Access Layer
 * @{
 */

/** FTMRH - Register Layout Typedef */
typedef struct {
  __IO uint8_t FCLKDIV;                            /**< Flash Clock Divider Register, offset: 0x0 */
  __I  uint8_t FSEC;                               /**< Flash Security Register, offset: 0x1 */
  __IO uint8_t FCCOBIX;                            /**< Flash CCOB Index Register, offset: 0x2 */
       uint8_t RESERVED_0[1];
  __IO uint8_t FCNFG;                              /**< Flash Configuration Register, offset: 0x4 */
  __IO uint8_t FERCNFG;                            /**< Flash Error Configuration Register, offset: 0x5 */
  __IO uint8_t FSTAT;                              /**< Flash Status Register, offset: 0x6 */
  __IO uint8_t FERSTAT;                            /**< Flash Error Status Register, offset: 0x7 */
  __IO uint8_t FPROT;                              /**< Flash Protection Register, offset: 0x8 */
  __IO uint8_t EEPROT;                             /**< EEPROM Protection Register, offset: 0x9 */
  __IO uint8_t FCCOBHI;                            /**< Flash Common Command Object Register:High, offset: 0xA */
  __IO uint8_t FCCOBLO;                            /**< Flash Common Command Object Register: Low, offset: 0xB */
  __I  uint8_t FOPT;                               /**< Flash Option Register, offset: 0xC */
} FTMRH_Type;

/* ----------------------------------------------------------------------------
   -- FTMRH Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTMRH_Register_Masks FTMRH Register Masks
 * @{
 */

/*! @name FCLKDIV - Flash Clock Divider Register */
#define FTMRH_FCLKDIV_FDIV_MASK                  (0x3FU)
#define FTMRH_FCLKDIV_FDIV_SHIFT                 (0U)
#define FTMRH_FCLKDIV_FDIV(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FCLKDIV_FDIV_SHIFT)) & FTMRH_FCLKDIV_FDIV_MASK)
#define FTMRH_FCLKDIV_FDIVLCK_MASK               (0x40U)
#define FTMRH_FCLKDIV_FDIVLCK_SHIFT              (6U)
#define FTMRH_FCLKDIV_FDIVLCK(x)                 (((uint8_t)(((uint8_t)(x)) << FTMRH_FCLKDIV_FDIVLCK_SHIFT)) & FTMRH_FCLKDIV_FDIVLCK_MASK)
#define FTMRH_FCLKDIV_FDIVLD_MASK                (0x80U)
#define FTMRH_FCLKDIV_FDIVLD_SHIFT               (7U)
#define FTMRH_FCLKDIV_FDIVLD(x)                  (((uint8_t)(((uint8_t)(x)) << FTMRH_FCLKDIV_FDIVLD_SHIFT)) & FTMRH_FCLKDIV_FDIVLD_MASK)

/*! @name FSEC - Flash Security Register */
#define FTMRH_FSEC_SEC_MASK                      (0x3U)
#define FTMRH_FSEC_SEC_SHIFT                     (0U)
#define FTMRH_FSEC_SEC(x)                        (((uint8_t)(((uint8_t)(x)) << FTMRH_FSEC_SEC_SHIFT)) & FTMRH_FSEC_SEC_MASK)
#define FTMRH_FSEC_KEYEN_MASK                    (0xC0U)
#define FTMRH_FSEC_KEYEN_SHIFT                   (6U)
#define FTMRH_FSEC_KEYEN(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FSEC_KEYEN_SHIFT)) & FTMRH_FSEC_KEYEN_MASK)

/*! @name FCCOBIX - Flash CCOB Index Register */
#define FTMRH_FCCOBIX_CCOBIX_MASK                (0x7U)
#define FTMRH_FCCOBIX_CCOBIX_SHIFT               (0U)
#define FTMRH_FCCOBIX_CCOBIX(x)                  (((uint8_t)(((uint8_t)(x)) << FTMRH_FCCOBIX_CCOBIX_SHIFT)) & FTMRH_FCCOBIX_CCOBIX_MASK)

/*! @name FCNFG - Flash Configuration Register */
#define FTMRH_FCNFG_FSFD_MASK                    (0x1U)
#define FTMRH_FCNFG_FSFD_SHIFT                   (0U)
#define FTMRH_FCNFG_FSFD(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FCNFG_FSFD_SHIFT)) & FTMRH_FCNFG_FSFD_MASK)
#define FTMRH_FCNFG_FDFD_MASK                    (0x2U)
#define FTMRH_FCNFG_FDFD_SHIFT                   (1U)
#define FTMRH_FCNFG_FDFD(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FCNFG_FDFD_SHIFT)) & FTMRH_FCNFG_FDFD_MASK)
#define FTMRH_FCNFG_IGNSF_MASK                   (0x10U)
#define FTMRH_FCNFG_IGNSF_SHIFT                  (4U)
#define FTMRH_FCNFG_IGNSF(x)                     (((uint8_t)(((uint8_t)(x)) << FTMRH_FCNFG_IGNSF_SHIFT)) & FTMRH_FCNFG_IGNSF_MASK)
#define FTMRH_FCNFG_CCIE_MASK                    (0x80U)
#define FTMRH_FCNFG_CCIE_SHIFT                   (7U)
#define FTMRH_FCNFG_CCIE(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FCNFG_CCIE_SHIFT)) & FTMRH_FCNFG_CCIE_MASK)

/*! @name FERCNFG - Flash Error Configuration Register */
#define FTMRH_FERCNFG_SFDIE_MASK                 (0x1U)
#define FTMRH_FERCNFG_SFDIE_SHIFT                (0U)
#define FTMRH_FERCNFG_SFDIE(x)                   (((uint8_t)(((uint8_t)(x)) << FTMRH_FERCNFG_SFDIE_SHIFT)) & FTMRH_FERCNFG_SFDIE_MASK)
#define FTMRH_FERCNFG_DFDIE_MASK                 (0x2U)
#define FTMRH_FERCNFG_DFDIE_SHIFT                (1U)
#define FTMRH_FERCNFG_DFDIE(x)                   (((uint8_t)(((uint8_t)(x)) << FTMRH_FERCNFG_DFDIE_SHIFT)) & FTMRH_FERCNFG_DFDIE_MASK)

/*! @name FSTAT - Flash Status Register */
#define FTMRH_FSTAT_MGSTAT_MASK                  (0x3U)
#define FTMRH_FSTAT_MGSTAT_SHIFT                 (0U)
#define FTMRH_FSTAT_MGSTAT(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FSTAT_MGSTAT_SHIFT)) & FTMRH_FSTAT_MGSTAT_MASK)
#define FTMRH_FSTAT_MGBUSY_MASK                  (0x8U)
#define FTMRH_FSTAT_MGBUSY_SHIFT                 (3U)
#define FTMRH_FSTAT_MGBUSY(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FSTAT_MGBUSY_SHIFT)) & FTMRH_FSTAT_MGBUSY_MASK)
#define FTMRH_FSTAT_FPVIOL_MASK                  (0x10U)
#define FTMRH_FSTAT_FPVIOL_SHIFT                 (4U)
#define FTMRH_FSTAT_FPVIOL(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FSTAT_FPVIOL_SHIFT)) & FTMRH_FSTAT_FPVIOL_MASK)
#define FTMRH_FSTAT_ACCERR_MASK                  (0x20U)
#define FTMRH_FSTAT_ACCERR_SHIFT                 (5U)
#define FTMRH_FSTAT_ACCERR(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FSTAT_ACCERR_SHIFT)) & FTMRH_FSTAT_ACCERR_MASK)
#define FTMRH_FSTAT_CCIF_MASK                    (0x80U)
#define FTMRH_FSTAT_CCIF_SHIFT                   (7U)
#define FTMRH_FSTAT_CCIF(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FSTAT_CCIF_SHIFT)) & FTMRH_FSTAT_CCIF_MASK)

/*! @name FERSTAT - Flash Error Status Register */
#define FTMRH_FERSTAT_SFDIF_MASK                 (0x1U)
#define FTMRH_FERSTAT_SFDIF_SHIFT                (0U)
#define FTMRH_FERSTAT_SFDIF(x)                   (((uint8_t)(((uint8_t)(x)) << FTMRH_FERSTAT_SFDIF_SHIFT)) & FTMRH_FERSTAT_SFDIF_MASK)
#define FTMRH_FERSTAT_DFDIF_MASK                 (0x2U)
#define FTMRH_FERSTAT_DFDIF_SHIFT                (1U)
#define FTMRH_FERSTAT_DFDIF(x)                   (((uint8_t)(((uint8_t)(x)) << FTMRH_FERSTAT_DFDIF_SHIFT)) & FTMRH_FERSTAT_DFDIF_MASK)

/*! @name FPROT - Flash Protection Register */
#define FTMRH_FPROT_FPLS_MASK                    (0x3U)
#define FTMRH_FPROT_FPLS_SHIFT                   (0U)
#define FTMRH_FPROT_FPLS(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FPROT_FPLS_SHIFT)) & FTMRH_FPROT_FPLS_MASK)
#define FTMRH_FPROT_FPLDIS_MASK                  (0x4U)
#define FTMRH_FPROT_FPLDIS_SHIFT                 (2U)
#define FTMRH_FPROT_FPLDIS(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FPROT_FPLDIS_SHIFT)) & FTMRH_FPROT_FPLDIS_MASK)
#define FTMRH_FPROT_FPHS_MASK                    (0x18U)
#define FTMRH_FPROT_FPHS_SHIFT                   (3U)
#define FTMRH_FPROT_FPHS(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FPROT_FPHS_SHIFT)) & FTMRH_FPROT_FPHS_MASK)
#define FTMRH_FPROT_FPHDIS_MASK                  (0x20U)
#define FTMRH_FPROT_FPHDIS_SHIFT                 (5U)
#define FTMRH_FPROT_FPHDIS(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FPROT_FPHDIS_SHIFT)) & FTMRH_FPROT_FPHDIS_MASK)
#define FTMRH_FPROT_RNV6_MASK                    (0x40U)
#define FTMRH_FPROT_RNV6_SHIFT                   (6U)
#define FTMRH_FPROT_RNV6(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_FPROT_RNV6_SHIFT)) & FTMRH_FPROT_RNV6_MASK)
#define FTMRH_FPROT_FPOPEN_MASK                  (0x80U)
#define FTMRH_FPROT_FPOPEN_SHIFT                 (7U)
#define FTMRH_FPROT_FPOPEN(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FPROT_FPOPEN_SHIFT)) & FTMRH_FPROT_FPOPEN_MASK)

/*! @name EEPROT - EEPROM Protection Register */
#define FTMRH_EEPROT_DPS_MASK                    (0x7U)
#define FTMRH_EEPROT_DPS_SHIFT                   (0U)
#define FTMRH_EEPROT_DPS(x)                      (((uint8_t)(((uint8_t)(x)) << FTMRH_EEPROT_DPS_SHIFT)) & FTMRH_EEPROT_DPS_MASK)
#define FTMRH_EEPROT_DPOPEN_MASK                 (0x80U)
#define FTMRH_EEPROT_DPOPEN_SHIFT                (7U)
#define FTMRH_EEPROT_DPOPEN(x)                   (((uint8_t)(((uint8_t)(x)) << FTMRH_EEPROT_DPOPEN_SHIFT)) & FTMRH_EEPROT_DPOPEN_MASK)

/*! @name FCCOBHI - Flash Common Command Object Register:High */
#define FTMRH_FCCOBHI_CCOB_MASK                  (0xFFU)
#define FTMRH_FCCOBHI_CCOB_SHIFT                 (0U)
#define FTMRH_FCCOBHI_CCOB(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FCCOBHI_CCOB_SHIFT)) & FTMRH_FCCOBHI_CCOB_MASK)

/*! @name FCCOBLO - Flash Common Command Object Register: Low */
#define FTMRH_FCCOBLO_CCOB_MASK                  (0xFFU)
#define FTMRH_FCCOBLO_CCOB_SHIFT                 (0U)
#define FTMRH_FCCOBLO_CCOB(x)                    (((uint8_t)(((uint8_t)(x)) << FTMRH_FCCOBLO_CCOB_SHIFT)) & FTMRH_FCCOBLO_CCOB_MASK)

/*! @name FOPT - Flash Option Register */
#define FTMRH_FOPT_NV_MASK                       (0xFFU)
#define FTMRH_FOPT_NV_SHIFT                      (0U)
#define FTMRH_FOPT_NV(x)                         (((uint8_t)(((uint8_t)(x)) << FTMRH_FOPT_NV_SHIFT)) & FTMRH_FOPT_NV_MASK)


/*!
 * @}
 */ /* end of group FTMRH_Register_Masks */


/* FTMRH - Peripheral instance base addresses */
/** Peripheral FTMRH base address */
#define FTMRH_BASE                               (0x40020000u)
/** Peripheral FTMRH base pointer */
#define FTMRH                                    ((FTMRH_Type *)FTMRH_BASE)
/** Array initializer of FTMRH peripheral base addresses */
#define FTMRH_BASE_ADDRS                         { FTMRH_BASE }
/** Array initializer of FTMRH peripheral base pointers */
#define FTMRH_BASE_PTRS                          { FTMRH }
/** Interrupt vectors for the FTMRH peripheral type */
#define FTMRH_IRQS                               { FTMRH_IRQn }

/*!
 * @}
 */ /* end of group FTMRH_Peripheral_Access_Layer */


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
} GPIO_Type;

/* ----------------------------------------------------------------------------
   -- GPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Masks GPIO Register Masks
 * @{
 */

/*! @name PDOR - Port Data Output Register */
#define GPIO_PDOR_PDO_MASK                       (0xFFFFFFFFU)
#define GPIO_PDOR_PDO_SHIFT                      (0U)
#define GPIO_PDOR_PDO(x)                         (((uint32_t)(((uint32_t)(x)) << GPIO_PDOR_PDO_SHIFT)) & GPIO_PDOR_PDO_MASK)

/*! @name PSOR - Port Set Output Register */
#define GPIO_PSOR_PTSO_MASK                      (0xFFFFFFFFU)
#define GPIO_PSOR_PTSO_SHIFT                     (0U)
#define GPIO_PSOR_PTSO(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_PSOR_PTSO_SHIFT)) & GPIO_PSOR_PTSO_MASK)

/*! @name PCOR - Port Clear Output Register */
#define GPIO_PCOR_PTCO_MASK                      (0xFFFFFFFFU)
#define GPIO_PCOR_PTCO_SHIFT                     (0U)
#define GPIO_PCOR_PTCO(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_PCOR_PTCO_SHIFT)) & GPIO_PCOR_PTCO_MASK)

/*! @name PTOR - Port Toggle Output Register */
#define GPIO_PTOR_PTTO_MASK                      (0xFFFFFFFFU)
#define GPIO_PTOR_PTTO_SHIFT                     (0U)
#define GPIO_PTOR_PTTO(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_PTOR_PTTO_SHIFT)) & GPIO_PTOR_PTTO_MASK)

/*! @name PDIR - Port Data Input Register */
#define GPIO_PDIR_PDI_MASK                       (0xFFFFFFFFU)
#define GPIO_PDIR_PDI_SHIFT                      (0U)
#define GPIO_PDIR_PDI(x)                         (((uint32_t)(((uint32_t)(x)) << GPIO_PDIR_PDI_SHIFT)) & GPIO_PDIR_PDI_MASK)

/*! @name PDDR - Port Data Direction Register */
#define GPIO_PDDR_PDD_MASK                       (0xFFFFFFFFU)
#define GPIO_PDDR_PDD_SHIFT                      (0U)
#define GPIO_PDDR_PDD(x)                         (((uint32_t)(((uint32_t)(x)) << GPIO_PDDR_PDD_SHIFT)) & GPIO_PDDR_PDD_MASK)

/*! @name PIDR - Port Input Disable Register */
#define GPIO_PIDR_PID_MASK                       (0xFFFFFFFFU)
#define GPIO_PIDR_PID_SHIFT                      (0U)
#define GPIO_PIDR_PID(x)                         (((uint32_t)(((uint32_t)(x)) << GPIO_PIDR_PID_SHIFT)) & GPIO_PIDR_PID_MASK)


/*!
 * @}
 */ /* end of group GPIO_Register_Masks */


/* GPIO - Peripheral instance base addresses */
/** Peripheral GPIOA base address */
#define GPIOA_BASE                               (0x400FF000u)
/** Peripheral GPIOA base pointer */
#define GPIOA                                    ((GPIO_Type *)GPIOA_BASE)
/** Peripheral GPIOB base address */
#define GPIOB_BASE                               (0x400FF040u)
/** Peripheral GPIOB base pointer */
#define GPIOB                                    ((GPIO_Type *)GPIOB_BASE)
/** Array initializer of GPIO peripheral base addresses */
#define GPIO_BASE_ADDRS                          { GPIOA_BASE, GPIOB_BASE }
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASE_PTRS                           { GPIOA, GPIOB }

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
  __IO uint8_t S;                                  /**< I2C Status register, offset: 0x3 */
  __IO uint8_t D;                                  /**< I2C Data I/O register, offset: 0x4 */
  __IO uint8_t C2;                                 /**< I2C Control Register 2, offset: 0x5 */
  __IO uint8_t FLT;                                /**< I2C Programmable Input Glitch Filter Register, offset: 0x6 */
  __IO uint8_t RA;                                 /**< I2C Range Address register, offset: 0x7 */
  __IO uint8_t SMB;                                /**< I2C SMBus Control and Status register, offset: 0x8 */
  __IO uint8_t A2;                                 /**< I2C Address Register 2, offset: 0x9 */
  __IO uint8_t SLTH;                               /**< I2C SCL Low Timeout Register High, offset: 0xA */
  __IO uint8_t SLTL;                               /**< I2C SCL Low Timeout Register Low, offset: 0xB */
} I2C_Type;

/* ----------------------------------------------------------------------------
   -- I2C Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Register_Masks I2C Register Masks
 * @{
 */

/*! @name A1 - I2C Address Register 1 */
#define I2C_A1_AD_MASK                           (0xFEU)
#define I2C_A1_AD_SHIFT                          (1U)
#define I2C_A1_AD(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_A1_AD_SHIFT)) & I2C_A1_AD_MASK)

/*! @name F - I2C Frequency Divider register */
#define I2C_F_ICR_MASK                           (0x3FU)
#define I2C_F_ICR_SHIFT                          (0U)
#define I2C_F_ICR(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_F_ICR_SHIFT)) & I2C_F_ICR_MASK)
#define I2C_F_MULT_MASK                          (0xC0U)
#define I2C_F_MULT_SHIFT                         (6U)
#define I2C_F_MULT(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_F_MULT_SHIFT)) & I2C_F_MULT_MASK)

/*! @name C1 - I2C Control Register 1 */
#define I2C_C1_WUEN_MASK                         (0x2U)
#define I2C_C1_WUEN_SHIFT                        (1U)
#define I2C_C1_WUEN(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_C1_WUEN_SHIFT)) & I2C_C1_WUEN_MASK)
#define I2C_C1_RSTA_MASK                         (0x4U)
#define I2C_C1_RSTA_SHIFT                        (2U)
#define I2C_C1_RSTA(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_C1_RSTA_SHIFT)) & I2C_C1_RSTA_MASK)
#define I2C_C1_TXAK_MASK                         (0x8U)
#define I2C_C1_TXAK_SHIFT                        (3U)
#define I2C_C1_TXAK(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_C1_TXAK_SHIFT)) & I2C_C1_TXAK_MASK)
#define I2C_C1_TX_MASK                           (0x10U)
#define I2C_C1_TX_SHIFT                          (4U)
#define I2C_C1_TX(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_C1_TX_SHIFT)) & I2C_C1_TX_MASK)
#define I2C_C1_MST_MASK                          (0x20U)
#define I2C_C1_MST_SHIFT                         (5U)
#define I2C_C1_MST(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_C1_MST_SHIFT)) & I2C_C1_MST_MASK)
#define I2C_C1_IICIE_MASK                        (0x40U)
#define I2C_C1_IICIE_SHIFT                       (6U)
#define I2C_C1_IICIE(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_C1_IICIE_SHIFT)) & I2C_C1_IICIE_MASK)
#define I2C_C1_IICEN_MASK                        (0x80U)
#define I2C_C1_IICEN_SHIFT                       (7U)
#define I2C_C1_IICEN(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_C1_IICEN_SHIFT)) & I2C_C1_IICEN_MASK)

/*! @name S - I2C Status register */
#define I2C_S_RXAK_MASK                          (0x1U)
#define I2C_S_RXAK_SHIFT                         (0U)
#define I2C_S_RXAK(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_S_RXAK_SHIFT)) & I2C_S_RXAK_MASK)
#define I2C_S_IICIF_MASK                         (0x2U)
#define I2C_S_IICIF_SHIFT                        (1U)
#define I2C_S_IICIF(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_S_IICIF_SHIFT)) & I2C_S_IICIF_MASK)
#define I2C_S_SRW_MASK                           (0x4U)
#define I2C_S_SRW_SHIFT                          (2U)
#define I2C_S_SRW(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_S_SRW_SHIFT)) & I2C_S_SRW_MASK)
#define I2C_S_RAM_MASK                           (0x8U)
#define I2C_S_RAM_SHIFT                          (3U)
#define I2C_S_RAM(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_S_RAM_SHIFT)) & I2C_S_RAM_MASK)
#define I2C_S_ARBL_MASK                          (0x10U)
#define I2C_S_ARBL_SHIFT                         (4U)
#define I2C_S_ARBL(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_S_ARBL_SHIFT)) & I2C_S_ARBL_MASK)
#define I2C_S_BUSY_MASK                          (0x20U)
#define I2C_S_BUSY_SHIFT                         (5U)
#define I2C_S_BUSY(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_S_BUSY_SHIFT)) & I2C_S_BUSY_MASK)
#define I2C_S_IAAS_MASK                          (0x40U)
#define I2C_S_IAAS_SHIFT                         (6U)
#define I2C_S_IAAS(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_S_IAAS_SHIFT)) & I2C_S_IAAS_MASK)
#define I2C_S_TCF_MASK                           (0x80U)
#define I2C_S_TCF_SHIFT                          (7U)
#define I2C_S_TCF(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_S_TCF_SHIFT)) & I2C_S_TCF_MASK)

/*! @name D - I2C Data I/O register */
#define I2C_D_DATA_MASK                          (0xFFU)
#define I2C_D_DATA_SHIFT                         (0U)
#define I2C_D_DATA(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_D_DATA_SHIFT)) & I2C_D_DATA_MASK)

/*! @name C2 - I2C Control Register 2 */
#define I2C_C2_AD_MASK                           (0x7U)
#define I2C_C2_AD_SHIFT                          (0U)
#define I2C_C2_AD(x)                             (((uint8_t)(((uint8_t)(x)) << I2C_C2_AD_SHIFT)) & I2C_C2_AD_MASK)
#define I2C_C2_RMEN_MASK                         (0x8U)
#define I2C_C2_RMEN_SHIFT                        (3U)
#define I2C_C2_RMEN(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_C2_RMEN_SHIFT)) & I2C_C2_RMEN_MASK)
#define I2C_C2_SBRC_MASK                         (0x10U)
#define I2C_C2_SBRC_SHIFT                        (4U)
#define I2C_C2_SBRC(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_C2_SBRC_SHIFT)) & I2C_C2_SBRC_MASK)
#define I2C_C2_ADEXT_MASK                        (0x40U)
#define I2C_C2_ADEXT_SHIFT                       (6U)
#define I2C_C2_ADEXT(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_C2_ADEXT_SHIFT)) & I2C_C2_ADEXT_MASK)
#define I2C_C2_GCAEN_MASK                        (0x80U)
#define I2C_C2_GCAEN_SHIFT                       (7U)
#define I2C_C2_GCAEN(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_C2_GCAEN_SHIFT)) & I2C_C2_GCAEN_MASK)

/*! @name FLT - I2C Programmable Input Glitch Filter Register */
#define I2C_FLT_FLT_MASK                         (0xFU)
#define I2C_FLT_FLT_SHIFT                        (0U)
#define I2C_FLT_FLT(x)                           (((uint8_t)(((uint8_t)(x)) << I2C_FLT_FLT_SHIFT)) & I2C_FLT_FLT_MASK)
#define I2C_FLT_STARTF_MASK                      (0x10U)
#define I2C_FLT_STARTF_SHIFT                     (4U)
#define I2C_FLT_STARTF(x)                        (((uint8_t)(((uint8_t)(x)) << I2C_FLT_STARTF_SHIFT)) & I2C_FLT_STARTF_MASK)
#define I2C_FLT_SSIE_MASK                        (0x20U)
#define I2C_FLT_SSIE_SHIFT                       (5U)
#define I2C_FLT_SSIE(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_FLT_SSIE_SHIFT)) & I2C_FLT_SSIE_MASK)
#define I2C_FLT_STOPF_MASK                       (0x40U)
#define I2C_FLT_STOPF_SHIFT                      (6U)
#define I2C_FLT_STOPF(x)                         (((uint8_t)(((uint8_t)(x)) << I2C_FLT_STOPF_SHIFT)) & I2C_FLT_STOPF_MASK)
#define I2C_FLT_SHEN_MASK                        (0x80U)
#define I2C_FLT_SHEN_SHIFT                       (7U)
#define I2C_FLT_SHEN(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_FLT_SHEN_SHIFT)) & I2C_FLT_SHEN_MASK)

/*! @name RA - I2C Range Address register */
#define I2C_RA_RAD_MASK                          (0xFEU)
#define I2C_RA_RAD_SHIFT                         (1U)
#define I2C_RA_RAD(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_RA_RAD_SHIFT)) & I2C_RA_RAD_MASK)

/*! @name SMB - I2C SMBus Control and Status register */
#define I2C_SMB_SHTF2IE_MASK                     (0x1U)
#define I2C_SMB_SHTF2IE_SHIFT                    (0U)
#define I2C_SMB_SHTF2IE(x)                       (((uint8_t)(((uint8_t)(x)) << I2C_SMB_SHTF2IE_SHIFT)) & I2C_SMB_SHTF2IE_MASK)
#define I2C_SMB_SHTF2_MASK                       (0x2U)
#define I2C_SMB_SHTF2_SHIFT                      (1U)
#define I2C_SMB_SHTF2(x)                         (((uint8_t)(((uint8_t)(x)) << I2C_SMB_SHTF2_SHIFT)) & I2C_SMB_SHTF2_MASK)
#define I2C_SMB_SHTF1_MASK                       (0x4U)
#define I2C_SMB_SHTF1_SHIFT                      (2U)
#define I2C_SMB_SHTF1(x)                         (((uint8_t)(((uint8_t)(x)) << I2C_SMB_SHTF1_SHIFT)) & I2C_SMB_SHTF1_MASK)
#define I2C_SMB_SLTF_MASK                        (0x8U)
#define I2C_SMB_SLTF_SHIFT                       (3U)
#define I2C_SMB_SLTF(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_SMB_SLTF_SHIFT)) & I2C_SMB_SLTF_MASK)
#define I2C_SMB_TCKSEL_MASK                      (0x10U)
#define I2C_SMB_TCKSEL_SHIFT                     (4U)
#define I2C_SMB_TCKSEL(x)                        (((uint8_t)(((uint8_t)(x)) << I2C_SMB_TCKSEL_SHIFT)) & I2C_SMB_TCKSEL_MASK)
#define I2C_SMB_SIICAEN_MASK                     (0x20U)
#define I2C_SMB_SIICAEN_SHIFT                    (5U)
#define I2C_SMB_SIICAEN(x)                       (((uint8_t)(((uint8_t)(x)) << I2C_SMB_SIICAEN_SHIFT)) & I2C_SMB_SIICAEN_MASK)
#define I2C_SMB_ALERTEN_MASK                     (0x40U)
#define I2C_SMB_ALERTEN_SHIFT                    (6U)
#define I2C_SMB_ALERTEN(x)                       (((uint8_t)(((uint8_t)(x)) << I2C_SMB_ALERTEN_SHIFT)) & I2C_SMB_ALERTEN_MASK)
#define I2C_SMB_FACK_MASK                        (0x80U)
#define I2C_SMB_FACK_SHIFT                       (7U)
#define I2C_SMB_FACK(x)                          (((uint8_t)(((uint8_t)(x)) << I2C_SMB_FACK_SHIFT)) & I2C_SMB_FACK_MASK)

/*! @name A2 - I2C Address Register 2 */
#define I2C_A2_SAD_MASK                          (0xFEU)
#define I2C_A2_SAD_SHIFT                         (1U)
#define I2C_A2_SAD(x)                            (((uint8_t)(((uint8_t)(x)) << I2C_A2_SAD_SHIFT)) & I2C_A2_SAD_MASK)

/*! @name SLTH - I2C SCL Low Timeout Register High */
#define I2C_SLTH_SSLT_MASK                       (0xFFU)
#define I2C_SLTH_SSLT_SHIFT                      (0U)
#define I2C_SLTH_SSLT(x)                         (((uint8_t)(((uint8_t)(x)) << I2C_SLTH_SSLT_SHIFT)) & I2C_SLTH_SSLT_MASK)

/*! @name SLTL - I2C SCL Low Timeout Register Low */
#define I2C_SLTL_SSLT_MASK                       (0xFFU)
#define I2C_SLTL_SSLT_SHIFT                      (0U)
#define I2C_SLTL_SSLT(x)                         (((uint8_t)(((uint8_t)(x)) << I2C_SLTL_SSLT_SHIFT)) & I2C_SLTL_SSLT_MASK)


/*!
 * @}
 */ /* end of group I2C_Register_Masks */


/* I2C - Peripheral instance base addresses */
/** Peripheral I2C0 base address */
#define I2C0_BASE                                (0x40066000u)
/** Peripheral I2C0 base pointer */
#define I2C0                                     ((I2C_Type *)I2C0_BASE)
/** Array initializer of I2C peripheral base addresses */
#define I2C_BASE_ADDRS                           { I2C0_BASE }
/** Array initializer of I2C peripheral base pointers */
#define I2C_BASE_PTRS                            { I2C0 }
/** Interrupt vectors for the I2C peripheral type */
#define I2C_IRQS                                 { I2C0_IRQn }

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
} ICS_Type;

/* ----------------------------------------------------------------------------
   -- ICS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ICS_Register_Masks ICS Register Masks
 * @{
 */

/*! @name C1 - ICS Control Register 1 */
#define ICS_C1_IREFSTEN_MASK                     (0x1U)
#define ICS_C1_IREFSTEN_SHIFT                    (0U)
#define ICS_C1_IREFSTEN(x)                       (((uint8_t)(((uint8_t)(x)) << ICS_C1_IREFSTEN_SHIFT)) & ICS_C1_IREFSTEN_MASK)
#define ICS_C1_IRCLKEN_MASK                      (0x2U)
#define ICS_C1_IRCLKEN_SHIFT                     (1U)
#define ICS_C1_IRCLKEN(x)                        (((uint8_t)(((uint8_t)(x)) << ICS_C1_IRCLKEN_SHIFT)) & ICS_C1_IRCLKEN_MASK)
#define ICS_C1_IREFS_MASK                        (0x4U)
#define ICS_C1_IREFS_SHIFT                       (2U)
#define ICS_C1_IREFS(x)                          (((uint8_t)(((uint8_t)(x)) << ICS_C1_IREFS_SHIFT)) & ICS_C1_IREFS_MASK)
#define ICS_C1_RDIV_MASK                         (0x38U)
#define ICS_C1_RDIV_SHIFT                        (3U)
#define ICS_C1_RDIV(x)                           (((uint8_t)(((uint8_t)(x)) << ICS_C1_RDIV_SHIFT)) & ICS_C1_RDIV_MASK)
#define ICS_C1_CLKS_MASK                         (0xC0U)
#define ICS_C1_CLKS_SHIFT                        (6U)
#define ICS_C1_CLKS(x)                           (((uint8_t)(((uint8_t)(x)) << ICS_C1_CLKS_SHIFT)) & ICS_C1_CLKS_MASK)

/*! @name C2 - ICS Control Register 2 */
#define ICS_C2_LP_MASK                           (0x10U)
#define ICS_C2_LP_SHIFT                          (4U)
#define ICS_C2_LP(x)                             (((uint8_t)(((uint8_t)(x)) << ICS_C2_LP_SHIFT)) & ICS_C2_LP_MASK)
#define ICS_C2_BDIV_MASK                         (0xE0U)
#define ICS_C2_BDIV_SHIFT                        (5U)
#define ICS_C2_BDIV(x)                           (((uint8_t)(((uint8_t)(x)) << ICS_C2_BDIV_SHIFT)) & ICS_C2_BDIV_MASK)

/*! @name C3 - ICS Control Register 3 */
#define ICS_C3_SCTRIM_MASK                       (0xFFU)
#define ICS_C3_SCTRIM_SHIFT                      (0U)
#define ICS_C3_SCTRIM(x)                         (((uint8_t)(((uint8_t)(x)) << ICS_C3_SCTRIM_SHIFT)) & ICS_C3_SCTRIM_MASK)

/*! @name C4 - ICS Control Register 4 */
#define ICS_C4_SCFTRIM_MASK                      (0x1U)
#define ICS_C4_SCFTRIM_SHIFT                     (0U)
#define ICS_C4_SCFTRIM(x)                        (((uint8_t)(((uint8_t)(x)) << ICS_C4_SCFTRIM_SHIFT)) & ICS_C4_SCFTRIM_MASK)
#define ICS_C4_CME_MASK                          (0x20U)
#define ICS_C4_CME_SHIFT                         (5U)
#define ICS_C4_CME(x)                            (((uint8_t)(((uint8_t)(x)) << ICS_C4_CME_SHIFT)) & ICS_C4_CME_MASK)
#define ICS_C4_LOLIE_MASK                        (0x80U)
#define ICS_C4_LOLIE_SHIFT                       (7U)
#define ICS_C4_LOLIE(x)                          (((uint8_t)(((uint8_t)(x)) << ICS_C4_LOLIE_SHIFT)) & ICS_C4_LOLIE_MASK)

/*! @name S - ICS Status Register */
#define ICS_S_CLKST_MASK                         (0xCU)
#define ICS_S_CLKST_SHIFT                        (2U)
#define ICS_S_CLKST(x)                           (((uint8_t)(((uint8_t)(x)) << ICS_S_CLKST_SHIFT)) & ICS_S_CLKST_MASK)
#define ICS_S_IREFST_MASK                        (0x10U)
#define ICS_S_IREFST_SHIFT                       (4U)
#define ICS_S_IREFST(x)                          (((uint8_t)(((uint8_t)(x)) << ICS_S_IREFST_SHIFT)) & ICS_S_IREFST_MASK)
#define ICS_S_LOCK_MASK                          (0x40U)
#define ICS_S_LOCK_SHIFT                         (6U)
#define ICS_S_LOCK(x)                            (((uint8_t)(((uint8_t)(x)) << ICS_S_LOCK_SHIFT)) & ICS_S_LOCK_MASK)
#define ICS_S_LOLS_MASK                          (0x80U)
#define ICS_S_LOLS_SHIFT                         (7U)
#define ICS_S_LOLS(x)                            (((uint8_t)(((uint8_t)(x)) << ICS_S_LOLS_SHIFT)) & ICS_S_LOLS_MASK)


/*!
 * @}
 */ /* end of group ICS_Register_Masks */


/* ICS - Peripheral instance base addresses */
/** Peripheral ICS base address */
#define ICS_BASE                                 (0x40064000u)
/** Peripheral ICS base pointer */
#define ICS                                      ((ICS_Type *)ICS_BASE)
/** Array initializer of ICS peripheral base addresses */
#define ICS_BASE_ADDRS                           { ICS_BASE }
/** Array initializer of ICS peripheral base pointers */
#define ICS_BASE_PTRS                            { ICS }
/** Interrupt vectors for the ICS peripheral type */
#define ICS_IRQS                                 { ICS_IRQn }

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
} IRQ_Type;

/* ----------------------------------------------------------------------------
   -- IRQ Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IRQ_Register_Masks IRQ Register Masks
 * @{
 */

/*! @name SC - Interrupt Pin Request Status and Control Register */
#define IRQ_SC_IRQMOD_MASK                       (0x1U)
#define IRQ_SC_IRQMOD_SHIFT                      (0U)
#define IRQ_SC_IRQMOD(x)                         (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQMOD_SHIFT)) & IRQ_SC_IRQMOD_MASK)
#define IRQ_SC_IRQIE_MASK                        (0x2U)
#define IRQ_SC_IRQIE_SHIFT                       (1U)
#define IRQ_SC_IRQIE(x)                          (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQIE_SHIFT)) & IRQ_SC_IRQIE_MASK)
#define IRQ_SC_IRQACK_MASK                       (0x4U)
#define IRQ_SC_IRQACK_SHIFT                      (2U)
#define IRQ_SC_IRQACK(x)                         (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQACK_SHIFT)) & IRQ_SC_IRQACK_MASK)
#define IRQ_SC_IRQF_MASK                         (0x8U)
#define IRQ_SC_IRQF_SHIFT                        (3U)
#define IRQ_SC_IRQF(x)                           (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQF_SHIFT)) & IRQ_SC_IRQF_MASK)
#define IRQ_SC_IRQPE_MASK                        (0x10U)
#define IRQ_SC_IRQPE_SHIFT                       (4U)
#define IRQ_SC_IRQPE(x)                          (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQPE_SHIFT)) & IRQ_SC_IRQPE_MASK)
#define IRQ_SC_IRQEDG_MASK                       (0x20U)
#define IRQ_SC_IRQEDG_SHIFT                      (5U)
#define IRQ_SC_IRQEDG(x)                         (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQEDG_SHIFT)) & IRQ_SC_IRQEDG_MASK)
#define IRQ_SC_IRQPDD_MASK                       (0x40U)
#define IRQ_SC_IRQPDD_SHIFT                      (6U)
#define IRQ_SC_IRQPDD(x)                         (((uint8_t)(((uint8_t)(x)) << IRQ_SC_IRQPDD_SHIFT)) & IRQ_SC_IRQPDD_MASK)


/*!
 * @}
 */ /* end of group IRQ_Register_Masks */


/* IRQ - Peripheral instance base addresses */
/** Peripheral IRQ base address */
#define IRQ_BASE                                 (0x40031000u)
/** Peripheral IRQ base pointer */
#define IRQ                                      ((IRQ_Type *)IRQ_BASE)
/** Array initializer of IRQ peripheral base addresses */
#define IRQ_BASE_ADDRS                           { IRQ_BASE }
/** Array initializer of IRQ peripheral base pointers */
#define IRQ_BASE_PTRS                            { IRQ }

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
  __IO uint8_t SC;                                 /**< KBI Status and Control Register, offset: 0x0 */
  __IO uint8_t PE;                                 /**< KBI Pin Enable Register, offset: 0x1 */
  __IO uint8_t ES;                                 /**< KBI Edge Select Register, offset: 0x2 */
} KBI_Type;

/* ----------------------------------------------------------------------------
   -- KBI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup KBI_Register_Masks KBI Register Masks
 * @{
 */

/*! @name SC - KBI Status and Control Register */
#define KBI_SC_KBMOD_MASK                        (0x1U)
#define KBI_SC_KBMOD_SHIFT                       (0U)
#define KBI_SC_KBMOD(x)                          (((uint8_t)(((uint8_t)(x)) << KBI_SC_KBMOD_SHIFT)) & KBI_SC_KBMOD_MASK)
#define KBI_SC_KBIE_MASK                         (0x2U)
#define KBI_SC_KBIE_SHIFT                        (1U)
#define KBI_SC_KBIE(x)                           (((uint8_t)(((uint8_t)(x)) << KBI_SC_KBIE_SHIFT)) & KBI_SC_KBIE_MASK)
#define KBI_SC_KBACK_MASK                        (0x4U)
#define KBI_SC_KBACK_SHIFT                       (2U)
#define KBI_SC_KBACK(x)                          (((uint8_t)(((uint8_t)(x)) << KBI_SC_KBACK_SHIFT)) & KBI_SC_KBACK_MASK)
#define KBI_SC_KBF_MASK                          (0x8U)
#define KBI_SC_KBF_SHIFT                         (3U)
#define KBI_SC_KBF(x)                            (((uint8_t)(((uint8_t)(x)) << KBI_SC_KBF_SHIFT)) & KBI_SC_KBF_MASK)

/*! @name PE - KBI Pin Enable Register */
#define KBI_PE_KBIPE_MASK                        (0xFFU)
#define KBI_PE_KBIPE_SHIFT                       (0U)
#define KBI_PE_KBIPE(x)                          (((uint8_t)(((uint8_t)(x)) << KBI_PE_KBIPE_SHIFT)) & KBI_PE_KBIPE_MASK)

/*! @name ES - KBI Edge Select Register */
#define KBI_ES_KBEDG_MASK                        (0xFFU)
#define KBI_ES_KBEDG_SHIFT                       (0U)
#define KBI_ES_KBEDG(x)                          (((uint8_t)(((uint8_t)(x)) << KBI_ES_KBEDG_SHIFT)) & KBI_ES_KBEDG_MASK)


/*!
 * @}
 */ /* end of group KBI_Register_Masks */


/* KBI - Peripheral instance base addresses */
/** Peripheral KBI0 base address */
#define KBI0_BASE                                (0x40079000u)
/** Peripheral KBI0 base pointer */
#define KBI0                                     ((KBI_Type *)KBI0_BASE)
/** Peripheral KBI1 base address */
#define KBI1_BASE                                (0x4007A000u)
/** Peripheral KBI1 base pointer */
#define KBI1                                     ((KBI_Type *)KBI1_BASE)
/** Array initializer of KBI peripheral base addresses */
#define KBI_BASE_ADDRS                           { KBI0_BASE, KBI1_BASE }
/** Array initializer of KBI peripheral base pointers */
#define KBI_BASE_PTRS                            { KBI0, KBI1 }
/** Interrupt vectors for the KBI peripheral type */
#define KBI_IRQS                                 { KBI0_IRQn, KBI1_IRQn }

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
} MCM_Type;

/* ----------------------------------------------------------------------------
   -- MCM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MCM_Register_Masks MCM Register Masks
 * @{
 */

/*! @name PLASC - Crossbar Switch (AXBS) Slave Configuration */
#define MCM_PLASC_ASC_MASK                       (0xFFU)
#define MCM_PLASC_ASC_SHIFT                      (0U)
#define MCM_PLASC_ASC(x)                         (((uint16_t)(((uint16_t)(x)) << MCM_PLASC_ASC_SHIFT)) & MCM_PLASC_ASC_MASK)

/*! @name PLAMC - Crossbar Switch (AXBS) Master Configuration */
#define MCM_PLAMC_AMC_MASK                       (0xFFU)
#define MCM_PLAMC_AMC_SHIFT                      (0U)
#define MCM_PLAMC_AMC(x)                         (((uint16_t)(((uint16_t)(x)) << MCM_PLAMC_AMC_SHIFT)) & MCM_PLAMC_AMC_MASK)

/*! @name PLACR - Platform Control Register */
#define MCM_PLACR_CFCC_MASK                      (0x400U)
#define MCM_PLACR_CFCC_SHIFT                     (10U)
#define MCM_PLACR_CFCC(x)                        (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_CFCC_SHIFT)) & MCM_PLACR_CFCC_MASK)
#define MCM_PLACR_DFCDA_MASK                     (0x800U)
#define MCM_PLACR_DFCDA_SHIFT                    (11U)
#define MCM_PLACR_DFCDA(x)                       (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_DFCDA_SHIFT)) & MCM_PLACR_DFCDA_MASK)
#define MCM_PLACR_DFCIC_MASK                     (0x1000U)
#define MCM_PLACR_DFCIC_SHIFT                    (12U)
#define MCM_PLACR_DFCIC(x)                       (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_DFCIC_SHIFT)) & MCM_PLACR_DFCIC_MASK)
#define MCM_PLACR_DFCC_MASK                      (0x2000U)
#define MCM_PLACR_DFCC_SHIFT                     (13U)
#define MCM_PLACR_DFCC(x)                        (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_DFCC_SHIFT)) & MCM_PLACR_DFCC_MASK)
#define MCM_PLACR_EFDS_MASK                      (0x4000U)
#define MCM_PLACR_EFDS_SHIFT                     (14U)
#define MCM_PLACR_EFDS(x)                        (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_EFDS_SHIFT)) & MCM_PLACR_EFDS_MASK)
#define MCM_PLACR_DFCS_MASK                      (0x8000U)
#define MCM_PLACR_DFCS_SHIFT                     (15U)
#define MCM_PLACR_DFCS(x)                        (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_DFCS_SHIFT)) & MCM_PLACR_DFCS_MASK)
#define MCM_PLACR_ESFC_MASK                      (0x10000U)
#define MCM_PLACR_ESFC_SHIFT                     (16U)
#define MCM_PLACR_ESFC(x)                        (((uint32_t)(((uint32_t)(x)) << MCM_PLACR_ESFC_SHIFT)) & MCM_PLACR_ESFC_MASK)


/*!
 * @}
 */ /* end of group MCM_Register_Masks */


/* MCM - Peripheral instance base addresses */
/** Peripheral MCM base address */
#define MCM_BASE                                 (0xF0003000u)
/** Peripheral MCM base pointer */
#define MCM                                      ((MCM_Type *)MCM_BASE)
/** Array initializer of MCM peripheral base addresses */
#define MCM_BASE_ADDRS                           { MCM_BASE }
/** Array initializer of MCM peripheral base pointers */
#define MCM_BASE_PTRS                            { MCM }

/*!
 * @}
 */ /* end of group MCM_Peripheral_Access_Layer */


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
} OSC_Type;

/* ----------------------------------------------------------------------------
   -- OSC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup OSC_Register_Masks OSC Register Masks
 * @{
 */

/*! @name CR - OSC Control Register */
#define OSC_CR_OSCINIT_MASK                      (0x1U)
#define OSC_CR_OSCINIT_SHIFT                     (0U)
#define OSC_CR_OSCINIT(x)                        (((uint8_t)(((uint8_t)(x)) << OSC_CR_OSCINIT_SHIFT)) & OSC_CR_OSCINIT_MASK)
#define OSC_CR_HGO_MASK                          (0x2U)
#define OSC_CR_HGO_SHIFT                         (1U)
#define OSC_CR_HGO(x)                            (((uint8_t)(((uint8_t)(x)) << OSC_CR_HGO_SHIFT)) & OSC_CR_HGO_MASK)
#define OSC_CR_RANGE_MASK                        (0x4U)
#define OSC_CR_RANGE_SHIFT                       (2U)
#define OSC_CR_RANGE(x)                          (((uint8_t)(((uint8_t)(x)) << OSC_CR_RANGE_SHIFT)) & OSC_CR_RANGE_MASK)
#define OSC_CR_OSCOS_MASK                        (0x10U)
#define OSC_CR_OSCOS_SHIFT                       (4U)
#define OSC_CR_OSCOS(x)                          (((uint8_t)(((uint8_t)(x)) << OSC_CR_OSCOS_SHIFT)) & OSC_CR_OSCOS_MASK)
#define OSC_CR_OSCSTEN_MASK                      (0x20U)
#define OSC_CR_OSCSTEN_SHIFT                     (5U)
#define OSC_CR_OSCSTEN(x)                        (((uint8_t)(((uint8_t)(x)) << OSC_CR_OSCSTEN_SHIFT)) & OSC_CR_OSCSTEN_MASK)
#define OSC_CR_OSCEN_MASK                        (0x80U)
#define OSC_CR_OSCEN_SHIFT                       (7U)
#define OSC_CR_OSCEN(x)                          (((uint8_t)(((uint8_t)(x)) << OSC_CR_OSCEN_SHIFT)) & OSC_CR_OSCEN_MASK)


/*!
 * @}
 */ /* end of group OSC_Register_Masks */


/* OSC - Peripheral instance base addresses */
/** Peripheral OSC base address */
#define OSC_BASE                                 (0x40065000u)
/** Peripheral OSC base pointer */
#define OSC                                      ((OSC_Type *)OSC_BASE)
/** Array initializer of OSC peripheral base addresses */
#define OSC_BASE_ADDRS                           { OSC_BASE }
/** Array initializer of OSC peripheral base pointers */
#define OSC_BASE_PTRS                            { OSC }

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
} PIT_Type;

/* ----------------------------------------------------------------------------
   -- PIT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Register_Masks PIT Register Masks
 * @{
 */

/*! @name MCR - PIT Module Control Register */
#define PIT_MCR_FRZ_MASK                         (0x1U)
#define PIT_MCR_FRZ_SHIFT                        (0U)
#define PIT_MCR_FRZ(x)                           (((uint32_t)(((uint32_t)(x)) << PIT_MCR_FRZ_SHIFT)) & PIT_MCR_FRZ_MASK)
#define PIT_MCR_MDIS_MASK                        (0x2U)
#define PIT_MCR_MDIS_SHIFT                       (1U)
#define PIT_MCR_MDIS(x)                          (((uint32_t)(((uint32_t)(x)) << PIT_MCR_MDIS_SHIFT)) & PIT_MCR_MDIS_MASK)

/*! @name LDVAL - Timer Load Value Register */
#define PIT_LDVAL_TSV_MASK                       (0xFFFFFFFFU)
#define PIT_LDVAL_TSV_SHIFT                      (0U)
#define PIT_LDVAL_TSV(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_LDVAL_TSV_SHIFT)) & PIT_LDVAL_TSV_MASK)

/* The count of PIT_LDVAL */
#define PIT_LDVAL_COUNT                          (2U)

/*! @name CVAL - Current Timer Value Register */
#define PIT_CVAL_TVL_MASK                        (0xFFFFFFFFU)
#define PIT_CVAL_TVL_SHIFT                       (0U)
#define PIT_CVAL_TVL(x)                          (((uint32_t)(((uint32_t)(x)) << PIT_CVAL_TVL_SHIFT)) & PIT_CVAL_TVL_MASK)

/* The count of PIT_CVAL */
#define PIT_CVAL_COUNT                           (2U)

/*! @name TCTRL - Timer Control Register */
#define PIT_TCTRL_TEN_MASK                       (0x1U)
#define PIT_TCTRL_TEN_SHIFT                      (0U)
#define PIT_TCTRL_TEN(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_TCTRL_TEN_SHIFT)) & PIT_TCTRL_TEN_MASK)
#define PIT_TCTRL_TIE_MASK                       (0x2U)
#define PIT_TCTRL_TIE_SHIFT                      (1U)
#define PIT_TCTRL_TIE(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_TCTRL_TIE_SHIFT)) & PIT_TCTRL_TIE_MASK)
#define PIT_TCTRL_CHN_MASK                       (0x4U)
#define PIT_TCTRL_CHN_SHIFT                      (2U)
#define PIT_TCTRL_CHN(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_TCTRL_CHN_SHIFT)) & PIT_TCTRL_CHN_MASK)

/* The count of PIT_TCTRL */
#define PIT_TCTRL_COUNT                          (2U)

/*! @name TFLG - Timer Flag Register */
#define PIT_TFLG_TIF_MASK                        (0x1U)
#define PIT_TFLG_TIF_SHIFT                       (0U)
#define PIT_TFLG_TIF(x)                          (((uint32_t)(((uint32_t)(x)) << PIT_TFLG_TIF_SHIFT)) & PIT_TFLG_TIF_MASK)

/* The count of PIT_TFLG */
#define PIT_TFLG_COUNT                           (2U)


/*!
 * @}
 */ /* end of group PIT_Register_Masks */


/* PIT - Peripheral instance base addresses */
/** Peripheral PIT base address */
#define PIT_BASE                                 (0x40037000u)
/** Peripheral PIT base pointer */
#define PIT                                      ((PIT_Type *)PIT_BASE)
/** Array initializer of PIT peripheral base addresses */
#define PIT_BASE_ADDRS                           { PIT_BASE }
/** Array initializer of PIT peripheral base pointers */
#define PIT_BASE_PTRS                            { PIT }
/** Interrupt vectors for the PIT peripheral type */
#define PIT_IRQS                                 { { PIT_CH0_IRQn, PIT_CH1_IRQn } }

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
} PMC_Type;

/* ----------------------------------------------------------------------------
   -- PMC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMC_Register_Masks PMC Register Masks
 * @{
 */

/*! @name SPMSC1 - System Power Management Status and Control 1 Register */
#define PMC_SPMSC1_BGBE_MASK                     (0x1U)
#define PMC_SPMSC1_BGBE_SHIFT                    (0U)
#define PMC_SPMSC1_BGBE(x)                       (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_BGBE_SHIFT)) & PMC_SPMSC1_BGBE_MASK)
#define PMC_SPMSC1_LVDE_MASK                     (0x4U)
#define PMC_SPMSC1_LVDE_SHIFT                    (2U)
#define PMC_SPMSC1_LVDE(x)                       (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_LVDE_SHIFT)) & PMC_SPMSC1_LVDE_MASK)
#define PMC_SPMSC1_LVDSE_MASK                    (0x8U)
#define PMC_SPMSC1_LVDSE_SHIFT                   (3U)
#define PMC_SPMSC1_LVDSE(x)                      (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_LVDSE_SHIFT)) & PMC_SPMSC1_LVDSE_MASK)
#define PMC_SPMSC1_LVDRE_MASK                    (0x10U)
#define PMC_SPMSC1_LVDRE_SHIFT                   (4U)
#define PMC_SPMSC1_LVDRE(x)                      (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_LVDRE_SHIFT)) & PMC_SPMSC1_LVDRE_MASK)
#define PMC_SPMSC1_LVWIE_MASK                    (0x20U)
#define PMC_SPMSC1_LVWIE_SHIFT                   (5U)
#define PMC_SPMSC1_LVWIE(x)                      (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_LVWIE_SHIFT)) & PMC_SPMSC1_LVWIE_MASK)
#define PMC_SPMSC1_LVWACK_MASK                   (0x40U)
#define PMC_SPMSC1_LVWACK_SHIFT                  (6U)
#define PMC_SPMSC1_LVWACK(x)                     (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_LVWACK_SHIFT)) & PMC_SPMSC1_LVWACK_MASK)
#define PMC_SPMSC1_LVWF_MASK                     (0x80U)
#define PMC_SPMSC1_LVWF_SHIFT                    (7U)
#define PMC_SPMSC1_LVWF(x)                       (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC1_LVWF_SHIFT)) & PMC_SPMSC1_LVWF_MASK)

/*! @name SPMSC2 - System Power Management Status and Control 2 Register */
#define PMC_SPMSC2_LVWV_MASK                     (0x30U)
#define PMC_SPMSC2_LVWV_SHIFT                    (4U)
#define PMC_SPMSC2_LVWV(x)                       (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC2_LVWV_SHIFT)) & PMC_SPMSC2_LVWV_MASK)
#define PMC_SPMSC2_LVDV_MASK                     (0x40U)
#define PMC_SPMSC2_LVDV_SHIFT                    (6U)
#define PMC_SPMSC2_LVDV(x)                       (((uint8_t)(((uint8_t)(x)) << PMC_SPMSC2_LVDV_SHIFT)) & PMC_SPMSC2_LVDV_MASK)


/*!
 * @}
 */ /* end of group PMC_Register_Masks */


/* PMC - Peripheral instance base addresses */
/** Peripheral PMC base address */
#define PMC_BASE                                 (0x4007D000u)
/** Peripheral PMC base pointer */
#define PMC                                      ((PMC_Type *)PMC_BASE)
/** Array initializer of PMC peripheral base addresses */
#define PMC_BASE_ADDRS                           { PMC_BASE }
/** Array initializer of PMC peripheral base pointers */
#define PMC_BASE_PTRS                            { PMC }
/** Interrupt vectors for the PMC peripheral type */
#define PMC_IRQS                                 { PMC_IRQn }

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
  __IO uint32_t IOFLT;                             /**< Port Filter Register, offset: 0x0 */
  __IO uint32_t PUEL;                              /**< Port Pullup Enable Low Register, offset: 0x4 */
  __IO uint32_t PUEH;                              /**< Port Pullup Enable High Register, offset: 0x8 */
  __IO uint32_t HDRVE;                             /**< Port High Drive Enable Register, offset: 0xC */
} PORT_Type;

/* ----------------------------------------------------------------------------
   -- PORT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Masks PORT Register Masks
 * @{
 */

/*! @name IOFLT - Port Filter Register */
#define PORT_IOFLT_FLTA_MASK                     (0x3U)
#define PORT_IOFLT_FLTA_SHIFT                    (0U)
#define PORT_IOFLT_FLTA(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTA_SHIFT)) & PORT_IOFLT_FLTA_MASK)
#define PORT_IOFLT_FLTB_MASK                     (0xCU)
#define PORT_IOFLT_FLTB_SHIFT                    (2U)
#define PORT_IOFLT_FLTB(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTB_SHIFT)) & PORT_IOFLT_FLTB_MASK)
#define PORT_IOFLT_FLTC_MASK                     (0x30U)
#define PORT_IOFLT_FLTC_SHIFT                    (4U)
#define PORT_IOFLT_FLTC(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTC_SHIFT)) & PORT_IOFLT_FLTC_MASK)
#define PORT_IOFLT_FLTD_MASK                     (0xC0U)
#define PORT_IOFLT_FLTD_SHIFT                    (6U)
#define PORT_IOFLT_FLTD(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTD_SHIFT)) & PORT_IOFLT_FLTD_MASK)
#define PORT_IOFLT_FLTE_MASK                     (0x300U)
#define PORT_IOFLT_FLTE_SHIFT                    (8U)
#define PORT_IOFLT_FLTE(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTE_SHIFT)) & PORT_IOFLT_FLTE_MASK)
#define PORT_IOFLT_FLTF_MASK                     (0xC00U)
#define PORT_IOFLT_FLTF_SHIFT                    (10U)
#define PORT_IOFLT_FLTF(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTF_SHIFT)) & PORT_IOFLT_FLTF_MASK)
#define PORT_IOFLT_FLTG_MASK                     (0x3000U)
#define PORT_IOFLT_FLTG_SHIFT                    (12U)
#define PORT_IOFLT_FLTG(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTG_SHIFT)) & PORT_IOFLT_FLTG_MASK)
#define PORT_IOFLT_FLTH_MASK                     (0xC000U)
#define PORT_IOFLT_FLTH_SHIFT                    (14U)
#define PORT_IOFLT_FLTH(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTH_SHIFT)) & PORT_IOFLT_FLTH_MASK)
#define PORT_IOFLT_FLTRST_MASK                   (0x30000U)
#define PORT_IOFLT_FLTRST_SHIFT                  (16U)
#define PORT_IOFLT_FLTRST(x)                     (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTRST_SHIFT)) & PORT_IOFLT_FLTRST_MASK)
#define PORT_IOFLT_FLTKBI0_MASK                  (0xC0000U)
#define PORT_IOFLT_FLTKBI0_SHIFT                 (18U)
#define PORT_IOFLT_FLTKBI0(x)                    (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTKBI0_SHIFT)) & PORT_IOFLT_FLTKBI0_MASK)
#define PORT_IOFLT_FLTKBI1_MASK                  (0x300000U)
#define PORT_IOFLT_FLTKBI1_SHIFT                 (20U)
#define PORT_IOFLT_FLTKBI1(x)                    (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTKBI1_SHIFT)) & PORT_IOFLT_FLTKBI1_MASK)
#define PORT_IOFLT_FLTNMI_MASK                   (0xC00000U)
#define PORT_IOFLT_FLTNMI_SHIFT                  (22U)
#define PORT_IOFLT_FLTNMI(x)                     (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTNMI_SHIFT)) & PORT_IOFLT_FLTNMI_MASK)
#define PORT_IOFLT_FLTDIV1_MASK                  (0x3000000U)
#define PORT_IOFLT_FLTDIV1_SHIFT                 (24U)
#define PORT_IOFLT_FLTDIV1(x)                    (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTDIV1_SHIFT)) & PORT_IOFLT_FLTDIV1_MASK)
#define PORT_IOFLT_FLTDIV2_MASK                  (0x1C000000U)
#define PORT_IOFLT_FLTDIV2_SHIFT                 (26U)
#define PORT_IOFLT_FLTDIV2(x)                    (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTDIV2_SHIFT)) & PORT_IOFLT_FLTDIV2_MASK)
#define PORT_IOFLT_FLTDIV3_MASK                  (0xE0000000U)
#define PORT_IOFLT_FLTDIV3_SHIFT                 (29U)
#define PORT_IOFLT_FLTDIV3(x)                    (((uint32_t)(((uint32_t)(x)) << PORT_IOFLT_FLTDIV3_SHIFT)) & PORT_IOFLT_FLTDIV3_MASK)

/*! @name PUEL - Port Pullup Enable Low Register */
#define PORT_PUEL_PTAPE0_MASK                    (0x1U)
#define PORT_PUEL_PTAPE0_SHIFT                   (0U)
#define PORT_PUEL_PTAPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE0_SHIFT)) & PORT_PUEL_PTAPE0_MASK)
#define PORT_PUEL_PTAPE1_MASK                    (0x2U)
#define PORT_PUEL_PTAPE1_SHIFT                   (1U)
#define PORT_PUEL_PTAPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE1_SHIFT)) & PORT_PUEL_PTAPE1_MASK)
#define PORT_PUEL_PTAPE2_MASK                    (0x4U)
#define PORT_PUEL_PTAPE2_SHIFT                   (2U)
#define PORT_PUEL_PTAPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE2_SHIFT)) & PORT_PUEL_PTAPE2_MASK)
#define PORT_PUEL_PTAPE3_MASK                    (0x8U)
#define PORT_PUEL_PTAPE3_SHIFT                   (3U)
#define PORT_PUEL_PTAPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE3_SHIFT)) & PORT_PUEL_PTAPE3_MASK)
#define PORT_PUEL_PTAPE4_MASK                    (0x10U)
#define PORT_PUEL_PTAPE4_SHIFT                   (4U)
#define PORT_PUEL_PTAPE4(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE4_SHIFT)) & PORT_PUEL_PTAPE4_MASK)
#define PORT_PUEL_PTAPE5_MASK                    (0x20U)
#define PORT_PUEL_PTAPE5_SHIFT                   (5U)
#define PORT_PUEL_PTAPE5(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE5_SHIFT)) & PORT_PUEL_PTAPE5_MASK)
#define PORT_PUEL_PTAPE6_MASK                    (0x40U)
#define PORT_PUEL_PTAPE6_SHIFT                   (6U)
#define PORT_PUEL_PTAPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE6_SHIFT)) & PORT_PUEL_PTAPE6_MASK)
#define PORT_PUEL_PTAPE7_MASK                    (0x80U)
#define PORT_PUEL_PTAPE7_SHIFT                   (7U)
#define PORT_PUEL_PTAPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTAPE7_SHIFT)) & PORT_PUEL_PTAPE7_MASK)
#define PORT_PUEL_PTBPE0_MASK                    (0x100U)
#define PORT_PUEL_PTBPE0_SHIFT                   (8U)
#define PORT_PUEL_PTBPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE0_SHIFT)) & PORT_PUEL_PTBPE0_MASK)
#define PORT_PUEL_PTBPE1_MASK                    (0x200U)
#define PORT_PUEL_PTBPE1_SHIFT                   (9U)
#define PORT_PUEL_PTBPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE1_SHIFT)) & PORT_PUEL_PTBPE1_MASK)
#define PORT_PUEL_PTBPE2_MASK                    (0x400U)
#define PORT_PUEL_PTBPE2_SHIFT                   (10U)
#define PORT_PUEL_PTBPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE2_SHIFT)) & PORT_PUEL_PTBPE2_MASK)
#define PORT_PUEL_PTBPE3_MASK                    (0x800U)
#define PORT_PUEL_PTBPE3_SHIFT                   (11U)
#define PORT_PUEL_PTBPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE3_SHIFT)) & PORT_PUEL_PTBPE3_MASK)
#define PORT_PUEL_PTBPE4_MASK                    (0x1000U)
#define PORT_PUEL_PTBPE4_SHIFT                   (12U)
#define PORT_PUEL_PTBPE4(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE4_SHIFT)) & PORT_PUEL_PTBPE4_MASK)
#define PORT_PUEL_PTBPE5_MASK                    (0x2000U)
#define PORT_PUEL_PTBPE5_SHIFT                   (13U)
#define PORT_PUEL_PTBPE5(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE5_SHIFT)) & PORT_PUEL_PTBPE5_MASK)
#define PORT_PUEL_PTBPE6_MASK                    (0x4000U)
#define PORT_PUEL_PTBPE6_SHIFT                   (14U)
#define PORT_PUEL_PTBPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE6_SHIFT)) & PORT_PUEL_PTBPE6_MASK)
#define PORT_PUEL_PTBPE7_MASK                    (0x8000U)
#define PORT_PUEL_PTBPE7_SHIFT                   (15U)
#define PORT_PUEL_PTBPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTBPE7_SHIFT)) & PORT_PUEL_PTBPE7_MASK)
#define PORT_PUEL_PTCPE0_MASK                    (0x10000U)
#define PORT_PUEL_PTCPE0_SHIFT                   (16U)
#define PORT_PUEL_PTCPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE0_SHIFT)) & PORT_PUEL_PTCPE0_MASK)
#define PORT_PUEL_PTCPE1_MASK                    (0x20000U)
#define PORT_PUEL_PTCPE1_SHIFT                   (17U)
#define PORT_PUEL_PTCPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE1_SHIFT)) & PORT_PUEL_PTCPE1_MASK)
#define PORT_PUEL_PTCPE2_MASK                    (0x40000U)
#define PORT_PUEL_PTCPE2_SHIFT                   (18U)
#define PORT_PUEL_PTCPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE2_SHIFT)) & PORT_PUEL_PTCPE2_MASK)
#define PORT_PUEL_PTCPE3_MASK                    (0x80000U)
#define PORT_PUEL_PTCPE3_SHIFT                   (19U)
#define PORT_PUEL_PTCPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE3_SHIFT)) & PORT_PUEL_PTCPE3_MASK)
#define PORT_PUEL_PTCPE4_MASK                    (0x100000U)
#define PORT_PUEL_PTCPE4_SHIFT                   (20U)
#define PORT_PUEL_PTCPE4(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE4_SHIFT)) & PORT_PUEL_PTCPE4_MASK)
#define PORT_PUEL_PTCPE5_MASK                    (0x200000U)
#define PORT_PUEL_PTCPE5_SHIFT                   (21U)
#define PORT_PUEL_PTCPE5(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE5_SHIFT)) & PORT_PUEL_PTCPE5_MASK)
#define PORT_PUEL_PTCPE6_MASK                    (0x400000U)
#define PORT_PUEL_PTCPE6_SHIFT                   (22U)
#define PORT_PUEL_PTCPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE6_SHIFT)) & PORT_PUEL_PTCPE6_MASK)
#define PORT_PUEL_PTCPE7_MASK                    (0x800000U)
#define PORT_PUEL_PTCPE7_SHIFT                   (23U)
#define PORT_PUEL_PTCPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTCPE7_SHIFT)) & PORT_PUEL_PTCPE7_MASK)
#define PORT_PUEL_PTDPE0_MASK                    (0x1000000U)
#define PORT_PUEL_PTDPE0_SHIFT                   (24U)
#define PORT_PUEL_PTDPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE0_SHIFT)) & PORT_PUEL_PTDPE0_MASK)
#define PORT_PUEL_PTDPE1_MASK                    (0x2000000U)
#define PORT_PUEL_PTDPE1_SHIFT                   (25U)
#define PORT_PUEL_PTDPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE1_SHIFT)) & PORT_PUEL_PTDPE1_MASK)
#define PORT_PUEL_PTDPE2_MASK                    (0x4000000U)
#define PORT_PUEL_PTDPE2_SHIFT                   (26U)
#define PORT_PUEL_PTDPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE2_SHIFT)) & PORT_PUEL_PTDPE2_MASK)
#define PORT_PUEL_PTDPE3_MASK                    (0x8000000U)
#define PORT_PUEL_PTDPE3_SHIFT                   (27U)
#define PORT_PUEL_PTDPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE3_SHIFT)) & PORT_PUEL_PTDPE3_MASK)
#define PORT_PUEL_PTDPE4_MASK                    (0x10000000U)
#define PORT_PUEL_PTDPE4_SHIFT                   (28U)
#define PORT_PUEL_PTDPE4(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE4_SHIFT)) & PORT_PUEL_PTDPE4_MASK)
#define PORT_PUEL_PTDPE5_MASK                    (0x20000000U)
#define PORT_PUEL_PTDPE5_SHIFT                   (29U)
#define PORT_PUEL_PTDPE5(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE5_SHIFT)) & PORT_PUEL_PTDPE5_MASK)
#define PORT_PUEL_PTDPE6_MASK                    (0x40000000U)
#define PORT_PUEL_PTDPE6_SHIFT                   (30U)
#define PORT_PUEL_PTDPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE6_SHIFT)) & PORT_PUEL_PTDPE6_MASK)
#define PORT_PUEL_PTDPE7_MASK                    (0x80000000U)
#define PORT_PUEL_PTDPE7_SHIFT                   (31U)
#define PORT_PUEL_PTDPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEL_PTDPE7_SHIFT)) & PORT_PUEL_PTDPE7_MASK)

/*! @name PUEH - Port Pullup Enable High Register */
#define PORT_PUEH_PTEPE0_MASK                    (0x1U)
#define PORT_PUEH_PTEPE0_SHIFT                   (0U)
#define PORT_PUEH_PTEPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE0_SHIFT)) & PORT_PUEH_PTEPE0_MASK)
#define PORT_PUEH_PTEPE1_MASK                    (0x2U)
#define PORT_PUEH_PTEPE1_SHIFT                   (1U)
#define PORT_PUEH_PTEPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE1_SHIFT)) & PORT_PUEH_PTEPE1_MASK)
#define PORT_PUEH_PTEPE2_MASK                    (0x4U)
#define PORT_PUEH_PTEPE2_SHIFT                   (2U)
#define PORT_PUEH_PTEPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE2_SHIFT)) & PORT_PUEH_PTEPE2_MASK)
#define PORT_PUEH_PTEPE3_MASK                    (0x8U)
#define PORT_PUEH_PTEPE3_SHIFT                   (3U)
#define PORT_PUEH_PTEPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE3_SHIFT)) & PORT_PUEH_PTEPE3_MASK)
#define PORT_PUEH_PTEPE4_MASK                    (0x10U)
#define PORT_PUEH_PTEPE4_SHIFT                   (4U)
#define PORT_PUEH_PTEPE4(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE4_SHIFT)) & PORT_PUEH_PTEPE4_MASK)
#define PORT_PUEH_PTEPE5_MASK                    (0x20U)
#define PORT_PUEH_PTEPE5_SHIFT                   (5U)
#define PORT_PUEH_PTEPE5(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE5_SHIFT)) & PORT_PUEH_PTEPE5_MASK)
#define PORT_PUEH_PTEPE6_MASK                    (0x40U)
#define PORT_PUEH_PTEPE6_SHIFT                   (6U)
#define PORT_PUEH_PTEPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE6_SHIFT)) & PORT_PUEH_PTEPE6_MASK)
#define PORT_PUEH_PTEPE7_MASK                    (0x80U)
#define PORT_PUEH_PTEPE7_SHIFT                   (7U)
#define PORT_PUEH_PTEPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTEPE7_SHIFT)) & PORT_PUEH_PTEPE7_MASK)
#define PORT_PUEH_PTFPE0_MASK                    (0x100U)
#define PORT_PUEH_PTFPE0_SHIFT                   (8U)
#define PORT_PUEH_PTFPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE0_SHIFT)) & PORT_PUEH_PTFPE0_MASK)
#define PORT_PUEH_PTFPE1_MASK                    (0x200U)
#define PORT_PUEH_PTFPE1_SHIFT                   (9U)
#define PORT_PUEH_PTFPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE1_SHIFT)) & PORT_PUEH_PTFPE1_MASK)
#define PORT_PUEH_PTFPE2_MASK                    (0x400U)
#define PORT_PUEH_PTFPE2_SHIFT                   (10U)
#define PORT_PUEH_PTFPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE2_SHIFT)) & PORT_PUEH_PTFPE2_MASK)
#define PORT_PUEH_PTFPE3_MASK                    (0x800U)
#define PORT_PUEH_PTFPE3_SHIFT                   (11U)
#define PORT_PUEH_PTFPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE3_SHIFT)) & PORT_PUEH_PTFPE3_MASK)
#define PORT_PUEH_PTFPE4_MASK                    (0x1000U)
#define PORT_PUEH_PTFPE4_SHIFT                   (12U)
#define PORT_PUEH_PTFPE4(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE4_SHIFT)) & PORT_PUEH_PTFPE4_MASK)
#define PORT_PUEH_PTFPE5_MASK                    (0x2000U)
#define PORT_PUEH_PTFPE5_SHIFT                   (13U)
#define PORT_PUEH_PTFPE5(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE5_SHIFT)) & PORT_PUEH_PTFPE5_MASK)
#define PORT_PUEH_PTFPE6_MASK                    (0x4000U)
#define PORT_PUEH_PTFPE6_SHIFT                   (14U)
#define PORT_PUEH_PTFPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE6_SHIFT)) & PORT_PUEH_PTFPE6_MASK)
#define PORT_PUEH_PTFPE7_MASK                    (0x8000U)
#define PORT_PUEH_PTFPE7_SHIFT                   (15U)
#define PORT_PUEH_PTFPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTFPE7_SHIFT)) & PORT_PUEH_PTFPE7_MASK)
#define PORT_PUEH_PTGPE0_MASK                    (0x10000U)
#define PORT_PUEH_PTGPE0_SHIFT                   (16U)
#define PORT_PUEH_PTGPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTGPE0_SHIFT)) & PORT_PUEH_PTGPE0_MASK)
#define PORT_PUEH_PTGPE1_MASK                    (0x20000U)
#define PORT_PUEH_PTGPE1_SHIFT                   (17U)
#define PORT_PUEH_PTGPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTGPE1_SHIFT)) & PORT_PUEH_PTGPE1_MASK)
#define PORT_PUEH_PTGPE2_MASK                    (0x40000U)
#define PORT_PUEH_PTGPE2_SHIFT                   (18U)
#define PORT_PUEH_PTGPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTGPE2_SHIFT)) & PORT_PUEH_PTGPE2_MASK)
#define PORT_PUEH_PTGPE3_MASK                    (0x80000U)
#define PORT_PUEH_PTGPE3_SHIFT                   (19U)
#define PORT_PUEH_PTGPE3(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTGPE3_SHIFT)) & PORT_PUEH_PTGPE3_MASK)
#define PORT_PUEH_PTHPE0_MASK                    (0x1000000U)
#define PORT_PUEH_PTHPE0_SHIFT                   (24U)
#define PORT_PUEH_PTHPE0(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTHPE0_SHIFT)) & PORT_PUEH_PTHPE0_MASK)
#define PORT_PUEH_PTHPE1_MASK                    (0x2000000U)
#define PORT_PUEH_PTHPE1_SHIFT                   (25U)
#define PORT_PUEH_PTHPE1(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTHPE1_SHIFT)) & PORT_PUEH_PTHPE1_MASK)
#define PORT_PUEH_PTHPE2_MASK                    (0x4000000U)
#define PORT_PUEH_PTHPE2_SHIFT                   (26U)
#define PORT_PUEH_PTHPE2(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTHPE2_SHIFT)) & PORT_PUEH_PTHPE2_MASK)
#define PORT_PUEH_PTHPE6_MASK                    (0x40000000U)
#define PORT_PUEH_PTHPE6_SHIFT                   (30U)
#define PORT_PUEH_PTHPE6(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTHPE6_SHIFT)) & PORT_PUEH_PTHPE6_MASK)
#define PORT_PUEH_PTHPE7_MASK                    (0x80000000U)
#define PORT_PUEH_PTHPE7_SHIFT                   (31U)
#define PORT_PUEH_PTHPE7(x)                      (((uint32_t)(((uint32_t)(x)) << PORT_PUEH_PTHPE7_SHIFT)) & PORT_PUEH_PTHPE7_MASK)

/*! @name HDRVE - Port High Drive Enable Register */
#define PORT_HDRVE_PTB4_MASK                     (0x1U)
#define PORT_HDRVE_PTB4_SHIFT                    (0U)
#define PORT_HDRVE_PTB4(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTB4_SHIFT)) & PORT_HDRVE_PTB4_MASK)
#define PORT_HDRVE_PTB5_MASK                     (0x2U)
#define PORT_HDRVE_PTB5_SHIFT                    (1U)
#define PORT_HDRVE_PTB5(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTB5_SHIFT)) & PORT_HDRVE_PTB5_MASK)
#define PORT_HDRVE_PTD0_MASK                     (0x4U)
#define PORT_HDRVE_PTD0_SHIFT                    (2U)
#define PORT_HDRVE_PTD0(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTD0_SHIFT)) & PORT_HDRVE_PTD0_MASK)
#define PORT_HDRVE_PTD1_MASK                     (0x8U)
#define PORT_HDRVE_PTD1_SHIFT                    (3U)
#define PORT_HDRVE_PTD1(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTD1_SHIFT)) & PORT_HDRVE_PTD1_MASK)
#define PORT_HDRVE_PTE0_MASK                     (0x10U)
#define PORT_HDRVE_PTE0_SHIFT                    (4U)
#define PORT_HDRVE_PTE0(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTE0_SHIFT)) & PORT_HDRVE_PTE0_MASK)
#define PORT_HDRVE_PTE1_MASK                     (0x20U)
#define PORT_HDRVE_PTE1_SHIFT                    (5U)
#define PORT_HDRVE_PTE1(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTE1_SHIFT)) & PORT_HDRVE_PTE1_MASK)
#define PORT_HDRVE_PTH0_MASK                     (0x40U)
#define PORT_HDRVE_PTH0_SHIFT                    (6U)
#define PORT_HDRVE_PTH0(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTH0_SHIFT)) & PORT_HDRVE_PTH0_MASK)
#define PORT_HDRVE_PTH1_MASK                     (0x80U)
#define PORT_HDRVE_PTH1_SHIFT                    (7U)
#define PORT_HDRVE_PTH1(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_HDRVE_PTH1_SHIFT)) & PORT_HDRVE_PTH1_MASK)


/*!
 * @}
 */ /* end of group PORT_Register_Masks */


/* PORT - Peripheral instance base addresses */
/** Peripheral PORT base address */
#define PORT_BASE                                (0x40049000u)
/** Peripheral PORT base pointer */
#define PORT                                     ((PORT_Type *)PORT_BASE)
/** Array initializer of PORT peripheral base addresses */
#define PORT_BASE_ADDRS                          { PORT_BASE }
/** Array initializer of PORT peripheral base pointers */
#define PORT_BASE_PTRS                           { PORT }

/*!
 * @}
 */ /* end of group PORT_Peripheral_Access_Layer */


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
} ROM_Type;

/* ----------------------------------------------------------------------------
   -- ROM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ROM_Register_Masks ROM Register Masks
 * @{
 */

/*! @name ENTRY - Entry */
#define ROM_ENTRY_ENTRY_MASK                     (0xFFFFFFFFU)
#define ROM_ENTRY_ENTRY_SHIFT                    (0U)
#define ROM_ENTRY_ENTRY(x)                       (((uint32_t)(((uint32_t)(x)) << ROM_ENTRY_ENTRY_SHIFT)) & ROM_ENTRY_ENTRY_MASK)

/* The count of ROM_ENTRY */
#define ROM_ENTRY_COUNT                          (1U)

/*! @name TABLEMARK - End of Table Marker Register */
#define ROM_TABLEMARK_MARK_MASK                  (0xFFFFFFFFU)
#define ROM_TABLEMARK_MARK_SHIFT                 (0U)
#define ROM_TABLEMARK_MARK(x)                    (((uint32_t)(((uint32_t)(x)) << ROM_TABLEMARK_MARK_SHIFT)) & ROM_TABLEMARK_MARK_MASK)

/*! @name SYSACCESS - System Access Register */
#define ROM_SYSACCESS_SYSACCESS_MASK             (0xFFFFFFFFU)
#define ROM_SYSACCESS_SYSACCESS_SHIFT            (0U)
#define ROM_SYSACCESS_SYSACCESS(x)               (((uint32_t)(((uint32_t)(x)) << ROM_SYSACCESS_SYSACCESS_SHIFT)) & ROM_SYSACCESS_SYSACCESS_MASK)

/*! @name PERIPHID4 - Peripheral ID Register */
#define ROM_PERIPHID4_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID4_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID4_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID4_PERIPHID_SHIFT)) & ROM_PERIPHID4_PERIPHID_MASK)

/*! @name PERIPHID5 - Peripheral ID Register */
#define ROM_PERIPHID5_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID5_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID5_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID5_PERIPHID_SHIFT)) & ROM_PERIPHID5_PERIPHID_MASK)

/*! @name PERIPHID6 - Peripheral ID Register */
#define ROM_PERIPHID6_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID6_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID6_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID6_PERIPHID_SHIFT)) & ROM_PERIPHID6_PERIPHID_MASK)

/*! @name PERIPHID7 - Peripheral ID Register */
#define ROM_PERIPHID7_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID7_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID7_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID7_PERIPHID_SHIFT)) & ROM_PERIPHID7_PERIPHID_MASK)

/*! @name PERIPHID0 - Peripheral ID Register */
#define ROM_PERIPHID0_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID0_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID0_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID0_PERIPHID_SHIFT)) & ROM_PERIPHID0_PERIPHID_MASK)

/*! @name PERIPHID1 - Peripheral ID Register */
#define ROM_PERIPHID1_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID1_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID1_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID1_PERIPHID_SHIFT)) & ROM_PERIPHID1_PERIPHID_MASK)

/*! @name PERIPHID2 - Peripheral ID Register */
#define ROM_PERIPHID2_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID2_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID2_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID2_PERIPHID_SHIFT)) & ROM_PERIPHID2_PERIPHID_MASK)

/*! @name PERIPHID3 - Peripheral ID Register */
#define ROM_PERIPHID3_PERIPHID_MASK              (0xFFFFFFFFU)
#define ROM_PERIPHID3_PERIPHID_SHIFT             (0U)
#define ROM_PERIPHID3_PERIPHID(x)                (((uint32_t)(((uint32_t)(x)) << ROM_PERIPHID3_PERIPHID_SHIFT)) & ROM_PERIPHID3_PERIPHID_MASK)

/*! @name COMPID - Component ID Register */
#define ROM_COMPID_COMPID_MASK                   (0xFFFFFFFFU)
#define ROM_COMPID_COMPID_SHIFT                  (0U)
#define ROM_COMPID_COMPID(x)                     (((uint32_t)(((uint32_t)(x)) << ROM_COMPID_COMPID_SHIFT)) & ROM_COMPID_COMPID_MASK)

/* The count of ROM_COMPID */
#define ROM_COMPID_COUNT                         (4U)


/*!
 * @}
 */ /* end of group ROM_Register_Masks */


/* ROM - Peripheral instance base addresses */
/** Peripheral ROM base address */
#define ROM_BASE                                 (0xF0002000u)
/** Peripheral ROM base pointer */
#define ROM                                      ((ROM_Type *)ROM_BASE)
/** Array initializer of ROM peripheral base addresses */
#define ROM_BASE_ADDRS                           { ROM_BASE }
/** Array initializer of ROM peripheral base pointers */
#define ROM_BASE_PTRS                            { ROM }

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
} RTC_Type;

/* ----------------------------------------------------------------------------
   -- RTC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Register_Masks RTC Register Masks
 * @{
 */

/*! @name SC - RTC Status and Control Register */
#define RTC_SC_RTCO_MASK                         (0x10U)
#define RTC_SC_RTCO_SHIFT                        (4U)
#define RTC_SC_RTCO(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_SC_RTCO_SHIFT)) & RTC_SC_RTCO_MASK)
#define RTC_SC_RTIE_MASK                         (0x40U)
#define RTC_SC_RTIE_SHIFT                        (6U)
#define RTC_SC_RTIE(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_SC_RTIE_SHIFT)) & RTC_SC_RTIE_MASK)
#define RTC_SC_RTIF_MASK                         (0x80U)
#define RTC_SC_RTIF_SHIFT                        (7U)
#define RTC_SC_RTIF(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_SC_RTIF_SHIFT)) & RTC_SC_RTIF_MASK)
#define RTC_SC_RTCPS_MASK                        (0x700U)
#define RTC_SC_RTCPS_SHIFT                       (8U)
#define RTC_SC_RTCPS(x)                          (((uint32_t)(((uint32_t)(x)) << RTC_SC_RTCPS_SHIFT)) & RTC_SC_RTCPS_MASK)
#define RTC_SC_RTCLKS_MASK                       (0xC000U)
#define RTC_SC_RTCLKS_SHIFT                      (14U)
#define RTC_SC_RTCLKS(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_SC_RTCLKS_SHIFT)) & RTC_SC_RTCLKS_MASK)

/*! @name MOD - RTC Modulo Register */
#define RTC_MOD_MOD_MASK                         (0xFFFFU)
#define RTC_MOD_MOD_SHIFT                        (0U)
#define RTC_MOD_MOD(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_MOD_MOD_SHIFT)) & RTC_MOD_MOD_MASK)

/*! @name CNT - RTC Counter Register */
#define RTC_CNT_CNT_MASK                         (0xFFFFU)
#define RTC_CNT_CNT_SHIFT                        (0U)
#define RTC_CNT_CNT(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_CNT_CNT_SHIFT)) & RTC_CNT_CNT_MASK)


/*!
 * @}
 */ /* end of group RTC_Register_Masks */


/* RTC - Peripheral instance base addresses */
/** Peripheral RTC base address */
#define RTC_BASE                                 (0x4003D000u)
/** Peripheral RTC base pointer */
#define RTC                                      ((RTC_Type *)RTC_BASE)
/** Array initializer of RTC peripheral base addresses */
#define RTC_BASE_ADDRS                           { RTC_BASE }
/** Array initializer of RTC peripheral base pointers */
#define RTC_BASE_PTRS                            { RTC }
/** Interrupt vectors for the RTC peripheral type */
#define RTC_IRQS                                 { RTC_IRQn }

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
  __IO uint32_t SOPT;                              /**< System Options Register, offset: 0x4 */
  __IO uint32_t PINSEL;                            /**< Pin Selection Register, offset: 0x8 */
  __IO uint32_t SCGC;                              /**< System Clock Gating Control Register, offset: 0xC */
  __I  uint32_t UUIDL;                             /**< Universally Unique Identifier Low Register, offset: 0x10 */
  __I  uint32_t UUIDH;                             /**< Universally Unique Identifier High Register, offset: 0x14 */
  __IO uint32_t BUSDIV;                            /**< BUS Clock Divider Register, offset: 0x18 */
} SIM_Type;

/* ----------------------------------------------------------------------------
   -- SIM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SIM_Register_Masks SIM Register Masks
 * @{
 */

/*! @name SRSID - System Reset Status and ID Register */
#define SIM_SRSID_LVD_MASK                       (0x2U)
#define SIM_SRSID_LVD_SHIFT                      (1U)
#define SIM_SRSID_LVD(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_LVD_SHIFT)) & SIM_SRSID_LVD_MASK)
#define SIM_SRSID_LOC_MASK                       (0x4U)
#define SIM_SRSID_LOC_SHIFT                      (2U)
#define SIM_SRSID_LOC(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_LOC_SHIFT)) & SIM_SRSID_LOC_MASK)
#define SIM_SRSID_WDOG_MASK                      (0x20U)
#define SIM_SRSID_WDOG_SHIFT                     (5U)
#define SIM_SRSID_WDOG(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_WDOG_SHIFT)) & SIM_SRSID_WDOG_MASK)
#define SIM_SRSID_PIN_MASK                       (0x40U)
#define SIM_SRSID_PIN_SHIFT                      (6U)
#define SIM_SRSID_PIN(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_PIN_SHIFT)) & SIM_SRSID_PIN_MASK)
#define SIM_SRSID_POR_MASK                       (0x80U)
#define SIM_SRSID_POR_SHIFT                      (7U)
#define SIM_SRSID_POR(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_POR_SHIFT)) & SIM_SRSID_POR_MASK)
#define SIM_SRSID_LOCKUP_MASK                    (0x200U)
#define SIM_SRSID_LOCKUP_SHIFT                   (9U)
#define SIM_SRSID_LOCKUP(x)                      (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_LOCKUP_SHIFT)) & SIM_SRSID_LOCKUP_MASK)
#define SIM_SRSID_SW_MASK                        (0x400U)
#define SIM_SRSID_SW_SHIFT                       (10U)
#define SIM_SRSID_SW(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_SW_SHIFT)) & SIM_SRSID_SW_MASK)
#define SIM_SRSID_MDMAP_MASK                     (0x800U)
#define SIM_SRSID_MDMAP_SHIFT                    (11U)
#define SIM_SRSID_MDMAP(x)                       (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_MDMAP_SHIFT)) & SIM_SRSID_MDMAP_MASK)
#define SIM_SRSID_SACKERR_MASK                   (0x2000U)
#define SIM_SRSID_SACKERR_SHIFT                  (13U)
#define SIM_SRSID_SACKERR(x)                     (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_SACKERR_SHIFT)) & SIM_SRSID_SACKERR_MASK)
#define SIM_SRSID_PINID_MASK                     (0xF0000U)
#define SIM_SRSID_PINID_SHIFT                    (16U)
#define SIM_SRSID_PINID(x)                       (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_PINID_SHIFT)) & SIM_SRSID_PINID_MASK)
#define SIM_SRSID_RevID_MASK                     (0xF00000U)
#define SIM_SRSID_RevID_SHIFT                    (20U)
#define SIM_SRSID_RevID(x)                       (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_RevID_SHIFT)) & SIM_SRSID_RevID_MASK)
#define SIM_SRSID_SUBFAMID_MASK                  (0xF000000U)
#define SIM_SRSID_SUBFAMID_SHIFT                 (24U)
#define SIM_SRSID_SUBFAMID(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_SUBFAMID_SHIFT)) & SIM_SRSID_SUBFAMID_MASK)
#define SIM_SRSID_FAMID_MASK                     (0xF0000000U)
#define SIM_SRSID_FAMID_SHIFT                    (28U)
#define SIM_SRSID_FAMID(x)                       (((uint32_t)(((uint32_t)(x)) << SIM_SRSID_FAMID_SHIFT)) & SIM_SRSID_FAMID_MASK)

/*! @name SOPT - System Options Register */
#define SIM_SOPT_NMIE_MASK                       (0x2U)
#define SIM_SOPT_NMIE_SHIFT                      (1U)
#define SIM_SOPT_NMIE(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_NMIE_SHIFT)) & SIM_SOPT_NMIE_MASK)
#define SIM_SOPT_RSTPE_MASK                      (0x4U)
#define SIM_SOPT_RSTPE_SHIFT                     (2U)
#define SIM_SOPT_RSTPE(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_RSTPE_SHIFT)) & SIM_SOPT_RSTPE_MASK)
#define SIM_SOPT_SWDE_MASK                       (0x8U)
#define SIM_SOPT_SWDE_SHIFT                      (3U)
#define SIM_SOPT_SWDE(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_SWDE_SHIFT)) & SIM_SOPT_SWDE_MASK)
#define SIM_SOPT_ADHWT_MASK                      (0x300U)
#define SIM_SOPT_ADHWT_SHIFT                     (8U)
#define SIM_SOPT_ADHWT(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_ADHWT_SHIFT)) & SIM_SOPT_ADHWT_MASK)
#define SIM_SOPT_RTCC_MASK                       (0x400U)
#define SIM_SOPT_RTCC_SHIFT                      (10U)
#define SIM_SOPT_RTCC(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_RTCC_SHIFT)) & SIM_SOPT_RTCC_MASK)
#define SIM_SOPT_ACIC_MASK                       (0x800U)
#define SIM_SOPT_ACIC_SHIFT                      (11U)
#define SIM_SOPT_ACIC(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_ACIC_SHIFT)) & SIM_SOPT_ACIC_MASK)
#define SIM_SOPT_RXDCE_MASK                      (0x1000U)
#define SIM_SOPT_RXDCE_SHIFT                     (12U)
#define SIM_SOPT_RXDCE(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_RXDCE_SHIFT)) & SIM_SOPT_RXDCE_MASK)
#define SIM_SOPT_RXDFE_MASK                      (0x2000U)
#define SIM_SOPT_RXDFE_SHIFT                     (13U)
#define SIM_SOPT_RXDFE(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_RXDFE_SHIFT)) & SIM_SOPT_RXDFE_MASK)
#define SIM_SOPT_FTMSYNC_MASK                    (0x4000U)
#define SIM_SOPT_FTMSYNC_SHIFT                   (14U)
#define SIM_SOPT_FTMSYNC(x)                      (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_FTMSYNC_SHIFT)) & SIM_SOPT_FTMSYNC_MASK)
#define SIM_SOPT_TXDME_MASK                      (0x8000U)
#define SIM_SOPT_TXDME_SHIFT                     (15U)
#define SIM_SOPT_TXDME(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_TXDME_SHIFT)) & SIM_SOPT_TXDME_MASK)
#define SIM_SOPT_BUSREF_MASK                     (0x70000U)
#define SIM_SOPT_BUSREF_SHIFT                    (16U)
#define SIM_SOPT_BUSREF(x)                       (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_BUSREF_SHIFT)) & SIM_SOPT_BUSREF_MASK)
#define SIM_SOPT_CLKOE_MASK                      (0x80000U)
#define SIM_SOPT_CLKOE_SHIFT                     (19U)
#define SIM_SOPT_CLKOE(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_CLKOE_SHIFT)) & SIM_SOPT_CLKOE_MASK)
#define SIM_SOPT_DLYACT_MASK                     (0x800000U)
#define SIM_SOPT_DLYACT_SHIFT                    (23U)
#define SIM_SOPT_DLYACT(x)                       (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_DLYACT_SHIFT)) & SIM_SOPT_DLYACT_MASK)
#define SIM_SOPT_DELAY_MASK                      (0xFF000000U)
#define SIM_SOPT_DELAY_SHIFT                     (24U)
#define SIM_SOPT_DELAY(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SOPT_DELAY_SHIFT)) & SIM_SOPT_DELAY_MASK)

/*! @name PINSEL - Pin Selection Register */
#define SIM_PINSEL_RTCPS_MASK                    (0x10U)
#define SIM_PINSEL_RTCPS_SHIFT                   (4U)
#define SIM_PINSEL_RTCPS(x)                      (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_RTCPS_SHIFT)) & SIM_PINSEL_RTCPS_MASK)
#define SIM_PINSEL_I2C0PS_MASK                   (0x20U)
#define SIM_PINSEL_I2C0PS_SHIFT                  (5U)
#define SIM_PINSEL_I2C0PS(x)                     (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_I2C0PS_SHIFT)) & SIM_PINSEL_I2C0PS_MASK)
#define SIM_PINSEL_SPI0PS_MASK                   (0x40U)
#define SIM_PINSEL_SPI0PS_SHIFT                  (6U)
#define SIM_PINSEL_SPI0PS(x)                     (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_SPI0PS_SHIFT)) & SIM_PINSEL_SPI0PS_MASK)
#define SIM_PINSEL_UART0PS_MASK                  (0x80U)
#define SIM_PINSEL_UART0PS_SHIFT                 (7U)
#define SIM_PINSEL_UART0PS(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_UART0PS_SHIFT)) & SIM_PINSEL_UART0PS_MASK)
#define SIM_PINSEL_FTM0PS0_MASK                  (0x100U)
#define SIM_PINSEL_FTM0PS0_SHIFT                 (8U)
#define SIM_PINSEL_FTM0PS0(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM0PS0_SHIFT)) & SIM_PINSEL_FTM0PS0_MASK)
#define SIM_PINSEL_FTM0PS1_MASK                  (0x200U)
#define SIM_PINSEL_FTM0PS1_SHIFT                 (9U)
#define SIM_PINSEL_FTM0PS1(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM0PS1_SHIFT)) & SIM_PINSEL_FTM0PS1_MASK)
#define SIM_PINSEL_FTM1PS0_MASK                  (0x400U)
#define SIM_PINSEL_FTM1PS0_SHIFT                 (10U)
#define SIM_PINSEL_FTM1PS0(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM1PS0_SHIFT)) & SIM_PINSEL_FTM1PS0_MASK)
#define SIM_PINSEL_FTM1PS1_MASK                  (0x800U)
#define SIM_PINSEL_FTM1PS1_SHIFT                 (11U)
#define SIM_PINSEL_FTM1PS1(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM1PS1_SHIFT)) & SIM_PINSEL_FTM1PS1_MASK)
#define SIM_PINSEL_FTM2PS0_MASK                  (0x1000U)
#define SIM_PINSEL_FTM2PS0_SHIFT                 (12U)
#define SIM_PINSEL_FTM2PS0(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM2PS0_SHIFT)) & SIM_PINSEL_FTM2PS0_MASK)
#define SIM_PINSEL_FTM2PS1_MASK                  (0x2000U)
#define SIM_PINSEL_FTM2PS1_SHIFT                 (13U)
#define SIM_PINSEL_FTM2PS1(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM2PS1_SHIFT)) & SIM_PINSEL_FTM2PS1_MASK)
#define SIM_PINSEL_FTM2PS2_MASK                  (0x4000U)
#define SIM_PINSEL_FTM2PS2_SHIFT                 (14U)
#define SIM_PINSEL_FTM2PS2(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM2PS2_SHIFT)) & SIM_PINSEL_FTM2PS2_MASK)
#define SIM_PINSEL_FTM2PS3_MASK                  (0x8000U)
#define SIM_PINSEL_FTM2PS3_SHIFT                 (15U)
#define SIM_PINSEL_FTM2PS3(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_PINSEL_FTM2PS3_SHIFT)) & SIM_PINSEL_FTM2PS3_MASK)

/*! @name SCGC - System Clock Gating Control Register */
#define SIM_SCGC_RTC_MASK                        (0x1U)
#define SIM_SCGC_RTC_SHIFT                       (0U)
#define SIM_SCGC_RTC(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_RTC_SHIFT)) & SIM_SCGC_RTC_MASK)
#define SIM_SCGC_PIT_MASK                        (0x2U)
#define SIM_SCGC_PIT_SHIFT                       (1U)
#define SIM_SCGC_PIT(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_PIT_SHIFT)) & SIM_SCGC_PIT_MASK)
#define SIM_SCGC_FTM0_MASK                       (0x20U)
#define SIM_SCGC_FTM0_SHIFT                      (5U)
#define SIM_SCGC_FTM0(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_FTM0_SHIFT)) & SIM_SCGC_FTM0_MASK)
#define SIM_SCGC_FTM1_MASK                       (0x40U)
#define SIM_SCGC_FTM1_SHIFT                      (6U)
#define SIM_SCGC_FTM1(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_FTM1_SHIFT)) & SIM_SCGC_FTM1_MASK)
#define SIM_SCGC_FTM2_MASK                       (0x80U)
#define SIM_SCGC_FTM2_SHIFT                      (7U)
#define SIM_SCGC_FTM2(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_FTM2_SHIFT)) & SIM_SCGC_FTM2_MASK)
#define SIM_SCGC_CRC_MASK                        (0x400U)
#define SIM_SCGC_CRC_SHIFT                       (10U)
#define SIM_SCGC_CRC(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_CRC_SHIFT)) & SIM_SCGC_CRC_MASK)
#define SIM_SCGC_FLASH_MASK                      (0x1000U)
#define SIM_SCGC_FLASH_SHIFT                     (12U)
#define SIM_SCGC_FLASH(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_FLASH_SHIFT)) & SIM_SCGC_FLASH_MASK)
#define SIM_SCGC_SWD_MASK                        (0x2000U)
#define SIM_SCGC_SWD_SHIFT                       (13U)
#define SIM_SCGC_SWD(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_SWD_SHIFT)) & SIM_SCGC_SWD_MASK)
#define SIM_SCGC_I2C_MASK                        (0x20000U)
#define SIM_SCGC_I2C_SHIFT                       (17U)
#define SIM_SCGC_I2C(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_I2C_SHIFT)) & SIM_SCGC_I2C_MASK)
#define SIM_SCGC_SPI0_MASK                       (0x40000U)
#define SIM_SCGC_SPI0_SHIFT                      (18U)
#define SIM_SCGC_SPI0(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_SPI0_SHIFT)) & SIM_SCGC_SPI0_MASK)
#define SIM_SCGC_SPI1_MASK                       (0x80000U)
#define SIM_SCGC_SPI1_SHIFT                      (19U)
#define SIM_SCGC_SPI1(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_SPI1_SHIFT)) & SIM_SCGC_SPI1_MASK)
#define SIM_SCGC_UART0_MASK                      (0x100000U)
#define SIM_SCGC_UART0_SHIFT                     (20U)
#define SIM_SCGC_UART0(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_UART0_SHIFT)) & SIM_SCGC_UART0_MASK)
#define SIM_SCGC_UART1_MASK                      (0x200000U)
#define SIM_SCGC_UART1_SHIFT                     (21U)
#define SIM_SCGC_UART1(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_UART1_SHIFT)) & SIM_SCGC_UART1_MASK)
#define SIM_SCGC_UART2_MASK                      (0x400000U)
#define SIM_SCGC_UART2_SHIFT                     (22U)
#define SIM_SCGC_UART2(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_UART2_SHIFT)) & SIM_SCGC_UART2_MASK)
#define SIM_SCGC_KBI0_MASK                       (0x1000000U)
#define SIM_SCGC_KBI0_SHIFT                      (24U)
#define SIM_SCGC_KBI0(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_KBI0_SHIFT)) & SIM_SCGC_KBI0_MASK)
#define SIM_SCGC_KBI1_MASK                       (0x2000000U)
#define SIM_SCGC_KBI1_SHIFT                      (25U)
#define SIM_SCGC_KBI1(x)                         (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_KBI1_SHIFT)) & SIM_SCGC_KBI1_MASK)
#define SIM_SCGC_IRQ_MASK                        (0x8000000U)
#define SIM_SCGC_IRQ_SHIFT                       (27U)
#define SIM_SCGC_IRQ(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_IRQ_SHIFT)) & SIM_SCGC_IRQ_MASK)
#define SIM_SCGC_ADC_MASK                        (0x20000000U)
#define SIM_SCGC_ADC_SHIFT                       (29U)
#define SIM_SCGC_ADC(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_ADC_SHIFT)) & SIM_SCGC_ADC_MASK)
#define SIM_SCGC_ACMP0_MASK                      (0x40000000U)
#define SIM_SCGC_ACMP0_SHIFT                     (30U)
#define SIM_SCGC_ACMP0(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_ACMP0_SHIFT)) & SIM_SCGC_ACMP0_MASK)
#define SIM_SCGC_ACMP1_MASK                      (0x80000000U)
#define SIM_SCGC_ACMP1_SHIFT                     (31U)
#define SIM_SCGC_ACMP1(x)                        (((uint32_t)(((uint32_t)(x)) << SIM_SCGC_ACMP1_SHIFT)) & SIM_SCGC_ACMP1_MASK)

/*! @name UUIDL - Universally Unique Identifier Low Register */
#define SIM_UUIDL_ID_MASK                        (0xFFFFFFFFU)
#define SIM_UUIDL_ID_SHIFT                       (0U)
#define SIM_UUIDL_ID(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_UUIDL_ID_SHIFT)) & SIM_UUIDL_ID_MASK)

/*! @name UUIDH - Universally Unique Identifier High Register */
#define SIM_UUIDH_ID_MASK                        (0xFFFFFFFFU)
#define SIM_UUIDH_ID_SHIFT                       (0U)
#define SIM_UUIDH_ID(x)                          (((uint32_t)(((uint32_t)(x)) << SIM_UUIDH_ID_SHIFT)) & SIM_UUIDH_ID_MASK)

/*! @name BUSDIV - BUS Clock Divider Register */
#define SIM_BUSDIV_BUSDIV_MASK                   (0x1U)
#define SIM_BUSDIV_BUSDIV_SHIFT                  (0U)
#define SIM_BUSDIV_BUSDIV(x)                     (((uint32_t)(((uint32_t)(x)) << SIM_BUSDIV_BUSDIV_SHIFT)) & SIM_BUSDIV_BUSDIV_MASK)


/*!
 * @}
 */ /* end of group SIM_Register_Masks */


/* SIM - Peripheral instance base addresses */
/** Peripheral SIM base address */
#define SIM_BASE                                 (0x40048000u)
/** Peripheral SIM base pointer */
#define SIM                                      ((SIM_Type *)SIM_BASE)
/** Array initializer of SIM peripheral base addresses */
#define SIM_BASE_ADDRS                           { SIM_BASE }
/** Array initializer of SIM peripheral base pointers */
#define SIM_BASE_PTRS                            { SIM }

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
} SPI_Type;

/* ----------------------------------------------------------------------------
   -- SPI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Register_Masks SPI Register Masks
 * @{
 */

/*! @name C1 - SPI Control Register 1 */
#define SPI_C1_LSBFE_MASK                        (0x1U)
#define SPI_C1_LSBFE_SHIFT                       (0U)
#define SPI_C1_LSBFE(x)                          (((uint8_t)(((uint8_t)(x)) << SPI_C1_LSBFE_SHIFT)) & SPI_C1_LSBFE_MASK)
#define SPI_C1_SSOE_MASK                         (0x2U)
#define SPI_C1_SSOE_SHIFT                        (1U)
#define SPI_C1_SSOE(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_C1_SSOE_SHIFT)) & SPI_C1_SSOE_MASK)
#define SPI_C1_CPHA_MASK                         (0x4U)
#define SPI_C1_CPHA_SHIFT                        (2U)
#define SPI_C1_CPHA(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_C1_CPHA_SHIFT)) & SPI_C1_CPHA_MASK)
#define SPI_C1_CPOL_MASK                         (0x8U)
#define SPI_C1_CPOL_SHIFT                        (3U)
#define SPI_C1_CPOL(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_C1_CPOL_SHIFT)) & SPI_C1_CPOL_MASK)
#define SPI_C1_MSTR_MASK                         (0x10U)
#define SPI_C1_MSTR_SHIFT                        (4U)
#define SPI_C1_MSTR(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_C1_MSTR_SHIFT)) & SPI_C1_MSTR_MASK)
#define SPI_C1_SPTIE_MASK                        (0x20U)
#define SPI_C1_SPTIE_SHIFT                       (5U)
#define SPI_C1_SPTIE(x)                          (((uint8_t)(((uint8_t)(x)) << SPI_C1_SPTIE_SHIFT)) & SPI_C1_SPTIE_MASK)
#define SPI_C1_SPE_MASK                          (0x40U)
#define SPI_C1_SPE_SHIFT                         (6U)
#define SPI_C1_SPE(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_C1_SPE_SHIFT)) & SPI_C1_SPE_MASK)
#define SPI_C1_SPIE_MASK                         (0x80U)
#define SPI_C1_SPIE_SHIFT                        (7U)
#define SPI_C1_SPIE(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_C1_SPIE_SHIFT)) & SPI_C1_SPIE_MASK)

/*! @name C2 - SPI Control Register 2 */
#define SPI_C2_SPC0_MASK                         (0x1U)
#define SPI_C2_SPC0_SHIFT                        (0U)
#define SPI_C2_SPC0(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_C2_SPC0_SHIFT)) & SPI_C2_SPC0_MASK)
#define SPI_C2_SPISWAI_MASK                      (0x2U)
#define SPI_C2_SPISWAI_SHIFT                     (1U)
#define SPI_C2_SPISWAI(x)                        (((uint8_t)(((uint8_t)(x)) << SPI_C2_SPISWAI_SHIFT)) & SPI_C2_SPISWAI_MASK)
#define SPI_C2_BIDIROE_MASK                      (0x8U)
#define SPI_C2_BIDIROE_SHIFT                     (3U)
#define SPI_C2_BIDIROE(x)                        (((uint8_t)(((uint8_t)(x)) << SPI_C2_BIDIROE_SHIFT)) & SPI_C2_BIDIROE_MASK)
#define SPI_C2_MODFEN_MASK                       (0x10U)
#define SPI_C2_MODFEN_SHIFT                      (4U)
#define SPI_C2_MODFEN(x)                         (((uint8_t)(((uint8_t)(x)) << SPI_C2_MODFEN_SHIFT)) & SPI_C2_MODFEN_MASK)
#define SPI_C2_SPMIE_MASK                        (0x80U)
#define SPI_C2_SPMIE_SHIFT                       (7U)
#define SPI_C2_SPMIE(x)                          (((uint8_t)(((uint8_t)(x)) << SPI_C2_SPMIE_SHIFT)) & SPI_C2_SPMIE_MASK)

/*! @name BR - SPI Baud Rate Register */
#define SPI_BR_SPR_MASK                          (0xFU)
#define SPI_BR_SPR_SHIFT                         (0U)
#define SPI_BR_SPR(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_BR_SPR_SHIFT)) & SPI_BR_SPR_MASK)
#define SPI_BR_SPPR_MASK                         (0x70U)
#define SPI_BR_SPPR_SHIFT                        (4U)
#define SPI_BR_SPPR(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_BR_SPPR_SHIFT)) & SPI_BR_SPPR_MASK)

/*! @name S - SPI Status Register */
#define SPI_S_MODF_MASK                          (0x10U)
#define SPI_S_MODF_SHIFT                         (4U)
#define SPI_S_MODF(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_S_MODF_SHIFT)) & SPI_S_MODF_MASK)
#define SPI_S_SPTEF_MASK                         (0x20U)
#define SPI_S_SPTEF_SHIFT                        (5U)
#define SPI_S_SPTEF(x)                           (((uint8_t)(((uint8_t)(x)) << SPI_S_SPTEF_SHIFT)) & SPI_S_SPTEF_MASK)
#define SPI_S_SPMF_MASK                          (0x40U)
#define SPI_S_SPMF_SHIFT                         (6U)
#define SPI_S_SPMF(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_S_SPMF_SHIFT)) & SPI_S_SPMF_MASK)
#define SPI_S_SPRF_MASK                          (0x80U)
#define SPI_S_SPRF_SHIFT                         (7U)
#define SPI_S_SPRF(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_S_SPRF_SHIFT)) & SPI_S_SPRF_MASK)

/*! @name D - SPI Data Register */
#define SPI_D_Bits_MASK                          (0xFFU)
#define SPI_D_Bits_SHIFT                         (0U)
#define SPI_D_Bits(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_D_Bits_SHIFT)) & SPI_D_Bits_MASK)

/*! @name M - SPI Match Register */
#define SPI_M_Bits_MASK                          (0xFFU)
#define SPI_M_Bits_SHIFT                         (0U)
#define SPI_M_Bits(x)                            (((uint8_t)(((uint8_t)(x)) << SPI_M_Bits_SHIFT)) & SPI_M_Bits_MASK)


/*!
 * @}
 */ /* end of group SPI_Register_Masks */


/* SPI - Peripheral instance base addresses */
/** Peripheral SPI0 base address */
#define SPI0_BASE                                (0x40076000u)
/** Peripheral SPI0 base pointer */
#define SPI0                                     ((SPI_Type *)SPI0_BASE)
/** Peripheral SPI1 base address */
#define SPI1_BASE                                (0x40077000u)
/** Peripheral SPI1 base pointer */
#define SPI1                                     ((SPI_Type *)SPI1_BASE)
/** Array initializer of SPI peripheral base addresses */
#define SPI_BASE_ADDRS                           { SPI0_BASE, SPI1_BASE }
/** Array initializer of SPI peripheral base pointers */
#define SPI_BASE_PTRS                            { SPI0, SPI1 }
/** Interrupt vectors for the SPI peripheral type */
#define SPI_IRQS                                 { SPI0_IRQn, SPI1_IRQn }

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
} UART_Type;

/* ----------------------------------------------------------------------------
   -- UART Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Register_Masks UART Register Masks
 * @{
 */

/*! @name BDH - UART Baud Rate Register: High */
#define UART_BDH_SBR_MASK                        (0x1FU)
#define UART_BDH_SBR_SHIFT                       (0U)
#define UART_BDH_SBR(x)                          (((uint8_t)(((uint8_t)(x)) << UART_BDH_SBR_SHIFT)) & UART_BDH_SBR_MASK)
#define UART_BDH_SBNS_MASK                       (0x20U)
#define UART_BDH_SBNS_SHIFT                      (5U)
#define UART_BDH_SBNS(x)                         (((uint8_t)(((uint8_t)(x)) << UART_BDH_SBNS_SHIFT)) & UART_BDH_SBNS_MASK)
#define UART_BDH_RXEDGIE_MASK                    (0x40U)
#define UART_BDH_RXEDGIE_SHIFT                   (6U)
#define UART_BDH_RXEDGIE(x)                      (((uint8_t)(((uint8_t)(x)) << UART_BDH_RXEDGIE_SHIFT)) & UART_BDH_RXEDGIE_MASK)
#define UART_BDH_LBKDIE_MASK                     (0x80U)
#define UART_BDH_LBKDIE_SHIFT                    (7U)
#define UART_BDH_LBKDIE(x)                       (((uint8_t)(((uint8_t)(x)) << UART_BDH_LBKDIE_SHIFT)) & UART_BDH_LBKDIE_MASK)

/*! @name BDL - UART Baud Rate Register: Low */
#define UART_BDL_SBR_MASK                        (0xFFU)
#define UART_BDL_SBR_SHIFT                       (0U)
#define UART_BDL_SBR(x)                          (((uint8_t)(((uint8_t)(x)) << UART_BDL_SBR_SHIFT)) & UART_BDL_SBR_MASK)

/*! @name C1 - UART Control Register 1 */
#define UART_C1_PT_MASK                          (0x1U)
#define UART_C1_PT_SHIFT                         (0U)
#define UART_C1_PT(x)                            (((uint8_t)(((uint8_t)(x)) << UART_C1_PT_SHIFT)) & UART_C1_PT_MASK)
#define UART_C1_PE_MASK                          (0x2U)
#define UART_C1_PE_SHIFT                         (1U)
#define UART_C1_PE(x)                            (((uint8_t)(((uint8_t)(x)) << UART_C1_PE_SHIFT)) & UART_C1_PE_MASK)
#define UART_C1_ILT_MASK                         (0x4U)
#define UART_C1_ILT_SHIFT                        (2U)
#define UART_C1_ILT(x)                           (((uint8_t)(((uint8_t)(x)) << UART_C1_ILT_SHIFT)) & UART_C1_ILT_MASK)
#define UART_C1_WAKE_MASK                        (0x8U)
#define UART_C1_WAKE_SHIFT                       (3U)
#define UART_C1_WAKE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C1_WAKE_SHIFT)) & UART_C1_WAKE_MASK)
#define UART_C1_M_MASK                           (0x10U)
#define UART_C1_M_SHIFT                          (4U)
#define UART_C1_M(x)                             (((uint8_t)(((uint8_t)(x)) << UART_C1_M_SHIFT)) & UART_C1_M_MASK)
#define UART_C1_RSRC_MASK                        (0x20U)
#define UART_C1_RSRC_SHIFT                       (5U)
#define UART_C1_RSRC(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C1_RSRC_SHIFT)) & UART_C1_RSRC_MASK)
#define UART_C1_UARTSWAI_MASK                    (0x40U)
#define UART_C1_UARTSWAI_SHIFT                   (6U)
#define UART_C1_UARTSWAI(x)                      (((uint8_t)(((uint8_t)(x)) << UART_C1_UARTSWAI_SHIFT)) & UART_C1_UARTSWAI_MASK)
#define UART_C1_LOOPS_MASK                       (0x80U)
#define UART_C1_LOOPS_SHIFT                      (7U)
#define UART_C1_LOOPS(x)                         (((uint8_t)(((uint8_t)(x)) << UART_C1_LOOPS_SHIFT)) & UART_C1_LOOPS_MASK)

/*! @name C2 - UART Control Register 2 */
#define UART_C2_SBK_MASK                         (0x1U)
#define UART_C2_SBK_SHIFT                        (0U)
#define UART_C2_SBK(x)                           (((uint8_t)(((uint8_t)(x)) << UART_C2_SBK_SHIFT)) & UART_C2_SBK_MASK)
#define UART_C2_RWU_MASK                         (0x2U)
#define UART_C2_RWU_SHIFT                        (1U)
#define UART_C2_RWU(x)                           (((uint8_t)(((uint8_t)(x)) << UART_C2_RWU_SHIFT)) & UART_C2_RWU_MASK)
#define UART_C2_RE_MASK                          (0x4U)
#define UART_C2_RE_SHIFT                         (2U)
#define UART_C2_RE(x)                            (((uint8_t)(((uint8_t)(x)) << UART_C2_RE_SHIFT)) & UART_C2_RE_MASK)
#define UART_C2_TE_MASK                          (0x8U)
#define UART_C2_TE_SHIFT                         (3U)
#define UART_C2_TE(x)                            (((uint8_t)(((uint8_t)(x)) << UART_C2_TE_SHIFT)) & UART_C2_TE_MASK)
#define UART_C2_ILIE_MASK                        (0x10U)
#define UART_C2_ILIE_SHIFT                       (4U)
#define UART_C2_ILIE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C2_ILIE_SHIFT)) & UART_C2_ILIE_MASK)
#define UART_C2_RIE_MASK                         (0x20U)
#define UART_C2_RIE_SHIFT                        (5U)
#define UART_C2_RIE(x)                           (((uint8_t)(((uint8_t)(x)) << UART_C2_RIE_SHIFT)) & UART_C2_RIE_MASK)
#define UART_C2_TCIE_MASK                        (0x40U)
#define UART_C2_TCIE_SHIFT                       (6U)
#define UART_C2_TCIE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C2_TCIE_SHIFT)) & UART_C2_TCIE_MASK)
#define UART_C2_TIE_MASK                         (0x80U)
#define UART_C2_TIE_SHIFT                        (7U)
#define UART_C2_TIE(x)                           (((uint8_t)(((uint8_t)(x)) << UART_C2_TIE_SHIFT)) & UART_C2_TIE_MASK)

/*! @name S1 - UART Status Register 1 */
#define UART_S1_PF_MASK                          (0x1U)
#define UART_S1_PF_SHIFT                         (0U)
#define UART_S1_PF(x)                            (((uint8_t)(((uint8_t)(x)) << UART_S1_PF_SHIFT)) & UART_S1_PF_MASK)
#define UART_S1_FE_MASK                          (0x2U)
#define UART_S1_FE_SHIFT                         (1U)
#define UART_S1_FE(x)                            (((uint8_t)(((uint8_t)(x)) << UART_S1_FE_SHIFT)) & UART_S1_FE_MASK)
#define UART_S1_NF_MASK                          (0x4U)
#define UART_S1_NF_SHIFT                         (2U)
#define UART_S1_NF(x)                            (((uint8_t)(((uint8_t)(x)) << UART_S1_NF_SHIFT)) & UART_S1_NF_MASK)
#define UART_S1_OR_MASK                          (0x8U)
#define UART_S1_OR_SHIFT                         (3U)
#define UART_S1_OR(x)                            (((uint8_t)(((uint8_t)(x)) << UART_S1_OR_SHIFT)) & UART_S1_OR_MASK)
#define UART_S1_IDLE_MASK                        (0x10U)
#define UART_S1_IDLE_SHIFT                       (4U)
#define UART_S1_IDLE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_S1_IDLE_SHIFT)) & UART_S1_IDLE_MASK)
#define UART_S1_RDRF_MASK                        (0x20U)
#define UART_S1_RDRF_SHIFT                       (5U)
#define UART_S1_RDRF(x)                          (((uint8_t)(((uint8_t)(x)) << UART_S1_RDRF_SHIFT)) & UART_S1_RDRF_MASK)
#define UART_S1_TC_MASK                          (0x40U)
#define UART_S1_TC_SHIFT                         (6U)
#define UART_S1_TC(x)                            (((uint8_t)(((uint8_t)(x)) << UART_S1_TC_SHIFT)) & UART_S1_TC_MASK)
#define UART_S1_TDRE_MASK                        (0x80U)
#define UART_S1_TDRE_SHIFT                       (7U)
#define UART_S1_TDRE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_S1_TDRE_SHIFT)) & UART_S1_TDRE_MASK)

/*! @name S2 - UART Status Register 2 */
#define UART_S2_RAF_MASK                         (0x1U)
#define UART_S2_RAF_SHIFT                        (0U)
#define UART_S2_RAF(x)                           (((uint8_t)(((uint8_t)(x)) << UART_S2_RAF_SHIFT)) & UART_S2_RAF_MASK)
#define UART_S2_LBKDE_MASK                       (0x2U)
#define UART_S2_LBKDE_SHIFT                      (1U)
#define UART_S2_LBKDE(x)                         (((uint8_t)(((uint8_t)(x)) << UART_S2_LBKDE_SHIFT)) & UART_S2_LBKDE_MASK)
#define UART_S2_BRK13_MASK                       (0x4U)
#define UART_S2_BRK13_SHIFT                      (2U)
#define UART_S2_BRK13(x)                         (((uint8_t)(((uint8_t)(x)) << UART_S2_BRK13_SHIFT)) & UART_S2_BRK13_MASK)
#define UART_S2_RWUID_MASK                       (0x8U)
#define UART_S2_RWUID_SHIFT                      (3U)
#define UART_S2_RWUID(x)                         (((uint8_t)(((uint8_t)(x)) << UART_S2_RWUID_SHIFT)) & UART_S2_RWUID_MASK)
#define UART_S2_RXINV_MASK                       (0x10U)
#define UART_S2_RXINV_SHIFT                      (4U)
#define UART_S2_RXINV(x)                         (((uint8_t)(((uint8_t)(x)) << UART_S2_RXINV_SHIFT)) & UART_S2_RXINV_MASK)
#define UART_S2_RXEDGIF_MASK                     (0x40U)
#define UART_S2_RXEDGIF_SHIFT                    (6U)
#define UART_S2_RXEDGIF(x)                       (((uint8_t)(((uint8_t)(x)) << UART_S2_RXEDGIF_SHIFT)) & UART_S2_RXEDGIF_MASK)
#define UART_S2_LBKDIF_MASK                      (0x80U)
#define UART_S2_LBKDIF_SHIFT                     (7U)
#define UART_S2_LBKDIF(x)                        (((uint8_t)(((uint8_t)(x)) << UART_S2_LBKDIF_SHIFT)) & UART_S2_LBKDIF_MASK)

/*! @name C3 - UART Control Register 3 */
#define UART_C3_PEIE_MASK                        (0x1U)
#define UART_C3_PEIE_SHIFT                       (0U)
#define UART_C3_PEIE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C3_PEIE_SHIFT)) & UART_C3_PEIE_MASK)
#define UART_C3_FEIE_MASK                        (0x2U)
#define UART_C3_FEIE_SHIFT                       (1U)
#define UART_C3_FEIE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C3_FEIE_SHIFT)) & UART_C3_FEIE_MASK)
#define UART_C3_NEIE_MASK                        (0x4U)
#define UART_C3_NEIE_SHIFT                       (2U)
#define UART_C3_NEIE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C3_NEIE_SHIFT)) & UART_C3_NEIE_MASK)
#define UART_C3_ORIE_MASK                        (0x8U)
#define UART_C3_ORIE_SHIFT                       (3U)
#define UART_C3_ORIE(x)                          (((uint8_t)(((uint8_t)(x)) << UART_C3_ORIE_SHIFT)) & UART_C3_ORIE_MASK)
#define UART_C3_TXINV_MASK                       (0x10U)
#define UART_C3_TXINV_SHIFT                      (4U)
#define UART_C3_TXINV(x)                         (((uint8_t)(((uint8_t)(x)) << UART_C3_TXINV_SHIFT)) & UART_C3_TXINV_MASK)
#define UART_C3_TXDIR_MASK                       (0x20U)
#define UART_C3_TXDIR_SHIFT                      (5U)
#define UART_C3_TXDIR(x)                         (((uint8_t)(((uint8_t)(x)) << UART_C3_TXDIR_SHIFT)) & UART_C3_TXDIR_MASK)
#define UART_C3_T8_MASK                          (0x40U)
#define UART_C3_T8_SHIFT                         (6U)
#define UART_C3_T8(x)                            (((uint8_t)(((uint8_t)(x)) << UART_C3_T8_SHIFT)) & UART_C3_T8_MASK)
#define UART_C3_R8_MASK                          (0x80U)
#define UART_C3_R8_SHIFT                         (7U)
#define UART_C3_R8(x)                            (((uint8_t)(((uint8_t)(x)) << UART_C3_R8_SHIFT)) & UART_C3_R8_MASK)

/*! @name D - UART Data Register */
#define UART_D_R0T0_MASK                         (0x1U)
#define UART_D_R0T0_SHIFT                        (0U)
#define UART_D_R0T0(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R0T0_SHIFT)) & UART_D_R0T0_MASK)
#define UART_D_R1T1_MASK                         (0x2U)
#define UART_D_R1T1_SHIFT                        (1U)
#define UART_D_R1T1(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R1T1_SHIFT)) & UART_D_R1T1_MASK)
#define UART_D_R2T2_MASK                         (0x4U)
#define UART_D_R2T2_SHIFT                        (2U)
#define UART_D_R2T2(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R2T2_SHIFT)) & UART_D_R2T2_MASK)
#define UART_D_R3T3_MASK                         (0x8U)
#define UART_D_R3T3_SHIFT                        (3U)
#define UART_D_R3T3(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R3T3_SHIFT)) & UART_D_R3T3_MASK)
#define UART_D_R4T4_MASK                         (0x10U)
#define UART_D_R4T4_SHIFT                        (4U)
#define UART_D_R4T4(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R4T4_SHIFT)) & UART_D_R4T4_MASK)
#define UART_D_R5T5_MASK                         (0x20U)
#define UART_D_R5T5_SHIFT                        (5U)
#define UART_D_R5T5(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R5T5_SHIFT)) & UART_D_R5T5_MASK)
#define UART_D_R6T6_MASK                         (0x40U)
#define UART_D_R6T6_SHIFT                        (6U)
#define UART_D_R6T6(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R6T6_SHIFT)) & UART_D_R6T6_MASK)
#define UART_D_R7T7_MASK                         (0x80U)
#define UART_D_R7T7_SHIFT                        (7U)
#define UART_D_R7T7(x)                           (((uint8_t)(((uint8_t)(x)) << UART_D_R7T7_SHIFT)) & UART_D_R7T7_MASK)


/*!
 * @}
 */ /* end of group UART_Register_Masks */


/* UART - Peripheral instance base addresses */
/** Peripheral UART0 base address */
#define UART0_BASE                               (0x4006A000u)
/** Peripheral UART0 base pointer */
#define UART0                                    ((UART_Type *)UART0_BASE)
/** Peripheral UART1 base address */
#define UART1_BASE                               (0x4006B000u)
/** Peripheral UART1 base pointer */
#define UART1                                    ((UART_Type *)UART1_BASE)
/** Peripheral UART2 base address */
#define UART2_BASE                               (0x4006C000u)
/** Peripheral UART2 base pointer */
#define UART2                                    ((UART_Type *)UART2_BASE)
/** Array initializer of UART peripheral base addresses */
#define UART_BASE_ADDRS                          { UART0_BASE, UART1_BASE, UART2_BASE }
/** Array initializer of UART peripheral base pointers */
#define UART_BASE_PTRS                           { UART0, UART1, UART2 }
/** Interrupt vectors for the UART peripheral type */
#define UART_RX_TX_IRQS                          { UART0_IRQn, UART1_IRQn, UART2_IRQn }
#define UART_ERR_IRQS                            { UART0_IRQn, UART1_IRQn, UART2_IRQn }

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
    __IO uint16_t CNT;                               /**< WDOG_CNT register., offset: 0x2 */
    struct {                                         /* offset: 0x2 */
      __IO uint8_t CNTH;                               /**< Watchdog Counter Register: High, offset: 0x2 */
      __IO uint8_t CNTL;                               /**< Watchdog Counter Register: Low, offset: 0x3 */
    } CNT8B;
  };
  union {                                          /* offset: 0x4 */
    __IO uint16_t TOVAL;                             /**< WDOG_TOVAL register., offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint8_t TOVALH;                             /**< Watchdog Timeout Value Register: High, offset: 0x4 */
      __IO uint8_t TOVALL;                             /**< Watchdog Timeout Value Register: Low, offset: 0x5 */
    } TOVAL8B;
  };
  union {                                          /* offset: 0x6 */
    __IO uint16_t WIN;                               /**< WDOG_WIN register., offset: 0x6 */
    struct {                                         /* offset: 0x6 */
      __IO uint8_t WINH;                               /**< Watchdog Window Register: High, offset: 0x6 */
      __IO uint8_t WINL;                               /**< Watchdog Window Register: Low, offset: 0x7 */
    } WIN8B;
  };
} WDOG_Type;

/* ----------------------------------------------------------------------------
   -- WDOG Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDOG_Register_Masks WDOG Register Masks
 * @{
 */

/*! @name CS1 - Watchdog Control and Status Register 1 */
#define WDOG_CS1_STOP_MASK                       (0x1U)
#define WDOG_CS1_STOP_SHIFT                      (0U)
#define WDOG_CS1_STOP(x)                         (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_STOP_SHIFT)) & WDOG_CS1_STOP_MASK)
#define WDOG_CS1_WAIT_MASK                       (0x2U)
#define WDOG_CS1_WAIT_SHIFT                      (1U)
#define WDOG_CS1_WAIT(x)                         (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_WAIT_SHIFT)) & WDOG_CS1_WAIT_MASK)
#define WDOG_CS1_DBG_MASK                        (0x4U)
#define WDOG_CS1_DBG_SHIFT                       (2U)
#define WDOG_CS1_DBG(x)                          (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_DBG_SHIFT)) & WDOG_CS1_DBG_MASK)
#define WDOG_CS1_TST_MASK                        (0x18U)
#define WDOG_CS1_TST_SHIFT                       (3U)
#define WDOG_CS1_TST(x)                          (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_TST_SHIFT)) & WDOG_CS1_TST_MASK)
#define WDOG_CS1_UPDATE_MASK                     (0x20U)
#define WDOG_CS1_UPDATE_SHIFT                    (5U)
#define WDOG_CS1_UPDATE(x)                       (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_UPDATE_SHIFT)) & WDOG_CS1_UPDATE_MASK)
#define WDOG_CS1_INT_MASK                        (0x40U)
#define WDOG_CS1_INT_SHIFT                       (6U)
#define WDOG_CS1_INT(x)                          (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_INT_SHIFT)) & WDOG_CS1_INT_MASK)
#define WDOG_CS1_EN_MASK                         (0x80U)
#define WDOG_CS1_EN_SHIFT                        (7U)
#define WDOG_CS1_EN(x)                           (((uint8_t)(((uint8_t)(x)) << WDOG_CS1_EN_SHIFT)) & WDOG_CS1_EN_MASK)

/*! @name CS2 - Watchdog Control and Status Register 2 */
#define WDOG_CS2_CLK_MASK                        (0x3U)
#define WDOG_CS2_CLK_SHIFT                       (0U)
#define WDOG_CS2_CLK(x)                          (((uint8_t)(((uint8_t)(x)) << WDOG_CS2_CLK_SHIFT)) & WDOG_CS2_CLK_MASK)
#define WDOG_CS2_PRES_MASK                       (0x10U)
#define WDOG_CS2_PRES_SHIFT                      (4U)
#define WDOG_CS2_PRES(x)                         (((uint8_t)(((uint8_t)(x)) << WDOG_CS2_PRES_SHIFT)) & WDOG_CS2_PRES_MASK)
#define WDOG_CS2_FLG_MASK                        (0x40U)
#define WDOG_CS2_FLG_SHIFT                       (6U)
#define WDOG_CS2_FLG(x)                          (((uint8_t)(((uint8_t)(x)) << WDOG_CS2_FLG_SHIFT)) & WDOG_CS2_FLG_MASK)
#define WDOG_CS2_WIN_MASK                        (0x80U)
#define WDOG_CS2_WIN_SHIFT                       (7U)
#define WDOG_CS2_WIN(x)                          (((uint8_t)(((uint8_t)(x)) << WDOG_CS2_WIN_SHIFT)) & WDOG_CS2_WIN_MASK)

/*! @name CNT - WDOG_CNT register. */
#define WDOG_CNT_CNTHIGH_MASK                    (0xFFU)
#define WDOG_CNT_CNTHIGH_SHIFT                   (0U)
#define WDOG_CNT_CNTHIGH(x)                      (((uint16_t)(((uint16_t)(x)) << WDOG_CNT_CNTHIGH_SHIFT)) & WDOG_CNT_CNTHIGH_MASK)
#define WDOG_CNT_CNTLOW_MASK                     (0xFF00U)
#define WDOG_CNT_CNTLOW_SHIFT                    (8U)
#define WDOG_CNT_CNTLOW(x)                       (((uint16_t)(((uint16_t)(x)) << WDOG_CNT_CNTLOW_SHIFT)) & WDOG_CNT_CNTLOW_MASK)

/*! @name CNTH - Watchdog Counter Register: High */
#define WDOG_CNTH_CNTHIGH_MASK                   (0xFFU)
#define WDOG_CNTH_CNTHIGH_SHIFT                  (0U)
#define WDOG_CNTH_CNTHIGH(x)                     (((uint8_t)(((uint8_t)(x)) << WDOG_CNTH_CNTHIGH_SHIFT)) & WDOG_CNTH_CNTHIGH_MASK)

/*! @name CNTL - Watchdog Counter Register: Low */
#define WDOG_CNTL_CNTLOW_MASK                    (0xFFU)
#define WDOG_CNTL_CNTLOW_SHIFT                   (0U)
#define WDOG_CNTL_CNTLOW(x)                      (((uint8_t)(((uint8_t)(x)) << WDOG_CNTL_CNTLOW_SHIFT)) & WDOG_CNTL_CNTLOW_MASK)

/*! @name TOVAL - WDOG_TOVAL register. */
#define WDOG_TOVAL_TOVALHIGH_MASK                (0xFFU)
#define WDOG_TOVAL_TOVALHIGH_SHIFT               (0U)
#define WDOG_TOVAL_TOVALHIGH(x)                  (((uint16_t)(((uint16_t)(x)) << WDOG_TOVAL_TOVALHIGH_SHIFT)) & WDOG_TOVAL_TOVALHIGH_MASK)
#define WDOG_TOVAL_TOVALLOW_MASK                 (0xFF00U)
#define WDOG_TOVAL_TOVALLOW_SHIFT                (8U)
#define WDOG_TOVAL_TOVALLOW(x)                   (((uint16_t)(((uint16_t)(x)) << WDOG_TOVAL_TOVALLOW_SHIFT)) & WDOG_TOVAL_TOVALLOW_MASK)

/*! @name TOVALH - Watchdog Timeout Value Register: High */
#define WDOG_TOVALH_TOVALHIGH_MASK               (0xFFU)
#define WDOG_TOVALH_TOVALHIGH_SHIFT              (0U)
#define WDOG_TOVALH_TOVALHIGH(x)                 (((uint8_t)(((uint8_t)(x)) << WDOG_TOVALH_TOVALHIGH_SHIFT)) & WDOG_TOVALH_TOVALHIGH_MASK)

/*! @name TOVALL - Watchdog Timeout Value Register: Low */
#define WDOG_TOVALL_TOVALLOW_MASK                (0xFFU)
#define WDOG_TOVALL_TOVALLOW_SHIFT               (0U)
#define WDOG_TOVALL_TOVALLOW(x)                  (((uint8_t)(((uint8_t)(x)) << WDOG_TOVALL_TOVALLOW_SHIFT)) & WDOG_TOVALL_TOVALLOW_MASK)

/*! @name WIN - WDOG_WIN register. */
#define WDOG_WIN_WINHIGH_MASK                    (0xFFU)
#define WDOG_WIN_WINHIGH_SHIFT                   (0U)
#define WDOG_WIN_WINHIGH(x)                      (((uint16_t)(((uint16_t)(x)) << WDOG_WIN_WINHIGH_SHIFT)) & WDOG_WIN_WINHIGH_MASK)
#define WDOG_WIN_WINLOW_MASK                     (0xFF00U)
#define WDOG_WIN_WINLOW_SHIFT                    (8U)
#define WDOG_WIN_WINLOW(x)                       (((uint16_t)(((uint16_t)(x)) << WDOG_WIN_WINLOW_SHIFT)) & WDOG_WIN_WINLOW_MASK)

/*! @name WINH - Watchdog Window Register: High */
#define WDOG_WINH_WINHIGH_MASK                   (0xFFU)
#define WDOG_WINH_WINHIGH_SHIFT                  (0U)
#define WDOG_WINH_WINHIGH(x)                     (((uint8_t)(((uint8_t)(x)) << WDOG_WINH_WINHIGH_SHIFT)) & WDOG_WINH_WINHIGH_MASK)

/*! @name WINL - Watchdog Window Register: Low */
#define WDOG_WINL_WINLOW_MASK                    (0xFFU)
#define WDOG_WINL_WINLOW_SHIFT                   (0U)
#define WDOG_WINL_WINLOW(x)                      (((uint8_t)(((uint8_t)(x)) << WDOG_WINL_WINLOW_SHIFT)) & WDOG_WINL_WINLOW_MASK)


/*!
 * @}
 */ /* end of group WDOG_Register_Masks */


/* WDOG - Peripheral instance base addresses */
/** Peripheral WDOG base address */
#define WDOG_BASE                                (0x40052000u)
/** Peripheral WDOG base pointer */
#define WDOG                                     ((WDOG_Type *)WDOG_BASE)
/** Array initializer of WDOG peripheral base addresses */
#define WDOG_BASE_ADDRS                          { WDOG_BASE }
/** Array initializer of WDOG peripheral base pointers */
#define WDOG_BASE_PTRS                           { WDOG }
/** Interrupt vectors for the WDOG peripheral type */
#define WDOG_IRQS                                { WDOG_IRQn }
#define WDOG_UPDATE_KEY1                         (0x20C5U)
#define WDOG_UPDATE_KEY_H1                       (0x20U)
#define WDOG_UPDATE_KEY_L1                       (0xC5U)
#define WDOG_UPDATE_KEY2                         (0x28D9U)
#define WDOG_UPDATE_KEY_H2                       (0x28U)
#define WDOG_UPDATE_KEY_L2                       (0xD9U)
#define WDOG_REFRESH_KEY1                        (0x02A6U)
#define WDOG_REFRESH_KEY_H1                      (0x02U)
#define WDOG_REFRESH_KEY_L1                      (0xA6U)
#define WDOG_REFRESH_KEY2                        (0x80B4U)
#define WDOG_REFRESH_KEY_H2                      (0x80U)
#define WDOG_REFRESH_KEY_L2                      (0xB4U)


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
#else
  #error Not supported compiler type
#endif

/*!
 * @}
 */ /* end of group Peripheral_access_layer */


/* ----------------------------------------------------------------------------
   -- Macros for use with bit field definitions (xxx_SHIFT, xxx_MASK).
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Bit_Field_Generic_Macros Macros for use with bit field definitions (xxx_SHIFT, xxx_MASK).
 * @{
 */

#if defined(__ARMCC_VERSION)
  #if (__ARMCC_VERSION >= 6010050)
    #pragma clang system_header
  #endif
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma system_include
#endif

/**
 * @brief Mask and left-shift a bit field value for use in a register bit range.
 * @param field Name of the register bit field.
 * @param value Value of the bit field.
 * @return Masked and shifted value.
 */
#define NXP_VAL2FLD(field, value)    (((value) << (field ## _SHIFT)) & (field ## _MASK))
/**
 * @brief Mask and right-shift a register value to extract a bit field value.
 * @param field Name of the register bit field.
 * @param value Value of the register.
 * @return Masked and shifted bit field value.
 */
#define NXP_FLD2VAL(field, value)    (((value) & (field ## _MASK)) >> (field ## _SHIFT))

/*!
 * @}
 */ /* end of group Bit_Field_Generic_Macros */


/* ----------------------------------------------------------------------------
   -- SDK Compatibility
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SDK_Compatibility_Symbols SDK Compatibility
 * @{
 */

/* No SDK compatibility issues. */

/*!
 * @}
 */ /* end of group SDK_Compatibility_Symbols */


#endif  /* _MKE02Z4_H_ */

