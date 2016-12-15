/**
 * \file
 *
 * \brief Component description for CCL
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

#ifndef _SAML21_CCL_COMPONENT_
#define _SAML21_CCL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR CCL */
/* ========================================================================== */
/** \addtogroup SAML21_CCL Configurable Custom Logic */
/*@{*/

#define CCL_U2225
#define REV_CCL                     0x100

/* -------- CCL_CTRL : (CCL Offset: 0x0) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} CCL_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CCL_CTRL_OFFSET             0x0          /**< \brief (CCL_CTRL offset) Control */
#define CCL_CTRL_RESETVALUE         0x00ul       /**< \brief (CCL_CTRL reset_value) Control */

#define CCL_CTRL_SWRST_Pos          0            /**< \brief (CCL_CTRL) Software Reset */
#define CCL_CTRL_SWRST              (0x1ul << CCL_CTRL_SWRST_Pos)
#define CCL_CTRL_ENABLE_Pos         1            /**< \brief (CCL_CTRL) Enable */
#define CCL_CTRL_ENABLE             (0x1ul << CCL_CTRL_ENABLE_Pos)
#define CCL_CTRL_RUNSTDBY_Pos       6            /**< \brief (CCL_CTRL) Run during Standby */
#define CCL_CTRL_RUNSTDBY           (0x1ul << CCL_CTRL_RUNSTDBY_Pos)
#define CCL_CTRL_MASK               0x43ul       /**< \brief (CCL_CTRL) MASK Register */

/* -------- CCL_SEQCTRL : (CCL Offset: 0x4) (R/W  8) SEQ Control x -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SEQSEL:4;         /*!< bit:  0.. 3  Sequential Selection               */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} CCL_SEQCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CCL_SEQCTRL_OFFSET          0x4          /**< \brief (CCL_SEQCTRL offset) SEQ Control x */
#define CCL_SEQCTRL_RESETVALUE      0x00ul       /**< \brief (CCL_SEQCTRL reset_value) SEQ Control x */

#define CCL_SEQCTRL_SEQSEL_Pos      0            /**< \brief (CCL_SEQCTRL) Sequential Selection */
#define CCL_SEQCTRL_SEQSEL_Msk      (0xFul << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL(value)   ((CCL_SEQCTRL_SEQSEL_Msk & ((value) << CCL_SEQCTRL_SEQSEL_Pos)))
#define   CCL_SEQCTRL_SEQSEL_DISABLE_Val  0x0ul  /**< \brief (CCL_SEQCTRL) Sequential logic is disabled */
#define   CCL_SEQCTRL_SEQSEL_DFF_Val      0x1ul  /**< \brief (CCL_SEQCTRL) D flip flop */
#define   CCL_SEQCTRL_SEQSEL_JK_Val       0x2ul  /**< \brief (CCL_SEQCTRL) JK flip flop */
#define   CCL_SEQCTRL_SEQSEL_LATCH_Val    0x3ul  /**< \brief (CCL_SEQCTRL) D latch */
#define   CCL_SEQCTRL_SEQSEL_RS_Val       0x4ul  /**< \brief (CCL_SEQCTRL) RS latch */
#define CCL_SEQCTRL_SEQSEL_DISABLE  (CCL_SEQCTRL_SEQSEL_DISABLE_Val << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_DFF      (CCL_SEQCTRL_SEQSEL_DFF_Val    << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_JK       (CCL_SEQCTRL_SEQSEL_JK_Val     << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_LATCH    (CCL_SEQCTRL_SEQSEL_LATCH_Val  << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_RS       (CCL_SEQCTRL_SEQSEL_RS_Val     << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_MASK            0x0Ful       /**< \brief (CCL_SEQCTRL) MASK Register */

