/***************************************************************************//**
* \file cy_crypto_core_hw.h
* \version 2.150
*
* \brief
*  This file provides the headers to the API for the utils
*  in the Crypto driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2020-2024), Cypress Semiconductor Corporation (an Infineon company) or
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


#if !defined (CY_CRYPTO_CORE_HW_H)
#define CY_CRYPTO_CORE_HW_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTO)

#include "cy_crypto_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 20.5', 1, \
'Since CY_CRYPTO_V1 is decided by PDL device agnostic / hardware specific model, use of #undef will not make it ambiguous that which macros exist at a particular point within a translation unit.')

#if (CY_IP_MXCRYPTO_VERSION == 1u)
#include "ip/cyip_crypto.h"
#endif

#if (CY_IP_MXCRYPTO_VERSION == 2u)
#if defined (CY_DEVICE_CAT1D)
#include "ip/cyip_crypto_v2_1.h"
#else
#include "ip/cyip_crypto_v2.h"
#endif

/* CRYPTO.CTL */
#if !defined(CRYPTO_CTL_ENABLED_Pos)
#define CRYPTO_CTL_ENABLED_Pos                  CRYPTO_V2_CTL_ENABLED_Pos
#endif
#if !defined(CRYPTO_CTL_ENABLED_Msk)
#define CRYPTO_CTL_ENABLED_Msk                  CRYPTO_V2_CTL_ENABLED_Msk
#endif

/* CRYPTO.STATUS */
#if !defined(CRYPTO_STATUS_VU_BUSY_Pos)
#define CRYPTO_STATUS_VU_BUSY_Pos               7UL
#endif
#if !defined(CRYPTO_STATUS_VU_BUSY_Msk)
#define CRYPTO_STATUS_VU_BUSY_Msk               0x80UL
#endif

/* CRYPTO.ERROR_STATUS1 */
#if !defined(CRYPTO_ERROR_STATUS1_DATA23_Pos)
#define CRYPTO_ERROR_STATUS1_DATA23_Pos         0UL
#endif
#if !defined(CRYPTO_ERROR_STATUS1_DATA23_Msk)
#define CRYPTO_ERROR_STATUS1_DATA23_Msk         0xFFFFFFUL
#endif
#if !defined(CRYPTO_ERROR_STATUS1_IDX_Pos)
#define CRYPTO_ERROR_STATUS1_IDX_Pos            CRYPTO_V2_ERROR_STATUS1_IDX_Pos
#endif
#if !defined(CRYPTO_ERROR_STATUS1_IDX_Msk)
#define CRYPTO_ERROR_STATUS1_IDX_Msk            CRYPTO_V2_ERROR_STATUS1_IDX_Msk
#endif
#if !defined(CRYPTO_ERROR_STATUS1_VALID_Pos)
#define CRYPTO_ERROR_STATUS1_VALID_Pos          CRYPTO_V2_ERROR_STATUS1_VALID_Pos
#endif
#if !defined(CRYPTO_ERROR_STATUS1_VALID_Msk)
#define CRYPTO_ERROR_STATUS1_VALID_Msk          CRYPTO_V2_ERROR_STATUS1_VALID_Msk
#endif

/* CRYPTO.INSTR_FF_STATUS */
#if !defined(CRYPTO_INSTR_FF_STATUS_USED_Pos)
#define CRYPTO_INSTR_FF_STATUS_USED_Pos         CRYPTO_V2_INSTR_FF_STATUS_USED_Pos
#endif
#if !defined(CRYPTO_INSTR_FF_STATUS_USED_Msk)
#define CRYPTO_INSTR_FF_STATUS_USED_Msk         CRYPTO_V2_INSTR_FF_STATUS_USED_Msk
#endif
#if !defined(CRYPTO_INSTR_FF_STATUS_EVENT_Pos)
#define CRYPTO_INSTR_FF_STATUS_EVENT_Pos        CRYPTO_V2_INSTR_FF_STATUS_EVENT_Pos
#endif
#if !defined(CRYPTO_INSTR_FF_STATUS_EVENT_Msk)
#define CRYPTO_INSTR_FF_STATUS_EVENT_Msk        CRYPTO_V2_INSTR_FF_STATUS_EVENT_Msk
#endif
#if !defined(CRYPTO_INSTR_FF_STATUS_BUSY_Pos)
#define CRYPTO_INSTR_FF_STATUS_BUSY_Pos         CRYPTO_V2_LOAD0_FF_STATUS_BUSY_Pos
#endif
#if !defined(CRYPTO_INSTR_FF_STATUS_BUSY_Msk)
#define CRYPTO_INSTR_FF_STATUS_BUSY_Msk         CRYPTO_V2_LOAD0_FF_STATUS_BUSY_Msk
#endif

/* CRYPTO.INSTR_FF_WR */
#if !defined(CRYPTO_INSTR_FF_WR_DATA32_Pos)
#define CRYPTO_INSTR_FF_WR_DATA32_Pos           CRYPTO_V2_INSTR_FF_WR_DATA32_Pos
#endif
#if !defined(CRYPTO_INSTR_FF_WR_DATA32_Msk)
#define CRYPTO_INSTR_FF_WR_DATA32_Msk           CRYPTO_V2_INSTR_FF_WR_DATA32_Msk
#endif

/* CRYPTO.RF_DATA */
#if !defined(CRYPTO_RF_DATA_DATA32_Pos)
#define CRYPTO_RF_DATA_DATA32_Pos               CRYPTO_V2_VU_RF_DATA_DATA32_Pos
#endif
#if !defined(CRYPTO_RF_DATA_DATA32_Msk)
#define CRYPTO_RF_DATA_DATA32_Msk               CRYPTO_V2_VU_RF_DATA_DATA32_Msk
#endif

/* CRYPTO.AES_CTL */
#if !defined(CRYPTO_AES_CTL_KEY_SIZE_Pos)
#define CRYPTO_AES_CTL_KEY_SIZE_Pos             CRYPTO_V2_AES_CTL_KEY_SIZE_Pos
#endif
#if !defined(CRYPTO_AES_CTL_KEY_SIZE_Msk)
#define CRYPTO_AES_CTL_KEY_SIZE_Msk             CRYPTO_V2_AES_CTL_KEY_SIZE_Msk
#endif

/* CRYPTO.PR_LFSR_CTL0 */
#if !defined(CRYPTO_PR_LFSR_CTL0_LFSR32_Pos)
#define CRYPTO_PR_LFSR_CTL0_LFSR32_Pos          CRYPTO_V2_PR_LFSR_CTL0_LFSR32_Pos
#endif
#if !defined(CRYPTO_PR_LFSR_CTL0_LFSR32_Msk)
#define CRYPTO_PR_LFSR_CTL0_LFSR32_Msk          CRYPTO_V2_PR_LFSR_CTL0_LFSR32_Msk
#endif

/* CRYPTO.PR_LFSR_CTL1 */
#if !defined(CRYPTO_PR_LFSR_CTL1_LFSR31_Pos)
#define CRYPTO_PR_LFSR_CTL1_LFSR31_Pos          CRYPTO_V2_PR_LFSR_CTL1_LFSR31_Pos
#endif
#if !defined(CRYPTO_PR_LFSR_CTL1_LFSR31_Msk)
#define CRYPTO_PR_LFSR_CTL1_LFSR31_Msk          CRYPTO_V2_PR_LFSR_CTL1_LFSR31_Msk
#endif

/* CRYPTO.PR_LFSR_CTL2 */
#if !defined(CRYPTO_PR_LFSR_CTL2_LFSR29_Pos)
#define CRYPTO_PR_LFSR_CTL2_LFSR29_Pos          CRYPTO_V2_PR_LFSR_CTL2_LFSR29_Pos
#endif
#if !defined(CRYPTO_PR_LFSR_CTL2_LFSR29_Msk)
#define CRYPTO_PR_LFSR_CTL2_LFSR29_Msk          CRYPTO_V2_PR_LFSR_CTL2_LFSR29_Msk
#endif

