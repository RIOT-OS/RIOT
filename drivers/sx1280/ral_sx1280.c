/**
 * @file      ral_sx1280.c
 *
 * @brief     SX1280 radio abstraction layer implementation
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

/*
 * Porting to RIOT
 *
 * @author      Nicolas Albarel
 * @author      Didier Donsez
 * @author      Olivier Alphand
 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <string.h>  // memcpy

#include "sx1280.h"
#include "ral_sx1280.h"

// GCC bug workaround
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

static const uint32_t ral_to_sx1280_lora_tx_dbm_to_ua[] = {
    10000,  // -18 dBm
    10000,  // -17 dBm
    10000,  // -16 dBm
    10000,  // -15 dBm
    10000,  // -14 dBm
    10000,  // -13 dBm
    10000,  // -12 dBm
    10000,  // -11 dBm
    10000,  // -10 dBm
    10000,  //  -9 dBm
    10000,  //  -8 dBm
    10000,  //  -7 dBm
    10000,  //  -6 dBm
    10000,  //  -5 dBm
    10000,  //  -4 dBm
    10000,  //  -3 dBm
    10000,  //  -2 dBm
    10000,  //  -1 dBm
    10000,  //   0 dBm
    10000,  //   1 dBm
    11000,  //   2 dBm
    11000,  //   3 dBm
    12000,  //   4 dBm
    12000,  //   5 dBm
    14000,  //   6 dBm
    14000,  //   7 dBm
    16000,  //   8 dBm
    16000,  //   9 dBm
    18000,  //  10 dBm
    18000,  //  11 dBm
    24000,  //  12 dBm
    24000,  //  13 dBm
};

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

static ral_status_t ral_sx1280_convert_gfsk_params_from_radio( const ral_params_gfsk_t *params,
                                                               sx1280_mod_params_gfsk_t *mod_params,
                                                               sx1280_pkt_params_gfsk_t *pkt_params );

static ral_status_t ral_sx1280_convert_lora_params_from_radio( const ral_params_lora_t *params,
                                                               sx1280_mod_params_lora_t *mod_params,
                                                               sx1280_pkt_params_lora_t *pkt_params );

static ral_status_t ral_sx1280_convert_flrc_params_from_radio( const ral_params_flrc_t *params,
                                                               sx1280_mod_params_flrc_t *mod_params,
                                                               sx1280_pkt_params_flrc_t *pkt_params );

static sx1280_lora_range_pbl_len_t sx1280_convert_pbl_len_from_symb(
    const uint16_t pbl_len_in_symb );

static ral_irq_t ral_sx1280_convert_irq_flags_to_radio( sx1280_irq_mask_t sx1280_irq );

static sx1280_irq_mask_t ral_sx1280_convert_irq_flags_from_radio( ral_irq_t ral_irq );

static uint8_t shift_and_count_trailing_zeros( uint16_t *x );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

ral_status_t ral_sx1280_init( const ral_t *ral )
{
    ral_status_t status = RAL_STATUS_ERROR;

    sx1280_reset( ral->context );

    status = ( ral_status_t )sx1280_set_reg_mode( ral->context, SX1280_REG_MODE_DCDC );
    return status;
}

ral_status_t ral_sx1280_setup_gfsk( const ral_t *ral, const ral_params_gfsk_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_mod_params_gfsk_t mod_params = { 0 };
    sx1280_pkt_params_gfsk_t pkt_params = { 0 };

    status = ral_sx1280_convert_gfsk_params_from_radio( params, &mod_params, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_standby( ral->context, SX1280_STANDBY_CFG_RC );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_pkt_type( ral->context, SX1280_PKT_TYPE_GFSK );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_rf_freq( ral->context, params->freq_in_hz );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_tx_params( ral->context, params->pwr_in_dbm,
                                                   SX1280_RAMP_20_US );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_buffer_base_addr( ral->context, 0x00, 0x00 );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_gfsk_mod_params( ral->context, &mod_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_gfsk_pkt_params( ral->context, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    if (params->crc_type != RAL_GFSK_CRC_OFF) {
        status = ( ral_status_t )sx1280_set_gfsk_crc_seed( ral->context, params->crc_seed );
        if (status != RAL_STATUS_OK) {
            return status;
        }

        status = ( ral_status_t )sx1280_set_gfsk_crc_polynomial( ral->context,
                                                                 params->crc_polynomial );
        if (status != RAL_STATUS_OK) {
            return status;
        }
    }

    status = ( ral_status_t )sx1280_set_gfsk_sync_word( ral->context, 1, params->sync_word,
                                                        params->sync_word_len_in_bytes );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    if (params->dc_free_is_on == true) {
        status = ( ral_status_t )sx1280_set_gfsk_whitening_seed( ral->context,
                                                                 params->whitening_seed );
    }

    return status;
}

ral_status_t ral_sx1280_setup_rx_gfsk( const ral_t *ral, const ral_params_gfsk_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    // Force unused parameter to a known value.
    ral_params_gfsk_t local_params = *params;

    local_params.pwr_in_dbm = 0;
    local_params.fdev_in_hz = 0;

    status = ral_sx1280_setup_gfsk( ral, &local_params );
    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params(
            ral->context,
            SX1280_IRQ_RX_DONE | SX1280_IRQ_SYNC_WORD_ERROR | SX1280_IRQ_CRC_ERROR | SX1280_IRQ_TIMEOUT,
            SX1280_IRQ_RX_DONE | SX1280_IRQ_SYNC_WORD_ERROR | SX1280_IRQ_CRC_ERROR | SX1280_IRQ_TIMEOUT,
            SX1280_IRQ_NONE, SX1280_IRQ_NONE );
        if (status != RAL_STATUS_OK) {
            return status;
        }
    }
    return status;
}

ral_status_t ral_sx1280_setup_tx_gfsk( const ral_t *ral, const ral_params_gfsk_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    // Force unused parameter to a known value.
    ral_params_gfsk_t local_params = *params;

    local_params.bw_ssb_in_hz = 0;

    status = ral_sx1280_setup_gfsk( ral, &local_params );
    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params( ral->context, SX1280_IRQ_TX_DONE,
                                                            SX1280_IRQ_TX_DONE,
                                                            SX1280_IRQ_NONE, SX1280_IRQ_NONE );
    }
    return status;
}

ral_status_t ral_sx1280_setup_lora( const ral_t *ral, const ral_params_lora_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_mod_params_lora_t mod_params = { 0 };
    sx1280_pkt_params_lora_t pkt_params = { 0 };

    status = ral_sx1280_convert_lora_params_from_radio( params, &mod_params, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_standby( ral->context, SX1280_STANDBY_CFG_RC );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_pkt_type( ral->context, SX1280_PKT_TYPE_LORA );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_rf_freq( ral->context, params->freq_in_hz );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    int8_t pwr_in_dbm_clipped = params->pwr_in_dbm;

    if (params->pwr_in_dbm > SX1280_PWR_MAX) {
        pwr_in_dbm_clipped = SX1280_PWR_MAX;
    }
    else if (params->pwr_in_dbm < SX1280_PWR_MIN) {
        pwr_in_dbm_clipped = SX1280_PWR_MIN;
    }
    status = ( ral_status_t )sx1280_set_tx_params( ral->context, pwr_in_dbm_clipped,
                                                   SX1280_RAMP_20_US );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_buffer_base_addr( ral->context, 0x00, 0x00 );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_lora_mod_params( ral->context, &mod_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_lora_pkt_params( ral->context, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_lora_sync_word( ral->context, params->sync_word );

    return status;
}

ral_status_t ral_sx1280_setup_rx_lora( const ral_t *ral, const ral_params_lora_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    // Force unused parameter to a known value.
    ral_params_lora_t local_params = *params;

    local_params.pwr_in_dbm = 0;

    status = ral_sx1280_setup_lora( ral, &local_params );
    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params(
            ral->context,
            SX1280_IRQ_RX_DONE | SX1280_IRQ_HEADER_ERROR | SX1280_IRQ_CRC_ERROR | SX1280_IRQ_TIMEOUT,
            SX1280_IRQ_RX_DONE | SX1280_IRQ_HEADER_ERROR | SX1280_IRQ_CRC_ERROR | SX1280_IRQ_TIMEOUT, SX1280_IRQ_NONE,
            SX1280_IRQ_NONE );
        if (status != RAL_STATUS_OK) {
            return status;
        }
    }
    return status;
}

ral_status_t ral_sx1280_setup_tx_lora( const ral_t *ral, const ral_params_lora_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    // Force unused parameter to a known value.
    ral_params_lora_t local_params = *params;

    local_params.symb_nb_timeout = 0;

    status = ral_sx1280_setup_lora( ral, &local_params );
    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params( ral->context, SX1280_IRQ_TX_DONE,
                                                            SX1280_IRQ_TX_DONE,
                                                            SX1280_IRQ_NONE, SX1280_IRQ_NONE );
    }
    return status;
}

ral_status_t ral_sx1280_setup_flrc( const ral_t *ral, const ral_params_flrc_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_mod_params_flrc_t mod_params = { 0 };
    sx1280_pkt_params_flrc_t pkt_params = { 0 };

    status = ral_sx1280_convert_flrc_params_from_radio( params, &mod_params, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_standby( ral->context, SX1280_STANDBY_CFG_RC );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_pkt_type( ral->context, SX1280_PKT_TYPE_FLRC );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_rf_freq( ral->context, params->freq_in_hz );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_tx_params( ral->context, params->pwr_in_dbm,
                                                   SX1280_RAMP_20_US );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_buffer_base_addr( ral->context, 0x00, 0x00 );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_flrc_mod_params( ral->context, &mod_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_flrc_pkt_params( ral->context, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_flrc_crc_seed( ral->context, params->crc_seed );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    status = ( ral_status_t )sx1280_set_flrc_sync_word( ral->context, 1, params->sync_word,
                                                        pkt_params.sync_word_len );

    return status;
}

ral_status_t ral_sx1280_setup_rx_flrc( const ral_t *ral, const ral_params_flrc_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    // Force unused parameter to a known value.
    ral_params_flrc_t local_params = *params;

    local_params.pwr_in_dbm = 0;

    status = ral_sx1280_setup_flrc( ral, &local_params );
    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params(
            ral->context,
            SX1280_IRQ_RX_DONE | SX1280_IRQ_SYNC_WORD_ERROR | SX1280_IRQ_CRC_ERROR | SX1280_IRQ_TIMEOUT,
            SX1280_IRQ_RX_DONE | SX1280_IRQ_SYNC_WORD_ERROR | SX1280_IRQ_CRC_ERROR | SX1280_IRQ_TIMEOUT,
            SX1280_IRQ_NONE, SX1280_IRQ_NONE );
        if (status != RAL_STATUS_OK) {
            return status;
        }
    }
    return status;
}

ral_status_t ral_sx1280_setup_tx_flrc( const ral_t *ral, const ral_params_flrc_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    // Force unused parameter to a known value.
    ral_params_flrc_t local_params = *params;

    local_params.bw_ssb_in_hz = 0;

    status = ral_sx1280_setup_flrc( ral, &local_params );
    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params( ral->context, SX1280_IRQ_TX_DONE,
                                                            SX1280_IRQ_TX_DONE,
                                                            SX1280_IRQ_NONE, SX1280_IRQ_NONE );
    }
    return status;
}

ral_status_t ral_sx1280_setup_tx_lora_e( const ral_t *ral, const ral_params_lora_e_t *params )
{
    return RAL_STATUS_UNSUPPORTED_FEATURE;
}

ral_status_t ral_sx1280_tx_bpsk( const ral_t *ral, const ral_params_bpsk_t *params )
{
    return RAL_STATUS_UNSUPPORTED_FEATURE;
}

ral_status_t ral_sx1280_setup_cad( const ral_t *ral, const ral_lora_cad_params_t *params )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_lora_cad_params_t sx1280_lora_cad_params;

    switch (params->cad_symb_nb) {
    case RAL_LORA_CAD_01_SYMB:
        sx1280_lora_cad_params.cad_symb_nb = SX1280_LORA_CAD_01_SYMB;
        break;
    case RAL_LORA_CAD_02_SYMB:
        sx1280_lora_cad_params.cad_symb_nb = SX1280_LORA_CAD_02_SYMB;
        break;
    case RAL_LORA_CAD_04_SYMB:
        sx1280_lora_cad_params.cad_symb_nb = SX1280_LORA_CAD_04_SYMB;
        break;
    case RAL_LORA_CAD_08_SYMB:
        sx1280_lora_cad_params.cad_symb_nb = SX1280_LORA_CAD_08_SYMB;
        break;
    case RAL_LORA_CAD_16_SYMB:
        sx1280_lora_cad_params.cad_symb_nb = SX1280_LORA_CAD_16_SYMB;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    status = ( ral_status_t )sx1280_set_cad_params( ral->context, &sx1280_lora_cad_params );

    if (status == RAL_STATUS_OK) {
        status = ( ral_status_t )sx1280_set_dio_irq_params( ral->context,
                                                            SX1280_IRQ_CAD_DONE | SX1280_IRQ_CAD_DET,
                                                            SX1280_IRQ_CAD_DONE | SX1280_IRQ_CAD_DET, SX1280_IRQ_NONE,
                                                            SX1280_IRQ_NONE );
    }

    return status;
}

ral_status_t ral_sx1280_set_pkt_payload( const ral_t *ral, const uint8_t *buffer,
                                         const uint16_t size )
{
    return ( ral_status_t )sx1280_write_buffer( ral->context, 0x00, buffer, size );
}

ral_status_t ral_sx1280_get_pkt_payload( const ral_t *ral, uint8_t *buffer, uint16_t max_size,
                                         uint16_t *size )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_rx_buffer_status_t sx_buf_status = { 0 };

    status = ( ral_status_t )sx1280_get_rx_buffer_status( ral->context, &sx_buf_status );
    if (status == RAL_STATUS_OK) {
        if (size != NULL) {
            sx1280_pkt_type_t pkt_type = SX1280_PKT_TYPE_GFSK;
            sx1280_lora_pkt_len_modes_t pkt_len_mode = SX1280_LORA_RANGE_PKT_EXPLICIT;

            status = ( ral_status_t )sx1280_get_pkt_type( ral->context, &pkt_type );

            if ((status == RAL_STATUS_OK) && (pkt_type == SX1280_PKT_TYPE_LORA)) {
                status =
                    ( ral_status_t )sx1280_get_lora_pkt_len_mode( ral->context, &pkt_len_mode );
            }

            if (status == RAL_STATUS_OK) {
                if ((pkt_type == SX1280_PKT_TYPE_LORA) &&
                    (pkt_len_mode == SX1280_LORA_RANGE_PKT_IMPLICIT)) {
                    uint8_t pkt_len = 0;

                    status = ( ral_status_t )sx1280_get_lora_pkt_len( ral->context, &pkt_len );
                    *size = ( uint16_t )pkt_len;
                }
                else {
                    *size = sx_buf_status.pld_len_in_bytes;
                }

                if (status == RAL_STATUS_OK) {
                    if (*size <= max_size) {
                        status = ( ral_status_t )sx1280_read_buffer( ral->context,
                                                                     sx_buf_status.buffer_start_pointer,
                                                                     buffer, *size );
                    }
                    else {
                        status = RAL_STATUS_ERROR;
                    }
                }
            }
        }
    }
    return status;
}

ral_status_t ral_sx1280_get_gfsk_pkt_status( const ral_t *ral,
                                             ral_rx_pkt_status_gfsk_t *pkt_status )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_pkt_status_gfsk_t sx_pkt_status = { 0 };

    status = ( ral_status_t )sx1280_get_gfsk_pkt_status( ral->context, &sx_pkt_status );

    if (status == RAL_STATUS_OK) {
        pkt_status->rssi_avg_in_dbm = sx_pkt_status.rssi;
    }

    return status;
}

ral_status_t ral_sx1280_get_lora_pkt_status( const ral_t *ral,
                                             ral_rx_pkt_status_lora_t *pkt_status )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_pkt_status_lora_t sx_pkt_status = { 0 };

    status = ( ral_status_t )sx1280_get_lora_pkt_status( ral->context, &sx_pkt_status );

    if (status == RAL_STATUS_OK) {
        pkt_status->rssi_pkt_in_dbm = sx_pkt_status.rssi;
        pkt_status->snr_pkt_in_db = sx_pkt_status.snr;
        pkt_status->signal_rssi_pkt_in_db = sx_pkt_status.rssi;
    }
    return status;
}

ral_status_t ral_sx1280_get_lora_incoming_pkt_config( const ral_t *ral, ral_lora_cr_t *rx_cr,
                                                      bool *rx_is_crc_en )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_lora_range_cr_t sx1280_lora_cr;

    status = ( ral_status_t )sx1280_get_lora_rx_packet_cr( ral->context, &sx1280_lora_cr );

    if (status == RAL_STATUS_OK) {
        switch (sx1280_lora_cr) {
        case SX1280_LORA_RANGE_CR_4_5:
            *rx_cr = RAL_LORA_CR_4_5;
            break;
        case SX1280_LORA_RANGE_CR_4_6:
            *rx_cr = RAL_LORA_CR_4_6;
            break;
        case SX1280_LORA_RANGE_CR_4_7:
            *rx_cr = RAL_LORA_CR_4_7;
            break;
        case SX1280_LORA_RANGE_CR_4_8:
            *rx_cr = RAL_LORA_CR_4_8;
            break;
        case SX1280_LORA_RANGE_CR_LI_4_5:
            *rx_cr = RAL_LORA_CR_LI_4_5;
            break;
        case SX1280_LORA_RANGE_CR_LI_4_6:
            *rx_cr = RAL_LORA_CR_LI_4_6;
            break;
        case SX1280_LORA_RANGE_CR_LI_4_8:
            *rx_cr = RAL_LORA_CR_LI_4_8;
            break;
        default:
            return RAL_STATUS_UNKNOWN_VALUE;
        }

        status = ( ral_status_t )sx1280_get_lora_rx_packet_crc_config( ral->context, rx_is_crc_en );
    }

    return status;
}

ral_status_t ral_sx1280_get_flrc_pkt_status( const ral_t *ral,
                                             ral_rx_pkt_status_flrc_t *pkt_status )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_pkt_status_flrc_t sx_pkt_status = { 0 };

    status = ( ral_status_t )sx1280_get_flrc_pkt_status( ral->context, &sx_pkt_status );

    if (status == RAL_STATUS_OK) {
        pkt_status->rssi_in_dbm = sx_pkt_status.rssi;
    }
    return status;
}

ral_status_t ral_sx1280_set_sleep( const ral_t *ral )
{
    return ( ral_status_t )sx1280_set_sleep(
        ral->context, SX1280_SLEEP_CFG_DATA_RETENTION | SX1280_SLEEP_CFG_DATA_BUFFER_RETENTION );
}

ral_status_t ral_sx1280_set_standby( const ral_t *ral )
{
    return ( ral_status_t )sx1280_set_standby( ral->context, SX1280_STANDBY_CFG_RC );
}

ral_status_t ral_sx1280_set_tx( const ral_t *ral )
{
    // Set no timeout.
    return ( ral_status_t )sx1280_set_tx( ral->context, SX1280_TICK_SIZE_1000_US, 0 );
}

ral_status_t ral_sx1280_set_tx_cw( const ral_t *ral )
{
    return ( ral_status_t )sx1280_set_tx_cw( ral->context );
}

// WORKAROUND #1 - LoRa modem not handling rx_single nb_symb_timeout
//                 (helper functions)

/*!
 * Get LoRa header time in ms for given spreading factor and bandwidth
 *
 * \param [in] sf_reg LoRa spreading factor register bits value [7:4]
 * \param [in] bw_reg LoRa bandwidth register bits value [3:1]
 *
 * \retval ral_bw RAL LoRa bandwidth value
 */
