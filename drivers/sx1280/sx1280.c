/*!
 * @file      sx1280.c
 *
 * @brief     SX1280 radio driver implementation
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

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <string.h>  // memcpy
#include "sx1280_hal.h"
#include "sx1280_regs.h"
#include "sx1280.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

/*!
 *  Provides the frequency of the chip running on the radio and the frequency step
 *
 * \remark These defines are used for computing the frequency divider to set the RF frequency
 */
#define XTAL_FREQ 52000000U
#define XTAL_FREQ_STEP_DIV_POWER 18

#define INV_FREQ_STEP (1.0F * (1U << XTAL_FREQ_STEP_DIV_POWER) / XTAL_FREQ)

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*!
 * Commands Interface
 */
typedef enum sx1280_commands_e {
    // Operational Modes Functions
    SX1280_SET_SLEEP                = 0x84,
    SX1280_SET_STANDBY              = 0x80,
    SX1280_SET_FS                   = 0xC1,
    SX1280_SET_TX                   = 0x83,
    SX1280_SET_RX                   = 0x82,
    SX1280_SET_RXDUTYCYCLE          = 0x94,
    SX1280_SET_CAD                  = 0xC5,
    SX1280_SET_AUTOTX               = 0x98,
    SX1280_SET_AUTOFS               = 0x9E,
    SX1280_SET_TXCONTINUOUSWAVE     = 0xD1,
    SX1280_SET_TXCONTINUOUSPREAMBLE = 0xD2,
    // Registers and buffer Access
    SX1280_WRITE_REGISTER           = 0x18,
    SX1280_READ_REGISTER            = 0x19,
    SX1280_WRITE_BUFFER             = 0x1A,
    SX1280_READ_BUFFER              = 0x1B,
    // DIO and IRQ Control Functions
    SX1280_SET_DIOIRQPARAMS         = 0x8D,
    SX1280_GET_IRQSTATUS            = 0x15,
    SX1280_CLR_IRQSTATUS            = 0x97,
    // RF Modulation and Packet-Related Functions
    SX1280_SET_RFFREQUENCY          = 0x86,
    SX1280_SET_PACKETTYPE           = 0x8A,
    SX1280_GET_PACKETTYPE           = 0x03,
    SX1280_SET_TXPARAMS             = 0x8E,
    SX1280_SET_MODULATIONPARAMS     = 0x8B,
    SX1280_SET_PACKETPARAMS         = 0x8C,
    SX1280_SET_CADPARAMS            = 0x88,
    SX1280_SET_BUFFERBASEADDRESS    = 0x8F,
    // Communication Status Information
    SX1280_GET_STATUS               = 0xC0,
    SX1280_GET_RXBUFFERSTATUS       = 0x17,
    SX1280_GET_PACKETSTATUS         = 0x1D,
    SX1280_GET_RSSIINST             = 0x1F,
    // Miscellaneous
    SX1280_SET_LONGPREAMBLE         = 0x9B,
    SX1280_SET_REGULATORMODE        = 0x96,
    SX1280_SET_SAVECONTEXT          = 0xD5,
    SX1280_SET_RANGING_ROLE         = 0xA3,
    SX1280_SET_ADV_RANGING          = 0x9A,
} sx1280_commands_t;

/*!
 * Commands Interface buffer sizes
 */
typedef enum sx1280_commands_size_e {
    // Operational Modes Functions
    SX1280_SIZE_SET_SLEEP                   = 2,
    SX1280_SIZE_SET_STANDBY                 = 2,
    SX1280_SIZE_SET_FS                      = 1,
    SX1280_SIZE_SET_TX                      = 4,
    SX1280_SIZE_SET_RX                      = 4,
    SX1280_SIZE_SET_RXDUTYCYCLE             = 6,
    SX1280_SIZE_SET_CAD                     = 1,
    SX1280_SIZE_SET_AUTOTX                  = 3,
    SX1280_SIZE_SET_AUTOFS                  = 2,
    SX1280_SIZE_SET_TXCONTINUOUSWAVE        = 1,
    SX1280_SIZE_SET_TXCONTINUOUSPREAMBLE    = 1,
    // Registers and buffer Access
    // Full size: this value plus buffer size
    SX1280_SIZE_WRITE_REGISTER              = 3,
    // Full size: this value plus buffer size
    SX1280_SIZE_READ_REGISTER               = 4,
    // Full size: this value plus buffer size
    SX1280_SIZE_WRITE_BUFFER                = 2,
    // Full size: this value plus buffer size
    SX1280_SIZE_READ_BUFFER                 = 3,
    // DIO and IRQ Control Functions
    SX1280_SIZE_SET_DIOIRQPARAMS            = 9,
    SX1280_SIZE_GET_IRQSTATUS               = 2,
    SX1280_SIZE_CLR_IRQSTATUS               = 3,
    // RF Modulation and Packet-Related Functions
    SX1280_SIZE_SET_RFFREQUENCY             = 4,
    SX1280_SIZE_SET_PACKETTYPE              = 2,
    SX1280_SIZE_GET_PACKETTYPE              = 2,
    SX1280_SIZE_SET_TXPARAMS                = 3,
    SX1280_SIZE_SET_MODULATIONPARAMS        = 4,
    SX1280_SIZE_SET_PACKETPARAMS            = 8,
    SX1280_SIZE_SET_CADPARAMS               = 2,
    SX1280_SIZE_SET_BUFFERBASEADDRESS       = 3,
    // Communication Status Information
    SX1280_SIZE_GET_STATUS                  = 1,
    SX1280_SIZE_GET_RXBUFFERSTATUS          = 2,
    SX1280_SIZE_GET_PACKETSTATUS            = 2,
    SX1280_SIZE_GET_RSSIINST                = 2,
    // Miscellaneous
    SX1280_SIZE_SET_LONGPREAMBLE            = 2,
    SX1280_SIZE_SET_REGULATORMODE           = 2,
    SX1280_SIZE_SET_SAVECONTEXT             = 1,
    SX1280_SIZE_SET_RANGING_ROLE            = 2,
    SX1280_SIZE_SET_ADV_RANGING             = 2,
    SX1280_SIZE_MAX_BUFFER                  = 255,
} sx1280_commands_size_t;

typedef struct {
    uint32_t br;
    uint32_t bw;
    uint8_t param;
} sx1280_br_bw_t;

static const sx1280_br_bw_t sx1280_gfsk_br_bw[] = {
    { 125000, 300000, SX1280_GFSK_BLE_BR_0_125_BW_0_3 },
    { 250000, 300000, SX1280_GFSK_BLE_BR_0_250_BW_0_3 },
    { 250000, 600000, SX1280_GFSK_BLE_BR_0_250_BW_0_6 },
    { 400000, 600000, SX1280_GFSK_BLE_BR_0_400_BW_0_6 },
    { 500000, 600000, SX1280_GFSK_BLE_BR_0_500_BW_0_6 },
    { 400000, 1200000, SX1280_GFSK_BLE_BR_0_400_BW_1_2 },
    { 500000, 1200000, SX1280_GFSK_BLE_BR_0_500_BW_1_2 },
    { 800000, 1200000, SX1280_GFSK_BLE_BR_0_800_BW_1_2 },
    { 1000000, 1200000, SX1280_GFSK_BLE_BR_1_000_BW_1_2 },
    { 800000, 2400000, SX1280_GFSK_BLE_BR_0_800_BW_2_4 },
    { 1000000, 2400000, SX1280_GFSK_BLE_BR_1_000_BW_2_4 },
    { 1600000, 2400000, SX1280_GFSK_BLE_BR_1_600_BW_2_4 },
    { 2000000, 2400000, SX1280_GFSK_BLE_BR_2_000_BW_2_4 },
};

