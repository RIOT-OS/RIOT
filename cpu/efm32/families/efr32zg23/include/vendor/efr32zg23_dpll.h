/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 DPLL register and bit field definitions
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

#ifndef EFR32ZG23_DPLL_H
#define EFR32ZG23_DPLL_H
#define DPLL_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_DPLL DPLL
 * @{
 * @brief EFR32ZG23 DPLL Register Declaration.
 *****************************************************************************/

/** DPLL Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version                                         */
  __IOM uint32_t EN;                            /**< Enable                                             */
  __IOM uint32_t CFG;                           /**< Config                                             */
  __IOM uint32_t CFG1;                          /**< Config1                                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag                                     */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS;                        /**< Status                                             */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Lock                                               */
  uint32_t       RESERVED1[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version                                         */
  __IOM uint32_t EN_SET;                        /**< Enable                                             */
  __IOM uint32_t CFG_SET;                       /**< Config                                             */
  __IOM uint32_t CFG1_SET;                      /**< Config1                                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status                                             */
  uint32_t       RESERVED2[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Lock                                               */
  uint32_t       RESERVED3[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version                                         */
  __IOM uint32_t EN_CLR;                        /**< Enable                                             */
  __IOM uint32_t CFG_CLR;                       /**< Config                                             */
  __IOM uint32_t CFG1_CLR;                      /**< Config1                                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status                                             */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Lock                                               */
  uint32_t       RESERVED5[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version                                         */
  __IOM uint32_t EN_TGL;                        /**< Enable                                             */
  __IOM uint32_t CFG_TGL;                       /**< Config                                             */
  __IOM uint32_t CFG1_TGL;                      /**< Config1                                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status                                             */
  uint32_t       RESERVED6[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Lock                                               */
} DPLL_TypeDef;
/** @} End of group EFR32ZG23_DPLL */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_DPLL
 * @{
 * @defgroup EFR32ZG23_DPLL_BitFields DPLL Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for DPLL IPVERSION */
#define _DPLL_IPVERSION_RESETVALUE           0x00000001UL                               /**< Default value for DPLL_IPVERSION            */
#define _DPLL_IPVERSION_MASK                 0xFFFFFFFFUL                               /**< Mask for DPLL_IPVERSION                     */
#define _DPLL_IPVERSION_IPVERSION_SHIFT      0                                          /**< Shift value for DPLL_IPVERSION              */
#define _DPLL_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                               /**< Bit mask for DPLL_IPVERSION                 */
#define _DPLL_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                               /**< Mode DEFAULT for DPLL_IPVERSION             */
#define DPLL_IPVERSION_IPVERSION_DEFAULT     (_DPLL_IPVERSION_IPVERSION_DEFAULT << 0)   /**< Shifted mode DEFAULT for DPLL_IPVERSION     */

/* Bit fields for DPLL EN */
#define _DPLL_EN_RESETVALUE                  0x00000000UL                               /**< Default value for DPLL_EN                   */
#define _DPLL_EN_MASK                        0x00000003UL                               /**< Mask for DPLL_EN                            */
#define DPLL_EN_EN                           (0x1UL << 0)                               /**< Module Enable                               */
#define _DPLL_EN_EN_SHIFT                    0                                          /**< Shift value for DPLL_EN                     */
#define _DPLL_EN_EN_MASK                     0x1UL                                      /**< Bit mask for DPLL_EN                        */
#define _DPLL_EN_EN_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for DPLL_EN                    */
#define DPLL_EN_EN_DEFAULT                   (_DPLL_EN_EN_DEFAULT << 0)                 /**< Shifted mode DEFAULT for DPLL_EN            */
#define DPLL_EN_DISABLING                    (0x1UL << 1)                               /**< Disablement Busy Status                     */
#define _DPLL_EN_DISABLING_SHIFT             1                                          /**< Shift value for DPLL_DISABLING              */
#define _DPLL_EN_DISABLING_MASK              0x2UL                                      /**< Bit mask for DPLL_DISABLING                 */
#define _DPLL_EN_DISABLING_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for DPLL_EN                    */
#define DPLL_EN_DISABLING_DEFAULT            (_DPLL_EN_DISABLING_DEFAULT << 1)          /**< Shifted mode DEFAULT for DPLL_EN            */

/* Bit fields for DPLL CFG */
#define _DPLL_CFG_RESETVALUE                 0x00000000UL                               /**< Default value for DPLL_CFG                  */
#define _DPLL_CFG_MASK                       0x00000047UL                               /**< Mask for DPLL_CFG                           */
#define DPLL_CFG_MODE                        (0x1UL << 0)                               /**< Operating Mode Control                      */
#define _DPLL_CFG_MODE_SHIFT                 0                                          /**< Shift value for DPLL_MODE                   */
#define _DPLL_CFG_MODE_MASK                  0x1UL                                      /**< Bit mask for DPLL_MODE                      */
#define _DPLL_CFG_MODE_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define _DPLL_CFG_MODE_FLL                   0x00000000UL                               /**< Mode FLL for DPLL_CFG                       */
#define _DPLL_CFG_MODE_PLL                   0x00000001UL                               /**< Mode PLL for DPLL_CFG                       */
#define DPLL_CFG_MODE_DEFAULT                (_DPLL_CFG_MODE_DEFAULT << 0)              /**< Shifted mode DEFAULT for DPLL_CFG           */
#define DPLL_CFG_MODE_FLL                    (_DPLL_CFG_MODE_FLL << 0)                  /**< Shifted mode FLL for DPLL_CFG               */
#define DPLL_CFG_MODE_PLL                    (_DPLL_CFG_MODE_PLL << 0)                  /**< Shifted mode PLL for DPLL_CFG               */
#define DPLL_CFG_EDGESEL                     (0x1UL << 1)                               /**< Reference Edge Select                       */
#define _DPLL_CFG_EDGESEL_SHIFT              1                                          /**< Shift value for DPLL_EDGESEL                */
#define _DPLL_CFG_EDGESEL_MASK               0x2UL                                      /**< Bit mask for DPLL_EDGESEL                   */
#define _DPLL_CFG_EDGESEL_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define DPLL_CFG_EDGESEL_DEFAULT             (_DPLL_CFG_EDGESEL_DEFAULT << 1)           /**< Shifted mode DEFAULT for DPLL_CFG           */
#define DPLL_CFG_AUTORECOVER                 (0x1UL << 2)                               /**< Automatic Recovery Control                  */
#define _DPLL_CFG_AUTORECOVER_SHIFT          2                                          /**< Shift value for DPLL_AUTORECOVER            */
#define _DPLL_CFG_AUTORECOVER_MASK           0x4UL                                      /**< Bit mask for DPLL_AUTORECOVER               */
#define _DPLL_CFG_AUTORECOVER_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define DPLL_CFG_AUTORECOVER_DEFAULT         (_DPLL_CFG_AUTORECOVER_DEFAULT << 2)       /**< Shifted mode DEFAULT for DPLL_CFG           */
#define DPLL_CFG_DITHEN                      (0x1UL << 6)                               /**< Dither Enable Control                       */
#define _DPLL_CFG_DITHEN_SHIFT               6                                          /**< Shift value for DPLL_DITHEN                 */
#define _DPLL_CFG_DITHEN_MASK                0x40UL                                     /**< Bit mask for DPLL_DITHEN                    */
#define _DPLL_CFG_DITHEN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define DPLL_CFG_DITHEN_DEFAULT              (_DPLL_CFG_DITHEN_DEFAULT << 6)            /**< Shifted mode DEFAULT for DPLL_CFG           */

/* Bit fields for DPLL CFG1 */
#define _DPLL_CFG1_RESETVALUE                0x00000000UL                               /**< Default value for DPLL_CFG1                 */
#define _DPLL_CFG1_MASK                      0x0FFF0FFFUL                               /**< Mask for DPLL_CFG1                          */
#define _DPLL_CFG1_M_SHIFT                   0                                          /**< Shift value for DPLL_M                      */
#define _DPLL_CFG1_M_MASK                    0xFFFUL                                    /**< Bit mask for DPLL_M                         */
#define _DPLL_CFG1_M_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG1                  */
#define DPLL_CFG1_M_DEFAULT                  (_DPLL_CFG1_M_DEFAULT << 0)                /**< Shifted mode DEFAULT for DPLL_CFG1          */
#define _DPLL_CFG1_N_SHIFT                   16                                         /**< Shift value for DPLL_N                      */
#define _DPLL_CFG1_N_MASK                    0xFFF0000UL                                /**< Bit mask for DPLL_N                         */
#define _DPLL_CFG1_N_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG1                  */
#define DPLL_CFG1_N_DEFAULT                  (_DPLL_CFG1_N_DEFAULT << 16)               /**< Shifted mode DEFAULT for DPLL_CFG1          */

/* Bit fields for DPLL IF */
#define _DPLL_IF_RESETVALUE                  0x00000000UL                               /**< Default value for DPLL_IF                   */
#define _DPLL_IF_MASK                        0x00000007UL                               /**< Mask for DPLL_IF                            */
#define DPLL_IF_LOCK                         (0x1UL << 0)                               /**< Lock Interrupt Flag                         */
#define _DPLL_IF_LOCK_SHIFT                  0                                          /**< Shift value for DPLL_LOCK                   */
#define _DPLL_IF_LOCK_MASK                   0x1UL                                      /**< Bit mask for DPLL_LOCK                      */
#define _DPLL_IF_LOCK_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for DPLL_IF                    */
#define DPLL_IF_LOCK_DEFAULT                 (_DPLL_IF_LOCK_DEFAULT << 0)               /**< Shifted mode DEFAULT for DPLL_IF            */
#define DPLL_IF_LOCKFAILLOW                  (0x1UL << 1)                               /**< Lock Failure Low Interrupt Flag             */
#define _DPLL_IF_LOCKFAILLOW_SHIFT           1                                          /**< Shift value for DPLL_LOCKFAILLOW            */
#define _DPLL_IF_LOCKFAILLOW_MASK            0x2UL                                      /**< Bit mask for DPLL_LOCKFAILLOW               */
#define _DPLL_IF_LOCKFAILLOW_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for DPLL_IF                    */
#define DPLL_IF_LOCKFAILLOW_DEFAULT          (_DPLL_IF_LOCKFAILLOW_DEFAULT << 1)        /**< Shifted mode DEFAULT for DPLL_IF            */
#define DPLL_IF_LOCKFAILHIGH                 (0x1UL << 2)                               /**< Lock Failure High Interrupt Flag            */
#define _DPLL_IF_LOCKFAILHIGH_SHIFT          2                                          /**< Shift value for DPLL_LOCKFAILHIGH           */
#define _DPLL_IF_LOCKFAILHIGH_MASK           0x4UL                                      /**< Bit mask for DPLL_LOCKFAILHIGH              */
#define _DPLL_IF_LOCKFAILHIGH_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for DPLL_IF                    */
#define DPLL_IF_LOCKFAILHIGH_DEFAULT         (_DPLL_IF_LOCKFAILHIGH_DEFAULT << 2)       /**< Shifted mode DEFAULT for DPLL_IF            */

/* Bit fields for DPLL IEN */
#define _DPLL_IEN_RESETVALUE                 0x00000000UL                               /**< Default value for DPLL_IEN                  */
#define _DPLL_IEN_MASK                       0x00000007UL                               /**< Mask for DPLL_IEN                           */
#define DPLL_IEN_LOCK                        (0x1UL << 0)                               /**< LOCK interrupt Enable                       */
#define _DPLL_IEN_LOCK_SHIFT                 0                                          /**< Shift value for DPLL_LOCK                   */
#define _DPLL_IEN_LOCK_MASK                  0x1UL                                      /**< Bit mask for DPLL_LOCK                      */
#define _DPLL_IEN_LOCK_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for DPLL_IEN                   */
#define DPLL_IEN_LOCK_DEFAULT                (_DPLL_IEN_LOCK_DEFAULT << 0)              /**< Shifted mode DEFAULT for DPLL_IEN           */
#define DPLL_IEN_LOCKFAILLOW                 (0x1UL << 1)                               /**< LOCKFAILLOW Interrupe Enable                */
#define _DPLL_IEN_LOCKFAILLOW_SHIFT          1                                          /**< Shift value for DPLL_LOCKFAILLOW            */
#define _DPLL_IEN_LOCKFAILLOW_MASK           0x2UL                                      /**< Bit mask for DPLL_LOCKFAILLOW               */
#define _DPLL_IEN_LOCKFAILLOW_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for DPLL_IEN                   */
#define DPLL_IEN_LOCKFAILLOW_DEFAULT         (_DPLL_IEN_LOCKFAILLOW_DEFAULT << 1)       /**< Shifted mode DEFAULT for DPLL_IEN           */
#define DPLL_IEN_LOCKFAILHIGH                (0x1UL << 2)                               /**< LOCKFAILHIGH Interrupt Enable               */
#define _DPLL_IEN_LOCKFAILHIGH_SHIFT         2                                          /**< Shift value for DPLL_LOCKFAILHIGH           */
#define _DPLL_IEN_LOCKFAILHIGH_MASK          0x4UL                                      /**< Bit mask for DPLL_LOCKFAILHIGH              */
#define _DPLL_IEN_LOCKFAILHIGH_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for DPLL_IEN                   */
#define DPLL_IEN_LOCKFAILHIGH_DEFAULT        (_DPLL_IEN_LOCKFAILHIGH_DEFAULT << 2)      /**< Shifted mode DEFAULT for DPLL_IEN           */

/* Bit fields for DPLL STATUS */
#define _DPLL_STATUS_RESETVALUE              0x00000000UL                               /**< Default value for DPLL_STATUS               */
#define _DPLL_STATUS_MASK                    0x80000003UL                               /**< Mask for DPLL_STATUS                        */
#define DPLL_STATUS_RDY                      (0x1UL << 0)                               /**< Ready Status                                */
#define _DPLL_STATUS_RDY_SHIFT               0                                          /**< Shift value for DPLL_RDY                    */
#define _DPLL_STATUS_RDY_MASK                0x1UL                                      /**< Bit mask for DPLL_RDY                       */
#define _DPLL_STATUS_RDY_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for DPLL_STATUS                */
#define DPLL_STATUS_RDY_DEFAULT              (_DPLL_STATUS_RDY_DEFAULT << 0)            /**< Shifted mode DEFAULT for DPLL_STATUS        */
#define DPLL_STATUS_ENS                      (0x1UL << 1)                               /**< Enable Status                               */
#define _DPLL_STATUS_ENS_SHIFT               1                                          /**< Shift value for DPLL_ENS                    */
#define _DPLL_STATUS_ENS_MASK                0x2UL                                      /**< Bit mask for DPLL_ENS                       */
#define _DPLL_STATUS_ENS_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for DPLL_STATUS                */
#define DPLL_STATUS_ENS_DEFAULT              (_DPLL_STATUS_ENS_DEFAULT << 1)            /**< Shifted mode DEFAULT for DPLL_STATUS        */
#define DPLL_STATUS_LOCK                     (0x1UL << 31)                              /**< Lock Status                                 */
#define _DPLL_STATUS_LOCK_SHIFT              31                                         /**< Shift value for DPLL_LOCK                   */
#define _DPLL_STATUS_LOCK_MASK               0x80000000UL                               /**< Bit mask for DPLL_LOCK                      */
#define _DPLL_STATUS_LOCK_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for DPLL_STATUS                */
#define _DPLL_STATUS_LOCK_UNLOCKED           0x00000000UL                               /**< Mode UNLOCKED for DPLL_STATUS               */
#define _DPLL_STATUS_LOCK_LOCKED             0x00000001UL                               /**< Mode LOCKED for DPLL_STATUS                 */
#define DPLL_STATUS_LOCK_DEFAULT             (_DPLL_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for DPLL_STATUS        */
#define DPLL_STATUS_LOCK_UNLOCKED            (_DPLL_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for DPLL_STATUS       */
#define DPLL_STATUS_LOCK_LOCKED              (_DPLL_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for DPLL_STATUS         */

/* Bit fields for DPLL LOCK */
#define _DPLL_LOCK_RESETVALUE                0x00007102UL                               /**< Default value for DPLL_LOCK                 */
#define _DPLL_LOCK_MASK                      0x0000FFFFUL                               /**< Mask for DPLL_LOCK                          */
#define _DPLL_LOCK_LOCKKEY_SHIFT             0                                          /**< Shift value for DPLL_LOCKKEY                */
#define _DPLL_LOCK_LOCKKEY_MASK              0xFFFFUL                                   /**< Bit mask for DPLL_LOCKKEY                   */
#define _DPLL_LOCK_LOCKKEY_DEFAULT           0x00007102UL                               /**< Mode DEFAULT for DPLL_LOCK                  */
#define _DPLL_LOCK_LOCKKEY_UNLOCK            0x00007102UL                               /**< Mode UNLOCK for DPLL_LOCK                   */
#define DPLL_LOCK_LOCKKEY_DEFAULT            (_DPLL_LOCK_LOCKKEY_DEFAULT << 0)          /**< Shifted mode DEFAULT for DPLL_LOCK          */
#define DPLL_LOCK_LOCKKEY_UNLOCK             (_DPLL_LOCK_LOCKKEY_UNLOCK << 0)           /**< Shifted mode UNLOCK for DPLL_LOCK           */

/** @} End of group EFR32ZG23_DPLL_BitFields */
/** @} End of group EFR32ZG23_DPLL */
/** @} End of group Parts */

#endif /* EFR32ZG23_DPLL_H */
#ifdef __cplusplus
}
#endif

