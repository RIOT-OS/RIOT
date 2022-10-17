/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 MAILBOX register and bit field definitions
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

#ifndef EFR32ZG23_MAILBOX_H
#define EFR32ZG23_MAILBOX_H
#define MAILBOX_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_MAILBOX MAILBOX
 * @{
 * @brief EFR32ZG23 MAILBOX Register Declaration.
 *****************************************************************************/

/** MAILBOX MSGPTRS Register Group Declaration. */
typedef struct {
  __IOM uint32_t MSGPTR;                             /**< Message Pointer                                    */
} MAILBOX_MSGPTRS_TypeDef;

/** MAILBOX Register Declaration. */
typedef struct {
  MAILBOX_MSGPTRS_TypeDef MSGPTRS[4U];          /**< Message Pointers                                   */
  uint32_t                RESERVED0[12U];       /**< Reserved for future use                            */
  __IOM uint32_t          IF;                   /**< Interrupt Flag register                            */
  __IOM uint32_t          IEN;                  /**< Interrupt Enable register                          */
  uint32_t                RESERVED1[1006U];     /**< Reserved for future use                            */
  MAILBOX_MSGPTRS_TypeDef MSGPTRS_SET[4U];      /**< Message Pointers                                   */
  uint32_t                RESERVED2[12U];       /**< Reserved for future use                            */
  __IOM uint32_t          IF_SET;               /**< Interrupt Flag register                            */
  __IOM uint32_t          IEN_SET;              /**< Interrupt Enable register                          */
  uint32_t                RESERVED3[1006U];     /**< Reserved for future use                            */
  MAILBOX_MSGPTRS_TypeDef MSGPTRS_CLR[4U];      /**< Message Pointers                                   */
  uint32_t                RESERVED4[12U];       /**< Reserved for future use                            */
  __IOM uint32_t          IF_CLR;               /**< Interrupt Flag register                            */
  __IOM uint32_t          IEN_CLR;              /**< Interrupt Enable register                          */
  uint32_t                RESERVED5[1006U];     /**< Reserved for future use                            */
  MAILBOX_MSGPTRS_TypeDef MSGPTRS_TGL[4U];      /**< Message Pointers                                   */
  uint32_t                RESERVED6[12U];       /**< Reserved for future use                            */
  __IOM uint32_t          IF_TGL;               /**< Interrupt Flag register                            */
  __IOM uint32_t          IEN_TGL;              /**< Interrupt Enable register                          */
} MAILBOX_TypeDef;
/** @} End of group EFR32ZG23_MAILBOX */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_MAILBOX
 * @{
 * @defgroup EFR32ZG23_MAILBOX_BitFields MAILBOX Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for MAILBOX MSGPTR */
#define _MAILBOX_MSGPTR_RESETVALUE       0x00000000UL                                   /**< Default value for MAILBOX_MSGPTR            */
#define _MAILBOX_MSGPTR_MASK             0xFFFFFFFFUL                                   /**< Mask for MAILBOX_MSGPTR                     */
#define _MAILBOX_MSGPTR_PTR_SHIFT        0                                              /**< Shift value for MAILBOX_PTR                 */
#define _MAILBOX_MSGPTR_PTR_MASK         0xFFFFFFFFUL                                   /**< Bit mask for MAILBOX_PTR                    */
#define _MAILBOX_MSGPTR_PTR_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_MSGPTR             */
#define MAILBOX_MSGPTR_PTR_DEFAULT       (_MAILBOX_MSGPTR_PTR_DEFAULT << 0)             /**< Shifted mode DEFAULT for MAILBOX_MSGPTR     */

/* Bit fields for MAILBOX IF */
#define _MAILBOX_IF_RESETVALUE           0x00000000UL                                   /**< Default value for MAILBOX_IF                */
#define _MAILBOX_IF_MASK                 0x0000000FUL                                   /**< Mask for MAILBOX_IF                         */
#define MAILBOX_IF_MBOXIF0               (0x1UL << 0)                                   /**< Mailbox Interupt Flag                       */
#define _MAILBOX_IF_MBOXIF0_SHIFT        0                                              /**< Shift value for MAILBOX_MBOXIF0             */
#define _MAILBOX_IF_MBOXIF0_MASK         0x1UL                                          /**< Bit mask for MAILBOX_MBOXIF0                */
#define _MAILBOX_IF_MBOXIF0_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IF                 */
#define MAILBOX_IF_MBOXIF0_DEFAULT       (_MAILBOX_IF_MBOXIF0_DEFAULT << 0)             /**< Shifted mode DEFAULT for MAILBOX_IF         */
#define MAILBOX_IF_MBOXIF1               (0x1UL << 1)                                   /**< Mailbox Interupt Flag                       */
#define _MAILBOX_IF_MBOXIF1_SHIFT        1                                              /**< Shift value for MAILBOX_MBOXIF1             */
#define _MAILBOX_IF_MBOXIF1_MASK         0x2UL                                          /**< Bit mask for MAILBOX_MBOXIF1                */
#define _MAILBOX_IF_MBOXIF1_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IF                 */
#define MAILBOX_IF_MBOXIF1_DEFAULT       (_MAILBOX_IF_MBOXIF1_DEFAULT << 1)             /**< Shifted mode DEFAULT for MAILBOX_IF         */
#define MAILBOX_IF_MBOXIF2               (0x1UL << 2)                                   /**< Mailbox Interupt Flag                       */
#define _MAILBOX_IF_MBOXIF2_SHIFT        2                                              /**< Shift value for MAILBOX_MBOXIF2             */
#define _MAILBOX_IF_MBOXIF2_MASK         0x4UL                                          /**< Bit mask for MAILBOX_MBOXIF2                */
#define _MAILBOX_IF_MBOXIF2_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IF                 */
#define MAILBOX_IF_MBOXIF2_DEFAULT       (_MAILBOX_IF_MBOXIF2_DEFAULT << 2)             /**< Shifted mode DEFAULT for MAILBOX_IF         */
#define MAILBOX_IF_MBOXIF3               (0x1UL << 3)                                   /**< Mailbox Interupt Flag                       */
#define _MAILBOX_IF_MBOXIF3_SHIFT        3                                              /**< Shift value for MAILBOX_MBOXIF3             */
#define _MAILBOX_IF_MBOXIF3_MASK         0x8UL                                          /**< Bit mask for MAILBOX_MBOXIF3                */
#define _MAILBOX_IF_MBOXIF3_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IF                 */
#define MAILBOX_IF_MBOXIF3_DEFAULT       (_MAILBOX_IF_MBOXIF3_DEFAULT << 3)             /**< Shifted mode DEFAULT for MAILBOX_IF         */

/* Bit fields for MAILBOX IEN */
#define _MAILBOX_IEN_RESETVALUE          0x00000000UL                                   /**< Default value for MAILBOX_IEN               */
#define _MAILBOX_IEN_MASK                0x0000000FUL                                   /**< Mask for MAILBOX_IEN                        */
#define MAILBOX_IEN_MBOXIEN0             (0x1UL << 0)                                   /**< Mailbox Interrupt Enable                    */
#define _MAILBOX_IEN_MBOXIEN0_SHIFT      0                                              /**< Shift value for MAILBOX_MBOXIEN0            */
#define _MAILBOX_IEN_MBOXIEN0_MASK       0x1UL                                          /**< Bit mask for MAILBOX_MBOXIEN0               */
#define _MAILBOX_IEN_MBOXIEN0_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IEN                */
#define MAILBOX_IEN_MBOXIEN0_DEFAULT     (_MAILBOX_IEN_MBOXIEN0_DEFAULT << 0)           /**< Shifted mode DEFAULT for MAILBOX_IEN        */
#define MAILBOX_IEN_MBOXIEN1             (0x1UL << 1)                                   /**< Mailbox Interrupt Enable                    */
#define _MAILBOX_IEN_MBOXIEN1_SHIFT      1                                              /**< Shift value for MAILBOX_MBOXIEN1            */
#define _MAILBOX_IEN_MBOXIEN1_MASK       0x2UL                                          /**< Bit mask for MAILBOX_MBOXIEN1               */
#define _MAILBOX_IEN_MBOXIEN1_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IEN                */
#define MAILBOX_IEN_MBOXIEN1_DEFAULT     (_MAILBOX_IEN_MBOXIEN1_DEFAULT << 1)           /**< Shifted mode DEFAULT for MAILBOX_IEN        */
#define MAILBOX_IEN_MBOXIEN2             (0x1UL << 2)                                   /**< Mailbox Interrupt Enable                    */
#define _MAILBOX_IEN_MBOXIEN2_SHIFT      2                                              /**< Shift value for MAILBOX_MBOXIEN2            */
#define _MAILBOX_IEN_MBOXIEN2_MASK       0x4UL                                          /**< Bit mask for MAILBOX_MBOXIEN2               */
#define _MAILBOX_IEN_MBOXIEN2_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IEN                */
#define MAILBOX_IEN_MBOXIEN2_DEFAULT     (_MAILBOX_IEN_MBOXIEN2_DEFAULT << 2)           /**< Shifted mode DEFAULT for MAILBOX_IEN        */
#define MAILBOX_IEN_MBOXIEN3             (0x1UL << 3)                                   /**< Mailbox Interrupt Enable                    */
#define _MAILBOX_IEN_MBOXIEN3_SHIFT      3                                              /**< Shift value for MAILBOX_MBOXIEN3            */
#define _MAILBOX_IEN_MBOXIEN3_MASK       0x8UL                                          /**< Bit mask for MAILBOX_MBOXIEN3               */
#define _MAILBOX_IEN_MBOXIEN3_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for MAILBOX_IEN                */
#define MAILBOX_IEN_MBOXIEN3_DEFAULT     (_MAILBOX_IEN_MBOXIEN3_DEFAULT << 3)           /**< Shifted mode DEFAULT for MAILBOX_IEN        */

/** @} End of group EFR32ZG23_MAILBOX_BitFields */
/** @} End of group EFR32ZG23_MAILBOX */
/** @} End of group Parts */

#endif /* EFR32ZG23_MAILBOX_H */
#ifdef __cplusplus
}
#endif

