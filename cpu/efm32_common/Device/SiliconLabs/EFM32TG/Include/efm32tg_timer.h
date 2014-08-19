/**************************************************************************//**
 * @file efm32tg_timer.h
 * @brief EFM32TG_TIMER register and bit field definitions
 * @version 3.20.6
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/
/**************************************************************************//**
 * @defgroup EFM32TG_TIMER
 * @brief EFM32TG_TIMER Register Declaration
 * @{
 *****************************************************************************/
typedef struct
{
  __IO uint32_t    CTRL;         /**< Control Register  */
  __IO uint32_t    CMD;          /**< Command Register  */
  __I uint32_t     STATUS;       /**< Status Register  */
  __IO uint32_t    IEN;          /**< Interrupt Enable Register  */
  __I uint32_t     IF;           /**< Interrupt Flag Register  */
  __IO uint32_t    IFS;          /**< Interrupt Flag Set Register  */
  __IO uint32_t    IFC;          /**< Interrupt Flag Clear Register  */
  __IO uint32_t    TOP;          /**< Counter Top Value Register  */
  __IO uint32_t    TOPB;         /**< Counter Top Value Buffer Register  */
  __IO uint32_t    CNT;          /**< Counter Value Register  */
  __IO uint32_t    ROUTE;        /**< I/O Routing Register  */

  uint32_t         RESERVED0[1]; /**< Reserved registers */

  TIMER_CC_TypeDef CC[3];        /**< Compare/Capture Channel */
} TIMER_TypeDef;                 /** @} */

/**************************************************************************//**
 * @defgroup EFM32TG_TIMER_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for TIMER CTRL */