static const sx1280_br_bw_t sx1280_flrc_br_bw[] = {
    { 260000, 300000, SX1280_FLRC_BR_0_260_BW_0_3 },
    { 325000, 300000, SX1280_FLRC_BR_0_325_BW_0_3 },
    { 520000, 600000, SX1280_FLRC_BR_0_520_BW_0_6 },
    { 650000, 600000, SX1280_FLRC_BR_0_650_BW_0_6 },
    { 1040000, 1200000, SX1280_FLRC_BR_1_040_BW_1_2 },
    { 1300000, 1200000, SX1280_FLRC_BR_1_300_BW_1_2 },
};

#define SX1280_GFSK_MOD_IND_DENOMINATOR 20
typedef struct {
    uint8_t numerator;
    sx1280_gfsk_ble_mod_ind_t mod_ind;
} sx1280_gfsk_mod_ind_frac_t;

static const sx1280_gfsk_mod_ind_frac_t sx1280_gfsk_mod_ind[] = {
    { 7, SX1280_GFSK_BLE_MOD_IND_0_35 },  { 10, SX1280_GFSK_BLE_MOD_IND_0_50 },
    { 15, SX1280_GFSK_BLE_MOD_IND_0_75 },
    { 20, SX1280_GFSK_BLE_MOD_IND_1_00 }, { 25, SX1280_GFSK_BLE_MOD_IND_1_25 },
    { 30, SX1280_GFSK_BLE_MOD_IND_1_50 },
    { 35, SX1280_GFSK_BLE_MOD_IND_1_75 }, { 40, SX1280_GFSK_BLE_MOD_IND_2_00 },
    { 45, SX1280_GFSK_BLE_MOD_IND_2_25 },
    { 50, SX1280_GFSK_BLE_MOD_IND_2_50 }, { 55, SX1280_GFSK_BLE_MOD_IND_2_75 },
    { 60, SX1280_GFSK_BLE_MOD_IND_3_00 },
    { 65, SX1280_GFSK_BLE_MOD_IND_3_25 }, { 70, SX1280_GFSK_BLE_MOD_IND_3_50 },
    { 75, SX1280_GFSK_BLE_MOD_IND_3_75 },
    { 80, SX1280_GFSK_BLE_MOD_IND_4_00 },

};

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

static uint32_t sx1280_get_pbl_len_in_bits_gfsk_flrc( sx1280_gfsk_pbl_len_t pbl_len );

static uint32_t sx1280_get_sync_word_len_in_bytes_gfsk( sx1280_gfsk_sync_word_len_t sync_word_len );

static uint32_t sx1280_get_crc_len_in_bytes_gfsk( sx1280_gfsk_crc_types_t crc_type );

static uint32_t sx1280_get_crc_len_in_bytes_flrc( sx1280_flrc_crc_types_t crc_type );

static uint32_t sx1280_get_hdr_len_in_bits_gfsk( sx1280_gfsk_flrc_pkt_len_modes_t hdr_type );

static uint32_t sx1280_get_gfsk_time_on_air_numerator( const sx1280_pkt_params_gfsk_t *pkt_params );

static uint32_t sx1280_get_lora_bw_in_hz( sx1280_lora_bw_t bw );

static uint32_t sx1280_get_lora_time_on_air_numerator( const sx1280_pkt_params_lora_t *pkt_params,
                                                       const sx1280_mod_params_lora_t *mod_params );

static inline uint32_t sx1280_get_sync_word_len_in_bytes_flrc(
    sx1280_flrc_sync_word_len_t sync_word_len );

static inline uint32_t sx1280_get_hdr_len_in_bits_flrc( sx1280_gfsk_flrc_pkt_len_modes_t hdr_type );

static inline uint32_t sx1280_get_tail_len_in_bits_flrc( sx1280_flrc_cr_t cr );

static inline uint32_t sx1280_get_cr_den_flrc( sx1280_flrc_cr_t cr );

static uint32_t sx1280_get_flrc_time_on_air_numerator( const sx1280_pkt_params_flrc_t *pkt_p,
                                                       const sx1280_mod_params_flrc_t *mod_p );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

sx1280_status_t sx1280_set_sleep( const void *context, const sx1280_sleep_cfg_t cfg )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_SLEEP] = { 0 };

    buf[0] = SX1280_SET_SLEEP;

    buf[1] = ( uint8_t )cfg;

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_SLEEP, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_SLEEP );
    }

    return status;
}

sx1280_status_t sx1280_set_standby( const void *context, const sx1280_standby_cfg_t cfg )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_STANDBY] = { 0 };

    buf[0] = SX1280_SET_STANDBY;

    buf[1] = ( uint8_t )cfg;

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_STANDBY, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode(
            context,
            (cfg ==
             SX1280_STANDBY_CFG_RC) ? SX1280_HAL_OP_MODE_STDBY_RC : SX1280_HAL_OP_MODE_STDBY_XOSC );
    }

    return status;
}

sx1280_status_t sx1280_wakeup( const void *context )
{
    return ( sx1280_status_t )sx1280_hal_wakeup( context );
}

sx1280_status_t sx1280_set_fs( const void *context )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_FS] = { 0 };

    buf[0] = SX1280_SET_FS;

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_FS, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_FS );
    }

    return status;
}

sx1280_status_t sx1280_set_tx( const void *context, sx1280_tick_size_t period_base,
                               const uint16_t timeout_in_ticks )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_TX] = { 0 };

    buf[0] = SX1280_SET_TX;

    buf[1] = ( uint8_t )period_base;
    buf[2] = ( uint8_t )(timeout_in_ticks >> 8);
    buf[3] = ( uint8_t )(timeout_in_ticks >> 0);

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_TX, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_TX );
    }

    return status;
}

sx1280_status_t sx1280_set_rx( const void *context, sx1280_tick_size_t period_base,
                               const uint16_t timeout_in_ticks )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_RX] = { 0 };

    buf[0] = SX1280_SET_RX;

    buf[1] = ( uint8_t )period_base;
    buf[2] = ( uint8_t )(timeout_in_ticks >> 8);
    buf[3] = ( uint8_t )(timeout_in_ticks >> 0);

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_RX, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        if (timeout_in_ticks == 0xFFFF) { // Continuous mode
            sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_RX_C );
        }
        else {
            sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_RX );
        }
    }

    return status;
}

sx1280_status_t sx1280_set_rx_duty_cycle( const void *context, sx1280_tick_size_t period_base,
                                          const uint16_t rx_time,
                                          const uint16_t sleep_time )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_RXDUTYCYCLE] = { 0 };

    buf[0] = SX1280_SET_RXDUTYCYCLE;

    buf[1] = ( uint8_t )period_base;

    buf[2] = ( uint8_t )(rx_time >> 8);
    buf[3] = ( uint8_t )(rx_time >> 0);

    buf[4] = ( uint8_t )(sleep_time >> 8);
    buf[5] = ( uint8_t )(sleep_time >> 0);

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_RXDUTYCYCLE, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_RX_DC );
    }

    return status;
}

sx1280_status_t sx1280_set_cad( const void *context )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_CAD] = { 0 };

    buf[0] = SX1280_SET_CAD;

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_CAD, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_CAD );
    }

    return status;
}

sx1280_status_t sx1280_set_tx_cw( const void *context )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_TXCONTINUOUSWAVE] = { 0 };

    buf[0] = SX1280_SET_TXCONTINUOUSWAVE;

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_TXCONTINUOUSWAVE, 0,
                                                  0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_TX );
    }

    return status;
}

sx1280_status_t sx1280_set_tx_cpbl( const void *context )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[SX1280_SIZE_SET_TXCONTINUOUSPREAMBLE] = { 0 };

    buf[0] = SX1280_SET_TXCONTINUOUSPREAMBLE;

    status = ( sx1280_status_t )sx1280_hal_write( context, buf,
                                                  SX1280_SIZE_SET_TXCONTINUOUSPREAMBLE, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_TX );
    }

    return status;
}

//
// Registers and buffer Access
//

