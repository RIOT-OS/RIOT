/**
 * \file
 *
 * \brief Component description for DAC
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAML21_DAC_COMPONENT_
#define _SAML21_DAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DAC */
/* ========================================================================== */
/** \addtogroup SAML21_DAC Digital-to-Analog Converter */
/*@{*/

#define DAC_U2244
#define REV_DAC                     0x100

/* -------- DAC_CTRLA : (DAC Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable DAC Controller              */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_CTRLA_OFFSET            0x00         /**< \brief (DAC_CTRLA offset) Control A */
#define DAC_CTRLA_RESETVALUE        0x00ul       /**< \brief (DAC_CTRLA reset_value) Control A */

#define DAC_CTRLA_SWRST_Pos         0            /**< \brief (DAC_CTRLA) Software Reset */
#define DAC_CTRLA_SWRST             (0x1ul << DAC_CTRLA_SWRST_Pos)
#define DAC_CTRLA_ENABLE_Pos        1            /**< \brief (DAC_CTRLA) Enable DAC Controller */
#define DAC_CTRLA_ENABLE            (0x1ul << DAC_CTRLA_ENABLE_Pos)
#define DAC_CTRLA_MASK              0x03ul       /**< \brief (DAC_CTRLA) MASK Register */

/* -------- DAC_CTRLB : (DAC Offset: 0x01) (R/W  8) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIFF:1;           /*!< bit:      0  Differential mode enable           */
    uint8_t  REFSEL:2;         /*!< bit:  1.. 2  Reference Selection for DAC0/1     */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_CTRLB_OFFSET            0x01         /**< \brief (DAC_CTRLB offset) Control B */
#define DAC_CTRLB_RESETVALUE        0x00ul       /**< \brief (DAC_CTRLB reset_value) Control B */

#define DAC_CTRLB_DIFF_Pos          0            /**< \brief (DAC_CTRLB) Differential mode enable */
#define DAC_CTRLB_DIFF              (0x1ul << DAC_CTRLB_DIFF_Pos)
#define DAC_CTRLB_REFSEL_Pos        1            /**< \brief (DAC_CTRLB) Reference Selection for DAC0/1 */
#define DAC_CTRLB_REFSEL_Msk        (0x3ul << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL(value)     ((DAC_CTRLB_REFSEL_Msk & ((value) << DAC_CTRLB_REFSEL_Pos)))
#define   DAC_CTRLB_REFSEL_VREFPU_Val     0x0ul  /**< \brief (DAC_CTRLB) External reference unbuffered */
#define   DAC_CTRLB_REFSEL_VDDANA_Val     0x1ul  /**< \brief (DAC_CTRLB) Analog supply */
#define   DAC_CTRLB_REFSEL_VREFPB_Val     0x2ul  /**< \brief (DAC_CTRLB) External reference buffered */
#define   DAC_CTRLB_REFSEL_INTREF_Val     0x3ul  /**< \brief (DAC_CTRLB) Internal bandgap reference */
#define DAC_CTRLB_REFSEL_VREFPU     (DAC_CTRLB_REFSEL_VREFPU_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_VDDANA     (DAC_CTRLB_REFSEL_VDDANA_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_VREFPB     (DAC_CTRLB_REFSEL_VREFPB_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_INTREF     (DAC_CTRLB_REFSEL_INTREF_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_MASK              0x07ul       /**< \brief (DAC_CTRLB) MASK Register */

/* -------- DAC_EVCTRL : (DAC Offset: 0x02) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STARTEI0:1;       /*!< bit:      0  Start Conversion Event Input DAC 0 */
    uint8_t  STARTEI1:1;       /*!< bit:      1  Start Conversion Event Input DAC 1 */
    uint8_t  EMPTYEO0:1;       /*!< bit:      2  Data Buffer Empty Event Output DAC 0 */
    uint8_t  EMPTYEO1:1;       /*!< bit:      3  Data Buffer Empty Event Output DAC 1 */
    uint8_t  INVEI0:1;         /*!< bit:      4  Enable Invertion of DAC 0 input event */
    uint8_t  INVEI1:1;         /*!< bit:      5  Enable Invertion of DAC 1 input event */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  STARTEI:2;        /*!< bit:  0.. 1  Start Conversion Event Input DAC x */
    uint8_t  EMPTYEO:2;        /*!< bit:  2.. 3  Data Buffer Empty Event Output DAC x */
    uint8_t  INVEI:2;          /*!< bit:  4.. 5  Enable Invertion of DAC x input event */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_EVCTRL_OFFSET           0x02         /**< \brief (DAC_EVCTRL offset) Event Control */
