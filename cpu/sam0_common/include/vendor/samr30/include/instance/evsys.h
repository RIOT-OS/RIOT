/**
 * \file
 *
 * \brief Instance description for EVSYS
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

#ifndef _SAMR30_EVSYS_INSTANCE_
#define _SAMR30_EVSYS_INSTANCE_

/* ========== Register definition for EVSYS peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EVSYS_CTRLA            (0x43000000U) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHSTATUS         (0x4300000CU) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (0x43000010U) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (0x43000014U) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (0x43000018U) /**< \brief (EVSYS) Interrupt Flag Status and Clear */
#define REG_EVSYS_SWEVT            (0x4300001CU) /**< \brief (EVSYS) Software Event */
#define REG_EVSYS_CHANNEL0         (0x43000020U) /**< \brief (EVSYS) Channel 0 */
#define REG_EVSYS_CHANNEL1         (0x43000024U) /**< \brief (EVSYS) Channel 1 */
#define REG_EVSYS_CHANNEL2         (0x43000028U) /**< \brief (EVSYS) Channel 2 */
#define REG_EVSYS_CHANNEL3         (0x4300002CU) /**< \brief (EVSYS) Channel 3 */
#define REG_EVSYS_CHANNEL4         (0x43000030U) /**< \brief (EVSYS) Channel 4 */
#define REG_EVSYS_CHANNEL5         (0x43000034U) /**< \brief (EVSYS) Channel 5 */
#define REG_EVSYS_CHANNEL6         (0x43000038U) /**< \brief (EVSYS) Channel 6 */
#define REG_EVSYS_CHANNEL7         (0x4300003CU) /**< \brief (EVSYS) Channel 7 */
#define REG_EVSYS_CHANNEL8         (0x43000040U) /**< \brief (EVSYS) Channel 8 */
#define REG_EVSYS_CHANNEL9         (0x43000044U) /**< \brief (EVSYS) Channel 9 */
#define REG_EVSYS_CHANNEL10        (0x43000048U) /**< \brief (EVSYS) Channel 10 */
#define REG_EVSYS_CHANNEL11        (0x4300004CU) /**< \brief (EVSYS) Channel 11 */
#define REG_EVSYS_USER0            (0x43000080U) /**< \brief (EVSYS) User Multiplexer 0 */
#define REG_EVSYS_USER1            (0x43000084U) /**< \brief (EVSYS) User Multiplexer 1 */
#define REG_EVSYS_USER2            (0x43000088U) /**< \brief (EVSYS) User Multiplexer 2 */
#define REG_EVSYS_USER3            (0x4300008CU) /**< \brief (EVSYS) User Multiplexer 3 */
#define REG_EVSYS_USER4            (0x43000090U) /**< \brief (EVSYS) User Multiplexer 4 */
#define REG_EVSYS_USER5            (0x43000094U) /**< \brief (EVSYS) User Multiplexer 5 */
#define REG_EVSYS_USER6            (0x43000098U) /**< \brief (EVSYS) User Multiplexer 6 */
#define REG_EVSYS_USER7            (0x4300009CU) /**< \brief (EVSYS) User Multiplexer 7 */
#define REG_EVSYS_USER8            (0x430000A0U) /**< \brief (EVSYS) User Multiplexer 8 */
#define REG_EVSYS_USER9            (0x430000A4U) /**< \brief (EVSYS) User Multiplexer 9 */
#define REG_EVSYS_USER10           (0x430000A8U) /**< \brief (EVSYS) User Multiplexer 10 */
#define REG_EVSYS_USER11           (0x430000ACU) /**< \brief (EVSYS) User Multiplexer 11 */
#define REG_EVSYS_USER12           (0x430000B0U) /**< \brief (EVSYS) User Multiplexer 12 */
#define REG_EVSYS_USER13           (0x430000B4U) /**< \brief (EVSYS) User Multiplexer 13 */
#define REG_EVSYS_USER14           (0x430000B8U) /**< \brief (EVSYS) User Multiplexer 14 */
#define REG_EVSYS_USER15           (0x430000BCU) /**< \brief (EVSYS) User Multiplexer 15 */
#define REG_EVSYS_USER16           (0x430000C0U) /**< \brief (EVSYS) User Multiplexer 16 */
#define REG_EVSYS_USER17           (0x430000C4U) /**< \brief (EVSYS) User Multiplexer 17 */
#define REG_EVSYS_USER18           (0x430000C8U) /**< \brief (EVSYS) User Multiplexer 18 */
#define REG_EVSYS_USER19           (0x430000CCU) /**< \brief (EVSYS) User Multiplexer 19 */
#define REG_EVSYS_USER20           (0x430000D0U) /**< \brief (EVSYS) User Multiplexer 20 */
#define REG_EVSYS_USER21           (0x430000D4U) /**< \brief (EVSYS) User Multiplexer 21 */
#define REG_EVSYS_USER22           (0x430000D8U) /**< \brief (EVSYS) User Multiplexer 22 */
#define REG_EVSYS_USER23           (0x430000DCU) /**< \brief (EVSYS) User Multiplexer 23 */
#define REG_EVSYS_USER24           (0x430000E0U) /**< \brief (EVSYS) User Multiplexer 24 */
#define REG_EVSYS_USER25           (0x430000E4U) /**< \brief (EVSYS) User Multiplexer 25 */
#define REG_EVSYS_USER26           (0x430000E8U) /**< \brief (EVSYS) User Multiplexer 26 */
#define REG_EVSYS_USER27           (0x430000ECU) /**< \brief (EVSYS) User Multiplexer 27 */
#define REG_EVSYS_USER28           (0x430000F0U) /**< \brief (EVSYS) User Multiplexer 28 */
#define REG_EVSYS_USER29           (0x430000F4U) /**< \brief (EVSYS) User Multiplexer 29 */
#define REG_EVSYS_USER30           (0x430000F8U) /**< \brief (EVSYS) User Multiplexer 30 */
#define REG_EVSYS_USER31           (0x430000FCU) /**< \brief (EVSYS) User Multiplexer 31 */
#define REG_EVSYS_USER32           (0x43000100U) /**< \brief (EVSYS) User Multiplexer 32 */
#define REG_EVSYS_USER33           (0x43000104U) /**< \brief (EVSYS) User Multiplexer 33 */
#define REG_EVSYS_USER34           (0x43000108U) /**< \brief (EVSYS) User Multiplexer 34 */
#define REG_EVSYS_USER35           (0x4300010CU) /**< \brief (EVSYS) User Multiplexer 35 */
#define REG_EVSYS_USER36           (0x43000110U) /**< \brief (EVSYS) User Multiplexer 36 */
#define REG_EVSYS_USER37           (0x43000114U) /**< \brief (EVSYS) User Multiplexer 37 */
#define REG_EVSYS_USER38           (0x43000118U) /**< \brief (EVSYS) User Multiplexer 38 */
#define REG_EVSYS_USER39           (0x4300011CU) /**< \brief (EVSYS) User Multiplexer 39 */
#define REG_EVSYS_USER40           (0x43000120U) /**< \brief (EVSYS) User Multiplexer 40 */
#define REG_EVSYS_USER41           (0x43000124U) /**< \brief (EVSYS) User Multiplexer 41 */
#define REG_EVSYS_USER42           (0x43000128U) /**< \brief (EVSYS) User Multiplexer 42 */
#define REG_EVSYS_USER43           (0x4300012CU) /**< \brief (EVSYS) User Multiplexer 43 */
#define REG_EVSYS_USER44           (0x43000130U) /**< \brief (EVSYS) User Multiplexer 44 */
#else
#define REG_EVSYS_CTRLA            (*(RwReg8 *)0x43000000U) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHSTATUS         (*(RoReg  *)0x4300000CU) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (*(RwReg  *)0x43000010U) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (*(RwReg  *)0x43000014U) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (*(RwReg  *)0x43000018U) /**< \brief (EVSYS) Interrupt Flag Status and Clear */
#define REG_EVSYS_SWEVT            (*(WoReg  *)0x4300001CU) /**< \brief (EVSYS) Software Event */
#define REG_EVSYS_CHANNEL0         (*(RwReg  *)0x43000020U) /**< \brief (EVSYS) Channel 0 */
#define REG_EVSYS_CHANNEL1         (*(RwReg  *)0x43000024U) /**< \brief (EVSYS) Channel 1 */
#define REG_EVSYS_CHANNEL2         (*(RwReg  *)0x43000028U) /**< \brief (EVSYS) Channel 2 */
#define REG_EVSYS_CHANNEL3         (*(RwReg  *)0x4300002CU) /**< \brief (EVSYS) Channel 3 */
#define REG_EVSYS_CHANNEL4         (*(RwReg  *)0x43000030U) /**< \brief (EVSYS) Channel 4 */
#define REG_EVSYS_CHANNEL5         (*(RwReg  *)0x43000034U) /**< \brief (EVSYS) Channel 5 */
#define REG_EVSYS_CHANNEL6         (*(RwReg  *)0x43000038U) /**< \brief (EVSYS) Channel 6 */
#define REG_EVSYS_CHANNEL7         (*(RwReg  *)0x4300003CU) /**< \brief (EVSYS) Channel 7 */
#define REG_EVSYS_CHANNEL8         (*(RwReg  *)0x43000040U) /**< \brief (EVSYS) Channel 8 */
#define REG_EVSYS_CHANNEL9         (*(RwReg  *)0x43000044U) /**< \brief (EVSYS) Channel 9 */
#define REG_EVSYS_CHANNEL10        (*(RwReg  *)0x43000048U) /**< \brief (EVSYS) Channel 10 */
#define REG_EVSYS_CHANNEL11        (*(RwReg  *)0x4300004CU) /**< \brief (EVSYS) Channel 11 */
#define REG_EVSYS_USER0            (*(RwReg  *)0x43000080U) /**< \brief (EVSYS) User Multiplexer 0 */
#define REG_EVSYS_USER1            (*(RwReg  *)0x43000084U) /**< \brief (EVSYS) User Multiplexer 1 */
#define REG_EVSYS_USER2            (*(RwReg  *)0x43000088U) /**< \brief (EVSYS) User Multiplexer 2 */
#define REG_EVSYS_USER3            (*(RwReg  *)0x4300008CU) /**< \brief (EVSYS) User Multiplexer 3 */
#define REG_EVSYS_USER4            (*(RwReg  *)0x43000090U) /**< \brief (EVSYS) User Multiplexer 4 */
#define REG_EVSYS_USER5            (*(RwReg  *)0x43000094U) /**< \brief (EVSYS) User Multiplexer 5 */
#define REG_EVSYS_USER6            (*(RwReg  *)0x43000098U) /**< \brief (EVSYS) User Multiplexer 6 */
#define REG_EVSYS_USER7            (*(RwReg  *)0x4300009CU) /**< \brief (EVSYS) User Multiplexer 7 */
#define REG_EVSYS_USER8            (*(RwReg  *)0x430000A0U) /**< \brief (EVSYS) User Multiplexer 8 */
#define REG_EVSYS_USER9            (*(RwReg  *)0x430000A4U) /**< \brief (EVSYS) User Multiplexer 9 */
#define REG_EVSYS_USER10           (*(RwReg  *)0x430000A8U) /**< \brief (EVSYS) User Multiplexer 10 */
#define REG_EVSYS_USER11           (*(RwReg  *)0x430000ACU) /**< \brief (EVSYS) User Multiplexer 11 */
#define REG_EVSYS_USER12           (*(RwReg  *)0x430000B0U) /**< \brief (EVSYS) User Multiplexer 12 */
#define REG_EVSYS_USER13           (*(RwReg  *)0x430000B4U) /**< \brief (EVSYS) User Multiplexer 13 */
#define REG_EVSYS_USER14           (*(RwReg  *)0x430000B8U) /**< \brief (EVSYS) User Multiplexer 14 */
#define REG_EVSYS_USER15           (*(RwReg  *)0x430000BCU) /**< \brief (EVSYS) User Multiplexer 15 */
#define REG_EVSYS_USER16           (*(RwReg  *)0x430000C0U) /**< \brief (EVSYS) User Multiplexer 16 */
#define REG_EVSYS_USER17           (*(RwReg  *)0x430000C4U) /**< \brief (EVSYS) User Multiplexer 17 */
#define REG_EVSYS_USER18           (*(RwReg  *)0x430000C8U) /**< \brief (EVSYS) User Multiplexer 18 */
#define REG_EVSYS_USER19           (*(RwReg  *)0x430000CCU) /**< \brief (EVSYS) User Multiplexer 19 */
#define REG_EVSYS_USER20           (*(RwReg  *)0x430000D0U) /**< \brief (EVSYS) User Multiplexer 20 */
#define REG_EVSYS_USER21           (*(RwReg  *)0x430000D4U) /**< \brief (EVSYS) User Multiplexer 21 */
#define REG_EVSYS_USER22           (*(RwReg  *)0x430000D8U) /**< \brief (EVSYS) User Multiplexer 22 */
#define REG_EVSYS_USER23           (*(RwReg  *)0x430000DCU) /**< \brief (EVSYS) User Multiplexer 23 */
#define REG_EVSYS_USER24           (*(RwReg  *)0x430000E0U) /**< \brief (EVSYS) User Multiplexer 24 */
#define REG_EVSYS_USER25           (*(RwReg  *)0x430000E4U) /**< \brief (EVSYS) User Multiplexer 25 */
#define REG_EVSYS_USER26           (*(RwReg  *)0x430000E8U) /**< \brief (EVSYS) User Multiplexer 26 */
#define REG_EVSYS_USER27           (*(RwReg  *)0x430000ECU) /**< \brief (EVSYS) User Multiplexer 27 */
#define REG_EVSYS_USER28           (*(RwReg  *)0x430000F0U) /**< \brief (EVSYS) User Multiplexer 28 */
#define REG_EVSYS_USER29           (*(RwReg  *)0x430000F4U) /**< \brief (EVSYS) User Multiplexer 29 */
#define REG_EVSYS_USER30           (*(RwReg  *)0x430000F8U) /**< \brief (EVSYS) User Multiplexer 30 */
#define REG_EVSYS_USER31           (*(RwReg  *)0x430000FCU) /**< \brief (EVSYS) User Multiplexer 31 */
#define REG_EVSYS_USER32           (*(RwReg  *)0x43000100U) /**< \brief (EVSYS) User Multiplexer 32 */
#define REG_EVSYS_USER33           (*(RwReg  *)0x43000104U) /**< \brief (EVSYS) User Multiplexer 33 */
#define REG_EVSYS_USER34           (*(RwReg  *)0x43000108U) /**< \brief (EVSYS) User Multiplexer 34 */
#define REG_EVSYS_USER35           (*(RwReg  *)0x4300010CU) /**< \brief (EVSYS) User Multiplexer 35 */
#define REG_EVSYS_USER36           (*(RwReg  *)0x43000110U) /**< \brief (EVSYS) User Multiplexer 36 */
#define REG_EVSYS_USER37           (*(RwReg  *)0x43000114U) /**< \brief (EVSYS) User Multiplexer 37 */
#define REG_EVSYS_USER38           (*(RwReg  *)0x43000118U) /**< \brief (EVSYS) User Multiplexer 38 */
#define REG_EVSYS_USER39           (*(RwReg  *)0x4300011CU) /**< \brief (EVSYS) User Multiplexer 39 */
#define REG_EVSYS_USER40           (*(RwReg  *)0x43000120U) /**< \brief (EVSYS) User Multiplexer 40 */
#define REG_EVSYS_USER41           (*(RwReg  *)0x43000124U) /**< \brief (EVSYS) User Multiplexer 41 */
#define REG_EVSYS_USER42           (*(RwReg  *)0x43000128U) /**< \brief (EVSYS) User Multiplexer 42 */
#define REG_EVSYS_USER43           (*(RwReg  *)0x4300012CU) /**< \brief (EVSYS) User Multiplexer 43 */
#define REG_EVSYS_USER44           (*(RwReg  *)0x43000130U) /**< \brief (EVSYS) User Multiplexer 44 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EVSYS peripheral ========== */
#define EVSYS_CHANNELS              12       // Number of Channels
#define EVSYS_CHANNELS_BITS         4        // Number of bits to select Channel
#define EVSYS_CHANNELS_MSB          11       // Number of Channels - 1
#define EVSYS_EXTEVT_NUM            0        // Number of External Event Generators
#define EVSYS_GCLK_ID_0             5
#define EVSYS_GCLK_ID_1             6
#define EVSYS_GCLK_ID_2             7
#define EVSYS_GCLK_ID_3             8
#define EVSYS_GCLK_ID_4             9
#define EVSYS_GCLK_ID_5             10
#define EVSYS_GCLK_ID_6             11
#define EVSYS_GCLK_ID_7             12
#define EVSYS_GCLK_ID_8             13
#define EVSYS_GCLK_ID_9             14
#define EVSYS_GCLK_ID_10            15
#define EVSYS_GCLK_ID_11            16
#define EVSYS_GCLK_ID_LSB           5
#define EVSYS_GCLK_ID_MSB           16
#define EVSYS_GCLK_ID_SIZE          12
#define EVSYS_GENERATORS            83       // Total Number of Event Generators
#define EVSYS_GENERATORS_BITS       7        // Number of bits to select Event Generator
#define EVSYS_USERS                 45       // Total Number of Event Users
#define EVSYS_USERS_BITS            6        // Number of bits to select Event User

