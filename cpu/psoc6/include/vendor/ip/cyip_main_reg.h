/***************************************************************************//**
* \file cyip_main_reg.h
*
* \brief
* MAIN_REG IP definitions
*
********************************************************************************
* \copyright
* (c) (2016-2025), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef _CYIP_MAIN_REG_H_
#define _CYIP_MAIN_REG_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                   MAIN_REG
*******************************************************************************/

#define MAIN_REG_TR_GR_SECTION_SIZE             0x00000400UL
#define MAIN_REG_TR_ASSIST_GR_SECTION_SIZE      0x00000400UL
#define MAIN_REG_SECTION_SIZE                   0x00001000UL

/**
  * \brief Trigger group (MAIN_REG_TR_GR)
  */
typedef struct {
   __IM uint32_t RESERVED[32];
  __IOM uint32_t TR_CTL[64];                    /*!< 0x00000080 Trigger control register */
  __IOM uint32_t TR_CDMA_CTL[16];               /*!< 0x00000180 Central DMA Trigger control register */
   __IM uint32_t RESERVED1[144];
} MAIN_REG_TR_GR_V1_Type;                       /*!< Size = 1024 (0x400) */

/**
  * \brief Trigger Assist - for CDMA triggers (MAIN_REG_TR_ASSIST_GR)
  */
typedef struct {
  __IOM uint32_t TRA_CTRL[16];                  /*!< 0x00000000 Trigger Assist Control Registers */
   __IM uint32_t RESERVED[48];
  __IOM uint32_t TRA_SLP_CTRL[16];              /*!< 0x00000100 Trigger Assist Short Length Packet / Zero Length Packet Control */
   __IM uint32_t RESERVED1[48];
   __IM uint32_t TRA_STAT[16];                  /*!< 0x00000200 Trigger Assist Status Registers */
   __IM uint32_t RESERVED2[112];
} MAIN_REG_TR_ASSIST_GR_V1_Type;                /*!< Size = 1024 (0x400) */

/**
  * \brief HBWSS Main Registers (MAIN_REG)
  */
typedef struct {
  __IOM uint32_t CTRL;                          /*!< 0x00000000 Main Control Register */
  __IOM uint32_t HBWSS_SPARE;                   /*!< 0x00000004 SPARE Config/Status */
  __IOM uint32_t SPCTRL;                        /*!< 0x00000008 SRAM Power Control Register */
   __IM uint32_t RESERVED[5];
  __IOM uint32_t TR_CMD;                        /*!< 0x00000020 Trigger command */
   __IM uint32_t RESERVED1[7];
  __IOM uint32_t TRA_CO_INTR;                   /*!< 0x00000040 CDMA output tr_assist Interrupt Status */
  __IOM uint32_t TRA_CO_INTR_SET;               /*!< 0x00000044 CDMA output tr_assist Interrupt Set */
  __IOM uint32_t TRA_CO_INTR_MASK;              /*!< 0x00000048 CDMA output tr_assist Interrupt Mask */
   __IM uint32_t TRA_CO_INTR_MASKED;            /*!< 0x0000004C CDMA output tr_assist Interrupt Masked */
  __IOM uint32_t DDFT_MUX;                      /*!< 0x00000050 DDFT0/1 mux selection Register */
   __IM uint32_t RESERVED2[235];
        MAIN_REG_TR_GR_V1_Type TR_GR[1];        /*!< 0x00000400 Trigger group */
        MAIN_REG_TR_ASSIST_GR_V1_Type TR_ASSIST_GR[1]; /*!< 0x00000800 Trigger Assist - for CDMA triggers */
} MAIN_REG_V1_Type;                             /*!< Size = 3072 (0xC00) */


/* MAIN_REG_TR_GR.TR_CTL */
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL0_Pos       0UL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL0_Msk       0x7FUL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL1_Pos       7UL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL1_Msk       0x3F80UL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL2_Pos       14UL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL2_Msk       0x1FC000UL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL3_Pos       21UL
#define MAIN_REG_TR_GR_TR_CTL_TR_SEL3_Msk       0xFE00000UL
#define MAIN_REG_TR_GR_TR_CTL_DBG_FREEZE_EN_Pos 31UL
#define MAIN_REG_TR_GR_TR_CTL_DBG_FREEZE_EN_Msk 0x80000000UL
/* MAIN_REG_TR_GR.TR_CDMA_CTL */
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_SEL0_Pos  0UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_SEL0_Msk  0x7FUL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_SEL1_Pos  7UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_SEL1_Msk  0x3F80UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_INV_Pos   17UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_INV_Msk   0x20000UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_EDGE_Pos  18UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_TR_EDGE_Msk  0x40000UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_DBG_FREEZE_EN_Pos 31UL
#define MAIN_REG_TR_GR_TR_CDMA_CTL_DBG_FREEZE_EN_Msk 0x80000000UL