/* CRYPTO.TR_CTL0 */
#if !defined(CRYPTO_TR_CTL0_SAMPLE_CLOCK_DIV_Pos)
#define CRYPTO_TR_CTL0_SAMPLE_CLOCK_DIV_Pos     CRYPTO_V2_TR_CTL0_SAMPLE_CLOCK_DIV_Pos
#endif
#if !defined(CRYPTO_TR_CTL0_SAMPLE_CLOCK_DIV_Msk)
#define CRYPTO_TR_CTL0_SAMPLE_CLOCK_DIV_Msk     CRYPTO_V2_TR_CTL0_SAMPLE_CLOCK_DIV_Msk
#endif
#if !defined(CRYPTO_TR_CTL0_RED_CLOCK_DIV_Pos)
#define CRYPTO_TR_CTL0_RED_CLOCK_DIV_Pos        CRYPTO_V2_TR_CTL0_RED_CLOCK_DIV_Pos
#endif
#if !defined(CRYPTO_TR_CTL0_RED_CLOCK_DIV_Msk)
#define CRYPTO_TR_CTL0_RED_CLOCK_DIV_Msk        CRYPTO_V2_TR_CTL0_RED_CLOCK_DIV_Msk
#endif
#if !defined(CRYPTO_TR_CTL0_INIT_DELAY_Pos)
#define CRYPTO_TR_CTL0_INIT_DELAY_Pos           CRYPTO_V2_TR_CTL0_INIT_DELAY_Pos
#endif
#if !defined(CRYPTO_TR_CTL0_INIT_DELAY_Msk)
#define CRYPTO_TR_CTL0_INIT_DELAY_Msk           CRYPTO_V2_TR_CTL0_INIT_DELAY_Msk
#endif
#if !defined(CRYPTO_TR_CTL0_VON_NEUMANN_CORR_Pos)
#define CRYPTO_TR_CTL0_VON_NEUMANN_CORR_Pos     CRYPTO_V2_TR_CTL0_VON_NEUMANN_CORR_Pos
#endif
#if !defined(CRYPTO_TR_CTL0_VON_NEUMANN_CORR_Msk)
#define CRYPTO_TR_CTL0_VON_NEUMANN_CORR_Msk     CRYPTO_V2_TR_CTL0_VON_NEUMANN_CORR_Msk
#endif
#if !defined(CRYPTO_TR_CTL0_STOP_ON_AP_DETECT_Pos)
#define CRYPTO_TR_CTL0_STOP_ON_AP_DETECT_Pos    CRYPTO_V2_TR_CTL0_STOP_ON_AP_DETECT_Pos
#endif
#if !defined(CRYPTO_TR_CTL0_STOP_ON_AP_DETECT_Msk)
#define CRYPTO_TR_CTL0_STOP_ON_AP_DETECT_Msk    CRYPTO_V2_TR_CTL0_STOP_ON_AP_DETECT_Msk
#endif
#if !defined(CRYPTO_TR_CTL0_STOP_ON_RC_DETECT_Pos)
#define CRYPTO_TR_CTL0_STOP_ON_RC_DETECT_Pos    CRYPTO_V2_TR_CTL0_STOP_ON_RC_DETECT_Pos
#endif
#if !defined(CRYPTO_TR_CTL0_STOP_ON_RC_DETECT_Msk)
#define CRYPTO_TR_CTL0_STOP_ON_RC_DETECT_Msk    CRYPTO_V2_TR_CTL0_STOP_ON_RC_DETECT_Msk
#endif

/* CRYPTO.TR_CTL1 */
#if !defined(CRYPTO_TR_CTL1_RO11_EN_Pos)
#define CRYPTO_TR_CTL1_RO11_EN_Pos              CRYPTO_V2_TR_CTL1_RO11_EN_Pos
#endif
#if !defined(CRYPTO_TR_CTL1_RO11_EN_Msk)
#define CRYPTO_TR_CTL1_RO11_EN_Msk              CRYPTO_V2_TR_CTL1_RO11_EN_Msk
#endif
#if !defined(CRYPTO_TR_CTL1_RO15_EN_Pos)
#define CRYPTO_TR_CTL1_RO15_EN_Pos              CRYPTO_V2_TR_CTL1_RO15_EN_Pos
#endif
#if !defined(CRYPTO_TR_CTL1_RO15_EN_Msk)
#define CRYPTO_TR_CTL1_RO15_EN_Msk              CRYPTO_V2_TR_CTL1_RO15_EN_Msk
#endif
#if !defined(CRYPTO_TR_CTL1_GARO15_EN_Pos)
#define CRYPTO_TR_CTL1_GARO15_EN_Pos            CRYPTO_V2_TR_CTL1_GARO15_EN_Pos
#endif
#if !defined(CRYPTO_TR_CTL1_GARO15_EN_Msk)
#define CRYPTO_TR_CTL1_GARO15_EN_Msk            CRYPTO_V2_TR_CTL1_GARO15_EN_Msk
#endif
#if !defined(CRYPTO_TR_CTL1_GARO31_EN_Pos)
#define CRYPTO_TR_CTL1_GARO31_EN_Pos            CRYPTO_V2_TR_CTL1_GARO31_EN_Pos
#endif
#if !defined(CRYPTO_TR_CTL1_GARO31_EN_Msk)
#define CRYPTO_TR_CTL1_GARO31_EN_Msk            CRYPTO_V2_TR_CTL1_GARO31_EN_Msk
#endif
#if !defined(CRYPTO_TR_CTL1_FIRO15_EN_Pos)
#define CRYPTO_TR_CTL1_FIRO15_EN_Pos            CRYPTO_V2_TR_CTL1_FIRO15_EN_Pos
#endif
#if !defined(CRYPTO_TR_CTL1_FIRO15_EN_Msk)
#define CRYPTO_TR_CTL1_FIRO15_EN_Msk            CRYPTO_V2_TR_CTL1_FIRO15_EN_Msk
#endif
#if !defined(CRYPTO_TR_CTL1_FIRO31_EN_Pos)
#define CRYPTO_TR_CTL1_FIRO31_EN_Pos            CRYPTO_V2_TR_CTL1_FIRO31_EN_Pos
#endif
#if !defined(CRYPTO_TR_CTL1_FIRO31_EN_Msk)
#define CRYPTO_TR_CTL1_FIRO31_EN_Msk            CRYPTO_V2_TR_CTL1_FIRO31_EN_Msk
#endif

/* CRYPTO.TR_RESULT */
#if !defined(CRYPTO_TR_RESULT_DATA32_Pos)
#define CRYPTO_TR_RESULT_DATA32_Pos             CRYPTO_V2_TR_RESULT_DATA32_Pos
#endif
#if !defined(CRYPTO_TR_RESULT_DATA32_Msk)
#define CRYPTO_TR_RESULT_DATA32_Msk             CRYPTO_V2_TR_RESULT_DATA32_Msk
#endif

/* CRYPTO.TR_GARO_CTL */
#if !defined(CRYPTO_TR_GARO_CTL_POLYNOMIAL31_Pos)
#define CRYPTO_TR_GARO_CTL_POLYNOMIAL31_Pos     CRYPTO_V2_TR_GARO_CTL_POLYNOMIAL31_Pos
#endif
#if !defined(CRYPTO_TR_GARO_CTL_POLYNOMIAL31_Msk)
#define CRYPTO_TR_GARO_CTL_POLYNOMIAL31_Msk     CRYPTO_V2_TR_GARO_CTL_POLYNOMIAL31_Msk
#endif

/* CRYPTO.TR_FIRO_CTL */
#if !defined(CRYPTO_TR_FIRO_CTL_POLYNOMIAL31_Pos)
#define CRYPTO_TR_FIRO_CTL_POLYNOMIAL31_Pos     CRYPTO_V2_TR_FIRO_CTL_POLYNOMIAL31_Pos
#endif
#if !defined(CRYPTO_TR_FIRO_CTL_POLYNOMIAL31_Msk)
#define CRYPTO_TR_FIRO_CTL_POLYNOMIAL31_Msk     CRYPTO_V2_TR_FIRO_CTL_POLYNOMIAL31_Msk
#endif

