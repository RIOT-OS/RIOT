/***************************************************************************//**
* \file cyip_lvdsss.h
*
* \brief
* LVDSSS IP definitions
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

#ifndef _CYIP_LVDSSS_H_
#define _CYIP_LVDSSS_H_

#include "cyip_headers.h"

/*******************************************************************************
*                                    LVDSSS
*******************************************************************************/

#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_SECTION_SIZE 0x00000020UL
#define LVDSSS_LVDS_THREAD_SECTION_SIZE         0x00000200UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_SECTION_SIZE 0x00000010UL
#define LVDSSS_LVDS_GPIF_LEFT_SECTION_SIZE      0x00001000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_SECTION_SIZE 0x00000010UL
#define LVDSSS_LVDS_GPIF_RIGHT_SECTION_SIZE     0x00001000UL
#define LVDSSS_LVDS_GPIF_SECTION_SIZE           0x00003000UL
#define LVDSSS_LVDS_AFE_SECTION_SIZE            0x00000800UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_SECTION_SIZE 0x00000080UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_SECTION_SIZE 0x00000100UL
#define LVDSSS_LVDS_ADAPTER_DMA_SECTION_SIZE    0x00010000UL
#define LVDSSS_LVDS_SECTION_SIZE                0x00040000UL
#define LVDSSS_SECTION_SIZE                     0x00040000UL

/**
  * \brief LVDS Low power and Wakeup Interrupt registers (LVDSSS_LVDS_LVDS_LOW_PWR_INTR)
  */
typedef struct {
  __IOM uint32_t LVDS_WAKEUP_INTR;              /*!< 0x00000000 LVDS_WAKEUP Interrupt Request Register */
  __IOM uint32_t LVDS_WAKEUP_INTR_MASK;         /*!< 0x00000004 LVDS_WAKEUP Interrupt Mask Register */
   __IM uint32_t LVDS_WAKEUP_INTR_MASKED;       /*!< 0x00000008 LVDS_WAKEUP Interrupt Generator */
  __IOM uint32_t LVDS_WAKEUP_INTR_SET;          /*!< 0x0000000C LVDS_WAKEUP Interrupt Set Register */
  __IOM uint32_t LOW_PWR_CTL[2];                /*!< 0x00000010 Low power mode control register */
   __IM uint32_t RESERVED[2];
} LVDSSS_LVDS_LVDS_LOW_PWR_INTR_V1_Type;        /*!< Size = 32 (0x20) */

/**
  * \brief Set of registers for a thread (LVDSSS_LVDS_THREAD)
  */
typedef struct {
  __IOM uint32_t GPIF_DATA_CTRL;                /*!< 0x00000000 Data Control Register */
   __IM uint32_t GPIF_INGRESS_DATA_WORD0;       /*!< 0x00000004 Socket Ingress Data */
   __IM uint32_t GPIF_INGRESS_DATA_WORD1;       /*!< 0x00000008 Socket Ingress Data */
   __IM uint32_t GPIF_INGRESS_DATA_WORD2;       /*!< 0x0000000C Socket Ingress Data */
   __IM uint32_t GPIF_INGRESS_DATA_WORD3;       /*!< 0x00000010 Socket Ingress Data */
  __IOM uint32_t GPIF_EGRESS_DATA_WORD0;        /*!< 0x00000014 Socket Egress Data */
  __IOM uint32_t GPIF_EGRESS_DATA_WORD1;        /*!< 0x00000018 Socket Egress Data */
   __IM uint32_t GPIF_INGRESS_ADDRESS;          /*!< 0x0000001C Thread Ingress Address */
  __IOM uint32_t GPIF_EGRESS_ADDRESS;           /*!< 0x00000020 Thread Egress Address */
  __IOM uint32_t GPIF_THREAD_CONFIG;            /*!< 0x00000024 Thread Configuration Register */
   __IM uint32_t RESERVED;
   __IM uint32_t VARW0;                         /*!< 0x0000002C Variable Register */
   __IM uint32_t VARW1;                         /*!< 0x00000030 Variable Register */
   __IM uint32_t PTSSS;                         /*!< 0x00000034 PTS snapshot register */
   __IM uint32_t SCRSS_DW0;                     /*!< 0x00000038 SCR snapshot register */
   __IM uint32_t SCRSS_DW1;                     /*!< 0x0000003C SCR snapshot register */
   __IM uint32_t HDR_FLGS;                      /*!< 0x00000040 Header Flags */
   __IM uint32_t RESERVED1[3];
   __IM uint32_t EVC_VAR0_DW0;                  /*!< 0x00000050 Event Count Variable0 */
   __IM uint32_t EVC_VAR0_DW1;                  /*!< 0x00000054 Event Count Variable0 */
   __IM uint32_t EVC_VAR1_DW0;                  /*!< 0x00000058 Event Count Variable1 */
   __IM uint32_t EVC_VAR1_DW1;                  /*!< 0x0000005C Event Count Variable1 */
   __IM uint32_t PLC_DW0;                       /*!< 0x00000060 Payload Count register */
   __IM uint32_t PLC_DW1;                       /*!< 0x00000064 Payload Count register */
  __IOM uint32_t PAYLOAD_CFG;                   /*!< 0x00000068 Payload Config register */
   __IM uint32_t RESERVED2;
  __IOM uint32_t DCRC_CONFIG;                   /*!< 0x00000070 Data CRC Configuration register */
  __IOM uint32_t DCRC;                          /*!< 0x00000074 Data CRC */
  __IOM uint32_t DCRC_STS;                      /*!< 0x00000078 Data CRC status */
   __IM uint32_t RESERVED3;
  __IOM uint32_t MCRC_CONFIG;                   /*!< 0x00000080 MetaData(or Header) CRC Configuration register */
  __IOM uint32_t MCRC;                          /*!< 0x00000084 MetaData CRC */
  __IOM uint32_t THREAD_DMA_INTF_CFG;           /*!< 0x00000088 Thread controller DMA interface configuration */
   __IM uint32_t RESERVED4;
  __IOM uint32_t MD0_CTRL;                      /*!< 0x00000090 Metadata-0 Control word */
  __IOM uint32_t MD1_CTRL;                      /*!< 0x00000094 Metadata-1 Control word */
  __IOM uint32_t MD2_CTRL;                      /*!< 0x00000098 Metadata-2 Control word */
  __IOM uint32_t MD3_CTRL;                      /*!< 0x0000009C Metadata-3 Control word */
   __IM uint32_t RESERVED5[88];
} LVDSSS_LVDS_THREAD_V1_Type;                   /*!< Size = 512 (0x200) */

/**
  * \brief GPIF  Waveform registers (LVDSSS_LVDS_GPIF_LEFT_WAVEFORM)
  */
typedef struct {
  __IOM uint32_t WAVEFORM0;                     /*!< 0x00000000 GPIF state definition */
  __IOM uint32_t WAVEFORM1;                     /*!< 0x00000004 GPIF state definition */
  __IOM uint32_t WAVEFORM2;                     /*!< 0x00000008 GPIF state definition */
  __IOM uint32_t WAVEFORM3;                     /*!< 0x0000000C GPIF state definition */
} LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_V1_Type;       /*!< Size = 16 (0x10) */

/**
  * \brief GPIF State Machine Left Waveform memory (LVDSSS_LVDS_GPIF_LEFT)
  */
typedef struct {
        LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_V1_Type WAVEFORM[256]; /*!< 0x00000000 GPIF  Waveform registers */
} LVDSSS_LVDS_GPIF_LEFT_V1_Type;                /*!< Size = 4096 (0x1000) */

/**
  * \brief GPIF  Waveform registers (LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM)
  */
typedef struct {
  __IOM uint32_t WAVEFORM0;                     /*!< 0x00000000 GPIF state definition */
  __IOM uint32_t WAVEFORM1;                     /*!< 0x00000004 GPIF state definition */
  __IOM uint32_t WAVEFORM2;                     /*!< 0x00000008 GPIF state definition */
  __IOM uint32_t WAVEFORM3;                     /*!< 0x0000000C GPIF state definition */
} LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_V1_Type;      /*!< Size = 16 (0x10) */

/**
  * \brief GPIF State Machine Right Waveform memory. (LVDSSS_LVDS_GPIF_RIGHT)
  */
typedef struct {
        LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_V1_Type WAVEFORM[256]; /*!< 0x00000000 GPIF  Waveform registers */
} LVDSSS_LVDS_GPIF_RIGHT_V1_Type;               /*!< Size = 4096 (0x1000) */

/**
  * \brief GPIF-II Configuration Registers (LVDSSS_LVDS_GPIF)
  */
typedef struct {
  __IOM uint32_t GPIF_CONFIG;                   /*!< 0x00000000 GPIF Configuration Register */
  __IOM uint32_t GPIF_BUS_CONFIG;               /*!< 0x00000004 Bus Configuration Register */
  __IOM uint32_t GPIF_BUS_CONFIG2;              /*!< 0x00000008 Bus Configuration Register #2 */
  __IOM uint32_t GPIF_AD_CONFIG;                /*!< 0x0000000C Address/Data configuration register */
  __IOM uint32_t GPIF_CTL_FUNC0;                /*!< 0x00000010 CTL pin functional usage register #0 */
  __IOM uint32_t GPIF_CTL_FUNC1;                /*!< 0x00000014 CTL pin functional usage register #1 */
  __IOM uint32_t GPIF_CTL_FUNC2;                /*!< 0x00000018 CTL pin functional usage register #2 */
  __IOM uint32_t GPIF_CTL_FUNC3;                /*!< 0x0000001C CTL pin functional usage register #3 */
  __IOM uint32_t GPIF_CTL_FUNC4;                /*!< 0x00000020 CTL pin functional usage register #4 */
   __IM uint32_t GPIF_STATUS;                   /*!< 0x00000024 GPIF Status Register */
   __IM uint32_t RESERVED[2];
  __IOM uint32_t GPIF_INTR;                     /*!< 0x00000030 GPIF Interrupt Request Register */
  __IOM uint32_t GPIF_INTR_MASK;                /*!< 0x00000034 GPIF Interrupt Mask Register */
   __IM uint32_t GPIF_INTR_MASKED;              /*!< 0x00000038 GPIF Interrupt Generator */
  __IOM uint32_t GPIF_INTR_SET;                 /*!< 0x0000003C GPIF Interrupt Set Register */
   __IM uint32_t GPIF_ERROR;                    /*!< 0x00000040 GPIF Error Register */
   __IM uint32_t RESERVED1[3];
  __IOM uint32_t GPIF_CTRL_BUS_DIRECTION[20];   /*!< 0x00000050 Control Bus in/out direction */
   __IM uint32_t RESERVED2[4];
  __IOM uint32_t GPIF_CTRL_BUS_DEFAULT;         /*!< 0x000000B0 Control bus default values */
  __IOM uint32_t GPIF_CTRL_BUS_POLARITY;        /*!< 0x000000B4 Control bus signal polarity */
  __IOM uint32_t GPIF_CTRL_BUS_TOGGLE;          /*!< 0x000000B8 Control bus output toggle mode */
   __IM uint32_t RESERVED3[17];
  __IOM uint32_t GPIF_CTRL_BUS_SELECT[20];      /*!< 0x00000100 Control bus connection matrix register */
   __IM uint32_t RESERVED4[4];
  __IOM uint32_t GPIF_CTRL_COUNT_CONFIG;        /*!< 0x00000160 Control counter configuration */
  __IOM uint32_t GPIF_CTRL_COUNT_RESET;         /*!< 0x00000164 Control counter reset register */
  __IOM uint32_t GPIF_CTRL_COUNT_LIMIT;         /*!< 0x00000168 Control counter limit register */
   __IM uint32_t RESERVED5;
  __IOM uint32_t GPIF_ADDR_COUNT_CONFIG;        /*!< 0x00000170 Address counter configuration */
  __IOM uint32_t GPIF_ADDR_COUNT_RESET;         /*!< 0x00000174 Address counter reset register */
  __IOM uint32_t GPIF_ADDR_COUNT_LIMIT;         /*!< 0x00000178 Address counter limit register */
   __IM uint32_t RESERVED6;
  __IOM uint32_t GPIF_STATE_COUNT_CONFIG;       /*!< 0x00000180 State counter configuration */
  __IOM uint32_t GPIF_STATE_COUNT_LIMIT;        /*!< 0x00000184 State counter limit register */
   __IM uint32_t RESERVED7[2];
  __IOM uint32_t GPIF_DATA_COUNT_CONFIG;        /*!< 0x00000190 Data counter configuration */
  __IOM uint32_t GPIF_DATA_COUNT_RESET_LSB;     /*!< 0x00000194 Data counter reset register */
   __IM uint32_t RESERVED8;
  __IOM uint32_t GPIF_DATA_COUNT_RESET_MSB;     /*!< 0x0000019C Data counter reset register */
  __IOM uint32_t GPIF_DATA_COUNT_LIMIT_LSB;     /*!< 0x000001A0 Data counter limit register */
  __IOM uint32_t GPIF_DATA_COUNT_LIMIT_MSB;     /*!< 0x000001A4 Data counter limit register */
  __IOM uint32_t GPIF_CTRL_COMP_VALUE;          /*!< 0x000001A8 Control comparator value */
  __IOM uint32_t GPIF_CTRL_COMP_MASK;           /*!< 0x000001AC Control comparator mask */
  __IOM uint32_t GPIF_DATA_COMP_VALUE_WORD0;    /*!< 0x000001B0 Data comparator value */
  __IOM uint32_t GPIF_DATA_COMP_VALUE_WORD1;    /*!< 0x000001B4 Data comparator value */
  __IOM uint32_t GPIF_DATA_COMP_VALUE_WORD2;    /*!< 0x000001B8 Data comparator value */
  __IOM uint32_t GPIF_DATA_COMP_VALUE_WORD3;    /*!< 0x000001BC Data comparator value */
   __IM uint32_t RESERVED9[4];
  __IOM uint32_t GPIF_DATA_COMP_MASK_WORD0;     /*!< 0x000001D0 Data comparator mask */
  __IOM uint32_t GPIF_DATA_COMP_MASK_WORD1;     /*!< 0x000001D4 Data comparator mask */
  __IOM uint32_t GPIF_DATA_COMP_MASK_WORD2;     /*!< 0x000001D8 Data comparator mask */
  __IOM uint32_t GPIF_DATA_COMP_MASK_WORD3;     /*!< 0x000001DC Data comparator mask */
  __IOM uint32_t GPIF_ADDR_COMP_VALUE;          /*!< 0x000001E0 Address comparator value */
  __IOM uint32_t GPIF_ADDR_COMP_MASK;           /*!< 0x000001E4 Address comparator mask */
   __IM uint32_t RESERVED10[2];
   __IM uint32_t GPIF_LAMBDA_STAT0;             /*!< 0x000001F0 Lambda Status Register */
   __IM uint32_t GPIF_LAMBDA_STAT1;             /*!< 0x000001F4 Lambda Status Register */
   __IM uint32_t GPIF_ALPHA_STAT;               /*!< 0x000001F8 Alpha Status Register */
   __IM uint32_t GPIF_BETA_STAT;                /*!< 0x000001FC Beta Status Register */
  __IOM uint32_t GPIF_WAVEFORM_CTRL_STAT;       /*!< 0x00000200 Waveform program control */
  __IOM uint32_t GPIF_WAVEFORM_SWITCH;          /*!< 0x00000204 Waveform switch control */
  __IOM uint32_t GPIF_WAVEFORM_SWITCH_TIMEOUT;  /*!< 0x00000208 Waveform timeout register */
   __IM uint32_t RESERVED11;
  __IOM uint32_t GPIF_CRC_CALC_CONFIG;          /*!< 0x00000210 CRC Calculate Configuration Register */
   __IM uint32_t RESERVED12;
  __IOM uint32_t GPIF_BETA_DEASSERT;            /*!< 0x00000218 Beta Deassert Register */
   __IM uint32_t RESERVED13;
  __IOM uint32_t GPIF_FUNCTION[32];             /*!< 0x00000220 Transition Function Registers */
   __IM uint32_t RESERVED14[4];
  __IOM uint32_t LINK_IDLE_CFG;                 /*!< 0x000002B0 Link Idle Config register */
   __IM uint32_t RESERVED15[3];
  __IOM uint32_t LVCMOS_CLK_OUT_CFG;            /*!< 0x000002C0 lvcmos clock out config register */
   __IM uint32_t RESERVED16[847];
        LVDSSS_LVDS_GPIF_LEFT_V1_Type LEFT;     /*!< 0x00001000 GPIF State Machine Left Waveform memory */
        LVDSSS_LVDS_GPIF_RIGHT_V1_Type RIGHT;   /*!< 0x00002000 GPIF State Machine Right Waveform memory. */
} LVDSSS_LVDS_GPIF_V1_Type;                     /*!< Size = 12288 (0x3000) */

/**
  * \brief Phy Configuration Registers (LVDSSS_LVDS_AFE)
  */
