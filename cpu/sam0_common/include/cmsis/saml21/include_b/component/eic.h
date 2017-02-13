/**
 * \file
 *
 * \brief Component description for EIC
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

#ifndef _SAML21_EIC_COMPONENT_
#define _SAML21_EIC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EIC */
/* ========================================================================== */
/** \addtogroup SAML21_EIC External Interrupt Controller */
/*@{*/

#define EIC_U2254
#define REV_EIC                     0x202

/* -------- EIC_CTRLA : (EIC Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  CKSEL:1;          /*!< bit:      4  Clock Selection                    */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CTRLA_OFFSET            0x00         /**< \brief (EIC_CTRLA offset) Control */
#define EIC_CTRLA_RESETVALUE        0x00ul       /**< \brief (EIC_CTRLA reset_value) Control */

#define EIC_CTRLA_SWRST_Pos         0            /**< \brief (EIC_CTRLA) Software Reset */
#define EIC_CTRLA_SWRST             (0x1ul << EIC_CTRLA_SWRST_Pos)
#define EIC_CTRLA_ENABLE_Pos        1            /**< \brief (EIC_CTRLA) Enable */
#define EIC_CTRLA_ENABLE            (0x1ul << EIC_CTRLA_ENABLE_Pos)
#define EIC_CTRLA_CKSEL_Pos         4            /**< \brief (EIC_CTRLA) Clock Selection */
#define EIC_CTRLA_CKSEL             (0x1ul << EIC_CTRLA_CKSEL_Pos)
#define EIC_CTRLA_MASK              0x13ul       /**< \brief (EIC_CTRLA) MASK Register */

/* -------- EIC_NMICTRL : (EIC Offset: 0x01) (R/W  8) NMI Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NMISENSE:3;       /*!< bit:  0.. 2  NMI Input Sense Configuration      */
    uint8_t  NMIFILTEN:1;      /*!< bit:      3  NMI Filter Enable                  */
    uint8_t  NMIASYNCH:1;      /*!< bit:      4  NMI Asynchronous edge Detection Enable */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_NMICTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMICTRL_OFFSET          0x01         /**< \brief (EIC_NMICTRL offset) NMI Control */
#define EIC_NMICTRL_RESETVALUE      0x00ul       /**< \brief (EIC_NMICTRL reset_value) NMI Control */

#define EIC_NMICTRL_NMISENSE_Pos    0            /**< \brief (EIC_NMICTRL) NMI Input Sense Configuration */
#define EIC_NMICTRL_NMISENSE_Msk    (0x7ul << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE(value) (EIC_NMICTRL_NMISENSE_Msk & ((value) << EIC_NMICTRL_NMISENSE_Pos))
#define   EIC_NMICTRL_NMISENSE_NONE_Val   0x0ul  /**< \brief (EIC_NMICTRL) No detection */
#define   EIC_NMICTRL_NMISENSE_RISE_Val   0x1ul  /**< \brief (EIC_NMICTRL) Rising edge detection */
#define   EIC_NMICTRL_NMISENSE_FALL_Val   0x2ul  /**< \brief (EIC_NMICTRL) Falling edge detection */
#define   EIC_NMICTRL_NMISENSE_BOTH_Val   0x3ul  /**< \brief (EIC_NMICTRL) Both edges detection */
#define   EIC_NMICTRL_NMISENSE_HIGH_Val   0x4ul  /**< \brief (EIC_NMICTRL) High level detection */
#define   EIC_NMICTRL_NMISENSE_LOW_Val    0x5ul  /**< \brief (EIC_NMICTRL) Low level detection */
#define EIC_NMICTRL_NMISENSE_NONE   (EIC_NMICTRL_NMISENSE_NONE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_RISE   (EIC_NMICTRL_NMISENSE_RISE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_FALL   (EIC_NMICTRL_NMISENSE_FALL_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_BOTH   (EIC_NMICTRL_NMISENSE_BOTH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_HIGH   (EIC_NMICTRL_NMISENSE_HIGH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_LOW    (EIC_NMICTRL_NMISENSE_LOW_Val  << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMIFILTEN_Pos   3            /**< \brief (EIC_NMICTRL) NMI Filter Enable */
#define EIC_NMICTRL_NMIFILTEN       (0x1ul << EIC_NMICTRL_NMIFILTEN_Pos)
#define EIC_NMICTRL_NMIASYNCH_Pos   4            /**< \brief (EIC_NMICTRL) NMI Asynchronous edge Detection Enable */
#define EIC_NMICTRL_NMIASYNCH       (0x1ul << EIC_NMICTRL_NMIASYNCH_Pos)
#define EIC_NMICTRL_MASK            0x1Ful       /**< \brief (EIC_NMICTRL) MASK Register */

