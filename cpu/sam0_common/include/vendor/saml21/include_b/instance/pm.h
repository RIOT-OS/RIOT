/**
 * \file
 *
 * \brief Instance description for PM
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

#ifndef _SAML21_PM_INSTANCE_
#define _SAML21_PM_INSTANCE_

/* ========== Register definition for PM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PM_CTRLA               (0x40000000) /**< \brief (PM) Control A */
#define REG_PM_SLEEPCFG            (0x40000001) /**< \brief (PM) Sleep Configuration */
#define REG_PM_PLCFG               (0x40000002) /**< \brief (PM) Performance Level Configuration */
#define REG_PM_INTENCLR            (0x40000004) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (0x40000005) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (0x40000006) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_STDBYCFG            (0x40000008) /**< \brief (PM) Standby Configuration */
#define REG_PM_PWSAKDLY            (0x4000000C) /**< \brief (PM) Power Switch Acknowledge Delay */
#else
#define REG_PM_CTRLA               (*(RwReg8 *)0x40000000UL) /**< \brief (PM) Control A */
#define REG_PM_SLEEPCFG            (*(RwReg8 *)0x40000001UL) /**< \brief (PM) Sleep Configuration */
#define REG_PM_PLCFG               (*(RwReg8 *)0x40000002UL) /**< \brief (PM) Performance Level Configuration */
#define REG_PM_INTENCLR            (*(RwReg8 *)0x40000004UL) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (*(RwReg8 *)0x40000005UL) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (*(RwReg8 *)0x40000006UL) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_STDBYCFG            (*(RwReg16*)0x40000008UL) /**< \brief (PM) Standby Configuration */
#define REG_PM_PWSAKDLY            (*(RwReg8 *)0x4000000CUL) /**< \brief (PM) Power Switch Acknowledge Delay */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PM peripheral ========== */
#define PM_PD_NUM                   3        // Number of switchable Power Domain

#endif /* _SAML21_PM_INSTANCE_ */