typedef struct {
  __IOM uint32_t DLL_DFTLPF;                    /*!< 0x00000000 DLL Low-Pass Filter Config Register */
  __IOM uint32_t DLL_CONFIG;                    /*!< 0x00000004 DLL Configuration Register */
   __IM uint32_t DLL_STATUS;                    /*!< 0x00000008 DLL Status Register */
  __IOM uint32_t DLL_M_CONFIG;                  /*!< 0x0000000C Master DLL Config Register */
   __IM uint32_t DLL_M_STATUS;                  /*!< 0x00000010 Master DLL Status Register */
  __IOM uint32_t DLL_S_CONFIG[12];              /*!< 0x00000014 Slave DLL Configuration */
   __IM uint32_t RESERVED[3];
   __IM uint32_t DLL_S_STATUS[12];              /*!< 0x00000050 Slave DLL Status */
  __IOM uint32_t GENERAL_LICIO_CIO;             /*!< 0x00000080 LICIO_CIO Config reg (common for all pads) */
  __IOM uint32_t LICIO_CIO[27];                 /*!< 0x00000084 LICIO_CIO Config reg (per pad) */
   __IM uint32_t RESERVED1[4];
  __IOM uint32_t GENERAL_LICIO_LI;              /*!< 0x00000100 LICIO_LI Config reg (common for all pads) */
  __IOM uint32_t LICIO_LI[10];                  /*!< 0x00000104 LICIO_LI Config reg (per pad) */
   __IM uint32_t RESERVED2;
  __IOM uint32_t LICIO_VSSIO_IREF;              /*!< 0x00000130 LICIO_VSSIO_IREF Config reg */
   __IM uint32_t LICIO_LI_OUT_STATUS;           /*!< 0x00000134 LICO_LI pad Rx output status */
  __IOM uint32_t PLL_CONFIG;                    /*!< 0x00000138 PLL Configuration register */
  __IOM uint32_t PLL_CONFIG_2;                  /*!< 0x0000013C PLL Configuration register2 */
   __IM uint32_t PLL_STATUS;                    /*!< 0x00000140 PLL Status Register */
  __IOM uint32_t REG_1P25;                      /*!< 0x00000144 REG1P25 Config reg */
  __IOM uint32_t RX[10];                        /*!< 0x00000148 LVDS RX Configuration Registers */
  __IOM uint32_t GENERAL_RX;                    /*!< 0x00000170 General RX Configuration Register */
  __IOM uint32_t GENERAL_RX_LVCMOS;             /*!< 0x00000174 General LVCMOS RX Configuration Register */
  __IOM uint32_t RX_LVCMOS[26];                 /*!< 0x00000178 LVCMOS RX Configuration Registers */
   __IM uint32_t RESERVED3[9];
  __IOM uint32_t PRBS_GEN;                      /*!< 0x00000204 State counter limit register */
  __IOM uint32_t PHY_GENERAL_CONFIG;            /*!< 0x00000208 Phy Configuration register */
   __IM uint32_t PHY_GENERAL_STATUS_1;          /*!< 0x0000020C Phy Status register (Loopback Status Lanes) */
   __IM uint32_t PHY_GENERAL_STATUS_2;          /*!< 0x00000210 Phy Status register */
  __IOM uint32_t PHY_INTR;                      /*!< 0x00000214 Phy Interrupt Register */
  __IOM uint32_t PHY_INTR_MASK;                 /*!< 0x00000218 Phy Interrupt Mask Register */
   __IM uint32_t PHY_INTR_MASKED;               /*!< 0x0000021C Phy Interrupt Masked Register */
  __IOM uint32_t PHY_INTR_SET;                  /*!< 0x00000220 Phy Interrupt Set Register */
  __IOM uint32_t PHY_TRAIN_CONFIG;              /*!< 0x00000224 Phy Training Mode Config register */
  __IOM uint32_t PHY_ADC_CONFIG;                /*!< 0x00000228 ADC config register */
  __IOM uint32_t PHY_DDFT_MUX_SEL;              /*!< 0x0000022C Digital Mux Select Register */
  __IOM uint32_t LICIO_VCCD_V1P1;               /*!< 0x00000230 LICIO_VCCD_V1P1 Config reg */
  __IOM uint32_t PHY_GPIO[26];                  /*!< 0x00000234 GPIO Config register */
   __IM uint32_t RESERVED4[26];
  __IOM uint32_t PHY_GPIO_INTR_CFG;             /*!< 0x00000304 GPIO Interrupt Config register */
   __IM uint32_t RESERVED5;
  __IOM uint32_t PHY_GENERAL_CONFIG_2;          /*!< 0x0000030C Phy Configuration Register2 */
   __IM uint32_t PRBS_STATUS;                   /*!< 0x00000310 Loopback error rate register */
  __IOM uint32_t LVDS_STATUS_MONITOR[10];       /*!< 0x00000314 LVDS Pin status monitor */
   __IM uint32_t RESERVED6;
   __IM uint32_t PHY_ADC_STATUS;                /*!< 0x00000340 ADC status register */
  __IOM uint32_t FRM_CLK_GLITCH_FILTER;         /*!< 0x00000344 Frame clock glitch filter configuration register */
   __IM uint32_t RESERVED7[46];
  __IOM uint32_t LICIO_CIO_CTRL_DS;             /*!< 0x00000400 LICIO_CIO control pad DS/SR register */
  __IOM uint32_t LICIO_CIO_DATA_DS[17];         /*!< 0x00000404 LICIO_CIO Data/Clk pad DS/SR register */
   __IM uint32_t RESERVED8[45];
  __IOM uint32_t DLY_CELL_LOAD;                 /*!< 0x000004FC Delay cell Load control register */
  __IOM uint32_t SAR_CLK_FREQ_CFG;              /*!< 0x00000500 SAR clock frequency configuration */
   __IM uint32_t STUCK_AT_CNTRS;                /*!< 0x00000504 Stuck-at fault counters */
  __IOM uint32_t STUCK_CNTR_RST_CTL;            /*!< 0x00000508 Stuck-at fault counters Reset control register */
   __IM uint32_t RESERVED9;
  __IOM uint32_t STUCK_CNTR_SIG_SEL_0;          /*!< 0x00000510 Stuck at fault counter signal select */
  __IOM uint32_t STUCK_CNTR_SIG_SEL_1;          /*!< 0x00000514 Stuck at fault counter signal select */
   __IM uint32_t RESERVED10[2];
  __IOM uint32_t CTL_PIN_SDLL_CFG[7];           /*!< 0x00000520 Control pin slave dll monitor config */
   __IM uint32_t RESERVED11;
   __IM uint32_t LVDS_BER_STATUS;               /*!< 0x00000540 LVDS rx_data_channel instance ber status */
   __IM uint32_t RESERVED12[175];
} LVDSSS_LVDS_AFE_V1_Type;                      /*!< Size = 2048 (0x800) */

/**
  * \brief Socket Registers (LVDSSS_LVDS_ADAPTER_DMA_SCK)
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
} LVDSSS_LVDS_ADAPTER_DMA_SCK_V1_Type;          /*!< Size = 128 (0x80) */

/**
  * \brief General DMA Registers (LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL)
  */
typedef struct {
   __IM uint32_t SCK_INTR;                      /*!< 0x00000000 Socket Interrupt Request Register */
   __IM uint32_t RESERVED[59];
  __IOM uint32_t ADAPTER_CTRL;                  /*!< 0x000000F0 Adapter Control Register */
   __IM uint32_t ADAPTER_DEBUG;                 /*!< 0x000000F4 Adapter Debug Observation Register */
  __IOM uint32_t ADAPTER_CONF;                  /*!< 0x000000F8 Adapter Configuration Register */
   __IM uint32_t ADAPTER_STATUS;                /*!< 0x000000FC Adapter Global Status Fields */
} LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_V1_Type;      /*!< Size = 256 (0x100) */

/**
  * \brief LVDS Adapter Registers
0x1_0000 is for Adapter-0 and 0x2_0000 is for Adapter-1 (LVDSSS_LVDS_ADAPTER_DMA)
  */
typedef struct {
   __IM uint32_t RESERVED[8192];
        LVDSSS_LVDS_ADAPTER_DMA_SCK_V1_Type SCK[16]; /*!< 0x00008000 Socket Registers */
   __IM uint32_t RESERVED1[7616];
        LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_V1_Type SCK_GBL; /*!< 0x0000FF00 General DMA Registers */
} LVDSSS_LVDS_ADAPTER_DMA_V1_Type;              /*!< Size = 65536 (0x10000) */

/**
  * \brief LVDS IP register map (LVDSSS_LVDS)
  */
typedef struct {
  __IOM uint32_t CTL;                           /*!< 0x00000000 IP Control Register */
  __IOM uint32_t LINK_CONFIG[2];                /*!< 0x00000004 Link Configuration Register */
  __IOM uint32_t THREAD_INTLV_CTL;              /*!< 0x0000000C Thread interleave control register */
  __IOM uint32_t LVDS_INTR_WD0;                 /*!< 0x00000010 LVDS Interrupt Request Register */
  __IOM uint32_t LVDS_INTR_WD1;                 /*!< 0x00000014 LVDS Interrupt Request Register */
   __IM uint32_t RESERVED[2];
  __IOM uint32_t LVDS_INTR_MASK_WD0;            /*!< 0x00000020 LVDS Interrupt Mask Register */
  __IOM uint32_t LVDS_INTR_MASK_WD1;            /*!< 0x00000024 LVDS Interrupt Mask Register */
   __IM uint32_t RESERVED1[2];
   __IM uint32_t LVDS_INTR_MASKED_WD0;          /*!< 0x00000030 LVDS Interrupt Generator */
   __IM uint32_t LVDS_INTR_MASKED_WD1;          /*!< 0x00000034 LVDS Interrupt Generator */
   __IM uint32_t RESERVED2[2];
  __IOM uint32_t LVDS_INTR_SET_WD0;             /*!< 0x00000040 LVDS Interrupt Set Register */
  __IOM uint32_t LVDS_INTR_SET_WD1;             /*!< 0x00000044 LVDS Interrupt Set Register */
   __IM uint32_t RESERVED3[2];
   __IM uint32_t LVDS_ERROR;                    /*!< 0x00000050 LVDS Error Indicator Register */
  __IOM uint32_t LVDS_EOP_EOT;                  /*!< 0x00000054 LVDS EOP/EOT configuration */
  __IOM uint32_t LANE_FIFO_STS[2];              /*!< 0x00000058 Lane fifo status Register */
  __IOM uint32_t GPIF_CLK_SEL[2];               /*!< 0x00000060 GPIF Clock selection Register */
   __IM uint32_t RESERVED4;
  __IOM uint32_t USB_FRM_CNTR_CFG;              /*!< 0x0000006C USB frame counter config */
   __IM uint32_t TIME_STAMP_CLK_DW0;            /*!< 0x00000070 Timestamp nanosecond counter */
   __IM uint32_t TIME_STAMP_CLK_DW1;            /*!< 0x00000074 Timestamp nanosecond counter */
   __IM uint32_t RESERVED5[2];
  __IOM uint32_t SCRSS_VALUE_CFG[2];            /*!< 0x00000080 SCRSS value configuration select */
   __IM uint32_t RESERVED6[2];
  __IOM uint32_t TRAINING_BLK_CFG;              /*!< 0x00000090 Training block configuration */
  __IOM uint32_t TRAINING_BLK;                  /*!< 0x00000094 Training block sequence bytes */
  __IOM uint32_t LINK_TRAINING_STS[2];          /*!< 0x00000098 Training block detection status register */
  __IOM uint32_t DDFT_MUX_SEL;                  /*!< 0x000000A0 Debug Mux select */
  __IOM uint32_t GPIO_DDFT_MUX_SEL;             /*!< 0x000000A4 Debug Mux select */
  __IOM uint32_t LOOPBACK_CFG;                  /*!< 0x000000A8 Loopback Configuration register */
   __IM uint32_t RESERVED7;
  __IOM uint32_t LVDS_CHAR_CFG[2];              /*!< 0x000000B0 LVDS lanes characterisation config register */
  __IOM uint32_t CLK_GATE_DIS;                  /*!< 0x000000B8 Thread controller Clock gate disable config register */
  __IOM uint32_t VERSION_SEL;                   /*!< 0x000000BC lvds ip version select */
        LVDSSS_LVDS_LVDS_LOW_PWR_INTR_V1_Type LVDS_LOW_PWR_INTR; /*!< 0x000000C0 LVDS Low power and Wakeup Interrupt registers */
   __IM uint32_t RESERVED8[8];
  __IOM uint32_t TH0_TH1_METADATA_RAM[64];      /*!< 0x00000100 Metadata Memory */
  __IOM uint32_t TH2_TH3_METADATA_RAM[64];      /*!< 0x00000200 Metadata Memory */
   __IM uint32_t RESERVED9[64];
        LVDSSS_LVDS_THREAD_V1_Type THREAD[4];   /*!< 0x00000400 Set of registers for a thread */
   __IM uint32_t RESERVED10[256];
        LVDSSS_LVDS_GPIF_V1_Type GPIF[2];       /*!< 0x00001000 GPIF-II Configuration Registers */
        LVDSSS_LVDS_AFE_V1_Type AFE[2];         /*!< 0x00007000 Phy Configuration Registers */
   __IM uint32_t RESERVED11[8192];
        LVDSSS_LVDS_ADAPTER_DMA_V1_Type ADAPTER_DMA[2]; /*!< 0x00010000 LVDS Adapter Registers 0x1_0000 is for Adapter-0 and 0x2_0000
                                                                is for Adapter-1 */
   __IM uint32_t RESERVED12[16384];
} LVDSSS_LVDS_V1_Type;                          /*!< Size = 262144 (0x40000) */

/**
  * \brief LVDS IP Registers (LVDSSS)
  */
typedef struct {
        LVDSSS_LVDS_V1_Type LVDS;               /*!< 0x00000000 LVDS IP register map */
} LVDSSS_V1_Type;                               /*!< Size = 262144 (0x40000) */


/* LVDSSS_LVDS_LVDS_LOW_PWR_INTR.LVDS_WAKEUP_INTR */
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_LNK0_L3_EXIT_Pos 0UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_LNK0_L3_EXIT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_LNK1_L3_EXIT_Pos 1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_LNK1_L3_EXIT_Msk 0x2UL
/* LVDSSS_LVDS_LVDS_LOW_PWR_INTR.LVDS_WAKEUP_INTR_MASK */
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASK_LNK0_L3_EXIT_Pos 0UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASK_LNK0_L3_EXIT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASK_LNK1_L3_EXIT_Pos 1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASK_LNK1_L3_EXIT_Msk 0x2UL
/* LVDSSS_LVDS_LVDS_LOW_PWR_INTR.LVDS_WAKEUP_INTR_MASKED */
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASKED_LNK0_L3_EXIT_Pos 0UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASKED_LNK0_L3_EXIT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASKED_LNK1_L3_EXIT_Pos 1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_MASKED_LNK1_L3_EXIT_Msk 0x2UL
/* LVDSSS_LVDS_LVDS_LOW_PWR_INTR.LVDS_WAKEUP_INTR_SET */
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_SET_LNK0_L3_EXIT_Pos 0UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_SET_LNK0_L3_EXIT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_SET_LNK1_L3_EXIT_Pos 1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LVDS_WAKEUP_INTR_SET_LNK1_L3_EXIT_Msk 0x2UL
/* LVDSSS_LVDS_LVDS_LOW_PWR_INTR.LOW_PWR_CTL */
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_MASK_L1_ENTRY_Pos 0UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_MASK_L1_ENTRY_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_MASK_L3_ENTRY_Pos 1UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_MASK_L3_ENTRY_Msk 0x2UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_AUTO_MODE_Pos 4UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_AUTO_MODE_Msk 0x10UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_SEL_ON_OFF_Pos 5UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_SEL_ON_OFF_Msk 0x20UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_IO_OFF_VALUE_Pos 6UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_IO_OFF_VALUE_Msk 0x40UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_IO_ON_VALUE_Pos 7UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_IO_ON_VALUE_Msk 0x80UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_L3_GPIO_CTL_Pos 8UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_L3_GPIO_CTL_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_L3_TO_IO_OFF_DLY_Pos 16UL
#define LVDSSS_LVDS_LVDS_LOW_PWR_INTR_LOW_PWR_CTL_L3_TO_IO_OFF_DLY_Msk 0x7F0000UL


