/**
 * \file
 *
 * \brief Component description for SUPC
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

#ifndef _SAMR30_SUPC_COMPONENT_
#define _SAMR30_SUPC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SUPC */
/* ========================================================================== */
/** \addtogroup SAMR30_SUPC Supply Controller */
/*@{*/

#define SUPC_U2117
#define REV_SUPC                    0x110

/* -------- SUPC_INTENCLR : (SUPC Offset: 0x00) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    uint32_t APWSRDY:1;        /*!< bit:      9  Automatic Power Switch Ready       */
    uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_INTENCLR_OFFSET        0x00         /**< \brief (SUPC_INTENCLR offset) Interrupt Enable Clear */
#define SUPC_INTENCLR_RESETVALUE    0x00000000ul /**< \brief (SUPC_INTENCLR reset_value) Interrupt Enable Clear */

#define SUPC_INTENCLR_BOD33RDY_Pos  0            /**< \brief (SUPC_INTENCLR) BOD33 Ready */
#define SUPC_INTENCLR_BOD33RDY      (0x1ul << SUPC_INTENCLR_BOD33RDY_Pos)
#define SUPC_INTENCLR_BOD33DET_Pos  1            /**< \brief (SUPC_INTENCLR) BOD33 Detection */
#define SUPC_INTENCLR_BOD33DET      (0x1ul << SUPC_INTENCLR_BOD33DET_Pos)
#define SUPC_INTENCLR_B33SRDY_Pos   2            /**< \brief (SUPC_INTENCLR) BOD33 Synchronization Ready */
#define SUPC_INTENCLR_B33SRDY       (0x1ul << SUPC_INTENCLR_B33SRDY_Pos)
#define SUPC_INTENCLR_BOD12RDY_Pos  3            /**< \brief (SUPC_INTENCLR) BOD12 Ready */
#define SUPC_INTENCLR_BOD12RDY      (0x1ul << SUPC_INTENCLR_BOD12RDY_Pos)
#define SUPC_INTENCLR_BOD12DET_Pos  4            /**< \brief (SUPC_INTENCLR) BOD12 Detection */
#define SUPC_INTENCLR_BOD12DET      (0x1ul << SUPC_INTENCLR_BOD12DET_Pos)
#define SUPC_INTENCLR_B12SRDY_Pos   5            /**< \brief (SUPC_INTENCLR) BOD12 Synchronization Ready */
#define SUPC_INTENCLR_B12SRDY       (0x1ul << SUPC_INTENCLR_B12SRDY_Pos)
#define SUPC_INTENCLR_VREGRDY_Pos   8            /**< \brief (SUPC_INTENCLR) Voltage Regulator Ready */
#define SUPC_INTENCLR_VREGRDY       (0x1ul << SUPC_INTENCLR_VREGRDY_Pos)
#define SUPC_INTENCLR_APWSRDY_Pos   9            /**< \brief (SUPC_INTENCLR) Automatic Power Switch Ready */
#define SUPC_INTENCLR_APWSRDY       (0x1ul << SUPC_INTENCLR_APWSRDY_Pos)
#define SUPC_INTENCLR_VCORERDY_Pos  10           /**< \brief (SUPC_INTENCLR) VDDCORE Ready */
#define SUPC_INTENCLR_VCORERDY      (0x1ul << SUPC_INTENCLR_VCORERDY_Pos)
#define SUPC_INTENCLR_MASK          0x0000073Ful /**< \brief (SUPC_INTENCLR) MASK Register */

/* -------- SUPC_INTENSET : (SUPC Offset: 0x04) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    uint32_t APWSRDY:1;        /*!< bit:      9  Automatic Power Switch Ready       */
    uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_INTENSET_OFFSET        0x04         /**< \brief (SUPC_INTENSET offset) Interrupt Enable Set */
#define SUPC_INTENSET_RESETVALUE    0x00000000ul /**< \brief (SUPC_INTENSET reset_value) Interrupt Enable Set */

#define SUPC_INTENSET_BOD33RDY_Pos  0            /**< \brief (SUPC_INTENSET) BOD33 Ready */
#define SUPC_INTENSET_BOD33RDY      (0x1ul << SUPC_INTENSET_BOD33RDY_Pos)
#define SUPC_INTENSET_BOD33DET_Pos  1            /**< \brief (SUPC_INTENSET) BOD33 Detection */
#define SUPC_INTENSET_BOD33DET      (0x1ul << SUPC_INTENSET_BOD33DET_Pos)
#define SUPC_INTENSET_B33SRDY_Pos   2            /**< \brief (SUPC_INTENSET) BOD33 Synchronization Ready */
#define SUPC_INTENSET_B33SRDY       (0x1ul << SUPC_INTENSET_B33SRDY_Pos)
#define SUPC_INTENSET_BOD12RDY_Pos  3            /**< \brief (SUPC_INTENSET) BOD12 Ready */
#define SUPC_INTENSET_BOD12RDY      (0x1ul << SUPC_INTENSET_BOD12RDY_Pos)
#define SUPC_INTENSET_BOD12DET_Pos  4            /**< \brief (SUPC_INTENSET) BOD12 Detection */
#define SUPC_INTENSET_BOD12DET      (0x1ul << SUPC_INTENSET_BOD12DET_Pos)
#define SUPC_INTENSET_B12SRDY_Pos   5            /**< \brief (SUPC_INTENSET) BOD12 Synchronization Ready */
#define SUPC_INTENSET_B12SRDY       (0x1ul << SUPC_INTENSET_B12SRDY_Pos)
#define SUPC_INTENSET_VREGRDY_Pos   8            /**< \brief (SUPC_INTENSET) Voltage Regulator Ready */
#define SUPC_INTENSET_VREGRDY       (0x1ul << SUPC_INTENSET_VREGRDY_Pos)
#define SUPC_INTENSET_APWSRDY_Pos   9            /**< \brief (SUPC_INTENSET) Automatic Power Switch Ready */
#define SUPC_INTENSET_APWSRDY       (0x1ul << SUPC_INTENSET_APWSRDY_Pos)
#define SUPC_INTENSET_VCORERDY_Pos  10           /**< \brief (SUPC_INTENSET) VDDCORE Ready */
#define SUPC_INTENSET_VCORERDY      (0x1ul << SUPC_INTENSET_VCORERDY_Pos)
#define SUPC_INTENSET_MASK          0x0000073Ful /**< \brief (SUPC_INTENSET) MASK Register */

