/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 GPIO Port register and bit field definitions
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

#ifndef GPIO_PORT_H
#define GPIO_PORT_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @brief EFR32ZG23 GPIO PORT
 *****************************************************************************/
typedef struct {
  __IOM uint32_t CTRL;                          /**< Port control            */
  __IOM uint32_t MODEL;                         /**< mode low                */
  uint32_t       RESERVED0[1];                  /**< Reserved for future use */
  __IOM uint32_t MODEH;                         /**< mode high               */
  __IOM uint32_t DOUT;                          /**< data out                */
  __IM uint32_t  DIN;                           /**< data in                 */
  uint32_t       RESERVED1[6];                  /**< Reserved for future use */
} GPIO_PORT_TypeDef;

/* Bit fields for GPIO_P CTRL */
#define _GPIO_P_CTRL_RESETVALUE                        0x00400040UL                             /**< Default value for GPIO_P_CTRL               */
#define _GPIO_P_CTRL_MASK                              0x10701070UL                             /**< Mask for GPIO_P_CTRL                        */
#define _GPIO_P_CTRL_SLEWRATE_SHIFT                    4                                        /**< Shift value for GPIO_SLEWRATE               */
#define _GPIO_P_CTRL_SLEWRATE_MASK                     0x70UL                                   /**< Bit mask for GPIO_SLEWRATE                  */
#define _GPIO_P_CTRL_SLEWRATE_DEFAULT                  0x00000004UL                             /**< Mode DEFAULT for GPIO_P_CTRL                */
#define GPIO_P_CTRL_SLEWRATE_DEFAULT                   (_GPIO_P_CTRL_SLEWRATE_DEFAULT << 4)     /**< Shifted mode DEFAULT for GPIO_P_CTRL        */
#define GPIO_P_CTRL_DINDIS                             (0x1UL << 12)                            /**< Data In Disable                             */
#define _GPIO_P_CTRL_DINDIS_SHIFT                      12                                       /**< Shift value for GPIO_DINDIS                 */
#define _GPIO_P_CTRL_DINDIS_MASK                       0x1000UL                                 /**< Bit mask for GPIO_DINDIS                    */
#define _GPIO_P_CTRL_DINDIS_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for GPIO_P_CTRL                */
#define GPIO_P_CTRL_DINDIS_DEFAULT                     (_GPIO_P_CTRL_DINDIS_DEFAULT << 12)      /**< Shifted mode DEFAULT for GPIO_P_CTRL        */
#define _GPIO_P_CTRL_SLEWRATEALT_SHIFT                 20                                       /**< Shift value for GPIO_SLEWRATEALT            */
#define _GPIO_P_CTRL_SLEWRATEALT_MASK                  0x700000UL                               /**< Bit mask for GPIO_SLEWRATEALT               */
#define _GPIO_P_CTRL_SLEWRATEALT_DEFAULT               0x00000004UL                             /**< Mode DEFAULT for GPIO_P_CTRL                */
#define GPIO_P_CTRL_SLEWRATEALT_DEFAULT                (_GPIO_P_CTRL_SLEWRATEALT_DEFAULT << 20) /**< Shifted mode DEFAULT for GPIO_P_CTRL        */
#define GPIO_P_CTRL_DINDISALT                          (0x1UL << 28)                            /**< Data In Disable Alt                         */
#define _GPIO_P_CTRL_DINDISALT_SHIFT                   28                                       /**< Shift value for GPIO_DINDISALT              */
#define _GPIO_P_CTRL_DINDISALT_MASK                    0x10000000UL                             /**< Bit mask for GPIO_DINDISALT                 */
#define _GPIO_P_CTRL_DINDISALT_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_P_CTRL                */
#define GPIO_P_CTRL_DINDISALT_DEFAULT                  (_GPIO_P_CTRL_DINDISALT_DEFAULT << 28)   /**< Shifted mode DEFAULT for GPIO_P_CTRL        */

