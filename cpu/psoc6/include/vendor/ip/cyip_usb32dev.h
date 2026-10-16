/***************************************************************************//**
* \file cyip_usb32dev.h
*
* \brief
* USB32DEV IP definitions
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

#ifndef _CYIP_USB32DEV_H_
#define _CYIP_USB32DEV_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                   USB32DEV
*******************************************************************************/

#define USB32DEV_MAIN_SECTION_SIZE 0x00001000UL
#define USB32DEV_EPM_SECTION_SIZE 0x00001000UL
#define USB32DEV_LNK_SECTION_SIZE 0x00001000UL
#define USB32DEV_PROT_SECTION_SIZE 0x00001000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_SECTION_SIZE 0x00000800UL
#define USB32DEV_PHYSS_USB40PHY_RX_SECTION_SIZE 0x00000100UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SECTION_SIZE 0x00000100UL
#define USB32DEV_PHYSS_USB40PHY_SECTION_SIZE 0x00001000UL
#define USB32DEV_PHYSS_SECTION_SIZE 0x00002000UL
#define USB32DEV_ADAPTER_DMA_SCK_SECTION_SIZE 0x00000080UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_SECTION_SIZE 0x00000100UL
#define USB32DEV_ADAPTER_DMA_SECTION_SIZE 0x00010000UL
#define USB32DEV_SECTION_SIZE          0x00030000UL

/**
  * \brief USB32 Main Register (USB32DEV_MAIN)
  */
typedef struct {
  __IOM uint32_t CTRL;                          /*!< 0x00000000 Main Control Register */
  __IOM uint32_t INTR;                          /*!< 0x00000004 Main Active Interrupt Register */
  __IOM uint32_t INTR_SET;                      /*!< 0x00000008 Main Active Interrupt Set Register */
  __IOM uint32_t INTR_MASK;                     /*!< 0x0000000C Main Active Interrupt Mask Register */
   __IM uint32_t INTR_MASKED;                   /*!< 0x00000010 Main Active Interrupt Masked Register */
  __IOM uint32_t DDFT_MUX;                      /*!< 0x00000014 DDFT0/1 mux selection Register */
  __IOM uint32_t GPIO_DDFT_MUX;                 /*!< 0x00000018 GPIO DDFT0/1 mux selection Register */
  __IOM uint32_t DEBUG_CONFIG;                  /*!< 0x0000001C Debug Configuration Register */
  __IOM uint32_t DEBUG_MUX_SEL;                 /*!< 0x00000020 Debug MUX selection Configuration Register */
  __IOM uint32_t LOOPBACK;                      /*!< 0x00000024 LOOPBACK Configuration Register */
   __IM uint32_t RESERVED;
  __IOM uint32_t BUG_FIX;                       /*!< 0x0000002C BUG Fix Configuration Register */
   __IM uint32_t RESERVED1[1012];
} USB32DEV_MAIN_V1_Type;      /*!< Size = 4096 (0x1000) */

/**
  * \brief USB32 Controller End Point Manager Registers (USB32DEV_EPM)
  */
typedef struct {
  __IOM uint32_t EEPM_CS;                       /*!< 0x00000000 Egress EPM Retry Buffer Status */
  __IOM uint32_t IEPM_CS;                       /*!< 0x00000004 Ingress EPM Control and Status */
  __IOM uint32_t IEPM_MULT;                     /*!< 0x00000008 Ingress EPM MULT function Control */
   __IM uint32_t RESERVED[13];
  __IOM uint32_t EEPM_ENDPOINT[16];             /*!< 0x00000040 Egress EPM per Endpoint Control and Status */
  __IOM uint32_t IEPM_ENDPOINT[16];             /*!< 0x00000080 Ingress EPM Per Endpoint Control and Status */
   __IM uint32_t IEPM_FIFO;                     /*!< 0x000000C0 Ingress EPM FIFO Entry */
  __IOM uint32_t EEPM_VALID;                    /*!< 0x000000C4 Egress EPM Retry Buffer Valid packet */
   __IM uint32_t RESERVED1[15];
  __IOM uint32_t EEPM_RETRY_OFFSET[16];         /*!< 0x00000104 Egress EPM Retry Buffer EndPoint offset */
  __IOM uint32_t EEPM_INJECT_CRC32_ERROR[16];   /*!< 0x00000144 Egress EPM CRC32 Error injection control */
   __IM uint32_t RESERVED2[927];
} USB32DEV_EPM_V1_Type;       /*!< Size = 4096 (0x1000) */

/**
  * \brief USB32 SuperSpeedPlus Device Controller Link Layer Registers (USB32DEV_LNK)
  */
typedef struct {
  __IOM uint32_t LNK_CONF;                      /*!< 0x00000000 Link Configuration Register */
  __IOM uint32_t LNK_INTR;                      /*!< 0x00000004 Link Interrupts */
  __IOM uint32_t LNK_INTR_SET;                  /*!< 0x00000008 Link Interrupts Set */
  __IOM uint32_t LNK_INTR_MASK;                 /*!< 0x0000000C Link Interrupts Mask */
   __IM uint32_t LNK_INTR_MASKED;               /*!< 0x00000010 Link Interrupts Masked */
  __IOM uint32_t LNK_ERROR_CONF;                /*!< 0x00000014 Link Error Counter Configuration */
  __IOM uint32_t LNK_ERROR_STATUS;              /*!< 0x00000018 Link Error Status Register */
   __IM uint32_t LNK_ERROR_COUNT;               /*!< 0x0000001C Error Counter Register */
  __IOM uint32_t LNK_ERROR_COUNT_THRESHOLD;     /*!< 0x00000020 Error Count Thresholds */
  __IOM uint32_t LNK_PHY_CONF;                  /*!< 0x00000024 USB 3.0 PHY Configuration */
   __IM uint32_t LNK_PHY_STATUS;                /*!< 0x00000028 USB 3.0 PHY Status */
   __IM uint32_t RESERVED[7];
  __IOM uint32_t LNK_PHY_ERROR_CONF;            /*!< 0x00000048 PHY Error Counter Configuration */
  __IOM uint32_t LNK_PHY_ERROR_STATUS;          /*!< 0x0000004C PHY Error Status Register */
  __IOM uint32_t LNK_PHY_TRAINING_HOLDOFF;      /*!< 0x00000050 PHY Training Hold Off */
   __IM uint32_t LNK_COMMAND_WORD;              /*!< 0x00000054 Link Command Word (received) */
  __IOM uint32_t LNK_DEVICE_POWER_CONTROL;      /*!< 0x00000058 USB 3.0 Device Power State Control */
  __IOM uint32_t LNK_LTSSM_STATE;               /*!< 0x0000005C Link Training Status State Machine (LTSSM) State */
  __IOM uint32_t LNK_LOOPBACK_INIT;             /*!< 0x00000060 Loopback LFSR Initial Value */
  __IOM uint32_t LNK_LOOPBACK_GENERATOR;        /*!< 0x00000064 Loopback LFSR Transmitter Generator Polynomial */
  __IOM uint32_t LNK_LTSSM_OBSERVE;             /*!< 0x00000068 Link Training Status State Machine (LTSSM) Observation */
  __IOM uint32_t LNK_LFPS_OBSERVE;              /*!< 0x0000006C LFPS Receiver Observability */
  __IOM uint32_t LNK_LFPS_TX_POLLING_BURST;     /*!< 0x00000070 LFPS Polling Transmit Configuration */
  __IOM uint32_t LNK_LFPS_TX_POLLING_REPEAT;    /*!< 0x00000074 LFPS Polling Transmit Configuration */
  __IOM uint32_t LNK_LFPS_TX_PING_BURST;        /*!< 0x00000078 LFPS Ping Transmit Configuration */
  __IOM uint32_t LNK_LFPS_TX_PING_REPEAT;       /*!< 0x0000007C LFPS Ping Transmit Configuration */
  __IOM uint32_t LNK_LFPS_TX_U1_EXIT;           /*!< 0x00000080 LFPS U1_EXIT Transmit Configuration */
  __IOM uint32_t LNK_LFPS_TX_U2_EXIT;           /*!< 0x00000084 LFPS U2_EXIT Transmit Configuration */
  __IOM uint32_t LNK_LFPS_TX_U3_EXIT;           /*!< 0x00000088 LFPS U3 Exit Transmit Configuration */
  __IOM uint32_t LNK_LFPS_RX_POLLING_BURST;     /*!< 0x0000008C LFPS Polling Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_POLLING_REPEAT;    /*!< 0x00000090 LFPS Polling Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_PING;              /*!< 0x00000094 LFPS Ping Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_RESET;             /*!< 0x00000098 LFPS Reset Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_U1_EXIT;           /*!< 0x0000009C LFPS U1 Exit Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_U2_EXIT;           /*!< 0x000000A0 LFPS U2 Exit Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_U3_EXIT;           /*!< 0x000000A4 LFPS U3 Exit Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_U1_HANDSHAKE;      /*!< 0x000000A8 LFPS U1 Exit Handshake Configuration */
  __IOM uint32_t LNK_LFPS_RX_U2_HANDSHAKE;      /*!< 0x000000AC LFPS U2 Exit Handshake Configuration */
  __IOM uint32_t LNK_LFPS_RX_U3_HANDSHAKE;      /*!< 0x000000B0 LFPS U3 Exit Handshake Configuration */
  __IOM uint32_t LNK_LFPS_RX_LOOPBACK_EXIT;     /*!< 0x000000B4 LFPS Loopback Exit Detect Configuration */
  __IOM uint32_t LNK_LFPS_RX_LOOPBACK_HANDSHAKE; /*!< 0x000000B8 LFPS Loopback Exit Handshake Configuration */
   __IM uint32_t LNK_LFPS_RX_IDLE;              /*!< 0x000000BC LFPS Idle Time */
   __IM uint32_t LNK_LFPS_RX_BURST;             /*!< 0x000000C0 LFPS Burst Length */
   __IM uint32_t LNK_LFPS_RX_TIME;              /*!< 0x000000C4 LFPS Idle Counter */
   __IM uint32_t LNK_PENDING_HP_TIMER;          /*!< 0x000000C8 Header Packet LGOOD/LBAD Timer */
  __IOM uint32_t LNK_PENDING_HP_TIMEOUT;        /*!< 0x000000CC Header Packet LGOOD/LBAD Timeout */
   __IM uint32_t LNK_CREDIT_HP_TIMER;           /*!< 0x000000D0 Header Packet LCRDx Timer */
  __IOM uint32_t LNK_CREDIT_HP_TIMEOUT;         /*!< 0x000000D4 Header Packet LCRDx Timeout */
   __IM uint32_t LNK_PM_TIMER;                  /*!< 0x000000D8 Power Mode Timer */
  __IOM uint32_t LNK_PM_LC_TIMEOUT;             /*!< 0x000000DC Power Mode PM_LC_TIMER Timeout */
  __IOM uint32_t LNK_PM_ENTRY_TIMEOUT;          /*!< 0x000000E0 Power Mode PM_ENTRY_TIMER Timeout */
  __IOM uint32_t LNK_PM_UX_EXIT_TIMEOUT;        /*!< 0x000000E4 Power Mode Ux_EXIT_TIMER Timeout */
   __IM uint32_t LNK_LTSSM_TIMER;               /*!< 0x000000E8 LTSSM Timer Register */
  __IOM uint32_t LNK_LTSSM_TIMEOUT;             /*!< 0x000000EC LTSSM Timeout Observability Register */
   __IM uint32_t RESERVED1[2];
  __IOM uint32_t LNK_LTSSM_RX_DETECT_PERIOD;    /*!< 0x000000F8 LTSSM RxDetect Period */
  __IOM uint32_t LNK_LTSSM_LUP_PERIOD;          /*!< 0x000000FC LTSSM LUP Period */
  __IOM uint32_t LNK_LTSSM_SS_INACTIVE_PERIOD;  /*!< 0x00000100 LTSSM SS.Inactive Timeout */
  __IOM uint32_t LNK_LTSSM_POLLING_LFPS_TIMEOUT; /*!< 0x00000104 LTSSM Polling LFPS Timeout */
  __IOM uint32_t LNK_LTSSM_POLLING_ACTIVE_TIMEOUT; /*!< 0x00000108 LTSSM Polling Active Timeout */
  __IOM uint32_t LNK_LTSSM_POLLING_CONFIG_TIMEOUT; /*!< 0x0000010C LTSSM Polling Configuration Timeout */
  __IOM uint32_t LNK_LTSSM_POLLING_IDLE_TIMEOUT; /*!< 0x00000110 LTSSM Polling Idle Timeout */
  __IOM uint32_t LNK_LTSSM_U1_EXIT_TIMEOUT;     /*!< 0x00000114 LTSSM U1 Exit Timeout */
  __IOM uint32_t LNK_LTSSM_U2_EXIT_TIMEOUT;     /*!< 0x00000118 LTSSM U2 Exit Timeout */
  __IOM uint32_t LNK_LTSSM_U3_EXIT_TIMEOUT;     /*!< 0x0000011C LTSSM U3 Exit Timeout */
  __IOM uint32_t LNK_LTSSM_HOT_RESET_ACTIVE_TIMEOUT; /*!< 0x00000120 LTSSM Hot Reset Active Timeout */
  __IOM uint32_t LNK_LTSSM_HOT_RESET_EXIT_TIMEOUT; /*!< 0x00000124 LTSSM Hot Reset Exit Timeout */
  __IOM uint32_t LNK_LTSSM_LOOPBACK_EXIT_TIMEOUT; /*!< 0x00000128 LTSSM Loopback Exit Timeout */
  __IOM uint32_t LNK_LTSSM_RECOVERY_IDLE_TIMEOUT; /*!< 0x0000012C LTSSM Recovery Idle Timeout */
  __IOM uint32_t LNK_LTSSM_RECOVERY_ACTIVE_TIMEOUT; /*!< 0x00000130 LTSSM Recovery Active Timeout */
  __IOM uint32_t LNK_LTSSM_RECOVERY_CONFIG_TIMEOUT; /*!< 0x00000134 LTSSM Recovery.Configuration Timeout */
  __IOM uint32_t LNK_LTSSM_LDN_TIMEOUT;         /*!< 0x00000138 LTSSM LDN Received Timeout */
   __IM uint32_t LNK_LTSSM_LDN_TIMER;           /*!< 0x0000013C LTSSM LDN Received Timer */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_0;      /*!< 0x00000140 Compliance Pattern CP0 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_1;      /*!< 0x00000144 Compliance Pattern CP1 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_2;      /*!< 0x00000148 Compliance Pattern CP2 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_3;      /*!< 0x0000014C Compliance Pattern CP3 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_4;      /*!< 0x00000150 Compliance Pattern CP4 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_5;      /*!< 0x00000154 Compliance Pattern CP5 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_6;      /*!< 0x00000158 Compliance Pattern CP6 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_7;      /*!< 0x0000015C Compliance Pattern CP7 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_8;      /*!< 0x00000160 Compliance Pattern CP8 */
  __IOM uint32_t LNK_DEBUG_BUFFER_CTRL;         /*!< 0x00000164 Buffer direct access control */
   __IM uint32_t LNK_DATARATE_CHG_OBSERVE;      /*!< 0x00000168 Dtat Rate Observability */
  __IOM uint32_t LNK_LFPS_TX_POLLING_BURST_GEN2; /*!< 0x0000016C LFPS Polling Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_POLLING_REPEAT_GEN2; /*!< 0x00000170 LFPS Polling Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_PING_BURST_GEN2;   /*!< 0x00000174 LFPS Ping Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_PING_REPEAT_GEN2;  /*!< 0x00000178 LFPS Ping Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_U1_EXIT_GEN2;      /*!< 0x0000017C LFPS U1_EXIT Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_U2_EXIT_GEN2;      /*!< 0x00000180 LFPS U2_EXIT Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_U3_EXIT_GEN2;      /*!< 0x00000184 LFPS U3 Exit Transmit Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_POLLING_BURST_GEN2; /*!< 0x00000188 LFPS Polling Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_POLLING_REPEAT_GEN2; /*!< 0x0000018C LFPS Polling Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_PING_GEN2;         /*!< 0x00000190 LFPS Ping Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_RESET_GEN2;        /*!< 0x00000194 LFPS Reset Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_U1_EXIT_GEN2;      /*!< 0x00000198 LFPS U1 Exit Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_U2_EXIT_GEN2;      /*!< 0x0000019C LFPS U2 Exit Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_U1_HANDSHAKE_GEN2; /*!< 0x000001A0 LFPS U1 Exit Handshake Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_U2_HANDSHAKE_GEN2; /*!< 0x000001A4 LFPS U2 Exit Handshake Gen 2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_U3_HANDSHAKE_GEN2; /*!< 0x000001A8 LFPS U3 Exit Handshake Gen 2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LOOPBACK_EXIT_GEN2; /*!< 0x000001AC LFPS Loopback Exit Detect Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LOOPBACK_HANDSHAKE_GEN2; /*!< 0x000001B0 LFPS Loopback Exit Handshake Gen2 Configuration */
  __IOM uint32_t LNK_PENDING_HP_TIMEOUT_GEN2;   /*!< 0x000001B4 Header Packet LGOOD/LBAD Gen2 Timeout */
   __IM uint32_t RESERVED2[3];
  __IOM uint32_t LNK_CREDIT_HP_TIMEOUT_GEN2;    /*!< 0x000001C4 Header Packet LCRD_x Gen2 Timeout */
  __IOM uint32_t LNK_PM_LC_X2_TIMEOUT_GEN1;     /*!< 0x000001C8 Power Mode PM_LC_X2_TIMER Gen1 Timeout */
  __IOM uint32_t LNK_PM_LC_X1_TIMEOUT_GEN2;     /*!< 0x000001CC Power Mode PM_LC_X1_TIMER Gen2 Timeout */
  __IOM uint32_t LNK_PM_LC_X2_TIMEOUT_GEN2;     /*!< 0x000001D0 Power Mode PM_LC_X2_TIMER Gen2 Timeout */
  __IOM uint32_t LNK_PM_ENTRY_X2_TIMEOUT_GEN1;  /*!< 0x000001D4 Power Mode PM_ENTRY_X2_TIMER Gen1 Timeout */
  __IOM uint32_t LNK_PM_ENTRY_X1_TIMEOUT_GEN2;  /*!< 0x000001D8 Power Mode PM_ENTRY_X1_TIMER Gen2 Timeout */
  __IOM uint32_t LNK_PM_ENTRY_X2_TIMEOUT_GEN2;  /*!< 0x000001DC Power Mode PM_ENTRY_X2_TIMER Gen2 Timeout */
  __IOM uint32_t LNK_PM_UX_EXIT_TIMEOUT_GEN2;   /*!< 0x000001E0 Power Mode Ux_EXIT_TIMER Gen2 Timeout */
  __IOM uint32_t LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN1; /*!< 0x000001E4 Power Mode U1_MIN_RESIDENCY_TIMER Gen1 Timeout */
  __IOM uint32_t LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN2; /*!< 0x000001E8 Power Mode U1_MIN_RESIDENCY_TIMER Gen2 Timeout */
  __IOM uint32_t LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN1; /*!< 0x000001EC LFPS SCD Transmit Logic 0 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN1; /*!< 0x000001F0 LFPS SCD Transmit Logic 1 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_TX_SCD_END_REPEAT_GEN1; /*!< 0x000001F4 LFPS SCD Transmit End Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN1; /*!< 0x000001F8 LFPS SCD Receive Logic 0 Gen1 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN1; /*!< 0x000001FC LFPS SCD Receive Logic 1 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_RX_SCD_END_REPEAT_GEN1; /*!< 0x00000200 LFPS SCD Receive End Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN2; /*!< 0x00000204 LFPS SCD Transmit Logic 0 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN2; /*!< 0x00000208 LFPS SCD Transmit Logic 1 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_SCD_END_REPEAT_GEN2; /*!< 0x0000020C LFPS SCD Transmit End Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN2; /*!< 0x00000210 LFPS SCD Receive Logic 0 Gen2 Configuration */
   __IM uint32_t RESERVED3[3];
  __IOM uint32_t LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN2; /*!< 0x00000220 LFPS SCD Receive Logic 1 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_SCD_END_REPEAT_GEN2; /*!< 0x00000224 LFPS SCD Receive End Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_LBPS_TPWM_GEN1;    /*!< 0x00000228 LFPS LBPS Transmit tPWM Gen1 Configuration */
   __IM uint32_t RESERVED4[3];
  __IOM uint32_t LNK_LFPS_TX_LBPS_TLFPS0_GEN1;  /*!< 0x00000238 LFPS LBPS Transmit tLFPS-0 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_TX_LBPS_TLFPS1_GEN1;  /*!< 0x0000023C LFPS LBPS Transmit tLFPS-1 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LBPS_TPWM_GEN1;    /*!< 0x00000240 LFPS LBPS Receive tPWM Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LBPS_TLFPS0_GEN1;  /*!< 0x00000244 LFPS LBPS Receive tLFPS-0 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LBPS_TLFPS1_GEN1;  /*!< 0x00000248 LFPS LBPS Receive tLFPS-1 Gen1 Configuration */
  __IOM uint32_t LNK_LFPS_TX_LBPS_TPWM_GEN2;    /*!< 0x0000024C LFPS LBPS Transmit tPWM Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_LBPS_TLFPS0_GEN2;  /*!< 0x00000250 LFPS LBPS Transmit tLFPS-0 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_TX_LBPS_TLFPS1_GEN2;  /*!< 0x00000254 LFPS LBPS Transmit tLFPS-1 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LBPS_TPWM_GEN2;    /*!< 0x00000258 LFPS LBPS Receive tPWM Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LBPS_TLFPS0_GEN2;  /*!< 0x0000025C LFPS LBPS Receive tLFPS-0 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_RX_LBPS_TLFPS1_GEN2;  /*!< 0x00000260 LFPS LBPS Receive tLFPS-1 Gen2 Configuration */
  __IOM uint32_t LNK_LFPS_SCD_PATTERN;          /*!< 0x00000264 LFPS SCD Pattern Configuration */
  __IOM uint32_t LNK_TSEQ_COUNT_GEN1;           /*!< 0x00000268 TSEQ Count Gen1 Configuration */
  __IOM uint32_t LNK_TSEQ_COUNT_GEN2;           /*!< 0x0000026C TSEQ Count Gen2 Configuration */
  __IOM uint32_t LNK_SCD1_GEN2_HSK;             /*!< 0x00000270 SCD1 Handshake Gen2 Configuration */
  __IOM uint32_t LNK_SCD1_GEN2_TO_GEN1_HSK;     /*!< 0x00000274 SCD1 Handshake Gen2_to_Gen1 Configuration */
  __IOM uint32_t LNK_SCD1_OBSERVE;              /*!< 0x00000278 Polling.LFPS SCD1 Observability */
  __IOM uint32_t LNK_SCD2_GEN2_HSK;             /*!< 0x0000027C SCD2 Handshake Gen2 Configuration */
  __IOM uint32_t LNK_SCD2_GEN2_TO_GEN1_HSK;     /*!< 0x00000280 SCD2 Handshake Gen2_to_Gen1 Configuration */
  __IOM uint32_t LNK_SCD2_OBSERVE;              /*!< 0x00000284 Polling.LFPSPlus SCD2 Observability */
  __IOM uint32_t LNK_CAP_LBPM_HSK;              /*!< 0x00000288 Polling.PortMatch LBPM Handshake Configuration */
  __IOM uint32_t LNK_CAP_LBPM_OBSERVE;          /*!< 0x0000028C Polling.PortMatch LBPM Observability */
  __IOM uint32_t LNK_READY_LBPM_HSK;            /*!< 0x00000290 Polling.PortConfig LBPM Handshake Configuration */
  __IOM uint32_t LNK_READY_LBPM_OBSERVE;        /*!< 0x00000294 Polling.PortConfig LBPM Observability */
   __IM uint32_t LNK_RCVD_LBPM_OBSERVE;         /*!< 0x00000298 Received LBPM Observability */
  __IOM uint32_t LNK_LTSSM_SCD_LFPS_TIMEOUT;    /*!< 0x0000029C LTSSM SCD LFPS Timeout (SuperSpeedPlus Only) */
  __IOM uint32_t LNK_LTSSM_LBPM_LFPS_TIMEOUT;   /*!< 0x000002A0 LTSSM LBPM LFPS Timeout (SuperSpeedPlus Only) */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_9_TO_12; /*!< 0x000002A4 Compliance Pattern CP9 to CP12 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_13;     /*!< 0x000002A8 Compliance Pattern CP13 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_14;     /*!< 0x000002AC Compliance Pattern CP14 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_15;     /*!< 0x000002B0 Compliance Pattern CP15 */
  __IOM uint32_t LNK_COMPLIANCE_PATTERN_16;     /*!< 0x000002B4 Compliance Pattern CP16 */
   __IM uint32_t LNK_COMPLIANCE_PATTERN_OBSERVE; /*!< 0x000002B8 Compliance Pattern Observability */
  __IOM uint32_t LNK_SOFT_ERR_CNT_CONF;         /*!< 0x000002BC Soft Error Counter Configuration */
   __IM uint32_t LNK_SOFT_ERR_CNT_OBSERVE;      /*!< 0x000002C0 Soft Error Counter Observability */
  __IOM uint32_t LNK_LTSSM_STATE_CHG_INTR_CONF; /*!< 0x000002C4 LTSSM State Change Interrupt Configuration */
  __IOM uint32_t LNK_DEBUG_RSVD;                /*!< 0x000002C8 Debugging and Reserved (Internal Use Only) */
  __IOM uint32_t LNK_TX_TS1_HOLDOFF_TIMEOUT;    /*!< 0x000002CC Transmitting TS1 Holdoff */
  __IOM uint32_t LNK_MISC_CONF;                 /*!< 0x000002D0 Miscellaneous Configuration */
   __IM uint32_t LNK_MISC_OBSERVE;              /*!< 0x000002D4 Miscellaneous Observability */
   __IM uint32_t LNK_LTSSM_SUBSTATE_OBSERVE;    /*!< 0x000002D8 LTSSM Substate Observability */
   __IM uint32_t RESERVED5[9];
   __IM uint32_t LNK_RX_TYPE1_HEADER_BUFFER[21]; /*!< 0x00000300 Receive Type1 Packet Header Buffer */
   __IM uint32_t RESERVED6[11];
   __IM uint32_t LNK_RX_TYPE1_HEADER_BUFFER_STATE_0; /*!< 0x00000380 Receive Type1 Packet Header Buffer Status 0 */
   __IM uint32_t LNK_RX_TYPE1_HEADER_BUFFER_STATE_1; /*!< 0x00000384 Receive Type1 Packet Header Buffer Status 1 */
   __IM uint32_t RESERVED7[30];
   __IM uint32_t LNK_RX_TYPE2_HEADER_BUFFER[21]; /*!< 0x00000400 Receive Type2 Packet Header Buffer */
   __IM uint32_t RESERVED8[11];
   __IM uint32_t LNK_RX_TYPE2_HEADER_BUFFER_STATE_0; /*!< 0x00000480 Receive Type2 Packet Header Buffer Status 0 */
   __IM uint32_t LNK_RX_TYPE2_HEADER_BUFFER_STATE_1; /*!< 0x00000484 Receive Type2 Packet Header Buffer Status 1 */
   __IM uint32_t RESERVED9[30];
   __IM uint32_t LNK_TX_TYPE1_HEADER_BUFFER[21]; /*!< 0x00000500 Transmit Type1 Packet Header Buffer */
   __IM uint32_t RESERVED10[11];
   __IM uint32_t LNK_TX_TYPE1_HEADER_BUFFER_STATE_0; /*!< 0x00000580 Transmit Type1 Packet Header Buffer Status 0 */
   __IM uint32_t LNK_TX_TYPE1_HEADER_BUFFER_STATE_1; /*!< 0x00000584 Transmit Type1 Packet Header Buffer Status 1 */
   __IM uint32_t RESERVED11[30];
   __IM uint32_t LNK_TX_TYPE2_HEADER_BUFFER[21]; /*!< 0x00000600 Transmit Type2 Packet Header Buffer */
   __IM uint32_t RESERVED12[11];
   __IM uint32_t LNK_TX_TYPE2_HEADER_BUFFER_STATE_0; /*!< 0x00000680 Transmit Type2 Packet Header Buffer Status 0 */
   __IM uint32_t LNK_TX_TYPE2_HEADER_BUFFER_STATE_1; /*!< 0x00000684 Transmit Type2 Packet Header Buffer Status 1 */
   __IM uint32_t RESERVED13[606];
} USB32DEV_LNK_V1_Type;       /*!< Size = 4096 (0x1000) */

/**
  * \brief USB32 SuperSpeedPlus Device Controller Protocol Layer Registers (USB32DEV_PROT)
  */
typedef struct {
  __IOM uint32_t PROT_CS;                       /*!< 0x00000000 Protocol Control and Status */
  __IOM uint32_t PROT_INTR;                     /*!< 0x00000004 Protocol Interrupts */
  __IOM uint32_t PROT_INTR_SET;                 /*!< 0x00000008 Protocol Interrupts Set */
  __IOM uint32_t PROT_INTR_MASK;                /*!< 0x0000000C Protocol Interrupts Mask */
   __IM uint32_t PROT_INTR_MASKED;              /*!< 0x00000010 Protocol Interrupts Masked */
   __IM uint32_t PROT_EP_INTR;                  /*!< 0x00000014 Endpoint Interrupts */
  __IOM uint32_t PROT_EP_INTR_SET;              /*!< 0x00000018 Endpoint Interrupts set */
  __IOM uint32_t PROT_EP_INTR_MASK;             /*!< 0x0000001C Endpoint Interrupts Mask */
   __IM uint32_t PROT_EP_INTR_MASKED;           /*!< 0x00000020 Endpoint Interrupts Masked */
  __IOM uint32_t PROT_DEVICE_NOTIF_FUNC_WAKE;   /*!< 0x00000024 Device Notification Remote Wake up TP */
  __IOM uint32_t PROT_DEVICE_NOTIF_LTM;         /*!< 0x00000028 Device Notification Latency Tolerance Message TP */
  __IOM uint32_t PROT_DEVICE_NOTIF_BIAM;        /*!< 0x0000002C Device Notification Bus Interval Adjustment TP */
  __IOM uint32_t PROT_LMP_PORT_CAPABILITY_TIMER; /*!< 0x00000030 Port Capabilites LMP Timeout Configuration */
  __IOM uint32_t PROT_LMP_PORT_CONFIGURATION_TIMER; /*!< 0x00000034 Port Configuration LMP Timeout Configuration */
  __IOM uint32_t PROT_PING_TIMEOUT;             /*!< 0x00000038 Ping Timeout Configuration */
   __IM uint32_t RESERVED[2];
   __IM uint32_t PROT_FRAMECNT;                 /*!< 0x00000044 Frame Counter Register */
   __IM uint32_t PROT_ITP_CORRECTION;           /*!< 0x00000048 Correction field of ITP. */
   __IM uint32_t RESERVED1[2];
   __IM uint32_t PROT_SETUPDAT0;                /*!< 0x00000054 Received SETUP Packet Data */
   __IM uint32_t PROT_SETUPDAT1;                /*!< 0x00000058 Received SETUP Packet Data */
  __IOM uint32_t PROT_SEQ_NUM;                  /*!< 0x0000005C Sequence Number */
   __IM uint32_t RESERVED2[8];
   __IM uint32_t PROT_LMP_RECEIVED;             /*!< 0x00000080 Link Management Packet Received Value */
  __IOM uint32_t PROT_LMP_OVERRIDE;             /*!< 0x00000084 Link Management Packet Override Values */
   __IM uint32_t PROT_LMP_PORT_CAPABILITIES_RX; /*!< 0x00000088 Port Capabilities LMP Received */
  __IOM uint32_t PROT_LMP_PORT_CAPABILITIES_TX; /*!< 0x0000008C Port Capabilities LMP Transmitted */
   __IM uint32_t PROT_LMP_PORT_CONFIGURATION_RX; /*!< 0x00000090 Port Configuration LMP Received */
  __IOM uint32_t PROT_LMP_PORT_CONFIGURATION_TX; /*!< 0x00000094 Port Configuration Response LMP */
  __IOM uint32_t PROT_STREAM_ERROR_DISABLE;     /*!< 0x00000098 Streams Error Disable Type Registers */
  __IOM uint32_t PROT_STREAM_ERROR_STATUS;      /*!< 0x0000009C Streams Error STATUS Registers */
   __IM uint32_t RESERVED3[24];
   __IM uint32_t PROT_LMP_PACKET_RX[3];         /*!< 0x00000100 Link Management Packet Received */
   __IM uint32_t RESERVED4;
  __IOM uint32_t PROT_LMP_PACKET_TX[3];         /*!< 0x00000110 Link Management Packet to be sent */
   __IM uint32_t RESERVED5[9];
  __IOM uint32_t PROT_EPI_INTR[16];             /*!< 0x00000140 Per IN-Endpoint Interrupt */
  __IOM uint32_t PROT_EPI_INTR_SET[16];         /*!< 0x00000180 Per IN-Endpoint Interrupt set */
  __IOM uint32_t PROT_EPI_INTR_MASK[16];        /*!< 0x000001C0 Per IN-Endpoint Interrupt Mask */
   __IM uint32_t PROT_EPI_INTR_MASKED[16];      /*!< 0x00000200 Per IN-Endpoint Interrupt Masked */
  __IOM uint32_t PROT_EPI_CS1[16];              /*!< 0x00000240 SuperSpeed IN Endpoint Control and Status */
  __IOM uint32_t PROT_EPI_CS2[16];              /*!< 0x00000280 SuperSpeed IN Endpoint Control and Status */
  __IOM uint32_t PROT_EPI_UNMAPPED_STREAM[16];  /*!< 0x000002C0 Unmapped Stream Request */
  __IOM uint32_t PROT_EPI_MAPPED_STREAM[16];    /*!< 0x00000300 Mapped Streams Registers */
  __IOM uint32_t PROT_EPO_INTR[16];             /*!< 0x00000340 Per OUT-Endpoint Interrupt */
  __IOM uint32_t PROT_EPO_INTR_SET[16];         /*!< 0x00000380 Per OUT-Endpoint Interrupt set */
  __IOM uint32_t PROT_EPO_INTR_MASK[16];        /*!< 0x000003C0 Per OUT-Endpoint Interrupt Mask */
   __IM uint32_t PROT_EPO_INTR_MASKED[16];      /*!< 0x00000400 Per OUT-Endpoint Interrupt Masked */
  __IOM uint32_t PROT_EPO_CS1[16];              /*!< 0x00000440 SuperSpeed OUT Endpoint Control and Status */
  __IOM uint32_t PROT_EPO_CS2[16];              /*!< 0x00000480 SuperSpeed IN Endpoint Control and Status */
  __IOM uint32_t PROT_EPO_UNMAPPED_STREAM[16];  /*!< 0x000004C0 Unmapped Stream Request */
  __IOM uint32_t PROT_EPO_MAPPED_STREAM[16];    /*!< 0x00000500 Mapped Streams Registers */
   __IM uint32_t RESERVED6[16];
  __IOM uint32_t PROT_SUBLINK_DEV_NNOTIFICATION; /*!< 0x00000580 Sublink Speed Device Notification Settings */
  __IOM uint32_t PROT_SUBLINK_LSM;              /*!< 0x00000584 Sublink Speed Device Notification Lane Speed Mantissa */
  __IOM uint32_t PROT_PTM_CONFIG;               /*!< 0x00000588 PTM configuration register */
  __IOM uint32_t PROT_FW_PTM_COUNTER;           /*!< 0x0000058C FW Load PTM counter register */
  __IOM uint32_t PROT_HW_PTM_COUNTER;           /*!< 0x00000590 HW Internal PTM counter */
   __IM uint32_t PROT_PTM_STATUS;               /*!< 0x00000594 PTM status register */
   __IM uint32_t PROT_PTM_T1;                   /*!< 0x00000598 PTM T1 value */
   __IM uint32_t PROT_PTM_T4;                   /*!< 0x0000059C PTM T4 value */
   __IM uint32_t PROT_LMP_LDM_RESPONSE;         /*!< 0x000005A0 Protocol LMP LDM received */
   __IM uint32_t RESERVED7[311];
  __IOM uint32_t PROT_EPI_ENHANCE[16];          /*!< 0x00000A80 Enhancements for IN-Endpoint transactions */
  __IOM uint32_t PROT_ENHANCE;                  /*!< 0x00000AC0 Protocol Enhancements Configuration register */
   __IM uint32_t RESERVED8[335];
} USB32DEV_PROT_V1_Type;      /*!< Size = 4096 (0x1000) */

/**
  * \brief PHY TOP, PCS, Tx registers (USB32DEV_PHYSS_USB40PHY_TOP)
  */
