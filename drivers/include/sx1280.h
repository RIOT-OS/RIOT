/*!
 * @file      sx1280.h
 *
 * @brief     SX1280 radio driver definition
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

#ifndef __SX1280_H__
#define __SX1280_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdbool.h>
#include <stdint.h>

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

// TODO: Move the following 2 defines to the RAL
/*!
 * CCITT CRC algorithm polynomial definition
 * Polynom : P16(x) = x16 + x12 + x5 + 1
 */
#define SX1280_CRC_POLYNOMIAL_CCITT 0x1021

/*!
 * CCITT CRC algorithm seed definition
 */
#define SX1280_CRC_CCITT_SEED 0x1D0F

/*!
 * Minimum Tx power
 */
#define SX1280_PWR_MIN (-18)

/*!
 * Maximum Tx power
 */
#define SX1280_PWR_MAX (13)

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*!
 * API return status
 */
typedef enum sx1280_status_e {
    SX1280_STATUS_OK = 0,
    SX1280_STATUS_UNSUPPORTED_FEATURE,
    SX1280_STATUS_UNKNOWN_VALUE,
    SX1280_STATUS_ERROR,
} sx1280_status_t;

/*!
 * Operating modes
 *
 * \remark The radio doesn't provide a way to get the current operating mode.
 *         The API sx1280_get_operating_mode returns the current state stored on
 *         a variable handled at the HAL level
 */
typedef enum sx1280_operating_mode_e {
    SX1280_OP_MODE_SLEEP = 0x00,    //! The radio is in sleep mode
    SX1280_OP_MODE_STDBY_RC,        //! The radio is in standby mode with RC
                                    //! oscillator
    SX1280_OP_MODE_STDBY_XOSC,      //! The radio is in standby mode with XOSC
                                    //! oscillator
    SX1280_OP_MODE_FS,              //! The radio is in frequency synthesis mode
    SX1280_OP_MODE_TX,              //! The radio is in transmit mode
    SX1280_OP_MODE_RX,              //! The radio is in receive single mode
    SX1280_OP_MODE_RX_C,            //! The radio is in receive continuous mode
    SX1280_OP_MODE_RX_DC,           //! The radio is in receive duty cycle mode
    SX1280_OP_MODE_CAD              //! The radio is in channel activity detection mode
} sx1280_operating_mode_t;

/*!
 * Sleep modes configurations
 */
enum sx1280_sleep_cfg_e {
    SX1280_SLEEP_CFG_DATA_RETENTION         = (1 << 0),
    SX1280_SLEEP_CFG_DATA_BUFFER_RETENTION  = (1 << 1),
    SX1280_SLEEP_CFG_IRAM_RETENTION         = (1 << 2),
};

typedef uint8_t sx1280_sleep_cfg_t;

/*!
 * Standby mode configurations
 */
typedef enum sx1280_standby_cfg_e {
    SX1280_STANDBY_CFG_RC   = 0x00,
    SX1280_STANDBY_CFG_XOSC = 0x01,
} sx1280_standby_cfg_t;

/*!
 * Power regulations used to power the device
 */
typedef enum sx1280_reg_mod_e {
    SX1280_REG_MODE_LDO     = 0x00, // default
    SX1280_REG_MODE_DCDC    = 0x01,
} sx1280_reg_mod_t;

/*!
 * Tick sizes available for Rx/Tx timeout operations
 */
typedef enum sx1280_tick_size_e {
    SX1280_TICK_SIZE_0015_US    = 0x00,
    SX1280_TICK_SIZE_0062_US    = 0x01,
    SX1280_TICK_SIZE_1000_US    = 0x02,
    SX1280_TICK_SIZE_4000_US    = 0x03,
} sx1280_tick_size_t;

/*!
 * IRQ definition
 */
enum sx1280_irq_mask_e {
    SX1280_IRQ_NONE                         = (0 << 0),
    SX1280_IRQ_TX_DONE                      = (1 << 0),
    SX1280_IRQ_RX_DONE                      = (1 << 1),
    SX1280_IRQ_SYNC_WORD_VALID              = (1 << 2),
    SX1280_IRQ_SYNC_WORD_ERROR              = (1 << 3),
    SX1280_IRQ_HEADER_VALID                 = (1 << 4),
    SX1280_IRQ_HEADER_ERROR                 = (1 << 5),
    SX1280_IRQ_CRC_ERROR                    = (1 << 6),
    SX1280_IRQ_RANGING_SLV_RES_DONE         = (1 << 7),
    SX1280_IRQ_RANGING_SLV_REQ_DISCARDED    = (1 << 8),
    SX1280_IRQ_RANGING_MST_RES_VALID        = (1 << 9),
    SX1280_IRQ_RANGING_MST_TIMEOUT          = (1 << 10),
    SX1280_IRQ_RANGING_SLV_REQ_VALID        = (1 << 11),
    SX1280_IRQ_CAD_DONE                     = (1 << 12),
    SX1280_IRQ_CAD_DET                      = (1 << 13),
    SX1280_IRQ_TIMEOUT                      = (1 << 14),
    SX1280_IRQ_PBL_DET                      = (1 << 15),
    SX1280_IRQ_ADV_RANGING_VALID            = (1 << 15),
    SX1280_IRQ_ALL                          = SX1280_IRQ_TX_DONE | SX1280_IRQ_RX_DONE |
                                              SX1280_IRQ_SYNC_WORD_VALID |
                                              SX1280_IRQ_SYNC_WORD_ERROR |
                                              SX1280_IRQ_HEADER_VALID | SX1280_IRQ_HEADER_ERROR |
                                              SX1280_IRQ_CRC_ERROR |
                                              SX1280_IRQ_RANGING_SLV_RES_DONE |
                                              SX1280_IRQ_RANGING_SLV_REQ_DISCARDED |
                                              SX1280_IRQ_RANGING_MST_RES_VALID |
                                              SX1280_IRQ_RANGING_MST_TIMEOUT |
                                              SX1280_IRQ_RANGING_SLV_REQ_VALID |
                                              SX1280_IRQ_CAD_DONE | SX1280_IRQ_CAD_DET |
                                              SX1280_IRQ_TIMEOUT |
                                              SX1280_IRQ_PBL_DET | SX1280_IRQ_ADV_RANGING_VALID,
};

