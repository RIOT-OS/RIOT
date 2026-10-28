/**
 * \file
 *
 * \brief Instance description for ADC
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

#ifndef _SAMR30_ADC_INSTANCE_
#define _SAMR30_ADC_INSTANCE_

/* ========== Register definition for ADC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ADC_CTRLA              (0x43000C00U) /**< \brief (ADC) Control A */
#define REG_ADC_CTRLB              (0x43000C01U) /**< \brief (ADC) Control B */
#define REG_ADC_REFCTRL            (0x43000C02U) /**< \brief (ADC) Reference Control */
#define REG_ADC_EVCTRL             (0x43000C03U) /**< \brief (ADC) Event Control */
#define REG_ADC_INTENCLR           (0x43000C04U) /**< \brief (ADC) Interrupt Enable Clear */
#define REG_ADC_INTENSET           (0x43000C05U) /**< \brief (ADC) Interrupt Enable Set */
#define REG_ADC_INTFLAG            (0x43000C06U) /**< \brief (ADC) Interrupt Flag Status and Clear */
#define REG_ADC_SEQSTATUS          (0x43000C07U) /**< \brief (ADC) Sequence Status */
#define REG_ADC_INPUTCTRL          (0x43000C08U) /**< \brief (ADC) Input Control */
#define REG_ADC_CTRLC              (0x43000C0AU) /**< \brief (ADC) Control C */
#define REG_ADC_AVGCTRL            (0x43000C0CU) /**< \brief (ADC) Average Control */
#define REG_ADC_SAMPCTRL           (0x43000C0DU) /**< \brief (ADC) Sample Time Control */
#define REG_ADC_WINLT              (0x43000C0EU) /**< \brief (ADC) Window Monitor Lower Threshold */
#define REG_ADC_WINUT              (0x43000C10U) /**< \brief (ADC) Window Monitor Upper Threshold */
#define REG_ADC_GAINCORR           (0x43000C12U) /**< \brief (ADC) Gain Correction */
#define REG_ADC_OFFSETCORR         (0x43000C14U) /**< \brief (ADC) Offset Correction */
#define REG_ADC_SWTRIG             (0x43000C18U) /**< \brief (ADC) Software Trigger */
#define REG_ADC_DBGCTRL            (0x43000C1CU) /**< \brief (ADC) Debug Control */
#define REG_ADC_SYNCBUSY           (0x43000C20U) /**< \brief (ADC) Synchronization Busy */
#define REG_ADC_RESULT             (0x43000C24U) /**< \brief (ADC) Result */
#define REG_ADC_SEQCTRL            (0x43000C28U) /**< \brief (ADC) Sequence Control */
#define REG_ADC_CALIB              (0x43000C2CU) /**< \brief (ADC) Calibration */
#else
#define REG_ADC_CTRLA              (*(RwReg8 *)0x43000C00U) /**< \brief (ADC) Control A */
#define REG_ADC_CTRLB              (*(RwReg8 *)0x43000C01U) /**< \brief (ADC) Control B */
#define REG_ADC_REFCTRL            (*(RwReg8 *)0x43000C02U) /**< \brief (ADC) Reference Control */
#define REG_ADC_EVCTRL             (*(RwReg8 *)0x43000C03U) /**< \brief (ADC) Event Control */
#define REG_ADC_INTENCLR           (*(RwReg8 *)0x43000C04U) /**< \brief (ADC) Interrupt Enable Clear */
#define REG_ADC_INTENSET           (*(RwReg8 *)0x43000C05U) /**< \brief (ADC) Interrupt Enable Set */
#define REG_ADC_INTFLAG            (*(RwReg8 *)0x43000C06U) /**< \brief (ADC) Interrupt Flag Status and Clear */
#define REG_ADC_SEQSTATUS          (*(RoReg8 *)0x43000C07U) /**< \brief (ADC) Sequence Status */
#define REG_ADC_INPUTCTRL          (*(RwReg16*)0x43000C08U) /**< \brief (ADC) Input Control */
#define REG_ADC_CTRLC              (*(RwReg16*)0x43000C0AU) /**< \brief (ADC) Control C */
#define REG_ADC_AVGCTRL            (*(RwReg8 *)0x43000C0CU) /**< \brief (ADC) Average Control */
#define REG_ADC_SAMPCTRL           (*(RwReg8 *)0x43000C0DU) /**< \brief (ADC) Sample Time Control */
#define REG_ADC_WINLT              (*(RwReg16*)0x43000C0EU) /**< \brief (ADC) Window Monitor Lower Threshold */
#define REG_ADC_WINUT              (*(RwReg16*)0x43000C10U) /**< \brief (ADC) Window Monitor Upper Threshold */
#define REG_ADC_GAINCORR           (*(RwReg16*)0x43000C12U) /**< \brief (ADC) Gain Correction */
#define REG_ADC_OFFSETCORR         (*(RwReg16*)0x43000C14U) /**< \brief (ADC) Offset Correction */
#define REG_ADC_SWTRIG             (*(RwReg8 *)0x43000C18U) /**< \brief (ADC) Software Trigger */
#define REG_ADC_DBGCTRL            (*(RwReg8 *)0x43000C1CU) /**< \brief (ADC) Debug Control */
#define REG_ADC_SYNCBUSY           (*(RoReg16*)0x43000C20U) /**< \brief (ADC) Synchronization Busy */
#define REG_ADC_RESULT             (*(RoReg16*)0x43000C24U) /**< \brief (ADC) Result */
#define REG_ADC_SEQCTRL            (*(RwReg  *)0x43000C28U) /**< \brief (ADC) Sequence Control */
#define REG_ADC_CALIB              (*(RwReg16*)0x43000C2CU) /**< \brief (ADC) Calibration */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for ADC peripheral ========== */
#define ADC_DMAC_ID_RESRDY          37       // index of DMA RESRDY trigger
#define ADC_EXTCHANNEL_MSB          19       // Number of external channels
#define ADC_GCLK_ID                 30       // index of Generic Clock
#define ADC_RESULT_BITS             16       // Size of RESULT.RESULT bitfield
#define ADC_RESULT_MSB              15       // Size of Result

#endif /* _SAMR30_ADC_INSTANCE_ */