typedef struct {
   __IM uint32_t RESERVED;
  __IOM uint32_t TOP_CTRL_0;                    /*!< 0x00000004 TOP control register */
   __IM uint32_t RESERVED1[2];
  __IOM uint32_t PCS_CTRL_0;                    /*!< 0x00000010 PCS control register */
   __IM uint32_t RESERVED2;
   __IM uint32_t PCS_STATUS;                    /*!< 0x00000018 PCS status register */
  __IOM uint32_t PCS_SPARE;                     /*!< 0x0000001C PCS spare */
   __IM uint32_t ERR_STATUS;                    /*!< 0x00000020 Error status register */
   __IM uint32_t RESERVED3[3];
  __IOM uint32_t PIPE_OVERRIDE_0;               /*!< 0x00000030 PIPE interface control signals override register #0 */
   __IM uint32_t RESERVED4;
   __IM uint32_t PIPE_STATUS;                   /*!< 0x00000038 PIPE interface status read register */
   __IM uint32_t RESERVED5;
  __IOM uint32_t INTR0;                         /*!< 0x00000040 INTR0 Cause. These are the wakeup interrupts get reflected on
                                                                interrupt_wakeup pin. */
  __IOM uint32_t INTR0_SET;                     /*!< 0x00000044 INTR0 Set */
  __IOM uint32_t INTR0_MASK;                    /*!< 0x00000048 INTR0 Mask */
   __IM uint32_t INTR0_MASKED;                  /*!< 0x0000004C INTR0 Masked */
  __IOM uint32_t INTR1;                         /*!< 0x00000050 INTR1 Cause. These are the wakeup interrupts get reflected on
                                                                interrupt_wakeup pin. */
  __IOM uint32_t INTR1_SET;                     /*!< 0x00000054 INTR1 Set */
  __IOM uint32_t INTR1_MASK;                    /*!< 0x00000058 INTR1 Mask */
   __IM uint32_t INTR1_MASKED;                  /*!< 0x0000005C INTR1 Masked */
  __IOM uint32_t TX_AFE_CTRL_0;                 /*!< 0x00000060 Tx AFE control register #0 */
  __IOM uint32_t TX_AFE_CTRL_1;                 /*!< 0x00000064 Tx AFE control register #1 */
   __IM uint32_t RESERVED6;
  __IOM uint32_t TX_AFE_RXDETECT_CNT;           /*!< 0x0000006C Tx AFE TxDetectRx wait counter */
  __IOM uint32_t TX_AFE_OVERRIDE_0;             /*!< 0x00000070 Tx AFE override 0 register */
   __IM uint32_t RESERVED7;
   __IM uint32_t TX_AFE_DEBUG;                  /*!< 0x00000078 Tx AFE debug register */
   __IM uint32_t TX_AFE_STATUS;                 /*!< 0x0000007C Tx AFE status register */
   __IM uint32_t RESERVED8;
  __IOM uint32_t TX_AFE_ZTRIM;                  /*!< 0x00000084 Tx AFE impedance calibration register */
   __IM uint32_t RESERVED9[2];
  __IOM uint32_t TX_AFE_CFG;                    /*!< 0x00000090 Tx AFE configuration control register */
  __IOM uint32_t TX_AFE_CFG_SFT_W_0;            /*!< 0x00000094 Tx AFE configuration control shift write register #0 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_1;            /*!< 0x00000098 Tx AFE configuration control shift write register #1 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_2;            /*!< 0x0000009C Tx AFE configuration control shift write register #2 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_3;            /*!< 0x000000A0 Tx AFE configuration control shift write register #3 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_4;            /*!< 0x000000A4 Tx AFE configuration control shift write register #4 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_5;            /*!< 0x000000A8 Tx AFE configuration control shift write register #5 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_6;            /*!< 0x000000AC Tx AFE configuration control shift write register #6 */
  __IOM uint32_t TX_AFE_CFG_SFT_W_7;            /*!< 0x000000B0 Tx AFE configuration control shift write register #7 */
   __IM uint32_t TX_AFE_CFG_SFT_R;              /*!< 0x000000B4 Tx AFE configuration control shift read register for debug */
   __IM uint32_t RESERVED10[2];
  __IOM uint32_t ADC;                           /*!< 0x000000C0 Analog to Digital Converter register #0 */
   __IM uint32_t ADC_STATUS;                    /*!< 0x000000C4 Analog to Digital Converter status register */
   __IM uint32_t RESERVED11[3];
  __IOM uint32_t PHYSS_LOOPBACK_CTRL_0;         /*!< 0x000000D4 PHY Loopback control register #0 */
   __IM uint32_t PHYSS_LOOPBACK_DEBUG_0;        /*!< 0x000000D8 PHY Loopback debug register #0 */
   __IM uint32_t PHYSS_LOOPBACK_DEBUG_1;        /*!< 0x000000DC PHY Loopback debug register #1 */
   __IM uint32_t PHYSS_LOOPBACK_STATUS_1;       /*!< 0x000000E0 PHY Loopback status register #1 */
  __IOM uint32_t PHYSS_LOOPBACK_USER_0;         /*!< 0x000000E4 PHY Loopback user pattern register #0 */
  __IOM uint32_t PHYSS_LOOPBACK_USER_1;         /*!< 0x000000E8 PHY Loopback user pattern register #1 */
   __IM uint32_t RESERVED12;
  __IOM uint32_t PHYSS_DDFT_MUX_SEL;            /*!< 0x000000F0 PHY DDFT selection */
  __IOM uint32_t PIPE_RX_CTRL;                  /*!< 0x000000F4 Configuration for PIPE RX */
  __IOM uint32_t LOW_POWER_CTRL;                /*!< 0x000000F8 Low Power configuration */
  __IOM uint32_t CDR_SW_CTRL;                   /*!< 0x000000FC Configuration for CDR mode switching */
   __IM uint32_t RESERVED13[448];
} USB32DEV_PHYSS_USB40PHY_TOP_V1_Type; /*!< Size = 2048 (0x800) */

/**
  * \brief PHY Rx registers (USB32DEV_PHYSS_USB40PHY_RX)
  */
typedef struct {
  __IOM uint32_t RX_SDM_CFG0;                   /*!< 0x00000000 Sigma-Delta Modulator (SDM) Configuration */
  __IOM uint32_t RX_SDM_CFG1;                   /*!< 0x00000004 Sigma-Delta Modulator (SDM) Configuration */
  __IOM uint32_t RX_SDM_CFG2;                   /*!< 0x00000008 Sigma-Delta Modulator (SDM) Configuration */
  __IOM uint32_t RX_SDM_CFG3;                   /*!< 0x0000000C Sigma-Delta Modulator (SDM) Configuration */
  __IOM uint32_t RX_DFE_CFG0;                   /*!< 0x00000010 DFE FSM (DFE) Configuration */
  __IOM uint32_t RX_DFE_CFG1;                   /*!< 0x00000014 DFE FSM (DFE) Configuration */
  __IOM uint32_t RX_DFE_CFG2;                   /*!< 0x00000018 DFE FSM (DFE) Configuration */
  __IOM uint32_t RX_DFE_CFG3;                   /*!< 0x0000001C DFE FSM (DFE) Configuration */
   __IM uint32_t RX_DFE_STAT0;                  /*!< 0x00000020 DFE FSM (DFE) status */
   __IM uint32_t RX_DFE_STAT1;                  /*!< 0x00000024 DFE FSM (DFE) status */
   __IM uint32_t RX_DFE_STAT2;                  /*!< 0x00000028 DFE FSM (DFE) status */
  __IOM uint32_t RX_OSA_CFG0;                   /*!< 0x0000002C Offset Calibration FSM (OSA) Configuration */
  __IOM uint32_t RX_OSA_CFG1;                   /*!< 0x00000030 Offset Calibration FSM (OSA) Configuration */
  __IOM uint32_t RX_OSA_CFG2;                   /*!< 0x00000034 Offset Calibration FSM (OSA) Configuration */
  __IOM uint32_t RX_OSA_CFG3;                   /*!< 0x00000038 Offset Calibration FSM (OSA) Configuration */
  __IOM uint32_t RX_OSA_CFG4;                   /*!< 0x0000003C Offset Calibration FSM (OSA) Configuration */
  __IOM uint32_t RX_OSA_CFG5;                   /*!< 0x00000040 Offset Calibration FSM (OSA) Configuration */
   __IM uint32_t RX_OSA_STAT0;                  /*!< 0x00000044 Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT1;                  /*!< 0x00000048 Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT2;                  /*!< 0x0000004C Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT3;                  /*!< 0x00000050 Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT4;                  /*!< 0x00000054 Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT5;                  /*!< 0x00000058 Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT6;                  /*!< 0x0000005C Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT7;                  /*!< 0x00000060 Offset Calibration FSM (OSA) status */
   __IM uint32_t RX_OSA_STAT8;                  /*!< 0x00000064 Offset Calibration FSM (OSA) status */
  __IOM uint32_t RX_CTLE_CFG0;                  /*!< 0x00000068 CTLE controls Configuration */
  __IOM uint32_t RX_CTLE_CFG1;                  /*!< 0x0000006C CTLE controls Configuration */
   __IM uint32_t RX_CTLE_STAT0;                 /*!< 0x00000070 CTLE controls status */
   __IM uint32_t RX_CTLE_STAT1;                 /*!< 0x00000074 CTLE controls status */
  __IOM uint32_t RX_EM_CFG0;                    /*!< 0x00000078 Eye monitor FSM (EM) Configuration */
  __IOM uint32_t RX_EM_CFG1;                    /*!< 0x0000007C Eye monitor FSM (EM) Configuration */
   __IM uint32_t RX_EM_STAT0;                   /*!< 0x00000080 Eye monitor FSM (EM) status */
   __IM uint32_t RX_EM_STAT1;                   /*!< 0x00000084 Eye monitor FSM (EM) status */
  __IOM uint32_t RX_DFEA_CFG0;                  /*!< 0x00000088 DFE Analog Configuration */
  __IOM uint32_t RX_OSAA_CFG0;                  /*!< 0x0000008C Offset Calibration Analog Configuration */
  __IOM uint32_t RX_OSAA_CFG1;                  /*!< 0x00000090 Offset Calibration Analog Configuration */
  __IOM uint32_t RX_AFE_CFG;                    /*!< 0x00000094 Analog Front End (AFE) Configuration */
  __IOM uint32_t RX_EMA_CFG;                    /*!< 0x00000098 Eye Monitor Analog Configuration */
  __IOM uint32_t RX_REFCKSEL_CFG;               /*!< 0x0000009C Reference Clock Select (REFCLKSEL) Configuration */
  __IOM uint32_t RX_DIVH_CFG;                   /*!< 0x000000A0 HS Divider (DIVH) Configuration */
  __IOM uint32_t RX_PFD_CFG;                    /*!< 0x000000A4 Phase/Frequency Detector (PFD) Configuration */
  __IOM uint32_t RX_CP_CFG;                     /*!< 0x000000A8 Charge Pump (CP) Configuration */
  __IOM uint32_t RX_LF_CFG;                     /*!< 0x000000AC Loop Filter (LF) Configuration */
  __IOM uint32_t RX_BIASGEN_CFG0;               /*!< 0x000000B0 Bias Generator (BIASGEN) Configuration */
  __IOM uint32_t RX_BIASGEN_CFG1;               /*!< 0x000000B4 Bias Generator (BIASGEN) Configuration */
  __IOM uint32_t RX_GNRL_CFG;                   /*!< 0x000000B8 General Controls Configuration */
  __IOM uint32_t RX_VREG_CFG0;                  /*!< 0x000000BC Regulators Configuration */
  __IOM uint32_t RX_VREG_CFG1;                  /*!< 0x000000C0 Regulators Configuration */
   __IM uint32_t RX_VREG_STAT;                  /*!< 0x000000C4 Regulators Status */
  __IOM uint32_t RX_SD_CFG;                     /*!< 0x000000C8 Signal detect/LFPS Configuration */
   __IM uint32_t RX_SD_STAT;                    /*!< 0x000000CC Signal detect/LFPS Status */
  __IOM uint32_t RX_LD_CFG;                     /*!< 0x000000D0 Lock Detect Configuration */
   __IM uint32_t RX_LD_STAT;                    /*!< 0x000000D4 Lock Detect Status */
  __IOM uint32_t RX_DFE_CFG4;                   /*!< 0x000000D8 DFE FSM (DFE) Configuration */
  __IOM uint32_t RX_HDWR_ENABLE;                /*!< 0x000000DC Configuration for hardware enable */
  __IOM uint32_t RX_ATEST_CFG;                  /*!< 0x000000E0 Analog Test Mux (ATEST) Configuration */
  __IOM uint32_t RX_DTEST_CFG;                  /*!< 0x000000E4 Digital Test Mux (DTEST) Configuration */
  __IOM uint32_t SPARE_CFG;                     /*!< 0x000000E8 Spare Configuration */
   __IM uint32_t SPARE_STAT;                    /*!< 0x000000EC Spare Status */
  __IOM uint32_t SPARE_HV_CFG;                  /*!< 0x000000F0 Spare Configuration for HV signlas */
   __IM uint32_t RESERVED[3];
} USB32DEV_PHYSS_USB40PHY_RX_V1_Type; /*!< Size = 256 (0x100) */

/**
  * \brief PHY PLL SYS registers (USB32DEV_PHYSS_USB40PHY_PLL_SYS)
  */
typedef struct {
  __IOM uint32_t PLL_SDM_CFG;                   /*!< 0x00000000 Sigma-Delta Modulator (SDM) Configuration */
  __IOM uint32_t PLL_SSM_CFG0;                  /*!< 0x00000004 Spread Spectrun Modulator (SSM) Configuration */
  __IOM uint32_t PLL_SSM_CFG1;                  /*!< 0x00000008 Spread Spectrun Modulator (SSM) Configuration */
  __IOM uint32_t PLL_SSM_CFG2;                  /*!< 0x0000000C Spread Spectrun Modulator (SSM) Configuration */
  __IOM uint32_t PLL_AFC_CFG0;                  /*!< 0x00000010 Automatic Frequency Control (AFC) Configuration */
  __IOM uint32_t PLL_AFC_CFG1;                  /*!< 0x00000014 Automatic Frequency Control (AFC) Configuration */
  __IOM uint32_t PLL_AFC_CFG2;                  /*!< 0x00000018 Automatic Frequency Control (AFC) Configuration */
   __IM uint32_t PLL_AFC_STAT;                  /*!< 0x0000001C Automatic Frequency Control (AFC) Status */
  __IOM uint32_t PLL_AAC_CFG0;                  /*!< 0x00000020 Automatic Amplitude Control (AAC) Configuration */
  __IOM uint32_t PLL_AAC_CFG1;                  /*!< 0x00000024 Automatic Amplitude Control (AAC) Configuration */
   __IM uint32_t PLL_AAC_STAT;                  /*!< 0x00000028 Automatic Amplitude Control (AAC) Status */
  __IOM uint32_t PLL_REFCKSEL_CFG;              /*!< 0x0000002C Reference Clock Select (REFCLKSEL) Configuration */
  __IOM uint32_t PLL_DIVR_CFG;                  /*!< 0x00000030 Reference Clock Divider (DIVR) Configuration */
  __IOM uint32_t PLL_DIVP_CFG;                  /*!< 0x00000034 Post Divider (DIVP) Configuration */
  __IOM uint32_t PLL_DIVH_CFG;                  /*!< 0x00000038 HS Divider (DIVH) Configuration */
  __IOM uint32_t PLL_PFD_CFG;                   /*!< 0x0000003C Phase/Frequency Detector (PFD) Configuration */
  __IOM uint32_t PLL_CP_CFG;                    /*!< 0x00000040 Charge Pump (CP) Configuration */
  __IOM uint32_t PLL_LF_CFG;                    /*!< 0x00000044 Loop Filter (LF) Configuration */
  __IOM uint32_t PLL_VCO_CFG;                   /*!< 0x00000048 Voltage-Controlled Oscillator (VCO) Configuration */
  __IOM uint32_t PLL_BIASGEN_CFG;               /*!< 0x0000004C Bias Generator (BIASGEN) Configuration */
  __IOM uint32_t PLL_GNRL_CFG;                  /*!< 0x00000050 General Controls Configuration */
  __IOM uint32_t PLL_VREG_CFG1;                 /*!< 0x00000054 Regulators Configuration */
  __IOM uint32_t PLL_VREG_CFG2;                 /*!< 0x00000058 Regulators Configuration */
   __IM uint32_t PLL_VREG_STAT;                 /*!< 0x0000005C Regulators Status */
  __IOM uint32_t PLL_LD_CFG;                    /*!< 0x00000060 Lock Detect Configuration */
   __IM uint32_t PLL_LD_STAT;                   /*!< 0x00000064 Lock Detect Status */
  __IOM uint32_t PLL_ATEST;                     /*!< 0x00000068 Analog Test Mux (ATEST) Configuration */
  __IOM uint32_t PLL_DTEST;                     /*!< 0x0000006C Digital Test Mux (DTEST) Configuration */
  __IOM uint32_t SPARE_CFG;                     /*!< 0x00000070 Spare Configuration */
   __IM uint32_t SPARE_STAT;                    /*!< 0x00000074 Spare Status */
   __IM uint32_t RESERVED[34];
} USB32DEV_PHYSS_USB40PHY_PLL_SYS_V1_Type; /*!< Size = 256 (0x100) */

/**
  * \brief PHY Registers (USB32DEV_PHYSS_USB40PHY)
  */
typedef struct {
        USB32DEV_PHYSS_USB40PHY_TOP_V1_Type USB40PHY_TOP; /*!< 0x00000000 PHY TOP, PCS, Tx registers */
        USB32DEV_PHYSS_USB40PHY_RX_V1_Type USB40PHY_RX; /*!< 0x00000800 PHY Rx registers */
        USB32DEV_PHYSS_USB40PHY_PLL_SYS_V1_Type USB40PHY_PLL_SYS; /*!< 0x00000900 PHY PLL SYS registers */
   __IM uint32_t RESERVED[384];
} USB32DEV_PHYSS_USB40PHY_V1_Type; /*!< Size = 4096 (0x1000) */

/**
  * \brief USB32 SuperSpeedPlus Physical Layer Registers (USB32DEV_PHYSS)
  */
typedef struct {
        USB32DEV_PHYSS_USB40PHY_V1_Type USB40PHY[2]; /*!< 0x00000000 PHY Registers */
} USB32DEV_PHYSS_V1_Type;     /*!< Size = 8192 (0x2000) */

/**
  * \brief Socket Registers (USB32DEV_ADAPTER_DMA_SCK)
  */
typedef struct {
  __IOM uint32_t SCK_DSCR;                      /*!< 0x00000000 Descriptor Chain Pointer */
  __IOM uint32_t SCK_SIZE;                      /*!< 0x00000004 Transfer Size Register */
  __IOM uint32_t SCK_COUNT;                     /*!< 0x00000008 Transfer Count Register */
  __IOM uint32_t SCK_STATUS;                    /*!< 0x0000000C Socket Status Register */
  __IOM uint32_t SCK_INTR;                      /*!< 0x00000010 Socket Interrupt Request Register */
  __IOM uint32_t SCK_INTR_MASK;                 /*!< 0x00000014 Socket Interrupt Mask Register */
   __IM uint32_t RESERVED[2];
  __IOM uint32_t DSCR_BUFFER;                   /*!< 0x00000020 Descriptor buffer base address register */
  __IOM uint32_t DSCR_SYNC;                     /*!< 0x00000024 Descriptor synchronization pointers register */
  __IOM uint32_t DSCR_CHAIN;                    /*!< 0x00000028 Descriptor Chain Pointers Register */
  __IOM uint32_t DSCR_SIZE;                     /*!< 0x0000002C Descriptor Size Register */
   __IM uint32_t RESERVED1[19];
   __OM uint32_t EVENT;                         /*!< 0x0000007C Event Communication Register */
} USB32DEV_ADAPTER_DMA_SCK_V1_Type; /*!< Size = 128 (0x80) */

/**
  * \brief General DMA Registers (USB32DEV_ADAPTER_DMA_SCK_GBL)
  */
typedef struct {
   __IM uint32_t SCK_INTR;                      /*!< 0x00000000 Socket Interrupt Request Register */
   __IM uint32_t RESERVED[59];
  __IOM uint32_t ADAPTER_CTRL;                  /*!< 0x000000F0 Adapter Control Register */
   __IM uint32_t ADAPTER_DEBUG;                 /*!< 0x000000F4 Adapter Debug Observation Register */
  __IOM uint32_t ADAPTER_CONF;                  /*!< 0x000000F8 Adapter Configuration Register */
   __IM uint32_t ADAPTER_STATUS;                /*!< 0x000000FC Adapter Global Status Fields */
} USB32DEV_ADAPTER_DMA_SCK_GBL_V1_Type; /*!< Size = 256 (0x100) */

/**
  * \brief USB32 DMA Adapter registers. 
0x10000 is for Ingress and 0x20000 for egress Adapter (USB32DEV_ADAPTER_DMA)
  */
typedef struct {
   __IM uint32_t RESERVED[8192];
        USB32DEV_ADAPTER_DMA_SCK_V1_Type SCK[16]; /*!< 0x00008000 Socket Registers */
   __IM uint32_t RESERVED1[7616];
        USB32DEV_ADAPTER_DMA_SCK_GBL_V1_Type SCK_GBL; /*!< 0x0000FF00 General DMA Registers */
} USB32DEV_ADAPTER_DMA_V1_Type; /*!< Size = 65536 (0x10000) */

/**
  * \brief USB32DEV Registers (USB32DEV_USB32DEV)
  */
typedef struct {
        USB32DEV_MAIN_V1_Type USB32DEV_MAIN; /*!< 0x00000000 USB32 Main Register */
        USB32DEV_EPM_V1_Type USB32DEV_EPM; /*!< 0x00001000 USB32 Controller End Point Manager Registers */
        USB32DEV_LNK_V1_Type USB32DEV_LNK; /*!< 0x00002000 USB32 SuperSpeedPlus Device Controller Link Layer Registers */
        USB32DEV_PROT_V1_Type USB32DEV_PROT; /*!< 0x00003000 USB32 SuperSpeedPlus Device Controller Protocol Layer Registers */
        USB32DEV_PHYSS_V1_Type USB32DEV_PHYSS; /*!< 0x00004000 USB32 SuperSpeedPlus Physical Layer Registers */
   __IM uint32_t RESERVED[10240];
        USB32DEV_ADAPTER_DMA_V1_Type USB32DEV_ADAPTER[2]; /*!< 0x00010000 USB32 DMA Adapter registers. 0x10000 is for Ingress and 0x20000
                                                                for egress Adapter */
} USB32DEV_V1_Type;                    /*!< Size = 196608 (0x30000) */


/* USB32DEV_MAIN.CTRL */
#define USB32DEV_MAIN_CTRL_SSDEV_ENABLE_Pos 0UL
#define USB32DEV_MAIN_CTRL_SSDEV_ENABLE_Msk 0x1UL
#define USB32DEV_MAIN_CTRL_DISABLE_SRAM_Pos 2UL
#define USB32DEV_MAIN_CTRL_DISABLE_SRAM_Msk 0x4UL
#define USB32DEV_MAIN_CTRL_USB2_BUS_RESET_Pos 3UL
#define USB32DEV_MAIN_CTRL_USB2_BUS_RESET_Msk 0x8UL
#define USB32DEV_MAIN_CTRL_CLK_EN_Pos 4UL
#define USB32DEV_MAIN_CTRL_CLK_EN_Msk 0x10UL
#define USB32DEV_MAIN_CTRL_PCLK_SRC_Pos 5UL
#define USB32DEV_MAIN_CTRL_PCLK_SRC_Msk 0x60UL
#define USB32DEV_MAIN_CTRL_EN_RST_CLOCK_MUX_Pos 7UL
#define USB32DEV_MAIN_CTRL_EN_RST_CLOCK_MUX_Msk 0x80UL
#define USB32DEV_MAIN_CTRL_EN_EGRS_RQ_HALF_DATA_RATE_Pos 8UL
#define USB32DEV_MAIN_CTRL_EN_EGRS_RQ_HALF_DATA_RATE_Msk 0x100UL
#define USB32DEV_MAIN_CTRL_CONFIG_LANE_Pos 30UL
#define USB32DEV_MAIN_CTRL_CONFIG_LANE_Msk 0x40000000UL
#define USB32DEV_MAIN_CTRL_IP_ENABLED_Pos 31UL
#define USB32DEV_MAIN_CTRL_IP_ENABLED_Msk 0x80000000UL
/* USB32DEV_MAIN.INTR */
#define USB32DEV_MAIN_INTR_LINK_Pos 0UL
#define USB32DEV_MAIN_INTR_LINK_Msk 0x1UL
#define USB32DEV_MAIN_INTR_PROT_Pos 1UL
#define USB32DEV_MAIN_INTR_PROT_Msk 0x2UL
#define USB32DEV_MAIN_INTR_PROT_EP_Pos 2UL
#define USB32DEV_MAIN_INTR_PROT_EP_Msk 0x4UL
#define USB32DEV_MAIN_INTR_EPM_URUN_Pos 3UL
#define USB32DEV_MAIN_INTR_EPM_URUN_Msk 0x8UL
#define USB32DEV_MAIN_INTR_P3P0_Pos 5UL
#define USB32DEV_MAIN_INTR_P3P0_Msk 0x20UL
#define USB32DEV_MAIN_INTR_P3P1_Pos 6UL
#define USB32DEV_MAIN_INTR_P3P1_Msk 0x40UL
#define USB32DEV_MAIN_INTR_P3P2_Pos 7UL
#define USB32DEV_MAIN_INTR_P3P2_Msk 0x80UL
#define USB32DEV_MAIN_INTR_PHY0_Pos 8UL
#define USB32DEV_MAIN_INTR_PHY0_Msk 0x100UL
#define USB32DEV_MAIN_INTR_PHY1_Pos 9UL
#define USB32DEV_MAIN_INTR_PHY1_Msk 0x200UL
/* USB32DEV_MAIN.INTR_SET */
#define USB32DEV_MAIN_INTR_SET_LINK_Pos 0UL
#define USB32DEV_MAIN_INTR_SET_LINK_Msk 0x1UL
#define USB32DEV_MAIN_INTR_SET_PROT_Pos 1UL
#define USB32DEV_MAIN_INTR_SET_PROT_Msk 0x2UL
#define USB32DEV_MAIN_INTR_SET_PROT_EP_Pos 2UL
#define USB32DEV_MAIN_INTR_SET_PROT_EP_Msk 0x4UL
#define USB32DEV_MAIN_INTR_SET_EPM_URUN_Pos 3UL
#define USB32DEV_MAIN_INTR_SET_EPM_URUN_Msk 0x8UL
#define USB32DEV_MAIN_INTR_SET_P3P0_Pos 5UL
#define USB32DEV_MAIN_INTR_SET_P3P0_Msk 0x20UL
#define USB32DEV_MAIN_INTR_SET_P3P1_Pos 6UL
#define USB32DEV_MAIN_INTR_SET_P3P1_Msk 0x40UL
#define USB32DEV_MAIN_INTR_SET_P3P2_Pos 7UL
#define USB32DEV_MAIN_INTR_SET_P3P2_Msk 0x80UL
#define USB32DEV_MAIN_INTR_SET_PHY0_Pos 8UL
#define USB32DEV_MAIN_INTR_SET_PHY0_Msk 0x100UL
#define USB32DEV_MAIN_INTR_SET_PHY1_Pos 9UL
#define USB32DEV_MAIN_INTR_SET_PHY1_Msk 0x200UL
/* USB32DEV_MAIN.INTR_MASK */
#define USB32DEV_MAIN_INTR_MASK_LINK_MASK_Pos 0UL
#define USB32DEV_MAIN_INTR_MASK_LINK_MASK_Msk 0x1UL
#define USB32DEV_MAIN_INTR_MASK_PROT_MASK_Pos 1UL
#define USB32DEV_MAIN_INTR_MASK_PROT_MASK_Msk 0x2UL
#define USB32DEV_MAIN_INTR_MASK_PROT_EP_MASK_Pos 2UL
#define USB32DEV_MAIN_INTR_MASK_PROT_EP_MASK_Msk 0x4UL
#define USB32DEV_MAIN_INTR_MASK_EPM_URUN_MASK_Pos 3UL
#define USB32DEV_MAIN_INTR_MASK_EPM_URUN_MASK_Msk 0x8UL
#define USB32DEV_MAIN_INTR_MASK_P3P0_MASK_Pos 5UL
#define USB32DEV_MAIN_INTR_MASK_P3P0_MASK_Msk 0x20UL
#define USB32DEV_MAIN_INTR_MASK_P3P1_MASK_Pos 6UL
#define USB32DEV_MAIN_INTR_MASK_P3P1_MASK_Msk 0x40UL
#define USB32DEV_MAIN_INTR_MASK_P3P2_MASK_Pos 7UL
#define USB32DEV_MAIN_INTR_MASK_P3P2_MASK_Msk 0x80UL
#define USB32DEV_MAIN_INTR_MASK_PHY0_MASK_Pos 8UL
#define USB32DEV_MAIN_INTR_MASK_PHY0_MASK_Msk 0x100UL
#define USB32DEV_MAIN_INTR_MASK_PHY1_MASK_Pos 9UL
#define USB32DEV_MAIN_INTR_MASK_PHY1_MASK_Msk 0x200UL
/* USB32DEV_MAIN.INTR_MASKED */
#define USB32DEV_MAIN_INTR_MASKED_LINK_MASKED_Pos 0UL
#define USB32DEV_MAIN_INTR_MASKED_LINK_MASKED_Msk 0x1UL
#define USB32DEV_MAIN_INTR_MASKED_PROT_MASKED_Pos 1UL
#define USB32DEV_MAIN_INTR_MASKED_PROT_MASKED_Msk 0x2UL
#define USB32DEV_MAIN_INTR_MASKED_PROT_EP_MASKED_Pos 2UL
#define USB32DEV_MAIN_INTR_MASKED_PROT_EP_MASKED_Msk 0x4UL
#define USB32DEV_MAIN_INTR_MASKED_EPM_URUN_MASKED_Pos 3UL
#define USB32DEV_MAIN_INTR_MASKED_EPM_URUN_MASKED_Msk 0x8UL
#define USB32DEV_MAIN_INTR_MASKED_P3P0_MASKED_Pos 5UL
#define USB32DEV_MAIN_INTR_MASKED_P3P0_MASKED_Msk 0x20UL
#define USB32DEV_MAIN_INTR_MASKED_P3P1_MASKED_Pos 6UL
#define USB32DEV_MAIN_INTR_MASKED_P3P1_MASKED_Msk 0x40UL
#define USB32DEV_MAIN_INTR_MASKED_P3P2_MASKED_Pos 7UL
#define USB32DEV_MAIN_INTR_MASKED_P3P2_MASKED_Msk 0x80UL
#define USB32DEV_MAIN_INTR_MASKED_PHY0_MASKED_Pos 8UL
#define USB32DEV_MAIN_INTR_MASKED_PHY0_MASKED_Msk 0x100UL
#define USB32DEV_MAIN_INTR_MASKED_PHY1_MASKED_Pos 9UL
#define USB32DEV_MAIN_INTR_MASKED_PHY1_MASKED_Msk 0x200UL
/* USB32DEV_MAIN.DDFT_MUX */
#define USB32DEV_MAIN_DDFT_MUX_DDFT0_SEL_Pos 0UL
#define USB32DEV_MAIN_DDFT_MUX_DDFT0_SEL_Msk 0xFFUL
#define USB32DEV_MAIN_DDFT_MUX_DDFT1_SEL_Pos 8UL
#define USB32DEV_MAIN_DDFT_MUX_DDFT1_SEL_Msk 0xFF00UL
/* USB32DEV_MAIN.GPIO_DDFT_MUX */
#define USB32DEV_MAIN_GPIO_DDFT_MUX_DDFT0_SEL_Pos 0UL
#define USB32DEV_MAIN_GPIO_DDFT_MUX_DDFT0_SEL_Msk 0xFFUL
#define USB32DEV_MAIN_GPIO_DDFT_MUX_DDFT1_SEL_Pos 8UL
#define USB32DEV_MAIN_GPIO_DDFT_MUX_DDFT1_SEL_Msk 0xFF00UL
/* USB32DEV_MAIN.DEBUG_CONFIG */
#define USB32DEV_MAIN_DEBUG_CONFIG_DEBUG_EN_Pos 0UL
#define USB32DEV_MAIN_DEBUG_CONFIG_DEBUG_EN_Msk 0x1UL
#define USB32DEV_MAIN_DEBUG_CONFIG_STOP_EN_Pos 1UL
#define USB32DEV_MAIN_DEBUG_CONFIG_STOP_EN_Msk 0x2UL
#define USB32DEV_MAIN_DEBUG_CONFIG_FUNC_SEL_Pos 2UL
#define USB32DEV_MAIN_DEBUG_CONFIG_FUNC_SEL_Msk 0x1CUL
#define USB32DEV_MAIN_DEBUG_CONFIG_START_EVENT_SEL_Pos 5UL
#define USB32DEV_MAIN_DEBUG_CONFIG_START_EVENT_SEL_Msk 0x7E0UL
#define USB32DEV_MAIN_DEBUG_CONFIG_END_EVENT_SEL_Pos 11UL
#define USB32DEV_MAIN_DEBUG_CONFIG_END_EVENT_SEL_Msk 0x1F800UL
#define USB32DEV_MAIN_DEBUG_CONFIG_NUM_OF_ACTIVITIES_Pos 17UL
#define USB32DEV_MAIN_DEBUG_CONFIG_NUM_OF_ACTIVITIES_Msk 0x1FE0000UL
#define USB32DEV_MAIN_DEBUG_CONFIG_SKT_NUM_Pos 25UL
#define USB32DEV_MAIN_DEBUG_CONFIG_SKT_NUM_Msk 0x1E000000UL
#define USB32DEV_MAIN_DEBUG_CONFIG_DEBUG_SKT_NUM_Pos 29UL
#define USB32DEV_MAIN_DEBUG_CONFIG_DEBUG_SKT_NUM_Msk 0xE0000000UL
/* USB32DEV_MAIN.DEBUG_MUX_SEL */
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_31_0_SEL_Pos 0UL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_31_0_SEL_Msk 0xFUL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_63_32_SEL_Pos 4UL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_63_32_SEL_Msk 0xF0UL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_95_64_SEL_Pos 8UL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_95_64_SEL_Msk 0xF00UL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_127_96_SEL_Pos 12UL
#define USB32DEV_MAIN_DEBUG_MUX_SEL_BIT_127_96_SEL_Msk 0xF000UL
/* USB32DEV_MAIN.LOOPBACK */
#define USB32DEV_MAIN_LOOPBACK_EP_NUM_Pos 0UL
#define USB32DEV_MAIN_LOOPBACK_EP_NUM_Msk 0xFUL
#define USB32DEV_MAIN_LOOPBACK_NUMP_Pos 4UL
#define USB32DEV_MAIN_LOOPBACK_NUMP_Msk 0x1F0UL
#define USB32DEV_MAIN_LOOPBACK_TT_FIELD_Pos 9UL
#define USB32DEV_MAIN_LOOPBACK_TT_FIELD_Msk 0xE00UL
#define USB32DEV_MAIN_LOOPBACK_SEQ_NUM_Pos 12UL
#define USB32DEV_MAIN_LOOPBACK_SEQ_NUM_Msk 0x1F000UL
#define USB32DEV_MAIN_LOOPBACK_SINGLE_XFER_Pos 29UL
#define USB32DEV_MAIN_LOOPBACK_SINGLE_XFER_Msk 0x20000000UL
#define USB32DEV_MAIN_LOOPBACK_BURN_IN_EN_Pos 30UL
#define USB32DEV_MAIN_LOOPBACK_BURN_IN_EN_Msk 0x40000000UL
#define USB32DEV_MAIN_LOOPBACK_PIPE_LOOPBACK_EN_Pos 31UL
#define USB32DEV_MAIN_LOOPBACK_PIPE_LOOPBACK_EN_Msk 0x80000000UL
/* USB32DEV_MAIN.BUG_FIX */
#define USB32DEV_MAIN_BUG_FIX_EPM_T1_T2_COLLISION_EN_Pos 0UL
#define USB32DEV_MAIN_BUG_FIX_EPM_T1_T2_COLLISION_EN_Msk 0x1UL
#define USB32DEV_MAIN_BUG_FIX_EPM_READ_END_RST_EN_Pos 1UL
#define USB32DEV_MAIN_BUG_FIX_EPM_READ_END_RST_EN_Msk 0x2UL
#define USB32DEV_MAIN_BUG_FIX_PROT_RTY_COLLISION_EN_Pos 8UL
#define USB32DEV_MAIN_BUG_FIX_PROT_RTY_COLLISION_EN_Msk 0x100UL
#define USB32DEV_MAIN_BUG_FIX_PROT_B2B_EN_Pos 9UL
#define USB32DEV_MAIN_BUG_FIX_PROT_B2B_EN_Msk 0x200UL
#define USB32DEV_MAIN_BUG_FIX_PROT_ISO_BRST_0_EN_Pos 10UL
#define USB32DEV_MAIN_BUG_FIX_PROT_ISO_BRST_0_EN_Msk 0x400UL
#define USB32DEV_MAIN_BUG_FIX_PROT_RX_COLLISION_EN_Pos 11UL
#define USB32DEV_MAIN_BUG_FIX_PROT_RX_COLLISION_EN_Msk 0x800UL
#define USB32DEV_MAIN_BUG_FIX_PROT_RX_SKP_NO_WRITE_EN_Pos 12UL
#define USB32DEV_MAIN_BUG_FIX_PROT_RX_SKP_NO_WRITE_EN_Msk 0x1000UL
#define USB32DEV_MAIN_BUG_FIX_RESET_PROT_STATE_Pos 13UL
#define USB32DEV_MAIN_BUG_FIX_RESET_PROT_STATE_Msk 0x2000UL
#define USB32DEV_MAIN_BUG_FIX_RESET_SKT_HAS_EOB_FUNC_Pos 14UL
#define USB32DEV_MAIN_BUG_FIX_RESET_SKT_HAS_EOB_FUNC_Msk 0x4000UL
#define USB32DEV_MAIN_BUG_FIX_DPH_WITH_EOB_CLEARED_Pos 16UL
#define USB32DEV_MAIN_BUG_FIX_DPH_WITH_EOB_CLEARED_Msk 0xFFFF0000UL


