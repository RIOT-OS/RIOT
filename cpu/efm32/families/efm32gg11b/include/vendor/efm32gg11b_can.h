/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_CAN register and bit field definitions
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
 * @defgroup EFM32GG11B_CAN CAN
 * @{
 * @brief EFM32GG11B_CAN Register Declaration
 ******************************************************************************/
/** CAN Register Declaration */
typedef struct {
  __IOM uint32_t  CTRL;          /**< Control Register  */
  __IOM uint32_t  STATUS;        /**< Status Register  */
  __IM uint32_t   ERRCNT;        /**< Error Count Register  */
  __IOM uint32_t  BITTIMING;     /**< Bit Timing Register  */
  __IM uint32_t   INTID;         /**< Interrupt Identification Register  */
  __IOM uint32_t  TEST;          /**< Test Register  */
  __IOM uint32_t  BRPE;          /**< BRP Extension Register  */
  __IM uint32_t   TRANSREQ;      /**< Transmission Request Register  */
  __IM uint32_t   MESSAGEDATA;   /**< New Data Register  */

  uint32_t        RESERVED0[1U]; /**< Reserved for future use **/
  __IM uint32_t   MESSAGESTATE;  /**< Message Valid Register  */
  __IOM uint32_t  CONFIG;        /**< Configuration Register  */
  __IM uint32_t   IF0IF;         /**< Message Object Interrupt Flag Register  */
  __IOM uint32_t  IF0IFS;        /**< Message Object Interrupt Flag Set Register  */
  __IOM uint32_t  IF0IFC;        /**< Message Object Interrupt Flag Clear Register  */
  __IOM uint32_t  IF0IEN;        /**< Message Object Interrupt Enable Register  */
  __IM uint32_t   IF1IF;         /**< Status Interrupt Flag Register  */
  __IOM uint32_t  IF1IFS;        /**< Message Object Interrupt Flag Set Register  */
  __IOM uint32_t  IF1IFC;        /**< Message Object Interrupt Flag Clear Register  */
  __IOM uint32_t  IF1IEN;        /**< Status Interrupt Enable Register  */
  __IOM uint32_t  ROUTE;         /**< I/O Routing Register  */

  uint32_t        RESERVED1[3U]; /**< Reserved registers */
  CAN_MIR_TypeDef MIR[2U];       /**< Interface Registers */
} CAN_TypeDef;                   /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_CAN
 * @{
 * @defgroup EFM32GG11B_CAN_BitFields  CAN Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for CAN CTRL */
#define _CAN_CTRL_RESETVALUE                     0x00000001UL                  /**< Default value for CAN_CTRL */
#define _CAN_CTRL_MASK                           0x000000EFUL                  /**< Mask for CAN_CTRL */
#define CAN_CTRL_INIT                            (0x1UL << 0)                  /**< Initialize */
#define _CAN_CTRL_INIT_SHIFT                     0                             /**< Shift value for CAN_INIT */
#define _CAN_CTRL_INIT_MASK                      0x1UL                         /**< Bit mask for CAN_INIT */
#define _CAN_CTRL_INIT_DEFAULT                   0x00000001UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_INIT_DEFAULT                    (_CAN_CTRL_INIT_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_IE                              (0x1UL << 1)                  /**< Module Interrupt Enable */
#define _CAN_CTRL_IE_SHIFT                       1                             /**< Shift value for CAN_IE */
#define _CAN_CTRL_IE_MASK                        0x2UL                         /**< Bit mask for CAN_IE */
#define _CAN_CTRL_IE_DEFAULT                     0x00000000UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_IE_DEFAULT                      (_CAN_CTRL_IE_DEFAULT << 1)   /**< Shifted mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_SIE                             (0x1UL << 2)                  /**< Status Change Interrupt Enable */
#define _CAN_CTRL_SIE_SHIFT                      2                             /**< Shift value for CAN_SIE */
#define _CAN_CTRL_SIE_MASK                       0x4UL                         /**< Bit mask for CAN_SIE */
#define _CAN_CTRL_SIE_DEFAULT                    0x00000000UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_SIE_DEFAULT                     (_CAN_CTRL_SIE_DEFAULT << 2)  /**< Shifted mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_EIE                             (0x1UL << 3)                  /**< Error Interrupt Enable */
#define _CAN_CTRL_EIE_SHIFT                      3                             /**< Shift value for CAN_EIE */
#define _CAN_CTRL_EIE_MASK                       0x8UL                         /**< Bit mask for CAN_EIE */
#define _CAN_CTRL_EIE_DEFAULT                    0x00000000UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_EIE_DEFAULT                     (_CAN_CTRL_EIE_DEFAULT << 3)  /**< Shifted mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_DAR                             (0x1UL << 5)                  /**< Disable Automatic Retransmission */
#define _CAN_CTRL_DAR_SHIFT                      5                             /**< Shift value for CAN_DAR */
#define _CAN_CTRL_DAR_MASK                       0x20UL                        /**< Bit mask for CAN_DAR */
#define _CAN_CTRL_DAR_DEFAULT                    0x00000000UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_DAR_DEFAULT                     (_CAN_CTRL_DAR_DEFAULT << 5)  /**< Shifted mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_CCE                             (0x1UL << 6)                  /**< Configuration Change Enable */
#define _CAN_CTRL_CCE_SHIFT                      6                             /**< Shift value for CAN_CCE */
#define _CAN_CTRL_CCE_MASK                       0x40UL                        /**< Bit mask for CAN_CCE */
#define _CAN_CTRL_CCE_DEFAULT                    0x00000000UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_CCE_DEFAULT                     (_CAN_CTRL_CCE_DEFAULT << 6)  /**< Shifted mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_TEST                            (0x1UL << 7)                  /**< Test Mode Enable Write */
#define _CAN_CTRL_TEST_SHIFT                     7                             /**< Shift value for CAN_TEST */
#define _CAN_CTRL_TEST_MASK                      0x80UL                        /**< Bit mask for CAN_TEST */
#define _CAN_CTRL_TEST_DEFAULT                   0x00000000UL                  /**< Mode DEFAULT for CAN_CTRL */
#define CAN_CTRL_TEST_DEFAULT                    (_CAN_CTRL_TEST_DEFAULT << 7) /**< Shifted mode DEFAULT for CAN_CTRL */

/* Bit fields for CAN STATUS */
#define _CAN_STATUS_RESETVALUE                   0x00000000UL                     /**< Default value for CAN_STATUS */
#define _CAN_STATUS_MASK                         0x000000FFUL                     /**< Mask for CAN_STATUS */
#define _CAN_STATUS_LEC_SHIFT                    0                                /**< Shift value for CAN_LEC */
#define _CAN_STATUS_LEC_MASK                     0x7UL                            /**< Bit mask for CAN_LEC */
#define _CAN_STATUS_LEC_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for CAN_STATUS */
#define _CAN_STATUS_LEC_NONE                     0x00000000UL                     /**< Mode NONE for CAN_STATUS */
#define _CAN_STATUS_LEC_STUFF                    0x00000001UL                     /**< Mode STUFF for CAN_STATUS */
#define _CAN_STATUS_LEC_FORM                     0x00000002UL                     /**< Mode FORM for CAN_STATUS */
#define _CAN_STATUS_LEC_ACK                      0x00000003UL                     /**< Mode ACK for CAN_STATUS */
#define _CAN_STATUS_LEC_BIT1                     0x00000004UL                     /**< Mode BIT1 for CAN_STATUS */
#define _CAN_STATUS_LEC_BIT0                     0x00000005UL                     /**< Mode BIT0 for CAN_STATUS */
#define _CAN_STATUS_LEC_CRC                      0x00000006UL                     /**< Mode CRC for CAN_STATUS */
#define _CAN_STATUS_LEC_UNUSED                   0x00000007UL                     /**< Mode UNUSED for CAN_STATUS */
#define CAN_STATUS_LEC_DEFAULT                   (_CAN_STATUS_LEC_DEFAULT << 0)   /**< Shifted mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_LEC_NONE                      (_CAN_STATUS_LEC_NONE << 0)      /**< Shifted mode NONE for CAN_STATUS */
#define CAN_STATUS_LEC_STUFF                     (_CAN_STATUS_LEC_STUFF << 0)     /**< Shifted mode STUFF for CAN_STATUS */
#define CAN_STATUS_LEC_FORM                      (_CAN_STATUS_LEC_FORM << 0)      /**< Shifted mode FORM for CAN_STATUS */
#define CAN_STATUS_LEC_ACK                       (_CAN_STATUS_LEC_ACK << 0)       /**< Shifted mode ACK for CAN_STATUS */
#define CAN_STATUS_LEC_BIT1                      (_CAN_STATUS_LEC_BIT1 << 0)      /**< Shifted mode BIT1 for CAN_STATUS */
#define CAN_STATUS_LEC_BIT0                      (_CAN_STATUS_LEC_BIT0 << 0)      /**< Shifted mode BIT0 for CAN_STATUS */
#define CAN_STATUS_LEC_CRC                       (_CAN_STATUS_LEC_CRC << 0)       /**< Shifted mode CRC for CAN_STATUS */
#define CAN_STATUS_LEC_UNUSED                    (_CAN_STATUS_LEC_UNUSED << 0)    /**< Shifted mode UNUSED for CAN_STATUS */
#define CAN_STATUS_TXOK                          (0x1UL << 3)                     /**< Transmitted a Message Successfully */
#define _CAN_STATUS_TXOK_SHIFT                   3                                /**< Shift value for CAN_TXOK */
#define _CAN_STATUS_TXOK_MASK                    0x8UL                            /**< Bit mask for CAN_TXOK */
#define _CAN_STATUS_TXOK_DEFAULT                 0x00000000UL                     /**< Mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_TXOK_DEFAULT                  (_CAN_STATUS_TXOK_DEFAULT << 3)  /**< Shifted mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_RXOK                          (0x1UL << 4)                     /**< Received a Message Successfully */
#define _CAN_STATUS_RXOK_SHIFT                   4                                /**< Shift value for CAN_RXOK */
#define _CAN_STATUS_RXOK_MASK                    0x10UL                           /**< Bit mask for CAN_RXOK */
#define _CAN_STATUS_RXOK_DEFAULT                 0x00000000UL                     /**< Mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_RXOK_DEFAULT                  (_CAN_STATUS_RXOK_DEFAULT << 4)  /**< Shifted mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_EPASS                         (0x1UL << 5)                     /**< Error Passive */
#define _CAN_STATUS_EPASS_SHIFT                  5                                /**< Shift value for CAN_EPASS */
#define _CAN_STATUS_EPASS_MASK                   0x20UL                           /**< Bit mask for CAN_EPASS */
#define _CAN_STATUS_EPASS_DEFAULT                0x00000000UL                     /**< Mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_EPASS_DEFAULT                 (_CAN_STATUS_EPASS_DEFAULT << 5) /**< Shifted mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_EWARN                         (0x1UL << 6)                     /**< Warning Status */
#define _CAN_STATUS_EWARN_SHIFT                  6                                /**< Shift value for CAN_EWARN */
#define _CAN_STATUS_EWARN_MASK                   0x40UL                           /**< Bit mask for CAN_EWARN */
#define _CAN_STATUS_EWARN_DEFAULT                0x00000000UL                     /**< Mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_EWARN_DEFAULT                 (_CAN_STATUS_EWARN_DEFAULT << 6) /**< Shifted mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_BOFF                          (0x1UL << 7)                     /**< Bus Off Status */
#define _CAN_STATUS_BOFF_SHIFT                   7                                /**< Shift value for CAN_BOFF */
#define _CAN_STATUS_BOFF_MASK                    0x80UL                           /**< Bit mask for CAN_BOFF */
#define _CAN_STATUS_BOFF_DEFAULT                 0x00000000UL                     /**< Mode DEFAULT for CAN_STATUS */
#define CAN_STATUS_BOFF_DEFAULT                  (_CAN_STATUS_BOFF_DEFAULT << 7)  /**< Shifted mode DEFAULT for CAN_STATUS */

/* Bit fields for CAN ERRCNT */
#define _CAN_ERRCNT_RESETVALUE                   0x00000000UL                        /**< Default value for CAN_ERRCNT */
#define _CAN_ERRCNT_MASK                         0x0000FFFFUL                        /**< Mask for CAN_ERRCNT */
#define _CAN_ERRCNT_TEC_SHIFT                    0                                   /**< Shift value for CAN_TEC */
#define _CAN_ERRCNT_TEC_MASK                     0xFFUL                              /**< Bit mask for CAN_TEC */
#define _CAN_ERRCNT_TEC_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for CAN_ERRCNT */
#define CAN_ERRCNT_TEC_DEFAULT                   (_CAN_ERRCNT_TEC_DEFAULT << 0)      /**< Shifted mode DEFAULT for CAN_ERRCNT */
#define _CAN_ERRCNT_REC_SHIFT                    8                                   /**< Shift value for CAN_REC */
#define _CAN_ERRCNT_REC_MASK                     0x7F00UL                            /**< Bit mask for CAN_REC */
#define _CAN_ERRCNT_REC_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for CAN_ERRCNT */
#define CAN_ERRCNT_REC_DEFAULT                   (_CAN_ERRCNT_REC_DEFAULT << 8)      /**< Shifted mode DEFAULT for CAN_ERRCNT */
#define CAN_ERRCNT_RECERRP                       (0x1UL << 15)                       /**< Receive Error Passive */
#define _CAN_ERRCNT_RECERRP_SHIFT                15                                  /**< Shift value for CAN_RECERRP */
#define _CAN_ERRCNT_RECERRP_MASK                 0x8000UL                            /**< Bit mask for CAN_RECERRP */
#define _CAN_ERRCNT_RECERRP_DEFAULT              0x00000000UL                        /**< Mode DEFAULT for CAN_ERRCNT */
#define _CAN_ERRCNT_RECERRP_FALSE                0x00000000UL                        /**< Mode FALSE for CAN_ERRCNT */
#define _CAN_ERRCNT_RECERRP_TRUE                 0x00000001UL                        /**< Mode TRUE for CAN_ERRCNT */
#define CAN_ERRCNT_RECERRP_DEFAULT               (_CAN_ERRCNT_RECERRP_DEFAULT << 15) /**< Shifted mode DEFAULT for CAN_ERRCNT */
#define CAN_ERRCNT_RECERRP_FALSE                 (_CAN_ERRCNT_RECERRP_FALSE << 15)   /**< Shifted mode FALSE for CAN_ERRCNT */
#define CAN_ERRCNT_RECERRP_TRUE                  (_CAN_ERRCNT_RECERRP_TRUE << 15)    /**< Shifted mode TRUE for CAN_ERRCNT */

/* Bit fields for CAN BITTIMING */
#define _CAN_BITTIMING_RESETVALUE                0x00002301UL                         /**< Default value for CAN_BITTIMING */
#define _CAN_BITTIMING_MASK                      0x00007FFFUL                         /**< Mask for CAN_BITTIMING */
#define _CAN_BITTIMING_BRP_SHIFT                 0                                    /**< Shift value for CAN_BRP */
#define _CAN_BITTIMING_BRP_MASK                  0x3FUL                               /**< Bit mask for CAN_BRP */
#define _CAN_BITTIMING_BRP_DEFAULT               0x00000001UL                         /**< Mode DEFAULT for CAN_BITTIMING */
#define CAN_BITTIMING_BRP_DEFAULT                (_CAN_BITTIMING_BRP_DEFAULT << 0)    /**< Shifted mode DEFAULT for CAN_BITTIMING */
#define _CAN_BITTIMING_SJW_SHIFT                 6                                    /**< Shift value for CAN_SJW */
#define _CAN_BITTIMING_SJW_MASK                  0xC0UL                               /**< Bit mask for CAN_SJW */
#define _CAN_BITTIMING_SJW_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for CAN_BITTIMING */
#define CAN_BITTIMING_SJW_DEFAULT                (_CAN_BITTIMING_SJW_DEFAULT << 6)    /**< Shifted mode DEFAULT for CAN_BITTIMING */
#define _CAN_BITTIMING_TSEG1_SHIFT               8                                    /**< Shift value for CAN_TSEG1 */
#define _CAN_BITTIMING_TSEG1_MASK                0xF00UL                              /**< Bit mask for CAN_TSEG1 */
#define _CAN_BITTIMING_TSEG1_DEFAULT             0x00000003UL                         /**< Mode DEFAULT for CAN_BITTIMING */
#define CAN_BITTIMING_TSEG1_DEFAULT              (_CAN_BITTIMING_TSEG1_DEFAULT << 8)  /**< Shifted mode DEFAULT for CAN_BITTIMING */
#define _CAN_BITTIMING_TSEG2_SHIFT               12                                   /**< Shift value for CAN_TSEG2 */
#define _CAN_BITTIMING_TSEG2_MASK                0x7000UL                             /**< Bit mask for CAN_TSEG2 */
#define _CAN_BITTIMING_TSEG2_DEFAULT             0x00000002UL                         /**< Mode DEFAULT for CAN_BITTIMING */
#define CAN_BITTIMING_TSEG2_DEFAULT              (_CAN_BITTIMING_TSEG2_DEFAULT << 12) /**< Shifted mode DEFAULT for CAN_BITTIMING */

/* Bit fields for CAN INTID */
#define _CAN_INTID_RESETVALUE                    0x00000000UL                       /**< Default value for CAN_INTID */
#define _CAN_INTID_MASK                          0x0000803FUL                       /**< Mask for CAN_INTID */
#define _CAN_INTID_INTID_SHIFT                   0                                  /**< Shift value for CAN_INTID */
#define _CAN_INTID_INTID_MASK                    0x3FUL                             /**< Bit mask for CAN_INTID */
#define _CAN_INTID_INTID_DEFAULT                 0x00000000UL                       /**< Mode DEFAULT for CAN_INTID */
#define CAN_INTID_INTID_DEFAULT                  (_CAN_INTID_INTID_DEFAULT << 0)    /**< Shifted mode DEFAULT for CAN_INTID */
#define CAN_INTID_INTSTAT                        (0x1UL << 15)                      /**< Status Interupt */
#define _CAN_INTID_INTSTAT_SHIFT                 15                                 /**< Shift value for CAN_INTSTAT */
#define _CAN_INTID_INTSTAT_MASK                  0x8000UL                           /**< Bit mask for CAN_INTSTAT */
#define _CAN_INTID_INTSTAT_DEFAULT               0x00000000UL                       /**< Mode DEFAULT for CAN_INTID */
#define _CAN_INTID_INTSTAT_FALSE                 0x00000000UL                       /**< Mode FALSE for CAN_INTID */
#define _CAN_INTID_INTSTAT_TRUE                  0x00000001UL                       /**< Mode TRUE for CAN_INTID */
#define CAN_INTID_INTSTAT_DEFAULT                (_CAN_INTID_INTSTAT_DEFAULT << 15) /**< Shifted mode DEFAULT for CAN_INTID */
#define CAN_INTID_INTSTAT_FALSE                  (_CAN_INTID_INTSTAT_FALSE << 15)   /**< Shifted mode FALSE for CAN_INTID */
#define CAN_INTID_INTSTAT_TRUE                   (_CAN_INTID_INTSTAT_TRUE << 15)    /**< Shifted mode TRUE for CAN_INTID */

/* Bit fields for CAN TEST */
#define _CAN_TEST_RESETVALUE                     0x00000000UL                    /**< Default value for CAN_TEST */
#define _CAN_TEST_MASK                           0x000000FCUL                    /**< Mask for CAN_TEST */
#define CAN_TEST_BASIC                           (0x1UL << 2)                    /**< Basic Mode */
#define _CAN_TEST_BASIC_SHIFT                    2                               /**< Shift value for CAN_BASIC */
#define _CAN_TEST_BASIC_MASK                     0x4UL                           /**< Bit mask for CAN_BASIC */
#define _CAN_TEST_BASIC_DEFAULT                  0x00000000UL                    /**< Mode DEFAULT for CAN_TEST */
#define CAN_TEST_BASIC_DEFAULT                   (_CAN_TEST_BASIC_DEFAULT << 2)  /**< Shifted mode DEFAULT for CAN_TEST */
#define CAN_TEST_SILENT                          (0x1UL << 3)                    /**< Silent Mode */
#define _CAN_TEST_SILENT_SHIFT                   3                               /**< Shift value for CAN_SILENT */
#define _CAN_TEST_SILENT_MASK                    0x8UL                           /**< Bit mask for CAN_SILENT */
#define _CAN_TEST_SILENT_DEFAULT                 0x00000000UL                    /**< Mode DEFAULT for CAN_TEST */
#define CAN_TEST_SILENT_DEFAULT                  (_CAN_TEST_SILENT_DEFAULT << 3) /**< Shifted mode DEFAULT for CAN_TEST */
#define CAN_TEST_LBACK                           (0x1UL << 4)                    /**< Loopback Mode */
#define _CAN_TEST_LBACK_SHIFT                    4                               /**< Shift value for CAN_LBACK */
#define _CAN_TEST_LBACK_MASK                     0x10UL                          /**< Bit mask for CAN_LBACK */
#define _CAN_TEST_LBACK_DEFAULT                  0x00000000UL                    /**< Mode DEFAULT for CAN_TEST */
#define CAN_TEST_LBACK_DEFAULT                   (_CAN_TEST_LBACK_DEFAULT << 4)  /**< Shifted mode DEFAULT for CAN_TEST */
#define _CAN_TEST_TX_SHIFT                       5                               /**< Shift value for CAN_TX */
#define _CAN_TEST_TX_MASK                        0x60UL                          /**< Bit mask for CAN_TX */
#define _CAN_TEST_TX_DEFAULT                     0x00000000UL                    /**< Mode DEFAULT for CAN_TEST */
#define _CAN_TEST_TX_CORE                        0x00000000UL                    /**< Mode CORE for CAN_TEST */
#define _CAN_TEST_TX_SAMPT                       0x00000001UL                    /**< Mode SAMPT for CAN_TEST */
#define _CAN_TEST_TX_LOW                         0x00000002UL                    /**< Mode LOW for CAN_TEST */
#define _CAN_TEST_TX_HIGH                        0x00000003UL                    /**< Mode HIGH for CAN_TEST */
#define CAN_TEST_TX_DEFAULT                      (_CAN_TEST_TX_DEFAULT << 5)     /**< Shifted mode DEFAULT for CAN_TEST */
#define CAN_TEST_TX_CORE                         (_CAN_TEST_TX_CORE << 5)        /**< Shifted mode CORE for CAN_TEST */
#define CAN_TEST_TX_SAMPT                        (_CAN_TEST_TX_SAMPT << 5)       /**< Shifted mode SAMPT for CAN_TEST */
#define CAN_TEST_TX_LOW                          (_CAN_TEST_TX_LOW << 5)         /**< Shifted mode LOW for CAN_TEST */
#define CAN_TEST_TX_HIGH                         (_CAN_TEST_TX_HIGH << 5)        /**< Shifted mode HIGH for CAN_TEST */
#define CAN_TEST_RX                              (0x1UL << 7)                    /**< Monitors the Actual Value of CAN_RX Pin */
#define _CAN_TEST_RX_SHIFT                       7                               /**< Shift value for CAN_RX */
#define _CAN_TEST_RX_MASK                        0x80UL                          /**< Bit mask for CAN_RX */
#define _CAN_TEST_RX_DEFAULT                     0x00000000UL                    /**< Mode DEFAULT for CAN_TEST */
#define _CAN_TEST_RX_LOW                         0x00000000UL                    /**< Mode LOW for CAN_TEST */
#define _CAN_TEST_RX_HIGH                        0x00000001UL                    /**< Mode HIGH for CAN_TEST */
#define CAN_TEST_RX_DEFAULT                      (_CAN_TEST_RX_DEFAULT << 7)     /**< Shifted mode DEFAULT for CAN_TEST */
#define CAN_TEST_RX_LOW                          (_CAN_TEST_RX_LOW << 7)         /**< Shifted mode LOW for CAN_TEST */
#define CAN_TEST_RX_HIGH                         (_CAN_TEST_RX_HIGH << 7)        /**< Shifted mode HIGH for CAN_TEST */

/* Bit fields for CAN BRPE */
#define _CAN_BRPE_RESETVALUE                     0x00000000UL                  /**< Default value for CAN_BRPE */
#define _CAN_BRPE_MASK                           0x0000000FUL                  /**< Mask for CAN_BRPE */
#define _CAN_BRPE_BRPE_SHIFT                     0                             /**< Shift value for CAN_BRPE */
#define _CAN_BRPE_BRPE_MASK                      0xFUL                         /**< Bit mask for CAN_BRPE */
#define _CAN_BRPE_BRPE_DEFAULT                   0x00000000UL                  /**< Mode DEFAULT for CAN_BRPE */
#define CAN_BRPE_BRPE_DEFAULT                    (_CAN_BRPE_BRPE_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_BRPE */

/* Bit fields for CAN TRANSREQ */
#define _CAN_TRANSREQ_RESETVALUE                 0x00000000UL                           /**< Default value for CAN_TRANSREQ */
#define _CAN_TRANSREQ_MASK                       0xFFFFFFFFUL                           /**< Mask for CAN_TRANSREQ */
#define _CAN_TRANSREQ_TXRQSTOUT_SHIFT            0                                      /**< Shift value for CAN_TXRQSTOUT */
#define _CAN_TRANSREQ_TXRQSTOUT_MASK             0xFFFFFFFFUL                           /**< Bit mask for CAN_TXRQSTOUT */
#define _CAN_TRANSREQ_TXRQSTOUT_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for CAN_TRANSREQ */
#define _CAN_TRANSREQ_TXRQSTOUT_FALSE            0x00000000UL                           /**< Mode FALSE for CAN_TRANSREQ */
#define _CAN_TRANSREQ_TXRQSTOUT_TRUE             0x00000001UL                           /**< Mode TRUE for CAN_TRANSREQ */
#define CAN_TRANSREQ_TXRQSTOUT_DEFAULT           (_CAN_TRANSREQ_TXRQSTOUT_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_TRANSREQ */
#define CAN_TRANSREQ_TXRQSTOUT_FALSE             (_CAN_TRANSREQ_TXRQSTOUT_FALSE << 0)   /**< Shifted mode FALSE for CAN_TRANSREQ */
#define CAN_TRANSREQ_TXRQSTOUT_TRUE              (_CAN_TRANSREQ_TXRQSTOUT_TRUE << 0)    /**< Shifted mode TRUE for CAN_TRANSREQ */

/* Bit fields for CAN MESSAGEDATA */
#define _CAN_MESSAGEDATA_RESETVALUE              0x00000000UL                          /**< Default value for CAN_MESSAGEDATA */
#define _CAN_MESSAGEDATA_MASK                    0xFFFFFFFFUL                          /**< Mask for CAN_MESSAGEDATA */
#define _CAN_MESSAGEDATA_VALID_SHIFT             0                                     /**< Shift value for CAN_VALID */
#define _CAN_MESSAGEDATA_VALID_MASK              0xFFFFFFFFUL                          /**< Bit mask for CAN_VALID */
#define _CAN_MESSAGEDATA_VALID_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for CAN_MESSAGEDATA */
#define CAN_MESSAGEDATA_VALID_DEFAULT            (_CAN_MESSAGEDATA_VALID_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_MESSAGEDATA */

/* Bit fields for CAN MESSAGESTATE */
#define _CAN_MESSAGESTATE_RESETVALUE             0x00000000UL                           /**< Default value for CAN_MESSAGESTATE */
#define _CAN_MESSAGESTATE_MASK                   0xFFFFFFFFUL                           /**< Mask for CAN_MESSAGESTATE */
#define _CAN_MESSAGESTATE_VALID_SHIFT            0                                      /**< Shift value for CAN_VALID */
#define _CAN_MESSAGESTATE_VALID_MASK             0xFFFFFFFFUL                           /**< Bit mask for CAN_VALID */
#define _CAN_MESSAGESTATE_VALID_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for CAN_MESSAGESTATE */
#define CAN_MESSAGESTATE_VALID_DEFAULT           (_CAN_MESSAGESTATE_VALID_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_MESSAGESTATE */

/* Bit fields for CAN CONFIG */
#define _CAN_CONFIG_RESETVALUE                   0x00000000UL                        /**< Default value for CAN_CONFIG */
#define _CAN_CONFIG_MASK                         0x00008000UL                        /**< Mask for CAN_CONFIG */
#define CAN_CONFIG_DBGHALT                       (0x1UL << 15)                       /**< Debug Halt */
#define _CAN_CONFIG_DBGHALT_SHIFT                15                                  /**< Shift value for CAN_DBGHALT */
#define _CAN_CONFIG_DBGHALT_MASK                 0x8000UL                            /**< Bit mask for CAN_DBGHALT */
#define _CAN_CONFIG_DBGHALT_DEFAULT              0x00000000UL                        /**< Mode DEFAULT for CAN_CONFIG */
#define _CAN_CONFIG_DBGHALT_NORMAL               0x00000000UL                        /**< Mode NORMAL for CAN_CONFIG */
#define _CAN_CONFIG_DBGHALT_STALL                0x00000001UL                        /**< Mode STALL for CAN_CONFIG */
#define CAN_CONFIG_DBGHALT_DEFAULT               (_CAN_CONFIG_DBGHALT_DEFAULT << 15) /**< Shifted mode DEFAULT for CAN_CONFIG */
#define CAN_CONFIG_DBGHALT_NORMAL                (_CAN_CONFIG_DBGHALT_NORMAL << 15)  /**< Shifted mode NORMAL for CAN_CONFIG */
#define CAN_CONFIG_DBGHALT_STALL                 (_CAN_CONFIG_DBGHALT_STALL << 15)   /**< Shifted mode STALL for CAN_CONFIG */

/* Bit fields for CAN IF0IF */
#define _CAN_IF0IF_RESETVALUE                    0x00000000UL                      /**< Default value for CAN_IF0IF */
#define _CAN_IF0IF_MASK                          0xFFFFFFFFUL                      /**< Mask for CAN_IF0IF */
#define _CAN_IF0IF_MESSAGE_SHIFT                 0                                 /**< Shift value for CAN_MESSAGE */
#define _CAN_IF0IF_MESSAGE_MASK                  0xFFFFFFFFUL                      /**< Bit mask for CAN_MESSAGE */
#define _CAN_IF0IF_MESSAGE_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for CAN_IF0IF */
#define CAN_IF0IF_MESSAGE_DEFAULT                (_CAN_IF0IF_MESSAGE_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF0IF */

/* Bit fields for CAN IF0IFS */
#define _CAN_IF0IFS_RESETVALUE                   0x00000000UL                       /**< Default value for CAN_IF0IFS */
#define _CAN_IF0IFS_MASK                         0xFFFFFFFFUL                       /**< Mask for CAN_IF0IFS */
#define _CAN_IF0IFS_MESSAGE_SHIFT                0                                  /**< Shift value for CAN_MESSAGE */
#define _CAN_IF0IFS_MESSAGE_MASK                 0xFFFFFFFFUL                       /**< Bit mask for CAN_MESSAGE */
#define _CAN_IF0IFS_MESSAGE_DEFAULT              0x00000000UL                       /**< Mode DEFAULT for CAN_IF0IFS */
#define CAN_IF0IFS_MESSAGE_DEFAULT               (_CAN_IF0IFS_MESSAGE_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF0IFS */

/* Bit fields for CAN IF0IFC */
#define _CAN_IF0IFC_RESETVALUE                   0x00000000UL                       /**< Default value for CAN_IF0IFC */
#define _CAN_IF0IFC_MASK                         0xFFFFFFFFUL                       /**< Mask for CAN_IF0IFC */
#define _CAN_IF0IFC_MESSAGE_SHIFT                0                                  /**< Shift value for CAN_MESSAGE */
#define _CAN_IF0IFC_MESSAGE_MASK                 0xFFFFFFFFUL                       /**< Bit mask for CAN_MESSAGE */
#define _CAN_IF0IFC_MESSAGE_DEFAULT              0x00000000UL                       /**< Mode DEFAULT for CAN_IF0IFC */
#define CAN_IF0IFC_MESSAGE_DEFAULT               (_CAN_IF0IFC_MESSAGE_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF0IFC */

/* Bit fields for CAN IF0IEN */
#define _CAN_IF0IEN_RESETVALUE                   0xFFFFFFFFUL                       /**< Default value for CAN_IF0IEN */
#define _CAN_IF0IEN_MASK                         0xFFFFFFFFUL                       /**< Mask for CAN_IF0IEN */
#define _CAN_IF0IEN_MESSAGE_SHIFT                0                                  /**< Shift value for CAN_MESSAGE */
#define _CAN_IF0IEN_MESSAGE_MASK                 0xFFFFFFFFUL                       /**< Bit mask for CAN_MESSAGE */
#define _CAN_IF0IEN_MESSAGE_DEFAULT              0xFFFFFFFFUL                       /**< Mode DEFAULT for CAN_IF0IEN */
#define CAN_IF0IEN_MESSAGE_DEFAULT               (_CAN_IF0IEN_MESSAGE_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF0IEN */

/* Bit fields for CAN IF1IF */
#define _CAN_IF1IF_RESETVALUE                    0x00000000UL                     /**< Default value for CAN_IF1IF */
#define _CAN_IF1IF_MASK                          0x00000001UL                     /**< Mask for CAN_IF1IF */
#define CAN_IF1IF_STATUS                         (0x1UL << 0)                     /**< Status Interrupt Flag */
#define _CAN_IF1IF_STATUS_SHIFT                  0                                /**< Shift value for CAN_STATUS */
#define _CAN_IF1IF_STATUS_MASK                   0x1UL                            /**< Bit mask for CAN_STATUS */
#define _CAN_IF1IF_STATUS_DEFAULT                0x00000000UL                     /**< Mode DEFAULT for CAN_IF1IF */
#define CAN_IF1IF_STATUS_DEFAULT                 (_CAN_IF1IF_STATUS_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF1IF */

/* Bit fields for CAN IF1IFS */
#define _CAN_IF1IFS_RESETVALUE                   0x00000000UL                      /**< Default value for CAN_IF1IFS */
#define _CAN_IF1IFS_MASK                         0x00000001UL                      /**< Mask for CAN_IF1IFS */
#define CAN_IF1IFS_STATUS                        (0x1UL << 0)                      /**< Set STATUS Interrupt Flag */
#define _CAN_IF1IFS_STATUS_SHIFT                 0                                 /**< Shift value for CAN_STATUS */
#define _CAN_IF1IFS_STATUS_MASK                  0x1UL                             /**< Bit mask for CAN_STATUS */
#define _CAN_IF1IFS_STATUS_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for CAN_IF1IFS */
#define CAN_IF1IFS_STATUS_DEFAULT                (_CAN_IF1IFS_STATUS_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF1IFS */

/* Bit fields for CAN IF1IFC */
#define _CAN_IF1IFC_RESETVALUE                   0x00000000UL                      /**< Default value for CAN_IF1IFC */
#define _CAN_IF1IFC_MASK                         0x00000001UL                      /**< Mask for CAN_IF1IFC */
#define CAN_IF1IFC_STATUS                        (0x1UL << 0)                      /**< Clear STATUS Interrupt Flag */
#define _CAN_IF1IFC_STATUS_SHIFT                 0                                 /**< Shift value for CAN_STATUS */
#define _CAN_IF1IFC_STATUS_MASK                  0x1UL                             /**< Bit mask for CAN_STATUS */
#define _CAN_IF1IFC_STATUS_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for CAN_IF1IFC */
#define CAN_IF1IFC_STATUS_DEFAULT                (_CAN_IF1IFC_STATUS_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF1IFC */

/* Bit fields for CAN IF1IEN */
#define _CAN_IF1IEN_RESETVALUE                   0x00000001UL                      /**< Default value for CAN_IF1IEN */
#define _CAN_IF1IEN_MASK                         0x00000001UL                      /**< Mask for CAN_IF1IEN */
#define CAN_IF1IEN_STATUS                        (0x1UL << 0)                      /**< STATUS Interrupt Enable */
#define _CAN_IF1IEN_STATUS_SHIFT                 0                                 /**< Shift value for CAN_STATUS */
#define _CAN_IF1IEN_STATUS_MASK                  0x1UL                             /**< Bit mask for CAN_STATUS */
#define _CAN_IF1IEN_STATUS_DEFAULT               0x00000001UL                      /**< Mode DEFAULT for CAN_IF1IEN */
#define CAN_IF1IEN_STATUS_DEFAULT                (_CAN_IF1IEN_STATUS_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_IF1IEN */

/* Bit fields for CAN ROUTE */
#define _CAN_ROUTE_RESETVALUE                    0x00000000UL                    /**< Default value for CAN_ROUTE */
#define _CAN_ROUTE_MASK                          0x0000071DUL                    /**< Mask for CAN_ROUTE */
#define CAN_ROUTE_TXPEN                          (0x1UL << 0)                    /**< TX Pin Enable */
#define _CAN_ROUTE_TXPEN_SHIFT                   0                               /**< Shift value for CAN_TXPEN */
#define _CAN_ROUTE_TXPEN_MASK                    0x1UL                           /**< Bit mask for CAN_TXPEN */
#define _CAN_ROUTE_TXPEN_DEFAULT                 0x00000000UL                    /**< Mode DEFAULT for CAN_ROUTE */
#define CAN_ROUTE_TXPEN_DEFAULT                  (_CAN_ROUTE_TXPEN_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_SHIFT                   2                               /**< Shift value for CAN_RXLOC */
#define _CAN_ROUTE_RXLOC_MASK                    0x1CUL                          /**< Bit mask for CAN_RXLOC */
#define _CAN_ROUTE_RXLOC_LOC0                    0x00000000UL                    /**< Mode LOC0 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_DEFAULT                 0x00000000UL                    /**< Mode DEFAULT for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC1                    0x00000001UL                    /**< Mode LOC1 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC2                    0x00000002UL                    /**< Mode LOC2 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC3                    0x00000003UL                    /**< Mode LOC3 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC4                    0x00000004UL                    /**< Mode LOC4 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC5                    0x00000005UL                    /**< Mode LOC5 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC6                    0x00000006UL                    /**< Mode LOC6 for CAN_ROUTE */
#define _CAN_ROUTE_RXLOC_LOC7                    0x00000007UL                    /**< Mode LOC7 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC0                     (_CAN_ROUTE_RXLOC_LOC0 << 2)    /**< Shifted mode LOC0 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_DEFAULT                  (_CAN_ROUTE_RXLOC_DEFAULT << 2) /**< Shifted mode DEFAULT for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC1                     (_CAN_ROUTE_RXLOC_LOC1 << 2)    /**< Shifted mode LOC1 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC2                     (_CAN_ROUTE_RXLOC_LOC2 << 2)    /**< Shifted mode LOC2 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC3                     (_CAN_ROUTE_RXLOC_LOC3 << 2)    /**< Shifted mode LOC3 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC4                     (_CAN_ROUTE_RXLOC_LOC4 << 2)    /**< Shifted mode LOC4 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC5                     (_CAN_ROUTE_RXLOC_LOC5 << 2)    /**< Shifted mode LOC5 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC6                     (_CAN_ROUTE_RXLOC_LOC6 << 2)    /**< Shifted mode LOC6 for CAN_ROUTE */
#define CAN_ROUTE_RXLOC_LOC7                     (_CAN_ROUTE_RXLOC_LOC7 << 2)    /**< Shifted mode LOC7 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_SHIFT                   8                               /**< Shift value for CAN_TXLOC */
#define _CAN_ROUTE_TXLOC_MASK                    0x700UL                         /**< Bit mask for CAN_TXLOC */
#define _CAN_ROUTE_TXLOC_LOC0                    0x00000000UL                    /**< Mode LOC0 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_DEFAULT                 0x00000000UL                    /**< Mode DEFAULT for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC1                    0x00000001UL                    /**< Mode LOC1 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC2                    0x00000002UL                    /**< Mode LOC2 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC3                    0x00000003UL                    /**< Mode LOC3 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC4                    0x00000004UL                    /**< Mode LOC4 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC5                    0x00000005UL                    /**< Mode LOC5 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC6                    0x00000006UL                    /**< Mode LOC6 for CAN_ROUTE */
#define _CAN_ROUTE_TXLOC_LOC7                    0x00000007UL                    /**< Mode LOC7 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC0                     (_CAN_ROUTE_TXLOC_LOC0 << 8)    /**< Shifted mode LOC0 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_DEFAULT                  (_CAN_ROUTE_TXLOC_DEFAULT << 8) /**< Shifted mode DEFAULT for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC1                     (_CAN_ROUTE_TXLOC_LOC1 << 8)    /**< Shifted mode LOC1 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC2                     (_CAN_ROUTE_TXLOC_LOC2 << 8)    /**< Shifted mode LOC2 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC3                     (_CAN_ROUTE_TXLOC_LOC3 << 8)    /**< Shifted mode LOC3 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC4                     (_CAN_ROUTE_TXLOC_LOC4 << 8)    /**< Shifted mode LOC4 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC5                     (_CAN_ROUTE_TXLOC_LOC5 << 8)    /**< Shifted mode LOC5 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC6                     (_CAN_ROUTE_TXLOC_LOC6 << 8)    /**< Shifted mode LOC6 for CAN_ROUTE */
#define CAN_ROUTE_TXLOC_LOC7                     (_CAN_ROUTE_TXLOC_LOC7 << 8)    /**< Shifted mode LOC7 for CAN_ROUTE */

/* Bit fields for CAN MIR_CMDMASK */
#define _CAN_MIR_CMDMASK_RESETVALUE              0x00000000UL                                 /**< Default value for CAN_MIR_CMDMASK */
#define _CAN_MIR_CMDMASK_MASK                    0x000000FFUL                                 /**< Mask for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_DATAB                    (0x1UL << 0)                                 /**< CC Channel Mode */
#define _CAN_MIR_CMDMASK_DATAB_SHIFT             0                                            /**< Shift value for CAN_DATAB */
#define _CAN_MIR_CMDMASK_DATAB_MASK              0x1UL                                        /**< Bit mask for CAN_DATAB */
#define _CAN_MIR_CMDMASK_DATAB_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_DATAB_DEFAULT            (_CAN_MIR_CMDMASK_DATAB_DEFAULT << 0)        /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_DATAA                    (0x1UL << 1)                                 /**< Access Data Bytes 0-3 */
#define _CAN_MIR_CMDMASK_DATAA_SHIFT             1                                            /**< Shift value for CAN_DATAA */
#define _CAN_MIR_CMDMASK_DATAA_MASK              0x2UL                                        /**< Bit mask for CAN_DATAA */
#define _CAN_MIR_CMDMASK_DATAA_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_DATAA_DEFAULT            (_CAN_MIR_CMDMASK_DATAA_DEFAULT << 1)        /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_TXRQSTNEWDAT             (0x1UL << 2)                                 /**< Transmission Request Bit/ New Data Bit */
#define _CAN_MIR_CMDMASK_TXRQSTNEWDAT_SHIFT      2                                            /**< Shift value for CAN_TXRQSTNEWDAT */
#define _CAN_MIR_CMDMASK_TXRQSTNEWDAT_MASK       0x4UL                                        /**< Bit mask for CAN_TXRQSTNEWDAT */
#define _CAN_MIR_CMDMASK_TXRQSTNEWDAT_DEFAULT    0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_TXRQSTNEWDAT_DEFAULT     (_CAN_MIR_CMDMASK_TXRQSTNEWDAT_DEFAULT << 2) /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_CLRINTPND                (0x1UL << 3)                                 /**< Clear Interrupt Pending Bit */
#define _CAN_MIR_CMDMASK_CLRINTPND_SHIFT         3                                            /**< Shift value for CAN_CLRINTPND */
#define _CAN_MIR_CMDMASK_CLRINTPND_MASK          0x8UL                                        /**< Bit mask for CAN_CLRINTPND */
#define _CAN_MIR_CMDMASK_CLRINTPND_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_CLRINTPND_DEFAULT        (_CAN_MIR_CMDMASK_CLRINTPND_DEFAULT << 3)    /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_CONTROL                  (0x1UL << 4)                                 /**< Access Control Bits */
#define _CAN_MIR_CMDMASK_CONTROL_SHIFT           4                                            /**< Shift value for CAN_CONTROL */
#define _CAN_MIR_CMDMASK_CONTROL_MASK            0x10UL                                       /**< Bit mask for CAN_CONTROL */
#define _CAN_MIR_CMDMASK_CONTROL_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_CONTROL_DEFAULT          (_CAN_MIR_CMDMASK_CONTROL_DEFAULT << 4)      /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_ARBACC                   (0x1UL << 5)                                 /**< Access Arbitration Bits */
#define _CAN_MIR_CMDMASK_ARBACC_SHIFT            5                                            /**< Shift value for CAN_ARBACC */
#define _CAN_MIR_CMDMASK_ARBACC_MASK             0x20UL                                       /**< Bit mask for CAN_ARBACC */
#define _CAN_MIR_CMDMASK_ARBACC_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_ARBACC_DEFAULT           (_CAN_MIR_CMDMASK_ARBACC_DEFAULT << 5)       /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_MASKACC                  (0x1UL << 6)                                 /**< Access Mask Bits */
#define _CAN_MIR_CMDMASK_MASKACC_SHIFT           6                                            /**< Shift value for CAN_MASKACC */
#define _CAN_MIR_CMDMASK_MASKACC_MASK            0x40UL                                       /**< Bit mask for CAN_MASKACC */
#define _CAN_MIR_CMDMASK_MASKACC_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_MASKACC_DEFAULT          (_CAN_MIR_CMDMASK_MASKACC_DEFAULT << 6)      /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_WRRD                     (0x1UL << 7)                                 /**< Write/Read RAM */
#define _CAN_MIR_CMDMASK_WRRD_SHIFT              7                                            /**< Shift value for CAN_WRRD */
#define _CAN_MIR_CMDMASK_WRRD_MASK               0x80UL                                       /**< Bit mask for CAN_WRRD */
#define _CAN_MIR_CMDMASK_WRRD_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for CAN_MIR_CMDMASK */
#define _CAN_MIR_CMDMASK_WRRD_READ               0x00000000UL                                 /**< Mode READ for CAN_MIR_CMDMASK */
#define _CAN_MIR_CMDMASK_WRRD_WRITE              0x00000001UL                                 /**< Mode WRITE for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_WRRD_DEFAULT             (_CAN_MIR_CMDMASK_WRRD_DEFAULT << 7)         /**< Shifted mode DEFAULT for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_WRRD_READ                (_CAN_MIR_CMDMASK_WRRD_READ << 7)            /**< Shifted mode READ for CAN_MIR_CMDMASK */
#define CAN_MIR_CMDMASK_WRRD_WRITE               (_CAN_MIR_CMDMASK_WRRD_WRITE << 7)           /**< Shifted mode WRITE for CAN_MIR_CMDMASK */

/* Bit fields for CAN MIR_MASK */
#define _CAN_MIR_MASK_RESETVALUE                 0xDFFFFFFFUL                       /**< Default value for CAN_MIR_MASK */
#define _CAN_MIR_MASK_MASK                       0xDFFFFFFFUL                       /**< Mask for CAN_MIR_MASK */
#define _CAN_MIR_MASK_MASK_SHIFT                 0                                  /**< Shift value for CAN_MASK */
#define _CAN_MIR_MASK_MASK_MASK                  0x1FFFFFFFUL                       /**< Bit mask for CAN_MASK */
#define _CAN_MIR_MASK_MASK_DEFAULT               0x1FFFFFFFUL                       /**< Mode DEFAULT for CAN_MIR_MASK */
#define CAN_MIR_MASK_MASK_DEFAULT                (_CAN_MIR_MASK_MASK_DEFAULT << 0)  /**< Shifted mode DEFAULT for CAN_MIR_MASK */
#define CAN_MIR_MASK_MDIR                        (0x1UL << 30)                      /**< Mask Message Direction */
#define _CAN_MIR_MASK_MDIR_SHIFT                 30                                 /**< Shift value for CAN_MDIR */
#define _CAN_MIR_MASK_MDIR_MASK                  0x40000000UL                       /**< Bit mask for CAN_MDIR */
#define _CAN_MIR_MASK_MDIR_DEFAULT               0x00000001UL                       /**< Mode DEFAULT for CAN_MIR_MASK */
#define CAN_MIR_MASK_MDIR_DEFAULT                (_CAN_MIR_MASK_MDIR_DEFAULT << 30) /**< Shifted mode DEFAULT for CAN_MIR_MASK */
#define CAN_MIR_MASK_MXTD                        (0x1UL << 31)                      /**< Mask Extended Identifier */
#define _CAN_MIR_MASK_MXTD_SHIFT                 31                                 /**< Shift value for CAN_MXTD */
#define _CAN_MIR_MASK_MXTD_MASK                  0x80000000UL                       /**< Bit mask for CAN_MXTD */
#define _CAN_MIR_MASK_MXTD_DEFAULT               0x00000001UL                       /**< Mode DEFAULT for CAN_MIR_MASK */
#define CAN_MIR_MASK_MXTD_DEFAULT                (_CAN_MIR_MASK_MXTD_DEFAULT << 31) /**< Shifted mode DEFAULT for CAN_MIR_MASK */

/* Bit fields for CAN MIR_ARB */
#define _CAN_MIR_ARB_RESETVALUE                  0x00000000UL                        /**< Default value for CAN_MIR_ARB */
#define _CAN_MIR_ARB_MASK                        0xFFFFFFFFUL                        /**< Mask for CAN_MIR_ARB */
#define _CAN_MIR_ARB_ID_SHIFT                    0                                   /**< Shift value for CAN_ID */
#define _CAN_MIR_ARB_ID_MASK                     0x1FFFFFFFUL                        /**< Bit mask for CAN_ID */
#define _CAN_MIR_ARB_ID_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for CAN_MIR_ARB */
#define CAN_MIR_ARB_ID_DEFAULT                   (_CAN_MIR_ARB_ID_DEFAULT << 0)      /**< Shifted mode DEFAULT for CAN_MIR_ARB */
#define CAN_MIR_ARB_DIR                          (0x1UL << 29)                       /**< Message Direction */
#define _CAN_MIR_ARB_DIR_SHIFT                   29                                  /**< Shift value for CAN_DIR */
#define _CAN_MIR_ARB_DIR_MASK                    0x20000000UL                        /**< Bit mask for CAN_DIR */
#define _CAN_MIR_ARB_DIR_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for CAN_MIR_ARB */
#define _CAN_MIR_ARB_DIR_RX                      0x00000000UL                        /**< Mode RX for CAN_MIR_ARB */
#define _CAN_MIR_ARB_DIR_TX                      0x00000001UL                        /**< Mode TX for CAN_MIR_ARB */
#define CAN_MIR_ARB_DIR_DEFAULT                  (_CAN_MIR_ARB_DIR_DEFAULT << 29)    /**< Shifted mode DEFAULT for CAN_MIR_ARB */
#define CAN_MIR_ARB_DIR_RX                       (_CAN_MIR_ARB_DIR_RX << 29)         /**< Shifted mode RX for CAN_MIR_ARB */
#define CAN_MIR_ARB_DIR_TX                       (_CAN_MIR_ARB_DIR_TX << 29)         /**< Shifted mode TX for CAN_MIR_ARB */
#define CAN_MIR_ARB_XTD                          (0x1UL << 30)                       /**< Extended Identifier */
#define _CAN_MIR_ARB_XTD_SHIFT                   30                                  /**< Shift value for CAN_XTD */
#define _CAN_MIR_ARB_XTD_MASK                    0x40000000UL                        /**< Bit mask for CAN_XTD */
#define _CAN_MIR_ARB_XTD_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for CAN_MIR_ARB */
#define _CAN_MIR_ARB_XTD_STD                     0x00000000UL                        /**< Mode STD for CAN_MIR_ARB */
#define _CAN_MIR_ARB_XTD_EXT                     0x00000001UL                        /**< Mode EXT for CAN_MIR_ARB */
#define CAN_MIR_ARB_XTD_DEFAULT                  (_CAN_MIR_ARB_XTD_DEFAULT << 30)    /**< Shifted mode DEFAULT for CAN_MIR_ARB */
#define CAN_MIR_ARB_XTD_STD                      (_CAN_MIR_ARB_XTD_STD << 30)        /**< Shifted mode STD for CAN_MIR_ARB */
#define CAN_MIR_ARB_XTD_EXT                      (_CAN_MIR_ARB_XTD_EXT << 30)        /**< Shifted mode EXT for CAN_MIR_ARB */
#define CAN_MIR_ARB_MSGVAL                       (0x1UL << 31)                       /**< Message Valid */
#define _CAN_MIR_ARB_MSGVAL_SHIFT                31                                  /**< Shift value for CAN_MSGVAL */
#define _CAN_MIR_ARB_MSGVAL_MASK                 0x80000000UL                        /**< Bit mask for CAN_MSGVAL */
#define _CAN_MIR_ARB_MSGVAL_DEFAULT              0x00000000UL                        /**< Mode DEFAULT for CAN_MIR_ARB */
#define CAN_MIR_ARB_MSGVAL_DEFAULT               (_CAN_MIR_ARB_MSGVAL_DEFAULT << 31) /**< Shifted mode DEFAULT for CAN_MIR_ARB */

/* Bit fields for CAN MIR_CTRL */
#define _CAN_MIR_CTRL_RESETVALUE                 0x00000000UL                            /**< Default value for CAN_MIR_CTRL */
#define _CAN_MIR_CTRL_MASK                       0x0000FF8FUL                            /**< Mask for CAN_MIR_CTRL */
#define _CAN_MIR_CTRL_DLC_SHIFT                  0                                       /**< Shift value for CAN_DLC */
#define _CAN_MIR_CTRL_DLC_MASK                   0xFUL                                   /**< Bit mask for CAN_DLC */
#define _CAN_MIR_CTRL_DLC_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_DLC_DEFAULT                 (_CAN_MIR_CTRL_DLC_DEFAULT << 0)        /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_EOB                         (0x1UL << 7)                            /**< End of Buffer */
#define _CAN_MIR_CTRL_EOB_SHIFT                  7                                       /**< Shift value for CAN_EOB */
#define _CAN_MIR_CTRL_EOB_MASK                   0x80UL                                  /**< Bit mask for CAN_EOB */
#define _CAN_MIR_CTRL_EOB_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_EOB_DEFAULT                 (_CAN_MIR_CTRL_EOB_DEFAULT << 7)        /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_TXRQST                      (0x1UL << 8)                            /**< Transmit Request */
#define _CAN_MIR_CTRL_TXRQST_SHIFT               8                                       /**< Shift value for CAN_TXRQST */
#define _CAN_MIR_CTRL_TXRQST_MASK                0x100UL                                 /**< Bit mask for CAN_TXRQST */
#define _CAN_MIR_CTRL_TXRQST_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_TXRQST_DEFAULT              (_CAN_MIR_CTRL_TXRQST_DEFAULT << 8)     /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_RMTEN                       (0x1UL << 9)                            /**< Remote Enable */
#define _CAN_MIR_CTRL_RMTEN_SHIFT                9                                       /**< Shift value for CAN_RMTEN */
#define _CAN_MIR_CTRL_RMTEN_MASK                 0x200UL                                 /**< Bit mask for CAN_RMTEN */
#define _CAN_MIR_CTRL_RMTEN_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_RMTEN_DEFAULT               (_CAN_MIR_CTRL_RMTEN_DEFAULT << 9)      /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_RXIE                        (0x1UL << 10)                           /**< Receive Interrupt Enable */
#define _CAN_MIR_CTRL_RXIE_SHIFT                 10                                      /**< Shift value for CAN_RXIE */
#define _CAN_MIR_CTRL_RXIE_MASK                  0x400UL                                 /**< Bit mask for CAN_RXIE */
#define _CAN_MIR_CTRL_RXIE_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_RXIE_DEFAULT                (_CAN_MIR_CTRL_RXIE_DEFAULT << 10)      /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_TXIE                        (0x1UL << 11)                           /**< Transmit Interrupt Enable */
#define _CAN_MIR_CTRL_TXIE_SHIFT                 11                                      /**< Shift value for CAN_TXIE */
#define _CAN_MIR_CTRL_TXIE_MASK                  0x800UL                                 /**< Bit mask for CAN_TXIE */
#define _CAN_MIR_CTRL_TXIE_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_TXIE_DEFAULT                (_CAN_MIR_CTRL_TXIE_DEFAULT << 11)      /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_UMASK                       (0x1UL << 12)                           /**< Use Acceptance Mask */
#define _CAN_MIR_CTRL_UMASK_SHIFT                12                                      /**< Shift value for CAN_UMASK */
#define _CAN_MIR_CTRL_UMASK_MASK                 0x1000UL                                /**< Bit mask for CAN_UMASK */
#define _CAN_MIR_CTRL_UMASK_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_UMASK_DEFAULT               (_CAN_MIR_CTRL_UMASK_DEFAULT << 12)     /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_INTPND                      (0x1UL << 13)                           /**< Interrupt Pending */
#define _CAN_MIR_CTRL_INTPND_SHIFT               13                                      /**< Shift value for CAN_INTPND */
#define _CAN_MIR_CTRL_INTPND_MASK                0x2000UL                                /**< Bit mask for CAN_INTPND */
#define _CAN_MIR_CTRL_INTPND_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_INTPND_DEFAULT              (_CAN_MIR_CTRL_INTPND_DEFAULT << 13)    /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_MESSAGEOF                   (0x1UL << 14)                           /**< Message Lost (only Valid for Message Objects With Direction = Receive) */
#define _CAN_MIR_CTRL_MESSAGEOF_SHIFT            14                                      /**< Shift value for CAN_MESSAGEOF */
#define _CAN_MIR_CTRL_MESSAGEOF_MASK             0x4000UL                                /**< Bit mask for CAN_MESSAGEOF */
#define _CAN_MIR_CTRL_MESSAGEOF_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_MESSAGEOF_DEFAULT           (_CAN_MIR_CTRL_MESSAGEOF_DEFAULT << 14) /**< Shifted mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_DATAVALID                   (0x1UL << 15)                           /**< New Data */
#define _CAN_MIR_CTRL_DATAVALID_SHIFT            15                                      /**< Shift value for CAN_DATAVALID */
#define _CAN_MIR_CTRL_DATAVALID_MASK             0x8000UL                                /**< Bit mask for CAN_DATAVALID */
#define _CAN_MIR_CTRL_DATAVALID_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for CAN_MIR_CTRL */
#define CAN_MIR_CTRL_DATAVALID_DEFAULT           (_CAN_MIR_CTRL_DATAVALID_DEFAULT << 15) /**< Shifted mode DEFAULT for CAN_MIR_CTRL */

/* Bit fields for CAN MIR_DATAL */
#define _CAN_MIR_DATAL_RESETVALUE                0x00000000UL                         /**< Default value for CAN_MIR_DATAL */
#define _CAN_MIR_DATAL_MASK                      0xFFFFFFFFUL                         /**< Mask for CAN_MIR_DATAL */
#define _CAN_MIR_DATAL_DATA0_SHIFT               0                                    /**< Shift value for CAN_DATA0 */
#define _CAN_MIR_DATAL_DATA0_MASK                0xFFUL                               /**< Bit mask for CAN_DATA0 */
#define _CAN_MIR_DATAL_DATA0_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAL */
#define CAN_MIR_DATAL_DATA0_DEFAULT              (_CAN_MIR_DATAL_DATA0_DEFAULT << 0)  /**< Shifted mode DEFAULT for CAN_MIR_DATAL */
#define _CAN_MIR_DATAL_DATA1_SHIFT               8                                    /**< Shift value for CAN_DATA1 */
#define _CAN_MIR_DATAL_DATA1_MASK                0xFF00UL                             /**< Bit mask for CAN_DATA1 */
#define _CAN_MIR_DATAL_DATA1_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAL */
#define CAN_MIR_DATAL_DATA1_DEFAULT              (_CAN_MIR_DATAL_DATA1_DEFAULT << 8)  /**< Shifted mode DEFAULT for CAN_MIR_DATAL */
#define _CAN_MIR_DATAL_DATA2_SHIFT               16                                   /**< Shift value for CAN_DATA2 */
#define _CAN_MIR_DATAL_DATA2_MASK                0xFF0000UL                           /**< Bit mask for CAN_DATA2 */
#define _CAN_MIR_DATAL_DATA2_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAL */
#define CAN_MIR_DATAL_DATA2_DEFAULT              (_CAN_MIR_DATAL_DATA2_DEFAULT << 16) /**< Shifted mode DEFAULT for CAN_MIR_DATAL */
#define _CAN_MIR_DATAL_DATA3_SHIFT               24                                   /**< Shift value for CAN_DATA3 */
#define _CAN_MIR_DATAL_DATA3_MASK                0xFF000000UL                         /**< Bit mask for CAN_DATA3 */
#define _CAN_MIR_DATAL_DATA3_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAL */
#define CAN_MIR_DATAL_DATA3_DEFAULT              (_CAN_MIR_DATAL_DATA3_DEFAULT << 24) /**< Shifted mode DEFAULT for CAN_MIR_DATAL */

/* Bit fields for CAN MIR_DATAH */
#define _CAN_MIR_DATAH_RESETVALUE                0x00000000UL                         /**< Default value for CAN_MIR_DATAH */
#define _CAN_MIR_DATAH_MASK                      0xFFFFFFFFUL                         /**< Mask for CAN_MIR_DATAH */
#define _CAN_MIR_DATAH_DATA4_SHIFT               0                                    /**< Shift value for CAN_DATA4 */
#define _CAN_MIR_DATAH_DATA4_MASK                0xFFUL                               /**< Bit mask for CAN_DATA4 */
#define _CAN_MIR_DATAH_DATA4_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAH */
#define CAN_MIR_DATAH_DATA4_DEFAULT              (_CAN_MIR_DATAH_DATA4_DEFAULT << 0)  /**< Shifted mode DEFAULT for CAN_MIR_DATAH */
#define _CAN_MIR_DATAH_DATA5_SHIFT               8                                    /**< Shift value for CAN_DATA5 */
#define _CAN_MIR_DATAH_DATA5_MASK                0xFF00UL                             /**< Bit mask for CAN_DATA5 */
#define _CAN_MIR_DATAH_DATA5_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAH */
#define CAN_MIR_DATAH_DATA5_DEFAULT              (_CAN_MIR_DATAH_DATA5_DEFAULT << 8)  /**< Shifted mode DEFAULT for CAN_MIR_DATAH */
#define _CAN_MIR_DATAH_DATA6_SHIFT               16                                   /**< Shift value for CAN_DATA6 */
#define _CAN_MIR_DATAH_DATA6_MASK                0xFF0000UL                           /**< Bit mask for CAN_DATA6 */
#define _CAN_MIR_DATAH_DATA6_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAH */
#define CAN_MIR_DATAH_DATA6_DEFAULT              (_CAN_MIR_DATAH_DATA6_DEFAULT << 16) /**< Shifted mode DEFAULT for CAN_MIR_DATAH */
#define _CAN_MIR_DATAH_DATA7_SHIFT               24                                   /**< Shift value for CAN_DATA7 */
#define _CAN_MIR_DATAH_DATA7_MASK                0xFF000000UL                         /**< Bit mask for CAN_DATA7 */
#define _CAN_MIR_DATAH_DATA7_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for CAN_MIR_DATAH */
#define CAN_MIR_DATAH_DATA7_DEFAULT              (_CAN_MIR_DATAH_DATA7_DEFAULT << 24) /**< Shifted mode DEFAULT for CAN_MIR_DATAH */

/* Bit fields for CAN MIR_CMDREQ */
#define _CAN_MIR_CMDREQ_RESETVALUE               0x00000001UL                          /**< Default value for CAN_MIR_CMDREQ */
#define _CAN_MIR_CMDREQ_MASK                     0x0000803FUL                          /**< Mask for CAN_MIR_CMDREQ */
#define _CAN_MIR_CMDREQ_MSGNUM_SHIFT             0                                     /**< Shift value for CAN_MSGNUM */
#define _CAN_MIR_CMDREQ_MSGNUM_MASK              0x3FUL                                /**< Bit mask for CAN_MSGNUM */
#define _CAN_MIR_CMDREQ_MSGNUM_DEFAULT           0x00000001UL                          /**< Mode DEFAULT for CAN_MIR_CMDREQ */
#define CAN_MIR_CMDREQ_MSGNUM_DEFAULT            (_CAN_MIR_CMDREQ_MSGNUM_DEFAULT << 0) /**< Shifted mode DEFAULT for CAN_MIR_CMDREQ */
#define CAN_MIR_CMDREQ_BUSY                      (0x1UL << 15)                         /**< Busy Flag */
#define _CAN_MIR_CMDREQ_BUSY_SHIFT               15                                    /**< Shift value for CAN_BUSY */
#define _CAN_MIR_CMDREQ_BUSY_MASK                0x8000UL                              /**< Bit mask for CAN_BUSY */
#define _CAN_MIR_CMDREQ_BUSY_DEFAULT             0x00000000UL                          /**< Mode DEFAULT for CAN_MIR_CMDREQ */
#define _CAN_MIR_CMDREQ_BUSY_FALSE               0x00000000UL                          /**< Mode FALSE for CAN_MIR_CMDREQ */
#define _CAN_MIR_CMDREQ_BUSY_TRUE                0x00000001UL                          /**< Mode TRUE for CAN_MIR_CMDREQ */
#define CAN_MIR_CMDREQ_BUSY_DEFAULT              (_CAN_MIR_CMDREQ_BUSY_DEFAULT << 15)  /**< Shifted mode DEFAULT for CAN_MIR_CMDREQ */
#define CAN_MIR_CMDREQ_BUSY_FALSE                (_CAN_MIR_CMDREQ_BUSY_FALSE << 15)    /**< Shifted mode FALSE for CAN_MIR_CMDREQ */
#define CAN_MIR_CMDREQ_BUSY_TRUE                 (_CAN_MIR_CMDREQ_BUSY_TRUE << 15)     /**< Shifted mode TRUE for CAN_MIR_CMDREQ */

/** @} */
/** @} End of group EFM32GG11B_CAN */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