typedef uint16_t sx1280_irq_mask_t;

/*!
 * Possible packet types (i.e. modem)
 */
typedef enum sx1280_pkt_type_e {
    SX1280_PKT_TYPE_GFSK    = 0x00,
    SX1280_PKT_TYPE_LORA    = 0x01,
    SX1280_PKT_TYPE_RANGING = 0x02,
    SX1280_PKT_TYPE_FLRC    = 0x03,
    SX1280_PKT_TYPE_BLE     = 0x04,
} sx1280_pkt_type_t;

/*!
 * Ramping times for the power amplifier
 */
typedef enum sx1280_ramp_time_e {
    SX1280_RAMP_02_US   = 0x00,
    SX1280_RAMP_04_US   = 0x20,
    SX1280_RAMP_06_US   = 0x40,
    SX1280_RAMP_08_US   = 0x60,
    SX1280_RAMP_10_US   = 0x80,
    SX1280_RAMP_12_US   = 0xA0,
    SX1280_RAMP_16_US   = 0xC0,
    SX1280_RAMP_20_US   = 0xE0,
} sx1280_ramp_time_t;

/*!
 * Combinations of bitrate and bandwidth for GFSK and BLE packet types
 *
 * \remark The bitrate is expressed in Mb/s and the bandwidth in MHz
 */
typedef enum sx1280_gfsk_ble_br_bw_e {
    SX1280_GFSK_BLE_BR_2_000_BW_2_4 = 0x04,
    SX1280_GFSK_BLE_BR_1_600_BW_2_4 = 0x28,
    SX1280_GFSK_BLE_BR_1_000_BW_2_4 = 0x4C,
    SX1280_GFSK_BLE_BR_1_000_BW_1_2 = 0x45,
    SX1280_GFSK_BLE_BR_0_800_BW_2_4 = 0x70,
    SX1280_GFSK_BLE_BR_0_800_BW_1_2 = 0x69,
    SX1280_GFSK_BLE_BR_0_500_BW_1_2 = 0x8D,
    SX1280_GFSK_BLE_BR_0_500_BW_0_6 = 0x86,
    SX1280_GFSK_BLE_BR_0_400_BW_1_2 = 0xB1,
    SX1280_GFSK_BLE_BR_0_400_BW_0_6 = 0xAA,
    SX1280_GFSK_BLE_BR_0_250_BW_0_6 = 0xCE,
    SX1280_GFSK_BLE_BR_0_250_BW_0_3 = 0xC7,
    SX1280_GFSK_BLE_BR_0_125_BW_0_3 = 0xEF,
} sx1280_gfsk_ble_br_bw_t;

/*!
 * Modulation indexes for GFSK and BLE packet types
 */
typedef enum sx1280_gfsk_ble_mod_ind_s {
    SX1280_GFSK_BLE_MOD_IND_0_35    = 0,
    SX1280_GFSK_BLE_MOD_IND_0_50    = 1,
    SX1280_GFSK_BLE_MOD_IND_0_75    = 2,
    SX1280_GFSK_BLE_MOD_IND_1_00    = 3,
    SX1280_GFSK_BLE_MOD_IND_1_25    = 4,
    SX1280_GFSK_BLE_MOD_IND_1_50    = 5,
    SX1280_GFSK_BLE_MOD_IND_1_75    = 6,
    SX1280_GFSK_BLE_MOD_IND_2_00    = 7,
    SX1280_GFSK_BLE_MOD_IND_2_25    = 8,
    SX1280_GFSK_BLE_MOD_IND_2_50    = 9,
    SX1280_GFSK_BLE_MOD_IND_2_75    = 10,
    SX1280_GFSK_BLE_MOD_IND_3_00    = 11,
    SX1280_GFSK_BLE_MOD_IND_3_25    = 12,
    SX1280_GFSK_BLE_MOD_IND_3_50    = 13,
    SX1280_GFSK_BLE_MOD_IND_3_75    = 14,
    SX1280_GFSK_BLE_MOD_IND_4_00    = 15,
} sx1280_gfsk_ble_mod_ind_t;

/*!
 * Modulation shapings for GFSK, FLRC and BLE packet types
 */
typedef enum sx1280_gfsk_flrc_ble_mod_shapes_e {
    SX1280_GFSK_FLRC_BLE_MOD_SHAPE_OFF      = 0x00,
    SX1280_GFSK_FLRC_BLE_MOD_SHAPE_BT_1     = 0x10,
    SX1280_GFSK_FLRC_BLE_MOD_SHAPE_BT_05    = 0x20,
} sx1280_gfsk_flrc_ble_mod_shapes_t;

/*!
 * Combinations of bitrate and bandwidth for GFSK packet type
 *
 * \remark The bitrate is expressed in Mb/s and the bandwidth in MHz
 */
typedef sx1280_gfsk_ble_br_bw_t sx1280_gfsk_br_bw_t;

/*!
 * Modulation indexes for GFSK packet type
 */