sx1280_status_t sx1280_write_register( const void *context, const uint16_t addr,
                                       const uint8_t *buffer,
                                       const uint8_t size )
{
    uint8_t buf[SX1280_SIZE_WRITE_REGISTER] = { 0 };

    buf[0] = SX1280_WRITE_REGISTER;

    buf[1] = ( uint8_t )(addr >> 8);
    buf[2] = ( uint8_t )(addr >> 0);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_WRITE_REGISTER, buffer,
                                                size );
}

sx1280_status_t sx1280_read_register( const void *context, const uint16_t addr, uint8_t *buffer,
                                      const uint8_t size )
{
    uint8_t buf[SX1280_SIZE_READ_REGISTER] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;

    buf[0] = SX1280_READ_REGISTER;

    buf[1] = ( uint8_t )(addr >> 8);
    buf[2] = ( uint8_t )(addr >> 0);

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_READ_REGISTER, buffer,
                                                 size );

    return status;
}

sx1280_status_t sx1280_write_buffer( const void *context, const uint8_t offset,
                                     const uint8_t *buffer,
                                     const uint8_t size )
{
    uint8_t buf[SX1280_SIZE_WRITE_BUFFER] = { 0 };

    buf[0] = SX1280_WRITE_BUFFER;

    buf[1] = offset;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_WRITE_BUFFER, buffer,
                                                size );
}

sx1280_status_t sx1280_read_buffer( const void *context, const uint8_t offset, uint8_t *buffer,
                                    const uint8_t size )
{
    uint8_t buf[SX1280_SIZE_READ_BUFFER] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;

    buf[0] = SX1280_READ_BUFFER;

    buf[1] = offset;

    status =
        ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_READ_BUFFER, buffer, size );

    return status;
}

//
// DIO and IRQ Control Functions
//
sx1280_status_t sx1280_set_dio_irq_params( const void *context, const uint16_t irq_mask,
                                           const uint16_t dio1_mask,
                                           const uint16_t dio2_mask, const uint16_t dio3_mask )
{
    uint8_t buf[SX1280_SIZE_SET_DIOIRQPARAMS] = { 0 };

    buf[0] = SX1280_SET_DIOIRQPARAMS;

    buf[1] = ( uint8_t )(irq_mask >> 8);
    buf[2] = ( uint8_t )(irq_mask >> 0);

    buf[3] = ( uint8_t )(dio1_mask >> 8);
    buf[4] = ( uint8_t )(dio1_mask >> 0);

    buf[5] = ( uint8_t )(dio2_mask >> 8);
    buf[6] = ( uint8_t )(dio2_mask >> 0);

    buf[7] = ( uint8_t )(dio3_mask >> 8);
    buf[8] = ( uint8_t )(dio3_mask >> 0);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_DIOIRQPARAMS, 0, 0 );
}

sx1280_status_t sx1280_get_irq_status( const void *context, sx1280_irq_mask_t *irq )
{
    uint8_t buf[SX1280_SIZE_GET_IRQSTATUS] = { 0 };
    uint8_t irq_local[sizeof(sx1280_irq_mask_t)] = { 0x00 };
    sx1280_status_t status = SX1280_STATUS_ERROR;

    buf[0] = SX1280_GET_IRQSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_IRQSTATUS, irq_local,
                                                 sizeof(sx1280_irq_mask_t));

    if (status == SX1280_STATUS_OK) {
        *irq = (( sx1280_irq_mask_t )irq_local[0] << 8) + (( sx1280_irq_mask_t )irq_local[1] << 0);
    }

    return status;
}

sx1280_status_t sx1280_clear_irq_status( const void *context, const sx1280_irq_mask_t irq_mask )
{
    uint8_t buf[SX1280_SIZE_CLR_IRQSTATUS] = { 0 };

    buf[0] = SX1280_CLR_IRQSTATUS;

    buf[1] = ( uint8_t )(irq_mask >> 8);
    buf[2] = ( uint8_t )(irq_mask >> 0);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_CLR_IRQSTATUS, 0, 0 );
}

sx1280_status_t sx1280_get_and_clear_irq_status( const void *context, sx1280_irq_mask_t *irq )
{
    sx1280_irq_mask_t sx1280_irq_mask = SX1280_IRQ_NONE;

    sx1280_status_t status = sx1280_get_irq_status( context, &sx1280_irq_mask );

    if ((status == SX1280_STATUS_OK) && (sx1280_irq_mask != 0)) {
        status = sx1280_clear_irq_status( context, sx1280_irq_mask );
    }
    if ((status == SX1280_STATUS_OK) && (irq != NULL)) {
        *irq = sx1280_irq_mask;
    }
    return status;
}

//
// RF Modulation and Packet-Related Functions
//

sx1280_status_t sx1280_set_rf_freq( const void *context, const uint32_t freq_in_hz )
{
    uint8_t buf[SX1280_SIZE_SET_RFFREQUENCY] = { 0 };
    uint32_t freq = freq_in_hz * INV_FREQ_STEP;

    // Alternate 64-bit integer calculation instead of floating point:
    // uint32_t freq = ((uint64_t)freq_in_hz << XTAL_FREQ_STEP_DIV_POWER) /
    // XTAL_FREQ;

    buf[0] = SX1280_SET_RFFREQUENCY;

    buf[1] = ( uint8_t )(freq >> 16);
    buf[2] = ( uint8_t )(freq >> 8);
    buf[3] = ( uint8_t )(freq >> 0);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_RFFREQUENCY, 0, 0 );
}

sx1280_status_t sx1280_set_pkt_type( const void *context, const sx1280_pkt_type_t pkt_type )
{
    uint8_t buf[SX1280_SIZE_SET_PACKETTYPE] = { 0 };

    buf[0] = SX1280_SET_PACKETTYPE;

    buf[1] = ( uint8_t )pkt_type;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_PACKETTYPE, 0, 0 );
}

sx1280_status_t sx1280_get_pkt_type( const void *context, sx1280_pkt_type_t *pkt_type )
{
    uint8_t buf[SX1280_SIZE_GET_PACKETTYPE] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;

    buf[0] = SX1280_GET_PACKETTYPE;

    status =
        ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_PACKETTYPE,
                                            ( uint8_t * )pkt_type, 1 );

    return status;
}

sx1280_status_t sx1280_set_tx_params( const void *context, const int8_t power,
                                      const sx1280_ramp_time_t ramp_time )
{
    uint8_t buf[SX1280_SIZE_SET_TXPARAMS] = { 0 };

    if ((power < SX1280_PWR_MIN) || (power > SX1280_PWR_MAX)) {
        return SX1280_STATUS_ERROR;
    }

    buf[0] = SX1280_SET_TXPARAMS;

    buf[1] = ( uint8_t )(power + 18);
    buf[2] = ( uint8_t )ramp_time;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_TXPARAMS, 0, 0 );
}

sx1280_status_t sx1280_set_gfsk_mod_params( const void *context,
                                            const sx1280_mod_params_gfsk_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_MODULATIONPARAMS] = { 0 };

    buf[0] = SX1280_SET_MODULATIONPARAMS;

    buf[1] = params->br_bw;
    buf[2] = params->mod_ind;
    buf[3] = params->mod_shape;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_MODULATIONPARAMS, 0,
                                                0 );
}