/* -------- SUPC_INTFLAG : (SUPC Offset: 0x08) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    __I uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    __I uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    __I uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    __I uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    __I uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    __I uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    __I uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    __I uint32_t APWSRDY:1;        /*!< bit:      9  Automatic Power Switch Ready       */
    __I uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    __I uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_INTFLAG_OFFSET         0x08         /**< \brief (SUPC_INTFLAG offset) Interrupt Flag Status and Clear */
#define SUPC_INTFLAG_RESETVALUE     0x00000000ul /**< \brief (SUPC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define SUPC_INTFLAG_BOD33RDY_Pos   0            /**< \brief (SUPC_INTFLAG) BOD33 Ready */
#define SUPC_INTFLAG_BOD33RDY       (0x1ul << SUPC_INTFLAG_BOD33RDY_Pos)
#define SUPC_INTFLAG_BOD33DET_Pos   1            /**< \brief (SUPC_INTFLAG) BOD33 Detection */
#define SUPC_INTFLAG_BOD33DET       (0x1ul << SUPC_INTFLAG_BOD33DET_Pos)
#define SUPC_INTFLAG_B33SRDY_Pos    2            /**< \brief (SUPC_INTFLAG) BOD33 Synchronization Ready */
#define SUPC_INTFLAG_B33SRDY        (0x1ul << SUPC_INTFLAG_B33SRDY_Pos)
#define SUPC_INTFLAG_BOD12RDY_Pos   3            /**< \brief (SUPC_INTFLAG) BOD12 Ready */
#define SUPC_INTFLAG_BOD12RDY       (0x1ul << SUPC_INTFLAG_BOD12RDY_Pos)
#define SUPC_INTFLAG_BOD12DET_Pos   4            /**< \brief (SUPC_INTFLAG) BOD12 Detection */
#define SUPC_INTFLAG_BOD12DET       (0x1ul << SUPC_INTFLAG_BOD12DET_Pos)
#define SUPC_INTFLAG_B12SRDY_Pos    5            /**< \brief (SUPC_INTFLAG) BOD12 Synchronization Ready */
#define SUPC_INTFLAG_B12SRDY        (0x1ul << SUPC_INTFLAG_B12SRDY_Pos)
#define SUPC_INTFLAG_VREGRDY_Pos    8            /**< \brief (SUPC_INTFLAG) Voltage Regulator Ready */
#define SUPC_INTFLAG_VREGRDY        (0x1ul << SUPC_INTFLAG_VREGRDY_Pos)
#define SUPC_INTFLAG_APWSRDY_Pos    9            /**< \brief (SUPC_INTFLAG) Automatic Power Switch Ready */
#define SUPC_INTFLAG_APWSRDY        (0x1ul << SUPC_INTFLAG_APWSRDY_Pos)
#define SUPC_INTFLAG_VCORERDY_Pos   10           /**< \brief (SUPC_INTFLAG) VDDCORE Ready */
#define SUPC_INTFLAG_VCORERDY       (0x1ul << SUPC_INTFLAG_VCORERDY_Pos)
#define SUPC_INTFLAG_MASK           0x0000073Ful /**< \brief (SUPC_INTFLAG) MASK Register */

/* -------- SUPC_STATUS : (SUPC Offset: 0x0C) (R/  32) Power and Clocks Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    uint32_t APWSRDY:1;        /*!< bit:      9  Automatic Power Switch Ready       */
    uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    uint32_t BBPS:1;           /*!< bit:     11  Battery Backup Power Switch        */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_STATUS_OFFSET          0x0C         /**< \brief (SUPC_STATUS offset) Power and Clocks Status */
#define SUPC_STATUS_RESETVALUE      0x00000000ul /**< \brief (SUPC_STATUS reset_value) Power and Clocks Status */