/* CRYPTO.TR_MON_CTL */
#if !defined(CRYPTO_TR_MON_CTL_BITSTREAM_SEL_Pos)
#define CRYPTO_TR_MON_CTL_BITSTREAM_SEL_Pos     CRYPTO_V2_TR_MON_CTL_BITSTREAM_SEL_Pos
#endif
#if !defined(CRYPTO_TR_MON_CTL_BITSTREAM_SEL_Msk)
#define CRYPTO_TR_MON_CTL_BITSTREAM_SEL_Msk     CRYPTO_V2_TR_MON_CTL_BITSTREAM_SEL_Msk
#endif

/* CRYPTO.TR_MON_CMD */
#if !defined(CRYPTO_TR_MON_CMD_START_AP_Pos)
#define CRYPTO_TR_MON_CMD_START_AP_Pos          CRYPTO_V2_TR_MON_CMD_START_AP_Pos
#endif
#if !defined(CRYPTO_TR_MON_CMD_START_AP_Msk)
#define CRYPTO_TR_MON_CMD_START_AP_Msk          CRYPTO_V2_TR_MON_CMD_START_AP_Msk
#endif
#if !defined(CRYPTO_TR_MON_CMD_START_RC_Pos)
#define CRYPTO_TR_MON_CMD_START_RC_Pos          CRYPTO_V2_TR_MON_CMD_START_RC_Pos
#endif
#if !defined(CRYPTO_TR_MON_CMD_START_RC_Msk)
#define CRYPTO_TR_MON_CMD_START_RC_Msk          CRYPTO_V2_TR_MON_CMD_START_RC_Msk
#endif

/* CRYPTO.TR_MON_RC_CTL */
#if !defined(CRYPTO_TR_MON_RC_CTL_CUTOFF_COUNT8_Pos)
#define CRYPTO_TR_MON_RC_CTL_CUTOFF_COUNT8_Pos  CRYPTO_V2_TR_MON_RC_CTL_CUTOFF_COUNT8_Pos
#endif
#if !defined(CRYPTO_TR_MON_RC_CTL_CUTOFF_COUNT8_Msk)
#define CRYPTO_TR_MON_RC_CTL_CUTOFF_COUNT8_Msk  CRYPTO_V2_TR_MON_RC_CTL_CUTOFF_COUNT8_Msk
#endif

/* CRYPTO.TR_MON_RC_STATUS0 */
#if !defined(CRYPTO_TR_MON_RC_STATUS0_BIT_Pos)
#define CRYPTO_TR_MON_RC_STATUS0_BIT_Pos        CRYPTO_V2_TR_MON_RC_STATUS0_BIT_Pos
#endif
#if !defined(CRYPTO_TR_MON_RC_STATUS0_BIT_Msk)
#define CRYPTO_TR_MON_RC_STATUS0_BIT_Msk        CRYPTO_V2_TR_MON_RC_STATUS0_BIT_Msk
#endif

/* CRYPTO.TR_MON_RC_STATUS1 */
#if !defined(CRYPTO_TR_MON_RC_STATUS1_REP_COUNT_Pos)
#define CRYPTO_TR_MON_RC_STATUS1_REP_COUNT_Pos  CRYPTO_V2_TR_MON_RC_STATUS1_REP_COUNT_Pos
#endif
#if !defined(CRYPTO_TR_MON_RC_STATUS1_REP_COUNT_Msk)
#define CRYPTO_TR_MON_RC_STATUS1_REP_COUNT_Msk  CRYPTO_V2_TR_MON_RC_STATUS1_REP_COUNT_Msk
#endif

/* CRYPTO.TR_MON_AP_CTL */
#if !defined(CRYPTO_TR_MON_AP_CTL_CUTOFF_COUNT16_Pos)
#define CRYPTO_TR_MON_AP_CTL_CUTOFF_COUNT16_Pos CRYPTO_V2_TR_MON_AP_CTL_CUTOFF_COUNT16_Pos
#endif
#if !defined(CRYPTO_TR_MON_AP_CTL_CUTOFF_COUNT16_Msk)
#define CRYPTO_TR_MON_AP_CTL_CUTOFF_COUNT16_Msk CRYPTO_V2_TR_MON_AP_CTL_CUTOFF_COUNT16_Msk
#endif
#if !defined(CRYPTO_TR_MON_AP_CTL_WINDOW_SIZE_Pos)
#define CRYPTO_TR_MON_AP_CTL_WINDOW_SIZE_Pos    CRYPTO_V2_TR_MON_AP_CTL_WINDOW_SIZE_Pos
#endif
#if !defined(CRYPTO_TR_MON_AP_CTL_WINDOW_SIZE_Msk)
#define CRYPTO_TR_MON_AP_CTL_WINDOW_SIZE_Msk    CRYPTO_V2_TR_MON_AP_CTL_WINDOW_SIZE_Msk
#endif

/* CRYPTO.TR_MON_AP_STATUS0 */
#if !defined(CRYPTO_TR_MON_AP_STATUS0_BIT_Pos)
#define CRYPTO_TR_MON_AP_STATUS0_BIT_Pos        CRYPTO_V2_TR_MON_AP_STATUS0_BIT_Pos
#endif
#if !defined(CRYPTO_TR_MON_AP_STATUS0_BIT_Msk)
#define CRYPTO_TR_MON_AP_STATUS0_BIT_Msk        CRYPTO_V2_TR_MON_AP_STATUS0_BIT_Msk
#endif

/* CRYPTO.TR_MON_AP_STATUS1 */
#if !defined(CRYPTO_TR_MON_AP_STATUS1_OCC_COUNT_Pos)
#define CRYPTO_TR_MON_AP_STATUS1_OCC_COUNT_Pos  CRYPTO_V2_TR_MON_AP_STATUS1_OCC_COUNT_Pos
#endif
#if !defined(CRYPTO_TR_MON_AP_STATUS1_OCC_COUNT_Msk)
#define CRYPTO_TR_MON_AP_STATUS1_OCC_COUNT_Msk  CRYPTO_V2_TR_MON_AP_STATUS1_OCC_COUNT_Msk
#endif
#if !defined(CRYPTO_TR_MON_AP_STATUS1_WINDOW_INDEX_Pos)
#define CRYPTO_TR_MON_AP_STATUS1_WINDOW_INDEX_Pos CRYPTO_V2_TR_MON_AP_STATUS1_WINDOW_INDEX_Pos
#endif
#if !defined(CRYPTO_TR_MON_AP_STATUS1_WINDOW_INDEX_Msk)
#define CRYPTO_TR_MON_AP_STATUS1_WINDOW_INDEX_Msk CRYPTO_V2_TR_MON_AP_STATUS1_WINDOW_INDEX_Msk
#endif

/* CRYPTO.VU_STATUS */
#if !defined(CRYPTO_VU_STATUS_CARRY_Pos)
#define CRYPTO_VU_STATUS_CARRY_Pos              CRYPTO_V2_VU_STATUS_CARRY_Pos
#endif
#if !defined(CRYPTO_VU_STATUS_CARRY_Msk)
#define CRYPTO_VU_STATUS_CARRY_Msk              CRYPTO_V2_VU_STATUS_CARRY_Msk
#endif
#if !defined(CRYPTO_VU_STATUS_EVEN_Pos)
#define CRYPTO_VU_STATUS_EVEN_Pos               CRYPTO_V2_VU_STATUS_EVEN_Pos
#endif
#if !defined(CRYPTO_VU_STATUS_EVEN_Msk)
#define CRYPTO_VU_STATUS_EVEN_Msk               CRYPTO_V2_VU_STATUS_EVEN_Msk
#endif
#if !defined(CRYPTO_VU_STATUS_ZERO_Pos)
#define CRYPTO_VU_STATUS_ZERO_Pos               CRYPTO_V2_VU_STATUS_ZERO_Pos
#endif
#if !defined(CRYPTO_VU_STATUS_ZERO_Msk)
#define CRYPTO_VU_STATUS_ZERO_Msk               CRYPTO_V2_VU_STATUS_ZERO_Msk
#endif
#if !defined(CRYPTO_VU_STATUS_ONE_Pos)
#define CRYPTO_VU_STATUS_ONE_Pos                CRYPTO_V2_VU_STATUS_ONE_Pos
#endif
#if !defined(CRYPTO_VU_STATUS_ONE_Msk)
#define CRYPTO_VU_STATUS_ONE_Msk                CRYPTO_V2_VU_STATUS_ONE_Msk
#endif

