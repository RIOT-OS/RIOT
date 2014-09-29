/**
 * \file
 *
 * \brief Component description for GCLK
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _SAMR21_GCLK_COMPONENT_
#define _SAMR21_GCLK_COMPONENT_

/** \addtogroup SAMR21G18A
/* @{ */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR GCLK */
/* ========================================================================== */
/** \addtogroup SAMR21_GCLK Generic Clock Generator */
/*@{*/

#define GCLK_U2102
#define REV_GCLK                    0x210

/* -------- GCLK_CTRL : (GCLK Offset: 0x0) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} GCLK_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_CTRL_OFFSET            0x0          /**< \brief (GCLK_CTRL offset) Control */
#define GCLK_CTRL_RESETVALUE        0x00         /**< \brief (GCLK_CTRL reset_value) Control */

#define GCLK_CTRL_SWRST_Pos         0            /**< \brief (GCLK_CTRL) Software Reset */
#define GCLK_CTRL_SWRST             (0x1u << GCLK_CTRL_SWRST_Pos)
#define GCLK_CTRL_MASK              0x01u        /**< \brief (GCLK_CTRL) MASK Register */

/* -------- GCLK_STATUS : (GCLK Offset: 0x1) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy Status        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} GCLK_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_STATUS_OFFSET          0x1          /**< \brief (GCLK_STATUS offset) Status */
#define GCLK_STATUS_RESETVALUE      0x00         /**< \brief (GCLK_STATUS reset_value) Status */

#define GCLK_STATUS_SYNCBUSY_Pos    7            /**< \brief (GCLK_STATUS) Synchronization Busy Status */
#define GCLK_STATUS_SYNCBUSY        (0x1u << GCLK_STATUS_SYNCBUSY_Pos)
#define GCLK_STATUS_MASK            0x80u        /**< \brief (GCLK_STATUS) MASK Register */

/* -------- GCLK_CLKCTRL : (GCLK Offset: 0x2) (R/W 16) Generic Clock Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ID:6;             /*!< bit:  0.. 5  Generic Clock Selection ID         */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t GEN:4;            /*!< bit:  8..11  Generic Clock Generator            */
    uint16_t :2;               /*!< bit: 12..13  Reserved                           */
    uint16_t CLKEN:1;          /*!< bit:     14  Clock Enable                       */
    uint16_t WRTLOCK:1;        /*!< bit:     15  Write Lock                         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} GCLK_CLKCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_CLKCTRL_OFFSET         0x2          /**< \brief (GCLK_CLKCTRL offset) Generic Clock Control */
#define GCLK_CLKCTRL_RESETVALUE     0x0000       /**< \brief (GCLK_CLKCTRL reset_value) Generic Clock Control */

#define GCLK_CLKCTRL_ID_Pos         0            /**< \brief (GCLK_CLKCTRL) Generic Clock Selection ID */
#define GCLK_CLKCTRL_ID_Msk         (0x3Fu << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID(value)      ((GCLK_CLKCTRL_ID_Msk & ((value) << GCLK_CLKCTRL_ID_Pos)))
#define GCLK_CLKCTRL_GEN_Pos        8            /**< \brief (GCLK_CLKCTRL) Generic Clock Generator */
#define GCLK_CLKCTRL_GEN_Msk        (0xFu << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN(value)     ((GCLK_CLKCTRL_GEN_Msk & ((value) << GCLK_CLKCTRL_GEN_Pos)))
#define   GCLK_CLKCTRL_GEN_GCLK0_Val      0x0u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 0 */
#define   GCLK_CLKCTRL_GEN_GCLK1_Val      0x1u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 1 */
#define   GCLK_CLKCTRL_GEN_GCLK2_Val      0x2u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 2 */
#define   GCLK_CLKCTRL_GEN_GCLK3_Val      0x3u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 3 */
#define   GCLK_CLKCTRL_GEN_GCLK4_Val      0x4u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 4 */
#define   GCLK_CLKCTRL_GEN_GCLK5_Val      0x5u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 5 */
#define   GCLK_CLKCTRL_GEN_GCLK6_Val      0x6u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 6 */
#define   GCLK_CLKCTRL_GEN_GCLK7_Val      0x7u   /**< \brief (GCLK_CLKCTRL) Generic clock generator 7 */
#define GCLK_CLKCTRL_GEN_GCLK0      (GCLK_CLKCTRL_GEN_GCLK0_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK1      (GCLK_CLKCTRL_GEN_GCLK1_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK2      (GCLK_CLKCTRL_GEN_GCLK2_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK3      (GCLK_CLKCTRL_GEN_GCLK3_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK4      (GCLK_CLKCTRL_GEN_GCLK4_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK5      (GCLK_CLKCTRL_GEN_GCLK5_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK6      (GCLK_CLKCTRL_GEN_GCLK6_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK7      (GCLK_CLKCTRL_GEN_GCLK7_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_CLKEN_Pos      14           /**< \brief (GCLK_CLKCTRL) Clock Enable */
#define GCLK_CLKCTRL_CLKEN          (0x1u << GCLK_CLKCTRL_CLKEN_Pos)
#define GCLK_CLKCTRL_WRTLOCK_Pos    15           /**< \brief (GCLK_CLKCTRL) Write Lock */
#define GCLK_CLKCTRL_WRTLOCK        (0x1u << GCLK_CLKCTRL_WRTLOCK_Pos)
#define GCLK_CLKCTRL_MASK           0xCF3Fu      /**< \brief (GCLK_CLKCTRL) MASK Register */

