/**
 * \file
 *
 * \brief Component description for RSTC
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21_RSTC_COMPONENT_
#define _SAML21_RSTC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR RSTC */
/* ========================================================================== */
/** \addtogroup SAML21_RSTC Reset Controller */
/*@{*/

#define RSTC_U2239
#define REV_RSTC                    0x110

/* -------- RSTC_RCAUSE : (RSTC Offset: 0x00) (R/   8) Reset Cause -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  POR:1;            /*!< bit:      0  Power On Reset                     */
    uint8_t  BOD12:1;          /*!< bit:      1  Brown Out 12 Detector Reset        */
    uint8_t  BOD33:1;          /*!< bit:      2  Brown Out 33 Detector Reset        */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  EXT:1;            /*!< bit:      4  External Reset                     */
    uint8_t  WDT:1;            /*!< bit:      5  Watchdog Reset                     */
    uint8_t  SYST:1;           /*!< bit:      6  System Reset Request               */
    uint8_t  BACKUP:1;         /*!< bit:      7  Backup Reset                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RSTC_RCAUSE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_RCAUSE_OFFSET          0x00         /**< \brief (RSTC_RCAUSE offset) Reset Cause */

#define RSTC_RCAUSE_POR_Pos         0            /**< \brief (RSTC_RCAUSE) Power On Reset */
#define RSTC_RCAUSE_POR             (0x1ul << RSTC_RCAUSE_POR_Pos)
#define RSTC_RCAUSE_BOD12_Pos       1            /**< \brief (RSTC_RCAUSE) Brown Out 12 Detector Reset */
#define RSTC_RCAUSE_BOD12           (0x1ul << RSTC_RCAUSE_BOD12_Pos)
#define RSTC_RCAUSE_BOD33_Pos       2            /**< \brief (RSTC_RCAUSE) Brown Out 33 Detector Reset */
#define RSTC_RCAUSE_BOD33           (0x1ul << RSTC_RCAUSE_BOD33_Pos)
#define RSTC_RCAUSE_EXT_Pos         4            /**< \brief (RSTC_RCAUSE) External Reset */
#define RSTC_RCAUSE_EXT             (0x1ul << RSTC_RCAUSE_EXT_Pos)
#define RSTC_RCAUSE_WDT_Pos         5            /**< \brief (RSTC_RCAUSE) Watchdog Reset */
#define RSTC_RCAUSE_WDT             (0x1ul << RSTC_RCAUSE_WDT_Pos)
#define RSTC_RCAUSE_SYST_Pos        6            /**< \brief (RSTC_RCAUSE) System Reset Request */
#define RSTC_RCAUSE_SYST            (0x1ul << RSTC_RCAUSE_SYST_Pos)
#define RSTC_RCAUSE_BACKUP_Pos      7            /**< \brief (RSTC_RCAUSE) Backup Reset */
#define RSTC_RCAUSE_BACKUP          (0x1ul << RSTC_RCAUSE_BACKUP_Pos)
#define RSTC_RCAUSE_MASK            0xF7ul       /**< \brief (RSTC_RCAUSE) MASK Register */

/* -------- RSTC_BKUPEXIT : (RSTC Offset: 0x02) (R/   8) Backup Exit Source -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EXTWAKE:1;        /*!< bit:      0  External Wakeup                    */
    uint8_t  RTC:1;            /*!< bit:      1  Real Timer Counter Interrupt       */
    uint8_t  BBPS:1;           /*!< bit:      2  Battery Backup Power Switch        */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RSTC_BKUPEXIT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_BKUPEXIT_OFFSET        0x02         /**< \brief (RSTC_BKUPEXIT offset) Backup Exit Source */

#define RSTC_BKUPEXIT_EXTWAKE_Pos   0            /**< \brief (RSTC_BKUPEXIT) External Wakeup */
#define RSTC_BKUPEXIT_EXTWAKE       (0x1ul << RSTC_BKUPEXIT_EXTWAKE_Pos)
#define RSTC_BKUPEXIT_RTC_Pos       1            /**< \brief (RSTC_BKUPEXIT) Real Timer Counter Interrupt */
#define RSTC_BKUPEXIT_RTC           (0x1ul << RSTC_BKUPEXIT_RTC_Pos)
#define RSTC_BKUPEXIT_BBPS_Pos      2            /**< \brief (RSTC_BKUPEXIT) Battery Backup Power Switch */
#define RSTC_BKUPEXIT_BBPS          (0x1ul << RSTC_BKUPEXIT_BBPS_Pos)
#define RSTC_BKUPEXIT_MASK          0x07ul       /**< \brief (RSTC_BKUPEXIT) MASK Register */

