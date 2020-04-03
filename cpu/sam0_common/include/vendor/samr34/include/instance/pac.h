/**
 * \file
 *
 * \brief Instance description for PAC
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMR34_PAC_INSTANCE_
#define _SAMR34_PAC_INSTANCE_

/* ========== Register definition for PAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PAC_WRCTRL             (0x44000000) /**< \brief (PAC) Write control */
#define REG_PAC_EVCTRL             (0x44000004) /**< \brief (PAC) Event control */
#define REG_PAC_INTENCLR           (0x44000008) /**< \brief (PAC) Interrupt enable clear */
#define REG_PAC_INTENSET           (0x44000009) /**< \brief (PAC) Interrupt enable set */
#define REG_PAC_INTFLAGAHB         (0x44000010) /**< \brief (PAC) Bridge interrupt flag status */
#define REG_PAC_INTFLAGA           (0x44000014) /**< \brief (PAC) Peripheral interrupt flag status - Bridge A */
#define REG_PAC_INTFLAGB           (0x44000018) /**< \brief (PAC) Peripheral interrupt flag status - Bridge B */
#define REG_PAC_INTFLAGC           (0x4400001C) /**< \brief (PAC) Peripheral interrupt flag status - Bridge C */
#define REG_PAC_INTFLAGD           (0x44000020) /**< \brief (PAC) Peripheral interrupt flag status - Bridge D */
#define REG_PAC_INTFLAGE           (0x44000024) /**< \brief (PAC) Peripheral interrupt flag status - Bridge E */
#define REG_PAC_STATUSA            (0x44000034) /**< \brief (PAC) Peripheral write protection status - Bridge A */
#define REG_PAC_STATUSB            (0x44000038) /**< \brief (PAC) Peripheral write protection status - Bridge B */
#define REG_PAC_STATUSC            (0x4400003C) /**< \brief (PAC) Peripheral write protection status - Bridge C */
#define REG_PAC_STATUSD            (0x44000040) /**< \brief (PAC) Peripheral write protection status - Bridge D */
#define REG_PAC_STATUSE            (0x44000044) /**< \brief (PAC) Peripheral write protection status - Bridge E */
#else
#define REG_PAC_WRCTRL             (*(RwReg  *)0x44000000UL) /**< \brief (PAC) Write control */
#define REG_PAC_EVCTRL             (*(RwReg8 *)0x44000004UL) /**< \brief (PAC) Event control */
#define REG_PAC_INTENCLR           (*(RwReg8 *)0x44000008UL) /**< \brief (PAC) Interrupt enable clear */
#define REG_PAC_INTENSET           (*(RwReg8 *)0x44000009UL) /**< \brief (PAC) Interrupt enable set */
#define REG_PAC_INTFLAGAHB         (*(RwReg  *)0x44000010UL) /**< \brief (PAC) Bridge interrupt flag status */
#define REG_PAC_INTFLAGA           (*(RwReg  *)0x44000014UL) /**< \brief (PAC) Peripheral interrupt flag status - Bridge A */
#define REG_PAC_INTFLAGB           (*(RwReg  *)0x44000018UL) /**< \brief (PAC) Peripheral interrupt flag status - Bridge B */
#define REG_PAC_INTFLAGC           (*(RwReg  *)0x4400001CUL) /**< \brief (PAC) Peripheral interrupt flag status - Bridge C */
#define REG_PAC_INTFLAGD           (*(RwReg  *)0x44000020UL) /**< \brief (PAC) Peripheral interrupt flag status - Bridge D */
#define REG_PAC_INTFLAGE           (*(RwReg  *)0x44000024UL) /**< \brief (PAC) Peripheral interrupt flag status - Bridge E */
#define REG_PAC_STATUSA            (*(RoReg  *)0x44000034UL) /**< \brief (PAC) Peripheral write protection status - Bridge A */
#define REG_PAC_STATUSB            (*(RoReg  *)0x44000038UL) /**< \brief (PAC) Peripheral write protection status - Bridge B */
#define REG_PAC_STATUSC            (*(RoReg  *)0x4400003CUL) /**< \brief (PAC) Peripheral write protection status - Bridge C */
#define REG_PAC_STATUSD            (*(RoReg  *)0x44000040UL) /**< \brief (PAC) Peripheral write protection status - Bridge D */
#define REG_PAC_STATUSE            (*(RoReg  *)0x44000044UL) /**< \brief (PAC) Peripheral write protection status - Bridge E */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PAC peripheral ========== */
#define PAC_CLK_AHB_DOMAIN                   // Clock domain of AHB clock
#define PAC_CLK_AHB_ID              14       // AHB clock index
#define PAC_HPB_NUM                 5        // Number of bridges AHB/APB
#define PAC_INTFLAG_NUM             6        // Number of intflag registers

#endif /* _SAMR34_PAC_INSTANCE_ */
