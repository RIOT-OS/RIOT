/**
 * \file
 *
 * \brief Instance description for MCLK
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

#ifndef _SAMR30_MCLK_INSTANCE_
#define _SAMR30_MCLK_INSTANCE_

/* ========== Register definition for MCLK peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_MCLK_CTRLA             (0x40000400U) /**< \brief (MCLK) Control A */
#define REG_MCLK_INTENCLR          (0x40000401U) /**< \brief (MCLK) Interrupt Enable Clear */
#define REG_MCLK_INTENSET          (0x40000402U) /**< \brief (MCLK) Interrupt Enable Set */
#define REG_MCLK_INTFLAG           (0x40000403U) /**< \brief (MCLK) Interrupt Flag Status and Clear */
#define REG_MCLK_CPUDIV            (0x40000404U) /**< \brief (MCLK) CPU Clock Division */
#define REG_MCLK_LPDIV             (0x40000405U) /**< \brief (MCLK) Low-Power Clock Division */
#define REG_MCLK_BUPDIV            (0x40000406U) /**< \brief (MCLK) Backup Clock Division */
#define REG_MCLK_AHBMASK           (0x40000410U) /**< \brief (MCLK) AHB Mask */
#define REG_MCLK_APBAMASK          (0x40000414U) /**< \brief (MCLK) APBA Mask */
#define REG_MCLK_APBBMASK          (0x40000418U) /**< \brief (MCLK) APBB Mask */
#define REG_MCLK_APBCMASK          (0x4000041CU) /**< \brief (MCLK) APBC Mask */
#define REG_MCLK_APBDMASK          (0x40000420U) /**< \brief (MCLK) APBD Mask */
#define REG_MCLK_APBEMASK          (0x40000424U) /**< \brief (MCLK) APBE Mask */
#else
#define REG_MCLK_CTRLA             (*(RwReg8 *)0x40000400U) /**< \brief (MCLK) Control A */
#define REG_MCLK_INTENCLR          (*(RwReg8 *)0x40000401U) /**< \brief (MCLK) Interrupt Enable Clear */
#define REG_MCLK_INTENSET          (*(RwReg8 *)0x40000402U) /**< \brief (MCLK) Interrupt Enable Set */
#define REG_MCLK_INTFLAG           (*(RwReg8 *)0x40000403U) /**< \brief (MCLK) Interrupt Flag Status and Clear */
#define REG_MCLK_CPUDIV            (*(RwReg8 *)0x40000404U) /**< \brief (MCLK) CPU Clock Division */
#define REG_MCLK_LPDIV             (*(RwReg8 *)0x40000405U) /**< \brief (MCLK) Low-Power Clock Division */
#define REG_MCLK_BUPDIV            (*(RwReg8 *)0x40000406U) /**< \brief (MCLK) Backup Clock Division */
#define REG_MCLK_AHBMASK           (*(RwReg  *)0x40000410U) /**< \brief (MCLK) AHB Mask */
#define REG_MCLK_APBAMASK          (*(RwReg  *)0x40000414U) /**< \brief (MCLK) APBA Mask */
#define REG_MCLK_APBBMASK          (*(RwReg  *)0x40000418U) /**< \brief (MCLK) APBB Mask */
#define REG_MCLK_APBCMASK          (*(RwReg  *)0x4000041CU) /**< \brief (MCLK) APBC Mask */
#define REG_MCLK_APBDMASK          (*(RwReg  *)0x40000420U) /**< \brief (MCLK) APBD Mask */
#define REG_MCLK_APBEMASK          (*(RwReg  *)0x40000424U) /**< \brief (MCLK) APBE Mask */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for MCLK peripheral ========== */
#define MCLK_CTRLA_MCSEL_GCLK       1       
#define MCLK_CTRLA_MCSEL_OSC8M      0       
#define MCLK_MCLK_CLK_APB_NUM       5       
#define MCLK_SYSTEM_CLOCK           4000000  // System Clock Frequency at Reset

#endif /* _SAMR30_MCLK_INSTANCE_ */
