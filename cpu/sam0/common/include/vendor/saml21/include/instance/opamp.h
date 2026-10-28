/**
 * \file
 *
 * \brief Instance description for OPAMP
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

#ifndef _SAML21_OPAMP_INSTANCE_
#define _SAML21_OPAMP_INSTANCE_

/* ========== Register definition for OPAMP peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_OPAMP_CTRLA            (0x43001800) /**< \brief (OPAMP) Control A */
#define REG_OPAMP_STATUS           (0x43001802) /**< \brief (OPAMP) Status */
#define REG_OPAMP_OPAMPCTRL0       (0x43001804) /**< \brief (OPAMP) OPAMP Control 0 */
#define REG_OPAMP_OPAMPCTRL1       (0x43001808) /**< \brief (OPAMP) OPAMP Control 1 */
#define REG_OPAMP_OPAMPCTRL2       (0x4300180C) /**< \brief (OPAMP) OPAMP Control 2 */
#else
#define REG_OPAMP_CTRLA            (*(RwReg8 *)0x43001800UL) /**< \brief (OPAMP) Control A */
#define REG_OPAMP_STATUS           (*(RoReg8 *)0x43001802UL) /**< \brief (OPAMP) Status */
#define REG_OPAMP_OPAMPCTRL0       (*(RwReg  *)0x43001804UL) /**< \brief (OPAMP) OPAMP Control 0 */
#define REG_OPAMP_OPAMPCTRL1       (*(RwReg  *)0x43001808UL) /**< \brief (OPAMP) OPAMP Control 1 */
#define REG_OPAMP_OPAMPCTRL2       (*(RwReg  *)0x4300180CUL) /**< \brief (OPAMP) OPAMP Control 2 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAML21_OPAMP_INSTANCE_ */