/* CRYPTO.INTR */
#if !defined(CRYPTO_INTR_TR_INITIALIZED_Pos)
#define CRYPTO_INTR_TR_INITIALIZED_Pos          CRYPTO_V2_INTR_TR_INITIALIZED_Pos
#endif
#if !defined(CRYPTO_INTR_TR_INITIALIZED_Msk)
#define CRYPTO_INTR_TR_INITIALIZED_Msk          CRYPTO_V2_INTR_TR_INITIALIZED_Msk
#endif
#if !defined(CRYPTO_INTR_TR_DATA_AVAILABLE_Pos)
#define CRYPTO_INTR_TR_DATA_AVAILABLE_Pos       CRYPTO_V2_INTR_TR_DATA_AVAILABLE_Pos
#endif
#if !defined(CRYPTO_INTR_TR_DATA_AVAILABLE_Msk)
#define CRYPTO_INTR_TR_DATA_AVAILABLE_Msk       CRYPTO_V2_INTR_TR_DATA_AVAILABLE_Msk
#endif
#if !defined(CRYPTO_INTR_INSTR_OPC_ERROR_Pos)
#define CRYPTO_INTR_INSTR_OPC_ERROR_Pos         CRYPTO_V2_INTR_INSTR_OPC_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_INSTR_OPC_ERROR_Msk)
#define CRYPTO_INTR_INSTR_OPC_ERROR_Msk         CRYPTO_V2_INTR_INSTR_OPC_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_INSTR_CC_ERROR_Pos)
#define CRYPTO_INTR_INSTR_CC_ERROR_Pos          CRYPTO_V2_INTR_INSTR_CC_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_INSTR_CC_ERROR_Msk)
#define CRYPTO_INTR_INSTR_CC_ERROR_Msk          CRYPTO_V2_INTR_INSTR_CC_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_BUS_ERROR_Pos)
#define CRYPTO_INTR_BUS_ERROR_Pos               CRYPTO_V2_INTR_BUS_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_BUS_ERROR_Msk)
#define CRYPTO_INTR_BUS_ERROR_Msk               CRYPTO_V2_INTR_BUS_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_TR_AP_DETECT_ERROR_Pos)
#define CRYPTO_INTR_TR_AP_DETECT_ERROR_Pos      CRYPTO_V2_INTR_TR_AP_DETECT_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_TR_AP_DETECT_ERROR_Msk)
#define CRYPTO_INTR_TR_AP_DETECT_ERROR_Msk      CRYPTO_V2_INTR_TR_AP_DETECT_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_TR_RC_DETECT_ERROR_Pos)
#define CRYPTO_INTR_TR_RC_DETECT_ERROR_Pos      CRYPTO_V2_INTR_TR_RC_DETECT_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_TR_RC_DETECT_ERROR_Msk)
#define CRYPTO_INTR_TR_RC_DETECT_ERROR_Msk      CRYPTO_V2_INTR_TR_RC_DETECT_ERROR_Msk
#endif

/* CRYPTO.INTR_MASK */
#if !defined(CRYPTO_INTR_MASK_INSTR_FF_LEVEL_Pos)
#define CRYPTO_INTR_MASK_INSTR_FF_LEVEL_Pos     CRYPTO_V2_INTR_MASK_INSTR_FF_LEVEL_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_FF_LEVEL_Msk)
#define CRYPTO_INTR_MASK_INSTR_FF_LEVEL_Msk     CRYPTO_V2_INTR_MASK_INSTR_FF_LEVEL_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_FF_OVERFLOW_Pos)
#define CRYPTO_INTR_MASK_INSTR_FF_OVERFLOW_Pos  CRYPTO_V2_INTR_MASK_INSTR_FF_OVERFLOW_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_FF_OVERFLOW_Msk)
#define CRYPTO_INTR_MASK_INSTR_FF_OVERFLOW_Msk  CRYPTO_V2_INTR_MASK_INSTR_FF_OVERFLOW_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_TR_INITIALIZED_Pos)
#define CRYPTO_INTR_MASK_TR_INITIALIZED_Pos     CRYPTO_V2_INTR_MASK_TR_INITIALIZED_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_TR_INITIALIZED_Msk)
#define CRYPTO_INTR_MASK_TR_INITIALIZED_Msk     CRYPTO_V2_INTR_MASK_TR_INITIALIZED_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_TR_DATA_AVAILABLE_Pos)
#define CRYPTO_INTR_MASK_TR_DATA_AVAILABLE_Pos  CRYPTO_V2_INTR_MASK_TR_DATA_AVAILABLE_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_TR_DATA_AVAILABLE_Msk)
#define CRYPTO_INTR_MASK_TR_DATA_AVAILABLE_Msk  CRYPTO_V2_INTR_MASK_TR_DATA_AVAILABLE_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_PR_DATA_AVAILABLE_Pos)
#define CRYPTO_INTR_MASK_PR_DATA_AVAILABLE_Pos  CRYPTO_V2_INTR_MASK_PR_DATA_AVAILABLE_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_PR_DATA_AVAILABLE_Msk)
#define CRYPTO_INTR_MASK_PR_DATA_AVAILABLE_Msk  CRYPTO_V2_INTR_MASK_PR_DATA_AVAILABLE_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_OPC_ERROR_Pos)
#define CRYPTO_INTR_MASK_INSTR_OPC_ERROR_Pos    CRYPTO_V2_INTR_MASK_INSTR_OPC_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_OPC_ERROR_Msk)
#define CRYPTO_INTR_MASK_INSTR_OPC_ERROR_Msk    CRYPTO_V2_INTR_MASK_INSTR_OPC_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_CC_ERROR_Pos)
#define CRYPTO_INTR_MASK_INSTR_CC_ERROR_Pos     CRYPTO_V2_INTR_MASK_INSTR_CC_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_INSTR_CC_ERROR_Msk)
#define CRYPTO_INTR_MASK_INSTR_CC_ERROR_Msk     CRYPTO_V2_INTR_MASK_INSTR_CC_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_BUS_ERROR_Pos)
#define CRYPTO_INTR_MASK_BUS_ERROR_Pos          CRYPTO_V2_INTR_MASK_BUS_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_BUS_ERROR_Msk)
#define CRYPTO_INTR_MASK_BUS_ERROR_Msk          CRYPTO_V2_INTR_MASK_BUS_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_TR_AP_DETECT_ERROR_Pos)
#define CRYPTO_INTR_MASK_TR_AP_DETECT_ERROR_Pos CRYPTO_V2_INTR_MASK_TR_AP_DETECT_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_TR_AP_DETECT_ERROR_Msk)
#define CRYPTO_INTR_MASK_TR_AP_DETECT_ERROR_Msk CRYPTO_V2_INTR_MASK_TR_AP_DETECT_ERROR_Msk
#endif
#if !defined(CRYPTO_INTR_MASK_TR_RC_DETECT_ERROR_Pos)
#define CRYPTO_INTR_MASK_TR_RC_DETECT_ERROR_Pos CRYPTO_V2_INTR_MASK_TR_RC_DETECT_ERROR_Pos
#endif
#if !defined(CRYPTO_INTR_MASK_TR_RC_DETECT_ERROR_Msk)
#define CRYPTO_INTR_MASK_TR_RC_DETECT_ERROR_Msk CRYPTO_V2_INTR_MASK_TR_RC_DETECT_ERROR_Msk
#endif
#endif


/** \cond INTERNAL */

