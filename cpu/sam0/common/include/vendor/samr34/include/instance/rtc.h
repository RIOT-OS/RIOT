/**
 * \file
 *
 * \brief Instance description for RTC
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

#ifndef _SAMR34_RTC_INSTANCE_
#define _SAMR34_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RTC_DBGCTRL            (0x4000200E) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (0x40002014) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_GP0                (0x40002040) /**< \brief (RTC) General Purpose 0 */
#define REG_RTC_GP1                (0x40002044) /**< \brief (RTC) General Purpose 1 */
#define REG_RTC_GP2                (0x40002048) /**< \brief (RTC) General Purpose 2 */
#define REG_RTC_GP3                (0x4000204C) /**< \brief (RTC) General Purpose 3 */
#define REG_RTC_MODE0_CTRLA        (0x40002000) /**< \brief (RTC) MODE0 Control A */
#define REG_RTC_MODE0_EVCTRL       (0x40002004) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (0x40002008) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (0x4000200A) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (0x4000200C) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_SYNCBUSY     (0x40002010) /**< \brief (RTC) MODE0 Synchronization Busy Status */
#define REG_RTC_MODE0_COUNT        (0x40002018) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (0x40002020) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE1_CTRLA        (0x40002000) /**< \brief (RTC) MODE1 Control A */
#define REG_RTC_MODE1_EVCTRL       (0x40002004) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (0x40002008) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (0x4000200A) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (0x4000200C) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_SYNCBUSY     (0x40002010) /**< \brief (RTC) MODE1 Synchronization Busy Status */
#define REG_RTC_MODE1_COUNT        (0x40002018) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (0x4000201C) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (0x40002020) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (0x40002022) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE2_CTRLA        (0x40002000) /**< \brief (RTC) MODE2 Control A */
#define REG_RTC_MODE2_EVCTRL       (0x40002004) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (0x40002008) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (0x4000200A) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (0x4000200C) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_SYNCBUSY     (0x40002010) /**< \brief (RTC) MODE2 Synchronization Busy Status */
#define REG_RTC_MODE2_CLOCK        (0x40002018) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_ALARM_ALARM0 (0x40002020) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (0x40002024) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#else
#define REG_RTC_DBGCTRL            (*(RwReg8 *)0x4000200EUL) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (*(RwReg8 *)0x40002014UL) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_GP0                (*(RwReg  *)0x40002040UL) /**< \brief (RTC) General Purpose 0 */
#define REG_RTC_GP1                (*(RwReg  *)0x40002044UL) /**< \brief (RTC) General Purpose 1 */
#define REG_RTC_GP2                (*(RwReg  *)0x40002048UL) /**< \brief (RTC) General Purpose 2 */
#define REG_RTC_GP3                (*(RwReg  *)0x4000204CUL) /**< \brief (RTC) General Purpose 3 */
#define REG_RTC_MODE0_CTRLA        (*(RwReg16*)0x40002000UL) /**< \brief (RTC) MODE0 Control A */
#define REG_RTC_MODE0_EVCTRL       (*(RwReg  *)0x40002004UL) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (*(RwReg16*)0x40002008UL) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (*(RwReg16*)0x4000200AUL) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (*(RwReg16*)0x4000200CUL) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_SYNCBUSY     (*(RoReg  *)0x40002010UL) /**< \brief (RTC) MODE0 Synchronization Busy Status */
#define REG_RTC_MODE0_COUNT        (*(RwReg  *)0x40002018UL) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (*(RwReg  *)0x40002020UL) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE1_CTRLA        (*(RwReg16*)0x40002000UL) /**< \brief (RTC) MODE1 Control A */
#define REG_RTC_MODE1_EVCTRL       (*(RwReg  *)0x40002004UL) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (*(RwReg16*)0x40002008UL) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (*(RwReg16*)0x4000200AUL) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (*(RwReg16*)0x4000200CUL) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_SYNCBUSY     (*(RoReg  *)0x40002010UL) /**< \brief (RTC) MODE1 Synchronization Busy Status */
#define REG_RTC_MODE1_COUNT        (*(RwReg16*)0x40002018UL) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (*(RwReg16*)0x4000201CUL) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (*(RwReg16*)0x40002020UL) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (*(RwReg16*)0x40002022UL) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE2_CTRLA        (*(RwReg16*)0x40002000UL) /**< \brief (RTC) MODE2 Control A */
#define REG_RTC_MODE2_EVCTRL       (*(RwReg  *)0x40002004UL) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (*(RwReg16*)0x40002008UL) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (*(RwReg16*)0x4000200AUL) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (*(RwReg16*)0x4000200CUL) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_SYNCBUSY     (*(RoReg  *)0x40002010UL) /**< \brief (RTC) MODE2 Synchronization Busy Status */
#define REG_RTC_MODE2_CLOCK        (*(RwReg  *)0x40002018UL) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_ALARM_ALARM0 (*(RwReg  *)0x40002020UL) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (*(RwReg8 *)0x40002024UL) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RTC peripheral ========== */
#define RTC_ALARM_NUM               1        // Number of Alarms
#define RTC_COMP16_NUM              2        // Number of 16-bit Comparators
#define RTC_COMP32_NUM              1        // Number of 32-bit Comparators
#define RTC_GPR_NUM                 4        // Number of General-Purpose Registers
#define RTC_PER_NUM                 8        // Number of Periodic Intervals

#endif /* _SAMR34_RTC_INSTANCE_ */
