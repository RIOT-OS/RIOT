/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_SMU register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @defgroup EFM32GG11B_SMU SMU
 * @{
 * @brief EFM32GG11B_SMU Register Declaration
 ******************************************************************************/
/** SMU Register Declaration */
typedef struct {
  uint32_t       RESERVED0[3U];  /**< Reserved for future use **/
  __IM uint32_t  IF;             /**< Interrupt Flag Register  */
  __IOM uint32_t IFS;            /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC;            /**< Interrupt Flag Clear Register  */
  __IOM uint32_t IEN;            /**< Interrupt Enable Register  */

  uint32_t       RESERVED1[9U];  /**< Reserved for future use **/
  __IOM uint32_t PPUCTRL;        /**< PPU Control Register  */
  uint32_t       RESERVED2[3U];  /**< Reserved for future use **/
  __IOM uint32_t PPUPATD0;       /**< PPU Privilege Access Type Descriptor 0  */
  __IOM uint32_t PPUPATD1;       /**< PPU Privilege Access Type Descriptor 1  */
  __IOM uint32_t PPUPATD2;       /**< PPU Privilege Access Type Descriptor 2  */

  uint32_t       RESERVED3[13U]; /**< Reserved for future use **/
  __IM uint32_t  PPUFS;          /**< PPU Fault Status  */
} SMU_TypeDef;                   /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_SMU
 * @{
 * @defgroup EFM32GG11B_SMU_BitFields  SMU Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for SMU IF */
#define _SMU_IF_RESETVALUE                 0x00000000UL                   /**< Default value for SMU_IF */
#define _SMU_IF_MASK                       0x00000001UL                   /**< Mask for SMU_IF */
#define SMU_IF_PPUPRIV                     (0x1UL << 0)                   /**< PPU Privilege Interrupt Flag */
#define _SMU_IF_PPUPRIV_SHIFT              0                              /**< Shift value for SMU_PPUPRIV */
#define _SMU_IF_PPUPRIV_MASK               0x1UL                          /**< Bit mask for SMU_PPUPRIV */
#define _SMU_IF_PPUPRIV_DEFAULT            0x00000000UL                   /**< Mode DEFAULT for SMU_IF */
#define SMU_IF_PPUPRIV_DEFAULT             (_SMU_IF_PPUPRIV_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_IF */

/* Bit fields for SMU IFS */
#define _SMU_IFS_RESETVALUE                0x00000000UL                    /**< Default value for SMU_IFS */
#define _SMU_IFS_MASK                      0x00000001UL                    /**< Mask for SMU_IFS */
#define SMU_IFS_PPUPRIV                    (0x1UL << 0)                    /**< Set PPUPRIV Interrupt Flag */
#define _SMU_IFS_PPUPRIV_SHIFT             0                               /**< Shift value for SMU_PPUPRIV */
#define _SMU_IFS_PPUPRIV_MASK              0x1UL                           /**< Bit mask for SMU_PPUPRIV */
#define _SMU_IFS_PPUPRIV_DEFAULT           0x00000000UL                    /**< Mode DEFAULT for SMU_IFS */
#define SMU_IFS_PPUPRIV_DEFAULT            (_SMU_IFS_PPUPRIV_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_IFS */

/* Bit fields for SMU IFC */
#define _SMU_IFC_RESETVALUE                0x00000000UL                    /**< Default value for SMU_IFC */
#define _SMU_IFC_MASK                      0x00000001UL                    /**< Mask for SMU_IFC */
#define SMU_IFC_PPUPRIV                    (0x1UL << 0)                    /**< Clear PPUPRIV Interrupt Flag */
#define _SMU_IFC_PPUPRIV_SHIFT             0                               /**< Shift value for SMU_PPUPRIV */
#define _SMU_IFC_PPUPRIV_MASK              0x1UL                           /**< Bit mask for SMU_PPUPRIV */
#define _SMU_IFC_PPUPRIV_DEFAULT           0x00000000UL                    /**< Mode DEFAULT for SMU_IFC */
#define SMU_IFC_PPUPRIV_DEFAULT            (_SMU_IFC_PPUPRIV_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_IFC */

/* Bit fields for SMU IEN */
#define _SMU_IEN_RESETVALUE                0x00000000UL                    /**< Default value for SMU_IEN */
#define _SMU_IEN_MASK                      0x00000001UL                    /**< Mask for SMU_IEN */
#define SMU_IEN_PPUPRIV                    (0x1UL << 0)                    /**< PPUPRIV Interrupt Enable */
#define _SMU_IEN_PPUPRIV_SHIFT             0                               /**< Shift value for SMU_PPUPRIV */
#define _SMU_IEN_PPUPRIV_MASK              0x1UL                           /**< Bit mask for SMU_PPUPRIV */
#define _SMU_IEN_PPUPRIV_DEFAULT           0x00000000UL                    /**< Mode DEFAULT for SMU_IEN */
#define SMU_IEN_PPUPRIV_DEFAULT            (_SMU_IEN_PPUPRIV_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_IEN */

/* Bit fields for SMU PPUCTRL */
#define _SMU_PPUCTRL_RESETVALUE            0x00000000UL                       /**< Default value for SMU_PPUCTRL */
#define _SMU_PPUCTRL_MASK                  0x00000001UL                       /**< Mask for SMU_PPUCTRL */
#define SMU_PPUCTRL_ENABLE                 (0x1UL << 0)                       /**<  */
#define _SMU_PPUCTRL_ENABLE_SHIFT          0                                  /**< Shift value for SMU_ENABLE */
#define _SMU_PPUCTRL_ENABLE_MASK           0x1UL                              /**< Bit mask for SMU_ENABLE */
#define _SMU_PPUCTRL_ENABLE_DEFAULT        0x00000000UL                       /**< Mode DEFAULT for SMU_PPUCTRL */
#define SMU_PPUCTRL_ENABLE_DEFAULT         (_SMU_PPUCTRL_ENABLE_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_PPUCTRL */

/* Bit fields for SMU PPUPATD0 */
#define _SMU_PPUPATD0_RESETVALUE           0x00000000UL                           /**< Default value for SMU_PPUPATD0 */
#define _SMU_PPUPATD0_MASK                 0xFFFFFFFFUL                           /**< Mask for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP0                 (0x1UL << 0)                           /**< Analog Comparator 0 access control bit */
#define _SMU_PPUPATD0_ACMP0_SHIFT          0                                      /**< Shift value for SMU_ACMP0 */
#define _SMU_PPUPATD0_ACMP0_MASK           0x1UL                                  /**< Bit mask for SMU_ACMP0 */
#define _SMU_PPUPATD0_ACMP0_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP0_DEFAULT         (_SMU_PPUPATD0_ACMP0_DEFAULT << 0)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP1                 (0x1UL << 1)                           /**< Analog Comparator 1 access control bit */
#define _SMU_PPUPATD0_ACMP1_SHIFT          1                                      /**< Shift value for SMU_ACMP1 */
#define _SMU_PPUPATD0_ACMP1_MASK           0x2UL                                  /**< Bit mask for SMU_ACMP1 */
#define _SMU_PPUPATD0_ACMP1_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP1_DEFAULT         (_SMU_PPUPATD0_ACMP1_DEFAULT << 1)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP2                 (0x1UL << 2)                           /**< Analog Comparator 1 access control bit */
#define _SMU_PPUPATD0_ACMP2_SHIFT          2                                      /**< Shift value for SMU_ACMP2 */
#define _SMU_PPUPATD0_ACMP2_MASK           0x4UL                                  /**< Bit mask for SMU_ACMP2 */
#define _SMU_PPUPATD0_ACMP2_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP2_DEFAULT         (_SMU_PPUPATD0_ACMP2_DEFAULT << 2)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP3                 (0x1UL << 3)                           /**< Analog Comparator 3 access control bit */
#define _SMU_PPUPATD0_ACMP3_SHIFT          3                                      /**< Shift value for SMU_ACMP3 */
#define _SMU_PPUPATD0_ACMP3_MASK           0x8UL                                  /**< Bit mask for SMU_ACMP3 */
#define _SMU_PPUPATD0_ACMP3_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ACMP3_DEFAULT         (_SMU_PPUPATD0_ACMP3_DEFAULT << 3)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ADC0                  (0x1UL << 4)                           /**< Analog to Digital Converter 0 access control bit */
#define _SMU_PPUPATD0_ADC0_SHIFT           4                                      /**< Shift value for SMU_ADC0 */
#define _SMU_PPUPATD0_ADC0_MASK            0x10UL                                 /**< Bit mask for SMU_ADC0 */
#define _SMU_PPUPATD0_ADC0_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ADC0_DEFAULT          (_SMU_PPUPATD0_ADC0_DEFAULT << 4)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ADC1                  (0x1UL << 5)                           /**< Analog to Digital Converter 0 access control bit */
#define _SMU_PPUPATD0_ADC1_SHIFT           5                                      /**< Shift value for SMU_ADC1 */
#define _SMU_PPUPATD0_ADC1_MASK            0x20UL                                 /**< Bit mask for SMU_ADC1 */
#define _SMU_PPUPATD0_ADC1_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ADC1_DEFAULT          (_SMU_PPUPATD0_ADC1_DEFAULT << 5)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CAN0                  (0x1UL << 6)                           /**< CAN 0 access control bit */
#define _SMU_PPUPATD0_CAN0_SHIFT           6                                      /**< Shift value for SMU_CAN0 */
#define _SMU_PPUPATD0_CAN0_MASK            0x40UL                                 /**< Bit mask for SMU_CAN0 */
#define _SMU_PPUPATD0_CAN0_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CAN0_DEFAULT          (_SMU_PPUPATD0_CAN0_DEFAULT << 6)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CAN1                  (0x1UL << 7)                           /**< CAN 1 access control bit */
#define _SMU_PPUPATD0_CAN1_SHIFT           7                                      /**< Shift value for SMU_CAN1 */
#define _SMU_PPUPATD0_CAN1_MASK            0x80UL                                 /**< Bit mask for SMU_CAN1 */
#define _SMU_PPUPATD0_CAN1_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CAN1_DEFAULT          (_SMU_PPUPATD0_CAN1_DEFAULT << 7)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CMU                   (0x1UL << 8)                           /**< Clock Management Unit access control bit */
#define _SMU_PPUPATD0_CMU_SHIFT            8                                      /**< Shift value for SMU_CMU */
#define _SMU_PPUPATD0_CMU_MASK             0x100UL                                /**< Bit mask for SMU_CMU */
#define _SMU_PPUPATD0_CMU_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CMU_DEFAULT           (_SMU_PPUPATD0_CMU_DEFAULT << 8)       /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CRYOTIMER             (0x1UL << 9)                           /**< CRYOTIMER access control bit */
#define _SMU_PPUPATD0_CRYOTIMER_SHIFT      9                                      /**< Shift value for SMU_CRYOTIMER */
#define _SMU_PPUPATD0_CRYOTIMER_MASK       0x200UL                                /**< Bit mask for SMU_CRYOTIMER */
#define _SMU_PPUPATD0_CRYOTIMER_DEFAULT    0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CRYOTIMER_DEFAULT     (_SMU_PPUPATD0_CRYOTIMER_DEFAULT << 9) /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CRYPTO0               (0x1UL << 10)                          /**< Advanced Encryption Standard Accelerator access control bit */
#define _SMU_PPUPATD0_CRYPTO0_SHIFT        10                                     /**< Shift value for SMU_CRYPTO0 */
#define _SMU_PPUPATD0_CRYPTO0_MASK         0x400UL                                /**< Bit mask for SMU_CRYPTO0 */
#define _SMU_PPUPATD0_CRYPTO0_DEFAULT      0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CRYPTO0_DEFAULT       (_SMU_PPUPATD0_CRYPTO0_DEFAULT << 10)  /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CSEN                  (0x1UL << 11)                          /**< Capacitive touch sense module access control bit */
#define _SMU_PPUPATD0_CSEN_SHIFT           11                                     /**< Shift value for SMU_CSEN */
#define _SMU_PPUPATD0_CSEN_MASK            0x800UL                                /**< Bit mask for SMU_CSEN */
#define _SMU_PPUPATD0_CSEN_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_CSEN_DEFAULT          (_SMU_PPUPATD0_CSEN_DEFAULT << 11)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_VDAC0                 (0x1UL << 12)                          /**< Digital to Analog Converter 0 access control bit */
#define _SMU_PPUPATD0_VDAC0_SHIFT          12                                     /**< Shift value for SMU_VDAC0 */
#define _SMU_PPUPATD0_VDAC0_MASK           0x1000UL                               /**< Bit mask for SMU_VDAC0 */
#define _SMU_PPUPATD0_VDAC0_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_VDAC0_DEFAULT         (_SMU_PPUPATD0_VDAC0_DEFAULT << 12)    /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_PRS                   (0x1UL << 13)                          /**< Peripheral Reflex System access control bit */
#define _SMU_PPUPATD0_PRS_SHIFT            13                                     /**< Shift value for SMU_PRS */
#define _SMU_PPUPATD0_PRS_MASK             0x2000UL                               /**< Bit mask for SMU_PRS */
#define _SMU_PPUPATD0_PRS_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_PRS_DEFAULT           (_SMU_PPUPATD0_PRS_DEFAULT << 13)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_EBI                   (0x1UL << 14)                          /**< External Bus Interface access control bit */
#define _SMU_PPUPATD0_EBI_SHIFT            14                                     /**< Shift value for SMU_EBI */
#define _SMU_PPUPATD0_EBI_MASK             0x4000UL                               /**< Bit mask for SMU_EBI */
#define _SMU_PPUPATD0_EBI_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_EBI_DEFAULT           (_SMU_PPUPATD0_EBI_DEFAULT << 14)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_EMU                   (0x1UL << 15)                          /**< Energy Management Unit access control bit */
#define _SMU_PPUPATD0_EMU_SHIFT            15                                     /**< Shift value for SMU_EMU */
#define _SMU_PPUPATD0_EMU_MASK             0x8000UL                               /**< Bit mask for SMU_EMU */
#define _SMU_PPUPATD0_EMU_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_EMU_DEFAULT           (_SMU_PPUPATD0_EMU_DEFAULT << 15)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ETH                   (0x1UL << 16)                          /**< Ethernet Controller access control bit */
#define _SMU_PPUPATD0_ETH_SHIFT            16                                     /**< Shift value for SMU_ETH */
#define _SMU_PPUPATD0_ETH_MASK             0x10000UL                              /**< Bit mask for SMU_ETH */
#define _SMU_PPUPATD0_ETH_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_ETH_DEFAULT           (_SMU_PPUPATD0_ETH_DEFAULT << 16)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_FPUEH                 (0x1UL << 17)                          /**< FPU Exception Handler access control bit */
#define _SMU_PPUPATD0_FPUEH_SHIFT          17                                     /**< Shift value for SMU_FPUEH */
#define _SMU_PPUPATD0_FPUEH_MASK           0x20000UL                              /**< Bit mask for SMU_FPUEH */
#define _SMU_PPUPATD0_FPUEH_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_FPUEH_DEFAULT         (_SMU_PPUPATD0_FPUEH_DEFAULT << 17)    /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_GPCRC                 (0x1UL << 18)                          /**< General Purpose CRC access control bit */
#define _SMU_PPUPATD0_GPCRC_SHIFT          18                                     /**< Shift value for SMU_GPCRC */
#define _SMU_PPUPATD0_GPCRC_MASK           0x40000UL                              /**< Bit mask for SMU_GPCRC */
#define _SMU_PPUPATD0_GPCRC_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_GPCRC_DEFAULT         (_SMU_PPUPATD0_GPCRC_DEFAULT << 18)    /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_GPIO                  (0x1UL << 19)                          /**< General purpose Input/Output access control bit */
#define _SMU_PPUPATD0_GPIO_SHIFT           19                                     /**< Shift value for SMU_GPIO */
#define _SMU_PPUPATD0_GPIO_MASK            0x80000UL                              /**< Bit mask for SMU_GPIO */
#define _SMU_PPUPATD0_GPIO_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_GPIO_DEFAULT          (_SMU_PPUPATD0_GPIO_DEFAULT << 19)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_I2C0                  (0x1UL << 20)                          /**< I2C 0 access control bit */
#define _SMU_PPUPATD0_I2C0_SHIFT           20                                     /**< Shift value for SMU_I2C0 */
#define _SMU_PPUPATD0_I2C0_MASK            0x100000UL                             /**< Bit mask for SMU_I2C0 */
#define _SMU_PPUPATD0_I2C0_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_I2C0_DEFAULT          (_SMU_PPUPATD0_I2C0_DEFAULT << 20)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_I2C1                  (0x1UL << 21)                          /**< I2C 1 access control bit */
#define _SMU_PPUPATD0_I2C1_SHIFT           21                                     /**< Shift value for SMU_I2C1 */
#define _SMU_PPUPATD0_I2C1_MASK            0x200000UL                             /**< Bit mask for SMU_I2C1 */
#define _SMU_PPUPATD0_I2C1_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_I2C1_DEFAULT          (_SMU_PPUPATD0_I2C1_DEFAULT << 21)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_I2C2                  (0x1UL << 22)                          /**< I2C 2 access control bit */
#define _SMU_PPUPATD0_I2C2_SHIFT           22                                     /**< Shift value for SMU_I2C2 */
#define _SMU_PPUPATD0_I2C2_MASK            0x400000UL                             /**< Bit mask for SMU_I2C2 */
#define _SMU_PPUPATD0_I2C2_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_I2C2_DEFAULT          (_SMU_PPUPATD0_I2C2_DEFAULT << 22)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_IDAC0                 (0x1UL << 23)                          /**< Current Digital to Analog Converter 0 access control bit */
#define _SMU_PPUPATD0_IDAC0_SHIFT          23                                     /**< Shift value for SMU_IDAC0 */
#define _SMU_PPUPATD0_IDAC0_MASK           0x800000UL                             /**< Bit mask for SMU_IDAC0 */
#define _SMU_PPUPATD0_IDAC0_DEFAULT        0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_IDAC0_DEFAULT         (_SMU_PPUPATD0_IDAC0_DEFAULT << 23)    /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_MSC                   (0x1UL << 24)                          /**< Memory System Controller access control bit */
#define _SMU_PPUPATD0_MSC_SHIFT            24                                     /**< Shift value for SMU_MSC */
#define _SMU_PPUPATD0_MSC_MASK             0x1000000UL                            /**< Bit mask for SMU_MSC */
#define _SMU_PPUPATD0_MSC_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_MSC_DEFAULT           (_SMU_PPUPATD0_MSC_DEFAULT << 24)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LCD                   (0x1UL << 25)                          /**< Liquid Crystal Display Controller access control bit */
#define _SMU_PPUPATD0_LCD_SHIFT            25                                     /**< Shift value for SMU_LCD */
#define _SMU_PPUPATD0_LCD_MASK             0x2000000UL                            /**< Bit mask for SMU_LCD */
#define _SMU_PPUPATD0_LCD_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LCD_DEFAULT           (_SMU_PPUPATD0_LCD_DEFAULT << 25)      /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LDMA                  (0x1UL << 26)                          /**< Linked Direct Memory Access Controller access control bit */
#define _SMU_PPUPATD0_LDMA_SHIFT           26                                     /**< Shift value for SMU_LDMA */
#define _SMU_PPUPATD0_LDMA_MASK            0x4000000UL                            /**< Bit mask for SMU_LDMA */
#define _SMU_PPUPATD0_LDMA_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LDMA_DEFAULT          (_SMU_PPUPATD0_LDMA_DEFAULT << 26)     /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LESENSE               (0x1UL << 27)                          /**< Low Energy Sensor Interface access control bit */
#define _SMU_PPUPATD0_LESENSE_SHIFT        27                                     /**< Shift value for SMU_LESENSE */
#define _SMU_PPUPATD0_LESENSE_MASK         0x8000000UL                            /**< Bit mask for SMU_LESENSE */
#define _SMU_PPUPATD0_LESENSE_DEFAULT      0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LESENSE_DEFAULT       (_SMU_PPUPATD0_LESENSE_DEFAULT << 27)  /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LETIMER0              (0x1UL << 28)                          /**< Low Energy Timer 0 access control bit */
#define _SMU_PPUPATD0_LETIMER0_SHIFT       28                                     /**< Shift value for SMU_LETIMER0 */
#define _SMU_PPUPATD0_LETIMER0_MASK        0x10000000UL                           /**< Bit mask for SMU_LETIMER0 */
#define _SMU_PPUPATD0_LETIMER0_DEFAULT     0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LETIMER0_DEFAULT      (_SMU_PPUPATD0_LETIMER0_DEFAULT << 28) /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LETIMER1              (0x1UL << 29)                          /**< Low Energy Timer 1 access control bit */
#define _SMU_PPUPATD0_LETIMER1_SHIFT       29                                     /**< Shift value for SMU_LETIMER1 */
#define _SMU_PPUPATD0_LETIMER1_MASK        0x20000000UL                           /**< Bit mask for SMU_LETIMER1 */
#define _SMU_PPUPATD0_LETIMER1_DEFAULT     0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LETIMER1_DEFAULT      (_SMU_PPUPATD0_LETIMER1_DEFAULT << 29) /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LEUART0               (0x1UL << 30)                          /**< Low Energy UART 0 access control bit */
#define _SMU_PPUPATD0_LEUART0_SHIFT        30                                     /**< Shift value for SMU_LEUART0 */
#define _SMU_PPUPATD0_LEUART0_MASK         0x40000000UL                           /**< Bit mask for SMU_LEUART0 */
#define _SMU_PPUPATD0_LEUART0_DEFAULT      0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LEUART0_DEFAULT       (_SMU_PPUPATD0_LEUART0_DEFAULT << 30)  /**< Shifted mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LEUART1               (0x1UL << 31)                          /**< Low Energy UART 1 access control bit */
#define _SMU_PPUPATD0_LEUART1_SHIFT        31                                     /**< Shift value for SMU_LEUART1 */
#define _SMU_PPUPATD0_LEUART1_MASK         0x80000000UL                           /**< Bit mask for SMU_LEUART1 */
#define _SMU_PPUPATD0_LEUART1_DEFAULT      0x00000000UL                           /**< Mode DEFAULT for SMU_PPUPATD0 */
#define SMU_PPUPATD0_LEUART1_DEFAULT       (_SMU_PPUPATD0_LEUART1_DEFAULT << 31)  /**< Shifted mode DEFAULT for SMU_PPUPATD0 */

/* Bit fields for SMU PPUPATD1 */
#define _SMU_PPUPATD1_RESETVALUE           0x00000000UL                          /**< Default value for SMU_PPUPATD1 */
#define _SMU_PPUPATD1_MASK                 0xFFFFFFFFUL                          /**< Mask for SMU_PPUPATD1 */
#define SMU_PPUPATD1_PCNT0                 (0x1UL << 0)                          /**< Pulse Counter 0 access control bit */
#define _SMU_PPUPATD1_PCNT0_SHIFT          0                                     /**< Shift value for SMU_PCNT0 */
#define _SMU_PPUPATD1_PCNT0_MASK           0x1UL                                 /**< Bit mask for SMU_PCNT0 */
#define _SMU_PPUPATD1_PCNT0_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_PCNT0_DEFAULT         (_SMU_PPUPATD1_PCNT0_DEFAULT << 0)    /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_PCNT1                 (0x1UL << 1)                          /**< Pulse Counter 1 access control bit */
#define _SMU_PPUPATD1_PCNT1_SHIFT          1                                     /**< Shift value for SMU_PCNT1 */
#define _SMU_PPUPATD1_PCNT1_MASK           0x2UL                                 /**< Bit mask for SMU_PCNT1 */
#define _SMU_PPUPATD1_PCNT1_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_PCNT1_DEFAULT         (_SMU_PPUPATD1_PCNT1_DEFAULT << 1)    /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_PCNT2                 (0x1UL << 2)                          /**< Pulse Counter 2 access control bit */
#define _SMU_PPUPATD1_PCNT2_SHIFT          2                                     /**< Shift value for SMU_PCNT2 */
#define _SMU_PPUPATD1_PCNT2_MASK           0x4UL                                 /**< Bit mask for SMU_PCNT2 */
#define _SMU_PPUPATD1_PCNT2_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_PCNT2_DEFAULT         (_SMU_PPUPATD1_PCNT2_DEFAULT << 2)    /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_QSPI0                 (0x1UL << 3)                          /**< Quad-SPI access control bit */
#define _SMU_PPUPATD1_QSPI0_SHIFT          3                                     /**< Shift value for SMU_QSPI0 */
#define _SMU_PPUPATD1_QSPI0_MASK           0x8UL                                 /**< Bit mask for SMU_QSPI0 */
#define _SMU_PPUPATD1_QSPI0_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_QSPI0_DEFAULT         (_SMU_PPUPATD1_QSPI0_DEFAULT << 3)    /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_RMU                   (0x1UL << 4)                          /**< Reset Management Unit access control bit */
#define _SMU_PPUPATD1_RMU_SHIFT            4                                     /**< Shift value for SMU_RMU */
#define _SMU_PPUPATD1_RMU_MASK             0x10UL                                /**< Bit mask for SMU_RMU */
#define _SMU_PPUPATD1_RMU_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_RMU_DEFAULT           (_SMU_PPUPATD1_RMU_DEFAULT << 4)      /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_RTC                   (0x1UL << 5)                          /**< Real-Time Counter access control bit */
#define _SMU_PPUPATD1_RTC_SHIFT            5                                     /**< Shift value for SMU_RTC */
#define _SMU_PPUPATD1_RTC_MASK             0x20UL                                /**< Bit mask for SMU_RTC */
#define _SMU_PPUPATD1_RTC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_RTC_DEFAULT           (_SMU_PPUPATD1_RTC_DEFAULT << 5)      /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_RTCC                  (0x1UL << 6)                          /**< Real-Time Counter and Calendar access control bit */
#define _SMU_PPUPATD1_RTCC_SHIFT           6                                     /**< Shift value for SMU_RTCC */
#define _SMU_PPUPATD1_RTCC_MASK            0x40UL                                /**< Bit mask for SMU_RTCC */
#define _SMU_PPUPATD1_RTCC_DEFAULT         0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_RTCC_DEFAULT          (_SMU_PPUPATD1_RTCC_DEFAULT << 6)     /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_SDIO                  (0x1UL << 7)                          /**< SDIO Controller access control bit */
#define _SMU_PPUPATD1_SDIO_SHIFT           7                                     /**< Shift value for SMU_SDIO */
#define _SMU_PPUPATD1_SDIO_MASK            0x80UL                                /**< Bit mask for SMU_SDIO */
#define _SMU_PPUPATD1_SDIO_DEFAULT         0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_SDIO_DEFAULT          (_SMU_PPUPATD1_SDIO_DEFAULT << 7)     /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_SMU                   (0x1UL << 8)                          /**< Security Management Unit access control bit */
#define _SMU_PPUPATD1_SMU_SHIFT            8                                     /**< Shift value for SMU_SMU */
#define _SMU_PPUPATD1_SMU_MASK             0x100UL                               /**< Bit mask for SMU_SMU */
#define _SMU_PPUPATD1_SMU_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_SMU_DEFAULT           (_SMU_PPUPATD1_SMU_DEFAULT << 8)      /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER0                (0x1UL << 9)                          /**< Timer 0 access control bit */
#define _SMU_PPUPATD1_TIMER0_SHIFT         9                                     /**< Shift value for SMU_TIMER0 */
#define _SMU_PPUPATD1_TIMER0_MASK          0x200UL                               /**< Bit mask for SMU_TIMER0 */
#define _SMU_PPUPATD1_TIMER0_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER0_DEFAULT        (_SMU_PPUPATD1_TIMER0_DEFAULT << 9)   /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER1                (0x1UL << 10)                         /**< Timer 1 access control bit */
#define _SMU_PPUPATD1_TIMER1_SHIFT         10                                    /**< Shift value for SMU_TIMER1 */
#define _SMU_PPUPATD1_TIMER1_MASK          0x400UL                               /**< Bit mask for SMU_TIMER1 */
#define _SMU_PPUPATD1_TIMER1_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER1_DEFAULT        (_SMU_PPUPATD1_TIMER1_DEFAULT << 10)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER2                (0x1UL << 11)                         /**< Timer 2 access control bit */
#define _SMU_PPUPATD1_TIMER2_SHIFT         11                                    /**< Shift value for SMU_TIMER2 */
#define _SMU_PPUPATD1_TIMER2_MASK          0x800UL                               /**< Bit mask for SMU_TIMER2 */
#define _SMU_PPUPATD1_TIMER2_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER2_DEFAULT        (_SMU_PPUPATD1_TIMER2_DEFAULT << 11)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER3                (0x1UL << 12)                         /**< Timer 3 access control bit */
#define _SMU_PPUPATD1_TIMER3_SHIFT         12                                    /**< Shift value for SMU_TIMER3 */
#define _SMU_PPUPATD1_TIMER3_MASK          0x1000UL                              /**< Bit mask for SMU_TIMER3 */
#define _SMU_PPUPATD1_TIMER3_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER3_DEFAULT        (_SMU_PPUPATD1_TIMER3_DEFAULT << 12)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER4                (0x1UL << 13)                         /**< Timer 4 access control bit */
#define _SMU_PPUPATD1_TIMER4_SHIFT         13                                    /**< Shift value for SMU_TIMER4 */
#define _SMU_PPUPATD1_TIMER4_MASK          0x2000UL                              /**< Bit mask for SMU_TIMER4 */
#define _SMU_PPUPATD1_TIMER4_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER4_DEFAULT        (_SMU_PPUPATD1_TIMER4_DEFAULT << 13)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER5                (0x1UL << 14)                         /**< Timer 5 access control bit */
#define _SMU_PPUPATD1_TIMER5_SHIFT         14                                    /**< Shift value for SMU_TIMER5 */
#define _SMU_PPUPATD1_TIMER5_MASK          0x4000UL                              /**< Bit mask for SMU_TIMER5 */
#define _SMU_PPUPATD1_TIMER5_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER5_DEFAULT        (_SMU_PPUPATD1_TIMER5_DEFAULT << 14)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER6                (0x1UL << 15)                         /**< Timer 6 access control bit */
#define _SMU_PPUPATD1_TIMER6_SHIFT         15                                    /**< Shift value for SMU_TIMER6 */
#define _SMU_PPUPATD1_TIMER6_MASK          0x8000UL                              /**< Bit mask for SMU_TIMER6 */
#define _SMU_PPUPATD1_TIMER6_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TIMER6_DEFAULT        (_SMU_PPUPATD1_TIMER6_DEFAULT << 15)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TRNG0                 (0x1UL << 16)                         /**< True Random Number Generator 0 access control bit */
#define _SMU_PPUPATD1_TRNG0_SHIFT          16                                    /**< Shift value for SMU_TRNG0 */
#define _SMU_PPUPATD1_TRNG0_MASK           0x10000UL                             /**< Bit mask for SMU_TRNG0 */
#define _SMU_PPUPATD1_TRNG0_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_TRNG0_DEFAULT         (_SMU_PPUPATD1_TRNG0_DEFAULT << 16)   /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_UART0                 (0x1UL << 17)                         /**< Universal Asynchronous Receiver/Transmitter 0 access control bit */
#define _SMU_PPUPATD1_UART0_SHIFT          17                                    /**< Shift value for SMU_UART0 */
#define _SMU_PPUPATD1_UART0_MASK           0x20000UL                             /**< Bit mask for SMU_UART0 */
#define _SMU_PPUPATD1_UART0_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_UART0_DEFAULT         (_SMU_PPUPATD1_UART0_DEFAULT << 17)   /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_UART1                 (0x1UL << 18)                         /**< Universal Asynchronous Receiver/Transmitter 1 access control bit */
#define _SMU_PPUPATD1_UART1_SHIFT          18                                    /**< Shift value for SMU_UART1 */
#define _SMU_PPUPATD1_UART1_MASK           0x40000UL                             /**< Bit mask for SMU_UART1 */
#define _SMU_PPUPATD1_UART1_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_UART1_DEFAULT         (_SMU_PPUPATD1_UART1_DEFAULT << 18)   /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART0                (0x1UL << 19)                         /**< Universal Synchronous/Asynchronous Receiver/Transmitter 0 access control bit */
#define _SMU_PPUPATD1_USART0_SHIFT         19                                    /**< Shift value for SMU_USART0 */
#define _SMU_PPUPATD1_USART0_MASK          0x80000UL                             /**< Bit mask for SMU_USART0 */
#define _SMU_PPUPATD1_USART0_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART0_DEFAULT        (_SMU_PPUPATD1_USART0_DEFAULT << 19)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART1                (0x1UL << 20)                         /**< Universal Synchronous/Asynchronous Receiver/Transmitter 1 access control bit */
#define _SMU_PPUPATD1_USART1_SHIFT         20                                    /**< Shift value for SMU_USART1 */
#define _SMU_PPUPATD1_USART1_MASK          0x100000UL                            /**< Bit mask for SMU_USART1 */
#define _SMU_PPUPATD1_USART1_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART1_DEFAULT        (_SMU_PPUPATD1_USART1_DEFAULT << 20)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART2                (0x1UL << 21)                         /**< Universal Synchronous/Asynchronous Receiver/Transmitter 2 access control bit */
#define _SMU_PPUPATD1_USART2_SHIFT         21                                    /**< Shift value for SMU_USART2 */
#define _SMU_PPUPATD1_USART2_MASK          0x200000UL                            /**< Bit mask for SMU_USART2 */
#define _SMU_PPUPATD1_USART2_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART2_DEFAULT        (_SMU_PPUPATD1_USART2_DEFAULT << 21)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART3                (0x1UL << 22)                         /**< Universal Synchronous/Asynchronous Receiver/Transmitter 3 access control bit */
#define _SMU_PPUPATD1_USART3_SHIFT         22                                    /**< Shift value for SMU_USART3 */
#define _SMU_PPUPATD1_USART3_MASK          0x400000UL                            /**< Bit mask for SMU_USART3 */
#define _SMU_PPUPATD1_USART3_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART3_DEFAULT        (_SMU_PPUPATD1_USART3_DEFAULT << 22)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART4                (0x1UL << 23)                         /**< Universal Synchronous/Asynchronous Receiver/Transmitter 4 access control bit */
#define _SMU_PPUPATD1_USART4_SHIFT         23                                    /**< Shift value for SMU_USART4 */
#define _SMU_PPUPATD1_USART4_MASK          0x800000UL                            /**< Bit mask for SMU_USART4 */
#define _SMU_PPUPATD1_USART4_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART4_DEFAULT        (_SMU_PPUPATD1_USART4_DEFAULT << 23)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART5                (0x1UL << 24)                         /**< Universal Synchronous/Asynchronous Receiver/Transmitter 5 access control bit */
#define _SMU_PPUPATD1_USART5_SHIFT         24                                    /**< Shift value for SMU_USART5 */
#define _SMU_PPUPATD1_USART5_MASK          0x1000000UL                           /**< Bit mask for SMU_USART5 */
#define _SMU_PPUPATD1_USART5_DEFAULT       0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USART5_DEFAULT        (_SMU_PPUPATD1_USART5_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USB                   (0x1UL << 25)                         /**< Universal Serial Bus Interface access control bit */
#define _SMU_PPUPATD1_USB_SHIFT            25                                    /**< Shift value for SMU_USB */
#define _SMU_PPUPATD1_USB_MASK             0x2000000UL                           /**< Bit mask for SMU_USB */
#define _SMU_PPUPATD1_USB_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_USB_DEFAULT           (_SMU_PPUPATD1_USB_DEFAULT << 25)     /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WDOG0                 (0x1UL << 26)                         /**< Watchdog access control bit */
#define _SMU_PPUPATD1_WDOG0_SHIFT          26                                    /**< Shift value for SMU_WDOG0 */
#define _SMU_PPUPATD1_WDOG0_MASK           0x4000000UL                           /**< Bit mask for SMU_WDOG0 */
#define _SMU_PPUPATD1_WDOG0_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WDOG0_DEFAULT         (_SMU_PPUPATD1_WDOG0_DEFAULT << 26)   /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WDOG1                 (0x1UL << 27)                         /**< Watchdog access control bit */
#define _SMU_PPUPATD1_WDOG1_SHIFT          27                                    /**< Shift value for SMU_WDOG1 */
#define _SMU_PPUPATD1_WDOG1_MASK           0x8000000UL                           /**< Bit mask for SMU_WDOG1 */
#define _SMU_PPUPATD1_WDOG1_DEFAULT        0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WDOG1_DEFAULT         (_SMU_PPUPATD1_WDOG1_DEFAULT << 27)   /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER0               (0x1UL << 28)                         /**< Wide Timer 0 access control bit */
#define _SMU_PPUPATD1_WTIMER0_SHIFT        28                                    /**< Shift value for SMU_WTIMER0 */
#define _SMU_PPUPATD1_WTIMER0_MASK         0x10000000UL                          /**< Bit mask for SMU_WTIMER0 */
#define _SMU_PPUPATD1_WTIMER0_DEFAULT      0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER0_DEFAULT       (_SMU_PPUPATD1_WTIMER0_DEFAULT << 28) /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER1               (0x1UL << 29)                         /**< Wide Timer 0 access control bit */
#define _SMU_PPUPATD1_WTIMER1_SHIFT        29                                    /**< Shift value for SMU_WTIMER1 */
#define _SMU_PPUPATD1_WTIMER1_MASK         0x20000000UL                          /**< Bit mask for SMU_WTIMER1 */
#define _SMU_PPUPATD1_WTIMER1_DEFAULT      0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER1_DEFAULT       (_SMU_PPUPATD1_WTIMER1_DEFAULT << 29) /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER2               (0x1UL << 30)                         /**< Wide Timer 2 access control bit */
#define _SMU_PPUPATD1_WTIMER2_SHIFT        30                                    /**< Shift value for SMU_WTIMER2 */
#define _SMU_PPUPATD1_WTIMER2_MASK         0x40000000UL                          /**< Bit mask for SMU_WTIMER2 */
#define _SMU_PPUPATD1_WTIMER2_DEFAULT      0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER2_DEFAULT       (_SMU_PPUPATD1_WTIMER2_DEFAULT << 30) /**< Shifted mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER3               (0x1UL << 31)                         /**< Wide Timer 3 access control bit */
#define _SMU_PPUPATD1_WTIMER3_SHIFT        31                                    /**< Shift value for SMU_WTIMER3 */
#define _SMU_PPUPATD1_WTIMER3_MASK         0x80000000UL                          /**< Bit mask for SMU_WTIMER3 */
#define _SMU_PPUPATD1_WTIMER3_DEFAULT      0x00000000UL                          /**< Mode DEFAULT for SMU_PPUPATD1 */
#define SMU_PPUPATD1_WTIMER3_DEFAULT       (_SMU_PPUPATD1_WTIMER3_DEFAULT << 31) /**< Shifted mode DEFAULT for SMU_PPUPATD1 */

/* Bit fields for SMU PPUPATD2 */
#define _SMU_PPUPATD2_RESETVALUE           0x00000000UL /**< Default value for SMU_PPUPATD2 */
#define _SMU_PPUPATD2_MASK                 0x00000000UL /**< Mask for SMU_PPUPATD2 */

/* Bit fields for SMU PPUFS */
#define _SMU_PPUFS_RESETVALUE              0x00000000UL                         /**< Default value for SMU_PPUFS */
#define _SMU_PPUFS_MASK                    0x0000007FUL                         /**< Mask for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_SHIFT          0                                    /**< Shift value for SMU_PERIPHID */
#define _SMU_PPUFS_PERIPHID_MASK           0x7FUL                               /**< Bit mask for SMU_PERIPHID */
#define _SMU_PPUFS_PERIPHID_DEFAULT        0x00000000UL                         /**< Mode DEFAULT for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ACMP0          0x00000000UL                         /**< Mode ACMP0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ACMP1          0x00000001UL                         /**< Mode ACMP1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ACMP2          0x00000002UL                         /**< Mode ACMP2 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ACMP3          0x00000003UL                         /**< Mode ACMP3 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ADC0           0x00000004UL                         /**< Mode ADC0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ADC1           0x00000005UL                         /**< Mode ADC1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_CAN0           0x00000006UL                         /**< Mode CAN0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_CAN1           0x00000007UL                         /**< Mode CAN1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_CMU            0x00000008UL                         /**< Mode CMU for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_CRYOTIMER      0x00000009UL                         /**< Mode CRYOTIMER for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_CRYPTO0        0x0000000AUL                         /**< Mode CRYPTO0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_CSEN           0x0000000BUL                         /**< Mode CSEN for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_VDAC0          0x0000000CUL                         /**< Mode VDAC0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_PRS            0x0000000DUL                         /**< Mode PRS for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_EBI            0x0000000EUL                         /**< Mode EBI for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_EMU            0x0000000FUL                         /**< Mode EMU for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_ETH            0x00000010UL                         /**< Mode ETH for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_FPUEH          0x00000011UL                         /**< Mode FPUEH for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_GPCRC          0x00000012UL                         /**< Mode GPCRC for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_GPIO           0x00000013UL                         /**< Mode GPIO for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_I2C0           0x00000014UL                         /**< Mode I2C0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_I2C1           0x00000015UL                         /**< Mode I2C1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_I2C2           0x00000016UL                         /**< Mode I2C2 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_IDAC0          0x00000017UL                         /**< Mode IDAC0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_MSC            0x00000018UL                         /**< Mode MSC for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LCD            0x00000019UL                         /**< Mode LCD for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LDMA           0x0000001AUL                         /**< Mode LDMA for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LESENSE        0x0000001BUL                         /**< Mode LESENSE for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LETIMER0       0x0000001CUL                         /**< Mode LETIMER0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LETIMER1       0x0000001DUL                         /**< Mode LETIMER1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LEUART0        0x0000001EUL                         /**< Mode LEUART0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_LEUART1        0x0000001FUL                         /**< Mode LEUART1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_PCNT0          0x00000020UL                         /**< Mode PCNT0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_PCNT1          0x00000021UL                         /**< Mode PCNT1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_PCNT2          0x00000022UL                         /**< Mode PCNT2 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_QSPI0          0x00000023UL                         /**< Mode QSPI0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_RMU            0x00000024UL                         /**< Mode RMU for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_RTC            0x00000025UL                         /**< Mode RTC for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_RTCC           0x00000026UL                         /**< Mode RTCC for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_SDIO           0x00000027UL                         /**< Mode SDIO for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_SMU            0x00000028UL                         /**< Mode SMU for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER0         0x00000029UL                         /**< Mode TIMER0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER1         0x0000002AUL                         /**< Mode TIMER1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER2         0x0000002BUL                         /**< Mode TIMER2 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER3         0x0000002CUL                         /**< Mode TIMER3 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER4         0x0000002DUL                         /**< Mode TIMER4 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER5         0x0000002EUL                         /**< Mode TIMER5 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TIMER6         0x0000002FUL                         /**< Mode TIMER6 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_TRNG0          0x00000030UL                         /**< Mode TRNG0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_UART0          0x00000031UL                         /**< Mode UART0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_UART1          0x00000032UL                         /**< Mode UART1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USART0         0x00000033UL                         /**< Mode USART0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USART1         0x00000034UL                         /**< Mode USART1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USART2         0x00000035UL                         /**< Mode USART2 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USART3         0x00000036UL                         /**< Mode USART3 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USART4         0x00000037UL                         /**< Mode USART4 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USART5         0x00000038UL                         /**< Mode USART5 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_USB            0x00000039UL                         /**< Mode USB for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_WDOG0          0x0000003AUL                         /**< Mode WDOG0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_WDOG1          0x0000003BUL                         /**< Mode WDOG1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_WTIMER0        0x0000003CUL                         /**< Mode WTIMER0 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_WTIMER1        0x0000003DUL                         /**< Mode WTIMER1 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_WTIMER2        0x0000003EUL                         /**< Mode WTIMER2 for SMU_PPUFS */
#define _SMU_PPUFS_PERIPHID_WTIMER3        0x0000003FUL                         /**< Mode WTIMER3 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_DEFAULT         (_SMU_PPUFS_PERIPHID_DEFAULT << 0)   /**< Shifted mode DEFAULT for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ACMP0           (_SMU_PPUFS_PERIPHID_ACMP0 << 0)     /**< Shifted mode ACMP0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ACMP1           (_SMU_PPUFS_PERIPHID_ACMP1 << 0)     /**< Shifted mode ACMP1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ACMP2           (_SMU_PPUFS_PERIPHID_ACMP2 << 0)     /**< Shifted mode ACMP2 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ACMP3           (_SMU_PPUFS_PERIPHID_ACMP3 << 0)     /**< Shifted mode ACMP3 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ADC0            (_SMU_PPUFS_PERIPHID_ADC0 << 0)      /**< Shifted mode ADC0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ADC1            (_SMU_PPUFS_PERIPHID_ADC1 << 0)      /**< Shifted mode ADC1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_CAN0            (_SMU_PPUFS_PERIPHID_CAN0 << 0)      /**< Shifted mode CAN0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_CAN1            (_SMU_PPUFS_PERIPHID_CAN1 << 0)      /**< Shifted mode CAN1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_CMU             (_SMU_PPUFS_PERIPHID_CMU << 0)       /**< Shifted mode CMU for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_CRYOTIMER       (_SMU_PPUFS_PERIPHID_CRYOTIMER << 0) /**< Shifted mode CRYOTIMER for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_CRYPTO0         (_SMU_PPUFS_PERIPHID_CRYPTO0 << 0)   /**< Shifted mode CRYPTO0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_CSEN            (_SMU_PPUFS_PERIPHID_CSEN << 0)      /**< Shifted mode CSEN for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_VDAC0           (_SMU_PPUFS_PERIPHID_VDAC0 << 0)     /**< Shifted mode VDAC0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_PRS             (_SMU_PPUFS_PERIPHID_PRS << 0)       /**< Shifted mode PRS for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_EBI             (_SMU_PPUFS_PERIPHID_EBI << 0)       /**< Shifted mode EBI for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_EMU             (_SMU_PPUFS_PERIPHID_EMU << 0)       /**< Shifted mode EMU for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_ETH             (_SMU_PPUFS_PERIPHID_ETH << 0)       /**< Shifted mode ETH for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_FPUEH           (_SMU_PPUFS_PERIPHID_FPUEH << 0)     /**< Shifted mode FPUEH for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_GPCRC           (_SMU_PPUFS_PERIPHID_GPCRC << 0)     /**< Shifted mode GPCRC for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_GPIO            (_SMU_PPUFS_PERIPHID_GPIO << 0)      /**< Shifted mode GPIO for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_I2C0            (_SMU_PPUFS_PERIPHID_I2C0 << 0)      /**< Shifted mode I2C0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_I2C1            (_SMU_PPUFS_PERIPHID_I2C1 << 0)      /**< Shifted mode I2C1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_I2C2            (_SMU_PPUFS_PERIPHID_I2C2 << 0)      /**< Shifted mode I2C2 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_IDAC0           (_SMU_PPUFS_PERIPHID_IDAC0 << 0)     /**< Shifted mode IDAC0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_MSC             (_SMU_PPUFS_PERIPHID_MSC << 0)       /**< Shifted mode MSC for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LCD             (_SMU_PPUFS_PERIPHID_LCD << 0)       /**< Shifted mode LCD for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LDMA            (_SMU_PPUFS_PERIPHID_LDMA << 0)      /**< Shifted mode LDMA for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LESENSE         (_SMU_PPUFS_PERIPHID_LESENSE << 0)   /**< Shifted mode LESENSE for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LETIMER0        (_SMU_PPUFS_PERIPHID_LETIMER0 << 0)  /**< Shifted mode LETIMER0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LETIMER1        (_SMU_PPUFS_PERIPHID_LETIMER1 << 0)  /**< Shifted mode LETIMER1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LEUART0         (_SMU_PPUFS_PERIPHID_LEUART0 << 0)   /**< Shifted mode LEUART0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_LEUART1         (_SMU_PPUFS_PERIPHID_LEUART1 << 0)   /**< Shifted mode LEUART1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_PCNT0           (_SMU_PPUFS_PERIPHID_PCNT0 << 0)     /**< Shifted mode PCNT0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_PCNT1           (_SMU_PPUFS_PERIPHID_PCNT1 << 0)     /**< Shifted mode PCNT1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_PCNT2           (_SMU_PPUFS_PERIPHID_PCNT2 << 0)     /**< Shifted mode PCNT2 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_QSPI0           (_SMU_PPUFS_PERIPHID_QSPI0 << 0)     /**< Shifted mode QSPI0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_RMU             (_SMU_PPUFS_PERIPHID_RMU << 0)       /**< Shifted mode RMU for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_RTC             (_SMU_PPUFS_PERIPHID_RTC << 0)       /**< Shifted mode RTC for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_RTCC            (_SMU_PPUFS_PERIPHID_RTCC << 0)      /**< Shifted mode RTCC for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_SDIO            (_SMU_PPUFS_PERIPHID_SDIO << 0)      /**< Shifted mode SDIO for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_SMU             (_SMU_PPUFS_PERIPHID_SMU << 0)       /**< Shifted mode SMU for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER0          (_SMU_PPUFS_PERIPHID_TIMER0 << 0)    /**< Shifted mode TIMER0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER1          (_SMU_PPUFS_PERIPHID_TIMER1 << 0)    /**< Shifted mode TIMER1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER2          (_SMU_PPUFS_PERIPHID_TIMER2 << 0)    /**< Shifted mode TIMER2 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER3          (_SMU_PPUFS_PERIPHID_TIMER3 << 0)    /**< Shifted mode TIMER3 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER4          (_SMU_PPUFS_PERIPHID_TIMER4 << 0)    /**< Shifted mode TIMER4 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER5          (_SMU_PPUFS_PERIPHID_TIMER5 << 0)    /**< Shifted mode TIMER5 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TIMER6          (_SMU_PPUFS_PERIPHID_TIMER6 << 0)    /**< Shifted mode TIMER6 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_TRNG0           (_SMU_PPUFS_PERIPHID_TRNG0 << 0)     /**< Shifted mode TRNG0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_UART0           (_SMU_PPUFS_PERIPHID_UART0 << 0)     /**< Shifted mode UART0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_UART1           (_SMU_PPUFS_PERIPHID_UART1 << 0)     /**< Shifted mode UART1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USART0          (_SMU_PPUFS_PERIPHID_USART0 << 0)    /**< Shifted mode USART0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USART1          (_SMU_PPUFS_PERIPHID_USART1 << 0)    /**< Shifted mode USART1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USART2          (_SMU_PPUFS_PERIPHID_USART2 << 0)    /**< Shifted mode USART2 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USART3          (_SMU_PPUFS_PERIPHID_USART3 << 0)    /**< Shifted mode USART3 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USART4          (_SMU_PPUFS_PERIPHID_USART4 << 0)    /**< Shifted mode USART4 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USART5          (_SMU_PPUFS_PERIPHID_USART5 << 0)    /**< Shifted mode USART5 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_USB             (_SMU_PPUFS_PERIPHID_USB << 0)       /**< Shifted mode USB for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_WDOG0           (_SMU_PPUFS_PERIPHID_WDOG0 << 0)     /**< Shifted mode WDOG0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_WDOG1           (_SMU_PPUFS_PERIPHID_WDOG1 << 0)     /**< Shifted mode WDOG1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_WTIMER0         (_SMU_PPUFS_PERIPHID_WTIMER0 << 0)   /**< Shifted mode WTIMER0 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_WTIMER1         (_SMU_PPUFS_PERIPHID_WTIMER1 << 0)   /**< Shifted mode WTIMER1 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_WTIMER2         (_SMU_PPUFS_PERIPHID_WTIMER2 << 0)   /**< Shifted mode WTIMER2 for SMU_PPUFS */
#define SMU_PPUFS_PERIPHID_WTIMER3         (_SMU_PPUFS_PERIPHID_WTIMER3 << 0)   /**< Shifted mode WTIMER3 for SMU_PPUFS */

/** @} */
/** @} End of group EFM32GG11B_SMU */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

