/**
 * \file
 *
 * \brief Component description for GCLK
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

#ifndef _SAMR30_GCLK_COMPONENT_
#define _SAMR30_GCLK_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR GCLK */
/* ========================================================================== */
/** \addtogroup SAMR30_GCLK Generic Clock Generator */
/*@{*/

#define GCLK_U2122
#define REV_GCLK                    0x111

/* -------- GCLK_CTRLA : (GCLK Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} GCLK_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_CTRLA_OFFSET           0x00         /**< \brief (GCLK_CTRLA offset) Control */
#define GCLK_CTRLA_RESETVALUE       0x00ul       /**< \brief (GCLK_CTRLA reset_value) Control */

#define GCLK_CTRLA_SWRST_Pos        0            /**< \brief (GCLK_CTRLA) Software Reset */
#define GCLK_CTRLA_SWRST            (0x1ul << GCLK_CTRLA_SWRST_Pos)
#define GCLK_CTRLA_MASK             0x01ul       /**< \brief (GCLK_CTRLA) MASK Register */

/* -------- GCLK_SYNCBUSY : (GCLK Offset: 0x04) (R/  32) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Synchroniation Busy bit */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t GENCTRL:9;        /*!< bit:  2..10  Generic Clock Generator Control Synchronization Busy bits */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_SYNCBUSY_OFFSET        0x04         /**< \brief (GCLK_SYNCBUSY offset) Synchronization Busy */
#define GCLK_SYNCBUSY_RESETVALUE    0x00000000ul /**< \brief (GCLK_SYNCBUSY reset_value) Synchronization Busy */

#define GCLK_SYNCBUSY_SWRST_Pos     0            /**< \brief (GCLK_SYNCBUSY) Software Reset Synchroniation Busy bit */
#define GCLK_SYNCBUSY_SWRST         (0x1ul << GCLK_SYNCBUSY_SWRST_Pos)
#define GCLK_SYNCBUSY_GENCTRL_Pos   2            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL_Msk   (0x1FFul << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL(value) (GCLK_SYNCBUSY_GENCTRL_Msk & ((value) << GCLK_SYNCBUSY_GENCTRL_Pos))
#define   GCLK_SYNCBUSY_GENCTRL_GCLK0_Val 0x0ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 0 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK1_Val 0x1ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 1 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK2_Val 0x2ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 2 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK3_Val 0x3ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 3 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK4_Val 0x4ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 4 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK5_Val 0x5ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 5 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK6_Val 0x6ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 6 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK7_Val 0x7ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 7 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK8_Val 0x8ul  /**< \brief (GCLK_SYNCBUSY) Generic clock generator 8 */
#define GCLK_SYNCBUSY_GENCTRL_GCLK0 (GCLK_SYNCBUSY_GENCTRL_GCLK0_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK1 (GCLK_SYNCBUSY_GENCTRL_GCLK1_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK2 (GCLK_SYNCBUSY_GENCTRL_GCLK2_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK3 (GCLK_SYNCBUSY_GENCTRL_GCLK3_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK4 (GCLK_SYNCBUSY_GENCTRL_GCLK4_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK5 (GCLK_SYNCBUSY_GENCTRL_GCLK5_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK6 (GCLK_SYNCBUSY_GENCTRL_GCLK6_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK7 (GCLK_SYNCBUSY_GENCTRL_GCLK7_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK8 (GCLK_SYNCBUSY_GENCTRL_GCLK8_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_MASK          0x000007FDul /**< \brief (GCLK_SYNCBUSY) MASK Register */

/* -------- GCLK_GENCTRL : (GCLK Offset: 0x20) (R/W 32) Generic Clock Generator Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SRC:4;            /*!< bit:  0.. 3  Source Select                      */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t GENEN:1;          /*!< bit:      8  Generic Clock Generator Enable     */
    uint32_t IDC:1;            /*!< bit:      9  Improve Duty Cycle                 */
    uint32_t OOV:1;            /*!< bit:     10  Output Off Value                   */
    uint32_t OE:1;             /*!< bit:     11  Output Enable                      */
    uint32_t DIVSEL:1;         /*!< bit:     12  Divide Selection                   */
    uint32_t RUNSTDBY:1;       /*!< bit:     13  Run in Standby                     */
    uint32_t :2;               /*!< bit: 14..15  Reserved                           */
    uint32_t DIV:16;           /*!< bit: 16..31  Division Factor                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_GENCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_GENCTRL_OFFSET         0x20         /**< \brief (GCLK_GENCTRL offset) Generic Clock Generator Control */