/*******************************************************************************
*                CRYPTO
*******************************************************************************/
/* Non-changed registers */
#define REG_CRYPTO_CTL(base)               (((CRYPTO_Type*)(base))->CTL)
#define REG_CRYPTO_ERROR_STATUS0(base)     (((CRYPTO_Type*)(base))->ERROR_STATUS0)
#define REG_CRYPTO_ERROR_STATUS1(base)     (((CRYPTO_Type*)(base))->ERROR_STATUS1)
#define REG_CRYPTO_PR_LFSR_CTL0(base)      (((CRYPTO_Type*)(base))->PR_LFSR_CTL0)
#define REG_CRYPTO_PR_LFSR_CTL1(base)      (((CRYPTO_Type*)(base))->PR_LFSR_CTL1)
#define REG_CRYPTO_PR_LFSR_CTL2(base)      (((CRYPTO_Type*)(base))->PR_LFSR_CTL2)
#define REG_CRYPTO_TR_CTL0(base)           (((CRYPTO_Type*)(base))->TR_CTL0)
#define REG_CRYPTO_TR_CTL1(base)           (((CRYPTO_Type*)(base))->TR_CTL1)
#define REG_CRYPTO_TR_GARO_CTL(base)       (((CRYPTO_Type*)(base))->TR_GARO_CTL)
#define REG_CRYPTO_TR_FIRO_CTL(base)       (((CRYPTO_Type*)(base))->TR_FIRO_CTL)
#define REG_CRYPTO_TR_MON_CTL(base)        (((CRYPTO_Type*)(base))->TR_MON_CTL)
#define REG_CRYPTO_TR_MON_CMD(base)        (((CRYPTO_Type*)(base))->TR_MON_CMD)
#define REG_CRYPTO_TR_MON_RC_CTL(base)     (((CRYPTO_Type*)(base))->TR_MON_RC_CTL)
#define REG_CRYPTO_TR_MON_RC_STATUS0(base) (((CRYPTO_Type*)(base))->TR_MON_RC_STATUS0)
#define REG_CRYPTO_TR_MON_RC_STATUS1(base) (((CRYPTO_Type*)(base))->TR_MON_RC_STATUS1)
#define REG_CRYPTO_TR_MON_AP_CTL(base)     (((CRYPTO_Type*)(base))->TR_MON_AP_CTL)
#define REG_CRYPTO_TR_MON_AP_STATUS0(base) (((CRYPTO_Type*)(base))->TR_MON_AP_STATUS0)
#define REG_CRYPTO_TR_MON_AP_STATUS1(base) (((CRYPTO_Type*)(base))->TR_MON_AP_STATUS1)

/* Changed registers in the regmap */
#if !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC)

/* Use PDL device agnostic model */

#define REG_CRYPTO_STATUS(base)            (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoStatusOffset))
#define REG_CRYPTO_INSTR_FF_CTL(base)      (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoIstrFfCtlOffset))
#define REG_CRYPTO_INSTR_FF_STATUS(base)   (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoInstrFfStatusOffset))
#define REG_CRYPTO_INSTR_FF_WR(base)       (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoInstrFfWrOffset))
#define REG_CRYPTO_AES_CTL(base)           (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoAesCtlOffset))
#define REG_CRYPTO_PR_RESULT(base)         (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoPrResultOffset))
#define REG_CRYPTO_TR_RESULT(base)         (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoTrResultOffset))
#define REG_CRYPTO_CRC_CTL(base)           (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoCrcCtlOffset))
#define REG_CRYPTO_CRC_DATA_CTL(base)      (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoCrcDataCtlOffset))
#define REG_CRYPTO_CRC_POL_CTL(base)       (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoCrcPolCtlOffset))
#define REG_CRYPTO_CRC_REM_CTL(base)       (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoCrcRemCtlOffset))
#define REG_CRYPTO_CRC_REM_RESULT(base)    (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoCrcRemResultOffset))
#define REG_CRYPTO_VU_CTL0(base)           (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoVuCtl0Offset))
#define REG_CRYPTO_VU_CTL1(base)           (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoVuCtl1Offset))
#define REG_CRYPTO_VU_STATUS(base)         (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoVuStatusOffset))
#define REG_CRYPTO_INTR(base)              (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoIntrOffset))
#define REG_CRYPTO_INTR_SET(base)          (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoIntrSetOffset))
#define REG_CRYPTO_INTR_MASK(base)         (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoIntrMaskOffset))
#define REG_CRYPTO_INTR_MASKED(base)       (*(volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoIntrMaskedOffset))
#define REG_CRYPTO_VU_RF_DATA(base, reg)   ( (volatile uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoVuRfDataOffset))[(reg)]
#define REG_CRYPTO_MEM_BUFF(base)          ( (uint32_t*)((uint32_t)(base) + cy_cryptoIP->cryptoMemBufOffset))

#else

/* Use hardware specific model*/

#undef  CY_CRYPTO_V1
#define CY_CRYPTO_V1                       (1U == CY_IP_MXCRYPTO_VERSION) /* true if the mxcrypto version is 1 */

#if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
#define REG_CRYPTO_STATUS(base)            (((CRYPTO_V1_Type*)(base))->STATUS)
#define REG_CRYPTO_INSTR_FF_CTL(base)      (((CRYPTO_V1_Type*)(base))->INSTR_FF_CTL)
#define REG_CRYPTO_INSTR_FF_STATUS(base)   (((CRYPTO_V1_Type*)(base))->INSTR_FF_STATUS)
#define REG_CRYPTO_INSTR_FF_WR(base)       (((CRYPTO_V1_Type*)(base))->INSTR_FF_WR)
#define REG_CRYPTO_AES_CTL(base)           (((CRYPTO_V1_Type*)(base))->AES_CTL)
#define REG_CRYPTO_PR_RESULT(base)         (((CRYPTO_V1_Type*)(base))->PR_RESULT)
#define REG_CRYPTO_TR_RESULT(base)         (((CRYPTO_V1_Type*)(base))->TR_RESULT)
#define REG_CRYPTO_CRC_CTL(base)           (((CRYPTO_V1_Type*)(base))->CRC_CTL)
#define REG_CRYPTO_CRC_DATA_CTL(base)      (((CRYPTO_V1_Type*)(base))->CRC_DATA_CTL)
#define REG_CRYPTO_CRC_POL_CTL(base)       (((CRYPTO_V1_Type*)(base))->CRC_POL_CTL)
#define REG_CRYPTO_CRC_REM_CTL(base)       (((CRYPTO_V1_Type*)(base))->CRC_REM_CTL)
#define REG_CRYPTO_CRC_REM_RESULT(base)    (((CRYPTO_V1_Type*)(base))->CRC_REM_RESULT)
#define REG_CRYPTO_VU_CTL0(base)           (((CRYPTO_V1_Type*)(base))->VU_CTL0)
#define REG_CRYPTO_VU_CTL1(base)           (((CRYPTO_V1_Type*)(base))->VU_CTL1)
#define REG_CRYPTO_VU_STATUS(base)         (((CRYPTO_V1_Type*)(base))->VU_STATUS)
#define REG_CRYPTO_INTR(base)              (((CRYPTO_V1_Type*)(base))->INTR)
#define REG_CRYPTO_INTR_SET(base)          (((CRYPTO_V1_Type*)(base))->INTR_SET)
#define REG_CRYPTO_INTR_MASK(base)         (((CRYPTO_V1_Type*)(base))->INTR_MASK)
#define REG_CRYPTO_INTR_MASKED(base)       (((CRYPTO_V1_Type*)(base))->INTR_MASKED)
#define REG_CRYPTO_VU_RF_DATA(base, reg)   (((CRYPTO_V1_Type*)(base))->RF_DATA[(reg)])
#define REG_CRYPTO_MEM_BUFF(base)          (uint32_t *)(&((CRYPTO_V1_Type*)(base))->MEM_BUFF)
#endif