/* MAIN_REG_TR_ASSIST_GR.TRA_CTRL */
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_DSCR_NUM_Pos 0UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_DSCR_NUM_Msk 0xFFFFUL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_STREAM_EN_Pos 16UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_STREAM_EN_Msk 0x10000UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_HBWSS_IS_CONS_Pos 20UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_HBWSS_IS_CONS_Msk 0x100000UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_PKT_MODE_Pos 21UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_PKT_MODE_Msk 0x200000UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_WRAP_UP_SKT_Pos 24UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_WRAP_UP_SKT_Msk 0x1000000UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_SUPPR_1ST_TR_Pos 28UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_SUPPR_1ST_TR_Msk 0x10000000UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_SUPPR_1ST_INT_Pos 29UL
#define MAIN_REG_TR_ASSIST_GR_TRA_CTRL_TRA_SUPPR_1ST_INT_Msk 0x20000000UL
/* MAIN_REG_TR_ASSIST_GR.TRA_SLP_CTRL */
#define MAIN_REG_TR_ASSIST_GR_TRA_SLP_CTRL_TRA_SHORT_CONFIG_Pos 0UL
#define MAIN_REG_TR_ASSIST_GR_TRA_SLP_CTRL_TRA_SHORT_CONFIG_Msk 0x1UL
#define MAIN_REG_TR_ASSIST_GR_TRA_SLP_CTRL_TRA_BYTE_COUNT_Pos 16UL
#define MAIN_REG_TR_ASSIST_GR_TRA_SLP_CTRL_TRA_BYTE_COUNT_Msk 0xFFFF0000UL
/* MAIN_REG_TR_ASSIST_GR.TRA_STAT */
#define MAIN_REG_TR_ASSIST_GR_TRA_STAT_TRA_STATE_Pos 0UL
#define MAIN_REG_TR_ASSIST_GR_TRA_STAT_TRA_STATE_Msk 0xFUL


