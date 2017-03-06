/**
 * \file
 *
 * \brief Component description for OPAMP
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

#ifndef _SAML21_OPAMP_COMPONENT_
#define _SAML21_OPAMP_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR OPAMP */
/* ========================================================================== */
/** \addtogroup SAML21_OPAMP Operational Amplifier */
/*@{*/

#define OPAMP_U2237
#define REV_OPAMP                   0x110

/* -------- OPAMP_CTRLA : (OPAMP Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :5;               /*!< bit:  2.. 6  Reserved                           */
    uint8_t  LPMUX:1;          /*!< bit:      7  Low-Power Mux                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OPAMP_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OPAMP_CTRLA_OFFSET          0x00         /**< \brief (OPAMP_CTRLA offset) Control A */
#define OPAMP_CTRLA_RESETVALUE      0x00ul       /**< \brief (OPAMP_CTRLA reset_value) Control A */

#define OPAMP_CTRLA_SWRST_Pos       0            /**< \brief (OPAMP_CTRLA) Software Reset */
#define OPAMP_CTRLA_SWRST           (0x1ul << OPAMP_CTRLA_SWRST_Pos)
#define OPAMP_CTRLA_ENABLE_Pos      1            /**< \brief (OPAMP_CTRLA) Enable */
#define OPAMP_CTRLA_ENABLE          (0x1ul << OPAMP_CTRLA_ENABLE_Pos)
#define OPAMP_CTRLA_LPMUX_Pos       7            /**< \brief (OPAMP_CTRLA) Low-Power Mux */
#define OPAMP_CTRLA_LPMUX           (0x1ul << OPAMP_CTRLA_LPMUX_Pos)
#define OPAMP_CTRLA_MASK            0x83ul       /**< \brief (OPAMP_CTRLA) MASK Register */

/* -------- OPAMP_STATUS : (OPAMP Offset: 0x02) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  READY0:1;         /*!< bit:      0  OPAMP 0 Ready                      */
    uint8_t  READY1:1;         /*!< bit:      1  OPAMP 1 Ready                      */
    uint8_t  READY2:1;         /*!< bit:      2  OPAMP 2 Ready                      */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  READY:3;          /*!< bit:  0.. 2  OPAMP x Ready                      */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OPAMP_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OPAMP_STATUS_OFFSET         0x02         /**< \brief (OPAMP_STATUS offset) Status */
#define OPAMP_STATUS_RESETVALUE     0x00ul       /**< \brief (OPAMP_STATUS reset_value) Status */

#define OPAMP_STATUS_READY0_Pos     0            /**< \brief (OPAMP_STATUS) OPAMP 0 Ready */
#define OPAMP_STATUS_READY0         (1 << OPAMP_STATUS_READY0_Pos)
#define OPAMP_STATUS_READY1_Pos     1            /**< \brief (OPAMP_STATUS) OPAMP 1 Ready */
#define OPAMP_STATUS_READY1         (1 << OPAMP_STATUS_READY1_Pos)
#define OPAMP_STATUS_READY2_Pos     2            /**< \brief (OPAMP_STATUS) OPAMP 2 Ready */
#define OPAMP_STATUS_READY2         (1 << OPAMP_STATUS_READY2_Pos)
#define OPAMP_STATUS_READY_Pos      0            /**< \brief (OPAMP_STATUS) OPAMP x Ready */
#define OPAMP_STATUS_READY_Msk      (0x7ul << OPAMP_STATUS_READY_Pos)
#define OPAMP_STATUS_READY(value)   (OPAMP_STATUS_READY_Msk & ((value) << OPAMP_STATUS_READY_Pos))
#define OPAMP_STATUS_MASK           0x07ul       /**< \brief (OPAMP_STATUS) MASK Register */

/* -------- OPAMP_OPAMPCTRL : (OPAMP Offset: 0x04) (R/W 32) OPAMP n Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Operational Amplifier Enable       */
    uint32_t ANAOUT:1;         /*!< bit:      2  Analog Output                      */
    uint32_t BIAS:2;           /*!< bit:  3.. 4  Bias Selection                     */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint32_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint32_t RES2OUT:1;        /*!< bit:      8  Resistor ladder To Output          */
    uint32_t RES2VCC:1;        /*!< bit:      9  Resistor ladder To VCC             */
    uint32_t RES1EN:1;         /*!< bit:     10  Resistor 1 Enable                  */
    uint32_t RES1MUX:2;        /*!< bit: 11..12  Resistor 1 Mux                     */
    uint32_t POTMUX:3;         /*!< bit: 13..15  Potentiometer Selection            */
    uint32_t MUXPOS:3;         /*!< bit: 16..18  Positive Input Mux Selection       */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t MUXNEG:3;         /*!< bit: 20..22  Negative Input Mux Selection       */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OPAMP_OPAMPCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OPAMP_OPAMPCTRL_OFFSET      0x04         /**< \brief (OPAMP_OPAMPCTRL offset) OPAMP n Control */
#define OPAMP_OPAMPCTRL_RESETVALUE  0x00000000ul /**< \brief (OPAMP_OPAMPCTRL reset_value) OPAMP n Control */

