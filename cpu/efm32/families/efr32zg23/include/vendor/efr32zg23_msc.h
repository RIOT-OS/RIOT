/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 MSC register and bit field definitions
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

#ifndef EFR32ZG23_MSC_H
#define EFR32ZG23_MSC_H
#define MSC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_MSC MSC
 * @{
 * @brief EFR32ZG23 MSC Register Declaration.
 *****************************************************************************/

/** MSC Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t READCTRL;                      /**< Read Control Register                              */
  __IOM uint32_t RDATACTRL;                     /**< Read Data Control Register                         */
  __IOM uint32_t WRITECTRL;                     /**< Write Control Register                             */
  __IOM uint32_t WRITECMD;                      /**< Write Command Register                             */
  __IOM uint32_t ADDRB;                         /**< Page Erase/Write Address Buffer                    */
  __IOM uint32_t WDATA;                         /**< Write Data Register                                */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED0[3U];                 /**< Reserved for future use                            */
  __IM uint32_t  USERDATASIZE;                  /**< User Data Region Size Register                     */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  __IOM uint32_t MISCLOCKWORD;                  /**< Mass erase and User data page lock word            */
  uint32_t       RESERVED1[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t PWRCTRL;                       /**< Power control register                             */
  uint32_t       RESERVED2[51U];                /**< Reserved for future use                            */
  __IOM uint32_t PAGELOCK0;                     /**< Main space page 0-31 lock word                     */
  __IOM uint32_t PAGELOCK1;                     /**< Main space page 32-63 lock word                    */
  uint32_t       RESERVED3[2U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED4[4U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED5[4U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED6[4U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[4U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED8[12U];                /**< Reserved for future use                            */
  uint32_t       RESERVED9[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED10[8U];                /**< Reserved for future use                            */
  uint32_t       RESERVED11[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED12[910U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t READCTRL_SET;                  /**< Read Control Register                              */
  __IOM uint32_t RDATACTRL_SET;                 /**< Read Data Control Register                         */
  __IOM uint32_t WRITECTRL_SET;                 /**< Write Control Register                             */
  __IOM uint32_t WRITECMD_SET;                  /**< Write Command Register                             */
  __IOM uint32_t ADDRB_SET;                     /**< Page Erase/Write Address Buffer                    */
  __IOM uint32_t WDATA_SET;                     /**< Write Data Register                                */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED13[3U];                /**< Reserved for future use                            */
  __IM uint32_t  USERDATASIZE_SET;              /**< User Data Region Size Register                     */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  __IOM uint32_t MISCLOCKWORD_SET;              /**< Mass erase and User data page lock word            */
  uint32_t       RESERVED14[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PWRCTRL_SET;                   /**< Power control register                             */
  uint32_t       RESERVED15[51U];               /**< Reserved for future use                            */
  __IOM uint32_t PAGELOCK0_SET;                 /**< Main space page 0-31 lock word                     */
  __IOM uint32_t PAGELOCK1_SET;                 /**< Main space page 32-63 lock word                    */
  uint32_t       RESERVED16[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED17[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED18[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED19[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED20[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED21[12U];               /**< Reserved for future use                            */
  uint32_t       RESERVED22[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED23[8U];                /**< Reserved for future use                            */
  uint32_t       RESERVED24[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED25[910U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t READCTRL_CLR;                  /**< Read Control Register                              */
  __IOM uint32_t RDATACTRL_CLR;                 /**< Read Data Control Register                         */
  __IOM uint32_t WRITECTRL_CLR;                 /**< Write Control Register                             */
  __IOM uint32_t WRITECMD_CLR;                  /**< Write Command Register                             */
  __IOM uint32_t ADDRB_CLR;                     /**< Page Erase/Write Address Buffer                    */
  __IOM uint32_t WDATA_CLR;                     /**< Write Data Register                                */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED26[3U];                /**< Reserved for future use                            */
  __IM uint32_t  USERDATASIZE_CLR;              /**< User Data Region Size Register                     */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  __IOM uint32_t MISCLOCKWORD_CLR;              /**< Mass erase and User data page lock word            */
  uint32_t       RESERVED27[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PWRCTRL_CLR;                   /**< Power control register                             */
  uint32_t       RESERVED28[51U];               /**< Reserved for future use                            */
  __IOM uint32_t PAGELOCK0_CLR;                 /**< Main space page 0-31 lock word                     */
  __IOM uint32_t PAGELOCK1_CLR;                 /**< Main space page 32-63 lock word                    */
  uint32_t       RESERVED29[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED30[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED31[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED32[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED33[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED34[12U];               /**< Reserved for future use                            */
  uint32_t       RESERVED35[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED36[8U];                /**< Reserved for future use                            */
  uint32_t       RESERVED37[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED38[910U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t READCTRL_TGL;                  /**< Read Control Register                              */
  __IOM uint32_t RDATACTRL_TGL;                 /**< Read Data Control Register                         */
  __IOM uint32_t WRITECTRL_TGL;                 /**< Write Control Register                             */
  __IOM uint32_t WRITECMD_TGL;                  /**< Write Command Register                             */
  __IOM uint32_t ADDRB_TGL;                     /**< Page Erase/Write Address Buffer                    */
  __IOM uint32_t WDATA_TGL;                     /**< Write Data Register                                */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED39[3U];                /**< Reserved for future use                            */
  __IM uint32_t  USERDATASIZE_TGL;              /**< User Data Region Size Register                     */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
  __IOM uint32_t MISCLOCKWORD_TGL;              /**< Mass erase and User data page lock word            */
  uint32_t       RESERVED40[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PWRCTRL_TGL;                   /**< Power control register                             */
  uint32_t       RESERVED41[51U];               /**< Reserved for future use                            */
  __IOM uint32_t PAGELOCK0_TGL;                 /**< Main space page 0-31 lock word                     */
  __IOM uint32_t PAGELOCK1_TGL;                 /**< Main space page 32-63 lock word                    */
  uint32_t       RESERVED42[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED43[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED44[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED45[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED46[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED47[12U];               /**< Reserved for future use                            */
  uint32_t       RESERVED48[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED49[8U];                /**< Reserved for future use                            */
  uint32_t       RESERVED50[1U];                /**< Reserved for future use                            */
} MSC_TypeDef;
/** @} End of group EFR32ZG23_MSC */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_MSC
 * @{
 * @defgroup EFR32ZG23_MSC_BitFields MSC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for MSC IPVERSION */
#define _MSC_IPVERSION_RESETVALUE                 0x00000002UL                            /**< Default value for MSC_IPVERSION             */
#define _MSC_IPVERSION_MASK                       0xFFFFFFFFUL                            /**< Mask for MSC_IPVERSION                      */
#define _MSC_IPVERSION_IPVERSION_SHIFT            0                                       /**< Shift value for MSC_IPVERSION               */
#define _MSC_IPVERSION_IPVERSION_MASK             0xFFFFFFFFUL                            /**< Bit mask for MSC_IPVERSION                  */
#define _MSC_IPVERSION_IPVERSION_DEFAULT          0x00000002UL                            /**< Mode DEFAULT for MSC_IPVERSION              */
#define MSC_IPVERSION_IPVERSION_DEFAULT           (_MSC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_IPVERSION      */

/* Bit fields for MSC READCTRL */
#define _MSC_READCTRL_RESETVALUE                  0x00200000UL                          /**< Default value for MSC_READCTRL              */
#define _MSC_READCTRL_MASK                        0x00300000UL                          /**< Mask for MSC_READCTRL                       */
#define _MSC_READCTRL_MODE_SHIFT                  20                                    /**< Shift value for MSC_MODE                    */
#define _MSC_READCTRL_MODE_MASK                   0x300000UL                            /**< Bit mask for MSC_MODE                       */
#define _MSC_READCTRL_MODE_DEFAULT                0x00000002UL                          /**< Mode DEFAULT for MSC_READCTRL               */
#define _MSC_READCTRL_MODE_WS0                    0x00000000UL                          /**< Mode WS0 for MSC_READCTRL                   */
#define _MSC_READCTRL_MODE_WS1                    0x00000001UL                          /**< Mode WS1 for MSC_READCTRL                   */
#define _MSC_READCTRL_MODE_WS2                    0x00000002UL                          /**< Mode WS2 for MSC_READCTRL                   */
#define _MSC_READCTRL_MODE_WS3                    0x00000003UL                          /**< Mode WS3 for MSC_READCTRL                   */
#define MSC_READCTRL_MODE_DEFAULT                 (_MSC_READCTRL_MODE_DEFAULT << 20)    /**< Shifted mode DEFAULT for MSC_READCTRL       */
#define MSC_READCTRL_MODE_WS0                     (_MSC_READCTRL_MODE_WS0 << 20)        /**< Shifted mode WS0 for MSC_READCTRL           */
#define MSC_READCTRL_MODE_WS1                     (_MSC_READCTRL_MODE_WS1 << 20)        /**< Shifted mode WS1 for MSC_READCTRL           */
#define MSC_READCTRL_MODE_WS2                     (_MSC_READCTRL_MODE_WS2 << 20)        /**< Shifted mode WS2 for MSC_READCTRL           */
#define MSC_READCTRL_MODE_WS3                     (_MSC_READCTRL_MODE_WS3 << 20)        /**< Shifted mode WS3 for MSC_READCTRL           */

/* Bit fields for MSC RDATACTRL */
#define _MSC_RDATACTRL_RESETVALUE                 0x00001000UL                             /**< Default value for MSC_RDATACTRL             */
#define _MSC_RDATACTRL_MASK                       0x00001002UL                             /**< Mask for MSC_RDATACTRL                      */
#define MSC_RDATACTRL_AFDIS                       (0x1UL << 1)                             /**< Automatic Invalidate Disable                */
#define _MSC_RDATACTRL_AFDIS_SHIFT                1                                        /**< Shift value for MSC_AFDIS                   */
#define _MSC_RDATACTRL_AFDIS_MASK                 0x2UL                                    /**< Bit mask for MSC_AFDIS                      */
#define _MSC_RDATACTRL_AFDIS_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for MSC_RDATACTRL              */
#define MSC_RDATACTRL_AFDIS_DEFAULT               (_MSC_RDATACTRL_AFDIS_DEFAULT << 1)      /**< Shifted mode DEFAULT for MSC_RDATACTRL      */
#define MSC_RDATACTRL_DOUTBUFEN                   (0x1UL << 12)                            /**< Flash dout pipeline buffer enable           */
#define _MSC_RDATACTRL_DOUTBUFEN_SHIFT            12                                       /**< Shift value for MSC_DOUTBUFEN               */
#define _MSC_RDATACTRL_DOUTBUFEN_MASK             0x1000UL                                 /**< Bit mask for MSC_DOUTBUFEN                  */
#define _MSC_RDATACTRL_DOUTBUFEN_DEFAULT          0x00000001UL                             /**< Mode DEFAULT for MSC_RDATACTRL              */
#define MSC_RDATACTRL_DOUTBUFEN_DEFAULT           (_MSC_RDATACTRL_DOUTBUFEN_DEFAULT << 12) /**< Shifted mode DEFAULT for MSC_RDATACTRL      */

/* Bit fields for MSC WRITECTRL */
#define _MSC_WRITECTRL_RESETVALUE                 0x00000000UL                                /**< Default value for MSC_WRITECTRL             */
#define _MSC_WRITECTRL_MASK                       0x00FF000BUL                                /**< Mask for MSC_WRITECTRL                      */
#define MSC_WRITECTRL_WREN                        (0x1UL << 0)                                /**< Enable Write/Erase Controller               */
#define _MSC_WRITECTRL_WREN_SHIFT                 0                                           /**< Shift value for MSC_WREN                    */
#define _MSC_WRITECTRL_WREN_MASK                  0x1UL                                       /**< Bit mask for MSC_WREN                       */
#define _MSC_WRITECTRL_WREN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for MSC_WRITECTRL              */
#define MSC_WRITECTRL_WREN_DEFAULT                (_MSC_WRITECTRL_WREN_DEFAULT << 0)          /**< Shifted mode DEFAULT for MSC_WRITECTRL      */
#define MSC_WRITECTRL_IRQERASEABORT               (0x1UL << 1)                                /**< Abort Page Erase on Interrupt               */
#define _MSC_WRITECTRL_IRQERASEABORT_SHIFT        1                                           /**< Shift value for MSC_IRQERASEABORT           */
#define _MSC_WRITECTRL_IRQERASEABORT_MASK         0x2UL                                       /**< Bit mask for MSC_IRQERASEABORT              */
#define _MSC_WRITECTRL_IRQERASEABORT_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for MSC_WRITECTRL              */
#define MSC_WRITECTRL_IRQERASEABORT_DEFAULT       (_MSC_WRITECTRL_IRQERASEABORT_DEFAULT << 1) /**< Shifted mode DEFAULT for MSC_WRITECTRL      */
#define MSC_WRITECTRL_LPWRITE                     (0x1UL << 3)                                /**< Low-Power Write                             */
#define _MSC_WRITECTRL_LPWRITE_SHIFT              3                                           /**< Shift value for MSC_LPWRITE                 */
#define _MSC_WRITECTRL_LPWRITE_MASK               0x8UL                                       /**< Bit mask for MSC_LPWRITE                    */
#define _MSC_WRITECTRL_LPWRITE_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for MSC_WRITECTRL              */
#define MSC_WRITECTRL_LPWRITE_DEFAULT             (_MSC_WRITECTRL_LPWRITE_DEFAULT << 3)       /**< Shifted mode DEFAULT for MSC_WRITECTRL      */
#define _MSC_WRITECTRL_RANGECOUNT_SHIFT           16                                          /**< Shift value for MSC_RANGECOUNT              */
#define _MSC_WRITECTRL_RANGECOUNT_MASK            0xFF0000UL                                  /**< Bit mask for MSC_RANGECOUNT                 */
#define _MSC_WRITECTRL_RANGECOUNT_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for MSC_WRITECTRL              */
#define MSC_WRITECTRL_RANGECOUNT_DEFAULT          (_MSC_WRITECTRL_RANGECOUNT_DEFAULT << 16)   /**< Shifted mode DEFAULT for MSC_WRITECTRL      */

/* Bit fields for MSC WRITECMD */
#define _MSC_WRITECMD_RESETVALUE                  0x00000000UL                             /**< Default value for MSC_WRITECMD              */
#define _MSC_WRITECMD_MASK                        0x00001136UL                             /**< Mask for MSC_WRITECMD                       */
#define MSC_WRITECMD_ERASEPAGE                    (0x1UL << 1)                             /**< Erase Page                                  */
#define _MSC_WRITECMD_ERASEPAGE_SHIFT             1                                        /**< Shift value for MSC_ERASEPAGE               */
#define _MSC_WRITECMD_ERASEPAGE_MASK              0x2UL                                    /**< Bit mask for MSC_ERASEPAGE                  */
#define _MSC_WRITECMD_ERASEPAGE_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD               */
#define MSC_WRITECMD_ERASEPAGE_DEFAULT            (_MSC_WRITECMD_ERASEPAGE_DEFAULT << 1)   /**< Shifted mode DEFAULT for MSC_WRITECMD       */
#define MSC_WRITECMD_WRITEEND                     (0x1UL << 2)                             /**< End Write Mode                              */
#define _MSC_WRITECMD_WRITEEND_SHIFT              2                                        /**< Shift value for MSC_WRITEEND                */
#define _MSC_WRITECMD_WRITEEND_MASK               0x4UL                                    /**< Bit mask for MSC_WRITEEND                   */
#define _MSC_WRITECMD_WRITEEND_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD               */
#define MSC_WRITECMD_WRITEEND_DEFAULT             (_MSC_WRITECMD_WRITEEND_DEFAULT << 2)    /**< Shifted mode DEFAULT for MSC_WRITECMD       */
#define MSC_WRITECMD_ERASERANGE                   (0x1UL << 4)                             /**< Erase range of pages                        */
#define _MSC_WRITECMD_ERASERANGE_SHIFT            4                                        /**< Shift value for MSC_ERASERANGE              */
#define _MSC_WRITECMD_ERASERANGE_MASK             0x10UL                                   /**< Bit mask for MSC_ERASERANGE                 */
#define _MSC_WRITECMD_ERASERANGE_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD               */
#define MSC_WRITECMD_ERASERANGE_DEFAULT           (_MSC_WRITECMD_ERASERANGE_DEFAULT << 4)  /**< Shifted mode DEFAULT for MSC_WRITECMD       */
#define MSC_WRITECMD_ERASEABORT                   (0x1UL << 5)                             /**< Abort erase sequence                        */
#define _MSC_WRITECMD_ERASEABORT_SHIFT            5                                        /**< Shift value for MSC_ERASEABORT              */
#define _MSC_WRITECMD_ERASEABORT_MASK             0x20UL                                   /**< Bit mask for MSC_ERASEABORT                 */
#define _MSC_WRITECMD_ERASEABORT_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD               */
#define MSC_WRITECMD_ERASEABORT_DEFAULT           (_MSC_WRITECMD_ERASEABORT_DEFAULT << 5)  /**< Shifted mode DEFAULT for MSC_WRITECMD       */
#define MSC_WRITECMD_ERASEMAIN0                   (0x1UL << 8)                             /**< Mass erase region 0                         */
#define _MSC_WRITECMD_ERASEMAIN0_SHIFT            8                                        /**< Shift value for MSC_ERASEMAIN0              */
#define _MSC_WRITECMD_ERASEMAIN0_MASK             0x100UL                                  /**< Bit mask for MSC_ERASEMAIN0                 */
#define _MSC_WRITECMD_ERASEMAIN0_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD               */
#define MSC_WRITECMD_ERASEMAIN0_DEFAULT           (_MSC_WRITECMD_ERASEMAIN0_DEFAULT << 8)  /**< Shifted mode DEFAULT for MSC_WRITECMD       */
#define MSC_WRITECMD_CLEARWDATA                   (0x1UL << 12)                            /**< Clear WDATA state                           */
#define _MSC_WRITECMD_CLEARWDATA_SHIFT            12                                       /**< Shift value for MSC_CLEARWDATA              */
#define _MSC_WRITECMD_CLEARWDATA_MASK             0x1000UL                                 /**< Bit mask for MSC_CLEARWDATA                 */
#define _MSC_WRITECMD_CLEARWDATA_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD               */
#define MSC_WRITECMD_CLEARWDATA_DEFAULT           (_MSC_WRITECMD_CLEARWDATA_DEFAULT << 12) /**< Shifted mode DEFAULT for MSC_WRITECMD       */

/* Bit fields for MSC ADDRB */
#define _MSC_ADDRB_RESETVALUE                     0x00000000UL                          /**< Default value for MSC_ADDRB                 */
#define _MSC_ADDRB_MASK                           0xFFFFFFFFUL                          /**< Mask for MSC_ADDRB                          */
#define _MSC_ADDRB_ADDRB_SHIFT                    0                                     /**< Shift value for MSC_ADDRB                   */
#define _MSC_ADDRB_ADDRB_MASK                     0xFFFFFFFFUL                          /**< Bit mask for MSC_ADDRB                      */
#define _MSC_ADDRB_ADDRB_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for MSC_ADDRB                  */
#define MSC_ADDRB_ADDRB_DEFAULT                   (_MSC_ADDRB_ADDRB_DEFAULT << 0)       /**< Shifted mode DEFAULT for MSC_ADDRB          */

/* Bit fields for MSC WDATA */
#define _MSC_WDATA_RESETVALUE                     0x00000000UL                          /**< Default value for MSC_WDATA                 */
#define _MSC_WDATA_MASK                           0xFFFFFFFFUL                          /**< Mask for MSC_WDATA                          */
#define _MSC_WDATA_DATAW_SHIFT                    0                                     /**< Shift value for MSC_DATAW                   */
#define _MSC_WDATA_DATAW_MASK                     0xFFFFFFFFUL                          /**< Bit mask for MSC_DATAW                      */
#define _MSC_WDATA_DATAW_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for MSC_WDATA                  */
#define MSC_WDATA_DATAW_DEFAULT                   (_MSC_WDATA_DATAW_DEFAULT << 0)       /**< Shifted mode DEFAULT for MSC_WDATA          */

/* Bit fields for MSC STATUS */
#define _MSC_STATUS_RESETVALUE                    0x08000008UL                                   /**< Default value for MSC_STATUS                */
#define _MSC_STATUS_MASK                          0xF90100FFUL                                   /**< Mask for MSC_STATUS                         */
#define MSC_STATUS_BUSY                           (0x1UL << 0)                                   /**< Erase/Write Busy                            */
#define _MSC_STATUS_BUSY_SHIFT                    0                                              /**< Shift value for MSC_BUSY                    */
#define _MSC_STATUS_BUSY_MASK                     0x1UL                                          /**< Bit mask for MSC_BUSY                       */
#define _MSC_STATUS_BUSY_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_BUSY_DEFAULT                   (_MSC_STATUS_BUSY_DEFAULT << 0)                /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_LOCKED                         (0x1UL << 1)                                   /**< Access Locked                               */
#define _MSC_STATUS_LOCKED_SHIFT                  1                                              /**< Shift value for MSC_LOCKED                  */
#define _MSC_STATUS_LOCKED_MASK                   0x2UL                                          /**< Bit mask for MSC_LOCKED                     */
#define _MSC_STATUS_LOCKED_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_LOCKED_DEFAULT                 (_MSC_STATUS_LOCKED_DEFAULT << 1)              /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_INVADDR                        (0x1UL << 2)                                   /**< Invalid Write Address or Erase Page         */
#define _MSC_STATUS_INVADDR_SHIFT                 2                                              /**< Shift value for MSC_INVADDR                 */
#define _MSC_STATUS_INVADDR_MASK                  0x4UL                                          /**< Bit mask for MSC_INVADDR                    */
#define _MSC_STATUS_INVADDR_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_INVADDR_DEFAULT                (_MSC_STATUS_INVADDR_DEFAULT << 2)             /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_WDATAREADY                     (0x1UL << 3)                                   /**< WDATA Write Ready                           */
#define _MSC_STATUS_WDATAREADY_SHIFT              3                                              /**< Shift value for MSC_WDATAREADY              */
#define _MSC_STATUS_WDATAREADY_MASK               0x8UL                                          /**< Bit mask for MSC_WDATAREADY                 */
#define _MSC_STATUS_WDATAREADY_DEFAULT            0x00000001UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_WDATAREADY_DEFAULT             (_MSC_STATUS_WDATAREADY_DEFAULT << 3)          /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_ERASEABORTED                   (0x1UL << 4)                                   /**< The Current Flash Erase Operation Aborte    */
#define _MSC_STATUS_ERASEABORTED_SHIFT            4                                              /**< Shift value for MSC_ERASEABORTED            */
#define _MSC_STATUS_ERASEABORTED_MASK             0x10UL                                         /**< Bit mask for MSC_ERASEABORTED               */
#define _MSC_STATUS_ERASEABORTED_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_ERASEABORTED_DEFAULT           (_MSC_STATUS_ERASEABORTED_DEFAULT << 4)        /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_PENDING                        (0x1UL << 5)                                   /**< Write command is in queue                   */
#define _MSC_STATUS_PENDING_SHIFT                 5                                              /**< Shift value for MSC_PENDING                 */
#define _MSC_STATUS_PENDING_MASK                  0x20UL                                         /**< Bit mask for MSC_PENDING                    */
#define _MSC_STATUS_PENDING_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_PENDING_DEFAULT                (_MSC_STATUS_PENDING_DEFAULT << 5)             /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_TIMEOUT                        (0x1UL << 6)                                   /**< Write command timeout flag                  */
#define _MSC_STATUS_TIMEOUT_SHIFT                 6                                              /**< Shift value for MSC_TIMEOUT                 */
#define _MSC_STATUS_TIMEOUT_MASK                  0x40UL                                         /**< Bit mask for MSC_TIMEOUT                    */
#define _MSC_STATUS_TIMEOUT_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_TIMEOUT_DEFAULT                (_MSC_STATUS_TIMEOUT_DEFAULT << 6)             /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_RANGEPARTIAL                   (0x1UL << 7)                                   /**< EraseRange with skipped locked pages        */
#define _MSC_STATUS_RANGEPARTIAL_SHIFT            7                                              /**< Shift value for MSC_RANGEPARTIAL            */
#define _MSC_STATUS_RANGEPARTIAL_MASK             0x80UL                                         /**< Bit mask for MSC_RANGEPARTIAL               */
#define _MSC_STATUS_RANGEPARTIAL_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_RANGEPARTIAL_DEFAULT           (_MSC_STATUS_RANGEPARTIAL_DEFAULT << 7)        /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_REGLOCK                        (0x1UL << 16)                                  /**< Register Lock Status                        */
#define _MSC_STATUS_REGLOCK_SHIFT                 16                                             /**< Shift value for MSC_REGLOCK                 */
#define _MSC_STATUS_REGLOCK_MASK                  0x10000UL                                      /**< Bit mask for MSC_REGLOCK                    */
#define _MSC_STATUS_REGLOCK_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define _MSC_STATUS_REGLOCK_UNLOCKED              0x00000000UL                                   /**< Mode UNLOCKED for MSC_STATUS                */
#define _MSC_STATUS_REGLOCK_LOCKED                0x00000001UL                                   /**< Mode LOCKED for MSC_STATUS                  */
#define MSC_STATUS_REGLOCK_DEFAULT                (_MSC_STATUS_REGLOCK_DEFAULT << 16)            /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_REGLOCK_UNLOCKED               (_MSC_STATUS_REGLOCK_UNLOCKED << 16)           /**< Shifted mode UNLOCKED for MSC_STATUS        */
#define MSC_STATUS_REGLOCK_LOCKED                 (_MSC_STATUS_REGLOCK_LOCKED << 16)             /**< Shifted mode LOCKED for MSC_STATUS          */
#define MSC_STATUS_PWRON                          (0x1UL << 24)                                  /**< Flash power on status                       */
#define _MSC_STATUS_PWRON_SHIFT                   24                                             /**< Shift value for MSC_PWRON                   */
#define _MSC_STATUS_PWRON_MASK                    0x1000000UL                                    /**< Bit mask for MSC_PWRON                      */
#define _MSC_STATUS_PWRON_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_PWRON_DEFAULT                  (_MSC_STATUS_PWRON_DEFAULT << 24)              /**< Shifted mode DEFAULT for MSC_STATUS         */
#define MSC_STATUS_WREADY                         (0x1UL << 27)                                  /**< Flash Write Ready                           */
#define _MSC_STATUS_WREADY_SHIFT                  27                                             /**< Shift value for MSC_WREADY                  */
#define _MSC_STATUS_WREADY_MASK                   0x8000000UL                                    /**< Bit mask for MSC_WREADY                     */
#define _MSC_STATUS_WREADY_DEFAULT                0x00000001UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_WREADY_DEFAULT                 (_MSC_STATUS_WREADY_DEFAULT << 27)             /**< Shifted mode DEFAULT for MSC_STATUS         */
#define _MSC_STATUS_PWRUPCKBDFAILCOUNT_SHIFT      28                                             /**< Shift value for MSC_PWRUPCKBDFAILCOUNT      */
#define _MSC_STATUS_PWRUPCKBDFAILCOUNT_MASK       0xF0000000UL                                   /**< Bit mask for MSC_PWRUPCKBDFAILCOUNT         */
#define _MSC_STATUS_PWRUPCKBDFAILCOUNT_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for MSC_STATUS                 */
#define MSC_STATUS_PWRUPCKBDFAILCOUNT_DEFAULT     (_MSC_STATUS_PWRUPCKBDFAILCOUNT_DEFAULT << 28) /**< Shifted mode DEFAULT for MSC_STATUS         */

/* Bit fields for MSC IF */
#define _MSC_IF_RESETVALUE                        0x00000000UL                          /**< Default value for MSC_IF                    */
#define _MSC_IF_MASK                              0x00000307UL                          /**< Mask for MSC_IF                             */
#define MSC_IF_ERASE                              (0x1UL << 0)                          /**< Host Erase Done Interrupt Read Flag         */
#define _MSC_IF_ERASE_SHIFT                       0                                     /**< Shift value for MSC_ERASE                   */
#define _MSC_IF_ERASE_MASK                        0x1UL                                 /**< Bit mask for MSC_ERASE                      */
#define _MSC_IF_ERASE_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for MSC_IF                     */
#define MSC_IF_ERASE_DEFAULT                      (_MSC_IF_ERASE_DEFAULT << 0)          /**< Shifted mode DEFAULT for MSC_IF             */
#define MSC_IF_WRITE                              (0x1UL << 1)                          /**< Host Write Done Interrupt Read Flag         */
#define _MSC_IF_WRITE_SHIFT                       1                                     /**< Shift value for MSC_WRITE                   */
#define _MSC_IF_WRITE_MASK                        0x2UL                                 /**< Bit mask for MSC_WRITE                      */
#define _MSC_IF_WRITE_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for MSC_IF                     */
#define MSC_IF_WRITE_DEFAULT                      (_MSC_IF_WRITE_DEFAULT << 1)          /**< Shifted mode DEFAULT for MSC_IF             */
#define MSC_IF_WDATAOV                            (0x1UL << 2)                          /**< Host write buffer overflow                  */
#define _MSC_IF_WDATAOV_SHIFT                     2                                     /**< Shift value for MSC_WDATAOV                 */
#define _MSC_IF_WDATAOV_MASK                      0x4UL                                 /**< Bit mask for MSC_WDATAOV                    */
#define _MSC_IF_WDATAOV_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for MSC_IF                     */
#define MSC_IF_WDATAOV_DEFAULT                    (_MSC_IF_WDATAOV_DEFAULT << 2)        /**< Shifted mode DEFAULT for MSC_IF             */
#define MSC_IF_PWRUPF                             (0x1UL << 8)                          /**< Flash Power Up Sequence Complete Flag       */
#define _MSC_IF_PWRUPF_SHIFT                      8                                     /**< Shift value for MSC_PWRUPF                  */
#define _MSC_IF_PWRUPF_MASK                       0x100UL                               /**< Bit mask for MSC_PWRUPF                     */
#define _MSC_IF_PWRUPF_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MSC_IF                     */
#define MSC_IF_PWRUPF_DEFAULT                     (_MSC_IF_PWRUPF_DEFAULT << 8)         /**< Shifted mode DEFAULT for MSC_IF             */
#define MSC_IF_PWROFF                             (0x1UL << 9)                          /**< Flash Power Off Sequence Complete Flag      */
#define _MSC_IF_PWROFF_SHIFT                      9                                     /**< Shift value for MSC_PWROFF                  */
#define _MSC_IF_PWROFF_MASK                       0x200UL                               /**< Bit mask for MSC_PWROFF                     */
#define _MSC_IF_PWROFF_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MSC_IF                     */
#define MSC_IF_PWROFF_DEFAULT                     (_MSC_IF_PWROFF_DEFAULT << 9)         /**< Shifted mode DEFAULT for MSC_IF             */

/* Bit fields for MSC IEN */
#define _MSC_IEN_RESETVALUE                       0x00000000UL                          /**< Default value for MSC_IEN                   */
#define _MSC_IEN_MASK                             0x00000307UL                          /**< Mask for MSC_IEN                            */
#define MSC_IEN_ERASE                             (0x1UL << 0)                          /**< Erase Done Interrupt enable                 */
#define _MSC_IEN_ERASE_SHIFT                      0                                     /**< Shift value for MSC_ERASE                   */
#define _MSC_IEN_ERASE_MASK                       0x1UL                                 /**< Bit mask for MSC_ERASE                      */
#define _MSC_IEN_ERASE_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MSC_IEN                    */
#define MSC_IEN_ERASE_DEFAULT                     (_MSC_IEN_ERASE_DEFAULT << 0)         /**< Shifted mode DEFAULT for MSC_IEN            */
#define MSC_IEN_WRITE                             (0x1UL << 1)                          /**< Write Done Interrupt enable                 */
#define _MSC_IEN_WRITE_SHIFT                      1                                     /**< Shift value for MSC_WRITE                   */
#define _MSC_IEN_WRITE_MASK                       0x2UL                                 /**< Bit mask for MSC_WRITE                      */
#define _MSC_IEN_WRITE_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MSC_IEN                    */
#define MSC_IEN_WRITE_DEFAULT                     (_MSC_IEN_WRITE_DEFAULT << 1)         /**< Shifted mode DEFAULT for MSC_IEN            */
#define MSC_IEN_WDATAOV                           (0x1UL << 2)                          /**< write data buffer overflow irq enable       */
#define _MSC_IEN_WDATAOV_SHIFT                    2                                     /**< Shift value for MSC_WDATAOV                 */
#define _MSC_IEN_WDATAOV_MASK                     0x4UL                                 /**< Bit mask for MSC_WDATAOV                    */
#define _MSC_IEN_WDATAOV_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for MSC_IEN                    */
#define MSC_IEN_WDATAOV_DEFAULT                   (_MSC_IEN_WDATAOV_DEFAULT << 2)       /**< Shifted mode DEFAULT for MSC_IEN            */
#define MSC_IEN_PWRUPF                            (0x1UL << 8)                          /**< Flash Power Up Seq done irq enable          */
#define _MSC_IEN_PWRUPF_SHIFT                     8                                     /**< Shift value for MSC_PWRUPF                  */
#define _MSC_IEN_PWRUPF_MASK                      0x100UL                               /**< Bit mask for MSC_PWRUPF                     */
#define _MSC_IEN_PWRUPF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for MSC_IEN                    */
#define MSC_IEN_PWRUPF_DEFAULT                    (_MSC_IEN_PWRUPF_DEFAULT << 8)        /**< Shifted mode DEFAULT for MSC_IEN            */
#define MSC_IEN_PWROFF                            (0x1UL << 9)                          /**< Flash Power Off Seq done irq enable         */
#define _MSC_IEN_PWROFF_SHIFT                     9                                     /**< Shift value for MSC_PWROFF                  */
#define _MSC_IEN_PWROFF_MASK                      0x200UL                               /**< Bit mask for MSC_PWROFF                     */
#define _MSC_IEN_PWROFF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for MSC_IEN                    */
#define MSC_IEN_PWROFF_DEFAULT                    (_MSC_IEN_PWROFF_DEFAULT << 9)        /**< Shifted mode DEFAULT for MSC_IEN            */

/* Bit fields for MSC USERDATASIZE */
#define _MSC_USERDATASIZE_RESETVALUE              0x00000004UL                                  /**< Default value for MSC_USERDATASIZE          */
#define _MSC_USERDATASIZE_MASK                    0x0000003FUL                                  /**< Mask for MSC_USERDATASIZE                   */
#define _MSC_USERDATASIZE_USERDATASIZE_SHIFT      0                                             /**< Shift value for MSC_USERDATASIZE            */
#define _MSC_USERDATASIZE_USERDATASIZE_MASK       0x3FUL                                        /**< Bit mask for MSC_USERDATASIZE               */
#define _MSC_USERDATASIZE_USERDATASIZE_DEFAULT    0x00000004UL                                  /**< Mode DEFAULT for MSC_USERDATASIZE           */
#define MSC_USERDATASIZE_USERDATASIZE_DEFAULT     (_MSC_USERDATASIZE_USERDATASIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_USERDATASIZE   */

/* Bit fields for MSC CMD */
#define _MSC_CMD_RESETVALUE                       0x00000000UL                          /**< Default value for MSC_CMD                   */
#define _MSC_CMD_MASK                             0x00000011UL                          /**< Mask for MSC_CMD                            */
#define MSC_CMD_PWRUP                             (0x1UL << 0)                          /**< Flash Power Up Command                      */
#define _MSC_CMD_PWRUP_SHIFT                      0                                     /**< Shift value for MSC_PWRUP                   */
#define _MSC_CMD_PWRUP_MASK                       0x1UL                                 /**< Bit mask for MSC_PWRUP                      */
#define _MSC_CMD_PWRUP_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MSC_CMD                    */
#define MSC_CMD_PWRUP_DEFAULT                     (_MSC_CMD_PWRUP_DEFAULT << 0)         /**< Shifted mode DEFAULT for MSC_CMD            */
#define MSC_CMD_PWROFF                            (0x1UL << 4)                          /**< Flash power off/sleep command               */
#define _MSC_CMD_PWROFF_SHIFT                     4                                     /**< Shift value for MSC_PWROFF                  */
#define _MSC_CMD_PWROFF_MASK                      0x10UL                                /**< Bit mask for MSC_PWROFF                     */
#define _MSC_CMD_PWROFF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for MSC_CMD                    */
#define MSC_CMD_PWROFF_DEFAULT                    (_MSC_CMD_PWROFF_DEFAULT << 4)        /**< Shifted mode DEFAULT for MSC_CMD            */

/* Bit fields for MSC LOCK */
#define _MSC_LOCK_RESETVALUE                      0x00000000UL                          /**< Default value for MSC_LOCK                  */
#define _MSC_LOCK_MASK                            0x0000FFFFUL                          /**< Mask for MSC_LOCK                           */
#define _MSC_LOCK_LOCKKEY_SHIFT                   0                                     /**< Shift value for MSC_LOCKKEY                 */
#define _MSC_LOCK_LOCKKEY_MASK                    0xFFFFUL                              /**< Bit mask for MSC_LOCKKEY                    */
#define _MSC_LOCK_LOCKKEY_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for MSC_LOCK                   */
#define _MSC_LOCK_LOCKKEY_LOCK                    0x00000000UL                          /**< Mode LOCK for MSC_LOCK                      */
#define _MSC_LOCK_LOCKKEY_UNLOCK                  0x00001B71UL                          /**< Mode UNLOCK for MSC_LOCK                    */
#define MSC_LOCK_LOCKKEY_DEFAULT                  (_MSC_LOCK_LOCKKEY_DEFAULT << 0)      /**< Shifted mode DEFAULT for MSC_LOCK           */
#define MSC_LOCK_LOCKKEY_LOCK                     (_MSC_LOCK_LOCKKEY_LOCK << 0)         /**< Shifted mode LOCK for MSC_LOCK              */
#define MSC_LOCK_LOCKKEY_UNLOCK                   (_MSC_LOCK_LOCKKEY_UNLOCK << 0)       /**< Shifted mode UNLOCK for MSC_LOCK            */

/* Bit fields for MSC MISCLOCKWORD */
#define _MSC_MISCLOCKWORD_RESETVALUE              0x00000011UL                               /**< Default value for MSC_MISCLOCKWORD          */
#define _MSC_MISCLOCKWORD_MASK                    0x00000011UL                               /**< Mask for MSC_MISCLOCKWORD                   */
#define MSC_MISCLOCKWORD_MELOCKBIT                (0x1UL << 0)                               /**< Mass Erase Lock                             */
#define _MSC_MISCLOCKWORD_MELOCKBIT_SHIFT         0                                          /**< Shift value for MSC_MELOCKBIT               */
#define _MSC_MISCLOCKWORD_MELOCKBIT_MASK          0x1UL                                      /**< Bit mask for MSC_MELOCKBIT                  */
#define _MSC_MISCLOCKWORD_MELOCKBIT_DEFAULT       0x00000001UL                               /**< Mode DEFAULT for MSC_MISCLOCKWORD           */
#define MSC_MISCLOCKWORD_MELOCKBIT_DEFAULT        (_MSC_MISCLOCKWORD_MELOCKBIT_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_MISCLOCKWORD   */
#define MSC_MISCLOCKWORD_UDLOCKBIT                (0x1UL << 4)                               /**< User Data Lock                              */
#define _MSC_MISCLOCKWORD_UDLOCKBIT_SHIFT         4                                          /**< Shift value for MSC_UDLOCKBIT               */
#define _MSC_MISCLOCKWORD_UDLOCKBIT_MASK          0x10UL                                     /**< Bit mask for MSC_UDLOCKBIT                  */
#define _MSC_MISCLOCKWORD_UDLOCKBIT_DEFAULT       0x00000001UL                               /**< Mode DEFAULT for MSC_MISCLOCKWORD           */
#define MSC_MISCLOCKWORD_UDLOCKBIT_DEFAULT        (_MSC_MISCLOCKWORD_UDLOCKBIT_DEFAULT << 4) /**< Shifted mode DEFAULT for MSC_MISCLOCKWORD   */

/* Bit fields for MSC PWRCTRL */
#define _MSC_PWRCTRL_RESETVALUE                   0x00100002UL                                  /**< Default value for MSC_PWRCTRL               */
#define _MSC_PWRCTRL_MASK                         0x00FF0013UL                                  /**< Mask for MSC_PWRCTRL                        */
#define MSC_PWRCTRL_PWROFFONEM1ENTRY              (0x1UL << 0)                                  /**< Power down Flash macro when enter EM1       */
#define _MSC_PWRCTRL_PWROFFONEM1ENTRY_SHIFT       0                                             /**< Shift value for MSC_PWROFFONEM1ENTRY        */
#define _MSC_PWRCTRL_PWROFFONEM1ENTRY_MASK        0x1UL                                         /**< Bit mask for MSC_PWROFFONEM1ENTRY           */
#define _MSC_PWRCTRL_PWROFFONEM1ENTRY_DEFAULT     0x00000000UL                                  /**< Mode DEFAULT for MSC_PWRCTRL                */
#define MSC_PWRCTRL_PWROFFONEM1ENTRY_DEFAULT      (_MSC_PWRCTRL_PWROFFONEM1ENTRY_DEFAULT << 0)  /**< Shifted mode DEFAULT for MSC_PWRCTRL        */
#define MSC_PWRCTRL_PWROFFONEM1PENTRY             (0x1UL << 1)                                  /**< Power down Flash macro when enter EM1P      */
#define _MSC_PWRCTRL_PWROFFONEM1PENTRY_SHIFT      1                                             /**< Shift value for MSC_PWROFFONEM1PENTRY       */
#define _MSC_PWRCTRL_PWROFFONEM1PENTRY_MASK       0x2UL                                         /**< Bit mask for MSC_PWROFFONEM1PENTRY          */
#define _MSC_PWRCTRL_PWROFFONEM1PENTRY_DEFAULT    0x00000001UL                                  /**< Mode DEFAULT for MSC_PWRCTRL                */
#define MSC_PWRCTRL_PWROFFONEM1PENTRY_DEFAULT     (_MSC_PWRCTRL_PWROFFONEM1PENTRY_DEFAULT << 1) /**< Shifted mode DEFAULT for MSC_PWRCTRL        */
#define MSC_PWRCTRL_PWROFFENTRYAGAIN              (0x1UL << 4)                                  /**< POWER down flash again in EM1/EM1p          */
#define _MSC_PWRCTRL_PWROFFENTRYAGAIN_SHIFT       4                                             /**< Shift value for MSC_PWROFFENTRYAGAIN        */
#define _MSC_PWRCTRL_PWROFFENTRYAGAIN_MASK        0x10UL                                        /**< Bit mask for MSC_PWROFFENTRYAGAIN           */
#define _MSC_PWRCTRL_PWROFFENTRYAGAIN_DEFAULT     0x00000000UL                                  /**< Mode DEFAULT for MSC_PWRCTRL                */
#define MSC_PWRCTRL_PWROFFENTRYAGAIN_DEFAULT      (_MSC_PWRCTRL_PWROFFENTRYAGAIN_DEFAULT << 4)  /**< Shifted mode DEFAULT for MSC_PWRCTRL        */
#define _MSC_PWRCTRL_PWROFFDLY_SHIFT              16                                            /**< Shift value for MSC_PWROFFDLY               */
#define _MSC_PWRCTRL_PWROFFDLY_MASK               0xFF0000UL                                    /**< Bit mask for MSC_PWROFFDLY                  */
#define _MSC_PWRCTRL_PWROFFDLY_DEFAULT            0x00000010UL                                  /**< Mode DEFAULT for MSC_PWRCTRL                */
#define MSC_PWRCTRL_PWROFFDLY_DEFAULT             (_MSC_PWRCTRL_PWROFFDLY_DEFAULT << 16)        /**< Shifted mode DEFAULT for MSC_PWRCTRL        */

/* Bit fields for MSC PAGELOCK0 */
#define _MSC_PAGELOCK0_RESETVALUE                 0x00000000UL                          /**< Default value for MSC_PAGELOCK0             */
#define _MSC_PAGELOCK0_MASK                       0xFFFFFFFFUL                          /**< Mask for MSC_PAGELOCK0                      */
#define _MSC_PAGELOCK0_LOCKBIT_SHIFT              0                                     /**< Shift value for MSC_LOCKBIT                 */
#define _MSC_PAGELOCK0_LOCKBIT_MASK               0xFFFFFFFFUL                          /**< Bit mask for MSC_LOCKBIT                    */
#define _MSC_PAGELOCK0_LOCKBIT_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MSC_PAGELOCK0              */
#define MSC_PAGELOCK0_LOCKBIT_DEFAULT             (_MSC_PAGELOCK0_LOCKBIT_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_PAGELOCK0      */

/* Bit fields for MSC PAGELOCK1 */
#define _MSC_PAGELOCK1_RESETVALUE                 0x00000000UL                          /**< Default value for MSC_PAGELOCK1             */
#define _MSC_PAGELOCK1_MASK                       0xFFFFFFFFUL                          /**< Mask for MSC_PAGELOCK1                      */
#define _MSC_PAGELOCK1_LOCKBIT_SHIFT              0                                     /**< Shift value for MSC_LOCKBIT                 */
#define _MSC_PAGELOCK1_LOCKBIT_MASK               0xFFFFFFFFUL                          /**< Bit mask for MSC_LOCKBIT                    */
#define _MSC_PAGELOCK1_LOCKBIT_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MSC_PAGELOCK1              */
#define MSC_PAGELOCK1_LOCKBIT_DEFAULT             (_MSC_PAGELOCK1_LOCKBIT_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_PAGELOCK1      */

/** @} End of group EFR32ZG23_MSC_BitFields */
/** @} End of group EFR32ZG23_MSC */
/** @} End of group Parts */

#endif /* EFR32ZG23_MSC_H */
#ifdef __cplusplus
}
#endif