typedef sx1280_gfsk_ble_mod_ind_t sx1280_gfsk_mod_ind_t;

/*!
 * Modulation shaping for GFSK packet type
 */
typedef sx1280_gfsk_flrc_ble_mod_shapes_t sx1280_gfsk_mod_shapes_t;

/*!
 * Modulation parameters for GFSK packet type
 */
typedef struct sx1280_mod_params_gfsk_s {
    sx1280_gfsk_br_bw_t br_bw;
    sx1280_gfsk_mod_ind_t mod_ind;
    sx1280_gfsk_mod_shapes_t mod_shape;
} sx1280_mod_params_gfsk_t;

/*!
 * Spreading factor values for LoRa and Ranging packet types
 */
typedef enum sx1280_lora_range_sf_e {
    SX1280_LORA_RANGE_SF5   = 0x50,
    SX1280_LORA_RANGE_SF6   = 0x60,
    SX1280_LORA_RANGE_SF7   = 0x70,
    SX1280_LORA_RANGE_SF8   = 0x80,
    SX1280_LORA_RANGE_SF9   = 0x90,
    SX1280_LORA_RANGE_SF10  = 0xA0,
    SX1280_LORA_RANGE_SF11  = 0xB0,
    SX1280_LORA_RANGE_SF12  = 0xC0,
} sx1280_lora_range_sf_t;

/*!
 * Bandwidth values for LoRa and Ranging packet types
 */
typedef enum sx1280_lora_range_bw_e {
    SX1280_LORA_RANGE_BW_200    = 0x34,
    SX1280_LORA_RANGE_BW_400    = 0x26,
    SX1280_LORA_RANGE_BW_800    = 0x18,
    SX1280_LORA_RANGE_BW_1600   = 0x0A,
} sx1280_lora_range_bw_t;

/*!
 * Coding rate values for LoRa and Ranging packet types
 */
typedef enum sx1280_lora_range_cr_e {
    SX1280_LORA_RANGE_CR_4_5    = 0x01,
    SX1280_LORA_RANGE_CR_4_6    = 0x02,
    SX1280_LORA_RANGE_CR_4_7    = 0x03,
    SX1280_LORA_RANGE_CR_4_8    = 0x04,
    SX1280_LORA_RANGE_CR_LI_4_5 = 0x05,
    SX1280_LORA_RANGE_CR_LI_4_6 = 0x06,
    SX1280_LORA_RANGE_CR_LI_4_8 = 0x07,
} sx1280_lora_range_cr_t;

/*!
 * Spreading factors for LoRa packet types
 */
typedef sx1280_lora_range_sf_t sx1280_lora_sf_t;

/*!
 * Bandwidths for LoRa packet type
 */
typedef sx1280_lora_range_bw_t sx1280_lora_bw_t;

/*!
 * Coding rates for LoRa packet type
 */
typedef sx1280_lora_range_cr_t sx1280_lora_cr_t;

/*!
 * Modulation parameters for LoRa packet type
 */
typedef struct sx1280_mod_params_lora_s {
    sx1280_lora_sf_t sf;
    sx1280_lora_bw_t bw;
    sx1280_lora_cr_t cr;
} sx1280_mod_params_lora_t;

/*!
 * Spreading factors for Ranging packet types
 */
typedef sx1280_lora_range_sf_t sx1280_range_sf_t;

/*!
 * Bandwidths for Ranging packet type
 */
typedef sx1280_lora_range_bw_t sx1280_range_bw_t;

/*!
 * Coding rates for Ranging packet type
 */
typedef sx1280_lora_range_cr_t sx1280_range_cr_t;

/*!
 * Modulation parameters for Ranging packet type
 */
typedef struct sx1280_mod_params_range_s {
    sx1280_range_sf_t sf;
    sx1280_range_bw_t bw;
    sx1280_range_cr_t cr;
} sx1280_mod_params_range_t;

// TODO: Change values from M to k
/*!
 * Combinations of bitrate and bandwidth for FLRC packet type
 *
 * \remark The bitrate is in Mb/s and the bandwidth in MHz
 */
typedef enum sx1280_flrc_br_bw_e {
    SX1280_FLRC_BR_2_600_BW_2_4 = 0x04,
    SX1280_FLRC_BR_2_080_BW_2_4 = 0x28,
    SX1280_FLRC_BR_1_300_BW_1_2 = 0x45,
    SX1280_FLRC_BR_1_040_BW_1_2 = 0x69,
    SX1280_FLRC_BR_0_650_BW_0_6 = 0x86,
    SX1280_FLRC_BR_0_520_BW_0_6 = 0xAA,
    SX1280_FLRC_BR_0_325_BW_0_3 = 0xC7,
    SX1280_FLRC_BR_0_260_BW_0_3 = 0xEB,
} sx1280_flrc_br_bw_t;

/*!
 * Coding rates for FLRC packet type
 */
typedef enum sx1280_flrc_cr_e {
    SX1280_FLRC_CR_1_2  = 0x00,
    SX1280_FLRC_CR_3_4  = 0x02,
    SX1280_FLRC_CR_1_1  = 0x04,
} sx1280_flrc_cr_t;

/*!
 * Modulation shapings for FLRC packet types
 */
typedef sx1280_gfsk_flrc_ble_mod_shapes_t sx1280_flrc_mod_shapes_t;

/*!
 * Modulation parameters for FLRC packet type
 */
typedef struct sx1280_mod_params_flrc_s {
    sx1280_flrc_br_bw_t br_bw;
    sx1280_flrc_cr_t cr;
    sx1280_flrc_mod_shapes_t mod_shape;
} sx1280_mod_params_flrc_t;