/* -------- GCLK_GENCTRL : (GCLK Offset: 0x4) (R/W 32) Generic Clock Generator Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:4;             /*!< bit:  0.. 3  Generic Clock Generator Selection  */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t SRC:5;            /*!< bit:  8..12  Source Select                      */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t GENEN:1;          /*!< bit:     16  Generic Clock Generator Enable     */
    uint32_t IDC:1;            /*!< bit:     17  Improve Duty Cycle                 */
    uint32_t OOV:1;            /*!< bit:     18  Output Off Value                   */
    uint32_t OE:1;             /*!< bit:     19  Output Enable                      */
    uint32_t DIVSEL:1;         /*!< bit:     20  Divide Selection                   */
    uint32_t RUNSTDBY:1;       /*!< bit:     21  Run in Standby                     */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_GENCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_GENCTRL_OFFSET         0x4          /**< \brief (GCLK_GENCTRL offset) Generic Clock Generator Control */
#define GCLK_GENCTRL_RESETVALUE     0x00000000   /**< \brief (GCLK_GENCTRL reset_value) Generic Clock Generator Control */

#define GCLK_GENCTRL_ID_Pos         0            /**< \brief (GCLK_GENCTRL) Generic Clock Generator Selection */
#define GCLK_GENCTRL_ID_Msk         (0xFu << GCLK_GENCTRL_ID_Pos)
#define GCLK_GENCTRL_ID(value)      ((GCLK_GENCTRL_ID_Msk & ((value) << GCLK_GENCTRL_ID_Pos)))
#define GCLK_GENCTRL_SRC_Pos        8            /**< \brief (GCLK_GENCTRL) Source Select */
#define GCLK_GENCTRL_SRC_Msk        (0x1Fu << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC(value)     ((GCLK_GENCTRL_SRC_Msk & ((value) << GCLK_GENCTRL_SRC_Pos)))
#define   GCLK_GENCTRL_SRC_XOSC_Val       0x0u   /**< \brief (GCLK_GENCTRL) XOSC oscillator output */
#define   GCLK_GENCTRL_SRC_GCLKIN_Val     0x1u   /**< \brief (GCLK_GENCTRL) Generator input pad */
#define   GCLK_GENCTRL_SRC_GCLKGEN1_Val   0x2u   /**< \brief (GCLK_GENCTRL) Generic clock generator 1 output */
#define   GCLK_GENCTRL_SRC_OSCULP32K_Val  0x3u   /**< \brief (GCLK_GENCTRL) OSCULP32K oscillator output */
#define   GCLK_GENCTRL_SRC_OSC32K_Val     0x4u   /**< \brief (GCLK_GENCTRL) OSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_XOSC32K_Val    0x5u   /**< \brief (GCLK_GENCTRL) XOSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_OSC8M_Val      0x6u   /**< \brief (GCLK_GENCTRL) OSC8M oscillator output */
#define   GCLK_GENCTRL_SRC_DFLL48M_Val    0x7u   /**< \brief (GCLK_GENCTRL) DFLL48M output */
#define GCLK_GENCTRL_SRC_XOSC       (GCLK_GENCTRL_SRC_XOSC_Val     << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKIN     (GCLK_GENCTRL_SRC_GCLKIN_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKGEN1   (GCLK_GENCTRL_SRC_GCLKGEN1_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSCULP32K  (GCLK_GENCTRL_SRC_OSCULP32K_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSC32K     (GCLK_GENCTRL_SRC_OSC32K_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_XOSC32K    (GCLK_GENCTRL_SRC_XOSC32K_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSC8M      (GCLK_GENCTRL_SRC_OSC8M_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DFLL48M    (GCLK_GENCTRL_SRC_DFLL48M_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_GENEN_Pos      16           /**< \brief (GCLK_GENCTRL) Generic Clock Generator Enable */
#define GCLK_GENCTRL_GENEN          (0x1u << GCLK_GENCTRL_GENEN_Pos)
#define GCLK_GENCTRL_IDC_Pos        17           /**< \brief (GCLK_GENCTRL) Improve Duty Cycle */
#define GCLK_GENCTRL_IDC            (0x1u << GCLK_GENCTRL_IDC_Pos)
#define GCLK_GENCTRL_OOV_Pos        18           /**< \brief (GCLK_GENCTRL) Output Off Value */
#define GCLK_GENCTRL_OOV            (0x1u << GCLK_GENCTRL_OOV_Pos)
#define GCLK_GENCTRL_OE_Pos         19           /**< \brief (GCLK_GENCTRL) Output Enable */
#define GCLK_GENCTRL_OE             (0x1u << GCLK_GENCTRL_OE_Pos)
#define GCLK_GENCTRL_DIVSEL_Pos     20           /**< \brief (GCLK_GENCTRL) Divide Selection */
#define GCLK_GENCTRL_DIVSEL         (0x1u << GCLK_GENCTRL_DIVSEL_Pos)
#define GCLK_GENCTRL_RUNSTDBY_Pos   21           /**< \brief (GCLK_GENCTRL) Run in Standby */
#define GCLK_GENCTRL_RUNSTDBY       (0x1u << GCLK_GENCTRL_RUNSTDBY_Pos)
#define GCLK_GENCTRL_MASK           0x003F1F0Fu  /**< \brief (GCLK_GENCTRL) MASK Register */

