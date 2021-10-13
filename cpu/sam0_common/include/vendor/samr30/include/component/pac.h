/**
 * \file
 *
 * \brief Component description for PAC
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _SAMR30_PAC_COMPONENT_
#define _SAMR30_PAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PAC */
/* ========================================================================== */
/** \addtogroup SAMR30_PAC Peripheral Access Controller */
/*@{*/

#define PAC_U2120
#define REV_PAC                     0x110

/* -------- PAC_WRCTRL : (PAC Offset: 0x00) (R/W 32) Write control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PERID:16;         /*!< bit:  0..15  Peripheral identifier              */
    uint32_t KEY:8;            /*!< bit: 16..23  Peripheral access control key      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_WRCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_WRCTRL_OFFSET           0x00         /**< \brief (PAC_WRCTRL offset) Write control */
#define PAC_WRCTRL_RESETVALUE       0x00000000ul /**< \brief (PAC_WRCTRL reset_value) Write control */

#define PAC_WRCTRL_PERID_Pos        0            /**< \brief (PAC_WRCTRL) Peripheral identifier */
#define PAC_WRCTRL_PERID_Msk        (0xFFFFul << PAC_WRCTRL_PERID_Pos)
#define PAC_WRCTRL_PERID(value)     (PAC_WRCTRL_PERID_Msk & ((value) << PAC_WRCTRL_PERID_Pos))
#define PAC_WRCTRL_KEY_Pos          16           /**< \brief (PAC_WRCTRL) Peripheral access control key */
#define PAC_WRCTRL_KEY_Msk          (0xFFul << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY(value)       (PAC_WRCTRL_KEY_Msk & ((value) << PAC_WRCTRL_KEY_Pos))
#define   PAC_WRCTRL_KEY_OFF_Val          0x0ul  /**< \brief (PAC_WRCTRL) No action */
#define   PAC_WRCTRL_KEY_CLR_Val          0x1ul  /**< \brief (PAC_WRCTRL) Clear protection */
#define   PAC_WRCTRL_KEY_SET_Val          0x2ul  /**< \brief (PAC_WRCTRL) Set protection */
#define   PAC_WRCTRL_KEY_SETLCK_Val       0x3ul  /**< \brief (PAC_WRCTRL) Set and lock protection */
#define PAC_WRCTRL_KEY_OFF          (PAC_WRCTRL_KEY_OFF_Val        << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY_CLR          (PAC_WRCTRL_KEY_CLR_Val        << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY_SET          (PAC_WRCTRL_KEY_SET_Val        << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY_SETLCK       (PAC_WRCTRL_KEY_SETLCK_Val     << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_MASK             0x00FFFFFFul /**< \brief (PAC_WRCTRL) MASK Register */

/* -------- PAC_EVCTRL : (PAC Offset: 0x04) (R/W  8) Event control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERREO:1;          /*!< bit:      0  Peripheral acess error event output */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PAC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_EVCTRL_OFFSET           0x04         /**< \brief (PAC_EVCTRL offset) Event control */
#define PAC_EVCTRL_RESETVALUE       0x00ul       /**< \brief (PAC_EVCTRL reset_value) Event control */

#define PAC_EVCTRL_ERREO_Pos        0            /**< \brief (PAC_EVCTRL) Peripheral acess error event output */
#define PAC_EVCTRL_ERREO            (0x1ul << PAC_EVCTRL_ERREO_Pos)
#define PAC_EVCTRL_MASK             0x01ul       /**< \brief (PAC_EVCTRL) MASK Register */

/* -------- PAC_INTENCLR : (PAC Offset: 0x08) (R/W  8) Interrupt enable clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERR:1;            /*!< bit:      0  Peripheral access error interrupt disable */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PAC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTENCLR_OFFSET         0x08         /**< \brief (PAC_INTENCLR offset) Interrupt enable clear */
#define PAC_INTENCLR_RESETVALUE     0x00ul       /**< \brief (PAC_INTENCLR reset_value) Interrupt enable clear */

#define PAC_INTENCLR_ERR_Pos        0            /**< \brief (PAC_INTENCLR) Peripheral access error interrupt disable */
#define PAC_INTENCLR_ERR            (0x1ul << PAC_INTENCLR_ERR_Pos)
#define PAC_INTENCLR_MASK           0x01ul       /**< \brief (PAC_INTENCLR) MASK Register */

/* -------- PAC_INTENSET : (PAC Offset: 0x09) (R/W  8) Interrupt enable set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERR:1;            /*!< bit:      0  Peripheral access error interrupt enable */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PAC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTENSET_OFFSET         0x09         /**< \brief (PAC_INTENSET offset) Interrupt enable set */
#define PAC_INTENSET_RESETVALUE     0x00ul       /**< \brief (PAC_INTENSET reset_value) Interrupt enable set */

#define PAC_INTENSET_ERR_Pos        0            /**< \brief (PAC_INTENSET) Peripheral access error interrupt enable */
#define PAC_INTENSET_ERR            (0x1ul << PAC_INTENSET_ERR_Pos)
#define PAC_INTENSET_MASK           0x01ul       /**< \brief (PAC_INTENSET) MASK Register */

