/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 MPAHBRAM register and bit field definitions
 ******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
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
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EFR32ZG23_MPAHBRAM_H
#define EFR32ZG23_MPAHBRAM_H
#define MPAHBRAM_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_MPAHBRAM MPAHBRAM
 * @{
 * @brief EFR32ZG23 MPAHBRAM Register Declaration.
 *****************************************************************************/

/** MPAHBRAM Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t CMD;                           /**< Command register                                   */
  __IOM uint32_t CTRL;                          /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0;                   /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1;                   /**< ECC Error Address 1                                */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND;                    /**< Multiple ECC error indication                      */
  __IOM uint32_t IF;                            /**< Interrupt Flags                                    */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  uint32_t       RESERVED1[7U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED3[1006U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t CMD_SET;                       /**< Command register                                   */
  __IOM uint32_t CTRL_SET;                      /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0_SET;               /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1_SET;               /**< ECC Error Address 1                                */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND_SET;                /**< Multiple ECC error indication                      */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED5[7U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[1006U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t CMD_CLR;                       /**< Command register                                   */
  __IOM uint32_t CTRL_CLR;                      /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0_CLR;               /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1_CLR;               /**< ECC Error Address 1                                */
  uint32_t       RESERVED8[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND_CLR;                /**< Multiple ECC error indication                      */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED9[7U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED11[1006U];             /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t CMD_TGL;                       /**< Command register                                   */
  __IOM uint32_t CTRL_TGL;                      /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0_TGL;               /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1_TGL;               /**< ECC Error Address 1                                */
  uint32_t       RESERVED12[2U];                /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND_TGL;                /**< Multiple ECC error indication                      */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED13[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
} MPAHBRAM_TypeDef;
/** @} End of group EFR32ZG23_MPAHBRAM */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_MPAHBRAM
 * @{
 * @defgroup EFR32ZG23_MPAHBRAM_BitFields MPAHBRAM Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for MPAHBRAM IPVERSION */
#define _MPAHBRAM_IPVERSION_RESETVALUE            0x00000001UL                                 /**< Default value for MPAHBRAM_IPVERSION        */
#define _MPAHBRAM_IPVERSION_MASK                  0x00000001UL                                 /**< Mask for MPAHBRAM_IPVERSION                 */
#define MPAHBRAM_IPVERSION_IPVERSION              (0x1UL << 0)                                 /**< New BitField                                */
#define _MPAHBRAM_IPVERSION_IPVERSION_SHIFT       0                                            /**< Shift value for MPAHBRAM_IPVERSION          */
#define _MPAHBRAM_IPVERSION_IPVERSION_MASK        0x1UL                                        /**< Bit mask for MPAHBRAM_IPVERSION             */
#define _MPAHBRAM_IPVERSION_IPVERSION_DEFAULT     0x00000001UL                                 /**< Mode DEFAULT for MPAHBRAM_IPVERSION         */
#define MPAHBRAM_IPVERSION_IPVERSION_DEFAULT      (_MPAHBRAM_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_IPVERSION */

/* Bit fields for MPAHBRAM CMD */
#define _MPAHBRAM_CMD_RESETVALUE                  0x00000000UL                               /**< Default value for MPAHBRAM_CMD              */
#define _MPAHBRAM_CMD_MASK                        0x00000003UL                               /**< Mask for MPAHBRAM_CMD                       */
#define MPAHBRAM_CMD_CLEARECCADDR0                (0x1UL << 0)                               /**< Clear ECCERRADDR0                           */
#define _MPAHBRAM_CMD_CLEARECCADDR0_SHIFT         0                                          /**< Shift value for MPAHBRAM_CLEARECCADDR0      */
#define _MPAHBRAM_CMD_CLEARECCADDR0_MASK          0x1UL                                      /**< Bit mask for MPAHBRAM_CLEARECCADDR0         */
#define _MPAHBRAM_CMD_CLEARECCADDR0_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for MPAHBRAM_CMD               */
#define MPAHBRAM_CMD_CLEARECCADDR0_DEFAULT        (_MPAHBRAM_CMD_CLEARECCADDR0_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_CMD       */
#define MPAHBRAM_CMD_CLEARECCADDR1                (0x1UL << 1)                               /**< Clear ECCERRADDR1                           */
#define _MPAHBRAM_CMD_CLEARECCADDR1_SHIFT         1                                          /**< Shift value for MPAHBRAM_CLEARECCADDR1      */
#define _MPAHBRAM_CMD_CLEARECCADDR1_MASK          0x2UL                                      /**< Bit mask for MPAHBRAM_CLEARECCADDR1         */
#define _MPAHBRAM_CMD_CLEARECCADDR1_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for MPAHBRAM_CMD               */
#define MPAHBRAM_CMD_CLEARECCADDR1_DEFAULT        (_MPAHBRAM_CMD_CLEARECCADDR1_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_CMD       */

/* Bit fields for MPAHBRAM CTRL */
#define _MPAHBRAM_CTRL_RESETVALUE                 0x00000040UL                                  /**< Default value for MPAHBRAM_CTRL             */
#define _MPAHBRAM_CTRL_MASK                       0x0000007FUL                                  /**< Mask for MPAHBRAM_CTRL                      */
#define MPAHBRAM_CTRL_ECCEN                       (0x1UL << 0)                                  /**< Enable ECC functionality                    */
#define _MPAHBRAM_CTRL_ECCEN_SHIFT                0                                             /**< Shift value for MPAHBRAM_ECCEN              */
#define _MPAHBRAM_CTRL_ECCEN_MASK                 0x1UL                                         /**< Bit mask for MPAHBRAM_ECCEN                 */
#define _MPAHBRAM_CTRL_ECCEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ECCEN_DEFAULT               (_MPAHBRAM_CTRL_ECCEN_DEFAULT << 0)           /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define MPAHBRAM_CTRL_ECCWEN                      (0x1UL << 1)                                  /**< Enable ECC syndrome writes                  */
#define _MPAHBRAM_CTRL_ECCWEN_SHIFT               1                                             /**< Shift value for MPAHBRAM_ECCWEN             */
#define _MPAHBRAM_CTRL_ECCWEN_MASK                0x2UL                                         /**< Bit mask for MPAHBRAM_ECCWEN                */
#define _MPAHBRAM_CTRL_ECCWEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ECCWEN_DEFAULT              (_MPAHBRAM_CTRL_ECCWEN_DEFAULT << 1)          /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define MPAHBRAM_CTRL_ECCERRFAULTEN               (0x1UL << 2)                                  /**< ECC Error bus fault enable                  */
#define _MPAHBRAM_CTRL_ECCERRFAULTEN_SHIFT        2                                             /**< Shift value for MPAHBRAM_ECCERRFAULTEN      */
#define _MPAHBRAM_CTRL_ECCERRFAULTEN_MASK         0x4UL                                         /**< Bit mask for MPAHBRAM_ECCERRFAULTEN         */
#define _MPAHBRAM_CTRL_ECCERRFAULTEN_DEFAULT      0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ECCERRFAULTEN_DEFAULT       (_MPAHBRAM_CTRL_ECCERRFAULTEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_SHIFT      3                                             /**< Shift value for MPAHBRAM_AHBPORTPRIORITY    */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_MASK       0x38UL                                        /**< Bit mask for MPAHBRAM_AHBPORTPRIORITY       */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_DEFAULT    0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE       0x00000000UL                                  /**< Mode NONE for MPAHBRAM_CTRL                 */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0      0x00000001UL                                  /**< Mode PORT0 for MPAHBRAM_CTRL                */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1      0x00000002UL                                  /**< Mode PORT1 for MPAHBRAM_CTRL                */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_DEFAULT     (_MPAHBRAM_CTRL_AHBPORTPRIORITY_DEFAULT << 3) /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE        (_MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE << 3)    /**< Shifted mode NONE for MPAHBRAM_CTRL         */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0       (_MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0 << 3)   /**< Shifted mode PORT0 for MPAHBRAM_CTRL        */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1       (_MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1 << 3)   /**< Shifted mode PORT1 for MPAHBRAM_CTRL        */
#define MPAHBRAM_CTRL_ADDRFAULTEN                 (0x1UL << 6)                                  /**< Address fault bus fault enable              */
#define _MPAHBRAM_CTRL_ADDRFAULTEN_SHIFT          6                                             /**< Shift value for MPAHBRAM_ADDRFAULTEN        */
#define _MPAHBRAM_CTRL_ADDRFAULTEN_MASK           0x40UL                                        /**< Bit mask for MPAHBRAM_ADDRFAULTEN           */
#define _MPAHBRAM_CTRL_ADDRFAULTEN_DEFAULT        0x00000001UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ADDRFAULTEN_DEFAULT         (_MPAHBRAM_CTRL_ADDRFAULTEN_DEFAULT << 6)     /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */

/* Bit fields for MPAHBRAM ECCERRADDR0 */
#define _MPAHBRAM_ECCERRADDR0_RESETVALUE          0x00000000UL                              /**< Default value for MPAHBRAM_ECCERRADDR0      */
#define _MPAHBRAM_ECCERRADDR0_MASK                0xFFFFFFFFUL                              /**< Mask for MPAHBRAM_ECCERRADDR0               */
#define _MPAHBRAM_ECCERRADDR0_ADDR_SHIFT          0                                         /**< Shift value for MPAHBRAM_ADDR               */
#define _MPAHBRAM_ECCERRADDR0_ADDR_MASK           0xFFFFFFFFUL                              /**< Bit mask for MPAHBRAM_ADDR                  */
#define _MPAHBRAM_ECCERRADDR0_ADDR_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for MPAHBRAM_ECCERRADDR0       */
#define MPAHBRAM_ECCERRADDR0_ADDR_DEFAULT         (_MPAHBRAM_ECCERRADDR0_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_ECCERRADDR0*/

/* Bit fields for MPAHBRAM ECCERRADDR1 */
#define _MPAHBRAM_ECCERRADDR1_RESETVALUE          0x00000000UL                              /**< Default value for MPAHBRAM_ECCERRADDR1      */
#define _MPAHBRAM_ECCERRADDR1_MASK                0xFFFFFFFFUL                              /**< Mask for MPAHBRAM_ECCERRADDR1               */
#define _MPAHBRAM_ECCERRADDR1_ADDR_SHIFT          0                                         /**< Shift value for MPAHBRAM_ADDR               */
#define _MPAHBRAM_ECCERRADDR1_ADDR_MASK           0xFFFFFFFFUL                              /**< Bit mask for MPAHBRAM_ADDR                  */
#define _MPAHBRAM_ECCERRADDR1_ADDR_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for MPAHBRAM_ECCERRADDR1       */
#define MPAHBRAM_ECCERRADDR1_ADDR_DEFAULT         (_MPAHBRAM_ECCERRADDR1_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_ECCERRADDR1*/

/* Bit fields for MPAHBRAM ECCMERRIND */
#define _MPAHBRAM_ECCMERRIND_RESETVALUE           0x00000000UL                           /**< Default value for MPAHBRAM_ECCMERRIND       */
#define _MPAHBRAM_ECCMERRIND_MASK                 0x00000003UL                           /**< Mask for MPAHBRAM_ECCMERRIND                */
#define MPAHBRAM_ECCMERRIND_P0                    (0x1UL << 0)                           /**< Multiple ECC errors on AHB port 0           */
#define _MPAHBRAM_ECCMERRIND_P0_SHIFT             0                                      /**< Shift value for MPAHBRAM_P0                 */
#define _MPAHBRAM_ECCMERRIND_P0_MASK              0x1UL                                  /**< Bit mask for MPAHBRAM_P0                    */
#define _MPAHBRAM_ECCMERRIND_P0_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_ECCMERRIND        */
#define MPAHBRAM_ECCMERRIND_P0_DEFAULT            (_MPAHBRAM_ECCMERRIND_P0_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_ECCMERRIND*/
#define MPAHBRAM_ECCMERRIND_P1                    (0x1UL << 1)                           /**< Multiple ECC errors on AHB port 1           */
#define _MPAHBRAM_ECCMERRIND_P1_SHIFT             1                                      /**< Shift value for MPAHBRAM_P1                 */
#define _MPAHBRAM_ECCMERRIND_P1_MASK              0x2UL                                  /**< Bit mask for MPAHBRAM_P1                    */
#define _MPAHBRAM_ECCMERRIND_P1_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_ECCMERRIND        */
#define MPAHBRAM_ECCMERRIND_P1_DEFAULT            (_MPAHBRAM_ECCMERRIND_P1_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_ECCMERRIND*/

/* Bit fields for MPAHBRAM IF */
#define _MPAHBRAM_IF_RESETVALUE                   0x00000000UL                          /**< Default value for MPAHBRAM_IF               */
#define _MPAHBRAM_IF_MASK                         0x00000033UL                          /**< Mask for MPAHBRAM_IF                        */
#define MPAHBRAM_IF_AHB0ERR1B                     (0x1UL << 0)                          /**< AHB0 1-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB0ERR1B_SHIFT              0                                     /**< Shift value for MPAHBRAM_AHB0ERR1B          */
#define _MPAHBRAM_IF_AHB0ERR1B_MASK               0x1UL                                 /**< Bit mask for MPAHBRAM_AHB0ERR1B             */
#define _MPAHBRAM_IF_AHB0ERR1B_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB0ERR1B_DEFAULT             (_MPAHBRAM_IF_AHB0ERR1B_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */
#define MPAHBRAM_IF_AHB1ERR1B                     (0x1UL << 1)                          /**< AHB1 1-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB1ERR1B_SHIFT              1                                     /**< Shift value for MPAHBRAM_AHB1ERR1B          */
#define _MPAHBRAM_IF_AHB1ERR1B_MASK               0x2UL                                 /**< Bit mask for MPAHBRAM_AHB1ERR1B             */
#define _MPAHBRAM_IF_AHB1ERR1B_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB1ERR1B_DEFAULT             (_MPAHBRAM_IF_AHB1ERR1B_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */
#define MPAHBRAM_IF_AHB0ERR2B                     (0x1UL << 4)                          /**< AHB0 2-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB0ERR2B_SHIFT              4                                     /**< Shift value for MPAHBRAM_AHB0ERR2B          */
#define _MPAHBRAM_IF_AHB0ERR2B_MASK               0x10UL                                /**< Bit mask for MPAHBRAM_AHB0ERR2B             */
#define _MPAHBRAM_IF_AHB0ERR2B_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB0ERR2B_DEFAULT             (_MPAHBRAM_IF_AHB0ERR2B_DEFAULT << 4) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */
#define MPAHBRAM_IF_AHB1ERR2B                     (0x1UL << 5)                          /**< AHB1 2-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB1ERR2B_SHIFT              5                                     /**< Shift value for MPAHBRAM_AHB1ERR2B          */
#define _MPAHBRAM_IF_AHB1ERR2B_MASK               0x20UL                                /**< Bit mask for MPAHBRAM_AHB1ERR2B             */
#define _MPAHBRAM_IF_AHB1ERR2B_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB1ERR2B_DEFAULT             (_MPAHBRAM_IF_AHB1ERR2B_DEFAULT << 5) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */

/* Bit fields for MPAHBRAM IEN */
#define _MPAHBRAM_IEN_RESETVALUE                  0x00000000UL                           /**< Default value for MPAHBRAM_IEN              */
#define _MPAHBRAM_IEN_MASK                        0x00000033UL                           /**< Mask for MPAHBRAM_IEN                       */
#define MPAHBRAM_IEN_AHB0ERR1B                    (0x1UL << 0)                           /**< AHB0 1-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB0ERR1B_SHIFT             0                                      /**< Shift value for MPAHBRAM_AHB0ERR1B          */
#define _MPAHBRAM_IEN_AHB0ERR1B_MASK              0x1UL                                  /**< Bit mask for MPAHBRAM_AHB0ERR1B             */
#define _MPAHBRAM_IEN_AHB0ERR1B_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB0ERR1B_DEFAULT            (_MPAHBRAM_IEN_AHB0ERR1B_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */
#define MPAHBRAM_IEN_AHB1ERR1B                    (0x1UL << 1)                           /**< AHB1 1-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB1ERR1B_SHIFT             1                                      /**< Shift value for MPAHBRAM_AHB1ERR1B          */
#define _MPAHBRAM_IEN_AHB1ERR1B_MASK              0x2UL                                  /**< Bit mask for MPAHBRAM_AHB1ERR1B             */
#define _MPAHBRAM_IEN_AHB1ERR1B_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB1ERR1B_DEFAULT            (_MPAHBRAM_IEN_AHB1ERR1B_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */
#define MPAHBRAM_IEN_AHB0ERR2B                    (0x1UL << 4)                           /**< AHB0 2-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB0ERR2B_SHIFT             4                                      /**< Shift value for MPAHBRAM_AHB0ERR2B          */
#define _MPAHBRAM_IEN_AHB0ERR2B_MASK              0x10UL                                 /**< Bit mask for MPAHBRAM_AHB0ERR2B             */
#define _MPAHBRAM_IEN_AHB0ERR2B_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB0ERR2B_DEFAULT            (_MPAHBRAM_IEN_AHB0ERR2B_DEFAULT << 4) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */
#define MPAHBRAM_IEN_AHB1ERR2B                    (0x1UL << 5)                           /**< AHB1 2-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB1ERR2B_SHIFT             5                                      /**< Shift value for MPAHBRAM_AHB1ERR2B          */
#define _MPAHBRAM_IEN_AHB1ERR2B_MASK              0x20UL                                 /**< Bit mask for MPAHBRAM_AHB1ERR2B             */
#define _MPAHBRAM_IEN_AHB1ERR2B_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB1ERR2B_DEFAULT            (_MPAHBRAM_IEN_AHB1ERR2B_DEFAULT << 5) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */

/** @} End of group EFR32ZG23_MPAHBRAM_BitFields */
/** @} End of group EFR32ZG23_MPAHBRAM */
/** @} End of group Parts */

#endif /* EFR32ZG23_MPAHBRAM_H */
#ifdef __cplusplus
}
#endif