#define SUPC_STATUS_BOD33RDY_Pos    0            /**< \brief (SUPC_STATUS) BOD33 Ready */
#define SUPC_STATUS_BOD33RDY        (0x1ul << SUPC_STATUS_BOD33RDY_Pos)
#define SUPC_STATUS_BOD33DET_Pos    1            /**< \brief (SUPC_STATUS) BOD33 Detection */
#define SUPC_STATUS_BOD33DET        (0x1ul << SUPC_STATUS_BOD33DET_Pos)
#define SUPC_STATUS_B33SRDY_Pos     2            /**< \brief (SUPC_STATUS) BOD33 Synchronization Ready */
#define SUPC_STATUS_B33SRDY         (0x1ul << SUPC_STATUS_B33SRDY_Pos)
#define SUPC_STATUS_BOD12RDY_Pos    3            /**< \brief (SUPC_STATUS) BOD12 Ready */
#define SUPC_STATUS_BOD12RDY        (0x1ul << SUPC_STATUS_BOD12RDY_Pos)
#define SUPC_STATUS_BOD12DET_Pos    4            /**< \brief (SUPC_STATUS) BOD12 Detection */
#define SUPC_STATUS_BOD12DET        (0x1ul << SUPC_STATUS_BOD12DET_Pos)
#define SUPC_STATUS_B12SRDY_Pos     5            /**< \brief (SUPC_STATUS) BOD12 Synchronization Ready */
#define SUPC_STATUS_B12SRDY         (0x1ul << SUPC_STATUS_B12SRDY_Pos)
#define SUPC_STATUS_VREGRDY_Pos     8            /**< \brief (SUPC_STATUS) Voltage Regulator Ready */
#define SUPC_STATUS_VREGRDY         (0x1ul << SUPC_STATUS_VREGRDY_Pos)
#define SUPC_STATUS_APWSRDY_Pos     9            /**< \brief (SUPC_STATUS) Automatic Power Switch Ready */
#define SUPC_STATUS_APWSRDY         (0x1ul << SUPC_STATUS_APWSRDY_Pos)
#define SUPC_STATUS_VCORERDY_Pos    10           /**< \brief (SUPC_STATUS) VDDCORE Ready */
#define SUPC_STATUS_VCORERDY        (0x1ul << SUPC_STATUS_VCORERDY_Pos)
#define SUPC_STATUS_BBPS_Pos        11           /**< \brief (SUPC_STATUS) Battery Backup Power Switch */
#define SUPC_STATUS_BBPS            (0x1ul << SUPC_STATUS_BBPS_Pos)
#define SUPC_STATUS_MASK            0x00000F3Ful /**< \brief (SUPC_STATUS) MASK Register */

/* -------- SUPC_BOD33 : (SUPC Offset: 0x10) (R/W 32) BOD33 Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t HYST:1;           /*!< bit:      2  Hysteresis Enable                  */
    uint32_t ACTION:2;         /*!< bit:  3.. 4  Action when Threshold Crossed      */
    uint32_t STDBYCFG:1;       /*!< bit:      5  Configuration in Standby mode      */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t RUNBKUP:1;        /*!< bit:      7  Configuration in Backup mode       */
    uint32_t ACTCFG:1;         /*!< bit:      8  Configuration in Active mode       */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t VMON:1;           /*!< bit:     10  Voltage Monitored in active and standby mode */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t PSEL:4;           /*!< bit: 12..15  Prescaler Select                   */
    uint32_t LEVEL:6;          /*!< bit: 16..21  Threshold Level for VDD            */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t BKUPLEVEL:6;      /*!< bit: 24..29  Threshold Level in backup sleep mode or for VBAT */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BOD33_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BOD33_OFFSET           0x10         /**< \brief (SUPC_BOD33 offset) BOD33 Control */
#define SUPC_BOD33_RESETVALUE       0x00000000ul /**< \brief (SUPC_BOD33 reset_value) BOD33 Control */