/* -------- PAC_INTFLAGAHB : (PAC Offset: 0x10) (R/W 32) Bridge interrupt flag status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t FLASH_:1;         /*!< bit:      0  FLASH                              */
    __I uint32_t HSRAMCM0P_:1;     /*!< bit:      1  HSRAMCM0P                          */
    __I uint32_t HSRAMDSU_:1;      /*!< bit:      2  HSRAMDSU                           */
    __I uint32_t HPB1_:1;          /*!< bit:      3  HPB1                               */
    __I uint32_t H2LBRIDGES_:1;    /*!< bit:      4  H2LBRIDGES                         */
    __I uint32_t :11;              /*!< bit:  5..15  Reserved                           */
    __I uint32_t HPB0_:1;          /*!< bit:     16  HPB0                               */
    __I uint32_t HPB2_:1;          /*!< bit:     17  HPB2                               */
    __I uint32_t HPB3_:1;          /*!< bit:     18  HPB3                               */
    __I uint32_t HPB4_:1;          /*!< bit:     19  HPB4                               */
    __I uint32_t :1;               /*!< bit:     20  Reserved                           */
    __I uint32_t LPRAMHS_:1;       /*!< bit:     21  LPRAMHS                            */
    __I uint32_t LPRAMPICOP_:1;    /*!< bit:     22  LPRAMPICOP                         */
    __I uint32_t LPRAMDMAC_:1;     /*!< bit:     23  LPRAMDMAC                          */
    __I uint32_t L2HBRIDGES_:1;    /*!< bit:     24  L2HBRIDGES                         */
    __I uint32_t HSRAMLP_:1;       /*!< bit:     25  HSRAMLP                            */
    __I uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGAHB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGAHB_OFFSET       0x10         /**< \brief (PAC_INTFLAGAHB offset) Bridge interrupt flag status */
#define PAC_INTFLAGAHB_RESETVALUE   0x00000000ul /**< \brief (PAC_INTFLAGAHB reset_value) Bridge interrupt flag status */

#define PAC_INTFLAGAHB_FLASH_Pos    0            /**< \brief (PAC_INTFLAGAHB) FLASH */
#define PAC_INTFLAGAHB_FLASH        (0x1ul << PAC_INTFLAGAHB_FLASH_Pos)
#define PAC_INTFLAGAHB_HSRAMCM0P_Pos 1            /**< \brief (PAC_INTFLAGAHB) HSRAMCM0P */
#define PAC_INTFLAGAHB_HSRAMCM0P    (0x1ul << PAC_INTFLAGAHB_HSRAMCM0P_Pos)
#define PAC_INTFLAGAHB_HSRAMDSU_Pos 2            /**< \brief (PAC_INTFLAGAHB) HSRAMDSU */
#define PAC_INTFLAGAHB_HSRAMDSU     (0x1ul << PAC_INTFLAGAHB_HSRAMDSU_Pos)
#define PAC_INTFLAGAHB_HPB1_Pos     3            /**< \brief (PAC_INTFLAGAHB) HPB1 */
#define PAC_INTFLAGAHB_HPB1         (0x1ul << PAC_INTFLAGAHB_HPB1_Pos)
#define PAC_INTFLAGAHB_H2LBRIDGES_Pos 4            /**< \brief (PAC_INTFLAGAHB) H2LBRIDGES */
#define PAC_INTFLAGAHB_H2LBRIDGES   (0x1ul << PAC_INTFLAGAHB_H2LBRIDGES_Pos)
#define PAC_INTFLAGAHB_HPB0_Pos     16           /**< \brief (PAC_INTFLAGAHB) HPB0 */
#define PAC_INTFLAGAHB_HPB0         (0x1ul << PAC_INTFLAGAHB_HPB0_Pos)
#define PAC_INTFLAGAHB_HPB2_Pos     17           /**< \brief (PAC_INTFLAGAHB) HPB2 */
#define PAC_INTFLAGAHB_HPB2         (0x1ul << PAC_INTFLAGAHB_HPB2_Pos)
#define PAC_INTFLAGAHB_HPB3_Pos     18           /**< \brief (PAC_INTFLAGAHB) HPB3 */
#define PAC_INTFLAGAHB_HPB3         (0x1ul << PAC_INTFLAGAHB_HPB3_Pos)
#define PAC_INTFLAGAHB_HPB4_Pos     19           /**< \brief (PAC_INTFLAGAHB) HPB4 */
#define PAC_INTFLAGAHB_HPB4         (0x1ul << PAC_INTFLAGAHB_HPB4_Pos)
#define PAC_INTFLAGAHB_LPRAMHS_Pos  21           /**< \brief (PAC_INTFLAGAHB) LPRAMHS */
#define PAC_INTFLAGAHB_LPRAMHS      (0x1ul << PAC_INTFLAGAHB_LPRAMHS_Pos)
#define PAC_INTFLAGAHB_LPRAMPICOP_Pos 22           /**< \brief (PAC_INTFLAGAHB) LPRAMPICOP */
#define PAC_INTFLAGAHB_LPRAMPICOP   (0x1ul << PAC_INTFLAGAHB_LPRAMPICOP_Pos)
#define PAC_INTFLAGAHB_LPRAMDMAC_Pos 23           /**< \brief (PAC_INTFLAGAHB) LPRAMDMAC */
#define PAC_INTFLAGAHB_LPRAMDMAC    (0x1ul << PAC_INTFLAGAHB_LPRAMDMAC_Pos)
#define PAC_INTFLAGAHB_L2HBRIDGES_Pos 24           /**< \brief (PAC_INTFLAGAHB) L2HBRIDGES */
#define PAC_INTFLAGAHB_L2HBRIDGES   (0x1ul << PAC_INTFLAGAHB_L2HBRIDGES_Pos)
#define PAC_INTFLAGAHB_HSRAMLP_Pos  25           /**< \brief (PAC_INTFLAGAHB) HSRAMLP */
#define PAC_INTFLAGAHB_HSRAMLP      (0x1ul << PAC_INTFLAGAHB_HSRAMLP_Pos)
#define PAC_INTFLAGAHB_MASK         0x03EF001Ful /**< \brief (PAC_INTFLAGAHB) MASK Register */

