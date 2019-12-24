/**
 * \file
 *
 * \brief Instance description for RSTC
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

#ifndef _SAMR34_RSTC_INSTANCE_
#define _SAMR34_RSTC_INSTANCE_

/* ========== Register definition for RSTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RSTC_RCAUSE            (0x40000800) /**< \brief (RSTC) Reset Cause */
#define REG_RSTC_BKUPEXIT          (0x40000802) /**< \brief (RSTC) Backup Exit Source */
#define REG_RSTC_WKDBCONF          (0x40000804) /**< \brief (RSTC) Wakeup Debounce Configuration */
#define REG_RSTC_WKPOL             (0x40000808) /**< \brief (RSTC) Wakeup Polarity */
#define REG_RSTC_WKEN              (0x4000080C) /**< \brief (RSTC) Wakeup Enable */
#define REG_RSTC_WKCAUSE           (0x40000810) /**< \brief (RSTC) Wakeup Cause */
#else
#define REG_RSTC_RCAUSE            (*(RoReg8 *)0x40000800UL) /**< \brief (RSTC) Reset Cause */
#define REG_RSTC_BKUPEXIT          (*(RoReg8 *)0x40000802UL) /**< \brief (RSTC) Backup Exit Source */
#define REG_RSTC_WKDBCONF          (*(RwReg8 *)0x40000804UL) /**< \brief (RSTC) Wakeup Debounce Configuration */
#define REG_RSTC_WKPOL             (*(RwReg16*)0x40000808UL) /**< \brief (RSTC) Wakeup Polarity */
#define REG_RSTC_WKEN              (*(RwReg16*)0x4000080CUL) /**< \brief (RSTC) Wakeup Enable */
#define REG_RSTC_WKCAUSE           (*(RwReg16*)0x40000810UL) /**< \brief (RSTC) Wakeup Cause */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RSTC peripheral ========== */
#define RSTC_NUMBER_OF_EXTWAKE      8        // number of external wakeup line

#endif /* _SAMR34_RSTC_INSTANCE_ */