/* Bit fields for GPIO_P MODEL */
#define _GPIO_P_MODEL_RESETVALUE                       0x00000000UL                                        /**< Default value for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MASK                             0xFFFFFFFFUL                                        /**< Mask for GPIO_P_MODEL                       */
#define _GPIO_P_MODEL_MODE0_SHIFT                      0                                                   /**< Shift value for GPIO_MODE0                  */
#define _GPIO_P_MODEL_MODE0_MASK                       0xFUL                                               /**< Bit mask for GPIO_MODE0                     */
#define _GPIO_P_MODEL_MODE0_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE0_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE0_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE0_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE0_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE0_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE0_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE0_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE0_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE0_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE0_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE0_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE0_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE0_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE0_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE0_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_DEFAULT                     (_GPIO_P_MODEL_MODE0_DEFAULT << 0)                  /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE0_DISABLED                    (_GPIO_P_MODEL_MODE0_DISABLED << 0)                 /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE0_INPUT                       (_GPIO_P_MODEL_MODE0_INPUT << 0)                    /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE0_INPUTPULL                   (_GPIO_P_MODEL_MODE0_INPUTPULL << 0)                /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE0_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE0_INPUTPULLFILTER << 0)          /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_PUSHPULL                    (_GPIO_P_MODEL_MODE0_PUSHPULL << 0)                 /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE0_PUSHPULLALT                 (_GPIO_P_MODEL_MODE0_PUSHPULLALT << 0)              /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE0_WIREDOR                     (_GPIO_P_MODEL_MODE0_WIREDOR << 0)                  /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE0_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE0_WIREDORPULLDOWN << 0)          /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_WIREDAND                    (_GPIO_P_MODEL_MODE0_WIREDAND << 0)                 /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE0_WIREDANDFILTER              (_GPIO_P_MODEL_MODE0_WIREDANDFILTER << 0)           /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE0_WIREDANDPULLUP << 0)           /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER << 0)     /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_WIREDANDALT                 (_GPIO_P_MODEL_MODE0_WIREDANDALT << 0)              /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE0_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE0_WIREDANDALTFILTER << 0)        /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE0_WIREDANDALTPULLUP << 0)        /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE0_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE0_WIREDANDALTPULLUPFILTER << 0)  /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE1_SHIFT                      4                                                   /**< Shift value for GPIO_MODE1                  */
#define _GPIO_P_MODEL_MODE1_MASK                       0xF0UL                                              /**< Bit mask for GPIO_MODE1                     */
#define _GPIO_P_MODEL_MODE1_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE1_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE1_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE1_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE1_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE1_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE1_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE1_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE1_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE1_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE1_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE1_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE1_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE1_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE1_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE1_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE1_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_DEFAULT                     (_GPIO_P_MODEL_MODE1_DEFAULT << 4)                  /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE1_DISABLED                    (_GPIO_P_MODEL_MODE1_DISABLED << 4)                 /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE1_INPUT                       (_GPIO_P_MODEL_MODE1_INPUT << 4)                    /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE1_INPUTPULL                   (_GPIO_P_MODEL_MODE1_INPUTPULL << 4)                /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE1_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE1_INPUTPULLFILTER << 4)          /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_PUSHPULL                    (_GPIO_P_MODEL_MODE1_PUSHPULL << 4)                 /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE1_PUSHPULLALT                 (_GPIO_P_MODEL_MODE1_PUSHPULLALT << 4)              /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE1_WIREDOR                     (_GPIO_P_MODEL_MODE1_WIREDOR << 4)                  /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE1_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE1_WIREDORPULLDOWN << 4)          /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_WIREDAND                    (_GPIO_P_MODEL_MODE1_WIREDAND << 4)                 /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE1_WIREDANDFILTER              (_GPIO_P_MODEL_MODE1_WIREDANDFILTER << 4)           /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE1_WIREDANDPULLUP << 4)           /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE1_WIREDANDPULLUPFILTER << 4)     /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_WIREDANDALT                 (_GPIO_P_MODEL_MODE1_WIREDANDALT << 4)              /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE1_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE1_WIREDANDALTFILTER << 4)        /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE1_WIREDANDALTPULLUP << 4)        /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE1_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE1_WIREDANDALTPULLUPFILTER << 4)  /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE2_SHIFT                      8                                                   /**< Shift value for GPIO_MODE2                  */
#define _GPIO_P_MODEL_MODE2_MASK                       0xF00UL                                             /**< Bit mask for GPIO_MODE2                     */
#define _GPIO_P_MODEL_MODE2_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE2_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE2_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE2_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE2_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE2_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE2_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE2_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE2_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE2_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE2_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE2_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE2_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE2_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE2_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE2_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE2_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_DEFAULT                     (_GPIO_P_MODEL_MODE2_DEFAULT << 8)                  /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE2_DISABLED                    (_GPIO_P_MODEL_MODE2_DISABLED << 8)                 /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE2_INPUT                       (_GPIO_P_MODEL_MODE2_INPUT << 8)                    /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE2_INPUTPULL                   (_GPIO_P_MODEL_MODE2_INPUTPULL << 8)                /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE2_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE2_INPUTPULLFILTER << 8)          /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_PUSHPULL                    (_GPIO_P_MODEL_MODE2_PUSHPULL << 8)                 /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE2_PUSHPULLALT                 (_GPIO_P_MODEL_MODE2_PUSHPULLALT << 8)              /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE2_WIREDOR                     (_GPIO_P_MODEL_MODE2_WIREDOR << 8)                  /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE2_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE2_WIREDORPULLDOWN << 8)          /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_WIREDAND                    (_GPIO_P_MODEL_MODE2_WIREDAND << 8)                 /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE2_WIREDANDFILTER              (_GPIO_P_MODEL_MODE2_WIREDANDFILTER << 8)           /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE2_WIREDANDPULLUP << 8)           /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE2_WIREDANDPULLUPFILTER << 8)     /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_WIREDANDALT                 (_GPIO_P_MODEL_MODE2_WIREDANDALT << 8)              /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE2_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE2_WIREDANDALTFILTER << 8)        /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE2_WIREDANDALTPULLUP << 8)        /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE2_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE2_WIREDANDALTPULLUPFILTER << 8)  /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE3_SHIFT                      12                                                  /**< Shift value for GPIO_MODE3                  */
#define _GPIO_P_MODEL_MODE3_MASK                       0xF000UL                                            /**< Bit mask for GPIO_MODE3                     */
#define _GPIO_P_MODEL_MODE3_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE3_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE3_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE3_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE3_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE3_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE3_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE3_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE3_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE3_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE3_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE3_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE3_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE3_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE3_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE3_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE3_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_DEFAULT                     (_GPIO_P_MODEL_MODE3_DEFAULT << 12)                 /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE3_DISABLED                    (_GPIO_P_MODEL_MODE3_DISABLED << 12)                /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE3_INPUT                       (_GPIO_P_MODEL_MODE3_INPUT << 12)                   /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE3_INPUTPULL                   (_GPIO_P_MODEL_MODE3_INPUTPULL << 12)               /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE3_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE3_INPUTPULLFILTER << 12)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_PUSHPULL                    (_GPIO_P_MODEL_MODE3_PUSHPULL << 12)                /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE3_PUSHPULLALT                 (_GPIO_P_MODEL_MODE3_PUSHPULLALT << 12)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE3_WIREDOR                     (_GPIO_P_MODEL_MODE3_WIREDOR << 12)                 /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE3_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE3_WIREDORPULLDOWN << 12)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_WIREDAND                    (_GPIO_P_MODEL_MODE3_WIREDAND << 12)                /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE3_WIREDANDFILTER              (_GPIO_P_MODEL_MODE3_WIREDANDFILTER << 12)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE3_WIREDANDPULLUP << 12)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE3_WIREDANDPULLUPFILTER << 12)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_WIREDANDALT                 (_GPIO_P_MODEL_MODE3_WIREDANDALT << 12)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE3_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE3_WIREDANDALTFILTER << 12)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE3_WIREDANDALTPULLUP << 12)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE3_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE3_WIREDANDALTPULLUPFILTER << 12) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE4_SHIFT                      16                                                  /**< Shift value for GPIO_MODE4                  */
#define _GPIO_P_MODEL_MODE4_MASK                       0xF0000UL                                           /**< Bit mask for GPIO_MODE4                     */
#define _GPIO_P_MODEL_MODE4_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE4_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE4_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE4_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE4_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE4_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE4_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE4_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE4_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE4_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE4_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE4_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE4_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE4_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE4_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE4_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE4_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_DEFAULT                     (_GPIO_P_MODEL_MODE4_DEFAULT << 16)                 /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE4_DISABLED                    (_GPIO_P_MODEL_MODE4_DISABLED << 16)                /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE4_INPUT                       (_GPIO_P_MODEL_MODE4_INPUT << 16)                   /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE4_INPUTPULL                   (_GPIO_P_MODEL_MODE4_INPUTPULL << 16)               /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE4_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE4_INPUTPULLFILTER << 16)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_PUSHPULL                    (_GPIO_P_MODEL_MODE4_PUSHPULL << 16)                /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE4_PUSHPULLALT                 (_GPIO_P_MODEL_MODE4_PUSHPULLALT << 16)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE4_WIREDOR                     (_GPIO_P_MODEL_MODE4_WIREDOR << 16)                 /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE4_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE4_WIREDORPULLDOWN << 16)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_WIREDAND                    (_GPIO_P_MODEL_MODE4_WIREDAND << 16)                /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE4_WIREDANDFILTER              (_GPIO_P_MODEL_MODE4_WIREDANDFILTER << 16)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE4_WIREDANDPULLUP << 16)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE4_WIREDANDPULLUPFILTER << 16)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_WIREDANDALT                 (_GPIO_P_MODEL_MODE4_WIREDANDALT << 16)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE4_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE4_WIREDANDALTFILTER << 16)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE4_WIREDANDALTPULLUP << 16)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE4_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE4_WIREDANDALTPULLUPFILTER << 16) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE5_SHIFT                      20                                                  /**< Shift value for GPIO_MODE5                  */
#define _GPIO_P_MODEL_MODE5_MASK                       0xF00000UL                                          /**< Bit mask for GPIO_MODE5                     */
#define _GPIO_P_MODEL_MODE5_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE5_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE5_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE5_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE5_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE5_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE5_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE5_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE5_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE5_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE5_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE5_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE5_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE5_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE5_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE5_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE5_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_DEFAULT                     (_GPIO_P_MODEL_MODE5_DEFAULT << 20)                 /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE5_DISABLED                    (_GPIO_P_MODEL_MODE5_DISABLED << 20)                /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE5_INPUT                       (_GPIO_P_MODEL_MODE5_INPUT << 20)                   /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE5_INPUTPULL                   (_GPIO_P_MODEL_MODE5_INPUTPULL << 20)               /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE5_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE5_INPUTPULLFILTER << 20)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_PUSHPULL                    (_GPIO_P_MODEL_MODE5_PUSHPULL << 20)                /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE5_PUSHPULLALT                 (_GPIO_P_MODEL_MODE5_PUSHPULLALT << 20)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE5_WIREDOR                     (_GPIO_P_MODEL_MODE5_WIREDOR << 20)                 /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE5_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE5_WIREDORPULLDOWN << 20)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_WIREDAND                    (_GPIO_P_MODEL_MODE5_WIREDAND << 20)                /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE5_WIREDANDFILTER              (_GPIO_P_MODEL_MODE5_WIREDANDFILTER << 20)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE5_WIREDANDPULLUP << 20)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE5_WIREDANDPULLUPFILTER << 20)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_WIREDANDALT                 (_GPIO_P_MODEL_MODE5_WIREDANDALT << 20)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE5_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE5_WIREDANDALTFILTER << 20)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE5_WIREDANDALTPULLUP << 20)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE5_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE5_WIREDANDALTPULLUPFILTER << 20) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE6_SHIFT                      24                                                  /**< Shift value for GPIO_MODE6                  */
#define _GPIO_P_MODEL_MODE6_MASK                       0xF000000UL                                         /**< Bit mask for GPIO_MODE6                     */
#define _GPIO_P_MODEL_MODE6_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE6_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE6_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE6_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE6_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE6_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE6_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE6_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE6_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE6_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE6_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE6_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE6_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE6_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE6_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE6_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE6_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_DEFAULT                     (_GPIO_P_MODEL_MODE6_DEFAULT << 24)                 /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE6_DISABLED                    (_GPIO_P_MODEL_MODE6_DISABLED << 24)                /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE6_INPUT                       (_GPIO_P_MODEL_MODE6_INPUT << 24)                   /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE6_INPUTPULL                   (_GPIO_P_MODEL_MODE6_INPUTPULL << 24)               /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE6_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE6_INPUTPULLFILTER << 24)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_PUSHPULL                    (_GPIO_P_MODEL_MODE6_PUSHPULL << 24)                /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE6_PUSHPULLALT                 (_GPIO_P_MODEL_MODE6_PUSHPULLALT << 24)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE6_WIREDOR                     (_GPIO_P_MODEL_MODE6_WIREDOR << 24)                 /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE6_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE6_WIREDORPULLDOWN << 24)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_WIREDAND                    (_GPIO_P_MODEL_MODE6_WIREDAND << 24)                /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE6_WIREDANDFILTER              (_GPIO_P_MODEL_MODE6_WIREDANDFILTER << 24)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE6_WIREDANDPULLUP << 24)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE6_WIREDANDPULLUPFILTER << 24)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_WIREDANDALT                 (_GPIO_P_MODEL_MODE6_WIREDANDALT << 24)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE6_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE6_WIREDANDALTFILTER << 24)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE6_WIREDANDALTPULLUP << 24)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE6_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE6_WIREDANDALTPULLUPFILTER << 24) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define _GPIO_P_MODEL_MODE7_SHIFT                      28                                                  /**< Shift value for GPIO_MODE7                  */
#define _GPIO_P_MODEL_MODE7_MASK                       0xF0000000UL                                        /**< Bit mask for GPIO_MODE7                     */
#define _GPIO_P_MODEL_MODE7_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE7_DISABLED                   0x00000000UL                                        /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE7_INPUT                      0x00000001UL                                        /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE7_INPUTPULL                  0x00000002UL                                        /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE7_INPUTPULLFILTER            0x00000003UL                                        /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE7_PUSHPULL                   0x00000004UL                                        /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE7_PUSHPULLALT                0x00000005UL                                        /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE7_WIREDOR                    0x00000006UL                                        /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE7_WIREDORPULLDOWN            0x00000007UL                                        /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE7_WIREDAND                   0x00000008UL                                        /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE7_WIREDANDFILTER             0x00000009UL                                        /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE7_WIREDANDPULLUP             0x0000000AUL                                        /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE7_WIREDANDPULLUPFILTER       0x0000000BUL                                        /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE7_WIREDANDALT                0x0000000CUL                                        /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE7_WIREDANDALTFILTER          0x0000000DUL                                        /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE7_WIREDANDALTPULLUP          0x0000000EUL                                        /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE7_WIREDANDALTPULLUPFILTER    0x0000000FUL                                        /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_DEFAULT                     (_GPIO_P_MODEL_MODE7_DEFAULT << 28)                 /**< Shifted mode DEFAULT for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE7_DISABLED                    (_GPIO_P_MODEL_MODE7_DISABLED << 28)                /**< Shifted mode DISABLED for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE7_INPUT                       (_GPIO_P_MODEL_MODE7_INPUT << 28)                   /**< Shifted mode INPUT for GPIO_P_MODEL         */
#define GPIO_P_MODEL_MODE7_INPUTPULL                   (_GPIO_P_MODEL_MODE7_INPUTPULL << 28)               /**< Shifted mode INPUTPULL for GPIO_P_MODEL     */
#define GPIO_P_MODEL_MODE7_INPUTPULLFILTER             (_GPIO_P_MODEL_MODE7_INPUTPULLFILTER << 28)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_PUSHPULL                    (_GPIO_P_MODEL_MODE7_PUSHPULL << 28)                /**< Shifted mode PUSHPULL for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE7_PUSHPULLALT                 (_GPIO_P_MODEL_MODE7_PUSHPULLALT << 28)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE7_WIREDOR                     (_GPIO_P_MODEL_MODE7_WIREDOR << 28)                 /**< Shifted mode WIREDOR for GPIO_P_MODEL       */
#define GPIO_P_MODEL_MODE7_WIREDORPULLDOWN             (_GPIO_P_MODEL_MODE7_WIREDORPULLDOWN << 28)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_WIREDAND                    (_GPIO_P_MODEL_MODE7_WIREDAND << 28)                /**< Shifted mode WIREDAND for GPIO_P_MODEL      */
#define GPIO_P_MODEL_MODE7_WIREDANDFILTER              (_GPIO_P_MODEL_MODE7_WIREDANDFILTER << 28)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_WIREDANDPULLUP              (_GPIO_P_MODEL_MODE7_WIREDANDPULLUP << 28)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_WIREDANDPULLUPFILTER        (_GPIO_P_MODEL_MODE7_WIREDANDPULLUPFILTER << 28)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_WIREDANDALT                 (_GPIO_P_MODEL_MODE7_WIREDANDALT << 28)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEL   */
#define GPIO_P_MODEL_MODE7_WIREDANDALTFILTER           (_GPIO_P_MODEL_MODE7_WIREDANDALTFILTER << 28)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_WIREDANDALTPULLUP           (_GPIO_P_MODEL_MODE7_WIREDANDALTPULLUP << 28)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEL*/
#define GPIO_P_MODEL_MODE7_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEL_MODE7_WIREDANDALTPULLUPFILTER << 28) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEL*/