#define DAC_EVCTRL_RESETVALUE       0x00ul       /**< \brief (DAC_EVCTRL reset_value) Event Control */

#define DAC_EVCTRL_STARTEI0_Pos     0            /**< \brief (DAC_EVCTRL) Start Conversion Event Input DAC 0 */
#define DAC_EVCTRL_STARTEI0         (1 << DAC_EVCTRL_STARTEI0_Pos)
#define DAC_EVCTRL_STARTEI1_Pos     1            /**< \brief (DAC_EVCTRL) Start Conversion Event Input DAC 1 */
#define DAC_EVCTRL_STARTEI1         (1 << DAC_EVCTRL_STARTEI1_Pos)
#define DAC_EVCTRL_STARTEI_Pos      0            /**< \brief (DAC_EVCTRL) Start Conversion Event Input DAC x */
#define DAC_EVCTRL_STARTEI_Msk      (0x3ul << DAC_EVCTRL_STARTEI_Pos)
#define DAC_EVCTRL_STARTEI(value)   ((DAC_EVCTRL_STARTEI_Msk & ((value) << DAC_EVCTRL_STARTEI_Pos)))
#define DAC_EVCTRL_EMPTYEO0_Pos     2            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output DAC 0 */
#define DAC_EVCTRL_EMPTYEO0         (1 << DAC_EVCTRL_EMPTYEO0_Pos)
#define DAC_EVCTRL_EMPTYEO1_Pos     3            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output DAC 1 */
#define DAC_EVCTRL_EMPTYEO1         (1 << DAC_EVCTRL_EMPTYEO1_Pos)
#define DAC_EVCTRL_EMPTYEO_Pos      2            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output DAC x */
#define DAC_EVCTRL_EMPTYEO_Msk      (0x3ul << DAC_EVCTRL_EMPTYEO_Pos)
#define DAC_EVCTRL_EMPTYEO(value)   ((DAC_EVCTRL_EMPTYEO_Msk & ((value) << DAC_EVCTRL_EMPTYEO_Pos)))
#define DAC_EVCTRL_INVEI0_Pos       4            /**< \brief (DAC_EVCTRL) Enable Invertion of DAC 0 input event */
#define DAC_EVCTRL_INVEI0           (1 << DAC_EVCTRL_INVEI0_Pos)
#define DAC_EVCTRL_INVEI1_Pos       5            /**< \brief (DAC_EVCTRL) Enable Invertion of DAC 1 input event */
#define DAC_EVCTRL_INVEI1           (1 << DAC_EVCTRL_INVEI1_Pos)
#define DAC_EVCTRL_INVEI_Pos        4            /**< \brief (DAC_EVCTRL) Enable Invertion of DAC x input event */
#define DAC_EVCTRL_INVEI_Msk        (0x3ul << DAC_EVCTRL_INVEI_Pos)
#define DAC_EVCTRL_INVEI(value)     ((DAC_EVCTRL_INVEI_Msk & ((value) << DAC_EVCTRL_INVEI_Pos)))
#define DAC_EVCTRL_MASK             0x3Ful       /**< \brief (DAC_EVCTRL) MASK Register */

/* -------- DAC_INTENCLR : (DAC Offset: 0x04) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN0:1;      /*!< bit:      0  Underrun Interrupt Enable for DAC 0 */
    uint8_t  UNDERRUN1:1;      /*!< bit:      1  Underrun Interrupt Enable for DAC 1 */
    uint8_t  EMPTY0:1;         /*!< bit:      2  Data Buffer 0 Empty Interrupt Enable */
    uint8_t  EMPTY1:1;         /*!< bit:      3  Data Buffer 1 Empty Interrupt Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  UNDERRUN:2;       /*!< bit:  0.. 1  Underrun Interrupt Enable for DAC x */
    uint8_t  EMPTY:2;          /*!< bit:  2.. 3  Data Buffer x Empty Interrupt Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTENCLR_OFFSET         0x04         /**< \brief (DAC_INTENCLR offset) Interrupt Enable Clear */
#define DAC_INTENCLR_RESETVALUE     0x00ul       /**< \brief (DAC_INTENCLR reset_value) Interrupt Enable Clear */

