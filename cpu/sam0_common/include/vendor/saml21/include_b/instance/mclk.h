/**
 * \file
 *
 * \brief Instance description for MCLK
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

#ifndef _SAML21_MCLK_INSTANCE_
#define _SAML21_MCLK_INSTANCE_

/* ========== Register definition for MCLK peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_MCLK_CTRLA             (0x40000400) /**< \brief (MCLK) Control A */
#define REG_MCLK_INTENCLR          (0x40000401) /**< \brief (MCLK) Interrupt Enable Clear */
#define REG_MCLK_INTENSET          (0x40000402) /**< \brief (MCLK) Interrupt Enable Set */
#define REG_MCLK_INTFLAG           (0x40000403) /**< \brief (MCLK) Interrupt Flag Status and Clear */
#define REG_MCLK_CPUDIV            (0x40000404) /**< \brief (MCLK) CPU Clock Division */
#define REG_MCLK_LPDIV             (0x40000405) /**< \brief (MCLK) Low-Power Clock Division */
#define REG_MCLK_BUPDIV            (0x40000406) /**< \brief (MCLK) Backup Clock Division */
#define REG_MCLK_AHBMASK           (0x40000410) /**< \brief (MCLK) AHB Mask */
#define REG_MCLK_APBAMASK          (0x40000414) /**< \brief (MCLK) APBA Mask */
#define REG_MCLK_APBBMASK          (0x40000418) /**< \brief (MCLK) APBB Mask */
#define REG_MCLK_APBCMASK          (0x4000041C) /**< \brief (MCLK) APBC Mask */
#define REG_MCLK_APBDMASK          (0x40000420) /**< \brief (MCLK) APBD Mask */
#define REG_MCLK_APBEMASK          (0x40000424) /**< \brief (MCLK) APBE Mask */
#else
#define REG_MCLK_CTRLA             (*(RwReg8 *)0x40000400UL) /**< \brief (MCLK) Control A */
#define REG_MCLK_INTENCLR          (*(RwReg8 *)0x40000401UL) /**< \brief (MCLK) Interrupt Enable Clear */
#define REG_MCLK_INTENSET          (*(RwReg8 *)0x40000402UL) /**< \brief (MCLK) Interrupt Enable Set */
#define REG_MCLK_INTFLAG           (*(RwReg8 *)0x40000403UL) /**< \brief (MCLK) Interrupt Flag Status and Clear */
#define REG_MCLK_CPUDIV            (*(RwReg8 *)0x40000404UL) /**< \brief (MCLK) CPU Clock Division */
#define REG_MCLK_LPDIV             (*(RwReg8 *)0x40000405UL) /**< \brief (MCLK) Low-Power Clock Division */
#define REG_MCLK_BUPDIV            (*(RwReg8 *)0x40000406UL) /**< \brief (MCLK) Backup Clock Division */
#define REG_MCLK_AHBMASK           (*(RwReg  *)0x40000410UL) /**< \brief (MCLK) AHB Mask */
#define REG_MCLK_APBAMASK          (*(RwReg  *)0x40000414UL) /**< \brief (MCLK) APBA Mask */
#define REG_MCLK_APBBMASK          (*(RwReg  *)0x40000418UL) /**< \brief (MCLK) APBB Mask */
#define REG_MCLK_APBCMASK          (*(RwReg  *)0x4000041CUL) /**< \brief (MCLK) APBC Mask */
#define REG_MCLK_APBDMASK          (*(RwReg  *)0x40000420UL) /**< \brief (MCLK) APBD Mask */
#define REG_MCLK_APBEMASK          (*(RwReg  *)0x40000424UL) /**< \brief (MCLK) APBE Mask */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for MCLK peripheral ========== */
#define MCLK_CTRLA_MCSEL_GCLK       1       
#define MCLK_CTRLA_MCSEL_OSC8M      0       
#define MCLK_MCLK_CLK_APB_NUM       5       
#define MCLK_SYSTEM_CLOCK           4000000  // System Clock Frequency at Reset

#endif /* _SAML21_MCLK_INSTANCE_ */