#define GCLK_GENCTRL_RESETVALUE     0x00000000ul /**< \brief (GCLK_GENCTRL reset_value) Generic Clock Generator Control */

#define GCLK_GENCTRL_SRC_Pos        0            /**< \brief (GCLK_GENCTRL) Source Select */
#define GCLK_GENCTRL_SRC_Msk        (0xFul << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC(value)     (GCLK_GENCTRL_SRC_Msk & ((value) << GCLK_GENCTRL_SRC_Pos))
#define   GCLK_GENCTRL_SRC_XOSC_Val       0x0ul  /**< \brief (GCLK_GENCTRL) XOSC oscillator output */
#define   GCLK_GENCTRL_SRC_GCLKIN_Val     0x1ul  /**< \brief (GCLK_GENCTRL) Generator input pad */
#define   GCLK_GENCTRL_SRC_GCLKGEN1_Val   0x2ul  /**< \brief (GCLK_GENCTRL) Generic clock generator 1 output */
#define   GCLK_GENCTRL_SRC_OSCULP32K_Val  0x3ul  /**< \brief (GCLK_GENCTRL) OSCULP32K oscillator output */
#define   GCLK_GENCTRL_SRC_OSC32K_Val     0x4ul  /**< \brief (GCLK_GENCTRL) OSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_XOSC32K_Val    0x5ul  /**< \brief (GCLK_GENCTRL) XOSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_OSC16M_Val     0x6ul  /**< \brief (GCLK_GENCTRL) OSC16M oscillator output */
#define   GCLK_GENCTRL_SRC_DFLL48M_Val    0x7ul  /**< \brief (GCLK_GENCTRL) DFLL48M output */
#define   GCLK_GENCTRL_SRC_DPLL96M_Val    0x8ul  /**< \brief (GCLK_GENCTRL) DPLL96M output */
#define GCLK_GENCTRL_SRC_XOSC       (GCLK_GENCTRL_SRC_XOSC_Val     << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKIN     (GCLK_GENCTRL_SRC_GCLKIN_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKGEN1   (GCLK_GENCTRL_SRC_GCLKGEN1_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSCULP32K  (GCLK_GENCTRL_SRC_OSCULP32K_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSC32K     (GCLK_GENCTRL_SRC_OSC32K_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_XOSC32K    (GCLK_GENCTRL_SRC_XOSC32K_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSC16M     (GCLK_GENCTRL_SRC_OSC16M_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DFLL48M    (GCLK_GENCTRL_SRC_DFLL48M_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DPLL96M    (GCLK_GENCTRL_SRC_DPLL96M_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_GENEN_Pos      8            /**< \brief (GCLK_GENCTRL) Generic Clock Generator Enable */
#define GCLK_GENCTRL_GENEN          (0x1ul << GCLK_GENCTRL_GENEN_Pos)
#define GCLK_GENCTRL_IDC_Pos        9            /**< \brief (GCLK_GENCTRL) Improve Duty Cycle */
#define GCLK_GENCTRL_IDC            (0x1ul << GCLK_GENCTRL_IDC_Pos)
#define GCLK_GENCTRL_OOV_Pos        10           /**< \brief (GCLK_GENCTRL) Output Off Value */
#define GCLK_GENCTRL_OOV            (0x1ul << GCLK_GENCTRL_OOV_Pos)
#define GCLK_GENCTRL_OE_Pos         11           /**< \brief (GCLK_GENCTRL) Output Enable */
#define GCLK_GENCTRL_OE             (0x1ul << GCLK_GENCTRL_OE_Pos)
#define GCLK_GENCTRL_DIVSEL_Pos     12           /**< \brief (GCLK_GENCTRL) Divide Selection */
#define GCLK_GENCTRL_DIVSEL         (0x1ul << GCLK_GENCTRL_DIVSEL_Pos)
#define GCLK_GENCTRL_RUNSTDBY_Pos   13           /**< \brief (GCLK_GENCTRL) Run in Standby */
#define GCLK_GENCTRL_RUNSTDBY       (0x1ul << GCLK_GENCTRL_RUNSTDBY_Pos)
#define GCLK_GENCTRL_DIV_Pos        16           /**< \brief (GCLK_GENCTRL) Division Factor */
#define GCLK_GENCTRL_DIV_Msk        (0xFFFFul << GCLK_GENCTRL_DIV_Pos)
#define GCLK_GENCTRL_DIV(value)     (GCLK_GENCTRL_DIV_Msk & ((value) << GCLK_GENCTRL_DIV_Pos))
#define GCLK_GENCTRL_MASK           0xFFFF3F0Ful /**< \brief (GCLK_GENCTRL) MASK Register */