/* LVDSSS_LVDS_THREAD.GPIF_DATA_CTRL */
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_ING_DATA_VALID_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_ING_DATA_VALID_Msk 0x1UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_EG_DATA_VALID_Pos 1UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_EG_DATA_VALID_Msk 0x2UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_IN_ADDR_VALID_Pos 2UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_IN_ADDR_VALID_Msk 0x4UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_EG_ADDR_VALID_Pos 3UL
#define LVDSSS_LVDS_THREAD_GPIF_DATA_CTRL_EG_ADDR_VALID_Msk 0x8UL
/* LVDSSS_LVDS_THREAD.GPIF_INGRESS_DATA_WORD0 */
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD0_DATA_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD0_DATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_INGRESS_DATA_WORD1 */
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD1_DATA_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD1_DATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_INGRESS_DATA_WORD2 */
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD2_DATA_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD2_DATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_INGRESS_DATA_WORD3 */
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD3_DATA_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_DATA_WORD3_DATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_EGRESS_DATA_WORD0 */
#define LVDSSS_LVDS_THREAD_GPIF_EGRESS_DATA_WORD0_DATA_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_EGRESS_DATA_WORD0_DATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_EGRESS_DATA_WORD1 */
#define LVDSSS_LVDS_THREAD_GPIF_EGRESS_DATA_WORD1_DATA_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_EGRESS_DATA_WORD1_DATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_INGRESS_ADDRESS */
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_ADDRESS_ADDRESS_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_INGRESS_ADDRESS_ADDRESS_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_EGRESS_ADDRESS */
#define LVDSSS_LVDS_THREAD_GPIF_EGRESS_ADDRESS_ADDRESS_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_EGRESS_ADDRESS_ADDRESS_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_THREAD.GPIF_THREAD_CONFIG */
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_THREAD_NUM_Pos 0UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_THREAD_NUM_Msk 0x1UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_THREAD_SOCK_Pos 1UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_THREAD_SOCK_Msk 0xEUL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_ADAPTOR_NUM_Pos 4UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_ADAPTOR_NUM_Msk 0x10UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_WM_CFG_Pos 7UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_WM_CFG_Msk 0x80UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_BURST_SIZE_Pos 8UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_BURST_SIZE_Msk 0xF00UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_WATERMARK_Pos 16UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_WATERMARK_Msk 0x3FFF0000UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_ENABLE_THREAD_CTLR_Pos 31UL
#define LVDSSS_LVDS_THREAD_GPIF_THREAD_CONFIG_ENABLE_THREAD_CTLR_Msk 0x80000000UL
/* LVDSSS_LVDS_THREAD.VARW0 */
#define LVDSSS_LVDS_THREAD_VARW0_VARW0_Pos      0UL
#define LVDSSS_LVDS_THREAD_VARW0_VARW0_Msk      0xFFFFUL
/* LVDSSS_LVDS_THREAD.VARW1 */
#define LVDSSS_LVDS_THREAD_VARW1_VARW1_Pos      0UL
#define LVDSSS_LVDS_THREAD_VARW1_VARW1_Msk      0xFFFFUL
/* LVDSSS_LVDS_THREAD.PTSSS */
#define LVDSSS_LVDS_THREAD_PTSSS_PTSSS_WD0_Pos  0UL
#define LVDSSS_LVDS_THREAD_PTSSS_PTSSS_WD0_Msk  0xFFFFUL
#define LVDSSS_LVDS_THREAD_PTSSS_PTSSS_WD1_Pos  16UL
#define LVDSSS_LVDS_THREAD_PTSSS_PTSSS_WD1_Msk  0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.SCRSS_DW0 */
#define LVDSSS_LVDS_THREAD_SCRSS_DW0_SCRSS_WD0_Pos 0UL
#define LVDSSS_LVDS_THREAD_SCRSS_DW0_SCRSS_WD0_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_SCRSS_DW0_SCRSS_WD1_Pos 16UL
#define LVDSSS_LVDS_THREAD_SCRSS_DW0_SCRSS_WD1_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.SCRSS_DW1 */
#define LVDSSS_LVDS_THREAD_SCRSS_DW1_SCRSS_WD2_Pos 0UL
#define LVDSSS_LVDS_THREAD_SCRSS_DW1_SCRSS_WD2_Msk 0xFFFFUL
/* LVDSSS_LVDS_THREAD.HDR_FLGS */
#define LVDSSS_LVDS_THREAD_HDR_FLGS_FLG_Pos     0UL
#define LVDSSS_LVDS_THREAD_HDR_FLGS_FLG_Msk     0xFUL
/* LVDSSS_LVDS_THREAD.EVC_VAR0_DW0 */
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW0_EVC_V0_WD0_Pos 0UL
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW0_EVC_V0_WD0_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW0_EVC_V0_WD1_Pos 16UL
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW0_EVC_V0_WD1_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.EVC_VAR0_DW1 */
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW1_EVC_V0_WD2_Pos 0UL
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW1_EVC_V0_WD2_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW1_EVC_V0_WD3_Pos 16UL
#define LVDSSS_LVDS_THREAD_EVC_VAR0_DW1_EVC_V0_WD3_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.EVC_VAR1_DW0 */
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW0_EVC_V1_WD0_Pos 0UL
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW0_EVC_V1_WD0_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW0_EVC_V1_WD1_Pos 16UL
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW0_EVC_V1_WD1_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.EVC_VAR1_DW1 */
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW1_EVC_V1_WD2_Pos 0UL
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW1_EVC_V1_WD2_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW1_EVC_V1_WD3_Pos 16UL
#define LVDSSS_LVDS_THREAD_EVC_VAR1_DW1_EVC_V1_WD3_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.PLC_DW0 */
#define LVDSSS_LVDS_THREAD_PLC_DW0_PLC_WD0_Pos  0UL
#define LVDSSS_LVDS_THREAD_PLC_DW0_PLC_WD0_Msk  0xFFFFUL
#define LVDSSS_LVDS_THREAD_PLC_DW0_PLC_WD1_Pos  16UL
#define LVDSSS_LVDS_THREAD_PLC_DW0_PLC_WD1_Msk  0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.PLC_DW1 */
#define LVDSSS_LVDS_THREAD_PLC_DW1_PLC_WD2_Pos  0UL
#define LVDSSS_LVDS_THREAD_PLC_DW1_PLC_WD2_Msk  0xFFFFUL
#define LVDSSS_LVDS_THREAD_PLC_DW1_PLC_WD3_Pos  16UL
#define LVDSSS_LVDS_THREAD_PLC_DW1_PLC_WD3_Msk  0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.PAYLOAD_CFG */
#define LVDSSS_LVDS_THREAD_PAYLOAD_CFG_CNT_MDATA_AS_PAYLOAD_Pos 0UL
#define LVDSSS_LVDS_THREAD_PAYLOAD_CFG_CNT_MDATA_AS_PAYLOAD_Msk 0x1UL
#define LVDSSS_LVDS_THREAD_PAYLOAD_CFG_CNT_CRC_AS_PAYLOAD_Pos 1UL
#define LVDSSS_LVDS_THREAD_PAYLOAD_CFG_CNT_CRC_AS_PAYLOAD_Msk 0x2UL
/* LVDSSS_LVDS_THREAD.DCRC_CONFIG */
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_BIT_ENDIAN_Pos 0UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_BIT_ENDIAN_Msk 0x1UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_BYTE_ENDIAN_Pos 1UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_BYTE_ENDIAN_Msk 0x2UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_CRC_ENABLE_Pos 2UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_CRC_ENABLE_Msk 0x4UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_CRC_VAL_BIT_ENDIAN_Pos 4UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_CRC_VAL_BIT_ENDIAN_Msk 0x10UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_CRC_VAL_BYTE_ENDIAN_Pos 5UL
#define LVDSSS_LVDS_THREAD_DCRC_CONFIG_CRC_VAL_BYTE_ENDIAN_Msk 0x20UL
/* LVDSSS_LVDS_THREAD.DCRC */
#define LVDSSS_LVDS_THREAD_DCRC_INITIAL_VALUE_Pos 0UL
#define LVDSSS_LVDS_THREAD_DCRC_INITIAL_VALUE_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_DCRC_CRC_VALUE_Pos   16UL
#define LVDSSS_LVDS_THREAD_DCRC_CRC_VALUE_Msk   0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.DCRC_STS */
#define LVDSSS_LVDS_THREAD_DCRC_STS_CRC_RECEIVED_Pos 0UL
#define LVDSSS_LVDS_THREAD_DCRC_STS_CRC_RECEIVED_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_DCRC_STS_CRC_ERR_CNT_Pos 16UL
#define LVDSSS_LVDS_THREAD_DCRC_STS_CRC_ERR_CNT_Msk 0xFF0000UL
#define LVDSSS_LVDS_THREAD_DCRC_STS_CRC_ERROR_Pos 31UL
#define LVDSSS_LVDS_THREAD_DCRC_STS_CRC_ERROR_Msk 0x80000000UL
/* LVDSSS_LVDS_THREAD.MCRC_CONFIG */
#define LVDSSS_LVDS_THREAD_MCRC_CONFIG_MCRC_BIT_ENDIAN_Pos 0UL
#define LVDSSS_LVDS_THREAD_MCRC_CONFIG_MCRC_BIT_ENDIAN_Msk 0x1UL
#define LVDSSS_LVDS_THREAD_MCRC_CONFIG_MCRC_BYTE_ENDIAN_Pos 1UL
#define LVDSSS_LVDS_THREAD_MCRC_CONFIG_MCRC_BYTE_ENDIAN_Msk 0x2UL
#define LVDSSS_LVDS_THREAD_MCRC_CONFIG_MCRC_ENABLE_Pos 2UL
#define LVDSSS_LVDS_THREAD_MCRC_CONFIG_MCRC_ENABLE_Msk 0x4UL
/* LVDSSS_LVDS_THREAD.MCRC */
#define LVDSSS_LVDS_THREAD_MCRC_INITIAL_VALUE_Pos 0UL
#define LVDSSS_LVDS_THREAD_MCRC_INITIAL_VALUE_Msk 0xFFFFUL
#define LVDSSS_LVDS_THREAD_MCRC_CRC_VALUE_Pos   16UL
#define LVDSSS_LVDS_THREAD_MCRC_CRC_VALUE_Msk   0xFFFF0000UL
/* LVDSSS_LVDS_THREAD.THREAD_DMA_INTF_CFG */
#define LVDSSS_LVDS_THREAD_THREAD_DMA_INTF_CFG_RQ_EMPTY_DLY_Pos 0UL
#define LVDSSS_LVDS_THREAD_THREAD_DMA_INTF_CFG_RQ_EMPTY_DLY_Msk 0x7UL
/* LVDSSS_LVDS_THREAD.MD0_CTRL */
#define LVDSSS_LVDS_THREAD_MD0_CTRL_MD_REF_CTL_Pos 0UL
#define LVDSSS_LVDS_THREAD_MD0_CTRL_MD_REF_CTL_Msk 0x7FFFFFFUL
#define LVDSSS_LVDS_THREAD_MD0_CTRL_MD_SIZE_Pos 27UL
#define LVDSSS_LVDS_THREAD_MD0_CTRL_MD_SIZE_Msk 0xF8000000UL
/* LVDSSS_LVDS_THREAD.MD1_CTRL */
#define LVDSSS_LVDS_THREAD_MD1_CTRL_MD_REF_CTL_Pos 0UL
#define LVDSSS_LVDS_THREAD_MD1_CTRL_MD_REF_CTL_Msk 0x7FFFFFFUL
#define LVDSSS_LVDS_THREAD_MD1_CTRL_MD_SIZE_Pos 27UL
#define LVDSSS_LVDS_THREAD_MD1_CTRL_MD_SIZE_Msk 0xF8000000UL
/* LVDSSS_LVDS_THREAD.MD2_CTRL */
#define LVDSSS_LVDS_THREAD_MD2_CTRL_MD_REF_CTL_Pos 0UL
#define LVDSSS_LVDS_THREAD_MD2_CTRL_MD_REF_CTL_Msk 0x7FFFFFFUL
#define LVDSSS_LVDS_THREAD_MD2_CTRL_MD_SIZE_Pos 27UL
#define LVDSSS_LVDS_THREAD_MD2_CTRL_MD_SIZE_Msk 0xF8000000UL
/* LVDSSS_LVDS_THREAD.MD3_CTRL */
#define LVDSSS_LVDS_THREAD_MD3_CTRL_MD_REF_CTL_Pos 0UL
#define LVDSSS_LVDS_THREAD_MD3_CTRL_MD_REF_CTL_Msk 0x7FFFFFFUL
#define LVDSSS_LVDS_THREAD_MD3_CTRL_MD_SIZE_Pos 27UL
#define LVDSSS_LVDS_THREAD_MD3_CTRL_MD_SIZE_Msk 0xF8000000UL


/* LVDSSS_LVDS_GPIF_LEFT_WAVEFORM.WAVEFORM0 */
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_NEXT_STATE_Pos 0UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_NEXT_STATE_Msk 0xFFUL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FA_Pos 8UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FA_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FB_Pos 13UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FB_Msk 0x3E000UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FC_Pos 18UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FC_Msk 0x7C0000UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FD_Pos 23UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_FD_Msk 0xF800000UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_F0_L_Pos 28UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM0_F0_L_Msk 0xF0000000UL
/* LVDSSS_LVDS_GPIF_LEFT_WAVEFORM.WAVEFORM1 */
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_F0_U_Pos 0UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_F0_U_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_F1_Pos 1UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_F1_Msk 0x3EUL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_ALPHA_LEFT_Pos 6UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_ALPHA_LEFT_Msk 0x3FC0UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_ALPHA_RIGHT_Pos 14UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_ALPHA_RIGHT_Msk 0x3FC000UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_BETA_L_Pos 22UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM1_BETA_L_Msk 0xFFC00000UL
/* LVDSSS_LVDS_GPIF_LEFT_WAVEFORM.WAVEFORM2 */
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_BETA_U_Pos 0UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_BETA_U_Msk 0x3FFFFFUL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_REPEAT_COUNT_Pos 22UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_REPEAT_COUNT_Msk 0x3FC00000UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_BETA_DEASSERT_Pos 30UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_BETA_DEASSERT_Msk 0x40000000UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_VALID_Pos 31UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM2_VALID_Msk 0x80000000UL
/* LVDSSS_LVDS_GPIF_LEFT_WAVEFORM.WAVEFORM3 */
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FA_5_Pos 0UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FA_5_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FB_5_Pos 1UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FB_5_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FC_5_Pos 2UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FC_5_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FD_5_Pos 3UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_FD_5_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_RESERVED_Pos 4UL
#define LVDSSS_LVDS_GPIF_LEFT_WAVEFORM_WAVEFORM3_RESERVED_Msk 0xFFFFFFF0UL


/* LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM.WAVEFORM0 */
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_NEXT_STATE_Pos 0UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_NEXT_STATE_Msk 0xFFUL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FA_Pos 8UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FA_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FB_Pos 13UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FB_Msk 0x3E000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FC_Pos 18UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FC_Msk 0x7C0000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FD_Pos 23UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_FD_Msk 0xF800000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_F0_L_Pos 28UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM0_F0_L_Msk 0xF0000000UL
/* LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM.WAVEFORM1 */
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_F0_U_Pos 0UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_F0_U_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_F1_Pos 1UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_F1_Msk 0x3EUL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_ALPHA_LEFT_Pos 6UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_ALPHA_LEFT_Msk 0x3FC0UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_ALPHA_RIGHT_Pos 14UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_ALPHA_RIGHT_Msk 0x3FC000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_BETA_L_Pos 22UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM1_BETA_L_Msk 0xFFC00000UL
/* LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM.WAVEFORM2 */
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_BETA_U_Pos 0UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_BETA_U_Msk 0x3FFFFFUL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_REPEAT_COUNT_Pos 22UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_REPEAT_COUNT_Msk 0x3FC00000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_BETA_DEASSERT_Pos 30UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_BETA_DEASSERT_Msk 0x40000000UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_VALID_Pos 31UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM2_VALID_Msk 0x80000000UL
/* LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM.WAVEFORM3 */
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FA_5_Pos 0UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FA_5_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FB_5_Pos 1UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FB_5_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FC_5_Pos 2UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FC_5_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FD_5_Pos 3UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_FD_5_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_RESERVED_Pos 4UL
#define LVDSSS_LVDS_GPIF_RIGHT_WAVEFORM_WAVEFORM3_RESERVED_Msk 0xFFFFFFF0UL