static uint16_t sx1280_get_lora_hdr_time_in_ms( uint8_t sf_reg, uint8_t bw_reg )
{
    // Header times in ms pre-computed table
    // hdr_nb_symb = 16.25
    // ts_in_ms = ( 1 << sf ) / bw_in_khz
    // hdr_time_in_ms = ceil( hdr_nb_symb * ts_in_ms );
    //
    const uint16_t hdr_times_in_ms[8][4] = {
        // clang-format off
        {   3,   2,   1,   1 }, // SF5  : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        {   6,   3,   2,   1 }, // SF6  : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        {  11,   6,   3,   2 }, // SF7  : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        {  21,  11,   6,   3 }, // SF8  : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        {  42,  21,  11,   6 }, // SF9  : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        {  84,  42,  21,  11 }, // SF10 : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        { 167,  84,  42,  21 }, // SF11 : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        { 333, 167,  84,  42 }, // SF12 : 200 kHz, 400 kHz, 800 kHz, 1600 kHz
        // clang-format on
    };
    uint8_t bw_index = 0;

    switch (bw_reg) {
    case (SX1280_LORA_RANGE_BW_200 & 0x0E):
        bw_index = 0;
        break;
    case (SX1280_LORA_RANGE_BW_400 & 0x0E):
        bw_index = 1;
        break;
    case (SX1280_LORA_RANGE_BW_800 & 0x0E):
        bw_index = 2;
        break;
    default:
    // Intentional fall through
    case (SX1280_LORA_RANGE_BW_1600 & 0x0E):
        bw_index = 3;
        break;
    }

    return hdr_times_in_ms[(sf_reg >> 4) - 5][bw_index];
}

