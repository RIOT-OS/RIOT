/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 I2C register and bit field definitions
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

#ifndef EFR32ZG23_I2C_H
#define EFR32ZG23_I2C_H
#define I2C_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_I2C I2C
 * @{
 * @brief EFR32ZG23 I2C Register Declaration.
 *****************************************************************************/

/** I2C Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP VERSION Register                                */
  __IOM uint32_t EN;                            /**< Enable Register                                    */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IM uint32_t  STATE;                         /**< State Register                                     */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t CLKDIV;                        /**< Clock Division Register                            */
  __IOM uint32_t SADDR;                         /**< Follower Address Register                          */
  __IOM uint32_t SADDRMASK;                     /**< Follower Address Mask Register                     */
  __IM uint32_t  RXDATA;                        /**< Receive Buffer Data Register                       */
  __IM uint32_t  RXDOUBLE;                      /**< Receive Buffer Double Data Register                */
  __IM uint32_t  RXDATAP;                       /**< Receive Buffer Data Peek Register                  */
  __IM uint32_t  RXDOUBLEP;                     /**< Receive Buffer Double Data Peek Register           */
  __IOM uint32_t TXDATA;                        /**< Transmit Buffer Data Register                      */
  __IOM uint32_t TXDOUBLE;                      /**< Transmit Buffer Double Data Register               */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED0[1007U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP VERSION Register                                */
  __IOM uint32_t EN_SET;                        /**< Enable Register                                    */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IM uint32_t  STATE_SET;                     /**< State Register                                     */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t CLKDIV_SET;                    /**< Clock Division Register                            */
  __IOM uint32_t SADDR_SET;                     /**< Follower Address Register                          */
  __IOM uint32_t SADDRMASK_SET;                 /**< Follower Address Mask Register                     */
  __IM uint32_t  RXDATA_SET;                    /**< Receive Buffer Data Register                       */
  __IM uint32_t  RXDOUBLE_SET;                  /**< Receive Buffer Double Data Register                */
  __IM uint32_t  RXDATAP_SET;                   /**< Receive Buffer Data Peek Register                  */
  __IM uint32_t  RXDOUBLEP_SET;                 /**< Receive Buffer Double Data Peek Register           */
  __IOM uint32_t TXDATA_SET;                    /**< Transmit Buffer Data Register                      */
  __IOM uint32_t TXDOUBLE_SET;                  /**< Transmit Buffer Double Data Register               */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED1[1007U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP VERSION Register                                */
  __IOM uint32_t EN_CLR;                        /**< Enable Register                                    */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IM uint32_t  STATE_CLR;                     /**< State Register                                     */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t CLKDIV_CLR;                    /**< Clock Division Register                            */
  __IOM uint32_t SADDR_CLR;                     /**< Follower Address Register                          */
  __IOM uint32_t SADDRMASK_CLR;                 /**< Follower Address Mask Register                     */
  __IM uint32_t  RXDATA_CLR;                    /**< Receive Buffer Data Register                       */
  __IM uint32_t  RXDOUBLE_CLR;                  /**< Receive Buffer Double Data Register                */
  __IM uint32_t  RXDATAP_CLR;                   /**< Receive Buffer Data Peek Register                  */
  __IM uint32_t  RXDOUBLEP_CLR;                 /**< Receive Buffer Double Data Peek Register           */
  __IOM uint32_t TXDATA_CLR;                    /**< Transmit Buffer Data Register                      */
  __IOM uint32_t TXDOUBLE_CLR;                  /**< Transmit Buffer Double Data Register               */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED2[1007U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP VERSION Register                                */
  __IOM uint32_t EN_TGL;                        /**< Enable Register                                    */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IM uint32_t  STATE_TGL;                     /**< State Register                                     */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t CLKDIV_TGL;                    /**< Clock Division Register                            */
  __IOM uint32_t SADDR_TGL;                     /**< Follower Address Register                          */
  __IOM uint32_t SADDRMASK_TGL;                 /**< Follower Address Mask Register                     */
  __IM uint32_t  RXDATA_TGL;                    /**< Receive Buffer Data Register                       */
  __IM uint32_t  RXDOUBLE_TGL;                  /**< Receive Buffer Double Data Register                */
  __IM uint32_t  RXDATAP_TGL;                   /**< Receive Buffer Data Peek Register                  */
  __IM uint32_t  RXDOUBLEP_TGL;                 /**< Receive Buffer Double Data Peek Register           */
  __IOM uint32_t TXDATA_TGL;                    /**< Transmit Buffer Data Register                      */
  __IOM uint32_t TXDOUBLE_TGL;                  /**< Transmit Buffer Double Data Register               */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
} I2C_TypeDef;
/** @} End of group EFR32ZG23_I2C */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_I2C
 * @{
 * @defgroup EFR32ZG23_I2C_BitFields I2C Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for I2C IPVERSION */
#define _I2C_IPVERSION_RESETVALUE           0x00000000UL                                /**< Default value for I2C_IPVERSION             */
#define _I2C_IPVERSION_MASK                 0xFFFFFFFFUL                                /**< Mask for I2C_IPVERSION                      */
#define _I2C_IPVERSION_IPVERSION_SHIFT      0                                           /**< Shift value for I2C_IPVERSION               */
#define _I2C_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                                /**< Bit mask for I2C_IPVERSION                  */
#define _I2C_IPVERSION_IPVERSION_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for I2C_IPVERSION              */
#define I2C_IPVERSION_IPVERSION_DEFAULT     (_I2C_IPVERSION_IPVERSION_DEFAULT << 0)     /**< Shifted mode DEFAULT for I2C_IPVERSION      */

/* Bit fields for I2C EN */
#define _I2C_EN_RESETVALUE                  0x00000000UL                                /**< Default value for I2C_EN                    */
#define _I2C_EN_MASK                        0x00000001UL                                /**< Mask for I2C_EN                             */
#define I2C_EN_EN                           (0x1UL << 0)                                /**< module enable                               */
#define _I2C_EN_EN_SHIFT                    0                                           /**< Shift value for I2C_EN                      */
#define _I2C_EN_EN_MASK                     0x1UL                                       /**< Bit mask for I2C_EN                         */
#define _I2C_EN_EN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for I2C_EN                     */
#define _I2C_EN_EN_DISABLE                  0x00000000UL                                /**< Mode DISABLE for I2C_EN                     */
#define _I2C_EN_EN_ENABLE                   0x00000001UL                                /**< Mode ENABLE for I2C_EN                      */
#define I2C_EN_EN_DEFAULT                   (_I2C_EN_EN_DEFAULT << 0)                   /**< Shifted mode DEFAULT for I2C_EN             */
#define I2C_EN_EN_DISABLE                   (_I2C_EN_EN_DISABLE << 0)                   /**< Shifted mode DISABLE for I2C_EN             */
#define I2C_EN_EN_ENABLE                    (_I2C_EN_EN_ENABLE << 0)                    /**< Shifted mode ENABLE for I2C_EN              */

/* Bit fields for I2C CTRL */
#define _I2C_CTRL_RESETVALUE                0x00000000UL                                /**< Default value for I2C_CTRL                  */
#define _I2C_CTRL_MASK                      0x0037B3FFUL                                /**< Mask for I2C_CTRL                           */
#define I2C_CTRL_CORERST                    (0x1UL << 0)                                /**< Soft Reset the internal state registers     */
#define _I2C_CTRL_CORERST_SHIFT             0                                           /**< Shift value for I2C_CORERST                 */
#define _I2C_CTRL_CORERST_MASK              0x1UL                                       /**< Bit mask for I2C_CORERST                    */
#define _I2C_CTRL_CORERST_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_CORERST_DISABLE           0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_CORERST_ENABLE            0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_CORERST_DEFAULT            (_I2C_CTRL_CORERST_DEFAULT << 0)            /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_CORERST_DISABLE            (_I2C_CTRL_CORERST_DISABLE << 0)            /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_CORERST_ENABLE             (_I2C_CTRL_CORERST_ENABLE << 0)             /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_SLAVE                      (0x1UL << 1)                                /**< Addressable as Follower                     */
#define _I2C_CTRL_SLAVE_SHIFT               1                                           /**< Shift value for I2C_SLAVE                   */
#define _I2C_CTRL_SLAVE_MASK                0x2UL                                       /**< Bit mask for I2C_SLAVE                      */
#define _I2C_CTRL_SLAVE_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_SLAVE_DISABLE             0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_SLAVE_ENABLE              0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_SLAVE_DEFAULT              (_I2C_CTRL_SLAVE_DEFAULT << 1)              /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_SLAVE_DISABLE              (_I2C_CTRL_SLAVE_DISABLE << 1)              /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_SLAVE_ENABLE               (_I2C_CTRL_SLAVE_ENABLE << 1)               /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_AUTOACK                    (0x1UL << 2)                                /**< Automatic Acknowledge                       */
#define _I2C_CTRL_AUTOACK_SHIFT             2                                           /**< Shift value for I2C_AUTOACK                 */
#define _I2C_CTRL_AUTOACK_MASK              0x4UL                                       /**< Bit mask for I2C_AUTOACK                    */
#define _I2C_CTRL_AUTOACK_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_AUTOACK_DISABLE           0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_AUTOACK_ENABLE            0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_AUTOACK_DEFAULT            (_I2C_CTRL_AUTOACK_DEFAULT << 2)            /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_AUTOACK_DISABLE            (_I2C_CTRL_AUTOACK_DISABLE << 2)            /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_AUTOACK_ENABLE             (_I2C_CTRL_AUTOACK_ENABLE << 2)             /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_AUTOSE                     (0x1UL << 3)                                /**< Automatic STOP when Empty                   */
#define _I2C_CTRL_AUTOSE_SHIFT              3                                           /**< Shift value for I2C_AUTOSE                  */
#define _I2C_CTRL_AUTOSE_MASK               0x8UL                                       /**< Bit mask for I2C_AUTOSE                     */
#define _I2C_CTRL_AUTOSE_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_AUTOSE_DISABLE            0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_AUTOSE_ENABLE             0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_AUTOSE_DEFAULT             (_I2C_CTRL_AUTOSE_DEFAULT << 3)             /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_AUTOSE_DISABLE             (_I2C_CTRL_AUTOSE_DISABLE << 3)             /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_AUTOSE_ENABLE              (_I2C_CTRL_AUTOSE_ENABLE << 3)              /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_AUTOSN                     (0x1UL << 4)                                /**< Automatic STOP on NACK                      */
#define _I2C_CTRL_AUTOSN_SHIFT              4                                           /**< Shift value for I2C_AUTOSN                  */
#define _I2C_CTRL_AUTOSN_MASK               0x10UL                                      /**< Bit mask for I2C_AUTOSN                     */
#define _I2C_CTRL_AUTOSN_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_AUTOSN_DISABLE            0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_AUTOSN_ENABLE             0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_AUTOSN_DEFAULT             (_I2C_CTRL_AUTOSN_DEFAULT << 4)             /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_AUTOSN_DISABLE             (_I2C_CTRL_AUTOSN_DISABLE << 4)             /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_AUTOSN_ENABLE              (_I2C_CTRL_AUTOSN_ENABLE << 4)              /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_ARBDIS                     (0x1UL << 5)                                /**< Arbitration Disable                         */
#define _I2C_CTRL_ARBDIS_SHIFT              5                                           /**< Shift value for I2C_ARBDIS                  */
#define _I2C_CTRL_ARBDIS_MASK               0x20UL                                      /**< Bit mask for I2C_ARBDIS                     */
#define _I2C_CTRL_ARBDIS_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_ARBDIS_DISABLE            0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_ARBDIS_ENABLE             0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_ARBDIS_DEFAULT             (_I2C_CTRL_ARBDIS_DEFAULT << 5)             /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_ARBDIS_DISABLE             (_I2C_CTRL_ARBDIS_DISABLE << 5)             /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_ARBDIS_ENABLE              (_I2C_CTRL_ARBDIS_ENABLE << 5)              /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_GCAMEN                     (0x1UL << 6)                                /**< General Call Address Match Enable           */
#define _I2C_CTRL_GCAMEN_SHIFT              6                                           /**< Shift value for I2C_GCAMEN                  */
#define _I2C_CTRL_GCAMEN_MASK               0x40UL                                      /**< Bit mask for I2C_GCAMEN                     */
#define _I2C_CTRL_GCAMEN_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_GCAMEN_DISABLE            0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_GCAMEN_ENABLE             0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_GCAMEN_DEFAULT             (_I2C_CTRL_GCAMEN_DEFAULT << 6)             /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_GCAMEN_DISABLE             (_I2C_CTRL_GCAMEN_DISABLE << 6)             /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_GCAMEN_ENABLE              (_I2C_CTRL_GCAMEN_ENABLE << 6)              /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_TXBIL                      (0x1UL << 7)                                /**< TX Buffer Interrupt Level                   */
#define _I2C_CTRL_TXBIL_SHIFT               7                                           /**< Shift value for I2C_TXBIL                   */
#define _I2C_CTRL_TXBIL_MASK                0x80UL                                      /**< Bit mask for I2C_TXBIL                      */
#define _I2C_CTRL_TXBIL_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_TXBIL_EMPTY               0x00000000UL                                /**< Mode EMPTY for I2C_CTRL                     */
#define _I2C_CTRL_TXBIL_HALF_FULL           0x00000001UL                                /**< Mode HALF_FULL for I2C_CTRL                 */
#define I2C_CTRL_TXBIL_DEFAULT              (_I2C_CTRL_TXBIL_DEFAULT << 7)              /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_TXBIL_EMPTY                (_I2C_CTRL_TXBIL_EMPTY << 7)                /**< Shifted mode EMPTY for I2C_CTRL             */
#define I2C_CTRL_TXBIL_HALF_FULL            (_I2C_CTRL_TXBIL_HALF_FULL << 7)            /**< Shifted mode HALF_FULL for I2C_CTRL         */
#define _I2C_CTRL_CLHR_SHIFT                8                                           /**< Shift value for I2C_CLHR                    */
#define _I2C_CTRL_CLHR_MASK                 0x300UL                                     /**< Bit mask for I2C_CLHR                       */
#define _I2C_CTRL_CLHR_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_CLHR_STANDARD             0x00000000UL                                /**< Mode STANDARD for I2C_CTRL                  */
#define _I2C_CTRL_CLHR_ASYMMETRIC           0x00000001UL                                /**< Mode ASYMMETRIC for I2C_CTRL                */
#define _I2C_CTRL_CLHR_FAST                 0x00000002UL                                /**< Mode FAST for I2C_CTRL                      */
#define I2C_CTRL_CLHR_DEFAULT               (_I2C_CTRL_CLHR_DEFAULT << 8)               /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_CLHR_STANDARD              (_I2C_CTRL_CLHR_STANDARD << 8)              /**< Shifted mode STANDARD for I2C_CTRL          */
#define I2C_CTRL_CLHR_ASYMMETRIC            (_I2C_CTRL_CLHR_ASYMMETRIC << 8)            /**< Shifted mode ASYMMETRIC for I2C_CTRL        */
#define I2C_CTRL_CLHR_FAST                  (_I2C_CTRL_CLHR_FAST << 8)                  /**< Shifted mode FAST for I2C_CTRL              */
#define _I2C_CTRL_BITO_SHIFT                12                                          /**< Shift value for I2C_BITO                    */
#define _I2C_CTRL_BITO_MASK                 0x3000UL                                    /**< Bit mask for I2C_BITO                       */
#define _I2C_CTRL_BITO_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_BITO_OFF                  0x00000000UL                                /**< Mode OFF for I2C_CTRL                       */
#define _I2C_CTRL_BITO_I2C40PCC             0x00000001UL                                /**< Mode I2C40PCC for I2C_CTRL                  */
#define _I2C_CTRL_BITO_I2C80PCC             0x00000002UL                                /**< Mode I2C80PCC for I2C_CTRL                  */
#define _I2C_CTRL_BITO_I2C160PCC            0x00000003UL                                /**< Mode I2C160PCC for I2C_CTRL                 */
#define I2C_CTRL_BITO_DEFAULT               (_I2C_CTRL_BITO_DEFAULT << 12)              /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_BITO_OFF                   (_I2C_CTRL_BITO_OFF << 12)                  /**< Shifted mode OFF for I2C_CTRL               */
#define I2C_CTRL_BITO_I2C40PCC              (_I2C_CTRL_BITO_I2C40PCC << 12)             /**< Shifted mode I2C40PCC for I2C_CTRL          */
#define I2C_CTRL_BITO_I2C80PCC              (_I2C_CTRL_BITO_I2C80PCC << 12)             /**< Shifted mode I2C80PCC for I2C_CTRL          */
#define I2C_CTRL_BITO_I2C160PCC             (_I2C_CTRL_BITO_I2C160PCC << 12)            /**< Shifted mode I2C160PCC for I2C_CTRL         */
#define I2C_CTRL_GIBITO                     (0x1UL << 15)                               /**< Go Idle on Bus Idle Timeout                 */
#define _I2C_CTRL_GIBITO_SHIFT              15                                          /**< Shift value for I2C_GIBITO                  */
#define _I2C_CTRL_GIBITO_MASK               0x8000UL                                    /**< Bit mask for I2C_GIBITO                     */
#define _I2C_CTRL_GIBITO_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_GIBITO_DISABLE            0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_GIBITO_ENABLE             0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_GIBITO_DEFAULT             (_I2C_CTRL_GIBITO_DEFAULT << 15)            /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_GIBITO_DISABLE             (_I2C_CTRL_GIBITO_DISABLE << 15)            /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_GIBITO_ENABLE              (_I2C_CTRL_GIBITO_ENABLE << 15)             /**< Shifted mode ENABLE for I2C_CTRL            */
#define _I2C_CTRL_CLTO_SHIFT                16                                          /**< Shift value for I2C_CLTO                    */
#define _I2C_CTRL_CLTO_MASK                 0x70000UL                                   /**< Bit mask for I2C_CLTO                       */
#define _I2C_CTRL_CLTO_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_CLTO_OFF                  0x00000000UL                                /**< Mode OFF for I2C_CTRL                       */
#define _I2C_CTRL_CLTO_I2C40PCC             0x00000001UL                                /**< Mode I2C40PCC for I2C_CTRL                  */
#define _I2C_CTRL_CLTO_I2C80PCC             0x00000002UL                                /**< Mode I2C80PCC for I2C_CTRL                  */
#define _I2C_CTRL_CLTO_I2C160PCC            0x00000003UL                                /**< Mode I2C160PCC for I2C_CTRL                 */
#define _I2C_CTRL_CLTO_I2C320PCC            0x00000004UL                                /**< Mode I2C320PCC for I2C_CTRL                 */
#define _I2C_CTRL_CLTO_I2C1024PCC           0x00000005UL                                /**< Mode I2C1024PCC for I2C_CTRL                */
#define I2C_CTRL_CLTO_DEFAULT               (_I2C_CTRL_CLTO_DEFAULT << 16)              /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_CLTO_OFF                   (_I2C_CTRL_CLTO_OFF << 16)                  /**< Shifted mode OFF for I2C_CTRL               */
#define I2C_CTRL_CLTO_I2C40PCC              (_I2C_CTRL_CLTO_I2C40PCC << 16)             /**< Shifted mode I2C40PCC for I2C_CTRL          */
#define I2C_CTRL_CLTO_I2C80PCC              (_I2C_CTRL_CLTO_I2C80PCC << 16)             /**< Shifted mode I2C80PCC for I2C_CTRL          */
#define I2C_CTRL_CLTO_I2C160PCC             (_I2C_CTRL_CLTO_I2C160PCC << 16)            /**< Shifted mode I2C160PCC for I2C_CTRL         */
#define I2C_CTRL_CLTO_I2C320PCC             (_I2C_CTRL_CLTO_I2C320PCC << 16)            /**< Shifted mode I2C320PCC for I2C_CTRL         */
#define I2C_CTRL_CLTO_I2C1024PCC            (_I2C_CTRL_CLTO_I2C1024PCC << 16)           /**< Shifted mode I2C1024PCC for I2C_CTRL        */
#define I2C_CTRL_SCLMONEN                   (0x1UL << 20)                               /**< SCL Monitor Enable                          */
#define _I2C_CTRL_SCLMONEN_SHIFT            20                                          /**< Shift value for I2C_SCLMONEN                */
#define _I2C_CTRL_SCLMONEN_MASK             0x100000UL                                  /**< Bit mask for I2C_SCLMONEN                   */
#define _I2C_CTRL_SCLMONEN_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_SCLMONEN_DISABLE          0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_SCLMONEN_ENABLE           0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_SCLMONEN_DEFAULT           (_I2C_CTRL_SCLMONEN_DEFAULT << 20)          /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_SCLMONEN_DISABLE           (_I2C_CTRL_SCLMONEN_DISABLE << 20)          /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_SCLMONEN_ENABLE            (_I2C_CTRL_SCLMONEN_ENABLE << 20)           /**< Shifted mode ENABLE for I2C_CTRL            */
#define I2C_CTRL_SDAMONEN                   (0x1UL << 21)                               /**< SDA Monitor Enable                          */
#define _I2C_CTRL_SDAMONEN_SHIFT            21                                          /**< Shift value for I2C_SDAMONEN                */
#define _I2C_CTRL_SDAMONEN_MASK             0x200000UL                                  /**< Bit mask for I2C_SDAMONEN                   */
#define _I2C_CTRL_SDAMONEN_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_CTRL                   */
#define _I2C_CTRL_SDAMONEN_DISABLE          0x00000000UL                                /**< Mode DISABLE for I2C_CTRL                   */
#define _I2C_CTRL_SDAMONEN_ENABLE           0x00000001UL                                /**< Mode ENABLE for I2C_CTRL                    */
#define I2C_CTRL_SDAMONEN_DEFAULT           (_I2C_CTRL_SDAMONEN_DEFAULT << 21)          /**< Shifted mode DEFAULT for I2C_CTRL           */
#define I2C_CTRL_SDAMONEN_DISABLE           (_I2C_CTRL_SDAMONEN_DISABLE << 21)          /**< Shifted mode DISABLE for I2C_CTRL           */
#define I2C_CTRL_SDAMONEN_ENABLE            (_I2C_CTRL_SDAMONEN_ENABLE << 21)           /**< Shifted mode ENABLE for I2C_CTRL            */

/* Bit fields for I2C CMD */
#define _I2C_CMD_RESETVALUE                 0x00000000UL                                /**< Default value for I2C_CMD                   */
#define _I2C_CMD_MASK                       0x000000FFUL                                /**< Mask for I2C_CMD                            */
#define I2C_CMD_START                       (0x1UL << 0)                                /**< Send start condition                        */
#define _I2C_CMD_START_SHIFT                0                                           /**< Shift value for I2C_START                   */
#define _I2C_CMD_START_MASK                 0x1UL                                       /**< Bit mask for I2C_START                      */
#define _I2C_CMD_START_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_START_DEFAULT               (_I2C_CMD_START_DEFAULT << 0)               /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_STOP                        (0x1UL << 1)                                /**< Send stop condition                         */
#define _I2C_CMD_STOP_SHIFT                 1                                           /**< Shift value for I2C_STOP                    */
#define _I2C_CMD_STOP_MASK                  0x2UL                                       /**< Bit mask for I2C_STOP                       */
#define _I2C_CMD_STOP_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_STOP_DEFAULT                (_I2C_CMD_STOP_DEFAULT << 1)                /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_ACK                         (0x1UL << 2)                                /**< Send ACK                                    */
#define _I2C_CMD_ACK_SHIFT                  2                                           /**< Shift value for I2C_ACK                     */
#define _I2C_CMD_ACK_MASK                   0x4UL                                       /**< Bit mask for I2C_ACK                        */
#define _I2C_CMD_ACK_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_ACK_DEFAULT                 (_I2C_CMD_ACK_DEFAULT << 2)                 /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_NACK                        (0x1UL << 3)                                /**< Send NACK                                   */
#define _I2C_CMD_NACK_SHIFT                 3                                           /**< Shift value for I2C_NACK                    */
#define _I2C_CMD_NACK_MASK                  0x8UL                                       /**< Bit mask for I2C_NACK                       */
#define _I2C_CMD_NACK_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_NACK_DEFAULT                (_I2C_CMD_NACK_DEFAULT << 3)                /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_CONT                        (0x1UL << 4)                                /**< Continue transmission                       */
#define _I2C_CMD_CONT_SHIFT                 4                                           /**< Shift value for I2C_CONT                    */
#define _I2C_CMD_CONT_MASK                  0x10UL                                      /**< Bit mask for I2C_CONT                       */
#define _I2C_CMD_CONT_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_CONT_DEFAULT                (_I2C_CMD_CONT_DEFAULT << 4)                /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_ABORT                       (0x1UL << 5)                                /**< Abort transmission                          */
#define _I2C_CMD_ABORT_SHIFT                5                                           /**< Shift value for I2C_ABORT                   */
#define _I2C_CMD_ABORT_MASK                 0x20UL                                      /**< Bit mask for I2C_ABORT                      */
#define _I2C_CMD_ABORT_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_ABORT_DEFAULT               (_I2C_CMD_ABORT_DEFAULT << 5)               /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_CLEARTX                     (0x1UL << 6)                                /**< Clear TX                                    */
#define _I2C_CMD_CLEARTX_SHIFT              6                                           /**< Shift value for I2C_CLEARTX                 */
#define _I2C_CMD_CLEARTX_MASK               0x40UL                                      /**< Bit mask for I2C_CLEARTX                    */
#define _I2C_CMD_CLEARTX_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_CLEARTX_DEFAULT             (_I2C_CMD_CLEARTX_DEFAULT << 6)             /**< Shifted mode DEFAULT for I2C_CMD            */
#define I2C_CMD_CLEARPC                     (0x1UL << 7)                                /**< Clear Pending Commands                      */
#define _I2C_CMD_CLEARPC_SHIFT              7                                           /**< Shift value for I2C_CLEARPC                 */
#define _I2C_CMD_CLEARPC_MASK               0x80UL                                      /**< Bit mask for I2C_CLEARPC                    */
#define _I2C_CMD_CLEARPC_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_CMD                    */
#define I2C_CMD_CLEARPC_DEFAULT             (_I2C_CMD_CLEARPC_DEFAULT << 7)             /**< Shifted mode DEFAULT for I2C_CMD            */

/* Bit fields for I2C STATE */
#define _I2C_STATE_RESETVALUE               0x00000001UL                                /**< Default value for I2C_STATE                 */
#define _I2C_STATE_MASK                     0x000000FFUL                                /**< Mask for I2C_STATE                          */
#define I2C_STATE_BUSY                      (0x1UL << 0)                                /**< Bus Busy                                    */
#define _I2C_STATE_BUSY_SHIFT               0                                           /**< Shift value for I2C_BUSY                    */
#define _I2C_STATE_BUSY_MASK                0x1UL                                       /**< Bit mask for I2C_BUSY                       */
#define _I2C_STATE_BUSY_DEFAULT             0x00000001UL                                /**< Mode DEFAULT for I2C_STATE                  */
#define I2C_STATE_BUSY_DEFAULT              (_I2C_STATE_BUSY_DEFAULT << 0)              /**< Shifted mode DEFAULT for I2C_STATE          */
#define I2C_STATE_MASTER                    (0x1UL << 1)                                /**< Leader                                      */
#define _I2C_STATE_MASTER_SHIFT             1                                           /**< Shift value for I2C_MASTER                  */
#define _I2C_STATE_MASTER_MASK              0x2UL                                       /**< Bit mask for I2C_MASTER                     */
#define _I2C_STATE_MASTER_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_STATE                  */
#define I2C_STATE_MASTER_DEFAULT            (_I2C_STATE_MASTER_DEFAULT << 1)            /**< Shifted mode DEFAULT for I2C_STATE          */
#define I2C_STATE_TRANSMITTER               (0x1UL << 2)                                /**< Transmitter                                 */
#define _I2C_STATE_TRANSMITTER_SHIFT        2                                           /**< Shift value for I2C_TRANSMITTER             */
#define _I2C_STATE_TRANSMITTER_MASK         0x4UL                                       /**< Bit mask for I2C_TRANSMITTER                */
#define _I2C_STATE_TRANSMITTER_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for I2C_STATE                  */
#define I2C_STATE_TRANSMITTER_DEFAULT       (_I2C_STATE_TRANSMITTER_DEFAULT << 2)       /**< Shifted mode DEFAULT for I2C_STATE          */
#define I2C_STATE_NACKED                    (0x1UL << 3)                                /**< Nack Received                               */
#define _I2C_STATE_NACKED_SHIFT             3                                           /**< Shift value for I2C_NACKED                  */
#define _I2C_STATE_NACKED_MASK              0x8UL                                       /**< Bit mask for I2C_NACKED                     */
#define _I2C_STATE_NACKED_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_STATE                  */
#define I2C_STATE_NACKED_DEFAULT            (_I2C_STATE_NACKED_DEFAULT << 3)            /**< Shifted mode DEFAULT for I2C_STATE          */
#define I2C_STATE_BUSHOLD                   (0x1UL << 4)                                /**< Bus Held                                    */
#define _I2C_STATE_BUSHOLD_SHIFT            4                                           /**< Shift value for I2C_BUSHOLD                 */
#define _I2C_STATE_BUSHOLD_MASK             0x10UL                                      /**< Bit mask for I2C_BUSHOLD                    */
#define _I2C_STATE_BUSHOLD_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_STATE                  */
#define I2C_STATE_BUSHOLD_DEFAULT           (_I2C_STATE_BUSHOLD_DEFAULT << 4)           /**< Shifted mode DEFAULT for I2C_STATE          */
#define _I2C_STATE_STATE_SHIFT              5                                           /**< Shift value for I2C_STATE                   */
#define _I2C_STATE_STATE_MASK               0xE0UL                                      /**< Bit mask for I2C_STATE                      */
#define _I2C_STATE_STATE_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_STATE                  */
#define _I2C_STATE_STATE_IDLE               0x00000000UL                                /**< Mode IDLE for I2C_STATE                     */
#define _I2C_STATE_STATE_WAIT               0x00000001UL                                /**< Mode WAIT for I2C_STATE                     */
#define _I2C_STATE_STATE_START              0x00000002UL                                /**< Mode START for I2C_STATE                    */
#define _I2C_STATE_STATE_ADDR               0x00000003UL                                /**< Mode ADDR for I2C_STATE                     */
#define _I2C_STATE_STATE_ADDRACK            0x00000004UL                                /**< Mode ADDRACK for I2C_STATE                  */
#define _I2C_STATE_STATE_DATA               0x00000005UL                                /**< Mode DATA for I2C_STATE                     */
#define _I2C_STATE_STATE_DATAACK            0x00000006UL                                /**< Mode DATAACK for I2C_STATE                  */
#define I2C_STATE_STATE_DEFAULT             (_I2C_STATE_STATE_DEFAULT << 5)             /**< Shifted mode DEFAULT for I2C_STATE          */
#define I2C_STATE_STATE_IDLE                (_I2C_STATE_STATE_IDLE << 5)                /**< Shifted mode IDLE for I2C_STATE             */
#define I2C_STATE_STATE_WAIT                (_I2C_STATE_STATE_WAIT << 5)                /**< Shifted mode WAIT for I2C_STATE             */
#define I2C_STATE_STATE_START               (_I2C_STATE_STATE_START << 5)               /**< Shifted mode START for I2C_STATE            */
#define I2C_STATE_STATE_ADDR                (_I2C_STATE_STATE_ADDR << 5)                /**< Shifted mode ADDR for I2C_STATE             */
#define I2C_STATE_STATE_ADDRACK             (_I2C_STATE_STATE_ADDRACK << 5)             /**< Shifted mode ADDRACK for I2C_STATE          */
#define I2C_STATE_STATE_DATA                (_I2C_STATE_STATE_DATA << 5)                /**< Shifted mode DATA for I2C_STATE             */
#define I2C_STATE_STATE_DATAACK             (_I2C_STATE_STATE_DATAACK << 5)             /**< Shifted mode DATAACK for I2C_STATE          */

/* Bit fields for I2C STATUS */
#define _I2C_STATUS_RESETVALUE              0x00000080UL                                /**< Default value for I2C_STATUS                */
#define _I2C_STATUS_MASK                    0x00000FFFUL                                /**< Mask for I2C_STATUS                         */
#define I2C_STATUS_PSTART                   (0x1UL << 0)                                /**< Pending START                               */
#define _I2C_STATUS_PSTART_SHIFT            0                                           /**< Shift value for I2C_PSTART                  */
#define _I2C_STATUS_PSTART_MASK             0x1UL                                       /**< Bit mask for I2C_PSTART                     */
#define _I2C_STATUS_PSTART_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_PSTART_DEFAULT           (_I2C_STATUS_PSTART_DEFAULT << 0)           /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_PSTOP                    (0x1UL << 1)                                /**< Pending STOP                                */
#define _I2C_STATUS_PSTOP_SHIFT             1                                           /**< Shift value for I2C_PSTOP                   */
#define _I2C_STATUS_PSTOP_MASK              0x2UL                                       /**< Bit mask for I2C_PSTOP                      */
#define _I2C_STATUS_PSTOP_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_PSTOP_DEFAULT            (_I2C_STATUS_PSTOP_DEFAULT << 1)            /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_PACK                     (0x1UL << 2)                                /**< Pending ACK                                 */
#define _I2C_STATUS_PACK_SHIFT              2                                           /**< Shift value for I2C_PACK                    */
#define _I2C_STATUS_PACK_MASK               0x4UL                                       /**< Bit mask for I2C_PACK                       */
#define _I2C_STATUS_PACK_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_PACK_DEFAULT             (_I2C_STATUS_PACK_DEFAULT << 2)             /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_PNACK                    (0x1UL << 3)                                /**< Pending NACK                                */
#define _I2C_STATUS_PNACK_SHIFT             3                                           /**< Shift value for I2C_PNACK                   */
#define _I2C_STATUS_PNACK_MASK              0x8UL                                       /**< Bit mask for I2C_PNACK                      */
#define _I2C_STATUS_PNACK_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_PNACK_DEFAULT            (_I2C_STATUS_PNACK_DEFAULT << 3)            /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_PCONT                    (0x1UL << 4)                                /**< Pending continue                            */
#define _I2C_STATUS_PCONT_SHIFT             4                                           /**< Shift value for I2C_PCONT                   */
#define _I2C_STATUS_PCONT_MASK              0x10UL                                      /**< Bit mask for I2C_PCONT                      */
#define _I2C_STATUS_PCONT_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_PCONT_DEFAULT            (_I2C_STATUS_PCONT_DEFAULT << 4)            /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_PABORT                   (0x1UL << 5)                                /**< Pending abort                               */
#define _I2C_STATUS_PABORT_SHIFT            5                                           /**< Shift value for I2C_PABORT                  */
#define _I2C_STATUS_PABORT_MASK             0x20UL                                      /**< Bit mask for I2C_PABORT                     */
#define _I2C_STATUS_PABORT_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_PABORT_DEFAULT           (_I2C_STATUS_PABORT_DEFAULT << 5)           /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_TXC                      (0x1UL << 6)                                /**< TX Complete                                 */
#define _I2C_STATUS_TXC_SHIFT               6                                           /**< Shift value for I2C_TXC                     */
#define _I2C_STATUS_TXC_MASK                0x40UL                                      /**< Bit mask for I2C_TXC                        */
#define _I2C_STATUS_TXC_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_TXC_DEFAULT              (_I2C_STATUS_TXC_DEFAULT << 6)              /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_TXBL                     (0x1UL << 7)                                /**< TX Buffer Level                             */
#define _I2C_STATUS_TXBL_SHIFT              7                                           /**< Shift value for I2C_TXBL                    */
#define _I2C_STATUS_TXBL_MASK               0x80UL                                      /**< Bit mask for I2C_TXBL                       */
#define _I2C_STATUS_TXBL_DEFAULT            0x00000001UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_TXBL_DEFAULT             (_I2C_STATUS_TXBL_DEFAULT << 7)             /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_RXDATAV                  (0x1UL << 8)                                /**< RX Data Valid                               */
#define _I2C_STATUS_RXDATAV_SHIFT           8                                           /**< Shift value for I2C_RXDATAV                 */
#define _I2C_STATUS_RXDATAV_MASK            0x100UL                                     /**< Bit mask for I2C_RXDATAV                    */
#define _I2C_STATUS_RXDATAV_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_RXDATAV_DEFAULT          (_I2C_STATUS_RXDATAV_DEFAULT << 8)          /**< Shifted mode DEFAULT for I2C_STATUS         */
#define I2C_STATUS_RXFULL                   (0x1UL << 9)                                /**< RX FIFO Full                                */
#define _I2C_STATUS_RXFULL_SHIFT            9                                           /**< Shift value for I2C_RXFULL                  */
#define _I2C_STATUS_RXFULL_MASK             0x200UL                                     /**< Bit mask for I2C_RXFULL                     */
#define _I2C_STATUS_RXFULL_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_RXFULL_DEFAULT           (_I2C_STATUS_RXFULL_DEFAULT << 9)           /**< Shifted mode DEFAULT for I2C_STATUS         */
#define _I2C_STATUS_TXBUFCNT_SHIFT          10                                          /**< Shift value for I2C_TXBUFCNT                */
#define _I2C_STATUS_TXBUFCNT_MASK           0xC00UL                                     /**< Bit mask for I2C_TXBUFCNT                   */
#define _I2C_STATUS_TXBUFCNT_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for I2C_STATUS                 */
#define I2C_STATUS_TXBUFCNT_DEFAULT         (_I2C_STATUS_TXBUFCNT_DEFAULT << 10)        /**< Shifted mode DEFAULT for I2C_STATUS         */

/* Bit fields for I2C CLKDIV */
#define _I2C_CLKDIV_RESETVALUE              0x00000000UL                                /**< Default value for I2C_CLKDIV                */
#define _I2C_CLKDIV_MASK                    0x000001FFUL                                /**< Mask for I2C_CLKDIV                         */
#define _I2C_CLKDIV_DIV_SHIFT               0                                           /**< Shift value for I2C_DIV                     */
#define _I2C_CLKDIV_DIV_MASK                0x1FFUL                                     /**< Bit mask for I2C_DIV                        */
#define _I2C_CLKDIV_DIV_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_CLKDIV                 */
#define I2C_CLKDIV_DIV_DEFAULT              (_I2C_CLKDIV_DIV_DEFAULT << 0)              /**< Shifted mode DEFAULT for I2C_CLKDIV         */

/* Bit fields for I2C SADDR */
#define _I2C_SADDR_RESETVALUE               0x00000000UL                                /**< Default value for I2C_SADDR                 */
#define _I2C_SADDR_MASK                     0x000000FEUL                                /**< Mask for I2C_SADDR                          */
#define _I2C_SADDR_ADDR_SHIFT               1                                           /**< Shift value for I2C_ADDR                    */
#define _I2C_SADDR_ADDR_MASK                0xFEUL                                      /**< Bit mask for I2C_ADDR                       */
#define _I2C_SADDR_ADDR_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_SADDR                  */
#define I2C_SADDR_ADDR_DEFAULT              (_I2C_SADDR_ADDR_DEFAULT << 1)              /**< Shifted mode DEFAULT for I2C_SADDR          */

/* Bit fields for I2C SADDRMASK */
#define _I2C_SADDRMASK_RESETVALUE           0x00000000UL                                /**< Default value for I2C_SADDRMASK             */
#define _I2C_SADDRMASK_MASK                 0x000000FEUL                                /**< Mask for I2C_SADDRMASK                      */
#define _I2C_SADDRMASK_SADDRMASK_SHIFT      1                                           /**< Shift value for I2C_SADDRMASK               */
#define _I2C_SADDRMASK_SADDRMASK_MASK       0xFEUL                                      /**< Bit mask for I2C_SADDRMASK                  */
#define _I2C_SADDRMASK_SADDRMASK_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for I2C_SADDRMASK              */
#define I2C_SADDRMASK_SADDRMASK_DEFAULT     (_I2C_SADDRMASK_SADDRMASK_DEFAULT << 1)     /**< Shifted mode DEFAULT for I2C_SADDRMASK      */

/* Bit fields for I2C RXDATA */
#define _I2C_RXDATA_RESETVALUE              0x00000000UL                                /**< Default value for I2C_RXDATA                */
#define _I2C_RXDATA_MASK                    0x000000FFUL                                /**< Mask for I2C_RXDATA                         */
#define _I2C_RXDATA_RXDATA_SHIFT            0                                           /**< Shift value for I2C_RXDATA                  */
#define _I2C_RXDATA_RXDATA_MASK             0xFFUL                                      /**< Bit mask for I2C_RXDATA                     */
#define _I2C_RXDATA_RXDATA_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_RXDATA                 */
#define I2C_RXDATA_RXDATA_DEFAULT           (_I2C_RXDATA_RXDATA_DEFAULT << 0)           /**< Shifted mode DEFAULT for I2C_RXDATA         */

/* Bit fields for I2C RXDOUBLE */
#define _I2C_RXDOUBLE_RESETVALUE            0x00000000UL                                /**< Default value for I2C_RXDOUBLE              */
#define _I2C_RXDOUBLE_MASK                  0x0000FFFFUL                                /**< Mask for I2C_RXDOUBLE                       */
#define _I2C_RXDOUBLE_RXDATA0_SHIFT         0                                           /**< Shift value for I2C_RXDATA0                 */
#define _I2C_RXDOUBLE_RXDATA0_MASK          0xFFUL                                      /**< Bit mask for I2C_RXDATA0                    */
#define _I2C_RXDOUBLE_RXDATA0_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for I2C_RXDOUBLE               */
#define I2C_RXDOUBLE_RXDATA0_DEFAULT        (_I2C_RXDOUBLE_RXDATA0_DEFAULT << 0)        /**< Shifted mode DEFAULT for I2C_RXDOUBLE       */
#define _I2C_RXDOUBLE_RXDATA1_SHIFT         8                                           /**< Shift value for I2C_RXDATA1                 */
#define _I2C_RXDOUBLE_RXDATA1_MASK          0xFF00UL                                    /**< Bit mask for I2C_RXDATA1                    */
#define _I2C_RXDOUBLE_RXDATA1_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for I2C_RXDOUBLE               */
#define I2C_RXDOUBLE_RXDATA1_DEFAULT        (_I2C_RXDOUBLE_RXDATA1_DEFAULT << 8)        /**< Shifted mode DEFAULT for I2C_RXDOUBLE       */

/* Bit fields for I2C RXDATAP */
#define _I2C_RXDATAP_RESETVALUE             0x00000000UL                                /**< Default value for I2C_RXDATAP               */
#define _I2C_RXDATAP_MASK                   0x000000FFUL                                /**< Mask for I2C_RXDATAP                        */
#define _I2C_RXDATAP_RXDATAP_SHIFT          0                                           /**< Shift value for I2C_RXDATAP                 */
#define _I2C_RXDATAP_RXDATAP_MASK           0xFFUL                                      /**< Bit mask for I2C_RXDATAP                    */
#define _I2C_RXDATAP_RXDATAP_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for I2C_RXDATAP                */
#define I2C_RXDATAP_RXDATAP_DEFAULT         (_I2C_RXDATAP_RXDATAP_DEFAULT << 0)         /**< Shifted mode DEFAULT for I2C_RXDATAP        */

/* Bit fields for I2C RXDOUBLEP */
#define _I2C_RXDOUBLEP_RESETVALUE           0x00000000UL                                /**< Default value for I2C_RXDOUBLEP             */
#define _I2C_RXDOUBLEP_MASK                 0x0000FFFFUL                                /**< Mask for I2C_RXDOUBLEP                      */
#define _I2C_RXDOUBLEP_RXDATAP0_SHIFT       0                                           /**< Shift value for I2C_RXDATAP0                */
#define _I2C_RXDOUBLEP_RXDATAP0_MASK        0xFFUL                                      /**< Bit mask for I2C_RXDATAP0                   */
#define _I2C_RXDOUBLEP_RXDATAP0_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for I2C_RXDOUBLEP              */
#define I2C_RXDOUBLEP_RXDATAP0_DEFAULT      (_I2C_RXDOUBLEP_RXDATAP0_DEFAULT << 0)      /**< Shifted mode DEFAULT for I2C_RXDOUBLEP      */
#define _I2C_RXDOUBLEP_RXDATAP1_SHIFT       8                                           /**< Shift value for I2C_RXDATAP1                */
#define _I2C_RXDOUBLEP_RXDATAP1_MASK        0xFF00UL                                    /**< Bit mask for I2C_RXDATAP1                   */
#define _I2C_RXDOUBLEP_RXDATAP1_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for I2C_RXDOUBLEP              */
#define I2C_RXDOUBLEP_RXDATAP1_DEFAULT      (_I2C_RXDOUBLEP_RXDATAP1_DEFAULT << 8)      /**< Shifted mode DEFAULT for I2C_RXDOUBLEP      */

/* Bit fields for I2C TXDATA */
#define _I2C_TXDATA_RESETVALUE              0x00000000UL                                /**< Default value for I2C_TXDATA                */
#define _I2C_TXDATA_MASK                    0x000000FFUL                                /**< Mask for I2C_TXDATA                         */
#define _I2C_TXDATA_TXDATA_SHIFT            0                                           /**< Shift value for I2C_TXDATA                  */
#define _I2C_TXDATA_TXDATA_MASK             0xFFUL                                      /**< Bit mask for I2C_TXDATA                     */
#define _I2C_TXDATA_TXDATA_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for I2C_TXDATA                 */
#define I2C_TXDATA_TXDATA_DEFAULT           (_I2C_TXDATA_TXDATA_DEFAULT << 0)           /**< Shifted mode DEFAULT for I2C_TXDATA         */

/* Bit fields for I2C TXDOUBLE */
#define _I2C_TXDOUBLE_RESETVALUE            0x00000000UL                                /**< Default value for I2C_TXDOUBLE              */
#define _I2C_TXDOUBLE_MASK                  0x0000FFFFUL                                /**< Mask for I2C_TXDOUBLE                       */
#define _I2C_TXDOUBLE_TXDATA0_SHIFT         0                                           /**< Shift value for I2C_TXDATA0                 */
#define _I2C_TXDOUBLE_TXDATA0_MASK          0xFFUL                                      /**< Bit mask for I2C_TXDATA0                    */
#define _I2C_TXDOUBLE_TXDATA0_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for I2C_TXDOUBLE               */
#define I2C_TXDOUBLE_TXDATA0_DEFAULT        (_I2C_TXDOUBLE_TXDATA0_DEFAULT << 0)        /**< Shifted mode DEFAULT for I2C_TXDOUBLE       */
#define _I2C_TXDOUBLE_TXDATA1_SHIFT         8                                           /**< Shift value for I2C_TXDATA1                 */
#define _I2C_TXDOUBLE_TXDATA1_MASK          0xFF00UL                                    /**< Bit mask for I2C_TXDATA1                    */
#define _I2C_TXDOUBLE_TXDATA1_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for I2C_TXDOUBLE               */
#define I2C_TXDOUBLE_TXDATA1_DEFAULT        (_I2C_TXDOUBLE_TXDATA1_DEFAULT << 8)        /**< Shifted mode DEFAULT for I2C_TXDOUBLE       */

/* Bit fields for I2C IF */
#define _I2C_IF_RESETVALUE                  0x00000000UL                                /**< Default value for I2C_IF                    */
#define _I2C_IF_MASK                        0x001FFFFFUL                                /**< Mask for I2C_IF                             */
#define I2C_IF_START                        (0x1UL << 0)                                /**< START condition Interrupt Flag              */
#define _I2C_IF_START_SHIFT                 0                                           /**< Shift value for I2C_START                   */
#define _I2C_IF_START_MASK                  0x1UL                                       /**< Bit mask for I2C_START                      */
#define _I2C_IF_START_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_START_DEFAULT                (_I2C_IF_START_DEFAULT << 0)                /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_RSTART                       (0x1UL << 1)                                /**< Repeated START condition Interrupt Flag     */
#define _I2C_IF_RSTART_SHIFT                1                                           /**< Shift value for I2C_RSTART                  */
#define _I2C_IF_RSTART_MASK                 0x2UL                                       /**< Bit mask for I2C_RSTART                     */
#define _I2C_IF_RSTART_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_RSTART_DEFAULT               (_I2C_IF_RSTART_DEFAULT << 1)               /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_ADDR                         (0x1UL << 2)                                /**< Address Interrupt Flag                      */
#define _I2C_IF_ADDR_SHIFT                  2                                           /**< Shift value for I2C_ADDR                    */
#define _I2C_IF_ADDR_MASK                   0x4UL                                       /**< Bit mask for I2C_ADDR                       */
#define _I2C_IF_ADDR_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_ADDR_DEFAULT                 (_I2C_IF_ADDR_DEFAULT << 2)                 /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_TXC                          (0x1UL << 3)                                /**< Transfer Completed Interrupt Flag           */
#define _I2C_IF_TXC_SHIFT                   3                                           /**< Shift value for I2C_TXC                     */
#define _I2C_IF_TXC_MASK                    0x8UL                                       /**< Bit mask for I2C_TXC                        */
#define _I2C_IF_TXC_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_TXC_DEFAULT                  (_I2C_IF_TXC_DEFAULT << 3)                  /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_TXBL                         (0x1UL << 4)                                /**< Transmit Buffer Level Interrupt Flag        */
#define _I2C_IF_TXBL_SHIFT                  4                                           /**< Shift value for I2C_TXBL                    */
#define _I2C_IF_TXBL_MASK                   0x10UL                                      /**< Bit mask for I2C_TXBL                       */
#define _I2C_IF_TXBL_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_TXBL_DEFAULT                 (_I2C_IF_TXBL_DEFAULT << 4)                 /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_RXDATAV                      (0x1UL << 5)                                /**< Receive Data Valid Interrupt Flag           */
#define _I2C_IF_RXDATAV_SHIFT               5                                           /**< Shift value for I2C_RXDATAV                 */
#define _I2C_IF_RXDATAV_MASK                0x20UL                                      /**< Bit mask for I2C_RXDATAV                    */
#define _I2C_IF_RXDATAV_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_RXDATAV_DEFAULT              (_I2C_IF_RXDATAV_DEFAULT << 5)              /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_ACK                          (0x1UL << 6)                                /**< Acknowledge Received Interrupt Flag         */
#define _I2C_IF_ACK_SHIFT                   6                                           /**< Shift value for I2C_ACK                     */
#define _I2C_IF_ACK_MASK                    0x40UL                                      /**< Bit mask for I2C_ACK                        */
#define _I2C_IF_ACK_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_ACK_DEFAULT                  (_I2C_IF_ACK_DEFAULT << 6)                  /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_NACK                         (0x1UL << 7)                                /**< Not Acknowledge Received Interrupt Flag     */
#define _I2C_IF_NACK_SHIFT                  7                                           /**< Shift value for I2C_NACK                    */
#define _I2C_IF_NACK_MASK                   0x80UL                                      /**< Bit mask for I2C_NACK                       */
#define _I2C_IF_NACK_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_NACK_DEFAULT                 (_I2C_IF_NACK_DEFAULT << 7)                 /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_MSTOP                        (0x1UL << 8)                                /**< Leader STOP Condition Interrupt Flag        */
#define _I2C_IF_MSTOP_SHIFT                 8                                           /**< Shift value for I2C_MSTOP                   */
#define _I2C_IF_MSTOP_MASK                  0x100UL                                     /**< Bit mask for I2C_MSTOP                      */
#define _I2C_IF_MSTOP_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_MSTOP_DEFAULT                (_I2C_IF_MSTOP_DEFAULT << 8)                /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_ARBLOST                      (0x1UL << 9)                                /**< Arbitration Lost Interrupt Flag             */
#define _I2C_IF_ARBLOST_SHIFT               9                                           /**< Shift value for I2C_ARBLOST                 */
#define _I2C_IF_ARBLOST_MASK                0x200UL                                     /**< Bit mask for I2C_ARBLOST                    */
#define _I2C_IF_ARBLOST_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_ARBLOST_DEFAULT              (_I2C_IF_ARBLOST_DEFAULT << 9)              /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_BUSERR                       (0x1UL << 10)                               /**< Bus Error Interrupt Flag                    */
#define _I2C_IF_BUSERR_SHIFT                10                                          /**< Shift value for I2C_BUSERR                  */
#define _I2C_IF_BUSERR_MASK                 0x400UL                                     /**< Bit mask for I2C_BUSERR                     */
#define _I2C_IF_BUSERR_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_BUSERR_DEFAULT               (_I2C_IF_BUSERR_DEFAULT << 10)              /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_BUSHOLD                      (0x1UL << 11)                               /**< Bus Held Interrupt Flag                     */
#define _I2C_IF_BUSHOLD_SHIFT               11                                          /**< Shift value for I2C_BUSHOLD                 */
#define _I2C_IF_BUSHOLD_MASK                0x800UL                                     /**< Bit mask for I2C_BUSHOLD                    */
#define _I2C_IF_BUSHOLD_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_BUSHOLD_DEFAULT              (_I2C_IF_BUSHOLD_DEFAULT << 11)             /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_TXOF                         (0x1UL << 12)                               /**< Transmit Buffer Overflow Interrupt Flag     */
#define _I2C_IF_TXOF_SHIFT                  12                                          /**< Shift value for I2C_TXOF                    */
#define _I2C_IF_TXOF_MASK                   0x1000UL                                    /**< Bit mask for I2C_TXOF                       */
#define _I2C_IF_TXOF_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_TXOF_DEFAULT                 (_I2C_IF_TXOF_DEFAULT << 12)                /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_RXUF                         (0x1UL << 13)                               /**< Receive Buffer Underflow Interrupt Flag     */
#define _I2C_IF_RXUF_SHIFT                  13                                          /**< Shift value for I2C_RXUF                    */
#define _I2C_IF_RXUF_MASK                   0x2000UL                                    /**< Bit mask for I2C_RXUF                       */
#define _I2C_IF_RXUF_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_RXUF_DEFAULT                 (_I2C_IF_RXUF_DEFAULT << 13)                /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_BITO                         (0x1UL << 14)                               /**< Bus Idle Timeout Interrupt Flag             */
#define _I2C_IF_BITO_SHIFT                  14                                          /**< Shift value for I2C_BITO                    */
#define _I2C_IF_BITO_MASK                   0x4000UL                                    /**< Bit mask for I2C_BITO                       */
#define _I2C_IF_BITO_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_BITO_DEFAULT                 (_I2C_IF_BITO_DEFAULT << 14)                /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_CLTO                         (0x1UL << 15)                               /**< Clock Low Timeout Interrupt Flag            */
#define _I2C_IF_CLTO_SHIFT                  15                                          /**< Shift value for I2C_CLTO                    */
#define _I2C_IF_CLTO_MASK                   0x8000UL                                    /**< Bit mask for I2C_CLTO                       */
#define _I2C_IF_CLTO_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_CLTO_DEFAULT                 (_I2C_IF_CLTO_DEFAULT << 15)                /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_SSTOP                        (0x1UL << 16)                               /**< Follower STOP condition Interrupt Flag      */
#define _I2C_IF_SSTOP_SHIFT                 16                                          /**< Shift value for I2C_SSTOP                   */
#define _I2C_IF_SSTOP_MASK                  0x10000UL                                   /**< Bit mask for I2C_SSTOP                      */
#define _I2C_IF_SSTOP_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_SSTOP_DEFAULT                (_I2C_IF_SSTOP_DEFAULT << 16)               /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_RXFULL                       (0x1UL << 17)                               /**< Receive Buffer Full Interrupt Flag          */
#define _I2C_IF_RXFULL_SHIFT                17                                          /**< Shift value for I2C_RXFULL                  */
#define _I2C_IF_RXFULL_MASK                 0x20000UL                                   /**< Bit mask for I2C_RXFULL                     */
#define _I2C_IF_RXFULL_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_RXFULL_DEFAULT               (_I2C_IF_RXFULL_DEFAULT << 17)              /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_CLERR                        (0x1UL << 18)                               /**< Clock Low Error Interrupt Flag              */
#define _I2C_IF_CLERR_SHIFT                 18                                          /**< Shift value for I2C_CLERR                   */
#define _I2C_IF_CLERR_MASK                  0x40000UL                                   /**< Bit mask for I2C_CLERR                      */
#define _I2C_IF_CLERR_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_CLERR_DEFAULT                (_I2C_IF_CLERR_DEFAULT << 18)               /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_SCLERR                       (0x1UL << 19)                               /**< SCL Error Interrupt Flag                    */
#define _I2C_IF_SCLERR_SHIFT                19                                          /**< Shift value for I2C_SCLERR                  */
#define _I2C_IF_SCLERR_MASK                 0x80000UL                                   /**< Bit mask for I2C_SCLERR                     */
#define _I2C_IF_SCLERR_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_SCLERR_DEFAULT               (_I2C_IF_SCLERR_DEFAULT << 19)              /**< Shifted mode DEFAULT for I2C_IF             */
#define I2C_IF_SDAERR                       (0x1UL << 20)                               /**< SDA Error Interrupt Flag                    */
#define _I2C_IF_SDAERR_SHIFT                20                                          /**< Shift value for I2C_SDAERR                  */
#define _I2C_IF_SDAERR_MASK                 0x100000UL                                  /**< Bit mask for I2C_SDAERR                     */
#define _I2C_IF_SDAERR_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IF                     */
#define I2C_IF_SDAERR_DEFAULT               (_I2C_IF_SDAERR_DEFAULT << 20)              /**< Shifted mode DEFAULT for I2C_IF             */

/* Bit fields for I2C IEN */
#define _I2C_IEN_RESETVALUE                 0x00000000UL                                /**< Default value for I2C_IEN                   */
#define _I2C_IEN_MASK                       0x001FFFFFUL                                /**< Mask for I2C_IEN                            */
#define I2C_IEN_START                       (0x1UL << 0)                                /**< START condition Interrupt Flag              */
#define _I2C_IEN_START_SHIFT                0                                           /**< Shift value for I2C_START                   */
#define _I2C_IEN_START_MASK                 0x1UL                                       /**< Bit mask for I2C_START                      */
#define _I2C_IEN_START_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_START_DEFAULT               (_I2C_IEN_START_DEFAULT << 0)               /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_RSTART                      (0x1UL << 1)                                /**< Repeated START condition Interrupt Flag     */
#define _I2C_IEN_RSTART_SHIFT               1                                           /**< Shift value for I2C_RSTART                  */
#define _I2C_IEN_RSTART_MASK                0x2UL                                       /**< Bit mask for I2C_RSTART                     */
#define _I2C_IEN_RSTART_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_RSTART_DEFAULT              (_I2C_IEN_RSTART_DEFAULT << 1)              /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_ADDR                        (0x1UL << 2)                                /**< Address Interrupt Flag                      */
#define _I2C_IEN_ADDR_SHIFT                 2                                           /**< Shift value for I2C_ADDR                    */
#define _I2C_IEN_ADDR_MASK                  0x4UL                                       /**< Bit mask for I2C_ADDR                       */
#define _I2C_IEN_ADDR_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_ADDR_DEFAULT                (_I2C_IEN_ADDR_DEFAULT << 2)                /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_TXC                         (0x1UL << 3)                                /**< Transfer Completed Interrupt Flag           */
#define _I2C_IEN_TXC_SHIFT                  3                                           /**< Shift value for I2C_TXC                     */
#define _I2C_IEN_TXC_MASK                   0x8UL                                       /**< Bit mask for I2C_TXC                        */
#define _I2C_IEN_TXC_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_TXC_DEFAULT                 (_I2C_IEN_TXC_DEFAULT << 3)                 /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_TXBL                        (0x1UL << 4)                                /**< Transmit Buffer Level Interrupt Flag        */
#define _I2C_IEN_TXBL_SHIFT                 4                                           /**< Shift value for I2C_TXBL                    */
#define _I2C_IEN_TXBL_MASK                  0x10UL                                      /**< Bit mask for I2C_TXBL                       */
#define _I2C_IEN_TXBL_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_TXBL_DEFAULT                (_I2C_IEN_TXBL_DEFAULT << 4)                /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_RXDATAV                     (0x1UL << 5)                                /**< Receive Data Valid Interrupt Flag           */
#define _I2C_IEN_RXDATAV_SHIFT              5                                           /**< Shift value for I2C_RXDATAV                 */
#define _I2C_IEN_RXDATAV_MASK               0x20UL                                      /**< Bit mask for I2C_RXDATAV                    */
#define _I2C_IEN_RXDATAV_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_RXDATAV_DEFAULT             (_I2C_IEN_RXDATAV_DEFAULT << 5)             /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_ACK                         (0x1UL << 6)                                /**< Acknowledge Received Interrupt Flag         */
#define _I2C_IEN_ACK_SHIFT                  6                                           /**< Shift value for I2C_ACK                     */
#define _I2C_IEN_ACK_MASK                   0x40UL                                      /**< Bit mask for I2C_ACK                        */
#define _I2C_IEN_ACK_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_ACK_DEFAULT                 (_I2C_IEN_ACK_DEFAULT << 6)                 /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_NACK                        (0x1UL << 7)                                /**< Not Acknowledge Received Interrupt Flag     */
#define _I2C_IEN_NACK_SHIFT                 7                                           /**< Shift value for I2C_NACK                    */
#define _I2C_IEN_NACK_MASK                  0x80UL                                      /**< Bit mask for I2C_NACK                       */
#define _I2C_IEN_NACK_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_NACK_DEFAULT                (_I2C_IEN_NACK_DEFAULT << 7)                /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_MSTOP                       (0x1UL << 8)                                /**< Leader STOP Condition Interrupt Flag        */
#define _I2C_IEN_MSTOP_SHIFT                8                                           /**< Shift value for I2C_MSTOP                   */
#define _I2C_IEN_MSTOP_MASK                 0x100UL                                     /**< Bit mask for I2C_MSTOP                      */
#define _I2C_IEN_MSTOP_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_MSTOP_DEFAULT               (_I2C_IEN_MSTOP_DEFAULT << 8)               /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_ARBLOST                     (0x1UL << 9)                                /**< Arbitration Lost Interrupt Flag             */
#define _I2C_IEN_ARBLOST_SHIFT              9                                           /**< Shift value for I2C_ARBLOST                 */
#define _I2C_IEN_ARBLOST_MASK               0x200UL                                     /**< Bit mask for I2C_ARBLOST                    */
#define _I2C_IEN_ARBLOST_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_ARBLOST_DEFAULT             (_I2C_IEN_ARBLOST_DEFAULT << 9)             /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_BUSERR                      (0x1UL << 10)                               /**< Bus Error Interrupt Flag                    */
#define _I2C_IEN_BUSERR_SHIFT               10                                          /**< Shift value for I2C_BUSERR                  */
#define _I2C_IEN_BUSERR_MASK                0x400UL                                     /**< Bit mask for I2C_BUSERR                     */
#define _I2C_IEN_BUSERR_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_BUSERR_DEFAULT              (_I2C_IEN_BUSERR_DEFAULT << 10)             /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_BUSHOLD                     (0x1UL << 11)                               /**< Bus Held Interrupt Flag                     */
#define _I2C_IEN_BUSHOLD_SHIFT              11                                          /**< Shift value for I2C_BUSHOLD                 */
#define _I2C_IEN_BUSHOLD_MASK               0x800UL                                     /**< Bit mask for I2C_BUSHOLD                    */
#define _I2C_IEN_BUSHOLD_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_BUSHOLD_DEFAULT             (_I2C_IEN_BUSHOLD_DEFAULT << 11)            /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_TXOF                        (0x1UL << 12)                               /**< Transmit Buffer Overflow Interrupt Flag     */
#define _I2C_IEN_TXOF_SHIFT                 12                                          /**< Shift value for I2C_TXOF                    */
#define _I2C_IEN_TXOF_MASK                  0x1000UL                                    /**< Bit mask for I2C_TXOF                       */
#define _I2C_IEN_TXOF_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_TXOF_DEFAULT                (_I2C_IEN_TXOF_DEFAULT << 12)               /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_RXUF                        (0x1UL << 13)                               /**< Receive Buffer Underflow Interrupt Flag     */
#define _I2C_IEN_RXUF_SHIFT                 13                                          /**< Shift value for I2C_RXUF                    */
#define _I2C_IEN_RXUF_MASK                  0x2000UL                                    /**< Bit mask for I2C_RXUF                       */
#define _I2C_IEN_RXUF_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_RXUF_DEFAULT                (_I2C_IEN_RXUF_DEFAULT << 13)               /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_BITO                        (0x1UL << 14)                               /**< Bus Idle Timeout Interrupt Flag             */
#define _I2C_IEN_BITO_SHIFT                 14                                          /**< Shift value for I2C_BITO                    */
#define _I2C_IEN_BITO_MASK                  0x4000UL                                    /**< Bit mask for I2C_BITO                       */
#define _I2C_IEN_BITO_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_BITO_DEFAULT                (_I2C_IEN_BITO_DEFAULT << 14)               /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_CLTO                        (0x1UL << 15)                               /**< Clock Low Timeout Interrupt Flag            */
#define _I2C_IEN_CLTO_SHIFT                 15                                          /**< Shift value for I2C_CLTO                    */
#define _I2C_IEN_CLTO_MASK                  0x8000UL                                    /**< Bit mask for I2C_CLTO                       */
#define _I2C_IEN_CLTO_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_CLTO_DEFAULT                (_I2C_IEN_CLTO_DEFAULT << 15)               /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_SSTOP                       (0x1UL << 16)                               /**< Follower STOP condition Interrupt Flag      */
#define _I2C_IEN_SSTOP_SHIFT                16                                          /**< Shift value for I2C_SSTOP                   */
#define _I2C_IEN_SSTOP_MASK                 0x10000UL                                   /**< Bit mask for I2C_SSTOP                      */
#define _I2C_IEN_SSTOP_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_SSTOP_DEFAULT               (_I2C_IEN_SSTOP_DEFAULT << 16)              /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_RXFULL                      (0x1UL << 17)                               /**< Receive Buffer Full Interrupt Flag          */
#define _I2C_IEN_RXFULL_SHIFT               17                                          /**< Shift value for I2C_RXFULL                  */
#define _I2C_IEN_RXFULL_MASK                0x20000UL                                   /**< Bit mask for I2C_RXFULL                     */
#define _I2C_IEN_RXFULL_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_RXFULL_DEFAULT              (_I2C_IEN_RXFULL_DEFAULT << 17)             /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_CLERR                       (0x1UL << 18)                               /**< Clock Low Error Interrupt Flag              */
#define _I2C_IEN_CLERR_SHIFT                18                                          /**< Shift value for I2C_CLERR                   */
#define _I2C_IEN_CLERR_MASK                 0x40000UL                                   /**< Bit mask for I2C_CLERR                      */
#define _I2C_IEN_CLERR_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_CLERR_DEFAULT               (_I2C_IEN_CLERR_DEFAULT << 18)              /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_SCLERR                      (0x1UL << 19)                               /**< SCL Error Interrupt Flag                    */
#define _I2C_IEN_SCLERR_SHIFT               19                                          /**< Shift value for I2C_SCLERR                  */
#define _I2C_IEN_SCLERR_MASK                0x80000UL                                   /**< Bit mask for I2C_SCLERR                     */
#define _I2C_IEN_SCLERR_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_SCLERR_DEFAULT              (_I2C_IEN_SCLERR_DEFAULT << 19)             /**< Shifted mode DEFAULT for I2C_IEN            */
#define I2C_IEN_SDAERR                      (0x1UL << 20)                               /**< SDA Error Interrupt Flag                    */
#define _I2C_IEN_SDAERR_SHIFT               20                                          /**< Shift value for I2C_SDAERR                  */
#define _I2C_IEN_SDAERR_MASK                0x100000UL                                  /**< Bit mask for I2C_SDAERR                     */
#define _I2C_IEN_SDAERR_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for I2C_IEN                    */
#define I2C_IEN_SDAERR_DEFAULT              (_I2C_IEN_SDAERR_DEFAULT << 20)             /**< Shifted mode DEFAULT for I2C_IEN            */

/** @} End of group EFR32ZG23_I2C_BitFields */
/** @} End of group EFR32ZG23_I2C */
/** @} End of group Parts */

#endif /* EFR32ZG23_I2C_H */
#ifdef __cplusplus
}
#endif

