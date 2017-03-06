/**
 * \file
 *
 * \brief Instance description for EIC
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

#ifndef _SAML21_EIC_INSTANCE_
#define _SAML21_EIC_INSTANCE_

/* ========== Register definition for EIC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EIC_CTRLA              (0x40002400U) /**< \brief (EIC) Control */
#define REG_EIC_NMICTRL            (0x40002401U) /**< \brief (EIC) NMI Control */
#define REG_EIC_NMIFLAG            (0x40002402U) /**< \brief (EIC) NMI Interrupt Flag */
#define REG_EIC_SYNCBUSY           (0x40002404U) /**< \brief (EIC) Syncbusy register */
#define REG_EIC_EVCTRL             (0x40002408U) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (0x4000240CU) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (0x40002410U) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (0x40002414U) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_ASYNCH             (0x40002418U) /**< \brief (EIC) EIC Asynchronous edge Detection Enable */
#define REG_EIC_CONFIG0            (0x4000241CU) /**< \brief (EIC) Configuration 0 */
#define REG_EIC_CONFIG1            (0x40002420U) /**< \brief (EIC) Configuration 1 */
#else
#define REG_EIC_CTRLA              (*(RwReg8 *)0x40002400U) /**< \brief (EIC) Control */
#define REG_EIC_NMICTRL            (*(RwReg8 *)0x40002401U) /**< \brief (EIC) NMI Control */
#define REG_EIC_NMIFLAG            (*(RwReg16*)0x40002402U) /**< \brief (EIC) NMI Interrupt Flag */
#define REG_EIC_SYNCBUSY           (*(RoReg  *)0x40002404U) /**< \brief (EIC) Syncbusy register */
#define REG_EIC_EVCTRL             (*(RwReg  *)0x40002408U) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (*(RwReg  *)0x4000240CU) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (*(RwReg  *)0x40002410U) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (*(RwReg  *)0x40002414U) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_ASYNCH             (*(RwReg  *)0x40002418U) /**< \brief (EIC) EIC Asynchronous edge Detection Enable */
#define REG_EIC_CONFIG0            (*(RwReg  *)0x4000241CU) /**< \brief (EIC) Configuration 0 */
#define REG_EIC_CONFIG1            (*(RwReg  *)0x40002420U) /**< \brief (EIC) Configuration 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EIC peripheral ========== */
#define EIC_EXTINT_NUM              16
#define EIC_GCLK_ID                 3
#define EIC_NUMBER_OF_CONFIG_REGS   2
#define EIC_NUMBER_OF_INTERRUPTS    16

#endif /* _SAML21_EIC_INSTANCE_ */
