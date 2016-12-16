/**
 * \file
 *
 * \brief Instance description for SUPC
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

#ifndef _SAML21_SUPC_INSTANCE_
#define _SAML21_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SUPC_INTENCLR          (0x40001400U) /**< \brief (SUPC) Interrupt Enable Clear */
#define REG_SUPC_INTENSET          (0x40001404U) /**< \brief (SUPC) Interrupt Enable Set */
#define REG_SUPC_INTFLAG           (0x40001408U) /**< \brief (SUPC) Interrupt Flag Status and Clear */
#define REG_SUPC_STATUS            (0x4000140CU) /**< \brief (SUPC) Power and Clocks Status */
#define REG_SUPC_BOD33             (0x40001410U) /**< \brief (SUPC) BOD33 Control */
#define REG_SUPC_BOD12             (0x40001414U) /**< \brief (SUPC) BOD12 Control */
#define REG_SUPC_VREG              (0x40001418U) /**< \brief (SUPC) VREG Control */
#define REG_SUPC_VREF              (0x4000141CU) /**< \brief (SUPC) VREF Control */
#define REG_SUPC_BBPS              (0x40001420U) /**< \brief (SUPC) Battery Backup Power Switch */
#define REG_SUPC_BKOUT             (0x40001424U) /**< \brief (SUPC) Backup Output Control */
#define REG_SUPC_BKIN              (0x40001428U) /**< \brief (SUPC) Backup Input Control */
#else
#define REG_SUPC_INTENCLR          (*(RwReg  *)0x40001400U) /**< \brief (SUPC) Interrupt Enable Clear */
#define REG_SUPC_INTENSET          (*(RwReg  *)0x40001404U) /**< \brief (SUPC) Interrupt Enable Set */
#define REG_SUPC_INTFLAG           (*(RwReg  *)0x40001408U) /**< \brief (SUPC) Interrupt Flag Status and Clear */
#define REG_SUPC_STATUS            (*(RoReg  *)0x4000140CU) /**< \brief (SUPC) Power and Clocks Status */
#define REG_SUPC_BOD33             (*(RwReg  *)0x40001410U) /**< \brief (SUPC) BOD33 Control */
#define REG_SUPC_BOD12             (*(RwReg  *)0x40001414U) /**< \brief (SUPC) BOD12 Control */
#define REG_SUPC_VREG              (*(RwReg  *)0x40001418U) /**< \brief (SUPC) VREG Control */
#define REG_SUPC_VREF              (*(RwReg  *)0x4000141CU) /**< \brief (SUPC) VREF Control */
#define REG_SUPC_BBPS              (*(RwReg  *)0x40001420U) /**< \brief (SUPC) Battery Backup Power Switch */
#define REG_SUPC_BKOUT             (*(RwReg  *)0x40001424U) /**< \brief (SUPC) Backup Output Control */
#define REG_SUPC_BKIN              (*(RoReg  *)0x40001428U) /**< \brief (SUPC) Backup Input Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SUPC peripheral ========== */
#define SUPC_BOD12_CALIB_MSB        5
#define SUPC_BOD33_CALIB_MSB        5
#define SUPC_SUPC_OUT_NUM_MSB       1        // MSB of backup output pad Number

#endif /* _SAML21_SUPC_INSTANCE_ */