#if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
#define REG_CRYPTO_STATUS(base)            (((CRYPTO_V2_Type*)(base))->STATUS)
#define REG_CRYPTO_INSTR_FF_CTL(base)      (((CRYPTO_V2_Type*)(base))->INSTR_FF_CTL)
#define REG_CRYPTO_INSTR_FF_STATUS(base)   (((CRYPTO_V2_Type*)(base))->INSTR_FF_STATUS)
#define REG_CRYPTO_INSTR_FF_WR(base)       (((CRYPTO_V2_Type*)(base))->INSTR_FF_WR)
#define REG_CRYPTO_AES_CTL(base)           (((CRYPTO_V2_Type*)(base))->AES_CTL)
#define REG_CRYPTO_PR_RESULT(base)         (((CRYPTO_V2_Type*)(base))->PR_RESULT)
#define REG_CRYPTO_TR_RESULT(base)         (((CRYPTO_V2_Type*)(base))->TR_RESULT)
#define REG_CRYPTO_CRC_CTL(base)           (((CRYPTO_V2_Type*)(base))->CRC_CTL)
#define REG_CRYPTO_CRC_DATA_CTL(base)      (((CRYPTO_V2_Type*)(base))->CRC_DATA_CTL)
#define REG_CRYPTO_CRC_POL_CTL(base)       (((CRYPTO_V2_Type*)(base))->CRC_POL_CTL)
#define REG_CRYPTO_CRC_REM_CTL(base)       (((CRYPTO_V2_Type*)(base))->CRC_REM_CTL)
#define REG_CRYPTO_CRC_REM_RESULT(base)    (((CRYPTO_V2_Type*)(base))->CRC_REM_RESULT)
#define REG_CRYPTO_VU_CTL0(base)           (((CRYPTO_V2_Type*)(base))->VU_CTL0)
#define REG_CRYPTO_VU_CTL1(base)           (((CRYPTO_V2_Type*)(base))->VU_CTL1)
#define REG_CRYPTO_VU_STATUS(base)         (((CRYPTO_V2_Type*)(base))->VU_STATUS)
#define REG_CRYPTO_INTR(base)              (((CRYPTO_V2_Type*)(base))->INTR)
#define REG_CRYPTO_INTR_SET(base)          (((CRYPTO_V2_Type*)(base))->INTR_SET)
#define REG_CRYPTO_INTR_MASK(base)         (((CRYPTO_V2_Type*)(base))->INTR_MASK)
#define REG_CRYPTO_INTR_MASKED(base)       (((CRYPTO_V2_Type*)(base))->INTR_MASKED)
#define REG_CRYPTO_VU_RF_DATA(base, reg)   (((CRYPTO_V2_Type*)(base))->VU_RF_DATA[(reg)])
#define REG_CRYPTO_MEM_BUFF(base)          (uint32_t *)(&((CRYPTO_V2_Type*)(base))->MEM_BUFF)
#endif

#endif /* !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC) */


/* For Internal memory VU operation, Crypto CTL register should always point to Non-secure memory address
for both Secure & Non-Secure execution */        
#if defined(SECURE_ALIAS_OFFSET)
#define SECURE_ALIAS_OFFSET_POS 28UL
#define CY_CRYPTO_VU_MEM_ALIAS_ADDRESS(base)             (uint32_t)(((uint32_t)(REG_CRYPTO_VU_CTL1(base))) | ((uint32_t)((CRYPTO_V2_CTL_NS_Pos) & ~_FLD2VAL(CRYPTO_V2_CTL_NS, REG_CRYPTO_CTL(base))) << SECURE_ALIAS_OFFSET_POS))
#define CY_CRYPTO_VU_MEM_NS_ALIAS_ADDRESS(base)             ((uint32_t)((uint32_t)(base) & ~(SECURE_ALIAS_OFFSET)))
#else
#define CY_CRYPTO_VU_MEM_ALIAS_ADDRESS(base) (REG_CRYPTO_VU_CTL1(base))
#define CY_CRYPTO_VU_MEM_NS_ALIAS_ADDRESS(base) (base)
#endif

/* Old V1 registers in the regmap */
#if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
#define REG_CRYPTO_RAM_PWRUP_DELAY(base)   (((CRYPTO_V1_Type*)(base))->RAM_PWRUP_DELAY)
#define REG_CRYPTO_STR_RESULT(base)        (((CRYPTO_V1_Type*)(base))->STR_RESULT)
#define REG_CRYPTO_SHA_CTL(base)           (((CRYPTO_V1_Type*)(base))->SHA_CTL)
#define REG_CRYPTO_CRC_LFSR_CTL(base)      (((CRYPTO_V1_Type*)(base))->CRC_LFSR_CTL)
#endif

/* New V2 registers in the regmap */
#if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
#define REG_CRYPTO_RAM_PWR_CTL(base)       (((CRYPTO_V2_Type*)(base))->RAM_PWR_CTL)
#define REG_CRYPTO_RAM_PWR_DELAY_CTL(base) (((CRYPTO_V2_Type*)(base))->RAM_PWR_DELAY_CTL)
#define REG_CRYPTO_ECC_CTL(base)           (((CRYPTO_V2_Type*)(base))->ECC_CTL)
#define REG_CRYPTO_PR_MAX_CTL(base)        (((CRYPTO_V2_Type*)(base))->PR_MAX_CTL)
#define REG_CRYPTO_PR_CMD(base)            (((CRYPTO_V2_Type*)(base))->PR_CMD)
#define REG_CRYPTO_TR_CTL2(base)           (((CRYPTO_V2_Type*)(base))->TR_CTL2)
#define REG_CRYPTO_TR_STATUS(base)         (((CRYPTO_V2_Type*)(base))->TR_STATUS)
#define REG_CRYPTO_TR_CMD(base)            (((CRYPTO_V2_Type*)(base))->TR_CMD)
#define REG_CRYPTO_LOAD0_FF_STATUS(base)   (((CRYPTO_V2_Type*)(base))->LOAD0_FF_STATUS)
#define REG_CRYPTO_LOAD1_FF_STATUS(base)   (((CRYPTO_V2_Type*)(base))->LOAD1_FF_STATUS)
#define REG_CRYPTO_STORE_FF_STATUS(base)   (((CRYPTO_V2_Type*)(base))->STORE_FF_STATUS)
#define REG_CRYPTO_RESULT(base)            (((CRYPTO_V2_Type*)(base))->RESULT)
#define REG_CRYPTO_VU_CTL2(base)           (((CRYPTO_V2_Type*)(base))->VU_CTL2)
#define REG_CRYPTO_DEV_KEY_ADDR0_CTL(base) (((CRYPTO_V2_Type*)(base))->DEV_KEY_ADDR0_CTL)
#define REG_CRYPTO_DEV_KEY_ADDR0(base)     (((CRYPTO_V2_Type*)(base))->DEV_KEY_ADDR0)
#define REG_CRYPTO_DEV_KEY_ADDR1_CTL(base) (((CRYPTO_V2_Type*)(base))->DEV_KEY_ADDR1_CTL)
#define REG_CRYPTO_DEV_KEY_ADDR1(base)     (((CRYPTO_V2_Type*)(base))->DEV_KEY_ADDR1)
#define REG_CRYPTO_DEV_KEY_STATUS(base)    (((CRYPTO_V2_Type*)(base))->DEV_KEY_STATUS)
#define REG_CRYPTO_DEV_KEY_CTL0(base)      (((CRYPTO_V2_Type*)(base))->DEV_KEY_CTL0)
#define REG_CRYPTO_DEV_KEY_CTL1(base)      (((CRYPTO_V2_Type*)(base))->DEV_KEY_CTL1)
#endif

/* The CRYPTO internal-memory buffer-size in 32-bit words. */
//#define CY_CRYPTO_MEM_BUFF_SIZE_U32        (cy_device->cryptoMemSize)
/* The CRYPTO internal-memory buffer-size in bytes. */
#define CY_CRYPTO_MEM_BUFF_SIZE            (CY_CRYPTO_MEM_BUFF_SIZE_U32 * 4u)

