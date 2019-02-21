/***************************************************************************//**
 * @file
 * @brief EFR32MG1P_MSC register and bit field definitions
 * @version 5.7.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @defgroup EFR32MG1P_MSC MSC
 * @{
 * @brief EFR32MG1P_MSC Register Declaration
 ******************************************************************************/
/** MSC Register Declaration */
typedef struct {
  __IOM uint32_t CTRL;          /**< Memory System Control Register  */
  __IOM uint32_t READCTRL;      /**< Read Control Register  */
  __IOM uint32_t WRITECTRL;     /**< Write Control Register  */
  __IOM uint32_t WRITECMD;      /**< Write Command Register  */
  __IOM uint32_t ADDRB;         /**< Page Erase/Write Address Buffer  */
  uint32_t       RESERVED0[1U]; /**< Reserved for future use **/
  __IOM uint32_t WDATA;         /**< Write Data Register  */
  __IM uint32_t  STATUS;        /**< Status Register  */

  uint32_t       RESERVED1[4U]; /**< Reserved for future use **/
  __IM uint32_t  IF;            /**< Interrupt Flag Register  */
  __IOM uint32_t IFS;           /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC;           /**< Interrupt Flag Clear Register  */
  __IOM uint32_t IEN;           /**< Interrupt Enable Register  */
  __IOM uint32_t LOCK;          /**< Configuration Lock Register  */
  __IOM uint32_t CACHECMD;      /**< Flash Cache Command Register  */
  __IM uint32_t  CACHEHITS;     /**< Cache Hits Performance Counter  */
  __IM uint32_t  CACHEMISSES;   /**< Cache Misses Performance Counter  */

  uint32_t       RESERVED2[1U]; /**< Reserved for future use **/
  __IOM uint32_t MASSLOCK;      /**< Mass Erase Lock Register  */

  uint32_t       RESERVED3[1U]; /**< Reserved for future use **/
  __IOM uint32_t STARTUP;       /**< Startup Control  */

  uint32_t       RESERVED4[5U]; /**< Reserved for future use **/
  __IOM uint32_t CMD;           /**< Command Register  */
} MSC_TypeDef;                  /** @} */

/***************************************************************************//**
 * @addtogroup EFR32MG1P_MSC
 * @{
 * @defgroup EFR32MG1P_MSC_BitFields  MSC Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for MSC CTRL */
#define _MSC_CTRL_RESETVALUE                    0x00000001UL                           /**< Default value for MSC_CTRL */
#define _MSC_CTRL_MASK                          0x0000000FUL                           /**< Mask for MSC_CTRL */
#define MSC_CTRL_ADDRFAULTEN                    (0x1UL << 0)                           /**< Invalid Address Bus Fault Response Enable */
#define _MSC_CTRL_ADDRFAULTEN_SHIFT             0                                      /**< Shift value for MSC_ADDRFAULTEN */
#define _MSC_CTRL_ADDRFAULTEN_MASK              0x1UL                                  /**< Bit mask for MSC_ADDRFAULTEN */
#define _MSC_CTRL_ADDRFAULTEN_DEFAULT           0x00000001UL                           /**< Mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_ADDRFAULTEN_DEFAULT            (_MSC_CTRL_ADDRFAULTEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_CLKDISFAULTEN                  (0x1UL << 1)                           /**< Clock-disabled Bus Fault Response Enable */
#define _MSC_CTRL_CLKDISFAULTEN_SHIFT           1                                      /**< Shift value for MSC_CLKDISFAULTEN */
#define _MSC_CTRL_CLKDISFAULTEN_MASK            0x2UL                                  /**< Bit mask for MSC_CLKDISFAULTEN */
#define _MSC_CTRL_CLKDISFAULTEN_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_CLKDISFAULTEN_DEFAULT          (_MSC_CTRL_CLKDISFAULTEN_DEFAULT << 1) /**< Shifted mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_PWRUPONDEMAND                  (0x1UL << 2)                           /**< Power Up on Demand During Wake Up */
#define _MSC_CTRL_PWRUPONDEMAND_SHIFT           2                                      /**< Shift value for MSC_PWRUPONDEMAND */
#define _MSC_CTRL_PWRUPONDEMAND_MASK            0x4UL                                  /**< Bit mask for MSC_PWRUPONDEMAND */
#define _MSC_CTRL_PWRUPONDEMAND_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_PWRUPONDEMAND_DEFAULT          (_MSC_CTRL_PWRUPONDEMAND_DEFAULT << 2) /**< Shifted mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_IFCREADCLEAR                   (0x1UL << 3)                           /**< IFC Read Clears IF */
#define _MSC_CTRL_IFCREADCLEAR_SHIFT            3                                      /**< Shift value for MSC_IFCREADCLEAR */
#define _MSC_CTRL_IFCREADCLEAR_MASK             0x8UL                                  /**< Bit mask for MSC_IFCREADCLEAR */
#define _MSC_CTRL_IFCREADCLEAR_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for MSC_CTRL */
#define MSC_CTRL_IFCREADCLEAR_DEFAULT           (_MSC_CTRL_IFCREADCLEAR_DEFAULT << 3)  /**< Shifted mode DEFAULT for MSC_CTRL */