/*!
 * Combinations of bitrate and bandwidth for BLE packet types
 *
 * \remark The bitrate is expressed in Mb/s and the bandwidth in MHz
 */
typedef sx1280_gfsk_ble_br_bw_t sx1280_ble_br_bw_t;

/*!
 * Modulation indexes for BLE packet types
 */
typedef sx1280_gfsk_ble_mod_ind_t sx1280_ble_mod_ind_t;

/*!
 * Modulation shapings for BLE packet types
 */
typedef sx1280_gfsk_flrc_ble_mod_shapes_t sx1280_ble_mod_shapes_t;

/*!
 * Modulation parameters for BLE packet type
 */
typedef struct sx1280_mod_params_ble_s {
    sx1280_ble_br_bw_t br_bw;
    sx1280_ble_mod_ind_t mod_ind;
    sx1280_ble_mod_shapes_t mod_shape;
} sx1280_mod_params_ble_t;

/*!
 * Preamble lengths for GFSK and FLRC packet types
 */
typedef enum sx1280_gfsk_flrc_pbl_len_e {
    SX1280_GFSK_FLRC_PBL_LEN_04_BITS    = 0x00,
    SX1280_GFSK_FLRC_PBL_LEN_08_BITS    = 0x10,
    SX1280_GFSK_FLRC_PBL_LEN_12_BITS    = 0x20,
    SX1280_GFSK_FLRC_PBL_LEN_16_BITS    = 0x30,
    SX1280_GFSK_FLRC_PBL_LEN_20_BITS    = 0x40,
    SX1280_GFSK_FLRC_PBL_LEN_24_BITS    = 0x50,
    SX1280_GFSK_FLRC_PBL_LEN_28_BITS    = 0x60,
    SX1280_GFSK_FLRC_PBL_LEN_32_BITS    = 0x70,
} sx1280_gfsk_flrc_pbl_len_t;

/*!
 * SyncWord lengths for GFSK packet type
 */
typedef enum sx1280_gfsk_sync_word_len_e {
    SX1280_GFSK_SYNC_WORD_LEN_1_BYTE    = 0x00,
    SX1280_GFSK_SYNC_WORD_LEN_2_BYTE    = 0x02,
    SX1280_GFSK_SYNC_WORD_LEN_3_BYTE    = 0x04,
    SX1280_GFSK_SYNC_WORD_LEN_4_BYTE    = 0x06,
    SX1280_GFSK_SYNC_WORD_LEN_5_BYTE    = 0x08,
} sx1280_gfsk_sync_word_len_t;

/*!
 * Combinations of SyncWord correlators activated for GFSK and FLRC packet
 * types
 */
typedef enum sx1280_gfsk_flrc_rx_match_sync_word_e {
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_OFF      = 0x00,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_1        = 0x10,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_2        = 0x20,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_1_2      = 0x30,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_3        = 0x40,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_1_3      = 0x50,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_2_3      = 0x60,
    SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_1_2_3    = 0x70,
} sx1280_gfsk_flrc_rx_match_sync_word_t;

/*!
 * Packet length mode for GFSK and FLRC packet types
 */
typedef enum sx1280_gfsk_flrc_pkt_len_modes_e {
    SX1280_GFSK_FLRC_PKT_FIX_LEN    = 0x00,
    SX1280_GFSK_FLRC_PKT_VAR_LEN    = 0x20,
} sx1280_gfsk_flrc_pkt_len_modes_t;

/*!
 * CRC lengths for GFSK packet types
 *
 * \remark Not all configurations are available for both GFSK and FLRC packet
 *         types. Please refer to the datasheet for possible configuration.
 */
typedef enum sx1280_gfsk_crc_types_e {
    SX1280_GFSK_CRC_OFF     = 0x00,
    SX1280_GFSK_CRC_1_BYTES = 0x10,
    SX1280_GFSK_CRC_2_BYTES = 0x20,
    SX1280_GFSK_CRC_3_BYTES = 0x30,
} sx1280_gfsk_crc_types_t;

/*!
 * Whitening modes for GFSK, FLRC and BLE packet types
 */
typedef enum sx1280_gfsk_flrc_ble_dc_free_e {
    SX1280_GFSK_FLRC_BLE_DC_FREE_OFF    = 0x08,
    SX1280_GFSK_FLRC_BLE_DC_FREE_ON     = 0x00,
} sx1280_gfsk_flrc_ble_dc_free_t;

/*!
 * Preamble lengths for GFS packet type
 */
typedef sx1280_gfsk_flrc_pbl_len_t sx1280_gfsk_pbl_len_t;

/*!
 * Combinations of SyncWord correlators activated for GFSK packet type
 */
typedef sx1280_gfsk_flrc_rx_match_sync_word_t sx1280_gfsk_rx_match_sync_word_t;

/*!
 * Packet length mode for GFSK packet type
 */
typedef sx1280_gfsk_flrc_pkt_len_modes_t sx1280_gfsk_pkt_len_modes_t;

/*!
 * Packet parameters for GFSK modulation
 */
typedef struct sx1280_pkt_params_gfsk_s {
    sx1280_gfsk_pbl_len_t pbl_len;
    sx1280_gfsk_sync_word_len_t sync_word_len;
    sx1280_gfsk_rx_match_sync_word_t match_sync_word;
    sx1280_gfsk_pkt_len_modes_t hdr_type;
    uint8_t pld_len_in_bytes;
    sx1280_gfsk_crc_types_t crc_type;
    sx1280_gfsk_flrc_ble_dc_free_t dc_free;
} sx1280_pkt_params_gfsk_t;