/* -------- GCLK_PCHCTRL : (GCLK Offset: 0x80) (R/W 32) Peripheral Clock Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t GEN:4;            /*!< bit:  0.. 3  Generic Clock Generator            */
    uint32_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint32_t CHEN:1;           /*!< bit:      6  Channel Enable                     */
    uint32_t WRTLOCK:1;        /*!< bit:      7  Write Lock                         */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_PCHCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_PCHCTRL_OFFSET         0x80         /**< \brief (GCLK_PCHCTRL offset) Peripheral Clock Control */
#define GCLK_PCHCTRL_RESETVALUE     0x00000000ul /**< \brief (GCLK_PCHCTRL reset_value) Peripheral Clock Control */

#define GCLK_PCHCTRL_GEN_Pos        0            /**< \brief (GCLK_PCHCTRL) Generic Clock Generator */
#define GCLK_PCHCTRL_GEN_Msk        (0xFul << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN(value)     (GCLK_PCHCTRL_GEN_Msk & ((value) << GCLK_PCHCTRL_GEN_Pos))
#define   GCLK_PCHCTRL_GEN_GCLK0_Val      0x0ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 0 */
#define   GCLK_PCHCTRL_GEN_GCLK1_Val      0x1ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 1 */
#define   GCLK_PCHCTRL_GEN_GCLK2_Val      0x2ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 2 */
#define   GCLK_PCHCTRL_GEN_GCLK3_Val      0x3ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 3 */
#define   GCLK_PCHCTRL_GEN_GCLK4_Val      0x4ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 4 */
#define   GCLK_PCHCTRL_GEN_GCLK5_Val      0x5ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 5 */
#define   GCLK_PCHCTRL_GEN_GCLK6_Val      0x6ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 6 */
#define   GCLK_PCHCTRL_GEN_GCLK7_Val      0x7ul  /**< \brief (GCLK_PCHCTRL) Generic clock generator 7 */
#define GCLK_PCHCTRL_GEN_GCLK0      (GCLK_PCHCTRL_GEN_GCLK0_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK1      (GCLK_PCHCTRL_GEN_GCLK1_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK2      (GCLK_PCHCTRL_GEN_GCLK2_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK3      (GCLK_PCHCTRL_GEN_GCLK3_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK4      (GCLK_PCHCTRL_GEN_GCLK4_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK5      (GCLK_PCHCTRL_GEN_GCLK5_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK6      (GCLK_PCHCTRL_GEN_GCLK6_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK7      (GCLK_PCHCTRL_GEN_GCLK7_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_CHEN_Pos       6            /**< \brief (GCLK_PCHCTRL) Channel Enable */
#define GCLK_PCHCTRL_CHEN           (0x1ul << GCLK_PCHCTRL_CHEN_Pos)
#define GCLK_PCHCTRL_WRTLOCK_Pos    7            /**< \brief (GCLK_PCHCTRL) Write Lock */
#define GCLK_PCHCTRL_WRTLOCK        (0x1ul << GCLK_PCHCTRL_WRTLOCK_Pos)
#define GCLK_PCHCTRL_MASK           0x000000CFul /**< \brief (GCLK_PCHCTRL) MASK Register */

/** \brief GCLK hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO GCLK_CTRLA_Type           CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control */
       RoReg8                    Reserved1[0x3];
  __I  GCLK_SYNCBUSY_Type        SYNCBUSY;    /**< \brief Offset: 0x04 (R/  32) Synchronization Busy */
       RoReg8                    Reserved2[0x18];
  __IO GCLK_GENCTRL_Type         GENCTRL[9];  /**< \brief Offset: 0x20 (R/W 32) Generic Clock Generator Control */
       RoReg8                    Reserved3[0x3C];
  __IO GCLK_PCHCTRL_Type         PCHCTRL[36]; /**< \brief Offset: 0x80 (R/W 32) Peripheral Clock Control */
} Gclk;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMR30_GCLK_COMPONENT_ */