// WORKAROUND #1 - END (helper functions)

ral_status_t ral_sx1280_set_rx( const ral_t *ral, const uint32_t timeout_ms )
{
    // WORKAROUND #1 - LoRa modem not handling rx_single nb_symb_timeout
    uint32_t local_timeout_ms = 0;
    sx1280_pkt_type_t pkt_type = SX1280_PKT_TYPE_GFSK;

    sx1280_get_pkt_type( ral->context, &pkt_type );
    switch (pkt_type) {
    case SX1280_PKT_TYPE_LORA:
    // Intentional fall through
    case SX1280_PKT_TYPE_RANGING:
    {
        uint8_t reg = 0;

        // Address 0x902: sf[7:4] - bw[3:1]
        sx1280_read_register( ral->context, 0x902, &reg, 1 );

        local_timeout_ms = timeout_ms + sx1280_get_lora_hdr_time_in_ms((reg & 0xF0), (reg & 0x0E));
        break;
    }
    case SX1280_PKT_TYPE_GFSK:
    // Intentional fall through
    case SX1280_PKT_TYPE_FLRC:
    // Intentional fall through
    case SX1280_PKT_TYPE_BLE:
        local_timeout_ms = timeout_ms;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }
    // WORKAROUND #1 - END

    if (timeout_ms == 0xFFFFFFFF) { // Continuous mode
        return ( ral_status_t )sx1280_set_rx( ral->context, SX1280_TICK_SIZE_1000_US, 0xFFFF );
    }
    else {
        if (local_timeout_ms <= UINT16_MAX) {
            return ( ral_status_t )sx1280_set_rx( ral->context, SX1280_TICK_SIZE_1000_US,
                                                  local_timeout_ms );
        }
        else {
            return ( ral_status_t )sx1280_set_rx( ral->context, SX1280_TICK_SIZE_4000_US,
                                                  local_timeout_ms >> 2 );
        }
    }
}

