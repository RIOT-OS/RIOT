/**
 * @file      ral_sx1280.h
 *
 * @brief     SX1280 radio abstraction layer definition
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

#ifndef __RAL_SX1280_H__
#define __RAL_SX1280_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "periph/gpio.h"
#include "ral_defs.h"

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/**
 * Radio initialization
 *
 * @param [in] radio Pointer to radio daa to be initialized
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_init( const ral_t *ral );

/**
 * Setup radio to transmit and receive data using GFSK modem
 *
 * @remark When transmitting in GFSK mode the radio access may be blocking
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params GFSK modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_gfsk( const ral_t *ral, const ral_params_gfsk_t *params );

/**
 * Setup radio to receive data using GFSK modem
 *
 * @remark When receiving in GFSK mode the radio access may be blocking
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params GFSK modem reception parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_rx_gfsk( const ral_t *ral, const ral_params_gfsk_t *params );

/**
 * Setup radio to transmit data using GFSK modem
 *
 * @remark When transmitting in GFSK mode the radio access may be blocking
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params GFSK modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_tx_gfsk( const ral_t *ral, const ral_params_gfsk_t *params );

/**
 * Setup radio to transmit and receive data using LoRa modem
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params LoRa modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_lora( const ral_t *ral, const ral_params_lora_t *params );

/**
 * Setup radio to receive data using LoRa modem
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params LoRa modem reception parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_rx_lora( const ral_t *ral, const ral_params_lora_t *params );

/**
 * Setup radio to transmit data using LoRa modem
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params LoRa modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_tx_lora( const ral_t *ral, const ral_params_lora_t *params );

/**
 * Setup radio to transmit and receive data using FLRC modem
 *
 * @remark Not all radios have this modem available
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params FLRC modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_flrc( const ral_t *ral, const ral_params_flrc_t *params );

/**
 * Setup radio to receive data using FLRC modem
 *
 * @remark Not all radios have this modem available
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params FLRC modem reception parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_rx_flrc( const ral_t *ral, const ral_params_flrc_t *params );

/**
 * Setup radio to transmit data using FLRC modem
 *
 * @remark Not all radios have this modem available
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params FLRC modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_tx_flrc( const ral_t *ral, const ral_params_flrc_t *params );

/**
 * Setup radio to transmit data using LoRaE modem
 *
 * @remark Not all radios have this modem available
 *
 * @remark When transmitting in LoRaE mode the radio access may be blocking
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params LoRaE modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_tx_lora_e( const ral_t *ral, const ral_params_lora_e_t *params );

/**
 * Setup radio and starts data transmission using BPSK modem
 *
 * @remark Not all radios have this modem available
 *
 * @remark When transmitting in BPSK mode the radio access is blocking
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params BPSK modem transmission parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_tx_bpsk( const ral_t *ral, const ral_params_bpsk_t *params );

/**
 * Setup radio in CAD mode
 *
 * @remark Not all radios have this feature available
 *
 * @param [in] radio Pointer to radio data
 * @param [in] params CAD operation parameters
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_setup_cad( const ral_t *ral, const ral_lora_cad_params_t *params );

/**
 * Fills radio transmission buffer
 *
 * @param [in] radio Pointer to radio data
 * @param [in] buffer Pointer to the buffer to be transmitted
 * @param [in] size   Size of the buffer to be transmitted
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_pkt_payload( const ral_t *ral, const uint8_t *buffer,
                                         const uint16_t size );

/**
 * Fetches radio reception buffer
 *
 * @param [in]  radio      Pointer to radio data
 * @param [out] buffer     Pointer to the buffer to be filled with received data
 * @param [in]  max_size   Size of user-provided buffer
 * @param [out] size       If non-zero, received packet size will be put here
 * @param [out] pkt_status Pointer a structure that will contain the Rx packet
 * status
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_pkt_payload( const ral_t *ral, uint8_t *buffer, uint16_t max_size,
                                         uint16_t *size );

/**
 * Fetches packet status
 *
 * @param [in]  radio      Pointer to radio data
 * @param [out] pkt_status Pointer a structure that will contain the packet
 * status status
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_gfsk_pkt_status( const ral_t *ral,
                                             ral_rx_pkt_status_gfsk_t *pkt_status );

/**
 * Fetches packet status
 *
 * @param [in]  radio      Pointer to radio data
 * @param [out] pkt_status Pointer a structure that will contain the packet
 * status status
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_lora_pkt_status( const ral_t *ral,
                                             ral_rx_pkt_status_lora_t *pkt_status );

/**
 * Fetches incoming LoRa packet configuration
 *
 * @param [in]  radio         Pointer to radio data
 * @param [out] rx_cr         Pointer to a placeholder for coding rate value
 * @param [out] rx_is_crc_en  Pointer to a placeholder for CRC status
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_lora_incoming_pkt_config( const ral_t *ral, ral_lora_cr_t *rx_cr,
                                                      bool *rx_is_crc_en );

/**
 * Fetches packet status
 *
 * @param [in]  radio      Pointer to radio data
 * @param [out] pkt_status Pointer a structure that will contain the packet
 * status status
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_flrc_pkt_status( const ral_t *ral,
                                             ral_rx_pkt_status_flrc_t *pkt_status );

/**
 * Radio is set in Sleep mode
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_sleep( const ral_t *ral );

/**
 * Radio is set in Standby mode
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_standby( const ral_t *ral );

/**
 * Radio is set in Tx mode
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_tx( const ral_t *ral );

/**
 * Radio is set in Tx CW mode
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_tx_cw( const ral_t *ral );

/**
 * Radio is set in Rx mode
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_rx( const ral_t *ral, const uint32_t timeout_ms );

/**
 * Radio is set in CAD mode
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_cad( const ral_t *ral );

/**
 * Gets current radio irq status
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_irq_status( const ral_t *ral, ral_irq_t *ral_irq );

/**
 * Clears radio irq status
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_clear_irq_status( const ral_t *ral, const ral_irq_t ral_irq );

/**
 * Clears any radio irq status flags that are set and returns the flags that
 * were cleared.
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_and_clear_irq_status( const ral_t *ral, ral_irq_t *ral_irq );

/**
 * Enable interrupts. If the chip has several IRQ lines, the first is used.
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_set_dio_irq_params( const ral_t *ral, const ral_irq_t ral_irq );

/**
 * Retrives the current radio irq status
 *
 * @remark Must be the first function to be called by the radio IRQ handler
 *
 * @param [in] radio Pointer to radio data
 * @param [out] ral_irq Current radio irq status
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_process_irq( const ral_t *ral, ral_irq_t *ral_irq );

/**
 * Gets current RSSI value
 *
 * @param [in] radio Pointer to radio data
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_rssi( const ral_t *ral, int16_t *rssi );

/**
 * Gets time on air, in milliseconds
 *
 * @param [in] params Modem transmission parameters
 * @param [out] toa   Time on air
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_lora_time_on_air_in_ms( const ral_params_lora_t *params,
                                                    uint32_t *toa );

/**
 * Gets time on air, in milliseconds
 *
 * @param [in] params Modem transmission parameters
 * @param [out] toa   Time on air
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_gfsk_time_on_air_in_ms( const ral_params_gfsk_t *params,
                                                    uint32_t *toa );

/**
 * Gets time on air, in milliseconds
 *
 * @param [in] params Modem transmission parameters
 * @param [out] toa   Time on air
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_get_flrc_time_on_air_in_ms( const ral_params_flrc_t *params,
                                                    uint32_t *toa );

/**
 * Gets RX power consumption, in micro_ampere from bw
 *
 * @param [in] bw
 * @param [out] micro_ampere
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_convert_lora_rx_bw_to_ua( const ral_params_lora_t *params,
                                                  uint32_t *micro_ampere );

/**
 * Gets TX power consumption, in micro_ampere from TX dbm
 *
 * @param [in] pwr_in_dbm
 * @param [out] micro_ampere
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_convert_lora_tx_dbm_to_ua( const ral_params_lora_t *params,
                                                   uint32_t *micro_ampere );

/**
 * Gets register values
 *
 * @param [in]  radio Pointer to radio data
 * @param [in]  address Address of the first register to read
 * @param [out] buffer Pointer to store data
 * @param [in]  size Number of bytes to read
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_read_register( const ral_t *ral, uint16_t address, uint8_t *buffer,
                                       uint16_t size );

/**
 * Write register values
 *
 * @param [in] radio Pointer to radio data
 * @param [in] address Address of the first register to write
 * @param [in] buffer Pointer to store data
 * @param [in] size Number of bytes to write
 *
 * @retval status Operation status
 */