/* Bit fields for MSC READCTRL */
#define _MSC_READCTRL_RESETVALUE                0x01000100UL                          /**< Default value for MSC_READCTRL */
#define _MSC_READCTRL_MASK                      0x13000338UL                          /**< Mask for MSC_READCTRL */
#define MSC_READCTRL_IFCDIS                     (0x1UL << 3)                          /**< Internal Flash Cache Disable */
#define _MSC_READCTRL_IFCDIS_SHIFT              3                                     /**< Shift value for MSC_IFCDIS */
#define _MSC_READCTRL_IFCDIS_MASK               0x8UL                                 /**< Bit mask for MSC_IFCDIS */
#define _MSC_READCTRL_IFCDIS_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_IFCDIS_DEFAULT             (_MSC_READCTRL_IFCDIS_DEFAULT << 3)   /**< Shifted mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_AIDIS                      (0x1UL << 4)                          /**< Automatic Invalidate Disable */
#define _MSC_READCTRL_AIDIS_SHIFT               4                                     /**< Shift value for MSC_AIDIS */
#define _MSC_READCTRL_AIDIS_MASK                0x10UL                                /**< Bit mask for MSC_AIDIS */
#define _MSC_READCTRL_AIDIS_DEFAULT             0x00000000UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_AIDIS_DEFAULT              (_MSC_READCTRL_AIDIS_DEFAULT << 4)    /**< Shifted mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_ICCDIS                     (0x1UL << 5)                          /**< Interrupt Context Cache Disable */
#define _MSC_READCTRL_ICCDIS_SHIFT              5                                     /**< Shift value for MSC_ICCDIS */
#define _MSC_READCTRL_ICCDIS_MASK               0x20UL                                /**< Bit mask for MSC_ICCDIS */
#define _MSC_READCTRL_ICCDIS_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_ICCDIS_DEFAULT             (_MSC_READCTRL_ICCDIS_DEFAULT << 5)   /**< Shifted mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_PREFETCH                   (0x1UL << 8)                          /**< Prefetch Mode */
#define _MSC_READCTRL_PREFETCH_SHIFT            8                                     /**< Shift value for MSC_PREFETCH */
#define _MSC_READCTRL_PREFETCH_MASK             0x100UL                               /**< Bit mask for MSC_PREFETCH */
#define _MSC_READCTRL_PREFETCH_DEFAULT          0x00000001UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_PREFETCH_DEFAULT           (_MSC_READCTRL_PREFETCH_DEFAULT << 8) /**< Shifted mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_USEHPROT                   (0x1UL << 9)                          /**< AHB_HPROT Mode */
#define _MSC_READCTRL_USEHPROT_SHIFT            9                                     /**< Shift value for MSC_USEHPROT */
#define _MSC_READCTRL_USEHPROT_MASK             0x200UL                               /**< Bit mask for MSC_USEHPROT */
#define _MSC_READCTRL_USEHPROT_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_USEHPROT_DEFAULT           (_MSC_READCTRL_USEHPROT_DEFAULT << 9) /**< Shifted mode DEFAULT for MSC_READCTRL */
#define _MSC_READCTRL_MODE_SHIFT                24                                    /**< Shift value for MSC_MODE */
#define _MSC_READCTRL_MODE_MASK                 0x3000000UL                           /**< Bit mask for MSC_MODE */
#define _MSC_READCTRL_MODE_WS0                  0x00000000UL                          /**< Mode WS0 for MSC_READCTRL */
#define _MSC_READCTRL_MODE_DEFAULT              0x00000001UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define _MSC_READCTRL_MODE_WS1                  0x00000001UL                          /**< Mode WS1 for MSC_READCTRL */
#define MSC_READCTRL_MODE_WS0                   (_MSC_READCTRL_MODE_WS0 << 24)        /**< Shifted mode WS0 for MSC_READCTRL */
#define MSC_READCTRL_MODE_DEFAULT               (_MSC_READCTRL_MODE_DEFAULT << 24)    /**< Shifted mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_MODE_WS1                   (_MSC_READCTRL_MODE_WS1 << 24)        /**< Shifted mode WS1 for MSC_READCTRL */
#define MSC_READCTRL_SCBTP                      (0x1UL << 28)                         /**< Suppress Conditional Branch Target Perfetch */
#define _MSC_READCTRL_SCBTP_SHIFT               28                                    /**< Shift value for MSC_SCBTP */
#define _MSC_READCTRL_SCBTP_MASK                0x10000000UL                          /**< Bit mask for MSC_SCBTP */
#define _MSC_READCTRL_SCBTP_DEFAULT             0x00000000UL                          /**< Mode DEFAULT for MSC_READCTRL */
#define MSC_READCTRL_SCBTP_DEFAULT              (_MSC_READCTRL_SCBTP_DEFAULT << 28)   /**< Shifted mode DEFAULT for MSC_READCTRL */

/* Bit fields for MSC WRITECTRL */
#define _MSC_WRITECTRL_RESETVALUE               0x00000000UL                                /**< Default value for MSC_WRITECTRL */
#define _MSC_WRITECTRL_MASK                     0x00000003UL                                /**< Mask for MSC_WRITECTRL */
#define MSC_WRITECTRL_WREN                      (0x1UL << 0)                                /**< Enable Write/Erase Controller */
#define _MSC_WRITECTRL_WREN_SHIFT               0                                           /**< Shift value for MSC_WREN */
#define _MSC_WRITECTRL_WREN_MASK                0x1UL                                       /**< Bit mask for MSC_WREN */
#define _MSC_WRITECTRL_WREN_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MSC_WRITECTRL */
#define MSC_WRITECTRL_WREN_DEFAULT              (_MSC_WRITECTRL_WREN_DEFAULT << 0)          /**< Shifted mode DEFAULT for MSC_WRITECTRL */
#define MSC_WRITECTRL_IRQERASEABORT             (0x1UL << 1)                                /**< Abort Page Erase on Interrupt */
#define _MSC_WRITECTRL_IRQERASEABORT_SHIFT      1                                           /**< Shift value for MSC_IRQERASEABORT */
#define _MSC_WRITECTRL_IRQERASEABORT_MASK       0x2UL                                       /**< Bit mask for MSC_IRQERASEABORT */
#define _MSC_WRITECTRL_IRQERASEABORT_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for MSC_WRITECTRL */
#define MSC_WRITECTRL_IRQERASEABORT_DEFAULT     (_MSC_WRITECTRL_IRQERASEABORT_DEFAULT << 1) /**< Shifted mode DEFAULT for MSC_WRITECTRL */