/*!
 * Packet lengths for LoRa and Ranging packet types
 */
typedef enum sx1280_lora_range_pkt_len_modes_e {
    SX1280_LORA_RANGE_PKT_EXPLICIT  = 0x00,
    SX1280_LORA_RANGE_PKT_IMPLICIT  = 0x80,
} sx1280_lora_range_pkt_len_modes_t;

/*!
 * Packet lengths for LoRa packet type
 */
typedef sx1280_lora_range_pkt_len_modes_t sx1280_lora_pkt_len_modes_t;

/*!
 * Preamble length for LoRa and Ranging packet types
 */
typedef struct sx1280_lora_range_pbl_len_s {
    uint8_t mant;
    uint8_t exp;
} sx1280_lora_range_pbl_len_t;

/*!
 * Preamble length for LoRa packet type
 */
typedef sx1280_lora_range_pbl_len_t sx1280_lora_pbl_len_t;

/*!
 * Packet parameters for LoRa packet type
 */
typedef struct sx1280_pkt_params_lora_s {
    sx1280_lora_pbl_len_t pbl_len_in_symb;
    sx1280_lora_pkt_len_modes_t hdr_type;
    uint8_t pld_len_in_bytes;
    bool crc_is_on;
    bool invert_iq_is_on;
} sx1280_pkt_params_lora_t;

/*!
 * Packet lengths for Ranging packet type
 */
typedef sx1280_lora_range_pkt_len_modes_t sx1280_range_pkt_len_modes_t;

/*!
 * Preamble length for Ranging packet type
 */
typedef sx1280_lora_range_pbl_len_t sx1280_range_pbl_len_t;

/*!
 * Packet parameters for Ranging packet type
 */
typedef struct sx1280_pkt_params_range_s {
    sx1280_range_pbl_len_t pbl_len_in_symb;
    sx1280_range_pkt_len_modes_t hdr_type;
    uint8_t pld_len_in_bytes;
    bool crc_is_on;
    bool invert_iq_is_on;
} sx1280_pkt_params_range_t;

/*!
 * Preamble length values for FLRC packet type
 */
typedef sx1280_gfsk_flrc_pbl_len_t sx1280_flrc_pbl_len_t;

/*!
 * SyncWord lengths for FLRC packet type
 */
typedef enum sx1280_flrc_sync_word_len_e {
    SX1280_FLRC_SYNC_WORD_OFF   = 0x00,
    SX1280_FLRC_SYNC_WORD_ON    = 0x04,
} sx1280_flrc_sync_word_len_t;

/*!
 * Combinations of SyncWord correlators activated for FLRC packet type
 */
typedef sx1280_gfsk_flrc_rx_match_sync_word_t sx1280_flrc_rx_match_sync_word_t;

/*!
 * Packet lengths for FLRC packet type
 */
typedef sx1280_gfsk_flrc_pkt_len_modes_t sx1280_flrc_pkt_len_modes_t;

/*!
 * CRC lengths for FLRC packet types
 *
 * \remark Not all configurations are available for both GFSK and FLRC packet
 *         types. Please refer to the datasheet for possible configuration.
 */
typedef enum sx1280_flrc_crc_types_e {
    SX1280_FLRC_CRC_OFF     = 0x00,
    SX1280_FLRC_CRC_2_BYTES = 0x10,
    SX1280_FLRC_CRC_3_BYTES = 0x20,
    SX1280_FLRC_CRC_4_BYTES = 0x30,
} sx1280_flrc_crc_types_t;

/*!
 * Packet parameters for FLRC packet type
 */
typedef struct sx1280_pkt_params_flrc_s {
    sx1280_flrc_pbl_len_t pbl_len;
    sx1280_flrc_sync_word_len_t sync_word_len;
    sx1280_flrc_rx_match_sync_word_t match_sync_word;
    sx1280_flrc_pkt_len_modes_t hdr_type;
    uint8_t pld_len_in_bytes;
    sx1280_flrc_crc_types_t crc_type;
} sx1280_pkt_params_flrc_t;

/*!
 * Connection states for BLE packet type
 */
typedef enum sx1280_ble_con_states_e {
    SX1280_BLE_PLD_LEN_MAX_31_BYTES     = 0x00,
    SX1280_BLE_PLD_LEN_MAX_37_BYTES     = 0x20,
    SX1280_BLE_TX_TEST_MODE             = 0x40,
    SX1280_BLE_RX_TEST_MODE             = 0x60,
    SX1280_BLE_PLD_LEN_MAX_255_BYTES    = 0x80,
} sx1280_ble_con_states_t;

/*!
 * CRC lengths for BLE packet type
 */
typedef enum sx1280_ble_crc_type_e {
    SX1280_BLE_CRC_OFF  = 0x00,
    SX1280_BLE_CRC_3B   = 0x10,
} sx1280_ble_crc_type_t;

/*!
 * BLE packet types for BLE packet type
 */
typedef enum sx1280_ble_pkt_types_e {
    //!< Pseudo Random Binary Sequence based on 9th degree polynomial
    SX1280_BLE_PKT_TYPE_PRBS_9          = 0x00,
    //!< Pseudo Random Binary Sequence based on 15th degree polynomial
    SX1280_BLE_PKT_TYPE_PRBS_15         = 0x0C,
    //!< Repeated '11110000' sequence
    SX1280_BLE_PKT_TYPE_EYELONG_1_0     = 0x04,
    //!< Repeated '00001111' sequence
    SX1280_BLE_PKT_TYPE_EYELONG_0_1     = 0x18,
    //!< Repeated '10101010' sequence
    SX1280_BLE_PKT_TYPE_EYESHORT_1_0    = 0x08,
    //!< Repeated '01010101' sequence
    SX1280_BLE_PKT_TYPE_EYESHORT_0_1    = 0x1C,
    //!< Repeated '11111111' sequence
    SX1280_BLE_PKT_TYPE_ALL_1           = 0x10,
    //!< Repeated '00000000' sequence
    SX1280_BLE_PKT_TYPE_ALL_0           = 0x14,
} sx1280_ble_pkt_type_t;