#define DAC_INTENCLR_UNDERRUN0_Pos  0            /**< \brief (DAC_INTENCLR) Underrun Interrupt Enable for DAC 0 */
#define DAC_INTENCLR_UNDERRUN0      (1 << DAC_INTENCLR_UNDERRUN0_Pos)
#define DAC_INTENCLR_UNDERRUN1_Pos  1            /**< \brief (DAC_INTENCLR) Underrun Interrupt Enable for DAC 1 */
#define DAC_INTENCLR_UNDERRUN1      (1 << DAC_INTENCLR_UNDERRUN1_Pos)
#define DAC_INTENCLR_UNDERRUN_Pos   0            /**< \brief (DAC_INTENCLR) Underrun Interrupt Enable for DAC x */
#define DAC_INTENCLR_UNDERRUN_Msk   (0x3ul << DAC_INTENCLR_UNDERRUN_Pos)
#define DAC_INTENCLR_UNDERRUN(value) ((DAC_INTENCLR_UNDERRUN_Msk & ((value) << DAC_INTENCLR_UNDERRUN_Pos)))
#define DAC_INTENCLR_EMPTY0_Pos     2            /**< \brief (DAC_INTENCLR) Data Buffer 0 Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY0         (1 << DAC_INTENCLR_EMPTY0_Pos)
#define DAC_INTENCLR_EMPTY1_Pos     3            /**< \brief (DAC_INTENCLR) Data Buffer 1 Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY1         (1 << DAC_INTENCLR_EMPTY1_Pos)
#define DAC_INTENCLR_EMPTY_Pos      2            /**< \brief (DAC_INTENCLR) Data Buffer x Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY_Msk      (0x3ul << DAC_INTENCLR_EMPTY_Pos)
#define DAC_INTENCLR_EMPTY(value)   ((DAC_INTENCLR_EMPTY_Msk & ((value) << DAC_INTENCLR_EMPTY_Pos)))
#define DAC_INTENCLR_MASK           0x0Ful       /**< \brief (DAC_INTENCLR) MASK Register */

/* -------- DAC_INTENSET : (DAC Offset: 0x05) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN0:1;      /*!< bit:      0  Underrun Interrupt Enable for DAC 0 */
    uint8_t  UNDERRUN1:1;      /*!< bit:      1  Underrun Interrupt Enable for DAC 1 */
    uint8_t  EMPTY0:1;         /*!< bit:      2  Data Buffer 0 Empty Interrupt Enable */
    uint8_t  EMPTY1:1;         /*!< bit:      3  Data Buffer 1 Empty Interrupt Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  UNDERRUN:2;       /*!< bit:  0.. 1  Underrun Interrupt Enable for DAC x */
    uint8_t  EMPTY:2;          /*!< bit:  2.. 3  Data Buffer x Empty Interrupt Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTENSET_OFFSET         0x05         /**< \brief (DAC_INTENSET offset) Interrupt Enable Set */
#define DAC_INTENSET_RESETVALUE     0x00ul       /**< \brief (DAC_INTENSET reset_value) Interrupt Enable Set */

#define DAC_INTENSET_UNDERRUN0_Pos  0            /**< \brief (DAC_INTENSET) Underrun Interrupt Enable for DAC 0 */
#define DAC_INTENSET_UNDERRUN0      (1 << DAC_INTENSET_UNDERRUN0_Pos)
#define DAC_INTENSET_UNDERRUN1_Pos  1            /**< \brief (DAC_INTENSET) Underrun Interrupt Enable for DAC 1 */
#define DAC_INTENSET_UNDERRUN1      (1 << DAC_INTENSET_UNDERRUN1_Pos)
#define DAC_INTENSET_UNDERRUN_Pos   0            /**< \brief (DAC_INTENSET) Underrun Interrupt Enable for DAC x */
#define DAC_INTENSET_UNDERRUN_Msk   (0x3ul << DAC_INTENSET_UNDERRUN_Pos)
#define DAC_INTENSET_UNDERRUN(value) ((DAC_INTENSET_UNDERRUN_Msk & ((value) << DAC_INTENSET_UNDERRUN_Pos)))
#define DAC_INTENSET_EMPTY0_Pos     2            /**< \brief (DAC_INTENSET) Data Buffer 0 Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY0         (1 << DAC_INTENSET_EMPTY0_Pos)
#define DAC_INTENSET_EMPTY1_Pos     3            /**< \brief (DAC_INTENSET) Data Buffer 1 Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY1         (1 << DAC_INTENSET_EMPTY1_Pos)
#define DAC_INTENSET_EMPTY_Pos      2            /**< \brief (DAC_INTENSET) Data Buffer x Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY_Msk      (0x3ul << DAC_INTENSET_EMPTY_Pos)
#define DAC_INTENSET_EMPTY(value)   ((DAC_INTENSET_EMPTY_Msk & ((value) << DAC_INTENSET_EMPTY_Pos)))
#define DAC_INTENSET_MASK           0x0Ful       /**< \brief (DAC_INTENSET) MASK Register */