#define OPAMP_OPAMPCTRL_ENABLE_Pos  1            /**< \brief (OPAMP_OPAMPCTRL) Operational Amplifier Enable */
#define OPAMP_OPAMPCTRL_ENABLE      (0x1ul << OPAMP_OPAMPCTRL_ENABLE_Pos)
#define OPAMP_OPAMPCTRL_ANAOUT_Pos  2            /**< \brief (OPAMP_OPAMPCTRL) Analog Output */
#define OPAMP_OPAMPCTRL_ANAOUT      (0x1ul << OPAMP_OPAMPCTRL_ANAOUT_Pos)
#define OPAMP_OPAMPCTRL_BIAS_Pos    3            /**< \brief (OPAMP_OPAMPCTRL) Bias Selection */
#define OPAMP_OPAMPCTRL_BIAS_Msk    (0x3ul << OPAMP_OPAMPCTRL_BIAS_Pos)
#define OPAMP_OPAMPCTRL_BIAS(value) (OPAMP_OPAMPCTRL_BIAS_Msk & ((value) << OPAMP_OPAMPCTRL_BIAS_Pos))
#define OPAMP_OPAMPCTRL_RUNSTDBY_Pos 6            /**< \brief (OPAMP_OPAMPCTRL) Run in Standby */
#define OPAMP_OPAMPCTRL_RUNSTDBY    (0x1ul << OPAMP_OPAMPCTRL_RUNSTDBY_Pos)
#define OPAMP_OPAMPCTRL_ONDEMAND_Pos 7            /**< \brief (OPAMP_OPAMPCTRL) On Demand Control */
#define OPAMP_OPAMPCTRL_ONDEMAND    (0x1ul << OPAMP_OPAMPCTRL_ONDEMAND_Pos)
#define OPAMP_OPAMPCTRL_RES2OUT_Pos 8            /**< \brief (OPAMP_OPAMPCTRL) Resistor ladder To Output */
#define OPAMP_OPAMPCTRL_RES2OUT     (0x1ul << OPAMP_OPAMPCTRL_RES2OUT_Pos)
#define OPAMP_OPAMPCTRL_RES2VCC_Pos 9            /**< \brief (OPAMP_OPAMPCTRL) Resistor ladder To VCC */
#define OPAMP_OPAMPCTRL_RES2VCC     (0x1ul << OPAMP_OPAMPCTRL_RES2VCC_Pos)
#define OPAMP_OPAMPCTRL_RES1EN_Pos  10           /**< \brief (OPAMP_OPAMPCTRL) Resistor 1 Enable */
#define OPAMP_OPAMPCTRL_RES1EN      (0x1ul << OPAMP_OPAMPCTRL_RES1EN_Pos)
#define OPAMP_OPAMPCTRL_RES1MUX_Pos 11           /**< \brief (OPAMP_OPAMPCTRL) Resistor 1 Mux */
#define OPAMP_OPAMPCTRL_RES1MUX_Msk (0x3ul << OPAMP_OPAMPCTRL_RES1MUX_Pos)
#define OPAMP_OPAMPCTRL_RES1MUX(value) (OPAMP_OPAMPCTRL_RES1MUX_Msk & ((value) << OPAMP_OPAMPCTRL_RES1MUX_Pos))
#define OPAMP_OPAMPCTRL_POTMUX_Pos  13           /**< \brief (OPAMP_OPAMPCTRL) Potentiometer Selection */
#define OPAMP_OPAMPCTRL_POTMUX_Msk  (0x7ul << OPAMP_OPAMPCTRL_POTMUX_Pos)
#define OPAMP_OPAMPCTRL_POTMUX(value) (OPAMP_OPAMPCTRL_POTMUX_Msk & ((value) << OPAMP_OPAMPCTRL_POTMUX_Pos))
#define OPAMP_OPAMPCTRL_MUXPOS_Pos  16           /**< \brief (OPAMP_OPAMPCTRL) Positive Input Mux Selection */
#define OPAMP_OPAMPCTRL_MUXPOS_Msk  (0x7ul << OPAMP_OPAMPCTRL_MUXPOS_Pos)
#define OPAMP_OPAMPCTRL_MUXPOS(value) (OPAMP_OPAMPCTRL_MUXPOS_Msk & ((value) << OPAMP_OPAMPCTRL_MUXPOS_Pos))
#define OPAMP_OPAMPCTRL_MUXNEG_Pos  20           /**< \brief (OPAMP_OPAMPCTRL) Negative Input Mux Selection */
#define OPAMP_OPAMPCTRL_MUXNEG_Msk  (0x7ul << OPAMP_OPAMPCTRL_MUXNEG_Pos)
#define OPAMP_OPAMPCTRL_MUXNEG(value) (OPAMP_OPAMPCTRL_MUXNEG_Msk & ((value) << OPAMP_OPAMPCTRL_MUXNEG_Pos))
#define OPAMP_OPAMPCTRL_MASK        0x0077FFDEul /**< \brief (OPAMP_OPAMPCTRL) MASK Register */

/** \brief OPAMP hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO OPAMP_CTRLA_Type          CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
       RoReg8                    Reserved1[0x1];
  __I  OPAMP_STATUS_Type         STATUS;      /**< \brief Offset: 0x02 (R/   8) Status */
       RoReg8                    Reserved2[0x1];
  __IO OPAMP_OPAMPCTRL_Type      OPAMPCTRL[3]; /**< \brief Offset: 0x04 (R/W 32) OPAMP n Control */
} Opamp;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAML21_OPAMP_COMPONENT_ */