/* -------- CCL_LUTCTRL : (CCL Offset: 0x8) (R/W 32) LUT Control x -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  LUT Enable                         */
    uint32_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint32_t FILTSEL:2;        /*!< bit:  4.. 5  Filter Selection                   */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t EDGESEL:1;        /*!< bit:      7  Edge Selection                     */
    uint32_t INSEL0:4;         /*!< bit:  8..11  Input Selection 0                  */
    uint32_t INSEL1:4;         /*!< bit: 12..15  Input Selection 1                  */
    uint32_t INSEL2:4;         /*!< bit: 16..19  Input Selection 2                  */
    uint32_t INVEI:1;          /*!< bit:     20  Input Event Invert                 */
    uint32_t LUTEI:1;          /*!< bit:     21  Event Input Enable                 */
    uint32_t LUTEO:1;          /*!< bit:     22  Event Output Enable                */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t TRUTH:8;          /*!< bit: 24..31  Truth Value                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CCL_LUTCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CCL_LUTCTRL_OFFSET          0x8          /**< \brief (CCL_LUTCTRL offset) LUT Control x */
#define CCL_LUTCTRL_RESETVALUE      0x00000000ul /**< \brief (CCL_LUTCTRL reset_value) LUT Control x */

#define CCL_LUTCTRL_ENABLE_Pos      1            /**< \brief (CCL_LUTCTRL) LUT Enable */
#define CCL_LUTCTRL_ENABLE          (0x1ul << CCL_LUTCTRL_ENABLE_Pos)
#define CCL_LUTCTRL_FILTSEL_Pos     4            /**< \brief (CCL_LUTCTRL) Filter Selection */
#define CCL_LUTCTRL_FILTSEL_Msk     (0x3ul << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_FILTSEL(value)  ((CCL_LUTCTRL_FILTSEL_Msk & ((value) << CCL_LUTCTRL_FILTSEL_Pos)))
#define   CCL_LUTCTRL_FILTSEL_DISABLE_Val 0x0ul  /**< \brief (CCL_LUTCTRL) Filter disabled */
#define   CCL_LUTCTRL_FILTSEL_SYNCH_Val   0x1ul  /**< \brief (CCL_LUTCTRL) Synchronizer enabled */
#define   CCL_LUTCTRL_FILTSEL_FILTER_Val  0x2ul  /**< \brief (CCL_LUTCTRL) Filter enabled */
#define CCL_LUTCTRL_FILTSEL_DISABLE (CCL_LUTCTRL_FILTSEL_DISABLE_Val << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_FILTSEL_SYNCH   (CCL_LUTCTRL_FILTSEL_SYNCH_Val << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_FILTSEL_FILTER  (CCL_LUTCTRL_FILTSEL_FILTER_Val << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_EDGESEL_Pos     7            /**< \brief (CCL_LUTCTRL) Edge Selection */
#define CCL_LUTCTRL_EDGESEL         (0x1ul << CCL_LUTCTRL_EDGESEL_Pos)
#define CCL_LUTCTRL_INSEL0_Pos      8            /**< \brief (CCL_LUTCTRL) Input Selection 0 */
#define CCL_LUTCTRL_INSEL0_Msk      (0xFul << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0(value)   ((CCL_LUTCTRL_INSEL0_Msk & ((value) << CCL_LUTCTRL_INSEL0_Pos)))
#define   CCL_LUTCTRL_INSEL0_MASK_Val     0x0ul  /**< \brief (CCL_LUTCTRL) Masked input */
#define   CCL_LUTCTRL_INSEL0_FEEDBACK_Val 0x1ul  /**< \brief (CCL_LUTCTRL) Feedback input source */
#define   CCL_LUTCTRL_INSEL0_LINK_Val     0x2ul  /**< \brief (CCL_LUTCTRL) Linked LUT input source */
#define   CCL_LUTCTRL_INSEL0_EVENT_Val    0x3ul  /**< \brief (CCL_LUTCTRL) Event in put source */
#define   CCL_LUTCTRL_INSEL0_IO_Val       0x4ul  /**< \brief (CCL_LUTCTRL) I/O pin input source */
#define   CCL_LUTCTRL_INSEL0_AC_Val       0x5ul  /**< \brief (CCL_LUTCTRL) AC input source */
#define   CCL_LUTCTRL_INSEL0_TC_Val       0x6ul  /**< \brief (CCL_LUTCTRL) TC input source */
#define   CCL_LUTCTRL_INSEL0_ALTTC_Val    0x7ul  /**< \brief (CCL_LUTCTRL) Alternate TC input source */
#define   CCL_LUTCTRL_INSEL0_TCC_Val      0x8ul  /**< \brief (CCL_LUTCTRL) TCC input source */
#define   CCL_LUTCTRL_INSEL0_SERCOM_Val   0x9ul  /**< \brief (CCL_LUTCTRL) SERCOM inout source */
#define CCL_LUTCTRL_INSEL0_MASK     (CCL_LUTCTRL_INSEL0_MASK_Val   << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_FEEDBACK (CCL_LUTCTRL_INSEL0_FEEDBACK_Val << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_LINK     (CCL_LUTCTRL_INSEL0_LINK_Val   << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_EVENT    (CCL_LUTCTRL_INSEL0_EVENT_Val  << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_IO       (CCL_LUTCTRL_INSEL0_IO_Val     << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_AC       (CCL_LUTCTRL_INSEL0_AC_Val     << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_TC       (CCL_LUTCTRL_INSEL0_TC_Val     << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_ALTTC    (CCL_LUTCTRL_INSEL0_ALTTC_Val  << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_TCC      (CCL_LUTCTRL_INSEL0_TCC_Val    << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_SERCOM   (CCL_LUTCTRL_INSEL0_SERCOM_Val << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL1_Pos      12           /**< \brief (CCL_LUTCTRL) Input Selection 1 */
#define CCL_LUTCTRL_INSEL1_Msk      (0xFul << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1(value)   ((CCL_LUTCTRL_INSEL1_Msk & ((value) << CCL_LUTCTRL_INSEL1_Pos)))
#define CCL_LUTCTRL_INSEL2_Pos      16           /**< \brief (CCL_LUTCTRL) Input Selection 2 */
#define CCL_LUTCTRL_INSEL2_Msk      (0xFul << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2(value)   ((CCL_LUTCTRL_INSEL2_Msk & ((value) << CCL_LUTCTRL_INSEL2_Pos)))
#define CCL_LUTCTRL_INVEI_Pos       20           /**< \brief (CCL_LUTCTRL) Input Event Invert */
#define CCL_LUTCTRL_INVEI           (0x1ul << CCL_LUTCTRL_INVEI_Pos)
#define CCL_LUTCTRL_LUTEI_Pos       21           /**< \brief (CCL_LUTCTRL) Event Input Enable */
#define CCL_LUTCTRL_LUTEI           (0x1ul << CCL_LUTCTRL_LUTEI_Pos)
#define CCL_LUTCTRL_LUTEO_Pos       22           /**< \brief (CCL_LUTCTRL) Event Output Enable */
#define CCL_LUTCTRL_LUTEO           (0x1ul << CCL_LUTCTRL_LUTEO_Pos)
#define CCL_LUTCTRL_TRUTH_Pos       24           /**< \brief (CCL_LUTCTRL) Truth Value */
#define CCL_LUTCTRL_TRUTH_Msk       (0xFFul << CCL_LUTCTRL_TRUTH_Pos)
#define CCL_LUTCTRL_TRUTH(value)    ((CCL_LUTCTRL_TRUTH_Msk & ((value) << CCL_LUTCTRL_TRUTH_Pos)))
#define CCL_LUTCTRL_MASK            0xFF7FFFB2ul /**< \brief (CCL_LUTCTRL) MASK Register */

/** \brief CCL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CCL_CTRL_Type             CTRL;        /**< \brief Offset: 0x0 (R/W  8) Control */
       RoReg8                    Reserved1[0x3];
  __IO CCL_SEQCTRL_Type          SEQCTRL[2];  /**< \brief Offset: 0x4 (R/W  8) SEQ Control x */
       RoReg8                    Reserved2[0x2];
  __IO CCL_LUTCTRL_Type          LUTCTRL[4];  /**< \brief Offset: 0x8 (R/W 32) LUT Control x */
} Ccl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAML21_CCL_COMPONENT_ */