#define _TIMER_CTRL_RESETVALUE                     0x00000000UL                             /**< Default value for TIMER_CTRL */
#define _TIMER_CTRL_MASK                           0x3F032FFBUL                             /**< Mask for TIMER_CTRL */
#define _TIMER_CTRL_MODE_SHIFT                     0                                        /**< Shift value for TIMER_MODE */
#define _TIMER_CTRL_MODE_MASK                      0x3UL                                    /**< Bit mask for TIMER_MODE */
#define _TIMER_CTRL_MODE_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_MODE_UP                        0x00000000UL                             /**< Mode UP for TIMER_CTRL */
#define _TIMER_CTRL_MODE_DOWN                      0x00000001UL                             /**< Mode DOWN for TIMER_CTRL */
#define _TIMER_CTRL_MODE_UPDOWN                    0x00000002UL                             /**< Mode UPDOWN for TIMER_CTRL */
#define _TIMER_CTRL_MODE_QDEC                      0x00000003UL                             /**< Mode QDEC for TIMER_CTRL */
#define TIMER_CTRL_MODE_DEFAULT                    (_TIMER_CTRL_MODE_DEFAULT << 0)          /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_MODE_UP                         (_TIMER_CTRL_MODE_UP << 0)               /**< Shifted mode UP for TIMER_CTRL */
#define TIMER_CTRL_MODE_DOWN                       (_TIMER_CTRL_MODE_DOWN << 0)             /**< Shifted mode DOWN for TIMER_CTRL */
#define TIMER_CTRL_MODE_UPDOWN                     (_TIMER_CTRL_MODE_UPDOWN << 0)           /**< Shifted mode UPDOWN for TIMER_CTRL */
#define TIMER_CTRL_MODE_QDEC                       (_TIMER_CTRL_MODE_QDEC << 0)             /**< Shifted mode QDEC for TIMER_CTRL */
#define TIMER_CTRL_SYNC                            (0x1UL << 3)                             /**< Timer Start/Stop/Reload Synchronization */
#define _TIMER_CTRL_SYNC_SHIFT                     3                                        /**< Shift value for TIMER_SYNC */
#define _TIMER_CTRL_SYNC_MASK                      0x8UL                                    /**< Bit mask for TIMER_SYNC */
#define _TIMER_CTRL_SYNC_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_SYNC_DEFAULT                    (_TIMER_CTRL_SYNC_DEFAULT << 3)          /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_OSMEN                           (0x1UL << 4)                             /**< One-shot Mode Enable */
#define _TIMER_CTRL_OSMEN_SHIFT                    4                                        /**< Shift value for TIMER_OSMEN */
#define _TIMER_CTRL_OSMEN_MASK                     0x10UL                                   /**< Bit mask for TIMER_OSMEN */
#define _TIMER_CTRL_OSMEN_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_OSMEN_DEFAULT                   (_TIMER_CTRL_OSMEN_DEFAULT << 4)         /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_QDM                             (0x1UL << 5)                             /**< Quadrature Decoder Mode Selection */
#define _TIMER_CTRL_QDM_SHIFT                      5                                        /**< Shift value for TIMER_QDM */
#define _TIMER_CTRL_QDM_MASK                       0x20UL                                   /**< Bit mask for TIMER_QDM */
#define _TIMER_CTRL_QDM_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_QDM_X2                         0x00000000UL                             /**< Mode X2 for TIMER_CTRL */
#define _TIMER_CTRL_QDM_X4                         0x00000001UL                             /**< Mode X4 for TIMER_CTRL */
#define TIMER_CTRL_QDM_DEFAULT                     (_TIMER_CTRL_QDM_DEFAULT << 5)           /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_QDM_X2                          (_TIMER_CTRL_QDM_X2 << 5)                /**< Shifted mode X2 for TIMER_CTRL */
#define TIMER_CTRL_QDM_X4                          (_TIMER_CTRL_QDM_X4 << 5)                /**< Shifted mode X4 for TIMER_CTRL */
#define TIMER_CTRL_DEBUGRUN                        (0x1UL << 6)                             /**< Debug Mode Run Enable */
#define _TIMER_CTRL_DEBUGRUN_SHIFT                 6                                        /**< Shift value for TIMER_DEBUGRUN */
#define _TIMER_CTRL_DEBUGRUN_MASK                  0x40UL                                   /**< Bit mask for TIMER_DEBUGRUN */
#define _TIMER_CTRL_DEBUGRUN_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_DEBUGRUN_DEFAULT                (_TIMER_CTRL_DEBUGRUN_DEFAULT << 6)      /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_DMACLRACT                       (0x1UL << 7)                             /**< DMA Request Clear on Active */
#define _TIMER_CTRL_DMACLRACT_SHIFT                7                                        /**< Shift value for TIMER_DMACLRACT */
#define _TIMER_CTRL_DMACLRACT_MASK                 0x80UL                                   /**< Bit mask for TIMER_DMACLRACT */
#define _TIMER_CTRL_DMACLRACT_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_DMACLRACT_DEFAULT               (_TIMER_CTRL_DMACLRACT_DEFAULT << 7)     /**< Shifted mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_RISEA_SHIFT                    8                                        /**< Shift value for TIMER_RISEA */
#define _TIMER_CTRL_RISEA_MASK                     0x300UL                                  /**< Bit mask for TIMER_RISEA */
#define _TIMER_CTRL_RISEA_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_RISEA_NONE                     0x00000000UL                             /**< Mode NONE for TIMER_CTRL */
#define _TIMER_CTRL_RISEA_START                    0x00000001UL                             /**< Mode START for TIMER_CTRL */
#define _TIMER_CTRL_RISEA_STOP                     0x00000002UL                             /**< Mode STOP for TIMER_CTRL */
#define _TIMER_CTRL_RISEA_RELOADSTART              0x00000003UL                             /**< Mode RELOADSTART for TIMER_CTRL */
#define TIMER_CTRL_RISEA_DEFAULT                   (_TIMER_CTRL_RISEA_DEFAULT << 8)         /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_RISEA_NONE                      (_TIMER_CTRL_RISEA_NONE << 8)            /**< Shifted mode NONE for TIMER_CTRL */
#define TIMER_CTRL_RISEA_START                     (_TIMER_CTRL_RISEA_START << 8)           /**< Shifted mode START for TIMER_CTRL */
#define TIMER_CTRL_RISEA_STOP                      (_TIMER_CTRL_RISEA_STOP << 8)            /**< Shifted mode STOP for TIMER_CTRL */
#define TIMER_CTRL_RISEA_RELOADSTART               (_TIMER_CTRL_RISEA_RELOADSTART << 8)     /**< Shifted mode RELOADSTART for TIMER_CTRL */
#define _TIMER_CTRL_FALLA_SHIFT                    10                                       /**< Shift value for TIMER_FALLA */
#define _TIMER_CTRL_FALLA_MASK                     0xC00UL                                  /**< Bit mask for TIMER_FALLA */
#define _TIMER_CTRL_FALLA_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_FALLA_NONE                     0x00000000UL                             /**< Mode NONE for TIMER_CTRL */
#define _TIMER_CTRL_FALLA_START                    0x00000001UL                             /**< Mode START for TIMER_CTRL */
#define _TIMER_CTRL_FALLA_STOP                     0x00000002UL                             /**< Mode STOP for TIMER_CTRL */
#define _TIMER_CTRL_FALLA_RELOADSTART              0x00000003UL                             /**< Mode RELOADSTART for TIMER_CTRL */
#define TIMER_CTRL_FALLA_DEFAULT                   (_TIMER_CTRL_FALLA_DEFAULT << 10)        /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_FALLA_NONE                      (_TIMER_CTRL_FALLA_NONE << 10)           /**< Shifted mode NONE for TIMER_CTRL */
#define TIMER_CTRL_FALLA_START                     (_TIMER_CTRL_FALLA_START << 10)          /**< Shifted mode START for TIMER_CTRL */
#define TIMER_CTRL_FALLA_STOP                      (_TIMER_CTRL_FALLA_STOP << 10)           /**< Shifted mode STOP for TIMER_CTRL */
#define TIMER_CTRL_FALLA_RELOADSTART               (_TIMER_CTRL_FALLA_RELOADSTART << 10)    /**< Shifted mode RELOADSTART for TIMER_CTRL */
#define TIMER_CTRL_X2CNT                           (0x1UL << 13)                            /**< 2x Count Mode */
#define _TIMER_CTRL_X2CNT_SHIFT                    13                                       /**< Shift value for TIMER_X2CNT */
#define _TIMER_CTRL_X2CNT_MASK                     0x2000UL                                 /**< Bit mask for TIMER_X2CNT */
#define _TIMER_CTRL_X2CNT_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_X2CNT_DEFAULT                   (_TIMER_CTRL_X2CNT_DEFAULT << 13)        /**< Shifted mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_CLKSEL_SHIFT                   16                                       /**< Shift value for TIMER_CLKSEL */
#define _TIMER_CTRL_CLKSEL_MASK                    0x30000UL                                /**< Bit mask for TIMER_CLKSEL */
#define _TIMER_CTRL_CLKSEL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_CLKSEL_PRESCHFPERCLK           0x00000000UL                             /**< Mode PRESCHFPERCLK for TIMER_CTRL */
#define _TIMER_CTRL_CLKSEL_CC1                     0x00000001UL                             /**< Mode CC1 for TIMER_CTRL */
#define _TIMER_CTRL_CLKSEL_TIMEROUF                0x00000002UL                             /**< Mode TIMEROUF for TIMER_CTRL */
#define TIMER_CTRL_CLKSEL_DEFAULT                  (_TIMER_CTRL_CLKSEL_DEFAULT << 16)       /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_CLKSEL_PRESCHFPERCLK            (_TIMER_CTRL_CLKSEL_PRESCHFPERCLK << 16) /**< Shifted mode PRESCHFPERCLK for TIMER_CTRL */
#define TIMER_CTRL_CLKSEL_CC1                      (_TIMER_CTRL_CLKSEL_CC1 << 16)           /**< Shifted mode CC1 for TIMER_CTRL */
#define TIMER_CTRL_CLKSEL_TIMEROUF                 (_TIMER_CTRL_CLKSEL_TIMEROUF << 16)      /**< Shifted mode TIMEROUF for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_SHIFT                    24                                       /**< Shift value for TIMER_PRESC */
#define _TIMER_CTRL_PRESC_MASK                     0xF000000UL                              /**< Bit mask for TIMER_PRESC */
#define _TIMER_CTRL_PRESC_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV1                     0x00000000UL                             /**< Mode DIV1 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV2                     0x00000001UL                             /**< Mode DIV2 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV4                     0x00000002UL                             /**< Mode DIV4 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV8                     0x00000003UL                             /**< Mode DIV8 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV16                    0x00000004UL                             /**< Mode DIV16 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV32                    0x00000005UL                             /**< Mode DIV32 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV64                    0x00000006UL                             /**< Mode DIV64 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV128                   0x00000007UL                             /**< Mode DIV128 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV256                   0x00000008UL                             /**< Mode DIV256 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV512                   0x00000009UL                             /**< Mode DIV512 for TIMER_CTRL */
#define _TIMER_CTRL_PRESC_DIV1024                  0x0000000AUL                             /**< Mode DIV1024 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DEFAULT                   (_TIMER_CTRL_PRESC_DEFAULT << 24)        /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV1                      (_TIMER_CTRL_PRESC_DIV1 << 24)           /**< Shifted mode DIV1 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV2                      (_TIMER_CTRL_PRESC_DIV2 << 24)           /**< Shifted mode DIV2 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV4                      (_TIMER_CTRL_PRESC_DIV4 << 24)           /**< Shifted mode DIV4 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV8                      (_TIMER_CTRL_PRESC_DIV8 << 24)           /**< Shifted mode DIV8 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV16                     (_TIMER_CTRL_PRESC_DIV16 << 24)          /**< Shifted mode DIV16 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV32                     (_TIMER_CTRL_PRESC_DIV32 << 24)          /**< Shifted mode DIV32 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV64                     (_TIMER_CTRL_PRESC_DIV64 << 24)          /**< Shifted mode DIV64 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV128                    (_TIMER_CTRL_PRESC_DIV128 << 24)         /**< Shifted mode DIV128 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV256                    (_TIMER_CTRL_PRESC_DIV256 << 24)         /**< Shifted mode DIV256 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV512                    (_TIMER_CTRL_PRESC_DIV512 << 24)         /**< Shifted mode DIV512 for TIMER_CTRL */
#define TIMER_CTRL_PRESC_DIV1024                   (_TIMER_CTRL_PRESC_DIV1024 << 24)        /**< Shifted mode DIV1024 for TIMER_CTRL */
#define TIMER_CTRL_ATI                             (0x1UL << 28)                            /**< Always Track Inputs */
#define _TIMER_CTRL_ATI_SHIFT                      28                                       /**< Shift value for TIMER_ATI */
#define _TIMER_CTRL_ATI_MASK                       0x10000000UL                             /**< Bit mask for TIMER_ATI */
#define _TIMER_CTRL_ATI_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_ATI_DEFAULT                     (_TIMER_CTRL_ATI_DEFAULT << 28)          /**< Shifted mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_RSSCOIST                        (0x1UL << 29)                            /**< Reload-Start Sets Compare Ouptut initial State */
#define _TIMER_CTRL_RSSCOIST_SHIFT                 29                                       /**< Shift value for TIMER_RSSCOIST */
#define _TIMER_CTRL_RSSCOIST_MASK                  0x20000000UL                             /**< Bit mask for TIMER_RSSCOIST */
#define _TIMER_CTRL_RSSCOIST_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for TIMER_CTRL */
#define TIMER_CTRL_RSSCOIST_DEFAULT                (_TIMER_CTRL_RSSCOIST_DEFAULT << 29)     /**< Shifted mode DEFAULT for TIMER_CTRL */

/* Bit fields for TIMER CMD */
#define _TIMER_CMD_RESETVALUE                      0x00000000UL                    /**< Default value for TIMER_CMD */
#define _TIMER_CMD_MASK                            0x00000003UL                    /**< Mask for TIMER_CMD */
#define TIMER_CMD_START                            (0x1UL << 0)                    /**< Start Timer */
#define _TIMER_CMD_START_SHIFT                     0                               /**< Shift value for TIMER_START */
#define _TIMER_CMD_START_MASK                      0x1UL                           /**< Bit mask for TIMER_START */
#define _TIMER_CMD_START_DEFAULT                   0x00000000UL                    /**< Mode DEFAULT for TIMER_CMD */
#define TIMER_CMD_START_DEFAULT                    (_TIMER_CMD_START_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_CMD */
#define TIMER_CMD_STOP                             (0x1UL << 1)                    /**< Stop Timer */
#define _TIMER_CMD_STOP_SHIFT                      1                               /**< Shift value for TIMER_STOP */
#define _TIMER_CMD_STOP_MASK                       0x2UL                           /**< Bit mask for TIMER_STOP */
#define _TIMER_CMD_STOP_DEFAULT                    0x00000000UL                    /**< Mode DEFAULT for TIMER_CMD */
#define TIMER_CMD_STOP_DEFAULT                     (_TIMER_CMD_STOP_DEFAULT << 1)  /**< Shifted mode DEFAULT for TIMER_CMD */