// GENERATORS
#define EVSYS_ID_GEN_RTC_CMP_0      1
#define EVSYS_ID_GEN_RTC_CMP_1      2
#define EVSYS_ID_GEN_RTC_OVF        3
#define EVSYS_ID_GEN_RTC_PER_0      4
#define EVSYS_ID_GEN_RTC_PER_1      5
#define EVSYS_ID_GEN_RTC_PER_2      6
#define EVSYS_ID_GEN_RTC_PER_3      7
#define EVSYS_ID_GEN_RTC_PER_4      8
#define EVSYS_ID_GEN_RTC_PER_5      9
#define EVSYS_ID_GEN_RTC_PER_6      10
#define EVSYS_ID_GEN_RTC_PER_7      11
#define EVSYS_ID_GEN_EIC_EXTINT_0   12
#define EVSYS_ID_GEN_EIC_EXTINT_1   13
#define EVSYS_ID_GEN_EIC_EXTINT_2   14
#define EVSYS_ID_GEN_EIC_EXTINT_3   15
#define EVSYS_ID_GEN_EIC_EXTINT_4   16
#define EVSYS_ID_GEN_EIC_EXTINT_5   17
#define EVSYS_ID_GEN_EIC_EXTINT_6   18
#define EVSYS_ID_GEN_EIC_EXTINT_7   19
#define EVSYS_ID_GEN_EIC_EXTINT_8   20
#define EVSYS_ID_GEN_EIC_EXTINT_9   21
#define EVSYS_ID_GEN_EIC_EXTINT_10  22
#define EVSYS_ID_GEN_EIC_EXTINT_11  23
#define EVSYS_ID_GEN_EIC_EXTINT_12  24
#define EVSYS_ID_GEN_EIC_EXTINT_13  25
#define EVSYS_ID_GEN_EIC_EXTINT_14  26
#define EVSYS_ID_GEN_EIC_EXTINT_15  27
#define EVSYS_ID_GEN_DMAC_CH_0      28
#define EVSYS_ID_GEN_DMAC_CH_1      29
#define EVSYS_ID_GEN_DMAC_CH_2      30
#define EVSYS_ID_GEN_DMAC_CH_3      31
#define EVSYS_ID_GEN_DMAC_CH_4      32
#define EVSYS_ID_GEN_DMAC_CH_5      33
#define EVSYS_ID_GEN_DMAC_CH_6      34
#define EVSYS_ID_GEN_DMAC_CH_7      35
#define EVSYS_ID_GEN_TCC0_OVF       36
#define EVSYS_ID_GEN_TCC0_TRG       37
#define EVSYS_ID_GEN_TCC0_CNT       38
#define EVSYS_ID_GEN_TCC0_MCX_0     39
#define EVSYS_ID_GEN_TCC0_MCX_1     40
#define EVSYS_ID_GEN_TCC0_MCX_2     41
#define EVSYS_ID_GEN_TCC0_MCX_3     42
#define EVSYS_ID_GEN_TCC1_OVF       43
#define EVSYS_ID_GEN_TCC1_TRG       44
#define EVSYS_ID_GEN_TCC1_CNT       45
#define EVSYS_ID_GEN_TCC1_MCX_0     46
#define EVSYS_ID_GEN_TCC1_MCX_1     47
#define EVSYS_ID_GEN_TCC2_OVF       48
#define EVSYS_ID_GEN_TCC2_TRG       49
#define EVSYS_ID_GEN_TCC2_CNT       50
#define EVSYS_ID_GEN_TCC2_MCX_0     51
#define EVSYS_ID_GEN_TCC2_MCX_1     52
#define EVSYS_ID_GEN_TC0_OVF        53
#define EVSYS_ID_GEN_TC0_MCX_0      54
#define EVSYS_ID_GEN_TC0_MCX_1      55
#define EVSYS_ID_GEN_TC1_OVF        56
#define EVSYS_ID_GEN_TC1_MCX_0      57
#define EVSYS_ID_GEN_TC1_MCX_1      58
#define EVSYS_ID_GEN_TC2_OVF        59
#define EVSYS_ID_GEN_TC2_MCX_0      60
#define EVSYS_ID_GEN_TC2_MCX_1      61
#define EVSYS_ID_GEN_TC3_OVF        62
#define EVSYS_ID_GEN_TC3_MCX_0      63
#define EVSYS_ID_GEN_TC3_MCX_1      64
#define EVSYS_ID_GEN_TC4_OVF        65
#define EVSYS_ID_GEN_TC4_MCX_0      66
#define EVSYS_ID_GEN_TC4_MCX_1      67
#define EVSYS_ID_GEN_ADC_RESRDY     68
#define EVSYS_ID_GEN_ADC_WINMON     69
#define EVSYS_ID_GEN_AC_COMP_0      70
#define EVSYS_ID_GEN_AC_COMP_1      71
#define EVSYS_ID_GEN_AC_WIN_0       72
#define EVSYS_ID_GEN_DAC_EMPTY_0    73
#define EVSYS_ID_GEN_DAC_EMPTY_1    74
#define EVSYS_ID_GEN_PTC_EOC        75
#define EVSYS_ID_GEN_PTC_WCOMP      76
#define EVSYS_ID_GEN_TRNG_READY     77
#define EVSYS_ID_GEN_CCL_LUTOUT_0   78
#define EVSYS_ID_GEN_CCL_LUTOUT_1   79
#define EVSYS_ID_GEN_CCL_LUTOUT_2   80
#define EVSYS_ID_GEN_CCL_LUTOUT_3   81
#define EVSYS_ID_GEN_PAC_ACCERR     82
#define EVSYS_ID_GEN_TAL_BRK        83

