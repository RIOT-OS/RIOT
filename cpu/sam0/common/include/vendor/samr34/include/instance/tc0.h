/**
 * \file
 *
 * \brief Instance description for TC0
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

#ifndef _SAMR34_TC0_INSTANCE_
#define _SAMR34_TC0_INSTANCE_

/* ========== Register definition for TC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC0_CTRLA              (0x42002000) /**< \brief (TC0) Control A */
#define REG_TC0_CTRLBCLR           (0x42002004) /**< \brief (TC0) Control B Clear */
#define REG_TC0_CTRLBSET           (0x42002005) /**< \brief (TC0) Control B Set */
#define REG_TC0_EVCTRL             (0x42002006) /**< \brief (TC0) Event Control */
#define REG_TC0_INTENCLR           (0x42002008) /**< \brief (TC0) Interrupt Enable Clear */
#define REG_TC0_INTENSET           (0x42002009) /**< \brief (TC0) Interrupt Enable Set */
#define REG_TC0_INTFLAG            (0x4200200A) /**< \brief (TC0) Interrupt Flag Status and Clear */
#define REG_TC0_STATUS             (0x4200200B) /**< \brief (TC0) Status */
#define REG_TC0_WAVE               (0x4200200C) /**< \brief (TC0) Waveform Generation Control */
#define REG_TC0_DRVCTRL            (0x4200200D) /**< \brief (TC0) Control C */
#define REG_TC0_DBGCTRL            (0x4200200F) /**< \brief (TC0) Debug Control */
#define REG_TC0_SYNCBUSY           (0x42002010) /**< \brief (TC0) Synchronization Status */
#define REG_TC0_COUNT16_COUNT      (0x42002014) /**< \brief (TC0) COUNT16 Count */
#define REG_TC0_COUNT16_CC0        (0x4200201C) /**< \brief (TC0) COUNT16 Compare and Capture 0 */
#define REG_TC0_COUNT16_CC1        (0x4200201E) /**< \brief (TC0) COUNT16 Compare and Capture 1 */
#define REG_TC0_COUNT16_CCBUF0     (0x42002030) /**< \brief (TC0) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC0_COUNT16_CCBUF1     (0x42002032) /**< \brief (TC0) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC0_COUNT32_COUNT      (0x42002014) /**< \brief (TC0) COUNT32 Count */
#define REG_TC0_COUNT32_CC0        (0x4200201C) /**< \brief (TC0) COUNT32 Compare and Capture 0 */
#define REG_TC0_COUNT32_CC1        (0x42002020) /**< \brief (TC0) COUNT32 Compare and Capture 1 */
#define REG_TC0_COUNT32_CCBUF0     (0x42002030) /**< \brief (TC0) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC0_COUNT32_CCBUF1     (0x42002034) /**< \brief (TC0) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC0_COUNT8_COUNT       (0x42002014) /**< \brief (TC0) COUNT8 Count */
#define REG_TC0_COUNT8_PER         (0x4200201B) /**< \brief (TC0) COUNT8 Period */
#define REG_TC0_COUNT8_CC0         (0x4200201C) /**< \brief (TC0) COUNT8 Compare and Capture 0 */
#define REG_TC0_COUNT8_CC1         (0x4200201D) /**< \brief (TC0) COUNT8 Compare and Capture 1 */
#define REG_TC0_COUNT8_PERBUF      (0x4200202F) /**< \brief (TC0) COUNT8 Period Buffer */
#define REG_TC0_COUNT8_CCBUF0      (0x42002030) /**< \brief (TC0) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC0_COUNT8_CCBUF1      (0x42002031) /**< \brief (TC0) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC0_CTRLA              (*(RwReg  *)0x42002000UL) /**< \brief (TC0) Control A */
#define REG_TC0_CTRLBCLR           (*(RwReg8 *)0x42002004UL) /**< \brief (TC0) Control B Clear */
#define REG_TC0_CTRLBSET           (*(RwReg8 *)0x42002005UL) /**< \brief (TC0) Control B Set */
#define REG_TC0_EVCTRL             (*(RwReg16*)0x42002006UL) /**< \brief (TC0) Event Control */
#define REG_TC0_INTENCLR           (*(RwReg8 *)0x42002008UL) /**< \brief (TC0) Interrupt Enable Clear */
#define REG_TC0_INTENSET           (*(RwReg8 *)0x42002009UL) /**< \brief (TC0) Interrupt Enable Set */
#define REG_TC0_INTFLAG            (*(RwReg8 *)0x4200200AUL) /**< \brief (TC0) Interrupt Flag Status and Clear */
#define REG_TC0_STATUS             (*(RwReg8 *)0x4200200BUL) /**< \brief (TC0) Status */
#define REG_TC0_WAVE               (*(RwReg8 *)0x4200200CUL) /**< \brief (TC0) Waveform Generation Control */
#define REG_TC0_DRVCTRL            (*(RwReg8 *)0x4200200DUL) /**< \brief (TC0) Control C */
#define REG_TC0_DBGCTRL            (*(RwReg8 *)0x4200200FUL) /**< \brief (TC0) Debug Control */
#define REG_TC0_SYNCBUSY           (*(RoReg  *)0x42002010UL) /**< \brief (TC0) Synchronization Status */
#define REG_TC0_COUNT16_COUNT      (*(RwReg16*)0x42002014UL) /**< \brief (TC0) COUNT16 Count */
#define REG_TC0_COUNT16_CC0        (*(RwReg16*)0x4200201CUL) /**< \brief (TC0) COUNT16 Compare and Capture 0 */
#define REG_TC0_COUNT16_CC1        (*(RwReg16*)0x4200201EUL) /**< \brief (TC0) COUNT16 Compare and Capture 1 */
#define REG_TC0_COUNT16_CCBUF0     (*(RwReg16*)0x42002030UL) /**< \brief (TC0) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC0_COUNT16_CCBUF1     (*(RwReg16*)0x42002032UL) /**< \brief (TC0) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC0_COUNT32_COUNT      (*(RwReg  *)0x42002014UL) /**< \brief (TC0) COUNT32 Count */
#define REG_TC0_COUNT32_CC0        (*(RwReg  *)0x4200201CUL) /**< \brief (TC0) COUNT32 Compare and Capture 0 */
#define REG_TC0_COUNT32_CC1        (*(RwReg  *)0x42002020UL) /**< \brief (TC0) COUNT32 Compare and Capture 1 */
#define REG_TC0_COUNT32_CCBUF0     (*(RwReg  *)0x42002030UL) /**< \brief (TC0) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC0_COUNT32_CCBUF1     (*(RwReg  *)0x42002034UL) /**< \brief (TC0) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC0_COUNT8_COUNT       (*(RwReg8 *)0x42002014UL) /**< \brief (TC0) COUNT8 Count */
#define REG_TC0_COUNT8_PER         (*(RwReg8 *)0x4200201BUL) /**< \brief (TC0) COUNT8 Period */
#define REG_TC0_COUNT8_CC0         (*(RwReg8 *)0x4200201CUL) /**< \brief (TC0) COUNT8 Compare and Capture 0 */
#define REG_TC0_COUNT8_CC1         (*(RwReg8 *)0x4200201DUL) /**< \brief (TC0) COUNT8 Compare and Capture 1 */
#define REG_TC0_COUNT8_PERBUF      (*(RwReg8 *)0x4200202FUL) /**< \brief (TC0) COUNT8 Period Buffer */
#define REG_TC0_COUNT8_CCBUF0      (*(RwReg8 *)0x42002030UL) /**< \brief (TC0) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC0_COUNT8_CCBUF1      (*(RwReg8 *)0x42002031UL) /**< \brief (TC0) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC0 peripheral ========== */
#define TC0_CC_NUM                  2       
#define TC0_DMAC_ID_MC_0            23
#define TC0_DMAC_ID_MC_1            24
#define TC0_DMAC_ID_MC_LSB          23
#define TC0_DMAC_ID_MC_MSB          24
#define TC0_DMAC_ID_MC_SIZE         2
#define TC0_DMAC_ID_OVF             22       // Indexes of DMA Overflow trigger
#define TC0_EXT                     0       
#define TC0_GCLK_ID                 27      
#define TC0_MASTER                  1       
#define TC0_OW_NUM                  2       

#endif /* _SAMR34_TC0_INSTANCE_ */