/* MAIN_REG.CTRL */
#define MAIN_REG_CTRL_DMA_SRC_SEL_Pos           0UL
#define MAIN_REG_CTRL_DMA_SRC_SEL_Msk           0x3UL
#define MAIN_REG_CTRL_DMA_DIV_SEL_Pos           4UL
#define MAIN_REG_CTRL_DMA_DIV_SEL_Msk           0x30UL
#define MAIN_REG_CTRL_BUFFSIZE_64KB_EN_Pos      8UL
#define MAIN_REG_CTRL_BUFFSIZE_64KB_EN_Msk      0x100UL
#define MAIN_REG_CTRL_EVICT_SLOW_AHB_RD_CACHE_Pos 16UL
#define MAIN_REG_CTRL_EVICT_SLOW_AHB_RD_CACHE_Msk 0x10000UL
#define MAIN_REG_CTRL_EVICT_FAST_AHB_RD_CACHE_Pos 17UL
#define MAIN_REG_CTRL_EVICT_FAST_AHB_RD_CACHE_Msk 0x20000UL
#define MAIN_REG_CTRL_NUM_SLOW_AHB_STALL_CYCLES_Pos 20UL
#define MAIN_REG_CTRL_NUM_SLOW_AHB_STALL_CYCLES_Msk 0x700000UL
#define MAIN_REG_CTRL_STALL_SLOW_AHB_WRITE_Pos  23UL
#define MAIN_REG_CTRL_STALL_SLOW_AHB_WRITE_Msk  0x800000UL
#define MAIN_REG_CTRL_NUM_FAST_AHB_STALL_CYCLES_Pos 24UL
#define MAIN_REG_CTRL_NUM_FAST_AHB_STALL_CYCLES_Msk 0x7000000UL
#define MAIN_REG_CTRL_STALL_FAST_AHB_WRITE_Pos  27UL
#define MAIN_REG_CTRL_STALL_FAST_AHB_WRITE_Msk  0x8000000UL
#define MAIN_REG_CTRL_IP_ENABLED_Pos            31UL
#define MAIN_REG_CTRL_IP_ENABLED_Msk            0x80000000UL
/* MAIN_REG.HBWSS_SPARE */
#define MAIN_REG_HBWSS_SPARE_SPARE_CFG_Pos      0UL
#define MAIN_REG_HBWSS_SPARE_SPARE_CFG_Msk      0xFFFFUL
/* MAIN_REG.SPCTRL */
#define MAIN_REG_SPCTRL_PWR_MODE_MACRO_0_Pos    0UL
#define MAIN_REG_SPCTRL_PWR_MODE_MACRO_0_Msk    0x3UL
#define MAIN_REG_SPCTRL_PWR_MODE_MACRO_1_Pos    2UL
#define MAIN_REG_SPCTRL_PWR_MODE_MACRO_1_Msk    0xCUL
#define MAIN_REG_SPCTRL_PWRUP_DELAY_Pos         8UL
#define MAIN_REG_SPCTRL_PWRUP_DELAY_Msk         0x3FF00UL
/* MAIN_REG.TR_CMD */
#define MAIN_REG_TR_CMD_TR_SEL_Pos              0UL
#define MAIN_REG_TR_CMD_TR_SEL_Msk              0xFFUL
#define MAIN_REG_TR_CMD_TRA_MINTENAB_Pos        28UL
#define MAIN_REG_TR_CMD_TRA_MINTENAB_Msk        0x10000000UL
#define MAIN_REG_TR_CMD_TR_EDGE_Pos             29UL
#define MAIN_REG_TR_CMD_TR_EDGE_Msk             0x20000000UL
#define MAIN_REG_TR_CMD_OUT_SEL_Pos             30UL
#define MAIN_REG_TR_CMD_OUT_SEL_Msk             0x40000000UL
#define MAIN_REG_TR_CMD_ACTIVATE_Pos            31UL
#define MAIN_REG_TR_CMD_ACTIVATE_Msk            0x80000000UL
/* MAIN_REG.TRA_CO_INTR */
#define MAIN_REG_TRA_CO_INTR_OINTR_BITS_Pos     0UL
#define MAIN_REG_TRA_CO_INTR_OINTR_BITS_Msk     0xFFFFUL
/* MAIN_REG.TRA_CO_INTR_SET */
#define MAIN_REG_TRA_CO_INTR_SET_OINTRS_BITS_Pos 0UL
#define MAIN_REG_TRA_CO_INTR_SET_OINTRS_BITS_Msk 0xFFFFUL
/* MAIN_REG.TRA_CO_INTR_MASK */
#define MAIN_REG_TRA_CO_INTR_MASK_OINTRM_BITS_Pos 0UL
#define MAIN_REG_TRA_CO_INTR_MASK_OINTRM_BITS_Msk 0xFFFFUL
/* MAIN_REG.TRA_CO_INTR_MASKED */
#define MAIN_REG_TRA_CO_INTR_MASKED_OINTRMD_BITS_Pos 0UL
#define MAIN_REG_TRA_CO_INTR_MASKED_OINTRMD_BITS_Msk 0xFFFFUL
/* MAIN_REG.DDFT_MUX */
#define MAIN_REG_DDFT_MUX_TOP_DDFT0_SEL_Pos     0UL
#define MAIN_REG_DDFT_MUX_TOP_DDFT0_SEL_Msk     0xFFUL
#define MAIN_REG_DDFT_MUX_TOP_DDFT1_SEL_Pos     8UL
#define MAIN_REG_DDFT_MUX_TOP_DDFT1_SEL_Msk     0xFF00UL
#define MAIN_REG_DDFT_MUX_HS_DDFT0_SEL_Pos      16UL
#define MAIN_REG_DDFT_MUX_HS_DDFT0_SEL_Msk      0xFF0000UL
#define MAIN_REG_DDFT_MUX_HS_DDFT1_SEL_Pos      24UL
#define MAIN_REG_DDFT_MUX_HS_DDFT1_SEL_Msk      0xFF000000UL


#endif /* _CYIP_MAIN_REG_H_ */


/* [] END OF FILE */