/* -------- EIC_NMIFLAG : (EIC Offset: 0x02) (R/W 16) NMI Interrupt Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t NMI:1;            /*!< bit:      0  NMI Interrupt Flag                 */
    uint16_t :15;              /*!< bit:  1..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} EIC_NMIFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMIFLAG_OFFSET          0x02         /**< \brief (EIC_NMIFLAG offset) NMI Interrupt Flag */
#define EIC_NMIFLAG_RESETVALUE      0x0000ul     /**< \brief (EIC_NMIFLAG reset_value) NMI Interrupt Flag */

#define EIC_NMIFLAG_NMI_Pos         0            /**< \brief (EIC_NMIFLAG) NMI Interrupt Flag */
#define EIC_NMIFLAG_NMI             (0x1ul << EIC_NMIFLAG_NMI_Pos)
#define EIC_NMIFLAG_MASK            0x0001ul     /**< \brief (EIC_NMIFLAG) MASK Register */

/* -------- EIC_SYNCBUSY : (EIC Offset: 0x04) (R/  32) Syncbusy register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software reset synchronisation     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable synchronisation             */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_SYNCBUSY_OFFSET         0x04         /**< \brief (EIC_SYNCBUSY offset) Syncbusy register */
#define EIC_SYNCBUSY_RESETVALUE     0x00000000ul /**< \brief (EIC_SYNCBUSY reset_value) Syncbusy register */

#define EIC_SYNCBUSY_SWRST_Pos      0            /**< \brief (EIC_SYNCBUSY) Software reset synchronisation */
#define EIC_SYNCBUSY_SWRST          (0x1ul << EIC_SYNCBUSY_SWRST_Pos)
#define EIC_SYNCBUSY_ENABLE_Pos     1            /**< \brief (EIC_SYNCBUSY) Enable synchronisation */
#define EIC_SYNCBUSY_ENABLE         (0x1ul << EIC_SYNCBUSY_ENABLE_Pos)
#define EIC_SYNCBUSY_MASK           0x00000003ul /**< \brief (EIC_SYNCBUSY) MASK Register */

/* -------- EIC_EVCTRL : (EIC Offset: 0x08) (R/W 32) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINTEO:16;      /*!< bit:  0..15  External Interrupt Event Output Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_EVCTRL_OFFSET           0x08         /**< \brief (EIC_EVCTRL offset) Event Control */
#define EIC_EVCTRL_RESETVALUE       0x00000000ul /**< \brief (EIC_EVCTRL reset_value) Event Control */

#define EIC_EVCTRL_EXTINTEO_Pos     0            /**< \brief (EIC_EVCTRL) External Interrupt Event Output Enable */
#define EIC_EVCTRL_EXTINTEO_Msk     (0xFFFFul << EIC_EVCTRL_EXTINTEO_Pos)
#define EIC_EVCTRL_EXTINTEO(value)  (EIC_EVCTRL_EXTINTEO_Msk & ((value) << EIC_EVCTRL_EXTINTEO_Pos))
#define EIC_EVCTRL_MASK             0x0000FFFFul /**< \brief (EIC_EVCTRL) MASK Register */