/* -------- RSTC_WKDBCONF : (RSTC Offset: 0x04) (R/W  8) Wakeup Debounce Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WKDBCNT:5;        /*!< bit:  0.. 4  Wakeup Debounce Counter            */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RSTC_WKDBCONF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_WKDBCONF_OFFSET        0x04         /**< \brief (RSTC_WKDBCONF offset) Wakeup Debounce Configuration */
#define RSTC_WKDBCONF_RESETVALUE    0x00ul       /**< \brief (RSTC_WKDBCONF reset_value) Wakeup Debounce Configuration */

#define RSTC_WKDBCONF_WKDBCNT_Pos   0            /**< \brief (RSTC_WKDBCONF) Wakeup Debounce Counter */
#define RSTC_WKDBCONF_WKDBCNT_Msk   (0x1Ful << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT(value) (RSTC_WKDBCONF_WKDBCNT_Msk & ((value) << RSTC_WKDBCONF_WKDBCNT_Pos))
#define   RSTC_WKDBCONF_WKDBCNT_OFF_Val   0x0ul  /**< \brief (RSTC_WKDBCONF) No debouncing.Input pin is low or high level sensitive depending on its WKPOLx bit. */
#define   RSTC_WKDBCONF_WKDBCNT_2CK32_Val 0x1ul  /**< \brief (RSTC_WKDBCONF) Input pin shall be active for at least two 32kHz clock period. */
#define   RSTC_WKDBCONF_WKDBCNT_3CK32_Val 0x2ul  /**< \brief (RSTC_WKDBCONF) Input pin shall be active for at least three 32kHz clock period. */
#define   RSTC_WKDBCONF_WKDBCNT_32CK32_Val 0x3ul  /**< \brief (RSTC_WKDBCONF) Input pin shall be active for at least 32 32kHz clock period. */
#define   RSTC_WKDBCONF_WKDBCNT_512CK32_Val 0x4ul  /**< \brief (RSTC_WKDBCONF) Input pin shall be active for at least 512 32kHz clock period. */
#define   RSTC_WKDBCONF_WKDBCNT_4096CK32_Val 0x5ul  /**< \brief (RSTC_WKDBCONF) Input pin shall be active for at least 4096 32kHz clock period. */
#define   RSTC_WKDBCONF_WKDBCNT_32768CK32_Val 0x6ul  /**< \brief (RSTC_WKDBCONF) Input pin shall be active for at least 32768 32kHz clock period. */
#define RSTC_WKDBCONF_WKDBCNT_OFF   (RSTC_WKDBCONF_WKDBCNT_OFF_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT_2CK32 (RSTC_WKDBCONF_WKDBCNT_2CK32_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT_3CK32 (RSTC_WKDBCONF_WKDBCNT_3CK32_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT_32CK32 (RSTC_WKDBCONF_WKDBCNT_32CK32_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT_512CK32 (RSTC_WKDBCONF_WKDBCNT_512CK32_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT_4096CK32 (RSTC_WKDBCONF_WKDBCNT_4096CK32_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_WKDBCNT_32768CK32 (RSTC_WKDBCONF_WKDBCNT_32768CK32_Val << RSTC_WKDBCONF_WKDBCNT_Pos)
#define RSTC_WKDBCONF_MASK          0x1Ful       /**< \brief (RSTC_WKDBCONF) MASK Register */

/* -------- RSTC_WKPOL : (RSTC Offset: 0x08) (R/W 16) Wakeup Polarity -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WKPOL:8;          /*!< bit:  0.. 7  Wakeup Polarity                    */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RSTC_WKPOL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_WKPOL_OFFSET           0x08         /**< \brief (RSTC_WKPOL offset) Wakeup Polarity */
#define RSTC_WKPOL_RESETVALUE       0x0000ul     /**< \brief (RSTC_WKPOL reset_value) Wakeup Polarity */

