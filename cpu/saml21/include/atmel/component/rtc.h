/**
 * \file
 *
 * \brief Component description for RTC
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

#ifndef _SAML21_RTC_COMPONENT_
#define _SAML21_RTC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR RTC */
/* ========================================================================== */
/** \addtogroup SAML21_RTC Real-Time Counter */
/*@{*/

#define RTC_U2250
#define REV_RTC                     0x110

/* -------- RTC_MODE0_CTRLA : (RTC Offset: 0x00) (R/W 16) MODE0 MODE0 Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint16_t MODE:2;           /*!< bit:  2.. 3  Operating Mode                     */
    uint16_t :3;               /*!< bit:  4.. 6  Reserved                           */
    uint16_t MATCHCLR:1;       /*!< bit:      7  Clear on Match                     */
    uint16_t PRESCALER:4;      /*!< bit:  8..11  Prescaler                          */
    uint16_t :3;               /*!< bit: 12..14  Reserved                           */
    uint16_t SYNCDIS:1;        /*!< bit:     15  Count Read Synchronization Disable */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_CTRLA_OFFSET      0x00         /**< \brief (RTC_MODE0_CTRLA offset) MODE0 Control A */
#define RTC_MODE0_CTRLA_RESETVALUE  0x0000ul     /**< \brief (RTC_MODE0_CTRLA reset_value) MODE0 Control A */

#define RTC_MODE0_CTRLA_SWRST_Pos   0            /**< \brief (RTC_MODE0_CTRLA) Software Reset */
#define RTC_MODE0_CTRLA_SWRST       (0x1ul << RTC_MODE0_CTRLA_SWRST_Pos)
#define RTC_MODE0_CTRLA_ENABLE_Pos  1            /**< \brief (RTC_MODE0_CTRLA) Enable */
#define RTC_MODE0_CTRLA_ENABLE      (0x1ul << RTC_MODE0_CTRLA_ENABLE_Pos)
#define RTC_MODE0_CTRLA_MODE_Pos    2            /**< \brief (RTC_MODE0_CTRLA) Operating Mode */
#define RTC_MODE0_CTRLA_MODE_Msk    (0x3ul << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MODE(value) ((RTC_MODE0_CTRLA_MODE_Msk & ((value) << RTC_MODE0_CTRLA_MODE_Pos)))
#define   RTC_MODE0_CTRLA_MODE_COUNT32_Val 0x0ul  /**< \brief (RTC_MODE0_CTRLA) Mode 0: 32-bit Counter */
#define   RTC_MODE0_CTRLA_MODE_COUNT16_Val 0x1ul  /**< \brief (RTC_MODE0_CTRLA) Mode 1: 16-bit Counter */
#define   RTC_MODE0_CTRLA_MODE_CLOCK_Val  0x2ul  /**< \brief (RTC_MODE0_CTRLA) Mode 2: Clock/Calendar */
#define RTC_MODE0_CTRLA_MODE_COUNT32 (RTC_MODE0_CTRLA_MODE_COUNT32_Val << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MODE_COUNT16 (RTC_MODE0_CTRLA_MODE_COUNT16_Val << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MODE_CLOCK  (RTC_MODE0_CTRLA_MODE_CLOCK_Val << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MATCHCLR_Pos 7            /**< \brief (RTC_MODE0_CTRLA) Clear on Match */
#define RTC_MODE0_CTRLA_MATCHCLR    (0x1ul << RTC_MODE0_CTRLA_MATCHCLR_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_Pos 8            /**< \brief (RTC_MODE0_CTRLA) Prescaler */
#define RTC_MODE0_CTRLA_PRESCALER_Msk (0xFul << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER(value) ((RTC_MODE0_CTRLA_PRESCALER_Msk & ((value) << RTC_MODE0_CTRLA_PRESCALER_Pos)))
#define   RTC_MODE0_CTRLA_PRESCALER_OFF_Val 0x0ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV1_Val 0x1ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV2_Val 0x2ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/2 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV4_Val 0x3ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/4 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV8_Val 0x4ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/8 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV16_Val 0x5ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/16 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV32_Val 0x6ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/32 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV64_Val 0x7ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/64 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV128_Val 0x8ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/128 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV256_Val 0x9ul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/256 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV512_Val 0xAul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/512 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV1024_Val 0xBul  /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/1024 */
#define RTC_MODE0_CTRLA_PRESCALER_OFF (RTC_MODE0_CTRLA_PRESCALER_OFF_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV1 (RTC_MODE0_CTRLA_PRESCALER_DIV1_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV2 (RTC_MODE0_CTRLA_PRESCALER_DIV2_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV4 (RTC_MODE0_CTRLA_PRESCALER_DIV4_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV8 (RTC_MODE0_CTRLA_PRESCALER_DIV8_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV16 (RTC_MODE0_CTRLA_PRESCALER_DIV16_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV32 (RTC_MODE0_CTRLA_PRESCALER_DIV32_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV64 (RTC_MODE0_CTRLA_PRESCALER_DIV64_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV128 (RTC_MODE0_CTRLA_PRESCALER_DIV128_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV256 (RTC_MODE0_CTRLA_PRESCALER_DIV256_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV512 (RTC_MODE0_CTRLA_PRESCALER_DIV512_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV1024 (RTC_MODE0_CTRLA_PRESCALER_DIV1024_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_SYNCDIS_Pos 15           /**< \brief (RTC_MODE0_CTRLA) Count Read Synchronization Disable */
#define RTC_MODE0_CTRLA_SYNCDIS     (0x1ul << RTC_MODE0_CTRLA_SYNCDIS_Pos)
#define RTC_MODE0_CTRLA_MASK        0x8F8Ful     /**< \brief (RTC_MODE0_CTRLA) MASK Register */

/* -------- RTC_MODE1_CTRLA : (RTC Offset: 0x00) (R/W 16) MODE1 MODE1 Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint16_t MODE:2;           /*!< bit:  2.. 3  Operating Mode                     */
    uint16_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint16_t PRESCALER:4;      /*!< bit:  8..11  Prescaler                          */
    uint16_t :3;               /*!< bit: 12..14  Reserved                           */
    uint16_t SYNCDIS:1;        /*!< bit:     15  Count Read Synchronization Disable */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_CTRLA_OFFSET      0x00         /**< \brief (RTC_MODE1_CTRLA offset) MODE1 Control A */
#define RTC_MODE1_CTRLA_RESETVALUE  0x0000ul     /**< \brief (RTC_MODE1_CTRLA reset_value) MODE1 Control A */

#define RTC_MODE1_CTRLA_SWRST_Pos   0            /**< \brief (RTC_MODE1_CTRLA) Software Reset */
#define RTC_MODE1_CTRLA_SWRST       (0x1ul << RTC_MODE1_CTRLA_SWRST_Pos)
#define RTC_MODE1_CTRLA_ENABLE_Pos  1            /**< \brief (RTC_MODE1_CTRLA) Enable */
#define RTC_MODE1_CTRLA_ENABLE      (0x1ul << RTC_MODE1_CTRLA_ENABLE_Pos)
#define RTC_MODE1_CTRLA_MODE_Pos    2            /**< \brief (RTC_MODE1_CTRLA) Operating Mode */
#define RTC_MODE1_CTRLA_MODE_Msk    (0x3ul << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_MODE(value) ((RTC_MODE1_CTRLA_MODE_Msk & ((value) << RTC_MODE1_CTRLA_MODE_Pos)))
#define   RTC_MODE1_CTRLA_MODE_COUNT32_Val 0x0ul  /**< \brief (RTC_MODE1_CTRLA) Mode 0: 32-bit Counter */
#define   RTC_MODE1_CTRLA_MODE_COUNT16_Val 0x1ul  /**< \brief (RTC_MODE1_CTRLA) Mode 1: 16-bit Counter */
#define   RTC_MODE1_CTRLA_MODE_CLOCK_Val  0x2ul  /**< \brief (RTC_MODE1_CTRLA) Mode 2: Clock/Calendar */
#define RTC_MODE1_CTRLA_MODE_COUNT32 (RTC_MODE1_CTRLA_MODE_COUNT32_Val << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_MODE_COUNT16 (RTC_MODE1_CTRLA_MODE_COUNT16_Val << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_MODE_CLOCK  (RTC_MODE1_CTRLA_MODE_CLOCK_Val << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_Pos 8            /**< \brief (RTC_MODE1_CTRLA) Prescaler */
#define RTC_MODE1_CTRLA_PRESCALER_Msk (0xFul << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER(value) ((RTC_MODE1_CTRLA_PRESCALER_Msk & ((value) << RTC_MODE1_CTRLA_PRESCALER_Pos)))
#define   RTC_MODE1_CTRLA_PRESCALER_OFF_Val 0x0ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV1_Val 0x1ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV2_Val 0x2ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/2 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV4_Val 0x3ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/4 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV8_Val 0x4ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/8 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV16_Val 0x5ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/16 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV32_Val 0x6ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/32 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV64_Val 0x7ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/64 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV128_Val 0x8ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/128 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV256_Val 0x9ul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/256 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV512_Val 0xAul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/512 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV1024_Val 0xBul  /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/1024 */
#define RTC_MODE1_CTRLA_PRESCALER_OFF (RTC_MODE1_CTRLA_PRESCALER_OFF_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV1 (RTC_MODE1_CTRLA_PRESCALER_DIV1_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV2 (RTC_MODE1_CTRLA_PRESCALER_DIV2_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV4 (RTC_MODE1_CTRLA_PRESCALER_DIV4_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV8 (RTC_MODE1_CTRLA_PRESCALER_DIV8_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV16 (RTC_MODE1_CTRLA_PRESCALER_DIV16_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV32 (RTC_MODE1_CTRLA_PRESCALER_DIV32_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV64 (RTC_MODE1_CTRLA_PRESCALER_DIV64_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV128 (RTC_MODE1_CTRLA_PRESCALER_DIV128_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV256 (RTC_MODE1_CTRLA_PRESCALER_DIV256_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV512 (RTC_MODE1_CTRLA_PRESCALER_DIV512_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV1024 (RTC_MODE1_CTRLA_PRESCALER_DIV1024_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_SYNCDIS_Pos 15           /**< \brief (RTC_MODE1_CTRLA) Count Read Synchronization Disable */
#define RTC_MODE1_CTRLA_SYNCDIS     (0x1ul << RTC_MODE1_CTRLA_SYNCDIS_Pos)
#define RTC_MODE1_CTRLA_MASK        0x8F0Ful     /**< \brief (RTC_MODE1_CTRLA) MASK Register */

/* -------- RTC_MODE2_CTRLA : (RTC Offset: 0x00) (R/W 16) MODE2 MODE2 Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint16_t MODE:2;           /*!< bit:  2.. 3  Operating Mode                     */
    uint16_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint16_t CLKREP:1;         /*!< bit:      6  Clock Representation               */
    uint16_t MATCHCLR:1;       /*!< bit:      7  Clear on Match                     */
    uint16_t PRESCALER:4;      /*!< bit:  8..11  Prescaler                          */
    uint16_t :3;               /*!< bit: 12..14  Reserved                           */
    uint16_t SYNCDIS:1;        /*!< bit:     15  Clock Read Synchronization Disable */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_CTRLA_OFFSET      0x00         /**< \brief (RTC_MODE2_CTRLA offset) MODE2 Control A */
#define RTC_MODE2_CTRLA_RESETVALUE  0x0000ul     /**< \brief (RTC_MODE2_CTRLA reset_value) MODE2 Control A */