/* Bit fields for TIMER STATUS */
#define _TIMER_STATUS_RESETVALUE                   0x00000000UL                          /**< Default value for TIMER_STATUS */
#define _TIMER_STATUS_MASK                         0x07070707UL                          /**< Mask for TIMER_STATUS */
#define TIMER_STATUS_RUNNING                       (0x1UL << 0)                          /**< Running */
#define _TIMER_STATUS_RUNNING_SHIFT                0                                     /**< Shift value for TIMER_RUNNING */
#define _TIMER_STATUS_RUNNING_MASK                 0x1UL                                 /**< Bit mask for TIMER_RUNNING */
#define _TIMER_STATUS_RUNNING_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_RUNNING_DEFAULT               (_TIMER_STATUS_RUNNING_DEFAULT << 0)  /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_DIR                           (0x1UL << 1)                          /**< Direction */
#define _TIMER_STATUS_DIR_SHIFT                    1                                     /**< Shift value for TIMER_DIR */
#define _TIMER_STATUS_DIR_MASK                     0x2UL                                 /**< Bit mask for TIMER_DIR */
#define _TIMER_STATUS_DIR_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define _TIMER_STATUS_DIR_UP                       0x00000000UL                          /**< Mode UP for TIMER_STATUS */
#define _TIMER_STATUS_DIR_DOWN                     0x00000001UL                          /**< Mode DOWN for TIMER_STATUS */
#define TIMER_STATUS_DIR_DEFAULT                   (_TIMER_STATUS_DIR_DEFAULT << 1)      /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_DIR_UP                        (_TIMER_STATUS_DIR_UP << 1)           /**< Shifted mode UP for TIMER_STATUS */
#define TIMER_STATUS_DIR_DOWN                      (_TIMER_STATUS_DIR_DOWN << 1)         /**< Shifted mode DOWN for TIMER_STATUS */
#define TIMER_STATUS_TOPBV                         (0x1UL << 2)                          /**< TOPB Valid */
#define _TIMER_STATUS_TOPBV_SHIFT                  2                                     /**< Shift value for TIMER_TOPBV */
#define _TIMER_STATUS_TOPBV_MASK                   0x4UL                                 /**< Bit mask for TIMER_TOPBV */
#define _TIMER_STATUS_TOPBV_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_TOPBV_DEFAULT                 (_TIMER_STATUS_TOPBV_DEFAULT << 2)    /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCVBV0                        (0x1UL << 8)                          /**< CC0 CCVB Valid */
#define _TIMER_STATUS_CCVBV0_SHIFT                 8                                     /**< Shift value for TIMER_CCVBV0 */
#define _TIMER_STATUS_CCVBV0_MASK                  0x100UL                               /**< Bit mask for TIMER_CCVBV0 */
#define _TIMER_STATUS_CCVBV0_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCVBV0_DEFAULT                (_TIMER_STATUS_CCVBV0_DEFAULT << 8)   /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCVBV1                        (0x1UL << 9)                          /**< CC1 CCVB Valid */
#define _TIMER_STATUS_CCVBV1_SHIFT                 9                                     /**< Shift value for TIMER_CCVBV1 */
#define _TIMER_STATUS_CCVBV1_MASK                  0x200UL                               /**< Bit mask for TIMER_CCVBV1 */
#define _TIMER_STATUS_CCVBV1_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCVBV1_DEFAULT                (_TIMER_STATUS_CCVBV1_DEFAULT << 9)   /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCVBV2                        (0x1UL << 10)                         /**< CC2 CCVB Valid */
#define _TIMER_STATUS_CCVBV2_SHIFT                 10                                    /**< Shift value for TIMER_CCVBV2 */
#define _TIMER_STATUS_CCVBV2_MASK                  0x400UL                               /**< Bit mask for TIMER_CCVBV2 */
#define _TIMER_STATUS_CCVBV2_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCVBV2_DEFAULT                (_TIMER_STATUS_CCVBV2_DEFAULT << 10)  /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_ICV0                          (0x1UL << 16)                         /**< CC0 Input Capture Valid */
#define _TIMER_STATUS_ICV0_SHIFT                   16                                    /**< Shift value for TIMER_ICV0 */
#define _TIMER_STATUS_ICV0_MASK                    0x10000UL                             /**< Bit mask for TIMER_ICV0 */
#define _TIMER_STATUS_ICV0_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_ICV0_DEFAULT                  (_TIMER_STATUS_ICV0_DEFAULT << 16)    /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_ICV1                          (0x1UL << 17)                         /**< CC1 Input Capture Valid */
#define _TIMER_STATUS_ICV1_SHIFT                   17                                    /**< Shift value for TIMER_ICV1 */
#define _TIMER_STATUS_ICV1_MASK                    0x20000UL                             /**< Bit mask for TIMER_ICV1 */
#define _TIMER_STATUS_ICV1_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_ICV1_DEFAULT                  (_TIMER_STATUS_ICV1_DEFAULT << 17)    /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_ICV2                          (0x1UL << 18)                         /**< CC2 Input Capture Valid */
#define _TIMER_STATUS_ICV2_SHIFT                   18                                    /**< Shift value for TIMER_ICV2 */
#define _TIMER_STATUS_ICV2_MASK                    0x40000UL                             /**< Bit mask for TIMER_ICV2 */
#define _TIMER_STATUS_ICV2_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_ICV2_DEFAULT                  (_TIMER_STATUS_ICV2_DEFAULT << 18)    /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCPOL0                        (0x1UL << 24)                         /**< CC0 Polarity */
#define _TIMER_STATUS_CCPOL0_SHIFT                 24                                    /**< Shift value for TIMER_CCPOL0 */
#define _TIMER_STATUS_CCPOL0_MASK                  0x1000000UL                           /**< Bit mask for TIMER_CCPOL0 */
#define _TIMER_STATUS_CCPOL0_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define _TIMER_STATUS_CCPOL0_LOWRISE               0x00000000UL                          /**< Mode LOWRISE for TIMER_STATUS */
#define _TIMER_STATUS_CCPOL0_HIGHFALL              0x00000001UL                          /**< Mode HIGHFALL for TIMER_STATUS */
#define TIMER_STATUS_CCPOL0_DEFAULT                (_TIMER_STATUS_CCPOL0_DEFAULT << 24)  /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCPOL0_LOWRISE                (_TIMER_STATUS_CCPOL0_LOWRISE << 24)  /**< Shifted mode LOWRISE for TIMER_STATUS */
#define TIMER_STATUS_CCPOL0_HIGHFALL               (_TIMER_STATUS_CCPOL0_HIGHFALL << 24) /**< Shifted mode HIGHFALL for TIMER_STATUS */
#define TIMER_STATUS_CCPOL1                        (0x1UL << 25)                         /**< CC1 Polarity */
#define _TIMER_STATUS_CCPOL1_SHIFT                 25                                    /**< Shift value for TIMER_CCPOL1 */
#define _TIMER_STATUS_CCPOL1_MASK                  0x2000000UL                           /**< Bit mask for TIMER_CCPOL1 */
#define _TIMER_STATUS_CCPOL1_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define _TIMER_STATUS_CCPOL1_LOWRISE               0x00000000UL                          /**< Mode LOWRISE for TIMER_STATUS */
#define _TIMER_STATUS_CCPOL1_HIGHFALL              0x00000001UL                          /**< Mode HIGHFALL for TIMER_STATUS */
#define TIMER_STATUS_CCPOL1_DEFAULT                (_TIMER_STATUS_CCPOL1_DEFAULT << 25)  /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCPOL1_LOWRISE                (_TIMER_STATUS_CCPOL1_LOWRISE << 25)  /**< Shifted mode LOWRISE for TIMER_STATUS */
#define TIMER_STATUS_CCPOL1_HIGHFALL               (_TIMER_STATUS_CCPOL1_HIGHFALL << 25) /**< Shifted mode HIGHFALL for TIMER_STATUS */
#define TIMER_STATUS_CCPOL2                        (0x1UL << 26)                         /**< CC2 Polarity */
#define _TIMER_STATUS_CCPOL2_SHIFT                 26                                    /**< Shift value for TIMER_CCPOL2 */
#define _TIMER_STATUS_CCPOL2_MASK                  0x4000000UL                           /**< Bit mask for TIMER_CCPOL2 */
#define _TIMER_STATUS_CCPOL2_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for TIMER_STATUS */
#define _TIMER_STATUS_CCPOL2_LOWRISE               0x00000000UL                          /**< Mode LOWRISE for TIMER_STATUS */
#define _TIMER_STATUS_CCPOL2_HIGHFALL              0x00000001UL                          /**< Mode HIGHFALL for TIMER_STATUS */
#define TIMER_STATUS_CCPOL2_DEFAULT                (_TIMER_STATUS_CCPOL2_DEFAULT << 26)  /**< Shifted mode DEFAULT for TIMER_STATUS */
#define TIMER_STATUS_CCPOL2_LOWRISE                (_TIMER_STATUS_CCPOL2_LOWRISE << 26)  /**< Shifted mode LOWRISE for TIMER_STATUS */
#define TIMER_STATUS_CCPOL2_HIGHFALL               (_TIMER_STATUS_CCPOL2_HIGHFALL << 26) /**< Shifted mode HIGHFALL for TIMER_STATUS */