/* Bit fields for MSC WRITECMD */
#define _MSC_WRITECMD_RESETVALUE                0x00000000UL                             /**< Default value for MSC_WRITECMD */
#define _MSC_WRITECMD_MASK                      0x0000113FUL                             /**< Mask for MSC_WRITECMD */
#define MSC_WRITECMD_LADDRIM                    (0x1UL << 0)                             /**< Load MSC_ADDRB Into ADDR */
#define _MSC_WRITECMD_LADDRIM_SHIFT             0                                        /**< Shift value for MSC_LADDRIM */
#define _MSC_WRITECMD_LADDRIM_MASK              0x1UL                                    /**< Bit mask for MSC_LADDRIM */
#define _MSC_WRITECMD_LADDRIM_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_LADDRIM_DEFAULT            (_MSC_WRITECMD_LADDRIM_DEFAULT << 0)     /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_ERASEPAGE                  (0x1UL << 1)                             /**< Erase Page */
#define _MSC_WRITECMD_ERASEPAGE_SHIFT           1                                        /**< Shift value for MSC_ERASEPAGE */
#define _MSC_WRITECMD_ERASEPAGE_MASK            0x2UL                                    /**< Bit mask for MSC_ERASEPAGE */
#define _MSC_WRITECMD_ERASEPAGE_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_ERASEPAGE_DEFAULT          (_MSC_WRITECMD_ERASEPAGE_DEFAULT << 1)   /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_WRITEEND                   (0x1UL << 2)                             /**< End Write Mode */
#define _MSC_WRITECMD_WRITEEND_SHIFT            2                                        /**< Shift value for MSC_WRITEEND */
#define _MSC_WRITECMD_WRITEEND_MASK             0x4UL                                    /**< Bit mask for MSC_WRITEEND */
#define _MSC_WRITECMD_WRITEEND_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_WRITEEND_DEFAULT           (_MSC_WRITECMD_WRITEEND_DEFAULT << 2)    /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_WRITEONCE                  (0x1UL << 3)                             /**< Word Write-Once Trigger */
#define _MSC_WRITECMD_WRITEONCE_SHIFT           3                                        /**< Shift value for MSC_WRITEONCE */
#define _MSC_WRITECMD_WRITEONCE_MASK            0x8UL                                    /**< Bit mask for MSC_WRITEONCE */
#define _MSC_WRITECMD_WRITEONCE_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_WRITEONCE_DEFAULT          (_MSC_WRITECMD_WRITEONCE_DEFAULT << 3)   /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_WRITETRIG                  (0x1UL << 4)                             /**< Word Write Sequence Trigger */
#define _MSC_WRITECMD_WRITETRIG_SHIFT           4                                        /**< Shift value for MSC_WRITETRIG */
#define _MSC_WRITECMD_WRITETRIG_MASK            0x10UL                                   /**< Bit mask for MSC_WRITETRIG */
#define _MSC_WRITECMD_WRITETRIG_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_WRITETRIG_DEFAULT          (_MSC_WRITECMD_WRITETRIG_DEFAULT << 4)   /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_ERASEABORT                 (0x1UL << 5)                             /**< Abort Erase Sequence */
#define _MSC_WRITECMD_ERASEABORT_SHIFT          5                                        /**< Shift value for MSC_ERASEABORT */
#define _MSC_WRITECMD_ERASEABORT_MASK           0x20UL                                   /**< Bit mask for MSC_ERASEABORT */
#define _MSC_WRITECMD_ERASEABORT_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_ERASEABORT_DEFAULT         (_MSC_WRITECMD_ERASEABORT_DEFAULT << 5)  /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_ERASEMAIN0                 (0x1UL << 8)                             /**< Mass Erase Region 0 */
#define _MSC_WRITECMD_ERASEMAIN0_SHIFT          8                                        /**< Shift value for MSC_ERASEMAIN0 */
#define _MSC_WRITECMD_ERASEMAIN0_MASK           0x100UL                                  /**< Bit mask for MSC_ERASEMAIN0 */
#define _MSC_WRITECMD_ERASEMAIN0_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_ERASEMAIN0_DEFAULT         (_MSC_WRITECMD_ERASEMAIN0_DEFAULT << 8)  /**< Shifted mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_CLEARWDATA                 (0x1UL << 12)                            /**< Clear WDATA State */
#define _MSC_WRITECMD_CLEARWDATA_SHIFT          12                                       /**< Shift value for MSC_CLEARWDATA */
#define _MSC_WRITECMD_CLEARWDATA_MASK           0x1000UL                                 /**< Bit mask for MSC_CLEARWDATA */
#define _MSC_WRITECMD_CLEARWDATA_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for MSC_WRITECMD */
#define MSC_WRITECMD_CLEARWDATA_DEFAULT         (_MSC_WRITECMD_CLEARWDATA_DEFAULT << 12) /**< Shifted mode DEFAULT for MSC_WRITECMD */

/* Bit fields for MSC ADDRB */
#define _MSC_ADDRB_RESETVALUE                   0x00000000UL                    /**< Default value for MSC_ADDRB */
#define _MSC_ADDRB_MASK                         0xFFFFFFFFUL                    /**< Mask for MSC_ADDRB */
#define _MSC_ADDRB_ADDRB_SHIFT                  0                               /**< Shift value for MSC_ADDRB */
#define _MSC_ADDRB_ADDRB_MASK                   0xFFFFFFFFUL                    /**< Bit mask for MSC_ADDRB */
#define _MSC_ADDRB_ADDRB_DEFAULT                0x00000000UL                    /**< Mode DEFAULT for MSC_ADDRB */
#define MSC_ADDRB_ADDRB_DEFAULT                 (_MSC_ADDRB_ADDRB_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_ADDRB */

/* Bit fields for MSC WDATA */
#define _MSC_WDATA_RESETVALUE                   0x00000000UL                    /**< Default value for MSC_WDATA */
#define _MSC_WDATA_MASK                         0xFFFFFFFFUL                    /**< Mask for MSC_WDATA */
#define _MSC_WDATA_WDATA_SHIFT                  0                               /**< Shift value for MSC_WDATA */
#define _MSC_WDATA_WDATA_MASK                   0xFFFFFFFFUL                    /**< Bit mask for MSC_WDATA */
#define _MSC_WDATA_WDATA_DEFAULT                0x00000000UL                    /**< Mode DEFAULT for MSC_WDATA */
#define MSC_WDATA_WDATA_DEFAULT                 (_MSC_WDATA_WDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_WDATA */

