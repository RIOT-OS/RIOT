/**
 * \file
 *
 * \brief Instance description for AC1
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

#ifndef _SAMD21_AC1_INSTANCE_
#define _SAMD21_AC1_INSTANCE_

/* ========== Register definition for AC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AC1_CTRLA              (0x42005400) /**< \brief (AC1) Control A */
#define REG_AC1_CTRLB              (0x42005401) /**< \brief (AC1) Control B */
#define REG_AC1_EVCTRL             (0x42005402) /**< \brief (AC1) Event Control */
#define REG_AC1_INTENCLR           (0x42005404) /**< \brief (AC1) Interrupt Enable Clear */
#define REG_AC1_INTENSET           (0x42005405) /**< \brief (AC1) Interrupt Enable Set */
#define REG_AC1_INTFLAG            (0x42005406) /**< \brief (AC1) Interrupt Flag Status and Clear */
#define REG_AC1_STATUSA            (0x42005408) /**< \brief (AC1) Status A */
#define REG_AC1_STATUSB            (0x42005409) /**< \brief (AC1) Status B */
#define REG_AC1_STATUSC            (0x4200540A) /**< \brief (AC1) Status C */
#define REG_AC1_WINCTRL            (0x4200540C) /**< \brief (AC1) Window Control */
#define REG_AC1_COMPCTRL0          (0x42005410) /**< \brief (AC1) Comparator Control 0 */
#define REG_AC1_COMPCTRL1          (0x42005414) /**< \brief (AC1) Comparator Control 1 */
#define REG_AC1_SCALER0            (0x42005420) /**< \brief (AC1) Scaler 0 */
#define REG_AC1_SCALER1            (0x42005421) /**< \brief (AC1) Scaler 1 */
#else
#define REG_AC1_CTRLA              (*(RwReg8 *)0x42005400UL) /**< \brief (AC1) Control A */
#define REG_AC1_CTRLB              (*(WoReg8 *)0x42005401UL) /**< \brief (AC1) Control B */
#define REG_AC1_EVCTRL             (*(RwReg16*)0x42005402UL) /**< \brief (AC1) Event Control */
#define REG_AC1_INTENCLR           (*(RwReg8 *)0x42005404UL) /**< \brief (AC1) Interrupt Enable Clear */
#define REG_AC1_INTENSET           (*(RwReg8 *)0x42005405UL) /**< \brief (AC1) Interrupt Enable Set */
#define REG_AC1_INTFLAG            (*(RwReg8 *)0x42005406UL) /**< \brief (AC1) Interrupt Flag Status and Clear */
#define REG_AC1_STATUSA            (*(RoReg8 *)0x42005408UL) /**< \brief (AC1) Status A */
#define REG_AC1_STATUSB            (*(RoReg8 *)0x42005409UL) /**< \brief (AC1) Status B */
#define REG_AC1_STATUSC            (*(RoReg8 *)0x4200540AUL) /**< \brief (AC1) Status C */
#define REG_AC1_WINCTRL            (*(RwReg8 *)0x4200540CUL) /**< \brief (AC1) Window Control */
#define REG_AC1_COMPCTRL0          (*(RwReg  *)0x42005410UL) /**< \brief (AC1) Comparator Control 0 */
#define REG_AC1_COMPCTRL1          (*(RwReg  *)0x42005414UL) /**< \brief (AC1) Comparator Control 1 */
#define REG_AC1_SCALER0            (*(RwReg8 *)0x42005420UL) /**< \brief (AC1) Scaler 0 */
#define REG_AC1_SCALER1            (*(RwReg8 *)0x42005421UL) /**< \brief (AC1) Scaler 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AC1 peripheral ========== */
#define AC1_CMP_NUM                 2        // Number of comparators
#define AC1_GCLK_ID_ANA             32       // Index of Generic Clock for analog
#define AC1_GCLK_ID_DIG             31       // Index of Generic Clock for digital
#define AC1_NUM_CMP                 2       
#define AC1_PAIRS                   1        // Number of pairs of comparators

#endif /* _SAMD21_AC1_INSTANCE_ */
