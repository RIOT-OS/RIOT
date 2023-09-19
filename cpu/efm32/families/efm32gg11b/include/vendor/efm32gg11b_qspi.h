/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_QSPI register and bit field definitions
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
 * @defgroup EFM32GG11B_QSPI QSPI
 * @{
 * @brief EFM32GG11B_QSPI Register Declaration
 ******************************************************************************/
/** QSPI Register Declaration */
typedef struct {
  __IOM uint32_t CONFIG;                     /**< Octal-SPI Configuration Register  */
  __IOM uint32_t DEVINSTRRDCONFIG;           /**< Device Read Instruction Configuration Register  */
  __IOM uint32_t DEVINSTRWRCONFIG;           /**< Device Write Instruction Configuration Register  */
  __IOM uint32_t DEVDELAY;                   /**< Device Delay Register  */
  __IOM uint32_t RDDATACAPTURE;              /**< Read Data Capture Register  */
  __IOM uint32_t DEVSIZECONFIG;              /**< Device Size Configuration Register  */
  __IOM uint32_t SRAMPARTITIONCFG;           /**< SRAM Partition Configuration Register  */
  __IOM uint32_t INDAHBADDRTRIGGER;          /**< Indirect Address Trigger Register  */
  uint32_t       RESERVED0[1U];              /**< Reserved for future use **/
  __IOM uint32_t REMAPADDR;                  /**< Remap Address Register  */
  __IOM uint32_t MODEBITCONFIG;              /**< Mode Bit Configuration Register  */
  __IM uint32_t  SRAMFILL;                   /**< SRAM Fill Register  */
  __IOM uint32_t TXTHRESH;                   /**< TX Threshold Register  */
  __IOM uint32_t RXTHRESH;                   /**< RX Threshold Register  */
  __IOM uint32_t WRITECOMPLETIONCTRL;        /**< Write Completion Control Register  */
  __IOM uint32_t NOOFPOLLSBEFEXP;            /**< Polling Expiration Register  */
  __IOM uint32_t IRQSTATUS;                  /**< Interrupt Status Register  */
  __IOM uint32_t IRQMASK;                    /**< Interrupt Mask  */
  uint32_t       RESERVED1[2U];              /**< Reserved for future use **/
  __IOM uint32_t LOWERWRPROT;                /**< Lower Write Protection Register  */
  __IOM uint32_t UPPERWRPROT;                /**< Upper Write Protection Register  */
  __IOM uint32_t WRPROTCTRL;                 /**< Write Protection Control Register  */
  uint32_t       RESERVED2[1U];              /**< Reserved for future use **/
  __IOM uint32_t INDIRECTREADXFERCTRL;       /**< Indirect Read Transfer Control Register  */
  __IOM uint32_t INDIRECTREADXFERWATERMARK;  /**< Indirect Read Transfer Watermark Register  */
  __IOM uint32_t INDIRECTREADXFERSTART;      /**< Indirect Read Transfer Start Address Register  */
  __IOM uint32_t INDIRECTREADXFERNUMBYTES;   /**< Indirect Read Transfer Number Bytes Register  */
  __IOM uint32_t INDIRECTWRITEXFERCTRL;      /**< Indirect Write Transfer Control Register  */
  __IOM uint32_t INDIRECTWRITEXFERWATERMARK; /**< Indirect Write Transfer Watermark Register  */
  __IOM uint32_t INDIRECTWRITEXFERSTART;     /**< Indirect Write Transfer Start Address Register  */
  __IOM uint32_t INDIRECTWRITEXFERNUMBYTES;  /**< Indirect Write Transfer Number Bytes Register  */
  __IOM uint32_t INDIRECTTRIGGERADDRRANGE;   /**< Indirect Trigger Address Range Register  */
  uint32_t       RESERVED3[2U];              /**< Reserved for future use **/
  __IOM uint32_t FLASHCOMMANDCTRLMEM;        /**< Flash Command Control Memory Register (STIG)  */
  __IOM uint32_t FLASHCMDCTRL;               /**< Flash Command Control Register (STIG)  */
  __IOM uint32_t FLASHCMDADDR;               /**< Flash Command Address Register (STIG)  */
  uint32_t       RESERVED4[2U];              /**< Reserved for future use **/
  __IM uint32_t  FLASHRDDATALOWER;           /**< Flash Command Read Data Register (Lower) (STIG)  */
  __IM uint32_t  FLASHRDDATAUPPER;           /**< Flash Command Read Data Register (Upper) (STIG)  */
  __IOM uint32_t FLASHWRDATALOWER;           /**< Flash Command Write Data Register (Lower) (STIG)  */
  __IOM uint32_t FLASHWRDATAUPPER;           /**< Flash Command Write Data Register (Upper) (STIG)  */
  __IOM uint32_t POLLINGFLASHSTATUS;         /**< Polling Flash Status Register  */
  __IOM uint32_t PHYCONFIGURATION;           /**< PHY Configuration Register  */

  uint32_t       RESERVED5[10U];             /**< Reserved for future use **/
  __IOM uint32_t OPCODEEXTLOWER;             /**< Opcode Extension Register (Lower)  */
  __IOM uint32_t OPCODEEXTUPPER;             /**< Opcode Extension Register (Upper)  */
  uint32_t       RESERVED6[5U];              /**< Reserved for future use **/
  __IM uint32_t  MODULEID;                   /**< Module ID Register  */

  uint32_t       RESERVED7[1U];              /**< Reserved for future use **/
  __IOM uint32_t ROUTEPEN;                   /**< I/O Routing Pin Enable Register  */
  __IOM uint32_t ROUTELOC0;                  /**< I/O Route Location Register 0  */
} QSPI_TypeDef;                              /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_QSPI
 * @{
 * @defgroup EFM32GG11B_QSPI_BitFields  QSPI Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for QSPI CONFIG */
#define _QSPI_CONFIG_RESETVALUE                                   0x80780081UL                                   /**< Default value for QSPI_CONFIG */
#define _QSPI_CONFIG_MASK                                         0xE3FF4F8FUL                                   /**< Mask for QSPI_CONFIG */
#define QSPI_CONFIG_ENBSPI                                        (0x1UL << 0)                                   /**< QSPI Enable */
#define _QSPI_CONFIG_ENBSPI_SHIFT                                 0                                              /**< Shift value for QSPI_ENBSPI */
#define _QSPI_CONFIG_ENBSPI_MASK                                  0x1UL                                          /**< Bit mask for QSPI_ENBSPI */
#define _QSPI_CONFIG_ENBSPI_DEFAULT                               0x00000001UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBSPI_DEFAULT                                (_QSPI_CONFIG_ENBSPI_DEFAULT << 0)             /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_SELCLKPOL                                     (0x1UL << 1)                                   /**< Clock Polarity, CPOL */
#define _QSPI_CONFIG_SELCLKPOL_SHIFT                              1                                              /**< Shift value for QSPI_SELCLKPOL */
#define _QSPI_CONFIG_SELCLKPOL_MASK                               0x2UL                                          /**< Bit mask for QSPI_SELCLKPOL */
#define _QSPI_CONFIG_SELCLKPOL_DEFAULT                            0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_SELCLKPOL_DEFAULT                             (_QSPI_CONFIG_SELCLKPOL_DEFAULT << 1)          /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_SELCLKPHASE                                   (0x1UL << 2)                                   /**< Clock Phase, CPHA */
#define _QSPI_CONFIG_SELCLKPHASE_SHIFT                            2                                              /**< Shift value for QSPI_SELCLKPHASE */
#define _QSPI_CONFIG_SELCLKPHASE_MASK                             0x4UL                                          /**< Bit mask for QSPI_SELCLKPHASE */
#define _QSPI_CONFIG_SELCLKPHASE_DEFAULT                          0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_SELCLKPHASE_DEFAULT                           (_QSPI_CONFIG_SELCLKPHASE_DEFAULT << 2)        /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PHYMODEENABLE                                 (0x1UL << 3)                                   /**< PHY Mode Enable */
#define _QSPI_CONFIG_PHYMODEENABLE_SHIFT                          3                                              /**< Shift value for QSPI_PHYMODEENABLE */
#define _QSPI_CONFIG_PHYMODEENABLE_MASK                           0x8UL                                          /**< Bit mask for QSPI_PHYMODEENABLE */
#define _QSPI_CONFIG_PHYMODEENABLE_DEFAULT                        0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PHYMODEENABLE_DEFAULT                         (_QSPI_CONFIG_PHYMODEENABLE_DEFAULT << 3)      /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBDIRACCCTLR                                 (0x1UL << 7)                                   /**< Enable Direct Access Controller */
#define _QSPI_CONFIG_ENBDIRACCCTLR_SHIFT                          7                                              /**< Shift value for QSPI_ENBDIRACCCTLR */
#define _QSPI_CONFIG_ENBDIRACCCTLR_MASK                           0x80UL                                         /**< Bit mask for QSPI_ENBDIRACCCTLR */
#define _QSPI_CONFIG_ENBDIRACCCTLR_DEFAULT                        0x00000001UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBDIRACCCTLR_DEFAULT                         (_QSPI_CONFIG_ENBDIRACCCTLR_DEFAULT << 7)      /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBLEGACYIPMODE                               (0x1UL << 8)                                   /**< Legacy IP Mode Enable */
#define _QSPI_CONFIG_ENBLEGACYIPMODE_SHIFT                        8                                              /**< Shift value for QSPI_ENBLEGACYIPMODE */
#define _QSPI_CONFIG_ENBLEGACYIPMODE_MASK                         0x100UL                                        /**< Bit mask for QSPI_ENBLEGACYIPMODE */
#define _QSPI_CONFIG_ENBLEGACYIPMODE_DEFAULT                      0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBLEGACYIPMODE_DEFAULT                       (_QSPI_CONFIG_ENBLEGACYIPMODE_DEFAULT << 8)    /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PERIPHSELDEC                                  (0x1UL << 9)                                   /**< Peripheral Select Decode */
#define _QSPI_CONFIG_PERIPHSELDEC_SHIFT                           9                                              /**< Shift value for QSPI_PERIPHSELDEC */
#define _QSPI_CONFIG_PERIPHSELDEC_MASK                            0x200UL                                        /**< Bit mask for QSPI_PERIPHSELDEC */
#define _QSPI_CONFIG_PERIPHSELDEC_DEFAULT                         0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PERIPHSELDEC_DEFAULT                          (_QSPI_CONFIG_PERIPHSELDEC_DEFAULT << 9)       /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define _QSPI_CONFIG_PERIPHCSLINES_SHIFT                          10                                             /**< Shift value for QSPI_PERIPHCSLINES */
#define _QSPI_CONFIG_PERIPHCSLINES_MASK                           0xC00UL                                        /**< Bit mask for QSPI_PERIPHCSLINES */
#define _QSPI_CONFIG_PERIPHCSLINES_DEFAULT                        0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PERIPHCSLINES_DEFAULT                         (_QSPI_CONFIG_PERIPHCSLINES_DEFAULT << 10)     /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_WRPROTFLASH                                   (0x1UL << 14)                                  /**< Write Protect Flash Pin */
#define _QSPI_CONFIG_WRPROTFLASH_SHIFT                            14                                             /**< Shift value for QSPI_WRPROTFLASH */
#define _QSPI_CONFIG_WRPROTFLASH_MASK                             0x4000UL                                       /**< Bit mask for QSPI_WRPROTFLASH */
#define _QSPI_CONFIG_WRPROTFLASH_DEFAULT                          0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_WRPROTFLASH_DEFAULT                           (_QSPI_CONFIG_WRPROTFLASH_DEFAULT << 14)       /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBAHBADDRREMAP                               (0x1UL << 16)                                  /**< Enable Address Remapping */
#define _QSPI_CONFIG_ENBAHBADDRREMAP_SHIFT                        16                                             /**< Shift value for QSPI_ENBAHBADDRREMAP */
#define _QSPI_CONFIG_ENBAHBADDRREMAP_MASK                         0x10000UL                                      /**< Bit mask for QSPI_ENBAHBADDRREMAP */
#define _QSPI_CONFIG_ENBAHBADDRREMAP_DEFAULT                      0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENBAHBADDRREMAP_DEFAULT                       (_QSPI_CONFIG_ENBAHBADDRREMAP_DEFAULT << 16)   /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENTERXIPMODE                                  (0x1UL << 17)                                  /**< Enter XIP Mode on Next READ */
#define _QSPI_CONFIG_ENTERXIPMODE_SHIFT                           17                                             /**< Shift value for QSPI_ENTERXIPMODE */
#define _QSPI_CONFIG_ENTERXIPMODE_MASK                            0x20000UL                                      /**< Bit mask for QSPI_ENTERXIPMODE */
#define _QSPI_CONFIG_ENTERXIPMODE_DEFAULT                         0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENTERXIPMODE_DEFAULT                          (_QSPI_CONFIG_ENTERXIPMODE_DEFAULT << 17)      /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENTERXIPMODEIMM                               (0x1UL << 18)                                  /**< Enter XIP Mode Immediately */
#define _QSPI_CONFIG_ENTERXIPMODEIMM_SHIFT                        18                                             /**< Shift value for QSPI_ENTERXIPMODEIMM */
#define _QSPI_CONFIG_ENTERXIPMODEIMM_MASK                         0x40000UL                                      /**< Bit mask for QSPI_ENTERXIPMODEIMM */
#define _QSPI_CONFIG_ENTERXIPMODEIMM_DEFAULT                      0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENTERXIPMODEIMM_DEFAULT                       (_QSPI_CONFIG_ENTERXIPMODEIMM_DEFAULT << 18)   /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define _QSPI_CONFIG_MSTRBAUDDIV_SHIFT                            19                                             /**< Shift value for QSPI_MSTRBAUDDIV */
#define _QSPI_CONFIG_MSTRBAUDDIV_MASK                             0x780000UL                                     /**< Bit mask for QSPI_MSTRBAUDDIV */
#define _QSPI_CONFIG_MSTRBAUDDIV_DEFAULT                          0x0000000FUL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_MSTRBAUDDIV_DEFAULT                           (_QSPI_CONFIG_MSTRBAUDDIV_DEFAULT << 19)       /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENABLEAHBDECODER                              (0x1UL << 23)                                  /**< Enable Address Decoder */
#define _QSPI_CONFIG_ENABLEAHBDECODER_SHIFT                       23                                             /**< Shift value for QSPI_ENABLEAHBDECODER */
#define _QSPI_CONFIG_ENABLEAHBDECODER_MASK                        0x800000UL                                     /**< Bit mask for QSPI_ENABLEAHBDECODER */
#define _QSPI_CONFIG_ENABLEAHBDECODER_DEFAULT                     0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENABLEAHBDECODER_DEFAULT                      (_QSPI_CONFIG_ENABLEAHBDECODER_DEFAULT << 23)  /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENABLEDTRPROTOCOL                             (0x1UL << 24)                                  /**< Enable DTR Protocol */
#define _QSPI_CONFIG_ENABLEDTRPROTOCOL_SHIFT                      24                                             /**< Shift value for QSPI_ENABLEDTRPROTOCOL */
#define _QSPI_CONFIG_ENABLEDTRPROTOCOL_MASK                       0x1000000UL                                    /**< Bit mask for QSPI_ENABLEDTRPROTOCOL */
#define _QSPI_CONFIG_ENABLEDTRPROTOCOL_DEFAULT                    0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_ENABLEDTRPROTOCOL_DEFAULT                     (_QSPI_CONFIG_ENABLEDTRPROTOCOL_DEFAULT << 24) /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PIPELINEPHY                                   (0x1UL << 25)                                  /**< Pipeline PHY Mode Enable */
#define _QSPI_CONFIG_PIPELINEPHY_SHIFT                            25                                             /**< Shift value for QSPI_PIPELINEPHY */
#define _QSPI_CONFIG_PIPELINEPHY_MASK                             0x2000000UL                                    /**< Bit mask for QSPI_PIPELINEPHY */
#define _QSPI_CONFIG_PIPELINEPHY_DEFAULT                          0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_PIPELINEPHY_DEFAULT                           (_QSPI_CONFIG_PIPELINEPHY_DEFAULT << 25)       /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_CRCENABLE                                     (0x1UL << 29)                                  /**< CRC Enable Bit */
#define _QSPI_CONFIG_CRCENABLE_SHIFT                              29                                             /**< Shift value for QSPI_CRCENABLE */
#define _QSPI_CONFIG_CRCENABLE_MASK                               0x20000000UL                                   /**< Bit mask for QSPI_CRCENABLE */
#define _QSPI_CONFIG_CRCENABLE_DEFAULT                            0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_CRCENABLE_DEFAULT                             (_QSPI_CONFIG_CRCENABLE_DEFAULT << 29)         /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_DUALBYTEOPCODEEN                              (0x1UL << 30)                                  /**< Dual-byte Opcode Mode Enable Bit */
#define _QSPI_CONFIG_DUALBYTEOPCODEEN_SHIFT                       30                                             /**< Shift value for QSPI_DUALBYTEOPCODEEN */
#define _QSPI_CONFIG_DUALBYTEOPCODEEN_MASK                        0x40000000UL                                   /**< Bit mask for QSPI_DUALBYTEOPCODEEN */
#define _QSPI_CONFIG_DUALBYTEOPCODEEN_DEFAULT                     0x00000000UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_DUALBYTEOPCODEEN_DEFAULT                      (_QSPI_CONFIG_DUALBYTEOPCODEEN_DEFAULT << 30)  /**< Shifted mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_IDLE                                          (0x1UL << 31)                                  /**< Serial Interface and Low Level SPI Pipeline is IDLE */
#define _QSPI_CONFIG_IDLE_SHIFT                                   31                                             /**< Shift value for QSPI_IDLE */
#define _QSPI_CONFIG_IDLE_MASK                                    0x80000000UL                                   /**< Bit mask for QSPI_IDLE */
#define _QSPI_CONFIG_IDLE_DEFAULT                                 0x00000001UL                                   /**< Mode DEFAULT for QSPI_CONFIG */
#define QSPI_CONFIG_IDLE_DEFAULT                                  (_QSPI_CONFIG_IDLE_DEFAULT << 31)              /**< Shifted mode DEFAULT for QSPI_CONFIG */

/* Bit fields for QSPI DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_RESETVALUE                         0x00000003UL                                               /**< Default value for QSPI_DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_MASK                               0x1F1337FFUL                                               /**< Mask for QSPI_DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_RDOPCODENONXIP_SHIFT               0                                                          /**< Shift value for QSPI_RDOPCODENONXIP */
#define _QSPI_DEVINSTRRDCONFIG_RDOPCODENONXIP_MASK                0xFFUL                                                     /**< Bit mask for QSPI_RDOPCODENONXIP */
#define _QSPI_DEVINSTRRDCONFIG_RDOPCODENONXIP_DEFAULT             0x00000003UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_RDOPCODENONXIP_DEFAULT              (_QSPI_DEVINSTRRDCONFIG_RDOPCODENONXIP_DEFAULT << 0)       /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_INSTRTYPE_SHIFT                    8                                                          /**< Shift value for QSPI_INSTRTYPE */
#define _QSPI_DEVINSTRRDCONFIG_INSTRTYPE_MASK                     0x300UL                                                    /**< Bit mask for QSPI_INSTRTYPE */
#define _QSPI_DEVINSTRRDCONFIG_INSTRTYPE_DEFAULT                  0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_INSTRTYPE_DEFAULT                   (_QSPI_DEVINSTRRDCONFIG_INSTRTYPE_DEFAULT << 8)            /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_DDREN                               (0x1UL << 10)                                              /**< DDR Enable */
#define _QSPI_DEVINSTRRDCONFIG_DDREN_SHIFT                        10                                                         /**< Shift value for QSPI_DDREN */
#define _QSPI_DEVINSTRRDCONFIG_DDREN_MASK                         0x400UL                                                    /**< Bit mask for QSPI_DDREN */
#define _QSPI_DEVINSTRRDCONFIG_DDREN_DEFAULT                      0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_DDREN_DEFAULT                       (_QSPI_DEVINSTRRDCONFIG_DDREN_DEFAULT << 10)               /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_ADDRXFERTYPESTDMODE_SHIFT          12                                                         /**< Shift value for QSPI_ADDRXFERTYPESTDMODE */
#define _QSPI_DEVINSTRRDCONFIG_ADDRXFERTYPESTDMODE_MASK           0x3000UL                                                   /**< Bit mask for QSPI_ADDRXFERTYPESTDMODE */
#define _QSPI_DEVINSTRRDCONFIG_ADDRXFERTYPESTDMODE_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_ADDRXFERTYPESTDMODE_DEFAULT         (_QSPI_DEVINSTRRDCONFIG_ADDRXFERTYPESTDMODE_DEFAULT << 12) /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_DATAXFERTYPEEXTMODE_SHIFT          16                                                         /**< Shift value for QSPI_DATAXFERTYPEEXTMODE */
#define _QSPI_DEVINSTRRDCONFIG_DATAXFERTYPEEXTMODE_MASK           0x30000UL                                                  /**< Bit mask for QSPI_DATAXFERTYPEEXTMODE */
#define _QSPI_DEVINSTRRDCONFIG_DATAXFERTYPEEXTMODE_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_DATAXFERTYPEEXTMODE_DEFAULT         (_QSPI_DEVINSTRRDCONFIG_DATAXFERTYPEEXTMODE_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_MODEBITENABLE                       (0x1UL << 20)                                              /**< Mode Bit Enable */
#define _QSPI_DEVINSTRRDCONFIG_MODEBITENABLE_SHIFT                20                                                         /**< Shift value for QSPI_MODEBITENABLE */
#define _QSPI_DEVINSTRRDCONFIG_MODEBITENABLE_MASK                 0x100000UL                                                 /**< Bit mask for QSPI_MODEBITENABLE */
#define _QSPI_DEVINSTRRDCONFIG_MODEBITENABLE_DEFAULT              0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_MODEBITENABLE_DEFAULT               (_QSPI_DEVINSTRRDCONFIG_MODEBITENABLE_DEFAULT << 20)       /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define _QSPI_DEVINSTRRDCONFIG_DUMMYRDCLKCYCLES_SHIFT             24                                                         /**< Shift value for QSPI_DUMMYRDCLKCYCLES */
#define _QSPI_DEVINSTRRDCONFIG_DUMMYRDCLKCYCLES_MASK              0x1F000000UL                                               /**< Bit mask for QSPI_DUMMYRDCLKCYCLES */
#define _QSPI_DEVINSTRRDCONFIG_DUMMYRDCLKCYCLES_DEFAULT           0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRRDCONFIG */
#define QSPI_DEVINSTRRDCONFIG_DUMMYRDCLKCYCLES_DEFAULT            (_QSPI_DEVINSTRRDCONFIG_DUMMYRDCLKCYCLES_DEFAULT << 24)    /**< Shifted mode DEFAULT for QSPI_DEVINSTRRDCONFIG */

/* Bit fields for QSPI DEVINSTRWRCONFIG */
#define _QSPI_DEVINSTRWRCONFIG_RESETVALUE                         0x00000002UL                                               /**< Default value for QSPI_DEVINSTRWRCONFIG */
#define _QSPI_DEVINSTRWRCONFIG_MASK                               0x1F0331FFUL                                               /**< Mask for QSPI_DEVINSTRWRCONFIG */
#define _QSPI_DEVINSTRWRCONFIG_WROPCODE_SHIFT                     0                                                          /**< Shift value for QSPI_WROPCODE */
#define _QSPI_DEVINSTRWRCONFIG_WROPCODE_MASK                      0xFFUL                                                     /**< Bit mask for QSPI_WROPCODE */
#define _QSPI_DEVINSTRWRCONFIG_WROPCODE_DEFAULT                   0x00000002UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define QSPI_DEVINSTRWRCONFIG_WROPCODE_DEFAULT                    (_QSPI_DEVINSTRWRCONFIG_WROPCODE_DEFAULT << 0)             /**< Shifted mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define QSPI_DEVINSTRWRCONFIG_WELDIS                              (0x1UL << 8)                                               /**< WEL Disable */
#define _QSPI_DEVINSTRWRCONFIG_WELDIS_SHIFT                       8                                                          /**< Shift value for QSPI_WELDIS */
#define _QSPI_DEVINSTRWRCONFIG_WELDIS_MASK                        0x100UL                                                    /**< Bit mask for QSPI_WELDIS */
#define _QSPI_DEVINSTRWRCONFIG_WELDIS_DEFAULT                     0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define QSPI_DEVINSTRWRCONFIG_WELDIS_DEFAULT                      (_QSPI_DEVINSTRWRCONFIG_WELDIS_DEFAULT << 8)               /**< Shifted mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define _QSPI_DEVINSTRWRCONFIG_ADDRXFERTYPESTDMODE_SHIFT          12                                                         /**< Shift value for QSPI_ADDRXFERTYPESTDMODE */
#define _QSPI_DEVINSTRWRCONFIG_ADDRXFERTYPESTDMODE_MASK           0x3000UL                                                   /**< Bit mask for QSPI_ADDRXFERTYPESTDMODE */
#define _QSPI_DEVINSTRWRCONFIG_ADDRXFERTYPESTDMODE_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define QSPI_DEVINSTRWRCONFIG_ADDRXFERTYPESTDMODE_DEFAULT         (_QSPI_DEVINSTRWRCONFIG_ADDRXFERTYPESTDMODE_DEFAULT << 12) /**< Shifted mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define _QSPI_DEVINSTRWRCONFIG_DATAXFERTYPEEXTMODE_SHIFT          16                                                         /**< Shift value for QSPI_DATAXFERTYPEEXTMODE */
#define _QSPI_DEVINSTRWRCONFIG_DATAXFERTYPEEXTMODE_MASK           0x30000UL                                                  /**< Bit mask for QSPI_DATAXFERTYPEEXTMODE */
#define _QSPI_DEVINSTRWRCONFIG_DATAXFERTYPEEXTMODE_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define QSPI_DEVINSTRWRCONFIG_DATAXFERTYPEEXTMODE_DEFAULT         (_QSPI_DEVINSTRWRCONFIG_DATAXFERTYPEEXTMODE_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define _QSPI_DEVINSTRWRCONFIG_DUMMYWRCLKCYCLES_SHIFT             24                                                         /**< Shift value for QSPI_DUMMYWRCLKCYCLES */
#define _QSPI_DEVINSTRWRCONFIG_DUMMYWRCLKCYCLES_MASK              0x1F000000UL                                               /**< Bit mask for QSPI_DUMMYWRCLKCYCLES */
#define _QSPI_DEVINSTRWRCONFIG_DUMMYWRCLKCYCLES_DEFAULT           0x00000000UL                                               /**< Mode DEFAULT for QSPI_DEVINSTRWRCONFIG */
#define QSPI_DEVINSTRWRCONFIG_DUMMYWRCLKCYCLES_DEFAULT            (_QSPI_DEVINSTRWRCONFIG_DUMMYWRCLKCYCLES_DEFAULT << 24)    /**< Shifted mode DEFAULT for QSPI_DEVINSTRWRCONFIG */

/* Bit fields for QSPI DEVDELAY */
#define _QSPI_DEVDELAY_RESETVALUE                                 0x00000000UL                         /**< Default value for QSPI_DEVDELAY */
#define _QSPI_DEVDELAY_MASK                                       0xFFFFFFFFUL                         /**< Mask for QSPI_DEVDELAY */
#define _QSPI_DEVDELAY_DINIT_SHIFT                                0                                    /**< Shift value for QSPI_DINIT */
#define _QSPI_DEVDELAY_DINIT_MASK                                 0xFFUL                               /**< Bit mask for QSPI_DINIT */
#define _QSPI_DEVDELAY_DINIT_DEFAULT                              0x00000000UL                         /**< Mode DEFAULT for QSPI_DEVDELAY */
#define QSPI_DEVDELAY_DINIT_DEFAULT                               (_QSPI_DEVDELAY_DINIT_DEFAULT << 0)  /**< Shifted mode DEFAULT for QSPI_DEVDELAY */
#define _QSPI_DEVDELAY_DAFTER_SHIFT                               8                                    /**< Shift value for QSPI_DAFTER */
#define _QSPI_DEVDELAY_DAFTER_MASK                                0xFF00UL                             /**< Bit mask for QSPI_DAFTER */
#define _QSPI_DEVDELAY_DAFTER_DEFAULT                             0x00000000UL                         /**< Mode DEFAULT for QSPI_DEVDELAY */
#define QSPI_DEVDELAY_DAFTER_DEFAULT                              (_QSPI_DEVDELAY_DAFTER_DEFAULT << 8) /**< Shifted mode DEFAULT for QSPI_DEVDELAY */
#define _QSPI_DEVDELAY_DBTWN_SHIFT                                16                                   /**< Shift value for QSPI_DBTWN */
#define _QSPI_DEVDELAY_DBTWN_MASK                                 0xFF0000UL                           /**< Bit mask for QSPI_DBTWN */
#define _QSPI_DEVDELAY_DBTWN_DEFAULT                              0x00000000UL                         /**< Mode DEFAULT for QSPI_DEVDELAY */
#define QSPI_DEVDELAY_DBTWN_DEFAULT                               (_QSPI_DEVDELAY_DBTWN_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_DEVDELAY */
#define _QSPI_DEVDELAY_DNSS_SHIFT                                 24                                   /**< Shift value for QSPI_DNSS */
#define _QSPI_DEVDELAY_DNSS_MASK                                  0xFF000000UL                         /**< Bit mask for QSPI_DNSS */
#define _QSPI_DEVDELAY_DNSS_DEFAULT                               0x00000000UL                         /**< Mode DEFAULT for QSPI_DEVDELAY */
#define QSPI_DEVDELAY_DNSS_DEFAULT                                (_QSPI_DEVDELAY_DNSS_DEFAULT << 24)  /**< Shifted mode DEFAULT for QSPI_DEVDELAY */

/* Bit fields for QSPI RDDATACAPTURE */
#define _QSPI_RDDATACAPTURE_RESETVALUE                            0x00000001UL                                     /**< Default value for QSPI_RDDATACAPTURE */
#define _QSPI_RDDATACAPTURE_MASK                                  0x000F011FUL                                     /**< Mask for QSPI_RDDATACAPTURE */
#define QSPI_RDDATACAPTURE_BYPASS                                 (0x1UL << 0)                                     /**< Bypass the Adapted Loopback Clock Circuit */
#define _QSPI_RDDATACAPTURE_BYPASS_SHIFT                          0                                                /**< Shift value for QSPI_BYPASS */
#define _QSPI_RDDATACAPTURE_BYPASS_MASK                           0x1UL                                            /**< Bit mask for QSPI_BYPASS */
#define _QSPI_RDDATACAPTURE_BYPASS_DEFAULT                        0x00000001UL                                     /**< Mode DEFAULT for QSPI_RDDATACAPTURE */
#define QSPI_RDDATACAPTURE_BYPASS_DEFAULT                         (_QSPI_RDDATACAPTURE_BYPASS_DEFAULT << 0)        /**< Shifted mode DEFAULT for QSPI_RDDATACAPTURE */
#define _QSPI_RDDATACAPTURE_DELAY_SHIFT                           1                                                /**< Shift value for QSPI_DELAY */
#define _QSPI_RDDATACAPTURE_DELAY_MASK                            0x1EUL                                           /**< Bit mask for QSPI_DELAY */
#define _QSPI_RDDATACAPTURE_DELAY_DEFAULT                         0x00000000UL                                     /**< Mode DEFAULT for QSPI_RDDATACAPTURE */
#define QSPI_RDDATACAPTURE_DELAY_DEFAULT                          (_QSPI_RDDATACAPTURE_DELAY_DEFAULT << 1)         /**< Shifted mode DEFAULT for QSPI_RDDATACAPTURE */
#define QSPI_RDDATACAPTURE_DQSENABLE                              (0x1UL << 8)                                     /**< DQS Enable Bit */
#define _QSPI_RDDATACAPTURE_DQSENABLE_SHIFT                       8                                                /**< Shift value for QSPI_DQSENABLE */
#define _QSPI_RDDATACAPTURE_DQSENABLE_MASK                        0x100UL                                          /**< Bit mask for QSPI_DQSENABLE */
#define _QSPI_RDDATACAPTURE_DQSENABLE_DEFAULT                     0x00000000UL                                     /**< Mode DEFAULT for QSPI_RDDATACAPTURE */
#define QSPI_RDDATACAPTURE_DQSENABLE_DEFAULT                      (_QSPI_RDDATACAPTURE_DQSENABLE_DEFAULT << 8)     /**< Shifted mode DEFAULT for QSPI_RDDATACAPTURE */
#define _QSPI_RDDATACAPTURE_DDRREADDELAY_SHIFT                    16                                               /**< Shift value for QSPI_DDRREADDELAY */
#define _QSPI_RDDATACAPTURE_DDRREADDELAY_MASK                     0xF0000UL                                        /**< Bit mask for QSPI_DDRREADDELAY */
#define _QSPI_RDDATACAPTURE_DDRREADDELAY_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for QSPI_RDDATACAPTURE */
#define QSPI_RDDATACAPTURE_DDRREADDELAY_DEFAULT                   (_QSPI_RDDATACAPTURE_DDRREADDELAY_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_RDDATACAPTURE */

/* Bit fields for QSPI DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_RESETVALUE                            0x00101002UL                                          /**< Default value for QSPI_DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_MASK                                  0x01FFFFFFUL                                          /**< Mask for QSPI_DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_NUMADDRBYTES_SHIFT                    0                                                     /**< Shift value for QSPI_NUMADDRBYTES */
#define _QSPI_DEVSIZECONFIG_NUMADDRBYTES_MASK                     0xFUL                                                 /**< Bit mask for QSPI_NUMADDRBYTES */
#define _QSPI_DEVSIZECONFIG_NUMADDRBYTES_DEFAULT                  0x00000002UL                                          /**< Mode DEFAULT for QSPI_DEVSIZECONFIG */
#define QSPI_DEVSIZECONFIG_NUMADDRBYTES_DEFAULT                   (_QSPI_DEVSIZECONFIG_NUMADDRBYTES_DEFAULT << 0)       /**< Shifted mode DEFAULT for QSPI_DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_BYTESPERDEVICEPAGE_SHIFT              4                                                     /**< Shift value for QSPI_BYTESPERDEVICEPAGE */
#define _QSPI_DEVSIZECONFIG_BYTESPERDEVICEPAGE_MASK               0xFFF0UL                                              /**< Bit mask for QSPI_BYTESPERDEVICEPAGE */
#define _QSPI_DEVSIZECONFIG_BYTESPERDEVICEPAGE_DEFAULT            0x00000100UL                                          /**< Mode DEFAULT for QSPI_DEVSIZECONFIG */
#define QSPI_DEVSIZECONFIG_BYTESPERDEVICEPAGE_DEFAULT             (_QSPI_DEVSIZECONFIG_BYTESPERDEVICEPAGE_DEFAULT << 4) /**< Shifted mode DEFAULT for QSPI_DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_BYTESPERSUBSECTOR_SHIFT               16                                                    /**< Shift value for QSPI_BYTESPERSUBSECTOR */
#define _QSPI_DEVSIZECONFIG_BYTESPERSUBSECTOR_MASK                0x1F0000UL                                            /**< Bit mask for QSPI_BYTESPERSUBSECTOR */
#define _QSPI_DEVSIZECONFIG_BYTESPERSUBSECTOR_DEFAULT             0x00000010UL                                          /**< Mode DEFAULT for QSPI_DEVSIZECONFIG */
#define QSPI_DEVSIZECONFIG_BYTESPERSUBSECTOR_DEFAULT              (_QSPI_DEVSIZECONFIG_BYTESPERSUBSECTOR_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_MEMSIZEONCS0_SHIFT                    21                                                    /**< Shift value for QSPI_MEMSIZEONCS0 */
#define _QSPI_DEVSIZECONFIG_MEMSIZEONCS0_MASK                     0x600000UL                                            /**< Bit mask for QSPI_MEMSIZEONCS0 */
#define _QSPI_DEVSIZECONFIG_MEMSIZEONCS0_DEFAULT                  0x00000000UL                                          /**< Mode DEFAULT for QSPI_DEVSIZECONFIG */
#define QSPI_DEVSIZECONFIG_MEMSIZEONCS0_DEFAULT                   (_QSPI_DEVSIZECONFIG_MEMSIZEONCS0_DEFAULT << 21)      /**< Shifted mode DEFAULT for QSPI_DEVSIZECONFIG */
#define _QSPI_DEVSIZECONFIG_MEMSIZEONCS1_SHIFT                    23                                                    /**< Shift value for QSPI_MEMSIZEONCS1 */
#define _QSPI_DEVSIZECONFIG_MEMSIZEONCS1_MASK                     0x1800000UL                                           /**< Bit mask for QSPI_MEMSIZEONCS1 */
#define _QSPI_DEVSIZECONFIG_MEMSIZEONCS1_DEFAULT                  0x00000000UL                                          /**< Mode DEFAULT for QSPI_DEVSIZECONFIG */
#define QSPI_DEVSIZECONFIG_MEMSIZEONCS1_DEFAULT                   (_QSPI_DEVSIZECONFIG_MEMSIZEONCS1_DEFAULT << 23)      /**< Shifted mode DEFAULT for QSPI_DEVSIZECONFIG */

/* Bit fields for QSPI SRAMPARTITIONCFG */
#define _QSPI_SRAMPARTITIONCFG_RESETVALUE                         0x00000080UL                               /**< Default value for QSPI_SRAMPARTITIONCFG */
#define _QSPI_SRAMPARTITIONCFG_MASK                               0x000000FFUL                               /**< Mask for QSPI_SRAMPARTITIONCFG */
#define _QSPI_SRAMPARTITIONCFG_ADDR_SHIFT                         0                                          /**< Shift value for QSPI_ADDR */
#define _QSPI_SRAMPARTITIONCFG_ADDR_MASK                          0xFFUL                                     /**< Bit mask for QSPI_ADDR */
#define _QSPI_SRAMPARTITIONCFG_ADDR_DEFAULT                       0x00000080UL                               /**< Mode DEFAULT for QSPI_SRAMPARTITIONCFG */
#define QSPI_SRAMPARTITIONCFG_ADDR_DEFAULT                        (_QSPI_SRAMPARTITIONCFG_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_SRAMPARTITIONCFG */

/* Bit fields for QSPI INDAHBADDRTRIGGER */
#define _QSPI_INDAHBADDRTRIGGER_RESETVALUE                        0x00000000UL                                /**< Default value for QSPI_INDAHBADDRTRIGGER */
#define _QSPI_INDAHBADDRTRIGGER_MASK                              0xFFFFFFFFUL                                /**< Mask for QSPI_INDAHBADDRTRIGGER */
#define _QSPI_INDAHBADDRTRIGGER_ADDR_SHIFT                        0                                           /**< Shift value for QSPI_ADDR */
#define _QSPI_INDAHBADDRTRIGGER_ADDR_MASK                         0xFFFFFFFFUL                                /**< Bit mask for QSPI_ADDR */
#define _QSPI_INDAHBADDRTRIGGER_ADDR_DEFAULT                      0x00000000UL                                /**< Mode DEFAULT for QSPI_INDAHBADDRTRIGGER */
#define QSPI_INDAHBADDRTRIGGER_ADDR_DEFAULT                       (_QSPI_INDAHBADDRTRIGGER_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDAHBADDRTRIGGER */

/* Bit fields for QSPI REMAPADDR */
#define _QSPI_REMAPADDR_RESETVALUE                                0x00000000UL                         /**< Default value for QSPI_REMAPADDR */
#define _QSPI_REMAPADDR_MASK                                      0xFFFFFFFFUL                         /**< Mask for QSPI_REMAPADDR */
#define _QSPI_REMAPADDR_VALUE_SHIFT                               0                                    /**< Shift value for QSPI_VALUE */
#define _QSPI_REMAPADDR_VALUE_MASK                                0xFFFFFFFFUL                         /**< Bit mask for QSPI_VALUE */
#define _QSPI_REMAPADDR_VALUE_DEFAULT                             0x00000000UL                         /**< Mode DEFAULT for QSPI_REMAPADDR */
#define QSPI_REMAPADDR_VALUE_DEFAULT                              (_QSPI_REMAPADDR_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_REMAPADDR */

/* Bit fields for QSPI MODEBITCONFIG */
#define _QSPI_MODEBITCONFIG_RESETVALUE                            0x00000200UL                                     /**< Default value for QSPI_MODEBITCONFIG */
#define _QSPI_MODEBITCONFIG_MASK                                  0xFFFF87FFUL                                     /**< Mask for QSPI_MODEBITCONFIG */
#define _QSPI_MODEBITCONFIG_MODE_SHIFT                            0                                                /**< Shift value for QSPI_MODE */
#define _QSPI_MODEBITCONFIG_MODE_MASK                             0xFFUL                                           /**< Bit mask for QSPI_MODE */
#define _QSPI_MODEBITCONFIG_MODE_DEFAULT                          0x00000000UL                                     /**< Mode DEFAULT for QSPI_MODEBITCONFIG */
#define QSPI_MODEBITCONFIG_MODE_DEFAULT                           (_QSPI_MODEBITCONFIG_MODE_DEFAULT << 0)          /**< Shifted mode DEFAULT for QSPI_MODEBITCONFIG */
#define _QSPI_MODEBITCONFIG_CHUNKSIZE_SHIFT                       8                                                /**< Shift value for QSPI_CHUNKSIZE */
#define _QSPI_MODEBITCONFIG_CHUNKSIZE_MASK                        0x700UL                                          /**< Bit mask for QSPI_CHUNKSIZE */
#define _QSPI_MODEBITCONFIG_CHUNKSIZE_DEFAULT                     0x00000002UL                                     /**< Mode DEFAULT for QSPI_MODEBITCONFIG */
#define QSPI_MODEBITCONFIG_CHUNKSIZE_DEFAULT                      (_QSPI_MODEBITCONFIG_CHUNKSIZE_DEFAULT << 8)     /**< Shifted mode DEFAULT for QSPI_MODEBITCONFIG */
#define QSPI_MODEBITCONFIG_CRCOUTENABLE                           (0x1UL << 15)                                    /**< CRC# Output Enable Bit */
#define _QSPI_MODEBITCONFIG_CRCOUTENABLE_SHIFT                    15                                               /**< Shift value for QSPI_CRCOUTENABLE */
#define _QSPI_MODEBITCONFIG_CRCOUTENABLE_MASK                     0x8000UL                                         /**< Bit mask for QSPI_CRCOUTENABLE */
#define _QSPI_MODEBITCONFIG_CRCOUTENABLE_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for QSPI_MODEBITCONFIG */
#define QSPI_MODEBITCONFIG_CRCOUTENABLE_DEFAULT                   (_QSPI_MODEBITCONFIG_CRCOUTENABLE_DEFAULT << 15) /**< Shifted mode DEFAULT for QSPI_MODEBITCONFIG */
#define _QSPI_MODEBITCONFIG_RXCRCDATAUP_SHIFT                     16                                               /**< Shift value for QSPI_RXCRCDATAUP */
#define _QSPI_MODEBITCONFIG_RXCRCDATAUP_MASK                      0xFF0000UL                                       /**< Bit mask for QSPI_RXCRCDATAUP */
#define _QSPI_MODEBITCONFIG_RXCRCDATAUP_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for QSPI_MODEBITCONFIG */
#define QSPI_MODEBITCONFIG_RXCRCDATAUP_DEFAULT                    (_QSPI_MODEBITCONFIG_RXCRCDATAUP_DEFAULT << 16)  /**< Shifted mode DEFAULT for QSPI_MODEBITCONFIG */
#define _QSPI_MODEBITCONFIG_RXCRCDATALOW_SHIFT                    24                                               /**< Shift value for QSPI_RXCRCDATALOW */
#define _QSPI_MODEBITCONFIG_RXCRCDATALOW_MASK                     0xFF000000UL                                     /**< Bit mask for QSPI_RXCRCDATALOW */
#define _QSPI_MODEBITCONFIG_RXCRCDATALOW_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for QSPI_MODEBITCONFIG */
#define QSPI_MODEBITCONFIG_RXCRCDATALOW_DEFAULT                   (_QSPI_MODEBITCONFIG_RXCRCDATALOW_DEFAULT << 24) /**< Shifted mode DEFAULT for QSPI_MODEBITCONFIG */

/* Bit fields for QSPI SRAMFILL */
#define _QSPI_SRAMFILL_RESETVALUE                                 0x00000000UL                                      /**< Default value for QSPI_SRAMFILL */
#define _QSPI_SRAMFILL_MASK                                       0xFFFFFFFFUL                                      /**< Mask for QSPI_SRAMFILL */
#define _QSPI_SRAMFILL_SRAMFILLINDACREAD_SHIFT                    0                                                 /**< Shift value for QSPI_SRAMFILLINDACREAD */
#define _QSPI_SRAMFILL_SRAMFILLINDACREAD_MASK                     0xFFFFUL                                          /**< Bit mask for QSPI_SRAMFILLINDACREAD */
#define _QSPI_SRAMFILL_SRAMFILLINDACREAD_DEFAULT                  0x00000000UL                                      /**< Mode DEFAULT for QSPI_SRAMFILL */
#define QSPI_SRAMFILL_SRAMFILLINDACREAD_DEFAULT                   (_QSPI_SRAMFILL_SRAMFILLINDACREAD_DEFAULT << 0)   /**< Shifted mode DEFAULT for QSPI_SRAMFILL */
#define _QSPI_SRAMFILL_SRAMFILLINDACWRITE_SHIFT                   16                                                /**< Shift value for QSPI_SRAMFILLINDACWRITE */
#define _QSPI_SRAMFILL_SRAMFILLINDACWRITE_MASK                    0xFFFF0000UL                                      /**< Bit mask for QSPI_SRAMFILLINDACWRITE */
#define _QSPI_SRAMFILL_SRAMFILLINDACWRITE_DEFAULT                 0x00000000UL                                      /**< Mode DEFAULT for QSPI_SRAMFILL */
#define QSPI_SRAMFILL_SRAMFILLINDACWRITE_DEFAULT                  (_QSPI_SRAMFILL_SRAMFILLINDACWRITE_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_SRAMFILL */

/* Bit fields for QSPI TXTHRESH */
#define _QSPI_TXTHRESH_RESETVALUE                                 0x00000001UL                        /**< Default value for QSPI_TXTHRESH */
#define _QSPI_TXTHRESH_MASK                                       0x0000001FUL                        /**< Mask for QSPI_TXTHRESH */
#define _QSPI_TXTHRESH_LEVEL_SHIFT                                0                                   /**< Shift value for QSPI_LEVEL */
#define _QSPI_TXTHRESH_LEVEL_MASK                                 0x1FUL                              /**< Bit mask for QSPI_LEVEL */
#define _QSPI_TXTHRESH_LEVEL_DEFAULT                              0x00000001UL                        /**< Mode DEFAULT for QSPI_TXTHRESH */
#define QSPI_TXTHRESH_LEVEL_DEFAULT                               (_QSPI_TXTHRESH_LEVEL_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_TXTHRESH */

/* Bit fields for QSPI RXTHRESH */
#define _QSPI_RXTHRESH_RESETVALUE                                 0x00000001UL                        /**< Default value for QSPI_RXTHRESH */
#define _QSPI_RXTHRESH_MASK                                       0x0000001FUL                        /**< Mask for QSPI_RXTHRESH */
#define _QSPI_RXTHRESH_LEVEL_SHIFT                                0                                   /**< Shift value for QSPI_LEVEL */
#define _QSPI_RXTHRESH_LEVEL_MASK                                 0x1FUL                              /**< Bit mask for QSPI_LEVEL */
#define _QSPI_RXTHRESH_LEVEL_DEFAULT                              0x00000001UL                        /**< Mode DEFAULT for QSPI_RXTHRESH */
#define QSPI_RXTHRESH_LEVEL_DEFAULT                               (_QSPI_RXTHRESH_LEVEL_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_RXTHRESH */

/* Bit fields for QSPI WRITECOMPLETIONCTRL */
#define _QSPI_WRITECOMPLETIONCTRL_RESETVALUE                      0x00010005UL                                               /**< Default value for QSPI_WRITECOMPLETIONCTRL */
#define _QSPI_WRITECOMPLETIONCTRL_MASK                            0xFFFFE7FFUL                                               /**< Mask for QSPI_WRITECOMPLETIONCTRL */
#define _QSPI_WRITECOMPLETIONCTRL_OPCODE_SHIFT                    0                                                          /**< Shift value for QSPI_OPCODE */
#define _QSPI_WRITECOMPLETIONCTRL_OPCODE_MASK                     0xFFUL                                                     /**< Bit mask for QSPI_OPCODE */
#define _QSPI_WRITECOMPLETIONCTRL_OPCODE_DEFAULT                  0x00000005UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_OPCODE_DEFAULT                   (_QSPI_WRITECOMPLETIONCTRL_OPCODE_DEFAULT << 0)            /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define _QSPI_WRITECOMPLETIONCTRL_POLLINGBITINDEX_SHIFT           8                                                          /**< Shift value for QSPI_POLLINGBITINDEX */
#define _QSPI_WRITECOMPLETIONCTRL_POLLINGBITINDEX_MASK            0x700UL                                                    /**< Bit mask for QSPI_POLLINGBITINDEX */
#define _QSPI_WRITECOMPLETIONCTRL_POLLINGBITINDEX_DEFAULT         0x00000000UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_POLLINGBITINDEX_DEFAULT          (_QSPI_WRITECOMPLETIONCTRL_POLLINGBITINDEX_DEFAULT << 8)   /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_POLLINGPOLARITY                  (0x1UL << 13)                                              /**< Polling Polarity */
#define _QSPI_WRITECOMPLETIONCTRL_POLLINGPOLARITY_SHIFT           13                                                         /**< Shift value for QSPI_POLLINGPOLARITY */
#define _QSPI_WRITECOMPLETIONCTRL_POLLINGPOLARITY_MASK            0x2000UL                                                   /**< Bit mask for QSPI_POLLINGPOLARITY */
#define _QSPI_WRITECOMPLETIONCTRL_POLLINGPOLARITY_DEFAULT         0x00000000UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_POLLINGPOLARITY_DEFAULT          (_QSPI_WRITECOMPLETIONCTRL_POLLINGPOLARITY_DEFAULT << 13)  /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_DISABLEPOLLING                   (0x1UL << 14)                                              /**< Disable Polling */
#define _QSPI_WRITECOMPLETIONCTRL_DISABLEPOLLING_SHIFT            14                                                         /**< Shift value for QSPI_DISABLEPOLLING */
#define _QSPI_WRITECOMPLETIONCTRL_DISABLEPOLLING_MASK             0x4000UL                                                   /**< Bit mask for QSPI_DISABLEPOLLING */
#define _QSPI_WRITECOMPLETIONCTRL_DISABLEPOLLING_DEFAULT          0x00000000UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_DISABLEPOLLING_DEFAULT           (_QSPI_WRITECOMPLETIONCTRL_DISABLEPOLLING_DEFAULT << 14)   /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_ENABLEPOLLINGEXP                 (0x1UL << 15)                                              /**< Enable Polling Expiration */
#define _QSPI_WRITECOMPLETIONCTRL_ENABLEPOLLINGEXP_SHIFT          15                                                         /**< Shift value for QSPI_ENABLEPOLLINGEXP */
#define _QSPI_WRITECOMPLETIONCTRL_ENABLEPOLLINGEXP_MASK           0x8000UL                                                   /**< Bit mask for QSPI_ENABLEPOLLINGEXP */
#define _QSPI_WRITECOMPLETIONCTRL_ENABLEPOLLINGEXP_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_ENABLEPOLLINGEXP_DEFAULT         (_QSPI_WRITECOMPLETIONCTRL_ENABLEPOLLINGEXP_DEFAULT << 15) /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define _QSPI_WRITECOMPLETIONCTRL_POLLCOUNT_SHIFT                 16                                                         /**< Shift value for QSPI_POLLCOUNT */
#define _QSPI_WRITECOMPLETIONCTRL_POLLCOUNT_MASK                  0xFF0000UL                                                 /**< Bit mask for QSPI_POLLCOUNT */
#define _QSPI_WRITECOMPLETIONCTRL_POLLCOUNT_DEFAULT               0x00000001UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_POLLCOUNT_DEFAULT                (_QSPI_WRITECOMPLETIONCTRL_POLLCOUNT_DEFAULT << 16)        /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define _QSPI_WRITECOMPLETIONCTRL_POLLREPDELAY_SHIFT              24                                                         /**< Shift value for QSPI_POLLREPDELAY */
#define _QSPI_WRITECOMPLETIONCTRL_POLLREPDELAY_MASK               0xFF000000UL                                               /**< Bit mask for QSPI_POLLREPDELAY */
#define _QSPI_WRITECOMPLETIONCTRL_POLLREPDELAY_DEFAULT            0x00000000UL                                               /**< Mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */
#define QSPI_WRITECOMPLETIONCTRL_POLLREPDELAY_DEFAULT             (_QSPI_WRITECOMPLETIONCTRL_POLLREPDELAY_DEFAULT << 24)     /**< Shifted mode DEFAULT for QSPI_WRITECOMPLETIONCTRL */

/* Bit fields for QSPI NOOFPOLLSBEFEXP */
#define _QSPI_NOOFPOLLSBEFEXP_RESETVALUE                          0xFFFFFFFFUL                                         /**< Default value for QSPI_NOOFPOLLSBEFEXP */
#define _QSPI_NOOFPOLLSBEFEXP_MASK                                0xFFFFFFFFUL                                         /**< Mask for QSPI_NOOFPOLLSBEFEXP */
#define _QSPI_NOOFPOLLSBEFEXP_NOOFPOLLSBEFEXP_SHIFT               0                                                    /**< Shift value for QSPI_NOOFPOLLSBEFEXP */
#define _QSPI_NOOFPOLLSBEFEXP_NOOFPOLLSBEFEXP_MASK                0xFFFFFFFFUL                                         /**< Bit mask for QSPI_NOOFPOLLSBEFEXP */
#define _QSPI_NOOFPOLLSBEFEXP_NOOFPOLLSBEFEXP_DEFAULT             0xFFFFFFFFUL                                         /**< Mode DEFAULT for QSPI_NOOFPOLLSBEFEXP */
#define QSPI_NOOFPOLLSBEFEXP_NOOFPOLLSBEFEXP_DEFAULT              (_QSPI_NOOFPOLLSBEFEXP_NOOFPOLLSBEFEXP_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_NOOFPOLLSBEFEXP */

/* Bit fields for QSPI IRQSTATUS */
#define _QSPI_IRQSTATUS_RESETVALUE                                0x00000000UL                                           /**< Default value for QSPI_IRQSTATUS */
#define _QSPI_IRQSTATUS_MASK                                      0x00077FFFUL                                           /**< Mask for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_MODEMFAIL                                  (0x1UL << 0)                                           /**< Mode M Failure */
#define _QSPI_IRQSTATUS_MODEMFAIL_SHIFT                           0                                                      /**< Shift value for QSPI_MODEMFAIL */
#define _QSPI_IRQSTATUS_MODEMFAIL_MASK                            0x1UL                                                  /**< Bit mask for QSPI_MODEMFAIL */
#define _QSPI_IRQSTATUS_MODEMFAIL_DEFAULT                         0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_MODEMFAIL_DEFAULT                          (_QSPI_IRQSTATUS_MODEMFAIL_DEFAULT << 0)               /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_UNDERFLOWDET                               (0x1UL << 1)                                           /**< Underflow Detected */
#define _QSPI_IRQSTATUS_UNDERFLOWDET_SHIFT                        1                                                      /**< Shift value for QSPI_UNDERFLOWDET */
#define _QSPI_IRQSTATUS_UNDERFLOWDET_MASK                         0x2UL                                                  /**< Bit mask for QSPI_UNDERFLOWDET */
#define _QSPI_IRQSTATUS_UNDERFLOWDET_DEFAULT                      0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_UNDERFLOWDET_DEFAULT                       (_QSPI_IRQSTATUS_UNDERFLOWDET_DEFAULT << 1)            /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDIRECTOPDONE                             (0x1UL << 2)                                           /**< Indirect Operation Complete */
#define _QSPI_IRQSTATUS_INDIRECTOPDONE_SHIFT                      2                                                      /**< Shift value for QSPI_INDIRECTOPDONE */
#define _QSPI_IRQSTATUS_INDIRECTOPDONE_MASK                       0x4UL                                                  /**< Bit mask for QSPI_INDIRECTOPDONE */
#define _QSPI_IRQSTATUS_INDIRECTOPDONE_DEFAULT                    0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDIRECTOPDONE_DEFAULT                     (_QSPI_IRQSTATUS_INDIRECTOPDONE_DEFAULT << 2)          /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDIRECTREADREJECT                         (0x1UL << 3)                                           /**< Indirect Operation Was Requested but Could Not Be Accepted */
#define _QSPI_IRQSTATUS_INDIRECTREADREJECT_SHIFT                  3                                                      /**< Shift value for QSPI_INDIRECTREADREJECT */
#define _QSPI_IRQSTATUS_INDIRECTREADREJECT_MASK                   0x8UL                                                  /**< Bit mask for QSPI_INDIRECTREADREJECT */
#define _QSPI_IRQSTATUS_INDIRECTREADREJECT_DEFAULT                0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDIRECTREADREJECT_DEFAULT                 (_QSPI_IRQSTATUS_INDIRECTREADREJECT_DEFAULT << 3)      /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_PROTWRATTEMPT                              (0x1UL << 4)                                           /**< Write to Protected Area Was Attempted and Rejected */
#define _QSPI_IRQSTATUS_PROTWRATTEMPT_SHIFT                       4                                                      /**< Shift value for QSPI_PROTWRATTEMPT */
#define _QSPI_IRQSTATUS_PROTWRATTEMPT_MASK                        0x10UL                                                 /**< Bit mask for QSPI_PROTWRATTEMPT */
#define _QSPI_IRQSTATUS_PROTWRATTEMPT_DEFAULT                     0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_PROTWRATTEMPT_DEFAULT                      (_QSPI_IRQSTATUS_PROTWRATTEMPT_DEFAULT << 4)           /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_ILLEGALACCESSDET                           (0x1UL << 5)                                           /**< Illegal Memory Access Has Been Detected */
#define _QSPI_IRQSTATUS_ILLEGALACCESSDET_SHIFT                    5                                                      /**< Shift value for QSPI_ILLEGALACCESSDET */
#define _QSPI_IRQSTATUS_ILLEGALACCESSDET_MASK                     0x20UL                                                 /**< Bit mask for QSPI_ILLEGALACCESSDET */
#define _QSPI_IRQSTATUS_ILLEGALACCESSDET_DEFAULT                  0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_ILLEGALACCESSDET_DEFAULT                   (_QSPI_IRQSTATUS_ILLEGALACCESSDET_DEFAULT << 5)        /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH                    (0x1UL << 6)                                           /**< Indirect Transfer Watermark Level Breached */
#define _QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH_SHIFT             6                                                      /**< Shift value for QSPI_INDIRECTXFERLEVELBREACH */
#define _QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH_MASK              0x40UL                                                 /**< Bit mask for QSPI_INDIRECTXFERLEVELBREACH */
#define _QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH_DEFAULT           0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH_DEFAULT            (_QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH_DEFAULT << 6) /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RECVOVERFLOW                               (0x1UL << 7)                                           /**< Receive Overflow */
#define _QSPI_IRQSTATUS_RECVOVERFLOW_SHIFT                        7                                                      /**< Shift value for QSPI_RECVOVERFLOW */
#define _QSPI_IRQSTATUS_RECVOVERFLOW_MASK                         0x80UL                                                 /**< Bit mask for QSPI_RECVOVERFLOW */
#define _QSPI_IRQSTATUS_RECVOVERFLOW_DEFAULT                      0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RECVOVERFLOW_DEFAULT                       (_QSPI_IRQSTATUS_RECVOVERFLOW_DEFAULT << 7)            /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_TXFIFONOTFULL                              (0x1UL << 8)                                           /**< Small TX FIFO Not Full */
#define _QSPI_IRQSTATUS_TXFIFONOTFULL_SHIFT                       8                                                      /**< Shift value for QSPI_TXFIFONOTFULL */
#define _QSPI_IRQSTATUS_TXFIFONOTFULL_MASK                        0x100UL                                                /**< Bit mask for QSPI_TXFIFONOTFULL */
#define _QSPI_IRQSTATUS_TXFIFONOTFULL_DEFAULT                     0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_TXFIFONOTFULL_DEFAULT                      (_QSPI_IRQSTATUS_TXFIFONOTFULL_DEFAULT << 8)           /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_TXFIFOFULL                                 (0x1UL << 9)                                           /**< Small TX FIFO Full */
#define _QSPI_IRQSTATUS_TXFIFOFULL_SHIFT                          9                                                      /**< Shift value for QSPI_TXFIFOFULL */
#define _QSPI_IRQSTATUS_TXFIFOFULL_MASK                           0x200UL                                                /**< Bit mask for QSPI_TXFIFOFULL */
#define _QSPI_IRQSTATUS_TXFIFOFULL_DEFAULT                        0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_TXFIFOFULL_DEFAULT                         (_QSPI_IRQSTATUS_TXFIFOFULL_DEFAULT << 9)              /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXFIFONOTEMPTY                             (0x1UL << 10)                                          /**< Small RX FIFO Not Empty */
#define _QSPI_IRQSTATUS_RXFIFONOTEMPTY_SHIFT                      10                                                     /**< Shift value for QSPI_RXFIFONOTEMPTY */
#define _QSPI_IRQSTATUS_RXFIFONOTEMPTY_MASK                       0x400UL                                                /**< Bit mask for QSPI_RXFIFONOTEMPTY */
#define _QSPI_IRQSTATUS_RXFIFONOTEMPTY_DEFAULT                    0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXFIFONOTEMPTY_DEFAULT                     (_QSPI_IRQSTATUS_RXFIFONOTEMPTY_DEFAULT << 10)         /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXFIFOFULL                                 (0x1UL << 11)                                          /**< Small RX FIFO Full */
#define _QSPI_IRQSTATUS_RXFIFOFULL_SHIFT                          11                                                     /**< Shift value for QSPI_RXFIFOFULL */
#define _QSPI_IRQSTATUS_RXFIFOFULL_MASK                           0x800UL                                                /**< Bit mask for QSPI_RXFIFOFULL */
#define _QSPI_IRQSTATUS_RXFIFOFULL_DEFAULT                        0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXFIFOFULL_DEFAULT                         (_QSPI_IRQSTATUS_RXFIFOFULL_DEFAULT << 11)             /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDRDSRAMFULL                              (0x1UL << 12)                                          /**< Indirect Read Partition Overflow */
#define _QSPI_IRQSTATUS_INDRDSRAMFULL_SHIFT                       12                                                     /**< Shift value for QSPI_INDRDSRAMFULL */
#define _QSPI_IRQSTATUS_INDRDSRAMFULL_MASK                        0x1000UL                                               /**< Bit mask for QSPI_INDRDSRAMFULL */
#define _QSPI_IRQSTATUS_INDRDSRAMFULL_DEFAULT                     0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_INDRDSRAMFULL_DEFAULT                      (_QSPI_IRQSTATUS_INDRDSRAMFULL_DEFAULT << 12)          /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_POLLEXPINT                                 (0x1UL << 13)                                          /**< The Maximum Number of Programmed Polls Cycles is Expired */
#define _QSPI_IRQSTATUS_POLLEXPINT_SHIFT                          13                                                     /**< Shift value for QSPI_POLLEXPINT */
#define _QSPI_IRQSTATUS_POLLEXPINT_MASK                           0x2000UL                                               /**< Bit mask for QSPI_POLLEXPINT */
#define _QSPI_IRQSTATUS_POLLEXPINT_DEFAULT                        0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_POLLEXPINT_DEFAULT                         (_QSPI_IRQSTATUS_POLLEXPINT_DEFAULT << 13)             /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_STIGREQINT                                 (0x1UL << 14)                                          /**< The Controller is Ready for Getting Another STIG Request */
#define _QSPI_IRQSTATUS_STIGREQINT_SHIFT                          14                                                     /**< Shift value for QSPI_STIGREQINT */
#define _QSPI_IRQSTATUS_STIGREQINT_MASK                           0x4000UL                                               /**< Bit mask for QSPI_STIGREQINT */
#define _QSPI_IRQSTATUS_STIGREQINT_DEFAULT                        0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_STIGREQINT_DEFAULT                         (_QSPI_IRQSTATUS_STIGREQINT_DEFAULT << 14)             /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXCRCDATAERR                               (0x1UL << 16)                                          /**< RX CRC Data Error */
#define _QSPI_IRQSTATUS_RXCRCDATAERR_SHIFT                        16                                                     /**< Shift value for QSPI_RXCRCDATAERR */
#define _QSPI_IRQSTATUS_RXCRCDATAERR_MASK                         0x10000UL                                              /**< Bit mask for QSPI_RXCRCDATAERR */
#define _QSPI_IRQSTATUS_RXCRCDATAERR_DEFAULT                      0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXCRCDATAERR_DEFAULT                       (_QSPI_IRQSTATUS_RXCRCDATAERR_DEFAULT << 16)           /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXCRCDATAVAL                               (0x1UL << 17)                                          /**< RX CRC Data Valid */
#define _QSPI_IRQSTATUS_RXCRCDATAVAL_SHIFT                        17                                                     /**< Shift value for QSPI_RXCRCDATAVAL */
#define _QSPI_IRQSTATUS_RXCRCDATAVAL_MASK                         0x20000UL                                              /**< Bit mask for QSPI_RXCRCDATAVAL */
#define _QSPI_IRQSTATUS_RXCRCDATAVAL_DEFAULT                      0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_RXCRCDATAVAL_DEFAULT                       (_QSPI_IRQSTATUS_RXCRCDATAVAL_DEFAULT << 17)           /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_TXCRCCHUNKBRK                              (0x1UL << 18)                                          /**< TX CRC Chunk Was Broken */
#define _QSPI_IRQSTATUS_TXCRCCHUNKBRK_SHIFT                       18                                                     /**< Shift value for QSPI_TXCRCCHUNKBRK */
#define _QSPI_IRQSTATUS_TXCRCCHUNKBRK_MASK                        0x40000UL                                              /**< Bit mask for QSPI_TXCRCCHUNKBRK */
#define _QSPI_IRQSTATUS_TXCRCCHUNKBRK_DEFAULT                     0x00000000UL                                           /**< Mode DEFAULT for QSPI_IRQSTATUS */
#define QSPI_IRQSTATUS_TXCRCCHUNKBRK_DEFAULT                      (_QSPI_IRQSTATUS_TXCRCCHUNKBRK_DEFAULT << 18)          /**< Shifted mode DEFAULT for QSPI_IRQSTATUS */

/* Bit fields for QSPI IRQMASK */
#define _QSPI_IRQMASK_RESETVALUE                                  0x00000000UL                                             /**< Default value for QSPI_IRQMASK */
#define _QSPI_IRQMASK_MASK                                        0x00077FFFUL                                             /**< Mask for QSPI_IRQMASK */
#define QSPI_IRQMASK_MODEMFAILMASK                                (0x1UL << 0)                                             /**< Mode M Failure Mask */
#define _QSPI_IRQMASK_MODEMFAILMASK_SHIFT                         0                                                        /**< Shift value for QSPI_MODEMFAILMASK */
#define _QSPI_IRQMASK_MODEMFAILMASK_MASK                          0x1UL                                                    /**< Bit mask for QSPI_MODEMFAILMASK */
#define _QSPI_IRQMASK_MODEMFAILMASK_DEFAULT                       0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_MODEMFAILMASK_DEFAULT                        (_QSPI_IRQMASK_MODEMFAILMASK_DEFAULT << 0)               /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_UNDERFLOWDETMASK                             (0x1UL << 1)                                             /**< Underflow Detected Mask */
#define _QSPI_IRQMASK_UNDERFLOWDETMASK_SHIFT                      1                                                        /**< Shift value for QSPI_UNDERFLOWDETMASK */
#define _QSPI_IRQMASK_UNDERFLOWDETMASK_MASK                       0x2UL                                                    /**< Bit mask for QSPI_UNDERFLOWDETMASK */
#define _QSPI_IRQMASK_UNDERFLOWDETMASK_DEFAULT                    0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_UNDERFLOWDETMASK_DEFAULT                     (_QSPI_IRQMASK_UNDERFLOWDETMASK_DEFAULT << 1)            /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDIRECTOPDONEMASK                           (0x1UL << 2)                                             /**< Indirect Complete Mask */
#define _QSPI_IRQMASK_INDIRECTOPDONEMASK_SHIFT                    2                                                        /**< Shift value for QSPI_INDIRECTOPDONEMASK */
#define _QSPI_IRQMASK_INDIRECTOPDONEMASK_MASK                     0x4UL                                                    /**< Bit mask for QSPI_INDIRECTOPDONEMASK */
#define _QSPI_IRQMASK_INDIRECTOPDONEMASK_DEFAULT                  0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDIRECTOPDONEMASK_DEFAULT                   (_QSPI_IRQMASK_INDIRECTOPDONEMASK_DEFAULT << 2)          /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDIRECTREADREJECTMASK                       (0x1UL << 3)                                             /**< Indirect Read Reject Mask */
#define _QSPI_IRQMASK_INDIRECTREADREJECTMASK_SHIFT                3                                                        /**< Shift value for QSPI_INDIRECTREADREJECTMASK */
#define _QSPI_IRQMASK_INDIRECTREADREJECTMASK_MASK                 0x8UL                                                    /**< Bit mask for QSPI_INDIRECTREADREJECTMASK */
#define _QSPI_IRQMASK_INDIRECTREADREJECTMASK_DEFAULT              0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDIRECTREADREJECTMASK_DEFAULT               (_QSPI_IRQMASK_INDIRECTREADREJECTMASK_DEFAULT << 3)      /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_PROTWRATTEMPTMASK                            (0x1UL << 4)                                             /**< Protected Area Write Attempt Mask */
#define _QSPI_IRQMASK_PROTWRATTEMPTMASK_SHIFT                     4                                                        /**< Shift value for QSPI_PROTWRATTEMPTMASK */
#define _QSPI_IRQMASK_PROTWRATTEMPTMASK_MASK                      0x10UL                                                   /**< Bit mask for QSPI_PROTWRATTEMPTMASK */
#define _QSPI_IRQMASK_PROTWRATTEMPTMASK_DEFAULT                   0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_PROTWRATTEMPTMASK_DEFAULT                    (_QSPI_IRQMASK_PROTWRATTEMPTMASK_DEFAULT << 4)           /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_ILLEGALACCESSDETMASK                         (0x1UL << 5)                                             /**< Illegal Access Detected Mask */
#define _QSPI_IRQMASK_ILLEGALACCESSDETMASK_SHIFT                  5                                                        /**< Shift value for QSPI_ILLEGALACCESSDETMASK */
#define _QSPI_IRQMASK_ILLEGALACCESSDETMASK_MASK                   0x20UL                                                   /**< Bit mask for QSPI_ILLEGALACCESSDETMASK */
#define _QSPI_IRQMASK_ILLEGALACCESSDETMASK_DEFAULT                0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_ILLEGALACCESSDETMASK_DEFAULT                 (_QSPI_IRQMASK_ILLEGALACCESSDETMASK_DEFAULT << 5)        /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK                  (0x1UL << 6)                                             /**< Transfer Watermark Breach Mask */
#define _QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK_SHIFT           6                                                        /**< Shift value for QSPI_INDIRECTXFERLEVELBREACHMASK */
#define _QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK_MASK            0x40UL                                                   /**< Bit mask for QSPI_INDIRECTXFERLEVELBREACHMASK */
#define _QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK_DEFAULT         0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK_DEFAULT          (_QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK_DEFAULT << 6) /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RECVOVERFLOWMASK                             (0x1UL << 7)                                             /**< Receive Overflow Mask */
#define _QSPI_IRQMASK_RECVOVERFLOWMASK_SHIFT                      7                                                        /**< Shift value for QSPI_RECVOVERFLOWMASK */
#define _QSPI_IRQMASK_RECVOVERFLOWMASK_MASK                       0x80UL                                                   /**< Bit mask for QSPI_RECVOVERFLOWMASK */
#define _QSPI_IRQMASK_RECVOVERFLOWMASK_DEFAULT                    0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RECVOVERFLOWMASK_DEFAULT                     (_QSPI_IRQMASK_RECVOVERFLOWMASK_DEFAULT << 7)            /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_TXFIFONOTFULLMASK                            (0x1UL << 8)                                             /**< Small TX FIFO Not Full Mask */
#define _QSPI_IRQMASK_TXFIFONOTFULLMASK_SHIFT                     8                                                        /**< Shift value for QSPI_TXFIFONOTFULLMASK */
#define _QSPI_IRQMASK_TXFIFONOTFULLMASK_MASK                      0x100UL                                                  /**< Bit mask for QSPI_TXFIFONOTFULLMASK */
#define _QSPI_IRQMASK_TXFIFONOTFULLMASK_DEFAULT                   0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_TXFIFONOTFULLMASK_DEFAULT                    (_QSPI_IRQMASK_TXFIFONOTFULLMASK_DEFAULT << 8)           /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_TXFIFOFULLMASK                               (0x1UL << 9)                                             /**< Small TX FIFO Full Mask */
#define _QSPI_IRQMASK_TXFIFOFULLMASK_SHIFT                        9                                                        /**< Shift value for QSPI_TXFIFOFULLMASK */
#define _QSPI_IRQMASK_TXFIFOFULLMASK_MASK                         0x200UL                                                  /**< Bit mask for QSPI_TXFIFOFULLMASK */
#define _QSPI_IRQMASK_TXFIFOFULLMASK_DEFAULT                      0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_TXFIFOFULLMASK_DEFAULT                       (_QSPI_IRQMASK_TXFIFOFULLMASK_DEFAULT << 9)              /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXFIFONOTEMPTYMASK                           (0x1UL << 10)                                            /**< Small RX FIFO Not Empty Mask */
#define _QSPI_IRQMASK_RXFIFONOTEMPTYMASK_SHIFT                    10                                                       /**< Shift value for QSPI_RXFIFONOTEMPTYMASK */
#define _QSPI_IRQMASK_RXFIFONOTEMPTYMASK_MASK                     0x400UL                                                  /**< Bit mask for QSPI_RXFIFONOTEMPTYMASK */
#define _QSPI_IRQMASK_RXFIFONOTEMPTYMASK_DEFAULT                  0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXFIFONOTEMPTYMASK_DEFAULT                   (_QSPI_IRQMASK_RXFIFONOTEMPTYMASK_DEFAULT << 10)         /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXFIFOFULLMASK                               (0x1UL << 11)                                            /**< Small RX FIFO Full Mask */
#define _QSPI_IRQMASK_RXFIFOFULLMASK_SHIFT                        11                                                       /**< Shift value for QSPI_RXFIFOFULLMASK */
#define _QSPI_IRQMASK_RXFIFOFULLMASK_MASK                         0x800UL                                                  /**< Bit mask for QSPI_RXFIFOFULLMASK */
#define _QSPI_IRQMASK_RXFIFOFULLMASK_DEFAULT                      0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXFIFOFULLMASK_DEFAULT                       (_QSPI_IRQMASK_RXFIFOFULLMASK_DEFAULT << 11)             /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDRDSRAMFULLMASK                            (0x1UL << 12)                                            /**< Indirect Read Partition Overflow Mask */
#define _QSPI_IRQMASK_INDRDSRAMFULLMASK_SHIFT                     12                                                       /**< Shift value for QSPI_INDRDSRAMFULLMASK */
#define _QSPI_IRQMASK_INDRDSRAMFULLMASK_MASK                      0x1000UL                                                 /**< Bit mask for QSPI_INDRDSRAMFULLMASK */
#define _QSPI_IRQMASK_INDRDSRAMFULLMASK_DEFAULT                   0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_INDRDSRAMFULLMASK_DEFAULT                    (_QSPI_IRQMASK_INDRDSRAMFULLMASK_DEFAULT << 12)          /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_POLLEXPINTMASK                               (0x1UL << 13)                                            /**< Polling Expiration Detected Mask */
#define _QSPI_IRQMASK_POLLEXPINTMASK_SHIFT                        13                                                       /**< Shift value for QSPI_POLLEXPINTMASK */
#define _QSPI_IRQMASK_POLLEXPINTMASK_MASK                         0x2000UL                                                 /**< Bit mask for QSPI_POLLEXPINTMASK */
#define _QSPI_IRQMASK_POLLEXPINTMASK_DEFAULT                      0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_POLLEXPINTMASK_DEFAULT                       (_QSPI_IRQMASK_POLLEXPINTMASK_DEFAULT << 13)             /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_STIGREQMASK                                  (0x1UL << 14)                                            /**< STIG Request Completion Mask */
#define _QSPI_IRQMASK_STIGREQMASK_SHIFT                           14                                                       /**< Shift value for QSPI_STIGREQMASK */
#define _QSPI_IRQMASK_STIGREQMASK_MASK                            0x4000UL                                                 /**< Bit mask for QSPI_STIGREQMASK */
#define _QSPI_IRQMASK_STIGREQMASK_DEFAULT                         0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_STIGREQMASK_DEFAULT                          (_QSPI_IRQMASK_STIGREQMASK_DEFAULT << 14)                /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXCRCDATAERRMASK                             (0x1UL << 16)                                            /**< RX CRC Data Error Mask */
#define _QSPI_IRQMASK_RXCRCDATAERRMASK_SHIFT                      16                                                       /**< Shift value for QSPI_RXCRCDATAERRMASK */
#define _QSPI_IRQMASK_RXCRCDATAERRMASK_MASK                       0x10000UL                                                /**< Bit mask for QSPI_RXCRCDATAERRMASK */
#define _QSPI_IRQMASK_RXCRCDATAERRMASK_DEFAULT                    0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXCRCDATAERRMASK_DEFAULT                     (_QSPI_IRQMASK_RXCRCDATAERRMASK_DEFAULT << 16)           /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXCRCDATAVALMASK                             (0x1UL << 17)                                            /**< RX CRC Data Valid Mask */
#define _QSPI_IRQMASK_RXCRCDATAVALMASK_SHIFT                      17                                                       /**< Shift value for QSPI_RXCRCDATAVALMASK */
#define _QSPI_IRQMASK_RXCRCDATAVALMASK_MASK                       0x20000UL                                                /**< Bit mask for QSPI_RXCRCDATAVALMASK */
#define _QSPI_IRQMASK_RXCRCDATAVALMASK_DEFAULT                    0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_RXCRCDATAVALMASK_DEFAULT                     (_QSPI_IRQMASK_RXCRCDATAVALMASK_DEFAULT << 17)           /**< Shifted mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_TXCRCCHUNKBRKMASK                            (0x1UL << 18)                                            /**< TX CRC Chunk Was Broken Mask */
#define _QSPI_IRQMASK_TXCRCCHUNKBRKMASK_SHIFT                     18                                                       /**< Shift value for QSPI_TXCRCCHUNKBRKMASK */
#define _QSPI_IRQMASK_TXCRCCHUNKBRKMASK_MASK                      0x40000UL                                                /**< Bit mask for QSPI_TXCRCCHUNKBRKMASK */
#define _QSPI_IRQMASK_TXCRCCHUNKBRKMASK_DEFAULT                   0x00000000UL                                             /**< Mode DEFAULT for QSPI_IRQMASK */
#define QSPI_IRQMASK_TXCRCCHUNKBRKMASK_DEFAULT                    (_QSPI_IRQMASK_TXCRCCHUNKBRKMASK_DEFAULT << 18)          /**< Shifted mode DEFAULT for QSPI_IRQMASK */

/* Bit fields for QSPI LOWERWRPROT */
#define _QSPI_LOWERWRPROT_RESETVALUE                              0x00000000UL                               /**< Default value for QSPI_LOWERWRPROT */
#define _QSPI_LOWERWRPROT_MASK                                    0xFFFFFFFFUL                               /**< Mask for QSPI_LOWERWRPROT */
#define _QSPI_LOWERWRPROT_SUBSECTOR_SHIFT                         0                                          /**< Shift value for QSPI_SUBSECTOR */
#define _QSPI_LOWERWRPROT_SUBSECTOR_MASK                          0xFFFFFFFFUL                               /**< Bit mask for QSPI_SUBSECTOR */
#define _QSPI_LOWERWRPROT_SUBSECTOR_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for QSPI_LOWERWRPROT */
#define QSPI_LOWERWRPROT_SUBSECTOR_DEFAULT                        (_QSPI_LOWERWRPROT_SUBSECTOR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_LOWERWRPROT */

/* Bit fields for QSPI UPPERWRPROT */
#define _QSPI_UPPERWRPROT_RESETVALUE                              0x00000000UL                               /**< Default value for QSPI_UPPERWRPROT */
#define _QSPI_UPPERWRPROT_MASK                                    0xFFFFFFFFUL                               /**< Mask for QSPI_UPPERWRPROT */
#define _QSPI_UPPERWRPROT_SUBSECTOR_SHIFT                         0                                          /**< Shift value for QSPI_SUBSECTOR */
#define _QSPI_UPPERWRPROT_SUBSECTOR_MASK                          0xFFFFFFFFUL                               /**< Bit mask for QSPI_SUBSECTOR */
#define _QSPI_UPPERWRPROT_SUBSECTOR_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for QSPI_UPPERWRPROT */
#define QSPI_UPPERWRPROT_SUBSECTOR_DEFAULT                        (_QSPI_UPPERWRPROT_SUBSECTOR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_UPPERWRPROT */

/* Bit fields for QSPI WRPROTCTRL */
#define _QSPI_WRPROTCTRL_RESETVALUE                               0x00000000UL                        /**< Default value for QSPI_WRPROTCTRL */
#define _QSPI_WRPROTCTRL_MASK                                     0x00000003UL                        /**< Mask for QSPI_WRPROTCTRL */
#define QSPI_WRPROTCTRL_INV                                       (0x1UL << 0)                        /**< Write Protection Inversion Bit */
#define _QSPI_WRPROTCTRL_INV_SHIFT                                0                                   /**< Shift value for QSPI_INV */
#define _QSPI_WRPROTCTRL_INV_MASK                                 0x1UL                               /**< Bit mask for QSPI_INV */
#define _QSPI_WRPROTCTRL_INV_DEFAULT                              0x00000000UL                        /**< Mode DEFAULT for QSPI_WRPROTCTRL */
#define QSPI_WRPROTCTRL_INV_DEFAULT                               (_QSPI_WRPROTCTRL_INV_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_WRPROTCTRL */
#define QSPI_WRPROTCTRL_ENB                                       (0x1UL << 1)                        /**< Write Protection Enable Bit */
#define _QSPI_WRPROTCTRL_ENB_SHIFT                                1                                   /**< Shift value for QSPI_ENB */
#define _QSPI_WRPROTCTRL_ENB_MASK                                 0x2UL                               /**< Bit mask for QSPI_ENB */
#define _QSPI_WRPROTCTRL_ENB_DEFAULT                              0x00000000UL                        /**< Mode DEFAULT for QSPI_WRPROTCTRL */
#define QSPI_WRPROTCTRL_ENB_DEFAULT                               (_QSPI_WRPROTCTRL_ENB_DEFAULT << 1) /**< Shifted mode DEFAULT for QSPI_WRPROTCTRL */

/* Bit fields for QSPI INDIRECTREADXFERCTRL */
#define _QSPI_INDIRECTREADXFERCTRL_RESETVALUE                     0x00000000UL                                               /**< Default value for QSPI_INDIRECTREADXFERCTRL */
#define _QSPI_INDIRECTREADXFERCTRL_MASK                           0x000000FFUL                                               /**< Mask for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_START                           (0x1UL << 0)                                               /**< Start Indirect Read */
#define _QSPI_INDIRECTREADXFERCTRL_START_SHIFT                    0                                                          /**< Shift value for QSPI_START */
#define _QSPI_INDIRECTREADXFERCTRL_START_MASK                     0x1UL                                                      /**< Bit mask for QSPI_START */
#define _QSPI_INDIRECTREADXFERCTRL_START_DEFAULT                  0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_START_DEFAULT                   (_QSPI_INDIRECTREADXFERCTRL_START_DEFAULT << 0)            /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_CANCEL                          (0x1UL << 1)                                               /**< Cancel Indirect Read */
#define _QSPI_INDIRECTREADXFERCTRL_CANCEL_SHIFT                   1                                                          /**< Shift value for QSPI_CANCEL */
#define _QSPI_INDIRECTREADXFERCTRL_CANCEL_MASK                    0x2UL                                                      /**< Bit mask for QSPI_CANCEL */
#define _QSPI_INDIRECTREADXFERCTRL_CANCEL_DEFAULT                 0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_CANCEL_DEFAULT                  (_QSPI_INDIRECTREADXFERCTRL_CANCEL_DEFAULT << 1)           /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_RDSTATUS                        (0x1UL << 2)                                               /**< Indirect Read Status */
#define _QSPI_INDIRECTREADXFERCTRL_RDSTATUS_SHIFT                 2                                                          /**< Shift value for QSPI_RDSTATUS */
#define _QSPI_INDIRECTREADXFERCTRL_RDSTATUS_MASK                  0x4UL                                                      /**< Bit mask for QSPI_RDSTATUS */
#define _QSPI_INDIRECTREADXFERCTRL_RDSTATUS_DEFAULT               0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_RDSTATUS_DEFAULT                (_QSPI_INDIRECTREADXFERCTRL_RDSTATUS_DEFAULT << 2)         /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_SRAMFULL                        (0x1UL << 3)                                               /**< SRAM Full */
#define _QSPI_INDIRECTREADXFERCTRL_SRAMFULL_SHIFT                 3                                                          /**< Shift value for QSPI_SRAMFULL */
#define _QSPI_INDIRECTREADXFERCTRL_SRAMFULL_MASK                  0x8UL                                                      /**< Bit mask for QSPI_SRAMFULL */
#define _QSPI_INDIRECTREADXFERCTRL_SRAMFULL_DEFAULT               0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_SRAMFULL_DEFAULT                (_QSPI_INDIRECTREADXFERCTRL_SRAMFULL_DEFAULT << 3)         /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_RDQUEUED                        (0x1UL << 4)                                               /**< Two Indirect Read Operations Have Been Queued */
#define _QSPI_INDIRECTREADXFERCTRL_RDQUEUED_SHIFT                 4                                                          /**< Shift value for QSPI_RDQUEUED */
#define _QSPI_INDIRECTREADXFERCTRL_RDQUEUED_MASK                  0x10UL                                                     /**< Bit mask for QSPI_RDQUEUED */
#define _QSPI_INDIRECTREADXFERCTRL_RDQUEUED_DEFAULT               0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_RDQUEUED_DEFAULT                (_QSPI_INDIRECTREADXFERCTRL_RDQUEUED_DEFAULT << 4)         /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS                (0x1UL << 5)                                               /**< Indirect Completion Status */
#define _QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS_SHIFT         5                                                          /**< Shift value for QSPI_INDOPSDONESTATUS */
#define _QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS_MASK          0x20UL                                                     /**< Bit mask for QSPI_INDOPSDONESTATUS */
#define _QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS_DEFAULT       0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS_DEFAULT        (_QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS_DEFAULT << 5) /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define _QSPI_INDIRECTREADXFERCTRL_NUMINDOPSDONE_SHIFT            6                                                          /**< Shift value for QSPI_NUMINDOPSDONE */
#define _QSPI_INDIRECTREADXFERCTRL_NUMINDOPSDONE_MASK             0xC0UL                                                     /**< Bit mask for QSPI_NUMINDOPSDONE */
#define _QSPI_INDIRECTREADXFERCTRL_NUMINDOPSDONE_DEFAULT          0x00000000UL                                               /**< Mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */
#define QSPI_INDIRECTREADXFERCTRL_NUMINDOPSDONE_DEFAULT           (_QSPI_INDIRECTREADXFERCTRL_NUMINDOPSDONE_DEFAULT << 6)    /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERCTRL */

/* Bit fields for QSPI INDIRECTREADXFERWATERMARK */
#define _QSPI_INDIRECTREADXFERWATERMARK_RESETVALUE                0x00000000UL                                         /**< Default value for QSPI_INDIRECTREADXFERWATERMARK */
#define _QSPI_INDIRECTREADXFERWATERMARK_MASK                      0xFFFFFFFFUL                                         /**< Mask for QSPI_INDIRECTREADXFERWATERMARK */
#define _QSPI_INDIRECTREADXFERWATERMARK_LEVEL_SHIFT               0                                                    /**< Shift value for QSPI_LEVEL */
#define _QSPI_INDIRECTREADXFERWATERMARK_LEVEL_MASK                0xFFFFFFFFUL                                         /**< Bit mask for QSPI_LEVEL */
#define _QSPI_INDIRECTREADXFERWATERMARK_LEVEL_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for QSPI_INDIRECTREADXFERWATERMARK */
#define QSPI_INDIRECTREADXFERWATERMARK_LEVEL_DEFAULT              (_QSPI_INDIRECTREADXFERWATERMARK_LEVEL_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERWATERMARK */

/* Bit fields for QSPI INDIRECTREADXFERSTART */
#define _QSPI_INDIRECTREADXFERSTART_RESETVALUE                    0x00000000UL                                    /**< Default value for QSPI_INDIRECTREADXFERSTART */
#define _QSPI_INDIRECTREADXFERSTART_MASK                          0xFFFFFFFFUL                                    /**< Mask for QSPI_INDIRECTREADXFERSTART */
#define _QSPI_INDIRECTREADXFERSTART_ADDR_SHIFT                    0                                               /**< Shift value for QSPI_ADDR */
#define _QSPI_INDIRECTREADXFERSTART_ADDR_MASK                     0xFFFFFFFFUL                                    /**< Bit mask for QSPI_ADDR */
#define _QSPI_INDIRECTREADXFERSTART_ADDR_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for QSPI_INDIRECTREADXFERSTART */
#define QSPI_INDIRECTREADXFERSTART_ADDR_DEFAULT                   (_QSPI_INDIRECTREADXFERSTART_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERSTART */

/* Bit fields for QSPI INDIRECTREADXFERNUMBYTES */
#define _QSPI_INDIRECTREADXFERNUMBYTES_RESETVALUE                 0x00000000UL                                        /**< Default value for QSPI_INDIRECTREADXFERNUMBYTES */
#define _QSPI_INDIRECTREADXFERNUMBYTES_MASK                       0xFFFFFFFFUL                                        /**< Mask for QSPI_INDIRECTREADXFERNUMBYTES */
#define _QSPI_INDIRECTREADXFERNUMBYTES_VALUE_SHIFT                0                                                   /**< Shift value for QSPI_VALUE */
#define _QSPI_INDIRECTREADXFERNUMBYTES_VALUE_MASK                 0xFFFFFFFFUL                                        /**< Bit mask for QSPI_VALUE */
#define _QSPI_INDIRECTREADXFERNUMBYTES_VALUE_DEFAULT              0x00000000UL                                        /**< Mode DEFAULT for QSPI_INDIRECTREADXFERNUMBYTES */
#define QSPI_INDIRECTREADXFERNUMBYTES_VALUE_DEFAULT               (_QSPI_INDIRECTREADXFERNUMBYTES_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTREADXFERNUMBYTES */

/* Bit fields for QSPI INDIRECTWRITEXFERCTRL */
#define _QSPI_INDIRECTWRITEXFERCTRL_RESETVALUE                    0x00000000UL                                                /**< Default value for QSPI_INDIRECTWRITEXFERCTRL */
#define _QSPI_INDIRECTWRITEXFERCTRL_MASK                          0x000000F7UL                                                /**< Mask for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_START                          (0x1UL << 0)                                                /**< Start Indirect Write */
#define _QSPI_INDIRECTWRITEXFERCTRL_START_SHIFT                   0                                                           /**< Shift value for QSPI_START */
#define _QSPI_INDIRECTWRITEXFERCTRL_START_MASK                    0x1UL                                                       /**< Bit mask for QSPI_START */
#define _QSPI_INDIRECTWRITEXFERCTRL_START_DEFAULT                 0x00000000UL                                                /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_START_DEFAULT                  (_QSPI_INDIRECTWRITEXFERCTRL_START_DEFAULT << 0)            /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_CANCEL                         (0x1UL << 1)                                                /**< Cancel Indirect Write */
#define _QSPI_INDIRECTWRITEXFERCTRL_CANCEL_SHIFT                  1                                                           /**< Shift value for QSPI_CANCEL */
#define _QSPI_INDIRECTWRITEXFERCTRL_CANCEL_MASK                   0x2UL                                                       /**< Bit mask for QSPI_CANCEL */
#define _QSPI_INDIRECTWRITEXFERCTRL_CANCEL_DEFAULT                0x00000000UL                                                /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_CANCEL_DEFAULT                 (_QSPI_INDIRECTWRITEXFERCTRL_CANCEL_DEFAULT << 1)           /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS                       (0x1UL << 2)                                                /**< Indirect Write Status */
#define _QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS_SHIFT                2                                                           /**< Shift value for QSPI_WRSTATUS */
#define _QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS_MASK                 0x4UL                                                       /**< Bit mask for QSPI_WRSTATUS */
#define _QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS_DEFAULT              0x00000000UL                                                /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS_DEFAULT               (_QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS_DEFAULT << 2)         /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_WRQUEUED                       (0x1UL << 4)                                                /**< Two Indirect Write Operations Have Been Queued */
#define _QSPI_INDIRECTWRITEXFERCTRL_WRQUEUED_SHIFT                4                                                           /**< Shift value for QSPI_WRQUEUED */
#define _QSPI_INDIRECTWRITEXFERCTRL_WRQUEUED_MASK                 0x10UL                                                      /**< Bit mask for QSPI_WRQUEUED */
#define _QSPI_INDIRECTWRITEXFERCTRL_WRQUEUED_DEFAULT              0x00000000UL                                                /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_WRQUEUED_DEFAULT               (_QSPI_INDIRECTWRITEXFERCTRL_WRQUEUED_DEFAULT << 4)         /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS               (0x1UL << 5)                                                /**< Indirect Completion Status */
#define _QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS_SHIFT        5                                                           /**< Shift value for QSPI_INDOPSDONESTATUS */
#define _QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS_MASK         0x20UL                                                      /**< Bit mask for QSPI_INDOPSDONESTATUS */
#define _QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS_DEFAULT      0x00000000UL                                                /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS_DEFAULT       (_QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS_DEFAULT << 5) /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define _QSPI_INDIRECTWRITEXFERCTRL_NUMINDOPSDONE_SHIFT           6                                                           /**< Shift value for QSPI_NUMINDOPSDONE */
#define _QSPI_INDIRECTWRITEXFERCTRL_NUMINDOPSDONE_MASK            0xC0UL                                                      /**< Bit mask for QSPI_NUMINDOPSDONE */
#define _QSPI_INDIRECTWRITEXFERCTRL_NUMINDOPSDONE_DEFAULT         0x00000000UL                                                /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */
#define QSPI_INDIRECTWRITEXFERCTRL_NUMINDOPSDONE_DEFAULT          (_QSPI_INDIRECTWRITEXFERCTRL_NUMINDOPSDONE_DEFAULT << 6)    /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERCTRL */

/* Bit fields for QSPI INDIRECTWRITEXFERWATERMARK */
#define _QSPI_INDIRECTWRITEXFERWATERMARK_RESETVALUE               0xFFFFFFFFUL                                          /**< Default value for QSPI_INDIRECTWRITEXFERWATERMARK */
#define _QSPI_INDIRECTWRITEXFERWATERMARK_MASK                     0xFFFFFFFFUL                                          /**< Mask for QSPI_INDIRECTWRITEXFERWATERMARK */
#define _QSPI_INDIRECTWRITEXFERWATERMARK_LEVEL_SHIFT              0                                                     /**< Shift value for QSPI_LEVEL */
#define _QSPI_INDIRECTWRITEXFERWATERMARK_LEVEL_MASK               0xFFFFFFFFUL                                          /**< Bit mask for QSPI_LEVEL */
#define _QSPI_INDIRECTWRITEXFERWATERMARK_LEVEL_DEFAULT            0xFFFFFFFFUL                                          /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERWATERMARK */
#define QSPI_INDIRECTWRITEXFERWATERMARK_LEVEL_DEFAULT             (_QSPI_INDIRECTWRITEXFERWATERMARK_LEVEL_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERWATERMARK */

/* Bit fields for QSPI INDIRECTWRITEXFERSTART */
#define _QSPI_INDIRECTWRITEXFERSTART_RESETVALUE                   0x00000000UL                                     /**< Default value for QSPI_INDIRECTWRITEXFERSTART */
#define _QSPI_INDIRECTWRITEXFERSTART_MASK                         0xFFFFFFFFUL                                     /**< Mask for QSPI_INDIRECTWRITEXFERSTART */
#define _QSPI_INDIRECTWRITEXFERSTART_ADDR_SHIFT                   0                                                /**< Shift value for QSPI_ADDR */
#define _QSPI_INDIRECTWRITEXFERSTART_ADDR_MASK                    0xFFFFFFFFUL                                     /**< Bit mask for QSPI_ADDR */
#define _QSPI_INDIRECTWRITEXFERSTART_ADDR_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERSTART */
#define QSPI_INDIRECTWRITEXFERSTART_ADDR_DEFAULT                  (_QSPI_INDIRECTWRITEXFERSTART_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERSTART */

/* Bit fields for QSPI INDIRECTWRITEXFERNUMBYTES */
#define _QSPI_INDIRECTWRITEXFERNUMBYTES_RESETVALUE                0x00000000UL                                         /**< Default value for QSPI_INDIRECTWRITEXFERNUMBYTES */
#define _QSPI_INDIRECTWRITEXFERNUMBYTES_MASK                      0xFFFFFFFFUL                                         /**< Mask for QSPI_INDIRECTWRITEXFERNUMBYTES */
#define _QSPI_INDIRECTWRITEXFERNUMBYTES_VALUE_SHIFT               0                                                    /**< Shift value for QSPI_VALUE */
#define _QSPI_INDIRECTWRITEXFERNUMBYTES_VALUE_MASK                0xFFFFFFFFUL                                         /**< Bit mask for QSPI_VALUE */
#define _QSPI_INDIRECTWRITEXFERNUMBYTES_VALUE_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for QSPI_INDIRECTWRITEXFERNUMBYTES */
#define QSPI_INDIRECTWRITEXFERNUMBYTES_VALUE_DEFAULT              (_QSPI_INDIRECTWRITEXFERNUMBYTES_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTWRITEXFERNUMBYTES */

/* Bit fields for QSPI INDIRECTTRIGGERADDRRANGE */
#define _QSPI_INDIRECTTRIGGERADDRRANGE_RESETVALUE                 0x00000004UL                                                /**< Default value for QSPI_INDIRECTTRIGGERADDRRANGE */
#define _QSPI_INDIRECTTRIGGERADDRRANGE_MASK                       0x0000000FUL                                                /**< Mask for QSPI_INDIRECTTRIGGERADDRRANGE */
#define _QSPI_INDIRECTTRIGGERADDRRANGE_INDRANGEWIDTH_SHIFT        0                                                           /**< Shift value for QSPI_INDRANGEWIDTH */
#define _QSPI_INDIRECTTRIGGERADDRRANGE_INDRANGEWIDTH_MASK         0xFUL                                                       /**< Bit mask for QSPI_INDRANGEWIDTH */
#define _QSPI_INDIRECTTRIGGERADDRRANGE_INDRANGEWIDTH_DEFAULT      0x00000004UL                                                /**< Mode DEFAULT for QSPI_INDIRECTTRIGGERADDRRANGE */
#define QSPI_INDIRECTTRIGGERADDRRANGE_INDRANGEWIDTH_DEFAULT       (_QSPI_INDIRECTTRIGGERADDRRANGE_INDRANGEWIDTH_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_INDIRECTTRIGGERADDRRANGE */

/* Bit fields for QSPI FLASHCOMMANDCTRLMEM */
#define _QSPI_FLASHCOMMANDCTRLMEM_RESETVALUE                      0x00000000UL                                                  /**< Default value for QSPI_FLASHCOMMANDCTRLMEM */
#define _QSPI_FLASHCOMMANDCTRLMEM_MASK                            0x1FF7FF03UL                                                  /**< Mask for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_TRIGGERMEMBANKREQ                (0x1UL << 0)                                                  /**< Trigger the Memory Bank Data Request */
#define _QSPI_FLASHCOMMANDCTRLMEM_TRIGGERMEMBANKREQ_SHIFT         0                                                             /**< Shift value for QSPI_TRIGGERMEMBANKREQ */
#define _QSPI_FLASHCOMMANDCTRLMEM_TRIGGERMEMBANKREQ_MASK          0x1UL                                                         /**< Bit mask for QSPI_TRIGGERMEMBANKREQ */
#define _QSPI_FLASHCOMMANDCTRLMEM_TRIGGERMEMBANKREQ_DEFAULT       0x00000000UL                                                  /**< Mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_TRIGGERMEMBANKREQ_DEFAULT        (_QSPI_FLASHCOMMANDCTRLMEM_TRIGGERMEMBANKREQ_DEFAULT << 0)    /**< Shifted mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREQINPROGRESS             (0x1UL << 1)                                                  /**< Memory Bank Data Request in Progress */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREQINPROGRESS_SHIFT      1                                                             /**< Shift value for QSPI_MEMBANKREQINPROGRESS */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREQINPROGRESS_MASK       0x2UL                                                         /**< Bit mask for QSPI_MEMBANKREQINPROGRESS */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREQINPROGRESS_DEFAULT    0x00000000UL                                                  /**< Mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREQINPROGRESS_DEFAULT     (_QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREQINPROGRESS_DEFAULT << 1) /**< Shifted mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREADDATA_SHIFT           8                                                             /**< Shift value for QSPI_MEMBANKREADDATA */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREADDATA_MASK            0xFF00UL                                                      /**< Bit mask for QSPI_MEMBANKREADDATA */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREADDATA_DEFAULT         0x00000000UL                                                  /**< Mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREADDATA_DEFAULT          (_QSPI_FLASHCOMMANDCTRLMEM_MEMBANKREADDATA_DEFAULT << 8)      /**< Shifted mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define _QSPI_FLASHCOMMANDCTRLMEM_NBOFSTIGREADBYTES_SHIFT         16                                                            /**< Shift value for QSPI_NBOFSTIGREADBYTES */
#define _QSPI_FLASHCOMMANDCTRLMEM_NBOFSTIGREADBYTES_MASK          0x70000UL                                                     /**< Bit mask for QSPI_NBOFSTIGREADBYTES */
#define _QSPI_FLASHCOMMANDCTRLMEM_NBOFSTIGREADBYTES_DEFAULT       0x00000000UL                                                  /**< Mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_NBOFSTIGREADBYTES_DEFAULT        (_QSPI_FLASHCOMMANDCTRLMEM_NBOFSTIGREADBYTES_DEFAULT << 16)   /**< Shifted mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKADDR_SHIFT               20                                                            /**< Shift value for QSPI_MEMBANKADDR */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKADDR_MASK                0x1FF00000UL                                                  /**< Bit mask for QSPI_MEMBANKADDR */
#define _QSPI_FLASHCOMMANDCTRLMEM_MEMBANKADDR_DEFAULT             0x00000000UL                                                  /**< Mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */
#define QSPI_FLASHCOMMANDCTRLMEM_MEMBANKADDR_DEFAULT              (_QSPI_FLASHCOMMANDCTRLMEM_MEMBANKADDR_DEFAULT << 20)         /**< Shifted mode DEFAULT for QSPI_FLASHCOMMANDCTRLMEM */

/* Bit fields for QSPI FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_RESETVALUE                             0x00000000UL                                      /**< Default value for QSPI_FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_MASK                                   0xFFFFFF87UL                                      /**< Mask for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_CMDEXEC                                 (0x1UL << 0)                                      /**< Execute the Command */
#define _QSPI_FLASHCMDCTRL_CMDEXEC_SHIFT                          0                                                 /**< Shift value for QSPI_CMDEXEC */
#define _QSPI_FLASHCMDCTRL_CMDEXEC_MASK                           0x1UL                                             /**< Bit mask for QSPI_CMDEXEC */
#define _QSPI_FLASHCMDCTRL_CMDEXEC_DEFAULT                        0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_CMDEXEC_DEFAULT                         (_QSPI_FLASHCMDCTRL_CMDEXEC_DEFAULT << 0)         /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_CMDEXECSTATUS                           (0x1UL << 1)                                      /**< Command Execution in Progress */
#define _QSPI_FLASHCMDCTRL_CMDEXECSTATUS_SHIFT                    1                                                 /**< Shift value for QSPI_CMDEXECSTATUS */
#define _QSPI_FLASHCMDCTRL_CMDEXECSTATUS_MASK                     0x2UL                                             /**< Bit mask for QSPI_CMDEXECSTATUS */
#define _QSPI_FLASHCMDCTRL_CMDEXECSTATUS_DEFAULT                  0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_CMDEXECSTATUS_DEFAULT                   (_QSPI_FLASHCMDCTRL_CMDEXECSTATUS_DEFAULT << 1)   /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_STIGMEMBANKEN                           (0x1UL << 2)                                      /**< STIG Memory Bank Enable Bit */
#define _QSPI_FLASHCMDCTRL_STIGMEMBANKEN_SHIFT                    2                                                 /**< Shift value for QSPI_STIGMEMBANKEN */
#define _QSPI_FLASHCMDCTRL_STIGMEMBANKEN_MASK                     0x4UL                                             /**< Bit mask for QSPI_STIGMEMBANKEN */
#define _QSPI_FLASHCMDCTRL_STIGMEMBANKEN_DEFAULT                  0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_STIGMEMBANKEN_DEFAULT                   (_QSPI_FLASHCMDCTRL_STIGMEMBANKEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_NUMDUMMYCYCLES_SHIFT                   7                                                 /**< Shift value for QSPI_NUMDUMMYCYCLES */
#define _QSPI_FLASHCMDCTRL_NUMDUMMYCYCLES_MASK                    0xF80UL                                           /**< Bit mask for QSPI_NUMDUMMYCYCLES */
#define _QSPI_FLASHCMDCTRL_NUMDUMMYCYCLES_DEFAULT                 0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_NUMDUMMYCYCLES_DEFAULT                  (_QSPI_FLASHCMDCTRL_NUMDUMMYCYCLES_DEFAULT << 7)  /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_NUMWRDATABYTES_SHIFT                   12                                                /**< Shift value for QSPI_NUMWRDATABYTES */
#define _QSPI_FLASHCMDCTRL_NUMWRDATABYTES_MASK                    0x7000UL                                          /**< Bit mask for QSPI_NUMWRDATABYTES */
#define _QSPI_FLASHCMDCTRL_NUMWRDATABYTES_DEFAULT                 0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_NUMWRDATABYTES_DEFAULT                  (_QSPI_FLASHCMDCTRL_NUMWRDATABYTES_DEFAULT << 12) /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBWRITEDATA                            (0x1UL << 15)                                     /**< Write Data Enable */
#define _QSPI_FLASHCMDCTRL_ENBWRITEDATA_SHIFT                     15                                                /**< Shift value for QSPI_ENBWRITEDATA */
#define _QSPI_FLASHCMDCTRL_ENBWRITEDATA_MASK                      0x8000UL                                          /**< Bit mask for QSPI_ENBWRITEDATA */
#define _QSPI_FLASHCMDCTRL_ENBWRITEDATA_DEFAULT                   0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBWRITEDATA_DEFAULT                    (_QSPI_FLASHCMDCTRL_ENBWRITEDATA_DEFAULT << 15)   /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_NUMADDRBYTES_SHIFT                     16                                                /**< Shift value for QSPI_NUMADDRBYTES */
#define _QSPI_FLASHCMDCTRL_NUMADDRBYTES_MASK                      0x30000UL                                         /**< Bit mask for QSPI_NUMADDRBYTES */
#define _QSPI_FLASHCMDCTRL_NUMADDRBYTES_DEFAULT                   0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_NUMADDRBYTES_DEFAULT                    (_QSPI_FLASHCMDCTRL_NUMADDRBYTES_DEFAULT << 16)   /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBMODEBIT                              (0x1UL << 18)                                     /**< Mode Bit Enable */
#define _QSPI_FLASHCMDCTRL_ENBMODEBIT_SHIFT                       18                                                /**< Shift value for QSPI_ENBMODEBIT */
#define _QSPI_FLASHCMDCTRL_ENBMODEBIT_MASK                        0x40000UL                                         /**< Bit mask for QSPI_ENBMODEBIT */
#define _QSPI_FLASHCMDCTRL_ENBMODEBIT_DEFAULT                     0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBMODEBIT_DEFAULT                      (_QSPI_FLASHCMDCTRL_ENBMODEBIT_DEFAULT << 18)     /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBCOMDADDR                             (0x1UL << 19)                                     /**< Command Address Enable */
#define _QSPI_FLASHCMDCTRL_ENBCOMDADDR_SHIFT                      19                                                /**< Shift value for QSPI_ENBCOMDADDR */
#define _QSPI_FLASHCMDCTRL_ENBCOMDADDR_MASK                       0x80000UL                                         /**< Bit mask for QSPI_ENBCOMDADDR */
#define _QSPI_FLASHCMDCTRL_ENBCOMDADDR_DEFAULT                    0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBCOMDADDR_DEFAULT                     (_QSPI_FLASHCMDCTRL_ENBCOMDADDR_DEFAULT << 19)    /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_NUMRDDATABYTES_SHIFT                   20                                                /**< Shift value for QSPI_NUMRDDATABYTES */
#define _QSPI_FLASHCMDCTRL_NUMRDDATABYTES_MASK                    0x700000UL                                        /**< Bit mask for QSPI_NUMRDDATABYTES */
#define _QSPI_FLASHCMDCTRL_NUMRDDATABYTES_DEFAULT                 0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_NUMRDDATABYTES_DEFAULT                  (_QSPI_FLASHCMDCTRL_NUMRDDATABYTES_DEFAULT << 20) /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBREADDATA                             (0x1UL << 23)                                     /**< Read Data Enable */
#define _QSPI_FLASHCMDCTRL_ENBREADDATA_SHIFT                      23                                                /**< Shift value for QSPI_ENBREADDATA */
#define _QSPI_FLASHCMDCTRL_ENBREADDATA_MASK                       0x800000UL                                        /**< Bit mask for QSPI_ENBREADDATA */
#define _QSPI_FLASHCMDCTRL_ENBREADDATA_DEFAULT                    0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_ENBREADDATA_DEFAULT                     (_QSPI_FLASHCMDCTRL_ENBREADDATA_DEFAULT << 23)    /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */
#define _QSPI_FLASHCMDCTRL_CMDOPCODE_SHIFT                        24                                                /**< Shift value for QSPI_CMDOPCODE */
#define _QSPI_FLASHCMDCTRL_CMDOPCODE_MASK                         0xFF000000UL                                      /**< Bit mask for QSPI_CMDOPCODE */
#define _QSPI_FLASHCMDCTRL_CMDOPCODE_DEFAULT                      0x00000000UL                                      /**< Mode DEFAULT for QSPI_FLASHCMDCTRL */
#define QSPI_FLASHCMDCTRL_CMDOPCODE_DEFAULT                       (_QSPI_FLASHCMDCTRL_CMDOPCODE_DEFAULT << 24)      /**< Shifted mode DEFAULT for QSPI_FLASHCMDCTRL */

/* Bit fields for QSPI FLASHCMDADDR */
#define _QSPI_FLASHCMDADDR_RESETVALUE                             0x00000000UL                           /**< Default value for QSPI_FLASHCMDADDR */
#define _QSPI_FLASHCMDADDR_MASK                                   0xFFFFFFFFUL                           /**< Mask for QSPI_FLASHCMDADDR */
#define _QSPI_FLASHCMDADDR_ADDR_SHIFT                             0                                      /**< Shift value for QSPI_ADDR */
#define _QSPI_FLASHCMDADDR_ADDR_MASK                              0xFFFFFFFFUL                           /**< Bit mask for QSPI_ADDR */
#define _QSPI_FLASHCMDADDR_ADDR_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for QSPI_FLASHCMDADDR */
#define QSPI_FLASHCMDADDR_ADDR_DEFAULT                            (_QSPI_FLASHCMDADDR_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_FLASHCMDADDR */

/* Bit fields for QSPI FLASHRDDATALOWER */
#define _QSPI_FLASHRDDATALOWER_RESETVALUE                         0x00000000UL                               /**< Default value for QSPI_FLASHRDDATALOWER */
#define _QSPI_FLASHRDDATALOWER_MASK                               0xFFFFFFFFUL                               /**< Mask for QSPI_FLASHRDDATALOWER */
#define _QSPI_FLASHRDDATALOWER_DATA_SHIFT                         0                                          /**< Shift value for QSPI_DATA */
#define _QSPI_FLASHRDDATALOWER_DATA_MASK                          0xFFFFFFFFUL                               /**< Bit mask for QSPI_DATA */
#define _QSPI_FLASHRDDATALOWER_DATA_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for QSPI_FLASHRDDATALOWER */
#define QSPI_FLASHRDDATALOWER_DATA_DEFAULT                        (_QSPI_FLASHRDDATALOWER_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_FLASHRDDATALOWER */

/* Bit fields for QSPI FLASHRDDATAUPPER */
#define _QSPI_FLASHRDDATAUPPER_RESETVALUE                         0x00000000UL                               /**< Default value for QSPI_FLASHRDDATAUPPER */
#define _QSPI_FLASHRDDATAUPPER_MASK                               0xFFFFFFFFUL                               /**< Mask for QSPI_FLASHRDDATAUPPER */
#define _QSPI_FLASHRDDATAUPPER_DATA_SHIFT                         0                                          /**< Shift value for QSPI_DATA */
#define _QSPI_FLASHRDDATAUPPER_DATA_MASK                          0xFFFFFFFFUL                               /**< Bit mask for QSPI_DATA */
#define _QSPI_FLASHRDDATAUPPER_DATA_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for QSPI_FLASHRDDATAUPPER */
#define QSPI_FLASHRDDATAUPPER_DATA_DEFAULT                        (_QSPI_FLASHRDDATAUPPER_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_FLASHRDDATAUPPER */

/* Bit fields for QSPI FLASHWRDATALOWER */
#define _QSPI_FLASHWRDATALOWER_RESETVALUE                         0x00000000UL                               /**< Default value for QSPI_FLASHWRDATALOWER */
#define _QSPI_FLASHWRDATALOWER_MASK                               0xFFFFFFFFUL                               /**< Mask for QSPI_FLASHWRDATALOWER */
#define _QSPI_FLASHWRDATALOWER_DATA_SHIFT                         0                                          /**< Shift value for QSPI_DATA */
#define _QSPI_FLASHWRDATALOWER_DATA_MASK                          0xFFFFFFFFUL                               /**< Bit mask for QSPI_DATA */
#define _QSPI_FLASHWRDATALOWER_DATA_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for QSPI_FLASHWRDATALOWER */
#define QSPI_FLASHWRDATALOWER_DATA_DEFAULT                        (_QSPI_FLASHWRDATALOWER_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_FLASHWRDATALOWER */

/* Bit fields for QSPI FLASHWRDATAUPPER */
#define _QSPI_FLASHWRDATAUPPER_RESETVALUE                         0x00000000UL                               /**< Default value for QSPI_FLASHWRDATAUPPER */
#define _QSPI_FLASHWRDATAUPPER_MASK                               0xFFFFFFFFUL                               /**< Mask for QSPI_FLASHWRDATAUPPER */
#define _QSPI_FLASHWRDATAUPPER_DATA_SHIFT                         0                                          /**< Shift value for QSPI_DATA */
#define _QSPI_FLASHWRDATAUPPER_DATA_MASK                          0xFFFFFFFFUL                               /**< Bit mask for QSPI_DATA */
#define _QSPI_FLASHWRDATAUPPER_DATA_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for QSPI_FLASHWRDATAUPPER */
#define QSPI_FLASHWRDATAUPPER_DATA_DEFAULT                        (_QSPI_FLASHWRDATAUPPER_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_FLASHWRDATAUPPER */

/* Bit fields for QSPI POLLINGFLASHSTATUS */
#define _QSPI_POLLINGFLASHSTATUS_RESETVALUE                       0x00000000UL                                                 /**< Default value for QSPI_POLLINGFLASHSTATUS */
#define _QSPI_POLLINGFLASHSTATUS_MASK                             0x000F01FFUL                                                 /**< Mask for QSPI_POLLINGFLASHSTATUS */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUS_SHIFT               0                                                            /**< Shift value for QSPI_DEVICESTATUS */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUS_MASK                0xFFUL                                                       /**< Bit mask for QSPI_DEVICESTATUS */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUS_DEFAULT             0x00000000UL                                                 /**< Mode DEFAULT for QSPI_POLLINGFLASHSTATUS */
#define QSPI_POLLINGFLASHSTATUS_DEVICESTATUS_DEFAULT              (_QSPI_POLLINGFLASHSTATUS_DEVICESTATUS_DEFAULT << 0)         /**< Shifted mode DEFAULT for QSPI_POLLINGFLASHSTATUS */
#define QSPI_POLLINGFLASHSTATUS_DEVICESTATUSVALID                 (0x1UL << 8)                                                 /**< Device Status Valid */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUSVALID_SHIFT          8                                                            /**< Shift value for QSPI_DEVICESTATUSVALID */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUSVALID_MASK           0x100UL                                                      /**< Bit mask for QSPI_DEVICESTATUSVALID */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUSVALID_DEFAULT        0x00000000UL                                                 /**< Mode DEFAULT for QSPI_POLLINGFLASHSTATUS */
#define QSPI_POLLINGFLASHSTATUS_DEVICESTATUSVALID_DEFAULT         (_QSPI_POLLINGFLASHSTATUS_DEVICESTATUSVALID_DEFAULT << 8)    /**< Shifted mode DEFAULT for QSPI_POLLINGFLASHSTATUS */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUSNBDUMMY_SHIFT        16                                                           /**< Shift value for QSPI_DEVICESTATUSNBDUMMY */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUSNBDUMMY_MASK         0xF0000UL                                                    /**< Bit mask for QSPI_DEVICESTATUSNBDUMMY */
#define _QSPI_POLLINGFLASHSTATUS_DEVICESTATUSNBDUMMY_DEFAULT      0x00000000UL                                                 /**< Mode DEFAULT for QSPI_POLLINGFLASHSTATUS */
#define QSPI_POLLINGFLASHSTATUS_DEVICESTATUSNBDUMMY_DEFAULT       (_QSPI_POLLINGFLASHSTATUS_DEVICESTATUSNBDUMMY_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_POLLINGFLASHSTATUS */

/* Bit fields for QSPI PHYCONFIGURATION */
#define _QSPI_PHYCONFIGURATION_RESETVALUE                         0x00000000UL                                               /**< Default value for QSPI_PHYCONFIGURATION */
#define _QSPI_PHYCONFIGURATION_MASK                               0x807F007FUL                                               /**< Mask for QSPI_PHYCONFIGURATION */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGRXDLLDELAY_SHIFT          0                                                          /**< Shift value for QSPI_PHYCONFIGRXDLLDELAY */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGRXDLLDELAY_MASK           0x7FUL                                                     /**< Bit mask for QSPI_PHYCONFIGRXDLLDELAY */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGRXDLLDELAY_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_PHYCONFIGURATION */
#define QSPI_PHYCONFIGURATION_PHYCONFIGRXDLLDELAY_DEFAULT         (_QSPI_PHYCONFIGURATION_PHYCONFIGRXDLLDELAY_DEFAULT << 0)  /**< Shifted mode DEFAULT for QSPI_PHYCONFIGURATION */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGTXDLLDELAY_SHIFT          16                                                         /**< Shift value for QSPI_PHYCONFIGTXDLLDELAY */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGTXDLLDELAY_MASK           0x7F0000UL                                                 /**< Bit mask for QSPI_PHYCONFIGTXDLLDELAY */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGTXDLLDELAY_DEFAULT        0x00000000UL                                               /**< Mode DEFAULT for QSPI_PHYCONFIGURATION */
#define QSPI_PHYCONFIGURATION_PHYCONFIGTXDLLDELAY_DEFAULT         (_QSPI_PHYCONFIGURATION_PHYCONFIGTXDLLDELAY_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_PHYCONFIGURATION */
#define QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC                     (0x1UL << 31)                                              /**< PHY Config Resync */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC_SHIFT              31                                                         /**< Shift value for QSPI_PHYCONFIGRESYNC */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC_MASK               0x80000000UL                                               /**< Bit mask for QSPI_PHYCONFIGRESYNC */
#define _QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC_DEFAULT            0x00000000UL                                               /**< Mode DEFAULT for QSPI_PHYCONFIGURATION */
#define QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC_DEFAULT             (_QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC_DEFAULT << 31)     /**< Shifted mode DEFAULT for QSPI_PHYCONFIGURATION */

/* Bit fields for QSPI OPCODEEXTLOWER */
#define _QSPI_OPCODEEXTLOWER_RESETVALUE                           0x13EDFA00UL                                        /**< Default value for QSPI_OPCODEEXTLOWER */
#define _QSPI_OPCODEEXTLOWER_MASK                                 0xFFFFFFFFUL                                        /**< Mask for QSPI_OPCODEEXTLOWER */
#define _QSPI_OPCODEEXTLOWER_EXTSTIGOPCODE_SHIFT                  0                                                   /**< Shift value for QSPI_EXTSTIGOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTSTIGOPCODE_MASK                   0xFFUL                                              /**< Bit mask for QSPI_EXTSTIGOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTSTIGOPCODE_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define QSPI_OPCODEEXTLOWER_EXTSTIGOPCODE_DEFAULT                 (_QSPI_OPCODEEXTLOWER_EXTSTIGOPCODE_DEFAULT << 0)   /**< Shifted mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define _QSPI_OPCODEEXTLOWER_EXTPOLLOPCODE_SHIFT                  8                                                   /**< Shift value for QSPI_EXTPOLLOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTPOLLOPCODE_MASK                   0xFF00UL                                            /**< Bit mask for QSPI_EXTPOLLOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTPOLLOPCODE_DEFAULT                0x000000FAUL                                        /**< Mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define QSPI_OPCODEEXTLOWER_EXTPOLLOPCODE_DEFAULT                 (_QSPI_OPCODEEXTLOWER_EXTPOLLOPCODE_DEFAULT << 8)   /**< Shifted mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define _QSPI_OPCODEEXTLOWER_EXTWRITEOPCODE_SHIFT                 16                                                  /**< Shift value for QSPI_EXTWRITEOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTWRITEOPCODE_MASK                  0xFF0000UL                                          /**< Bit mask for QSPI_EXTWRITEOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTWRITEOPCODE_DEFAULT               0x000000EDUL                                        /**< Mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define QSPI_OPCODEEXTLOWER_EXTWRITEOPCODE_DEFAULT                (_QSPI_OPCODEEXTLOWER_EXTWRITEOPCODE_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define _QSPI_OPCODEEXTLOWER_EXTREADOPCODE_SHIFT                  24                                                  /**< Shift value for QSPI_EXTREADOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTREADOPCODE_MASK                   0xFF000000UL                                        /**< Bit mask for QSPI_EXTREADOPCODE */
#define _QSPI_OPCODEEXTLOWER_EXTREADOPCODE_DEFAULT                0x00000013UL                                        /**< Mode DEFAULT for QSPI_OPCODEEXTLOWER */
#define QSPI_OPCODEEXTLOWER_EXTREADOPCODE_DEFAULT                 (_QSPI_OPCODEEXTLOWER_EXTREADOPCODE_DEFAULT << 24)  /**< Shifted mode DEFAULT for QSPI_OPCODEEXTLOWER */

/* Bit fields for QSPI OPCODEEXTUPPER */
#define _QSPI_OPCODEEXTUPPER_RESETVALUE                           0x06F90000UL                                      /**< Default value for QSPI_OPCODEEXTUPPER */
#define _QSPI_OPCODEEXTUPPER_MASK                                 0xFFFF0000UL                                      /**< Mask for QSPI_OPCODEEXTUPPER */
#define _QSPI_OPCODEEXTUPPER_EXTWELOPCODE_SHIFT                   16                                                /**< Shift value for QSPI_EXTWELOPCODE */
#define _QSPI_OPCODEEXTUPPER_EXTWELOPCODE_MASK                    0xFF0000UL                                        /**< Bit mask for QSPI_EXTWELOPCODE */
#define _QSPI_OPCODEEXTUPPER_EXTWELOPCODE_DEFAULT                 0x000000F9UL                                      /**< Mode DEFAULT for QSPI_OPCODEEXTUPPER */
#define QSPI_OPCODEEXTUPPER_EXTWELOPCODE_DEFAULT                  (_QSPI_OPCODEEXTUPPER_EXTWELOPCODE_DEFAULT << 16) /**< Shifted mode DEFAULT for QSPI_OPCODEEXTUPPER */
#define _QSPI_OPCODEEXTUPPER_WELOPCODE_SHIFT                      24                                                /**< Shift value for QSPI_WELOPCODE */
#define _QSPI_OPCODEEXTUPPER_WELOPCODE_MASK                       0xFF000000UL                                      /**< Bit mask for QSPI_WELOPCODE */
#define _QSPI_OPCODEEXTUPPER_WELOPCODE_DEFAULT                    0x00000006UL                                      /**< Mode DEFAULT for QSPI_OPCODEEXTUPPER */
#define QSPI_OPCODEEXTUPPER_WELOPCODE_DEFAULT                     (_QSPI_OPCODEEXTUPPER_WELOPCODE_DEFAULT << 24)    /**< Shifted mode DEFAULT for QSPI_OPCODEEXTUPPER */

/* Bit fields for QSPI MODULEID */
#define _QSPI_MODULEID_RESETVALUE                                 0x00000200UL                            /**< Default value for QSPI_MODULEID */
#define _QSPI_MODULEID_MASK                                       0xFFFFFF03UL                            /**< Mask for QSPI_MODULEID */
#define _QSPI_MODULEID_CONF_SHIFT                                 0                                       /**< Shift value for QSPI_CONF */
#define _QSPI_MODULEID_CONF_MASK                                  0x3UL                                   /**< Bit mask for QSPI_CONF */
#define _QSPI_MODULEID_CONF_DEFAULT                               0x00000000UL                            /**< Mode DEFAULT for QSPI_MODULEID */
#define QSPI_MODULEID_CONF_DEFAULT                                (_QSPI_MODULEID_CONF_DEFAULT << 0)      /**< Shifted mode DEFAULT for QSPI_MODULEID */
#define _QSPI_MODULEID_MODULEID_SHIFT                             8                                       /**< Shift value for QSPI_MODULEID */
#define _QSPI_MODULEID_MODULEID_MASK                              0xFFFF00UL                              /**< Bit mask for QSPI_MODULEID */
#define _QSPI_MODULEID_MODULEID_DEFAULT                           0x00000002UL                            /**< Mode DEFAULT for QSPI_MODULEID */
#define QSPI_MODULEID_MODULEID_DEFAULT                            (_QSPI_MODULEID_MODULEID_DEFAULT << 8)  /**< Shifted mode DEFAULT for QSPI_MODULEID */
#define _QSPI_MODULEID_FIXPATCH_SHIFT                             24                                      /**< Shift value for QSPI_FIXPATCH */
#define _QSPI_MODULEID_FIXPATCH_MASK                              0xFF000000UL                            /**< Bit mask for QSPI_FIXPATCH */
#define _QSPI_MODULEID_FIXPATCH_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for QSPI_MODULEID */
#define QSPI_MODULEID_FIXPATCH_DEFAULT                            (_QSPI_MODULEID_FIXPATCH_DEFAULT << 24) /**< Shifted mode DEFAULT for QSPI_MODULEID */

/* Bit fields for QSPI ROUTEPEN */
#define _QSPI_ROUTEPEN_RESETVALUE                                 0x00000000UL                             /**< Default value for QSPI_ROUTEPEN */
#define _QSPI_ROUTEPEN_MASK                                       0x00007FE7UL                             /**< Mask for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_SCLKPEN                                     (0x1UL << 0)                             /**< SCLK Pin Enable */
#define _QSPI_ROUTEPEN_SCLKPEN_SHIFT                              0                                        /**< Shift value for QSPI_SCLKPEN */
#define _QSPI_ROUTEPEN_SCLKPEN_MASK                               0x1UL                                    /**< Bit mask for QSPI_SCLKPEN */
#define _QSPI_ROUTEPEN_SCLKPEN_DEFAULT                            0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_SCLKPEN_DEFAULT                             (_QSPI_ROUTEPEN_SCLKPEN_DEFAULT << 0)    /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_CS0PEN                                      (0x1UL << 1)                             /**< CS0 Pin Enable */
#define _QSPI_ROUTEPEN_CS0PEN_SHIFT                               1                                        /**< Shift value for QSPI_CS0PEN */
#define _QSPI_ROUTEPEN_CS0PEN_MASK                                0x2UL                                    /**< Bit mask for QSPI_CS0PEN */
#define _QSPI_ROUTEPEN_CS0PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_CS0PEN_DEFAULT                              (_QSPI_ROUTEPEN_CS0PEN_DEFAULT << 1)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_CS1PEN                                      (0x1UL << 2)                             /**< CS1 Pin Enable */
#define _QSPI_ROUTEPEN_CS1PEN_SHIFT                               2                                        /**< Shift value for QSPI_CS1PEN */
#define _QSPI_ROUTEPEN_CS1PEN_MASK                                0x4UL                                    /**< Bit mask for QSPI_CS1PEN */
#define _QSPI_ROUTEPEN_CS1PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_CS1PEN_DEFAULT                              (_QSPI_ROUTEPEN_CS1PEN_DEFAULT << 2)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ0PEN                                      (0x1UL << 5)                             /**< DQ0 Pin Enable */
#define _QSPI_ROUTEPEN_DQ0PEN_SHIFT                               5                                        /**< Shift value for QSPI_DQ0PEN */
#define _QSPI_ROUTEPEN_DQ0PEN_MASK                                0x20UL                                   /**< Bit mask for QSPI_DQ0PEN */
#define _QSPI_ROUTEPEN_DQ0PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ0PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ0PEN_DEFAULT << 5)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ1PEN                                      (0x1UL << 6)                             /**< DQ1 Pin Enable */
#define _QSPI_ROUTEPEN_DQ1PEN_SHIFT                               6                                        /**< Shift value for QSPI_DQ1PEN */
#define _QSPI_ROUTEPEN_DQ1PEN_MASK                                0x40UL                                   /**< Bit mask for QSPI_DQ1PEN */
#define _QSPI_ROUTEPEN_DQ1PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ1PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ1PEN_DEFAULT << 6)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ2PEN                                      (0x1UL << 7)                             /**< DQ2 Pin Enable */
#define _QSPI_ROUTEPEN_DQ2PEN_SHIFT                               7                                        /**< Shift value for QSPI_DQ2PEN */
#define _QSPI_ROUTEPEN_DQ2PEN_MASK                                0x80UL                                   /**< Bit mask for QSPI_DQ2PEN */
#define _QSPI_ROUTEPEN_DQ2PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ2PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ2PEN_DEFAULT << 7)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ3PEN                                      (0x1UL << 8)                             /**< DQ3 Pin Enable */
#define _QSPI_ROUTEPEN_DQ3PEN_SHIFT                               8                                        /**< Shift value for QSPI_DQ3PEN */
#define _QSPI_ROUTEPEN_DQ3PEN_MASK                                0x100UL                                  /**< Bit mask for QSPI_DQ3PEN */
#define _QSPI_ROUTEPEN_DQ3PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ3PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ3PEN_DEFAULT << 8)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ4PEN                                      (0x1UL << 9)                             /**< DQ4 Pin Enable */
#define _QSPI_ROUTEPEN_DQ4PEN_SHIFT                               9                                        /**< Shift value for QSPI_DQ4PEN */
#define _QSPI_ROUTEPEN_DQ4PEN_MASK                                0x200UL                                  /**< Bit mask for QSPI_DQ4PEN */
#define _QSPI_ROUTEPEN_DQ4PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ4PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ4PEN_DEFAULT << 9)     /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ5PEN                                      (0x1UL << 10)                            /**< DQ5 Pin Enable */
#define _QSPI_ROUTEPEN_DQ5PEN_SHIFT                               10                                       /**< Shift value for QSPI_DQ5PEN */
#define _QSPI_ROUTEPEN_DQ5PEN_MASK                                0x400UL                                  /**< Bit mask for QSPI_DQ5PEN */
#define _QSPI_ROUTEPEN_DQ5PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ5PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ5PEN_DEFAULT << 10)    /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ6PEN                                      (0x1UL << 11)                            /**< DQ6 Pin Enable */
#define _QSPI_ROUTEPEN_DQ6PEN_SHIFT                               11                                       /**< Shift value for QSPI_DQ6PEN */
#define _QSPI_ROUTEPEN_DQ6PEN_MASK                                0x800UL                                  /**< Bit mask for QSPI_DQ6PEN */
#define _QSPI_ROUTEPEN_DQ6PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ6PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ6PEN_DEFAULT << 11)    /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ7PEN                                      (0x1UL << 12)                            /**< DQ7 Pin Enable */
#define _QSPI_ROUTEPEN_DQ7PEN_SHIFT                               12                                       /**< Shift value for QSPI_DQ7PEN */
#define _QSPI_ROUTEPEN_DQ7PEN_MASK                                0x1000UL                                 /**< Bit mask for QSPI_DQ7PEN */
#define _QSPI_ROUTEPEN_DQ7PEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQ7PEN_DEFAULT                              (_QSPI_ROUTEPEN_DQ7PEN_DEFAULT << 12)    /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQSPEN                                      (0x1UL << 13)                            /**< DQS Pin Enable */
#define _QSPI_ROUTEPEN_DQSPEN_SHIFT                               13                                       /**< Shift value for QSPI_DQSPEN */
#define _QSPI_ROUTEPEN_DQSPEN_MASK                                0x2000UL                                 /**< Bit mask for QSPI_DQSPEN */
#define _QSPI_ROUTEPEN_DQSPEN_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_DQSPEN_DEFAULT                              (_QSPI_ROUTEPEN_DQSPEN_DEFAULT << 13)    /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_SCLKINPEN                                   (0x1UL << 14)                            /**< SCLKIN Pin Enable */
#define _QSPI_ROUTEPEN_SCLKINPEN_SHIFT                            14                                       /**< Shift value for QSPI_SCLKINPEN */
#define _QSPI_ROUTEPEN_SCLKINPEN_MASK                             0x4000UL                                 /**< Bit mask for QSPI_SCLKINPEN */
#define _QSPI_ROUTEPEN_SCLKINPEN_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for QSPI_ROUTEPEN */
#define QSPI_ROUTEPEN_SCLKINPEN_DEFAULT                           (_QSPI_ROUTEPEN_SCLKINPEN_DEFAULT << 14) /**< Shifted mode DEFAULT for QSPI_ROUTEPEN */

/* Bit fields for QSPI ROUTELOC0 */
#define _QSPI_ROUTELOC0_RESETVALUE                                0x00000000UL                           /**< Default value for QSPI_ROUTELOC0 */
#define _QSPI_ROUTELOC0_MASK                                      0x00000003UL                           /**< Mask for QSPI_ROUTELOC0 */
#define _QSPI_ROUTELOC0_QSPILOC_SHIFT                             0                                      /**< Shift value for QSPI_QSPILOC */
#define _QSPI_ROUTELOC0_QSPILOC_MASK                              0x3UL                                  /**< Bit mask for QSPI_QSPILOC */
#define _QSPI_ROUTELOC0_QSPILOC_LOC0                              0x00000000UL                           /**< Mode LOC0 for QSPI_ROUTELOC0 */
#define _QSPI_ROUTELOC0_QSPILOC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for QSPI_ROUTELOC0 */
#define _QSPI_ROUTELOC0_QSPILOC_LOC1                              0x00000001UL                           /**< Mode LOC1 for QSPI_ROUTELOC0 */
#define _QSPI_ROUTELOC0_QSPILOC_LOC2                              0x00000002UL                           /**< Mode LOC2 for QSPI_ROUTELOC0 */
#define QSPI_ROUTELOC0_QSPILOC_LOC0                               (_QSPI_ROUTELOC0_QSPILOC_LOC0 << 0)    /**< Shifted mode LOC0 for QSPI_ROUTELOC0 */
#define QSPI_ROUTELOC0_QSPILOC_DEFAULT                            (_QSPI_ROUTELOC0_QSPILOC_DEFAULT << 0) /**< Shifted mode DEFAULT for QSPI_ROUTELOC0 */
#define QSPI_ROUTELOC0_QSPILOC_LOC1                               (_QSPI_ROUTELOC0_QSPILOC_LOC1 << 0)    /**< Shifted mode LOC1 for QSPI_ROUTELOC0 */
#define QSPI_ROUTELOC0_QSPILOC_LOC2                               (_QSPI_ROUTELOC0_QSPILOC_LOC2 << 0)    /**< Shifted mode LOC2 for QSPI_ROUTELOC0 */

/** @} */
/** @} End of group EFM32GG11B_QSPI */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