#define RTC_MODE2_CTRLA_SWRST_Pos   0            /**< \brief (RTC_MODE2_CTRLA) Software Reset */
#define RTC_MODE2_CTRLA_SWRST       (0x1ul << RTC_MODE2_CTRLA_SWRST_Pos)
#define RTC_MODE2_CTRLA_ENABLE_Pos  1            /**< \brief (RTC_MODE2_CTRLA) Enable */
#define RTC_MODE2_CTRLA_ENABLE      (0x1ul << RTC_MODE2_CTRLA_ENABLE_Pos)
#define RTC_MODE2_CTRLA_MODE_Pos    2            /**< \brief (RTC_MODE2_CTRLA) Operating Mode */
#define RTC_MODE2_CTRLA_MODE_Msk    (0x3ul << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_MODE(value) ((RTC_MODE2_CTRLA_MODE_Msk & ((value) << RTC_MODE2_CTRLA_MODE_Pos)))
#define   RTC_MODE2_CTRLA_MODE_COUNT32_Val 0x0ul  /**< \brief (RTC_MODE2_CTRLA) Mode 0: 32-bit Counter */
#define   RTC_MODE2_CTRLA_MODE_COUNT16_Val 0x1ul  /**< \brief (RTC_MODE2_CTRLA) Mode 1: 16-bit Counter */
#define   RTC_MODE2_CTRLA_MODE_CLOCK_Val  0x2ul  /**< \brief (RTC_MODE2_CTRLA) Mode 2: Clock/Calendar */
#define RTC_MODE2_CTRLA_MODE_COUNT32 (RTC_MODE2_CTRLA_MODE_COUNT32_Val << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_MODE_COUNT16 (RTC_MODE2_CTRLA_MODE_COUNT16_Val << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_MODE_CLOCK  (RTC_MODE2_CTRLA_MODE_CLOCK_Val << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_CLKREP_Pos  6            /**< \brief (RTC_MODE2_CTRLA) Clock Representation */
#define RTC_MODE2_CTRLA_CLKREP      (0x1ul << RTC_MODE2_CTRLA_CLKREP_Pos)
#define RTC_MODE2_CTRLA_MATCHCLR_Pos 7            /**< \brief (RTC_MODE2_CTRLA) Clear on Match */
#define RTC_MODE2_CTRLA_MATCHCLR    (0x1ul << RTC_MODE2_CTRLA_MATCHCLR_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_Pos 8            /**< \brief (RTC_MODE2_CTRLA) Prescaler */
#define RTC_MODE2_CTRLA_PRESCALER_Msk (0xFul << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER(value) ((RTC_MODE2_CTRLA_PRESCALER_Msk & ((value) << RTC_MODE2_CTRLA_PRESCALER_Pos)))
#define   RTC_MODE2_CTRLA_PRESCALER_OFF_Val 0x0ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV1_Val 0x1ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV2_Val 0x2ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/2 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV4_Val 0x3ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/4 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV8_Val 0x4ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/8 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV16_Val 0x5ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/16 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV32_Val 0x6ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/32 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV64_Val 0x7ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/64 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV128_Val 0x8ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/128 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV256_Val 0x9ul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/256 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV512_Val 0xAul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/512 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV1024_Val 0xBul  /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/1024 */
#define RTC_MODE2_CTRLA_PRESCALER_OFF (RTC_MODE2_CTRLA_PRESCALER_OFF_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV1 (RTC_MODE2_CTRLA_PRESCALER_DIV1_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV2 (RTC_MODE2_CTRLA_PRESCALER_DIV2_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV4 (RTC_MODE2_CTRLA_PRESCALER_DIV4_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV8 (RTC_MODE2_CTRLA_PRESCALER_DIV8_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV16 (RTC_MODE2_CTRLA_PRESCALER_DIV16_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV32 (RTC_MODE2_CTRLA_PRESCALER_DIV32_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV64 (RTC_MODE2_CTRLA_PRESCALER_DIV64_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV128 (RTC_MODE2_CTRLA_PRESCALER_DIV128_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV256 (RTC_MODE2_CTRLA_PRESCALER_DIV256_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV512 (RTC_MODE2_CTRLA_PRESCALER_DIV512_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV1024 (RTC_MODE2_CTRLA_PRESCALER_DIV1024_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_SYNCDIS_Pos 15           /**< \brief (RTC_MODE2_CTRLA) Clock Read Synchronization Disable */
#define RTC_MODE2_CTRLA_SYNCDIS     (0x1ul << RTC_MODE2_CTRLA_SYNCDIS_Pos)
#define RTC_MODE2_CTRLA_MASK        0x8FCFul     /**< \brief (RTC_MODE2_CTRLA) MASK Register */

/* -------- RTC_MODE0_EVCTRL : (RTC Offset: 0x04) (R/W 32) MODE0 MODE0 Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PEREO0:1;         /*!< bit:      0  Periodic Interval 0 Event Output Enable */
    uint32_t PEREO1:1;         /*!< bit:      1  Periodic Interval 1 Event Output Enable */
    uint32_t PEREO2:1;         /*!< bit:      2  Periodic Interval 2 Event Output Enable */
    uint32_t PEREO3:1;         /*!< bit:      3  Periodic Interval 3 Event Output Enable */
    uint32_t PEREO4:1;         /*!< bit:      4  Periodic Interval 4 Event Output Enable */
    uint32_t PEREO5:1;         /*!< bit:      5  Periodic Interval 5 Event Output Enable */
    uint32_t PEREO6:1;         /*!< bit:      6  Periodic Interval 6 Event Output Enable */
    uint32_t PEREO7:1;         /*!< bit:      7  Periodic Interval 7 Event Output Enable */
    uint32_t CMPEO0:1;         /*!< bit:      8  Compare 0 Event Output Enable      */
    uint32_t :6;               /*!< bit:  9..14  Reserved                           */
    uint32_t OVFEO:1;          /*!< bit:     15  Overflow Event Output Enable       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PEREO:8;          /*!< bit:  0.. 7  Periodic Interval x Event Output Enable */
    uint32_t CMPEO:1;          /*!< bit:      8  Compare x Event Output Enable      */
    uint32_t :23;              /*!< bit:  9..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_EVCTRL_OFFSET     0x04         /**< \brief (RTC_MODE0_EVCTRL offset) MODE0 Event Control */
#define RTC_MODE0_EVCTRL_RESETVALUE 0x00000000ul /**< \brief (RTC_MODE0_EVCTRL reset_value) MODE0 Event Control */

#define RTC_MODE0_EVCTRL_PEREO0_Pos 0            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 0 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO0     (1 << RTC_MODE0_EVCTRL_PEREO0_Pos)
#define RTC_MODE0_EVCTRL_PEREO1_Pos 1            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 1 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO1     (1 << RTC_MODE0_EVCTRL_PEREO1_Pos)
#define RTC_MODE0_EVCTRL_PEREO2_Pos 2            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 2 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO2     (1 << RTC_MODE0_EVCTRL_PEREO2_Pos)
#define RTC_MODE0_EVCTRL_PEREO3_Pos 3            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 3 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO3     (1 << RTC_MODE0_EVCTRL_PEREO3_Pos)
#define RTC_MODE0_EVCTRL_PEREO4_Pos 4            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 4 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO4     (1 << RTC_MODE0_EVCTRL_PEREO4_Pos)
#define RTC_MODE0_EVCTRL_PEREO5_Pos 5            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 5 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO5     (1 << RTC_MODE0_EVCTRL_PEREO5_Pos)
#define RTC_MODE0_EVCTRL_PEREO6_Pos 6            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 6 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO6     (1 << RTC_MODE0_EVCTRL_PEREO6_Pos)
#define RTC_MODE0_EVCTRL_PEREO7_Pos 7            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 7 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO7     (1 << RTC_MODE0_EVCTRL_PEREO7_Pos)
#define RTC_MODE0_EVCTRL_PEREO_Pos  0            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval x Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO_Msk  (0xFFul << RTC_MODE0_EVCTRL_PEREO_Pos)
#define RTC_MODE0_EVCTRL_PEREO(value) ((RTC_MODE0_EVCTRL_PEREO_Msk & ((value) << RTC_MODE0_EVCTRL_PEREO_Pos)))
#define RTC_MODE0_EVCTRL_CMPEO0_Pos 8            /**< \brief (RTC_MODE0_EVCTRL) Compare 0 Event Output Enable */
#define RTC_MODE0_EVCTRL_CMPEO0     (1 << RTC_MODE0_EVCTRL_CMPEO0_Pos)
#define RTC_MODE0_EVCTRL_CMPEO_Pos  8            /**< \brief (RTC_MODE0_EVCTRL) Compare x Event Output Enable */
#define RTC_MODE0_EVCTRL_CMPEO_Msk  (0x1ul << RTC_MODE0_EVCTRL_CMPEO_Pos)
#define RTC_MODE0_EVCTRL_CMPEO(value) ((RTC_MODE0_EVCTRL_CMPEO_Msk & ((value) << RTC_MODE0_EVCTRL_CMPEO_Pos)))
#define RTC_MODE0_EVCTRL_OVFEO_Pos  15           /**< \brief (RTC_MODE0_EVCTRL) Overflow Event Output Enable */
#define RTC_MODE0_EVCTRL_OVFEO      (0x1ul << RTC_MODE0_EVCTRL_OVFEO_Pos)
#define RTC_MODE0_EVCTRL_MASK       0x000081FFul /**< \brief (RTC_MODE0_EVCTRL) MASK Register */

/* -------- RTC_MODE1_EVCTRL : (RTC Offset: 0x04) (R/W 32) MODE1 MODE1 Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PEREO0:1;         /*!< bit:      0  Periodic Interval 0 Event Output Enable */
    uint32_t PEREO1:1;         /*!< bit:      1  Periodic Interval 1 Event Output Enable */
    uint32_t PEREO2:1;         /*!< bit:      2  Periodic Interval 2 Event Output Enable */
    uint32_t PEREO3:1;         /*!< bit:      3  Periodic Interval 3 Event Output Enable */
    uint32_t PEREO4:1;         /*!< bit:      4  Periodic Interval 4 Event Output Enable */
    uint32_t PEREO5:1;         /*!< bit:      5  Periodic Interval 5 Event Output Enable */
    uint32_t PEREO6:1;         /*!< bit:      6  Periodic Interval 6 Event Output Enable */
    uint32_t PEREO7:1;         /*!< bit:      7  Periodic Interval 7 Event Output Enable */
    uint32_t CMPEO0:1;         /*!< bit:      8  Compare 0 Event Output Enable      */
    uint32_t CMPEO1:1;         /*!< bit:      9  Compare 1 Event Output Enable      */
    uint32_t :5;               /*!< bit: 10..14  Reserved                           */
    uint32_t OVFEO:1;          /*!< bit:     15  Overflow Event Output Enable       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PEREO:8;          /*!< bit:  0.. 7  Periodic Interval x Event Output Enable */
    uint32_t CMPEO:2;          /*!< bit:  8.. 9  Compare x Event Output Enable      */
    uint32_t :22;              /*!< bit: 10..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_EVCTRL_OFFSET     0x04         /**< \brief (RTC_MODE1_EVCTRL offset) MODE1 Event Control */
#define RTC_MODE1_EVCTRL_RESETVALUE 0x00000000ul /**< \brief (RTC_MODE1_EVCTRL reset_value) MODE1 Event Control */

#define RTC_MODE1_EVCTRL_PEREO0_Pos 0            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 0 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO0     (1 << RTC_MODE1_EVCTRL_PEREO0_Pos)
#define RTC_MODE1_EVCTRL_PEREO1_Pos 1            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 1 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO1     (1 << RTC_MODE1_EVCTRL_PEREO1_Pos)
#define RTC_MODE1_EVCTRL_PEREO2_Pos 2            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 2 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO2     (1 << RTC_MODE1_EVCTRL_PEREO2_Pos)
#define RTC_MODE1_EVCTRL_PEREO3_Pos 3            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 3 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO3     (1 << RTC_MODE1_EVCTRL_PEREO3_Pos)
#define RTC_MODE1_EVCTRL_PEREO4_Pos 4            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 4 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO4     (1 << RTC_MODE1_EVCTRL_PEREO4_Pos)
#define RTC_MODE1_EVCTRL_PEREO5_Pos 5            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 5 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO5     (1 << RTC_MODE1_EVCTRL_PEREO5_Pos)
#define RTC_MODE1_EVCTRL_PEREO6_Pos 6            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 6 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO6     (1 << RTC_MODE1_EVCTRL_PEREO6_Pos)
#define RTC_MODE1_EVCTRL_PEREO7_Pos 7            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 7 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO7     (1 << RTC_MODE1_EVCTRL_PEREO7_Pos)
#define RTC_MODE1_EVCTRL_PEREO_Pos  0            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval x Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO_Msk  (0xFFul << RTC_MODE1_EVCTRL_PEREO_Pos)
#define RTC_MODE1_EVCTRL_PEREO(value) ((RTC_MODE1_EVCTRL_PEREO_Msk & ((value) << RTC_MODE1_EVCTRL_PEREO_Pos)))
#define RTC_MODE1_EVCTRL_CMPEO0_Pos 8            /**< \brief (RTC_MODE1_EVCTRL) Compare 0 Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO0     (1 << RTC_MODE1_EVCTRL_CMPEO0_Pos)
#define RTC_MODE1_EVCTRL_CMPEO1_Pos 9            /**< \brief (RTC_MODE1_EVCTRL) Compare 1 Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO1     (1 << RTC_MODE1_EVCTRL_CMPEO1_Pos)
#define RTC_MODE1_EVCTRL_CMPEO_Pos  8            /**< \brief (RTC_MODE1_EVCTRL) Compare x Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO_Msk  (0x3ul << RTC_MODE1_EVCTRL_CMPEO_Pos)
#define RTC_MODE1_EVCTRL_CMPEO(value) ((RTC_MODE1_EVCTRL_CMPEO_Msk & ((value) << RTC_MODE1_EVCTRL_CMPEO_Pos)))
#define RTC_MODE1_EVCTRL_OVFEO_Pos  15           /**< \brief (RTC_MODE1_EVCTRL) Overflow Event Output Enable */
#define RTC_MODE1_EVCTRL_OVFEO      (0x1ul << RTC_MODE1_EVCTRL_OVFEO_Pos)
#define RTC_MODE1_EVCTRL_MASK       0x000083FFul /**< \brief (RTC_MODE1_EVCTRL) MASK Register */

