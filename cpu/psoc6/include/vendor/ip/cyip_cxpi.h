/***************************************************************************//**
* \file cyip_cxpi.h
*
* \brief
* CXPI IP definitions
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

#ifndef _CYIP_CXPI_H_
#define _CYIP_CXPI_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                     CXPI
*******************************************************************************/

#define CXPI_CH_SECTION_SIZE                    0x00000100UL
#define CXPI_SECTION_SIZE                       0x00010000UL

/**
  * \brief CXPI channel structure (CXPI_CH)
  */
typedef struct {
  __IOM uint32_t CTL0;                          /*!< 0x00000000 Control 0 */
  __IOM uint32_t CTL1;                          /*!< 0x00000004 Control 1 */
  __IOM uint32_t CTL2;                          /*!< 0x00000008 Control 2 */
   __IM uint32_t STATUS;                        /*!< 0x0000000C Status */
  __IOM uint32_t CMD;                           /*!< 0x00000010 Command */
   __IM uint32_t RESERVED[11];
  __IOM uint32_t TX_RX_STATUS;                  /*!< 0x00000040 TX/RX status */
   __IM uint32_t RESERVED1[3];
  __IOM uint32_t TXPID_FI;                      /*!< 0x00000050 TXPID and Frame Information */
   __IM uint32_t RXPID_FI;                      /*!< 0x00000054 RXPID and Frame Information */
   __IM uint32_t CRC;                           /*!< 0x00000058 CRC */
   __IM uint32_t RESERVED2[9];
  __IOM uint32_t TX_FIFO_CTL;                   /*!< 0x00000080 TX FIFO control */
   __IM uint32_t TX_FIFO_STATUS;                /*!< 0x00000084 TX FIFO status */
   __OM uint32_t TX_FIFO_WR;                    /*!< 0x00000088 TX FIFO write */
   __IM uint32_t RESERVED3[5];
  __IOM uint32_t RX_FIFO_CTL;                   /*!< 0x000000A0 RX FIFO control */
   __IM uint32_t RX_FIFO_STATUS;                /*!< 0x000000A4 RX FIFO status */
   __IM uint32_t RX_FIFO_RD;                    /*!< 0x000000A8 RX FIFO read */
   __IM uint32_t RX_FIFO_RD_SILENT;             /*!< 0x000000AC RX FIFO silent read */
   __IM uint32_t RESERVED4[4];
  __IOM uint32_t INTR;                          /*!< 0x000000C0 Interrupt */
  __IOM uint32_t INTR_SET;                      /*!< 0x000000C4 Interrupt set */
  __IOM uint32_t INTR_MASK;                     /*!< 0x000000C8 Interrupt mask */
   __IM uint32_t INTR_MASKED;                   /*!< 0x000000CC Interrupt masked */
   __IM uint32_t RESERVED5[12];
} CXPI_CH_V1_Type;                              /*!< Size = 256 (0x100) */

/**
  * \brief CXPI (CXPI)
  */
typedef struct {
  __IOM uint32_t ERROR_CTL;                     /*!< 0x00000000 Error control */
  __IOM uint32_t TEST_CTL;                      /*!< 0x00000004 Test control */
   __IM uint32_t RESERVED[8190];
        CXPI_CH_V1_Type CH[4];                  /*!< 0x00008000 CXPI channel structure */
} CXPI_V1_Type;                                 /*!< Size = 33792 (0x8400) */


