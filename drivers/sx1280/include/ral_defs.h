/**
 * @file      ral_defs.h
 *
 * @brief     Radio Abstraction Layer (RAL) API types definition
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
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __RAL_DEFS_H__
#define __RAL_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

typedef enum ral_radio_types_e {
    RAL_RADIO_UNKNOWN,
    RAL_RADIO_SX1272,
    RAL_RADIO_SX1276,
    RAL_RADIO_SX126X,
    RAL_RADIO_SX1280,
} ral_radio_types_t;

typedef enum ral_tcxo_ctrl_modes_e {
    RAL_TCXO_NONE,              //! No TCXO connected
    RAL_TCXO_CTRL_RADIO,        //! TCXO fully controlled by the radio chip
    RAL_TCXO_CTRL_HOST_EXT,     //! TCXO fully controlled by the host
} ral_tcxo_ctrl_modes_t;

typedef enum ral_tcxo_ctrl_voltages_e {
    RAL_TCXO_CTRL_1_6V,
    RAL_TCXO_CTRL_1_7V,
    RAL_TCXO_CTRL_1_8V,
    RAL_TCXO_CTRL_2_2V,
    RAL_TCXO_CTRL_2_4V,
    RAL_TCXO_CTRL_2_7V,
    RAL_TCXO_CTRL_3_0V,
    RAL_TCXO_CTRL_3_3V,
} ral_tcxo_ctrl_voltages_t;

typedef struct ral_tcxo_conf_s {
    ral_tcxo_ctrl_modes_t tcxo_ctrl_mode;
    ral_tcxo_ctrl_voltages_t tcxo_ctrl_voltage;
    uint16_t tcxo_startup_time_ms;
} ral_tcxo_cfg_t;

typedef struct ral_s {
    const void *context;
    const ral_radio_types_t radio_type;
    const ral_tcxo_cfg_t tcxo_cfg;
} ral_t;

/**
 * API return status
 */
typedef enum ral_status_e {
    RAL_STATUS_OK = 0,
    RAL_STATUS_UNSUPPORTED_FEATURE,
    RAL_STATUS_UNKNOWN_VALUE,
    RAL_STATUS_ERROR,
} ral_status_t;

typedef enum ral_gfsk_crc_type_e {
    RAL_GFSK_CRC_OFF,
    RAL_GFSK_CRC_1_BYTE,
    RAL_GFSK_CRC_2_BYTES,
    RAL_GFSK_CRC_1_BYTE_INV,
    RAL_GFSK_CRC_2_BYTES_INV,
} ral_gfsk_crc_type_t;

typedef enum ral_gfsk_mod_shape_e {
    RAL_GFSK_MOD_SHAPE_OFF,
    RAL_GFSK_MOD_SHAPE_BT_03,
    RAL_GFSK_MOD_SHAPE_BT_05,
    RAL_GFSK_MOD_SHAPE_BT_07,
    RAL_GFSK_MOD_SHAPE_BT_1,
} ral_gfsk_mod_shape_t;

typedef struct ral_params_gfsk_s {
    uint32_t freq_in_hz;
    uint32_t br_in_bps;
    uint16_t pbl_len_in_bytes;
    uint8_t sync_word_len_in_bytes;
    const uint8_t *sync_word;
    bool pld_is_fix;
    uint8_t pld_len_in_bytes;
    ral_gfsk_crc_type_t crc_type;
    bool dc_free_is_on;
    uint16_t crc_seed;
    uint16_t crc_polynomial;
    uint16_t whitening_seed;
    ral_gfsk_mod_shape_t pulse_shape;
    uint32_t bw_ssb_in_hz;              //! Rx parameters
    int8_t pwr_in_dbm;                  //! Tx parameters
    uint32_t fdev_in_hz;                //! Tx parameters
} ral_params_gfsk_t;

typedef enum ral_lora_sf_e {
    RAL_LORA_SF5 = 5,   // SX126x, SX1280
    RAL_LORA_SF6,       // SX126x, SX1280
    RAL_LORA_SF7,       // All radios
    RAL_LORA_SF8,       // All radios
    RAL_LORA_SF9,       // All radios
    RAL_LORA_SF10,      // All radios
    RAL_LORA_SF11,      // All radios
    RAL_LORA_SF12,      // All radios
} ral_lora_sf_t;