/* Bit fields for TIMER IEN */
#define _TIMER_IEN_RESETVALUE                      0x00000000UL                      /**< Default value for TIMER_IEN */
#define _TIMER_IEN_MASK                            0x00000773UL                      /**< Mask for TIMER_IEN */
#define TIMER_IEN_OF                               (0x1UL << 0)                      /**< Overflow Interrupt Enable */
#define _TIMER_IEN_OF_SHIFT                        0                                 /**< Shift value for TIMER_OF */
#define _TIMER_IEN_OF_MASK                         0x1UL                             /**< Bit mask for TIMER_OF */
#define _TIMER_IEN_OF_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_OF_DEFAULT                       (_TIMER_IEN_OF_DEFAULT << 0)      /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_UF                               (0x1UL << 1)                      /**< Underflow Interrupt Enable */
#define _TIMER_IEN_UF_SHIFT                        1                                 /**< Shift value for TIMER_UF */
#define _TIMER_IEN_UF_MASK                         0x2UL                             /**< Bit mask for TIMER_UF */
#define _TIMER_IEN_UF_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_UF_DEFAULT                       (_TIMER_IEN_UF_DEFAULT << 1)      /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_CC0                              (0x1UL << 4)                      /**< CC Channel 0 Interrupt Enable */
#define _TIMER_IEN_CC0_SHIFT                       4                                 /**< Shift value for TIMER_CC0 */
#define _TIMER_IEN_CC0_MASK                        0x10UL                            /**< Bit mask for TIMER_CC0 */
#define _TIMER_IEN_CC0_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_CC0_DEFAULT                      (_TIMER_IEN_CC0_DEFAULT << 4)     /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_CC1                              (0x1UL << 5)                      /**< CC Channel 1 Interrupt Enable */
#define _TIMER_IEN_CC1_SHIFT                       5                                 /**< Shift value for TIMER_CC1 */
#define _TIMER_IEN_CC1_MASK                        0x20UL                            /**< Bit mask for TIMER_CC1 */
#define _TIMER_IEN_CC1_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_CC1_DEFAULT                      (_TIMER_IEN_CC1_DEFAULT << 5)     /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_CC2                              (0x1UL << 6)                      /**< CC Channel 2 Interrupt Enable */
#define _TIMER_IEN_CC2_SHIFT                       6                                 /**< Shift value for TIMER_CC2 */
#define _TIMER_IEN_CC2_MASK                        0x40UL                            /**< Bit mask for TIMER_CC2 */
#define _TIMER_IEN_CC2_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_CC2_DEFAULT                      (_TIMER_IEN_CC2_DEFAULT << 6)     /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_ICBOF0                           (0x1UL << 8)                      /**< CC Channel 0 Input Capture Buffer Overflow Interrupt Enable */
#define _TIMER_IEN_ICBOF0_SHIFT                    8                                 /**< Shift value for TIMER_ICBOF0 */
#define _TIMER_IEN_ICBOF0_MASK                     0x100UL                           /**< Bit mask for TIMER_ICBOF0 */
#define _TIMER_IEN_ICBOF0_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_ICBOF0_DEFAULT                   (_TIMER_IEN_ICBOF0_DEFAULT << 8)  /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_ICBOF1                           (0x1UL << 9)                      /**< CC Channel 1 Input Capture Buffer Overflow Interrupt Enable */
#define _TIMER_IEN_ICBOF1_SHIFT                    9                                 /**< Shift value for TIMER_ICBOF1 */
#define _TIMER_IEN_ICBOF1_MASK                     0x200UL                           /**< Bit mask for TIMER_ICBOF1 */
#define _TIMER_IEN_ICBOF1_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_ICBOF1_DEFAULT                   (_TIMER_IEN_ICBOF1_DEFAULT << 9)  /**< Shifted mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_ICBOF2                           (0x1UL << 10)                     /**< CC Channel 2 Input Capture Buffer Overflow Interrupt Enable */
#define _TIMER_IEN_ICBOF2_SHIFT                    10                                /**< Shift value for TIMER_ICBOF2 */
#define _TIMER_IEN_ICBOF2_MASK                     0x400UL                           /**< Bit mask for TIMER_ICBOF2 */
#define _TIMER_IEN_ICBOF2_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IEN */
#define TIMER_IEN_ICBOF2_DEFAULT                   (_TIMER_IEN_ICBOF2_DEFAULT << 10) /**< Shifted mode DEFAULT for TIMER_IEN */

/* Bit fields for TIMER IF */
#define _TIMER_IF_RESETVALUE                       0x00000000UL                     /**< Default value for TIMER_IF */
#define _TIMER_IF_MASK                             0x00000773UL                     /**< Mask for TIMER_IF */
#define TIMER_IF_OF                                (0x1UL << 0)                     /**< Overflow Interrupt Flag */
#define _TIMER_IF_OF_SHIFT                         0                                /**< Shift value for TIMER_OF */
#define _TIMER_IF_OF_MASK                          0x1UL                            /**< Bit mask for TIMER_OF */
#define _TIMER_IF_OF_DEFAULT                       0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_OF_DEFAULT                        (_TIMER_IF_OF_DEFAULT << 0)      /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_UF                                (0x1UL << 1)                     /**< Underflow Interrupt Flag */
#define _TIMER_IF_UF_SHIFT                         1                                /**< Shift value for TIMER_UF */
#define _TIMER_IF_UF_MASK                          0x2UL                            /**< Bit mask for TIMER_UF */
#define _TIMER_IF_UF_DEFAULT                       0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_UF_DEFAULT                        (_TIMER_IF_UF_DEFAULT << 1)      /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_CC0                               (0x1UL << 4)                     /**< CC Channel 0 Interrupt Flag */
#define _TIMER_IF_CC0_SHIFT                        4                                /**< Shift value for TIMER_CC0 */
#define _TIMER_IF_CC0_MASK                         0x10UL                           /**< Bit mask for TIMER_CC0 */
#define _TIMER_IF_CC0_DEFAULT                      0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_CC0_DEFAULT                       (_TIMER_IF_CC0_DEFAULT << 4)     /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_CC1                               (0x1UL << 5)                     /**< CC Channel 1 Interrupt Flag */
#define _TIMER_IF_CC1_SHIFT                        5                                /**< Shift value for TIMER_CC1 */
#define _TIMER_IF_CC1_MASK                         0x20UL                           /**< Bit mask for TIMER_CC1 */
#define _TIMER_IF_CC1_DEFAULT                      0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_CC1_DEFAULT                       (_TIMER_IF_CC1_DEFAULT << 5)     /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_CC2                               (0x1UL << 6)                     /**< CC Channel 2 Interrupt Flag */
#define _TIMER_IF_CC2_SHIFT                        6                                /**< Shift value for TIMER_CC2 */
#define _TIMER_IF_CC2_MASK                         0x40UL                           /**< Bit mask for TIMER_CC2 */
#define _TIMER_IF_CC2_DEFAULT                      0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_CC2_DEFAULT                       (_TIMER_IF_CC2_DEFAULT << 6)     /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_ICBOF0                            (0x1UL << 8)                     /**< CC Channel 0 Input Capture Buffer Overflow Interrupt Flag */
#define _TIMER_IF_ICBOF0_SHIFT                     8                                /**< Shift value for TIMER_ICBOF0 */
#define _TIMER_IF_ICBOF0_MASK                      0x100UL                          /**< Bit mask for TIMER_ICBOF0 */
#define _TIMER_IF_ICBOF0_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_ICBOF0_DEFAULT                    (_TIMER_IF_ICBOF0_DEFAULT << 8)  /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_ICBOF1                            (0x1UL << 9)                     /**< CC Channel 1 Input Capture Buffer Overflow Interrupt Flag */
#define _TIMER_IF_ICBOF1_SHIFT                     9                                /**< Shift value for TIMER_ICBOF1 */
#define _TIMER_IF_ICBOF1_MASK                      0x200UL                          /**< Bit mask for TIMER_ICBOF1 */
#define _TIMER_IF_ICBOF1_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_ICBOF1_DEFAULT                    (_TIMER_IF_ICBOF1_DEFAULT << 9)  /**< Shifted mode DEFAULT for TIMER_IF */
#define TIMER_IF_ICBOF2                            (0x1UL << 10)                    /**< CC Channel 2 Input Capture Buffer Overflow Interrupt Flag */
#define _TIMER_IF_ICBOF2_SHIFT                     10                               /**< Shift value for TIMER_ICBOF2 */
#define _TIMER_IF_ICBOF2_MASK                      0x400UL                          /**< Bit mask for TIMER_ICBOF2 */
#define _TIMER_IF_ICBOF2_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for TIMER_IF */
#define TIMER_IF_ICBOF2_DEFAULT                    (_TIMER_IF_ICBOF2_DEFAULT << 10) /**< Shifted mode DEFAULT for TIMER_IF */