/* -------- GCLK_GENDIV : (GCLK Offset: 0x8) (R/W 32) Generic Clock Generator Division -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:4;             /*!< bit:  0.. 3  Generic Clock Generator Selection  */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t DIV:16;           /*!< bit:  8..23  Division Factor                    */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_GENDIV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_GENDIV_OFFSET          0x8          /**< \brief (GCLK_GENDIV offset) Generic Clock Generator Division */
#define GCLK_GENDIV_RESETVALUE      0x00000000   /**< \brief (GCLK_GENDIV reset_value) Generic Clock Generator Division */

#define GCLK_GENDIV_ID_Pos          0            /**< \brief (GCLK_GENDIV) Generic Clock Generator Selection */
#define GCLK_GENDIV_ID_Msk          (0xFu << GCLK_GENDIV_ID_Pos)
#define GCLK_GENDIV_ID(value)       ((GCLK_GENDIV_ID_Msk & ((value) << GCLK_GENDIV_ID_Pos)))
#define GCLK_GENDIV_DIV_Pos         8            /**< \brief (GCLK_GENDIV) Division Factor */
#define GCLK_GENDIV_DIV_Msk         (0xFFFFu << GCLK_GENDIV_DIV_Pos)
#define GCLK_GENDIV_DIV(value)      ((GCLK_GENDIV_DIV_Msk & ((value) << GCLK_GENDIV_DIV_Pos)))
#define GCLK_GENDIV_MASK            0x00FFFF0Fu  /**< \brief (GCLK_GENDIV) MASK Register */

/** \brief GCLK hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO GCLK_CTRL_Type            CTRL;        /**< \brief Offset: 0x0 (R/W  8) Control */
  __I  GCLK_STATUS_Type          STATUS;      /**< \brief Offset: 0x1 (R/   8) Status */
  __IO GCLK_CLKCTRL_Type         CLKCTRL;     /**< \brief Offset: 0x2 (R/W 16) Generic Clock Control */
  __IO GCLK_GENCTRL_Type         GENCTRL;     /**< \brief Offset: 0x4 (R/W 32) Generic Clock Generator Control */
  __IO GCLK_GENDIV_Type          GENDIV;      /**< \brief Offset: 0x8 (R/W 32) Generic Clock Generator Division */
} Gclk;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMR21_GCLK_COMPONENT_ */