sx1280_status_t sx1280_set_lora_mod_params( const void *context,
                                            const sx1280_mod_params_lora_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_MODULATIONPARAMS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;

    buf[0] = SX1280_SET_MODULATIONPARAMS;

    buf[1] = ( uint8_t )(params->sf);
    buf[2] = ( uint8_t )(params->bw);
    buf[3] = ( uint8_t )(params->cr);

    status = ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_MODULATIONPARAMS, 0,
                                                  0 );

    if (status == SX1280_STATUS_OK) {
        uint8_t reg_buffer;

        switch (params->sf) {
        case SX1280_LORA_RANGE_SF5:
        case SX1280_LORA_RANGE_SF6:
            reg_buffer = 0x1E;
            break;
        case SX1280_LORA_RANGE_SF7:
        case SX1280_LORA_RANGE_SF8:
            reg_buffer = 0x37;
            break;
        case SX1280_LORA_RANGE_SF9:
        case SX1280_LORA_RANGE_SF10:
        case SX1280_LORA_RANGE_SF11:
        case SX1280_LORA_RANGE_SF12:
            reg_buffer = 0x32;
            break;
        }

        status = sx1280_write_register( context, SX1280_REG_LORA_SF_CFG0, &reg_buffer, 1 );

        if (status == SX1280_STATUS_OK) {
            reg_buffer = 0;
            status = sx1280_read_register( context, SX1280_REG_LORA_FREQ_ERR_CORR, &reg_buffer, 1 );

            if (status == SX1280_STATUS_OK) {
                reg_buffer &=
                    ~(SX1280_REG_LORA_FREQ_ERR_CORR_MASK << SX1280_REG_LORA_FREQ_ERR_CORR_POS);
                reg_buffer += (0x01 << SX1280_REG_LORA_FREQ_ERR_CORR_POS);

                status = sx1280_write_register( context, SX1280_REG_LORA_FREQ_ERR_CORR, &reg_buffer,
                                                1 );
            }
        }
    }

    return status;
}

sx1280_status_t sx1280_set_range_mod_params( const void *context,
                                             const sx1280_mod_params_range_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_MODULATIONPARAMS] = { 0 };

    buf[0] = SX1280_SET_MODULATIONPARAMS;

    buf[1] = ( uint8_t )(params->sf);
    buf[2] = ( uint8_t )(params->bw);
    buf[3] = ( uint8_t )(params->cr);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_MODULATIONPARAMS, 0,
                                                0 );
}

sx1280_status_t sx1280_set_flrc_mod_params( const void *context,
                                            const sx1280_mod_params_flrc_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_MODULATIONPARAMS] = { 0 };

    buf[0] = SX1280_SET_MODULATIONPARAMS;

    buf[1] = ( uint8_t )(params->br_bw);
    buf[2] = ( uint8_t )(params->cr);
    buf[3] = ( uint8_t )(params->mod_shape);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_MODULATIONPARAMS, 0,
                                                0 );
}

sx1280_status_t sx1280_set_ble_mod_params( const void *context,
                                           const sx1280_mod_params_ble_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_MODULATIONPARAMS] = { 0 };

    buf[0] = SX1280_SET_MODULATIONPARAMS;

    buf[1] = ( uint8_t )(params->br_bw);
    buf[2] = ( uint8_t )(params->mod_ind);
    buf[3] = ( uint8_t )(params->mod_shape);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_MODULATIONPARAMS, 0,
                                                0 );
}

sx1280_status_t sx1280_set_gfsk_pkt_params( const void *context,
                                            const sx1280_pkt_params_gfsk_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_PACKETPARAMS] = { 0 };

    buf[0] = SX1280_SET_PACKETPARAMS;

    buf[1] = ( uint8_t )(params->pbl_len);
    buf[2] = ( uint8_t )(params->sync_word_len);
    buf[3] = ( uint8_t )(params->match_sync_word);
    buf[4] = ( uint8_t )(params->hdr_type);
    buf[5] = params->pld_len_in_bytes;
    buf[6] = ( uint8_t )(params->crc_type);
    buf[7] = ( uint8_t )(params->dc_free);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_PACKETPARAMS, 0, 0 );
}

sx1280_status_t sx1280_set_lora_pkt_params( const void *context,
                                            const sx1280_pkt_params_lora_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_PACKETPARAMS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;

    buf[0] = SX1280_SET_PACKETPARAMS;

    buf[1] = params->pbl_len_in_symb.mant | (params->pbl_len_in_symb.exp << 4);
    buf[2] = ( uint8_t )(params->hdr_type);
    buf[3] = params->pld_len_in_bytes;
    buf[4] = ( uint8_t )((params->crc_is_on == true) ? 0x20 : 0x00);
    buf[5] = ( uint8_t )((params->invert_iq_is_on == true) ? 0x00 : 0x40);

    status =
        ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_PACKETPARAMS, 0, 0 );

    if (status == SX1280_STATUS_OK) {
        uint8_t data = (buf[5] == 0x00) ? 0x0D : 0x09;
        status = sx1280_write_register( context, SX1280_REG_LR_IQ_CONF, &data, 1 );
    }

    return status;
}

sx1280_status_t sx1280_set_range_pkt_params( const void *context,
                                             const sx1280_pkt_params_range_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_PACKETPARAMS] = { 0 };

    buf[0] = SX1280_SET_PACKETPARAMS;

    buf[1] = params->pbl_len_in_symb.mant | (params->pbl_len_in_symb.exp << 4);
    buf[2] = ( uint8_t )(params->hdr_type);
    buf[3] = params->pld_len_in_bytes;
    buf[4] = ( uint8_t )((params->crc_is_on == true) ? 0x20 : 0x00);
    buf[5] = ( uint8_t )((params->invert_iq_is_on == true) ? 0x00 : 0x40);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_PACKETPARAMS, 0, 0 );
}

sx1280_status_t sx1280_set_flrc_pkt_params( const void *context,
                                            const sx1280_pkt_params_flrc_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_PACKETPARAMS] = { 0 };

    buf[0] = SX1280_SET_PACKETPARAMS;

    buf[1] = ( uint8_t )(params->pbl_len);
    buf[2] = ( uint8_t )(params->sync_word_len);
    buf[3] = ( uint8_t )(params->match_sync_word);
    buf[4] = ( uint8_t )(params->hdr_type);
    buf[5] = params->pld_len_in_bytes;
    buf[6] = ( uint8_t )(params->crc_type);
    buf[7] = SX1280_GFSK_FLRC_BLE_DC_FREE_OFF;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_PACKETPARAMS, 0, 0 );
}

sx1280_status_t sx1280_set_ble_pkt_params( const void *context,
                                           const sx1280_pkt_params_ble_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_PACKETPARAMS] = { 0 };

    buf[0] = SX1280_SET_PACKETPARAMS;

    buf[1] = ( uint8_t )(params->con_state);
    buf[2] = ( uint8_t )(params->crc_type);
    buf[3] = ( uint8_t )(params->pkt_type);
    buf[4] = ( uint8_t )(params->dc_free);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_PACKETPARAMS, 0, 0 );
}

sx1280_status_t sx1280_set_cad_params( const void *context, const sx1280_lora_cad_params_t *params )
{
    uint8_t buf[SX1280_SIZE_SET_CADPARAMS] = { 0 };

    buf[0] = SX1280_SET_CADPARAMS;

    buf[1] = ( uint8_t )params->cad_symb_nb;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_CADPARAMS, 0, 0 );
}

sx1280_status_t sx1280_set_buffer_base_addr( const void *context, const uint8_t tx_base_addr,
                                             const uint8_t rx_base_addr )
{
    uint8_t buf[SX1280_SIZE_SET_BUFFERBASEADDRESS] = { 0 };

    buf[0] = SX1280_SET_BUFFERBASEADDRESS;

    buf[1] = tx_base_addr;
    buf[2] = rx_base_addr;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_BUFFERBASEADDRESS, 0,
                                                0 );
}

//
// Communication Status Information
//

sx1280_status_t sx1280_get_status( const void *context, sx1280_chip_status_t *radio_status )
{
    uint8_t buf[SX1280_SIZE_GET_STATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t radio_status_raw = 0x00;

    buf[0] = SX1280_GET_STATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_STATUS,
                                                 &radio_status_raw, 1 );

    if (status == SX1280_STATUS_OK) {
        radio_status->cmd_status =
            ( sx1280_cmd_status_t )((radio_status_raw >>
                                     SX1280_CMD_STATUS_POS) & SX1280_CMD_STATUS_MASK);
        radio_status->chip_mode =
            ( sx1280_chip_modes_t )((radio_status_raw >>
                                     SX1280_CHIP_MODES_POS) & SX1280_CHIP_MODES_MASK);
    }

    return status;
}