/* Bit fields for TIMER IFS */
#define _TIMER_IFS_RESETVALUE                      0x00000000UL                      /**< Default value for TIMER_IFS */
#define _TIMER_IFS_MASK                            0x00000773UL                      /**< Mask for TIMER_IFS */
#define TIMER_IFS_OF                               (0x1UL << 0)                      /**< Overflow Interrupt Flag Set */
#define _TIMER_IFS_OF_SHIFT                        0                                 /**< Shift value for TIMER_OF */
#define _TIMER_IFS_OF_MASK                         0x1UL                             /**< Bit mask for TIMER_OF */
#define _TIMER_IFS_OF_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_OF_DEFAULT                       (_TIMER_IFS_OF_DEFAULT << 0)      /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_UF                               (0x1UL << 1)                      /**< Underflow Interrupt Flag Set */
#define _TIMER_IFS_UF_SHIFT                        1                                 /**< Shift value for TIMER_UF */
#define _TIMER_IFS_UF_MASK                         0x2UL                             /**< Bit mask for TIMER_UF */
#define _TIMER_IFS_UF_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_UF_DEFAULT                       (_TIMER_IFS_UF_DEFAULT << 1)      /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_CC0                              (0x1UL << 4)                      /**< CC Channel 0 Interrupt Flag Set */
#define _TIMER_IFS_CC0_SHIFT                       4                                 /**< Shift value for TIMER_CC0 */
#define _TIMER_IFS_CC0_MASK                        0x10UL                            /**< Bit mask for TIMER_CC0 */
#define _TIMER_IFS_CC0_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_CC0_DEFAULT                      (_TIMER_IFS_CC0_DEFAULT << 4)     /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_CC1                              (0x1UL << 5)                      /**< CC Channel 1 Interrupt Flag Set */
#define _TIMER_IFS_CC1_SHIFT                       5                                 /**< Shift value for TIMER_CC1 */
#define _TIMER_IFS_CC1_MASK                        0x20UL                            /**< Bit mask for TIMER_CC1 */
#define _TIMER_IFS_CC1_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_CC1_DEFAULT                      (_TIMER_IFS_CC1_DEFAULT << 5)     /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_CC2                              (0x1UL << 6)                      /**< CC Channel 2 Interrupt Flag Set */
#define _TIMER_IFS_CC2_SHIFT                       6                                 /**< Shift value for TIMER_CC2 */
#define _TIMER_IFS_CC2_MASK                        0x40UL                            /**< Bit mask for TIMER_CC2 */
#define _TIMER_IFS_CC2_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_CC2_DEFAULT                      (_TIMER_IFS_CC2_DEFAULT << 6)     /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_ICBOF0                           (0x1UL << 8)                      /**< CC Channel 0 Input Capture Buffer Overflow Interrupt Flag Set */
#define _TIMER_IFS_ICBOF0_SHIFT                    8                                 /**< Shift value for TIMER_ICBOF0 */
#define _TIMER_IFS_ICBOF0_MASK                     0x100UL                           /**< Bit mask for TIMER_ICBOF0 */
#define _TIMER_IFS_ICBOF0_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_ICBOF0_DEFAULT                   (_TIMER_IFS_ICBOF0_DEFAULT << 8)  /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_ICBOF1                           (0x1UL << 9)                      /**< CC Channel 1 Input Capture Buffer Overflow Interrupt Flag Set */
#define _TIMER_IFS_ICBOF1_SHIFT                    9                                 /**< Shift value for TIMER_ICBOF1 */
#define _TIMER_IFS_ICBOF1_MASK                     0x200UL                           /**< Bit mask for TIMER_ICBOF1 */
#define _TIMER_IFS_ICBOF1_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_ICBOF1_DEFAULT                   (_TIMER_IFS_ICBOF1_DEFAULT << 9)  /**< Shifted mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_ICBOF2                           (0x1UL << 10)                     /**< CC Channel 2 Input Capture Buffer Overflow Interrupt Flag Set */
#define _TIMER_IFS_ICBOF2_SHIFT                    10                                /**< Shift value for TIMER_ICBOF2 */
#define _TIMER_IFS_ICBOF2_MASK                     0x400UL                           /**< Bit mask for TIMER_ICBOF2 */
#define _TIMER_IFS_ICBOF2_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IFS */
#define TIMER_IFS_ICBOF2_DEFAULT                   (_TIMER_IFS_ICBOF2_DEFAULT << 10) /**< Shifted mode DEFAULT for TIMER_IFS */