#if !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC)
/* Device Crypto IP descriptor type */
typedef struct
{
    /* CRYPTO register offsets */
    uint32_t cryptoStatusOffset;
    uint32_t cryptoIstrFfCtlOffset;
    uint32_t cryptoInstrFfStatusOffset;
    uint32_t cryptoInstrFfWrOffset;
    uint32_t cryptoVuRfDataOffset;
    uint32_t cryptoAesCtlOffset;
    uint32_t cryptoPrResultOffset;
    uint32_t cryptoTrResultOffset;
    uint32_t cryptoCrcCtlOffset;
    uint32_t cryptoCrcDataCtlOffset;
    uint32_t cryptoCrcPolCtlOffset;
    uint32_t cryptoCrcRemCtlOffset;
    uint32_t cryptoCrcRemResultOffset;
    uint32_t cryptoVuCtl0Offset;
    uint32_t cryptoVuCtl1Offset;
    uint32_t cryptoVuStatusOffset;
    uint32_t cryptoIntrOffset;
    uint32_t cryptoIntrSetOffset;
    uint32_t cryptoIntrMaskOffset;
    uint32_t cryptoIntrMaskedOffset;
    uint32_t cryptoMemBufOffset;
} cy_stc_cryptoIP_t;

/*******************************************************************************
*                   Global Variables
*******************************************************************************/
extern const cy_stc_cryptoIP_t   cy_cryptoIpBlockCfgPSoC6_01;
extern const cy_stc_cryptoIP_t   cy_cryptoIpBlockCfgPSoC6_02;
extern const cy_stc_cryptoIP_t  *cy_cryptoIP;
#endif

#define CY_CRYPTO_REGFILE_R0             (0x00u)
#define CY_CRYPTO_REGFILE_R1             (0x01u)
#define CY_CRYPTO_REGFILE_R2             (0x02u)
#define CY_CRYPTO_REGFILE_R3             (0x03u)
#define CY_CRYPTO_REGFILE_R4             (0x04u)
#define CY_CRYPTO_REGFILE_R5             (0x05u)
#define CY_CRYPTO_REGFILE_R6             (0x06u)
#define CY_CRYPTO_REGFILE_R7             (0x07u)
#define CY_CRYPTO_REGFILE_R8             (0x08u)
#define CY_CRYPTO_REGFILE_R9             (0x09u)
#define CY_CRYPTO_REGFILE_R10            (0x0Au)
#define CY_CRYPTO_REGFILE_R11            (0x0Bu)
#define CY_CRYPTO_REGFILE_R12            (0x0Cu)
#define CY_CRYPTO_REGFILE_R13            (0x0Du)
#define CY_CRYPTO_REGFILE_R14            (0x0Eu)
#define CY_CRYPTO_REGFILE_R15            (0x0Fu)

#define CY_CRYPTO_RSRC0_SHIFT            (0u)
#define CY_CRYPTO_RSRC4_SHIFT            (4u)
#define CY_CRYPTO_RSRC8_SHIFT            (8u)
#define CY_CRYPTO_RSRC12_SHIFT           (12u)
#define CY_CRYPTO_RSRC13_SHIFT           (13u)
#define CY_CRYPTO_RSRC16_SHIFT           (16u)
#define CY_CRYPTO_RSRC20_SHIFT           (20u)
#define CY_CRYPTO_RSRC23_SHIFT           (23u)
#define CY_CRYPTO_RSRC26_SHIFT           (26u)
#define CY_CRYPTO_RSRC30_SHIFT           (30u)

#define CY_CRYPTO_OPCODE_POS             (24u)

/* Define bit mask for all errors interrupt sources */
#define CY_CRYPTO_INTR_ERROR_MASK       ((uint32_t)(CRYPTO_INTR_INSTR_OPC_ERROR_Msk | \
                                                    CRYPTO_INTR_INSTR_CC_ERROR_Msk | \
                                                    CRYPTO_INTR_BUS_ERROR_Msk | \
                                                    CRYPTO_INTR_TR_AP_DETECT_ERROR_Msk | \
                                                    CRYPTO_INTR_TR_RC_DETECT_ERROR_Msk))

/* Define bit mask for all errors interrupt sources for interrupt setting register */
#define CY_CRYPTO_INTR_SET_ERROR_MASK   ((uint32_t)(CRYPTO_INTR_SET_INSTR_OPC_ERROR_Msk | \
                                                    CRYPTO_INTR_SET_INSTR_CC_ERROR_Msk | \
                                                    CRYPTO_INTR_SET_BUS_ERROR_Msk | \
                                                    CRYPTO_INTR_SET_TR_AP_DETECT_ERROR_Msk | \
                                                    CRYPTO_INTR_SET_TR_RC_DETECT_ERROR_Msk))

/* Define bit mask for all errors interrupt sources for interrupt masking register */
#define CY_CRYPTO_INTR_MASK_ERROR_MASK   ((uint32_t)(CRYPTO_INTR_MASK_INSTR_OPC_ERROR_Msk | \
                                                    CRYPTO_INTR_MASK_INSTR_CC_ERROR_Msk | \
                                                    CRYPTO_INTR_MASK_BUS_ERROR_Msk | \
                                                    CRYPTO_INTR_MASK_TR_AP_DETECT_ERROR_Msk | \
                                                    CRYPTO_INTR_MASK_TR_RC_DETECT_ERROR_Msk))

/* Define bit mask for all errors interrupt sources for interrupt masked sources check */
#define CY_CRYPTO_INTR_MASKED_ERROR_MASK   ((uint32_t)(CRYPTO_INTR_MASKED_INSTR_OPC_ERROR_Msk | \
                                                    CRYPTO_INTR_MASKED_INSTR_CC_ERROR_Msk | \
                                                    CRYPTO_INTR_MASKED_BUS_ERROR_Msk | \
                                                    CRYPTO_INTR_MASKED_TR_AP_DETECT_ERROR_Msk | \
                                                    CRYPTO_INTR_MASKED_TR_RC_DETECT_ERROR_Msk))

#define CY_CRYPTO_INSTR_FIFODEPTH          (8u)
#define CY_CRYPTO_V1_DATA_FIFODEPTH        (8u)
#define CY_CRYPTO_V2_DATA_FIFODEPTH        (16u)

#define CY_CRYPTO_INSTR_SINGLE             (1u)
#define CY_CRYPTO_INSTR_DOUBLE             (2u)
#define CY_CRYPTO_INSTR_TRIPLE             (3u)

#define CY_CRYPTO_MIN(a,b)                 (((a) < (b)) ? (a) : (b))
#define CY_CRYPTO_MAX(a,b)                 (((a) > (b)) ? (a) : (b))

typedef enum
{
    CY_CRYPTO_CTL_ENABLED_DISABLED  = 0u,
    CY_CRYPTO_CTL_ENABLED_ENABLED   = 1u,
} cy_en_crypto_hw_enable_t;

#if !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC)
__STATIC_INLINE void Cy_Crypto_Core_HwInit(void);
#endif

void Cy_Crypto_Core_ClearVuRegisters(CRYPTO_Type *base);
void Cy_Crypto_Core_Vu_RunInstr(CRYPTO_Type *base, bool blockingMode, uint32_t instr, uint32_t params);

/** \endcond */


/**
* \addtogroup group_crypto_lld_hw_functions
* \{
*/

cy_en_crypto_status_t Cy_Crypto_Core_Enable(CRYPTO_Type *base);

cy_en_crypto_status_t Cy_Crypto_Core_Disable(CRYPTO_Type *base);

cy_en_crypto_status_t Cy_Crypto_Core_Cleanup(CRYPTO_Type *base);

cy_en_crypto_status_t Cy_Crypto_Core_Shutdown(CRYPTO_Type *base);

cy_en_crypto_status_t Cy_Crypto_Core_GetLibInfo(cy_en_crypto_lib_info_t *libInfo);

cy_en_crypto_status_t Cy_Crypto_Core_SetVuMemoryAddress(CRYPTO_Type *base, uint32_t const *vuMemoryAddr, uint32_t vuMemorySize);

