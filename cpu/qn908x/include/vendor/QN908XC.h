/*
** ###################################################################
**     Processors:          QN9080C
**                          QN9083C
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          MCUXpresso Compiler
**
**     Reference manual:    QN908X User manual Rev.1.0 21 Mar 2017
**     Version:             rev. 1.0, 2017-03-27
**     Build:               b170424
**
**     Abstract:
**         CMSIS Peripheral Access Layer for QN908XC
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2017 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2017-03-27)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file QN908XC.h
 * @version 1.0
 * @date 2017-03-27
 * @brief CMSIS Peripheral Access Layer for QN908XC
 *
 * CMSIS Peripheral Access Layer for QN908XC
 */

#ifndef _QN908XC_H_
#define _QN908XC_H_                              /**< Symbol preventing repeated inclusion */

#ifdef __cplusplus
extern "C" {
#endif

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
#define NUMBER_OF_INT_VECTORS 68                 /**< Number of interrupts in the Vector table */

typedef enum IRQn {
  /* Auxiliary constants */
  NotAvail_IRQn                = -128,             /**< Not available device specific interrupt */

  /* Core interrupts */
  NonMaskableInt_IRQn          = -14,              /**< Non Maskable Interrupt */
  HardFault_IRQn               = -13,              /**< Cortex-M4 SV Hard Fault Interrupt */
  MemoryManagement_IRQn        = -12,              /**< Cortex-M4 Memory Management Interrupt */
  BusFault_IRQn                = -11,              /**< Cortex-M4 Bus Fault Interrupt */
  UsageFault_IRQn              = -10,              /**< Cortex-M4 Usage Fault Interrupt */
  SVCall_IRQn                  = -5,               /**< Cortex-M4 SV Call Interrupt */
  DebugMonitor_IRQn            = -4,               /**< Cortex-M4 Debug Monitor Interrupt */
  PendSV_IRQn                  = -2,               /**< Cortex-M4 Pend SV Interrupt */
  SysTick_IRQn                 = -1,               /**< Cortex-M4 System Tick Interrupt */

  /* Device specific interrupts */
  EXT_GPIO_WAKEUP_IRQn         = 0,                /**< Ext GPIO wakeup */
  OSC_IRQn                     = 1,                /**< BLE wakeup */
  ACMP0_IRQn                   = 2,                /**< Analog comparator0 */
  ACMP1_IRQn                   = 3,                /**< Analog comparator1 */
  RTC_SEC_IRQn                 = 5,                /**< RTC second */
  RTC_FR_IRQn                  = 6,                /**< RTC free running */
  CS_WAKEUP_IRQn               = 7,                /**< Capacitive sense wakeup */
  CS_IRQn                      = 8,                /**< Capacitive sense */
  GPIOA_IRQn                   = 9,                /**< GPIO group A */
  GPIOB_IRQn                   = 10,               /**< GPIO group B */
  DMA0_IRQn                    = 11,               /**< DMA controller */
  PIN_INT0_IRQn                = 12,               /**< pin or pattern match engine slice 0 */
  PIN_INT1_IRQn                = 13,               /**< pin or pattern match engine slice 1 */
  PIN_INT2_IRQn                = 14,               /**< pin or pattern match engine slice 2 */
  PIN_INT3_IRQn                = 15,               /**< pin or pattern match engine slice 3 */
  OSC_INT_LOW_IRQn             = 16,               /**< Inverse of OSC */
  USB0_IRQn                    = 17,               /**< USB device */
  FLEXCOMM0_IRQn               = 18,               /**< Flexcomm Interface 0 (USART) */
  FLEXCOMM1_IRQn               = 19,               /**< Flexcomm Interface 1 (USART, I2C) */
  FLEXCOMM2_IRQn               = 20,               /**< Flexcomm Interface 2 (SPI, I2C) */
  FLEXCOMM3_IRQn               = 21,               /**< Flexcomm Interface 3 (SPI) */
  BLE_IRQn                     = 22,               /**< BLE interrupts */
  FSP_IRQn                     = 23,               /**< FSP */
  QDEC0_IRQn                   = 24,               /**< QDEC0 */
  QDEC1_IRQn                   = 25,               /**< QDEC1 */
  CTIMER0_IRQn                 = 26,               /**< Standard counter/timer CTIMER0 */
  CTIMER1_IRQn                 = 27,               /**< Standard counter/timer CTIMER1 */
  CTIMER2_IRQn                 = 28,               /**< Standard counter/timer CTIMER2 */
  CTIMER3_IRQn                 = 29,               /**< Standard counter/timer CTIMER3 */
  WDT_IRQn                     = 30,               /**< Watch dog timer */
  ADC_IRQn                     = 31,               /**< ADC */
  DAC_IRQn                     = 32,               /**< DAC */
  XTAL_READY_IRQn              = 33,               /**< High frequency crystal ready */
  FLASH_IRQn                   = 34,               /**< Flash */
  SPIFI0_IRQn                  = 35,               /**< SPI flash interface */
  SCT0_IRQn                    = 36,               /**< SCTimer/PWM */
  RNG_IRQn                     = 38,               /**< Random number generator */
  CALIB_IRQn                   = 40,               /**< Calibration */
  BLE_TX_IRQn                  = 42,               /**< ble tx flag */
  BLE_RX_IRQn                  = 43,               /**< ble rx flag */
  BLE_FREQ_HOP_IRQn            = 44,               /**< ble frequency hop */
  BOD_IRQn                     = 51                /**< Brown out dectect */
} IRQn_Type;

/*!
 * @}
 */ /* end of group Interrupt_vector_numbers */


/* ----------------------------------------------------------------------------
   -- Cortex M4 Core Configuration
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Cortex_Core_Configuration Cortex M4 Core Configuration
 * @{
 */

#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               3         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */
#define __FPU_PRESENT                  1         /**< Defines if an FPU is present or not */

#include "core_cm4.h"                  /* Core Peripheral Access Layer */
#include "system_QN908XC.h"            /* Device specific configuration file */

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
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#else
  #error Not supported compiler type
#endif

/* ----------------------------------------------------------------------------
   -- ADC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Peripheral_Access_Layer ADC Peripheral Access Layer
 * @{
 */

/** ADC - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL;                              /**< ADC control register, offset: 0x0 */
  __IO uint32_t CH_SEL;                            /**< ADC channel selection register, offset: 0x4 */
  __IO uint32_t CH_CFG;                            /**< ADC channel configuration register, offset: 0x8 */
  __IO uint32_t WCMP_THR;                          /**< Window compare threshold register, offset: 0xC */
  __IO uint32_t INTEN;                             /**< ADC interrupt enable register, offset: 0x10 */
  __IO uint32_t INT;                               /**< ADC interrupt status register, offset: 0x14 */
  __I  uint32_t DATA;                              /**< ADC converted data output, offset: 0x18 */
       uint8_t RESERVED_0[4];
  __IO uint32_t CFG[2];                            /**< ADC configuration register, array offset: 0x20, array step: 0x4 */
  __IO uint32_t BG_BF;                             /**< ADC bandcap and buffer setting register, offset: 0x28 */
  __IO uint32_t ANA_CTRL;                          /**< ADC core and reference setting regsiter, offset: 0x2C */
} ADC_Type;

/* ----------------------------------------------------------------------------
   -- ADC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Register_Masks ADC Register Masks
 * @{
 */

/*! @name CTRL - ADC control register */
#define ADC_CTRL_ENABLE_MASK                     (0x1U)
#define ADC_CTRL_ENABLE_SHIFT                    (0U)
#define ADC_CTRL_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_ENABLE_SHIFT)) & ADC_CTRL_ENABLE_MASK)
#define ADC_CTRL_CONV_MODE_MASK                  (0x2U)
#define ADC_CTRL_CONV_MODE_SHIFT                 (1U)
#define ADC_CTRL_CONV_MODE(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_CONV_MODE_SHIFT)) & ADC_CTRL_CONV_MODE_MASK)
#define ADC_CTRL_SCAN_EN_MASK                    (0x4U)
#define ADC_CTRL_SCAN_EN_SHIFT                   (2U)
#define ADC_CTRL_SCAN_EN(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_SCAN_EN_SHIFT)) & ADC_CTRL_SCAN_EN_MASK)
#define ADC_CTRL_WCMP_EN_MASK                    (0x8U)
#define ADC_CTRL_WCMP_EN_SHIFT                   (3U)
#define ADC_CTRL_WCMP_EN(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_WCMP_EN_SHIFT)) & ADC_CTRL_WCMP_EN_MASK)
#define ADC_CTRL_SW_START_MASK                   (0x80U)
#define ADC_CTRL_SW_START_SHIFT                  (7U)
#define ADC_CTRL_SW_START(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_SW_START_SHIFT)) & ADC_CTRL_SW_START_MASK)
#define ADC_CTRL_CLKSEL_MASK                     (0x1F00U)
#define ADC_CTRL_CLKSEL_SHIFT                    (8U)
#define ADC_CTRL_CLKSEL(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_CLKSEL_SHIFT)) & ADC_CTRL_CLKSEL_MASK)
#define ADC_CTRL_SIG_INV_EN_MASK                 (0x2000U)
#define ADC_CTRL_SIG_INV_EN_SHIFT                (13U)
#define ADC_CTRL_SIG_INV_EN(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_SIG_INV_EN_SHIFT)) & ADC_CTRL_SIG_INV_EN_MASK)
#define ADC_CTRL_VREF_SEL_MASK                   (0xC000U)
#define ADC_CTRL_VREF_SEL_SHIFT                  (14U)
#define ADC_CTRL_VREF_SEL(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_VREF_SEL_SHIFT)) & ADC_CTRL_VREF_SEL_MASK)
#define ADC_CTRL_CH_IDX_EN_MASK                  (0x40000U)
#define ADC_CTRL_CH_IDX_EN_SHIFT                 (18U)
#define ADC_CTRL_CH_IDX_EN(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_CH_IDX_EN_SHIFT)) & ADC_CTRL_CH_IDX_EN_MASK)
#define ADC_CTRL_DATA_FORMAT_MASK                (0x80000U)
#define ADC_CTRL_DATA_FORMAT_SHIFT               (19U)
#define ADC_CTRL_DATA_FORMAT(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_DATA_FORMAT_SHIFT)) & ADC_CTRL_DATA_FORMAT_MASK)
#define ADC_CTRL_VREFO_EN_MASK                   (0x100000U)
#define ADC_CTRL_VREFO_EN_SHIFT                  (20U)
#define ADC_CTRL_VREFO_EN(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_VREFO_EN_SHIFT)) & ADC_CTRL_VREFO_EN_MASK)
#define ADC_CTRL_SRST_DIS_MASK                   (0x200000U)
#define ADC_CTRL_SRST_DIS_SHIFT                  (21U)
#define ADC_CTRL_SRST_DIS(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_SRST_DIS_SHIFT)) & ADC_CTRL_SRST_DIS_MASK)
#define ADC_CTRL_TRIGGER_MASK                    (0x1F800000U)
#define ADC_CTRL_TRIGGER_SHIFT                   (23U)
#define ADC_CTRL_TRIGGER(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_TRIGGER_SHIFT)) & ADC_CTRL_TRIGGER_MASK)

/*! @name CH_SEL - ADC channel selection register */
#define ADC_CH_SEL_CH_SEL_MASK                   (0xFFFFFFFFU)
#define ADC_CH_SEL_CH_SEL_SHIFT                  (0U)
#define ADC_CH_SEL_CH_SEL(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CH_SEL_CH_SEL_SHIFT)) & ADC_CH_SEL_CH_SEL_MASK)

/*! @name CH_CFG - ADC channel configuration register */
#define ADC_CH_CFG_CH_CFG_MASK                   (0xFFFFFFFFU)
#define ADC_CH_CFG_CH_CFG_SHIFT                  (0U)
#define ADC_CH_CFG_CH_CFG(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CH_CFG_CH_CFG_SHIFT)) & ADC_CH_CFG_CH_CFG_MASK)

/*! @name WCMP_THR - Window compare threshold register */
#define ADC_WCMP_THR_WCMP_THR_LOW_MASK           (0xFFFFU)
#define ADC_WCMP_THR_WCMP_THR_LOW_SHIFT          (0U)
#define ADC_WCMP_THR_WCMP_THR_LOW(x)             (((uint32_t)(((uint32_t)(x)) << ADC_WCMP_THR_WCMP_THR_LOW_SHIFT)) & ADC_WCMP_THR_WCMP_THR_LOW_MASK)
#define ADC_WCMP_THR_WCMP_THR_HIGH_MASK          (0xFFFF0000U)
#define ADC_WCMP_THR_WCMP_THR_HIGH_SHIFT         (16U)
#define ADC_WCMP_THR_WCMP_THR_HIGH(x)            (((uint32_t)(((uint32_t)(x)) << ADC_WCMP_THR_WCMP_THR_HIGH_SHIFT)) & ADC_WCMP_THR_WCMP_THR_HIGH_MASK)

/*! @name INTEN - ADC interrupt enable register */
#define ADC_INTEN_DAT_RDY_INTEN_MASK             (0x1U)
#define ADC_INTEN_DAT_RDY_INTEN_SHIFT            (0U)
#define ADC_INTEN_DAT_RDY_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << ADC_INTEN_DAT_RDY_INTEN_SHIFT)) & ADC_INTEN_DAT_RDY_INTEN_MASK)
#define ADC_INTEN_WCMP_INTEN_MASK                (0x2U)
#define ADC_INTEN_WCMP_INTEN_SHIFT               (1U)
#define ADC_INTEN_WCMP_INTEN(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_INTEN_WCMP_INTEN_SHIFT)) & ADC_INTEN_WCMP_INTEN_MASK)
#define ADC_INTEN_FIFO_OF_INTEN_MASK             (0x4U)
#define ADC_INTEN_FIFO_OF_INTEN_SHIFT            (2U)
#define ADC_INTEN_FIFO_OF_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << ADC_INTEN_FIFO_OF_INTEN_SHIFT)) & ADC_INTEN_FIFO_OF_INTEN_MASK)
#define ADC_INTEN_ADC_INTEN_MASK                 (0x80000000U)
#define ADC_INTEN_ADC_INTEN_SHIFT                (31U)
#define ADC_INTEN_ADC_INTEN(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_INTEN_ADC_INTEN_SHIFT)) & ADC_INTEN_ADC_INTEN_MASK)

/*! @name INT - ADC interrupt status register */
#define ADC_INT_DAT_RDY_INT_MASK                 (0x1U)
#define ADC_INT_DAT_RDY_INT_SHIFT                (0U)
#define ADC_INT_DAT_RDY_INT(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_INT_DAT_RDY_INT_SHIFT)) & ADC_INT_DAT_RDY_INT_MASK)
#define ADC_INT_WCMP_INT_MASK                    (0x2U)
#define ADC_INT_WCMP_INT_SHIFT                   (1U)
#define ADC_INT_WCMP_INT(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_INT_WCMP_INT_SHIFT)) & ADC_INT_WCMP_INT_MASK)
#define ADC_INT_FIFO_OF_INT_MASK                 (0x4U)
#define ADC_INT_FIFO_OF_INT_SHIFT                (2U)
#define ADC_INT_FIFO_OF_INT(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_INT_FIFO_OF_INT_SHIFT)) & ADC_INT_FIFO_OF_INT_MASK)
#define ADC_INT_ADC_INT_MASK                     (0x80000000U)
#define ADC_INT_ADC_INT_SHIFT                    (31U)
#define ADC_INT_ADC_INT(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_INT_ADC_INT_SHIFT)) & ADC_INT_ADC_INT_MASK)

/*! @name DATA - ADC converted data output */
#define ADC_DATA_DATA_MASK                       (0xFFFFFFFFU)
#define ADC_DATA_DATA_SHIFT                      (0U)
#define ADC_DATA_DATA(x)                         (((uint32_t)(((uint32_t)(x)) << ADC_DATA_DATA_SHIFT)) & ADC_DATA_DATA_MASK)

/*! @name CFG - ADC configuration register */
#define ADC_CFG_PGA_GAIN_MASK                    (0x7U)
#define ADC_CFG_PGA_GAIN_SHIFT                   (0U)
#define ADC_CFG_PGA_GAIN(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_CFG_PGA_GAIN_SHIFT)) & ADC_CFG_PGA_GAIN_MASK)
#define ADC_CFG_PGA_BP_MASK                      (0x8U)
#define ADC_CFG_PGA_BP_SHIFT                     (3U)
#define ADC_CFG_PGA_BP(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_CFG_PGA_BP_SHIFT)) & ADC_CFG_PGA_BP_MASK)
#define ADC_CFG_PGA_VINN_MASK                    (0x30U)
#define ADC_CFG_PGA_VINN_SHIFT                   (4U)
#define ADC_CFG_PGA_VINN(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_CFG_PGA_VINN_SHIFT)) & ADC_CFG_PGA_VINN_MASK)
#define ADC_CFG_ADC_GAIN_MASK                    (0xC0U)
#define ADC_CFG_ADC_GAIN_SHIFT                   (6U)
#define ADC_CFG_ADC_GAIN(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_CFG_ADC_GAIN_SHIFT)) & ADC_CFG_ADC_GAIN_MASK)
#define ADC_CFG_VREF_GAIN_MASK                   (0x100U)
#define ADC_CFG_VREF_GAIN_SHIFT                  (8U)
#define ADC_CFG_VREF_GAIN(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CFG_VREF_GAIN_SHIFT)) & ADC_CFG_VREF_GAIN_MASK)
#define ADC_CFG_ADC_VCM_MASK                     (0xE00U)
#define ADC_CFG_ADC_VCM_SHIFT                    (9U)
#define ADC_CFG_ADC_VCM(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_CFG_ADC_VCM_SHIFT)) & ADC_CFG_ADC_VCM_MASK)
#define ADC_CFG_PGA_VCM_EN_MASK                  (0x1000U)
#define ADC_CFG_PGA_VCM_EN_SHIFT                 (12U)
#define ADC_CFG_PGA_VCM_EN(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_CFG_PGA_VCM_EN_SHIFT)) & ADC_CFG_PGA_VCM_EN_MASK)
#define ADC_CFG_PGA_VCM_DIR_MASK                 (0x2000U)
#define ADC_CFG_PGA_VCM_DIR_SHIFT                (13U)
#define ADC_CFG_PGA_VCM_DIR(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_CFG_PGA_VCM_DIR_SHIFT)) & ADC_CFG_PGA_VCM_DIR_MASK)
#define ADC_CFG_PGA_VCM_MASK                     (0xFC000U)
#define ADC_CFG_PGA_VCM_SHIFT                    (14U)
#define ADC_CFG_PGA_VCM(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_CFG_PGA_VCM_SHIFT)) & ADC_CFG_PGA_VCM_MASK)
#define ADC_CFG_DOWN_SAMPLE_RATE_MASK            (0x700000U)
#define ADC_CFG_DOWN_SAMPLE_RATE_SHIFT           (20U)
#define ADC_CFG_DOWN_SAMPLE_RATE(x)              (((uint32_t)(((uint32_t)(x)) << ADC_CFG_DOWN_SAMPLE_RATE_SHIFT)) & ADC_CFG_DOWN_SAMPLE_RATE_MASK)
#define ADC_CFG_DS_DATA_STABLE_MASK              (0x1F800000U)
#define ADC_CFG_DS_DATA_STABLE_SHIFT             (23U)
#define ADC_CFG_DS_DATA_STABLE(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CFG_DS_DATA_STABLE_SHIFT)) & ADC_CFG_DS_DATA_STABLE_MASK)
#define ADC_CFG_SCAN_INTV_MASK                   (0xE0000000U)
#define ADC_CFG_SCAN_INTV_SHIFT                  (29U)
#define ADC_CFG_SCAN_INTV(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_CFG_SCAN_INTV_SHIFT)) & ADC_CFG_SCAN_INTV_MASK)

/* The count of ADC_CFG */
#define ADC_CFG_COUNT                            (2U)

/*! @name BG_BF - ADC bandcap and buffer setting register */
#define ADC_BG_BF_PGA_BM_MASK                    (0x7U)
#define ADC_BG_BF_PGA_BM_SHIFT                   (0U)
#define ADC_BG_BF_PGA_BM(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_BG_BF_PGA_BM_SHIFT)) & ADC_BG_BF_PGA_BM_MASK)
#define ADC_BG_BF_BG_SEL_MASK                    (0xF0U)
#define ADC_BG_BF_BG_SEL_SHIFT                   (4U)
#define ADC_BG_BF_BG_SEL(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_BG_BF_BG_SEL_SHIFT)) & ADC_BG_BF_BG_SEL_MASK)
#define ADC_BG_BF_TEMP_EN_MASK                   (0x1000U)
#define ADC_BG_BF_TEMP_EN_SHIFT                  (12U)
#define ADC_BG_BF_TEMP_EN(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_BG_BF_TEMP_EN_SHIFT)) & ADC_BG_BF_TEMP_EN_MASK)
#define ADC_BG_BF_PGA_CHOP_EN_MASK               (0x2000U)
#define ADC_BG_BF_PGA_CHOP_EN_SHIFT              (13U)
#define ADC_BG_BF_PGA_CHOP_EN(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_BG_BF_PGA_CHOP_EN_SHIFT)) & ADC_BG_BF_PGA_CHOP_EN_MASK)
#define ADC_BG_BF_PGA_BM_DIV2_MASK               (0x4000U)
#define ADC_BG_BF_PGA_BM_DIV2_SHIFT              (14U)
#define ADC_BG_BF_PGA_BM_DIV2(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_BG_BF_PGA_BM_DIV2_SHIFT)) & ADC_BG_BF_PGA_BM_DIV2_MASK)

/*! @name ANA_CTRL - ADC core and reference setting regsiter */
#define ADC_ANA_CTRL_ADC_BM_MASK                 (0x7U)
#define ADC_ANA_CTRL_ADC_BM_SHIFT                (0U)
#define ADC_ANA_CTRL_ADC_BM(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_ADC_BM_SHIFT)) & ADC_ANA_CTRL_ADC_BM_MASK)
#define ADC_ANA_CTRL_ADC_ORDER_MASK              (0x10U)
#define ADC_ANA_CTRL_ADC_ORDER_SHIFT             (4U)
#define ADC_ANA_CTRL_ADC_ORDER(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_ADC_ORDER_SHIFT)) & ADC_ANA_CTRL_ADC_ORDER_MASK)
#define ADC_ANA_CTRL_DITHER_EN_MASK              (0x20U)
#define ADC_ANA_CTRL_DITHER_EN_SHIFT             (5U)
#define ADC_ANA_CTRL_DITHER_EN(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_DITHER_EN_SHIFT)) & ADC_ANA_CTRL_DITHER_EN_MASK)
#define ADC_ANA_CTRL_CHOP_EN_MASK                (0x40U)
#define ADC_ANA_CTRL_CHOP_EN_SHIFT               (6U)
#define ADC_ANA_CTRL_CHOP_EN(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_CHOP_EN_SHIFT)) & ADC_ANA_CTRL_CHOP_EN_MASK)
#define ADC_ANA_CTRL_INV_CLK_MASK                (0x80U)
#define ADC_ANA_CTRL_INV_CLK_SHIFT               (7U)
#define ADC_ANA_CTRL_INV_CLK(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_INV_CLK_SHIFT)) & ADC_ANA_CTRL_INV_CLK_MASK)
#define ADC_ANA_CTRL_VREF_BM_MASK                (0x700U)
#define ADC_ANA_CTRL_VREF_BM_SHIFT               (8U)
#define ADC_ANA_CTRL_VREF_BM(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_VREF_BM_SHIFT)) & ADC_ANA_CTRL_VREF_BM_MASK)
#define ADC_ANA_CTRL_VREF_BM_X3_MASK             (0x800U)
#define ADC_ANA_CTRL_VREF_BM_X3_SHIFT            (11U)
#define ADC_ANA_CTRL_VREF_BM_X3(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_VREF_BM_X3_SHIFT)) & ADC_ANA_CTRL_VREF_BM_X3_MASK)
#define ADC_ANA_CTRL_VINN_IN_BM_MASK             (0x7000U)
#define ADC_ANA_CTRL_VINN_IN_BM_SHIFT            (12U)
#define ADC_ANA_CTRL_VINN_IN_BM(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_VINN_IN_BM_SHIFT)) & ADC_ANA_CTRL_VINN_IN_BM_MASK)
#define ADC_ANA_CTRL_VINN_OUT_BM_MASK            (0x70000U)
#define ADC_ANA_CTRL_VINN_OUT_BM_SHIFT           (16U)
#define ADC_ANA_CTRL_VINN_OUT_BM(x)              (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_VINN_OUT_BM_SHIFT)) & ADC_ANA_CTRL_VINN_OUT_BM_MASK)
#define ADC_ANA_CTRL_VINN_OUT_BM_X3_MASK         (0x80000U)
#define ADC_ANA_CTRL_VINN_OUT_BM_X3_SHIFT        (19U)
#define ADC_ANA_CTRL_VINN_OUT_BM_X3(x)           (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_VINN_OUT_BM_X3_SHIFT)) & ADC_ANA_CTRL_VINN_OUT_BM_X3_MASK)
#define ADC_ANA_CTRL_ADC_BM_DIV2_MASK            (0x100000U)
#define ADC_ANA_CTRL_ADC_BM_DIV2_SHIFT           (20U)
#define ADC_ANA_CTRL_ADC_BM_DIV2(x)              (((uint32_t)(((uint32_t)(x)) << ADC_ANA_CTRL_ADC_BM_DIV2_SHIFT)) & ADC_ANA_CTRL_ADC_BM_DIV2_MASK)


/*!
 * @}
 */ /* end of group ADC_Register_Masks */


/* ADC - Peripheral instance base addresses */
/** Peripheral ADC base address */
#define ADC_BASE                                 (0x40007000u)
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
   -- AGC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup AGC_Peripheral_Access_Layer AGC Peripheral Access Layer
 * @{
 */

/** AGC - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL0;                             /**< AGC control register 0, offset: 0x0 */
  __IO uint32_t CTRL1;                             /**< AGC control register 1, offset: 0x4 */
  __IO uint32_t CTRL2;                             /**< AGC control register 2, offset: 0x8 */
  __IO uint32_t CTRL3;                             /**< AGC control register 3, offset: 0xC */
  __IO uint32_t CTRL4;                             /**< AGC control register 4, offset: 0x10 */
  __IO uint32_t CTRL5;                             /**< AGC control register 5, offset: 0x14 */
  __I  uint32_t STAT;                              /**< AGC status register, offset: 0x18 */
} AGC_Type;

/* ----------------------------------------------------------------------------
   -- AGC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup AGC_Register_Masks AGC Register Masks
 * @{
 */

/*! @name CTRL0 - AGC control register 0 */
#define AGC_CTRL0_PPF_INTRPT_MOD_MASK            (0x3U)
#define AGC_CTRL0_PPF_INTRPT_MOD_SHIFT           (0U)
#define AGC_CTRL0_PPF_INTRPT_MOD(x)              (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_PPF_INTRPT_MOD_SHIFT)) & AGC_CTRL0_PPF_INTRPT_MOD_MASK)
#define AGC_CTRL0_FREZ_MOD_MASK                  (0xCU)
#define AGC_CTRL0_FREZ_MOD_SHIFT                 (2U)
#define AGC_CTRL0_FREZ_MOD(x)                    (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_FREZ_MOD_SHIFT)) & AGC_CTRL0_FREZ_MOD_MASK)
#define AGC_CTRL0_RRF_GAIN_SEL_MASK              (0x70U)
#define AGC_CTRL0_RRF_GAIN_SEL_SHIFT             (4U)
#define AGC_CTRL0_RRF_GAIN_SEL(x)                (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_RRF_GAIN_SEL_SHIFT)) & AGC_CTRL0_RRF_GAIN_SEL_MASK)
#define AGC_CTRL0_RRF_WEN_MASK                   (0x80U)
#define AGC_CTRL0_RRF_WEN_SHIFT                  (7U)
#define AGC_CTRL0_RRF_WEN(x)                     (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_RRF_WEN_SHIFT)) & AGC_CTRL0_RRF_WEN_MASK)
#define AGC_CTRL0_PPF_GAIN_MASK                  (0xF00U)
#define AGC_CTRL0_PPF_GAIN_SHIFT                 (8U)
#define AGC_CTRL0_PPF_GAIN(x)                    (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_PPF_GAIN_SHIFT)) & AGC_CTRL0_PPF_GAIN_MASK)
#define AGC_CTRL0_PPF_WEN_MASK                   (0x1000U)
#define AGC_CTRL0_PPF_WEN_SHIFT                  (12U)
#define AGC_CTRL0_PPF_WEN(x)                     (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_PPF_WEN_SHIFT)) & AGC_CTRL0_PPF_WEN_MASK)
#define AGC_CTRL0_PKWT_TH_DIG_1_MASK             (0x3E000U)
#define AGC_CTRL0_PKWT_TH_DIG_1_SHIFT            (13U)
#define AGC_CTRL0_PKWT_TH_DIG_1(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_PKWT_TH_DIG_1_SHIFT)) & AGC_CTRL0_PKWT_TH_DIG_1_MASK)
#define AGC_CTRL0_PD_CLR_EN_MASK                 (0x40000U)
#define AGC_CTRL0_PD_CLR_EN_SHIFT                (18U)
#define AGC_CTRL0_PD_CLR_EN(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_PD_CLR_EN_SHIFT)) & AGC_CTRL0_PD_CLR_EN_MASK)
#define AGC_CTRL0_PD_RST_LEN_MASK                (0x380000U)
#define AGC_CTRL0_PD_RST_LEN_SHIFT               (19U)
#define AGC_CTRL0_PD_RST_LEN(x)                  (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_PD_RST_LEN_SHIFT)) & AGC_CTRL0_PD_RST_LEN_MASK)
#define AGC_CTRL0_RFAGC_FSYNC_DET_DIS_MASK       (0x400000U)
#define AGC_CTRL0_RFAGC_FSYNC_DET_DIS_SHIFT      (22U)
#define AGC_CTRL0_RFAGC_FSYNC_DET_DIS(x)         (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_RFAGC_FSYNC_DET_DIS_SHIFT)) & AGC_CTRL0_RFAGC_FSYNC_DET_DIS_MASK)
#define AGC_CTRL0_RFAGC_DIRECTION_FREEZE_MASK    (0x800000U)
#define AGC_CTRL0_RFAGC_DIRECTION_FREEZE_SHIFT   (23U)
#define AGC_CTRL0_RFAGC_DIRECTION_FREEZE(x)      (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_RFAGC_DIRECTION_FREEZE_SHIFT)) & AGC_CTRL0_RFAGC_DIRECTION_FREEZE_MASK)
#define AGC_CTRL0_DOWN_24_EN_MASK                (0x1000000U)
#define AGC_CTRL0_DOWN_24_EN_SHIFT               (24U)
#define AGC_CTRL0_DOWN_24_EN(x)                  (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_DOWN_24_EN_SHIFT)) & AGC_CTRL0_DOWN_24_EN_MASK)
#define AGC_CTRL0_SWITCH_PD_RST_LEN_MASK         (0x6000000U)
#define AGC_CTRL0_SWITCH_PD_RST_LEN_SHIFT        (25U)
#define AGC_CTRL0_SWITCH_PD_RST_LEN(x)           (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_SWITCH_PD_RST_LEN_SHIFT)) & AGC_CTRL0_SWITCH_PD_RST_LEN_MASK)
#define AGC_CTRL0_GLNA_MAX_REDU_MASK             (0x8000000U)
#define AGC_CTRL0_GLNA_MAX_REDU_SHIFT            (27U)
#define AGC_CTRL0_GLNA_MAX_REDU(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL0_GLNA_MAX_REDU_SHIFT)) & AGC_CTRL0_GLNA_MAX_REDU_MASK)

/*! @name CTRL1 - AGC control register 1 */
#define AGC_CTRL1_PD3_TH_REG_MASK                (0x7U)
#define AGC_CTRL1_PD3_TH_REG_SHIFT               (0U)
#define AGC_CTRL1_PD3_TH_REG(x)                  (((uint32_t)(((uint32_t)(x)) << AGC_CTRL1_PD3_TH_REG_SHIFT)) & AGC_CTRL1_PD3_TH_REG_MASK)
#define AGC_CTRL1_PD3_TH_HYST_REG_MASK           (0x78U)
#define AGC_CTRL1_PD3_TH_HYST_REG_SHIFT          (3U)
#define AGC_CTRL1_PD3_TH_HYST_REG(x)             (((uint32_t)(((uint32_t)(x)) << AGC_CTRL1_PD3_TH_HYST_REG_SHIFT)) & AGC_CTRL1_PD3_TH_HYST_REG_MASK)
#define AGC_CTRL1_PKWT_TH_ANA_1_MASK             (0x1F80U)
#define AGC_CTRL1_PKWT_TH_ANA_1_SHIFT            (7U)
#define AGC_CTRL1_PKWT_TH_ANA_1(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL1_PKWT_TH_ANA_1_SHIFT)) & AGC_CTRL1_PKWT_TH_ANA_1_MASK)
#define AGC_CTRL1_PKWT_TH_ANA_0_MASK             (0x3E000U)
#define AGC_CTRL1_PKWT_TH_ANA_0_SHIFT            (13U)
#define AGC_CTRL1_PKWT_TH_ANA_0(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL1_PKWT_TH_ANA_0_SHIFT)) & AGC_CTRL1_PKWT_TH_ANA_0_MASK)
#define AGC_CTRL1_PKWT_TH_DIG_0_MASK             (0x7C0000U)
#define AGC_CTRL1_PKWT_TH_DIG_0_SHIFT            (18U)
#define AGC_CTRL1_PKWT_TH_DIG_0(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL1_PKWT_TH_DIG_0_SHIFT)) & AGC_CTRL1_PKWT_TH_DIG_0_MASK)
#define AGC_CTRL1_SETL_TH_PPF_2_MASK             (0xF800000U)
#define AGC_CTRL1_SETL_TH_PPF_2_SHIFT            (23U)
#define AGC_CTRL1_SETL_TH_PPF_2(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL1_SETL_TH_PPF_2_SHIFT)) & AGC_CTRL1_SETL_TH_PPF_2_MASK)

/*! @name CTRL2 - AGC control register 2 */
#define AGC_CTRL2_PPF_PDVTH_LOW_MASK             (0x100U)
#define AGC_CTRL2_PPF_PDVTH_LOW_SHIFT            (8U)
#define AGC_CTRL2_PPF_PDVTH_LOW(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL2_PPF_PDVTH_LOW_SHIFT)) & AGC_CTRL2_PPF_PDVTH_LOW_MASK)
#define AGC_CTRL2_RRF_MG_PK_MASK                 (0xE00U)
#define AGC_CTRL2_RRF_MG_PK_SHIFT                (9U)
#define AGC_CTRL2_RRF_MG_PK(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL2_RRF_MG_PK_SHIFT)) & AGC_CTRL2_RRF_MG_PK_MASK)
#define AGC_CTRL2_RRF_HG_PK_MASK                 (0x7000U)
#define AGC_CTRL2_RRF_HG_PK_SHIFT                (12U)
#define AGC_CTRL2_RRF_HG_PK(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL2_RRF_HG_PK_SHIFT)) & AGC_CTRL2_RRF_HG_PK_MASK)

/*! @name CTRL3 - AGC control register 3 */
#define AGC_CTRL3_GF2_PAR00_MASK                 (0xFU)
#define AGC_CTRL3_GF2_PAR00_SHIFT                (0U)
#define AGC_CTRL3_GF2_PAR00(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL3_GF2_PAR00_SHIFT)) & AGC_CTRL3_GF2_PAR00_MASK)
#define AGC_CTRL3_GF2_PAR01_MASK                 (0xF0U)
#define AGC_CTRL3_GF2_PAR01_SHIFT                (4U)
#define AGC_CTRL3_GF2_PAR01(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL3_GF2_PAR01_SHIFT)) & AGC_CTRL3_GF2_PAR01_MASK)
#define AGC_CTRL3_GF2_PAR10_MASK                 (0xF00U)
#define AGC_CTRL3_GF2_PAR10_SHIFT                (8U)
#define AGC_CTRL3_GF2_PAR10(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL3_GF2_PAR10_SHIFT)) & AGC_CTRL3_GF2_PAR10_MASK)
#define AGC_CTRL3_SETL_TH_OVSHT_DIG_MASK         (0x7000U)
#define AGC_CTRL3_SETL_TH_OVSHT_DIG_SHIFT        (12U)
#define AGC_CTRL3_SETL_TH_OVSHT_DIG(x)           (((uint32_t)(((uint32_t)(x)) << AGC_CTRL3_SETL_TH_OVSHT_DIG_SHIFT)) & AGC_CTRL3_SETL_TH_OVSHT_DIG_MASK)
#define AGC_CTRL3_SETL_TH_OVSHT_INTRPT_MASK      (0x38000U)
#define AGC_CTRL3_SETL_TH_OVSHT_INTRPT_SHIFT     (15U)
#define AGC_CTRL3_SETL_TH_OVSHT_INTRPT(x)        (((uint32_t)(((uint32_t)(x)) << AGC_CTRL3_SETL_TH_OVSHT_INTRPT_SHIFT)) & AGC_CTRL3_SETL_TH_OVSHT_INTRPT_MASK)
#define AGC_CTRL3_SETL_TH_OVSHT_MASK             (0x1C0000U)
#define AGC_CTRL3_SETL_TH_OVSHT_SHIFT            (18U)
#define AGC_CTRL3_SETL_TH_OVSHT(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL3_SETL_TH_OVSHT_SHIFT)) & AGC_CTRL3_SETL_TH_OVSHT_MASK)

/*! @name CTRL4 - AGC control register 4 */
#define AGC_CTRL4_SETL_TH_PD1_MASK               (0xFU)
#define AGC_CTRL4_SETL_TH_PD1_SHIFT              (0U)
#define AGC_CTRL4_SETL_TH_PD1(x)                 (((uint32_t)(((uint32_t)(x)) << AGC_CTRL4_SETL_TH_PD1_SHIFT)) & AGC_CTRL4_SETL_TH_PD1_MASK)
#define AGC_CTRL4_SETL_TH_PD2_MASK               (0xF0U)
#define AGC_CTRL4_SETL_TH_PD2_SHIFT              (4U)
#define AGC_CTRL4_SETL_TH_PD2(x)                 (((uint32_t)(((uint32_t)(x)) << AGC_CTRL4_SETL_TH_PD2_SHIFT)) & AGC_CTRL4_SETL_TH_PD2_MASK)
#define AGC_CTRL4_SETL_TH_PD3_1_MASK             (0x3F00U)
#define AGC_CTRL4_SETL_TH_PD3_1_SHIFT            (8U)
#define AGC_CTRL4_SETL_TH_PD3_1(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL4_SETL_TH_PD3_1_SHIFT)) & AGC_CTRL4_SETL_TH_PD3_1_MASK)
#define AGC_CTRL4_SETL_TH_PD3_2_MASK             (0xFC000U)
#define AGC_CTRL4_SETL_TH_PD3_2_SHIFT            (14U)
#define AGC_CTRL4_SETL_TH_PD3_2(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL4_SETL_TH_PD3_2_SHIFT)) & AGC_CTRL4_SETL_TH_PD3_2_MASK)
#define AGC_CTRL4_GF2_STAT24_TH_MASK             (0xF00000U)
#define AGC_CTRL4_GF2_STAT24_TH_SHIFT            (20U)
#define AGC_CTRL4_GF2_STAT24_TH(x)               (((uint32_t)(((uint32_t)(x)) << AGC_CTRL4_GF2_STAT24_TH_SHIFT)) & AGC_CTRL4_GF2_STAT24_TH_MASK)

/*! @name CTRL5 - AGC control register 5 */
#define AGC_CTRL5_TEST_CTRL_MASK                 (0xFU)
#define AGC_CTRL5_TEST_CTRL_SHIFT                (0U)
#define AGC_CTRL5_TEST_CTRL(x)                   (((uint32_t)(((uint32_t)(x)) << AGC_CTRL5_TEST_CTRL_SHIFT)) & AGC_CTRL5_TEST_CTRL_MASK)

/*! @name STAT - AGC status register */
#define AGC_STAT_GLNA_CODE_OUT_MASK              (0x7U)
#define AGC_STAT_GLNA_CODE_OUT_SHIFT             (0U)
#define AGC_STAT_GLNA_CODE_OUT(x)                (((uint32_t)(((uint32_t)(x)) << AGC_STAT_GLNA_CODE_OUT_SHIFT)) & AGC_STAT_GLNA_CODE_OUT_MASK)
#define AGC_STAT_GF2_CODE_OUT_MASK               (0x78U)
#define AGC_STAT_GF2_CODE_OUT_SHIFT              (3U)
#define AGC_STAT_GF2_CODE_OUT(x)                 (((uint32_t)(((uint32_t)(x)) << AGC_STAT_GF2_CODE_OUT_SHIFT)) & AGC_STAT_GF2_CODE_OUT_MASK)
#define AGC_STAT_RFAGC_TRIGGER_O_MASK            (0x80U)
#define AGC_STAT_RFAGC_TRIGGER_O_SHIFT           (7U)
#define AGC_STAT_RFAGC_TRIGGER_O(x)              (((uint32_t)(((uint32_t)(x)) << AGC_STAT_RFAGC_TRIGGER_O_SHIFT)) & AGC_STAT_RFAGC_TRIGGER_O_MASK)
#define AGC_STAT_RF_GAIN_MASK                    (0x7F00U)
#define AGC_STAT_RF_GAIN_SHIFT                   (8U)
#define AGC_STAT_RF_GAIN(x)                      (((uint32_t)(((uint32_t)(x)) << AGC_STAT_RF_GAIN_SHIFT)) & AGC_STAT_RF_GAIN_MASK)
#define AGC_STAT_NUM_GAIN_ADJ_MASK               (0xF8000U)
#define AGC_STAT_NUM_GAIN_ADJ_SHIFT              (15U)
#define AGC_STAT_NUM_GAIN_ADJ(x)                 (((uint32_t)(((uint32_t)(x)) << AGC_STAT_NUM_GAIN_ADJ_SHIFT)) & AGC_STAT_NUM_GAIN_ADJ_MASK)
#define AGC_STAT_CUR_STAT_MASK                   (0x700000U)
#define AGC_STAT_CUR_STAT_SHIFT                  (20U)
#define AGC_STAT_CUR_STAT(x)                     (((uint32_t)(((uint32_t)(x)) << AGC_STAT_CUR_STAT_SHIFT)) & AGC_STAT_CUR_STAT_MASK)


/*!
 * @}
 */ /* end of group AGC_Register_Masks */


/* AGC - Peripheral instance base addresses */
/** Peripheral AGC base address */
#define AGC_BASE                                 (0x4000C000u)
/** Peripheral AGC base pointer */
#define AGC                                      ((AGC_Type *)AGC_BASE)
/** Array initializer of AGC peripheral base addresses */
#define AGC_BASE_ADDRS                           { AGC_BASE }
/** Array initializer of AGC peripheral base pointers */
#define AGC_BASE_PTRS                            { AGC }

/*!
 * @}
 */ /* end of group AGC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- BLEDP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup BLEDP_Peripheral_Access_Layer BLEDP Peripheral Access Layer
 * @{
 */

/** BLEDP - Register Layout Typedef */
typedef struct {
  __IO uint32_t DP_TOP_SYSTEM_CTRL;                /**< datapath system control register, offset: 0x0 */
  __IO uint32_t PROP_MODE_CTRL;                    /**< properity mode control register, offset: 0x4 */
  __IO uint32_t ACCESS_ADDRESS;                    /**< access address register, offset: 0x8 */
  __IO uint32_t ANT_PDU_DATA0;                     /**< pdu data 0 to 1 byte, and preamble register, offset: 0xC */
  __IO uint32_t ANT_PDU_DATA1;                     /**< pdu data 2 to 5 byte, offset: 0x10 */
  __IO uint32_t ANT_PDU_DATA2;                     /**< pdu data 6 to 9 byte, offset: 0x14 */
  __IO uint32_t ANT_PDU_DATA3;                     /**< pdu data 10 to 13 byte, offset: 0x18 */
  __IO uint32_t ANT_PDU_DATA4;                     /**< pdu data 14 to 17 byte, offset: 0x1C */
  __IO uint32_t ANT_PDU_DATA5;                     /**< pdu data 18 to 21 byte, offset: 0x20 */
  __IO uint32_t ANT_PDU_DATA6;                     /**< pdu data 22 to 25 byte, offset: 0x24 */
  __IO uint32_t ANT_PDU_DATA7;                     /**< pdu data 26 to 29 byte, offset: 0x28 */
  __IO uint32_t CRCSEED;                           /**< crc seed, offset: 0x2C */
  __IO uint32_t DP_FUNCTION_CTRL;                  /**< datapath function control register, offset: 0x30 */
  __IO uint32_t DP_TEST_CTRL;                      /**< datapath test iinterface register, offset: 0x34 */
  __I  uint32_t BLE_DP_STATUS1;                    /**< datapath status register 1, offset: 0x38 */
  __I  uint32_t BLE_DP_STATUS2;                    /**< datapath status register 2, offset: 0x3C */
  __I  uint32_t BLE_DP_STATUS3;                    /**< datapath status register 3, offset: 0x40 */
  __I  uint32_t BLE_DP_STATUS4;                    /**< datapath status register 4, offset: 0x44 */
  __IO uint32_t RX_FRONT_END_CTRL1;                /**< rx front end control register 1, offset: 0x48 */
  __IO uint32_t RX_FRONT_END_CTRL2;                /**< rx front end control register 2, offset: 0x4C */
  __IO uint32_t FREQ_DOMAIN_CTRL1;                 /**< frequency domain control register 1, offset: 0x50 */
  __IO uint32_t FREQ_DOMAIN_CTRL2;                 /**< frequency domain control register 2, offset: 0x54 */
  __IO uint32_t FREQ_DOMAIN_CTRL3;                 /**< frequency domain control register 3, offset: 0x58 */
  __IO uint32_t FREQ_DOMAIN_CTRL4;                 /**< frequency domain control register 4, offset: 0x5C */
  __IO uint32_t FREQ_DOMAIN_CTRL5;                 /**< frequency domain control register 5, offset: 0x60 */
  __IO uint32_t FREQ_DOMAIN_CTRL6;                 /**< frequency domain control register 5, offset: 0x64 */
  __IO uint32_t HP_MODE_CTRL1;                     /**< when high hp mode training size same as cfo tracking., offset: 0x68 */
  __IO uint32_t HP_MODE_CTRL2;                     /**< q paramter in training period of phase offset iir of bmc, offset: 0x6C */
  __I  uint32_t FREQ_DOMAIN_STATUS1;               /**< frequency domain status register 1, offset: 0x70 */
  __I  uint32_t FREQ_DOMAIN_STATUS2;               /**< frequency domain status register 2, offset: 0x74 */
       uint8_t RESERVED_0[12];
  __IO uint32_t DP_AA_ERROR_CTRL;                  /**< AA error control register, offset: 0x84 */
  __IO uint32_t DP_INT;                            /**< data path interrupt register, offset: 0x88 */
  __IO uint32_t DP_AA_ERROR_TH;                    /**< AA error threshold register, offset: 0x8C */
  __IO uint32_t DF_ANTENNA_CTRL;                   /**< antenna register, offset: 0x90 */
  __IO uint32_t ANTENNA_MAP01;                     /**< antenna switch map register 0, offset: 0x94 */
  __IO uint32_t ANTENNA_MAP23;                     /**< antenna switch map register 1, offset: 0x98 */
  __IO uint32_t ANTENNA_MAP45;                     /**< antenna switch map register 2, offset: 0x9C */
  __IO uint32_t ANTENNA_MAP67;                     /**< antenna switch map register 3, offset: 0xA0 */
} BLEDP_Type;

/* ----------------------------------------------------------------------------
   -- BLEDP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup BLEDP_Register_Masks BLEDP Register Masks
 * @{
 */

/*! @name DP_TOP_SYSTEM_CTRL - datapath system control register */
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_PDU_LEN_IN_MASK (0x3FFFU)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_PDU_LEN_IN_SHIFT (0U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_PDU_LEN_IN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_RX_PDU_LEN_IN_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_RX_PDU_LEN_IN_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_AA_SEL_MASK     (0x4000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_AA_SEL_SHIFT    (14U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_AA_SEL(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_AA_SEL_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_AA_SEL_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_PDU_LEN_SEL_MASK (0x8000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_PDU_LEN_SEL_SHIFT (15U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_PDU_LEN_SEL(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_PDU_LEN_SEL_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_PDU_LEN_SEL_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_H_IDX_MASK      (0xFF0000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_H_IDX_SHIFT     (16U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_H_IDX(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_H_IDX_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_H_IDX_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_EN_SEL_MASK  (0x1000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_EN_SEL_SHIFT (24U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_EN_SEL(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_RX_EN_SEL_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_RX_EN_SEL_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_TX_EN_SEL_MASK  (0x2000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_TX_EN_SEL_SHIFT (25U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_TX_EN_SEL(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_TX_EN_SEL_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_TX_EN_SEL_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_REQ_MASK     (0x4000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_REQ_SHIFT    (26U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_REQ(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_RX_REQ_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_RX_REQ_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_TX_REQ_MASK     (0x8000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_TX_REQ_SHIFT    (27U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_TX_REQ(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_TX_REQ_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_TX_REQ_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_MODE_MASK    (0x30000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_MODE_SHIFT   (28U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_RX_MODE(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_RX_MODE_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_RX_MODE_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_ANT_DATA_START_MASK (0x40000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_ANT_DATA_START_SHIFT (30U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_ANT_DATA_START(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_ANT_DATA_START_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_ANT_DATA_START_MASK)
#define BLEDP_DP_TOP_SYSTEM_CTRL_DET_MODE_MASK   (0x80000000U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_DET_MODE_SHIFT  (31U)
#define BLEDP_DP_TOP_SYSTEM_CTRL_DET_MODE(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TOP_SYSTEM_CTRL_DET_MODE_SHIFT)) & BLEDP_DP_TOP_SYSTEM_CTRL_DET_MODE_MASK)

/*! @name PROP_MODE_CTRL - properity mode control register */
#define BLEDP_PROP_MODE_CTRL_PROP_AA_ADDR_IN_MASK (0xFFU)
#define BLEDP_PROP_MODE_CTRL_PROP_AA_ADDR_IN_SHIFT (0U)
#define BLEDP_PROP_MODE_CTRL_PROP_AA_ADDR_IN(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_AA_ADDR_IN_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_AA_ADDR_IN_MASK)
#define BLEDP_PROP_MODE_CTRL_PROP_CRC_NUM_MASK   (0x300U)
#define BLEDP_PROP_MODE_CTRL_PROP_CRC_NUM_SHIFT  (8U)
#define BLEDP_PROP_MODE_CTRL_PROP_CRC_NUM(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_CRC_NUM_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_CRC_NUM_MASK)
#define BLEDP_PROP_MODE_CTRL_PROP_AA_NUM_MASK    (0x3000U)
#define BLEDP_PROP_MODE_CTRL_PROP_AA_NUM_SHIFT   (12U)
#define BLEDP_PROP_MODE_CTRL_PROP_AA_NUM(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_AA_NUM_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_AA_NUM_MASK)
#define BLEDP_PROP_MODE_CTRL_PROP_PRE_NUM_MASK   (0x70000U)
#define BLEDP_PROP_MODE_CTRL_PROP_PRE_NUM_SHIFT  (16U)
#define BLEDP_PROP_MODE_CTRL_PROP_PRE_NUM(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_PRE_NUM_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_PRE_NUM_MASK)
#define BLEDP_PROP_MODE_CTRL_PROP_DATA_RATE_MASK (0x300000U)
#define BLEDP_PROP_MODE_CTRL_PROP_DATA_RATE_SHIFT (20U)
#define BLEDP_PROP_MODE_CTRL_PROP_DATA_RATE(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_DATA_RATE_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_DATA_RATE_MASK)
#define BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_RATE_MASK (0xC00000U)
#define BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_RATE_SHIFT (22U)
#define BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_RATE(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_RATE_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_RATE_MASK)
#define BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_MODE_MASK (0x1000000U)
#define BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_MODE_SHIFT (24U)
#define BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_MODE(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_MODE_SHIFT)) & BLEDP_PROP_MODE_CTRL_PROP_DIRECTION_MODE_MASK)
#define BLEDP_PROP_MODE_CTRL_RX_ALWAYS_ON_MASK   (0x2000000U)
#define BLEDP_PROP_MODE_CTRL_RX_ALWAYS_ON_SHIFT  (25U)
#define BLEDP_PROP_MODE_CTRL_RX_ALWAYS_ON(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_RX_ALWAYS_ON_SHIFT)) & BLEDP_PROP_MODE_CTRL_RX_ALWAYS_ON_MASK)
#define BLEDP_PROP_MODE_CTRL_TX_ALWAYS_ON_MASK   (0x4000000U)
#define BLEDP_PROP_MODE_CTRL_TX_ALWAYS_ON_SHIFT  (26U)
#define BLEDP_PROP_MODE_CTRL_TX_ALWAYS_ON(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_TX_ALWAYS_ON_SHIFT)) & BLEDP_PROP_MODE_CTRL_TX_ALWAYS_ON_MASK)
#define BLEDP_PROP_MODE_CTRL_TX_POWER_DONE_TIME_MASK (0xF8000000U)
#define BLEDP_PROP_MODE_CTRL_TX_POWER_DONE_TIME_SHIFT (27U)
#define BLEDP_PROP_MODE_CTRL_TX_POWER_DONE_TIME(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_PROP_MODE_CTRL_TX_POWER_DONE_TIME_SHIFT)) & BLEDP_PROP_MODE_CTRL_TX_POWER_DONE_TIME_MASK)

/*! @name ACCESS_ADDRESS - access address register */
#define BLEDP_ACCESS_ADDRESS_AA_ADDR_IN_MASK     (0xFFFFFFFFU)
#define BLEDP_ACCESS_ADDRESS_AA_ADDR_IN_SHIFT    (0U)
#define BLEDP_ACCESS_ADDRESS_AA_ADDR_IN(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_ACCESS_ADDRESS_AA_ADDR_IN_SHIFT)) & BLEDP_ACCESS_ADDRESS_AA_ADDR_IN_MASK)

/*! @name ANT_PDU_DATA0 - pdu data 0 to 1 byte, and preamble register */
#define BLEDP_ANT_PDU_DATA0_PDU_DATA0_MASK       (0xFFFFU)
#define BLEDP_ANT_PDU_DATA0_PDU_DATA0_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA0_PDU_DATA0(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA0_PDU_DATA0_SHIFT)) & BLEDP_ANT_PDU_DATA0_PDU_DATA0_MASK)
#define BLEDP_ANT_PDU_DATA0_PATTERN_SEL_MASK     (0xF0000U)
#define BLEDP_ANT_PDU_DATA0_PATTERN_SEL_SHIFT    (16U)
#define BLEDP_ANT_PDU_DATA0_PATTERN_SEL(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA0_PATTERN_SEL_SHIFT)) & BLEDP_ANT_PDU_DATA0_PATTERN_SEL_MASK)
#define BLEDP_ANT_PDU_DATA0_TEST_PATTERN_EN_MASK (0x100000U)
#define BLEDP_ANT_PDU_DATA0_TEST_PATTERN_EN_SHIFT (20U)
#define BLEDP_ANT_PDU_DATA0_TEST_PATTERN_EN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA0_TEST_PATTERN_EN_SHIFT)) & BLEDP_ANT_PDU_DATA0_TEST_PATTERN_EN_MASK)
#define BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_WEN_MASK (0x800000U)
#define BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_WEN_SHIFT (23U)
#define BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_WEN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_WEN_SHIFT)) & BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_WEN_MASK)
#define BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_MASK   (0xFF000000U)
#define BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_SHIFT  (24U)
#define BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_SHIFT)) & BLEDP_ANT_PDU_DATA0_PROP_PREAMBLE_MASK)

/*! @name ANT_PDU_DATA1 - pdu data 2 to 5 byte */
#define BLEDP_ANT_PDU_DATA1_PDU_DATA1_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA1_PDU_DATA1_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA1_PDU_DATA1(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA1_PDU_DATA1_SHIFT)) & BLEDP_ANT_PDU_DATA1_PDU_DATA1_MASK)

/*! @name ANT_PDU_DATA2 - pdu data 6 to 9 byte */
#define BLEDP_ANT_PDU_DATA2_PDU_DATA2_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA2_PDU_DATA2_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA2_PDU_DATA2(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA2_PDU_DATA2_SHIFT)) & BLEDP_ANT_PDU_DATA2_PDU_DATA2_MASK)

/*! @name ANT_PDU_DATA3 - pdu data 10 to 13 byte */
#define BLEDP_ANT_PDU_DATA3_PDU_DATA3_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA3_PDU_DATA3_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA3_PDU_DATA3(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA3_PDU_DATA3_SHIFT)) & BLEDP_ANT_PDU_DATA3_PDU_DATA3_MASK)

/*! @name ANT_PDU_DATA4 - pdu data 14 to 17 byte */
#define BLEDP_ANT_PDU_DATA4_PDU_DATA4_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA4_PDU_DATA4_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA4_PDU_DATA4(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA4_PDU_DATA4_SHIFT)) & BLEDP_ANT_PDU_DATA4_PDU_DATA4_MASK)

/*! @name ANT_PDU_DATA5 - pdu data 18 to 21 byte */
#define BLEDP_ANT_PDU_DATA5_PDU_DATA5_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA5_PDU_DATA5_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA5_PDU_DATA5(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA5_PDU_DATA5_SHIFT)) & BLEDP_ANT_PDU_DATA5_PDU_DATA5_MASK)

/*! @name ANT_PDU_DATA6 - pdu data 22 to 25 byte */
#define BLEDP_ANT_PDU_DATA6_PDU_DATA6_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA6_PDU_DATA6_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA6_PDU_DATA6(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA6_PDU_DATA6_SHIFT)) & BLEDP_ANT_PDU_DATA6_PDU_DATA6_MASK)

/*! @name ANT_PDU_DATA7 - pdu data 26 to 29 byte */
#define BLEDP_ANT_PDU_DATA7_PDU_DATA7_MASK       (0xFFFFFFFFU)
#define BLEDP_ANT_PDU_DATA7_PDU_DATA7_SHIFT      (0U)
#define BLEDP_ANT_PDU_DATA7_PDU_DATA7(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_ANT_PDU_DATA7_PDU_DATA7_SHIFT)) & BLEDP_ANT_PDU_DATA7_PDU_DATA7_MASK)

/*! @name CRCSEED - crc seed */
#define BLEDP_CRCSEED_CRC_SEED_IN_MASK           (0xFFFFFFU)
#define BLEDP_CRCSEED_CRC_SEED_IN_SHIFT          (0U)
#define BLEDP_CRCSEED_CRC_SEED_IN(x)             (((uint32_t)(((uint32_t)(x)) << BLEDP_CRCSEED_CRC_SEED_IN_SHIFT)) & BLEDP_CRCSEED_CRC_SEED_IN_MASK)
#define BLEDP_CRCSEED_CRC_SEED_WEN_MASK          (0x1000000U)
#define BLEDP_CRCSEED_CRC_SEED_WEN_SHIFT         (24U)
#define BLEDP_CRCSEED_CRC_SEED_WEN(x)            (((uint32_t)(((uint32_t)(x)) << BLEDP_CRCSEED_CRC_SEED_WEN_SHIFT)) & BLEDP_CRCSEED_CRC_SEED_WEN_MASK)

/*! @name DP_FUNCTION_CTRL - datapath function control register */
#define BLEDP_DP_FUNCTION_CTRL_DP_STATISTICS_SEL_MASK (0x7U)
#define BLEDP_DP_FUNCTION_CTRL_DP_STATISTICS_SEL_SHIFT (0U)
#define BLEDP_DP_FUNCTION_CTRL_DP_STATISTICS_SEL(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_DP_STATISTICS_SEL_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_DP_STATISTICS_SEL_MASK)
#define BLEDP_DP_FUNCTION_CTRL_CHF_COEF_WEN_MASK (0x8U)
#define BLEDP_DP_FUNCTION_CTRL_CHF_COEF_WEN_SHIFT (3U)
#define BLEDP_DP_FUNCTION_CTRL_CHF_COEF_WEN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_CHF_COEF_WEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_CHF_COEF_WEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_CHF_COEF_IDX_MASK (0x30U)
#define BLEDP_DP_FUNCTION_CTRL_CHF_COEF_IDX_SHIFT (4U)
#define BLEDP_DP_FUNCTION_CTRL_CHF_COEF_IDX(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_CHF_COEF_IDX_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_CHF_COEF_IDX_MASK)
#define BLEDP_DP_FUNCTION_CTRL_LP_SNR_LEN_AUTO_MASK (0x40U)
#define BLEDP_DP_FUNCTION_CTRL_LP_SNR_LEN_AUTO_SHIFT (6U)
#define BLEDP_DP_FUNCTION_CTRL_LP_SNR_LEN_AUTO(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_LP_SNR_LEN_AUTO_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_LP_SNR_LEN_AUTO_MASK)
#define BLEDP_DP_FUNCTION_CTRL_DOUT_ADJ_DIS_MASK (0x80U)
#define BLEDP_DP_FUNCTION_CTRL_DOUT_ADJ_DIS_SHIFT (7U)
#define BLEDP_DP_FUNCTION_CTRL_DOUT_ADJ_DIS(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_DOUT_ADJ_DIS_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_DOUT_ADJ_DIS_MASK)
#define BLEDP_DP_FUNCTION_CTRL_LP_ADJ_MODE_MASK  (0x100U)
#define BLEDP_DP_FUNCTION_CTRL_LP_ADJ_MODE_SHIFT (8U)
#define BLEDP_DP_FUNCTION_CTRL_LP_ADJ_MODE(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_LP_ADJ_MODE_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_LP_ADJ_MODE_MASK)
#define BLEDP_DP_FUNCTION_CTRL_FR_OFFSET_EN_MASK (0x200U)
#define BLEDP_DP_FUNCTION_CTRL_FR_OFFSET_EN_SHIFT (9U)
#define BLEDP_DP_FUNCTION_CTRL_FR_OFFSET_EN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_FR_OFFSET_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_FR_OFFSET_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_DC_AVE_EN_MASK    (0x400U)
#define BLEDP_DP_FUNCTION_CTRL_DC_AVE_EN_SHIFT   (10U)
#define BLEDP_DP_FUNCTION_CTRL_DC_AVE_EN(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_DC_AVE_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_DC_AVE_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_FIX_DELAY_EN_MASK (0x800U)
#define BLEDP_DP_FUNCTION_CTRL_FIX_DELAY_EN_SHIFT (11U)
#define BLEDP_DP_FUNCTION_CTRL_FIX_DELAY_EN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_FIX_DELAY_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_FIX_DELAY_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_MASK    (0x3000U)
#define BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_SHIFT   (12U)
#define BLEDP_DP_FUNCTION_CTRL_TRACK_LEN(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_WEN_MASK (0x4000U)
#define BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_WEN_SHIFT (14U)
#define BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_WEN(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_WEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_TRACK_LEN_WEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_FILT_EN_MASK (0x10000U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_FILT_EN_SHIFT (16U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_FILT_EN(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_XCORR_FILT_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_XCORR_FILT_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_FULLWIN_EN_MASK (0x20000U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_FULLWIN_EN_SHIFT (17U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_FULLWIN_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_XCORR_FULLWIN_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_XCORR_FULLWIN_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_MASK (0x40000U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_SHIFT (18U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_WEN_MASK (0x80000U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_WEN_SHIFT (19U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_WEN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_WEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_XCORR_AA_LEN_WEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_WIN_AUTO_EN_MASK (0x100000U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_WIN_AUTO_EN_SHIFT (20U)
#define BLEDP_DP_FUNCTION_CTRL_XCORR_WIN_AUTO_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_XCORR_WIN_AUTO_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_XCORR_WIN_AUTO_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_MASK (0x200000U)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_SHIFT (21U)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_MASK)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_WEN_MASK (0x400000U)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_WEN_SHIFT (22U)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_WEN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_WEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_RESAMPLER_TAP_WEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_BP_MASK (0x800000U)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_BP_SHIFT (23U)
#define BLEDP_DP_FUNCTION_CTRL_RESAMPLER_BP(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_RESAMPLER_BP_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_RESAMPLER_BP_MASK)
#define BLEDP_DP_FUNCTION_CTRL_FAGC_WIN_LEN_MASK (0x1000000U)
#define BLEDP_DP_FUNCTION_CTRL_FAGC_WIN_LEN_SHIFT (24U)
#define BLEDP_DP_FUNCTION_CTRL_FAGC_WIN_LEN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_FAGC_WIN_LEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_FAGC_WIN_LEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_FAGC_WEN_MASK     (0x2000000U)
#define BLEDP_DP_FUNCTION_CTRL_FAGC_WEN_SHIFT    (25U)
#define BLEDP_DP_FUNCTION_CTRL_FAGC_WEN(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_FAGC_WEN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_FAGC_WEN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_HP_CFO_EN_MASK    (0x4000000U)
#define BLEDP_DP_FUNCTION_CTRL_HP_CFO_EN_SHIFT   (26U)
#define BLEDP_DP_FUNCTION_CTRL_HP_CFO_EN(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_HP_CFO_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_HP_CFO_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_CFO_TRACK_EN_MASK (0x8000000U)
#define BLEDP_DP_FUNCTION_CTRL_CFO_TRACK_EN_SHIFT (27U)
#define BLEDP_DP_FUNCTION_CTRL_CFO_TRACK_EN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_CFO_TRACK_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_CFO_TRACK_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_CFO_INI_EN_MASK   (0x10000000U)
#define BLEDP_DP_FUNCTION_CTRL_CFO_INI_EN_SHIFT  (28U)
#define BLEDP_DP_FUNCTION_CTRL_CFO_INI_EN(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_CFO_INI_EN_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_CFO_INI_EN_MASK)
#define BLEDP_DP_FUNCTION_CTRL_ADC_IN_FLIP_MASK  (0x20000000U)
#define BLEDP_DP_FUNCTION_CTRL_ADC_IN_FLIP_SHIFT (29U)
#define BLEDP_DP_FUNCTION_CTRL_ADC_IN_FLIP(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_ADC_IN_FLIP_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_ADC_IN_FLIP_MASK)
#define BLEDP_DP_FUNCTION_CTRL_TX_EN_MODE_MASK   (0x40000000U)
#define BLEDP_DP_FUNCTION_CTRL_TX_EN_MODE_SHIFT  (30U)
#define BLEDP_DP_FUNCTION_CTRL_TX_EN_MODE(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_TX_EN_MODE_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_TX_EN_MODE_MASK)
#define BLEDP_DP_FUNCTION_CTRL_RX_EN_MODE_MASK   (0x80000000U)
#define BLEDP_DP_FUNCTION_CTRL_RX_EN_MODE_SHIFT  (31U)
#define BLEDP_DP_FUNCTION_CTRL_RX_EN_MODE(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_FUNCTION_CTRL_RX_EN_MODE_SHIFT)) & BLEDP_DP_FUNCTION_CTRL_RX_EN_MODE_MASK)

/*! @name DP_TEST_CTRL - datapath test iinterface register */
#define BLEDP_DP_TEST_CTRL_TIF_SEL_MASK          (0xFFU)
#define BLEDP_DP_TEST_CTRL_TIF_SEL_SHIFT         (0U)
#define BLEDP_DP_TEST_CTRL_TIF_SEL(x)            (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_TIF_SEL_SHIFT)) & BLEDP_DP_TEST_CTRL_TIF_SEL_MASK)
#define BLEDP_DP_TEST_CTRL_TIF_CLK_SEL_MASK      (0x300U)
#define BLEDP_DP_TEST_CTRL_TIF_CLK_SEL_SHIFT     (8U)
#define BLEDP_DP_TEST_CTRL_TIF_CLK_SEL(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_TIF_CLK_SEL_SHIFT)) & BLEDP_DP_TEST_CTRL_TIF_CLK_SEL_MASK)
#define BLEDP_DP_TEST_CTRL_CORDIC_DAC_OUT_MASK   (0x800U)
#define BLEDP_DP_TEST_CTRL_CORDIC_DAC_OUT_SHIFT  (11U)
#define BLEDP_DP_TEST_CTRL_CORDIC_DAC_OUT(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CORDIC_DAC_OUT_SHIFT)) & BLEDP_DP_TEST_CTRL_CORDIC_DAC_OUT_MASK)
#define BLEDP_DP_TEST_CTRL_TIF_EN_MASK           (0x1000U)
#define BLEDP_DP_TEST_CTRL_TIF_EN_SHIFT          (12U)
#define BLEDP_DP_TEST_CTRL_TIF_EN(x)             (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_TIF_EN_SHIFT)) & BLEDP_DP_TEST_CTRL_TIF_EN_MASK)
#define BLEDP_DP_TEST_CTRL_IMR_INV_MASK          (0x2000U)
#define BLEDP_DP_TEST_CTRL_IMR_INV_SHIFT         (13U)
#define BLEDP_DP_TEST_CTRL_IMR_INV(x)            (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_IMR_INV_SHIFT)) & BLEDP_DP_TEST_CTRL_IMR_INV_MASK)
#define BLEDP_DP_TEST_CTRL_CLK_TX_GATE_DIS_MASK  (0x4000U)
#define BLEDP_DP_TEST_CTRL_CLK_TX_GATE_DIS_SHIFT (14U)
#define BLEDP_DP_TEST_CTRL_CLK_TX_GATE_DIS(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CLK_TX_GATE_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_CLK_TX_GATE_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_BUF_FULL_OFFRF_DIS_MASK (0x8000U)
#define BLEDP_DP_TEST_CTRL_BUF_FULL_OFFRF_DIS_SHIFT (15U)
#define BLEDP_DP_TEST_CTRL_BUF_FULL_OFFRF_DIS(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_BUF_FULL_OFFRF_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_BUF_FULL_OFFRF_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_CLK_BUST_GATE_DIS_MASK (0x10000U)
#define BLEDP_DP_TEST_CTRL_CLK_BUST_GATE_DIS_SHIFT (16U)
#define BLEDP_DP_TEST_CTRL_CLK_BUST_GATE_DIS(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CLK_BUST_GATE_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_CLK_BUST_GATE_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_CLK_RX_GATE_DIS_MASK  (0x20000U)
#define BLEDP_DP_TEST_CTRL_CLK_RX_GATE_DIS_SHIFT (17U)
#define BLEDP_DP_TEST_CTRL_CLK_RX_GATE_DIS(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CLK_RX_GATE_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_CLK_RX_GATE_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_CLK_LPDET_GATE_DIS_MASK (0x40000U)
#define BLEDP_DP_TEST_CTRL_CLK_LPDET_GATE_DIS_SHIFT (18U)
#define BLEDP_DP_TEST_CTRL_CLK_LPDET_GATE_DIS(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CLK_LPDET_GATE_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_CLK_LPDET_GATE_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_CLK_HPDET_GATE_DIS_MASK (0x80000U)
#define BLEDP_DP_TEST_CTRL_CLK_HPDET_GATE_DIS_SHIFT (19U)
#define BLEDP_DP_TEST_CTRL_CLK_HPDET_GATE_DIS(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CLK_HPDET_GATE_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_CLK_HPDET_GATE_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_CLK_RFE_GATE_DIS_MASK (0x100000U)
#define BLEDP_DP_TEST_CTRL_CLK_RFE_GATE_DIS_SHIFT (20U)
#define BLEDP_DP_TEST_CTRL_CLK_RFE_GATE_DIS(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_CLK_RFE_GATE_DIS_SHIFT)) & BLEDP_DP_TEST_CTRL_CLK_RFE_GATE_DIS_MASK)
#define BLEDP_DP_TEST_CTRL_IQSWAP_XOR_MASK       (0x200000U)
#define BLEDP_DP_TEST_CTRL_IQSWAP_XOR_SHIFT      (21U)
#define BLEDP_DP_TEST_CTRL_IQSWAP_XOR(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_IQSWAP_XOR_SHIFT)) & BLEDP_DP_TEST_CTRL_IQSWAP_XOR_MASK)
#define BLEDP_DP_TEST_CTRL_DAC_TEST_EN_MASK      (0x800000U)
#define BLEDP_DP_TEST_CTRL_DAC_TEST_EN_SHIFT     (23U)
#define BLEDP_DP_TEST_CTRL_DAC_TEST_EN(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_DAC_TEST_EN_SHIFT)) & BLEDP_DP_TEST_CTRL_DAC_TEST_EN_MASK)
#define BLEDP_DP_TEST_CTRL_DAC_TEST_MASK         (0xFF000000U)
#define BLEDP_DP_TEST_CTRL_DAC_TEST_SHIFT        (24U)
#define BLEDP_DP_TEST_CTRL_DAC_TEST(x)           (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_TEST_CTRL_DAC_TEST_SHIFT)) & BLEDP_DP_TEST_CTRL_DAC_TEST_MASK)

/*! @name BLE_DP_STATUS1 - datapath status register 1 */
#define BLEDP_BLE_DP_STATUS1_SNR_EST_MASK        (0xFFU)
#define BLEDP_BLE_DP_STATUS1_SNR_EST_SHIFT       (0U)
#define BLEDP_BLE_DP_STATUS1_SNR_EST(x)          (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_SNR_EST_SHIFT)) & BLEDP_BLE_DP_STATUS1_SNR_EST_MASK)
#define BLEDP_BLE_DP_STATUS1_CNR_EST_MASK        (0x3F00U)
#define BLEDP_BLE_DP_STATUS1_CNR_EST_SHIFT       (8U)
#define BLEDP_BLE_DP_STATUS1_CNR_EST(x)          (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_CNR_EST_SHIFT)) & BLEDP_BLE_DP_STATUS1_CNR_EST_MASK)
#define BLEDP_BLE_DP_STATUS1_AGC_RSSI_MASK       (0xFF0000U)
#define BLEDP_BLE_DP_STATUS1_AGC_RSSI_SHIFT      (16U)
#define BLEDP_BLE_DP_STATUS1_AGC_RSSI(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_AGC_RSSI_SHIFT)) & BLEDP_BLE_DP_STATUS1_AGC_RSSI_MASK)
#define BLEDP_BLE_DP_STATUS1_AGC_RSSI_READY_MASK (0x1000000U)
#define BLEDP_BLE_DP_STATUS1_AGC_RSSI_READY_SHIFT (24U)
#define BLEDP_BLE_DP_STATUS1_AGC_RSSI_READY(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_AGC_RSSI_READY_SHIFT)) & BLEDP_BLE_DP_STATUS1_AGC_RSSI_READY_MASK)
#define BLEDP_BLE_DP_STATUS1_SNR_VLD_MASK        (0x2000000U)
#define BLEDP_BLE_DP_STATUS1_SNR_VLD_SHIFT       (25U)
#define BLEDP_BLE_DP_STATUS1_SNR_VLD(x)          (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_SNR_VLD_SHIFT)) & BLEDP_BLE_DP_STATUS1_SNR_VLD_MASK)
#define BLEDP_BLE_DP_STATUS1_CNR_VLD_MASK        (0x4000000U)
#define BLEDP_BLE_DP_STATUS1_CNR_VLD_SHIFT       (26U)
#define BLEDP_BLE_DP_STATUS1_CNR_VLD(x)          (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_CNR_VLD_SHIFT)) & BLEDP_BLE_DP_STATUS1_CNR_VLD_MASK)
#define BLEDP_BLE_DP_STATUS1_TX_BUSY_MASK        (0x8000000U)
#define BLEDP_BLE_DP_STATUS1_TX_BUSY_SHIFT       (27U)
#define BLEDP_BLE_DP_STATUS1_TX_BUSY(x)          (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS1_TX_BUSY_SHIFT)) & BLEDP_BLE_DP_STATUS1_TX_BUSY_MASK)

/*! @name BLE_DP_STATUS2 - datapath status register 2 */
#define BLEDP_BLE_DP_STATUS2_VALID_PCK_NUM_MASK  (0xFFFFU)
#define BLEDP_BLE_DP_STATUS2_VALID_PCK_NUM_SHIFT (0U)
#define BLEDP_BLE_DP_STATUS2_VALID_PCK_NUM(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS2_VALID_PCK_NUM_SHIFT)) & BLEDP_BLE_DP_STATUS2_VALID_PCK_NUM_MASK)
#define BLEDP_BLE_DP_STATUS2_AA_ERR_NUM_MASK     (0x3F0000U)
#define BLEDP_BLE_DP_STATUS2_AA_ERR_NUM_SHIFT    (16U)
#define BLEDP_BLE_DP_STATUS2_AA_ERR_NUM(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS2_AA_ERR_NUM_SHIFT)) & BLEDP_BLE_DP_STATUS2_AA_ERR_NUM_MASK)
#define BLEDP_BLE_DP_STATUS2_CRC_ERROR_MASK      (0x20000000U)
#define BLEDP_BLE_DP_STATUS2_CRC_ERROR_SHIFT     (29U)
#define BLEDP_BLE_DP_STATUS2_CRC_ERROR(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS2_CRC_ERROR_SHIFT)) & BLEDP_BLE_DP_STATUS2_CRC_ERROR_MASK)
#define BLEDP_BLE_DP_STATUS2_BURST_DET_MASK      (0x40000000U)
#define BLEDP_BLE_DP_STATUS2_BURST_DET_SHIFT     (30U)
#define BLEDP_BLE_DP_STATUS2_BURST_DET(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS2_BURST_DET_SHIFT)) & BLEDP_BLE_DP_STATUS2_BURST_DET_MASK)
#define BLEDP_BLE_DP_STATUS2_DP_STATUS_VLD_0_MASK (0x80000000U)
#define BLEDP_BLE_DP_STATUS2_DP_STATUS_VLD_0_SHIFT (31U)
#define BLEDP_BLE_DP_STATUS2_DP_STATUS_VLD_0(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS2_DP_STATUS_VLD_0_SHIFT)) & BLEDP_BLE_DP_STATUS2_DP_STATUS_VLD_0_MASK)

/*! @name BLE_DP_STATUS3 - datapath status register 3 */
#define BLEDP_BLE_DP_STATUS3_FD_CFO_TRACK_MASK   (0x7FFU)
#define BLEDP_BLE_DP_STATUS3_FD_CFO_TRACK_SHIFT  (0U)
#define BLEDP_BLE_DP_STATUS3_FD_CFO_TRACK(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS3_FD_CFO_TRACK_SHIFT)) & BLEDP_BLE_DP_STATUS3_FD_CFO_TRACK_MASK)
#define BLEDP_BLE_DP_STATUS3_CFO_EST_FD_MASK     (0x7FF0000U)
#define BLEDP_BLE_DP_STATUS3_CFO_EST_FD_SHIFT    (16U)
#define BLEDP_BLE_DP_STATUS3_CFO_EST_FD(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS3_CFO_EST_FD_SHIFT)) & BLEDP_BLE_DP_STATUS3_CFO_EST_FD_MASK)

/*! @name BLE_DP_STATUS4 - datapath status register 4 */
#define BLEDP_BLE_DP_STATUS4_RESAMPLER_PH_MASK   (0x3FFU)
#define BLEDP_BLE_DP_STATUS4_RESAMPLER_PH_SHIFT  (0U)
#define BLEDP_BLE_DP_STATUS4_RESAMPLER_PH(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS4_RESAMPLER_PH_SHIFT)) & BLEDP_BLE_DP_STATUS4_RESAMPLER_PH_MASK)
#define BLEDP_BLE_DP_STATUS4_HP_CFO_MASK         (0xFFF0000U)
#define BLEDP_BLE_DP_STATUS4_HP_CFO_SHIFT        (16U)
#define BLEDP_BLE_DP_STATUS4_HP_CFO(x)           (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS4_HP_CFO_SHIFT)) & BLEDP_BLE_DP_STATUS4_HP_CFO_MASK)
#define BLEDP_BLE_DP_STATUS4_HP_CFO_VLD_MASK     (0x80000000U)
#define BLEDP_BLE_DP_STATUS4_HP_CFO_VLD_SHIFT    (31U)
#define BLEDP_BLE_DP_STATUS4_HP_CFO_VLD(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_BLE_DP_STATUS4_HP_CFO_VLD_SHIFT)) & BLEDP_BLE_DP_STATUS4_HP_CFO_VLD_MASK)

/*! @name RX_FRONT_END_CTRL1 - rx front end control register 1 */
#define BLEDP_RX_FRONT_END_CTRL1_CFO_COMP_MASK   (0x7FFFU)
#define BLEDP_RX_FRONT_END_CTRL1_CFO_COMP_SHIFT  (0U)
#define BLEDP_RX_FRONT_END_CTRL1_CFO_COMP(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL1_CFO_COMP_SHIFT)) & BLEDP_RX_FRONT_END_CTRL1_CFO_COMP_MASK)
#define BLEDP_RX_FRONT_END_CTRL1_DCNOTCH_GIN_MASK (0x30000U)
#define BLEDP_RX_FRONT_END_CTRL1_DCNOTCH_GIN_SHIFT (16U)
#define BLEDP_RX_FRONT_END_CTRL1_DCNOTCH_GIN(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL1_DCNOTCH_GIN_SHIFT)) & BLEDP_RX_FRONT_END_CTRL1_DCNOTCH_GIN_MASK)

/*! @name RX_FRONT_END_CTRL2 - rx front end control register 2 */
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_GAIN_MASK  (0x7FFU)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_GAIN_SHIFT (0U)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_GAIN(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_FAGC_GAIN_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_FAGC_GAIN_MASK)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_INI_VAL_MASK (0x800U)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_INI_VAL_SHIFT (11U)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_INI_VAL(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_FAGC_INI_VAL_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_FAGC_INI_VAL_MASK)
#define BLEDP_RX_FRONT_END_CTRL2_CNR_IDX_DELTA_MASK (0xF000U)
#define BLEDP_RX_FRONT_END_CTRL2_CNR_IDX_DELTA_SHIFT (12U)
#define BLEDP_RX_FRONT_END_CTRL2_CNR_IDX_DELTA(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_CNR_IDX_DELTA_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_CNR_IDX_DELTA_MASK)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_REF_MASK   (0xFF0000U)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_REF_SHIFT  (16U)
#define BLEDP_RX_FRONT_END_CTRL2_FAGC_REF(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_FAGC_REF_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_FAGC_REF_MASK)
#define BLEDP_RX_FRONT_END_CTRL2_CORDIC_MIN_VIN_TH_MASK (0xF000000U)
#define BLEDP_RX_FRONT_END_CTRL2_CORDIC_MIN_VIN_TH_SHIFT (24U)
#define BLEDP_RX_FRONT_END_CTRL2_CORDIC_MIN_VIN_TH(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_CORDIC_MIN_VIN_TH_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_CORDIC_MIN_VIN_TH_MASK)
#define BLEDP_RX_FRONT_END_CTRL2_FREQ_TRADE_EN_MASK (0x10000000U)
#define BLEDP_RX_FRONT_END_CTRL2_FREQ_TRADE_EN_SHIFT (28U)
#define BLEDP_RX_FRONT_END_CTRL2_FREQ_TRADE_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_FREQ_TRADE_EN_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_FREQ_TRADE_EN_MASK)
#define BLEDP_RX_FRONT_END_CTRL2_CHN_SHIFT_MASK  (0xE0000000U)
#define BLEDP_RX_FRONT_END_CTRL2_CHN_SHIFT_SHIFT (29U)
#define BLEDP_RX_FRONT_END_CTRL2_CHN_SHIFT(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_RX_FRONT_END_CTRL2_CHN_SHIFT_SHIFT)) & BLEDP_RX_FRONT_END_CTRL2_CHN_SHIFT_MASK)

/*! @name FREQ_DOMAIN_CTRL1 - frequency domain control register 1 */
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_IN0_MASK (0xFFU)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_IN0_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_IN0(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_IN0_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_IN0_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_WEN_MASK (0x100U)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_WEN_SHIFT (8U)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_WEN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_WEN_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL1_SYNC_WORD_WEN_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_P_SEL_MASK  (0x8000U)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_P_SEL_SHIFT (15U)
#define BLEDP_FREQ_DOMAIN_CTRL1_SYNC_P_SEL(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL1_SYNC_P_SEL_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL1_SYNC_P_SEL_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL1_RD_EXBIT_EN_MASK (0x10000U)
#define BLEDP_FREQ_DOMAIN_CTRL1_RD_EXBIT_EN_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_CTRL1_RD_EXBIT_EN(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL1_RD_EXBIT_EN_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL1_RD_EXBIT_EN_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL1_RFAGC_TRACK_DLY_MASK (0xE0000U)
#define BLEDP_FREQ_DOMAIN_CTRL1_RFAGC_TRACK_DLY_SHIFT (17U)
#define BLEDP_FREQ_DOMAIN_CTRL1_RFAGC_TRACK_DLY(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL1_RFAGC_TRACK_DLY_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL1_RFAGC_TRACK_DLY_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL1_PROP_DF_16US_MASK (0xFF000000U)
#define BLEDP_FREQ_DOMAIN_CTRL1_PROP_DF_16US_SHIFT (24U)
#define BLEDP_FREQ_DOMAIN_CTRL1_PROP_DF_16US(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL1_PROP_DF_16US_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL1_PROP_DF_16US_MASK)

/*! @name FREQ_DOMAIN_CTRL2 - frequency domain control register 2 */
#define BLEDP_FREQ_DOMAIN_CTRL2_SYNC_WORD_IN1_MASK (0xFFFFFFFFU)
#define BLEDP_FREQ_DOMAIN_CTRL2_SYNC_WORD_IN1_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_CTRL2_SYNC_WORD_IN1(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL2_SYNC_WORD_IN1_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL2_SYNC_WORD_IN1_MASK)

/*! @name FREQ_DOMAIN_CTRL3 - frequency domain control register 3 */
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH3_MASK (0x3FU)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH3_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH3(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH3_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH3_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH2_MASK (0x3F00U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH2_SHIFT (8U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH2(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH2_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH2_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH1_MASK (0x3F0000U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH1_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH1(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH1_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH1_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH0_MASK (0x3F000000U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH0_SHIFT (24U)
#define BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH0(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH0_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL3_XCORR_PAR_TH0_MASK)

/*! @name FREQ_DOMAIN_CTRL4 - frequency domain control register 4 */
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH3_MASK (0x3FU)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH3_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH3(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH3_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH3_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH2_MASK (0x3F00U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH2_SHIFT (8U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH2(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH2_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH2_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH1_MASK (0x3F0000U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH1_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH1(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH1_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH1_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH0_MASK (0x3F000000U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH0_SHIFT (24U)
#define BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH0(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH0_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL4_XCORR_POW_TH0_MASK)

/*! @name FREQ_DOMAIN_CTRL5 - frequency domain control register 5 */
#define BLEDP_FREQ_DOMAIN_CTRL5_GAIN_TED_MASK    (0x3U)
#define BLEDP_FREQ_DOMAIN_CTRL5_GAIN_TED_SHIFT   (0U)
#define BLEDP_FREQ_DOMAIN_CTRL5_GAIN_TED(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_GAIN_TED_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_GAIN_TED_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_VALUE_MASK (0x70U)
#define BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_VALUE_SHIFT (4U)
#define BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_VALUE(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_VALUE_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_VALUE_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_EN_MASK (0x80U)
#define BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_EN_SHIFT (7U)
#define BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_EN_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_SYNC_DIN_SAT_EN_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_CNT_SETTLE_IDX_MASK (0x700U)
#define BLEDP_FREQ_DOMAIN_CTRL5_CNT_SETTLE_IDX_SHIFT (8U)
#define BLEDP_FREQ_DOMAIN_CTRL5_CNT_SETTLE_IDX(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_CNT_SETTLE_IDX_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_CNT_SETTLE_IDX_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_TRIG_XCORR_CNT_MASK (0xF000U)
#define BLEDP_FREQ_DOMAIN_CTRL5_TRIG_XCORR_CNT_SHIFT (12U)
#define BLEDP_FREQ_DOMAIN_CTRL5_TRIG_XCORR_CNT(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_TRIG_XCORR_CNT_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_TRIG_XCORR_CNT_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH3_MASK (0xF0000U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH3_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH3(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH3_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH3_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH2_MASK (0xF00000U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH2_SHIFT (20U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH2(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH2_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH2_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH1_MASK (0xF000000U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH1_SHIFT (24U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH1(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH1_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH1_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH0_MASK (0xF0000000U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH0_SHIFT (28U)
#define BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH0(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH0_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL5_XCORR_RSSI_TH0_MASK)

/*! @name FREQ_DOMAIN_CTRL6 - frequency domain control register 5 */
#define BLEDP_FREQ_DOMAIN_CTRL6_HP_TRAIN_SIZ_MASK (0x1FU)
#define BLEDP_FREQ_DOMAIN_CTRL6_HP_TRAIN_SIZ_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_CTRL6_HP_TRAIN_SIZ(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL6_HP_TRAIN_SIZ_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL6_HP_TRAIN_SIZ_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL6_HP_HIDX_GAIN_MASK (0xFF00U)
#define BLEDP_FREQ_DOMAIN_CTRL6_HP_HIDX_GAIN_SHIFT (8U)
#define BLEDP_FREQ_DOMAIN_CTRL6_HP_HIDX_GAIN(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL6_HP_HIDX_GAIN_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL6_HP_HIDX_GAIN_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL6_H_REF_GAIN_MASK  (0x3F0000U)
#define BLEDP_FREQ_DOMAIN_CTRL6_H_REF_GAIN_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_CTRL6_H_REF_GAIN(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL6_H_REF_GAIN_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL6_H_REF_GAIN_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL6_DET_FR_IDX_MASK  (0x3000000U)
#define BLEDP_FREQ_DOMAIN_CTRL6_DET_FR_IDX_SHIFT (24U)
#define BLEDP_FREQ_DOMAIN_CTRL6_DET_FR_IDX(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL6_DET_FR_IDX_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL6_DET_FR_IDX_MASK)
#define BLEDP_FREQ_DOMAIN_CTRL6_CFO_FR_IDX_MASK  (0x30000000U)
#define BLEDP_FREQ_DOMAIN_CTRL6_CFO_FR_IDX_SHIFT (28U)
#define BLEDP_FREQ_DOMAIN_CTRL6_CFO_FR_IDX(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_CTRL6_CFO_FR_IDX_SHIFT)) & BLEDP_FREQ_DOMAIN_CTRL6_CFO_FR_IDX_MASK)

/*! @name HP_MODE_CTRL1 - when high hp mode training size same as cfo tracking. */
#define BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRACK_MASK  (0x3FU)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRACK_SHIFT (0U)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRACK(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRACK_SHIFT)) & BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRACK_MASK)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRAIN_MASK  (0x3F00U)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRAIN_SHIFT (8U)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRAIN(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRAIN_SHIFT)) & BLEDP_HP_MODE_CTRL1_HP_BMC_P_TRAIN_MASK)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_CZ1_MASK      (0x3F0000U)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_CZ1_SHIFT     (16U)
#define BLEDP_HP_MODE_CTRL1_HP_BMC_CZ1(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL1_HP_BMC_CZ1_SHIFT)) & BLEDP_HP_MODE_CTRL1_HP_BMC_CZ1_MASK)
#define BLEDP_HP_MODE_CTRL1_BUF_IDX_DELTA_MASK   (0xF000000U)
#define BLEDP_HP_MODE_CTRL1_BUF_IDX_DELTA_SHIFT  (24U)
#define BLEDP_HP_MODE_CTRL1_BUF_IDX_DELTA(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL1_BUF_IDX_DELTA_SHIFT)) & BLEDP_HP_MODE_CTRL1_BUF_IDX_DELTA_MASK)
#define BLEDP_HP_MODE_CTRL1_WMF2_DSAMP_IDX_MASK  (0x70000000U)
#define BLEDP_HP_MODE_CTRL1_WMF2_DSAMP_IDX_SHIFT (28U)
#define BLEDP_HP_MODE_CTRL1_WMF2_DSAMP_IDX(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL1_WMF2_DSAMP_IDX_SHIFT)) & BLEDP_HP_MODE_CTRL1_WMF2_DSAMP_IDX_MASK)
#define BLEDP_HP_MODE_CTRL1_HP_TRAIN_SIZ_FIX_MASK (0x80000000U)
#define BLEDP_HP_MODE_CTRL1_HP_TRAIN_SIZ_FIX_SHIFT (31U)
#define BLEDP_HP_MODE_CTRL1_HP_TRAIN_SIZ_FIX(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL1_HP_TRAIN_SIZ_FIX_SHIFT)) & BLEDP_HP_MODE_CTRL1_HP_TRAIN_SIZ_FIX_MASK)

/*! @name HP_MODE_CTRL2 - q paramter in training period of phase offset iir of bmc */
#define BLEDP_HP_MODE_CTRL2_SNR_EST_REF_MASK     (0xFFU)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_REF_SHIFT    (0U)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_REF(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL2_SNR_EST_REF_SHIFT)) & BLEDP_HP_MODE_CTRL2_SNR_EST_REF_MASK)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_LEN_MASK     (0x300U)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_LEN_SHIFT    (8U)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_LEN(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL2_SNR_EST_LEN_SHIFT)) & BLEDP_HP_MODE_CTRL2_SNR_EST_LEN_MASK)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_EN_MASK      (0x1000U)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_EN_SHIFT     (12U)
#define BLEDP_HP_MODE_CTRL2_SNR_EST_EN(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL2_SNR_EST_EN_SHIFT)) & BLEDP_HP_MODE_CTRL2_SNR_EST_EN_MASK)
#define BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRACK_MASK  (0xFF0000U)
#define BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRACK_SHIFT (16U)
#define BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRACK(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRACK_SHIFT)) & BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRACK_MASK)
#define BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRAIN_MASK  (0xFF000000U)
#define BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRAIN_SHIFT (24U)
#define BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRAIN(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRAIN_SHIFT)) & BLEDP_HP_MODE_CTRL2_HP_BMC_Q_TRAIN_MASK)

/*! @name FREQ_DOMAIN_STATUS1 - frequency domain status register 1 */
#define BLEDP_FREQ_DOMAIN_STATUS1_MAX_XCORR_MASK (0x3FFU)
#define BLEDP_FREQ_DOMAIN_STATUS1_MAX_XCORR_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_STATUS1_MAX_XCORR(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_STATUS1_MAX_XCORR_SHIFT)) & BLEDP_FREQ_DOMAIN_STATUS1_MAX_XCORR_MASK)
#define BLEDP_FREQ_DOMAIN_STATUS1_PKT_OFFSET_COM_MASK (0x1FF0000U)
#define BLEDP_FREQ_DOMAIN_STATUS1_PKT_OFFSET_COM_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_STATUS1_PKT_OFFSET_COM(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_STATUS1_PKT_OFFSET_COM_SHIFT)) & BLEDP_FREQ_DOMAIN_STATUS1_PKT_OFFSET_COM_MASK)
#define BLEDP_FREQ_DOMAIN_STATUS1_NIDX_MASK      (0xF0000000U)
#define BLEDP_FREQ_DOMAIN_STATUS1_NIDX_SHIFT     (28U)
#define BLEDP_FREQ_DOMAIN_STATUS1_NIDX(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_STATUS1_NIDX_SHIFT)) & BLEDP_FREQ_DOMAIN_STATUS1_NIDX_MASK)

/*! @name FREQ_DOMAIN_STATUS2 - frequency domain status register 2 */
#define BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_SPWR_MASK (0x3FFU)
#define BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_SPWR_SHIFT (0U)
#define BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_SPWR(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_SPWR_SHIFT)) & BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_SPWR_MASK)
#define BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_XCORR_MASK (0x3FF0000U)
#define BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_XCORR_SHIFT (16U)
#define BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_XCORR(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_XCORR_SHIFT)) & BLEDP_FREQ_DOMAIN_STATUS2_MAX_PAR_XCORR_MASK)

/*! @name DP_AA_ERROR_CTRL - AA error control register */
#define BLEDP_DP_AA_ERROR_CTRL_IQSWAP_SEL_MASK   (0x1U)
#define BLEDP_DP_AA_ERROR_CTRL_IQSWAP_SEL_SHIFT  (0U)
#define BLEDP_DP_AA_ERROR_CTRL_IQSWAP_SEL(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_CTRL_IQSWAP_SEL_SHIFT)) & BLEDP_DP_AA_ERROR_CTRL_IQSWAP_SEL_MASK)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_EN_MASK  (0x2U)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_EN_SHIFT (1U)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_EN(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_EN_SHIFT)) & BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_EN_MASK)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_EN_MASK (0x4U)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_EN_SHIFT (2U)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_EN_SHIFT)) & BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_EN_MASK)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_SEL_MASK (0x8U)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_SEL_SHIFT (3U)
#define BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_SEL(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_SEL_SHIFT)) & BLEDP_DP_AA_ERROR_CTRL_AA_ERROR_CNR_SEL_MASK)

/*! @name DP_INT - data path interrupt register */
#define BLEDP_DP_INT_DP_INTERRUPT0_MASK          (0x1U)
#define BLEDP_DP_INT_DP_INTERRUPT0_SHIFT         (0U)
#define BLEDP_DP_INT_DP_INTERRUPT0(x)            (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT0_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT0_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT1_MASK          (0x2U)
#define BLEDP_DP_INT_DP_INTERRUPT1_SHIFT         (1U)
#define BLEDP_DP_INT_DP_INTERRUPT1(x)            (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT1_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT1_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT2_MASK          (0x4U)
#define BLEDP_DP_INT_DP_INTERRUPT2_SHIFT         (2U)
#define BLEDP_DP_INT_DP_INTERRUPT2(x)            (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT2_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT2_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT_MASK           (0x8U)
#define BLEDP_DP_INT_DP_INTERRUPT_SHIFT          (3U)
#define BLEDP_DP_INT_DP_INTERRUPT(x)             (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT0_SEL_MASK      (0xF0000U)
#define BLEDP_DP_INT_DP_INTERRUPT0_SEL_SHIFT     (16U)
#define BLEDP_DP_INT_DP_INTERRUPT0_SEL(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT0_SEL_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT0_SEL_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT1_SEL_MASK      (0xF00000U)
#define BLEDP_DP_INT_DP_INTERRUPT1_SEL_SHIFT     (20U)
#define BLEDP_DP_INT_DP_INTERRUPT1_SEL(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT1_SEL_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT1_SEL_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT2_SEL_MASK      (0xF000000U)
#define BLEDP_DP_INT_DP_INTERRUPT2_SEL_SHIFT     (24U)
#define BLEDP_DP_INT_DP_INTERRUPT2_SEL(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT2_SEL_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT2_SEL_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT0_MSK_MASK      (0x10000000U)
#define BLEDP_DP_INT_DP_INTERRUPT0_MSK_SHIFT     (28U)
#define BLEDP_DP_INT_DP_INTERRUPT0_MSK(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT0_MSK_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT0_MSK_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT1_MSK_MASK      (0x20000000U)
#define BLEDP_DP_INT_DP_INTERRUPT1_MSK_SHIFT     (29U)
#define BLEDP_DP_INT_DP_INTERRUPT1_MSK(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT1_MSK_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT1_MSK_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT2_MSK_MASK      (0x40000000U)
#define BLEDP_DP_INT_DP_INTERRUPT2_MSK_SHIFT     (30U)
#define BLEDP_DP_INT_DP_INTERRUPT2_MSK(x)        (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT2_MSK_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT2_MSK_MASK)
#define BLEDP_DP_INT_DP_INTERRUPT_MSK_MASK       (0x80000000U)
#define BLEDP_DP_INT_DP_INTERRUPT_MSK_SHIFT      (31U)
#define BLEDP_DP_INT_DP_INTERRUPT_MSK(x)         (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_INT_DP_INTERRUPT_MSK_SHIFT)) & BLEDP_DP_INT_DP_INTERRUPT_MSK_MASK)

/*! @name DP_AA_ERROR_TH - AA error threshold register */
#define BLEDP_DP_AA_ERROR_TH_HP_TRAIN_POSITION_MASK (0x1U)
#define BLEDP_DP_AA_ERROR_TH_HP_TRAIN_POSITION_SHIFT (0U)
#define BLEDP_DP_AA_ERROR_TH_HP_TRAIN_POSITION(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_HP_TRAIN_POSITION_SHIFT)) & BLEDP_DP_AA_ERROR_TH_HP_TRAIN_POSITION_MASK)
#define BLEDP_DP_AA_ERROR_TH_CORDIC_IN_SCALE_MASK (0x2U)
#define BLEDP_DP_AA_ERROR_TH_CORDIC_IN_SCALE_SHIFT (1U)
#define BLEDP_DP_AA_ERROR_TH_CORDIC_IN_SCALE(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_CORDIC_IN_SCALE_SHIFT)) & BLEDP_DP_AA_ERROR_TH_CORDIC_IN_SCALE_MASK)
#define BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_SEL_MASK (0x4U)
#define BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_SEL_SHIFT (2U)
#define BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_SEL(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_SEL_SHIFT)) & BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_SEL_MASK)
#define BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_EN_MASK (0x8U)
#define BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_EN_SHIFT (3U)
#define BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_EN_SHIFT)) & BLEDP_DP_AA_ERROR_TH_PAR_AUTO_HIGHER_EN_MASK)
#define BLEDP_DP_AA_ERROR_TH_SNR_GOOD_TH_MASK    (0x70U)
#define BLEDP_DP_AA_ERROR_TH_SNR_GOOD_TH_SHIFT   (4U)
#define BLEDP_DP_AA_ERROR_TH_SNR_GOOD_TH(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_SNR_GOOD_TH_SHIFT)) & BLEDP_DP_AA_ERROR_TH_SNR_GOOD_TH_MASK)
#define BLEDP_DP_AA_ERROR_TH_CNR_GOOD_TH_MASK    (0x3F00U)
#define BLEDP_DP_AA_ERROR_TH_CNR_GOOD_TH_SHIFT   (8U)
#define BLEDP_DP_AA_ERROR_TH_CNR_GOOD_TH(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_CNR_GOOD_TH_SHIFT)) & BLEDP_DP_AA_ERROR_TH_CNR_GOOD_TH_MASK)
#define BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_TH_MASK   (0xFF0000U)
#define BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_TH_SHIFT  (16U)
#define BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_TH(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_TH_SHIFT)) & BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_TH_MASK)
#define BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_DBM_MASK  (0xFF000000U)
#define BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_DBM_SHIFT (24U)
#define BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_DBM(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_DBM_SHIFT)) & BLEDP_DP_AA_ERROR_TH_RSSI_GOOD_DBM_MASK)

/*! @name DF_ANTENNA_CTRL - antenna register */
#define BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_8F_MASK (0x3U)
#define BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_8F_SHIFT (0U)
#define BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_8F(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_8F_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_8F_MASK)
#define BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_07_MASK (0xCU)
#define BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_07_SHIFT (2U)
#define BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_07(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_07_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_SWITCH_MAP_SEL_07_MASK)
#define BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_MASK (0xF0U)
#define BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_SHIFT (4U)
#define BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_MASK)
#define BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_WEN_MASK (0x100U)
#define BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_WEN_SHIFT (8U)
#define BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_WEN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_WEN_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_EXT_ANTENNA_NUM_WEN_MASK)
#define BLEDP_DF_ANTENNA_CTRL_BUFFER_BP_MASK     (0x10000U)
#define BLEDP_DF_ANTENNA_CTRL_BUFFER_BP_SHIFT    (16U)
#define BLEDP_DF_ANTENNA_CTRL_BUFFER_BP(x)       (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_BUFFER_BP_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_BUFFER_BP_MASK)
#define BLEDP_DF_ANTENNA_CTRL_TEST_TD_POWER_MASK (0x20000U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_TD_POWER_SHIFT (17U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_TD_POWER(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_TEST_TD_POWER_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_TEST_TD_POWER_MASK)
#define BLEDP_DF_ANTENNA_CTRL_TEST_FD_POWER_MASK (0x40000U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_FD_POWER_SHIFT (18U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_FD_POWER(x)   (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_TEST_FD_POWER_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_TEST_FD_POWER_MASK)
#define BLEDP_DF_ANTENNA_CTRL_TEST_SYNC_POWER_MASK (0x80000U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_SYNC_POWER_SHIFT (19U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_SYNC_POWER(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_TEST_SYNC_POWER_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_TEST_SYNC_POWER_MASK)
#define BLEDP_DF_ANTENNA_CTRL_TEST_RFE_CORDIC_POWER_MASK (0x100000U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_RFE_CORDIC_POWER_SHIFT (20U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_RFE_CORDIC_POWER(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_TEST_RFE_CORDIC_POWER_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_TEST_RFE_CORDIC_POWER_MASK)
#define BLEDP_DF_ANTENNA_CTRL_TEST_RFE_POWER_MASK (0x200000U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_RFE_POWER_SHIFT (21U)
#define BLEDP_DF_ANTENNA_CTRL_TEST_RFE_POWER(x)  (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_TEST_RFE_POWER_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_TEST_RFE_POWER_MASK)
#define BLEDP_DF_ANTENNA_CTRL_ADC01_SAMPLE_TIME_MASK (0x400000U)
#define BLEDP_DF_ANTENNA_CTRL_ADC01_SAMPLE_TIME_SHIFT (22U)
#define BLEDP_DF_ANTENNA_CTRL_ADC01_SAMPLE_TIME(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_ADC01_SAMPLE_TIME_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_ADC01_SAMPLE_TIME_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_MUX_MASK  (0x800000U)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_MUX_SHIFT (23U)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_MUX(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PHY_RATE_MUX_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PHY_RATE_MUX_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_REG_MASK  (0x1000000U)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_REG_SHIFT (24U)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_REG(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PHY_RATE_REG_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PHY_RATE_REG_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_WEN_MASK  (0x2000000U)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_WEN_SHIFT (25U)
#define BLEDP_DF_ANTENNA_CTRL_PHY_RATE_WEN(x)    (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PHY_RATE_WEN_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PHY_RATE_WEN_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WAIT_TIME_MASK (0x4000000U)
#define BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WAIT_TIME_SHIFT (26U)
#define BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WAIT_TIME(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WAIT_TIME_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WAIT_TIME_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WIN_LEN_MASK (0x8000000U)
#define BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WIN_LEN_SHIFT (27U)
#define BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WIN_LEN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WIN_LEN_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PDU_RSSI_WIN_LEN_MASK)
#define BLEDP_DF_ANTENNA_CTRL_CAL_PDU_RSSI_EN_MASK (0x10000000U)
#define BLEDP_DF_ANTENNA_CTRL_CAL_PDU_RSSI_EN_SHIFT (28U)
#define BLEDP_DF_ANTENNA_CTRL_CAL_PDU_RSSI_EN(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_CAL_PDU_RSSI_EN_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_CAL_PDU_RSSI_EN_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PROP_CRC_AA_DIS_MASK (0x20000000U)
#define BLEDP_DF_ANTENNA_CTRL_PROP_CRC_AA_DIS_SHIFT (29U)
#define BLEDP_DF_ANTENNA_CTRL_PROP_CRC_AA_DIS(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PROP_CRC_AA_DIS_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PROP_CRC_AA_DIS_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PROP_AA_LSB_FIRST_MASK (0x40000000U)
#define BLEDP_DF_ANTENNA_CTRL_PROP_AA_LSB_FIRST_SHIFT (30U)
#define BLEDP_DF_ANTENNA_CTRL_PROP_AA_LSB_FIRST(x) (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PROP_AA_LSB_FIRST_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PROP_AA_LSB_FIRST_MASK)
#define BLEDP_DF_ANTENNA_CTRL_PRE_NUM_WEN_MASK   (0x80000000U)
#define BLEDP_DF_ANTENNA_CTRL_PRE_NUM_WEN_SHIFT  (31U)
#define BLEDP_DF_ANTENNA_CTRL_PRE_NUM_WEN(x)     (((uint32_t)(((uint32_t)(x)) << BLEDP_DF_ANTENNA_CTRL_PRE_NUM_WEN_SHIFT)) & BLEDP_DF_ANTENNA_CTRL_PRE_NUM_WEN_MASK)

/*! @name ANTENNA_MAP01 - antenna switch map register 0 */
#define BLEDP_ANTENNA_MAP01_SWITCH_MAP_1_MASK    (0x3FFFU)
#define BLEDP_ANTENNA_MAP01_SWITCH_MAP_1_SHIFT   (0U)
#define BLEDP_ANTENNA_MAP01_SWITCH_MAP_1(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP01_SWITCH_MAP_1_SHIFT)) & BLEDP_ANTENNA_MAP01_SWITCH_MAP_1_MASK)
#define BLEDP_ANTENNA_MAP01_SWITCH_MAP_0_MASK    (0x3FFF0000U)
#define BLEDP_ANTENNA_MAP01_SWITCH_MAP_0_SHIFT   (16U)
#define BLEDP_ANTENNA_MAP01_SWITCH_MAP_0(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP01_SWITCH_MAP_0_SHIFT)) & BLEDP_ANTENNA_MAP01_SWITCH_MAP_0_MASK)

/*! @name ANTENNA_MAP23 - antenna switch map register 1 */
#define BLEDP_ANTENNA_MAP23_SWITCH_MAP_3_MASK    (0x3FFFU)
#define BLEDP_ANTENNA_MAP23_SWITCH_MAP_3_SHIFT   (0U)
#define BLEDP_ANTENNA_MAP23_SWITCH_MAP_3(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP23_SWITCH_MAP_3_SHIFT)) & BLEDP_ANTENNA_MAP23_SWITCH_MAP_3_MASK)
#define BLEDP_ANTENNA_MAP23_SWITCH_MAP_2_MASK    (0x3FFF0000U)
#define BLEDP_ANTENNA_MAP23_SWITCH_MAP_2_SHIFT   (16U)
#define BLEDP_ANTENNA_MAP23_SWITCH_MAP_2(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP23_SWITCH_MAP_2_SHIFT)) & BLEDP_ANTENNA_MAP23_SWITCH_MAP_2_MASK)

/*! @name ANTENNA_MAP45 - antenna switch map register 2 */
#define BLEDP_ANTENNA_MAP45_SWITCH_MAP_5_MASK    (0x3FFFU)
#define BLEDP_ANTENNA_MAP45_SWITCH_MAP_5_SHIFT   (0U)
#define BLEDP_ANTENNA_MAP45_SWITCH_MAP_5(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP45_SWITCH_MAP_5_SHIFT)) & BLEDP_ANTENNA_MAP45_SWITCH_MAP_5_MASK)
#define BLEDP_ANTENNA_MAP45_SWITCH_MAP_4_MASK    (0x3FFF0000U)
#define BLEDP_ANTENNA_MAP45_SWITCH_MAP_4_SHIFT   (16U)
#define BLEDP_ANTENNA_MAP45_SWITCH_MAP_4(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP45_SWITCH_MAP_4_SHIFT)) & BLEDP_ANTENNA_MAP45_SWITCH_MAP_4_MASK)

/*! @name ANTENNA_MAP67 - antenna switch map register 3 */
#define BLEDP_ANTENNA_MAP67_SWITCH_MAP_7_MASK    (0x3FFFU)
#define BLEDP_ANTENNA_MAP67_SWITCH_MAP_7_SHIFT   (0U)
#define BLEDP_ANTENNA_MAP67_SWITCH_MAP_7(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP67_SWITCH_MAP_7_SHIFT)) & BLEDP_ANTENNA_MAP67_SWITCH_MAP_7_MASK)
#define BLEDP_ANTENNA_MAP67_SWITCH_MAP_6_MASK    (0x3FFF0000U)
#define BLEDP_ANTENNA_MAP67_SWITCH_MAP_6_SHIFT   (16U)
#define BLEDP_ANTENNA_MAP67_SWITCH_MAP_6(x)      (((uint32_t)(((uint32_t)(x)) << BLEDP_ANTENNA_MAP67_SWITCH_MAP_6_SHIFT)) & BLEDP_ANTENNA_MAP67_SWITCH_MAP_6_MASK)


/*!
 * @}
 */ /* end of group BLEDP_Register_Masks */


/* BLEDP - Peripheral instance base addresses */
/** Peripheral BLEDP base address */
#define BLEDP_BASE                               (0x4000E000u)
/** Peripheral BLEDP base pointer */
#define BLEDP                                    ((BLEDP_Type *)BLEDP_BASE)
/** Array initializer of BLEDP peripheral base addresses */
#define BLEDP_BASE_ADDRS                         { BLEDP_BASE }
/** Array initializer of BLEDP peripheral base pointers */
#define BLEDP_BASE_PTRS                          { BLEDP }
/** Interrupt vectors for the BLEDP peripheral type */
#define BLEDP_IRQS                               { BLE_IRQn }

/*!
 * @}
 */ /* end of group BLEDP_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CALIB Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CALIB_Peripheral_Access_Layer CALIB Peripheral Access Layer
 * @{
 */

/** CALIB - Register Layout Typedef */
typedef struct {
  __IO uint32_t START;                             /**< calibration start register, offset: 0x0 */
  __I  uint32_t STATUS;                            /**< calibration FSM status register, offset: 0x4 */
  __I  uint32_t DC_CODE;                           /**< DC code status register, offset: 0x8 */
  __IO uint32_t DC_CFG;                            /**< DC code configured code register, offset: 0xC */
  __I  uint32_t RCO_RC_REF_OSC_CODE;               /**< RCO RC PLL48M OSC code status register, offset: 0x10 */
  __IO uint32_t RCO_RC_REF_OSC_CFG;                /**< RCO RC PLL48M OSC configured code register, offset: 0x14 */
  __I  uint32_t VCOA_KVCO2M_CODE;                  /**< reserved, offset: 0x18 */
  __IO uint32_t VCOA_KVCO2M_CFG;                   /**< reserved, offset: 0x1C */
  __I  uint32_t VCOF_KVCO_PO_CODE;                 /**< reserved, offset: 0x20 */
  __IO uint32_t VCOF_KVCO_CFG;                     /**< VCOF hop calibration bypass, offset: 0x24 */
  __I  uint32_t VCOF_KVCO_CODE;                    /**< reserved, offset: 0x28 */
  __I  uint32_t KVCO_HOP_CODE;                     /**< reserved, offset: 0x2C */
  __IO uint32_t VCOF_CNT_SLOPE;                    /**< reserved, offset: 0x30 */
  __I  uint32_t XTL_CODE;                          /**< Reserved, offset: 0x34 */
  __IO uint32_t XTL_CFG;                           /**< Reserved, offset: 0x38 */
  __IO uint32_t CAL_DLY;                           /**< hop calibration delay bypass, offset: 0x3C */
  __I  uint32_t DONE;                              /**< Reserved, offset: 0x40 */
       uint8_t RESERVED_0[956];
  __IO uint32_t RRF1;                              /**< Amplitude of LO buffer for active mixer, offset: 0x400 */
  __IO uint32_t PLL48_PPF;                         /**< reserved, offset: 0x404 */
  __IO uint32_t LO0;                               /**< reserved, offset: 0x408 */
  __IO uint32_t LO1;                               /**< Reserved, offset: 0x40C */
  __IO uint32_t PA_CTRL;                           /**< Reserved, offset: 0x410 */
       uint8_t RESERVED_1[1004];
  __IO uint32_t CTRL;                              /**< Reserved, offset: 0x800 */
  __IO uint32_t INT_RAW;                           /**< Reserved, offset: 0x804 */
  __IO uint32_t INTEN;                             /**< Reserved, offset: 0x808 */
  __I  uint32_t INT_STAT;                          /**< Reserved, offset: 0x80C */
  __IO uint32_t TIF;                               /**< reserved, offset: 0x810 */
  __I  uint32_t KVCO_MEAN;                         /**< reserved, offset: 0x814 */
  __I  uint32_t KVCO_DLT;                          /**< reserved, offset: 0x818 */
  __IO uint32_t LO_CFG;                            /**< , offset: 0x81C */
  __I  uint32_t LO_TABLE;                          /**< , offset: 0x820 */
  __I  uint32_t LO_RATIO;                          /**< , offset: 0x824 */
  __IO uint32_t VCO_MOD_CFG;                       /**< TRX 2M mode selection signal, offset: 0x828 */
  __I  uint32_t VCO_MOD_STAT;                      /**< , offset: 0x82C */
  __I  uint32_t CH_IDX;                            /**< , offset: 0x830 */
  __I  uint32_t VCOF_CNT_UP;                       /**< reserved, offset: 0x834 */
  __I  uint32_t VCOF_CNT_DN;                       /**< reserved, offset: 0x838 */
} CALIB_Type;

/* ----------------------------------------------------------------------------
   -- CALIB Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CALIB_Register_Masks CALIB Register Masks
 * @{
 */

/*! @name START - calibration start register */
#define CALIB_START_PO_CLB_START_MASK            (0x1U)
#define CALIB_START_PO_CLB_START_SHIFT           (0U)
#define CALIB_START_PO_CLB_START(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_START_PO_CLB_START_SHIFT)) & CALIB_START_PO_CLB_START_MASK)
#define CALIB_START_HOP_CLB_START_MASK           (0x2U)
#define CALIB_START_HOP_CLB_START_SHIFT          (1U)
#define CALIB_START_HOP_CLB_START(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_START_HOP_CLB_START_SHIFT)) & CALIB_START_HOP_CLB_START_MASK)
#define CALIB_START_OSC_CLB_START_MASK           (0x4U)
#define CALIB_START_OSC_CLB_START_SHIFT          (2U)
#define CALIB_START_OSC_CLB_START(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_START_OSC_CLB_START_SHIFT)) & CALIB_START_OSC_CLB_START_MASK)
#define CALIB_START_REF_CLB_START_MASK           (0x8U)
#define CALIB_START_REF_CLB_START_SHIFT          (3U)
#define CALIB_START_REF_CLB_START(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_START_REF_CLB_START_SHIFT)) & CALIB_START_REF_CLB_START_MASK)
#define CALIB_START_RCO_CLB_START_MASK           (0x10U)
#define CALIB_START_RCO_CLB_START_SHIFT          (4U)
#define CALIB_START_RCO_CLB_START(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_START_RCO_CLB_START_SHIFT)) & CALIB_START_RCO_CLB_START_MASK)
#define CALIB_START_XTL_CLB_START_MASK           (0x20U)
#define CALIB_START_XTL_CLB_START_SHIFT          (5U)
#define CALIB_START_XTL_CLB_START(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_START_XTL_CLB_START_SHIFT)) & CALIB_START_XTL_CLB_START_MASK)

/*! @name STATUS - calibration FSM status register */
#define CALIB_STATUS_TOP_FSM_MASK                (0x1FU)
#define CALIB_STATUS_TOP_FSM_SHIFT               (0U)
#define CALIB_STATUS_TOP_FSM(x)                  (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_TOP_FSM_SHIFT)) & CALIB_STATUS_TOP_FSM_MASK)
#define CALIB_STATUS_DC_FSM_MASK                 (0x1E0U)
#define CALIB_STATUS_DC_FSM_SHIFT                (5U)
#define CALIB_STATUS_DC_FSM(x)                   (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_DC_FSM_SHIFT)) & CALIB_STATUS_DC_FSM_MASK)
#define CALIB_STATUS_VCOA_FSM_MASK               (0xE00U)
#define CALIB_STATUS_VCOA_FSM_SHIFT              (9U)
#define CALIB_STATUS_VCOA_FSM(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_VCOA_FSM_SHIFT)) & CALIB_STATUS_VCOA_FSM_MASK)
#define CALIB_STATUS_VCOF_FSM_MASK               (0x1F000U)
#define CALIB_STATUS_VCOF_FSM_SHIFT              (12U)
#define CALIB_STATUS_VCOF_FSM(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_VCOF_FSM_SHIFT)) & CALIB_STATUS_VCOF_FSM_MASK)
#define CALIB_STATUS_KVCO_FSM_MASK               (0x1E0000U)
#define CALIB_STATUS_KVCO_FSM_SHIFT              (17U)
#define CALIB_STATUS_KVCO_FSM(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_KVCO_FSM_SHIFT)) & CALIB_STATUS_KVCO_FSM_MASK)
#define CALIB_STATUS_RCO_FSM_MASK                (0xE00000U)
#define CALIB_STATUS_RCO_FSM_SHIFT               (21U)
#define CALIB_STATUS_RCO_FSM(x)                  (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_RCO_FSM_SHIFT)) & CALIB_STATUS_RCO_FSM_MASK)
#define CALIB_STATUS_OSC_FSM_MASK                (0x7000000U)
#define CALIB_STATUS_OSC_FSM_SHIFT               (24U)
#define CALIB_STATUS_OSC_FSM(x)                  (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_OSC_FSM_SHIFT)) & CALIB_STATUS_OSC_FSM_MASK)
#define CALIB_STATUS_REF_FSM_MASK                (0x38000000U)
#define CALIB_STATUS_REF_FSM_SHIFT               (27U)
#define CALIB_STATUS_REF_FSM(x)                  (((uint32_t)(((uint32_t)(x)) << CALIB_STATUS_REF_FSM_SHIFT)) & CALIB_STATUS_REF_FSM_MASK)

/*! @name DC_CODE - DC code status register */
#define CALIB_DC_CODE_PPF_DCCAL2_I_MASK          (0xFU)
#define CALIB_DC_CODE_PPF_DCCAL2_I_SHIFT         (0U)
#define CALIB_DC_CODE_PPF_DCCAL2_I(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CODE_PPF_DCCAL2_I_SHIFT)) & CALIB_DC_CODE_PPF_DCCAL2_I_MASK)
#define CALIB_DC_CODE_PPF_DCCAL2_Q_MASK          (0xF0U)
#define CALIB_DC_CODE_PPF_DCCAL2_Q_SHIFT         (4U)
#define CALIB_DC_CODE_PPF_DCCAL2_Q(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CODE_PPF_DCCAL2_Q_SHIFT)) & CALIB_DC_CODE_PPF_DCCAL2_Q_MASK)
#define CALIB_DC_CODE_PPF_DCCAL_I_MASK           (0x3F0000U)
#define CALIB_DC_CODE_PPF_DCCAL_I_SHIFT          (16U)
#define CALIB_DC_CODE_PPF_DCCAL_I(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CODE_PPF_DCCAL_I_SHIFT)) & CALIB_DC_CODE_PPF_DCCAL_I_MASK)
#define CALIB_DC_CODE_PPF_DCCAL_Q_MASK           (0x3F000000U)
#define CALIB_DC_CODE_PPF_DCCAL_Q_SHIFT          (24U)
#define CALIB_DC_CODE_PPF_DCCAL_Q(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CODE_PPF_DCCAL_Q_SHIFT)) & CALIB_DC_CODE_PPF_DCCAL_Q_MASK)

/*! @name DC_CFG - DC code configured code register */
#define CALIB_DC_CFG_PPF_DCCAL2_CFG_I_MASK       (0xFU)
#define CALIB_DC_CFG_PPF_DCCAL2_CFG_I_SHIFT      (0U)
#define CALIB_DC_CFG_PPF_DCCAL2_CFG_I(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_PPF_DCCAL2_CFG_I_SHIFT)) & CALIB_DC_CFG_PPF_DCCAL2_CFG_I_MASK)
#define CALIB_DC_CFG_PPF_DCCAL2_CFG_Q_MASK       (0xF0U)
#define CALIB_DC_CFG_PPF_DCCAL2_CFG_Q_SHIFT      (4U)
#define CALIB_DC_CFG_PPF_DCCAL2_CFG_Q(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_PPF_DCCAL2_CFG_Q_SHIFT)) & CALIB_DC_CFG_PPF_DCCAL2_CFG_Q_MASK)
#define CALIB_DC_CFG_DC_2NDCAL_DIS_MASK          (0x100U)
#define CALIB_DC_CFG_DC_2NDCAL_DIS_SHIFT         (8U)
#define CALIB_DC_CFG_DC_2NDCAL_DIS(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_DC_2NDCAL_DIS_SHIFT)) & CALIB_DC_CFG_DC_2NDCAL_DIS_MASK)
#define CALIB_DC_CFG_DC_2NDCAL_REQ_MASK          (0x200U)
#define CALIB_DC_CFG_DC_2NDCAL_REQ_SHIFT         (9U)
#define CALIB_DC_CFG_DC_2NDCAL_REQ(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_DC_2NDCAL_REQ_SHIFT)) & CALIB_DC_CFG_DC_2NDCAL_REQ_MASK)
#define CALIB_DC_CFG_PPF_DCCAL_CFG_I_MASK        (0x3F0000U)
#define CALIB_DC_CFG_PPF_DCCAL_CFG_I_SHIFT       (16U)
#define CALIB_DC_CFG_PPF_DCCAL_CFG_I(x)          (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_PPF_DCCAL_CFG_I_SHIFT)) & CALIB_DC_CFG_PPF_DCCAL_CFG_I_MASK)
#define CALIB_DC_CFG_DC_HOP_CAL_BP_MASK          (0x400000U)
#define CALIB_DC_CFG_DC_HOP_CAL_BP_SHIFT         (22U)
#define CALIB_DC_CFG_DC_HOP_CAL_BP(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_DC_HOP_CAL_BP_SHIFT)) & CALIB_DC_CFG_DC_HOP_CAL_BP_MASK)
#define CALIB_DC_CFG_PPF_DCCAL_CFG_Q_MASK        (0x3F000000U)
#define CALIB_DC_CFG_PPF_DCCAL_CFG_Q_SHIFT       (24U)
#define CALIB_DC_CFG_PPF_DCCAL_CFG_Q(x)          (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_PPF_DCCAL_CFG_Q_SHIFT)) & CALIB_DC_CFG_PPF_DCCAL_CFG_Q_MASK)
#define CALIB_DC_CFG_DC_1STCAL_DIS_MASK          (0x40000000U)
#define CALIB_DC_CFG_DC_1STCAL_DIS_SHIFT         (30U)
#define CALIB_DC_CFG_DC_1STCAL_DIS(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_DC_1STCAL_DIS_SHIFT)) & CALIB_DC_CFG_DC_1STCAL_DIS_MASK)
#define CALIB_DC_CFG_DC_1STCAL_REQ_MASK          (0x80000000U)
#define CALIB_DC_CFG_DC_1STCAL_REQ_SHIFT         (31U)
#define CALIB_DC_CFG_DC_1STCAL_REQ(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_DC_CFG_DC_1STCAL_REQ_SHIFT)) & CALIB_DC_CFG_DC_1STCAL_REQ_MASK)

/*! @name RCO_RC_REF_OSC_CODE - RCO RC PLL48M OSC code status register */
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_RCO_CAP_MASK (0xFU)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_RCO_CAP_SHIFT (0U)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_RCO_CAP(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CODE_CAU_RCO_CAP_SHIFT)) & CALIB_RCO_RC_REF_OSC_CODE_CAU_RCO_CAP_MASK)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_OSC_CUR_MASK (0x1F00U)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_OSC_CUR_SHIFT (8U)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_OSC_CUR(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CODE_CAU_OSC_CUR_SHIFT)) & CALIB_RCO_RC_REF_OSC_CODE_CAU_OSC_CUR_MASK)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_RC_CAL_OUT2REG_MASK (0xF0000U)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_RC_CAL_OUT2REG_SHIFT (16U)
#define CALIB_RCO_RC_REF_OSC_CODE_CAU_RC_CAL_OUT2REG(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CODE_CAU_RC_CAL_OUT2REG_SHIFT)) & CALIB_RCO_RC_REF_OSC_CODE_CAU_RC_CAL_OUT2REG_MASK)
#define CALIB_RCO_RC_REF_OSC_CODE_PLL48_ENREF_MASK (0xF000000U)
#define CALIB_RCO_RC_REF_OSC_CODE_PLL48_ENREF_SHIFT (24U)
#define CALIB_RCO_RC_REF_OSC_CODE_PLL48_ENREF(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CODE_PLL48_ENREF_SHIFT)) & CALIB_RCO_RC_REF_OSC_CODE_PLL48_ENREF_MASK)

/*! @name RCO_RC_REF_OSC_CFG - RCO RC PLL48M OSC configured code register */
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG_MASK (0xFU)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG_SHIFT (0U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_DIS_MASK (0x10U)
#define CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_DIS_SHIFT (4U)
#define CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_DIS(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_DIS_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_DIS_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_REQ_MASK (0x20U)
#define CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_REQ_SHIFT (5U)
#define CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_REQ(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_REQ_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_RCO_CAL_REQ_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_OSC_CUR_CFG_MASK (0x1F00U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_OSC_CUR_CFG_SHIFT (8U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_OSC_CUR_CFG(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_CAU_OSC_CUR_CFG_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_CAU_OSC_CUR_CFG_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_DIS_MASK (0x2000U)
#define CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_DIS_SHIFT (13U)
#define CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_DIS(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_DIS_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_DIS_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_REQ_MASK (0x4000U)
#define CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_REQ_SHIFT (14U)
#define CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_REQ(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_REQ_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_OSC_CAL_REQ_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_REG_IN_MASK (0xF0000U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_REG_IN_SHIFT (16U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_REG_IN(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_REG_IN_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_REG_IN_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_DIS_MASK (0x100000U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_DIS_SHIFT (20U)
#define CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_DIS(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_DIS_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_CAU_RC_CAL_DIS_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_RC_CAL_REQ_MASK (0x200000U)
#define CALIB_RCO_RC_REF_OSC_CFG_RC_CAL_REQ_SHIFT (21U)
#define CALIB_RCO_RC_REF_OSC_CFG_RC_CAL_REQ(x)   (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_RC_CAL_REQ_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_RC_CAL_REQ_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_PLL48_ENREF_CFG_MASK (0xF000000U)
#define CALIB_RCO_RC_REF_OSC_CFG_PLL48_ENREF_CFG_SHIFT (24U)
#define CALIB_RCO_RC_REF_OSC_CFG_PLL48_ENREF_CFG(x) (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_PLL48_ENREF_CFG_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_PLL48_ENREF_CFG_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_DIS_MASK (0x10000000U)
#define CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_DIS_SHIFT (28U)
#define CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_DIS(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_DIS_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_DIS_MASK)
#define CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_REQ_MASK (0x20000000U)
#define CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_REQ_SHIFT (29U)
#define CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_REQ(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_REQ_SHIFT)) & CALIB_RCO_RC_REF_OSC_CFG_REF_CAL_REQ_MASK)

/*! @name VCOA_KVCO2M_CODE - reserved */
#define CALIB_VCOA_KVCO2M_CODE_KCALF2M_PO_MASK   (0x7FFU)
#define CALIB_VCOA_KVCO2M_CODE_KCALF2M_PO_SHIFT  (0U)
#define CALIB_VCOA_KVCO2M_CODE_KCALF2M_PO(x)     (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CODE_KCALF2M_PO_SHIFT)) & CALIB_VCOA_KVCO2M_CODE_KCALF2M_PO_MASK)
#define CALIB_VCOA_KVCO2M_CODE_TX_VCO_AMP_MASK   (0x1F0000U)
#define CALIB_VCOA_KVCO2M_CODE_TX_VCO_AMP_SHIFT  (16U)
#define CALIB_VCOA_KVCO2M_CODE_TX_VCO_AMP(x)     (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CODE_TX_VCO_AMP_SHIFT)) & CALIB_VCOA_KVCO2M_CODE_TX_VCO_AMP_MASK)
#define CALIB_VCOA_KVCO2M_CODE_RX_VCO_AMP_MASK   (0x1F000000U)
#define CALIB_VCOA_KVCO2M_CODE_RX_VCO_AMP_SHIFT  (24U)
#define CALIB_VCOA_KVCO2M_CODE_RX_VCO_AMP(x)     (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CODE_RX_VCO_AMP_SHIFT)) & CALIB_VCOA_KVCO2M_CODE_RX_VCO_AMP_MASK)

/*! @name VCOA_KVCO2M_CFG - reserved */
#define CALIB_VCOA_KVCO2M_CFG_KCALF2M_CFG_MASK   (0x7FFU)
#define CALIB_VCOA_KVCO2M_CFG_KCALF2M_CFG_SHIFT  (0U)
#define CALIB_VCOA_KVCO2M_CFG_KCALF2M_CFG(x)     (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_KCALF2M_CFG_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_KCALF2M_CFG_MASK)
#define CALIB_VCOA_KVCO2M_CFG_KCALF2M_BP_MASK    (0x800U)
#define CALIB_VCOA_KVCO2M_CFG_KCALF2M_BP_SHIFT   (11U)
#define CALIB_VCOA_KVCO2M_CFG_KCALF2M_BP(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_KCALF2M_BP_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_KCALF2M_BP_MASK)
#define CALIB_VCOA_KVCO2M_CFG_KVCO_CAL_E_MASK    (0x7000U)
#define CALIB_VCOA_KVCO2M_CFG_KVCO_CAL_E_SHIFT   (12U)
#define CALIB_VCOA_KVCO2M_CFG_KVCO_CAL_E(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_KVCO_CAL_E_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_KVCO_CAL_E_MASK)
#define CALIB_VCOA_KVCO2M_CFG_TX_VCO_AMP_CFG_MASK (0x1F0000U)
#define CALIB_VCOA_KVCO2M_CFG_TX_VCO_AMP_CFG_SHIFT (16U)
#define CALIB_VCOA_KVCO2M_CFG_TX_VCO_AMP_CFG(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_TX_VCO_AMP_CFG_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_TX_VCO_AMP_CFG_MASK)
#define CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_DIS_MASK  (0x200000U)
#define CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_DIS_SHIFT (21U)
#define CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_DIS(x)    (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_DIS_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_DIS_MASK)
#define CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_REQ_MASK  (0x400000U)
#define CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_REQ_SHIFT (22U)
#define CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_REQ(x)    (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_REQ_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_VCOA_CAL_REQ_MASK)
#define CALIB_VCOA_KVCO2M_CFG_RX_VCO_AMP_CFG_MASK (0x1F000000U)
#define CALIB_VCOA_KVCO2M_CFG_RX_VCO_AMP_CFG_SHIFT (24U)
#define CALIB_VCOA_KVCO2M_CFG_RX_VCO_AMP_CFG(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_VCOA_KVCO2M_CFG_RX_VCO_AMP_CFG_SHIFT)) & CALIB_VCOA_KVCO2M_CFG_RX_VCO_AMP_CFG_MASK)

/*! @name VCOF_KVCO_PO_CODE - reserved */
#define CALIB_VCOF_KVCO_PO_CODE_KCALF_PO_MASK    (0x7FFU)
#define CALIB_VCOF_KVCO_PO_CODE_KCALF_PO_SHIFT   (0U)
#define CALIB_VCOF_KVCO_PO_CODE_KCALF_PO(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_PO_CODE_KCALF_PO_SHIFT)) & CALIB_VCOF_KVCO_PO_CODE_KCALF_PO_MASK)
#define CALIB_VCOF_KVCO_PO_CODE_TX_VCO_CBANK_PO_MASK (0x3F0000U)
#define CALIB_VCOF_KVCO_PO_CODE_TX_VCO_CBANK_PO_SHIFT (16U)
#define CALIB_VCOF_KVCO_PO_CODE_TX_VCO_CBANK_PO(x) (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_PO_CODE_TX_VCO_CBANK_PO_SHIFT)) & CALIB_VCOF_KVCO_PO_CODE_TX_VCO_CBANK_PO_MASK)
#define CALIB_VCOF_KVCO_PO_CODE_RX_VCO_CBANK_PO_MASK (0x3F000000U)
#define CALIB_VCOF_KVCO_PO_CODE_RX_VCO_CBANK_PO_SHIFT (24U)
#define CALIB_VCOF_KVCO_PO_CODE_RX_VCO_CBANK_PO(x) (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_PO_CODE_RX_VCO_CBANK_PO_SHIFT)) & CALIB_VCOF_KVCO_PO_CODE_RX_VCO_CBANK_PO_MASK)

/*! @name VCOF_KVCO_CFG - VCOF hop calibration bypass */
#define CALIB_VCOF_KVCO_CFG_KCALF_CFG_MASK       (0x7FFU)
#define CALIB_VCOF_KVCO_CFG_KCALF_CFG_SHIFT      (0U)
#define CALIB_VCOF_KVCO_CFG_KCALF_CFG(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_KCALF_CFG_SHIFT)) & CALIB_VCOF_KVCO_CFG_KCALF_CFG_MASK)
#define CALIB_VCOF_KVCO_CFG_KVCO_REQ_MASK        (0x800U)
#define CALIB_VCOF_KVCO_CFG_KVCO_REQ_SHIFT       (11U)
#define CALIB_VCOF_KVCO_CFG_KVCO_REQ(x)          (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_KVCO_REQ_SHIFT)) & CALIB_VCOF_KVCO_CFG_KVCO_REQ_MASK)
#define CALIB_VCOF_KVCO_CFG_KVCO_DIS_MASK        (0x1000U)
#define CALIB_VCOF_KVCO_CFG_KVCO_DIS_SHIFT       (12U)
#define CALIB_VCOF_KVCO_CFG_KVCO_DIS(x)          (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_KVCO_DIS_SHIFT)) & CALIB_VCOF_KVCO_CFG_KVCO_DIS_MASK)
#define CALIB_VCOF_KVCO_CFG_KVCO_SKIP_MASK       (0x2000U)
#define CALIB_VCOF_KVCO_CFG_KVCO_SKIP_SHIFT      (13U)
#define CALIB_VCOF_KVCO_CFG_KVCO_SKIP(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_KVCO_SKIP_SHIFT)) & CALIB_VCOF_KVCO_CFG_KVCO_SKIP_MASK)
#define CALIB_VCOF_KVCO_CFG_TX_VCO_CBANK_CFG_MASK (0x3F0000U)
#define CALIB_VCOF_KVCO_CFG_TX_VCO_CBANK_CFG_SHIFT (16U)
#define CALIB_VCOF_KVCO_CFG_TX_VCO_CBANK_CFG(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_TX_VCO_CBANK_CFG_SHIFT)) & CALIB_VCOF_KVCO_CFG_TX_VCO_CBANK_CFG_MASK)
#define CALIB_VCOF_KVCO_CFG_VCOF_CAL_DIS_MASK    (0x400000U)
#define CALIB_VCOF_KVCO_CFG_VCOF_CAL_DIS_SHIFT   (22U)
#define CALIB_VCOF_KVCO_CFG_VCOF_CAL_DIS(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_VCOF_CAL_DIS_SHIFT)) & CALIB_VCOF_KVCO_CFG_VCOF_CAL_DIS_MASK)
#define CALIB_VCOF_KVCO_CFG_VCOF_CAL_REQ_MASK    (0x800000U)
#define CALIB_VCOF_KVCO_CFG_VCOF_CAL_REQ_SHIFT   (23U)
#define CALIB_VCOF_KVCO_CFG_VCOF_CAL_REQ(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_VCOF_CAL_REQ_SHIFT)) & CALIB_VCOF_KVCO_CFG_VCOF_CAL_REQ_MASK)
#define CALIB_VCOF_KVCO_CFG_RX_VCO_CBANK_CFG_MASK (0x3F000000U)
#define CALIB_VCOF_KVCO_CFG_RX_VCO_CBANK_CFG_SHIFT (24U)
#define CALIB_VCOF_KVCO_CFG_RX_VCO_CBANK_CFG(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_RX_VCO_CBANK_CFG_SHIFT)) & CALIB_VCOF_KVCO_CFG_RX_VCO_CBANK_CFG_MASK)
#define CALIB_VCOF_KVCO_CFG_VCOF_SKIP_MASK       (0x40000000U)
#define CALIB_VCOF_KVCO_CFG_VCOF_SKIP_SHIFT      (30U)
#define CALIB_VCOF_KVCO_CFG_VCOF_SKIP(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_VCOF_SKIP_SHIFT)) & CALIB_VCOF_KVCO_CFG_VCOF_SKIP_MASK)
#define CALIB_VCOF_KVCO_CFG_VCOF_HOP_BP_MASK     (0x80000000U)
#define CALIB_VCOF_KVCO_CFG_VCOF_HOP_BP_SHIFT    (31U)
#define CALIB_VCOF_KVCO_CFG_VCOF_HOP_BP(x)       (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CFG_VCOF_HOP_BP_SHIFT)) & CALIB_VCOF_KVCO_CFG_VCOF_HOP_BP_MASK)

/*! @name VCOF_KVCO_CODE - reserved */
#define CALIB_VCOF_KVCO_CODE_KCALF_MASK          (0x7FFU)
#define CALIB_VCOF_KVCO_CODE_KCALF_SHIFT         (0U)
#define CALIB_VCOF_KVCO_CODE_KCALF(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CODE_KCALF_SHIFT)) & CALIB_VCOF_KVCO_CODE_KCALF_MASK)
#define CALIB_VCOF_KVCO_CODE_TX_VCO_CBANK_MASK   (0x3F0000U)
#define CALIB_VCOF_KVCO_CODE_TX_VCO_CBANK_SHIFT  (16U)
#define CALIB_VCOF_KVCO_CODE_TX_VCO_CBANK(x)     (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CODE_TX_VCO_CBANK_SHIFT)) & CALIB_VCOF_KVCO_CODE_TX_VCO_CBANK_MASK)
#define CALIB_VCOF_KVCO_CODE_RX_VCO_CBANK_MASK   (0x3F000000U)
#define CALIB_VCOF_KVCO_CODE_RX_VCO_CBANK_SHIFT  (24U)
#define CALIB_VCOF_KVCO_CODE_RX_VCO_CBANK(x)     (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_KVCO_CODE_RX_VCO_CBANK_SHIFT)) & CALIB_VCOF_KVCO_CODE_RX_VCO_CBANK_MASK)

/*! @name KVCO_HOP_CODE - reserved */
#define CALIB_KVCO_HOP_CODE_KCALF1M_MASK         (0x7FFU)
#define CALIB_KVCO_HOP_CODE_KCALF1M_SHIFT        (0U)
#define CALIB_KVCO_HOP_CODE_KCALF1M(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_KVCO_HOP_CODE_KCALF1M_SHIFT)) & CALIB_KVCO_HOP_CODE_KCALF1M_MASK)
#define CALIB_KVCO_HOP_CODE_KCALF2M_MASK         (0x7FF0000U)
#define CALIB_KVCO_HOP_CODE_KCALF2M_SHIFT        (16U)
#define CALIB_KVCO_HOP_CODE_KCALF2M(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_KVCO_HOP_CODE_KCALF2M_SHIFT)) & CALIB_KVCO_HOP_CODE_KCALF2M_MASK)

/*! @name VCOF_CNT_SLOPE - reserved */
#define CALIB_VCOF_CNT_SLOPE_TX_VCOF_CNT_MASK    (0xFFU)
#define CALIB_VCOF_CNT_SLOPE_TX_VCOF_CNT_SHIFT   (0U)
#define CALIB_VCOF_CNT_SLOPE_TX_VCOF_CNT(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_SLOPE_TX_VCOF_CNT_SHIFT)) & CALIB_VCOF_CNT_SLOPE_TX_VCOF_CNT_MASK)
#define CALIB_VCOF_CNT_SLOPE_TX_SLOPE_MASK       (0x3F00U)
#define CALIB_VCOF_CNT_SLOPE_TX_SLOPE_SHIFT      (8U)
#define CALIB_VCOF_CNT_SLOPE_TX_SLOPE(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_SLOPE_TX_SLOPE_SHIFT)) & CALIB_VCOF_CNT_SLOPE_TX_SLOPE_MASK)
#define CALIB_VCOF_CNT_SLOPE_RX_VCOF_CNT_MASK    (0xFF0000U)
#define CALIB_VCOF_CNT_SLOPE_RX_VCOF_CNT_SHIFT   (16U)
#define CALIB_VCOF_CNT_SLOPE_RX_VCOF_CNT(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_SLOPE_RX_VCOF_CNT_SHIFT)) & CALIB_VCOF_CNT_SLOPE_RX_VCOF_CNT_MASK)
#define CALIB_VCOF_CNT_SLOPE_RX_SLOPE_MASK       (0x3F000000U)
#define CALIB_VCOF_CNT_SLOPE_RX_SLOPE_SHIFT      (24U)
#define CALIB_VCOF_CNT_SLOPE_RX_SLOPE(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_SLOPE_RX_SLOPE_SHIFT)) & CALIB_VCOF_CNT_SLOPE_RX_SLOPE_MASK)

/*! @name XTL_CODE - Reserved */
#define CALIB_XTL_CODE_XTL_XICTRL_CODE_MASK      (0x3FU)
#define CALIB_XTL_CODE_XTL_XICTRL_CODE_SHIFT     (0U)
#define CALIB_XTL_CODE_XTL_XICTRL_CODE(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_XTL_CODE_XTL_XICTRL_CODE_SHIFT)) & CALIB_XTL_CODE_XTL_XICTRL_CODE_MASK)
#define CALIB_XTL_CODE_XTL_AMP_DET_OUT_MASK      (0x100U)
#define CALIB_XTL_CODE_XTL_AMP_DET_OUT_SHIFT     (8U)
#define CALIB_XTL_CODE_XTL_AMP_DET_OUT(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_XTL_CODE_XTL_AMP_DET_OUT_SHIFT)) & CALIB_XTL_CODE_XTL_AMP_DET_OUT_MASK)

/*! @name XTL_CFG - Reserved */
#define CALIB_XTL_CFG_XTL_XICTRL_CFG_MASK        (0x3FU)
#define CALIB_XTL_CFG_XTL_XICTRL_CFG_SHIFT       (0U)
#define CALIB_XTL_CFG_XTL_XICTRL_CFG(x)          (((uint32_t)(((uint32_t)(x)) << CALIB_XTL_CFG_XTL_XICTRL_CFG_SHIFT)) & CALIB_XTL_CFG_XTL_XICTRL_CFG_MASK)
#define CALIB_XTL_CFG_XTL_CAL_DIS_MASK           (0x40U)
#define CALIB_XTL_CFG_XTL_CAL_DIS_SHIFT          (6U)
#define CALIB_XTL_CFG_XTL_CAL_DIS(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_XTL_CFG_XTL_CAL_DIS_SHIFT)) & CALIB_XTL_CFG_XTL_CAL_DIS_MASK)
#define CALIB_XTL_CFG_XTL_CAL_REQ_MASK           (0x80U)
#define CALIB_XTL_CFG_XTL_CAL_REQ_SHIFT          (7U)
#define CALIB_XTL_CFG_XTL_CAL_REQ(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_XTL_CFG_XTL_CAL_REQ_SHIFT)) & CALIB_XTL_CFG_XTL_CAL_REQ_MASK)

/*! @name CAL_DLY - hop calibration delay bypass */
#define CALIB_CAL_DLY_HOP_DLY_MASK               (0x3FU)
#define CALIB_CAL_DLY_HOP_DLY_SHIFT              (0U)
#define CALIB_CAL_DLY_HOP_DLY(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_HOP_DLY_SHIFT)) & CALIB_CAL_DLY_HOP_DLY_MASK)
#define CALIB_CAL_DLY_HOP_DLY_BP_MASK            (0x80U)
#define CALIB_CAL_DLY_HOP_DLY_BP_SHIFT           (7U)
#define CALIB_CAL_DLY_HOP_DLY_BP(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_HOP_DLY_BP_SHIFT)) & CALIB_CAL_DLY_HOP_DLY_BP_MASK)
#define CALIB_CAL_DLY_TX_DLY_DIG1M_MASK          (0x300U)
#define CALIB_CAL_DLY_TX_DLY_DIG1M_SHIFT         (8U)
#define CALIB_CAL_DLY_TX_DLY_DIG1M(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_TX_DLY_DIG1M_SHIFT)) & CALIB_CAL_DLY_TX_DLY_DIG1M_MASK)
#define CALIB_CAL_DLY_TX_DLY_DIG2M_MASK          (0xC00U)
#define CALIB_CAL_DLY_TX_DLY_DIG2M_SHIFT         (10U)
#define CALIB_CAL_DLY_TX_DLY_DIG2M(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_TX_DLY_DIG2M_SHIFT)) & CALIB_CAL_DLY_TX_DLY_DIG2M_MASK)
#define CALIB_CAL_DLY_TX_DLY_DAC_1M_MASK         (0x3000U)
#define CALIB_CAL_DLY_TX_DLY_DAC_1M_SHIFT        (12U)
#define CALIB_CAL_DLY_TX_DLY_DAC_1M(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_TX_DLY_DAC_1M_SHIFT)) & CALIB_CAL_DLY_TX_DLY_DAC_1M_MASK)
#define CALIB_CAL_DLY_TX_DLY_DAC_2M_MASK         (0xC000U)
#define CALIB_CAL_DLY_TX_DLY_DAC_2M_SHIFT        (14U)
#define CALIB_CAL_DLY_TX_DLY_DAC_2M(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_TX_DLY_DAC_2M_SHIFT)) & CALIB_CAL_DLY_TX_DLY_DAC_2M_MASK)
#define CALIB_CAL_DLY_RX_PWRUP_CNT_TH1M_MASK     (0xFF0000U)
#define CALIB_CAL_DLY_RX_PWRUP_CNT_TH1M_SHIFT    (16U)
#define CALIB_CAL_DLY_RX_PWRUP_CNT_TH1M(x)       (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_RX_PWRUP_CNT_TH1M_SHIFT)) & CALIB_CAL_DLY_RX_PWRUP_CNT_TH1M_MASK)
#define CALIB_CAL_DLY_RX_PWRUP_CNT_TH2M_MASK     (0xFF000000U)
#define CALIB_CAL_DLY_RX_PWRUP_CNT_TH2M_SHIFT    (24U)
#define CALIB_CAL_DLY_RX_PWRUP_CNT_TH2M(x)       (((uint32_t)(((uint32_t)(x)) << CALIB_CAL_DLY_RX_PWRUP_CNT_TH2M_SHIFT)) & CALIB_CAL_DLY_RX_PWRUP_CNT_TH2M_MASK)

/*! @name DONE - Reserved */
#define CALIB_DONE_OSC_CAL_DONE_MASK             (0x4U)
#define CALIB_DONE_OSC_CAL_DONE_SHIFT            (2U)
#define CALIB_DONE_OSC_CAL_DONE(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_OSC_CAL_DONE_SHIFT)) & CALIB_DONE_OSC_CAL_DONE_MASK)
#define CALIB_DONE_REF_CAL_DONE_MASK             (0x8U)
#define CALIB_DONE_REF_CAL_DONE_SHIFT            (3U)
#define CALIB_DONE_REF_CAL_DONE(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_REF_CAL_DONE_SHIFT)) & CALIB_DONE_REF_CAL_DONE_MASK)
#define CALIB_DONE_RCO_CAL_DONE_MASK             (0x10U)
#define CALIB_DONE_RCO_CAL_DONE_SHIFT            (4U)
#define CALIB_DONE_RCO_CAL_DONE(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_RCO_CAL_DONE_SHIFT)) & CALIB_DONE_RCO_CAL_DONE_MASK)
#define CALIB_DONE_RC_CAL_DONE_MASK              (0x20U)
#define CALIB_DONE_RC_CAL_DONE_SHIFT             (5U)
#define CALIB_DONE_RC_CAL_DONE(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_RC_CAL_DONE_SHIFT)) & CALIB_DONE_RC_CAL_DONE_MASK)
#define CALIB_DONE_VCOF_CAL_DONE_MASK            (0x40U)
#define CALIB_DONE_VCOF_CAL_DONE_SHIFT           (6U)
#define CALIB_DONE_VCOF_CAL_DONE(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_VCOF_CAL_DONE_SHIFT)) & CALIB_DONE_VCOF_CAL_DONE_MASK)
#define CALIB_DONE_VCOA_CAL_DONE_MASK            (0x80U)
#define CALIB_DONE_VCOA_CAL_DONE_SHIFT           (7U)
#define CALIB_DONE_VCOA_CAL_DONE(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_VCOA_CAL_DONE_SHIFT)) & CALIB_DONE_VCOA_CAL_DONE_MASK)
#define CALIB_DONE_DC2ND_CAL_DONE_MASK           (0x100U)
#define CALIB_DONE_DC2ND_CAL_DONE_SHIFT          (8U)
#define CALIB_DONE_DC2ND_CAL_DONE(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_DC2ND_CAL_DONE_SHIFT)) & CALIB_DONE_DC2ND_CAL_DONE_MASK)
#define CALIB_DONE_DC1ST_CAL_DONE_MASK           (0x200U)
#define CALIB_DONE_DC1ST_CAL_DONE_SHIFT          (9U)
#define CALIB_DONE_DC1ST_CAL_DONE(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_DC1ST_CAL_DONE_SHIFT)) & CALIB_DONE_DC1ST_CAL_DONE_MASK)
#define CALIB_DONE_XTL_CAL_DONE_MASK             (0x400U)
#define CALIB_DONE_XTL_CAL_DONE_SHIFT            (10U)
#define CALIB_DONE_XTL_CAL_DONE(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_XTL_CAL_DONE_SHIFT)) & CALIB_DONE_XTL_CAL_DONE_MASK)
#define CALIB_DONE_KVCO_CAL_DONE_MASK            (0x800U)
#define CALIB_DONE_KVCO_CAL_DONE_SHIFT           (11U)
#define CALIB_DONE_KVCO_CAL_DONE(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_KVCO_CAL_DONE_SHIFT)) & CALIB_DONE_KVCO_CAL_DONE_MASK)
#define CALIB_DONE_KVCO_HOP_DONE_MASK            (0x1000U)
#define CALIB_DONE_KVCO_HOP_DONE_SHIFT           (12U)
#define CALIB_DONE_KVCO_HOP_DONE(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_DONE_KVCO_HOP_DONE_SHIFT)) & CALIB_DONE_KVCO_HOP_DONE_MASK)

/*! @name RRF1 - Amplitude of LO buffer for active mixer */
#define CALIB_RRF1_RRF_INCAP2_MASK               (0x7U)
#define CALIB_RRF1_RRF_INCAP2_SHIFT              (0U)
#define CALIB_RRF1_RRF_INCAP2(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_INCAP2_SHIFT)) & CALIB_RRF1_RRF_INCAP2_MASK)
#define CALIB_RRF1_RRF_LOAD_CAP_MASK             (0x78U)
#define CALIB_RRF1_RRF_LOAD_CAP_SHIFT            (3U)
#define CALIB_RRF1_RRF_LOAD_CAP(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_LOAD_CAP_SHIFT)) & CALIB_RRF1_RRF_LOAD_CAP_MASK)
#define CALIB_RRF1_RRF_TX_INCAP1_MASK            (0x380U)
#define CALIB_RRF1_RRF_TX_INCAP1_SHIFT           (7U)
#define CALIB_RRF1_RRF_TX_INCAP1(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_TX_INCAP1_SHIFT)) & CALIB_RRF1_RRF_TX_INCAP1_MASK)
#define CALIB_RRF1_RRF_RX_INCAP1_MASK            (0x1C00U)
#define CALIB_RRF1_RRF_RX_INCAP1_SHIFT           (10U)
#define CALIB_RRF1_RRF_RX_INCAP1(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_RX_INCAP1_SHIFT)) & CALIB_RRF1_RRF_RX_INCAP1_MASK)
#define CALIB_RRF1_RRF_VGATE11_LNA_MASK          (0xE000U)
#define CALIB_RRF1_RRF_VGATE11_LNA_SHIFT         (13U)
#define CALIB_RRF1_RRF_VGATE11_LNA(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_VGATE11_LNA_SHIFT)) & CALIB_RRF1_RRF_VGATE11_LNA_MASK)
#define CALIB_RRF1_RRF_BM_GM_MASK                (0x30000U)
#define CALIB_RRF1_RRF_BM_GM_SHIFT               (16U)
#define CALIB_RRF1_RRF_BM_GM(x)                  (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_BM_GM_SHIFT)) & CALIB_RRF1_RRF_BM_GM_MASK)
#define CALIB_RRF1_RRF_BM_LNA_MASK               (0xC0000U)
#define CALIB_RRF1_RRF_BM_LNA_SHIFT              (18U)
#define CALIB_RRF1_RRF_BM_LNA(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_BM_LNA_SHIFT)) & CALIB_RRF1_RRF_BM_LNA_MASK)
#define CALIB_RRF1_RRF_BM_MIXER_MASK             (0x300000U)
#define CALIB_RRF1_RRF_BM_MIXER_SHIFT            (20U)
#define CALIB_RRF1_RRF_BM_MIXER(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_BM_MIXER_SHIFT)) & CALIB_RRF1_RRF_BM_MIXER_MASK)
#define CALIB_RRF1_PPF_DCCAL_RES_MASK            (0xC00000U)
#define CALIB_RRF1_PPF_DCCAL_RES_SHIFT           (22U)
#define CALIB_RRF1_PPF_DCCAL_RES(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_PPF_DCCAL_RES_SHIFT)) & CALIB_RRF1_PPF_DCCAL_RES_MASK)
#define CALIB_RRF1_RRF_CAL_MIX_EN_MASK           (0x1000000U)
#define CALIB_RRF1_RRF_CAL_MIX_EN_SHIFT          (24U)
#define CALIB_RRF1_RRF_CAL_MIX_EN(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_CAL_MIX_EN_SHIFT)) & CALIB_RRF1_RRF_CAL_MIX_EN_MASK)
#define CALIB_RRF1_RRF_CAL_MIX1_EN_MASK          (0x2000000U)
#define CALIB_RRF1_RRF_CAL_MIX1_EN_SHIFT         (25U)
#define CALIB_RRF1_RRF_CAL_MIX1_EN(x)            (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_CAL_MIX1_EN_SHIFT)) & CALIB_RRF1_RRF_CAL_MIX1_EN_MASK)
#define CALIB_RRF1_RRF_LO_SEL_P_MASK             (0xC000000U)
#define CALIB_RRF1_RRF_LO_SEL_P_SHIFT            (26U)
#define CALIB_RRF1_RRF_LO_SEL_P(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_LO_SEL_P_SHIFT)) & CALIB_RRF1_RRF_LO_SEL_P_MASK)
#define CALIB_RRF1_RRF_LO_SEL_N_MASK             (0x30000000U)
#define CALIB_RRF1_RRF_LO_SEL_N_SHIFT            (28U)
#define CALIB_RRF1_RRF_LO_SEL_N(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_LO_SEL_N_SHIFT)) & CALIB_RRF1_RRF_LO_SEL_N_MASK)
#define CALIB_RRF1_RRF_LO_AMP_MASK               (0xC0000000U)
#define CALIB_RRF1_RRF_LO_AMP_SHIFT              (30U)
#define CALIB_RRF1_RRF_LO_AMP(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_RRF1_RRF_LO_AMP_SHIFT)) & CALIB_RRF1_RRF_LO_AMP_MASK)

/*! @name PLL48_PPF - reserved */
#define CALIB_PLL48_PPF_PPF_BM_MASK              (0x3U)
#define CALIB_PLL48_PPF_PPF_BM_SHIFT             (0U)
#define CALIB_PLL48_PPF_PPF_BM(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_PLL48_PPF_PPF_BM_SHIFT)) & CALIB_PLL48_PPF_PPF_BM_MASK)
#define CALIB_PLL48_PPF_PPF_IQSW_MASK            (0x4U)
#define CALIB_PLL48_PPF_PPF_IQSW_SHIFT           (2U)
#define CALIB_PLL48_PPF_PPF_IQSW(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_PLL48_PPF_PPF_IQSW_SHIFT)) & CALIB_PLL48_PPF_PPF_IQSW_MASK)
#define CALIB_PLL48_PPF_PLL48_DIFF_CLK_48M_DIS_MASK (0x8U)
#define CALIB_PLL48_PPF_PLL48_DIFF_CLK_48M_DIS_SHIFT (3U)
#define CALIB_PLL48_PPF_PLL48_DIFF_CLK_48M_DIS(x) (((uint32_t)(((uint32_t)(x)) << CALIB_PLL48_PPF_PLL48_DIFF_CLK_48M_DIS_SHIFT)) & CALIB_PLL48_PPF_PLL48_DIFF_CLK_48M_DIS_MASK)
#define CALIB_PLL48_PPF_PLL48_TST_CPREF_MASK     (0xF0U)
#define CALIB_PLL48_PPF_PLL48_TST_CPREF_SHIFT    (4U)
#define CALIB_PLL48_PPF_PLL48_TST_CPREF(x)       (((uint32_t)(((uint32_t)(x)) << CALIB_PLL48_PPF_PLL48_TST_CPREF_SHIFT)) & CALIB_PLL48_PPF_PLL48_TST_CPREF_MASK)

/*! @name LO0 - reserved */
#define CALIB_LO0_VCO_DAC_IPTAT_MASK             (0xFU)
#define CALIB_LO0_VCO_DAC_IPTAT_SHIFT            (0U)
#define CALIB_LO0_VCO_DAC_IPTAT(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_DAC_IPTAT_SHIFT)) & CALIB_LO0_VCO_DAC_IPTAT_MASK)
#define CALIB_LO0_VCO_TST_CP_MASK                (0xF0U)
#define CALIB_LO0_VCO_TST_CP_SHIFT               (4U)
#define CALIB_LO0_VCO_TST_CP(x)                  (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_TST_CP_SHIFT)) & CALIB_LO0_VCO_TST_CP_MASK)
#define CALIB_LO0_VCO_VTUN_SET_MASK              (0x1F00U)
#define CALIB_LO0_VCO_VTUN_SET_SHIFT             (8U)
#define CALIB_LO0_VCO_VTUN_SET(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_VTUN_SET_SHIFT)) & CALIB_LO0_VCO_VTUN_SET_MASK)
#define CALIB_LO0_VCO_ACAL_SET_MASK              (0xE000U)
#define CALIB_LO0_VCO_ACAL_SET_SHIFT             (13U)
#define CALIB_LO0_VCO_ACAL_SET(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_ACAL_SET_SHIFT)) & CALIB_LO0_VCO_ACAL_SET_MASK)
#define CALIB_LO0_VCO_BM_TXFIL_MASK              (0x30000U)
#define CALIB_LO0_VCO_BM_TXFIL_SHIFT             (16U)
#define CALIB_LO0_VCO_BM_TXFIL(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_BM_TXFIL_SHIFT)) & CALIB_LO0_VCO_BM_TXFIL_MASK)
#define CALIB_LO0_VCO_BM_TXDAC_MASK              (0xC0000U)
#define CALIB_LO0_VCO_BM_TXDAC_SHIFT             (18U)
#define CALIB_LO0_VCO_BM_TXDAC(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_BM_TXDAC_SHIFT)) & CALIB_LO0_VCO_BM_TXDAC_MASK)
#define CALIB_LO0_VCO_SAMP_EN_MASK               (0x800000U)
#define CALIB_LO0_VCO_SAMP_EN_SHIFT              (23U)
#define CALIB_LO0_VCO_SAMP_EN(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_SAMP_EN_SHIFT)) & CALIB_LO0_VCO_SAMP_EN_MASK)
#define CALIB_LO0_VCO_CAP_HALF_EN_MASK           (0x1000000U)
#define CALIB_LO0_VCO_CAP_HALF_EN_SHIFT          (24U)
#define CALIB_LO0_VCO_CAP_HALF_EN(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_CAP_HALF_EN_SHIFT)) & CALIB_LO0_VCO_CAP_HALF_EN_MASK)
#define CALIB_LO0_VCO_SET_VCO_VDD_LOW_MASK       (0x2000000U)
#define CALIB_LO0_VCO_SET_VCO_VDD_LOW_SHIFT      (25U)
#define CALIB_LO0_VCO_SET_VCO_VDD_LOW(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_SET_VCO_VDD_LOW_SHIFT)) & CALIB_LO0_VCO_SET_VCO_VDD_LOW_MASK)
#define CALIB_LO0_VCO_8OR16M_INV_EN_MASK         (0x4000000U)
#define CALIB_LO0_VCO_8OR16M_INV_EN_SHIFT        (26U)
#define CALIB_LO0_VCO_8OR16M_INV_EN(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_8OR16M_INV_EN_SHIFT)) & CALIB_LO0_VCO_8OR16M_INV_EN_MASK)
#define CALIB_LO0_VCO_DIV_PD_EN_MASK             (0x8000000U)
#define CALIB_LO0_VCO_DIV_PD_EN_SHIFT            (27U)
#define CALIB_LO0_VCO_DIV_PD_EN(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_DIV_PD_EN_SHIFT)) & CALIB_LO0_VCO_DIV_PD_EN_MASK)
#define CALIB_LO0_VCO_TXDLY1M_MASK               (0x10000000U)
#define CALIB_LO0_VCO_TXDLY1M_SHIFT              (28U)
#define CALIB_LO0_VCO_TXDLY1M(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_TXDLY1M_SHIFT)) & CALIB_LO0_VCO_TXDLY1M_MASK)
#define CALIB_LO0_VCO_TXDLY2M_MASK               (0x20000000U)
#define CALIB_LO0_VCO_TXDLY2M_SHIFT              (29U)
#define CALIB_LO0_VCO_TXDLY2M(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_TXDLY2M_SHIFT)) & CALIB_LO0_VCO_TXDLY2M_MASK)
#define CALIB_LO0_VCO_RX_CK_TST_MASK             (0x40000000U)
#define CALIB_LO0_VCO_RX_CK_TST_SHIFT            (30U)
#define CALIB_LO0_VCO_RX_CK_TST(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_RX_CK_TST_SHIFT)) & CALIB_LO0_VCO_RX_CK_TST_MASK)
#define CALIB_LO0_VCO_DSM_INT_EN_MASK            (0x80000000U)
#define CALIB_LO0_VCO_DSM_INT_EN_SHIFT           (31U)
#define CALIB_LO0_VCO_DSM_INT_EN(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_LO0_VCO_DSM_INT_EN_SHIFT)) & CALIB_LO0_VCO_DSM_INT_EN_MASK)

/*! @name LO1 - Reserved */
#define CALIB_LO1_SPEED_UP_TIME_MASK             (0x1FU)
#define CALIB_LO1_SPEED_UP_TIME_SHIFT            (0U)
#define CALIB_LO1_SPEED_UP_TIME(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_SPEED_UP_TIME_SHIFT)) & CALIB_LO1_SPEED_UP_TIME_MASK)
#define CALIB_LO1_SW_LO_SPEED_UP_MASK            (0x20U)
#define CALIB_LO1_SW_LO_SPEED_UP_SHIFT           (5U)
#define CALIB_LO1_SW_LO_SPEED_UP(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_SW_LO_SPEED_UP_SHIFT)) & CALIB_LO1_SW_LO_SPEED_UP_MASK)
#define CALIB_LO1_RX_PLLPFD_EN_MASK              (0x40U)
#define CALIB_LO1_RX_PLLPFD_EN_SHIFT             (6U)
#define CALIB_LO1_RX_PLLPFD_EN(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_RX_PLLPFD_EN_SHIFT)) & CALIB_LO1_RX_PLLPFD_EN_MASK)
#define CALIB_LO1_TX_PLLPFD_EN_MASK              (0x80U)
#define CALIB_LO1_TX_PLLPFD_EN_SHIFT             (7U)
#define CALIB_LO1_TX_PLLPFD_EN(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_TX_PLLPFD_EN_SHIFT)) & CALIB_LO1_TX_PLLPFD_EN_MASK)
#define CALIB_LO1_LO_SET_TIME_MASK               (0x3F00U)
#define CALIB_LO1_LO_SET_TIME_SHIFT              (8U)
#define CALIB_LO1_LO_SET_TIME(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_LO_SET_TIME_SHIFT)) & CALIB_LO1_LO_SET_TIME_MASK)
#define CALIB_LO1_MOD_TEST_MASK                  (0x4000U)
#define CALIB_LO1_MOD_TEST_SHIFT                 (14U)
#define CALIB_LO1_MOD_TEST(x)                    (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_MOD_TEST_SHIFT)) & CALIB_LO1_MOD_TEST_MASK)
#define CALIB_LO1_DIV_DIFF_CLK_LO_DIS_MASK       (0x8000U)
#define CALIB_LO1_DIV_DIFF_CLK_LO_DIS_SHIFT      (15U)
#define CALIB_LO1_DIV_DIFF_CLK_LO_DIS(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_DIV_DIFF_CLK_LO_DIS_SHIFT)) & CALIB_LO1_DIV_DIFF_CLK_LO_DIS_MASK)
#define CALIB_LO1_TX_VCO_FTC_SET_MASK            (0x30000U)
#define CALIB_LO1_TX_VCO_FTC_SET_SHIFT           (16U)
#define CALIB_LO1_TX_VCO_FTC_SET(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_TX_VCO_FTC_SET_SHIFT)) & CALIB_LO1_TX_VCO_FTC_SET_MASK)
#define CALIB_LO1_RX_VCO_FTC_SET_MASK            (0xC0000U)
#define CALIB_LO1_RX_VCO_FTC_SET_SHIFT           (18U)
#define CALIB_LO1_RX_VCO_FTC_SET(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_LO1_RX_VCO_FTC_SET_SHIFT)) & CALIB_LO1_RX_VCO_FTC_SET_MASK)

/*! @name PA_CTRL - Reserved */
#define CALIB_PA_CTRL_PA_ON_DLY_MASK             (0x3FU)
#define CALIB_PA_CTRL_PA_ON_DLY_SHIFT            (0U)
#define CALIB_PA_CTRL_PA_ON_DLY(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_ON_DLY_SHIFT)) & CALIB_PA_CTRL_PA_ON_DLY_MASK)
#define CALIB_PA_CTRL_PA_OFF_DLY_MASK            (0xF00U)
#define CALIB_PA_CTRL_PA_OFF_DLY_SHIFT           (8U)
#define CALIB_PA_CTRL_PA_OFF_DLY(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_OFF_DLY_SHIFT)) & CALIB_PA_CTRL_PA_OFF_DLY_MASK)
#define CALIB_PA_CTRL_PA_INCREASE_SEL_MASK       (0x70000U)
#define CALIB_PA_CTRL_PA_INCREASE_SEL_SHIFT      (16U)
#define CALIB_PA_CTRL_PA_INCREASE_SEL(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_INCREASE_SEL_SHIFT)) & CALIB_PA_CTRL_PA_INCREASE_SEL_MASK)
#define CALIB_PA_CTRL_PA_SEL_BIAS_MASK           (0x80000U)
#define CALIB_PA_CTRL_PA_SEL_BIAS_SHIFT          (19U)
#define CALIB_PA_CTRL_PA_SEL_BIAS(x)             (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_SEL_BIAS_SHIFT)) & CALIB_PA_CTRL_PA_SEL_BIAS_MASK)
#define CALIB_PA_CTRL_PA_BM_CUR_MASK             (0x300000U)
#define CALIB_PA_CTRL_PA_BM_CUR_SHIFT            (20U)
#define CALIB_PA_CTRL_PA_BM_CUR(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_BM_CUR_SHIFT)) & CALIB_PA_CTRL_PA_BM_CUR_MASK)
#define CALIB_PA_CTRL_PA_VDUTY_CYCLE_SEL_MASK    (0xC00000U)
#define CALIB_PA_CTRL_PA_VDUTY_CYCLE_SEL_SHIFT   (22U)
#define CALIB_PA_CTRL_PA_VDUTY_CYCLE_SEL(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_VDUTY_CYCLE_SEL_SHIFT)) & CALIB_PA_CTRL_PA_VDUTY_CYCLE_SEL_MASK)
#define CALIB_PA_CTRL_PA_VCDCG_MASK              (0x1000000U)
#define CALIB_PA_CTRL_PA_VCDCG_SHIFT             (24U)
#define CALIB_PA_CTRL_PA_VCDCG(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_PA_CTRL_PA_VCDCG_SHIFT)) & CALIB_PA_CTRL_PA_VCDCG_MASK)

/*! @name CTRL - Reserved */
#define CALIB_CTRL_RC_TIM_MASK                   (0x3U)
#define CALIB_CTRL_RC_TIM_SHIFT                  (0U)
#define CALIB_CTRL_RC_TIM(x)                     (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_RC_TIM_SHIFT)) & CALIB_CTRL_RC_TIM_MASK)
#define CALIB_CTRL_VCO_TEST_INT_MASK             (0x10U)
#define CALIB_CTRL_VCO_TEST_INT_SHIFT            (4U)
#define CALIB_CTRL_VCO_TEST_INT(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_VCO_TEST_INT_SHIFT)) & CALIB_CTRL_VCO_TEST_INT_MASK)
#define CALIB_CTRL_HOP_CLB_SEL_MASK              (0x100U)
#define CALIB_CTRL_HOP_CLB_SEL_SHIFT             (8U)
#define CALIB_CTRL_HOP_CLB_SEL(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_HOP_CLB_SEL_SHIFT)) & CALIB_CTRL_HOP_CLB_SEL_MASK)
#define CALIB_CTRL_XTL_PO_TIM_MASK               (0x30000U)
#define CALIB_CTRL_XTL_PO_TIM_SHIFT              (16U)
#define CALIB_CTRL_XTL_PO_TIM(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_XTL_PO_TIM_SHIFT)) & CALIB_CTRL_XTL_PO_TIM_MASK)
#define CALIB_CTRL_XTL_CAL_TIM_MASK              (0xC0000U)
#define CALIB_CTRL_XTL_CAL_TIM_SHIFT             (18U)
#define CALIB_CTRL_XTL_CAL_TIM(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_XTL_CAL_TIM_SHIFT)) & CALIB_CTRL_XTL_CAL_TIM_MASK)
#define CALIB_CTRL_XTL_AMP_DET_PWR_SEL_MASK      (0x300000U)
#define CALIB_CTRL_XTL_AMP_DET_PWR_SEL_SHIFT     (20U)
#define CALIB_CTRL_XTL_AMP_DET_PWR_SEL(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_XTL_AMP_DET_PWR_SEL_SHIFT)) & CALIB_CTRL_XTL_AMP_DET_PWR_SEL_MASK)
#define CALIB_CTRL_XTL_SWCAL_EN_MASK             (0x400000U)
#define CALIB_CTRL_XTL_SWCAL_EN_SHIFT            (22U)
#define CALIB_CTRL_XTL_SWCAL_EN(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_CTRL_XTL_SWCAL_EN_SHIFT)) & CALIB_CTRL_XTL_SWCAL_EN_MASK)

/*! @name INT_RAW - Reserved */
#define CALIB_INT_RAW_PO_CAL_DONE_INT_MASK       (0x1U)
#define CALIB_INT_RAW_PO_CAL_DONE_INT_SHIFT      (0U)
#define CALIB_INT_RAW_PO_CAL_DONE_INT(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_PO_CAL_DONE_INT_SHIFT)) & CALIB_INT_RAW_PO_CAL_DONE_INT_MASK)
#define CALIB_INT_RAW_HOP_CAL_DONE_INT_MASK      (0x2U)
#define CALIB_INT_RAW_HOP_CAL_DONE_INT_SHIFT     (1U)
#define CALIB_INT_RAW_HOP_CAL_DONE_INT(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_HOP_CAL_DONE_INT_SHIFT)) & CALIB_INT_RAW_HOP_CAL_DONE_INT_MASK)
#define CALIB_INT_RAW_OSC_CAL_DONE_INT_MASK      (0x4U)
#define CALIB_INT_RAW_OSC_CAL_DONE_INT_SHIFT     (2U)
#define CALIB_INT_RAW_OSC_CAL_DONE_INT(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_OSC_CAL_DONE_INT_SHIFT)) & CALIB_INT_RAW_OSC_CAL_DONE_INT_MASK)
#define CALIB_INT_RAW_REF_CAL_DONE_INT_MASK      (0x8U)
#define CALIB_INT_RAW_REF_CAL_DONE_INT_SHIFT     (3U)
#define CALIB_INT_RAW_REF_CAL_DONE_INT(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_REF_CAL_DONE_INT_SHIFT)) & CALIB_INT_RAW_REF_CAL_DONE_INT_MASK)
#define CALIB_INT_RAW_RCO_CAL_DONE_INT_MASK      (0x10U)
#define CALIB_INT_RAW_RCO_CAL_DONE_INT_SHIFT     (4U)
#define CALIB_INT_RAW_RCO_CAL_DONE_INT(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_RCO_CAL_DONE_INT_SHIFT)) & CALIB_INT_RAW_RCO_CAL_DONE_INT_MASK)
#define CALIB_INT_RAW_XTL_CAL_DONE_INT_MASK      (0x20U)
#define CALIB_INT_RAW_XTL_CAL_DONE_INT_SHIFT     (5U)
#define CALIB_INT_RAW_XTL_CAL_DONE_INT(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_XTL_CAL_DONE_INT_SHIFT)) & CALIB_INT_RAW_XTL_CAL_DONE_INT_MASK)
#define CALIB_INT_RAW_PO_ALL_DONE_INT_MASK       (0x100U)
#define CALIB_INT_RAW_PO_ALL_DONE_INT_SHIFT      (8U)
#define CALIB_INT_RAW_PO_ALL_DONE_INT(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_PO_ALL_DONE_INT_SHIFT)) & CALIB_INT_RAW_PO_ALL_DONE_INT_MASK)
#define CALIB_INT_RAW_CAL_INT_MASK               (0x10000U)
#define CALIB_INT_RAW_CAL_INT_SHIFT              (16U)
#define CALIB_INT_RAW_CAL_INT(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_INT_RAW_CAL_INT_SHIFT)) & CALIB_INT_RAW_CAL_INT_MASK)

/*! @name INTEN - Reserved */
#define CALIB_INTEN_PO_CAL_DONE_INTEN_MASK       (0x1U)
#define CALIB_INTEN_PO_CAL_DONE_INTEN_SHIFT      (0U)
#define CALIB_INTEN_PO_CAL_DONE_INTEN(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_PO_CAL_DONE_INTEN_SHIFT)) & CALIB_INTEN_PO_CAL_DONE_INTEN_MASK)
#define CALIB_INTEN_HOP_CAL_DONE_INTEN_MASK      (0x2U)
#define CALIB_INTEN_HOP_CAL_DONE_INTEN_SHIFT     (1U)
#define CALIB_INTEN_HOP_CAL_DONE_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_HOP_CAL_DONE_INTEN_SHIFT)) & CALIB_INTEN_HOP_CAL_DONE_INTEN_MASK)
#define CALIB_INTEN_OSC_CAL_DONE_INTEN_MASK      (0x4U)
#define CALIB_INTEN_OSC_CAL_DONE_INTEN_SHIFT     (2U)
#define CALIB_INTEN_OSC_CAL_DONE_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_OSC_CAL_DONE_INTEN_SHIFT)) & CALIB_INTEN_OSC_CAL_DONE_INTEN_MASK)
#define CALIB_INTEN_REF_CAL_DONE_INTEN_MASK      (0x8U)
#define CALIB_INTEN_REF_CAL_DONE_INTEN_SHIFT     (3U)
#define CALIB_INTEN_REF_CAL_DONE_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_REF_CAL_DONE_INTEN_SHIFT)) & CALIB_INTEN_REF_CAL_DONE_INTEN_MASK)
#define CALIB_INTEN_RCO_CAL_DONE_INTEN_MASK      (0x10U)
#define CALIB_INTEN_RCO_CAL_DONE_INTEN_SHIFT     (4U)
#define CALIB_INTEN_RCO_CAL_DONE_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_RCO_CAL_DONE_INTEN_SHIFT)) & CALIB_INTEN_RCO_CAL_DONE_INTEN_MASK)
#define CALIB_INTEN_XTL_CAL_DONE_INTEN_MASK      (0x20U)
#define CALIB_INTEN_XTL_CAL_DONE_INTEN_SHIFT     (5U)
#define CALIB_INTEN_XTL_CAL_DONE_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_XTL_CAL_DONE_INTEN_SHIFT)) & CALIB_INTEN_XTL_CAL_DONE_INTEN_MASK)
#define CALIB_INTEN_PO_ALL_DONE_INTEN_MASK       (0x100U)
#define CALIB_INTEN_PO_ALL_DONE_INTEN_SHIFT      (8U)
#define CALIB_INTEN_PO_ALL_DONE_INTEN(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_INTEN_PO_ALL_DONE_INTEN_SHIFT)) & CALIB_INTEN_PO_ALL_DONE_INTEN_MASK)

/*! @name INT_STAT - Reserved */
#define CALIB_INT_STAT_PO_CAL_DONE_INT_STAT_MASK (0x1U)
#define CALIB_INT_STAT_PO_CAL_DONE_INT_STAT_SHIFT (0U)
#define CALIB_INT_STAT_PO_CAL_DONE_INT_STAT(x)   (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_PO_CAL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_PO_CAL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_HOP_CAL_DONE_INT_STAT_MASK (0x2U)
#define CALIB_INT_STAT_HOP_CAL_DONE_INT_STAT_SHIFT (1U)
#define CALIB_INT_STAT_HOP_CAL_DONE_INT_STAT(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_HOP_CAL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_HOP_CAL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_OSC_CAL_DONE_INT_STAT_MASK (0x4U)
#define CALIB_INT_STAT_OSC_CAL_DONE_INT_STAT_SHIFT (2U)
#define CALIB_INT_STAT_OSC_CAL_DONE_INT_STAT(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_OSC_CAL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_OSC_CAL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_REF_CAL_DONE_INT_STAT_MASK (0x8U)
#define CALIB_INT_STAT_REF_CAL_DONE_INT_STAT_SHIFT (3U)
#define CALIB_INT_STAT_REF_CAL_DONE_INT_STAT(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_REF_CAL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_REF_CAL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_RCO_CAL_DONE_INT_STAT_MASK (0x10U)
#define CALIB_INT_STAT_RCO_CAL_DONE_INT_STAT_SHIFT (4U)
#define CALIB_INT_STAT_RCO_CAL_DONE_INT_STAT(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_RCO_CAL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_RCO_CAL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_XTL_CAL_DONE_INT_STAT_MASK (0x20U)
#define CALIB_INT_STAT_XTL_CAL_DONE_INT_STAT_SHIFT (5U)
#define CALIB_INT_STAT_XTL_CAL_DONE_INT_STAT(x)  (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_XTL_CAL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_XTL_CAL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_PO_ALL_DONE_INT_STAT_MASK (0x100U)
#define CALIB_INT_STAT_PO_ALL_DONE_INT_STAT_SHIFT (8U)
#define CALIB_INT_STAT_PO_ALL_DONE_INT_STAT(x)   (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_PO_ALL_DONE_INT_STAT_SHIFT)) & CALIB_INT_STAT_PO_ALL_DONE_INT_STAT_MASK)
#define CALIB_INT_STAT_CAL_INT_STAT_MASK         (0x10000U)
#define CALIB_INT_STAT_CAL_INT_STAT_SHIFT        (16U)
#define CALIB_INT_STAT_CAL_INT_STAT(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_INT_STAT_CAL_INT_STAT_SHIFT)) & CALIB_INT_STAT_CAL_INT_STAT_MASK)

/*! @name TIF - reserved */
#define CALIB_TIF_TEST_CTRL_MASK                 (0xFU)
#define CALIB_TIF_TEST_CTRL_SHIFT                (0U)
#define CALIB_TIF_TEST_CTRL(x)                   (((uint32_t)(((uint32_t)(x)) << CALIB_TIF_TEST_CTRL_SHIFT)) & CALIB_TIF_TEST_CTRL_MASK)

/*! @name KVCO_MEAN - reserved */
#define CALIB_KVCO_MEAN_KVCO_CNT_MEAN_MASK       (0x1FFFFFU)
#define CALIB_KVCO_MEAN_KVCO_CNT_MEAN_SHIFT      (0U)
#define CALIB_KVCO_MEAN_KVCO_CNT_MEAN(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_KVCO_MEAN_KVCO_CNT_MEAN_SHIFT)) & CALIB_KVCO_MEAN_KVCO_CNT_MEAN_MASK)

/*! @name KVCO_DLT - reserved */
#define CALIB_KVCO_DLT_KVCO_CNT_DLT_MASK         (0x1FFU)
#define CALIB_KVCO_DLT_KVCO_CNT_DLT_SHIFT        (0U)
#define CALIB_KVCO_DLT_KVCO_CNT_DLT(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_KVCO_DLT_KVCO_CNT_DLT_SHIFT)) & CALIB_KVCO_DLT_KVCO_CNT_DLT_MASK)

/*! @name LO_CFG -  */
#define CALIB_LO_CFG_LO_INT_CFG_MASK             (0x3FU)
#define CALIB_LO_CFG_LO_INT_CFG_SHIFT            (0U)
#define CALIB_LO_CFG_LO_INT_CFG(x)               (((uint32_t)(((uint32_t)(x)) << CALIB_LO_CFG_LO_INT_CFG_SHIFT)) & CALIB_LO_CFG_LO_INT_CFG_MASK)
#define CALIB_LO_CFG_LO_FRAC_CFG_MASK            (0xFFFFFC0U)
#define CALIB_LO_CFG_LO_FRAC_CFG_SHIFT           (6U)
#define CALIB_LO_CFG_LO_FRAC_CFG(x)              (((uint32_t)(((uint32_t)(x)) << CALIB_LO_CFG_LO_FRAC_CFG_SHIFT)) & CALIB_LO_CFG_LO_FRAC_CFG_MASK)
#define CALIB_LO_CFG_LO_SEL_MASK                 (0x40000000U)
#define CALIB_LO_CFG_LO_SEL_SHIFT                (30U)
#define CALIB_LO_CFG_LO_SEL(x)                   (((uint32_t)(((uint32_t)(x)) << CALIB_LO_CFG_LO_SEL_SHIFT)) & CALIB_LO_CFG_LO_SEL_MASK)
#define CALIB_LO_CFG_LO_CHANGE_MASK              (0x80000000U)
#define CALIB_LO_CFG_LO_CHANGE_SHIFT             (31U)
#define CALIB_LO_CFG_LO_CHANGE(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO_CFG_LO_CHANGE_SHIFT)) & CALIB_LO_CFG_LO_CHANGE_MASK)

/*! @name LO_TABLE -  */
#define CALIB_LO_TABLE_LO_INT_TABLE_MASK         (0x3FU)
#define CALIB_LO_TABLE_LO_INT_TABLE_SHIFT        (0U)
#define CALIB_LO_TABLE_LO_INT_TABLE(x)           (((uint32_t)(((uint32_t)(x)) << CALIB_LO_TABLE_LO_INT_TABLE_SHIFT)) & CALIB_LO_TABLE_LO_INT_TABLE_MASK)
#define CALIB_LO_TABLE_LO_FRAC_TABLE_MASK        (0x3FFFFC0U)
#define CALIB_LO_TABLE_LO_FRAC_TABLE_SHIFT       (6U)
#define CALIB_LO_TABLE_LO_FRAC_TABLE(x)          (((uint32_t)(((uint32_t)(x)) << CALIB_LO_TABLE_LO_FRAC_TABLE_SHIFT)) & CALIB_LO_TABLE_LO_FRAC_TABLE_MASK)

/*! @name LO_RATIO -  */
#define CALIB_LO_RATIO_LO_INT_MASK               (0x3FU)
#define CALIB_LO_RATIO_LO_INT_SHIFT              (0U)
#define CALIB_LO_RATIO_LO_INT(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_LO_RATIO_LO_INT_SHIFT)) & CALIB_LO_RATIO_LO_INT_MASK)
#define CALIB_LO_RATIO_LO_FRAC_MASK              (0xFFFFFC0U)
#define CALIB_LO_RATIO_LO_FRAC_SHIFT             (6U)
#define CALIB_LO_RATIO_LO_FRAC(x)                (((uint32_t)(((uint32_t)(x)) << CALIB_LO_RATIO_LO_FRAC_SHIFT)) & CALIB_LO_RATIO_LO_FRAC_MASK)

/*! @name VCO_MOD_CFG - TRX 2M mode selection signal */
#define CALIB_VCO_MOD_CFG_VCO_MOD_TX_CFG_MASK    (0x1U)
#define CALIB_VCO_MOD_CFG_VCO_MOD_TX_CFG_SHIFT   (0U)
#define CALIB_VCO_MOD_CFG_VCO_MOD_TX_CFG(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_CFG_VCO_MOD_TX_CFG_SHIFT)) & CALIB_VCO_MOD_CFG_VCO_MOD_TX_CFG_MASK)
#define CALIB_VCO_MOD_CFG_VCO_MOD_TX_SEL_MASK    (0x2U)
#define CALIB_VCO_MOD_CFG_VCO_MOD_TX_SEL_SHIFT   (1U)
#define CALIB_VCO_MOD_CFG_VCO_MOD_TX_SEL(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_CFG_VCO_MOD_TX_SEL_SHIFT)) & CALIB_VCO_MOD_CFG_VCO_MOD_TX_SEL_MASK)
#define CALIB_VCO_MOD_CFG_TRX2M_MODE_CFG_MASK    (0x4U)
#define CALIB_VCO_MOD_CFG_TRX2M_MODE_CFG_SHIFT   (2U)
#define CALIB_VCO_MOD_CFG_TRX2M_MODE_CFG(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_CFG_TRX2M_MODE_CFG_SHIFT)) & CALIB_VCO_MOD_CFG_TRX2M_MODE_CFG_MASK)
#define CALIB_VCO_MOD_CFG_TRX2M_MODE_SEL_MASK    (0x8U)
#define CALIB_VCO_MOD_CFG_TRX2M_MODE_SEL_SHIFT   (3U)
#define CALIB_VCO_MOD_CFG_TRX2M_MODE_SEL(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_CFG_TRX2M_MODE_SEL_SHIFT)) & CALIB_VCO_MOD_CFG_TRX2M_MODE_SEL_MASK)
#define CALIB_VCO_MOD_CFG_IMR_MASK               (0x10U)
#define CALIB_VCO_MOD_CFG_IMR_SHIFT              (4U)
#define CALIB_VCO_MOD_CFG_IMR(x)                 (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_CFG_IMR_SHIFT)) & CALIB_VCO_MOD_CFG_IMR_MASK)

/*! @name VCO_MOD_STAT -  */
#define CALIB_VCO_MOD_STAT_VCO_MOD_TX_MASK       (0x1U)
#define CALIB_VCO_MOD_STAT_VCO_MOD_TX_SHIFT      (0U)
#define CALIB_VCO_MOD_STAT_VCO_MOD_TX(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_STAT_VCO_MOD_TX_SHIFT)) & CALIB_VCO_MOD_STAT_VCO_MOD_TX_MASK)
#define CALIB_VCO_MOD_STAT_TRX2M_MODE_MASK       (0x4U)
#define CALIB_VCO_MOD_STAT_TRX2M_MODE_SHIFT      (2U)
#define CALIB_VCO_MOD_STAT_TRX2M_MODE(x)         (((uint32_t)(((uint32_t)(x)) << CALIB_VCO_MOD_STAT_TRX2M_MODE_SHIFT)) & CALIB_VCO_MOD_STAT_TRX2M_MODE_MASK)

/*! @name CH_IDX -  */
#define CALIB_CH_IDX_CH_IDX_MASK                 (0xFFU)
#define CALIB_CH_IDX_CH_IDX_SHIFT                (0U)
#define CALIB_CH_IDX_CH_IDX(x)                   (((uint32_t)(((uint32_t)(x)) << CALIB_CH_IDX_CH_IDX_SHIFT)) & CALIB_CH_IDX_CH_IDX_MASK)

/*! @name VCOF_CNT_UP - reserved */
#define CALIB_VCOF_CNT_UP_TX_VCOF_CNT_UP_MASK    (0xFFU)
#define CALIB_VCOF_CNT_UP_TX_VCOF_CNT_UP_SHIFT   (0U)
#define CALIB_VCOF_CNT_UP_TX_VCOF_CNT_UP(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_UP_TX_VCOF_CNT_UP_SHIFT)) & CALIB_VCOF_CNT_UP_TX_VCOF_CNT_UP_MASK)
#define CALIB_VCOF_CNT_UP_RX_VCOF_CNT_UP_MASK    (0xFF0000U)
#define CALIB_VCOF_CNT_UP_RX_VCOF_CNT_UP_SHIFT   (16U)
#define CALIB_VCOF_CNT_UP_RX_VCOF_CNT_UP(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_UP_RX_VCOF_CNT_UP_SHIFT)) & CALIB_VCOF_CNT_UP_RX_VCOF_CNT_UP_MASK)

/*! @name VCOF_CNT_DN - reserved */
#define CALIB_VCOF_CNT_DN_TX_VCOF_CNT_DN_MASK    (0xFFU)
#define CALIB_VCOF_CNT_DN_TX_VCOF_CNT_DN_SHIFT   (0U)
#define CALIB_VCOF_CNT_DN_TX_VCOF_CNT_DN(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_DN_TX_VCOF_CNT_DN_SHIFT)) & CALIB_VCOF_CNT_DN_TX_VCOF_CNT_DN_MASK)
#define CALIB_VCOF_CNT_DN_RX_VCOF_CNT_DN_MASK    (0xFF0000U)
#define CALIB_VCOF_CNT_DN_RX_VCOF_CNT_DN_SHIFT   (16U)
#define CALIB_VCOF_CNT_DN_RX_VCOF_CNT_DN(x)      (((uint32_t)(((uint32_t)(x)) << CALIB_VCOF_CNT_DN_RX_VCOF_CNT_DN_SHIFT)) & CALIB_VCOF_CNT_DN_RX_VCOF_CNT_DN_MASK)


/*!
 * @}
 */ /* end of group CALIB_Register_Masks */


/* CALIB - Peripheral instance base addresses */
/** Peripheral CALIB base address */
#define CALIB_BASE                               (0x4000F000u)
/** Peripheral CALIB base pointer */
#define CALIB                                    ((CALIB_Type *)CALIB_BASE)
/** Array initializer of CALIB peripheral base addresses */
#define CALIB_BASE_ADDRS                         { CALIB_BASE }
/** Array initializer of CALIB peripheral base pointers */
#define CALIB_BASE_PTRS                          { CALIB }
/** Interrupt vectors for the CALIB peripheral type */
#define CALIB_IRQS                               { CALIB_IRQn }

/*!
 * @}
 */ /* end of group CALIB_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CRC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Peripheral_Access_Layer CRC Peripheral Access Layer
 * @{
 */

/** CRC - Register Layout Typedef */
typedef struct {
  __IO uint32_t MODE;                              /**< CRC mode register, offset: 0x0 */
  __IO uint32_t SEED;                              /**< CRC seed register, offset: 0x4 */
  union {                                          /* offset: 0x8 */
    __I  uint32_t SUM;                               /**< CRC checksum register, offset: 0x8 */
    __O  uint32_t WR_DATA;                           /**< CRC data register, offset: 0x8 */
  };
} CRC_Type;

/* ----------------------------------------------------------------------------
   -- CRC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Register_Masks CRC Register Masks
 * @{
 */

/*! @name MODE - CRC mode register */
#define CRC_MODE_CRC_POLY_MASK                   (0x3U)
#define CRC_MODE_CRC_POLY_SHIFT                  (0U)
#define CRC_MODE_CRC_POLY(x)                     (((uint32_t)(((uint32_t)(x)) << CRC_MODE_CRC_POLY_SHIFT)) & CRC_MODE_CRC_POLY_MASK)
#define CRC_MODE_BIT_RVS_WR_MASK                 (0x4U)
#define CRC_MODE_BIT_RVS_WR_SHIFT                (2U)
#define CRC_MODE_BIT_RVS_WR(x)                   (((uint32_t)(((uint32_t)(x)) << CRC_MODE_BIT_RVS_WR_SHIFT)) & CRC_MODE_BIT_RVS_WR_MASK)
#define CRC_MODE_CMPL_WR_MASK                    (0x8U)
#define CRC_MODE_CMPL_WR_SHIFT                   (3U)
#define CRC_MODE_CMPL_WR(x)                      (((uint32_t)(((uint32_t)(x)) << CRC_MODE_CMPL_WR_SHIFT)) & CRC_MODE_CMPL_WR_MASK)
#define CRC_MODE_BIT_RVS_SUM_MASK                (0x10U)
#define CRC_MODE_BIT_RVS_SUM_SHIFT               (4U)
#define CRC_MODE_BIT_RVS_SUM(x)                  (((uint32_t)(((uint32_t)(x)) << CRC_MODE_BIT_RVS_SUM_SHIFT)) & CRC_MODE_BIT_RVS_SUM_MASK)
#define CRC_MODE_CMPL_SUM_MASK                   (0x20U)
#define CRC_MODE_CMPL_SUM_SHIFT                  (5U)
#define CRC_MODE_CMPL_SUM(x)                     (((uint32_t)(((uint32_t)(x)) << CRC_MODE_CMPL_SUM_SHIFT)) & CRC_MODE_CMPL_SUM_MASK)

/*! @name SEED - CRC seed register */
#define CRC_SEED_CRC_SEED_MASK                   (0xFFFFFFFFU)
#define CRC_SEED_CRC_SEED_SHIFT                  (0U)
#define CRC_SEED_CRC_SEED(x)                     (((uint32_t)(((uint32_t)(x)) << CRC_SEED_CRC_SEED_SHIFT)) & CRC_SEED_CRC_SEED_MASK)

/*! @name SUM - CRC checksum register */
#define CRC_SUM_CRC_SUM_MASK                     (0xFFFFFFFFU)
#define CRC_SUM_CRC_SUM_SHIFT                    (0U)
#define CRC_SUM_CRC_SUM(x)                       (((uint32_t)(((uint32_t)(x)) << CRC_SUM_CRC_SUM_SHIFT)) & CRC_SUM_CRC_SUM_MASK)

/*! @name WR_DATA - CRC data register */
#define CRC_WR_DATA_CRC_WR_DATA_MASK             (0xFFFFFFFFU)
#define CRC_WR_DATA_CRC_WR_DATA_SHIFT            (0U)
#define CRC_WR_DATA_CRC_WR_DATA(x)               (((uint32_t)(((uint32_t)(x)) << CRC_WR_DATA_CRC_WR_DATA_SHIFT)) & CRC_WR_DATA_CRC_WR_DATA_MASK)


/*!
 * @}
 */ /* end of group CRC_Register_Masks */


/* CRC - Peripheral instance base addresses */
/** Peripheral CRC_ENGINE base address */
#define CRC_ENGINE_BASE                          (0x4008E000u)
/** Peripheral CRC_ENGINE base pointer */
#define CRC_ENGINE                               ((CRC_Type *)CRC_ENGINE_BASE)
/** Array initializer of CRC peripheral base addresses */
#define CRC_BASE_ADDRS                           { CRC_ENGINE_BASE }
/** Array initializer of CRC peripheral base pointers */
#define CRC_BASE_PTRS                            { CRC_ENGINE }

/*!
 * @}
 */ /* end of group CRC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CS_Peripheral_Access_Layer CS Peripheral Access Layer
 * @{
 */

/** CS - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL0;                             /**< CapSense control register 0, offset: 0x0 */
  __IO uint32_t CTRL1;                             /**< CapSense control register 1, offset: 0x4 */
  __IO uint32_t INT;                               /**< Interrupt status register, offset: 0x8 */
  __IO uint32_t INTEN;                             /**< Interrupt mask register, offset: 0xC */
  __I  uint32_t DATA;                              /**< Output data register, offset: 0x10 */
  __IO uint32_t LP_CTRL;                           /**< Control register for low power mode, offset: 0x14 */
  __I  uint32_t LP_INT;                            /**< Low power interrupt register, offset: 0x18 */
  __IO uint32_t LP_INTEN;                          /**< low power interrupt enable register, offset: 0x1C */
  __IO uint32_t IDLE_PERIOD;                       /**< Idle preiod number register, offset: 0x20 */
} CS_Type;

/* ----------------------------------------------------------------------------
   -- CS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CS_Register_Masks CS Register Masks
 * @{
 */

/*! @name CTRL0 - CapSense control register 0 */
#define CS_CTRL0_ENABLE_MASK                     (0x1U)
#define CS_CTRL0_ENABLE_SHIFT                    (0U)
#define CS_CTRL0_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << CS_CTRL0_ENABLE_SHIFT)) & CS_CTRL0_ENABLE_MASK)
#define CS_CTRL0_SRST_MASK                       (0x2U)
#define CS_CTRL0_SRST_SHIFT                      (1U)
#define CS_CTRL0_SRST(x)                         (((uint32_t)(((uint32_t)(x)) << CS_CTRL0_SRST_SHIFT)) & CS_CTRL0_SRST_MASK)
#define CS_CTRL0_OSC_FREQ_MASK                   (0xFCU)
#define CS_CTRL0_OSC_FREQ_SHIFT                  (2U)
#define CS_CTRL0_OSC_FREQ(x)                     (((uint32_t)(((uint32_t)(x)) << CS_CTRL0_OSC_FREQ_SHIFT)) & CS_CTRL0_OSC_FREQ_MASK)
#define CS_CTRL0_CLK_DIV_MASK                    (0x1FF0000U)
#define CS_CTRL0_CLK_DIV_SHIFT                   (16U)
#define CS_CTRL0_CLK_DIV(x)                      (((uint32_t)(((uint32_t)(x)) << CS_CTRL0_CLK_DIV_SHIFT)) & CS_CTRL0_CLK_DIV_MASK)

/*! @name CTRL1 - CapSense control register 1 */
#define CS_CTRL1_PERIOD_MASK                     (0xFFFFU)
#define CS_CTRL1_PERIOD_SHIFT                    (0U)
#define CS_CTRL1_PERIOD(x)                       (((uint32_t)(((uint32_t)(x)) << CS_CTRL1_PERIOD_SHIFT)) & CS_CTRL1_PERIOD_MASK)
#define CS_CTRL1_CH_MASK                         (0xFF0000U)
#define CS_CTRL1_CH_SHIFT                        (16U)
#define CS_CTRL1_CH(x)                           (((uint32_t)(((uint32_t)(x)) << CS_CTRL1_CH_SHIFT)) & CS_CTRL1_CH_MASK)

/*! @name INT - Interrupt status register */
#define CS_INT_FIFO_NOTEMPTY_INT_MASK            (0x1U)
#define CS_INT_FIFO_NOTEMPTY_INT_SHIFT           (0U)
#define CS_INT_FIFO_NOTEMPTY_INT(x)              (((uint32_t)(((uint32_t)(x)) << CS_INT_FIFO_NOTEMPTY_INT_SHIFT)) & CS_INT_FIFO_NOTEMPTY_INT_MASK)
#define CS_INT_FIFO_HFULL_INT_MASK               (0x2U)
#define CS_INT_FIFO_HFULL_INT_SHIFT              (1U)
#define CS_INT_FIFO_HFULL_INT(x)                 (((uint32_t)(((uint32_t)(x)) << CS_INT_FIFO_HFULL_INT_SHIFT)) & CS_INT_FIFO_HFULL_INT_MASK)
#define CS_INT_FIFO_FULL_INT_MASK                (0x4U)
#define CS_INT_FIFO_FULL_INT_SHIFT               (2U)
#define CS_INT_FIFO_FULL_INT(x)                  (((uint32_t)(((uint32_t)(x)) << CS_INT_FIFO_FULL_INT_SHIFT)) & CS_INT_FIFO_FULL_INT_MASK)
#define CS_INT_SCAN_INT_MASK                     (0x8U)
#define CS_INT_SCAN_INT_SHIFT                    (3U)
#define CS_INT_SCAN_INT(x)                       (((uint32_t)(((uint32_t)(x)) << CS_INT_SCAN_INT_SHIFT)) & CS_INT_SCAN_INT_MASK)

/*! @name INTEN - Interrupt mask register */
#define CS_INTEN_FIFO_NOTEMPTY_INTEN_MASK        (0x1U)
#define CS_INTEN_FIFO_NOTEMPTY_INTEN_SHIFT       (0U)
#define CS_INTEN_FIFO_NOTEMPTY_INTEN(x)          (((uint32_t)(((uint32_t)(x)) << CS_INTEN_FIFO_NOTEMPTY_INTEN_SHIFT)) & CS_INTEN_FIFO_NOTEMPTY_INTEN_MASK)
#define CS_INTEN_FIFO_HFULL_INTEN_MASK           (0x2U)
#define CS_INTEN_FIFO_HFULL_INTEN_SHIFT          (1U)
#define CS_INTEN_FIFO_HFULL_INTEN(x)             (((uint32_t)(((uint32_t)(x)) << CS_INTEN_FIFO_HFULL_INTEN_SHIFT)) & CS_INTEN_FIFO_HFULL_INTEN_MASK)
#define CS_INTEN_FIFO_FULL_INTEN_MASK            (0x4U)
#define CS_INTEN_FIFO_FULL_INTEN_SHIFT           (2U)
#define CS_INTEN_FIFO_FULL_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << CS_INTEN_FIFO_FULL_INTEN_SHIFT)) & CS_INTEN_FIFO_FULL_INTEN_MASK)
#define CS_INTEN_SCAN_INTEN_MASK                 (0x8U)
#define CS_INTEN_SCAN_INTEN_SHIFT                (3U)
#define CS_INTEN_SCAN_INTEN(x)                   (((uint32_t)(((uint32_t)(x)) << CS_INTEN_SCAN_INTEN_SHIFT)) & CS_INTEN_SCAN_INTEN_MASK)

/*! @name DATA - Output data register */
#define CS_DATA_DATA_MASK                        (0x7FFFFU)
#define CS_DATA_DATA_SHIFT                       (0U)
#define CS_DATA_DATA(x)                          (((uint32_t)(((uint32_t)(x)) << CS_DATA_DATA_SHIFT)) & CS_DATA_DATA_MASK)

/*! @name LP_CTRL - Control register for low power mode */
#define CS_LP_CTRL_DEBONCE_NUM_MASK              (0xFU)
#define CS_LP_CTRL_DEBONCE_NUM_SHIFT             (0U)
#define CS_LP_CTRL_DEBONCE_NUM(x)                (((uint32_t)(((uint32_t)(x)) << CS_LP_CTRL_DEBONCE_NUM_SHIFT)) & CS_LP_CTRL_DEBONCE_NUM_MASK)
#define CS_LP_CTRL_LP_EN_MASK                    (0x10U)
#define CS_LP_CTRL_LP_EN_SHIFT                   (4U)
#define CS_LP_CTRL_LP_EN(x)                      (((uint32_t)(((uint32_t)(x)) << CS_LP_CTRL_LP_EN_SHIFT)) & CS_LP_CTRL_LP_EN_MASK)
#define CS_LP_CTRL_LP_CH_MASK                    (0xE0U)
#define CS_LP_CTRL_LP_CH_SHIFT                   (5U)
#define CS_LP_CTRL_LP_CH(x)                      (((uint32_t)(((uint32_t)(x)) << CS_LP_CTRL_LP_CH_SHIFT)) & CS_LP_CTRL_LP_CH_MASK)
#define CS_LP_CTRL_THR_MASK                      (0xFFFF0000U)
#define CS_LP_CTRL_THR_SHIFT                     (16U)
#define CS_LP_CTRL_THR(x)                        (((uint32_t)(((uint32_t)(x)) << CS_LP_CTRL_THR_SHIFT)) & CS_LP_CTRL_THR_MASK)

/*! @name LP_INT - Low power interrupt register */
#define CS_LP_INT_LP_INT_MASK                    (0x1U)
#define CS_LP_INT_LP_INT_SHIFT                   (0U)
#define CS_LP_INT_LP_INT(x)                      (((uint32_t)(((uint32_t)(x)) << CS_LP_INT_LP_INT_SHIFT)) & CS_LP_INT_LP_INT_MASK)

/*! @name LP_INTEN - low power interrupt enable register */
#define CS_LP_INTEN_LP_INTEN_MASK                (0x1U)
#define CS_LP_INTEN_LP_INTEN_SHIFT               (0U)
#define CS_LP_INTEN_LP_INTEN(x)                  (((uint32_t)(((uint32_t)(x)) << CS_LP_INTEN_LP_INTEN_SHIFT)) & CS_LP_INTEN_LP_INTEN_MASK)

/*! @name IDLE_PERIOD - Idle preiod number register */
#define CS_IDLE_PERIOD_IDLE_PERIOD_MASK          (0xFFFFU)
#define CS_IDLE_PERIOD_IDLE_PERIOD_SHIFT         (0U)
#define CS_IDLE_PERIOD_IDLE_PERIOD(x)            (((uint32_t)(((uint32_t)(x)) << CS_IDLE_PERIOD_IDLE_PERIOD_SHIFT)) & CS_IDLE_PERIOD_IDLE_PERIOD_MASK)


/*!
 * @}
 */ /* end of group CS_Register_Masks */


/* CS - Peripheral instance base addresses */
/** Peripheral CS base address */
#define CS_BASE                                  (0x40007800u)
/** Peripheral CS base pointer */
#define CS                                       ((CS_Type *)CS_BASE)
/** Array initializer of CS peripheral base addresses */
#define CS_BASE_ADDRS                            { CS_BASE }
/** Array initializer of CS peripheral base pointers */
#define CS_BASE_PTRS                             { CS }
/** Interrupt vectors for the CS peripheral type */
#define CS_IRQS                                  { { CS_IRQn, CS_WAKEUP_IRQn } }

/*!
 * @}
 */ /* end of group CS_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CTIMER Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CTIMER_Peripheral_Access_Layer CTIMER Peripheral Access Layer
 * @{
 */

/** CTIMER - Register Layout Typedef */
typedef struct {
  __IO uint32_t IR;                                /**< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending., offset: 0x0 */
  __IO uint32_t TCR;                               /**< Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR., offset: 0x4 */
  __IO uint32_t TC;                                /**< Timer Counter. The 32 bit TC is incremented every PR+1 cycles of the APB bus clock. The TC is controlled through the TCR., offset: 0x8 */
  __IO uint32_t PR;                                /**< Prescale Register. When the Prescale Counter (PC) is equal to this value, the next clock increments the TC and clears the PC., offset: 0xC */
  __IO uint32_t PC;                                /**< Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface., offset: 0x10 */
  __IO uint32_t MCR;                               /**< Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs., offset: 0x14 */
  __IO uint32_t MR[4];                             /**< Match Register . MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC., array offset: 0x18, array step: 0x4 */
  __IO uint32_t CCR;                               /**< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place., offset: 0x28 */
  __I  uint32_t CR[3];                             /**< Capture Register . CR is loaded with the value of TC when there is an event on the CAPn. input., array offset: 0x2C, array step: 0x4 */
       uint8_t RESERVED_0[4];
  __IO uint32_t EMR;                               /**< External Match Register. The EMR controls the match function and the external match pins., offset: 0x3C */
       uint8_t RESERVED_1[48];
  __IO uint32_t CTCR;                              /**< Count Control Register. The CTCR selects between Timer and Counter mode, and in Counter mode selects the signal and edge(s) for counting., offset: 0x70 */
  __IO uint32_t PWMC;                              /**< PWM Control Register. The PWMCON enables PWM mode for the external match pins., offset: 0x74 */
} CTIMER_Type;

/* ----------------------------------------------------------------------------
   -- CTIMER Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CTIMER_Register_Masks CTIMER Register Masks
 * @{
 */

/*! @name IR - Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending. */
#define CTIMER_IR_MR0INT_MASK                    (0x1U)
#define CTIMER_IR_MR0INT_SHIFT                   (0U)
#define CTIMER_IR_MR0INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_MR0INT_SHIFT)) & CTIMER_IR_MR0INT_MASK)
#define CTIMER_IR_MR1INT_MASK                    (0x2U)
#define CTIMER_IR_MR1INT_SHIFT                   (1U)
#define CTIMER_IR_MR1INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_MR1INT_SHIFT)) & CTIMER_IR_MR1INT_MASK)
#define CTIMER_IR_MR2INT_MASK                    (0x4U)
#define CTIMER_IR_MR2INT_SHIFT                   (2U)
#define CTIMER_IR_MR2INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_MR2INT_SHIFT)) & CTIMER_IR_MR2INT_MASK)
#define CTIMER_IR_MR3INT_MASK                    (0x8U)
#define CTIMER_IR_MR3INT_SHIFT                   (3U)
#define CTIMER_IR_MR3INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_MR3INT_SHIFT)) & CTIMER_IR_MR3INT_MASK)
#define CTIMER_IR_CR0INT_MASK                    (0x10U)
#define CTIMER_IR_CR0INT_SHIFT                   (4U)
#define CTIMER_IR_CR0INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_CR0INT_SHIFT)) & CTIMER_IR_CR0INT_MASK)
#define CTIMER_IR_CR1INT_MASK                    (0x20U)
#define CTIMER_IR_CR1INT_SHIFT                   (5U)
#define CTIMER_IR_CR1INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_CR1INT_SHIFT)) & CTIMER_IR_CR1INT_MASK)
#define CTIMER_IR_CR2INT_MASK                    (0x40U)
#define CTIMER_IR_CR2INT_SHIFT                   (6U)
#define CTIMER_IR_CR2INT(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_IR_CR2INT_SHIFT)) & CTIMER_IR_CR2INT_MASK)

/*! @name TCR - Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR. */
#define CTIMER_TCR_CEN_MASK                      (0x1U)
#define CTIMER_TCR_CEN_SHIFT                     (0U)
#define CTIMER_TCR_CEN(x)                        (((uint32_t)(((uint32_t)(x)) << CTIMER_TCR_CEN_SHIFT)) & CTIMER_TCR_CEN_MASK)
#define CTIMER_TCR_CRST_MASK                     (0x2U)
#define CTIMER_TCR_CRST_SHIFT                    (1U)
#define CTIMER_TCR_CRST(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_TCR_CRST_SHIFT)) & CTIMER_TCR_CRST_MASK)

/*! @name TC - Timer Counter. The 32 bit TC is incremented every PR+1 cycles of the APB bus clock. The TC is controlled through the TCR. */
#define CTIMER_TC_TCVAL_MASK                     (0xFFFFFFFFU)
#define CTIMER_TC_TCVAL_SHIFT                    (0U)
#define CTIMER_TC_TCVAL(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_TC_TCVAL_SHIFT)) & CTIMER_TC_TCVAL_MASK)

/*! @name PR - Prescale Register. When the Prescale Counter (PC) is equal to this value, the next clock increments the TC and clears the PC. */
#define CTIMER_PR_PRVAL_MASK                     (0xFFFFFFFFU)
#define CTIMER_PR_PRVAL_SHIFT                    (0U)
#define CTIMER_PR_PRVAL(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_PR_PRVAL_SHIFT)) & CTIMER_PR_PRVAL_MASK)

/*! @name PC - Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface. */
#define CTIMER_PC_PCVAL_MASK                     (0xFFFFFFFFU)
#define CTIMER_PC_PCVAL_SHIFT                    (0U)
#define CTIMER_PC_PCVAL(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_PC_PCVAL_SHIFT)) & CTIMER_PC_PCVAL_MASK)

/*! @name MCR - Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs. */
#define CTIMER_MCR_MR0I_MASK                     (0x1U)
#define CTIMER_MCR_MR0I_SHIFT                    (0U)
#define CTIMER_MCR_MR0I(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR0I_SHIFT)) & CTIMER_MCR_MR0I_MASK)
#define CTIMER_MCR_MR0R_MASK                     (0x2U)
#define CTIMER_MCR_MR0R_SHIFT                    (1U)
#define CTIMER_MCR_MR0R(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR0R_SHIFT)) & CTIMER_MCR_MR0R_MASK)
#define CTIMER_MCR_MR0S_MASK                     (0x4U)
#define CTIMER_MCR_MR0S_SHIFT                    (2U)
#define CTIMER_MCR_MR0S(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR0S_SHIFT)) & CTIMER_MCR_MR0S_MASK)
#define CTIMER_MCR_MR1I_MASK                     (0x8U)
#define CTIMER_MCR_MR1I_SHIFT                    (3U)
#define CTIMER_MCR_MR1I(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR1I_SHIFT)) & CTIMER_MCR_MR1I_MASK)
#define CTIMER_MCR_MR1R_MASK                     (0x10U)
#define CTIMER_MCR_MR1R_SHIFT                    (4U)
#define CTIMER_MCR_MR1R(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR1R_SHIFT)) & CTIMER_MCR_MR1R_MASK)
#define CTIMER_MCR_MR1S_MASK                     (0x20U)
#define CTIMER_MCR_MR1S_SHIFT                    (5U)
#define CTIMER_MCR_MR1S(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR1S_SHIFT)) & CTIMER_MCR_MR1S_MASK)
#define CTIMER_MCR_MR2I_MASK                     (0x40U)
#define CTIMER_MCR_MR2I_SHIFT                    (6U)
#define CTIMER_MCR_MR2I(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR2I_SHIFT)) & CTIMER_MCR_MR2I_MASK)
#define CTIMER_MCR_MR2R_MASK                     (0x80U)
#define CTIMER_MCR_MR2R_SHIFT                    (7U)
#define CTIMER_MCR_MR2R(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR2R_SHIFT)) & CTIMER_MCR_MR2R_MASK)
#define CTIMER_MCR_MR2S_MASK                     (0x100U)
#define CTIMER_MCR_MR2S_SHIFT                    (8U)
#define CTIMER_MCR_MR2S(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR2S_SHIFT)) & CTIMER_MCR_MR2S_MASK)
#define CTIMER_MCR_MR3I_MASK                     (0x200U)
#define CTIMER_MCR_MR3I_SHIFT                    (9U)
#define CTIMER_MCR_MR3I(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR3I_SHIFT)) & CTIMER_MCR_MR3I_MASK)
#define CTIMER_MCR_MR3R_MASK                     (0x400U)
#define CTIMER_MCR_MR3R_SHIFT                    (10U)
#define CTIMER_MCR_MR3R(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR3R_SHIFT)) & CTIMER_MCR_MR3R_MASK)
#define CTIMER_MCR_MR3S_MASK                     (0x800U)
#define CTIMER_MCR_MR3S_SHIFT                    (11U)
#define CTIMER_MCR_MR3S(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MCR_MR3S_SHIFT)) & CTIMER_MCR_MR3S_MASK)

/*! @name MR - Match Register . MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
#define CTIMER_MR_MATCH_MASK                     (0xFFFFFFFFU)
#define CTIMER_MR_MATCH_SHIFT                    (0U)
#define CTIMER_MR_MATCH(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_MR_MATCH_SHIFT)) & CTIMER_MR_MATCH_MASK)

/* The count of CTIMER_MR */
#define CTIMER_MR_COUNT                          (4U)

/*! @name CCR - Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place. */
#define CTIMER_CCR_CAP0RE_MASK                   (0x1U)
#define CTIMER_CCR_CAP0RE_SHIFT                  (0U)
#define CTIMER_CCR_CAP0RE(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP0RE_SHIFT)) & CTIMER_CCR_CAP0RE_MASK)
#define CTIMER_CCR_CAP0FE_MASK                   (0x2U)
#define CTIMER_CCR_CAP0FE_SHIFT                  (1U)
#define CTIMER_CCR_CAP0FE(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP0FE_SHIFT)) & CTIMER_CCR_CAP0FE_MASK)
#define CTIMER_CCR_CAP0I_MASK                    (0x4U)
#define CTIMER_CCR_CAP0I_SHIFT                   (2U)
#define CTIMER_CCR_CAP0I(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP0I_SHIFT)) & CTIMER_CCR_CAP0I_MASK)
#define CTIMER_CCR_CAP1RE_MASK                   (0x8U)
#define CTIMER_CCR_CAP1RE_SHIFT                  (3U)
#define CTIMER_CCR_CAP1RE(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP1RE_SHIFT)) & CTIMER_CCR_CAP1RE_MASK)
#define CTIMER_CCR_CAP1FE_MASK                   (0x10U)
#define CTIMER_CCR_CAP1FE_SHIFT                  (4U)
#define CTIMER_CCR_CAP1FE(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP1FE_SHIFT)) & CTIMER_CCR_CAP1FE_MASK)
#define CTIMER_CCR_CAP1I_MASK                    (0x20U)
#define CTIMER_CCR_CAP1I_SHIFT                   (5U)
#define CTIMER_CCR_CAP1I(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP1I_SHIFT)) & CTIMER_CCR_CAP1I_MASK)
#define CTIMER_CCR_CAP2RE_MASK                   (0x40U)
#define CTIMER_CCR_CAP2RE_SHIFT                  (6U)
#define CTIMER_CCR_CAP2RE(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP2RE_SHIFT)) & CTIMER_CCR_CAP2RE_MASK)
#define CTIMER_CCR_CAP2FE_MASK                   (0x80U)
#define CTIMER_CCR_CAP2FE_SHIFT                  (7U)
#define CTIMER_CCR_CAP2FE(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP2FE_SHIFT)) & CTIMER_CCR_CAP2FE_MASK)
#define CTIMER_CCR_CAP2I_MASK                    (0x100U)
#define CTIMER_CCR_CAP2I_SHIFT                   (8U)
#define CTIMER_CCR_CAP2I(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_CCR_CAP2I_SHIFT)) & CTIMER_CCR_CAP2I_MASK)

/*! @name CR - Capture Register . CR is loaded with the value of TC when there is an event on the CAPn. input. */
#define CTIMER_CR_CAP_MASK                       (0xFFFFFFFFU)
#define CTIMER_CR_CAP_SHIFT                      (0U)
#define CTIMER_CR_CAP(x)                         (((uint32_t)(((uint32_t)(x)) << CTIMER_CR_CAP_SHIFT)) & CTIMER_CR_CAP_MASK)

/* The count of CTIMER_CR */
#define CTIMER_CR_COUNT                          (3U)

/*! @name EMR - External Match Register. The EMR controls the match function and the external match pins. */
#define CTIMER_EMR_EM0_MASK                      (0x1U)
#define CTIMER_EMR_EM0_SHIFT                     (0U)
#define CTIMER_EMR_EM0(x)                        (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EM0_SHIFT)) & CTIMER_EMR_EM0_MASK)
#define CTIMER_EMR_EM1_MASK                      (0x2U)
#define CTIMER_EMR_EM1_SHIFT                     (1U)
#define CTIMER_EMR_EM1(x)                        (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EM1_SHIFT)) & CTIMER_EMR_EM1_MASK)
#define CTIMER_EMR_EM2_MASK                      (0x4U)
#define CTIMER_EMR_EM2_SHIFT                     (2U)
#define CTIMER_EMR_EM2(x)                        (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EM2_SHIFT)) & CTIMER_EMR_EM2_MASK)
#define CTIMER_EMR_EM3_MASK                      (0x8U)
#define CTIMER_EMR_EM3_SHIFT                     (3U)
#define CTIMER_EMR_EM3(x)                        (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EM3_SHIFT)) & CTIMER_EMR_EM3_MASK)
#define CTIMER_EMR_EMC0_MASK                     (0x30U)
#define CTIMER_EMR_EMC0_SHIFT                    (4U)
#define CTIMER_EMR_EMC0(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EMC0_SHIFT)) & CTIMER_EMR_EMC0_MASK)
#define CTIMER_EMR_EMC1_MASK                     (0xC0U)
#define CTIMER_EMR_EMC1_SHIFT                    (6U)
#define CTIMER_EMR_EMC1(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EMC1_SHIFT)) & CTIMER_EMR_EMC1_MASK)
#define CTIMER_EMR_EMC2_MASK                     (0x300U)
#define CTIMER_EMR_EMC2_SHIFT                    (8U)
#define CTIMER_EMR_EMC2(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EMC2_SHIFT)) & CTIMER_EMR_EMC2_MASK)
#define CTIMER_EMR_EMC3_MASK                     (0xC00U)
#define CTIMER_EMR_EMC3_SHIFT                    (10U)
#define CTIMER_EMR_EMC3(x)                       (((uint32_t)(((uint32_t)(x)) << CTIMER_EMR_EMC3_SHIFT)) & CTIMER_EMR_EMC3_MASK)

/*! @name CTCR - Count Control Register. The CTCR selects between Timer and Counter mode, and in Counter mode selects the signal and edge(s) for counting. */
#define CTIMER_CTCR_CTMODE_MASK                  (0x3U)
#define CTIMER_CTCR_CTMODE_SHIFT                 (0U)
#define CTIMER_CTCR_CTMODE(x)                    (((uint32_t)(((uint32_t)(x)) << CTIMER_CTCR_CTMODE_SHIFT)) & CTIMER_CTCR_CTMODE_MASK)
#define CTIMER_CTCR_CINSEL_MASK                  (0xCU)
#define CTIMER_CTCR_CINSEL_SHIFT                 (2U)
#define CTIMER_CTCR_CINSEL(x)                    (((uint32_t)(((uint32_t)(x)) << CTIMER_CTCR_CINSEL_SHIFT)) & CTIMER_CTCR_CINSEL_MASK)
#define CTIMER_CTCR_ENCC_MASK                    (0x10U)
#define CTIMER_CTCR_ENCC_SHIFT                   (4U)
#define CTIMER_CTCR_ENCC(x)                      (((uint32_t)(((uint32_t)(x)) << CTIMER_CTCR_ENCC_SHIFT)) & CTIMER_CTCR_ENCC_MASK)
#define CTIMER_CTCR_SELCC_MASK                   (0xE0U)
#define CTIMER_CTCR_SELCC_SHIFT                  (5U)
#define CTIMER_CTCR_SELCC(x)                     (((uint32_t)(((uint32_t)(x)) << CTIMER_CTCR_SELCC_SHIFT)) & CTIMER_CTCR_SELCC_MASK)

/*! @name PWMC - PWM Control Register. The PWMCON enables PWM mode for the external match pins. */
#define CTIMER_PWMC_PWMEN0_MASK                  (0x1U)
#define CTIMER_PWMC_PWMEN0_SHIFT                 (0U)
#define CTIMER_PWMC_PWMEN0(x)                    (((uint32_t)(((uint32_t)(x)) << CTIMER_PWMC_PWMEN0_SHIFT)) & CTIMER_PWMC_PWMEN0_MASK)
#define CTIMER_PWMC_PWMEN1_MASK                  (0x2U)
#define CTIMER_PWMC_PWMEN1_SHIFT                 (1U)
#define CTIMER_PWMC_PWMEN1(x)                    (((uint32_t)(((uint32_t)(x)) << CTIMER_PWMC_PWMEN1_SHIFT)) & CTIMER_PWMC_PWMEN1_MASK)
#define CTIMER_PWMC_PWMEN2_MASK                  (0x4U)
#define CTIMER_PWMC_PWMEN2_SHIFT                 (2U)
#define CTIMER_PWMC_PWMEN2(x)                    (((uint32_t)(((uint32_t)(x)) << CTIMER_PWMC_PWMEN2_SHIFT)) & CTIMER_PWMC_PWMEN2_MASK)
#define CTIMER_PWMC_PWMEN3_MASK                  (0x8U)
#define CTIMER_PWMC_PWMEN3_SHIFT                 (3U)
#define CTIMER_PWMC_PWMEN3(x)                    (((uint32_t)(((uint32_t)(x)) << CTIMER_PWMC_PWMEN3_SHIFT)) & CTIMER_PWMC_PWMEN3_MASK)


/*!
 * @}
 */ /* end of group CTIMER_Register_Masks */


/* CTIMER - Peripheral instance base addresses */
/** Peripheral CTIMER0 base address */
#define CTIMER0_BASE                             (0x40002000u)
/** Peripheral CTIMER0 base pointer */
#define CTIMER0                                  ((CTIMER_Type *)CTIMER0_BASE)
/** Peripheral CTIMER1 base address */
#define CTIMER1_BASE                             (0x40003000u)
/** Peripheral CTIMER1 base pointer */
#define CTIMER1                                  ((CTIMER_Type *)CTIMER1_BASE)
/** Peripheral CTIMER2 base address */
#define CTIMER2_BASE                             (0x40004000u)
/** Peripheral CTIMER2 base pointer */
#define CTIMER2                                  ((CTIMER_Type *)CTIMER2_BASE)
/** Peripheral CTIMER3 base address */
#define CTIMER3_BASE                             (0x40005000u)
/** Peripheral CTIMER3 base pointer */
#define CTIMER3                                  ((CTIMER_Type *)CTIMER3_BASE)
/** Array initializer of CTIMER peripheral base addresses */
#define CTIMER_BASE_ADDRS                        { CTIMER0_BASE, CTIMER1_BASE, CTIMER2_BASE, CTIMER3_BASE }
/** Array initializer of CTIMER peripheral base pointers */
#define CTIMER_BASE_PTRS                         { CTIMER0, CTIMER1, CTIMER2, CTIMER3 }
/** Interrupt vectors for the CTIMER peripheral type */
#define CTIMER_IRQS                              { CTIMER0_IRQn, CTIMER1_IRQn, CTIMER2_IRQn, CTIMER3_IRQn }

/*!
 * @}
 */ /* end of group CTIMER_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- DAC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DAC_Peripheral_Access_Layer DAC Peripheral Access Layer
 * @{
 */

/** DAC - Register Layout Typedef */
typedef struct {
  __IO uint32_t ANA_CFG;                           /**< reserved, offset: 0x0 */
  __IO uint32_t CTRL;                              /**< DAC clock invert, offset: 0x4 */
  __IO uint32_t SIN_CFG0;                          /**< sin amplitude, offset: 0x8 */
  __IO uint32_t SIN_CFG1;                          /**< reserved, offset: 0xC */
  __IO uint32_t GAIN_CTRL;                         /**< reserved, offset: 0x10 */
  __IO uint32_t CLR_TRG;                           /**< Reserved, offset: 0x14 */
  __O  uint32_t DIN;                               /**< DAC data input, offset: 0x18 */
  __IO uint32_t INT;                               /**< Reserved, offset: 0x1C */
  __IO uint32_t INTEN;                             /**< Reserved, offset: 0x20 */
  __I  uint32_t INT_STAT;                          /**< Reserved, offset: 0x24 */
  __I  uint32_t STATUS;                            /**< Reserved, offset: 0x28 */
} DAC_Type;

/* ----------------------------------------------------------------------------
   -- DAC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DAC_Register_Masks DAC Register Masks
 * @{
 */

/*! @name ANA_CFG - reserved */
#define DAC_ANA_CFG_FILTER_BM_MASK               (0x7U)
#define DAC_ANA_CFG_FILTER_BM_SHIFT              (0U)
#define DAC_ANA_CFG_FILTER_BM(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_ANA_CFG_FILTER_BM_SHIFT)) & DAC_ANA_CFG_FILTER_BM_MASK)
#define DAC_ANA_CFG_DAC_AMP_MASK                 (0x70U)
#define DAC_ANA_CFG_DAC_AMP_SHIFT                (4U)
#define DAC_ANA_CFG_DAC_AMP(x)                   (((uint32_t)(((uint32_t)(x)) << DAC_ANA_CFG_DAC_AMP_SHIFT)) & DAC_ANA_CFG_DAC_AMP_MASK)
#define DAC_ANA_CFG_FILTER_BW_MASK               (0x300U)
#define DAC_ANA_CFG_FILTER_BW_SHIFT              (8U)
#define DAC_ANA_CFG_FILTER_BW(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_ANA_CFG_FILTER_BW_SHIFT)) & DAC_ANA_CFG_FILTER_BW_MASK)
#define DAC_ANA_CFG_FILTER_150K_EN_MASK          (0x1000U)
#define DAC_ANA_CFG_FILTER_150K_EN_SHIFT         (12U)
#define DAC_ANA_CFG_FILTER_150K_EN(x)            (((uint32_t)(((uint32_t)(x)) << DAC_ANA_CFG_FILTER_150K_EN_SHIFT)) & DAC_ANA_CFG_FILTER_150K_EN_MASK)
#define DAC_ANA_CFG_VCM_MASK                     (0xF0000U)
#define DAC_ANA_CFG_VCM_SHIFT                    (16U)
#define DAC_ANA_CFG_VCM(x)                       (((uint32_t)(((uint32_t)(x)) << DAC_ANA_CFG_VCM_SHIFT)) & DAC_ANA_CFG_VCM_MASK)

/*! @name CTRL - DAC clock invert */
#define DAC_CTRL_ENABLE_MASK                     (0x1U)
#define DAC_CTRL_ENABLE_SHIFT                    (0U)
#define DAC_CTRL_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_ENABLE_SHIFT)) & DAC_CTRL_ENABLE_MASK)
#define DAC_CTRL_SIN_EN_MASK                     (0x2U)
#define DAC_CTRL_SIN_EN_SHIFT                    (1U)
#define DAC_CTRL_SIN_EN(x)                       (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_SIN_EN_SHIFT)) & DAC_CTRL_SIN_EN_MASK)
#define DAC_CTRL_MOD_EN_MASK                     (0x4U)
#define DAC_CTRL_MOD_EN_SHIFT                    (2U)
#define DAC_CTRL_MOD_EN(x)                       (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_MOD_EN_SHIFT)) & DAC_CTRL_MOD_EN_MASK)
#define DAC_CTRL_MOD_WD_MASK                     (0x8U)
#define DAC_CTRL_MOD_WD_SHIFT                    (3U)
#define DAC_CTRL_MOD_WD(x)                       (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_MOD_WD_SHIFT)) & DAC_CTRL_MOD_WD_MASK)
#define DAC_CTRL_SMPL_RATE_MASK                  (0x70U)
#define DAC_CTRL_SMPL_RATE_SHIFT                 (4U)
#define DAC_CTRL_SMPL_RATE(x)                    (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_SMPL_RATE_SHIFT)) & DAC_CTRL_SMPL_RATE_MASK)
#define DAC_CTRL_SGN_INV_MASK                    (0x80U)
#define DAC_CTRL_SGN_INV_SHIFT                   (7U)
#define DAC_CTRL_SGN_INV(x)                      (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_SGN_INV_SHIFT)) & DAC_CTRL_SGN_INV_MASK)
#define DAC_CTRL_BUF_IN_ALGN_MASK                (0x100U)
#define DAC_CTRL_BUF_IN_ALGN_SHIFT               (8U)
#define DAC_CTRL_BUF_IN_ALGN(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_BUF_IN_ALGN_SHIFT)) & DAC_CTRL_BUF_IN_ALGN_MASK)
#define DAC_CTRL_BUF_OUT_ALGN_MASK               (0x200U)
#define DAC_CTRL_BUF_OUT_ALGN_SHIFT              (9U)
#define DAC_CTRL_BUF_OUT_ALGN(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_BUF_OUT_ALGN_SHIFT)) & DAC_CTRL_BUF_OUT_ALGN_MASK)
#define DAC_CTRL_TRG_MODE_MASK                   (0x400U)
#define DAC_CTRL_TRG_MODE_SHIFT                  (10U)
#define DAC_CTRL_TRG_MODE(x)                     (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_TRG_MODE_SHIFT)) & DAC_CTRL_TRG_MODE_MASK)
#define DAC_CTRL_TRG_EDGE_MASK                   (0x1800U)
#define DAC_CTRL_TRG_EDGE_SHIFT                  (11U)
#define DAC_CTRL_TRG_EDGE(x)                     (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_TRG_EDGE_SHIFT)) & DAC_CTRL_TRG_EDGE_MASK)
#define DAC_CTRL_TRG_SEL_MASK                    (0x3F0000U)
#define DAC_CTRL_TRG_SEL_SHIFT                   (16U)
#define DAC_CTRL_TRG_SEL(x)                      (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_TRG_SEL_SHIFT)) & DAC_CTRL_TRG_SEL_MASK)
#define DAC_CTRL_CLK_DIV_MASK                    (0x7F000000U)
#define DAC_CTRL_CLK_DIV_SHIFT                   (24U)
#define DAC_CTRL_CLK_DIV(x)                      (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_CLK_DIV_SHIFT)) & DAC_CTRL_CLK_DIV_MASK)
#define DAC_CTRL_CLK_INV_MASK                    (0x80000000U)
#define DAC_CTRL_CLK_INV_SHIFT                   (31U)
#define DAC_CTRL_CLK_INV(x)                      (((uint32_t)(((uint32_t)(x)) << DAC_CTRL_CLK_INV_SHIFT)) & DAC_CTRL_CLK_INV_MASK)

/*! @name SIN_CFG0 - sin amplitude */
#define DAC_SIN_CFG0_SIN_FREQ_MASK               (0xFFFFU)
#define DAC_SIN_CFG0_SIN_FREQ_SHIFT              (0U)
#define DAC_SIN_CFG0_SIN_FREQ(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_SIN_CFG0_SIN_FREQ_SHIFT)) & DAC_SIN_CFG0_SIN_FREQ_MASK)
#define DAC_SIN_CFG0_SIN_AMP_MASK                (0xFFFF0000U)
#define DAC_SIN_CFG0_SIN_AMP_SHIFT               (16U)
#define DAC_SIN_CFG0_SIN_AMP(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_SIN_CFG0_SIN_AMP_SHIFT)) & DAC_SIN_CFG0_SIN_AMP_MASK)

/*! @name SIN_CFG1 - reserved */
#define DAC_SIN_CFG1_SIN_DC_MASK                 (0xFFFFFU)
#define DAC_SIN_CFG1_SIN_DC_SHIFT                (0U)
#define DAC_SIN_CFG1_SIN_DC(x)                   (((uint32_t)(((uint32_t)(x)) << DAC_SIN_CFG1_SIN_DC_SHIFT)) & DAC_SIN_CFG1_SIN_DC_MASK)

/*! @name GAIN_CTRL - reserved */
#define DAC_GAIN_CTRL_GAIN_CTRL_MASK             (0xFFU)
#define DAC_GAIN_CTRL_GAIN_CTRL_SHIFT            (0U)
#define DAC_GAIN_CTRL_GAIN_CTRL(x)               (((uint32_t)(((uint32_t)(x)) << DAC_GAIN_CTRL_GAIN_CTRL_SHIFT)) & DAC_GAIN_CTRL_GAIN_CTRL_MASK)

/*! @name CLR_TRG - Reserved */
#define DAC_CLR_TRG_BUF_CLR_MASK                 (0x1U)
#define DAC_CLR_TRG_BUF_CLR_SHIFT                (0U)
#define DAC_CLR_TRG_BUF_CLR(x)                   (((uint32_t)(((uint32_t)(x)) << DAC_CLR_TRG_BUF_CLR_SHIFT)) & DAC_CLR_TRG_BUF_CLR_MASK)
#define DAC_CLR_TRG_SW_TRG_MASK                  (0x2U)
#define DAC_CLR_TRG_SW_TRG_SHIFT                 (1U)
#define DAC_CLR_TRG_SW_TRG(x)                    (((uint32_t)(((uint32_t)(x)) << DAC_CLR_TRG_SW_TRG_SHIFT)) & DAC_CLR_TRG_SW_TRG_MASK)

/*! @name DIN - DAC data input */
#define DAC_DIN_DIN_MASK                         (0xFFFFFFFFU)
#define DAC_DIN_DIN_SHIFT                        (0U)
#define DAC_DIN_DIN(x)                           (((uint32_t)(((uint32_t)(x)) << DAC_DIN_DIN_SHIFT)) & DAC_DIN_DIN_MASK)

/*! @name INT - Reserved */
#define DAC_INT_BUF_NFUL_INT_MASK                (0x1U)
#define DAC_INT_BUF_NFUL_INT_SHIFT               (0U)
#define DAC_INT_BUF_NFUL_INT(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_NFUL_INT_SHIFT)) & DAC_INT_BUF_NFUL_INT_MASK)
#define DAC_INT_BUF_FUL_INT_MASK                 (0x2U)
#define DAC_INT_BUF_FUL_INT_SHIFT                (1U)
#define DAC_INT_BUF_FUL_INT(x)                   (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_FUL_INT_SHIFT)) & DAC_INT_BUF_FUL_INT_MASK)
#define DAC_INT_BUF_EMT_INT_MASK                 (0x4U)
#define DAC_INT_BUF_EMT_INT_SHIFT                (2U)
#define DAC_INT_BUF_EMT_INT(x)                   (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_EMT_INT_SHIFT)) & DAC_INT_BUF_EMT_INT_MASK)
#define DAC_INT_BUF_HEMT_INT_MASK                (0x8U)
#define DAC_INT_BUF_HEMT_INT_SHIFT               (3U)
#define DAC_INT_BUF_HEMT_INT(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_HEMT_INT_SHIFT)) & DAC_INT_BUF_HEMT_INT_MASK)
#define DAC_INT_BUF_OV_INT_MASK                  (0x10U)
#define DAC_INT_BUF_OV_INT_SHIFT                 (4U)
#define DAC_INT_BUF_OV_INT(x)                    (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_OV_INT_SHIFT)) & DAC_INT_BUF_OV_INT_MASK)
#define DAC_INT_BUF_UD_INT_MASK                  (0x20U)
#define DAC_INT_BUF_UD_INT_SHIFT                 (5U)
#define DAC_INT_BUF_UD_INT(x)                    (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_UD_INT_SHIFT)) & DAC_INT_BUF_UD_INT_MASK)
#define DAC_INT_BUF_HFUL_INT_MASK                (0x40U)
#define DAC_INT_BUF_HFUL_INT_SHIFT               (6U)
#define DAC_INT_BUF_HFUL_INT(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_INT_BUF_HFUL_INT_SHIFT)) & DAC_INT_BUF_HFUL_INT_MASK)

/*! @name INTEN - Reserved */
#define DAC_INTEN_BUF_NFUL_INTEN_MASK            (0x1U)
#define DAC_INTEN_BUF_NFUL_INTEN_SHIFT           (0U)
#define DAC_INTEN_BUF_NFUL_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_NFUL_INTEN_SHIFT)) & DAC_INTEN_BUF_NFUL_INTEN_MASK)
#define DAC_INTEN_BUF_FUL_INTEN_MASK             (0x2U)
#define DAC_INTEN_BUF_FUL_INTEN_SHIFT            (1U)
#define DAC_INTEN_BUF_FUL_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_FUL_INTEN_SHIFT)) & DAC_INTEN_BUF_FUL_INTEN_MASK)
#define DAC_INTEN_BUF_EMT_INTEN_MASK             (0x4U)
#define DAC_INTEN_BUF_EMT_INTEN_SHIFT            (2U)
#define DAC_INTEN_BUF_EMT_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_EMT_INTEN_SHIFT)) & DAC_INTEN_BUF_EMT_INTEN_MASK)
#define DAC_INTEN_BUF_HEMT_INTEN_MASK            (0x8U)
#define DAC_INTEN_BUF_HEMT_INTEN_SHIFT           (3U)
#define DAC_INTEN_BUF_HEMT_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_HEMT_INTEN_SHIFT)) & DAC_INTEN_BUF_HEMT_INTEN_MASK)
#define DAC_INTEN_BUF_OV_INTEN_MASK              (0x10U)
#define DAC_INTEN_BUF_OV_INTEN_SHIFT             (4U)
#define DAC_INTEN_BUF_OV_INTEN(x)                (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_OV_INTEN_SHIFT)) & DAC_INTEN_BUF_OV_INTEN_MASK)
#define DAC_INTEN_BUF_UD_INTEN_MASK              (0x20U)
#define DAC_INTEN_BUF_UD_INTEN_SHIFT             (5U)
#define DAC_INTEN_BUF_UD_INTEN(x)                (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_UD_INTEN_SHIFT)) & DAC_INTEN_BUF_UD_INTEN_MASK)
#define DAC_INTEN_BUF_HFUL_INTEN_MASK            (0x40U)
#define DAC_INTEN_BUF_HFUL_INTEN_SHIFT           (6U)
#define DAC_INTEN_BUF_HFUL_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << DAC_INTEN_BUF_HFUL_INTEN_SHIFT)) & DAC_INTEN_BUF_HFUL_INTEN_MASK)

/*! @name INT_STAT - Reserved */
#define DAC_INT_STAT_BUF_NFUL_INT_STAT_MASK      (0x1U)
#define DAC_INT_STAT_BUF_NFUL_INT_STAT_SHIFT     (0U)
#define DAC_INT_STAT_BUF_NFUL_INT_STAT(x)        (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_NFUL_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_NFUL_INT_STAT_MASK)
#define DAC_INT_STAT_BUF_FUL_INT_STAT_MASK       (0x2U)
#define DAC_INT_STAT_BUF_FUL_INT_STAT_SHIFT      (1U)
#define DAC_INT_STAT_BUF_FUL_INT_STAT(x)         (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_FUL_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_FUL_INT_STAT_MASK)
#define DAC_INT_STAT_BUF_EMT_INT_STAT_MASK       (0x4U)
#define DAC_INT_STAT_BUF_EMT_INT_STAT_SHIFT      (2U)
#define DAC_INT_STAT_BUF_EMT_INT_STAT(x)         (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_EMT_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_EMT_INT_STAT_MASK)
#define DAC_INT_STAT_BUF_HEMT_INT_STAT_MASK      (0x8U)
#define DAC_INT_STAT_BUF_HEMT_INT_STAT_SHIFT     (3U)
#define DAC_INT_STAT_BUF_HEMT_INT_STAT(x)        (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_HEMT_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_HEMT_INT_STAT_MASK)
#define DAC_INT_STAT_BUF_OV_INT_STAT_MASK        (0x10U)
#define DAC_INT_STAT_BUF_OV_INT_STAT_SHIFT       (4U)
#define DAC_INT_STAT_BUF_OV_INT_STAT(x)          (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_OV_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_OV_INT_STAT_MASK)
#define DAC_INT_STAT_BUF_UD_INT_STAT_MASK        (0x20U)
#define DAC_INT_STAT_BUF_UD_INT_STAT_SHIFT       (5U)
#define DAC_INT_STAT_BUF_UD_INT_STAT(x)          (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_UD_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_UD_INT_STAT_MASK)
#define DAC_INT_STAT_BUF_HFUL_INT_STAT_MASK      (0x40U)
#define DAC_INT_STAT_BUF_HFUL_INT_STAT_SHIFT     (6U)
#define DAC_INT_STAT_BUF_HFUL_INT_STAT(x)        (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_BUF_HFUL_INT_STAT_SHIFT)) & DAC_INT_STAT_BUF_HFUL_INT_STAT_MASK)
#define DAC_INT_STAT_DAC_INT_STAT_MASK           (0x10000U)
#define DAC_INT_STAT_DAC_INT_STAT_SHIFT          (16U)
#define DAC_INT_STAT_DAC_INT_STAT(x)             (((uint32_t)(((uint32_t)(x)) << DAC_INT_STAT_DAC_INT_STAT_SHIFT)) & DAC_INT_STAT_DAC_INT_STAT_MASK)

/*! @name STATUS - Reserved */
#define DAC_STATUS_BUSY_MASK                     (0x1U)
#define DAC_STATUS_BUSY_SHIFT                    (0U)
#define DAC_STATUS_BUSY(x)                       (((uint32_t)(((uint32_t)(x)) << DAC_STATUS_BUSY_SHIFT)) & DAC_STATUS_BUSY_MASK)
#define DAC_STATUS_BUF_WR_PTR_MASK               (0x70000U)
#define DAC_STATUS_BUF_WR_PTR_SHIFT              (16U)
#define DAC_STATUS_BUF_WR_PTR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_STATUS_BUF_WR_PTR_SHIFT)) & DAC_STATUS_BUF_WR_PTR_MASK)
#define DAC_STATUS_BUF_RD_PTR_MASK               (0x700000U)
#define DAC_STATUS_BUF_RD_PTR_SHIFT              (20U)
#define DAC_STATUS_BUF_RD_PTR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_STATUS_BUF_RD_PTR_SHIFT)) & DAC_STATUS_BUF_RD_PTR_MASK)


/*!
 * @}
 */ /* end of group DAC_Register_Masks */


/* DAC - Peripheral instance base addresses */
/** Peripheral DAC base address */
#define DAC_BASE                                 (0x40007400u)
/** Peripheral DAC base pointer */
#define DAC                                      ((DAC_Type *)DAC_BASE)
/** Array initializer of DAC peripheral base addresses */
#define DAC_BASE_ADDRS                           { DAC_BASE }
/** Array initializer of DAC peripheral base pointers */
#define DAC_BASE_PTRS                            { DAC }
/** Interrupt vectors for the DAC peripheral type */
#define DAC_IRQS                                 { DAC_IRQn }

/*!
 * @}
 */ /* end of group DAC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- DMA Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DMA_Peripheral_Access_Layer DMA Peripheral Access Layer
 * @{
 */

/** DMA - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL;                              /**< DMA control., offset: 0x0 */
  __I  uint32_t INTSTAT;                           /**< Interrupt status., offset: 0x4 */
  __IO uint32_t SRAMBASE;                          /**< SRAM address of the channel configuration table., offset: 0x8 */
       uint8_t RESERVED_0[20];
  struct {                                         /* offset: 0x20, array step: 0x5C */
    __IO uint32_t ENABLESET;                         /**< Channel Enable read and Set for all DMA channels., array offset: 0x20, array step: 0x5C */
         uint8_t RESERVED_0[4];
    __O  uint32_t ENABLECLR;                         /**< Channel Enable Clear for all DMA channels., array offset: 0x28, array step: 0x5C */
         uint8_t RESERVED_1[4];
    __I  uint32_t ACTIVE;                            /**< Channel Active status for all DMA channels., array offset: 0x30, array step: 0x5C */
         uint8_t RESERVED_2[4];
    __I  uint32_t BUSY;                              /**< Channel Busy status for all DMA channels., array offset: 0x38, array step: 0x5C */
         uint8_t RESERVED_3[4];
    __IO uint32_t ERRINT;                            /**< Error Interrupt status for all DMA channels., array offset: 0x40, array step: 0x5C */
         uint8_t RESERVED_4[4];
    __IO uint32_t INTENSET;                          /**< Interrupt Enable read and Set for all DMA channels., array offset: 0x48, array step: 0x5C */
         uint8_t RESERVED_5[4];
    __O  uint32_t INTENCLR;                          /**< Interrupt Enable Clear for all DMA channels., array offset: 0x50, array step: 0x5C */
         uint8_t RESERVED_6[4];
    __IO uint32_t INTA;                              /**< Interrupt A status for all DMA channels., array offset: 0x58, array step: 0x5C */
         uint8_t RESERVED_7[4];
    __IO uint32_t INTB;                              /**< Interrupt B status for all DMA channels., array offset: 0x60, array step: 0x5C */
         uint8_t RESERVED_8[4];
    __O  uint32_t SETVALID;                          /**< Set ValidPending control bits for all DMA channels., array offset: 0x68, array step: 0x5C */
         uint8_t RESERVED_9[4];
    __O  uint32_t SETTRIG;                           /**< Set Trigger control bits for all DMA channels., array offset: 0x70, array step: 0x5C */
         uint8_t RESERVED_10[4];
    __O  uint32_t ABORT;                             /**< Channel Abort control for all DMA channels., array offset: 0x78, array step: 0x5C */
  } COMMON[1];
       uint8_t RESERVED_1[900];
  struct {                                         /* offset: 0x400, array step: 0x10 */
    __IO uint32_t CFG;                               /**< Configuration register for DMA channel ., array offset: 0x400, array step: 0x10 */
    __I  uint32_t CTLSTAT;                           /**< Control and status register for DMA channel ., array offset: 0x404, array step: 0x10 */
    __IO uint32_t XFERCFG;                           /**< Transfer configuration register for DMA channel ., array offset: 0x408, array step: 0x10 */
         uint8_t RESERVED_0[4];
  } CHANNEL[20];
} DMA_Type;

/* ----------------------------------------------------------------------------
   -- DMA Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DMA_Register_Masks DMA Register Masks
 * @{
 */

/*! @name CTRL - DMA control. */
#define DMA_CTRL_ENABLE_MASK                     (0x1U)
#define DMA_CTRL_ENABLE_SHIFT                    (0U)
#define DMA_CTRL_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_ENABLE_SHIFT)) & DMA_CTRL_ENABLE_MASK)

/*! @name INTSTAT - Interrupt status. */
#define DMA_INTSTAT_ACTIVEINT_MASK               (0x2U)
#define DMA_INTSTAT_ACTIVEINT_SHIFT              (1U)
#define DMA_INTSTAT_ACTIVEINT(x)                 (((uint32_t)(((uint32_t)(x)) << DMA_INTSTAT_ACTIVEINT_SHIFT)) & DMA_INTSTAT_ACTIVEINT_MASK)
#define DMA_INTSTAT_ACTIVEERRINT_MASK            (0x4U)
#define DMA_INTSTAT_ACTIVEERRINT_SHIFT           (2U)
#define DMA_INTSTAT_ACTIVEERRINT(x)              (((uint32_t)(((uint32_t)(x)) << DMA_INTSTAT_ACTIVEERRINT_SHIFT)) & DMA_INTSTAT_ACTIVEERRINT_MASK)

/*! @name SRAMBASE - SRAM address of the channel configuration table. */
#define DMA_SRAMBASE_OFFSET_MASK                 (0xFFFFFE00U)
#define DMA_SRAMBASE_OFFSET_SHIFT                (9U)
#define DMA_SRAMBASE_OFFSET(x)                   (((uint32_t)(((uint32_t)(x)) << DMA_SRAMBASE_OFFSET_SHIFT)) & DMA_SRAMBASE_OFFSET_MASK)

/*! @name COMMON_ENABLESET - Channel Enable read and Set for all DMA channels. */
#define DMA_COMMON_ENABLESET_ENA_MASK            (0xFFFFFFFFU)
#define DMA_COMMON_ENABLESET_ENA_SHIFT           (0U)
#define DMA_COMMON_ENABLESET_ENA(x)              (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_ENABLESET_ENA_SHIFT)) & DMA_COMMON_ENABLESET_ENA_MASK)

/* The count of DMA_COMMON_ENABLESET */
#define DMA_COMMON_ENABLESET_COUNT               (1U)

/*! @name COMMON_ENABLECLR - Channel Enable Clear for all DMA channels. */
#define DMA_COMMON_ENABLECLR_CLR_MASK            (0xFFFFFFFFU)
#define DMA_COMMON_ENABLECLR_CLR_SHIFT           (0U)
#define DMA_COMMON_ENABLECLR_CLR(x)              (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_ENABLECLR_CLR_SHIFT)) & DMA_COMMON_ENABLECLR_CLR_MASK)

/* The count of DMA_COMMON_ENABLECLR */
#define DMA_COMMON_ENABLECLR_COUNT               (1U)

/*! @name COMMON_ACTIVE - Channel Active status for all DMA channels. */
#define DMA_COMMON_ACTIVE_ACT_MASK               (0xFFFFFFFFU)
#define DMA_COMMON_ACTIVE_ACT_SHIFT              (0U)
#define DMA_COMMON_ACTIVE_ACT(x)                 (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_ACTIVE_ACT_SHIFT)) & DMA_COMMON_ACTIVE_ACT_MASK)

/* The count of DMA_COMMON_ACTIVE */
#define DMA_COMMON_ACTIVE_COUNT                  (1U)

/*! @name COMMON_BUSY - Channel Busy status for all DMA channels. */
#define DMA_COMMON_BUSY_BSY_MASK                 (0xFFFFFFFFU)
#define DMA_COMMON_BUSY_BSY_SHIFT                (0U)
#define DMA_COMMON_BUSY_BSY(x)                   (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_BUSY_BSY_SHIFT)) & DMA_COMMON_BUSY_BSY_MASK)

/* The count of DMA_COMMON_BUSY */
#define DMA_COMMON_BUSY_COUNT                    (1U)

/*! @name COMMON_ERRINT - Error Interrupt status for all DMA channels. */
#define DMA_COMMON_ERRINT_ERR_MASK               (0xFFFFFFFFU)
#define DMA_COMMON_ERRINT_ERR_SHIFT              (0U)
#define DMA_COMMON_ERRINT_ERR(x)                 (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_ERRINT_ERR_SHIFT)) & DMA_COMMON_ERRINT_ERR_MASK)

/* The count of DMA_COMMON_ERRINT */
#define DMA_COMMON_ERRINT_COUNT                  (1U)

/*! @name COMMON_INTENSET - Interrupt Enable read and Set for all DMA channels. */
#define DMA_COMMON_INTENSET_INTEN_MASK           (0xFFFFFFFFU)
#define DMA_COMMON_INTENSET_INTEN_SHIFT          (0U)
#define DMA_COMMON_INTENSET_INTEN(x)             (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_INTENSET_INTEN_SHIFT)) & DMA_COMMON_INTENSET_INTEN_MASK)

/* The count of DMA_COMMON_INTENSET */
#define DMA_COMMON_INTENSET_COUNT                (1U)

/*! @name COMMON_INTENCLR - Interrupt Enable Clear for all DMA channels. */
#define DMA_COMMON_INTENCLR_CLR_MASK             (0xFFFFFFFFU)
#define DMA_COMMON_INTENCLR_CLR_SHIFT            (0U)
#define DMA_COMMON_INTENCLR_CLR(x)               (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_INTENCLR_CLR_SHIFT)) & DMA_COMMON_INTENCLR_CLR_MASK)

/* The count of DMA_COMMON_INTENCLR */
#define DMA_COMMON_INTENCLR_COUNT                (1U)

/*! @name COMMON_INTA - Interrupt A status for all DMA channels. */
#define DMA_COMMON_INTA_IA_MASK                  (0xFFFFFFFFU)
#define DMA_COMMON_INTA_IA_SHIFT                 (0U)
#define DMA_COMMON_INTA_IA(x)                    (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_INTA_IA_SHIFT)) & DMA_COMMON_INTA_IA_MASK)

/* The count of DMA_COMMON_INTA */
#define DMA_COMMON_INTA_COUNT                    (1U)

/*! @name COMMON_INTB - Interrupt B status for all DMA channels. */
#define DMA_COMMON_INTB_IB_MASK                  (0xFFFFFFFFU)
#define DMA_COMMON_INTB_IB_SHIFT                 (0U)
#define DMA_COMMON_INTB_IB(x)                    (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_INTB_IB_SHIFT)) & DMA_COMMON_INTB_IB_MASK)

/* The count of DMA_COMMON_INTB */
#define DMA_COMMON_INTB_COUNT                    (1U)

/*! @name COMMON_SETVALID - Set ValidPending control bits for all DMA channels. */
#define DMA_COMMON_SETVALID_SV_MASK              (0xFFFFFFFFU)
#define DMA_COMMON_SETVALID_SV_SHIFT             (0U)
#define DMA_COMMON_SETVALID_SV(x)                (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_SETVALID_SV_SHIFT)) & DMA_COMMON_SETVALID_SV_MASK)

/* The count of DMA_COMMON_SETVALID */
#define DMA_COMMON_SETVALID_COUNT                (1U)

/*! @name COMMON_SETTRIG - Set Trigger control bits for all DMA channels. */
#define DMA_COMMON_SETTRIG_TRIG_MASK             (0xFFFFFFFFU)
#define DMA_COMMON_SETTRIG_TRIG_SHIFT            (0U)
#define DMA_COMMON_SETTRIG_TRIG(x)               (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_SETTRIG_TRIG_SHIFT)) & DMA_COMMON_SETTRIG_TRIG_MASK)

/* The count of DMA_COMMON_SETTRIG */
#define DMA_COMMON_SETTRIG_COUNT                 (1U)

/*! @name COMMON_ABORT - Channel Abort control for all DMA channels. */
#define DMA_COMMON_ABORT_ABORTCTRL_MASK          (0xFFFFFFFFU)
#define DMA_COMMON_ABORT_ABORTCTRL_SHIFT         (0U)
#define DMA_COMMON_ABORT_ABORTCTRL(x)            (((uint32_t)(((uint32_t)(x)) << DMA_COMMON_ABORT_ABORTCTRL_SHIFT)) & DMA_COMMON_ABORT_ABORTCTRL_MASK)

/* The count of DMA_COMMON_ABORT */
#define DMA_COMMON_ABORT_COUNT                   (1U)

/*! @name CHANNEL_CFG - Configuration register for DMA channel . */
#define DMA_CHANNEL_CFG_PERIPHREQEN_MASK         (0x1U)
#define DMA_CHANNEL_CFG_PERIPHREQEN_SHIFT        (0U)
#define DMA_CHANNEL_CFG_PERIPHREQEN(x)           (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_PERIPHREQEN_SHIFT)) & DMA_CHANNEL_CFG_PERIPHREQEN_MASK)
#define DMA_CHANNEL_CFG_HWTRIGEN_MASK            (0x2U)
#define DMA_CHANNEL_CFG_HWTRIGEN_SHIFT           (1U)
#define DMA_CHANNEL_CFG_HWTRIGEN(x)              (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_HWTRIGEN_SHIFT)) & DMA_CHANNEL_CFG_HWTRIGEN_MASK)
#define DMA_CHANNEL_CFG_TRIGPOL_MASK             (0x10U)
#define DMA_CHANNEL_CFG_TRIGPOL_SHIFT            (4U)
#define DMA_CHANNEL_CFG_TRIGPOL(x)               (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_TRIGPOL_SHIFT)) & DMA_CHANNEL_CFG_TRIGPOL_MASK)
#define DMA_CHANNEL_CFG_TRIGTYPE_MASK            (0x20U)
#define DMA_CHANNEL_CFG_TRIGTYPE_SHIFT           (5U)
#define DMA_CHANNEL_CFG_TRIGTYPE(x)              (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_TRIGTYPE_SHIFT)) & DMA_CHANNEL_CFG_TRIGTYPE_MASK)
#define DMA_CHANNEL_CFG_TRIGBURST_MASK           (0x40U)
#define DMA_CHANNEL_CFG_TRIGBURST_SHIFT          (6U)
#define DMA_CHANNEL_CFG_TRIGBURST(x)             (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_TRIGBURST_SHIFT)) & DMA_CHANNEL_CFG_TRIGBURST_MASK)
#define DMA_CHANNEL_CFG_BURSTPOWER_MASK          (0xF00U)
#define DMA_CHANNEL_CFG_BURSTPOWER_SHIFT         (8U)
#define DMA_CHANNEL_CFG_BURSTPOWER(x)            (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_BURSTPOWER_SHIFT)) & DMA_CHANNEL_CFG_BURSTPOWER_MASK)
#define DMA_CHANNEL_CFG_SRCBURSTWRAP_MASK        (0x4000U)
#define DMA_CHANNEL_CFG_SRCBURSTWRAP_SHIFT       (14U)
#define DMA_CHANNEL_CFG_SRCBURSTWRAP(x)          (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_SRCBURSTWRAP_SHIFT)) & DMA_CHANNEL_CFG_SRCBURSTWRAP_MASK)
#define DMA_CHANNEL_CFG_DSTBURSTWRAP_MASK        (0x8000U)
#define DMA_CHANNEL_CFG_DSTBURSTWRAP_SHIFT       (15U)
#define DMA_CHANNEL_CFG_DSTBURSTWRAP(x)          (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_DSTBURSTWRAP_SHIFT)) & DMA_CHANNEL_CFG_DSTBURSTWRAP_MASK)
#define DMA_CHANNEL_CFG_CHPRIORITY_MASK          (0x70000U)
#define DMA_CHANNEL_CFG_CHPRIORITY_SHIFT         (16U)
#define DMA_CHANNEL_CFG_CHPRIORITY(x)            (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CFG_CHPRIORITY_SHIFT)) & DMA_CHANNEL_CFG_CHPRIORITY_MASK)

/* The count of DMA_CHANNEL_CFG */
#define DMA_CHANNEL_CFG_COUNT                    (20U)

/*! @name CHANNEL_CTLSTAT - Control and status register for DMA channel . */
#define DMA_CHANNEL_CTLSTAT_VALIDPENDING_MASK    (0x1U)
#define DMA_CHANNEL_CTLSTAT_VALIDPENDING_SHIFT   (0U)
#define DMA_CHANNEL_CTLSTAT_VALIDPENDING(x)      (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CTLSTAT_VALIDPENDING_SHIFT)) & DMA_CHANNEL_CTLSTAT_VALIDPENDING_MASK)
#define DMA_CHANNEL_CTLSTAT_TRIG_MASK            (0x4U)
#define DMA_CHANNEL_CTLSTAT_TRIG_SHIFT           (2U)
#define DMA_CHANNEL_CTLSTAT_TRIG(x)              (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_CTLSTAT_TRIG_SHIFT)) & DMA_CHANNEL_CTLSTAT_TRIG_MASK)

/* The count of DMA_CHANNEL_CTLSTAT */
#define DMA_CHANNEL_CTLSTAT_COUNT                (20U)

/*! @name CHANNEL_XFERCFG - Transfer configuration register for DMA channel . */
#define DMA_CHANNEL_XFERCFG_CFGVALID_MASK        (0x1U)
#define DMA_CHANNEL_XFERCFG_CFGVALID_SHIFT       (0U)
#define DMA_CHANNEL_XFERCFG_CFGVALID(x)          (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_CFGVALID_SHIFT)) & DMA_CHANNEL_XFERCFG_CFGVALID_MASK)
#define DMA_CHANNEL_XFERCFG_RELOAD_MASK          (0x2U)
#define DMA_CHANNEL_XFERCFG_RELOAD_SHIFT         (1U)
#define DMA_CHANNEL_XFERCFG_RELOAD(x)            (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_RELOAD_SHIFT)) & DMA_CHANNEL_XFERCFG_RELOAD_MASK)
#define DMA_CHANNEL_XFERCFG_SWTRIG_MASK          (0x4U)
#define DMA_CHANNEL_XFERCFG_SWTRIG_SHIFT         (2U)
#define DMA_CHANNEL_XFERCFG_SWTRIG(x)            (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_SWTRIG_SHIFT)) & DMA_CHANNEL_XFERCFG_SWTRIG_MASK)
#define DMA_CHANNEL_XFERCFG_CLRTRIG_MASK         (0x8U)
#define DMA_CHANNEL_XFERCFG_CLRTRIG_SHIFT        (3U)
#define DMA_CHANNEL_XFERCFG_CLRTRIG(x)           (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_CLRTRIG_SHIFT)) & DMA_CHANNEL_XFERCFG_CLRTRIG_MASK)
#define DMA_CHANNEL_XFERCFG_SETINTA_MASK         (0x10U)
#define DMA_CHANNEL_XFERCFG_SETINTA_SHIFT        (4U)
#define DMA_CHANNEL_XFERCFG_SETINTA(x)           (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_SETINTA_SHIFT)) & DMA_CHANNEL_XFERCFG_SETINTA_MASK)
#define DMA_CHANNEL_XFERCFG_SETINTB_MASK         (0x20U)
#define DMA_CHANNEL_XFERCFG_SETINTB_SHIFT        (5U)
#define DMA_CHANNEL_XFERCFG_SETINTB(x)           (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_SETINTB_SHIFT)) & DMA_CHANNEL_XFERCFG_SETINTB_MASK)
#define DMA_CHANNEL_XFERCFG_WIDTH_MASK           (0x300U)
#define DMA_CHANNEL_XFERCFG_WIDTH_SHIFT          (8U)
#define DMA_CHANNEL_XFERCFG_WIDTH(x)             (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_WIDTH_SHIFT)) & DMA_CHANNEL_XFERCFG_WIDTH_MASK)
#define DMA_CHANNEL_XFERCFG_SRCINC_MASK          (0x3000U)
#define DMA_CHANNEL_XFERCFG_SRCINC_SHIFT         (12U)
#define DMA_CHANNEL_XFERCFG_SRCINC(x)            (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_SRCINC_SHIFT)) & DMA_CHANNEL_XFERCFG_SRCINC_MASK)
#define DMA_CHANNEL_XFERCFG_DSTINC_MASK          (0xC000U)
#define DMA_CHANNEL_XFERCFG_DSTINC_SHIFT         (14U)
#define DMA_CHANNEL_XFERCFG_DSTINC(x)            (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_DSTINC_SHIFT)) & DMA_CHANNEL_XFERCFG_DSTINC_MASK)
#define DMA_CHANNEL_XFERCFG_XFERCOUNT_MASK       (0x3FF0000U)
#define DMA_CHANNEL_XFERCFG_XFERCOUNT_SHIFT      (16U)
#define DMA_CHANNEL_XFERCFG_XFERCOUNT(x)         (((uint32_t)(((uint32_t)(x)) << DMA_CHANNEL_XFERCFG_XFERCOUNT_SHIFT)) & DMA_CHANNEL_XFERCFG_XFERCOUNT_MASK)

/* The count of DMA_CHANNEL_XFERCFG */
#define DMA_CHANNEL_XFERCFG_COUNT                (20U)


/*!
 * @}
 */ /* end of group DMA_Register_Masks */


/* DMA - Peripheral instance base addresses */
/** Peripheral DMA0 base address */
#define DMA0_BASE                                (0x40082000u)
/** Peripheral DMA0 base pointer */
#define DMA0                                     ((DMA_Type *)DMA0_BASE)
/** Array initializer of DMA peripheral base addresses */
#define DMA_BASE_ADDRS                           { DMA0_BASE }
/** Array initializer of DMA peripheral base pointers */
#define DMA_BASE_PTRS                            { DMA0 }
/** Interrupt vectors for the DMA peripheral type */
#define DMA_IRQS                                 { DMA0_IRQn }

/*!
 * @}
 */ /* end of group DMA_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FLASH Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLASH_Peripheral_Access_Layer FLASH Peripheral Access Layer
 * @{
 */

/** FLASH - Register Layout Typedef */
typedef struct {
  __IO uint32_t INI_RD_EN;                         /**< flash initial read register, offset: 0x0 */
  __IO uint32_t ERASE_CTRL;                        /**< flash erase control register, offset: 0x4 */
  __IO uint32_t ERASE_TIME;                        /**< flash erase time setting register, offset: 0x8 */
  __IO uint32_t TIME_CTRL;                         /**< flash operation time setting register, offset: 0xC */
  __IO uint32_t SMART_CTRL;                        /**< smart erase control register, offset: 0x10 */
  __IO uint32_t INTEN;                             /**< interrupt enable register, offset: 0x14 */
  __IO uint32_t INT_STAT;                          /**< interrupt status register, offset: 0x18 */
  __IO uint32_t INTCLR;                            /**< interrupt clear register, offset: 0x1C */
  __I  uint32_t LOCK_STAT0;                        /**< lock control register 0, offset: 0x20 */
  __I  uint32_t LOCK_STAT1;                        /**< , offset: 0x24 */
  __I  uint32_t LOCK_STAT2;                        /**< , offset: 0x28 */
  __I  uint32_t LOCK_STAT3;                        /**< , offset: 0x2C */
  __I  uint32_t LOCK_STAT4;                        /**< , offset: 0x30 */
  __I  uint32_t LOCK_STAT5;                        /**< , offset: 0x34 */
  __I  uint32_t LOCK_STAT6;                        /**< , offset: 0x38 */
  __I  uint32_t LOCK_STAT7;                        /**< , offset: 0x3C */
  __IO uint32_t LOCK_STAT8;                        /**< , offset: 0x40 */
       uint8_t RESERVED_0[4];
  __I  uint32_t STATUS1;                           /**< , offset: 0x48 */
       uint8_t RESERVED_1[16];
  __I  uint32_t ERR_INFOL1;                        /**< , offset: 0x5C */
  __I  uint32_t ERR_INFOL2;                        /**< , offset: 0x60 */
  __I  uint32_t ERR_INFOL3;                        /**< , offset: 0x64 */
  __I  uint32_t ERR_INFOH1;                        /**< , offset: 0x68 */
  __I  uint32_t ERR_INFOH2;                        /**< , offset: 0x6C */
  __I  uint32_t ERR_INFOH3;                        /**< , offset: 0x70 */
       uint8_t RESERVED_2[52];
  __IO uint32_t DEBUG_PASSWORD;                    /**< , offset: 0xA8 */
  __IO uint32_t ERASE_PASSWORD;                    /**< , offset: 0xAC */
} FLASH_Type;

/* ----------------------------------------------------------------------------
   -- FLASH Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLASH_Register_Masks FLASH Register Masks
 * @{
 */

/*! @name INI_RD_EN - flash initial read register */
#define FLASH_INI_RD_EN_INI_RD_EN_MASK           (0x1U)
#define FLASH_INI_RD_EN_INI_RD_EN_SHIFT          (0U)
#define FLASH_INI_RD_EN_INI_RD_EN(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INI_RD_EN_INI_RD_EN_SHIFT)) & FLASH_INI_RD_EN_INI_RD_EN_MASK)

/*! @name ERASE_CTRL - flash erase control register */
#define FLASH_ERASE_CTRL_PAGE_IDXL_MASK          (0x7FU)
#define FLASH_ERASE_CTRL_PAGE_IDXL_SHIFT         (0U)
#define FLASH_ERASE_CTRL_PAGE_IDXL(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_CTRL_PAGE_IDXL_SHIFT)) & FLASH_ERASE_CTRL_PAGE_IDXL_MASK)
#define FLASH_ERASE_CTRL_PAGE_IDXH_MASK          (0x7F00U)
#define FLASH_ERASE_CTRL_PAGE_IDXH_SHIFT         (8U)
#define FLASH_ERASE_CTRL_PAGE_IDXH(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_CTRL_PAGE_IDXH_SHIFT)) & FLASH_ERASE_CTRL_PAGE_IDXH_MASK)
#define FLASH_ERASE_CTRL_HALF_ERASEL_EN_MASK     (0x10000000U)
#define FLASH_ERASE_CTRL_HALF_ERASEL_EN_SHIFT    (28U)
#define FLASH_ERASE_CTRL_HALF_ERASEL_EN(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_CTRL_HALF_ERASEL_EN_SHIFT)) & FLASH_ERASE_CTRL_HALF_ERASEL_EN_MASK)
#define FLASH_ERASE_CTRL_HALF_ERASEH_EN_MASK     (0x20000000U)
#define FLASH_ERASE_CTRL_HALF_ERASEH_EN_SHIFT    (29U)
#define FLASH_ERASE_CTRL_HALF_ERASEH_EN(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_CTRL_HALF_ERASEH_EN_SHIFT)) & FLASH_ERASE_CTRL_HALF_ERASEH_EN_MASK)
#define FLASH_ERASE_CTRL_PAGE_ERASEL_EN_MASK     (0x40000000U)
#define FLASH_ERASE_CTRL_PAGE_ERASEL_EN_SHIFT    (30U)
#define FLASH_ERASE_CTRL_PAGE_ERASEL_EN(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_CTRL_PAGE_ERASEL_EN_SHIFT)) & FLASH_ERASE_CTRL_PAGE_ERASEL_EN_MASK)
#define FLASH_ERASE_CTRL_PAGE_ERASEH_EN_MASK     (0x80000000U)
#define FLASH_ERASE_CTRL_PAGE_ERASEH_EN_SHIFT    (31U)
#define FLASH_ERASE_CTRL_PAGE_ERASEH_EN(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_CTRL_PAGE_ERASEH_EN_SHIFT)) & FLASH_ERASE_CTRL_PAGE_ERASEH_EN_MASK)

/*! @name ERASE_TIME - flash erase time setting register */
#define FLASH_ERASE_TIME_ERASE_TIME_BASE_MASK    (0xFFFFFU)
#define FLASH_ERASE_TIME_ERASE_TIME_BASE_SHIFT   (0U)
#define FLASH_ERASE_TIME_ERASE_TIME_BASE(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_TIME_ERASE_TIME_BASE_SHIFT)) & FLASH_ERASE_TIME_ERASE_TIME_BASE_MASK)

/*! @name TIME_CTRL - flash operation time setting register */
#define FLASH_TIME_CTRL_PRGM_CYCLE_MASK          (0xFFFU)
#define FLASH_TIME_CTRL_PRGM_CYCLE_SHIFT         (0U)
#define FLASH_TIME_CTRL_PRGM_CYCLE(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_TIME_CTRL_PRGM_CYCLE_SHIFT)) & FLASH_TIME_CTRL_PRGM_CYCLE_MASK)
#define FLASH_TIME_CTRL_TIME_BASE_MASK           (0xFF000U)
#define FLASH_TIME_CTRL_TIME_BASE_SHIFT          (12U)
#define FLASH_TIME_CTRL_TIME_BASE(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_TIME_CTRL_TIME_BASE_SHIFT)) & FLASH_TIME_CTRL_TIME_BASE_MASK)

/*! @name SMART_CTRL - smart erase control register */
#define FLASH_SMART_CTRL_PRGML_EN_MASK           (0x1U)
#define FLASH_SMART_CTRL_PRGML_EN_SHIFT          (0U)
#define FLASH_SMART_CTRL_PRGML_EN(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_PRGML_EN_SHIFT)) & FLASH_SMART_CTRL_PRGML_EN_MASK)
#define FLASH_SMART_CTRL_PRGMH_EN_MASK           (0x2U)
#define FLASH_SMART_CTRL_PRGMH_EN_SHIFT          (1U)
#define FLASH_SMART_CTRL_PRGMH_EN(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_PRGMH_EN_SHIFT)) & FLASH_SMART_CTRL_PRGMH_EN_MASK)
#define FLASH_SMART_CTRL_SMART_WRITEL_EN_MASK    (0x4U)
#define FLASH_SMART_CTRL_SMART_WRITEL_EN_SHIFT   (2U)
#define FLASH_SMART_CTRL_SMART_WRITEL_EN(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_SMART_WRITEL_EN_SHIFT)) & FLASH_SMART_CTRL_SMART_WRITEL_EN_MASK)
#define FLASH_SMART_CTRL_SMART_WRITEH_EN_MASK    (0x8U)
#define FLASH_SMART_CTRL_SMART_WRITEH_EN_SHIFT   (3U)
#define FLASH_SMART_CTRL_SMART_WRITEH_EN(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_SMART_WRITEH_EN_SHIFT)) & FLASH_SMART_CTRL_SMART_WRITEH_EN_MASK)
#define FLASH_SMART_CTRL_SMART_ERASEL_EN_MASK    (0x10U)
#define FLASH_SMART_CTRL_SMART_ERASEL_EN_SHIFT   (4U)
#define FLASH_SMART_CTRL_SMART_ERASEL_EN(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_SMART_ERASEL_EN_SHIFT)) & FLASH_SMART_CTRL_SMART_ERASEL_EN_MASK)
#define FLASH_SMART_CTRL_SMART_ERASEH_EN_MASK    (0x20U)
#define FLASH_SMART_CTRL_SMART_ERASEH_EN_SHIFT   (5U)
#define FLASH_SMART_CTRL_SMART_ERASEH_EN(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_SMART_ERASEH_EN_SHIFT)) & FLASH_SMART_CTRL_SMART_ERASEH_EN_MASK)
#define FLASH_SMART_CTRL_MAX_WRITE_MASK          (0xF00U)
#define FLASH_SMART_CTRL_MAX_WRITE_SHIFT         (8U)
#define FLASH_SMART_CTRL_MAX_WRITE(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_MAX_WRITE_SHIFT)) & FLASH_SMART_CTRL_MAX_WRITE_MASK)
#define FLASH_SMART_CTRL_MAX_ERASE_MASK          (0x3F000U)
#define FLASH_SMART_CTRL_MAX_ERASE_SHIFT         (12U)
#define FLASH_SMART_CTRL_MAX_ERASE(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_SMART_CTRL_MAX_ERASE_SHIFT)) & FLASH_SMART_CTRL_MAX_ERASE_MASK)

/*! @name INTEN - interrupt enable register */
#define FLASH_INTEN_AHBL_INTEN_MASK              (0x1U)
#define FLASH_INTEN_AHBL_INTEN_SHIFT             (0U)
#define FLASH_INTEN_AHBL_INTEN(x)                (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_AHBL_INTEN_SHIFT)) & FLASH_INTEN_AHBL_INTEN_MASK)
#define FLASH_INTEN_LOCKL_INTEN_MASK             (0x2U)
#define FLASH_INTEN_LOCKL_INTEN_SHIFT            (1U)
#define FLASH_INTEN_LOCKL_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_LOCKL_INTEN_SHIFT)) & FLASH_INTEN_LOCKL_INTEN_MASK)
#define FLASH_INTEN_ERASEL_INTEN_MASK            (0x4U)
#define FLASH_INTEN_ERASEL_INTEN_SHIFT           (2U)
#define FLASH_INTEN_ERASEL_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_ERASEL_INTEN_SHIFT)) & FLASH_INTEN_ERASEL_INTEN_MASK)
#define FLASH_INTEN_WRITEL_INTEN_MASK            (0x8U)
#define FLASH_INTEN_WRITEL_INTEN_SHIFT           (3U)
#define FLASH_INTEN_WRITEL_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_WRITEL_INTEN_SHIFT)) & FLASH_INTEN_WRITEL_INTEN_MASK)
#define FLASH_INTEN_WRBUFL_INTEN_MASK            (0x10U)
#define FLASH_INTEN_WRBUFL_INTEN_SHIFT           (4U)
#define FLASH_INTEN_WRBUFL_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_WRBUFL_INTEN_SHIFT)) & FLASH_INTEN_WRBUFL_INTEN_MASK)
#define FLASH_INTEN_AHBH_INTEN_MASK              (0x100U)
#define FLASH_INTEN_AHBH_INTEN_SHIFT             (8U)
#define FLASH_INTEN_AHBH_INTEN(x)                (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_AHBH_INTEN_SHIFT)) & FLASH_INTEN_AHBH_INTEN_MASK)
#define FLASH_INTEN_LOCKH_INTEN_MASK             (0x200U)
#define FLASH_INTEN_LOCKH_INTEN_SHIFT            (9U)
#define FLASH_INTEN_LOCKH_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_LOCKH_INTEN_SHIFT)) & FLASH_INTEN_LOCKH_INTEN_MASK)
#define FLASH_INTEN_ERASEH_INTEN_MASK            (0x400U)
#define FLASH_INTEN_ERASEH_INTEN_SHIFT           (10U)
#define FLASH_INTEN_ERASEH_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_ERASEH_INTEN_SHIFT)) & FLASH_INTEN_ERASEH_INTEN_MASK)
#define FLASH_INTEN_WRITEH_INTEN_MASK            (0x800U)
#define FLASH_INTEN_WRITEH_INTEN_SHIFT           (11U)
#define FLASH_INTEN_WRITEH_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_WRITEH_INTEN_SHIFT)) & FLASH_INTEN_WRITEH_INTEN_MASK)
#define FLASH_INTEN_WRBUFH_INTEN_MASK            (0x1000U)
#define FLASH_INTEN_WRBUFH_INTEN_SHIFT           (12U)
#define FLASH_INTEN_WRBUFH_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_WRBUFH_INTEN_SHIFT)) & FLASH_INTEN_WRBUFH_INTEN_MASK)
#define FLASH_INTEN_FLASH_INTEN_MASK             (0x80000000U)
#define FLASH_INTEN_FLASH_INTEN_SHIFT            (31U)
#define FLASH_INTEN_FLASH_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << FLASH_INTEN_FLASH_INTEN_SHIFT)) & FLASH_INTEN_FLASH_INTEN_MASK)

/*! @name INT_STAT - interrupt status register */
#define FLASH_INT_STAT_AHBL_INT_MASK             (0x1U)
#define FLASH_INT_STAT_AHBL_INT_SHIFT            (0U)
#define FLASH_INT_STAT_AHBL_INT(x)               (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_AHBL_INT_SHIFT)) & FLASH_INT_STAT_AHBL_INT_MASK)
#define FLASH_INT_STAT_LOCKL_INT_MASK            (0x2U)
#define FLASH_INT_STAT_LOCKL_INT_SHIFT           (1U)
#define FLASH_INT_STAT_LOCKL_INT(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_LOCKL_INT_SHIFT)) & FLASH_INT_STAT_LOCKL_INT_MASK)
#define FLASH_INT_STAT_ERASEL_INT_MASK           (0x4U)
#define FLASH_INT_STAT_ERASEL_INT_SHIFT          (2U)
#define FLASH_INT_STAT_ERASEL_INT(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_ERASEL_INT_SHIFT)) & FLASH_INT_STAT_ERASEL_INT_MASK)
#define FLASH_INT_STAT_WRITEL_INT_MASK           (0x8U)
#define FLASH_INT_STAT_WRITEL_INT_SHIFT          (3U)
#define FLASH_INT_STAT_WRITEL_INT(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_WRITEL_INT_SHIFT)) & FLASH_INT_STAT_WRITEL_INT_MASK)
#define FLASH_INT_STAT_WRBUFL_INT_MASK           (0x10U)
#define FLASH_INT_STAT_WRBUFL_INT_SHIFT          (4U)
#define FLASH_INT_STAT_WRBUFL_INT(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_WRBUFL_INT_SHIFT)) & FLASH_INT_STAT_WRBUFL_INT_MASK)
#define FLASH_INT_STAT_WRITE_FAIL_L_INT_MASK     (0x20U)
#define FLASH_INT_STAT_WRITE_FAIL_L_INT_SHIFT    (5U)
#define FLASH_INT_STAT_WRITE_FAIL_L_INT(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_WRITE_FAIL_L_INT_SHIFT)) & FLASH_INT_STAT_WRITE_FAIL_L_INT_MASK)
#define FLASH_INT_STAT_ERASE_FAIL_L_INT_MASK     (0x40U)
#define FLASH_INT_STAT_ERASE_FAIL_L_INT_SHIFT    (6U)
#define FLASH_INT_STAT_ERASE_FAIL_L_INT(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_ERASE_FAIL_L_INT_SHIFT)) & FLASH_INT_STAT_ERASE_FAIL_L_INT_MASK)
#define FLASH_INT_STAT_AHBH_INT_MASK             (0x100U)
#define FLASH_INT_STAT_AHBH_INT_SHIFT            (8U)
#define FLASH_INT_STAT_AHBH_INT(x)               (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_AHBH_INT_SHIFT)) & FLASH_INT_STAT_AHBH_INT_MASK)
#define FLASH_INT_STAT_LOCKH_INT_MASK            (0x200U)
#define FLASH_INT_STAT_LOCKH_INT_SHIFT           (9U)
#define FLASH_INT_STAT_LOCKH_INT(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_LOCKH_INT_SHIFT)) & FLASH_INT_STAT_LOCKH_INT_MASK)
#define FLASH_INT_STAT_ERASEH_INT_MASK           (0x400U)
#define FLASH_INT_STAT_ERASEH_INT_SHIFT          (10U)
#define FLASH_INT_STAT_ERASEH_INT(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_ERASEH_INT_SHIFT)) & FLASH_INT_STAT_ERASEH_INT_MASK)
#define FLASH_INT_STAT_WRITEH_INT_MASK           (0x800U)
#define FLASH_INT_STAT_WRITEH_INT_SHIFT          (11U)
#define FLASH_INT_STAT_WRITEH_INT(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_WRITEH_INT_SHIFT)) & FLASH_INT_STAT_WRITEH_INT_MASK)
#define FLASH_INT_STAT_WRBUFH_INT_MASK           (0x1000U)
#define FLASH_INT_STAT_WRBUFH_INT_SHIFT          (12U)
#define FLASH_INT_STAT_WRBUFH_INT(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_WRBUFH_INT_SHIFT)) & FLASH_INT_STAT_WRBUFH_INT_MASK)
#define FLASH_INT_STAT_WRITE_FAIL_H_INT_MASK     (0x2000U)
#define FLASH_INT_STAT_WRITE_FAIL_H_INT_SHIFT    (13U)
#define FLASH_INT_STAT_WRITE_FAIL_H_INT(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_WRITE_FAIL_H_INT_SHIFT)) & FLASH_INT_STAT_WRITE_FAIL_H_INT_MASK)
#define FLASH_INT_STAT_ERASE_FAIL_H_INT_MASK     (0x4000U)
#define FLASH_INT_STAT_ERASE_FAIL_H_INT_SHIFT    (14U)
#define FLASH_INT_STAT_ERASE_FAIL_H_INT(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_INT_STAT_ERASE_FAIL_H_INT_SHIFT)) & FLASH_INT_STAT_ERASE_FAIL_H_INT_MASK)

/*! @name INTCLR - interrupt clear register */
#define FLASH_INTCLR_AHBL_INTCLR_MASK            (0x1U)
#define FLASH_INTCLR_AHBL_INTCLR_SHIFT           (0U)
#define FLASH_INTCLR_AHBL_INTCLR(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_AHBL_INTCLR_SHIFT)) & FLASH_INTCLR_AHBL_INTCLR_MASK)
#define FLASH_INTCLR_LOCKL_INTCLR_MASK           (0x2U)
#define FLASH_INTCLR_LOCKL_INTCLR_SHIFT          (1U)
#define FLASH_INTCLR_LOCKL_INTCLR(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_LOCKL_INTCLR_SHIFT)) & FLASH_INTCLR_LOCKL_INTCLR_MASK)
#define FLASH_INTCLR_ERASEL_INTCLR_MASK          (0x4U)
#define FLASH_INTCLR_ERASEL_INTCLR_SHIFT         (2U)
#define FLASH_INTCLR_ERASEL_INTCLR(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_ERASEL_INTCLR_SHIFT)) & FLASH_INTCLR_ERASEL_INTCLR_MASK)
#define FLASH_INTCLR_WRITEL_INTCLR_MASK          (0x8U)
#define FLASH_INTCLR_WRITEL_INTCLR_SHIFT         (3U)
#define FLASH_INTCLR_WRITEL_INTCLR(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_WRITEL_INTCLR_SHIFT)) & FLASH_INTCLR_WRITEL_INTCLR_MASK)
#define FLASH_INTCLR_AHBH_INTCLR_MASK            (0x100U)
#define FLASH_INTCLR_AHBH_INTCLR_SHIFT           (8U)
#define FLASH_INTCLR_AHBH_INTCLR(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_AHBH_INTCLR_SHIFT)) & FLASH_INTCLR_AHBH_INTCLR_MASK)
#define FLASH_INTCLR_LOCKH_INTCLR_MASK           (0x200U)
#define FLASH_INTCLR_LOCKH_INTCLR_SHIFT          (9U)
#define FLASH_INTCLR_LOCKH_INTCLR(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_LOCKH_INTCLR_SHIFT)) & FLASH_INTCLR_LOCKH_INTCLR_MASK)
#define FLASH_INTCLR_ERASEH_INTCLR_MASK          (0x400U)
#define FLASH_INTCLR_ERASEH_INTCLR_SHIFT         (10U)
#define FLASH_INTCLR_ERASEH_INTCLR(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_ERASEH_INTCLR_SHIFT)) & FLASH_INTCLR_ERASEH_INTCLR_MASK)
#define FLASH_INTCLR_WRITEH_INTCLR_MASK          (0x800U)
#define FLASH_INTCLR_WRITEH_INTCLR_SHIFT         (11U)
#define FLASH_INTCLR_WRITEH_INTCLR(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_INTCLR_WRITEH_INTCLR_SHIFT)) & FLASH_INTCLR_WRITEH_INTCLR_MASK)

/*! @name LOCK_STAT0 - lock control register 0 */
#define FLASH_LOCK_STAT0_PAGE_LOCK0_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT0_PAGE_LOCK0_SHIFT        (0U)
#define FLASH_LOCK_STAT0_PAGE_LOCK0(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT0_PAGE_LOCK0_SHIFT)) & FLASH_LOCK_STAT0_PAGE_LOCK0_MASK)

/*! @name LOCK_STAT1 -  */
#define FLASH_LOCK_STAT1_PAGE_LOCK1_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT1_PAGE_LOCK1_SHIFT        (0U)
#define FLASH_LOCK_STAT1_PAGE_LOCK1(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT1_PAGE_LOCK1_SHIFT)) & FLASH_LOCK_STAT1_PAGE_LOCK1_MASK)

/*! @name LOCK_STAT2 -  */
#define FLASH_LOCK_STAT2_PAGE_LOCK2_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT2_PAGE_LOCK2_SHIFT        (0U)
#define FLASH_LOCK_STAT2_PAGE_LOCK2(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT2_PAGE_LOCK2_SHIFT)) & FLASH_LOCK_STAT2_PAGE_LOCK2_MASK)

/*! @name LOCK_STAT3 -  */
#define FLASH_LOCK_STAT3_PAGE_LOCK3_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT3_PAGE_LOCK3_SHIFT        (0U)
#define FLASH_LOCK_STAT3_PAGE_LOCK3(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT3_PAGE_LOCK3_SHIFT)) & FLASH_LOCK_STAT3_PAGE_LOCK3_MASK)

/*! @name LOCK_STAT4 -  */
#define FLASH_LOCK_STAT4_PAGE_LOCK4_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT4_PAGE_LOCK4_SHIFT        (0U)
#define FLASH_LOCK_STAT4_PAGE_LOCK4(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT4_PAGE_LOCK4_SHIFT)) & FLASH_LOCK_STAT4_PAGE_LOCK4_MASK)

/*! @name LOCK_STAT5 -  */
#define FLASH_LOCK_STAT5_PAGE_LOCK5_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT5_PAGE_LOCK5_SHIFT        (0U)
#define FLASH_LOCK_STAT5_PAGE_LOCK5(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT5_PAGE_LOCK5_SHIFT)) & FLASH_LOCK_STAT5_PAGE_LOCK5_MASK)

/*! @name LOCK_STAT6 -  */
#define FLASH_LOCK_STAT6_PAGE_LOCK6_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT6_PAGE_LOCK6_SHIFT        (0U)
#define FLASH_LOCK_STAT6_PAGE_LOCK6(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT6_PAGE_LOCK6_SHIFT)) & FLASH_LOCK_STAT6_PAGE_LOCK6_MASK)

/*! @name LOCK_STAT7 -  */
#define FLASH_LOCK_STAT7_PAGE_LOCK7_MASK         (0xFFFFFFFFU)
#define FLASH_LOCK_STAT7_PAGE_LOCK7_SHIFT        (0U)
#define FLASH_LOCK_STAT7_PAGE_LOCK7(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT7_PAGE_LOCK7_SHIFT)) & FLASH_LOCK_STAT7_PAGE_LOCK7_MASK)

/*! @name LOCK_STAT8 -  */
#define FLASH_LOCK_STAT8_MASS_ERASE_LOCK_MASK    (0x1U)
#define FLASH_LOCK_STAT8_MASS_ERASE_LOCK_SHIFT   (0U)
#define FLASH_LOCK_STAT8_MASS_ERASE_LOCK(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT8_MASS_ERASE_LOCK_SHIFT)) & FLASH_LOCK_STAT8_MASS_ERASE_LOCK_MASK)
#define FLASH_LOCK_STAT8_FSH_PROTECT_MASK        (0x2U)
#define FLASH_LOCK_STAT8_FSH_PROTECT_SHIFT       (1U)
#define FLASH_LOCK_STAT8_FSH_PROTECT(x)          (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT8_FSH_PROTECT_SHIFT)) & FLASH_LOCK_STAT8_FSH_PROTECT_MASK)
#define FLASH_LOCK_STAT8_MEM_PROTECT_MASK        (0x4U)
#define FLASH_LOCK_STAT8_MEM_PROTECT_SHIFT       (2U)
#define FLASH_LOCK_STAT8_MEM_PROTECT(x)          (((uint32_t)(((uint32_t)(x)) << FLASH_LOCK_STAT8_MEM_PROTECT_SHIFT)) & FLASH_LOCK_STAT8_MEM_PROTECT_MASK)

/*! @name STATUS1 -  */
#define FLASH_STATUS1_FSH_ERA_BUSY_L_MASK        (0x200U)
#define FLASH_STATUS1_FSH_ERA_BUSY_L_SHIFT       (9U)
#define FLASH_STATUS1_FSH_ERA_BUSY_L(x)          (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_FSH_ERA_BUSY_L_SHIFT)) & FLASH_STATUS1_FSH_ERA_BUSY_L_MASK)
#define FLASH_STATUS1_FSH_WR_BUSY_L_MASK         (0x400U)
#define FLASH_STATUS1_FSH_WR_BUSY_L_SHIFT        (10U)
#define FLASH_STATUS1_FSH_WR_BUSY_L(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_FSH_WR_BUSY_L_SHIFT)) & FLASH_STATUS1_FSH_WR_BUSY_L_MASK)
#define FLASH_STATUS1_DBG_ERA_DONE_L_MASK        (0x800U)
#define FLASH_STATUS1_DBG_ERA_DONE_L_SHIFT       (11U)
#define FLASH_STATUS1_DBG_ERA_DONE_L(x)          (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_DBG_ERA_DONE_L_SHIFT)) & FLASH_STATUS1_DBG_ERA_DONE_L_MASK)
#define FLASH_STATUS1_FSH_ERA_BUSY_H_MASK        (0x1000U)
#define FLASH_STATUS1_FSH_ERA_BUSY_H_SHIFT       (12U)
#define FLASH_STATUS1_FSH_ERA_BUSY_H(x)          (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_FSH_ERA_BUSY_H_SHIFT)) & FLASH_STATUS1_FSH_ERA_BUSY_H_MASK)
#define FLASH_STATUS1_FSH_WR_BUSY_H_MASK         (0x2000U)
#define FLASH_STATUS1_FSH_WR_BUSY_H_SHIFT        (13U)
#define FLASH_STATUS1_FSH_WR_BUSY_H(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_FSH_WR_BUSY_H_SHIFT)) & FLASH_STATUS1_FSH_WR_BUSY_H_MASK)
#define FLASH_STATUS1_DBG_ERA_DONE_H_MASK        (0x4000U)
#define FLASH_STATUS1_DBG_ERA_DONE_H_SHIFT       (14U)
#define FLASH_STATUS1_DBG_ERA_DONE_H(x)          (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_DBG_ERA_DONE_H_SHIFT)) & FLASH_STATUS1_DBG_ERA_DONE_H_MASK)
#define FLASH_STATUS1_INI_RD_DONE_MASK           (0x8000U)
#define FLASH_STATUS1_INI_RD_DONE_SHIFT          (15U)
#define FLASH_STATUS1_INI_RD_DONE(x)             (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_INI_RD_DONE_SHIFT)) & FLASH_STATUS1_INI_RD_DONE_MASK)
#define FLASH_STATUS1_FSH_STA_MASK               (0x4000000U)
#define FLASH_STATUS1_FSH_STA_SHIFT              (26U)
#define FLASH_STATUS1_FSH_STA(x)                 (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_FSH_STA_SHIFT)) & FLASH_STATUS1_FSH_STA_MASK)
#define FLASH_STATUS1_RESERVED_MASK              (0xF8000000U)
#define FLASH_STATUS1_RESERVED_SHIFT             (27U)
#define FLASH_STATUS1_RESERVED(x)                (((uint32_t)(((uint32_t)(x)) << FLASH_STATUS1_RESERVED_SHIFT)) & FLASH_STATUS1_RESERVED_MASK)

/*! @name ERR_INFOL1 -  */
#define FLASH_ERR_INFOL1_WR_FAILEDL_ADDR_MASK    (0x3FFFFU)
#define FLASH_ERR_INFOL1_WR_FAILEDL_ADDR_SHIFT   (0U)
#define FLASH_ERR_INFOL1_WR_FAILEDL_ADDR(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOL1_WR_FAILEDL_ADDR_SHIFT)) & FLASH_ERR_INFOL1_WR_FAILEDL_ADDR_MASK)
#define FLASH_ERR_INFOL1_SMART_FAILL_CTR_MASK    (0xFC0000U)
#define FLASH_ERR_INFOL1_SMART_FAILL_CTR_SHIFT   (18U)
#define FLASH_ERR_INFOL1_SMART_FAILL_CTR(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOL1_SMART_FAILL_CTR_SHIFT)) & FLASH_ERR_INFOL1_SMART_FAILL_CTR_MASK)

/*! @name ERR_INFOL2 -  */
#define FLASH_ERR_INFOL2_WR_FAILEDL_DATA_MASK    (0xFFFFFFFFU)
#define FLASH_ERR_INFOL2_WR_FAILEDL_DATA_SHIFT   (0U)
#define FLASH_ERR_INFOL2_WR_FAILEDL_DATA(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOL2_WR_FAILEDL_DATA_SHIFT)) & FLASH_ERR_INFOL2_WR_FAILEDL_DATA_MASK)

/*! @name ERR_INFOL3 -  */
#define FLASH_ERR_INFOL3_ERA_FAILEDL_INFO_MASK   (0x3FFFFU)
#define FLASH_ERR_INFOL3_ERA_FAILEDL_INFO_SHIFT  (0U)
#define FLASH_ERR_INFOL3_ERA_FAILEDL_INFO(x)     (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOL3_ERA_FAILEDL_INFO_SHIFT)) & FLASH_ERR_INFOL3_ERA_FAILEDL_INFO_MASK)

/*! @name ERR_INFOH1 -  */
#define FLASH_ERR_INFOH1_WR_FAILEDH_ADDR_MASK    (0x3FFFFU)
#define FLASH_ERR_INFOH1_WR_FAILEDH_ADDR_SHIFT   (0U)
#define FLASH_ERR_INFOH1_WR_FAILEDH_ADDR(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOH1_WR_FAILEDH_ADDR_SHIFT)) & FLASH_ERR_INFOH1_WR_FAILEDH_ADDR_MASK)
#define FLASH_ERR_INFOH1_SMART_FAILH_CTR_MASK    (0xFC0000U)
#define FLASH_ERR_INFOH1_SMART_FAILH_CTR_SHIFT   (18U)
#define FLASH_ERR_INFOH1_SMART_FAILH_CTR(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOH1_SMART_FAILH_CTR_SHIFT)) & FLASH_ERR_INFOH1_SMART_FAILH_CTR_MASK)

/*! @name ERR_INFOH2 -  */
#define FLASH_ERR_INFOH2_WR_FAILEDH_DATA_MASK    (0xFFFFFFFFU)
#define FLASH_ERR_INFOH2_WR_FAILEDH_DATA_SHIFT   (0U)
#define FLASH_ERR_INFOH2_WR_FAILEDH_DATA(x)      (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOH2_WR_FAILEDH_DATA_SHIFT)) & FLASH_ERR_INFOH2_WR_FAILEDH_DATA_MASK)

/*! @name ERR_INFOH3 -  */
#define FLASH_ERR_INFOH3_ERA_FAILEDH_INFO_MASK   (0x3FFFFU)
#define FLASH_ERR_INFOH3_ERA_FAILEDH_INFO_SHIFT  (0U)
#define FLASH_ERR_INFOH3_ERA_FAILEDH_INFO(x)     (((uint32_t)(((uint32_t)(x)) << FLASH_ERR_INFOH3_ERA_FAILEDH_INFO_SHIFT)) & FLASH_ERR_INFOH3_ERA_FAILEDH_INFO_MASK)

/*! @name DEBUG_PASSWORD -  */
#define FLASH_DEBUG_PASSWORD_DEBUG_PASSWORD_MASK (0xFFFFFFFFU)
#define FLASH_DEBUG_PASSWORD_DEBUG_PASSWORD_SHIFT (0U)
#define FLASH_DEBUG_PASSWORD_DEBUG_PASSWORD(x)   (((uint32_t)(((uint32_t)(x)) << FLASH_DEBUG_PASSWORD_DEBUG_PASSWORD_SHIFT)) & FLASH_DEBUG_PASSWORD_DEBUG_PASSWORD_MASK)

/*! @name ERASE_PASSWORD -  */
#define FLASH_ERASE_PASSWORD_ERASE_PASSWORD_MASK (0xFFFFFFFFU)
#define FLASH_ERASE_PASSWORD_ERASE_PASSWORD_SHIFT (0U)
#define FLASH_ERASE_PASSWORD_ERASE_PASSWORD(x)   (((uint32_t)(((uint32_t)(x)) << FLASH_ERASE_PASSWORD_ERASE_PASSWORD_SHIFT)) & FLASH_ERASE_PASSWORD_ERASE_PASSWORD_MASK)


/*!
 * @}
 */ /* end of group FLASH_Register_Masks */


/* FLASH - Peripheral instance base addresses */
/** Peripheral FLASH base address */
#define FLASH_BASE                               (0x40081000u)
/** Peripheral FLASH base pointer */
#define FLASH                                    ((FLASH_Type *)FLASH_BASE)
/** Array initializer of FLASH peripheral base addresses */
#define FLASH_BASE_ADDRS                         { FLASH_BASE }
/** Array initializer of FLASH peripheral base pointers */
#define FLASH_BASE_PTRS                          { FLASH }
/** Interrupt vectors for the FLASH peripheral type */
#define FLASH_IRQS                               { FLASH_IRQn }

/*!
 * @}
 */ /* end of group FLASH_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FLEXCOMM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLEXCOMM_Peripheral_Access_Layer FLEXCOMM Peripheral Access Layer
 * @{
 */

/** FLEXCOMM - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[3840];
  __IO uint32_t IOMODE;                            /**< io mode register, offset: 0xF00 */
       uint8_t RESERVED_1[244];
  __IO uint32_t PSELID;                            /**< Peripheral Select and Flexcomm ID register., offset: 0xFF8 */
  __I  uint32_t PID;                               /**< Peripheral identification register., offset: 0xFFC */
} FLEXCOMM_Type;

/* ----------------------------------------------------------------------------
   -- FLEXCOMM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLEXCOMM_Register_Masks FLEXCOMM Register Masks
 * @{
 */

/*! @name IOMODE - io mode register */
#define FLEXCOMM_IOMODE_DIO_MODE_MASK            (0x1U)
#define FLEXCOMM_IOMODE_DIO_MODE_SHIFT           (0U)
#define FLEXCOMM_IOMODE_DIO_MODE(x)              (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_IOMODE_DIO_MODE_SHIFT)) & FLEXCOMM_IOMODE_DIO_MODE_MASK)
#define FLEXCOMM_IOMODE_DIO_OEN_MASK             (0x2U)
#define FLEXCOMM_IOMODE_DIO_OEN_SHIFT            (1U)
#define FLEXCOMM_IOMODE_DIO_OEN(x)               (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_IOMODE_DIO_OEN_SHIFT)) & FLEXCOMM_IOMODE_DIO_OEN_MASK)

/*! @name PSELID - Peripheral Select and Flexcomm ID register. */
#define FLEXCOMM_PSELID_PERSEL_MASK              (0x7U)
#define FLEXCOMM_PSELID_PERSEL_SHIFT             (0U)
#define FLEXCOMM_PSELID_PERSEL(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_PERSEL_SHIFT)) & FLEXCOMM_PSELID_PERSEL_MASK)
#define FLEXCOMM_PSELID_LOCK_MASK                (0x8U)
#define FLEXCOMM_PSELID_LOCK_SHIFT               (3U)
#define FLEXCOMM_PSELID_LOCK(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_LOCK_SHIFT)) & FLEXCOMM_PSELID_LOCK_MASK)
#define FLEXCOMM_PSELID_USARTPRESENT_MASK        (0x10U)
#define FLEXCOMM_PSELID_USARTPRESENT_SHIFT       (4U)
#define FLEXCOMM_PSELID_USARTPRESENT(x)          (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_USARTPRESENT_SHIFT)) & FLEXCOMM_PSELID_USARTPRESENT_MASK)
#define FLEXCOMM_PSELID_SPIPRESENT_MASK          (0x20U)
#define FLEXCOMM_PSELID_SPIPRESENT_SHIFT         (5U)
#define FLEXCOMM_PSELID_SPIPRESENT(x)            (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_SPIPRESENT_SHIFT)) & FLEXCOMM_PSELID_SPIPRESENT_MASK)
#define FLEXCOMM_PSELID_I2CPRESENT_MASK          (0x40U)
#define FLEXCOMM_PSELID_I2CPRESENT_SHIFT         (6U)
#define FLEXCOMM_PSELID_I2CPRESENT(x)            (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_I2CPRESENT_SHIFT)) & FLEXCOMM_PSELID_I2CPRESENT_MASK)
#define FLEXCOMM_PSELID_I2SPRESENT_MASK          (0x80U)
#define FLEXCOMM_PSELID_I2SPRESENT_SHIFT         (7U)
#define FLEXCOMM_PSELID_I2SPRESENT(x)            (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_I2SPRESENT_SHIFT)) & FLEXCOMM_PSELID_I2SPRESENT_MASK)
#define FLEXCOMM_PSELID_SC3W_MASK                (0x100U)
#define FLEXCOMM_PSELID_SC3W_SHIFT               (8U)
#define FLEXCOMM_PSELID_SC3W(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_SC3W_SHIFT)) & FLEXCOMM_PSELID_SC3W_MASK)
#define FLEXCOMM_PSELID_ID_MASK                  (0xFFFFF000U)
#define FLEXCOMM_PSELID_ID_SHIFT                 (12U)
#define FLEXCOMM_PSELID_ID(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PSELID_ID_SHIFT)) & FLEXCOMM_PSELID_ID_MASK)

/*! @name PID - Peripheral identification register. */
#define FLEXCOMM_PID_Minor_Rev_MASK              (0xF00U)
#define FLEXCOMM_PID_Minor_Rev_SHIFT             (8U)
#define FLEXCOMM_PID_Minor_Rev(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PID_Minor_Rev_SHIFT)) & FLEXCOMM_PID_Minor_Rev_MASK)
#define FLEXCOMM_PID_Major_Rev_MASK              (0xF000U)
#define FLEXCOMM_PID_Major_Rev_SHIFT             (12U)
#define FLEXCOMM_PID_Major_Rev(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PID_Major_Rev_SHIFT)) & FLEXCOMM_PID_Major_Rev_MASK)
#define FLEXCOMM_PID_ID_MASK                     (0xFFFF0000U)
#define FLEXCOMM_PID_ID_SHIFT                    (16U)
#define FLEXCOMM_PID_ID(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCOMM_PID_ID_SHIFT)) & FLEXCOMM_PID_ID_MASK)


/*!
 * @}
 */ /* end of group FLEXCOMM_Register_Masks */


/* FLEXCOMM - Peripheral instance base addresses */
/** Peripheral FLEXCOMM0 base address */
#define FLEXCOMM0_BASE                           (0x40083000u)
/** Peripheral FLEXCOMM0 base pointer */
#define FLEXCOMM0                                ((FLEXCOMM_Type *)FLEXCOMM0_BASE)
/** Peripheral FLEXCOMM1 base address */
#define FLEXCOMM1_BASE                           (0x40086000u)
/** Peripheral FLEXCOMM1 base pointer */
#define FLEXCOMM1                                ((FLEXCOMM_Type *)FLEXCOMM1_BASE)
/** Peripheral FLEXCOMM2 base address */
#define FLEXCOMM2_BASE                           (0x40087000u)
/** Peripheral FLEXCOMM2 base pointer */
#define FLEXCOMM2                                ((FLEXCOMM_Type *)FLEXCOMM2_BASE)
/** Peripheral FLEXCOMM3 base address */
#define FLEXCOMM3_BASE                           (0x4008F000u)
/** Peripheral FLEXCOMM3 base pointer */
#define FLEXCOMM3                                ((FLEXCOMM_Type *)FLEXCOMM3_BASE)
/** Array initializer of FLEXCOMM peripheral base addresses */
#define FLEXCOMM_BASE_ADDRS                      { FLEXCOMM0_BASE, FLEXCOMM1_BASE, FLEXCOMM2_BASE, FLEXCOMM3_BASE }
/** Array initializer of FLEXCOMM peripheral base pointers */
#define FLEXCOMM_BASE_PTRS                       { FLEXCOMM0, FLEXCOMM1, FLEXCOMM2, FLEXCOMM3 }
/** Interrupt vectors for the FLEXCOMM peripheral type */
#define FLEXCOMM_IRQS                            { FLEXCOMM0_IRQn, FLEXCOMM1_IRQn, FLEXCOMM2_IRQn, FLEXCOMM3_IRQn }

/*!
 * @}
 */ /* end of group FLEXCOMM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FSP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FSP_Peripheral_Access_Layer FSP Peripheral Access Layer
 * @{
 */

/** FSP - Register Layout Typedef */
typedef struct {
  __IO uint32_t SYS_CTRL;                          /**< FSP system control register, offset: 0x0 */
  __I  uint32_t STATUS;                            /**< FSP status register, offset: 0x4 */
  __IO uint32_t INT;                               /**< FSP interrupt register, offset: 0x8 */
  __IO uint32_t INTEN;                             /**< FSP interrupt enable register, offset: 0xC */
       uint8_t RESERVED_0[16];
  __IO uint32_t TE_CTRL;                           /**< transmit engine control register, offset: 0x20 */
  __IO uint32_t TE_SRC_BASE;                       /**< transfer engine source data memory base register, offset: 0x24 */
  __IO uint32_t TE_DST_BASE;                       /**< transfer engine destination data memory base register, offset: 0x28 */
       uint8_t RESERVED_1[20];
  __IO uint32_t MOU_CTRL;                          /**< matrix operation unit control register, offset: 0x40 */
  __IO uint32_t MA_SRC_BASE;                       /**< matrix A source data memory base register, offset: 0x44 */
  __IO uint32_t MB_SRC_BASE;                       /**< matrix B source data memory base register, offset: 0x48 */
  __IO uint32_t MO_DST_BASE;                       /**< matrix output data memory base register, offset: 0x4C */
  __IO uint32_t MOU_SCALEA;                        /**< scale coefficient A register, offset: 0x50 */
  __IO uint32_t MOU_SCALEB;                        /**< scale coefficient B register, offset: 0x54 */
       uint8_t RESERVED_2[8];
  __IO uint32_t SE_CTRL;                           /**< stastic engine control register, offset: 0x60 */
  __IO uint32_t SE_SRC_BASE;                       /**< statistic engine source data base register, offset: 0x64 */
  __I  uint32_t SE_IDX;                            /**< max or min data index register, offset: 0x68 */
  __I  uint32_t SE_SUM;                            /**< array summary result register, offset: 0x6C */
  __I  uint32_t SE_PWR;                            /**< array power result register, offset: 0x70 */
       uint8_t RESERVED_3[12];
  __IO uint32_t COR_CTRL;                          /**< correlation control register, offset: 0x80 */
  __IO uint32_t CX_SRC_BASE;                       /**< correlation x sequence base register, offset: 0x84 */
  __IO uint32_t CY_SRC_BASE;                       /**< correlation y sequence base register, offset: 0x88 */
  __IO uint32_t CO_DST_BASE;                       /**< correlation output sequence base register, offset: 0x8C */
  __IO uint32_t COR_OFFSET;                        /**< correlation offset register, offset: 0x90 */
       uint8_t RESERVED_4[12];
  __IO uint32_t FIR_CFG_CH0;                       /**< FIR channel 0 configuration register, offset: 0xA0 */
  __IO uint32_t FIR_CFG_CH1;                       /**< FIR channel 1 configuration register, offset: 0xA4 */
  __IO uint32_t FIR_CFG_CH2;                       /**< FIR channel 2 configuration register, offset: 0xA8 */
  __IO uint32_t FIR_CFG_CH3;                       /**< FIR channel 3 configuration register, offset: 0xAC */
  __IO uint32_t FIR_CFG_CH4;                       /**< FIR channel 4 configuration register, offset: 0xB0 */
  __IO uint32_t FIR_CFG_CH5;                       /**< FIR channel 5 configuration register, offset: 0xB4 */
  __IO uint32_t FIR_CFG_CH6;                       /**< FIR channel 6 configuration register, offset: 0xB8 */
  __IO uint32_t FIR_CFG_CH7;                       /**< FIR channel 7 configuration register, offset: 0xBC */
  __IO uint32_t FIR_CFG_CH8;                       /**< FIR channel 8 configuration register, offset: 0xC0 */
       uint8_t RESERVED_5[12];
  __IO uint32_t FIR_DAT0_FX;                       /**< FIR channel 0 fix point data input &amp; output register, offset: 0xD0 */
  __IO uint32_t FIR_DAT1_FX;                       /**< FIR channel 1 fix point data input &amp; output register, offset: 0xD4 */
  __IO uint32_t FIR_DAT2_FX;                       /**< FIR channel 2 fix point data input &amp; output register, offset: 0xD8 */
  __IO uint32_t FIR_DAT3_FX;                       /**< FIR channel 3 fix point data input &amp; output register, offset: 0xDC */
  __IO uint32_t FIR_DAT4_FX;                       /**< FIR channel 4 fix point data input &amp; output register, offset: 0xE0 */
  __IO uint32_t FIR_DAT5_FX;                       /**< FIR channel 5 fix point data input &amp; output register, offset: 0xE4 */
  __IO uint32_t FIR_DAT6_FX;                       /**< FIR channel 6 fix point data input &amp; output register, offset: 0xE8 */
  __IO uint32_t FIR_DAT7_FX;                       /**< FIR channel 7 fix point data input &amp; output register, offset: 0xEC */
  __IO uint32_t FIR_DAT8_FX;                       /**< FIR channel 8 fix point data input &amp; output register, offset: 0xF0 */
       uint8_t RESERVED_6[12];
  __IO uint32_t FIR_DAT0_FL;                       /**< FIR channel 0 float point data input &amp; output register, offset: 0x100 */
  __IO uint32_t FIR_DAT1_FL;                       /**< FIR channel 1 float point data input &amp; output register, offset: 0x104 */
  __IO uint32_t FIR_DAT2_FL;                       /**< FIR channel 2 float point data input &amp; output register, offset: 0x108 */
  __IO uint32_t FIR_DAT3_FL;                       /**< FIR channel 3 float point data input &amp; output register, offset: 0x10C */
  __IO uint32_t FIR_DAT4_FL;                       /**< FIR channel 4 float point data input &amp; output register, offset: 0x110 */
  __IO uint32_t FIR_DAT5_FL;                       /**< FIR channel 5 float point data input &amp; output register, offset: 0x114 */
  __IO uint32_t FIR_DAT6_FL;                       /**< FIR channel 6 float point data input &amp; output register, offset: 0x118 */
  __IO uint32_t FIR_DAT7_FL;                       /**< FIR channel 7 float point data input &amp; output register, offset: 0x11C */
  __IO uint32_t FIR_DAT8_FL;                       /**< FIR channel 8 float point data input &amp; output register, offset: 0x120 */
       uint8_t RESERVED_7[28];
  __O  uint32_t SIN_COS_IXOX;                      /**< sin &amp; cos input fix output fix mode data address register, offset: 0x140 */
  __O  uint32_t SIN_COS_IXOL;                      /**< sin &amp; cos input fix output float mode data address register, offset: 0x144 */
  __O  uint32_t SIN_COS_ILOX;                      /**< sin &amp; cos input float output fix mode data address register, offset: 0x148 */
  __O  uint32_t SIN_COS_ILOL;                      /**< sin &amp; cos input float output float mode data address register, offset: 0x14C */
  __O  uint32_t LN_SQRT_IXOX;                      /**< LN &amp; sqrt input fix output fix mode data address register, offset: 0x150 */
  __O  uint32_t LN_SQRT_IXOL;                      /**< LN &amp; sqrt input fix output float mode data address register, offset: 0x154 */
  __O  uint32_t LN_SQRT_ILOX;                      /**< LN &amp; sqrt input float output fix mode data address register, offset: 0x158 */
  __O  uint32_t LN_SQRT_ILOL;                      /**< LN &amp; sqrt input float output float mode data address register, offset: 0x15C */
  __O  uint32_t CORDIC_T0UP_IXOX;                  /**< native cordic input fix output fix, t=0, u=1 mode data address register, offset: 0x160 */
  __O  uint32_t CORDIC_T0UP_IXOL;                  /**< native cordic input fix output float, t=0, u=1 mode data address register, offset: 0x164 */
  __O  uint32_t CORDIC_T0UP_ILOX;                  /**< native cordic input float output fix, t=0, u=1 mode data address register, offset: 0x168 */
  __O  uint32_t CORDIC_T0UP_ILOL;                  /**< native cordic input float output float, t=0, u=1 mode data address register, offset: 0x16C */
  __O  uint32_t CORDIC_T0UN_IXOX;                  /**< native cordic input fix output fix, t=0, u=-1 mode data address register, offset: 0x170 */
  __O  uint32_t CORDIC_T0UN_IXOL;                  /**< native cordic input fix output float, t=0, u=-1 mode data address register, offset: 0x174 */
  __O  uint32_t CORDIC_T0UN_ILOX;                  /**< native cordic input float output fix, t=0, u=-1 mode data address register, offset: 0x178 */
  __O  uint32_t CORDIC_T0UN_ILOL;                  /**< native cordic input float output float, t=0, u=-1 mode data address register, offset: 0x17C */
  __O  uint32_t CORDIC_T1UP_IXOX;                  /**< native cordic input fix output fix, t=1, u=1 mode data address register, offset: 0x180 */
  __O  uint32_t CORDIC_T1UP_IXOL;                  /**< native cordic input fix output float, t=1, u=1 mode data address register, offset: 0x184 */
  __O  uint32_t CORDIC_T1UP_ILOX;                  /**< native cordic input float output fix, t=1, u=1 mode data address register, offset: 0x188 */
  __O  uint32_t CORDIC_T1UP_ILOL;                  /**< native cordic input float output float, t=1, u=1 mode data address register, offset: 0x18C */
  __O  uint32_t CORDIC_T1UN_IXOX;                  /**< native cordic input fix output fix, t=1, u=-1 mode data address register, offset: 0x190 */
  __O  uint32_t CORDIC_T1UN_IXOL;                  /**< native cordic input fix output float, t=1, u=-1 mode data address register, offset: 0x194 */
  __O  uint32_t CORDIC_T1UN_ILOX;                  /**< native cordic input float output fix, t=1, u=-1 mode data address register, offset: 0x198 */
  __O  uint32_t CORDIC_T1UN_ILOL;                  /**< native cordic input float output float, t=1, u=-1 mode data address register, offset: 0x19C */
} FSP_Type;

/* ----------------------------------------------------------------------------
   -- FSP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FSP_Register_Masks FSP Register Masks
 * @{
 */

/*! @name SYS_CTRL - FSP system control register */
#define FSP_SYS_CTRL_TE_ABORT_MASK               (0x1U)
#define FSP_SYS_CTRL_TE_ABORT_SHIFT              (0U)
#define FSP_SYS_CTRL_TE_ABORT(x)                 (((uint32_t)(((uint32_t)(x)) << FSP_SYS_CTRL_TE_ABORT_SHIFT)) & FSP_SYS_CTRL_TE_ABORT_MASK)
#define FSP_SYS_CTRL_MOU_ABORT_MASK              (0x2U)
#define FSP_SYS_CTRL_MOU_ABORT_SHIFT             (1U)
#define FSP_SYS_CTRL_MOU_ABORT(x)                (((uint32_t)(((uint32_t)(x)) << FSP_SYS_CTRL_MOU_ABORT_SHIFT)) & FSP_SYS_CTRL_MOU_ABORT_MASK)
#define FSP_SYS_CTRL_SCF_ABORT_MASK              (0x4U)
#define FSP_SYS_CTRL_SCF_ABORT_SHIFT             (2U)
#define FSP_SYS_CTRL_SCF_ABORT(x)                (((uint32_t)(((uint32_t)(x)) << FSP_SYS_CTRL_SCF_ABORT_SHIFT)) & FSP_SYS_CTRL_SCF_ABORT_MASK)

/*! @name STATUS - FSP status register */
#define FSP_STATUS_FPU0_BUSY_MASK                (0x1U)
#define FSP_STATUS_FPU0_BUSY_SHIFT               (0U)
#define FSP_STATUS_FPU0_BUSY(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_STATUS_FPU0_BUSY_SHIFT)) & FSP_STATUS_FPU0_BUSY_MASK)
#define FSP_STATUS_FPU1_BUSY_MASK                (0x2U)
#define FSP_STATUS_FPU1_BUSY_SHIFT               (1U)
#define FSP_STATUS_FPU1_BUSY(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_STATUS_FPU1_BUSY_SHIFT)) & FSP_STATUS_FPU1_BUSY_MASK)
#define FSP_STATUS_FIR_READY_MASK                (0x4U)
#define FSP_STATUS_FIR_READY_SHIFT               (2U)
#define FSP_STATUS_FIR_READY(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_STATUS_FIR_READY_SHIFT)) & FSP_STATUS_FIR_READY_MASK)

/*! @name INT - FSP interrupt register */
#define FSP_INT_TE_DONE_INT_MASK                 (0x1U)
#define FSP_INT_TE_DONE_INT_SHIFT                (0U)
#define FSP_INT_TE_DONE_INT(x)                   (((uint32_t)(((uint32_t)(x)) << FSP_INT_TE_DONE_INT_SHIFT)) & FSP_INT_TE_DONE_INT_MASK)
#define FSP_INT_MOU_DONE_INT_MASK                (0x2U)
#define FSP_INT_MOU_DONE_INT_SHIFT               (1U)
#define FSP_INT_MOU_DONE_INT(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_INT_MOU_DONE_INT_SHIFT)) & FSP_INT_MOU_DONE_INT_MASK)
#define FSP_INT_SE_DONE_INT_MASK                 (0x4U)
#define FSP_INT_SE_DONE_INT_SHIFT                (2U)
#define FSP_INT_SE_DONE_INT(x)                   (((uint32_t)(((uint32_t)(x)) << FSP_INT_SE_DONE_INT_SHIFT)) & FSP_INT_SE_DONE_INT_MASK)
#define FSP_INT_COR_DONE_INT_MASK                (0x8U)
#define FSP_INT_COR_DONE_INT_SHIFT               (3U)
#define FSP_INT_COR_DONE_INT(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_INT_COR_DONE_INT_SHIFT)) & FSP_INT_COR_DONE_INT_MASK)
#define FSP_INT_FPU0_CALC_IN_ERR_INT_MASK        (0x100U)
#define FSP_INT_FPU0_CALC_IN_ERR_INT_SHIFT       (8U)
#define FSP_INT_FPU0_CALC_IN_ERR_INT(x)          (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU0_CALC_IN_ERR_INT_SHIFT)) & FSP_INT_FPU0_CALC_IN_ERR_INT_MASK)
#define FSP_INT_FPU0_CALC_OUT_ERR_INT_MASK       (0x200U)
#define FSP_INT_FPU0_CALC_OUT_ERR_INT_SHIFT      (9U)
#define FSP_INT_FPU0_CALC_OUT_ERR_INT(x)         (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU0_CALC_OUT_ERR_INT_SHIFT)) & FSP_INT_FPU0_CALC_OUT_ERR_INT_MASK)
#define FSP_INT_FPU0_DIN_OV_INT_MASK             (0x400U)
#define FSP_INT_FPU0_DIN_OV_INT_SHIFT            (10U)
#define FSP_INT_FPU0_DIN_OV_INT(x)               (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU0_DIN_OV_INT_SHIFT)) & FSP_INT_FPU0_DIN_OV_INT_MASK)
#define FSP_INT_FPU0_DOUT_OV_INT_MASK            (0x800U)
#define FSP_INT_FPU0_DOUT_OV_INT_SHIFT           (11U)
#define FSP_INT_FPU0_DOUT_OV_INT(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU0_DOUT_OV_INT_SHIFT)) & FSP_INT_FPU0_DOUT_OV_INT_MASK)
#define FSP_INT_SINGULAR_INT_MASK                (0x1000U)
#define FSP_INT_SINGULAR_INT_SHIFT               (12U)
#define FSP_INT_SINGULAR_INT(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_INT_SINGULAR_INT_SHIFT)) & FSP_INT_SINGULAR_INT_MASK)
#define FSP_INT_FPU1_CALC_IN_ERR_INT_MASK        (0x10000U)
#define FSP_INT_FPU1_CALC_IN_ERR_INT_SHIFT       (16U)
#define FSP_INT_FPU1_CALC_IN_ERR_INT(x)          (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU1_CALC_IN_ERR_INT_SHIFT)) & FSP_INT_FPU1_CALC_IN_ERR_INT_MASK)
#define FSP_INT_FPU1_CALC_OUT_ERR_INT_MASK       (0x20000U)
#define FSP_INT_FPU1_CALC_OUT_ERR_INT_SHIFT      (17U)
#define FSP_INT_FPU1_CALC_OUT_ERR_INT(x)         (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU1_CALC_OUT_ERR_INT_SHIFT)) & FSP_INT_FPU1_CALC_OUT_ERR_INT_MASK)
#define FSP_INT_FPU1_DIN_OV_INT_MASK             (0x40000U)
#define FSP_INT_FPU1_DIN_OV_INT_SHIFT            (18U)
#define FSP_INT_FPU1_DIN_OV_INT(x)               (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU1_DIN_OV_INT_SHIFT)) & FSP_INT_FPU1_DIN_OV_INT_MASK)
#define FSP_INT_FPU1_DOUT_OV_INT_MASK            (0x80000U)
#define FSP_INT_FPU1_DOUT_OV_INT_SHIFT           (19U)
#define FSP_INT_FPU1_DOUT_OV_INT(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INT_FPU1_DOUT_OV_INT_SHIFT)) & FSP_INT_FPU1_DOUT_OV_INT_MASK)
#define FSP_INT_FINV_DIN_ERR_INT_MASK            (0x100000U)
#define FSP_INT_FINV_DIN_ERR_INT_SHIFT           (20U)
#define FSP_INT_FINV_DIN_ERR_INT(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INT_FINV_DIN_ERR_INT_SHIFT)) & FSP_INT_FINV_DIN_ERR_INT_MASK)
#define FSP_INT_FINV_DOUT_OV_INT_MASK            (0x200000U)
#define FSP_INT_FINV_DOUT_OV_INT_SHIFT           (21U)
#define FSP_INT_FINV_DOUT_OV_INT(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INT_FINV_DOUT_OV_INT_SHIFT)) & FSP_INT_FINV_DOUT_OV_INT_MASK)
#define FSP_INT_FINV_ZERO_INT_MASK               (0x400000U)
#define FSP_INT_FINV_ZERO_INT_SHIFT              (22U)
#define FSP_INT_FINV_ZERO_INT(x)                 (((uint32_t)(((uint32_t)(x)) << FSP_INT_FINV_ZERO_INT_SHIFT)) & FSP_INT_FINV_ZERO_INT_MASK)
#define FSP_INT_CORDIC_DIN_ERR_MASK              (0x1000000U)
#define FSP_INT_CORDIC_DIN_ERR_SHIFT             (24U)
#define FSP_INT_CORDIC_DIN_ERR(x)                (((uint32_t)(((uint32_t)(x)) << FSP_INT_CORDIC_DIN_ERR_SHIFT)) & FSP_INT_CORDIC_DIN_ERR_MASK)
#define FSP_INT_CORDIC_DOUT_ERR_INT_MASK         (0x2000000U)
#define FSP_INT_CORDIC_DOUT_ERR_INT_SHIFT        (25U)
#define FSP_INT_CORDIC_DOUT_ERR_INT(x)           (((uint32_t)(((uint32_t)(x)) << FSP_INT_CORDIC_DOUT_ERR_INT_SHIFT)) & FSP_INT_CORDIC_DOUT_ERR_INT_MASK)
#define FSP_INT_CORDIC_CALC_ERR_INT_MASK         (0x4000000U)
#define FSP_INT_CORDIC_CALC_ERR_INT_SHIFT        (26U)
#define FSP_INT_CORDIC_CALC_ERR_INT(x)           (((uint32_t)(((uint32_t)(x)) << FSP_INT_CORDIC_CALC_ERR_INT_SHIFT)) & FSP_INT_CORDIC_CALC_ERR_INT_MASK)
#define FSP_INT_FSP_INT_MASK                     (0x80000000U)
#define FSP_INT_FSP_INT_SHIFT                    (31U)
#define FSP_INT_FSP_INT(x)                       (((uint32_t)(((uint32_t)(x)) << FSP_INT_FSP_INT_SHIFT)) & FSP_INT_FSP_INT_MASK)

/*! @name INTEN - FSP interrupt enable register */
#define FSP_INTEN_TE_DONE_INTEN_MASK             (0x1U)
#define FSP_INTEN_TE_DONE_INTEN_SHIFT            (0U)
#define FSP_INTEN_TE_DONE_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_TE_DONE_INTEN_SHIFT)) & FSP_INTEN_TE_DONE_INTEN_MASK)
#define FSP_INTEN_MOU_DONE_INTEN_MASK            (0x2U)
#define FSP_INTEN_MOU_DONE_INTEN_SHIFT           (1U)
#define FSP_INTEN_MOU_DONE_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_MOU_DONE_INTEN_SHIFT)) & FSP_INTEN_MOU_DONE_INTEN_MASK)
#define FSP_INTEN_SE_DONE_INTEN_MASK             (0x4U)
#define FSP_INTEN_SE_DONE_INTEN_SHIFT            (2U)
#define FSP_INTEN_SE_DONE_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_SE_DONE_INTEN_SHIFT)) & FSP_INTEN_SE_DONE_INTEN_MASK)
#define FSP_INTEN_COR_DONE_INTEN_MASK            (0x8U)
#define FSP_INTEN_COR_DONE_INTEN_SHIFT           (3U)
#define FSP_INTEN_COR_DONE_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_COR_DONE_INTEN_SHIFT)) & FSP_INTEN_COR_DONE_INTEN_MASK)
#define FSP_INTEN_FPU0_CALC_IN_ERR_INTEN_MASK    (0x100U)
#define FSP_INTEN_FPU0_CALC_IN_ERR_INTEN_SHIFT   (8U)
#define FSP_INTEN_FPU0_CALC_IN_ERR_INTEN(x)      (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU0_CALC_IN_ERR_INTEN_SHIFT)) & FSP_INTEN_FPU0_CALC_IN_ERR_INTEN_MASK)
#define FSP_INTEN_FPU0_CALC_OUT_ERR_INTEN_MASK   (0x200U)
#define FSP_INTEN_FPU0_CALC_OUT_ERR_INTEN_SHIFT  (9U)
#define FSP_INTEN_FPU0_CALC_OUT_ERR_INTEN(x)     (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU0_CALC_OUT_ERR_INTEN_SHIFT)) & FSP_INTEN_FPU0_CALC_OUT_ERR_INTEN_MASK)
#define FSP_INTEN_FPU0_DIN_OV_INTEN_MASK         (0x400U)
#define FSP_INTEN_FPU0_DIN_OV_INTEN_SHIFT        (10U)
#define FSP_INTEN_FPU0_DIN_OV_INTEN(x)           (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU0_DIN_OV_INTEN_SHIFT)) & FSP_INTEN_FPU0_DIN_OV_INTEN_MASK)
#define FSP_INTEN_FPU0_DOUT_OV_INTEN_MASK        (0x800U)
#define FSP_INTEN_FPU0_DOUT_OV_INTEN_SHIFT       (11U)
#define FSP_INTEN_FPU0_DOUT_OV_INTEN(x)          (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU0_DOUT_OV_INTEN_SHIFT)) & FSP_INTEN_FPU0_DOUT_OV_INTEN_MASK)
#define FSP_INTEN_SINGULAR_INTEN_MASK            (0x1000U)
#define FSP_INTEN_SINGULAR_INTEN_SHIFT           (12U)
#define FSP_INTEN_SINGULAR_INTEN(x)              (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_SINGULAR_INTEN_SHIFT)) & FSP_INTEN_SINGULAR_INTEN_MASK)
#define FSP_INTEN_FPU1_CALC_IN_ERR_INTEN_MASK    (0x10000U)
#define FSP_INTEN_FPU1_CALC_IN_ERR_INTEN_SHIFT   (16U)
#define FSP_INTEN_FPU1_CALC_IN_ERR_INTEN(x)      (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU1_CALC_IN_ERR_INTEN_SHIFT)) & FSP_INTEN_FPU1_CALC_IN_ERR_INTEN_MASK)
#define FSP_INTEN_FPU1_CALC_OUT_ERR_INTEN_MASK   (0x20000U)
#define FSP_INTEN_FPU1_CALC_OUT_ERR_INTEN_SHIFT  (17U)
#define FSP_INTEN_FPU1_CALC_OUT_ERR_INTEN(x)     (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU1_CALC_OUT_ERR_INTEN_SHIFT)) & FSP_INTEN_FPU1_CALC_OUT_ERR_INTEN_MASK)
#define FSP_INTEN_FPU1_DIN_OV_INTEN_MASK         (0x40000U)
#define FSP_INTEN_FPU1_DIN_OV_INTEN_SHIFT        (18U)
#define FSP_INTEN_FPU1_DIN_OV_INTEN(x)           (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU1_DIN_OV_INTEN_SHIFT)) & FSP_INTEN_FPU1_DIN_OV_INTEN_MASK)
#define FSP_INTEN_FPU1_DOUT_OV_INTEN_MASK        (0x80000U)
#define FSP_INTEN_FPU1_DOUT_OV_INTEN_SHIFT       (19U)
#define FSP_INTEN_FPU1_DOUT_OV_INTEN(x)          (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FPU1_DOUT_OV_INTEN_SHIFT)) & FSP_INTEN_FPU1_DOUT_OV_INTEN_MASK)
#define FSP_INTEN_FINV_DIN_ERR_INTEN_MASK        (0x100000U)
#define FSP_INTEN_FINV_DIN_ERR_INTEN_SHIFT       (20U)
#define FSP_INTEN_FINV_DIN_ERR_INTEN(x)          (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FINV_DIN_ERR_INTEN_SHIFT)) & FSP_INTEN_FINV_DIN_ERR_INTEN_MASK)
#define FSP_INTEN_FINV_DOUT_OV_INTEN_MASK        (0x200000U)
#define FSP_INTEN_FINV_DOUT_OV_INTEN_SHIFT       (21U)
#define FSP_INTEN_FINV_DOUT_OV_INTEN(x)          (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FINV_DOUT_OV_INTEN_SHIFT)) & FSP_INTEN_FINV_DOUT_OV_INTEN_MASK)
#define FSP_INTEN_FINV_ZERO_INTEN_MASK           (0x400000U)
#define FSP_INTEN_FINV_ZERO_INTEN_SHIFT          (22U)
#define FSP_INTEN_FINV_ZERO_INTEN(x)             (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FINV_ZERO_INTEN_SHIFT)) & FSP_INTEN_FINV_ZERO_INTEN_MASK)
#define FSP_INTEN_CORDIC_DIN_ERR_INTEN_MASK      (0x1000000U)
#define FSP_INTEN_CORDIC_DIN_ERR_INTEN_SHIFT     (24U)
#define FSP_INTEN_CORDIC_DIN_ERR_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_CORDIC_DIN_ERR_INTEN_SHIFT)) & FSP_INTEN_CORDIC_DIN_ERR_INTEN_MASK)
#define FSP_INTEN_CORDIC_DOUT_ERR_INTEN_MASK     (0x2000000U)
#define FSP_INTEN_CORDIC_DOUT_ERR_INTEN_SHIFT    (25U)
#define FSP_INTEN_CORDIC_DOUT_ERR_INTEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_CORDIC_DOUT_ERR_INTEN_SHIFT)) & FSP_INTEN_CORDIC_DOUT_ERR_INTEN_MASK)
#define FSP_INTEN_CORDIC_CALC_ERR_INTEN_MASK     (0x4000000U)
#define FSP_INTEN_CORDIC_CALC_ERR_INTEN_SHIFT    (26U)
#define FSP_INTEN_CORDIC_CALC_ERR_INTEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_CORDIC_CALC_ERR_INTEN_SHIFT)) & FSP_INTEN_CORDIC_CALC_ERR_INTEN_MASK)
#define FSP_INTEN_FSP_INTEN_MASK                 (0x80000000U)
#define FSP_INTEN_FSP_INTEN_SHIFT                (31U)
#define FSP_INTEN_FSP_INTEN(x)                   (((uint32_t)(((uint32_t)(x)) << FSP_INTEN_FSP_INTEN_SHIFT)) & FSP_INTEN_FSP_INTEN_MASK)

/*! @name TE_CTRL - transmit engine control register */
#define FSP_TE_CTRL_TE_MODE_MASK                 (0x3U)
#define FSP_TE_CTRL_TE_MODE_SHIFT                (0U)
#define FSP_TE_CTRL_TE_MODE(x)                   (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_MODE_SHIFT)) & FSP_TE_CTRL_TE_MODE_MASK)
#define FSP_TE_CTRL_TE_IO_MODE_MASK              (0xCU)
#define FSP_TE_CTRL_TE_IO_MODE_SHIFT             (2U)
#define FSP_TE_CTRL_TE_IO_MODE(x)                (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_IO_MODE_SHIFT)) & FSP_TE_CTRL_TE_IO_MODE_MASK)
#define FSP_TE_CTRL_TE_PTS_MASK                  (0x30U)
#define FSP_TE_CTRL_TE_PTS_SHIFT                 (4U)
#define FSP_TE_CTRL_TE_PTS(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_PTS_SHIFT)) & FSP_TE_CTRL_TE_PTS_MASK)
#define FSP_TE_CTRL_TE_DIN_FP_SEL_MASK           (0x40U)
#define FSP_TE_CTRL_TE_DIN_FP_SEL_SHIFT          (6U)
#define FSP_TE_CTRL_TE_DIN_FP_SEL(x)             (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_DIN_FP_SEL_SHIFT)) & FSP_TE_CTRL_TE_DIN_FP_SEL_MASK)
#define FSP_TE_CTRL_TE_DOUT_FP_SEL_MASK          (0x80U)
#define FSP_TE_CTRL_TE_DOUT_FP_SEL_SHIFT         (7U)
#define FSP_TE_CTRL_TE_DOUT_FP_SEL(x)            (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_DOUT_FP_SEL_SHIFT)) & FSP_TE_CTRL_TE_DOUT_FP_SEL_MASK)
#define FSP_TE_CTRL_TE_SCALE_MASK                (0xFF00U)
#define FSP_TE_CTRL_TE_SCALE_SHIFT               (8U)
#define FSP_TE_CTRL_TE_SCALE(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_SCALE_SHIFT)) & FSP_TE_CTRL_TE_SCALE_MASK)
#define FSP_TE_CTRL_TE_PAUSE_LVL_MASK            (0x7000000U)
#define FSP_TE_CTRL_TE_PAUSE_LVL_SHIFT           (24U)
#define FSP_TE_CTRL_TE_PAUSE_LVL(x)              (((uint32_t)(((uint32_t)(x)) << FSP_TE_CTRL_TE_PAUSE_LVL_SHIFT)) & FSP_TE_CTRL_TE_PAUSE_LVL_MASK)

/*! @name TE_SRC_BASE - transfer engine source data memory base register */
#define FSP_TE_SRC_BASE_TE_SRC_BASE_MASK         (0x1FFFFU)
#define FSP_TE_SRC_BASE_TE_SRC_BASE_SHIFT        (0U)
#define FSP_TE_SRC_BASE_TE_SRC_BASE(x)           (((uint32_t)(((uint32_t)(x)) << FSP_TE_SRC_BASE_TE_SRC_BASE_SHIFT)) & FSP_TE_SRC_BASE_TE_SRC_BASE_MASK)

/*! @name TE_DST_BASE - transfer engine destination data memory base register */
#define FSP_TE_DST_BASE_TE_DST_BASE_MASK         (0x1FFFFU)
#define FSP_TE_DST_BASE_TE_DST_BASE_SHIFT        (0U)
#define FSP_TE_DST_BASE_TE_DST_BASE(x)           (((uint32_t)(((uint32_t)(x)) << FSP_TE_DST_BASE_TE_DST_BASE_SHIFT)) & FSP_TE_DST_BASE_TE_DST_BASE_MASK)

/*! @name MOU_CTRL - matrix operation unit control register */
#define FSP_MOU_CTRL_OP_MODE_MASK                (0xFU)
#define FSP_MOU_CTRL_OP_MODE_SHIFT               (0U)
#define FSP_MOU_CTRL_OP_MODE(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_OP_MODE_SHIFT)) & FSP_MOU_CTRL_OP_MODE_MASK)
#define FSP_MOU_CTRL_MOU_DIN_FP_SEL_MASK         (0x100U)
#define FSP_MOU_CTRL_MOU_DIN_FP_SEL_SHIFT        (8U)
#define FSP_MOU_CTRL_MOU_DIN_FP_SEL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_MOU_DIN_FP_SEL_SHIFT)) & FSP_MOU_CTRL_MOU_DIN_FP_SEL_MASK)
#define FSP_MOU_CTRL_MOU_DOUT_FP_SEL_MASK        (0x200U)
#define FSP_MOU_CTRL_MOU_DOUT_FP_SEL_SHIFT       (9U)
#define FSP_MOU_CTRL_MOU_DOUT_FP_SEL(x)          (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_MOU_DOUT_FP_SEL_SHIFT)) & FSP_MOU_CTRL_MOU_DOUT_FP_SEL_MASK)
#define FSP_MOU_CTRL_MAT_M_MASK                  (0xF0000U)
#define FSP_MOU_CTRL_MAT_M_SHIFT                 (16U)
#define FSP_MOU_CTRL_MAT_M(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_MAT_M_SHIFT)) & FSP_MOU_CTRL_MAT_M_MASK)
#define FSP_MOU_CTRL_MAT_N_MASK                  (0xF00000U)
#define FSP_MOU_CTRL_MAT_N_SHIFT                 (20U)
#define FSP_MOU_CTRL_MAT_N(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_MAT_N_SHIFT)) & FSP_MOU_CTRL_MAT_N_MASK)
#define FSP_MOU_CTRL_MAT_K_MASK                  (0xF000000U)
#define FSP_MOU_CTRL_MAT_K_SHIFT                 (24U)
#define FSP_MOU_CTRL_MAT_K(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_MAT_K_SHIFT)) & FSP_MOU_CTRL_MAT_K_MASK)
#define FSP_MOU_CTRL_DIV_EPSILON_MASK            (0x30000000U)
#define FSP_MOU_CTRL_DIV_EPSILON_SHIFT           (28U)
#define FSP_MOU_CTRL_DIV_EPSILON(x)              (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_DIV_EPSILON_SHIFT)) & FSP_MOU_CTRL_DIV_EPSILON_MASK)
#define FSP_MOU_CTRL_LU_STOP_MASK                (0x40000000U)
#define FSP_MOU_CTRL_LU_STOP_SHIFT               (30U)
#define FSP_MOU_CTRL_LU_STOP(x)                  (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_LU_STOP_SHIFT)) & FSP_MOU_CTRL_LU_STOP_MASK)
#define FSP_MOU_CTRL_UINV_STOP_MASK              (0x80000000U)
#define FSP_MOU_CTRL_UINV_STOP_SHIFT             (31U)
#define FSP_MOU_CTRL_UINV_STOP(x)                (((uint32_t)(((uint32_t)(x)) << FSP_MOU_CTRL_UINV_STOP_SHIFT)) & FSP_MOU_CTRL_UINV_STOP_MASK)

/*! @name MA_SRC_BASE - matrix A source data memory base register */
#define FSP_MA_SRC_BASE_MA_SRC_BASE_MASK         (0x1FFFFU)
#define FSP_MA_SRC_BASE_MA_SRC_BASE_SHIFT        (0U)
#define FSP_MA_SRC_BASE_MA_SRC_BASE(x)           (((uint32_t)(((uint32_t)(x)) << FSP_MA_SRC_BASE_MA_SRC_BASE_SHIFT)) & FSP_MA_SRC_BASE_MA_SRC_BASE_MASK)

/*! @name MB_SRC_BASE - matrix B source data memory base register */
#define FSP_MB_SRC_BASE_MB_SRC_BASE_MASK         (0x1FFFFU)
#define FSP_MB_SRC_BASE_MB_SRC_BASE_SHIFT        (0U)
#define FSP_MB_SRC_BASE_MB_SRC_BASE(x)           (((uint32_t)(((uint32_t)(x)) << FSP_MB_SRC_BASE_MB_SRC_BASE_SHIFT)) & FSP_MB_SRC_BASE_MB_SRC_BASE_MASK)

/*! @name MO_DST_BASE - matrix output data memory base register */
#define FSP_MO_DST_BASE_MO_DST_BASE_MASK         (0x1FFFFU)
#define FSP_MO_DST_BASE_MO_DST_BASE_SHIFT        (0U)
#define FSP_MO_DST_BASE_MO_DST_BASE(x)           (((uint32_t)(((uint32_t)(x)) << FSP_MO_DST_BASE_MO_DST_BASE_SHIFT)) & FSP_MO_DST_BASE_MO_DST_BASE_MASK)

/*! @name MOU_SCALEA - scale coefficient A register */
#define FSP_MOU_SCALEA_MOU_SCALEA_MASK           (0xFFFFFFFFU)
#define FSP_MOU_SCALEA_MOU_SCALEA_SHIFT          (0U)
#define FSP_MOU_SCALEA_MOU_SCALEA(x)             (((uint32_t)(((uint32_t)(x)) << FSP_MOU_SCALEA_MOU_SCALEA_SHIFT)) & FSP_MOU_SCALEA_MOU_SCALEA_MASK)

/*! @name MOU_SCALEB - scale coefficient B register */
#define FSP_MOU_SCALEB_MOU_SCALEB_MASK           (0xFFFFFFFFU)
#define FSP_MOU_SCALEB_MOU_SCALEB_SHIFT          (0U)
#define FSP_MOU_SCALEB_MOU_SCALEB(x)             (((uint32_t)(((uint32_t)(x)) << FSP_MOU_SCALEB_MOU_SCALEB_SHIFT)) & FSP_MOU_SCALEB_MOU_SCALEB_MASK)

/*! @name SE_CTRL - stastic engine control register */
#define FSP_SE_CTRL_MIN_SEL_MASK                 (0x1U)
#define FSP_SE_CTRL_MIN_SEL_SHIFT                (0U)
#define FSP_SE_CTRL_MIN_SEL(x)                   (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_MIN_SEL_SHIFT)) & FSP_SE_CTRL_MIN_SEL_MASK)
#define FSP_SE_CTRL_MAX_SEL_MASK                 (0x2U)
#define FSP_SE_CTRL_MAX_SEL_SHIFT                (1U)
#define FSP_SE_CTRL_MAX_SEL(x)                   (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_MAX_SEL_SHIFT)) & FSP_SE_CTRL_MAX_SEL_MASK)
#define FSP_SE_CTRL_MIN_IDX_EN_MASK              (0x4U)
#define FSP_SE_CTRL_MIN_IDX_EN_SHIFT             (2U)
#define FSP_SE_CTRL_MIN_IDX_EN(x)                (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_MIN_IDX_EN_SHIFT)) & FSP_SE_CTRL_MIN_IDX_EN_MASK)
#define FSP_SE_CTRL_MAX_IDX_EN_MASK              (0x8U)
#define FSP_SE_CTRL_MAX_IDX_EN_SHIFT             (3U)
#define FSP_SE_CTRL_MAX_IDX_EN(x)                (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_MAX_IDX_EN_SHIFT)) & FSP_SE_CTRL_MAX_IDX_EN_MASK)
#define FSP_SE_CTRL_SUM_EN_MASK                  (0x10U)
#define FSP_SE_CTRL_SUM_EN_SHIFT                 (4U)
#define FSP_SE_CTRL_SUM_EN(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_SUM_EN_SHIFT)) & FSP_SE_CTRL_SUM_EN_MASK)
#define FSP_SE_CTRL_PWR_EN_MASK                  (0x20U)
#define FSP_SE_CTRL_PWR_EN_SHIFT                 (5U)
#define FSP_SE_CTRL_PWR_EN(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_PWR_EN_SHIFT)) & FSP_SE_CTRL_PWR_EN_MASK)
#define FSP_SE_CTRL_SE_DIN_FP_SEL_MASK           (0x40U)
#define FSP_SE_CTRL_SE_DIN_FP_SEL_SHIFT          (6U)
#define FSP_SE_CTRL_SE_DIN_FP_SEL(x)             (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_SE_DIN_FP_SEL_SHIFT)) & FSP_SE_CTRL_SE_DIN_FP_SEL_MASK)
#define FSP_SE_CTRL_SE_DOUT_FP_SEL_MASK          (0x80U)
#define FSP_SE_CTRL_SE_DOUT_FP_SEL_SHIFT         (7U)
#define FSP_SE_CTRL_SE_DOUT_FP_SEL(x)            (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_SE_DOUT_FP_SEL_SHIFT)) & FSP_SE_CTRL_SE_DOUT_FP_SEL_MASK)
#define FSP_SE_CTRL_SE_LEN_MASK                  (0xFF0000U)
#define FSP_SE_CTRL_SE_LEN_SHIFT                 (16U)
#define FSP_SE_CTRL_SE_LEN(x)                    (((uint32_t)(((uint32_t)(x)) << FSP_SE_CTRL_SE_LEN_SHIFT)) & FSP_SE_CTRL_SE_LEN_MASK)

/*! @name SE_SRC_BASE - statistic engine source data base register */
#define FSP_SE_SRC_BASE_SE_SRC_BASE_MASK         (0x1FFFFU)
#define FSP_SE_SRC_BASE_SE_SRC_BASE_SHIFT        (0U)
#define FSP_SE_SRC_BASE_SE_SRC_BASE(x)           (((uint32_t)(((uint32_t)(x)) << FSP_SE_SRC_BASE_SE_SRC_BASE_SHIFT)) & FSP_SE_SRC_BASE_SE_SRC_BASE_MASK)

/*! @name SE_IDX - max or min data index register */
#define FSP_SE_IDX_SE_MIN_IDX_MASK               (0xFFU)
#define FSP_SE_IDX_SE_MIN_IDX_SHIFT              (0U)
#define FSP_SE_IDX_SE_MIN_IDX(x)                 (((uint32_t)(((uint32_t)(x)) << FSP_SE_IDX_SE_MIN_IDX_SHIFT)) & FSP_SE_IDX_SE_MIN_IDX_MASK)
#define FSP_SE_IDX_SE_MAX_IDX_MASK               (0xFF0000U)
#define FSP_SE_IDX_SE_MAX_IDX_SHIFT              (16U)
#define FSP_SE_IDX_SE_MAX_IDX(x)                 (((uint32_t)(((uint32_t)(x)) << FSP_SE_IDX_SE_MAX_IDX_SHIFT)) & FSP_SE_IDX_SE_MAX_IDX_MASK)

/*! @name SE_SUM - array summary result register */
#define FSP_SE_SUM_SE_SUM_MASK                   (0xFFFFFFFFU)
#define FSP_SE_SUM_SE_SUM_SHIFT                  (0U)
#define FSP_SE_SUM_SE_SUM(x)                     (((uint32_t)(((uint32_t)(x)) << FSP_SE_SUM_SE_SUM_SHIFT)) & FSP_SE_SUM_SE_SUM_MASK)

/*! @name SE_PWR - array power result register */
#define FSP_SE_PWR_SE_PWR_MASK                   (0xFFFFFFFFU)
#define FSP_SE_PWR_SE_PWR_SHIFT                  (0U)
#define FSP_SE_PWR_SE_PWR(x)                     (((uint32_t)(((uint32_t)(x)) << FSP_SE_PWR_SE_PWR_SHIFT)) & FSP_SE_PWR_SE_PWR_MASK)

/*! @name COR_CTRL - correlation control register */
#define FSP_COR_CTRL_COR_DIN_FP_SEL_MASK         (0x100U)
#define FSP_COR_CTRL_COR_DIN_FP_SEL_SHIFT        (8U)
#define FSP_COR_CTRL_COR_DIN_FP_SEL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_COR_CTRL_COR_DIN_FP_SEL_SHIFT)) & FSP_COR_CTRL_COR_DIN_FP_SEL_MASK)
#define FSP_COR_CTRL_COR_DOUT_FP_SEL_MASK        (0x200U)
#define FSP_COR_CTRL_COR_DOUT_FP_SEL_SHIFT       (9U)
#define FSP_COR_CTRL_COR_DOUT_FP_SEL(x)          (((uint32_t)(((uint32_t)(x)) << FSP_COR_CTRL_COR_DOUT_FP_SEL_SHIFT)) & FSP_COR_CTRL_COR_DOUT_FP_SEL_MASK)
#define FSP_COR_CTRL_COR_X_LEN_MASK              (0xFF0000U)
#define FSP_COR_CTRL_COR_X_LEN_SHIFT             (16U)
#define FSP_COR_CTRL_COR_X_LEN(x)                (((uint32_t)(((uint32_t)(x)) << FSP_COR_CTRL_COR_X_LEN_SHIFT)) & FSP_COR_CTRL_COR_X_LEN_MASK)
#define FSP_COR_CTRL_COR_Y_LEN_MASK              (0xFF000000U)
#define FSP_COR_CTRL_COR_Y_LEN_SHIFT             (24U)
#define FSP_COR_CTRL_COR_Y_LEN(x)                (((uint32_t)(((uint32_t)(x)) << FSP_COR_CTRL_COR_Y_LEN_SHIFT)) & FSP_COR_CTRL_COR_Y_LEN_MASK)

/*! @name CX_SRC_BASE - correlation x sequence base register */
#define FSP_CX_SRC_BASE_COR_X_ADDR_MASK          (0x1FFFFU)
#define FSP_CX_SRC_BASE_COR_X_ADDR_SHIFT         (0U)
#define FSP_CX_SRC_BASE_COR_X_ADDR(x)            (((uint32_t)(((uint32_t)(x)) << FSP_CX_SRC_BASE_COR_X_ADDR_SHIFT)) & FSP_CX_SRC_BASE_COR_X_ADDR_MASK)

/*! @name CY_SRC_BASE - correlation y sequence base register */
#define FSP_CY_SRC_BASE_COR_Y_ADDR_MASK          (0x1FFFFU)
#define FSP_CY_SRC_BASE_COR_Y_ADDR_SHIFT         (0U)
#define FSP_CY_SRC_BASE_COR_Y_ADDR(x)            (((uint32_t)(((uint32_t)(x)) << FSP_CY_SRC_BASE_COR_Y_ADDR_SHIFT)) & FSP_CY_SRC_BASE_COR_Y_ADDR_MASK)

/*! @name CO_DST_BASE - correlation output sequence base register */
#define FSP_CO_DST_BASE_COR_DST_BASE_MASK        (0x1FFFFU)
#define FSP_CO_DST_BASE_COR_DST_BASE_SHIFT       (0U)
#define FSP_CO_DST_BASE_COR_DST_BASE(x)          (((uint32_t)(((uint32_t)(x)) << FSP_CO_DST_BASE_COR_DST_BASE_SHIFT)) & FSP_CO_DST_BASE_COR_DST_BASE_MASK)

/*! @name COR_OFFSET - correlation offset register */
#define FSP_COR_OFFSET_COR_X_OFFSET_MASK         (0xFFU)
#define FSP_COR_OFFSET_COR_X_OFFSET_SHIFT        (0U)
#define FSP_COR_OFFSET_COR_X_OFFSET(x)           (((uint32_t)(((uint32_t)(x)) << FSP_COR_OFFSET_COR_X_OFFSET_SHIFT)) & FSP_COR_OFFSET_COR_X_OFFSET_MASK)
#define FSP_COR_OFFSET_COR_Y_OFFSET_MASK         (0xFF00U)
#define FSP_COR_OFFSET_COR_Y_OFFSET_SHIFT        (8U)
#define FSP_COR_OFFSET_COR_Y_OFFSET(x)           (((uint32_t)(((uint32_t)(x)) << FSP_COR_OFFSET_COR_Y_OFFSET_SHIFT)) & FSP_COR_OFFSET_COR_Y_OFFSET_MASK)

/*! @name FIR_CFG_CH0 - FIR channel 0 configuration register */
#define FSP_FIR_CFG_CH0_FIR_CH0_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH0_FIR_CH0_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH0_FIR_CH0_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH0_FIR_CH0_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH0_FIR_CH0_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH0_FIR_CH0_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH0_FIR_CH0_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH0_FIR_CH0_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH0_FIR_CH0_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH0_FIR_CH0_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH0_FIR_BUF_CLR_ALL_MASK     (0x40000000U)
#define FSP_FIR_CFG_CH0_FIR_BUF_CLR_ALL_SHIFT    (30U)
#define FSP_FIR_CFG_CH0_FIR_BUF_CLR_ALL(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH0_FIR_BUF_CLR_ALL_SHIFT)) & FSP_FIR_CFG_CH0_FIR_BUF_CLR_ALL_MASK)
#define FSP_FIR_CFG_CH0_FIR_CH0_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH0_FIR_CH0_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH0_FIR_CH0_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH0_FIR_CH0_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH0_FIR_CH0_BUF_CLR_MASK)

/*! @name FIR_CFG_CH1 - FIR channel 1 configuration register */
#define FSP_FIR_CFG_CH1_FIR_CH1_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH1_FIR_CH1_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH1_FIR_CH1_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH1_FIR_CH1_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH1_FIR_CH1_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH1_FIR_CH1_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH1_FIR_CH1_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH1_FIR_CH1_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH1_FIR_CH1_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH1_FIR_CH1_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH1_FIR_CH1_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH1_FIR_CH1_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH1_FIR_CH1_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH1_FIR_CH1_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH1_FIR_CH1_BUF_CLR_MASK)

/*! @name FIR_CFG_CH2 - FIR channel 2 configuration register */
#define FSP_FIR_CFG_CH2_FIR_CH2_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH2_FIR_CH2_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH2_FIR_CH2_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH2_FIR_CH2_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH2_FIR_CH2_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH2_FIR_CH2_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH2_FIR_CH2_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH2_FIR_CH2_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH2_FIR_CH2_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH2_FIR_CH2_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH2_FIR_CH2_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH2_FIR_CH2_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH2_FIR_CH2_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH2_FIR_CH2_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH2_FIR_CH2_BUF_CLR_MASK)

/*! @name FIR_CFG_CH3 - FIR channel 3 configuration register */
#define FSP_FIR_CFG_CH3_FIR_CH3_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH3_FIR_CH3_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH3_FIR_CH3_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH3_FIR_CH3_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH3_FIR_CH3_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH3_FIR_CH3_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH3_FIR_CH3_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH3_FIR_CH3_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH3_FIR_CH3_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH3_FIR_CH3_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH3_FIR_CH3_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH3_FIR_CH3_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH3_FIR_CH3_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH3_FIR_CH3_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH3_FIR_CH3_BUF_CLR_MASK)

/*! @name FIR_CFG_CH4 - FIR channel 4 configuration register */
#define FSP_FIR_CFG_CH4_FIR_CH4_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH4_FIR_CH4_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH4_FIR_CH4_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH4_FIR_CH4_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH4_FIR_CH4_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH4_FIR_CH4_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH4_FIR_CH4_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH4_FIR_CH4_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH4_FIR_CH4_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH4_FIR_CH4_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH4_FIR_CH4_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH4_FIR_CH4_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH4_FIR_CH4_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH4_FIR_CH4_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH4_FIR_CH4_BUF_CLR_MASK)

/*! @name FIR_CFG_CH5 - FIR channel 5 configuration register */
#define FSP_FIR_CFG_CH5_FIR_CH5_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH5_FIR_CH5_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH5_FIR_CH5_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH5_FIR_CH5_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH5_FIR_CH5_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH5_FIR_CH5_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH5_FIR_CH5_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH5_FIR_CH5_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH5_FIR_CH5_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH5_FIR_CH5_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH5_FIR_CH5_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH5_FIR_CH5_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH5_FIR_CH5_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH5_FIR_CH5_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH5_FIR_CH5_BUF_CLR_MASK)

/*! @name FIR_CFG_CH6 - FIR channel 6 configuration register */
#define FSP_FIR_CFG_CH6_FIR_CH6_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH6_FIR_CH6_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH6_FIR_CH6_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH6_FIR_CH6_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH6_FIR_CH6_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH6_FIR_CH6_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH6_FIR_CH6_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH6_FIR_CH6_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH6_FIR_CH6_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH6_FIR_CH6_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH6_FIR_CH6_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH6_FIR_CH6_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH6_FIR_CH6_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH6_FIR_CH6_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH6_FIR_CH6_BUF_CLR_MASK)

/*! @name FIR_CFG_CH7 - FIR channel 7 configuration register */
#define FSP_FIR_CFG_CH7_FIR_CH7_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH7_FIR_CH7_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH7_FIR_CH7_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH7_FIR_CH7_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH7_FIR_CH7_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH7_FIR_CH7_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH7_FIR_CH7_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH7_FIR_CH7_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH7_FIR_CH7_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH7_FIR_CH7_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH7_FIR_CH7_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH7_FIR_CH7_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH7_FIR_CH7_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH7_FIR_CH7_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH7_FIR_CH7_BUF_CLR_MASK)

/*! @name FIR_CFG_CH8 - FIR channel 8 configuration register */
#define FSP_FIR_CFG_CH8_FIR_CH8_COEF_BASE_MASK   (0xFFFFU)
#define FSP_FIR_CFG_CH8_FIR_CH8_COEF_BASE_SHIFT  (0U)
#define FSP_FIR_CFG_CH8_FIR_CH8_COEF_BASE(x)     (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH8_FIR_CH8_COEF_BASE_SHIFT)) & FSP_FIR_CFG_CH8_FIR_CH8_COEF_BASE_MASK)
#define FSP_FIR_CFG_CH8_FIR_CH8_TAP_LEN_MASK     (0xF0000U)
#define FSP_FIR_CFG_CH8_FIR_CH8_TAP_LEN_SHIFT    (16U)
#define FSP_FIR_CFG_CH8_FIR_CH8_TAP_LEN(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH8_FIR_CH8_TAP_LEN_SHIFT)) & FSP_FIR_CFG_CH8_FIR_CH8_TAP_LEN_MASK)
#define FSP_FIR_CFG_CH8_FIR_CH8_BUF_CLR_MASK     (0x80000000U)
#define FSP_FIR_CFG_CH8_FIR_CH8_BUF_CLR_SHIFT    (31U)
#define FSP_FIR_CFG_CH8_FIR_CH8_BUF_CLR(x)       (((uint32_t)(((uint32_t)(x)) << FSP_FIR_CFG_CH8_FIR_CH8_BUF_CLR_SHIFT)) & FSP_FIR_CFG_CH8_FIR_CH8_BUF_CLR_MASK)

/*! @name FIR_DAT0_FX - FIR channel 0 fix point data input &amp; output register */
#define FSP_FIR_DAT0_FX_FIR_DAT0_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT0_FX_FIR_DAT0_FX_SHIFT        (0U)
#define FSP_FIR_DAT0_FX_FIR_DAT0_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT0_FX_FIR_DAT0_FX_SHIFT)) & FSP_FIR_DAT0_FX_FIR_DAT0_FX_MASK)

/*! @name FIR_DAT1_FX - FIR channel 1 fix point data input &amp; output register */
#define FSP_FIR_DAT1_FX_FIR_DAT1_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT1_FX_FIR_DAT1_FX_SHIFT        (0U)
#define FSP_FIR_DAT1_FX_FIR_DAT1_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT1_FX_FIR_DAT1_FX_SHIFT)) & FSP_FIR_DAT1_FX_FIR_DAT1_FX_MASK)

/*! @name FIR_DAT2_FX - FIR channel 2 fix point data input &amp; output register */
#define FSP_FIR_DAT2_FX_FIR_DAT2_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT2_FX_FIR_DAT2_FX_SHIFT        (0U)
#define FSP_FIR_DAT2_FX_FIR_DAT2_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT2_FX_FIR_DAT2_FX_SHIFT)) & FSP_FIR_DAT2_FX_FIR_DAT2_FX_MASK)

/*! @name FIR_DAT3_FX - FIR channel 3 fix point data input &amp; output register */
#define FSP_FIR_DAT3_FX_FIR_DAT3_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT3_FX_FIR_DAT3_FX_SHIFT        (0U)
#define FSP_FIR_DAT3_FX_FIR_DAT3_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT3_FX_FIR_DAT3_FX_SHIFT)) & FSP_FIR_DAT3_FX_FIR_DAT3_FX_MASK)

/*! @name FIR_DAT4_FX - FIR channel 4 fix point data input &amp; output register */
#define FSP_FIR_DAT4_FX_FIR_DAT4_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT4_FX_FIR_DAT4_FX_SHIFT        (0U)
#define FSP_FIR_DAT4_FX_FIR_DAT4_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT4_FX_FIR_DAT4_FX_SHIFT)) & FSP_FIR_DAT4_FX_FIR_DAT4_FX_MASK)

/*! @name FIR_DAT5_FX - FIR channel 5 fix point data input &amp; output register */
#define FSP_FIR_DAT5_FX_FIR_DAT5_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT5_FX_FIR_DAT5_FX_SHIFT        (0U)
#define FSP_FIR_DAT5_FX_FIR_DAT5_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT5_FX_FIR_DAT5_FX_SHIFT)) & FSP_FIR_DAT5_FX_FIR_DAT5_FX_MASK)

/*! @name FIR_DAT6_FX - FIR channel 6 fix point data input &amp; output register */
#define FSP_FIR_DAT6_FX_FIR_DAT6_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT6_FX_FIR_DAT6_FX_SHIFT        (0U)
#define FSP_FIR_DAT6_FX_FIR_DAT6_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT6_FX_FIR_DAT6_FX_SHIFT)) & FSP_FIR_DAT6_FX_FIR_DAT6_FX_MASK)

/*! @name FIR_DAT7_FX - FIR channel 7 fix point data input &amp; output register */
#define FSP_FIR_DAT7_FX_FIR_DAT7_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT7_FX_FIR_DAT7_FX_SHIFT        (0U)
#define FSP_FIR_DAT7_FX_FIR_DAT7_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT7_FX_FIR_DAT7_FX_SHIFT)) & FSP_FIR_DAT7_FX_FIR_DAT7_FX_MASK)

/*! @name FIR_DAT8_FX - FIR channel 8 fix point data input &amp; output register */
#define FSP_FIR_DAT8_FX_FIR_DAT8_FX_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT8_FX_FIR_DAT8_FX_SHIFT        (0U)
#define FSP_FIR_DAT8_FX_FIR_DAT8_FX(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT8_FX_FIR_DAT8_FX_SHIFT)) & FSP_FIR_DAT8_FX_FIR_DAT8_FX_MASK)

/*! @name FIR_DAT0_FL - FIR channel 0 float point data input &amp; output register */
#define FSP_FIR_DAT0_FL_FIR_DAT0_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT0_FL_FIR_DAT0_FL_SHIFT        (0U)
#define FSP_FIR_DAT0_FL_FIR_DAT0_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT0_FL_FIR_DAT0_FL_SHIFT)) & FSP_FIR_DAT0_FL_FIR_DAT0_FL_MASK)

/*! @name FIR_DAT1_FL - FIR channel 1 float point data input &amp; output register */
#define FSP_FIR_DAT1_FL_FIR_DAT1_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT1_FL_FIR_DAT1_FL_SHIFT        (0U)
#define FSP_FIR_DAT1_FL_FIR_DAT1_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT1_FL_FIR_DAT1_FL_SHIFT)) & FSP_FIR_DAT1_FL_FIR_DAT1_FL_MASK)

/*! @name FIR_DAT2_FL - FIR channel 2 float point data input &amp; output register */
#define FSP_FIR_DAT2_FL_FIR_DAT2_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT2_FL_FIR_DAT2_FL_SHIFT        (0U)
#define FSP_FIR_DAT2_FL_FIR_DAT2_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT2_FL_FIR_DAT2_FL_SHIFT)) & FSP_FIR_DAT2_FL_FIR_DAT2_FL_MASK)

/*! @name FIR_DAT3_FL - FIR channel 3 float point data input &amp; output register */
#define FSP_FIR_DAT3_FL_FIR_DAT3_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT3_FL_FIR_DAT3_FL_SHIFT        (0U)
#define FSP_FIR_DAT3_FL_FIR_DAT3_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT3_FL_FIR_DAT3_FL_SHIFT)) & FSP_FIR_DAT3_FL_FIR_DAT3_FL_MASK)

/*! @name FIR_DAT4_FL - FIR channel 4 float point data input &amp; output register */
#define FSP_FIR_DAT4_FL_FIR_DAT4_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT4_FL_FIR_DAT4_FL_SHIFT        (0U)
#define FSP_FIR_DAT4_FL_FIR_DAT4_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT4_FL_FIR_DAT4_FL_SHIFT)) & FSP_FIR_DAT4_FL_FIR_DAT4_FL_MASK)

/*! @name FIR_DAT5_FL - FIR channel 5 float point data input &amp; output register */
#define FSP_FIR_DAT5_FL_FIR_DAT5_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT5_FL_FIR_DAT5_FL_SHIFT        (0U)
#define FSP_FIR_DAT5_FL_FIR_DAT5_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT5_FL_FIR_DAT5_FL_SHIFT)) & FSP_FIR_DAT5_FL_FIR_DAT5_FL_MASK)

/*! @name FIR_DAT6_FL - FIR channel 6 float point data input &amp; output register */
#define FSP_FIR_DAT6_FL_FIR_DAT6_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT6_FL_FIR_DAT6_FL_SHIFT        (0U)
#define FSP_FIR_DAT6_FL_FIR_DAT6_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT6_FL_FIR_DAT6_FL_SHIFT)) & FSP_FIR_DAT6_FL_FIR_DAT6_FL_MASK)

/*! @name FIR_DAT7_FL - FIR channel 7 float point data input &amp; output register */
#define FSP_FIR_DAT7_FL_FIR_DAT7_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT7_FL_FIR_DAT7_FL_SHIFT        (0U)
#define FSP_FIR_DAT7_FL_FIR_DAT7_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT7_FL_FIR_DAT7_FL_SHIFT)) & FSP_FIR_DAT7_FL_FIR_DAT7_FL_MASK)

/*! @name FIR_DAT8_FL - FIR channel 8 float point data input &amp; output register */
#define FSP_FIR_DAT8_FL_FIR_DAT8_FL_MASK         (0xFFFFFFFFU)
#define FSP_FIR_DAT8_FL_FIR_DAT8_FL_SHIFT        (0U)
#define FSP_FIR_DAT8_FL_FIR_DAT8_FL(x)           (((uint32_t)(((uint32_t)(x)) << FSP_FIR_DAT8_FL_FIR_DAT8_FL_SHIFT)) & FSP_FIR_DAT8_FL_FIR_DAT8_FL_MASK)

/*! @name SIN_COS_IXOX - sin &amp; cos input fix output fix mode data address register */
#define FSP_SIN_COS_IXOX_SIN_COS_IXOX_SRC_MASK   (0xFFFFU)
#define FSP_SIN_COS_IXOX_SIN_COS_IXOX_SRC_SHIFT  (0U)
#define FSP_SIN_COS_IXOX_SIN_COS_IXOX_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_IXOX_SIN_COS_IXOX_SRC_SHIFT)) & FSP_SIN_COS_IXOX_SIN_COS_IXOX_SRC_MASK)
#define FSP_SIN_COS_IXOX_SIN_COS_IXOX_DST_MASK   (0xFFFF0000U)
#define FSP_SIN_COS_IXOX_SIN_COS_IXOX_DST_SHIFT  (16U)
#define FSP_SIN_COS_IXOX_SIN_COS_IXOX_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_IXOX_SIN_COS_IXOX_DST_SHIFT)) & FSP_SIN_COS_IXOX_SIN_COS_IXOX_DST_MASK)

/*! @name SIN_COS_IXOL - sin &amp; cos input fix output float mode data address register */
#define FSP_SIN_COS_IXOL_SIN_COS_IXOL_SRC_MASK   (0xFFFFU)
#define FSP_SIN_COS_IXOL_SIN_COS_IXOL_SRC_SHIFT  (0U)
#define FSP_SIN_COS_IXOL_SIN_COS_IXOL_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_IXOL_SIN_COS_IXOL_SRC_SHIFT)) & FSP_SIN_COS_IXOL_SIN_COS_IXOL_SRC_MASK)
#define FSP_SIN_COS_IXOL_SIN_COS_IXOL_DST_MASK   (0xFFFF0000U)
#define FSP_SIN_COS_IXOL_SIN_COS_IXOL_DST_SHIFT  (16U)
#define FSP_SIN_COS_IXOL_SIN_COS_IXOL_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_IXOL_SIN_COS_IXOL_DST_SHIFT)) & FSP_SIN_COS_IXOL_SIN_COS_IXOL_DST_MASK)

/*! @name SIN_COS_ILOX - sin &amp; cos input float output fix mode data address register */
#define FSP_SIN_COS_ILOX_SIN_COS_ILOX_SRC_MASK   (0xFFFFU)
#define FSP_SIN_COS_ILOX_SIN_COS_ILOX_SRC_SHIFT  (0U)
#define FSP_SIN_COS_ILOX_SIN_COS_ILOX_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_ILOX_SIN_COS_ILOX_SRC_SHIFT)) & FSP_SIN_COS_ILOX_SIN_COS_ILOX_SRC_MASK)
#define FSP_SIN_COS_ILOX_SIN_COS_ILOX_DST_MASK   (0xFFFF0000U)
#define FSP_SIN_COS_ILOX_SIN_COS_ILOX_DST_SHIFT  (16U)
#define FSP_SIN_COS_ILOX_SIN_COS_ILOX_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_ILOX_SIN_COS_ILOX_DST_SHIFT)) & FSP_SIN_COS_ILOX_SIN_COS_ILOX_DST_MASK)

/*! @name SIN_COS_ILOL - sin &amp; cos input float output float mode data address register */
#define FSP_SIN_COS_ILOL_SIN_COS_ILOL_SRC_MASK   (0xFFFFU)
#define FSP_SIN_COS_ILOL_SIN_COS_ILOL_SRC_SHIFT  (0U)
#define FSP_SIN_COS_ILOL_SIN_COS_ILOL_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_ILOL_SIN_COS_ILOL_SRC_SHIFT)) & FSP_SIN_COS_ILOL_SIN_COS_ILOL_SRC_MASK)
#define FSP_SIN_COS_ILOL_SIN_COS_ILOL_DST_MASK   (0xFFFF0000U)
#define FSP_SIN_COS_ILOL_SIN_COS_ILOL_DST_SHIFT  (16U)
#define FSP_SIN_COS_ILOL_SIN_COS_ILOL_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_SIN_COS_ILOL_SIN_COS_ILOL_DST_SHIFT)) & FSP_SIN_COS_ILOL_SIN_COS_ILOL_DST_MASK)

/*! @name LN_SQRT_IXOX - LN &amp; sqrt input fix output fix mode data address register */
#define FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_SRC_MASK   (0xFFFFU)
#define FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_SRC_SHIFT  (0U)
#define FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_SRC_SHIFT)) & FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_SRC_MASK)
#define FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_DST_MASK   (0xFFFF0000U)
#define FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_DST_SHIFT  (16U)
#define FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_DST_SHIFT)) & FSP_LN_SQRT_IXOX_LN_SQRT_IXOX_DST_MASK)

/*! @name LN_SQRT_IXOL - LN &amp; sqrt input fix output float mode data address register */
#define FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_SRC_MASK   (0xFFFFU)
#define FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_SRC_SHIFT  (0U)
#define FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_SRC_SHIFT)) & FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_SRC_MASK)
#define FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_DST_MASK   (0xFFFF0000U)
#define FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_DST_SHIFT  (16U)
#define FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_DST_SHIFT)) & FSP_LN_SQRT_IXOL_LN_SQRT_IXOL_DST_MASK)

/*! @name LN_SQRT_ILOX - LN &amp; sqrt input float output fix mode data address register */
#define FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_SRC_MASK   (0xFFFFU)
#define FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_SRC_SHIFT  (0U)
#define FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_SRC_SHIFT)) & FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_SRC_MASK)
#define FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_DST_MASK   (0xFFFF0000U)
#define FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_DST_SHIFT  (16U)
#define FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_DST_SHIFT)) & FSP_LN_SQRT_ILOX_LN_SQRT_ILOX_DST_MASK)

/*! @name LN_SQRT_ILOL - LN &amp; sqrt input float output float mode data address register */
#define FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_SRC_MASK   (0xFFFFU)
#define FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_SRC_SHIFT  (0U)
#define FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_SRC(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_SRC_SHIFT)) & FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_SRC_MASK)
#define FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_DST_MASK   (0xFFFF0000U)
#define FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_DST_SHIFT  (16U)
#define FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_DST(x)     (((uint32_t)(((uint32_t)(x)) << FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_DST_SHIFT)) & FSP_LN_SQRT_ILOL_LN_SQRT_ILOL_DST_MASK)

/*! @name CORDIC_T0UP_IXOX - native cordic input fix output fix, t=0, u=1 mode data address register */
#define FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_SRC_SHIFT)) & FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_SRC_MASK)
#define FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_DST_SHIFT (16U)
#define FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_DST_SHIFT)) & FSP_CORDIC_T0UP_IXOX_CORDIC_T0UP_IXOX_DST_MASK)

/*! @name CORDIC_T0UP_IXOL - native cordic input fix output float, t=0, u=1 mode data address register */
#define FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_SRC_SHIFT)) & FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_SRC_MASK)
#define FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_DST_SHIFT (16U)
#define FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_DST_SHIFT)) & FSP_CORDIC_T0UP_IXOL_CORDIC_T0UP_IXOL_DST_MASK)

/*! @name CORDIC_T0UP_ILOX - native cordic input float output fix, t=0, u=1 mode data address register */
#define FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_SRC_SHIFT)) & FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_SRC_MASK)
#define FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_DST_SHIFT (16U)
#define FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_DST_SHIFT)) & FSP_CORDIC_T0UP_ILOX_CORDIC_T0UP_ILOX_DST_MASK)

/*! @name CORDIC_T0UP_ILOL - native cordic input float output float, t=0, u=1 mode data address register */
#define FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_SRC_SHIFT)) & FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_SRC_MASK)
#define FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_DST_SHIFT (16U)
#define FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_DST_SHIFT)) & FSP_CORDIC_T0UP_ILOL_CORDIC_T0UP_ILOL_DST_MASK)

/*! @name CORDIC_T0UN_IXOX - native cordic input fix output fix, t=0, u=-1 mode data address register */
#define FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_SRC_SHIFT)) & FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_SRC_MASK)
#define FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_DST_SHIFT (16U)
#define FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_DST_SHIFT)) & FSP_CORDIC_T0UN_IXOX_CORDIC_T0UN_IXOX_DST_MASK)

/*! @name CORDIC_T0UN_IXOL - native cordic input fix output float, t=0, u=-1 mode data address register */
#define FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_SRC_SHIFT)) & FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_SRC_MASK)
#define FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_DST_SHIFT (16U)
#define FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_DST_SHIFT)) & FSP_CORDIC_T0UN_IXOL_CORDIC_T0UN_IXOL_DST_MASK)

/*! @name CORDIC_T0UN_ILOX - native cordic input float output fix, t=0, u=-1 mode data address register */
#define FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_SRC_SHIFT)) & FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_SRC_MASK)
#define FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_DST_SHIFT (16U)
#define FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_DST_SHIFT)) & FSP_CORDIC_T0UN_ILOX_CORDIC_T0UN_ILOX_DST_MASK)

/*! @name CORDIC_T0UN_ILOL - native cordic input float output float, t=0, u=-1 mode data address register */
#define FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_SRC_SHIFT)) & FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_SRC_MASK)
#define FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_DST_SHIFT (16U)
#define FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_DST_SHIFT)) & FSP_CORDIC_T0UN_ILOL_CORDIC_T0UN_ILOL_DST_MASK)

/*! @name CORDIC_T1UP_IXOX - native cordic input fix output fix, t=1, u=1 mode data address register */
#define FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_SRC_SHIFT)) & FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_SRC_MASK)
#define FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_DST_SHIFT (16U)
#define FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_DST_SHIFT)) & FSP_CORDIC_T1UP_IXOX_CORDIC_T1UP_IXOX_DST_MASK)

/*! @name CORDIC_T1UP_IXOL - native cordic input fix output float, t=1, u=1 mode data address register */
#define FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_SRC_SHIFT)) & FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_SRC_MASK)
#define FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_DST_SHIFT (16U)
#define FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_DST_SHIFT)) & FSP_CORDIC_T1UP_IXOL_CORDIC_T1UP_IXOL_DST_MASK)

/*! @name CORDIC_T1UP_ILOX - native cordic input float output fix, t=1, u=1 mode data address register */
#define FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_SRC_SHIFT)) & FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_SRC_MASK)
#define FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_DST_SHIFT (16U)
#define FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_DST_SHIFT)) & FSP_CORDIC_T1UP_ILOX_CORDIC_T1UP_ILOX_DST_MASK)

/*! @name CORDIC_T1UP_ILOL - native cordic input float output float, t=1, u=1 mode data address register */
#define FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_SRC_SHIFT)) & FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_SRC_MASK)
#define FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_DST_SHIFT (16U)
#define FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_DST_SHIFT)) & FSP_CORDIC_T1UP_ILOL_CORDIC_T1UP_ILOL_DST_MASK)

/*! @name CORDIC_T1UN_IXOX - native cordic input fix output fix, t=1, u=-1 mode data address register */
#define FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_SRC_SHIFT)) & FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_SRC_MASK)
#define FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_DST_SHIFT (16U)
#define FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_DST_SHIFT)) & FSP_CORDIC_T1UN_IXOX_CORDIC_T1UN_IXOX_DST_MASK)

/*! @name CORDIC_T1UN_IXOL - native cordic input fix output float, t=1, u=-1 mode data address register */
#define FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_SRC_SHIFT)) & FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_SRC_MASK)
#define FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_DST_SHIFT (16U)
#define FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_DST_SHIFT)) & FSP_CORDIC_T1UN_IXOL_CORDIC_T1UN_IXOL_DST_MASK)

/*! @name CORDIC_T1UN_ILOX - native cordic input float output fix, t=1, u=-1 mode data address register */
#define FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_SRC_SHIFT)) & FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_SRC_MASK)
#define FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_DST_SHIFT (16U)
#define FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_DST_SHIFT)) & FSP_CORDIC_T1UN_ILOX_CORDIC_T1UN_ILOX_DST_MASK)

/*! @name CORDIC_T1UN_ILOL - native cordic input float output float, t=1, u=-1 mode data address register */
#define FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_SRC_MASK (0xFFFFU)
#define FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_SRC_SHIFT (0U)
#define FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_SRC(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_SRC_SHIFT)) & FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_SRC_MASK)
#define FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_DST_MASK (0xFFFF0000U)
#define FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_DST_SHIFT (16U)
#define FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_DST(x) (((uint32_t)(((uint32_t)(x)) << FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_DST_SHIFT)) & FSP_CORDIC_T1UN_ILOL_CORDIC_T1UN_ILOL_DST_MASK)


/*!
 * @}
 */ /* end of group FSP_Register_Masks */


/* FSP - Peripheral instance base addresses */
/** Peripheral FSP base address */
#define FSP_BASE                                 (0x40088000u)
/** Peripheral FSP base pointer */
#define FSP                                      ((FSP_Type *)FSP_BASE)
/** Array initializer of FSP peripheral base addresses */
#define FSP_BASE_ADDRS                           { FSP_BASE }
/** Array initializer of FSP peripheral base pointers */
#define FSP_BASE_PTRS                            { FSP }
/** Interrupt vectors for the FSP peripheral type */
#define FSP_IRQS                                 { FSP_IRQn }

/*!
 * @}
 */ /* end of group FSP_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Peripheral_Access_Layer GPIO Peripheral Access Layer
 * @{
 */

/** GPIO - Register Layout Typedef */
typedef struct {
  __I  uint32_t DATA;                              /**< GPIO value register, offset: 0x0 */
  __IO uint32_t DATAOUT;                           /**< GPIO output status register, offset: 0x4 */
       uint8_t RESERVED_0[8];
  __IO uint32_t OUTENSET;                          /**< GPIO output enable set register, offset: 0x10 */
  __IO uint32_t OUTENCLR;                          /**< GPIO output clear register, offset: 0x14 */
       uint8_t RESERVED_1[8];
  __O  uint32_t INTENSET;                          /**< GPIO interrupt enable set register, offset: 0x20 */
  __IO uint32_t INTENCLR;                          /**< GPIO interrupt enable clear register, offset: 0x24 */
  __O  uint32_t INTTYPESET;                        /**< GPIO interrupt type set register, offset: 0x28 */
  __IO uint32_t INTTYPECLR;                        /**< GPIO interrupt type set register, offset: 0x2C */
  __O  uint32_t INTPOLSET;                         /**< GPIO interrupt polarity set register, offset: 0x30 */
  __IO uint32_t INTPOLCLR;                         /**< GPIO interrupt polarity clear register, offset: 0x34 */
  __IO uint32_t INTSTATUS;                         /**< GPIO interrupt status register, offset: 0x38 */
} GPIO_Type;

/* ----------------------------------------------------------------------------
   -- GPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Masks GPIO Register Masks
 * @{
 */

/*! @name DATA - GPIO value register */
#define GPIO_DATA_DATA_MASK                      (0xFFFFFFFFU)
#define GPIO_DATA_DATA_SHIFT                     (0U)
#define GPIO_DATA_DATA(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_DATA_DATA_SHIFT)) & GPIO_DATA_DATA_MASK)

/*! @name DATAOUT - GPIO output status register */
#define GPIO_DATAOUT_DATAOUT_MASK                (0xFFFFFFFFU)
#define GPIO_DATAOUT_DATAOUT_SHIFT               (0U)
#define GPIO_DATAOUT_DATAOUT(x)                  (((uint32_t)(((uint32_t)(x)) << GPIO_DATAOUT_DATAOUT_SHIFT)) & GPIO_DATAOUT_DATAOUT_MASK)

/*! @name OUTENSET - GPIO output enable set register */
#define GPIO_OUTENSET_OUTENSET_MASK              (0xFFFFFFFFU)
#define GPIO_OUTENSET_OUTENSET_SHIFT             (0U)
#define GPIO_OUTENSET_OUTENSET(x)                (((uint32_t)(((uint32_t)(x)) << GPIO_OUTENSET_OUTENSET_SHIFT)) & GPIO_OUTENSET_OUTENSET_MASK)

/*! @name OUTENCLR - GPIO output clear register */
#define GPIO_OUTENCLR_OUTENCLR_MASK              (0xFFFFFFFFU)
#define GPIO_OUTENCLR_OUTENCLR_SHIFT             (0U)
#define GPIO_OUTENCLR_OUTENCLR(x)                (((uint32_t)(((uint32_t)(x)) << GPIO_OUTENCLR_OUTENCLR_SHIFT)) & GPIO_OUTENCLR_OUTENCLR_MASK)

/*! @name INTENSET - GPIO interrupt enable set register */
#define GPIO_INTENSET_INTENSET_MASK              (0xFFFFFFFFU)
#define GPIO_INTENSET_INTENSET_SHIFT             (0U)
#define GPIO_INTENSET_INTENSET(x)                (((uint32_t)(((uint32_t)(x)) << GPIO_INTENSET_INTENSET_SHIFT)) & GPIO_INTENSET_INTENSET_MASK)

/*! @name INTENCLR - GPIO interrupt enable clear register */
#define GPIO_INTENCLR_INTENCLR_MASK              (0xFFFFFFFFU)
#define GPIO_INTENCLR_INTENCLR_SHIFT             (0U)
#define GPIO_INTENCLR_INTENCLR(x)                (((uint32_t)(((uint32_t)(x)) << GPIO_INTENCLR_INTENCLR_SHIFT)) & GPIO_INTENCLR_INTENCLR_MASK)

/*! @name INTTYPESET - GPIO interrupt type set register */
#define GPIO_INTTYPESET_INTTYPESET_MASK          (0xFFFFFFFFU)
#define GPIO_INTTYPESET_INTTYPESET_SHIFT         (0U)
#define GPIO_INTTYPESET_INTTYPESET(x)            (((uint32_t)(((uint32_t)(x)) << GPIO_INTTYPESET_INTTYPESET_SHIFT)) & GPIO_INTTYPESET_INTTYPESET_MASK)

/*! @name INTTYPECLR - GPIO interrupt type set register */
#define GPIO_INTTYPECLR_INTTYPECLR_MASK          (0xFFFFFFFFU)
#define GPIO_INTTYPECLR_INTTYPECLR_SHIFT         (0U)
#define GPIO_INTTYPECLR_INTTYPECLR(x)            (((uint32_t)(((uint32_t)(x)) << GPIO_INTTYPECLR_INTTYPECLR_SHIFT)) & GPIO_INTTYPECLR_INTTYPECLR_MASK)

/*! @name INTPOLSET - GPIO interrupt polarity set register */
#define GPIO_INTPOLSET_INTPOLSET_MASK            (0xFFFFFFFFU)
#define GPIO_INTPOLSET_INTPOLSET_SHIFT           (0U)
#define GPIO_INTPOLSET_INTPOLSET(x)              (((uint32_t)(((uint32_t)(x)) << GPIO_INTPOLSET_INTPOLSET_SHIFT)) & GPIO_INTPOLSET_INTPOLSET_MASK)

/*! @name INTPOLCLR - GPIO interrupt polarity clear register */
#define GPIO_INTPOLCLR_INTPOLCLR_MASK            (0xFFFFFFFFU)
#define GPIO_INTPOLCLR_INTPOLCLR_SHIFT           (0U)
#define GPIO_INTPOLCLR_INTPOLCLR(x)              (((uint32_t)(((uint32_t)(x)) << GPIO_INTPOLCLR_INTPOLCLR_SHIFT)) & GPIO_INTPOLCLR_INTPOLCLR_MASK)

/*! @name INTSTATUS - GPIO interrupt status register */
#define GPIO_INTSTATUS_INTSTATUS_MASK            (0xFFFFFFFFU)
#define GPIO_INTSTATUS_INTSTATUS_SHIFT           (0U)
#define GPIO_INTSTATUS_INTSTATUS(x)              (((uint32_t)(((uint32_t)(x)) << GPIO_INTSTATUS_INTSTATUS_SHIFT)) & GPIO_INTSTATUS_INTSTATUS_MASK)


/*!
 * @}
 */ /* end of group GPIO_Register_Masks */


/* GPIO - Peripheral instance base addresses */
/** Peripheral GPIOA base address */
#define GPIOA_BASE                               (0x4008C000u)
/** Peripheral GPIOA base pointer */
#define GPIOA                                    ((GPIO_Type *)GPIOA_BASE)
/** Peripheral GPIOB base address */
#define GPIOB_BASE                               (0x4008D000u)
/** Peripheral GPIOB base pointer */
#define GPIOB                                    ((GPIO_Type *)GPIOB_BASE)
/** Array initializer of GPIO peripheral base addresses */
#define GPIO_BASE_ADDRS                          { GPIOA_BASE, GPIOB_BASE }
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASE_PTRS                           { GPIOA, GPIOB }
/** Interrupt vectors for the GPIO peripheral type */
#define GPIO_IRQS                                { GPIOA_IRQn, GPIOB_IRQn }

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
       uint8_t RESERVED_0[2048];
  __IO uint32_t CFG;                               /**< Configuration for shared functions., offset: 0x800 */
  __IO uint32_t STAT;                              /**< Status register for Master, Slave, and Monitor functions., offset: 0x804 */
  __IO uint32_t INTENSET;                          /**< Interrupt Enable Set and read register., offset: 0x808 */
  __O  uint32_t INTENCLR;                          /**< Interrupt Enable Clear register., offset: 0x80C */
  __IO uint32_t TIMEOUT;                           /**< Time-out value register., offset: 0x810 */
  __IO uint32_t CLKDIV;                            /**< Clock pre-divider for the entire I2C interface. This determines what time increments are used for the MSTTIME register, and controls some timing of the Slave function., offset: 0x814 */
  __I  uint32_t INTSTAT;                           /**< Interrupt Status register for Master, Slave, and Monitor functions., offset: 0x818 */
       uint8_t RESERVED_1[4];
  __IO uint32_t MSTCTL;                            /**< Master control register., offset: 0x820 */
  __IO uint32_t MSTTIME;                           /**< Master timing configuration., offset: 0x824 */
  __IO uint32_t MSTDAT;                            /**< Combined Master receiver and transmitter data register., offset: 0x828 */
       uint8_t RESERVED_2[20];
  __IO uint32_t SLVCTL;                            /**< Slave control register., offset: 0x840 */
  __IO uint32_t SLVDAT;                            /**< Combined Slave receiver and transmitter data register., offset: 0x844 */
  __IO uint32_t SLVADR[4];                         /**< Slave address register., array offset: 0x848, array step: 0x4 */
  __IO uint32_t SLVQUAL0;                          /**< Slave Qualification for address 0., offset: 0x858 */
       uint8_t RESERVED_3[36];
  __I  uint32_t MONRXDAT;                          /**< Monitor receiver data register., offset: 0x880 */
       uint8_t RESERVED_4[1912];
  __I  uint32_t ID;                                /**< I2C module Identification. This value appears in the shared Flexcomm peripheral ID register when I2C is selected., offset: 0xFFC */
} I2C_Type;

/* ----------------------------------------------------------------------------
   -- I2C Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Register_Masks I2C Register Masks
 * @{
 */

/*! @name CFG - Configuration for shared functions. */
#define I2C_CFG_MSTEN_MASK                       (0x1U)
#define I2C_CFG_MSTEN_SHIFT                      (0U)
#define I2C_CFG_MSTEN(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_CFG_MSTEN_SHIFT)) & I2C_CFG_MSTEN_MASK)
#define I2C_CFG_SLVEN_MASK                       (0x2U)
#define I2C_CFG_SLVEN_SHIFT                      (1U)
#define I2C_CFG_SLVEN(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_CFG_SLVEN_SHIFT)) & I2C_CFG_SLVEN_MASK)
#define I2C_CFG_MONEN_MASK                       (0x4U)
#define I2C_CFG_MONEN_SHIFT                      (2U)
#define I2C_CFG_MONEN(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_CFG_MONEN_SHIFT)) & I2C_CFG_MONEN_MASK)
#define I2C_CFG_TIMEOUTEN_MASK                   (0x8U)
#define I2C_CFG_TIMEOUTEN_SHIFT                  (3U)
#define I2C_CFG_TIMEOUTEN(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_CFG_TIMEOUTEN_SHIFT)) & I2C_CFG_TIMEOUTEN_MASK)
#define I2C_CFG_MONCLKSTR_MASK                   (0x10U)
#define I2C_CFG_MONCLKSTR_SHIFT                  (4U)
#define I2C_CFG_MONCLKSTR(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_CFG_MONCLKSTR_SHIFT)) & I2C_CFG_MONCLKSTR_MASK)

/*! @name STAT - Status register for Master, Slave, and Monitor functions. */
#define I2C_STAT_MSTPENDING_MASK                 (0x1U)
#define I2C_STAT_MSTPENDING_SHIFT                (0U)
#define I2C_STAT_MSTPENDING(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MSTPENDING_SHIFT)) & I2C_STAT_MSTPENDING_MASK)
#define I2C_STAT_MSTSTATE_MASK                   (0xEU)
#define I2C_STAT_MSTSTATE_SHIFT                  (1U)
#define I2C_STAT_MSTSTATE(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MSTSTATE_SHIFT)) & I2C_STAT_MSTSTATE_MASK)
#define I2C_STAT_MSTARBLOSS_MASK                 (0x10U)
#define I2C_STAT_MSTARBLOSS_SHIFT                (4U)
#define I2C_STAT_MSTARBLOSS(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MSTARBLOSS_SHIFT)) & I2C_STAT_MSTARBLOSS_MASK)
#define I2C_STAT_MSTSTSTPERR_MASK                (0x40U)
#define I2C_STAT_MSTSTSTPERR_SHIFT               (6U)
#define I2C_STAT_MSTSTSTPERR(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MSTSTSTPERR_SHIFT)) & I2C_STAT_MSTSTSTPERR_MASK)
#define I2C_STAT_SLVPENDING_MASK                 (0x100U)
#define I2C_STAT_SLVPENDING_SHIFT                (8U)
#define I2C_STAT_SLVPENDING(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SLVPENDING_SHIFT)) & I2C_STAT_SLVPENDING_MASK)
#define I2C_STAT_SLVSTATE_MASK                   (0x600U)
#define I2C_STAT_SLVSTATE_SHIFT                  (9U)
#define I2C_STAT_SLVSTATE(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SLVSTATE_SHIFT)) & I2C_STAT_SLVSTATE_MASK)
#define I2C_STAT_SLVNOTSTR_MASK                  (0x800U)
#define I2C_STAT_SLVNOTSTR_SHIFT                 (11U)
#define I2C_STAT_SLVNOTSTR(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SLVNOTSTR_SHIFT)) & I2C_STAT_SLVNOTSTR_MASK)
#define I2C_STAT_SLVIDX_MASK                     (0x3000U)
#define I2C_STAT_SLVIDX_SHIFT                    (12U)
#define I2C_STAT_SLVIDX(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SLVIDX_SHIFT)) & I2C_STAT_SLVIDX_MASK)
#define I2C_STAT_SLVSEL_MASK                     (0x4000U)
#define I2C_STAT_SLVSEL_SHIFT                    (14U)
#define I2C_STAT_SLVSEL(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SLVSEL_SHIFT)) & I2C_STAT_SLVSEL_MASK)
#define I2C_STAT_SLVDESEL_MASK                   (0x8000U)
#define I2C_STAT_SLVDESEL_SHIFT                  (15U)
#define I2C_STAT_SLVDESEL(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SLVDESEL_SHIFT)) & I2C_STAT_SLVDESEL_MASK)
#define I2C_STAT_MONRDY_MASK                     (0x10000U)
#define I2C_STAT_MONRDY_SHIFT                    (16U)
#define I2C_STAT_MONRDY(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MONRDY_SHIFT)) & I2C_STAT_MONRDY_MASK)
#define I2C_STAT_MONOV_MASK                      (0x20000U)
#define I2C_STAT_MONOV_SHIFT                     (17U)
#define I2C_STAT_MONOV(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MONOV_SHIFT)) & I2C_STAT_MONOV_MASK)
#define I2C_STAT_MONACTIVE_MASK                  (0x40000U)
#define I2C_STAT_MONACTIVE_SHIFT                 (18U)
#define I2C_STAT_MONACTIVE(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MONACTIVE_SHIFT)) & I2C_STAT_MONACTIVE_MASK)
#define I2C_STAT_MONIDLE_MASK                    (0x80000U)
#define I2C_STAT_MONIDLE_SHIFT                   (19U)
#define I2C_STAT_MONIDLE(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_STAT_MONIDLE_SHIFT)) & I2C_STAT_MONIDLE_MASK)
#define I2C_STAT_EVENTTIMEOUT_MASK               (0x1000000U)
#define I2C_STAT_EVENTTIMEOUT_SHIFT              (24U)
#define I2C_STAT_EVENTTIMEOUT(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_STAT_EVENTTIMEOUT_SHIFT)) & I2C_STAT_EVENTTIMEOUT_MASK)
#define I2C_STAT_SCLTIMEOUT_MASK                 (0x2000000U)
#define I2C_STAT_SCLTIMEOUT_SHIFT                (25U)
#define I2C_STAT_SCLTIMEOUT(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_STAT_SCLTIMEOUT_SHIFT)) & I2C_STAT_SCLTIMEOUT_MASK)

/*! @name INTENSET - Interrupt Enable Set and read register. */
#define I2C_INTENSET_MSTPENDINGEN_MASK           (0x1U)
#define I2C_INTENSET_MSTPENDINGEN_SHIFT          (0U)
#define I2C_INTENSET_MSTPENDINGEN(x)             (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_MSTPENDINGEN_SHIFT)) & I2C_INTENSET_MSTPENDINGEN_MASK)
#define I2C_INTENSET_MSTARBLOSSEN_MASK           (0x10U)
#define I2C_INTENSET_MSTARBLOSSEN_SHIFT          (4U)
#define I2C_INTENSET_MSTARBLOSSEN(x)             (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_MSTARBLOSSEN_SHIFT)) & I2C_INTENSET_MSTARBLOSSEN_MASK)
#define I2C_INTENSET_MSTSTSTPERREN_MASK          (0x40U)
#define I2C_INTENSET_MSTSTSTPERREN_SHIFT         (6U)
#define I2C_INTENSET_MSTSTSTPERREN(x)            (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_MSTSTSTPERREN_SHIFT)) & I2C_INTENSET_MSTSTSTPERREN_MASK)
#define I2C_INTENSET_SLVPENDINGEN_MASK           (0x100U)
#define I2C_INTENSET_SLVPENDINGEN_SHIFT          (8U)
#define I2C_INTENSET_SLVPENDINGEN(x)             (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_SLVPENDINGEN_SHIFT)) & I2C_INTENSET_SLVPENDINGEN_MASK)
#define I2C_INTENSET_SLVNOTSTREN_MASK            (0x800U)
#define I2C_INTENSET_SLVNOTSTREN_SHIFT           (11U)
#define I2C_INTENSET_SLVNOTSTREN(x)              (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_SLVNOTSTREN_SHIFT)) & I2C_INTENSET_SLVNOTSTREN_MASK)
#define I2C_INTENSET_SLVDESELEN_MASK             (0x8000U)
#define I2C_INTENSET_SLVDESELEN_SHIFT            (15U)
#define I2C_INTENSET_SLVDESELEN(x)               (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_SLVDESELEN_SHIFT)) & I2C_INTENSET_SLVDESELEN_MASK)
#define I2C_INTENSET_MONRDYEN_MASK               (0x10000U)
#define I2C_INTENSET_MONRDYEN_SHIFT              (16U)
#define I2C_INTENSET_MONRDYEN(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_MONRDYEN_SHIFT)) & I2C_INTENSET_MONRDYEN_MASK)
#define I2C_INTENSET_MONOVEN_MASK                (0x20000U)
#define I2C_INTENSET_MONOVEN_SHIFT               (17U)
#define I2C_INTENSET_MONOVEN(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_MONOVEN_SHIFT)) & I2C_INTENSET_MONOVEN_MASK)
#define I2C_INTENSET_MONIDLEEN_MASK              (0x80000U)
#define I2C_INTENSET_MONIDLEEN_SHIFT             (19U)
#define I2C_INTENSET_MONIDLEEN(x)                (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_MONIDLEEN_SHIFT)) & I2C_INTENSET_MONIDLEEN_MASK)
#define I2C_INTENSET_EVENTTIMEOUTEN_MASK         (0x1000000U)
#define I2C_INTENSET_EVENTTIMEOUTEN_SHIFT        (24U)
#define I2C_INTENSET_EVENTTIMEOUTEN(x)           (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_EVENTTIMEOUTEN_SHIFT)) & I2C_INTENSET_EVENTTIMEOUTEN_MASK)
#define I2C_INTENSET_SCLTIMEOUTEN_MASK           (0x2000000U)
#define I2C_INTENSET_SCLTIMEOUTEN_SHIFT          (25U)
#define I2C_INTENSET_SCLTIMEOUTEN(x)             (((uint32_t)(((uint32_t)(x)) << I2C_INTENSET_SCLTIMEOUTEN_SHIFT)) & I2C_INTENSET_SCLTIMEOUTEN_MASK)

/*! @name INTENCLR - Interrupt Enable Clear register. */
#define I2C_INTENCLR_MSTPENDINGCLR_MASK          (0x1U)
#define I2C_INTENCLR_MSTPENDINGCLR_SHIFT         (0U)
#define I2C_INTENCLR_MSTPENDINGCLR(x)            (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_MSTPENDINGCLR_SHIFT)) & I2C_INTENCLR_MSTPENDINGCLR_MASK)
#define I2C_INTENCLR_MSTARBLOSSCLR_MASK          (0x10U)
#define I2C_INTENCLR_MSTARBLOSSCLR_SHIFT         (4U)
#define I2C_INTENCLR_MSTARBLOSSCLR(x)            (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_MSTARBLOSSCLR_SHIFT)) & I2C_INTENCLR_MSTARBLOSSCLR_MASK)
#define I2C_INTENCLR_MSTSTSTPERRCLR_MASK         (0x40U)
#define I2C_INTENCLR_MSTSTSTPERRCLR_SHIFT        (6U)
#define I2C_INTENCLR_MSTSTSTPERRCLR(x)           (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_MSTSTSTPERRCLR_SHIFT)) & I2C_INTENCLR_MSTSTSTPERRCLR_MASK)
#define I2C_INTENCLR_SLVPENDINGCLR_MASK          (0x100U)
#define I2C_INTENCLR_SLVPENDINGCLR_SHIFT         (8U)
#define I2C_INTENCLR_SLVPENDINGCLR(x)            (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_SLVPENDINGCLR_SHIFT)) & I2C_INTENCLR_SLVPENDINGCLR_MASK)
#define I2C_INTENCLR_SLVNOTSTRCLR_MASK           (0x800U)
#define I2C_INTENCLR_SLVNOTSTRCLR_SHIFT          (11U)
#define I2C_INTENCLR_SLVNOTSTRCLR(x)             (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_SLVNOTSTRCLR_SHIFT)) & I2C_INTENCLR_SLVNOTSTRCLR_MASK)
#define I2C_INTENCLR_SLVDESELCLR_MASK            (0x8000U)
#define I2C_INTENCLR_SLVDESELCLR_SHIFT           (15U)
#define I2C_INTENCLR_SLVDESELCLR(x)              (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_SLVDESELCLR_SHIFT)) & I2C_INTENCLR_SLVDESELCLR_MASK)
#define I2C_INTENCLR_MONRDYCLR_MASK              (0x10000U)
#define I2C_INTENCLR_MONRDYCLR_SHIFT             (16U)
#define I2C_INTENCLR_MONRDYCLR(x)                (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_MONRDYCLR_SHIFT)) & I2C_INTENCLR_MONRDYCLR_MASK)
#define I2C_INTENCLR_MONOVCLR_MASK               (0x20000U)
#define I2C_INTENCLR_MONOVCLR_SHIFT              (17U)
#define I2C_INTENCLR_MONOVCLR(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_MONOVCLR_SHIFT)) & I2C_INTENCLR_MONOVCLR_MASK)
#define I2C_INTENCLR_MONIDLECLR_MASK             (0x80000U)
#define I2C_INTENCLR_MONIDLECLR_SHIFT            (19U)
#define I2C_INTENCLR_MONIDLECLR(x)               (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_MONIDLECLR_SHIFT)) & I2C_INTENCLR_MONIDLECLR_MASK)
#define I2C_INTENCLR_EVENTTIMEOUTCLR_MASK        (0x1000000U)
#define I2C_INTENCLR_EVENTTIMEOUTCLR_SHIFT       (24U)
#define I2C_INTENCLR_EVENTTIMEOUTCLR(x)          (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_EVENTTIMEOUTCLR_SHIFT)) & I2C_INTENCLR_EVENTTIMEOUTCLR_MASK)
#define I2C_INTENCLR_SCLTIMEOUTCLR_MASK          (0x2000000U)
#define I2C_INTENCLR_SCLTIMEOUTCLR_SHIFT         (25U)
#define I2C_INTENCLR_SCLTIMEOUTCLR(x)            (((uint32_t)(((uint32_t)(x)) << I2C_INTENCLR_SCLTIMEOUTCLR_SHIFT)) & I2C_INTENCLR_SCLTIMEOUTCLR_MASK)

/*! @name TIMEOUT - Time-out value register. */
#define I2C_TIMEOUT_TOMIN_MASK                   (0xFU)
#define I2C_TIMEOUT_TOMIN_SHIFT                  (0U)
#define I2C_TIMEOUT_TOMIN(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_TIMEOUT_TOMIN_SHIFT)) & I2C_TIMEOUT_TOMIN_MASK)
#define I2C_TIMEOUT_TO_MASK                      (0xFFF0U)
#define I2C_TIMEOUT_TO_SHIFT                     (4U)
#define I2C_TIMEOUT_TO(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_TIMEOUT_TO_SHIFT)) & I2C_TIMEOUT_TO_MASK)

/*! @name CLKDIV - Clock pre-divider for the entire I2C interface. This determines what time increments are used for the MSTTIME register, and controls some timing of the Slave function. */
#define I2C_CLKDIV_DIVVAL_MASK                   (0xFFFFU)
#define I2C_CLKDIV_DIVVAL_SHIFT                  (0U)
#define I2C_CLKDIV_DIVVAL(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_CLKDIV_DIVVAL_SHIFT)) & I2C_CLKDIV_DIVVAL_MASK)

/*! @name INTSTAT - Interrupt Status register for Master, Slave, and Monitor functions. */
#define I2C_INTSTAT_MSTPENDING_MASK              (0x1U)
#define I2C_INTSTAT_MSTPENDING_SHIFT             (0U)
#define I2C_INTSTAT_MSTPENDING(x)                (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_MSTPENDING_SHIFT)) & I2C_INTSTAT_MSTPENDING_MASK)
#define I2C_INTSTAT_MSTARBLOSS_MASK              (0x10U)
#define I2C_INTSTAT_MSTARBLOSS_SHIFT             (4U)
#define I2C_INTSTAT_MSTARBLOSS(x)                (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_MSTARBLOSS_SHIFT)) & I2C_INTSTAT_MSTARBLOSS_MASK)
#define I2C_INTSTAT_MSTSTSTPERR_MASK             (0x40U)
#define I2C_INTSTAT_MSTSTSTPERR_SHIFT            (6U)
#define I2C_INTSTAT_MSTSTSTPERR(x)               (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_MSTSTSTPERR_SHIFT)) & I2C_INTSTAT_MSTSTSTPERR_MASK)
#define I2C_INTSTAT_SLVPENDING_MASK              (0x100U)
#define I2C_INTSTAT_SLVPENDING_SHIFT             (8U)
#define I2C_INTSTAT_SLVPENDING(x)                (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_SLVPENDING_SHIFT)) & I2C_INTSTAT_SLVPENDING_MASK)
#define I2C_INTSTAT_SLVNOTSTR_MASK               (0x800U)
#define I2C_INTSTAT_SLVNOTSTR_SHIFT              (11U)
#define I2C_INTSTAT_SLVNOTSTR(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_SLVNOTSTR_SHIFT)) & I2C_INTSTAT_SLVNOTSTR_MASK)
#define I2C_INTSTAT_SLVDESEL_MASK                (0x8000U)
#define I2C_INTSTAT_SLVDESEL_SHIFT               (15U)
#define I2C_INTSTAT_SLVDESEL(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_SLVDESEL_SHIFT)) & I2C_INTSTAT_SLVDESEL_MASK)
#define I2C_INTSTAT_MONRDY_MASK                  (0x10000U)
#define I2C_INTSTAT_MONRDY_SHIFT                 (16U)
#define I2C_INTSTAT_MONRDY(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_MONRDY_SHIFT)) & I2C_INTSTAT_MONRDY_MASK)
#define I2C_INTSTAT_MONOV_MASK                   (0x20000U)
#define I2C_INTSTAT_MONOV_SHIFT                  (17U)
#define I2C_INTSTAT_MONOV(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_MONOV_SHIFT)) & I2C_INTSTAT_MONOV_MASK)
#define I2C_INTSTAT_MONIDLE_MASK                 (0x80000U)
#define I2C_INTSTAT_MONIDLE_SHIFT                (19U)
#define I2C_INTSTAT_MONIDLE(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_MONIDLE_SHIFT)) & I2C_INTSTAT_MONIDLE_MASK)
#define I2C_INTSTAT_EVENTTIMEOUT_MASK            (0x1000000U)
#define I2C_INTSTAT_EVENTTIMEOUT_SHIFT           (24U)
#define I2C_INTSTAT_EVENTTIMEOUT(x)              (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_EVENTTIMEOUT_SHIFT)) & I2C_INTSTAT_EVENTTIMEOUT_MASK)
#define I2C_INTSTAT_SCLTIMEOUT_MASK              (0x2000000U)
#define I2C_INTSTAT_SCLTIMEOUT_SHIFT             (25U)
#define I2C_INTSTAT_SCLTIMEOUT(x)                (((uint32_t)(((uint32_t)(x)) << I2C_INTSTAT_SCLTIMEOUT_SHIFT)) & I2C_INTSTAT_SCLTIMEOUT_MASK)

/*! @name MSTCTL - Master control register. */
#define I2C_MSTCTL_MSTCONTINUE_MASK              (0x1U)
#define I2C_MSTCTL_MSTCONTINUE_SHIFT             (0U)
#define I2C_MSTCTL_MSTCONTINUE(x)                (((uint32_t)(((uint32_t)(x)) << I2C_MSTCTL_MSTCONTINUE_SHIFT)) & I2C_MSTCTL_MSTCONTINUE_MASK)
#define I2C_MSTCTL_MSTSTART_MASK                 (0x2U)
#define I2C_MSTCTL_MSTSTART_SHIFT                (1U)
#define I2C_MSTCTL_MSTSTART(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_MSTCTL_MSTSTART_SHIFT)) & I2C_MSTCTL_MSTSTART_MASK)
#define I2C_MSTCTL_MSTSTOP_MASK                  (0x4U)
#define I2C_MSTCTL_MSTSTOP_SHIFT                 (2U)
#define I2C_MSTCTL_MSTSTOP(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_MSTCTL_MSTSTOP_SHIFT)) & I2C_MSTCTL_MSTSTOP_MASK)
#define I2C_MSTCTL_MSTDMA_MASK                   (0x8U)
#define I2C_MSTCTL_MSTDMA_SHIFT                  (3U)
#define I2C_MSTCTL_MSTDMA(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_MSTCTL_MSTDMA_SHIFT)) & I2C_MSTCTL_MSTDMA_MASK)

/*! @name MSTTIME - Master timing configuration. */
#define I2C_MSTTIME_MSTSCLLOW_MASK               (0x7U)
#define I2C_MSTTIME_MSTSCLLOW_SHIFT              (0U)
#define I2C_MSTTIME_MSTSCLLOW(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_MSTTIME_MSTSCLLOW_SHIFT)) & I2C_MSTTIME_MSTSCLLOW_MASK)
#define I2C_MSTTIME_MSTSCLHIGH_MASK              (0x70U)
#define I2C_MSTTIME_MSTSCLHIGH_SHIFT             (4U)
#define I2C_MSTTIME_MSTSCLHIGH(x)                (((uint32_t)(((uint32_t)(x)) << I2C_MSTTIME_MSTSCLHIGH_SHIFT)) & I2C_MSTTIME_MSTSCLHIGH_MASK)

/*! @name MSTDAT - Combined Master receiver and transmitter data register. */
#define I2C_MSTDAT_DATA_MASK                     (0xFFU)
#define I2C_MSTDAT_DATA_SHIFT                    (0U)
#define I2C_MSTDAT_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_MSTDAT_DATA_SHIFT)) & I2C_MSTDAT_DATA_MASK)

/*! @name SLVCTL - Slave control register. */
#define I2C_SLVCTL_SLVCONTINUE_MASK              (0x1U)
#define I2C_SLVCTL_SLVCONTINUE_SHIFT             (0U)
#define I2C_SLVCTL_SLVCONTINUE(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SLVCTL_SLVCONTINUE_SHIFT)) & I2C_SLVCTL_SLVCONTINUE_MASK)
#define I2C_SLVCTL_SLVNACK_MASK                  (0x2U)
#define I2C_SLVCTL_SLVNACK_SHIFT                 (1U)
#define I2C_SLVCTL_SLVNACK(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_SLVCTL_SLVNACK_SHIFT)) & I2C_SLVCTL_SLVNACK_MASK)
#define I2C_SLVCTL_SLVDMA_MASK                   (0x8U)
#define I2C_SLVCTL_SLVDMA_SHIFT                  (3U)
#define I2C_SLVCTL_SLVDMA(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_SLVCTL_SLVDMA_SHIFT)) & I2C_SLVCTL_SLVDMA_MASK)
#define I2C_SLVCTL_AUTOACK_MASK                  (0x100U)
#define I2C_SLVCTL_AUTOACK_SHIFT                 (8U)
#define I2C_SLVCTL_AUTOACK(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_SLVCTL_AUTOACK_SHIFT)) & I2C_SLVCTL_AUTOACK_MASK)
#define I2C_SLVCTL_AUTOMATCHREAD_MASK            (0x200U)
#define I2C_SLVCTL_AUTOMATCHREAD_SHIFT           (9U)
#define I2C_SLVCTL_AUTOMATCHREAD(x)              (((uint32_t)(((uint32_t)(x)) << I2C_SLVCTL_AUTOMATCHREAD_SHIFT)) & I2C_SLVCTL_AUTOMATCHREAD_MASK)

/*! @name SLVDAT - Combined Slave receiver and transmitter data register. */
#define I2C_SLVDAT_DATA_MASK                     (0xFFU)
#define I2C_SLVDAT_DATA_SHIFT                    (0U)
#define I2C_SLVDAT_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_SLVDAT_DATA_SHIFT)) & I2C_SLVDAT_DATA_MASK)

/*! @name SLVADR - Slave address register. */
#define I2C_SLVADR_SADISABLE_MASK                (0x1U)
#define I2C_SLVADR_SADISABLE_SHIFT               (0U)
#define I2C_SLVADR_SADISABLE(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SLVADR_SADISABLE_SHIFT)) & I2C_SLVADR_SADISABLE_MASK)
#define I2C_SLVADR_SLVADR_MASK                   (0xFEU)
#define I2C_SLVADR_SLVADR_SHIFT                  (1U)
#define I2C_SLVADR_SLVADR(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_SLVADR_SLVADR_SHIFT)) & I2C_SLVADR_SLVADR_MASK)
#define I2C_SLVADR_AUTONACK_MASK                 (0x8000U)
#define I2C_SLVADR_AUTONACK_SHIFT                (15U)
#define I2C_SLVADR_AUTONACK(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SLVADR_AUTONACK_SHIFT)) & I2C_SLVADR_AUTONACK_MASK)

/* The count of I2C_SLVADR */
#define I2C_SLVADR_COUNT                         (4U)

/*! @name SLVQUAL0 - Slave Qualification for address 0. */
#define I2C_SLVQUAL0_QUALMODE0_MASK              (0x1U)
#define I2C_SLVQUAL0_QUALMODE0_SHIFT             (0U)
#define I2C_SLVQUAL0_QUALMODE0(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SLVQUAL0_QUALMODE0_SHIFT)) & I2C_SLVQUAL0_QUALMODE0_MASK)
#define I2C_SLVQUAL0_SLVQUAL0_MASK               (0xFEU)
#define I2C_SLVQUAL0_SLVQUAL0_SHIFT              (1U)
#define I2C_SLVQUAL0_SLVQUAL0(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SLVQUAL0_SLVQUAL0_SHIFT)) & I2C_SLVQUAL0_SLVQUAL0_MASK)

/*! @name MONRXDAT - Monitor receiver data register. */
#define I2C_MONRXDAT_MONRXDAT_MASK               (0xFFU)
#define I2C_MONRXDAT_MONRXDAT_SHIFT              (0U)
#define I2C_MONRXDAT_MONRXDAT(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_MONRXDAT_MONRXDAT_SHIFT)) & I2C_MONRXDAT_MONRXDAT_MASK)
#define I2C_MONRXDAT_MONSTART_MASK               (0x100U)
#define I2C_MONRXDAT_MONSTART_SHIFT              (8U)
#define I2C_MONRXDAT_MONSTART(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_MONRXDAT_MONSTART_SHIFT)) & I2C_MONRXDAT_MONSTART_MASK)
#define I2C_MONRXDAT_MONRESTART_MASK             (0x200U)
#define I2C_MONRXDAT_MONRESTART_SHIFT            (9U)
#define I2C_MONRXDAT_MONRESTART(x)               (((uint32_t)(((uint32_t)(x)) << I2C_MONRXDAT_MONRESTART_SHIFT)) & I2C_MONRXDAT_MONRESTART_MASK)
#define I2C_MONRXDAT_MONNACK_MASK                (0x400U)
#define I2C_MONRXDAT_MONNACK_SHIFT               (10U)
#define I2C_MONRXDAT_MONNACK(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_MONRXDAT_MONNACK_SHIFT)) & I2C_MONRXDAT_MONNACK_MASK)

/*! @name ID - I2C module Identification. This value appears in the shared Flexcomm peripheral ID register when I2C is selected. */
#define I2C_ID_APERTURE_MASK                     (0xFFU)
#define I2C_ID_APERTURE_SHIFT                    (0U)
#define I2C_ID_APERTURE(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_ID_APERTURE_SHIFT)) & I2C_ID_APERTURE_MASK)
#define I2C_ID_MINOR_REV_MASK                    (0xF00U)
#define I2C_ID_MINOR_REV_SHIFT                   (8U)
#define I2C_ID_MINOR_REV(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ID_MINOR_REV_SHIFT)) & I2C_ID_MINOR_REV_MASK)
#define I2C_ID_MAJOR_REV_MASK                    (0xF000U)
#define I2C_ID_MAJOR_REV_SHIFT                   (12U)
#define I2C_ID_MAJOR_REV(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ID_MAJOR_REV_SHIFT)) & I2C_ID_MAJOR_REV_MASK)
#define I2C_ID_ID_MASK                           (0xFFFF0000U)
#define I2C_ID_ID_SHIFT                          (16U)
#define I2C_ID_ID(x)                             (((uint32_t)(((uint32_t)(x)) << I2C_ID_ID_SHIFT)) & I2C_ID_ID_MASK)


/*!
 * @}
 */ /* end of group I2C_Register_Masks */


/* I2C - Peripheral instance base addresses */
/** Peripheral I2C0 base address */
#define I2C0_BASE                                (0x40086000u)
/** Peripheral I2C0 base pointer */
#define I2C0                                     ((I2C_Type *)I2C0_BASE)
/** Peripheral I2C1 base address */
#define I2C1_BASE                                (0x40087000u)
/** Peripheral I2C1 base pointer */
#define I2C1                                     ((I2C_Type *)I2C1_BASE)
/** Array initializer of I2C peripheral base addresses */
#define I2C_BASE_ADDRS                           { I2C0_BASE, I2C1_BASE }
/** Array initializer of I2C peripheral base pointers */
#define I2C_BASE_PTRS                            { I2C0, I2C1 }
/** Interrupt vectors for the I2C peripheral type */
#define I2C_IRQS                                 { FLEXCOMM1_IRQn, FLEXCOMM2_IRQn }

/*!
 * @}
 */ /* end of group I2C_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- INPUTMUX Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup INPUTMUX_Peripheral_Access_Layer INPUTMUX Peripheral Access Layer
 * @{
 */

/** INPUTMUX - Register Layout Typedef */
typedef struct {
  __IO uint32_t PINTSEL[4];                        /**< Pin interrupt select register, array offset: 0x0, array step: 0x4 */
       uint8_t RESERVED_0[496];
  __IO uint32_t DMA_ITRIG_INMUX[20];               /**< Trigger select register for DMA channel, array offset: 0x200, array step: 0x4 */
       uint8_t RESERVED_1[1456];
  __IO uint32_t DMA_OTRIG_INMUX[4];                /**< DMA output trigger selection to become DMA trigger, array offset: 0x800, array step: 0x4 */
} INPUTMUX_Type;

/* ----------------------------------------------------------------------------
   -- INPUTMUX Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup INPUTMUX_Register_Masks INPUTMUX Register Masks
 * @{
 */

/*! @name PINTSEL - Pin interrupt select register */
#define INPUTMUX_PINTSEL_INTPIN_MASK             (0x1FU)
#define INPUTMUX_PINTSEL_INTPIN_SHIFT            (0U)
#define INPUTMUX_PINTSEL_INTPIN(x)               (((uint32_t)(((uint32_t)(x)) << INPUTMUX_PINTSEL_INTPIN_SHIFT)) & INPUTMUX_PINTSEL_INTPIN_MASK)

/* The count of INPUTMUX_PINTSEL */
#define INPUTMUX_PINTSEL_COUNT                   (4U)

/*! @name DMA_ITRIG_INMUX - Trigger select register for DMA channel */
#define INPUTMUX_DMA_ITRIG_INMUX_INP_MASK        (0x1FU)
#define INPUTMUX_DMA_ITRIG_INMUX_INP_SHIFT       (0U)
#define INPUTMUX_DMA_ITRIG_INMUX_INP(x)          (((uint32_t)(((uint32_t)(x)) << INPUTMUX_DMA_ITRIG_INMUX_INP_SHIFT)) & INPUTMUX_DMA_ITRIG_INMUX_INP_MASK)

/* The count of INPUTMUX_DMA_ITRIG_INMUX */
#define INPUTMUX_DMA_ITRIG_INMUX_COUNT           (20U)

/*! @name DMA_OTRIG_INMUX - DMA output trigger selection to become DMA trigger */
#define INPUTMUX_DMA_OTRIG_INMUX_INP_MASK        (0x1FU)
#define INPUTMUX_DMA_OTRIG_INMUX_INP_SHIFT       (0U)
#define INPUTMUX_DMA_OTRIG_INMUX_INP(x)          (((uint32_t)(((uint32_t)(x)) << INPUTMUX_DMA_OTRIG_INMUX_INP_SHIFT)) & INPUTMUX_DMA_OTRIG_INMUX_INP_MASK)

/* The count of INPUTMUX_DMA_OTRIG_INMUX */
#define INPUTMUX_DMA_OTRIG_INMUX_COUNT           (4U)


/*!
 * @}
 */ /* end of group INPUTMUX_Register_Masks */


/* INPUTMUX - Peripheral instance base addresses */
/** Peripheral INPUTMUX base address */
#define INPUTMUX_BASE                            (0x40006200u)
/** Peripheral INPUTMUX base pointer */
#define INPUTMUX                                 ((INPUTMUX_Type *)INPUTMUX_BASE)
/** Array initializer of INPUTMUX peripheral base addresses */
#define INPUTMUX_BASE_ADDRS                      { INPUTMUX_BASE }
/** Array initializer of INPUTMUX peripheral base pointers */
#define INPUTMUX_BASE_PTRS                       { INPUTMUX }

/*!
 * @}
 */ /* end of group INPUTMUX_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PINT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PINT_Peripheral_Access_Layer PINT Peripheral Access Layer
 * @{
 */

/** PINT - Register Layout Typedef */
typedef struct {
  __IO uint32_t ISEL;                              /**< Pin Interrupt Mode register, offset: 0x0 */
  __IO uint32_t IENR;                              /**< Pin interrupt level or rising edge interrupt enable register, offset: 0x4 */
  __O  uint32_t SIENR;                             /**< Pin interrupt level or rising edge interrupt set register, offset: 0x8 */
  __O  uint32_t CIENR;                             /**< Pin interrupt level (rising edge interrupt) clear register, offset: 0xC */
  __IO uint32_t IENF;                              /**< Pin interrupt active level or falling edge interrupt enable register, offset: 0x10 */
  __O  uint32_t SIENF;                             /**< Pin interrupt active level or falling edge interrupt set register, offset: 0x14 */
  __O  uint32_t CIENF;                             /**< Pin interrupt active level or falling edge interrupt clear register, offset: 0x18 */
  __IO uint32_t RISE;                              /**< Pin interrupt rising edge register, offset: 0x1C */
  __IO uint32_t FALL;                              /**< Pin interrupt falling edge register, offset: 0x20 */
  __IO uint32_t IST;                               /**< Pin interrupt status register, offset: 0x24 */
  __IO uint32_t PMCTRL;                            /**< Pattern match interrupt control register, offset: 0x28 */
  __IO uint32_t PMSRC;                             /**< Pattern match interrupt bit-slice source register, offset: 0x2C */
  __IO uint32_t PMCFG;                             /**< Pattern match interrupt bit slice configuration register, offset: 0x30 */
} PINT_Type;

/* ----------------------------------------------------------------------------
   -- PINT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PINT_Register_Masks PINT Register Masks
 * @{
 */

/*! @name ISEL - Pin Interrupt Mode register */
#define PINT_ISEL_PMODE_MASK                     (0xFU)
#define PINT_ISEL_PMODE_SHIFT                    (0U)
#define PINT_ISEL_PMODE(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_ISEL_PMODE_SHIFT)) & PINT_ISEL_PMODE_MASK)

/*! @name IENR - Pin interrupt level or rising edge interrupt enable register */
#define PINT_IENR_ENRL_MASK                      (0xFU)
#define PINT_IENR_ENRL_SHIFT                     (0U)
#define PINT_IENR_ENRL(x)                        (((uint32_t)(((uint32_t)(x)) << PINT_IENR_ENRL_SHIFT)) & PINT_IENR_ENRL_MASK)

/*! @name SIENR - Pin interrupt level or rising edge interrupt set register */
#define PINT_SIENR_SETENRL_MASK                  (0xFU)
#define PINT_SIENR_SETENRL_SHIFT                 (0U)
#define PINT_SIENR_SETENRL(x)                    (((uint32_t)(((uint32_t)(x)) << PINT_SIENR_SETENRL_SHIFT)) & PINT_SIENR_SETENRL_MASK)

/*! @name CIENR - Pin interrupt level (rising edge interrupt) clear register */
#define PINT_CIENR_CENRL_MASK                    (0xFU)
#define PINT_CIENR_CENRL_SHIFT                   (0U)
#define PINT_CIENR_CENRL(x)                      (((uint32_t)(((uint32_t)(x)) << PINT_CIENR_CENRL_SHIFT)) & PINT_CIENR_CENRL_MASK)

/*! @name IENF - Pin interrupt active level or falling edge interrupt enable register */
#define PINT_IENF_ENAF_MASK                      (0xFU)
#define PINT_IENF_ENAF_SHIFT                     (0U)
#define PINT_IENF_ENAF(x)                        (((uint32_t)(((uint32_t)(x)) << PINT_IENF_ENAF_SHIFT)) & PINT_IENF_ENAF_MASK)

/*! @name SIENF - Pin interrupt active level or falling edge interrupt set register */
#define PINT_SIENF_SETENAF_MASK                  (0xFU)
#define PINT_SIENF_SETENAF_SHIFT                 (0U)
#define PINT_SIENF_SETENAF(x)                    (((uint32_t)(((uint32_t)(x)) << PINT_SIENF_SETENAF_SHIFT)) & PINT_SIENF_SETENAF_MASK)

/*! @name CIENF - Pin interrupt active level or falling edge interrupt clear register */
#define PINT_CIENF_CENAF_MASK                    (0xFU)
#define PINT_CIENF_CENAF_SHIFT                   (0U)
#define PINT_CIENF_CENAF(x)                      (((uint32_t)(((uint32_t)(x)) << PINT_CIENF_CENAF_SHIFT)) & PINT_CIENF_CENAF_MASK)

/*! @name RISE - Pin interrupt rising edge register */
#define PINT_RISE_RDET_MASK                      (0xFU)
#define PINT_RISE_RDET_SHIFT                     (0U)
#define PINT_RISE_RDET(x)                        (((uint32_t)(((uint32_t)(x)) << PINT_RISE_RDET_SHIFT)) & PINT_RISE_RDET_MASK)

/*! @name FALL - Pin interrupt falling edge register */
#define PINT_FALL_FDET_MASK                      (0xFU)
#define PINT_FALL_FDET_SHIFT                     (0U)
#define PINT_FALL_FDET(x)                        (((uint32_t)(((uint32_t)(x)) << PINT_FALL_FDET_SHIFT)) & PINT_FALL_FDET_MASK)

/*! @name IST - Pin interrupt status register */
#define PINT_IST_PSTAT_MASK                      (0xFU)
#define PINT_IST_PSTAT_SHIFT                     (0U)
#define PINT_IST_PSTAT(x)                        (((uint32_t)(((uint32_t)(x)) << PINT_IST_PSTAT_SHIFT)) & PINT_IST_PSTAT_MASK)

/*! @name PMCTRL - Pattern match interrupt control register */
#define PINT_PMCTRL_SEL_PMATCH_MASK              (0x1U)
#define PINT_PMCTRL_SEL_PMATCH_SHIFT             (0U)
#define PINT_PMCTRL_SEL_PMATCH(x)                (((uint32_t)(((uint32_t)(x)) << PINT_PMCTRL_SEL_PMATCH_SHIFT)) & PINT_PMCTRL_SEL_PMATCH_MASK)
#define PINT_PMCTRL_ENA_RXEV_MASK                (0x2U)
#define PINT_PMCTRL_ENA_RXEV_SHIFT               (1U)
#define PINT_PMCTRL_ENA_RXEV(x)                  (((uint32_t)(((uint32_t)(x)) << PINT_PMCTRL_ENA_RXEV_SHIFT)) & PINT_PMCTRL_ENA_RXEV_MASK)
#define PINT_PMCTRL_PMAT_MASK                    (0xFF000000U)
#define PINT_PMCTRL_PMAT_SHIFT                   (24U)
#define PINT_PMCTRL_PMAT(x)                      (((uint32_t)(((uint32_t)(x)) << PINT_PMCTRL_PMAT_SHIFT)) & PINT_PMCTRL_PMAT_MASK)

/*! @name PMSRC - Pattern match interrupt bit-slice source register */
#define PINT_PMSRC_SRC0_MASK                     (0x700U)
#define PINT_PMSRC_SRC0_SHIFT                    (8U)
#define PINT_PMSRC_SRC0(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC0_SHIFT)) & PINT_PMSRC_SRC0_MASK)
#define PINT_PMSRC_SRC1_MASK                     (0x3800U)
#define PINT_PMSRC_SRC1_SHIFT                    (11U)
#define PINT_PMSRC_SRC1(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC1_SHIFT)) & PINT_PMSRC_SRC1_MASK)
#define PINT_PMSRC_SRC2_MASK                     (0x1C000U)
#define PINT_PMSRC_SRC2_SHIFT                    (14U)
#define PINT_PMSRC_SRC2(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC2_SHIFT)) & PINT_PMSRC_SRC2_MASK)
#define PINT_PMSRC_SRC3_MASK                     (0xE0000U)
#define PINT_PMSRC_SRC3_SHIFT                    (17U)
#define PINT_PMSRC_SRC3(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC3_SHIFT)) & PINT_PMSRC_SRC3_MASK)
#define PINT_PMSRC_SRC4_MASK                     (0x700000U)
#define PINT_PMSRC_SRC4_SHIFT                    (20U)
#define PINT_PMSRC_SRC4(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC4_SHIFT)) & PINT_PMSRC_SRC4_MASK)
#define PINT_PMSRC_SRC5_MASK                     (0x3800000U)
#define PINT_PMSRC_SRC5_SHIFT                    (23U)
#define PINT_PMSRC_SRC5(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC5_SHIFT)) & PINT_PMSRC_SRC5_MASK)
#define PINT_PMSRC_SRC6_MASK                     (0x1C000000U)
#define PINT_PMSRC_SRC6_SHIFT                    (26U)
#define PINT_PMSRC_SRC6(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC6_SHIFT)) & PINT_PMSRC_SRC6_MASK)
#define PINT_PMSRC_SRC7_MASK                     (0xE0000000U)
#define PINT_PMSRC_SRC7_SHIFT                    (29U)
#define PINT_PMSRC_SRC7(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMSRC_SRC7_SHIFT)) & PINT_PMSRC_SRC7_MASK)

/*! @name PMCFG - Pattern match interrupt bit slice configuration register */
#define PINT_PMCFG_PROD_ENDPTS0_MASK             (0x1U)
#define PINT_PMCFG_PROD_ENDPTS0_SHIFT            (0U)
#define PINT_PMCFG_PROD_ENDPTS0(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS0_SHIFT)) & PINT_PMCFG_PROD_ENDPTS0_MASK)
#define PINT_PMCFG_PROD_ENDPTS1_MASK             (0x2U)
#define PINT_PMCFG_PROD_ENDPTS1_SHIFT            (1U)
#define PINT_PMCFG_PROD_ENDPTS1(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS1_SHIFT)) & PINT_PMCFG_PROD_ENDPTS1_MASK)
#define PINT_PMCFG_PROD_ENDPTS2_MASK             (0x4U)
#define PINT_PMCFG_PROD_ENDPTS2_SHIFT            (2U)
#define PINT_PMCFG_PROD_ENDPTS2(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS2_SHIFT)) & PINT_PMCFG_PROD_ENDPTS2_MASK)
#define PINT_PMCFG_PROD_ENDPTS3_MASK             (0x8U)
#define PINT_PMCFG_PROD_ENDPTS3_SHIFT            (3U)
#define PINT_PMCFG_PROD_ENDPTS3(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS3_SHIFT)) & PINT_PMCFG_PROD_ENDPTS3_MASK)
#define PINT_PMCFG_PROD_ENDPTS4_MASK             (0x10U)
#define PINT_PMCFG_PROD_ENDPTS4_SHIFT            (4U)
#define PINT_PMCFG_PROD_ENDPTS4(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS4_SHIFT)) & PINT_PMCFG_PROD_ENDPTS4_MASK)
#define PINT_PMCFG_PROD_ENDPTS5_MASK             (0x20U)
#define PINT_PMCFG_PROD_ENDPTS5_SHIFT            (5U)
#define PINT_PMCFG_PROD_ENDPTS5(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS5_SHIFT)) & PINT_PMCFG_PROD_ENDPTS5_MASK)
#define PINT_PMCFG_PROD_ENDPTS6_MASK             (0x40U)
#define PINT_PMCFG_PROD_ENDPTS6_SHIFT            (6U)
#define PINT_PMCFG_PROD_ENDPTS6(x)               (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_PROD_ENDPTS6_SHIFT)) & PINT_PMCFG_PROD_ENDPTS6_MASK)
#define PINT_PMCFG_CFG0_MASK                     (0x700U)
#define PINT_PMCFG_CFG0_SHIFT                    (8U)
#define PINT_PMCFG_CFG0(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG0_SHIFT)) & PINT_PMCFG_CFG0_MASK)
#define PINT_PMCFG_CFG1_MASK                     (0x3800U)
#define PINT_PMCFG_CFG1_SHIFT                    (11U)
#define PINT_PMCFG_CFG1(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG1_SHIFT)) & PINT_PMCFG_CFG1_MASK)
#define PINT_PMCFG_CFG2_MASK                     (0x1C000U)
#define PINT_PMCFG_CFG2_SHIFT                    (14U)
#define PINT_PMCFG_CFG2(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG2_SHIFT)) & PINT_PMCFG_CFG2_MASK)
#define PINT_PMCFG_CFG3_MASK                     (0xE0000U)
#define PINT_PMCFG_CFG3_SHIFT                    (17U)
#define PINT_PMCFG_CFG3(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG3_SHIFT)) & PINT_PMCFG_CFG3_MASK)
#define PINT_PMCFG_CFG4_MASK                     (0x700000U)
#define PINT_PMCFG_CFG4_SHIFT                    (20U)
#define PINT_PMCFG_CFG4(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG4_SHIFT)) & PINT_PMCFG_CFG4_MASK)
#define PINT_PMCFG_CFG5_MASK                     (0x3800000U)
#define PINT_PMCFG_CFG5_SHIFT                    (23U)
#define PINT_PMCFG_CFG5(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG5_SHIFT)) & PINT_PMCFG_CFG5_MASK)
#define PINT_PMCFG_CFG6_MASK                     (0x1C000000U)
#define PINT_PMCFG_CFG6_SHIFT                    (26U)
#define PINT_PMCFG_CFG6(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG6_SHIFT)) & PINT_PMCFG_CFG6_MASK)
#define PINT_PMCFG_CFG7_MASK                     (0xE0000000U)
#define PINT_PMCFG_CFG7_SHIFT                    (29U)
#define PINT_PMCFG_CFG7(x)                       (((uint32_t)(((uint32_t)(x)) << PINT_PMCFG_CFG7_SHIFT)) & PINT_PMCFG_CFG7_MASK)


/*!
 * @}
 */ /* end of group PINT_Register_Masks */


/* PINT - Peripheral instance base addresses */
/** Peripheral PINT base address */
#define PINT_BASE                                (0x40006000u)
/** Peripheral PINT base pointer */
#define PINT                                     ((PINT_Type *)PINT_BASE)
/** Array initializer of PINT peripheral base addresses */
#define PINT_BASE_ADDRS                          { PINT_BASE }
/** Array initializer of PINT peripheral base pointers */
#define PINT_BASE_PTRS                           { PINT }
/** Interrupt vectors for the PINT peripheral type */
#define PINT_IRQS                                { PIN_INT0_IRQn, PIN_INT1_IRQn, PIN_INT2_IRQn, PIN_INT3_IRQn }

/*!
 * @}
 */ /* end of group PINT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PROP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PROP_Peripheral_Access_Layer PROP Peripheral Access Layer
 * @{
 */

/** PROP - Register Layout Typedef */
typedef struct {
  __IO uint32_t TX_BUF;                            /**< transmit data buffer input port register, offset: 0x0 */
  __I  uint32_t RX_BUF;                            /**< received data buffer output register, offset: 0x4 */
  __IO uint32_t STAT;                              /**< status register, offset: 0x8 */
} PROP_Type;

/* ----------------------------------------------------------------------------
   -- PROP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PROP_Register_Masks PROP Register Masks
 * @{
 */

/*! @name TX_BUF - transmit data buffer input port register */
#define PROP_TX_BUF_TX_BUF_MASK                  (0xFFU)
#define PROP_TX_BUF_TX_BUF_SHIFT                 (0U)
#define PROP_TX_BUF_TX_BUF(x)                    (((uint32_t)(((uint32_t)(x)) << PROP_TX_BUF_TX_BUF_SHIFT)) & PROP_TX_BUF_TX_BUF_MASK)

/*! @name RX_BUF - received data buffer output register */
#define PROP_RX_BUF_RX_BUF_MASK                  (0xFFU)
#define PROP_RX_BUF_RX_BUF_SHIFT                 (0U)
#define PROP_RX_BUF_RX_BUF(x)                    (((uint32_t)(((uint32_t)(x)) << PROP_RX_BUF_RX_BUF_SHIFT)) & PROP_RX_BUF_RX_BUF_MASK)

/*! @name STAT - status register */
#define PROP_STAT_BIT_ORDER_MASK                 (0x1U)
#define PROP_STAT_BIT_ORDER_SHIFT                (0U)
#define PROP_STAT_BIT_ORDER(x)                   (((uint32_t)(((uint32_t)(x)) << PROP_STAT_BIT_ORDER_SHIFT)) & PROP_STAT_BIT_ORDER_MASK)
#define PROP_STAT_TX_INTEN_MASK                  (0x2U)
#define PROP_STAT_TX_INTEN_SHIFT                 (1U)
#define PROP_STAT_TX_INTEN(x)                    (((uint32_t)(((uint32_t)(x)) << PROP_STAT_TX_INTEN_SHIFT)) & PROP_STAT_TX_INTEN_MASK)
#define PROP_STAT_RX_INTEN_MASK                  (0x4U)
#define PROP_STAT_RX_INTEN_SHIFT                 (2U)
#define PROP_STAT_RX_INTEN(x)                    (((uint32_t)(((uint32_t)(x)) << PROP_STAT_RX_INTEN_SHIFT)) & PROP_STAT_RX_INTEN_MASK)
#define PROP_STAT_RX_INT_MASK                    (0x8U)
#define PROP_STAT_RX_INT_SHIFT                   (3U)
#define PROP_STAT_RX_INT(x)                      (((uint32_t)(((uint32_t)(x)) << PROP_STAT_RX_INT_SHIFT)) & PROP_STAT_RX_INT_MASK)
#define PROP_STAT_TX_INT_MASK                    (0x10U)
#define PROP_STAT_TX_INT_SHIFT                   (4U)
#define PROP_STAT_TX_INT(x)                      (((uint32_t)(((uint32_t)(x)) << PROP_STAT_TX_INT_SHIFT)) & PROP_STAT_TX_INT_MASK)
#define PROP_STAT_RX_BUSY_MASK                   (0x20U)
#define PROP_STAT_RX_BUSY_SHIFT                  (5U)
#define PROP_STAT_RX_BUSY(x)                     (((uint32_t)(((uint32_t)(x)) << PROP_STAT_RX_BUSY_SHIFT)) & PROP_STAT_RX_BUSY_MASK)
#define PROP_STAT_TX_BUSY_MASK                   (0x40U)
#define PROP_STAT_TX_BUSY_SHIFT                  (6U)
#define PROP_STAT_TX_BUSY(x)                     (((uint32_t)(((uint32_t)(x)) << PROP_STAT_TX_BUSY_SHIFT)) & PROP_STAT_TX_BUSY_MASK)
#define PROP_STAT_CLR_MASK                       (0x80U)
#define PROP_STAT_CLR_SHIFT                      (7U)
#define PROP_STAT_CLR(x)                         (((uint32_t)(((uint32_t)(x)) << PROP_STAT_CLR_SHIFT)) & PROP_STAT_CLR_MASK)


/*!
 * @}
 */ /* end of group PROP_Register_Masks */


/* PROP - Peripheral instance base addresses */
/** Peripheral PROP base address */
#define PROP_BASE                                (0x4000D000u)
/** Peripheral PROP base pointer */
#define PROP                                     ((PROP_Type *)PROP_BASE)
/** Array initializer of PROP peripheral base addresses */
#define PROP_BASE_ADDRS                          { PROP_BASE }
/** Array initializer of PROP peripheral base pointers */
#define PROP_BASE_PTRS                           { PROP }

/*!
 * @}
 */ /* end of group PROP_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- QDEC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup QDEC_Peripheral_Access_Layer QDEC Peripheral Access Layer
 * @{
 */

/** QDEC - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL;                              /**< control register, offset: 0x0 */
  __IO uint32_t SAMP_CTRL;                         /**< QDEC sample settting register, offset: 0x4 */
  __I  uint32_t SAMPLE;                            /**< QDEC sample result register, offset: 0x8 */
  __I  uint32_t ACC;                               /**< QDEC accumulate register, offset: 0xC */
  __I  uint32_t ACC_R;                             /**< QDEC accumulate snapshot register, offset: 0x10 */
  __I  uint32_t DB;                                /**< double sample register, offset: 0x14 */
  __I  uint32_t DB_R;                              /**< DB snapshot register, offset: 0x18 */
  __IO uint32_t INT;                               /**< interrupt register, offset: 0x1C */
  __IO uint32_t INTEN;                             /**< interrupt mask register, offset: 0x20 */
  __I  uint32_t STAT;                              /**< QDEC is running, offset: 0x24 */
} QDEC_Type;

/* ----------------------------------------------------------------------------
   -- QDEC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup QDEC_Register_Masks QDEC Register Masks
 * @{
 */

/*! @name CTRL - control register */
#define QDEC_CTRL_QDEC_EN_MASK                   (0x1U)
#define QDEC_CTRL_QDEC_EN_SHIFT                  (0U)
#define QDEC_CTRL_QDEC_EN(x)                     (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_QDEC_EN_SHIFT)) & QDEC_CTRL_QDEC_EN_MASK)
#define QDEC_CTRL_START_MASK                     (0x2U)
#define QDEC_CTRL_START_SHIFT                    (1U)
#define QDEC_CTRL_START(x)                       (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_START_SHIFT)) & QDEC_CTRL_START_MASK)
#define QDEC_CTRL_STOP_MASK                      (0x4U)
#define QDEC_CTRL_STOP_SHIFT                     (2U)
#define QDEC_CTRL_STOP(x)                        (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_STOP_SHIFT)) & QDEC_CTRL_STOP_MASK)
#define QDEC_CTRL_SOFT_CLR_MASK                  (0x8U)
#define QDEC_CTRL_SOFT_CLR_SHIFT                 (3U)
#define QDEC_CTRL_SOFT_CLR(x)                    (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_SOFT_CLR_SHIFT)) & QDEC_CTRL_SOFT_CLR_MASK)
#define QDEC_CTRL_AUTO_CLR_EN_MASK               (0x10U)
#define QDEC_CTRL_AUTO_CLR_EN_SHIFT              (4U)
#define QDEC_CTRL_AUTO_CLR_EN(x)                 (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_AUTO_CLR_EN_SHIFT)) & QDEC_CTRL_AUTO_CLR_EN_MASK)
#define QDEC_CTRL_SINGLE_SAMPLE_SRST_EN_MASK     (0x20U)
#define QDEC_CTRL_SINGLE_SAMPLE_SRST_EN_SHIFT    (5U)
#define QDEC_CTRL_SINGLE_SAMPLE_SRST_EN(x)       (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_SINGLE_SAMPLE_SRST_EN_SHIFT)) & QDEC_CTRL_SINGLE_SAMPLE_SRST_EN_MASK)
#define QDEC_CTRL_DB_FILTER_EN_MASK              (0x40U)
#define QDEC_CTRL_DB_FILTER_EN_SHIFT             (6U)
#define QDEC_CTRL_DB_FILTER_EN(x)                (((uint32_t)(((uint32_t)(x)) << QDEC_CTRL_DB_FILTER_EN_SHIFT)) & QDEC_CTRL_DB_FILTER_EN_MASK)

/*! @name SAMP_CTRL - QDEC sample settting register */
#define QDEC_SAMP_CTRL_DIVIDE_MASK               (0x1FU)
#define QDEC_SAMP_CTRL_DIVIDE_SHIFT              (0U)
#define QDEC_SAMP_CTRL_DIVIDE(x)                 (((uint32_t)(((uint32_t)(x)) << QDEC_SAMP_CTRL_DIVIDE_SHIFT)) & QDEC_SAMP_CTRL_DIVIDE_MASK)
#define QDEC_SAMP_CTRL_PTS_MASK                  (0xF00U)
#define QDEC_SAMP_CTRL_PTS_SHIFT                 (8U)
#define QDEC_SAMP_CTRL_PTS(x)                    (((uint32_t)(((uint32_t)(x)) << QDEC_SAMP_CTRL_PTS_SHIFT)) & QDEC_SAMP_CTRL_PTS_MASK)
#define QDEC_SAMP_CTRL_DB_SAMP_DIV_MASK          (0xF0000U)
#define QDEC_SAMP_CTRL_DB_SAMP_DIV_SHIFT         (16U)
#define QDEC_SAMP_CTRL_DB_SAMP_DIV(x)            (((uint32_t)(((uint32_t)(x)) << QDEC_SAMP_CTRL_DB_SAMP_DIV_SHIFT)) & QDEC_SAMP_CTRL_DB_SAMP_DIV_MASK)

/*! @name SAMPLE - QDEC sample result register */
#define QDEC_SAMPLE_SAMPLE_MASK                  (0x3U)
#define QDEC_SAMPLE_SAMPLE_SHIFT                 (0U)
#define QDEC_SAMPLE_SAMPLE(x)                    (((uint32_t)(((uint32_t)(x)) << QDEC_SAMPLE_SAMPLE_SHIFT)) & QDEC_SAMPLE_SAMPLE_MASK)

/*! @name ACC - QDEC accumulate register */
#define QDEC_ACC_ACC_MASK                        (0x7FFU)
#define QDEC_ACC_ACC_SHIFT                       (0U)
#define QDEC_ACC_ACC(x)                          (((uint32_t)(((uint32_t)(x)) << QDEC_ACC_ACC_SHIFT)) & QDEC_ACC_ACC_MASK)

/*! @name ACC_R - QDEC accumulate snapshot register */
#define QDEC_ACC_R_ACC_R_MASK                    (0x7FFU)
#define QDEC_ACC_R_ACC_R_SHIFT                   (0U)
#define QDEC_ACC_R_ACC_R(x)                      (((uint32_t)(((uint32_t)(x)) << QDEC_ACC_R_ACC_R_SHIFT)) & QDEC_ACC_R_ACC_R_MASK)

/*! @name DB - double sample register */
#define QDEC_DB_DB_MASK                          (0xFU)
#define QDEC_DB_DB_SHIFT                         (0U)
#define QDEC_DB_DB(x)                            (((uint32_t)(((uint32_t)(x)) << QDEC_DB_DB_SHIFT)) & QDEC_DB_DB_MASK)

/*! @name DB_R - DB snapshot register */
#define QDEC_DB_R_DB_R_MASK                      (0xFU)
#define QDEC_DB_R_DB_R_SHIFT                     (0U)
#define QDEC_DB_R_DB_R(x)                        (((uint32_t)(((uint32_t)(x)) << QDEC_DB_R_DB_R_SHIFT)) & QDEC_DB_R_DB_R_MASK)

/*! @name INT - interrupt register */
#define QDEC_INT_SINGLE_SAMPLE_MASK              (0x1U)
#define QDEC_INT_SINGLE_SAMPLE_SHIFT             (0U)
#define QDEC_INT_SINGLE_SAMPLE(x)                (((uint32_t)(((uint32_t)(x)) << QDEC_INT_SINGLE_SAMPLE_SHIFT)) & QDEC_INT_SINGLE_SAMPLE_MASK)
#define QDEC_INT_SAMPLE_END_MASK                 (0x2U)
#define QDEC_INT_SAMPLE_END_SHIFT                (1U)
#define QDEC_INT_SAMPLE_END(x)                   (((uint32_t)(((uint32_t)(x)) << QDEC_INT_SAMPLE_END_SHIFT)) & QDEC_INT_SAMPLE_END_MASK)
#define QDEC_INT_ACC_OF_MASK                     (0x4U)
#define QDEC_INT_ACC_OF_SHIFT                    (2U)
#define QDEC_INT_ACC_OF(x)                       (((uint32_t)(((uint32_t)(x)) << QDEC_INT_ACC_OF_SHIFT)) & QDEC_INT_ACC_OF_MASK)
#define QDEC_INT_DB_OF_MASK                      (0x8U)
#define QDEC_INT_DB_OF_SHIFT                     (3U)
#define QDEC_INT_DB_OF(x)                        (((uint32_t)(((uint32_t)(x)) << QDEC_INT_DB_OF_SHIFT)) & QDEC_INT_DB_OF_MASK)

/*! @name INTEN - interrupt mask register */
#define QDEC_INTEN_SINGLE_SAMPLE_INTEN_MASK      (0x1U)
#define QDEC_INTEN_SINGLE_SAMPLE_INTEN_SHIFT     (0U)
#define QDEC_INTEN_SINGLE_SAMPLE_INTEN(x)        (((uint32_t)(((uint32_t)(x)) << QDEC_INTEN_SINGLE_SAMPLE_INTEN_SHIFT)) & QDEC_INTEN_SINGLE_SAMPLE_INTEN_MASK)
#define QDEC_INTEN_SAMPLE_END_INTEN_MASK         (0x2U)
#define QDEC_INTEN_SAMPLE_END_INTEN_SHIFT        (1U)
#define QDEC_INTEN_SAMPLE_END_INTEN(x)           (((uint32_t)(((uint32_t)(x)) << QDEC_INTEN_SAMPLE_END_INTEN_SHIFT)) & QDEC_INTEN_SAMPLE_END_INTEN_MASK)
#define QDEC_INTEN_ACC_OF_INTEN_MASK             (0x4U)
#define QDEC_INTEN_ACC_OF_INTEN_SHIFT            (2U)
#define QDEC_INTEN_ACC_OF_INTEN(x)               (((uint32_t)(((uint32_t)(x)) << QDEC_INTEN_ACC_OF_INTEN_SHIFT)) & QDEC_INTEN_ACC_OF_INTEN_MASK)
#define QDEC_INTEN_DB_OF_INTEN_MASK              (0x8U)
#define QDEC_INTEN_DB_OF_INTEN_SHIFT             (3U)
#define QDEC_INTEN_DB_OF_INTEN(x)                (((uint32_t)(((uint32_t)(x)) << QDEC_INTEN_DB_OF_INTEN_SHIFT)) & QDEC_INTEN_DB_OF_INTEN_MASK)

/*! @name STAT - QDEC is running */
#define QDEC_STAT_BUSY_MASK                      (0x1U)
#define QDEC_STAT_BUSY_SHIFT                     (0U)
#define QDEC_STAT_BUSY(x)                        (((uint32_t)(((uint32_t)(x)) << QDEC_STAT_BUSY_SHIFT)) & QDEC_STAT_BUSY_MASK)


/*!
 * @}
 */ /* end of group QDEC_Register_Masks */


/* QDEC - Peripheral instance base addresses */
/** Peripheral QDEC0 base address */
#define QDEC0_BASE                               (0x40009000u)
/** Peripheral QDEC0 base pointer */
#define QDEC0                                    ((QDEC_Type *)QDEC0_BASE)
/** Peripheral QDEC1 base address */
#define QDEC1_BASE                               (0x40009800u)
/** Peripheral QDEC1 base pointer */
#define QDEC1                                    ((QDEC_Type *)QDEC1_BASE)
/** Array initializer of QDEC peripheral base addresses */
#define QDEC_BASE_ADDRS                          { QDEC0_BASE, QDEC1_BASE }
/** Array initializer of QDEC peripheral base pointers */
#define QDEC_BASE_PTRS                           { QDEC0, QDEC1 }
/** Interrupt vectors for the QDEC peripheral type */
#define QDEC_IRQS                                { QDEC0_IRQn, QDEC1_IRQn }

/*!
 * @}
 */ /* end of group QDEC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RNG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RNG_Peripheral_Access_Layer RNG Peripheral Access Layer
 * @{
 */

/** RNG - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL;                              /**< control register, offset: 0x0 */
  __I  uint32_t STAT;                              /**< status register, offset: 0x4 */
  __I  uint32_t DATA;                              /**< random data output register, offset: 0x8 */
  __IO uint32_t INT;                               /**< interrupt register, offset: 0xC */
  __IO uint32_t INTEN;                             /**< interrupt mask register, offset: 0x10 */
} RNG_Type;

/* ----------------------------------------------------------------------------
   -- RNG Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RNG_Register_Masks RNG Register Masks
 * @{
 */

/*! @name CTRL - control register */
#define RNG_CTRL_ENABLE_MASK                     (0x1U)
#define RNG_CTRL_ENABLE_SHIFT                    (0U)
#define RNG_CTRL_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << RNG_CTRL_ENABLE_SHIFT)) & RNG_CTRL_ENABLE_MASK)
#define RNG_CTRL_START_MASK                      (0x2U)
#define RNG_CTRL_START_SHIFT                     (1U)
#define RNG_CTRL_START(x)                        (((uint32_t)(((uint32_t)(x)) << RNG_CTRL_START_SHIFT)) & RNG_CTRL_START_MASK)
#define RNG_CTRL_NUM_MASK                        (0x30U)
#define RNG_CTRL_NUM_SHIFT                       (4U)
#define RNG_CTRL_NUM(x)                          (((uint32_t)(((uint32_t)(x)) << RNG_CTRL_NUM_SHIFT)) & RNG_CTRL_NUM_MASK)

/*! @name STAT - status register */
#define RNG_STAT_BUSY_MASK                       (0x1U)
#define RNG_STAT_BUSY_SHIFT                      (0U)
#define RNG_STAT_BUSY(x)                         (((uint32_t)(((uint32_t)(x)) << RNG_STAT_BUSY_SHIFT)) & RNG_STAT_BUSY_MASK)

/*! @name DATA - random data output register */
#define RNG_DATA_DATA_MASK                       (0xFFFFFFFFU)
#define RNG_DATA_DATA_SHIFT                      (0U)
#define RNG_DATA_DATA(x)                         (((uint32_t)(((uint32_t)(x)) << RNG_DATA_DATA_SHIFT)) & RNG_DATA_DATA_MASK)

/*! @name INT - interrupt register */
#define RNG_INT_DONE_MASK                        (0x1U)
#define RNG_INT_DONE_SHIFT                       (0U)
#define RNG_INT_DONE(x)                          (((uint32_t)(((uint32_t)(x)) << RNG_INT_DONE_SHIFT)) & RNG_INT_DONE_MASK)

/*! @name INTEN - interrupt mask register */
#define RNG_INTEN_DONE_INTEN_MASK                (0x1U)
#define RNG_INTEN_DONE_INTEN_SHIFT               (0U)
#define RNG_INTEN_DONE_INTEN(x)                  (((uint32_t)(((uint32_t)(x)) << RNG_INTEN_DONE_INTEN_SHIFT)) & RNG_INTEN_DONE_INTEN_MASK)


/*!
 * @}
 */ /* end of group RNG_Register_Masks */


/* RNG - Peripheral instance base addresses */
/** Peripheral RNG base address */
#define RNG_BASE                                 (0x40007C00u)
/** Peripheral RNG base pointer */
#define RNG                                      ((RNG_Type *)RNG_BASE)
/** Array initializer of RNG peripheral base addresses */
#define RNG_BASE_ADDRS                           { RNG_BASE }
/** Array initializer of RNG peripheral base pointers */
#define RNG_BASE_PTRS                            { RNG }
/** Interrupt vectors for the RNG peripheral type */
#define RNG_IRQS                                 { RNG_IRQn }

/*!
 * @}
 */ /* end of group RNG_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RTC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Peripheral_Access_Layer RTC Peripheral Access Layer
 * @{
 */

/** RTC - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL;                              /**< RTC control register, offset: 0x0 */
  __IO uint32_t STATUS;                            /**< RTC status register, offset: 0x4 */
  __IO uint32_t SEC;                               /**< RTC second register, offset: 0x8 */
       uint8_t RESERVED_0[4];
  __IO uint32_t CAL;                               /**< RTC calibration register, offset: 0x10 */
  __I  uint32_t CNT_VAL;                           /**< RTC count value register, offset: 0x14 */
       uint8_t RESERVED_1[8];
  __IO uint32_t CNT2_CTRL;                         /**< Free running control register, offset: 0x20 */
  __IO uint32_t THR_INT;                           /**< interrupt threshold of free running counter register, offset: 0x24 */
  __IO uint32_t THR_RST;                           /**< reset threshold of free running counter register, offset: 0x28 */
  __I  uint32_t CNT2;                              /**< free running count value, offset: 0x2C */
} RTC_Type;

/* ----------------------------------------------------------------------------
   -- RTC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_Register_Masks RTC Register Masks
 * @{
 */

/*! @name CTRL - RTC control register */
#define RTC_CTRL_SEC_INT_EN_MASK                 (0x1U)
#define RTC_CTRL_SEC_INT_EN_SHIFT                (0U)
#define RTC_CTRL_SEC_INT_EN(x)                   (((uint32_t)(((uint32_t)(x)) << RTC_CTRL_SEC_INT_EN_SHIFT)) & RTC_CTRL_SEC_INT_EN_MASK)
#define RTC_CTRL_CFG_MASK                        (0x4U)
#define RTC_CTRL_CFG_SHIFT                       (2U)
#define RTC_CTRL_CFG(x)                          (((uint32_t)(((uint32_t)(x)) << RTC_CTRL_CFG_SHIFT)) & RTC_CTRL_CFG_MASK)
#define RTC_CTRL_CAL_EN_MASK                     (0x100U)
#define RTC_CTRL_CAL_EN_SHIFT                    (8U)
#define RTC_CTRL_CAL_EN(x)                       (((uint32_t)(((uint32_t)(x)) << RTC_CTRL_CAL_EN_SHIFT)) & RTC_CTRL_CAL_EN_MASK)

/*! @name STATUS - RTC status register */
#define RTC_STATUS_SEC_INT_MASK                  (0x1U)
#define RTC_STATUS_SEC_INT_SHIFT                 (0U)
#define RTC_STATUS_SEC_INT(x)                    (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_SEC_INT_SHIFT)) & RTC_STATUS_SEC_INT_MASK)
#define RTC_STATUS_CTRL_SYNC_MASK                (0x100U)
#define RTC_STATUS_CTRL_SYNC_SHIFT               (8U)
#define RTC_STATUS_CTRL_SYNC(x)                  (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_CTRL_SYNC_SHIFT)) & RTC_STATUS_CTRL_SYNC_MASK)
#define RTC_STATUS_STATUS_SYNC_MASK              (0x200U)
#define RTC_STATUS_STATUS_SYNC_SHIFT             (9U)
#define RTC_STATUS_STATUS_SYNC(x)                (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_STATUS_SYNC_SHIFT)) & RTC_STATUS_STATUS_SYNC_MASK)
#define RTC_STATUS_SEC_SYNC_MASK                 (0x400U)
#define RTC_STATUS_SEC_SYNC_SHIFT                (10U)
#define RTC_STATUS_SEC_SYNC(x)                   (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_SEC_SYNC_SHIFT)) & RTC_STATUS_SEC_SYNC_MASK)
#define RTC_STATUS_CALIB_SYNC_MASK               (0x1000U)
#define RTC_STATUS_CALIB_SYNC_SHIFT              (12U)
#define RTC_STATUS_CALIB_SYNC(x)                 (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_CALIB_SYNC_SHIFT)) & RTC_STATUS_CALIB_SYNC_MASK)
#define RTC_STATUS_FREE_SYNC_MASK                (0x10000U)
#define RTC_STATUS_FREE_SYNC_SHIFT               (16U)
#define RTC_STATUS_FREE_SYNC(x)                  (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_FREE_SYNC_SHIFT)) & RTC_STATUS_FREE_SYNC_MASK)
#define RTC_STATUS_THR_INT_SYNC_MASK             (0x20000U)
#define RTC_STATUS_THR_INT_SYNC_SHIFT            (17U)
#define RTC_STATUS_THR_INT_SYNC(x)               (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_THR_INT_SYNC_SHIFT)) & RTC_STATUS_THR_INT_SYNC_MASK)
#define RTC_STATUS_THR_RST_SYNC_MASK             (0x40000U)
#define RTC_STATUS_THR_RST_SYNC_SHIFT            (18U)
#define RTC_STATUS_THR_RST_SYNC(x)               (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_THR_RST_SYNC_SHIFT)) & RTC_STATUS_THR_RST_SYNC_MASK)
#define RTC_STATUS_FREE_RUNNING_INT_MASK         (0x80000000U)
#define RTC_STATUS_FREE_RUNNING_INT_SHIFT        (31U)
#define RTC_STATUS_FREE_RUNNING_INT(x)           (((uint32_t)(((uint32_t)(x)) << RTC_STATUS_FREE_RUNNING_INT_SHIFT)) & RTC_STATUS_FREE_RUNNING_INT_MASK)

/*! @name SEC - RTC second register */
#define RTC_SEC_SEC_MASK                         (0xFFFFFFFFU)
#define RTC_SEC_SEC_SHIFT                        (0U)
#define RTC_SEC_SEC(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_SEC_SEC_SHIFT)) & RTC_SEC_SEC_MASK)

/*! @name CAL - RTC calibration register */
#define RTC_CAL_PPM_MASK                         (0xFFFFU)
#define RTC_CAL_PPM_SHIFT                        (0U)
#define RTC_CAL_PPM(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_CAL_PPM_SHIFT)) & RTC_CAL_PPM_MASK)
#define RTC_CAL_DIR_MASK                         (0x10000U)
#define RTC_CAL_DIR_SHIFT                        (16U)
#define RTC_CAL_DIR(x)                           (((uint32_t)(((uint32_t)(x)) << RTC_CAL_DIR_SHIFT)) & RTC_CAL_DIR_MASK)

/*! @name CNT_VAL - RTC count value register */
#define RTC_CNT_VAL_CNT_MASK                     (0x7FFFU)
#define RTC_CNT_VAL_CNT_SHIFT                    (0U)
#define RTC_CNT_VAL_CNT(x)                       (((uint32_t)(((uint32_t)(x)) << RTC_CNT_VAL_CNT_SHIFT)) & RTC_CNT_VAL_CNT_MASK)

/*! @name CNT2_CTRL - Free running control register */
#define RTC_CNT2_CTRL_CNT2_EN_MASK               (0x1U)
#define RTC_CNT2_CTRL_CNT2_EN_SHIFT              (0U)
#define RTC_CNT2_CTRL_CNT2_EN(x)                 (((uint32_t)(((uint32_t)(x)) << RTC_CNT2_CTRL_CNT2_EN_SHIFT)) & RTC_CNT2_CTRL_CNT2_EN_MASK)
#define RTC_CNT2_CTRL_CNT2_INT_EN_MASK           (0x2U)
#define RTC_CNT2_CTRL_CNT2_INT_EN_SHIFT          (1U)
#define RTC_CNT2_CTRL_CNT2_INT_EN(x)             (((uint32_t)(((uint32_t)(x)) << RTC_CNT2_CTRL_CNT2_INT_EN_SHIFT)) & RTC_CNT2_CTRL_CNT2_INT_EN_MASK)
#define RTC_CNT2_CTRL_CNT2_WAKEUP_MASK           (0x4U)
#define RTC_CNT2_CTRL_CNT2_WAKEUP_SHIFT          (2U)
#define RTC_CNT2_CTRL_CNT2_WAKEUP(x)             (((uint32_t)(((uint32_t)(x)) << RTC_CNT2_CTRL_CNT2_WAKEUP_SHIFT)) & RTC_CNT2_CTRL_CNT2_WAKEUP_MASK)
#define RTC_CNT2_CTRL_CNT2_RST_MASK              (0x8U)
#define RTC_CNT2_CTRL_CNT2_RST_SHIFT             (3U)
#define RTC_CNT2_CTRL_CNT2_RST(x)                (((uint32_t)(((uint32_t)(x)) << RTC_CNT2_CTRL_CNT2_RST_SHIFT)) & RTC_CNT2_CTRL_CNT2_RST_MASK)

/*! @name THR_INT - interrupt threshold of free running counter register */
#define RTC_THR_INT_THR_INT_MASK                 (0xFFFFFFFFU)
#define RTC_THR_INT_THR_INT_SHIFT                (0U)
#define RTC_THR_INT_THR_INT(x)                   (((uint32_t)(((uint32_t)(x)) << RTC_THR_INT_THR_INT_SHIFT)) & RTC_THR_INT_THR_INT_MASK)

/*! @name THR_RST - reset threshold of free running counter register */
#define RTC_THR_RST_THR_RST_MASK                 (0xFFFFFFFFU)
#define RTC_THR_RST_THR_RST_SHIFT                (0U)
#define RTC_THR_RST_THR_RST(x)                   (((uint32_t)(((uint32_t)(x)) << RTC_THR_RST_THR_RST_SHIFT)) & RTC_THR_RST_THR_RST_MASK)

/*! @name CNT2 - free running count value */
#define RTC_CNT2_CNT2_MASK                       (0xFFFFFFFFU)
#define RTC_CNT2_CNT2_SHIFT                      (0U)
#define RTC_CNT2_CNT2(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_CNT2_CNT2_SHIFT)) & RTC_CNT2_CNT2_MASK)


/*!
 * @}
 */ /* end of group RTC_Register_Masks */


/* RTC - Peripheral instance base addresses */
/** Peripheral RTC base address */
#define RTC_BASE                                 (0x4000B000u)
/** Peripheral RTC base pointer */
#define RTC                                      ((RTC_Type *)RTC_BASE)
/** Array initializer of RTC peripheral base addresses */
#define RTC_BASE_ADDRS                           { RTC_BASE }
/** Array initializer of RTC peripheral base pointers */
#define RTC_BASE_PTRS                            { RTC }
/** Interrupt vectors for the RTC peripheral type */
#define RTC_IRQS                                 { { RTC_SEC_IRQn, RTC_FR_IRQn } }

/*!
 * @}
 */ /* end of group RTC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SCT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SCT_Peripheral_Access_Layer SCT Peripheral Access Layer
 * @{
 */

/** SCT - Register Layout Typedef */
typedef struct {
  __IO uint32_t CONFIG;                            /**< SCT configuration register, offset: 0x0 */
  __IO uint32_t CTRL;                              /**< SCT control register, offset: 0x4 */
  __IO uint32_t LIMIT;                             /**< SCT limit event select register, offset: 0x8 */
  __IO uint32_t HALT;                              /**< SCT halt event select register, offset: 0xC */
  __IO uint32_t STOP;                              /**< SCT stop event select register, offset: 0x10 */
  __IO uint32_t START;                             /**< SCT start event select register, offset: 0x14 */
       uint8_t RESERVED_0[40];
  __IO uint32_t COUNT;                             /**< SCT counter register, offset: 0x40 */
  __IO uint32_t STATE;                             /**< SCT state register, offset: 0x44 */
  __I  uint32_t INPUT;                             /**< SCT input register, offset: 0x48 */
  __IO uint32_t REGMODE;                           /**< SCT match/capture mode register, offset: 0x4C */
  __IO uint32_t OUTPUT;                            /**< SCT output register, offset: 0x50 */
  __IO uint32_t OUTPUTDIRCTRL;                     /**< SCT output counter direction control register, offset: 0x54 */
  __IO uint32_t RES;                               /**< SCT conflict resolution register, offset: 0x58 */
  __IO uint32_t DMA0REQUEST;                       /**< SCT DMA request 0 register, offset: 0x5C */
  __IO uint32_t DMA1REQUEST;                       /**< SCT DMA request 1 register, offset: 0x60 */
       uint8_t RESERVED_1[140];
  __IO uint32_t EVEN;                              /**< SCT event interrupt enable register, offset: 0xF0 */
  __IO uint32_t EVFLAG;                            /**< SCT event flag register, offset: 0xF4 */
  __IO uint32_t CONEN;                             /**< SCT conflict interrupt enable register, offset: 0xF8 */
  __IO uint32_t CONFLAG;                           /**< SCT conflict flag register, offset: 0xFC */
  union {                                          /* offset: 0x100 */
    __IO uint32_t SCTCAP[10];                        /**< SCT capture register of capture channel, array offset: 0x100, array step: 0x4 */
    __IO uint32_t SCTMATCH[10];                      /**< SCT match value register of match channels, array offset: 0x100, array step: 0x4 */
  };
       uint8_t RESERVED_2[216];
  union {                                          /* offset: 0x200 */
    __IO uint32_t SCTCAPCTRL[10];                    /**< SCT capture control register, array offset: 0x200, array step: 0x4 */
    __IO uint32_t SCTMATCHREL[10];                   /**< SCT match reload value register, array offset: 0x200, array step: 0x4 */
  };
       uint8_t RESERVED_3[216];
  struct {                                         /* offset: 0x300, array step: 0x8 */
    __IO uint32_t STATE;                             /**< SCT event state register 0, array offset: 0x300, array step: 0x8 */
    __IO uint32_t CTRL;                              /**< SCT event control register 0, array offset: 0x304, array step: 0x8 */
  } EVENT[10];
       uint8_t RESERVED_4[432];
  struct {                                         /* offset: 0x500, array step: 0x8 */
    __IO uint32_t SET;                               /**< SCT output 0 set register, array offset: 0x500, array step: 0x8 */
    __IO uint32_t CLR;                               /**< SCT output 0 clear register, array offset: 0x504, array step: 0x8 */
  } OUT[8];
       uint8_t RESERVED_5[700];
  __IO uint32_t MODULECONTENT;                     /**< Reserved, offset: 0x7FC */
} SCT_Type;

/* ----------------------------------------------------------------------------
   -- SCT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SCT_Register_Masks SCT Register Masks
 * @{
 */

/*! @name CONFIG - SCT configuration register */
#define SCT_CONFIG_UNIFY_MASK                    (0x1U)
#define SCT_CONFIG_UNIFY_SHIFT                   (0U)
#define SCT_CONFIG_UNIFY(x)                      (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_UNIFY_SHIFT)) & SCT_CONFIG_UNIFY_MASK)
#define SCT_CONFIG_CLKMODE_MASK                  (0x6U)
#define SCT_CONFIG_CLKMODE_SHIFT                 (1U)
#define SCT_CONFIG_CLKMODE(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_CLKMODE_SHIFT)) & SCT_CONFIG_CLKMODE_MASK)
#define SCT_CONFIG_CKSEL_MASK                    (0x78U)
#define SCT_CONFIG_CKSEL_SHIFT                   (3U)
#define SCT_CONFIG_CKSEL(x)                      (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_CKSEL_SHIFT)) & SCT_CONFIG_CKSEL_MASK)
#define SCT_CONFIG_NORELAOD_L_MASK               (0x80U)
#define SCT_CONFIG_NORELAOD_L_SHIFT              (7U)
#define SCT_CONFIG_NORELAOD_L(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_NORELAOD_L_SHIFT)) & SCT_CONFIG_NORELAOD_L_MASK)
#define SCT_CONFIG_NORELOAD_H_MASK               (0x100U)
#define SCT_CONFIG_NORELOAD_H_SHIFT              (8U)
#define SCT_CONFIG_NORELOAD_H(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_NORELOAD_H_SHIFT)) & SCT_CONFIG_NORELOAD_H_MASK)
#define SCT_CONFIG_INSYNC_MASK                   (0x1E00U)
#define SCT_CONFIG_INSYNC_SHIFT                  (9U)
#define SCT_CONFIG_INSYNC(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_INSYNC_SHIFT)) & SCT_CONFIG_INSYNC_MASK)
#define SCT_CONFIG_AUTOLIMIT_L_MASK              (0x20000U)
#define SCT_CONFIG_AUTOLIMIT_L_SHIFT             (17U)
#define SCT_CONFIG_AUTOLIMIT_L(x)                (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_AUTOLIMIT_L_SHIFT)) & SCT_CONFIG_AUTOLIMIT_L_MASK)
#define SCT_CONFIG_AUTOLIMIT_H_MASK              (0x40000U)
#define SCT_CONFIG_AUTOLIMIT_H_SHIFT             (18U)
#define SCT_CONFIG_AUTOLIMIT_H(x)                (((uint32_t)(((uint32_t)(x)) << SCT_CONFIG_AUTOLIMIT_H_SHIFT)) & SCT_CONFIG_AUTOLIMIT_H_MASK)

/*! @name CTRL - SCT control register */
#define SCT_CTRL_DOWN_L_MASK                     (0x1U)
#define SCT_CTRL_DOWN_L_SHIFT                    (0U)
#define SCT_CTRL_DOWN_L(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_DOWN_L_SHIFT)) & SCT_CTRL_DOWN_L_MASK)
#define SCT_CTRL_STOP_L_MASK                     (0x2U)
#define SCT_CTRL_STOP_L_SHIFT                    (1U)
#define SCT_CTRL_STOP_L(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_STOP_L_SHIFT)) & SCT_CTRL_STOP_L_MASK)
#define SCT_CTRL_HALT_L_MASK                     (0x4U)
#define SCT_CTRL_HALT_L_SHIFT                    (2U)
#define SCT_CTRL_HALT_L(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_HALT_L_SHIFT)) & SCT_CTRL_HALT_L_MASK)
#define SCT_CTRL_CLRCTR_L_MASK                   (0x8U)
#define SCT_CTRL_CLRCTR_L_SHIFT                  (3U)
#define SCT_CTRL_CLRCTR_L(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_CLRCTR_L_SHIFT)) & SCT_CTRL_CLRCTR_L_MASK)
#define SCT_CTRL_BIDIR_L_MASK                    (0x10U)
#define SCT_CTRL_BIDIR_L_SHIFT                   (4U)
#define SCT_CTRL_BIDIR_L(x)                      (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_BIDIR_L_SHIFT)) & SCT_CTRL_BIDIR_L_MASK)
#define SCT_CTRL_PRE_L_MASK                      (0x1FE0U)
#define SCT_CTRL_PRE_L_SHIFT                     (5U)
#define SCT_CTRL_PRE_L(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_PRE_L_SHIFT)) & SCT_CTRL_PRE_L_MASK)
#define SCT_CTRL_DOWN_H_MASK                     (0x10000U)
#define SCT_CTRL_DOWN_H_SHIFT                    (16U)
#define SCT_CTRL_DOWN_H(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_DOWN_H_SHIFT)) & SCT_CTRL_DOWN_H_MASK)
#define SCT_CTRL_STOP_H_MASK                     (0x20000U)
#define SCT_CTRL_STOP_H_SHIFT                    (17U)
#define SCT_CTRL_STOP_H(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_STOP_H_SHIFT)) & SCT_CTRL_STOP_H_MASK)
#define SCT_CTRL_HALT_H_MASK                     (0x40000U)
#define SCT_CTRL_HALT_H_SHIFT                    (18U)
#define SCT_CTRL_HALT_H(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_HALT_H_SHIFT)) & SCT_CTRL_HALT_H_MASK)
#define SCT_CTRL_CLRCTR_H_MASK                   (0x80000U)
#define SCT_CTRL_CLRCTR_H_SHIFT                  (19U)
#define SCT_CTRL_CLRCTR_H(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_CLRCTR_H_SHIFT)) & SCT_CTRL_CLRCTR_H_MASK)
#define SCT_CTRL_BIDIR_H_MASK                    (0x100000U)
#define SCT_CTRL_BIDIR_H_SHIFT                   (20U)
#define SCT_CTRL_BIDIR_H(x)                      (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_BIDIR_H_SHIFT)) & SCT_CTRL_BIDIR_H_MASK)
#define SCT_CTRL_PRE_H_MASK                      (0x1FE00000U)
#define SCT_CTRL_PRE_H_SHIFT                     (21U)
#define SCT_CTRL_PRE_H(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_CTRL_PRE_H_SHIFT)) & SCT_CTRL_PRE_H_MASK)

/*! @name LIMIT - SCT limit event select register */
#define SCT_LIMIT_LIMMSK_L_MASK                  (0xFFFFU)
#define SCT_LIMIT_LIMMSK_L_SHIFT                 (0U)
#define SCT_LIMIT_LIMMSK_L(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_LIMIT_LIMMSK_L_SHIFT)) & SCT_LIMIT_LIMMSK_L_MASK)
#define SCT_LIMIT_LIMMSK_H_MASK                  (0xFFFF0000U)
#define SCT_LIMIT_LIMMSK_H_SHIFT                 (16U)
#define SCT_LIMIT_LIMMSK_H(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_LIMIT_LIMMSK_H_SHIFT)) & SCT_LIMIT_LIMMSK_H_MASK)

/*! @name HALT - SCT halt event select register */
#define SCT_HALT_HALTMSK_L_MASK                  (0xFFFFU)
#define SCT_HALT_HALTMSK_L_SHIFT                 (0U)
#define SCT_HALT_HALTMSK_L(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_HALT_HALTMSK_L_SHIFT)) & SCT_HALT_HALTMSK_L_MASK)
#define SCT_HALT_HALTMSK_H_MASK                  (0xFFFF0000U)
#define SCT_HALT_HALTMSK_H_SHIFT                 (16U)
#define SCT_HALT_HALTMSK_H(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_HALT_HALTMSK_H_SHIFT)) & SCT_HALT_HALTMSK_H_MASK)

/*! @name STOP - SCT stop event select register */
#define SCT_STOP_STOPMSK_L_MASK                  (0xFFFFU)
#define SCT_STOP_STOPMSK_L_SHIFT                 (0U)
#define SCT_STOP_STOPMSK_L(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_STOP_STOPMSK_L_SHIFT)) & SCT_STOP_STOPMSK_L_MASK)
#define SCT_STOP_STOPMSK_H_MASK                  (0xFFFF0000U)
#define SCT_STOP_STOPMSK_H_SHIFT                 (16U)
#define SCT_STOP_STOPMSK_H(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_STOP_STOPMSK_H_SHIFT)) & SCT_STOP_STOPMSK_H_MASK)

/*! @name START - SCT start event select register */
#define SCT_START_STARTMSK_L_MASK                (0xFFFFU)
#define SCT_START_STARTMSK_L_SHIFT               (0U)
#define SCT_START_STARTMSK_L(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_START_STARTMSK_L_SHIFT)) & SCT_START_STARTMSK_L_MASK)
#define SCT_START_STARTMSK_H_MASK                (0xFFFF0000U)
#define SCT_START_STARTMSK_H_SHIFT               (16U)
#define SCT_START_STARTMSK_H(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_START_STARTMSK_H_SHIFT)) & SCT_START_STARTMSK_H_MASK)

/*! @name COUNT - SCT counter register */
#define SCT_COUNT_CTR_L_MASK                     (0xFFFFU)
#define SCT_COUNT_CTR_L_SHIFT                    (0U)
#define SCT_COUNT_CTR_L(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_COUNT_CTR_L_SHIFT)) & SCT_COUNT_CTR_L_MASK)
#define SCT_COUNT_CTR_H_MASK                     (0xFFFF0000U)
#define SCT_COUNT_CTR_H_SHIFT                    (16U)
#define SCT_COUNT_CTR_H(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_COUNT_CTR_H_SHIFT)) & SCT_COUNT_CTR_H_MASK)

/*! @name STATE - SCT state register */
#define SCT_STATE_STATE_L_MASK                   (0x1FU)
#define SCT_STATE_STATE_L_SHIFT                  (0U)
#define SCT_STATE_STATE_L(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_STATE_STATE_L_SHIFT)) & SCT_STATE_STATE_L_MASK)
#define SCT_STATE_STATE_H_MASK                   (0x1F0000U)
#define SCT_STATE_STATE_H_SHIFT                  (16U)
#define SCT_STATE_STATE_H(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_STATE_STATE_H_SHIFT)) & SCT_STATE_STATE_H_MASK)

/*! @name INPUT - SCT input register */
#define SCT_INPUT_AIN0_MASK                      (0x1U)
#define SCT_INPUT_AIN0_SHIFT                     (0U)
#define SCT_INPUT_AIN0(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN0_SHIFT)) & SCT_INPUT_AIN0_MASK)
#define SCT_INPUT_AIN1_MASK                      (0x2U)
#define SCT_INPUT_AIN1_SHIFT                     (1U)
#define SCT_INPUT_AIN1(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN1_SHIFT)) & SCT_INPUT_AIN1_MASK)
#define SCT_INPUT_AIN2_MASK                      (0x4U)
#define SCT_INPUT_AIN2_SHIFT                     (2U)
#define SCT_INPUT_AIN2(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN2_SHIFT)) & SCT_INPUT_AIN2_MASK)
#define SCT_INPUT_AIN3_MASK                      (0x8U)
#define SCT_INPUT_AIN3_SHIFT                     (3U)
#define SCT_INPUT_AIN3(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN3_SHIFT)) & SCT_INPUT_AIN3_MASK)
#define SCT_INPUT_AIN4_MASK                      (0x10U)
#define SCT_INPUT_AIN4_SHIFT                     (4U)
#define SCT_INPUT_AIN4(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN4_SHIFT)) & SCT_INPUT_AIN4_MASK)
#define SCT_INPUT_AIN5_MASK                      (0x20U)
#define SCT_INPUT_AIN5_SHIFT                     (5U)
#define SCT_INPUT_AIN5(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN5_SHIFT)) & SCT_INPUT_AIN5_MASK)
#define SCT_INPUT_AIN6_MASK                      (0x40U)
#define SCT_INPUT_AIN6_SHIFT                     (6U)
#define SCT_INPUT_AIN6(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN6_SHIFT)) & SCT_INPUT_AIN6_MASK)
#define SCT_INPUT_AIN7_MASK                      (0x80U)
#define SCT_INPUT_AIN7_SHIFT                     (7U)
#define SCT_INPUT_AIN7(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN7_SHIFT)) & SCT_INPUT_AIN7_MASK)
#define SCT_INPUT_AIN8_MASK                      (0x100U)
#define SCT_INPUT_AIN8_SHIFT                     (8U)
#define SCT_INPUT_AIN8(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN8_SHIFT)) & SCT_INPUT_AIN8_MASK)
#define SCT_INPUT_AIN9_MASK                      (0x200U)
#define SCT_INPUT_AIN9_SHIFT                     (9U)
#define SCT_INPUT_AIN9(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN9_SHIFT)) & SCT_INPUT_AIN9_MASK)
#define SCT_INPUT_AIN10_MASK                     (0x400U)
#define SCT_INPUT_AIN10_SHIFT                    (10U)
#define SCT_INPUT_AIN10(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN10_SHIFT)) & SCT_INPUT_AIN10_MASK)
#define SCT_INPUT_AIN11_MASK                     (0x800U)
#define SCT_INPUT_AIN11_SHIFT                    (11U)
#define SCT_INPUT_AIN11(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN11_SHIFT)) & SCT_INPUT_AIN11_MASK)
#define SCT_INPUT_AIN12_MASK                     (0x1000U)
#define SCT_INPUT_AIN12_SHIFT                    (12U)
#define SCT_INPUT_AIN12(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN12_SHIFT)) & SCT_INPUT_AIN12_MASK)
#define SCT_INPUT_AIN13_MASK                     (0x2000U)
#define SCT_INPUT_AIN13_SHIFT                    (13U)
#define SCT_INPUT_AIN13(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN13_SHIFT)) & SCT_INPUT_AIN13_MASK)
#define SCT_INPUT_AIN14_MASK                     (0x4000U)
#define SCT_INPUT_AIN14_SHIFT                    (14U)
#define SCT_INPUT_AIN14(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN14_SHIFT)) & SCT_INPUT_AIN14_MASK)
#define SCT_INPUT_AIN15_MASK                     (0x8000U)
#define SCT_INPUT_AIN15_SHIFT                    (15U)
#define SCT_INPUT_AIN15(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_AIN15_SHIFT)) & SCT_INPUT_AIN15_MASK)
#define SCT_INPUT_SIN0_MASK                      (0x10000U)
#define SCT_INPUT_SIN0_SHIFT                     (16U)
#define SCT_INPUT_SIN0(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN0_SHIFT)) & SCT_INPUT_SIN0_MASK)
#define SCT_INPUT_SIN1_MASK                      (0x20000U)
#define SCT_INPUT_SIN1_SHIFT                     (17U)
#define SCT_INPUT_SIN1(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN1_SHIFT)) & SCT_INPUT_SIN1_MASK)
#define SCT_INPUT_SIN2_MASK                      (0x40000U)
#define SCT_INPUT_SIN2_SHIFT                     (18U)
#define SCT_INPUT_SIN2(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN2_SHIFT)) & SCT_INPUT_SIN2_MASK)
#define SCT_INPUT_SIN3_MASK                      (0x80000U)
#define SCT_INPUT_SIN3_SHIFT                     (19U)
#define SCT_INPUT_SIN3(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN3_SHIFT)) & SCT_INPUT_SIN3_MASK)
#define SCT_INPUT_SIN4_MASK                      (0x100000U)
#define SCT_INPUT_SIN4_SHIFT                     (20U)
#define SCT_INPUT_SIN4(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN4_SHIFT)) & SCT_INPUT_SIN4_MASK)
#define SCT_INPUT_SIN5_MASK                      (0x200000U)
#define SCT_INPUT_SIN5_SHIFT                     (21U)
#define SCT_INPUT_SIN5(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN5_SHIFT)) & SCT_INPUT_SIN5_MASK)
#define SCT_INPUT_SIN6_MASK                      (0x400000U)
#define SCT_INPUT_SIN6_SHIFT                     (22U)
#define SCT_INPUT_SIN6(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN6_SHIFT)) & SCT_INPUT_SIN6_MASK)
#define SCT_INPUT_SIN7_MASK                      (0x800000U)
#define SCT_INPUT_SIN7_SHIFT                     (23U)
#define SCT_INPUT_SIN7(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN7_SHIFT)) & SCT_INPUT_SIN7_MASK)
#define SCT_INPUT_SIN8_MASK                      (0x1000000U)
#define SCT_INPUT_SIN8_SHIFT                     (24U)
#define SCT_INPUT_SIN8(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN8_SHIFT)) & SCT_INPUT_SIN8_MASK)
#define SCT_INPUT_SIN9_MASK                      (0x2000000U)
#define SCT_INPUT_SIN9_SHIFT                     (25U)
#define SCT_INPUT_SIN9(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN9_SHIFT)) & SCT_INPUT_SIN9_MASK)
#define SCT_INPUT_SIN10_MASK                     (0x4000000U)
#define SCT_INPUT_SIN10_SHIFT                    (26U)
#define SCT_INPUT_SIN10(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN10_SHIFT)) & SCT_INPUT_SIN10_MASK)
#define SCT_INPUT_SIN11_MASK                     (0x8000000U)
#define SCT_INPUT_SIN11_SHIFT                    (27U)
#define SCT_INPUT_SIN11(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN11_SHIFT)) & SCT_INPUT_SIN11_MASK)
#define SCT_INPUT_SIN12_MASK                     (0x10000000U)
#define SCT_INPUT_SIN12_SHIFT                    (28U)
#define SCT_INPUT_SIN12(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN12_SHIFT)) & SCT_INPUT_SIN12_MASK)
#define SCT_INPUT_SIN13_MASK                     (0x20000000U)
#define SCT_INPUT_SIN13_SHIFT                    (29U)
#define SCT_INPUT_SIN13(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN13_SHIFT)) & SCT_INPUT_SIN13_MASK)
#define SCT_INPUT_SIN14_MASK                     (0x40000000U)
#define SCT_INPUT_SIN14_SHIFT                    (30U)
#define SCT_INPUT_SIN14(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN14_SHIFT)) & SCT_INPUT_SIN14_MASK)
#define SCT_INPUT_SIN15_MASK                     (0x80000000U)
#define SCT_INPUT_SIN15_SHIFT                    (31U)
#define SCT_INPUT_SIN15(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_INPUT_SIN15_SHIFT)) & SCT_INPUT_SIN15_MASK)

/*! @name REGMODE - SCT match/capture mode register */
#define SCT_REGMODE_REGMOD_L_MASK                (0xFFFFU)
#define SCT_REGMODE_REGMOD_L_SHIFT               (0U)
#define SCT_REGMODE_REGMOD_L(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_REGMODE_REGMOD_L_SHIFT)) & SCT_REGMODE_REGMOD_L_MASK)
#define SCT_REGMODE_REGMOD_H_MASK                (0xFFFF0000U)
#define SCT_REGMODE_REGMOD_H_SHIFT               (16U)
#define SCT_REGMODE_REGMOD_H(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_REGMODE_REGMOD_H_SHIFT)) & SCT_REGMODE_REGMOD_H_MASK)

/*! @name OUTPUT - SCT output register */
#define SCT_OUTPUT_OUT_MASK                      (0xFFFFU)
#define SCT_OUTPUT_OUT_SHIFT                     (0U)
#define SCT_OUTPUT_OUT(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUT_OUT_SHIFT)) & SCT_OUTPUT_OUT_MASK)

/*! @name OUTPUTDIRCTRL - SCT output counter direction control register */
#define SCT_OUTPUTDIRCTRL_SETCLR0_MASK           (0x3U)
#define SCT_OUTPUTDIRCTRL_SETCLR0_SHIFT          (0U)
#define SCT_OUTPUTDIRCTRL_SETCLR0(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR0_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR0_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR1_MASK           (0xCU)
#define SCT_OUTPUTDIRCTRL_SETCLR1_SHIFT          (2U)
#define SCT_OUTPUTDIRCTRL_SETCLR1(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR1_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR1_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR2_MASK           (0x30U)
#define SCT_OUTPUTDIRCTRL_SETCLR2_SHIFT          (4U)
#define SCT_OUTPUTDIRCTRL_SETCLR2(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR2_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR2_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR3_MASK           (0xC0U)
#define SCT_OUTPUTDIRCTRL_SETCLR3_SHIFT          (6U)
#define SCT_OUTPUTDIRCTRL_SETCLR3(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR3_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR3_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR4_MASK           (0x300U)
#define SCT_OUTPUTDIRCTRL_SETCLR4_SHIFT          (8U)
#define SCT_OUTPUTDIRCTRL_SETCLR4(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR4_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR4_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR5_MASK           (0xC00U)
#define SCT_OUTPUTDIRCTRL_SETCLR5_SHIFT          (10U)
#define SCT_OUTPUTDIRCTRL_SETCLR5(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR5_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR5_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR6_MASK           (0x3000U)
#define SCT_OUTPUTDIRCTRL_SETCLR6_SHIFT          (12U)
#define SCT_OUTPUTDIRCTRL_SETCLR6(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR6_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR6_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR7_MASK           (0xC000U)
#define SCT_OUTPUTDIRCTRL_SETCLR7_SHIFT          (14U)
#define SCT_OUTPUTDIRCTRL_SETCLR7(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR7_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR7_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR8_MASK           (0x30000U)
#define SCT_OUTPUTDIRCTRL_SETCLR8_SHIFT          (16U)
#define SCT_OUTPUTDIRCTRL_SETCLR8(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR8_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR8_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR9_MASK           (0xC0000U)
#define SCT_OUTPUTDIRCTRL_SETCLR9_SHIFT          (18U)
#define SCT_OUTPUTDIRCTRL_SETCLR9(x)             (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR9_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR9_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR10_MASK          (0x300000U)
#define SCT_OUTPUTDIRCTRL_SETCLR10_SHIFT         (20U)
#define SCT_OUTPUTDIRCTRL_SETCLR10(x)            (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR10_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR10_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR11_MASK          (0xC00000U)
#define SCT_OUTPUTDIRCTRL_SETCLR11_SHIFT         (22U)
#define SCT_OUTPUTDIRCTRL_SETCLR11(x)            (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR11_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR11_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR12_MASK          (0x3000000U)
#define SCT_OUTPUTDIRCTRL_SETCLR12_SHIFT         (24U)
#define SCT_OUTPUTDIRCTRL_SETCLR12(x)            (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR12_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR12_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR13_MASK          (0xC000000U)
#define SCT_OUTPUTDIRCTRL_SETCLR13_SHIFT         (26U)
#define SCT_OUTPUTDIRCTRL_SETCLR13(x)            (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR13_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR13_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR14_MASK          (0x30000000U)
#define SCT_OUTPUTDIRCTRL_SETCLR14_SHIFT         (28U)
#define SCT_OUTPUTDIRCTRL_SETCLR14(x)            (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR14_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR14_MASK)
#define SCT_OUTPUTDIRCTRL_SETCLR15_MASK          (0xC0000000U)
#define SCT_OUTPUTDIRCTRL_SETCLR15_SHIFT         (30U)
#define SCT_OUTPUTDIRCTRL_SETCLR15(x)            (((uint32_t)(((uint32_t)(x)) << SCT_OUTPUTDIRCTRL_SETCLR15_SHIFT)) & SCT_OUTPUTDIRCTRL_SETCLR15_MASK)

/*! @name RES - SCT conflict resolution register */
#define SCT_RES_O0RES_MASK                       (0x3U)
#define SCT_RES_O0RES_SHIFT                      (0U)
#define SCT_RES_O0RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O0RES_SHIFT)) & SCT_RES_O0RES_MASK)
#define SCT_RES_O1RES_MASK                       (0xCU)
#define SCT_RES_O1RES_SHIFT                      (2U)
#define SCT_RES_O1RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O1RES_SHIFT)) & SCT_RES_O1RES_MASK)
#define SCT_RES_O2RES_MASK                       (0x30U)
#define SCT_RES_O2RES_SHIFT                      (4U)
#define SCT_RES_O2RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O2RES_SHIFT)) & SCT_RES_O2RES_MASK)
#define SCT_RES_O3RES_MASK                       (0xC0U)
#define SCT_RES_O3RES_SHIFT                      (6U)
#define SCT_RES_O3RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O3RES_SHIFT)) & SCT_RES_O3RES_MASK)
#define SCT_RES_O4RES_MASK                       (0x300U)
#define SCT_RES_O4RES_SHIFT                      (8U)
#define SCT_RES_O4RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O4RES_SHIFT)) & SCT_RES_O4RES_MASK)
#define SCT_RES_O5RES_MASK                       (0xC00U)
#define SCT_RES_O5RES_SHIFT                      (10U)
#define SCT_RES_O5RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O5RES_SHIFT)) & SCT_RES_O5RES_MASK)
#define SCT_RES_O6RES_MASK                       (0x3000U)
#define SCT_RES_O6RES_SHIFT                      (12U)
#define SCT_RES_O6RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O6RES_SHIFT)) & SCT_RES_O6RES_MASK)
#define SCT_RES_O7RES_MASK                       (0xC000U)
#define SCT_RES_O7RES_SHIFT                      (14U)
#define SCT_RES_O7RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O7RES_SHIFT)) & SCT_RES_O7RES_MASK)
#define SCT_RES_O8RES_MASK                       (0x30000U)
#define SCT_RES_O8RES_SHIFT                      (16U)
#define SCT_RES_O8RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O8RES_SHIFT)) & SCT_RES_O8RES_MASK)
#define SCT_RES_O9RES_MASK                       (0xC0000U)
#define SCT_RES_O9RES_SHIFT                      (18U)
#define SCT_RES_O9RES(x)                         (((uint32_t)(((uint32_t)(x)) << SCT_RES_O9RES_SHIFT)) & SCT_RES_O9RES_MASK)
#define SCT_RES_O10RES_MASK                      (0x300000U)
#define SCT_RES_O10RES_SHIFT                     (20U)
#define SCT_RES_O10RES(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_RES_O10RES_SHIFT)) & SCT_RES_O10RES_MASK)
#define SCT_RES_O11RES_MASK                      (0xC00000U)
#define SCT_RES_O11RES_SHIFT                     (22U)
#define SCT_RES_O11RES(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_RES_O11RES_SHIFT)) & SCT_RES_O11RES_MASK)
#define SCT_RES_O12RES_MASK                      (0x3000000U)
#define SCT_RES_O12RES_SHIFT                     (24U)
#define SCT_RES_O12RES(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_RES_O12RES_SHIFT)) & SCT_RES_O12RES_MASK)
#define SCT_RES_O13RES_MASK                      (0xC000000U)
#define SCT_RES_O13RES_SHIFT                     (26U)
#define SCT_RES_O13RES(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_RES_O13RES_SHIFT)) & SCT_RES_O13RES_MASK)
#define SCT_RES_O14RES_MASK                      (0x30000000U)
#define SCT_RES_O14RES_SHIFT                     (28U)
#define SCT_RES_O14RES(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_RES_O14RES_SHIFT)) & SCT_RES_O14RES_MASK)
#define SCT_RES_O15RES_MASK                      (0xC0000000U)
#define SCT_RES_O15RES_SHIFT                     (30U)
#define SCT_RES_O15RES(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_RES_O15RES_SHIFT)) & SCT_RES_O15RES_MASK)

/*! @name DMA0REQUEST - SCT DMA request 0 register */
#define SCT_DMA0REQUEST_DEV_0_MASK               (0xFFFFU)
#define SCT_DMA0REQUEST_DEV_0_SHIFT              (0U)
#define SCT_DMA0REQUEST_DEV_0(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_DMA0REQUEST_DEV_0_SHIFT)) & SCT_DMA0REQUEST_DEV_0_MASK)
#define SCT_DMA0REQUEST_DRL0_MASK                (0x40000000U)
#define SCT_DMA0REQUEST_DRL0_SHIFT               (30U)
#define SCT_DMA0REQUEST_DRL0(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_DMA0REQUEST_DRL0_SHIFT)) & SCT_DMA0REQUEST_DRL0_MASK)
#define SCT_DMA0REQUEST_DRQ0_MASK                (0x80000000U)
#define SCT_DMA0REQUEST_DRQ0_SHIFT               (31U)
#define SCT_DMA0REQUEST_DRQ0(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_DMA0REQUEST_DRQ0_SHIFT)) & SCT_DMA0REQUEST_DRQ0_MASK)

/*! @name DMA1REQUEST - SCT DMA request 1 register */
#define SCT_DMA1REQUEST_DEV_1_MASK               (0xFFFFU)
#define SCT_DMA1REQUEST_DEV_1_SHIFT              (0U)
#define SCT_DMA1REQUEST_DEV_1(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_DMA1REQUEST_DEV_1_SHIFT)) & SCT_DMA1REQUEST_DEV_1_MASK)
#define SCT_DMA1REQUEST_DRL1_MASK                (0x40000000U)
#define SCT_DMA1REQUEST_DRL1_SHIFT               (30U)
#define SCT_DMA1REQUEST_DRL1(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_DMA1REQUEST_DRL1_SHIFT)) & SCT_DMA1REQUEST_DRL1_MASK)
#define SCT_DMA1REQUEST_DRQ1_MASK                (0x80000000U)
#define SCT_DMA1REQUEST_DRQ1_SHIFT               (31U)
#define SCT_DMA1REQUEST_DRQ1(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_DMA1REQUEST_DRQ1_SHIFT)) & SCT_DMA1REQUEST_DRQ1_MASK)

/*! @name EVEN - SCT event interrupt enable register */
#define SCT_EVEN_IEN_MASK                        (0xFFFFU)
#define SCT_EVEN_IEN_SHIFT                       (0U)
#define SCT_EVEN_IEN(x)                          (((uint32_t)(((uint32_t)(x)) << SCT_EVEN_IEN_SHIFT)) & SCT_EVEN_IEN_MASK)

/*! @name EVFLAG - SCT event flag register */
#define SCT_EVFLAG_FLAG_MASK                     (0xFFFFU)
#define SCT_EVFLAG_FLAG_SHIFT                    (0U)
#define SCT_EVFLAG_FLAG(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_EVFLAG_FLAG_SHIFT)) & SCT_EVFLAG_FLAG_MASK)

/*! @name CONEN - SCT conflict interrupt enable register */
#define SCT_CONEN_NCEN_MASK                      (0xFFFFU)
#define SCT_CONEN_NCEN_SHIFT                     (0U)
#define SCT_CONEN_NCEN(x)                        (((uint32_t)(((uint32_t)(x)) << SCT_CONEN_NCEN_SHIFT)) & SCT_CONEN_NCEN_MASK)

/*! @name CONFLAG - SCT conflict flag register */
#define SCT_CONFLAG_NCFLAG_MASK                  (0xFFFFU)
#define SCT_CONFLAG_NCFLAG_SHIFT                 (0U)
#define SCT_CONFLAG_NCFLAG(x)                    (((uint32_t)(((uint32_t)(x)) << SCT_CONFLAG_NCFLAG_SHIFT)) & SCT_CONFLAG_NCFLAG_MASK)
#define SCT_CONFLAG_BUSERRL_MASK                 (0x40000000U)
#define SCT_CONFLAG_BUSERRL_SHIFT                (30U)
#define SCT_CONFLAG_BUSERRL(x)                   (((uint32_t)(((uint32_t)(x)) << SCT_CONFLAG_BUSERRL_SHIFT)) & SCT_CONFLAG_BUSERRL_MASK)
#define SCT_CONFLAG_BUSERRH_MASK                 (0x80000000U)
#define SCT_CONFLAG_BUSERRH_SHIFT                (31U)
#define SCT_CONFLAG_BUSERRH(x)                   (((uint32_t)(((uint32_t)(x)) << SCT_CONFLAG_BUSERRH_SHIFT)) & SCT_CONFLAG_BUSERRH_MASK)

/*! @name SCTCAP - SCT capture register of capture channel */
#define SCT_SCTCAP_CAPn_L_MASK                   (0xFFFFU)
#define SCT_SCTCAP_CAPn_L_SHIFT                  (0U)
#define SCT_SCTCAP_CAPn_L(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_SCTCAP_CAPn_L_SHIFT)) & SCT_SCTCAP_CAPn_L_MASK)
#define SCT_SCTCAP_CAPn_H_MASK                   (0xFFFF0000U)
#define SCT_SCTCAP_CAPn_H_SHIFT                  (16U)
#define SCT_SCTCAP_CAPn_H(x)                     (((uint32_t)(((uint32_t)(x)) << SCT_SCTCAP_CAPn_H_SHIFT)) & SCT_SCTCAP_CAPn_H_MASK)

/* The count of SCT_SCTCAP */
#define SCT_SCTCAP_COUNT                         (10U)

/*! @name SCTMATCH - SCT match value register of match channels */
#define SCT_SCTMATCH_MATCHn_L_MASK               (0xFFFFU)
#define SCT_SCTMATCH_MATCHn_L_SHIFT              (0U)
#define SCT_SCTMATCH_MATCHn_L(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_SCTMATCH_MATCHn_L_SHIFT)) & SCT_SCTMATCH_MATCHn_L_MASK)
#define SCT_SCTMATCH_MATCHn_H_MASK               (0xFFFF0000U)
#define SCT_SCTMATCH_MATCHn_H_SHIFT              (16U)
#define SCT_SCTMATCH_MATCHn_H(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_SCTMATCH_MATCHn_H_SHIFT)) & SCT_SCTMATCH_MATCHn_H_MASK)

/* The count of SCT_SCTMATCH */
#define SCT_SCTMATCH_COUNT                       (10U)

/*! @name SCTCAPCTRL - SCT capture control register */
#define SCT_SCTCAPCTRL_CAPCONn_L_MASK            (0xFFFFU)
#define SCT_SCTCAPCTRL_CAPCONn_L_SHIFT           (0U)
#define SCT_SCTCAPCTRL_CAPCONn_L(x)              (((uint32_t)(((uint32_t)(x)) << SCT_SCTCAPCTRL_CAPCONn_L_SHIFT)) & SCT_SCTCAPCTRL_CAPCONn_L_MASK)
#define SCT_SCTCAPCTRL_CAPCONn_H_MASK            (0xFFFF0000U)
#define SCT_SCTCAPCTRL_CAPCONn_H_SHIFT           (16U)
#define SCT_SCTCAPCTRL_CAPCONn_H(x)              (((uint32_t)(((uint32_t)(x)) << SCT_SCTCAPCTRL_CAPCONn_H_SHIFT)) & SCT_SCTCAPCTRL_CAPCONn_H_MASK)

/* The count of SCT_SCTCAPCTRL */
#define SCT_SCTCAPCTRL_COUNT                     (10U)

/*! @name SCTMATCHREL - SCT match reload value register */
#define SCT_SCTMATCHREL_RELOADn_L_MASK           (0xFFFFU)
#define SCT_SCTMATCHREL_RELOADn_L_SHIFT          (0U)
#define SCT_SCTMATCHREL_RELOADn_L(x)             (((uint32_t)(((uint32_t)(x)) << SCT_SCTMATCHREL_RELOADn_L_SHIFT)) & SCT_SCTMATCHREL_RELOADn_L_MASK)
#define SCT_SCTMATCHREL_RELOADn_H_MASK           (0xFFFF0000U)
#define SCT_SCTMATCHREL_RELOADn_H_SHIFT          (16U)
#define SCT_SCTMATCHREL_RELOADn_H(x)             (((uint32_t)(((uint32_t)(x)) << SCT_SCTMATCHREL_RELOADn_H_SHIFT)) & SCT_SCTMATCHREL_RELOADn_H_MASK)

/* The count of SCT_SCTMATCHREL */
#define SCT_SCTMATCHREL_COUNT                    (10U)

/*! @name EVENT_STATE - SCT event state register 0 */
#define SCT_EVENT_STATE_STATEMSKn_MASK           (0xFFFFU)
#define SCT_EVENT_STATE_STATEMSKn_SHIFT          (0U)
#define SCT_EVENT_STATE_STATEMSKn(x)             (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_STATE_STATEMSKn_SHIFT)) & SCT_EVENT_STATE_STATEMSKn_MASK)

/* The count of SCT_EVENT_STATE */
#define SCT_EVENT_STATE_COUNT                    (10U)

/*! @name EVENT_CTRL - SCT event control register 0 */
#define SCT_EVENT_CTRL_MATCHSEL_MASK             (0xFU)
#define SCT_EVENT_CTRL_MATCHSEL_SHIFT            (0U)
#define SCT_EVENT_CTRL_MATCHSEL(x)               (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_MATCHSEL_SHIFT)) & SCT_EVENT_CTRL_MATCHSEL_MASK)
#define SCT_EVENT_CTRL_HEVENT_MASK               (0x10U)
#define SCT_EVENT_CTRL_HEVENT_SHIFT              (4U)
#define SCT_EVENT_CTRL_HEVENT(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_HEVENT_SHIFT)) & SCT_EVENT_CTRL_HEVENT_MASK)
#define SCT_EVENT_CTRL_OUTSEL_MASK               (0x20U)
#define SCT_EVENT_CTRL_OUTSEL_SHIFT              (5U)
#define SCT_EVENT_CTRL_OUTSEL(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_OUTSEL_SHIFT)) & SCT_EVENT_CTRL_OUTSEL_MASK)
#define SCT_EVENT_CTRL_IOSEL_MASK                (0x3C0U)
#define SCT_EVENT_CTRL_IOSEL_SHIFT               (6U)
#define SCT_EVENT_CTRL_IOSEL(x)                  (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_IOSEL_SHIFT)) & SCT_EVENT_CTRL_IOSEL_MASK)
#define SCT_EVENT_CTRL_IOCOND_MASK               (0xC00U)
#define SCT_EVENT_CTRL_IOCOND_SHIFT              (10U)
#define SCT_EVENT_CTRL_IOCOND(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_IOCOND_SHIFT)) & SCT_EVENT_CTRL_IOCOND_MASK)
#define SCT_EVENT_CTRL_COMBMODE_MASK             (0x3000U)
#define SCT_EVENT_CTRL_COMBMODE_SHIFT            (12U)
#define SCT_EVENT_CTRL_COMBMODE(x)               (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_COMBMODE_SHIFT)) & SCT_EVENT_CTRL_COMBMODE_MASK)
#define SCT_EVENT_CTRL_STATELD_MASK              (0x4000U)
#define SCT_EVENT_CTRL_STATELD_SHIFT             (14U)
#define SCT_EVENT_CTRL_STATELD(x)                (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_STATELD_SHIFT)) & SCT_EVENT_CTRL_STATELD_MASK)
#define SCT_EVENT_CTRL_STATEV_MASK               (0xF8000U)
#define SCT_EVENT_CTRL_STATEV_SHIFT              (15U)
#define SCT_EVENT_CTRL_STATEV(x)                 (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_STATEV_SHIFT)) & SCT_EVENT_CTRL_STATEV_MASK)
#define SCT_EVENT_CTRL_MATCHMEM_MASK             (0x100000U)
#define SCT_EVENT_CTRL_MATCHMEM_SHIFT            (20U)
#define SCT_EVENT_CTRL_MATCHMEM(x)               (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_MATCHMEM_SHIFT)) & SCT_EVENT_CTRL_MATCHMEM_MASK)
#define SCT_EVENT_CTRL_DIRECTION_MASK            (0x600000U)
#define SCT_EVENT_CTRL_DIRECTION_SHIFT           (21U)
#define SCT_EVENT_CTRL_DIRECTION(x)              (((uint32_t)(((uint32_t)(x)) << SCT_EVENT_CTRL_DIRECTION_SHIFT)) & SCT_EVENT_CTRL_DIRECTION_MASK)

/* The count of SCT_EVENT_CTRL */
#define SCT_EVENT_CTRL_COUNT                     (10U)

/*! @name OUT_SET - SCT output 0 set register */
#define SCT_OUT_SET_SET_MASK                     (0xFFFFU)
#define SCT_OUT_SET_SET_SHIFT                    (0U)
#define SCT_OUT_SET_SET(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_OUT_SET_SET_SHIFT)) & SCT_OUT_SET_SET_MASK)

/* The count of SCT_OUT_SET */
#define SCT_OUT_SET_COUNT                        (8U)

/*! @name OUT_CLR - SCT output 0 clear register */
#define SCT_OUT_CLR_CLR_MASK                     (0xFFFFU)
#define SCT_OUT_CLR_CLR_SHIFT                    (0U)
#define SCT_OUT_CLR_CLR(x)                       (((uint32_t)(((uint32_t)(x)) << SCT_OUT_CLR_CLR_SHIFT)) & SCT_OUT_CLR_CLR_MASK)

/* The count of SCT_OUT_CLR */
#define SCT_OUT_CLR_COUNT                        (8U)


/*!
 * @}
 */ /* end of group SCT_Register_Masks */


/* SCT - Peripheral instance base addresses */
/** Peripheral SCT0 base address */
#define SCT0_BASE                                (0x40085000u)
/** Peripheral SCT0 base pointer */
#define SCT0                                     ((SCT_Type *)SCT0_BASE)
/** Array initializer of SCT peripheral base addresses */
#define SCT_BASE_ADDRS                           { SCT0_BASE }
/** Array initializer of SCT peripheral base pointers */
#define SCT_BASE_PTRS                            { SCT0 }
/** Interrupt vectors for the SCT peripheral type */
#define SCT_IRQS                                 { SCT0_IRQn }

/*!
 * @}
 */ /* end of group SCT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SPI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Peripheral_Access_Layer SPI Peripheral Access Layer
 * @{
 */

/** SPI - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[1024];
  __IO uint32_t CFG;                               /**< SPI Configuration register, offset: 0x400 */
  __IO uint32_t DLY;                               /**< SPI Delay register, offset: 0x404 */
  __IO uint32_t STAT;                              /**< SPI Status. Some status flags can be cleared by writing a 1 to that bit position., offset: 0x408 */
  __IO uint32_t INTENSET;                          /**< SPI Interrupt Enable read and Set. A complete value may be read from this register. Writing a 1 to any implemented bit position causes that bit to be set., offset: 0x40C */
  __O  uint32_t INTENCLR;                          /**< SPI Interrupt Enable Clear. Writing a 1 to any implemented bit position causes the corresponding bit in INTENSET to be cleared., offset: 0x410 */
       uint8_t RESERVED_1[16];
  __IO uint32_t DIV;                               /**< SPI clock Divider, offset: 0x424 */
  __I  uint32_t INTSTAT;                           /**< SPI Interrupt Status, offset: 0x428 */
       uint8_t RESERVED_2[2516];
  __IO uint32_t FIFOCFG;                           /**< FIFO configuration and enable register., offset: 0xE00 */
  __IO uint32_t FIFOSTAT;                          /**< FIFO status register., offset: 0xE04 */
  __IO uint32_t FIFOTRIG;                          /**< FIFO trigger settings for interrupt and DMA request., offset: 0xE08 */
       uint8_t RESERVED_3[4];
  __IO uint32_t FIFOINTENSET;                      /**< FIFO interrupt enable set (enable) and read register., offset: 0xE10 */
  __IO uint32_t FIFOINTENCLR;                      /**< FIFO interrupt enable clear (disable) and read register., offset: 0xE14 */
  __I  uint32_t FIFOINTSTAT;                       /**< FIFO interrupt status register., offset: 0xE18 */
       uint8_t RESERVED_4[4];
  __IO uint32_t FIFOWR;                            /**< FIFO write data., offset: 0xE20 */
       uint8_t RESERVED_5[12];
  __I  uint32_t FIFORD;                            /**< FIFO read data., offset: 0xE30 */
       uint8_t RESERVED_6[12];
  __I  uint32_t FIFORDNOPOP;                       /**< FIFO data read with no FIFO pop., offset: 0xE40 */
       uint8_t RESERVED_7[440];
  __I  uint32_t ID;                                /**< SPI module Identification. This value appears in the shared Flexcomm peripheral ID register when SPI is selected., offset: 0xFFC */
} SPI_Type;

/* ----------------------------------------------------------------------------
   -- SPI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Register_Masks SPI Register Masks
 * @{
 */

/*! @name CFG - SPI Configuration register */
#define SPI_CFG_ENABLE_MASK                      (0x1U)
#define SPI_CFG_ENABLE_SHIFT                     (0U)
#define SPI_CFG_ENABLE(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_CFG_ENABLE_SHIFT)) & SPI_CFG_ENABLE_MASK)
#define SPI_CFG_MASTER_MASK                      (0x4U)
#define SPI_CFG_MASTER_SHIFT                     (2U)
#define SPI_CFG_MASTER(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_CFG_MASTER_SHIFT)) & SPI_CFG_MASTER_MASK)
#define SPI_CFG_LSBF_MASK                        (0x8U)
#define SPI_CFG_LSBF_SHIFT                       (3U)
#define SPI_CFG_LSBF(x)                          (((uint32_t)(((uint32_t)(x)) << SPI_CFG_LSBF_SHIFT)) & SPI_CFG_LSBF_MASK)
#define SPI_CFG_CPHA_MASK                        (0x10U)
#define SPI_CFG_CPHA_SHIFT                       (4U)
#define SPI_CFG_CPHA(x)                          (((uint32_t)(((uint32_t)(x)) << SPI_CFG_CPHA_SHIFT)) & SPI_CFG_CPHA_MASK)
#define SPI_CFG_CPOL_MASK                        (0x20U)
#define SPI_CFG_CPOL_SHIFT                       (5U)
#define SPI_CFG_CPOL(x)                          (((uint32_t)(((uint32_t)(x)) << SPI_CFG_CPOL_SHIFT)) & SPI_CFG_CPOL_MASK)
#define SPI_CFG_LOOP_MASK                        (0x80U)
#define SPI_CFG_LOOP_SHIFT                       (7U)
#define SPI_CFG_LOOP(x)                          (((uint32_t)(((uint32_t)(x)) << SPI_CFG_LOOP_SHIFT)) & SPI_CFG_LOOP_MASK)
#define SPI_CFG_SPOL0_MASK                       (0x100U)
#define SPI_CFG_SPOL0_SHIFT                      (8U)
#define SPI_CFG_SPOL0(x)                         (((uint32_t)(((uint32_t)(x)) << SPI_CFG_SPOL0_SHIFT)) & SPI_CFG_SPOL0_MASK)
#define SPI_CFG_SPOL1_MASK                       (0x200U)
#define SPI_CFG_SPOL1_SHIFT                      (9U)
#define SPI_CFG_SPOL1(x)                         (((uint32_t)(((uint32_t)(x)) << SPI_CFG_SPOL1_SHIFT)) & SPI_CFG_SPOL1_MASK)
#define SPI_CFG_SPOL2_MASK                       (0x400U)
#define SPI_CFG_SPOL2_SHIFT                      (10U)
#define SPI_CFG_SPOL2(x)                         (((uint32_t)(((uint32_t)(x)) << SPI_CFG_SPOL2_SHIFT)) & SPI_CFG_SPOL2_MASK)
#define SPI_CFG_SPOL3_MASK                       (0x800U)
#define SPI_CFG_SPOL3_SHIFT                      (11U)
#define SPI_CFG_SPOL3(x)                         (((uint32_t)(((uint32_t)(x)) << SPI_CFG_SPOL3_SHIFT)) & SPI_CFG_SPOL3_MASK)

/*! @name DLY - SPI Delay register */
#define SPI_DLY_PRE_DELAY_MASK                   (0xFU)
#define SPI_DLY_PRE_DELAY_SHIFT                  (0U)
#define SPI_DLY_PRE_DELAY(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_DLY_PRE_DELAY_SHIFT)) & SPI_DLY_PRE_DELAY_MASK)
#define SPI_DLY_POST_DELAY_MASK                  (0xF0U)
#define SPI_DLY_POST_DELAY_SHIFT                 (4U)
#define SPI_DLY_POST_DELAY(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_DLY_POST_DELAY_SHIFT)) & SPI_DLY_POST_DELAY_MASK)
#define SPI_DLY_FRAME_DELAY_MASK                 (0xF00U)
#define SPI_DLY_FRAME_DELAY_SHIFT                (8U)
#define SPI_DLY_FRAME_DELAY(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_DLY_FRAME_DELAY_SHIFT)) & SPI_DLY_FRAME_DELAY_MASK)
#define SPI_DLY_TRANSFER_DELAY_MASK              (0xF000U)
#define SPI_DLY_TRANSFER_DELAY_SHIFT             (12U)
#define SPI_DLY_TRANSFER_DELAY(x)                (((uint32_t)(((uint32_t)(x)) << SPI_DLY_TRANSFER_DELAY_SHIFT)) & SPI_DLY_TRANSFER_DELAY_MASK)

/*! @name STAT - SPI Status. Some status flags can be cleared by writing a 1 to that bit position. */
#define SPI_STAT_SSA_MASK                        (0x10U)
#define SPI_STAT_SSA_SHIFT                       (4U)
#define SPI_STAT_SSA(x)                          (((uint32_t)(((uint32_t)(x)) << SPI_STAT_SSA_SHIFT)) & SPI_STAT_SSA_MASK)
#define SPI_STAT_SSD_MASK                        (0x20U)
#define SPI_STAT_SSD_SHIFT                       (5U)
#define SPI_STAT_SSD(x)                          (((uint32_t)(((uint32_t)(x)) << SPI_STAT_SSD_SHIFT)) & SPI_STAT_SSD_MASK)
#define SPI_STAT_STALLED_MASK                    (0x40U)
#define SPI_STAT_STALLED_SHIFT                   (6U)
#define SPI_STAT_STALLED(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_STAT_STALLED_SHIFT)) & SPI_STAT_STALLED_MASK)
#define SPI_STAT_ENDTRANSFER_MASK                (0x80U)
#define SPI_STAT_ENDTRANSFER_SHIFT               (7U)
#define SPI_STAT_ENDTRANSFER(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_STAT_ENDTRANSFER_SHIFT)) & SPI_STAT_ENDTRANSFER_MASK)
#define SPI_STAT_MSTIDLE_MASK                    (0x100U)
#define SPI_STAT_MSTIDLE_SHIFT                   (8U)
#define SPI_STAT_MSTIDLE(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_STAT_MSTIDLE_SHIFT)) & SPI_STAT_MSTIDLE_MASK)

/*! @name INTENSET - SPI Interrupt Enable read and Set. A complete value may be read from this register. Writing a 1 to any implemented bit position causes that bit to be set. */
#define SPI_INTENSET_SSAEN_MASK                  (0x10U)
#define SPI_INTENSET_SSAEN_SHIFT                 (4U)
#define SPI_INTENSET_SSAEN(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_INTENSET_SSAEN_SHIFT)) & SPI_INTENSET_SSAEN_MASK)
#define SPI_INTENSET_SSDEN_MASK                  (0x20U)
#define SPI_INTENSET_SSDEN_SHIFT                 (5U)
#define SPI_INTENSET_SSDEN(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_INTENSET_SSDEN_SHIFT)) & SPI_INTENSET_SSDEN_MASK)
#define SPI_INTENSET_MSTIDLEEN_MASK              (0x100U)
#define SPI_INTENSET_MSTIDLEEN_SHIFT             (8U)
#define SPI_INTENSET_MSTIDLEEN(x)                (((uint32_t)(((uint32_t)(x)) << SPI_INTENSET_MSTIDLEEN_SHIFT)) & SPI_INTENSET_MSTIDLEEN_MASK)

/*! @name INTENCLR - SPI Interrupt Enable Clear. Writing a 1 to any implemented bit position causes the corresponding bit in INTENSET to be cleared. */
#define SPI_INTENCLR_SSAEN_MASK                  (0x10U)
#define SPI_INTENCLR_SSAEN_SHIFT                 (4U)
#define SPI_INTENCLR_SSAEN(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_INTENCLR_SSAEN_SHIFT)) & SPI_INTENCLR_SSAEN_MASK)
#define SPI_INTENCLR_SSDEN_MASK                  (0x20U)
#define SPI_INTENCLR_SSDEN_SHIFT                 (5U)
#define SPI_INTENCLR_SSDEN(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_INTENCLR_SSDEN_SHIFT)) & SPI_INTENCLR_SSDEN_MASK)
#define SPI_INTENCLR_MSTIDLE_MASK                (0x100U)
#define SPI_INTENCLR_MSTIDLE_SHIFT               (8U)
#define SPI_INTENCLR_MSTIDLE(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_INTENCLR_MSTIDLE_SHIFT)) & SPI_INTENCLR_MSTIDLE_MASK)

/*! @name DIV - SPI clock Divider */
#define SPI_DIV_DIVVAL_MASK                      (0xFFFFU)
#define SPI_DIV_DIVVAL_SHIFT                     (0U)
#define SPI_DIV_DIVVAL(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_DIV_DIVVAL_SHIFT)) & SPI_DIV_DIVVAL_MASK)

/*! @name INTSTAT - SPI Interrupt Status */
#define SPI_INTSTAT_SSA_MASK                     (0x10U)
#define SPI_INTSTAT_SSA_SHIFT                    (4U)
#define SPI_INTSTAT_SSA(x)                       (((uint32_t)(((uint32_t)(x)) << SPI_INTSTAT_SSA_SHIFT)) & SPI_INTSTAT_SSA_MASK)
#define SPI_INTSTAT_SSD_MASK                     (0x20U)
#define SPI_INTSTAT_SSD_SHIFT                    (5U)
#define SPI_INTSTAT_SSD(x)                       (((uint32_t)(((uint32_t)(x)) << SPI_INTSTAT_SSD_SHIFT)) & SPI_INTSTAT_SSD_MASK)
#define SPI_INTSTAT_MSTIDLE_MASK                 (0x100U)
#define SPI_INTSTAT_MSTIDLE_SHIFT                (8U)
#define SPI_INTSTAT_MSTIDLE(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_INTSTAT_MSTIDLE_SHIFT)) & SPI_INTSTAT_MSTIDLE_MASK)

/*! @name FIFOCFG - FIFO configuration and enable register. */
#define SPI_FIFOCFG_ENABLETX_MASK                (0x1U)
#define SPI_FIFOCFG_ENABLETX_SHIFT               (0U)
#define SPI_FIFOCFG_ENABLETX(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_ENABLETX_SHIFT)) & SPI_FIFOCFG_ENABLETX_MASK)
#define SPI_FIFOCFG_ENABLERX_MASK                (0x2U)
#define SPI_FIFOCFG_ENABLERX_SHIFT               (1U)
#define SPI_FIFOCFG_ENABLERX(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_ENABLERX_SHIFT)) & SPI_FIFOCFG_ENABLERX_MASK)
#define SPI_FIFOCFG_SIZE_MASK                    (0x30U)
#define SPI_FIFOCFG_SIZE_SHIFT                   (4U)
#define SPI_FIFOCFG_SIZE(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_SIZE_SHIFT)) & SPI_FIFOCFG_SIZE_MASK)
#define SPI_FIFOCFG_DMATX_MASK                   (0x1000U)
#define SPI_FIFOCFG_DMATX_SHIFT                  (12U)
#define SPI_FIFOCFG_DMATX(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_DMATX_SHIFT)) & SPI_FIFOCFG_DMATX_MASK)
#define SPI_FIFOCFG_DMARX_MASK                   (0x2000U)
#define SPI_FIFOCFG_DMARX_SHIFT                  (13U)
#define SPI_FIFOCFG_DMARX(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_DMARX_SHIFT)) & SPI_FIFOCFG_DMARX_MASK)
#define SPI_FIFOCFG_EMPTYTX_MASK                 (0x10000U)
#define SPI_FIFOCFG_EMPTYTX_SHIFT                (16U)
#define SPI_FIFOCFG_EMPTYTX(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_EMPTYTX_SHIFT)) & SPI_FIFOCFG_EMPTYTX_MASK)
#define SPI_FIFOCFG_EMPTYRX_MASK                 (0x20000U)
#define SPI_FIFOCFG_EMPTYRX_SHIFT                (17U)
#define SPI_FIFOCFG_EMPTYRX(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_FIFOCFG_EMPTYRX_SHIFT)) & SPI_FIFOCFG_EMPTYRX_MASK)

/*! @name FIFOSTAT - FIFO status register. */
#define SPI_FIFOSTAT_TXERR_MASK                  (0x1U)
#define SPI_FIFOSTAT_TXERR_SHIFT                 (0U)
#define SPI_FIFOSTAT_TXERR(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_TXERR_SHIFT)) & SPI_FIFOSTAT_TXERR_MASK)
#define SPI_FIFOSTAT_RXERR_MASK                  (0x2U)
#define SPI_FIFOSTAT_RXERR_SHIFT                 (1U)
#define SPI_FIFOSTAT_RXERR(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_RXERR_SHIFT)) & SPI_FIFOSTAT_RXERR_MASK)
#define SPI_FIFOSTAT_PERINT_MASK                 (0x8U)
#define SPI_FIFOSTAT_PERINT_SHIFT                (3U)
#define SPI_FIFOSTAT_PERINT(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_PERINT_SHIFT)) & SPI_FIFOSTAT_PERINT_MASK)
#define SPI_FIFOSTAT_TXEMPTY_MASK                (0x10U)
#define SPI_FIFOSTAT_TXEMPTY_SHIFT               (4U)
#define SPI_FIFOSTAT_TXEMPTY(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_TXEMPTY_SHIFT)) & SPI_FIFOSTAT_TXEMPTY_MASK)
#define SPI_FIFOSTAT_TXNOTFULL_MASK              (0x20U)
#define SPI_FIFOSTAT_TXNOTFULL_SHIFT             (5U)
#define SPI_FIFOSTAT_TXNOTFULL(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_TXNOTFULL_SHIFT)) & SPI_FIFOSTAT_TXNOTFULL_MASK)
#define SPI_FIFOSTAT_RXNOTEMPTY_MASK             (0x40U)
#define SPI_FIFOSTAT_RXNOTEMPTY_SHIFT            (6U)
#define SPI_FIFOSTAT_RXNOTEMPTY(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_RXNOTEMPTY_SHIFT)) & SPI_FIFOSTAT_RXNOTEMPTY_MASK)
#define SPI_FIFOSTAT_RXFULL_MASK                 (0x80U)
#define SPI_FIFOSTAT_RXFULL_SHIFT                (7U)
#define SPI_FIFOSTAT_RXFULL(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_RXFULL_SHIFT)) & SPI_FIFOSTAT_RXFULL_MASK)
#define SPI_FIFOSTAT_TXLVL_MASK                  (0x1F00U)
#define SPI_FIFOSTAT_TXLVL_SHIFT                 (8U)
#define SPI_FIFOSTAT_TXLVL(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_TXLVL_SHIFT)) & SPI_FIFOSTAT_TXLVL_MASK)
#define SPI_FIFOSTAT_RXLVL_MASK                  (0x1F0000U)
#define SPI_FIFOSTAT_RXLVL_SHIFT                 (16U)
#define SPI_FIFOSTAT_RXLVL(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_FIFOSTAT_RXLVL_SHIFT)) & SPI_FIFOSTAT_RXLVL_MASK)

/*! @name FIFOTRIG - FIFO trigger settings for interrupt and DMA request. */
#define SPI_FIFOTRIG_TXLVLENA_MASK               (0x1U)
#define SPI_FIFOTRIG_TXLVLENA_SHIFT              (0U)
#define SPI_FIFOTRIG_TXLVLENA(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_FIFOTRIG_TXLVLENA_SHIFT)) & SPI_FIFOTRIG_TXLVLENA_MASK)
#define SPI_FIFOTRIG_RXLVLENA_MASK               (0x2U)
#define SPI_FIFOTRIG_RXLVLENA_SHIFT              (1U)
#define SPI_FIFOTRIG_RXLVLENA(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_FIFOTRIG_RXLVLENA_SHIFT)) & SPI_FIFOTRIG_RXLVLENA_MASK)
#define SPI_FIFOTRIG_TXLVL_MASK                  (0xF00U)
#define SPI_FIFOTRIG_TXLVL_SHIFT                 (8U)
#define SPI_FIFOTRIG_TXLVL(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_FIFOTRIG_TXLVL_SHIFT)) & SPI_FIFOTRIG_TXLVL_MASK)
#define SPI_FIFOTRIG_RXLVL_MASK                  (0xF0000U)
#define SPI_FIFOTRIG_RXLVL_SHIFT                 (16U)
#define SPI_FIFOTRIG_RXLVL(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_FIFOTRIG_RXLVL_SHIFT)) & SPI_FIFOTRIG_RXLVL_MASK)

/*! @name FIFOINTENSET - FIFO interrupt enable set (enable) and read register. */
#define SPI_FIFOINTENSET_TXERR_MASK              (0x1U)
#define SPI_FIFOINTENSET_TXERR_SHIFT             (0U)
#define SPI_FIFOINTENSET_TXERR(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENSET_TXERR_SHIFT)) & SPI_FIFOINTENSET_TXERR_MASK)
#define SPI_FIFOINTENSET_RXERR_MASK              (0x2U)
#define SPI_FIFOINTENSET_RXERR_SHIFT             (1U)
#define SPI_FIFOINTENSET_RXERR(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENSET_RXERR_SHIFT)) & SPI_FIFOINTENSET_RXERR_MASK)
#define SPI_FIFOINTENSET_TXLVL_MASK              (0x4U)
#define SPI_FIFOINTENSET_TXLVL_SHIFT             (2U)
#define SPI_FIFOINTENSET_TXLVL(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENSET_TXLVL_SHIFT)) & SPI_FIFOINTENSET_TXLVL_MASK)
#define SPI_FIFOINTENSET_RXLVL_MASK              (0x8U)
#define SPI_FIFOINTENSET_RXLVL_SHIFT             (3U)
#define SPI_FIFOINTENSET_RXLVL(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENSET_RXLVL_SHIFT)) & SPI_FIFOINTENSET_RXLVL_MASK)

/*! @name FIFOINTENCLR - FIFO interrupt enable clear (disable) and read register. */
#define SPI_FIFOINTENCLR_TXERR_MASK              (0x1U)
#define SPI_FIFOINTENCLR_TXERR_SHIFT             (0U)
#define SPI_FIFOINTENCLR_TXERR(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENCLR_TXERR_SHIFT)) & SPI_FIFOINTENCLR_TXERR_MASK)
#define SPI_FIFOINTENCLR_RXERR_MASK              (0x2U)
#define SPI_FIFOINTENCLR_RXERR_SHIFT             (1U)
#define SPI_FIFOINTENCLR_RXERR(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENCLR_RXERR_SHIFT)) & SPI_FIFOINTENCLR_RXERR_MASK)
#define SPI_FIFOINTENCLR_TXLVL_MASK              (0x4U)
#define SPI_FIFOINTENCLR_TXLVL_SHIFT             (2U)
#define SPI_FIFOINTENCLR_TXLVL(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENCLR_TXLVL_SHIFT)) & SPI_FIFOINTENCLR_TXLVL_MASK)
#define SPI_FIFOINTENCLR_RXLVL_MASK              (0x8U)
#define SPI_FIFOINTENCLR_RXLVL_SHIFT             (3U)
#define SPI_FIFOINTENCLR_RXLVL(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTENCLR_RXLVL_SHIFT)) & SPI_FIFOINTENCLR_RXLVL_MASK)

/*! @name FIFOINTSTAT - FIFO interrupt status register. */
#define SPI_FIFOINTSTAT_TXERR_MASK               (0x1U)
#define SPI_FIFOINTSTAT_TXERR_SHIFT              (0U)
#define SPI_FIFOINTSTAT_TXERR(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTSTAT_TXERR_SHIFT)) & SPI_FIFOINTSTAT_TXERR_MASK)
#define SPI_FIFOINTSTAT_RXERR_MASK               (0x2U)
#define SPI_FIFOINTSTAT_RXERR_SHIFT              (1U)
#define SPI_FIFOINTSTAT_RXERR(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTSTAT_RXERR_SHIFT)) & SPI_FIFOINTSTAT_RXERR_MASK)
#define SPI_FIFOINTSTAT_TXLVL_MASK               (0x4U)
#define SPI_FIFOINTSTAT_TXLVL_SHIFT              (2U)
#define SPI_FIFOINTSTAT_TXLVL(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTSTAT_TXLVL_SHIFT)) & SPI_FIFOINTSTAT_TXLVL_MASK)
#define SPI_FIFOINTSTAT_RXLVL_MASK               (0x8U)
#define SPI_FIFOINTSTAT_RXLVL_SHIFT              (3U)
#define SPI_FIFOINTSTAT_RXLVL(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTSTAT_RXLVL_SHIFT)) & SPI_FIFOINTSTAT_RXLVL_MASK)
#define SPI_FIFOINTSTAT_PERINT_MASK              (0x10U)
#define SPI_FIFOINTSTAT_PERINT_SHIFT             (4U)
#define SPI_FIFOINTSTAT_PERINT(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFOINTSTAT_PERINT_SHIFT)) & SPI_FIFOINTSTAT_PERINT_MASK)

/*! @name FIFOWR - FIFO write data. */
#define SPI_FIFOWR_TXDATA_MASK                   (0xFFFFU)
#define SPI_FIFOWR_TXDATA_SHIFT                  (0U)
#define SPI_FIFOWR_TXDATA(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_TXDATA_SHIFT)) & SPI_FIFOWR_TXDATA_MASK)
#define SPI_FIFOWR_TXSSEL0_N_MASK                (0x10000U)
#define SPI_FIFOWR_TXSSEL0_N_SHIFT               (16U)
#define SPI_FIFOWR_TXSSEL0_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_TXSSEL0_N_SHIFT)) & SPI_FIFOWR_TXSSEL0_N_MASK)
#define SPI_FIFOWR_TXSSEL1_N_MASK                (0x20000U)
#define SPI_FIFOWR_TXSSEL1_N_SHIFT               (17U)
#define SPI_FIFOWR_TXSSEL1_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_TXSSEL1_N_SHIFT)) & SPI_FIFOWR_TXSSEL1_N_MASK)
#define SPI_FIFOWR_TXSSEL2_N_MASK                (0x40000U)
#define SPI_FIFOWR_TXSSEL2_N_SHIFT               (18U)
#define SPI_FIFOWR_TXSSEL2_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_TXSSEL2_N_SHIFT)) & SPI_FIFOWR_TXSSEL2_N_MASK)
#define SPI_FIFOWR_TXSSEL3_N_MASK                (0x80000U)
#define SPI_FIFOWR_TXSSEL3_N_SHIFT               (19U)
#define SPI_FIFOWR_TXSSEL3_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_TXSSEL3_N_SHIFT)) & SPI_FIFOWR_TXSSEL3_N_MASK)
#define SPI_FIFOWR_EOT_MASK                      (0x100000U)
#define SPI_FIFOWR_EOT_SHIFT                     (20U)
#define SPI_FIFOWR_EOT(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_EOT_SHIFT)) & SPI_FIFOWR_EOT_MASK)
#define SPI_FIFOWR_EOF_MASK                      (0x200000U)
#define SPI_FIFOWR_EOF_SHIFT                     (21U)
#define SPI_FIFOWR_EOF(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_EOF_SHIFT)) & SPI_FIFOWR_EOF_MASK)
#define SPI_FIFOWR_RXIGNORE_MASK                 (0x400000U)
#define SPI_FIFOWR_RXIGNORE_SHIFT                (22U)
#define SPI_FIFOWR_RXIGNORE(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_RXIGNORE_SHIFT)) & SPI_FIFOWR_RXIGNORE_MASK)
#define SPI_FIFOWR_LEN_MASK                      (0xF000000U)
#define SPI_FIFOWR_LEN_SHIFT                     (24U)
#define SPI_FIFOWR_LEN(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_FIFOWR_LEN_SHIFT)) & SPI_FIFOWR_LEN_MASK)

/*! @name FIFORD - FIFO read data. */
#define SPI_FIFORD_RXDATA_MASK                   (0xFFFFU)
#define SPI_FIFORD_RXDATA_SHIFT                  (0U)
#define SPI_FIFORD_RXDATA(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_FIFORD_RXDATA_SHIFT)) & SPI_FIFORD_RXDATA_MASK)
#define SPI_FIFORD_RXSSEL0_N_MASK                (0x10000U)
#define SPI_FIFORD_RXSSEL0_N_SHIFT               (16U)
#define SPI_FIFORD_RXSSEL0_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFORD_RXSSEL0_N_SHIFT)) & SPI_FIFORD_RXSSEL0_N_MASK)
#define SPI_FIFORD_RXSSEL1_N_MASK                (0x20000U)
#define SPI_FIFORD_RXSSEL1_N_SHIFT               (17U)
#define SPI_FIFORD_RXSSEL1_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFORD_RXSSEL1_N_SHIFT)) & SPI_FIFORD_RXSSEL1_N_MASK)
#define SPI_FIFORD_RXSSEL2_N_MASK                (0x40000U)
#define SPI_FIFORD_RXSSEL2_N_SHIFT               (18U)
#define SPI_FIFORD_RXSSEL2_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFORD_RXSSEL2_N_SHIFT)) & SPI_FIFORD_RXSSEL2_N_MASK)
#define SPI_FIFORD_RXSSEL3_N_MASK                (0x80000U)
#define SPI_FIFORD_RXSSEL3_N_SHIFT               (19U)
#define SPI_FIFORD_RXSSEL3_N(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_FIFORD_RXSSEL3_N_SHIFT)) & SPI_FIFORD_RXSSEL3_N_MASK)
#define SPI_FIFORD_SOT_MASK                      (0x100000U)
#define SPI_FIFORD_SOT_SHIFT                     (20U)
#define SPI_FIFORD_SOT(x)                        (((uint32_t)(((uint32_t)(x)) << SPI_FIFORD_SOT_SHIFT)) & SPI_FIFORD_SOT_MASK)

/*! @name FIFORDNOPOP - FIFO data read with no FIFO pop. */
#define SPI_FIFORDNOPOP_RXDATA_MASK              (0xFFFFU)
#define SPI_FIFORDNOPOP_RXDATA_SHIFT             (0U)
#define SPI_FIFORDNOPOP_RXDATA(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FIFORDNOPOP_RXDATA_SHIFT)) & SPI_FIFORDNOPOP_RXDATA_MASK)
#define SPI_FIFORDNOPOP_RXSSEL0_N_MASK           (0x10000U)
#define SPI_FIFORDNOPOP_RXSSEL0_N_SHIFT          (16U)
#define SPI_FIFORDNOPOP_RXSSEL0_N(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FIFORDNOPOP_RXSSEL0_N_SHIFT)) & SPI_FIFORDNOPOP_RXSSEL0_N_MASK)
#define SPI_FIFORDNOPOP_RXSSEL1_N_MASK           (0x20000U)
#define SPI_FIFORDNOPOP_RXSSEL1_N_SHIFT          (17U)
#define SPI_FIFORDNOPOP_RXSSEL1_N(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FIFORDNOPOP_RXSSEL1_N_SHIFT)) & SPI_FIFORDNOPOP_RXSSEL1_N_MASK)
#define SPI_FIFORDNOPOP_RXSSEL2_N_MASK           (0x40000U)
#define SPI_FIFORDNOPOP_RXSSEL2_N_SHIFT          (18U)
#define SPI_FIFORDNOPOP_RXSSEL2_N(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FIFORDNOPOP_RXSSEL2_N_SHIFT)) & SPI_FIFORDNOPOP_RXSSEL2_N_MASK)
#define SPI_FIFORDNOPOP_RXSSEL3_N_MASK           (0x80000U)
#define SPI_FIFORDNOPOP_RXSSEL3_N_SHIFT          (19U)
#define SPI_FIFORDNOPOP_RXSSEL3_N(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FIFORDNOPOP_RXSSEL3_N_SHIFT)) & SPI_FIFORDNOPOP_RXSSEL3_N_MASK)
#define SPI_FIFORDNOPOP_SOT_MASK                 (0x100000U)
#define SPI_FIFORDNOPOP_SOT_SHIFT                (20U)
#define SPI_FIFORDNOPOP_SOT(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_FIFORDNOPOP_SOT_SHIFT)) & SPI_FIFORDNOPOP_SOT_MASK)

/*! @name ID - SPI module Identification. This value appears in the shared Flexcomm peripheral ID register when SPI is selected. */
#define SPI_ID_APERTURE_MASK                     (0xFFU)
#define SPI_ID_APERTURE_SHIFT                    (0U)
#define SPI_ID_APERTURE(x)                       (((uint32_t)(((uint32_t)(x)) << SPI_ID_APERTURE_SHIFT)) & SPI_ID_APERTURE_MASK)
#define SPI_ID_MINOR_REV_MASK                    (0xF00U)
#define SPI_ID_MINOR_REV_SHIFT                   (8U)
#define SPI_ID_MINOR_REV(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_ID_MINOR_REV_SHIFT)) & SPI_ID_MINOR_REV_MASK)
#define SPI_ID_MAJOR_REV_MASK                    (0xF000U)
#define SPI_ID_MAJOR_REV_SHIFT                   (12U)
#define SPI_ID_MAJOR_REV(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_ID_MAJOR_REV_SHIFT)) & SPI_ID_MAJOR_REV_MASK)
#define SPI_ID_ID_MASK                           (0xFFFF0000U)
#define SPI_ID_ID_SHIFT                          (16U)
#define SPI_ID_ID(x)                             (((uint32_t)(((uint32_t)(x)) << SPI_ID_ID_SHIFT)) & SPI_ID_ID_MASK)


/*!
 * @}
 */ /* end of group SPI_Register_Masks */


/* SPI - Peripheral instance base addresses */
/** Peripheral SPI0 base address */
#define SPI0_BASE                                (0x40087000u)
/** Peripheral SPI0 base pointer */
#define SPI0                                     ((SPI_Type *)SPI0_BASE)
/** Peripheral SPI1 base address */
#define SPI1_BASE                                (0x4008F000u)
/** Peripheral SPI1 base pointer */
#define SPI1                                     ((SPI_Type *)SPI1_BASE)
/** Array initializer of SPI peripheral base addresses */
#define SPI_BASE_ADDRS                           { SPI0_BASE, SPI1_BASE }
/** Array initializer of SPI peripheral base pointers */
#define SPI_BASE_PTRS                            { SPI0, SPI1 }
/** Interrupt vectors for the SPI peripheral type */
#define SPI_IRQS                                 { FLEXCOMM2_IRQn, FLEXCOMM3_IRQn }

/*!
 * @}
 */ /* end of group SPI_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SPIFI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPIFI_Peripheral_Access_Layer SPIFI Peripheral Access Layer
 * @{
 */

/** SPIFI - Register Layout Typedef */
typedef struct {
  __IO uint32_t CTRL;                              /**< SPIFI control register, offset: 0x0 */
  __IO uint32_t CMD;                               /**< SPIFI command register, offset: 0x4 */
  __IO uint32_t ADDR;                              /**< SPIFI address register, offset: 0x8 */
  __IO uint32_t IDATA;                             /**< SPIFI intermediate data register, offset: 0xC */
  __IO uint32_t CLIMIT;                            /**< SPIFI limit register, offset: 0x10 */
  __IO uint32_t DATA;                              /**< SPIFI data register, offset: 0x14 */
  __IO uint32_t MCMD;                              /**< SPIFI memory command register, offset: 0x18 */
  __IO uint32_t STAT;                              /**< SPIFI status register, offset: 0x1C */
} SPIFI_Type;

/* ----------------------------------------------------------------------------
   -- SPIFI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPIFI_Register_Masks SPIFI Register Masks
 * @{
 */

/*! @name CTRL - SPIFI control register */
#define SPIFI_CTRL_TIMEOUT_MASK                  (0xFFFFU)
#define SPIFI_CTRL_TIMEOUT_SHIFT                 (0U)
#define SPIFI_CTRL_TIMEOUT(x)                    (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_TIMEOUT_SHIFT)) & SPIFI_CTRL_TIMEOUT_MASK)
#define SPIFI_CTRL_CSHIGH_MASK                   (0xF0000U)
#define SPIFI_CTRL_CSHIGH_SHIFT                  (16U)
#define SPIFI_CTRL_CSHIGH(x)                     (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_CSHIGH_SHIFT)) & SPIFI_CTRL_CSHIGH_MASK)
#define SPIFI_CTRL_D_PRFTCH_DIS_MASK             (0x200000U)
#define SPIFI_CTRL_D_PRFTCH_DIS_SHIFT            (21U)
#define SPIFI_CTRL_D_PRFTCH_DIS(x)               (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_D_PRFTCH_DIS_SHIFT)) & SPIFI_CTRL_D_PRFTCH_DIS_MASK)
#define SPIFI_CTRL_INTEN_MASK                    (0x400000U)
#define SPIFI_CTRL_INTEN_SHIFT                   (22U)
#define SPIFI_CTRL_INTEN(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_INTEN_SHIFT)) & SPIFI_CTRL_INTEN_MASK)
#define SPIFI_CTRL_MODE3_MASK                    (0x800000U)
#define SPIFI_CTRL_MODE3_SHIFT                   (23U)
#define SPIFI_CTRL_MODE3(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_MODE3_SHIFT)) & SPIFI_CTRL_MODE3_MASK)
#define SPIFI_CTRL_PRFTCH_DIS_MASK               (0x8000000U)
#define SPIFI_CTRL_PRFTCH_DIS_SHIFT              (27U)
#define SPIFI_CTRL_PRFTCH_DIS(x)                 (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_PRFTCH_DIS_SHIFT)) & SPIFI_CTRL_PRFTCH_DIS_MASK)
#define SPIFI_CTRL_DUAL_MASK                     (0x10000000U)
#define SPIFI_CTRL_DUAL_SHIFT                    (28U)
#define SPIFI_CTRL_DUAL(x)                       (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_DUAL_SHIFT)) & SPIFI_CTRL_DUAL_MASK)
#define SPIFI_CTRL_RFCLK_MASK                    (0x20000000U)
#define SPIFI_CTRL_RFCLK_SHIFT                   (29U)
#define SPIFI_CTRL_RFCLK(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_RFCLK_SHIFT)) & SPIFI_CTRL_RFCLK_MASK)
#define SPIFI_CTRL_FBCLK_MASK                    (0x40000000U)
#define SPIFI_CTRL_FBCLK_SHIFT                   (30U)
#define SPIFI_CTRL_FBCLK(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_FBCLK_SHIFT)) & SPIFI_CTRL_FBCLK_MASK)
#define SPIFI_CTRL_DMAEN_MASK                    (0x80000000U)
#define SPIFI_CTRL_DMAEN_SHIFT                   (31U)
#define SPIFI_CTRL_DMAEN(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CTRL_DMAEN_SHIFT)) & SPIFI_CTRL_DMAEN_MASK)

/*! @name CMD - SPIFI command register */
#define SPIFI_CMD_DATALEN_MASK                   (0x3FFFU)
#define SPIFI_CMD_DATALEN_SHIFT                  (0U)
#define SPIFI_CMD_DATALEN(x)                     (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_DATALEN_SHIFT)) & SPIFI_CMD_DATALEN_MASK)
#define SPIFI_CMD_POLL_MASK                      (0x4000U)
#define SPIFI_CMD_POLL_SHIFT                     (14U)
#define SPIFI_CMD_POLL(x)                        (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_POLL_SHIFT)) & SPIFI_CMD_POLL_MASK)
#define SPIFI_CMD_DOUT_MASK                      (0x8000U)
#define SPIFI_CMD_DOUT_SHIFT                     (15U)
#define SPIFI_CMD_DOUT(x)                        (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_DOUT_SHIFT)) & SPIFI_CMD_DOUT_MASK)
#define SPIFI_CMD_INTLEN_MASK                    (0x70000U)
#define SPIFI_CMD_INTLEN_SHIFT                   (16U)
#define SPIFI_CMD_INTLEN(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_INTLEN_SHIFT)) & SPIFI_CMD_INTLEN_MASK)
#define SPIFI_CMD_FIELDFORM_MASK                 (0x180000U)
#define SPIFI_CMD_FIELDFORM_SHIFT                (19U)
#define SPIFI_CMD_FIELDFORM(x)                   (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_FIELDFORM_SHIFT)) & SPIFI_CMD_FIELDFORM_MASK)
#define SPIFI_CMD_FRAMEFORM_MASK                 (0xE00000U)
#define SPIFI_CMD_FRAMEFORM_SHIFT                (21U)
#define SPIFI_CMD_FRAMEFORM(x)                   (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_FRAMEFORM_SHIFT)) & SPIFI_CMD_FRAMEFORM_MASK)
#define SPIFI_CMD_OPCODE_MASK                    (0xFF000000U)
#define SPIFI_CMD_OPCODE_SHIFT                   (24U)
#define SPIFI_CMD_OPCODE(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_CMD_OPCODE_SHIFT)) & SPIFI_CMD_OPCODE_MASK)

/*! @name ADDR - SPIFI address register */
#define SPIFI_ADDR_ADDRESS_MASK                  (0xFFFFFFFFU)
#define SPIFI_ADDR_ADDRESS_SHIFT                 (0U)
#define SPIFI_ADDR_ADDRESS(x)                    (((uint32_t)(((uint32_t)(x)) << SPIFI_ADDR_ADDRESS_SHIFT)) & SPIFI_ADDR_ADDRESS_MASK)

/*! @name IDATA - SPIFI intermediate data register */
#define SPIFI_IDATA_IDATA_MASK                   (0xFFFFFFFFU)
#define SPIFI_IDATA_IDATA_SHIFT                  (0U)
#define SPIFI_IDATA_IDATA(x)                     (((uint32_t)(((uint32_t)(x)) << SPIFI_IDATA_IDATA_SHIFT)) & SPIFI_IDATA_IDATA_MASK)

/*! @name CLIMIT - SPIFI limit register */
#define SPIFI_CLIMIT_CLIMIT_MASK                 (0xFFFFFFFFU)
#define SPIFI_CLIMIT_CLIMIT_SHIFT                (0U)
#define SPIFI_CLIMIT_CLIMIT(x)                   (((uint32_t)(((uint32_t)(x)) << SPIFI_CLIMIT_CLIMIT_SHIFT)) & SPIFI_CLIMIT_CLIMIT_MASK)

/*! @name DATA - SPIFI data register */
#define SPIFI_DATA_DATA_MASK                     (0xFFFFFFFFU)
#define SPIFI_DATA_DATA_SHIFT                    (0U)
#define SPIFI_DATA_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << SPIFI_DATA_DATA_SHIFT)) & SPIFI_DATA_DATA_MASK)

/*! @name MCMD - SPIFI memory command register */
#define SPIFI_MCMD_POLL_MASK                     (0x4000U)
#define SPIFI_MCMD_POLL_SHIFT                    (14U)
#define SPIFI_MCMD_POLL(x)                       (((uint32_t)(((uint32_t)(x)) << SPIFI_MCMD_POLL_SHIFT)) & SPIFI_MCMD_POLL_MASK)
#define SPIFI_MCMD_DOUT_MASK                     (0x8000U)
#define SPIFI_MCMD_DOUT_SHIFT                    (15U)
#define SPIFI_MCMD_DOUT(x)                       (((uint32_t)(((uint32_t)(x)) << SPIFI_MCMD_DOUT_SHIFT)) & SPIFI_MCMD_DOUT_MASK)
#define SPIFI_MCMD_INTLEN_MASK                   (0x70000U)
#define SPIFI_MCMD_INTLEN_SHIFT                  (16U)
#define SPIFI_MCMD_INTLEN(x)                     (((uint32_t)(((uint32_t)(x)) << SPIFI_MCMD_INTLEN_SHIFT)) & SPIFI_MCMD_INTLEN_MASK)
#define SPIFI_MCMD_FIELDFORM_MASK                (0x180000U)
#define SPIFI_MCMD_FIELDFORM_SHIFT               (19U)
#define SPIFI_MCMD_FIELDFORM(x)                  (((uint32_t)(((uint32_t)(x)) << SPIFI_MCMD_FIELDFORM_SHIFT)) & SPIFI_MCMD_FIELDFORM_MASK)
#define SPIFI_MCMD_FRAMEFORM_MASK                (0xE00000U)
#define SPIFI_MCMD_FRAMEFORM_SHIFT               (21U)
#define SPIFI_MCMD_FRAMEFORM(x)                  (((uint32_t)(((uint32_t)(x)) << SPIFI_MCMD_FRAMEFORM_SHIFT)) & SPIFI_MCMD_FRAMEFORM_MASK)
#define SPIFI_MCMD_OPCODE_MASK                   (0xFF000000U)
#define SPIFI_MCMD_OPCODE_SHIFT                  (24U)
#define SPIFI_MCMD_OPCODE(x)                     (((uint32_t)(((uint32_t)(x)) << SPIFI_MCMD_OPCODE_SHIFT)) & SPIFI_MCMD_OPCODE_MASK)

/*! @name STAT - SPIFI status register */
#define SPIFI_STAT_MCINIT_MASK                   (0x1U)
#define SPIFI_STAT_MCINIT_SHIFT                  (0U)
#define SPIFI_STAT_MCINIT(x)                     (((uint32_t)(((uint32_t)(x)) << SPIFI_STAT_MCINIT_SHIFT)) & SPIFI_STAT_MCINIT_MASK)
#define SPIFI_STAT_CMD_MASK                      (0x2U)
#define SPIFI_STAT_CMD_SHIFT                     (1U)
#define SPIFI_STAT_CMD(x)                        (((uint32_t)(((uint32_t)(x)) << SPIFI_STAT_CMD_SHIFT)) & SPIFI_STAT_CMD_MASK)
#define SPIFI_STAT_RESET_MASK                    (0x10U)
#define SPIFI_STAT_RESET_SHIFT                   (4U)
#define SPIFI_STAT_RESET(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_STAT_RESET_SHIFT)) & SPIFI_STAT_RESET_MASK)
#define SPIFI_STAT_INTRQ_MASK                    (0x20U)
#define SPIFI_STAT_INTRQ_SHIFT                   (5U)
#define SPIFI_STAT_INTRQ(x)                      (((uint32_t)(((uint32_t)(x)) << SPIFI_STAT_INTRQ_SHIFT)) & SPIFI_STAT_INTRQ_MASK)
#define SPIFI_STAT_VERSION_MASK                  (0xFF000000U)
#define SPIFI_STAT_VERSION_SHIFT                 (24U)
#define SPIFI_STAT_VERSION(x)                    (((uint32_t)(((uint32_t)(x)) << SPIFI_STAT_VERSION_SHIFT)) & SPIFI_STAT_VERSION_MASK)


/*!
 * @}
 */ /* end of group SPIFI_Register_Masks */


/* SPIFI - Peripheral instance base addresses */
/** Peripheral SPIFI0 base address */
#define SPIFI0_BASE                              (0x40080000u)
/** Peripheral SPIFI0 base pointer */
#define SPIFI0                                   ((SPIFI_Type *)SPIFI0_BASE)
/** Array initializer of SPIFI peripheral base addresses */
#define SPIFI_BASE_ADDRS                         { SPIFI0_BASE }
/** Array initializer of SPIFI peripheral base pointers */
#define SPIFI_BASE_PTRS                          { SPIFI0 }
/** Interrupt vectors for the SPIFI peripheral type */
#define SPIFI_IRQS                               { SPIFI0_IRQn }

/*!
 * @}
 */ /* end of group SPIFI_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SYSCON Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SYSCON_Peripheral_Access_Layer SYSCON Peripheral Access Layer
 * @{
 */

/** SYSCON - Register Layout Typedef */
typedef struct {
  __IO uint32_t RST_SW_SET;                        /**< block software reset set register, offset: 0x0 */
  __IO uint32_t RST_SW_CLR;                        /**< block software reset clear register, offset: 0x4 */
  __IO uint32_t CLK_DIS;                           /**< clock disable register, offset: 0x8 */
  __IO uint32_t CLK_EN;                            /**< clock enable register, offset: 0xC */
  __IO uint32_t CLK_CTRL;                          /**< system clock source and divider register, offset: 0x10 */
  __IO uint32_t SYS_MODE_CTRL;                     /**< system mode and address remap register, offset: 0x14 */
       uint8_t RESERVED_0[104];
  __I  uint32_t SYS_STAT;                          /**< system status register, offset: 0x80 */
       uint8_t RESERVED_1[124];
  __IO uint32_t SYS_TICK;                          /**< systick timer control register, offset: 0x100 */
  __IO uint32_t SRAM_CTRL;                         /**< Exchange memory base address register, offset: 0x104 */
  __I  uint32_t CHIP_ID;                           /**< chip id register, offset: 0x108 */
       uint8_t RESERVED_2[4];
  __IO uint32_t ANA_CTRL0;                         /**< crystal and PA register, offset: 0x110 */
       uint8_t RESERVED_3[108];
  __IO uint32_t XTAL_CTRL;                         /**< crystal control register, offset: 0x180 */
  __IO uint32_t BUCK;                              /**< buck control register, offset: 0x184 */
       uint8_t RESERVED_4[120];
  __IO uint32_t FC_FRG;                            /**< flexcomm 0 and 1 clock divider register, offset: 0x200 */
       uint8_t RESERVED_5[1532];
  __IO uint32_t PIO_PULL_CFG[3];                   /**< pad pull control register 0..pad pull control register 2, array offset: 0x800, array step: 0x4 */
  __IO uint32_t IO_CAP;                            /**< io status capture register, offset: 0x80C */
  __IO uint32_t PIO_DRV_CFG[3];                    /**< pad drive strength register 0..pad drive extra register, array offset: 0x810, array step: 0x4 */
  __IO uint32_t PIO_CFG_MISC;                      /**< pin misc control register, offset: 0x81C */
  __IO uint32_t PIO_WAKEUP_LVL0;                   /**< pin wakeup polarity register 0, offset: 0x820 */
  __IO uint32_t PIO_WAKEUP_LVL1;                   /**< pin wakeup polarity register 1, offset: 0x824 */
  __IO uint32_t PIO_IE_CFG0;                       /**< pad input enable register 0, offset: 0x828 */
  __IO uint32_t PIO_IE_CFG1;                       /**< pad input enable register 1, offset: 0x82C */
  __IO uint32_t PIO_FUNC_CFG[4];                   /**< pin mux control register 0..pin mux control register 3, array offset: 0x830, array step: 0x4 */
  __IO uint32_t PIO_WAKEUP_EN0;                    /**< pin function selection in power down mode register 0, offset: 0x840 */
  __IO uint32_t PIO_WAKEUP_EN1;                    /**< pin function selection in power down mode register 1, offset: 0x844 */
  __I  uint32_t PIO_CAP_OE0;                       /**< pin output enable status register 0 while captured by writing 1 to IO_CAP, offset: 0x848 */
  __I  uint32_t PIO_CAP_OE1;                       /**< pin output enable status register 1 while captured by writing 1 to IO_CAP, offset: 0x84C */
  __I  uint32_t PIO_CAP_OUT0;                      /**< pin output status register 0 while captured by writing 1 to IO_CAP, offset: 0x850 */
  __I  uint32_t PIO_CAP_OUT1;                      /**< pin output status register 0 while captured by writing 1 to IO_CAP, offset: 0x854 */
  __IO uint32_t RST_CAUSE_SRC;                     /**< reset source status register, offset: 0x858 */
  __IO uint32_t PMU_CTRL0;                         /**< power management uinit control register 0, offset: 0x85C */
  __IO uint32_t PMU_CTRL1;                         /**< power management uinit control register 1, offset: 0x860 */
  __IO uint32_t ANA_EN;                            /**< analog setting register, offset: 0x864 */
  __IO uint32_t XTAL32K_CTRL;                      /**< crystal 32K control register, offset: 0x868 */
  __IO uint32_t USB_CFG;                           /**< USB configuration register, offset: 0x86C */
       uint8_t RESERVED_6[16];
  __IO uint32_t PMU_CTRL2;                         /**< power management uinit control register 2, offset: 0x880 */
  __IO uint32_t ANA_CTRL1;                         /**< IVREF and DVREG setting register, offset: 0x884 */
       uint8_t RESERVED_7[8];
  __IO uint32_t MISC;                              /**< MISC register, offset: 0x890 */
} SYSCON_Type;

/* ----------------------------------------------------------------------------
   -- SYSCON Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SYSCON_Register_Masks SYSCON Register Masks
 * @{
 */

/*! @name RST_SW_SET - block software reset set register */
#define SYSCON_RST_SW_SET_SET_FC0_RST_MASK       (0x1U)
#define SYSCON_RST_SW_SET_SET_FC0_RST_SHIFT      (0U)
#define SYSCON_RST_SW_SET_SET_FC0_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_FC0_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_FC0_RST_MASK)
#define SYSCON_RST_SW_SET_SET_FC1_RST_MASK       (0x2U)
#define SYSCON_RST_SW_SET_SET_FC1_RST_SHIFT      (1U)
#define SYSCON_RST_SW_SET_SET_FC1_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_FC1_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_FC1_RST_MASK)
#define SYSCON_RST_SW_SET_SET_FC2_RST_MASK       (0x4U)
#define SYSCON_RST_SW_SET_SET_FC2_RST_SHIFT      (2U)
#define SYSCON_RST_SW_SET_SET_FC2_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_FC2_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_FC2_RST_MASK)
#define SYSCON_RST_SW_SET_SET_FC3_RST_MASK       (0x8U)
#define SYSCON_RST_SW_SET_SET_FC3_RST_SHIFT      (3U)
#define SYSCON_RST_SW_SET_SET_FC3_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_FC3_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_FC3_RST_MASK)
#define SYSCON_RST_SW_SET_SET_TIM0_RST_MASK      (0x10U)
#define SYSCON_RST_SW_SET_SET_TIM0_RST_SHIFT     (4U)
#define SYSCON_RST_SW_SET_SET_TIM0_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_TIM0_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_TIM0_RST_MASK)
#define SYSCON_RST_SW_SET_SET_TIM1_RST_MASK      (0x20U)
#define SYSCON_RST_SW_SET_SET_TIM1_RST_SHIFT     (5U)
#define SYSCON_RST_SW_SET_SET_TIM1_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_TIM1_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_TIM1_RST_MASK)
#define SYSCON_RST_SW_SET_SET_TIM2_RST_MASK      (0x40U)
#define SYSCON_RST_SW_SET_SET_TIM2_RST_SHIFT     (6U)
#define SYSCON_RST_SW_SET_SET_TIM2_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_TIM2_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_TIM2_RST_MASK)
#define SYSCON_RST_SW_SET_SET_TIM3_RST_MASK      (0x80U)
#define SYSCON_RST_SW_SET_SET_TIM3_RST_SHIFT     (7U)
#define SYSCON_RST_SW_SET_SET_TIM3_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_TIM3_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_TIM3_RST_MASK)
#define SYSCON_RST_SW_SET_SET_SCT_RST_MASK       (0x100U)
#define SYSCON_RST_SW_SET_SET_SCT_RST_SHIFT      (8U)
#define SYSCON_RST_SW_SET_SET_SCT_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_SCT_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_SCT_RST_MASK)
#define SYSCON_RST_SW_SET_SET_WDT_RST_MASK       (0x200U)
#define SYSCON_RST_SW_SET_SET_WDT_RST_SHIFT      (9U)
#define SYSCON_RST_SW_SET_SET_WDT_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_WDT_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_WDT_RST_MASK)
#define SYSCON_RST_SW_SET_SET_USB_RST_MASK       (0x400U)
#define SYSCON_RST_SW_SET_SET_USB_RST_SHIFT      (10U)
#define SYSCON_RST_SW_SET_SET_USB_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_USB_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_USB_RST_MASK)
#define SYSCON_RST_SW_SET_SET_GPIO_RST_MASK      (0x800U)
#define SYSCON_RST_SW_SET_SET_GPIO_RST_SHIFT     (11U)
#define SYSCON_RST_SW_SET_SET_GPIO_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_GPIO_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_GPIO_RST_MASK)
#define SYSCON_RST_SW_SET_SET_RTC_RST_MASK       (0x1000U)
#define SYSCON_RST_SW_SET_SET_RTC_RST_SHIFT      (12U)
#define SYSCON_RST_SW_SET_SET_RTC_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_RTC_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_RTC_RST_MASK)
#define SYSCON_RST_SW_SET_SET_ADC_RST_MASK       (0x2000U)
#define SYSCON_RST_SW_SET_SET_ADC_RST_SHIFT      (13U)
#define SYSCON_RST_SW_SET_SET_ADC_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_ADC_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_ADC_RST_MASK)
#define SYSCON_RST_SW_SET_SET_DAC_RST_MASK       (0x4000U)
#define SYSCON_RST_SW_SET_SET_DAC_RST_SHIFT      (14U)
#define SYSCON_RST_SW_SET_SET_DAC_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_DAC_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_DAC_RST_MASK)
#define SYSCON_RST_SW_SET_SET_CS_RST_MASK        (0x8000U)
#define SYSCON_RST_SW_SET_SET_CS_RST_SHIFT       (15U)
#define SYSCON_RST_SW_SET_SET_CS_RST(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_CS_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_CS_RST_MASK)
#define SYSCON_RST_SW_SET_SET_FSP_RST_MASK       (0x10000U)
#define SYSCON_RST_SW_SET_SET_FSP_RST_SHIFT      (16U)
#define SYSCON_RST_SW_SET_SET_FSP_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_FSP_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_FSP_RST_MASK)
#define SYSCON_RST_SW_SET_SET_DMA_RST_MASK       (0x20000U)
#define SYSCON_RST_SW_SET_SET_DMA_RST_SHIFT      (17U)
#define SYSCON_RST_SW_SET_SET_DMA_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_DMA_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_DMA_RST_MASK)
#define SYSCON_RST_SW_SET_SET_QDEC0_RST_MASK     (0x80000U)
#define SYSCON_RST_SW_SET_SET_QDEC0_RST_SHIFT    (19U)
#define SYSCON_RST_SW_SET_SET_QDEC0_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_QDEC0_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_QDEC0_RST_MASK)
#define SYSCON_RST_SW_SET_SET_QDEC1_RST_MASK     (0x100000U)
#define SYSCON_RST_SW_SET_SET_QDEC1_RST_SHIFT    (20U)
#define SYSCON_RST_SW_SET_SET_QDEC1_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_QDEC1_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_QDEC1_RST_MASK)
#define SYSCON_RST_SW_SET_SET_SPIFI_RST_MASK     (0x400000U)
#define SYSCON_RST_SW_SET_SET_SPIFI_RST_SHIFT    (22U)
#define SYSCON_RST_SW_SET_SET_SPIFI_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_SPIFI_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_SPIFI_RST_MASK)
#define SYSCON_RST_SW_SET_SET_CPU_RST_MASK       (0x4000000U)
#define SYSCON_RST_SW_SET_SET_CPU_RST_SHIFT      (26U)
#define SYSCON_RST_SW_SET_SET_CPU_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_CPU_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_CPU_RST_MASK)
#define SYSCON_RST_SW_SET_SET_BLE_RST_MASK       (0x8000000U)
#define SYSCON_RST_SW_SET_SET_BLE_RST_SHIFT      (27U)
#define SYSCON_RST_SW_SET_SET_BLE_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_BLE_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_BLE_RST_MASK)
#define SYSCON_RST_SW_SET_SET_FLASH_RST_MASK     (0x10000000U)
#define SYSCON_RST_SW_SET_SET_FLASH_RST_SHIFT    (28U)
#define SYSCON_RST_SW_SET_SET_FLASH_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_FLASH_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_FLASH_RST_MASK)
#define SYSCON_RST_SW_SET_SET_DP_RST_MASK        (0x20000000U)
#define SYSCON_RST_SW_SET_SET_DP_RST_SHIFT       (29U)
#define SYSCON_RST_SW_SET_SET_DP_RST(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_DP_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_DP_RST_MASK)
#define SYSCON_RST_SW_SET_SET_REG_RST_MASK       (0x40000000U)
#define SYSCON_RST_SW_SET_SET_REG_RST_SHIFT      (30U)
#define SYSCON_RST_SW_SET_SET_REG_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_REG_RST_SHIFT)) & SYSCON_RST_SW_SET_SET_REG_RST_MASK)
#define SYSCON_RST_SW_SET_SET_REBOOT_MASK        (0x80000000U)
#define SYSCON_RST_SW_SET_SET_REBOOT_SHIFT       (31U)
#define SYSCON_RST_SW_SET_SET_REBOOT(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_SET_SET_REBOOT_SHIFT)) & SYSCON_RST_SW_SET_SET_REBOOT_MASK)

/*! @name RST_SW_CLR - block software reset clear register */
#define SYSCON_RST_SW_CLR_CLR_FC0_RST_MASK       (0x1U)
#define SYSCON_RST_SW_CLR_CLR_FC0_RST_SHIFT      (0U)
#define SYSCON_RST_SW_CLR_CLR_FC0_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_FC0_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_FC0_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_FC1_RST_MASK       (0x2U)
#define SYSCON_RST_SW_CLR_CLR_FC1_RST_SHIFT      (1U)
#define SYSCON_RST_SW_CLR_CLR_FC1_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_FC1_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_FC1_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_FC2_RST_MASK       (0x4U)
#define SYSCON_RST_SW_CLR_CLR_FC2_RST_SHIFT      (2U)
#define SYSCON_RST_SW_CLR_CLR_FC2_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_FC2_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_FC2_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_FC3_RST_MASK       (0x8U)
#define SYSCON_RST_SW_CLR_CLR_FC3_RST_SHIFT      (3U)
#define SYSCON_RST_SW_CLR_CLR_FC3_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_FC3_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_FC3_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_TIM0_RST_MASK      (0x10U)
#define SYSCON_RST_SW_CLR_CLR_TIM0_RST_SHIFT     (4U)
#define SYSCON_RST_SW_CLR_CLR_TIM0_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_TIM0_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_TIM0_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_TIM1_RST_MASK      (0x20U)
#define SYSCON_RST_SW_CLR_CLR_TIM1_RST_SHIFT     (5U)
#define SYSCON_RST_SW_CLR_CLR_TIM1_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_TIM1_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_TIM1_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_TIM2_RST_MASK      (0x40U)
#define SYSCON_RST_SW_CLR_CLR_TIM2_RST_SHIFT     (6U)
#define SYSCON_RST_SW_CLR_CLR_TIM2_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_TIM2_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_TIM2_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_TIM3_RST_MASK      (0x80U)
#define SYSCON_RST_SW_CLR_CLR_TIM3_RST_SHIFT     (7U)
#define SYSCON_RST_SW_CLR_CLR_TIM3_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_TIM3_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_TIM3_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_SCT_RST_MASK       (0x100U)
#define SYSCON_RST_SW_CLR_CLR_SCT_RST_SHIFT      (8U)
#define SYSCON_RST_SW_CLR_CLR_SCT_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_SCT_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_SCT_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_WDT_RST_MASK       (0x200U)
#define SYSCON_RST_SW_CLR_CLR_WDT_RST_SHIFT      (9U)
#define SYSCON_RST_SW_CLR_CLR_WDT_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_WDT_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_WDT_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_USB_RST_MASK       (0x400U)
#define SYSCON_RST_SW_CLR_CLR_USB_RST_SHIFT      (10U)
#define SYSCON_RST_SW_CLR_CLR_USB_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_USB_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_USB_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_GPIO_RST_MASK      (0x800U)
#define SYSCON_RST_SW_CLR_CLR_GPIO_RST_SHIFT     (11U)
#define SYSCON_RST_SW_CLR_CLR_GPIO_RST(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_GPIO_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_GPIO_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_RTC_RST_MASK       (0x1000U)
#define SYSCON_RST_SW_CLR_CLR_RTC_RST_SHIFT      (12U)
#define SYSCON_RST_SW_CLR_CLR_RTC_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_RTC_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_RTC_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_ADC_RST_MASK       (0x2000U)
#define SYSCON_RST_SW_CLR_CLR_ADC_RST_SHIFT      (13U)
#define SYSCON_RST_SW_CLR_CLR_ADC_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_ADC_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_ADC_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_DAC_RST_MASK       (0x4000U)
#define SYSCON_RST_SW_CLR_CLR_DAC_RST_SHIFT      (14U)
#define SYSCON_RST_SW_CLR_CLR_DAC_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_DAC_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_DAC_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_CS_RST_MASK        (0x8000U)
#define SYSCON_RST_SW_CLR_CLR_CS_RST_SHIFT       (15U)
#define SYSCON_RST_SW_CLR_CLR_CS_RST(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_CS_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_CS_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_FSP_RST_MASK       (0x10000U)
#define SYSCON_RST_SW_CLR_CLR_FSP_RST_SHIFT      (16U)
#define SYSCON_RST_SW_CLR_CLR_FSP_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_FSP_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_FSP_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_DMA_RST_MASK       (0x20000U)
#define SYSCON_RST_SW_CLR_CLR_DMA_RST_SHIFT      (17U)
#define SYSCON_RST_SW_CLR_CLR_DMA_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_DMA_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_DMA_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_QDEC0_RST_MASK     (0x80000U)
#define SYSCON_RST_SW_CLR_CLR_QDEC0_RST_SHIFT    (19U)
#define SYSCON_RST_SW_CLR_CLR_QDEC0_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_QDEC0_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_QDEC0_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_QDEC1_RST_MASK     (0x100000U)
#define SYSCON_RST_SW_CLR_CLR_QDEC1_RST_SHIFT    (20U)
#define SYSCON_RST_SW_CLR_CLR_QDEC1_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_QDEC1_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_QDEC1_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_SPIFI_RST_MASK     (0x400000U)
#define SYSCON_RST_SW_CLR_CLR_SPIFI_RST_SHIFT    (22U)
#define SYSCON_RST_SW_CLR_CLR_SPIFI_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_SPIFI_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_SPIFI_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_CPU_RST_MASK       (0x4000000U)
#define SYSCON_RST_SW_CLR_CLR_CPU_RST_SHIFT      (26U)
#define SYSCON_RST_SW_CLR_CLR_CPU_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_CPU_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_CPU_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_BLE_RST_MASK       (0x8000000U)
#define SYSCON_RST_SW_CLR_CLR_BLE_RST_SHIFT      (27U)
#define SYSCON_RST_SW_CLR_CLR_BLE_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_BLE_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_BLE_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_FLASH_RST_MASK     (0x10000000U)
#define SYSCON_RST_SW_CLR_CLR_FLASH_RST_SHIFT    (28U)
#define SYSCON_RST_SW_CLR_CLR_FLASH_RST(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_FLASH_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_FLASH_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_DP_RST_MASK        (0x20000000U)
#define SYSCON_RST_SW_CLR_CLR_DP_RST_SHIFT       (29U)
#define SYSCON_RST_SW_CLR_CLR_DP_RST(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_DP_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_DP_RST_MASK)
#define SYSCON_RST_SW_CLR_CLR_REG_RST_MASK       (0x40000000U)
#define SYSCON_RST_SW_CLR_CLR_REG_RST_SHIFT      (30U)
#define SYSCON_RST_SW_CLR_CLR_REG_RST(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_SW_CLR_CLR_REG_RST_SHIFT)) & SYSCON_RST_SW_CLR_CLR_REG_RST_MASK)

/*! @name CLK_DIS - clock disable register */
#define SYSCON_CLK_DIS_CLK_FC0_DIS_MASK          (0x1U)
#define SYSCON_CLK_DIS_CLK_FC0_DIS_SHIFT         (0U)
#define SYSCON_CLK_DIS_CLK_FC0_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_FC0_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_FC0_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_FC1_DIS_MASK          (0x2U)
#define SYSCON_CLK_DIS_CLK_FC1_DIS_SHIFT         (1U)
#define SYSCON_CLK_DIS_CLK_FC1_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_FC1_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_FC1_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_FC2_DIS_MASK          (0x4U)
#define SYSCON_CLK_DIS_CLK_FC2_DIS_SHIFT         (2U)
#define SYSCON_CLK_DIS_CLK_FC2_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_FC2_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_FC2_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_FC3_DIS_MASK          (0x8U)
#define SYSCON_CLK_DIS_CLK_FC3_DIS_SHIFT         (3U)
#define SYSCON_CLK_DIS_CLK_FC3_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_FC3_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_FC3_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_TIM0_DIS_MASK         (0x10U)
#define SYSCON_CLK_DIS_CLK_TIM0_DIS_SHIFT        (4U)
#define SYSCON_CLK_DIS_CLK_TIM0_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_TIM0_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_TIM0_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_TIM1_DIS_MASK         (0x20U)
#define SYSCON_CLK_DIS_CLK_TIM1_DIS_SHIFT        (5U)
#define SYSCON_CLK_DIS_CLK_TIM1_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_TIM1_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_TIM1_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_TIM2_DIS_MASK         (0x40U)
#define SYSCON_CLK_DIS_CLK_TIM2_DIS_SHIFT        (6U)
#define SYSCON_CLK_DIS_CLK_TIM2_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_TIM2_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_TIM2_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_TIM3_DIS_MASK         (0x80U)
#define SYSCON_CLK_DIS_CLK_TIM3_DIS_SHIFT        (7U)
#define SYSCON_CLK_DIS_CLK_TIM3_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_TIM3_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_TIM3_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_SCT_DIS_MASK          (0x100U)
#define SYSCON_CLK_DIS_CLK_SCT_DIS_SHIFT         (8U)
#define SYSCON_CLK_DIS_CLK_SCT_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_SCT_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_SCT_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_WDT_DIS_MASK          (0x200U)
#define SYSCON_CLK_DIS_CLK_WDT_DIS_SHIFT         (9U)
#define SYSCON_CLK_DIS_CLK_WDT_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_WDT_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_WDT_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_USB_DIS_MASK          (0x400U)
#define SYSCON_CLK_DIS_CLK_USB_DIS_SHIFT         (10U)
#define SYSCON_CLK_DIS_CLK_USB_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_USB_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_USB_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_GPIO_DIS_MASK         (0x800U)
#define SYSCON_CLK_DIS_CLK_GPIO_DIS_SHIFT        (11U)
#define SYSCON_CLK_DIS_CLK_GPIO_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_GPIO_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_GPIO_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_BIV_DIS_MASK          (0x1000U)
#define SYSCON_CLK_DIS_CLK_BIV_DIS_SHIFT         (12U)
#define SYSCON_CLK_DIS_CLK_BIV_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_BIV_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_BIV_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_ADC_DIS_MASK          (0x2000U)
#define SYSCON_CLK_DIS_CLK_ADC_DIS_SHIFT         (13U)
#define SYSCON_CLK_DIS_CLK_ADC_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_ADC_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_ADC_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_DAC_DIS_MASK          (0x4000U)
#define SYSCON_CLK_DIS_CLK_DAC_DIS_SHIFT         (14U)
#define SYSCON_CLK_DIS_CLK_DAC_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_DAC_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_DAC_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_CS_DIS_MASK           (0x8000U)
#define SYSCON_CLK_DIS_CLK_CS_DIS_SHIFT          (15U)
#define SYSCON_CLK_DIS_CLK_CS_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_CS_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_CS_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_FSP_DIS_MASK          (0x10000U)
#define SYSCON_CLK_DIS_CLK_FSP_DIS_SHIFT         (16U)
#define SYSCON_CLK_DIS_CLK_FSP_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_FSP_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_FSP_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_DMA_DIS_MASK          (0x20000U)
#define SYSCON_CLK_DIS_CLK_DMA_DIS_SHIFT         (17U)
#define SYSCON_CLK_DIS_CLK_DMA_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_DMA_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_DMA_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_QDEC0_DIS_MASK        (0x80000U)
#define SYSCON_CLK_DIS_CLK_QDEC0_DIS_SHIFT       (19U)
#define SYSCON_CLK_DIS_CLK_QDEC0_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_QDEC0_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_QDEC0_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_QDEC1_DIS_MASK        (0x100000U)
#define SYSCON_CLK_DIS_CLK_QDEC1_DIS_SHIFT       (20U)
#define SYSCON_CLK_DIS_CLK_QDEC1_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_QDEC1_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_QDEC1_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_DP_DIS_MASK           (0x200000U)
#define SYSCON_CLK_DIS_CLK_DP_DIS_SHIFT          (21U)
#define SYSCON_CLK_DIS_CLK_DP_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_DP_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_DP_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_SPIFI_DIS_MASK        (0x400000U)
#define SYSCON_CLK_DIS_CLK_SPIFI_DIS_SHIFT       (22U)
#define SYSCON_CLK_DIS_CLK_SPIFI_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_SPIFI_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_SPIFI_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_CAL_DIS_MASK          (0x2000000U)
#define SYSCON_CLK_DIS_CLK_CAL_DIS_SHIFT         (25U)
#define SYSCON_CLK_DIS_CLK_CAL_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_CAL_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_CAL_DIS_MASK)
#define SYSCON_CLK_DIS_CLK_BLE_DIS_MASK          (0x8000000U)
#define SYSCON_CLK_DIS_CLK_BLE_DIS_SHIFT         (27U)
#define SYSCON_CLK_DIS_CLK_BLE_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_CLK_BLE_DIS_SHIFT)) & SYSCON_CLK_DIS_CLK_BLE_DIS_MASK)
#define SYSCON_CLK_DIS_PCLK_DIS_MASK             (0x40000000U)
#define SYSCON_CLK_DIS_PCLK_DIS_SHIFT            (30U)
#define SYSCON_CLK_DIS_PCLK_DIS(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_PCLK_DIS_SHIFT)) & SYSCON_CLK_DIS_PCLK_DIS_MASK)
#define SYSCON_CLK_DIS_FCLK_DIS_MASK             (0x80000000U)
#define SYSCON_CLK_DIS_FCLK_DIS_SHIFT            (31U)
#define SYSCON_CLK_DIS_FCLK_DIS(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_DIS_FCLK_DIS_SHIFT)) & SYSCON_CLK_DIS_FCLK_DIS_MASK)

/*! @name CLK_EN - clock enable register */
#define SYSCON_CLK_EN_CLK_FC0_EN_MASK            (0x1U)
#define SYSCON_CLK_EN_CLK_FC0_EN_SHIFT           (0U)
#define SYSCON_CLK_EN_CLK_FC0_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_FC0_EN_SHIFT)) & SYSCON_CLK_EN_CLK_FC0_EN_MASK)
#define SYSCON_CLK_EN_CLK_FC1_EN_MASK            (0x2U)
#define SYSCON_CLK_EN_CLK_FC1_EN_SHIFT           (1U)
#define SYSCON_CLK_EN_CLK_FC1_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_FC1_EN_SHIFT)) & SYSCON_CLK_EN_CLK_FC1_EN_MASK)
#define SYSCON_CLK_EN_CLK_FC2_EN_MASK            (0x4U)
#define SYSCON_CLK_EN_CLK_FC2_EN_SHIFT           (2U)
#define SYSCON_CLK_EN_CLK_FC2_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_FC2_EN_SHIFT)) & SYSCON_CLK_EN_CLK_FC2_EN_MASK)
#define SYSCON_CLK_EN_CLK_FC3_EN_MASK            (0x8U)
#define SYSCON_CLK_EN_CLK_FC3_EN_SHIFT           (3U)
#define SYSCON_CLK_EN_CLK_FC3_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_FC3_EN_SHIFT)) & SYSCON_CLK_EN_CLK_FC3_EN_MASK)
#define SYSCON_CLK_EN_CLK_TIM0_EN_MASK           (0x10U)
#define SYSCON_CLK_EN_CLK_TIM0_EN_SHIFT          (4U)
#define SYSCON_CLK_EN_CLK_TIM0_EN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_TIM0_EN_SHIFT)) & SYSCON_CLK_EN_CLK_TIM0_EN_MASK)
#define SYSCON_CLK_EN_CLK_TIM1_EN_MASK           (0x20U)
#define SYSCON_CLK_EN_CLK_TIM1_EN_SHIFT          (5U)
#define SYSCON_CLK_EN_CLK_TIM1_EN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_TIM1_EN_SHIFT)) & SYSCON_CLK_EN_CLK_TIM1_EN_MASK)
#define SYSCON_CLK_EN_CLK_TIM2_EN_MASK           (0x40U)
#define SYSCON_CLK_EN_CLK_TIM2_EN_SHIFT          (6U)
#define SYSCON_CLK_EN_CLK_TIM2_EN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_TIM2_EN_SHIFT)) & SYSCON_CLK_EN_CLK_TIM2_EN_MASK)
#define SYSCON_CLK_EN_CLK_TIM3_EN_MASK           (0x80U)
#define SYSCON_CLK_EN_CLK_TIM3_EN_SHIFT          (7U)
#define SYSCON_CLK_EN_CLK_TIM3_EN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_TIM3_EN_SHIFT)) & SYSCON_CLK_EN_CLK_TIM3_EN_MASK)
#define SYSCON_CLK_EN_CLK_SCT_EN_MASK            (0x100U)
#define SYSCON_CLK_EN_CLK_SCT_EN_SHIFT           (8U)
#define SYSCON_CLK_EN_CLK_SCT_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_SCT_EN_SHIFT)) & SYSCON_CLK_EN_CLK_SCT_EN_MASK)
#define SYSCON_CLK_EN_CLK_WDT_EN_MASK            (0x200U)
#define SYSCON_CLK_EN_CLK_WDT_EN_SHIFT           (9U)
#define SYSCON_CLK_EN_CLK_WDT_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_WDT_EN_SHIFT)) & SYSCON_CLK_EN_CLK_WDT_EN_MASK)
#define SYSCON_CLK_EN_CLK_USB_EN_MASK            (0x400U)
#define SYSCON_CLK_EN_CLK_USB_EN_SHIFT           (10U)
#define SYSCON_CLK_EN_CLK_USB_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_USB_EN_SHIFT)) & SYSCON_CLK_EN_CLK_USB_EN_MASK)
#define SYSCON_CLK_EN_CLK_GPIO_EN_MASK           (0x800U)
#define SYSCON_CLK_EN_CLK_GPIO_EN_SHIFT          (11U)
#define SYSCON_CLK_EN_CLK_GPIO_EN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_GPIO_EN_SHIFT)) & SYSCON_CLK_EN_CLK_GPIO_EN_MASK)
#define SYSCON_CLK_EN_CLK_BIV_EN_MASK            (0x1000U)
#define SYSCON_CLK_EN_CLK_BIV_EN_SHIFT           (12U)
#define SYSCON_CLK_EN_CLK_BIV_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_BIV_EN_SHIFT)) & SYSCON_CLK_EN_CLK_BIV_EN_MASK)
#define SYSCON_CLK_EN_CLK_ADC_EN_MASK            (0x2000U)
#define SYSCON_CLK_EN_CLK_ADC_EN_SHIFT           (13U)
#define SYSCON_CLK_EN_CLK_ADC_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_ADC_EN_SHIFT)) & SYSCON_CLK_EN_CLK_ADC_EN_MASK)
#define SYSCON_CLK_EN_CLK_DAC_EN_MASK            (0x4000U)
#define SYSCON_CLK_EN_CLK_DAC_EN_SHIFT           (14U)
#define SYSCON_CLK_EN_CLK_DAC_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_DAC_EN_SHIFT)) & SYSCON_CLK_EN_CLK_DAC_EN_MASK)
#define SYSCON_CLK_EN_CLK_CS_EN_MASK             (0x8000U)
#define SYSCON_CLK_EN_CLK_CS_EN_SHIFT            (15U)
#define SYSCON_CLK_EN_CLK_CS_EN(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_CS_EN_SHIFT)) & SYSCON_CLK_EN_CLK_CS_EN_MASK)
#define SYSCON_CLK_EN_CLK_FSP_EN_MASK            (0x10000U)
#define SYSCON_CLK_EN_CLK_FSP_EN_SHIFT           (16U)
#define SYSCON_CLK_EN_CLK_FSP_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_FSP_EN_SHIFT)) & SYSCON_CLK_EN_CLK_FSP_EN_MASK)
#define SYSCON_CLK_EN_CLK_DMA_EN_MASK            (0x20000U)
#define SYSCON_CLK_EN_CLK_DMA_EN_SHIFT           (17U)
#define SYSCON_CLK_EN_CLK_DMA_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_DMA_EN_SHIFT)) & SYSCON_CLK_EN_CLK_DMA_EN_MASK)
#define SYSCON_CLK_EN_CLK_QDEC0_EN_MASK          (0x80000U)
#define SYSCON_CLK_EN_CLK_QDEC0_EN_SHIFT         (19U)
#define SYSCON_CLK_EN_CLK_QDEC0_EN(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_QDEC0_EN_SHIFT)) & SYSCON_CLK_EN_CLK_QDEC0_EN_MASK)
#define SYSCON_CLK_EN_CLK_QDEC1_EN_MASK          (0x100000U)
#define SYSCON_CLK_EN_CLK_QDEC1_EN_SHIFT         (20U)
#define SYSCON_CLK_EN_CLK_QDEC1_EN(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_QDEC1_EN_SHIFT)) & SYSCON_CLK_EN_CLK_QDEC1_EN_MASK)
#define SYSCON_CLK_EN_CLK_DP_EN_MASK             (0x200000U)
#define SYSCON_CLK_EN_CLK_DP_EN_SHIFT            (21U)
#define SYSCON_CLK_EN_CLK_DP_EN(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_DP_EN_SHIFT)) & SYSCON_CLK_EN_CLK_DP_EN_MASK)
#define SYSCON_CLK_EN_CLK_SPIFI_EN_MASK          (0x400000U)
#define SYSCON_CLK_EN_CLK_SPIFI_EN_SHIFT         (22U)
#define SYSCON_CLK_EN_CLK_SPIFI_EN(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_SPIFI_EN_SHIFT)) & SYSCON_CLK_EN_CLK_SPIFI_EN_MASK)
#define SYSCON_CLK_EN_CLK_CAL_EN_MASK            (0x2000000U)
#define SYSCON_CLK_EN_CLK_CAL_EN_SHIFT           (25U)
#define SYSCON_CLK_EN_CLK_CAL_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_CAL_EN_SHIFT)) & SYSCON_CLK_EN_CLK_CAL_EN_MASK)
#define SYSCON_CLK_EN_CLK_BLE_EN_MASK            (0x8000000U)
#define SYSCON_CLK_EN_CLK_BLE_EN_SHIFT           (27U)
#define SYSCON_CLK_EN_CLK_BLE_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_EN_CLK_BLE_EN_SHIFT)) & SYSCON_CLK_EN_CLK_BLE_EN_MASK)

/*! @name CLK_CTRL - system clock source and divider register */
#define SYSCON_CLK_CTRL_APB_DIV_MASK             (0xFU)
#define SYSCON_CLK_CTRL_APB_DIV_SHIFT            (0U)
#define SYSCON_CLK_CTRL_APB_DIV(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_APB_DIV_SHIFT)) & SYSCON_CLK_CTRL_APB_DIV_MASK)
#define SYSCON_CLK_CTRL_AHB_DIV_MASK             (0x1FFF0U)
#define SYSCON_CLK_CTRL_AHB_DIV_SHIFT            (4U)
#define SYSCON_CLK_CTRL_AHB_DIV(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_AHB_DIV_SHIFT)) & SYSCON_CLK_CTRL_AHB_DIV_MASK)
#define SYSCON_CLK_CTRL_CLK_BLE_SEL_MASK         (0x20000U)
#define SYSCON_CLK_CTRL_CLK_BLE_SEL_SHIFT        (17U)
#define SYSCON_CLK_CTRL_CLK_BLE_SEL(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_BLE_SEL_SHIFT)) & SYSCON_CLK_CTRL_CLK_BLE_SEL_MASK)
#define SYSCON_CLK_CTRL_CLK_WDT_SEL_MASK         (0x40000U)
#define SYSCON_CLK_CTRL_CLK_WDT_SEL_SHIFT        (18U)
#define SYSCON_CLK_CTRL_CLK_WDT_SEL(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_WDT_SEL_SHIFT)) & SYSCON_CLK_CTRL_CLK_WDT_SEL_MASK)
#define SYSCON_CLK_CTRL_CLK_XTAL_SEL_MASK        (0x80000U)
#define SYSCON_CLK_CTRL_CLK_XTAL_SEL_SHIFT       (19U)
#define SYSCON_CLK_CTRL_CLK_XTAL_SEL(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_XTAL_SEL_SHIFT)) & SYSCON_CLK_CTRL_CLK_XTAL_SEL_MASK)
#define SYSCON_CLK_CTRL_CLK_OSC32M_DIV_MASK      (0x100000U)
#define SYSCON_CLK_CTRL_CLK_OSC32M_DIV_SHIFT     (20U)
#define SYSCON_CLK_CTRL_CLK_OSC32M_DIV(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_OSC32M_DIV_SHIFT)) & SYSCON_CLK_CTRL_CLK_OSC32M_DIV_MASK)
#define SYSCON_CLK_CTRL_CLK_32K_SEL_MASK         (0x200000U)
#define SYSCON_CLK_CTRL_CLK_32K_SEL_SHIFT        (21U)
#define SYSCON_CLK_CTRL_CLK_32K_SEL(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_32K_SEL_SHIFT)) & SYSCON_CLK_CTRL_CLK_32K_SEL_MASK)
#define SYSCON_CLK_CTRL_CLK_XTAL_OE_MASK         (0x400000U)
#define SYSCON_CLK_CTRL_CLK_XTAL_OE_SHIFT        (22U)
#define SYSCON_CLK_CTRL_CLK_XTAL_OE(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_XTAL_OE_SHIFT)) & SYSCON_CLK_CTRL_CLK_XTAL_OE_MASK)
#define SYSCON_CLK_CTRL_CLK_32K_OE_MASK          (0x800000U)
#define SYSCON_CLK_CTRL_CLK_32K_OE_SHIFT         (23U)
#define SYSCON_CLK_CTRL_CLK_32K_OE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CLK_32K_OE_SHIFT)) & SYSCON_CLK_CTRL_CLK_32K_OE_MASK)
#define SYSCON_CLK_CTRL_XTAL_OUT_DIV_MASK        (0xF000000U)
#define SYSCON_CLK_CTRL_XTAL_OUT_DIV_SHIFT       (24U)
#define SYSCON_CLK_CTRL_XTAL_OUT_DIV(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_XTAL_OUT_DIV_SHIFT)) & SYSCON_CLK_CTRL_XTAL_OUT_DIV_MASK)
#define SYSCON_CLK_CTRL_CGBYPASS_MASK            (0x10000000U)
#define SYSCON_CLK_CTRL_CGBYPASS_SHIFT           (28U)
#define SYSCON_CLK_CTRL_CGBYPASS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_CGBYPASS_SHIFT)) & SYSCON_CLK_CTRL_CGBYPASS_MASK)
#define SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK         (0xC0000000U)
#define SYSCON_CLK_CTRL_SYS_CLK_SEL_SHIFT        (30U)
#define SYSCON_CLK_CTRL_SYS_CLK_SEL(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CLK_CTRL_SYS_CLK_SEL_SHIFT)) & SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK)

/*! @name SYS_MODE_CTRL - system mode and address remap register */
#define SYSCON_SYS_MODE_CTRL_REMAP_MASK          (0x3U)
#define SYSCON_SYS_MODE_CTRL_REMAP_SHIFT         (0U)
#define SYSCON_SYS_MODE_CTRL_REMAP(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_REMAP_SHIFT)) & SYSCON_SYS_MODE_CTRL_REMAP_MASK)
#define SYSCON_SYS_MODE_CTRL_LOCKUP_EN_MASK      (0x4U)
#define SYSCON_SYS_MODE_CTRL_LOCKUP_EN_SHIFT     (2U)
#define SYSCON_SYS_MODE_CTRL_LOCKUP_EN(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_LOCKUP_EN_SHIFT)) & SYSCON_SYS_MODE_CTRL_LOCKUP_EN_MASK)
#define SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK       (0x2000000U)
#define SYSCON_SYS_MODE_CTRL_XTAL_RDY_SHIFT      (25U)
#define SYSCON_SYS_MODE_CTRL_XTAL_RDY(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_XTAL_RDY_SHIFT)) & SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK)
#define SYSCON_SYS_MODE_CTRL_XTAL32K_RDY_MASK    (0x4000000U)
#define SYSCON_SYS_MODE_CTRL_XTAL32K_RDY_SHIFT   (26U)
#define SYSCON_SYS_MODE_CTRL_XTAL32K_RDY(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_XTAL32K_RDY_SHIFT)) & SYSCON_SYS_MODE_CTRL_XTAL32K_RDY_MASK)
#define SYSCON_SYS_MODE_CTRL_PLL48M_RDY_MASK     (0x8000000U)
#define SYSCON_SYS_MODE_CTRL_PLL48M_RDY_SHIFT    (27U)
#define SYSCON_SYS_MODE_CTRL_PLL48M_RDY(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_PLL48M_RDY_SHIFT)) & SYSCON_SYS_MODE_CTRL_PLL48M_RDY_MASK)
#define SYSCON_SYS_MODE_CTRL_OSC32M_RDY_MASK     (0x10000000U)
#define SYSCON_SYS_MODE_CTRL_OSC32M_RDY_SHIFT    (28U)
#define SYSCON_SYS_MODE_CTRL_OSC32M_RDY(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_OSC32M_RDY_SHIFT)) & SYSCON_SYS_MODE_CTRL_OSC32M_RDY_MASK)
#define SYSCON_SYS_MODE_CTRL_BG_RDY_MASK         (0x20000000U)
#define SYSCON_SYS_MODE_CTRL_BG_RDY_SHIFT        (29U)
#define SYSCON_SYS_MODE_CTRL_BG_RDY(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_BG_RDY_SHIFT)) & SYSCON_SYS_MODE_CTRL_BG_RDY_MASK)
#define SYSCON_SYS_MODE_CTRL_BOOT_MODE_MASK      (0x80000000U)
#define SYSCON_SYS_MODE_CTRL_BOOT_MODE_SHIFT     (31U)
#define SYSCON_SYS_MODE_CTRL_BOOT_MODE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_MODE_CTRL_BOOT_MODE_SHIFT)) & SYSCON_SYS_MODE_CTRL_BOOT_MODE_MASK)

/*! @name SYS_STAT - system status register */
#define SYSCON_SYS_STAT_FREQ_WORD_MASK           (0xFFU)
#define SYSCON_SYS_STAT_FREQ_WORD_SHIFT          (0U)
#define SYSCON_SYS_STAT_FREQ_WORD(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_FREQ_WORD_SHIFT)) & SYSCON_SYS_STAT_FREQ_WORD_MASK)
#define SYSCON_SYS_STAT_BLE_FREQ_HOP_MASK        (0x100U)
#define SYSCON_SYS_STAT_BLE_FREQ_HOP_SHIFT       (8U)
#define SYSCON_SYS_STAT_BLE_FREQ_HOP(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_BLE_FREQ_HOP_SHIFT)) & SYSCON_SYS_STAT_BLE_FREQ_HOP_MASK)
#define SYSCON_SYS_STAT_EVENT_IN_PROCESS_MASK    (0x200U)
#define SYSCON_SYS_STAT_EVENT_IN_PROCESS_SHIFT   (9U)
#define SYSCON_SYS_STAT_EVENT_IN_PROCESS(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_EVENT_IN_PROCESS_SHIFT)) & SYSCON_SYS_STAT_EVENT_IN_PROCESS_MASK)
#define SYSCON_SYS_STAT_RX_EN_MASK               (0x400U)
#define SYSCON_SYS_STAT_RX_EN_SHIFT              (10U)
#define SYSCON_SYS_STAT_RX_EN(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_RX_EN_SHIFT)) & SYSCON_SYS_STAT_RX_EN_MASK)
#define SYSCON_SYS_STAT_TX_EN_MASK               (0x800U)
#define SYSCON_SYS_STAT_TX_EN_SHIFT              (11U)
#define SYSCON_SYS_STAT_TX_EN(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_TX_EN_SHIFT)) & SYSCON_SYS_STAT_TX_EN_MASK)
#define SYSCON_SYS_STAT_OSC_EN_MASK              (0x1000U)
#define SYSCON_SYS_STAT_OSC_EN_SHIFT             (12U)
#define SYSCON_SYS_STAT_OSC_EN(x)                (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_OSC_EN_SHIFT)) & SYSCON_SYS_STAT_OSC_EN_MASK)
#define SYSCON_SYS_STAT_RADIO_EN_MASK            (0x2000U)
#define SYSCON_SYS_STAT_RADIO_EN_SHIFT           (13U)
#define SYSCON_SYS_STAT_RADIO_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_RADIO_EN_SHIFT)) & SYSCON_SYS_STAT_RADIO_EN_MASK)
#define SYSCON_SYS_STAT_CLK_STATUS_MASK          (0x4000U)
#define SYSCON_SYS_STAT_CLK_STATUS_SHIFT         (14U)
#define SYSCON_SYS_STAT_CLK_STATUS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_STAT_CLK_STATUS_SHIFT)) & SYSCON_SYS_STAT_CLK_STATUS_MASK)

/*! @name SYS_TICK - systick timer control register */
#define SYSCON_SYS_TICK_TENMS_MASK               (0xFFFFFFU)
#define SYSCON_SYS_TICK_TENMS_SHIFT              (0U)
#define SYSCON_SYS_TICK_TENMS(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_TICK_TENMS_SHIFT)) & SYSCON_SYS_TICK_TENMS_MASK)
#define SYSCON_SYS_TICK_SKEW_MASK                (0x1000000U)
#define SYSCON_SYS_TICK_SKEW_SHIFT               (24U)
#define SYSCON_SYS_TICK_SKEW(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_TICK_SKEW_SHIFT)) & SYSCON_SYS_TICK_SKEW_MASK)
#define SYSCON_SYS_TICK_NOREF_MASK               (0x2000000U)
#define SYSCON_SYS_TICK_NOREF_SHIFT              (25U)
#define SYSCON_SYS_TICK_NOREF(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_TICK_NOREF_SHIFT)) & SYSCON_SYS_TICK_NOREF_MASK)
#define SYSCON_SYS_TICK_EN_STCLKEN_MASK          (0x80000000U)
#define SYSCON_SYS_TICK_EN_STCLKEN_SHIFT         (31U)
#define SYSCON_SYS_TICK_EN_STCLKEN(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_SYS_TICK_EN_STCLKEN_SHIFT)) & SYSCON_SYS_TICK_EN_STCLKEN_MASK)

/*! @name SRAM_CTRL - Exchange memory base address register */
#define SYSCON_SRAM_CTRL_EM_BASE_ADDR_MASK       (0x7FFFU)
#define SYSCON_SRAM_CTRL_EM_BASE_ADDR_SHIFT      (0U)
#define SYSCON_SRAM_CTRL_EM_BASE_ADDR(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_SRAM_CTRL_EM_BASE_ADDR_SHIFT)) & SYSCON_SRAM_CTRL_EM_BASE_ADDR_MASK)

/*! @name CHIP_ID - chip id register */
#define SYSCON_CHIP_ID_CID0_MASK                 (0x7U)
#define SYSCON_CHIP_ID_CID0_SHIFT                (0U)
#define SYSCON_CHIP_ID_CID0(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_CID0_SHIFT)) & SYSCON_CHIP_ID_CID0_MASK)
#define SYSCON_CHIP_ID_CID1_MASK                 (0x38U)
#define SYSCON_CHIP_ID_CID1_SHIFT                (3U)
#define SYSCON_CHIP_ID_CID1(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_CID1_SHIFT)) & SYSCON_CHIP_ID_CID1_MASK)
#define SYSCON_CHIP_ID_CID2_MASK                 (0xC0U)
#define SYSCON_CHIP_ID_CID2_SHIFT                (6U)
#define SYSCON_CHIP_ID_CID2(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_CID2_SHIFT)) & SYSCON_CHIP_ID_CID2_MASK)
#define SYSCON_CHIP_ID_CID3_MASK                 (0x3F00U)
#define SYSCON_CHIP_ID_CID3_SHIFT                (8U)
#define SYSCON_CHIP_ID_CID3(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_CID3_SHIFT)) & SYSCON_CHIP_ID_CID3_MASK)
#define SYSCON_CHIP_ID_CID4_MASK                 (0xC000U)
#define SYSCON_CHIP_ID_CID4_SHIFT                (14U)
#define SYSCON_CHIP_ID_CID4(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_CID4_SHIFT)) & SYSCON_CHIP_ID_CID4_MASK)
#define SYSCON_CHIP_ID_MEM_OPTION_MASK           (0x4000000U)
#define SYSCON_CHIP_ID_MEM_OPTION_SHIFT          (26U)
#define SYSCON_CHIP_ID_MEM_OPTION(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_MEM_OPTION_SHIFT)) & SYSCON_CHIP_ID_MEM_OPTION_MASK)
#define SYSCON_CHIP_ID_ADC_OPTION_MASK           (0x8000000U)
#define SYSCON_CHIP_ID_ADC_OPTION_SHIFT          (27U)
#define SYSCON_CHIP_ID_ADC_OPTION(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_ADC_OPTION_SHIFT)) & SYSCON_CHIP_ID_ADC_OPTION_MASK)
#define SYSCON_CHIP_ID_FLASH_OPTION_MASK         (0x10000000U)
#define SYSCON_CHIP_ID_FLASH_OPTION_SHIFT        (28U)
#define SYSCON_CHIP_ID_FLASH_OPTION(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_FLASH_OPTION_SHIFT)) & SYSCON_CHIP_ID_FLASH_OPTION_MASK)
#define SYSCON_CHIP_ID_FPU_OPTION_MASK           (0x20000000U)
#define SYSCON_CHIP_ID_FPU_OPTION_SHIFT          (29U)
#define SYSCON_CHIP_ID_FPU_OPTION(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_FPU_OPTION_SHIFT)) & SYSCON_CHIP_ID_FPU_OPTION_MASK)
#define SYSCON_CHIP_ID_USB_OPTION_MASK           (0x40000000U)
#define SYSCON_CHIP_ID_USB_OPTION_SHIFT          (30U)
#define SYSCON_CHIP_ID_USB_OPTION(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_USB_OPTION_SHIFT)) & SYSCON_CHIP_ID_USB_OPTION_MASK)
#define SYSCON_CHIP_ID_FSP_OPTION_MASK           (0x80000000U)
#define SYSCON_CHIP_ID_FSP_OPTION_SHIFT          (31U)
#define SYSCON_CHIP_ID_FSP_OPTION(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_CHIP_ID_FSP_OPTION_SHIFT)) & SYSCON_CHIP_ID_FSP_OPTION_MASK)

/*! @name ANA_CTRL0 - crystal and PA register */
#define SYSCON_ANA_CTRL0_PA_POWER_MASK           (0xFFU)
#define SYSCON_ANA_CTRL0_PA_POWER_SHIFT          (0U)
#define SYSCON_ANA_CTRL0_PA_POWER(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL0_PA_POWER_SHIFT)) & SYSCON_ANA_CTRL0_PA_POWER_MASK)
#define SYSCON_ANA_CTRL0_XTAL_AMP_MASK           (0x300000U)
#define SYSCON_ANA_CTRL0_XTAL_AMP_SHIFT          (20U)
#define SYSCON_ANA_CTRL0_XTAL_AMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL0_XTAL_AMP_SHIFT)) & SYSCON_ANA_CTRL0_XTAL_AMP_MASK)
#define SYSCON_ANA_CTRL0_XTAL_LOAD_CAP_MASK      (0xFC00000U)
#define SYSCON_ANA_CTRL0_XTAL_LOAD_CAP_SHIFT     (22U)
#define SYSCON_ANA_CTRL0_XTAL_LOAD_CAP(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL0_XTAL_LOAD_CAP_SHIFT)) & SYSCON_ANA_CTRL0_XTAL_LOAD_CAP_MASK)
#define SYSCON_ANA_CTRL0_XTAL_EXTRA_CAP_MASK     (0x10000000U)
#define SYSCON_ANA_CTRL0_XTAL_EXTRA_CAP_SHIFT    (28U)
#define SYSCON_ANA_CTRL0_XTAL_EXTRA_CAP(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL0_XTAL_EXTRA_CAP_SHIFT)) & SYSCON_ANA_CTRL0_XTAL_EXTRA_CAP_MASK)
#define SYSCON_ANA_CTRL0_XTAL_MODE_MASK          (0xC0000000U)
#define SYSCON_ANA_CTRL0_XTAL_MODE_SHIFT         (30U)
#define SYSCON_ANA_CTRL0_XTAL_MODE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL0_XTAL_MODE_SHIFT)) & SYSCON_ANA_CTRL0_XTAL_MODE_MASK)

/*! @name XTAL_CTRL - crystal control register */
#define SYSCON_XTAL_CTRL_XTAL_XCUR_BOOST_REG_MASK (0x20U)
#define SYSCON_XTAL_CTRL_XTAL_XCUR_BOOST_REG_SHIFT (5U)
#define SYSCON_XTAL_CTRL_XTAL_XCUR_BOOST_REG(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_XCUR_BOOST_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_XCUR_BOOST_REG_MASK)
#define SYSCON_XTAL_CTRL_XTAL_BPXDLY_MASK        (0x40U)
#define SYSCON_XTAL_CTRL_XTAL_BPXDLY_SHIFT       (6U)
#define SYSCON_XTAL_CTRL_XTAL_BPXDLY(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_BPXDLY_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_BPXDLY_MASK)
#define SYSCON_XTAL_CTRL_XTAL_BP_HYSRES_REG_MASK (0x80U)
#define SYSCON_XTAL_CTRL_XTAL_BP_HYSRES_REG_SHIFT (7U)
#define SYSCON_XTAL_CTRL_XTAL_BP_HYSRES_REG(x)   (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_BP_HYSRES_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_BP_HYSRES_REG_MASK)
#define SYSCON_XTAL_CTRL_XTAL_XSMT_EN_REG_MASK   (0x100U)
#define SYSCON_XTAL_CTRL_XTAL_XSMT_EN_REG_SHIFT  (8U)
#define SYSCON_XTAL_CTRL_XTAL_XSMT_EN_REG(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_XSMT_EN_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_XSMT_EN_REG_MASK)
#define SYSCON_XTAL_CTRL_XTAL_XRDY_REG_MASK      (0x200U)
#define SYSCON_XTAL_CTRL_XTAL_XRDY_REG_SHIFT     (9U)
#define SYSCON_XTAL_CTRL_XTAL_XRDY_REG(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_XRDY_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_XRDY_REG_MASK)
#define SYSCON_XTAL_CTRL_XTAL_XOUT_DIS_REG_MASK  (0x400U)
#define SYSCON_XTAL_CTRL_XTAL_XOUT_DIS_REG_SHIFT (10U)
#define SYSCON_XTAL_CTRL_XTAL_XOUT_DIS_REG(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_XOUT_DIS_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_XOUT_DIS_REG_MASK)
#define SYSCON_XTAL_CTRL_DIV_DIFF_CLK_DIG_DIS_MASK (0x800U)
#define SYSCON_XTAL_CTRL_DIV_DIFF_CLK_DIG_DIS_SHIFT (11U)
#define SYSCON_XTAL_CTRL_DIV_DIFF_CLK_DIG_DIS(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_DIV_DIFF_CLK_DIG_DIS_SHIFT)) & SYSCON_XTAL_CTRL_DIV_DIFF_CLK_DIG_DIS_MASK)
#define SYSCON_XTAL_CTRL_XTAL_SU_CB_REG_MASK     (0x3F0000U)
#define SYSCON_XTAL_CTRL_XTAL_SU_CB_REG_SHIFT    (16U)
#define SYSCON_XTAL_CTRL_XTAL_SU_CB_REG(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_SU_CB_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_SU_CB_REG_MASK)
#define SYSCON_XTAL_CTRL_XTAL_SU_CA_REG_MASK     (0x3F000000U)
#define SYSCON_XTAL_CTRL_XTAL_SU_CA_REG_SHIFT    (24U)
#define SYSCON_XTAL_CTRL_XTAL_SU_CA_REG(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_SU_CA_REG_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_SU_CA_REG_MASK)
#define SYSCON_XTAL_CTRL_XTAL_INV_MASK           (0x40000000U)
#define SYSCON_XTAL_CTRL_XTAL_INV_SHIFT          (30U)
#define SYSCON_XTAL_CTRL_XTAL_INV(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_INV_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_INV_MASK)
#define SYSCON_XTAL_CTRL_XTAL_DIV_MASK           (0x80000000U)
#define SYSCON_XTAL_CTRL_XTAL_DIV_SHIFT          (31U)
#define SYSCON_XTAL_CTRL_XTAL_DIV(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL_CTRL_XTAL_DIV_SHIFT)) & SYSCON_XTAL_CTRL_XTAL_DIV_MASK)

/*! @name BUCK - buck control register */
#define SYSCON_BUCK_BUCK_DRIVER_PART_EN_MASK     (0x1U)
#define SYSCON_BUCK_BUCK_DRIVER_PART_EN_SHIFT    (0U)
#define SYSCON_BUCK_BUCK_DRIVER_PART_EN(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_DRIVER_PART_EN_SHIFT)) & SYSCON_BUCK_BUCK_DRIVER_PART_EN_MASK)
#define SYSCON_BUCK_BUCK_IND_USE_EN_MASK         (0x2U)
#define SYSCON_BUCK_BUCK_IND_USE_EN_SHIFT        (1U)
#define SYSCON_BUCK_BUCK_IND_USE_EN(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_IND_USE_EN_SHIFT)) & SYSCON_BUCK_BUCK_IND_USE_EN_MASK)
#define SYSCON_BUCK_BUCK_ISEL_MASK               (0x300U)
#define SYSCON_BUCK_BUCK_ISEL_SHIFT              (8U)
#define SYSCON_BUCK_BUCK_ISEL(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_ISEL_SHIFT)) & SYSCON_BUCK_BUCK_ISEL_MASK)
#define SYSCON_BUCK_BUCK_VREF_SEL_MASK           (0xC00U)
#define SYSCON_BUCK_BUCK_VREF_SEL_SHIFT          (10U)
#define SYSCON_BUCK_BUCK_VREF_SEL(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_VREF_SEL_SHIFT)) & SYSCON_BUCK_BUCK_VREF_SEL_MASK)
#define SYSCON_BUCK_BUCK_VBG_SEL_MASK            (0x3000U)
#define SYSCON_BUCK_BUCK_VBG_SEL_SHIFT           (12U)
#define SYSCON_BUCK_BUCK_VBG_SEL(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_VBG_SEL_SHIFT)) & SYSCON_BUCK_BUCK_VBG_SEL_MASK)
#define SYSCON_BUCK_BUCK_TMOS_MASK               (0x1F0000U)
#define SYSCON_BUCK_BUCK_TMOS_SHIFT              (16U)
#define SYSCON_BUCK_BUCK_TMOS(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_TMOS_SHIFT)) & SYSCON_BUCK_BUCK_TMOS_MASK)
#define SYSCON_BUCK_BUCK_IC_MASK                 (0x200000U)
#define SYSCON_BUCK_BUCK_IC_SHIFT                (21U)
#define SYSCON_BUCK_BUCK_IC(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCON_BUCK_BUCK_IC_SHIFT)) & SYSCON_BUCK_BUCK_IC_MASK)

/*! @name FC_FRG - flexcomm 0 and 1 clock divider register */
#define SYSCON_FC_FRG_FRG_DIV0_MASK              (0xFFU)
#define SYSCON_FC_FRG_FRG_DIV0_SHIFT             (0U)
#define SYSCON_FC_FRG_FRG_DIV0(x)                (((uint32_t)(((uint32_t)(x)) << SYSCON_FC_FRG_FRG_DIV0_SHIFT)) & SYSCON_FC_FRG_FRG_DIV0_MASK)
#define SYSCON_FC_FRG_FRG_MULT0_MASK             (0xFF00U)
#define SYSCON_FC_FRG_FRG_MULT0_SHIFT            (8U)
#define SYSCON_FC_FRG_FRG_MULT0(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_FC_FRG_FRG_MULT0_SHIFT)) & SYSCON_FC_FRG_FRG_MULT0_MASK)
#define SYSCON_FC_FRG_FRG_DIV1_MASK              (0xFF0000U)
#define SYSCON_FC_FRG_FRG_DIV1_SHIFT             (16U)
#define SYSCON_FC_FRG_FRG_DIV1(x)                (((uint32_t)(((uint32_t)(x)) << SYSCON_FC_FRG_FRG_DIV1_SHIFT)) & SYSCON_FC_FRG_FRG_DIV1_MASK)
#define SYSCON_FC_FRG_FRG_MULT1_MASK             (0xFF000000U)
#define SYSCON_FC_FRG_FRG_MULT1_SHIFT            (24U)
#define SYSCON_FC_FRG_FRG_MULT1(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_FC_FRG_FRG_MULT1_SHIFT)) & SYSCON_FC_FRG_FRG_MULT1_MASK)

/*! @name PIO_PULL_CFG - pad pull control register 0..pad pull control register 2 */
#define SYSCON_PIO_PULL_CFG_PA00_PULL_MASK       (0x3U)
#define SYSCON_PIO_PULL_CFG_PA00_PULL_SHIFT      (0U)
#define SYSCON_PIO_PULL_CFG_PA00_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA00_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA00_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA16_PULL_MASK       (0x3U)
#define SYSCON_PIO_PULL_CFG_PA16_PULL_SHIFT      (0U)
#define SYSCON_PIO_PULL_CFG_PA16_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA16_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA16_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PB00_PULL_MASK       (0x3U)
#define SYSCON_PIO_PULL_CFG_PB00_PULL_SHIFT      (0U)
#define SYSCON_PIO_PULL_CFG_PB00_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PB00_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PB00_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PB01_PULL_MASK       (0xCU)
#define SYSCON_PIO_PULL_CFG_PB01_PULL_SHIFT      (2U)
#define SYSCON_PIO_PULL_CFG_PB01_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PB01_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PB01_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA01_PULL_MASK       (0xCU)
#define SYSCON_PIO_PULL_CFG_PA01_PULL_SHIFT      (2U)
#define SYSCON_PIO_PULL_CFG_PA01_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA01_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA01_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA17_PULL_MASK       (0xCU)
#define SYSCON_PIO_PULL_CFG_PA17_PULL_SHIFT      (2U)
#define SYSCON_PIO_PULL_CFG_PA17_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA17_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA17_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PB02_PULL_MASK       (0x30U)
#define SYSCON_PIO_PULL_CFG_PB02_PULL_SHIFT      (4U)
#define SYSCON_PIO_PULL_CFG_PB02_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PB02_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PB02_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA18_PULL_MASK       (0x30U)
#define SYSCON_PIO_PULL_CFG_PA18_PULL_SHIFT      (4U)
#define SYSCON_PIO_PULL_CFG_PA18_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA18_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA18_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA02_PULL_MASK       (0x30U)
#define SYSCON_PIO_PULL_CFG_PA02_PULL_SHIFT      (4U)
#define SYSCON_PIO_PULL_CFG_PA02_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA02_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA02_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA19_PULL_MASK       (0xC0U)
#define SYSCON_PIO_PULL_CFG_PA19_PULL_SHIFT      (6U)
#define SYSCON_PIO_PULL_CFG_PA19_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA19_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA19_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA03_PULL_MASK       (0xC0U)
#define SYSCON_PIO_PULL_CFG_PA03_PULL_SHIFT      (6U)
#define SYSCON_PIO_PULL_CFG_PA03_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA03_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA03_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA04_PULL_MASK       (0x300U)
#define SYSCON_PIO_PULL_CFG_PA04_PULL_SHIFT      (8U)
#define SYSCON_PIO_PULL_CFG_PA04_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA04_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA04_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA20_PULL_MASK       (0x300U)
#define SYSCON_PIO_PULL_CFG_PA20_PULL_SHIFT      (8U)
#define SYSCON_PIO_PULL_CFG_PA20_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA20_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA20_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA21_PULL_MASK       (0xC00U)
#define SYSCON_PIO_PULL_CFG_PA21_PULL_SHIFT      (10U)
#define SYSCON_PIO_PULL_CFG_PA21_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA21_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA21_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA05_PULL_MASK       (0xC00U)
#define SYSCON_PIO_PULL_CFG_PA05_PULL_SHIFT      (10U)
#define SYSCON_PIO_PULL_CFG_PA05_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA05_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA05_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA22_PULL_MASK       (0x3000U)
#define SYSCON_PIO_PULL_CFG_PA22_PULL_SHIFT      (12U)
#define SYSCON_PIO_PULL_CFG_PA22_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA22_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA22_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA06_PULL_MASK       (0x3000U)
#define SYSCON_PIO_PULL_CFG_PA06_PULL_SHIFT      (12U)
#define SYSCON_PIO_PULL_CFG_PA06_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA06_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA06_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA23_PULL_MASK       (0xC000U)
#define SYSCON_PIO_PULL_CFG_PA23_PULL_SHIFT      (14U)
#define SYSCON_PIO_PULL_CFG_PA23_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA23_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA23_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA07_PULL_MASK       (0xC000U)
#define SYSCON_PIO_PULL_CFG_PA07_PULL_SHIFT      (14U)
#define SYSCON_PIO_PULL_CFG_PA07_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA07_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA07_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA08_PULL_MASK       (0x30000U)
#define SYSCON_PIO_PULL_CFG_PA08_PULL_SHIFT      (16U)
#define SYSCON_PIO_PULL_CFG_PA08_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA08_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA08_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA24_PULL_MASK       (0x30000U)
#define SYSCON_PIO_PULL_CFG_PA24_PULL_SHIFT      (16U)
#define SYSCON_PIO_PULL_CFG_PA24_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA24_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA24_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA09_PULL_MASK       (0xC0000U)
#define SYSCON_PIO_PULL_CFG_PA09_PULL_SHIFT      (18U)
#define SYSCON_PIO_PULL_CFG_PA09_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA09_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA09_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA25_PULL_MASK       (0xC0000U)
#define SYSCON_PIO_PULL_CFG_PA25_PULL_SHIFT      (18U)
#define SYSCON_PIO_PULL_CFG_PA25_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA25_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA25_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA26_PULL_MASK       (0x300000U)
#define SYSCON_PIO_PULL_CFG_PA26_PULL_SHIFT      (20U)
#define SYSCON_PIO_PULL_CFG_PA26_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA26_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA26_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA10_PULL_MASK       (0x300000U)
#define SYSCON_PIO_PULL_CFG_PA10_PULL_SHIFT      (20U)
#define SYSCON_PIO_PULL_CFG_PA10_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA10_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA10_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA11_PULL_MASK       (0xC00000U)
#define SYSCON_PIO_PULL_CFG_PA11_PULL_SHIFT      (22U)
#define SYSCON_PIO_PULL_CFG_PA11_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA11_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA11_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA27_PULL_MASK       (0xC00000U)
#define SYSCON_PIO_PULL_CFG_PA27_PULL_SHIFT      (22U)
#define SYSCON_PIO_PULL_CFG_PA27_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA27_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA27_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA12_PULL_MASK       (0x3000000U)
#define SYSCON_PIO_PULL_CFG_PA12_PULL_SHIFT      (24U)
#define SYSCON_PIO_PULL_CFG_PA12_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA12_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA12_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA28_PULL_MASK       (0x3000000U)
#define SYSCON_PIO_PULL_CFG_PA28_PULL_SHIFT      (24U)
#define SYSCON_PIO_PULL_CFG_PA28_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA28_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA28_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA29_PULL_MASK       (0xC000000U)
#define SYSCON_PIO_PULL_CFG_PA29_PULL_SHIFT      (26U)
#define SYSCON_PIO_PULL_CFG_PA29_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA29_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA29_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA13_PULL_MASK       (0xC000000U)
#define SYSCON_PIO_PULL_CFG_PA13_PULL_SHIFT      (26U)
#define SYSCON_PIO_PULL_CFG_PA13_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA13_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA13_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA30_PULL_MASK       (0x30000000U)
#define SYSCON_PIO_PULL_CFG_PA30_PULL_SHIFT      (28U)
#define SYSCON_PIO_PULL_CFG_PA30_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA30_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA30_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA14_PULL_MASK       (0x30000000U)
#define SYSCON_PIO_PULL_CFG_PA14_PULL_SHIFT      (28U)
#define SYSCON_PIO_PULL_CFG_PA14_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA14_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA14_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA15_PULL_MASK       (0xC0000000U)
#define SYSCON_PIO_PULL_CFG_PA15_PULL_SHIFT      (30U)
#define SYSCON_PIO_PULL_CFG_PA15_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA15_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA15_PULL_MASK)
#define SYSCON_PIO_PULL_CFG_PA31_PULL_MASK       (0xC0000000U)
#define SYSCON_PIO_PULL_CFG_PA31_PULL_SHIFT      (30U)
#define SYSCON_PIO_PULL_CFG_PA31_PULL(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_PULL_CFG_PA31_PULL_SHIFT)) & SYSCON_PIO_PULL_CFG_PA31_PULL_MASK)

/* The count of SYSCON_PIO_PULL_CFG */
#define SYSCON_PIO_PULL_CFG_COUNT                (3U)

/*! @name IO_CAP - io status capture register */
#define SYSCON_IO_CAP_PIN_RETENTION_MASK         (0x1U)
#define SYSCON_IO_CAP_PIN_RETENTION_SHIFT        (0U)
#define SYSCON_IO_CAP_PIN_RETENTION(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_IO_CAP_PIN_RETENTION_SHIFT)) & SYSCON_IO_CAP_PIN_RETENTION_MASK)

/*! @name PIO_DRV_CFG - pad drive strength register 0..pad drive extra register */
#define SYSCON_PIO_DRV_CFG_PB00_DRV_MASK         (0x1U)
#define SYSCON_PIO_DRV_CFG_PB00_DRV_SHIFT        (0U)
#define SYSCON_PIO_DRV_CFG_PB00_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PB00_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PB00_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA00_DRV_MASK         (0x1U)
#define SYSCON_PIO_DRV_CFG_PA00_DRV_SHIFT        (0U)
#define SYSCON_PIO_DRV_CFG_PA00_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA00_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA00_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA01_DRV_MASK         (0x2U)
#define SYSCON_PIO_DRV_CFG_PA01_DRV_SHIFT        (1U)
#define SYSCON_PIO_DRV_CFG_PA01_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA01_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA01_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PB01_DRV_MASK         (0x2U)
#define SYSCON_PIO_DRV_CFG_PB01_DRV_SHIFT        (1U)
#define SYSCON_PIO_DRV_CFG_PB01_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PB01_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PB01_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PB02_DRV_MASK         (0x4U)
#define SYSCON_PIO_DRV_CFG_PB02_DRV_SHIFT        (2U)
#define SYSCON_PIO_DRV_CFG_PB02_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PB02_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PB02_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA02_DRV_MASK         (0x4U)
#define SYSCON_PIO_DRV_CFG_PA02_DRV_SHIFT        (2U)
#define SYSCON_PIO_DRV_CFG_PA02_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA02_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA02_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA03_DRV_MASK         (0x8U)
#define SYSCON_PIO_DRV_CFG_PA03_DRV_SHIFT        (3U)
#define SYSCON_PIO_DRV_CFG_PA03_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA03_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA03_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA04_DRV_MASK         (0x10U)
#define SYSCON_PIO_DRV_CFG_PA04_DRV_SHIFT        (4U)
#define SYSCON_PIO_DRV_CFG_PA04_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA04_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA04_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA05_DRV_MASK         (0x20U)
#define SYSCON_PIO_DRV_CFG_PA05_DRV_SHIFT        (5U)
#define SYSCON_PIO_DRV_CFG_PA05_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA05_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA05_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA06_DRV_MASK         (0x40U)
#define SYSCON_PIO_DRV_CFG_PA06_DRV_SHIFT        (6U)
#define SYSCON_PIO_DRV_CFG_PA06_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA06_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA06_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA06_DRV_EXTRA_MASK   (0x40U)
#define SYSCON_PIO_DRV_CFG_PA06_DRV_EXTRA_SHIFT  (6U)
#define SYSCON_PIO_DRV_CFG_PA06_DRV_EXTRA(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA06_DRV_EXTRA_SHIFT)) & SYSCON_PIO_DRV_CFG_PA06_DRV_EXTRA_MASK)
#define SYSCON_PIO_DRV_CFG_PA07_DRV_MASK         (0x80U)
#define SYSCON_PIO_DRV_CFG_PA07_DRV_SHIFT        (7U)
#define SYSCON_PIO_DRV_CFG_PA07_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA07_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA07_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA08_DRV_MASK         (0x100U)
#define SYSCON_PIO_DRV_CFG_PA08_DRV_SHIFT        (8U)
#define SYSCON_PIO_DRV_CFG_PA08_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA08_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA08_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA09_DRV_MASK         (0x200U)
#define SYSCON_PIO_DRV_CFG_PA09_DRV_SHIFT        (9U)
#define SYSCON_PIO_DRV_CFG_PA09_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA09_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA09_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA10_DRV_MASK         (0x400U)
#define SYSCON_PIO_DRV_CFG_PA10_DRV_SHIFT        (10U)
#define SYSCON_PIO_DRV_CFG_PA10_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA10_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA10_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA11_DRV_EXTRA_MASK   (0x800U)
#define SYSCON_PIO_DRV_CFG_PA11_DRV_EXTRA_SHIFT  (11U)
#define SYSCON_PIO_DRV_CFG_PA11_DRV_EXTRA(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA11_DRV_EXTRA_SHIFT)) & SYSCON_PIO_DRV_CFG_PA11_DRV_EXTRA_MASK)
#define SYSCON_PIO_DRV_CFG_PA11_DRV_MASK         (0x800U)
#define SYSCON_PIO_DRV_CFG_PA11_DRV_SHIFT        (11U)
#define SYSCON_PIO_DRV_CFG_PA11_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA11_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA11_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA12_DRV_MASK         (0x1000U)
#define SYSCON_PIO_DRV_CFG_PA12_DRV_SHIFT        (12U)
#define SYSCON_PIO_DRV_CFG_PA12_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA12_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA12_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA13_DRV_MASK         (0x2000U)
#define SYSCON_PIO_DRV_CFG_PA13_DRV_SHIFT        (13U)
#define SYSCON_PIO_DRV_CFG_PA13_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA13_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA13_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA14_DRV_MASK         (0x4000U)
#define SYSCON_PIO_DRV_CFG_PA14_DRV_SHIFT        (14U)
#define SYSCON_PIO_DRV_CFG_PA14_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA14_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA14_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA15_DRV_MASK         (0x8000U)
#define SYSCON_PIO_DRV_CFG_PA15_DRV_SHIFT        (15U)
#define SYSCON_PIO_DRV_CFG_PA15_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA15_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA15_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA16_DRV_MASK         (0x10000U)
#define SYSCON_PIO_DRV_CFG_PA16_DRV_SHIFT        (16U)
#define SYSCON_PIO_DRV_CFG_PA16_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA16_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA16_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA17_DRV_MASK         (0x20000U)
#define SYSCON_PIO_DRV_CFG_PA17_DRV_SHIFT        (17U)
#define SYSCON_PIO_DRV_CFG_PA17_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA17_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA17_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA18_DRV_MASK         (0x40000U)
#define SYSCON_PIO_DRV_CFG_PA18_DRV_SHIFT        (18U)
#define SYSCON_PIO_DRV_CFG_PA18_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA18_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA18_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA19_DRV_EXTRA_MASK   (0x80000U)
#define SYSCON_PIO_DRV_CFG_PA19_DRV_EXTRA_SHIFT  (19U)
#define SYSCON_PIO_DRV_CFG_PA19_DRV_EXTRA(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA19_DRV_EXTRA_SHIFT)) & SYSCON_PIO_DRV_CFG_PA19_DRV_EXTRA_MASK)
#define SYSCON_PIO_DRV_CFG_PA19_DRV_MASK         (0x80000U)
#define SYSCON_PIO_DRV_CFG_PA19_DRV_SHIFT        (19U)
#define SYSCON_PIO_DRV_CFG_PA19_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA19_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA19_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA20_DRV_MASK         (0x100000U)
#define SYSCON_PIO_DRV_CFG_PA20_DRV_SHIFT        (20U)
#define SYSCON_PIO_DRV_CFG_PA20_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA20_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA20_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA21_DRV_MASK         (0x200000U)
#define SYSCON_PIO_DRV_CFG_PA21_DRV_SHIFT        (21U)
#define SYSCON_PIO_DRV_CFG_PA21_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA21_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA21_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA22_DRV_MASK         (0x400000U)
#define SYSCON_PIO_DRV_CFG_PA22_DRV_SHIFT        (22U)
#define SYSCON_PIO_DRV_CFG_PA22_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA22_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA22_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA23_DRV_MASK         (0x800000U)
#define SYSCON_PIO_DRV_CFG_PA23_DRV_SHIFT        (23U)
#define SYSCON_PIO_DRV_CFG_PA23_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA23_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA23_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA24_DRV_MASK         (0x1000000U)
#define SYSCON_PIO_DRV_CFG_PA24_DRV_SHIFT        (24U)
#define SYSCON_PIO_DRV_CFG_PA24_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA24_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA24_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA25_DRV_MASK         (0x2000000U)
#define SYSCON_PIO_DRV_CFG_PA25_DRV_SHIFT        (25U)
#define SYSCON_PIO_DRV_CFG_PA25_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA25_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA25_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA26_DRV_MASK         (0x4000000U)
#define SYSCON_PIO_DRV_CFG_PA26_DRV_SHIFT        (26U)
#define SYSCON_PIO_DRV_CFG_PA26_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA26_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA26_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA26_DRV_EXTRA_MASK   (0x4000000U)
#define SYSCON_PIO_DRV_CFG_PA26_DRV_EXTRA_SHIFT  (26U)
#define SYSCON_PIO_DRV_CFG_PA26_DRV_EXTRA(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA26_DRV_EXTRA_SHIFT)) & SYSCON_PIO_DRV_CFG_PA26_DRV_EXTRA_MASK)
#define SYSCON_PIO_DRV_CFG_PA27_DRV_EXTRA_MASK   (0x8000000U)
#define SYSCON_PIO_DRV_CFG_PA27_DRV_EXTRA_SHIFT  (27U)
#define SYSCON_PIO_DRV_CFG_PA27_DRV_EXTRA(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA27_DRV_EXTRA_SHIFT)) & SYSCON_PIO_DRV_CFG_PA27_DRV_EXTRA_MASK)
#define SYSCON_PIO_DRV_CFG_PA27_DRV_MASK         (0x8000000U)
#define SYSCON_PIO_DRV_CFG_PA27_DRV_SHIFT        (27U)
#define SYSCON_PIO_DRV_CFG_PA27_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA27_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA27_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA28_DRV_MASK         (0x10000000U)
#define SYSCON_PIO_DRV_CFG_PA28_DRV_SHIFT        (28U)
#define SYSCON_PIO_DRV_CFG_PA28_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA28_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA28_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA29_DRV_MASK         (0x20000000U)
#define SYSCON_PIO_DRV_CFG_PA29_DRV_SHIFT        (29U)
#define SYSCON_PIO_DRV_CFG_PA29_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA29_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA29_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA30_DRV_MASK         (0x40000000U)
#define SYSCON_PIO_DRV_CFG_PA30_DRV_SHIFT        (30U)
#define SYSCON_PIO_DRV_CFG_PA30_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA30_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA30_DRV_MASK)
#define SYSCON_PIO_DRV_CFG_PA31_DRV_MASK         (0x80000000U)
#define SYSCON_PIO_DRV_CFG_PA31_DRV_SHIFT        (31U)
#define SYSCON_PIO_DRV_CFG_PA31_DRV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_DRV_CFG_PA31_DRV_SHIFT)) & SYSCON_PIO_DRV_CFG_PA31_DRV_MASK)

/* The count of SYSCON_PIO_DRV_CFG */
#define SYSCON_PIO_DRV_CFG_COUNT                 (3U)

/*! @name PIO_CFG_MISC - pin misc control register */
#define SYSCON_PIO_CFG_MISC_PB00_AE_MASK         (0x1U)
#define SYSCON_PIO_CFG_MISC_PB00_AE_SHIFT        (0U)
#define SYSCON_PIO_CFG_MISC_PB00_AE(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CFG_MISC_PB00_AE_SHIFT)) & SYSCON_PIO_CFG_MISC_PB00_AE_MASK)
#define SYSCON_PIO_CFG_MISC_PB01_AE_MASK         (0x2U)
#define SYSCON_PIO_CFG_MISC_PB01_AE_SHIFT        (1U)
#define SYSCON_PIO_CFG_MISC_PB01_AE(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CFG_MISC_PB01_AE_SHIFT)) & SYSCON_PIO_CFG_MISC_PB01_AE_MASK)
#define SYSCON_PIO_CFG_MISC_PSYNC_MASK           (0x8000U)
#define SYSCON_PIO_CFG_MISC_PSYNC_SHIFT          (15U)
#define SYSCON_PIO_CFG_MISC_PSYNC(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CFG_MISC_PSYNC_SHIFT)) & SYSCON_PIO_CFG_MISC_PSYNC_MASK)
#define SYSCON_PIO_CFG_MISC_PB02_MODE_MASK       (0x10000U)
#define SYSCON_PIO_CFG_MISC_PB02_MODE_SHIFT      (16U)
#define SYSCON_PIO_CFG_MISC_PB02_MODE(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CFG_MISC_PB02_MODE_SHIFT)) & SYSCON_PIO_CFG_MISC_PB02_MODE_MASK)
#define SYSCON_PIO_CFG_MISC_TRX_EN_INV_MASK      (0x40000U)
#define SYSCON_PIO_CFG_MISC_TRX_EN_INV_SHIFT     (18U)
#define SYSCON_PIO_CFG_MISC_TRX_EN_INV(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CFG_MISC_TRX_EN_INV_SHIFT)) & SYSCON_PIO_CFG_MISC_TRX_EN_INV_MASK)
#define SYSCON_PIO_CFG_MISC_RFE_INV_MASK         (0x80000U)
#define SYSCON_PIO_CFG_MISC_RFE_INV_SHIFT        (19U)
#define SYSCON_PIO_CFG_MISC_RFE_INV(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CFG_MISC_RFE_INV_SHIFT)) & SYSCON_PIO_CFG_MISC_RFE_INV_MASK)

/*! @name PIO_WAKEUP_LVL0 - pin wakeup polarity register 0 */
#define SYSCON_PIO_WAKEUP_LVL0_PA00_WAKEUP_LVL_MASK (0x1U)
#define SYSCON_PIO_WAKEUP_LVL0_PA00_WAKEUP_LVL_SHIFT (0U)
#define SYSCON_PIO_WAKEUP_LVL0_PA00_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA00_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA00_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA01_WAKEUP_LVL_MASK (0x2U)
#define SYSCON_PIO_WAKEUP_LVL0_PA01_WAKEUP_LVL_SHIFT (1U)
#define SYSCON_PIO_WAKEUP_LVL0_PA01_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA01_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA01_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA02_WAKEUP_LVL_MASK (0x4U)
#define SYSCON_PIO_WAKEUP_LVL0_PA02_WAKEUP_LVL_SHIFT (2U)
#define SYSCON_PIO_WAKEUP_LVL0_PA02_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA02_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA02_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA03_WAKEUP_LVL_MASK (0x8U)
#define SYSCON_PIO_WAKEUP_LVL0_PA03_WAKEUP_LVL_SHIFT (3U)
#define SYSCON_PIO_WAKEUP_LVL0_PA03_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA03_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA03_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA04_WAKEUP_LVL_MASK (0x10U)
#define SYSCON_PIO_WAKEUP_LVL0_PA04_WAKEUP_LVL_SHIFT (4U)
#define SYSCON_PIO_WAKEUP_LVL0_PA04_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA04_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA04_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA05_WAKEUP_LVL_MASK (0x20U)
#define SYSCON_PIO_WAKEUP_LVL0_PA05_WAKEUP_LVL_SHIFT (5U)
#define SYSCON_PIO_WAKEUP_LVL0_PA05_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA05_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA05_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA06_WAKEUP_LVL_MASK (0x40U)
#define SYSCON_PIO_WAKEUP_LVL0_PA06_WAKEUP_LVL_SHIFT (6U)
#define SYSCON_PIO_WAKEUP_LVL0_PA06_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA06_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA06_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA07_WAKEUP_LVL_MASK (0x80U)
#define SYSCON_PIO_WAKEUP_LVL0_PA07_WAKEUP_LVL_SHIFT (7U)
#define SYSCON_PIO_WAKEUP_LVL0_PA07_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA07_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA07_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA08_WAKEUP_LVL_MASK (0x100U)
#define SYSCON_PIO_WAKEUP_LVL0_PA08_WAKEUP_LVL_SHIFT (8U)
#define SYSCON_PIO_WAKEUP_LVL0_PA08_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA08_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA08_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA09_WAKEUP_LVL_MASK (0x200U)
#define SYSCON_PIO_WAKEUP_LVL0_PA09_WAKEUP_LVL_SHIFT (9U)
#define SYSCON_PIO_WAKEUP_LVL0_PA09_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA09_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA09_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA10_WAKEUP_LVL_MASK (0x400U)
#define SYSCON_PIO_WAKEUP_LVL0_PA10_WAKEUP_LVL_SHIFT (10U)
#define SYSCON_PIO_WAKEUP_LVL0_PA10_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA10_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA10_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA11_WAKEUP_LVL_MASK (0x800U)
#define SYSCON_PIO_WAKEUP_LVL0_PA11_WAKEUP_LVL_SHIFT (11U)
#define SYSCON_PIO_WAKEUP_LVL0_PA11_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA11_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA11_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA12_WAKEUP_LVL_MASK (0x1000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA12_WAKEUP_LVL_SHIFT (12U)
#define SYSCON_PIO_WAKEUP_LVL0_PA12_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA12_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA12_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA13_WAKEUP_LVL_MASK (0x2000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA13_WAKEUP_LVL_SHIFT (13U)
#define SYSCON_PIO_WAKEUP_LVL0_PA13_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA13_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA13_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA14_WAKEUP_LVL_MASK (0x4000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA14_WAKEUP_LVL_SHIFT (14U)
#define SYSCON_PIO_WAKEUP_LVL0_PA14_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA14_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA14_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA15_WAKEUP_LVL_MASK (0x8000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA15_WAKEUP_LVL_SHIFT (15U)
#define SYSCON_PIO_WAKEUP_LVL0_PA15_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA15_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA15_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA16_WAKEUP_LVL_MASK (0x10000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA16_WAKEUP_LVL_SHIFT (16U)
#define SYSCON_PIO_WAKEUP_LVL0_PA16_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA16_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA16_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA17_WAKEUP_LVL_MASK (0x20000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA17_WAKEUP_LVL_SHIFT (17U)
#define SYSCON_PIO_WAKEUP_LVL0_PA17_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA17_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA17_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA18_WAKEUP_LVL_MASK (0x40000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA18_WAKEUP_LVL_SHIFT (18U)
#define SYSCON_PIO_WAKEUP_LVL0_PA18_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA18_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA18_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA19_WAKEUP_LVL_MASK (0x80000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA19_WAKEUP_LVL_SHIFT (19U)
#define SYSCON_PIO_WAKEUP_LVL0_PA19_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA19_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA19_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA20_WAKEUP_LVL_MASK (0x100000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA20_WAKEUP_LVL_SHIFT (20U)
#define SYSCON_PIO_WAKEUP_LVL0_PA20_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA20_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA20_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA21_WAKEUP_LVL_MASK (0x200000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA21_WAKEUP_LVL_SHIFT (21U)
#define SYSCON_PIO_WAKEUP_LVL0_PA21_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA21_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA21_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA22_WAKEUP_LVL_MASK (0x400000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA22_WAKEUP_LVL_SHIFT (22U)
#define SYSCON_PIO_WAKEUP_LVL0_PA22_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA22_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA22_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA23_WAKEUP_LVL_MASK (0x800000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA23_WAKEUP_LVL_SHIFT (23U)
#define SYSCON_PIO_WAKEUP_LVL0_PA23_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA23_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA23_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA24_WAKEUP_LVL_MASK (0x1000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA24_WAKEUP_LVL_SHIFT (24U)
#define SYSCON_PIO_WAKEUP_LVL0_PA24_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA24_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA24_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA25_WAKEUP_LVL_MASK (0x2000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA25_WAKEUP_LVL_SHIFT (25U)
#define SYSCON_PIO_WAKEUP_LVL0_PA25_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA25_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA25_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA26_WAKEUP_LVL_MASK (0x4000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA26_WAKEUP_LVL_SHIFT (26U)
#define SYSCON_PIO_WAKEUP_LVL0_PA26_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA26_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA26_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA27_WAKEUP_LVL_MASK (0x8000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA27_WAKEUP_LVL_SHIFT (27U)
#define SYSCON_PIO_WAKEUP_LVL0_PA27_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA27_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA27_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA28_WAKEUP_LVL_MASK (0x10000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA28_WAKEUP_LVL_SHIFT (28U)
#define SYSCON_PIO_WAKEUP_LVL0_PA28_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA28_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA28_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA29_WAKEUP_LVL_MASK (0x20000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA29_WAKEUP_LVL_SHIFT (29U)
#define SYSCON_PIO_WAKEUP_LVL0_PA29_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA29_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA29_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA30_WAKEUP_LVL_MASK (0x40000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA30_WAKEUP_LVL_SHIFT (30U)
#define SYSCON_PIO_WAKEUP_LVL0_PA30_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA30_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA30_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL0_PA31_WAKEUP_LVL_MASK (0x80000000U)
#define SYSCON_PIO_WAKEUP_LVL0_PA31_WAKEUP_LVL_SHIFT (31U)
#define SYSCON_PIO_WAKEUP_LVL0_PA31_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL0_PA31_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL0_PA31_WAKEUP_LVL_MASK)

/*! @name PIO_WAKEUP_LVL1 - pin wakeup polarity register 1 */
#define SYSCON_PIO_WAKEUP_LVL1_PB00_WAKEUP_LVL_MASK (0x1U)
#define SYSCON_PIO_WAKEUP_LVL1_PB00_WAKEUP_LVL_SHIFT (0U)
#define SYSCON_PIO_WAKEUP_LVL1_PB00_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL1_PB00_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL1_PB00_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL1_PB01_WAKEUP_LVL_MASK (0x2U)
#define SYSCON_PIO_WAKEUP_LVL1_PB01_WAKEUP_LVL_SHIFT (1U)
#define SYSCON_PIO_WAKEUP_LVL1_PB01_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL1_PB01_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL1_PB01_WAKEUP_LVL_MASK)
#define SYSCON_PIO_WAKEUP_LVL1_PB02_WAKEUP_LVL_MASK (0x4U)
#define SYSCON_PIO_WAKEUP_LVL1_PB02_WAKEUP_LVL_SHIFT (2U)
#define SYSCON_PIO_WAKEUP_LVL1_PB02_WAKEUP_LVL(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_LVL1_PB02_WAKEUP_LVL_SHIFT)) & SYSCON_PIO_WAKEUP_LVL1_PB02_WAKEUP_LVL_MASK)

/*! @name PIO_IE_CFG0 - pad input enable register 0 */
#define SYSCON_PIO_IE_CFG0_PA00_IE_MASK          (0x1U)
#define SYSCON_PIO_IE_CFG0_PA00_IE_SHIFT         (0U)
#define SYSCON_PIO_IE_CFG0_PA00_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA00_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA00_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA01_IE_MASK          (0x2U)
#define SYSCON_PIO_IE_CFG0_PA01_IE_SHIFT         (1U)
#define SYSCON_PIO_IE_CFG0_PA01_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA01_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA01_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA02_IE_MASK          (0x4U)
#define SYSCON_PIO_IE_CFG0_PA02_IE_SHIFT         (2U)
#define SYSCON_PIO_IE_CFG0_PA02_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA02_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA02_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA03_IE_MASK          (0x8U)
#define SYSCON_PIO_IE_CFG0_PA03_IE_SHIFT         (3U)
#define SYSCON_PIO_IE_CFG0_PA03_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA03_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA03_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA04_IE_MASK          (0x10U)
#define SYSCON_PIO_IE_CFG0_PA04_IE_SHIFT         (4U)
#define SYSCON_PIO_IE_CFG0_PA04_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA04_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA04_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA05_IE_MASK          (0x20U)
#define SYSCON_PIO_IE_CFG0_PA05_IE_SHIFT         (5U)
#define SYSCON_PIO_IE_CFG0_PA05_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA05_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA05_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA06_IE_MASK          (0x40U)
#define SYSCON_PIO_IE_CFG0_PA06_IE_SHIFT         (6U)
#define SYSCON_PIO_IE_CFG0_PA06_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA06_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA06_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA07_IE_MASK          (0x80U)
#define SYSCON_PIO_IE_CFG0_PA07_IE_SHIFT         (7U)
#define SYSCON_PIO_IE_CFG0_PA07_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA07_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA07_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA08_IE_MASK          (0x100U)
#define SYSCON_PIO_IE_CFG0_PA08_IE_SHIFT         (8U)
#define SYSCON_PIO_IE_CFG0_PA08_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA08_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA08_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA09_IE_MASK          (0x200U)
#define SYSCON_PIO_IE_CFG0_PA09_IE_SHIFT         (9U)
#define SYSCON_PIO_IE_CFG0_PA09_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA09_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA09_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA10_IE_MASK          (0x400U)
#define SYSCON_PIO_IE_CFG0_PA10_IE_SHIFT         (10U)
#define SYSCON_PIO_IE_CFG0_PA10_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA10_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA10_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA11_IE_MASK          (0x800U)
#define SYSCON_PIO_IE_CFG0_PA11_IE_SHIFT         (11U)
#define SYSCON_PIO_IE_CFG0_PA11_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA11_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA11_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA12_IE_MASK          (0x1000U)
#define SYSCON_PIO_IE_CFG0_PA12_IE_SHIFT         (12U)
#define SYSCON_PIO_IE_CFG0_PA12_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA12_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA12_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA13_IE_MASK          (0x2000U)
#define SYSCON_PIO_IE_CFG0_PA13_IE_SHIFT         (13U)
#define SYSCON_PIO_IE_CFG0_PA13_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA13_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA13_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA14_IE_MASK          (0x4000U)
#define SYSCON_PIO_IE_CFG0_PA14_IE_SHIFT         (14U)
#define SYSCON_PIO_IE_CFG0_PA14_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA14_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA14_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA15_IE_MASK          (0x8000U)
#define SYSCON_PIO_IE_CFG0_PA15_IE_SHIFT         (15U)
#define SYSCON_PIO_IE_CFG0_PA15_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA15_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA15_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA16_IE_MASK          (0x10000U)
#define SYSCON_PIO_IE_CFG0_PA16_IE_SHIFT         (16U)
#define SYSCON_PIO_IE_CFG0_PA16_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA16_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA16_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA17_IE_MASK          (0x20000U)
#define SYSCON_PIO_IE_CFG0_PA17_IE_SHIFT         (17U)
#define SYSCON_PIO_IE_CFG0_PA17_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA17_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA17_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA18_IE_MASK          (0x40000U)
#define SYSCON_PIO_IE_CFG0_PA18_IE_SHIFT         (18U)
#define SYSCON_PIO_IE_CFG0_PA18_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA18_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA18_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA19_IE_MASK          (0x80000U)
#define SYSCON_PIO_IE_CFG0_PA19_IE_SHIFT         (19U)
#define SYSCON_PIO_IE_CFG0_PA19_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA19_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA19_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA20_IE_MASK          (0x100000U)
#define SYSCON_PIO_IE_CFG0_PA20_IE_SHIFT         (20U)
#define SYSCON_PIO_IE_CFG0_PA20_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA20_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA20_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA21_IE_MASK          (0x200000U)
#define SYSCON_PIO_IE_CFG0_PA21_IE_SHIFT         (21U)
#define SYSCON_PIO_IE_CFG0_PA21_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA21_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA21_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA22_IE_MASK          (0x400000U)
#define SYSCON_PIO_IE_CFG0_PA22_IE_SHIFT         (22U)
#define SYSCON_PIO_IE_CFG0_PA22_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA22_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA22_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA23_IE_MASK          (0x800000U)
#define SYSCON_PIO_IE_CFG0_PA23_IE_SHIFT         (23U)
#define SYSCON_PIO_IE_CFG0_PA23_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA23_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA23_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA24_IE_MASK          (0x1000000U)
#define SYSCON_PIO_IE_CFG0_PA24_IE_SHIFT         (24U)
#define SYSCON_PIO_IE_CFG0_PA24_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA24_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA24_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA25_IE_MASK          (0x2000000U)
#define SYSCON_PIO_IE_CFG0_PA25_IE_SHIFT         (25U)
#define SYSCON_PIO_IE_CFG0_PA25_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA25_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA25_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA26_IE_MASK          (0x4000000U)
#define SYSCON_PIO_IE_CFG0_PA26_IE_SHIFT         (26U)
#define SYSCON_PIO_IE_CFG0_PA26_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA26_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA26_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA27_IE_MASK          (0x8000000U)
#define SYSCON_PIO_IE_CFG0_PA27_IE_SHIFT         (27U)
#define SYSCON_PIO_IE_CFG0_PA27_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA27_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA27_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA28_IE_MASK          (0x10000000U)
#define SYSCON_PIO_IE_CFG0_PA28_IE_SHIFT         (28U)
#define SYSCON_PIO_IE_CFG0_PA28_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA28_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA28_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA29_IE_MASK          (0x20000000U)
#define SYSCON_PIO_IE_CFG0_PA29_IE_SHIFT         (29U)
#define SYSCON_PIO_IE_CFG0_PA29_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA29_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA29_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA30_IE_MASK          (0x40000000U)
#define SYSCON_PIO_IE_CFG0_PA30_IE_SHIFT         (30U)
#define SYSCON_PIO_IE_CFG0_PA30_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA30_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA30_IE_MASK)
#define SYSCON_PIO_IE_CFG0_PA31_IE_MASK          (0x80000000U)
#define SYSCON_PIO_IE_CFG0_PA31_IE_SHIFT         (31U)
#define SYSCON_PIO_IE_CFG0_PA31_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG0_PA31_IE_SHIFT)) & SYSCON_PIO_IE_CFG0_PA31_IE_MASK)

/*! @name PIO_IE_CFG1 - pad input enable register 1 */
#define SYSCON_PIO_IE_CFG1_PB00_IE_MASK          (0x1U)
#define SYSCON_PIO_IE_CFG1_PB00_IE_SHIFT         (0U)
#define SYSCON_PIO_IE_CFG1_PB00_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG1_PB00_IE_SHIFT)) & SYSCON_PIO_IE_CFG1_PB00_IE_MASK)
#define SYSCON_PIO_IE_CFG1_PB01_IE_MASK          (0x2U)
#define SYSCON_PIO_IE_CFG1_PB01_IE_SHIFT         (1U)
#define SYSCON_PIO_IE_CFG1_PB01_IE(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG1_PB01_IE_SHIFT)) & SYSCON_PIO_IE_CFG1_PB01_IE_MASK)
#define SYSCON_PIO_IE_CFG1_BOOT_MODE_IE_MASK     (0x4U)
#define SYSCON_PIO_IE_CFG1_BOOT_MODE_IE_SHIFT    (2U)
#define SYSCON_PIO_IE_CFG1_BOOT_MODE_IE(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_IE_CFG1_BOOT_MODE_IE_SHIFT)) & SYSCON_PIO_IE_CFG1_BOOT_MODE_IE_MASK)

/*! @name PIO_FUNC_CFG - pin mux control register 0..pin mux control register 3 */
#define SYSCON_PIO_FUNC_CFG_PA00_FUNC_MASK       (0x7U)
#define SYSCON_PIO_FUNC_CFG_PA00_FUNC_SHIFT      (0U)
#define SYSCON_PIO_FUNC_CFG_PA00_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA00_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA00_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA24_FUNC_MASK       (0x7U)
#define SYSCON_PIO_FUNC_CFG_PA24_FUNC_SHIFT      (0U)
#define SYSCON_PIO_FUNC_CFG_PA24_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA24_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA24_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA16_FUNC_MASK       (0x7U)
#define SYSCON_PIO_FUNC_CFG_PA16_FUNC_SHIFT      (0U)
#define SYSCON_PIO_FUNC_CFG_PA16_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA16_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA16_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA08_FUNC_MASK       (0x7U)
#define SYSCON_PIO_FUNC_CFG_PA08_FUNC_SHIFT      (0U)
#define SYSCON_PIO_FUNC_CFG_PA08_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA08_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA08_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA25_FUNC_MASK       (0x70U)
#define SYSCON_PIO_FUNC_CFG_PA25_FUNC_SHIFT      (4U)
#define SYSCON_PIO_FUNC_CFG_PA25_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA25_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA25_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA17_FUNC_MASK       (0x70U)
#define SYSCON_PIO_FUNC_CFG_PA17_FUNC_SHIFT      (4U)
#define SYSCON_PIO_FUNC_CFG_PA17_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA17_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA17_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA09_FUNC_MASK       (0x70U)
#define SYSCON_PIO_FUNC_CFG_PA09_FUNC_SHIFT      (4U)
#define SYSCON_PIO_FUNC_CFG_PA09_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA09_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA09_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA01_FUNC_MASK       (0x70U)
#define SYSCON_PIO_FUNC_CFG_PA01_FUNC_SHIFT      (4U)
#define SYSCON_PIO_FUNC_CFG_PA01_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA01_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA01_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA10_FUNC_MASK       (0x700U)
#define SYSCON_PIO_FUNC_CFG_PA10_FUNC_SHIFT      (8U)
#define SYSCON_PIO_FUNC_CFG_PA10_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA10_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA10_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA02_FUNC_MASK       (0x700U)
#define SYSCON_PIO_FUNC_CFG_PA02_FUNC_SHIFT      (8U)
#define SYSCON_PIO_FUNC_CFG_PA02_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA02_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA02_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA18_FUNC_MASK       (0x700U)
#define SYSCON_PIO_FUNC_CFG_PA18_FUNC_SHIFT      (8U)
#define SYSCON_PIO_FUNC_CFG_PA18_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA18_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA18_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA26_FUNC_MASK       (0x700U)
#define SYSCON_PIO_FUNC_CFG_PA26_FUNC_SHIFT      (8U)
#define SYSCON_PIO_FUNC_CFG_PA26_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA26_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA26_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA27_FUNC_MASK       (0x7000U)
#define SYSCON_PIO_FUNC_CFG_PA27_FUNC_SHIFT      (12U)
#define SYSCON_PIO_FUNC_CFG_PA27_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA27_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA27_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA11_FUNC_MASK       (0x7000U)
#define SYSCON_PIO_FUNC_CFG_PA11_FUNC_SHIFT      (12U)
#define SYSCON_PIO_FUNC_CFG_PA11_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA11_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA11_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA03_FUNC_MASK       (0x7000U)
#define SYSCON_PIO_FUNC_CFG_PA03_FUNC_SHIFT      (12U)
#define SYSCON_PIO_FUNC_CFG_PA03_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA03_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA03_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA19_FUNC_MASK       (0x7000U)
#define SYSCON_PIO_FUNC_CFG_PA19_FUNC_SHIFT      (12U)
#define SYSCON_PIO_FUNC_CFG_PA19_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA19_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA19_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA20_FUNC_MASK       (0x70000U)
#define SYSCON_PIO_FUNC_CFG_PA20_FUNC_SHIFT      (16U)
#define SYSCON_PIO_FUNC_CFG_PA20_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA20_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA20_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA12_FUNC_MASK       (0x70000U)
#define SYSCON_PIO_FUNC_CFG_PA12_FUNC_SHIFT      (16U)
#define SYSCON_PIO_FUNC_CFG_PA12_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA12_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA12_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA28_FUNC_MASK       (0x70000U)
#define SYSCON_PIO_FUNC_CFG_PA28_FUNC_SHIFT      (16U)
#define SYSCON_PIO_FUNC_CFG_PA28_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA28_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA28_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA04_FUNC_MASK       (0x70000U)
#define SYSCON_PIO_FUNC_CFG_PA04_FUNC_SHIFT      (16U)
#define SYSCON_PIO_FUNC_CFG_PA04_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA04_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA04_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA05_FUNC_MASK       (0x700000U)
#define SYSCON_PIO_FUNC_CFG_PA05_FUNC_SHIFT      (20U)
#define SYSCON_PIO_FUNC_CFG_PA05_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA05_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA05_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA29_FUNC_MASK       (0x700000U)
#define SYSCON_PIO_FUNC_CFG_PA29_FUNC_SHIFT      (20U)
#define SYSCON_PIO_FUNC_CFG_PA29_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA29_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA29_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA13_FUNC_MASK       (0x700000U)
#define SYSCON_PIO_FUNC_CFG_PA13_FUNC_SHIFT      (20U)
#define SYSCON_PIO_FUNC_CFG_PA13_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA13_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA13_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA21_FUNC_MASK       (0x700000U)
#define SYSCON_PIO_FUNC_CFG_PA21_FUNC_SHIFT      (20U)
#define SYSCON_PIO_FUNC_CFG_PA21_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA21_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA21_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA06_FUNC_MASK       (0x7000000U)
#define SYSCON_PIO_FUNC_CFG_PA06_FUNC_SHIFT      (24U)
#define SYSCON_PIO_FUNC_CFG_PA06_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA06_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA06_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA22_FUNC_MASK       (0x7000000U)
#define SYSCON_PIO_FUNC_CFG_PA22_FUNC_SHIFT      (24U)
#define SYSCON_PIO_FUNC_CFG_PA22_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA22_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA22_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA30_FUNC_MASK       (0x7000000U)
#define SYSCON_PIO_FUNC_CFG_PA30_FUNC_SHIFT      (24U)
#define SYSCON_PIO_FUNC_CFG_PA30_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA30_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA30_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA14_FUNC_MASK       (0x7000000U)
#define SYSCON_PIO_FUNC_CFG_PA14_FUNC_SHIFT      (24U)
#define SYSCON_PIO_FUNC_CFG_PA14_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA14_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA14_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA31_FUNC_MASK       (0x70000000U)
#define SYSCON_PIO_FUNC_CFG_PA31_FUNC_SHIFT      (28U)
#define SYSCON_PIO_FUNC_CFG_PA31_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA31_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA31_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA07_FUNC_MASK       (0x70000000U)
#define SYSCON_PIO_FUNC_CFG_PA07_FUNC_SHIFT      (28U)
#define SYSCON_PIO_FUNC_CFG_PA07_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA07_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA07_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA23_FUNC_MASK       (0x70000000U)
#define SYSCON_PIO_FUNC_CFG_PA23_FUNC_SHIFT      (28U)
#define SYSCON_PIO_FUNC_CFG_PA23_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA23_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA23_FUNC_MASK)
#define SYSCON_PIO_FUNC_CFG_PA15_FUNC_MASK       (0x70000000U)
#define SYSCON_PIO_FUNC_CFG_PA15_FUNC_SHIFT      (28U)
#define SYSCON_PIO_FUNC_CFG_PA15_FUNC(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_FUNC_CFG_PA15_FUNC_SHIFT)) & SYSCON_PIO_FUNC_CFG_PA15_FUNC_MASK)

/* The count of SYSCON_PIO_FUNC_CFG */
#define SYSCON_PIO_FUNC_CFG_COUNT                (4U)

/*! @name PIO_WAKEUP_EN0 - pin function selection in power down mode register 0 */
#define SYSCON_PIO_WAKEUP_EN0_PA00_WAKEUP_EN_MASK (0x1U)
#define SYSCON_PIO_WAKEUP_EN0_PA00_WAKEUP_EN_SHIFT (0U)
#define SYSCON_PIO_WAKEUP_EN0_PA00_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA00_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA00_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA01_WAKEUP_EN_MASK (0x2U)
#define SYSCON_PIO_WAKEUP_EN0_PA01_WAKEUP_EN_SHIFT (1U)
#define SYSCON_PIO_WAKEUP_EN0_PA01_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA01_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA01_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA02_WAKEUP_EN_MASK (0x4U)
#define SYSCON_PIO_WAKEUP_EN0_PA02_WAKEUP_EN_SHIFT (2U)
#define SYSCON_PIO_WAKEUP_EN0_PA02_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA02_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA02_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA03_WAKEUP_EN_MASK (0x8U)
#define SYSCON_PIO_WAKEUP_EN0_PA03_WAKEUP_EN_SHIFT (3U)
#define SYSCON_PIO_WAKEUP_EN0_PA03_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA03_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA03_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA04_WAKEUP_EN_MASK (0x10U)
#define SYSCON_PIO_WAKEUP_EN0_PA04_WAKEUP_EN_SHIFT (4U)
#define SYSCON_PIO_WAKEUP_EN0_PA04_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA04_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA04_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA05_WAKEUP_EN_MASK (0x20U)
#define SYSCON_PIO_WAKEUP_EN0_PA05_WAKEUP_EN_SHIFT (5U)
#define SYSCON_PIO_WAKEUP_EN0_PA05_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA05_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA05_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA06_WAKEUP_EN_MASK (0x40U)
#define SYSCON_PIO_WAKEUP_EN0_PA06_WAKEUP_EN_SHIFT (6U)
#define SYSCON_PIO_WAKEUP_EN0_PA06_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA06_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA06_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA07_WAKEUP_EN_MASK (0x80U)
#define SYSCON_PIO_WAKEUP_EN0_PA07_WAKEUP_EN_SHIFT (7U)
#define SYSCON_PIO_WAKEUP_EN0_PA07_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA07_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA07_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA08_WAKEUP_EN_MASK (0x100U)
#define SYSCON_PIO_WAKEUP_EN0_PA08_WAKEUP_EN_SHIFT (8U)
#define SYSCON_PIO_WAKEUP_EN0_PA08_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA08_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA08_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA09_WAKEUP_EN_MASK (0x200U)
#define SYSCON_PIO_WAKEUP_EN0_PA09_WAKEUP_EN_SHIFT (9U)
#define SYSCON_PIO_WAKEUP_EN0_PA09_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA09_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA09_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA10_WAKEUP_EN_MASK (0x400U)
#define SYSCON_PIO_WAKEUP_EN0_PA10_WAKEUP_EN_SHIFT (10U)
#define SYSCON_PIO_WAKEUP_EN0_PA10_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA10_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA10_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA11_WAKEUP_EN_MASK (0x800U)
#define SYSCON_PIO_WAKEUP_EN0_PA11_WAKEUP_EN_SHIFT (11U)
#define SYSCON_PIO_WAKEUP_EN0_PA11_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA11_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA11_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA12_WAKEUP_EN_MASK (0x1000U)
#define SYSCON_PIO_WAKEUP_EN0_PA12_WAKEUP_EN_SHIFT (12U)
#define SYSCON_PIO_WAKEUP_EN0_PA12_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA12_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA12_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA13_WAKEUP_EN_MASK (0x2000U)
#define SYSCON_PIO_WAKEUP_EN0_PA13_WAKEUP_EN_SHIFT (13U)
#define SYSCON_PIO_WAKEUP_EN0_PA13_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA13_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA13_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA14_WAKEUP_EN_MASK (0x4000U)
#define SYSCON_PIO_WAKEUP_EN0_PA14_WAKEUP_EN_SHIFT (14U)
#define SYSCON_PIO_WAKEUP_EN0_PA14_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA14_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA14_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA15_WAKEUP_EN_MASK (0x8000U)
#define SYSCON_PIO_WAKEUP_EN0_PA15_WAKEUP_EN_SHIFT (15U)
#define SYSCON_PIO_WAKEUP_EN0_PA15_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA15_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA15_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA16_WAKEUP_EN_MASK (0x10000U)
#define SYSCON_PIO_WAKEUP_EN0_PA16_WAKEUP_EN_SHIFT (16U)
#define SYSCON_PIO_WAKEUP_EN0_PA16_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA16_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA16_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA17_WAKEUP_EN_MASK (0x20000U)
#define SYSCON_PIO_WAKEUP_EN0_PA17_WAKEUP_EN_SHIFT (17U)
#define SYSCON_PIO_WAKEUP_EN0_PA17_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA17_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA17_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA18_WAKEUP_EN_MASK (0x40000U)
#define SYSCON_PIO_WAKEUP_EN0_PA18_WAKEUP_EN_SHIFT (18U)
#define SYSCON_PIO_WAKEUP_EN0_PA18_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA18_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA18_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA19_WAKEUP_EN_MASK (0x80000U)
#define SYSCON_PIO_WAKEUP_EN0_PA19_WAKEUP_EN_SHIFT (19U)
#define SYSCON_PIO_WAKEUP_EN0_PA19_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA19_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA19_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA20_WAKEUP_EN_MASK (0x100000U)
#define SYSCON_PIO_WAKEUP_EN0_PA20_WAKEUP_EN_SHIFT (20U)
#define SYSCON_PIO_WAKEUP_EN0_PA20_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA20_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA20_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA21_WAKEUP_EN_MASK (0x200000U)
#define SYSCON_PIO_WAKEUP_EN0_PA21_WAKEUP_EN_SHIFT (21U)
#define SYSCON_PIO_WAKEUP_EN0_PA21_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA21_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA21_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA22_WAKEUP_EN_MASK (0x400000U)
#define SYSCON_PIO_WAKEUP_EN0_PA22_WAKEUP_EN_SHIFT (22U)
#define SYSCON_PIO_WAKEUP_EN0_PA22_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA22_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA22_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA23_WAKEUP_EN_MASK (0x800000U)
#define SYSCON_PIO_WAKEUP_EN0_PA23_WAKEUP_EN_SHIFT (23U)
#define SYSCON_PIO_WAKEUP_EN0_PA23_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA23_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA23_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA24_WAKEUP_EN_MASK (0x1000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA24_WAKEUP_EN_SHIFT (24U)
#define SYSCON_PIO_WAKEUP_EN0_PA24_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA24_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA24_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA25_WAKEUP_EN_MASK (0x2000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA25_WAKEUP_EN_SHIFT (25U)
#define SYSCON_PIO_WAKEUP_EN0_PA25_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA25_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA25_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA26_WAKEUP_EN_MASK (0x4000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA26_WAKEUP_EN_SHIFT (26U)
#define SYSCON_PIO_WAKEUP_EN0_PA26_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA26_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA26_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA27_WAKEUP_EN_MASK (0x8000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA27_WAKEUP_EN_SHIFT (27U)
#define SYSCON_PIO_WAKEUP_EN0_PA27_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA27_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA27_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA28_WAKEUP_EN_MASK (0x10000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA28_WAKEUP_EN_SHIFT (28U)
#define SYSCON_PIO_WAKEUP_EN0_PA28_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA28_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA28_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA29_WAKEUP_EN_MASK (0x20000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA29_WAKEUP_EN_SHIFT (29U)
#define SYSCON_PIO_WAKEUP_EN0_PA29_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA29_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA29_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA30_WAKEUP_EN_MASK (0x40000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA30_WAKEUP_EN_SHIFT (30U)
#define SYSCON_PIO_WAKEUP_EN0_PA30_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA30_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA30_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN0_PA31_WAKEUP_EN_MASK (0x80000000U)
#define SYSCON_PIO_WAKEUP_EN0_PA31_WAKEUP_EN_SHIFT (31U)
#define SYSCON_PIO_WAKEUP_EN0_PA31_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN0_PA31_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN0_PA31_WAKEUP_EN_MASK)

/*! @name PIO_WAKEUP_EN1 - pin function selection in power down mode register 1 */
#define SYSCON_PIO_WAKEUP_EN1_PB00_WAKEUP_EN_MASK (0x1U)
#define SYSCON_PIO_WAKEUP_EN1_PB00_WAKEUP_EN_SHIFT (0U)
#define SYSCON_PIO_WAKEUP_EN1_PB00_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PB00_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PB00_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PB01_WAKEUP_EN_MASK (0x2U)
#define SYSCON_PIO_WAKEUP_EN1_PB01_WAKEUP_EN_SHIFT (1U)
#define SYSCON_PIO_WAKEUP_EN1_PB01_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PB01_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PB01_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PB02_WAKEUP_EN_MASK (0x4U)
#define SYSCON_PIO_WAKEUP_EN1_PB02_WAKEUP_EN_SHIFT (2U)
#define SYSCON_PIO_WAKEUP_EN1_PB02_WAKEUP_EN(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PB02_WAKEUP_EN_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PB02_WAKEUP_EN_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA04_32K_OE_MASK   (0x10U)
#define SYSCON_PIO_WAKEUP_EN1_PA04_32K_OE_SHIFT  (4U)
#define SYSCON_PIO_WAKEUP_EN1_PA04_32K_OE(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA04_32K_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA04_32K_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA05_XTAL_OE_MASK  (0x20U)
#define SYSCON_PIO_WAKEUP_EN1_PA05_XTAL_OE_SHIFT (5U)
#define SYSCON_PIO_WAKEUP_EN1_PA05_XTAL_OE(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA05_XTAL_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA05_XTAL_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA10_32K_OE_MASK   (0x400U)
#define SYSCON_PIO_WAKEUP_EN1_PA10_32K_OE_SHIFT  (10U)
#define SYSCON_PIO_WAKEUP_EN1_PA10_32K_OE(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA10_32K_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA10_32K_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA11_XTAL_OE_MASK  (0x800U)
#define SYSCON_PIO_WAKEUP_EN1_PA11_XTAL_OE_SHIFT (11U)
#define SYSCON_PIO_WAKEUP_EN1_PA11_XTAL_OE(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA11_XTAL_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA11_XTAL_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA18_32K_OE_MASK   (0x40000U)
#define SYSCON_PIO_WAKEUP_EN1_PA18_32K_OE_SHIFT  (18U)
#define SYSCON_PIO_WAKEUP_EN1_PA18_32K_OE(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA18_32K_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA18_32K_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA19_XTAL_OE_MASK  (0x80000U)
#define SYSCON_PIO_WAKEUP_EN1_PA19_XTAL_OE_SHIFT (19U)
#define SYSCON_PIO_WAKEUP_EN1_PA19_XTAL_OE(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA19_XTAL_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA19_XTAL_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA24_32K_OE_MASK   (0x1000000U)
#define SYSCON_PIO_WAKEUP_EN1_PA24_32K_OE_SHIFT  (24U)
#define SYSCON_PIO_WAKEUP_EN1_PA24_32K_OE(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA24_32K_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA24_32K_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PA25_XTAL_OE_MASK  (0x2000000U)
#define SYSCON_PIO_WAKEUP_EN1_PA25_XTAL_OE_SHIFT (25U)
#define SYSCON_PIO_WAKEUP_EN1_PA25_XTAL_OE(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PA25_XTAL_OE_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PA25_XTAL_OE_MASK)
#define SYSCON_PIO_WAKEUP_EN1_PDM_IO_SEL_MASK    (0x80000000U)
#define SYSCON_PIO_WAKEUP_EN1_PDM_IO_SEL_SHIFT   (31U)
#define SYSCON_PIO_WAKEUP_EN1_PDM_IO_SEL(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_WAKEUP_EN1_PDM_IO_SEL_SHIFT)) & SYSCON_PIO_WAKEUP_EN1_PDM_IO_SEL_MASK)

/*! @name PIO_CAP_OE0 - pin output enable status register 0 while captured by writing 1 to IO_CAP */
#define SYSCON_PIO_CAP_OE0_PA00_CAP_OE_MASK      (0x1U)
#define SYSCON_PIO_CAP_OE0_PA00_CAP_OE_SHIFT     (0U)
#define SYSCON_PIO_CAP_OE0_PA00_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA00_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA00_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA01_CAP_OE_MASK      (0x2U)
#define SYSCON_PIO_CAP_OE0_PA01_CAP_OE_SHIFT     (1U)
#define SYSCON_PIO_CAP_OE0_PA01_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA01_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA01_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA02_CAP_OE_MASK      (0x4U)
#define SYSCON_PIO_CAP_OE0_PA02_CAP_OE_SHIFT     (2U)
#define SYSCON_PIO_CAP_OE0_PA02_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA02_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA02_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA03_CAP_OE_MASK      (0x8U)
#define SYSCON_PIO_CAP_OE0_PA03_CAP_OE_SHIFT     (3U)
#define SYSCON_PIO_CAP_OE0_PA03_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA03_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA03_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA04_CAP_OE_MASK      (0x10U)
#define SYSCON_PIO_CAP_OE0_PA04_CAP_OE_SHIFT     (4U)
#define SYSCON_PIO_CAP_OE0_PA04_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA04_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA04_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA05_CAP_OE_MASK      (0x20U)
#define SYSCON_PIO_CAP_OE0_PA05_CAP_OE_SHIFT     (5U)
#define SYSCON_PIO_CAP_OE0_PA05_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA05_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA05_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA06_CAP_OE_MASK      (0x40U)
#define SYSCON_PIO_CAP_OE0_PA06_CAP_OE_SHIFT     (6U)
#define SYSCON_PIO_CAP_OE0_PA06_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA06_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA06_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA07_CAP_OE_MASK      (0x80U)
#define SYSCON_PIO_CAP_OE0_PA07_CAP_OE_SHIFT     (7U)
#define SYSCON_PIO_CAP_OE0_PA07_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA07_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA07_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA08_CAP_OE_MASK      (0x100U)
#define SYSCON_PIO_CAP_OE0_PA08_CAP_OE_SHIFT     (8U)
#define SYSCON_PIO_CAP_OE0_PA08_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA08_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA08_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA09_CAP_OE_MASK      (0x200U)
#define SYSCON_PIO_CAP_OE0_PA09_CAP_OE_SHIFT     (9U)
#define SYSCON_PIO_CAP_OE0_PA09_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA09_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA09_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA10_CAP_OE_MASK      (0x400U)
#define SYSCON_PIO_CAP_OE0_PA10_CAP_OE_SHIFT     (10U)
#define SYSCON_PIO_CAP_OE0_PA10_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA10_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA10_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA11_CAP_OE_MASK      (0x800U)
#define SYSCON_PIO_CAP_OE0_PA11_CAP_OE_SHIFT     (11U)
#define SYSCON_PIO_CAP_OE0_PA11_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA11_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA11_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA12_CAP_OE_MASK      (0x1000U)
#define SYSCON_PIO_CAP_OE0_PA12_CAP_OE_SHIFT     (12U)
#define SYSCON_PIO_CAP_OE0_PA12_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA12_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA12_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA13_CAP_OE_MASK      (0x2000U)
#define SYSCON_PIO_CAP_OE0_PA13_CAP_OE_SHIFT     (13U)
#define SYSCON_PIO_CAP_OE0_PA13_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA13_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA13_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA14_CAP_OE_MASK      (0x4000U)
#define SYSCON_PIO_CAP_OE0_PA14_CAP_OE_SHIFT     (14U)
#define SYSCON_PIO_CAP_OE0_PA14_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA14_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA14_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA15_CAP_OE_MASK      (0x8000U)
#define SYSCON_PIO_CAP_OE0_PA15_CAP_OE_SHIFT     (15U)
#define SYSCON_PIO_CAP_OE0_PA15_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA15_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA15_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA16_CAP_OE_MASK      (0x10000U)
#define SYSCON_PIO_CAP_OE0_PA16_CAP_OE_SHIFT     (16U)
#define SYSCON_PIO_CAP_OE0_PA16_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA16_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA16_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA17_CAP_OE_MASK      (0x20000U)
#define SYSCON_PIO_CAP_OE0_PA17_CAP_OE_SHIFT     (17U)
#define SYSCON_PIO_CAP_OE0_PA17_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA17_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA17_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA18_CAP_OE_MASK      (0x40000U)
#define SYSCON_PIO_CAP_OE0_PA18_CAP_OE_SHIFT     (18U)
#define SYSCON_PIO_CAP_OE0_PA18_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA18_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA18_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA19_CAP_OE_MASK      (0x80000U)
#define SYSCON_PIO_CAP_OE0_PA19_CAP_OE_SHIFT     (19U)
#define SYSCON_PIO_CAP_OE0_PA19_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA19_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA19_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA20_CAP_OE_MASK      (0x100000U)
#define SYSCON_PIO_CAP_OE0_PA20_CAP_OE_SHIFT     (20U)
#define SYSCON_PIO_CAP_OE0_PA20_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA20_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA20_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA21_CAP_OE_MASK      (0x200000U)
#define SYSCON_PIO_CAP_OE0_PA21_CAP_OE_SHIFT     (21U)
#define SYSCON_PIO_CAP_OE0_PA21_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA21_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA21_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA22_CAP_OE_MASK      (0x400000U)
#define SYSCON_PIO_CAP_OE0_PA22_CAP_OE_SHIFT     (22U)
#define SYSCON_PIO_CAP_OE0_PA22_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA22_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA22_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA23_CAP_OE_MASK      (0x800000U)
#define SYSCON_PIO_CAP_OE0_PA23_CAP_OE_SHIFT     (23U)
#define SYSCON_PIO_CAP_OE0_PA23_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA23_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA23_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA24_CAP_OE_MASK      (0x1000000U)
#define SYSCON_PIO_CAP_OE0_PA24_CAP_OE_SHIFT     (24U)
#define SYSCON_PIO_CAP_OE0_PA24_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA24_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA24_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA25_CAP_OE_MASK      (0x2000000U)
#define SYSCON_PIO_CAP_OE0_PA25_CAP_OE_SHIFT     (25U)
#define SYSCON_PIO_CAP_OE0_PA25_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA25_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA25_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA26_CAP_OE_MASK      (0x4000000U)
#define SYSCON_PIO_CAP_OE0_PA26_CAP_OE_SHIFT     (26U)
#define SYSCON_PIO_CAP_OE0_PA26_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA26_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA26_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA27_CAP_OE_MASK      (0x8000000U)
#define SYSCON_PIO_CAP_OE0_PA27_CAP_OE_SHIFT     (27U)
#define SYSCON_PIO_CAP_OE0_PA27_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA27_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA27_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA28_CAP_OE_MASK      (0x10000000U)
#define SYSCON_PIO_CAP_OE0_PA28_CAP_OE_SHIFT     (28U)
#define SYSCON_PIO_CAP_OE0_PA28_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA28_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA28_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA29_CAP_OE_MASK      (0x20000000U)
#define SYSCON_PIO_CAP_OE0_PA29_CAP_OE_SHIFT     (29U)
#define SYSCON_PIO_CAP_OE0_PA29_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA29_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA29_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA30_CAP_OE_MASK      (0x40000000U)
#define SYSCON_PIO_CAP_OE0_PA30_CAP_OE_SHIFT     (30U)
#define SYSCON_PIO_CAP_OE0_PA30_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA30_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA30_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE0_PA31_CAP_OE_MASK      (0x80000000U)
#define SYSCON_PIO_CAP_OE0_PA31_CAP_OE_SHIFT     (31U)
#define SYSCON_PIO_CAP_OE0_PA31_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE0_PA31_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE0_PA31_CAP_OE_MASK)

/*! @name PIO_CAP_OE1 - pin output enable status register 1 while captured by writing 1 to IO_CAP */
#define SYSCON_PIO_CAP_OE1_PB00_CAP_OE_MASK      (0x1U)
#define SYSCON_PIO_CAP_OE1_PB00_CAP_OE_SHIFT     (0U)
#define SYSCON_PIO_CAP_OE1_PB00_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE1_PB00_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE1_PB00_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE1_PB01_CAP_OE_MASK      (0x2U)
#define SYSCON_PIO_CAP_OE1_PB01_CAP_OE_SHIFT     (1U)
#define SYSCON_PIO_CAP_OE1_PB01_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE1_PB01_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE1_PB01_CAP_OE_MASK)
#define SYSCON_PIO_CAP_OE1_PB02_CAP_OE_MASK      (0x4U)
#define SYSCON_PIO_CAP_OE1_PB02_CAP_OE_SHIFT     (2U)
#define SYSCON_PIO_CAP_OE1_PB02_CAP_OE(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OE1_PB02_CAP_OE_SHIFT)) & SYSCON_PIO_CAP_OE1_PB02_CAP_OE_MASK)

/*! @name PIO_CAP_OUT0 - pin output status register 0 while captured by writing 1 to IO_CAP */
#define SYSCON_PIO_CAP_OUT0_PA00_CAP_OUT_MASK    (0x1U)
#define SYSCON_PIO_CAP_OUT0_PA00_CAP_OUT_SHIFT   (0U)
#define SYSCON_PIO_CAP_OUT0_PA00_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA00_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA00_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA01_CAP_OUT_MASK    (0x2U)
#define SYSCON_PIO_CAP_OUT0_PA01_CAP_OUT_SHIFT   (1U)
#define SYSCON_PIO_CAP_OUT0_PA01_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA01_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA01_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA02_CAP_OUT_MASK    (0x4U)
#define SYSCON_PIO_CAP_OUT0_PA02_CAP_OUT_SHIFT   (2U)
#define SYSCON_PIO_CAP_OUT0_PA02_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA02_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA02_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA03_CAP_OUT_MASK    (0x8U)
#define SYSCON_PIO_CAP_OUT0_PA03_CAP_OUT_SHIFT   (3U)
#define SYSCON_PIO_CAP_OUT0_PA03_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA03_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA03_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA04_CAP_OUT_MASK    (0x10U)
#define SYSCON_PIO_CAP_OUT0_PA04_CAP_OUT_SHIFT   (4U)
#define SYSCON_PIO_CAP_OUT0_PA04_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA04_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA04_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA05_CAP_OUT_MASK    (0x20U)
#define SYSCON_PIO_CAP_OUT0_PA05_CAP_OUT_SHIFT   (5U)
#define SYSCON_PIO_CAP_OUT0_PA05_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA05_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA05_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA06_CAP_OUT_MASK    (0x40U)
#define SYSCON_PIO_CAP_OUT0_PA06_CAP_OUT_SHIFT   (6U)
#define SYSCON_PIO_CAP_OUT0_PA06_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA06_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA06_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA07_CAP_OUT_MASK    (0x80U)
#define SYSCON_PIO_CAP_OUT0_PA07_CAP_OUT_SHIFT   (7U)
#define SYSCON_PIO_CAP_OUT0_PA07_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA07_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA07_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA08_CAP_OUT_MASK    (0x100U)
#define SYSCON_PIO_CAP_OUT0_PA08_CAP_OUT_SHIFT   (8U)
#define SYSCON_PIO_CAP_OUT0_PA08_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA08_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA08_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA09_CAP_OUT_MASK    (0x200U)
#define SYSCON_PIO_CAP_OUT0_PA09_CAP_OUT_SHIFT   (9U)
#define SYSCON_PIO_CAP_OUT0_PA09_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA09_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA09_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA10_CAP_OUT_MASK    (0x400U)
#define SYSCON_PIO_CAP_OUT0_PA10_CAP_OUT_SHIFT   (10U)
#define SYSCON_PIO_CAP_OUT0_PA10_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA10_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA10_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA11_CAP_OUT_MASK    (0x800U)
#define SYSCON_PIO_CAP_OUT0_PA11_CAP_OUT_SHIFT   (11U)
#define SYSCON_PIO_CAP_OUT0_PA11_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA11_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA11_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA12_CAP_OUT_MASK    (0x1000U)
#define SYSCON_PIO_CAP_OUT0_PA12_CAP_OUT_SHIFT   (12U)
#define SYSCON_PIO_CAP_OUT0_PA12_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA12_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA12_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA13_CAP_OUT_MASK    (0x2000U)
#define SYSCON_PIO_CAP_OUT0_PA13_CAP_OUT_SHIFT   (13U)
#define SYSCON_PIO_CAP_OUT0_PA13_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA13_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA13_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA14_CAP_OUT_MASK    (0x4000U)
#define SYSCON_PIO_CAP_OUT0_PA14_CAP_OUT_SHIFT   (14U)
#define SYSCON_PIO_CAP_OUT0_PA14_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA14_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA14_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA15_CAP_OUT_MASK    (0x8000U)
#define SYSCON_PIO_CAP_OUT0_PA15_CAP_OUT_SHIFT   (15U)
#define SYSCON_PIO_CAP_OUT0_PA15_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA15_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA15_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA16_CAP_OUT_MASK    (0x10000U)
#define SYSCON_PIO_CAP_OUT0_PA16_CAP_OUT_SHIFT   (16U)
#define SYSCON_PIO_CAP_OUT0_PA16_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA16_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA16_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA17_CAP_OUT_MASK    (0x20000U)
#define SYSCON_PIO_CAP_OUT0_PA17_CAP_OUT_SHIFT   (17U)
#define SYSCON_PIO_CAP_OUT0_PA17_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA17_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA17_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA18_CAP_OUT_MASK    (0x40000U)
#define SYSCON_PIO_CAP_OUT0_PA18_CAP_OUT_SHIFT   (18U)
#define SYSCON_PIO_CAP_OUT0_PA18_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA18_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA18_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA19_CAP_OUT_MASK    (0x80000U)
#define SYSCON_PIO_CAP_OUT0_PA19_CAP_OUT_SHIFT   (19U)
#define SYSCON_PIO_CAP_OUT0_PA19_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA19_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA19_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA20_CAP_OUT_MASK    (0x100000U)
#define SYSCON_PIO_CAP_OUT0_PA20_CAP_OUT_SHIFT   (20U)
#define SYSCON_PIO_CAP_OUT0_PA20_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA20_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA20_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA21_CAP_OUT_MASK    (0x200000U)
#define SYSCON_PIO_CAP_OUT0_PA21_CAP_OUT_SHIFT   (21U)
#define SYSCON_PIO_CAP_OUT0_PA21_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA21_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA21_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA22_CAP_OUT_MASK    (0x400000U)
#define SYSCON_PIO_CAP_OUT0_PA22_CAP_OUT_SHIFT   (22U)
#define SYSCON_PIO_CAP_OUT0_PA22_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA22_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA22_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA23_CAP_OUT_MASK    (0x800000U)
#define SYSCON_PIO_CAP_OUT0_PA23_CAP_OUT_SHIFT   (23U)
#define SYSCON_PIO_CAP_OUT0_PA23_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA23_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA23_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA24_CAP_OUT_MASK    (0x1000000U)
#define SYSCON_PIO_CAP_OUT0_PA24_CAP_OUT_SHIFT   (24U)
#define SYSCON_PIO_CAP_OUT0_PA24_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA24_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA24_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA25_CAP_OUT_MASK    (0x2000000U)
#define SYSCON_PIO_CAP_OUT0_PA25_CAP_OUT_SHIFT   (25U)
#define SYSCON_PIO_CAP_OUT0_PA25_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA25_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA25_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA26_CAP_OUT_MASK    (0x4000000U)
#define SYSCON_PIO_CAP_OUT0_PA26_CAP_OUT_SHIFT   (26U)
#define SYSCON_PIO_CAP_OUT0_PA26_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA26_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA26_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA27_CAP_OUT_MASK    (0x8000000U)
#define SYSCON_PIO_CAP_OUT0_PA27_CAP_OUT_SHIFT   (27U)
#define SYSCON_PIO_CAP_OUT0_PA27_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA27_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA27_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA28_CAP_OUT_MASK    (0x10000000U)
#define SYSCON_PIO_CAP_OUT0_PA28_CAP_OUT_SHIFT   (28U)
#define SYSCON_PIO_CAP_OUT0_PA28_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA28_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA28_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA29_CAP_OUT_MASK    (0x20000000U)
#define SYSCON_PIO_CAP_OUT0_PA29_CAP_OUT_SHIFT   (29U)
#define SYSCON_PIO_CAP_OUT0_PA29_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA29_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA29_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA30_CAP_OUT_MASK    (0x40000000U)
#define SYSCON_PIO_CAP_OUT0_PA30_CAP_OUT_SHIFT   (30U)
#define SYSCON_PIO_CAP_OUT0_PA30_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA30_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA30_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT0_PA31_CAP_OUT_MASK    (0x80000000U)
#define SYSCON_PIO_CAP_OUT0_PA31_CAP_OUT_SHIFT   (31U)
#define SYSCON_PIO_CAP_OUT0_PA31_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT0_PA31_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT0_PA31_CAP_OUT_MASK)

/*! @name PIO_CAP_OUT1 - pin output status register 0 while captured by writing 1 to IO_CAP */
#define SYSCON_PIO_CAP_OUT1_PB00_CAP_OUT_MASK    (0x1U)
#define SYSCON_PIO_CAP_OUT1_PB00_CAP_OUT_SHIFT   (0U)
#define SYSCON_PIO_CAP_OUT1_PB00_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT1_PB00_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT1_PB00_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT1_PB01_CAP_OUT_MASK    (0x2U)
#define SYSCON_PIO_CAP_OUT1_PB01_CAP_OUT_SHIFT   (1U)
#define SYSCON_PIO_CAP_OUT1_PB01_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT1_PB01_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT1_PB01_CAP_OUT_MASK)
#define SYSCON_PIO_CAP_OUT1_PB02_CAP_OUT_MASK    (0x4U)
#define SYSCON_PIO_CAP_OUT1_PB02_CAP_OUT_SHIFT   (2U)
#define SYSCON_PIO_CAP_OUT1_PB02_CAP_OUT(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PIO_CAP_OUT1_PB02_CAP_OUT_SHIFT)) & SYSCON_PIO_CAP_OUT1_PB02_CAP_OUT_MASK)

/*! @name RST_CAUSE_SRC - reset source status register */
#define SYSCON_RST_CAUSE_SRC_RESET_CAUSE_MASK    (0x1FFU)
#define SYSCON_RST_CAUSE_SRC_RESET_CAUSE_SHIFT   (0U)
#define SYSCON_RST_CAUSE_SRC_RESET_CAUSE(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_CAUSE_SRC_RESET_CAUSE_SHIFT)) & SYSCON_RST_CAUSE_SRC_RESET_CAUSE_MASK)
#define SYSCON_RST_CAUSE_SRC_RST_CAUSE_CLR_MASK  (0x80000000U)
#define SYSCON_RST_CAUSE_SRC_RST_CAUSE_CLR_SHIFT (31U)
#define SYSCON_RST_CAUSE_SRC_RST_CAUSE_CLR(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_RST_CAUSE_SRC_RST_CAUSE_CLR_SHIFT)) & SYSCON_RST_CAUSE_SRC_RST_CAUSE_CLR_MASK)

/*! @name PMU_CTRL0 - power management uinit control register 0 */
#define SYSCON_PMU_CTRL0_MEM0_DIS_MASK           (0x1U)
#define SYSCON_PMU_CTRL0_MEM0_DIS_SHIFT          (0U)
#define SYSCON_PMU_CTRL0_MEM0_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM0_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM0_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM1_DIS_MASK           (0x2U)
#define SYSCON_PMU_CTRL0_MEM1_DIS_SHIFT          (1U)
#define SYSCON_PMU_CTRL0_MEM1_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM1_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM1_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM2_DIS_MASK           (0x4U)
#define SYSCON_PMU_CTRL0_MEM2_DIS_SHIFT          (2U)
#define SYSCON_PMU_CTRL0_MEM2_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM2_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM2_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM3_DIS_MASK           (0x8U)
#define SYSCON_PMU_CTRL0_MEM3_DIS_SHIFT          (3U)
#define SYSCON_PMU_CTRL0_MEM3_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM3_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM3_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM4_DIS_MASK           (0x10U)
#define SYSCON_PMU_CTRL0_MEM4_DIS_SHIFT          (4U)
#define SYSCON_PMU_CTRL0_MEM4_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM4_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM4_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM5_DIS_MASK           (0x20U)
#define SYSCON_PMU_CTRL0_MEM5_DIS_SHIFT          (5U)
#define SYSCON_PMU_CTRL0_MEM5_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM5_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM5_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM6_DIS_MASK           (0x40U)
#define SYSCON_PMU_CTRL0_MEM6_DIS_SHIFT          (6U)
#define SYSCON_PMU_CTRL0_MEM6_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM6_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM6_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM7_DIS_MASK           (0x80U)
#define SYSCON_PMU_CTRL0_MEM7_DIS_SHIFT          (7U)
#define SYSCON_PMU_CTRL0_MEM7_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM7_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM7_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM8_DIS_MASK           (0x100U)
#define SYSCON_PMU_CTRL0_MEM8_DIS_SHIFT          (8U)
#define SYSCON_PMU_CTRL0_MEM8_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM8_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM8_DIS_MASK)
#define SYSCON_PMU_CTRL0_MEM9_DIS_MASK           (0x200U)
#define SYSCON_PMU_CTRL0_MEM9_DIS_SHIFT          (9U)
#define SYSCON_PMU_CTRL0_MEM9_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MEM9_DIS_SHIFT)) & SYSCON_PMU_CTRL0_MEM9_DIS_MASK)
#define SYSCON_PMU_CTRL0_BLE_DIS_MASK            (0x10000U)
#define SYSCON_PMU_CTRL0_BLE_DIS_SHIFT           (16U)
#define SYSCON_PMU_CTRL0_BLE_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_BLE_DIS_SHIFT)) & SYSCON_PMU_CTRL0_BLE_DIS_MASK)
#define SYSCON_PMU_CTRL0_FIR_DIS_MASK            (0x20000U)
#define SYSCON_PMU_CTRL0_FIR_DIS_SHIFT           (17U)
#define SYSCON_PMU_CTRL0_FIR_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_FIR_DIS_SHIFT)) & SYSCON_PMU_CTRL0_FIR_DIS_MASK)
#define SYSCON_PMU_CTRL0_FSP_DIS_MASK            (0x40000U)
#define SYSCON_PMU_CTRL0_FSP_DIS_SHIFT           (18U)
#define SYSCON_PMU_CTRL0_FSP_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_FSP_DIS_SHIFT)) & SYSCON_PMU_CTRL0_FSP_DIS_MASK)
#define SYSCON_PMU_CTRL0_MCU_MODE_MASK           (0x100000U)
#define SYSCON_PMU_CTRL0_MCU_MODE_SHIFT          (20U)
#define SYSCON_PMU_CTRL0_MCU_MODE(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_MCU_MODE_SHIFT)) & SYSCON_PMU_CTRL0_MCU_MODE_MASK)
#define SYSCON_PMU_CTRL0_OSC_INT_EN_MASK         (0x4000000U)
#define SYSCON_PMU_CTRL0_OSC_INT_EN_SHIFT        (26U)
#define SYSCON_PMU_CTRL0_OSC_INT_EN(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_OSC_INT_EN_SHIFT)) & SYSCON_PMU_CTRL0_OSC_INT_EN_MASK)
#define SYSCON_PMU_CTRL0_RTC_SEC_WAKEUP_EN_MASK  (0x8000000U)
#define SYSCON_PMU_CTRL0_RTC_SEC_WAKEUP_EN_SHIFT (27U)
#define SYSCON_PMU_CTRL0_RTC_SEC_WAKEUP_EN(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_RTC_SEC_WAKEUP_EN_SHIFT)) & SYSCON_PMU_CTRL0_RTC_SEC_WAKEUP_EN_MASK)
#define SYSCON_PMU_CTRL0_WAKEUP_EN_MASK          (0x10000000U)
#define SYSCON_PMU_CTRL0_WAKEUP_EN_SHIFT         (28U)
#define SYSCON_PMU_CTRL0_WAKEUP_EN(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_WAKEUP_EN_SHIFT)) & SYSCON_PMU_CTRL0_WAKEUP_EN_MASK)
#define SYSCON_PMU_CTRL0_PMU_EN_MASK             (0x20000000U)
#define SYSCON_PMU_CTRL0_PMU_EN_SHIFT            (29U)
#define SYSCON_PMU_CTRL0_PMU_EN(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_PMU_EN_SHIFT)) & SYSCON_PMU_CTRL0_PMU_EN_MASK)
#define SYSCON_PMU_CTRL0_RETENTION_EN_MASK       (0x40000000U)
#define SYSCON_PMU_CTRL0_RETENTION_EN_SHIFT      (30U)
#define SYSCON_PMU_CTRL0_RETENTION_EN(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_RETENTION_EN_SHIFT)) & SYSCON_PMU_CTRL0_RETENTION_EN_MASK)
#define SYSCON_PMU_CTRL0_BOND_EN_MASK            (0x80000000U)
#define SYSCON_PMU_CTRL0_BOND_EN_SHIFT           (31U)
#define SYSCON_PMU_CTRL0_BOND_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL0_BOND_EN_SHIFT)) & SYSCON_PMU_CTRL0_BOND_EN_MASK)

/*! @name PMU_CTRL1 - power management uinit control register 1 */
#define SYSCON_PMU_CTRL1_RCO32K_DIS_MASK         (0x1U)
#define SYSCON_PMU_CTRL1_RCO32K_DIS_SHIFT        (0U)
#define SYSCON_PMU_CTRL1_RCO32K_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_RCO32K_DIS_SHIFT)) & SYSCON_PMU_CTRL1_RCO32K_DIS_MASK)
#define SYSCON_PMU_CTRL1_XTAL32K_DIS_MASK        (0x2U)
#define SYSCON_PMU_CTRL1_XTAL32K_DIS_SHIFT       (1U)
#define SYSCON_PMU_CTRL1_XTAL32K_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_XTAL32K_DIS_SHIFT)) & SYSCON_PMU_CTRL1_XTAL32K_DIS_MASK)
#define SYSCON_PMU_CTRL1_XTAL_DIS_MASK           (0x4U)
#define SYSCON_PMU_CTRL1_XTAL_DIS_SHIFT          (2U)
#define SYSCON_PMU_CTRL1_XTAL_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_XTAL_DIS_SHIFT)) & SYSCON_PMU_CTRL1_XTAL_DIS_MASK)
#define SYSCON_PMU_CTRL1_OSC32M_DIS_MASK         (0x8U)
#define SYSCON_PMU_CTRL1_OSC32M_DIS_SHIFT        (3U)
#define SYSCON_PMU_CTRL1_OSC32M_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_OSC32M_DIS_SHIFT)) & SYSCON_PMU_CTRL1_OSC32M_DIS_MASK)
#define SYSCON_PMU_CTRL1_USBPLL_DIS_MASK         (0x10U)
#define SYSCON_PMU_CTRL1_USBPLL_DIS_SHIFT        (4U)
#define SYSCON_PMU_CTRL1_USBPLL_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_USBPLL_DIS_SHIFT)) & SYSCON_PMU_CTRL1_USBPLL_DIS_MASK)
#define SYSCON_PMU_CTRL1_ADC_BUF_DIS_MASK        (0x20U)
#define SYSCON_PMU_CTRL1_ADC_BUF_DIS_SHIFT       (5U)
#define SYSCON_PMU_CTRL1_ADC_BUF_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_ADC_BUF_DIS_SHIFT)) & SYSCON_PMU_CTRL1_ADC_BUF_DIS_MASK)
#define SYSCON_PMU_CTRL1_ADC_BG_DIS_MASK         (0x40U)
#define SYSCON_PMU_CTRL1_ADC_BG_DIS_SHIFT        (6U)
#define SYSCON_PMU_CTRL1_ADC_BG_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_ADC_BG_DIS_SHIFT)) & SYSCON_PMU_CTRL1_ADC_BG_DIS_MASK)
#define SYSCON_PMU_CTRL1_ADC_DIS_MASK            (0x80U)
#define SYSCON_PMU_CTRL1_ADC_DIS_SHIFT           (7U)
#define SYSCON_PMU_CTRL1_ADC_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_ADC_DIS_SHIFT)) & SYSCON_PMU_CTRL1_ADC_DIS_MASK)
#define SYSCON_PMU_CTRL1_ADC_VCM_DIS_MASK        (0x100U)
#define SYSCON_PMU_CTRL1_ADC_VCM_DIS_SHIFT       (8U)
#define SYSCON_PMU_CTRL1_ADC_VCM_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_ADC_VCM_DIS_SHIFT)) & SYSCON_PMU_CTRL1_ADC_VCM_DIS_MASK)
#define SYSCON_PMU_CTRL1_ADC_VREF_DIS_MASK       (0x200U)
#define SYSCON_PMU_CTRL1_ADC_VREF_DIS_SHIFT      (9U)
#define SYSCON_PMU_CTRL1_ADC_VREF_DIS(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_ADC_VREF_DIS_SHIFT)) & SYSCON_PMU_CTRL1_ADC_VREF_DIS_MASK)
#define SYSCON_PMU_CTRL1_DAC_DIS_MASK            (0x400U)
#define SYSCON_PMU_CTRL1_DAC_DIS_SHIFT           (10U)
#define SYSCON_PMU_CTRL1_DAC_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_DAC_DIS_SHIFT)) & SYSCON_PMU_CTRL1_DAC_DIS_MASK)
#define SYSCON_PMU_CTRL1_CAP_SEN_DIS_MASK        (0x800U)
#define SYSCON_PMU_CTRL1_CAP_SEN_DIS_SHIFT       (11U)
#define SYSCON_PMU_CTRL1_CAP_SEN_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_CAP_SEN_DIS_SHIFT)) & SYSCON_PMU_CTRL1_CAP_SEN_DIS_MASK)
#define SYSCON_PMU_CTRL1_BUCK_CTRL_MASK          (0xF0000U)
#define SYSCON_PMU_CTRL1_BUCK_CTRL_SHIFT         (16U)
#define SYSCON_PMU_CTRL1_BUCK_CTRL(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_BUCK_CTRL_SHIFT)) & SYSCON_PMU_CTRL1_BUCK_CTRL_MASK)
#define SYSCON_PMU_CTRL1_RCO32K_PDM_DIS_MASK     (0x40000000U)
#define SYSCON_PMU_CTRL1_RCO32K_PDM_DIS_SHIFT    (30U)
#define SYSCON_PMU_CTRL1_RCO32K_PDM_DIS(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_RCO32K_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL1_RCO32K_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS_MASK    (0x80000000U)
#define SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS_SHIFT   (31U)
#define SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS_MASK)

/*! @name ANA_EN - analog setting register */
#define SYSCON_ANA_EN_BOD_AMP_EN_MASK            (0x1U)
#define SYSCON_ANA_EN_BOD_AMP_EN_SHIFT           (0U)
#define SYSCON_ANA_EN_BOD_AMP_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BOD_AMP_EN_SHIFT)) & SYSCON_ANA_EN_BOD_AMP_EN_MASK)
#define SYSCON_ANA_EN_BOD_EN_MASK                (0x2U)
#define SYSCON_ANA_EN_BOD_EN_SHIFT               (1U)
#define SYSCON_ANA_EN_BOD_EN(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BOD_EN_SHIFT)) & SYSCON_ANA_EN_BOD_EN_MASK)
#define SYSCON_ANA_EN_BAT_MON_EN_MASK            (0x4U)
#define SYSCON_ANA_EN_BAT_MON_EN_SHIFT           (2U)
#define SYSCON_ANA_EN_BAT_MON_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BAT_MON_EN_SHIFT)) & SYSCON_ANA_EN_BAT_MON_EN_MASK)
#define SYSCON_ANA_EN_ACMP0_EN_MASK              (0x8U)
#define SYSCON_ANA_EN_ACMP0_EN_SHIFT             (3U)
#define SYSCON_ANA_EN_ACMP0_EN(x)                (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP0_EN_SHIFT)) & SYSCON_ANA_EN_ACMP0_EN_MASK)
#define SYSCON_ANA_EN_ACMP1_EN_MASK              (0x10U)
#define SYSCON_ANA_EN_ACMP1_EN_SHIFT             (4U)
#define SYSCON_ANA_EN_ACMP1_EN(x)                (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP1_EN_SHIFT)) & SYSCON_ANA_EN_ACMP1_EN_MASK)
#define SYSCON_ANA_EN_BOR_AMP_EN_MASK            (0x20U)
#define SYSCON_ANA_EN_BOR_AMP_EN_SHIFT           (5U)
#define SYSCON_ANA_EN_BOR_AMP_EN(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BOR_AMP_EN_SHIFT)) & SYSCON_ANA_EN_BOR_AMP_EN_MASK)
#define SYSCON_ANA_EN_BOR_EN_MASK                (0x40U)
#define SYSCON_ANA_EN_BOR_EN_SHIFT               (6U)
#define SYSCON_ANA_EN_BOR_EN(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BOR_EN_SHIFT)) & SYSCON_ANA_EN_BOR_EN_MASK)
#define SYSCON_ANA_EN_ACMP0_REF_MASK             (0xF00U)
#define SYSCON_ANA_EN_ACMP0_REF_SHIFT            (8U)
#define SYSCON_ANA_EN_ACMP0_REF(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP0_REF_SHIFT)) & SYSCON_ANA_EN_ACMP0_REF_MASK)
#define SYSCON_ANA_EN_ACMP1_REF_MASK             (0xF000U)
#define SYSCON_ANA_EN_ACMP1_REF_SHIFT            (12U)
#define SYSCON_ANA_EN_ACMP1_REF(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP1_REF_SHIFT)) & SYSCON_ANA_EN_ACMP1_REF_MASK)
#define SYSCON_ANA_EN_ACMP0_HYST_EN_MASK         (0x10000U)
#define SYSCON_ANA_EN_ACMP0_HYST_EN_SHIFT        (16U)
#define SYSCON_ANA_EN_ACMP0_HYST_EN(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP0_HYST_EN_SHIFT)) & SYSCON_ANA_EN_ACMP0_HYST_EN_MASK)
#define SYSCON_ANA_EN_ACMP1_HYST_EN_MASK         (0x20000U)
#define SYSCON_ANA_EN_ACMP1_HYST_EN_SHIFT        (17U)
#define SYSCON_ANA_EN_ACMP1_HYST_EN(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP1_HYST_EN_SHIFT)) & SYSCON_ANA_EN_ACMP1_HYST_EN_MASK)
#define SYSCON_ANA_EN_ACMP_VREF_SEL_MASK         (0x40000U)
#define SYSCON_ANA_EN_ACMP_VREF_SEL_SHIFT        (18U)
#define SYSCON_ANA_EN_ACMP_VREF_SEL(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP_VREF_SEL_SHIFT)) & SYSCON_ANA_EN_ACMP_VREF_SEL_MASK)
#define SYSCON_ANA_EN_BOD_THR_MASK               (0x180000U)
#define SYSCON_ANA_EN_BOD_THR_SHIFT              (19U)
#define SYSCON_ANA_EN_BOD_THR(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BOD_THR_SHIFT)) & SYSCON_ANA_EN_BOD_THR_MASK)
#define SYSCON_ANA_EN_BOR_THR_MASK               (0x600000U)
#define SYSCON_ANA_EN_BOR_THR_SHIFT              (21U)
#define SYSCON_ANA_EN_BOR_THR(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_BOR_THR_SHIFT)) & SYSCON_ANA_EN_BOR_THR_MASK)
#define SYSCON_ANA_EN_ACMP0_OUT_MASK             (0x1000000U)
#define SYSCON_ANA_EN_ACMP0_OUT_SHIFT            (24U)
#define SYSCON_ANA_EN_ACMP0_OUT(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP0_OUT_SHIFT)) & SYSCON_ANA_EN_ACMP0_OUT_MASK)
#define SYSCON_ANA_EN_ACMP1_OUT_MASK             (0x2000000U)
#define SYSCON_ANA_EN_ACMP1_OUT_SHIFT            (25U)
#define SYSCON_ANA_EN_ACMP1_OUT(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP1_OUT_SHIFT)) & SYSCON_ANA_EN_ACMP1_OUT_MASK)
#define SYSCON_ANA_EN_ACMP0_EDGE_SEL_MASK        (0xC000000U)
#define SYSCON_ANA_EN_ACMP0_EDGE_SEL_SHIFT       (26U)
#define SYSCON_ANA_EN_ACMP0_EDGE_SEL(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP0_EDGE_SEL_SHIFT)) & SYSCON_ANA_EN_ACMP0_EDGE_SEL_MASK)
#define SYSCON_ANA_EN_ACMP1_EDGE_SEL_MASK        (0x30000000U)
#define SYSCON_ANA_EN_ACMP1_EDGE_SEL_SHIFT       (28U)
#define SYSCON_ANA_EN_ACMP1_EDGE_SEL(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP1_EDGE_SEL_SHIFT)) & SYSCON_ANA_EN_ACMP1_EDGE_SEL_MASK)
#define SYSCON_ANA_EN_ACMP0_INTEN_MASK           (0x40000000U)
#define SYSCON_ANA_EN_ACMP0_INTEN_SHIFT          (30U)
#define SYSCON_ANA_EN_ACMP0_INTEN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP0_INTEN_SHIFT)) & SYSCON_ANA_EN_ACMP0_INTEN_MASK)
#define SYSCON_ANA_EN_ACMP1_INTEN_MASK           (0x80000000U)
#define SYSCON_ANA_EN_ACMP1_INTEN_SHIFT          (31U)
#define SYSCON_ANA_EN_ACMP1_INTEN(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_EN_ACMP1_INTEN_SHIFT)) & SYSCON_ANA_EN_ACMP1_INTEN_MASK)

/*! @name XTAL32K_CTRL - crystal 32K control register */
#define SYSCON_XTAL32K_CTRL_XTAL32K_ICTRL_MASK   (0x3FU)
#define SYSCON_XTAL32K_CTRL_XTAL32K_ICTRL_SHIFT  (0U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_ICTRL(x)     (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL32K_CTRL_XTAL32K_ICTRL_SHIFT)) & SYSCON_XTAL32K_CTRL_XTAL32K_ICTRL_MASK)
#define SYSCON_XTAL32K_CTRL_XTAL32K_INJ_MASK     (0xC0U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_INJ_SHIFT    (6U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_INJ(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL32K_CTRL_XTAL32K_INJ_SHIFT)) & SYSCON_XTAL32K_CTRL_XTAL32K_INJ_MASK)
#define SYSCON_XTAL32K_CTRL_XTAL32K_LOAD_CAP_MASK (0x3F00U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_LOAD_CAP_SHIFT (8U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_LOAD_CAP(x)  (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL32K_CTRL_XTAL32K_LOAD_CAP_SHIFT)) & SYSCON_XTAL32K_CTRL_XTAL32K_LOAD_CAP_MASK)
#define SYSCON_XTAL32K_CTRL_XTAL32K_EXTRA_CAP_MASK (0x4000U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_EXTRA_CAP_SHIFT (14U)
#define SYSCON_XTAL32K_CTRL_XTAL32K_EXTRA_CAP(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_XTAL32K_CTRL_XTAL32K_EXTRA_CAP_SHIFT)) & SYSCON_XTAL32K_CTRL_XTAL32K_EXTRA_CAP_MASK)

/*! @name USB_CFG - USB configuration register */
#define SYSCON_USB_CFG_DPPUEN_B_PHY_POL_MASK     (0x1U)
#define SYSCON_USB_CFG_DPPUEN_B_PHY_POL_SHIFT    (0U)
#define SYSCON_USB_CFG_DPPUEN_B_PHY_POL(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_USB_CFG_DPPUEN_B_PHY_POL_SHIFT)) & SYSCON_USB_CFG_DPPUEN_B_PHY_POL_MASK)
#define SYSCON_USB_CFG_DPPUEN_B_PHY_SEL_MASK     (0x2U)
#define SYSCON_USB_CFG_DPPUEN_B_PHY_SEL_SHIFT    (1U)
#define SYSCON_USB_CFG_DPPUEN_B_PHY_SEL(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_USB_CFG_DPPUEN_B_PHY_SEL_SHIFT)) & SYSCON_USB_CFG_DPPUEN_B_PHY_SEL_MASK)
#define SYSCON_USB_CFG_USB_VBUS_MASK             (0x8U)
#define SYSCON_USB_CFG_USB_VBUS_SHIFT            (3U)
#define SYSCON_USB_CFG_USB_VBUS(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_USB_CFG_USB_VBUS_SHIFT)) & SYSCON_USB_CFG_USB_VBUS_MASK)
#define SYSCON_USB_CFG_USB_PHYSTDBY_MASK         (0x10U)
#define SYSCON_USB_CFG_USB_PHYSTDBY_SHIFT        (4U)
#define SYSCON_USB_CFG_USB_PHYSTDBY(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_USB_CFG_USB_PHYSTDBY_SHIFT)) & SYSCON_USB_CFG_USB_PHYSTDBY_MASK)
#define SYSCON_USB_CFG_USB_PHYSTDBY_WEN_MASK     (0x20U)
#define SYSCON_USB_CFG_USB_PHYSTDBY_WEN_SHIFT    (5U)
#define SYSCON_USB_CFG_USB_PHYSTDBY_WEN(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_USB_CFG_USB_PHYSTDBY_WEN_SHIFT)) & SYSCON_USB_CFG_USB_PHYSTDBY_WEN_MASK)

/*! @name PMU_CTRL2 - power management uinit control register 2 */
#define SYSCON_PMU_CTRL2_BG_PDM_DIS_MASK         (0x1U)
#define SYSCON_PMU_CTRL2_BG_PDM_DIS_SHIFT        (0U)
#define SYSCON_PMU_CTRL2_BG_PDM_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_BG_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_BG_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_V2I_PDM_DIS_MASK        (0x2U)
#define SYSCON_PMU_CTRL2_V2I_PDM_DIS_SHIFT       (1U)
#define SYSCON_PMU_CTRL2_V2I_PDM_DIS(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_V2I_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_V2I_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_VREG_A_PDM_DIS_MASK     (0x4U)
#define SYSCON_PMU_CTRL2_VREG_A_PDM_DIS_SHIFT    (2U)
#define SYSCON_PMU_CTRL2_VREG_A_PDM_DIS(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_VREG_A_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_VREG_A_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_VREG_D_PDM_DIS_MASK     (0x8U)
#define SYSCON_PMU_CTRL2_VREG_D_PDM_DIS_SHIFT    (3U)
#define SYSCON_PMU_CTRL2_VREG_D_PDM_DIS(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_VREG_D_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_VREG_D_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_XTAL_PDM_DIS_MASK       (0x10U)
#define SYSCON_PMU_CTRL2_XTAL_PDM_DIS_SHIFT      (4U)
#define SYSCON_PMU_CTRL2_XTAL_PDM_DIS(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_XTAL_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_XTAL_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_OSC32M_PDM_DIS_MASK     (0x20U)
#define SYSCON_PMU_CTRL2_OSC32M_PDM_DIS_SHIFT    (5U)
#define SYSCON_PMU_CTRL2_OSC32M_PDM_DIS(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_OSC32M_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_OSC32M_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_RFAGC_ON_MASK           (0x40U)
#define SYSCON_PMU_CTRL2_RFAGC_ON_SHIFT          (6U)
#define SYSCON_PMU_CTRL2_RFAGC_ON(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_RFAGC_ON_SHIFT)) & SYSCON_PMU_CTRL2_RFAGC_ON_MASK)
#define SYSCON_PMU_CTRL2_RX_EN_SEL_MASK          (0x80U)
#define SYSCON_PMU_CTRL2_RX_EN_SEL_SHIFT         (7U)
#define SYSCON_PMU_CTRL2_RX_EN_SEL(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_RX_EN_SEL_SHIFT)) & SYSCON_PMU_CTRL2_RX_EN_SEL_MASK)
#define SYSCON_PMU_CTRL2_BG_DIS_MASK             (0x100U)
#define SYSCON_PMU_CTRL2_BG_DIS_SHIFT            (8U)
#define SYSCON_PMU_CTRL2_BG_DIS(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_BG_DIS_SHIFT)) & SYSCON_PMU_CTRL2_BG_DIS_MASK)
#define SYSCON_PMU_CTRL2_V2I_DIS_MASK            (0x200U)
#define SYSCON_PMU_CTRL2_V2I_DIS_SHIFT           (9U)
#define SYSCON_PMU_CTRL2_V2I_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_V2I_DIS_SHIFT)) & SYSCON_PMU_CTRL2_V2I_DIS_MASK)
#define SYSCON_PMU_CTRL2_VREG_A_DIS_MASK         (0x400U)
#define SYSCON_PMU_CTRL2_VREG_A_DIS_SHIFT        (10U)
#define SYSCON_PMU_CTRL2_VREG_A_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_VREG_A_DIS_SHIFT)) & SYSCON_PMU_CTRL2_VREG_A_DIS_MASK)
#define SYSCON_PMU_CTRL2_VREG_D_DIS_MASK         (0x800U)
#define SYSCON_PMU_CTRL2_VREG_D_DIS_SHIFT        (11U)
#define SYSCON_PMU_CTRL2_VREG_D_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_VREG_D_DIS_SHIFT)) & SYSCON_PMU_CTRL2_VREG_D_DIS_MASK)
#define SYSCON_PMU_CTRL2_LO_DIS_MASK             (0x1000U)
#define SYSCON_PMU_CTRL2_LO_DIS_SHIFT            (12U)
#define SYSCON_PMU_CTRL2_LO_DIS(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_LO_DIS_SHIFT)) & SYSCON_PMU_CTRL2_LO_DIS_MASK)
#define SYSCON_PMU_CTRL2_VCO_DIS_MASK            (0x2000U)
#define SYSCON_PMU_CTRL2_VCO_DIS_SHIFT           (13U)
#define SYSCON_PMU_CTRL2_VCO_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_VCO_DIS_SHIFT)) & SYSCON_PMU_CTRL2_VCO_DIS_MASK)
#define SYSCON_PMU_CTRL2_PA_PK_DIS_MASK          (0x4000U)
#define SYSCON_PMU_CTRL2_PA_PK_DIS_SHIFT         (14U)
#define SYSCON_PMU_CTRL2_PA_PK_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_PA_PK_DIS_SHIFT)) & SYSCON_PMU_CTRL2_PA_PK_DIS_MASK)
#define SYSCON_PMU_CTRL2_PA_DIS_MASK             (0x8000U)
#define SYSCON_PMU_CTRL2_PA_DIS_SHIFT            (15U)
#define SYSCON_PMU_CTRL2_PA_DIS(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_PA_DIS_SHIFT)) & SYSCON_PMU_CTRL2_PA_DIS_MASK)
#define SYSCON_PMU_CTRL2_LNA_DIS_MASK            (0x10000U)
#define SYSCON_PMU_CTRL2_LNA_DIS_SHIFT           (16U)
#define SYSCON_PMU_CTRL2_LNA_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_LNA_DIS_SHIFT)) & SYSCON_PMU_CTRL2_LNA_DIS_MASK)
#define SYSCON_PMU_CTRL2_MIXER_DIS_MASK          (0x20000U)
#define SYSCON_PMU_CTRL2_MIXER_DIS_SHIFT         (17U)
#define SYSCON_PMU_CTRL2_MIXER_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_MIXER_DIS_SHIFT)) & SYSCON_PMU_CTRL2_MIXER_DIS_MASK)
#define SYSCON_PMU_CTRL2_PKDET_DIS_MASK          (0x40000U)
#define SYSCON_PMU_CTRL2_PKDET_DIS_SHIFT         (18U)
#define SYSCON_PMU_CTRL2_PKDET_DIS(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_PKDET_DIS_SHIFT)) & SYSCON_PMU_CTRL2_PKDET_DIS_MASK)
#define SYSCON_PMU_CTRL2_PPF_DIS_MASK            (0x80000U)
#define SYSCON_PMU_CTRL2_PPF_DIS_SHIFT           (19U)
#define SYSCON_PMU_CTRL2_PPF_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_PPF_DIS_SHIFT)) & SYSCON_PMU_CTRL2_PPF_DIS_MASK)
#define SYSCON_PMU_CTRL2_SAR_DIS_MASK            (0x100000U)
#define SYSCON_PMU_CTRL2_SAR_DIS_SHIFT           (20U)
#define SYSCON_PMU_CTRL2_SAR_DIS(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_SAR_DIS_SHIFT)) & SYSCON_PMU_CTRL2_SAR_DIS_MASK)
#define SYSCON_PMU_CTRL2_RC_CAL_DIS_MASK         (0x200000U)
#define SYSCON_PMU_CTRL2_RC_CAL_DIS_SHIFT        (21U)
#define SYSCON_PMU_CTRL2_RC_CAL_DIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_RC_CAL_DIS_SHIFT)) & SYSCON_PMU_CTRL2_RC_CAL_DIS_MASK)
#define SYSCON_PMU_CTRL2_FLSH_DIS_MASK           (0x20000000U)
#define SYSCON_PMU_CTRL2_FLSH_DIS_SHIFT          (29U)
#define SYSCON_PMU_CTRL2_FLSH_DIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_FLSH_DIS_SHIFT)) & SYSCON_PMU_CTRL2_FLSH_DIS_MASK)
#define SYSCON_PMU_CTRL2_FLSH_PDM_DIS_MASK       (0x40000000U)
#define SYSCON_PMU_CTRL2_FLSH_PDM_DIS_SHIFT      (30U)
#define SYSCON_PMU_CTRL2_FLSH_PDM_DIS(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_FLSH_PDM_DIS_SHIFT)) & SYSCON_PMU_CTRL2_FLSH_PDM_DIS_MASK)
#define SYSCON_PMU_CTRL2_SEL_PD_MASK             (0x80000000U)
#define SYSCON_PMU_CTRL2_SEL_PD_SHIFT            (31U)
#define SYSCON_PMU_CTRL2_SEL_PD(x)               (((uint32_t)(((uint32_t)(x)) << SYSCON_PMU_CTRL2_SEL_PD_SHIFT)) & SYSCON_PMU_CTRL2_SEL_PD_MASK)

/*! @name ANA_CTRL1 - IVREF and DVREG setting register */
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_PDM_MASK    (0x3U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_PDM_SHIFT   (0U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_PDM(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_PMU_SET_PDM_SHIFT)) & SYSCON_ANA_CTRL1_VDD_PMU_SET_PDM_MASK)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_MASK        (0xCU)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_SHIFT       (2U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_PMU_SET_SHIFT)) & SYSCON_ANA_CTRL1_VDD_PMU_SET_MASK)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_PDM_MASK    (0x30U)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_PDM_SHIFT   (4U)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_PDM(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_MEM_SET_PDM_SHIFT)) & SYSCON_ANA_CTRL1_VDD_MEM_SET_PDM_MASK)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_MASK        (0xC0U)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_SHIFT       (6U)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_MEM_SET_SHIFT)) & SYSCON_ANA_CTRL1_VDD_MEM_SET_MASK)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_EXTRA_MASK  (0x100U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_EXTRA_SHIFT (8U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_EXTRA(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_PMU_SET_EXTRA_SHIFT)) & SYSCON_ANA_CTRL1_VDD_PMU_SET_EXTRA_MASK)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_EXTRA_MASK  (0x200U)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_EXTRA_SHIFT (9U)
#define SYSCON_ANA_CTRL1_VDD_MEM_SET_EXTRA(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_MEM_SET_EXTRA_SHIFT)) & SYSCON_ANA_CTRL1_VDD_MEM_SET_EXTRA_MASK)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_ULTRA_LOW_MASK (0x400U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_ULTRA_LOW_SHIFT (10U)
#define SYSCON_ANA_CTRL1_VDD_PMU_SET_ULTRA_LOW(x) (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_PMU_SET_ULTRA_LOW_SHIFT)) & SYSCON_ANA_CTRL1_VDD_PMU_SET_ULTRA_LOW_MASK)
#define SYSCON_ANA_CTRL1_VDD_PMU_MEM_SW_MASK     (0x800U)
#define SYSCON_ANA_CTRL1_VDD_PMU_MEM_SW_SHIFT    (11U)
#define SYSCON_ANA_CTRL1_VDD_PMU_MEM_SW(x)       (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_VDD_PMU_MEM_SW_SHIFT)) & SYSCON_ANA_CTRL1_VDD_PMU_MEM_SW_MASK)
#define SYSCON_ANA_CTRL1_IV_BG_SEL_MASK          (0xF000U)
#define SYSCON_ANA_CTRL1_IV_BG_SEL_SHIFT         (12U)
#define SYSCON_ANA_CTRL1_IV_BG_SEL(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_IV_BG_SEL_SHIFT)) & SYSCON_ANA_CTRL1_IV_BG_SEL_MASK)
#define SYSCON_ANA_CTRL1_PDM_DIS_BUCK_MASK       (0x10000U)
#define SYSCON_ANA_CTRL1_PDM_DIS_BUCK_SHIFT      (16U)
#define SYSCON_ANA_CTRL1_PDM_DIS_BUCK(x)         (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_PDM_DIS_BUCK_SHIFT)) & SYSCON_ANA_CTRL1_PDM_DIS_BUCK_MASK)
#define SYSCON_ANA_CTRL1_BUCK_PD_CCM_MASK        (0x20000U)
#define SYSCON_ANA_CTRL1_BUCK_PD_CCM_SHIFT       (17U)
#define SYSCON_ANA_CTRL1_BUCK_PD_CCM(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_BUCK_PD_CCM_SHIFT)) & SYSCON_ANA_CTRL1_BUCK_PD_CCM_MASK)
#define SYSCON_ANA_CTRL1_BUCK_PD_DCM_MASK        (0x40000U)
#define SYSCON_ANA_CTRL1_BUCK_PD_DCM_SHIFT       (18U)
#define SYSCON_ANA_CTRL1_BUCK_PD_DCM(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_BUCK_PD_DCM_SHIFT)) & SYSCON_ANA_CTRL1_BUCK_PD_DCM_MASK)
#define SYSCON_ANA_CTRL1_IV_IREF_SEL_MASK        (0x180000U)
#define SYSCON_ANA_CTRL1_IV_IREF_SEL_SHIFT       (19U)
#define SYSCON_ANA_CTRL1_IV_IREF_SEL(x)          (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_IV_IREF_SEL_SHIFT)) & SYSCON_ANA_CTRL1_IV_IREF_SEL_MASK)
#define SYSCON_ANA_CTRL1_IV_VREG11_SET_MASK      (0xE00000U)
#define SYSCON_ANA_CTRL1_IV_VREG11_SET_SHIFT     (21U)
#define SYSCON_ANA_CTRL1_IV_VREG11_SET(x)        (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_IV_VREG11_SET_SHIFT)) & SYSCON_ANA_CTRL1_IV_VREG11_SET_MASK)
#define SYSCON_ANA_CTRL1_XTAL32K_FORCE_RDY_MASK  (0x1000000U)
#define SYSCON_ANA_CTRL1_XTAL32K_FORCE_RDY_SHIFT (24U)
#define SYSCON_ANA_CTRL1_XTAL32K_FORCE_RDY(x)    (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_XTAL32K_FORCE_RDY_SHIFT)) & SYSCON_ANA_CTRL1_XTAL32K_FORCE_RDY_MASK)
#define SYSCON_ANA_CTRL1_X32_SMT_EN_MASK         (0x2000000U)
#define SYSCON_ANA_CTRL1_X32_SMT_EN_SHIFT        (25U)
#define SYSCON_ANA_CTRL1_X32_SMT_EN(x)           (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_X32_SMT_EN_SHIFT)) & SYSCON_ANA_CTRL1_X32_SMT_EN_MASK)
#define SYSCON_ANA_CTRL1_BM_X32BUF_MASK          (0xC000000U)
#define SYSCON_ANA_CTRL1_BM_X32BUF_SHIFT         (26U)
#define SYSCON_ANA_CTRL1_BM_X32BUF(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_BM_X32BUF_SHIFT)) & SYSCON_ANA_CTRL1_BM_X32BUF_MASK)
#define SYSCON_ANA_CTRL1_DVREG11_SET_DIG_MASK    (0x70000000U)
#define SYSCON_ANA_CTRL1_DVREG11_SET_DIG_SHIFT   (28U)
#define SYSCON_ANA_CTRL1_DVREG11_SET_DIG(x)      (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_DVREG11_SET_DIG_SHIFT)) & SYSCON_ANA_CTRL1_DVREG11_SET_DIG_MASK)
#define SYSCON_ANA_CTRL1_BUCK_DPD_MASK           (0x80000000U)
#define SYSCON_ANA_CTRL1_BUCK_DPD_SHIFT          (31U)
#define SYSCON_ANA_CTRL1_BUCK_DPD(x)             (((uint32_t)(((uint32_t)(x)) << SYSCON_ANA_CTRL1_BUCK_DPD_SHIFT)) & SYSCON_ANA_CTRL1_BUCK_DPD_MASK)

/*! @name MISC - MISC register */
#define SYSCON_MISC_RCO_PWR_MODE_MASK            (0x3U)
#define SYSCON_MISC_RCO_PWR_MODE_SHIFT           (0U)
#define SYSCON_MISC_RCO_PWR_MODE(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_MISC_RCO_PWR_MODE_SHIFT)) & SYSCON_MISC_RCO_PWR_MODE_MASK)
#define SYSCON_MISC_EN_SWD_MASK                  (0x10000U)
#define SYSCON_MISC_EN_SWD_SHIFT                 (16U)
#define SYSCON_MISC_EN_SWD(x)                    (((uint32_t)(((uint32_t)(x)) << SYSCON_MISC_EN_SWD_SHIFT)) & SYSCON_MISC_EN_SWD_MASK)
#define SYSCON_MISC_DIS_FLSH_POWER_MASK          (0x20000U)
#define SYSCON_MISC_DIS_FLSH_POWER_SHIFT         (17U)
#define SYSCON_MISC_DIS_FLSH_POWER(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_MISC_DIS_FLSH_POWER_SHIFT)) & SYSCON_MISC_DIS_FLSH_POWER_MASK)
#define SYSCON_MISC_DIS_USB_PULLUP_MASK          (0x40000U)
#define SYSCON_MISC_DIS_USB_PULLUP_SHIFT         (18U)
#define SYSCON_MISC_DIS_USB_PULLUP(x)            (((uint32_t)(((uint32_t)(x)) << SYSCON_MISC_DIS_USB_PULLUP_SHIFT)) & SYSCON_MISC_DIS_USB_PULLUP_MASK)
#define SYSCON_MISC_DPPU_OPT_SEL_MASK            (0x1000000U)
#define SYSCON_MISC_DPPU_OPT_SEL_SHIFT           (24U)
#define SYSCON_MISC_DPPU_OPT_SEL(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_MISC_DPPU_OPT_SEL_SHIFT)) & SYSCON_MISC_DPPU_OPT_SEL_MASK)
#define SYSCON_MISC_DPPU_OPT_POL_MASK            (0x2000000U)
#define SYSCON_MISC_DPPU_OPT_POL_SHIFT           (25U)
#define SYSCON_MISC_DPPU_OPT_POL(x)              (((uint32_t)(((uint32_t)(x)) << SYSCON_MISC_DPPU_OPT_POL_SHIFT)) & SYSCON_MISC_DPPU_OPT_POL_MASK)


/*!
 * @}
 */ /* end of group SYSCON_Register_Masks */


/* SYSCON - Peripheral instance base addresses */
/** Peripheral SYSCON base address */
#define SYSCON_BASE                              (0x40000000u)
/** Peripheral SYSCON base pointer */
#define SYSCON                                   ((SYSCON_Type *)SYSCON_BASE)
/** Array initializer of SYSCON peripheral base addresses */
#define SYSCON_BASE_ADDRS                        { SYSCON_BASE }
/** Array initializer of SYSCON peripheral base pointers */
#define SYSCON_BASE_PTRS                         { SYSCON }
/** Interrupt vectors for the SYSCON peripheral type */
#define SYSCON_IRQS                              { { EXT_GPIO_WAKEUP_IRQn, ACMP0_IRQn, ACMP1_IRQn, XTAL_READY_IRQn, OSC_IRQn, OSC_INT_LOW_IRQn, BOD_IRQn, BLE_TX_IRQn, BLE_RX_IRQn, BLE_FREQ_HOP_IRQn } }

/*!
 * @}
 */ /* end of group SYSCON_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- USART Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup USART_Peripheral_Access_Layer USART Peripheral Access Layer
 * @{
 */

/** USART - Register Layout Typedef */
typedef struct {
  __IO uint32_t CFG;                               /**< USART Configuration register. Basic USART configuration settings that typically are not changed during operation., offset: 0x0 */
  __IO uint32_t CTL;                               /**< USART Control register. USART control settings that are more likely to change during operation., offset: 0x4 */
  __IO uint32_t STAT;                              /**< USART Status register. The complete status value can be read here. Writing ones clears some bits in the register. Some bits can be cleared by writing a 1 to them., offset: 0x8 */
  __IO uint32_t INTENSET;                          /**< Interrupt Enable read and Set register for USART (not FIFO) status. Contains individual interrupt enable bits for each potential USART interrupt. A complete value may be read from this register. Writing a 1 to any implemented bit position causes that bit to be set., offset: 0xC */
  __O  uint32_t INTENCLR;                          /**< Interrupt Enable Clear register. Allows clearing any combination of bits in the INTENSET register. Writing a 1 to any implemented bit position causes the corresponding bit to be cleared., offset: 0x10 */
       uint8_t RESERVED_0[12];
  __IO uint32_t BRG;                               /**< Baud Rate Generator register. 16-bit integer baud rate divisor value., offset: 0x20 */
  __I  uint32_t INTSTAT;                           /**< Interrupt status register. Reflects interrupts that are currently enabled., offset: 0x24 */
  __IO uint32_t OSR;                               /**< Oversample selection register for asynchronous communication., offset: 0x28 */
  __IO uint32_t ADDR;                              /**< Address register for automatic address matching., offset: 0x2C */
       uint8_t RESERVED_1[3536];
  __IO uint32_t FIFOCFG;                           /**< FIFO configuration and enable register., offset: 0xE00 */
  __IO uint32_t FIFOSTAT;                          /**< FIFO status register., offset: 0xE04 */
  __IO uint32_t FIFOTRIG;                          /**< FIFO trigger settings for interrupt and DMA request., offset: 0xE08 */
       uint8_t RESERVED_2[4];
  __IO uint32_t FIFOINTENSET;                      /**< FIFO interrupt enable set (enable) and read register., offset: 0xE10 */
  __IO uint32_t FIFOINTENCLR;                      /**< FIFO interrupt enable clear (disable) and read register., offset: 0xE14 */
  __I  uint32_t FIFOINTSTAT;                       /**< FIFO interrupt status register., offset: 0xE18 */
       uint8_t RESERVED_3[4];
  __IO uint32_t FIFOWR;                            /**< FIFO write data., offset: 0xE20 */
       uint8_t RESERVED_4[12];
  __I  uint32_t FIFORD;                            /**< FIFO read data., offset: 0xE30 */
       uint8_t RESERVED_5[12];
  __I  uint32_t FIFORDNOPOP;                       /**< FIFO data read with no FIFO pop., offset: 0xE40 */
       uint8_t RESERVED_6[440];
  __I  uint32_t ID;                                /**< USART module Identification. This value appears in the shared Flexcomm peripheral ID register when USART is selected., offset: 0xFFC */
} USART_Type;

/* ----------------------------------------------------------------------------
   -- USART Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup USART_Register_Masks USART Register Masks
 * @{
 */

/*! @name CFG - USART Configuration register. Basic USART configuration settings that typically are not changed during operation. */
#define USART_CFG_ENABLE_MASK                    (0x1U)
#define USART_CFG_ENABLE_SHIFT                   (0U)
#define USART_CFG_ENABLE(x)                      (((uint32_t)(((uint32_t)(x)) << USART_CFG_ENABLE_SHIFT)) & USART_CFG_ENABLE_MASK)
#define USART_CFG_DATALEN_MASK                   (0xCU)
#define USART_CFG_DATALEN_SHIFT                  (2U)
#define USART_CFG_DATALEN(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CFG_DATALEN_SHIFT)) & USART_CFG_DATALEN_MASK)
#define USART_CFG_PARITYSEL_MASK                 (0x30U)
#define USART_CFG_PARITYSEL_SHIFT                (4U)
#define USART_CFG_PARITYSEL(x)                   (((uint32_t)(((uint32_t)(x)) << USART_CFG_PARITYSEL_SHIFT)) & USART_CFG_PARITYSEL_MASK)
#define USART_CFG_STOPLEN_MASK                   (0x40U)
#define USART_CFG_STOPLEN_SHIFT                  (6U)
#define USART_CFG_STOPLEN(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CFG_STOPLEN_SHIFT)) & USART_CFG_STOPLEN_MASK)
#define USART_CFG_MODE32K_MASK                   (0x80U)
#define USART_CFG_MODE32K_SHIFT                  (7U)
#define USART_CFG_MODE32K(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CFG_MODE32K_SHIFT)) & USART_CFG_MODE32K_MASK)
#define USART_CFG_LINMODE_MASK                   (0x100U)
#define USART_CFG_LINMODE_SHIFT                  (8U)
#define USART_CFG_LINMODE(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CFG_LINMODE_SHIFT)) & USART_CFG_LINMODE_MASK)
#define USART_CFG_CTSEN_MASK                     (0x200U)
#define USART_CFG_CTSEN_SHIFT                    (9U)
#define USART_CFG_CTSEN(x)                       (((uint32_t)(((uint32_t)(x)) << USART_CFG_CTSEN_SHIFT)) & USART_CFG_CTSEN_MASK)
#define USART_CFG_SYNCEN_MASK                    (0x800U)
#define USART_CFG_SYNCEN_SHIFT                   (11U)
#define USART_CFG_SYNCEN(x)                      (((uint32_t)(((uint32_t)(x)) << USART_CFG_SYNCEN_SHIFT)) & USART_CFG_SYNCEN_MASK)
#define USART_CFG_CLKPOL_MASK                    (0x1000U)
#define USART_CFG_CLKPOL_SHIFT                   (12U)
#define USART_CFG_CLKPOL(x)                      (((uint32_t)(((uint32_t)(x)) << USART_CFG_CLKPOL_SHIFT)) & USART_CFG_CLKPOL_MASK)
#define USART_CFG_SYNCMST_MASK                   (0x4000U)
#define USART_CFG_SYNCMST_SHIFT                  (14U)
#define USART_CFG_SYNCMST(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CFG_SYNCMST_SHIFT)) & USART_CFG_SYNCMST_MASK)
#define USART_CFG_LOOP_MASK                      (0x8000U)
#define USART_CFG_LOOP_SHIFT                     (15U)
#define USART_CFG_LOOP(x)                        (((uint32_t)(((uint32_t)(x)) << USART_CFG_LOOP_SHIFT)) & USART_CFG_LOOP_MASK)
#define USART_CFG_OETA_MASK                      (0x40000U)
#define USART_CFG_OETA_SHIFT                     (18U)
#define USART_CFG_OETA(x)                        (((uint32_t)(((uint32_t)(x)) << USART_CFG_OETA_SHIFT)) & USART_CFG_OETA_MASK)
#define USART_CFG_AUTOADDR_MASK                  (0x80000U)
#define USART_CFG_AUTOADDR_SHIFT                 (19U)
#define USART_CFG_AUTOADDR(x)                    (((uint32_t)(((uint32_t)(x)) << USART_CFG_AUTOADDR_SHIFT)) & USART_CFG_AUTOADDR_MASK)
#define USART_CFG_OESEL_MASK                     (0x100000U)
#define USART_CFG_OESEL_SHIFT                    (20U)
#define USART_CFG_OESEL(x)                       (((uint32_t)(((uint32_t)(x)) << USART_CFG_OESEL_SHIFT)) & USART_CFG_OESEL_MASK)
#define USART_CFG_OEPOL_MASK                     (0x200000U)
#define USART_CFG_OEPOL_SHIFT                    (21U)
#define USART_CFG_OEPOL(x)                       (((uint32_t)(((uint32_t)(x)) << USART_CFG_OEPOL_SHIFT)) & USART_CFG_OEPOL_MASK)
#define USART_CFG_RXPOL_MASK                     (0x400000U)
#define USART_CFG_RXPOL_SHIFT                    (22U)
#define USART_CFG_RXPOL(x)                       (((uint32_t)(((uint32_t)(x)) << USART_CFG_RXPOL_SHIFT)) & USART_CFG_RXPOL_MASK)
#define USART_CFG_TXPOL_MASK                     (0x800000U)
#define USART_CFG_TXPOL_SHIFT                    (23U)
#define USART_CFG_TXPOL(x)                       (((uint32_t)(((uint32_t)(x)) << USART_CFG_TXPOL_SHIFT)) & USART_CFG_TXPOL_MASK)

/*! @name CTL - USART Control register. USART control settings that are more likely to change during operation. */
#define USART_CTL_TXBRKEN_MASK                   (0x2U)
#define USART_CTL_TXBRKEN_SHIFT                  (1U)
#define USART_CTL_TXBRKEN(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CTL_TXBRKEN_SHIFT)) & USART_CTL_TXBRKEN_MASK)
#define USART_CTL_ADDRDET_MASK                   (0x4U)
#define USART_CTL_ADDRDET_SHIFT                  (2U)
#define USART_CTL_ADDRDET(x)                     (((uint32_t)(((uint32_t)(x)) << USART_CTL_ADDRDET_SHIFT)) & USART_CTL_ADDRDET_MASK)
#define USART_CTL_TXDIS_MASK                     (0x40U)
#define USART_CTL_TXDIS_SHIFT                    (6U)
#define USART_CTL_TXDIS(x)                       (((uint32_t)(((uint32_t)(x)) << USART_CTL_TXDIS_SHIFT)) & USART_CTL_TXDIS_MASK)
#define USART_CTL_CC_MASK                        (0x100U)
#define USART_CTL_CC_SHIFT                       (8U)
#define USART_CTL_CC(x)                          (((uint32_t)(((uint32_t)(x)) << USART_CTL_CC_SHIFT)) & USART_CTL_CC_MASK)
#define USART_CTL_CLRCCONRX_MASK                 (0x200U)
#define USART_CTL_CLRCCONRX_SHIFT                (9U)
#define USART_CTL_CLRCCONRX(x)                   (((uint32_t)(((uint32_t)(x)) << USART_CTL_CLRCCONRX_SHIFT)) & USART_CTL_CLRCCONRX_MASK)
#define USART_CTL_AUTOBAUD_MASK                  (0x10000U)
#define USART_CTL_AUTOBAUD_SHIFT                 (16U)
#define USART_CTL_AUTOBAUD(x)                    (((uint32_t)(((uint32_t)(x)) << USART_CTL_AUTOBAUD_SHIFT)) & USART_CTL_AUTOBAUD_MASK)

/*! @name STAT - USART Status register. The complete status value can be read here. Writing ones clears some bits in the register. Some bits can be cleared by writing a 1 to them. */
#define USART_STAT_RXIDLE_MASK                   (0x2U)
#define USART_STAT_RXIDLE_SHIFT                  (1U)
#define USART_STAT_RXIDLE(x)                     (((uint32_t)(((uint32_t)(x)) << USART_STAT_RXIDLE_SHIFT)) & USART_STAT_RXIDLE_MASK)
#define USART_STAT_TXIDLE_MASK                   (0x8U)
#define USART_STAT_TXIDLE_SHIFT                  (3U)
#define USART_STAT_TXIDLE(x)                     (((uint32_t)(((uint32_t)(x)) << USART_STAT_TXIDLE_SHIFT)) & USART_STAT_TXIDLE_MASK)
#define USART_STAT_CTS_MASK                      (0x10U)
#define USART_STAT_CTS_SHIFT                     (4U)
#define USART_STAT_CTS(x)                        (((uint32_t)(((uint32_t)(x)) << USART_STAT_CTS_SHIFT)) & USART_STAT_CTS_MASK)
#define USART_STAT_DELTACTS_MASK                 (0x20U)
#define USART_STAT_DELTACTS_SHIFT                (5U)
#define USART_STAT_DELTACTS(x)                   (((uint32_t)(((uint32_t)(x)) << USART_STAT_DELTACTS_SHIFT)) & USART_STAT_DELTACTS_MASK)
#define USART_STAT_TXDISSTAT_MASK                (0x40U)
#define USART_STAT_TXDISSTAT_SHIFT               (6U)
#define USART_STAT_TXDISSTAT(x)                  (((uint32_t)(((uint32_t)(x)) << USART_STAT_TXDISSTAT_SHIFT)) & USART_STAT_TXDISSTAT_MASK)
#define USART_STAT_RXBRK_MASK                    (0x400U)
#define USART_STAT_RXBRK_SHIFT                   (10U)
#define USART_STAT_RXBRK(x)                      (((uint32_t)(((uint32_t)(x)) << USART_STAT_RXBRK_SHIFT)) & USART_STAT_RXBRK_MASK)
#define USART_STAT_DELTARXBRK_MASK               (0x800U)
#define USART_STAT_DELTARXBRK_SHIFT              (11U)
#define USART_STAT_DELTARXBRK(x)                 (((uint32_t)(((uint32_t)(x)) << USART_STAT_DELTARXBRK_SHIFT)) & USART_STAT_DELTARXBRK_MASK)
#define USART_STAT_START_MASK                    (0x1000U)
#define USART_STAT_START_SHIFT                   (12U)
#define USART_STAT_START(x)                      (((uint32_t)(((uint32_t)(x)) << USART_STAT_START_SHIFT)) & USART_STAT_START_MASK)
#define USART_STAT_FRAMERRINT_MASK               (0x2000U)
#define USART_STAT_FRAMERRINT_SHIFT              (13U)
#define USART_STAT_FRAMERRINT(x)                 (((uint32_t)(((uint32_t)(x)) << USART_STAT_FRAMERRINT_SHIFT)) & USART_STAT_FRAMERRINT_MASK)
#define USART_STAT_PARITYERRINT_MASK             (0x4000U)
#define USART_STAT_PARITYERRINT_SHIFT            (14U)
#define USART_STAT_PARITYERRINT(x)               (((uint32_t)(((uint32_t)(x)) << USART_STAT_PARITYERRINT_SHIFT)) & USART_STAT_PARITYERRINT_MASK)
#define USART_STAT_RXNOISEINT_MASK               (0x8000U)
#define USART_STAT_RXNOISEINT_SHIFT              (15U)
#define USART_STAT_RXNOISEINT(x)                 (((uint32_t)(((uint32_t)(x)) << USART_STAT_RXNOISEINT_SHIFT)) & USART_STAT_RXNOISEINT_MASK)
#define USART_STAT_ABERR_MASK                    (0x10000U)
#define USART_STAT_ABERR_SHIFT                   (16U)
#define USART_STAT_ABERR(x)                      (((uint32_t)(((uint32_t)(x)) << USART_STAT_ABERR_SHIFT)) & USART_STAT_ABERR_MASK)

/*! @name INTENSET - Interrupt Enable read and Set register for USART (not FIFO) status. Contains individual interrupt enable bits for each potential USART interrupt. A complete value may be read from this register. Writing a 1 to any implemented bit position causes that bit to be set. */
#define USART_INTENSET_TXIDLEEN_MASK             (0x8U)
#define USART_INTENSET_TXIDLEEN_SHIFT            (3U)
#define USART_INTENSET_TXIDLEEN(x)               (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_TXIDLEEN_SHIFT)) & USART_INTENSET_TXIDLEEN_MASK)
#define USART_INTENSET_DELTACTSEN_MASK           (0x20U)
#define USART_INTENSET_DELTACTSEN_SHIFT          (5U)
#define USART_INTENSET_DELTACTSEN(x)             (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_DELTACTSEN_SHIFT)) & USART_INTENSET_DELTACTSEN_MASK)
#define USART_INTENSET_TXDISEN_MASK              (0x40U)
#define USART_INTENSET_TXDISEN_SHIFT             (6U)
#define USART_INTENSET_TXDISEN(x)                (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_TXDISEN_SHIFT)) & USART_INTENSET_TXDISEN_MASK)
#define USART_INTENSET_DELTARXBRKEN_MASK         (0x800U)
#define USART_INTENSET_DELTARXBRKEN_SHIFT        (11U)
#define USART_INTENSET_DELTARXBRKEN(x)           (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_DELTARXBRKEN_SHIFT)) & USART_INTENSET_DELTARXBRKEN_MASK)
#define USART_INTENSET_STARTEN_MASK              (0x1000U)
#define USART_INTENSET_STARTEN_SHIFT             (12U)
#define USART_INTENSET_STARTEN(x)                (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_STARTEN_SHIFT)) & USART_INTENSET_STARTEN_MASK)
#define USART_INTENSET_FRAMERREN_MASK            (0x2000U)
#define USART_INTENSET_FRAMERREN_SHIFT           (13U)
#define USART_INTENSET_FRAMERREN(x)              (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_FRAMERREN_SHIFT)) & USART_INTENSET_FRAMERREN_MASK)
#define USART_INTENSET_PARITYERREN_MASK          (0x4000U)
#define USART_INTENSET_PARITYERREN_SHIFT         (14U)
#define USART_INTENSET_PARITYERREN(x)            (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_PARITYERREN_SHIFT)) & USART_INTENSET_PARITYERREN_MASK)
#define USART_INTENSET_RXNOISEEN_MASK            (0x8000U)
#define USART_INTENSET_RXNOISEEN_SHIFT           (15U)
#define USART_INTENSET_RXNOISEEN(x)              (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_RXNOISEEN_SHIFT)) & USART_INTENSET_RXNOISEEN_MASK)
#define USART_INTENSET_ABERREN_MASK              (0x10000U)
#define USART_INTENSET_ABERREN_SHIFT             (16U)
#define USART_INTENSET_ABERREN(x)                (((uint32_t)(((uint32_t)(x)) << USART_INTENSET_ABERREN_SHIFT)) & USART_INTENSET_ABERREN_MASK)

/*! @name INTENCLR - Interrupt Enable Clear register. Allows clearing any combination of bits in the INTENSET register. Writing a 1 to any implemented bit position causes the corresponding bit to be cleared. */
#define USART_INTENCLR_TXIDLECLR_MASK            (0x8U)
#define USART_INTENCLR_TXIDLECLR_SHIFT           (3U)
#define USART_INTENCLR_TXIDLECLR(x)              (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_TXIDLECLR_SHIFT)) & USART_INTENCLR_TXIDLECLR_MASK)
#define USART_INTENCLR_DELTACTSCLR_MASK          (0x20U)
#define USART_INTENCLR_DELTACTSCLR_SHIFT         (5U)
#define USART_INTENCLR_DELTACTSCLR(x)            (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_DELTACTSCLR_SHIFT)) & USART_INTENCLR_DELTACTSCLR_MASK)
#define USART_INTENCLR_TXDISCLR_MASK             (0x40U)
#define USART_INTENCLR_TXDISCLR_SHIFT            (6U)
#define USART_INTENCLR_TXDISCLR(x)               (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_TXDISCLR_SHIFT)) & USART_INTENCLR_TXDISCLR_MASK)
#define USART_INTENCLR_DELTARXBRKCLR_MASK        (0x800U)
#define USART_INTENCLR_DELTARXBRKCLR_SHIFT       (11U)
#define USART_INTENCLR_DELTARXBRKCLR(x)          (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_DELTARXBRKCLR_SHIFT)) & USART_INTENCLR_DELTARXBRKCLR_MASK)
#define USART_INTENCLR_STARTCLR_MASK             (0x1000U)
#define USART_INTENCLR_STARTCLR_SHIFT            (12U)
#define USART_INTENCLR_STARTCLR(x)               (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_STARTCLR_SHIFT)) & USART_INTENCLR_STARTCLR_MASK)
#define USART_INTENCLR_FRAMERRCLR_MASK           (0x2000U)
#define USART_INTENCLR_FRAMERRCLR_SHIFT          (13U)
#define USART_INTENCLR_FRAMERRCLR(x)             (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_FRAMERRCLR_SHIFT)) & USART_INTENCLR_FRAMERRCLR_MASK)
#define USART_INTENCLR_PARITYERRCLR_MASK         (0x4000U)
#define USART_INTENCLR_PARITYERRCLR_SHIFT        (14U)
#define USART_INTENCLR_PARITYERRCLR(x)           (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_PARITYERRCLR_SHIFT)) & USART_INTENCLR_PARITYERRCLR_MASK)
#define USART_INTENCLR_RXNOISECLR_MASK           (0x8000U)
#define USART_INTENCLR_RXNOISECLR_SHIFT          (15U)
#define USART_INTENCLR_RXNOISECLR(x)             (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_RXNOISECLR_SHIFT)) & USART_INTENCLR_RXNOISECLR_MASK)
#define USART_INTENCLR_ABERRCLR_MASK             (0x10000U)
#define USART_INTENCLR_ABERRCLR_SHIFT            (16U)
#define USART_INTENCLR_ABERRCLR(x)               (((uint32_t)(((uint32_t)(x)) << USART_INTENCLR_ABERRCLR_SHIFT)) & USART_INTENCLR_ABERRCLR_MASK)

/*! @name BRG - Baud Rate Generator register. 16-bit integer baud rate divisor value. */
#define USART_BRG_BRGVAL_MASK                    (0xFFFFU)
#define USART_BRG_BRGVAL_SHIFT                   (0U)
#define USART_BRG_BRGVAL(x)                      (((uint32_t)(((uint32_t)(x)) << USART_BRG_BRGVAL_SHIFT)) & USART_BRG_BRGVAL_MASK)

/*! @name INTSTAT - Interrupt status register. Reflects interrupts that are currently enabled. */
#define USART_INTSTAT_TXIDLE_MASK                (0x8U)
#define USART_INTSTAT_TXIDLE_SHIFT               (3U)
#define USART_INTSTAT_TXIDLE(x)                  (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_TXIDLE_SHIFT)) & USART_INTSTAT_TXIDLE_MASK)
#define USART_INTSTAT_DELTACTS_MASK              (0x20U)
#define USART_INTSTAT_DELTACTS_SHIFT             (5U)
#define USART_INTSTAT_DELTACTS(x)                (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_DELTACTS_SHIFT)) & USART_INTSTAT_DELTACTS_MASK)
#define USART_INTSTAT_TXDISINT_MASK              (0x40U)
#define USART_INTSTAT_TXDISINT_SHIFT             (6U)
#define USART_INTSTAT_TXDISINT(x)                (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_TXDISINT_SHIFT)) & USART_INTSTAT_TXDISINT_MASK)
#define USART_INTSTAT_DELTARXBRK_MASK            (0x800U)
#define USART_INTSTAT_DELTARXBRK_SHIFT           (11U)
#define USART_INTSTAT_DELTARXBRK(x)              (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_DELTARXBRK_SHIFT)) & USART_INTSTAT_DELTARXBRK_MASK)
#define USART_INTSTAT_START_MASK                 (0x1000U)
#define USART_INTSTAT_START_SHIFT                (12U)
#define USART_INTSTAT_START(x)                   (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_START_SHIFT)) & USART_INTSTAT_START_MASK)
#define USART_INTSTAT_FRAMERRINT_MASK            (0x2000U)
#define USART_INTSTAT_FRAMERRINT_SHIFT           (13U)
#define USART_INTSTAT_FRAMERRINT(x)              (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_FRAMERRINT_SHIFT)) & USART_INTSTAT_FRAMERRINT_MASK)
#define USART_INTSTAT_PARITYERRINT_MASK          (0x4000U)
#define USART_INTSTAT_PARITYERRINT_SHIFT         (14U)
#define USART_INTSTAT_PARITYERRINT(x)            (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_PARITYERRINT_SHIFT)) & USART_INTSTAT_PARITYERRINT_MASK)
#define USART_INTSTAT_RXNOISEINT_MASK            (0x8000U)
#define USART_INTSTAT_RXNOISEINT_SHIFT           (15U)
#define USART_INTSTAT_RXNOISEINT(x)              (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_RXNOISEINT_SHIFT)) & USART_INTSTAT_RXNOISEINT_MASK)
#define USART_INTSTAT_ABERRINT_MASK              (0x10000U)
#define USART_INTSTAT_ABERRINT_SHIFT             (16U)
#define USART_INTSTAT_ABERRINT(x)                (((uint32_t)(((uint32_t)(x)) << USART_INTSTAT_ABERRINT_SHIFT)) & USART_INTSTAT_ABERRINT_MASK)

/*! @name OSR - Oversample selection register for asynchronous communication. */
#define USART_OSR_OSRVAL_MASK                    (0xFU)
#define USART_OSR_OSRVAL_SHIFT                   (0U)
#define USART_OSR_OSRVAL(x)                      (((uint32_t)(((uint32_t)(x)) << USART_OSR_OSRVAL_SHIFT)) & USART_OSR_OSRVAL_MASK)

/*! @name ADDR - Address register for automatic address matching. */
#define USART_ADDR_ADDRESS_MASK                  (0xFFU)
#define USART_ADDR_ADDRESS_SHIFT                 (0U)
#define USART_ADDR_ADDRESS(x)                    (((uint32_t)(((uint32_t)(x)) << USART_ADDR_ADDRESS_SHIFT)) & USART_ADDR_ADDRESS_MASK)

/*! @name FIFOCFG - FIFO configuration and enable register. */
#define USART_FIFOCFG_ENABLETX_MASK              (0x1U)
#define USART_FIFOCFG_ENABLETX_SHIFT             (0U)
#define USART_FIFOCFG_ENABLETX(x)                (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_ENABLETX_SHIFT)) & USART_FIFOCFG_ENABLETX_MASK)
#define USART_FIFOCFG_ENABLERX_MASK              (0x2U)
#define USART_FIFOCFG_ENABLERX_SHIFT             (1U)
#define USART_FIFOCFG_ENABLERX(x)                (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_ENABLERX_SHIFT)) & USART_FIFOCFG_ENABLERX_MASK)
#define USART_FIFOCFG_SIZE_MASK                  (0x30U)
#define USART_FIFOCFG_SIZE_SHIFT                 (4U)
#define USART_FIFOCFG_SIZE(x)                    (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_SIZE_SHIFT)) & USART_FIFOCFG_SIZE_MASK)
#define USART_FIFOCFG_DMATX_MASK                 (0x1000U)
#define USART_FIFOCFG_DMATX_SHIFT                (12U)
#define USART_FIFOCFG_DMATX(x)                   (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_DMATX_SHIFT)) & USART_FIFOCFG_DMATX_MASK)
#define USART_FIFOCFG_DMARX_MASK                 (0x2000U)
#define USART_FIFOCFG_DMARX_SHIFT                (13U)
#define USART_FIFOCFG_DMARX(x)                   (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_DMARX_SHIFT)) & USART_FIFOCFG_DMARX_MASK)
#define USART_FIFOCFG_EMPTYTX_MASK               (0x10000U)
#define USART_FIFOCFG_EMPTYTX_SHIFT              (16U)
#define USART_FIFOCFG_EMPTYTX(x)                 (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_EMPTYTX_SHIFT)) & USART_FIFOCFG_EMPTYTX_MASK)
#define USART_FIFOCFG_EMPTYRX_MASK               (0x20000U)
#define USART_FIFOCFG_EMPTYRX_SHIFT              (17U)
#define USART_FIFOCFG_EMPTYRX(x)                 (((uint32_t)(((uint32_t)(x)) << USART_FIFOCFG_EMPTYRX_SHIFT)) & USART_FIFOCFG_EMPTYRX_MASK)

/*! @name FIFOSTAT - FIFO status register. */
#define USART_FIFOSTAT_TXERR_MASK                (0x1U)
#define USART_FIFOSTAT_TXERR_SHIFT               (0U)
#define USART_FIFOSTAT_TXERR(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_TXERR_SHIFT)) & USART_FIFOSTAT_TXERR_MASK)
#define USART_FIFOSTAT_RXERR_MASK                (0x2U)
#define USART_FIFOSTAT_RXERR_SHIFT               (1U)
#define USART_FIFOSTAT_RXERR(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_RXERR_SHIFT)) & USART_FIFOSTAT_RXERR_MASK)
#define USART_FIFOSTAT_PERINT_MASK               (0x8U)
#define USART_FIFOSTAT_PERINT_SHIFT              (3U)
#define USART_FIFOSTAT_PERINT(x)                 (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_PERINT_SHIFT)) & USART_FIFOSTAT_PERINT_MASK)
#define USART_FIFOSTAT_TXEMPTY_MASK              (0x10U)
#define USART_FIFOSTAT_TXEMPTY_SHIFT             (4U)
#define USART_FIFOSTAT_TXEMPTY(x)                (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_TXEMPTY_SHIFT)) & USART_FIFOSTAT_TXEMPTY_MASK)
#define USART_FIFOSTAT_TXNOTFULL_MASK            (0x20U)
#define USART_FIFOSTAT_TXNOTFULL_SHIFT           (5U)
#define USART_FIFOSTAT_TXNOTFULL(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_TXNOTFULL_SHIFT)) & USART_FIFOSTAT_TXNOTFULL_MASK)
#define USART_FIFOSTAT_RXNOTEMPTY_MASK           (0x40U)
#define USART_FIFOSTAT_RXNOTEMPTY_SHIFT          (6U)
#define USART_FIFOSTAT_RXNOTEMPTY(x)             (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_RXNOTEMPTY_SHIFT)) & USART_FIFOSTAT_RXNOTEMPTY_MASK)
#define USART_FIFOSTAT_RXFULL_MASK               (0x80U)
#define USART_FIFOSTAT_RXFULL_SHIFT              (7U)
#define USART_FIFOSTAT_RXFULL(x)                 (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_RXFULL_SHIFT)) & USART_FIFOSTAT_RXFULL_MASK)
#define USART_FIFOSTAT_TXLVL_MASK                (0x1F00U)
#define USART_FIFOSTAT_TXLVL_SHIFT               (8U)
#define USART_FIFOSTAT_TXLVL(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_TXLVL_SHIFT)) & USART_FIFOSTAT_TXLVL_MASK)
#define USART_FIFOSTAT_RXLVL_MASK                (0x1F0000U)
#define USART_FIFOSTAT_RXLVL_SHIFT               (16U)
#define USART_FIFOSTAT_RXLVL(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFOSTAT_RXLVL_SHIFT)) & USART_FIFOSTAT_RXLVL_MASK)

/*! @name FIFOTRIG - FIFO trigger settings for interrupt and DMA request. */
#define USART_FIFOTRIG_TXLVLENA_MASK             (0x1U)
#define USART_FIFOTRIG_TXLVLENA_SHIFT            (0U)
#define USART_FIFOTRIG_TXLVLENA(x)               (((uint32_t)(((uint32_t)(x)) << USART_FIFOTRIG_TXLVLENA_SHIFT)) & USART_FIFOTRIG_TXLVLENA_MASK)
#define USART_FIFOTRIG_RXLVLENA_MASK             (0x2U)
#define USART_FIFOTRIG_RXLVLENA_SHIFT            (1U)
#define USART_FIFOTRIG_RXLVLENA(x)               (((uint32_t)(((uint32_t)(x)) << USART_FIFOTRIG_RXLVLENA_SHIFT)) & USART_FIFOTRIG_RXLVLENA_MASK)
#define USART_FIFOTRIG_TXLVL_MASK                (0xF00U)
#define USART_FIFOTRIG_TXLVL_SHIFT               (8U)
#define USART_FIFOTRIG_TXLVL(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFOTRIG_TXLVL_SHIFT)) & USART_FIFOTRIG_TXLVL_MASK)
#define USART_FIFOTRIG_RXLVL_MASK                (0xF0000U)
#define USART_FIFOTRIG_RXLVL_SHIFT               (16U)
#define USART_FIFOTRIG_RXLVL(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFOTRIG_RXLVL_SHIFT)) & USART_FIFOTRIG_RXLVL_MASK)

/*! @name FIFOINTENSET - FIFO interrupt enable set (enable) and read register. */
#define USART_FIFOINTENSET_TXERR_MASK            (0x1U)
#define USART_FIFOINTENSET_TXERR_SHIFT           (0U)
#define USART_FIFOINTENSET_TXERR(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENSET_TXERR_SHIFT)) & USART_FIFOINTENSET_TXERR_MASK)
#define USART_FIFOINTENSET_RXERR_MASK            (0x2U)
#define USART_FIFOINTENSET_RXERR_SHIFT           (1U)
#define USART_FIFOINTENSET_RXERR(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENSET_RXERR_SHIFT)) & USART_FIFOINTENSET_RXERR_MASK)
#define USART_FIFOINTENSET_TXLVL_MASK            (0x4U)
#define USART_FIFOINTENSET_TXLVL_SHIFT           (2U)
#define USART_FIFOINTENSET_TXLVL(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENSET_TXLVL_SHIFT)) & USART_FIFOINTENSET_TXLVL_MASK)
#define USART_FIFOINTENSET_RXLVL_MASK            (0x8U)
#define USART_FIFOINTENSET_RXLVL_SHIFT           (3U)
#define USART_FIFOINTENSET_RXLVL(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENSET_RXLVL_SHIFT)) & USART_FIFOINTENSET_RXLVL_MASK)

/*! @name FIFOINTENCLR - FIFO interrupt enable clear (disable) and read register. */
#define USART_FIFOINTENCLR_TXERR_MASK            (0x1U)
#define USART_FIFOINTENCLR_TXERR_SHIFT           (0U)
#define USART_FIFOINTENCLR_TXERR(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENCLR_TXERR_SHIFT)) & USART_FIFOINTENCLR_TXERR_MASK)
#define USART_FIFOINTENCLR_RXERR_MASK            (0x2U)
#define USART_FIFOINTENCLR_RXERR_SHIFT           (1U)
#define USART_FIFOINTENCLR_RXERR(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENCLR_RXERR_SHIFT)) & USART_FIFOINTENCLR_RXERR_MASK)
#define USART_FIFOINTENCLR_TXLVL_MASK            (0x4U)
#define USART_FIFOINTENCLR_TXLVL_SHIFT           (2U)
#define USART_FIFOINTENCLR_TXLVL(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENCLR_TXLVL_SHIFT)) & USART_FIFOINTENCLR_TXLVL_MASK)
#define USART_FIFOINTENCLR_RXLVL_MASK            (0x8U)
#define USART_FIFOINTENCLR_RXLVL_SHIFT           (3U)
#define USART_FIFOINTENCLR_RXLVL(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTENCLR_RXLVL_SHIFT)) & USART_FIFOINTENCLR_RXLVL_MASK)

/*! @name FIFOINTSTAT - FIFO interrupt status register. */
#define USART_FIFOINTSTAT_TXERR_MASK             (0x1U)
#define USART_FIFOINTSTAT_TXERR_SHIFT            (0U)
#define USART_FIFOINTSTAT_TXERR(x)               (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTSTAT_TXERR_SHIFT)) & USART_FIFOINTSTAT_TXERR_MASK)
#define USART_FIFOINTSTAT_RXERR_MASK             (0x2U)
#define USART_FIFOINTSTAT_RXERR_SHIFT            (1U)
#define USART_FIFOINTSTAT_RXERR(x)               (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTSTAT_RXERR_SHIFT)) & USART_FIFOINTSTAT_RXERR_MASK)
#define USART_FIFOINTSTAT_TXLVL_MASK             (0x4U)
#define USART_FIFOINTSTAT_TXLVL_SHIFT            (2U)
#define USART_FIFOINTSTAT_TXLVL(x)               (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTSTAT_TXLVL_SHIFT)) & USART_FIFOINTSTAT_TXLVL_MASK)
#define USART_FIFOINTSTAT_RXLVL_MASK             (0x8U)
#define USART_FIFOINTSTAT_RXLVL_SHIFT            (3U)
#define USART_FIFOINTSTAT_RXLVL(x)               (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTSTAT_RXLVL_SHIFT)) & USART_FIFOINTSTAT_RXLVL_MASK)
#define USART_FIFOINTSTAT_PERINT_MASK            (0x10U)
#define USART_FIFOINTSTAT_PERINT_SHIFT           (4U)
#define USART_FIFOINTSTAT_PERINT(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFOINTSTAT_PERINT_SHIFT)) & USART_FIFOINTSTAT_PERINT_MASK)

/*! @name FIFOWR - FIFO write data. */
#define USART_FIFOWR_TXDATA_MASK                 (0x1FFU)
#define USART_FIFOWR_TXDATA_SHIFT                (0U)
#define USART_FIFOWR_TXDATA(x)                   (((uint32_t)(((uint32_t)(x)) << USART_FIFOWR_TXDATA_SHIFT)) & USART_FIFOWR_TXDATA_MASK)

/*! @name FIFORD - FIFO read data. */
#define USART_FIFORD_RXDATA_MASK                 (0x1FFU)
#define USART_FIFORD_RXDATA_SHIFT                (0U)
#define USART_FIFORD_RXDATA(x)                   (((uint32_t)(((uint32_t)(x)) << USART_FIFORD_RXDATA_SHIFT)) & USART_FIFORD_RXDATA_MASK)
#define USART_FIFORD_FRAMERR_MASK                (0x2000U)
#define USART_FIFORD_FRAMERR_SHIFT               (13U)
#define USART_FIFORD_FRAMERR(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFORD_FRAMERR_SHIFT)) & USART_FIFORD_FRAMERR_MASK)
#define USART_FIFORD_PARITYERR_MASK              (0x4000U)
#define USART_FIFORD_PARITYERR_SHIFT             (14U)
#define USART_FIFORD_PARITYERR(x)                (((uint32_t)(((uint32_t)(x)) << USART_FIFORD_PARITYERR_SHIFT)) & USART_FIFORD_PARITYERR_MASK)
#define USART_FIFORD_RXNOISE_MASK                (0x8000U)
#define USART_FIFORD_RXNOISE_SHIFT               (15U)
#define USART_FIFORD_RXNOISE(x)                  (((uint32_t)(((uint32_t)(x)) << USART_FIFORD_RXNOISE_SHIFT)) & USART_FIFORD_RXNOISE_MASK)

/*! @name FIFORDNOPOP - FIFO data read with no FIFO pop. */
#define USART_FIFORDNOPOP_RXDATA_MASK            (0x1FFU)
#define USART_FIFORDNOPOP_RXDATA_SHIFT           (0U)
#define USART_FIFORDNOPOP_RXDATA(x)              (((uint32_t)(((uint32_t)(x)) << USART_FIFORDNOPOP_RXDATA_SHIFT)) & USART_FIFORDNOPOP_RXDATA_MASK)
#define USART_FIFORDNOPOP_FRAMERR_MASK           (0x2000U)
#define USART_FIFORDNOPOP_FRAMERR_SHIFT          (13U)
#define USART_FIFORDNOPOP_FRAMERR(x)             (((uint32_t)(((uint32_t)(x)) << USART_FIFORDNOPOP_FRAMERR_SHIFT)) & USART_FIFORDNOPOP_FRAMERR_MASK)
#define USART_FIFORDNOPOP_PARITYERR_MASK         (0x4000U)
#define USART_FIFORDNOPOP_PARITYERR_SHIFT        (14U)
#define USART_FIFORDNOPOP_PARITYERR(x)           (((uint32_t)(((uint32_t)(x)) << USART_FIFORDNOPOP_PARITYERR_SHIFT)) & USART_FIFORDNOPOP_PARITYERR_MASK)
#define USART_FIFORDNOPOP_RXNOISE_MASK           (0x8000U)
#define USART_FIFORDNOPOP_RXNOISE_SHIFT          (15U)
#define USART_FIFORDNOPOP_RXNOISE(x)             (((uint32_t)(((uint32_t)(x)) << USART_FIFORDNOPOP_RXNOISE_SHIFT)) & USART_FIFORDNOPOP_RXNOISE_MASK)

/*! @name ID - USART module Identification. This value appears in the shared Flexcomm peripheral ID register when USART is selected. */
#define USART_ID_APERTURE_MASK                   (0xFFU)
#define USART_ID_APERTURE_SHIFT                  (0U)
#define USART_ID_APERTURE(x)                     (((uint32_t)(((uint32_t)(x)) << USART_ID_APERTURE_SHIFT)) & USART_ID_APERTURE_MASK)
#define USART_ID_MINOR_REV_MASK                  (0xF00U)
#define USART_ID_MINOR_REV_SHIFT                 (8U)
#define USART_ID_MINOR_REV(x)                    (((uint32_t)(((uint32_t)(x)) << USART_ID_MINOR_REV_SHIFT)) & USART_ID_MINOR_REV_MASK)
#define USART_ID_MAJOR_REV_MASK                  (0xF000U)
#define USART_ID_MAJOR_REV_SHIFT                 (12U)
#define USART_ID_MAJOR_REV(x)                    (((uint32_t)(((uint32_t)(x)) << USART_ID_MAJOR_REV_SHIFT)) & USART_ID_MAJOR_REV_MASK)
#define USART_ID_ID_MASK                         (0xFFFF0000U)
#define USART_ID_ID_SHIFT                        (16U)
#define USART_ID_ID(x)                           (((uint32_t)(((uint32_t)(x)) << USART_ID_ID_SHIFT)) & USART_ID_ID_MASK)


/*!
 * @}
 */ /* end of group USART_Register_Masks */


/* USART - Peripheral instance base addresses */
/** Peripheral USART0 base address */
#define USART0_BASE                              (0x40083000u)
/** Peripheral USART0 base pointer */
#define USART0                                   ((USART_Type *)USART0_BASE)
/** Peripheral USART1 base address */
#define USART1_BASE                              (0x40086000u)
/** Peripheral USART1 base pointer */
#define USART1                                   ((USART_Type *)USART1_BASE)
/** Array initializer of USART peripheral base addresses */
#define USART_BASE_ADDRS                         { USART0_BASE, USART1_BASE }
/** Array initializer of USART peripheral base pointers */
#define USART_BASE_PTRS                          { USART0, USART1 }
/** Interrupt vectors for the USART peripheral type */
#define USART_IRQS                               { FLEXCOMM0_IRQn, FLEXCOMM1_IRQn }

/*!
 * @}
 */ /* end of group USART_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- USB Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup USB_Peripheral_Access_Layer USB Peripheral Access Layer
 * @{
 */

/** USB - Register Layout Typedef */
typedef struct {
  __IO uint32_t DEVCMDSTAT;                        /**< USB Device Command/Status register, offset: 0x0 */
  __IO uint32_t INFO;                              /**< USB Info register, offset: 0x4 */
  __IO uint32_t EPLISTSTART;                       /**< USB EP Command/Status List start address, offset: 0x8 */
  __IO uint32_t DATABUFSTART;                      /**< USB Data buffer start address, offset: 0xC */
  __IO uint32_t LPM;                               /**< USB Link Power Management register, offset: 0x10 */
  __IO uint32_t EPSKIP;                            /**< USB Endpoint skip, offset: 0x14 */
  __IO uint32_t EPINUSE;                           /**< USB Endpoint Buffer in use, offset: 0x18 */
  __IO uint32_t EPBUFCFG;                          /**< USB Endpoint Buffer Configuration register, offset: 0x1C */
  __IO uint32_t INTSTAT;                           /**< USB interrupt status register, offset: 0x20 */
  __IO uint32_t INTEN;                             /**< USB interrupt enable register, offset: 0x24 */
  __IO uint32_t INTSETSTAT;                        /**< USB set interrupt status register, offset: 0x28 */
       uint8_t RESERVED_0[8];
  __I  uint32_t EPTOGGLE;                          /**< USB Endpoint toggle register, offset: 0x34 */
} USB_Type;

/* ----------------------------------------------------------------------------
   -- USB Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup USB_Register_Masks USB Register Masks
 * @{
 */

/*! @name DEVCMDSTAT - USB Device Command/Status register */
#define USB_DEVCMDSTAT_DEV_ADDR_MASK             (0x7FU)
#define USB_DEVCMDSTAT_DEV_ADDR_SHIFT            (0U)
#define USB_DEVCMDSTAT_DEV_ADDR(x)               (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DEV_ADDR_SHIFT)) & USB_DEVCMDSTAT_DEV_ADDR_MASK)
#define USB_DEVCMDSTAT_DEV_EN_MASK               (0x80U)
#define USB_DEVCMDSTAT_DEV_EN_SHIFT              (7U)
#define USB_DEVCMDSTAT_DEV_EN(x)                 (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DEV_EN_SHIFT)) & USB_DEVCMDSTAT_DEV_EN_MASK)
#define USB_DEVCMDSTAT_SETUP_MASK                (0x100U)
#define USB_DEVCMDSTAT_SETUP_SHIFT               (8U)
#define USB_DEVCMDSTAT_SETUP(x)                  (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_SETUP_SHIFT)) & USB_DEVCMDSTAT_SETUP_MASK)
#define USB_DEVCMDSTAT_FORCE_NEEDCLK_MASK        (0x200U)
#define USB_DEVCMDSTAT_FORCE_NEEDCLK_SHIFT       (9U)
#define USB_DEVCMDSTAT_FORCE_NEEDCLK(x)          (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_FORCE_NEEDCLK_SHIFT)) & USB_DEVCMDSTAT_FORCE_NEEDCLK_MASK)
#define USB_DEVCMDSTAT_LPM_SUP_MASK              (0x800U)
#define USB_DEVCMDSTAT_LPM_SUP_SHIFT             (11U)
#define USB_DEVCMDSTAT_LPM_SUP(x)                (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_LPM_SUP_SHIFT)) & USB_DEVCMDSTAT_LPM_SUP_MASK)
#define USB_DEVCMDSTAT_INTONNAK_AO_MASK          (0x1000U)
#define USB_DEVCMDSTAT_INTONNAK_AO_SHIFT         (12U)
#define USB_DEVCMDSTAT_INTONNAK_AO(x)            (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_INTONNAK_AO_SHIFT)) & USB_DEVCMDSTAT_INTONNAK_AO_MASK)
#define USB_DEVCMDSTAT_INTONNAK_AI_MASK          (0x2000U)
#define USB_DEVCMDSTAT_INTONNAK_AI_SHIFT         (13U)
#define USB_DEVCMDSTAT_INTONNAK_AI(x)            (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_INTONNAK_AI_SHIFT)) & USB_DEVCMDSTAT_INTONNAK_AI_MASK)
#define USB_DEVCMDSTAT_INTONNAK_CO_MASK          (0x4000U)
#define USB_DEVCMDSTAT_INTONNAK_CO_SHIFT         (14U)
#define USB_DEVCMDSTAT_INTONNAK_CO(x)            (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_INTONNAK_CO_SHIFT)) & USB_DEVCMDSTAT_INTONNAK_CO_MASK)
#define USB_DEVCMDSTAT_INTONNAK_CI_MASK          (0x8000U)
#define USB_DEVCMDSTAT_INTONNAK_CI_SHIFT         (15U)
#define USB_DEVCMDSTAT_INTONNAK_CI(x)            (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_INTONNAK_CI_SHIFT)) & USB_DEVCMDSTAT_INTONNAK_CI_MASK)
#define USB_DEVCMDSTAT_DCON_MASK                 (0x10000U)
#define USB_DEVCMDSTAT_DCON_SHIFT                (16U)
#define USB_DEVCMDSTAT_DCON(x)                   (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DCON_SHIFT)) & USB_DEVCMDSTAT_DCON_MASK)
#define USB_DEVCMDSTAT_DSUS_MASK                 (0x20000U)
#define USB_DEVCMDSTAT_DSUS_SHIFT                (17U)
#define USB_DEVCMDSTAT_DSUS(x)                   (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DSUS_SHIFT)) & USB_DEVCMDSTAT_DSUS_MASK)
#define USB_DEVCMDSTAT_LPM_SUS_MASK              (0x80000U)
#define USB_DEVCMDSTAT_LPM_SUS_SHIFT             (19U)
#define USB_DEVCMDSTAT_LPM_SUS(x)                (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_LPM_SUS_SHIFT)) & USB_DEVCMDSTAT_LPM_SUS_MASK)
#define USB_DEVCMDSTAT_LPM_REWP_MASK             (0x100000U)
#define USB_DEVCMDSTAT_LPM_REWP_SHIFT            (20U)
#define USB_DEVCMDSTAT_LPM_REWP(x)               (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_LPM_REWP_SHIFT)) & USB_DEVCMDSTAT_LPM_REWP_MASK)
#define USB_DEVCMDSTAT_DCON_C_MASK               (0x1000000U)
#define USB_DEVCMDSTAT_DCON_C_SHIFT              (24U)
#define USB_DEVCMDSTAT_DCON_C(x)                 (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DCON_C_SHIFT)) & USB_DEVCMDSTAT_DCON_C_MASK)
#define USB_DEVCMDSTAT_DSUS_C_MASK               (0x2000000U)
#define USB_DEVCMDSTAT_DSUS_C_SHIFT              (25U)
#define USB_DEVCMDSTAT_DSUS_C(x)                 (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DSUS_C_SHIFT)) & USB_DEVCMDSTAT_DSUS_C_MASK)
#define USB_DEVCMDSTAT_DRES_C_MASK               (0x4000000U)
#define USB_DEVCMDSTAT_DRES_C_SHIFT              (26U)
#define USB_DEVCMDSTAT_DRES_C(x)                 (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_DRES_C_SHIFT)) & USB_DEVCMDSTAT_DRES_C_MASK)
#define USB_DEVCMDSTAT_VBUSDEBOUNCED_MASK        (0x10000000U)
#define USB_DEVCMDSTAT_VBUSDEBOUNCED_SHIFT       (28U)
#define USB_DEVCMDSTAT_VBUSDEBOUNCED(x)          (((uint32_t)(((uint32_t)(x)) << USB_DEVCMDSTAT_VBUSDEBOUNCED_SHIFT)) & USB_DEVCMDSTAT_VBUSDEBOUNCED_MASK)

/*! @name INFO - USB Info register */
#define USB_INFO_FRAME_NR_MASK                   (0x7FFU)
#define USB_INFO_FRAME_NR_SHIFT                  (0U)
#define USB_INFO_FRAME_NR(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INFO_FRAME_NR_SHIFT)) & USB_INFO_FRAME_NR_MASK)
#define USB_INFO_ERR_CODE_MASK                   (0x7800U)
#define USB_INFO_ERR_CODE_SHIFT                  (11U)
#define USB_INFO_ERR_CODE(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INFO_ERR_CODE_SHIFT)) & USB_INFO_ERR_CODE_MASK)

/*! @name EPLISTSTART - USB EP Command/Status List start address */
#define USB_EPLISTSTART_EP_LIST_MASK             (0xFFFFFF00U)
#define USB_EPLISTSTART_EP_LIST_SHIFT            (8U)
#define USB_EPLISTSTART_EP_LIST(x)               (((uint32_t)(((uint32_t)(x)) << USB_EPLISTSTART_EP_LIST_SHIFT)) & USB_EPLISTSTART_EP_LIST_MASK)

/*! @name DATABUFSTART - USB Data buffer start address */
#define USB_DATABUFSTART_DA_BUF_MASK             (0xFFC00000U)
#define USB_DATABUFSTART_DA_BUF_SHIFT            (22U)
#define USB_DATABUFSTART_DA_BUF(x)               (((uint32_t)(((uint32_t)(x)) << USB_DATABUFSTART_DA_BUF_SHIFT)) & USB_DATABUFSTART_DA_BUF_MASK)

/*! @name LPM - USB Link Power Management register */
#define USB_LPM_HIRD_HW_MASK                     (0xFU)
#define USB_LPM_HIRD_HW_SHIFT                    (0U)
#define USB_LPM_HIRD_HW(x)                       (((uint32_t)(((uint32_t)(x)) << USB_LPM_HIRD_HW_SHIFT)) & USB_LPM_HIRD_HW_MASK)
#define USB_LPM_HIRD_SW_MASK                     (0xF0U)
#define USB_LPM_HIRD_SW_SHIFT                    (4U)
#define USB_LPM_HIRD_SW(x)                       (((uint32_t)(((uint32_t)(x)) << USB_LPM_HIRD_SW_SHIFT)) & USB_LPM_HIRD_SW_MASK)
#define USB_LPM_DATA_PENDING_MASK                (0x100U)
#define USB_LPM_DATA_PENDING_SHIFT               (8U)
#define USB_LPM_DATA_PENDING(x)                  (((uint32_t)(((uint32_t)(x)) << USB_LPM_DATA_PENDING_SHIFT)) & USB_LPM_DATA_PENDING_MASK)

/*! @name EPSKIP - USB Endpoint skip */
#define USB_EPSKIP_SKIP_MASK                     (0x3FFFFFFFU)
#define USB_EPSKIP_SKIP_SHIFT                    (0U)
#define USB_EPSKIP_SKIP(x)                       (((uint32_t)(((uint32_t)(x)) << USB_EPSKIP_SKIP_SHIFT)) & USB_EPSKIP_SKIP_MASK)

/*! @name EPINUSE - USB Endpoint Buffer in use */
#define USB_EPINUSE_BUF_MASK                     (0x3FCU)
#define USB_EPINUSE_BUF_SHIFT                    (2U)
#define USB_EPINUSE_BUF(x)                       (((uint32_t)(((uint32_t)(x)) << USB_EPINUSE_BUF_SHIFT)) & USB_EPINUSE_BUF_MASK)

/*! @name EPBUFCFG - USB Endpoint Buffer Configuration register */
#define USB_EPBUFCFG_BUF_SB_MASK                 (0x3FCU)
#define USB_EPBUFCFG_BUF_SB_SHIFT                (2U)
#define USB_EPBUFCFG_BUF_SB(x)                   (((uint32_t)(((uint32_t)(x)) << USB_EPBUFCFG_BUF_SB_SHIFT)) & USB_EPBUFCFG_BUF_SB_MASK)

/*! @name INTSTAT - USB interrupt status register */
#define USB_INTSTAT_EP0OUT_MASK                  (0x1U)
#define USB_INTSTAT_EP0OUT_SHIFT                 (0U)
#define USB_INTSTAT_EP0OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP0OUT_SHIFT)) & USB_INTSTAT_EP0OUT_MASK)
#define USB_INTSTAT_EP0IN_MASK                   (0x2U)
#define USB_INTSTAT_EP0IN_SHIFT                  (1U)
#define USB_INTSTAT_EP0IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP0IN_SHIFT)) & USB_INTSTAT_EP0IN_MASK)
#define USB_INTSTAT_EP1OUT_MASK                  (0x4U)
#define USB_INTSTAT_EP1OUT_SHIFT                 (2U)
#define USB_INTSTAT_EP1OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP1OUT_SHIFT)) & USB_INTSTAT_EP1OUT_MASK)
#define USB_INTSTAT_EP1IN_MASK                   (0x8U)
#define USB_INTSTAT_EP1IN_SHIFT                  (3U)
#define USB_INTSTAT_EP1IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP1IN_SHIFT)) & USB_INTSTAT_EP1IN_MASK)
#define USB_INTSTAT_EP2OUT_MASK                  (0x10U)
#define USB_INTSTAT_EP2OUT_SHIFT                 (4U)
#define USB_INTSTAT_EP2OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP2OUT_SHIFT)) & USB_INTSTAT_EP2OUT_MASK)
#define USB_INTSTAT_EP2IN_MASK                   (0x20U)
#define USB_INTSTAT_EP2IN_SHIFT                  (5U)
#define USB_INTSTAT_EP2IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP2IN_SHIFT)) & USB_INTSTAT_EP2IN_MASK)
#define USB_INTSTAT_EP3OUT_MASK                  (0x40U)
#define USB_INTSTAT_EP3OUT_SHIFT                 (6U)
#define USB_INTSTAT_EP3OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP3OUT_SHIFT)) & USB_INTSTAT_EP3OUT_MASK)
#define USB_INTSTAT_EP3IN_MASK                   (0x80U)
#define USB_INTSTAT_EP3IN_SHIFT                  (7U)
#define USB_INTSTAT_EP3IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP3IN_SHIFT)) & USB_INTSTAT_EP3IN_MASK)
#define USB_INTSTAT_EP4OUT_MASK                  (0x100U)
#define USB_INTSTAT_EP4OUT_SHIFT                 (8U)
#define USB_INTSTAT_EP4OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP4OUT_SHIFT)) & USB_INTSTAT_EP4OUT_MASK)
#define USB_INTSTAT_EP4IN_MASK                   (0x200U)
#define USB_INTSTAT_EP4IN_SHIFT                  (9U)
#define USB_INTSTAT_EP4IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP4IN_SHIFT)) & USB_INTSTAT_EP4IN_MASK)
#define USB_INTSTAT_EP5OUT_MASK                  (0x400U)
#define USB_INTSTAT_EP5OUT_SHIFT                 (10U)
#define USB_INTSTAT_EP5OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP5OUT_SHIFT)) & USB_INTSTAT_EP5OUT_MASK)
#define USB_INTSTAT_EP5IN_MASK                   (0x800U)
#define USB_INTSTAT_EP5IN_SHIFT                  (11U)
#define USB_INTSTAT_EP5IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP5IN_SHIFT)) & USB_INTSTAT_EP5IN_MASK)
#define USB_INTSTAT_EP6OUT_MASK                  (0x1000U)
#define USB_INTSTAT_EP6OUT_SHIFT                 (12U)
#define USB_INTSTAT_EP6OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP6OUT_SHIFT)) & USB_INTSTAT_EP6OUT_MASK)
#define USB_INTSTAT_EP6IN_MASK                   (0x2000U)
#define USB_INTSTAT_EP6IN_SHIFT                  (13U)
#define USB_INTSTAT_EP6IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP6IN_SHIFT)) & USB_INTSTAT_EP6IN_MASK)
#define USB_INTSTAT_EP7OUT_MASK                  (0x4000U)
#define USB_INTSTAT_EP7OUT_SHIFT                 (14U)
#define USB_INTSTAT_EP7OUT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP7OUT_SHIFT)) & USB_INTSTAT_EP7OUT_MASK)
#define USB_INTSTAT_EP7IN_MASK                   (0x8000U)
#define USB_INTSTAT_EP7IN_SHIFT                  (15U)
#define USB_INTSTAT_EP7IN(x)                     (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_EP7IN_SHIFT)) & USB_INTSTAT_EP7IN_MASK)
#define USB_INTSTAT_FRAME_INT_MASK               (0x40000000U)
#define USB_INTSTAT_FRAME_INT_SHIFT              (30U)
#define USB_INTSTAT_FRAME_INT(x)                 (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_FRAME_INT_SHIFT)) & USB_INTSTAT_FRAME_INT_MASK)
#define USB_INTSTAT_DEV_INT_MASK                 (0x80000000U)
#define USB_INTSTAT_DEV_INT_SHIFT                (31U)
#define USB_INTSTAT_DEV_INT(x)                   (((uint32_t)(((uint32_t)(x)) << USB_INTSTAT_DEV_INT_SHIFT)) & USB_INTSTAT_DEV_INT_MASK)

/*! @name INTEN - USB interrupt enable register */
#define USB_INTEN_EP_INT_EN_MASK                 (0xFFFFU)
#define USB_INTEN_EP_INT_EN_SHIFT                (0U)
#define USB_INTEN_EP_INT_EN(x)                   (((uint32_t)(((uint32_t)(x)) << USB_INTEN_EP_INT_EN_SHIFT)) & USB_INTEN_EP_INT_EN_MASK)
#define USB_INTEN_FRAME_INT_EN_MASK              (0x40000000U)
#define USB_INTEN_FRAME_INT_EN_SHIFT             (30U)
#define USB_INTEN_FRAME_INT_EN(x)                (((uint32_t)(((uint32_t)(x)) << USB_INTEN_FRAME_INT_EN_SHIFT)) & USB_INTEN_FRAME_INT_EN_MASK)
#define USB_INTEN_DEV_INT_EN_MASK                (0x80000000U)
#define USB_INTEN_DEV_INT_EN_SHIFT               (31U)
#define USB_INTEN_DEV_INT_EN(x)                  (((uint32_t)(((uint32_t)(x)) << USB_INTEN_DEV_INT_EN_SHIFT)) & USB_INTEN_DEV_INT_EN_MASK)

/*! @name INTSETSTAT - USB set interrupt status register */
#define USB_INTSETSTAT_EP_SET_INT_MASK           (0xFFFFU)
#define USB_INTSETSTAT_EP_SET_INT_SHIFT          (0U)
#define USB_INTSETSTAT_EP_SET_INT(x)             (((uint32_t)(((uint32_t)(x)) << USB_INTSETSTAT_EP_SET_INT_SHIFT)) & USB_INTSETSTAT_EP_SET_INT_MASK)
#define USB_INTSETSTAT_FRAME_SET_INT_MASK        (0x40000000U)
#define USB_INTSETSTAT_FRAME_SET_INT_SHIFT       (30U)
#define USB_INTSETSTAT_FRAME_SET_INT(x)          (((uint32_t)(((uint32_t)(x)) << USB_INTSETSTAT_FRAME_SET_INT_SHIFT)) & USB_INTSETSTAT_FRAME_SET_INT_MASK)
#define USB_INTSETSTAT_DEV_SET_INT_MASK          (0x80000000U)
#define USB_INTSETSTAT_DEV_SET_INT_SHIFT         (31U)
#define USB_INTSETSTAT_DEV_SET_INT(x)            (((uint32_t)(((uint32_t)(x)) << USB_INTSETSTAT_DEV_SET_INT_SHIFT)) & USB_INTSETSTAT_DEV_SET_INT_MASK)

/*! @name EPTOGGLE - USB Endpoint toggle register */
#define USB_EPTOGGLE_TOGGLE_MASK                 (0xFFFFU)
#define USB_EPTOGGLE_TOGGLE_SHIFT                (0U)
#define USB_EPTOGGLE_TOGGLE(x)                   (((uint32_t)(((uint32_t)(x)) << USB_EPTOGGLE_TOGGLE_SHIFT)) & USB_EPTOGGLE_TOGGLE_MASK)


/*!
 * @}
 */ /* end of group USB_Register_Masks */


/* USB - Peripheral instance base addresses */
/** Peripheral USB0 base address */
#define USB0_BASE                                (0x40084000u)
/** Peripheral USB0 base pointer */
#define USB0                                     ((USB_Type *)USB0_BASE)
/** Array initializer of USB peripheral base addresses */
#define USB_BASE_ADDRS                           { USB0_BASE }
/** Array initializer of USB peripheral base pointers */
#define USB_BASE_PTRS                            { USB0 }
/** Interrupt vectors for the USB peripheral type */
#define USB_IRQS                                 { USB0_IRQn }

/*!
 * @}
 */ /* end of group USB_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- WDT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDT_Peripheral_Access_Layer WDT Peripheral Access Layer
 * @{
 */

/** WDT - Register Layout Typedef */
typedef struct {
  __IO uint32_t LOAD;                              /**< watch dog counter start value register, offset: 0x0 */
  __IO uint32_t VALUE;                             /**< watch dog counter value register, offset: 0x4 */
  __IO uint32_t CTRL;                              /**< watch dog control register, offset: 0x8 */
  __IO uint32_t INT_CLR;                           /**< interrupt clear register, offset: 0xC */
  __I  uint32_t INT_RAW;                           /**< raw interrupt status register, offset: 0x10 */
  __IO uint32_t MIS;                               /**< interrupt mask register, offset: 0x14 */
       uint8_t RESERVED_0[8];
  __IO uint32_t LOCK;                              /**< watch dog lock register, offset: 0x20 */
} WDT_Type;

/* ----------------------------------------------------------------------------
   -- WDT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup WDT_Register_Masks WDT Register Masks
 * @{
 */

/*! @name LOAD - watch dog counter start value register */
#define WDT_LOAD_LOAD_MASK                       (0xFFFFFFFFU)
#define WDT_LOAD_LOAD_SHIFT                      (0U)
#define WDT_LOAD_LOAD(x)                         (((uint32_t)(((uint32_t)(x)) << WDT_LOAD_LOAD_SHIFT)) & WDT_LOAD_LOAD_MASK)

/*! @name VALUE - watch dog counter value register */
#define WDT_VALUE_VALUE_MASK                     (0xFFFFFFFFU)
#define WDT_VALUE_VALUE_SHIFT                    (0U)
#define WDT_VALUE_VALUE(x)                       (((uint32_t)(((uint32_t)(x)) << WDT_VALUE_VALUE_SHIFT)) & WDT_VALUE_VALUE_MASK)

/*! @name CTRL - watch dog control register */
#define WDT_CTRL_INTEN_MASK                      (0x1U)
#define WDT_CTRL_INTEN_SHIFT                     (0U)
#define WDT_CTRL_INTEN(x)                        (((uint32_t)(((uint32_t)(x)) << WDT_CTRL_INTEN_SHIFT)) & WDT_CTRL_INTEN_MASK)
#define WDT_CTRL_RESEN_MASK                      (0x2U)
#define WDT_CTRL_RESEN_SHIFT                     (1U)
#define WDT_CTRL_RESEN(x)                        (((uint32_t)(((uint32_t)(x)) << WDT_CTRL_RESEN_SHIFT)) & WDT_CTRL_RESEN_MASK)

/*! @name INT_CLR - interrupt clear register */
#define WDT_INT_CLR_INTCLR_MASK                  (0x1U)
#define WDT_INT_CLR_INTCLR_SHIFT                 (0U)
#define WDT_INT_CLR_INTCLR(x)                    (((uint32_t)(((uint32_t)(x)) << WDT_INT_CLR_INTCLR_SHIFT)) & WDT_INT_CLR_INTCLR_MASK)

/*! @name INT_RAW - raw interrupt status register */
#define WDT_INT_RAW_RAWINTSTAT_MASK              (0x1U)
#define WDT_INT_RAW_RAWINTSTAT_SHIFT             (0U)
#define WDT_INT_RAW_RAWINTSTAT(x)                (((uint32_t)(((uint32_t)(x)) << WDT_INT_RAW_RAWINTSTAT_SHIFT)) & WDT_INT_RAW_RAWINTSTAT_MASK)

/*! @name MIS - interrupt mask register */
#define WDT_MIS_MASKINTSTAT_MASK                 (0x1U)
#define WDT_MIS_MASKINTSTAT_SHIFT                (0U)
#define WDT_MIS_MASKINTSTAT(x)                   (((uint32_t)(((uint32_t)(x)) << WDT_MIS_MASKINTSTAT_SHIFT)) & WDT_MIS_MASKINTSTAT_MASK)

/*! @name LOCK - watch dog lock register */
#define WDT_LOCK_LOCK_31_0_MASK                  (0xFFFFFFFFU)
#define WDT_LOCK_LOCK_31_0_SHIFT                 (0U)
#define WDT_LOCK_LOCK_31_0(x)                    (((uint32_t)(((uint32_t)(x)) << WDT_LOCK_LOCK_31_0_SHIFT)) & WDT_LOCK_LOCK_31_0_MASK)


/*!
 * @}
 */ /* end of group WDT_Register_Masks */


/* WDT - Peripheral instance base addresses */
/** Peripheral WDT base address */
#define WDT_BASE                                 (0x40001000u)
/** Peripheral WDT base pointer */
#define WDT                                      ((WDT_Type *)WDT_BASE)
/** Array initializer of WDT peripheral base addresses */
#define WDT_BASE_ADDRS                           { WDT_BASE }
/** Array initializer of WDT peripheral base pointers */
#define WDT_BASE_PTRS                            { WDT }
/** Interrupt vectors for the WDT peripheral type */
#define WDT_IRQS                                 { WDT_IRQn }

/*!
 * @}
 */ /* end of group WDT_Peripheral_Access_Layer */


/*
** End of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
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

#ifdef __cplusplus
}
#endif

#endif  /* _QN908XC_H_ */