/* USB32DEV_EPM.EEPM_CS */
#define USB32DEV_EPM_EEPM_CS_A_RQ_EMPTY_DLY_Pos 0UL
#define USB32DEV_EPM_EEPM_CS_A_RQ_EMPTY_DLY_Msk 0x1FUL
#define USB32DEV_EPM_EEPM_CS_URUN_T1_EP_NUM_Pos 18UL
#define USB32DEV_EPM_EEPM_CS_URUN_T1_EP_NUM_Msk 0x3C0000UL
#define USB32DEV_EPM_EEPM_CS_URUN_T2_EP_NUM_Pos 22UL
#define USB32DEV_EPM_EEPM_CS_URUN_T2_EP_NUM_Msk 0x3C00000UL
#define USB32DEV_EPM_EEPM_CS_EG_EPNUM_Pos 28UL
#define USB32DEV_EPM_EEPM_CS_EG_EPNUM_Msk 0xF0000000UL
/* USB32DEV_EPM.IEPM_CS */
#define USB32DEV_EPM_IEPM_CS_READ_PTR_Pos 0UL
#define USB32DEV_EPM_IEPM_CS_READ_PTR_Msk 0xFFFUL
#define USB32DEV_EPM_IEPM_CS_WRITE_PTR_Pos 16UL
#define USB32DEV_EPM_IEPM_CS_WRITE_PTR_Msk 0xFFF0000UL
#define USB32DEV_EPM_IEPM_CS_EPM_FLUSH_Pos 28UL
#define USB32DEV_EPM_IEPM_CS_EPM_FLUSH_Msk 0x10000000UL
#define USB32DEV_EPM_IEPM_CS_DISABLE_IEPM_CLK_GT_Pos 31UL
#define USB32DEV_EPM_IEPM_CS_DISABLE_IEPM_CLK_GT_Msk 0x80000000UL
/* USB32DEV_EPM.IEPM_MULT */
#define USB32DEV_EPM_IEPM_MULT_MULT_EN_Pos 0UL
#define USB32DEV_EPM_IEPM_MULT_MULT_EN_Msk 0x7FFFUL
#define USB32DEV_EPM_IEPM_MULT_MULT_THRSHOLD_Pos 15UL
#define USB32DEV_EPM_IEPM_MULT_MULT_THRSHOLD_Msk 0x3FF8000UL
/* USB32DEV_EPM.EEPM_ENDPOINT */
#define USB32DEV_EPM_EEPM_ENDPOINT_PACKET_SIZE_Pos 0UL
#define USB32DEV_EPM_EEPM_ENDPOINT_PACKET_SIZE_Msk 0x7FFUL
#define USB32DEV_EPM_EEPM_ENDPOINT_EEPM_BYTE_COUNT_Pos 11UL
#define USB32DEV_EPM_EEPM_ENDPOINT_EEPM_BYTE_COUNT_Msk 0x7FFF800UL
#define USB32DEV_EPM_EEPM_ENDPOINT_ZLP_Pos 27UL
#define USB32DEV_EPM_EEPM_ENDPOINT_ZLP_Msk 0x8000000UL
#define USB32DEV_EPM_EEPM_ENDPOINT_MSB_EEPM_BYTE_COUNT_Pos 28UL
#define USB32DEV_EPM_EEPM_ENDPOINT_MSB_EEPM_BYTE_COUNT_Msk 0x10000000UL
#define USB32DEV_EPM_EEPM_ENDPOINT_EPM_EMPTY_Pos 29UL
#define USB32DEV_EPM_EEPM_ENDPOINT_EPM_EMPTY_Msk 0x20000000UL
#define USB32DEV_EPM_EEPM_ENDPOINT_EEPM_EP_READY_Pos 30UL
#define USB32DEV_EPM_EEPM_ENDPOINT_EEPM_EP_READY_Msk 0x40000000UL
#define USB32DEV_EPM_EEPM_ENDPOINT_SOCKET_FLUSH_Pos 31UL
#define USB32DEV_EPM_EEPM_ENDPOINT_SOCKET_FLUSH_Msk 0x80000000UL
/* USB32DEV_EPM.IEPM_ENDPOINT */
#define USB32DEV_EPM_IEPM_ENDPOINT_PACKET_SIZE_Pos 0UL
#define USB32DEV_EPM_IEPM_ENDPOINT_PACKET_SIZE_Msk 0x7FFUL
#define USB32DEV_EPM_IEPM_ENDPOINT_NUM_IN_PACKETS_Pos 11UL
#define USB32DEV_EPM_IEPM_ENDPOINT_NUM_IN_PACKETS_Msk 0x3FF800UL
#define USB32DEV_EPM_IEPM_ENDPOINT_EP_READY_Pos 22UL
#define USB32DEV_EPM_IEPM_ENDPOINT_EP_READY_Msk 0x400000UL
#define USB32DEV_EPM_IEPM_ENDPOINT_ODD_MAX_NUM_PKTS_Pos 24UL
#define USB32DEV_EPM_IEPM_ENDPOINT_ODD_MAX_NUM_PKTS_Msk 0x1F000000UL
#define USB32DEV_EPM_IEPM_ENDPOINT_ODD_MAX_PKT_SIZE_EN_Pos 29UL
#define USB32DEV_EPM_IEPM_ENDPOINT_ODD_MAX_PKT_SIZE_EN_Msk 0x20000000UL
#define USB32DEV_EPM_IEPM_ENDPOINT_EOT_EOP_Pos 30UL
#define USB32DEV_EPM_IEPM_ENDPOINT_EOT_EOP_Msk 0x40000000UL
#define USB32DEV_EPM_IEPM_ENDPOINT_SOCKET_FLUSH_Pos 31UL
#define USB32DEV_EPM_IEPM_ENDPOINT_SOCKET_FLUSH_Msk 0x80000000UL
/* USB32DEV_EPM.IEPM_FIFO */
#define USB32DEV_EPM_IEPM_FIFO_BYTES_Pos 0UL
#define USB32DEV_EPM_IEPM_FIFO_BYTES_Msk 0x7FFUL
#define USB32DEV_EPM_IEPM_FIFO_EOT_Pos 11UL
#define USB32DEV_EPM_IEPM_FIFO_EOT_Msk 0x800UL
#define USB32DEV_EPM_IEPM_FIFO_IN_EPNUM_Pos 12UL
#define USB32DEV_EPM_IEPM_FIFO_IN_EPNUM_Msk 0xF000UL
#define USB32DEV_EPM_IEPM_FIFO_EP_VALID_Pos 16UL
#define USB32DEV_EPM_IEPM_FIFO_EP_VALID_Msk 0x10000UL
/* USB32DEV_EPM.EEPM_VALID */
#define USB32DEV_EPM_EEPM_VALID_SKT_NUM_Pos 0UL
#define USB32DEV_EPM_EEPM_VALID_SKT_NUM_Msk 0xFUL
#define USB32DEV_EPM_EEPM_VALID_VALID_PACKETS_Pos 16UL
#define USB32DEV_EPM_EEPM_VALID_VALID_PACKETS_Msk 0xFFFF0000UL
/* USB32DEV_EPM.EEPM_RETRY_OFFSET */
#define USB32DEV_EPM_EEPM_RETRY_OFFSET_START_OFFSET_Pos 0UL
#define USB32DEV_EPM_EEPM_RETRY_OFFSET_START_OFFSET_Msk 0x3FFFUL
/* USB32DEV_EPM.EEPM_INJECT_CRC32_ERROR */
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_EN_CRC32_ERROR_ON_SW_BUF_Pos 0UL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_EN_CRC32_ERROR_ON_SW_BUF_Msk 0x1UL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_EN_CRC32_ERROR_ON_RETRY_Pos 1UL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_EN_CRC32_ERROR_ON_RETRY_Msk 0x2UL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_SW_BUF_PKT_NUM_Pos 2UL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_SW_BUF_PKT_NUM_Msk 0x3CUL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_RETRY_BUF_PKT_NUM_Pos 6UL
#define USB32DEV_EPM_EEPM_INJECT_CRC32_ERROR_RETRY_BUF_PKT_NUM_Msk 0x3C0UL