/* -------- RTC_MODE2_EVCTRL : (RTC Offset: 0x04) (R/W 32) MODE2 MODE2 Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PEREO0:1;         /*!< bit:      0  Periodic Interval 0 Event Output Enable */
    uint32_t PEREO1:1;         /*!< bit:      1  Periodic Interval 1 Event Output Enable */
    uint32_t PEREO2:1;         /*!< bit:      2  Periodic Interval 2 Event Output Enable */
    uint32_t PEREO3:1;         /*!< bit:      3  Periodic Interval 3 Event Output Enable */
    uint32_t PEREO4:1;         /*!< bit:      4  Periodic Interval 4 Event Output Enable */
    uint32_t PEREO5:1;         /*!< bit:      5  Periodic Interval 5 Event Output Enable */
    uint32_t PEREO6:1;         /*!< bit:      6  Periodic Interval 6 Event Output Enable */
    uint32_t PEREO7:1;         /*!< bit:      7  Periodic Interval 7 Event Output Enable */
    uint32_t ALARMEO0:1;       /*!< bit:      8  Alarm 0 Event Output Enable        */
    uint32_t :6;               /*!< bit:  9..14  Reserved                           */
    uint32_t OVFEO:1;          /*!< bit:     15  Overflow Event Output Enable       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PEREO:8;          /*!< bit:  0.. 7  Periodic Interval x Event Output Enable */
    uint32_t ALARMEO:1;        /*!< bit:      8  Alarm x Event Output Enable        */
    uint32_t :23;              /*!< bit:  9..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_EVCTRL_OFFSET     0x04         /**< \brief (RTC_MODE2_EVCTRL offset) MODE2 Event Control */
#define RTC_MODE2_EVCTRL_RESETVALUE 0x00000000ul /**< \brief (RTC_MODE2_EVCTRL reset_value) MODE2 Event Control */

#define RTC_MODE2_EVCTRL_PEREO0_Pos 0            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 0 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO0     (1 << RTC_MODE2_EVCTRL_PEREO0_Pos)
#define RTC_MODE2_EVCTRL_PEREO1_Pos 1            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 1 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO1     (1 << RTC_MODE2_EVCTRL_PEREO1_Pos)
#define RTC_MODE2_EVCTRL_PEREO2_Pos 2            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 2 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO2     (1 << RTC_MODE2_EVCTRL_PEREO2_Pos)
#define RTC_MODE2_EVCTRL_PEREO3_Pos 3            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 3 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO3     (1 << RTC_MODE2_EVCTRL_PEREO3_Pos)
#define RTC_MODE2_EVCTRL_PEREO4_Pos 4            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 4 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO4     (1 << RTC_MODE2_EVCTRL_PEREO4_Pos)
#define RTC_MODE2_EVCTRL_PEREO5_Pos 5            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 5 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO5     (1 << RTC_MODE2_EVCTRL_PEREO5_Pos)
#define RTC_MODE2_EVCTRL_PEREO6_Pos 6            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 6 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO6     (1 << RTC_MODE2_EVCTRL_PEREO6_Pos)
#define RTC_MODE2_EVCTRL_PEREO7_Pos 7            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 7 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO7     (1 << RTC_MODE2_EVCTRL_PEREO7_Pos)
#define RTC_MODE2_EVCTRL_PEREO_Pos  0            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval x Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO_Msk  (0xFFul << RTC_MODE2_EVCTRL_PEREO_Pos)
#define RTC_MODE2_EVCTRL_PEREO(value) ((RTC_MODE2_EVCTRL_PEREO_Msk & ((value) << RTC_MODE2_EVCTRL_PEREO_Pos)))
#define RTC_MODE2_EVCTRL_ALARMEO0_Pos 8            /**< \brief (RTC_MODE2_EVCTRL) Alarm 0 Event Output Enable */
#define RTC_MODE2_EVCTRL_ALARMEO0   (1 << RTC_MODE2_EVCTRL_ALARMEO0_Pos)
#define RTC_MODE2_EVCTRL_ALARMEO_Pos 8            /**< \brief (RTC_MODE2_EVCTRL) Alarm x Event Output Enable */
#define RTC_MODE2_EVCTRL_ALARMEO_Msk (0x1ul << RTC_MODE2_EVCTRL_ALARMEO_Pos)
#define RTC_MODE2_EVCTRL_ALARMEO(value) ((RTC_MODE2_EVCTRL_ALARMEO_Msk & ((value) << RTC_MODE2_EVCTRL_ALARMEO_Pos)))
#define RTC_MODE2_EVCTRL_OVFEO_Pos  15           /**< \brief (RTC_MODE2_EVCTRL) Overflow Event Output Enable */
#define RTC_MODE2_EVCTRL_OVFEO      (0x1ul << RTC_MODE2_EVCTRL_OVFEO_Pos)
#define RTC_MODE2_EVCTRL_MASK       0x000081FFul /**< \brief (RTC_MODE2_EVCTRL) MASK Register */

/* -------- RTC_MODE0_INTENCLR : (RTC Offset: 0x08) (R/W 16) MODE0 MODE0 Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:1;            /*!< bit:      8  Compare x Interrupt Enable         */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_INTENCLR_OFFSET   0x08         /**< \brief (RTC_MODE0_INTENCLR offset) MODE0 Interrupt Enable Clear */
#define RTC_MODE0_INTENCLR_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE0_INTENCLR reset_value) MODE0 Interrupt Enable Clear */

#define RTC_MODE0_INTENCLR_PER0_Pos 0            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER0     (1 << RTC_MODE0_INTENCLR_PER0_Pos)
#define RTC_MODE0_INTENCLR_PER1_Pos 1            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER1     (1 << RTC_MODE0_INTENCLR_PER1_Pos)
#define RTC_MODE0_INTENCLR_PER2_Pos 2            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER2     (1 << RTC_MODE0_INTENCLR_PER2_Pos)
#define RTC_MODE0_INTENCLR_PER3_Pos 3            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER3     (1 << RTC_MODE0_INTENCLR_PER3_Pos)
#define RTC_MODE0_INTENCLR_PER4_Pos 4            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER4     (1 << RTC_MODE0_INTENCLR_PER4_Pos)
#define RTC_MODE0_INTENCLR_PER5_Pos 5            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER5     (1 << RTC_MODE0_INTENCLR_PER5_Pos)
#define RTC_MODE0_INTENCLR_PER6_Pos 6            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER6     (1 << RTC_MODE0_INTENCLR_PER6_Pos)
#define RTC_MODE0_INTENCLR_PER7_Pos 7            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER7     (1 << RTC_MODE0_INTENCLR_PER7_Pos)
#define RTC_MODE0_INTENCLR_PER_Pos  0            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval x Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER_Msk  (0xFFul << RTC_MODE0_INTENCLR_PER_Pos)
#define RTC_MODE0_INTENCLR_PER(value) ((RTC_MODE0_INTENCLR_PER_Msk & ((value) << RTC_MODE0_INTENCLR_PER_Pos)))
#define RTC_MODE0_INTENCLR_CMP0_Pos 8            /**< \brief (RTC_MODE0_INTENCLR) Compare 0 Interrupt Enable */
#define RTC_MODE0_INTENCLR_CMP0     (1 << RTC_MODE0_INTENCLR_CMP0_Pos)
#define RTC_MODE0_INTENCLR_CMP_Pos  8            /**< \brief (RTC_MODE0_INTENCLR) Compare x Interrupt Enable */
#define RTC_MODE0_INTENCLR_CMP_Msk  (0x1ul << RTC_MODE0_INTENCLR_CMP_Pos)
#define RTC_MODE0_INTENCLR_CMP(value) ((RTC_MODE0_INTENCLR_CMP_Msk & ((value) << RTC_MODE0_INTENCLR_CMP_Pos)))
#define RTC_MODE0_INTENCLR_OVF_Pos  15           /**< \brief (RTC_MODE0_INTENCLR) Overflow Interrupt Enable */
#define RTC_MODE0_INTENCLR_OVF      (0x1ul << RTC_MODE0_INTENCLR_OVF_Pos)
#define RTC_MODE0_INTENCLR_MASK     0x81FFul     /**< \brief (RTC_MODE0_INTENCLR) MASK Register */

/* -------- RTC_MODE1_INTENCLR : (RTC Offset: 0x08) (R/W 16) MODE1 MODE1 Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1 Interrupt Enable         */
    uint16_t :5;               /*!< bit: 10..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:2;            /*!< bit:  8.. 9  Compare x Interrupt Enable         */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_INTENCLR_OFFSET   0x08         /**< \brief (RTC_MODE1_INTENCLR offset) MODE1 Interrupt Enable Clear */
#define RTC_MODE1_INTENCLR_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE1_INTENCLR reset_value) MODE1 Interrupt Enable Clear */

#define RTC_MODE1_INTENCLR_PER0_Pos 0            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER0     (1 << RTC_MODE1_INTENCLR_PER0_Pos)
#define RTC_MODE1_INTENCLR_PER1_Pos 1            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER1     (1 << RTC_MODE1_INTENCLR_PER1_Pos)
#define RTC_MODE1_INTENCLR_PER2_Pos 2            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER2     (1 << RTC_MODE1_INTENCLR_PER2_Pos)
#define RTC_MODE1_INTENCLR_PER3_Pos 3            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER3     (1 << RTC_MODE1_INTENCLR_PER3_Pos)
#define RTC_MODE1_INTENCLR_PER4_Pos 4            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER4     (1 << RTC_MODE1_INTENCLR_PER4_Pos)
#define RTC_MODE1_INTENCLR_PER5_Pos 5            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER5     (1 << RTC_MODE1_INTENCLR_PER5_Pos)
#define RTC_MODE1_INTENCLR_PER6_Pos 6            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER6     (1 << RTC_MODE1_INTENCLR_PER6_Pos)
#define RTC_MODE1_INTENCLR_PER7_Pos 7            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER7     (1 << RTC_MODE1_INTENCLR_PER7_Pos)
#define RTC_MODE1_INTENCLR_PER_Pos  0            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval x Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER_Msk  (0xFFul << RTC_MODE1_INTENCLR_PER_Pos)
#define RTC_MODE1_INTENCLR_PER(value) ((RTC_MODE1_INTENCLR_PER_Msk & ((value) << RTC_MODE1_INTENCLR_PER_Pos)))
#define RTC_MODE1_INTENCLR_CMP0_Pos 8            /**< \brief (RTC_MODE1_INTENCLR) Compare 0 Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP0     (1 << RTC_MODE1_INTENCLR_CMP0_Pos)
#define RTC_MODE1_INTENCLR_CMP1_Pos 9            /**< \brief (RTC_MODE1_INTENCLR) Compare 1 Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP1     (1 << RTC_MODE1_INTENCLR_CMP1_Pos)
#define RTC_MODE1_INTENCLR_CMP_Pos  8            /**< \brief (RTC_MODE1_INTENCLR) Compare x Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP_Msk  (0x3ul << RTC_MODE1_INTENCLR_CMP_Pos)
#define RTC_MODE1_INTENCLR_CMP(value) ((RTC_MODE1_INTENCLR_CMP_Msk & ((value) << RTC_MODE1_INTENCLR_CMP_Pos)))
#define RTC_MODE1_INTENCLR_OVF_Pos  15           /**< \brief (RTC_MODE1_INTENCLR) Overflow Interrupt Enable */
#define RTC_MODE1_INTENCLR_OVF      (0x1ul << RTC_MODE1_INTENCLR_OVF_Pos)
#define RTC_MODE1_INTENCLR_MASK     0x83FFul     /**< \brief (RTC_MODE1_INTENCLR) MASK Register */

/* -------- RTC_MODE2_INTENCLR : (RTC Offset: 0x08) (R/W 16) MODE2 MODE2 Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t ALARM0:1;         /*!< bit:      8  Alarm 0 Interrupt Enable           */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t ALARM:1;          /*!< bit:      8  Alarm x Interrupt Enable           */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_INTENCLR_OFFSET   0x08         /**< \brief (RTC_MODE2_INTENCLR offset) MODE2 Interrupt Enable Clear */
#define RTC_MODE2_INTENCLR_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE2_INTENCLR reset_value) MODE2 Interrupt Enable Clear */