/* -------- EIC_INTENCLR : (EIC Offset: 0x0C) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt Disable         */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENCLR_OFFSET         0x0C         /**< \brief (EIC_INTENCLR offset) Interrupt Enable Clear */
#define EIC_INTENCLR_RESETVALUE     0x00000000ul /**< \brief (EIC_INTENCLR reset_value) Interrupt Enable Clear */

#define EIC_INTENCLR_EXTINT_Pos     0            /**< \brief (EIC_INTENCLR) External Interrupt Disable */
#define EIC_INTENCLR_EXTINT_Msk     (0xFFFFul << EIC_INTENCLR_EXTINT_Pos)
#define EIC_INTENCLR_EXTINT(value)  (EIC_INTENCLR_EXTINT_Msk & ((value) << EIC_INTENCLR_EXTINT_Pos))
#define EIC_INTENCLR_MASK           0x0000FFFFul /**< \brief (EIC_INTENCLR) MASK Register */

/* -------- EIC_INTENSET : (EIC Offset: 0x10) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt Disable         */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENSET_OFFSET         0x10         /**< \brief (EIC_INTENSET offset) Interrupt Enable Set */
#define EIC_INTENSET_RESETVALUE     0x00000000ul /**< \brief (EIC_INTENSET reset_value) Interrupt Enable Set */

#define EIC_INTENSET_EXTINT_Pos     0            /**< \brief (EIC_INTENSET) External Interrupt Disable */
#define EIC_INTENSET_EXTINT_Msk     (0xFFFFul << EIC_INTENSET_EXTINT_Pos)
#define EIC_INTENSET_EXTINT(value)  (EIC_INTENSET_EXTINT_Msk & ((value) << EIC_INTENSET_EXTINT_Pos))
#define EIC_INTENSET_MASK           0x0000FFFFul /**< \brief (EIC_INTENSET) MASK Register */

/* -------- EIC_INTFLAG : (EIC Offset: 0x14) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt Flag            */
    __I uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTFLAG_OFFSET          0x14         /**< \brief (EIC_INTFLAG offset) Interrupt Flag Status and Clear */
#define EIC_INTFLAG_RESETVALUE      0x00000000ul /**< \brief (EIC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define EIC_INTFLAG_EXTINT_Pos      0            /**< \brief (EIC_INTFLAG) External Interrupt Flag */
#define EIC_INTFLAG_EXTINT_Msk      (0xFFFFul << EIC_INTFLAG_EXTINT_Pos)
#define EIC_INTFLAG_EXTINT(value)   (EIC_INTFLAG_EXTINT_Msk & ((value) << EIC_INTFLAG_EXTINT_Pos))
#define EIC_INTFLAG_MASK            0x0000FFFFul /**< \brief (EIC_INTFLAG) MASK Register */

/* -------- EIC_ASYNCH : (EIC Offset: 0x18) (R/W 32) EIC Asynchronous edge Detection Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ASYNCH:16;        /*!< bit:  0..15  EIC Asynchronous edge Detection Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_ASYNCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_ASYNCH_OFFSET           0x18         /**< \brief (EIC_ASYNCH offset) EIC Asynchronous edge Detection Enable */
#define EIC_ASYNCH_RESETVALUE       0x00000000ul /**< \brief (EIC_ASYNCH reset_value) EIC Asynchronous edge Detection Enable */

#define EIC_ASYNCH_ASYNCH_Pos       0            /**< \brief (EIC_ASYNCH) EIC Asynchronous edge Detection Enable */
#define EIC_ASYNCH_ASYNCH_Msk       (0xFFFFul << EIC_ASYNCH_ASYNCH_Pos)
#define EIC_ASYNCH_ASYNCH(value)    (EIC_ASYNCH_ASYNCH_Msk & ((value) << EIC_ASYNCH_ASYNCH_Pos))
#define EIC_ASYNCH_MASK             0x0000FFFFul /**< \brief (EIC_ASYNCH) MASK Register */