/* CXPI_CH.CTL0 */
#define CXPI_CH_CTL0_MODE_Pos                   0UL
#define CXPI_CH_CTL0_MODE_Msk                   0x1UL
#define CXPI_CH_CTL0_AUTO_EN_Pos                4UL
#define CXPI_CH_CTL0_AUTO_EN_Msk                0x10UL
#define CXPI_CH_CTL0_RXPIDZERO_CHECK_EN_Pos     7UL
#define CXPI_CH_CTL0_RXPIDZERO_CHECK_EN_Msk     0x80UL
#define CXPI_CH_CTL0_FILTER_EN_Pos              8UL
#define CXPI_CH_CTL0_FILTER_EN_Msk              0x100UL
#define CXPI_CH_CTL0_IFS_Pos                    16UL
#define CXPI_CH_CTL0_IFS_Msk                    0x1F0000UL
#define CXPI_CH_CTL0_IBS_Pos                    21UL
#define CXPI_CH_CTL0_IBS_Msk                    0x1E00000UL
#define CXPI_CH_CTL0_BIT_ERROR_IGNORE_Pos       27UL
#define CXPI_CH_CTL0_BIT_ERROR_IGNORE_Msk       0x8000000UL
#define CXPI_CH_CTL0_MASTER_Pos                 30UL
#define CXPI_CH_CTL0_MASTER_Msk                 0x40000000UL
#define CXPI_CH_CTL0_ENABLED_Pos                31UL
#define CXPI_CH_CTL0_ENABLED_Msk                0x80000000UL
/* CXPI_CH.CTL1 */
#define CXPI_CH_CTL1_T_LOW1_Pos                 0UL
#define CXPI_CH_CTL1_T_LOW1_Msk                 0x1FFUL
#define CXPI_CH_CTL1_T_LOW0_Pos                 12UL
#define CXPI_CH_CTL1_T_LOW0_Msk                 0x1FF000UL
#define CXPI_CH_CTL1_T_OFFSET_Pos               22UL
#define CXPI_CH_CTL1_T_OFFSET_Msk               0x7FC00000UL
/* CXPI_CH.CTL2 */
#define CXPI_CH_CTL2_RETRY_Pos                  0UL
#define CXPI_CH_CTL2_RETRY_Msk                  0x3UL
#define CXPI_CH_CTL2_T_WAKEUP_LENGTH_Pos        8UL
#define CXPI_CH_CTL2_T_WAKEUP_LENGTH_Msk        0x3F00UL
#define CXPI_CH_CTL2_TIMEOUT_LENGTH_Pos         16UL
#define CXPI_CH_CTL2_TIMEOUT_LENGTH_Msk         0xF0000UL
#define CXPI_CH_CTL2_TIMEOUT_SEL_Pos            30UL
#define CXPI_CH_CTL2_TIMEOUT_SEL_Msk            0xC0000000UL
/* CXPI_CH.STATUS */
#define CXPI_CH_STATUS_RETRIES_COUNT_Pos        0UL
#define CXPI_CH_STATUS_RETRIES_COUNT_Msk        0x3UL
#define CXPI_CH_STATUS_HEADER_RESPONSE_Pos      4UL
#define CXPI_CH_STATUS_HEADER_RESPONSE_Msk      0x10UL
#define CXPI_CH_STATUS_TX_BUSY_Pos              8UL
#define CXPI_CH_STATUS_TX_BUSY_Msk              0x100UL
#define CXPI_CH_STATUS_RX_BUSY_Pos              9UL
#define CXPI_CH_STATUS_RX_BUSY_Msk              0x200UL
#define CXPI_CH_STATUS_TX_DONE_Pos              12UL
#define CXPI_CH_STATUS_TX_DONE_Msk              0x1000UL
#define CXPI_CH_STATUS_RX_DONE_Pos              13UL
#define CXPI_CH_STATUS_RX_DONE_Msk              0x2000UL
#define CXPI_CH_STATUS_TIMEOUT_Pos              18UL
#define CXPI_CH_STATUS_TIMEOUT_Msk              0x40000UL
#define CXPI_CH_STATUS_TX_HEADER_ARB_LOST_Pos   19UL
#define CXPI_CH_STATUS_TX_HEADER_ARB_LOST_Msk   0x80000UL
#define CXPI_CH_STATUS_TX_BIT_ERROR_Pos         20UL
#define CXPI_CH_STATUS_TX_BIT_ERROR_Msk         0x100000UL
#define CXPI_CH_STATUS_RX_CRC_ERROR_Pos         21UL
#define CXPI_CH_STATUS_RX_CRC_ERROR_Msk         0x200000UL
#define CXPI_CH_STATUS_RX_HEADER_PARITY_ERROR_Pos 22UL
#define CXPI_CH_STATUS_RX_HEADER_PARITY_ERROR_Msk 0x400000UL
#define CXPI_CH_STATUS_RX_DATA_LENGTH_ERROR_Pos 23UL
#define CXPI_CH_STATUS_RX_DATA_LENGTH_ERROR_Msk 0x800000UL
#define CXPI_CH_STATUS_TX_DATA_LENGTH_ERROR_Pos 24UL
#define CXPI_CH_STATUS_TX_DATA_LENGTH_ERROR_Msk 0x1000000UL
#define CXPI_CH_STATUS_RX_OVERFLOW_ERROR_Pos    25UL
#define CXPI_CH_STATUS_RX_OVERFLOW_ERROR_Msk    0x2000000UL
#define CXPI_CH_STATUS_TX_OVERFLOW_ERROR_Pos    26UL
#define CXPI_CH_STATUS_TX_OVERFLOW_ERROR_Msk    0x4000000UL
#define CXPI_CH_STATUS_RX_UNDERFLOW_ERROR_Pos   27UL
#define CXPI_CH_STATUS_RX_UNDERFLOW_ERROR_Msk   0x8000000UL
#define CXPI_CH_STATUS_TX_UNDERFLOW_ERROR_Pos   28UL
#define CXPI_CH_STATUS_TX_UNDERFLOW_ERROR_Msk   0x10000000UL
#define CXPI_CH_STATUS_RX_FRAME_ERROR_Pos       29UL
#define CXPI_CH_STATUS_RX_FRAME_ERROR_Msk       0x20000000UL
#define CXPI_CH_STATUS_TX_FRAME_ERROR_Pos       30UL
#define CXPI_CH_STATUS_TX_FRAME_ERROR_Msk       0x40000000UL
/* CXPI_CH.CMD */
#define CXPI_CH_CMD_TX_HEADER_Pos               0UL
#define CXPI_CH_CMD_TX_HEADER_Msk               0x1UL
#define CXPI_CH_CMD_TX_RESPONSE_Pos             1UL
#define CXPI_CH_CMD_TX_RESPONSE_Msk             0x2UL
#define CXPI_CH_CMD_SLEEP_Pos                   2UL
#define CXPI_CH_CMD_SLEEP_Msk                   0x4UL
#define CXPI_CH_CMD_WAKE_TO_STANDBY_Pos         3UL
#define CXPI_CH_CMD_WAKE_TO_STANDBY_Msk         0x8UL
#define CXPI_CH_CMD_TX_WAKE_PULSE_Pos           4UL
#define CXPI_CH_CMD_TX_WAKE_PULSE_Msk           0x10UL
#define CXPI_CH_CMD_IFS_WAIT_Pos                5UL
#define CXPI_CH_CMD_IFS_WAIT_Msk                0x20UL
#define CXPI_CH_CMD_RX_HEADER_Pos               8UL
#define CXPI_CH_CMD_RX_HEADER_Msk               0x100UL
#define CXPI_CH_CMD_RX_RESPONSE_Pos             9UL
#define CXPI_CH_CMD_RX_RESPONSE_Msk             0x200UL
/* CXPI_CH.TX_RX_STATUS */
#define CXPI_CH_TX_RX_STATUS_TX_IN_Pos          16UL
#define CXPI_CH_TX_RX_STATUS_TX_IN_Msk          0x10000UL
#define CXPI_CH_TX_RX_STATUS_RX_IN_Pos          17UL
#define CXPI_CH_TX_RX_STATUS_RX_IN_Msk          0x20000UL
#define CXPI_CH_TX_RX_STATUS_TX_OUT_Pos         24UL
#define CXPI_CH_TX_RX_STATUS_TX_OUT_Msk         0x1000000UL
#define CXPI_CH_TX_RX_STATUS_EN_OUT_Pos         26UL
#define CXPI_CH_TX_RX_STATUS_EN_OUT_Msk         0x4000000UL
/* CXPI_CH.TXPID_FI */
#define CXPI_CH_TXPID_FI_PID_Pos                0UL
#define CXPI_CH_TXPID_FI_PID_Msk                0xFFUL
#define CXPI_CH_TXPID_FI_FI_Pos                 8UL
#define CXPI_CH_TXPID_FI_FI_Msk                 0xFF00UL
#define CXPI_CH_TXPID_FI_DLCEXT_Pos             16UL
#define CXPI_CH_TXPID_FI_DLCEXT_Msk             0xFF0000UL
/* CXPI_CH.RXPID_FI */
#define CXPI_CH_RXPID_FI_PID_Pos                0UL
#define CXPI_CH_RXPID_FI_PID_Msk                0xFFUL
#define CXPI_CH_RXPID_FI_FI_Pos                 8UL
#define CXPI_CH_RXPID_FI_FI_Msk                 0xFF00UL
#define CXPI_CH_RXPID_FI_DLCEXT_Pos             16UL
#define CXPI_CH_RXPID_FI_DLCEXT_Msk             0xFF0000UL
/* CXPI_CH.CRC */
#define CXPI_CH_CRC_RXCRC1_Pos                  0UL
#define CXPI_CH_CRC_RXCRC1_Msk                  0xFFUL
#define CXPI_CH_CRC_RXCRC2_Pos                  8UL
#define CXPI_CH_CRC_RXCRC2_Msk                  0xFF00UL
#define CXPI_CH_CRC_TXCRC1_Pos                  16UL
#define CXPI_CH_CRC_TXCRC1_Msk                  0xFF0000UL
#define CXPI_CH_CRC_TXCRC2_Pos                  24UL
#define CXPI_CH_CRC_TXCRC2_Msk                  0xFF000000UL
/* CXPI_CH.TX_FIFO_CTL */
#define CXPI_CH_TX_FIFO_CTL_TRIGGER_LEVEL_Pos   0UL
#define CXPI_CH_TX_FIFO_CTL_TRIGGER_LEVEL_Msk   0x1FUL
#define CXPI_CH_TX_FIFO_CTL_CLEAR_Pos           16UL
#define CXPI_CH_TX_FIFO_CTL_CLEAR_Msk           0x10000UL
#define CXPI_CH_TX_FIFO_CTL_FREEZE_Pos          17UL
#define CXPI_CH_TX_FIFO_CTL_FREEZE_Msk          0x20000UL
/* CXPI_CH.TX_FIFO_STATUS */
#define CXPI_CH_TX_FIFO_STATUS_USED_Pos         0UL
#define CXPI_CH_TX_FIFO_STATUS_USED_Msk         0x1FUL
#define CXPI_CH_TX_FIFO_STATUS_AVAIL_Pos        16UL
#define CXPI_CH_TX_FIFO_STATUS_AVAIL_Msk        0x1F0000UL
/* CXPI_CH.TX_FIFO_WR */
#define CXPI_CH_TX_FIFO_WR_DATA_Pos             0UL
#define CXPI_CH_TX_FIFO_WR_DATA_Msk             0xFFUL
/* CXPI_CH.RX_FIFO_CTL */
#define CXPI_CH_RX_FIFO_CTL_TRIGGER_LEVEL_Pos   0UL
#define CXPI_CH_RX_FIFO_CTL_TRIGGER_LEVEL_Msk   0x1FUL
#define CXPI_CH_RX_FIFO_CTL_CLEAR_Pos           16UL
#define CXPI_CH_RX_FIFO_CTL_CLEAR_Msk           0x10000UL
#define CXPI_CH_RX_FIFO_CTL_FREEZE_Pos          17UL
#define CXPI_CH_RX_FIFO_CTL_FREEZE_Msk          0x20000UL
/* CXPI_CH.RX_FIFO_STATUS */
#define CXPI_CH_RX_FIFO_STATUS_USED_Pos         0UL
#define CXPI_CH_RX_FIFO_STATUS_USED_Msk         0x1FUL
#define CXPI_CH_RX_FIFO_STATUS_AVAIL_Pos        16UL
#define CXPI_CH_RX_FIFO_STATUS_AVAIL_Msk        0x1F0000UL
/* CXPI_CH.RX_FIFO_RD */
#define CXPI_CH_RX_FIFO_RD_DATA_Pos             0UL
#define CXPI_CH_RX_FIFO_RD_DATA_Msk             0xFFUL
/* CXPI_CH.RX_FIFO_RD_SILENT */
#define CXPI_CH_RX_FIFO_RD_SILENT_DATA_Pos      0UL
#define CXPI_CH_RX_FIFO_RD_SILENT_DATA_Msk      0xFFUL
/* CXPI_CH.INTR */
#define CXPI_CH_INTR_TX_HEADER_DONE_Pos         0UL
#define CXPI_CH_INTR_TX_HEADER_DONE_Msk         0x1UL
#define CXPI_CH_INTR_TX_RESPONSE_DONE_Pos       1UL
#define CXPI_CH_INTR_TX_RESPONSE_DONE_Msk       0x2UL
#define CXPI_CH_INTR_TX_WAKEUP_DONE_Pos         3UL
#define CXPI_CH_INTR_TX_WAKEUP_DONE_Msk         0x8UL
#define CXPI_CH_INTR_TX_FIFO_TRIGGER_Pos        4UL
#define CXPI_CH_INTR_TX_FIFO_TRIGGER_Msk        0x10UL
#define CXPI_CH_INTR_RX_HEADER_DONE_Pos         8UL
#define CXPI_CH_INTR_RX_HEADER_DONE_Msk         0x100UL
#define CXPI_CH_INTR_RX_RESPONSE_DONE_Pos       9UL
#define CXPI_CH_INTR_RX_RESPONSE_DONE_Msk       0x200UL
#define CXPI_CH_INTR_RX_WAKEUP_DETECT_Pos       10UL
#define CXPI_CH_INTR_RX_WAKEUP_DETECT_Msk       0x400UL
#define CXPI_CH_INTR_RX_FIFO_TRIGGER_Pos        11UL
#define CXPI_CH_INTR_RX_FIFO_TRIGGER_Msk        0x800UL
#define CXPI_CH_INTR_RX_HEADER_PID_DONE_Pos     12UL
#define CXPI_CH_INTR_RX_HEADER_PID_DONE_Msk     0x1000UL
#define CXPI_CH_INTR_TXRX_COMPLETE_Pos          13UL
#define CXPI_CH_INTR_TXRX_COMPLETE_Msk          0x2000UL
#define CXPI_CH_INTR_TIMEOUT_Pos                18UL
#define CXPI_CH_INTR_TIMEOUT_Msk                0x40000UL
#define CXPI_CH_INTR_TX_HEADER_ARB_LOST_Pos     19UL
#define CXPI_CH_INTR_TX_HEADER_ARB_LOST_Msk     0x80000UL
#define CXPI_CH_INTR_TX_BIT_ERROR_Pos           20UL
#define CXPI_CH_INTR_TX_BIT_ERROR_Msk           0x100000UL
#define CXPI_CH_INTR_RX_CRC_ERROR_Pos           21UL
#define CXPI_CH_INTR_RX_CRC_ERROR_Msk           0x200000UL
#define CXPI_CH_INTR_RX_HEADER_PARITY_ERROR_Pos 22UL
#define CXPI_CH_INTR_RX_HEADER_PARITY_ERROR_Msk 0x400000UL
#define CXPI_CH_INTR_RX_DATA_LENGTH_ERROR_Pos   23UL
#define CXPI_CH_INTR_RX_DATA_LENGTH_ERROR_Msk   0x800000UL
#define CXPI_CH_INTR_TX_DATA_LENGTH_ERROR_Pos   24UL
#define CXPI_CH_INTR_TX_DATA_LENGTH_ERROR_Msk   0x1000000UL
#define CXPI_CH_INTR_RX_OVERFLOW_ERROR_Pos      25UL
#define CXPI_CH_INTR_RX_OVERFLOW_ERROR_Msk      0x2000000UL
#define CXPI_CH_INTR_TX_OVERFLOW_ERROR_Pos      26UL
#define CXPI_CH_INTR_TX_OVERFLOW_ERROR_Msk      0x4000000UL
#define CXPI_CH_INTR_RX_UNDERFLOW_ERROR_Pos     27UL
#define CXPI_CH_INTR_RX_UNDERFLOW_ERROR_Msk     0x8000000UL
#define CXPI_CH_INTR_TX_UNDERFLOW_ERROR_Pos     28UL
#define CXPI_CH_INTR_TX_UNDERFLOW_ERROR_Msk     0x10000000UL
#define CXPI_CH_INTR_RX_FRAME_ERROR_Pos         29UL
#define CXPI_CH_INTR_RX_FRAME_ERROR_Msk         0x20000000UL
#define CXPI_CH_INTR_TX_FRAME_ERROR_Pos         30UL
#define CXPI_CH_INTR_TX_FRAME_ERROR_Msk         0x40000000UL
/* CXPI_CH.INTR_SET */
#define CXPI_CH_INTR_SET_TX_HEADER_DONE_Pos     0UL
#define CXPI_CH_INTR_SET_TX_HEADER_DONE_Msk     0x1UL
#define CXPI_CH_INTR_SET_TX_RESPONSE_DONE_Pos   1UL
#define CXPI_CH_INTR_SET_TX_RESPONSE_DONE_Msk   0x2UL
#define CXPI_CH_INTR_SET_TX_WAKEUP_DONE_Pos     3UL
#define CXPI_CH_INTR_SET_TX_WAKEUP_DONE_Msk     0x8UL
#define CXPI_CH_INTR_SET_TX_FIFO_TRIGGER_Pos    4UL
#define CXPI_CH_INTR_SET_TX_FIFO_TRIGGER_Msk    0x10UL
#define CXPI_CH_INTR_SET_RX_HEADER_DONE_Pos     8UL
#define CXPI_CH_INTR_SET_RX_HEADER_DONE_Msk     0x100UL
#define CXPI_CH_INTR_SET_RX_RESPONSE_DONE_Pos   9UL
#define CXPI_CH_INTR_SET_RX_RESPONSE_DONE_Msk   0x200UL
#define CXPI_CH_INTR_SET_RX_WAKEUP_DETECT_Pos   10UL
#define CXPI_CH_INTR_SET_RX_WAKEUP_DETECT_Msk   0x400UL
#define CXPI_CH_INTR_SET_RX_FIFO_TRIGGER_Pos    11UL
#define CXPI_CH_INTR_SET_RX_FIFO_TRIGGER_Msk    0x800UL
#define CXPI_CH_INTR_SET_RX_HEADER_PID_DONE_Pos 12UL
#define CXPI_CH_INTR_SET_RX_HEADER_PID_DONE_Msk 0x1000UL
#define CXPI_CH_INTR_SET_TXRX_COMPLETE_Pos      13UL
#define CXPI_CH_INTR_SET_TXRX_COMPLETE_Msk      0x2000UL
#define CXPI_CH_INTR_SET_TIMEOUT_Pos            18UL
#define CXPI_CH_INTR_SET_TIMEOUT_Msk            0x40000UL
#define CXPI_CH_INTR_SET_TX_HEADER_ARB_LOST_Pos 19UL
#define CXPI_CH_INTR_SET_TX_HEADER_ARB_LOST_Msk 0x80000UL
#define CXPI_CH_INTR_SET_TX_BIT_ERROR_Pos       20UL
#define CXPI_CH_INTR_SET_TX_BIT_ERROR_Msk       0x100000UL
#define CXPI_CH_INTR_SET_RX_CRC_ERROR_Pos       21UL
#define CXPI_CH_INTR_SET_RX_CRC_ERROR_Msk       0x200000UL
#define CXPI_CH_INTR_SET_RX_HEADER_PARITY_ERROR_Pos 22UL
#define CXPI_CH_INTR_SET_RX_HEADER_PARITY_ERROR_Msk 0x400000UL
#define CXPI_CH_INTR_SET_RX_DATA_LENGTH_ERROR_Pos 23UL
#define CXPI_CH_INTR_SET_RX_DATA_LENGTH_ERROR_Msk 0x800000UL
#define CXPI_CH_INTR_SET_TX_DATA_LENGTH_ERROR_Pos 24UL
#define CXPI_CH_INTR_SET_TX_DATA_LENGTH_ERROR_Msk 0x1000000UL
#define CXPI_CH_INTR_SET_RX_OVERFLOW_ERROR_Pos  25UL
#define CXPI_CH_INTR_SET_RX_OVERFLOW_ERROR_Msk  0x2000000UL
#define CXPI_CH_INTR_SET_TX_OVERFLOW_ERROR_Pos  26UL
#define CXPI_CH_INTR_SET_TX_OVERFLOW_ERROR_Msk  0x4000000UL
#define CXPI_CH_INTR_SET_RX_UNDERFLOW_ERROR_Pos 27UL
#define CXPI_CH_INTR_SET_RX_UNDERFLOW_ERROR_Msk 0x8000000UL
#define CXPI_CH_INTR_SET_TX_UNDERFLOW_ERROR_Pos 28UL
#define CXPI_CH_INTR_SET_TX_UNDERFLOW_ERROR_Msk 0x10000000UL
#define CXPI_CH_INTR_SET_RX_FRAME_ERROR_Pos     29UL
#define CXPI_CH_INTR_SET_RX_FRAME_ERROR_Msk     0x20000000UL
#define CXPI_CH_INTR_SET_TX_FRAME_ERROR_Pos     30UL
#define CXPI_CH_INTR_SET_TX_FRAME_ERROR_Msk     0x40000000UL
/* CXPI_CH.INTR_MASK */
#define CXPI_CH_INTR_MASK_TX_HEADER_DONE_Pos    0UL
#define CXPI_CH_INTR_MASK_TX_HEADER_DONE_Msk    0x1UL
#define CXPI_CH_INTR_MASK_TX_RESPONSE_DONE_Pos  1UL
#define CXPI_CH_INTR_MASK_TX_RESPONSE_DONE_Msk  0x2UL
#define CXPI_CH_INTR_MASK_TX_WAKEUP_DONE_Pos    3UL
#define CXPI_CH_INTR_MASK_TX_WAKEUP_DONE_Msk    0x8UL
#define CXPI_CH_INTR_MASK_TX_FIFO_TRIGGER_Pos   4UL
#define CXPI_CH_INTR_MASK_TX_FIFO_TRIGGER_Msk   0x10UL
#define CXPI_CH_INTR_MASK_RX_HEADER_DONE_Pos    8UL
#define CXPI_CH_INTR_MASK_RX_HEADER_DONE_Msk    0x100UL
#define CXPI_CH_INTR_MASK_RX_RESPONSE_DONE_Pos  9UL
#define CXPI_CH_INTR_MASK_RX_RESPONSE_DONE_Msk  0x200UL
#define CXPI_CH_INTR_MASK_RX_WAKEUP_DETECT_Pos  10UL
#define CXPI_CH_INTR_MASK_RX_WAKEUP_DETECT_Msk  0x400UL
#define CXPI_CH_INTR_MASK_RX_FIFO_TRIGGER_Pos   11UL
#define CXPI_CH_INTR_MASK_RX_FIFO_TRIGGER_Msk   0x800UL
#define CXPI_CH_INTR_MASK_RX_HEADER_PID_DONE_Pos 12UL
#define CXPI_CH_INTR_MASK_RX_HEADER_PID_DONE_Msk 0x1000UL
#define CXPI_CH_INTR_MASK_TXRX_COMPLETE_Pos     13UL
#define CXPI_CH_INTR_MASK_TXRX_COMPLETE_Msk     0x2000UL
#define CXPI_CH_INTR_MASK_TIMEOUT_Pos           18UL
#define CXPI_CH_INTR_MASK_TIMEOUT_Msk           0x40000UL
#define CXPI_CH_INTR_MASK_TX_HEADER_ARB_LOST_Pos 19UL
#define CXPI_CH_INTR_MASK_TX_HEADER_ARB_LOST_Msk 0x80000UL
#define CXPI_CH_INTR_MASK_TX_BIT_ERROR_Pos      20UL
#define CXPI_CH_INTR_MASK_TX_BIT_ERROR_Msk      0x100000UL
#define CXPI_CH_INTR_MASK_RX_CRC_ERROR_Pos      21UL
#define CXPI_CH_INTR_MASK_RX_CRC_ERROR_Msk      0x200000UL
#define CXPI_CH_INTR_MASK_RX_HEADER_PARITY_ERROR_Pos 22UL
#define CXPI_CH_INTR_MASK_RX_HEADER_PARITY_ERROR_Msk 0x400000UL
#define CXPI_CH_INTR_MASK_RX_DATA_LENGTH_ERROR_Pos 23UL
#define CXPI_CH_INTR_MASK_RX_DATA_LENGTH_ERROR_Msk 0x800000UL
#define CXPI_CH_INTR_MASK_TX_DATA_LENGTH_ERROR_Pos 24UL
#define CXPI_CH_INTR_MASK_TX_DATA_LENGTH_ERROR_Msk 0x1000000UL
#define CXPI_CH_INTR_MASK_RX_OVERFLOW_ERROR_Pos 25UL
#define CXPI_CH_INTR_MASK_RX_OVERFLOW_ERROR_Msk 0x2000000UL
#define CXPI_CH_INTR_MASK_TX_OVERFLOW_ERROR_Pos 26UL
#define CXPI_CH_INTR_MASK_TX_OVERFLOW_ERROR_Msk 0x4000000UL
#define CXPI_CH_INTR_MASK_RX_UNDERFLOW_ERROR_Pos 27UL
#define CXPI_CH_INTR_MASK_RX_UNDERFLOW_ERROR_Msk 0x8000000UL
#define CXPI_CH_INTR_MASK_TX_UNDERFLOW_ERROR_Pos 28UL
#define CXPI_CH_INTR_MASK_TX_UNDERFLOW_ERROR_Msk 0x10000000UL
#define CXPI_CH_INTR_MASK_RX_FRAME_ERROR_Pos    29UL
#define CXPI_CH_INTR_MASK_RX_FRAME_ERROR_Msk    0x20000000UL
#define CXPI_CH_INTR_MASK_TX_FRAME_ERROR_Pos    30UL
#define CXPI_CH_INTR_MASK_TX_FRAME_ERROR_Msk    0x40000000UL
/* CXPI_CH.INTR_MASKED */
#define CXPI_CH_INTR_MASKED_TX_HEADER_DONE_Pos  0UL
#define CXPI_CH_INTR_MASKED_TX_HEADER_DONE_Msk  0x1UL
#define CXPI_CH_INTR_MASKED_TX_RESPONSE_DONE_Pos 1UL
#define CXPI_CH_INTR_MASKED_TX_RESPONSE_DONE_Msk 0x2UL
#define CXPI_CH_INTR_MASKED_TX_WAKEUP_DONE_Pos  3UL
#define CXPI_CH_INTR_MASKED_TX_WAKEUP_DONE_Msk  0x8UL
#define CXPI_CH_INTR_MASKED_TX_FIFO_TRIGGER_Pos 4UL
#define CXPI_CH_INTR_MASKED_TX_FIFO_TRIGGER_Msk 0x10UL
#define CXPI_CH_INTR_MASKED_RX_HEADER_DONE_Pos  8UL
#define CXPI_CH_INTR_MASKED_RX_HEADER_DONE_Msk  0x100UL
#define CXPI_CH_INTR_MASKED_RX_RESPONSE_DONE_Pos 9UL
#define CXPI_CH_INTR_MASKED_RX_RESPONSE_DONE_Msk 0x200UL
#define CXPI_CH_INTR_MASKED_RX_WAKEUP_DETECT_Pos 10UL
#define CXPI_CH_INTR_MASKED_RX_WAKEUP_DETECT_Msk 0x400UL
#define CXPI_CH_INTR_MASKED_RX_FIFO_TRIGGER_Pos 11UL
#define CXPI_CH_INTR_MASKED_RX_FIFO_TRIGGER_Msk 0x800UL
#define CXPI_CH_INTR_MASKED_RX_HEADER_PID_DONE_Pos 12UL
#define CXPI_CH_INTR_MASKED_RX_HEADER_PID_DONE_Msk 0x1000UL
#define CXPI_CH_INTR_MASKED_TXRX_COMPLETE_Pos   13UL
#define CXPI_CH_INTR_MASKED_TXRX_COMPLETE_Msk   0x2000UL
#define CXPI_CH_INTR_MASKED_TIMEOUT_Pos         18UL
#define CXPI_CH_INTR_MASKED_TIMEOUT_Msk         0x40000UL
#define CXPI_CH_INTR_MASKED_TX_HEADER_ARB_LOST_Pos 19UL
#define CXPI_CH_INTR_MASKED_TX_HEADER_ARB_LOST_Msk 0x80000UL
#define CXPI_CH_INTR_MASKED_TX_BIT_ERROR_Pos    20UL
#define CXPI_CH_INTR_MASKED_TX_BIT_ERROR_Msk    0x100000UL
#define CXPI_CH_INTR_MASKED_RX_CRC_ERROR_Pos    21UL
#define CXPI_CH_INTR_MASKED_RX_CRC_ERROR_Msk    0x200000UL
#define CXPI_CH_INTR_MASKED_RX_HEADER_PARITY_ERROR_Pos 22UL
#define CXPI_CH_INTR_MASKED_RX_HEADER_PARITY_ERROR_Msk 0x400000UL
#define CXPI_CH_INTR_MASKED_RX_DATA_LENGTH_ERROR_Pos 23UL
#define CXPI_CH_INTR_MASKED_RX_DATA_LENGTH_ERROR_Msk 0x800000UL
#define CXPI_CH_INTR_MASKED_TX_DATA_LENGTH_ERROR_Pos 24UL
#define CXPI_CH_INTR_MASKED_TX_DATA_LENGTH_ERROR_Msk 0x1000000UL
#define CXPI_CH_INTR_MASKED_RX_OVERFLOW_ERROR_Pos 25UL
#define CXPI_CH_INTR_MASKED_RX_OVERFLOW_ERROR_Msk 0x2000000UL
#define CXPI_CH_INTR_MASKED_TX_OVERFLOW_ERROR_Pos 26UL
#define CXPI_CH_INTR_MASKED_TX_OVERFLOW_ERROR_Msk 0x4000000UL
#define CXPI_CH_INTR_MASKED_RX_UNDERFLOW_ERROR_Pos 27UL
#define CXPI_CH_INTR_MASKED_RX_UNDERFLOW_ERROR_Msk 0x8000000UL
#define CXPI_CH_INTR_MASKED_TX_UNDERFLOW_ERROR_Pos 28UL
#define CXPI_CH_INTR_MASKED_TX_UNDERFLOW_ERROR_Msk 0x10000000UL
#define CXPI_CH_INTR_MASKED_RX_FRAME_ERROR_Pos  29UL
#define CXPI_CH_INTR_MASKED_RX_FRAME_ERROR_Msk  0x20000000UL
#define CXPI_CH_INTR_MASKED_TX_FRAME_ERROR_Pos  30UL
#define CXPI_CH_INTR_MASKED_TX_FRAME_ERROR_Msk  0x40000000UL