typedef enum ral_lora_bw_e {
    RAL_LORA_BW_007_KHZ = 0,    // SX126x, SX1276 only
    RAL_LORA_BW_010_KHZ,        // SX126x, SX1276 only
    RAL_LORA_BW_015_KHZ,        // SX126x, SX1276 only
    RAL_LORA_BW_020_KHZ,        // SX126x, SX1276 only
    RAL_LORA_BW_031_KHZ,        // SX126x, SX1276 only
    RAL_LORA_BW_041_KHZ,        // SX126x, SX1276 only
    RAL_LORA_BW_062_KHZ,        // SX126x, SX1276 only
    RAL_LORA_BW_125_KHZ,        // All except SX1280
    RAL_LORA_BW_200_KHZ,        // SX1280 only
    RAL_LORA_BW_250_KHZ,        // All except SX1280
    RAL_LORA_BW_400_KHZ,        // SX1280 only
    RAL_LORA_BW_500_KHZ,        // All except SX1280
    RAL_LORA_BW_800_KHZ,        // SX1280 only
    RAL_LORA_BW_1600_KHZ,       // SX1280 only
} ral_lora_bw_t;

typedef enum ral_lora_cr_e {
    RAL_LORA_CR_4_5 = 0,
    RAL_LORA_CR_4_6,
    RAL_LORA_CR_4_7,
    RAL_LORA_CR_4_8,
    RAL_LORA_CR_LI_4_5,     // SX1280 only
    RAL_LORA_CR_LI_4_6,     // SX1280 only
    // RAL_LORA_CR_LI_4_7,  // No radio support available
    RAL_LORA_CR_LI_4_8,     // SX1280 only
} ral_lora_cr_t;

typedef struct ral_params_lora_s {
    uint32_t freq_in_hz;
    ral_lora_sf_t sf;
    ral_lora_bw_t bw;
    ral_lora_cr_t cr;
    uint16_t pbl_len_in_symb;
    uint8_t sync_word;
    bool pld_is_fix;
    uint8_t pld_len_in_bytes;
    bool crc_is_on;
    bool invert_iq_is_on;
    uint8_t symb_nb_timeout;        //! Rx only parameters
    int8_t pwr_in_dbm;              //! Tx only parameters
} ral_params_lora_t;

typedef enum ral_flrc_cr_e {
    RAL_FLRC_CR_1_2 = 0,
    RAL_FLRC_CR_3_4,
    RAL_FLRC_CR_1_0,
} ral_flrc_cr_t;

typedef enum ral_flrc_crc_type_e {
    RAL_FLRC_CRC_OFF = 0,
    RAL_FLRC_CRC_2_BYTES,
    RAL_FLRC_CRC_3_BYTES,
    RAL_FLRC_CRC_4_BYTES
} ral_flrc_crc_type_t;

typedef enum ral_flrc_mod_shape_e {
    RAL_FLRC_MOD_SHAPE_OFF,
    RAL_FLRC_MOD_SHAPE_BT_05,
    RAL_FLRC_MOD_SHAPE_BT_1,
} ral_flrc_mod_shape_t;

typedef struct ral_params_flrc_s {
    uint32_t freq_in_hz;
    uint32_t br_in_bps;
    ral_flrc_cr_t cr;
    uint16_t pbl_len_in_bytes;
    bool sync_word_is_on;
    const uint8_t *sync_word;
    bool pld_is_fix;
    uint8_t pld_len_in_bytes;
    ral_flrc_crc_type_t crc_type;
    uint32_t crc_seed;
    ral_flrc_mod_shape_t mod_shape;
    uint32_t bw_ssb_in_hz;              //! Rx only parameters
    int8_t pwr_in_dbm;                  //! Tx only parameters
} ral_params_flrc_t;

typedef struct ral_params_lora_e_s {
    void *todo;
} ral_params_lora_e_t;

typedef struct ral_params_bpsk_s {
    void *todo;
} ral_params_bpsk_t;

typedef enum ral_pkt_types_e {
    RAL_PKT_TYPE_GFSK   = 0x00,
    RAL_PKT_TYPE_LORA   = 0x01,
    RAL_PKT_TYPE_FLRC   = 0x02,
    RAL_PKT_TYPE_LORA_E = 0x03,
    RAL_PKT_TYPE_BPSK   = 0x04,
    RAL_PKT_TYPE_NONE   = 0x0F,
} ral_pkt_type_t;

typedef struct ral_rx_pkt_status_gfsk_s {
    uint8_t rx_status;
    int16_t rssi_sync_in_dbm;
    int16_t rssi_avg_in_dbm;
} ral_rx_pkt_status_gfsk_t;

typedef struct ral_rx_pkt_status_lora_s {
    int16_t rssi_pkt_in_dbm;
    int16_t snr_pkt_in_db;
    int16_t signal_rssi_pkt_in_db;  // Last packet received LoRa signal RSSI
                                    // power in dB estimation
                                    // (done after despreading)
} ral_rx_pkt_status_lora_t;