#define RTC_MODE2_INTENCLR_PER0_Pos 0            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER0     (1 << RTC_MODE2_INTENCLR_PER0_Pos)
#define RTC_MODE2_INTENCLR_PER1_Pos 1            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER1     (1 << RTC_MODE2_INTENCLR_PER1_Pos)
#define RTC_MODE2_INTENCLR_PER2_Pos 2            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER2     (1 << RTC_MODE2_INTENCLR_PER2_Pos)
#define RTC_MODE2_INTENCLR_PER3_Pos 3            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER3     (1 << RTC_MODE2_INTENCLR_PER3_Pos)
#define RTC_MODE2_INTENCLR_PER4_Pos 4            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER4     (1 << RTC_MODE2_INTENCLR_PER4_Pos)
#define RTC_MODE2_INTENCLR_PER5_Pos 5            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER5     (1 << RTC_MODE2_INTENCLR_PER5_Pos)
#define RTC_MODE2_INTENCLR_PER6_Pos 6            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER6     (1 << RTC_MODE2_INTENCLR_PER6_Pos)
#define RTC_MODE2_INTENCLR_PER7_Pos 7            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER7     (1 << RTC_MODE2_INTENCLR_PER7_Pos)
#define RTC_MODE2_INTENCLR_PER_Pos  0            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval x Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER_Msk  (0xFFul << RTC_MODE2_INTENCLR_PER_Pos)
#define RTC_MODE2_INTENCLR_PER(value) ((RTC_MODE2_INTENCLR_PER_Msk & ((value) << RTC_MODE2_INTENCLR_PER_Pos)))
#define RTC_MODE2_INTENCLR_ALARM0_Pos 8            /**< \brief (RTC_MODE2_INTENCLR) Alarm 0 Interrupt Enable */
#define RTC_MODE2_INTENCLR_ALARM0   (1 << RTC_MODE2_INTENCLR_ALARM0_Pos)
#define RTC_MODE2_INTENCLR_ALARM_Pos 8            /**< \brief (RTC_MODE2_INTENCLR) Alarm x Interrupt Enable */
#define RTC_MODE2_INTENCLR_ALARM_Msk (0x1ul << RTC_MODE2_INTENCLR_ALARM_Pos)
#define RTC_MODE2_INTENCLR_ALARM(value) ((RTC_MODE2_INTENCLR_ALARM_Msk & ((value) << RTC_MODE2_INTENCLR_ALARM_Pos)))
#define RTC_MODE2_INTENCLR_OVF_Pos  15           /**< \brief (RTC_MODE2_INTENCLR) Overflow Interrupt Enable */
#define RTC_MODE2_INTENCLR_OVF      (0x1ul << RTC_MODE2_INTENCLR_OVF_Pos)
#define RTC_MODE2_INTENCLR_MASK     0x81FFul     /**< \brief (RTC_MODE2_INTENCLR) MASK Register */

/* -------- RTC_MODE0_INTENSET : (RTC Offset: 0x0A) (R/W 16) MODE0 MODE0 Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:1;            /*!< bit:      8  Compare x Interrupt Enable         */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_INTENSET_OFFSET   0x0A         /**< \brief (RTC_MODE0_INTENSET offset) MODE0 Interrupt Enable Set */
#define RTC_MODE0_INTENSET_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE0_INTENSET reset_value) MODE0 Interrupt Enable Set */

#define RTC_MODE0_INTENSET_PER0_Pos 0            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER0     (1 << RTC_MODE0_INTENSET_PER0_Pos)
#define RTC_MODE0_INTENSET_PER1_Pos 1            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER1     (1 << RTC_MODE0_INTENSET_PER1_Pos)
#define RTC_MODE0_INTENSET_PER2_Pos 2            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER2     (1 << RTC_MODE0_INTENSET_PER2_Pos)
#define RTC_MODE0_INTENSET_PER3_Pos 3            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER3     (1 << RTC_MODE0_INTENSET_PER3_Pos)
#define RTC_MODE0_INTENSET_PER4_Pos 4            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER4     (1 << RTC_MODE0_INTENSET_PER4_Pos)
#define RTC_MODE0_INTENSET_PER5_Pos 5            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER5     (1 << RTC_MODE0_INTENSET_PER5_Pos)
#define RTC_MODE0_INTENSET_PER6_Pos 6            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER6     (1 << RTC_MODE0_INTENSET_PER6_Pos)
#define RTC_MODE0_INTENSET_PER7_Pos 7            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER7     (1 << RTC_MODE0_INTENSET_PER7_Pos)
#define RTC_MODE0_INTENSET_PER_Pos  0            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval x Interrupt Enable */
#define RTC_MODE0_INTENSET_PER_Msk  (0xFFul << RTC_MODE0_INTENSET_PER_Pos)
#define RTC_MODE0_INTENSET_PER(value) ((RTC_MODE0_INTENSET_PER_Msk & ((value) << RTC_MODE0_INTENSET_PER_Pos)))
#define RTC_MODE0_INTENSET_CMP0_Pos 8            /**< \brief (RTC_MODE0_INTENSET) Compare 0 Interrupt Enable */
#define RTC_MODE0_INTENSET_CMP0     (1 << RTC_MODE0_INTENSET_CMP0_Pos)
#define RTC_MODE0_INTENSET_CMP_Pos  8            /**< \brief (RTC_MODE0_INTENSET) Compare x Interrupt Enable */
#define RTC_MODE0_INTENSET_CMP_Msk  (0x1ul << RTC_MODE0_INTENSET_CMP_Pos)
#define RTC_MODE0_INTENSET_CMP(value) ((RTC_MODE0_INTENSET_CMP_Msk & ((value) << RTC_MODE0_INTENSET_CMP_Pos)))
#define RTC_MODE0_INTENSET_OVF_Pos  15           /**< \brief (RTC_MODE0_INTENSET) Overflow Interrupt Enable */
#define RTC_MODE0_INTENSET_OVF      (0x1ul << RTC_MODE0_INTENSET_OVF_Pos)
#define RTC_MODE0_INTENSET_MASK     0x81FFul     /**< \brief (RTC_MODE0_INTENSET) MASK Register */

/* -------- RTC_MODE1_INTENSET : (RTC Offset: 0x0A) (R/W 16) MODE1 MODE1 Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1 Interrupt Enable         */
    uint16_t :5;               /*!< bit: 10..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:2;            /*!< bit:  8.. 9  Compare x Interrupt Enable         */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_INTENSET_OFFSET   0x0A         /**< \brief (RTC_MODE1_INTENSET offset) MODE1 Interrupt Enable Set */
#define RTC_MODE1_INTENSET_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE1_INTENSET reset_value) MODE1 Interrupt Enable Set */

#define RTC_MODE1_INTENSET_PER0_Pos 0            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER0     (1 << RTC_MODE1_INTENSET_PER0_Pos)
#define RTC_MODE1_INTENSET_PER1_Pos 1            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER1     (1 << RTC_MODE1_INTENSET_PER1_Pos)
#define RTC_MODE1_INTENSET_PER2_Pos 2            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER2     (1 << RTC_MODE1_INTENSET_PER2_Pos)
#define RTC_MODE1_INTENSET_PER3_Pos 3            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER3     (1 << RTC_MODE1_INTENSET_PER3_Pos)
#define RTC_MODE1_INTENSET_PER4_Pos 4            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER4     (1 << RTC_MODE1_INTENSET_PER4_Pos)
#define RTC_MODE1_INTENSET_PER5_Pos 5            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER5     (1 << RTC_MODE1_INTENSET_PER5_Pos)
#define RTC_MODE1_INTENSET_PER6_Pos 6            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER6     (1 << RTC_MODE1_INTENSET_PER6_Pos)
#define RTC_MODE1_INTENSET_PER7_Pos 7            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER7     (1 << RTC_MODE1_INTENSET_PER7_Pos)
#define RTC_MODE1_INTENSET_PER_Pos  0            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval x Interrupt Enable */
#define RTC_MODE1_INTENSET_PER_Msk  (0xFFul << RTC_MODE1_INTENSET_PER_Pos)
#define RTC_MODE1_INTENSET_PER(value) ((RTC_MODE1_INTENSET_PER_Msk & ((value) << RTC_MODE1_INTENSET_PER_Pos)))
#define RTC_MODE1_INTENSET_CMP0_Pos 8            /**< \brief (RTC_MODE1_INTENSET) Compare 0 Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP0     (1 << RTC_MODE1_INTENSET_CMP0_Pos)
#define RTC_MODE1_INTENSET_CMP1_Pos 9            /**< \brief (RTC_MODE1_INTENSET) Compare 1 Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP1     (1 << RTC_MODE1_INTENSET_CMP1_Pos)
#define RTC_MODE1_INTENSET_CMP_Pos  8            /**< \brief (RTC_MODE1_INTENSET) Compare x Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP_Msk  (0x3ul << RTC_MODE1_INTENSET_CMP_Pos)
#define RTC_MODE1_INTENSET_CMP(value) ((RTC_MODE1_INTENSET_CMP_Msk & ((value) << RTC_MODE1_INTENSET_CMP_Pos)))
#define RTC_MODE1_INTENSET_OVF_Pos  15           /**< \brief (RTC_MODE1_INTENSET) Overflow Interrupt Enable */
#define RTC_MODE1_INTENSET_OVF      (0x1ul << RTC_MODE1_INTENSET_OVF_Pos)
#define RTC_MODE1_INTENSET_MASK     0x83FFul     /**< \brief (RTC_MODE1_INTENSET) MASK Register */

/* -------- RTC_MODE2_INTENSET : (RTC Offset: 0x0A) (R/W 16) MODE2 MODE2 Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Enable         */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Enable         */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Enable         */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Enable         */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Enable         */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Enable         */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Enable         */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Enable         */
    uint16_t ALARM0:1;         /*!< bit:      8  Alarm 0 Interrupt Enable           */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Enable         */
    uint16_t ALARM:1;          /*!< bit:      8  Alarm x Interrupt Enable           */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_INTENSET_OFFSET   0x0A         /**< \brief (RTC_MODE2_INTENSET offset) MODE2 Interrupt Enable Set */
#define RTC_MODE2_INTENSET_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE2_INTENSET reset_value) MODE2 Interrupt Enable Set */

#define RTC_MODE2_INTENSET_PER0_Pos 0            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 0 Enable */
#define RTC_MODE2_INTENSET_PER0     (1 << RTC_MODE2_INTENSET_PER0_Pos)
#define RTC_MODE2_INTENSET_PER1_Pos 1            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 1 Enable */
#define RTC_MODE2_INTENSET_PER1     (1 << RTC_MODE2_INTENSET_PER1_Pos)
#define RTC_MODE2_INTENSET_PER2_Pos 2            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 2 Enable */
#define RTC_MODE2_INTENSET_PER2     (1 << RTC_MODE2_INTENSET_PER2_Pos)
#define RTC_MODE2_INTENSET_PER3_Pos 3            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 3 Enable */
#define RTC_MODE2_INTENSET_PER3     (1 << RTC_MODE2_INTENSET_PER3_Pos)
#define RTC_MODE2_INTENSET_PER4_Pos 4            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 4 Enable */
#define RTC_MODE2_INTENSET_PER4     (1 << RTC_MODE2_INTENSET_PER4_Pos)
#define RTC_MODE2_INTENSET_PER5_Pos 5            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 5 Enable */
#define RTC_MODE2_INTENSET_PER5     (1 << RTC_MODE2_INTENSET_PER5_Pos)
#define RTC_MODE2_INTENSET_PER6_Pos 6            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 6 Enable */
#define RTC_MODE2_INTENSET_PER6     (1 << RTC_MODE2_INTENSET_PER6_Pos)
#define RTC_MODE2_INTENSET_PER7_Pos 7            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 7 Enable */
#define RTC_MODE2_INTENSET_PER7     (1 << RTC_MODE2_INTENSET_PER7_Pos)
#define RTC_MODE2_INTENSET_PER_Pos  0            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval x Enable */
#define RTC_MODE2_INTENSET_PER_Msk  (0xFFul << RTC_MODE2_INTENSET_PER_Pos)
#define RTC_MODE2_INTENSET_PER(value) ((RTC_MODE2_INTENSET_PER_Msk & ((value) << RTC_MODE2_INTENSET_PER_Pos)))
#define RTC_MODE2_INTENSET_ALARM0_Pos 8            /**< \brief (RTC_MODE2_INTENSET) Alarm 0 Interrupt Enable */
#define RTC_MODE2_INTENSET_ALARM0   (1 << RTC_MODE2_INTENSET_ALARM0_Pos)
#define RTC_MODE2_INTENSET_ALARM_Pos 8            /**< \brief (RTC_MODE2_INTENSET) Alarm x Interrupt Enable */
#define RTC_MODE2_INTENSET_ALARM_Msk (0x1ul << RTC_MODE2_INTENSET_ALARM_Pos)
#define RTC_MODE2_INTENSET_ALARM(value) ((RTC_MODE2_INTENSET_ALARM_Msk & ((value) << RTC_MODE2_INTENSET_ALARM_Pos)))
#define RTC_MODE2_INTENSET_OVF_Pos  15           /**< \brief (RTC_MODE2_INTENSET) Overflow Interrupt Enable */
#define RTC_MODE2_INTENSET_OVF      (0x1ul << RTC_MODE2_INTENSET_OVF_Pos)
#define RTC_MODE2_INTENSET_MASK     0x81FFul     /**< \brief (RTC_MODE2_INTENSET) MASK Register */