/* -------- DAC_INTFLAG : (DAC Offset: 0x06) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN0:1;      /*!< bit:      0  DAC 0 Underrun                     */
    uint8_t  UNDERRUN1:1;      /*!< bit:      1  DAC 1 Underrun                     */
    uint8_t  EMPTY0:1;         /*!< bit:      2  Data Buffer 0 Empty                */
    uint8_t  EMPTY1:1;         /*!< bit:      3  Data Buffer 1 Empty                */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  UNDERRUN:2;       /*!< bit:  0.. 1  DAC x Underrun                     */
    uint8_t  EMPTY:2;          /*!< bit:  2.. 3  Data Buffer x Empty                */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTFLAG_OFFSET          0x06         /**< \brief (DAC_INTFLAG offset) Interrupt Flag Status and Clear */
#define DAC_INTFLAG_RESETVALUE      0x00ul       /**< \brief (DAC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define DAC_INTFLAG_UNDERRUN0_Pos   0            /**< \brief (DAC_INTFLAG) DAC 0 Underrun */
#define DAC_INTFLAG_UNDERRUN0       (1 << DAC_INTFLAG_UNDERRUN0_Pos)
#define DAC_INTFLAG_UNDERRUN1_Pos   1            /**< \brief (DAC_INTFLAG) DAC 1 Underrun */
#define DAC_INTFLAG_UNDERRUN1       (1 << DAC_INTFLAG_UNDERRUN1_Pos)
#define DAC_INTFLAG_UNDERRUN_Pos    0            /**< \brief (DAC_INTFLAG) DAC x Underrun */
#define DAC_INTFLAG_UNDERRUN_Msk    (0x3ul << DAC_INTFLAG_UNDERRUN_Pos)
#define DAC_INTFLAG_UNDERRUN(value) ((DAC_INTFLAG_UNDERRUN_Msk & ((value) << DAC_INTFLAG_UNDERRUN_Pos)))
#define DAC_INTFLAG_EMPTY0_Pos      2            /**< \brief (DAC_INTFLAG) Data Buffer 0 Empty */
#define DAC_INTFLAG_EMPTY0          (1 << DAC_INTFLAG_EMPTY0_Pos)
#define DAC_INTFLAG_EMPTY1_Pos      3            /**< \brief (DAC_INTFLAG) Data Buffer 1 Empty */
#define DAC_INTFLAG_EMPTY1          (1 << DAC_INTFLAG_EMPTY1_Pos)
#define DAC_INTFLAG_EMPTY_Pos       2            /**< \brief (DAC_INTFLAG) Data Buffer x Empty */
#define DAC_INTFLAG_EMPTY_Msk       (0x3ul << DAC_INTFLAG_EMPTY_Pos)
#define DAC_INTFLAG_EMPTY(value)    ((DAC_INTFLAG_EMPTY_Msk & ((value) << DAC_INTFLAG_EMPTY_Pos)))
#define DAC_INTFLAG_MASK            0x0Ful       /**< \brief (DAC_INTFLAG) MASK Register */

/* -------- DAC_STATUS : (DAC Offset: 0x07) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  READY0:1;         /*!< bit:      0  DAC 0 Startup Ready                */
    uint8_t  READY1:1;         /*!< bit:      1  DAC 1 Startup Ready                */
    uint8_t  EOC0:1;           /*!< bit:      2  DAC 0 End of Conversion            */
    uint8_t  EOC1:1;           /*!< bit:      3  DAC 1 End of Conversion            */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  READY:2;          /*!< bit:  0.. 1  DAC x Startup Ready                */
    uint8_t  EOC:2;            /*!< bit:  2.. 3  DAC x End of Conversion            */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_STATUS_OFFSET           0x07         /**< \brief (DAC_STATUS offset) Status */