/* -------- PAC_INTFLAGA : (PAC Offset: 0x14) (R/W 32) Peripheral interrupt flag status - Bridge A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t PM_:1;            /*!< bit:      0  PM                                 */
    __I uint32_t MCLK_:1;          /*!< bit:      1  MCLK                               */
    __I uint32_t RSTC_:1;          /*!< bit:      2  RSTC                               */
    __I uint32_t OSCCTRL_:1;       /*!< bit:      3  OSCCTRL                            */
    __I uint32_t OSC32KCTRL_:1;    /*!< bit:      4  OSC32KCTRL                         */
    __I uint32_t SUPC_:1;          /*!< bit:      5  SUPC                               */
    __I uint32_t GCLK_:1;          /*!< bit:      6  GCLK                               */
    __I uint32_t WDT_:1;           /*!< bit:      7  WDT                                */
    __I uint32_t RTC_:1;           /*!< bit:      8  RTC                                */
    __I uint32_t EIC_:1;           /*!< bit:      9  EIC                                */
    __I uint32_t PORT_:1;          /*!< bit:     10  PORT                               */
    __I uint32_t TAL_:1;           /*!< bit:     11  TAL                                */
    __I uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGA_OFFSET         0x14         /**< \brief (PAC_INTFLAGA offset) Peripheral interrupt flag status - Bridge A */
#define PAC_INTFLAGA_RESETVALUE     0x00000000ul /**< \brief (PAC_INTFLAGA reset_value) Peripheral interrupt flag status - Bridge A */

#define PAC_INTFLAGA_PM_Pos         0            /**< \brief (PAC_INTFLAGA) PM */
#define PAC_INTFLAGA_PM             (0x1ul << PAC_INTFLAGA_PM_Pos)
#define PAC_INTFLAGA_MCLK_Pos       1            /**< \brief (PAC_INTFLAGA) MCLK */
#define PAC_INTFLAGA_MCLK           (0x1ul << PAC_INTFLAGA_MCLK_Pos)
#define PAC_INTFLAGA_RSTC_Pos       2            /**< \brief (PAC_INTFLAGA) RSTC */
#define PAC_INTFLAGA_RSTC           (0x1ul << PAC_INTFLAGA_RSTC_Pos)
#define PAC_INTFLAGA_OSCCTRL_Pos    3            /**< \brief (PAC_INTFLAGA) OSCCTRL */
#define PAC_INTFLAGA_OSCCTRL        (0x1ul << PAC_INTFLAGA_OSCCTRL_Pos)
#define PAC_INTFLAGA_OSC32KCTRL_Pos 4            /**< \brief (PAC_INTFLAGA) OSC32KCTRL */
#define PAC_INTFLAGA_OSC32KCTRL     (0x1ul << PAC_INTFLAGA_OSC32KCTRL_Pos)
#define PAC_INTFLAGA_SUPC_Pos       5            /**< \brief (PAC_INTFLAGA) SUPC */
#define PAC_INTFLAGA_SUPC           (0x1ul << PAC_INTFLAGA_SUPC_Pos)
#define PAC_INTFLAGA_GCLK_Pos       6            /**< \brief (PAC_INTFLAGA) GCLK */
#define PAC_INTFLAGA_GCLK           (0x1ul << PAC_INTFLAGA_GCLK_Pos)
#define PAC_INTFLAGA_WDT_Pos        7            /**< \brief (PAC_INTFLAGA) WDT */
#define PAC_INTFLAGA_WDT            (0x1ul << PAC_INTFLAGA_WDT_Pos)
#define PAC_INTFLAGA_RTC_Pos        8            /**< \brief (PAC_INTFLAGA) RTC */
#define PAC_INTFLAGA_RTC            (0x1ul << PAC_INTFLAGA_RTC_Pos)
#define PAC_INTFLAGA_EIC_Pos        9            /**< \brief (PAC_INTFLAGA) EIC */
#define PAC_INTFLAGA_EIC            (0x1ul << PAC_INTFLAGA_EIC_Pos)
#define PAC_INTFLAGA_PORT_Pos       10           /**< \brief (PAC_INTFLAGA) PORT */
#define PAC_INTFLAGA_PORT           (0x1ul << PAC_INTFLAGA_PORT_Pos)
#define PAC_INTFLAGA_TAL_Pos        11           /**< \brief (PAC_INTFLAGA) TAL */
#define PAC_INTFLAGA_TAL            (0x1ul << PAC_INTFLAGA_TAL_Pos)
#define PAC_INTFLAGA_MASK           0x00000FFFul /**< \brief (PAC_INTFLAGA) MASK Register */