#define SUPC_BOD33_ENABLE_Pos       1            /**< \brief (SUPC_BOD33) Enable */
#define SUPC_BOD33_ENABLE           (0x1ul << SUPC_BOD33_ENABLE_Pos)
#define SUPC_BOD33_HYST_Pos         2            /**< \brief (SUPC_BOD33) Hysteresis Enable */
#define SUPC_BOD33_HYST             (0x1ul << SUPC_BOD33_HYST_Pos)
#define SUPC_BOD33_ACTION_Pos       3            /**< \brief (SUPC_BOD33) Action when Threshold Crossed */
#define SUPC_BOD33_ACTION_Msk       (0x3ul << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION(value)    (SUPC_BOD33_ACTION_Msk & ((value) << SUPC_BOD33_ACTION_Pos))
#define   SUPC_BOD33_ACTION_NONE_Val      0x0ul  /**< \brief (SUPC_BOD33) No action */
#define   SUPC_BOD33_ACTION_RESET_Val     0x1ul  /**< \brief (SUPC_BOD33) The BOD33 generates a reset */
#define   SUPC_BOD33_ACTION_INT_Val       0x2ul  /**< \brief (SUPC_BOD33) The BOD33 generates an interrupt */
#define   SUPC_BOD33_ACTION_BKUP_Val      0x3ul  /**< \brief (SUPC_BOD33) The BOD33 puts the device in backup sleep mode if VMON=0 */
#define SUPC_BOD33_ACTION_NONE      (SUPC_BOD33_ACTION_NONE_Val    << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION_RESET     (SUPC_BOD33_ACTION_RESET_Val   << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION_INT       (SUPC_BOD33_ACTION_INT_Val     << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION_BKUP      (SUPC_BOD33_ACTION_BKUP_Val    << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_STDBYCFG_Pos     5            /**< \brief (SUPC_BOD33) Configuration in Standby mode */
#define SUPC_BOD33_STDBYCFG         (0x1ul << SUPC_BOD33_STDBYCFG_Pos)
#define SUPC_BOD33_RUNSTDBY_Pos     6            /**< \brief (SUPC_BOD33) Run during Standby */
#define SUPC_BOD33_RUNSTDBY         (0x1ul << SUPC_BOD33_RUNSTDBY_Pos)
#define SUPC_BOD33_RUNBKUP_Pos      7            /**< \brief (SUPC_BOD33) Configuration in Backup mode */
#define SUPC_BOD33_RUNBKUP          (0x1ul << SUPC_BOD33_RUNBKUP_Pos)
#define SUPC_BOD33_ACTCFG_Pos       8            /**< \brief (SUPC_BOD33) Configuration in Active mode */
#define SUPC_BOD33_ACTCFG           (0x1ul << SUPC_BOD33_ACTCFG_Pos)
#define SUPC_BOD33_VMON_Pos         10           /**< \brief (SUPC_BOD33) Voltage Monitored in active and standby mode */
#define SUPC_BOD33_VMON             (0x1ul << SUPC_BOD33_VMON_Pos)
#define SUPC_BOD33_PSEL_Pos         12           /**< \brief (SUPC_BOD33) Prescaler Select */
#define SUPC_BOD33_PSEL_Msk         (0xFul << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL(value)      (SUPC_BOD33_PSEL_Msk & ((value) << SUPC_BOD33_PSEL_Pos))
#define   SUPC_BOD33_PSEL_DIV2_Val        0x0ul  /**< \brief (SUPC_BOD33) Divide clock by 2 */
#define   SUPC_BOD33_PSEL_DIV4_Val        0x1ul  /**< \brief (SUPC_BOD33) Divide clock by 4 */
#define   SUPC_BOD33_PSEL_DIV8_Val        0x2ul  /**< \brief (SUPC_BOD33) Divide clock by 8 */
#define   SUPC_BOD33_PSEL_DIV16_Val       0x3ul  /**< \brief (SUPC_BOD33) Divide clock by 16 */
#define   SUPC_BOD33_PSEL_DIV32_Val       0x4ul  /**< \brief (SUPC_BOD33) Divide clock by 32 */
#define   SUPC_BOD33_PSEL_DIV64_Val       0x5ul  /**< \brief (SUPC_BOD33) Divide clock by 64 */
#define   SUPC_BOD33_PSEL_DIV128_Val      0x6ul  /**< \brief (SUPC_BOD33) Divide clock by 128 */
#define   SUPC_BOD33_PSEL_DIV256_Val      0x7ul  /**< \brief (SUPC_BOD33) Divide clock by 256 */
#define   SUPC_BOD33_PSEL_DIV512_Val      0x8ul  /**< \brief (SUPC_BOD33) Divide clock by 512 */
#define   SUPC_BOD33_PSEL_DIV1024_Val     0x9ul  /**< \brief (SUPC_BOD33) Divide clock by 1024 */
#define   SUPC_BOD33_PSEL_DIV2048_Val     0xAul  /**< \brief (SUPC_BOD33) Divide clock by 2048 */
#define   SUPC_BOD33_PSEL_DIV4096_Val     0xBul  /**< \brief (SUPC_BOD33) Divide clock by 4096 */
#define   SUPC_BOD33_PSEL_DIV8192_Val     0xCul  /**< \brief (SUPC_BOD33) Divide clock by 8192 */
#define   SUPC_BOD33_PSEL_DIV16384_Val    0xDul  /**< \brief (SUPC_BOD33) Divide clock by 16384 */
#define   SUPC_BOD33_PSEL_DIV32768_Val    0xEul  /**< \brief (SUPC_BOD33) Divide clock by 32768 */
#define   SUPC_BOD33_PSEL_DIV65536_Val    0xFul  /**< \brief (SUPC_BOD33) Divide clock by 65536 */
#define SUPC_BOD33_PSEL_DIV2        (SUPC_BOD33_PSEL_DIV2_Val      << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV4        (SUPC_BOD33_PSEL_DIV4_Val      << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV8        (SUPC_BOD33_PSEL_DIV8_Val      << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV16       (SUPC_BOD33_PSEL_DIV16_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV32       (SUPC_BOD33_PSEL_DIV32_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV64       (SUPC_BOD33_PSEL_DIV64_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV128      (SUPC_BOD33_PSEL_DIV128_Val    << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV256      (SUPC_BOD33_PSEL_DIV256_Val    << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV512      (SUPC_BOD33_PSEL_DIV512_Val    << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV1024     (SUPC_BOD33_PSEL_DIV1024_Val   << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV2048     (SUPC_BOD33_PSEL_DIV2048_Val   << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV4096     (SUPC_BOD33_PSEL_DIV4096_Val   << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV8192     (SUPC_BOD33_PSEL_DIV8192_Val   << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV16384    (SUPC_BOD33_PSEL_DIV16384_Val  << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV32768    (SUPC_BOD33_PSEL_DIV32768_Val  << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV65536    (SUPC_BOD33_PSEL_DIV65536_Val  << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_LEVEL_Pos        16           /**< \brief (SUPC_BOD33) Threshold Level for VDD */
#define SUPC_BOD33_LEVEL_Msk        (0x3Ful << SUPC_BOD33_LEVEL_Pos)
#define SUPC_BOD33_LEVEL(value)     (SUPC_BOD33_LEVEL_Msk & ((value) << SUPC_BOD33_LEVEL_Pos))
#define SUPC_BOD33_BKUPLEVEL_Pos    24           /**< \brief (SUPC_BOD33) Threshold Level in backup sleep mode or for VBAT */
#define SUPC_BOD33_BKUPLEVEL_Msk    (0x3Ful << SUPC_BOD33_BKUPLEVEL_Pos)
#define SUPC_BOD33_BKUPLEVEL(value) (SUPC_BOD33_BKUPLEVEL_Msk & ((value) << SUPC_BOD33_BKUPLEVEL_Pos))
#define SUPC_BOD33_MASK             0x3F3FF5FEul /**< \brief (SUPC_BOD33) MASK Register */

/* -------- SUPC_BOD12 : (SUPC Offset: 0x14) (R/W 32) BOD12 Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t HYST:1;           /*!< bit:      2  Hysteresis Enable                  */
    uint32_t ACTION:2;         /*!< bit:  3.. 4  Action when Threshold Crossed      */
    uint32_t STDBYCFG:1;       /*!< bit:      5  Configuration in Standby mode      */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t ACTCFG:1;         /*!< bit:      8  Configuration in Active mode       */
    uint32_t :3;               /*!< bit:  9..11  Reserved                           */
    uint32_t PSEL:4;           /*!< bit: 12..15  Prescaler Select                   */
    uint32_t LEVEL:6;          /*!< bit: 16..21  Threshold Level                    */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BOD12_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BOD12_OFFSET           0x14         /**< \brief (SUPC_BOD12 offset) BOD12 Control */