sx1280_status_t sx1280_get_rx_buffer_status( const void *context,
                                             sx1280_rx_buffer_status_t *rx_buffer_status )
{
    uint8_t buf[SX1280_SIZE_GET_RXBUFFERSTATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t rx_buffer_status_raw[sizeof(sx1280_rx_buffer_status_t)] = { 0x00 };

    buf[0] = SX1280_GET_RXBUFFERSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_RXBUFFERSTATUS,
                                                 rx_buffer_status_raw,
                                                 sizeof(sx1280_rx_buffer_status_t));

    if (status == SX1280_STATUS_OK) {
        rx_buffer_status->pld_len_in_bytes = rx_buffer_status_raw[0];
        rx_buffer_status->buffer_start_pointer = rx_buffer_status_raw[1];
    }

    return status;
}

sx1280_status_t sx1280_get_gfsk_pkt_status( const void *context,
                                            sx1280_pkt_status_gfsk_t *pkt_status )
{
    uint8_t buf[SX1280_SIZE_GET_PACKETSTATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t pkt_status_raw[5] = { 0x00 };

    buf[0] = SX1280_GET_PACKETSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_PACKETSTATUS,
                                                 pkt_status_raw, 5 );

    if (status == SX1280_STATUS_OK) {
        // pkt_status_raw[0] is RFU
        pkt_status->rssi = ( int8_t )(-pkt_status_raw[1] >> 1);
        pkt_status->errors = pkt_status_raw[2];
        pkt_status->status = pkt_status_raw[3];
        pkt_status->sync = pkt_status_raw[4];
    }

    return status;
}

sx1280_status_t sx1280_get_lora_pkt_status( const void *context,
                                            sx1280_pkt_status_lora_t *pkt_status )
{
    uint8_t buf[SX1280_SIZE_GET_PACKETSTATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t pkt_status_raw[5] = { 0x00 };

    buf[0] = SX1280_GET_PACKETSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_PACKETSTATUS,
                                                 pkt_status_raw, 5 );

    if (status == SX1280_STATUS_OK) {
        pkt_status->rssi = ( int8_t )(-pkt_status_raw[0] >> 1);
        pkt_status->snr = ((( int8_t )pkt_status_raw[1]) + 2) >> 2;
    }

    return status;
}

sx1280_status_t sx1280_get_range_pkt_status( const void *context,
                                             sx1280_pkt_status_lora_t *pkt_status )
{
    uint8_t buf[SX1280_SIZE_GET_PACKETSTATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t pkt_status_raw[5] = { 0x00 };

    buf[0] = SX1280_GET_PACKETSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_PACKETSTATUS,
                                                 pkt_status_raw, 5 );

    if (status == SX1280_STATUS_OK) {
        pkt_status->rssi = ( int8_t )(-pkt_status_raw[0] >> 1);
        pkt_status->snr = ((( int8_t )pkt_status_raw[1]) + 2) >> 2;
    }

    return status;
}

sx1280_status_t sx1280_get_flrc_pkt_status( const void *context,
                                            sx1280_pkt_status_flrc_t *pkt_status )
{
    uint8_t buf[SX1280_SIZE_GET_PACKETSTATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t pkt_status_raw[5] = { 0x00 };

    buf[0] = SX1280_GET_PACKETSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_PACKETSTATUS,
                                                 pkt_status_raw, 5 );

    if (status == SX1280_STATUS_OK) {
        // pkt_status_raw[0] is RFU
        pkt_status->rssi = ( int8_t )(-pkt_status_raw[1] >> 1);
        pkt_status->errors = pkt_status_raw[2];
        pkt_status->status = pkt_status_raw[3];
        pkt_status->sync = pkt_status_raw[4];
    }

    return status;
}

sx1280_status_t sx1280_get_ble_pkt_status( const void *context,
                                           sx1280_pkt_status_ble_t *pkt_status )
{
    uint8_t buf[SX1280_SIZE_GET_PACKETSTATUS] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t pkt_status_raw[5] = { 0x00 };

    buf[0] = SX1280_GET_PACKETSTATUS;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_PACKETSTATUS,
                                                 pkt_status_raw, 5 );

    if (status == SX1280_STATUS_OK) {
        // pkt_status_raw[0] is RFU
        pkt_status->rssi = ( int8_t )(-pkt_status_raw[1] >> 1);
        pkt_status->errors = pkt_status_raw[2];
        pkt_status->status = pkt_status_raw[3];
        pkt_status->sync = pkt_status_raw[4];
    }

    return status;
}

sx1280_status_t sx1280_get_rssi_inst( const void *context, int16_t *rssi )
{
    uint8_t buf[SX1280_SIZE_GET_RSSIINST] = { 0 };
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t rssi_raw = 0x00;

    buf[0] = SX1280_GET_RSSIINST;

    status = ( sx1280_status_t )sx1280_hal_read( context, buf, SX1280_SIZE_GET_RSSIINST, &rssi_raw,
                                                 1 );

    if (status == SX1280_STATUS_OK) {
        *rssi = ( int16_t )(-rssi_raw >> 1);
    }

    return status;
}

//
// Miscellaneous
//

sx1280_operating_mode_t sx1280_get_operating_mode( const void *context )
{
    return ( sx1280_operating_mode_t )sx1280_hal_get_operating_mode( context );
}

sx1280_status_t sx1280_reset( const void *context )
{
    sx1280_hal_reset( context );
    sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_STDBY_RC );
    return SX1280_STATUS_OK;
}

sx1280_status_t sx1280_process_irq( const void *context, sx1280_irq_mask_t *irq_status )
{
    sx1280_operating_mode_t op_mode = sx1280_get_operating_mode( context );

    sx1280_get_irq_status( context, irq_status );

    if (((*irq_status & SX1280_IRQ_TX_DONE) == SX1280_IRQ_TX_DONE) ||
        ((*irq_status & SX1280_IRQ_CAD_DONE) == SX1280_IRQ_CAD_DONE) ||
        ((*irq_status & SX1280_IRQ_TIMEOUT) == SX1280_IRQ_TIMEOUT)) {
        sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_STDBY_RC );
    }

    if (((*irq_status & SX1280_IRQ_HEADER_ERROR) == SX1280_IRQ_HEADER_ERROR) ||
        ((*irq_status & SX1280_IRQ_SYNC_WORD_ERROR) == SX1280_IRQ_SYNC_WORD_ERROR) ||
        ((*irq_status & SX1280_IRQ_RX_DONE) == SX1280_IRQ_RX_DONE) ||
        ((*irq_status & SX1280_IRQ_CRC_ERROR) == SX1280_IRQ_CRC_ERROR)) {
        if (op_mode != SX1280_OP_MODE_RX_C) {
            sx1280_hal_set_operating_mode( context, SX1280_HAL_OP_MODE_STDBY_RC );
        }
    }

    return SX1280_STATUS_OK;
}

sx1280_status_t sx1280_set_long_pbl( const void *context, const bool state )
{
    uint8_t buf[SX1280_SIZE_SET_LONGPREAMBLE] = { 0 };

    buf[0] = SX1280_SET_LONGPREAMBLE;

    buf[1] = ( uint8_t )((state == true) ? 0x01 : 0x00);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_LONGPREAMBLE, 0, 0 );
}

sx1280_status_t sx1280_set_reg_mode( const void *context, const sx1280_reg_mod_t mode )
{
    uint8_t buf[SX1280_SIZE_SET_REGULATORMODE] = { 0 };

    buf[0] = SX1280_SET_REGULATORMODE;

    buf[1] = ( uint8_t )mode;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_REGULATORMODE, 0, 0 );
}