#define RSTC_WKPOL_WKPOL_Pos        0            /**< \brief (RSTC_WKPOL) Wakeup Polarity */
#define RSTC_WKPOL_WKPOL_Msk        (0xFFul << RSTC_WKPOL_WKPOL_Pos)
#define RSTC_WKPOL_WKPOL(value)     (RSTC_WKPOL_WKPOL_Msk & ((value) << RSTC_WKPOL_WKPOL_Pos))
#define RSTC_WKPOL_MASK             0x00FFul     /**< \brief (RSTC_WKPOL) MASK Register */

/* -------- RSTC_WKEN : (RSTC Offset: 0x0C) (R/W 16) Wakeup Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WKEN:8;           /*!< bit:  0.. 7  Wakeup Enable                      */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RSTC_WKEN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_WKEN_OFFSET            0x0C         /**< \brief (RSTC_WKEN offset) Wakeup Enable */
#define RSTC_WKEN_RESETVALUE        0x0000ul     /**< \brief (RSTC_WKEN reset_value) Wakeup Enable */

#define RSTC_WKEN_WKEN_Pos          0            /**< \brief (RSTC_WKEN) Wakeup Enable */
#define RSTC_WKEN_WKEN_Msk          (0xFFul << RSTC_WKEN_WKEN_Pos)
#define RSTC_WKEN_WKEN(value)       (RSTC_WKEN_WKEN_Msk & ((value) << RSTC_WKEN_WKEN_Pos))
#define RSTC_WKEN_MASK              0x00FFul     /**< \brief (RSTC_WKEN) MASK Register */

/* -------- RSTC_WKCAUSE : (RSTC Offset: 0x10) (R/W 16) Wakeup Cause -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WKCAUSE:16;       /*!< bit:  0..15  Wakeup Cause                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RSTC_WKCAUSE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_WKCAUSE_OFFSET         0x10         /**< \brief (RSTC_WKCAUSE offset) Wakeup Cause */
#define RSTC_WKCAUSE_RESETVALUE     0x0000ul     /**< \brief (RSTC_WKCAUSE reset_value) Wakeup Cause */

#define RSTC_WKCAUSE_WKCAUSE_Pos    0            /**< \brief (RSTC_WKCAUSE) Wakeup Cause */
#define RSTC_WKCAUSE_WKCAUSE_Msk    (0xFFFFul << RSTC_WKCAUSE_WKCAUSE_Pos)
#define RSTC_WKCAUSE_WKCAUSE(value) (RSTC_WKCAUSE_WKCAUSE_Msk & ((value) << RSTC_WKCAUSE_WKCAUSE_Pos))
#define RSTC_WKCAUSE_MASK           0xFFFFul     /**< \brief (RSTC_WKCAUSE) MASK Register */

/** \brief RSTC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __I  RSTC_RCAUSE_Type          RCAUSE;      /**< \brief Offset: 0x00 (R/   8) Reset Cause */
       RoReg8                    Reserved1[0x1];
  __I  RSTC_BKUPEXIT_Type        BKUPEXIT;    /**< \brief Offset: 0x02 (R/   8) Backup Exit Source */
       RoReg8                    Reserved2[0x1];
  __IO RSTC_WKDBCONF_Type        WKDBCONF;    /**< \brief Offset: 0x04 (R/W  8) Wakeup Debounce Configuration */
       RoReg8                    Reserved3[0x3];
  __IO RSTC_WKPOL_Type           WKPOL;       /**< \brief Offset: 0x08 (R/W 16) Wakeup Polarity */
       RoReg8                    Reserved4[0x2];
  __IO RSTC_WKEN_Type            WKEN;        /**< \brief Offset: 0x0C (R/W 16) Wakeup Enable */
       RoReg8                    Reserved5[0x2];
  __IO RSTC_WKCAUSE_Type         WKCAUSE;     /**< \brief Offset: 0x10 (R/W 16) Wakeup Cause */
} Rstc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAML21_RSTC_COMPONENT_ */