/* LVDSSS_LVDS_GPIF.GPIF_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_CTRL_COMP_ENABLE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_CTRL_COMP_ENABLE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_ADDR_COMP_ENABLE_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_ADDR_COMP_ENABLE_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_DATA_COMP_ENABLE_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_DATA_COMP_ENABLE_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_CLK_SOURCE_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_CLK_SOURCE_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_SPARE0_Pos 5UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_SPARE0_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_EN_CLK_IN_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_EN_CLK_IN_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_DOUT_POP_EN_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_DOUT_POP_EN_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_ENDIAN_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_ENDIAN_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_ADDR_COMP_TOGGLE_Pos 11UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_ADDR_COMP_TOGGLE_Msk 0x800UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_CTRL_COMP_TOGGLE_Pos 12UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_CTRL_COMP_TOGGLE_Msk 0x1000UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_DATA_COMP_TOGGLE_Pos 13UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_DATA_COMP_TOGGLE_Msk 0x2000UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_THREAD_IN_STATE_Pos 15UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_THREAD_IN_STATE_Msk 0x8000UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_THREAD_IN_CTL_BYT_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_THREAD_IN_CTL_BYT_Msk 0x10000UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_SOCKET_IN_CTL_BYT_Pos 17UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_SOCKET_IN_CTL_BYT_Msk 0x20000UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_EOP_4M_FSM_Pos 18UL
#define LVDSSS_LVDS_GPIF_GPIF_CONFIG_EOP_4M_FSM_Msk 0x40000UL
/* LVDSSS_LVDS_GPIF.GPIF_BUS_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH0_PTS_TRIG_PRESENT_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH0_PTS_TRIG_PRESENT_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH1_PTS_TRIG_PRESENT_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH1_PTS_TRIG_PRESENT_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH2_PTS_TRIG_PRESENT_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH2_PTS_TRIG_PRESENT_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH3_PTS_TRIG_PRESENT_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_TH3_PTS_TRIG_PRESENT_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_ADR_CTRL_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_ADR_CTRL_Msk 0x1F0UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_CE_PRESENT_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_CE_PRESENT_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_WE_PRESENT_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_WE_PRESENT_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_EM_VLD_PRESENT_Pos 11UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_EM_VLD_PRESENT_Msk 0x800UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_OE_PRESENT_Pos 12UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_OE_PRESENT_Msk 0x1000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_EM_VLD_MASK_CS_WR_Pos 13UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_EM_VLD_MASK_CS_WR_Msk 0x2000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO0_PRESENT_Pos 14UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO0_PRESENT_Msk 0x4000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO1_PRESENT_Pos 15UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO1_PRESENT_Msk 0x8000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_CNTR_PRESENT_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_CNTR_PRESENT_Msk 0x10000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_SPARE2_Pos 18UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_SPARE2_Msk 0xC0000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_SPARE3_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_SPARE3_Msk 0x100000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO0_CONF_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO0_CONF_Msk 0xF000000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO1_CONF_Pos 28UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG_FIO1_CONF_Msk 0xF0000000UL
/* LVDSSS_LVDS_GPIF.GPIF_BUS_CONFIG2 */
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE_FROM_CTRL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE_FROM_CTRL_Msk 0x7UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE5_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE5_Msk 0x3F00UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE6_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE6_Msk 0x3F0000UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE7_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_BUS_CONFIG2_STATE7_Msk 0x3F000000UL
/* LVDSSS_LVDS_GPIF.GPIF_AD_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DQ_OEN_CFG_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DQ_OEN_CFG_Msk 0x3UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_A_OEN_CFG_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_A_OEN_CFG_Msk 0xCUL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_AIN_SELECT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_AIN_SELECT_Msk 0x30UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_AOUT_SELECT_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_AOUT_SELECT_Msk 0xC0UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DOUT_SELECT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DOUT_SELECT_Msk 0x100UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_AIN_DATA_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_AIN_DATA_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_ADDRESS_THREAD_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_ADDRESS_THREAD_Msk 0x30000UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DATA_THREAD_Pos 18UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DATA_THREAD_Msk 0xC0000UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DQ_INP_EN_CFG_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_DQ_INP_EN_CFG_Msk 0x300000UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_A_INP_EN_CFG_Pos 22UL
#define LVDSSS_LVDS_GPIF_GPIF_AD_CONFIG_A_INP_EN_CFG_Msk 0xC00000UL
/* LVDSSS_LVDS_GPIF.GPIF_CTL_FUNC0 */
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_CE_CTL_SEL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_CE_CTL_SEL_Msk 0x1FUL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_WE_CTL_SEL_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_WE_CTL_SEL_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_OE_CTL_SEL_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_OE_CTL_SEL_Msk 0x1F0000UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_L3_ENTRY_SEL_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC0_L3_ENTRY_SEL_Msk 0x1F000000UL
/* LVDSSS_LVDS_GPIF.GPIF_CTL_FUNC1 */
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_PKTEND_SEL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_PKTEND_SEL_Msk 0x1FUL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_EM_VLD_CTL_SEL_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_EM_VLD_CTL_SEL_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_TH0_PTS_TRIG_CTL_SEL_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_TH0_PTS_TRIG_CTL_SEL_Msk 0x1F0000UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_TH1_PTS_TRIG_CTL_SEL_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC1_TH1_PTS_TRIG_CTL_SEL_Msk 0x1F000000UL
/* LVDSSS_LVDS_GPIF.GPIF_CTL_FUNC2 */
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_TH2_PTS_TRIG_CTL_SEL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_TH2_PTS_TRIG_CTL_SEL_Msk 0x1FUL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_TH3_PTS_TRIG_CTL_SEL_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_TH3_PTS_TRIG_CTL_SEL_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_CTL_2_LAMBDA10_SEL_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_CTL_2_LAMBDA10_SEL_Msk 0x1F0000UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_CTL_2_LAMBDA11_SEL_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC2_CTL_2_LAMBDA11_SEL_Msk 0x1F000000UL
/* LVDSSS_LVDS_GPIF.GPIF_CTL_FUNC3 */
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA12_SEL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA12_SEL_Msk 0x1FUL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA13_SEL_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA13_SEL_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA14_SEL_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA14_SEL_Msk 0x1F0000UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA15_SEL_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC3_CTL_2_LAMBDA15_SEL_Msk 0x1F000000UL
/* LVDSSS_LVDS_GPIF.GPIF_CTL_FUNC4 */
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA16_SEL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA16_SEL_Msk 0x1FUL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA17_SEL_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA17_SEL_Msk 0x1F00UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA18_SEL_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA18_SEL_Msk 0x1F0000UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA19_SEL_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_CTL_FUNC4_CTL_2_LAMBDA19_SEL_Msk 0x1F000000UL
/* LVDSSS_LVDS_GPIF.GPIF_STATUS */
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_GPIF_DONE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_GPIF_DONE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_GPIF_FSM_INTR_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_GPIF_FSM_INTR_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_SWITCH_TIMEOUT_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_SWITCH_TIMEOUT_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_CRC_ERROR_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_CRC_ERROR_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_ADDR_COUNT_HIT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_ADDR_COUNT_HIT_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_DATA_COUNT_HIT_Pos 5UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_DATA_COUNT_HIT_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_CTRL_COUNT_HIT_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_CTRL_COUNT_HIT_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_ADDR_COMP_HIT_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_ADDR_COMP_HIT_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_DATA_COMP_HIT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_DATA_COMP_HIT_Msk 0x100UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_CTRL_COMP_HIT_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_CTRL_COMP_HIT_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_WAVEFORM_BUSY_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_WAVEFORM_BUSY_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_EG_DATA_EMPTY_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_EG_DATA_EMPTY_Msk 0xF0000UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_IN_DATA_VALID_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_IN_DATA_VALID_Msk 0xF00000UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_INTERRUPT_STATE_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_STATUS_INTERRUPT_STATE_Msk 0xFF000000UL
/* LVDSSS_LVDS_GPIF.GPIF_INTR */
#define LVDSSS_LVDS_GPIF_GPIF_INTR_GPIF_DONE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_GPIF_DONE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_GPIF_FSM_INTR_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_GPIF_FSM_INTR_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SWITCH_TIMEOUT_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SWITCH_TIMEOUT_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_CRC_ERROR_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_CRC_ERROR_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_ADDR_COUNT_HIT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_ADDR_COUNT_HIT_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_DATA_COUNT_HIT_Pos 5UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_DATA_COUNT_HIT_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_CTRL_COUNT_HIT_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_CTRL_COUNT_HIT_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_ADDR_COMP_HIT_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_ADDR_COMP_HIT_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_DATA_COMP_HIT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_DATA_COMP_HIT_Msk 0x100UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_CTRL_COMP_HIT_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_CTRL_COMP_HIT_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_WAVEFORM_BUSY_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_WAVEFORM_BUSY_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT0_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT0_Msk 0x10000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT1_Pos 17UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT1_Msk 0x20000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT2_Pos 18UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT2_Msk 0x40000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT3_Pos 19UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_EG_DATA_EMPTY_BIT3_Msk 0x80000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT0_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT0_Msk 0x100000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT1_Pos 21UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT1_Msk 0x200000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT2_Pos 22UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT2_Msk 0x400000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT3_Pos 23UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_IN_DATA_VALID_BIT3_Msk 0x800000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_GPIF_ERR_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_GPIF_ERR_Msk 0x1000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_INVLD_CMD_DET_Pos 25UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_INVLD_CMD_DET_Msk 0x2000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_LINK_IDLE_Pos 26UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_LINK_IDLE_Msk 0x4000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SPARE4_Pos   27UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SPARE4_Msk   0x8000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SPARE5_Pos   28UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SPARE5_Msk   0x10000000UL
/* LVDSSS_LVDS_GPIF.GPIF_INTR_MASK */
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_GPIF_DONE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_GPIF_DONE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_GPIF_FSM_INTR_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_GPIF_FSM_INTR_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_SWITCH_TIMEOUT_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_SWITCH_TIMEOUT_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_CRC_ERROR_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_CRC_ERROR_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_ADDR_COUNT_HIT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_ADDR_COUNT_HIT_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_DATA_COUNT_HIT_Pos 5UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_DATA_COUNT_HIT_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_CTRL_COUNT_HIT_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_CTRL_COUNT_HIT_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_ADDR_COMP_HIT_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_ADDR_COMP_HIT_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_DATA_COMP_HIT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_DATA_COMP_HIT_Msk 0x100UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_CTRL_COMP_HIT_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_CTRL_COMP_HIT_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_WAVEFORM_BUSY_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_WAVEFORM_BUSY_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_EG_DATA_EMPTY_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_EG_DATA_EMPTY_Msk 0xF0000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_IN_DATA_VALID_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_IN_DATA_VALID_Msk 0xF00000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_GPIF_ERR_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_GPIF_ERR_Msk 0x1000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_INVLD_CMD_DET_Pos 25UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_INVLD_CMD_DET_Msk 0x2000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_LINK_IDLE_Pos 26UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_LINK_IDLE_Msk 0x4000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_SPARE4_Pos 27UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_SPARE4_Msk 0x8000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_SPARE5_Pos 28UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASK_SPARE5_Msk 0x10000000UL
/* LVDSSS_LVDS_GPIF.GPIF_INTR_MASKED */
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_GPIF_DONE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_GPIF_DONE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_GPIF_FSM_INTR_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_GPIF_FSM_INTR_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_SWITCH_TIMEOUT_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_SWITCH_TIMEOUT_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_CRC_ERROR_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_CRC_ERROR_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_ADDR_COUNT_HIT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_ADDR_COUNT_HIT_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_DATA_COUNT_HIT_Pos 5UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_DATA_COUNT_HIT_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_CTRL_COUNT_HIT_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_CTRL_COUNT_HIT_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_ADDR_COMP_HIT_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_ADDR_COMP_HIT_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_DATA_COMP_HIT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_DATA_COMP_HIT_Msk 0x100UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_CTRL_COMP_HIT_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_CTRL_COMP_HIT_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_WAVEFORM_BUSY_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_WAVEFORM_BUSY_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_EG_DATA_EMPTY_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_EG_DATA_EMPTY_Msk 0xF0000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_IN_DATA_VALID_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_IN_DATA_VALID_Msk 0xF00000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_GPIF_ERR_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_GPIF_ERR_Msk 0x1000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_INVLD_CMD_DET_Pos 25UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_INVLD_CMD_DET_Msk 0x2000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_LINK_IDLE_Pos 26UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_LINK_IDLE_Msk 0x4000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_SPARE4_Pos 27UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_SPARE4_Msk 0x8000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_SPARE5_Pos 28UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_MASKED_SPARE5_Msk 0x10000000UL
/* LVDSSS_LVDS_GPIF.GPIF_INTR_SET */
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_GPIF_DONE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_GPIF_DONE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_GPIF_FSM_INTR_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_GPIF_FSM_INTR_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_SWITCH_TIMEOUT_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_SWITCH_TIMEOUT_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_CRC_ERROR_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_CRC_ERROR_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_ADDR_COUNT_HIT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_ADDR_COUNT_HIT_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_DATA_COUNT_HIT_Pos 5UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_DATA_COUNT_HIT_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_CTRL_COUNT_HIT_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_CTRL_COUNT_HIT_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_ADDR_COMP_HIT_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_ADDR_COMP_HIT_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_DATA_COMP_HIT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_DATA_COMP_HIT_Msk 0x100UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_CTRL_COMP_HIT_Pos 9UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_CTRL_COMP_HIT_Msk 0x200UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_WAVEFORM_BUSY_Pos 10UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_WAVEFORM_BUSY_Msk 0x400UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_EG_DATA_EMPTY_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_EG_DATA_EMPTY_Msk 0xF0000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_IN_DATA_VALID_Pos 20UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_IN_DATA_VALID_Msk 0xF00000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_GPIF_ERR_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_GPIF_ERR_Msk 0x1000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_INVLD_CMD_DET_Pos 25UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_INVLD_CMD_DET_Msk 0x2000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_LINK_IDLE_Pos 26UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_LINK_IDLE_Msk 0x4000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_SPARE4_Pos 27UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_SPARE4_Msk 0x8000000UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_SPARE5_Pos 28UL
#define LVDSSS_LVDS_GPIF_GPIF_INTR_SET_SPARE5_Msk 0x10000000UL
/* LVDSSS_LVDS_GPIF.GPIF_ERROR */
#define LVDSSS_LVDS_GPIF_GPIF_ERROR_GPIF_ERR_CODE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ERROR_GPIF_ERR_CODE_Msk 0x1FUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_BUS_DIRECTION */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_INP_EN_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_INP_EN_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_OEN_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_OEN_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_BIDI_EN_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_BIDI_EN_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_OPEN_DRAIN_EN_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DIRECTION_OPEN_DRAIN_EN_Msk 0x8UL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_BUS_DEFAULT */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DEFAULT_DEFAULT_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_DEFAULT_DEFAULT_Msk 0xFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_BUS_POLARITY */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_POLARITY_POLARITY_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_POLARITY_POLARITY_Msk 0xFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_BUS_TOGGLE */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_TOGGLE_TOGGLE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_TOGGLE_TOGGLE_Msk 0xFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_BUS_SELECT */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_SELECT_OMEGA_INDEX_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_BUS_SELECT_OMEGA_INDEX_Msk 0x1FUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_COUNT_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_ENABLE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_ENABLE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_DOWN_UP_CNTR_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_DOWN_UP_CNTR_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_RELOAD_CNTR_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_RELOAD_CNTR_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_SW_RESET_CNTR_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_SW_RESET_CNTR_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_CONNECT_Pos 4UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_CONFIG_CONNECT_Msk 0xF0UL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_COUNT_RESET */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_RESET_RESET_LOAD_VAL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_RESET_RESET_LOAD_VAL_Msk 0xFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_COUNT_LIMIT */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_LIMIT_LIMIT_VAL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COUNT_LIMIT_LIMIT_VAL_Msk 0xFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_ADDR_COUNT_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_ENABLE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_ENABLE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_RELOAD_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_RELOAD_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_SW_RESET_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_SW_RESET_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_DOWN_UP_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_DOWN_UP_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_INCREMENT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_CONFIG_INCREMENT_Msk 0xFF00UL
/* LVDSSS_LVDS_GPIF.GPIF_ADDR_COUNT_RESET */
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_RESET_RESET_LOAD_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_RESET_RESET_LOAD_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_ADDR_COUNT_LIMIT */
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_LIMIT_LIMIT_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COUNT_LIMIT_LIMIT_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_STATE_COUNT_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_STATE_COUNT_CONFIG_ENABLE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_STATE_COUNT_CONFIG_ENABLE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_STATE_COUNT_CONFIG_SW_RESET_STATE_CNT_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_STATE_COUNT_CONFIG_SW_RESET_STATE_CNT_Msk 0x2UL
/* LVDSSS_LVDS_GPIF.GPIF_STATE_COUNT_LIMIT */
#define LVDSSS_LVDS_GPIF_GPIF_STATE_COUNT_LIMIT_LIMIT_VAL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_STATE_COUNT_LIMIT_LIMIT_VAL_Msk 0xFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COUNT_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_ENABLE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_ENABLE_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_RELOAD_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_RELOAD_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_SW_RESET_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_SW_RESET_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_DOWN_UP_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_DOWN_UP_Msk 0x8UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_INCREMENT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_CONFIG_INCREMENT_Msk 0xFF00UL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COUNT_RESET_LSB */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_RESET_LSB_RESET_LOAD_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_RESET_LSB_RESET_LOAD_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COUNT_RESET_MSB */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_RESET_MSB_RESET_LOAD_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_RESET_MSB_RESET_LOAD_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COUNT_LIMIT_LSB */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_LIMIT_LSB_LIMIT_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_LIMIT_LSB_LIMIT_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COUNT_LIMIT_MSB */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_LIMIT_MSB_LIMIT_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COUNT_LIMIT_MSB_LIMIT_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_COMP_VALUE */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COMP_VALUE_COMP_VALUE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COMP_VALUE_COMP_VALUE_Msk 0xFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CTRL_COMP_MASK */
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COMP_MASK_CTRL_COMP_MASK_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CTRL_COMP_MASK_CTRL_COMP_MASK_Msk 0xFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_VALUE_WORD0 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD0_VALUE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD0_VALUE_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_VALUE_WORD1 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD1_VALUE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD1_VALUE_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_VALUE_WORD2 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD2_VALUE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD2_VALUE_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_VALUE_WORD3 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD3_VALUE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_VALUE_WORD3_VALUE_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_MASK_WORD0 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD0_MASK_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD0_MASK_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_MASK_WORD1 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD1_MASK_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD1_MASK_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_MASK_WORD2 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD2_MASK_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD2_MASK_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_DATA_COMP_MASK_WORD3 */
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD3_MASK_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_DATA_COMP_MASK_WORD3_MASK_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_ADDR_COMP_VALUE */
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COMP_VALUE_VALUE_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COMP_VALUE_VALUE_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_ADDR_COMP_MASK */
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COMP_MASK_MASK_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ADDR_COMP_MASK_MASK_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_LAMBDA_STAT0 */
#define LVDSSS_LVDS_GPIF_GPIF_LAMBDA_STAT0_LAMBDA_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_LAMBDA_STAT0_LAMBDA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_LAMBDA_STAT1 */
#define LVDSSS_LVDS_GPIF_GPIF_LAMBDA_STAT1_LAMBDA_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_LAMBDA_STAT1_LAMBDA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_ALPHA_STAT */
#define LVDSSS_LVDS_GPIF_GPIF_ALPHA_STAT_ALPHA_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_ALPHA_STAT_ALPHA_Msk 0xFFUL
/* LVDSSS_LVDS_GPIF.GPIF_BETA_STAT */
#define LVDSSS_LVDS_GPIF_GPIF_BETA_STAT_BETA_VAL_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_BETA_STAT_BETA_VAL_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_WAVEFORM_CTRL_STAT */
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_WAVEFORM_VALID_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_WAVEFORM_VALID_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_PAUSE_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_PAUSE_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_GPIF_STAT_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_GPIF_STAT_Msk 0x700UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_CPU_LAMBDA_Pos 11UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_CPU_LAMBDA_Msk 0x800UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_ALPHA_INIT_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_ALPHA_INIT_Msk 0xFF0000UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_CURRENT_STATE_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_CTRL_STAT_CURRENT_STATE_Msk 0xFF000000UL
/* LVDSSS_LVDS_GPIF.GPIF_WAVEFORM_SWITCH */
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_WAVEFORM_SWITCH_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_WAVEFORM_SWITCH_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_DONE_ENABLE_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_DONE_ENABLE_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_SWITCH_NOW_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_SWITCH_NOW_Msk 0x4UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TIMEOUT_MODE_Pos 3UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TIMEOUT_MODE_Msk 0x38UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TIMEOUT_REACHED_Pos 6UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TIMEOUT_REACHED_Msk 0x40UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TERMINATED_Pos 7UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TERMINATED_Msk 0x80UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TERMINAL_STATE_Pos 8UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TERMINAL_STATE_Msk 0xFF00UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_DESTINATION_STATE_Pos 16UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_DESTINATION_STATE_Msk 0xFF0000UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_DONE_STATE_Pos 24UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_DONE_STATE_Msk 0xFF000000UL
/* LVDSSS_LVDS_GPIF.GPIF_WAVEFORM_SWITCH_TIMEOUT */
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TIMEOUT_RESET_LOAD_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_WAVEFORM_SWITCH_TIMEOUT_RESET_LOAD_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_CRC_CALC_CONFIG */
#define LVDSSS_LVDS_GPIF_GPIF_CRC_CALC_CONFIG_INIT_CRC_CTL_4MFSM_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_CRC_CALC_CONFIG_INIT_CRC_CTL_4MFSM_Msk 0x1UL
#define LVDSSS_LVDS_GPIF_GPIF_CRC_CALC_CONFIG_USE_CRC_CTL_4MFSM_Pos 1UL
#define LVDSSS_LVDS_GPIF_GPIF_CRC_CALC_CONFIG_USE_CRC_CTL_4MFSM_Msk 0x2UL
#define LVDSSS_LVDS_GPIF_GPIF_CRC_CALC_CONFIG_USE_CALC_CRC_4MFSM_Pos 2UL
#define LVDSSS_LVDS_GPIF_GPIF_CRC_CALC_CONFIG_USE_CALC_CRC_4MFSM_Msk 0x4UL
/* LVDSSS_LVDS_GPIF.GPIF_BETA_DEASSERT */
#define LVDSSS_LVDS_GPIF_GPIF_BETA_DEASSERT_APPLY_DEASSERT_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_BETA_DEASSERT_APPLY_DEASSERT_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_GPIF.GPIF_FUNCTION */
#define LVDSSS_LVDS_GPIF_GPIF_FUNCTION_FUNCTION_Pos 0UL
#define LVDSSS_LVDS_GPIF_GPIF_FUNCTION_FUNCTION_Msk 0xFFFFUL
/* LVDSSS_LVDS_GPIF.LINK_IDLE_CFG */
#define LVDSSS_LVDS_GPIF_LINK_IDLE_CFG_IDLE_CMD_CNT_Pos 0UL
#define LVDSSS_LVDS_GPIF_LINK_IDLE_CFG_IDLE_CMD_CNT_Msk 0xFFFFUL
/* LVDSSS_LVDS_GPIF.LVCMOS_CLK_OUT_CFG */
#define LVDSSS_LVDS_GPIF_LVCMOS_CLK_OUT_CFG_CLK_SRC_Pos 0UL
#define LVDSSS_LVDS_GPIF_LVCMOS_CLK_OUT_CFG_CLK_SRC_Msk 0x3UL
#define LVDSSS_LVDS_GPIF_LVCMOS_CLK_OUT_CFG_CLK_SRC_DIV_VAL_Pos 4UL
#define LVDSSS_LVDS_GPIF_LVCMOS_CLK_OUT_CFG_CLK_SRC_DIV_VAL_Msk 0x3F0UL


