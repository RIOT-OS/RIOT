/**
 * \file
 *
 * \brief Instance description for EIC
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

#ifndef _SAML21_EIC_INSTANCE_
#define _SAML21_EIC_INSTANCE_

/* ========== Register definition for EIC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EIC_CTRLA              (0x40002400) /**< \brief (EIC) Control */
#define REG_EIC_NMICTRL            (0x40002401) /**< \brief (EIC) NMI Control */
#define REG_EIC_NMIFLAG            (0x40002402) /**< \brief (EIC) NMI Interrupt Flag */
#define REG_EIC_SYNCBUSY           (0x40002404) /**< \brief (EIC) Syncbusy register */
#define REG_EIC_EVCTRL             (0x40002408) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (0x4000240C) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (0x40002410) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (0x40002414) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_CONFIG0            (0x4000241C) /**< \brief (EIC) Configuration 0 */
#define REG_EIC_CONFIG1            (0x40002420) /**< \brief (EIC) Configuration 1 */
#else
#define REG_EIC_CTRLA              (*(RwReg8 *)0x40002400UL) /**< \brief (EIC) Control */
#define REG_EIC_NMICTRL            (*(RwReg8 *)0x40002401UL) /**< \brief (EIC) NMI Control */
#define REG_EIC_NMIFLAG            (*(RwReg16*)0x40002402UL) /**< \brief (EIC) NMI Interrupt Flag */
#define REG_EIC_SYNCBUSY           (*(RoReg  *)0x40002404UL) /**< \brief (EIC) Syncbusy register */
#define REG_EIC_EVCTRL             (*(RwReg  *)0x40002408UL) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (*(RwReg  *)0x4000240CUL) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (*(RwReg  *)0x40002410UL) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (*(RwReg  *)0x40002414UL) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_CONFIG0            (*(RwReg  *)0x4000241CUL) /**< \brief (EIC) Configuration 0 */
#define REG_EIC_CONFIG1            (*(RwReg  *)0x40002420UL) /**< \brief (EIC) Configuration 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EIC peripheral ========== */
#define EIC_GCLK_ID                 3       
#define EIC_NUMBER_OF_CONFIG_REGS   2       
#define EIC_NUMBER_OF_INTERRUPTS    16      

#endif /* _SAML21_EIC_INSTANCE_ */
