/*!
 * @file      sx1280_regs.h
 *
 * @brief     SX1280 register definitions
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH S.A. BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SX1280_REGS_H__
#define __SX1280_REGS_H__

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*!
 * Firmware version value register. (16 bits)
 */
#define SX1280_REG_FW_VERSION 0x01F0

/*!
 * GFSK CRC seed value register. (16 bits)
 */
#define SX1280_REG_GFSK_CRC_SEED 0x09C8

/*!
 * GFSK polynomial value register. (16 bits)
 */
#define SX1280_REG_GFSK_CRC_POLYNOM 0x09C6

/*!
 * GFSK whitening seed value register. (16 bits)
 */
#define SX1280_REG_GFSK_WHITENING_SEED 0x09C5

/*!
 * FLRC CRC seed value register. (32 bits)
 */
#define SX1280_REG_FLRC_CRC_SEED 0x09C6

/*!
 * Ranging ID length check control register. (8 bits)
 */
#define SX1280_REG_RANGE_ID_LEN_CHECK 0x0931

/*!
 * Ranging slave req address register. (32 bits)
 */
#define SX1280_REG_RANGE_SLV_REQ_ADDR 0x0916

/*!
 * Ranging master req address register. (32 bits)
 */
#define SX1280_REG_RANGE_MST_REQ_ADDR 0x0912

/*!
 * Ranging results configuration register. (8 bits)
 */
#define SX1280_REG_RANGE_RESULTS_CFG 0x0924

/*!
 * LoRa SF additional configuration register. (8 bits)
 */
#define SX1280_REG_LORA_SF_CFG0 0x0925

/*!
 * LoRa frequency error correction register (3 bits)
 */
#define SX1280_REG_LORA_FREQ_ERR_CORR 0x093C
#define SX1280_REG_LORA_FREQ_ERR_CORR_POS 0x00
#define SX1280_REG_LORA_FREQ_ERR_CORR_MASK 0x07

/*!
 * Raging results register. (24 bits)
 */
#define SX1280_REG_RANGE_RESULTS 0x0961

/*!
 * Raging results freeze control register. (8 bits)
 */
#define SX1280_REG_RANGE_RESULTS_FREEZE 0x097F

/*!
 * Ranging Rx/Tx delay calibration value register. (24 bits)
 */
#define SX1280_REG_RANGE_RX_TX_DELAY_CAL 0x092B

/*!
 * Ranging sliding filter window size ragister. (8 bits)
 */
#define SX1280_REG_RANGE_SLD_FILTER_WINDOW_SIZE 0x091E

/*!
 * Ranging sliding filter reset register. (8 bits)
 */
#define SX1280_REG_RANGE_SLD_FILTER_RST 0x0923

/*!
 * The RSSI value of the last ranging exchange
 */
#define SX1280_REG_RANGE_RSSI 0x0964

/*!
 * Indicates the LoRa modem header mode. 0=Header, 1=No header
 */
#define SX1280_REG_LORA_HEADER_MODE 0x903

/*!
 * LoRa payload length value register. Required only when using implicit header
 * mode.
 *
 * \remark SHALL be used only when calling \ref sx1280_get_rx_buffer_status_lora
 */
#define SX1280_REG_LR_PAYLOAD_LENGTH 0x901

/*!
 * GFSK sync word 1 value register. (40 bits)
 */
#define SX1280_REG_GFSK_SYNC_WORD_1 0x09CE

/*!
 * GFSK sync word 2 value register. (40 bits)
 */
#define SX1280_REG_GFSK_SYNC_WORD_2 0x09D3

/*!
 * GFSK sync word 3 value register. (40 bits)
 */
#define SX1280_REG_GFSK_SYNC_WORD_3 0x09D8

/*!
 * FLRC sync word 1 value register. (32 bits)
 */
#define SX1280_REG_FLRC_SYNC_WORD_1 0x09CF

/*!
 * FLRC sync word 2 value register. (32 bits)
 */
#define SX1280_REG_FLRC_SYNC_WORD_2 0x09D4

/*!
 * FLRC sync word 3 value register. (32 bits)
 */
#define SX1280_REG_FLRC_SYNC_WORD_3 0x09D9

/*!
 * Defines how many bit errors are tolerated in sync word detection
 */
#define SX1280_REG_GFSK_FLRC_SYNC_WORD_TOLERANCE 0x09CD

/*!
 * The addresses of the register holding LoRa Modem SyncWord value
 *     0x1424: LoRaWAN private network,
 *     0x3444: LoRaWAN public network
 */
#define SX1280_REG_LR_SYNC_WORD 0x0944

/*!
 * LoRa Frequency Error Indicator (FEI) register. (20 bits signed - 3 bytes)
 */
#define SX1280_REG_LR_FEI 0x0954

/*!
 * Register for MSB Access Address (BLE)
 */
#define SX1280_REG_BLE_ACCESS_ADDRESS 0x09CF

/*!
 * Register address and mask for LNA regime selection
 */
#define SX1280_REG_LNA_REGIME 0x0891

/*!
 * Manual Gain Control control register
 */
#define SX1280_REG_MGC_CTRL_1 0x089F

/*!
 * Register and mask controlling demodulation detection
 */
#define SX1280_REG_MGC_CTRL_2 0x0895

/*!
 * Manual Gain Control value register
 */
#define SX1280_REG_MGC_VALUE 0x089E

/*!
 * LoRa incoming packet coding rate register
 */
#define SX1280_REG_LR_INCOMING_CR 0x0950
#define SX1280_REG_LR_INCOMING_CR_POS 0x04
#define SX1280_REG_LR_INCOMING_CR_MASK 0x07

/*!
 * LoRa incoming packet coding rate register
 */
#define SX1280_REG_LR_INCOMING_CRC 0x0954
#define SX1280_REG_LR_INCOMING_CRC_POS 0x04
#define SX1280_REG_LR_INCOMING_CRC_MASK 0x01

/*!
 * LoRa incoming packet coding rate register
 */
#define SX1280_REG_LR_IQ_CONF 0x093B

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

#endif  // __SX1280_REGS_H__

/* --- EOF ------------------------------------------------------------------ */