#define DAC_STATUS_RESETVALUE       0x00ul       /**< \brief (DAC_STATUS reset_value) Status */

#define DAC_STATUS_READY0_Pos       0            /**< \brief (DAC_STATUS) DAC 0 Startup Ready */
#define DAC_STATUS_READY0           (1 << DAC_STATUS_READY0_Pos)
#define DAC_STATUS_READY1_Pos       1            /**< \brief (DAC_STATUS) DAC 1 Startup Ready */
#define DAC_STATUS_READY1           (1 << DAC_STATUS_READY1_Pos)
#define DAC_STATUS_READY_Pos        0            /**< \brief (DAC_STATUS) DAC x Startup Ready */
#define DAC_STATUS_READY_Msk        (0x3ul << DAC_STATUS_READY_Pos)
#define DAC_STATUS_READY(value)     ((DAC_STATUS_READY_Msk & ((value) << DAC_STATUS_READY_Pos)))
#define DAC_STATUS_EOC0_Pos         2            /**< \brief (DAC_STATUS) DAC 0 End of Conversion */
#define DAC_STATUS_EOC0             (1 << DAC_STATUS_EOC0_Pos)
#define DAC_STATUS_EOC1_Pos         3            /**< \brief (DAC_STATUS) DAC 1 End of Conversion */
#define DAC_STATUS_EOC1             (1 << DAC_STATUS_EOC1_Pos)
#define DAC_STATUS_EOC_Pos          2            /**< \brief (DAC_STATUS) DAC x End of Conversion */
#define DAC_STATUS_EOC_Msk          (0x3ul << DAC_STATUS_EOC_Pos)
#define DAC_STATUS_EOC(value)       ((DAC_STATUS_EOC_Msk & ((value) << DAC_STATUS_EOC_Pos)))
#define DAC_STATUS_MASK             0x0Ful       /**< \brief (DAC_STATUS) MASK Register */

/* -------- DAC_SYNCBUSY : (DAC Offset: 0x08) (R/  32) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  DAC Enable Status                  */
    uint32_t DATA0:1;          /*!< bit:      2  Data DAC 0                         */
    uint32_t DATA1:1;          /*!< bit:      3  Data DAC 1                         */
    uint32_t DATABUF0:1;       /*!< bit:      4  Data Buffer DAC 0                  */
    uint32_t DATABUF1:1;       /*!< bit:      5  Data Buffer DAC 1                  */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t DATA:2;           /*!< bit:  2.. 3  Data DAC x                         */
    uint32_t DATABUF:2;        /*!< bit:  4.. 5  Data Buffer DAC x                  */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DAC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_SYNCBUSY_OFFSET         0x08         /**< \brief (DAC_SYNCBUSY offset) Synchronization Busy */
#define DAC_SYNCBUSY_RESETVALUE     0x00000000ul /**< \brief (DAC_SYNCBUSY reset_value) Synchronization Busy */

#define DAC_SYNCBUSY_SWRST_Pos      0            /**< \brief (DAC_SYNCBUSY) Software Reset */
#define DAC_SYNCBUSY_SWRST          (0x1ul << DAC_SYNCBUSY_SWRST_Pos)
#define DAC_SYNCBUSY_ENABLE_Pos     1            /**< \brief (DAC_SYNCBUSY) DAC Enable Status */
#define DAC_SYNCBUSY_ENABLE         (0x1ul << DAC_SYNCBUSY_ENABLE_Pos)
#define DAC_SYNCBUSY_DATA0_Pos      2            /**< \brief (DAC_SYNCBUSY) Data DAC 0 */
#define DAC_SYNCBUSY_DATA0          (1 << DAC_SYNCBUSY_DATA0_Pos)
#define DAC_SYNCBUSY_DATA1_Pos      3            /**< \brief (DAC_SYNCBUSY) Data DAC 1 */
#define DAC_SYNCBUSY_DATA1          (1 << DAC_SYNCBUSY_DATA1_Pos)
#define DAC_SYNCBUSY_DATA_Pos       2            /**< \brief (DAC_SYNCBUSY) Data DAC x */
#define DAC_SYNCBUSY_DATA_Msk       (0x3ul << DAC_SYNCBUSY_DATA_Pos)
#define DAC_SYNCBUSY_DATA(value)    ((DAC_SYNCBUSY_DATA_Msk & ((value) << DAC_SYNCBUSY_DATA_Pos)))
#define DAC_SYNCBUSY_DATABUF0_Pos   4            /**< \brief (DAC_SYNCBUSY) Data Buffer DAC 0 */
#define DAC_SYNCBUSY_DATABUF0       (1 << DAC_SYNCBUSY_DATABUF0_Pos)
#define DAC_SYNCBUSY_DATABUF1_Pos   5            /**< \brief (DAC_SYNCBUSY) Data Buffer DAC 1 */
#define DAC_SYNCBUSY_DATABUF1       (1 << DAC_SYNCBUSY_DATABUF1_Pos)
#define DAC_SYNCBUSY_DATABUF_Pos    4            /**< \brief (DAC_SYNCBUSY) Data Buffer DAC x */
#define DAC_SYNCBUSY_DATABUF_Msk    (0x3ul << DAC_SYNCBUSY_DATABUF_Pos)
#define DAC_SYNCBUSY_DATABUF(value) ((DAC_SYNCBUSY_DATABUF_Msk & ((value) << DAC_SYNCBUSY_DATABUF_Pos)))
#define DAC_SYNCBUSY_MASK           0x0000003Ful /**< \brief (DAC_SYNCBUSY) MASK Register */