/* Bit fields for GPIO_P MODEH */
#define _GPIO_P_MODEH_RESETVALUE                       0x00000000UL                                       /**< Default value for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MASK                             0x00000FFFUL                                       /**< Mask for GPIO_P_MODEH                       */
#define _GPIO_P_MODEH_MODE0_SHIFT                      0                                                  /**< Shift value for GPIO_MODE0                  */
#define _GPIO_P_MODEH_MODE0_MASK                       0xFUL                                              /**< Bit mask for GPIO_MODE0                     */
#define _GPIO_P_MODEH_MODE0_DEFAULT                    0x00000000UL                                       /**< Mode DEFAULT for GPIO_P_MODEH               */
#define _GPIO_P_MODEH_MODE0_DISABLED                   0x00000000UL                                       /**< Mode DISABLED for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE0_INPUT                      0x00000001UL                                       /**< Mode INPUT for GPIO_P_MODEH                 */
#define _GPIO_P_MODEH_MODE0_INPUTPULL                  0x00000002UL                                       /**< Mode INPUTPULL for GPIO_P_MODEH             */
#define _GPIO_P_MODEH_MODE0_INPUTPULLFILTER            0x00000003UL                                       /**< Mode INPUTPULLFILTER for GPIO_P_MODEH       */
#define _GPIO_P_MODEH_MODE0_PUSHPULL                   0x00000004UL                                       /**< Mode PUSHPULL for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE0_PUSHPULLALT                0x00000005UL                                       /**< Mode PUSHPULLALT for GPIO_P_MODEH           */
#define _GPIO_P_MODEH_MODE0_WIREDOR                    0x00000006UL                                       /**< Mode WIREDOR for GPIO_P_MODEH               */
#define _GPIO_P_MODEH_MODE0_WIREDORPULLDOWN            0x00000007UL                                       /**< Mode WIREDORPULLDOWN for GPIO_P_MODEH       */
#define _GPIO_P_MODEH_MODE0_WIREDAND                   0x00000008UL                                       /**< Mode WIREDAND for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE0_WIREDANDFILTER             0x00000009UL                                       /**< Mode WIREDANDFILTER for GPIO_P_MODEH        */
#define _GPIO_P_MODEH_MODE0_WIREDANDPULLUP             0x0000000AUL                                       /**< Mode WIREDANDPULLUP for GPIO_P_MODEH        */
#define _GPIO_P_MODEH_MODE0_WIREDANDPULLUPFILTER       0x0000000BUL                                       /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEH  */
#define _GPIO_P_MODEH_MODE0_WIREDANDALT                0x0000000CUL                                       /**< Mode WIREDANDALT for GPIO_P_MODEH           */
#define _GPIO_P_MODEH_MODE0_WIREDANDALTFILTER          0x0000000DUL                                       /**< Mode WIREDANDALTFILTER for GPIO_P_MODEH     */
#define _GPIO_P_MODEH_MODE0_WIREDANDALTPULLUP          0x0000000EUL                                       /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEH     */
#define _GPIO_P_MODEH_MODE0_WIREDANDALTPULLUPFILTER    0x0000000FUL                                       /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_DEFAULT                     (_GPIO_P_MODEH_MODE0_DEFAULT << 0)                 /**< Shifted mode DEFAULT for GPIO_P_MODEH       */
#define GPIO_P_MODEH_MODE0_DISABLED                    (_GPIO_P_MODEH_MODE0_DISABLED << 0)                /**< Shifted mode DISABLED for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE0_INPUT                       (_GPIO_P_MODEH_MODE0_INPUT << 0)                   /**< Shifted mode INPUT for GPIO_P_MODEH         */
#define GPIO_P_MODEH_MODE0_INPUTPULL                   (_GPIO_P_MODEH_MODE0_INPUTPULL << 0)               /**< Shifted mode INPUTPULL for GPIO_P_MODEH     */
#define GPIO_P_MODEH_MODE0_INPUTPULLFILTER             (_GPIO_P_MODEH_MODE0_INPUTPULLFILTER << 0)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_PUSHPULL                    (_GPIO_P_MODEH_MODE0_PUSHPULL << 0)                /**< Shifted mode PUSHPULL for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE0_PUSHPULLALT                 (_GPIO_P_MODEH_MODE0_PUSHPULLALT << 0)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEH   */
#define GPIO_P_MODEH_MODE0_WIREDOR                     (_GPIO_P_MODEH_MODE0_WIREDOR << 0)                 /**< Shifted mode WIREDOR for GPIO_P_MODEH       */
#define GPIO_P_MODEH_MODE0_WIREDORPULLDOWN             (_GPIO_P_MODEH_MODE0_WIREDORPULLDOWN << 0)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_WIREDAND                    (_GPIO_P_MODEH_MODE0_WIREDAND << 0)                /**< Shifted mode WIREDAND for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE0_WIREDANDFILTER              (_GPIO_P_MODEH_MODE0_WIREDANDFILTER << 0)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_WIREDANDPULLUP              (_GPIO_P_MODEH_MODE0_WIREDANDPULLUP << 0)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_WIREDANDPULLUPFILTER        (_GPIO_P_MODEH_MODE0_WIREDANDPULLUPFILTER << 0)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_WIREDANDALT                 (_GPIO_P_MODEH_MODE0_WIREDANDALT << 0)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEH   */
#define GPIO_P_MODEH_MODE0_WIREDANDALTFILTER           (_GPIO_P_MODEH_MODE0_WIREDANDALTFILTER << 0)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_WIREDANDALTPULLUP           (_GPIO_P_MODEH_MODE0_WIREDANDALTPULLUP << 0)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE0_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEH_MODE0_WIREDANDALTPULLUPFILTER << 0) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEH*/
#define _GPIO_P_MODEH_MODE1_SHIFT                      4                                                  /**< Shift value for GPIO_MODE1                  */
#define _GPIO_P_MODEH_MODE1_MASK                       0xF0UL                                             /**< Bit mask for GPIO_MODE1                     */
#define _GPIO_P_MODEH_MODE1_DEFAULT                    0x00000000UL                                       /**< Mode DEFAULT for GPIO_P_MODEH               */
#define _GPIO_P_MODEH_MODE1_DISABLED                   0x00000000UL                                       /**< Mode DISABLED for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE1_INPUT                      0x00000001UL                                       /**< Mode INPUT for GPIO_P_MODEH                 */
#define _GPIO_P_MODEH_MODE1_INPUTPULL                  0x00000002UL                                       /**< Mode INPUTPULL for GPIO_P_MODEH             */
#define _GPIO_P_MODEH_MODE1_INPUTPULLFILTER            0x00000003UL                                       /**< Mode INPUTPULLFILTER for GPIO_P_MODEH       */
#define _GPIO_P_MODEH_MODE1_PUSHPULL                   0x00000004UL                                       /**< Mode PUSHPULL for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE1_PUSHPULLALT                0x00000005UL                                       /**< Mode PUSHPULLALT for GPIO_P_MODEH           */
#define _GPIO_P_MODEH_MODE1_WIREDOR                    0x00000006UL                                       /**< Mode WIREDOR for GPIO_P_MODEH               */
#define _GPIO_P_MODEH_MODE1_WIREDORPULLDOWN            0x00000007UL                                       /**< Mode WIREDORPULLDOWN for GPIO_P_MODEH       */
#define _GPIO_P_MODEH_MODE1_WIREDAND                   0x00000008UL                                       /**< Mode WIREDAND for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE1_WIREDANDFILTER             0x00000009UL                                       /**< Mode WIREDANDFILTER for GPIO_P_MODEH        */
#define _GPIO_P_MODEH_MODE1_WIREDANDPULLUP             0x0000000AUL                                       /**< Mode WIREDANDPULLUP for GPIO_P_MODEH        */
#define _GPIO_P_MODEH_MODE1_WIREDANDPULLUPFILTER       0x0000000BUL                                       /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEH  */
#define _GPIO_P_MODEH_MODE1_WIREDANDALT                0x0000000CUL                                       /**< Mode WIREDANDALT for GPIO_P_MODEH           */
#define _GPIO_P_MODEH_MODE1_WIREDANDALTFILTER          0x0000000DUL                                       /**< Mode WIREDANDALTFILTER for GPIO_P_MODEH     */
#define _GPIO_P_MODEH_MODE1_WIREDANDALTPULLUP          0x0000000EUL                                       /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEH     */
#define _GPIO_P_MODEH_MODE1_WIREDANDALTPULLUPFILTER    0x0000000FUL                                       /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_DEFAULT                     (_GPIO_P_MODEH_MODE1_DEFAULT << 4)                 /**< Shifted mode DEFAULT for GPIO_P_MODEH       */
#define GPIO_P_MODEH_MODE1_DISABLED                    (_GPIO_P_MODEH_MODE1_DISABLED << 4)                /**< Shifted mode DISABLED for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE1_INPUT                       (_GPIO_P_MODEH_MODE1_INPUT << 4)                   /**< Shifted mode INPUT for GPIO_P_MODEH         */
#define GPIO_P_MODEH_MODE1_INPUTPULL                   (_GPIO_P_MODEH_MODE1_INPUTPULL << 4)               /**< Shifted mode INPUTPULL for GPIO_P_MODEH     */
#define GPIO_P_MODEH_MODE1_INPUTPULLFILTER             (_GPIO_P_MODEH_MODE1_INPUTPULLFILTER << 4)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_PUSHPULL                    (_GPIO_P_MODEH_MODE1_PUSHPULL << 4)                /**< Shifted mode PUSHPULL for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE1_PUSHPULLALT                 (_GPIO_P_MODEH_MODE1_PUSHPULLALT << 4)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEH   */
#define GPIO_P_MODEH_MODE1_WIREDOR                     (_GPIO_P_MODEH_MODE1_WIREDOR << 4)                 /**< Shifted mode WIREDOR for GPIO_P_MODEH       */
#define GPIO_P_MODEH_MODE1_WIREDORPULLDOWN             (_GPIO_P_MODEH_MODE1_WIREDORPULLDOWN << 4)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_WIREDAND                    (_GPIO_P_MODEH_MODE1_WIREDAND << 4)                /**< Shifted mode WIREDAND for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE1_WIREDANDFILTER              (_GPIO_P_MODEH_MODE1_WIREDANDFILTER << 4)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_WIREDANDPULLUP              (_GPIO_P_MODEH_MODE1_WIREDANDPULLUP << 4)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_WIREDANDPULLUPFILTER        (_GPIO_P_MODEH_MODE1_WIREDANDPULLUPFILTER << 4)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_WIREDANDALT                 (_GPIO_P_MODEH_MODE1_WIREDANDALT << 4)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEH   */
#define GPIO_P_MODEH_MODE1_WIREDANDALTFILTER           (_GPIO_P_MODEH_MODE1_WIREDANDALTFILTER << 4)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_WIREDANDALTPULLUP           (_GPIO_P_MODEH_MODE1_WIREDANDALTPULLUP << 4)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE1_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEH_MODE1_WIREDANDALTPULLUPFILTER << 4) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEH*/
#define _GPIO_P_MODEH_MODE2_SHIFT                      8                                                  /**< Shift value for GPIO_MODE2                  */
#define _GPIO_P_MODEH_MODE2_MASK                       0xF00UL                                            /**< Bit mask for GPIO_MODE2                     */
#define _GPIO_P_MODEH_MODE2_DEFAULT                    0x00000000UL                                       /**< Mode DEFAULT for GPIO_P_MODEH               */
#define _GPIO_P_MODEH_MODE2_DISABLED                   0x00000000UL                                       /**< Mode DISABLED for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE2_INPUT                      0x00000001UL                                       /**< Mode INPUT for GPIO_P_MODEH                 */
#define _GPIO_P_MODEH_MODE2_INPUTPULL                  0x00000002UL                                       /**< Mode INPUTPULL for GPIO_P_MODEH             */
#define _GPIO_P_MODEH_MODE2_INPUTPULLFILTER            0x00000003UL                                       /**< Mode INPUTPULLFILTER for GPIO_P_MODEH       */
#define _GPIO_P_MODEH_MODE2_PUSHPULL                   0x00000004UL                                       /**< Mode PUSHPULL for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE2_PUSHPULLALT                0x00000005UL                                       /**< Mode PUSHPULLALT for GPIO_P_MODEH           */
#define _GPIO_P_MODEH_MODE2_WIREDOR                    0x00000006UL                                       /**< Mode WIREDOR for GPIO_P_MODEH               */
#define _GPIO_P_MODEH_MODE2_WIREDORPULLDOWN            0x00000007UL                                       /**< Mode WIREDORPULLDOWN for GPIO_P_MODEH       */
#define _GPIO_P_MODEH_MODE2_WIREDAND                   0x00000008UL                                       /**< Mode WIREDAND for GPIO_P_MODEH              */
#define _GPIO_P_MODEH_MODE2_WIREDANDFILTER             0x00000009UL                                       /**< Mode WIREDANDFILTER for GPIO_P_MODEH        */
#define _GPIO_P_MODEH_MODE2_WIREDANDPULLUP             0x0000000AUL                                       /**< Mode WIREDANDPULLUP for GPIO_P_MODEH        */
#define _GPIO_P_MODEH_MODE2_WIREDANDPULLUPFILTER       0x0000000BUL                                       /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEH  */
#define _GPIO_P_MODEH_MODE2_WIREDANDALT                0x0000000CUL                                       /**< Mode WIREDANDALT for GPIO_P_MODEH           */
#define _GPIO_P_MODEH_MODE2_WIREDANDALTFILTER          0x0000000DUL                                       /**< Mode WIREDANDALTFILTER for GPIO_P_MODEH     */
#define _GPIO_P_MODEH_MODE2_WIREDANDALTPULLUP          0x0000000EUL                                       /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEH     */
#define _GPIO_P_MODEH_MODE2_WIREDANDALTPULLUPFILTER    0x0000000FUL                                       /**< Mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_DEFAULT                     (_GPIO_P_MODEH_MODE2_DEFAULT << 8)                 /**< Shifted mode DEFAULT for GPIO_P_MODEH       */
#define GPIO_P_MODEH_MODE2_DISABLED                    (_GPIO_P_MODEH_MODE2_DISABLED << 8)                /**< Shifted mode DISABLED for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE2_INPUT                       (_GPIO_P_MODEH_MODE2_INPUT << 8)                   /**< Shifted mode INPUT for GPIO_P_MODEH         */
#define GPIO_P_MODEH_MODE2_INPUTPULL                   (_GPIO_P_MODEH_MODE2_INPUTPULL << 8)               /**< Shifted mode INPUTPULL for GPIO_P_MODEH     */
#define GPIO_P_MODEH_MODE2_INPUTPULLFILTER             (_GPIO_P_MODEH_MODE2_INPUTPULLFILTER << 8)         /**< Shifted mode INPUTPULLFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_PUSHPULL                    (_GPIO_P_MODEH_MODE2_PUSHPULL << 8)                /**< Shifted mode PUSHPULL for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE2_PUSHPULLALT                 (_GPIO_P_MODEH_MODE2_PUSHPULLALT << 8)             /**< Shifted mode PUSHPULLALT for GPIO_P_MODEH   */
#define GPIO_P_MODEH_MODE2_WIREDOR                     (_GPIO_P_MODEH_MODE2_WIREDOR << 8)                 /**< Shifted mode WIREDOR for GPIO_P_MODEH       */
#define GPIO_P_MODEH_MODE2_WIREDORPULLDOWN             (_GPIO_P_MODEH_MODE2_WIREDORPULLDOWN << 8)         /**< Shifted mode WIREDORPULLDOWN for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_WIREDAND                    (_GPIO_P_MODEH_MODE2_WIREDAND << 8)                /**< Shifted mode WIREDAND for GPIO_P_MODEH      */
#define GPIO_P_MODEH_MODE2_WIREDANDFILTER              (_GPIO_P_MODEH_MODE2_WIREDANDFILTER << 8)          /**< Shifted mode WIREDANDFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_WIREDANDPULLUP              (_GPIO_P_MODEH_MODE2_WIREDANDPULLUP << 8)          /**< Shifted mode WIREDANDPULLUP for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_WIREDANDPULLUPFILTER        (_GPIO_P_MODEH_MODE2_WIREDANDPULLUPFILTER << 8)    /**< Shifted mode WIREDANDPULLUPFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_WIREDANDALT                 (_GPIO_P_MODEH_MODE2_WIREDANDALT << 8)             /**< Shifted mode WIREDANDALT for GPIO_P_MODEH   */
#define GPIO_P_MODEH_MODE2_WIREDANDALTFILTER           (_GPIO_P_MODEH_MODE2_WIREDANDALTFILTER << 8)       /**< Shifted mode WIREDANDALTFILTER for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_WIREDANDALTPULLUP           (_GPIO_P_MODEH_MODE2_WIREDANDALTPULLUP << 8)       /**< Shifted mode WIREDANDALTPULLUP for GPIO_P_MODEH*/
#define GPIO_P_MODEH_MODE2_WIREDANDALTPULLUPFILTER     (_GPIO_P_MODEH_MODE2_WIREDANDALTPULLUPFILTER << 8) /**< Shifted mode WIREDANDALTPULLUPFILTER for GPIO_P_MODEH*/