#define SUPC_BOD12_RESETVALUE       0x00000000ul /**< \brief (SUPC_BOD12 reset_value) BOD12 Control */

#define SUPC_BOD12_ENABLE_Pos       1            /**< \brief (SUPC_BOD12) Enable */
#define SUPC_BOD12_ENABLE           (0x1ul << SUPC_BOD12_ENABLE_Pos)
#define SUPC_BOD12_HYST_Pos         2            /**< \brief (SUPC_BOD12) Hysteresis Enable */
#define SUPC_BOD12_HYST             (0x1ul << SUPC_BOD12_HYST_Pos)
#define SUPC_BOD12_ACTION_Pos       3            /**< \brief (SUPC_BOD12) Action when Threshold Crossed */
#define SUPC_BOD12_ACTION_Msk       (0x3ul << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_ACTION(value)    (SUPC_BOD12_ACTION_Msk & ((value) << SUPC_BOD12_ACTION_Pos))
#define   SUPC_BOD12_ACTION_NONE_Val      0x0ul  /**< \brief (SUPC_BOD12) No action */
#define   SUPC_BOD12_ACTION_RESET_Val     0x1ul  /**< \brief (SUPC_BOD12) The BOD12 generates a reset */
#define   SUPC_BOD12_ACTION_INT_Val       0x2ul  /**< \brief (SUPC_BOD12) The BOD12 generates an interrupt */
#define SUPC_BOD12_ACTION_NONE      (SUPC_BOD12_ACTION_NONE_Val    << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_ACTION_RESET     (SUPC_BOD12_ACTION_RESET_Val   << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_ACTION_INT       (SUPC_BOD12_ACTION_INT_Val     << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_STDBYCFG_Pos     5            /**< \brief (SUPC_BOD12) Configuration in Standby mode */
#define SUPC_BOD12_STDBYCFG         (0x1ul << SUPC_BOD12_STDBYCFG_Pos)
#define SUPC_BOD12_RUNSTDBY_Pos     6            /**< \brief (SUPC_BOD12) Run during Standby */
#define SUPC_BOD12_RUNSTDBY         (0x1ul << SUPC_BOD12_RUNSTDBY_Pos)
#define SUPC_BOD12_ACTCFG_Pos       8            /**< \brief (SUPC_BOD12) Configuration in Active mode */
#define SUPC_BOD12_ACTCFG           (0x1ul << SUPC_BOD12_ACTCFG_Pos)
#define SUPC_BOD12_PSEL_Pos         12           /**< \brief (SUPC_BOD12) Prescaler Select */
#define SUPC_BOD12_PSEL_Msk         (0xFul << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL(value)      (SUPC_BOD12_PSEL_Msk & ((value) << SUPC_BOD12_PSEL_Pos))
#define   SUPC_BOD12_PSEL_DIV2_Val        0x0ul  /**< \brief (SUPC_BOD12) Divide clock by 2 */
#define   SUPC_BOD12_PSEL_DIV4_Val        0x1ul  /**< \brief (SUPC_BOD12) Divide clock by 4 */
#define   SUPC_BOD12_PSEL_DIV8_Val        0x2ul  /**< \brief (SUPC_BOD12) Divide clock by 8 */
#define   SUPC_BOD12_PSEL_DIV16_Val       0x3ul  /**< \brief (SUPC_BOD12) Divide clock by 16 */
#define   SUPC_BOD12_PSEL_DIV32_Val       0x4ul  /**< \brief (SUPC_BOD12) Divide clock by 32 */
#define   SUPC_BOD12_PSEL_DIV64_Val       0x5ul  /**< \brief (SUPC_BOD12) Divide clock by 64 */
#define   SUPC_BOD12_PSEL_DIV128_Val      0x6ul  /**< \brief (SUPC_BOD12) Divide clock by 128 */
#define   SUPC_BOD12_PSEL_DIV256_Val      0x7ul  /**< \brief (SUPC_BOD12) Divide clock by 256 */
#define   SUPC_BOD12_PSEL_DIV512_Val      0x8ul  /**< \brief (SUPC_BOD12) Divide clock by 512 */
#define   SUPC_BOD12_PSEL_DIV1024_Val     0x9ul  /**< \brief (SUPC_BOD12) Divide clock by 1024 */
#define   SUPC_BOD12_PSEL_DIV2048_Val     0xAul  /**< \brief (SUPC_BOD12) Divide clock by 2048 */
#define   SUPC_BOD12_PSEL_DIV4096_Val     0xBul  /**< \brief (SUPC_BOD12) Divide clock by 4096 */
#define   SUPC_BOD12_PSEL_DIV8192_Val     0xCul  /**< \brief (SUPC_BOD12) Divide clock by 8192 */
#define   SUPC_BOD12_PSEL_DIV16384_Val    0xDul  /**< \brief (SUPC_BOD12) Divide clock by 16384 */
#define   SUPC_BOD12_PSEL_DIV32768_Val    0xEul  /**< \brief (SUPC_BOD12) Divide clock by 32768 */
#define   SUPC_BOD12_PSEL_DIV65536_Val    0xFul  /**< \brief (SUPC_BOD12) Divide clock by 65536 */
#define SUPC_BOD12_PSEL_DIV2        (SUPC_BOD12_PSEL_DIV2_Val      << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV4        (SUPC_BOD12_PSEL_DIV4_Val      << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV8        (SUPC_BOD12_PSEL_DIV8_Val      << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV16       (SUPC_BOD12_PSEL_DIV16_Val     << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV32       (SUPC_BOD12_PSEL_DIV32_Val     << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV64       (SUPC_BOD12_PSEL_DIV64_Val     << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV128      (SUPC_BOD12_PSEL_DIV128_Val    << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV256      (SUPC_BOD12_PSEL_DIV256_Val    << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV512      (SUPC_BOD12_PSEL_DIV512_Val    << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV1024     (SUPC_BOD12_PSEL_DIV1024_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV2048     (SUPC_BOD12_PSEL_DIV2048_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV4096     (SUPC_BOD12_PSEL_DIV4096_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV8192     (SUPC_BOD12_PSEL_DIV8192_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV16384    (SUPC_BOD12_PSEL_DIV16384_Val  << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV32768    (SUPC_BOD12_PSEL_DIV32768_Val  << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV65536    (SUPC_BOD12_PSEL_DIV65536_Val  << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_LEVEL_Pos        16           /**< \brief (SUPC_BOD12) Threshold Level */
#define SUPC_BOD12_LEVEL_Msk        (0x3Ful << SUPC_BOD12_LEVEL_Pos)
#define SUPC_BOD12_LEVEL(value)     (SUPC_BOD12_LEVEL_Msk & ((value) << SUPC_BOD12_LEVEL_Pos))
#define SUPC_BOD12_MASK             0x003FF17Eul /**< \brief (SUPC_BOD12) MASK Register */