/*!
 * Whitening modes for BLE packet type
 */
typedef sx1280_gfsk_flrc_ble_dc_free_t sx1280_ble_dc_free_t;

/*!
 * Packet parameters for BLE packet type
 */
typedef struct sx1280_pkt_params_ble_s {
    sx1280_ble_con_states_t con_state;
    sx1280_ble_crc_type_t crc_type;
    sx1280_ble_pkt_type_t pkt_type;
    sx1280_ble_dc_free_t dc_free;
} sx1280_pkt_params_ble_t;

/*!
 * CAD number of symbols
 */
typedef enum sx1280_lora_cad_symbs_e {
    SX1280_LORA_CAD_01_SYMB = 0x00,
    SX1280_LORA_CAD_02_SYMB = 0x20,
    SX1280_LORA_CAD_04_SYMB = 0x40,
    SX1280_LORA_CAD_08_SYMB = 0x60,
    SX1280_LORA_CAD_16_SYMB = 0x80,
} sx1280_lora_cad_symbs_t;

/*!
 * CAD parameters.
 */
typedef struct sx1280_lora_cad_param_s {
    sx1280_lora_cad_symbs_t cad_symb_nb;
} sx1280_lora_cad_params_t;

/*!
 * Chip mode status
 */
#define SX1280_CHIP_MODES_POS 5
#define SX1280_CHIP_MODES_MASK 0x07
typedef enum sx1280_chip_modes_e {
    SX1280_CHIP_MODE_STBY_RC    = 2,
    SX1280_CHIP_MODE_STBY_XOSC  = 3,
    SX1280_CHIP_MODE_FS         = 4,
    SX1280_CHIP_MODE_RX         = 5,
    SX1280_CHIP_MODE_TX         = 6,
} sx1280_chip_modes_t;

/*!
 * Chip command status
 */
#define SX1280_CMD_STATUS_POS 2
#define SX1280_CMD_STATUS_MASK 0x07
typedef enum sx1280_cmd_status_e {
    SX1280_CMD_STATUS_CMD_OK            = 1,
    SX1280_CMD_STATUS_DATA_AVAILABLE    = 2,
    SX1280_CMD_STATUS_CMD_TIMEOUT       = 3,
    SX1280_CMD_STATUS_CMD_PROCESS_ERROR = 4,
    SX1280_CMD_STATUS_CMD_EXEC_FAILURE  = 5,
    SX1280_CMD_STATUS_CMD_TX_DONE       = 6,
} sx1280_cmd_status_t;

/*!
 * Chip status parameters
 */
typedef struct sx1280_chip_status_s {
    sx1280_cmd_status_t cmd_status;
    sx1280_chip_modes_t chip_mode;
} sx1280_chip_status_t;

/*!
 * Rx buffer status parameters for GFSK, LoRa, FLRC and BLE packet types
 */
typedef struct sx1280_rx_buffer_status_s {
    uint8_t pld_len_in_bytes;
    uint8_t buffer_start_pointer;
} sx1280_rx_buffer_status_t;

enum sx1280_pkt_status_errors_e {
    SX1280_PKT_STATUS_ERROR_PKT_CTRL_BUSY   = (1 << 0),
    SX1280_PKT_STATUS_ERROR_PKT_RX          = (1 << 1),
    SX1280_PKT_STATUS_ERROR_HDR_RX          = (1 << 2),
    SX1280_PKT_STATUS_ERROR_TX_RX_ABORTED   = (1 << 3),
    SX1280_PKT_STATUS_ERROR_PKT_CRC         = (1 << 4),
    SX1280_PKT_STATUS_ERROR_PKT_LEN         = (1 << 5),
    SX1280_PKT_STATUS_ERROR_PKT_SYNC        = (1 << 6),
};

typedef uint8_t sx1280_pkt_status_errors_t;

enum sx1280_pkt_status_e {
    SX1280_PKT_STATUS_PKT_SENT  = (1 << 0),
    SX1280_PKT_STATUS_RX_NO_ACK = (1 << 5),
};

typedef uint8_t sx1280_pkt_status_t;

enum sx1280_pkt_status_sync_e {
    SX1280_PKT_STATUS_SYNC_ADDR_1   = (1 << 0),
    SX1280_PKT_STATUS_SYNC_ADDR_2   = (1 << 1),
    SX1280_PKT_STATUS_SYNC_ADDR_3   = (1 << 2),
};

typedef uint8_t sx1280_pkt_status_sync_t;

/*!
 * Packet status parameters for GFSK, FLRC and BLE packet types
 */
typedef struct sx1280_pkt_status_gfsk_flrc_ble_s {
    int8_t rssi;
    sx1280_pkt_status_errors_t errors;
    sx1280_pkt_status_t status;
    sx1280_pkt_status_sync_t sync;
} sx1280_pkt_status_gfsk_flrc_ble_t;

/*!
 * Packet status parameters for GFSK packet type
 */
typedef sx1280_pkt_status_gfsk_flrc_ble_t sx1280_pkt_status_gfsk_t;

/*!
 * Packet status parameters for FLRC packet type
 */
typedef sx1280_pkt_status_gfsk_flrc_ble_t sx1280_pkt_status_flrc_t;

