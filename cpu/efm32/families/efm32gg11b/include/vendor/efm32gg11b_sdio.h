/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_SDIO register and bit field definitions
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
 * @defgroup EFM32GG11B_SDIO SDIO
 * @{
 * @brief EFM32GG11B_SDIO Register Declaration
 ******************************************************************************/
/** SDIO Register Declaration */
typedef struct {
  __IOM uint32_t SDMASYSADDR;     /**< SDMA System Address Register  */
  __IOM uint32_t BLKSIZE;         /**< Block Size and Block Count Register  */
  __IOM uint32_t CMDARG1;         /**< SD Command Argument Register  */
  __IOM uint32_t TFRMODE;         /**< Transfer Mode and Command Register  */
  __IM uint32_t  RESP0;           /**< Response0 and Response1 Register  */
  __IM uint32_t  RESP2;           /**< Response2 and Response3 Register  */
  __IM uint32_t  RESP4;           /**< Response4 and Response5 Register  */
  __IM uint32_t  RESP6;           /**< Response6 and Response7 Register  */
  __IOM uint32_t BUFDATPORT;      /**< Buffer Data Register  */
  __IM uint32_t  PRSSTAT;         /**< Present State Register  */
  __IOM uint32_t HOSTCTRL1;       /**< Host Control1, Power, Block Gap and Wakeup-up Control Register  */
  __IOM uint32_t CLOCKCTRL;       /**< Clock Control, Timeout Control and Software Register  */
  __IOM uint32_t IFCR;            /**< Normal and Error Interrupt Status Register  */
  __IOM uint32_t IFENC;           /**< Normal and Error Interrupt Status Enable Register  */
  __IOM uint32_t IEN;             /**< Normal and Error Interrupt Signal Enable Register  */
  __IOM uint32_t AC12ERRSTAT;     /**< AUTO CMD12 Error Status and Host Control2 Register  */
  __IM uint32_t  CAPAB0;          /**< Capabilities Register to Hold Bits 31~0  */
  __IM uint32_t  CAPAB2;          /**< Capabilities Register to Hold Bits 63~32  */
  __IM uint32_t  MAXCURCAPAB;     /**< Maximum Current Capabilities Register  */
  uint32_t       RESERVED0[1U];   /**< Reserved for future use **/
  __IOM uint32_t FEVTERRSTAT;     /**< Force Event Register for Auto CMD Error Status  */
  __IM uint32_t  ADMAES;          /**< ADMA Error Status Register  */
  __IOM uint32_t ADSADDR;         /**< ADMA System Address Register  */
  uint32_t       RESERVED1[1U];   /**< Reserved for future use **/
  __IM uint32_t  PRSTVAL0;        /**< Preset Value for Initialization and Default Speed Mode  */
  __IM uint32_t  PRSTVAL2;        /**< Preset Value for High Speed and SDR12 Modes  */
  __IM uint32_t  PRSTVAL4;        /**< Preset Value for SDR25 and SDR50 Modes  */
  __IM uint32_t  PRSTVAL6;        /**< Preset Value for SDR104 and DDR50 Modes  */
  __IOM uint32_t BOOTTOCTRL;      /**< Boot Timeout Control Register  */
  uint32_t       RESERVED2[34U];  /**< Reserved for future use **/
  __IM uint32_t  SLOTINTSTAT;     /**< Slot Interrupt Status Register  */

  uint32_t       RESERVED3[448U]; /**< Reserved for future use **/
  __IOM uint32_t CTRL;            /**< Core Control Signals  */
  __IOM uint32_t CFG0;            /**< Core Configuration 0  */
  __IOM uint32_t CFG1;            /**< Core Configuration 1  */
  __IOM uint32_t CFGPRESETVAL0;   /**< Core Configuration Preset Value 0  */
  __IOM uint32_t CFGPRESETVAL1;   /**< Core Configuration Preset Value 1  */
  __IOM uint32_t CFGPRESETVAL2;   /**< Core Configuration Preset Value 2  */
  __IOM uint32_t CFGPRESETVAL3;   /**< Core Configuration Preset Value 3  */
  __IOM uint32_t ROUTELOC0;       /**< I/O LOCATION Register  */
  __IOM uint32_t ROUTELOC1;       /**< I/O LOCATION Register  */
  __IOM uint32_t ROUTEPEN;        /**< I/O LOCATION Enable Register  */
} SDIO_TypeDef;                   /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_SDIO
 * @{
 * @defgroup EFM32GG11B_SDIO_BitFields  SDIO Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for SDIO SDMASYSADDR */
#define _SDIO_SDMASYSADDR_RESETVALUE                   0x00000000UL                                    /**< Default value for SDIO_SDMASYSADDR */
#define _SDIO_SDMASYSADDR_MASK                         0xFFFFFFFFUL                                    /**< Mask for SDIO_SDMASYSADDR */
#define _SDIO_SDMASYSADDR_SDMASYSADDRARG_SHIFT         0                                               /**< Shift value for SDIO_SDMASYSADDRARG */
#define _SDIO_SDMASYSADDR_SDMASYSADDRARG_MASK          0xFFFFFFFFUL                                    /**< Bit mask for SDIO_SDMASYSADDRARG */
#define _SDIO_SDMASYSADDR_SDMASYSADDRARG_DEFAULT       0x00000000UL                                    /**< Mode DEFAULT for SDIO_SDMASYSADDR */
#define SDIO_SDMASYSADDR_SDMASYSADDRARG_DEFAULT        (_SDIO_SDMASYSADDR_SDMASYSADDRARG_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_SDMASYSADDR */

/* Bit fields for SDIO BLKSIZE */
#define _SDIO_BLKSIZE_RESETVALUE                       0x00000000UL                                    /**< Default value for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_MASK                             0xFFFF7FFFUL                                    /**< Mask for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_TFRBLKSIZE_SHIFT                 0                                               /**< Shift value for SDIO_TFRBLKSIZE */
#define _SDIO_BLKSIZE_TFRBLKSIZE_MASK                  0xFFFUL                                         /**< Bit mask for SDIO_TFRBLKSIZE */
#define _SDIO_BLKSIZE_TFRBLKSIZE_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_TFRBLKSIZE_NOXFER                0x00000000UL                                    /**< Mode NOXFER for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_TFRBLKSIZE_DEFAULT                (_SDIO_BLKSIZE_TFRBLKSIZE_DEFAULT << 0)         /**< Shifted mode DEFAULT for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_TFRBLKSIZE_NOXFER                 (_SDIO_BLKSIZE_TFRBLKSIZE_NOXFER << 0)          /**< Shifted mode NOXFER for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SHIFT             12                                              /**< Shift value for SDIO_HSTSDMABUFSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_MASK              0x7000UL                                        /**< Bit mask for SDIO_HSTSDMABUFSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE4             0x00000000UL                                    /**< Mode SIZE4 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE8             0x00000001UL                                    /**< Mode SIZE8 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE16            0x00000002UL                                    /**< Mode SIZE16 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE32            0x00000003UL                                    /**< Mode SIZE32 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE64            0x00000004UL                                    /**< Mode SIZE64 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE128           0x00000005UL                                    /**< Mode SIZE128 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE256           0x00000006UL                                    /**< Mode SIZE256 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE512           0x00000007UL                                    /**< Mode SIZE512 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_DEFAULT            (_SDIO_BLKSIZE_HSTSDMABUFSIZE_DEFAULT << 12)    /**< Shifted mode DEFAULT for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE4              (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE4 << 12)      /**< Shifted mode SIZE4 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE8              (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE8 << 12)      /**< Shifted mode SIZE8 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE16             (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE16 << 12)     /**< Shifted mode SIZE16 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE32             (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE32 << 12)     /**< Shifted mode SIZE32 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE64             (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE64 << 12)     /**< Shifted mode SIZE64 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE128            (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE128 << 12)    /**< Shifted mode SIZE128 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE256            (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE256 << 12)    /**< Shifted mode SIZE256 for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE512            (_SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE512 << 12)    /**< Shifted mode SIZE512 for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_BLKSCNTFORCURRTFR_SHIFT          16                                              /**< Shift value for SDIO_BLKSCNTFORCURRTFR */
#define _SDIO_BLKSIZE_BLKSCNTFORCURRTFR_MASK           0xFFFF0000UL                                    /**< Bit mask for SDIO_BLKSCNTFORCURRTFR */
#define _SDIO_BLKSIZE_BLKSCNTFORCURRTFR_DEFAULT        0x00000000UL                                    /**< Mode DEFAULT for SDIO_BLKSIZE */
#define _SDIO_BLKSIZE_BLKSCNTFORCURRTFR_STOPCNT        0x00000000UL                                    /**< Mode STOPCNT for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_BLKSCNTFORCURRTFR_DEFAULT         (_SDIO_BLKSIZE_BLKSCNTFORCURRTFR_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_BLKSIZE */
#define SDIO_BLKSIZE_BLKSCNTFORCURRTFR_STOPCNT         (_SDIO_BLKSIZE_BLKSCNTFORCURRTFR_STOPCNT << 16) /**< Shifted mode STOPCNT for SDIO_BLKSIZE */

/* Bit fields for SDIO CMDARG1 */
#define _SDIO_CMDARG1_RESETVALUE                       0x00000000UL                         /**< Default value for SDIO_CMDARG1 */
#define _SDIO_CMDARG1_MASK                             0xFFFFFFFFUL                         /**< Mask for SDIO_CMDARG1 */
#define _SDIO_CMDARG1_CMDARG1_SHIFT                    0                                    /**< Shift value for SDIO_CMDARG1 */
#define _SDIO_CMDARG1_CMDARG1_MASK                     0xFFFFFFFFUL                         /**< Bit mask for SDIO_CMDARG1 */
#define _SDIO_CMDARG1_CMDARG1_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for SDIO_CMDARG1 */
#define SDIO_CMDARG1_CMDARG1_DEFAULT                   (_SDIO_CMDARG1_CMDARG1_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_CMDARG1 */

/* Bit fields for SDIO TFRMODE */
#define _SDIO_TFRMODE_RESETVALUE                       0x00000000UL                                  /**< Default value for SDIO_TFRMODE */
#define _SDIO_TFRMODE_MASK                             0x3FFB003FUL                                  /**< Mask for SDIO_TFRMODE */
#define SDIO_TFRMODE_DMAEN                             (0x1UL << 0)                                  /**< DMA Enable */
#define _SDIO_TFRMODE_DMAEN_SHIFT                      0                                             /**< Shift value for SDIO_DMAEN */
#define _SDIO_TFRMODE_DMAEN_MASK                       0x1UL                                         /**< Bit mask for SDIO_DMAEN */
#define _SDIO_TFRMODE_DMAEN_DEFAULT                    0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_DMAEN_DISABLE                    0x00000000UL                                  /**< Mode DISABLE for SDIO_TFRMODE */
#define _SDIO_TFRMODE_DMAEN_ENABLE                     0x00000001UL                                  /**< Mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_DMAEN_DEFAULT                     (_SDIO_TFRMODE_DMAEN_DEFAULT << 0)            /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_DMAEN_DISABLE                     (_SDIO_TFRMODE_DMAEN_DISABLE << 0)            /**< Shifted mode DISABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_DMAEN_ENABLE                      (_SDIO_TFRMODE_DMAEN_ENABLE << 0)             /**< Shifted mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_BLKCNTEN                          (0x1UL << 1)                                  /**< Block Count Enable */
#define _SDIO_TFRMODE_BLKCNTEN_SHIFT                   1                                             /**< Shift value for SDIO_BLKCNTEN */
#define _SDIO_TFRMODE_BLKCNTEN_MASK                    0x2UL                                         /**< Bit mask for SDIO_BLKCNTEN */
#define _SDIO_TFRMODE_BLKCNTEN_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_BLKCNTEN_DISABLE                 0x00000000UL                                  /**< Mode DISABLE for SDIO_TFRMODE */
#define _SDIO_TFRMODE_BLKCNTEN_ENABLE                  0x00000001UL                                  /**< Mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_BLKCNTEN_DEFAULT                  (_SDIO_TFRMODE_BLKCNTEN_DEFAULT << 1)         /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_BLKCNTEN_DISABLE                  (_SDIO_TFRMODE_BLKCNTEN_DISABLE << 1)         /**< Shifted mode DISABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_BLKCNTEN_ENABLE                   (_SDIO_TFRMODE_BLKCNTEN_ENABLE << 1)          /**< Shifted mode ENABLE for SDIO_TFRMODE */
#define _SDIO_TFRMODE_AUTOCMDEN_SHIFT                  2                                             /**< Shift value for SDIO_AUTOCMDEN */
#define _SDIO_TFRMODE_AUTOCMDEN_MASK                   0xCUL                                         /**< Bit mask for SDIO_AUTOCMDEN */
#define _SDIO_TFRMODE_AUTOCMDEN_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_AUTOCMDEN_ACMDDISABLED           0x00000000UL                                  /**< Mode ACMDDISABLED for SDIO_TFRMODE */
#define _SDIO_TFRMODE_AUTOCMDEN_ACMD12EN               0x00000001UL                                  /**< Mode ACMD12EN for SDIO_TFRMODE */
#define _SDIO_TFRMODE_AUTOCMDEN_ACMD23EN               0x00000002UL                                  /**< Mode ACMD23EN for SDIO_TFRMODE */
#define SDIO_TFRMODE_AUTOCMDEN_DEFAULT                 (_SDIO_TFRMODE_AUTOCMDEN_DEFAULT << 2)        /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_AUTOCMDEN_ACMDDISABLED            (_SDIO_TFRMODE_AUTOCMDEN_ACMDDISABLED << 2)   /**< Shifted mode ACMDDISABLED for SDIO_TFRMODE */
#define SDIO_TFRMODE_AUTOCMDEN_ACMD12EN                (_SDIO_TFRMODE_AUTOCMDEN_ACMD12EN << 2)       /**< Shifted mode ACMD12EN for SDIO_TFRMODE */
#define SDIO_TFRMODE_AUTOCMDEN_ACMD23EN                (_SDIO_TFRMODE_AUTOCMDEN_ACMD23EN << 2)       /**< Shifted mode ACMD23EN for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATDIRSEL                         (0x1UL << 4)                                  /**< Data Transfer Direction Select */
#define _SDIO_TFRMODE_DATDIRSEL_SHIFT                  4                                             /**< Shift value for SDIO_DATDIRSEL */
#define _SDIO_TFRMODE_DATDIRSEL_MASK                   0x10UL                                        /**< Bit mask for SDIO_DATDIRSEL */
#define _SDIO_TFRMODE_DATDIRSEL_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_DATDIRSEL_DISABLE                0x00000000UL                                  /**< Mode DISABLE for SDIO_TFRMODE */
#define _SDIO_TFRMODE_DATDIRSEL_ENABLE                 0x00000001UL                                  /**< Mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATDIRSEL_DEFAULT                 (_SDIO_TFRMODE_DATDIRSEL_DEFAULT << 4)        /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATDIRSEL_DISABLE                 (_SDIO_TFRMODE_DATDIRSEL_DISABLE << 4)        /**< Shifted mode DISABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATDIRSEL_ENABLE                  (_SDIO_TFRMODE_DATDIRSEL_ENABLE << 4)         /**< Shifted mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_MULTSINGBLKSEL                    (0x1UL << 5)                                  /**< Multiple or Single Block Data Transfer Selection */
#define _SDIO_TFRMODE_MULTSINGBLKSEL_SHIFT             5                                             /**< Shift value for SDIO_MULTSINGBLKSEL */
#define _SDIO_TFRMODE_MULTSINGBLKSEL_MASK              0x20UL                                        /**< Bit mask for SDIO_MULTSINGBLKSEL */
#define _SDIO_TFRMODE_MULTSINGBLKSEL_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_MULTSINGBLKSEL_SINGLEBLK         0x00000000UL                                  /**< Mode SINGLEBLK for SDIO_TFRMODE */
#define _SDIO_TFRMODE_MULTSINGBLKSEL_MULTIBLK          0x00000001UL                                  /**< Mode MULTIBLK for SDIO_TFRMODE */
#define SDIO_TFRMODE_MULTSINGBLKSEL_DEFAULT            (_SDIO_TFRMODE_MULTSINGBLKSEL_DEFAULT << 5)   /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_MULTSINGBLKSEL_SINGLEBLK          (_SDIO_TFRMODE_MULTSINGBLKSEL_SINGLEBLK << 5) /**< Shifted mode SINGLEBLK for SDIO_TFRMODE */
#define SDIO_TFRMODE_MULTSINGBLKSEL_MULTIBLK           (_SDIO_TFRMODE_MULTSINGBLKSEL_MULTIBLK << 5)  /**< Shifted mode MULTIBLK for SDIO_TFRMODE */
#define _SDIO_TFRMODE_RESPTYPESEL_SHIFT                16                                            /**< Shift value for SDIO_RESPTYPESEL */
#define _SDIO_TFRMODE_RESPTYPESEL_MASK                 0x30000UL                                     /**< Bit mask for SDIO_RESPTYPESEL */
#define _SDIO_TFRMODE_RESPTYPESEL_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_RESPTYPESEL_NORESP               0x00000000UL                                  /**< Mode NORESP for SDIO_TFRMODE */
#define _SDIO_TFRMODE_RESPTYPESEL_RESP136              0x00000001UL                                  /**< Mode RESP136 for SDIO_TFRMODE */
#define _SDIO_TFRMODE_RESPTYPESEL_RESP48               0x00000002UL                                  /**< Mode RESP48 for SDIO_TFRMODE */
#define _SDIO_TFRMODE_RESPTYPESEL_BUSYAFTRESP          0x00000003UL                                  /**< Mode BUSYAFTRESP for SDIO_TFRMODE */
#define SDIO_TFRMODE_RESPTYPESEL_DEFAULT               (_SDIO_TFRMODE_RESPTYPESEL_DEFAULT << 16)     /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_RESPTYPESEL_NORESP                (_SDIO_TFRMODE_RESPTYPESEL_NORESP << 16)      /**< Shifted mode NORESP for SDIO_TFRMODE */
#define SDIO_TFRMODE_RESPTYPESEL_RESP136               (_SDIO_TFRMODE_RESPTYPESEL_RESP136 << 16)     /**< Shifted mode RESP136 for SDIO_TFRMODE */
#define SDIO_TFRMODE_RESPTYPESEL_RESP48                (_SDIO_TFRMODE_RESPTYPESEL_RESP48 << 16)      /**< Shifted mode RESP48 for SDIO_TFRMODE */
#define SDIO_TFRMODE_RESPTYPESEL_BUSYAFTRESP           (_SDIO_TFRMODE_RESPTYPESEL_BUSYAFTRESP << 16) /**< Shifted mode BUSYAFTRESP for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDCRCCHKEN                       (0x1UL << 19)                                 /**< Command CRC Check Enable */
#define _SDIO_TFRMODE_CMDCRCCHKEN_SHIFT                19                                            /**< Shift value for SDIO_CMDCRCCHKEN */
#define _SDIO_TFRMODE_CMDCRCCHKEN_MASK                 0x80000UL                                     /**< Bit mask for SDIO_CMDCRCCHKEN */
#define _SDIO_TFRMODE_CMDCRCCHKEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDCRCCHKEN_DISABLE              0x00000000UL                                  /**< Mode DISABLE for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDCRCCHKEN_ENABLE               0x00000001UL                                  /**< Mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDCRCCHKEN_DEFAULT               (_SDIO_TFRMODE_CMDCRCCHKEN_DEFAULT << 19)     /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDCRCCHKEN_DISABLE               (_SDIO_TFRMODE_CMDCRCCHKEN_DISABLE << 19)     /**< Shifted mode DISABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDCRCCHKEN_ENABLE                (_SDIO_TFRMODE_CMDCRCCHKEN_ENABLE << 19)      /**< Shifted mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDINDXCHKEN                      (0x1UL << 20)                                 /**< Command Index Check Enable */
#define _SDIO_TFRMODE_CMDINDXCHKEN_SHIFT               20                                            /**< Shift value for SDIO_CMDINDXCHKEN */
#define _SDIO_TFRMODE_CMDINDXCHKEN_MASK                0x100000UL                                    /**< Bit mask for SDIO_CMDINDXCHKEN */
#define _SDIO_TFRMODE_CMDINDXCHKEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDINDXCHKEN_DISABLE             0x00000000UL                                  /**< Mode DISABLE for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDINDXCHKEN_ENABLE              0x00000001UL                                  /**< Mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDINDXCHKEN_DEFAULT              (_SDIO_TFRMODE_CMDINDXCHKEN_DEFAULT << 20)    /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDINDXCHKEN_DISABLE              (_SDIO_TFRMODE_CMDINDXCHKEN_DISABLE << 20)    /**< Shifted mode DISABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDINDXCHKEN_ENABLE               (_SDIO_TFRMODE_CMDINDXCHKEN_ENABLE << 20)     /**< Shifted mode ENABLE for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATPRESSEL                        (0x1UL << 21)                                 /**< Data Present Select */
#define _SDIO_TFRMODE_DATPRESSEL_SHIFT                 21                                            /**< Shift value for SDIO_DATPRESSEL */
#define _SDIO_TFRMODE_DATPRESSEL_MASK                  0x200000UL                                    /**< Bit mask for SDIO_DATPRESSEL */
#define _SDIO_TFRMODE_DATPRESSEL_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_DATPRESSEL_NODATA                0x00000000UL                                  /**< Mode NODATA for SDIO_TFRMODE */
#define _SDIO_TFRMODE_DATPRESSEL_DATA                  0x00000001UL                                  /**< Mode DATA for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATPRESSEL_DEFAULT                (_SDIO_TFRMODE_DATPRESSEL_DEFAULT << 21)      /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATPRESSEL_NODATA                 (_SDIO_TFRMODE_DATPRESSEL_NODATA << 21)       /**< Shifted mode NODATA for SDIO_TFRMODE */
#define SDIO_TFRMODE_DATPRESSEL_DATA                   (_SDIO_TFRMODE_DATPRESSEL_DATA << 21)         /**< Shifted mode DATA for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDTYPE_SHIFT                    22                                            /**< Shift value for SDIO_CMDTYPE */
#define _SDIO_TFRMODE_CMDTYPE_MASK                     0xC00000UL                                    /**< Bit mask for SDIO_CMDTYPE */
#define _SDIO_TFRMODE_CMDTYPE_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDTYPE_NORMAL                   0x00000000UL                                  /**< Mode NORMAL for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDTYPE_SUSPEND                  0x00000001UL                                  /**< Mode SUSPEND for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDTYPE_RESUME                   0x00000002UL                                  /**< Mode RESUME for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDTYPE_ABORT                    0x00000003UL                                  /**< Mode ABORT for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDTYPE_DEFAULT                   (_SDIO_TFRMODE_CMDTYPE_DEFAULT << 22)         /**< Shifted mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDTYPE_NORMAL                    (_SDIO_TFRMODE_CMDTYPE_NORMAL << 22)          /**< Shifted mode NORMAL for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDTYPE_SUSPEND                   (_SDIO_TFRMODE_CMDTYPE_SUSPEND << 22)         /**< Shifted mode SUSPEND for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDTYPE_RESUME                    (_SDIO_TFRMODE_CMDTYPE_RESUME << 22)          /**< Shifted mode RESUME for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDTYPE_ABORT                     (_SDIO_TFRMODE_CMDTYPE_ABORT << 22)           /**< Shifted mode ABORT for SDIO_TFRMODE */
#define _SDIO_TFRMODE_CMDINDEX_SHIFT                   24                                            /**< Shift value for SDIO_CMDINDEX */
#define _SDIO_TFRMODE_CMDINDEX_MASK                    0x3F000000UL                                  /**< Bit mask for SDIO_CMDINDEX */
#define _SDIO_TFRMODE_CMDINDEX_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for SDIO_TFRMODE */
#define SDIO_TFRMODE_CMDINDEX_DEFAULT                  (_SDIO_TFRMODE_CMDINDEX_DEFAULT << 24)        /**< Shifted mode DEFAULT for SDIO_TFRMODE */

/* Bit fields for SDIO RESP0 */
#define _SDIO_RESP0_RESETVALUE                         0x00000000UL                        /**< Default value for SDIO_RESP0 */
#define _SDIO_RESP0_MASK                               0xFFFFFFFFUL                        /**< Mask for SDIO_RESP0 */
#define _SDIO_RESP0_CMDRESP0_SHIFT                     0                                   /**< Shift value for SDIO_CMDRESP0 */
#define _SDIO_RESP0_CMDRESP0_MASK                      0xFFFFFFFFUL                        /**< Bit mask for SDIO_CMDRESP0 */
#define _SDIO_RESP0_CMDRESP0_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for SDIO_RESP0 */
#define SDIO_RESP0_CMDRESP0_DEFAULT                    (_SDIO_RESP0_CMDRESP0_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_RESP0 */

/* Bit fields for SDIO RESP2 */
#define _SDIO_RESP2_RESETVALUE                         0x00000000UL                        /**< Default value for SDIO_RESP2 */
#define _SDIO_RESP2_MASK                               0xFFFFFFFFUL                        /**< Mask for SDIO_RESP2 */
#define _SDIO_RESP2_CMDRESP1_SHIFT                     0                                   /**< Shift value for SDIO_CMDRESP1 */
#define _SDIO_RESP2_CMDRESP1_MASK                      0xFFFFFFFFUL                        /**< Bit mask for SDIO_CMDRESP1 */
#define _SDIO_RESP2_CMDRESP1_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for SDIO_RESP2 */
#define SDIO_RESP2_CMDRESP1_DEFAULT                    (_SDIO_RESP2_CMDRESP1_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_RESP2 */

/* Bit fields for SDIO RESP4 */
#define _SDIO_RESP4_RESETVALUE                         0x00000000UL                        /**< Default value for SDIO_RESP4 */
#define _SDIO_RESP4_MASK                               0xFFFFFFFFUL                        /**< Mask for SDIO_RESP4 */
#define _SDIO_RESP4_CMDRESP2_SHIFT                     0                                   /**< Shift value for SDIO_CMDRESP2 */
#define _SDIO_RESP4_CMDRESP2_MASK                      0xFFFFFFFFUL                        /**< Bit mask for SDIO_CMDRESP2 */
#define _SDIO_RESP4_CMDRESP2_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for SDIO_RESP4 */
#define SDIO_RESP4_CMDRESP2_DEFAULT                    (_SDIO_RESP4_CMDRESP2_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_RESP4 */

/* Bit fields for SDIO RESP6 */
#define _SDIO_RESP6_RESETVALUE                         0x00000000UL                        /**< Default value for SDIO_RESP6 */
#define _SDIO_RESP6_MASK                               0xFFFFFFFFUL                        /**< Mask for SDIO_RESP6 */
#define _SDIO_RESP6_CMDRESP3_SHIFT                     0                                   /**< Shift value for SDIO_CMDRESP3 */
#define _SDIO_RESP6_CMDRESP3_MASK                      0xFFFFFFFFUL                        /**< Bit mask for SDIO_CMDRESP3 */
#define _SDIO_RESP6_CMDRESP3_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for SDIO_RESP6 */
#define SDIO_RESP6_CMDRESP3_DEFAULT                    (_SDIO_RESP6_CMDRESP3_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_RESP6 */

/* Bit fields for SDIO BUFDATPORT */
#define _SDIO_BUFDATPORT_RESETVALUE                    0x00000000UL                           /**< Default value for SDIO_BUFDATPORT */
#define _SDIO_BUFDATPORT_MASK                          0xFFFFFFFFUL                           /**< Mask for SDIO_BUFDATPORT */
#define _SDIO_BUFDATPORT_BUFDAT_SHIFT                  0                                      /**< Shift value for SDIO_BUFDAT */
#define _SDIO_BUFDATPORT_BUFDAT_MASK                   0xFFFFFFFFUL                           /**< Bit mask for SDIO_BUFDAT */
#define _SDIO_BUFDATPORT_BUFDAT_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for SDIO_BUFDATPORT */
#define SDIO_BUFDATPORT_BUFDAT_DEFAULT                 (_SDIO_BUFDATPORT_BUFDAT_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_BUFDATPORT */

/* Bit fields for SDIO PRSSTAT */
#define _SDIO_PRSSTAT_RESETVALUE                       0x00000000UL                                    /**< Default value for SDIO_PRSSTAT */
#define _SDIO_PRSSTAT_MASK                             0x1FFF0F0FUL                                    /**< Mask for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CMDINHIBITCMD                     (0x1UL << 0)                                    /**< Command Inhibit (CMD) */
#define _SDIO_PRSSTAT_CMDINHIBITCMD_SHIFT              0                                               /**< Shift value for SDIO_CMDINHIBITCMD */
#define _SDIO_PRSSTAT_CMDINHIBITCMD_MASK               0x1UL                                           /**< Bit mask for SDIO_CMDINHIBITCMD */
#define _SDIO_PRSSTAT_CMDINHIBITCMD_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CMDINHIBITCMD_DEFAULT             (_SDIO_PRSSTAT_CMDINHIBITCMD_DEFAULT << 0)      /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CMDINHIBITDAT                     (0x1UL << 1)                                    /**< Command Inhibit (DAT) */
#define _SDIO_PRSSTAT_CMDINHIBITDAT_SHIFT              1                                               /**< Shift value for SDIO_CMDINHIBITDAT */
#define _SDIO_PRSSTAT_CMDINHIBITDAT_MASK               0x2UL                                           /**< Bit mask for SDIO_CMDINHIBITDAT */
#define _SDIO_PRSSTAT_CMDINHIBITDAT_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CMDINHIBITDAT_DEFAULT             (_SDIO_PRSSTAT_CMDINHIBITDAT_DEFAULT << 1)      /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_DATLINEACTIVE                     (0x1UL << 2)                                    /**< DAT Line Active */
#define _SDIO_PRSSTAT_DATLINEACTIVE_SHIFT              2                                               /**< Shift value for SDIO_DATLINEACTIVE */
#define _SDIO_PRSSTAT_DATLINEACTIVE_MASK               0x4UL                                           /**< Bit mask for SDIO_DATLINEACTIVE */
#define _SDIO_PRSSTAT_DATLINEACTIVE_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_DATLINEACTIVE_DEFAULT             (_SDIO_PRSSTAT_DATLINEACTIVE_DEFAULT << 2)      /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_RETUNINGREQ                       (0x1UL << 3)                                    /**< Re-Tuning Request */
#define _SDIO_PRSSTAT_RETUNINGREQ_SHIFT                3                                               /**< Shift value for SDIO_RETUNINGREQ */
#define _SDIO_PRSSTAT_RETUNINGREQ_MASK                 0x8UL                                           /**< Bit mask for SDIO_RETUNINGREQ */
#define _SDIO_PRSSTAT_RETUNINGREQ_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_RETUNINGREQ_DEFAULT               (_SDIO_PRSSTAT_RETUNINGREQ_DEFAULT << 3)        /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_WRTRANACT                         (0x1UL << 8)                                    /**< Write Transfer Active */
#define _SDIO_PRSSTAT_WRTRANACT_SHIFT                  8                                               /**< Shift value for SDIO_WRTRANACT */
#define _SDIO_PRSSTAT_WRTRANACT_MASK                   0x100UL                                         /**< Bit mask for SDIO_WRTRANACT */
#define _SDIO_PRSSTAT_WRTRANACT_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_WRTRANACT_DEFAULT                 (_SDIO_PRSSTAT_WRTRANACT_DEFAULT << 8)          /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_RDTRANACT                         (0x1UL << 9)                                    /**< Read Transfer Active */
#define _SDIO_PRSSTAT_RDTRANACT_SHIFT                  9                                               /**< Shift value for SDIO_RDTRANACT */
#define _SDIO_PRSSTAT_RDTRANACT_MASK                   0x200UL                                         /**< Bit mask for SDIO_RDTRANACT */
#define _SDIO_PRSSTAT_RDTRANACT_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_RDTRANACT_DEFAULT                 (_SDIO_PRSSTAT_RDTRANACT_DEFAULT << 9)          /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_BUFFERWRITEENABLE                 (0x1UL << 10)                                   /**< Buffer Write Enable */
#define _SDIO_PRSSTAT_BUFFERWRITEENABLE_SHIFT          10                                              /**< Shift value for SDIO_BUFFERWRITEENABLE */
#define _SDIO_PRSSTAT_BUFFERWRITEENABLE_MASK           0x400UL                                         /**< Bit mask for SDIO_BUFFERWRITEENABLE */
#define _SDIO_PRSSTAT_BUFFERWRITEENABLE_DEFAULT        0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_BUFFERWRITEENABLE_DEFAULT         (_SDIO_PRSSTAT_BUFFERWRITEENABLE_DEFAULT << 10) /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_BUFRDEN                           (0x1UL << 11)                                   /**< Buffer Read Enable */
#define _SDIO_PRSSTAT_BUFRDEN_SHIFT                    11                                              /**< Shift value for SDIO_BUFRDEN */
#define _SDIO_PRSSTAT_BUFRDEN_MASK                     0x800UL                                         /**< Bit mask for SDIO_BUFRDEN */
#define _SDIO_PRSSTAT_BUFRDEN_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_BUFRDEN_DEFAULT                   (_SDIO_PRSSTAT_BUFRDEN_DEFAULT << 11)           /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CARDINS                           (0x1UL << 16)                                   /**< Card Inserted Status */
#define _SDIO_PRSSTAT_CARDINS_SHIFT                    16                                              /**< Shift value for SDIO_CARDINS */
#define _SDIO_PRSSTAT_CARDINS_MASK                     0x10000UL                                       /**< Bit mask for SDIO_CARDINS */
#define _SDIO_PRSSTAT_CARDINS_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CARDINS_DEFAULT                   (_SDIO_PRSSTAT_CARDINS_DEFAULT << 16)           /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CARDSTATESTABLE                   (0x1UL << 17)                                   /**< Card State Stable Status */
#define _SDIO_PRSSTAT_CARDSTATESTABLE_SHIFT            17                                              /**< Shift value for SDIO_CARDSTATESTABLE */
#define _SDIO_PRSSTAT_CARDSTATESTABLE_MASK             0x20000UL                                       /**< Bit mask for SDIO_CARDSTATESTABLE */
#define _SDIO_PRSSTAT_CARDSTATESTABLE_DEFAULT          0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CARDSTATESTABLE_DEFAULT           (_SDIO_PRSSTAT_CARDSTATESTABLE_DEFAULT << 17)   /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CARDDETPINLVL                     (0x1UL << 18)                                   /**< Card Detect Pin Level */
#define _SDIO_PRSSTAT_CARDDETPINLVL_SHIFT              18                                              /**< Shift value for SDIO_CARDDETPINLVL */
#define _SDIO_PRSSTAT_CARDDETPINLVL_MASK               0x40000UL                                       /**< Bit mask for SDIO_CARDDETPINLVL */
#define _SDIO_PRSSTAT_CARDDETPINLVL_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CARDDETPINLVL_DEFAULT             (_SDIO_PRSSTAT_CARDDETPINLVL_DEFAULT << 18)     /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_WRPROTSWPINLVL                    (0x1UL << 19)                                   /**< Write Protect Switch Pin Level */
#define _SDIO_PRSSTAT_WRPROTSWPINLVL_SHIFT             19                                              /**< Shift value for SDIO_WRPROTSWPINLVL */
#define _SDIO_PRSSTAT_WRPROTSWPINLVL_MASK              0x80000UL                                       /**< Bit mask for SDIO_WRPROTSWPINLVL */
#define _SDIO_PRSSTAT_WRPROTSWPINLVL_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_WRPROTSWPINLVL_DEFAULT            (_SDIO_PRSSTAT_WRPROTSWPINLVL_DEFAULT << 19)    /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define _SDIO_PRSSTAT_DAT3TO0SIGLVL_SHIFT              20                                              /**< Shift value for SDIO_DAT3TO0SIGLVL */
#define _SDIO_PRSSTAT_DAT3TO0SIGLVL_MASK               0xF00000UL                                      /**< Bit mask for SDIO_DAT3TO0SIGLVL */
#define _SDIO_PRSSTAT_DAT3TO0SIGLVL_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_DAT3TO0SIGLVL_DEFAULT             (_SDIO_PRSSTAT_DAT3TO0SIGLVL_DEFAULT << 20)     /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CMDSIGLVL                         (0x1UL << 24)                                   /**< Command Line Signal Level */
#define _SDIO_PRSSTAT_CMDSIGLVL_SHIFT                  24                                              /**< Shift value for SDIO_CMDSIGLVL */
#define _SDIO_PRSSTAT_CMDSIGLVL_MASK                   0x1000000UL                                     /**< Bit mask for SDIO_CMDSIGLVL */
#define _SDIO_PRSSTAT_CMDSIGLVL_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_CMDSIGLVL_DEFAULT                 (_SDIO_PRSSTAT_CMDSIGLVL_DEFAULT << 24)         /**< Shifted mode DEFAULT for SDIO_PRSSTAT */
#define _SDIO_PRSSTAT_DAT7TO4SIGLVL_SHIFT              25                                              /**< Shift value for SDIO_DAT7TO4SIGLVL */
#define _SDIO_PRSSTAT_DAT7TO4SIGLVL_MASK               0x1E000000UL                                    /**< Bit mask for SDIO_DAT7TO4SIGLVL */
#define _SDIO_PRSSTAT_DAT7TO4SIGLVL_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SDIO_PRSSTAT */
#define SDIO_PRSSTAT_DAT7TO4SIGLVL_DEFAULT             (_SDIO_PRSSTAT_DAT7TO4SIGLVL_DEFAULT << 25)     /**< Shifted mode DEFAULT for SDIO_PRSSTAT */

/* Bit fields for SDIO HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_RESETVALUE                     0x00800000UL                                        /**< Default value for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_MASK                           0x07FF1FFFUL                                        /**< Mask for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_LEDCTRL                         (0x1UL << 0)                                        /**< LED Control */
#define _SDIO_HOSTCTRL1_LEDCTRL_SHIFT                  0                                                   /**< Shift value for SDIO_LEDCTRL */
#define _SDIO_HOSTCTRL1_LEDCTRL_MASK                   0x1UL                                               /**< Bit mask for SDIO_LEDCTRL */
#define _SDIO_HOSTCTRL1_LEDCTRL_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_LEDCTRL_LEDOFF                 0x00000000UL                                        /**< Mode LEDOFF for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_LEDCTRL_LEDON                  0x00000001UL                                        /**< Mode LEDON for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_LEDCTRL_DEFAULT                 (_SDIO_HOSTCTRL1_LEDCTRL_DEFAULT << 0)              /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_LEDCTRL_LEDOFF                  (_SDIO_HOSTCTRL1_LEDCTRL_LEDOFF << 0)               /**< Shifted mode LEDOFF for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_LEDCTRL_LEDON                   (_SDIO_HOSTCTRL1_LEDCTRL_LEDON << 0)                /**< Shifted mode LEDON for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DATTRANWD                       (0x1UL << 1)                                        /**< Data Transfer Width 1-bit or 4-bit Mode */
#define _SDIO_HOSTCTRL1_DATTRANWD_SHIFT                1                                                   /**< Shift value for SDIO_DATTRANWD */
#define _SDIO_HOSTCTRL1_DATTRANWD_MASK                 0x2UL                                               /**< Bit mask for SDIO_DATTRANWD */
#define _SDIO_HOSTCTRL1_DATTRANWD_DEFAULT              0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DATTRANWD_SD1                  0x00000000UL                                        /**< Mode SD1 for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DATTRANWD_SD4                  0x00000001UL                                        /**< Mode SD4 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DATTRANWD_DEFAULT               (_SDIO_HOSTCTRL1_DATTRANWD_DEFAULT << 1)            /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DATTRANWD_SD1                   (_SDIO_HOSTCTRL1_DATTRANWD_SD1 << 1)                /**< Shifted mode SD1 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DATTRANWD_SD4                   (_SDIO_HOSTCTRL1_DATTRANWD_SD4 << 1)                /**< Shifted mode SD4 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_HSEN                            (0x1UL << 2)                                        /**< High Speed Enable */
#define _SDIO_HOSTCTRL1_HSEN_SHIFT                     2                                                   /**< Shift value for SDIO_HSEN */
#define _SDIO_HOSTCTRL1_HSEN_MASK                      0x4UL                                               /**< Bit mask for SDIO_HSEN */
#define _SDIO_HOSTCTRL1_HSEN_DEFAULT                   0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_HSEN_NS                        0x00000000UL                                        /**< Mode NS for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_HSEN_HS                        0x00000001UL                                        /**< Mode HS for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_HSEN_DEFAULT                    (_SDIO_HOSTCTRL1_HSEN_DEFAULT << 2)                 /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_HSEN_NS                         (_SDIO_HOSTCTRL1_HSEN_NS << 2)                      /**< Shifted mode NS for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_HSEN_HS                         (_SDIO_HOSTCTRL1_HSEN_HS << 2)                      /**< Shifted mode HS for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DMASEL_SHIFT                   3                                                   /**< Shift value for SDIO_DMASEL */
#define _SDIO_HOSTCTRL1_DMASEL_MASK                    0x18UL                                              /**< Bit mask for SDIO_DMASEL */
#define _SDIO_HOSTCTRL1_DMASEL_DEFAULT                 0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DMASEL_SDMA                    0x00000000UL                                        /**< Mode SDMA for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DMASEL_ADMA1                   0x00000001UL                                        /**< Mode ADMA1 for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DMASEL_ADMA2                   0x00000002UL                                        /**< Mode ADMA2 for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_DMASEL_64BITADMA2              0x00000003UL                                        /**< Mode 64BITADMA2 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DMASEL_DEFAULT                  (_SDIO_HOSTCTRL1_DMASEL_DEFAULT << 3)               /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DMASEL_SDMA                     (_SDIO_HOSTCTRL1_DMASEL_SDMA << 3)                  /**< Shifted mode SDMA for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DMASEL_ADMA1                    (_SDIO_HOSTCTRL1_DMASEL_ADMA1 << 3)                 /**< Shifted mode ADMA1 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DMASEL_ADMA2                    (_SDIO_HOSTCTRL1_DMASEL_ADMA2 << 3)                 /**< Shifted mode ADMA2 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_DMASEL_64BITADMA2               (_SDIO_HOSTCTRL1_DMASEL_64BITADMA2 << 3)            /**< Shifted mode 64BITADMA2 for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_EXTDATTRANWD                    (0x1UL << 5)                                        /**< Extended Data Transfer Width */
#define _SDIO_HOSTCTRL1_EXTDATTRANWD_SHIFT             5                                                   /**< Shift value for SDIO_EXTDATTRANWD */
#define _SDIO_HOSTCTRL1_EXTDATTRANWD_MASK              0x20UL                                              /**< Bit mask for SDIO_EXTDATTRANWD */
#define _SDIO_HOSTCTRL1_EXTDATTRANWD_DEFAULT           0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_EXTDATTRANWD_8BIT              0x00000000UL                                        /**< Mode 8BIT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_EXTDATTRANWD_DATXFR            0x00000001UL                                        /**< Mode DATXFR for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_EXTDATTRANWD_DEFAULT            (_SDIO_HOSTCTRL1_EXTDATTRANWD_DEFAULT << 5)         /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_EXTDATTRANWD_8BIT               (_SDIO_HOSTCTRL1_EXTDATTRANWD_8BIT << 5)            /**< Shifted mode 8BIT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_EXTDATTRANWD_DATXFR             (_SDIO_HOSTCTRL1_EXTDATTRANWD_DATXFR << 5)          /**< Shifted mode DATXFR for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDTSTLVL                        (0x1UL << 6)                                        /**< Card Detect Test Level */
#define _SDIO_HOSTCTRL1_CDTSTLVL_SHIFT                 6                                                   /**< Shift value for SDIO_CDTSTLVL */
#define _SDIO_HOSTCTRL1_CDTSTLVL_MASK                  0x40UL                                              /**< Bit mask for SDIO_CDTSTLVL */
#define _SDIO_HOSTCTRL1_CDTSTLVL_DEFAULT               0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_CDTSTLVL_NOCARD                0x00000000UL                                        /**< Mode NOCARD for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_CDTSTLVL_CARDIN                0x00000001UL                                        /**< Mode CARDIN for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDTSTLVL_DEFAULT                (_SDIO_HOSTCTRL1_CDTSTLVL_DEFAULT << 6)             /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDTSTLVL_NOCARD                 (_SDIO_HOSTCTRL1_CDTSTLVL_NOCARD << 6)              /**< Shifted mode NOCARD for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDTSTLVL_CARDIN                 (_SDIO_HOSTCTRL1_CDTSTLVL_CARDIN << 6)              /**< Shifted mode CARDIN for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDSIGDET                        (0x1UL << 7)                                        /**< Card Detetct Signal Detection */
#define _SDIO_HOSTCTRL1_CDSIGDET_SHIFT                 7                                                   /**< Shift value for SDIO_CDSIGDET */
#define _SDIO_HOSTCTRL1_CDSIGDET_MASK                  0x80UL                                              /**< Bit mask for SDIO_CDSIGDET */
#define _SDIO_HOSTCTRL1_CDSIGDET_DEFAULT               0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_CDSIGDET_SDCD                  0x00000000UL                                        /**< Mode SDCD for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_CDSIGDET_TSTLVL                0x00000001UL                                        /**< Mode TSTLVL for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDSIGDET_DEFAULT                (_SDIO_HOSTCTRL1_CDSIGDET_DEFAULT << 7)             /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDSIGDET_SDCD                   (_SDIO_HOSTCTRL1_CDSIGDET_SDCD << 7)                /**< Shifted mode SDCD for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CDSIGDET_TSTLVL                 (_SDIO_HOSTCTRL1_CDSIGDET_TSTLVL << 7)              /**< Shifted mode TSTLVL for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SDBUSPOWER                      (0x1UL << 8)                                        /**< SD Bus Power */
#define _SDIO_HOSTCTRL1_SDBUSPOWER_SHIFT               8                                                   /**< Shift value for SDIO_SDBUSPOWER */
#define _SDIO_HOSTCTRL1_SDBUSPOWER_MASK                0x100UL                                             /**< Bit mask for SDIO_SDBUSPOWER */
#define _SDIO_HOSTCTRL1_SDBUSPOWER_DEFAULT             0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SDBUSPOWER_DEFAULT              (_SDIO_HOSTCTRL1_SDBUSPOWER_DEFAULT << 8)           /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_SDBUSVOLTSEL_SHIFT             9                                                   /**< Shift value for SDIO_SDBUSVOLTSEL */
#define _SDIO_HOSTCTRL1_SDBUSVOLTSEL_MASK              0xE00UL                                             /**< Bit mask for SDIO_SDBUSVOLTSEL */
#define _SDIO_HOSTCTRL1_SDBUSVOLTSEL_DEFAULT           0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_SDBUSVOLTSEL_1P8V              0x00000005UL                                        /**< Mode 1P8V for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P0V              0x00000006UL                                        /**< Mode 3P0V for SDIO_HOSTCTRL1 */
#define _SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P3V              0x00000007UL                                        /**< Mode 3P3V for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SDBUSVOLTSEL_DEFAULT            (_SDIO_HOSTCTRL1_SDBUSVOLTSEL_DEFAULT << 9)         /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SDBUSVOLTSEL_1P8V               (_SDIO_HOSTCTRL1_SDBUSVOLTSEL_1P8V << 9)            /**< Shifted mode 1P8V for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P0V               (_SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P0V << 9)            /**< Shifted mode 3P0V for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P3V               (_SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P3V << 9)            /**< Shifted mode 3P3V for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_HRDRST                          (0x1UL << 12)                                       /**< Hardware Reset Signal */
#define _SDIO_HOSTCTRL1_HRDRST_SHIFT                   12                                                  /**< Shift value for SDIO_HRDRST */
#define _SDIO_HOSTCTRL1_HRDRST_MASK                    0x1000UL                                            /**< Bit mask for SDIO_HRDRST */
#define _SDIO_HOSTCTRL1_HRDRST_DEFAULT                 0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_HRDRST_DEFAULT                  (_SDIO_HOSTCTRL1_HRDRST_DEFAULT << 12)              /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_STOPATBLKGAPREQ                 (0x1UL << 16)                                       /**< Stop at Block Gap Request */
#define _SDIO_HOSTCTRL1_STOPATBLKGAPREQ_SHIFT          16                                                  /**< Shift value for SDIO_STOPATBLKGAPREQ */
#define _SDIO_HOSTCTRL1_STOPATBLKGAPREQ_MASK           0x10000UL                                           /**< Bit mask for SDIO_STOPATBLKGAPREQ */
#define _SDIO_HOSTCTRL1_STOPATBLKGAPREQ_DEFAULT        0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_STOPATBLKGAPREQ_DEFAULT         (_SDIO_HOSTCTRL1_STOPATBLKGAPREQ_DEFAULT << 16)     /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CONTINUEREQ                     (0x1UL << 17)                                       /**< Continue Request */
#define _SDIO_HOSTCTRL1_CONTINUEREQ_SHIFT              17                                                  /**< Shift value for SDIO_CONTINUEREQ */
#define _SDIO_HOSTCTRL1_CONTINUEREQ_MASK               0x20000UL                                           /**< Bit mask for SDIO_CONTINUEREQ */
#define _SDIO_HOSTCTRL1_CONTINUEREQ_DEFAULT            0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_CONTINUEREQ_DEFAULT             (_SDIO_HOSTCTRL1_CONTINUEREQ_DEFAULT << 17)         /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_RDWAITCTRL                      (0x1UL << 18)                                       /**< Read Wait Control */
#define _SDIO_HOSTCTRL1_RDWAITCTRL_SHIFT               18                                                  /**< Shift value for SDIO_RDWAITCTRL */
#define _SDIO_HOSTCTRL1_RDWAITCTRL_MASK                0x40000UL                                           /**< Bit mask for SDIO_RDWAITCTRL */
#define _SDIO_HOSTCTRL1_RDWAITCTRL_DEFAULT             0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_RDWAITCTRL_DEFAULT              (_SDIO_HOSTCTRL1_RDWAITCTRL_DEFAULT << 18)          /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_INTATBLKGAP                     (0x1UL << 19)                                       /**< Interrupt at Block Gap */
#define _SDIO_HOSTCTRL1_INTATBLKGAP_SHIFT              19                                                  /**< Shift value for SDIO_INTATBLKGAP */
#define _SDIO_HOSTCTRL1_INTATBLKGAP_MASK               0x80000UL                                           /**< Bit mask for SDIO_INTATBLKGAP */
#define _SDIO_HOSTCTRL1_INTATBLKGAP_DEFAULT            0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_INTATBLKGAP_DEFAULT             (_SDIO_HOSTCTRL1_INTATBLKGAP_DEFAULT << 19)         /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SPIMODE                         (0x1UL << 20)                                       /**< SPI Mode Enable */
#define _SDIO_HOSTCTRL1_SPIMODE_SHIFT                  20                                                  /**< Shift value for SDIO_SPIMODE */
#define _SDIO_HOSTCTRL1_SPIMODE_MASK                   0x100000UL                                          /**< Bit mask for SDIO_SPIMODE */
#define _SDIO_HOSTCTRL1_SPIMODE_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_SPIMODE_DEFAULT                 (_SDIO_HOSTCTRL1_SPIMODE_DEFAULT << 20)             /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_BOOTEN                          (0x1UL << 21)                                       /**< Boot Enable */
#define _SDIO_HOSTCTRL1_BOOTEN_SHIFT                   21                                                  /**< Shift value for SDIO_BOOTEN */
#define _SDIO_HOSTCTRL1_BOOTEN_MASK                    0x200000UL                                          /**< Bit mask for SDIO_BOOTEN */
#define _SDIO_HOSTCTRL1_BOOTEN_DEFAULT                 0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_BOOTEN_DEFAULT                  (_SDIO_HOSTCTRL1_BOOTEN_DEFAULT << 21)              /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_ALTBOOTEN                       (0x1UL << 22)                                       /**< Alternate Boot Enable */
#define _SDIO_HOSTCTRL1_ALTBOOTEN_SHIFT                22                                                  /**< Shift value for SDIO_ALTBOOTEN */
#define _SDIO_HOSTCTRL1_ALTBOOTEN_MASK                 0x400000UL                                          /**< Bit mask for SDIO_ALTBOOTEN */
#define _SDIO_HOSTCTRL1_ALTBOOTEN_DEFAULT              0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_ALTBOOTEN_DEFAULT               (_SDIO_HOSTCTRL1_ALTBOOTEN_DEFAULT << 22)           /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_BOOTACKCHK                      (0x1UL << 23)                                       /**< Boot Ack Check */
#define _SDIO_HOSTCTRL1_BOOTACKCHK_SHIFT               23                                                  /**< Shift value for SDIO_BOOTACKCHK */
#define _SDIO_HOSTCTRL1_BOOTACKCHK_MASK                0x800000UL                                          /**< Bit mask for SDIO_BOOTACKCHK */
#define _SDIO_HOSTCTRL1_BOOTACKCHK_DEFAULT             0x00000001UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_BOOTACKCHK_DEFAULT              (_SDIO_HOSTCTRL1_BOOTACKCHK_DEFAULT << 23)          /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_WKUPEVNTENONCARDINT             (0x1UL << 24)                                       /**< Wakeup Event Enable on Card Interrupt */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCARDINT_SHIFT      24                                                  /**< Shift value for SDIO_WKUPEVNTENONCARDINT */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCARDINT_MASK       0x1000000UL                                         /**< Bit mask for SDIO_WKUPEVNTENONCARDINT */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCARDINT_DEFAULT    0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_WKUPEVNTENONCARDINT_DEFAULT     (_SDIO_HOSTCTRL1_WKUPEVNTENONCARDINT_DEFAULT << 24) /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_WKUPEVNTENONCINS                (0x1UL << 25)                                       /**< Wakeup Event Enable on SD Card Insertion */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCINS_SHIFT         25                                                  /**< Shift value for SDIO_WKUPEVNTENONCINS */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCINS_MASK          0x2000000UL                                         /**< Bit mask for SDIO_WKUPEVNTENONCINS */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCINS_DEFAULT       0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_WKUPEVNTENONCINS_DEFAULT        (_SDIO_HOSTCTRL1_WKUPEVNTENONCINS_DEFAULT << 25)    /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_WKUPEVNTENONCRM                 (0x1UL << 26)                                       /**< Wakeup Event Enable on SD Card Removal */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCRM_SHIFT          26                                                  /**< Shift value for SDIO_WKUPEVNTENONCRM */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCRM_MASK           0x4000000UL                                         /**< Bit mask for SDIO_WKUPEVNTENONCRM */
#define _SDIO_HOSTCTRL1_WKUPEVNTENONCRM_DEFAULT        0x00000000UL                                        /**< Mode DEFAULT for SDIO_HOSTCTRL1 */
#define SDIO_HOSTCTRL1_WKUPEVNTENONCRM_DEFAULT         (_SDIO_HOSTCTRL1_WKUPEVNTENONCRM_DEFAULT << 26)     /**< Shifted mode DEFAULT for SDIO_HOSTCTRL1 */

/* Bit fields for SDIO CLOCKCTRL */
#define _SDIO_CLOCKCTRL_RESETVALUE                     0x00000000UL                                   /**< Default value for SDIO_CLOCKCTRL */
#define _SDIO_CLOCKCTRL_MASK                           0x070FFFE7UL                                   /**< Mask for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_INTCLKEN                        (0x1UL << 0)                                   /**< Internal Clock Enable */
#define _SDIO_CLOCKCTRL_INTCLKEN_SHIFT                 0                                              /**< Shift value for SDIO_INTCLKEN */
#define _SDIO_CLOCKCTRL_INTCLKEN_MASK                  0x1UL                                          /**< Bit mask for SDIO_INTCLKEN */
#define _SDIO_CLOCKCTRL_INTCLKEN_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_INTCLKEN_DEFAULT                (_SDIO_CLOCKCTRL_INTCLKEN_DEFAULT << 0)        /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_INTCLKSTABLE                    (0x1UL << 1)                                   /**< Internal Clock Stable */
#define _SDIO_CLOCKCTRL_INTCLKSTABLE_SHIFT             1                                              /**< Shift value for SDIO_INTCLKSTABLE */
#define _SDIO_CLOCKCTRL_INTCLKSTABLE_MASK              0x2UL                                          /**< Bit mask for SDIO_INTCLKSTABLE */
#define _SDIO_CLOCKCTRL_INTCLKSTABLE_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_INTCLKSTABLE_DEFAULT            (_SDIO_CLOCKCTRL_INTCLKSTABLE_DEFAULT << 1)    /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SDCLKEN                         (0x1UL << 2)                                   /**< SDIO_CLK Pin Clock Enable */
#define _SDIO_CLOCKCTRL_SDCLKEN_SHIFT                  2                                              /**< Shift value for SDIO_SDCLKEN */
#define _SDIO_CLOCKCTRL_SDCLKEN_MASK                   0x4UL                                          /**< Bit mask for SDIO_SDCLKEN */
#define _SDIO_CLOCKCTRL_SDCLKEN_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SDCLKEN_DEFAULT                 (_SDIO_CLOCKCTRL_SDCLKEN_DEFAULT << 2)         /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_CLKGENSEL                       (0x1UL << 5)                                   /**< Clock Generator Select */
#define _SDIO_CLOCKCTRL_CLKGENSEL_SHIFT                5                                              /**< Shift value for SDIO_CLKGENSEL */
#define _SDIO_CLOCKCTRL_CLKGENSEL_MASK                 0x20UL                                         /**< Bit mask for SDIO_CLKGENSEL */
#define _SDIO_CLOCKCTRL_CLKGENSEL_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_CLKGENSEL_DEFAULT               (_SDIO_CLOCKCTRL_CLKGENSEL_DEFAULT << 5)       /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define _SDIO_CLOCKCTRL_UPPSDCLKFRE_SHIFT              6                                              /**< Shift value for SDIO_UPPSDCLKFRE */
#define _SDIO_CLOCKCTRL_UPPSDCLKFRE_MASK               0xC0UL                                         /**< Bit mask for SDIO_UPPSDCLKFRE */
#define _SDIO_CLOCKCTRL_UPPSDCLKFRE_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_UPPSDCLKFRE_DEFAULT             (_SDIO_CLOCKCTRL_UPPSDCLKFRE_DEFAULT << 6)     /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define _SDIO_CLOCKCTRL_SDCLKFREQSEL_SHIFT             8                                              /**< Shift value for SDIO_SDCLKFREQSEL */
#define _SDIO_CLOCKCTRL_SDCLKFREQSEL_MASK              0xFF00UL                                       /**< Bit mask for SDIO_SDCLKFREQSEL */
#define _SDIO_CLOCKCTRL_SDCLKFREQSEL_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define _SDIO_CLOCKCTRL_SDCLKFREQSEL_NODIVISION        0x00000000UL                                   /**< Mode NODIVISION for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SDCLKFREQSEL_DEFAULT            (_SDIO_CLOCKCTRL_SDCLKFREQSEL_DEFAULT << 8)    /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SDCLKFREQSEL_NODIVISION         (_SDIO_CLOCKCTRL_SDCLKFREQSEL_NODIVISION << 8) /**< Shifted mode NODIVISION for SDIO_CLOCKCTRL */
#define _SDIO_CLOCKCTRL_DATTOUTCNTVAL_SHIFT            16                                             /**< Shift value for SDIO_DATTOUTCNTVAL */
#define _SDIO_CLOCKCTRL_DATTOUTCNTVAL_MASK             0xF0000UL                                      /**< Bit mask for SDIO_DATTOUTCNTVAL */
#define _SDIO_CLOCKCTRL_DATTOUTCNTVAL_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_DATTOUTCNTVAL_DEFAULT           (_SDIO_CLOCKCTRL_DATTOUTCNTVAL_DEFAULT << 16)  /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SFTRSTA                         (0x1UL << 24)                                  /**< Software Reset for All */
#define _SDIO_CLOCKCTRL_SFTRSTA_SHIFT                  24                                             /**< Shift value for SDIO_SFTRSTA */
#define _SDIO_CLOCKCTRL_SFTRSTA_MASK                   0x1000000UL                                    /**< Bit mask for SDIO_SFTRSTA */
#define _SDIO_CLOCKCTRL_SFTRSTA_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SFTRSTA_DEFAULT                 (_SDIO_CLOCKCTRL_SFTRSTA_DEFAULT << 24)        /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SFTRSTCMD                       (0x1UL << 25)                                  /**< Software Reset for CMD Line */
#define _SDIO_CLOCKCTRL_SFTRSTCMD_SHIFT                25                                             /**< Shift value for SDIO_SFTRSTCMD */
#define _SDIO_CLOCKCTRL_SFTRSTCMD_MASK                 0x2000000UL                                    /**< Bit mask for SDIO_SFTRSTCMD */
#define _SDIO_CLOCKCTRL_SFTRSTCMD_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SFTRSTCMD_DEFAULT               (_SDIO_CLOCKCTRL_SFTRSTCMD_DEFAULT << 25)      /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SFTRSTDAT                       (0x1UL << 26)                                  /**< Software Reset for DAT Line */
#define _SDIO_CLOCKCTRL_SFTRSTDAT_SHIFT                26                                             /**< Shift value for SDIO_SFTRSTDAT */
#define _SDIO_CLOCKCTRL_SFTRSTDAT_MASK                 0x4000000UL                                    /**< Bit mask for SDIO_SFTRSTDAT */
#define _SDIO_CLOCKCTRL_SFTRSTDAT_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for SDIO_CLOCKCTRL */
#define SDIO_CLOCKCTRL_SFTRSTDAT_DEFAULT               (_SDIO_CLOCKCTRL_SFTRSTDAT_DEFAULT << 26)      /**< Shifted mode DEFAULT for SDIO_CLOCKCTRL */

/* Bit fields for SDIO IFCR */
#define _SDIO_IFCR_RESETVALUE                          0x00000000UL                               /**< Default value for SDIO_IFCR */
#define _SDIO_IFCR_MASK                                0x13FFF1FFUL                               /**< Mask for SDIO_IFCR */
#define SDIO_IFCR_CMDCOM                               (0x1UL << 0)                               /**< Command Complete */
#define _SDIO_IFCR_CMDCOM_SHIFT                        0                                          /**< Shift value for SDIO_CMDCOM */
#define _SDIO_IFCR_CMDCOM_MASK                         0x1UL                                      /**< Bit mask for SDIO_CMDCOM */
#define _SDIO_IFCR_CMDCOM_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDCOM_DEFAULT                       (_SDIO_IFCR_CMDCOM_DEFAULT << 0)           /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_TRANCOM                              (0x1UL << 1)                               /**< Transfer Complete */
#define _SDIO_IFCR_TRANCOM_SHIFT                       1                                          /**< Shift value for SDIO_TRANCOM */
#define _SDIO_IFCR_TRANCOM_MASK                        0x2UL                                      /**< Bit mask for SDIO_TRANCOM */
#define _SDIO_IFCR_TRANCOM_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_TRANCOM_DEFAULT                      (_SDIO_IFCR_TRANCOM_DEFAULT << 1)          /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BLKGAPEVT                            (0x1UL << 2)                               /**< Block Gap Event */
#define _SDIO_IFCR_BLKGAPEVT_SHIFT                     2                                          /**< Shift value for SDIO_BLKGAPEVT */
#define _SDIO_IFCR_BLKGAPEVT_MASK                      0x4UL                                      /**< Bit mask for SDIO_BLKGAPEVT */
#define _SDIO_IFCR_BLKGAPEVT_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BLKGAPEVT_DEFAULT                    (_SDIO_IFCR_BLKGAPEVT_DEFAULT << 2)        /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DMAINT                               (0x1UL << 3)                               /**< DMA Interrupt */
#define _SDIO_IFCR_DMAINT_SHIFT                        3                                          /**< Shift value for SDIO_DMAINT */
#define _SDIO_IFCR_DMAINT_MASK                         0x8UL                                      /**< Bit mask for SDIO_DMAINT */
#define _SDIO_IFCR_DMAINT_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DMAINT_DEFAULT                       (_SDIO_IFCR_DMAINT_DEFAULT << 3)           /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BFRWRRDY                             (0x1UL << 4)                               /**< Buffer Write Ready */
#define _SDIO_IFCR_BFRWRRDY_SHIFT                      4                                          /**< Shift value for SDIO_BFRWRRDY */
#define _SDIO_IFCR_BFRWRRDY_MASK                       0x10UL                                     /**< Bit mask for SDIO_BFRWRRDY */
#define _SDIO_IFCR_BFRWRRDY_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BFRWRRDY_DEFAULT                     (_SDIO_IFCR_BFRWRRDY_DEFAULT << 4)         /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BFRRDRDY                             (0x1UL << 5)                               /**< Buffer Read Ready */
#define _SDIO_IFCR_BFRRDRDY_SHIFT                      5                                          /**< Shift value for SDIO_BFRRDRDY */
#define _SDIO_IFCR_BFRRDRDY_MASK                       0x20UL                                     /**< Bit mask for SDIO_BFRRDRDY */
#define _SDIO_IFCR_BFRRDRDY_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BFRRDRDY_DEFAULT                     (_SDIO_IFCR_BFRRDRDY_DEFAULT << 5)         /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CARDINS                              (0x1UL << 6)                               /**< Card Insertion */
#define _SDIO_IFCR_CARDINS_SHIFT                       6                                          /**< Shift value for SDIO_CARDINS */
#define _SDIO_IFCR_CARDINS_MASK                        0x40UL                                     /**< Bit mask for SDIO_CARDINS */
#define _SDIO_IFCR_CARDINS_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CARDINS_DEFAULT                      (_SDIO_IFCR_CARDINS_DEFAULT << 6)          /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CARDRM                               (0x1UL << 7)                               /**< Card Removal */
#define _SDIO_IFCR_CARDRM_SHIFT                        7                                          /**< Shift value for SDIO_CARDRM */
#define _SDIO_IFCR_CARDRM_MASK                         0x80UL                                     /**< Bit mask for SDIO_CARDRM */
#define _SDIO_IFCR_CARDRM_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CARDRM_DEFAULT                       (_SDIO_IFCR_CARDRM_DEFAULT << 7)           /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CARDINT                              (0x1UL << 8)                               /**< Card Interrupt */
#define _SDIO_IFCR_CARDINT_SHIFT                       8                                          /**< Shift value for SDIO_CARDINT */
#define _SDIO_IFCR_CARDINT_MASK                        0x100UL                                    /**< Bit mask for SDIO_CARDINT */
#define _SDIO_IFCR_CARDINT_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CARDINT_DEFAULT                      (_SDIO_IFCR_CARDINT_DEFAULT << 8)          /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_RETUNINGEVT                          (0x1UL << 12)                              /**< Re-Tunning Event */
#define _SDIO_IFCR_RETUNINGEVT_SHIFT                   12                                         /**< Shift value for SDIO_RETUNINGEVT */
#define _SDIO_IFCR_RETUNINGEVT_MASK                    0x1000UL                                   /**< Bit mask for SDIO_RETUNINGEVT */
#define _SDIO_IFCR_RETUNINGEVT_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_RETUNINGEVT_DEFAULT                  (_SDIO_IFCR_RETUNINGEVT_DEFAULT << 12)     /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BOOTACKRCV                           (0x1UL << 13)                              /**< Boot Ack Received */
#define _SDIO_IFCR_BOOTACKRCV_SHIFT                    13                                         /**< Shift value for SDIO_BOOTACKRCV */
#define _SDIO_IFCR_BOOTACKRCV_MASK                     0x2000UL                                   /**< Bit mask for SDIO_BOOTACKRCV */
#define _SDIO_IFCR_BOOTACKRCV_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BOOTACKRCV_DEFAULT                   (_SDIO_IFCR_BOOTACKRCV_DEFAULT << 13)      /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BOOTTERMINATE                        (0x1UL << 14)                              /**< Boot Terminate Interrupt */
#define _SDIO_IFCR_BOOTTERMINATE_SHIFT                 14                                         /**< Shift value for SDIO_BOOTTERMINATE */
#define _SDIO_IFCR_BOOTTERMINATE_MASK                  0x4000UL                                   /**< Bit mask for SDIO_BOOTTERMINATE */
#define _SDIO_IFCR_BOOTTERMINATE_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_BOOTTERMINATE_DEFAULT                (_SDIO_IFCR_BOOTTERMINATE_DEFAULT << 14)   /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_ERRINT                               (0x1UL << 15)                              /**< Error Interrupt */
#define _SDIO_IFCR_ERRINT_SHIFT                        15                                         /**< Shift value for SDIO_ERRINT */
#define _SDIO_IFCR_ERRINT_MASK                         0x8000UL                                   /**< Bit mask for SDIO_ERRINT */
#define _SDIO_IFCR_ERRINT_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_ERRINT_DEFAULT                       (_SDIO_IFCR_ERRINT_DEFAULT << 15)          /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDTOUTERR                           (0x1UL << 16)                              /**< Command Timeout Error */
#define _SDIO_IFCR_CMDTOUTERR_SHIFT                    16                                         /**< Shift value for SDIO_CMDTOUTERR */
#define _SDIO_IFCR_CMDTOUTERR_MASK                     0x10000UL                                  /**< Bit mask for SDIO_CMDTOUTERR */
#define _SDIO_IFCR_CMDTOUTERR_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDTOUTERR_DEFAULT                   (_SDIO_IFCR_CMDTOUTERR_DEFAULT << 16)      /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDCRCERR                            (0x1UL << 17)                              /**< CMD CRC Error */
#define _SDIO_IFCR_CMDCRCERR_SHIFT                     17                                         /**< Shift value for SDIO_CMDCRCERR */
#define _SDIO_IFCR_CMDCRCERR_MASK                      0x20000UL                                  /**< Bit mask for SDIO_CMDCRCERR */
#define _SDIO_IFCR_CMDCRCERR_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDCRCERR_DEFAULT                    (_SDIO_IFCR_CMDCRCERR_DEFAULT << 17)       /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDENDBITERR                         (0x1UL << 18)                              /**< Command End Bit Error */
#define _SDIO_IFCR_CMDENDBITERR_SHIFT                  18                                         /**< Shift value for SDIO_CMDENDBITERR */
#define _SDIO_IFCR_CMDENDBITERR_MASK                   0x40000UL                                  /**< Bit mask for SDIO_CMDENDBITERR */
#define _SDIO_IFCR_CMDENDBITERR_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDENDBITERR_DEFAULT                 (_SDIO_IFCR_CMDENDBITERR_DEFAULT << 18)    /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDINDEXERR                          (0x1UL << 19)                              /**< Command Index Error */
#define _SDIO_IFCR_CMDINDEXERR_SHIFT                   19                                         /**< Shift value for SDIO_CMDINDEXERR */
#define _SDIO_IFCR_CMDINDEXERR_MASK                    0x80000UL                                  /**< Bit mask for SDIO_CMDINDEXERR */
#define _SDIO_IFCR_CMDINDEXERR_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CMDINDEXERR_DEFAULT                  (_SDIO_IFCR_CMDINDEXERR_DEFAULT << 19)     /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DATTOUTERR                           (0x1UL << 20)                              /**< Data Time-out Error */
#define _SDIO_IFCR_DATTOUTERR_SHIFT                    20                                         /**< Shift value for SDIO_DATTOUTERR */
#define _SDIO_IFCR_DATTOUTERR_MASK                     0x100000UL                                 /**< Bit mask for SDIO_DATTOUTERR */
#define _SDIO_IFCR_DATTOUTERR_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DATTOUTERR_DEFAULT                   (_SDIO_IFCR_DATTOUTERR_DEFAULT << 20)      /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DATCRCERR                            (0x1UL << 21)                              /**< Data CRC Error */
#define _SDIO_IFCR_DATCRCERR_SHIFT                     21                                         /**< Shift value for SDIO_DATCRCERR */
#define _SDIO_IFCR_DATCRCERR_MASK                      0x200000UL                                 /**< Bit mask for SDIO_DATCRCERR */
#define _SDIO_IFCR_DATCRCERR_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DATCRCERR_DEFAULT                    (_SDIO_IFCR_DATCRCERR_DEFAULT << 21)       /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DATENDBITERR                         (0x1UL << 22)                              /**< Data End Bit Error */
#define _SDIO_IFCR_DATENDBITERR_SHIFT                  22                                         /**< Shift value for SDIO_DATENDBITERR */
#define _SDIO_IFCR_DATENDBITERR_MASK                   0x400000UL                                 /**< Bit mask for SDIO_DATENDBITERR */
#define _SDIO_IFCR_DATENDBITERR_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_DATENDBITERR_DEFAULT                 (_SDIO_IFCR_DATENDBITERR_DEFAULT << 22)    /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CURRENTLIMITERR                      (0x1UL << 23)                              /**< Current Limit Error */
#define _SDIO_IFCR_CURRENTLIMITERR_SHIFT               23                                         /**< Shift value for SDIO_CURRENTLIMITERR */
#define _SDIO_IFCR_CURRENTLIMITERR_MASK                0x800000UL                                 /**< Bit mask for SDIO_CURRENTLIMITERR */
#define _SDIO_IFCR_CURRENTLIMITERR_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_CURRENTLIMITERR_DEFAULT              (_SDIO_IFCR_CURRENTLIMITERR_DEFAULT << 23) /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_AUTOCMDERR                           (0x1UL << 24)                              /**< Auto CMD Error */
#define _SDIO_IFCR_AUTOCMDERR_SHIFT                    24                                         /**< Shift value for SDIO_AUTOCMDERR */
#define _SDIO_IFCR_AUTOCMDERR_MASK                     0x1000000UL                                /**< Bit mask for SDIO_AUTOCMDERR */
#define _SDIO_IFCR_AUTOCMDERR_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_AUTOCMDERR_DEFAULT                   (_SDIO_IFCR_AUTOCMDERR_DEFAULT << 24)      /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_ADMAERR                              (0x1UL << 25)                              /**< ADMA Error */
#define _SDIO_IFCR_ADMAERR_SHIFT                       25                                         /**< Shift value for SDIO_ADMAERR */
#define _SDIO_IFCR_ADMAERR_MASK                        0x2000000UL                                /**< Bit mask for SDIO_ADMAERR */
#define _SDIO_IFCR_ADMAERR_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_ADMAERR_DEFAULT                      (_SDIO_IFCR_ADMAERR_DEFAULT << 25)         /**< Shifted mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_TARGETRESP                           (0x1UL << 28)                              /**<  Specific  Error STAT */
#define _SDIO_IFCR_TARGETRESP_SHIFT                    28                                         /**< Shift value for SDIO_TARGETRESP */
#define _SDIO_IFCR_TARGETRESP_MASK                     0x10000000UL                               /**< Bit mask for SDIO_TARGETRESP */
#define _SDIO_IFCR_TARGETRESP_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for SDIO_IFCR */
#define SDIO_IFCR_TARGETRESP_DEFAULT                   (_SDIO_IFCR_TARGETRESP_DEFAULT << 28)      /**< Shifted mode DEFAULT for SDIO_IFCR */

/* Bit fields for SDIO IFENC */
#define _SDIO_IFENC_RESETVALUE                         0x00000000UL                                  /**< Default value for SDIO_IFENC */
#define _SDIO_IFENC_MASK                               0x17FF71FFUL                                  /**< Mask for SDIO_IFENC */
#define SDIO_IFENC_CMDCOMEN                            (0x1UL << 0)                                  /**< Command Complete Signal Enable */
#define _SDIO_IFENC_CMDCOMEN_SHIFT                     0                                             /**< Shift value for SDIO_CMDCOMEN */
#define _SDIO_IFENC_CMDCOMEN_MASK                      0x1UL                                         /**< Bit mask for SDIO_CMDCOMEN */
#define _SDIO_IFENC_CMDCOMEN_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDCOMEN_DEFAULT                    (_SDIO_IFENC_CMDCOMEN_DEFAULT << 0)           /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_TRANCOMEN                           (0x1UL << 1)                                  /**< Transfer Complete Signal Enable */
#define _SDIO_IFENC_TRANCOMEN_SHIFT                    1                                             /**< Shift value for SDIO_TRANCOMEN */
#define _SDIO_IFENC_TRANCOMEN_MASK                     0x2UL                                         /**< Bit mask for SDIO_TRANCOMEN */
#define _SDIO_IFENC_TRANCOMEN_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_TRANCOMEN_DEFAULT                   (_SDIO_IFENC_TRANCOMEN_DEFAULT << 1)          /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BLKGAPEVTEN                         (0x1UL << 2)                                  /**< Block Gap Event Signal Enable */
#define _SDIO_IFENC_BLKGAPEVTEN_SHIFT                  2                                             /**< Shift value for SDIO_BLKGAPEVTEN */
#define _SDIO_IFENC_BLKGAPEVTEN_MASK                   0x4UL                                         /**< Bit mask for SDIO_BLKGAPEVTEN */
#define _SDIO_IFENC_BLKGAPEVTEN_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BLKGAPEVTEN_DEFAULT                 (_SDIO_IFENC_BLKGAPEVTEN_DEFAULT << 2)        /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DMAINTEN                            (0x1UL << 3)                                  /**< DMA Interrupt Signal Enable */
#define _SDIO_IFENC_DMAINTEN_SHIFT                     3                                             /**< Shift value for SDIO_DMAINTEN */
#define _SDIO_IFENC_DMAINTEN_MASK                      0x8UL                                         /**< Bit mask for SDIO_DMAINTEN */
#define _SDIO_IFENC_DMAINTEN_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DMAINTEN_DEFAULT                    (_SDIO_IFENC_DMAINTEN_DEFAULT << 3)           /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BUFWRRDYEN                          (0x1UL << 4)                                  /**< Buffer Write Ready Signal Enable */
#define _SDIO_IFENC_BUFWRRDYEN_SHIFT                   4                                             /**< Shift value for SDIO_BUFWRRDYEN */
#define _SDIO_IFENC_BUFWRRDYEN_MASK                    0x10UL                                        /**< Bit mask for SDIO_BUFWRRDYEN */
#define _SDIO_IFENC_BUFWRRDYEN_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BUFWRRDYEN_DEFAULT                  (_SDIO_IFENC_BUFWRRDYEN_DEFAULT << 4)         /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BUFRDRDYEN                          (0x1UL << 5)                                  /**< Buffer Read Ready Signal Enable */
#define _SDIO_IFENC_BUFRDRDYEN_SHIFT                   5                                             /**< Shift value for SDIO_BUFRDRDYEN */
#define _SDIO_IFENC_BUFRDRDYEN_MASK                    0x20UL                                        /**< Bit mask for SDIO_BUFRDRDYEN */
#define _SDIO_IFENC_BUFRDRDYEN_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BUFRDRDYEN_DEFAULT                  (_SDIO_IFENC_BUFRDRDYEN_DEFAULT << 5)         /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CARDINSEN                           (0x1UL << 6)                                  /**< Card Insertion Signal Enable */
#define _SDIO_IFENC_CARDINSEN_SHIFT                    6                                             /**< Shift value for SDIO_CARDINSEN */
#define _SDIO_IFENC_CARDINSEN_MASK                     0x40UL                                        /**< Bit mask for SDIO_CARDINSEN */
#define _SDIO_IFENC_CARDINSEN_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CARDINSEN_DEFAULT                   (_SDIO_IFENC_CARDINSEN_DEFAULT << 6)          /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CARDRMEN                            (0x1UL << 7)                                  /**< Card Removal Signal Enable */
#define _SDIO_IFENC_CARDRMEN_SHIFT                     7                                             /**< Shift value for SDIO_CARDRMEN */
#define _SDIO_IFENC_CARDRMEN_MASK                      0x80UL                                        /**< Bit mask for SDIO_CARDRMEN */
#define _SDIO_IFENC_CARDRMEN_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CARDRMEN_DEFAULT                    (_SDIO_IFENC_CARDRMEN_DEFAULT << 7)           /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CARDINTEN                           (0x1UL << 8)                                  /**< Card Interrupt Signal Enable */
#define _SDIO_IFENC_CARDINTEN_SHIFT                    8                                             /**< Shift value for SDIO_CARDINTEN */
#define _SDIO_IFENC_CARDINTEN_MASK                     0x100UL                                       /**< Bit mask for SDIO_CARDINTEN */
#define _SDIO_IFENC_CARDINTEN_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CARDINTEN_DEFAULT                   (_SDIO_IFENC_CARDINTEN_DEFAULT << 8)          /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_RETUNINGEVTEN                       (0x1UL << 12)                                 /**< Re-Tunning Event Signal Enable */
#define _SDIO_IFENC_RETUNINGEVTEN_SHIFT                12                                            /**< Shift value for SDIO_RETUNINGEVTEN */
#define _SDIO_IFENC_RETUNINGEVTEN_MASK                 0x1000UL                                      /**< Bit mask for SDIO_RETUNINGEVTEN */
#define _SDIO_IFENC_RETUNINGEVTEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_RETUNINGEVTEN_DEFAULT               (_SDIO_IFENC_RETUNINGEVTEN_DEFAULT << 12)     /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BOOTACKRCVEN                        (0x1UL << 13)                                 /**< Boot Ack Received Signal Enable */
#define _SDIO_IFENC_BOOTACKRCVEN_SHIFT                 13                                            /**< Shift value for SDIO_BOOTACKRCVEN */
#define _SDIO_IFENC_BOOTACKRCVEN_MASK                  0x2000UL                                      /**< Bit mask for SDIO_BOOTACKRCVEN */
#define _SDIO_IFENC_BOOTACKRCVEN_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BOOTACKRCVEN_DEFAULT                (_SDIO_IFENC_BOOTACKRCVEN_DEFAULT << 13)      /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BOOTTERMINATEEN                     (0x1UL << 14)                                 /**< Boot Terminate Interrupt Signal Enable */
#define _SDIO_IFENC_BOOTTERMINATEEN_SHIFT              14                                            /**< Shift value for SDIO_BOOTTERMINATEEN */
#define _SDIO_IFENC_BOOTTERMINATEEN_MASK               0x4000UL                                      /**< Bit mask for SDIO_BOOTTERMINATEEN */
#define _SDIO_IFENC_BOOTTERMINATEEN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_BOOTTERMINATEEN_DEFAULT             (_SDIO_IFENC_BOOTTERMINATEEN_DEFAULT << 14)   /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDTOUTERREN                        (0x1UL << 16)                                 /**< Command Time-out Error Status Enable */
#define _SDIO_IFENC_CMDTOUTERREN_SHIFT                 16                                            /**< Shift value for SDIO_CMDTOUTERREN */
#define _SDIO_IFENC_CMDTOUTERREN_MASK                  0x10000UL                                     /**< Bit mask for SDIO_CMDTOUTERREN */
#define _SDIO_IFENC_CMDTOUTERREN_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDTOUTERREN_DEFAULT                (_SDIO_IFENC_CMDTOUTERREN_DEFAULT << 16)      /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDCRCERREN                         (0x1UL << 17)                                 /**< Command CRC Error Status Enable */
#define _SDIO_IFENC_CMDCRCERREN_SHIFT                  17                                            /**< Shift value for SDIO_CMDCRCERREN */
#define _SDIO_IFENC_CMDCRCERREN_MASK                   0x20000UL                                     /**< Bit mask for SDIO_CMDCRCERREN */
#define _SDIO_IFENC_CMDCRCERREN_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDCRCERREN_DEFAULT                 (_SDIO_IFENC_CMDCRCERREN_DEFAULT << 17)       /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDENDBITERREN                      (0x1UL << 18)                                 /**< Command End Bit Error Status Enable */
#define _SDIO_IFENC_CMDENDBITERREN_SHIFT               18                                            /**< Shift value for SDIO_CMDENDBITERREN */
#define _SDIO_IFENC_CMDENDBITERREN_MASK                0x40000UL                                     /**< Bit mask for SDIO_CMDENDBITERREN */
#define _SDIO_IFENC_CMDENDBITERREN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDENDBITERREN_DEFAULT              (_SDIO_IFENC_CMDENDBITERREN_DEFAULT << 18)    /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDINDEXERREN                       (0x1UL << 19)                                 /**< Command Index Error Status Enable */
#define _SDIO_IFENC_CMDINDEXERREN_SHIFT                19                                            /**< Shift value for SDIO_CMDINDEXERREN */
#define _SDIO_IFENC_CMDINDEXERREN_MASK                 0x80000UL                                     /**< Bit mask for SDIO_CMDINDEXERREN */
#define _SDIO_IFENC_CMDINDEXERREN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CMDINDEXERREN_DEFAULT               (_SDIO_IFENC_CMDINDEXERREN_DEFAULT << 19)     /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DATTOUTERREN                        (0x1UL << 20)                                 /**< Data Timeout Error Status Enable */
#define _SDIO_IFENC_DATTOUTERREN_SHIFT                 20                                            /**< Shift value for SDIO_DATTOUTERREN */
#define _SDIO_IFENC_DATTOUTERREN_MASK                  0x100000UL                                    /**< Bit mask for SDIO_DATTOUTERREN */
#define _SDIO_IFENC_DATTOUTERREN_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DATTOUTERREN_DEFAULT                (_SDIO_IFENC_DATTOUTERREN_DEFAULT << 20)      /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DATCRCERREN                         (0x1UL << 21)                                 /**< Data CRC Error Status Enable */
#define _SDIO_IFENC_DATCRCERREN_SHIFT                  21                                            /**< Shift value for SDIO_DATCRCERREN */
#define _SDIO_IFENC_DATCRCERREN_MASK                   0x200000UL                                    /**< Bit mask for SDIO_DATCRCERREN */
#define _SDIO_IFENC_DATCRCERREN_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DATCRCERREN_DEFAULT                 (_SDIO_IFENC_DATCRCERREN_DEFAULT << 21)       /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DATENDBITERREN                      (0x1UL << 22)                                 /**< Data End Bit Error Status Enable */
#define _SDIO_IFENC_DATENDBITERREN_SHIFT               22                                            /**< Shift value for SDIO_DATENDBITERREN */
#define _SDIO_IFENC_DATENDBITERREN_MASK                0x400000UL                                    /**< Bit mask for SDIO_DATENDBITERREN */
#define _SDIO_IFENC_DATENDBITERREN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_DATENDBITERREN_DEFAULT              (_SDIO_IFENC_DATENDBITERREN_DEFAULT << 22)    /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CURRENTLIMITERREN                   (0x1UL << 23)                                 /**< Current Limit Error Status Enable */
#define _SDIO_IFENC_CURRENTLIMITERREN_SHIFT            23                                            /**< Shift value for SDIO_CURRENTLIMITERREN */
#define _SDIO_IFENC_CURRENTLIMITERREN_MASK             0x800000UL                                    /**< Bit mask for SDIO_CURRENTLIMITERREN */
#define _SDIO_IFENC_CURRENTLIMITERREN_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_CURRENTLIMITERREN_DEFAULT           (_SDIO_IFENC_CURRENTLIMITERREN_DEFAULT << 23) /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_AUTOCMDERREN                        (0x1UL << 24)                                 /**< Auto CMD12 Error Status Enable */
#define _SDIO_IFENC_AUTOCMDERREN_SHIFT                 24                                            /**< Shift value for SDIO_AUTOCMDERREN */
#define _SDIO_IFENC_AUTOCMDERREN_MASK                  0x1000000UL                                   /**< Bit mask for SDIO_AUTOCMDERREN */
#define _SDIO_IFENC_AUTOCMDERREN_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_AUTOCMDERREN_DEFAULT                (_SDIO_IFENC_AUTOCMDERREN_DEFAULT << 24)      /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_ADMAERREN                           (0x1UL << 25)                                 /**< ADMA Error Status Enable */
#define _SDIO_IFENC_ADMAERREN_SHIFT                    25                                            /**< Shift value for SDIO_ADMAERREN */
#define _SDIO_IFENC_ADMAERREN_MASK                     0x2000000UL                                   /**< Bit mask for SDIO_ADMAERREN */
#define _SDIO_IFENC_ADMAERREN_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_ADMAERREN_DEFAULT                   (_SDIO_IFENC_ADMAERREN_DEFAULT << 25)         /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_TUNINGERREN                         (0x1UL << 26)                                 /**< Tuning Error Status Enable */
#define _SDIO_IFENC_TUNINGERREN_SHIFT                  26                                            /**< Shift value for SDIO_TUNINGERREN */
#define _SDIO_IFENC_TUNINGERREN_MASK                   0x4000000UL                                   /**< Bit mask for SDIO_TUNINGERREN */
#define _SDIO_IFENC_TUNINGERREN_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_TUNINGERREN_DEFAULT                 (_SDIO_IFENC_TUNINGERREN_DEFAULT << 26)       /**< Shifted mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_TARGETRESPEN                        (0x1UL << 28)                                 /**< Target Response/Host Error Status Enable */
#define _SDIO_IFENC_TARGETRESPEN_SHIFT                 28                                            /**< Shift value for SDIO_TARGETRESPEN */
#define _SDIO_IFENC_TARGETRESPEN_MASK                  0x10000000UL                                  /**< Bit mask for SDIO_TARGETRESPEN */
#define _SDIO_IFENC_TARGETRESPEN_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for SDIO_IFENC */
#define SDIO_IFENC_TARGETRESPEN_DEFAULT                (_SDIO_IFENC_TARGETRESPEN_DEFAULT << 28)      /**< Shifted mode DEFAULT for SDIO_IFENC */

/* Bit fields for SDIO IEN */
#define _SDIO_IEN_RESETVALUE                           0x00000000UL                                    /**< Default value for SDIO_IEN */
#define _SDIO_IEN_MASK                                 0x17FF71FFUL                                    /**< Mask for SDIO_IEN */
#define SDIO_IEN_CMDCOMSEN                             (0x1UL << 0)                                    /**< Command Complete Signal Enable */
#define _SDIO_IEN_CMDCOMSEN_SHIFT                      0                                               /**< Shift value for SDIO_CMDCOMSEN */
#define _SDIO_IEN_CMDCOMSEN_MASK                       0x1UL                                           /**< Bit mask for SDIO_CMDCOMSEN */
#define _SDIO_IEN_CMDCOMSEN_DEFAULT                    0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDCOMSEN_DEFAULT                     (_SDIO_IEN_CMDCOMSEN_DEFAULT << 0)              /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_TRANCOMSEN                            (0x1UL << 1)                                    /**< Transfer Complete Signal Enable */
#define _SDIO_IEN_TRANCOMSEN_SHIFT                     1                                               /**< Shift value for SDIO_TRANCOMSEN */
#define _SDIO_IEN_TRANCOMSEN_MASK                      0x2UL                                           /**< Bit mask for SDIO_TRANCOMSEN */
#define _SDIO_IEN_TRANCOMSEN_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_TRANCOMSEN_DEFAULT                    (_SDIO_IEN_TRANCOMSEN_DEFAULT << 1)             /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BLKGAPEVTSEN                          (0x1UL << 2)                                    /**< Block Gap Event Signal Enable */
#define _SDIO_IEN_BLKGAPEVTSEN_SHIFT                   2                                               /**< Shift value for SDIO_BLKGAPEVTSEN */
#define _SDIO_IEN_BLKGAPEVTSEN_MASK                    0x4UL                                           /**< Bit mask for SDIO_BLKGAPEVTSEN */
#define _SDIO_IEN_BLKGAPEVTSEN_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BLKGAPEVTSEN_DEFAULT                  (_SDIO_IEN_BLKGAPEVTSEN_DEFAULT << 2)           /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DMAINTSEN                             (0x1UL << 3)                                    /**< DMA Interrupt Signal Enable */
#define _SDIO_IEN_DMAINTSEN_SHIFT                      3                                               /**< Shift value for SDIO_DMAINTSEN */
#define _SDIO_IEN_DMAINTSEN_MASK                       0x8UL                                           /**< Bit mask for SDIO_DMAINTSEN */
#define _SDIO_IEN_DMAINTSEN_DEFAULT                    0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DMAINTSEN_DEFAULT                     (_SDIO_IEN_DMAINTSEN_DEFAULT << 3)              /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BUFWRRDYSEN                           (0x1UL << 4)                                    /**< Buffer Write Ready Signal Enable */
#define _SDIO_IEN_BUFWRRDYSEN_SHIFT                    4                                               /**< Shift value for SDIO_BUFWRRDYSEN */
#define _SDIO_IEN_BUFWRRDYSEN_MASK                     0x10UL                                          /**< Bit mask for SDIO_BUFWRRDYSEN */
#define _SDIO_IEN_BUFWRRDYSEN_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BUFWRRDYSEN_DEFAULT                   (_SDIO_IEN_BUFWRRDYSEN_DEFAULT << 4)            /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BUFRDRDYSEN                           (0x1UL << 5)                                    /**< Buffer Read Ready Signal Enable */
#define _SDIO_IEN_BUFRDRDYSEN_SHIFT                    5                                               /**< Shift value for SDIO_BUFRDRDYSEN */
#define _SDIO_IEN_BUFRDRDYSEN_MASK                     0x20UL                                          /**< Bit mask for SDIO_BUFRDRDYSEN */
#define _SDIO_IEN_BUFRDRDYSEN_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BUFRDRDYSEN_DEFAULT                   (_SDIO_IEN_BUFRDRDYSEN_DEFAULT << 5)            /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CARDINSSEN                            (0x1UL << 6)                                    /**< Card Insertion Signal Enable */
#define _SDIO_IEN_CARDINSSEN_SHIFT                     6                                               /**< Shift value for SDIO_CARDINSSEN */
#define _SDIO_IEN_CARDINSSEN_MASK                      0x40UL                                          /**< Bit mask for SDIO_CARDINSSEN */
#define _SDIO_IEN_CARDINSSEN_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CARDINSSEN_DEFAULT                    (_SDIO_IEN_CARDINSSEN_DEFAULT << 6)             /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CARDREMSEN                            (0x1UL << 7)                                    /**< Card Removal Signal Enable */
#define _SDIO_IEN_CARDREMSEN_SHIFT                     7                                               /**< Shift value for SDIO_CARDREMSEN */
#define _SDIO_IEN_CARDREMSEN_MASK                      0x80UL                                          /**< Bit mask for SDIO_CARDREMSEN */
#define _SDIO_IEN_CARDREMSEN_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CARDREMSEN_DEFAULT                    (_SDIO_IEN_CARDREMSEN_DEFAULT << 7)             /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CARDINTSEN                            (0x1UL << 8)                                    /**< Card Interrupt Signal Enable */
#define _SDIO_IEN_CARDINTSEN_SHIFT                     8                                               /**< Shift value for SDIO_CARDINTSEN */
#define _SDIO_IEN_CARDINTSEN_MASK                      0x100UL                                         /**< Bit mask for SDIO_CARDINTSEN */
#define _SDIO_IEN_CARDINTSEN_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CARDINTSEN_DEFAULT                    (_SDIO_IEN_CARDINTSEN_DEFAULT << 8)             /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_RETUNINGEVTSEN                        (0x1UL << 12)                                   /**< Re-Tuning Event Signal Enable */
#define _SDIO_IEN_RETUNINGEVTSEN_SHIFT                 12                                              /**< Shift value for SDIO_RETUNINGEVTSEN */
#define _SDIO_IEN_RETUNINGEVTSEN_MASK                  0x1000UL                                        /**< Bit mask for SDIO_RETUNINGEVTSEN */
#define _SDIO_IEN_RETUNINGEVTSEN_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_RETUNINGEVTSEN_DEFAULT                (_SDIO_IEN_RETUNINGEVTSEN_DEFAULT << 12)        /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BOOTACKRCVSEN                         (0x1UL << 13)                                   /**< Boot Ack Received Signal Enable */
#define _SDIO_IEN_BOOTACKRCVSEN_SHIFT                  13                                              /**< Shift value for SDIO_BOOTACKRCVSEN */
#define _SDIO_IEN_BOOTACKRCVSEN_MASK                   0x2000UL                                        /**< Bit mask for SDIO_BOOTACKRCVSEN */
#define _SDIO_IEN_BOOTACKRCVSEN_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BOOTACKRCVSEN_DEFAULT                 (_SDIO_IEN_BOOTACKRCVSEN_DEFAULT << 13)         /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BOOTTERMINATESEN                      (0x1UL << 14)                                   /**< Boot Terminate Interrupt Signal Enable */
#define _SDIO_IEN_BOOTTERMINATESEN_SHIFT               14                                              /**< Shift value for SDIO_BOOTTERMINATESEN */
#define _SDIO_IEN_BOOTTERMINATESEN_MASK                0x4000UL                                        /**< Bit mask for SDIO_BOOTTERMINATESEN */
#define _SDIO_IEN_BOOTTERMINATESEN_DEFAULT             0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_BOOTTERMINATESEN_DEFAULT              (_SDIO_IEN_BOOTTERMINATESEN_DEFAULT << 14)      /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDTOUTERRSEN                         (0x1UL << 16)                                   /**< Command Timeout Error Signal Enable */
#define _SDIO_IEN_CMDTOUTERRSEN_SHIFT                  16                                              /**< Shift value for SDIO_CMDTOUTERRSEN */
#define _SDIO_IEN_CMDTOUTERRSEN_MASK                   0x10000UL                                       /**< Bit mask for SDIO_CMDTOUTERRSEN */
#define _SDIO_IEN_CMDTOUTERRSEN_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDTOUTERRSEN_DEFAULT                 (_SDIO_IEN_CMDTOUTERRSEN_DEFAULT << 16)         /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDCRCERRSEN                          (0x1UL << 17)                                   /**< Command CRC Error Signal Enable */
#define _SDIO_IEN_CMDCRCERRSEN_SHIFT                   17                                              /**< Shift value for SDIO_CMDCRCERRSEN */
#define _SDIO_IEN_CMDCRCERRSEN_MASK                    0x20000UL                                       /**< Bit mask for SDIO_CMDCRCERRSEN */
#define _SDIO_IEN_CMDCRCERRSEN_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDCRCERRSEN_DEFAULT                  (_SDIO_IEN_CMDCRCERRSEN_DEFAULT << 17)          /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDENDBITERRSEN                       (0x1UL << 18)                                   /**< Command End Bit Error Signal Enable */
#define _SDIO_IEN_CMDENDBITERRSEN_SHIFT                18                                              /**< Shift value for SDIO_CMDENDBITERRSEN */
#define _SDIO_IEN_CMDENDBITERRSEN_MASK                 0x40000UL                                       /**< Bit mask for SDIO_CMDENDBITERRSEN */
#define _SDIO_IEN_CMDENDBITERRSEN_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDENDBITERRSEN_DEFAULT               (_SDIO_IEN_CMDENDBITERRSEN_DEFAULT << 18)       /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDINDEXERRSEN                        (0x1UL << 19)                                   /**< Command Index Error Signal Enable */
#define _SDIO_IEN_CMDINDEXERRSEN_SHIFT                 19                                              /**< Shift value for SDIO_CMDINDEXERRSEN */
#define _SDIO_IEN_CMDINDEXERRSEN_MASK                  0x80000UL                                       /**< Bit mask for SDIO_CMDINDEXERRSEN */
#define _SDIO_IEN_CMDINDEXERRSEN_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CMDINDEXERRSEN_DEFAULT                (_SDIO_IEN_CMDINDEXERRSEN_DEFAULT << 19)        /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DATTOUTERRSEN                         (0x1UL << 20)                                   /**< Data Timeout Error Signal Enable */
#define _SDIO_IEN_DATTOUTERRSEN_SHIFT                  20                                              /**< Shift value for SDIO_DATTOUTERRSEN */
#define _SDIO_IEN_DATTOUTERRSEN_MASK                   0x100000UL                                      /**< Bit mask for SDIO_DATTOUTERRSEN */
#define _SDIO_IEN_DATTOUTERRSEN_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DATTOUTERRSEN_DEFAULT                 (_SDIO_IEN_DATTOUTERRSEN_DEFAULT << 20)         /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DATCRCERRSEN                          (0x1UL << 21)                                   /**< Data CRC Error Signal Enable */
#define _SDIO_IEN_DATCRCERRSEN_SHIFT                   21                                              /**< Shift value for SDIO_DATCRCERRSEN */
#define _SDIO_IEN_DATCRCERRSEN_MASK                    0x200000UL                                      /**< Bit mask for SDIO_DATCRCERRSEN */
#define _SDIO_IEN_DATCRCERRSEN_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DATCRCERRSEN_DEFAULT                  (_SDIO_IEN_DATCRCERRSEN_DEFAULT << 21)          /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DATENDBITERRSEN                       (0x1UL << 22)                                   /**< Data End Bit Error Signal Enable */
#define _SDIO_IEN_DATENDBITERRSEN_SHIFT                22                                              /**< Shift value for SDIO_DATENDBITERRSEN */
#define _SDIO_IEN_DATENDBITERRSEN_MASK                 0x400000UL                                      /**< Bit mask for SDIO_DATENDBITERRSEN */
#define _SDIO_IEN_DATENDBITERRSEN_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_DATENDBITERRSEN_DEFAULT               (_SDIO_IEN_DATENDBITERRSEN_DEFAULT << 22)       /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CURRENTLIMITERRSEN                    (0x1UL << 23)                                   /**< Current Limit Error Signal Enable */
#define _SDIO_IEN_CURRENTLIMITERRSEN_SHIFT             23                                              /**< Shift value for SDIO_CURRENTLIMITERRSEN */
#define _SDIO_IEN_CURRENTLIMITERRSEN_MASK              0x800000UL                                      /**< Bit mask for SDIO_CURRENTLIMITERRSEN */
#define _SDIO_IEN_CURRENTLIMITERRSEN_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_CURRENTLIMITERRSEN_DEFAULT            (_SDIO_IEN_CURRENTLIMITERRSEN_DEFAULT << 23)    /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_AUTOCMDERRSEN                         (0x1UL << 24)                                   /**< Auto CMD12 Error Signal Enable */
#define _SDIO_IEN_AUTOCMDERRSEN_SHIFT                  24                                              /**< Shift value for SDIO_AUTOCMDERRSEN */
#define _SDIO_IEN_AUTOCMDERRSEN_MASK                   0x1000000UL                                     /**< Bit mask for SDIO_AUTOCMDERRSEN */
#define _SDIO_IEN_AUTOCMDERRSEN_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_AUTOCMDERRSEN_DEFAULT                 (_SDIO_IEN_AUTOCMDERRSEN_DEFAULT << 24)         /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_ADMAERRSEN                            (0x1UL << 25)                                   /**< ADMA Error Signal Enable */
#define _SDIO_IEN_ADMAERRSEN_SHIFT                     25                                              /**< Shift value for SDIO_ADMAERRSEN */
#define _SDIO_IEN_ADMAERRSEN_MASK                      0x2000000UL                                     /**< Bit mask for SDIO_ADMAERRSEN */
#define _SDIO_IEN_ADMAERRSEN_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_ADMAERRSEN_DEFAULT                    (_SDIO_IEN_ADMAERRSEN_DEFAULT << 25)            /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_TUNINGERRSIGNALENABLE                 (0x1UL << 26)                                   /**< Tuning Error Signal Enable */
#define _SDIO_IEN_TUNINGERRSIGNALENABLE_SHIFT          26                                              /**< Shift value for SDIO_TUNINGERRSIGNALENABLE */
#define _SDIO_IEN_TUNINGERRSIGNALENABLE_MASK           0x4000000UL                                     /**< Bit mask for SDIO_TUNINGERRSIGNALENABLE */
#define _SDIO_IEN_TUNINGERRSIGNALENABLE_DEFAULT        0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_TUNINGERRSIGNALENABLE_DEFAULT         (_SDIO_IEN_TUNINGERRSIGNALENABLE_DEFAULT << 26) /**< Shifted mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_TARGETRESPERRSEN                      (0x1UL << 28)                                   /**< Target Response Error Signal Enable */
#define _SDIO_IEN_TARGETRESPERRSEN_SHIFT               28                                              /**< Shift value for SDIO_TARGETRESPERRSEN */
#define _SDIO_IEN_TARGETRESPERRSEN_MASK                0x10000000UL                                    /**< Bit mask for SDIO_TARGETRESPERRSEN */
#define _SDIO_IEN_TARGETRESPERRSEN_DEFAULT             0x00000000UL                                    /**< Mode DEFAULT for SDIO_IEN */
#define SDIO_IEN_TARGETRESPERRSEN_DEFAULT              (_SDIO_IEN_TARGETRESPERRSEN_DEFAULT << 28)      /**< Shifted mode DEFAULT for SDIO_IEN */

/* Bit fields for SDIO AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_RESETVALUE                   0x00000000UL                                   /**< Default value for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_MASK                         0xC0FF009FUL                                   /**< Mask for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12NOTEXE                    (0x1UL << 0)                                   /**< Auto CMD12 Not Executed */
#define _SDIO_AC12ERRSTAT_AC12NOTEXE_SHIFT             0                                              /**< Shift value for SDIO_AC12NOTEXE */
#define _SDIO_AC12ERRSTAT_AC12NOTEXE_MASK              0x1UL                                          /**< Bit mask for SDIO_AC12NOTEXE */
#define _SDIO_AC12ERRSTAT_AC12NOTEXE_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12NOTEXE_DEFAULT            (_SDIO_AC12ERRSTAT_AC12NOTEXE_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12TOE                       (0x1UL << 1)                                   /**< Auto CMD12 Timeout Error */
#define _SDIO_AC12ERRSTAT_AC12TOE_SHIFT                1                                              /**< Shift value for SDIO_AC12TOE */
#define _SDIO_AC12ERRSTAT_AC12TOE_MASK                 0x2UL                                          /**< Bit mask for SDIO_AC12TOE */
#define _SDIO_AC12ERRSTAT_AC12TOE_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12TOE_DEFAULT               (_SDIO_AC12ERRSTAT_AC12TOE_DEFAULT << 1)       /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12CRCERR                    (0x1UL << 2)                                   /**< Auto CMD CRC Error */
#define _SDIO_AC12ERRSTAT_AC12CRCERR_SHIFT             2                                              /**< Shift value for SDIO_AC12CRCERR */
#define _SDIO_AC12ERRSTAT_AC12CRCERR_MASK              0x4UL                                          /**< Bit mask for SDIO_AC12CRCERR */
#define _SDIO_AC12ERRSTAT_AC12CRCERR_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12CRCERR_DEFAULT            (_SDIO_AC12ERRSTAT_AC12CRCERR_DEFAULT << 2)    /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12ENDBITERR                 (0x1UL << 3)                                   /**< Auto CMD End Bit Error */
#define _SDIO_AC12ERRSTAT_AC12ENDBITERR_SHIFT          3                                              /**< Shift value for SDIO_AC12ENDBITERR */
#define _SDIO_AC12ERRSTAT_AC12ENDBITERR_MASK           0x8UL                                          /**< Bit mask for SDIO_AC12ENDBITERR */
#define _SDIO_AC12ERRSTAT_AC12ENDBITERR_DEFAULT        0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12ENDBITERR_DEFAULT         (_SDIO_AC12ERRSTAT_AC12ENDBITERR_DEFAULT << 3) /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12INDEXERR                  (0x1UL << 4)                                   /**< Auto CMD Index Error */
#define _SDIO_AC12ERRSTAT_AC12INDEXERR_SHIFT           4                                              /**< Shift value for SDIO_AC12INDEXERR */
#define _SDIO_AC12ERRSTAT_AC12INDEXERR_MASK            0x10UL                                         /**< Bit mask for SDIO_AC12INDEXERR */
#define _SDIO_AC12ERRSTAT_AC12INDEXERR_DEFAULT         0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_AC12INDEXERR_DEFAULT          (_SDIO_AC12ERRSTAT_AC12INDEXERR_DEFAULT << 4)  /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_CNIBAC12ERR                   (0x1UL << 7)                                   /**< Command Not Issued By Auto CMD12 Error */
#define _SDIO_AC12ERRSTAT_CNIBAC12ERR_SHIFT            7                                              /**< Shift value for SDIO_CNIBAC12ERR */
#define _SDIO_AC12ERRSTAT_CNIBAC12ERR_MASK             0x80UL                                         /**< Bit mask for SDIO_CNIBAC12ERR */
#define _SDIO_AC12ERRSTAT_CNIBAC12ERR_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_CNIBAC12ERR_DEFAULT           (_SDIO_AC12ERRSTAT_CNIBAC12ERR_DEFAULT << 7)   /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_SHIFT             16                                             /**< Shift value for SDIO_UHSMODESEL */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_MASK              0x70000UL                                      /**< Bit mask for SDIO_UHSMODESEL */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_SDR12             0x00000000UL                                   /**< Mode SDR12 for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_SDR25             0x00000001UL                                   /**< Mode SDR25 for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_SDR50             0x00000002UL                                   /**< Mode SDR50 for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_SDR104            0x00000003UL                                   /**< Mode SDR104 for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_UHSMODESEL_DDR50             0x00000004UL                                   /**< Mode DDR50 for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_UHSMODESEL_DEFAULT            (_SDIO_AC12ERRSTAT_UHSMODESEL_DEFAULT << 16)   /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_UHSMODESEL_SDR12              (_SDIO_AC12ERRSTAT_UHSMODESEL_SDR12 << 16)     /**< Shifted mode SDR12 for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_UHSMODESEL_SDR25              (_SDIO_AC12ERRSTAT_UHSMODESEL_SDR25 << 16)     /**< Shifted mode SDR25 for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_UHSMODESEL_SDR50              (_SDIO_AC12ERRSTAT_UHSMODESEL_SDR50 << 16)     /**< Shifted mode SDR50 for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_UHSMODESEL_SDR104             (_SDIO_AC12ERRSTAT_UHSMODESEL_SDR104 << 16)    /**< Shifted mode SDR104 for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_UHSMODESEL_DDR50              (_SDIO_AC12ERRSTAT_UHSMODESEL_DDR50 << 16)     /**< Shifted mode DDR50 for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_SIGEN1P8V                     (0x1UL << 19)                                  /**< Voltage 1.8V Signal Enable */
#define _SDIO_AC12ERRSTAT_SIGEN1P8V_SHIFT              19                                             /**< Shift value for SDIO_SIGEN1P8V */
#define _SDIO_AC12ERRSTAT_SIGEN1P8V_MASK               0x80000UL                                      /**< Bit mask for SDIO_SIGEN1P8V */
#define _SDIO_AC12ERRSTAT_SIGEN1P8V_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_SIGEN1P8V_DEFAULT             (_SDIO_AC12ERRSTAT_SIGEN1P8V_DEFAULT << 19)    /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_SHIFT              20                                             /**< Shift value for SDIO_DRVSTNSEL */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_MASK               0x300000UL                                     /**< Bit mask for SDIO_DRVSTNSEL */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEB              0x00000000UL                                   /**< Mode TYPEB for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEA              0x00000001UL                                   /**< Mode TYPEA for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEC              0x00000002UL                                   /**< Mode TYPEC for SDIO_AC12ERRSTAT */
#define _SDIO_AC12ERRSTAT_DRVSTNSEL_TYPED              0x00000003UL                                   /**< Mode TYPED for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_DRVSTNSEL_DEFAULT             (_SDIO_AC12ERRSTAT_DRVSTNSEL_DEFAULT << 20)    /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEB               (_SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEB << 20)      /**< Shifted mode TYPEB for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEA               (_SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEA << 20)      /**< Shifted mode TYPEA for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEC               (_SDIO_AC12ERRSTAT_DRVSTNSEL_TYPEC << 20)      /**< Shifted mode TYPEC for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_DRVSTNSEL_TYPED               (_SDIO_AC12ERRSTAT_DRVSTNSEL_TYPED << 20)      /**< Shifted mode TYPED for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_EXETUNING                     (0x1UL << 22)                                  /**< Execute Tuning */
#define _SDIO_AC12ERRSTAT_EXETUNING_SHIFT              22                                             /**< Shift value for SDIO_EXETUNING */
#define _SDIO_AC12ERRSTAT_EXETUNING_MASK               0x400000UL                                     /**< Bit mask for SDIO_EXETUNING */
#define _SDIO_AC12ERRSTAT_EXETUNING_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_EXETUNING_DEFAULT             (_SDIO_AC12ERRSTAT_EXETUNING_DEFAULT << 22)    /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_SAMPCLKSEL                    (0x1UL << 23)                                  /**< Sampling Clock Select */
#define _SDIO_AC12ERRSTAT_SAMPCLKSEL_SHIFT             23                                             /**< Shift value for SDIO_SAMPCLKSEL */
#define _SDIO_AC12ERRSTAT_SAMPCLKSEL_MASK              0x800000UL                                     /**< Bit mask for SDIO_SAMPCLKSEL */
#define _SDIO_AC12ERRSTAT_SAMPCLKSEL_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_SAMPCLKSEL_DEFAULT            (_SDIO_AC12ERRSTAT_SAMPCLKSEL_DEFAULT << 23)   /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_ASYNCINTEN                    (0x1UL << 30)                                  /**< Asynchronous Interrupt Enable */
#define _SDIO_AC12ERRSTAT_ASYNCINTEN_SHIFT             30                                             /**< Shift value for SDIO_ASYNCINTEN */
#define _SDIO_AC12ERRSTAT_ASYNCINTEN_MASK              0x40000000UL                                   /**< Bit mask for SDIO_ASYNCINTEN */
#define _SDIO_AC12ERRSTAT_ASYNCINTEN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_ASYNCINTEN_DEFAULT            (_SDIO_AC12ERRSTAT_ASYNCINTEN_DEFAULT << 30)   /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_PRSTVALEN                     (0x1UL << 31)                                  /**< Preset Value Enable */
#define _SDIO_AC12ERRSTAT_PRSTVALEN_SHIFT              31                                             /**< Shift value for SDIO_PRSTVALEN */
#define _SDIO_AC12ERRSTAT_PRSTVALEN_MASK               0x80000000UL                                   /**< Bit mask for SDIO_PRSTVALEN */
#define _SDIO_AC12ERRSTAT_PRSTVALEN_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_AC12ERRSTAT */
#define SDIO_AC12ERRSTAT_PRSTVALEN_DEFAULT             (_SDIO_AC12ERRSTAT_PRSTVALEN_DEFAULT << 31)    /**< Shifted mode DEFAULT for SDIO_AC12ERRSTAT */

/* Bit fields for SDIO CAPAB0 */
#define _SDIO_CAPAB0_RESETVALUE                        0x00000000UL                                /**< Default value for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_MASK                              0xF7EFFFBFUL                                /**< Mask for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_TMOUTCLKFREQ_SHIFT                0                                           /**< Shift value for SDIO_TMOUTCLKFREQ */
#define _SDIO_CAPAB0_TMOUTCLKFREQ_MASK                 0x3FUL                                      /**< Bit mask for SDIO_TMOUTCLKFREQ */
#define _SDIO_CAPAB0_TMOUTCLKFREQ_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_TMOUTCLKFREQ_DEFAULT               (_SDIO_CAPAB0_TMOUTCLKFREQ_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_TMOUTCLKUNIT                       (0x1UL << 7)                                /**< Timeout Clock Unit */
#define _SDIO_CAPAB0_TMOUTCLKUNIT_SHIFT                7                                           /**< Shift value for SDIO_TMOUTCLKUNIT */
#define _SDIO_CAPAB0_TMOUTCLKUNIT_MASK                 0x80UL                                      /**< Bit mask for SDIO_TMOUTCLKUNIT */
#define _SDIO_CAPAB0_TMOUTCLKUNIT_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_TMOUTCLKUNIT_DEFAULT               (_SDIO_CAPAB0_TMOUTCLKUNIT_DEFAULT << 7)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_BASECLKFREQSD_SHIFT               8                                           /**< Shift value for SDIO_BASECLKFREQSD */
#define _SDIO_CAPAB0_BASECLKFREQSD_MASK                0xFF00UL                                    /**< Bit mask for SDIO_BASECLKFREQSD */
#define _SDIO_CAPAB0_BASECLKFREQSD_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_BASECLKFREQSD_DEFAULT              (_SDIO_CAPAB0_BASECLKFREQSD_DEFAULT << 8)   /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_MAXBLOCKLEN_SHIFT                 16                                          /**< Shift value for SDIO_MAXBLOCKLEN */
#define _SDIO_CAPAB0_MAXBLOCKLEN_MASK                  0x30000UL                                   /**< Bit mask for SDIO_MAXBLOCKLEN */
#define _SDIO_CAPAB0_MAXBLOCKLEN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_MAXBLOCKLEN_DEFAULT                (_SDIO_CAPAB0_MAXBLOCKLEN_DEFAULT << 16)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_EXTMEDIABUSSUP                     (0x1UL << 18)                               /**< Extended Media Bus Support */
#define _SDIO_CAPAB0_EXTMEDIABUSSUP_SHIFT              18                                          /**< Shift value for SDIO_EXTMEDIABUSSUP */
#define _SDIO_CAPAB0_EXTMEDIABUSSUP_MASK               0x40000UL                                   /**< Bit mask for SDIO_EXTMEDIABUSSUP */
#define _SDIO_CAPAB0_EXTMEDIABUSSUP_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_EXTMEDIABUSSUP_DEFAULT             (_SDIO_CAPAB0_EXTMEDIABUSSUP_DEFAULT << 18) /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_ADMA2SUP                           (0x1UL << 19)                               /**< ADMA2 Support */
#define _SDIO_CAPAB0_ADMA2SUP_SHIFT                    19                                          /**< Shift value for SDIO_ADMA2SUP */
#define _SDIO_CAPAB0_ADMA2SUP_MASK                     0x80000UL                                   /**< Bit mask for SDIO_ADMA2SUP */
#define _SDIO_CAPAB0_ADMA2SUP_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_ADMA2SUP_DEFAULT                   (_SDIO_CAPAB0_ADMA2SUP_DEFAULT << 19)       /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_HSSUP                              (0x1UL << 21)                               /**< High Speed Support */
#define _SDIO_CAPAB0_HSSUP_SHIFT                       21                                          /**< Shift value for SDIO_HSSUP */
#define _SDIO_CAPAB0_HSSUP_MASK                        0x200000UL                                  /**< Bit mask for SDIO_HSSUP */
#define _SDIO_CAPAB0_HSSUP_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_HSSUP_DEFAULT                      (_SDIO_CAPAB0_HSSUP_DEFAULT << 21)          /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_SDMASUP                            (0x1UL << 22)                               /**< SDMA Support */
#define _SDIO_CAPAB0_SDMASUP_SHIFT                     22                                          /**< Shift value for SDIO_SDMASUP */
#define _SDIO_CAPAB0_SDMASUP_MASK                      0x400000UL                                  /**< Bit mask for SDIO_SDMASUP */
#define _SDIO_CAPAB0_SDMASUP_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_SDMASUP_DEFAULT                    (_SDIO_CAPAB0_SDMASUP_DEFAULT << 22)        /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_SUSRESSUP                          (0x1UL << 23)                               /**< Suspend / Resume Support */
#define _SDIO_CAPAB0_SUSRESSUP_SHIFT                   23                                          /**< Shift value for SDIO_SUSRESSUP */
#define _SDIO_CAPAB0_SUSRESSUP_MASK                    0x800000UL                                  /**< Bit mask for SDIO_SUSRESSUP */
#define _SDIO_CAPAB0_SUSRESSUP_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_SUSRESSUP_DEFAULT                  (_SDIO_CAPAB0_SUSRESSUP_DEFAULT << 23)      /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_VOLTSUP3P3V                        (0x1UL << 24)                               /**< Voltage Support 3.3V */
#define _SDIO_CAPAB0_VOLTSUP3P3V_SHIFT                 24                                          /**< Shift value for SDIO_VOLTSUP3P3V */
#define _SDIO_CAPAB0_VOLTSUP3P3V_MASK                  0x1000000UL                                 /**< Bit mask for SDIO_VOLTSUP3P3V */
#define _SDIO_CAPAB0_VOLTSUP3P3V_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_VOLTSUP3P3V_DEFAULT                (_SDIO_CAPAB0_VOLTSUP3P3V_DEFAULT << 24)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_VOLTSUP3P0V                        (0x1UL << 25)                               /**< Voltage Support 3.0V */
#define _SDIO_CAPAB0_VOLTSUP3P0V_SHIFT                 25                                          /**< Shift value for SDIO_VOLTSUP3P0V */
#define _SDIO_CAPAB0_VOLTSUP3P0V_MASK                  0x2000000UL                                 /**< Bit mask for SDIO_VOLTSUP3P0V */
#define _SDIO_CAPAB0_VOLTSUP3P0V_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_VOLTSUP3P0V_DEFAULT                (_SDIO_CAPAB0_VOLTSUP3P0V_DEFAULT << 25)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_VOLTSUP1P8V                        (0x1UL << 26)                               /**< Voltage Support 1.8V */
#define _SDIO_CAPAB0_VOLTSUP1P8V_SHIFT                 26                                          /**< Shift value for SDIO_VOLTSUP1P8V */
#define _SDIO_CAPAB0_VOLTSUP1P8V_MASK                  0x4000000UL                                 /**< Bit mask for SDIO_VOLTSUP1P8V */
#define _SDIO_CAPAB0_VOLTSUP1P8V_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_VOLTSUP1P8V_DEFAULT                (_SDIO_CAPAB0_VOLTSUP1P8V_DEFAULT << 26)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_SYSBUS64BSUP                       (0x1UL << 28)                               /**< System Bus 64-bit Support */
#define _SDIO_CAPAB0_SYSBUS64BSUP_SHIFT                28                                          /**< Shift value for SDIO_SYSBUS64BSUP */
#define _SDIO_CAPAB0_SYSBUS64BSUP_MASK                 0x10000000UL                                /**< Bit mask for SDIO_SYSBUS64BSUP */
#define _SDIO_CAPAB0_SYSBUS64BSUP_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_SYSBUS64BSUP_DEFAULT               (_SDIO_CAPAB0_SYSBUS64BSUP_DEFAULT << 28)   /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_ASYNCINTSUP                        (0x1UL << 29)                               /**< Asynchronous Interrupt Support */
#define _SDIO_CAPAB0_ASYNCINTSUP_SHIFT                 29                                          /**< Shift value for SDIO_ASYNCINTSUP */
#define _SDIO_CAPAB0_ASYNCINTSUP_MASK                  0x20000000UL                                /**< Bit mask for SDIO_ASYNCINTSUP */
#define _SDIO_CAPAB0_ASYNCINTSUP_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_ASYNCINTSUP_DEFAULT                (_SDIO_CAPAB0_ASYNCINTSUP_DEFAULT << 29)    /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_IFSLOTTYPE_SHIFT                  30                                          /**< Shift value for SDIO_IFSLOTTYPE */
#define _SDIO_CAPAB0_IFSLOTTYPE_MASK                   0xC0000000UL                                /**< Bit mask for SDIO_IFSLOTTYPE */
#define _SDIO_CAPAB0_IFSLOTTYPE_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_IFSLOTTYPE_REMOVABLE              0x00000000UL                                /**< Mode REMOVABLE for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_IFSLOTTYPE_EMBEDDED               0x00000001UL                                /**< Mode EMBEDDED for SDIO_CAPAB0 */
#define _SDIO_CAPAB0_IFSLOTTYPE_SHARED                 0x00000002UL                                /**< Mode SHARED for SDIO_CAPAB0 */
#define SDIO_CAPAB0_IFSLOTTYPE_DEFAULT                 (_SDIO_CAPAB0_IFSLOTTYPE_DEFAULT << 30)     /**< Shifted mode DEFAULT for SDIO_CAPAB0 */
#define SDIO_CAPAB0_IFSLOTTYPE_REMOVABLE               (_SDIO_CAPAB0_IFSLOTTYPE_REMOVABLE << 30)   /**< Shifted mode REMOVABLE for SDIO_CAPAB0 */
#define SDIO_CAPAB0_IFSLOTTYPE_EMBEDDED                (_SDIO_CAPAB0_IFSLOTTYPE_EMBEDDED << 30)    /**< Shifted mode EMBEDDED for SDIO_CAPAB0 */
#define SDIO_CAPAB0_IFSLOTTYPE_SHARED                  (_SDIO_CAPAB0_IFSLOTTYPE_SHARED << 30)      /**< Shifted mode SHARED for SDIO_CAPAB0 */

/* Bit fields for SDIO CAPAB2 */
#define _SDIO_CAPAB2_RESETVALUE                        0x00000000UL                              /**< Default value for SDIO_CAPAB2 */
#define _SDIO_CAPAB2_MASK                              0x03FFEF77UL                              /**< Mask for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SDR50SUP                           (0x1UL << 0)                              /**< SDR50 Support */
#define _SDIO_CAPAB2_SDR50SUP_SHIFT                    0                                         /**< Shift value for SDIO_SDR50SUP */
#define _SDIO_CAPAB2_SDR50SUP_MASK                     0x1UL                                     /**< Bit mask for SDIO_SDR50SUP */
#define _SDIO_CAPAB2_SDR50SUP_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SDR50SUP_DEFAULT                   (_SDIO_CAPAB2_SDR50SUP_DEFAULT << 0)      /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SDR104SUP                          (0x1UL << 1)                              /**< SDR104 Support */
#define _SDIO_CAPAB2_SDR104SUP_SHIFT                   1                                         /**< Shift value for SDIO_SDR104SUP */
#define _SDIO_CAPAB2_SDR104SUP_MASK                    0x2UL                                     /**< Bit mask for SDIO_SDR104SUP */
#define _SDIO_CAPAB2_SDR104SUP_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SDR104SUP_DEFAULT                  (_SDIO_CAPAB2_SDR104SUP_DEFAULT << 1)     /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DDR50SUP                           (0x1UL << 2)                              /**< DDR50 Support */
#define _SDIO_CAPAB2_DDR50SUP_SHIFT                    2                                         /**< Shift value for SDIO_DDR50SUP */
#define _SDIO_CAPAB2_DDR50SUP_MASK                     0x4UL                                     /**< Bit mask for SDIO_DDR50SUP */
#define _SDIO_CAPAB2_DDR50SUP_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DDR50SUP_DEFAULT                   (_SDIO_CAPAB2_DDR50SUP_DEFAULT << 2)      /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DRVTYPASUP                         (0x1UL << 4)                              /**< Driver Type a Support */
#define _SDIO_CAPAB2_DRVTYPASUP_SHIFT                  4                                         /**< Shift value for SDIO_DRVTYPASUP */
#define _SDIO_CAPAB2_DRVTYPASUP_MASK                   0x10UL                                    /**< Bit mask for SDIO_DRVTYPASUP */
#define _SDIO_CAPAB2_DRVTYPASUP_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DRVTYPASUP_DEFAULT                 (_SDIO_CAPAB2_DRVTYPASUP_DEFAULT << 4)    /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DRVTYPCSUP                         (0x1UL << 5)                              /**< Driver Type C Support */
#define _SDIO_CAPAB2_DRVTYPCSUP_SHIFT                  5                                         /**< Shift value for SDIO_DRVTYPCSUP */
#define _SDIO_CAPAB2_DRVTYPCSUP_MASK                   0x20UL                                    /**< Bit mask for SDIO_DRVTYPCSUP */
#define _SDIO_CAPAB2_DRVTYPCSUP_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DRVTYPCSUP_DEFAULT                 (_SDIO_CAPAB2_DRVTYPCSUP_DEFAULT << 5)    /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DRVTYPDSUP                         (0x1UL << 6)                              /**< Driver Type D Support */
#define _SDIO_CAPAB2_DRVTYPDSUP_SHIFT                  6                                         /**< Shift value for SDIO_DRVTYPDSUP */
#define _SDIO_CAPAB2_DRVTYPDSUP_MASK                   0x40UL                                    /**< Bit mask for SDIO_DRVTYPDSUP */
#define _SDIO_CAPAB2_DRVTYPDSUP_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_DRVTYPDSUP_DEFAULT                 (_SDIO_CAPAB2_DRVTYPDSUP_DEFAULT << 6)    /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define _SDIO_CAPAB2_TIMCNTRETUN_SHIFT                 8                                         /**< Shift value for SDIO_TIMCNTRETUN */
#define _SDIO_CAPAB2_TIMCNTRETUN_MASK                  0xF00UL                                   /**< Bit mask for SDIO_TIMCNTRETUN */
#define _SDIO_CAPAB2_TIMCNTRETUN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_TIMCNTRETUN_DEFAULT                (_SDIO_CAPAB2_TIMCNTRETUN_DEFAULT << 8)   /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_USETUNSDR50                        (0x1UL << 13)                             /**< Use Tuning for SDR50 */
#define _SDIO_CAPAB2_USETUNSDR50_SHIFT                 13                                        /**< Shift value for SDIO_USETUNSDR50 */
#define _SDIO_CAPAB2_USETUNSDR50_MASK                  0x2000UL                                  /**< Bit mask for SDIO_USETUNSDR50 */
#define _SDIO_CAPAB2_USETUNSDR50_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_USETUNSDR50_DEFAULT                (_SDIO_CAPAB2_USETUNSDR50_DEFAULT << 13)  /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define _SDIO_CAPAB2_RETUNEMODES_SHIFT                 14                                        /**< Shift value for SDIO_RETUNEMODES */
#define _SDIO_CAPAB2_RETUNEMODES_MASK                  0xC000UL                                  /**< Bit mask for SDIO_RETUNEMODES */
#define _SDIO_CAPAB2_RETUNEMODES_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_RETUNEMODES_DEFAULT                (_SDIO_CAPAB2_RETUNEMODES_DEFAULT << 14)  /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define _SDIO_CAPAB2_CLOCKKMUL_SHIFT                   16                                        /**< Shift value for SDIO_CLOCKKMUL */
#define _SDIO_CAPAB2_CLOCKKMUL_MASK                    0xFF0000UL                                /**< Bit mask for SDIO_CLOCKKMUL */
#define _SDIO_CAPAB2_CLOCKKMUL_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_CLOCKKMUL_DEFAULT                  (_SDIO_CAPAB2_CLOCKKMUL_DEFAULT << 16)    /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SPIMODE                            (0x1UL << 24)                             /**< SPI Mode Support */
#define _SDIO_CAPAB2_SPIMODE_SHIFT                     24                                        /**< Shift value for SDIO_SPIMODE */
#define _SDIO_CAPAB2_SPIMODE_MASK                      0x1000000UL                               /**< Bit mask for SDIO_SPIMODE */
#define _SDIO_CAPAB2_SPIMODE_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SPIMODE_DEFAULT                    (_SDIO_CAPAB2_SPIMODE_DEFAULT << 24)      /**< Shifted mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SPIBLOCKMODE                       (0x1UL << 25)                             /**< SPI Block Mode Support */
#define _SDIO_CAPAB2_SPIBLOCKMODE_SHIFT                25                                        /**< Shift value for SDIO_SPIBLOCKMODE */
#define _SDIO_CAPAB2_SPIBLOCKMODE_MASK                 0x2000000UL                               /**< Bit mask for SDIO_SPIBLOCKMODE */
#define _SDIO_CAPAB2_SPIBLOCKMODE_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SDIO_CAPAB2 */
#define SDIO_CAPAB2_SPIBLOCKMODE_DEFAULT               (_SDIO_CAPAB2_SPIBLOCKMODE_DEFAULT << 25) /**< Shifted mode DEFAULT for SDIO_CAPAB2 */

/* Bit fields for SDIO MAXCURCAPAB */
#define _SDIO_MAXCURCAPAB_RESETVALUE                   0x00000000UL                                   /**< Default value for SDIO_MAXCURCAPAB */
#define _SDIO_MAXCURCAPAB_MASK                         0x00FFFFFFUL                                   /**< Mask for SDIO_MAXCURCAPAB */
#define _SDIO_MAXCURCAPAB_MAXCUR3P3VAL_SHIFT           0                                              /**< Shift value for SDIO_MAXCUR3P3VAL */
#define _SDIO_MAXCURCAPAB_MAXCUR3P3VAL_MASK            0xFFUL                                         /**< Bit mask for SDIO_MAXCUR3P3VAL */
#define _SDIO_MAXCURCAPAB_MAXCUR3P3VAL_DEFAULT         0x00000000UL                                   /**< Mode DEFAULT for SDIO_MAXCURCAPAB */
#define SDIO_MAXCURCAPAB_MAXCUR3P3VAL_DEFAULT          (_SDIO_MAXCURCAPAB_MAXCUR3P3VAL_DEFAULT << 0)  /**< Shifted mode DEFAULT for SDIO_MAXCURCAPAB */
#define _SDIO_MAXCURCAPAB_MAXCUR3P0VAL_SHIFT           8                                              /**< Shift value for SDIO_MAXCUR3P0VAL */
#define _SDIO_MAXCURCAPAB_MAXCUR3P0VAL_MASK            0xFF00UL                                       /**< Bit mask for SDIO_MAXCUR3P0VAL */
#define _SDIO_MAXCURCAPAB_MAXCUR3P0VAL_DEFAULT         0x00000000UL                                   /**< Mode DEFAULT for SDIO_MAXCURCAPAB */
#define SDIO_MAXCURCAPAB_MAXCUR3P0VAL_DEFAULT          (_SDIO_MAXCURCAPAB_MAXCUR3P0VAL_DEFAULT << 8)  /**< Shifted mode DEFAULT for SDIO_MAXCURCAPAB */
#define _SDIO_MAXCURCAPAB_MAXCUR1P8VAL_SHIFT           16                                             /**< Shift value for SDIO_MAXCUR1P8VAL */
#define _SDIO_MAXCURCAPAB_MAXCUR1P8VAL_MASK            0xFF0000UL                                     /**< Bit mask for SDIO_MAXCUR1P8VAL */
#define _SDIO_MAXCURCAPAB_MAXCUR1P8VAL_DEFAULT         0x00000000UL                                   /**< Mode DEFAULT for SDIO_MAXCURCAPAB */
#define SDIO_MAXCURCAPAB_MAXCUR1P8VAL_DEFAULT          (_SDIO_MAXCURCAPAB_MAXCUR1P8VAL_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_MAXCURCAPAB */

/* Bit fields for SDIO FEVTERRSTAT */
#define _SDIO_FEVTERRSTAT_RESETVALUE                   0x00000000UL                                /**< Default value for SDIO_FEVTERRSTAT */
#define _SDIO_FEVTERRSTAT_MASK                         0xF7FF009FUL                                /**< Mask for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12NEX                       (0x1UL << 0)                                /**< Force Event for Command Not Issued By Auto CM12 Not Executed */
#define _SDIO_FEVTERRSTAT_AC12NEX_SHIFT                0                                           /**< Shift value for SDIO_AC12NEX */
#define _SDIO_FEVTERRSTAT_AC12NEX_MASK                 0x1UL                                       /**< Bit mask for SDIO_AC12NEX */
#define _SDIO_FEVTERRSTAT_AC12NEX_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12NEX_DEFAULT               (_SDIO_FEVTERRSTAT_AC12NEX_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12TOE                       (0x1UL << 1)                                /**< Force Event for Auto CMD Timeout Error */
#define _SDIO_FEVTERRSTAT_AC12TOE_SHIFT                1                                           /**< Shift value for SDIO_AC12TOE */
#define _SDIO_FEVTERRSTAT_AC12TOE_MASK                 0x2UL                                       /**< Bit mask for SDIO_AC12TOE */
#define _SDIO_FEVTERRSTAT_AC12TOE_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12TOE_DEFAULT               (_SDIO_FEVTERRSTAT_AC12TOE_DEFAULT << 1)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12CRCE                      (0x1UL << 2)                                /**< Force Event for Auto CMD CRC Error */
#define _SDIO_FEVTERRSTAT_AC12CRCE_SHIFT               2                                           /**< Shift value for SDIO_AC12CRCE */
#define _SDIO_FEVTERRSTAT_AC12CRCE_MASK                0x4UL                                       /**< Bit mask for SDIO_AC12CRCE */
#define _SDIO_FEVTERRSTAT_AC12CRCE_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12CRCE_DEFAULT              (_SDIO_FEVTERRSTAT_AC12CRCE_DEFAULT << 2)   /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12EBE                       (0x1UL << 3)                                /**< Force Event for Auto CMD End Bit Error */
#define _SDIO_FEVTERRSTAT_AC12EBE_SHIFT                3                                           /**< Shift value for SDIO_AC12EBE */
#define _SDIO_FEVTERRSTAT_AC12EBE_MASK                 0x8UL                                       /**< Bit mask for SDIO_AC12EBE */
#define _SDIO_FEVTERRSTAT_AC12EBE_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12EBE_DEFAULT               (_SDIO_FEVTERRSTAT_AC12EBE_DEFAULT << 3)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12INDXE                     (0x1UL << 4)                                /**< Force Event for Auto CMD Index Error */
#define _SDIO_FEVTERRSTAT_AC12INDXE_SHIFT              4                                           /**< Shift value for SDIO_AC12INDXE */
#define _SDIO_FEVTERRSTAT_AC12INDXE_MASK               0x10UL                                      /**< Bit mask for SDIO_AC12INDXE */
#define _SDIO_FEVTERRSTAT_AC12INDXE_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12INDXE_DEFAULT             (_SDIO_FEVTERRSTAT_AC12INDXE_DEFAULT << 4)  /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CNIBAC12E                     (0x1UL << 7)                                /**< Force Event for Command Not Issued By Auto CMD12 Error */
#define _SDIO_FEVTERRSTAT_CNIBAC12E_SHIFT              7                                           /**< Shift value for SDIO_CNIBAC12E */
#define _SDIO_FEVTERRSTAT_CNIBAC12E_MASK               0x80UL                                      /**< Bit mask for SDIO_CNIBAC12E */
#define _SDIO_FEVTERRSTAT_CNIBAC12E_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CNIBAC12E_DEFAULT             (_SDIO_FEVTERRSTAT_CNIBAC12E_DEFAULT << 7)  /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDTOE                        (0x1UL << 16)                               /**< Force Event for Command Timeout Error */
#define _SDIO_FEVTERRSTAT_CMDTOE_SHIFT                 16                                          /**< Shift value for SDIO_CMDTOE */
#define _SDIO_FEVTERRSTAT_CMDTOE_MASK                  0x10000UL                                   /**< Bit mask for SDIO_CMDTOE */
#define _SDIO_FEVTERRSTAT_CMDTOE_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDTOE_DEFAULT                (_SDIO_FEVTERRSTAT_CMDTOE_DEFAULT << 16)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDCRCE                       (0x1UL << 17)                               /**< Force Event for Command CRC Error */
#define _SDIO_FEVTERRSTAT_CMDCRCE_SHIFT                17                                          /**< Shift value for SDIO_CMDCRCE */
#define _SDIO_FEVTERRSTAT_CMDCRCE_MASK                 0x20000UL                                   /**< Bit mask for SDIO_CMDCRCE */
#define _SDIO_FEVTERRSTAT_CMDCRCE_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDCRCE_DEFAULT               (_SDIO_FEVTERRSTAT_CMDCRCE_DEFAULT << 17)   /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDEBE                        (0x1UL << 18)                               /**< Force Event for Command End Bit Error */
#define _SDIO_FEVTERRSTAT_CMDEBE_SHIFT                 18                                          /**< Shift value for SDIO_CMDEBE */
#define _SDIO_FEVTERRSTAT_CMDEBE_MASK                  0x40000UL                                   /**< Bit mask for SDIO_CMDEBE */
#define _SDIO_FEVTERRSTAT_CMDEBE_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDEBE_DEFAULT                (_SDIO_FEVTERRSTAT_CMDEBE_DEFAULT << 18)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDINDXE                      (0x1UL << 19)                               /**< Force Event for Command Index Error */
#define _SDIO_FEVTERRSTAT_CMDINDXE_SHIFT               19                                          /**< Shift value for SDIO_CMDINDXE */
#define _SDIO_FEVTERRSTAT_CMDINDXE_MASK                0x80000UL                                   /**< Bit mask for SDIO_CMDINDXE */
#define _SDIO_FEVTERRSTAT_CMDINDXE_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CMDINDXE_DEFAULT              (_SDIO_FEVTERRSTAT_CMDINDXE_DEFAULT << 19)  /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_DATTOE                        (0x1UL << 20)                               /**< Force Event for Data Timeout Error */
#define _SDIO_FEVTERRSTAT_DATTOE_SHIFT                 20                                          /**< Shift value for SDIO_DATTOE */
#define _SDIO_FEVTERRSTAT_DATTOE_MASK                  0x100000UL                                  /**< Bit mask for SDIO_DATTOE */
#define _SDIO_FEVTERRSTAT_DATTOE_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_DATTOE_DEFAULT                (_SDIO_FEVTERRSTAT_DATTOE_DEFAULT << 20)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_DATCRCE                       (0x1UL << 21)                               /**< Force Event for Data CRC Error */
#define _SDIO_FEVTERRSTAT_DATCRCE_SHIFT                21                                          /**< Shift value for SDIO_DATCRCE */
#define _SDIO_FEVTERRSTAT_DATCRCE_MASK                 0x200000UL                                  /**< Bit mask for SDIO_DATCRCE */
#define _SDIO_FEVTERRSTAT_DATCRCE_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_DATCRCE_DEFAULT               (_SDIO_FEVTERRSTAT_DATCRCE_DEFAULT << 21)   /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_DATEBE                        (0x1UL << 22)                               /**< Force Event for Data End Bit Error */
#define _SDIO_FEVTERRSTAT_DATEBE_SHIFT                 22                                          /**< Shift value for SDIO_DATEBE */
#define _SDIO_FEVTERRSTAT_DATEBE_MASK                  0x400000UL                                  /**< Bit mask for SDIO_DATEBE */
#define _SDIO_FEVTERRSTAT_DATEBE_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_DATEBE_DEFAULT                (_SDIO_FEVTERRSTAT_DATEBE_DEFAULT << 22)    /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CURLIMITE                     (0x1UL << 23)                               /**< Force Event for Current Limit Error */
#define _SDIO_FEVTERRSTAT_CURLIMITE_SHIFT              23                                          /**< Shift value for SDIO_CURLIMITE */
#define _SDIO_FEVTERRSTAT_CURLIMITE_MASK               0x800000UL                                  /**< Bit mask for SDIO_CURLIMITE */
#define _SDIO_FEVTERRSTAT_CURLIMITE_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_CURLIMITE_DEFAULT             (_SDIO_FEVTERRSTAT_CURLIMITE_DEFAULT << 23) /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12E                         (0x1UL << 24)                               /**< Force Event for Auto CMD Error */
#define _SDIO_FEVTERRSTAT_AC12E_SHIFT                  24                                          /**< Shift value for SDIO_AC12E */
#define _SDIO_FEVTERRSTAT_AC12E_MASK                   0x1000000UL                                 /**< Bit mask for SDIO_AC12E */
#define _SDIO_FEVTERRSTAT_AC12E_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_AC12E_DEFAULT                 (_SDIO_FEVTERRSTAT_AC12E_DEFAULT << 24)     /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_ADMAE                         (0x1UL << 25)                               /**< Force Event for ADMA Error */
#define _SDIO_FEVTERRSTAT_ADMAE_SHIFT                  25                                          /**< Shift value for SDIO_ADMAE */
#define _SDIO_FEVTERRSTAT_ADMAE_MASK                   0x2000000UL                                 /**< Bit mask for SDIO_ADMAE */
#define _SDIO_FEVTERRSTAT_ADMAE_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_ADMAE_DEFAULT                 (_SDIO_FEVTERRSTAT_ADMAE_DEFAULT << 25)     /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_TUNINGE                       (0x1UL << 26)                               /**< Force Event for Tuning Errro */
#define _SDIO_FEVTERRSTAT_TUNINGE_SHIFT                26                                          /**< Shift value for SDIO_TUNINGE */
#define _SDIO_FEVTERRSTAT_TUNINGE_MASK                 0x4000000UL                                 /**< Bit mask for SDIO_TUNINGE */
#define _SDIO_FEVTERRSTAT_TUNINGE_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_TUNINGE_DEFAULT               (_SDIO_FEVTERRSTAT_TUNINGE_DEFAULT << 26)   /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */
#define _SDIO_FEVTERRSTAT_VENSPECE_SHIFT               28                                          /**< Shift value for SDIO_VENSPECE */
#define _SDIO_FEVTERRSTAT_VENSPECE_MASK                0xF0000000UL                                /**< Bit mask for SDIO_VENSPECE */
#define _SDIO_FEVTERRSTAT_VENSPECE_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for SDIO_FEVTERRSTAT */
#define SDIO_FEVTERRSTAT_VENSPECE_DEFAULT              (_SDIO_FEVTERRSTAT_VENSPECE_DEFAULT << 28)  /**< Shifted mode DEFAULT for SDIO_FEVTERRSTAT */

/* Bit fields for SDIO ADMAES */
#define _SDIO_ADMAES_RESETVALUE                        0x00000000UL                        /**< Default value for SDIO_ADMAES */
#define _SDIO_ADMAES_MASK                              0x00000007UL                        /**< Mask for SDIO_ADMAES */
#define _SDIO_ADMAES_ADMAES_SHIFT                      0                                   /**< Shift value for SDIO_ADMAES */
#define _SDIO_ADMAES_ADMAES_MASK                       0x3UL                               /**< Bit mask for SDIO_ADMAES */
#define _SDIO_ADMAES_ADMAES_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for SDIO_ADMAES */
#define SDIO_ADMAES_ADMAES_DEFAULT                     (_SDIO_ADMAES_ADMAES_DEFAULT << 0)  /**< Shifted mode DEFAULT for SDIO_ADMAES */
#define SDIO_ADMAES_ADMALME                            (0x1UL << 2)                        /**< ADMA Length Mismatch Error */
#define _SDIO_ADMAES_ADMALME_SHIFT                     2                                   /**< Shift value for SDIO_ADMALME */
#define _SDIO_ADMAES_ADMALME_MASK                      0x4UL                               /**< Bit mask for SDIO_ADMALME */
#define _SDIO_ADMAES_ADMALME_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for SDIO_ADMAES */
#define SDIO_ADMAES_ADMALME_DEFAULT                    (_SDIO_ADMAES_ADMALME_DEFAULT << 2) /**< Shifted mode DEFAULT for SDIO_ADMAES */

/* Bit fields for SDIO ADSADDR */
#define _SDIO_ADSADDR_RESETVALUE                       0x00000000UL                         /**< Default value for SDIO_ADSADDR */
#define _SDIO_ADSADDR_MASK                             0xFFFFFFFFUL                         /**< Mask for SDIO_ADSADDR */
#define _SDIO_ADSADDR_ADSADDR_SHIFT                    0                                    /**< Shift value for SDIO_ADSADDR */
#define _SDIO_ADSADDR_ADSADDR_MASK                     0xFFFFFFFFUL                         /**< Bit mask for SDIO_ADSADDR */
#define _SDIO_ADSADDR_ADSADDR_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for SDIO_ADSADDR */
#define SDIO_ADSADDR_ADSADDR_DEFAULT                   (_SDIO_ADSADDR_ADSADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_ADSADDR */

/* Bit fields for SDIO PRSTVAL0 */
#define _SDIO_PRSTVAL0_RESETVALUE                      0x00000000UL                                   /**< Default value for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_MASK                            0xC7FFC7FFUL                                   /**< Mask for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_INITSDCLKFREQVAL_SHIFT          0                                              /**< Shift value for SDIO_INITSDCLKFREQVAL */
#define _SDIO_PRSTVAL0_INITSDCLKFREQVAL_MASK           0x3FFUL                                        /**< Bit mask for SDIO_INITSDCLKFREQVAL */
#define _SDIO_PRSTVAL0_INITSDCLKFREQVAL_DEFAULT        0x00000000UL                                   /**< Mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITSDCLKFREQVAL_DEFAULT         (_SDIO_PRSTVAL0_INITSDCLKFREQVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITCLCKGENVAL                   (0x1UL << 10)                                  /**< Clock Generator Select Value for Initialization */
#define _SDIO_PRSTVAL0_INITCLCKGENVAL_SHIFT            10                                             /**< Shift value for SDIO_INITCLCKGENVAL */
#define _SDIO_PRSTVAL0_INITCLCKGENVAL_MASK             0x400UL                                        /**< Bit mask for SDIO_INITCLCKGENVAL */
#define _SDIO_PRSTVAL0_INITCLCKGENVAL_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITCLCKGENVAL_DEFAULT           (_SDIO_PRSTVAL0_INITCLCKGENVAL_DEFAULT << 10)  /**< Shifted mode DEFAULT for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_SHIFT              14                                             /**< Shift value for SDIO_INITDRVSTVAL */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_MASK               0xC000UL                                       /**< Bit mask for SDIO_INITDRVSTVAL */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_TYPEB              0x00000000UL                                   /**< Mode TYPEB for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_TYPEA              0x00000001UL                                   /**< Mode TYPEA for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_TYPEC              0x00000002UL                                   /**< Mode TYPEC for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_INITDRVSTVAL_TYPED              0x00000003UL                                   /**< Mode TYPED for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITDRVSTVAL_DEFAULT             (_SDIO_PRSTVAL0_INITDRVSTVAL_DEFAULT << 14)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITDRVSTVAL_TYPEB               (_SDIO_PRSTVAL0_INITDRVSTVAL_TYPEB << 14)      /**< Shifted mode TYPEB for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITDRVSTVAL_TYPEA               (_SDIO_PRSTVAL0_INITDRVSTVAL_TYPEA << 14)      /**< Shifted mode TYPEA for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITDRVSTVAL_TYPEC               (_SDIO_PRSTVAL0_INITDRVSTVAL_TYPEC << 14)      /**< Shifted mode TYPEC for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_INITDRVSTVAL_TYPED               (_SDIO_PRSTVAL0_INITDRVSTVAL_TYPED << 14)      /**< Shifted mode TYPED for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_DSPSDCLKFREQVAL_SHIFT           16                                             /**< Shift value for SDIO_DSPSDCLKFREQVAL */
#define _SDIO_PRSTVAL0_DSPSDCLKFREQVAL_MASK            0x3FF0000UL                                    /**< Bit mask for SDIO_DSPSDCLKFREQVAL */
#define _SDIO_PRSTVAL0_DSPSDCLKFREQVAL_DEFAULT         0x00000000UL                                   /**< Mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPSDCLKFREQVAL_DEFAULT          (_SDIO_PRSTVAL0_DSPSDCLKFREQVAL_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPCLKGENVAL                     (0x1UL << 26)                                  /**< Clock Generator Select Value for Default Speed */
#define _SDIO_PRSTVAL0_DSPCLKGENVAL_SHIFT              26                                             /**< Shift value for SDIO_DSPCLKGENVAL */
#define _SDIO_PRSTVAL0_DSPCLKGENVAL_MASK               0x4000000UL                                    /**< Bit mask for SDIO_DSPCLKGENVAL */
#define _SDIO_PRSTVAL0_DSPCLKGENVAL_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPCLKGENVAL_DEFAULT             (_SDIO_PRSTVAL0_DSPCLKGENVAL_DEFAULT << 26)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_SHIFT               30                                             /**< Shift value for SDIO_DSPDRVSTVAL */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_MASK                0xC0000000UL                                   /**< Bit mask for SDIO_DSPDRVSTVAL */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEB               0x00000000UL                                   /**< Mode TYPEB for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEA               0x00000001UL                                   /**< Mode TYPEA for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEC               0x00000002UL                                   /**< Mode TYPEC for SDIO_PRSTVAL0 */
#define _SDIO_PRSTVAL0_DSPDRVSTVAL_TYPED               0x00000003UL                                   /**< Mode TYPED for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPDRVSTVAL_DEFAULT              (_SDIO_PRSTVAL0_DSPDRVSTVAL_DEFAULT << 30)     /**< Shifted mode DEFAULT for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEB                (_SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEB << 30)       /**< Shifted mode TYPEB for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEA                (_SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEA << 30)       /**< Shifted mode TYPEA for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEC                (_SDIO_PRSTVAL0_DSPDRVSTVAL_TYPEC << 30)       /**< Shifted mode TYPEC for SDIO_PRSTVAL0 */
#define SDIO_PRSTVAL0_DSPDRVSTVAL_TYPED                (_SDIO_PRSTVAL0_DSPDRVSTVAL_TYPED << 30)       /**< Shifted mode TYPED for SDIO_PRSTVAL0 */

/* Bit fields for SDIO PRSTVAL2 */
#define _SDIO_PRSTVAL2_RESETVALUE                      0x00000000UL                                     /**< Default value for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_MASK                            0xC7FFC7FFUL                                     /**< Mask for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_HSPSDCLKFREQVAL_SHIFT           0                                                /**< Shift value for SDIO_HSPSDCLKFREQVAL */
#define _SDIO_PRSTVAL2_HSPSDCLKFREQVAL_MASK            0x3FFUL                                          /**< Bit mask for SDIO_HSPSDCLKFREQVAL */
#define _SDIO_PRSTVAL2_HSPSDCLKFREQVAL_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPSDCLKFREQVAL_DEFAULT          (_SDIO_PRSTVAL2_HSPSDCLKFREQVAL_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPCLKGENVAL                     (0x1UL << 10)                                    /**< Clock Generator Select Value for High Speed */
#define _SDIO_PRSTVAL2_HSPCLKGENVAL_SHIFT              10                                               /**< Shift value for SDIO_HSPCLKGENVAL */
#define _SDIO_PRSTVAL2_HSPCLKGENVAL_MASK               0x400UL                                          /**< Bit mask for SDIO_HSPCLKGENVAL */
#define _SDIO_PRSTVAL2_HSPCLKGENVAL_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPCLKGENVAL_DEFAULT             (_SDIO_PRSTVAL2_HSPCLKGENVAL_DEFAULT << 10)      /**< Shifted mode DEFAULT for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_SHIFT               14                                               /**< Shift value for SDIO_HSPDRVSTVAL */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_MASK                0xC000UL                                         /**< Bit mask for SDIO_HSPDRVSTVAL */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEB               0x00000000UL                                     /**< Mode TYPEB for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEA               0x00000001UL                                     /**< Mode TYPEA for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEC               0x00000002UL                                     /**< Mode TYPEC for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_HSPDRVSTVAL_TYPED               0x00000003UL                                     /**< Mode TYPED for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPDRVSTVAL_DEFAULT              (_SDIO_PRSTVAL2_HSPDRVSTVAL_DEFAULT << 14)       /**< Shifted mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEB                (_SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEB << 14)         /**< Shifted mode TYPEB for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEA                (_SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEA << 14)         /**< Shifted mode TYPEA for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEC                (_SDIO_PRSTVAL2_HSPDRVSTVAL_TYPEC << 14)         /**< Shifted mode TYPEC for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_HSPDRVSTVAL_TYPED                (_SDIO_PRSTVAL2_HSPDRVSTVAL_TYPED << 14)         /**< Shifted mode TYPED for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_SDR12SDCLKFREQVAL_SHIFT         16                                               /**< Shift value for SDIO_SDR12SDCLKFREQVAL */
#define _SDIO_PRSTVAL2_SDR12SDCLKFREQVAL_MASK          0x3FF0000UL                                      /**< Bit mask for SDIO_SDR12SDCLKFREQVAL */
#define _SDIO_PRSTVAL2_SDR12SDCLKFREQVAL_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12SDCLKFREQVAL_DEFAULT        (_SDIO_PRSTVAL2_SDR12SDCLKFREQVAL_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12CLKGENVAL                   (0x1UL << 26)                                    /**< Clock Generator Select Value for SDR12 */
#define _SDIO_PRSTVAL2_SDR12CLKGENVAL_SHIFT            26                                               /**< Shift value for SDIO_SDR12CLKGENVAL */
#define _SDIO_PRSTVAL2_SDR12CLKGENVAL_MASK             0x4000000UL                                      /**< Bit mask for SDIO_SDR12CLKGENVAL */
#define _SDIO_PRSTVAL2_SDR12CLKGENVAL_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12CLKGENVAL_DEFAULT           (_SDIO_PRSTVAL2_SDR12CLKGENVAL_DEFAULT << 26)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_SHIFT             30                                               /**< Shift value for SDIO_SDR12DRVSTVAL */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_MASK              0xC0000000UL                                     /**< Bit mask for SDIO_SDR12DRVSTVAL */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEB             0x00000000UL                                     /**< Mode TYPEB for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEA             0x00000001UL                                     /**< Mode TYPEA for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEC             0x00000002UL                                     /**< Mode TYPEC for SDIO_PRSTVAL2 */
#define _SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPED             0x00000003UL                                     /**< Mode TYPED for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12DRVSTVAL_DEFAULT            (_SDIO_PRSTVAL2_SDR12DRVSTVAL_DEFAULT << 30)     /**< Shifted mode DEFAULT for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEB              (_SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEB << 30)       /**< Shifted mode TYPEB for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEA              (_SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEA << 30)       /**< Shifted mode TYPEA for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEC              (_SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPEC << 30)       /**< Shifted mode TYPEC for SDIO_PRSTVAL2 */
#define SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPED              (_SDIO_PRSTVAL2_SDR12DRVSTVAL_TYPED << 30)       /**< Shifted mode TYPED for SDIO_PRSTVAL2 */

/* Bit fields for SDIO PRSTVAL4 */
#define _SDIO_PRSTVAL4_RESETVALUE                      0x00000000UL                                     /**< Default value for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_MASK                            0xC7FFC7FFUL                                     /**< Mask for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR25SDCLKFREQVAL_SHIFT         0                                                /**< Shift value for SDIO_SDR25SDCLKFREQVAL */
#define _SDIO_PRSTVAL4_SDR25SDCLKFREQVAL_MASK          0x3FFUL                                          /**< Bit mask for SDIO_SDR25SDCLKFREQVAL */
#define _SDIO_PRSTVAL4_SDR25SDCLKFREQVAL_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25SDCLKFREQVAL_DEFAULT        (_SDIO_PRSTVAL4_SDR25SDCLKFREQVAL_DEFAULT << 0)  /**< Shifted mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25CLKGENVAL                   (0x1UL << 10)                                    /**< Clock Generator Select Value for SDR25 */
#define _SDIO_PRSTVAL4_SDR25CLKGENVAL_SHIFT            10                                               /**< Shift value for SDIO_SDR25CLKGENVAL */
#define _SDIO_PRSTVAL4_SDR25CLKGENVAL_MASK             0x400UL                                          /**< Bit mask for SDIO_SDR25CLKGENVAL */
#define _SDIO_PRSTVAL4_SDR25CLKGENVAL_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25CLKGENVAL_DEFAULT           (_SDIO_PRSTVAL4_SDR25CLKGENVAL_DEFAULT << 10)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_SHIFT             14                                               /**< Shift value for SDIO_SDR25DRVSTVAL */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_MASK              0xC000UL                                         /**< Bit mask for SDIO_SDR25DRVSTVAL */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEB             0x00000000UL                                     /**< Mode TYPEB for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEA             0x00000001UL                                     /**< Mode TYPEA for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEC             0x00000002UL                                     /**< Mode TYPEC for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPED             0x00000003UL                                     /**< Mode TYPED for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25DRVSTVAL_DEFAULT            (_SDIO_PRSTVAL4_SDR25DRVSTVAL_DEFAULT << 14)     /**< Shifted mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEB              (_SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEB << 14)       /**< Shifted mode TYPEB for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEA              (_SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEA << 14)       /**< Shifted mode TYPEA for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEC              (_SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPEC << 14)       /**< Shifted mode TYPEC for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPED              (_SDIO_PRSTVAL4_SDR25DRVSTVAL_TYPED << 14)       /**< Shifted mode TYPED for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR50SDCLKFREQVAL_SHIFT         16                                               /**< Shift value for SDIO_SDR50SDCLKFREQVAL */
#define _SDIO_PRSTVAL4_SDR50SDCLKFREQVAL_MASK          0x3FF0000UL                                      /**< Bit mask for SDIO_SDR50SDCLKFREQVAL */
#define _SDIO_PRSTVAL4_SDR50SDCLKFREQVAL_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50SDCLKFREQVAL_DEFAULT        (_SDIO_PRSTVAL4_SDR50SDCLKFREQVAL_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50CLCKGENVAL                  (0x1UL << 26)                                    /**< Clock Generator Select Value for SDR50 */
#define _SDIO_PRSTVAL4_SDR50CLCKGENVAL_SHIFT           26                                               /**< Shift value for SDIO_SDR50CLCKGENVAL */
#define _SDIO_PRSTVAL4_SDR50CLCKGENVAL_MASK            0x4000000UL                                      /**< Bit mask for SDIO_SDR50CLCKGENVAL */
#define _SDIO_PRSTVAL4_SDR50CLCKGENVAL_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50CLCKGENVAL_DEFAULT          (_SDIO_PRSTVAL4_SDR50CLCKGENVAL_DEFAULT << 26)   /**< Shifted mode DEFAULT for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_SHIFT             30                                               /**< Shift value for SDIO_SDR50DRVSTVAL */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_MASK              0xC0000000UL                                     /**< Bit mask for SDIO_SDR50DRVSTVAL */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEB             0x00000000UL                                     /**< Mode TYPEB for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEA             0x00000001UL                                     /**< Mode TYPEA for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEC             0x00000002UL                                     /**< Mode TYPEC for SDIO_PRSTVAL4 */
#define _SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPED             0x00000003UL                                     /**< Mode TYPED for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50DRVSTVAL_DEFAULT            (_SDIO_PRSTVAL4_SDR50DRVSTVAL_DEFAULT << 30)     /**< Shifted mode DEFAULT for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEB              (_SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEB << 30)       /**< Shifted mode TYPEB for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEA              (_SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEA << 30)       /**< Shifted mode TYPEA for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEC              (_SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPEC << 30)       /**< Shifted mode TYPEC for SDIO_PRSTVAL4 */
#define SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPED              (_SDIO_PRSTVAL4_SDR50DRVSTVAL_TYPED << 30)       /**< Shifted mode TYPED for SDIO_PRSTVAL4 */

/* Bit fields for SDIO PRSTVAL6 */
#define _SDIO_PRSTVAL6_RESETVALUE                      0x00000000UL                                     /**< Default value for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_MASK                            0xC7FFC7FFUL                                     /**< Mask for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_SDR104SDCLKFREQVAL_SHIFT        0                                                /**< Shift value for SDIO_SDR104SDCLKFREQVAL */
#define _SDIO_PRSTVAL6_SDR104SDCLKFREQVAL_MASK         0x3FFUL                                          /**< Bit mask for SDIO_SDR104SDCLKFREQVAL */
#define _SDIO_PRSTVAL6_SDR104SDCLKFREQVAL_DEFAULT      0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104SDCLKFREQVAL_DEFAULT       (_SDIO_PRSTVAL6_SDR104SDCLKFREQVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104CLKGENVAL                  (0x1UL << 10)                                    /**< Clock Generator Select Value for SDR104 */
#define _SDIO_PRSTVAL6_SDR104CLKGENVAL_SHIFT           10                                               /**< Shift value for SDIO_SDR104CLKGENVAL */
#define _SDIO_PRSTVAL6_SDR104CLKGENVAL_MASK            0x400UL                                          /**< Bit mask for SDIO_SDR104CLKGENVAL */
#define _SDIO_PRSTVAL6_SDR104CLKGENVAL_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104CLKGENVAL_DEFAULT          (_SDIO_PRSTVAL6_SDR104CLKGENVAL_DEFAULT << 10)   /**< Shifted mode DEFAULT for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_SHIFT            14                                               /**< Shift value for SDIO_SDR104DRVSTVAL */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_MASK             0xC000UL                                         /**< Bit mask for SDIO_SDR104DRVSTVAL */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEB            0x00000000UL                                     /**< Mode TYPEB for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEA            0x00000001UL                                     /**< Mode TYPEA for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEC            0x00000002UL                                     /**< Mode TYPEC for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPED            0x00000003UL                                     /**< Mode TYPED for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104DRVSTVAL_DEFAULT           (_SDIO_PRSTVAL6_SDR104DRVSTVAL_DEFAULT << 14)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEB             (_SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEB << 14)      /**< Shifted mode TYPEB for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEA             (_SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEA << 14)      /**< Shifted mode TYPEA for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEC             (_SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPEC << 14)      /**< Shifted mode TYPEC for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPED             (_SDIO_PRSTVAL6_SDR104DRVSTVAL_TYPED << 14)      /**< Shifted mode TYPED for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_DDR50SDCLKFREQVAL_SHIFT         16                                               /**< Shift value for SDIO_DDR50SDCLKFREQVAL */
#define _SDIO_PRSTVAL6_DDR50SDCLKFREQVAL_MASK          0x3FF0000UL                                      /**< Bit mask for SDIO_DDR50SDCLKFREQVAL */
#define _SDIO_PRSTVAL6_DDR50SDCLKFREQVAL_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50SDCLKFREQVAL_DEFAULT        (_SDIO_PRSTVAL6_DDR50SDCLKFREQVAL_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50CLKGENVAL                   (0x1UL << 26)                                    /**< Clock Generator Select Value for DDR50 */
#define _SDIO_PRSTVAL6_DDR50CLKGENVAL_SHIFT            26                                               /**< Shift value for SDIO_DDR50CLKGENVAL */
#define _SDIO_PRSTVAL6_DDR50CLKGENVAL_MASK             0x4000000UL                                      /**< Bit mask for SDIO_DDR50CLKGENVAL */
#define _SDIO_PRSTVAL6_DDR50CLKGENVAL_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50CLKGENVAL_DEFAULT           (_SDIO_PRSTVAL6_DDR50CLKGENVAL_DEFAULT << 26)    /**< Shifted mode DEFAULT for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_SHIFT             30                                               /**< Shift value for SDIO_DDR50DRVSTVAL */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_MASK              0xC0000000UL                                     /**< Bit mask for SDIO_DDR50DRVSTVAL */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEB             0x00000000UL                                     /**< Mode TYPEB for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEA             0x00000001UL                                     /**< Mode TYPEA for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEC             0x00000002UL                                     /**< Mode TYPEC for SDIO_PRSTVAL6 */
#define _SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPED             0x00000003UL                                     /**< Mode TYPED for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50DRVSTVAL_DEFAULT            (_SDIO_PRSTVAL6_DDR50DRVSTVAL_DEFAULT << 30)     /**< Shifted mode DEFAULT for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEB              (_SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEB << 30)       /**< Shifted mode TYPEB for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEA              (_SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEA << 30)       /**< Shifted mode TYPEA for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEC              (_SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPEC << 30)       /**< Shifted mode TYPEC for SDIO_PRSTVAL6 */
#define SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPED              (_SDIO_PRSTVAL6_DDR50DRVSTVAL_TYPED << 30)       /**< Shifted mode TYPED for SDIO_PRSTVAL6 */

/* Bit fields for SDIO BOOTTOCTRL */
#define _SDIO_BOOTTOCTRL_RESETVALUE                    0x00000000UL                                 /**< Default value for SDIO_BOOTTOCTRL */
#define _SDIO_BOOTTOCTRL_MASK                          0xFFFFFFFFUL                                 /**< Mask for SDIO_BOOTTOCTRL */
#define _SDIO_BOOTTOCTRL_BOOTDATTOCNT_SHIFT            0                                            /**< Shift value for SDIO_BOOTDATTOCNT */
#define _SDIO_BOOTTOCTRL_BOOTDATTOCNT_MASK             0xFFFFFFFFUL                                 /**< Bit mask for SDIO_BOOTDATTOCNT */
#define _SDIO_BOOTTOCTRL_BOOTDATTOCNT_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for SDIO_BOOTTOCTRL */
#define SDIO_BOOTTOCTRL_BOOTDATTOCNT_DEFAULT           (_SDIO_BOOTTOCTRL_BOOTDATTOCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_BOOTTOCTRL */

/* Bit fields for SDIO SLOTINTSTAT */
#define _SDIO_SLOTINTSTAT_RESETVALUE                   0x10020000UL                                 /**< Default value for SDIO_SLOTINTSTAT */
#define _SDIO_SLOTINTSTAT_MASK                         0xFFFF0001UL                                 /**< Mask for SDIO_SLOTINTSTAT */
#define SDIO_SLOTINTSTAT_INTSLOT0                      (0x1UL << 0)                                 /**< Interrupt Signal for Slot#0 */
#define _SDIO_SLOTINTSTAT_INTSLOT0_SHIFT               0                                            /**< Shift value for SDIO_INTSLOT0 */
#define _SDIO_SLOTINTSTAT_INTSLOT0_MASK                0x1UL                                        /**< Bit mask for SDIO_INTSLOT0 */
#define _SDIO_SLOTINTSTAT_INTSLOT0_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SDIO_SLOTINTSTAT */
#define SDIO_SLOTINTSTAT_INTSLOT0_DEFAULT              (_SDIO_SLOTINTSTAT_INTSLOT0_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_SLOTINTSTAT */
#define _SDIO_SLOTINTSTAT_SPECVERNUM_SHIFT             16                                           /**< Shift value for SDIO_SPECVERNUM */
#define _SDIO_SLOTINTSTAT_SPECVERNUM_MASK              0xFF0000UL                                   /**< Bit mask for SDIO_SPECVERNUM */
#define _SDIO_SLOTINTSTAT_SPECVERNUM_DEFAULT           0x00000002UL                                 /**< Mode DEFAULT for SDIO_SLOTINTSTAT */
#define SDIO_SLOTINTSTAT_SPECVERNUM_DEFAULT            (_SDIO_SLOTINTSTAT_SPECVERNUM_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_SLOTINTSTAT */
#define _SDIO_SLOTINTSTAT_VENDVERNUM_SHIFT             24                                           /**< Shift value for SDIO_VENDVERNUM */
#define _SDIO_SLOTINTSTAT_VENDVERNUM_MASK              0xFF000000UL                                 /**< Bit mask for SDIO_VENDVERNUM */
#define _SDIO_SLOTINTSTAT_VENDVERNUM_DEFAULT           0x00000010UL                                 /**< Mode DEFAULT for SDIO_SLOTINTSTAT */
#define SDIO_SLOTINTSTAT_VENDVERNUM_DEFAULT            (_SDIO_SLOTINTSTAT_VENDVERNUM_DEFAULT << 24) /**< Shifted mode DEFAULT for SDIO_SLOTINTSTAT */

/* Bit fields for SDIO CTRL */
#define _SDIO_CTRL_RESETVALUE                          0x00000000UL                           /**< Default value for SDIO_CTRL */
#define _SDIO_CTRL_MASK                                0x00030FFFUL                           /**< Mask for SDIO_CTRL */
#define SDIO_CTRL_ITAPDLYEN                            (0x1UL << 0)                           /**< Selective Tap Delay Line Enable on Rxclk_in */
#define _SDIO_CTRL_ITAPDLYEN_SHIFT                     0                                      /**< Shift value for SDIO_ITAPDLYEN */
#define _SDIO_CTRL_ITAPDLYEN_MASK                      0x1UL                                  /**< Bit mask for SDIO_ITAPDLYEN */
#define _SDIO_CTRL_ITAPDLYEN_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_ITAPDLYEN_DEFAULT                    (_SDIO_CTRL_ITAPDLYEN_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_CTRL */
#define _SDIO_CTRL_ITAPDLYSEL_SHIFT                    1                                      /**< Shift value for SDIO_ITAPDLYSEL */
#define _SDIO_CTRL_ITAPDLYSEL_MASK                     0x3EUL                                 /**< Bit mask for SDIO_ITAPDLYSEL */
#define _SDIO_CTRL_ITAPDLYSEL_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_ITAPDLYSEL_DEFAULT                   (_SDIO_CTRL_ITAPDLYSEL_DEFAULT << 1)   /**< Shifted mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_ITAPCHGWIN                           (0x1UL << 6)                           /**< Gating Signal for Tap Delay Change */
#define _SDIO_CTRL_ITAPCHGWIN_SHIFT                    6                                      /**< Shift value for SDIO_ITAPCHGWIN */
#define _SDIO_CTRL_ITAPCHGWIN_MASK                     0x40UL                                 /**< Bit mask for SDIO_ITAPCHGWIN */
#define _SDIO_CTRL_ITAPCHGWIN_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_ITAPCHGWIN_DEFAULT                   (_SDIO_CTRL_ITAPCHGWIN_DEFAULT << 6)   /**< Shifted mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_OTAPDLYEN                            (0x1UL << 7)                           /**< Selective Tap Delay Line Enable on SDIO_CLK Pin */
#define _SDIO_CTRL_OTAPDLYEN_SHIFT                     7                                      /**< Shift value for SDIO_OTAPDLYEN */
#define _SDIO_CTRL_OTAPDLYEN_MASK                      0x80UL                                 /**< Bit mask for SDIO_OTAPDLYEN */
#define _SDIO_CTRL_OTAPDLYEN_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_OTAPDLYEN_DEFAULT                    (_SDIO_CTRL_OTAPDLYEN_DEFAULT << 7)    /**< Shifted mode DEFAULT for SDIO_CTRL */
#define _SDIO_CTRL_OTAPDLYSEL_SHIFT                    8                                      /**< Shift value for SDIO_OTAPDLYSEL */
#define _SDIO_CTRL_OTAPDLYSEL_MASK                     0xF00UL                                /**< Bit mask for SDIO_OTAPDLYSEL */
#define _SDIO_CTRL_OTAPDLYSEL_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_OTAPDLYSEL_DEFAULT                   (_SDIO_CTRL_OTAPDLYSEL_DEFAULT << 8)   /**< Shifted mode DEFAULT for SDIO_CTRL */
#define _SDIO_CTRL_TXDLYMUXSEL_SHIFT                   16                                     /**< Shift value for SDIO_TXDLYMUXSEL */
#define _SDIO_CTRL_TXDLYMUXSEL_MASK                    0x30000UL                              /**< Bit mask for SDIO_TXDLYMUXSEL */
#define _SDIO_CTRL_TXDLYMUXSEL_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CTRL */
#define SDIO_CTRL_TXDLYMUXSEL_DEFAULT                  (_SDIO_CTRL_TXDLYMUXSEL_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_CTRL */

/* Bit fields for SDIO CFG0 */
#define _SDIO_CFG0_RESETVALUE                          0x00000000UL                           /**< Default value for SDIO_CFG0 */
#define _SDIO_CFG0_MASK                                0x7FFFFFFFUL                           /**< Mask for SDIO_CFG0 */
#define _SDIO_CFG0_TUNINGCNT_SHIFT                     0                                      /**< Shift value for SDIO_TUNINGCNT */
#define _SDIO_CFG0_TUNINGCNT_MASK                      0x3FUL                                 /**< Bit mask for SDIO_TUNINGCNT */
#define _SDIO_CFG0_TUNINGCNT_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_TUNINGCNT_DEFAULT                    (_SDIO_CFG0_TUNINGCNT_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define _SDIO_CFG0_TOUTCLKFREQ_SHIFT                   6                                      /**< Shift value for SDIO_TOUTCLKFREQ */
#define _SDIO_CFG0_TOUTCLKFREQ_MASK                    0xFC0UL                                /**< Bit mask for SDIO_TOUTCLKFREQ */
#define _SDIO_CFG0_TOUTCLKFREQ_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_TOUTCLKFREQ_DEFAULT                  (_SDIO_CFG0_TOUTCLKFREQ_DEFAULT << 6)  /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_TOUTCLKUNIT                          (0x1UL << 12)                          /**< Timeout Clock Unit in kHz or MHz */
#define _SDIO_CFG0_TOUTCLKUNIT_SHIFT                   12                                     /**< Shift value for SDIO_TOUTCLKUNIT */
#define _SDIO_CFG0_TOUTCLKUNIT_MASK                    0x1000UL                               /**< Bit mask for SDIO_TOUTCLKUNIT */
#define _SDIO_CFG0_TOUTCLKUNIT_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_TOUTCLKUNIT_DEFAULT                  (_SDIO_CFG0_TOUTCLKUNIT_DEFAULT << 12) /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define _SDIO_CFG0_BASECLKFREQ_SHIFT                   13                                     /**< Shift value for SDIO_BASECLKFREQ */
#define _SDIO_CFG0_BASECLKFREQ_MASK                    0x1FE000UL                             /**< Bit mask for SDIO_BASECLKFREQ */
#define _SDIO_CFG0_BASECLKFREQ_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_BASECLKFREQ_DEFAULT                  (_SDIO_CFG0_BASECLKFREQ_DEFAULT << 13) /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define _SDIO_CFG0_MAXBLKLEN_SHIFT                     21                                     /**< Shift value for SDIO_MAXBLKLEN */
#define _SDIO_CFG0_MAXBLKLEN_MASK                      0x600000UL                             /**< Bit mask for SDIO_MAXBLKLEN */
#define _SDIO_CFG0_MAXBLKLEN_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define _SDIO_CFG0_MAXBLKLEN_512B                      0x00000000UL                           /**< Mode 512B for SDIO_CFG0 */
#define _SDIO_CFG0_MAXBLKLEN_1024B                     0x00000001UL                           /**< Mode 1024B for SDIO_CFG0 */
#define _SDIO_CFG0_MAXBLKLEN_2048B                     0x00000002UL                           /**< Mode 2048B for SDIO_CFG0 */
#define SDIO_CFG0_MAXBLKLEN_DEFAULT                    (_SDIO_CFG0_MAXBLKLEN_DEFAULT << 21)   /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_MAXBLKLEN_512B                       (_SDIO_CFG0_MAXBLKLEN_512B << 21)      /**< Shifted mode 512B for SDIO_CFG0 */
#define SDIO_CFG0_MAXBLKLEN_1024B                      (_SDIO_CFG0_MAXBLKLEN_1024B << 21)     /**< Shifted mode 1024B for SDIO_CFG0 */
#define SDIO_CFG0_MAXBLKLEN_2048B                      (_SDIO_CFG0_MAXBLKLEN_2048B << 21)     /**< Shifted mode 2048B for SDIO_CFG0 */
#define SDIO_CFG0_C8BITSUP                             (0x1UL << 23)                          /**< 8-bit Interface Support */
#define _SDIO_CFG0_C8BITSUP_SHIFT                      23                                     /**< Shift value for SDIO_C8BITSUP */
#define _SDIO_CFG0_C8BITSUP_MASK                       0x800000UL                             /**< Bit mask for SDIO_C8BITSUP */
#define _SDIO_CFG0_C8BITSUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C8BITSUP_DEFAULT                     (_SDIO_CFG0_C8BITSUP_DEFAULT << 23)    /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CADMA2SUP                            (0x1UL << 24)                          /**< ADMA2 Mode Support */
#define _SDIO_CFG0_CADMA2SUP_SHIFT                     24                                     /**< Shift value for SDIO_CADMA2SUP */
#define _SDIO_CFG0_CADMA2SUP_MASK                      0x1000000UL                            /**< Bit mask for SDIO_CADMA2SUP */
#define _SDIO_CFG0_CADMA2SUP_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CADMA2SUP_DEFAULT                    (_SDIO_CFG0_CADMA2SUP_DEFAULT << 24)   /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CHSSUP                               (0x1UL << 25)                          /**< High Speed Mode Support */
#define _SDIO_CFG0_CHSSUP_SHIFT                        25                                     /**< Shift value for SDIO_CHSSUP */
#define _SDIO_CFG0_CHSSUP_MASK                         0x2000000UL                            /**< Bit mask for SDIO_CHSSUP */
#define _SDIO_CFG0_CHSSUP_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CHSSUP_DEFAULT                       (_SDIO_CFG0_CHSSUP_DEFAULT << 25)      /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CSDMASUP                             (0x1UL << 26)                          /**< SDMA Mode Support */
#define _SDIO_CFG0_CSDMASUP_SHIFT                      26                                     /**< Shift value for SDIO_CSDMASUP */
#define _SDIO_CFG0_CSDMASUP_MASK                       0x4000000UL                            /**< Bit mask for SDIO_CSDMASUP */
#define _SDIO_CFG0_CSDMASUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CSDMASUP_DEFAULT                     (_SDIO_CFG0_CSDMASUP_DEFAULT << 26)    /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CSUSPRESSUP                          (0x1UL << 27)                          /**< Suspend/Resume Support */
#define _SDIO_CFG0_CSUSPRESSUP_SHIFT                   27                                     /**< Shift value for SDIO_CSUSPRESSUP */
#define _SDIO_CFG0_CSUSPRESSUP_MASK                    0x8000000UL                            /**< Bit mask for SDIO_CSUSPRESSUP */
#define _SDIO_CFG0_CSUSPRESSUP_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_CSUSPRESSUP_DEFAULT                  (_SDIO_CFG0_CSUSPRESSUP_DEFAULT << 27) /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C3P3VSUP                             (0x1UL << 28)                          /**< Core 3P3V Support */
#define _SDIO_CFG0_C3P3VSUP_SHIFT                      28                                     /**< Shift value for SDIO_C3P3VSUP */
#define _SDIO_CFG0_C3P3VSUP_MASK                       0x10000000UL                           /**< Bit mask for SDIO_C3P3VSUP */
#define _SDIO_CFG0_C3P3VSUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C3P3VSUP_DEFAULT                     (_SDIO_CFG0_C3P3VSUP_DEFAULT << 28)    /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C3P0VSUP                             (0x1UL << 29)                          /**< 3P0V Support */
#define _SDIO_CFG0_C3P0VSUP_SHIFT                      29                                     /**< Shift value for SDIO_C3P0VSUP */
#define _SDIO_CFG0_C3P0VSUP_MASK                       0x20000000UL                           /**< Bit mask for SDIO_C3P0VSUP */
#define _SDIO_CFG0_C3P0VSUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C3P0VSUP_DEFAULT                     (_SDIO_CFG0_C3P0VSUP_DEFAULT << 29)    /**< Shifted mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C1P8VSUP                             (0x1UL << 30)                          /**< 1P8V Support */
#define _SDIO_CFG0_C1P8VSUP_SHIFT                      30                                     /**< Shift value for SDIO_C1P8VSUP */
#define _SDIO_CFG0_C1P8VSUP_MASK                       0x40000000UL                           /**< Bit mask for SDIO_C1P8VSUP */
#define _SDIO_CFG0_C1P8VSUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG0 */
#define SDIO_CFG0_C1P8VSUP_DEFAULT                     (_SDIO_CFG0_C1P8VSUP_DEFAULT << 30)    /**< Shifted mode DEFAULT for SDIO_CFG0 */

/* Bit fields for SDIO CFG1 */
#define _SDIO_CFG1_RESETVALUE                          0x00000000UL                           /**< Default value for SDIO_CFG1 */
#define _SDIO_CFG1_MASK                                0x0005FFFFUL                           /**< Mask for SDIO_CFG1 */
#define SDIO_CFG1_ASYNCINTRSUP                         (0x1UL << 0)                           /**< Asynchronous Interrupt Support */
#define _SDIO_CFG1_ASYNCINTRSUP_SHIFT                  0                                      /**< Shift value for SDIO_ASYNCINTRSUP */
#define _SDIO_CFG1_ASYNCINTRSUP_MASK                   0x1UL                                  /**< Bit mask for SDIO_ASYNCINTRSUP */
#define _SDIO_CFG1_ASYNCINTRSUP_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_ASYNCINTRSUP_DEFAULT                 (_SDIO_CFG1_ASYNCINTRSUP_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define _SDIO_CFG1_SLOTTYPE_SHIFT                      1                                      /**< Shift value for SDIO_SLOTTYPE */
#define _SDIO_CFG1_SLOTTYPE_MASK                       0x6UL                                  /**< Bit mask for SDIO_SLOTTYPE */
#define _SDIO_CFG1_SLOTTYPE_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define _SDIO_CFG1_SLOTTYPE_RMSDSLOT                   0x00000000UL                           /**< Mode RMSDSLOT for SDIO_CFG1 */
#define _SDIO_CFG1_SLOTTYPE_EMSDSLOT                   0x00000001UL                           /**< Mode EMSDSLOT for SDIO_CFG1 */
#define _SDIO_CFG1_SLOTTYPE_SHBUSSLOT                  0x00000002UL                           /**< Mode SHBUSSLOT for SDIO_CFG1 */
#define SDIO_CFG1_SLOTTYPE_DEFAULT                     (_SDIO_CFG1_SLOTTYPE_DEFAULT << 1)     /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_SLOTTYPE_RMSDSLOT                    (_SDIO_CFG1_SLOTTYPE_RMSDSLOT << 1)    /**< Shifted mode RMSDSLOT for SDIO_CFG1 */
#define SDIO_CFG1_SLOTTYPE_EMSDSLOT                    (_SDIO_CFG1_SLOTTYPE_EMSDSLOT << 1)    /**< Shifted mode EMSDSLOT for SDIO_CFG1 */
#define SDIO_CFG1_SLOTTYPE_SHBUSSLOT                   (_SDIO_CFG1_SLOTTYPE_SHBUSSLOT << 1)   /**< Shifted mode SHBUSSLOT for SDIO_CFG1 */
#define SDIO_CFG1_CSDR50SUP                            (0x1UL << 3)                           /**< Core Support SDR50 */
#define _SDIO_CFG1_CSDR50SUP_SHIFT                     3                                      /**< Shift value for SDIO_CSDR50SUP */
#define _SDIO_CFG1_CSDR50SUP_MASK                      0x8UL                                  /**< Bit mask for SDIO_CSDR50SUP */
#define _SDIO_CFG1_CSDR50SUP_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CSDR50SUP_DEFAULT                    (_SDIO_CFG1_CSDR50SUP_DEFAULT << 3)    /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CSDR104SUP                           (0x1UL << 4)                           /**< Support SDR104 */
#define _SDIO_CFG1_CSDR104SUP_SHIFT                    4                                      /**< Shift value for SDIO_CSDR104SUP */
#define _SDIO_CFG1_CSDR104SUP_MASK                     0x10UL                                 /**< Bit mask for SDIO_CSDR104SUP */
#define _SDIO_CFG1_CSDR104SUP_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CSDR104SUP_DEFAULT                   (_SDIO_CFG1_CSDR104SUP_DEFAULT << 4)   /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDDR50SUP                            (0x1UL << 5)                           /**< Support DDR50 */
#define _SDIO_CFG1_CDDR50SUP_SHIFT                     5                                      /**< Shift value for SDIO_CDDR50SUP */
#define _SDIO_CFG1_CDDR50SUP_MASK                      0x20UL                                 /**< Bit mask for SDIO_CDDR50SUP */
#define _SDIO_CFG1_CDDR50SUP_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDDR50SUP_DEFAULT                    (_SDIO_CFG1_CDDR50SUP_DEFAULT << 5)    /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDRVASUP                             (0x1UL << 6)                           /**< Support Type a Driver */
#define _SDIO_CFG1_CDRVASUP_SHIFT                      6                                      /**< Shift value for SDIO_CDRVASUP */
#define _SDIO_CFG1_CDRVASUP_MASK                       0x40UL                                 /**< Bit mask for SDIO_CDRVASUP */
#define _SDIO_CFG1_CDRVASUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDRVASUP_DEFAULT                     (_SDIO_CFG1_CDRVASUP_DEFAULT << 6)     /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDRVCSUP                             (0x1UL << 7)                           /**< Support Type C Driver */
#define _SDIO_CFG1_CDRVCSUP_SHIFT                      7                                      /**< Shift value for SDIO_CDRVCSUP */
#define _SDIO_CFG1_CDRVCSUP_MASK                       0x80UL                                 /**< Bit mask for SDIO_CDRVCSUP */
#define _SDIO_CFG1_CDRVCSUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDRVCSUP_DEFAULT                     (_SDIO_CFG1_CDRVCSUP_DEFAULT << 7)     /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDRVDSUP                             (0x1UL << 8)                           /**< Support Type D Driver */
#define _SDIO_CFG1_CDRVDSUP_SHIFT                      8                                      /**< Shift value for SDIO_CDRVDSUP */
#define _SDIO_CFG1_CDRVDSUP_MASK                       0x100UL                                /**< Bit mask for SDIO_CDRVDSUP */
#define _SDIO_CFG1_CDRVDSUP_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_CDRVDSUP_DEFAULT                     (_SDIO_CFG1_CDRVDSUP_DEFAULT << 8)     /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define _SDIO_CFG1_RETUNTMRCTL_SHIFT                   9                                      /**< Shift value for SDIO_RETUNTMRCTL */
#define _SDIO_CFG1_RETUNTMRCTL_MASK                    0x1E00UL                               /**< Bit mask for SDIO_RETUNTMRCTL */
#define _SDIO_CFG1_RETUNTMRCTL_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_RETUNTMRCTL_DEFAULT                  (_SDIO_CFG1_RETUNTMRCTL_DEFAULT << 9)  /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_TUNSDR50                             (0x1UL << 13)                          /**< Tuning for SDR50 */
#define _SDIO_CFG1_TUNSDR50_SHIFT                      13                                     /**< Shift value for SDIO_TUNSDR50 */
#define _SDIO_CFG1_TUNSDR50_MASK                       0x2000UL                               /**< Bit mask for SDIO_TUNSDR50 */
#define _SDIO_CFG1_TUNSDR50_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_TUNSDR50_DEFAULT                     (_SDIO_CFG1_TUNSDR50_DEFAULT << 13)    /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define _SDIO_CFG1_RETUNMODES_SHIFT                    14                                     /**< Shift value for SDIO_RETUNMODES */
#define _SDIO_CFG1_RETUNMODES_MASK                     0xC000UL                               /**< Bit mask for SDIO_RETUNMODES */
#define _SDIO_CFG1_RETUNMODES_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_RETUNMODES_DEFAULT                   (_SDIO_CFG1_RETUNMODES_DEFAULT << 14)  /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_SPISUP                               (0x1UL << 16)                          /**< SPI Support */
#define _SDIO_CFG1_SPISUP_SHIFT                        16                                     /**< Shift value for SDIO_SPISUP */
#define _SDIO_CFG1_SPISUP_MASK                         0x10000UL                              /**< Bit mask for SDIO_SPISUP */
#define _SDIO_CFG1_SPISUP_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_SPISUP_DEFAULT                       (_SDIO_CFG1_SPISUP_DEFAULT << 16)      /**< Shifted mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_ASYNCWKUPEN                          (0x1UL << 18)                          /**< Asynchronous Wakeup Enable */
#define _SDIO_CFG1_ASYNCWKUPEN_SHIFT                   18                                     /**< Shift value for SDIO_ASYNCWKUPEN */
#define _SDIO_CFG1_ASYNCWKUPEN_MASK                    0x40000UL                              /**< Bit mask for SDIO_ASYNCWKUPEN */
#define _SDIO_CFG1_ASYNCWKUPEN_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_CFG1 */
#define SDIO_CFG1_ASYNCWKUPEN_DEFAULT                  (_SDIO_CFG1_ASYNCWKUPEN_DEFAULT << 18) /**< Shifted mode DEFAULT for SDIO_CFG1 */

/* Bit fields for SDIO CFGPRESETVAL0 */
#define _SDIO_CFGPRESETVAL0_RESETVALUE                 0x00000000UL                                     /**< Default value for SDIO_CFGPRESETVAL0 */
#define _SDIO_CFGPRESETVAL0_MASK                       0x1FFF1FFFUL                                     /**< Mask for SDIO_CFGPRESETVAL0 */
#define _SDIO_CFGPRESETVAL0_INITSDCLKFREQ_SHIFT        0                                                /**< Shift value for SDIO_INITSDCLKFREQ */
#define _SDIO_CFGPRESETVAL0_INITSDCLKFREQ_MASK         0x3FFUL                                          /**< Bit mask for SDIO_INITSDCLKFREQ */
#define _SDIO_CFGPRESETVAL0_INITSDCLKFREQ_DEFAULT      0x00000000UL                                     /**< Mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_INITSDCLKFREQ_DEFAULT       (_SDIO_CFGPRESETVAL0_INITSDCLKFREQ_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_INITCLKGENEN                (0x1UL << 10)                                    /**< Initial Clock Gen Enable */
#define _SDIO_CFGPRESETVAL0_INITCLKGENEN_SHIFT         10                                               /**< Shift value for SDIO_INITCLKGENEN */
#define _SDIO_CFGPRESETVAL0_INITCLKGENEN_MASK          0x400UL                                          /**< Bit mask for SDIO_INITCLKGENEN */
#define _SDIO_CFGPRESETVAL0_INITCLKGENEN_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_INITCLKGENEN_DEFAULT        (_SDIO_CFGPRESETVAL0_INITCLKGENEN_DEFAULT << 10) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define _SDIO_CFGPRESETVAL0_INITDRVST_SHIFT            11                                               /**< Shift value for SDIO_INITDRVST */
#define _SDIO_CFGPRESETVAL0_INITDRVST_MASK             0x1800UL                                         /**< Bit mask for SDIO_INITDRVST */
#define _SDIO_CFGPRESETVAL0_INITDRVST_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_INITDRVST_DEFAULT           (_SDIO_CFGPRESETVAL0_INITDRVST_DEFAULT << 11)    /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define _SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_SHIFT         16                                               /**< Shift value for SDIO_DSPSDCLKFREQ */
#define _SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_MASK          0x3FF0000UL                                      /**< Bit mask for SDIO_DSPSDCLKFREQ */
#define _SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_DEFAULT        (_SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_DSPCLKGENEN                 (0x1UL << 26)                                    /**< Default Speed Clock Gen Enable */
#define _SDIO_CFGPRESETVAL0_DSPCLKGENEN_SHIFT          26                                               /**< Shift value for SDIO_DSPCLKGENEN */
#define _SDIO_CFGPRESETVAL0_DSPCLKGENEN_MASK           0x4000000UL                                      /**< Bit mask for SDIO_DSPCLKGENEN */
#define _SDIO_CFGPRESETVAL0_DSPCLKGENEN_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_DSPCLKGENEN_DEFAULT         (_SDIO_CFGPRESETVAL0_DSPCLKGENEN_DEFAULT << 26)  /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define _SDIO_CFGPRESETVAL0_DSPDRVST_SHIFT             27                                               /**< Shift value for SDIO_DSPDRVST */
#define _SDIO_CFGPRESETVAL0_DSPDRVST_MASK              0x18000000UL                                     /**< Bit mask for SDIO_DSPDRVST */
#define _SDIO_CFGPRESETVAL0_DSPDRVST_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SDIO_CFGPRESETVAL0 */
#define SDIO_CFGPRESETVAL0_DSPDRVST_DEFAULT            (_SDIO_CFGPRESETVAL0_DSPDRVST_DEFAULT << 27)     /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL0 */

/* Bit fields for SDIO CFGPRESETVAL1 */
#define _SDIO_CFGPRESETVAL1_RESETVALUE                 0x00000000UL                                       /**< Default value for SDIO_CFGPRESETVAL1 */
#define _SDIO_CFGPRESETVAL1_MASK                       0x1FFF1FFFUL                                       /**< Mask for SDIO_CFGPRESETVAL1 */
#define _SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_SHIFT         0                                                  /**< Shift value for SDIO_HSPSDCLKFREQ */
#define _SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_MASK          0x3FFUL                                            /**< Bit mask for SDIO_HSPSDCLKFREQ */
#define _SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_DEFAULT       0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_DEFAULT        (_SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_DEFAULT << 0)    /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_HSPCLKGENEN                 (0x1UL << 10)                                      /**< High Speed SD_CLK Gen Enable */
#define _SDIO_CFGPRESETVAL1_HSPCLKGENEN_SHIFT          10                                                 /**< Shift value for SDIO_HSPCLKGENEN */
#define _SDIO_CFGPRESETVAL1_HSPCLKGENEN_MASK           0x400UL                                            /**< Bit mask for SDIO_HSPCLKGENEN */
#define _SDIO_CFGPRESETVAL1_HSPCLKGENEN_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_HSPCLKGENEN_DEFAULT         (_SDIO_CFGPRESETVAL1_HSPCLKGENEN_DEFAULT << 10)    /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define _SDIO_CFGPRESETVAL1_HSPDRVST_SHIFT             11                                                 /**< Shift value for SDIO_HSPDRVST */
#define _SDIO_CFGPRESETVAL1_HSPDRVST_MASK              0x1800UL                                           /**< Bit mask for SDIO_HSPDRVST */
#define _SDIO_CFGPRESETVAL1_HSPDRVST_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_HSPDRVST_DEFAULT            (_SDIO_CFGPRESETVAL1_HSPDRVST_DEFAULT << 11)       /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define _SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_SHIFT       16                                                 /**< Shift value for SDIO_SDR12SDCLKFREQ */
#define _SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_MASK        0x3FF0000UL                                        /**< Bit mask for SDIO_SDR12SDCLKFREQ */
#define _SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_DEFAULT     0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_DEFAULT      (_SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_SDR12CLKGENEN               (0x1UL << 26)                                      /**< SDR12 Speed Clock Gen Enable */
#define _SDIO_CFGPRESETVAL1_SDR12CLKGENEN_SHIFT        26                                                 /**< Shift value for SDIO_SDR12CLKGENEN */
#define _SDIO_CFGPRESETVAL1_SDR12CLKGENEN_MASK         0x4000000UL                                        /**< Bit mask for SDIO_SDR12CLKGENEN */
#define _SDIO_CFGPRESETVAL1_SDR12CLKGENEN_DEFAULT      0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_SDR12CLKGENEN_DEFAULT       (_SDIO_CFGPRESETVAL1_SDR12CLKGENEN_DEFAULT << 26)  /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define _SDIO_CFGPRESETVAL1_SDR12DRVST_SHIFT           27                                                 /**< Shift value for SDIO_SDR12DRVST */
#define _SDIO_CFGPRESETVAL1_SDR12DRVST_MASK            0x18000000UL                                       /**< Bit mask for SDIO_SDR12DRVST */
#define _SDIO_CFGPRESETVAL1_SDR12DRVST_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL1 */
#define SDIO_CFGPRESETVAL1_SDR12DRVST_DEFAULT          (_SDIO_CFGPRESETVAL1_SDR12DRVST_DEFAULT << 27)     /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL1 */

/* Bit fields for SDIO CFGPRESETVAL2 */
#define _SDIO_CFGPRESETVAL2_RESETVALUE                 0x00000000UL                                       /**< Default value for SDIO_CFGPRESETVAL2 */
#define _SDIO_CFGPRESETVAL2_MASK                       0x1FFF1FFFUL                                       /**< Mask for SDIO_CFGPRESETVAL2 */
#define _SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_SHIFT       0                                                  /**< Shift value for SDIO_SDR25SDCLKFREQ */
#define _SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_MASK        0x3FFUL                                            /**< Bit mask for SDIO_SDR25SDCLKFREQ */
#define _SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_DEFAULT     0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_DEFAULT      (_SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_DEFAULT << 0)  /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR25CLKGENEN               (0x1UL << 10)                                      /**< SDR25 SD_CLK Gen Enable */
#define _SDIO_CFGPRESETVAL2_SDR25CLKGENEN_SHIFT        10                                                 /**< Shift value for SDIO_SDR25CLKGENEN */
#define _SDIO_CFGPRESETVAL2_SDR25CLKGENEN_MASK         0x400UL                                            /**< Bit mask for SDIO_SDR25CLKGENEN */
#define _SDIO_CFGPRESETVAL2_SDR25CLKGENEN_DEFAULT      0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR25CLKGENEN_DEFAULT       (_SDIO_CFGPRESETVAL2_SDR25CLKGENEN_DEFAULT << 10)  /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define _SDIO_CFGPRESETVAL2_SDR25DRVST_SHIFT           11                                                 /**< Shift value for SDIO_SDR25DRVST */
#define _SDIO_CFGPRESETVAL2_SDR25DRVST_MASK            0x1800UL                                           /**< Bit mask for SDIO_SDR25DRVST */
#define _SDIO_CFGPRESETVAL2_SDR25DRVST_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR25DRVST_DEFAULT          (_SDIO_CFGPRESETVAL2_SDR25DRVST_DEFAULT << 11)     /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define _SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_SHIFT       16                                                 /**< Shift value for SDIO_SDR50SDCLKFREQ */
#define _SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_MASK        0x3FF0000UL                                        /**< Bit mask for SDIO_SDR50SDCLKFREQ */
#define _SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_DEFAULT     0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_DEFAULT      (_SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR50CLKGENEN               (0x1UL << 26)                                      /**< SDR50 Speed Clock Gen Enable */
#define _SDIO_CFGPRESETVAL2_SDR50CLKGENEN_SHIFT        26                                                 /**< Shift value for SDIO_SDR50CLKGENEN */
#define _SDIO_CFGPRESETVAL2_SDR50CLKGENEN_MASK         0x4000000UL                                        /**< Bit mask for SDIO_SDR50CLKGENEN */
#define _SDIO_CFGPRESETVAL2_SDR50CLKGENEN_DEFAULT      0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR50CLKGENEN_DEFAULT       (_SDIO_CFGPRESETVAL2_SDR50CLKGENEN_DEFAULT << 26)  /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define _SDIO_CFGPRESETVAL2_SDR50DRVST_SHIFT           27                                                 /**< Shift value for SDIO_SDR50DRVST */
#define _SDIO_CFGPRESETVAL2_SDR50DRVST_MASK            0x18000000UL                                       /**< Bit mask for SDIO_SDR50DRVST */
#define _SDIO_CFGPRESETVAL2_SDR50DRVST_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL2 */
#define SDIO_CFGPRESETVAL2_SDR50DRVST_DEFAULT          (_SDIO_CFGPRESETVAL2_SDR50DRVST_DEFAULT << 27)     /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL2 */

/* Bit fields for SDIO CFGPRESETVAL3 */
#define _SDIO_CFGPRESETVAL3_RESETVALUE                 0x00000000UL                                       /**< Default value for SDIO_CFGPRESETVAL3 */
#define _SDIO_CFGPRESETVAL3_MASK                       0x1FFF1FFFUL                                       /**< Mask for SDIO_CFGPRESETVAL3 */
#define _SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_SHIFT      0                                                  /**< Shift value for SDIO_SDR104SDCLKFREQ */
#define _SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_MASK       0x3FFUL                                            /**< Bit mask for SDIO_SDR104SDCLKFREQ */
#define _SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_DEFAULT    0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_DEFAULT     (_SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_SDR104CLKGENEN              (0x1UL << 10)                                      /**< SDR104 SD_CLK Gen Enable */
#define _SDIO_CFGPRESETVAL3_SDR104CLKGENEN_SHIFT       10                                                 /**< Shift value for SDIO_SDR104CLKGENEN */
#define _SDIO_CFGPRESETVAL3_SDR104CLKGENEN_MASK        0x400UL                                            /**< Bit mask for SDIO_SDR104CLKGENEN */
#define _SDIO_CFGPRESETVAL3_SDR104CLKGENEN_DEFAULT     0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_SDR104CLKGENEN_DEFAULT      (_SDIO_CFGPRESETVAL3_SDR104CLKGENEN_DEFAULT << 10) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define _SDIO_CFGPRESETVAL3_SDR104DRVST_SHIFT          11                                                 /**< Shift value for SDIO_SDR104DRVST */
#define _SDIO_CFGPRESETVAL3_SDR104DRVST_MASK           0x1800UL                                           /**< Bit mask for SDIO_SDR104DRVST */
#define _SDIO_CFGPRESETVAL3_SDR104DRVST_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_SDR104DRVST_DEFAULT         (_SDIO_CFGPRESETVAL3_SDR104DRVST_DEFAULT << 11)    /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define _SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_SHIFT       16                                                 /**< Shift value for SDIO_DDR50SDCLKFREQ */
#define _SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_MASK        0x3FF0000UL                                        /**< Bit mask for SDIO_DDR50SDCLKFREQ */
#define _SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_DEFAULT     0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_DEFAULT      (_SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_DEFAULT << 16) /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_DDR50CLKGENEN               (0x1UL << 26)                                      /**< DDR50 Speed Clock Gen Enable */
#define _SDIO_CFGPRESETVAL3_DDR50CLKGENEN_SHIFT        26                                                 /**< Shift value for SDIO_DDR50CLKGENEN */
#define _SDIO_CFGPRESETVAL3_DDR50CLKGENEN_MASK         0x4000000UL                                        /**< Bit mask for SDIO_DDR50CLKGENEN */
#define _SDIO_CFGPRESETVAL3_DDR50CLKGENEN_DEFAULT      0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_DDR50CLKGENEN_DEFAULT       (_SDIO_CFGPRESETVAL3_DDR50CLKGENEN_DEFAULT << 26)  /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define _SDIO_CFGPRESETVAL3_DDR50DRVST_SHIFT           27                                                 /**< Shift value for SDIO_DDR50DRVST */
#define _SDIO_CFGPRESETVAL3_DDR50DRVST_MASK            0x18000000UL                                       /**< Bit mask for SDIO_DDR50DRVST */
#define _SDIO_CFGPRESETVAL3_DDR50DRVST_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for SDIO_CFGPRESETVAL3 */
#define SDIO_CFGPRESETVAL3_DDR50DRVST_DEFAULT          (_SDIO_CFGPRESETVAL3_DDR50DRVST_DEFAULT << 27)     /**< Shifted mode DEFAULT for SDIO_CFGPRESETVAL3 */

/* Bit fields for SDIO ROUTELOC0 */
#define _SDIO_ROUTELOC0_RESETVALUE                     0x00000000UL                           /**< Default value for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_MASK                           0x01030301UL                           /**< Mask for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_DATLOC_SHIFT                   0                                      /**< Shift value for SDIO_DATLOC */
#define _SDIO_ROUTELOC0_DATLOC_MASK                    0x1UL                                  /**< Bit mask for SDIO_DATLOC */
#define _SDIO_ROUTELOC0_DATLOC_LOC0                    0x00000000UL                           /**< Mode LOC0 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_DATLOC_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_DATLOC_LOC1                    0x00000001UL                           /**< Mode LOC1 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_DATLOC_LOC0                     (_SDIO_ROUTELOC0_DATLOC_LOC0 << 0)     /**< Shifted mode LOC0 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_DATLOC_DEFAULT                  (_SDIO_ROUTELOC0_DATLOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_DATLOC_LOC1                     (_SDIO_ROUTELOC0_DATLOC_LOC1 << 0)     /**< Shifted mode LOC1 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CDLOC_SHIFT                    8                                      /**< Shift value for SDIO_CDLOC */
#define _SDIO_ROUTELOC0_CDLOC_MASK                     0x300UL                                /**< Bit mask for SDIO_CDLOC */
#define _SDIO_ROUTELOC0_CDLOC_LOC0                     0x00000000UL                           /**< Mode LOC0 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CDLOC_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CDLOC_LOC1                     0x00000001UL                           /**< Mode LOC1 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CDLOC_LOC2                     0x00000002UL                           /**< Mode LOC2 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CDLOC_LOC3                     0x00000003UL                           /**< Mode LOC3 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CDLOC_LOC0                      (_SDIO_ROUTELOC0_CDLOC_LOC0 << 8)      /**< Shifted mode LOC0 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CDLOC_DEFAULT                   (_SDIO_ROUTELOC0_CDLOC_DEFAULT << 8)   /**< Shifted mode DEFAULT for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CDLOC_LOC1                      (_SDIO_ROUTELOC0_CDLOC_LOC1 << 8)      /**< Shifted mode LOC1 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CDLOC_LOC2                      (_SDIO_ROUTELOC0_CDLOC_LOC2 << 8)      /**< Shifted mode LOC2 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CDLOC_LOC3                      (_SDIO_ROUTELOC0_CDLOC_LOC3 << 8)      /**< Shifted mode LOC3 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_WPLOC_SHIFT                    16                                     /**< Shift value for SDIO_WPLOC */
#define _SDIO_ROUTELOC0_WPLOC_MASK                     0x30000UL                              /**< Bit mask for SDIO_WPLOC */
#define _SDIO_ROUTELOC0_WPLOC_LOC0                     0x00000000UL                           /**< Mode LOC0 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_WPLOC_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_WPLOC_LOC1                     0x00000001UL                           /**< Mode LOC1 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_WPLOC_LOC2                     0x00000002UL                           /**< Mode LOC2 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_WPLOC_LOC3                     0x00000003UL                           /**< Mode LOC3 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_WPLOC_LOC0                      (_SDIO_ROUTELOC0_WPLOC_LOC0 << 16)     /**< Shifted mode LOC0 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_WPLOC_DEFAULT                   (_SDIO_ROUTELOC0_WPLOC_DEFAULT << 16)  /**< Shifted mode DEFAULT for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_WPLOC_LOC1                      (_SDIO_ROUTELOC0_WPLOC_LOC1 << 16)     /**< Shifted mode LOC1 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_WPLOC_LOC2                      (_SDIO_ROUTELOC0_WPLOC_LOC2 << 16)     /**< Shifted mode LOC2 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_WPLOC_LOC3                      (_SDIO_ROUTELOC0_WPLOC_LOC3 << 16)     /**< Shifted mode LOC3 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CLKLOC_SHIFT                   24                                     /**< Shift value for SDIO_CLKLOC */
#define _SDIO_ROUTELOC0_CLKLOC_MASK                    0x1000000UL                            /**< Bit mask for SDIO_CLKLOC */
#define _SDIO_ROUTELOC0_CLKLOC_LOC0                    0x00000000UL                           /**< Mode LOC0 for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CLKLOC_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SDIO_ROUTELOC0 */
#define _SDIO_ROUTELOC0_CLKLOC_LOC1                    0x00000001UL                           /**< Mode LOC1 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CLKLOC_LOC0                     (_SDIO_ROUTELOC0_CLKLOC_LOC0 << 24)    /**< Shifted mode LOC0 for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CLKLOC_DEFAULT                  (_SDIO_ROUTELOC0_CLKLOC_DEFAULT << 24) /**< Shifted mode DEFAULT for SDIO_ROUTELOC0 */
#define SDIO_ROUTELOC0_CLKLOC_LOC1                     (_SDIO_ROUTELOC0_CLKLOC_LOC1 << 24)    /**< Shifted mode LOC1 for SDIO_ROUTELOC0 */

/* Bit fields for SDIO ROUTELOC1 */
#define _SDIO_ROUTELOC1_RESETVALUE                     0x00000000UL                          /**< Default value for SDIO_ROUTELOC1 */
#define _SDIO_ROUTELOC1_MASK                           0x00000001UL                          /**< Mask for SDIO_ROUTELOC1 */
#define _SDIO_ROUTELOC1_CMDLOC_SHIFT                   0                                     /**< Shift value for SDIO_CMDLOC */
#define _SDIO_ROUTELOC1_CMDLOC_MASK                    0x1UL                                 /**< Bit mask for SDIO_CMDLOC */
#define _SDIO_ROUTELOC1_CMDLOC_LOC0                    0x00000000UL                          /**< Mode LOC0 for SDIO_ROUTELOC1 */
#define _SDIO_ROUTELOC1_CMDLOC_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for SDIO_ROUTELOC1 */
#define _SDIO_ROUTELOC1_CMDLOC_LOC1                    0x00000001UL                          /**< Mode LOC1 for SDIO_ROUTELOC1 */
#define SDIO_ROUTELOC1_CMDLOC_LOC0                     (_SDIO_ROUTELOC1_CMDLOC_LOC0 << 0)    /**< Shifted mode LOC0 for SDIO_ROUTELOC1 */
#define SDIO_ROUTELOC1_CMDLOC_DEFAULT                  (_SDIO_ROUTELOC1_CMDLOC_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_ROUTELOC1 */
#define SDIO_ROUTELOC1_CMDLOC_LOC1                     (_SDIO_ROUTELOC1_CMDLOC_LOC1 << 0)    /**< Shifted mode LOC1 for SDIO_ROUTELOC1 */

/* Bit fields for SDIO ROUTEPEN */
#define _SDIO_ROUTEPEN_RESETVALUE                      0x00000000UL                         /**< Default value for SDIO_ROUTEPEN */
#define _SDIO_ROUTEPEN_MASK                            0x000003FFUL                         /**< Mask for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_CLKPEN                           (0x1UL << 0)                         /**< CLK I/O Enable */
#define _SDIO_ROUTEPEN_CLKPEN_SHIFT                    0                                    /**< Shift value for SDIO_CLKPEN */
#define _SDIO_ROUTEPEN_CLKPEN_MASK                     0x1UL                                /**< Bit mask for SDIO_CLKPEN */
#define _SDIO_ROUTEPEN_CLKPEN_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_CLKPEN_DEFAULT                   (_SDIO_ROUTEPEN_CLKPEN_DEFAULT << 0) /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_CMDPEN                           (0x1UL << 1)                         /**< CMD I/O Enable */
#define _SDIO_ROUTEPEN_CMDPEN_SHIFT                    1                                    /**< Shift value for SDIO_CMDPEN */
#define _SDIO_ROUTEPEN_CMDPEN_MASK                     0x2UL                                /**< Bit mask for SDIO_CMDPEN */
#define _SDIO_ROUTEPEN_CMDPEN_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_CMDPEN_DEFAULT                   (_SDIO_ROUTEPEN_CMDPEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D0PEN                            (0x1UL << 2)                         /**< Dat0 I/O Enable */
#define _SDIO_ROUTEPEN_D0PEN_SHIFT                     2                                    /**< Shift value for SDIO_D0PEN */
#define _SDIO_ROUTEPEN_D0PEN_MASK                      0x4UL                                /**< Bit mask for SDIO_D0PEN */
#define _SDIO_ROUTEPEN_D0PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D0PEN_DEFAULT                    (_SDIO_ROUTEPEN_D0PEN_DEFAULT << 2)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D1PEN                            (0x1UL << 3)                         /**< Dat1 I/O Enable */
#define _SDIO_ROUTEPEN_D1PEN_SHIFT                     3                                    /**< Shift value for SDIO_D1PEN */
#define _SDIO_ROUTEPEN_D1PEN_MASK                      0x8UL                                /**< Bit mask for SDIO_D1PEN */
#define _SDIO_ROUTEPEN_D1PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D1PEN_DEFAULT                    (_SDIO_ROUTEPEN_D1PEN_DEFAULT << 3)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D2PEN                            (0x1UL << 4)                         /**< Dat2 I/O Enable */
#define _SDIO_ROUTEPEN_D2PEN_SHIFT                     4                                    /**< Shift value for SDIO_D2PEN */
#define _SDIO_ROUTEPEN_D2PEN_MASK                      0x10UL                               /**< Bit mask for SDIO_D2PEN */
#define _SDIO_ROUTEPEN_D2PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D2PEN_DEFAULT                    (_SDIO_ROUTEPEN_D2PEN_DEFAULT << 4)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D3PEN                            (0x1UL << 5)                         /**< Dat3 I/O Enable */
#define _SDIO_ROUTEPEN_D3PEN_SHIFT                     5                                    /**< Shift value for SDIO_D3PEN */
#define _SDIO_ROUTEPEN_D3PEN_MASK                      0x20UL                               /**< Bit mask for SDIO_D3PEN */
#define _SDIO_ROUTEPEN_D3PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D3PEN_DEFAULT                    (_SDIO_ROUTEPEN_D3PEN_DEFAULT << 5)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D4PEN                            (0x1UL << 6)                         /**< Dat4 I/O Enable */
#define _SDIO_ROUTEPEN_D4PEN_SHIFT                     6                                    /**< Shift value for SDIO_D4PEN */
#define _SDIO_ROUTEPEN_D4PEN_MASK                      0x40UL                               /**< Bit mask for SDIO_D4PEN */
#define _SDIO_ROUTEPEN_D4PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D4PEN_DEFAULT                    (_SDIO_ROUTEPEN_D4PEN_DEFAULT << 6)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D5PEN                            (0x1UL << 7)                         /**< Dat5 Enable */
#define _SDIO_ROUTEPEN_D5PEN_SHIFT                     7                                    /**< Shift value for SDIO_D5PEN */
#define _SDIO_ROUTEPEN_D5PEN_MASK                      0x80UL                               /**< Bit mask for SDIO_D5PEN */
#define _SDIO_ROUTEPEN_D5PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D5PEN_DEFAULT                    (_SDIO_ROUTEPEN_D5PEN_DEFAULT << 7)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D6PEN                            (0x1UL << 8)                         /**< Dat6 Enable */
#define _SDIO_ROUTEPEN_D6PEN_SHIFT                     8                                    /**< Shift value for SDIO_D6PEN */
#define _SDIO_ROUTEPEN_D6PEN_MASK                      0x100UL                              /**< Bit mask for SDIO_D6PEN */
#define _SDIO_ROUTEPEN_D6PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D6PEN_DEFAULT                    (_SDIO_ROUTEPEN_D6PEN_DEFAULT << 8)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D7PEN                            (0x1UL << 9)                         /**< Data7 I/O Enable */
#define _SDIO_ROUTEPEN_D7PEN_SHIFT                     9                                    /**< Shift value for SDIO_D7PEN */
#define _SDIO_ROUTEPEN_D7PEN_MASK                      0x200UL                              /**< Bit mask for SDIO_D7PEN */
#define _SDIO_ROUTEPEN_D7PEN_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for SDIO_ROUTEPEN */
#define SDIO_ROUTEPEN_D7PEN_DEFAULT                    (_SDIO_ROUTEPEN_D7PEN_DEFAULT << 9)  /**< Shifted mode DEFAULT for SDIO_ROUTEPEN */

/** @} */
/** @} End of group EFM32GG11B_SDIO */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