/* USB32DEV_LNK.LNK_CONF */
#define USB32DEV_LNK_CONF_TX_ARBITRATION_Pos 0UL
#define USB32DEV_LNK_CONF_TX_ARBITRATION_Msk 0x3UL
#define USB32DEV_LNK_CONF_LCW_IGNORE_RSVD_Pos 6UL
#define USB32DEV_LNK_CONF_LCW_IGNORE_RSVD_Msk 0x40UL
#define USB32DEV_LNK_CONF_DEBUG_FEATURE_ENABLE_Pos 7UL
#define USB32DEV_LNK_CONF_DEBUG_FEATURE_ENABLE_Msk 0x80UL
#define USB32DEV_LNK_CONF_FORCE_POWER_PRESENT_Pos 8UL
#define USB32DEV_LNK_CONF_FORCE_POWER_PRESENT_Msk 0x100UL
#define USB32DEV_LNK_CONF_LDN_DETECTION_Pos 9UL
#define USB32DEV_LNK_CONF_LDN_DETECTION_Msk 0x200UL
#define USB32DEV_LNK_CONF_CREDIT_ADV_HOLDOFF_Pos 10UL
#define USB32DEV_LNK_CONF_CREDIT_ADV_HOLDOFF_Msk 0x400UL
#define USB32DEV_LNK_CONF_GEN1X2_SKP_RULE_SEL_Pos 11UL
#define USB32DEV_LNK_CONF_GEN1X2_SKP_RULE_SEL_Msk 0x800UL
#define USB32DEV_LNK_CONF_EPM_FIRST_DELAY_Pos 12UL
#define USB32DEV_LNK_CONF_EPM_FIRST_DELAY_Msk 0xF000UL
#define USB32DEV_LNK_CONF_GEN1X1_INVALID_OS_DET_EN_Pos 16UL
#define USB32DEV_LNK_CONF_GEN1X1_INVALID_OS_DET_EN_Msk 0x10000UL
#define USB32DEV_LNK_CONF_GEN1X1_CP_TXDETECT_RXLPBK_EN_Pos 17UL
#define USB32DEV_LNK_CONF_GEN1X1_CP_TXDETECT_RXLPBK_EN_Msk 0x20000UL
#define USB32DEV_LNK_CONF_GEN1X1_LOOPBACK_MASTER_SKP_EN_Pos 18UL
#define USB32DEV_LNK_CONF_GEN1X1_LOOPBACK_MASTER_SKP_EN_Msk 0x40000UL
#define USB32DEV_LNK_CONF_DC_BAL_ALL_OS_Pos 19UL
#define USB32DEV_LNK_CONF_DC_BAL_ALL_OS_Msk 0x80000UL
#define USB32DEV_LNK_CONF_DC_BAL_NON_OS_RESET_Pos 20UL
#define USB32DEV_LNK_CONF_DC_BAL_NON_OS_RESET_Msk 0x100000UL
#define USB32DEV_LNK_CONF_SKP_IN_RXEQ_OFF_Pos 21UL
#define USB32DEV_LNK_CONF_SKP_IN_RXEQ_OFF_Msk 0x200000UL
#define USB32DEV_LNK_CONF_DC_BAL_EN_Pos 24UL
#define USB32DEV_LNK_CONF_DC_BAL_EN_Msk 0x1000000UL
#define USB32DEV_LNK_CONF_RX_DPP_ERR_GO_RECOVERY_EN_Pos 25UL
#define USB32DEV_LNK_CONF_RX_DPP_ERR_GO_RECOVERY_EN_Msk 0x2000000UL
#define USB32DEV_LNK_CONF_SKEW_VAL_Pos 26UL
#define USB32DEV_LNK_CONF_SKEW_VAL_Msk 0x7C000000UL
#define USB32DEV_LNK_CONF_DESKEW_CTRL_SEL_Pos 31UL
#define USB32DEV_LNK_CONF_DESKEW_CTRL_SEL_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_INTR */
#define USB32DEV_LNK_INTR_LTSSM_STATE_CHG_Pos 0UL
#define USB32DEV_LNK_INTR_LTSSM_STATE_CHG_Msk 0x1UL
#define USB32DEV_LNK_INTR_LGOOD_Pos 1UL
#define USB32DEV_LNK_INTR_LGOOD_Msk 0x2UL
#define USB32DEV_LNK_INTR_LRTY_Pos 2UL
#define USB32DEV_LNK_INTR_LRTY_Msk 0x4UL
#define USB32DEV_LNK_INTR_LBAD_Pos 3UL
#define USB32DEV_LNK_INTR_LBAD_Msk 0x8UL
#define USB32DEV_LNK_INTR_LCRD_Pos 4UL
#define USB32DEV_LNK_INTR_LCRD_Msk 0x10UL
#define USB32DEV_LNK_INTR_LGO_U1_Pos 5UL
#define USB32DEV_LNK_INTR_LGO_U1_Msk 0x20UL
#define USB32DEV_LNK_INTR_LGO_U2_Pos 6UL
#define USB32DEV_LNK_INTR_LGO_U2_Msk 0x40UL
#define USB32DEV_LNK_INTR_LGO_U3_Pos 7UL
#define USB32DEV_LNK_INTR_LGO_U3_Msk 0x80UL
#define USB32DEV_LNK_INTR_LAU_Pos 8UL
#define USB32DEV_LNK_INTR_LAU_Msk 0x100UL
#define USB32DEV_LNK_INTR_LXU_Pos 9UL
#define USB32DEV_LNK_INTR_LXU_Msk 0x200UL
#define USB32DEV_LNK_INTR_LPMA_Pos 10UL
#define USB32DEV_LNK_INTR_LPMA_Msk 0x400UL
#define USB32DEV_LNK_INTR_BAD_LCW_Pos 11UL
#define USB32DEV_LNK_INTR_BAD_LCW_Msk 0x800UL
#define USB32DEV_LNK_INTR_LINK_ERROR_Pos 12UL
#define USB32DEV_LNK_INTR_LINK_ERROR_Msk 0x1000UL
#define USB32DEV_LNK_INTR_PHY_ERROR_Pos 13UL
#define USB32DEV_LNK_INTR_PHY_ERROR_Msk 0x2000UL
#define USB32DEV_LNK_INTR_U2_INACTIVITY_TIMEOUT_Pos 14UL
#define USB32DEV_LNK_INTR_U2_INACTIVITY_TIMEOUT_Msk 0x4000UL
#define USB32DEV_LNK_INTR_LTSSM_CONNECT_Pos 15UL
#define USB32DEV_LNK_INTR_LTSSM_CONNECT_Msk 0x8000UL
#define USB32DEV_LNK_INTR_LTSSM_DISCONNECT_Pos 16UL
#define USB32DEV_LNK_INTR_LTSSM_DISCONNECT_Msk 0x10000UL
#define USB32DEV_LNK_INTR_LTSSM_RESET_Pos 17UL
#define USB32DEV_LNK_INTR_LTSSM_RESET_Msk 0x20000UL
#define USB32DEV_LNK_INTR_DATA_RATE_CHANGE_Pos 18UL
#define USB32DEV_LNK_INTR_DATA_RATE_CHANGE_Msk 0x40000UL
#define USB32DEV_LNK_INTR_LTSSM_U3_ENTRY_Pos 19UL
#define USB32DEV_LNK_INTR_LTSSM_U3_ENTRY_Msk 0x80000UL
#define USB32DEV_LNK_INTR_RX_PING_LFPS_Pos 20UL
#define USB32DEV_LNK_INTR_RX_PING_LFPS_Msk 0x100000UL
/* USB32DEV_LNK.LNK_INTR_SET */
#define USB32DEV_LNK_INTR_SET_LTSSM_STATE_CHG_Pos 0UL
#define USB32DEV_LNK_INTR_SET_LTSSM_STATE_CHG_Msk 0x1UL
#define USB32DEV_LNK_INTR_SET_LGOOD_Pos 1UL
#define USB32DEV_LNK_INTR_SET_LGOOD_Msk 0x2UL
#define USB32DEV_LNK_INTR_SET_LRTY_Pos 2UL
#define USB32DEV_LNK_INTR_SET_LRTY_Msk 0x4UL
#define USB32DEV_LNK_INTR_SET_LBAD_Pos 3UL
#define USB32DEV_LNK_INTR_SET_LBAD_Msk 0x8UL
#define USB32DEV_LNK_INTR_SET_LCRD_Pos 4UL
#define USB32DEV_LNK_INTR_SET_LCRD_Msk 0x10UL
#define USB32DEV_LNK_INTR_SET_LGO_U1_Pos 5UL
#define USB32DEV_LNK_INTR_SET_LGO_U1_Msk 0x20UL
#define USB32DEV_LNK_INTR_SET_LGO_U2_Pos 6UL
#define USB32DEV_LNK_INTR_SET_LGO_U2_Msk 0x40UL
#define USB32DEV_LNK_INTR_SET_LGO_U3_Pos 7UL
#define USB32DEV_LNK_INTR_SET_LGO_U3_Msk 0x80UL
#define USB32DEV_LNK_INTR_SET_LAU_Pos 8UL
#define USB32DEV_LNK_INTR_SET_LAU_Msk 0x100UL
#define USB32DEV_LNK_INTR_SET_LXU_Pos 9UL
#define USB32DEV_LNK_INTR_SET_LXU_Msk 0x200UL
#define USB32DEV_LNK_INTR_SET_LPMA_Pos 10UL
#define USB32DEV_LNK_INTR_SET_LPMA_Msk 0x400UL
#define USB32DEV_LNK_INTR_SET_BAD_LCW_Pos 11UL
#define USB32DEV_LNK_INTR_SET_BAD_LCW_Msk 0x800UL
#define USB32DEV_LNK_INTR_SET_LINK_ERROR_Pos 12UL
#define USB32DEV_LNK_INTR_SET_LINK_ERROR_Msk 0x1000UL
#define USB32DEV_LNK_INTR_SET_PHY_ERROR_Pos 13UL
#define USB32DEV_LNK_INTR_SET_PHY_ERROR_Msk 0x2000UL
#define USB32DEV_LNK_INTR_SET_U2_INACTIVITY_TIMEOUT_Pos 14UL
#define USB32DEV_LNK_INTR_SET_U2_INACTIVITY_TIMEOUT_Msk 0x4000UL
#define USB32DEV_LNK_INTR_SET_LTSSM_CONNECT_Pos 15UL
#define USB32DEV_LNK_INTR_SET_LTSSM_CONNECT_Msk 0x8000UL
#define USB32DEV_LNK_INTR_SET_LTSSM_DISCONNECT_Pos 16UL
#define USB32DEV_LNK_INTR_SET_LTSSM_DISCONNECT_Msk 0x10000UL
#define USB32DEV_LNK_INTR_SET_LTSSM_RESET_Pos 17UL
#define USB32DEV_LNK_INTR_SET_LTSSM_RESET_Msk 0x20000UL
#define USB32DEV_LNK_INTR_SET_DATA_RATE_CHANGE_Pos 18UL
#define USB32DEV_LNK_INTR_SET_DATA_RATE_CHANGE_Msk 0x40000UL
#define USB32DEV_LNK_INTR_SET_LTSSM_U3_ENTRY_Pos 19UL
#define USB32DEV_LNK_INTR_SET_LTSSM_U3_ENTRY_Msk 0x80000UL
#define USB32DEV_LNK_INTR_SET_RX_PING_LFPS_Pos 20UL
#define USB32DEV_LNK_INTR_SET_RX_PING_LFPS_Msk 0x100000UL
/* USB32DEV_LNK.LNK_INTR_MASK */
#define USB32DEV_LNK_INTR_MASK_LTSSM_STATE_CHG_MASK_Pos 0UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_STATE_CHG_MASK_Msk 0x1UL
#define USB32DEV_LNK_INTR_MASK_LGOOD_MASK_Pos 1UL
#define USB32DEV_LNK_INTR_MASK_LGOOD_MASK_Msk 0x2UL
#define USB32DEV_LNK_INTR_MASK_LRTY_MASK_Pos 2UL
#define USB32DEV_LNK_INTR_MASK_LRTY_MASK_Msk 0x4UL
#define USB32DEV_LNK_INTR_MASK_LBAD_MASK_Pos 3UL
#define USB32DEV_LNK_INTR_MASK_LBAD_MASK_Msk 0x8UL
#define USB32DEV_LNK_INTR_MASK_LCRD_MASK_Pos 4UL
#define USB32DEV_LNK_INTR_MASK_LCRD_MASK_Msk 0x10UL
#define USB32DEV_LNK_INTR_MASK_LGO_U1_MASK_Pos 5UL
#define USB32DEV_LNK_INTR_MASK_LGO_U1_MASK_Msk 0x20UL
#define USB32DEV_LNK_INTR_MASK_LGO_U2_MASK_Pos 6UL
#define USB32DEV_LNK_INTR_MASK_LGO_U2_MASK_Msk 0x40UL
#define USB32DEV_LNK_INTR_MASK_LGO_U3_MASK_Pos 7UL
#define USB32DEV_LNK_INTR_MASK_LGO_U3_MASK_Msk 0x80UL
#define USB32DEV_LNK_INTR_MASK_LAU_MASK_Pos 8UL
#define USB32DEV_LNK_INTR_MASK_LAU_MASK_Msk 0x100UL
#define USB32DEV_LNK_INTR_MASK_LXU_MASK_Pos 9UL
#define USB32DEV_LNK_INTR_MASK_LXU_MASK_Msk 0x200UL
#define USB32DEV_LNK_INTR_MASK_LPMA_MASK_Pos 10UL
#define USB32DEV_LNK_INTR_MASK_LPMA_MASK_Msk 0x400UL
#define USB32DEV_LNK_INTR_MASK_BAD_LCW_MASK_Pos 11UL
#define USB32DEV_LNK_INTR_MASK_BAD_LCW_MASK_Msk 0x800UL
#define USB32DEV_LNK_INTR_MASK_LINK_ERROR_MASK_Pos 12UL
#define USB32DEV_LNK_INTR_MASK_LINK_ERROR_MASK_Msk 0x1000UL
#define USB32DEV_LNK_INTR_MASK_PHY_ERROR_MASK_Pos 13UL
#define USB32DEV_LNK_INTR_MASK_PHY_ERROR_MASK_Msk 0x2000UL
#define USB32DEV_LNK_INTR_MASK_U2_INACTIVITY_TIMEOUT_MASK_Pos 14UL
#define USB32DEV_LNK_INTR_MASK_U2_INACTIVITY_TIMEOUT_MASK_Msk 0x4000UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_CONNECT_MASK_Pos 15UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_CONNECT_MASK_Msk 0x8000UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_DISCONNECT_MASK_Pos 16UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_DISCONNECT_MASK_Msk 0x10000UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_RESET_MASK_Pos 17UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_RESET_MASK_Msk 0x20000UL
#define USB32DEV_LNK_INTR_MASK_DATA_RATE_CHANGE_MASK_Pos 18UL
#define USB32DEV_LNK_INTR_MASK_DATA_RATE_CHANGE_MASK_Msk 0x40000UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_U3_ENTRY_MASK_Pos 19UL
#define USB32DEV_LNK_INTR_MASK_LTSSM_U3_ENTRY_MASK_Msk 0x80000UL
#define USB32DEV_LNK_INTR_MASK_RX_PING_LFPS_Pos 20UL
#define USB32DEV_LNK_INTR_MASK_RX_PING_LFPS_Msk 0x100000UL
/* USB32DEV_LNK.LNK_INTR_MASKED */
#define USB32DEV_LNK_INTR_MASKED_LTSSM_STATE_CHG_MASKED_Pos 0UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_STATE_CHG_MASKED_Msk 0x1UL
#define USB32DEV_LNK_INTR_MASKED_LGOOD_MASKED_Pos 1UL
#define USB32DEV_LNK_INTR_MASKED_LGOOD_MASKED_Msk 0x2UL
#define USB32DEV_LNK_INTR_MASKED_LRTY_MASKED_Pos 2UL
#define USB32DEV_LNK_INTR_MASKED_LRTY_MASKED_Msk 0x4UL
#define USB32DEV_LNK_INTR_MASKED_LBAD_MASKED_Pos 3UL
#define USB32DEV_LNK_INTR_MASKED_LBAD_MASKED_Msk 0x8UL
#define USB32DEV_LNK_INTR_MASKED_LCRD_MASKED_Pos 4UL
#define USB32DEV_LNK_INTR_MASKED_LCRD_MASKED_Msk 0x10UL
#define USB32DEV_LNK_INTR_MASKED_LGO_U1_MASKED_Pos 5UL
#define USB32DEV_LNK_INTR_MASKED_LGO_U1_MASKED_Msk 0x20UL
#define USB32DEV_LNK_INTR_MASKED_LGO_U2_MASKED_Pos 6UL
#define USB32DEV_LNK_INTR_MASKED_LGO_U2_MASKED_Msk 0x40UL
#define USB32DEV_LNK_INTR_MASKED_LGO_U3_MASKED_Pos 7UL
#define USB32DEV_LNK_INTR_MASKED_LGO_U3_MASKED_Msk 0x80UL
#define USB32DEV_LNK_INTR_MASKED_LAU_MASKED_Pos 8UL
#define USB32DEV_LNK_INTR_MASKED_LAU_MASKED_Msk 0x100UL
#define USB32DEV_LNK_INTR_MASKED_LXU_MASKED_Pos 9UL
#define USB32DEV_LNK_INTR_MASKED_LXU_MASKED_Msk 0x200UL
#define USB32DEV_LNK_INTR_MASKED_LPMA_MASKED_Pos 10UL
#define USB32DEV_LNK_INTR_MASKED_LPMA_MASKED_Msk 0x400UL
#define USB32DEV_LNK_INTR_MASKED_BAD_LCW_MASKED_Pos 11UL
#define USB32DEV_LNK_INTR_MASKED_BAD_LCW_MASKED_Msk 0x800UL
#define USB32DEV_LNK_INTR_MASKED_LINK_ERROR_MASKED_Pos 12UL
#define USB32DEV_LNK_INTR_MASKED_LINK_ERROR_MASKED_Msk 0x1000UL
#define USB32DEV_LNK_INTR_MASKED_PHY_ERROR_MASKED_Pos 13UL
#define USB32DEV_LNK_INTR_MASKED_PHY_ERROR_MASKED_Msk 0x2000UL
#define USB32DEV_LNK_INTR_MASKED_U2_INACTIVITY_TIMEOUT_MASKED_Pos 14UL
#define USB32DEV_LNK_INTR_MASKED_U2_INACTIVITY_TIMEOUT_MASKED_Msk 0x4000UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_CONNECT_MASKED_Pos 15UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_CONNECT_MASKED_Msk 0x8000UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_DISCONNECT_MASKED_Pos 16UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_DISCONNECT_MASKED_Msk 0x10000UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_RESET_MASKED_Pos 17UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_RESET_MASKED_Msk 0x20000UL
#define USB32DEV_LNK_INTR_MASKED_DATA_RATE_CHANGE_MASKED_Pos 18UL
#define USB32DEV_LNK_INTR_MASKED_DATA_RATE_CHANGE_MASKED_Msk 0x40000UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_U3_ENTRY_MASKED_Pos 19UL
#define USB32DEV_LNK_INTR_MASKED_LTSSM_U3_ENTRY_MASKED_Msk 0x80000UL
#define USB32DEV_LNK_INTR_MASKED_RX_PING_LFPS_Pos 20UL
#define USB32DEV_LNK_INTR_MASKED_RX_PING_LFPS_Msk 0x100000UL
/* USB32DEV_LNK.LNK_ERROR_CONF */
#define USB32DEV_LNK_ERROR_CONF_HP_TIMEOUT_EN_Pos 0UL
#define USB32DEV_LNK_ERROR_CONF_HP_TIMEOUT_EN_Msk 0x1UL
#define USB32DEV_LNK_ERROR_CONF_RX_SEQ_NUM_ERR_EN_Pos 1UL
#define USB32DEV_LNK_ERROR_CONF_RX_SEQ_NUM_ERR_EN_Msk 0x2UL
#define USB32DEV_LNK_ERROR_CONF_RX_HP_FAIL_EN_Pos 2UL
#define USB32DEV_LNK_ERROR_CONF_RX_HP_FAIL_EN_Msk 0x4UL
#define USB32DEV_LNK_ERROR_CONF_MISSING_LGOOD_EN_Pos 3UL
#define USB32DEV_LNK_ERROR_CONF_MISSING_LGOOD_EN_Msk 0x8UL
#define USB32DEV_LNK_ERROR_CONF_MISSING_LCRD_EN_Pos 4UL
#define USB32DEV_LNK_ERROR_CONF_MISSING_LCRD_EN_Msk 0x10UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_HP_TIMEOUT_EN_Pos 5UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_HP_TIMEOUT_EN_Msk 0x20UL
#define USB32DEV_LNK_ERROR_CONF_PM_LC_TIMEOUT_EN_Pos 6UL
#define USB32DEV_LNK_ERROR_CONF_PM_LC_TIMEOUT_EN_Msk 0x40UL
#define USB32DEV_LNK_ERROR_CONF_TX_SEQ_NUM_ERR_EN_Pos 7UL
#define USB32DEV_LNK_ERROR_CONF_TX_SEQ_NUM_ERR_EN_Msk 0x80UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_TIMEOUT_EN_Pos 8UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_TIMEOUT_EN_Msk 0x100UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_HP_EN_Pos 9UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_HP_EN_Msk 0x200UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_LCRD_EN_Pos 10UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_LCRD_EN_Msk 0x400UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_LGO_EN_Pos 11UL
#define USB32DEV_LNK_ERROR_CONF_HDR_ADV_LGO_EN_Msk 0x800UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_ADV_TIMEOUT_EN_Pos 12UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_ADV_TIMEOUT_EN_Msk 0x1000UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_ADV_HP_EN_Pos 13UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_ADV_HP_EN_Msk 0x2000UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_ADV_LGO_EN_Pos 14UL
#define USB32DEV_LNK_ERROR_CONF_CREDIT_ADV_LGO_EN_Msk 0x4000UL
/* USB32DEV_LNK.LNK_ERROR_STATUS */
#define USB32DEV_LNK_ERROR_STATUS_HP_TIMEOUT_EV_Pos 0UL
#define USB32DEV_LNK_ERROR_STATUS_HP_TIMEOUT_EV_Msk 0x1UL
#define USB32DEV_LNK_ERROR_STATUS_RX_SEQ_NUM_ERR_EV_Pos 1UL
#define USB32DEV_LNK_ERROR_STATUS_RX_SEQ_NUM_ERR_EV_Msk 0x2UL
#define USB32DEV_LNK_ERROR_STATUS_RX_HP_FAIL_EV_Pos 2UL
#define USB32DEV_LNK_ERROR_STATUS_RX_HP_FAIL_EV_Msk 0x4UL
#define USB32DEV_LNK_ERROR_STATUS_MISSING_LGOOD_EV_Pos 3UL
#define USB32DEV_LNK_ERROR_STATUS_MISSING_LGOOD_EV_Msk 0x8UL
#define USB32DEV_LNK_ERROR_STATUS_MISSING_LCRD_EV_Pos 4UL
#define USB32DEV_LNK_ERROR_STATUS_MISSING_LCRD_EV_Msk 0x10UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_HP_TIMEOUT_EV_Pos 5UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_HP_TIMEOUT_EV_Msk 0x20UL
#define USB32DEV_LNK_ERROR_STATUS_PM_LC_TIMEOUT_EV_Pos 6UL
#define USB32DEV_LNK_ERROR_STATUS_PM_LC_TIMEOUT_EV_Msk 0x40UL
#define USB32DEV_LNK_ERROR_STATUS_TX_SEQ_NUM_ERR_EV_Pos 7UL
#define USB32DEV_LNK_ERROR_STATUS_TX_SEQ_NUM_ERR_EV_Msk 0x80UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_TIMEOUT_EV_Pos 8UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_TIMEOUT_EV_Msk 0x100UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_HP_EV_Pos 9UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_HP_EV_Msk 0x200UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_LCRD_EV_Pos 10UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_LCRD_EV_Msk 0x400UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_LGO_EV_Pos 11UL
#define USB32DEV_LNK_ERROR_STATUS_HDR_ADV_LGO_EV_Msk 0x800UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_ADV_TIMEOUT_EV_Pos 12UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_ADV_TIMEOUT_EV_Msk 0x1000UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_ADV_HP_EV_Pos 13UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_ADV_HP_EV_Msk 0x2000UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_ADV_LGO_EV_Pos 14UL
#define USB32DEV_LNK_ERROR_STATUS_CREDIT_ADV_LGO_EV_Msk 0x4000UL
/* USB32DEV_LNK.LNK_ERROR_COUNT */
#define USB32DEV_LNK_ERROR_COUNT_LINK_ERROR_COUNT_Pos 0UL
#define USB32DEV_LNK_ERROR_COUNT_LINK_ERROR_COUNT_Msk 0xFFFFUL
#define USB32DEV_LNK_ERROR_COUNT_PHY_ERROR_COUNT_Pos 16UL
#define USB32DEV_LNK_ERROR_COUNT_PHY_ERROR_COUNT_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_ERROR_COUNT_THRESHOLD */
#define USB32DEV_LNK_ERROR_COUNT_THRESHOLD_LINK_ERROR_THRESHOLD_Pos 0UL
#define USB32DEV_LNK_ERROR_COUNT_THRESHOLD_LINK_ERROR_THRESHOLD_Msk 0xFFFFUL
#define USB32DEV_LNK_ERROR_COUNT_THRESHOLD_PHY_ERROR_THRESHOLD_Pos 16UL
#define USB32DEV_LNK_ERROR_COUNT_THRESHOLD_PHY_ERROR_THRESHOLD_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_PHY_CONF */
#define USB32DEV_LNK_PHY_CONF_PHY_MODE_Pos 0UL
#define USB32DEV_LNK_PHY_CONF_PHY_MODE_Msk 0x3UL
#define USB32DEV_LNK_PHY_CONF_ELASTICIY_BUFFER_MODE_Pos 2UL
#define USB32DEV_LNK_PHY_CONF_ELASTICIY_BUFFER_MODE_Msk 0x4UL
#define USB32DEV_LNK_PHY_CONF_TXDETECTRX_LB_OVR_Pos 3UL
#define USB32DEV_LNK_PHY_CONF_TXDETECTRX_LB_OVR_Msk 0x8UL
#define USB32DEV_LNK_PHY_CONF_TXDETECTRX_LB_OVR_VAL_Pos 4UL
#define USB32DEV_LNK_PHY_CONF_TXDETECTRX_LB_OVR_VAL_Msk 0x10UL
#define USB32DEV_LNK_PHY_CONF_TXELECIDLE_OVR_Pos 5UL
#define USB32DEV_LNK_PHY_CONF_TXELECIDLE_OVR_Msk 0x20UL
#define USB32DEV_LNK_PHY_CONF_TXELECIDLE_OVR_VAL_Pos 6UL
#define USB32DEV_LNK_PHY_CONF_TXELECIDLE_OVR_VAL_Msk 0x40UL
#define USB32DEV_LNK_PHY_CONF_TXCOMPLIANCE_OVR_Pos 7UL
#define USB32DEV_LNK_PHY_CONF_TXCOMPLIANCE_OVR_Msk 0x80UL
#define USB32DEV_LNK_PHY_CONF_TXCOMPLIANCE_OVR_VAL_Pos 8UL
#define USB32DEV_LNK_PHY_CONF_TXCOMPLIANCE_OVR_VAL_Msk 0x100UL
#define USB32DEV_LNK_PHY_CONF_TXONESZEROS_OVR_Pos 9UL
#define USB32DEV_LNK_PHY_CONF_TXONESZEROS_OVR_Msk 0x200UL
#define USB32DEV_LNK_PHY_CONF_TXONESZEROS_OVR_VAL_Pos 10UL
#define USB32DEV_LNK_PHY_CONF_TXONESZEROS_OVR_VAL_Msk 0x400UL
#define USB32DEV_LNK_PHY_CONF_RXPOLARITY_OVR_Pos 11UL
#define USB32DEV_LNK_PHY_CONF_RXPOLARITY_OVR_Msk 0x800UL
#define USB32DEV_LNK_PHY_CONF_RXPOLARITY_OVR_VAL_Pos 12UL
#define USB32DEV_LNK_PHY_CONF_RXPOLARITY_OVR_VAL_Msk 0x1000UL
#define USB32DEV_LNK_PHY_CONF_RXEQ_TRAINING_OVR_Pos 13UL
#define USB32DEV_LNK_PHY_CONF_RXEQ_TRAINING_OVR_Msk 0x2000UL
#define USB32DEV_LNK_PHY_CONF_RXEQ_TRAINING_OVR_VAL_Pos 14UL
#define USB32DEV_LNK_PHY_CONF_RXEQ_TRAINING_OVR_VAL_Msk 0x4000UL
#define USB32DEV_LNK_PHY_CONF_PHY_RESET_N_OVR_Pos 15UL
#define USB32DEV_LNK_PHY_CONF_PHY_RESET_N_OVR_Msk 0x8000UL
#define USB32DEV_LNK_PHY_CONF_PHY_RESET_N_OVR_VAL_Pos 16UL
#define USB32DEV_LNK_PHY_CONF_PHY_RESET_N_OVR_VAL_Msk 0x10000UL
#define USB32DEV_LNK_PHY_CONF_PHY_POWERDOWN_OVR_Pos 17UL
#define USB32DEV_LNK_PHY_CONF_PHY_POWERDOWN_OVR_Msk 0x20000UL
#define USB32DEV_LNK_PHY_CONF_PHY_POWERDOWN_OVR_VAL_Pos 18UL
#define USB32DEV_LNK_PHY_CONF_PHY_POWERDOWN_OVR_VAL_Msk 0xC0000UL
#define USB32DEV_LNK_PHY_CONF_PHY_RATE_OVR_Pos 20UL
#define USB32DEV_LNK_PHY_CONF_PHY_RATE_OVR_Msk 0x100000UL
#define USB32DEV_LNK_PHY_CONF_PHY_RATE_OVR_VAL_Pos 21UL
#define USB32DEV_LNK_PHY_CONF_PHY_RATE_OVR_VAL_Msk 0x200000UL
#define USB32DEV_LNK_PHY_CONF_PHY_TX_DEEMPH_OVR_Pos 22UL
#define USB32DEV_LNK_PHY_CONF_PHY_TX_DEEMPH_OVR_Msk 0x400000UL
#define USB32DEV_LNK_PHY_CONF_PHY_TX_DEEMPH_OVR_VAL_Pos 23UL
#define USB32DEV_LNK_PHY_CONF_PHY_TX_DEEMPH_OVR_VAL_Msk 0x1800000UL
#define USB32DEV_LNK_PHY_CONF_PHY_TX_MARGIN_Pos 25UL
#define USB32DEV_LNK_PHY_CONF_PHY_TX_MARGIN_Msk 0xE000000UL
#define USB32DEV_LNK_PHY_CONF_TXSWING_Pos 28UL
#define USB32DEV_LNK_PHY_CONF_TXSWING_Msk 0x10000000UL
#define USB32DEV_LNK_PHY_CONF_RX_TERMINATION_OVR_Pos 29UL
#define USB32DEV_LNK_PHY_CONF_RX_TERMINATION_OVR_Msk 0x20000000UL
#define USB32DEV_LNK_PHY_CONF_RX_TERMINATION_OVR_VAL_Pos 30UL
#define USB32DEV_LNK_PHY_CONF_RX_TERMINATION_OVR_VAL_Msk 0x40000000UL
#define USB32DEV_LNK_PHY_CONF_RX_TERMINATION_ENABLE_Pos 31UL
#define USB32DEV_LNK_PHY_CONF_RX_TERMINATION_ENABLE_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_PHY_STATUS */
#define USB32DEV_LNK_PHY_STATUS_RXVALID_Pos 0UL
#define USB32DEV_LNK_PHY_STATUS_RXVALID_Msk 0x1UL
#define USB32DEV_LNK_PHY_STATUS_PHY_STATUS_Pos 1UL
#define USB32DEV_LNK_PHY_STATUS_PHY_STATUS_Msk 0x2UL
#define USB32DEV_LNK_PHY_STATUS_RX_ELEC_IDLE_Pos 2UL
#define USB32DEV_LNK_PHY_STATUS_RX_ELEC_IDLE_Msk 0x4UL
#define USB32DEV_LNK_PHY_STATUS_RXSTATUS_Pos 3UL
#define USB32DEV_LNK_PHY_STATUS_RXSTATUS_Msk 0x38UL
#define USB32DEV_LNK_PHY_STATUS_POWER_PRESENT_Pos 6UL
#define USB32DEV_LNK_PHY_STATUS_POWER_PRESENT_Msk 0x40UL
#define USB32DEV_LNK_PHY_STATUS_DATA_BUS_WIDTH_Pos 7UL
#define USB32DEV_LNK_PHY_STATUS_DATA_BUS_WIDTH_Msk 0x180UL
#define USB32DEV_LNK_PHY_STATUS_RXCONNECT_Pos 9UL
#define USB32DEV_LNK_PHY_STATUS_RXCONNECT_Msk 0x200UL
/* USB32DEV_LNK.LNK_PHY_ERROR_CONF */
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_DECODE_EN_Pos 0UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_DECODE_EN_Msk 0x1UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_EB_OVR_EN_Pos 1UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_EB_OVR_EN_Msk 0x2UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_EB_UND_EN_Pos 2UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_EB_UND_EN_Msk 0x4UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_DISPARITY_EN_Pos 3UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_ERROR_DISPARITY_EN_Msk 0x8UL
#define USB32DEV_LNK_PHY_ERROR_CONF_RX_ERROR_CRC5_EN_Pos 4UL
#define USB32DEV_LNK_PHY_ERROR_CONF_RX_ERROR_CRC5_EN_Msk 0x10UL
#define USB32DEV_LNK_PHY_ERROR_CONF_RX_ERROR_CRC16_EN_Pos 5UL
#define USB32DEV_LNK_PHY_ERROR_CONF_RX_ERROR_CRC16_EN_Msk 0x20UL
#define USB32DEV_LNK_PHY_ERROR_CONF_RX_ERROR_CRC32_EN_Pos 6UL
#define USB32DEV_LNK_PHY_ERROR_CONF_RX_ERROR_CRC32_EN_Msk 0x40UL
#define USB32DEV_LNK_PHY_ERROR_CONF_TRAINING_ERROR_EN_Pos 7UL
#define USB32DEV_LNK_PHY_ERROR_CONF_TRAINING_ERROR_EN_Msk 0x80UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_LOCK_EN_Pos 8UL
#define USB32DEV_LNK_PHY_ERROR_CONF_PHY_LOCK_EN_Msk 0x100UL
/* USB32DEV_LNK.LNK_PHY_ERROR_STATUS */
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_DECODE_EV_Pos 0UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_DECODE_EV_Msk 0x1UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_EB_OVR_EV_Pos 1UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_EB_OVR_EV_Msk 0x2UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_EB_UND_EV_Pos 2UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_EB_UND_EV_Msk 0x4UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_DISPARITY_EV_Pos 3UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_ERROR_DISPARITY_EV_Msk 0x8UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_RX_ERROR_CRC5_EV_Pos 4UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_RX_ERROR_CRC5_EV_Msk 0x10UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_RX_ERROR_CRC16_EV_Pos 5UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_RX_ERROR_CRC16_EV_Msk 0x20UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_RX_ERROR_CRC32_EV_Pos 6UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_RX_ERROR_CRC32_EV_Msk 0x40UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_TRAINING_ERROR_EV_Pos 7UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_TRAINING_ERROR_EV_Msk 0x80UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_LOCK_EV_Pos 8UL
#define USB32DEV_LNK_PHY_ERROR_STATUS_PHY_LOCK_EV_Msk 0x100UL
/* USB32DEV_LNK.LNK_PHY_TRAINING_HOLDOFF */
#define USB32DEV_LNK_PHY_TRAINING_HOLDOFF_GEN1_HOLDOFF_Pos 0UL
#define USB32DEV_LNK_PHY_TRAINING_HOLDOFF_GEN1_HOLDOFF_Msk 0xFFFFUL
#define USB32DEV_LNK_PHY_TRAINING_HOLDOFF_GEN2_HOLDOFF_Pos 16UL
#define USB32DEV_LNK_PHY_TRAINING_HOLDOFF_GEN2_HOLDOFF_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_COMMAND_WORD */
#define USB32DEV_LNK_COMMAND_WORD_COMMAND_Pos 0UL
#define USB32DEV_LNK_COMMAND_WORD_COMMAND_Msk 0x7FFUL
/* USB32DEV_LNK.LNK_DEVICE_POWER_CONTROL */
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_U1_Pos 0UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_U1_Msk 0x1UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_U2_Pos 1UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_U2_Msk 0x2UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_U3_Pos 2UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_U3_Msk 0x4UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_RX_U1_Pos 4UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_RX_U1_Msk 0x10UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_RX_U2_Pos 5UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_RX_U2_Msk 0x20UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_RX_U3_Pos 6UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_RX_U3_Msk 0x40UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_LAU_Pos 7UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_LAU_Msk 0x80UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_LXU_Pos 8UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_TX_LXU_Msk 0x100UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_EXIT_LP_Pos 9UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_EXIT_LP_Msk 0x200UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_U3_EXIT_ON_HOLD_Pos 16UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_U3_EXIT_ON_HOLD_Msk 0x10000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_U3_EXIT_ON_HOLD_EN_Pos 17UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_U3_EXIT_ON_HOLD_EN_Msk 0x20000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_AUTO_U1_Pos 26UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_AUTO_U1_Msk 0x4000000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_AUTO_U2_Pos 27UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_AUTO_U2_Msk 0x8000000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_NO_U1_Pos 28UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_NO_U1_Msk 0x10000000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_NO_U2_Pos 29UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_NO_U2_Msk 0x20000000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_YES_U1_Pos 30UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_YES_U1_Msk 0x40000000UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_YES_U2_Pos 31UL
#define USB32DEV_LNK_DEVICE_POWER_CONTROL_YES_U2_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_LTSSM_STATE */
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_STATE_Pos 0UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_STATE_Msk 0x3FUL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_OVERRIDE_VALUE_Pos 6UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_OVERRIDE_VALUE_Msk 0xFC0UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_OVERRIDE_EN_Pos 12UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_OVERRIDE_EN_Msk 0x1000UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_OVERRIDE_GO_Pos 13UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_OVERRIDE_GO_Msk 0x2000UL
#define USB32DEV_LNK_LTSSM_STATE_LOOPBACK_MASTER_Pos 14UL
#define USB32DEV_LNK_LTSSM_STATE_LOOPBACK_MASTER_Msk 0x4000UL
#define USB32DEV_LNK_LTSSM_STATE_DISABLE_SCRAMBLING_Pos 15UL
#define USB32DEV_LNK_LTSSM_STATE_DISABLE_SCRAMBLING_Msk 0x8000UL
#define USB32DEV_LNK_LTSSM_STATE_LOOPBACK_ERROR_Pos 16UL
#define USB32DEV_LNK_LTSSM_STATE_LOOPBACK_ERROR_Msk 0x10000UL
#define USB32DEV_LNK_LTSSM_STATE_LOOPBACK_GOOD_Pos 17UL
#define USB32DEV_LNK_LTSSM_STATE_LOOPBACK_GOOD_Msk 0x20000UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_FREEZE_Pos 31UL
#define USB32DEV_LNK_LTSSM_STATE_LTSSM_FREEZE_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_LOOPBACK_INIT */
#define USB32DEV_LNK_LOOPBACK_INIT_INIT_Pos 0UL
#define USB32DEV_LNK_LOOPBACK_INIT_INIT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LOOPBACK_GENERATOR */
#define USB32DEV_LNK_LOOPBACK_GENERATOR_GENERATOR_Pos 0UL
#define USB32DEV_LNK_LOOPBACK_GENERATOR_GENERATOR_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_OBSERVE */
#define USB32DEV_LNK_LTSSM_OBSERVE_RX_DETECT_MISS_CNT_Pos 0UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RX_DETECT_MISS_CNT_Msk 0xFUL
#define USB32DEV_LNK_LTSSM_OBSERVE_RX_DETECT_MISS_LIMIT_Pos 4UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RX_DETECT_MISS_LIMIT_Msk 0xF0UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RECOVERY_CNT_Pos 8UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RECOVERY_CNT_Msk 0xF00UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RECOVERY_LIMIT_Pos 12UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RECOVERY_LIMIT_Msk 0xF000UL
#define USB32DEV_LNK_LTSSM_OBSERVE_TS1_RCVD_CNT_Pos 16UL
#define USB32DEV_LNK_LTSSM_OBSERVE_TS1_RCVD_CNT_Msk 0xF0000UL
#define USB32DEV_LNK_LTSSM_OBSERVE_TS2_RCVD_CNT_Pos 20UL
#define USB32DEV_LNK_LTSSM_OBSERVE_TS2_RCVD_CNT_Msk 0xF00000UL
#define USB32DEV_LNK_LTSSM_OBSERVE_IDLE_RCVD_CNT_Pos 24UL
#define USB32DEV_LNK_LTSSM_OBSERVE_IDLE_RCVD_CNT_Msk 0xF000000UL
#define USB32DEV_LNK_LTSSM_OBSERVE_POLLING_LFPS_COMPLETED_Pos 28UL
#define USB32DEV_LNK_LTSSM_OBSERVE_POLLING_LFPS_COMPLETED_Msk 0x10000000UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RX_DISABLE_SCRAMBLING_Pos 29UL
#define USB32DEV_LNK_LTSSM_OBSERVE_RX_DISABLE_SCRAMBLING_Msk 0x20000000UL
#define USB32DEV_LNK_LTSSM_OBSERVE_DATA_RATE_CONFIG_Pos 30UL
#define USB32DEV_LNK_LTSSM_OBSERVE_DATA_RATE_CONFIG_Msk 0xC0000000UL
/* USB32DEV_LNK.LNK_LFPS_OBSERVE */
#define USB32DEV_LNK_LFPS_OBSERVE_POLLING_DET_Pos 0UL
#define USB32DEV_LNK_LFPS_OBSERVE_POLLING_DET_Msk 0x1UL
#define USB32DEV_LNK_LFPS_OBSERVE_PING_DET_Pos 1UL
#define USB32DEV_LNK_LFPS_OBSERVE_PING_DET_Msk 0x2UL
#define USB32DEV_LNK_LFPS_OBSERVE_RESET_DET_Pos 2UL
#define USB32DEV_LNK_LFPS_OBSERVE_RESET_DET_Msk 0x4UL
#define USB32DEV_LNK_LFPS_OBSERVE_U1_EXIT_DET_Pos 3UL
#define USB32DEV_LNK_LFPS_OBSERVE_U1_EXIT_DET_Msk 0x8UL
#define USB32DEV_LNK_LFPS_OBSERVE_U2_EXIT_DET_Pos 4UL
#define USB32DEV_LNK_LFPS_OBSERVE_U2_EXIT_DET_Msk 0x10UL
#define USB32DEV_LNK_LFPS_OBSERVE_U3_EXIT_DET_Pos 5UL
#define USB32DEV_LNK_LFPS_OBSERVE_U3_EXIT_DET_Msk 0x20UL
#define USB32DEV_LNK_LFPS_OBSERVE_LOOPBACK_DET_Pos 6UL
#define USB32DEV_LNK_LFPS_OBSERVE_LOOPBACK_DET_Msk 0x40UL
#define USB32DEV_LNK_LFPS_OBSERVE_SCD1_DET_Pos 7UL
#define USB32DEV_LNK_LFPS_OBSERVE_SCD1_DET_Msk 0x80UL
#define USB32DEV_LNK_LFPS_OBSERVE_SCD2_DET_Pos 8UL
#define USB32DEV_LNK_LFPS_OBSERVE_SCD2_DET_Msk 0x100UL
#define USB32DEV_LNK_LFPS_OBSERVE_PHY_CAP_LBPM_10G_DET_Pos 9UL
#define USB32DEV_LNK_LFPS_OBSERVE_PHY_CAP_LBPM_10G_DET_Msk 0x200UL
#define USB32DEV_LNK_LFPS_OBSERVE_PHY_CAP_LBPM_5G_DET_Pos 10UL
#define USB32DEV_LNK_LFPS_OBSERVE_PHY_CAP_LBPM_5G_DET_Msk 0x400UL
#define USB32DEV_LNK_LFPS_OBSERVE_PHY_READY_LBPM_DET_Pos 11UL
#define USB32DEV_LNK_LFPS_OBSERVE_PHY_READY_LBPM_DET_Msk 0x800UL
#define USB32DEV_LNK_LFPS_OBSERVE_POLLING_LFPS_RCVD_Pos 16UL
#define USB32DEV_LNK_LFPS_OBSERVE_POLLING_LFPS_RCVD_Msk 0x1F0000UL
#define USB32DEV_LNK_LFPS_OBSERVE_POLLING_LFPS_SENT_Pos 21UL
#define USB32DEV_LNK_LFPS_OBSERVE_POLLING_LFPS_SENT_Msk 0x3E00000UL
/* USB32DEV_LNK.LNK_LFPS_TX_POLLING_BURST */
#define USB32DEV_LNK_LFPS_TX_POLLING_BURST_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_POLLING_BURST_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_POLLING_REPEAT */
#define USB32DEV_LNK_LFPS_TX_POLLING_REPEAT_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_POLLING_REPEAT_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_PING_BURST */
#define USB32DEV_LNK_LFPS_TX_PING_BURST_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_PING_BURST_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_PING_REPEAT */
#define USB32DEV_LNK_LFPS_TX_PING_REPEAT_REPEAT32_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_PING_REPEAT_REPEAT32_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_U1_EXIT */
#define USB32DEV_LNK_LFPS_TX_U1_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_U1_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_U2_EXIT */
#define USB32DEV_LNK_LFPS_TX_U2_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_U2_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_U3_EXIT */
#define USB32DEV_LNK_LFPS_TX_U3_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_U3_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_POLLING_BURST */
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_POLLING_REPEAT */
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_PING */
#define USB32DEV_LNK_LFPS_RX_PING_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_PING_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_PING_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_PING_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_RESET */
#define USB32DEV_LNK_LFPS_RX_RESET_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_RESET_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U1_EXIT */
#define USB32DEV_LNK_LFPS_RX_U1_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U1_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U2_EXIT */
#define USB32DEV_LNK_LFPS_RX_U2_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U2_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U3_EXIT */
#define USB32DEV_LNK_LFPS_RX_U3_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U3_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U1_HANDSHAKE */
#define USB32DEV_LNK_LFPS_RX_U1_HANDSHAKE_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U1_HANDSHAKE_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U2_HANDSHAKE */
#define USB32DEV_LNK_LFPS_RX_U2_HANDSHAKE_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U2_HANDSHAKE_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U3_HANDSHAKE */
#define USB32DEV_LNK_LFPS_RX_U3_HANDSHAKE_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U3_HANDSHAKE_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_LOOPBACK_EXIT */
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_EXIT_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_EXIT_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_LOOPBACK_HANDSHAKE */
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_HANDSHAKE_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_HANDSHAKE_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_IDLE */
#define USB32DEV_LNK_LFPS_RX_IDLE_TIME_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_IDLE_TIME_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_BURST */
#define USB32DEV_LNK_LFPS_RX_BURST_TIME_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_BURST_TIME_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_TIME */
#define USB32DEV_LNK_LFPS_RX_TIME_TIME_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_TIME_TIME_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_PENDING_HP_TIMER */
#define USB32DEV_LNK_PENDING_HP_TIMER_TIMER16_Pos 0UL
#define USB32DEV_LNK_PENDING_HP_TIMER_TIMER16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PENDING_HP_TIMEOUT */
#define USB32DEV_LNK_PENDING_HP_TIMEOUT_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PENDING_HP_TIMEOUT_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_CREDIT_HP_TIMER */
#define USB32DEV_LNK_CREDIT_HP_TIMER_TIMER_Pos 0UL
#define USB32DEV_LNK_CREDIT_HP_TIMER_TIMER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_CREDIT_HP_TIMEOUT */
#define USB32DEV_LNK_CREDIT_HP_TIMEOUT_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_CREDIT_HP_TIMEOUT_TIMEOUT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_PM_TIMER */
#define USB32DEV_LNK_PM_TIMER_TIMER_Pos 0UL
#define USB32DEV_LNK_PM_TIMER_TIMER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_PM_LC_TIMEOUT */
#define USB32DEV_LNK_PM_LC_TIMEOUT_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_LC_TIMEOUT_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_ENTRY_TIMEOUT */
#define USB32DEV_LNK_PM_ENTRY_TIMEOUT_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_ENTRY_TIMEOUT_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_UX_EXIT_TIMEOUT */
#define USB32DEV_LNK_PM_UX_EXIT_TIMEOUT_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_PM_UX_EXIT_TIMEOUT_TIMEOUT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_TIMER */
#define USB32DEV_LNK_LTSSM_TIMER_TIMER_Pos 0UL
#define USB32DEV_LNK_LTSSM_TIMER_TIMER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_TIMEOUT */
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_LFPS_Pos 0UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_LFPS_Msk 0x1UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_ACTIVE_Pos 1UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_ACTIVE_Msk 0x2UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_IDLE_Pos 2UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_IDLE_Msk 0x4UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_U1_EXIT_Pos 3UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_U1_EXIT_Msk 0x8UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_U2_EXIT_Pos 4UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_U2_EXIT_Msk 0x10UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_U3_EXIT_Pos 5UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_U3_EXIT_Msk 0x20UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_HOT_RESET_ACTIVE_Pos 6UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_HOT_RESET_ACTIVE_Msk 0x40UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_HOT_RESET_EXIT_Pos 7UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_HOT_RESET_EXIT_Msk 0x80UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_LOOPBACK_EXIT_Pos 8UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_LOOPBACK_EXIT_Msk 0x100UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_RECOVERY_IDLE_Pos 9UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_RECOVERY_IDLE_Msk 0x200UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_RECOVERY_ACTIVE_Pos 10UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_RECOVERY_ACTIVE_Msk 0x400UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_RECOVERY_CONFIG_Pos 11UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_RECOVERY_CONFIG_Msk 0x800UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_SCD_LFPS_Pos 12UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_SCD_LFPS_Msk 0x1000UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_LBPM_LFPS_Pos 13UL
#define USB32DEV_LNK_LTSSM_TIMEOUT_POLLING_LBPM_LFPS_Msk 0x2000UL
/* USB32DEV_LNK.LNK_LTSSM_RX_DETECT_PERIOD */
#define USB32DEV_LNK_LTSSM_RX_DETECT_PERIOD_PERIOD24_Pos 0UL
#define USB32DEV_LNK_LTSSM_RX_DETECT_PERIOD_PERIOD24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_LUP_PERIOD */
#define USB32DEV_LNK_LTSSM_LUP_PERIOD_PERIOD24_Pos 0UL
#define USB32DEV_LNK_LTSSM_LUP_PERIOD_PERIOD24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_SS_INACTIVE_PERIOD */
#define USB32DEV_LNK_LTSSM_SS_INACTIVE_PERIOD_PERIOD24_Pos 0UL
#define USB32DEV_LNK_LTSSM_SS_INACTIVE_PERIOD_PERIOD24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_POLLING_LFPS_TIMEOUT */
#define USB32DEV_LNK_LTSSM_POLLING_LFPS_TIMEOUT_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_LTSSM_POLLING_LFPS_TIMEOUT_TIMEOUT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_POLLING_ACTIVE_TIMEOUT */
#define USB32DEV_LNK_LTSSM_POLLING_ACTIVE_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_POLLING_ACTIVE_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_POLLING_CONFIG_TIMEOUT */
#define USB32DEV_LNK_LTSSM_POLLING_CONFIG_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_POLLING_CONFIG_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_POLLING_IDLE_TIMEOUT */
#define USB32DEV_LNK_LTSSM_POLLING_IDLE_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_POLLING_IDLE_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_U1_EXIT_TIMEOUT */
#define USB32DEV_LNK_LTSSM_U1_EXIT_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_U1_EXIT_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_U2_EXIT_TIMEOUT */
#define USB32DEV_LNK_LTSSM_U2_EXIT_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_U2_EXIT_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_U3_EXIT_TIMEOUT */
#define USB32DEV_LNK_LTSSM_U3_EXIT_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_U3_EXIT_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_HOT_RESET_ACTIVE_TIMEOUT */
#define USB32DEV_LNK_LTSSM_HOT_RESET_ACTIVE_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_HOT_RESET_ACTIVE_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_HOT_RESET_EXIT_TIMEOUT */
#define USB32DEV_LNK_LTSSM_HOT_RESET_EXIT_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_HOT_RESET_EXIT_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_LOOPBACK_EXIT_TIMEOUT */
#define USB32DEV_LNK_LTSSM_LOOPBACK_EXIT_TIMEOUT_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_LTSSM_LOOPBACK_EXIT_TIMEOUT_TIMEOUT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_RECOVERY_IDLE_TIMEOUT */
#define USB32DEV_LNK_LTSSM_RECOVERY_IDLE_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_RECOVERY_IDLE_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_RECOVERY_ACTIVE_TIMEOUT */
#define USB32DEV_LNK_LTSSM_RECOVERY_ACTIVE_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_RECOVERY_ACTIVE_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_RECOVERY_CONFIG_TIMEOUT */
#define USB32DEV_LNK_LTSSM_RECOVERY_CONFIG_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_RECOVERY_CONFIG_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_LDN_TIMEOUT */
#define USB32DEV_LNK_LTSSM_LDN_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_LDN_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_LDN_TIMER */
#define USB32DEV_LNK_LTSSM_LDN_TIMER_TIMER24_Pos 0UL
#define USB32DEV_LNK_LTSSM_LDN_TIMER_TIMER24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_0 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_0_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_1 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_1_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_2 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_2_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_3 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_3_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_4 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_4_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_5 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_5_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_6 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_6_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_7 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_7_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_8 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_CP_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_CP_Msk 0xFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_K_D_Pos 8UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_K_D_Msk 0x100UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_SCRAMBLED_Pos 9UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_SCRAMBLED_Msk 0x200UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_DEEMPHASIS_Pos 10UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_DEEMPHASIS_Msk 0x400UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_LFPS_Pos 11UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_LFPS_Msk 0x800UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_TXONESZEROS_Pos 12UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_8_TXONESZEROS_Msk 0x1000UL
/* USB32DEV_LNK.LNK_DEBUG_BUFFER_CTRL */
#define USB32DEV_LNK_DEBUG_BUFFER_CTRL_MEM_PTR_Pos 0UL
#define USB32DEV_LNK_DEBUG_BUFFER_CTRL_MEM_PTR_Msk 0x7UL
/* USB32DEV_LNK.LNK_DATARATE_CHG_OBSERVE */
#define USB32DEV_LNK_DATARATE_CHG_OBSERVE_DATA_RATE_CHG_DET_Pos 0UL
#define USB32DEV_LNK_DATARATE_CHG_OBSERVE_DATA_RATE_CHG_DET_Msk 0x1FUL
/* USB32DEV_LNK.LNK_LFPS_TX_POLLING_BURST_GEN2 */
#define USB32DEV_LNK_LFPS_TX_POLLING_BURST_GEN2_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_POLLING_BURST_GEN2_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_POLLING_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_POLLING_REPEAT_GEN2_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_POLLING_REPEAT_GEN2_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_PING_BURST_GEN2 */
#define USB32DEV_LNK_LFPS_TX_PING_BURST_GEN2_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_PING_BURST_GEN2_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_PING_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_PING_REPEAT_GEN2_REPEAT32_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_PING_REPEAT_GEN2_REPEAT32_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_U1_EXIT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_U1_EXIT_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_U1_EXIT_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_U2_EXIT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_U2_EXIT_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_U2_EXIT_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_U3_EXIT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_U3_EXIT_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_U3_EXIT_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_POLLING_BURST_GEN2 */
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_GEN2_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_GEN2_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_GEN2_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_POLLING_BURST_GEN2_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_POLLING_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_GEN2_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_GEN2_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_GEN2_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_POLLING_REPEAT_GEN2_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_PING_GEN2 */
#define USB32DEV_LNK_LFPS_RX_PING_GEN2_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_PING_GEN2_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_PING_GEN2_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_PING_GEN2_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_RESET_GEN2 */
#define USB32DEV_LNK_LFPS_RX_RESET_GEN2_BURST32_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_RESET_GEN2_BURST32_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U1_EXIT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_U1_EXIT_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U1_EXIT_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U2_EXIT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_U2_EXIT_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U2_EXIT_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U1_HANDSHAKE_GEN2 */
#define USB32DEV_LNK_LFPS_RX_U1_HANDSHAKE_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U1_HANDSHAKE_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U2_HANDSHAKE_GEN2 */
#define USB32DEV_LNK_LFPS_RX_U2_HANDSHAKE_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U2_HANDSHAKE_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_U3_HANDSHAKE_GEN2 */
#define USB32DEV_LNK_LFPS_RX_U3_HANDSHAKE_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_U3_HANDSHAKE_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_LOOPBACK_EXIT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_EXIT_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_EXIT_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_LOOPBACK_HANDSHAKE_GEN2 */
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_HANDSHAKE_GEN2_BURST24_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LOOPBACK_HANDSHAKE_GEN2_BURST24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_PENDING_HP_TIMEOUT_GEN2 */
#define USB32DEV_LNK_PENDING_HP_TIMEOUT_GEN2_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PENDING_HP_TIMEOUT_GEN2_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_CREDIT_HP_TIMEOUT_GEN2 */
#define USB32DEV_LNK_CREDIT_HP_TIMEOUT_GEN2_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_CREDIT_HP_TIMEOUT_GEN2_TIMEOUT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_PM_LC_X2_TIMEOUT_GEN1 */
#define USB32DEV_LNK_PM_LC_X2_TIMEOUT_GEN1_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_LC_X2_TIMEOUT_GEN1_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_LC_X1_TIMEOUT_GEN2 */
#define USB32DEV_LNK_PM_LC_X1_TIMEOUT_GEN2_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_LC_X1_TIMEOUT_GEN2_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_LC_X2_TIMEOUT_GEN2 */
#define USB32DEV_LNK_PM_LC_X2_TIMEOUT_GEN2_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_LC_X2_TIMEOUT_GEN2_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_ENTRY_X2_TIMEOUT_GEN1 */
#define USB32DEV_LNK_PM_ENTRY_X2_TIMEOUT_GEN1_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_ENTRY_X2_TIMEOUT_GEN1_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_ENTRY_X1_TIMEOUT_GEN2 */
#define USB32DEV_LNK_PM_ENTRY_X1_TIMEOUT_GEN2_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_ENTRY_X1_TIMEOUT_GEN2_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_ENTRY_X2_TIMEOUT_GEN2 */
#define USB32DEV_LNK_PM_ENTRY_X2_TIMEOUT_GEN2_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_PM_ENTRY_X2_TIMEOUT_GEN2_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_PM_UX_EXIT_TIMEOUT_GEN2 */
#define USB32DEV_LNK_PM_UX_EXIT_TIMEOUT_GEN2_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_PM_UX_EXIT_TIMEOUT_GEN2_TIMEOUT_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN1 */
#define USB32DEV_LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN1_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN1_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN2 */
#define USB32DEV_LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN2_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_U1_MIN_RESIDENCY_TIMEOUT_GEN2_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN1 */
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN1_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN1_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN1 */
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN1_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN1_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_SCD_END_REPEAT_GEN1 */
#define USB32DEV_LNK_LFPS_TX_SCD_END_REPEAT_GEN1_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_SCD_END_REPEAT_GEN1_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN1 */
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN1_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN1_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN1_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN1_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN1 */
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN1_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN1_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN1_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN1_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_SCD_END_REPEAT_GEN1 */
#define USB32DEV_LNK_LFPS_RX_SCD_END_REPEAT_GEN1_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_SCD_END_REPEAT_GEN1_REPEAT_MIN_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN2_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC0_REPEAT_GEN2_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN2_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_SCD_LOGIC1_REPEAT_GEN2_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_SCD_END_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_TX_SCD_END_REPEAT_GEN2_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_SCD_END_REPEAT_GEN2_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN2_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN2_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN2_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC0_REPEAT_GEN2_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN2_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN2_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN2_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_SCD_LOGIC1_REPEAT_GEN2_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_SCD_END_REPEAT_GEN2 */
#define USB32DEV_LNK_LFPS_RX_SCD_END_REPEAT_GEN2_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_SCD_END_REPEAT_GEN2_REPEAT_MIN_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_LBPS_TPWM_GEN1 */
#define USB32DEV_LNK_LFPS_TX_LBPS_TPWM_GEN1_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_LBPS_TPWM_GEN1_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_LBPS_TLFPS0_GEN1 */
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS0_GEN1_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS0_GEN1_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_LBPS_TLFPS1_GEN1 */
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS1_GEN1_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS1_GEN1_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_LBPS_TPWM_GEN1 */
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN1_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN1_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN1_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN1_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_LBPS_TLFPS0_GEN1 */
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN1_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN1_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN1_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN1_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_LBPS_TLFPS1_GEN1 */
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN1_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN1_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN1_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN1_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_TX_LBPS_TPWM_GEN2 */
#define USB32DEV_LNK_LFPS_TX_LBPS_TPWM_GEN2_REPEAT16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_LBPS_TPWM_GEN2_REPEAT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_LBPS_TLFPS0_GEN2 */
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS0_GEN2_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS0_GEN2_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_TX_LBPS_TLFPS1_GEN2 */
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS1_GEN2_BURST16_Pos 0UL
#define USB32DEV_LNK_LFPS_TX_LBPS_TLFPS1_GEN2_BURST16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LFPS_RX_LBPS_TPWM_GEN2 */
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN2_REPEAT_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN2_REPEAT_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN2_REPEAT_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TPWM_GEN2_REPEAT_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_LBPS_TLFPS0_GEN2 */
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN2_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN2_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN2_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS0_GEN2_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_RX_LBPS_TLFPS1_GEN2 */
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN2_BURST_MIN_Pos 0UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN2_BURST_MIN_Msk 0xFFFFUL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN2_BURST_MAX_Pos 16UL
#define USB32DEV_LNK_LFPS_RX_LBPS_TLFPS1_GEN2_BURST_MAX_Msk 0xFFFF0000UL
/* USB32DEV_LNK.LNK_LFPS_SCD_PATTERN */
#define USB32DEV_LNK_LFPS_SCD_PATTERN_SCD1_PATTERN_Pos 0UL
#define USB32DEV_LNK_LFPS_SCD_PATTERN_SCD1_PATTERN_Msk 0xFUL
#define USB32DEV_LNK_LFPS_SCD_PATTERN_SCD2_PATTERN_Pos 4UL
#define USB32DEV_LNK_LFPS_SCD_PATTERN_SCD2_PATTERN_Msk 0xF0UL
/* USB32DEV_LNK.LNK_TSEQ_COUNT_GEN1 */
#define USB32DEV_LNK_TSEQ_COUNT_GEN1_COUNT24_Pos 0UL
#define USB32DEV_LNK_TSEQ_COUNT_GEN1_COUNT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_TSEQ_COUNT_GEN2 */
#define USB32DEV_LNK_TSEQ_COUNT_GEN2_COUNT24_Pos 0UL
#define USB32DEV_LNK_TSEQ_COUNT_GEN2_COUNT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_SCD1_GEN2_HSK */
#define USB32DEV_LNK_SCD1_GEN2_HSK_RX_SCDX_CNT_Pos 0UL
#define USB32DEV_LNK_SCD1_GEN2_HSK_RX_SCDX_CNT_Msk 0x7UL
#define USB32DEV_LNK_SCD1_GEN2_HSK_ARX_SCDX_LIMIT_Pos 8UL
#define USB32DEV_LNK_SCD1_GEN2_HSK_ARX_SCDX_LIMIT_Msk 0x300UL
#define USB32DEV_LNK_SCD1_GEN2_HSK_TX_ARX_SCD1_CNT_Pos 12UL
#define USB32DEV_LNK_SCD1_GEN2_HSK_TX_ARX_SCD1_CNT_Msk 0x7000UL
/* USB32DEV_LNK.LNK_SCD1_GEN2_TO_GEN1_HSK */
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_SCDXCHK_RX_LFPS_CNT_Pos 0UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_SCDXCHK_RX_LFPS_CNT_Msk 0x7FUL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_TX_SCD1_CNT_Pos 8UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_TX_SCD1_CNT_Msk 0xF00UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_RX_LFPS_CNT_Pos 12UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_RX_LFPS_CNT_Msk 0x7000UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_ARX_LFPS_LIMIT_Pos 16UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_ARX_LFPS_LIMIT_Msk 0x70000UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_TX_ARX_SCD1_CNT_Pos 20UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_TX_ARX_SCD1_CNT_Msk 0x700000UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_TX_LFPS_CNT_Pos 24UL
#define USB32DEV_LNK_SCD1_GEN2_TO_GEN1_HSK_G2G1_TX_LFPS_CNT_Msk 0x1F000000UL
/* USB32DEV_LNK.LNK_SCD1_OBSERVE */
#define USB32DEV_LNK_SCD1_OBSERVE_RCVD_SCD2_CNT_Pos 0UL
#define USB32DEV_LNK_SCD1_OBSERVE_RCVD_SCD2_CNT_Msk 0x7UL
#define USB32DEV_LNK_SCD1_OBSERVE_RCVD_SCD1_CNT_Pos 4UL
#define USB32DEV_LNK_SCD1_OBSERVE_RCVD_SCD1_CNT_Msk 0x70UL
#define USB32DEV_LNK_SCD1_OBSERVE_SENT_SCD1_CNT_G2_ARX_Pos 12UL
#define USB32DEV_LNK_SCD1_OBSERVE_SENT_SCD1_CNT_G2_ARX_Msk 0x7000UL
#define USB32DEV_LNK_SCD1_OBSERVE_SENT_SCD1_CNT_G2G1_ARX_Pos 20UL
#define USB32DEV_LNK_SCD1_OBSERVE_SENT_SCD1_CNT_G2G1_ARX_Msk 0x700000UL
#define USB32DEV_LNK_SCD1_OBSERVE_G2G1_LFPS_HSK_DONE_Pos 30UL
#define USB32DEV_LNK_SCD1_OBSERVE_G2G1_LFPS_HSK_DONE_Msk 0x40000000UL
#define USB32DEV_LNK_SCD1_OBSERVE_SCD1_HSK_DONE_Pos 31UL
#define USB32DEV_LNK_SCD1_OBSERVE_SCD1_HSK_DONE_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_SCD2_GEN2_HSK */
#define USB32DEV_LNK_SCD2_GEN2_HSK_RX_SCD2_CNT_Pos 0UL
#define USB32DEV_LNK_SCD2_GEN2_HSK_RX_SCD2_CNT_Msk 0x7UL
#define USB32DEV_LNK_SCD2_GEN2_HSK_ARX_SCD2_LIMIT_Pos 8UL
#define USB32DEV_LNK_SCD2_GEN2_HSK_ARX_SCD2_LIMIT_Msk 0x300UL
#define USB32DEV_LNK_SCD2_GEN2_HSK_TX_ARX_SCD2_CNT_Pos 12UL
#define USB32DEV_LNK_SCD2_GEN2_HSK_TX_ARX_SCD2_CNT_Msk 0x7000UL
/* USB32DEV_LNK.LNK_SCD2_GEN2_TO_GEN1_HSK */
#define USB32DEV_LNK_SCD2_GEN2_TO_GEN1_HSK_SCD2CHK_RX_LFPS_CNT_Pos 0UL
#define USB32DEV_LNK_SCD2_GEN2_TO_GEN1_HSK_SCD2CHK_RX_LFPS_CNT_Msk 0x7FUL
#define USB32DEV_LNK_SCD2_GEN2_TO_GEN1_HSK_G2G1_TX_LFPS_CNT_Pos 24UL
#define USB32DEV_LNK_SCD2_GEN2_TO_GEN1_HSK_G2G1_TX_LFPS_CNT_Msk 0x1F000000UL
/* USB32DEV_LNK.LNK_SCD2_OBSERVE */
#define USB32DEV_LNK_SCD2_OBSERVE_RCVD_SCD2_CNT_Pos 0UL
#define USB32DEV_LNK_SCD2_OBSERVE_RCVD_SCD2_CNT_Msk 0x7UL
#define USB32DEV_LNK_SCD2_OBSERVE_SENT_SCD2_CNT_G2_ARX_Pos 8UL
#define USB32DEV_LNK_SCD2_OBSERVE_SENT_SCD2_CNT_G2_ARX_Msk 0x700UL
#define USB32DEV_LNK_SCD2_OBSERVE_G2G1_LFPS_HSK_DONE_Pos 30UL
#define USB32DEV_LNK_SCD2_OBSERVE_G2G1_LFPS_HSK_DONE_Msk 0x40000000UL
#define USB32DEV_LNK_SCD2_OBSERVE_SCD2_HSK_DONE_Pos 31UL
#define USB32DEV_LNK_SCD2_OBSERVE_SCD2_HSK_DONE_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_CAP_LBPM_HSK */
#define USB32DEV_LNK_CAP_LBPM_HSK_RX_CAP_LBPM_CNT_Pos 4UL
#define USB32DEV_LNK_CAP_LBPM_HSK_RX_CAP_LBPM_CNT_Msk 0x70UL
#define USB32DEV_LNK_CAP_LBPM_HSK_ARX_LBPM_LIMIT_Pos 8UL
#define USB32DEV_LNK_CAP_LBPM_HSK_ARX_LBPM_LIMIT_Msk 0x700UL
#define USB32DEV_LNK_CAP_LBPM_HSK_TX_ARX_CAP_LBPM_CNT_Pos 12UL
#define USB32DEV_LNK_CAP_LBPM_HSK_TX_ARX_CAP_LBPM_CNT_Msk 0xF000UL
/* USB32DEV_LNK.LNK_CAP_LBPM_OBSERVE */
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_10GX1_CAP_LBPM_CNT_Pos 0UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_10GX1_CAP_LBPM_CNT_Msk 0x7UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_5GX1_CAP_LBPM_CNT_Pos 4UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_5GX1_CAP_LBPM_CNT_Msk 0x70UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_10GX2_CAP_LBPM_CNT_Pos 8UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_10GX2_CAP_LBPM_CNT_Msk 0x700UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_5GX2_CAP_LBPM_CNT_Pos 12UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_5GX2_CAP_LBPM_CNT_Msk 0x7000UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_READY_LBPM_CNT_Pos 16UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_RCVD_READY_LBPM_CNT_Msk 0x70000UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_SENT_CAP_LBPM_CNT_ARX_Pos 20UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_SENT_CAP_LBPM_CNT_ARX_Msk 0x700000UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_CAP_LBPM_HSK_DONE_Pos 31UL
#define USB32DEV_LNK_CAP_LBPM_OBSERVE_CAP_LBPM_HSK_DONE_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_READY_LBPM_HSK */
#define USB32DEV_LNK_READY_LBPM_HSK_RX_RDY_LBPM_CNT_Pos 0UL
#define USB32DEV_LNK_READY_LBPM_HSK_RX_RDY_LBPM_CNT_Msk 0x7UL
#define USB32DEV_LNK_READY_LBPM_HSK_ARX_RDY_LBPM_LIMIT_Pos 4UL
#define USB32DEV_LNK_READY_LBPM_HSK_ARX_RDY_LBPM_LIMIT_Msk 0x70UL
#define USB32DEV_LNK_READY_LBPM_HSK_TX_ARX_RDY_LBPM_CNT_Pos 8UL
#define USB32DEV_LNK_READY_LBPM_HSK_TX_ARX_RDY_LBPM_CNT_Msk 0xF00UL
#define USB32DEV_LNK_READY_LBPM_HSK_ENTRY_TO_TX_DELAY_Pos 16UL
#define USB32DEV_LNK_READY_LBPM_HSK_ENTRY_TO_TX_DELAY_Msk 0xFF0000UL
/* USB32DEV_LNK.LNK_READY_LBPM_OBSERVE */
#define USB32DEV_LNK_READY_LBPM_OBSERVE_RCVD_RDY_LBPM_CNT_Pos 0UL
#define USB32DEV_LNK_READY_LBPM_OBSERVE_RCVD_RDY_LBPM_CNT_Msk 0x7UL
#define USB32DEV_LNK_READY_LBPM_OBSERVE_SENT_RDY_LBPM_CNT_ARX_Pos 4UL
#define USB32DEV_LNK_READY_LBPM_OBSERVE_SENT_RDY_LBPM_CNT_ARX_Msk 0xF0UL
#define USB32DEV_LNK_READY_LBPM_OBSERVE_RDY_LBPM_HSK_DONE_Pos 31UL
#define USB32DEV_LNK_READY_LBPM_OBSERVE_RDY_LBPM_HSK_DONE_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_RCVD_LBPM_OBSERVE */
#define USB32DEV_LNK_RCVD_LBPM_OBSERVE_LBPM_Pos 0UL
#define USB32DEV_LNK_RCVD_LBPM_OBSERVE_LBPM_Msk 0xFFUL
/* USB32DEV_LNK.LNK_LTSSM_SCD_LFPS_TIMEOUT */
#define USB32DEV_LNK_LTSSM_SCD_LFPS_TIMEOUT_TIMEOUT16_Pos 0UL
#define USB32DEV_LNK_LTSSM_SCD_LFPS_TIMEOUT_TIMEOUT16_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_LBPM_LFPS_TIMEOUT */
#define USB32DEV_LNK_LTSSM_LBPM_LFPS_TIMEOUT_TIMEOUT24_Pos 0UL
#define USB32DEV_LNK_LTSSM_LBPM_LFPS_TIMEOUT_TIMEOUT24_Msk 0xFFFFFFUL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_9_TO_12 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_9_TO_12_DEEMPH_COEFF_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_9_TO_12_DEEMPH_COEFF_Msk 0x3FFFFUL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_13 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_13_DEEMPH_COEFF_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_13_DEEMPH_COEFF_Msk 0x3FFFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_13_GEN2_TXONESZEROS_Pos 18UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_13_GEN2_TXONESZEROS_Msk 0x40000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_14 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_14_DEEMPH_COEFF_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_14_DEEMPH_COEFF_Msk 0x3FFFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_14_GEN2_TXONESZEROS_Pos 18UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_14_GEN2_TXONESZEROS_Msk 0x40000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_15 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_15_DEEMPH_COEFF_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_15_DEEMPH_COEFF_Msk 0x3FFFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_15_GEN2_TXONESZEROS_Pos 18UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_15_GEN2_TXONESZEROS_Msk 0x40000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_16 */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_16_DEEMPH_COEFF_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_16_DEEMPH_COEFF_Msk 0x3FFFFUL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_16_GEN2_TXONESZEROS_Pos 18UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_16_GEN2_TXONESZEROS_Msk 0x40000UL
/* USB32DEV_LNK.LNK_COMPLIANCE_PATTERN_OBSERVE */
#define USB32DEV_LNK_COMPLIANCE_PATTERN_OBSERVE_CP_OBSERVE_Pos 0UL
#define USB32DEV_LNK_COMPLIANCE_PATTERN_OBSERVE_CP_OBSERVE_Msk 0x3FUL
/* USB32DEV_LNK.LNK_SOFT_ERR_CNT_CONF */
#define USB32DEV_LNK_SOFT_ERR_CNT_CONF_SOFT_ERR_LFR_OPT_Pos 29UL
#define USB32DEV_LNK_SOFT_ERR_CNT_CONF_SOFT_ERR_LFR_OPT_Msk 0x20000000UL
#define USB32DEV_LNK_SOFT_ERR_CNT_CONF_SOFT_ERR_CNT_RESET_Pos 30UL
#define USB32DEV_LNK_SOFT_ERR_CNT_CONF_SOFT_ERR_CNT_RESET_Msk 0x40000000UL
#define USB32DEV_LNK_SOFT_ERR_CNT_CONF_SOFT_ERR_CNT_EN_Pos 31UL
#define USB32DEV_LNK_SOFT_ERR_CNT_CONF_SOFT_ERR_CNT_EN_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_SOFT_ERR_CNT_OBSERVE */
#define USB32DEV_LNK_SOFT_ERR_CNT_OBSERVE_SOFT_ERR_CNT_OBSERVE_Pos 0UL
#define USB32DEV_LNK_SOFT_ERR_CNT_OBSERVE_SOFT_ERR_CNT_OBSERVE_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_LTSSM_STATE_CHG_INTR_CONF */
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_SS_DISABLE_INTR_Pos 0UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_SS_DISABLE_INTR_Msk 0x1UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RX_DETECT_RESET_INTR_Pos 1UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RX_DETECT_RESET_INTR_Msk 0x2UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RX_DETECT_ACTIVE_INTR_Pos 2UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RX_DETECT_ACTIVE_INTR_Msk 0x4UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RX_DETECT_QUIET_INTR_Pos 3UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RX_DETECT_QUIET_INTR_Msk 0x8UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_SS_INACTIVE_QUIET_INTR_Pos 4UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_SS_INACTIVE_QUIET_INTR_Msk 0x10UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_SS_INACTIVE_DISCONNECT_DETECT_INTR_Pos 5UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_SS_INACTIVE_DISCONNECT_DETECT_INTR_Msk 0x20UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_HOTRESET_ACTIVE_INTR_Pos 6UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_HOTRESET_ACTIVE_INTR_Msk 0x40UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_HOTRESET_EXIT_INTR_Pos 7UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_HOTRESET_EXIT_INTR_Msk 0x80UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_LFPS_INTR_Pos 8UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_LFPS_INTR_Msk 0x100UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_RXEQ_INTR_Pos 9UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_RXEQ_INTR_Msk 0x200UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_ACTIVE_INTR_Pos 10UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_ACTIVE_INTR_Msk 0x400UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_CONFIGURATION_INTR_Pos 11UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_CONFIGURATION_INTR_Msk 0x800UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_IDLE_INTR_Pos 12UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_IDLE_INTR_Msk 0x1000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_LFPSPLUS_INTR_Pos 13UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_LFPSPLUS_INTR_Msk 0x2000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_PORT_MATCH_INTR_Pos 14UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_PORT_MATCH_INTR_Msk 0x4000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_PORT_CONFIG_INTR_Pos 15UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_POLLING_PORT_CONFIG_INTR_Msk 0x8000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U0_INTR_Pos 16UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U0_INTR_Msk 0x10000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U1_INTR_Pos 17UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U1_INTR_Msk 0x20000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U2_INTR_Pos 18UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U2_INTR_Msk 0x40000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U3_INTR_Pos 19UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_U3_INTR_Msk 0x80000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_LOOPBACK_ACTIVE_INTR_Pos 20UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_LOOPBACK_ACTIVE_INTR_Msk 0x100000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_LOOPBACK_EXIT_INTR_Pos 21UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_LOOPBACK_EXIT_INTR_Msk 0x200000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_COMPLIANCE_INTR_Pos 22UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_COMPLIANCE_INTR_Msk 0x400000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RECOVERY_ACTIVE_INTR_Pos 23UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RECOVERY_ACTIVE_INTR_Msk 0x800000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RECOVERY_CONFIGURATION_INTR_Pos 24UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RECOVERY_CONFIGURATION_INTR_Msk 0x1000000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RECOVERY_IDLE_INTR_Pos 25UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_RECOVERY_IDLE_INTR_Msk 0x2000000UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_ILLEGAL_STATE_INTR_Pos 26UL
#define USB32DEV_LNK_LTSSM_STATE_CHG_INTR_CONF_ILLEGAL_STATE_INTR_Msk 0x4000000UL
/* USB32DEV_LNK.LNK_DEBUG_RSVD */
#define USB32DEV_LNK_DEBUG_RSVD_DEBUG_SIG_SEL_Pos 0UL
#define USB32DEV_LNK_DEBUG_RSVD_DEBUG_SIG_SEL_Msk 0xFFFFUL
#define USB32DEV_LNK_DEBUG_RSVD_HS_DDFT_SEL_Pos 16UL
#define USB32DEV_LNK_DEBUG_RSVD_HS_DDFT_SEL_Msk 0x1F0000UL
#define USB32DEV_LNK_DEBUG_RSVD_HS_DDFT_NOT_EXTD_Pos 21UL
#define USB32DEV_LNK_DEBUG_RSVD_HS_DDFT_NOT_EXTD_Msk 0x200000UL
#define USB32DEV_LNK_DEBUG_RSVD_RSVD_Pos 22UL
#define USB32DEV_LNK_DEBUG_RSVD_RSVD_Msk 0x7FC00000UL
#define USB32DEV_LNK_DEBUG_RSVD_FORCE_RATE_CONFIG_TO_GEN1_Pos 31UL
#define USB32DEV_LNK_DEBUG_RSVD_FORCE_RATE_CONFIG_TO_GEN1_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_TX_TS1_HOLDOFF_TIMEOUT */
#define USB32DEV_LNK_TX_TS1_HOLDOFF_TIMEOUT_TX_TS1_HOLDOFF_TIMEOUT_Pos 0UL
#define USB32DEV_LNK_TX_TS1_HOLDOFF_TIMEOUT_TX_TS1_HOLDOFF_TIMEOUT_Msk 0xFFFFUL
/* USB32DEV_LNK.LNK_MISC_CONF */
#define USB32DEV_LNK_MISC_CONF_REPLACED_SKP_SYMB_Pos 0UL
#define USB32DEV_LNK_MISC_CONF_REPLACED_SKP_SYMB_Msk 0xFFUL
#define USB32DEV_LNK_MISC_CONF_TX_TS2_CONF_Pos 8UL
#define USB32DEV_LNK_MISC_CONF_TX_TS2_CONF_Msk 0x300UL
#define USB32DEV_LNK_MISC_CONF_INVALID_LFPS_NOT_RST_TIMER_EN_Pos 10UL
#define USB32DEV_LNK_MISC_CONF_INVALID_LFPS_NOT_RST_TIMER_EN_Msk 0x400UL
#define USB32DEV_LNK_MISC_CONF_GEN1X2_SKP_REMOVAL_EN_Pos 11UL
#define USB32DEV_LNK_MISC_CONF_GEN1X2_SKP_REMOVAL_EN_Msk 0x800UL
#define USB32DEV_LNK_MISC_CONF_TX_DL_SET_ENABLE_Pos 12UL
#define USB32DEV_LNK_MISC_CONF_TX_DL_SET_ENABLE_Msk 0x1000UL
#define USB32DEV_LNK_MISC_CONF_GEN1_GT_4CLK_SKEW_DET_EN_Pos 13UL
#define USB32DEV_LNK_MISC_CONF_GEN1_GT_4CLK_SKEW_DET_EN_Msk 0x2000UL
#define USB32DEV_LNK_MISC_CONF_GEN2_GT_4CLK_SKEW_DET_EN_Pos 14UL
#define USB32DEV_LNK_MISC_CONF_GEN2_GT_4CLK_SKEW_DET_EN_Msk 0x4000UL
#define USB32DEV_LNK_MISC_CONF_GEN1_SKEW_EVAL_EN_Pos 15UL
#define USB32DEV_LNK_MISC_CONF_GEN1_SKEW_EVAL_EN_Msk 0x8000UL
#define USB32DEV_LNK_MISC_CONF_GEN2_SKEW_EVAL_EN_Pos 16UL
#define USB32DEV_LNK_MISC_CONF_GEN2_SKEW_EVAL_EN_Msk 0x10000UL
#define USB32DEV_LNK_MISC_CONF_GEN2_SKP_REMOVAL_OPT_Pos 17UL
#define USB32DEV_LNK_MISC_CONF_GEN2_SKP_REMOVAL_OPT_Msk 0x20000UL
#define USB32DEV_LNK_MISC_CONF_GEN2_ALGN_FSM_RST_OPT_Pos 18UL
#define USB32DEV_LNK_MISC_CONF_GEN2_ALGN_FSM_RST_OPT_Msk 0x40000UL
#define USB32DEV_LNK_MISC_CONF_PENDING_LXU_BEFORE_LGOOD_BUG_FIX_EN_Pos 19UL
#define USB32DEV_LNK_MISC_CONF_PENDING_LXU_BEFORE_LGOOD_BUG_FIX_EN_Msk 0x80000UL
#define USB32DEV_LNK_MISC_CONF_RX_DESKEW_CLK_GT_EN_Pos 20UL
#define USB32DEV_LNK_MISC_CONF_RX_DESKEW_CLK_GT_EN_Msk 0x100000UL
#define USB32DEV_LNK_MISC_CONF_GEN2X2_DESKEW_ON_TS1_Pos 21UL
#define USB32DEV_LNK_MISC_CONF_GEN2X2_DESKEW_ON_TS1_Msk 0x200000UL
#define USB32DEV_LNK_MISC_CONF_TX_IDLE_CONF_Pos 22UL
#define USB32DEV_LNK_MISC_CONF_TX_IDLE_CONF_Msk 0xC00000UL
#define USB32DEV_LNK_MISC_CONF_RX_DESKEW_RST_IF_RECOV_Pos 30UL
#define USB32DEV_LNK_MISC_CONF_RX_DESKEW_RST_IF_RECOV_Msk 0x40000000UL
#define USB32DEV_LNK_MISC_CONF_RX_DESKEW_EN_Pos 31UL
#define USB32DEV_LNK_MISC_CONF_RX_DESKEW_EN_Msk 0x80000000UL
/* USB32DEV_LNK.LNK_MISC_OBSERVE */
#define USB32DEV_LNK_MISC_OBSERVE_LOCKED_SKEW_VAL_Pos 0UL
#define USB32DEV_LNK_MISC_OBSERVE_LOCKED_SKEW_VAL_Msk 0x1FUL
/* USB32DEV_LNK.LNK_LTSSM_SUBSTATE_OBSERVE */
#define USB32DEV_LNK_LTSSM_SUBSTATE_OBSERVE_LTSSM_SUBSTATE_OBSERVE_Pos 0UL
#define USB32DEV_LNK_LTSSM_SUBSTATE_OBSERVE_LTSSM_SUBSTATE_OBSERVE_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_RX_TYPE1_HEADER_BUFFER */
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_HEADER_Pos 0UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_HEADER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_RX_TYPE1_HEADER_BUFFER_STATE_0 */
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE0_Pos 0UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE0_Msk 0xFUL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID0_Pos 4UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID0_Msk 0x10UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD0_Pos 5UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD0_Msk 0x20UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE1_Pos 8UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE1_Msk 0xF00UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID1_Pos 12UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID1_Msk 0x1000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD1_Pos 13UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD1_Msk 0x2000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE2_Pos 16UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE2_Msk 0xF0000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID2_Pos 20UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID2_Msk 0x100000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD2_Pos 21UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD2_Msk 0x200000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE3_Pos 24UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE3_Msk 0xF000000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID3_Pos 28UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_VALID3_Msk 0x10000000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD3_Pos 29UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_0_RCVD3_Msk 0x20000000UL
/* USB32DEV_LNK.LNK_RX_TYPE1_HEADER_BUFFER_STATE_1 */
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE4_Pos 0UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE4_Msk 0xFUL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_VALID4_Pos 4UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_VALID4_Msk 0x10UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_RCVD4_Pos 5UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_RCVD4_Msk 0x20UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE5_Pos 8UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE5_Msk 0xF00UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_VALID5_Pos 12UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_VALID5_Msk 0x1000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_RCVD5_Pos 13UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_RCVD5_Msk 0x2000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE6_Pos 16UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE6_Msk 0xF0000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_VALID6_Pos 20UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_VALID6_Msk 0x100000UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_RCVD6_Pos 21UL
#define USB32DEV_LNK_RX_TYPE1_HEADER_BUFFER_STATE_1_RCVD6_Msk 0x200000UL
/* USB32DEV_LNK.LNK_RX_TYPE2_HEADER_BUFFER */
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_HEADER_Pos 0UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_HEADER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_RX_TYPE2_HEADER_BUFFER_STATE_0 */
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE0_Pos 0UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE0_Msk 0xFUL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID0_Pos 4UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID0_Msk 0x10UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD0_Pos 5UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD0_Msk 0x20UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE1_Pos 8UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE1_Msk 0xF00UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID1_Pos 12UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID1_Msk 0x1000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD1_Pos 13UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD1_Msk 0x2000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE2_Pos 16UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE2_Msk 0xF0000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID2_Pos 20UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID2_Msk 0x100000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD2_Pos 21UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD2_Msk 0x200000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE3_Pos 24UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE3_Msk 0xF000000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID3_Pos 28UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_VALID3_Msk 0x10000000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD3_Pos 29UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_0_RCVD3_Msk 0x20000000UL
/* USB32DEV_LNK.LNK_RX_TYPE2_HEADER_BUFFER_STATE_1 */
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE4_Pos 0UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE4_Msk 0xFUL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_VALID4_Pos 4UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_VALID4_Msk 0x10UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_RCVD4_Pos 5UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_RCVD4_Msk 0x20UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE5_Pos 8UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE5_Msk 0xF00UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_VALID5_Pos 12UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_VALID5_Msk 0x1000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_RCVD5_Pos 13UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_RCVD5_Msk 0x2000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE6_Pos 16UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE6_Msk 0xF0000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_VALID6_Pos 20UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_VALID6_Msk 0x100000UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_RCVD6_Pos 21UL
#define USB32DEV_LNK_RX_TYPE2_HEADER_BUFFER_STATE_1_RCVD6_Msk 0x200000UL
/* USB32DEV_LNK.LNK_TX_TYPE1_HEADER_BUFFER */
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_HEADER_Pos 0UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_HEADER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_TX_TYPE1_HEADER_BUFFER_STATE_0 */
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE0_Pos 0UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE0_Msk 0xFUL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID0_Pos 4UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID0_Msk 0x10UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT0_Pos 5UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT0_Msk 0x20UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE1_Pos 8UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE1_Msk 0xF00UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID1_Pos 12UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID1_Msk 0x1000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT1_Pos 13UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT1_Msk 0x2000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE2_Pos 16UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE2_Msk 0xF0000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID2_Pos 20UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID2_Msk 0x100000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT2_Pos 21UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT2_Msk 0x200000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE3_Pos 24UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SEQUENCE3_Msk 0xF000000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID3_Pos 28UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_VALID3_Msk 0x10000000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT3_Pos 29UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_0_SENT3_Msk 0x20000000UL
/* USB32DEV_LNK.LNK_TX_TYPE1_HEADER_BUFFER_STATE_1 */
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE4_Pos 0UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE4_Msk 0xFUL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_VALID4_Pos 4UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_VALID4_Msk 0x10UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SENT4_Pos 5UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SENT4_Msk 0x20UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE5_Pos 8UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE5_Msk 0xF00UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_VALID5_Pos 12UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_VALID5_Msk 0x1000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SENT5_Pos 13UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SENT5_Msk 0x2000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE6_Pos 16UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SEQUENCE6_Msk 0xF0000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_VALID6_Pos 20UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_VALID6_Msk 0x100000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SENT6_Pos 21UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_SENT6_Msk 0x200000UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_REMOTE_CREDITS_Pos 29UL
#define USB32DEV_LNK_TX_TYPE1_HEADER_BUFFER_STATE_1_REMOTE_CREDITS_Msk 0xE0000000UL
/* USB32DEV_LNK.LNK_TX_TYPE2_HEADER_BUFFER */
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_HEADER_Pos 0UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_HEADER_Msk 0xFFFFFFFFUL
/* USB32DEV_LNK.LNK_TX_TYPE2_HEADER_BUFFER_STATE_0 */
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE0_Pos 0UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE0_Msk 0xFUL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID0_Pos 4UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID0_Msk 0x10UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT0_Pos 5UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT0_Msk 0x20UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE1_Pos 8UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE1_Msk 0xF00UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID1_Pos 12UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID1_Msk 0x1000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT1_Pos 13UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT1_Msk 0x2000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE2_Pos 16UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE2_Msk 0xF0000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID2_Pos 20UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID2_Msk 0x100000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT2_Pos 21UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT2_Msk 0x200000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE3_Pos 24UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SEQUENCE3_Msk 0xF000000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID3_Pos 28UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_VALID3_Msk 0x10000000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT3_Pos 29UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_0_SENT3_Msk 0x20000000UL
/* USB32DEV_LNK.LNK_TX_TYPE2_HEADER_BUFFER_STATE_1 */
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE4_Pos 0UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE4_Msk 0xFUL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_VALID4_Pos 4UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_VALID4_Msk 0x10UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SENT4_Pos 5UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SENT4_Msk 0x20UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE5_Pos 8UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE5_Msk 0xF00UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_VALID5_Pos 12UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_VALID5_Msk 0x1000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SENT5_Pos 13UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SENT5_Msk 0x2000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE6_Pos 16UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SEQUENCE6_Msk 0xF0000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_VALID6_Pos 20UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_VALID6_Msk 0x100000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SENT6_Pos 21UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_SENT6_Msk 0x200000UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_REMOTE_CREDITS_Pos 29UL
#define USB32DEV_LNK_TX_TYPE2_HEADER_BUFFER_STATE_1_REMOTE_CREDITS_Msk 0xE0000000UL


