/**
 * \file
 *
 * \brief Instance description for GCLK
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAML21_GCLK_INSTANCE_
#define _SAML21_GCLK_INSTANCE_

/* ========== Register definition for GCLK peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_GCLK_CTRLA             (0x40001800U) /**< \brief (GCLK) Control */
#define REG_GCLK_SYNCBUSY          (0x40001804U) /**< \brief (GCLK) Synchronization Busy */
#define REG_GCLK_GENCTRL0          (0x40001820U) /**< \brief (GCLK) Generic Clock Generator Control 0 */
#define REG_GCLK_GENCTRL1          (0x40001824U) /**< \brief (GCLK) Generic Clock Generator Control 1 */
#define REG_GCLK_GENCTRL2          (0x40001828U) /**< \brief (GCLK) Generic Clock Generator Control 2 */
#define REG_GCLK_GENCTRL3          (0x4000182CU) /**< \brief (GCLK) Generic Clock Generator Control 3 */
#define REG_GCLK_GENCTRL4          (0x40001830U) /**< \brief (GCLK) Generic Clock Generator Control 4 */
#define REG_GCLK_GENCTRL5          (0x40001834U) /**< \brief (GCLK) Generic Clock Generator Control 5 */
#define REG_GCLK_GENCTRL6          (0x40001838U) /**< \brief (GCLK) Generic Clock Generator Control 6 */
#define REG_GCLK_GENCTRL7          (0x4000183CU) /**< \brief (GCLK) Generic Clock Generator Control 7 */
#define REG_GCLK_GENCTRL8          (0x40001840U) /**< \brief (GCLK) Generic Clock Generator Control 8 */
#define REG_GCLK_PCHCTRL0          (0x40001880U) /**< \brief (GCLK) Peripheral Clock Control 0 */
#define REG_GCLK_PCHCTRL1          (0x40001884U) /**< \brief (GCLK) Peripheral Clock Control 1 */
#define REG_GCLK_PCHCTRL2          (0x40001888U) /**< \brief (GCLK) Peripheral Clock Control 2 */
#define REG_GCLK_PCHCTRL3          (0x4000188CU) /**< \brief (GCLK) Peripheral Clock Control 3 */
#define REG_GCLK_PCHCTRL4          (0x40001890U) /**< \brief (GCLK) Peripheral Clock Control 4 */
#define REG_GCLK_PCHCTRL5          (0x40001894U) /**< \brief (GCLK) Peripheral Clock Control 5 */
#define REG_GCLK_PCHCTRL6          (0x40001898U) /**< \brief (GCLK) Peripheral Clock Control 6 */
#define REG_GCLK_PCHCTRL7          (0x4000189CU) /**< \brief (GCLK) Peripheral Clock Control 7 */
#define REG_GCLK_PCHCTRL8          (0x400018A0U) /**< \brief (GCLK) Peripheral Clock Control 8 */
#define REG_GCLK_PCHCTRL9          (0x400018A4U) /**< \brief (GCLK) Peripheral Clock Control 9 */
#define REG_GCLK_PCHCTRL10         (0x400018A8U) /**< \brief (GCLK) Peripheral Clock Control 10 */
#define REG_GCLK_PCHCTRL11         (0x400018ACU) /**< \brief (GCLK) Peripheral Clock Control 11 */
#define REG_GCLK_PCHCTRL12         (0x400018B0U) /**< \brief (GCLK) Peripheral Clock Control 12 */
#define REG_GCLK_PCHCTRL13         (0x400018B4U) /**< \brief (GCLK) Peripheral Clock Control 13 */
#define REG_GCLK_PCHCTRL14         (0x400018B8U) /**< \brief (GCLK) Peripheral Clock Control 14 */
#define REG_GCLK_PCHCTRL15         (0x400018BCU) /**< \brief (GCLK) Peripheral Clock Control 15 */
#define REG_GCLK_PCHCTRL16         (0x400018C0U) /**< \brief (GCLK) Peripheral Clock Control 16 */
#define REG_GCLK_PCHCTRL17         (0x400018C4U) /**< \brief (GCLK) Peripheral Clock Control 17 */
#define REG_GCLK_PCHCTRL18         (0x400018C8U) /**< \brief (GCLK) Peripheral Clock Control 18 */
#define REG_GCLK_PCHCTRL19         (0x400018CCU) /**< \brief (GCLK) Peripheral Clock Control 19 */
#define REG_GCLK_PCHCTRL20         (0x400018D0U) /**< \brief (GCLK) Peripheral Clock Control 20 */
#define REG_GCLK_PCHCTRL21         (0x400018D4U) /**< \brief (GCLK) Peripheral Clock Control 21 */
#define REG_GCLK_PCHCTRL22         (0x400018D8U) /**< \brief (GCLK) Peripheral Clock Control 22 */
#define REG_GCLK_PCHCTRL23         (0x400018DCU) /**< \brief (GCLK) Peripheral Clock Control 23 */
#define REG_GCLK_PCHCTRL24         (0x400018E0U) /**< \brief (GCLK) Peripheral Clock Control 24 */
#define REG_GCLK_PCHCTRL25         (0x400018E4U) /**< \brief (GCLK) Peripheral Clock Control 25 */
#define REG_GCLK_PCHCTRL26         (0x400018E8U) /**< \brief (GCLK) Peripheral Clock Control 26 */
#define REG_GCLK_PCHCTRL27         (0x400018ECU) /**< \brief (GCLK) Peripheral Clock Control 27 */
#define REG_GCLK_PCHCTRL28         (0x400018F0U) /**< \brief (GCLK) Peripheral Clock Control 28 */
#define REG_GCLK_PCHCTRL29         (0x400018F4U) /**< \brief (GCLK) Peripheral Clock Control 29 */
#define REG_GCLK_PCHCTRL30         (0x400018F8U) /**< \brief (GCLK) Peripheral Clock Control 30 */
#define REG_GCLK_PCHCTRL31         (0x400018FCU) /**< \brief (GCLK) Peripheral Clock Control 31 */
#define REG_GCLK_PCHCTRL32         (0x40001900U) /**< \brief (GCLK) Peripheral Clock Control 32 */
#define REG_GCLK_PCHCTRL33         (0x40001904U) /**< \brief (GCLK) Peripheral Clock Control 33 */
#define REG_GCLK_PCHCTRL34         (0x40001908U) /**< \brief (GCLK) Peripheral Clock Control 34 */
#define REG_GCLK_PCHCTRL35         (0x4000190CU) /**< \brief (GCLK) Peripheral Clock Control 35 */
#else
#define REG_GCLK_CTRLA             (*(RwReg8 *)0x40001800U) /**< \brief (GCLK) Control */
#define REG_GCLK_SYNCBUSY          (*(RoReg  *)0x40001804U) /**< \brief (GCLK) Synchronization Busy */
#define REG_GCLK_GENCTRL0          (*(RwReg  *)0x40001820U) /**< \brief (GCLK) Generic Clock Generator Control 0 */
#define REG_GCLK_GENCTRL1          (*(RwReg  *)0x40001824U) /**< \brief (GCLK) Generic Clock Generator Control 1 */
#define REG_GCLK_GENCTRL2          (*(RwReg  *)0x40001828U) /**< \brief (GCLK) Generic Clock Generator Control 2 */
#define REG_GCLK_GENCTRL3          (*(RwReg  *)0x4000182CU) /**< \brief (GCLK) Generic Clock Generator Control 3 */
#define REG_GCLK_GENCTRL4          (*(RwReg  *)0x40001830U) /**< \brief (GCLK) Generic Clock Generator Control 4 */
#define REG_GCLK_GENCTRL5          (*(RwReg  *)0x40001834U) /**< \brief (GCLK) Generic Clock Generator Control 5 */
#define REG_GCLK_GENCTRL6          (*(RwReg  *)0x40001838U) /**< \brief (GCLK) Generic Clock Generator Control 6 */
#define REG_GCLK_GENCTRL7          (*(RwReg  *)0x4000183CU) /**< \brief (GCLK) Generic Clock Generator Control 7 */
#define REG_GCLK_GENCTRL8          (*(RwReg  *)0x40001840U) /**< \brief (GCLK) Generic Clock Generator Control 8 */
#define REG_GCLK_PCHCTRL0          (*(RwReg  *)0x40001880U) /**< \brief (GCLK) Peripheral Clock Control 0 */
#define REG_GCLK_PCHCTRL1          (*(RwReg  *)0x40001884U) /**< \brief (GCLK) Peripheral Clock Control 1 */
#define REG_GCLK_PCHCTRL2          (*(RwReg  *)0x40001888U) /**< \brief (GCLK) Peripheral Clock Control 2 */
#define REG_GCLK_PCHCTRL3          (*(RwReg  *)0x4000188CU) /**< \brief (GCLK) Peripheral Clock Control 3 */
#define REG_GCLK_PCHCTRL4          (*(RwReg  *)0x40001890U) /**< \brief (GCLK) Peripheral Clock Control 4 */
#define REG_GCLK_PCHCTRL5          (*(RwReg  *)0x40001894U) /**< \brief (GCLK) Peripheral Clock Control 5 */
#define REG_GCLK_PCHCTRL6          (*(RwReg  *)0x40001898U) /**< \brief (GCLK) Peripheral Clock Control 6 */
#define REG_GCLK_PCHCTRL7          (*(RwReg  *)0x4000189CU) /**< \brief (GCLK) Peripheral Clock Control 7 */
#define REG_GCLK_PCHCTRL8          (*(RwReg  *)0x400018A0U) /**< \brief (GCLK) Peripheral Clock Control 8 */
#define REG_GCLK_PCHCTRL9          (*(RwReg  *)0x400018A4U) /**< \brief (GCLK) Peripheral Clock Control 9 */
#define REG_GCLK_PCHCTRL10         (*(RwReg  *)0x400018A8U) /**< \brief (GCLK) Peripheral Clock Control 10 */
#define REG_GCLK_PCHCTRL11         (*(RwReg  *)0x400018ACU) /**< \brief (GCLK) Peripheral Clock Control 11 */
#define REG_GCLK_PCHCTRL12         (*(RwReg  *)0x400018B0U) /**< \brief (GCLK) Peripheral Clock Control 12 */
#define REG_GCLK_PCHCTRL13         (*(RwReg  *)0x400018B4U) /**< \brief (GCLK) Peripheral Clock Control 13 */
#define REG_GCLK_PCHCTRL14         (*(RwReg  *)0x400018B8U) /**< \brief (GCLK) Peripheral Clock Control 14 */
#define REG_GCLK_PCHCTRL15         (*(RwReg  *)0x400018BCU) /**< \brief (GCLK) Peripheral Clock Control 15 */
#define REG_GCLK_PCHCTRL16         (*(RwReg  *)0x400018C0U) /**< \brief (GCLK) Peripheral Clock Control 16 */
#define REG_GCLK_PCHCTRL17         (*(RwReg  *)0x400018C4U) /**< \brief (GCLK) Peripheral Clock Control 17 */
#define REG_GCLK_PCHCTRL18         (*(RwReg  *)0x400018C8U) /**< \brief (GCLK) Peripheral Clock Control 18 */
#define REG_GCLK_PCHCTRL19         (*(RwReg  *)0x400018CCU) /**< \brief (GCLK) Peripheral Clock Control 19 */
#define REG_GCLK_PCHCTRL20         (*(RwReg  *)0x400018D0U) /**< \brief (GCLK) Peripheral Clock Control 20 */
#define REG_GCLK_PCHCTRL21         (*(RwReg  *)0x400018D4U) /**< \brief (GCLK) Peripheral Clock Control 21 */
#define REG_GCLK_PCHCTRL22         (*(RwReg  *)0x400018D8U) /**< \brief (GCLK) Peripheral Clock Control 22 */
#define REG_GCLK_PCHCTRL23         (*(RwReg  *)0x400018DCU) /**< \brief (GCLK) Peripheral Clock Control 23 */
#define REG_GCLK_PCHCTRL24         (*(RwReg  *)0x400018E0U) /**< \brief (GCLK) Peripheral Clock Control 24 */
#define REG_GCLK_PCHCTRL25         (*(RwReg  *)0x400018E4U) /**< \brief (GCLK) Peripheral Clock Control 25 */
#define REG_GCLK_PCHCTRL26         (*(RwReg  *)0x400018E8U) /**< \brief (GCLK) Peripheral Clock Control 26 */
#define REG_GCLK_PCHCTRL27         (*(RwReg  *)0x400018ECU) /**< \brief (GCLK) Peripheral Clock Control 27 */
#define REG_GCLK_PCHCTRL28         (*(RwReg  *)0x400018F0U) /**< \brief (GCLK) Peripheral Clock Control 28 */
#define REG_GCLK_PCHCTRL29         (*(RwReg  *)0x400018F4U) /**< \brief (GCLK) Peripheral Clock Control 29 */
#define REG_GCLK_PCHCTRL30         (*(RwReg  *)0x400018F8U) /**< \brief (GCLK) Peripheral Clock Control 30 */
#define REG_GCLK_PCHCTRL31         (*(RwReg  *)0x400018FCU) /**< \brief (GCLK) Peripheral Clock Control 31 */
#define REG_GCLK_PCHCTRL32         (*(RwReg  *)0x40001900U) /**< \brief (GCLK) Peripheral Clock Control 32 */
#define REG_GCLK_PCHCTRL33         (*(RwReg  *)0x40001904U) /**< \brief (GCLK) Peripheral Clock Control 33 */
#define REG_GCLK_PCHCTRL34         (*(RwReg  *)0x40001908U) /**< \brief (GCLK) Peripheral Clock Control 34 */
#define REG_GCLK_PCHCTRL35         (*(RwReg  *)0x4000190CU) /**< \brief (GCLK) Peripheral Clock Control 35 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for GCLK peripheral ========== */
#define GCLK_GENDIV_BITS            16
#define GCLK_GEN_NUM                9        // Number of Generic Clock Generators
#define GCLK_GEN_NUM_MSB            8        // Number of Generic Clock Generators - 1
#define GCLK_GEN_SOURCE_NUM_MSB     8        // Number of Generic Clock Sources - 1
#define GCLK_NUM                    36       // Number of Generic Clock Users
#define GCLK_SOURCE_BITS            4
#define GCLK_SOURCE_DFLL48M         7
#define GCLK_SOURCE_FDPLL           8
#define GCLK_SOURCE_GCLKGEN1        2
#define GCLK_SOURCE_GCLKIN          1
#define GCLK_SOURCE_NUM             9        // Number of Generic Clock Sources
#define GCLK_SOURCE_OSCULP32K       3
#define GCLK_SOURCE_OSC16M          6
#define GCLK_SOURCE_OSC32K          4
#define GCLK_SOURCE_XOSC            0
#define GCLK_SOURCE_XOSC32K         5

#endif /* _SAML21_GCLK_INSTANCE_ */