sx1280_status_t sx1280_set_lna_settings( const void *context, sx1280_lna_settings_t settings )
{
    sx1280_status_t status;
    uint8_t reg_value;

    status = sx1280_read_register( context, SX1280_REG_LNA_REGIME, &reg_value, 1 );

    if (status == SX1280_STATUS_OK) {
        if (settings == SX1280_LNA_HIGH_SENSITIVITY_MODE) {
            reg_value |= 0xC0;
        }
        if (settings == SX1280_LNA_LOW_POWER_MODE) {
            reg_value &= ~0xC0;
        }

        status = sx1280_write_register( context, SX1280_REG_LNA_REGIME, &reg_value, 1 );
    }

    return status;
}

sx1280_status_t sx1280_save_context( const void *context )
{
    uint8_t buf[SX1280_SIZE_SET_SAVECONTEXT] = { 0 };

    buf[0] = SX1280_SET_SAVECONTEXT;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_SAVECONTEXT, 0, 0 );
}

sx1280_status_t sx1280_set_ranging_role( const void *context, const sx1280_range_role_t role )
{
    uint8_t buf[SX1280_SIZE_SET_RANGING_ROLE] = { 0 };

    buf[0] = SX1280_SET_RANGING_ROLE;

    buf[1] = ( uint8_t )role;

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_RANGING_ROLE, 0, 0 );
}

sx1280_status_t sx1280_set_adv_ranging( const void *context, const bool state )
{
    uint8_t buf[SX1280_SIZE_SET_ADV_RANGING] = { 0 };

    buf[0] = SX1280_SET_ADV_RANGING;

    buf[1] = ( uint8_t )((state == true) ? 0x01 : 0x00);

    return ( sx1280_status_t )sx1280_hal_write( context, buf, SX1280_SIZE_SET_ADV_RANGING, 0, 0 );
}

sx1280_status_t sx1280_set_gfsk_flrc_sync_word_tolerance( const void *context, uint8_t tolerance )
{
    sx1280_status_t status;
    uint8_t reg_value;

    status =
        sx1280_read_register( context, SX1280_REG_GFSK_FLRC_SYNC_WORD_TOLERANCE, &reg_value, 1 );

    if (status == SX1280_STATUS_OK) {
        reg_value = (reg_value & 0xF0) | (tolerance & 0x0F);
        status = sx1280_write_register( context, SX1280_REG_GFSK_FLRC_SYNC_WORD_TOLERANCE,
                                        &reg_value, 1 );
    }

    return status;
}

sx1280_status_t sx1280_get_gfsk_br_bw_param( const uint32_t br, const uint32_t bw,
                                             sx1280_gfsk_ble_br_bw_t *param )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;

    if ((br != 0) && (bw != 0)) {
        status = SX1280_STATUS_UNKNOWN_VALUE;
        for (uint8_t i = 0; i < (sizeof(sx1280_gfsk_br_bw) / sizeof(sx1280_br_bw_t)); i++) {
            if ((br <= sx1280_gfsk_br_bw[i].br) && (bw <= sx1280_gfsk_br_bw[i].bw)) {
                *param = sx1280_gfsk_br_bw[i].param;
                status = SX1280_STATUS_OK;
                break;
            }
        }
    }

    return status;
}

sx1280_status_t sx1280_get_gfsk_mod_ind_param( const uint32_t br, const uint32_t fdev,
                                               sx1280_gfsk_ble_mod_ind_t *param )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;

    if (br != 0) {
        status = SX1280_STATUS_UNKNOWN_VALUE;

        for (int8_t i = (sizeof(sx1280_gfsk_mod_ind) / sizeof(sx1280_gfsk_mod_ind_frac_t)) - 1;
             i >= 0; i--) {
            if ((br * sx1280_gfsk_mod_ind[i].numerator) <=
                ((2 * SX1280_GFSK_MOD_IND_DENOMINATOR) * fdev)) {
                *param = sx1280_gfsk_mod_ind[i].mod_ind;
                status = SX1280_STATUS_OK;
                break;
            }
        }
    }

    return status;
}

sx1280_status_t sx1280_get_flrc_br_bw_param( const uint32_t br, const uint32_t bw,
                                             sx1280_flrc_br_bw_t *param )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;

    if ((br != 0) && (bw != 0)) {
        status = SX1280_STATUS_UNKNOWN_VALUE;
        for (uint8_t i = 0; i < (sizeof(sx1280_flrc_br_bw) / sizeof(sx1280_br_bw_t)); i++) {
            if ((br <= sx1280_flrc_br_bw[i].br) && (bw <= sx1280_flrc_br_bw[i].bw)) {
                *param = sx1280_flrc_br_bw[i].param;
                status = SX1280_STATUS_OK;
                break;
            }
        }
    }

    return status;
}

uint32_t sx1280_get_lora_time_on_air_in_ms( const sx1280_pkt_params_lora_t *pkt_p,
                                            const sx1280_mod_params_lora_t *mod_p )
{
    uint32_t numerator = 1000U * sx1280_get_lora_time_on_air_numerator( pkt_p, mod_p );
    uint32_t denominator = sx1280_get_lora_bw_in_hz( mod_p->bw );

    // Perform integral ceil()
    return (numerator + denominator - 1) / denominator;
}

uint32_t sx1280_get_bitrate_in_hz_gfsk( sx1280_gfsk_br_bw_t br_bw )
{
    switch (br_bw) {
    case SX1280_GFSK_BLE_BR_2_000_BW_2_4:
        return 2000000UL;
    case SX1280_GFSK_BLE_BR_1_600_BW_2_4:
        return 1600000UL;
    case SX1280_GFSK_BLE_BR_1_000_BW_2_4:
        return 1000000UL;
    case SX1280_GFSK_BLE_BR_1_000_BW_1_2:
        return 1000000UL;
    case SX1280_GFSK_BLE_BR_0_800_BW_2_4:
        return 800000UL;
    case SX1280_GFSK_BLE_BR_0_800_BW_1_2:
        return 800000UL;
    case SX1280_GFSK_BLE_BR_0_500_BW_1_2:
        return 500000UL;
    case SX1280_GFSK_BLE_BR_0_500_BW_0_6:
        return 500000UL;
    case SX1280_GFSK_BLE_BR_0_400_BW_1_2:
        return 400000UL;
    case SX1280_GFSK_BLE_BR_0_400_BW_0_6:
        return 400000UL;
    case SX1280_GFSK_BLE_BR_0_250_BW_0_6:
        return 250000UL;
    case SX1280_GFSK_BLE_BR_0_250_BW_0_3:
        return 250000UL;
    case SX1280_GFSK_BLE_BR_0_125_BW_0_3:
        return 125000UL;
    }
    return 0;
}

uint32_t sx1280_get_gfsk_time_on_air_in_ms( const sx1280_pkt_params_gfsk_t *pkt_p,
                                            const sx1280_mod_params_gfsk_t *mod_p )
{
    uint32_t numerator = 1000U * sx1280_get_gfsk_time_on_air_numerator( pkt_p );
    uint32_t denominator = sx1280_get_bitrate_in_hz_gfsk( mod_p->br_bw );

    // Perform integral ceil()
    return (numerator + denominator - 1) / denominator;
}

uint32_t sx1280_get_bitrate_in_hz_flrc( sx1280_flrc_br_bw_t br_bw )
{
    switch (br_bw) {
    case SX1280_FLRC_BR_2_600_BW_2_4:
        return 2600000UL;
    case SX1280_FLRC_BR_2_080_BW_2_4:
        return 2080000UL;
    case SX1280_FLRC_BR_1_300_BW_1_2:
        return 1300000UL;
    case SX1280_FLRC_BR_1_040_BW_1_2:
        return 1040000UL;
    case SX1280_FLRC_BR_0_650_BW_0_6:
        return 650000UL;
    case SX1280_FLRC_BR_0_520_BW_0_6:
        return 520000UL;
    case SX1280_FLRC_BR_0_325_BW_0_3:
        return 325000UL;
    case SX1280_FLRC_BR_0_260_BW_0_3:
        return 260000UL;
    }
    return 0;
}