/* USB32DEV_PROT.PROT_CS */
#define USB32DEV_PROT_CS_DEVICEADDR_Pos 0UL
#define USB32DEV_PROT_CS_DEVICEADDR_Msk 0x7FUL
#define USB32DEV_PROT_CS_DISABLE_INGRS_SEQ_MATCH_Pos 7UL
#define USB32DEV_PROT_CS_DISABLE_INGRS_SEQ_MATCH_Msk 0x80UL
#define USB32DEV_PROT_CS_DISABLE_EGRS_SEQ_MATCH_Pos 8UL
#define USB32DEV_PROT_CS_DISABLE_EGRS_SEQ_MATCH_Msk 0x100UL
#define USB32DEV_PROT_CS_DISABLE_HE_DETECTION_Pos 9UL
#define USB32DEV_PROT_CS_DISABLE_HE_DETECTION_Msk 0x200UL
#define USB32DEV_PROT_CS_SPARE_CS_Pos 10UL
#define USB32DEV_PROT_CS_SPARE_CS_Msk 0x1C00UL
#define USB32DEV_PROT_CS_EN_STATUS_CONTROL_Pos 13UL
#define USB32DEV_PROT_CS_EN_STATUS_CONTROL_Msk 0x2000UL
#define USB32DEV_PROT_CS_STATUS_RESPONSE_Pos 14UL
#define USB32DEV_PROT_CS_STATUS_RESPONSE_Msk 0x4000UL
#define USB32DEV_PROT_CS_STATUS_CLR_BUSY_Pos 15UL
#define USB32DEV_PROT_CS_STATUS_CLR_BUSY_Msk 0x8000UL
#define USB32DEV_PROT_CS_SETUP_CLR_BUSY_Pos 16UL
#define USB32DEV_PROT_CS_SETUP_CLR_BUSY_Msk 0x10000UL
#define USB32DEV_PROT_CS_NRDY_ALL_Pos 17UL
#define USB32DEV_PROT_CS_NRDY_ALL_Msk 0x20000UL
#define USB32DEV_PROT_CS_TP_THRESHOLD_Pos 18UL
#define USB32DEV_PROT_CS_TP_THRESHOLD_Msk 0xFC0000UL
#define USB32DEV_PROT_CS_PROT_HOST_RESET_RESP_Pos 24UL
#define USB32DEV_PROT_CS_PROT_HOST_RESET_RESP_Msk 0x1000000UL
#define USB32DEV_PROT_CS_SEQ_NUM_CONFIG_Pos 25UL
#define USB32DEV_PROT_CS_SEQ_NUM_CONFIG_Msk 0x2000000UL
#define USB32DEV_PROT_CS_DISABLE_IDLE_DET_Pos 26UL
#define USB32DEV_PROT_CS_DISABLE_IDLE_DET_Msk 0x4000000UL
#define USB32DEV_PROT_CS_MULT_TIMER_Pos 27UL
#define USB32DEV_PROT_CS_MULT_TIMER_Msk 0xF8000000UL
/* USB32DEV_PROT.PROT_INTR */
#define USB32DEV_PROT_INTR_LMP_RCV_EV_Pos 0UL
#define USB32DEV_PROT_INTR_LMP_RCV_EV_Msk 0x1UL
#define USB32DEV_PROT_INTR_LMP_UNKNOWN_EV_Pos 1UL
#define USB32DEV_PROT_INTR_LMP_UNKNOWN_EV_Msk 0x2UL
#define USB32DEV_PROT_INTR_LMP_PORT_CAP_EV_Pos 2UL
#define USB32DEV_PROT_INTR_LMP_PORT_CAP_EV_Msk 0x4UL
#define USB32DEV_PROT_INTR_LMP_PORT_CFG_EV_Pos 3UL
#define USB32DEV_PROT_INTR_LMP_PORT_CFG_EV_Msk 0x8UL
#define USB32DEV_PROT_INTR_TIMEOUT_PORT_CAP_EV_Pos 4UL
#define USB32DEV_PROT_INTR_TIMEOUT_PORT_CAP_EV_Msk 0x10UL
#define USB32DEV_PROT_INTR_TIMEOUT_PORT_CFG_EV_Pos 5UL
#define USB32DEV_PROT_INTR_TIMEOUT_PORT_CFG_EV_Msk 0x20UL
#define USB32DEV_PROT_INTR_TIMEOUT_PING_EV_Pos 6UL
#define USB32DEV_PROT_INTR_TIMEOUT_PING_EV_Msk 0x40UL
#define USB32DEV_PROT_INTR_ITP_EV_Pos 8UL
#define USB32DEV_PROT_INTR_ITP_EV_Msk 0x100UL
#define USB32DEV_PROT_INTR_SUTOK_EV_Pos 9UL
#define USB32DEV_PROT_INTR_SUTOK_EV_Msk 0x200UL
#define USB32DEV_PROT_INTR_HOST_ERR_EV_Pos 10UL
#define USB32DEV_PROT_INTR_HOST_ERR_EV_Msk 0x400UL
#define USB32DEV_PROT_INTR_STATUS_STAGE_Pos 11UL
#define USB32DEV_PROT_INTR_STATUS_STAGE_Msk 0x800UL
#define USB32DEV_PROT_INTR_LMP_INVALID_PORT_CAP_EV_Pos 12UL
#define USB32DEV_PROT_INTR_LMP_INVALID_PORT_CAP_EV_Msk 0x1000UL
#define USB32DEV_PROT_INTR_LMP_INVALID_PORT_CFG_EV_Pos 13UL
#define USB32DEV_PROT_INTR_LMP_INVALID_PORT_CFG_EV_Msk 0x2000UL
#define USB32DEV_PROT_INTR_EP0_STALLED_EV_Pos 14UL
#define USB32DEV_PROT_INTR_EP0_STALLED_EV_Msk 0x4000UL
#define USB32DEV_PROT_INTR_SET_ADDR0_EV_Pos 15UL
#define USB32DEV_PROT_INTR_SET_ADDR0_EV_Msk 0x8000UL
#define USB32DEV_PROT_INTR_LDM_TX_Pos 16UL
#define USB32DEV_PROT_INTR_LDM_TX_Msk 0x10000UL
#define USB32DEV_PROT_INTR_LDM_DL_TX_Pos 17UL
#define USB32DEV_PROT_INTR_LDM_DL_TX_Msk 0x20000UL
#define USB32DEV_PROT_INTR_LDM_RX_Pos 18UL
#define USB32DEV_PROT_INTR_LDM_RX_Msk 0x40000UL
#define USB32DEV_PROT_INTR_LDM_DL_RX_Pos 19UL
#define USB32DEV_PROT_INTR_LDM_DL_RX_Msk 0x80000UL
#define USB32DEV_PROT_INTR_LDM_RESP_TIMEOUT_1_Pos 20UL
#define USB32DEV_PROT_INTR_LDM_RESP_TIMEOUT_1_Msk 0x100000UL
#define USB32DEV_PROT_INTR_LDM_RESP_TIMEOUT_2_Pos 21UL
#define USB32DEV_PROT_INTR_LDM_RESP_TIMEOUT_2_Msk 0x200000UL
#define USB32DEV_PROT_INTR_LDM_RESP_TIMEOUT_3_Pos 22UL
#define USB32DEV_PROT_INTR_LDM_RESP_TIMEOUT_3_Msk 0x400000UL
#define USB32DEV_PROT_INTR_SET_ADDR_Pos 23UL
#define USB32DEV_PROT_INTR_SET_ADDR_Msk 0x800000UL
#define USB32DEV_PROT_INTR_DEFERRED_SET_Pos 24UL
#define USB32DEV_PROT_INTR_DEFERRED_SET_Msk 0x1000000UL
#define USB32DEV_PROT_INTR_DEFERRED_CLEAR_Pos 25UL
#define USB32DEV_PROT_INTR_DEFERRED_CLEAR_Msk 0x2000000UL
#define USB32DEV_PROT_INTR_LDM_STATE_CHANGE_Pos 26UL
#define USB32DEV_PROT_INTR_LDM_STATE_CHANGE_Msk 0x4000000UL
#define USB32DEV_PROT_INTR_T1_SCH_FULL_Pos 27UL
#define USB32DEV_PROT_INTR_T1_SCH_FULL_Msk 0x8000000UL
#define USB32DEV_PROT_INTR_T2_SCH_FULL_Pos 28UL
#define USB32DEV_PROT_INTR_T2_SCH_FULL_Msk 0x10000000UL
/* USB32DEV_PROT.PROT_INTR_SET */
#define USB32DEV_PROT_INTR_SET_LMP_RCV_EV_Pos 0UL
#define USB32DEV_PROT_INTR_SET_LMP_RCV_EV_Msk 0x1UL
#define USB32DEV_PROT_INTR_SET_LMP_UNKNOWN_EV_Pos 1UL
#define USB32DEV_PROT_INTR_SET_LMP_UNKNOWN_EV_Msk 0x2UL
#define USB32DEV_PROT_INTR_SET_LMP_PORT_CAP_EV_Pos 2UL
#define USB32DEV_PROT_INTR_SET_LMP_PORT_CAP_EV_Msk 0x4UL
#define USB32DEV_PROT_INTR_SET_LMP_PORT_CFG_EV_Pos 3UL
#define USB32DEV_PROT_INTR_SET_LMP_PORT_CFG_EV_Msk 0x8UL
#define USB32DEV_PROT_INTR_SET_TIMEOUT_PORT_CAP_EV_Pos 4UL
#define USB32DEV_PROT_INTR_SET_TIMEOUT_PORT_CAP_EV_Msk 0x10UL
#define USB32DEV_PROT_INTR_SET_TIMEOUT_PORT_CFG_EV_Pos 5UL
#define USB32DEV_PROT_INTR_SET_TIMEOUT_PORT_CFG_EV_Msk 0x20UL
#define USB32DEV_PROT_INTR_SET_TIMEOUT_PING_EV_Pos 6UL
#define USB32DEV_PROT_INTR_SET_TIMEOUT_PING_EV_Msk 0x40UL
#define USB32DEV_PROT_INTR_SET_ITP_EV_Pos 8UL
#define USB32DEV_PROT_INTR_SET_ITP_EV_Msk 0x100UL
#define USB32DEV_PROT_INTR_SET_SUTOK_EV_Pos 9UL
#define USB32DEV_PROT_INTR_SET_SUTOK_EV_Msk 0x200UL
#define USB32DEV_PROT_INTR_SET_HOST_ERR_EV_Pos 10UL
#define USB32DEV_PROT_INTR_SET_HOST_ERR_EV_Msk 0x400UL
#define USB32DEV_PROT_INTR_SET_STATUS_STAGE_Pos 11UL
#define USB32DEV_PROT_INTR_SET_STATUS_STAGE_Msk 0x800UL
#define USB32DEV_PROT_INTR_SET_LMP_INVALID_PORT_CAP_EV_Pos 12UL
#define USB32DEV_PROT_INTR_SET_LMP_INVALID_PORT_CAP_EV_Msk 0x1000UL
#define USB32DEV_PROT_INTR_SET_LMP_INVALID_PORT_CFG_EV_Pos 13UL
#define USB32DEV_PROT_INTR_SET_LMP_INVALID_PORT_CFG_EV_Msk 0x2000UL
#define USB32DEV_PROT_INTR_SET_EP0_STALLED_EV_Pos 14UL
#define USB32DEV_PROT_INTR_SET_EP0_STALLED_EV_Msk 0x4000UL
#define USB32DEV_PROT_INTR_SET_SET_ADDR0_EV_Pos 15UL
#define USB32DEV_PROT_INTR_SET_SET_ADDR0_EV_Msk 0x8000UL
#define USB32DEV_PROT_INTR_SET_LDM_TX_Pos 16UL
#define USB32DEV_PROT_INTR_SET_LDM_TX_Msk 0x10000UL
#define USB32DEV_PROT_INTR_SET_LDM_DL_TX_Pos 17UL
#define USB32DEV_PROT_INTR_SET_LDM_DL_TX_Msk 0x20000UL
#define USB32DEV_PROT_INTR_SET_LDM_RX_Pos 18UL
#define USB32DEV_PROT_INTR_SET_LDM_RX_Msk 0x40000UL
#define USB32DEV_PROT_INTR_SET_LDM_DL_RX_Pos 19UL
#define USB32DEV_PROT_INTR_SET_LDM_DL_RX_Msk 0x80000UL
#define USB32DEV_PROT_INTR_SET_RESP_TIMEOUT_1_Pos 20UL
#define USB32DEV_PROT_INTR_SET_RESP_TIMEOUT_1_Msk 0x100000UL
#define USB32DEV_PROT_INTR_SET_RESP_TIMEOUT_2_Pos 21UL
#define USB32DEV_PROT_INTR_SET_RESP_TIMEOUT_2_Msk 0x200000UL
#define USB32DEV_PROT_INTR_SET_RESP_TIMEOUT_3_Pos 22UL
#define USB32DEV_PROT_INTR_SET_RESP_TIMEOUT_3_Msk 0x400000UL
#define USB32DEV_PROT_INTR_SET_SET_ADDR_Pos 23UL
#define USB32DEV_PROT_INTR_SET_SET_ADDR_Msk 0x800000UL
#define USB32DEV_PROT_INTR_SET_DEFERRED_SET_Pos 24UL
#define USB32DEV_PROT_INTR_SET_DEFERRED_SET_Msk 0x1000000UL
#define USB32DEV_PROT_INTR_SET_DEFERRED_CLEAR_Pos 25UL
#define USB32DEV_PROT_INTR_SET_DEFERRED_CLEAR_Msk 0x2000000UL
#define USB32DEV_PROT_INTR_SET_LDM_STATE_CHANGE_Pos 26UL
#define USB32DEV_PROT_INTR_SET_LDM_STATE_CHANGE_Msk 0x4000000UL
#define USB32DEV_PROT_INTR_SET_T1_SCH_FULL_Pos 27UL
#define USB32DEV_PROT_INTR_SET_T1_SCH_FULL_Msk 0x8000000UL
#define USB32DEV_PROT_INTR_SET_T2_SCH_FULL_Pos 28UL
#define USB32DEV_PROT_INTR_SET_T2_SCH_FULL_Msk 0x10000000UL
/* USB32DEV_PROT.PROT_INTR_MASK */
#define USB32DEV_PROT_INTR_MASK_LMP_RCV_MASK_Pos 0UL
#define USB32DEV_PROT_INTR_MASK_LMP_RCV_MASK_Msk 0x1UL
#define USB32DEV_PROT_INTR_MASK_LMP_UNKNOWN_MASK_Pos 1UL
#define USB32DEV_PROT_INTR_MASK_LMP_UNKNOWN_MASK_Msk 0x2UL
#define USB32DEV_PROT_INTR_MASK_LMP_PORT_CAP_MASK_Pos 2UL
#define USB32DEV_PROT_INTR_MASK_LMP_PORT_CAP_MASK_Msk 0x4UL
#define USB32DEV_PROT_INTR_MASK_LMP_PORT_CFG_MASK_Pos 3UL
#define USB32DEV_PROT_INTR_MASK_LMP_PORT_CFG_MASK_Msk 0x8UL
#define USB32DEV_PROT_INTR_MASK_TIMEOUT_PORT_CAP_MASK_Pos 4UL
#define USB32DEV_PROT_INTR_MASK_TIMEOUT_PORT_CAP_MASK_Msk 0x10UL
#define USB32DEV_PROT_INTR_MASK_TIMEOUT_PORT_CFG_MASK_Pos 5UL
#define USB32DEV_PROT_INTR_MASK_TIMEOUT_PORT_CFG_MASK_Msk 0x20UL
#define USB32DEV_PROT_INTR_MASK_TIMEOUT_PING_MASK_Pos 6UL
#define USB32DEV_PROT_INTR_MASK_TIMEOUT_PING_MASK_Msk 0x40UL
#define USB32DEV_PROT_INTR_MASK_ITP_MASK_Pos 8UL
#define USB32DEV_PROT_INTR_MASK_ITP_MASK_Msk 0x100UL
#define USB32DEV_PROT_INTR_MASK_SUTOK_MASK_Pos 9UL
#define USB32DEV_PROT_INTR_MASK_SUTOK_MASK_Msk 0x200UL
#define USB32DEV_PROT_INTR_MASK_HOST_ERR_MASK_Pos 10UL
#define USB32DEV_PROT_INTR_MASK_HOST_ERR_MASK_Msk 0x400UL
#define USB32DEV_PROT_INTR_MASK_STATUS_STAGE_MASK_Pos 11UL
#define USB32DEV_PROT_INTR_MASK_STATUS_STAGE_MASK_Msk 0x800UL
#define USB32DEV_PROT_INTR_MASK_LMP_INVALID_PORT_CAP_MASK_Pos 12UL
#define USB32DEV_PROT_INTR_MASK_LMP_INVALID_PORT_CAP_MASK_Msk 0x1000UL
#define USB32DEV_PROT_INTR_MASK_LMP_INVALID_PORT_CFG_MASK_Pos 13UL
#define USB32DEV_PROT_INTR_MASK_LMP_INVALID_PORT_CFG_MASK_Msk 0x2000UL
#define USB32DEV_PROT_INTR_MASK_EP0_STALLED_MASK_Pos 14UL
#define USB32DEV_PROT_INTR_MASK_EP0_STALLED_MASK_Msk 0x4000UL
#define USB32DEV_PROT_INTR_MASK_SET_ADDR0_MASK_Pos 15UL
#define USB32DEV_PROT_INTR_MASK_SET_ADDR0_MASK_Msk 0x8000UL
#define USB32DEV_PROT_INTR_MASK_LDM_TX_MASK_Pos 16UL
#define USB32DEV_PROT_INTR_MASK_LDM_TX_MASK_Msk 0x10000UL
#define USB32DEV_PROT_INTR_MASK_LDM_DL_TX_MASK_Pos 17UL
#define USB32DEV_PROT_INTR_MASK_LDM_DL_TX_MASK_Msk 0x20000UL
#define USB32DEV_PROT_INTR_MASK_LDM_RX_MASK_Pos 18UL
#define USB32DEV_PROT_INTR_MASK_LDM_RX_MASK_Msk 0x40000UL
#define USB32DEV_PROT_INTR_MASK_LDM_DL_RX_MASK_Pos 19UL
#define USB32DEV_PROT_INTR_MASK_LDM_DL_RX_MASK_Msk 0x80000UL
#define USB32DEV_PROT_INTR_MASK_RESP_TIMEOUT_1_MASK_Pos 20UL
#define USB32DEV_PROT_INTR_MASK_RESP_TIMEOUT_1_MASK_Msk 0x100000UL
#define USB32DEV_PROT_INTR_MASK_RESP_TIMEOUT_2_MASK_Pos 21UL
#define USB32DEV_PROT_INTR_MASK_RESP_TIMEOUT_2_MASK_Msk 0x200000UL
#define USB32DEV_PROT_INTR_MASK_RESP_TIMEOUT_3_MASK_Pos 22UL
#define USB32DEV_PROT_INTR_MASK_RESP_TIMEOUT_3_MASK_Msk 0x400000UL
#define USB32DEV_PROT_INTR_MASK_SET_ADDR_MASK_Pos 23UL
#define USB32DEV_PROT_INTR_MASK_SET_ADDR_MASK_Msk 0x800000UL
#define USB32DEV_PROT_INTR_MASK_DEFERRED_SET_MASK_Pos 24UL
#define USB32DEV_PROT_INTR_MASK_DEFERRED_SET_MASK_Msk 0x1000000UL
#define USB32DEV_PROT_INTR_MASK_DEFERRED_CLEAR_MASK_Pos 25UL
#define USB32DEV_PROT_INTR_MASK_DEFERRED_CLEAR_MASK_Msk 0x2000000UL
#define USB32DEV_PROT_INTR_MASK_LDM_STATE_CHANGE_MASK_Pos 26UL
#define USB32DEV_PROT_INTR_MASK_LDM_STATE_CHANGE_MASK_Msk 0x4000000UL
#define USB32DEV_PROT_INTR_MASK_T1_SCH_FULL_MASK_Pos 27UL
#define USB32DEV_PROT_INTR_MASK_T1_SCH_FULL_MASK_Msk 0x8000000UL
#define USB32DEV_PROT_INTR_MASK_T2_SCH_FULL_MASK_Pos 28UL
#define USB32DEV_PROT_INTR_MASK_T2_SCH_FULL_MASK_Msk 0x10000000UL
/* USB32DEV_PROT.PROT_INTR_MASKED */
#define USB32DEV_PROT_INTR_MASKED_LMP_RCV_MASKED_Pos 0UL
#define USB32DEV_PROT_INTR_MASKED_LMP_RCV_MASKED_Msk 0x1UL
#define USB32DEV_PROT_INTR_MASKED_LMP_UNKNOWN_MASKED_Pos 1UL
#define USB32DEV_PROT_INTR_MASKED_LMP_UNKNOWN_MASKED_Msk 0x2UL
#define USB32DEV_PROT_INTR_MASKED_LMP_PORT_CAP_MASKED_Pos 2UL
#define USB32DEV_PROT_INTR_MASKED_LMP_PORT_CAP_MASKED_Msk 0x4UL
#define USB32DEV_PROT_INTR_MASKED_LMP_PORT_CFG_MASKED_Pos 3UL
#define USB32DEV_PROT_INTR_MASKED_LMP_PORT_CFG_MASKED_Msk 0x8UL
#define USB32DEV_PROT_INTR_MASKED_TIMEOUT_PORT_CAP_MASKED_Pos 4UL
#define USB32DEV_PROT_INTR_MASKED_TIMEOUT_PORT_CAP_MASKED_Msk 0x10UL
#define USB32DEV_PROT_INTR_MASKED_TIMEOUT_PORT_CFG_MASKED_Pos 5UL
#define USB32DEV_PROT_INTR_MASKED_TIMEOUT_PORT_CFG_MASKED_Msk 0x20UL
#define USB32DEV_PROT_INTR_MASKED_TIMEOUT_PING_MASKED_Pos 6UL
#define USB32DEV_PROT_INTR_MASKED_TIMEOUT_PING_MASKED_Msk 0x40UL
#define USB32DEV_PROT_INTR_MASKED_ITP_MASKED_Pos 8UL
#define USB32DEV_PROT_INTR_MASKED_ITP_MASKED_Msk 0x100UL
#define USB32DEV_PROT_INTR_MASKED_SUTOK_MASKED_Pos 9UL
#define USB32DEV_PROT_INTR_MASKED_SUTOK_MASKED_Msk 0x200UL
#define USB32DEV_PROT_INTR_MASKED_HOST_ERR_MASKED_Pos 10UL
#define USB32DEV_PROT_INTR_MASKED_HOST_ERR_MASKED_Msk 0x400UL
#define USB32DEV_PROT_INTR_MASKED_STATUS_STAGE_MASKED_Pos 11UL
#define USB32DEV_PROT_INTR_MASKED_STATUS_STAGE_MASKED_Msk 0x800UL
#define USB32DEV_PROT_INTR_MASKED_LMP_INVALID_PORT_CAP_MASKED_Pos 12UL
#define USB32DEV_PROT_INTR_MASKED_LMP_INVALID_PORT_CAP_MASKED_Msk 0x1000UL
#define USB32DEV_PROT_INTR_MASKED_LMP_INVALID_PORT_CFG_MASKED_Pos 13UL
#define USB32DEV_PROT_INTR_MASKED_LMP_INVALID_PORT_CFG_MASKED_Msk 0x2000UL
#define USB32DEV_PROT_INTR_MASKED_EP0_STALLED_MASKED_Pos 14UL
#define USB32DEV_PROT_INTR_MASKED_EP0_STALLED_MASKED_Msk 0x4000UL
#define USB32DEV_PROT_INTR_MASKED_SET_ADDR0_MASKED_Pos 15UL
#define USB32DEV_PROT_INTR_MASKED_SET_ADDR0_MASKED_Msk 0x8000UL
#define USB32DEV_PROT_INTR_MASKED_LDM_TX_MASKED_Pos 16UL
#define USB32DEV_PROT_INTR_MASKED_LDM_TX_MASKED_Msk 0x10000UL
#define USB32DEV_PROT_INTR_MASKED_LDM_DL_TX_MASKED_Pos 17UL
#define USB32DEV_PROT_INTR_MASKED_LDM_DL_TX_MASKED_Msk 0x20000UL
#define USB32DEV_PROT_INTR_MASKED_LDM_RX_MASKED_Pos 18UL
#define USB32DEV_PROT_INTR_MASKED_LDM_RX_MASKED_Msk 0x40000UL
#define USB32DEV_PROT_INTR_MASKED_LDM_DL_RX_MASKED_Pos 19UL
#define USB32DEV_PROT_INTR_MASKED_LDM_DL_RX_MASKED_Msk 0x80000UL
#define USB32DEV_PROT_INTR_MASKED_RESP_TIMEOUT_1_MASKED_Pos 20UL
#define USB32DEV_PROT_INTR_MASKED_RESP_TIMEOUT_1_MASKED_Msk 0x100000UL
#define USB32DEV_PROT_INTR_MASKED_RESP_TIMEOUT_2_MASKED_Pos 21UL
#define USB32DEV_PROT_INTR_MASKED_RESP_TIMEOUT_2_MASKED_Msk 0x200000UL
#define USB32DEV_PROT_INTR_MASKED_RESP_TIMEOUT_3_MASKED_Pos 22UL
#define USB32DEV_PROT_INTR_MASKED_RESP_TIMEOUT_3_MASKED_Msk 0x400000UL
#define USB32DEV_PROT_INTR_MASKED_SET_ADDR_MASKED_Pos 23UL
#define USB32DEV_PROT_INTR_MASKED_SET_ADDR_MASKED_Msk 0x800000UL
#define USB32DEV_PROT_INTR_MASKED_DEFERRED_SET_MASKED_Pos 24UL
#define USB32DEV_PROT_INTR_MASKED_DEFERRED_SET_MASKED_Msk 0x1000000UL
#define USB32DEV_PROT_INTR_MASKED_DEFERRED_CLEAR_MASKED_Pos 25UL
#define USB32DEV_PROT_INTR_MASKED_DEFERRED_CLEAR_MASKED_Msk 0x2000000UL
#define USB32DEV_PROT_INTR_MASKED_LDM_STATE_CHANGE_MASKED_Pos 26UL
#define USB32DEV_PROT_INTR_MASKED_LDM_STATE_CHANGE_MASKED_Msk 0x4000000UL
#define USB32DEV_PROT_INTR_MASKED_T1_SCH_FULL_MASKED_Pos 27UL
#define USB32DEV_PROT_INTR_MASKED_T1_SCH_FULL_MASKED_Msk 0x8000000UL
#define USB32DEV_PROT_INTR_MASKED_T2_SCH_FULL_MASKED_Pos 28UL
#define USB32DEV_PROT_INTR_MASKED_T2_SCH_FULL_MASKED_Msk 0x10000000UL
/* USB32DEV_PROT.PROT_EP_INTR */
#define USB32DEV_PROT_EP_INTR_EP_IN_Pos 0UL
#define USB32DEV_PROT_EP_INTR_EP_IN_Msk 0xFFFFUL
#define USB32DEV_PROT_EP_INTR_EP_OUT_Pos 16UL
#define USB32DEV_PROT_EP_INTR_EP_OUT_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_EP_INTR_SET */
#define USB32DEV_PROT_EP_INTR_SET_EP_IN_Pos 0UL
#define USB32DEV_PROT_EP_INTR_SET_EP_IN_Msk 0xFFFFUL
#define USB32DEV_PROT_EP_INTR_SET_EP_OUT_Pos 16UL
#define USB32DEV_PROT_EP_INTR_SET_EP_OUT_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_EP_INTR_MASK */
#define USB32DEV_PROT_EP_INTR_MASK_EP_IN_MASK_Pos 0UL
#define USB32DEV_PROT_EP_INTR_MASK_EP_IN_MASK_Msk 0xFFFFUL
#define USB32DEV_PROT_EP_INTR_MASK_EP_OUT_MASK_Pos 16UL
#define USB32DEV_PROT_EP_INTR_MASK_EP_OUT_MASK_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_EP_INTR_MASKED */
#define USB32DEV_PROT_EP_INTR_MASKED_EP_IN_MASKED_Pos 0UL
#define USB32DEV_PROT_EP_INTR_MASKED_EP_IN_MASKED_Msk 0xFFFFUL
#define USB32DEV_PROT_EP_INTR_MASKED_EP_OUT_MASKED_Pos 16UL
#define USB32DEV_PROT_EP_INTR_MASKED_EP_OUT_MASKED_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_DEVICE_NOTIF_FUNC_WAKE */
#define USB32DEV_PROT_DEVICE_NOTIF_FUNC_WAKE_INTERFACE_Pos 0UL
#define USB32DEV_PROT_DEVICE_NOTIF_FUNC_WAKE_INTERFACE_Msk 0xFFUL
#define USB32DEV_PROT_DEVICE_NOTIF_FUNC_WAKE_REQUEST_Pos 31UL
#define USB32DEV_PROT_DEVICE_NOTIF_FUNC_WAKE_REQUEST_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_DEVICE_NOTIF_LTM */
#define USB32DEV_PROT_DEVICE_NOTIF_LTM_BELT_LATENCY_Pos 0UL
#define USB32DEV_PROT_DEVICE_NOTIF_LTM_BELT_LATENCY_Msk 0x3FFUL
#define USB32DEV_PROT_DEVICE_NOTIF_LTM_BELT_SCALE_Pos 10UL
#define USB32DEV_PROT_DEVICE_NOTIF_LTM_BELT_SCALE_Msk 0xC00UL
#define USB32DEV_PROT_DEVICE_NOTIF_LTM_REQUEST_Pos 31UL
#define USB32DEV_PROT_DEVICE_NOTIF_LTM_REQUEST_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_DEVICE_NOTIF_BIAM */
#define USB32DEV_PROT_DEVICE_NOTIF_BIAM_BIA_Pos 0UL
#define USB32DEV_PROT_DEVICE_NOTIF_BIAM_BIA_Msk 0xFFFFUL
#define USB32DEV_PROT_DEVICE_NOTIF_BIAM_REQUEST_Pos 31UL
#define USB32DEV_PROT_DEVICE_NOTIF_BIAM_REQUEST_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_LMP_PORT_CAPABILITY_TIMER */
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_RX_TIMEOUT_Pos 0UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_RX_TIMEOUT_Msk 0x7FFFUL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_RX_DISABLE_Pos 15UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_RX_DISABLE_Msk 0x8000UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_TX_TIMEOUT_Pos 16UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_TX_TIMEOUT_Msk 0x7FFF0000UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_TX_DISABLE_Pos 31UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITY_TIMER_TX_DISABLE_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_LMP_PORT_CONFIGURATION_TIMER */
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_RX_TIMEOUT_Pos 0UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_RX_TIMEOUT_Msk 0x7FFFUL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_RX_DISABLE_Pos 15UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_RX_DISABLE_Msk 0x8000UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_TX_TIMEOUT_Pos 16UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_TX_TIMEOUT_Msk 0x7FFF0000UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_TX_DISABLE_Pos 31UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TIMER_TX_DISABLE_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_PING_TIMEOUT */
#define USB32DEV_PROT_PING_TIMEOUT_PING_TIMEOUT_Pos 0UL
#define USB32DEV_PROT_PING_TIMEOUT_PING_TIMEOUT_Msk 0xFFFFFFUL
#define USB32DEV_PROT_PING_TIMEOUT_PING_DISABLE_Pos 31UL
#define USB32DEV_PROT_PING_TIMEOUT_PING_DISABLE_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_FRAMECNT */
#define USB32DEV_PROT_FRAMECNT_SS_MICROFRAME_Pos 0UL
#define USB32DEV_PROT_FRAMECNT_SS_MICROFRAME_Msk 0x3FFFUL
#define USB32DEV_PROT_FRAMECNT_DELTA_Pos 14UL
#define USB32DEV_PROT_FRAMECNT_DELTA_Msk 0x7FFC000UL
/* USB32DEV_PROT.PROT_ITP_CORRECTION */
#define USB32DEV_PROT_ITP_CORRECTION_CORRECTION_Pos 0UL
#define USB32DEV_PROT_ITP_CORRECTION_CORRECTION_Msk 0x3FFFUL
/* USB32DEV_PROT.PROT_SETUPDAT0 */
#define USB32DEV_PROT_SETUPDAT0_SETUP_REQUEST_TYPE_Pos 0UL
#define USB32DEV_PROT_SETUPDAT0_SETUP_REQUEST_TYPE_Msk 0xFFUL
#define USB32DEV_PROT_SETUPDAT0_SETUP_REQUEST_Pos 8UL
#define USB32DEV_PROT_SETUPDAT0_SETUP_REQUEST_Msk 0xFF00UL
#define USB32DEV_PROT_SETUPDAT0_SETUP_VALUE_Pos 16UL
#define USB32DEV_PROT_SETUPDAT0_SETUP_VALUE_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_SETUPDAT1 */
#define USB32DEV_PROT_SETUPDAT1_SETUP_INDEX_Pos 0UL
#define USB32DEV_PROT_SETUPDAT1_SETUP_INDEX_Msk 0xFFFFUL
#define USB32DEV_PROT_SETUPDAT1_SETUP_LENGTH_Pos 16UL
#define USB32DEV_PROT_SETUPDAT1_SETUP_LENGTH_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_SEQ_NUM */
#define USB32DEV_PROT_SEQ_NUM_INGRESS_EP_NUM_Pos 0UL
#define USB32DEV_PROT_SEQ_NUM_INGRESS_EP_NUM_Msk 0xFUL
#define USB32DEV_PROT_SEQ_NUM_INGRESS_READ_SEQ_NUM_Pos 4UL
#define USB32DEV_PROT_SEQ_NUM_INGRESS_READ_SEQ_NUM_Msk 0x1F0UL
#define USB32DEV_PROT_SEQ_NUM_INGRESS_WRITE_SEQ_NUM_Pos 9UL
#define USB32DEV_PROT_SEQ_NUM_INGRESS_WRITE_SEQ_NUM_Msk 0x3E00UL
#define USB32DEV_PROT_SEQ_NUM_EGRESS_EP_NUM_Pos 16UL
#define USB32DEV_PROT_SEQ_NUM_EGRESS_EP_NUM_Msk 0xF0000UL
#define USB32DEV_PROT_SEQ_NUM_EGRESS_READ_SEQ_NUM_Pos 20UL
#define USB32DEV_PROT_SEQ_NUM_EGRESS_READ_SEQ_NUM_Msk 0x1F00000UL
#define USB32DEV_PROT_SEQ_NUM_EGRESS_WRITE_SEQ_NUM_Pos 25UL
#define USB32DEV_PROT_SEQ_NUM_EGRESS_WRITE_SEQ_NUM_Msk 0x3E000000UL
/* USB32DEV_PROT.PROT_LMP_RECEIVED */
#define USB32DEV_PROT_LMP_RECEIVED_U2_INACTIVITY_TIMEOUT_Pos 0UL
#define USB32DEV_PROT_LMP_RECEIVED_U2_INACTIVITY_TIMEOUT_Msk 0xFFUL
#define USB32DEV_PROT_LMP_RECEIVED_FORCE_LINKPM_ACCEPT_Pos 8UL
#define USB32DEV_PROT_LMP_RECEIVED_FORCE_LINKPM_ACCEPT_Msk 0x100UL
/* USB32DEV_PROT.PROT_LMP_OVERRIDE */
#define USB32DEV_PROT_LMP_OVERRIDE_U2_INACTIVITY_TIMEOUT_Pos 0UL
#define USB32DEV_PROT_LMP_OVERRIDE_U2_INACTIVITY_TIMEOUT_Msk 0xFFUL
#define USB32DEV_PROT_LMP_OVERRIDE_FORCE_LINKPM_ACCEPT_Pos 8UL
#define USB32DEV_PROT_LMP_OVERRIDE_FORCE_LINKPM_ACCEPT_Msk 0x100UL
#define USB32DEV_PROT_LMP_OVERRIDE_INACITIVITY_TIMEOUT_OVR_Pos 29UL
#define USB32DEV_PROT_LMP_OVERRIDE_INACITIVITY_TIMEOUT_OVR_Msk 0x20000000UL
#define USB32DEV_PROT_LMP_OVERRIDE_LINKPM_ACCEPT_OVR_Pos 30UL
#define USB32DEV_PROT_LMP_OVERRIDE_LINKPM_ACCEPT_OVR_Msk 0x40000000UL
#define USB32DEV_PROT_LMP_OVERRIDE_LMP_SEND_Pos 31UL
#define USB32DEV_PROT_LMP_OVERRIDE_LMP_SEND_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_LMP_PORT_CAPABILITIES_RX */
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_LINK_SPEED_Pos 0UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_LINK_SPEED_Msk 0x7FUL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_NUM_HP_BUFFERS_Pos 7UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_NUM_HP_BUFFERS_Msk 0x7F80UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_DIRECTION_Pos 15UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_DIRECTION_Msk 0x18000UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_TIEBREAKER_Pos 17UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_RX_TIEBREAKER_Msk 0x1E0000UL
/* USB32DEV_PROT.PROT_LMP_PORT_CAPABILITIES_TX */
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_LINK_SPEED_Pos 0UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_LINK_SPEED_Msk 0x7FUL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_NUM_HP_BUFFERS_Pos 7UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_NUM_HP_BUFFERS_Msk 0x7F80UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_DIRECTION_Pos 15UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_DIRECTION_Msk 0x18000UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_TIEBREAKER_Pos 17UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_TIEBREAKER_Msk 0x1E0000UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_LMP_SEND_Pos 31UL
#define USB32DEV_PROT_LMP_PORT_CAPABILITIES_TX_LMP_SEND_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_LMP_PORT_CONFIGURATION_RX */
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_RX_LINK_SPEED_Pos 0UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_RX_LINK_SPEED_Msk 0x7FUL
/* USB32DEV_PROT.PROT_LMP_PORT_CONFIGURATION_TX */
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TX_LINK_SPEED_Pos 0UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TX_LINK_SPEED_Msk 0x7FUL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TX_LMP_SEND_Pos 31UL
#define USB32DEV_PROT_LMP_PORT_CONFIGURATION_TX_LMP_SEND_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_STREAM_ERROR_DISABLE */
#define USB32DEV_PROT_STREAM_ERROR_DISABLE_TYPE_Pos 0UL
#define USB32DEV_PROT_STREAM_ERROR_DISABLE_TYPE_Msk 0x3FUL
/* USB32DEV_PROT.PROT_STREAM_ERROR_STATUS */
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ID_Pos 0UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ID_Msk 0xFFFFUL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_EP_NUM_Pos 16UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_EP_NUM_Msk 0xF0000UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_EP_IO_Pos 20UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_EP_IO_Msk 0x100000UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ERROR_TYPE_Pos 21UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ERROR_TYPE_Msk 0x7E00000UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ERROR_STATE_Pos 27UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ERROR_STATE_Msk 0x38000000UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ERROR_DETECTED_Pos 31UL
#define USB32DEV_PROT_STREAM_ERROR_STATUS_ERROR_DETECTED_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_LMP_PACKET_RX */
#define USB32DEV_PROT_LMP_PACKET_RX_LMP_PACKET_Pos 0UL
#define USB32DEV_PROT_LMP_PACKET_RX_LMP_PACKET_Msk 0xFFFFFFFFUL
/* USB32DEV_PROT.PROT_LMP_PACKET_TX */
#define USB32DEV_PROT_LMP_PACKET_TX_LMP_PACKET_Pos 0UL
#define USB32DEV_PROT_LMP_PACKET_TX_LMP_PACKET_Msk 0xFFFFFFFFUL
/* USB32DEV_PROT.PROT_EPI_INTR */
#define USB32DEV_PROT_EPI_INTR_COMMIT_Pos 0UL
#define USB32DEV_PROT_EPI_INTR_COMMIT_Msk 0x1UL
#define USB32DEV_PROT_EPI_INTR_RETRY_Pos 1UL
#define USB32DEV_PROT_EPI_INTR_RETRY_Msk 0x2UL
#define USB32DEV_PROT_EPI_INTR_FLOWCONTROL_Pos 2UL
#define USB32DEV_PROT_EPI_INTR_FLOWCONTROL_Msk 0x4UL
#define USB32DEV_PROT_EPI_INTR_ZERO_Pos 4UL
#define USB32DEV_PROT_EPI_INTR_ZERO_Msk 0x10UL
#define USB32DEV_PROT_EPI_INTR_SHORT_Pos 5UL
#define USB32DEV_PROT_EPI_INTR_SHORT_Msk 0x20UL
#define USB32DEV_PROT_EPI_INTR_OOSERR_Pos 6UL
#define USB32DEV_PROT_EPI_INTR_OOSERR_Msk 0x40UL
#define USB32DEV_PROT_EPI_INTR_DBTERM_Pos 8UL
#define USB32DEV_PROT_EPI_INTR_DBTERM_Msk 0x100UL
#define USB32DEV_PROT_EPI_INTR_STREAM_ERROR_Pos 9UL
#define USB32DEV_PROT_EPI_INTR_STREAM_ERROR_Msk 0x200UL
#define USB32DEV_PROT_EPI_INTR_FIRST_ACK_NUMP_0_Pos 10UL
#define USB32DEV_PROT_EPI_INTR_FIRST_ACK_NUMP_0_Msk 0x400UL
#define USB32DEV_PROT_EPI_INTR_STREAM_ST_CHG_Pos 16UL
#define USB32DEV_PROT_EPI_INTR_STREAM_ST_CHG_Msk 0x10000UL
#define USB32DEV_PROT_EPI_INTR_FLOWCONTROL_EXIT_Pos 17UL
#define USB32DEV_PROT_EPI_INTR_FLOWCONTROL_EXIT_Msk 0x20000UL
#define USB32DEV_PROT_EPI_INTR_SPARE_REG_Pos 18UL
#define USB32DEV_PROT_EPI_INTR_SPARE_REG_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPI_INTR_SET */
#define USB32DEV_PROT_EPI_INTR_SET_COMMIT_Pos 0UL
#define USB32DEV_PROT_EPI_INTR_SET_COMMIT_Msk 0x1UL
#define USB32DEV_PROT_EPI_INTR_SET_RETRY_Pos 1UL
#define USB32DEV_PROT_EPI_INTR_SET_RETRY_Msk 0x2UL
#define USB32DEV_PROT_EPI_INTR_SET_FLOWCONTROL_Pos 2UL
#define USB32DEV_PROT_EPI_INTR_SET_FLOWCONTROL_Msk 0x4UL
#define USB32DEV_PROT_EPI_INTR_SET_ZERO_Pos 4UL
#define USB32DEV_PROT_EPI_INTR_SET_ZERO_Msk 0x10UL
#define USB32DEV_PROT_EPI_INTR_SET_SHORT_Pos 5UL
#define USB32DEV_PROT_EPI_INTR_SET_SHORT_Msk 0x20UL
#define USB32DEV_PROT_EPI_INTR_SET_OOSERR_Pos 6UL
#define USB32DEV_PROT_EPI_INTR_SET_OOSERR_Msk 0x40UL
#define USB32DEV_PROT_EPI_INTR_SET_DBTERM_Pos 8UL
#define USB32DEV_PROT_EPI_INTR_SET_DBTERM_Msk 0x100UL
#define USB32DEV_PROT_EPI_INTR_SET_STREAM_ERROR_Pos 9UL
#define USB32DEV_PROT_EPI_INTR_SET_STREAM_ERROR_Msk 0x200UL
#define USB32DEV_PROT_EPI_INTR_SET_FIRST_ACK_NUMP_0_Pos 10UL
#define USB32DEV_PROT_EPI_INTR_SET_FIRST_ACK_NUMP_0_Msk 0x400UL
#define USB32DEV_PROT_EPI_INTR_SET_STREAM_ST_CHG_Pos 16UL
#define USB32DEV_PROT_EPI_INTR_SET_STREAM_ST_CHG_Msk 0x10000UL
#define USB32DEV_PROT_EPI_INTR_SET_FLOWCONTROL_EXIT_Pos 17UL
#define USB32DEV_PROT_EPI_INTR_SET_FLOWCONTROL_EXIT_Msk 0x20000UL
#define USB32DEV_PROT_EPI_INTR_SET_SPARE_REG_Pos 18UL
#define USB32DEV_PROT_EPI_INTR_SET_SPARE_REG_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPI_INTR_MASK */
#define USB32DEV_PROT_EPI_INTR_MASK_COMMIT_MASK_Pos 0UL
#define USB32DEV_PROT_EPI_INTR_MASK_COMMIT_MASK_Msk 0x1UL
#define USB32DEV_PROT_EPI_INTR_MASK_RETRY_MASK_Pos 1UL
#define USB32DEV_PROT_EPI_INTR_MASK_RETRY_MASK_Msk 0x2UL
#define USB32DEV_PROT_EPI_INTR_MASK_FLOWCONTROL_MASK_Pos 2UL
#define USB32DEV_PROT_EPI_INTR_MASK_FLOWCONTROL_MASK_Msk 0x4UL
#define USB32DEV_PROT_EPI_INTR_MASK_ZERO_MASK_Pos 4UL
#define USB32DEV_PROT_EPI_INTR_MASK_ZERO_MASK_Msk 0x10UL
#define USB32DEV_PROT_EPI_INTR_MASK_SHORT_MASK_Pos 5UL
#define USB32DEV_PROT_EPI_INTR_MASK_SHORT_MASK_Msk 0x20UL
#define USB32DEV_PROT_EPI_INTR_MASK_OOSERR_MASK_Pos 6UL
#define USB32DEV_PROT_EPI_INTR_MASK_OOSERR_MASK_Msk 0x40UL
#define USB32DEV_PROT_EPI_INTR_MASK_DBTERM_MASK_Pos 8UL
#define USB32DEV_PROT_EPI_INTR_MASK_DBTERM_MASK_Msk 0x100UL
#define USB32DEV_PROT_EPI_INTR_MASK_STREAM_ERROR_MASK_Pos 9UL
#define USB32DEV_PROT_EPI_INTR_MASK_STREAM_ERROR_MASK_Msk 0x200UL
#define USB32DEV_PROT_EPI_INTR_MASK_FIRST_ACK_NUMP_0_MASK_Pos 10UL
#define USB32DEV_PROT_EPI_INTR_MASK_FIRST_ACK_NUMP_0_MASK_Msk 0x400UL
#define USB32DEV_PROT_EPI_INTR_MASK_STREAM_ST_CHG_MASK_Pos 16UL
#define USB32DEV_PROT_EPI_INTR_MASK_STREAM_ST_CHG_MASK_Msk 0x10000UL
#define USB32DEV_PROT_EPI_INTR_MASK_FLOWCONTROL_EXIT_MASK_Pos 17UL
#define USB32DEV_PROT_EPI_INTR_MASK_FLOWCONTROL_EXIT_MASK_Msk 0x20000UL
#define USB32DEV_PROT_EPI_INTR_MASK_SPARE_REG_MASK_Pos 18UL
#define USB32DEV_PROT_EPI_INTR_MASK_SPARE_REG_MASK_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPI_INTR_MASKED */
#define USB32DEV_PROT_EPI_INTR_MASKED_COMMIT_MASKED_Pos 0UL
#define USB32DEV_PROT_EPI_INTR_MASKED_COMMIT_MASKED_Msk 0x1UL
#define USB32DEV_PROT_EPI_INTR_MASKED_RETRY_MASKED_Pos 1UL
#define USB32DEV_PROT_EPI_INTR_MASKED_RETRY_MASKED_Msk 0x2UL
#define USB32DEV_PROT_EPI_INTR_MASKED_FLOWCONTROL_MASKED_Pos 2UL
#define USB32DEV_PROT_EPI_INTR_MASKED_FLOWCONTROL_MASKED_Msk 0x4UL
#define USB32DEV_PROT_EPI_INTR_MASKED_ZERO_MASKED_Pos 4UL
#define USB32DEV_PROT_EPI_INTR_MASKED_ZERO_MASKED_Msk 0x10UL
#define USB32DEV_PROT_EPI_INTR_MASKED_SHORT_MASKED_Pos 5UL
#define USB32DEV_PROT_EPI_INTR_MASKED_SHORT_MASKED_Msk 0x20UL
#define USB32DEV_PROT_EPI_INTR_MASKED_OOSERR_MASKED_Pos 6UL
#define USB32DEV_PROT_EPI_INTR_MASKED_OOSERR_MASKED_Msk 0x40UL
#define USB32DEV_PROT_EPI_INTR_MASKED_DBTERM_MASKED_Pos 8UL
#define USB32DEV_PROT_EPI_INTR_MASKED_DBTERM_MASKED_Msk 0x100UL
#define USB32DEV_PROT_EPI_INTR_MASKED_STREAM_ERROR_MASKED_Pos 9UL
#define USB32DEV_PROT_EPI_INTR_MASKED_STREAM_ERROR_MASKED_Msk 0x200UL
#define USB32DEV_PROT_EPI_INTR_MASKED_FIRST_ACK_NUMP_0_MASKED_Pos 10UL
#define USB32DEV_PROT_EPI_INTR_MASKED_FIRST_ACK_NUMP_0_MASKED_Msk 0x400UL
#define USB32DEV_PROT_EPI_INTR_MASKED_STREAM_ST_CHG_MASKED_Pos 16UL
#define USB32DEV_PROT_EPI_INTR_MASKED_STREAM_ST_CHG_MASKED_Msk 0x10000UL
#define USB32DEV_PROT_EPI_INTR_MASKED_FLOWCONTROL_EXIT_MASKED_Pos 17UL
#define USB32DEV_PROT_EPI_INTR_MASKED_FLOWCONTROL_EXIT_MASKED_Msk 0x20000UL
#define USB32DEV_PROT_EPI_INTR_MASKED_SPARE_REG_MASKED_Pos 18UL
#define USB32DEV_PROT_EPI_INTR_MASKED_SPARE_REG_MASKED_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPI_CS1 */
#define USB32DEV_PROT_EPI_CS1_VALID_Pos 0UL
#define USB32DEV_PROT_EPI_CS1_VALID_Msk 0x1UL
#define USB32DEV_PROT_EPI_CS1_NRDY_Pos 1UL
#define USB32DEV_PROT_EPI_CS1_NRDY_Msk 0x2UL
#define USB32DEV_PROT_EPI_CS1_STALL_Pos 2UL
#define USB32DEV_PROT_EPI_CS1_STALL_Msk 0x4UL
#define USB32DEV_PROT_EPI_CS1_STREAM_EN_Pos 3UL
#define USB32DEV_PROT_EPI_CS1_STREAM_EN_Msk 0x8UL
#define USB32DEV_PROT_EPI_CS1_EP_RESET_Pos 4UL
#define USB32DEV_PROT_EPI_CS1_EP_RESET_Msk 0x10UL
#define USB32DEV_PROT_EPI_CS1_ERDY_FLOWCONTROL_Pos 8UL
#define USB32DEV_PROT_EPI_CS1_ERDY_FLOWCONTROL_Msk 0x100UL
/* USB32DEV_PROT.PROT_EPI_CS2 */
#define USB32DEV_PROT_EPI_CS2_EPI_TYPE_Pos 0UL
#define USB32DEV_PROT_EPI_CS2_EPI_TYPE_Msk 0x3UL
#define USB32DEV_PROT_EPI_CS2_SPARE_Pos 2UL
#define USB32DEV_PROT_EPI_CS2_SPARE_Msk 0xFCUL
#define USB32DEV_PROT_EPI_CS2_MAXBURST_Pos 8UL
#define USB32DEV_PROT_EPI_CS2_MAXBURST_Msk 0xF00UL
#define USB32DEV_PROT_EPI_CS2_ISOINPKS_Pos 17UL
#define USB32DEV_PROT_EPI_CS2_ISOINPKS_Msk 0x1FE0000UL
#define USB32DEV_PROT_EPI_CS2_BINTERVAL_Pos 25UL
#define USB32DEV_PROT_EPI_CS2_BINTERVAL_Msk 0x1E000000UL
/* USB32DEV_PROT.PROT_EPI_UNMAPPED_STREAM */
#define USB32DEV_PROT_EPI_UNMAPPED_STREAM_SPSM_STATE_Pos 16UL
#define USB32DEV_PROT_EPI_UNMAPPED_STREAM_SPSM_STATE_Msk 0xF0000UL
#define USB32DEV_PROT_EPI_UNMAPPED_STREAM_COMMAND_Pos 30UL
#define USB32DEV_PROT_EPI_UNMAPPED_STREAM_COMMAND_Msk 0x40000000UL
/* USB32DEV_PROT.PROT_EPI_MAPPED_STREAM */
#define USB32DEV_PROT_EPI_MAPPED_STREAM_STREAM_ID_Pos 0UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_STREAM_ID_Msk 0xFFFFUL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_EP_NUMBER_Pos 16UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_EP_NUMBER_Msk 0xF0000UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_UNMAPPED_Pos 29UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_UNMAPPED_Msk 0x20000000UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_UNMAP_Pos 30UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_UNMAP_Msk 0x40000000UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_ENABLE_Pos 31UL
#define USB32DEV_PROT_EPI_MAPPED_STREAM_ENABLE_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_EPO_INTR */
#define USB32DEV_PROT_EPO_INTR_COMMIT_Pos 0UL
#define USB32DEV_PROT_EPO_INTR_COMMIT_Msk 0x1UL
#define USB32DEV_PROT_EPO_INTR_RETRY_Pos 1UL
#define USB32DEV_PROT_EPO_INTR_RETRY_Msk 0x2UL
#define USB32DEV_PROT_EPO_INTR_FLOWCONTROL_Pos 2UL
#define USB32DEV_PROT_EPO_INTR_FLOWCONTROL_Msk 0x4UL
#define USB32DEV_PROT_EPO_INTR_ZERO_Pos 4UL
#define USB32DEV_PROT_EPO_INTR_ZERO_Msk 0x10UL
#define USB32DEV_PROT_EPO_INTR_SHORT_Pos 5UL
#define USB32DEV_PROT_EPO_INTR_SHORT_Msk 0x20UL
#define USB32DEV_PROT_EPO_INTR_OOSERR_Pos 6UL
#define USB32DEV_PROT_EPO_INTR_OOSERR_Msk 0x40UL
#define USB32DEV_PROT_EPO_INTR_STREAM_ERROR_Pos 9UL
#define USB32DEV_PROT_EPO_INTR_STREAM_ERROR_Msk 0x200UL
#define USB32DEV_PROT_EPO_INTR_PP_ZERO_Pos 11UL
#define USB32DEV_PROT_EPO_INTR_PP_ZERO_Msk 0x800UL
#define USB32DEV_PROT_EPO_INTR_EOB_ONE_Pos 12UL
#define USB32DEV_PROT_EPO_INTR_EOB_ONE_Msk 0x1000UL
#define USB32DEV_PROT_EPO_INTR_STREAM_ST_CHG_Pos 16UL
#define USB32DEV_PROT_EPO_INTR_STREAM_ST_CHG_Msk 0x10000UL
#define USB32DEV_PROT_EPO_INTR_FLOWCONTROL_EXIT_Pos 17UL
#define USB32DEV_PROT_EPO_INTR_FLOWCONTROL_EXIT_Msk 0x20000UL
#define USB32DEV_PROT_EPO_INTR_SPARE_REG_Pos 18UL
#define USB32DEV_PROT_EPO_INTR_SPARE_REG_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPO_INTR_SET */
#define USB32DEV_PROT_EPO_INTR_SET_COMMIT_Pos 0UL
#define USB32DEV_PROT_EPO_INTR_SET_COMMIT_Msk 0x1UL
#define USB32DEV_PROT_EPO_INTR_SET_RETRY_Pos 1UL
#define USB32DEV_PROT_EPO_INTR_SET_RETRY_Msk 0x2UL
#define USB32DEV_PROT_EPO_INTR_SET_FLOWCONTROL_Pos 2UL
#define USB32DEV_PROT_EPO_INTR_SET_FLOWCONTROL_Msk 0x4UL
#define USB32DEV_PROT_EPO_INTR_SET_ZERO_Pos 4UL
#define USB32DEV_PROT_EPO_INTR_SET_ZERO_Msk 0x10UL
#define USB32DEV_PROT_EPO_INTR_SET_SHORT_Pos 5UL
#define USB32DEV_PROT_EPO_INTR_SET_SHORT_Msk 0x20UL
#define USB32DEV_PROT_EPO_INTR_SET_OOSERR_Pos 6UL
#define USB32DEV_PROT_EPO_INTR_SET_OOSERR_Msk 0x40UL
#define USB32DEV_PROT_EPO_INTR_SET_STREAM_ERROR_Pos 9UL
#define USB32DEV_PROT_EPO_INTR_SET_STREAM_ERROR_Msk 0x200UL
#define USB32DEV_PROT_EPO_INTR_SET_PP_ZERO_Pos 11UL
#define USB32DEV_PROT_EPO_INTR_SET_PP_ZERO_Msk 0x800UL
#define USB32DEV_PROT_EPO_INTR_SET_EOB_ONE_Pos 12UL
#define USB32DEV_PROT_EPO_INTR_SET_EOB_ONE_Msk 0x1000UL
#define USB32DEV_PROT_EPO_INTR_SET_STREAM_ST_CHG_Pos 16UL
#define USB32DEV_PROT_EPO_INTR_SET_STREAM_ST_CHG_Msk 0x10000UL
#define USB32DEV_PROT_EPO_INTR_SET_FLOWCONTROL_EXIT_Pos 17UL
#define USB32DEV_PROT_EPO_INTR_SET_FLOWCONTROL_EXIT_Msk 0x20000UL
#define USB32DEV_PROT_EPO_INTR_SET_SPARE_REG_Pos 18UL
#define USB32DEV_PROT_EPO_INTR_SET_SPARE_REG_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPO_INTR_MASK */
#define USB32DEV_PROT_EPO_INTR_MASK_COMMIT_MASK_Pos 0UL
#define USB32DEV_PROT_EPO_INTR_MASK_COMMIT_MASK_Msk 0x1UL
#define USB32DEV_PROT_EPO_INTR_MASK_RETRY_MASK_Pos 1UL
#define USB32DEV_PROT_EPO_INTR_MASK_RETRY_MASK_Msk 0x2UL
#define USB32DEV_PROT_EPO_INTR_MASK_FLOWCONTROL_MASK_Pos 2UL
#define USB32DEV_PROT_EPO_INTR_MASK_FLOWCONTROL_MASK_Msk 0x4UL
#define USB32DEV_PROT_EPO_INTR_MASK_ZERO_MASK_Pos 4UL
#define USB32DEV_PROT_EPO_INTR_MASK_ZERO_MASK_Msk 0x10UL
#define USB32DEV_PROT_EPO_INTR_MASK_SHORT_MASK_Pos 5UL
#define USB32DEV_PROT_EPO_INTR_MASK_SHORT_MASK_Msk 0x20UL
#define USB32DEV_PROT_EPO_INTR_MASK_OOSERR_MASK_Pos 6UL
#define USB32DEV_PROT_EPO_INTR_MASK_OOSERR_MASK_Msk 0x40UL
#define USB32DEV_PROT_EPO_INTR_MASK_STREAM_ERROR_MASK_Pos 9UL
#define USB32DEV_PROT_EPO_INTR_MASK_STREAM_ERROR_MASK_Msk 0x200UL
#define USB32DEV_PROT_EPO_INTR_MASK_PP_ZERO_MASK_Pos 11UL
#define USB32DEV_PROT_EPO_INTR_MASK_PP_ZERO_MASK_Msk 0x800UL
#define USB32DEV_PROT_EPO_INTR_MASK_EOB_ONE_MASK_Pos 12UL
#define USB32DEV_PROT_EPO_INTR_MASK_EOB_ONE_MASK_Msk 0x1000UL
#define USB32DEV_PROT_EPO_INTR_MASK_STREAM_ST_CHG_MASK_Pos 16UL
#define USB32DEV_PROT_EPO_INTR_MASK_STREAM_ST_CHG_MASK_Msk 0x10000UL
#define USB32DEV_PROT_EPO_INTR_MASK_FLOWCONTROL_EXIT_MASK_Pos 17UL
#define USB32DEV_PROT_EPO_INTR_MASK_FLOWCONTROL_EXIT_MASK_Msk 0x20000UL
#define USB32DEV_PROT_EPO_INTR_MASK_SPARE_REG_MASK_Pos 18UL
#define USB32DEV_PROT_EPO_INTR_MASK_SPARE_REG_MASK_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPO_INTR_MASKED */
#define USB32DEV_PROT_EPO_INTR_MASKED_COMMIT_MASKED_Pos 0UL
#define USB32DEV_PROT_EPO_INTR_MASKED_COMMIT_MASKED_Msk 0x1UL
#define USB32DEV_PROT_EPO_INTR_MASKED_RETRY_MASKED_Pos 1UL
#define USB32DEV_PROT_EPO_INTR_MASKED_RETRY_MASKED_Msk 0x2UL
#define USB32DEV_PROT_EPO_INTR_MASKED_FLOWCONTROL_MASKED_Pos 2UL
#define USB32DEV_PROT_EPO_INTR_MASKED_FLOWCONTROL_MASKED_Msk 0x4UL
#define USB32DEV_PROT_EPO_INTR_MASKED_ZERO_MASKED_Pos 4UL
#define USB32DEV_PROT_EPO_INTR_MASKED_ZERO_MASKED_Msk 0x10UL
#define USB32DEV_PROT_EPO_INTR_MASKED_SHORT_MASKED_Pos 5UL
#define USB32DEV_PROT_EPO_INTR_MASKED_SHORT_MASKED_Msk 0x20UL
#define USB32DEV_PROT_EPO_INTR_MASKED_OOSERR_MASKED_Pos 6UL
#define USB32DEV_PROT_EPO_INTR_MASKED_OOSERR_MASKED_Msk 0x40UL
#define USB32DEV_PROT_EPO_INTR_MASKED_STREAM_ERROR_MASKED_Pos 9UL
#define USB32DEV_PROT_EPO_INTR_MASKED_STREAM_ERROR_MASKED_Msk 0x200UL
#define USB32DEV_PROT_EPO_INTR_MASKED_PP_ZERO_MASKED_Pos 11UL
#define USB32DEV_PROT_EPO_INTR_MASKED_PP_ZERO_MASKED_Msk 0x800UL
#define USB32DEV_PROT_EPO_INTR_MASKED_EOB_ONE_MASKED_Pos 12UL
#define USB32DEV_PROT_EPO_INTR_MASKED_EOB_ONE_MASKED_Msk 0x1000UL
#define USB32DEV_PROT_EPO_INTR_MASKED_STREAM_ST_CHG_MASKED_Pos 16UL
#define USB32DEV_PROT_EPO_INTR_MASKED_STREAM_ST_CHG_MASKED_Msk 0x10000UL
#define USB32DEV_PROT_EPO_INTR_MASKED_FLOWCONTROL_EXIT_MASKED_Pos 17UL
#define USB32DEV_PROT_EPO_INTR_MASKED_FLOWCONTROL_EXIT_MASKED_Msk 0x20000UL
#define USB32DEV_PROT_EPO_INTR_MASKED_SPARE_REG_MASKED_Pos 18UL
#define USB32DEV_PROT_EPO_INTR_MASKED_SPARE_REG_MASKED_Msk 0x40000UL
/* USB32DEV_PROT.PROT_EPO_CS1 */
#define USB32DEV_PROT_EPO_CS1_VALID_Pos 0UL
#define USB32DEV_PROT_EPO_CS1_VALID_Msk 0x1UL
#define USB32DEV_PROT_EPO_CS1_NRDY_Pos 1UL
#define USB32DEV_PROT_EPO_CS1_NRDY_Msk 0x2UL
#define USB32DEV_PROT_EPO_CS1_STALL_Pos 2UL
#define USB32DEV_PROT_EPO_CS1_STALL_Msk 0x4UL
#define USB32DEV_PROT_EPO_CS1_STREAM_EN_Pos 3UL
#define USB32DEV_PROT_EPO_CS1_STREAM_EN_Msk 0x8UL
#define USB32DEV_PROT_EPO_CS1_EP_RESET_Pos 4UL
#define USB32DEV_PROT_EPO_CS1_EP_RESET_Msk 0x10UL
#define USB32DEV_PROT_EPO_CS1_EN_PP_ZERO_DET_Pos 6UL
#define USB32DEV_PROT_EPO_CS1_EN_PP_ZERO_DET_Msk 0x40UL
#define USB32DEV_PROT_EPO_CS1_EN_EOB_ONE_DET_Pos 7UL
#define USB32DEV_PROT_EPO_CS1_EN_EOB_ONE_DET_Msk 0x80UL
#define USB32DEV_PROT_EPO_CS1_ERDY_FLOWCONTROL_Pos 8UL
#define USB32DEV_PROT_EPO_CS1_ERDY_FLOWCONTROL_Msk 0x100UL
/* USB32DEV_PROT.PROT_EPO_CS2 */
#define USB32DEV_PROT_EPO_CS2_EPO_TYPE_Pos 0UL
#define USB32DEV_PROT_EPO_CS2_EPO_TYPE_Msk 0x3UL
#define USB32DEV_PROT_EPO_CS2_SPARE_Pos 2UL
#define USB32DEV_PROT_EPO_CS2_SPARE_Msk 0xFCUL
#define USB32DEV_PROT_EPO_CS2_MAXBURST_Pos 8UL
#define USB32DEV_PROT_EPO_CS2_MAXBURST_Msk 0xF00UL
#define USB32DEV_PROT_EPO_CS2_ISOINPKS_Pos 17UL
#define USB32DEV_PROT_EPO_CS2_ISOINPKS_Msk 0x1FE0000UL
/* USB32DEV_PROT.PROT_EPO_UNMAPPED_STREAM */
#define USB32DEV_PROT_EPO_UNMAPPED_STREAM_SPSM_STATE_Pos 16UL
#define USB32DEV_PROT_EPO_UNMAPPED_STREAM_SPSM_STATE_Msk 0xF0000UL
#define USB32DEV_PROT_EPO_UNMAPPED_STREAM_COMMAND_Pos 30UL
#define USB32DEV_PROT_EPO_UNMAPPED_STREAM_COMMAND_Msk 0x40000000UL
/* USB32DEV_PROT.PROT_EPO_MAPPED_STREAM */
#define USB32DEV_PROT_EPO_MAPPED_STREAM_STREAM_ID_Pos 0UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_STREAM_ID_Msk 0xFFFFUL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_EP_NUMBER_Pos 16UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_EP_NUMBER_Msk 0xF0000UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_UNMAPPED_Pos 29UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_UNMAPPED_Msk 0x20000000UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_UNMAP_Pos 30UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_UNMAP_Msk 0x40000000UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_ENABLE_Pos 31UL
#define USB32DEV_PROT_EPO_MAPPED_STREAM_ENABLE_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_SUBLINK_DEV_NNOTIFICATION */
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_EN_SUBLINK_Pos 0UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_EN_SUBLINK_Msk 0x1UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_EN_SUBLINK_SET_ADDR0_Pos 1UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_EN_SUBLINK_SET_ADDR0_Msk 0x2UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_LSE_Pos 2UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_LSE_Msk 0xCUL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_ST_Pos 4UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_ST_Msk 0x30UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_LANES_Pos 6UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_LANES_Msk 0x3C0UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_LP_Pos 10UL
#define USB32DEV_PROT_SUBLINK_DEV_NNOTIFICATION_LP_Msk 0xC00UL
/* USB32DEV_PROT.PROT_SUBLINK_LSM */
#define USB32DEV_PROT_SUBLINK_LSM_RX_LSM_Pos 0UL
#define USB32DEV_PROT_SUBLINK_LSM_RX_LSM_Msk 0xFFFFUL
#define USB32DEV_PROT_SUBLINK_LSM_TX_LSM_Pos 16UL
#define USB32DEV_PROT_SUBLINK_LSM_TX_LSM_Msk 0xFFFF0000UL
/* USB32DEV_PROT.PROT_PTM_CONFIG */
#define USB32DEV_PROT_PTM_CONFIG_LDM_ENABLE_Pos 0UL
#define USB32DEV_PROT_PTM_CONFIG_LDM_ENABLE_Msk 0x1UL
#define USB32DEV_PROT_PTM_CONFIG_COUNTER_CFG_Pos 1UL
#define USB32DEV_PROT_PTM_CONFIG_COUNTER_CFG_Msk 0x2UL
#define USB32DEV_PROT_PTM_CONFIG_WAIT_ITP_Pos 2UL
#define USB32DEV_PROT_PTM_CONFIG_WAIT_ITP_Msk 0x4UL
#define USB32DEV_PROT_PTM_CONFIG_START_Pos 8UL
#define USB32DEV_PROT_PTM_CONFIG_START_Msk 0x100UL
#define USB32DEV_PROT_PTM_CONFIG_STATE_CHANGE_SEL_Pos 9UL
#define USB32DEV_PROT_PTM_CONFIG_STATE_CHANGE_SEL_Msk 0xE00UL
#define USB32DEV_PROT_PTM_CONFIG_LMP_SEND_Pos 31UL
#define USB32DEV_PROT_PTM_CONFIG_LMP_SEND_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_FW_PTM_COUNTER */
#define USB32DEV_PROT_FW_PTM_COUNTER_LOAD_Pos 0UL
#define USB32DEV_PROT_FW_PTM_COUNTER_LOAD_Msk 0x1UL
#define USB32DEV_PROT_FW_PTM_COUNTER_VALUE_Pos 1UL
#define USB32DEV_PROT_FW_PTM_COUNTER_VALUE_Msk 0xFFFFFFEUL
/* USB32DEV_PROT.PROT_HW_PTM_COUNTER */
#define USB32DEV_PROT_HW_PTM_COUNTER_READ_Pos 0UL
#define USB32DEV_PROT_HW_PTM_COUNTER_READ_Msk 0x1UL
#define USB32DEV_PROT_HW_PTM_COUNTER_BIC_Pos 1UL
#define USB32DEV_PROT_HW_PTM_COUNTER_BIC_Msk 0x7FFEUL
#define USB32DEV_PROT_HW_PTM_COUNTER_DC_Pos 15UL
#define USB32DEV_PROT_HW_PTM_COUNTER_DC_Msk 0xFFF8000UL
/* USB32DEV_PROT.PROT_PTM_STATUS */
#define USB32DEV_PROT_PTM_STATUS_LDM_REQ_STATE_Pos 0UL
#define USB32DEV_PROT_PTM_STATUS_LDM_REQ_STATE_Msk 0x7UL
#define USB32DEV_PROT_PTM_STATUS_LDMS_TX_Pos 3UL
#define USB32DEV_PROT_PTM_STATUS_LDMS_TX_Msk 0x18UL
/* USB32DEV_PROT.PROT_PTM_T1 */
#define USB32DEV_PROT_PTM_T1_TIME_Pos 0UL
#define USB32DEV_PROT_PTM_T1_TIME_Msk 0x7FFFFFFUL
/* USB32DEV_PROT.PROT_PTM_T4 */
#define USB32DEV_PROT_PTM_T4_TIME_Pos 0UL
#define USB32DEV_PROT_PTM_T4_TIME_Msk 0x7FFFFFFUL
/* USB32DEV_PROT.PROT_LMP_LDM_RESPONSE */
#define USB32DEV_PROT_LMP_LDM_RESPONSE_LDM_TYPE_Pos 0UL
#define USB32DEV_PROT_LMP_LDM_RESPONSE_LDM_TYPE_Msk 0x7UL
#define USB32DEV_PROT_LMP_LDM_RESPONSE_LDMS_RX_Pos 3UL
#define USB32DEV_PROT_LMP_LDM_RESPONSE_LDMS_RX_Msk 0x18UL
#define USB32DEV_PROT_LMP_LDM_RESPONSE_LDM_RESP_DELAY_Pos 15UL
#define USB32DEV_PROT_LMP_LDM_RESPONSE_LDM_RESP_DELAY_Msk 0x1FFF8000UL
/* USB32DEV_PROT.PROT_EPI_ENHANCE */
#define USB32DEV_PROT_EPI_ENHANCE_EN_TERM_NUMP0_RTY1_Pos 0UL
#define USB32DEV_PROT_EPI_ENHANCE_EN_TERM_NUMP0_RTY1_Msk 0x1UL
#define USB32DEV_PROT_EPI_ENHANCE_EN_EOB_ZLP_Pos 1UL
#define USB32DEV_PROT_EPI_ENHANCE_EN_EOB_ZLP_Msk 0x2UL
#define USB32DEV_PROT_EPI_ENHANCE_EN_EOB_SHORT_Pos 31UL
#define USB32DEV_PROT_EPI_ENHANCE_EN_EOB_SHORT_Msk 0x80000000UL
/* USB32DEV_PROT.PROT_ENHANCE */
#define USB32DEV_PROT_ENHANCE_EN_DPH_ERR_DET_Pos 0UL
#define USB32DEV_PROT_ENHANCE_EN_DPH_ERR_DET_Msk 0x1UL
#define USB32DEV_PROT_ENHANCE_DISABLE_STREAM_ERDY_Pos 1UL
#define USB32DEV_PROT_ENHANCE_DISABLE_STREAM_ERDY_Msk 0x2UL
#define USB32DEV_PROT_ENHANCE_STREAM_ERDY_ONLY_DISABLED_ST_Pos 2UL
#define USB32DEV_PROT_ENHANCE_STREAM_ERDY_ONLY_DISABLED_ST_Msk 0x4UL
#define USB32DEV_PROT_ENHANCE_EN_REJECTED_ERDY_Pos 3UL
#define USB32DEV_PROT_ENHANCE_EN_REJECTED_ERDY_Msk 0x8UL
#define USB32DEV_PROT_ENHANCE_DISABLE_SI_MODE_Pos 4UL
#define USB32DEV_PROT_ENHANCE_DISABLE_SI_MODE_Msk 0x10UL
#define USB32DEV_PROT_ENHANCE_EN_EGRS_ISO_SEQ_MATCH_Pos 5UL
#define USB32DEV_PROT_ENHANCE_EN_EGRS_ISO_SEQ_MATCH_Msk 0x20UL
#define USB32DEV_PROT_ENHANCE_ITP_RST_SEQ_NUM_CONDITION_Pos 6UL
#define USB32DEV_PROT_ENHANCE_ITP_RST_SEQ_NUM_CONDITION_Msk 0x3C0UL
#define USB32DEV_PROT_ENHANCE_ITP_PTM_INTR_CONDITION_Pos 10UL
#define USB32DEV_PROT_ENHANCE_ITP_PTM_INTR_CONDITION_Msk 0x3C00UL
#define USB32DEV_PROT_ENHANCE_INGRESS_FULL_COUNTER_Pos 14UL
#define USB32DEV_PROT_ENHANCE_INGRESS_FULL_COUNTER_Msk 0x1C000UL
#define USB32DEV_PROT_ENHANCE_STREAM_READ_DIR_Pos 17UL
#define USB32DEV_PROT_ENHANCE_STREAM_READ_DIR_Msk 0x20000UL
#define USB32DEV_PROT_ENHANCE_STREAM_READ_END_POINT_Pos 18UL
#define USB32DEV_PROT_ENHANCE_STREAM_READ_END_POINT_Msk 0x3C0000UL
#define USB32DEV_PROT_ENHANCE_STREAM_READ_STATE_Pos 22UL
#define USB32DEV_PROT_ENHANCE_STREAM_READ_STATE_Msk 0x3FC00000UL
#define USB32DEV_PROT_ENHANCE_RESET_SCH_TYPE1_Pos 30UL
#define USB32DEV_PROT_ENHANCE_RESET_SCH_TYPE1_Msk 0x40000000UL
#define USB32DEV_PROT_ENHANCE_RESET_SCH_TYPE2_Pos 31UL
#define USB32DEV_PROT_ENHANCE_RESET_SCH_TYPE2_Msk 0x80000000UL