/* -------- PAC_INTFLAGB : (PAC Offset: 0x18) (R/W 32) Peripheral interrupt flag status - Bridge B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t USB_:1;           /*!< bit:      0  USB                                */
    __I uint32_t DSU_:1;           /*!< bit:      1  DSU                                */
    __I uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL                            */
    __I uint32_t MTB_:1;           /*!< bit:      3  MTB                                */
    __I uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGB_OFFSET         0x18         /**< \brief (PAC_INTFLAGB offset) Peripheral interrupt flag status - Bridge B */
#define PAC_INTFLAGB_RESETVALUE     0x00000000ul /**< \brief (PAC_INTFLAGB reset_value) Peripheral interrupt flag status - Bridge B */

#define PAC_INTFLAGB_USB_Pos        0            /**< \brief (PAC_INTFLAGB) USB */
#define PAC_INTFLAGB_USB            (0x1ul << PAC_INTFLAGB_USB_Pos)
#define PAC_INTFLAGB_DSU_Pos        1            /**< \brief (PAC_INTFLAGB) DSU */
#define PAC_INTFLAGB_DSU            (0x1ul << PAC_INTFLAGB_DSU_Pos)
#define PAC_INTFLAGB_NVMCTRL_Pos    2            /**< \brief (PAC_INTFLAGB) NVMCTRL */
#define PAC_INTFLAGB_NVMCTRL        (0x1ul << PAC_INTFLAGB_NVMCTRL_Pos)
#define PAC_INTFLAGB_MTB_Pos        3            /**< \brief (PAC_INTFLAGB) MTB */
#define PAC_INTFLAGB_MTB            (0x1ul << PAC_INTFLAGB_MTB_Pos)
#define PAC_INTFLAGB_MASK           0x0000000Ful /**< \brief (PAC_INTFLAGB) MASK Register */

/* -------- PAC_INTFLAGC : (PAC Offset: 0x1C) (R/W 32) Peripheral interrupt flag status - Bridge C -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t SERCOM0_:1;       /*!< bit:      0  SERCOM0                            */
    __I uint32_t SERCOM1_:1;       /*!< bit:      1  SERCOM1                            */
    __I uint32_t SERCOM2_:1;       /*!< bit:      2  SERCOM2                            */
    __I uint32_t SERCOM3_:1;       /*!< bit:      3  SERCOM3                            */
    __I uint32_t SERCOM4_:1;       /*!< bit:      4  SERCOM4                            */
    __I uint32_t TCC0_:1;          /*!< bit:      5  TCC0                               */
    __I uint32_t TCC1_:1;          /*!< bit:      6  TCC1                               */
    __I uint32_t TCC2_:1;          /*!< bit:      7  TCC2                               */
    __I uint32_t TC0_:1;           /*!< bit:      8  TC0                                */
    __I uint32_t TC1_:1;           /*!< bit:      9  TC1                                */
    __I uint32_t :22;              /*!< bit: 10..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGC_OFFSET         0x1C         /**< \brief (PAC_INTFLAGC offset) Peripheral interrupt flag status - Bridge C */
#define PAC_INTFLAGC_RESETVALUE     0x00000000ul /**< \brief (PAC_INTFLAGC reset_value) Peripheral interrupt flag status - Bridge C */

#define PAC_INTFLAGC_SERCOM0_Pos    0            /**< \brief (PAC_INTFLAGC) SERCOM0 */
#define PAC_INTFLAGC_SERCOM0        (0x1ul << PAC_INTFLAGC_SERCOM0_Pos)
#define PAC_INTFLAGC_SERCOM1_Pos    1            /**< \brief (PAC_INTFLAGC) SERCOM1 */
#define PAC_INTFLAGC_SERCOM1        (0x1ul << PAC_INTFLAGC_SERCOM1_Pos)
#define PAC_INTFLAGC_SERCOM2_Pos    2            /**< \brief (PAC_INTFLAGC) SERCOM2 */
#define PAC_INTFLAGC_SERCOM2        (0x1ul << PAC_INTFLAGC_SERCOM2_Pos)
#define PAC_INTFLAGC_SERCOM3_Pos    3            /**< \brief (PAC_INTFLAGC) SERCOM3 */
#define PAC_INTFLAGC_SERCOM3        (0x1ul << PAC_INTFLAGC_SERCOM3_Pos)
#define PAC_INTFLAGC_SERCOM4_Pos    4            /**< \brief (PAC_INTFLAGC) SERCOM4 */
#define PAC_INTFLAGC_SERCOM4        (0x1ul << PAC_INTFLAGC_SERCOM4_Pos)
#define PAC_INTFLAGC_TCC0_Pos       5            /**< \brief (PAC_INTFLAGC) TCC0 */
#define PAC_INTFLAGC_TCC0           (0x1ul << PAC_INTFLAGC_TCC0_Pos)
#define PAC_INTFLAGC_TCC1_Pos       6            /**< \brief (PAC_INTFLAGC) TCC1 */
#define PAC_INTFLAGC_TCC1           (0x1ul << PAC_INTFLAGC_TCC1_Pos)
#define PAC_INTFLAGC_TCC2_Pos       7            /**< \brief (PAC_INTFLAGC) TCC2 */
#define PAC_INTFLAGC_TCC2           (0x1ul << PAC_INTFLAGC_TCC2_Pos)
#define PAC_INTFLAGC_TC0_Pos        8            /**< \brief (PAC_INTFLAGC) TC0 */
#define PAC_INTFLAGC_TC0            (0x1ul << PAC_INTFLAGC_TC0_Pos)
#define PAC_INTFLAGC_TC1_Pos        9            /**< \brief (PAC_INTFLAGC) TC1 */
#define PAC_INTFLAGC_TC1            (0x1ul << PAC_INTFLAGC_TC1_Pos)
#define PAC_INTFLAGC_MASK           0x000003FFul /**< \brief (PAC_INTFLAGC) MASK Register */