/* Bit fields for MSC STATUS */
#define _MSC_STATUS_RESETVALUE                  0x00000008UL                            /**< Default value for MSC_STATUS */
#define _MSC_STATUS_MASK                        0x0000007FUL                            /**< Mask for MSC_STATUS */
#define MSC_STATUS_BUSY                         (0x1UL << 0)                            /**< Erase/Write Busy */
#define _MSC_STATUS_BUSY_SHIFT                  0                                       /**< Shift value for MSC_BUSY */
#define _MSC_STATUS_BUSY_MASK                   0x1UL                                   /**< Bit mask for MSC_BUSY */
#define _MSC_STATUS_BUSY_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_BUSY_DEFAULT                 (_MSC_STATUS_BUSY_DEFAULT << 0)         /**< Shifted mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_LOCKED                       (0x1UL << 1)                            /**< Access Locked */
#define _MSC_STATUS_LOCKED_SHIFT                1                                       /**< Shift value for MSC_LOCKED */
#define _MSC_STATUS_LOCKED_MASK                 0x2UL                                   /**< Bit mask for MSC_LOCKED */
#define _MSC_STATUS_LOCKED_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_LOCKED_DEFAULT               (_MSC_STATUS_LOCKED_DEFAULT << 1)       /**< Shifted mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_INVADDR                      (0x1UL << 2)                            /**< Invalid Write Address or Erase Page */
#define _MSC_STATUS_INVADDR_SHIFT               2                                       /**< Shift value for MSC_INVADDR */
#define _MSC_STATUS_INVADDR_MASK                0x4UL                                   /**< Bit mask for MSC_INVADDR */
#define _MSC_STATUS_INVADDR_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_INVADDR_DEFAULT              (_MSC_STATUS_INVADDR_DEFAULT << 2)      /**< Shifted mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_WDATAREADY                   (0x1UL << 3)                            /**< WDATA Write Ready */
#define _MSC_STATUS_WDATAREADY_SHIFT            3                                       /**< Shift value for MSC_WDATAREADY */
#define _MSC_STATUS_WDATAREADY_MASK             0x8UL                                   /**< Bit mask for MSC_WDATAREADY */
#define _MSC_STATUS_WDATAREADY_DEFAULT          0x00000001UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_WDATAREADY_DEFAULT           (_MSC_STATUS_WDATAREADY_DEFAULT << 3)   /**< Shifted mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_WORDTIMEOUT                  (0x1UL << 4)                            /**< Flash Write Word Timeout */
#define _MSC_STATUS_WORDTIMEOUT_SHIFT           4                                       /**< Shift value for MSC_WORDTIMEOUT */
#define _MSC_STATUS_WORDTIMEOUT_MASK            0x10UL                                  /**< Bit mask for MSC_WORDTIMEOUT */
#define _MSC_STATUS_WORDTIMEOUT_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_WORDTIMEOUT_DEFAULT          (_MSC_STATUS_WORDTIMEOUT_DEFAULT << 4)  /**< Shifted mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_ERASEABORTED                 (0x1UL << 5)                            /**< The Current Flash Erase Operation Aborted */
#define _MSC_STATUS_ERASEABORTED_SHIFT          5                                       /**< Shift value for MSC_ERASEABORTED */
#define _MSC_STATUS_ERASEABORTED_MASK           0x20UL                                  /**< Bit mask for MSC_ERASEABORTED */
#define _MSC_STATUS_ERASEABORTED_DEFAULT        0x00000000UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_ERASEABORTED_DEFAULT         (_MSC_STATUS_ERASEABORTED_DEFAULT << 5) /**< Shifted mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_PCRUNNING                    (0x1UL << 6)                            /**< Performance Counters Running */
#define _MSC_STATUS_PCRUNNING_SHIFT             6                                       /**< Shift value for MSC_PCRUNNING */
#define _MSC_STATUS_PCRUNNING_MASK              0x40UL                                  /**< Bit mask for MSC_PCRUNNING */
#define _MSC_STATUS_PCRUNNING_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for MSC_STATUS */
#define MSC_STATUS_PCRUNNING_DEFAULT            (_MSC_STATUS_PCRUNNING_DEFAULT << 6)    /**< Shifted mode DEFAULT for MSC_STATUS */

/* Bit fields for MSC IF */
#define _MSC_IF_RESETVALUE                      0x00000000UL                    /**< Default value for MSC_IF */
#define _MSC_IF_MASK                            0x0000003FUL                    /**< Mask for MSC_IF */
#define MSC_IF_ERASE                            (0x1UL << 0)                    /**< Erase Done Interrupt Read Flag */
#define _MSC_IF_ERASE_SHIFT                     0                               /**< Shift value for MSC_ERASE */
#define _MSC_IF_ERASE_MASK                      0x1UL                           /**< Bit mask for MSC_ERASE */
#define _MSC_IF_ERASE_DEFAULT                   0x00000000UL                    /**< Mode DEFAULT for MSC_IF */
#define MSC_IF_ERASE_DEFAULT                    (_MSC_IF_ERASE_DEFAULT << 0)    /**< Shifted mode DEFAULT for MSC_IF */
#define MSC_IF_WRITE                            (0x1UL << 1)                    /**< Write Done Interrupt Read Flag */
#define _MSC_IF_WRITE_SHIFT                     1                               /**< Shift value for MSC_WRITE */
#define _MSC_IF_WRITE_MASK                      0x2UL                           /**< Bit mask for MSC_WRITE */
#define _MSC_IF_WRITE_DEFAULT                   0x00000000UL                    /**< Mode DEFAULT for MSC_IF */
#define MSC_IF_WRITE_DEFAULT                    (_MSC_IF_WRITE_DEFAULT << 1)    /**< Shifted mode DEFAULT for MSC_IF */
#define MSC_IF_CHOF                             (0x1UL << 2)                    /**< Cache Hits Overflow Interrupt Flag */
#define _MSC_IF_CHOF_SHIFT                      2                               /**< Shift value for MSC_CHOF */
#define _MSC_IF_CHOF_MASK                       0x4UL                           /**< Bit mask for MSC_CHOF */
#define _MSC_IF_CHOF_DEFAULT                    0x00000000UL                    /**< Mode DEFAULT for MSC_IF */
#define MSC_IF_CHOF_DEFAULT                     (_MSC_IF_CHOF_DEFAULT << 2)     /**< Shifted mode DEFAULT for MSC_IF */
#define MSC_IF_CMOF                             (0x1UL << 3)                    /**< Cache Misses Overflow Interrupt Flag */
#define _MSC_IF_CMOF_SHIFT                      3                               /**< Shift value for MSC_CMOF */
#define _MSC_IF_CMOF_MASK                       0x8UL                           /**< Bit mask for MSC_CMOF */
#define _MSC_IF_CMOF_DEFAULT                    0x00000000UL                    /**< Mode DEFAULT for MSC_IF */
#define MSC_IF_CMOF_DEFAULT                     (_MSC_IF_CMOF_DEFAULT << 3)     /**< Shifted mode DEFAULT for MSC_IF */
#define MSC_IF_PWRUPF                           (0x1UL << 4)                    /**< Flash Power Up Sequence Complete Flag */
#define _MSC_IF_PWRUPF_SHIFT                    4                               /**< Shift value for MSC_PWRUPF */
#define _MSC_IF_PWRUPF_MASK                     0x10UL                          /**< Bit mask for MSC_PWRUPF */
#define _MSC_IF_PWRUPF_DEFAULT                  0x00000000UL                    /**< Mode DEFAULT for MSC_IF */
#define MSC_IF_PWRUPF_DEFAULT                   (_MSC_IF_PWRUPF_DEFAULT << 4)   /**< Shifted mode DEFAULT for MSC_IF */
#define MSC_IF_ICACHERR                         (0x1UL << 5)                    /**< ICache RAM Parity Error Flag */
#define _MSC_IF_ICACHERR_SHIFT                  5                               /**< Shift value for MSC_ICACHERR */
#define _MSC_IF_ICACHERR_MASK                   0x20UL                          /**< Bit mask for MSC_ICACHERR */
#define _MSC_IF_ICACHERR_DEFAULT                0x00000000UL                    /**< Mode DEFAULT for MSC_IF */
#define MSC_IF_ICACHERR_DEFAULT                 (_MSC_IF_ICACHERR_DEFAULT << 5) /**< Shifted mode DEFAULT for MSC_IF */