/* USB32DEV_PHYSS_USB40PHY_TOP.TOP_CTRL_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PWR_GOOD_CORE_RX_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PWR_GOOD_CORE_RX_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PWR_GOOD_CORE_PLL_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PWR_GOOD_CORE_PLL_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_VBUS_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_VBUS_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PLL_RSTB_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PLL_RSTB_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_LFPS_CLK_DIV_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_LFPS_CLK_DIV_Msk 0xF00UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_TX_SHIFT_CLK_DIV_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_TX_SHIFT_CLK_DIV_Msk 0xF000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PHYSS_EN_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_PHYSS_EN_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_RX_EQ_TRAINING_FLAG_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_REG_RX_EQ_TRAINING_FLAG_Msk 0x20000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_PCLK_EN_Pos 19UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_PCLK_EN_Msk 0x80000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_RATE_CHANGE_DONE_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_RATE_CHANGE_DONE_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_RATE_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_RATE_Msk 0x600000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_PLL_VCCD_LEVELSHIFT_EN_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_PLL_VCCD_LEVELSHIFT_EN_Msk 0x1000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_PLL_VHS_LEVELSHIFT_EN_Pos 25UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_PLL_VHS_LEVELSHIFT_EN_Msk 0x2000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_GEN1_BERT_EN_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CTRL_0_GEN1_BERT_EN_Msk 0x10000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PCS_CTRL_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_HDR_CORRECTION_DISABLE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_HDR_CORRECTION_DISABLE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_SKP_CORRECTION_DISABLE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_SKP_CORRECTION_DISABLE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_SDS_CORRECTION_DISABLE_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_SDS_CORRECTION_DISABLE_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_CLEAR_ERR_CNTR_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_CLEAR_ERR_CNTR_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_ONEZERO_CNT_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_ONEZERO_CNT_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_TX_SER_EN_DLY_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_TX_SER_EN_DLY_Msk 0x3800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN1_EB_HALF_DEPTH_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN1_EB_HALF_DEPTH_Msk 0xF000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_EB_HALF_DEPTH_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_CTRL_0_GEN2_EB_HALF_DEPTH_Msk 0xF0000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PCS_STATUS */
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_REG_PCS_RX_ALIGNED_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_REG_PCS_RX_ALIGNED_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_REG_PCS_RX_MATCHED_CNT_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_REG_PCS_RX_MATCHED_CNT_Msk 0xFEUL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_REG_PIPE_RATE_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_REG_PIPE_RATE_Msk 0x300UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_ALIGNER_STATE_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_ALIGNER_STATE_Msk 0x7000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_CDR_LOCK_TO_REFERENCE_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_STATUS_CDR_LOCK_TO_REFERENCE_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PCS_SPARE */
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_SPARE_REG_DFT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_SPARE_REG_DFT_Msk 0xFFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_SPARE_REG_SPARE0_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_SPARE_REG_SPARE0_Msk 0xFFF000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_SPARE_REG_SPARE1_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PCS_SPARE_REG_SPARE1_Msk 0xFF000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.ERR_STATUS */
#define USB32DEV_PHYSS_USB40PHY_TOP_ERR_STATUS_HDR_ERR_CORRECTED_CNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ERR_STATUS_HDR_ERR_CORRECTED_CNT_Msk 0xFFFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_ERR_STATUS_SKP_ERR_CORRECTED_CNT_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ERR_STATUS_SKP_ERR_CORRECTED_CNT_Msk 0x3FF0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ERR_STATUS_SDS_ERR_CORRECTED_CNT_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ERR_STATUS_SDS_ERR_CORRECTED_CNT_Msk 0x3C000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PIPE_OVERRIDE_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_PIPE_ENCDEC_BYPASS_OVRD_EN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_PIPE_ENCDEC_BYPASS_OVRD_EN_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_PIPE_ENCDEC_BYPASS_OVRD_VALUE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_PIPE_ENCDEC_BYPASS_OVRD_VALUE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_EB_MODE_OVRD_EN_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_EB_MODE_OVRD_EN_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_EB_MODE_OVRD_VALUE_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_EB_MODE_OVRD_VALUE_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_DETECTRX_OVRD_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_DETECTRX_OVRD_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_DETECTRX_OVRD_VALUE_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_DETECTRX_OVRD_VALUE_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ELECTIDLE_OVRD_EN_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ELECTIDLE_OVRD_EN_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ELECTIDLE_OVRD_VALUE_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ELECTIDLE_OVRD_VALUE_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ONE_ZEROS_OVRD_EN_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ONE_ZEROS_OVRD_EN_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ONE_ZEROS_OVRD_VALUE_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_TX_ONE_ZEROS_OVRD_VALUE_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_POLARITY_OVRD_EN_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_POLARITY_OVRD_EN_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_POLARITY_OVRD_VALUE_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_POLARITY_OVRD_VALUE_Msk 0x4000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_EQ_TRAINING_OVRD_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_EQ_TRAINING_OVRD_EN_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_EQ_TRAINING_OVRD_VALUE_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_RX_EQ_TRAINING_OVRD_VALUE_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_POWER_DOWN_OVRD_EN_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_POWER_DOWN_OVRD_EN_Msk 0x20000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_POWER_DOWN_OVRD_VALUE_Pos 18UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_POWER_DOWN_OVRD_VALUE_Msk 0x1C0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_BLOCK_ALIGN_CONTROL_OVRD_EN_Pos 30UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_BLOCK_ALIGN_CONTROL_OVRD_EN_Msk 0x40000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_BLOCK_ALIGN_CONTROL_OVRD_VALUE_Pos 31UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_OVERRIDE_0_REG_PIPE_BLOCK_ALIGN_CONTROL_OVRD_VALUE_Msk 0x80000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PIPE_STATUS */
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_RXVALID_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_RXVALID_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_PHY_STATUS_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_PHY_STATUS_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_RX_ELECIDLE_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_RX_ELECIDLE_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_RX_STATUS_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_RX_STATUS_Msk 0x38UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_POWER_PRESENT_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_REG_PIPE_POWER_PRESENT_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_RX_TERMINATION_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_RX_TERMINATION_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_RX_STANDBY_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_RX_STANDBY_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P0_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P0_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P1_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P1_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P2_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P2_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P3_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_STATUS_PHY_IN_P3_Msk 0x1000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RATE_CHANGE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RATE_CHANGE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P0_CHANGE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P0_CHANGE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P1_CHANGE_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P1_CHANGE_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P2_CHANGE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P2_CHANGE_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P3_CHANGE_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_P3_CHANGE_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_TX_SFT_REG_WDONE_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_TX_SFT_REG_WDONE_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_PLL_LOCKED_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_PLL_LOCKED_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_POWER_GOOD_RXA_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_POWER_GOOD_RXA_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_POWER_GOOD_RXCK_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_POWER_GOOD_RXCK_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_POWER_GOOD_RXD_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_POWER_GOOD_RXD_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_OSA_ERROR_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_OSA_ERROR_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_OSA_ALL_DONE_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_OSA_ALL_DONE_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_EQ_START_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_EQ_START_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_EYE_HIGHT_DONE_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_EYE_HIGHT_DONE_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_EYE_MON_DONE_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_RX_EYE_MON_DONE_Msk 0x4000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_LOCKED_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_LOCKED_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_PWR_GOOD_LCPLL_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_PWR_GOOD_LCPLL_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_PWR_GOOD_REF_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_PWR_GOOD_REF_Msk 0x20000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_PWR_GOOD_DIG_Pos 18UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PLL_PWR_GOOD_DIG_Msk 0x40000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_RX_ALIGNER_LOCK_Pos 19UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_RX_ALIGNER_LOCK_Msk 0x80000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_RX_DETECTED_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_RX_DETECTED_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_DECODE_ERR_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_DECODE_ERR_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_EB_OVERFLOW_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_EB_OVERFLOW_Msk 0x400000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_EB_UNDERFLOW_Pos 23UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_PCS_EB_UNDERFLOW_Msk 0x800000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_LOOPBACK_LOCK_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_LOOPBACK_LOCK_Msk 0x1000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_LOOPBACK_DONE_Pos 25UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_LOOPBACK_DONE_Msk 0x2000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_TX_IMP_CAL_DONE_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_REG_INT_TX_IMP_CAL_DONE_Msk 0x4000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SAR_DONE_Pos 27UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SAR_DONE_Msk 0x8000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_RX_TERMINATION_CHANGE_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_RX_TERMINATION_CHANGE_Msk 0x10000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_RX_STANDBY_CHANGE_Pos 29UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_RX_STANDBY_CHANGE_Msk 0x20000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR0_SET */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RATE_CHANGE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RATE_CHANGE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P0_CHANGE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P0_CHANGE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P1_CHANGE_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P1_CHANGE_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P2_CHANGE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P2_CHANGE_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P3_CHANGE_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_P3_CHANGE_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_TX_SFT_REG_WDONE_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_TX_SFT_REG_WDONE_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_PLL_LOCKED_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_PLL_LOCKED_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_POWER_GOOD_RXA_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_POWER_GOOD_RXA_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_POWER_GOOD_RXCK_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_POWER_GOOD_RXCK_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_POWER_GOOD_RXD_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_POWER_GOOD_RXD_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_OSA_ERROR_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_OSA_ERROR_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_OSA_ALL_DONE_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_OSA_ALL_DONE_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_EQ_START_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_EQ_START_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_EYE_HIGHT_DONE_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_EYE_HIGHT_DONE_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_EYE_MON_DONE_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_RX_EYE_MON_DONE_Msk 0x4000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_LOCKED_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_LOCKED_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_PWR_GOOD_LCPLL_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_PWR_GOOD_LCPLL_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_PWR_GOOD_REF_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_PWR_GOOD_REF_Msk 0x20000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_PWR_GOOD_DIG_Pos 18UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PLL_PWR_GOOD_DIG_Msk 0x40000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_RX_ALIGNER_LOCK_Pos 19UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_RX_ALIGNER_LOCK_Msk 0x80000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_RX_DETECTED_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_RX_DETECTED_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_DECODE_ERR_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_DECODE_ERR_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_EB_OVERFLOW_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_EB_OVERFLOW_Msk 0x400000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_EB_UNDERFLOW_Pos 23UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_PCS_EB_UNDERFLOW_Msk 0x800000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_LOOPBACK_LOCK_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_LOOPBACK_LOCK_Msk 0x1000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_LOOPBACK_DONE_Pos 25UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_LOOPBACK_DONE_Msk 0x2000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_TX_IMP_CAL_DONE_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_REG_INT_TX_IMP_CAL_DONE_Msk 0x4000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_SAR_DONE_Pos 27UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_SAR_DONE_Msk 0x8000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_RX_TERMINATION_CHANGE_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_RX_TERMINATION_CHANGE_Msk 0x10000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_RX_STANDBY_CHANGE_Pos 29UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_SET_RX_STANDBY_CHANGE_Msk 0x20000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR0_MASK */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RATE_CHANGE_MASK_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RATE_CHANGE_MASK_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P0_CHANGE_MASK_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P0_CHANGE_MASK_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P1_CHANGE_MASK_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P1_CHANGE_MASK_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P2_CHANGE_MASK_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P2_CHANGE_MASK_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P3_CHANGE_MASK_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_P3_CHANGE_MASK_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_TX_SFT_REG_WDONE_MASK_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_TX_SFT_REG_WDONE_MASK_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_PLL_LOCKED_MASK_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_PLL_LOCKED_MASK_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_POWER_GOOD_RXA_MASK_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_POWER_GOOD_RXA_MASK_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_POWER_GOOD_RXCK_MASK_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_POWER_GOOD_RXCK_MASK_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_POWER_GOOD_RXD_MASK_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_POWER_GOOD_RXD_MASK_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_OSA_ERROR_MASK_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_OSA_ERROR_MASK_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_OSA_ALL_DONE_MASK_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_OSA_ALL_DONE_MASK_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_EQ_START_MASK_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_EQ_START_MASK_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_EYE_HIGHT_DONE_MASK_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_EYE_HIGHT_DONE_MASK_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_EYE_MON_DONE_MASK_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_RX_EYE_MON_DONE_MASK_Msk 0x4000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_LOCKED_MASK_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_LOCKED_MASK_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_PWR_GOOD_LCPLL_MASK_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_PWR_GOOD_LCPLL_MASK_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_PWR_GOOD_REF_MASK_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_PWR_GOOD_REF_MASK_Msk 0x20000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_PWR_GOOD_DIG_MASK_Pos 18UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PLL_PWR_GOOD_DIG_MASK_Msk 0x40000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_RX_ALIGNER_LOCK_MASK_Pos 19UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_RX_ALIGNER_LOCK_MASK_Msk 0x80000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_RX_DETECTED_MASK_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_RX_DETECTED_MASK_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_DECODE_ERR_MASK_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_DECODE_ERR_MASK_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_EB_OVERFLOW_MASK_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_EB_OVERFLOW_MASK_Msk 0x400000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_EB_UNDERFLOW_MASK_Pos 23UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_PCS_EB_UNDERFLOW_MASK_Msk 0x800000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_LOOPBACK_LOCK_MASK_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_LOOPBACK_LOCK_MASK_Msk 0x1000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_LOOPBACK_DONE_MASK_Pos 25UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_LOOPBACK_DONE_MASK_Msk 0x2000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_TX_IMP_CAL_DONE_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_REG_INT_TX_IMP_CAL_DONE_Msk 0x4000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_SAR_DONE_Pos 27UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_SAR_DONE_Msk 0x8000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_RX_TERMINATION_CHANGE_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_RX_TERMINATION_CHANGE_Msk 0x10000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_RX_STANDBY_CHANGE_Pos 29UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASK_RX_STANDBY_CHANGE_Msk 0x20000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR0_MASKED */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RATE_CHANGE_MASKD_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RATE_CHANGE_MASKD_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P0_CHANGE_MASKD_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P0_CHANGE_MASKD_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P1_CHANGE_MASKD_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P1_CHANGE_MASKD_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P2_CHANGE_MASKD_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P2_CHANGE_MASKD_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P3_CHANGE_MASKD_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_P3_CHANGE_MASKD_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_TX_SFT_REG_WDONE_MASKD_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_TX_SFT_REG_WDONE_MASKD_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_PLL_LOCKED_MASKD_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_PLL_LOCKED_MASKD_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_POWER_GOOD_RXA_MASKD_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_POWER_GOOD_RXA_MASKD_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_POWER_GOOD_RXCK_MASKD_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_POWER_GOOD_RXCK_MASKD_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_POWER_GOOD_RXD_MASKD_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_POWER_GOOD_RXD_MASKD_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_OSA_ERROR_MASKD_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_OSA_ERROR_MASKD_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_OSA_ALL_DONE_MASKD_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_OSA_ALL_DONE_MASKD_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_EQ_START_MASKD_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_EQ_START_MASKD_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_EYE_HIGHT_DONE_MASKD_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_EYE_HIGHT_DONE_MASKD_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_EYE_MON_DONE_MASKD_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_RX_EYE_MON_DONE_MASKD_Msk 0x4000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_LOCKED_MASKD_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_LOCKED_MASKD_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_PWR_GOOD_LCPLL_MASKD_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_PWR_GOOD_LCPLL_MASKD_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_PWR_GOOD_REF_MASKD_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_PWR_GOOD_REF_MASKD_Msk 0x20000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_PWR_GOOD_DIG_MASKD_Pos 18UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PLL_PWR_GOOD_DIG_MASKD_Msk 0x40000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_RX_ALIGNER_LOCK_MASKD_Pos 19UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_RX_ALIGNER_LOCK_MASKD_Msk 0x80000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_RX_DETECTED_MASKD_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_RX_DETECTED_MASKD_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_DECODE_ERR_MASKD_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_DECODE_ERR_MASKD_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_EB_OVERFLOW_MASKD_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_EB_OVERFLOW_MASKD_Msk 0x400000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_EB_UNDERFLOW_MASKD_Pos 23UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_PCS_EB_UNDERFLOW_MASKD_Msk 0x800000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_LOOPBACK_LOCK_MASKD_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_LOOPBACK_LOCK_MASKD_Msk 0x1000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_LOOPBACK_DONE_MASKD_Pos 25UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_LOOPBACK_DONE_MASKD_Msk 0x2000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_TX_IMP_CAL_DONE_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_REG_INT_TX_IMP_CAL_DONE_Msk 0x4000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_SAR_DONE_Pos 27UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_SAR_DONE_Msk 0x8000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_RX_TERMINATION_CHANGE_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_RX_TERMINATION_CHANGE_Msk 0x10000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_RX_STANDBY_CHANGE_Pos 29UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR0_MASKED_RX_STANDBY_CHANGE_Msk 0x20000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR1 */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_REG_INT_RX_LFPSDET_OUT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_REG_INT_RX_LFPSDET_OUT_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR1_SET */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_SET_REG_INT_RX_LFPSDET_OUT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_SET_REG_INT_RX_LFPSDET_OUT_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR1_MASK */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_MASK_REG_INT_RX_LFPSDET_OUT_MASK_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_MASK_REG_INT_RX_LFPSDET_OUT_MASK_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_TOP.INTR1_MASKED */
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_MASKED_REG_INT_RX_LFPSDET_OUT_MASKD_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_INTR1_MASKED_REG_INT_RX_LFPSDET_OUT_MASKD_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CTRL_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_EN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_EN_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_DRV_EN_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_DRV_EN_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_CML2CMOS_EN_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_CML2CMOS_EN_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_TX_SERIALIZER_RST_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_TX_SERIALIZER_RST_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VCPREG_EN_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VCPREG_EN_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VCPREG_SEL_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VCPREG_SEL_Msk 0x1E0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_CPCLK_SEL_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_CPCLK_SEL_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_CLK_EN_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_CLK_EN_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_CLK_SEL_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_CLK_SEL_Msk 0x7800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_DRV_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_DRV_EN_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_DRV_SEL_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_VREG_DRV_SEL_Msk 0xF0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_PULL_DN_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_PULL_DN_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_LOOPBK_EN_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_LOOPBK_EN_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_DCD_CTRL_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_DCD_CTRL_Msk 0x3C00000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_DETECTRX_TRIM_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_DETECTRX_TRIM_Msk 0xC000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_ELECIDLE_DRV_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_0_REG_TX_AFE_TX_ELECIDLE_DRV_Msk 0x30000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CTRL_1 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_ICAL_TRIM_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_ICAL_TRIM_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_BURNIN_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_BURNIN_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_ADFT_EN_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_ADFT_EN_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_ADFT_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_ADFT_Msk 0x3E00UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_DDFT_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CTRL_1_REG_TX_AFE_TX_DDFT_Msk 0x7C000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_RXDETECT_CNT */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_RXDETECT_CNT_RXDETECT_WAIT_CNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_RXDETECT_CNT_RXDETECT_WAIT_CNT_Msk 0xFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_RXDETECT_CNT_PULLUP_ASSERT_DLY_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_RXDETECT_CNT_PULLUP_ASSERT_DLY_Msk 0xFF00UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_OVERRIDE_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_PULL_UP_OVRD_EN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_PULL_UP_OVRD_EN_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_PULL_UP_OVRD_VALUE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_PULL_UP_OVRD_VALUE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_SER_EN_OVRD_EN_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_SER_EN_OVRD_EN_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_SER_EN_OVRD_VALUE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_SER_EN_OVRD_VALUE_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_HSCLK_DRV_EN_OVRD_EN_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_HSCLK_DRV_EN_OVRD_EN_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_HSCLK_DRV_EN_OVRD_VALUE_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_HSCLK_DRV_EN_OVRD_VALUE_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_TX_AFE_SER_RESET_OVRD_EN_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_TX_AFE_SER_RESET_OVRD_EN_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_TX_AFE_SER_RESET_OVRD_VALUE_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_TX_AFE_SER_RESET_OVRD_VALUE_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_DETECTRX_OVRD_EN_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_DETECTRX_OVRD_EN_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_DETECTRX_OVRD_VALUE_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_DETECTRX_OVRD_VALUE_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ELECIDLE_OVRD_EN_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ELECIDLE_OVRD_EN_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ELECIDLE_OVRD_VALUE_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ELECIDLE_OVRD_VALUE_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_LFPS_OVRD_EN_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_LFPS_OVRD_EN_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_LFPS_OVRD_VALUE_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_LFPS_OVRD_VALUE_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ZTRIM_OVRD_EN_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ZTRIM_OVRD_EN_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ZTRIM_OVRD_VALUE_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_OVERRIDE_0_REG_TX_AFE_TX_ZTRIM_OVRD_VALUE_Msk 0x3E0000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_DEBUG */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_DEBUG_REG_TX_AFE_DDFT_OUT1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_DEBUG_REG_TX_AFE_DDFT_OUT1_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_DEBUG_REG_TX_AFE_DDFT_OUT2_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_DEBUG_REG_TX_AFE_DDFT_OUT2_Msk 0x2UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_STATUS */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_ZCAL_STATUS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_ZCAL_STATUS_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_ZCAL_CODE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_ZCAL_CODE_Msk 0x3EUL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_ZCAL_COMP_OUT_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_ZCAL_COMP_OUT_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_TX_DETECTRX_OUT_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_STATUS_TX_DETECTRX_OUT_Msk 0x80UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_ZTRIM */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_EN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_EN_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_MODE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_MODE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_COMP_WAIT_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_COMP_WAIT_Msk 0xCUL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_START_CODE_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_START_CODE_Msk 0x1F0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_FINAL_CODE_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_FINAL_CODE_Msk 0x3E00UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_CURR_STATE_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_ZTRIM_ZCAL_CURR_STATE_Msk 0x70000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_REG_TX_CFG_WRITE_DONE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_REG_TX_CFG_WRITE_DONE_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_0_REG_TX_CFG_SFT_W_0_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_0_REG_TX_CFG_SFT_W_0_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_1 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_1_REG_TX_CFG_SFT_W_1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_1_REG_TX_CFG_SFT_W_1_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_2 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_2_REG_TX_CFG_SFT_W_2_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_2_REG_TX_CFG_SFT_W_2_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_3 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_3_REG_TX_CFG_SFT_W_3_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_3_REG_TX_CFG_SFT_W_3_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_4 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_4_REG_TX_CFG_SFT_W_4_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_4_REG_TX_CFG_SFT_W_4_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_5 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_5_REG_TX_CFG_SFT_W_5_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_5_REG_TX_CFG_SFT_W_5_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_6 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_6_REG_TX_CFG_SFT_W_6_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_6_REG_TX_CFG_SFT_W_6_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_W_7 */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_7_REG_TX_CFG_SFT_W_7_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_W_7_REG_TX_CFG_SFT_W_7_Msk 0x3FFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.TX_AFE_CFG_SFT_R */
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_R_REG_TX_CFG_SFT_R_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_TX_AFE_CFG_SFT_R_REG_TX_CFG_SFT_R_Msk 0xFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.ADC */
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_DAC_CNTRL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_DAC_CNTRL_Msk 0xFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_MID_VAL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_MID_VAL_Msk 0xFF00UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_LDO_TRIM_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_LDO_TRIM_Msk 0xF0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_LDO_EN_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_LDO_EN_Msk 0x100000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_ISO_N_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_ISO_N_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_PD_LV_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_PD_LV_Msk 0x400000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_DFT_MUXSEL_Pos 23UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_DFT_MUXSEL_Msk 0x800000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_VSEL_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_VSEL_Msk 0x7000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_VREF_DAC_SEL_Pos 27UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_VREF_DAC_SEL_Msk 0x18000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_SAR_EN_Pos 29UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_SAR_EN_Msk 0x20000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.ADC_STATUS */
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_STATUS_CMP_OUT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_STATUS_CMP_OUT_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_STATUS_SAR_OUT_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_ADC_STATUS_SAR_OUT_Msk 0xFF00UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_LOOPBACK_CTRL_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_EN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_EN_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_NO_AFE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_NO_AFE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_START_PTTN_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_START_PTTN_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_NO_8B_10B_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_NO_8B_10B_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_PTTN_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_PTTN_Msk 0x70UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_NO_RX_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_LOOPBACK_NO_RX_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_TX_ONE_ZERO_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_TX_ONE_ZERO_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_SAMPLE_RX_AFE_OUT_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_CTRL_0_SAMPLE_RX_AFE_OUT_Msk 0x400UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_LOOPBACK_DEBUG_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_DEBUG_0_RX_AFE_DATA_0_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_DEBUG_0_RX_AFE_DATA_0_Msk 0xFFFFFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_LOOPBACK_DEBUG_1 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_DEBUG_1_RX_AFE_DATA_1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_DEBUG_1_RX_AFE_DATA_1_Msk 0xFFFFFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_LOOPBACK_STATUS_1 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_ERR_COUNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_ERR_COUNT_Msk 0xFFFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_STATUS_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_STATUS_Msk 0x30000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_DONE_Pos 18UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_DONE_Msk 0x40000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_GEN_STATE_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_GEN_STATE_Msk 0x300000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_CHK_STATE_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_STATUS_1_LOOPBACK_CHK_STATE_Msk 0xC00000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_LOOPBACK_USER_0 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_USER_0_REG_LOOPBACK_USER_0_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_USER_0_REG_LOOPBACK_USER_0_Msk 0xFFFFFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_LOOPBACK_USER_1 */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_USER_1_REG_LOOPBACK_USER_1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_LOOPBACK_USER_1_REG_LOOPBACK_USER_1_Msk 0xFFFFFFFFUL
/* USB32DEV_PHYSS_USB40PHY_TOP.PHYSS_DDFT_MUX_SEL */
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT0_SEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT0_SEL_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT0_POLARITY_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT0_POLARITY_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT1_SEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT1_SEL_Msk 0x3F00UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT1_POLARITY_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_DDFT1_POLARITY_Msk 0x8000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_HS_DDFT_EN_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_HS_DDFT_EN_Msk 0x10000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_HS_DDFT0_SEL_Pos 17UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_HS_DDFT0_SEL_Msk 0x3E0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_HS_DDFT1_SEL_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PHYSS_DDFT_MUX_SEL_HS_DDFT1_SEL_Msk 0x7C00000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.PIPE_RX_CTRL */
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_RESERVED_CFG_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_RESERVED_CFG_Msk 0xFFFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_HS_ERROR_DEBOUNCE_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_HS_ERROR_DEBOUNCE_Msk 0x1F0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_HS_DATA_ALIGN_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_HS_DATA_ALIGN_Msk 0x3E00000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_FORCE_REALIGN_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_FORCE_REALIGN_Msk 0x4000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_CDR_L2R_ON_POWER_MODE_Pos 27UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_CDR_L2R_ON_POWER_MODE_Msk 0x8000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_VCC_SEL_LFPS_DETECT_Pos 28UL
#define USB32DEV_PHYSS_USB40PHY_TOP_PIPE_RX_CTRL_VCC_SEL_LFPS_DETECT_Msk 0x10000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.LOW_POWER_CTRL */
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_LFPS_FILTER_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_LFPS_FILTER_Msk 0xFFFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_HW_CTRL_DELAY_Pos 16UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_HW_CTRL_DELAY_Msk 0x1F0000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_LFPS_MASK_ON_TX_DATA_Pos 21UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_LFPS_MASK_ON_TX_DATA_Msk 0x200000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_DISABLE_LFPS_FILTER_Pos 22UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_DISABLE_LFPS_FILTER_Msk 0x400000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_LFPS_FILTER_MODE_Pos 23UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_LFPS_FILTER_MODE_Msk 0x800000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_FORCE_CDR_LOCK_TO_DATA_Pos 24UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_FORCE_CDR_LOCK_TO_DATA_Msk 0x1000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_FORCE_CDR_LOCK_TO_REFERENCE_Pos 25UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_FORCE_CDR_LOCK_TO_REFERENCE_Msk 0x2000000UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_BYPASS_FILTER_U3_WAKEUP_Pos 26UL
#define USB32DEV_PHYSS_USB40PHY_TOP_LOW_POWER_CTRL_BYPASS_FILTER_U3_WAKEUP_Msk 0x4000000UL
/* USB32DEV_PHYSS_USB40PHY_TOP.CDR_SW_CTRL */
#define USB32DEV_PHYSS_USB40PHY_TOP_CDR_SW_CTRL_L2D_START_DELAY_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CDR_SW_CTRL_L2D_START_DELAY_Msk 0xFFFFFUL
#define USB32DEV_PHYSS_USB40PHY_TOP_CDR_SW_CTRL_L2R_START_DELAY_Pos 20UL
#define USB32DEV_PHYSS_USB40PHY_TOP_CDR_SW_CTRL_L2R_START_DELAY_Msk 0xFF00000UL