/* -------- RTC_MODE0_INTFLAG : (RTC Offset: 0x0C) (R/W 16) MODE0 MODE0 Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0                */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1                */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2                */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3                */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4                */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5                */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6                */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7                */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0                          */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x                */
    uint16_t CMP:1;            /*!< bit:      8  Compare x                          */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_INTFLAG_OFFSET    0x0C         /**< \brief (RTC_MODE0_INTFLAG offset) MODE0 Interrupt Flag Status and Clear */
#define RTC_MODE0_INTFLAG_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE0_INTFLAG reset_value) MODE0 Interrupt Flag Status and Clear */

#define RTC_MODE0_INTFLAG_PER0_Pos  0            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 0 */
#define RTC_MODE0_INTFLAG_PER0      (1 << RTC_MODE0_INTFLAG_PER0_Pos)
#define RTC_MODE0_INTFLAG_PER1_Pos  1            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 1 */
#define RTC_MODE0_INTFLAG_PER1      (1 << RTC_MODE0_INTFLAG_PER1_Pos)
#define RTC_MODE0_INTFLAG_PER2_Pos  2            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 2 */
#define RTC_MODE0_INTFLAG_PER2      (1 << RTC_MODE0_INTFLAG_PER2_Pos)
#define RTC_MODE0_INTFLAG_PER3_Pos  3            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 3 */
#define RTC_MODE0_INTFLAG_PER3      (1 << RTC_MODE0_INTFLAG_PER3_Pos)
#define RTC_MODE0_INTFLAG_PER4_Pos  4            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 4 */
#define RTC_MODE0_INTFLAG_PER4      (1 << RTC_MODE0_INTFLAG_PER4_Pos)
#define RTC_MODE0_INTFLAG_PER5_Pos  5            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 5 */
#define RTC_MODE0_INTFLAG_PER5      (1 << RTC_MODE0_INTFLAG_PER5_Pos)
#define RTC_MODE0_INTFLAG_PER6_Pos  6            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 6 */
#define RTC_MODE0_INTFLAG_PER6      (1 << RTC_MODE0_INTFLAG_PER6_Pos)
#define RTC_MODE0_INTFLAG_PER7_Pos  7            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 7 */
#define RTC_MODE0_INTFLAG_PER7      (1 << RTC_MODE0_INTFLAG_PER7_Pos)
#define RTC_MODE0_INTFLAG_PER_Pos   0            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval x */
#define RTC_MODE0_INTFLAG_PER_Msk   (0xFFul << RTC_MODE0_INTFLAG_PER_Pos)
#define RTC_MODE0_INTFLAG_PER(value) ((RTC_MODE0_INTFLAG_PER_Msk & ((value) << RTC_MODE0_INTFLAG_PER_Pos)))
#define RTC_MODE0_INTFLAG_CMP0_Pos  8            /**< \brief (RTC_MODE0_INTFLAG) Compare 0 */
#define RTC_MODE0_INTFLAG_CMP0      (1 << RTC_MODE0_INTFLAG_CMP0_Pos)
#define RTC_MODE0_INTFLAG_CMP_Pos   8            /**< \brief (RTC_MODE0_INTFLAG) Compare x */
#define RTC_MODE0_INTFLAG_CMP_Msk   (0x1ul << RTC_MODE0_INTFLAG_CMP_Pos)
#define RTC_MODE0_INTFLAG_CMP(value) ((RTC_MODE0_INTFLAG_CMP_Msk & ((value) << RTC_MODE0_INTFLAG_CMP_Pos)))
#define RTC_MODE0_INTFLAG_OVF_Pos   15           /**< \brief (RTC_MODE0_INTFLAG) Overflow */
#define RTC_MODE0_INTFLAG_OVF       (0x1ul << RTC_MODE0_INTFLAG_OVF_Pos)
#define RTC_MODE0_INTFLAG_MASK      0x81FFul     /**< \brief (RTC_MODE0_INTFLAG) MASK Register */

/* -------- RTC_MODE1_INTFLAG : (RTC Offset: 0x0C) (R/W 16) MODE1 MODE1 Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0                */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1                */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2                */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3                */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4                */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5                */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6                */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7                */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0                          */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1                          */
    uint16_t :5;               /*!< bit: 10..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x                */
    uint16_t CMP:2;            /*!< bit:  8.. 9  Compare x                          */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_INTFLAG_OFFSET    0x0C         /**< \brief (RTC_MODE1_INTFLAG offset) MODE1 Interrupt Flag Status and Clear */
#define RTC_MODE1_INTFLAG_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE1_INTFLAG reset_value) MODE1 Interrupt Flag Status and Clear */

#define RTC_MODE1_INTFLAG_PER0_Pos  0            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 0 */
#define RTC_MODE1_INTFLAG_PER0      (1 << RTC_MODE1_INTFLAG_PER0_Pos)
#define RTC_MODE1_INTFLAG_PER1_Pos  1            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 1 */
#define RTC_MODE1_INTFLAG_PER1      (1 << RTC_MODE1_INTFLAG_PER1_Pos)
#define RTC_MODE1_INTFLAG_PER2_Pos  2            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 2 */
#define RTC_MODE1_INTFLAG_PER2      (1 << RTC_MODE1_INTFLAG_PER2_Pos)
#define RTC_MODE1_INTFLAG_PER3_Pos  3            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 3 */
#define RTC_MODE1_INTFLAG_PER3      (1 << RTC_MODE1_INTFLAG_PER3_Pos)
#define RTC_MODE1_INTFLAG_PER4_Pos  4            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 4 */
#define RTC_MODE1_INTFLAG_PER4      (1 << RTC_MODE1_INTFLAG_PER4_Pos)
#define RTC_MODE1_INTFLAG_PER5_Pos  5            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 5 */
#define RTC_MODE1_INTFLAG_PER5      (1 << RTC_MODE1_INTFLAG_PER5_Pos)
#define RTC_MODE1_INTFLAG_PER6_Pos  6            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 6 */
#define RTC_MODE1_INTFLAG_PER6      (1 << RTC_MODE1_INTFLAG_PER6_Pos)
#define RTC_MODE1_INTFLAG_PER7_Pos  7            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 7 */
#define RTC_MODE1_INTFLAG_PER7      (1 << RTC_MODE1_INTFLAG_PER7_Pos)
#define RTC_MODE1_INTFLAG_PER_Pos   0            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval x */
#define RTC_MODE1_INTFLAG_PER_Msk   (0xFFul << RTC_MODE1_INTFLAG_PER_Pos)
#define RTC_MODE1_INTFLAG_PER(value) ((RTC_MODE1_INTFLAG_PER_Msk & ((value) << RTC_MODE1_INTFLAG_PER_Pos)))
#define RTC_MODE1_INTFLAG_CMP0_Pos  8            /**< \brief (RTC_MODE1_INTFLAG) Compare 0 */
#define RTC_MODE1_INTFLAG_CMP0      (1 << RTC_MODE1_INTFLAG_CMP0_Pos)
#define RTC_MODE1_INTFLAG_CMP1_Pos  9            /**< \brief (RTC_MODE1_INTFLAG) Compare 1 */
#define RTC_MODE1_INTFLAG_CMP1      (1 << RTC_MODE1_INTFLAG_CMP1_Pos)
#define RTC_MODE1_INTFLAG_CMP_Pos   8            /**< \brief (RTC_MODE1_INTFLAG) Compare x */
#define RTC_MODE1_INTFLAG_CMP_Msk   (0x3ul << RTC_MODE1_INTFLAG_CMP_Pos)
#define RTC_MODE1_INTFLAG_CMP(value) ((RTC_MODE1_INTFLAG_CMP_Msk & ((value) << RTC_MODE1_INTFLAG_CMP_Pos)))
#define RTC_MODE1_INTFLAG_OVF_Pos   15           /**< \brief (RTC_MODE1_INTFLAG) Overflow */
#define RTC_MODE1_INTFLAG_OVF       (0x1ul << RTC_MODE1_INTFLAG_OVF_Pos)
#define RTC_MODE1_INTFLAG_MASK      0x83FFul     /**< \brief (RTC_MODE1_INTFLAG) MASK Register */

/* -------- RTC_MODE2_INTFLAG : (RTC Offset: 0x0C) (R/W 16) MODE2 MODE2 Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0                */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1                */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2                */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3                */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4                */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5                */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6                */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7                */
    uint16_t ALARM0:1;         /*!< bit:      8  Alarm 0                            */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t OVF:1;            /*!< bit:     15  Overflow                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x                */
    uint16_t ALARM:1;          /*!< bit:      8  Alarm x                            */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_INTFLAG_OFFSET    0x0C         /**< \brief (RTC_MODE2_INTFLAG offset) MODE2 Interrupt Flag Status and Clear */
#define RTC_MODE2_INTFLAG_RESETVALUE 0x0000ul     /**< \brief (RTC_MODE2_INTFLAG reset_value) MODE2 Interrupt Flag Status and Clear */

#define RTC_MODE2_INTFLAG_PER0_Pos  0            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 0 */
#define RTC_MODE2_INTFLAG_PER0      (1 << RTC_MODE2_INTFLAG_PER0_Pos)
#define RTC_MODE2_INTFLAG_PER1_Pos  1            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 1 */
#define RTC_MODE2_INTFLAG_PER1      (1 << RTC_MODE2_INTFLAG_PER1_Pos)
#define RTC_MODE2_INTFLAG_PER2_Pos  2            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 2 */
#define RTC_MODE2_INTFLAG_PER2      (1 << RTC_MODE2_INTFLAG_PER2_Pos)
#define RTC_MODE2_INTFLAG_PER3_Pos  3            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 3 */
#define RTC_MODE2_INTFLAG_PER3      (1 << RTC_MODE2_INTFLAG_PER3_Pos)
#define RTC_MODE2_INTFLAG_PER4_Pos  4            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 4 */
#define RTC_MODE2_INTFLAG_PER4      (1 << RTC_MODE2_INTFLAG_PER4_Pos)
#define RTC_MODE2_INTFLAG_PER5_Pos  5            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 5 */
#define RTC_MODE2_INTFLAG_PER5      (1 << RTC_MODE2_INTFLAG_PER5_Pos)
#define RTC_MODE2_INTFLAG_PER6_Pos  6            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 6 */
#define RTC_MODE2_INTFLAG_PER6      (1 << RTC_MODE2_INTFLAG_PER6_Pos)
#define RTC_MODE2_INTFLAG_PER7_Pos  7            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 7 */
#define RTC_MODE2_INTFLAG_PER7      (1 << RTC_MODE2_INTFLAG_PER7_Pos)
#define RTC_MODE2_INTFLAG_PER_Pos   0            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval x */
#define RTC_MODE2_INTFLAG_PER_Msk   (0xFFul << RTC_MODE2_INTFLAG_PER_Pos)
#define RTC_MODE2_INTFLAG_PER(value) ((RTC_MODE2_INTFLAG_PER_Msk & ((value) << RTC_MODE2_INTFLAG_PER_Pos)))
#define RTC_MODE2_INTFLAG_ALARM0_Pos 8            /**< \brief (RTC_MODE2_INTFLAG) Alarm 0 */
#define RTC_MODE2_INTFLAG_ALARM0    (1 << RTC_MODE2_INTFLAG_ALARM0_Pos)
#define RTC_MODE2_INTFLAG_ALARM_Pos 8            /**< \brief (RTC_MODE2_INTFLAG) Alarm x */
#define RTC_MODE2_INTFLAG_ALARM_Msk (0x1ul << RTC_MODE2_INTFLAG_ALARM_Pos)
#define RTC_MODE2_INTFLAG_ALARM(value) ((RTC_MODE2_INTFLAG_ALARM_Msk & ((value) << RTC_MODE2_INTFLAG_ALARM_Pos)))
#define RTC_MODE2_INTFLAG_OVF_Pos   15           /**< \brief (RTC_MODE2_INTFLAG) Overflow */
#define RTC_MODE2_INTFLAG_OVF       (0x1ul << RTC_MODE2_INTFLAG_OVF_Pos)
#define RTC_MODE2_INTFLAG_MASK      0x81FFul     /**< \brief (RTC_MODE2_INTFLAG) MASK Register */