uint32_t sx1280_get_flrc_time_on_air_in_ms( const sx1280_pkt_params_flrc_t *pkt_p,
                                            const sx1280_mod_params_flrc_t *mod_p )
{
    uint32_t numerator = 1000U * sx1280_get_flrc_time_on_air_numerator( pkt_p, mod_p );
    uint32_t denominator = sx1280_get_bitrate_in_hz_flrc( mod_p->br_bw );

    // Perform integral ceil()
    return (numerator + denominator - 1) / denominator;
}

//
// Registers access
//

sx1280_status_t sx1280_set_gfsk_sync_word( const void *context, const uint8_t sync_word_id,
                                           const uint8_t *sync_word,
                                           const uint8_t sync_word_len )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[5] = { 0 };
    uint16_t reg_addr = 0;

    switch (sync_word_id) {
    case 1:
        reg_addr = SX1280_REG_GFSK_SYNC_WORD_1;
        break;
    case 2:
        reg_addr = SX1280_REG_GFSK_SYNC_WORD_2;
        break;
    case 3:
        reg_addr = SX1280_REG_GFSK_SYNC_WORD_3;
        break;
    default:
        return SX1280_STATUS_UNKNOWN_VALUE;
    }
    if (sync_word_len <= 5) {
        memcpy( buf, sync_word, sync_word_len );
        status = sx1280_write_register( context, reg_addr, buf, 5 );
    }

    return status;
}

sx1280_status_t sx1280_set_lora_sync_word( const void *context, const uint8_t sync_word )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buffer[2] = { 0x00 };

    status = sx1280_read_register( context, SX1280_REG_LR_SYNC_WORD, buffer, 2 );

    if (status == SX1280_STATUS_OK) {
        buffer[0] = (buffer[0] & ~0xF0) + (sync_word & 0xF0);
        buffer[1] = (buffer[1] & ~0xF0) + ((sync_word & 0x0F) << 4);

        status = sx1280_write_register( context, SX1280_REG_LR_SYNC_WORD, buffer, 2 );
    }

    return status;
}

sx1280_status_t sx1280_set_flrc_sync_word( const void *context, const uint8_t sync_word_id,
                                           const uint8_t *sync_word,
                                           const uint8_t sync_word_len )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buf[4] = { 0 };
    uint16_t reg_addr = 0;

    switch (sync_word_id) {
    case 1:
        reg_addr = SX1280_REG_FLRC_SYNC_WORD_1;
        break;
    case 2:
        reg_addr = SX1280_REG_FLRC_SYNC_WORD_2;
        break;
    case 3:
        reg_addr = SX1280_REG_FLRC_SYNC_WORD_3;
        break;
    default:
        return SX1280_STATUS_UNKNOWN_VALUE;
    }
    if (sync_word_len <= 4) {
        memcpy( buf, sync_word, sync_word_len );
        status = sx1280_write_register( context, reg_addr, buf, 4 );
    }

    return status;
}

sx1280_status_t sx1280_set_gfsk_crc_seed( const void *context, uint16_t seed )
{
    uint8_t s[] = { ( uint8_t )(seed >> 8), ( uint8_t )seed };

    return sx1280_write_register( context, SX1280_REG_GFSK_CRC_SEED, s, sizeof(s));
}

sx1280_status_t sx1280_set_flrc_crc_seed( const void *context, uint32_t seed )
{
    uint8_t s[] = {
        ( uint8_t )(seed >> 24),
        ( uint8_t )(seed >> 16),
        ( uint8_t )(seed >> 8),
        ( uint8_t )seed,
    };

    return sx1280_write_register( context, SX1280_REG_FLRC_CRC_SEED, s, sizeof(s));
}

sx1280_status_t sx1280_set_gfsk_crc_polynomial( const void *context, uint16_t polynomial )
{
    uint8_t poly[] = { ( uint8_t )(polynomial >> 8), ( uint8_t )polynomial };

    return sx1280_write_register( context, SX1280_REG_GFSK_CRC_POLYNOM, poly, sizeof(poly));
}

sx1280_status_t sx1280_set_gfsk_whitening_seed( const void *context, uint16_t seed )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t reg_value = 0;

    // The SX1280_REG_GFSK_WHITENING_SEED \ref LSBit is used for the
    // seed value. The 7 MSBits must not be modified. Thus, we first need to
    // read the current value and then change the LSB according to the
    // provided seed \ref value.
    status = sx1280_read_register( context, SX1280_REG_GFSK_WHITENING_SEED, &reg_value, 1 );
    if (status == SX1280_STATUS_OK) {
        reg_value = (reg_value & 0xFE) | (( uint8_t )(seed >> 8) & 0x01);
        status = sx1280_write_register( context, SX1280_REG_GFSK_WHITENING_SEED, &reg_value, 1 );
        if (status == SX1280_STATUS_OK) {
            reg_value = ( uint8_t )seed;
            status = sx1280_write_register( context, SX1280_REG_GFSK_WHITENING_SEED + 1, &reg_value,
                                            1 );
        }
    }

    return status;
}

sx1280_status_t sx1280_get_lora_pkt_len_mode( const void *context,
                                              sx1280_lora_pkt_len_modes_t *hdr_type )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t reg_value = 0;

    status = sx1280_read_register( context, SX1280_REG_LORA_HEADER_MODE, &reg_value, 1 );
    if (status == SX1280_STATUS_OK) {
        // We need only to read the MSBit
        *hdr_type = ( sx1280_lora_pkt_len_modes_t )(reg_value & SX1280_LORA_RANGE_PKT_IMPLICIT);
    }

    return status;
}

sx1280_status_t sx1280_get_lora_pkt_len( const void *context, uint8_t *pkt_len )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;

    status = sx1280_read_register( context, SX1280_REG_LR_PAYLOAD_LENGTH, pkt_len, 1 );
    return status;
}

sx1280_status_t sx1280_get_lora_rx_packet_cr( const void *context,
                                              sx1280_lora_range_cr_t *lora_incoming_cr )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buffer = 0;

    status = sx1280_read_register( context, SX1280_REG_LR_INCOMING_CR, &buffer, 1 );
    if (status == SX1280_STATUS_OK) {
        *lora_incoming_cr =
            ( sx1280_lora_range_cr_t )((buffer >>
                                        SX1280_REG_LR_INCOMING_CR_POS) &
                                       SX1280_REG_LR_INCOMING_CR_MASK);
    }

    return status;
}