/* Bit fields for MSC IFS */
#define _MSC_IFS_RESETVALUE                     0x00000000UL                     /**< Default value for MSC_IFS */
#define _MSC_IFS_MASK                           0x0000003FUL                     /**< Mask for MSC_IFS */
#define MSC_IFS_ERASE                           (0x1UL << 0)                     /**< Set ERASE Interrupt Flag */
#define _MSC_IFS_ERASE_SHIFT                    0                                /**< Shift value for MSC_ERASE */
#define _MSC_IFS_ERASE_MASK                     0x1UL                            /**< Bit mask for MSC_ERASE */
#define _MSC_IFS_ERASE_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for MSC_IFS */
#define MSC_IFS_ERASE_DEFAULT                   (_MSC_IFS_ERASE_DEFAULT << 0)    /**< Shifted mode DEFAULT for MSC_IFS */
#define MSC_IFS_WRITE                           (0x1UL << 1)                     /**< Set WRITE Interrupt Flag */
#define _MSC_IFS_WRITE_SHIFT                    1                                /**< Shift value for MSC_WRITE */
#define _MSC_IFS_WRITE_MASK                     0x2UL                            /**< Bit mask for MSC_WRITE */
#define _MSC_IFS_WRITE_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for MSC_IFS */
#define MSC_IFS_WRITE_DEFAULT                   (_MSC_IFS_WRITE_DEFAULT << 1)    /**< Shifted mode DEFAULT for MSC_IFS */
#define MSC_IFS_CHOF                            (0x1UL << 2)                     /**< Set CHOF Interrupt Flag */
#define _MSC_IFS_CHOF_SHIFT                     2                                /**< Shift value for MSC_CHOF */
#define _MSC_IFS_CHOF_MASK                      0x4UL                            /**< Bit mask for MSC_CHOF */
#define _MSC_IFS_CHOF_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for MSC_IFS */
#define MSC_IFS_CHOF_DEFAULT                    (_MSC_IFS_CHOF_DEFAULT << 2)     /**< Shifted mode DEFAULT for MSC_IFS */
#define MSC_IFS_CMOF                            (0x1UL << 3)                     /**< Set CMOF Interrupt Flag */
#define _MSC_IFS_CMOF_SHIFT                     3                                /**< Shift value for MSC_CMOF */
#define _MSC_IFS_CMOF_MASK                      0x8UL                            /**< Bit mask for MSC_CMOF */
#define _MSC_IFS_CMOF_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for MSC_IFS */
#define MSC_IFS_CMOF_DEFAULT                    (_MSC_IFS_CMOF_DEFAULT << 3)     /**< Shifted mode DEFAULT for MSC_IFS */
#define MSC_IFS_PWRUPF                          (0x1UL << 4)                     /**< Set PWRUPF Interrupt Flag */
#define _MSC_IFS_PWRUPF_SHIFT                   4                                /**< Shift value for MSC_PWRUPF */
#define _MSC_IFS_PWRUPF_MASK                    0x10UL                           /**< Bit mask for MSC_PWRUPF */
#define _MSC_IFS_PWRUPF_DEFAULT                 0x00000000UL                     /**< Mode DEFAULT for MSC_IFS */
#define MSC_IFS_PWRUPF_DEFAULT                  (_MSC_IFS_PWRUPF_DEFAULT << 4)   /**< Shifted mode DEFAULT for MSC_IFS */
#define MSC_IFS_ICACHERR                        (0x1UL << 5)                     /**< Set ICACHERR Interrupt Flag */
#define _MSC_IFS_ICACHERR_SHIFT                 5                                /**< Shift value for MSC_ICACHERR */
#define _MSC_IFS_ICACHERR_MASK                  0x20UL                           /**< Bit mask for MSC_ICACHERR */
#define _MSC_IFS_ICACHERR_DEFAULT               0x00000000UL                     /**< Mode DEFAULT for MSC_IFS */
#define MSC_IFS_ICACHERR_DEFAULT                (_MSC_IFS_ICACHERR_DEFAULT << 5) /**< Shifted mode DEFAULT for MSC_IFS */

/* Bit fields for MSC IFC */
#define _MSC_IFC_RESETVALUE                     0x00000000UL                     /**< Default value for MSC_IFC */
#define _MSC_IFC_MASK                           0x0000003FUL                     /**< Mask for MSC_IFC */
#define MSC_IFC_ERASE                           (0x1UL << 0)                     /**< Clear ERASE Interrupt Flag */
#define _MSC_IFC_ERASE_SHIFT                    0                                /**< Shift value for MSC_ERASE */
#define _MSC_IFC_ERASE_MASK                     0x1UL                            /**< Bit mask for MSC_ERASE */
#define _MSC_IFC_ERASE_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for MSC_IFC */
#define MSC_IFC_ERASE_DEFAULT                   (_MSC_IFC_ERASE_DEFAULT << 0)    /**< Shifted mode DEFAULT for MSC_IFC */
#define MSC_IFC_WRITE                           (0x1UL << 1)                     /**< Clear WRITE Interrupt Flag */
#define _MSC_IFC_WRITE_SHIFT                    1                                /**< Shift value for MSC_WRITE */
#define _MSC_IFC_WRITE_MASK                     0x2UL                            /**< Bit mask for MSC_WRITE */
#define _MSC_IFC_WRITE_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for MSC_IFC */
#define MSC_IFC_WRITE_DEFAULT                   (_MSC_IFC_WRITE_DEFAULT << 1)    /**< Shifted mode DEFAULT for MSC_IFC */
#define MSC_IFC_CHOF                            (0x1UL << 2)                     /**< Clear CHOF Interrupt Flag */
#define _MSC_IFC_CHOF_SHIFT                     2                                /**< Shift value for MSC_CHOF */
#define _MSC_IFC_CHOF_MASK                      0x4UL                            /**< Bit mask for MSC_CHOF */
#define _MSC_IFC_CHOF_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for MSC_IFC */
#define MSC_IFC_CHOF_DEFAULT                    (_MSC_IFC_CHOF_DEFAULT << 2)     /**< Shifted mode DEFAULT for MSC_IFC */
#define MSC_IFC_CMOF                            (0x1UL << 3)                     /**< Clear CMOF Interrupt Flag */
#define _MSC_IFC_CMOF_SHIFT                     3                                /**< Shift value for MSC_CMOF */
#define _MSC_IFC_CMOF_MASK                      0x8UL                            /**< Bit mask for MSC_CMOF */
#define _MSC_IFC_CMOF_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for MSC_IFC */
#define MSC_IFC_CMOF_DEFAULT                    (_MSC_IFC_CMOF_DEFAULT << 3)     /**< Shifted mode DEFAULT for MSC_IFC */
#define MSC_IFC_PWRUPF                          (0x1UL << 4)                     /**< Clear PWRUPF Interrupt Flag */
#define _MSC_IFC_PWRUPF_SHIFT                   4                                /**< Shift value for MSC_PWRUPF */
#define _MSC_IFC_PWRUPF_MASK                    0x10UL                           /**< Bit mask for MSC_PWRUPF */
#define _MSC_IFC_PWRUPF_DEFAULT                 0x00000000UL                     /**< Mode DEFAULT for MSC_IFC */
#define MSC_IFC_PWRUPF_DEFAULT                  (_MSC_IFC_PWRUPF_DEFAULT << 4)   /**< Shifted mode DEFAULT for MSC_IFC */
#define MSC_IFC_ICACHERR                        (0x1UL << 5)                     /**< Clear ICACHERR Interrupt Flag */
#define _MSC_IFC_ICACHERR_SHIFT                 5                                /**< Shift value for MSC_ICACHERR */
#define _MSC_IFC_ICACHERR_MASK                  0x20UL                           /**< Bit mask for MSC_ICACHERR */
#define _MSC_IFC_ICACHERR_DEFAULT               0x00000000UL                     /**< Mode DEFAULT for MSC_IFC */
#define MSC_IFC_ICACHERR_DEFAULT                (_MSC_IFC_ICACHERR_DEFAULT << 5) /**< Shifted mode DEFAULT for MSC_IFC */