/* USB32DEV_PHYSS_USB40PHY_RX.RX_SDM_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG0_DIVF_INTEGER_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG0_DIVF_INTEGER_Msk 0xFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_SDM_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG1_DIVF_FRAC_MSB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG1_DIVF_FRAC_MSB_Msk 0xFFFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_SDM_CFG2 */
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG2_DIVF_FRAC_LSB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG2_DIVF_FRAC_LSB_Msk 0x3UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_SDM_CFG3 */
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG3_SDM_ENABLE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG3_SDM_ENABLE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG3_DITHER_EN_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG3_DITHER_EN_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG3_DITHER_GAIN_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_SDM_CFG3_DITHER_GAIN_Msk 0x1CUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_VGATAP_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_VGATAP_Msk 0x1FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_VGATAP_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_VGATAP_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_DFETAP1_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_DFETAP1_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_DFETAP1_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG0_OVRD_DFETAP1_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP2_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP2_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP2_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP2_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP3_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP3_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP3_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG1_OVRD_DFETAP3_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_CFG2 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_SLICEPOINTVALUE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_SLICEPOINTVALUE_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_SLICEPOINTSIGN1_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_SLICEPOINTSIGN1_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_SLICEPOINTSIGN2_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_SLICEPOINTSIGN2_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_VGA_MU_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_VGA_MU_Msk 0xF00UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_MU_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG2_DFE_MU_Msk 0xF000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_CFG3 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_BAL_AVRG_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_BAL_AVRG_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_BALANCE_SCALE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_BALANCE_SCALE_Msk 0x18UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_BALANCE_SCALE_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_BALANCE_SCALE_Msk 0x60UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_POLARITY_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_POLARITY_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_POLARITY_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_POLARITY_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_ADAPT_EN_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_ADAPT_EN_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_ENABLE_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_ENABLE_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_FREEZE_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_VGA_FREEZE_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_FREEZE_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_FREEZE_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_QUALIFIER_INV_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_QUALIFIER_INV_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_RESETB_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG3_DFE_RESETB_Msk 0x4000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_STAT0 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT0_VGATAP_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT0_VGATAP_Msk 0x1FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT0_DFETAP1_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT0_DFETAP1_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_STAT1 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT1_DFETAP2_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT1_DFETAP2_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT1_DFETAP3_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT1_DFETAP3_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_STAT2 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT2_ERRSLIC1DELTA_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT2_ERRSLIC1DELTA_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT2_ERRSLIC2DELTA_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_STAT2_ERRSLIC2DELTA_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE1OS_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE1OS_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE2OS_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE2OS_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG0_OVRD_EDGSLICE2OS_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE1OS_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE1OS_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE2OS_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE2OS_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG1_OVRD_DATSLICE2OS_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_CFG2 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE1OS_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE1OS_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE2OS_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE2OS_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG2_OVRD_ERRSLICE2OS_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_CFG3 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE1OS_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE1OS_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE2OS_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE2OS_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG3_OVRD_ROAMSLICE2OS_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_CFG4 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG4_OVRD_AFEOSDAC_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG4_OVRD_AFEOSDAC_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG4_OVRD_AFEOSDAC_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG4_OVRD_AFEOSDAC_EN_Msk 0x80UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_CFG5 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_DFE_START_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_DFE_START_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_AFE_START_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_AFE_START_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_RESETB_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_RESETB_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_ALL_DONE_OVRD_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_ALL_DONE_OVRD_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_ERROR_BITS_MASK_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_ERROR_BITS_MASK_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_DFE_POLARITY_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_DFE_POLARITY_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_AFE_POLARITY_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_CFG5_OSA_AFE_POLARITY_Msk 0x200UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT0 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT0_EDGSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT0_EDGSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT0_EDGSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT0_EDGSLICE2OS_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT1 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT1_DATSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT1_DATSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT1_DATSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT1_DATSLICE2OS_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT2 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT2_ERRSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT2_ERRSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT2_ERRSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT2_ERRSLICE2OS_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT3 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT3_ROAMSLICE1OS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT3_ROAMSLICE1OS_Msk 0x7FUL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT3_ROAMSLICE2OS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT3_ROAMSLICE2OS_Msk 0x7F00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT4 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT4_AFEOSDAC_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT4_AFEOSDAC_Msk 0x7FUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT5 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_DFE_DONE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_DFE_DONE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_AFE_DONE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_AFE_DONE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_ALL_DONE_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_ALL_DONE_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_DFE_ERROR_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_DFE_ERROR_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_AFE_ERROR_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_AFE_ERROR_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_ERROR_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT5_OSA_ERROR_Msk 0x40UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT6 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT6_OSA_ERR_BITS0_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT6_OSA_ERR_BITS0_Msk 0xFFFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT7 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT7_OSA_ERR_BITS1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT7_OSA_ERR_BITS1_Msk 0xFFFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSA_STAT8 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT8_OSA_ERR_BITS2_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSA_STAT8_OSA_ERR_BITS2_Msk 0xFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_CTLE_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_EYEHT_EN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_EYEHT_EN_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_EYEHT_START_CNT_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_EYEHT_START_CNT_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_STG1_CMADJ_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_STG1_CMADJ_Msk 0x700UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_CAP_FZERO_EN_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_CAP_FZERO_EN_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_CAP_GAIN_ADJ_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG0_CTLE_CAP_GAIN_ADJ_Msk 0x1000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_CTLE_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG1RES_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG1RES_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG1CAP_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG1CAP_Msk 0x70UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG1BOOST_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG1BOOST_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG2RES_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG2RES_Msk 0xF00UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG2CAP_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG2CAP_Msk 0x7000UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG2BOOST_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_CFG1_CTLE_STG2BOOST_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_CTLE_STAT0 */
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_STAT0_EYEHT_ERR_CNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_STAT0_EYEHT_ERR_CNT_Msk 0xFFFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_CTLE_STAT1 */
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_STAT1_EYEHT_CNT_DONE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_CTLE_STAT1_EYEHT_CNT_DONE_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_EM_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EYE_WORD_CNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EYE_WORD_CNT_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_DATA_SAMPLE_DELAY_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_DATA_SAMPLE_DELAY_Msk 0x38UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_ROAM_SAMPLE_DELAY_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_ROAM_SAMPLE_DELAY_Msk 0x1C0UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EM_CTRL_UNUSED_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EM_CTRL_UNUSED_Msk 0xE00UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EN_EYE_ODD_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EN_EYE_ODD_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EN_EYE_EVEN_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EN_EYE_EVEN_Msk 0x2000UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_START_COUNT_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_START_COUNT_Msk 0x4000UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EN_EYEMON_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG0_EN_EYEMON_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_EM_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_ROAM_SLICEPOINT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_ROAM_SLICEPOINT_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_ROAM_SLICEPOINTSIGN1_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_ROAM_SLICEPOINTSIGN1_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_ROAM_SLICEPOINTSIGN2_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_ROAM_SLICEPOINTSIGN2_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_PIROAMPSEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_PIROAMPSEL_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_PIROAMPSEL_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_CFG1_PIROAMPSEL_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_EM_STAT0 */
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT0_EYE_ERROR_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT0_EYE_ERROR_Msk 0xFFFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_EM_STAT1 */
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT1_EYE_DONE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT1_EYE_DONE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT1_RX_ROAMSLIC1OS_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT1_RX_ROAMSLIC1OS_Msk 0x1FCUL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT1_RX_ROAMSLIC2OS_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_EM_STAT1_RX_ROAMSLIC2OS_Msk 0xFE00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFEA_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFEMAINADJ_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFEMAINADJ_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFE_PDB_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFE_PDB_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP1ADJ_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP1ADJ_Msk 0x70UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP1_PDB_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP1_PDB_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP2ADJ_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP2ADJ_Msk 0x700UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP2_PDB_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP2_PDB_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP3ADJ_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP3ADJ_Msk 0x7000UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP3_PDB_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFEA_CFG0_DFETAP3_PDB_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSAA_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJEDGESLIC1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJEDGESLIC1_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJEDGESLIC2_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJEDGESLIC2_Msk 0x38UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJDATSLIC1_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJDATSLIC1_Msk 0x1C0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJDATSLIC2_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJDATSLIC2_Msk 0xE00UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJERRSLIC1_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG0_ADJERRSLIC1_Msk 0x7000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_OSAA_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_ADJERRSLIC2_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_ADJERRSLIC2_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_ADJROAMSLIC1_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_ADJROAMSLIC1_Msk 0x38UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_ADJROAMSLIC2_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_ADJROAMSLIC2_Msk 0x1C0UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_AFEOSBIASADJ_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_OSAA_CFG1_AFEOSBIASADJ_Msk 0xE00UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_AFE_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_CMADJ_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_CMADJ_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_TERM_EN_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_TERM_EN_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_TERM_TRIM_CFG_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_TERM_TRIM_CFG_Msk 0x1F0UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_LPBK_EN_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_AFE_CFG_LPBK_EN_Msk 0x200UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_EMA_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_EMA_CFG_PICAPSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_EMA_CFG_PICAPSEL_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_EMA_CFG_PI_PDB_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_EMA_CFG_PI_PDB_Msk 0x10UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_REFCKSEL_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_REFCKSEL_CFG_LOCK2REF_SEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_REFCKSEL_CFG_LOCK2REF_SEL_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DIVH_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_DIVH_CFG_RECCLK_DIVH_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DIVH_CFG_RECCLK_DIVH_Msk 0x1FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DIVH_CFG_RECCLK_EN_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DIVH_CFG_RECCLK_EN_Msk 0x100UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_PFD_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_PFD_CFG_PFDDELAY_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_PFD_CFG_PFDDELAY_Msk 0xFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_CP_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_CP_CFG_IPUP_ADJ_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_CP_CFG_IPUP_ADJ_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_CP_CFG_IPDN_ADJ_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_CP_CFG_IPDN_ADJ_Msk 0x70UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_LF_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_LF_CFG_LPF_ADJ_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_LF_CFG_LPF_ADJ_Msk 0x3UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_BIASGEN_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG0_AFEBIASSET0_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG0_AFEBIASSET0_Msk 0xFFFUL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_BIASGEN_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG1_AFEBIASSET1_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG1_AFEBIASSET1_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG1_CFG_BIAS_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG1_CFG_BIAS_Msk 0x3F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG1_BIAS_PDB_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_RX_BIASGEN_CFG1_BIAS_PDB_Msk 0x4000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_GNRL_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_BUS_BIT_MODE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_BUS_BIT_MODE_Msk 0x3UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_DESERRESETB_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_DESERRESETB_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_DATA_RATE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_DATA_RATE_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_CLKGEN_PDB_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_CLKGEN_PDB_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_LOWPWR_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_LOWPWR_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_ERR_PATH_EN_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_ERR_PATH_EN_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_ROM_PATH_EN_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_ROM_PATH_EN_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_EDGE_DESER_EN_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_EDGE_DESER_EN_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_SUMEVN_PDB_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_SUMEVN_PDB_Msk 0x200UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_SUMODD_PDB_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_SUMODD_PDB_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_SUMVDD_PULLDOWN_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_SUMVDD_PULLDOWN_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_VCO_ADJ_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_VCO_ADJ_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_CLKINV_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_RX_GNRL_CFG_CLKINV_Msk 0x2000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_VREG_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VCPREGSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VCPREGSEL_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VREGRXASEL_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VREGRXASEL_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VREGRXCKSEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VREGRXCKSEL_Msk 0xF00UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VREGRXDSEL_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG0_VREGRXDSEL_Msk 0xF000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_VREG_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VCPREG_PDB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VCPREG_PDB_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VREGRXA_PDB_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VREGRXA_PDB_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VREGRXCK_PDB_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VREGRXCK_PDB_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VREGRXD_PDB_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VREGRXD_PDB_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VCPREG_BPB_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_VCPREG_BPB_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_CPCLK_SEL_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_CPCLK_SEL_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_BURNIN_EN_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_CFG1_BURNIN_EN_Msk 0x40UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_VREG_STAT */
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_STAT_POWER_GOOD_RXA_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_STAT_POWER_GOOD_RXA_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_STAT_POWER_GOOD_RXCK_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_STAT_POWER_GOOD_RXCK_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_STAT_POWER_GOOD_RXD_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_VREG_STAT_POWER_GOOD_RXD_Msk 0x4UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_SD_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_SD_CFG_LFPS_VTH_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SD_CFG_LFPS_VTH_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_SD_CFG_LFPSDET_PDB_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_SD_CFG_LFPSDET_PDB_Msk 0x8UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_SD_STAT */
#define USB32DEV_PHYSS_USB40PHY_RX_SD_STAT_LFPSDET_OUT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SD_STAT_LFPSDET_OUT_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_LD_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LOCKIN_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LOCKIN_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_FLOCKSEL_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_FLOCKSEL_Msk 0x30UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LOCKOUTSEL_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LOCKOUTSEL_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LUDDISABLE_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LUDDISABLE_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_FD_OVRD_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_FD_OVRD_Msk 0x300UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_RESETLOCKB_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_RESETLOCKB_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LD_RESETB_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_CFG_LD_RESETB_Msk 0x800UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_LD_STAT */
#define USB32DEV_PHYSS_USB40PHY_RX_LD_STAT_PLL_LOCKED_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_LD_STAT_PLL_LOCKED_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DFE_CFG4 */
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG4_INIT_VGATAP_VALUE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG4_INIT_VGATAP_VALUE_Msk 0x1FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG4_TAP_UPDATE_TRSHLD_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_RX_DFE_CFG4_TAP_UPDATE_TRSHLD_Msk 0xE0UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_HDWR_ENABLE */
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_CTLE_HDWR_ENABLE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_CTLE_HDWR_ENABLE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_DFE_HDWR_ENABLE_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_DFE_HDWR_ENABLE_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_DESER_HDWR_ENABLE_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_DESER_HDWR_ENABLE_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_CDR_HDWR_ENABLE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_CDR_HDWR_ENABLE_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_SPARE_HDWR_ENABLE_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_HDWR_ENABLE_SPARE_HDWR_ENABLE_Msk 0x30UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_ATEST_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_CNTL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_CNTL_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_ENABLE_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_ENABLE_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_IB20U_SEL_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_IB20U_SEL_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_DAC_TEST_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_DAC_TEST_Msk 0xF00UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_DFE_TEST_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_DFE_TEST_Msk 0x7000UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_VREGDIV_EN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_ATEST_CFG_ADFT_VREGDIV_EN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.RX_DTEST_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_DTEST_CFG_DDFT0_SEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_DTEST_CFG_DDFT0_SEL_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_RX_DTEST_CFG_DDFT1_SEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_RX_DTEST_CFG_DDFT1_SEL_Msk 0x3F00UL
#define USB32DEV_PHYSS_USB40PHY_RX_DTEST_CFG_DDFT_ENABLE_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_RX_DTEST_CFG_DDFT_ENABLE_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_RX.SPARE_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_CFG_SPARE_OUT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_CFG_SPARE_OUT_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_CFG_SPARE_FT_OUT_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_CFG_SPARE_FT_OUT_Msk 0xFFF8UL
/* USB32DEV_PHYSS_USB40PHY_RX.SPARE_STAT */
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_STAT_SPARE_INPUTS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_STAT_SPARE_INPUTS_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_STAT_SPARE_FT_IN_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_STAT_SPARE_FT_IN_Msk 0xFFF8UL
/* USB32DEV_PHYSS_USB40PHY_RX.SPARE_HV_CFG */
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_HV_CFG_SPARE_HV_CFG_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_RX_SPARE_HV_CFG_SPARE_HV_CFG_Msk 0x7UL