/* Bit fields for GPIO_P DOUT */
#define _GPIO_P_DOUT_RESETVALUE                        0x00000000UL                     /**< Default value for GPIO_P_DOUT               */
#define _GPIO_P_DOUT_MASK                              0x000007FFUL                     /**< Mask for GPIO_P_DOUT                        */
#define _GPIO_P_DOUT_DOUT_SHIFT                        0                                /**< Shift value for GPIO_DOUT                   */
#define _GPIO_P_DOUT_DOUT_MASK                         0x7FFUL                          /**< Bit mask for GPIO_DOUT                      */
#define _GPIO_P_DOUT_DOUT_DEFAULT                      0x00000000UL                     /**< Mode DEFAULT for GPIO_P_DOUT                */
#define GPIO_P_DOUT_DOUT_DEFAULT                       (_GPIO_P_DOUT_DOUT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_P_DOUT        */

/* Bit fields for GPIO_P DIN */
#define _GPIO_P_DIN_RESETVALUE                         0x00000000UL                     /**< Default value for GPIO_P_DIN                */
#define _GPIO_P_DIN_MASK                               0x000007FFUL                     /**< Mask for GPIO_P_DIN                         */
#define _GPIO_P_DIN_DIN_SHIFT                          0                                /**< Shift value for GPIO_DIN                    */
#define _GPIO_P_DIN_DIN_MASK                           0x7FFUL                          /**< Bit mask for GPIO_DIN                       */
#define _GPIO_P_DIN_DIN_DEFAULT                        0x00000000UL                     /**< Mode DEFAULT for GPIO_P_DIN                 */
#define GPIO_P_DIN_DIN_DEFAULT                         (_GPIO_P_DIN_DIN_DEFAULT << 0)   /**< Shifted mode DEFAULT for GPIO_P_DIN         */
/** @} End of group Parts */

#endif /* GPIO_PORT_H */
#ifdef __cplusplus
}
#endif