/* LVDSSS_LVDS_AFE.DLL_DFTLPF */
#define LVDSSS_LVDS_AFE_DLL_DFTLPF_EN_Pos       0UL
#define LVDSSS_LVDS_AFE_DLL_DFTLPF_EN_Msk       0x1UL
#define LVDSSS_LVDS_AFE_DLL_DFTLPF_FILT_SEL_Pos 1UL
#define LVDSSS_LVDS_AFE_DLL_DFTLPF_FILT_SEL_Msk 0x2UL
/* LVDSSS_LVDS_AFE.DLL_CONFIG */
#define LVDSSS_LVDS_AFE_DLL_CONFIG_DFT_RINGO_RST_Pos 0UL
#define LVDSSS_LVDS_AFE_DLL_CONFIG_DFT_RINGO_RST_Msk 0x1UL
/* LVDSSS_LVDS_AFE.DLL_STATUS */
#define LVDSSS_LVDS_AFE_DLL_STATUS_FT_Pos       0UL
#define LVDSSS_LVDS_AFE_DLL_STATUS_FT_Msk       0x3FUL
/* LVDSSS_LVDS_AFE.DLL_M_CONFIG */
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_EN_Pos     0UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_EN_Msk     0x1UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_CLK_SEL_Pos 1UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_CLK_SEL_Msk 0x2UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_INV_CLK_MUX_Pos 2UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_INV_CLK_MUX_Msk 0x4UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_EN_Pos 3UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_EN_Msk 0x8UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_TEST_CLK_EN_Pos 4UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_TEST_CLK_EN_Msk 0x10UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_BBPD_EN_Pos 5UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_BBPD_EN_Msk 0x20UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_BBPD_SEL_Pos 6UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_DFT_BBPD_SEL_Msk 0xC0UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_SPEED_MODE_Pos 8UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_SPEED_MODE_Msk 0x100UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_ADFT_EN_LV_Pos 9UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_ADFT_EN_LV_Msk 0x200UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_FLT_EN_Pos 10UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_FLT_EN_Msk 0x400UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_MDLL_SEL_PH_Pos 11UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_MDLL_SEL_PH_Msk 0x7800UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_MDLL_OW_PH_Pos 15UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_MDLL_OW_PH_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_HSPEED_MFREQ_SEL_Pos 16UL
#define LVDSSS_LVDS_AFE_DLL_M_CONFIG_HSPEED_MFREQ_SEL_Msk 0x10000UL
/* LVDSSS_LVDS_AFE.DLL_M_STATUS */
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_SEL_PH_Pos 0UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_SEL_PH_Msk 0xFUL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_CT_Pos     4UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_CT_Msk     0xF0UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_BBPD_Pos   8UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_BBPD_Msk   0x100UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_DFT_BBPD_Pos 9UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_DFT_BBPD_Msk 0x200UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_CORRECT_PERIOD_Pos 10UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_CORRECT_PERIOD_Msk 0x400UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_DLL_LOCK_Pos 11UL
#define LVDSSS_LVDS_AFE_DLL_M_STATUS_DLL_LOCK_Msk 0x800UL
/* LVDSSS_LVDS_AFE.DLL_S_CONFIG */
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_INV_CLK_MUXED_Pos 0UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_INV_CLK_MUXED_Msk 0x1UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_DFT_EN_Pos 1UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_DFT_EN_Msk 0x2UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_EN_Pos 2UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_EN_Msk 0x4UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_DFT_LPF_EN_Pos 3UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_DFT_LPF_EN_Msk 0x8UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_SEL_PH_Pos 4UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_SEL_PH_Msk 0xF0UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_OW_PH_Pos 8UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_OW_PH_Msk 0x100UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_SPEED_MODE_Pos 9UL
#define LVDSSS_LVDS_AFE_DLL_S_CONFIG_SDLL_SPEED_MODE_Msk 0x200UL
/* LVDSSS_LVDS_AFE.DLL_S_STATUS */
#define LVDSSS_LVDS_AFE_DLL_S_STATUS_SEL_PH_Pos 0UL
#define LVDSSS_LVDS_AFE_DLL_S_STATUS_SEL_PH_Msk 0xFUL
#define LVDSSS_LVDS_AFE_DLL_S_STATUS_CLK_DFT_Pos 4UL
#define LVDSSS_LVDS_AFE_DLL_S_STATUS_CLK_DFT_Msk 0x10UL
/* LVDSSS_LVDS_AFE.GENERAL_LICIO_CIO */
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_CIO_LVDS_RTERM_EN_Pos 0UL
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_CIO_LVDS_RTERM_EN_Msk 0x3FFUL
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_CIO_LVCMOS_LB_EN_Pos 10UL
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_CIO_LVCMOS_LB_EN_Msk 0x400UL
/* LVDSSS_LVDS_AFE.LICIO_CIO */
#define LVDSSS_LVDS_AFE_LICIO_CIO_LVCMOS_RX_EN_Pos 0UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_LVCMOS_RX_EN_Msk 0x1UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_LVCMOS_TX_EN_Pos 1UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_LVCMOS_TX_EN_Msk 0x2UL
/* LVDSSS_LVDS_AFE.GENERAL_LICIO_LI */
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_LI_LVDS_LB_EN_Pos 0UL
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_LI_LVDS_LB_EN_Msk 0x1UL
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_LI_LVDS_CONFIG_Pos 1UL
#define LVDSSS_LVDS_AFE_GENERAL_LICIO_LI_LVDS_CONFIG_Msk 0x6UL
/* LVDSSS_LVDS_AFE.LICIO_LI */
#define LVDSSS_LVDS_AFE_LICIO_LI_LVDS_RX_EN_Pos 0UL
#define LVDSSS_LVDS_AFE_LICIO_LI_LVDS_RX_EN_Msk 0x1UL
#define LVDSSS_LVDS_AFE_LICIO_LI_ATST_Pos       7UL
#define LVDSSS_LVDS_AFE_LICIO_LI_ATST_Msk       0x380UL
/* LVDSSS_LVDS_AFE.LICIO_VSSIO_IREF */
#define LVDSSS_LVDS_AFE_LICIO_VSSIO_IREF_EN_Pos 0UL
#define LVDSSS_LVDS_AFE_LICIO_VSSIO_IREF_EN_Msk 0x1UL
#define LVDSSS_LVDS_AFE_LICIO_VSSIO_IREF_IN_SEL_Pos 1UL
#define LVDSSS_LVDS_AFE_LICIO_VSSIO_IREF_IN_SEL_Msk 0x6UL
#define LVDSSS_LVDS_AFE_LICIO_VSSIO_IREF_ATST_EN_Pos 3UL
#define LVDSSS_LVDS_AFE_LICIO_VSSIO_IREF_ATST_EN_Msk 0x8UL
/* LVDSSS_LVDS_AFE.LICIO_LI_OUT_STATUS */
#define LVDSSS_LVDS_AFE_LICIO_LI_OUT_STATUS_LVDS_DATA_Pos 0UL
#define LVDSSS_LVDS_AFE_LICIO_LI_OUT_STATUS_LVDS_DATA_Msk 0xFFUL
#define LVDSSS_LVDS_AFE_LICIO_LI_OUT_STATUS_LVDS_CTRL_Pos 8UL
#define LVDSSS_LVDS_AFE_LICIO_LI_OUT_STATUS_LVDS_CTRL_Msk 0x100UL
#define LVDSSS_LVDS_AFE_LICIO_LI_OUT_STATUS_LVDS_CLK_Pos 9UL
#define LVDSSS_LVDS_AFE_LICIO_LI_OUT_STATUS_LVDS_CLK_Msk 0x200UL
/* LVDSSS_LVDS_AFE.PLL_CONFIG */
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_CP_CUR_TRIM_Pos 0UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_CP_CUR_TRIM_Msk 0x3UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_BYPASS_Pos 2UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_BYPASS_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_PFD_DELAY_Pos 3UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_PFD_DELAY_Msk 0x18UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_LD_DELAY_Pos 5UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_LD_DELAY_Msk 0x60UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_EN_Pos   7UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_EN_Msk   0x80UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_REG_VCO_BYPASS_Pos 8UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_REG_VCO_BYPASS_Msk 0x100UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_REF_SEL_Pos 9UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_REF_SEL_Msk 0x600UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_SUPPLY_EN_Pos 11UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_SUPPLY_EN_Msk 0x800UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_IN_DIV_Pos 12UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_IN_DIV_Msk 0x3000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_SERIAL_DIV_Pos 15UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_SERIAL_DIV_Msk 0x18000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_FRAME_DIV_Pos 17UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_FRAME_DIV_Msk 0x20000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_FB_DIV_Pos 18UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_N_FB_DIV_Msk 0x3C0000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_ATST_SEL_Pos 22UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_ATST_SEL_Msk 0x3C00000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_RUN_AWAY_DIS_Pos 26UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_RUN_AWAY_DIS_Msk 0x4000000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_RUN_AWAY_TRIM_Pos 27UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_RUN_AWAY_TRIM_Msk 0x18000000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_RUN_AWAY_DEL_Pos 29UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_PLL_RUN_AWAY_DEL_Msk 0x60000000UL
/* LVDSSS_LVDS_AFE.PLL_CONFIG_2 */
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_REG_CORE_TRIM_Pos 0UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_REG_CORE_TRIM_Msk 0x7UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CAL_UP_DN_Pos 3UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CAL_UP_DN_Msk 0x78UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_RA_UP_TR_Pos 7UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_RA_UP_TR_Msk 0x180UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_VCO_GAIN_Pos 9UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_VCO_GAIN_Msk 0x1E00UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_FB_SEL_Pos 13UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_FB_SEL_Msk 0x6000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_MD_Pos 15UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_MD_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_LOCK_DELAY_Pos 16UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_LOCK_DELAY_Msk 0xFF0000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_LOCK_LOSS_DELAY_Pos 24UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_LOCK_LOSS_DELAY_Msk 0x1F000000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_EN_OVRDE_Pos 29UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_EN_OVRDE_Msk 0x20000000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_OVRDE_LOCK_VAL_Pos 30UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_OVRDE_LOCK_VAL_Msk 0x40000000UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_SOURCE_LOCK_Pos 31UL
#define LVDSSS_LVDS_AFE_PLL_CONFIG_2_PLL_CTRL_SOURCE_LOCK_Msk 0x80000000UL
/* LVDSSS_LVDS_AFE.PLL_STATUS */
#define LVDSSS_LVDS_AFE_PLL_STATUS_PLL_DFT_Pos  0UL
#define LVDSSS_LVDS_AFE_PLL_STATUS_PLL_DFT_Msk  0x1UL
#define LVDSSS_LVDS_AFE_PLL_STATUS_PLL_LOCK_Pos 1UL
#define LVDSSS_LVDS_AFE_PLL_STATUS_PLL_LOCK_Msk 0x2UL
#define LVDSSS_LVDS_AFE_PLL_STATUS_PLL_RUN_AWAY_STICKY_Pos 2UL
#define LVDSSS_LVDS_AFE_PLL_STATUS_PLL_RUN_AWAY_STICKY_Msk 0x4UL
/* LVDSSS_LVDS_AFE.REG_1P25 */
#define LVDSSS_LVDS_AFE_REG_1P25_IREF_DLL_SEL_Pos 0UL
#define LVDSSS_LVDS_AFE_REG_1P25_IREF_DLL_SEL_Msk 0x7UL
#define LVDSSS_LVDS_AFE_REG_1P25_ENABLE_Pos     3UL
#define LVDSSS_LVDS_AFE_REG_1P25_ENABLE_Msk     0x8UL
#define LVDSSS_LVDS_AFE_REG_1P25_USE_REG_Pos    4UL
#define LVDSSS_LVDS_AFE_REG_1P25_USE_REG_Msk    0x10UL
#define LVDSSS_LVDS_AFE_REG_1P25_ADFT_CTRL_Pos  5UL
#define LVDSSS_LVDS_AFE_REG_1P25_ADFT_CTRL_Msk  0x1E0UL
#define LVDSSS_LVDS_AFE_REG_1P25_ADFT_EN_Pos    9UL
#define LVDSSS_LVDS_AFE_REG_1P25_ADFT_EN_Msk    0x200UL
#define LVDSSS_LVDS_AFE_REG_1P25_BURN_IN_EN_Pos 10UL
#define LVDSSS_LVDS_AFE_REG_1P25_BURN_IN_EN_Msk 0x400UL
#define LVDSSS_LVDS_AFE_REG_1P25_TRIM_VREG_1P25_Pos 11UL
#define LVDSSS_LVDS_AFE_REG_1P25_TRIM_VREG_1P25_Msk 0x7800UL
/* LVDSSS_LVDS_AFE.RX */
#define LVDSSS_LVDS_AFE_RX_FIXTIME_FRAMECLK_Pos 0UL
#define LVDSSS_LVDS_AFE_RX_FIXTIME_FRAMECLK_Msk 0x1UL
#define LVDSSS_LVDS_AFE_RX_EN_DESER_Pos         1UL
#define LVDSSS_LVDS_AFE_RX_EN_DESER_Msk         0x2UL
#define LVDSSS_LVDS_AFE_RX_INV_CLK_FRAME_Pos    2UL
#define LVDSSS_LVDS_AFE_RX_INV_CLK_FRAME_Msk    0x4UL
#define LVDSSS_LVDS_AFE_RX_INV_CLK_SER_Pos      3UL
#define LVDSSS_LVDS_AFE_RX_INV_CLK_SER_Msk      0x8UL
#define LVDSSS_LVDS_AFE_RX_INV_DATA_SER_Pos     4UL
#define LVDSSS_LVDS_AFE_RX_INV_DATA_SER_Msk     0x10UL
#define LVDSSS_LVDS_AFE_RX_CLK_MUX_SEL_Pos      5UL
#define LVDSSS_LVDS_AFE_RX_CLK_MUX_SEL_Msk      0x60UL
/* LVDSSS_LVDS_AFE.GENERAL_RX */
#define LVDSSS_LVDS_AFE_GENERAL_RX_DR_MODE_Pos  0UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_DR_MODE_Msk  0x1UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_BER_RSTN_Pos 1UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_BER_RSTN_Msk 0x2UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_DLL_BYPASS_EN_Pos 2UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_DLL_BYPASS_EN_Msk 0x4UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LSB_FIRST_MODE_Pos 3UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LSB_FIRST_MODE_Msk 0x8UL
/* LVDSSS_LVDS_AFE.GENERAL_RX_LVCMOS */
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_CLK_MODE_Pos 1UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_CLK_MODE_Msk 0x2UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_EN_DDR_Pos 2UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_EN_DDR_Msk 0x4UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_CLK_BYPASS_Pos 3UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_CLK_BYPASS_Msk 0x8UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_MUX_SEL_Pos 4UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_MUX_SEL_Msk 0x10UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_PLL_CLK_EN_Pos 5UL
#define LVDSSS_LVDS_AFE_GENERAL_RX_LVCMOS_PLL_CLK_EN_Msk 0x20UL
/* LVDSSS_LVDS_AFE.RX_LVCMOS */
#define LVDSSS_LVDS_AFE_RX_LVCMOS_EN_LVCMOS_Pos 0UL
#define LVDSSS_LVDS_AFE_RX_LVCMOS_EN_LVCMOS_Msk 0x1UL
/* LVDSSS_LVDS_AFE.PRBS_GEN */
#define LVDSSS_LVDS_AFE_PRBS_GEN_EN_Pos         0UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_EN_Msk         0x1UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_SEED_Pos       1UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_SEED_Msk       0x3EUL
#define LVDSSS_LVDS_AFE_PRBS_GEN_DDR_EN_Pos     6UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_DDR_EN_Msk     0x40UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_MULT_ERR_EN_Pos 7UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_MULT_ERR_EN_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_CLK_SRC_Pos    8UL
#define LVDSSS_LVDS_AFE_PRBS_GEN_CLK_SRC_Msk    0x100UL
/* LVDSSS_LVDS_AFE.PHY_GENERAL_CONFIG */
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_DESKEW_ALGORITHM_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_DESKEW_ALGORITHM_Msk 0x3UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_SCANON_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_SCANON_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_V1P25_VCCD_Pos 3UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_V1P25_VCCD_Msk 0x8UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_V1P1_VCCD_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_V1P1_VCCD_Msk 0x10UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_VDDIO_Pos 5UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_ENABLE_VDDIO_Msk 0x20UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_LVCMOS_TX_CLKSEL_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_LVCMOS_TX_CLKSEL_Msk 0xC0UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_BERCLK_SKEW_MARGIN_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_BERCLK_SKEW_MARGIN_Msk 0x300UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_MONITOR_FREQ_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_MONITOR_FREQ_Msk 0x1C00UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_MONITOR_TIME_Pos 13UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_MONITOR_TIME_Msk 0xE000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_MONITOR_NON_STOP_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_MONITOR_NON_STOP_Msk 0x10000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_DESKEW_ERR_THRESH1_Pos 17UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_DESKEW_ERR_THRESH1_Msk 0xFE0000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_DESKEW_ERR_THRESH2_Pos 24UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_DESKEW_ERR_THRESH2_Msk 0x7F000000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_BIST_RLXD_MARGIN_SEL_Pos 31UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_BIST_RLXD_MARGIN_SEL_Msk 0x80000000UL
/* LVDSSS_LVDS_AFE.PHY_GENERAL_STATUS_1 */
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D0_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D0_Msk 0x3UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D1_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D1_Msk 0xCUL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D2_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D2_Msk 0x30UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D3_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D3_Msk 0xC0UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D4_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D4_Msk 0x300UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D5_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D5_Msk 0xC00UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D6_Pos 12UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D6_Msk 0x3000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D7_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_D7_Msk 0xC000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_CTRL_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_CHECKER_STATE_CTRL_Msk 0x30000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_RUN_STATUS_Pos 18UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_1_PRBS_RUN_STATUS_Msk 0x40000UL
/* LVDSSS_LVDS_AFE.PHY_GENERAL_STATUS_2 */
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_LOOPBACK_STOPPED_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_LOOPBACK_STOPPED_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MONITOR_RUNNING_Pos 1UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MONITOR_RUNNING_Msk 0x2UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MONITOR_COMPLETED_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MONITOR_COMPLETED_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_DESKEW_RUNNING_Pos 3UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_DESKEW_RUNNING_Msk 0x8UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_DESKEW_COMPLETED_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_DESKEW_COMPLETED_Msk 0x10UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MONITOR_ADJUST_FAIL_Pos 5UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MONITOR_ADJUST_FAIL_Msk 0x20UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_OK_DLL_VCCD_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_OK_DLL_VCCD_Msk 0x40UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_OK_V1P1_VCCD_Pos 7UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_OK_V1P1_VCCD_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_OK_V1P25_VCCD_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_OK_V1P25_VCCD_Msk 0x100UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_PLL_LOCK_ACQUIRED_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_PLL_LOCK_ACQUIRED_Msk 0x400UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_PLL_LOCK_LOST_Pos 11UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_PLL_LOCK_LOST_Msk 0x800UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MDLL_LOCK_ACQUIRED_Pos 12UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MDLL_LOCK_ACQUIRED_Msk 0x1000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MDLL_LOCK_LOST_Pos 13UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_MDLL_LOCK_LOST_Msk 0x2000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_BIST_MDLL_DONE_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_BIST_MDLL_DONE_Msk 0x4000UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_BIST_SDLL_DONE_Pos 15UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_STATUS_2_BIST_SDLL_DONE_Msk 0x8000UL
/* LVDSSS_LVDS_AFE.PHY_INTR */
#define LVDSSS_LVDS_AFE_PHY_INTR_LOOPBACK_STOPPED_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_INTR_LOOPBACK_STOPPED_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MONITOR_RUNNING_Pos 1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MONITOR_RUNNING_Msk 0x2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MONITOR_COMPLETED_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MONITOR_COMPLETED_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_DESKEW_RUNNING_Pos 3UL
#define LVDSSS_LVDS_AFE_PHY_INTR_DESKEW_RUNNING_Msk 0x8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_DESKEW_COMPLETED_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_DESKEW_COMPLETED_Msk 0x10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MONITOR_ADJUST_FAIL_Pos 5UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MONITOR_ADJUST_FAIL_Msk 0x20UL
#define LVDSSS_LVDS_AFE_PHY_INTR_OK_DLL_VCCD_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_INTR_OK_DLL_VCCD_Msk 0x40UL
#define LVDSSS_LVDS_AFE_PHY_INTR_OK_V1P1_VCCD_Pos 7UL
#define LVDSSS_LVDS_AFE_PHY_INTR_OK_V1P1_VCCD_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PHY_INTR_OK_V1P25_VCCD_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_OK_V1P25_VCCD_Msk 0x100UL
#define LVDSSS_LVDS_AFE_PHY_INTR_GPIO_INTR_Pos  9UL
#define LVDSSS_LVDS_AFE_PHY_INTR_GPIO_INTR_Msk  0x200UL
#define LVDSSS_LVDS_AFE_PHY_INTR_PLL_LOCK_ACQUIRED_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_PLL_LOCK_ACQUIRED_Msk 0x400UL
#define LVDSSS_LVDS_AFE_PHY_INTR_PLL_LOCK_LOST_Pos 11UL
#define LVDSSS_LVDS_AFE_PHY_INTR_PLL_LOCK_LOST_Msk 0x800UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MDLL_LOCK_ACQUIRED_Pos 12UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MDLL_LOCK_ACQUIRED_Msk 0x1000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MDLL_LOCK_LOST_Pos 13UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MDLL_LOCK_LOST_Msk 0x2000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_BIST_MDLL_DONE_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_INTR_BIST_MDLL_DONE_Msk 0x4000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_BIST_SDLL_DONE_Pos 15UL
#define LVDSSS_LVDS_AFE_PHY_INTR_BIST_SDLL_DONE_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SAR_DONE_Pos   16UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SAR_DONE_Msk   0x10000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_ADC_CMP_OUT_POS_EDGE_Pos 17UL
#define LVDSSS_LVDS_AFE_PHY_INTR_ADC_CMP_OUT_POS_EDGE_Msk 0x20000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_ADC_CMP_OUT_FALL_EDGE_Pos 18UL
#define LVDSSS_LVDS_AFE_PHY_INTR_ADC_CMP_OUT_FALL_EDGE_Msk 0x40000UL
/* LVDSSS_LVDS_AFE.PHY_INTR_MASK */
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_LOOPBACK_STOPPED_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_LOOPBACK_STOPPED_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MONITOR_RUNNING_Pos 1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MONITOR_RUNNING_Msk 0x2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MONITOR_COMPLETED_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MONITOR_COMPLETED_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_DESKEW_RUNNING_Pos 3UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_DESKEW_RUNNING_Msk 0x8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_DESKEW_COMPLETED_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_DESKEW_COMPLETED_Msk 0x10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MONITOR_ADJUST_FAIL_Pos 5UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MONITOR_ADJUST_FAIL_Msk 0x20UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_OK_DLL_VCCD_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_OK_DLL_VCCD_Msk 0x40UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_OK_V1P1_VCCD_Pos 7UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_OK_V1P1_VCCD_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_OK_V1P25_VCCD_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_OK_V1P25_VCCD_Msk 0x100UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_GPIO_INTR_Pos 9UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_GPIO_INTR_Msk 0x200UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_PLL_LOCK_ACQUIRED_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_PLL_LOCK_ACQUIRED_Msk 0x400UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_PLL_LOCK_LOST_Pos 11UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_PLL_LOCK_LOST_Msk 0x800UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MDLL_LOCK_ACQUIRED_Pos 12UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MDLL_LOCK_ACQUIRED_Msk 0x1000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MDLL_LOCK_LOST_Pos 13UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_MDLL_LOCK_LOST_Msk 0x2000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_BIST_MDLL_DONE_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_BIST_MDLL_DONE_Msk 0x4000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_BIST_SDLL_DONE_Pos 15UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_BIST_SDLL_DONE_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_SAR_DONE_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_SAR_DONE_Msk 0x10000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_ADC_CMP_OUT_POS_EDGE_Pos 17UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_ADC_CMP_OUT_POS_EDGE_Msk 0x20000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_ADC_CMP_OUT_FALL_EDGE_Pos 18UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASK_ADC_CMP_OUT_FALL_EDGE_Msk 0x40000UL
/* LVDSSS_LVDS_AFE.PHY_INTR_MASKED */
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_LOOPBACK_STOPPED_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_LOOPBACK_STOPPED_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MONITOR_RUNNING_Pos 1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MONITOR_RUNNING_Msk 0x2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MONITOR_COMPLETED_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MONITOR_COMPLETED_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_DESKEW_RUNNING_Pos 3UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_DESKEW_RUNNING_Msk 0x8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_DESKEW_COMPLETED_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_DESKEW_COMPLETED_Msk 0x10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MONITOR_ADJUST_FAIL_Pos 5UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MONITOR_ADJUST_FAIL_Msk 0x20UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_OK_DLL_VCCD_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_OK_DLL_VCCD_Msk 0x40UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_OK_V1P1_VCCD_Pos 7UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_OK_V1P1_VCCD_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_OK_V1P25_VCCD_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_OK_V1P25_VCCD_Msk 0x100UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_GPIO_INTR_Pos 9UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_GPIO_INTR_Msk 0x200UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_PLL_LOCK_ACQUIRED_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_PLL_LOCK_ACQUIRED_Msk 0x400UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_PLL_LOCK_LOST_Pos 11UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_PLL_LOCK_LOST_Msk 0x800UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MDLL_LOCK_ACQUIRED_Pos 12UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MDLL_LOCK_ACQUIRED_Msk 0x1000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MDLL_LOCK_LOST_Pos 13UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_MDLL_LOCK_LOST_Msk 0x2000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_BIST_MDLL_DONE_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_BIST_MDLL_DONE_Msk 0x4000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_BIST_SDLL_DONE_Pos 15UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_BIST_SDLL_DONE_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_SAR_DONE_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_SAR_DONE_Msk 0x10000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_ADC_CMP_OUT_POS_EDGE_Pos 17UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_ADC_CMP_OUT_POS_EDGE_Msk 0x20000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_ADC_CMP_OUT_FALL_EDGE_Pos 18UL
#define LVDSSS_LVDS_AFE_PHY_INTR_MASKED_ADC_CMP_OUT_FALL_EDGE_Msk 0x40000UL
/* LVDSSS_LVDS_AFE.PHY_INTR_SET */
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_LOOPBACK_STOPPED_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_LOOPBACK_STOPPED_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MONITOR_RUNNING_Pos 1UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MONITOR_RUNNING_Msk 0x2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MONITOR_COMPLETED_Pos 2UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MONITOR_COMPLETED_Msk 0x4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_DESKEW_RUNNING_Pos 3UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_DESKEW_RUNNING_Msk 0x8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_DESKEW_COMPLETED_Pos 4UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_DESKEW_COMPLETED_Msk 0x10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MONITOR_ADJUST_FAIL_Pos 5UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MONITOR_ADJUST_FAIL_Msk 0x20UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_OK_DLL_VCCD_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_OK_DLL_VCCD_Msk 0x40UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_OK_V1P1_VCCD_Pos 7UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_OK_V1P1_VCCD_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_OK_V1P25_VCCD_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_OK_V1P25_VCCD_Msk 0x100UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_GPIO_INTR_Pos 9UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_GPIO_INTR_Msk 0x200UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_PLL_LOCK_ACQUIRED_Pos 10UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_PLL_LOCK_ACQUIRED_Msk 0x400UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_PLL_LOCK_LOST_Pos 11UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_PLL_LOCK_LOST_Msk 0x800UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MDLL_LOCK_ACQUIRED_Pos 12UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MDLL_LOCK_ACQUIRED_Msk 0x1000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MDLL_LOCK_LOST_Pos 13UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_MDLL_LOCK_LOST_Msk 0x2000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_BIST_MDLL_DONE_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_BIST_MDLL_DONE_Msk 0x4000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_BIST_SDLL_DONE_Pos 15UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_BIST_SDLL_DONE_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_SAR_DONE_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_SAR_DONE_Msk 0x10000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_ADC_CMP_OUT_POS_EDGE_Pos 17UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_ADC_CMP_OUT_POS_EDGE_Msk 0x20000UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_ADC_CMP_OUT_FALL_EDGE_Pos 18UL
#define LVDSSS_LVDS_AFE_PHY_INTR_SET_ADC_CMP_OUT_FALL_EDGE_Msk 0x40000UL
/* LVDSSS_LVDS_AFE.PHY_TRAIN_CONFIG */
#define LVDSSS_LVDS_AFE_PHY_TRAIN_CONFIG_TRAIN_EN_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_TRAIN_CONFIG_TRAIN_EN_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_TRAIN_CONFIG_TRAIN_SEQ_Pos 1UL
#define LVDSSS_LVDS_AFE_PHY_TRAIN_CONFIG_TRAIN_SEQ_Msk 0x1FEUL
#define LVDSSS_LVDS_AFE_PHY_TRAIN_CONFIG_TRAIN_CYCLES_NO_Pos 9UL
#define LVDSSS_LVDS_AFE_PHY_TRAIN_CONFIG_TRAIN_CYCLES_NO_Msk 0xFE00UL
/* LVDSSS_LVDS_AFE.PHY_ADC_CONFIG */
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_DAC_CNTRL_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_DAC_CNTRL_Msk 0xFFUL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_MID_VAL_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_MID_VAL_Msk 0xFF00UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_LDO_TRIM_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_LDO_TRIM_Msk 0xF0000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_LDO_EN_Pos 20UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_LDO_EN_Msk 0x100000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_ISO_N_Pos 21UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_ISO_N_Msk 0x200000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_PD_LV_Pos 22UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_PD_LV_Msk 0x400000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_DFT_MUXSEL_Pos 23UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_DFT_MUXSEL_Msk 0x800000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_VSEL_Pos 24UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_VSEL_Msk 0x7000000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_VREF_DAC_SEL_Pos 27UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_VREF_DAC_SEL_Msk 0x18000000UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_SAR_EN_Pos 29UL
#define LVDSSS_LVDS_AFE_PHY_ADC_CONFIG_SAR_EN_Msk 0x20000000UL
/* LVDSSS_LVDS_AFE.PHY_DDFT_MUX_SEL */
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT0_MUX_SEL_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT0_MUX_SEL_Msk 0x3FUL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT0_LVCMOSOUT_SEL_Pos 6UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT0_LVCMOSOUT_SEL_Msk 0x40UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT0_LINK_SEL_Pos 7UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT0_LINK_SEL_Msk 0x80UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT1_MUX_SEL_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT1_MUX_SEL_Msk 0x3F00UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT1_LVCMOSOUT_SEL_Pos 14UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT1_LVCMOSOUT_SEL_Msk 0x4000UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT1_LINK_SEL_Pos 15UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_DDFT1_LINK_SEL_Msk 0x8000UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO0_MUX_SEL_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO0_MUX_SEL_Msk 0x3F0000UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO0_LINK_SEL_Pos 23UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO0_LINK_SEL_Msk 0x800000UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO1_MUX_SEL_Pos 24UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO1_MUX_SEL_Msk 0x3F000000UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO1_LINK_SEL_Pos 31UL
#define LVDSSS_LVDS_AFE_PHY_DDFT_MUX_SEL_GPIO1_LINK_SEL_Msk 0x80000000UL
/* LVDSSS_LVDS_AFE.LICIO_VCCD_V1P1 */
#define LVDSSS_LVDS_AFE_LICIO_VCCD_V1P1_EN_Pos  0UL
#define LVDSSS_LVDS_AFE_LICIO_VCCD_V1P1_EN_Msk  0x1UL
#define LVDSSS_LVDS_AFE_LICIO_VCCD_V1P1_ATST_EN_Pos 3UL
#define LVDSSS_LVDS_AFE_LICIO_VCCD_V1P1_ATST_EN_Msk 0x8UL
/* LVDSSS_LVDS_AFE.PHY_GPIO */
#define LVDSSS_LVDS_AFE_PHY_GPIO_OUT_VALUE_Pos  0UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_OUT_VALUE_Msk  0x1UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_IN_VALUE_Pos   1UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_IN_VALUE_Msk   0x2UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_OUTPUT_EN_Pos  2UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_OUTPUT_EN_Msk  0x4UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_INPUT_EN_Pos   3UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_INPUT_EN_Msk   0x8UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_GPIO_ENABLE_Pos 31UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_GPIO_ENABLE_Msk 0x80000000UL
/* LVDSSS_LVDS_AFE.PHY_GPIO_INTR_CFG */
#define LVDSSS_LVDS_AFE_PHY_GPIO_INTR_CFG_GPIO_INTR_PIN_SEL_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_INTR_CFG_GPIO_INTR_PIN_SEL_Msk 0x1FUL
#define LVDSSS_LVDS_AFE_PHY_GPIO_INTR_CFG_INTRMODE_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_GPIO_INTR_CFG_INTRMODE_Msk 0x700UL
/* LVDSSS_LVDS_AFE.PHY_GENERAL_CONFIG_2 */
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_2_CNT_10US24_VAL_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_2_CNT_10US24_VAL_Msk 0xFFFFUL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_2_MONITOR_ERR_THRESH_Pos 16UL
#define LVDSSS_LVDS_AFE_PHY_GENERAL_CONFIG_2_MONITOR_ERR_THRESH_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_AFE.PRBS_STATUS */
#define LVDSSS_LVDS_AFE_PRBS_STATUS_BER_COUNTER_Pos 0UL
#define LVDSSS_LVDS_AFE_PRBS_STATUS_BER_COUNTER_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_AFE.LVDS_STATUS_MONITOR */
#define LVDSSS_LVDS_AFE_LVDS_STATUS_MONITOR_LVDS_PAD_VALUE_Pos 0UL
#define LVDSSS_LVDS_AFE_LVDS_STATUS_MONITOR_LVDS_PAD_VALUE_Msk 0x1UL
#define LVDSSS_LVDS_AFE_LVDS_STATUS_MONITOR_LVDS_PAD_RDEN_Pos 1UL
#define LVDSSS_LVDS_AFE_LVDS_STATUS_MONITOR_LVDS_PAD_RDEN_Msk 0x2UL
/* LVDSSS_LVDS_AFE.PHY_ADC_STATUS */
#define LVDSSS_LVDS_AFE_PHY_ADC_STATUS_CMP_OUT_Pos 0UL
#define LVDSSS_LVDS_AFE_PHY_ADC_STATUS_CMP_OUT_Msk 0x1UL
#define LVDSSS_LVDS_AFE_PHY_ADC_STATUS_SAR_OUT_Pos 8UL
#define LVDSSS_LVDS_AFE_PHY_ADC_STATUS_SAR_OUT_Msk 0xFF00UL
/* LVDSSS_LVDS_AFE.FRM_CLK_GLITCH_FILTER */
#define LVDSSS_LVDS_AFE_FRM_CLK_GLITCH_FILTER_CLKGATE_DURATION_Pos 0UL
#define LVDSSS_LVDS_AFE_FRM_CLK_GLITCH_FILTER_CLKGATE_DURATION_Msk 0xFFUL
#define LVDSSS_LVDS_AFE_FRM_CLK_GLITCH_FILTER_PHASE_CHNG_EFFECT_TIME_Pos 8UL
#define LVDSSS_LVDS_AFE_FRM_CLK_GLITCH_FILTER_PHASE_CHNG_EFFECT_TIME_Msk 0xFF00UL
#define LVDSSS_LVDS_AFE_FRM_CLK_GLITCH_FILTER_BYPASS_GLITCH_FILTER_Pos 16UL
#define LVDSSS_LVDS_AFE_FRM_CLK_GLITCH_FILTER_BYPASS_GLITCH_FILTER_Msk 0x10000UL
/* LVDSSS_LVDS_AFE.LICIO_CIO_CTRL_DS */
#define LVDSSS_LVDS_AFE_LICIO_CIO_CTRL_DS_DS_Pos 0UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_CTRL_DS_DS_Msk 0x1FUL
#define LVDSSS_LVDS_AFE_LICIO_CIO_CTRL_DS_SR_Pos 8UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_CTRL_DS_SR_Msk 0x300UL
/* LVDSSS_LVDS_AFE.LICIO_CIO_DATA_DS */
#define LVDSSS_LVDS_AFE_LICIO_CIO_DATA_DS_DS_Pos 0UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_DATA_DS_DS_Msk 0x1FUL
#define LVDSSS_LVDS_AFE_LICIO_CIO_DATA_DS_SR_Pos 8UL
#define LVDSSS_LVDS_AFE_LICIO_CIO_DATA_DS_SR_Msk 0x300UL
/* LVDSSS_LVDS_AFE.DLY_CELL_LOAD */
#define LVDSSS_LVDS_AFE_DLY_CELL_LOAD_LOAD_Pos  0UL
#define LVDSSS_LVDS_AFE_DLY_CELL_LOAD_LOAD_Msk  0x3UL
/* LVDSSS_LVDS_AFE.SAR_CLK_FREQ_CFG */
#define LVDSSS_LVDS_AFE_SAR_CLK_FREQ_CFG_CLK_DIV_Pos 0UL
#define LVDSSS_LVDS_AFE_SAR_CLK_FREQ_CFG_CLK_DIV_Msk 0x3UL
/* LVDSSS_LVDS_AFE.STUCK_AT_CNTRS */
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR0_Pos 0UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR0_Msk 0xFUL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR1_Pos 4UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR1_Msk 0xF0UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR2_Pos 8UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR2_Msk 0xF00UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR3_Pos 12UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR3_Msk 0xF000UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR4_Pos 16UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR4_Msk 0xF0000UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR5_Pos 20UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR5_Msk 0xF00000UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR6_Pos 24UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR6_Msk 0xF000000UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR7_Pos 28UL
#define LVDSSS_LVDS_AFE_STUCK_AT_CNTRS_CNTR7_Msk 0xF0000000UL
/* LVDSSS_LVDS_AFE.STUCK_CNTR_RST_CTL */
#define LVDSSS_LVDS_AFE_STUCK_CNTR_RST_CTL_CNTR_RST_Pos 0UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_RST_CTL_CNTR_RST_Msk 0xFFUL
/* LVDSSS_LVDS_AFE.STUCK_CNTR_SIG_SEL_0 */
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_0_Pos 0UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_0_Msk 0xFFUL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_1_Pos 8UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_1_Msk 0xFF00UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_2_Pos 16UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_2_Msk 0xFF0000UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_3_Pos 24UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_0_SIG_SEL_OF_CNTR_3_Msk 0xFF000000UL
/* LVDSSS_LVDS_AFE.STUCK_CNTR_SIG_SEL_1 */
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_4_Pos 0UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_4_Msk 0xFFUL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_5_Pos 8UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_5_Msk 0xFF00UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_6_Pos 16UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_6_Msk 0xFF0000UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_7_Pos 24UL
#define LVDSSS_LVDS_AFE_STUCK_CNTR_SIG_SEL_1_SIG_SEL_OF_CNTR_7_Msk 0xFF000000UL
/* LVDSSS_LVDS_AFE.CTL_PIN_SDLL_CFG */
#define LVDSSS_LVDS_AFE_CTL_PIN_SDLL_CFG_CTL_PIN_SEL_Pos 0UL
#define LVDSSS_LVDS_AFE_CTL_PIN_SDLL_CFG_CTL_PIN_SEL_Msk 0xFUL
#define LVDSSS_LVDS_AFE_CTL_PIN_SDLL_CFG_EN_SDLL_CLK_MON_Pos 4UL
#define LVDSSS_LVDS_AFE_CTL_PIN_SDLL_CFG_EN_SDLL_CLK_MON_Msk 0x10UL
/* LVDSSS_LVDS_AFE.LVDS_BER_STATUS */
#define LVDSSS_LVDS_AFE_LVDS_BER_STATUS_LVDS_DATA_BER_Pos 0UL
#define LVDSSS_LVDS_AFE_LVDS_BER_STATUS_LVDS_DATA_BER_Msk 0xFFUL
#define LVDSSS_LVDS_AFE_LVDS_BER_STATUS_LVDS_CTRL_BER_Pos 8UL
#define LVDSSS_LVDS_AFE_LVDS_BER_STATUS_LVDS_CTRL_BER_Msk 0x100UL