/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_SDM_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SDM_CFG_SDM_ENABLE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SDM_CFG_SDM_ENABLE_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SDM_CFG_DITHER_EN_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SDM_CFG_DITHER_EN_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SDM_CFG_DITHER_GAIN_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SDM_CFG_DITHER_GAIN_Msk 0x1CUL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_SSM_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG0_DIVF_FRAC_MSB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG0_DIVF_FRAC_MSB_Msk 0xFFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG0_DIVF_INTEGER_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG0_DIVF_INTEGER_Msk 0xFF00UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_SSM_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG1_DIVF_FRAC_LSB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG1_DIVF_FRAC_LSB_Msk 0x3FFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG1_SPREAD_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG1_SPREAD_Msk 0xE000UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_SSM_CFG2 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG2_SSM_STEP_CNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG2_SSM_STEP_CNT_Msk 0x1FFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG2_SSM_UPDATE_CNT_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG2_SSM_UPDATE_CNT_Msk 0x3E00UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG2_SSM_ENABLE_Pos 14UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_SSM_CFG2_SSM_ENABLE_Msk 0x4000UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AFC_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_VCTL_SET_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_VCTL_SET_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_VCMP_PDB_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_VCMP_PDB_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_VCMP_SEL_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_VCMP_SEL_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_OVRD_CAPSEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_OVRD_CAPSEL_Msk 0x7F00UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_OVRRIDEN_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG0_AFC_OVRRIDEN_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AFC_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_HI_CNT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_HI_CNT_Msk 0x1FUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_LO_CNT_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_LO_CNT_Msk 0x3E0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_ENABLE_OVERRIDE_Pos 10UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_ENABLE_OVERRIDE_Msk 0x400UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_ENABLE_OVERRIDE_VALUE_Pos 11UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_VCTL_ENABLE_OVERRIDE_VALUE_Msk 0x800UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_SEL_MIN_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_SEL_MIN_Msk 0x1000UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_SEL_MAX_Pos 13UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG1_AFC_SEL_MAX_Msk 0x2000UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AFC_CFG2 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG2_AFC_CLKDIVSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG2_AFC_CLKDIVSEL_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG2_AFC_PDB_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_CFG2_AFC_PDB_Msk 0x40UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AFC_STAT */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_STAT_CAPSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AFC_STAT_CAPSEL_Msk 0x7FUL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AAC_CFG0 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG0_AACREFSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG0_AACREFSEL_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG0_AAC_OVRD_AACSEL_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG0_AAC_OVRD_AACSEL_Msk 0x1F0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG0_AAC_OVRRIDEN_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG0_AAC_OVRRIDEN_Msk 0x200UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AAC_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_CLKDIVSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_CLKDIVSEL_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_FRZCNT_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_FRZCNT_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_NORECAL_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_NORECAL_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_PDB_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_CFG1_AAC_PDB_Msk 0x100UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_AAC_STAT */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_STAT_AACSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_AAC_STAT_AACSEL_Msk 0x1FUL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_REFCKSEL_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_REFCKSEL_CFG_REFCLKSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_REFCKSEL_CFG_REFCLKSEL_Msk 0x7UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_DIVR_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVR_CFG_LCDIVR_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVR_CFG_LCDIVR_Msk 0xFUL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_DIVP_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_LCDIVP_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_LCDIVP_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_LCBYPASS_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_LCBYPASS_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_TX0_DIVP_PDB_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_TX0_DIVP_PDB_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_TX1_DIVP_PDB_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_TX1_DIVP_PDB_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_PLL_SYNC_ENABLE_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVP_CFG_PLL_SYNC_ENABLE_Msk 0x80UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_DIVH_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVH_CFG_HSREF_DIVH_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVH_CFG_HSREF_DIVH_Msk 0x1FUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVH_CFG_HSREF_EN_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DIVH_CFG_HSREF_EN_Msk 0x20UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_PFD_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_PFD_CFG_PFDDELAY_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_PFD_CFG_PFDDELAY_Msk 0xFUL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_CP_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_CP_CFG_CFGRCP_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_CP_CFG_CFGRCP_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_CP_CFG_ICPDAC_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_CP_CFG_ICPDAC_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_CP_CFG_CP_PDB_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_CP_CFG_CP_PDB_Msk 0x100UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_LF_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LF_CFG_LF_RTUNE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LF_CFG_LF_RTUNE_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LF_CFG_LF_CTUNE_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LF_CFG_LF_CTUNE_Msk 0x18UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_VCO_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VCO_CFG_DCVARMODE_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VCO_CFG_DCVARMODE_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VCO_CFG_VCOVARCM1_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VCO_CFG_VCOVARCM1_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VCO_CFG_VCOVARCM2_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VCO_CFG_VCOVARCM2_Msk 0xF00UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_BIASGEN_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_BIASGEN_CFG_CFG_BIAS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_BIASGEN_CFG_CFG_BIAS_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_BIASGEN_CFG_TERM_TRIM_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_BIASGEN_CFG_TERM_TRIM_Msk 0x1F00UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_GNRL_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_GNRL_CFG_PLL_PDB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_GNRL_CFG_PLL_PDB_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_GNRL_CFG_LKDT_RESETB_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_GNRL_CFG_LKDT_RESETB_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_GNRL_CFG_RESETLOCKB_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_GNRL_CFG_RESETLOCKB_Msk 0x4UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_VREG_CFG1 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VCPREGSEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VCPREGSEL_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VREGLCPLLSEL_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VREGLCPLLSEL_Msk 0xF0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VREGREFSEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VREGREFSEL_Msk 0xF00UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VREGDIGSEL_Pos 12UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG1_VREGDIGSEL_Msk 0xF000UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_VREG_CFG2 */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VCPREG_PDB_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VCPREG_PDB_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VREGLCPLL_PDB_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VREGLCPLL_PDB_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VREGREF_PDB_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VREGREF_PDB_Msk 0x4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VREGDIG_PDB_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VREGDIG_PDB_Msk 0x8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VCPREG_BPB_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_VCPREG_BPB_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_CPCLK_SEL_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_CPCLK_SEL_Msk 0x20UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_BURNIN_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_CFG2_BURNIN_Msk 0x40UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_VREG_STAT */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_STAT_PWR_GOOD_LCPLL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_STAT_PWR_GOOD_LCPLL_Msk 0x1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_STAT_PWR_GOOD_REF_Pos 1UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_STAT_PWR_GOOD_REF_Msk 0x2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_STAT_PWR_GOOD_DIG_Pos 2UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_VREG_STAT_PWR_GOOD_DIG_Msk 0x4UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_LD_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_LOCKOS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_LOCKOS_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_DLFCNTSEL_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_DLFCNTSEL_Msk 0x30UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_LUDDIVSEL_Pos 6UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_LUDDIVSEL_Msk 0x40UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_LUDDISABLE_Pos 7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_LUDDISABLE_Msk 0x80UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_CRS_FD_AUXCLK_SEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_CRS_FD_AUXCLK_SEL_Msk 0x100UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_CRS_FD_DISABLE_Pos 9UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_CFG_CRS_FD_DISABLE_Msk 0x200UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_LD_STAT */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_STAT_FLOCK_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_LD_STAT_FLOCK_Msk 0x1UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_ATEST */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_ATEST_ADFT_SEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_ATEST_ADFT_SEL_Msk 0xFUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_ATEST_ADFT_ENABLE_Pos 4UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_ATEST_ADFT_ENABLE_Msk 0x10UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_ATEST_ADFT_VREGDIV_EN_Pos 5UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_ATEST_ADFT_VREGDIV_EN_Msk 0x20UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.PLL_DTEST */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DTEST_DDFT0_SEL_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DTEST_DDFT0_SEL_Msk 0x3FUL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DTEST_DDFT1_SEL_Pos 8UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DTEST_DDFT1_SEL_Msk 0x3F00UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DTEST_DDFT_ENABLE_Pos 15UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_PLL_DTEST_DDFT_ENABLE_Msk 0x8000UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.SPARE_CFG */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_CFG_SPARE_OUT_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_CFG_SPARE_OUT_Msk 0x3UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_CFG_SPARE_FT_OUT_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_CFG_SPARE_FT_OUT_Msk 0xFFF8UL
/* USB32DEV_PHYSS_USB40PHY_PLL_SYS.SPARE_STAT */
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_STAT_SPARE_INPUTS_Pos 0UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_STAT_SPARE_INPUTS_Msk 0x7UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_STAT_SPARE_FT_IN_Pos 3UL
#define USB32DEV_PHYSS_USB40PHY_PLL_SYS_SPARE_STAT_SPARE_FT_IN_Msk 0xFFF8UL


/* USB32DEV_ADAPTER_DMA_SCK.SCK_DSCR */
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_DSCR_NUMBER_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_DSCR_NUMBER_Msk 0xFFFFUL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_DSCR_COUNT_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_DSCR_COUNT_Msk 0xFF0000UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_DSCR_LOW_Pos 24UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_DSCR_LOW_Msk 0xFF000000UL
/* USB32DEV_ADAPTER_DMA_SCK.SCK_SIZE */
#define USB32DEV_ADAPTER_DMA_SCK_SIZE_TRANS_SIZE_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_SIZE_TRANS_SIZE_Msk 0xFFFFFFFFUL
/* USB32DEV_ADAPTER_DMA_SCK.SCK_COUNT */
#define USB32DEV_ADAPTER_DMA_SCK_COUNT_TRANS_COUNT_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_COUNT_TRANS_COUNT_Msk 0xFFFFFFFFUL
/* USB32DEV_ADAPTER_DMA_SCK.SCK_STATUS */
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_AVL_COUNT_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_AVL_COUNT_Msk 0x1FUL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_AVL_MIN_Pos 5UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_AVL_MIN_Msk 0x3E0UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_AVL_ENABLE_Pos 10UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_AVL_ENABLE_Msk 0x400UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SCK_ZLP_ALWAYS_Pos 11UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SCK_ZLP_ALWAYS_Msk 0x800UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_STATE_Pos 15UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_STATE_Msk 0x38000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_ZLP_RCVD_Pos 18UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_ZLP_RCVD_Msk 0x40000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSPENDED_Pos 19UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSPENDED_Msk 0x80000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_ENABLED_Pos 20UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_ENABLED_Msk 0x100000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_TRUNCATE_Pos 21UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_TRUNCATE_Msk 0x200000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_EN_PROD_EVENTS_Pos 22UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_EN_PROD_EVENTS_Msk 0x400000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_EN_CONS_EVENTS_Pos 23UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_EN_CONS_EVENTS_Msk 0x800000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_PARTIAL_Pos 24UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_PARTIAL_Msk 0x1000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_LAST_Pos 25UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_LAST_Msk 0x2000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_TRANS_Pos 26UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_TRANS_Msk 0x4000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_EOP_Pos 27UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_SUSP_EOP_Msk 0x8000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_WRAPUP_Pos 28UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_WRAPUP_Msk 0x10000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_UNIT_Pos 29UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_UNIT_Msk 0x20000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_GO_SUSPEND_Pos 30UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_GO_SUSPEND_Msk 0x40000000UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_GO_ENABLE_Pos 31UL
#define USB32DEV_ADAPTER_DMA_SCK_STATUS_GO_ENABLE_Msk 0x80000000UL
/* USB32DEV_ADAPTER_DMA_SCK.SCK_INTR */
#define USB32DEV_ADAPTER_DMA_SCK_INTR_PRODUCE_EVENT_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_PRODUCE_EVENT_Msk 0x1UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_CONSUME_EVENT_Pos 1UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_CONSUME_EVENT_Msk 0x2UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_DSCR_IS_LOW_Pos 2UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_DSCR_IS_LOW_Msk 0x4UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_DSCR_NOT_AVL_Pos 3UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_DSCR_NOT_AVL_Msk 0x8UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_STALL_Pos 4UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_STALL_Msk 0x10UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_SUSPEND_Pos 5UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_SUSPEND_Msk 0x20UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_ERROR_Pos 6UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_ERROR_Msk 0x40UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_TRANS_DONE_Pos 7UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_TRANS_DONE_Msk 0x80UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_PARTIAL_BUF_Pos 8UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_PARTIAL_BUF_Msk 0x100UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_LAST_BUF_Pos 9UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_LAST_BUF_Msk 0x200UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_EVENT_RCVD_Pos 10UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_EVENT_RCVD_Msk 0x400UL
/* USB32DEV_ADAPTER_DMA_SCK.SCK_INTR_MASK */
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_PRODUCE_EVENT_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_PRODUCE_EVENT_Msk 0x1UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_CONSUME_EVENT_Pos 1UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_CONSUME_EVENT_Msk 0x2UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_DSCR_IS_LOW_Pos 2UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_DSCR_IS_LOW_Msk 0x4UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_DSCR_NOT_AVL_Pos 3UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_DSCR_NOT_AVL_Msk 0x8UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_STALL_Pos 4UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_STALL_Msk 0x10UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_SUSPEND_Pos 5UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_SUSPEND_Msk 0x20UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_ERROR_Pos 6UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_ERROR_Msk 0x40UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_TRANS_DONE_Pos 7UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_TRANS_DONE_Msk 0x80UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_PARTIAL_BUF_Pos 8UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_PARTIAL_BUF_Msk 0x100UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_LAST_BUF_Pos 9UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_LAST_BUF_Msk 0x200UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_EVENT_RCVD_Pos 10UL
#define USB32DEV_ADAPTER_DMA_SCK_INTR_MASK_EVENT_RCVD_Msk 0x400UL
/* USB32DEV_ADAPTER_DMA_SCK.DSCR_BUFFER */
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_BUFFER_BUFFER_ADDR_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_BUFFER_BUFFER_ADDR_Msk 0x7FFFFFFFUL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_BUFFER_MARKER_Pos 31UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_BUFFER_MARKER_Msk 0x80000000UL
/* USB32DEV_ADAPTER_DMA_SCK.DSCR_SYNC */
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_SCK_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_SCK_Msk 0xFFUL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_IP_Pos 8UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_IP_Msk 0x3F00UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_EVENT_Pos 14UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_EVENT_Msk 0x4000UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_INT_Pos 15UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_INT_Msk 0x8000UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_SCK_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_SCK_Msk 0xFF0000UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_IP_Pos 24UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_IP_Msk 0x3F000000UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_EVENT_Pos 30UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_EVENT_Msk 0x40000000UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_INT_Pos 31UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_INT_Msk 0x80000000UL
/* USB32DEV_ADAPTER_DMA_SCK.DSCR_CHAIN */
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_CHAIN_RD_NEXT_DSCR_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_CHAIN_RD_NEXT_DSCR_Msk 0xFFFFUL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_CHAIN_WR_NEXT_DSCR_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_CHAIN_WR_NEXT_DSCR_Msk 0xFFFF0000UL
/* USB32DEV_ADAPTER_DMA_SCK.DSCR_SIZE */
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_MSB_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_MSB_Msk 0x1UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_EOP_Pos 1UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_EOP_Msk 0x2UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_ERROR_Pos 2UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_ERROR_Msk 0x4UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_OCCUPIED_Pos 3UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_OCCUPIED_Msk 0x8UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_SIZE_Pos 4UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_SIZE_Msk 0xFFF0UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_Msk 0xFFFF0000UL
/* USB32DEV_ADAPTER_DMA_SCK.EVENT */
#define USB32DEV_ADAPTER_DMA_SCK_EVENT_ACTIVE_DSCR_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_EVENT_ACTIVE_DSCR_Msk 0xFFFFUL
#define USB32DEV_ADAPTER_DMA_SCK_EVENT_EVENT_TYPE_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_EVENT_EVENT_TYPE_Msk 0x10000UL


/* USB32DEV_ADAPTER_DMA_SCK_GBL.SCK_INTR */
#define USB32DEV_ADAPTER_DMA_SCK_GBL_SCK_INTR_SCKINTR_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_SCK_INTR_SCKINTR_Msk 0xFFFFFFFFUL
/* USB32DEV_ADAPTER_DMA_SCK_GBL.ADAPTER_CTRL */
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAPTER_EN_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAPTER_EN_Msk 0x1UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_WR_PRIO_Pos 4UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_WR_PRIO_Msk 0x10UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_RD_PRIO_Pos 5UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_RD_PRIO_Msk 0x20UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_WR_PRIO_N_Pos 6UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_WR_PRIO_N_Msk 0x40UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_RD_PRIO_N_Pos 7UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_RD_PRIO_N_Msk 0x80UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_SUSP_PARTIAL_NO_EVT_N_Pos 10UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_SUSP_PARTIAL_NO_EVT_N_Msk 0x400UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_FQ_DEPTH_CTRL_Pos 12UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_FQ_DEPTH_CTRL_Msk 0x3F000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_DEBUG_CAPTURE_Pos 20UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_DEBUG_CAPTURE_Msk 0x100000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_EN_UNALIGNED_READ_N_Pos 21UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_EN_UNALIGNED_READ_N_Msk 0x200000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_TR_OUT_DELAY_Pos 24UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_TR_OUT_DELAY_Msk 0x3F000000UL
/* USB32DEV_ADAPTER_DMA_SCK_GBL.ADAPTER_DEBUG */
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_COUNT_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_COUNT_Msk 0xFUL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_STATE_Pos 4UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_STATE_Msk 0x70UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ID_Pos 7UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ID_Msk 0x1F80UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_COUNT_Pos 13UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_COUNT_Msk 0x3FE000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_STATE_Pos 22UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_STATE_Msk 0x400000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_ID_Pos 23UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_ID_Msk 0x1F800000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ABORT_Pos 29UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ABORT_Msk 0x20000000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_BS_STATE_Pos 30UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_BS_STATE_Msk 0xC0000000UL
/* USB32DEV_ADAPTER_DMA_SCK_GBL.ADAPTER_CONF */
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SOCKET_ACTIVE_THRSHLD_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SOCKET_ACTIVE_THRSHLD_Msk 0x3FUL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAM_COUNT_Pos 6UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAM_COUNT_Msk 0x7FC0UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAMING_MODE_Pos 15UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAMING_MODE_Msk 0x8000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_CYCLES_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_CYCLES_Msk 0xF0000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_ES_CYCLES_Pos 20UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_ES_CYCLES_Msk 0xF00000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_GBL_CYCLES_Pos 24UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_GBL_CYCLES_Msk 0xF000000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_MMIO_LOW_PRIORITY_Pos 28UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_MMIO_LOW_PRIORITY_Msk 0x10000000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SWITCH_HIGH_PRIORITY_Pos 29UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SWITCH_HIGH_PRIORITY_Msk 0x20000000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_ABORT_EN_Pos 30UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_ABORT_EN_Msk 0x40000000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_DESCR_PF_EN_N_Pos 31UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_DESCR_PF_EN_N_Msk 0x80000000UL
/* USB32DEV_ADAPTER_DMA_SCK_GBL.ADAPTER_STATUS */
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_TTL_SOCKETS_Pos 0UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_TTL_SOCKETS_Msk 0xFFUL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_IG_ONLY_Pos 8UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_IG_ONLY_Msk 0xFF00UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_FQ_SIZE_Pos 16UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_FQ_SIZE_Msk 0x1FF0000UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_WORD_SIZE_Pos 25UL
#define USB32DEV_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_WORD_SIZE_Msk 0x6000000UL


#endif /* _CYIP_USB32DEV_H_ */


/* [] END OF FILE */