sx1280_status_t sx1280_get_lora_rx_packet_crc_config( const void *context,
                                                      bool *is_lora_crc_enabled )
{
    sx1280_status_t status = SX1280_STATUS_ERROR;
    uint8_t buffer = 0;

    status = sx1280_read_register( context, SX1280_REG_LR_INCOMING_CRC, &buffer, 1 );
    if (status == SX1280_STATUS_OK) {
        *is_lora_crc_enabled =
            ((buffer >> SX1280_REG_LR_INCOMING_CRC_POS) & SX1280_REG_LR_INCOMING_CRC_MASK) !=
            0 ? true : false;
    }

    return status;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

static inline uint32_t sx1280_get_pbl_len_in_bits_gfsk_flrc( sx1280_gfsk_pbl_len_t pbl_len )
{
    return (pbl_len >> 2) + 4;
}

static inline uint32_t sx1280_get_sync_word_len_in_bytes_gfsk(
    sx1280_gfsk_sync_word_len_t sync_word_len )
{
    return (sync_word_len >> 1) + 1;
}

static inline uint32_t sx1280_get_crc_len_in_bytes_gfsk( sx1280_gfsk_crc_types_t crc_type )
{
    return crc_type >> 4;
}

static inline uint32_t sx1280_get_crc_len_in_bytes_flrc( sx1280_flrc_crc_types_t crc_len )
{
    if (crc_len == SX1280_FLRC_CRC_OFF) {
        return 0;
    }
    return (crc_len >> 4) + 1;
}

static inline uint32_t sx1280_get_hdr_len_in_bits_gfsk( sx1280_gfsk_flrc_pkt_len_modes_t hdr_type )
{
    if (hdr_type == SX1280_GFSK_FLRC_PKT_VAR_LEN) {
        return 9;
    }
    else {
        return 0;
    }
}

static uint32_t sx1280_get_gfsk_time_on_air_numerator( const sx1280_pkt_params_gfsk_t *pkt_p )
{
    return sx1280_get_pbl_len_in_bits_gfsk_flrc( pkt_p->pbl_len ) + sx1280_get_hdr_len_in_bits_gfsk(
        pkt_p->hdr_type ) +
           ((sx1280_get_sync_word_len_in_bytes_gfsk( pkt_p->sync_word_len ) +
             pkt_p->pld_len_in_bytes +
             sx1280_get_crc_len_in_bytes_gfsk( pkt_p->crc_type ))
            << 3);
}

static uint32_t sx1280_get_lora_bw_in_hz( sx1280_lora_bw_t bw )
{
    switch (bw) {
    case SX1280_LORA_RANGE_BW_200:
        return 203125UL;
    case SX1280_LORA_RANGE_BW_400:
        return 406250UL;
    case SX1280_LORA_RANGE_BW_800:
        return 812500UL;
    case SX1280_LORA_RANGE_BW_1600:
        return 1625000UL;
    }
    return 0;
}

uint32_t sx1280_get_lora_time_on_air_numerator( const sx1280_pkt_params_lora_t *pkt_params,
                                                const sx1280_mod_params_lora_t *mod_params )
{
    const bool pld_is_fix = pkt_params->hdr_type == SX1280_LORA_RANGE_PKT_IMPLICIT;
    const uint8_t cr_plus_4 = (mod_params->cr == 7) ? 8 : ((mod_params->cr - 1) & 0x3) + 5;

    const int32_t pld_len_in_bits = pkt_params->pld_len_in_bytes << 3;
    const int32_t sf = mod_params->sf >> 4;
    const int32_t n_sym_header = pld_is_fix == true ? 0 : 20;
    const int32_t n_bit_crc = (pkt_params->crc_is_on == true) ? 16 : 0;
    const bool long_interleaving = mod_params->cr > SX1280_LORA_RANGE_CR_4_8;

    int32_t pbl_len_in_symb = (( int32_t )pkt_params->pbl_len_in_symb.mant) <<
                              pkt_params->pbl_len_in_symb.exp;

    int32_t ceil_num;
    int32_t ceil_den;
    int32_t max_val = pld_len_in_bits + n_bit_crc;
    int32_t ceiling;

    if (pld_is_fix == true && long_interleaving == true) {
        int32_t n_sym_begin_num = max_val * cr_plus_4;
        int32_t n_sym_begin_den;

        if (sf < 7) {
            n_sym_begin_den = 4 * sf;
        }
        else if (sf <= 10) {
            n_sym_begin_den = 4 * (sf - 2);
        }
        else {
            n_sym_begin_den = 4 * (sf - 2);
        }

        const int32_t n_sym_begin = (n_sym_begin_num + n_sym_begin_den - 1) / n_sym_begin_den;

        if (n_sym_begin <= 8) {
            ceiling = n_sym_begin;
        }
        else {
            if (sf < 7) {
                ceil_num = n_sym_begin_num - 32 * sf;
                ceil_den = 4 * sf;
            }
            else if (sf <= 10) {
                ceil_num = n_sym_begin_num + 64 - 32 * sf;
                ceil_den = 4 * sf;
            }
            else {
                ceil_num = n_sym_begin_num - 32 * (sf - 2);
                ceil_den = 4 * (sf - 2);
            }

            ceiling = ((ceil_num + ceil_den - 1) / ceil_den) + 8;
        }
    }
    else {
        if (long_interleaving == true) {
            int32_t n_bit_hdr_space;
            if (sf < 7) {
                n_bit_hdr_space = ((sf - 5) >> 1) * 8;
            }
            else {
                n_bit_hdr_space = ((sf - 7) >> 1) * 8;
            }

            if (max_val > n_bit_hdr_space) {
                if (pld_len_in_bits < n_bit_hdr_space) {
                    n_bit_hdr_space = pld_len_in_bits;
                }
            }

            max_val -= n_bit_hdr_space;

            if (max_val < 0) {
                max_val = 0;
            }

            ceil_num = max_val * cr_plus_4;
        }
        else {
            max_val = max_val - (4 * sf) + n_sym_header;

            if (sf >= 7) {
                max_val += 8;
            }

            if (max_val < 0) {
                max_val = 0;
            }

            ceil_num = max_val;
        }

        if (sf <= 10) {
            ceil_den = 4 * sf;
        }
        else {
            ceil_den = 4 * (sf - 2);
        }

        ceiling = ((ceil_num + ceil_den - 1) / ceil_den);

        if (long_interleaving == false) {
            ceiling *= cr_plus_4;
        }

        ceiling += 8;
    }

    int32_t intermed = ceiling + pbl_len_in_symb + 4;

    if (sf < 7) {
        intermed += 2;
    }

    // Add the quarter symbol and calculate the TOA numerator
    return ( uint32_t )((4 * intermed + 1) * (1 << (sf - 2)));
}

static inline uint32_t sx1280_get_sync_word_len_in_bytes_flrc(
    sx1280_flrc_sync_word_len_t sync_word_len )
{
    return ( uint32_t )sync_word_len;
}

static inline uint32_t sx1280_get_hdr_len_in_bits_flrc( sx1280_gfsk_flrc_pkt_len_modes_t hdr_type )
{
    if (hdr_type == SX1280_GFSK_FLRC_PKT_VAR_LEN) {
        return 12;
    }
    else {
        return 0;
    }
}

static inline uint32_t sx1280_get_tail_len_in_bits_flrc( sx1280_flrc_cr_t cr )
{
    if ((cr == SX1280_FLRC_CR_1_2) || (cr == SX1280_FLRC_CR_3_4)) {
        return 6;
    }
    else {
        return 0;
    }
}

static inline uint32_t sx1280_get_cr_den_flrc( sx1280_flrc_cr_t cr )
{
    uint32_t ceil_den = 0;

    if (cr == SX1280_FLRC_CR_1_2) {
        ceil_den = 2;
    }
    else if (cr == SX1280_FLRC_CR_3_4) {
        ceil_den = 3;
    }
    else if (cr == SX1280_FLRC_CR_1_1) {
        ceil_den = 4;
    }

    return ceil_den;
}

static uint32_t sx1280_get_flrc_time_on_air_numerator( const sx1280_pkt_params_flrc_t *pkt_p,
                                                       const sx1280_mod_params_flrc_t *mod_p )
{
    uint32_t ceil_num = 0;

    ceil_num += sx1280_get_hdr_len_in_bits_flrc( pkt_p->hdr_type );
    ceil_num += sx1280_get_tail_len_in_bits_flrc( mod_p->cr );
    ceil_num += (pkt_p->pld_len_in_bytes + sx1280_get_crc_len_in_bytes_flrc( pkt_p->crc_type )) <<
                3;

    // Divide by coding rate, rounding up
    ceil_num <<= 2;
    const uint32_t ceil_den = sx1280_get_cr_den_flrc( mod_p->cr );
    uint32_t numerator = ((ceil_num + ceil_den - 1) / ceil_den);

    numerator += sx1280_get_pbl_len_in_bits_gfsk_flrc( pkt_p->pbl_len ) + 21;
    numerator += sx1280_get_sync_word_len_in_bytes_flrc( pkt_p->sync_word_len ) << 3;

    return numerator;
}

/* --- EOF ------------------------------------------------------------------ */