/* -------- EIC_CONFIG : (EIC Offset: 0x1C) (R/W 32) Configuration n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SENSE0:3;         /*!< bit:  0.. 2  Input Sense Configuration 0        */
    uint32_t FILTEN0:1;        /*!< bit:      3  Filter Enable 0                    */
    uint32_t SENSE1:3;         /*!< bit:  4.. 6  Input Sense Configuration 1        */
    uint32_t FILTEN1:1;        /*!< bit:      7  Filter Enable 1                    */
    uint32_t SENSE2:3;         /*!< bit:  8..10  Input Sense Configuration 2        */
    uint32_t FILTEN2:1;        /*!< bit:     11  Filter Enable 2                    */
    uint32_t SENSE3:3;         /*!< bit: 12..14  Input Sense Configuration 3        */
    uint32_t FILTEN3:1;        /*!< bit:     15  Filter Enable 3                    */
    uint32_t SENSE4:3;         /*!< bit: 16..18  Input Sense Configuration 4        */
    uint32_t FILTEN4:1;        /*!< bit:     19  Filter Enable 4                    */
    uint32_t SENSE5:3;         /*!< bit: 20..22  Input Sense Configuration 5        */
    uint32_t FILTEN5:1;        /*!< bit:     23  Filter Enable 5                    */
    uint32_t SENSE6:3;         /*!< bit: 24..26  Input Sense Configuration 6        */
    uint32_t FILTEN6:1;        /*!< bit:     27  Filter Enable 6                    */
    uint32_t SENSE7:3;         /*!< bit: 28..30  Input Sense Configuration 7        */
    uint32_t FILTEN7:1;        /*!< bit:     31  Filter Enable 7                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_CONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CONFIG_OFFSET           0x1C         /**< \brief (EIC_CONFIG offset) Configuration n */
#define EIC_CONFIG_RESETVALUE       0x00000000ul /**< \brief (EIC_CONFIG reset_value) Configuration n */