__STATIC_INLINE uint32_t * Cy_Crypto_Core_GetVuMemoryAddress(CRYPTO_Type *base);

uint32_t Cy_Crypto_Core_GetVuMemorySize(CRYPTO_Type *base);

void Cy_Crypto_Core_InvertEndianness(void *inArrPtr, uint32_t byteSize);

#if !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC)
/*******************************************************************************
* Function Name: Cy_Crypto_Core_HwInit
****************************************************************************//**
*
* The function to initialize the Crypto hardware.
*
* This function is for internal use.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Crypto_Core_HwInit(void)
{
    cy_cryptoIP = (CY_CRYPTO_V1) ? &cy_cryptoIpBlockCfgPSoC6_01 : &cy_cryptoIpBlockCfgPSoC6_02;
}
#endif

/*******************************************************************************
* Function Name: Cy_Crypto_Core_IsEnabled
****************************************************************************//**
*
* The function checks whether the Crypto hardware is enabled.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \return
* Crypto status \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_myCryptoCoreStopCryptoUse
*
*******************************************************************************/
__STATIC_INLINE bool Cy_Crypto_Core_IsEnabled(CRYPTO_Type *base)
{
#if !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC)
    if (cy_cryptoIP == NULL)
    {
        Cy_Crypto_Core_HwInit();
    }
#endif

    return (1uL == (uint32_t)_FLD2VAL(CRYPTO_CTL_ENABLED, REG_CRYPTO_CTL(base)));
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_GetFIFODepth
****************************************************************************//**
*
* Returns the total available number of instructions in the instruction FIFO.
*
* \param base
* The pointer to the CRYPTO instance.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Crypto_Core_GetFIFODepth(CRYPTO_Type *base)
{
    (void)base; /* Suppress warning */
    return (CY_CRYPTO_INSTR_FIFODEPTH);
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_GetFIFOUsed
****************************************************************************//**
*
* Returns the number of instructions in the instruction FIFO.
* The value of this field ranges from 0 to 8
*
* \param base
* The pointer to the CRYPTO instance.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Crypto_Core_GetFIFOUsed(CRYPTO_Type *base)
{
    return((uint8_t)_FLD2VAL(CRYPTO_INSTR_FF_STATUS_USED, REG_CRYPTO_INSTR_FF_STATUS(base)));
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_WaitForInstrFifoAvailable
*****************************************************************************//**
*
* Waits until number of entries in the instruction FIFO is less than
* specified number.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param instr
* The number of needed available space in the instruction FIFO.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Crypto_Core_WaitForInstrFifoAvailable(CRYPTO_Type *base, uint32_t instr)
{
    while((uint32_t)(_FLD2VAL(CRYPTO_INSTR_FF_STATUS_USED, REG_CRYPTO_INSTR_FF_STATUS(base))) >= (CY_CRYPTO_INSTR_FIFODEPTH - instr))
    {
    }
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_WaitForFifoAvailable
*****************************************************************************//**
*
* Waits until number of entries in the instruction FIFO is less than
* specified in EVENT_LEVEL field in FF_CTL register, an event is generated:
* "event" = INSTR_FF_STATUS.USED < EVENT_LEVEL.
* By default EVENT_LEVEL = 0;
*
* \param base
* The pointer to the CRYPTO instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Crypto_Core_WaitForFifoAvailable(CRYPTO_Type *base)
{
    while((_FLD2VAL(CRYPTO_INSTR_FF_STATUS_EVENT, REG_CRYPTO_INSTR_FF_STATUS(base))) == 0u)
    {
    }
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_WaitForReady
*****************************************************************************//**
*
* Waits until all instruction in FIFO will be completed
*
* \param base
* The pointer to the CRYPTO instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Crypto_Core_WaitForReady(CRYPTO_Type *base)
{
    while(REG_CRYPTO_STATUS(base) != 0u)
    {
    }
}

/*******************************************************************************
* Function Name: Cy_Crypto_Wait_Vu_ForComplete
****************************************************************************//**
*
* Waits until VU instruction will be completed
*
* \param base
* The pointer to the CRYPTO instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Crypto_Core_Vu_WaitForComplete(CRYPTO_Type *base)
{
    /* Wait until the VU instruction is complete */
    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        while (0uL != _FLD2VAL(CRYPTO_STATUS_VU_BUSY, REG_CRYPTO_STATUS(base)))
        {
        }
        #endif
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        while (0uL != REG_CRYPTO_STATUS(base))
        {
        }
        #endif
    }
}

/******************************************************************************
* Function Name: Cy_Crypto_Core_SetInterruptMask
***************************************************************************//**
*
* Masks / unmasks multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param interrupts
* Mask bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE void Cy_Crypto_Core_SetInterruptMask(CRYPTO_Type *base, uint32_t interrupts)
{
    REG_CRYPTO_INTR_MASK(base) = interrupts;
}

/******************************************************************************
* Function Name: Cy_Crypto_Core_GetInterruptMask
***************************************************************************//**
*
* Reports mask / unmask multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \return
* Mask bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_Crypto_Core_GetInterruptMask(CRYPTO_Type const *base)
{
    return (REG_CRYPTO_INTR_MASK(base));
}

/******************************************************************************
* Function Name: Cy_Crypto_Core_GetInterruptStatusMasked
***************************************************************************//**
*
* Reports states of multiple enabled interrupt sources.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \return
* Source bits. See definitions above.
*
*****************************************************************************/
__STATIC_INLINE uint32_t Cy_Crypto_Core_GetInterruptStatusMasked(CRYPTO_Type const *base)
{
    return (REG_CRYPTO_INTR_MASKED(base));
}

/******************************************************************************
* Function Name: Cy_Crypto_Core_GetInterruptStatus
***************************************************************************//**
*
* Reports states of multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \return
* Source bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_Crypto_Core_GetInterruptStatus(CRYPTO_Type *base)
{
    return (REG_CRYPTO_INTR(base));
}

/******************************************************************************
* Function Name: Cy_Crypto_Core_SetInterrupt
***************************************************************************//**
*
* Sets one of more interrupt sources
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param interrupts
* Source bit(s)
*
******************************************************************************/
__STATIC_INLINE void  Cy_Crypto_Core_SetInterrupt(CRYPTO_Type *base, uint32_t interrupts)
{
    REG_CRYPTO_INTR_SET(base) = interrupts;
}

/******************************************************************************
* Function Name: Cy_Crypto_Core_ClearInterrupt
***************************************************************************//**
*
* Clears multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param interrupts
* Source bit(s). See definitions above.
*
******************************************************************************/
__STATIC_INLINE void  Cy_Crypto_Core_ClearInterrupt(CRYPTO_Type *base, uint32_t interrupts)
{
    REG_CRYPTO_INTR(base) = interrupts;
    /* This dummy reading is necessary here. It provides a guarantee that interrupt is cleared at returning from this function. */
    (void) REG_CRYPTO_INTR(base);
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_GetVuMemoryAddress
****************************************************************************//**
*
* Gets Crypto memory buffer address
*
* \param base
* The pointer to the CRYPTO instance.
*
* \return
* Current Crypto MEM_BUFF location address or NULL if Crypto IP is not enabled.
*
*******************************************************************************/
__STATIC_INLINE uint32_t * Cy_Crypto_Core_GetVuMemoryAddress(CRYPTO_Type *base)
{
    #if !defined(CY_CRYPTO_CFG_HW_USE_MPN_SPECIFIC)
    return (cy_cryptoIP != NULL) ? (uint32_t *)CY_CRYPTO_VU_MEM_ALIAS_ADDRESS(base) : (uint32_t *)NULL;
    #else
    return (uint32_t *)CY_CRYPTO_VU_MEM_ALIAS_ADDRESS(base);
    #endif
}

/** \} group_crypto_lld_hw_functions */

CY_MISRA_BLOCK_END('MISRA C-2012 Rule 20.5')

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTO */

#endif /* #if !defined (CY_CRYPTO_CORE_HW_H) */


/* [] END OF FILE */