/* Bit fields for TIMER IFC */
#define _TIMER_IFC_RESETVALUE                      0x00000000UL                      /**< Default value for TIMER_IFC */
#define _TIMER_IFC_MASK                            0x00000773UL                      /**< Mask for TIMER_IFC */
#define TIMER_IFC_OF                               (0x1UL << 0)                      /**< Overflow Interrupt Flag Clear */
#define _TIMER_IFC_OF_SHIFT                        0                                 /**< Shift value for TIMER_OF */
#define _TIMER_IFC_OF_MASK                         0x1UL                             /**< Bit mask for TIMER_OF */
#define _TIMER_IFC_OF_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_OF_DEFAULT                       (_TIMER_IFC_OF_DEFAULT << 0)      /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_UF                               (0x1UL << 1)                      /**< Underflow Interrupt Flag Clear */
#define _TIMER_IFC_UF_SHIFT                        1                                 /**< Shift value for TIMER_UF */
#define _TIMER_IFC_UF_MASK                         0x2UL                             /**< Bit mask for TIMER_UF */
#define _TIMER_IFC_UF_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_UF_DEFAULT                       (_TIMER_IFC_UF_DEFAULT << 1)      /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_CC0                              (0x1UL << 4)                      /**< CC Channel 0 Interrupt Flag Clear */
#define _TIMER_IFC_CC0_SHIFT                       4                                 /**< Shift value for TIMER_CC0 */
#define _TIMER_IFC_CC0_MASK                        0x10UL                            /**< Bit mask for TIMER_CC0 */
#define _TIMER_IFC_CC0_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_CC0_DEFAULT                      (_TIMER_IFC_CC0_DEFAULT << 4)     /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_CC1                              (0x1UL << 5)                      /**< CC Channel 1 Interrupt Flag Clear */
#define _TIMER_IFC_CC1_SHIFT                       5                                 /**< Shift value for TIMER_CC1 */
#define _TIMER_IFC_CC1_MASK                        0x20UL                            /**< Bit mask for TIMER_CC1 */
#define _TIMER_IFC_CC1_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_CC1_DEFAULT                      (_TIMER_IFC_CC1_DEFAULT << 5)     /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_CC2                              (0x1UL << 6)                      /**< CC Channel 2 Interrupt Flag Clear */
#define _TIMER_IFC_CC2_SHIFT                       6                                 /**< Shift value for TIMER_CC2 */
#define _TIMER_IFC_CC2_MASK                        0x40UL                            /**< Bit mask for TIMER_CC2 */
#define _TIMER_IFC_CC2_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_CC2_DEFAULT                      (_TIMER_IFC_CC2_DEFAULT << 6)     /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_ICBOF0                           (0x1UL << 8)                      /**< CC Channel 0 Input Capture Buffer Overflow Interrupt Flag Clear */
#define _TIMER_IFC_ICBOF0_SHIFT                    8                                 /**< Shift value for TIMER_ICBOF0 */
#define _TIMER_IFC_ICBOF0_MASK                     0x100UL                           /**< Bit mask for TIMER_ICBOF0 */
#define _TIMER_IFC_ICBOF0_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_ICBOF0_DEFAULT                   (_TIMER_IFC_ICBOF0_DEFAULT << 8)  /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_ICBOF1                           (0x1UL << 9)                      /**< CC Channel 1 Input Capture Buffer Overflow Interrupt Flag Clear */
#define _TIMER_IFC_ICBOF1_SHIFT                    9                                 /**< Shift value for TIMER_ICBOF1 */
#define _TIMER_IFC_ICBOF1_MASK                     0x200UL                           /**< Bit mask for TIMER_ICBOF1 */
#define _TIMER_IFC_ICBOF1_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_ICBOF1_DEFAULT                   (_TIMER_IFC_ICBOF1_DEFAULT << 9)  /**< Shifted mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_ICBOF2                           (0x1UL << 10)                     /**< CC Channel 2 Input Capture Buffer Overflow Interrupt Flag Clear */
#define _TIMER_IFC_ICBOF2_SHIFT                    10                                /**< Shift value for TIMER_ICBOF2 */
#define _TIMER_IFC_ICBOF2_MASK                     0x400UL                           /**< Bit mask for TIMER_ICBOF2 */
#define _TIMER_IFC_ICBOF2_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for TIMER_IFC */
#define TIMER_IFC_ICBOF2_DEFAULT                   (_TIMER_IFC_ICBOF2_DEFAULT << 10) /**< Shifted mode DEFAULT for TIMER_IFC */

/* Bit fields for TIMER TOP */
#define _TIMER_TOP_RESETVALUE                      0x0000FFFFUL                  /**< Default value for TIMER_TOP */
#define _TIMER_TOP_MASK                            0x0000FFFFUL                  /**< Mask for TIMER_TOP */
#define _TIMER_TOP_TOP_SHIFT                       0                             /**< Shift value for TIMER_TOP */
#define _TIMER_TOP_TOP_MASK                        0xFFFFUL                      /**< Bit mask for TIMER_TOP */
#define _TIMER_TOP_TOP_DEFAULT                     0x0000FFFFUL                  /**< Mode DEFAULT for TIMER_TOP */
#define TIMER_TOP_TOP_DEFAULT                      (_TIMER_TOP_TOP_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_TOP */

/* Bit fields for TIMER TOPB */
#define _TIMER_TOPB_RESETVALUE                     0x00000000UL                    /**< Default value for TIMER_TOPB */
#define _TIMER_TOPB_MASK                           0x0000FFFFUL                    /**< Mask for TIMER_TOPB */
#define _TIMER_TOPB_TOPB_SHIFT                     0                               /**< Shift value for TIMER_TOPB */
#define _TIMER_TOPB_TOPB_MASK                      0xFFFFUL                        /**< Bit mask for TIMER_TOPB */
#define _TIMER_TOPB_TOPB_DEFAULT                   0x00000000UL                    /**< Mode DEFAULT for TIMER_TOPB */
#define TIMER_TOPB_TOPB_DEFAULT                    (_TIMER_TOPB_TOPB_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_TOPB */

/* Bit fields for TIMER CNT */
#define _TIMER_CNT_RESETVALUE                      0x00000000UL                  /**< Default value for TIMER_CNT */
#define _TIMER_CNT_MASK                            0x0000FFFFUL                  /**< Mask for TIMER_CNT */
#define _TIMER_CNT_CNT_SHIFT                       0                             /**< Shift value for TIMER_CNT */
#define _TIMER_CNT_CNT_MASK                        0xFFFFUL                      /**< Bit mask for TIMER_CNT */
#define _TIMER_CNT_CNT_DEFAULT                     0x00000000UL                  /**< Mode DEFAULT for TIMER_CNT */
#define TIMER_CNT_CNT_DEFAULT                      (_TIMER_CNT_CNT_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_CNT */

/* Bit fields for TIMER ROUTE */
#define _TIMER_ROUTE_RESETVALUE                    0x00000000UL                          /**< Default value for TIMER_ROUTE */
#define _TIMER_ROUTE_MASK                          0x00070007UL                          /**< Mask for TIMER_ROUTE */
#define TIMER_ROUTE_CC0PEN                         (0x1UL << 0)                          /**< CC Channel 0 Pin Enable */
#define _TIMER_ROUTE_CC0PEN_SHIFT                  0                                     /**< Shift value for TIMER_CC0PEN */
#define _TIMER_ROUTE_CC0PEN_MASK                   0x1UL                                 /**< Bit mask for TIMER_CC0PEN */
#define _TIMER_ROUTE_CC0PEN_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for TIMER_ROUTE */
#define TIMER_ROUTE_CC0PEN_DEFAULT                 (_TIMER_ROUTE_CC0PEN_DEFAULT << 0)    /**< Shifted mode DEFAULT for TIMER_ROUTE */
#define TIMER_ROUTE_CC1PEN                         (0x1UL << 1)                          /**< CC Channel 1 Pin Enable */
#define _TIMER_ROUTE_CC1PEN_SHIFT                  1                                     /**< Shift value for TIMER_CC1PEN */
#define _TIMER_ROUTE_CC1PEN_MASK                   0x2UL                                 /**< Bit mask for TIMER_CC1PEN */
#define _TIMER_ROUTE_CC1PEN_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for TIMER_ROUTE */
#define TIMER_ROUTE_CC1PEN_DEFAULT                 (_TIMER_ROUTE_CC1PEN_DEFAULT << 1)    /**< Shifted mode DEFAULT for TIMER_ROUTE */
#define TIMER_ROUTE_CC2PEN                         (0x1UL << 2)                          /**< CC Channel 2 Pin Enable */
#define _TIMER_ROUTE_CC2PEN_SHIFT                  2                                     /**< Shift value for TIMER_CC2PEN */
#define _TIMER_ROUTE_CC2PEN_MASK                   0x4UL                                 /**< Bit mask for TIMER_CC2PEN */
#define _TIMER_ROUTE_CC2PEN_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for TIMER_ROUTE */
#define TIMER_ROUTE_CC2PEN_DEFAULT                 (_TIMER_ROUTE_CC2PEN_DEFAULT << 2)    /**< Shifted mode DEFAULT for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_SHIFT                16                                    /**< Shift value for TIMER_LOCATION */
#define _TIMER_ROUTE_LOCATION_MASK                 0x70000UL                             /**< Bit mask for TIMER_LOCATION */
#define _TIMER_ROUTE_LOCATION_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_LOC0                 0x00000000UL                          /**< Mode LOC0 for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_LOC1                 0x00000001UL                          /**< Mode LOC1 for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_LOC2                 0x00000002UL                          /**< Mode LOC2 for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_LOC3                 0x00000003UL                          /**< Mode LOC3 for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_LOC4                 0x00000004UL                          /**< Mode LOC4 for TIMER_ROUTE */
#define _TIMER_ROUTE_LOCATION_LOC5                 0x00000005UL                          /**< Mode LOC5 for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_DEFAULT               (_TIMER_ROUTE_LOCATION_DEFAULT << 16) /**< Shifted mode DEFAULT for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_LOC0                  (_TIMER_ROUTE_LOCATION_LOC0 << 16)    /**< Shifted mode LOC0 for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_LOC1                  (_TIMER_ROUTE_LOCATION_LOC1 << 16)    /**< Shifted mode LOC1 for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_LOC2                  (_TIMER_ROUTE_LOCATION_LOC2 << 16)    /**< Shifted mode LOC2 for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_LOC3                  (_TIMER_ROUTE_LOCATION_LOC3 << 16)    /**< Shifted mode LOC3 for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_LOC4                  (_TIMER_ROUTE_LOCATION_LOC4 << 16)    /**< Shifted mode LOC4 for TIMER_ROUTE */
#define TIMER_ROUTE_LOCATION_LOC5                  (_TIMER_ROUTE_LOCATION_LOC5 << 16)    /**< Shifted mode LOC5 for TIMER_ROUTE */