#define EIC_CONFIG_SENSE0_Pos       0            /**< \brief (EIC_CONFIG) Input Sense Configuration 0 */
#define EIC_CONFIG_SENSE0_Msk       (0x7ul << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0(value)    (EIC_CONFIG_SENSE0_Msk & ((value) << EIC_CONFIG_SENSE0_Pos))
#define   EIC_CONFIG_SENSE0_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE0_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE0_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE0_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE0_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE0_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE0_NONE      (EIC_CONFIG_SENSE0_NONE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_RISE      (EIC_CONFIG_SENSE0_RISE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_FALL      (EIC_CONFIG_SENSE0_FALL_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_BOTH      (EIC_CONFIG_SENSE0_BOTH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_HIGH      (EIC_CONFIG_SENSE0_HIGH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_LOW       (EIC_CONFIG_SENSE0_LOW_Val     << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_FILTEN0_Pos      3            /**< \brief (EIC_CONFIG) Filter Enable 0 */
#define EIC_CONFIG_FILTEN0          (0x1ul << EIC_CONFIG_FILTEN0_Pos)
#define EIC_CONFIG_SENSE1_Pos       4            /**< \brief (EIC_CONFIG) Input Sense Configuration 1 */
#define EIC_CONFIG_SENSE1_Msk       (0x7ul << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1(value)    (EIC_CONFIG_SENSE1_Msk & ((value) << EIC_CONFIG_SENSE1_Pos))
#define   EIC_CONFIG_SENSE1_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE1_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE1_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE1_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE1_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE1_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE1_NONE      (EIC_CONFIG_SENSE1_NONE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_RISE      (EIC_CONFIG_SENSE1_RISE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_FALL      (EIC_CONFIG_SENSE1_FALL_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_BOTH      (EIC_CONFIG_SENSE1_BOTH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_HIGH      (EIC_CONFIG_SENSE1_HIGH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_LOW       (EIC_CONFIG_SENSE1_LOW_Val     << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_FILTEN1_Pos      7            /**< \brief (EIC_CONFIG) Filter Enable 1 */
#define EIC_CONFIG_FILTEN1          (0x1ul << EIC_CONFIG_FILTEN1_Pos)
#define EIC_CONFIG_SENSE2_Pos       8            /**< \brief (EIC_CONFIG) Input Sense Configuration 2 */
#define EIC_CONFIG_SENSE2_Msk       (0x7ul << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2(value)    (EIC_CONFIG_SENSE2_Msk & ((value) << EIC_CONFIG_SENSE2_Pos))
#define   EIC_CONFIG_SENSE2_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE2_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE2_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE2_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE2_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE2_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE2_NONE      (EIC_CONFIG_SENSE2_NONE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_RISE      (EIC_CONFIG_SENSE2_RISE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_FALL      (EIC_CONFIG_SENSE2_FALL_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_BOTH      (EIC_CONFIG_SENSE2_BOTH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_HIGH      (EIC_CONFIG_SENSE2_HIGH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_LOW       (EIC_CONFIG_SENSE2_LOW_Val     << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_FILTEN2_Pos      11           /**< \brief (EIC_CONFIG) Filter Enable 2 */
#define EIC_CONFIG_FILTEN2          (0x1ul << EIC_CONFIG_FILTEN2_Pos)
#define EIC_CONFIG_SENSE3_Pos       12           /**< \brief (EIC_CONFIG) Input Sense Configuration 3 */
#define EIC_CONFIG_SENSE3_Msk       (0x7ul << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3(value)    (EIC_CONFIG_SENSE3_Msk & ((value) << EIC_CONFIG_SENSE3_Pos))
#define   EIC_CONFIG_SENSE3_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE3_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE3_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE3_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE3_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE3_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE3_NONE      (EIC_CONFIG_SENSE3_NONE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_RISE      (EIC_CONFIG_SENSE3_RISE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_FALL      (EIC_CONFIG_SENSE3_FALL_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_BOTH      (EIC_CONFIG_SENSE3_BOTH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_HIGH      (EIC_CONFIG_SENSE3_HIGH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_LOW       (EIC_CONFIG_SENSE3_LOW_Val     << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_FILTEN3_Pos      15           /**< \brief (EIC_CONFIG) Filter Enable 3 */
#define EIC_CONFIG_FILTEN3          (0x1ul << EIC_CONFIG_FILTEN3_Pos)
#define EIC_CONFIG_SENSE4_Pos       16           /**< \brief (EIC_CONFIG) Input Sense Configuration 4 */
#define EIC_CONFIG_SENSE4_Msk       (0x7ul << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4(value)    (EIC_CONFIG_SENSE4_Msk & ((value) << EIC_CONFIG_SENSE4_Pos))
#define   EIC_CONFIG_SENSE4_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE4_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE4_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE4_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE4_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE4_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE4_NONE      (EIC_CONFIG_SENSE4_NONE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_RISE      (EIC_CONFIG_SENSE4_RISE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_FALL      (EIC_CONFIG_SENSE4_FALL_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_BOTH      (EIC_CONFIG_SENSE4_BOTH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_HIGH      (EIC_CONFIG_SENSE4_HIGH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_LOW       (EIC_CONFIG_SENSE4_LOW_Val     << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_FILTEN4_Pos      19           /**< \brief (EIC_CONFIG) Filter Enable 4 */
#define EIC_CONFIG_FILTEN4          (0x1ul << EIC_CONFIG_FILTEN4_Pos)
#define EIC_CONFIG_SENSE5_Pos       20           /**< \brief (EIC_CONFIG) Input Sense Configuration 5 */
#define EIC_CONFIG_SENSE5_Msk       (0x7ul << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5(value)    (EIC_CONFIG_SENSE5_Msk & ((value) << EIC_CONFIG_SENSE5_Pos))
#define   EIC_CONFIG_SENSE5_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE5_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE5_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE5_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE5_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE5_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE5_NONE      (EIC_CONFIG_SENSE5_NONE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_RISE      (EIC_CONFIG_SENSE5_RISE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_FALL      (EIC_CONFIG_SENSE5_FALL_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_BOTH      (EIC_CONFIG_SENSE5_BOTH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_HIGH      (EIC_CONFIG_SENSE5_HIGH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_LOW       (EIC_CONFIG_SENSE5_LOW_Val     << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_FILTEN5_Pos      23           /**< \brief (EIC_CONFIG) Filter Enable 5 */
#define EIC_CONFIG_FILTEN5          (0x1ul << EIC_CONFIG_FILTEN5_Pos)
#define EIC_CONFIG_SENSE6_Pos       24           /**< \brief (EIC_CONFIG) Input Sense Configuration 6 */
#define EIC_CONFIG_SENSE6_Msk       (0x7ul << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6(value)    (EIC_CONFIG_SENSE6_Msk & ((value) << EIC_CONFIG_SENSE6_Pos))
#define   EIC_CONFIG_SENSE6_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE6_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE6_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE6_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE6_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE6_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE6_NONE      (EIC_CONFIG_SENSE6_NONE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_RISE      (EIC_CONFIG_SENSE6_RISE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_FALL      (EIC_CONFIG_SENSE6_FALL_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_BOTH      (EIC_CONFIG_SENSE6_BOTH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_HIGH      (EIC_CONFIG_SENSE6_HIGH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_LOW       (EIC_CONFIG_SENSE6_LOW_Val     << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_FILTEN6_Pos      27           /**< \brief (EIC_CONFIG) Filter Enable 6 */
#define EIC_CONFIG_FILTEN6          (0x1ul << EIC_CONFIG_FILTEN6_Pos)
#define EIC_CONFIG_SENSE7_Pos       28           /**< \brief (EIC_CONFIG) Input Sense Configuration 7 */
#define EIC_CONFIG_SENSE7_Msk       (0x7ul << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7(value)    (EIC_CONFIG_SENSE7_Msk & ((value) << EIC_CONFIG_SENSE7_Pos))
#define   EIC_CONFIG_SENSE7_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE7_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE7_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE7_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE7_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE7_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE7_NONE      (EIC_CONFIG_SENSE7_NONE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_RISE      (EIC_CONFIG_SENSE7_RISE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_FALL      (EIC_CONFIG_SENSE7_FALL_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_BOTH      (EIC_CONFIG_SENSE7_BOTH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_HIGH      (EIC_CONFIG_SENSE7_HIGH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_LOW       (EIC_CONFIG_SENSE7_LOW_Val     << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_FILTEN7_Pos      31           /**< \brief (EIC_CONFIG) Filter Enable 7 */
#define EIC_CONFIG_FILTEN7          (0x1ul << EIC_CONFIG_FILTEN7_Pos)
#define EIC_CONFIG_MASK             0xFFFFFFFFul /**< \brief (EIC_CONFIG) MASK Register */

/** \brief EIC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO EIC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control */
  __IO EIC_NMICTRL_Type          NMICTRL;     /**< \brief Offset: 0x01 (R/W  8) NMI Control */
  __IO EIC_NMIFLAG_Type          NMIFLAG;     /**< \brief Offset: 0x02 (R/W 16) NMI Interrupt Flag */
  __I  EIC_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x04 (R/  32) Syncbusy register */
  __IO EIC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x08 (R/W 32) Event Control */
  __IO EIC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x0C (R/W 32) Interrupt Enable Clear */
  __IO EIC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x10 (R/W 32) Interrupt Enable Set */
  __IO EIC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x14 (R/W 32) Interrupt Flag Status and Clear */
  __IO EIC_ASYNCH_Type           ASYNCH;      /**< \brief Offset: 0x18 (R/W 32) EIC Asynchronous edge Detection Enable */
  __IO EIC_CONFIG_Type           CONFIG[2];   /**< \brief Offset: 0x1C (R/W 32) Configuration n */
} Eic;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAML21_EIC_COMPONENT_ */