// USERS
#define EVSYS_ID_USER_PORT_EV_0     0
#define EVSYS_ID_USER_PORT_EV_1     1
#define EVSYS_ID_USER_PORT_EV_2     2
#define EVSYS_ID_USER_PORT_EV_3     3
#define EVSYS_ID_USER_DMAC_CH_0     4
#define EVSYS_ID_USER_DMAC_CH_1     5
#define EVSYS_ID_USER_DMAC_CH_2     6
#define EVSYS_ID_USER_DMAC_CH_3     7
#define EVSYS_ID_USER_DMAC_CH_4     8
#define EVSYS_ID_USER_DMAC_CH_5     9
#define EVSYS_ID_USER_DMAC_CH_6     10
#define EVSYS_ID_USER_DMAC_CH_7     11
#define EVSYS_ID_USER_TCC0_EV_0     12
#define EVSYS_ID_USER_TCC0_EV_1     13
#define EVSYS_ID_USER_TCC0_MC_0     14
#define EVSYS_ID_USER_TCC0_MC_1     15
#define EVSYS_ID_USER_TCC0_MC_2     16
#define EVSYS_ID_USER_TCC0_MC_3     17
#define EVSYS_ID_USER_TCC1_EV_0     18
#define EVSYS_ID_USER_TCC1_EV_1     19
#define EVSYS_ID_USER_TCC1_MC_0     20
#define EVSYS_ID_USER_TCC1_MC_1     21
#define EVSYS_ID_USER_TCC2_EV_0     22
#define EVSYS_ID_USER_TCC2_EV_1     23
#define EVSYS_ID_USER_TCC2_MC_0     24
#define EVSYS_ID_USER_TCC2_MC_1     25
#define EVSYS_ID_USER_TC0_EVU       26
#define EVSYS_ID_USER_TC1_EVU       27
#define EVSYS_ID_USER_TC2_EVU       28
#define EVSYS_ID_USER_TC3_EVU       29
#define EVSYS_ID_USER_TC4_EVU       30
#define EVSYS_ID_USER_ADC_START     31
#define EVSYS_ID_USER_ADC_SYNC      32
#define EVSYS_ID_USER_AC_SOC_0      33
#define EVSYS_ID_USER_AC_SOC_1      34
#define EVSYS_ID_USER_DAC_START_0   35
#define EVSYS_ID_USER_DAC_START_1   36
#define EVSYS_ID_USER_PTC_STCONV    37
#define EVSYS_ID_USER_CCL_LUTIN_0   38
#define EVSYS_ID_USER_CCL_LUTIN_1   39
#define EVSYS_ID_USER_CCL_LUTIN_2   40
#define EVSYS_ID_USER_CCL_LUTIN_3   41
#define EVSYS_ID_USER_TAL_BRK       42
#define EVSYS_ID_USER_MTB_START     43
#define EVSYS_ID_USER_MTB_STOP      44

#endif /* _SAMR30_EVSYS_INSTANCE_ */