/* Bit fields for TIMER CC_CTRL */
#define _TIMER_CC_CTRL_RESETVALUE                  0x00000000UL                                    /**< Default value for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_MASK                        0x0F373F17UL                                    /**< Mask for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_MODE_SHIFT                  0                                               /**< Shift value for TIMER_MODE */
#define _TIMER_CC_CTRL_MODE_MASK                   0x3UL                                           /**< Bit mask for TIMER_MODE */
#define _TIMER_CC_CTRL_MODE_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_MODE_OFF                    0x00000000UL                                    /**< Mode OFF for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_MODE_INPUTCAPTURE           0x00000001UL                                    /**< Mode INPUTCAPTURE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE          0x00000002UL                                    /**< Mode OUTPUTCOMPARE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_MODE_PWM                    0x00000003UL                                    /**< Mode PWM for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_MODE_DEFAULT                 (_TIMER_CC_CTRL_MODE_DEFAULT << 0)              /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_MODE_OFF                     (_TIMER_CC_CTRL_MODE_OFF << 0)                  /**< Shifted mode OFF for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_MODE_INPUTCAPTURE            (_TIMER_CC_CTRL_MODE_INPUTCAPTURE << 0)         /**< Shifted mode INPUTCAPTURE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_MODE_OUTPUTCOMPARE           (_TIMER_CC_CTRL_MODE_OUTPUTCOMPARE << 0)        /**< Shifted mode OUTPUTCOMPARE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_MODE_PWM                     (_TIMER_CC_CTRL_MODE_PWM << 0)                  /**< Shifted mode PWM for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_OUTINV                       (0x1UL << 2)                                    /**< Output Invert */
#define _TIMER_CC_CTRL_OUTINV_SHIFT                2                                               /**< Shift value for TIMER_OUTINV */
#define _TIMER_CC_CTRL_OUTINV_MASK                 0x4UL                                           /**< Bit mask for TIMER_OUTINV */
#define _TIMER_CC_CTRL_OUTINV_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_OUTINV_DEFAULT               (_TIMER_CC_CTRL_OUTINV_DEFAULT << 2)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COIST                        (0x1UL << 4)                                    /**< Compare Output Initial State */
#define _TIMER_CC_CTRL_COIST_SHIFT                 4                                               /**< Shift value for TIMER_COIST */
#define _TIMER_CC_CTRL_COIST_MASK                  0x10UL                                          /**< Bit mask for TIMER_COIST */
#define _TIMER_CC_CTRL_COIST_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COIST_DEFAULT                (_TIMER_CC_CTRL_COIST_DEFAULT << 4)             /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CMOA_SHIFT                  8                                               /**< Shift value for TIMER_CMOA */
#define _TIMER_CC_CTRL_CMOA_MASK                   0x300UL                                         /**< Bit mask for TIMER_CMOA */
#define _TIMER_CC_CTRL_CMOA_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CMOA_NONE                   0x00000000UL                                    /**< Mode NONE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CMOA_TOGGLE                 0x00000001UL                                    /**< Mode TOGGLE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CMOA_CLEAR                  0x00000002UL                                    /**< Mode CLEAR for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CMOA_SET                    0x00000003UL                                    /**< Mode SET for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CMOA_DEFAULT                 (_TIMER_CC_CTRL_CMOA_DEFAULT << 8)              /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CMOA_NONE                    (_TIMER_CC_CTRL_CMOA_NONE << 8)                 /**< Shifted mode NONE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CMOA_TOGGLE                  (_TIMER_CC_CTRL_CMOA_TOGGLE << 8)               /**< Shifted mode TOGGLE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CMOA_CLEAR                   (_TIMER_CC_CTRL_CMOA_CLEAR << 8)                /**< Shifted mode CLEAR for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CMOA_SET                     (_TIMER_CC_CTRL_CMOA_SET << 8)                  /**< Shifted mode SET for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_COFOA_SHIFT                 10                                              /**< Shift value for TIMER_COFOA */
#define _TIMER_CC_CTRL_COFOA_MASK                  0xC00UL                                         /**< Bit mask for TIMER_COFOA */
#define _TIMER_CC_CTRL_COFOA_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_COFOA_NONE                  0x00000000UL                                    /**< Mode NONE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_COFOA_TOGGLE                0x00000001UL                                    /**< Mode TOGGLE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_COFOA_CLEAR                 0x00000002UL                                    /**< Mode CLEAR for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_COFOA_SET                   0x00000003UL                                    /**< Mode SET for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COFOA_DEFAULT                (_TIMER_CC_CTRL_COFOA_DEFAULT << 10)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COFOA_NONE                   (_TIMER_CC_CTRL_COFOA_NONE << 10)               /**< Shifted mode NONE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COFOA_TOGGLE                 (_TIMER_CC_CTRL_COFOA_TOGGLE << 10)             /**< Shifted mode TOGGLE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COFOA_CLEAR                  (_TIMER_CC_CTRL_COFOA_CLEAR << 10)              /**< Shifted mode CLEAR for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_COFOA_SET                    (_TIMER_CC_CTRL_COFOA_SET << 10)                /**< Shifted mode SET for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CUFOA_SHIFT                 12                                              /**< Shift value for TIMER_CUFOA */
#define _TIMER_CC_CTRL_CUFOA_MASK                  0x3000UL                                        /**< Bit mask for TIMER_CUFOA */
#define _TIMER_CC_CTRL_CUFOA_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CUFOA_NONE                  0x00000000UL                                    /**< Mode NONE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CUFOA_TOGGLE                0x00000001UL                                    /**< Mode TOGGLE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CUFOA_CLEAR                 0x00000002UL                                    /**< Mode CLEAR for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_CUFOA_SET                   0x00000003UL                                    /**< Mode SET for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CUFOA_DEFAULT                (_TIMER_CC_CTRL_CUFOA_DEFAULT << 12)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CUFOA_NONE                   (_TIMER_CC_CTRL_CUFOA_NONE << 12)               /**< Shifted mode NONE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CUFOA_TOGGLE                 (_TIMER_CC_CTRL_CUFOA_TOGGLE << 12)             /**< Shifted mode TOGGLE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CUFOA_CLEAR                  (_TIMER_CC_CTRL_CUFOA_CLEAR << 12)              /**< Shifted mode CLEAR for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_CUFOA_SET                    (_TIMER_CC_CTRL_CUFOA_SET << 12)                /**< Shifted mode SET for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_SHIFT                16                                              /**< Shift value for TIMER_PRSSEL */
#define _TIMER_CC_CTRL_PRSSEL_MASK                 0x70000UL                                       /**< Bit mask for TIMER_PRSSEL */
#define _TIMER_CC_CTRL_PRSSEL_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH0               0x00000000UL                                    /**< Mode PRSCH0 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH1               0x00000001UL                                    /**< Mode PRSCH1 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH2               0x00000002UL                                    /**< Mode PRSCH2 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH3               0x00000003UL                                    /**< Mode PRSCH3 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH4               0x00000004UL                                    /**< Mode PRSCH4 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH5               0x00000005UL                                    /**< Mode PRSCH5 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH6               0x00000006UL                                    /**< Mode PRSCH6 for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_PRSSEL_PRSCH7               0x00000007UL                                    /**< Mode PRSCH7 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_DEFAULT               (_TIMER_CC_CTRL_PRSSEL_DEFAULT << 16)           /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH0                (_TIMER_CC_CTRL_PRSSEL_PRSCH0 << 16)            /**< Shifted mode PRSCH0 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH1                (_TIMER_CC_CTRL_PRSSEL_PRSCH1 << 16)            /**< Shifted mode PRSCH1 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH2                (_TIMER_CC_CTRL_PRSSEL_PRSCH2 << 16)            /**< Shifted mode PRSCH2 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH3                (_TIMER_CC_CTRL_PRSSEL_PRSCH3 << 16)            /**< Shifted mode PRSCH3 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH4                (_TIMER_CC_CTRL_PRSSEL_PRSCH4 << 16)            /**< Shifted mode PRSCH4 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH5                (_TIMER_CC_CTRL_PRSSEL_PRSCH5 << 16)            /**< Shifted mode PRSCH5 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH6                (_TIMER_CC_CTRL_PRSSEL_PRSCH6 << 16)            /**< Shifted mode PRSCH6 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_PRSSEL_PRSCH7                (_TIMER_CC_CTRL_PRSSEL_PRSCH7 << 16)            /**< Shifted mode PRSCH7 for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_INSEL                        (0x1UL << 20)                                   /**< Input Selection */
#define _TIMER_CC_CTRL_INSEL_SHIFT                 20                                              /**< Shift value for TIMER_INSEL */
#define _TIMER_CC_CTRL_INSEL_MASK                  0x100000UL                                      /**< Bit mask for TIMER_INSEL */
#define _TIMER_CC_CTRL_INSEL_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_INSEL_PIN                   0x00000000UL                                    /**< Mode PIN for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_INSEL_PRS                   0x00000001UL                                    /**< Mode PRS for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_INSEL_DEFAULT                (_TIMER_CC_CTRL_INSEL_DEFAULT << 20)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_INSEL_PIN                    (_TIMER_CC_CTRL_INSEL_PIN << 20)                /**< Shifted mode PIN for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_INSEL_PRS                    (_TIMER_CC_CTRL_INSEL_PRS << 20)                /**< Shifted mode PRS for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_FILT                         (0x1UL << 21)                                   /**< Digital Filter */
#define _TIMER_CC_CTRL_FILT_SHIFT                  21                                              /**< Shift value for TIMER_FILT */
#define _TIMER_CC_CTRL_FILT_MASK                   0x200000UL                                      /**< Bit mask for TIMER_FILT */
#define _TIMER_CC_CTRL_FILT_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_FILT_DISABLE                0x00000000UL                                    /**< Mode DISABLE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_FILT_ENABLE                 0x00000001UL                                    /**< Mode ENABLE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_FILT_DEFAULT                 (_TIMER_CC_CTRL_FILT_DEFAULT << 21)             /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_FILT_DISABLE                 (_TIMER_CC_CTRL_FILT_DISABLE << 21)             /**< Shifted mode DISABLE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_FILT_ENABLE                  (_TIMER_CC_CTRL_FILT_ENABLE << 21)              /**< Shifted mode ENABLE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEDGE_SHIFT                24                                              /**< Shift value for TIMER_ICEDGE */
#define _TIMER_CC_CTRL_ICEDGE_MASK                 0x3000000UL                                     /**< Bit mask for TIMER_ICEDGE */
#define _TIMER_CC_CTRL_ICEDGE_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEDGE_RISING               0x00000000UL                                    /**< Mode RISING for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEDGE_FALLING              0x00000001UL                                    /**< Mode FALLING for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEDGE_BOTH                 0x00000002UL                                    /**< Mode BOTH for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEDGE_NONE                 0x00000003UL                                    /**< Mode NONE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEDGE_DEFAULT               (_TIMER_CC_CTRL_ICEDGE_DEFAULT << 24)           /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEDGE_RISING                (_TIMER_CC_CTRL_ICEDGE_RISING << 24)            /**< Shifted mode RISING for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEDGE_FALLING               (_TIMER_CC_CTRL_ICEDGE_FALLING << 24)           /**< Shifted mode FALLING for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEDGE_BOTH                  (_TIMER_CC_CTRL_ICEDGE_BOTH << 24)              /**< Shifted mode BOTH for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEDGE_NONE                  (_TIMER_CC_CTRL_ICEDGE_NONE << 24)              /**< Shifted mode NONE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_SHIFT              26                                              /**< Shift value for TIMER_ICEVCTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_MASK               0xC000000UL                                     /**< Bit mask for TIMER_ICEVCTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE          0x00000000UL                                    /**< Mode EVERYEDGE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE    0x00000001UL                                    /**< Mode EVERYSECONDEDGE for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_RISING             0x00000002UL                                    /**< Mode RISING for TIMER_CC_CTRL */
#define _TIMER_CC_CTRL_ICEVCTRL_FALLING            0x00000003UL                                    /**< Mode FALLING for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEVCTRL_DEFAULT             (_TIMER_CC_CTRL_ICEVCTRL_DEFAULT << 26)         /**< Shifted mode DEFAULT for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE           (_TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE << 26)       /**< Shifted mode EVERYEDGE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE     (_TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE << 26) /**< Shifted mode EVERYSECONDEDGE for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEVCTRL_RISING              (_TIMER_CC_CTRL_ICEVCTRL_RISING << 26)          /**< Shifted mode RISING for TIMER_CC_CTRL */
#define TIMER_CC_CTRL_ICEVCTRL_FALLING             (_TIMER_CC_CTRL_ICEVCTRL_FALLING << 26)         /**< Shifted mode FALLING for TIMER_CC_CTRL */