/* LVDSSS_LVDS_ADAPTER_DMA_SCK.SCK_DSCR */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_DSCR_NUMBER_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_DSCR_NUMBER_Msk 0xFFFFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_DSCR_COUNT_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_DSCR_COUNT_Msk 0xFF0000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_DSCR_LOW_Pos 24UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_DSCR_LOW_Msk 0xFF000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.SCK_SIZE */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_SIZE_TRANS_SIZE_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_SIZE_TRANS_SIZE_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.SCK_COUNT */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_COUNT_TRANS_COUNT_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_COUNT_TRANS_COUNT_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.SCK_STATUS */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_AVL_COUNT_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_AVL_COUNT_Msk 0x1FUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_AVL_MIN_Pos 5UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_AVL_MIN_Msk 0x3E0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_AVL_ENABLE_Pos 10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_AVL_ENABLE_Msk 0x400UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SCK_ZLP_ALWAYS_Pos 11UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SCK_ZLP_ALWAYS_Msk 0x800UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_STATE_Pos 15UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_STATE_Msk 0x38000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_ZLP_RCVD_Pos 18UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_ZLP_RCVD_Msk 0x40000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSPENDED_Pos 19UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSPENDED_Msk 0x80000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_ENABLED_Pos 20UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_ENABLED_Msk 0x100000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_TRUNCATE_Pos 21UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_TRUNCATE_Msk 0x200000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_EN_PROD_EVENTS_Pos 22UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_EN_PROD_EVENTS_Msk 0x400000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_EN_CONS_EVENTS_Pos 23UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_EN_CONS_EVENTS_Msk 0x800000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_PARTIAL_Pos 24UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_PARTIAL_Msk 0x1000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_LAST_Pos 25UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_LAST_Msk 0x2000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_TRANS_Pos 26UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_TRANS_Msk 0x4000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_EOP_Pos 27UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_SUSP_EOP_Msk 0x8000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_WRAPUP_Pos 28UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_WRAPUP_Msk 0x10000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_UNIT_Pos 29UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_UNIT_Msk 0x20000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_GO_SUSPEND_Pos 30UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_GO_SUSPEND_Msk 0x40000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_GO_ENABLE_Pos 31UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_STATUS_GO_ENABLE_Msk 0x80000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.SCK_INTR */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_PRODUCE_EVENT_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_PRODUCE_EVENT_Msk 0x1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_CONSUME_EVENT_Pos 1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_CONSUME_EVENT_Msk 0x2UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_DSCR_IS_LOW_Pos 2UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_DSCR_IS_LOW_Msk 0x4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_DSCR_NOT_AVL_Pos 3UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_DSCR_NOT_AVL_Msk 0x8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_STALL_Pos 4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_STALL_Msk 0x10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_SUSPEND_Pos 5UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_SUSPEND_Msk 0x20UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_ERROR_Pos 6UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_ERROR_Msk 0x40UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_TRANS_DONE_Pos 7UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_TRANS_DONE_Msk 0x80UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_PARTIAL_BUF_Pos 8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_PARTIAL_BUF_Msk 0x100UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_LAST_BUF_Pos 9UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_LAST_BUF_Msk 0x200UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_EVENT_RCVD_Pos 10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_EVENT_RCVD_Msk 0x400UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.SCK_INTR_MASK */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_PRODUCE_EVENT_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_PRODUCE_EVENT_Msk 0x1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_CONSUME_EVENT_Pos 1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_CONSUME_EVENT_Msk 0x2UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_DSCR_IS_LOW_Pos 2UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_DSCR_IS_LOW_Msk 0x4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_DSCR_NOT_AVL_Pos 3UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_DSCR_NOT_AVL_Msk 0x8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_STALL_Pos 4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_STALL_Msk 0x10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_SUSPEND_Pos 5UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_SUSPEND_Msk 0x20UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_ERROR_Pos 6UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_ERROR_Msk 0x40UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_TRANS_DONE_Pos 7UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_TRANS_DONE_Msk 0x80UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_PARTIAL_BUF_Pos 8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_PARTIAL_BUF_Msk 0x100UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_LAST_BUF_Pos 9UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_LAST_BUF_Msk 0x200UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_EVENT_RCVD_Pos 10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_INTR_MASK_EVENT_RCVD_Msk 0x400UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.DSCR_BUFFER */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_BUFFER_BUFFER_ADDR_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_BUFFER_BUFFER_ADDR_Msk 0x7FFFFFFFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_BUFFER_MARKER_Pos 31UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_BUFFER_MARKER_Msk 0x80000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.DSCR_SYNC */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_SCK_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_SCK_Msk 0xFFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_IP_Pos 8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_CONS_IP_Msk 0x3F00UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_EVENT_Pos 14UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_EVENT_Msk 0x4000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_INT_Pos 15UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_CONS_INT_Msk 0x8000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_SCK_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_SCK_Msk 0xFF0000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_IP_Pos 24UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_PROD_IP_Msk 0x3F000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_EVENT_Pos 30UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_EVENT_Msk 0x40000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_INT_Pos 31UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SYNC_EN_PROD_INT_Msk 0x80000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.DSCR_CHAIN */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_CHAIN_RD_NEXT_DSCR_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_CHAIN_RD_NEXT_DSCR_Msk 0xFFFFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_CHAIN_WR_NEXT_DSCR_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_CHAIN_WR_NEXT_DSCR_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.DSCR_SIZE */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_MSB_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_MSB_Msk 0x1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_EOP_Pos 1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_EOP_Msk 0x2UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_ERROR_Pos 2UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_ERROR_Msk 0x4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_OCCUPIED_Pos 3UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_OCCUPIED_Msk 0x8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_SIZE_Pos 4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BUFFER_SIZE_Msk 0xFFF0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_DSCR_SIZE_BYTE_COUNT_Msk 0xFFFF0000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK.EVENT */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_EVENT_ACTIVE_DSCR_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_EVENT_ACTIVE_DSCR_Msk 0xFFFFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_EVENT_EVENT_TYPE_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_EVENT_EVENT_TYPE_Msk 0x10000UL


/* LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL.SCK_INTR */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_SCK_INTR_SCKINTR_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_SCK_INTR_SCKINTR_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL.ADAPTER_CTRL */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAPTER_EN_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAPTER_EN_Msk 0x1UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_WR_PRIO_Pos 4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_WR_PRIO_Msk 0x10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_RD_PRIO_Pos 5UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DATA_RD_PRIO_Msk 0x20UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_WR_PRIO_N_Pos 6UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_WR_PRIO_N_Msk 0x40UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_RD_PRIO_N_Pos 7UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_ADAP_DESC_RD_PRIO_N_Msk 0x80UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_SUSP_PARTIAL_NO_EVT_N_Pos 10UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_SUSP_PARTIAL_NO_EVT_N_Msk 0x400UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_FQ_DEPTH_CTRL_Pos 12UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_FQ_DEPTH_CTRL_Msk 0x3F000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_DEBUG_CAPTURE_Pos 20UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_DEBUG_CAPTURE_Msk 0x100000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_EN_UNALIGNED_READ_N_Pos 21UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_EN_UNALIGNED_READ_N_Msk 0x200000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_TR_OUT_DELAY_Pos 24UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CTRL_TR_OUT_DELAY_Msk 0x3F000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL.ADAPTER_DEBUG */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_COUNT_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_COUNT_Msk 0xFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_STATE_Pos 4UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_STATE_Msk 0x70UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ID_Pos 7UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ID_Msk 0x1F80UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_COUNT_Pos 13UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_COUNT_Msk 0x3FE000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_STATE_Pos 22UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_STATE_Msk 0x400000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_ID_Pos 23UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_SS_ID_Msk 0x1F800000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ABORT_Pos 29UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_TS_ABORT_Msk 0x20000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_BS_STATE_Pos 30UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_DEBUG_BS_STATE_Msk 0xC0000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL.ADAPTER_CONF */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SOCKET_ACTIVE_THRSHLD_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SOCKET_ACTIVE_THRSHLD_Msk 0x3FUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAM_COUNT_Pos 6UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAM_COUNT_Msk 0x7FC0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAMING_MODE_Pos 15UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_STREAMING_MODE_Msk 0x8000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_CYCLES_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_CYCLES_Msk 0xF0000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_ES_CYCLES_Pos 20UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_ES_CYCLES_Msk 0xF00000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_GBL_CYCLES_Pos 24UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_GBL_CYCLES_Msk 0xF000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_MMIO_LOW_PRIORITY_Pos 28UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_MMIO_LOW_PRIORITY_Msk 0x10000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SWITCH_HIGH_PRIORITY_Pos 29UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_SWITCH_HIGH_PRIORITY_Msk 0x20000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_ABORT_EN_Pos 30UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_TS_ABORT_EN_Msk 0x40000000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_DESCR_PF_EN_N_Pos 31UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_CONF_DESCR_PF_EN_N_Msk 0x80000000UL
/* LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL.ADAPTER_STATUS */
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_TTL_SOCKETS_Pos 0UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_TTL_SOCKETS_Msk 0xFFUL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_IG_ONLY_Pos 8UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_IG_ONLY_Msk 0xFF00UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_FQ_SIZE_Pos 16UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_FQ_SIZE_Msk 0x1FF0000UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_WORD_SIZE_Pos 25UL
#define LVDSSS_LVDS_ADAPTER_DMA_SCK_GBL_ADAPTER_STATUS_WORD_SIZE_Msk 0x6000000UL


