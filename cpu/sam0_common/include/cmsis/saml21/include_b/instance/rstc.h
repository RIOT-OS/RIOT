/**
 * \file
 *
 * \brief Instance description for RSTC
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

#ifndef _SAML21_RSTC_INSTANCE_
#define _SAML21_RSTC_INSTANCE_

/* ========== Register definition for RSTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RSTC_RCAUSE            (0x40000800U) /**< \brief (RSTC) Reset Cause */
#define REG_RSTC_BKUPEXIT          (0x40000802U) /**< \brief (RSTC) Backup Exit Source */
#define REG_RSTC_WKDBCONF          (0x40000804U) /**< \brief (RSTC) Wakeup Debounce Configuration */
#define REG_RSTC_WKPOL             (0x40000808U) /**< \brief (RSTC) Wakeup Polarity */
#define REG_RSTC_WKEN              (0x4000080CU) /**< \brief (RSTC) Wakeup Enable */
#define REG_RSTC_WKCAUSE           (0x40000810U) /**< \brief (RSTC) Wakeup Cause */
#else
#define REG_RSTC_RCAUSE            (*(RoReg8 *)0x40000800U) /**< \brief (RSTC) Reset Cause */
#define REG_RSTC_BKUPEXIT          (*(RoReg8 *)0x40000802U) /**< \brief (RSTC) Backup Exit Source */
#define REG_RSTC_WKDBCONF          (*(RwReg8 *)0x40000804U) /**< \brief (RSTC) Wakeup Debounce Configuration */
#define REG_RSTC_WKPOL             (*(RwReg16*)0x40000808U) /**< \brief (RSTC) Wakeup Polarity */
#define REG_RSTC_WKEN              (*(RwReg16*)0x4000080CU) /**< \brief (RSTC) Wakeup Enable */
#define REG_RSTC_WKCAUSE           (*(RwReg16*)0x40000810U) /**< \brief (RSTC) Wakeup Cause */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RSTC peripheral ========== */
#define RSTC_NUMBER_OF_EXTWAKE      8        // number of external wakeup line

#endif /* _SAML21_RSTC_INSTANCE_ */
