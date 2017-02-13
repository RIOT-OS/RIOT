/**
 * \file
 *
 * \brief Instance description for PAC
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

#ifndef _SAML21_PAC_INSTANCE_
#define _SAML21_PAC_INSTANCE_

/* ========== Register definition for PAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PAC_WRCTRL             (0x44000000U) /**< \brief (PAC) Write control */
#define REG_PAC_EVCTRL             (0x44000004U) /**< \brief (PAC) Event control */
#define REG_PAC_INTENCLR           (0x44000008U) /**< \brief (PAC) Interrupt enable clear */
#define REG_PAC_INTENSET           (0x44000009U) /**< \brief (PAC) Interrupt enable set */
#define REG_PAC_INTFLAGAHB         (0x44000010U) /**< \brief (PAC) Bridge interrupt flag status */
#define REG_PAC_INTFLAGA           (0x44000014U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge A */
#define REG_PAC_INTFLAGB           (0x44000018U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge B */
#define REG_PAC_INTFLAGC           (0x4400001CU) /**< \brief (PAC) Peripheral interrupt flag status - Bridge C */
#define REG_PAC_INTFLAGD           (0x44000020U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge D */
#define REG_PAC_INTFLAGE           (0x44000024U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge E */
#define REG_PAC_STATUSA            (0x44000034U) /**< \brief (PAC) Peripheral write protection status - Bridge A */
#define REG_PAC_STATUSB            (0x44000038U) /**< \brief (PAC) Peripheral write protection status - Bridge B */
#define REG_PAC_STATUSC            (0x4400003CU) /**< \brief (PAC) Peripheral write protection status - Bridge C */
#define REG_PAC_STATUSD            (0x44000040U) /**< \brief (PAC) Peripheral write protection status - Bridge D */
#define REG_PAC_STATUSE            (0x44000044U) /**< \brief (PAC) Peripheral write protection status - Bridge E */
#else
#define REG_PAC_WRCTRL             (*(RwReg  *)0x44000000U) /**< \brief (PAC) Write control */
#define REG_PAC_EVCTRL             (*(RwReg8 *)0x44000004U) /**< \brief (PAC) Event control */
#define REG_PAC_INTENCLR           (*(RwReg8 *)0x44000008U) /**< \brief (PAC) Interrupt enable clear */
#define REG_PAC_INTENSET           (*(RwReg8 *)0x44000009U) /**< \brief (PAC) Interrupt enable set */
#define REG_PAC_INTFLAGAHB         (*(RwReg  *)0x44000010U) /**< \brief (PAC) Bridge interrupt flag status */
#define REG_PAC_INTFLAGA           (*(RwReg  *)0x44000014U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge A */
#define REG_PAC_INTFLAGB           (*(RwReg  *)0x44000018U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge B */
#define REG_PAC_INTFLAGC           (*(RwReg  *)0x4400001CU) /**< \brief (PAC) Peripheral interrupt flag status - Bridge C */
#define REG_PAC_INTFLAGD           (*(RwReg  *)0x44000020U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge D */
#define REG_PAC_INTFLAGE           (*(RwReg  *)0x44000024U) /**< \brief (PAC) Peripheral interrupt flag status - Bridge E */
#define REG_PAC_STATUSA            (*(RoReg  *)0x44000034U) /**< \brief (PAC) Peripheral write protection status - Bridge A */
#define REG_PAC_STATUSB            (*(RoReg  *)0x44000038U) /**< \brief (PAC) Peripheral write protection status - Bridge B */
#define REG_PAC_STATUSC            (*(RoReg  *)0x4400003CU) /**< \brief (PAC) Peripheral write protection status - Bridge C */
#define REG_PAC_STATUSD            (*(RoReg  *)0x44000040U) /**< \brief (PAC) Peripheral write protection status - Bridge D */
#define REG_PAC_STATUSE            (*(RoReg  *)0x44000044U) /**< \brief (PAC) Peripheral write protection status - Bridge E */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PAC peripheral ========== */
#define PAC_CLK_AHB_DOMAIN                   // Clock domain of AHB clock
#define PAC_CLK_AHB_ID              14       // AHB clock index
#define PAC_HPB_NUM                 5        // Number of bridges AHB/APB
#define PAC_INTFLAG_NUM             6        // Number of intflag registers

#endif /* _SAML21_PAC_INSTANCE_ */
