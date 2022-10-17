/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 ACMP register and bit field definitions
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

#ifndef EFR32ZG23_ACMP_H
#define EFR32ZG23_ACMP_H
#define ACMP_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_ACMP ACMP
 * @{
 * @brief EFR32ZG23 ACMP Register Declaration.
 *****************************************************************************/

/** ACMP Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t EN;                            /**< ACMP enable                                        */
  __IOM uint32_t SWRST;                         /**< Software reset                                     */
  __IOM uint32_t CFG;                           /**< Configuration register                             */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IOM uint32_t INPUTCTRL;                     /**< Input Control Register                             */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY;                      /**< Syncbusy                                           */
  uint32_t       RESERVED0[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t EN_SET;                        /**< ACMP enable                                        */
  __IOM uint32_t SWRST_SET;                     /**< Software reset                                     */
  __IOM uint32_t CFG_SET;                       /**< Configuration register                             */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IOM uint32_t INPUTCTRL_SET;                 /**< Input Control Register                             */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Syncbusy                                           */
  uint32_t       RESERVED1[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t EN_CLR;                        /**< ACMP enable                                        */
  __IOM uint32_t SWRST_CLR;                     /**< Software reset                                     */
  __IOM uint32_t CFG_CLR;                       /**< Configuration register                             */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IOM uint32_t INPUTCTRL_CLR;                 /**< Input Control Register                             */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Syncbusy                                           */
  uint32_t       RESERVED2[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t EN_TGL;                        /**< ACMP enable                                        */
  __IOM uint32_t SWRST_TGL;                     /**< Software reset                                     */
  __IOM uint32_t CFG_TGL;                       /**< Configuration register                             */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IOM uint32_t INPUTCTRL_TGL;                 /**< Input Control Register                             */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Syncbusy                                           */
} ACMP_TypeDef;
/** @} End of group EFR32ZG23_ACMP */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_ACMP
 * @{
 * @defgroup EFR32ZG23_ACMP_BitFields ACMP Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for ACMP IPVERSION */
#define _ACMP_IPVERSION_RESETVALUE               0x00000001UL                             /**< Default value for ACMP_IPVERSION            */
#define _ACMP_IPVERSION_MASK                     0xFFFFFFFFUL                             /**< Mask for ACMP_IPVERSION                     */
#define _ACMP_IPVERSION_IPVERSION_SHIFT          0                                        /**< Shift value for ACMP_IPVERSION              */
#define _ACMP_IPVERSION_IPVERSION_MASK           0xFFFFFFFFUL                             /**< Bit mask for ACMP_IPVERSION                 */
#define _ACMP_IPVERSION_IPVERSION_DEFAULT        0x00000001UL                             /**< Mode DEFAULT for ACMP_IPVERSION             */
#define ACMP_IPVERSION_IPVERSION_DEFAULT         (_ACMP_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for ACMP_IPVERSION     */

/* Bit fields for ACMP EN */
#define _ACMP_EN_RESETVALUE                      0x00000000UL                           /**< Default value for ACMP_EN                   */
#define _ACMP_EN_MASK                            0x00000003UL                           /**< Mask for ACMP_EN                            */
#define ACMP_EN_EN                               (0x1UL << 0)                           /**< Module enable                               */
#define _ACMP_EN_EN_SHIFT                        0                                      /**< Shift value for ACMP_EN                     */
#define _ACMP_EN_EN_MASK                         0x1UL                                  /**< Bit mask for ACMP_EN                        */
#define _ACMP_EN_EN_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for ACMP_EN                    */
#define ACMP_EN_EN_DEFAULT                       (_ACMP_EN_EN_DEFAULT << 0)             /**< Shifted mode DEFAULT for ACMP_EN            */
#define ACMP_EN_DISABLING                        (0x1UL << 1)                           /**< Disablement busy status                     */
#define _ACMP_EN_DISABLING_SHIFT                 1                                      /**< Shift value for ACMP_DISABLING              */
#define _ACMP_EN_DISABLING_MASK                  0x2UL                                  /**< Bit mask for ACMP_DISABLING                 */
#define _ACMP_EN_DISABLING_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for ACMP_EN                    */
#define ACMP_EN_DISABLING_DEFAULT                (_ACMP_EN_DISABLING_DEFAULT << 1)      /**< Shifted mode DEFAULT for ACMP_EN            */

/* Bit fields for ACMP SWRST */
#define _ACMP_SWRST_RESETVALUE                   0x00000000UL                           /**< Default value for ACMP_SWRST                */
#define _ACMP_SWRST_MASK                         0x00000003UL                           /**< Mask for ACMP_SWRST                         */
#define ACMP_SWRST_SWRST                         (0x1UL << 0)                           /**< Software reset                              */
#define _ACMP_SWRST_SWRST_SHIFT                  0                                      /**< Shift value for ACMP_SWRST                  */
#define _ACMP_SWRST_SWRST_MASK                   0x1UL                                  /**< Bit mask for ACMP_SWRST                     */
#define _ACMP_SWRST_SWRST_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for ACMP_SWRST                 */
#define ACMP_SWRST_SWRST_DEFAULT                 (_ACMP_SWRST_SWRST_DEFAULT << 0)       /**< Shifted mode DEFAULT for ACMP_SWRST         */
#define ACMP_SWRST_RESETTING                     (0x1UL << 1)                           /**< Software reset busy status                  */
#define _ACMP_SWRST_RESETTING_SHIFT              1                                      /**< Shift value for ACMP_RESETTING              */
#define _ACMP_SWRST_RESETTING_MASK               0x2UL                                  /**< Bit mask for ACMP_RESETTING                 */
#define _ACMP_SWRST_RESETTING_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for ACMP_SWRST                 */
#define ACMP_SWRST_RESETTING_DEFAULT             (_ACMP_SWRST_RESETTING_DEFAULT << 1)   /**< Shifted mode DEFAULT for ACMP_SWRST         */

/* Bit fields for ACMP CFG */
#define _ACMP_CFG_RESETVALUE                     0x00000004UL                           /**< Default value for ACMP_CFG                  */
#define _ACMP_CFG_MASK                           0x00030F07UL                           /**< Mask for ACMP_CFG                           */
#define _ACMP_CFG_BIAS_SHIFT                     0                                      /**< Shift value for ACMP_BIAS                   */
#define _ACMP_CFG_BIAS_MASK                      0x7UL                                  /**< Bit mask for ACMP_BIAS                      */
#define _ACMP_CFG_BIAS_DEFAULT                   0x00000004UL                           /**< Mode DEFAULT for ACMP_CFG                   */
#define ACMP_CFG_BIAS_DEFAULT                    (_ACMP_CFG_BIAS_DEFAULT << 0)          /**< Shifted mode DEFAULT for ACMP_CFG           */
#define _ACMP_CFG_HYST_SHIFT                     8                                      /**< Shift value for ACMP_HYST                   */
#define _ACMP_CFG_HYST_MASK                      0xF00UL                                /**< Bit mask for ACMP_HYST                      */
#define _ACMP_CFG_HYST_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ACMP_CFG                   */
#define _ACMP_CFG_HYST_DISABLED                  0x00000000UL                           /**< Mode DISABLED for ACMP_CFG                  */
#define _ACMP_CFG_HYST_SYM10MV                   0x00000001UL                           /**< Mode SYM10MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_SYM20MV                   0x00000002UL                           /**< Mode SYM20MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_SYM30MV                   0x00000003UL                           /**< Mode SYM30MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_POS10MV                   0x00000004UL                           /**< Mode POS10MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_POS20MV                   0x00000005UL                           /**< Mode POS20MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_POS30MV                   0x00000006UL                           /**< Mode POS30MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_NEG10MV                   0x00000008UL                           /**< Mode NEG10MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_NEG20MV                   0x00000009UL                           /**< Mode NEG20MV for ACMP_CFG                   */
#define _ACMP_CFG_HYST_NEG30MV                   0x0000000AUL                           /**< Mode NEG30MV for ACMP_CFG                   */
#define ACMP_CFG_HYST_DEFAULT                    (_ACMP_CFG_HYST_DEFAULT << 8)          /**< Shifted mode DEFAULT for ACMP_CFG           */
#define ACMP_CFG_HYST_DISABLED                   (_ACMP_CFG_HYST_DISABLED << 8)         /**< Shifted mode DISABLED for ACMP_CFG          */
#define ACMP_CFG_HYST_SYM10MV                    (_ACMP_CFG_HYST_SYM10MV << 8)          /**< Shifted mode SYM10MV for ACMP_CFG           */
#define ACMP_CFG_HYST_SYM20MV                    (_ACMP_CFG_HYST_SYM20MV << 8)          /**< Shifted mode SYM20MV for ACMP_CFG           */
#define ACMP_CFG_HYST_SYM30MV                    (_ACMP_CFG_HYST_SYM30MV << 8)          /**< Shifted mode SYM30MV for ACMP_CFG           */
#define ACMP_CFG_HYST_POS10MV                    (_ACMP_CFG_HYST_POS10MV << 8)          /**< Shifted mode POS10MV for ACMP_CFG           */
#define ACMP_CFG_HYST_POS20MV                    (_ACMP_CFG_HYST_POS20MV << 8)          /**< Shifted mode POS20MV for ACMP_CFG           */
#define ACMP_CFG_HYST_POS30MV                    (_ACMP_CFG_HYST_POS30MV << 8)          /**< Shifted mode POS30MV for ACMP_CFG           */
#define ACMP_CFG_HYST_NEG10MV                    (_ACMP_CFG_HYST_NEG10MV << 8)          /**< Shifted mode NEG10MV for ACMP_CFG           */
#define ACMP_CFG_HYST_NEG20MV                    (_ACMP_CFG_HYST_NEG20MV << 8)          /**< Shifted mode NEG20MV for ACMP_CFG           */
#define ACMP_CFG_HYST_NEG30MV                    (_ACMP_CFG_HYST_NEG30MV << 8)          /**< Shifted mode NEG30MV for ACMP_CFG           */
#define ACMP_CFG_INPUTRANGE                      (0x1UL << 16)                          /**< Input Range                                 */
#define _ACMP_CFG_INPUTRANGE_SHIFT               16                                     /**< Shift value for ACMP_INPUTRANGE             */
#define _ACMP_CFG_INPUTRANGE_MASK                0x10000UL                              /**< Bit mask for ACMP_INPUTRANGE                */
#define _ACMP_CFG_INPUTRANGE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for ACMP_CFG                   */
#define _ACMP_CFG_INPUTRANGE_FULL                0x00000000UL                           /**< Mode FULL for ACMP_CFG                      */
#define _ACMP_CFG_INPUTRANGE_REDUCED             0x00000001UL                           /**< Mode REDUCED for ACMP_CFG                   */
#define ACMP_CFG_INPUTRANGE_DEFAULT              (_ACMP_CFG_INPUTRANGE_DEFAULT << 16)   /**< Shifted mode DEFAULT for ACMP_CFG           */
#define ACMP_CFG_INPUTRANGE_FULL                 (_ACMP_CFG_INPUTRANGE_FULL << 16)      /**< Shifted mode FULL for ACMP_CFG              */
#define ACMP_CFG_INPUTRANGE_REDUCED              (_ACMP_CFG_INPUTRANGE_REDUCED << 16)   /**< Shifted mode REDUCED for ACMP_CFG           */
#define ACMP_CFG_ACCURACY                        (0x1UL << 17)                          /**< ACMP accuracy mode                          */
#define _ACMP_CFG_ACCURACY_SHIFT                 17                                     /**< Shift value for ACMP_ACCURACY               */
#define _ACMP_CFG_ACCURACY_MASK                  0x20000UL                              /**< Bit mask for ACMP_ACCURACY                  */
#define _ACMP_CFG_ACCURACY_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for ACMP_CFG                   */
#define _ACMP_CFG_ACCURACY_LOW                   0x00000000UL                           /**< Mode LOW for ACMP_CFG                       */
#define _ACMP_CFG_ACCURACY_HIGH                  0x00000001UL                           /**< Mode HIGH for ACMP_CFG                      */
#define ACMP_CFG_ACCURACY_DEFAULT                (_ACMP_CFG_ACCURACY_DEFAULT << 17)     /**< Shifted mode DEFAULT for ACMP_CFG           */
#define ACMP_CFG_ACCURACY_LOW                    (_ACMP_CFG_ACCURACY_LOW << 17)         /**< Shifted mode LOW for ACMP_CFG               */
#define ACMP_CFG_ACCURACY_HIGH                   (_ACMP_CFG_ACCURACY_HIGH << 17)        /**< Shifted mode HIGH for ACMP_CFG              */

/* Bit fields for ACMP CTRL */
#define _ACMP_CTRL_RESETVALUE                    0x00000000UL                           /**< Default value for ACMP_CTRL                 */
#define _ACMP_CTRL_MASK                          0x00000003UL                           /**< Mask for ACMP_CTRL                          */
#define ACMP_CTRL_NOTRDYVAL                      (0x1UL << 0)                           /**< Not Ready Value                             */
#define _ACMP_CTRL_NOTRDYVAL_SHIFT               0                                      /**< Shift value for ACMP_NOTRDYVAL              */
#define _ACMP_CTRL_NOTRDYVAL_MASK                0x1UL                                  /**< Bit mask for ACMP_NOTRDYVAL                 */
#define _ACMP_CTRL_NOTRDYVAL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for ACMP_CTRL                  */
#define _ACMP_CTRL_NOTRDYVAL_LOW                 0x00000000UL                           /**< Mode LOW for ACMP_CTRL                      */
#define _ACMP_CTRL_NOTRDYVAL_HIGH                0x00000001UL                           /**< Mode HIGH for ACMP_CTRL                     */
#define ACMP_CTRL_NOTRDYVAL_DEFAULT              (_ACMP_CTRL_NOTRDYVAL_DEFAULT << 0)    /**< Shifted mode DEFAULT for ACMP_CTRL          */
#define ACMP_CTRL_NOTRDYVAL_LOW                  (_ACMP_CTRL_NOTRDYVAL_LOW << 0)        /**< Shifted mode LOW for ACMP_CTRL              */
#define ACMP_CTRL_NOTRDYVAL_HIGH                 (_ACMP_CTRL_NOTRDYVAL_HIGH << 0)       /**< Shifted mode HIGH for ACMP_CTRL             */
#define ACMP_CTRL_GPIOINV                        (0x1UL << 1)                           /**< Comparator GPIO Output Invert               */
#define _ACMP_CTRL_GPIOINV_SHIFT                 1                                      /**< Shift value for ACMP_GPIOINV                */
#define _ACMP_CTRL_GPIOINV_MASK                  0x2UL                                  /**< Bit mask for ACMP_GPIOINV                   */
#define _ACMP_CTRL_GPIOINV_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for ACMP_CTRL                  */
#define _ACMP_CTRL_GPIOINV_NOTINV                0x00000000UL                           /**< Mode NOTINV for ACMP_CTRL                   */
#define _ACMP_CTRL_GPIOINV_INV                   0x00000001UL                           /**< Mode INV for ACMP_CTRL                      */
#define ACMP_CTRL_GPIOINV_DEFAULT                (_ACMP_CTRL_GPIOINV_DEFAULT << 1)      /**< Shifted mode DEFAULT for ACMP_CTRL          */
#define ACMP_CTRL_GPIOINV_NOTINV                 (_ACMP_CTRL_GPIOINV_NOTINV << 1)       /**< Shifted mode NOTINV for ACMP_CTRL           */
#define ACMP_CTRL_GPIOINV_INV                    (_ACMP_CTRL_GPIOINV_INV << 1)          /**< Shifted mode INV for ACMP_CTRL              */

/* Bit fields for ACMP INPUTCTRL */
#define _ACMP_INPUTCTRL_RESETVALUE               0x00000000UL                                 /**< Default value for ACMP_INPUTCTRL            */
#define _ACMP_INPUTCTRL_MASK                     0x703FFFFFUL                                 /**< Mask for ACMP_INPUTCTRL                     */
#define _ACMP_INPUTCTRL_POSSEL_SHIFT             0                                            /**< Shift value for ACMP_POSSEL                 */
#define _ACMP_INPUTCTRL_POSSEL_MASK              0xFFUL                                       /**< Bit mask for ACMP_POSSEL                    */
#define _ACMP_INPUTCTRL_POSSEL_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for ACMP_INPUTCTRL             */
#define _ACMP_INPUTCTRL_POSSEL_VSS               0x00000000UL                                 /**< Mode VSS for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_VREFDIVAVDD       0x00000010UL                                 /**< Mode VREFDIVAVDD for ACMP_INPUTCTRL         */
#define _ACMP_INPUTCTRL_POSSEL_VREFDIVAVDDLP     0x00000011UL                                 /**< Mode VREFDIVAVDDLP for ACMP_INPUTCTRL       */
#define _ACMP_INPUTCTRL_POSSEL_VREFDIV1V25       0x00000012UL                                 /**< Mode VREFDIV1V25 for ACMP_INPUTCTRL         */
#define _ACMP_INPUTCTRL_POSSEL_VREFDIV1V25LP     0x00000013UL                                 /**< Mode VREFDIV1V25LP for ACMP_INPUTCTRL       */
#define _ACMP_INPUTCTRL_POSSEL_VREFDIV2V5        0x00000014UL                                 /**< Mode VREFDIV2V5 for ACMP_INPUTCTRL          */
#define _ACMP_INPUTCTRL_POSSEL_VREFDIV2V5LP      0x00000015UL                                 /**< Mode VREFDIV2V5LP for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4      0x00000020UL                                 /**< Mode VSENSE01DIV4 for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4LP    0x00000021UL                                 /**< Mode VSENSE01DIV4LP for ACMP_INPUTCTRL      */
#define _ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4      0x00000022UL                                 /**< Mode VSENSE11DIV4 for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4LP    0x00000023UL                                 /**< Mode VSENSE11DIV4LP for ACMP_INPUTCTRL      */
#define _ACMP_INPUTCTRL_POSSEL_VDACOUT0          0x00000040UL                                 /**< Mode VDACOUT0 for ACMP_INPUTCTRL            */
#define _ACMP_INPUTCTRL_POSSEL_VDACOUT1          0x00000041UL                                 /**< Mode VDACOUT1 for ACMP_INPUTCTRL            */
#define _ACMP_INPUTCTRL_POSSEL_EXTPA             0x00000050UL                                 /**< Mode EXTPA for ACMP_INPUTCTRL               */
#define _ACMP_INPUTCTRL_POSSEL_EXTPB             0x00000051UL                                 /**< Mode EXTPB for ACMP_INPUTCTRL               */
#define _ACMP_INPUTCTRL_POSSEL_EXTPC             0x00000052UL                                 /**< Mode EXTPC for ACMP_INPUTCTRL               */
#define _ACMP_INPUTCTRL_POSSEL_EXTPD             0x00000053UL                                 /**< Mode EXTPD for ACMP_INPUTCTRL               */
#define _ACMP_INPUTCTRL_POSSEL_PA0               0x00000080UL                                 /**< Mode PA0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA1               0x00000081UL                                 /**< Mode PA1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA2               0x00000082UL                                 /**< Mode PA2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA3               0x00000083UL                                 /**< Mode PA3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA4               0x00000084UL                                 /**< Mode PA4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA5               0x00000085UL                                 /**< Mode PA5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA6               0x00000086UL                                 /**< Mode PA6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA7               0x00000087UL                                 /**< Mode PA7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA8               0x00000088UL                                 /**< Mode PA8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA9               0x00000089UL                                 /**< Mode PA9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PA10              0x0000008AUL                                 /**< Mode PA10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PA11              0x0000008BUL                                 /**< Mode PA11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PA12              0x0000008CUL                                 /**< Mode PA12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PA13              0x0000008DUL                                 /**< Mode PA13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PA14              0x0000008EUL                                 /**< Mode PA14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PA15              0x0000008FUL                                 /**< Mode PA15 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PB0               0x00000090UL                                 /**< Mode PB0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB1               0x00000091UL                                 /**< Mode PB1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB2               0x00000092UL                                 /**< Mode PB2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB3               0x00000093UL                                 /**< Mode PB3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB4               0x00000094UL                                 /**< Mode PB4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB5               0x00000095UL                                 /**< Mode PB5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB6               0x00000096UL                                 /**< Mode PB6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB7               0x00000097UL                                 /**< Mode PB7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB8               0x00000098UL                                 /**< Mode PB8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB9               0x00000099UL                                 /**< Mode PB9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PB10              0x0000009AUL                                 /**< Mode PB10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PB11              0x0000009BUL                                 /**< Mode PB11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PB12              0x0000009CUL                                 /**< Mode PB12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PB13              0x0000009DUL                                 /**< Mode PB13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PB14              0x0000009EUL                                 /**< Mode PB14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PB15              0x0000009FUL                                 /**< Mode PB15 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PC0               0x000000A0UL                                 /**< Mode PC0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC1               0x000000A1UL                                 /**< Mode PC1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC2               0x000000A2UL                                 /**< Mode PC2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC3               0x000000A3UL                                 /**< Mode PC3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC4               0x000000A4UL                                 /**< Mode PC4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC5               0x000000A5UL                                 /**< Mode PC5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC6               0x000000A6UL                                 /**< Mode PC6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC7               0x000000A7UL                                 /**< Mode PC7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC8               0x000000A8UL                                 /**< Mode PC8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC9               0x000000A9UL                                 /**< Mode PC9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PC10              0x000000AAUL                                 /**< Mode PC10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PC11              0x000000ABUL                                 /**< Mode PC11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PC12              0x000000ACUL                                 /**< Mode PC12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PC13              0x000000ADUL                                 /**< Mode PC13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PC14              0x000000AEUL                                 /**< Mode PC14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PC15              0x000000AFUL                                 /**< Mode PC15 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PD0               0x000000B0UL                                 /**< Mode PD0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD1               0x000000B1UL                                 /**< Mode PD1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD2               0x000000B2UL                                 /**< Mode PD2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD3               0x000000B3UL                                 /**< Mode PD3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD4               0x000000B4UL                                 /**< Mode PD4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD5               0x000000B5UL                                 /**< Mode PD5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD6               0x000000B6UL                                 /**< Mode PD6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD7               0x000000B7UL                                 /**< Mode PD7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD8               0x000000B8UL                                 /**< Mode PD8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD9               0x000000B9UL                                 /**< Mode PD9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_POSSEL_PD10              0x000000BAUL                                 /**< Mode PD10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PD11              0x000000BBUL                                 /**< Mode PD11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PD12              0x000000BCUL                                 /**< Mode PD12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PD13              0x000000BDUL                                 /**< Mode PD13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PD14              0x000000BEUL                                 /**< Mode PD14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_POSSEL_PD15              0x000000BFUL                                 /**< Mode PD15 for ACMP_INPUTCTRL                */
#define ACMP_INPUTCTRL_POSSEL_DEFAULT            (_ACMP_INPUTCTRL_POSSEL_DEFAULT << 0)        /**< Shifted mode DEFAULT for ACMP_INPUTCTRL     */
#define ACMP_INPUTCTRL_POSSEL_VSS                (_ACMP_INPUTCTRL_POSSEL_VSS << 0)            /**< Shifted mode VSS for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_VREFDIVAVDD        (_ACMP_INPUTCTRL_POSSEL_VREFDIVAVDD << 0)    /**< Shifted mode VREFDIVAVDD for ACMP_INPUTCTRL */
#define ACMP_INPUTCTRL_POSSEL_VREFDIVAVDDLP      (_ACMP_INPUTCTRL_POSSEL_VREFDIVAVDDLP << 0)  /**< Shifted mode VREFDIVAVDDLP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VREFDIV1V25        (_ACMP_INPUTCTRL_POSSEL_VREFDIV1V25 << 0)    /**< Shifted mode VREFDIV1V25 for ACMP_INPUTCTRL */
#define ACMP_INPUTCTRL_POSSEL_VREFDIV1V25LP      (_ACMP_INPUTCTRL_POSSEL_VREFDIV1V25LP << 0)  /**< Shifted mode VREFDIV1V25LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VREFDIV2V5         (_ACMP_INPUTCTRL_POSSEL_VREFDIV2V5 << 0)     /**< Shifted mode VREFDIV2V5 for ACMP_INPUTCTRL  */
#define ACMP_INPUTCTRL_POSSEL_VREFDIV2V5LP       (_ACMP_INPUTCTRL_POSSEL_VREFDIV2V5LP << 0)   /**< Shifted mode VREFDIV2V5LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4       (_ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4 << 0)   /**< Shifted mode VSENSE01DIV4 for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4LP     (_ACMP_INPUTCTRL_POSSEL_VSENSE01DIV4LP << 0) /**< Shifted mode VSENSE01DIV4LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4       (_ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4 << 0)   /**< Shifted mode VSENSE11DIV4 for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4LP     (_ACMP_INPUTCTRL_POSSEL_VSENSE11DIV4LP << 0) /**< Shifted mode VSENSE11DIV4LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_POSSEL_VDACOUT0           (_ACMP_INPUTCTRL_POSSEL_VDACOUT0 << 0)       /**< Shifted mode VDACOUT0 for ACMP_INPUTCTRL    */
#define ACMP_INPUTCTRL_POSSEL_VDACOUT1           (_ACMP_INPUTCTRL_POSSEL_VDACOUT1 << 0)       /**< Shifted mode VDACOUT1 for ACMP_INPUTCTRL    */
#define ACMP_INPUTCTRL_POSSEL_EXTPA              (_ACMP_INPUTCTRL_POSSEL_EXTPA << 0)          /**< Shifted mode EXTPA for ACMP_INPUTCTRL       */
#define ACMP_INPUTCTRL_POSSEL_EXTPB              (_ACMP_INPUTCTRL_POSSEL_EXTPB << 0)          /**< Shifted mode EXTPB for ACMP_INPUTCTRL       */
#define ACMP_INPUTCTRL_POSSEL_EXTPC              (_ACMP_INPUTCTRL_POSSEL_EXTPC << 0)          /**< Shifted mode EXTPC for ACMP_INPUTCTRL       */
#define ACMP_INPUTCTRL_POSSEL_EXTPD              (_ACMP_INPUTCTRL_POSSEL_EXTPD << 0)          /**< Shifted mode EXTPD for ACMP_INPUTCTRL       */
#define ACMP_INPUTCTRL_POSSEL_PA0                (_ACMP_INPUTCTRL_POSSEL_PA0 << 0)            /**< Shifted mode PA0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA1                (_ACMP_INPUTCTRL_POSSEL_PA1 << 0)            /**< Shifted mode PA1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA2                (_ACMP_INPUTCTRL_POSSEL_PA2 << 0)            /**< Shifted mode PA2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA3                (_ACMP_INPUTCTRL_POSSEL_PA3 << 0)            /**< Shifted mode PA3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA4                (_ACMP_INPUTCTRL_POSSEL_PA4 << 0)            /**< Shifted mode PA4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA5                (_ACMP_INPUTCTRL_POSSEL_PA5 << 0)            /**< Shifted mode PA5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA6                (_ACMP_INPUTCTRL_POSSEL_PA6 << 0)            /**< Shifted mode PA6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA7                (_ACMP_INPUTCTRL_POSSEL_PA7 << 0)            /**< Shifted mode PA7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA8                (_ACMP_INPUTCTRL_POSSEL_PA8 << 0)            /**< Shifted mode PA8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA9                (_ACMP_INPUTCTRL_POSSEL_PA9 << 0)            /**< Shifted mode PA9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PA10               (_ACMP_INPUTCTRL_POSSEL_PA10 << 0)           /**< Shifted mode PA10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PA11               (_ACMP_INPUTCTRL_POSSEL_PA11 << 0)           /**< Shifted mode PA11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PA12               (_ACMP_INPUTCTRL_POSSEL_PA12 << 0)           /**< Shifted mode PA12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PA13               (_ACMP_INPUTCTRL_POSSEL_PA13 << 0)           /**< Shifted mode PA13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PA14               (_ACMP_INPUTCTRL_POSSEL_PA14 << 0)           /**< Shifted mode PA14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PA15               (_ACMP_INPUTCTRL_POSSEL_PA15 << 0)           /**< Shifted mode PA15 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PB0                (_ACMP_INPUTCTRL_POSSEL_PB0 << 0)            /**< Shifted mode PB0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB1                (_ACMP_INPUTCTRL_POSSEL_PB1 << 0)            /**< Shifted mode PB1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB2                (_ACMP_INPUTCTRL_POSSEL_PB2 << 0)            /**< Shifted mode PB2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB3                (_ACMP_INPUTCTRL_POSSEL_PB3 << 0)            /**< Shifted mode PB3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB4                (_ACMP_INPUTCTRL_POSSEL_PB4 << 0)            /**< Shifted mode PB4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB5                (_ACMP_INPUTCTRL_POSSEL_PB5 << 0)            /**< Shifted mode PB5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB6                (_ACMP_INPUTCTRL_POSSEL_PB6 << 0)            /**< Shifted mode PB6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB7                (_ACMP_INPUTCTRL_POSSEL_PB7 << 0)            /**< Shifted mode PB7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB8                (_ACMP_INPUTCTRL_POSSEL_PB8 << 0)            /**< Shifted mode PB8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB9                (_ACMP_INPUTCTRL_POSSEL_PB9 << 0)            /**< Shifted mode PB9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PB10               (_ACMP_INPUTCTRL_POSSEL_PB10 << 0)           /**< Shifted mode PB10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PB11               (_ACMP_INPUTCTRL_POSSEL_PB11 << 0)           /**< Shifted mode PB11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PB12               (_ACMP_INPUTCTRL_POSSEL_PB12 << 0)           /**< Shifted mode PB12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PB13               (_ACMP_INPUTCTRL_POSSEL_PB13 << 0)           /**< Shifted mode PB13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PB14               (_ACMP_INPUTCTRL_POSSEL_PB14 << 0)           /**< Shifted mode PB14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PB15               (_ACMP_INPUTCTRL_POSSEL_PB15 << 0)           /**< Shifted mode PB15 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PC0                (_ACMP_INPUTCTRL_POSSEL_PC0 << 0)            /**< Shifted mode PC0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC1                (_ACMP_INPUTCTRL_POSSEL_PC1 << 0)            /**< Shifted mode PC1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC2                (_ACMP_INPUTCTRL_POSSEL_PC2 << 0)            /**< Shifted mode PC2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC3                (_ACMP_INPUTCTRL_POSSEL_PC3 << 0)            /**< Shifted mode PC3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC4                (_ACMP_INPUTCTRL_POSSEL_PC4 << 0)            /**< Shifted mode PC4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC5                (_ACMP_INPUTCTRL_POSSEL_PC5 << 0)            /**< Shifted mode PC5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC6                (_ACMP_INPUTCTRL_POSSEL_PC6 << 0)            /**< Shifted mode PC6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC7                (_ACMP_INPUTCTRL_POSSEL_PC7 << 0)            /**< Shifted mode PC7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC8                (_ACMP_INPUTCTRL_POSSEL_PC8 << 0)            /**< Shifted mode PC8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC9                (_ACMP_INPUTCTRL_POSSEL_PC9 << 0)            /**< Shifted mode PC9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PC10               (_ACMP_INPUTCTRL_POSSEL_PC10 << 0)           /**< Shifted mode PC10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PC11               (_ACMP_INPUTCTRL_POSSEL_PC11 << 0)           /**< Shifted mode PC11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PC12               (_ACMP_INPUTCTRL_POSSEL_PC12 << 0)           /**< Shifted mode PC12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PC13               (_ACMP_INPUTCTRL_POSSEL_PC13 << 0)           /**< Shifted mode PC13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PC14               (_ACMP_INPUTCTRL_POSSEL_PC14 << 0)           /**< Shifted mode PC14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PC15               (_ACMP_INPUTCTRL_POSSEL_PC15 << 0)           /**< Shifted mode PC15 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PD0                (_ACMP_INPUTCTRL_POSSEL_PD0 << 0)            /**< Shifted mode PD0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD1                (_ACMP_INPUTCTRL_POSSEL_PD1 << 0)            /**< Shifted mode PD1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD2                (_ACMP_INPUTCTRL_POSSEL_PD2 << 0)            /**< Shifted mode PD2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD3                (_ACMP_INPUTCTRL_POSSEL_PD3 << 0)            /**< Shifted mode PD3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD4                (_ACMP_INPUTCTRL_POSSEL_PD4 << 0)            /**< Shifted mode PD4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD5                (_ACMP_INPUTCTRL_POSSEL_PD5 << 0)            /**< Shifted mode PD5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD6                (_ACMP_INPUTCTRL_POSSEL_PD6 << 0)            /**< Shifted mode PD6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD7                (_ACMP_INPUTCTRL_POSSEL_PD7 << 0)            /**< Shifted mode PD7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD8                (_ACMP_INPUTCTRL_POSSEL_PD8 << 0)            /**< Shifted mode PD8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD9                (_ACMP_INPUTCTRL_POSSEL_PD9 << 0)            /**< Shifted mode PD9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_POSSEL_PD10               (_ACMP_INPUTCTRL_POSSEL_PD10 << 0)           /**< Shifted mode PD10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PD11               (_ACMP_INPUTCTRL_POSSEL_PD11 << 0)           /**< Shifted mode PD11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PD12               (_ACMP_INPUTCTRL_POSSEL_PD12 << 0)           /**< Shifted mode PD12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PD13               (_ACMP_INPUTCTRL_POSSEL_PD13 << 0)           /**< Shifted mode PD13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PD14               (_ACMP_INPUTCTRL_POSSEL_PD14 << 0)           /**< Shifted mode PD14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_POSSEL_PD15               (_ACMP_INPUTCTRL_POSSEL_PD15 << 0)           /**< Shifted mode PD15 for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_NEGSEL_SHIFT             8                                            /**< Shift value for ACMP_NEGSEL                 */
#define _ACMP_INPUTCTRL_NEGSEL_MASK              0xFF00UL                                     /**< Bit mask for ACMP_NEGSEL                    */
#define _ACMP_INPUTCTRL_NEGSEL_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for ACMP_INPUTCTRL             */
#define _ACMP_INPUTCTRL_NEGSEL_VSS               0x00000000UL                                 /**< Mode VSS for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_VREFDIVAVDD       0x00000010UL                                 /**< Mode VREFDIVAVDD for ACMP_INPUTCTRL         */
#define _ACMP_INPUTCTRL_NEGSEL_VREFDIVAVDDLP     0x00000011UL                                 /**< Mode VREFDIVAVDDLP for ACMP_INPUTCTRL       */
#define _ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25       0x00000012UL                                 /**< Mode VREFDIV1V25 for ACMP_INPUTCTRL         */
#define _ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25LP     0x00000013UL                                 /**< Mode VREFDIV1V25LP for ACMP_INPUTCTRL       */
#define _ACMP_INPUTCTRL_NEGSEL_VREFDIV2V5        0x00000014UL                                 /**< Mode VREFDIV2V5 for ACMP_INPUTCTRL          */
#define _ACMP_INPUTCTRL_NEGSEL_VREFDIV2V5LP      0x00000015UL                                 /**< Mode VREFDIV2V5LP for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_NEGSEL_VSENSE01DIV4      0x00000020UL                                 /**< Mode VSENSE01DIV4 for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_NEGSEL_VSENSE01DIV4LP    0x00000021UL                                 /**< Mode VSENSE01DIV4LP for ACMP_INPUTCTRL      */
#define _ACMP_INPUTCTRL_NEGSEL_VSENSE11DIV4      0x00000022UL                                 /**< Mode VSENSE11DIV4 for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_NEGSEL_VSENSE11DIV4LP    0x00000023UL                                 /**< Mode VSENSE11DIV4LP for ACMP_INPUTCTRL      */
#define _ACMP_INPUTCTRL_NEGSEL_CAPSENSE          0x00000030UL                                 /**< Mode CAPSENSE for ACMP_INPUTCTRL            */
#define _ACMP_INPUTCTRL_NEGSEL_VDACOUT0          0x00000040UL                                 /**< Mode VDACOUT0 for ACMP_INPUTCTRL            */
#define _ACMP_INPUTCTRL_NEGSEL_VDACOUT1          0x00000041UL                                 /**< Mode VDACOUT1 for ACMP_INPUTCTRL            */
#define _ACMP_INPUTCTRL_NEGSEL_PA0               0x00000080UL                                 /**< Mode PA0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA1               0x00000081UL                                 /**< Mode PA1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA2               0x00000082UL                                 /**< Mode PA2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA3               0x00000083UL                                 /**< Mode PA3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA4               0x00000084UL                                 /**< Mode PA4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA5               0x00000085UL                                 /**< Mode PA5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA6               0x00000086UL                                 /**< Mode PA6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA7               0x00000087UL                                 /**< Mode PA7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA8               0x00000088UL                                 /**< Mode PA8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA9               0x00000089UL                                 /**< Mode PA9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PA10              0x0000008AUL                                 /**< Mode PA10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PA11              0x0000008BUL                                 /**< Mode PA11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PA12              0x0000008CUL                                 /**< Mode PA12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PA13              0x0000008DUL                                 /**< Mode PA13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PA14              0x0000008EUL                                 /**< Mode PA14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PA15              0x0000008FUL                                 /**< Mode PA15 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PB0               0x00000090UL                                 /**< Mode PB0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB1               0x00000091UL                                 /**< Mode PB1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB2               0x00000092UL                                 /**< Mode PB2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB3               0x00000093UL                                 /**< Mode PB3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB4               0x00000094UL                                 /**< Mode PB4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB5               0x00000095UL                                 /**< Mode PB5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB6               0x00000096UL                                 /**< Mode PB6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB7               0x00000097UL                                 /**< Mode PB7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB8               0x00000098UL                                 /**< Mode PB8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB9               0x00000099UL                                 /**< Mode PB9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PB10              0x0000009AUL                                 /**< Mode PB10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PB11              0x0000009BUL                                 /**< Mode PB11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PB12              0x0000009CUL                                 /**< Mode PB12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PB13              0x0000009DUL                                 /**< Mode PB13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PB14              0x0000009EUL                                 /**< Mode PB14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PB15              0x0000009FUL                                 /**< Mode PB15 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PC0               0x000000A0UL                                 /**< Mode PC0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC1               0x000000A1UL                                 /**< Mode PC1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC2               0x000000A2UL                                 /**< Mode PC2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC3               0x000000A3UL                                 /**< Mode PC3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC4               0x000000A4UL                                 /**< Mode PC4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC5               0x000000A5UL                                 /**< Mode PC5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC6               0x000000A6UL                                 /**< Mode PC6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC7               0x000000A7UL                                 /**< Mode PC7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC8               0x000000A8UL                                 /**< Mode PC8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC9               0x000000A9UL                                 /**< Mode PC9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PC10              0x000000AAUL                                 /**< Mode PC10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PC11              0x000000ABUL                                 /**< Mode PC11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PC12              0x000000ACUL                                 /**< Mode PC12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PC13              0x000000ADUL                                 /**< Mode PC13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PC14              0x000000AEUL                                 /**< Mode PC14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PC15              0x000000AFUL                                 /**< Mode PC15 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PD0               0x000000B0UL                                 /**< Mode PD0 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD1               0x000000B1UL                                 /**< Mode PD1 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD2               0x000000B2UL                                 /**< Mode PD2 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD3               0x000000B3UL                                 /**< Mode PD3 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD4               0x000000B4UL                                 /**< Mode PD4 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD5               0x000000B5UL                                 /**< Mode PD5 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD6               0x000000B6UL                                 /**< Mode PD6 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD7               0x000000B7UL                                 /**< Mode PD7 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD8               0x000000B8UL                                 /**< Mode PD8 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD9               0x000000B9UL                                 /**< Mode PD9 for ACMP_INPUTCTRL                 */
#define _ACMP_INPUTCTRL_NEGSEL_PD10              0x000000BAUL                                 /**< Mode PD10 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PD11              0x000000BBUL                                 /**< Mode PD11 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PD12              0x000000BCUL                                 /**< Mode PD12 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PD13              0x000000BDUL                                 /**< Mode PD13 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PD14              0x000000BEUL                                 /**< Mode PD14 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_NEGSEL_PD15              0x000000BFUL                                 /**< Mode PD15 for ACMP_INPUTCTRL                */
#define ACMP_INPUTCTRL_NEGSEL_DEFAULT            (_ACMP_INPUTCTRL_NEGSEL_DEFAULT << 8)        /**< Shifted mode DEFAULT for ACMP_INPUTCTRL     */
#define ACMP_INPUTCTRL_NEGSEL_VSS                (_ACMP_INPUTCTRL_NEGSEL_VSS << 8)            /**< Shifted mode VSS for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_VREFDIVAVDD        (_ACMP_INPUTCTRL_NEGSEL_VREFDIVAVDD << 8)    /**< Shifted mode VREFDIVAVDD for ACMP_INPUTCTRL */
#define ACMP_INPUTCTRL_NEGSEL_VREFDIVAVDDLP      (_ACMP_INPUTCTRL_NEGSEL_VREFDIVAVDDLP << 8)  /**< Shifted mode VREFDIVAVDDLP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25        (_ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25 << 8)    /**< Shifted mode VREFDIV1V25 for ACMP_INPUTCTRL */
#define ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25LP      (_ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25LP << 8)  /**< Shifted mode VREFDIV1V25LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_VREFDIV2V5         (_ACMP_INPUTCTRL_NEGSEL_VREFDIV2V5 << 8)     /**< Shifted mode VREFDIV2V5 for ACMP_INPUTCTRL  */
#define ACMP_INPUTCTRL_NEGSEL_VREFDIV2V5LP       (_ACMP_INPUTCTRL_NEGSEL_VREFDIV2V5LP << 8)   /**< Shifted mode VREFDIV2V5LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_VSENSE01DIV4       (_ACMP_INPUTCTRL_NEGSEL_VSENSE01DIV4 << 8)   /**< Shifted mode VSENSE01DIV4 for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_VSENSE01DIV4LP     (_ACMP_INPUTCTRL_NEGSEL_VSENSE01DIV4LP << 8) /**< Shifted mode VSENSE01DIV4LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_VSENSE11DIV4       (_ACMP_INPUTCTRL_NEGSEL_VSENSE11DIV4 << 8)   /**< Shifted mode VSENSE11DIV4 for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_VSENSE11DIV4LP     (_ACMP_INPUTCTRL_NEGSEL_VSENSE11DIV4LP << 8) /**< Shifted mode VSENSE11DIV4LP for ACMP_INPUTCTRL*/
#define ACMP_INPUTCTRL_NEGSEL_CAPSENSE           (_ACMP_INPUTCTRL_NEGSEL_CAPSENSE << 8)       /**< Shifted mode CAPSENSE for ACMP_INPUTCTRL    */
#define ACMP_INPUTCTRL_NEGSEL_VDACOUT0           (_ACMP_INPUTCTRL_NEGSEL_VDACOUT0 << 8)       /**< Shifted mode VDACOUT0 for ACMP_INPUTCTRL    */
#define ACMP_INPUTCTRL_NEGSEL_VDACOUT1           (_ACMP_INPUTCTRL_NEGSEL_VDACOUT1 << 8)       /**< Shifted mode VDACOUT1 for ACMP_INPUTCTRL    */
#define ACMP_INPUTCTRL_NEGSEL_PA0                (_ACMP_INPUTCTRL_NEGSEL_PA0 << 8)            /**< Shifted mode PA0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA1                (_ACMP_INPUTCTRL_NEGSEL_PA1 << 8)            /**< Shifted mode PA1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA2                (_ACMP_INPUTCTRL_NEGSEL_PA2 << 8)            /**< Shifted mode PA2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA3                (_ACMP_INPUTCTRL_NEGSEL_PA3 << 8)            /**< Shifted mode PA3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA4                (_ACMP_INPUTCTRL_NEGSEL_PA4 << 8)            /**< Shifted mode PA4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA5                (_ACMP_INPUTCTRL_NEGSEL_PA5 << 8)            /**< Shifted mode PA5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA6                (_ACMP_INPUTCTRL_NEGSEL_PA6 << 8)            /**< Shifted mode PA6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA7                (_ACMP_INPUTCTRL_NEGSEL_PA7 << 8)            /**< Shifted mode PA7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA8                (_ACMP_INPUTCTRL_NEGSEL_PA8 << 8)            /**< Shifted mode PA8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA9                (_ACMP_INPUTCTRL_NEGSEL_PA9 << 8)            /**< Shifted mode PA9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PA10               (_ACMP_INPUTCTRL_NEGSEL_PA10 << 8)           /**< Shifted mode PA10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PA11               (_ACMP_INPUTCTRL_NEGSEL_PA11 << 8)           /**< Shifted mode PA11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PA12               (_ACMP_INPUTCTRL_NEGSEL_PA12 << 8)           /**< Shifted mode PA12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PA13               (_ACMP_INPUTCTRL_NEGSEL_PA13 << 8)           /**< Shifted mode PA13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PA14               (_ACMP_INPUTCTRL_NEGSEL_PA14 << 8)           /**< Shifted mode PA14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PA15               (_ACMP_INPUTCTRL_NEGSEL_PA15 << 8)           /**< Shifted mode PA15 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PB0                (_ACMP_INPUTCTRL_NEGSEL_PB0 << 8)            /**< Shifted mode PB0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB1                (_ACMP_INPUTCTRL_NEGSEL_PB1 << 8)            /**< Shifted mode PB1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB2                (_ACMP_INPUTCTRL_NEGSEL_PB2 << 8)            /**< Shifted mode PB2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB3                (_ACMP_INPUTCTRL_NEGSEL_PB3 << 8)            /**< Shifted mode PB3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB4                (_ACMP_INPUTCTRL_NEGSEL_PB4 << 8)            /**< Shifted mode PB4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB5                (_ACMP_INPUTCTRL_NEGSEL_PB5 << 8)            /**< Shifted mode PB5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB6                (_ACMP_INPUTCTRL_NEGSEL_PB6 << 8)            /**< Shifted mode PB6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB7                (_ACMP_INPUTCTRL_NEGSEL_PB7 << 8)            /**< Shifted mode PB7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB8                (_ACMP_INPUTCTRL_NEGSEL_PB8 << 8)            /**< Shifted mode PB8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB9                (_ACMP_INPUTCTRL_NEGSEL_PB9 << 8)            /**< Shifted mode PB9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PB10               (_ACMP_INPUTCTRL_NEGSEL_PB10 << 8)           /**< Shifted mode PB10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PB11               (_ACMP_INPUTCTRL_NEGSEL_PB11 << 8)           /**< Shifted mode PB11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PB12               (_ACMP_INPUTCTRL_NEGSEL_PB12 << 8)           /**< Shifted mode PB12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PB13               (_ACMP_INPUTCTRL_NEGSEL_PB13 << 8)           /**< Shifted mode PB13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PB14               (_ACMP_INPUTCTRL_NEGSEL_PB14 << 8)           /**< Shifted mode PB14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PB15               (_ACMP_INPUTCTRL_NEGSEL_PB15 << 8)           /**< Shifted mode PB15 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PC0                (_ACMP_INPUTCTRL_NEGSEL_PC0 << 8)            /**< Shifted mode PC0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC1                (_ACMP_INPUTCTRL_NEGSEL_PC1 << 8)            /**< Shifted mode PC1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC2                (_ACMP_INPUTCTRL_NEGSEL_PC2 << 8)            /**< Shifted mode PC2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC3                (_ACMP_INPUTCTRL_NEGSEL_PC3 << 8)            /**< Shifted mode PC3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC4                (_ACMP_INPUTCTRL_NEGSEL_PC4 << 8)            /**< Shifted mode PC4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC5                (_ACMP_INPUTCTRL_NEGSEL_PC5 << 8)            /**< Shifted mode PC5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC6                (_ACMP_INPUTCTRL_NEGSEL_PC6 << 8)            /**< Shifted mode PC6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC7                (_ACMP_INPUTCTRL_NEGSEL_PC7 << 8)            /**< Shifted mode PC7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC8                (_ACMP_INPUTCTRL_NEGSEL_PC8 << 8)            /**< Shifted mode PC8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC9                (_ACMP_INPUTCTRL_NEGSEL_PC9 << 8)            /**< Shifted mode PC9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PC10               (_ACMP_INPUTCTRL_NEGSEL_PC10 << 8)           /**< Shifted mode PC10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PC11               (_ACMP_INPUTCTRL_NEGSEL_PC11 << 8)           /**< Shifted mode PC11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PC12               (_ACMP_INPUTCTRL_NEGSEL_PC12 << 8)           /**< Shifted mode PC12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PC13               (_ACMP_INPUTCTRL_NEGSEL_PC13 << 8)           /**< Shifted mode PC13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PC14               (_ACMP_INPUTCTRL_NEGSEL_PC14 << 8)           /**< Shifted mode PC14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PC15               (_ACMP_INPUTCTRL_NEGSEL_PC15 << 8)           /**< Shifted mode PC15 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PD0                (_ACMP_INPUTCTRL_NEGSEL_PD0 << 8)            /**< Shifted mode PD0 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD1                (_ACMP_INPUTCTRL_NEGSEL_PD1 << 8)            /**< Shifted mode PD1 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD2                (_ACMP_INPUTCTRL_NEGSEL_PD2 << 8)            /**< Shifted mode PD2 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD3                (_ACMP_INPUTCTRL_NEGSEL_PD3 << 8)            /**< Shifted mode PD3 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD4                (_ACMP_INPUTCTRL_NEGSEL_PD4 << 8)            /**< Shifted mode PD4 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD5                (_ACMP_INPUTCTRL_NEGSEL_PD5 << 8)            /**< Shifted mode PD5 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD6                (_ACMP_INPUTCTRL_NEGSEL_PD6 << 8)            /**< Shifted mode PD6 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD7                (_ACMP_INPUTCTRL_NEGSEL_PD7 << 8)            /**< Shifted mode PD7 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD8                (_ACMP_INPUTCTRL_NEGSEL_PD8 << 8)            /**< Shifted mode PD8 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD9                (_ACMP_INPUTCTRL_NEGSEL_PD9 << 8)            /**< Shifted mode PD9 for ACMP_INPUTCTRL         */
#define ACMP_INPUTCTRL_NEGSEL_PD10               (_ACMP_INPUTCTRL_NEGSEL_PD10 << 8)           /**< Shifted mode PD10 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PD11               (_ACMP_INPUTCTRL_NEGSEL_PD11 << 8)           /**< Shifted mode PD11 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PD12               (_ACMP_INPUTCTRL_NEGSEL_PD12 << 8)           /**< Shifted mode PD12 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PD13               (_ACMP_INPUTCTRL_NEGSEL_PD13 << 8)           /**< Shifted mode PD13 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PD14               (_ACMP_INPUTCTRL_NEGSEL_PD14 << 8)           /**< Shifted mode PD14 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_NEGSEL_PD15               (_ACMP_INPUTCTRL_NEGSEL_PD15 << 8)           /**< Shifted mode PD15 for ACMP_INPUTCTRL        */
#define _ACMP_INPUTCTRL_VREFDIV_SHIFT            16                                           /**< Shift value for ACMP_VREFDIV                */
#define _ACMP_INPUTCTRL_VREFDIV_MASK             0x3F0000UL                                   /**< Bit mask for ACMP_VREFDIV                   */
#define _ACMP_INPUTCTRL_VREFDIV_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for ACMP_INPUTCTRL             */
#define ACMP_INPUTCTRL_VREFDIV_DEFAULT           (_ACMP_INPUTCTRL_VREFDIV_DEFAULT << 16)      /**< Shifted mode DEFAULT for ACMP_INPUTCTRL     */
#define _ACMP_INPUTCTRL_CSRESSEL_SHIFT           28                                           /**< Shift value for ACMP_CSRESSEL               */
#define _ACMP_INPUTCTRL_CSRESSEL_MASK            0x70000000UL                                 /**< Bit mask for ACMP_CSRESSEL                  */
#define _ACMP_INPUTCTRL_CSRESSEL_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for ACMP_INPUTCTRL             */
#define _ACMP_INPUTCTRL_CSRESSEL_RES0            0x00000000UL                                 /**< Mode RES0 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_CSRESSEL_RES1            0x00000001UL                                 /**< Mode RES1 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_CSRESSEL_RES2            0x00000002UL                                 /**< Mode RES2 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_CSRESSEL_RES3            0x00000003UL                                 /**< Mode RES3 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_CSRESSEL_RES4            0x00000004UL                                 /**< Mode RES4 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_CSRESSEL_RES5            0x00000005UL                                 /**< Mode RES5 for ACMP_INPUTCTRL                */
#define _ACMP_INPUTCTRL_CSRESSEL_RES6            0x00000006UL                                 /**< Mode RES6 for ACMP_INPUTCTRL                */
#define ACMP_INPUTCTRL_CSRESSEL_DEFAULT          (_ACMP_INPUTCTRL_CSRESSEL_DEFAULT << 28)     /**< Shifted mode DEFAULT for ACMP_INPUTCTRL     */
#define ACMP_INPUTCTRL_CSRESSEL_RES0             (_ACMP_INPUTCTRL_CSRESSEL_RES0 << 28)        /**< Shifted mode RES0 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_CSRESSEL_RES1             (_ACMP_INPUTCTRL_CSRESSEL_RES1 << 28)        /**< Shifted mode RES1 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_CSRESSEL_RES2             (_ACMP_INPUTCTRL_CSRESSEL_RES2 << 28)        /**< Shifted mode RES2 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_CSRESSEL_RES3             (_ACMP_INPUTCTRL_CSRESSEL_RES3 << 28)        /**< Shifted mode RES3 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_CSRESSEL_RES4             (_ACMP_INPUTCTRL_CSRESSEL_RES4 << 28)        /**< Shifted mode RES4 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_CSRESSEL_RES5             (_ACMP_INPUTCTRL_CSRESSEL_RES5 << 28)        /**< Shifted mode RES5 for ACMP_INPUTCTRL        */
#define ACMP_INPUTCTRL_CSRESSEL_RES6             (_ACMP_INPUTCTRL_CSRESSEL_RES6 << 28)        /**< Shifted mode RES6 for ACMP_INPUTCTRL        */

/* Bit fields for ACMP STATUS */
#define _ACMP_STATUS_RESETVALUE                  0x00000000UL                              /**< Default value for ACMP_STATUS               */
#define _ACMP_STATUS_MASK                        0x0000001DUL                              /**< Mask for ACMP_STATUS                        */
#define ACMP_STATUS_ACMPOUT                      (0x1UL << 0)                              /**< Analog Comparator Output                    */
#define _ACMP_STATUS_ACMPOUT_SHIFT               0                                         /**< Shift value for ACMP_ACMPOUT                */
#define _ACMP_STATUS_ACMPOUT_MASK                0x1UL                                     /**< Bit mask for ACMP_ACMPOUT                   */
#define _ACMP_STATUS_ACMPOUT_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for ACMP_STATUS                */
#define ACMP_STATUS_ACMPOUT_DEFAULT              (_ACMP_STATUS_ACMPOUT_DEFAULT << 0)       /**< Shifted mode DEFAULT for ACMP_STATUS        */
#define ACMP_STATUS_ACMPRDY                      (0x1UL << 2)                              /**< Analog Comparator Ready                     */
#define _ACMP_STATUS_ACMPRDY_SHIFT               2                                         /**< Shift value for ACMP_ACMPRDY                */
#define _ACMP_STATUS_ACMPRDY_MASK                0x4UL                                     /**< Bit mask for ACMP_ACMPRDY                   */
#define _ACMP_STATUS_ACMPRDY_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for ACMP_STATUS                */
#define ACMP_STATUS_ACMPRDY_DEFAULT              (_ACMP_STATUS_ACMPRDY_DEFAULT << 2)       /**< Shifted mode DEFAULT for ACMP_STATUS        */
#define ACMP_STATUS_INPUTCONFLICT                (0x1UL << 3)                              /**< INPUT conflict                              */
#define _ACMP_STATUS_INPUTCONFLICT_SHIFT         3                                         /**< Shift value for ACMP_INPUTCONFLICT          */
#define _ACMP_STATUS_INPUTCONFLICT_MASK          0x8UL                                     /**< Bit mask for ACMP_INPUTCONFLICT             */
#define _ACMP_STATUS_INPUTCONFLICT_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for ACMP_STATUS                */
#define ACMP_STATUS_INPUTCONFLICT_DEFAULT        (_ACMP_STATUS_INPUTCONFLICT_DEFAULT << 3) /**< Shifted mode DEFAULT for ACMP_STATUS        */
#define ACMP_STATUS_PORTALLOCERR                 (0x1UL << 4)                              /**< Port allocation error                       */
#define _ACMP_STATUS_PORTALLOCERR_SHIFT          4                                         /**< Shift value for ACMP_PORTALLOCERR           */
#define _ACMP_STATUS_PORTALLOCERR_MASK           0x10UL                                    /**< Bit mask for ACMP_PORTALLOCERR              */
#define _ACMP_STATUS_PORTALLOCERR_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for ACMP_STATUS                */
#define ACMP_STATUS_PORTALLOCERR_DEFAULT         (_ACMP_STATUS_PORTALLOCERR_DEFAULT << 4)  /**< Shifted mode DEFAULT for ACMP_STATUS        */

/* Bit fields for ACMP IF */
#define _ACMP_IF_RESETVALUE                      0x00000000UL                           /**< Default value for ACMP_IF                   */
#define _ACMP_IF_MASK                            0x0000001FUL                           /**< Mask for ACMP_IF                            */
#define ACMP_IF_RISE                             (0x1UL << 0)                           /**< Rising Edge Triggered Interrupt Flag        */
#define _ACMP_IF_RISE_SHIFT                      0                                      /**< Shift value for ACMP_RISE                   */
#define _ACMP_IF_RISE_MASK                       0x1UL                                  /**< Bit mask for ACMP_RISE                      */
#define _ACMP_IF_RISE_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for ACMP_IF                    */
#define ACMP_IF_RISE_DEFAULT                     (_ACMP_IF_RISE_DEFAULT << 0)           /**< Shifted mode DEFAULT for ACMP_IF            */
#define ACMP_IF_FALL                             (0x1UL << 1)                           /**< Falling Edge Triggered Interrupt Flag       */
#define _ACMP_IF_FALL_SHIFT                      1                                      /**< Shift value for ACMP_FALL                   */
#define _ACMP_IF_FALL_MASK                       0x2UL                                  /**< Bit mask for ACMP_FALL                      */
#define _ACMP_IF_FALL_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for ACMP_IF                    */
#define ACMP_IF_FALL_DEFAULT                     (_ACMP_IF_FALL_DEFAULT << 1)           /**< Shifted mode DEFAULT for ACMP_IF            */
#define ACMP_IF_ACMPRDY                          (0x1UL << 2)                           /**< ACMP ready Interrupt flag                   */
#define _ACMP_IF_ACMPRDY_SHIFT                   2                                      /**< Shift value for ACMP_ACMPRDY                */
#define _ACMP_IF_ACMPRDY_MASK                    0x4UL                                  /**< Bit mask for ACMP_ACMPRDY                   */
#define _ACMP_IF_ACMPRDY_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for ACMP_IF                    */
#define ACMP_IF_ACMPRDY_DEFAULT                  (_ACMP_IF_ACMPRDY_DEFAULT << 2)        /**< Shifted mode DEFAULT for ACMP_IF            */
#define ACMP_IF_INPUTCONFLICT                    (0x1UL << 3)                           /**< Input conflict                              */
#define _ACMP_IF_INPUTCONFLICT_SHIFT             3                                      /**< Shift value for ACMP_INPUTCONFLICT          */
#define _ACMP_IF_INPUTCONFLICT_MASK              0x8UL                                  /**< Bit mask for ACMP_INPUTCONFLICT             */
#define _ACMP_IF_INPUTCONFLICT_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for ACMP_IF                    */
#define ACMP_IF_INPUTCONFLICT_DEFAULT            (_ACMP_IF_INPUTCONFLICT_DEFAULT << 3)  /**< Shifted mode DEFAULT for ACMP_IF            */
#define ACMP_IF_PORTALLOCERR                     (0x1UL << 4)                           /**< Port allocation error                       */
#define _ACMP_IF_PORTALLOCERR_SHIFT              4                                      /**< Shift value for ACMP_PORTALLOCERR           */
#define _ACMP_IF_PORTALLOCERR_MASK               0x10UL                                 /**< Bit mask for ACMP_PORTALLOCERR              */
#define _ACMP_IF_PORTALLOCERR_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for ACMP_IF                    */
#define ACMP_IF_PORTALLOCERR_DEFAULT             (_ACMP_IF_PORTALLOCERR_DEFAULT << 4)   /**< Shifted mode DEFAULT for ACMP_IF            */

/* Bit fields for ACMP IEN */
#define _ACMP_IEN_RESETVALUE                     0x00000000UL                           /**< Default value for ACMP_IEN                  */
#define _ACMP_IEN_MASK                           0x0000001FUL                           /**< Mask for ACMP_IEN                           */
#define ACMP_IEN_RISE                            (0x1UL << 0)                           /**< Rising edge interrupt enable                */
#define _ACMP_IEN_RISE_SHIFT                     0                                      /**< Shift value for ACMP_RISE                   */
#define _ACMP_IEN_RISE_MASK                      0x1UL                                  /**< Bit mask for ACMP_RISE                      */
#define _ACMP_IEN_RISE_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ACMP_IEN                   */
#define ACMP_IEN_RISE_DEFAULT                    (_ACMP_IEN_RISE_DEFAULT << 0)          /**< Shifted mode DEFAULT for ACMP_IEN           */
#define ACMP_IEN_FALL                            (0x1UL << 1)                           /**< Falling edge interrupt enable               */
#define _ACMP_IEN_FALL_SHIFT                     1                                      /**< Shift value for ACMP_FALL                   */
#define _ACMP_IEN_FALL_MASK                      0x2UL                                  /**< Bit mask for ACMP_FALL                      */
#define _ACMP_IEN_FALL_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ACMP_IEN                   */
#define ACMP_IEN_FALL_DEFAULT                    (_ACMP_IEN_FALL_DEFAULT << 1)          /**< Shifted mode DEFAULT for ACMP_IEN           */
#define ACMP_IEN_ACMPRDY                         (0x1UL << 2)                           /**< ACMP ready interrupt enable                 */
#define _ACMP_IEN_ACMPRDY_SHIFT                  2                                      /**< Shift value for ACMP_ACMPRDY                */
#define _ACMP_IEN_ACMPRDY_MASK                   0x4UL                                  /**< Bit mask for ACMP_ACMPRDY                   */
#define _ACMP_IEN_ACMPRDY_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for ACMP_IEN                   */
#define ACMP_IEN_ACMPRDY_DEFAULT                 (_ACMP_IEN_ACMPRDY_DEFAULT << 2)       /**< Shifted mode DEFAULT for ACMP_IEN           */
#define ACMP_IEN_INPUTCONFLICT                   (0x1UL << 3)                           /**< Input conflict interrupt enable             */
#define _ACMP_IEN_INPUTCONFLICT_SHIFT            3                                      /**< Shift value for ACMP_INPUTCONFLICT          */
#define _ACMP_IEN_INPUTCONFLICT_MASK             0x8UL                                  /**< Bit mask for ACMP_INPUTCONFLICT             */
#define _ACMP_IEN_INPUTCONFLICT_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for ACMP_IEN                   */
#define ACMP_IEN_INPUTCONFLICT_DEFAULT           (_ACMP_IEN_INPUTCONFLICT_DEFAULT << 3) /**< Shifted mode DEFAULT for ACMP_IEN           */
#define ACMP_IEN_PORTALLOCERR                    (0x1UL << 4)                           /**< Port allocation error interrupt enable      */
#define _ACMP_IEN_PORTALLOCERR_SHIFT             4                                      /**< Shift value for ACMP_PORTALLOCERR           */
#define _ACMP_IEN_PORTALLOCERR_MASK              0x10UL                                 /**< Bit mask for ACMP_PORTALLOCERR              */
#define _ACMP_IEN_PORTALLOCERR_DEFAULT           0x00000000UL                           /**< Mode DEFAULT for ACMP_IEN                   */
#define ACMP_IEN_PORTALLOCERR_DEFAULT            (_ACMP_IEN_PORTALLOCERR_DEFAULT << 4)  /**< Shifted mode DEFAULT for ACMP_IEN           */

/* Bit fields for ACMP SYNCBUSY */
#define _ACMP_SYNCBUSY_RESETVALUE                0x00000000UL                            /**< Default value for ACMP_SYNCBUSY             */
#define _ACMP_SYNCBUSY_MASK                      0x00000001UL                            /**< Mask for ACMP_SYNCBUSY                      */
#define ACMP_SYNCBUSY_INPUTCTRL                  (0x1UL << 0)                            /**< Syncbusy for INPUTCTRL                      */
#define _ACMP_SYNCBUSY_INPUTCTRL_SHIFT           0                                       /**< Shift value for ACMP_INPUTCTRL              */
#define _ACMP_SYNCBUSY_INPUTCTRL_MASK            0x1UL                                   /**< Bit mask for ACMP_INPUTCTRL                 */
#define _ACMP_SYNCBUSY_INPUTCTRL_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for ACMP_SYNCBUSY              */
#define ACMP_SYNCBUSY_INPUTCTRL_DEFAULT          (_ACMP_SYNCBUSY_INPUTCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for ACMP_SYNCBUSY      */

/** @} End of group EFR32ZG23_ACMP_BitFields */
/** @} End of group EFR32ZG23_ACMP */
/** @} End of group Parts */

#endif /* EFR32ZG23_ACMP_H */
#ifdef __cplusplus
}
#endif