/* Bit fields for TIMER CC_CCV */
#define _TIMER_CC_CCV_RESETVALUE                   0x00000000UL                     /**< Default value for TIMER_CC_CCV */
#define _TIMER_CC_CCV_MASK                         0x0000FFFFUL                     /**< Mask for TIMER_CC_CCV */
#define _TIMER_CC_CCV_CCV_SHIFT                    0                                /**< Shift value for TIMER_CCV */
#define _TIMER_CC_CCV_CCV_MASK                     0xFFFFUL                         /**< Bit mask for TIMER_CCV */
#define _TIMER_CC_CCV_CCV_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for TIMER_CC_CCV */
#define TIMER_CC_CCV_CCV_DEFAULT                   (_TIMER_CC_CCV_CCV_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_CC_CCV */

/* Bit fields for TIMER CC_CCVP */
#define _TIMER_CC_CCVP_RESETVALUE                  0x00000000UL                       /**< Default value for TIMER_CC_CCVP */
#define _TIMER_CC_CCVP_MASK                        0x0000FFFFUL                       /**< Mask for TIMER_CC_CCVP */
#define _TIMER_CC_CCVP_CCVP_SHIFT                  0                                  /**< Shift value for TIMER_CCVP */
#define _TIMER_CC_CCVP_CCVP_MASK                   0xFFFFUL                           /**< Bit mask for TIMER_CCVP */
#define _TIMER_CC_CCVP_CCVP_DEFAULT                0x00000000UL                       /**< Mode DEFAULT for TIMER_CC_CCVP */
#define TIMER_CC_CCVP_CCVP_DEFAULT                 (_TIMER_CC_CCVP_CCVP_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_CC_CCVP */

/* Bit fields for TIMER CC_CCVB */
#define _TIMER_CC_CCVB_RESETVALUE                  0x00000000UL                       /**< Default value for TIMER_CC_CCVB */
#define _TIMER_CC_CCVB_MASK                        0x0000FFFFUL                       /**< Mask for TIMER_CC_CCVB */
#define _TIMER_CC_CCVB_CCVB_SHIFT                  0                                  /**< Shift value for TIMER_CCVB */
#define _TIMER_CC_CCVB_CCVB_MASK                   0xFFFFUL                           /**< Bit mask for TIMER_CCVB */
#define _TIMER_CC_CCVB_CCVB_DEFAULT                0x00000000UL                       /**< Mode DEFAULT for TIMER_CC_CCVB */
#define TIMER_CC_CCVB_CCVB_DEFAULT                 (_TIMER_CC_CCVB_CCVB_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_CC_CCVB */

/** @} End of group EFM32TG_TIMER */


