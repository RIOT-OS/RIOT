/**
 * \file
 *
 * \brief Instance description for PORT
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

#ifndef _SAML21_PORT_INSTANCE_
#define _SAML21_PORT_INSTANCE_

/* ========== Register definition for PORT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PORT_DIR0              (0x40002800) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (0x40002804) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (0x40002808) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (0x4000280C) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (0x40002810) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (0x40002814) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (0x40002818) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (0x4000281C) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (0x40002820) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (0x40002824) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (0x40002828) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_EVCTRL0           (0x4000282C) /**< \brief (PORT) Event Input Control 0 */
#define REG_PORT_PMUX0             (0x40002830) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (0x40002840) /**< \brief (PORT) Pin Configuration 0 */
#define REG_PORT_DIR1              (0x40002880) /**< \brief (PORT) Data Direction 1 */
#define REG_PORT_DIRCLR1           (0x40002884) /**< \brief (PORT) Data Direction Clear 1 */
#define REG_PORT_DIRSET1           (0x40002888) /**< \brief (PORT) Data Direction Set 1 */
#define REG_PORT_DIRTGL1           (0x4000288C) /**< \brief (PORT) Data Direction Toggle 1 */
#define REG_PORT_OUT1              (0x40002890) /**< \brief (PORT) Data Output Value 1 */
#define REG_PORT_OUTCLR1           (0x40002894) /**< \brief (PORT) Data Output Value Clear 1 */
#define REG_PORT_OUTSET1           (0x40002898) /**< \brief (PORT) Data Output Value Set 1 */
#define REG_PORT_OUTTGL1           (0x4000289C) /**< \brief (PORT) Data Output Value Toggle 1 */
#define REG_PORT_IN1               (0x400028A0) /**< \brief (PORT) Data Input Value 1 */
#define REG_PORT_CTRL1             (0x400028A4) /**< \brief (PORT) Control 1 */
#define REG_PORT_WRCONFIG1         (0x400028A8) /**< \brief (PORT) Write Configuration 1 */
#define REG_PORT_EVCTRL1           (0x400028AC) /**< \brief (PORT) Event Input Control 1 */
#define REG_PORT_PMUX1             (0x400028B0) /**< \brief (PORT) Peripheral Multiplexing 1 */
#define REG_PORT_PINCFG1           (0x400028C0) /**< \brief (PORT) Pin Configuration 1 */
#else
#define REG_PORT_DIR0              (*(RwReg  *)0x40002800UL) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (*(RwReg  *)0x40002804UL) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (*(RwReg  *)0x40002808UL) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (*(RwReg  *)0x4000280CUL) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (*(RwReg  *)0x40002810UL) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (*(RwReg  *)0x40002814UL) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (*(RwReg  *)0x40002818UL) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (*(RwReg  *)0x4000281CUL) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (*(RoReg  *)0x40002820UL) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (*(RwReg  *)0x40002824UL) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (*(WoReg  *)0x40002828UL) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_EVCTRL0           (*(RwReg  *)0x4000282CUL) /**< \brief (PORT) Event Input Control 0 */
#define REG_PORT_PMUX0             (*(RwReg8 *)0x40002830UL) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (*(RwReg8 *)0x40002840UL) /**< \brief (PORT) Pin Configuration 0 */
#define REG_PORT_DIR1              (*(RwReg  *)0x40002880UL) /**< \brief (PORT) Data Direction 1 */
#define REG_PORT_DIRCLR1           (*(RwReg  *)0x40002884UL) /**< \brief (PORT) Data Direction Clear 1 */
#define REG_PORT_DIRSET1           (*(RwReg  *)0x40002888UL) /**< \brief (PORT) Data Direction Set 1 */
#define REG_PORT_DIRTGL1           (*(RwReg  *)0x4000288CUL) /**< \brief (PORT) Data Direction Toggle 1 */
#define REG_PORT_OUT1              (*(RwReg  *)0x40002890UL) /**< \brief (PORT) Data Output Value 1 */
#define REG_PORT_OUTCLR1           (*(RwReg  *)0x40002894UL) /**< \brief (PORT) Data Output Value Clear 1 */
#define REG_PORT_OUTSET1           (*(RwReg  *)0x40002898UL) /**< \brief (PORT) Data Output Value Set 1 */
#define REG_PORT_OUTTGL1           (*(RwReg  *)0x4000289CUL) /**< \brief (PORT) Data Output Value Toggle 1 */
#define REG_PORT_IN1               (*(RoReg  *)0x400028A0UL) /**< \brief (PORT) Data Input Value 1 */
#define REG_PORT_CTRL1             (*(RwReg  *)0x400028A4UL) /**< \brief (PORT) Control 1 */
#define REG_PORT_WRCONFIG1         (*(WoReg  *)0x400028A8UL) /**< \brief (PORT) Write Configuration 1 */
#define REG_PORT_EVCTRL1           (*(RwReg  *)0x400028ACUL) /**< \brief (PORT) Event Input Control 1 */
#define REG_PORT_PMUX1             (*(RwReg8 *)0x400028B0UL) /**< \brief (PORT) Peripheral Multiplexing 1 */
#define REG_PORT_PINCFG1           (*(RwReg8 *)0x400028C0UL) /**< \brief (PORT) Pin Configuration 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PORT peripheral ========== */
#define PORT_BITS                   84      
#define PORT_DIR_DEFAULT_VAL        { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_DIR_IMPLEMENTED        { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_DRVSTR                 1        // DRVSTR supported?
#define PORT_DRVSTR_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_DRVSTR_IMPLEMENTED     { 0xD8FFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_EVENT_IMPLEMENTED      { 0xCBFFFFFF, 0xC0C3FFFF, 0x00000000 }
#define PORT_EV_NUM                 4       
#define PORT_INEN_DEFAULT_VAL       { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_INEN_IMPLEMENTED       { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_ODRAIN                 0        // ODRAIN supported?
#define PORT_ODRAIN_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_ODRAIN_IMPLEMENTED     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_OUT_DEFAULT_VAL        { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_OUT_IMPLEMENTED        { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_PIN_IMPLEMENTED        { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_PMUXBIT0_DEFAULT_VAL   { 0x00000000, 0x00000000, 0x000D0000 }
#define PORT_PMUXBIT0_IMPLEMENTED   { 0xDBFFFFFF, 0xC0C3FFFF, 0x00000000 }
#define PORT_PMUXBIT1_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT1_IMPLEMENTED   { 0xDBFFFFF3, 0xC0C3FF0F, 0x00000000 }
#define PORT_PMUXBIT2_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x000D0000 }
#define PORT_PMUXBIT2_IMPLEMENTED   { 0xDBFFFFF3, 0xC0C3FF0F, 0x00000000 }
#define PORT_PMUXBIT3_DEFAULT_VAL   { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT3_IMPLEMENTED   { 0xC3CF0FF0, 0x00C3CFC7, 0x00000000 }
#define PORT_PMUXEN_DEFAULT_VAL     { 0x40000000, 0x00000000, 0x00000000 }
#define PORT_PMUXEN_IMPLEMENTED     { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_PULLEN_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PULLEN_IMPLEMENTED     { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_SLEWLIM                0        // SLEWLIM supported?
#define PORT_SLEWLIM_DEFAULT_VAL    { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_SLEWLIM_IMPLEMENTED    { 0x00000000, 0x00000000, 0x00000000 }

#endif /* _SAML21_PORT_INSTANCE_ */