/* -------- RTC_DBGCTRL : (RTC Offset: 0x0E) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Run During Debug                   */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RTC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_DBGCTRL_OFFSET          0x0E         /**< \brief (RTC_DBGCTRL offset) Debug Control */
#define RTC_DBGCTRL_RESETVALUE      0x00ul       /**< \brief (RTC_DBGCTRL reset_value) Debug Control */

#define RTC_DBGCTRL_DBGRUN_Pos      0            /**< \brief (RTC_DBGCTRL) Run During Debug */
#define RTC_DBGCTRL_DBGRUN          (0x1ul << RTC_DBGCTRL_DBGRUN_Pos)
#define RTC_DBGCTRL_MASK            0x01ul       /**< \brief (RTC_DBGCTRL) MASK Register */

/* -------- RTC_MODE0_SYNCBUSY : (RTC Offset: 0x10) (R/  32) MODE0 MODE0 Synchronization Busy Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Busy                */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Bit Busy                    */
    uint32_t FREQCORR:1;       /*!< bit:      2  FREQCORR Register Busy             */
    uint32_t COUNT:1;          /*!< bit:      3  COUNT Register Busy                */
    uint32_t :1;               /*!< bit:      4  Reserved                           */
    uint32_t COMP0:1;          /*!< bit:      5  COMP 0 Register Busy               */
    uint32_t :9;               /*!< bit:  6..14  Reserved                           */
    uint32_t SYNCDIS:1;        /*!< bit:     15  Count Read Synchronization Disable Bit Busy */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t COMP:1;           /*!< bit:      5  COMP x Register Busy               */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_SYNCBUSY_OFFSET   0x10         /**< \brief (RTC_MODE0_SYNCBUSY offset) MODE0 Synchronization Busy Status */
#define RTC_MODE0_SYNCBUSY_RESETVALUE 0x00000000ul /**< \brief (RTC_MODE0_SYNCBUSY reset_value) MODE0 Synchronization Busy Status */

#define RTC_MODE0_SYNCBUSY_SWRST_Pos 0            /**< \brief (RTC_MODE0_SYNCBUSY) Software Reset Busy */
#define RTC_MODE0_SYNCBUSY_SWRST    (0x1ul << RTC_MODE0_SYNCBUSY_SWRST_Pos)
#define RTC_MODE0_SYNCBUSY_ENABLE_Pos 1            /**< \brief (RTC_MODE0_SYNCBUSY) Enable Bit Busy */
#define RTC_MODE0_SYNCBUSY_ENABLE   (0x1ul << RTC_MODE0_SYNCBUSY_ENABLE_Pos)
#define RTC_MODE0_SYNCBUSY_FREQCORR_Pos 2            /**< \brief (RTC_MODE0_SYNCBUSY) FREQCORR Register Busy */
#define RTC_MODE0_SYNCBUSY_FREQCORR (0x1ul << RTC_MODE0_SYNCBUSY_FREQCORR_Pos)
#define RTC_MODE0_SYNCBUSY_COUNT_Pos 3            /**< \brief (RTC_MODE0_SYNCBUSY) COUNT Register Busy */
#define RTC_MODE0_SYNCBUSY_COUNT    (0x1ul << RTC_MODE0_SYNCBUSY_COUNT_Pos)
#define RTC_MODE0_SYNCBUSY_COMP0_Pos 5            /**< \brief (RTC_MODE0_SYNCBUSY) COMP 0 Register Busy */
#define RTC_MODE0_SYNCBUSY_COMP0    (1 << RTC_MODE0_SYNCBUSY_COMP0_Pos)
#define RTC_MODE0_SYNCBUSY_COMP_Pos 5            /**< \brief (RTC_MODE0_SYNCBUSY) COMP x Register Busy */
#define RTC_MODE0_SYNCBUSY_COMP_Msk (0x1ul << RTC_MODE0_SYNCBUSY_COMP_Pos)
#define RTC_MODE0_SYNCBUSY_COMP(value) ((RTC_MODE0_SYNCBUSY_COMP_Msk & ((value) << RTC_MODE0_SYNCBUSY_COMP_Pos)))
#define RTC_MODE0_SYNCBUSY_SYNCDIS_Pos 15           /**< \brief (RTC_MODE0_SYNCBUSY) Count Read Synchronization Disable Bit Busy */
#define RTC_MODE0_SYNCBUSY_SYNCDIS  (0x1ul << RTC_MODE0_SYNCBUSY_SYNCDIS_Pos)
#define RTC_MODE0_SYNCBUSY_MASK     0x0000802Ful /**< \brief (RTC_MODE0_SYNCBUSY) MASK Register */

/* -------- RTC_MODE1_SYNCBUSY : (RTC Offset: 0x10) (R/  32) MODE1 MODE1 Synchronization Busy Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Bit Busy            */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Bit Busy                    */
    uint32_t FREQCORR:1;       /*!< bit:      2  FREQCORR Register Busy             */
    uint32_t COUNT:1;          /*!< bit:      3  COUNT Register Busy                */
    uint32_t PER:1;            /*!< bit:      4  PER Register Busy                  */
    uint32_t COMP0:1;          /*!< bit:      5  COMP 0 Register Busy               */
    uint32_t COMP1:1;          /*!< bit:      6  COMP 1 Register Busy               */
    uint32_t :8;               /*!< bit:  7..14  Reserved                           */
    uint32_t SYNCDIS:1;        /*!< bit:     15  Count Read Synchronization Disable Bit Busy */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t COMP:2;           /*!< bit:  5.. 6  COMP x Register Busy               */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_SYNCBUSY_OFFSET   0x10         /**< \brief (RTC_MODE1_SYNCBUSY offset) MODE1 Synchronization Busy Status */
#define RTC_MODE1_SYNCBUSY_RESETVALUE 0x00000000ul /**< \brief (RTC_MODE1_SYNCBUSY reset_value) MODE1 Synchronization Busy Status */

#define RTC_MODE1_SYNCBUSY_SWRST_Pos 0            /**< \brief (RTC_MODE1_SYNCBUSY) Software Reset Bit Busy */
#define RTC_MODE1_SYNCBUSY_SWRST    (0x1ul << RTC_MODE1_SYNCBUSY_SWRST_Pos)
#define RTC_MODE1_SYNCBUSY_ENABLE_Pos 1            /**< \brief (RTC_MODE1_SYNCBUSY) Enable Bit Busy */
#define RTC_MODE1_SYNCBUSY_ENABLE   (0x1ul << RTC_MODE1_SYNCBUSY_ENABLE_Pos)
#define RTC_MODE1_SYNCBUSY_FREQCORR_Pos 2            /**< \brief (RTC_MODE1_SYNCBUSY) FREQCORR Register Busy */
#define RTC_MODE1_SYNCBUSY_FREQCORR (0x1ul << RTC_MODE1_SYNCBUSY_FREQCORR_Pos)
#define RTC_MODE1_SYNCBUSY_COUNT_Pos 3            /**< \brief (RTC_MODE1_SYNCBUSY) COUNT Register Busy */
#define RTC_MODE1_SYNCBUSY_COUNT    (0x1ul << RTC_MODE1_SYNCBUSY_COUNT_Pos)
#define RTC_MODE1_SYNCBUSY_PER_Pos  4            /**< \brief (RTC_MODE1_SYNCBUSY) PER Register Busy */
#define RTC_MODE1_SYNCBUSY_PER      (0x1ul << RTC_MODE1_SYNCBUSY_PER_Pos)
#define RTC_MODE1_SYNCBUSY_COMP0_Pos 5            /**< \brief (RTC_MODE1_SYNCBUSY) COMP 0 Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP0    (1 << RTC_MODE1_SYNCBUSY_COMP0_Pos)
#define RTC_MODE1_SYNCBUSY_COMP1_Pos 6            /**< \brief (RTC_MODE1_SYNCBUSY) COMP 1 Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP1    (1 << RTC_MODE1_SYNCBUSY_COMP1_Pos)
#define RTC_MODE1_SYNCBUSY_COMP_Pos 5            /**< \brief (RTC_MODE1_SYNCBUSY) COMP x Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP_Msk (0x3ul << RTC_MODE1_SYNCBUSY_COMP_Pos)
#define RTC_MODE1_SYNCBUSY_COMP(value) ((RTC_MODE1_SYNCBUSY_COMP_Msk & ((value) << RTC_MODE1_SYNCBUSY_COMP_Pos)))
#define RTC_MODE1_SYNCBUSY_SYNCDIS_Pos 15           /**< \brief (RTC_MODE1_SYNCBUSY) Count Read Synchronization Disable Bit Busy */
#define RTC_MODE1_SYNCBUSY_SYNCDIS  (0x1ul << RTC_MODE1_SYNCBUSY_SYNCDIS_Pos)
#define RTC_MODE1_SYNCBUSY_MASK     0x0000807Ful /**< \brief (RTC_MODE1_SYNCBUSY) MASK Register */

/* -------- RTC_MODE2_SYNCBUSY : (RTC Offset: 0x10) (R/  32) MODE2 MODE2 Synchronization Busy Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Bit Busy            */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Bit Busy                    */
    uint32_t FREQCORR:1;       /*!< bit:      2  FREQCORR Register Busy             */
    uint32_t CLOCK:1;          /*!< bit:      3  CLOCK Register Busy                */
    uint32_t :1;               /*!< bit:      4  Reserved                           */
    uint32_t ALARM0:1;         /*!< bit:      5  ALARM 0 Register Busy              */
    uint32_t :5;               /*!< bit:  6..10  Reserved                           */
    uint32_t MASK0:1;          /*!< bit:     11  MASK 0 Register Busy               */
    uint32_t :3;               /*!< bit: 12..14  Reserved                           */
    uint32_t SYNCDIS:1;        /*!< bit:     15  Clock Read Synchronization Disable Bit Busy */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t ALARM:1;          /*!< bit:      5  ALARM x Register Busy              */
    uint32_t :5;               /*!< bit:  6..10  Reserved                           */
    uint32_t MASK:1;           /*!< bit:     11  MASK x Register Busy               */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_SYNCBUSY_OFFSET   0x10         /**< \brief (RTC_MODE2_SYNCBUSY offset) MODE2 Synchronization Busy Status */
#define RTC_MODE2_SYNCBUSY_RESETVALUE 0x00000000ul /**< \brief (RTC_MODE2_SYNCBUSY reset_value) MODE2 Synchronization Busy Status */

#define RTC_MODE2_SYNCBUSY_SWRST_Pos 0            /**< \brief (RTC_MODE2_SYNCBUSY) Software Reset Bit Busy */
#define RTC_MODE2_SYNCBUSY_SWRST    (0x1ul << RTC_MODE2_SYNCBUSY_SWRST_Pos)
#define RTC_MODE2_SYNCBUSY_ENABLE_Pos 1            /**< \brief (RTC_MODE2_SYNCBUSY) Enable Bit Busy */
#define RTC_MODE2_SYNCBUSY_ENABLE   (0x1ul << RTC_MODE2_SYNCBUSY_ENABLE_Pos)
#define RTC_MODE2_SYNCBUSY_FREQCORR_Pos 2            /**< \brief (RTC_MODE2_SYNCBUSY) FREQCORR Register Busy */
#define RTC_MODE2_SYNCBUSY_FREQCORR (0x1ul << RTC_MODE2_SYNCBUSY_FREQCORR_Pos)
#define RTC_MODE2_SYNCBUSY_CLOCK_Pos 3            /**< \brief (RTC_MODE2_SYNCBUSY) CLOCK Register Busy */
#define RTC_MODE2_SYNCBUSY_CLOCK    (0x1ul << RTC_MODE2_SYNCBUSY_CLOCK_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM0_Pos 5            /**< \brief (RTC_MODE2_SYNCBUSY) ALARM 0 Register Busy */
#define RTC_MODE2_SYNCBUSY_ALARM0   (1 << RTC_MODE2_SYNCBUSY_ALARM0_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM_Pos 5            /**< \brief (RTC_MODE2_SYNCBUSY) ALARM x Register Busy */
#define RTC_MODE2_SYNCBUSY_ALARM_Msk (0x1ul << RTC_MODE2_SYNCBUSY_ALARM_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM(value) ((RTC_MODE2_SYNCBUSY_ALARM_Msk & ((value) << RTC_MODE2_SYNCBUSY_ALARM_Pos)))
#define RTC_MODE2_SYNCBUSY_MASK0_Pos 11           /**< \brief (RTC_MODE2_SYNCBUSY) MASK 0 Register Busy */
#define RTC_MODE2_SYNCBUSY_MASK0    (1 << RTC_MODE2_SYNCBUSY_MASK0_Pos)
#define RTC_MODE2_SYNCBUSY_MASK_Pos 11           /**< \brief (RTC_MODE2_SYNCBUSY) MASK x Register Busy */
#define RTC_MODE2_SYNCBUSY_MASK_Msk (0x1ul << RTC_MODE2_SYNCBUSY_MASK_Pos)
#define RTC_MODE2_SYNCBUSY_MASK(value) ((RTC_MODE2_SYNCBUSY_MASK_Msk & ((value) << RTC_MODE2_SYNCBUSY_MASK_Pos)))
#define RTC_MODE2_SYNCBUSY_SYNCDIS_Pos 15           /**< \brief (RTC_MODE2_SYNCBUSY) Clock Read Synchronization Disable Bit Busy */
#define RTC_MODE2_SYNCBUSY_SYNCDIS  (0x1ul << RTC_MODE2_SYNCBUSY_SYNCDIS_Pos)
#define RTC_MODE2_SYNCBUSY_MASK_    0x0000882Ful /**< \brief (RTC_MODE2_SYNCBUSY) MASK Register */