/* CXPI.ERROR_CTL */
#define CXPI_ERROR_CTL_CH_IDX_Pos               0UL
#define CXPI_ERROR_CTL_CH_IDX_Msk               0x1FUL
#define CXPI_ERROR_CTL_TX_CRC_ERROR_Pos         18UL
#define CXPI_ERROR_CTL_TX_CRC_ERROR_Msk         0x40000UL
#define CXPI_ERROR_CTL_TX_PID_PARITY_ERROR_Pos  19UL
#define CXPI_ERROR_CTL_TX_PID_PARITY_ERROR_Msk  0x80000UL
#define CXPI_ERROR_CTL_TX_DATA_LENGTH_ERROR_Pos 20UL
#define CXPI_ERROR_CTL_TX_DATA_LENGTH_ERROR_Msk 0x100000UL
#define CXPI_ERROR_CTL_TX_DATA_STOP_ERROR_Pos   25UL
#define CXPI_ERROR_CTL_TX_DATA_STOP_ERROR_Msk   0x2000000UL
#define CXPI_ERROR_CTL_ENABLED_Pos              31UL
#define CXPI_ERROR_CTL_ENABLED_Msk              0x80000000UL
/* CXPI.TEST_CTL */
#define CXPI_TEST_CTL_CH_IDX_Pos                0UL
#define CXPI_TEST_CTL_CH_IDX_Msk                0x1FUL
#define CXPI_TEST_CTL_MODE_Pos                  16UL
#define CXPI_TEST_CTL_MODE_Msk                  0x10000UL
#define CXPI_TEST_CTL_ENABLED_Pos               31UL
#define CXPI_TEST_CTL_ENABLED_Msk               0x80000000UL


#endif /* _CYIP_CXPI_H_ */


/* [] END OF FILE */
