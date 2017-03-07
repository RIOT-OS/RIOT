/**
 * \file
 *
 * \brief Instance description for PM
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#ifndef _SAML21_PM_INSTANCE_
#define _SAML21_PM_INSTANCE_

/* ========== Register definition for PM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PM_CTRLA               (0x40000000U) /**< \brief (PM) Control A */
#define REG_PM_SLEEPCFG            (0x40000001U) /**< \brief (PM) Sleep Configuration */
#define REG_PM_PLCFG               (0x40000002U) /**< \brief (PM) Performance Level Configuration */
#define REG_PM_INTENCLR            (0x40000004U) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (0x40000005U) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (0x40000006U) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_STDBYCFG            (0x40000008U) /**< \brief (PM) Standby Configuration */
#define REG_PM_PWSAKDLY            (0x4000000CU) /**< \brief (PM) Power Switch Acknowledge Delay */
#else
#define REG_PM_CTRLA               (*(RwReg8 *)0x40000000U) /**< \brief (PM) Control A */
#define REG_PM_SLEEPCFG            (*(RwReg8 *)0x40000001U) /**< \brief (PM) Sleep Configuration */
#define REG_PM_PLCFG               (*(RwReg8 *)0x40000002U) /**< \brief (PM) Performance Level Configuration */
#define REG_PM_INTENCLR            (*(RwReg8 *)0x40000004U) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (*(RwReg8 *)0x40000005U) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (*(RwReg8 *)0x40000006U) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_STDBYCFG            (*(RwReg16*)0x40000008U) /**< \brief (PM) Standby Configuration */
#define REG_PM_PWSAKDLY            (*(RwReg8 *)0x4000000CU) /**< \brief (PM) Power Switch Acknowledge Delay */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PM peripheral ========== */
#define PM_PD_NUM                   3        // Number of switchable Power Domain

#endif /* _SAML21_PM_INSTANCE_ */
