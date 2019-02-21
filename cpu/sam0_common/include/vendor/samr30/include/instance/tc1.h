/**
 * \file
 *
 * \brief Instance description for TC1
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _SAMR30_TC1_INSTANCE_
#define _SAMR30_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC1_CTRLA              (0x42002400U) /**< \brief (TC1) Control A */
#define REG_TC1_CTRLBCLR           (0x42002404U) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (0x42002405U) /**< \brief (TC1) Control B Set */
#define REG_TC1_EVCTRL             (0x42002406U) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (0x42002408U) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (0x42002409U) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (0x4200240AU) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (0x4200240BU) /**< \brief (TC1) Status */
#define REG_TC1_WAVE               (0x4200240CU) /**< \brief (TC1) Waveform Generation Control */
#define REG_TC1_DRVCTRL            (0x4200240DU) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (0x4200240FU) /**< \brief (TC1) Debug Control */
#define REG_TC1_SYNCBUSY           (0x42002410U) /**< \brief (TC1) Synchronization Status */
#define REG_TC1_COUNT16_COUNT      (0x42002414U) /**< \brief (TC1) COUNT16 Count */
#define REG_TC1_COUNT16_CC0        (0x4200241CU) /**< \brief (TC1) COUNT16 Compare and Capture 0 */
#define REG_TC1_COUNT16_CC1        (0x4200241EU) /**< \brief (TC1) COUNT16 Compare and Capture 1 */
#define REG_TC1_COUNT16_CCBUF0     (0x42002430U) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT16_CCBUF1     (0x42002432U) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT32_COUNT      (0x42002414U) /**< \brief (TC1) COUNT32 Count */
#define REG_TC1_COUNT32_CC0        (0x4200241CU) /**< \brief (TC1) COUNT32 Compare and Capture 0 */
#define REG_TC1_COUNT32_CC1        (0x42002420U) /**< \brief (TC1) COUNT32 Compare and Capture 1 */
#define REG_TC1_COUNT32_CCBUF0     (0x42002430U) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT32_CCBUF1     (0x42002434U) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT8_COUNT       (0x42002414U) /**< \brief (TC1) COUNT8 Count */
#define REG_TC1_COUNT8_PER         (0x4200241BU) /**< \brief (TC1) COUNT8 Period */
#define REG_TC1_COUNT8_CC0         (0x4200241CU) /**< \brief (TC1) COUNT8 Compare and Capture 0 */
#define REG_TC1_COUNT8_CC1         (0x4200241DU) /**< \brief (TC1) COUNT8 Compare and Capture 1 */
#define REG_TC1_COUNT8_PERBUF      (0x4200242FU) /**< \brief (TC1) COUNT8 Period Buffer */
#define REG_TC1_COUNT8_CCBUF0      (0x42002430U) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT8_CCBUF1      (0x42002431U) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC1_CTRLA              (*(RwReg  *)0x42002400U) /**< \brief (TC1) Control A */
#define REG_TC1_CTRLBCLR           (*(RwReg8 *)0x42002404U) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (*(RwReg8 *)0x42002405U) /**< \brief (TC1) Control B Set */
#define REG_TC1_EVCTRL             (*(RwReg16*)0x42002406U) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (*(RwReg8 *)0x42002408U) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (*(RwReg8 *)0x42002409U) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (*(RwReg8 *)0x4200240AU) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (*(RwReg8 *)0x4200240BU) /**< \brief (TC1) Status */
#define REG_TC1_WAVE               (*(RwReg8 *)0x4200240CU) /**< \brief (TC1) Waveform Generation Control */
#define REG_TC1_DRVCTRL            (*(RwReg8 *)0x4200240DU) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (*(RwReg8 *)0x4200240FU) /**< \brief (TC1) Debug Control */
#define REG_TC1_SYNCBUSY           (*(RoReg  *)0x42002410U) /**< \brief (TC1) Synchronization Status */
#define REG_TC1_COUNT16_COUNT      (*(RwReg16*)0x42002414U) /**< \brief (TC1) COUNT16 Count */
#define REG_TC1_COUNT16_CC0        (*(RwReg16*)0x4200241CU) /**< \brief (TC1) COUNT16 Compare and Capture 0 */
#define REG_TC1_COUNT16_CC1        (*(RwReg16*)0x4200241EU) /**< \brief (TC1) COUNT16 Compare and Capture 1 */
#define REG_TC1_COUNT16_CCBUF0     (*(RwReg16*)0x42002430U) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT16_CCBUF1     (*(RwReg16*)0x42002432U) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT32_COUNT      (*(RwReg  *)0x42002414U) /**< \brief (TC1) COUNT32 Count */
#define REG_TC1_COUNT32_CC0        (*(RwReg  *)0x4200241CU) /**< \brief (TC1) COUNT32 Compare and Capture 0 */
#define REG_TC1_COUNT32_CC1        (*(RwReg  *)0x42002420U) /**< \brief (TC1) COUNT32 Compare and Capture 1 */
#define REG_TC1_COUNT32_CCBUF0     (*(RwReg  *)0x42002430U) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT32_CCBUF1     (*(RwReg  *)0x42002434U) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT8_COUNT       (*(RwReg8 *)0x42002414U) /**< \brief (TC1) COUNT8 Count */
#define REG_TC1_COUNT8_PER         (*(RwReg8 *)0x4200241BU) /**< \brief (TC1) COUNT8 Period */
#define REG_TC1_COUNT8_CC0         (*(RwReg8 *)0x4200241CU) /**< \brief (TC1) COUNT8 Compare and Capture 0 */
#define REG_TC1_COUNT8_CC1         (*(RwReg8 *)0x4200241DU) /**< \brief (TC1) COUNT8 Compare and Capture 1 */
#define REG_TC1_COUNT8_PERBUF      (*(RwReg8 *)0x4200242FU) /**< \brief (TC1) COUNT8 Period Buffer */
#define REG_TC1_COUNT8_CCBUF0      (*(RwReg8 *)0x42002430U) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT8_CCBUF1      (*(RwReg8 *)0x42002431U) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 1 */
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

#endif /* _SAMR30_TC1_INSTANCE_ */