/*!
 * Packet status parameters for BLE packet type
 */
typedef sx1280_pkt_status_gfsk_flrc_ble_t sx1280_pkt_status_ble_t;

/*!
 * Packet status for LoRa and Ranging packet types
 */
typedef struct sx1280_pkt_status_lora_range_s {
    int8_t rssi;
    int8_t snr;
} sx1280_pkt_status_lora_range_t;

/*!
 * Packet status for LoRa packet types
 */
typedef sx1280_pkt_status_lora_range_t sx1280_pkt_status_lora_t;

/*!
 * Packet status for Ranging packet types
 */
typedef sx1280_pkt_status_lora_range_t sx1280_pkt_status_range_t;

/*!
 * Ranging roles for ranging packet type
 */
typedef enum sx1280_range_role_e {
    SX1280_RANGE_ROLE_SLV   = 0,
    SX1280_RANGE_ROLE_MST   = 1,
} sx1280_range_role_t;

/*!
 * Selector values to configure LNA regime
 */
typedef enum sx1280_lna_settings_e {
    //!< Allow maximum efficiency of sx128x (default)
    SX1280_LNA_LOW_POWER_MODE,
    //!< Allow to use highest three steps of LNA gain and increase current
    //!< consumption
    SX1280_LNA_HIGH_SENSITIVITY_MODE,
} sx1280_lna_settings_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

//
// Operational Modes Functions
//

/*!
 * Get radio operating mode
 *
 * \param [in] context Radio implementation parameters
 * \retval     op_mode Radio current operating mode
 */
sx1280_operating_mode_t sx1280_get_operating_mode( const void *context );

sx1280_status_t sx1280_set_sleep( const void *context, const sx1280_sleep_cfg_t cfg );

sx1280_status_t sx1280_set_standby( const void *context, const sx1280_standby_cfg_t cfg );

sx1280_status_t sx1280_wakeup( const void *context );

sx1280_status_t sx1280_set_fs( const void *context );

sx1280_status_t sx1280_set_tx( const void *context, sx1280_tick_size_t period_base,
                               const uint16_t timeout_in_ticks );

sx1280_status_t sx1280_set_rx( const void *context, sx1280_tick_size_t period_base,
                               const uint16_t timeout_in_ticks );

sx1280_status_t sx1280_set_rx_duty_cycle( const void *context, sx1280_tick_size_t period_base,
                                          const uint16_t rx_time,
                                          const uint16_t sleep_time );

sx1280_status_t sx1280_set_cad( const void *context );

sx1280_status_t sx1280_set_auto_tx( const void *context, uint16_t time_in_us );

sx1280_status_t sx1280_set_auto_fs( const void *context, bool state );

sx1280_status_t sx1280_set_tx_cw( const void *context );

sx1280_status_t sx1280_set_tx_cpbl( const void *context );

//
// Registers and Buffer Access
//

sx1280_status_t sx1280_write_register( const void *context, const uint16_t addr,
                                       const uint8_t *buffer,
                                       const uint8_t size );

sx1280_status_t sx1280_read_register( const void *context, const uint16_t addr, uint8_t *buffer,
                                      const uint8_t size );

sx1280_status_t sx1280_write_buffer( const void *context, const uint8_t offset,
                                     const uint8_t *buffer,
                                     const uint8_t size );

sx1280_status_t sx1280_read_buffer( const void *context, const uint8_t offset, uint8_t *buffer,
                                    const uint8_t size );

//
// DIO and IRQ Control Functions
//

sx1280_status_t sx1280_set_dio_irq_params( const void *context, const uint16_t irq_mask,
                                           const uint16_t dio1_mask,
                                           const uint16_t dio2_mask, const uint16_t dio3_mask );
sx1280_status_t sx1280_get_irq_status( const void *context, sx1280_irq_mask_t *irq );

sx1280_status_t sx1280_clear_irq_status( const void *context, const sx1280_irq_mask_t irq_mask );

/*!
 * \brief Clears any radio irq status flags that are set and returns the flags that
 * were cleared.
 *
 * \param [in] context Chip implementation context.
 * \param [out] irq Pointer to a variable for holding the system interrupt status. Can be NULL.
 *
 * \returns Operation status
 */
sx1280_status_t sx1280_get_and_clear_irq_status( const void *context, sx1280_irq_mask_t *irq );

//
// RF Modulation and Packet-Related Functions
//

sx1280_status_t sx1280_set_rf_freq( const void *context, const uint32_t freq_in_hz );

sx1280_status_t sx1280_set_pkt_type( const void *context, const sx1280_pkt_type_t pkt_type );

sx1280_status_t sx1280_get_pkt_type( const void *context, sx1280_pkt_type_t *pkt_type );

sx1280_status_t sx1280_set_tx_params( const void *context, const int8_t pwr,
                                      const sx1280_ramp_time_t ramp_time );

sx1280_status_t sx1280_set_gfsk_mod_params( const void *context,
                                            const sx1280_mod_params_gfsk_t *params );

sx1280_status_t sx1280_set_lora_mod_params( const void *context,
                                            const sx1280_mod_params_lora_t *params );

sx1280_status_t sx1280_set_range_mod_params( const void *context,
                                             const sx1280_mod_params_range_t *params );

sx1280_status_t sx1280_set_flrc_mod_params( const void *context,
                                            const sx1280_mod_params_flrc_t *params );

sx1280_status_t sx1280_set_ble_mod_params( const void *context,
                                           const sx1280_mod_params_ble_t *params );

sx1280_status_t sx1280_set_gfsk_pkt_params( const void *context,
                                            const sx1280_pkt_params_gfsk_t *params );