/* Bit fields for MSC IEN */
#define _MSC_IEN_RESETVALUE                     0x00000000UL                     /**< Default value for MSC_IEN */
#define _MSC_IEN_MASK                           0x0000003FUL                     /**< Mask for MSC_IEN */
#define MSC_IEN_ERASE                           (0x1UL << 0)                     /**< ERASE Interrupt Enable */
#define _MSC_IEN_ERASE_SHIFT                    0                                /**< Shift value for MSC_ERASE */
#define _MSC_IEN_ERASE_MASK                     0x1UL                            /**< Bit mask for MSC_ERASE */
#define _MSC_IEN_ERASE_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for MSC_IEN */
#define MSC_IEN_ERASE_DEFAULT                   (_MSC_IEN_ERASE_DEFAULT << 0)    /**< Shifted mode DEFAULT for MSC_IEN */
#define MSC_IEN_WRITE                           (0x1UL << 1)                     /**< WRITE Interrupt Enable */
#define _MSC_IEN_WRITE_SHIFT                    1                                /**< Shift value for MSC_WRITE */
#define _MSC_IEN_WRITE_MASK                     0x2UL                            /**< Bit mask for MSC_WRITE */
#define _MSC_IEN_WRITE_DEFAULT                  0x00000000UL                     /**< Mode DEFAULT for MSC_IEN */
#define MSC_IEN_WRITE_DEFAULT                   (_MSC_IEN_WRITE_DEFAULT << 1)    /**< Shifted mode DEFAULT for MSC_IEN */
#define MSC_IEN_CHOF                            (0x1UL << 2)                     /**< CHOF Interrupt Enable */
#define _MSC_IEN_CHOF_SHIFT                     2                                /**< Shift value for MSC_CHOF */
#define _MSC_IEN_CHOF_MASK                      0x4UL                            /**< Bit mask for MSC_CHOF */
#define _MSC_IEN_CHOF_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for MSC_IEN */
#define MSC_IEN_CHOF_DEFAULT                    (_MSC_IEN_CHOF_DEFAULT << 2)     /**< Shifted mode DEFAULT for MSC_IEN */
#define MSC_IEN_CMOF                            (0x1UL << 3)                     /**< CMOF Interrupt Enable */
#define _MSC_IEN_CMOF_SHIFT                     3                                /**< Shift value for MSC_CMOF */
#define _MSC_IEN_CMOF_MASK                      0x8UL                            /**< Bit mask for MSC_CMOF */
#define _MSC_IEN_CMOF_DEFAULT                   0x00000000UL                     /**< Mode DEFAULT for MSC_IEN */
#define MSC_IEN_CMOF_DEFAULT                    (_MSC_IEN_CMOF_DEFAULT << 3)     /**< Shifted mode DEFAULT for MSC_IEN */
#define MSC_IEN_PWRUPF                          (0x1UL << 4)                     /**< PWRUPF Interrupt Enable */
#define _MSC_IEN_PWRUPF_SHIFT                   4                                /**< Shift value for MSC_PWRUPF */
#define _MSC_IEN_PWRUPF_MASK                    0x10UL                           /**< Bit mask for MSC_PWRUPF */
#define _MSC_IEN_PWRUPF_DEFAULT                 0x00000000UL                     /**< Mode DEFAULT for MSC_IEN */
#define MSC_IEN_PWRUPF_DEFAULT                  (_MSC_IEN_PWRUPF_DEFAULT << 4)   /**< Shifted mode DEFAULT for MSC_IEN */
#define MSC_IEN_ICACHERR                        (0x1UL << 5)                     /**< ICACHERR Interrupt Enable */
#define _MSC_IEN_ICACHERR_SHIFT                 5                                /**< Shift value for MSC_ICACHERR */
#define _MSC_IEN_ICACHERR_MASK                  0x20UL                           /**< Bit mask for MSC_ICACHERR */
#define _MSC_IEN_ICACHERR_DEFAULT               0x00000000UL                     /**< Mode DEFAULT for MSC_IEN */
#define MSC_IEN_ICACHERR_DEFAULT                (_MSC_IEN_ICACHERR_DEFAULT << 5) /**< Shifted mode DEFAULT for MSC_IEN */