/* -------- PAC_INTFLAGD : (PAC Offset: 0x20) (R/W 32) Peripheral interrupt flag status - Bridge D -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t EVSYS_:1;         /*!< bit:      0  EVSYS                              */
    __I uint32_t SERCOM5_:1;       /*!< bit:      1  SERCOM5                            */
    __I uint32_t TC4_:1;           /*!< bit:      2  TC4                                */
    __I uint32_t ADC_:1;           /*!< bit:      3  ADC                                */
    __I uint32_t AC_:1;            /*!< bit:      4  AC                                 */
    __I uint32_t PTC_:1;           /*!< bit:      5  PTC                                */
    __I uint32_t :1;               /*!< bit:      6  Reserved                           */
    __I uint32_t CCL_:1;           /*!< bit:      7  CCL                                */
    __I uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGD_OFFSET         0x20         /**< \brief (PAC_INTFLAGD offset) Peripheral interrupt flag status - Bridge D */
#define PAC_INTFLAGD_RESETVALUE     0x00000000ul /**< \brief (PAC_INTFLAGD reset_value) Peripheral interrupt flag status - Bridge D */

#define PAC_INTFLAGD_EVSYS_Pos      0            /**< \brief (PAC_INTFLAGD) EVSYS */
#define PAC_INTFLAGD_EVSYS          (0x1ul << PAC_INTFLAGD_EVSYS_Pos)
#define PAC_INTFLAGD_SERCOM5_Pos    1            /**< \brief (PAC_INTFLAGD) SERCOM5 */
#define PAC_INTFLAGD_SERCOM5        (0x1ul << PAC_INTFLAGD_SERCOM5_Pos)
#define PAC_INTFLAGD_TC4_Pos        2            /**< \brief (PAC_INTFLAGD) TC4 */
#define PAC_INTFLAGD_TC4            (0x1ul << PAC_INTFLAGD_TC4_Pos)
#define PAC_INTFLAGD_ADC_Pos        3            /**< \brief (PAC_INTFLAGD) ADC */
#define PAC_INTFLAGD_ADC            (0x1ul << PAC_INTFLAGD_ADC_Pos)
#define PAC_INTFLAGD_AC_Pos         4            /**< \brief (PAC_INTFLAGD) AC */
#define PAC_INTFLAGD_AC             (0x1ul << PAC_INTFLAGD_AC_Pos)
#define PAC_INTFLAGD_PTC_Pos        5            /**< \brief (PAC_INTFLAGD) PTC */
#define PAC_INTFLAGD_PTC            (0x1ul << PAC_INTFLAGD_PTC_Pos)
#define PAC_INTFLAGD_CCL_Pos        7            /**< \brief (PAC_INTFLAGD) CCL */
#define PAC_INTFLAGD_CCL            (0x1ul << PAC_INTFLAGD_CCL_Pos)
#define PAC_INTFLAGD_MASK           0x000000BFul /**< \brief (PAC_INTFLAGD) MASK Register */

/* -------- PAC_INTFLAGE : (PAC Offset: 0x24) (R/W 32) Peripheral interrupt flag status - Bridge E -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t PAC_:1;           /*!< bit:      0  PAC                                */
    __I uint32_t DMAC_:1;          /*!< bit:      1  DMAC                               */
    __I uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGE_OFFSET         0x24         /**< \brief (PAC_INTFLAGE offset) Peripheral interrupt flag status - Bridge E */
#define PAC_INTFLAGE_RESETVALUE     0x00000000ul /**< \brief (PAC_INTFLAGE reset_value) Peripheral interrupt flag status - Bridge E */

#define PAC_INTFLAGE_PAC_Pos        0            /**< \brief (PAC_INTFLAGE) PAC */
#define PAC_INTFLAGE_PAC            (0x1ul << PAC_INTFLAGE_PAC_Pos)
#define PAC_INTFLAGE_DMAC_Pos       1            /**< \brief (PAC_INTFLAGE) DMAC */
#define PAC_INTFLAGE_DMAC           (0x1ul << PAC_INTFLAGE_DMAC_Pos)
#define PAC_INTFLAGE_MASK           0x00000003ul /**< \brief (PAC_INTFLAGE) MASK Register */

/* -------- PAC_STATUSA : (PAC Offset: 0x34) (R/  32) Peripheral write protection status - Bridge A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PM_:1;            /*!< bit:      0  PM APB Protect Enable              */
    uint32_t MCLK_:1;          /*!< bit:      1  MCLK APB Protect Enable            */
    uint32_t RSTC_:1;          /*!< bit:      2  RSTC APB Protect Enable            */
    uint32_t OSCCTRL_:1;       /*!< bit:      3  OSCCTRL APB Protect Enable         */
    uint32_t OSC32KCTRL_:1;    /*!< bit:      4  OSC32KCTRL APB Protect Enable      */
    uint32_t SUPC_:1;          /*!< bit:      5  SUPC APB Protect Enable            */
    uint32_t GCLK_:1;          /*!< bit:      6  GCLK APB Protect Enable            */
    uint32_t WDT_:1;           /*!< bit:      7  WDT APB Protect Enable             */
    uint32_t RTC_:1;           /*!< bit:      8  RTC APB Protect Enable             */
    uint32_t EIC_:1;           /*!< bit:      9  EIC APB Protect Enable             */
    uint32_t PORT_:1;          /*!< bit:     10  PORT APB Protect Enable            */
    uint32_t TAL_:1;           /*!< bit:     11  TAL APB Protect Enable             */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSA_OFFSET          0x34         /**< \brief (PAC_STATUSA offset) Peripheral write protection status - Bridge A */