/* -------- RTC_FREQCORR : (RTC Offset: 0x14) (R/W  8) Frequency Correction -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  VALUE:7;          /*!< bit:  0.. 6  Correction Value                   */
    uint8_t  SIGN:1;           /*!< bit:      7  Correction Sign                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RTC_FREQCORR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_FREQCORR_OFFSET         0x14         /**< \brief (RTC_FREQCORR offset) Frequency Correction */
#define RTC_FREQCORR_RESETVALUE     0x00ul       /**< \brief (RTC_FREQCORR reset_value) Frequency Correction */

#define RTC_FREQCORR_VALUE_Pos      0            /**< \brief (RTC_FREQCORR) Correction Value */
#define RTC_FREQCORR_VALUE_Msk      (0x7Ful << RTC_FREQCORR_VALUE_Pos)
#define RTC_FREQCORR_VALUE(value)   ((RTC_FREQCORR_VALUE_Msk & ((value) << RTC_FREQCORR_VALUE_Pos)))
#define RTC_FREQCORR_SIGN_Pos       7            /**< \brief (RTC_FREQCORR) Correction Sign */
#define RTC_FREQCORR_SIGN           (0x1ul << RTC_FREQCORR_SIGN_Pos)
#define RTC_FREQCORR_MASK           0xFFul       /**< \brief (RTC_FREQCORR) MASK Register */

/* -------- RTC_MODE0_COUNT : (RTC Offset: 0x18) (R/W 32) MODE0 MODE0 Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COUNT:32;         /*!< bit:  0..31  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_COUNT_OFFSET      0x18         /**< \brief (RTC_MODE0_COUNT offset) MODE0 Counter Value */
#define RTC_MODE0_COUNT_RESETVALUE  0x00000000ul /**< \brief (RTC_MODE0_COUNT reset_value) MODE0 Counter Value */

#define RTC_MODE0_COUNT_COUNT_Pos   0            /**< \brief (RTC_MODE0_COUNT) Counter Value */
#define RTC_MODE0_COUNT_COUNT_Msk   (0xFFFFFFFFul << RTC_MODE0_COUNT_COUNT_Pos)
#define RTC_MODE0_COUNT_COUNT(value) ((RTC_MODE0_COUNT_COUNT_Msk & ((value) << RTC_MODE0_COUNT_COUNT_Pos)))
#define RTC_MODE0_COUNT_MASK        0xFFFFFFFFul /**< \brief (RTC_MODE0_COUNT) MASK Register */

/* -------- RTC_MODE1_COUNT : (RTC Offset: 0x18) (R/W 16) MODE1 MODE1 Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t COUNT:16;         /*!< bit:  0..15  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_COUNT_OFFSET      0x18         /**< \brief (RTC_MODE1_COUNT offset) MODE1 Counter Value */
#define RTC_MODE1_COUNT_RESETVALUE  0x0000ul     /**< \brief (RTC_MODE1_COUNT reset_value) MODE1 Counter Value */

#define RTC_MODE1_COUNT_COUNT_Pos   0            /**< \brief (RTC_MODE1_COUNT) Counter Value */
#define RTC_MODE1_COUNT_COUNT_Msk   (0xFFFFul << RTC_MODE1_COUNT_COUNT_Pos)
#define RTC_MODE1_COUNT_COUNT(value) ((RTC_MODE1_COUNT_COUNT_Msk & ((value) << RTC_MODE1_COUNT_COUNT_Pos)))
#define RTC_MODE1_COUNT_MASK        0xFFFFul     /**< \brief (RTC_MODE1_COUNT) MASK Register */

/* -------- RTC_MODE2_CLOCK : (RTC Offset: 0x18) (R/W 32) MODE2 MODE2 Clock Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SECOND:6;         /*!< bit:  0.. 5  Second                             */
    uint32_t MINUTE:6;         /*!< bit:  6..11  Minute                             */
    uint32_t HOUR:5;           /*!< bit: 12..16  Hour                               */
    uint32_t DAY:5;            /*!< bit: 17..21  Day                                */
    uint32_t MONTH:4;          /*!< bit: 22..25  Month                              */
    uint32_t YEAR:6;           /*!< bit: 26..31  Year                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_CLOCK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_CLOCK_OFFSET      0x18         /**< \brief (RTC_MODE2_CLOCK offset) MODE2 Clock Value */
#define RTC_MODE2_CLOCK_RESETVALUE  0x00000000ul /**< \brief (RTC_MODE2_CLOCK reset_value) MODE2 Clock Value */

#define RTC_MODE2_CLOCK_SECOND_Pos  0            /**< \brief (RTC_MODE2_CLOCK) Second */
#define RTC_MODE2_CLOCK_SECOND_Msk  (0x3Ful << RTC_MODE2_CLOCK_SECOND_Pos)
#define RTC_MODE2_CLOCK_SECOND(value) ((RTC_MODE2_CLOCK_SECOND_Msk & ((value) << RTC_MODE2_CLOCK_SECOND_Pos)))
#define RTC_MODE2_CLOCK_MINUTE_Pos  6            /**< \brief (RTC_MODE2_CLOCK) Minute */
#define RTC_MODE2_CLOCK_MINUTE_Msk  (0x3Ful << RTC_MODE2_CLOCK_MINUTE_Pos)
#define RTC_MODE2_CLOCK_MINUTE(value) ((RTC_MODE2_CLOCK_MINUTE_Msk & ((value) << RTC_MODE2_CLOCK_MINUTE_Pos)))
#define RTC_MODE2_CLOCK_HOUR_Pos    12           /**< \brief (RTC_MODE2_CLOCK) Hour */
#define RTC_MODE2_CLOCK_HOUR_Msk    (0x1Ful << RTC_MODE2_CLOCK_HOUR_Pos)
#define RTC_MODE2_CLOCK_HOUR(value) ((RTC_MODE2_CLOCK_HOUR_Msk & ((value) << RTC_MODE2_CLOCK_HOUR_Pos)))
#define RTC_MODE2_CLOCK_DAY_Pos     17           /**< \brief (RTC_MODE2_CLOCK) Day */
#define RTC_MODE2_CLOCK_DAY_Msk     (0x1Ful << RTC_MODE2_CLOCK_DAY_Pos)
#define RTC_MODE2_CLOCK_DAY(value)  ((RTC_MODE2_CLOCK_DAY_Msk & ((value) << RTC_MODE2_CLOCK_DAY_Pos)))
#define RTC_MODE2_CLOCK_MONTH_Pos   22           /**< \brief (RTC_MODE2_CLOCK) Month */
#define RTC_MODE2_CLOCK_MONTH_Msk   (0xFul << RTC_MODE2_CLOCK_MONTH_Pos)
#define RTC_MODE2_CLOCK_MONTH(value) ((RTC_MODE2_CLOCK_MONTH_Msk & ((value) << RTC_MODE2_CLOCK_MONTH_Pos)))
#define RTC_MODE2_CLOCK_YEAR_Pos    26           /**< \brief (RTC_MODE2_CLOCK) Year */
#define RTC_MODE2_CLOCK_YEAR_Msk    (0x3Ful << RTC_MODE2_CLOCK_YEAR_Pos)
#define RTC_MODE2_CLOCK_YEAR(value) ((RTC_MODE2_CLOCK_YEAR_Msk & ((value) << RTC_MODE2_CLOCK_YEAR_Pos)))
#define RTC_MODE2_CLOCK_MASK        0xFFFFFFFFul /**< \brief (RTC_MODE2_CLOCK) MASK Register */

/* -------- RTC_MODE1_PER : (RTC Offset: 0x1C) (R/W 16) MODE1 MODE1 Counter Period -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER:16;           /*!< bit:  0..15  Counter Period                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_PER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_PER_OFFSET        0x1C         /**< \brief (RTC_MODE1_PER offset) MODE1 Counter Period */
#define RTC_MODE1_PER_RESETVALUE    0x0000ul     /**< \brief (RTC_MODE1_PER reset_value) MODE1 Counter Period */

#define RTC_MODE1_PER_PER_Pos       0            /**< \brief (RTC_MODE1_PER) Counter Period */
#define RTC_MODE1_PER_PER_Msk       (0xFFFFul << RTC_MODE1_PER_PER_Pos)
#define RTC_MODE1_PER_PER(value)    ((RTC_MODE1_PER_PER_Msk & ((value) << RTC_MODE1_PER_PER_Pos)))
#define RTC_MODE1_PER_MASK          0xFFFFul     /**< \brief (RTC_MODE1_PER) MASK Register */

/* -------- RTC_MODE0_COMP : (RTC Offset: 0x20) (R/W 32) MODE0 MODE0 Compare n Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COMP:32;          /*!< bit:  0..31  Compare Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_COMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_COMP_OFFSET       0x20         /**< \brief (RTC_MODE0_COMP offset) MODE0 Compare n Value */
#define RTC_MODE0_COMP_RESETVALUE   0x00000000ul /**< \brief (RTC_MODE0_COMP reset_value) MODE0 Compare n Value */

#define RTC_MODE0_COMP_COMP_Pos     0            /**< \brief (RTC_MODE0_COMP) Compare Value */
#define RTC_MODE0_COMP_COMP_Msk     (0xFFFFFFFFul << RTC_MODE0_COMP_COMP_Pos)
#define RTC_MODE0_COMP_COMP(value)  ((RTC_MODE0_COMP_COMP_Msk & ((value) << RTC_MODE0_COMP_COMP_Pos)))
#define RTC_MODE0_COMP_MASK         0xFFFFFFFFul /**< \brief (RTC_MODE0_COMP) MASK Register */

/* -------- RTC_MODE1_COMP : (RTC Offset: 0x20) (R/W 16) MODE1 MODE1 Compare n Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t COMP:16;          /*!< bit:  0..15  Compare Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_COMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_COMP_OFFSET       0x20         /**< \brief (RTC_MODE1_COMP offset) MODE1 Compare n Value */
#define RTC_MODE1_COMP_RESETVALUE   0x0000ul     /**< \brief (RTC_MODE1_COMP reset_value) MODE1 Compare n Value */

#define RTC_MODE1_COMP_COMP_Pos     0            /**< \brief (RTC_MODE1_COMP) Compare Value */
#define RTC_MODE1_COMP_COMP_Msk     (0xFFFFul << RTC_MODE1_COMP_COMP_Pos)
#define RTC_MODE1_COMP_COMP(value)  ((RTC_MODE1_COMP_COMP_Msk & ((value) << RTC_MODE1_COMP_COMP_Pos)))
#define RTC_MODE1_COMP_MASK         0xFFFFul     /**< \brief (RTC_MODE1_COMP) MASK Register */

/* -------- RTC_MODE2_ALARM : (RTC Offset: 0x20) (R/W 32) MODE2 MODE2_ALARM Alarm n Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SECOND:6;         /*!< bit:  0.. 5  Second                             */
    uint32_t MINUTE:6;         /*!< bit:  6..11  Minute                             */
    uint32_t HOUR:5;           /*!< bit: 12..16  Hour                               */
    uint32_t DAY:5;            /*!< bit: 17..21  Day                                */
    uint32_t MONTH:4;          /*!< bit: 22..25  Month                              */
    uint32_t YEAR:6;           /*!< bit: 26..31  Year                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_ALARM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_ALARM_OFFSET      0x20         /**< \brief (RTC_MODE2_ALARM offset) MODE2_ALARM Alarm n Value */