/* Bit fields for MSC LOCK */
#define _MSC_LOCK_RESETVALUE                    0x00000000UL                      /**< Default value for MSC_LOCK */
#define _MSC_LOCK_MASK                          0x0000FFFFUL                      /**< Mask for MSC_LOCK */
#define _MSC_LOCK_LOCKKEY_SHIFT                 0                                 /**< Shift value for MSC_LOCKKEY */
#define _MSC_LOCK_LOCKKEY_MASK                  0xFFFFUL                          /**< Bit mask for MSC_LOCKKEY */
#define _MSC_LOCK_LOCKKEY_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for MSC_LOCK */
#define _MSC_LOCK_LOCKKEY_LOCK                  0x00000000UL                      /**< Mode LOCK for MSC_LOCK */
#define _MSC_LOCK_LOCKKEY_UNLOCKED              0x00000000UL                      /**< Mode UNLOCKED for MSC_LOCK */
#define _MSC_LOCK_LOCKKEY_LOCKED                0x00000001UL                      /**< Mode LOCKED for MSC_LOCK */
#define _MSC_LOCK_LOCKKEY_UNLOCK                0x00001B71UL                      /**< Mode UNLOCK for MSC_LOCK */
#define MSC_LOCK_LOCKKEY_DEFAULT                (_MSC_LOCK_LOCKKEY_DEFAULT << 0)  /**< Shifted mode DEFAULT for MSC_LOCK */
#define MSC_LOCK_LOCKKEY_LOCK                   (_MSC_LOCK_LOCKKEY_LOCK << 0)     /**< Shifted mode LOCK for MSC_LOCK */
#define MSC_LOCK_LOCKKEY_UNLOCKED               (_MSC_LOCK_LOCKKEY_UNLOCKED << 0) /**< Shifted mode UNLOCKED for MSC_LOCK */
#define MSC_LOCK_LOCKKEY_LOCKED                 (_MSC_LOCK_LOCKKEY_LOCKED << 0)   /**< Shifted mode LOCKED for MSC_LOCK */
#define MSC_LOCK_LOCKKEY_UNLOCK                 (_MSC_LOCK_LOCKKEY_UNLOCK << 0)   /**< Shifted mode UNLOCK for MSC_LOCK */

/* Bit fields for MSC CACHECMD */
#define _MSC_CACHECMD_RESETVALUE                0x00000000UL                          /**< Default value for MSC_CACHECMD */
#define _MSC_CACHECMD_MASK                      0x00000007UL                          /**< Mask for MSC_CACHECMD */
#define MSC_CACHECMD_INVCACHE                   (0x1UL << 0)                          /**< Invalidate Instruction Cache */
#define _MSC_CACHECMD_INVCACHE_SHIFT            0                                     /**< Shift value for MSC_INVCACHE */
#define _MSC_CACHECMD_INVCACHE_MASK             0x1UL                                 /**< Bit mask for MSC_INVCACHE */
#define _MSC_CACHECMD_INVCACHE_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for MSC_CACHECMD */
#define MSC_CACHECMD_INVCACHE_DEFAULT           (_MSC_CACHECMD_INVCACHE_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_CACHECMD */
#define MSC_CACHECMD_STARTPC                    (0x1UL << 1)                          /**< Start Performance Counters */
#define _MSC_CACHECMD_STARTPC_SHIFT             1                                     /**< Shift value for MSC_STARTPC */
#define _MSC_CACHECMD_STARTPC_MASK              0x2UL                                 /**< Bit mask for MSC_STARTPC */
#define _MSC_CACHECMD_STARTPC_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for MSC_CACHECMD */
#define MSC_CACHECMD_STARTPC_DEFAULT            (_MSC_CACHECMD_STARTPC_DEFAULT << 1)  /**< Shifted mode DEFAULT for MSC_CACHECMD */
#define MSC_CACHECMD_STOPPC                     (0x1UL << 2)                          /**< Stop Performance Counters */
#define _MSC_CACHECMD_STOPPC_SHIFT              2                                     /**< Shift value for MSC_STOPPC */
#define _MSC_CACHECMD_STOPPC_MASK               0x4UL                                 /**< Bit mask for MSC_STOPPC */
#define _MSC_CACHECMD_STOPPC_DEFAULT            0x00000000UL                          /**< Mode DEFAULT for MSC_CACHECMD */
#define MSC_CACHECMD_STOPPC_DEFAULT             (_MSC_CACHECMD_STOPPC_DEFAULT << 2)   /**< Shifted mode DEFAULT for MSC_CACHECMD */

/* Bit fields for MSC CACHEHITS */
#define _MSC_CACHEHITS_RESETVALUE               0x00000000UL                            /**< Default value for MSC_CACHEHITS */
#define _MSC_CACHEHITS_MASK                     0x000FFFFFUL                            /**< Mask for MSC_CACHEHITS */
#define _MSC_CACHEHITS_CACHEHITS_SHIFT          0                                       /**< Shift value for MSC_CACHEHITS */
#define _MSC_CACHEHITS_CACHEHITS_MASK           0xFFFFFUL                               /**< Bit mask for MSC_CACHEHITS */
#define _MSC_CACHEHITS_CACHEHITS_DEFAULT        0x00000000UL                            /**< Mode DEFAULT for MSC_CACHEHITS */
#define MSC_CACHEHITS_CACHEHITS_DEFAULT         (_MSC_CACHEHITS_CACHEHITS_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_CACHEHITS */

/* Bit fields for MSC CACHEMISSES */
#define _MSC_CACHEMISSES_RESETVALUE             0x00000000UL                                /**< Default value for MSC_CACHEMISSES */
#define _MSC_CACHEMISSES_MASK                   0x000FFFFFUL                                /**< Mask for MSC_CACHEMISSES */
#define _MSC_CACHEMISSES_CACHEMISSES_SHIFT      0                                           /**< Shift value for MSC_CACHEMISSES */
#define _MSC_CACHEMISSES_CACHEMISSES_MASK       0xFFFFFUL                                   /**< Bit mask for MSC_CACHEMISSES */
#define _MSC_CACHEMISSES_CACHEMISSES_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for MSC_CACHEMISSES */
#define MSC_CACHEMISSES_CACHEMISSES_DEFAULT     (_MSC_CACHEMISSES_CACHEMISSES_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_CACHEMISSES */