#define PAC_STATUSA_RESETVALUE      0x00003000ul /**< \brief (PAC_STATUSA reset_value) Peripheral write protection status - Bridge A */

#define PAC_STATUSA_PM_Pos          0            /**< \brief (PAC_STATUSA) PM APB Protect Enable */
#define PAC_STATUSA_PM              (0x1ul << PAC_STATUSA_PM_Pos)
#define PAC_STATUSA_MCLK_Pos        1            /**< \brief (PAC_STATUSA) MCLK APB Protect Enable */
#define PAC_STATUSA_MCLK            (0x1ul << PAC_STATUSA_MCLK_Pos)
#define PAC_STATUSA_RSTC_Pos        2            /**< \brief (PAC_STATUSA) RSTC APB Protect Enable */
#define PAC_STATUSA_RSTC            (0x1ul << PAC_STATUSA_RSTC_Pos)
#define PAC_STATUSA_OSCCTRL_Pos     3            /**< \brief (PAC_STATUSA) OSCCTRL APB Protect Enable */
#define PAC_STATUSA_OSCCTRL         (0x1ul << PAC_STATUSA_OSCCTRL_Pos)
#define PAC_STATUSA_OSC32KCTRL_Pos  4            /**< \brief (PAC_STATUSA) OSC32KCTRL APB Protect Enable */
#define PAC_STATUSA_OSC32KCTRL      (0x1ul << PAC_STATUSA_OSC32KCTRL_Pos)
#define PAC_STATUSA_SUPC_Pos        5            /**< \brief (PAC_STATUSA) SUPC APB Protect Enable */
#define PAC_STATUSA_SUPC            (0x1ul << PAC_STATUSA_SUPC_Pos)
#define PAC_STATUSA_GCLK_Pos        6            /**< \brief (PAC_STATUSA) GCLK APB Protect Enable */
#define PAC_STATUSA_GCLK            (0x1ul << PAC_STATUSA_GCLK_Pos)
#define PAC_STATUSA_WDT_Pos         7            /**< \brief (PAC_STATUSA) WDT APB Protect Enable */
#define PAC_STATUSA_WDT             (0x1ul << PAC_STATUSA_WDT_Pos)
#define PAC_STATUSA_RTC_Pos         8            /**< \brief (PAC_STATUSA) RTC APB Protect Enable */
#define PAC_STATUSA_RTC             (0x1ul << PAC_STATUSA_RTC_Pos)
#define PAC_STATUSA_EIC_Pos         9            /**< \brief (PAC_STATUSA) EIC APB Protect Enable */
#define PAC_STATUSA_EIC             (0x1ul << PAC_STATUSA_EIC_Pos)
#define PAC_STATUSA_PORT_Pos        10           /**< \brief (PAC_STATUSA) PORT APB Protect Enable */
#define PAC_STATUSA_PORT            (0x1ul << PAC_STATUSA_PORT_Pos)
#define PAC_STATUSA_TAL_Pos         11           /**< \brief (PAC_STATUSA) TAL APB Protect Enable */
#define PAC_STATUSA_TAL             (0x1ul << PAC_STATUSA_TAL_Pos)
#define PAC_STATUSA_MASK            0x00000FFFul /**< \brief (PAC_STATUSA) MASK Register */

/* -------- PAC_STATUSB : (PAC Offset: 0x38) (R/  32) Peripheral write protection status - Bridge B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t USB_:1;           /*!< bit:      0  USB APB Protect Enable             */
    uint32_t DSU_:1;           /*!< bit:      1  DSU APB Protect Enable             */
    uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL APB Protect Enable         */
    uint32_t MTB_:1;           /*!< bit:      3  MTB APB Protect Enable             */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSB_OFFSET          0x38         /**< \brief (PAC_STATUSB offset) Peripheral write protection status - Bridge B */
#define PAC_STATUSB_RESETVALUE      0x00000002ul /**< \brief (PAC_STATUSB reset_value) Peripheral write protection status - Bridge B */

#define PAC_STATUSB_USB_Pos         0            /**< \brief (PAC_STATUSB) USB APB Protect Enable */
#define PAC_STATUSB_USB             (0x1ul << PAC_STATUSB_USB_Pos)
#define PAC_STATUSB_DSU_Pos         1            /**< \brief (PAC_STATUSB) DSU APB Protect Enable */
#define PAC_STATUSB_DSU             (0x1ul << PAC_STATUSB_DSU_Pos)
#define PAC_STATUSB_NVMCTRL_Pos     2            /**< \brief (PAC_STATUSB) NVMCTRL APB Protect Enable */
#define PAC_STATUSB_NVMCTRL         (0x1ul << PAC_STATUSB_NVMCTRL_Pos)
#define PAC_STATUSB_MTB_Pos         3            /**< \brief (PAC_STATUSB) MTB APB Protect Enable */
#define PAC_STATUSB_MTB             (0x1ul << PAC_STATUSB_MTB_Pos)
#define PAC_STATUSB_MASK            0x0000000Ful /**< \brief (PAC_STATUSB) MASK Register */