sx1280_status_t sx1280_set_lora_pkt_params( const void *context,
                                            const sx1280_pkt_params_lora_t *params );

sx1280_status_t sx1280_set_range_pkt_params( const void *context,
                                             const sx1280_pkt_params_range_t *params );

sx1280_status_t sx1280_set_flrc_pkt_params( const void *context,
                                            const sx1280_pkt_params_flrc_t *params );

sx1280_status_t sx1280_set_ble_pkt_params( const void *context,
                                           const sx1280_pkt_params_ble_t *params );

sx1280_status_t sx1280_set_cad_params( const void *context,
                                       const sx1280_lora_cad_params_t *params );

sx1280_status_t sx1280_set_buffer_base_addr( const void *context, const uint8_t tx_base_addr,
                                             const uint8_t rx_base_addr );

//
// Communication Status Information
//

sx1280_status_t sx1280_get_status( const void *context, sx1280_chip_status_t *radio_status );

sx1280_status_t sx1280_get_rx_buffer_status( const void *context,
                                             sx1280_rx_buffer_status_t *rx_buffer_status );

sx1280_status_t sx1280_get_gfsk_pkt_status( const void *context,
                                            sx1280_pkt_status_gfsk_t *pkt_status );

sx1280_status_t sx1280_get_lora_pkt_status( const void *context,
                                            sx1280_pkt_status_lora_t *pkt_status );

sx1280_status_t sx1280_get_range_pkt_status( const void *context,
                                             sx1280_pkt_status_range_t *pkt_status );

sx1280_status_t sx1280_get_flrc_pkt_status( const void *context,
                                            sx1280_pkt_status_flrc_t *pkt_status );

sx1280_status_t sx1280_get_ble_pkt_status( const void *context,
                                           sx1280_pkt_status_ble_t *pkt_status );

sx1280_status_t sx1280_get_rssi_inst( const void *context, int16_t *rssi );

//
// Miscellaneous
//

sx1280_status_t sx1280_reset( const void *context );

sx1280_status_t sx1280_process_irq( const void *context, sx1280_irq_mask_t *irq_status );

sx1280_status_t sx1280_set_long_pbl( const void *context, const bool state );

sx1280_status_t sx1280_set_reg_mode( const void *context, const sx1280_reg_mod_t mode );

sx1280_status_t sx1280_set_lna_settings( const void *context, sx1280_lna_settings_t settings );

sx1280_status_t sx1280_save_context( const void *context );

sx1280_status_t sx1280_set_ranging_role( const void *context, const sx1280_range_role_t role );

sx1280_status_t sx1280_set_adv_ranging( const void *context, const bool state );

sx1280_status_t sx1280_set_gfsk_flrc_sync_word_tolerance( const void *context, uint8_t tolerance );

sx1280_status_t sx1280_get_gfsk_br_bw_param( const uint32_t br, const uint32_t bw,
                                             sx1280_gfsk_ble_br_bw_t *param );

sx1280_status_t sx1280_get_gfsk_mod_ind_param( const uint32_t br, const uint32_t fdev,
                                               sx1280_gfsk_ble_mod_ind_t *param );

uint32_t sx1280_get_bitrate_in_hz_gfsk( sx1280_gfsk_br_bw_t br_bw );

sx1280_status_t sx1280_get_flrc_br_bw_param( const uint32_t br, const uint32_t bw,
                                             sx1280_flrc_br_bw_t *param );

uint32_t sx1280_get_gfsk_time_on_air_in_ms( const sx1280_pkt_params_gfsk_t *pkt_params,
                                            const sx1280_mod_params_gfsk_t *mod_params );

uint32_t sx1280_get_lora_time_on_air_in_ms( const sx1280_pkt_params_lora_t *pkt_params,
                                            const sx1280_mod_params_lora_t *mod_params );

uint32_t sx1280_get_flrc_time_on_air_in_ms( const sx1280_pkt_params_flrc_t *pkt_p,
                                            const sx1280_mod_params_flrc_t *mod_p );

//
// Registers access
//

sx1280_status_t sx1280_set_gfsk_sync_word( const void *context, const uint8_t sync_word_id,
                                           const uint8_t *sync_word,
                                           const uint8_t sync_word_len );

sx1280_status_t sx1280_set_lora_sync_word( const void *context, const uint8_t sync_word );

sx1280_status_t sx1280_set_flrc_sync_word( const void *context, const uint8_t sync_word_id,
                                           const uint8_t *sync_word,
                                           const uint8_t sync_word_len );

sx1280_status_t sx1280_set_gfsk_crc_seed( const void *context, uint16_t seed );

sx1280_status_t sx1280_set_flrc_crc_seed( const void *context, uint32_t seed );

sx1280_status_t sx1280_set_gfsk_crc_polynomial( const void *context, uint16_t polynomial );

sx1280_status_t sx1280_set_gfsk_whitening_seed( const void *context, uint16_t seed );

sx1280_status_t sx1280_get_lora_pkt_len_mode( const void *context,
                                              sx1280_lora_pkt_len_modes_t *hdr_type );

sx1280_status_t sx1280_get_lora_pkt_len( const void *context, uint8_t *pkt_len );

sx1280_status_t sx1280_get_lora_rx_packet_cr( const void *context,
                                              sx1280_lora_range_cr_t *lora_incoming_cr );

sx1280_status_t sx1280_get_lora_rx_packet_crc_config( const void *context,
                                                      bool *is_lora_crc_enabled );

#ifdef __cplusplus
}
#endif

#endif  // __SX1280_H__

/* --- EOF ------------------------------------------------------------------ */
