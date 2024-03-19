/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 ICACHE register and bit field definitions
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

#ifndef EFR32ZG23_ICACHE_H
#define EFR32ZG23_ICACHE_H
#define ICACHE_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_ICACHE ICACHE
 * @{
 * @brief EFR32ZG23 ICACHE Register Declaration.
 *****************************************************************************/

/** ICACHE Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version                                         */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IM uint32_t  PCHITS;                        /**< Performance Counter Hits                           */
  __IM uint32_t  PCMISSES;                      /**< Performance Counter Misses                         */
  __IM uint32_t  PCAHITS;                       /**< Performance Counter Advanced Hits                  */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IOM uint32_t LPMODE;                        /**< Low Power Mode                                     */
  __IOM uint32_t IF;                            /**< Interrupt Flag                                     */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  uint32_t       RESERVED0[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version                                         */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IM uint32_t  PCHITS_SET;                    /**< Performance Counter Hits                           */
  __IM uint32_t  PCMISSES_SET;                  /**< Performance Counter Misses                         */
  __IM uint32_t  PCAHITS_SET;                   /**< Performance Counter Advanced Hits                  */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IOM uint32_t LPMODE_SET;                    /**< Low Power Mode                                     */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED1[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version                                         */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IM uint32_t  PCHITS_CLR;                    /**< Performance Counter Hits                           */
  __IM uint32_t  PCMISSES_CLR;                  /**< Performance Counter Misses                         */
  __IM uint32_t  PCAHITS_CLR;                   /**< Performance Counter Advanced Hits                  */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IOM uint32_t LPMODE_CLR;                    /**< Low Power Mode                                     */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED2[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version                                         */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IM uint32_t  PCHITS_TGL;                    /**< Performance Counter Hits                           */
  __IM uint32_t  PCMISSES_TGL;                  /**< Performance Counter Misses                         */
  __IM uint32_t  PCAHITS_TGL;                   /**< Performance Counter Advanced Hits                  */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IOM uint32_t LPMODE_TGL;                    /**< Low Power Mode                                     */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
} ICACHE_TypeDef;
/** @} End of group EFR32ZG23_ICACHE */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_ICACHE
 * @{
 * @defgroup EFR32ZG23_ICACHE_BitFields ICACHE Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for ICACHE IPVERSION */
#define _ICACHE_IPVERSION_RESETVALUE           0x00000000UL                               /**< Default value for ICACHE_IPVERSION          */
#define _ICACHE_IPVERSION_MASK                 0xFFFFFFFFUL                               /**< Mask for ICACHE_IPVERSION                   */
#define _ICACHE_IPVERSION_IPVERSION_SHIFT      0                                          /**< Shift value for ICACHE_IPVERSION            */
#define _ICACHE_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                               /**< Bit mask for ICACHE_IPVERSION               */
#define _ICACHE_IPVERSION_IPVERSION_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for ICACHE_IPVERSION           */
#define ICACHE_IPVERSION_IPVERSION_DEFAULT     (_ICACHE_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for ICACHE_IPVERSION   */

/* Bit fields for ICACHE CTRL */
#define _ICACHE_CTRL_RESETVALUE                0x00000000UL                             /**< Default value for ICACHE_CTRL               */
#define _ICACHE_CTRL_MASK                      0x00000007UL                             /**< Mask for ICACHE_CTRL                        */
#define ICACHE_CTRL_CACHEDIS                   (0x1UL << 0)                             /**< Cache Disable                               */
#define _ICACHE_CTRL_CACHEDIS_SHIFT            0                                        /**< Shift value for ICACHE_CACHEDIS             */
#define _ICACHE_CTRL_CACHEDIS_MASK             0x1UL                                    /**< Bit mask for ICACHE_CACHEDIS                */
#define _ICACHE_CTRL_CACHEDIS_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for ICACHE_CTRL                */
#define ICACHE_CTRL_CACHEDIS_DEFAULT           (_ICACHE_CTRL_CACHEDIS_DEFAULT << 0)     /**< Shifted mode DEFAULT for ICACHE_CTRL        */
#define ICACHE_CTRL_USEMPU                     (0x1UL << 1)                             /**< Use MPU                                     */
#define _ICACHE_CTRL_USEMPU_SHIFT              1                                        /**< Shift value for ICACHE_USEMPU               */
#define _ICACHE_CTRL_USEMPU_MASK               0x2UL                                    /**< Bit mask for ICACHE_USEMPU                  */
#define _ICACHE_CTRL_USEMPU_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for ICACHE_CTRL                */
#define ICACHE_CTRL_USEMPU_DEFAULT             (_ICACHE_CTRL_USEMPU_DEFAULT << 1)       /**< Shifted mode DEFAULT for ICACHE_CTRL        */
#define ICACHE_CTRL_AUTOFLUSHDIS               (0x1UL << 2)                             /**< Automatic Flushing Disable                  */
#define _ICACHE_CTRL_AUTOFLUSHDIS_SHIFT        2                                        /**< Shift value for ICACHE_AUTOFLUSHDIS         */
#define _ICACHE_CTRL_AUTOFLUSHDIS_MASK         0x4UL                                    /**< Bit mask for ICACHE_AUTOFLUSHDIS            */
#define _ICACHE_CTRL_AUTOFLUSHDIS_DEFAULT      0x00000000UL                             /**< Mode DEFAULT for ICACHE_CTRL                */
#define ICACHE_CTRL_AUTOFLUSHDIS_DEFAULT       (_ICACHE_CTRL_AUTOFLUSHDIS_DEFAULT << 2) /**< Shifted mode DEFAULT for ICACHE_CTRL        */

/* Bit fields for ICACHE PCHITS */
#define _ICACHE_PCHITS_RESETVALUE              0x00000000UL                             /**< Default value for ICACHE_PCHITS             */
#define _ICACHE_PCHITS_MASK                    0xFFFFFFFFUL                             /**< Mask for ICACHE_PCHITS                      */
#define _ICACHE_PCHITS_PCHITS_SHIFT            0                                        /**< Shift value for ICACHE_PCHITS               */
#define _ICACHE_PCHITS_PCHITS_MASK             0xFFFFFFFFUL                             /**< Bit mask for ICACHE_PCHITS                  */
#define _ICACHE_PCHITS_PCHITS_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for ICACHE_PCHITS              */
#define ICACHE_PCHITS_PCHITS_DEFAULT           (_ICACHE_PCHITS_PCHITS_DEFAULT << 0)     /**< Shifted mode DEFAULT for ICACHE_PCHITS      */

/* Bit fields for ICACHE PCMISSES */
#define _ICACHE_PCMISSES_RESETVALUE            0x00000000UL                             /**< Default value for ICACHE_PCMISSES           */
#define _ICACHE_PCMISSES_MASK                  0xFFFFFFFFUL                             /**< Mask for ICACHE_PCMISSES                    */
#define _ICACHE_PCMISSES_PCMISSES_SHIFT        0                                        /**< Shift value for ICACHE_PCMISSES             */
#define _ICACHE_PCMISSES_PCMISSES_MASK         0xFFFFFFFFUL                             /**< Bit mask for ICACHE_PCMISSES                */
#define _ICACHE_PCMISSES_PCMISSES_DEFAULT      0x00000000UL                             /**< Mode DEFAULT for ICACHE_PCMISSES            */
#define ICACHE_PCMISSES_PCMISSES_DEFAULT       (_ICACHE_PCMISSES_PCMISSES_DEFAULT << 0) /**< Shifted mode DEFAULT for ICACHE_PCMISSES    */

/* Bit fields for ICACHE PCAHITS */
#define _ICACHE_PCAHITS_RESETVALUE             0x00000000UL                             /**< Default value for ICACHE_PCAHITS            */
#define _ICACHE_PCAHITS_MASK                   0xFFFFFFFFUL                             /**< Mask for ICACHE_PCAHITS                     */
#define _ICACHE_PCAHITS_PCAHITS_SHIFT          0                                        /**< Shift value for ICACHE_PCAHITS              */
#define _ICACHE_PCAHITS_PCAHITS_MASK           0xFFFFFFFFUL                             /**< Bit mask for ICACHE_PCAHITS                 */
#define _ICACHE_PCAHITS_PCAHITS_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for ICACHE_PCAHITS             */
#define ICACHE_PCAHITS_PCAHITS_DEFAULT         (_ICACHE_PCAHITS_PCAHITS_DEFAULT << 0)   /**< Shifted mode DEFAULT for ICACHE_PCAHITS     */

/* Bit fields for ICACHE STATUS */
#define _ICACHE_STATUS_RESETVALUE              0x00000000UL                             /**< Default value for ICACHE_STATUS             */
#define _ICACHE_STATUS_MASK                    0x00000001UL                             /**< Mask for ICACHE_STATUS                      */
#define ICACHE_STATUS_PCRUNNING                (0x1UL << 0)                             /**< PC Running                                  */
#define _ICACHE_STATUS_PCRUNNING_SHIFT         0                                        /**< Shift value for ICACHE_PCRUNNING            */
#define _ICACHE_STATUS_PCRUNNING_MASK          0x1UL                                    /**< Bit mask for ICACHE_PCRUNNING               */
#define _ICACHE_STATUS_PCRUNNING_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for ICACHE_STATUS              */
#define ICACHE_STATUS_PCRUNNING_DEFAULT        (_ICACHE_STATUS_PCRUNNING_DEFAULT << 0)  /**< Shifted mode DEFAULT for ICACHE_STATUS      */

/* Bit fields for ICACHE CMD */
#define _ICACHE_CMD_RESETVALUE                 0x00000000UL                             /**< Default value for ICACHE_CMD                */
#define _ICACHE_CMD_MASK                       0x00000007UL                             /**< Mask for ICACHE_CMD                         */
#define ICACHE_CMD_FLUSH                       (0x1UL << 0)                             /**< Flush                                       */
#define _ICACHE_CMD_FLUSH_SHIFT                0                                        /**< Shift value for ICACHE_FLUSH                */
#define _ICACHE_CMD_FLUSH_MASK                 0x1UL                                    /**< Bit mask for ICACHE_FLUSH                   */
#define _ICACHE_CMD_FLUSH_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for ICACHE_CMD                 */
#define ICACHE_CMD_FLUSH_DEFAULT               (_ICACHE_CMD_FLUSH_DEFAULT << 0)         /**< Shifted mode DEFAULT for ICACHE_CMD         */
#define ICACHE_CMD_STARTPC                     (0x1UL << 1)                             /**< Start Performance Counters                  */
#define _ICACHE_CMD_STARTPC_SHIFT              1                                        /**< Shift value for ICACHE_STARTPC              */
#define _ICACHE_CMD_STARTPC_MASK               0x2UL                                    /**< Bit mask for ICACHE_STARTPC                 */
#define _ICACHE_CMD_STARTPC_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for ICACHE_CMD                 */
#define ICACHE_CMD_STARTPC_DEFAULT             (_ICACHE_CMD_STARTPC_DEFAULT << 1)       /**< Shifted mode DEFAULT for ICACHE_CMD         */
#define ICACHE_CMD_STOPPC                      (0x1UL << 2)                             /**< Stop Performance Counters                   */
#define _ICACHE_CMD_STOPPC_SHIFT               2                                        /**< Shift value for ICACHE_STOPPC               */
#define _ICACHE_CMD_STOPPC_MASK                0x4UL                                    /**< Bit mask for ICACHE_STOPPC                  */
#define _ICACHE_CMD_STOPPC_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for ICACHE_CMD                 */
#define ICACHE_CMD_STOPPC_DEFAULT              (_ICACHE_CMD_STOPPC_DEFAULT << 2)        /**< Shifted mode DEFAULT for ICACHE_CMD         */

/* Bit fields for ICACHE LPMODE */
#define _ICACHE_LPMODE_RESETVALUE              0x00000023UL                              /**< Default value for ICACHE_LPMODE             */
#define _ICACHE_LPMODE_MASK                    0x000000F3UL                              /**< Mask for ICACHE_LPMODE                      */
#define _ICACHE_LPMODE_LPLEVEL_SHIFT           0                                         /**< Shift value for ICACHE_LPLEVEL              */
#define _ICACHE_LPMODE_LPLEVEL_MASK            0x3UL                                     /**< Bit mask for ICACHE_LPLEVEL                 */
#define _ICACHE_LPMODE_LPLEVEL_DEFAULT         0x00000003UL                              /**< Mode DEFAULT for ICACHE_LPMODE              */
#define _ICACHE_LPMODE_LPLEVEL_BASIC           0x00000000UL                              /**< Mode BASIC for ICACHE_LPMODE                */
#define _ICACHE_LPMODE_LPLEVEL_ADVANCED        0x00000001UL                              /**< Mode ADVANCED for ICACHE_LPMODE             */
#define _ICACHE_LPMODE_LPLEVEL_MINACTIVITY     0x00000003UL                              /**< Mode MINACTIVITY for ICACHE_LPMODE          */
#define ICACHE_LPMODE_LPLEVEL_DEFAULT          (_ICACHE_LPMODE_LPLEVEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for ICACHE_LPMODE      */
#define ICACHE_LPMODE_LPLEVEL_BASIC            (_ICACHE_LPMODE_LPLEVEL_BASIC << 0)       /**< Shifted mode BASIC for ICACHE_LPMODE        */
#define ICACHE_LPMODE_LPLEVEL_ADVANCED         (_ICACHE_LPMODE_LPLEVEL_ADVANCED << 0)    /**< Shifted mode ADVANCED for ICACHE_LPMODE     */
#define ICACHE_LPMODE_LPLEVEL_MINACTIVITY      (_ICACHE_LPMODE_LPLEVEL_MINACTIVITY << 0) /**< Shifted mode MINACTIVITY for ICACHE_LPMODE  */
#define _ICACHE_LPMODE_NESTFACTOR_SHIFT        4                                         /**< Shift value for ICACHE_NESTFACTOR           */
#define _ICACHE_LPMODE_NESTFACTOR_MASK         0xF0UL                                    /**< Bit mask for ICACHE_NESTFACTOR              */
#define _ICACHE_LPMODE_NESTFACTOR_DEFAULT      0x00000002UL                              /**< Mode DEFAULT for ICACHE_LPMODE              */
#define ICACHE_LPMODE_NESTFACTOR_DEFAULT       (_ICACHE_LPMODE_NESTFACTOR_DEFAULT << 4)  /**< Shifted mode DEFAULT for ICACHE_LPMODE      */

/* Bit fields for ICACHE IF */
#define _ICACHE_IF_RESETVALUE                  0x00000000UL                             /**< Default value for ICACHE_IF                 */
#define _ICACHE_IF_MASK                        0x00000107UL                             /**< Mask for ICACHE_IF                          */
#define ICACHE_IF_HITOF                        (0x1UL << 0)                             /**< Hit Overflow Interrupt Flag                 */
#define _ICACHE_IF_HITOF_SHIFT                 0                                        /**< Shift value for ICACHE_HITOF                */
#define _ICACHE_IF_HITOF_MASK                  0x1UL                                    /**< Bit mask for ICACHE_HITOF                   */
#define _ICACHE_IF_HITOF_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for ICACHE_IF                  */
#define ICACHE_IF_HITOF_DEFAULT                (_ICACHE_IF_HITOF_DEFAULT << 0)          /**< Shifted mode DEFAULT for ICACHE_IF          */
#define ICACHE_IF_MISSOF                       (0x1UL << 1)                             /**< Miss Overflow Interrupt Flag                */
#define _ICACHE_IF_MISSOF_SHIFT                1                                        /**< Shift value for ICACHE_MISSOF               */
#define _ICACHE_IF_MISSOF_MASK                 0x2UL                                    /**< Bit mask for ICACHE_MISSOF                  */
#define _ICACHE_IF_MISSOF_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for ICACHE_IF                  */
#define ICACHE_IF_MISSOF_DEFAULT               (_ICACHE_IF_MISSOF_DEFAULT << 1)         /**< Shifted mode DEFAULT for ICACHE_IF          */
#define ICACHE_IF_AHITOF                       (0x1UL << 2)                             /**< Advanced Hit Overflow Interrupt Flag        */
#define _ICACHE_IF_AHITOF_SHIFT                2                                        /**< Shift value for ICACHE_AHITOF               */
#define _ICACHE_IF_AHITOF_MASK                 0x4UL                                    /**< Bit mask for ICACHE_AHITOF                  */
#define _ICACHE_IF_AHITOF_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for ICACHE_IF                  */
#define ICACHE_IF_AHITOF_DEFAULT               (_ICACHE_IF_AHITOF_DEFAULT << 2)         /**< Shifted mode DEFAULT for ICACHE_IF          */
#define ICACHE_IF_RAMERROR                     (0x1UL << 8)                             /**< RAM error Interrupt Flag                    */
#define _ICACHE_IF_RAMERROR_SHIFT              8                                        /**< Shift value for ICACHE_RAMERROR             */
#define _ICACHE_IF_RAMERROR_MASK               0x100UL                                  /**< Bit mask for ICACHE_RAMERROR                */
#define _ICACHE_IF_RAMERROR_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for ICACHE_IF                  */
#define ICACHE_IF_RAMERROR_DEFAULT             (_ICACHE_IF_RAMERROR_DEFAULT << 8)       /**< Shifted mode DEFAULT for ICACHE_IF          */

/* Bit fields for ICACHE IEN */
#define _ICACHE_IEN_RESETVALUE                 0x00000000UL                             /**< Default value for ICACHE_IEN                */
#define _ICACHE_IEN_MASK                       0x00000107UL                             /**< Mask for ICACHE_IEN                         */
#define ICACHE_IEN_HITOF                       (0x1UL << 0)                             /**< Hit Overflow Interrupt Enable               */
#define _ICACHE_IEN_HITOF_SHIFT                0                                        /**< Shift value for ICACHE_HITOF                */
#define _ICACHE_IEN_HITOF_MASK                 0x1UL                                    /**< Bit mask for ICACHE_HITOF                   */
#define _ICACHE_IEN_HITOF_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for ICACHE_IEN                 */
#define ICACHE_IEN_HITOF_DEFAULT               (_ICACHE_IEN_HITOF_DEFAULT << 0)         /**< Shifted mode DEFAULT for ICACHE_IEN         */
#define ICACHE_IEN_MISSOF                      (0x1UL << 1)                             /**< Miss Overflow Interrupt Enable              */
#define _ICACHE_IEN_MISSOF_SHIFT               1                                        /**< Shift value for ICACHE_MISSOF               */
#define _ICACHE_IEN_MISSOF_MASK                0x2UL                                    /**< Bit mask for ICACHE_MISSOF                  */
#define _ICACHE_IEN_MISSOF_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for ICACHE_IEN                 */
#define ICACHE_IEN_MISSOF_DEFAULT              (_ICACHE_IEN_MISSOF_DEFAULT << 1)        /**< Shifted mode DEFAULT for ICACHE_IEN         */
#define ICACHE_IEN_AHITOF                      (0x1UL << 2)                             /**< Advanced Hit Overflow Interrupt Enable      */
#define _ICACHE_IEN_AHITOF_SHIFT               2                                        /**< Shift value for ICACHE_AHITOF               */
#define _ICACHE_IEN_AHITOF_MASK                0x4UL                                    /**< Bit mask for ICACHE_AHITOF                  */
#define _ICACHE_IEN_AHITOF_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for ICACHE_IEN                 */
#define ICACHE_IEN_AHITOF_DEFAULT              (_ICACHE_IEN_AHITOF_DEFAULT << 2)        /**< Shifted mode DEFAULT for ICACHE_IEN         */
#define ICACHE_IEN_RAMERROR                    (0x1UL << 8)                             /**< RAM error Interrupt Enable                  */
#define _ICACHE_IEN_RAMERROR_SHIFT             8                                        /**< Shift value for ICACHE_RAMERROR             */
#define _ICACHE_IEN_RAMERROR_MASK              0x100UL                                  /**< Bit mask for ICACHE_RAMERROR                */
#define _ICACHE_IEN_RAMERROR_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for ICACHE_IEN                 */
#define ICACHE_IEN_RAMERROR_DEFAULT            (_ICACHE_IEN_RAMERROR_DEFAULT << 8)      /**< Shifted mode DEFAULT for ICACHE_IEN         */

/** @} End of group EFR32ZG23_ICACHE_BitFields */
/** @} End of group EFR32ZG23_ICACHE */
/** @} End of group Parts */

#endif /* EFR32ZG23_ICACHE_H */
#ifdef __cplusplus
}
#endif

