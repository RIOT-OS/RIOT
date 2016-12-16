/**
 * \file
 *
 * \brief Instance description for RTC
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

#ifndef _SAML21_RTC_INSTANCE_
#define _SAML21_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RTC_DBGCTRL            (0x4000200EU) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (0x40002014U) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_GP0                (0x40002040U) /**< \brief (RTC) General Purpose 0 */
#define REG_RTC_GP1                (0x40002044U) /**< \brief (RTC) General Purpose 1 */
#define REG_RTC_GP2                (0x40002048U) /**< \brief (RTC) General Purpose 2 */
#define REG_RTC_GP3                (0x4000204CU) /**< \brief (RTC) General Purpose 3 */
#define REG_RTC_MODE0_CTRLA        (0x40002000U) /**< \brief (RTC) MODE0 Control A */
#define REG_RTC_MODE0_EVCTRL       (0x40002004U) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (0x40002008U) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (0x4000200AU) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (0x4000200CU) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_SYNCBUSY     (0x40002010U) /**< \brief (RTC) MODE0 Synchronization Busy Status */
#define REG_RTC_MODE0_COUNT        (0x40002018U) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (0x40002020U) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE1_CTRLA        (0x40002000U) /**< \brief (RTC) MODE1 Control A */
#define REG_RTC_MODE1_EVCTRL       (0x40002004U) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (0x40002008U) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (0x4000200AU) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (0x4000200CU) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_SYNCBUSY     (0x40002010U) /**< \brief (RTC) MODE1 Synchronization Busy Status */
#define REG_RTC_MODE1_COUNT        (0x40002018U) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (0x4000201CU) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (0x40002020U) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (0x40002022U) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE2_CTRLA        (0x40002000U) /**< \brief (RTC) MODE2 Control A */
#define REG_RTC_MODE2_EVCTRL       (0x40002004U) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (0x40002008U) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (0x4000200AU) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (0x4000200CU) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_SYNCBUSY     (0x40002010U) /**< \brief (RTC) MODE2 Synchronization Busy Status */
#define REG_RTC_MODE2_CLOCK        (0x40002018U) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_ALARM_ALARM0 (0x40002020U) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (0x40002024U) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#else
#define REG_RTC_DBGCTRL            (*(RwReg8 *)0x4000200EU) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (*(RwReg8 *)0x40002014U) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_GP0                (*(RwReg  *)0x40002040U) /**< \brief (RTC) General Purpose 0 */
#define REG_RTC_GP1                (*(RwReg  *)0x40002044U) /**< \brief (RTC) General Purpose 1 */
#define REG_RTC_GP2                (*(RwReg  *)0x40002048U) /**< \brief (RTC) General Purpose 2 */
#define REG_RTC_GP3                (*(RwReg  *)0x4000204CU) /**< \brief (RTC) General Purpose 3 */
#define REG_RTC_MODE0_CTRLA        (*(RwReg16*)0x40002000U) /**< \brief (RTC) MODE0 Control A */
#define REG_RTC_MODE0_EVCTRL       (*(RwReg  *)0x40002004U) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (*(RwReg16*)0x40002008U) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (*(RwReg16*)0x4000200AU) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (*(RwReg16*)0x4000200CU) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_SYNCBUSY     (*(RoReg  *)0x40002010U) /**< \brief (RTC) MODE0 Synchronization Busy Status */
#define REG_RTC_MODE0_COUNT        (*(RwReg  *)0x40002018U) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (*(RwReg  *)0x40002020U) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE1_CTRLA        (*(RwReg16*)0x40002000U) /**< \brief (RTC) MODE1 Control A */
#define REG_RTC_MODE1_EVCTRL       (*(RwReg  *)0x40002004U) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (*(RwReg16*)0x40002008U) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (*(RwReg16*)0x4000200AU) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (*(RwReg16*)0x4000200CU) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_SYNCBUSY     (*(RoReg  *)0x40002010U) /**< \brief (RTC) MODE1 Synchronization Busy Status */
#define REG_RTC_MODE1_COUNT        (*(RwReg16*)0x40002018U) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (*(RwReg16*)0x4000201CU) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (*(RwReg16*)0x40002020U) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (*(RwReg16*)0x40002022U) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE2_CTRLA        (*(RwReg16*)0x40002000U) /**< \brief (RTC) MODE2 Control A */
#define REG_RTC_MODE2_EVCTRL       (*(RwReg  *)0x40002004U) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (*(RwReg16*)0x40002008U) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (*(RwReg16*)0x4000200AU) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (*(RwReg16*)0x4000200CU) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_SYNCBUSY     (*(RoReg  *)0x40002010U) /**< \brief (RTC) MODE2 Synchronization Busy Status */
#define REG_RTC_MODE2_CLOCK        (*(RwReg  *)0x40002018U) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_ALARM_ALARM0 (*(RwReg  *)0x40002020U) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (*(RwReg  *)0x40002024U) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RTC peripheral ========== */
#define RTC_ALARM_NUM               1        // Number of Alarms
#define RTC_COMP16_NUM              2        // Number of 16-bit Comparators
#define RTC_COMP32_NUM              1        // Number of 32-bit Comparators
#define RTC_GPR_NUM                 4        // Number of General-Purpose Registers
#define RTC_PER_NUM                 8        // Number of Periodic Intervals

#endif /* _SAML21_RTC_INSTANCE_ */
