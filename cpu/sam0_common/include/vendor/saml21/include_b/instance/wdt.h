/**
 * \file
 *
 * \brief Instance description for WDT
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

#ifndef _SAML21_WDT_INSTANCE_
#define _SAML21_WDT_INSTANCE_

/* ========== Register definition for WDT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_WDT_CTRLA              (0x40001C00) /**< \brief (WDT) Control */
#define REG_WDT_CONFIG             (0x40001C01) /**< \brief (WDT) Configuration */
#define REG_WDT_EWCTRL             (0x40001C02) /**< \brief (WDT) Early Warning Interrupt Control */
#define REG_WDT_INTENCLR           (0x40001C04) /**< \brief (WDT) Interrupt Enable Clear */
#define REG_WDT_INTENSET           (0x40001C05) /**< \brief (WDT) Interrupt Enable Set */
#define REG_WDT_INTFLAG            (0x40001C06) /**< \brief (WDT) Interrupt Flag Status and Clear */
#define REG_WDT_SYNCBUSY           (0x40001C08) /**< \brief (WDT) Synchronization Busy */
#define REG_WDT_CLEAR              (0x40001C0C) /**< \brief (WDT) Clear */
#else
#define REG_WDT_CTRLA              (*(RwReg8 *)0x40001C00UL) /**< \brief (WDT) Control */
#define REG_WDT_CONFIG             (*(RwReg8 *)0x40001C01UL) /**< \brief (WDT) Configuration */
#define REG_WDT_EWCTRL             (*(RwReg8 *)0x40001C02UL) /**< \brief (WDT) Early Warning Interrupt Control */
#define REG_WDT_INTENCLR           (*(RwReg8 *)0x40001C04UL) /**< \brief (WDT) Interrupt Enable Clear */
#define REG_WDT_INTENSET           (*(RwReg8 *)0x40001C05UL) /**< \brief (WDT) Interrupt Enable Set */
#define REG_WDT_INTFLAG            (*(RwReg8 *)0x40001C06UL) /**< \brief (WDT) Interrupt Flag Status and Clear */
#define REG_WDT_SYNCBUSY           (*(RoReg  *)0x40001C08UL) /**< \brief (WDT) Synchronization Busy */
#define REG_WDT_CLEAR              (*(WoReg8 *)0x40001C0CUL) /**< \brief (WDT) Clear */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAML21_WDT_INSTANCE_ */