/* -------- SUPC_VREG : (SUPC Offset: 0x18) (R/W 32) VREG Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t SEL:2;            /*!< bit:  2.. 3  Voltage Regulator Selection in active mode */
    uint32_t :1;               /*!< bit:      4  Reserved                           */
    uint32_t STDBYPL0:1;       /*!< bit:      5  Standby in PL0                     */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t LPEFF:1;          /*!< bit:      8  Low Power Efficiency               */
    uint32_t :7;               /*!< bit:  9..15  Reserved                           */
    uint32_t VSVSTEP:4;        /*!< bit: 16..19  Voltage Scaling Voltage Step       */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t VSPER:8;          /*!< bit: 24..31  Voltage Scaling Period             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_VREG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_VREG_OFFSET            0x18         /**< \brief (SUPC_VREG offset) VREG Control */
#define SUPC_VREG_RESETVALUE        0x00000000ul /**< \brief (SUPC_VREG reset_value) VREG Control */

#define SUPC_VREG_ENABLE_Pos        1            /**< \brief (SUPC_VREG) Enable */
#define SUPC_VREG_ENABLE            (0x1ul << SUPC_VREG_ENABLE_Pos)
#define SUPC_VREG_SEL_Pos           2            /**< \brief (SUPC_VREG) Voltage Regulator Selection in active mode */
#define SUPC_VREG_SEL_Msk           (0x3ul << SUPC_VREG_SEL_Pos)
#define SUPC_VREG_SEL(value)        (SUPC_VREG_SEL_Msk & ((value) << SUPC_VREG_SEL_Pos))
#define   SUPC_VREG_SEL_LDO_Val           0x0ul  /**< \brief (SUPC_VREG) LDO selection */
#define   SUPC_VREG_SEL_BUCK_Val          0x1ul  /**< \brief (SUPC_VREG) Buck selection */
#define   SUPC_VREG_SEL_SCVREG_Val        0x2ul  /**< \brief (SUPC_VREG) Switched Cap selection */
#define SUPC_VREG_SEL_LDO           (SUPC_VREG_SEL_LDO_Val         << SUPC_VREG_SEL_Pos)
#define SUPC_VREG_SEL_BUCK          (SUPC_VREG_SEL_BUCK_Val        << SUPC_VREG_SEL_Pos)
#define SUPC_VREG_SEL_SCVREG        (SUPC_VREG_SEL_SCVREG_Val      << SUPC_VREG_SEL_Pos)
#define SUPC_VREG_STDBYPL0_Pos      5            /**< \brief (SUPC_VREG) Standby in PL0 */
#define SUPC_VREG_STDBYPL0          (0x1ul << SUPC_VREG_STDBYPL0_Pos)
#define SUPC_VREG_RUNSTDBY_Pos      6            /**< \brief (SUPC_VREG) Run during Standby */
#define SUPC_VREG_RUNSTDBY          (0x1ul << SUPC_VREG_RUNSTDBY_Pos)
#define SUPC_VREG_LPEFF_Pos         8            /**< \brief (SUPC_VREG) Low Power Efficiency */
#define SUPC_VREG_LPEFF             (0x1ul << SUPC_VREG_LPEFF_Pos)
#define SUPC_VREG_VSVSTEP_Pos       16           /**< \brief (SUPC_VREG) Voltage Scaling Voltage Step */
#define SUPC_VREG_VSVSTEP_Msk       (0xFul << SUPC_VREG_VSVSTEP_Pos)
#define SUPC_VREG_VSVSTEP(value)    (SUPC_VREG_VSVSTEP_Msk & ((value) << SUPC_VREG_VSVSTEP_Pos))
#define SUPC_VREG_VSPER_Pos         24           /**< \brief (SUPC_VREG) Voltage Scaling Period */
#define SUPC_VREG_VSPER_Msk         (0xFFul << SUPC_VREG_VSPER_Pos)
#define SUPC_VREG_VSPER(value)      (SUPC_VREG_VSPER_Msk & ((value) << SUPC_VREG_VSPER_Pos))
#define SUPC_VREG_MASK              0xFF0F016Eul /**< \brief (SUPC_VREG) MASK Register */

/* -------- SUPC_VREF : (SUPC Offset: 0x1C) (R/W 32) VREF Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t TSEN:1;           /*!< bit:      1  Temperature Sensor Output Enable   */
    uint32_t VREFOE:1;         /*!< bit:      2  Voltage Reference Output Enable    */
    uint32_t :3;               /*!< bit:  3.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t ONDEMAND:1;       /*!< bit:      7  On Demand Contrl                   */
    uint32_t :8;               /*!< bit:  8..15  Reserved                           */
    uint32_t SEL:4;            /*!< bit: 16..19  Voltage Reference Selection        */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_VREF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_VREF_OFFSET            0x1C         /**< \brief (SUPC_VREF offset) VREF Control */
