/**
 * \file
 *
 * \brief Instance description for GCLK
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

#ifndef _SAML21_GCLK_INSTANCE_
#define _SAML21_GCLK_INSTANCE_

/* ========== Register definition for GCLK peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_GCLK_CTRLA             (0x40001800) /**< \brief (GCLK) Control */
#define REG_GCLK_SYNCBUSY          (0x40001804) /**< \brief (GCLK) Synchronization Busy */
#define REG_GCLK_GENCTRL0          (0x40001820) /**< \brief (GCLK) Generic Clock Generator Control 0 */
#define REG_GCLK_GENCTRL1          (0x40001824) /**< \brief (GCLK) Generic Clock Generator Control 1 */
#define REG_GCLK_GENCTRL2          (0x40001828) /**< \brief (GCLK) Generic Clock Generator Control 2 */
#define REG_GCLK_GENCTRL3          (0x4000182C) /**< \brief (GCLK) Generic Clock Generator Control 3 */
#define REG_GCLK_GENCTRL4          (0x40001830) /**< \brief (GCLK) Generic Clock Generator Control 4 */
#define REG_GCLK_GENCTRL5          (0x40001834) /**< \brief (GCLK) Generic Clock Generator Control 5 */
#define REG_GCLK_GENCTRL6          (0x40001838) /**< \brief (GCLK) Generic Clock Generator Control 6 */
#define REG_GCLK_GENCTRL7          (0x4000183C) /**< \brief (GCLK) Generic Clock Generator Control 7 */
#define REG_GCLK_GENCTRL8          (0x40001840) /**< \brief (GCLK) Generic Clock Generator Control 8 */
#define REG_GCLK_PCHCTRL0          (0x40001880) /**< \brief (GCLK) Peripheral Clock Control 0 */
#define REG_GCLK_PCHCTRL1          (0x40001884) /**< \brief (GCLK) Peripheral Clock Control 1 */
#define REG_GCLK_PCHCTRL2          (0x40001888) /**< \brief (GCLK) Peripheral Clock Control 2 */
#define REG_GCLK_PCHCTRL3          (0x4000188C) /**< \brief (GCLK) Peripheral Clock Control 3 */
#define REG_GCLK_PCHCTRL4          (0x40001890) /**< \brief (GCLK) Peripheral Clock Control 4 */
#define REG_GCLK_PCHCTRL5          (0x40001894) /**< \brief (GCLK) Peripheral Clock Control 5 */
#define REG_GCLK_PCHCTRL6          (0x40001898) /**< \brief (GCLK) Peripheral Clock Control 6 */
#define REG_GCLK_PCHCTRL7          (0x4000189C) /**< \brief (GCLK) Peripheral Clock Control 7 */
#define REG_GCLK_PCHCTRL8          (0x400018A0) /**< \brief (GCLK) Peripheral Clock Control 8 */
#define REG_GCLK_PCHCTRL9          (0x400018A4) /**< \brief (GCLK) Peripheral Clock Control 9 */
#define REG_GCLK_PCHCTRL10         (0x400018A8) /**< \brief (GCLK) Peripheral Clock Control 10 */
#define REG_GCLK_PCHCTRL11         (0x400018AC) /**< \brief (GCLK) Peripheral Clock Control 11 */
#define REG_GCLK_PCHCTRL12         (0x400018B0) /**< \brief (GCLK) Peripheral Clock Control 12 */
#define REG_GCLK_PCHCTRL13         (0x400018B4) /**< \brief (GCLK) Peripheral Clock Control 13 */
#define REG_GCLK_PCHCTRL14         (0x400018B8) /**< \brief (GCLK) Peripheral Clock Control 14 */
#define REG_GCLK_PCHCTRL15         (0x400018BC) /**< \brief (GCLK) Peripheral Clock Control 15 */
#define REG_GCLK_PCHCTRL16         (0x400018C0) /**< \brief (GCLK) Peripheral Clock Control 16 */
#define REG_GCLK_PCHCTRL17         (0x400018C4) /**< \brief (GCLK) Peripheral Clock Control 17 */
#define REG_GCLK_PCHCTRL18         (0x400018C8) /**< \brief (GCLK) Peripheral Clock Control 18 */
#define REG_GCLK_PCHCTRL19         (0x400018CC) /**< \brief (GCLK) Peripheral Clock Control 19 */
#define REG_GCLK_PCHCTRL20         (0x400018D0) /**< \brief (GCLK) Peripheral Clock Control 20 */
#define REG_GCLK_PCHCTRL21         (0x400018D4) /**< \brief (GCLK) Peripheral Clock Control 21 */
#define REG_GCLK_PCHCTRL22         (0x400018D8) /**< \brief (GCLK) Peripheral Clock Control 22 */
#define REG_GCLK_PCHCTRL23         (0x400018DC) /**< \brief (GCLK) Peripheral Clock Control 23 */
#define REG_GCLK_PCHCTRL24         (0x400018E0) /**< \brief (GCLK) Peripheral Clock Control 24 */
#define REG_GCLK_PCHCTRL25         (0x400018E4) /**< \brief (GCLK) Peripheral Clock Control 25 */
#define REG_GCLK_PCHCTRL26         (0x400018E8) /**< \brief (GCLK) Peripheral Clock Control 26 */
#define REG_GCLK_PCHCTRL27         (0x400018EC) /**< \brief (GCLK) Peripheral Clock Control 27 */
#define REG_GCLK_PCHCTRL28         (0x400018F0) /**< \brief (GCLK) Peripheral Clock Control 28 */
#define REG_GCLK_PCHCTRL29         (0x400018F4) /**< \brief (GCLK) Peripheral Clock Control 29 */
#define REG_GCLK_PCHCTRL30         (0x400018F8) /**< \brief (GCLK) Peripheral Clock Control 30 */
#define REG_GCLK_PCHCTRL31         (0x400018FC) /**< \brief (GCLK) Peripheral Clock Control 31 */
#define REG_GCLK_PCHCTRL32         (0x40001900) /**< \brief (GCLK) Peripheral Clock Control 32 */
#define REG_GCLK_PCHCTRL33         (0x40001904) /**< \brief (GCLK) Peripheral Clock Control 33 */
#define REG_GCLK_PCHCTRL34         (0x40001908) /**< \brief (GCLK) Peripheral Clock Control 34 */
#define REG_GCLK_PCHCTRL35         (0x4000190C) /**< \brief (GCLK) Peripheral Clock Control 35 */
#else
#define REG_GCLK_CTRLA             (*(RwReg8 *)0x40001800UL) /**< \brief (GCLK) Control */
#define REG_GCLK_SYNCBUSY          (*(RoReg  *)0x40001804UL) /**< \brief (GCLK) Synchronization Busy */
#define REG_GCLK_GENCTRL0          (*(RwReg  *)0x40001820UL) /**< \brief (GCLK) Generic Clock Generator Control 0 */
#define REG_GCLK_GENCTRL1          (*(RwReg  *)0x40001824UL) /**< \brief (GCLK) Generic Clock Generator Control 1 */
#define REG_GCLK_GENCTRL2          (*(RwReg  *)0x40001828UL) /**< \brief (GCLK) Generic Clock Generator Control 2 */
#define REG_GCLK_GENCTRL3          (*(RwReg  *)0x4000182CUL) /**< \brief (GCLK) Generic Clock Generator Control 3 */
#define REG_GCLK_GENCTRL4          (*(RwReg  *)0x40001830UL) /**< \brief (GCLK) Generic Clock Generator Control 4 */
#define REG_GCLK_GENCTRL5          (*(RwReg  *)0x40001834UL) /**< \brief (GCLK) Generic Clock Generator Control 5 */
#define REG_GCLK_GENCTRL6          (*(RwReg  *)0x40001838UL) /**< \brief (GCLK) Generic Clock Generator Control 6 */
#define REG_GCLK_GENCTRL7          (*(RwReg  *)0x4000183CUL) /**< \brief (GCLK) Generic Clock Generator Control 7 */
#define REG_GCLK_GENCTRL8          (*(RwReg  *)0x40001840UL) /**< \brief (GCLK) Generic Clock Generator Control 8 */
#define REG_GCLK_PCHCTRL0          (*(RwReg  *)0x40001880UL) /**< \brief (GCLK) Peripheral Clock Control 0 */
#define REG_GCLK_PCHCTRL1          (*(RwReg  *)0x40001884UL) /**< \brief (GCLK) Peripheral Clock Control 1 */
#define REG_GCLK_PCHCTRL2          (*(RwReg  *)0x40001888UL) /**< \brief (GCLK) Peripheral Clock Control 2 */
#define REG_GCLK_PCHCTRL3          (*(RwReg  *)0x4000188CUL) /**< \brief (GCLK) Peripheral Clock Control 3 */
#define REG_GCLK_PCHCTRL4          (*(RwReg  *)0x40001890UL) /**< \brief (GCLK) Peripheral Clock Control 4 */
#define REG_GCLK_PCHCTRL5          (*(RwReg  *)0x40001894UL) /**< \brief (GCLK) Peripheral Clock Control 5 */
#define REG_GCLK_PCHCTRL6          (*(RwReg  *)0x40001898UL) /**< \brief (GCLK) Peripheral Clock Control 6 */
#define REG_GCLK_PCHCTRL7          (*(RwReg  *)0x4000189CUL) /**< \brief (GCLK) Peripheral Clock Control 7 */
#define REG_GCLK_PCHCTRL8          (*(RwReg  *)0x400018A0UL) /**< \brief (GCLK) Peripheral Clock Control 8 */
#define REG_GCLK_PCHCTRL9          (*(RwReg  *)0x400018A4UL) /**< \brief (GCLK) Peripheral Clock Control 9 */
#define REG_GCLK_PCHCTRL10         (*(RwReg  *)0x400018A8UL) /**< \brief (GCLK) Peripheral Clock Control 10 */
#define REG_GCLK_PCHCTRL11         (*(RwReg  *)0x400018ACUL) /**< \brief (GCLK) Peripheral Clock Control 11 */
#define REG_GCLK_PCHCTRL12         (*(RwReg  *)0x400018B0UL) /**< \brief (GCLK) Peripheral Clock Control 12 */
#define REG_GCLK_PCHCTRL13         (*(RwReg  *)0x400018B4UL) /**< \brief (GCLK) Peripheral Clock Control 13 */
#define REG_GCLK_PCHCTRL14         (*(RwReg  *)0x400018B8UL) /**< \brief (GCLK) Peripheral Clock Control 14 */
#define REG_GCLK_PCHCTRL15         (*(RwReg  *)0x400018BCUL) /**< \brief (GCLK) Peripheral Clock Control 15 */
#define REG_GCLK_PCHCTRL16         (*(RwReg  *)0x400018C0UL) /**< \brief (GCLK) Peripheral Clock Control 16 */
#define REG_GCLK_PCHCTRL17         (*(RwReg  *)0x400018C4UL) /**< \brief (GCLK) Peripheral Clock Control 17 */
#define REG_GCLK_PCHCTRL18         (*(RwReg  *)0x400018C8UL) /**< \brief (GCLK) Peripheral Clock Control 18 */
#define REG_GCLK_PCHCTRL19         (*(RwReg  *)0x400018CCUL) /**< \brief (GCLK) Peripheral Clock Control 19 */
#define REG_GCLK_PCHCTRL20         (*(RwReg  *)0x400018D0UL) /**< \brief (GCLK) Peripheral Clock Control 20 */
#define REG_GCLK_PCHCTRL21         (*(RwReg  *)0x400018D4UL) /**< \brief (GCLK) Peripheral Clock Control 21 */
#define REG_GCLK_PCHCTRL22         (*(RwReg  *)0x400018D8UL) /**< \brief (GCLK) Peripheral Clock Control 22 */
#define REG_GCLK_PCHCTRL23         (*(RwReg  *)0x400018DCUL) /**< \brief (GCLK) Peripheral Clock Control 23 */
#define REG_GCLK_PCHCTRL24         (*(RwReg  *)0x400018E0UL) /**< \brief (GCLK) Peripheral Clock Control 24 */
#define REG_GCLK_PCHCTRL25         (*(RwReg  *)0x400018E4UL) /**< \brief (GCLK) Peripheral Clock Control 25 */
#define REG_GCLK_PCHCTRL26         (*(RwReg  *)0x400018E8UL) /**< \brief (GCLK) Peripheral Clock Control 26 */
#define REG_GCLK_PCHCTRL27         (*(RwReg  *)0x400018ECUL) /**< \brief (GCLK) Peripheral Clock Control 27 */
#define REG_GCLK_PCHCTRL28         (*(RwReg  *)0x400018F0UL) /**< \brief (GCLK) Peripheral Clock Control 28 */
#define REG_GCLK_PCHCTRL29         (*(RwReg  *)0x400018F4UL) /**< \brief (GCLK) Peripheral Clock Control 29 */
#define REG_GCLK_PCHCTRL30         (*(RwReg  *)0x400018F8UL) /**< \brief (GCLK) Peripheral Clock Control 30 */
#define REG_GCLK_PCHCTRL31         (*(RwReg  *)0x400018FCUL) /**< \brief (GCLK) Peripheral Clock Control 31 */
#define REG_GCLK_PCHCTRL32         (*(RwReg  *)0x40001900UL) /**< \brief (GCLK) Peripheral Clock Control 32 */
#define REG_GCLK_PCHCTRL33         (*(RwReg  *)0x40001904UL) /**< \brief (GCLK) Peripheral Clock Control 33 */
#define REG_GCLK_PCHCTRL34         (*(RwReg  *)0x40001908UL) /**< \brief (GCLK) Peripheral Clock Control 34 */
#define REG_GCLK_PCHCTRL35         (*(RwReg  *)0x4000190CUL) /**< \brief (GCLK) Peripheral Clock Control 35 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for GCLK peripheral ========== */
#define GCLK_GENDIV_BITS            16      
#define GCLK_GEN_NUM                9        // Number of Generic Clock Generators
#define GCLK_GEN_NUM_MSB            8        // Number of Generic Clock Generators - 1
#define GCLK_GEN_SOURCE_NUM_MSB     8        // Number of Generic Clock Sources - 1
#define GCLK_NUM                    36       // Number of Generic Clock Users
#define GCLK_SOURCE_DFLL48M         7       
#define GCLK_SOURCE_DPLL96M         8       
#define GCLK_SOURCE_GCLKGEN1        2       
#define GCLK_SOURCE_GCLKIN          1       
#define GCLK_SOURCE_NUM             9        // Number of Generic Clock Sources
#define GCLK_SOURCE_OSCULP32K       3       
#define GCLK_SOURCE_OSC16M          6       
#define GCLK_SOURCE_OSC32K          4       
#define GCLK_SOURCE_XOSC            0       
#define GCLK_SOURCE_XOSC32K         5       

#endif /* _SAML21_GCLK_INSTANCE_ */