/* Bit fields for MSC MASSLOCK */
#define _MSC_MASSLOCK_RESETVALUE                0x00000001UL                          /**< Default value for MSC_MASSLOCK */
#define _MSC_MASSLOCK_MASK                      0x0000FFFFUL                          /**< Mask for MSC_MASSLOCK */
#define _MSC_MASSLOCK_LOCKKEY_SHIFT             0                                     /**< Shift value for MSC_LOCKKEY */
#define _MSC_MASSLOCK_LOCKKEY_MASK              0xFFFFUL                              /**< Bit mask for MSC_LOCKKEY */
#define _MSC_MASSLOCK_LOCKKEY_LOCK              0x00000000UL                          /**< Mode LOCK for MSC_MASSLOCK */
#define _MSC_MASSLOCK_LOCKKEY_UNLOCKED          0x00000000UL                          /**< Mode UNLOCKED for MSC_MASSLOCK */
#define _MSC_MASSLOCK_LOCKKEY_DEFAULT           0x00000001UL                          /**< Mode DEFAULT for MSC_MASSLOCK */
#define _MSC_MASSLOCK_LOCKKEY_LOCKED            0x00000001UL                          /**< Mode LOCKED for MSC_MASSLOCK */
#define _MSC_MASSLOCK_LOCKKEY_UNLOCK            0x0000631AUL                          /**< Mode UNLOCK for MSC_MASSLOCK */
#define MSC_MASSLOCK_LOCKKEY_LOCK               (_MSC_MASSLOCK_LOCKKEY_LOCK << 0)     /**< Shifted mode LOCK for MSC_MASSLOCK */
#define MSC_MASSLOCK_LOCKKEY_UNLOCKED           (_MSC_MASSLOCK_LOCKKEY_UNLOCKED << 0) /**< Shifted mode UNLOCKED for MSC_MASSLOCK */
#define MSC_MASSLOCK_LOCKKEY_DEFAULT            (_MSC_MASSLOCK_LOCKKEY_DEFAULT << 0)  /**< Shifted mode DEFAULT for MSC_MASSLOCK */
#define MSC_MASSLOCK_LOCKKEY_LOCKED             (_MSC_MASSLOCK_LOCKKEY_LOCKED << 0)   /**< Shifted mode LOCKED for MSC_MASSLOCK */
#define MSC_MASSLOCK_LOCKKEY_UNLOCK             (_MSC_MASSLOCK_LOCKKEY_UNLOCK << 0)   /**< Shifted mode UNLOCK for MSC_MASSLOCK */

/* Bit fields for MSC STARTUP */
#define _MSC_STARTUP_RESETVALUE                 0x1300104DUL                         /**< Default value for MSC_STARTUP */
#define _MSC_STARTUP_MASK                       0x773FF3FFUL                         /**< Mask for MSC_STARTUP */
#define _MSC_STARTUP_STDLY0_SHIFT               0                                    /**< Shift value for MSC_STDLY0 */
#define _MSC_STARTUP_STDLY0_MASK                0x3FFUL                              /**< Bit mask for MSC_STDLY0 */
#define _MSC_STARTUP_STDLY0_DEFAULT             0x0000004DUL                         /**< Mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STDLY0_DEFAULT              (_MSC_STARTUP_STDLY0_DEFAULT << 0)   /**< Shifted mode DEFAULT for MSC_STARTUP */
#define _MSC_STARTUP_STDLY1_SHIFT               12                                   /**< Shift value for MSC_STDLY1 */
#define _MSC_STARTUP_STDLY1_MASK                0x3FF000UL                           /**< Bit mask for MSC_STDLY1 */
#define _MSC_STARTUP_STDLY1_DEFAULT             0x00000001UL                         /**< Mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STDLY1_DEFAULT              (_MSC_STARTUP_STDLY1_DEFAULT << 12)  /**< Shifted mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_ASTWAIT                     (0x1UL << 24)                        /**< Active Startup Wait */
#define _MSC_STARTUP_ASTWAIT_SHIFT              24                                   /**< Shift value for MSC_ASTWAIT */
#define _MSC_STARTUP_ASTWAIT_MASK               0x1000000UL                          /**< Bit mask for MSC_ASTWAIT */
#define _MSC_STARTUP_ASTWAIT_DEFAULT            0x00000001UL                         /**< Mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_ASTWAIT_DEFAULT             (_MSC_STARTUP_ASTWAIT_DEFAULT << 24) /**< Shifted mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STWSEN                      (0x1UL << 25)                        /**< Startup Waitstates Enable */
#define _MSC_STARTUP_STWSEN_SHIFT               25                                   /**< Shift value for MSC_STWSEN */
#define _MSC_STARTUP_STWSEN_MASK                0x2000000UL                          /**< Bit mask for MSC_STWSEN */
#define _MSC_STARTUP_STWSEN_DEFAULT             0x00000001UL                         /**< Mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STWSEN_DEFAULT              (_MSC_STARTUP_STWSEN_DEFAULT << 25)  /**< Shifted mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STWSAEN                     (0x1UL << 26)                        /**< Startup Waitstates Always Enable */
#define _MSC_STARTUP_STWSAEN_SHIFT              26                                   /**< Shift value for MSC_STWSAEN */
#define _MSC_STARTUP_STWSAEN_MASK               0x4000000UL                          /**< Bit mask for MSC_STWSAEN */
#define _MSC_STARTUP_STWSAEN_DEFAULT            0x00000000UL                         /**< Mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STWSAEN_DEFAULT             (_MSC_STARTUP_STWSAEN_DEFAULT << 26) /**< Shifted mode DEFAULT for MSC_STARTUP */
#define _MSC_STARTUP_STWS_SHIFT                 28                                   /**< Shift value for MSC_STWS */
#define _MSC_STARTUP_STWS_MASK                  0x70000000UL                         /**< Bit mask for MSC_STWS */
#define _MSC_STARTUP_STWS_DEFAULT               0x00000001UL                         /**< Mode DEFAULT for MSC_STARTUP */
#define MSC_STARTUP_STWS_DEFAULT                (_MSC_STARTUP_STWS_DEFAULT << 28)    /**< Shifted mode DEFAULT for MSC_STARTUP */

/* Bit fields for MSC CMD */
#define _MSC_CMD_RESETVALUE                     0x00000000UL                  /**< Default value for MSC_CMD */
#define _MSC_CMD_MASK                           0x00000001UL                  /**< Mask for MSC_CMD */
#define MSC_CMD_PWRUP                           (0x1UL << 0)                  /**< Flash Power Up Command */
#define _MSC_CMD_PWRUP_SHIFT                    0                             /**< Shift value for MSC_PWRUP */
#define _MSC_CMD_PWRUP_MASK                     0x1UL                         /**< Bit mask for MSC_PWRUP */
#define _MSC_CMD_PWRUP_DEFAULT                  0x00000000UL                  /**< Mode DEFAULT for MSC_CMD */
#define MSC_CMD_PWRUP_DEFAULT                   (_MSC_CMD_PWRUP_DEFAULT << 0) /**< Shifted mode DEFAULT for MSC_CMD */

/** @} */
/** @} End of group EFR32MG1P_MSC */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