#define SUPC_VREF_RESETVALUE        0x00000000ul /**< \brief (SUPC_VREF reset_value) VREF Control */

#define SUPC_VREF_TSEN_Pos          1            /**< \brief (SUPC_VREF) Temperature Sensor Output Enable */
#define SUPC_VREF_TSEN              (0x1ul << SUPC_VREF_TSEN_Pos)
#define SUPC_VREF_VREFOE_Pos        2            /**< \brief (SUPC_VREF) Voltage Reference Output Enable */
#define SUPC_VREF_VREFOE            (0x1ul << SUPC_VREF_VREFOE_Pos)
#define SUPC_VREF_RUNSTDBY_Pos      6            /**< \brief (SUPC_VREF) Run during Standby */
#define SUPC_VREF_RUNSTDBY          (0x1ul << SUPC_VREF_RUNSTDBY_Pos)
#define SUPC_VREF_ONDEMAND_Pos      7            /**< \brief (SUPC_VREF) On Demand Contrl */
#define SUPC_VREF_ONDEMAND          (0x1ul << SUPC_VREF_ONDEMAND_Pos)
#define SUPC_VREF_SEL_Pos           16           /**< \brief (SUPC_VREF) Voltage Reference Selection */
#define SUPC_VREF_SEL_Msk           (0xFul << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL(value)        (SUPC_VREF_SEL_Msk & ((value) << SUPC_VREF_SEL_Pos))
#define   SUPC_VREF_SEL_1V0_Val           0x0ul  /**< \brief (SUPC_VREF) 1.0V voltage reference typical value */
#define   SUPC_VREF_SEL_1V1_Val           0x1ul  /**< \brief (SUPC_VREF) 1.1V voltage reference typical value */
#define   SUPC_VREF_SEL_1V2_Val           0x2ul  /**< \brief (SUPC_VREF) 1.2V voltage reference typical value */
#define   SUPC_VREF_SEL_1V25_Val          0x3ul  /**< \brief (SUPC_VREF) 1.25V voltage reference typical value */
#define   SUPC_VREF_SEL_2V0_Val           0x4ul  /**< \brief (SUPC_VREF) 2.0V voltage reference typical value */
#define   SUPC_VREF_SEL_2V2_Val           0x5ul  /**< \brief (SUPC_VREF) 2.2V voltage reference typical value */
#define   SUPC_VREF_SEL_2V4_Val           0x6ul  /**< \brief (SUPC_VREF) 2.4V voltage reference typical value */
#define   SUPC_VREF_SEL_2V5_Val           0x7ul  /**< \brief (SUPC_VREF) 2.5V voltage reference typical value */
#define SUPC_VREF_SEL_1V0           (SUPC_VREF_SEL_1V0_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_1V1           (SUPC_VREF_SEL_1V1_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_1V2           (SUPC_VREF_SEL_1V2_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_1V25          (SUPC_VREF_SEL_1V25_Val        << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V0           (SUPC_VREF_SEL_2V0_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V2           (SUPC_VREF_SEL_2V2_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V4           (SUPC_VREF_SEL_2V4_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V5           (SUPC_VREF_SEL_2V5_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_MASK              0x000F00C6ul /**< \brief (SUPC_VREF) MASK Register */

/* -------- SUPC_BBPS : (SUPC Offset: 0x20) (R/W 32) Battery Backup Power Switch -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CONF:2;           /*!< bit:  0.. 1  Battery Backup Configuration       */
    uint32_t WAKEEN:1;         /*!< bit:      2  Wake Enable                        */
    uint32_t PSOKEN:1;         /*!< bit:      3  Power Supply OK Enable             */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BBPS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BBPS_OFFSET            0x20         /**< \brief (SUPC_BBPS offset) Battery Backup Power Switch */
#define SUPC_BBPS_RESETVALUE        0x00000000ul /**< \brief (SUPC_BBPS reset_value) Battery Backup Power Switch */

#define SUPC_BBPS_CONF_Pos          0            /**< \brief (SUPC_BBPS) Battery Backup Configuration */
#define SUPC_BBPS_CONF_Msk          (0x3ul << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_CONF(value)       (SUPC_BBPS_CONF_Msk & ((value) << SUPC_BBPS_CONF_Pos))
#define   SUPC_BBPS_CONF_NONE_Val         0x0ul  /**< \brief (SUPC_BBPS) The backup domain is always supplied by main power */
#define   SUPC_BBPS_CONF_APWS_Val         0x1ul  /**< \brief (SUPC_BBPS) The power switch is handled by the automatic power switch */
#define   SUPC_BBPS_CONF_FORCED_Val       0x2ul  /**< \brief (SUPC_BBPS) The backup domain is always supplied by battery backup power */
#define   SUPC_BBPS_CONF_BOD33_Val        0x3ul  /**< \brief (SUPC_BBPS) The power switch is handled by the BOD33 */
#define SUPC_BBPS_CONF_NONE         (SUPC_BBPS_CONF_NONE_Val       << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_CONF_APWS         (SUPC_BBPS_CONF_APWS_Val       << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_CONF_FORCED       (SUPC_BBPS_CONF_FORCED_Val     << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_CONF_BOD33        (SUPC_BBPS_CONF_BOD33_Val      << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_WAKEEN_Pos        2            /**< \brief (SUPC_BBPS) Wake Enable */
#define SUPC_BBPS_WAKEEN            (0x1ul << SUPC_BBPS_WAKEEN_Pos)
#define SUPC_BBPS_PSOKEN_Pos        3            /**< \brief (SUPC_BBPS) Power Supply OK Enable */
#define SUPC_BBPS_PSOKEN            (0x1ul << SUPC_BBPS_PSOKEN_Pos)
#define SUPC_BBPS_MASK              0x0000000Ful /**< \brief (SUPC_BBPS) MASK Register */