/* LVDSSS_LVDS.CTL */
#define LVDSSS_LVDS_CTL_PHY_ENABLED_Pos         0UL
#define LVDSSS_LVDS_CTL_PHY_ENABLED_Msk         0x1UL
#define LVDSSS_LVDS_CTL_LINK_ENABLED_Pos        1UL
#define LVDSSS_LVDS_CTL_LINK_ENABLED_Msk        0x2UL
#define LVDSSS_LVDS_CTL_DMA_ENABLED_Pos         2UL
#define LVDSSS_LVDS_CTL_DMA_ENABLED_Msk         0x4UL
#define LVDSSS_LVDS_CTL_IP_ENABLED_Pos          31UL
#define LVDSSS_LVDS_CTL_IP_ENABLED_Msk          0x80000000UL
/* LVDSSS_LVDS.LINK_CONFIG */
#define LVDSSS_LVDS_LINK_CONFIG_LVDS_MODE_Pos   0UL
#define LVDSSS_LVDS_LINK_CONFIG_LVDS_MODE_Msk   0x1UL
#define LVDSSS_LVDS_LINK_CONFIG_GEARING_RATIO_Pos 1UL
#define LVDSSS_LVDS_LINK_CONFIG_GEARING_RATIO_Msk 0x6UL
#define LVDSSS_LVDS_LINK_CONFIG_WIDE_LINK_Pos   3UL
#define LVDSSS_LVDS_LINK_CONFIG_WIDE_LINK_Msk   0x8UL
#define LVDSSS_LVDS_LINK_CONFIG_NUM_LANES_Pos   4UL
#define LVDSSS_LVDS_LINK_CONFIG_NUM_LANES_Msk   0xF0UL
#define LVDSSS_LVDS_LINK_CONFIG_TWO_BIT_SLV_FF_Pos 8UL
#define LVDSSS_LVDS_LINK_CONFIG_TWO_BIT_SLV_FF_Msk 0x100UL
#define LVDSSS_LVDS_LINK_CONFIG_LINK_ENABLE_Pos 15UL
#define LVDSSS_LVDS_LINK_CONFIG_LINK_ENABLE_Msk 0x8000UL
#define LVDSSS_LVDS_LINK_CONFIG_BYPASS_DDR_PHY_Pos 16UL
#define LVDSSS_LVDS_LINK_CONFIG_BYPASS_DDR_PHY_Msk 0x10000UL
/* LVDSSS_LVDS.THREAD_INTLV_CTL */
#define LVDSSS_LVDS_THREAD_INTLV_CTL_TH0_TH1_INTERLEAVED_Pos 0UL
#define LVDSSS_LVDS_THREAD_INTLV_CTL_TH0_TH1_INTERLEAVED_Msk 0x1UL
#define LVDSSS_LVDS_THREAD_INTLV_CTL_TH2_TH3_INTERLEAVED_Pos 1UL
#define LVDSSS_LVDS_THREAD_INTLV_CTL_TH2_TH3_INTERLEAVED_Msk 0x2UL
/* LVDSSS_LVDS.LVDS_INTR_WD0 */
#define LVDSSS_LVDS_LVDS_INTR_WD0_GPIF0_INTERRUPT_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_GPIF0_INTERRUPT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_GPIF1_INTERRUPT_Pos 1UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_GPIF1_INTERRUPT_Msk 0x2UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_PHY_LINK0_INTERRUPT_Pos 2UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_PHY_LINK0_INTERRUPT_Msk 0x4UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_PHY_LINK1_INTERRUPT_Pos 3UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_PHY_LINK1_INTERRUPT_Msk 0x8UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD0_ERR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD0_ERR_Msk 0x10UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD1_ERR_Pos 5UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD1_ERR_Msk 0x20UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD2_ERR_Pos 6UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD2_ERR_Msk 0x40UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD3_ERR_Pos 7UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD3_ERR_Msk 0x80UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_FF_OVER_FLOW_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_FF_OVER_FLOW_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_FF_OVER_FLOW_Pos 9UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_FF_OVER_FLOW_Msk 0x200UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_TRAINING_DONE_Pos 10UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_TRAINING_DONE_Msk 0x400UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_TRAINING_DONE_Pos 11UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_TRAINING_DONE_Msk 0x800UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_TRAINING_BLK_DETECTED_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_TRAINING_BLK_DETECTED_Msk 0x1000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_TRAINING_BLK_DETECTED_Pos 13UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_TRAINING_BLK_DETECTED_Msk 0x2000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_TRAINING_BLK_DET_FAILD_Pos 14UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_TRAINING_BLK_DET_FAILD_Msk 0x4000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_TRAINING_BLK_DET_FAILD_Pos 15UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_TRAINING_BLK_DET_FAILD_Msk 0x8000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD0_DATA_CRC_ERR_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD0_DATA_CRC_ERR_Msk 0x10000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD1_DATA_CRC_ERR_Pos 17UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD1_DATA_CRC_ERR_Msk 0x20000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD2_DATA_CRC_ERR_Pos 18UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD2_DATA_CRC_ERR_Msk 0x40000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD3_DATA_CRC_ERR_Pos 19UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_THREAD3_DATA_CRC_ERR_Msk 0x80000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_L1_ENTRY_Pos 26UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_L1_ENTRY_Msk 0x4000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_L1_ENTRY_Pos 27UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_L1_ENTRY_Msk 0x8000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_L1_EXIT_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_L1_EXIT_Msk 0x10000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_L1_EXIT_Pos 29UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_L1_EXIT_Msk 0x20000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_L3_ENTRY_Pos 30UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK0_L3_ENTRY_Msk 0x40000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_L3_ENTRY_Pos 31UL
#define LVDSSS_LVDS_LVDS_INTR_WD0_LNK1_L3_ENTRY_Msk 0x80000000UL
/* LVDSSS_LVDS.LVDS_INTR_WD1 */
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT0_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT0_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT1_Pos 1UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT1_Msk 0x2UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT2_Pos 2UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT2_Msk 0x4UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT3_Pos 3UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_CLR_BIT3_Msk 0x8UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT0_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT0_Msk 0x10UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT1_Pos 5UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT1_Msk 0x20UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT2_Pos 6UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT2_Msk 0x40UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT3_Pos 7UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_CLR_BIT3_Msk 0x80UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT0_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT0_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT1_Pos 9UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT1_Msk 0x200UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT2_Pos 10UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT2_Msk 0x400UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT3_Pos 11UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_CLR_BIT3_Msk 0x800UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT0_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT0_Msk 0x1000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT1_Pos 13UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT1_Msk 0x2000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT2_Pos 14UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT2_Msk 0x4000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT3_Pos 15UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_CLR_BIT3_Msk 0x8000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT0_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT0_Msk 0x10000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT1_Pos 17UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT1_Msk 0x20000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT2_Pos 18UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT2_Msk 0x40000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT3_Pos 19UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH0_HDR_FLGS_SET_BIT3_Msk 0x80000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT0_Pos 20UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT0_Msk 0x100000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT1_Pos 21UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT1_Msk 0x200000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT2_Pos 22UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT2_Msk 0x400000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT3_Pos 23UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH1_HDR_FLGS_SET_BIT3_Msk 0x800000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT0_Pos 24UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT0_Msk 0x1000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT1_Pos 25UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT1_Msk 0x2000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT2_Pos 26UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT2_Msk 0x4000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT3_Pos 27UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH2_HDR_FLGS_SET_BIT3_Msk 0x8000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT0_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT0_Msk 0x10000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT1_Pos 29UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT1_Msk 0x20000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT2_Pos 30UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT2_Msk 0x40000000UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT3_Pos 31UL
#define LVDSSS_LVDS_LVDS_INTR_WD1_TH3_HDR_FLGS_SET_BIT3_Msk 0x80000000UL
/* LVDSSS_LVDS.LVDS_INTR_MASK_WD0 */
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_GPIF0_INTERRUPT_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_GPIF0_INTERRUPT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_GPIF1_INTERRUPT_Pos 1UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_GPIF1_INTERRUPT_Msk 0x2UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_PHY_LINK0_INTERRUPT_Pos 2UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_PHY_LINK0_INTERRUPT_Msk 0x4UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_PHY_LINK1_INTERRUPT_Pos 3UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_PHY_LINK1_INTERRUPT_Msk 0x8UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD0_ERR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD0_ERR_Msk 0x10UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD1_ERR_Pos 5UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD1_ERR_Msk 0x20UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD2_ERR_Pos 6UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD2_ERR_Msk 0x40UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD3_ERR_Pos 7UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD3_ERR_Msk 0x80UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_FF_OVER_FLOW_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_FF_OVER_FLOW_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_FF_OVER_FLOW_Pos 9UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_FF_OVER_FLOW_Msk 0x200UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_TRAINING_DONE_Pos 10UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_TRAINING_DONE_Msk 0x400UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_TRAINING_DONE_Pos 11UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_TRAINING_DONE_Msk 0x800UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_TRAINING_BLK_DETECTED_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_TRAINING_BLK_DETECTED_Msk 0x1000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_TRAINING_BLK_DETECTED_Pos 13UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_TRAINING_BLK_DETECTED_Msk 0x2000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_TRAINING_BLK_DET_FAILD_Pos 14UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_TRAINING_BLK_DET_FAILD_Msk 0x4000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_TRAINING_BLK_DET_FAILD_Pos 15UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_TRAINING_BLK_DET_FAILD_Msk 0x8000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD0_DATA_CRC_ERR_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD0_DATA_CRC_ERR_Msk 0x10000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD1_DATA_CRC_ERR_Pos 17UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD1_DATA_CRC_ERR_Msk 0x20000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD2_DATA_CRC_ERR_Pos 18UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD2_DATA_CRC_ERR_Msk 0x40000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD3_DATA_CRC_ERR_Pos 19UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_THREAD3_DATA_CRC_ERR_Msk 0x80000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_L1_ENTRY_Pos 26UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_L1_ENTRY_Msk 0x4000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_L1_ENTRY_Pos 27UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_L1_ENTRY_Msk 0x8000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_L1_EXIT_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_L1_EXIT_Msk 0x10000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_L1_EXIT_Pos 29UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_L1_EXIT_Msk 0x20000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_L3_ENTRY_Pos 30UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK0_L3_ENTRY_Msk 0x40000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_L3_ENTRY_Pos 31UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD0_LNK1_L3_ENTRY_Msk 0x80000000UL
/* LVDSSS_LVDS.LVDS_INTR_MASK_WD1 */
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH0_HDR_FLGS_CLR_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH0_HDR_FLGS_CLR_Msk 0xFUL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH1_HDR_FLGS_CLR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH1_HDR_FLGS_CLR_Msk 0xF0UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH2_HDR_FLGS_CLR_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH2_HDR_FLGS_CLR_Msk 0xF00UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH3_HDR_FLGS_CLR_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH3_HDR_FLGS_CLR_Msk 0xF000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH0_HDR_FLGS_SET_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH0_HDR_FLGS_SET_Msk 0xF0000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH1_HDR_FLGS_SET_Pos 20UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH1_HDR_FLGS_SET_Msk 0xF00000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH2_HDR_FLGS_SET_Pos 24UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH2_HDR_FLGS_SET_Msk 0xF000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH3_HDR_FLGS_SET_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_MASK_WD1_TH3_HDR_FLGS_SET_Msk 0xF0000000UL
/* LVDSSS_LVDS.LVDS_INTR_MASKED_WD0 */
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_GPIF0_INTERRUPT_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_GPIF0_INTERRUPT_Msk 0x1UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_GPIF1_INTERRUPT_Pos 1UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_GPIF1_INTERRUPT_Msk 0x2UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_PHY_LINK0_INTERRUPT_Pos 2UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_PHY_LINK0_INTERRUPT_Msk 0x4UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_PHY_LINK1_INTERRUPT_Pos 3UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_PHY_LINK1_INTERRUPT_Msk 0x8UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD0_ERR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD0_ERR_Msk 0x10UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD1_ERR_Pos 5UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD1_ERR_Msk 0x20UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD2_ERR_Pos 6UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD2_ERR_Msk 0x40UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD3_ERR_Pos 7UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD3_ERR_Msk 0x80UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_FF_OVER_FLOW_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_FF_OVER_FLOW_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_FF_OVER_FLOW_Pos 9UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_FF_OVER_FLOW_Msk 0x200UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_TRAINING_DONE_Pos 10UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_TRAINING_DONE_Msk 0x400UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_TRAINING_DONE_Pos 11UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_TRAINING_DONE_Msk 0x800UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_TRAINING_BLK_DETECTED_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_TRAINING_BLK_DETECTED_Msk 0x1000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_TRAINING_BLK_DETECTED_Pos 13UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_TRAINING_BLK_DETECTED_Msk 0x2000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_TRAINING_BLK_DET_FAILD_Pos 14UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_TRAINING_BLK_DET_FAILD_Msk 0x4000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_TRAINING_BLK_DET_FAILD_Pos 15UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_TRAINING_BLK_DET_FAILD_Msk 0x8000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD0_DATA_CRC_ERR_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD0_DATA_CRC_ERR_Msk 0x10000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD1_DATA_CRC_ERR_Pos 17UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD1_DATA_CRC_ERR_Msk 0x20000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD2_DATA_CRC_ERR_Pos 18UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD2_DATA_CRC_ERR_Msk 0x40000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD3_DATA_CRC_ERR_Pos 19UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_THREAD3_DATA_CRC_ERR_Msk 0x80000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_L1_ENTRY_Pos 26UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_L1_ENTRY_Msk 0x4000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_L1_ENTRY_Pos 27UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_L1_ENTRY_Msk 0x8000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_L1_EXIT_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_L1_EXIT_Msk 0x10000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_L1_EXIT_Pos 29UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_L1_EXIT_Msk 0x20000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_L3_ENTRY_Pos 30UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK0_L3_ENTRY_Msk 0x40000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_L3_ENTRY_Pos 31UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD0_LNK1_L3_ENTRY_Msk 0x80000000UL
/* LVDSSS_LVDS.LVDS_INTR_MASKED_WD1 */
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH0_HDR_FLGS_CLR_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH0_HDR_FLGS_CLR_Msk 0xFUL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH1_HDR_FLGS_CLR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH1_HDR_FLGS_CLR_Msk 0xF0UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH2_HDR_FLGS_CLR_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH2_HDR_FLGS_CLR_Msk 0xF00UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH3_HDR_FLGS_CLR_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH3_HDR_FLGS_CLR_Msk 0xF000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH0_HDR_FLGS_SET_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH0_HDR_FLGS_SET_Msk 0xF0000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH1_HDR_FLGS_SET_Pos 20UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH1_HDR_FLGS_SET_Msk 0xF00000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH2_HDR_FLGS_SET_Pos 24UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH2_HDR_FLGS_SET_Msk 0xF000000UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH3_HDR_FLGS_SET_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_MASKED_WD1_TH3_HDR_FLGS_SET_Msk 0xF0000000UL
/* LVDSSS_LVDS.LVDS_INTR_SET_WD0 */
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD0_ERR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD0_ERR_Msk 0x10UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD1_ERR_Pos 5UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD1_ERR_Msk 0x20UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD2_ERR_Pos 6UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD2_ERR_Msk 0x40UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD3_ERR_Pos 7UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD3_ERR_Msk 0x80UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_FF_OVER_FLOW_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_FF_OVER_FLOW_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_FF_OVER_FLOW_Pos 9UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_FF_OVER_FLOW_Msk 0x200UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_TRAINING_DONE_Pos 10UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_TRAINING_DONE_Msk 0x400UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_TRAINING_DONE_Pos 11UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_TRAINING_DONE_Msk 0x800UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_TRAINING_BLK_DETECTED_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_TRAINING_BLK_DETECTED_Msk 0x1000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_TRAINING_BLK_DETECTED_Pos 13UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_TRAINING_BLK_DETECTED_Msk 0x2000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_TRAINING_BLK_DET_FAILD_Pos 14UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_TRAINING_BLK_DET_FAILD_Msk 0x4000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_TRAINING_BLK_DET_FAILD_Pos 15UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_TRAINING_BLK_DET_FAILD_Msk 0x8000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD0_DATA_CRC_ERR_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD0_DATA_CRC_ERR_Msk 0x10000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD1_DATA_CRC_ERR_Pos 17UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD1_DATA_CRC_ERR_Msk 0x20000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD2_DATA_CRC_ERR_Pos 18UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD2_DATA_CRC_ERR_Msk 0x40000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD3_DATA_CRC_ERR_Pos 19UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_THREAD3_DATA_CRC_ERR_Msk 0x80000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_L1_ENTRY_Pos 26UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_L1_ENTRY_Msk 0x4000000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_L1_ENTRY_Pos 27UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_L1_ENTRY_Msk 0x8000000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_L1_EXIT_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_L1_EXIT_Msk 0x10000000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_L1_EXIT_Pos 29UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_L1_EXIT_Msk 0x20000000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_L3_ENTRY_Pos 30UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK0_L3_ENTRY_Msk 0x40000000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_L3_ENTRY_Pos 31UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD0_LNK1_L3_ENTRY_Msk 0x80000000UL
/* LVDSSS_LVDS.LVDS_INTR_SET_WD1 */
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH0_HDR_FLGS_CLR_Pos 0UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH0_HDR_FLGS_CLR_Msk 0xFUL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH1_HDR_FLGS_CLR_Pos 4UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH1_HDR_FLGS_CLR_Msk 0xF0UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH2_HDR_FLGS_CLR_Pos 8UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH2_HDR_FLGS_CLR_Msk 0xF00UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH3_HDR_FLGS_CLR_Pos 12UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH3_HDR_FLGS_CLR_Msk 0xF000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH0_HDR_FLGS_SET_Pos 16UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH0_HDR_FLGS_SET_Msk 0xF0000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH1_HDR_FLGS_SET_Pos 20UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH1_HDR_FLGS_SET_Msk 0xF00000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH2_HDR_FLGS_SET_Pos 24UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH2_HDR_FLGS_SET_Msk 0xF000000UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH3_HDR_FLGS_SET_Pos 28UL
#define LVDSSS_LVDS_LVDS_INTR_SET_WD1_TH3_HDR_FLGS_SET_Msk 0xF0000000UL
/* LVDSSS_LVDS.LVDS_ERROR */
#define LVDSSS_LVDS_LVDS_ERROR_THREAD0_ERR_CODE_Pos 0UL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD0_ERR_CODE_Msk 0xFUL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD1_ERR_CODE_Pos 4UL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD1_ERR_CODE_Msk 0xF0UL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD2_ERR_CODE_Pos 8UL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD2_ERR_CODE_Msk 0xF00UL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD3_ERR_CODE_Pos 12UL
#define LVDSSS_LVDS_LVDS_ERROR_THREAD3_ERR_CODE_Msk 0xF000UL
/* LVDSSS_LVDS.LVDS_EOP_EOT */
#define LVDSSS_LVDS_LVDS_EOP_EOT_EOP_EOT_CFG_Pos 0UL
#define LVDSSS_LVDS_LVDS_EOP_EOT_EOP_EOT_CFG_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS.LANE_FIFO_STS */
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE0_FF_OVERFLOW_Pos 0UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE0_FF_OVERFLOW_Msk 0x1UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE1_FF_OVERFLOW_Pos 1UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE1_FF_OVERFLOW_Msk 0x2UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE2_FF_OVERFLOW_Pos 2UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE2_FF_OVERFLOW_Msk 0x4UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE3_FF_OVERFLOW_Pos 3UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE3_FF_OVERFLOW_Msk 0x8UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE4_FF_OVERFLOW_Pos 4UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE4_FF_OVERFLOW_Msk 0x10UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE5_FF_OVERFLOW_Pos 5UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE5_FF_OVERFLOW_Msk 0x20UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE6_FF_OVERFLOW_Pos 6UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE6_FF_OVERFLOW_Msk 0x40UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE7_FF_OVERFLOW_Pos 7UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE7_FF_OVERFLOW_Msk 0x80UL
#define LVDSSS_LVDS_LANE_FIFO_STS_CTRL_LANE_FF_OVERFLOW_Pos 8UL
#define LVDSSS_LVDS_LANE_FIFO_STS_CTRL_LANE_FF_OVERFLOW_Msk 0x100UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE0_FF_UNDERFLOW_Pos 16UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE0_FF_UNDERFLOW_Msk 0x10000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE1_FF_UNDERFLOW_Pos 17UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE1_FF_UNDERFLOW_Msk 0x20000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE2_FF_UNDERFLOW_Pos 18UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE2_FF_UNDERFLOW_Msk 0x40000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE3_FF_UNDERFLOW_Pos 19UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE3_FF_UNDERFLOW_Msk 0x80000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE4_FF_UNDERFLOW_Pos 20UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE4_FF_UNDERFLOW_Msk 0x100000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE5_FF_UNDERFLOW_Pos 21UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE5_FF_UNDERFLOW_Msk 0x200000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE6_FF_UNDERFLOW_Pos 22UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE6_FF_UNDERFLOW_Msk 0x400000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE7_FF_UNDERFLOW_Pos 23UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LANE7_FF_UNDERFLOW_Msk 0x800000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_CTRL_LANE_FF_UNDERFLOW_Pos 24UL
#define LVDSSS_LVDS_LANE_FIFO_STS_CTRL_LANE_FF_UNDERFLOW_Msk 0x1000000UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LNK_FF_UNDERFLOW_Pos 31UL
#define LVDSSS_LVDS_LANE_FIFO_STS_LNK_FF_UNDERFLOW_Msk 0x80000000UL
/* LVDSSS_LVDS.GPIF_CLK_SEL */
#define LVDSSS_LVDS_GPIF_CLK_SEL_GPIF_CLK_SRC_Pos 0UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_GPIF_CLK_SRC_Msk 0x3UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_USB_CLK_DIV_VAL_Pos 2UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_USB_CLK_DIV_VAL_Msk 0xCUL
#define LVDSSS_LVDS_GPIF_CLK_SEL_GATE_WAVEFORM_MEM_CLK_Pos 4UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_GATE_WAVEFORM_MEM_CLK_Msk 0x10UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_LVCMOS_IF_CLK_100MHZ_Pos 5UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_LVCMOS_IF_CLK_100MHZ_Msk 0x20UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_WAVEFORM_RAM_CG_DURATION_Pos 8UL
#define LVDSSS_LVDS_GPIF_CLK_SEL_WAVEFORM_RAM_CG_DURATION_Msk 0xF00UL
/* LVDSSS_LVDS.USB_FRM_CNTR_CFG */
#define LVDSSS_LVDS_USB_FRM_CNTR_CFG_SEL_USB32_FRM_CNT_Pos 0UL
#define LVDSSS_LVDS_USB_FRM_CNTR_CFG_SEL_USB32_FRM_CNT_Msk 0x1UL
/* LVDSSS_LVDS.TIME_STAMP_CLK_DW0 */
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW0_TS_WD0_Pos 0UL
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW0_TS_WD0_Msk 0xFFFFUL
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW0_TS_WD1_Pos 16UL
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW0_TS_WD1_Msk 0xFFFF0000UL
/* LVDSSS_LVDS.TIME_STAMP_CLK_DW1 */
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW1_TS_WD2_Pos 0UL
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW1_TS_WD2_Msk 0xFFFFUL
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW1_TS_WD3_Pos 16UL
#define LVDSSS_LVDS_TIME_STAMP_CLK_DW1_TS_WD3_Msk 0xFFFF0000UL
/* LVDSSS_LVDS.SCRSS_VALUE_CFG */
#define LVDSSS_LVDS_SCRSS_VALUE_CFG_TS_CNTR_BITS_4SCRSS_Pos 0UL
#define LVDSSS_LVDS_SCRSS_VALUE_CFG_TS_CNTR_BITS_4SCRSS_Msk 0x3UL
#define LVDSSS_LVDS_SCRSS_VALUE_CFG_BUS_INTRVL_BITS_4SCRSS_Pos 2UL
#define LVDSSS_LVDS_SCRSS_VALUE_CFG_BUS_INTRVL_BITS_4SCRSS_Msk 0xCUL
/* LVDSSS_LVDS.TRAINING_BLK_CFG */
#define LVDSSS_LVDS_TRAINING_BLK_CFG_ENABLE_LNK0_DESKEW_Pos 0UL
#define LVDSSS_LVDS_TRAINING_BLK_CFG_ENABLE_LNK0_DESKEW_Msk 0x1UL
#define LVDSSS_LVDS_TRAINING_BLK_CFG_ENABLE_LNK1_DESKEW_Pos 1UL
#define LVDSSS_LVDS_TRAINING_BLK_CFG_ENABLE_LNK1_DESKEW_Msk 0x2UL
/* LVDSSS_LVDS.TRAINING_BLK */
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT0_Pos 0UL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT0_Msk 0xFFUL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT1_Pos 8UL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT1_Msk 0xFF00UL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT2_Pos 16UL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT2_Msk 0xFF0000UL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT3_Pos 24UL
#define LVDSSS_LVDS_TRAINING_BLK_TRAINING_BLK_BYT3_Msk 0xFF000000UL
/* LVDSSS_LVDS.LINK_TRAINING_STS */
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE0_TRAINING_BLK_DTCTD_Pos 0UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE0_TRAINING_BLK_DTCTD_Msk 0x1UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE1_TRAINING_BLK_DTCTD_Pos 1UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE1_TRAINING_BLK_DTCTD_Msk 0x2UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE2_TRAINING_BLK_DTCTD_Pos 2UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE2_TRAINING_BLK_DTCTD_Msk 0x4UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE3_TRAINING_BLK_DTCTD_Pos 3UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE3_TRAINING_BLK_DTCTD_Msk 0x8UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE4_TRAINING_BLK_DTCTD_Pos 4UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE4_TRAINING_BLK_DTCTD_Msk 0x10UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE5_TRAINING_BLK_DTCTD_Pos 5UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE5_TRAINING_BLK_DTCTD_Msk 0x20UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE6_TRAINING_BLK_DTCTD_Pos 6UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE6_TRAINING_BLK_DTCTD_Msk 0x40UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE7_TRAINING_BLK_DTCTD_Pos 7UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE7_TRAINING_BLK_DTCTD_Msk 0x80UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_CTRL_LANE_TRAINING_BLK_DTCTD_Pos 8UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_CTRL_LANE_TRAINING_BLK_DTCTD_Msk 0x100UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE0_TRAINING_BLK_DTCT_FAILED_Pos 16UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE0_TRAINING_BLK_DTCT_FAILED_Msk 0x10000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE1_TRAINING_BLK_DTCT_FAILED_Pos 17UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE1_TRAINING_BLK_DTCT_FAILED_Msk 0x20000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE2_TRAINING_BLK_DTCT_FAILED_Pos 18UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE2_TRAINING_BLK_DTCT_FAILED_Msk 0x40000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE3_TRAINING_BLK_DTCT_FAILED_Pos 19UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE3_TRAINING_BLK_DTCT_FAILED_Msk 0x80000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE4_TRAINING_BLK_DTCT_FAILED_Pos 20UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE4_TRAINING_BLK_DTCT_FAILED_Msk 0x100000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE5_TRAINING_BLK_DTCT_FAILED_Pos 21UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE5_TRAINING_BLK_DTCT_FAILED_Msk 0x200000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE6_TRAINING_BLK_DTCT_FAILED_Pos 22UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE6_TRAINING_BLK_DTCT_FAILED_Msk 0x400000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE7_TRAINING_BLK_DTCT_FAILED_Pos 23UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_LANE7_TRAINING_BLK_DTCT_FAILED_Msk 0x800000UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_CTRL_LANE_TRAINING_BLK_DTCT_FAILED_Pos 24UL
#define LVDSSS_LVDS_LINK_TRAINING_STS_CTRL_LANE_TRAINING_BLK_DTCT_FAILED_Msk 0x1000000UL
/* LVDSSS_LVDS.DDFT_MUX_SEL */
#define LVDSSS_LVDS_DDFT_MUX_SEL_DDFT0_MUX_SEL_Pos 0UL
#define LVDSSS_LVDS_DDFT_MUX_SEL_DDFT0_MUX_SEL_Msk 0xFFFUL
#define LVDSSS_LVDS_DDFT_MUX_SEL_DDFT1_MUX_SEL_Pos 15UL
#define LVDSSS_LVDS_DDFT_MUX_SEL_DDFT1_MUX_SEL_Msk 0x7FF8000UL
/* LVDSSS_LVDS.GPIO_DDFT_MUX_SEL */
#define LVDSSS_LVDS_GPIO_DDFT_MUX_SEL_GPIO_DDFT0_MUX_SEL_Pos 0UL
#define LVDSSS_LVDS_GPIO_DDFT_MUX_SEL_GPIO_DDFT0_MUX_SEL_Msk 0xFFFUL
#define LVDSSS_LVDS_GPIO_DDFT_MUX_SEL_GPIO_DDFT1_MUX_SEL_Pos 15UL
#define LVDSSS_LVDS_GPIO_DDFT_MUX_SEL_GPIO_DDFT1_MUX_SEL_Msk 0x7FF8000UL
/* LVDSSS_LVDS.LOOPBACK_CFG */
#define LVDSSS_LVDS_LOOPBACK_CFG_LINK0_PUT_LNK1_TESTER_EN_Pos 0UL
#define LVDSSS_LVDS_LOOPBACK_CFG_LINK0_PUT_LNK1_TESTER_EN_Msk 0x1UL
#define LVDSSS_LVDS_LOOPBACK_CFG_LINK1_PUT_LNK0_TESTER_EN_Pos 1UL
#define LVDSSS_LVDS_LOOPBACK_CFG_LINK1_PUT_LNK0_TESTER_EN_Msk 0x2UL
/* LVDSSS_LVDS.LVDS_CHAR_CFG */
#define LVDSSS_LVDS_LVDS_CHAR_CFG_LVDS_DATA_LANE_EN_Pos 0UL
#define LVDSSS_LVDS_LVDS_CHAR_CFG_LVDS_DATA_LANE_EN_Msk 0xFFUL
#define LVDSSS_LVDS_LVDS_CHAR_CFG_LVDS_CTRL_LANE_EN_Pos 8UL
#define LVDSSS_LVDS_LVDS_CHAR_CFG_LVDS_CTRL_LANE_EN_Msk 0x100UL
#define LVDSSS_LVDS_LVDS_CHAR_CFG_LVDS_CHAR_EN_Pos 31UL
#define LVDSSS_LVDS_LVDS_CHAR_CFG_LVDS_CHAR_EN_Msk 0x80000000UL
/* LVDSSS_LVDS.CLK_GATE_DIS */
#define LVDSSS_LVDS_CLK_GATE_DIS_TH0_CLK_GATE_DISABLE_Pos 0UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH0_CLK_GATE_DISABLE_Msk 0x1UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH1_CLK_GATE_DISABLE_Pos 1UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH1_CLK_GATE_DISABLE_Msk 0x2UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH2_CLK_GATE_DISABLE_Pos 2UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH2_CLK_GATE_DISABLE_Msk 0x4UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH3_CLK_GATE_DISABLE_Pos 3UL
#define LVDSSS_LVDS_CLK_GATE_DIS_TH3_CLK_GATE_DISABLE_Msk 0x8UL
/* LVDSSS_LVDS.VERSION_SEL */
#define LVDSSS_LVDS_VERSION_SEL_A0_SEL_Pos      0UL
#define LVDSSS_LVDS_VERSION_SEL_A0_SEL_Msk      0x1UL
#define LVDSSS_LVDS_VERSION_SEL_SKEW_MONITOR_A0_SEL_Pos 1UL
#define LVDSSS_LVDS_VERSION_SEL_SKEW_MONITOR_A0_SEL_Msk 0x2UL
/* LVDSSS_LVDS.TH0_TH1_METADATA_RAM */
#define LVDSSS_LVDS_TH0_TH1_METADATA_RAM_METADATA_Pos 0UL
#define LVDSSS_LVDS_TH0_TH1_METADATA_RAM_METADATA_Msk 0xFFFFFFFFUL
/* LVDSSS_LVDS.TH2_TH3_METADATA_RAM */
#define LVDSSS_LVDS_TH2_TH3_METADATA_RAM_METADATA_Pos 0UL
#define LVDSSS_LVDS_TH2_TH3_METADATA_RAM_METADATA_Msk 0xFFFFFFFFUL


#endif /* _CYIP_LVDSSS_H_ */


/* [] END OF FILE */