typedef struct ral_rx_pkt_status_flrc_s {
    int16_t rssi_in_dbm;
} ral_rx_pkt_status_flrc_t;

/*!
 *  Represents the number of symbs to be used for channel activity
 * detection operation
 */
typedef enum ral_lora_cad_symbs_e {
    RAL_LORA_CAD_01_SYMB    = 0x00,
    RAL_LORA_CAD_02_SYMB    = 0x01,
    RAL_LORA_CAD_04_SYMB    = 0x02,
    RAL_LORA_CAD_08_SYMB    = 0x03,
    RAL_LORA_CAD_16_SYMB    = 0x04,
} ral_lora_cad_symbs_t;

/*!
 *  Represents the Channel Activity Detection actions after the CAD
 * operation is finished
 */
typedef enum ral_lora_cad_exit_modes_e {
    RAL_LORA_CAD_ONLY   = 0x00,
    RAL_LORA_CAD_RX     = 0x01,
    RAL_LORA_CAD_LBT    = 0x10,
} ral_lora_cad_exit_modes_t;

/*!
 * Defines \ref ral_set_cad_params function parameters.
 */
typedef struct ral_lora_cad_param_s {
    ral_lora_cad_symbs_t cad_symb_nb;
    uint8_t cad_det_peak_in_symb;
    uint8_t cad_det_min_in_symb;
    ral_lora_cad_exit_modes_t cad_exit_mode;
    uint32_t cad_timeout_in_ms;
} ral_lora_cad_params_t;

/*
 * IRQ definitions
 */
enum ral_irq_e {
    RAL_IRQ_NONE            = (0 << 0),
    RAL_IRQ_TX_DONE         = (1 << 1),
    RAL_IRQ_RX_DONE         = (1 << 2),
    RAL_IRQ_RX_TIMEOUT      = (1 << 3),
    RAL_IRQ_RX_HDR_OK       = (1 << 4),
    RAL_IRQ_RX_HDR_ERROR    = (1 << 5),
    RAL_IRQ_RX_CRC_ERROR    = (1 << 6),
    RAL_IRQ_CAD_DONE        = (1 << 7),
    RAL_IRQ_CAD_OK          = (1 << 8),
    RAL_IRQ_ALL             = RAL_IRQ_TX_DONE | RAL_IRQ_RX_DONE | RAL_IRQ_RX_TIMEOUT |
                              RAL_IRQ_RX_HDR_OK | RAL_IRQ_RX_HDR_ERROR |
                              RAL_IRQ_RX_CRC_ERROR | RAL_IRQ_CAD_DONE | RAL_IRQ_CAD_OK,
};

typedef uint16_t ral_irq_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

static inline uint8_t ral_compute_lora_ldro( const ral_lora_sf_t sf, const ral_lora_bw_t bw )
{
    // Since the SX1280 chip manages the LDRO configuration by itself, all
    // bandwidth related to it are not handled in the following switch
    // implementation.

    switch (bw) {
    case RAL_LORA_BW_500_KHZ:
        return 0;
    case RAL_LORA_BW_250_KHZ:
        if (sf == RAL_LORA_SF12) {
            return 1;
        }
        else {
            return 0;
        }
    case RAL_LORA_BW_125_KHZ:
        if ((sf == RAL_LORA_SF12) || (sf == RAL_LORA_SF11)) {
            return 1;
        }
        else {
            return 0;
        }
    case RAL_LORA_BW_062_KHZ:
        if ((sf == RAL_LORA_SF12) || (sf == RAL_LORA_SF11) || (sf == RAL_LORA_SF10)) {
            return 1;
        }
        else {
            return 0;
        }
    case RAL_LORA_BW_041_KHZ:
        if ((sf == RAL_LORA_SF12) || (sf == RAL_LORA_SF11) || (sf == RAL_LORA_SF10) ||
            (sf == RAL_LORA_SF9)) {
            return 1;
        }
        else {
            return 0;
        }
    case RAL_LORA_BW_031_KHZ:
    case RAL_LORA_BW_020_KHZ:
    case RAL_LORA_BW_015_KHZ:
    case RAL_LORA_BW_010_KHZ:
    case RAL_LORA_BW_007_KHZ:
        return 1;
    default:
        return 0;
    }
}

#ifdef __cplusplus
}
#endif

#endif  // __RAL_DEFS_H__

/* --- EOF ------------------------------------------------------------------ */