/* -------- SUPC_BKOUT : (SUPC Offset: 0x24) (R/W 32) Backup Output Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EN:2;             /*!< bit:  0.. 1  Enable Output                      */
    uint32_t :6;               /*!< bit:  2.. 7  Reserved                           */
    uint32_t CLR:2;            /*!< bit:  8.. 9  Clear Output                       */
    uint32_t :6;               /*!< bit: 10..15  Reserved                           */
    uint32_t SET:2;            /*!< bit: 16..17  Set Output                         */
    uint32_t :6;               /*!< bit: 18..23  Reserved                           */
    uint32_t RTCTGL:2;         /*!< bit: 24..25  RTC Toggle Output                  */
    uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BKOUT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BKOUT_OFFSET           0x24         /**< \brief (SUPC_BKOUT offset) Backup Output Control */
#define SUPC_BKOUT_RESETVALUE       0x00000000ul /**< \brief (SUPC_BKOUT reset_value) Backup Output Control */

#define SUPC_BKOUT_EN_Pos           0            /**< \brief (SUPC_BKOUT) Enable Output */
#define SUPC_BKOUT_EN_Msk           (0x3ul << SUPC_BKOUT_EN_Pos)
#define SUPC_BKOUT_EN(value)        (SUPC_BKOUT_EN_Msk & ((value) << SUPC_BKOUT_EN_Pos))
#define SUPC_BKOUT_CLR_Pos          8            /**< \brief (SUPC_BKOUT) Clear Output */
#define SUPC_BKOUT_CLR_Msk          (0x3ul << SUPC_BKOUT_CLR_Pos)
#define SUPC_BKOUT_CLR(value)       (SUPC_BKOUT_CLR_Msk & ((value) << SUPC_BKOUT_CLR_Pos))
#define SUPC_BKOUT_SET_Pos          16           /**< \brief (SUPC_BKOUT) Set Output */
#define SUPC_BKOUT_SET_Msk          (0x3ul << SUPC_BKOUT_SET_Pos)
#define SUPC_BKOUT_SET(value)       (SUPC_BKOUT_SET_Msk & ((value) << SUPC_BKOUT_SET_Pos))
#define SUPC_BKOUT_RTCTGL_Pos       24           /**< \brief (SUPC_BKOUT) RTC Toggle Output */
#define SUPC_BKOUT_RTCTGL_Msk       (0x3ul << SUPC_BKOUT_RTCTGL_Pos)
#define SUPC_BKOUT_RTCTGL(value)    (SUPC_BKOUT_RTCTGL_Msk & ((value) << SUPC_BKOUT_RTCTGL_Pos))
#define SUPC_BKOUT_MASK             0x03030303ul /**< \brief (SUPC_BKOUT) MASK Register */

/* -------- SUPC_BKIN : (SUPC Offset: 0x28) (R/  32) Backup Input Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BKIN:8;           /*!< bit:  0.. 7  Backup Input Value                 */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BKIN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BKIN_OFFSET            0x28         /**< \brief (SUPC_BKIN offset) Backup Input Control */
#define SUPC_BKIN_RESETVALUE        0x00000000ul /**< \brief (SUPC_BKIN reset_value) Backup Input Control */

#define SUPC_BKIN_BKIN_Pos          0            /**< \brief (SUPC_BKIN) Backup Input Value */
#define SUPC_BKIN_BKIN_Msk          (0xFFul << SUPC_BKIN_BKIN_Pos)
#define SUPC_BKIN_BKIN(value)       (SUPC_BKIN_BKIN_Msk & ((value) << SUPC_BKIN_BKIN_Pos))
#define SUPC_BKIN_MASK              0x000000FFul /**< \brief (SUPC_BKIN) MASK Register */

/** \brief SUPC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO SUPC_INTENCLR_Type        INTENCLR;    /**< \brief Offset: 0x00 (R/W 32) Interrupt Enable Clear */
  __IO SUPC_INTENSET_Type        INTENSET;    /**< \brief Offset: 0x04 (R/W 32) Interrupt Enable Set */
  __IO SUPC_INTFLAG_Type         INTFLAG;     /**< \brief Offset: 0x08 (R/W 32) Interrupt Flag Status and Clear */
  __I  SUPC_STATUS_Type          STATUS;      /**< \brief Offset: 0x0C (R/  32) Power and Clocks Status */
  __IO SUPC_BOD33_Type           BOD33;       /**< \brief Offset: 0x10 (R/W 32) BOD33 Control */
  __IO SUPC_BOD12_Type           BOD12;       /**< \brief Offset: 0x14 (R/W 32) BOD12 Control */
  __IO SUPC_VREG_Type            VREG;        /**< \brief Offset: 0x18 (R/W 32) VREG Control */
  __IO SUPC_VREF_Type            VREF;        /**< \brief Offset: 0x1C (R/W 32) VREF Control */
  __IO SUPC_BBPS_Type            BBPS;        /**< \brief Offset: 0x20 (R/W 32) Battery Backup Power Switch */
  __IO SUPC_BKOUT_Type           BKOUT;       /**< \brief Offset: 0x24 (R/W 32) Backup Output Control */
  __I  SUPC_BKIN_Type            BKIN;        /**< \brief Offset: 0x28 (R/  32) Backup Input Control */
} Supc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMR30_SUPC_COMPONENT_ */