ral_status_t ral_sx1280_write_register( const ral_t *ral, uint16_t address, uint8_t *buffer,
                                        uint16_t size );

#ifdef __cplusplus
}
#endif


/*
 * -----------------------------------------------------------------------------
 * --- ADDONS        -----------------------------------------------------------
 */

#include "sx1280_regs.h"
#include "sx1280.h"
#include <stddef.h>

/*!
 * \brief The radio callbacks structure
 * Holds function pointers to be called on radio interrupts
 */
typedef struct {
    void (*txDone)( void );         //!< Pointer to a function run on successful transmission
    void (*rxDone)( void );         //!< Pointer to a function run on successful reception
    void (*rxHeaderDone)( void );   //!< Pointer to a function run on successful Header reception
    void (*rxTimeout)( void );      //!< Pointer to a function run on reception timeout
    void (*rxError)( void );        //!< Pointer to a function run on reception error
} radio_callbacks_t;


/* LOW LEVEL HAL ADDONS */

void sx1280_hal_setup_io( const radio_callbacks_t *cb  );
void sx1280_hal_setup_irq( gpio_cb_t evt_handler );
void sx1280_hal_dispatch( const ral_irq_t irq_status );

/* END */


static const ral_t ral_default_cfg = {
    .context = NULL,
    .radio_type = RAL_RADIO_SX1280,
    .tcxo_cfg = { .tcxo_ctrl_mode = RAL_TCXO_NONE }
};