ral_status_t ral_sx1280_set_cad( const ral_t *ral )
{
    return ( ral_status_t )sx1280_set_cad( ral->context );
}

ral_status_t ral_sx1280_get_irq_status( const ral_t *ral, ral_irq_t *ral_irq )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_irq_mask_t sx1280_irq_mask = SX1280_IRQ_NONE;

    status = ( ral_status_t )sx1280_get_irq_status( ral->context, &sx1280_irq_mask );
    if (status == RAL_STATUS_OK) {
        *ral_irq = ral_sx1280_convert_irq_flags_to_radio( sx1280_irq_mask );
    }
    return status;
}

ral_status_t ral_sx1280_clear_irq_status( const ral_t *ral, const ral_irq_t ral_irq )
{
    sx1280_irq_mask_t sx1280_irq_mask = ral_sx1280_convert_irq_flags_from_radio( ral_irq );

    if (sx1280_irq_mask != 0) {
        return ( ral_status_t )sx1280_clear_irq_status( ral->context, sx1280_irq_mask );
    }
    else {
        return RAL_STATUS_OK;
    }
}

ral_status_t ral_sx1280_get_and_clear_irq_status( const ral_t *ral, ral_irq_t *ral_irq )
{
    ral_irq_t ral_irq_mask;
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_irq_mask_t sx1280_irq_mask = SX1280_IRQ_NONE;

    status = ( ral_status_t )sx1280_get_and_clear_irq_status( ral->context, &sx1280_irq_mask );

    if (status == RAL_STATUS_OK) {
        ral_irq_mask = ral_sx1280_convert_irq_flags_to_radio( sx1280_irq_mask );
        if (ral_irq != NULL) {
            *ral_irq = ral_irq_mask;
        }
    }
    return status;
}

