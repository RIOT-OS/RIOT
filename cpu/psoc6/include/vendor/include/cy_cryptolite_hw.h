/***************************************************************************//**
* \file cy_cryptolite_hw.h
* \version 2.80
*
* \brief
*  This file provides common constants and macros
*  for the Cryptolite driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2020-2022), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/
#if !defined (CY_CRYPTOLITE_HW_H)
#define CY_CRYPTOLITE_HW_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#define REG_CRYPTOLITE_CTL(base)                         (((CRYPTOLITE_Type*)(base))->CTL)
#define REG_CRYPTOLITE_STATUS(base)                      (((CRYPTOLITE_Type*)(base))->STATUS)
#define REG_CRYPTOLITE_AES_DESCR(base)                   (((CRYPTOLITE_Type*)(base))->AES_DESCR)
#define REG_CRYPTOLITE_SHA_DESCR(base)                   (((CRYPTOLITE_Type*)(base))->SHA_DESCR)
#define REG_CRYPTOLITE_VU_DESCR(base)                    (((CRYPTOLITE_Type*)(base))->VU_DESCR)
#define REG_CRYPTOLITE_SHA_INTR_ERROR(base)              (((CRYPTOLITE_Type*)(base))->INTR_ERROR)
#define REG_CRYPTOLITE_SHA_INTR_ERROR_SET(base)          (((CRYPTOLITE_Type*)(base))->INTR_ERROR_SET)
#define REG_CRYPTOLITE_SHA_INTR_ERROR_MASK(base)         (((CRYPTOLITE_Type*)(base))->INTR_ERROR_MASK)
#define REG_CRYPTOLITE_SHA_INTR_ERROR_MASKED(base)       (((CRYPTOLITE_Type*)(base))->INTR_ERROR_MASKED)

#define REG_CRYPTOLITE_TRNG_CTL0(base)                     (((CRYPTOLITE_Type*)(base))->TRNG_CTL0)
#define REG_CRYPTOLITE_TRNG_CTL1(base)                     (((CRYPTOLITE_Type*)(base))->TRNG_CTL1)
#define REG_CRYPTOLITE_TRNG_STATUS(base)                   (((CRYPTOLITE_Type*)(base))->TRNG_STATUS)
#define REG_CRYPTOLITE_TRNG_RESULT(base)                   (((CRYPTOLITE_Type*)(base))->TRNG_RESULT)
#define REG_CRYPTOLITE_TRNG_GARO_CTL(base)                 (((CRYPTOLITE_Type*)(base))->TRNG_GARO_CTL)
#define REG_CRYPTOLITE_TRNG_FIRO_CTL(base)                 (((CRYPTOLITE_Type*)(base))->TRNG_FIRO_CTL)
#define REG_CRYPTOLITE_TRNG_MON_CTL(base)                  (((CRYPTOLITE_Type*)(base))->TRNG_MON_CTL)
#define REG_CRYPTOLITE_TRNG_MON_RC_CTL(base)               (((CRYPTOLITE_Type*)(base))->TRNG_MON_RC_CTL)
#define REG_CRYPTOLITE_TRNG_MON_RC_STATUS0(base)           (((CRYPTOLITE_Type*)(base))->TRNG_MON_RC_STATUS0)
#define REG_CRYPTOLITE_TRNG_MON_RC_STATUS1(base)           (((CRYPTOLITE_Type*)(base))->TRNG_MON_RC_STATUS1)
#define REG_CRYPTOLITE_TRNG_MON_AP_CTL(base)               (((CRYPTOLITE_Type*)(base))->TRNG_MON_AP_CTL)
#define REG_CRYPTOLITE_TRNG_MON_AP_STATUS0(base)           (((CRYPTOLITE_Type*)(base))->TRNG_MON_AP_STATUS0)
#define REG_CRYPTOLITE_TRNG_MON_AP_STATUS1(base)           (((CRYPTOLITE_Type*)(base))->TRNG_MON_AP_STATUS1)
#define REG_CRYPTOLITE_INTR_TRNG(base)                     (((CRYPTOLITE_Type*)(base))->INTR_TRNG)
#define REG_CRYPTOLITE_INTR_TRNG_SET(base)                 (((CRYPTOLITE_Type*)(base))->INTR_TRNG_SET)
#define REG_CRYPTOLITE_INTR_TRNG_MASK(base)                (((CRYPTOLITE_Type*)(base))->INTR_TRNG_MASK)
#define REG_CRYPTOLITE_INTR_TRNG_MASKED(base)              (((CRYPTOLITE_Type*)(base))->INTR_TRNG_MASKED)

#define REG_CRYPTOLITE_INTR_ERROR(base)                 (((CRYPTOLITE_Type*)(base))->INTR_ERROR)
#define REG_CRYPTOLITE_INTR_ERROR_SET(base)             (((CRYPTOLITE_Type*)(base))->INTR_ERROR_SET)
#define REG_CRYPTOLITE_INTR_ERROR_MASK(base)            (((CRYPTOLITE_Type*)(base))->INTR_ERROR_MASK)
#define REG_CRYPTOLITE_INTR_ERROR_MASKED(base)          (((CRYPTOLITE_Type*)(base))->INTR_ERROR_MASKED)

#define CY_CRYPTOLITE_MSG_SCH_CTLWD                      ((uint32_t)(0UL << 28UL))
#define CY_CRYPTOLITE_PROCESS_CTLWD                      ((uint32_t)(1UL << 28UL))
/*bus error interrupt mask*/
#define CY_CRYPTOLITE_INTR_BUS_ERROR_MASK                ((uint32_t)CRYPTOLITE_INTR_ERROR_BUS_ERROR_Msk)

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_HW_H) */