/* -------- DAC_DACCTRL : (DAC Offset: 0x0C) (R/W 16) DAC n Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t LEFTADJ:1;        /*!< bit:      0  Left Adjusted Data                 */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable DAC0                        */
    uint16_t CCTRL:2;          /*!< bit:  2.. 3  Current Control                    */
    uint16_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t DITHER:1;         /*!< bit:      7  Dithering Mode                     */
    uint16_t REFRESH:4;        /*!< bit:  8..11  Refresh period                     */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DACCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DACCTRL_OFFSET          0x0C         /**< \brief (DAC_DACCTRL offset) DAC n Control */
#define DAC_DACCTRL_RESETVALUE      0x0000ul     /**< \brief (DAC_DACCTRL reset_value) DAC n Control */

#define DAC_DACCTRL_LEFTADJ_Pos     0            /**< \brief (DAC_DACCTRL) Left Adjusted Data */
#define DAC_DACCTRL_LEFTADJ         (0x1ul << DAC_DACCTRL_LEFTADJ_Pos)
#define DAC_DACCTRL_ENABLE_Pos      1            /**< \brief (DAC_DACCTRL) Enable DAC0 */
#define DAC_DACCTRL_ENABLE          (0x1ul << DAC_DACCTRL_ENABLE_Pos)
#define DAC_DACCTRL_CCTRL_Pos       2            /**< \brief (DAC_DACCTRL) Current Control */
#define DAC_DACCTRL_CCTRL_Msk       (0x3ul << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL(value)    ((DAC_DACCTRL_CCTRL_Msk & ((value) << DAC_DACCTRL_CCTRL_Pos)))
#define   DAC_DACCTRL_CCTRL_CC12M_Val     0x0ul  /**< \brief (DAC_DACCTRL) 1MHz<GCLK_DAC<12MHz */
#define   DAC_DACCTRL_CCTRL_CC1M_Val      0x1ul  /**< \brief (DAC_DACCTRL) 100kHz<GCLK_DAC<1MHz */
#define   DAC_DACCTRL_CCTRL_CC100K_Val    0x2ul  /**< \brief (DAC_DACCTRL) 10kHz<GCLK_DAC<100kHz */
#define   DAC_DACCTRL_CCTRL_CC10K_Val     0x3ul  /**< \brief (DAC_DACCTRL) GCLK_DAC<100kHz */
#define DAC_DACCTRL_CCTRL_CC12M     (DAC_DACCTRL_CCTRL_CC12M_Val   << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL_CC1M      (DAC_DACCTRL_CCTRL_CC1M_Val    << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL_CC100K    (DAC_DACCTRL_CCTRL_CC100K_Val  << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL_CC10K     (DAC_DACCTRL_CCTRL_CC10K_Val   << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_RUNSTDBY_Pos    6            /**< \brief (DAC_DACCTRL) Run in Standby */
#define DAC_DACCTRL_RUNSTDBY        (0x1ul << DAC_DACCTRL_RUNSTDBY_Pos)
#define DAC_DACCTRL_DITHER_Pos      7            /**< \brief (DAC_DACCTRL) Dithering Mode */
#define DAC_DACCTRL_DITHER          (0x1ul << DAC_DACCTRL_DITHER_Pos)
#define DAC_DACCTRL_REFRESH_Pos     8            /**< \brief (DAC_DACCTRL) Refresh period */
#define DAC_DACCTRL_REFRESH_Msk     (0xFul << DAC_DACCTRL_REFRESH_Pos)
#define DAC_DACCTRL_REFRESH(value)  ((DAC_DACCTRL_REFRESH_Msk & ((value) << DAC_DACCTRL_REFRESH_Pos)))
#define DAC_DACCTRL_MASK            0x0FCFul     /**< \brief (DAC_DACCTRL) MASK Register */