ral_status_t ral_sx1280_set_dio_irq_params( const ral_t *ral, const ral_irq_t ral_irq )
{
    uint16_t sx1280_irq = ral_sx1280_convert_irq_flags_from_radio( ral_irq );

    return ( ral_status_t )sx1280_set_dio_irq_params( ral->context, sx1280_irq, sx1280_irq,
                                                      SX1280_IRQ_NONE,
                                                      SX1280_IRQ_NONE );
}

ral_status_t ral_sx1280_process_irq( const ral_t *ral, ral_irq_t *ral_irq )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_irq_mask_t sx1280_irq_mask = SX1280_IRQ_NONE;

    status = ( ral_status_t )sx1280_process_irq( ral->context, &sx1280_irq_mask );
    if (status == RAL_STATUS_OK) {
        *ral_irq = ral_sx1280_convert_irq_flags_to_radio( sx1280_irq_mask );
    }
    return status;
}

ral_status_t ral_sx1280_get_rssi( const ral_t *ral, int16_t *rssi )
{
    return ( ral_status_t )sx1280_get_rssi_inst( ral->context, rssi );
}

ral_status_t ral_sx1280_get_lora_time_on_air_in_ms( const ral_params_lora_t *params, uint32_t *toa )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_mod_params_lora_t mod_params = { 0 };
    sx1280_pkt_params_lora_t pkt_params = { 0 };

    status = ral_sx1280_convert_lora_params_from_radio( params, &mod_params, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    *toa = sx1280_get_lora_time_on_air_in_ms( &pkt_params, &mod_params );

    return status;
}

ral_status_t ral_sx1280_get_gfsk_time_on_air_in_ms( const ral_params_gfsk_t *params, uint32_t *toa )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_mod_params_gfsk_t mod_params = { 0 };
    sx1280_pkt_params_gfsk_t pkt_params = { 0 };

    status = ral_sx1280_convert_gfsk_params_from_radio( params, &mod_params, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    *toa = sx1280_get_gfsk_time_on_air_in_ms( &pkt_params, &mod_params );

    return RAL_STATUS_OK;
}