/* -------- PAC_STATUSC : (PAC Offset: 0x3C) (R/  32) Peripheral write protection status - Bridge C -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SERCOM0_:1;       /*!< bit:      0  SERCOM0 APB Protect Enable         */
    uint32_t SERCOM1_:1;       /*!< bit:      1  SERCOM1 APB Protect Enable         */
    uint32_t SERCOM2_:1;       /*!< bit:      2  SERCOM2 APB Protect Enable         */
    uint32_t SERCOM3_:1;       /*!< bit:      3  SERCOM3 APB Protect Enable         */
    uint32_t SERCOM4_:1;       /*!< bit:      4  SERCOM4 APB Protect Enable         */
    uint32_t TCC0_:1;          /*!< bit:      5  TCC0 APB Protect Enable            */
    uint32_t TCC1_:1;          /*!< bit:      6  TCC1 APB Protect Enable            */
    uint32_t TCC2_:1;          /*!< bit:      7  TCC2 APB Protect Enable            */
    uint32_t TC0_:1;           /*!< bit:      8  TC0 APB Protect Enable             */
    uint32_t TC1_:1;           /*!< bit:      9  TC1 APB Protect Enable             */
    uint32_t :22;              /*!< bit: 10..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSC_OFFSET          0x3C         /**< \brief (PAC_STATUSC offset) Peripheral write protection status - Bridge C */
#define PAC_STATUSC_RESETVALUE      0x00000000ul /**< \brief (PAC_STATUSC reset_value) Peripheral write protection status - Bridge C */

#define PAC_STATUSC_SERCOM0_Pos     0            /**< \brief (PAC_STATUSC) SERCOM0 APB Protect Enable */
#define PAC_STATUSC_SERCOM0         (0x1ul << PAC_STATUSC_SERCOM0_Pos)
#define PAC_STATUSC_SERCOM1_Pos     1            /**< \brief (PAC_STATUSC) SERCOM1 APB Protect Enable */
#define PAC_STATUSC_SERCOM1         (0x1ul << PAC_STATUSC_SERCOM1_Pos)
#define PAC_STATUSC_SERCOM2_Pos     2            /**< \brief (PAC_STATUSC) SERCOM2 APB Protect Enable */
#define PAC_STATUSC_SERCOM2         (0x1ul << PAC_STATUSC_SERCOM2_Pos)
#define PAC_STATUSC_SERCOM3_Pos     3            /**< \brief (PAC_STATUSC) SERCOM3 APB Protect Enable */
#define PAC_STATUSC_SERCOM3         (0x1ul << PAC_STATUSC_SERCOM3_Pos)
#define PAC_STATUSC_SERCOM4_Pos     4            /**< \brief (PAC_STATUSC) SERCOM4 APB Protect Enable */
#define PAC_STATUSC_SERCOM4         (0x1ul << PAC_STATUSC_SERCOM4_Pos)
#define PAC_STATUSC_TCC0_Pos        5            /**< \brief (PAC_STATUSC) TCC0 APB Protect Enable */
#define PAC_STATUSC_TCC0            (0x1ul << PAC_STATUSC_TCC0_Pos)
#define PAC_STATUSC_TCC1_Pos        6            /**< \brief (PAC_STATUSC) TCC1 APB Protect Enable */
#define PAC_STATUSC_TCC1            (0x1ul << PAC_STATUSC_TCC1_Pos)
#define PAC_STATUSC_TCC2_Pos        7            /**< \brief (PAC_STATUSC) TCC2 APB Protect Enable */
#define PAC_STATUSC_TCC2            (0x1ul << PAC_STATUSC_TCC2_Pos)
#define PAC_STATUSC_TC0_Pos         8            /**< \brief (PAC_STATUSC) TC0 APB Protect Enable */
#define PAC_STATUSC_TC0             (0x1ul << PAC_STATUSC_TC0_Pos)
#define PAC_STATUSC_TC1_Pos         9            /**< \brief (PAC_STATUSC) TC1 APB Protect Enable */
#define PAC_STATUSC_TC1             (0x1ul << PAC_STATUSC_TC1_Pos)
#define PAC_STATUSC_MASK            0x000003FFul /**< \brief (PAC_STATUSC) MASK Register */

/* -------- PAC_STATUSD : (PAC Offset: 0x40) (R/  32) Peripheral write protection status - Bridge D -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EVSYS_:1;         /*!< bit:      0  EVSYS APB Protect Enable           */
    uint32_t SERCOM5_:1;       /*!< bit:      1  SERCOM5 APB Protect Enable         */
    uint32_t TC4_:1;           /*!< bit:      2  TC4 APB Protect Enable             */
    uint32_t ADC_:1;           /*!< bit:      3  ADC APB Protect Enable             */
    uint32_t AC_:1;            /*!< bit:      4  AC APB Protect Enable              */
    uint32_t PTC_:1;           /*!< bit:      5  PTC APB Protect Enable             */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t CCL_:1;           /*!< bit:      7  CCL APB Protect Enable             */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSD_OFFSET          0x40         /**< \brief (PAC_STATUSD offset) Peripheral write protection status - Bridge D */
