/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 SEMAILBOX register and bit field definitions
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

#ifndef EFR32ZG23_SEMAILBOX_H
#define EFR32ZG23_SEMAILBOX_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_SEMAILBOX_HOST SEMAILBOX_HOST
 * @{
 * @brief EFR32ZG23 SEMAILBOX_HOST Register Declaration.
 *****************************************************************************/

/** SEMAILBOX_HOST Register Declaration. */
typedef struct {
  __IOM uint32_t FIFO;                               /**< ESECURE_MAILBOX_FIFO                               */
  uint32_t       RESERVED0[15U];                     /**< Reserved for future use                            */
  __IM uint32_t  TX_STATUS;                          /**< ESECURE_MAILBOX_TXSTAT                             */
  __IM uint32_t  RX_STATUS;                          /**< ESECURE_MAILBOX_RXSTAT                             */
  __IM uint32_t  TX_PROT;                            /**< ESECURE_MAILBOX_TXPROTECT                          */
  __IM uint32_t  RX_PROT;                            /**< ESECURE_MAILBOX_RXPROTECT                          */
  __IOM uint32_t TX_HEADER;                          /**< ESECURE_MAILBOX_TXHEADER                           */
  __IM uint32_t  RX_HEADER;                          /**< ESECURE_MAILBOX_RXHEADER                           */
  __IOM uint32_t CONFIGURATION;                      /**< ESECURE_MAILBOX_CONFIG                             */
} SEMAILBOX_HOST_TypeDef;
/** @} End of group EFR32ZG23_SEMAILBOX_HOST */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_SEMAILBOX_HOST
 * @{
 * @defgroup EFR32ZG23_SEMAILBOX_HOST_BitFields SEMAILBOX_HOST Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SEMAILBOX FIFO */
#define _SEMAILBOX_FIFO_RESETVALUE                  0x00000000UL                        /**< Default value for SEMAILBOX_FIFO            */
#define _SEMAILBOX_FIFO_MASK                        0xFFFFFFFFUL                        /**< Mask for SEMAILBOX_FIFO                     */
#define _SEMAILBOX_FIFO_FIFO_SHIFT                  0                                   /**< Shift value for SEMAILBOX_FIFO              */
#define _SEMAILBOX_FIFO_FIFO_MASK                   0xFFFFFFFFUL                        /**< Bit mask for SEMAILBOX_FIFO                 */
#define _SEMAILBOX_FIFO_FIFO_DEFAULT                0x00000000UL                        /**< Mode DEFAULT for SEMAILBOX_FIFO             */
#define SEMAILBOX_FIFO_FIFO_DEFAULT                 (_SEMAILBOX_FIFO_FIFO_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_FIFO     */

/* Bit fields for SEMAILBOX TX_STATUS */
#define _SEMAILBOX_TX_STATUS_RESETVALUE             0x00000000UL                                 /**< Default value for SEMAILBOX_TX_STATUS       */
#define _SEMAILBOX_TX_STATUS_MASK                   0x00BFFFFFUL                                 /**< Mask for SEMAILBOX_TX_STATUS                */
#define _SEMAILBOX_TX_STATUS_REMBYTES_SHIFT         0                                            /**< Shift value for SEMAILBOX_REMBYTES          */
#define _SEMAILBOX_TX_STATUS_REMBYTES_MASK          0xFFFFUL                                     /**< Bit mask for SEMAILBOX_REMBYTES             */
#define _SEMAILBOX_TX_STATUS_REMBYTES_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_REMBYTES_DEFAULT        (_SEMAILBOX_TX_STATUS_REMBYTES_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define _SEMAILBOX_TX_STATUS_MSGINFO_SHIFT          16                                           /**< Shift value for SEMAILBOX_MSGINFO           */
#define _SEMAILBOX_TX_STATUS_MSGINFO_MASK           0xF0000UL                                    /**< Bit mask for SEMAILBOX_MSGINFO              */
#define _SEMAILBOX_TX_STATUS_MSGINFO_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_MSGINFO_DEFAULT         (_SEMAILBOX_TX_STATUS_MSGINFO_DEFAULT << 16) /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define SEMAILBOX_TX_STATUS_TXINT                   (0x1UL << 20)                                /**< TXINT                                       */
#define _SEMAILBOX_TX_STATUS_TXINT_SHIFT            20                                           /**< Shift value for SEMAILBOX_TXINT             */
#define _SEMAILBOX_TX_STATUS_TXINT_MASK             0x100000UL                                   /**< Bit mask for SEMAILBOX_TXINT                */
#define _SEMAILBOX_TX_STATUS_TXINT_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_TXINT_DEFAULT           (_SEMAILBOX_TX_STATUS_TXINT_DEFAULT << 20)   /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define SEMAILBOX_TX_STATUS_TXFULL                  (0x1UL << 21)                                /**< TXFULL                                      */
#define _SEMAILBOX_TX_STATUS_TXFULL_SHIFT           21                                           /**< Shift value for SEMAILBOX_TXFULL            */
#define _SEMAILBOX_TX_STATUS_TXFULL_MASK            0x200000UL                                   /**< Bit mask for SEMAILBOX_TXFULL               */
#define _SEMAILBOX_TX_STATUS_TXFULL_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_TXFULL_DEFAULT          (_SEMAILBOX_TX_STATUS_TXFULL_DEFAULT << 21)  /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define SEMAILBOX_TX_STATUS_TXERROR                 (0x1UL << 23)                                /**< TXERROR                                     */
#define _SEMAILBOX_TX_STATUS_TXERROR_SHIFT          23                                           /**< Shift value for SEMAILBOX_TXERROR           */
#define _SEMAILBOX_TX_STATUS_TXERROR_MASK           0x800000UL                                   /**< Bit mask for SEMAILBOX_TXERROR              */
#define _SEMAILBOX_TX_STATUS_TXERROR_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_TXERROR_DEFAULT         (_SEMAILBOX_TX_STATUS_TXERROR_DEFAULT << 23) /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/

/* Bit fields for SEMAILBOX RX_STATUS */
#define _SEMAILBOX_RX_STATUS_RESETVALUE             0x00000000UL                                 /**< Default value for SEMAILBOX_RX_STATUS       */
#define _SEMAILBOX_RX_STATUS_MASK                   0x00FFFFFFUL                                 /**< Mask for SEMAILBOX_RX_STATUS                */
#define _SEMAILBOX_RX_STATUS_REMBYTES_SHIFT         0                                            /**< Shift value for SEMAILBOX_REMBYTES          */
#define _SEMAILBOX_RX_STATUS_REMBYTES_MASK          0xFFFFUL                                     /**< Bit mask for SEMAILBOX_REMBYTES             */
#define _SEMAILBOX_RX_STATUS_REMBYTES_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_REMBYTES_DEFAULT        (_SEMAILBOX_RX_STATUS_REMBYTES_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define _SEMAILBOX_RX_STATUS_MSGINFO_SHIFT          16                                           /**< Shift value for SEMAILBOX_MSGINFO           */
#define _SEMAILBOX_RX_STATUS_MSGINFO_MASK           0xF0000UL                                    /**< Bit mask for SEMAILBOX_MSGINFO              */
#define _SEMAILBOX_RX_STATUS_MSGINFO_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_MSGINFO_DEFAULT         (_SEMAILBOX_RX_STATUS_MSGINFO_DEFAULT << 16) /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXINT                   (0x1UL << 20)                                /**< RXINT                                       */
#define _SEMAILBOX_RX_STATUS_RXINT_SHIFT            20                                           /**< Shift value for SEMAILBOX_RXINT             */
#define _SEMAILBOX_RX_STATUS_RXINT_MASK             0x100000UL                                   /**< Bit mask for SEMAILBOX_RXINT                */
#define _SEMAILBOX_RX_STATUS_RXINT_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXINT_DEFAULT           (_SEMAILBOX_RX_STATUS_RXINT_DEFAULT << 20)   /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXEMPTY                 (0x1UL << 21)                                /**< RXEMPTY                                     */
#define _SEMAILBOX_RX_STATUS_RXEMPTY_SHIFT          21                                           /**< Shift value for SEMAILBOX_RXEMPTY           */
#define _SEMAILBOX_RX_STATUS_RXEMPTY_MASK           0x200000UL                                   /**< Bit mask for SEMAILBOX_RXEMPTY              */
#define _SEMAILBOX_RX_STATUS_RXEMPTY_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXEMPTY_DEFAULT         (_SEMAILBOX_RX_STATUS_RXEMPTY_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXHDR                   (0x1UL << 22)                                /**< RXHDR                                       */
#define _SEMAILBOX_RX_STATUS_RXHDR_SHIFT            22                                           /**< Shift value for SEMAILBOX_RXHDR             */
#define _SEMAILBOX_RX_STATUS_RXHDR_MASK             0x400000UL                                   /**< Bit mask for SEMAILBOX_RXHDR                */
#define _SEMAILBOX_RX_STATUS_RXHDR_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXHDR_DEFAULT           (_SEMAILBOX_RX_STATUS_RXHDR_DEFAULT << 22)   /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXERROR                 (0x1UL << 23)                                /**< RXERROR                                     */
#define _SEMAILBOX_RX_STATUS_RXERROR_SHIFT          23                                           /**< Shift value for SEMAILBOX_RXERROR           */
#define _SEMAILBOX_RX_STATUS_RXERROR_MASK           0x800000UL                                   /**< Bit mask for SEMAILBOX_RXERROR              */
#define _SEMAILBOX_RX_STATUS_RXERROR_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXERROR_DEFAULT         (_SEMAILBOX_RX_STATUS_RXERROR_DEFAULT << 23) /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/

/* Bit fields for SEMAILBOX TX_PROT */
#define _SEMAILBOX_TX_PROT_RESETVALUE               0x00000000UL                                   /**< Default value for SEMAILBOX_TX_PROT         */
#define _SEMAILBOX_TX_PROT_MASK                     0xFFE00000UL                                   /**< Mask for SEMAILBOX_TX_PROT                  */
#define SEMAILBOX_TX_PROT_UNPROTECTED               (0x1UL << 21)                                  /**< UNPROTECTED                                 */
#define _SEMAILBOX_TX_PROT_UNPROTECTED_SHIFT        21                                             /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_TX_PROT_UNPROTECTED_MASK         0x200000UL                                     /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_TX_PROT_UNPROTECTED_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_UNPROTECTED_DEFAULT       (_SEMAILBOX_TX_PROT_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */
#define SEMAILBOX_TX_PROT_PRIVILEGED                (0x1UL << 22)                                  /**< PRIVILEGED                                  */
#define _SEMAILBOX_TX_PROT_PRIVILEGED_SHIFT         22                                             /**< Shift value for SEMAILBOX_PRIVILEGED        */
#define _SEMAILBOX_TX_PROT_PRIVILEGED_MASK          0x400000UL                                     /**< Bit mask for SEMAILBOX_PRIVILEGED           */
#define _SEMAILBOX_TX_PROT_PRIVILEGED_DEFAULT       0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_PRIVILEGED_DEFAULT        (_SEMAILBOX_TX_PROT_PRIVILEGED_DEFAULT << 22)  /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */
#define SEMAILBOX_TX_PROT_NONSECURE                 (0x1UL << 23)                                  /**< NONSECURE                                   */
#define _SEMAILBOX_TX_PROT_NONSECURE_SHIFT          23                                             /**< Shift value for SEMAILBOX_NONSECURE         */
#define _SEMAILBOX_TX_PROT_NONSECURE_MASK           0x800000UL                                     /**< Bit mask for SEMAILBOX_NONSECURE            */
#define _SEMAILBOX_TX_PROT_NONSECURE_DEFAULT        0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_NONSECURE_DEFAULT         (_SEMAILBOX_TX_PROT_NONSECURE_DEFAULT << 23)   /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */
#define _SEMAILBOX_TX_PROT_USER_SHIFT               24                                             /**< Shift value for SEMAILBOX_USER              */
#define _SEMAILBOX_TX_PROT_USER_MASK                0xFF000000UL                                   /**< Bit mask for SEMAILBOX_USER                 */
#define _SEMAILBOX_TX_PROT_USER_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_USER_DEFAULT              (_SEMAILBOX_TX_PROT_USER_DEFAULT << 24)        /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */

/* Bit fields for SEMAILBOX RX_PROT */
#define _SEMAILBOX_RX_PROT_RESETVALUE               0x00000000UL                                   /**< Default value for SEMAILBOX_RX_PROT         */
#define _SEMAILBOX_RX_PROT_MASK                     0xFFE00000UL                                   /**< Mask for SEMAILBOX_RX_PROT                  */
#define SEMAILBOX_RX_PROT_UNPROTECTED               (0x1UL << 21)                                  /**< UNPROTECTED                                 */
#define _SEMAILBOX_RX_PROT_UNPROTECTED_SHIFT        21                                             /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_RX_PROT_UNPROTECTED_MASK         0x200000UL                                     /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_RX_PROT_UNPROTECTED_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_UNPROTECTED_DEFAULT       (_SEMAILBOX_RX_PROT_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */
#define SEMAILBOX_RX_PROT_PRIVILEGED                (0x1UL << 22)                                  /**< PRIVILEGED                                  */
#define _SEMAILBOX_RX_PROT_PRIVILEGED_SHIFT         22                                             /**< Shift value for SEMAILBOX_PRIVILEGED        */
#define _SEMAILBOX_RX_PROT_PRIVILEGED_MASK          0x400000UL                                     /**< Bit mask for SEMAILBOX_PRIVILEGED           */
#define _SEMAILBOX_RX_PROT_PRIVILEGED_DEFAULT       0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_PRIVILEGED_DEFAULT        (_SEMAILBOX_RX_PROT_PRIVILEGED_DEFAULT << 22)  /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */
#define SEMAILBOX_RX_PROT_NONSECURE                 (0x1UL << 23)                                  /**< NONSECURE                                   */
#define _SEMAILBOX_RX_PROT_NONSECURE_SHIFT          23                                             /**< Shift value for SEMAILBOX_NONSECURE         */
#define _SEMAILBOX_RX_PROT_NONSECURE_MASK           0x800000UL                                     /**< Bit mask for SEMAILBOX_NONSECURE            */
#define _SEMAILBOX_RX_PROT_NONSECURE_DEFAULT        0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_NONSECURE_DEFAULT         (_SEMAILBOX_RX_PROT_NONSECURE_DEFAULT << 23)   /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */
#define _SEMAILBOX_RX_PROT_USER_SHIFT               24                                             /**< Shift value for SEMAILBOX_USER              */
#define _SEMAILBOX_RX_PROT_USER_MASK                0xFF000000UL                                   /**< Bit mask for SEMAILBOX_USER                 */
#define _SEMAILBOX_RX_PROT_USER_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_USER_DEFAULT              (_SEMAILBOX_RX_PROT_USER_DEFAULT << 24)        /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */

/* Bit fields for SEMAILBOX TX_HEADER */
#define _SEMAILBOX_TX_HEADER_RESETVALUE             0x00000000UL                                 /**< Default value for SEMAILBOX_TX_HEADER       */
#define _SEMAILBOX_TX_HEADER_MASK                   0xFFFFFFFFUL                                 /**< Mask for SEMAILBOX_TX_HEADER                */
#define _SEMAILBOX_TX_HEADER_TXHEADER_SHIFT         0                                            /**< Shift value for SEMAILBOX_TXHEADER          */
#define _SEMAILBOX_TX_HEADER_TXHEADER_MASK          0xFFFFFFFFUL                                 /**< Bit mask for SEMAILBOX_TXHEADER             */
#define _SEMAILBOX_TX_HEADER_TXHEADER_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_HEADER        */
#define SEMAILBOX_TX_HEADER_TXHEADER_DEFAULT        (_SEMAILBOX_TX_HEADER_TXHEADER_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_TX_HEADER*/

/* Bit fields for SEMAILBOX RX_HEADER */
#define _SEMAILBOX_RX_HEADER_RESETVALUE             0x00000000UL                                 /**< Default value for SEMAILBOX_RX_HEADER       */
#define _SEMAILBOX_RX_HEADER_MASK                   0xFFFFFFFFUL                                 /**< Mask for SEMAILBOX_RX_HEADER                */
#define _SEMAILBOX_RX_HEADER_RXHEADER_SHIFT         0                                            /**< Shift value for SEMAILBOX_RXHEADER          */
#define _SEMAILBOX_RX_HEADER_RXHEADER_MASK          0xFFFFFFFFUL                                 /**< Bit mask for SEMAILBOX_RXHEADER             */
#define _SEMAILBOX_RX_HEADER_RXHEADER_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_RX_HEADER        */
#define SEMAILBOX_RX_HEADER_RXHEADER_DEFAULT        (_SEMAILBOX_RX_HEADER_RXHEADER_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_RX_HEADER*/

/* Bit fields for SEMAILBOX CONFIGURATION */
#define _SEMAILBOX_CONFIGURATION_RESETVALUE         0x00000000UL                                    /**< Default value for SEMAILBOX_CONFIGURATION   */
#define _SEMAILBOX_CONFIGURATION_MASK               0x00000003UL                                    /**< Mask for SEMAILBOX_CONFIGURATION            */
#define SEMAILBOX_CONFIGURATION_TXINTEN             (0x1UL << 0)                                    /**< TXINTEN                                     */
#define _SEMAILBOX_CONFIGURATION_TXINTEN_SHIFT      0                                               /**< Shift value for SEMAILBOX_TXINTEN           */
#define _SEMAILBOX_CONFIGURATION_TXINTEN_MASK       0x1UL                                           /**< Bit mask for SEMAILBOX_TXINTEN              */
#define _SEMAILBOX_CONFIGURATION_TXINTEN_DEFAULT    0x00000000UL                                    /**< Mode DEFAULT for SEMAILBOX_CONFIGURATION    */
#define SEMAILBOX_CONFIGURATION_TXINTEN_DEFAULT     (_SEMAILBOX_CONFIGURATION_TXINTEN_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_CONFIGURATION*/
#define SEMAILBOX_CONFIGURATION_RXINTEN             (0x1UL << 1)                                    /**< RXINTEN                                     */
#define _SEMAILBOX_CONFIGURATION_RXINTEN_SHIFT      1                                               /**< Shift value for SEMAILBOX_RXINTEN           */
#define _SEMAILBOX_CONFIGURATION_RXINTEN_MASK       0x2UL                                           /**< Bit mask for SEMAILBOX_RXINTEN              */
#define _SEMAILBOX_CONFIGURATION_RXINTEN_DEFAULT    0x00000000UL                                    /**< Mode DEFAULT for SEMAILBOX_CONFIGURATION    */
#define SEMAILBOX_CONFIGURATION_RXINTEN_DEFAULT     (_SEMAILBOX_CONFIGURATION_RXINTEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SEMAILBOX_CONFIGURATION*/

/** @} End of group EFR32ZG23_SEMAILBOX_HOST_BitFields */
/** @} End of group EFR32ZG23_SEMAILBOX_HOST */
/**************************************************************************//**
 * @defgroup EFR32ZG23_SEMAILBOX_APBSE SEMAILBOX_APBSE
 * @{
 * @brief EFR32ZG23 SEMAILBOX_APBSE Register Declaration.
 *****************************************************************************/

/** SEMAILBOX_APBSE Register Declaration. */
typedef struct {
  __IOM uint32_t SE_ESECURE_MAILBOX_FIFO;            /**< ESECURE_MAILBOX_FIFO                               */
  uint32_t       RESERVED0[15U];                     /**< Reserved for future use                            */
  __IM uint32_t  SE_ESECURE_MAILBOX_TXSTAT;          /**< ESECURE_MAILBOX_TXSTAT                             */
  __IM uint32_t  SE_ESECURE_MAILBOX_RXSTAT;          /**< ESECURE_MAILBOX_RXSTAT                             */
  __IM uint32_t  SE_ESECURE_MAILBOX_TXPROTECT;       /**< ESECURE_MAILBOX_TXPROTECT                          */
  __IM uint32_t  SE_ESECURE_MAILBOX_RXPROTECT;       /**< ESECURE_MAILBOX_RXPROTECT                          */
  __IOM uint32_t SE_ESECURE_MAILBOX_TXHEADER;        /**< ESECURE_MAILBOX_TXHEADER                           */
  __IM uint32_t  SE_ESECURE_MAILBOX_RXHEADER;        /**< ESECURE_MAILBOX_RXHEADER                           */
  __IOM uint32_t SE_ESECURE_MAILBOX_CONFIG;          /**< ESECURE_MAILBOX_CONFIG                             */
} SEMAILBOX_APBSE_TypeDef;
/** @} End of group EFR32ZG23_SEMAILBOX_APBSE */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_SEMAILBOX_APBSE
 * @{
 * @defgroup EFR32ZG23_SEMAILBOX_APBSE_BitFields SEMAILBOX_APBSE Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_FIFO */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_RESETVALUE                  0x00000000UL                                           /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_FIFO*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_MASK                        0xFFFFFFFFUL                                           /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_FIFO  */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_FIFO_SHIFT                  0                                                      /**< Shift value for SEMAILBOX_FIFO              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_FIFO_MASK                   0xFFFFFFFFUL                                           /**< Bit mask for SEMAILBOX_FIFO                 */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_FIFO_DEFAULT                0x00000000UL                                           /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_FIFO*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_FIFO_DEFAULT                 (_SEMAILBOX_SE_ESECURE_MAILBOX_FIFO_FIFO_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_FIFO*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_TXSTAT */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_RESETVALUE                0x00000000UL                                                 /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_MASK                      0x00BFFFFFUL                                                 /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_REMBYTES_SHIFT            0                                                            /**< Shift value for SEMAILBOX_REMBYTES          */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_REMBYTES_MASK             0xFFFFUL                                                     /**< Bit mask for SEMAILBOX_REMBYTES             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_REMBYTES_DEFAULT          0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_REMBYTES_DEFAULT           (_SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_REMBYTES_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_MSGINFO_SHIFT             16                                                           /**< Shift value for SEMAILBOX_MSGINFO           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_MSGINFO_MASK              0xF0000UL                                                    /**< Bit mask for SEMAILBOX_MSGINFO              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_MSGINFO_DEFAULT           0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_MSGINFO_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_MSGINFO_DEFAULT << 16) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXINT                      (0x1UL << 20)                                                /**< TXINT                                       */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXINT_SHIFT               20                                                           /**< Shift value for SEMAILBOX_TXINT             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXINT_MASK                0x100000UL                                                   /**< Bit mask for SEMAILBOX_TXINT                */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXINT_DEFAULT             0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXINT_DEFAULT              (_SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXINT_DEFAULT << 20)   /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXFULL                     (0x1UL << 21)                                                /**< TXFULL                                      */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXFULL_SHIFT              21                                                           /**< Shift value for SEMAILBOX_TXFULL            */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXFULL_MASK               0x200000UL                                                   /**< Bit mask for SEMAILBOX_TXFULL               */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXFULL_DEFAULT            0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXFULL_DEFAULT             (_SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXFULL_DEFAULT << 21)  /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXERROR                    (0x1UL << 23)                                                /**< TXERROR                                     */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXERROR_SHIFT             23                                                           /**< Shift value for SEMAILBOX_TXERROR           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXERROR_MASK              0x800000UL                                                   /**< Bit mask for SEMAILBOX_TXERROR              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXERROR_DEFAULT           0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXERROR_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT_TXERROR_DEFAULT << 23) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXSTAT*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_RXSTAT */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RESETVALUE                0x00000000UL                                                 /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_MASK                      0x00FFFFFFUL                                                 /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_REMBYTES_SHIFT            0                                                            /**< Shift value for SEMAILBOX_REMBYTES          */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_REMBYTES_MASK             0xFFFFUL                                                     /**< Bit mask for SEMAILBOX_REMBYTES             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_REMBYTES_DEFAULT          0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_REMBYTES_DEFAULT           (_SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_REMBYTES_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_MSGINFO_SHIFT             16                                                           /**< Shift value for SEMAILBOX_MSGINFO           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_MSGINFO_MASK              0xF0000UL                                                    /**< Bit mask for SEMAILBOX_MSGINFO              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_MSGINFO_DEFAULT           0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_MSGINFO_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_MSGINFO_DEFAULT << 16) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXINT                      (0x1UL << 20)                                                /**< RXINT                                       */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXINT_SHIFT               20                                                           /**< Shift value for SEMAILBOX_RXINT             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXINT_MASK                0x100000UL                                                   /**< Bit mask for SEMAILBOX_RXINT                */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXINT_DEFAULT             0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXINT_DEFAULT              (_SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXINT_DEFAULT << 20)   /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXEMPTY                    (0x1UL << 21)                                                /**< RXEMPTY                                     */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXEMPTY_SHIFT             21                                                           /**< Shift value for SEMAILBOX_RXEMPTY           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXEMPTY_MASK              0x200000UL                                                   /**< Bit mask for SEMAILBOX_RXEMPTY              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXEMPTY_DEFAULT           0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXEMPTY_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXEMPTY_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXHDR                      (0x1UL << 22)                                                /**< RXHDR                                       */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXHDR_SHIFT               22                                                           /**< Shift value for SEMAILBOX_RXHDR             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXHDR_MASK                0x400000UL                                                   /**< Bit mask for SEMAILBOX_RXHDR                */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXHDR_DEFAULT             0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXHDR_DEFAULT              (_SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXHDR_DEFAULT << 22)   /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXERROR                    (0x1UL << 23)                                                /**< RXERROR                                     */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXERROR_SHIFT             23                                                           /**< Shift value for SEMAILBOX_RXERROR           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXERROR_MASK              0x800000UL                                                   /**< Bit mask for SEMAILBOX_RXERROR              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXERROR_DEFAULT           0x00000000UL                                                 /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXERROR_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT_RXERROR_DEFAULT << 23) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXSTAT*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_TXPROTECT */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_RESETVALUE             0x00000000UL                                                        /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_MASK                   0xFFE00000UL                                                        /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_UNPROTECTED             (0x1UL << 21)                                                       /**< UNPROTECTED                                 */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_UNPROTECTED_SHIFT      21                                                                  /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_UNPROTECTED_MASK       0x200000UL                                                          /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_UNPROTECTED_DEFAULT    0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_UNPROTECTED_DEFAULT     (_SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_PRIVILEGED              (0x1UL << 22)                                                       /**< PRIVILEGED                                  */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_PRIVILEGED_SHIFT       22                                                                  /**< Shift value for SEMAILBOX_PRIVILEGED        */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_PRIVILEGED_MASK        0x400000UL                                                          /**< Bit mask for SEMAILBOX_PRIVILEGED           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_PRIVILEGED_DEFAULT     0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_PRIVILEGED_DEFAULT      (_SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_PRIVILEGED_DEFAULT << 22)  /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_NONSECURE               (0x1UL << 23)                                                       /**< NONSECURE                                   */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_NONSECURE_SHIFT        23                                                                  /**< Shift value for SEMAILBOX_NONSECURE         */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_NONSECURE_MASK         0x800000UL                                                          /**< Bit mask for SEMAILBOX_NONSECURE            */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_NONSECURE_DEFAULT      0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_NONSECURE_DEFAULT       (_SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_NONSECURE_DEFAULT << 23)   /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_USER_SHIFT             24                                                                  /**< Shift value for SEMAILBOX_USER              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_USER_MASK              0xFF000000UL                                                        /**< Bit mask for SEMAILBOX_USER                 */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_USER_DEFAULT           0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_USER_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT_USER_DEFAULT << 24)        /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXPROTECT*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_RXPROTECT */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_RESETVALUE             0x00000000UL                                                        /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_MASK                   0xFFE00000UL                                                        /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_UNPROTECTED             (0x1UL << 21)                                                       /**< UNPROTECTED                                 */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_UNPROTECTED_SHIFT      21                                                                  /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_UNPROTECTED_MASK       0x200000UL                                                          /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_UNPROTECTED_DEFAULT    0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_UNPROTECTED_DEFAULT     (_SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_PRIVILEGED              (0x1UL << 22)                                                       /**< PRIVILEGED                                  */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_PRIVILEGED_SHIFT       22                                                                  /**< Shift value for SEMAILBOX_PRIVILEGED        */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_PRIVILEGED_MASK        0x400000UL                                                          /**< Bit mask for SEMAILBOX_PRIVILEGED           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_PRIVILEGED_DEFAULT     0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_PRIVILEGED_DEFAULT      (_SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_PRIVILEGED_DEFAULT << 22)  /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_NONSECURE               (0x1UL << 23)                                                       /**< NONSECURE                                   */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_NONSECURE_SHIFT        23                                                                  /**< Shift value for SEMAILBOX_NONSECURE         */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_NONSECURE_MASK         0x800000UL                                                          /**< Bit mask for SEMAILBOX_NONSECURE            */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_NONSECURE_DEFAULT      0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_NONSECURE_DEFAULT       (_SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_NONSECURE_DEFAULT << 23)   /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_USER_SHIFT             24                                                                  /**< Shift value for SEMAILBOX_USER              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_USER_MASK              0xFF000000UL                                                        /**< Bit mask for SEMAILBOX_USER                 */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_USER_DEFAULT           0x00000000UL                                                        /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_USER_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT_USER_DEFAULT << 24)        /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXPROTECT*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_TXHEADER */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_RESETVALUE              0x00000000UL                                                   /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_MASK                    0xFFFFFFFFUL                                                   /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_TXHEADER_SHIFT          0                                                              /**< Shift value for SEMAILBOX_TXHEADER          */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_TXHEADER_MASK           0xFFFFFFFFUL                                                   /**< Bit mask for SEMAILBOX_TXHEADER             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_TXHEADER_DEFAULT        0x00000000UL                                                   /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_TXHEADER_DEFAULT         (_SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER_TXHEADER_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_TXHEADER*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_RXHEADER */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_RESETVALUE              0x00000000UL                                                   /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_MASK                    0xFFFFFFFFUL                                                   /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_RXHEADER_SHIFT          0                                                              /**< Shift value for SEMAILBOX_RXHEADER          */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_RXHEADER_MASK           0xFFFFFFFFUL                                                   /**< Bit mask for SEMAILBOX_RXHEADER             */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_RXHEADER_DEFAULT        0x00000000UL                                                   /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_RXHEADER_DEFAULT         (_SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER_RXHEADER_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_RXHEADER*/

/* Bit fields for SEMAILBOX SE_ESECURE_MAILBOX_CONFIG */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RESETVALUE                0x00000000UL                                                /**< Default value for SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG*/
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_MASK                      0x00000003UL                                                /**< Mask for SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_TXINTEN                    (0x1UL << 0)                                                /**< TXINTEN                                     */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_TXINTEN_SHIFT             0                                                           /**< Shift value for SEMAILBOX_TXINTEN           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_TXINTEN_MASK              0x1UL                                                       /**< Bit mask for SEMAILBOX_TXINTEN              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_TXINTEN_DEFAULT           0x00000000UL                                                /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_TXINTEN_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_TXINTEN_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RXINTEN                    (0x1UL << 1)                                                /**< RXINTEN                                     */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RXINTEN_SHIFT             1                                                           /**< Shift value for SEMAILBOX_RXINTEN           */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RXINTEN_MASK              0x2UL                                                       /**< Bit mask for SEMAILBOX_RXINTEN              */
#define _SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RXINTEN_DEFAULT           0x00000000UL                                                /**< Mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG*/
#define SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RXINTEN_DEFAULT            (_SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG_RXINTEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SEMAILBOX_SE_ESECURE_MAILBOX_CONFIG*/

/** @} End of group EFR32ZG23_SEMAILBOX_APBSE_BitFields */
/** @} End of group EFR32ZG23_SEMAILBOX_APBSE */
/** @} End of group Parts */

#endif /* EFR32ZG23_SEMAILBOX_H */
#ifdef __cplusplus
}
#endif