ral_status_t ral_sx1280_get_flrc_time_on_air_in_ms( const ral_params_flrc_t *params, uint32_t *toa )
{
    ral_status_t status = RAL_STATUS_ERROR;
    sx1280_mod_params_flrc_t mod_params = { 0 };
    sx1280_pkt_params_flrc_t pkt_params = { 0 };

    status = ral_sx1280_convert_flrc_params_from_radio( params, &mod_params, &pkt_params );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    *toa = sx1280_get_flrc_time_on_air_in_ms( &pkt_params, &mod_params );

    return RAL_STATUS_OK;
}

ral_status_t ral_sx1280_convert_lora_rx_bw_to_ua( const ral_params_lora_t *params,
                                                  uint32_t *micro_ampere )
{
    switch (params->bw) {
    case RAL_LORA_BW_200_KHZ:
    {
        *micro_ampere = 6200;
        return RAL_STATUS_OK;
    }
    case RAL_LORA_BW_400_KHZ:
    {
        *micro_ampere = 6700;
        return RAL_STATUS_OK;
    }
    case RAL_LORA_BW_800_KHZ:
    {
        *micro_ampere = 7700;
        return RAL_STATUS_OK;
    }
    case RAL_LORA_BW_1600_KHZ:
    {
        *micro_ampere = 8200;
        return RAL_STATUS_OK;
    }
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }
}

