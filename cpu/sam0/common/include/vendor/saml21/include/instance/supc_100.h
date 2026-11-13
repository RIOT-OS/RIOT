/**
 * \file
 *
 * \brief Instance description for SUPC
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

#ifndef _SAML21_SUPC_INSTANCE_
#define _SAML21_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SUPC_INTENCLR          (0x40001400) /**< \brief (SUPC) Interrupt Enable Clear */
#define REG_SUPC_INTENSET          (0x40001404) /**< \brief (SUPC) Interrupt Enable Set */
#define REG_SUPC_INTFLAG           (0x40001408) /**< \brief (SUPC) Interrupt Flag Status and Clear */
#define REG_SUPC_STATUS            (0x4000140C) /**< \brief (SUPC) Power and Clocks Status */
#define REG_SUPC_BOD33             (0x40001410) /**< \brief (SUPC) BOD33 Control */
#define REG_SUPC_BOD12             (0x40001414) /**< \brief (SUPC) BOD12 Control */
#define REG_SUPC_VREG              (0x40001418) /**< \brief (SUPC) VREG Control */
#define REG_SUPC_VREF              (0x4000141C) /**< \brief (SUPC) VREF Control */
#define REG_SUPC_BBPS              (0x40001420) /**< \brief (SUPC) Battery Backup Power Switch */
#define REG_SUPC_BKOUT             (0x40001424) /**< \brief (SUPC) Backup Output Control */
#define REG_SUPC_BKIN              (0x40001428) /**< \brief (SUPC) Backup Input Control */
#else
#define REG_SUPC_INTENCLR          (*(RwReg  *)0x40001400UL) /**< \brief (SUPC) Interrupt Enable Clear */
#define REG_SUPC_INTENSET          (*(RwReg  *)0x40001404UL) /**< \brief (SUPC) Interrupt Enable Set */
#define REG_SUPC_INTFLAG           (*(RwReg  *)0x40001408UL) /**< \brief (SUPC) Interrupt Flag Status and Clear */
#define REG_SUPC_STATUS            (*(RoReg  *)0x4000140CUL) /**< \brief (SUPC) Power and Clocks Status */
#define REG_SUPC_BOD33             (*(RwReg  *)0x40001410UL) /**< \brief (SUPC) BOD33 Control */
#define REG_SUPC_BOD12             (*(RwReg  *)0x40001414UL) /**< \brief (SUPC) BOD12 Control */
#define REG_SUPC_VREG              (*(RwReg  *)0x40001418UL) /**< \brief (SUPC) VREG Control */
#define REG_SUPC_VREF              (*(RwReg  *)0x4000141CUL) /**< \brief (SUPC) VREF Control */
#define REG_SUPC_BBPS              (*(RwReg  *)0x40001420UL) /**< \brief (SUPC) Battery Backup Power Switch */
#define REG_SUPC_BKOUT             (*(RwReg  *)0x40001424UL) /**< \brief (SUPC) Backup Output Control */
#define REG_SUPC_BKIN              (*(RoReg  *)0x40001428UL) /**< \brief (SUPC) Backup Input Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SUPC peripheral ========== */
#define SUPC_BOD12_CALIB_MSB        5       
#define SUPC_BOD33_CALIB_MSB        5       
#define SUPC_SUPC_OUT_NUM_MSB       1        // MSB of backup output pad Number

#endif /* _SAML21_SUPC_INSTANCE_ */