/* -------- DAC_DATA : (DAC Offset: 0x10) ( /W 16) DAC n Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DATA:16;          /*!< bit:  0..15  DAC0 Data                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DATA_OFFSET             0x10         /**< \brief (DAC_DATA offset) DAC n Data */
#define DAC_DATA_RESETVALUE         0x0000ul     /**< \brief (DAC_DATA reset_value) DAC n Data */

#define DAC_DATA_DATA_Pos           0            /**< \brief (DAC_DATA) DAC0 Data */
#define DAC_DATA_DATA_Msk           (0xFFFFul << DAC_DATA_DATA_Pos)
#define DAC_DATA_DATA(value)        ((DAC_DATA_DATA_Msk & ((value) << DAC_DATA_DATA_Pos)))
#define DAC_DATA_MASK               0xFFFFul     /**< \brief (DAC_DATA) MASK Register */

/* -------- DAC_DATABUF : (DAC Offset: 0x14) ( /W 16) DAC n Data Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DATABUF:16;       /*!< bit:  0..15  DAC0 Data Buffer                   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DATABUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DATABUF_OFFSET          0x14         /**< \brief (DAC_DATABUF offset) DAC n Data Buffer */
#define DAC_DATABUF_RESETVALUE      0x0000ul     /**< \brief (DAC_DATABUF reset_value) DAC n Data Buffer */

#define DAC_DATABUF_DATABUF_Pos     0            /**< \brief (DAC_DATABUF) DAC0 Data Buffer */
#define DAC_DATABUF_DATABUF_Msk     (0xFFFFul << DAC_DATABUF_DATABUF_Pos)
#define DAC_DATABUF_DATABUF(value)  ((DAC_DATABUF_DATABUF_Msk & ((value) << DAC_DATABUF_DATABUF_Pos)))
#define DAC_DATABUF_MASK            0xFFFFul     /**< \brief (DAC_DATABUF) MASK Register */

/* -------- DAC_DBGCTRL : (DAC Offset: 0x18) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Run                          */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DBGCTRL_OFFSET          0x18         /**< \brief (DAC_DBGCTRL offset) Debug Control */
#define DAC_DBGCTRL_RESETVALUE      0x00ul       /**< \brief (DAC_DBGCTRL reset_value) Debug Control */

#define DAC_DBGCTRL_DBGRUN_Pos      0            /**< \brief (DAC_DBGCTRL) Debug Run */
#define DAC_DBGCTRL_DBGRUN          (0x1ul << DAC_DBGCTRL_DBGRUN_Pos)
#define DAC_DBGCTRL_MASK            0x01ul       /**< \brief (DAC_DBGCTRL) MASK Register */

/** \brief DAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DAC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
  __IO DAC_CTRLB_Type            CTRLB;       /**< \brief Offset: 0x01 (R/W  8) Control B */
  __IO DAC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x02 (R/W  8) Event Control */
       RoReg8                    Reserved1[0x1];
  __IO DAC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO DAC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO DAC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
  __I  DAC_STATUS_Type           STATUS;      /**< \brief Offset: 0x07 (R/   8) Status */
  __I  DAC_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x08 (R/  32) Synchronization Busy */
  __IO DAC_DACCTRL_Type          DACCTRL[2];  /**< \brief Offset: 0x0C (R/W 16) DAC n Control */
  __O  DAC_DATA_Type             DATA[2];     /**< \brief Offset: 0x10 ( /W 16) DAC n Data */
  __O  DAC_DATABUF_Type          DATABUF[2];  /**< \brief Offset: 0x14 ( /W 16) DAC n Data Buffer */
  __IO DAC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x18 (R/W  8) Debug Control */
} Dac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAML21_DAC_COMPONENT_ */