ral_status_t ral_sx1280_convert_lora_tx_dbm_to_ua( const ral_params_lora_t *params,
                                                   uint32_t *micro_ampere )
{
    if ((params->pwr_in_dbm < -18) || (params->pwr_in_dbm > 13)) {
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    *micro_ampere = ral_to_sx1280_lora_tx_dbm_to_ua[params->pwr_in_dbm + 18];

    return RAL_STATUS_OK;
}

ral_status_t ral_sx1280_read_register( const ral_t *ral, uint16_t address, uint8_t *buffer,
                                       uint16_t size )
{
    return ( ral_status_t )sx1280_read_register( ral->context, address, buffer, size );
}

ral_status_t ral_sx1280_write_register( const ral_t *ral, uint16_t address, uint8_t *buffer,
                                        uint16_t size )
{
    return ( ral_status_t )sx1280_write_register( ral->context, address, buffer, size );
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

static ral_status_t ral_sx1280_convert_gfsk_params_from_radio( const ral_params_gfsk_t *params,
                                                               sx1280_mod_params_gfsk_t *mod_params,
                                                               sx1280_pkt_params_gfsk_t *pkt_params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    uint32_t bw_dsb_in_hz = params->bw_ssb_in_hz << 1;
    sx1280_gfsk_ble_br_bw_t br_bw_dsb_param = 0;
    sx1280_gfsk_crc_types_t crc_type;
    sx1280_gfsk_flrc_ble_mod_shapes_t pulse_shape;

    status = ( ral_status_t )sx1280_get_gfsk_br_bw_param( params->br_in_bps, bw_dsb_in_hz,
                                                          &br_bw_dsb_param );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    switch (params->crc_type) {
    case RAL_GFSK_CRC_OFF:
        crc_type = SX1280_GFSK_CRC_OFF;
        break;
    case RAL_GFSK_CRC_1_BYTE:
        crc_type = SX1280_GFSK_CRC_1_BYTES;
        break;
    case RAL_GFSK_CRC_2_BYTES:
        crc_type = SX1280_GFSK_CRC_2_BYTES;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    switch (params->pulse_shape) {
    case RAL_GFSK_MOD_SHAPE_OFF:
        pulse_shape = SX1280_GFSK_FLRC_BLE_MOD_SHAPE_OFF;
        break;
    case RAL_GFSK_MOD_SHAPE_BT_05:
        pulse_shape = SX1280_GFSK_FLRC_BLE_MOD_SHAPE_BT_05;
        break;
    case RAL_GFSK_MOD_SHAPE_BT_1:
        pulse_shape = SX1280_GFSK_FLRC_BLE_MOD_SHAPE_BT_1;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    *mod_params = ( sx1280_mod_params_gfsk_t ){
        .br_bw = br_bw_dsb_param,
        .mod_ind = SX1280_GFSK_BLE_MOD_IND_1_00,
        .mod_shape = pulse_shape,
    };

    *pkt_params = ( sx1280_pkt_params_gfsk_t ){
        .pbl_len = ( sx1280_gfsk_pbl_len_t )(((params->pbl_len_in_bytes << 3) << 2) - 0x10),
        .sync_word_len = ( sx1280_gfsk_sync_word_len_t )((params->sync_word_len_in_bytes - 1) << 1),
        .match_sync_word = SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_1,
        .hdr_type =
            params->pld_is_fix ? SX1280_GFSK_FLRC_PKT_FIX_LEN : SX1280_GFSK_FLRC_PKT_VAR_LEN,
        .pld_len_in_bytes = params->pld_len_in_bytes,
        .crc_type = crc_type,
        .dc_free =
            params->dc_free_is_on ? SX1280_GFSK_FLRC_BLE_DC_FREE_ON :
            SX1280_GFSK_FLRC_BLE_DC_FREE_OFF,
    };

    return RAL_STATUS_OK;
}

static ral_status_t ral_sx1280_convert_lora_params_from_radio( const ral_params_lora_t *params,
                                                               sx1280_mod_params_lora_t *mod_params,
                                                               sx1280_pkt_params_lora_t *pkt_params )
{
    uint16_t pbl_len_in_symb = params->pbl_len_in_symb;

    // Ensure that the preamble length is at least 12 symbols when using SF5 or SF6
    if ((params->sf == RAL_LORA_SF5) || (params->sf == RAL_LORA_SF6)) {
        if (pbl_len_in_symb < 12) {
            pbl_len_in_symb = 12;
        }
    }

    mod_params->sf = ( sx1280_lora_sf_t )(params->sf << 4);

    switch (params->bw) {
    case RAL_LORA_BW_200_KHZ:
        mod_params->bw = SX1280_LORA_RANGE_BW_200;
        break;
    case RAL_LORA_BW_400_KHZ:
        mod_params->bw = SX1280_LORA_RANGE_BW_400;
        break;
    case RAL_LORA_BW_800_KHZ:
        mod_params->bw = SX1280_LORA_RANGE_BW_800;
        break;
    case RAL_LORA_BW_1600_KHZ:
        mod_params->bw = SX1280_LORA_RANGE_BW_1600;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    switch (params->cr) {
    case RAL_LORA_CR_4_5:
        mod_params->cr = SX1280_LORA_RANGE_CR_4_5;
        break;
    case RAL_LORA_CR_4_6:
        mod_params->cr = SX1280_LORA_RANGE_CR_4_6;
        break;
    case RAL_LORA_CR_4_7:
        mod_params->cr = SX1280_LORA_RANGE_CR_4_7;
        break;
    case RAL_LORA_CR_4_8:
        mod_params->cr = SX1280_LORA_RANGE_CR_4_8;
        break;
    case RAL_LORA_CR_LI_4_5:
        mod_params->cr = SX1280_LORA_RANGE_CR_LI_4_5;
        break;
    case RAL_LORA_CR_LI_4_6:
        mod_params->cr = SX1280_LORA_RANGE_CR_LI_4_6;
        break;
    case RAL_LORA_CR_LI_4_8:
        mod_params->cr = SX1280_LORA_RANGE_CR_LI_4_8;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    *pkt_params = ( sx1280_pkt_params_lora_t ){
        .pbl_len_in_symb = sx1280_convert_pbl_len_from_symb( pbl_len_in_symb ),
        .hdr_type =
            params->pld_is_fix ? SX1280_LORA_RANGE_PKT_IMPLICIT : SX1280_LORA_RANGE_PKT_EXPLICIT,
        .pld_len_in_bytes = params->pld_len_in_bytes,
        .crc_is_on = params->crc_is_on,
        .invert_iq_is_on = params->invert_iq_is_on,
    };

    return RAL_STATUS_OK;
}

static ral_status_t ral_sx1280_convert_flrc_params_from_radio( const ral_params_flrc_t *params,
                                                               sx1280_mod_params_flrc_t *mod_params,
                                                               sx1280_pkt_params_flrc_t *pkt_params )
{
    ral_status_t status = RAL_STATUS_ERROR;

    uint32_t bw_dsb_in_hz = params->bw_ssb_in_hz << 1;
    sx1280_flrc_br_bw_t br_bw_dsb_param = 0;
    sx1280_gfsk_flrc_ble_mod_shapes_t mod_shape;

    status = ( ral_status_t )sx1280_get_flrc_br_bw_param( params->br_in_bps, bw_dsb_in_hz,
                                                          &br_bw_dsb_param );
    if (status != RAL_STATUS_OK) {
        return status;
    }

    switch (params->mod_shape) {
    case RAL_FLRC_MOD_SHAPE_OFF:
        mod_shape = SX1280_GFSK_FLRC_BLE_MOD_SHAPE_OFF;
        break;
    case RAL_FLRC_MOD_SHAPE_BT_05:
        mod_shape = SX1280_GFSK_FLRC_BLE_MOD_SHAPE_BT_05;
        break;
    case RAL_FLRC_MOD_SHAPE_BT_1:
        mod_shape = SX1280_GFSK_FLRC_BLE_MOD_SHAPE_BT_1;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    *mod_params = ( sx1280_mod_params_flrc_t ){
        .br_bw = br_bw_dsb_param,
        .cr = ( sx1280_flrc_cr_t )(params->cr << 1),
        .mod_shape = mod_shape,
    };

    *pkt_params = ( sx1280_pkt_params_flrc_t ){
        .pbl_len = ( sx1280_flrc_pbl_len_t )(((params->pbl_len_in_bytes << 3) << 2) - 0x10),
        .sync_word_len =
            (params->sync_word_is_on ==
             true) ? SX1280_FLRC_SYNC_WORD_ON : SX1280_FLRC_SYNC_WORD_OFF,
        .match_sync_word = SX1280_GFSK_FLRC_RX_MATCH_SYNCWORD_1,
        .hdr_type =
            params->pld_is_fix ? SX1280_GFSK_FLRC_PKT_FIX_LEN : SX1280_GFSK_FLRC_PKT_VAR_LEN,
        .pld_len_in_bytes = params->pld_len_in_bytes,
        .crc_type = SX1280_FLRC_CRC_OFF,
    };

    switch (params->crc_type) {
    case RAL_FLRC_CRC_OFF:
        pkt_params->crc_type = SX1280_FLRC_CRC_OFF;
        break;
    case RAL_FLRC_CRC_2_BYTES:
        pkt_params->crc_type = SX1280_FLRC_CRC_2_BYTES;
        break;
    case RAL_FLRC_CRC_3_BYTES:
        pkt_params->crc_type = SX1280_FLRC_CRC_3_BYTES;
        break;
    case RAL_FLRC_CRC_4_BYTES:
        pkt_params->crc_type = SX1280_FLRC_CRC_4_BYTES;
        break;
    default:
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    return RAL_STATUS_OK;
}

static sx1280_lora_range_pbl_len_t sx1280_convert_pbl_len_from_symb(
    const uint16_t pbl_len_in_symb )
{
    uint16_t mant = pbl_len_in_symb;
    int exp = shift_and_count_trailing_zeros( &mant );

    // This will loop at most 5 times
    while (mant > 15) {
        // Divide by two, rounding up
        mant = (mant + 1) >> 1;
        exp++;
    }

    return ( sx1280_lora_range_pbl_len_t ){
               .mant = mant,
               .exp = exp,
    };
}

static ral_irq_t ral_sx1280_convert_irq_flags_to_radio( sx1280_irq_mask_t sx1280_irq )
{
    ral_irq_t ral_irq = RAL_IRQ_NONE;

    if ((sx1280_irq & SX1280_IRQ_TX_DONE) == SX1280_IRQ_TX_DONE) {
        ral_irq |= RAL_IRQ_TX_DONE;
    }
    if ((sx1280_irq & SX1280_IRQ_RX_DONE) == SX1280_IRQ_RX_DONE) {
        ral_irq |= RAL_IRQ_RX_DONE;
    }
    if ((sx1280_irq & SX1280_IRQ_TIMEOUT) == SX1280_IRQ_TIMEOUT) {
        ral_irq |= RAL_IRQ_RX_TIMEOUT;
    }
    if (((sx1280_irq & SX1280_IRQ_SYNC_WORD_VALID) == SX1280_IRQ_SYNC_WORD_VALID) ||
        ((sx1280_irq & SX1280_IRQ_HEADER_VALID) == SX1280_IRQ_HEADER_VALID)) {
        ral_irq |= RAL_IRQ_RX_HDR_OK;
    }
    if (((sx1280_irq & SX1280_IRQ_SYNC_WORD_ERROR) == SX1280_IRQ_SYNC_WORD_ERROR) ||
        ((sx1280_irq & SX1280_IRQ_HEADER_ERROR) == SX1280_IRQ_HEADER_ERROR)) {
        ral_irq |= RAL_IRQ_RX_HDR_ERROR;
    }
    if ((sx1280_irq & SX1280_IRQ_CRC_ERROR) == SX1280_IRQ_CRC_ERROR) {
        ral_irq |= RAL_IRQ_RX_CRC_ERROR;
    }
    if ((sx1280_irq & SX1280_IRQ_CAD_DONE) == SX1280_IRQ_CAD_DONE) {
        ral_irq |= RAL_IRQ_CAD_DONE;
    }
    if ((sx1280_irq & SX1280_IRQ_CAD_DET) == SX1280_IRQ_CAD_DET) {
        ral_irq |= RAL_IRQ_CAD_OK;
    }
    return ral_irq;
}

static sx1280_irq_mask_t ral_sx1280_convert_irq_flags_from_radio( ral_irq_t ral_irq )
{
    sx1280_irq_mask_t sx1280_irq_mask = SX1280_IRQ_NONE;

    if ((ral_irq & RAL_IRQ_TX_DONE) == RAL_IRQ_TX_DONE) {
        sx1280_irq_mask |= SX1280_IRQ_TX_DONE;
    }
    if ((ral_irq & RAL_IRQ_RX_DONE) == RAL_IRQ_RX_DONE) {
        sx1280_irq_mask |= SX1280_IRQ_RX_DONE;
    }
    if ((ral_irq & RAL_IRQ_RX_TIMEOUT) == RAL_IRQ_RX_TIMEOUT) {
        sx1280_irq_mask |= SX1280_IRQ_TIMEOUT;
    }
    if ((ral_irq & RAL_IRQ_RX_HDR_OK) == RAL_IRQ_RX_HDR_OK) {
        sx1280_irq_mask |= SX1280_IRQ_SYNC_WORD_VALID;
        sx1280_irq_mask |= SX1280_IRQ_HEADER_VALID;
    }
    if ((ral_irq & RAL_IRQ_RX_HDR_ERROR) == RAL_IRQ_RX_HDR_ERROR) {
        sx1280_irq_mask |= SX1280_IRQ_SYNC_WORD_ERROR;
        sx1280_irq_mask |= SX1280_IRQ_HEADER_ERROR;
    }
    if ((ral_irq & RAL_IRQ_RX_CRC_ERROR) == RAL_IRQ_RX_CRC_ERROR) {
        sx1280_irq_mask |= SX1280_IRQ_CRC_ERROR;
    }
    if ((ral_irq & RAL_IRQ_CAD_DONE) == RAL_IRQ_CAD_DONE) {
        sx1280_irq_mask |= SX1280_IRQ_CAD_DONE;
    }
    if ((ral_irq & RAL_IRQ_CAD_OK) == RAL_IRQ_CAD_OK) {
        sx1280_irq_mask |= SX1280_IRQ_CAD_DET;
    }
    if ((ral_irq & RAL_IRQ_ALL) == RAL_IRQ_ALL) {
        sx1280_irq_mask |= SX1280_IRQ_ALL;
    }

    return sx1280_irq_mask;
}

static uint8_t shift_and_count_trailing_zeros( uint16_t *x )
{
    uint8_t exponent = 0;

    if ((*x & 0x00FF) == 0) {
        exponent += 8;
        *x >>= 8;
    }
    if ((*x & 0x000F) == 0) {
        exponent += 4;
        *x >>= 4;
    }
    if ((*x & 0x0003) == 0) {
        exponent += 2;
        *x >>= 2;
    }
    if ((*x & 0x0001) == 0) {
        exponent += 1;
        *x >>= 1;
    }

    return exponent;
}

#pragma GCC diagnostic pop
/* --- EOF ------------------------------------------------------------------ */
