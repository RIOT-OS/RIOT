/**
 * \file
 *
 * \brief Instance description for TC1
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

#ifndef _SAML21_TC1_INSTANCE_
#define _SAML21_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC1_CTRLA              (0x42002400) /**< \brief (TC1) Control A */
#define REG_TC1_CTRLBCLR           (0x42002404) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (0x42002405) /**< \brief (TC1) Control B Set */
#define REG_TC1_EVCTRL             (0x42002406) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (0x42002408) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (0x42002409) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (0x4200240A) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (0x4200240B) /**< \brief (TC1) Status */
#define REG_TC1_WAVE               (0x4200240C) /**< \brief (TC1) Waveform Generation Control */
#define REG_TC1_DRVCTRL            (0x4200240D) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (0x4200240F) /**< \brief (TC1) Debug Control */
#define REG_TC1_SYNCBUSY           (0x42002410) /**< \brief (TC1) Synchronization Status */
#define REG_TC1_COUNT16_COUNT      (0x42002414) /**< \brief (TC1) COUNT16 Count */
#define REG_TC1_COUNT16_CC0        (0x4200241C) /**< \brief (TC1) COUNT16 Compare and Capture 0 */
#define REG_TC1_COUNT16_CC1        (0x4200241E) /**< \brief (TC1) COUNT16 Compare and Capture 1 */
#define REG_TC1_COUNT16_CCBUF0     (0x42002430) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT16_CCBUF1     (0x42002432) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT32_COUNT      (0x42002414) /**< \brief (TC1) COUNT32 Count */
#define REG_TC1_COUNT32_CC0        (0x4200241C) /**< \brief (TC1) COUNT32 Compare and Capture 0 */
#define REG_TC1_COUNT32_CC1        (0x42002420) /**< \brief (TC1) COUNT32 Compare and Capture 1 */
#define REG_TC1_COUNT32_CCBUF0     (0x42002430) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT32_CCBUF1     (0x42002434) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT8_COUNT       (0x42002414) /**< \brief (TC1) COUNT8 Count */
#define REG_TC1_COUNT8_PER         (0x4200241B) /**< \brief (TC1) COUNT8 Period */
#define REG_TC1_COUNT8_CC0         (0x4200241C) /**< \brief (TC1) COUNT8 Compare and Capture 0 */
#define REG_TC1_COUNT8_CC1         (0x4200241D) /**< \brief (TC1) COUNT8 Compare and Capture 1 */
#define REG_TC1_COUNT8_PERBUF      (0x4200242F) /**< \brief (TC1) COUNT8 Period Buffer */
#define REG_TC1_COUNT8_CCBUF0      (0x42002430) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT8_CCBUF1      (0x42002431) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC1_CTRLA              (*(RwReg  *)0x42002400UL) /**< \brief (TC1) Control A */
#define REG_TC1_CTRLBCLR           (*(RwReg8 *)0x42002404UL) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (*(RwReg8 *)0x42002405UL) /**< \brief (TC1) Control B Set */
#define REG_TC1_EVCTRL             (*(RwReg16*)0x42002406UL) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (*(RwReg8 *)0x42002408UL) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (*(RwReg8 *)0x42002409UL) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (*(RwReg8 *)0x4200240AUL) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (*(RwReg8 *)0x4200240BUL) /**< \brief (TC1) Status */
#define REG_TC1_WAVE               (*(RwReg8 *)0x4200240CUL) /**< \brief (TC1) Waveform Generation Control */
#define REG_TC1_DRVCTRL            (*(RwReg8 *)0x4200240DUL) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (*(RwReg8 *)0x4200240FUL) /**< \brief (TC1) Debug Control */
#define REG_TC1_SYNCBUSY           (*(RoReg  *)0x42002410UL) /**< \brief (TC1) Synchronization Status */
#define REG_TC1_COUNT16_COUNT      (*(RwReg16*)0x42002414UL) /**< \brief (TC1) COUNT16 Count */
#define REG_TC1_COUNT16_CC0        (*(RwReg16*)0x4200241CUL) /**< \brief (TC1) COUNT16 Compare and Capture 0 */
#define REG_TC1_COUNT16_CC1        (*(RwReg16*)0x4200241EUL) /**< \brief (TC1) COUNT16 Compare and Capture 1 */
#define REG_TC1_COUNT16_CCBUF0     (*(RwReg16*)0x42002430UL) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT16_CCBUF1     (*(RwReg16*)0x42002432UL) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT32_COUNT      (*(RwReg  *)0x42002414UL) /**< \brief (TC1) COUNT32 Count */
#define REG_TC1_COUNT32_CC0        (*(RwReg  *)0x4200241CUL) /**< \brief (TC1) COUNT32 Compare and Capture 0 */
#define REG_TC1_COUNT32_CC1        (*(RwReg  *)0x42002420UL) /**< \brief (TC1) COUNT32 Compare and Capture 1 */
#define REG_TC1_COUNT32_CCBUF0     (*(RwReg  *)0x42002430UL) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT32_CCBUF1     (*(RwReg  *)0x42002434UL) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT8_COUNT       (*(RwReg8 *)0x42002414UL) /**< \brief (TC1) COUNT8 Count */
#define REG_TC1_COUNT8_PER         (*(RwReg8 *)0x4200241BUL) /**< \brief (TC1) COUNT8 Period */
#define REG_TC1_COUNT8_CC0         (*(RwReg8 *)0x4200241CUL) /**< \brief (TC1) COUNT8 Compare and Capture 0 */
#define REG_TC1_COUNT8_CC1         (*(RwReg8 *)0x4200241DUL) /**< \brief (TC1) COUNT8 Compare and Capture 1 */
#define REG_TC1_COUNT8_PERBUF      (*(RwReg8 *)0x4200242FUL) /**< \brief (TC1) COUNT8 Period Buffer */
#define REG_TC1_COUNT8_CCBUF0      (*(RwReg8 *)0x42002430UL) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT8_CCBUF1      (*(RwReg8 *)0x42002431UL) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC1 peripheral ========== */
#define TC1_CC_NUM                  2       
#define TC1_DMAC_ID_MC_0            26
#define TC1_DMAC_ID_MC_1            27
#define TC1_DMAC_ID_MC_LSB          26
#define TC1_DMAC_ID_MC_MSB          27
#define TC1_DMAC_ID_MC_SIZE         2
#define TC1_DMAC_ID_OVF             25       // Indexes of DMA Overflow trigger
#define TC1_EXT                     0       
#define TC1_GCLK_ID                 27      
#define TC1_MASTER                  0       
#define TC1_OW_NUM                  2       

#endif /* _SAML21_TC1_INSTANCE_ */