static inline uint16_t ral_sx1280_get_firmware_version( const ral_t *ral )
{
    uint16_t res = 0;

    ral_sx1280_read_register(ral, SX1280_REG_FW_VERSION, (uint8_t *)&res, 2);
    return res;
}


static inline ral_status_t ral_sx1280_init_all(const ral_t *ral, gpio_cb_t evt_handler,
                                               const radio_callbacks_t *cb)
{
    ral_status_t res = RAL_STATUS_ERROR;

    sx1280_hal_setup_io(cb);
    res = ral_sx1280_init(ral);
    if (res == RAL_STATUS_OK) {
        uint16_t fwid = ral_sx1280_get_firmware_version(ral);
        printf("SX1280 firmware version = %x\n", fwid);
        if (fwid == 0x5853) {
            sx1280_hal_setup_irq(evt_handler);
            sx1280_set_lna_settings(ral->context, SX1280_LNA_HIGH_SENSITIVITY_MODE);
        }
        else {
            res = RAL_STATUS_UNKNOWN_VALUE;
        }
    }

    return res;
}


static inline void ral_sx1280_proces_lora_irqs( const ral_t *ral  )
{

    ral_irq_t irq_status = RAL_IRQ_NONE;

    ral_sx1280_process_irq( ral, &irq_status );
    ral_sx1280_clear_irq_status(ral, RAL_IRQ_ALL);

    sx1280_hal_dispatch(irq_status);
} /* ral_sx1280_proces_lora_irqs */


#endif  // __RAL_SX1280_H__

/* --- EOF ------------------------------------------------------------------ */