#define RTC_MODE2_ALARM_RESETVALUE  0x00000000ul /**< \brief (RTC_MODE2_ALARM reset_value) MODE2_ALARM Alarm n Value */

#define RTC_MODE2_ALARM_SECOND_Pos  0            /**< \brief (RTC_MODE2_ALARM) Second */
#define RTC_MODE2_ALARM_SECOND_Msk  (0x3Ful << RTC_MODE2_ALARM_SECOND_Pos)
#define RTC_MODE2_ALARM_SECOND(value) ((RTC_MODE2_ALARM_SECOND_Msk & ((value) << RTC_MODE2_ALARM_SECOND_Pos)))
#define RTC_MODE2_ALARM_MINUTE_Pos  6            /**< \brief (RTC_MODE2_ALARM) Minute */
#define RTC_MODE2_ALARM_MINUTE_Msk  (0x3Ful << RTC_MODE2_ALARM_MINUTE_Pos)
#define RTC_MODE2_ALARM_MINUTE(value) ((RTC_MODE2_ALARM_MINUTE_Msk & ((value) << RTC_MODE2_ALARM_MINUTE_Pos)))
#define RTC_MODE2_ALARM_HOUR_Pos    12           /**< \brief (RTC_MODE2_ALARM) Hour */
#define RTC_MODE2_ALARM_HOUR_Msk    (0x1Ful << RTC_MODE2_ALARM_HOUR_Pos)
#define RTC_MODE2_ALARM_HOUR(value) ((RTC_MODE2_ALARM_HOUR_Msk & ((value) << RTC_MODE2_ALARM_HOUR_Pos)))
#define RTC_MODE2_ALARM_DAY_Pos     17           /**< \brief (RTC_MODE2_ALARM) Day */
#define RTC_MODE2_ALARM_DAY_Msk     (0x1Ful << RTC_MODE2_ALARM_DAY_Pos)
#define RTC_MODE2_ALARM_DAY(value)  ((RTC_MODE2_ALARM_DAY_Msk & ((value) << RTC_MODE2_ALARM_DAY_Pos)))
#define RTC_MODE2_ALARM_MONTH_Pos   22           /**< \brief (RTC_MODE2_ALARM) Month */
#define RTC_MODE2_ALARM_MONTH_Msk   (0xFul << RTC_MODE2_ALARM_MONTH_Pos)
#define RTC_MODE2_ALARM_MONTH(value) ((RTC_MODE2_ALARM_MONTH_Msk & ((value) << RTC_MODE2_ALARM_MONTH_Pos)))
#define RTC_MODE2_ALARM_YEAR_Pos    26           /**< \brief (RTC_MODE2_ALARM) Year */
#define RTC_MODE2_ALARM_YEAR_Msk    (0x3Ful << RTC_MODE2_ALARM_YEAR_Pos)
#define RTC_MODE2_ALARM_YEAR(value) ((RTC_MODE2_ALARM_YEAR_Msk & ((value) << RTC_MODE2_ALARM_YEAR_Pos)))
#define RTC_MODE2_ALARM_MASK        0xFFFFFFFFul /**< \brief (RTC_MODE2_ALARM) MASK Register */

/* -------- RTC_MODE2_MASK : (RTC Offset: 0x24) (R/W  8) MODE2 MODE2_ALARM Alarm n Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SEL:3;            /*!< bit:  0.. 2  Alarm Mask Selection               */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RTC_MODE2_MASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_MASK_OFFSET       0x24         /**< \brief (RTC_MODE2_MASK offset) MODE2_ALARM Alarm n Mask */
#define RTC_MODE2_MASK_RESETVALUE   0x00ul       /**< \brief (RTC_MODE2_MASK reset_value) MODE2_ALARM Alarm n Mask */

#define RTC_MODE2_MASK_SEL_Pos      0            /**< \brief (RTC_MODE2_MASK) Alarm Mask Selection */
#define RTC_MODE2_MASK_SEL_Msk      (0x7ul << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL(value)   ((RTC_MODE2_MASK_SEL_Msk & ((value) << RTC_MODE2_MASK_SEL_Pos)))
#define   RTC_MODE2_MASK_SEL_OFF_Val      0x0ul  /**< \brief (RTC_MODE2_MASK) Alarm Disabled */
#define   RTC_MODE2_MASK_SEL_SS_Val       0x1ul  /**< \brief (RTC_MODE2_MASK) Match seconds only */
#define   RTC_MODE2_MASK_SEL_MMSS_Val     0x2ul  /**< \brief (RTC_MODE2_MASK) Match seconds and minutes only */
#define   RTC_MODE2_MASK_SEL_HHMMSS_Val   0x3ul  /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, and hours only */
#define   RTC_MODE2_MASK_SEL_DDHHMMSS_Val 0x4ul  /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, hours, and days only */
#define   RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val 0x5ul  /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, hours, days, and months only */
#define   RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val 0x6ul  /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, hours, days, months, and years */
#define RTC_MODE2_MASK_SEL_OFF      (RTC_MODE2_MASK_SEL_OFF_Val    << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_SS       (RTC_MODE2_MASK_SEL_SS_Val     << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_MMSS     (RTC_MODE2_MASK_SEL_MMSS_Val   << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_HHMMSS   (RTC_MODE2_MASK_SEL_HHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_DDHHMMSS (RTC_MODE2_MASK_SEL_DDHHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_MMDDHHMMSS (RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_YYMMDDHHMMSS (RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_MASK         0x07ul       /**< \brief (RTC_MODE2_MASK) MASK Register */

/* -------- RTC_GP : (RTC Offset: 0x40) (R/W 32) General Purpose -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_GP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_GP_OFFSET               0x40         /**< \brief (RTC_GP offset) General Purpose */
#define RTC_GP_RESETVALUE           0x00000000ul /**< \brief (RTC_GP reset_value) General Purpose */
#define RTC_GP_MASK                 0xFFFFFFFFul /**< \brief (RTC_GP) MASK Register */

/** \brief RtcMode2Alarm hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO RTC_MODE2_ALARM_Type      ALARM;       /**< \brief Offset: 0x00 (R/W 32) MODE2_ALARM Alarm n Value */
  __IO RTC_MODE2_MASK_Type       MASK;        /**< \brief Offset: 0x04 (R/W  8) MODE2_ALARM Alarm n Mask */
       RoReg8                    Reserved1[0x3];
} RtcMode2Alarm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief RTC_MODE0 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 32-bit Counter with Single 32-bit Compare */
  __IO RTC_MODE0_CTRLA_Type      CTRLA;       /**< \brief Offset: 0x00 (R/W 16) MODE0 Control A */
       RoReg8                    Reserved1[0x2];
  __IO RTC_MODE0_EVCTRL_Type     EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) MODE0 Event Control */
  __IO RTC_MODE0_INTENCLR_Type   INTENCLR;    /**< \brief Offset: 0x08 (R/W 16) MODE0 Interrupt Enable Clear */
  __IO RTC_MODE0_INTENSET_Type   INTENSET;    /**< \brief Offset: 0x0A (R/W 16) MODE0 Interrupt Enable Set */
  __IO RTC_MODE0_INTFLAG_Type    INTFLAG;     /**< \brief Offset: 0x0C (R/W 16) MODE0 Interrupt Flag Status and Clear */
  __IO RTC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x0E (R/W  8) Debug Control */
       RoReg8                    Reserved2[0x1];
  __I  RTC_MODE0_SYNCBUSY_Type   SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) MODE0 Synchronization Busy Status */
  __IO RTC_FREQCORR_Type         FREQCORR;    /**< \brief Offset: 0x14 (R/W  8) Frequency Correction */
       RoReg8                    Reserved3[0x3];
  __IO RTC_MODE0_COUNT_Type      COUNT;       /**< \brief Offset: 0x18 (R/W 32) MODE0 Counter Value */
       RoReg8                    Reserved4[0x4];
  __IO RTC_MODE0_COMP_Type       COMP[1];     /**< \brief Offset: 0x20 (R/W 32) MODE0 Compare n Value */
       RoReg8                    Reserved5[0x1C];
  __IO RTC_GP_Type               GP[4];       /**< \brief Offset: 0x40 (R/W 32) General Purpose */
} RtcMode0;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief RTC_MODE1 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 16-bit Counter with Two 16-bit Compares */
  __IO RTC_MODE1_CTRLA_Type      CTRLA;       /**< \brief Offset: 0x00 (R/W 16) MODE1 Control A */
       RoReg8                    Reserved1[0x2];
  __IO RTC_MODE1_EVCTRL_Type     EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) MODE1 Event Control */
  __IO RTC_MODE1_INTENCLR_Type   INTENCLR;    /**< \brief Offset: 0x08 (R/W 16) MODE1 Interrupt Enable Clear */
  __IO RTC_MODE1_INTENSET_Type   INTENSET;    /**< \brief Offset: 0x0A (R/W 16) MODE1 Interrupt Enable Set */
  __IO RTC_MODE1_INTFLAG_Type    INTFLAG;     /**< \brief Offset: 0x0C (R/W 16) MODE1 Interrupt Flag Status and Clear */
  __IO RTC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x0E (R/W  8) Debug Control */
       RoReg8                    Reserved2[0x1];
  __I  RTC_MODE1_SYNCBUSY_Type   SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) MODE1 Synchronization Busy Status */
  __IO RTC_FREQCORR_Type         FREQCORR;    /**< \brief Offset: 0x14 (R/W  8) Frequency Correction */
       RoReg8                    Reserved3[0x3];
  __IO RTC_MODE1_COUNT_Type      COUNT;       /**< \brief Offset: 0x18 (R/W 16) MODE1 Counter Value */
       RoReg8                    Reserved4[0x2];
  __IO RTC_MODE1_PER_Type        PER;         /**< \brief Offset: 0x1C (R/W 16) MODE1 Counter Period */
       RoReg8                    Reserved5[0x2];
  __IO RTC_MODE1_COMP_Type       COMP[2];     /**< \brief Offset: 0x20 (R/W 16) MODE1 Compare n Value */
       RoReg8                    Reserved6[0x1C];
  __IO RTC_GP_Type               GP[4];       /**< \brief Offset: 0x40 (R/W 32) General Purpose */
} RtcMode1;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief RTC_MODE2 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* Clock/Calendar with Alarm */
  __IO RTC_MODE2_CTRLA_Type      CTRLA;       /**< \brief Offset: 0x00 (R/W 16) MODE2 Control A */
       RoReg8                    Reserved1[0x2];
  __IO RTC_MODE2_EVCTRL_Type     EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) MODE2 Event Control */
  __IO RTC_MODE2_INTENCLR_Type   INTENCLR;    /**< \brief Offset: 0x08 (R/W 16) MODE2 Interrupt Enable Clear */
  __IO RTC_MODE2_INTENSET_Type   INTENSET;    /**< \brief Offset: 0x0A (R/W 16) MODE2 Interrupt Enable Set */
  __IO RTC_MODE2_INTFLAG_Type    INTFLAG;     /**< \brief Offset: 0x0C (R/W 16) MODE2 Interrupt Flag Status and Clear */
  __IO RTC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x0E (R/W  8) Debug Control */
       RoReg8                    Reserved2[0x1];
  __I  RTC_MODE2_SYNCBUSY_Type   SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) MODE2 Synchronization Busy Status */
  __IO RTC_FREQCORR_Type         FREQCORR;    /**< \brief Offset: 0x14 (R/W  8) Frequency Correction */
       RoReg8                    Reserved3[0x3];
  __IO RTC_MODE2_CLOCK_Type      CLOCK;       /**< \brief Offset: 0x18 (R/W 32) MODE2 Clock Value */
       RoReg8                    Reserved4[0x4];
       RtcMode2Alarm             Mode2Alarm[1]; /**< \brief Offset: 0x20 RtcMode2Alarm groups [ALARM_NUM] */
       RoReg8                    Reserved5[0x18];
  __IO RTC_GP_Type               GP[4];       /**< \brief Offset: 0x40 (R/W 32) General Purpose */
} RtcMode2;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
       RtcMode0                  MODE0;       /**< \brief Offset: 0x00 32-bit Counter with Single 32-bit Compare */
       RtcMode1                  MODE1;       /**< \brief Offset: 0x00 16-bit Counter with Two 16-bit Compares */
       RtcMode2                  MODE2;       /**< \brief Offset: 0x00 Clock/Calendar with Alarm */
} Rtc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAML21_RTC_COMPONENT_ */