#define PAC_STATUSD_RESETVALUE      0x00000000ul /**< \brief (PAC_STATUSD reset_value) Peripheral write protection status - Bridge D */

#define PAC_STATUSD_EVSYS_Pos       0            /**< \brief (PAC_STATUSD) EVSYS APB Protect Enable */
#define PAC_STATUSD_EVSYS           (0x1ul << PAC_STATUSD_EVSYS_Pos)
#define PAC_STATUSD_SERCOM5_Pos     1            /**< \brief (PAC_STATUSD) SERCOM5 APB Protect Enable */
#define PAC_STATUSD_SERCOM5         (0x1ul << PAC_STATUSD_SERCOM5_Pos)
#define PAC_STATUSD_TC4_Pos         2            /**< \brief (PAC_STATUSD) TC4 APB Protect Enable */
#define PAC_STATUSD_TC4             (0x1ul << PAC_STATUSD_TC4_Pos)
#define PAC_STATUSD_ADC_Pos         3            /**< \brief (PAC_STATUSD) ADC APB Protect Enable */
#define PAC_STATUSD_ADC             (0x1ul << PAC_STATUSD_ADC_Pos)
#define PAC_STATUSD_AC_Pos          4            /**< \brief (PAC_STATUSD) AC APB Protect Enable */
#define PAC_STATUSD_AC              (0x1ul << PAC_STATUSD_AC_Pos)
#define PAC_STATUSD_PTC_Pos         5            /**< \brief (PAC_STATUSD) PTC APB Protect Enable */
#define PAC_STATUSD_PTC             (0x1ul << PAC_STATUSD_PTC_Pos)
#define PAC_STATUSD_CCL_Pos         7            /**< \brief (PAC_STATUSD) CCL APB Protect Enable */
#define PAC_STATUSD_CCL             (0x1ul << PAC_STATUSD_CCL_Pos)
#define PAC_STATUSD_MASK            0x000000BFul /**< \brief (PAC_STATUSD) MASK Register */

/* -------- PAC_STATUSE : (PAC Offset: 0x44) (R/  32) Peripheral write protection status - Bridge E -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC_:1;           /*!< bit:      0  PAC APB Protect Enable             */
    uint32_t DMAC_:1;          /*!< bit:      1  DMAC APB Protect Enable            */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSE_OFFSET          0x44         /**< \brief (PAC_STATUSE offset) Peripheral write protection status - Bridge E */
#define PAC_STATUSE_RESETVALUE      0x00000000ul /**< \brief (PAC_STATUSE reset_value) Peripheral write protection status - Bridge E */

#define PAC_STATUSE_PAC_Pos         0            /**< \brief (PAC_STATUSE) PAC APB Protect Enable */
#define PAC_STATUSE_PAC             (0x1ul << PAC_STATUSE_PAC_Pos)
#define PAC_STATUSE_DMAC_Pos        1            /**< \brief (PAC_STATUSE) DMAC APB Protect Enable */
#define PAC_STATUSE_DMAC            (0x1ul << PAC_STATUSE_DMAC_Pos)
#define PAC_STATUSE_MASK            0x00000003ul /**< \brief (PAC_STATUSE) MASK Register */

/** \brief PAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PAC_WRCTRL_Type           WRCTRL;      /**< \brief Offset: 0x00 (R/W 32) Write control */
  __IO PAC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x04 (R/W  8) Event control */
       RoReg8                    Reserved1[0x3];
  __IO PAC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt enable clear */
  __IO PAC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt enable set */
       RoReg8                    Reserved2[0x6];
  __IO PAC_INTFLAGAHB_Type       INTFLAGAHB;  /**< \brief Offset: 0x10 (R/W 32) Bridge interrupt flag status */
  __IO PAC_INTFLAGA_Type         INTFLAGA;    /**< \brief Offset: 0x14 (R/W 32) Peripheral interrupt flag status - Bridge A */
  __IO PAC_INTFLAGB_Type         INTFLAGB;    /**< \brief Offset: 0x18 (R/W 32) Peripheral interrupt flag status - Bridge B */
  __IO PAC_INTFLAGC_Type         INTFLAGC;    /**< \brief Offset: 0x1C (R/W 32) Peripheral interrupt flag status - Bridge C */
  __IO PAC_INTFLAGD_Type         INTFLAGD;    /**< \brief Offset: 0x20 (R/W 32) Peripheral interrupt flag status - Bridge D */
  __IO PAC_INTFLAGE_Type         INTFLAGE;    /**< \brief Offset: 0x24 (R/W 32) Peripheral interrupt flag status - Bridge E */
       RoReg8                    Reserved3[0xC];
  __I  PAC_STATUSA_Type          STATUSA;     /**< \brief Offset: 0x34 (R/  32) Peripheral write protection status - Bridge A */
  __I  PAC_STATUSB_Type          STATUSB;     /**< \brief Offset: 0x38 (R/  32) Peripheral write protection status - Bridge B */
  __I  PAC_STATUSC_Type          STATUSC;     /**< \brief Offset: 0x3C (R/  32) Peripheral write protection status - Bridge C */
  __I  PAC_STATUSD_Type          STATUSD;     /**< \brief Offset: 0x40 (R/  32) Peripheral write protection status - Bridge D */
  __I  PAC_STATUSE_Type          STATUSE;     /**< \brief Offset: 0x44 (R/  32) Peripheral write protection status - Bridge E */
} Pac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMR30_PAC_COMPONENT_ */
