/*!
 * @file      sx1280_hal.h
 *
 * @brief     Hardware Abstraction Layer for SX1280
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

#ifndef __SX1280_HAL_H__
#define __SX1280_HAL_H__

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

#ifndef SX1280_PARAM_SPI
#define SX1280_PARAM_SPI (SPI_DEV(0))
#endif

// SPI CS
#ifndef SX1280_PARAM_SPI_NSS
#define SX1280_PARAM_SPI_NSS ARDUINO_PIN_10
#endif

// Optional RESET
#ifndef SX1280_PARAM_RESET
#define SX1280_PARAM_RESET ARDUINO_PIN_7
#endif

// Busy PIN
#ifndef SX1280_PARAM_DIO0
#define SX1280_PARAM_DIO0 ARDUINO_PIN_6
// TODO Check PIN #define SX1280_PARAM_DIO0 ARDUINO_PIN_8
#endif

// IRQ
#ifndef SX1280_PARAM_DIO1
#define SX1280_PARAM_DIO1 ARDUINO_PIN_9
#endif

#ifndef SX1280_PARAM_DIO2
#define SX1280_PARAM_DIO2 GPIO_UNDEF
#endif

#ifndef SX1280_PARAM_DIO3
#define SX1280_PARAM_DIO3 GPIO_UNDEF
#endif


/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

typedef enum sx1280_hal_status_e {
    SX1280_HAL_STATUS_OK    = 0,
    SX1280_HAL_STATUS_ERROR = 3,
} sx1280_hal_status_t;

typedef enum sx1280_hal_operating_mode_e {
    SX1280_HAL_OP_MODE_SLEEP = 0x00,    //! The radio is in sleep mode
    SX1280_HAL_OP_MODE_STDBY_RC,        //! The radio is in standby mode with RC
                                        //! oscillator
    SX1280_HAL_OP_MODE_STDBY_XOSC,      //! The radio is in standby mode with XOSC
                                        //! oscillator
    SX1280_HAL_OP_MODE_FS,              //! The radio is in frequency synthesis mode
    SX1280_HAL_OP_MODE_TX,              //! The radio is in transmit mode
    SX1280_HAL_OP_MODE_RX,              //! The radio is in receive single mode
    SX1280_HAL_OP_MODE_RX_C,            //! The radio is in receive continuous mode
    SX1280_HAL_OP_MODE_RX_DC,           //! The radio is in receive duty cycle mode
    SX1280_HAL_OP_MODE_CAD              //! The radio is in channel activity detection mode
} sx1280_hal_operating_mode_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/*!
 * Radio data transfer - write
 *
 * \remark Must be implemented by the upper layer
 * \remark Do not forget to call \ref sx1280_hal_wakeup function at the very
 * beginning of the implementation.
 *
 * \param [in] context          Radio implementation parameters
 * \param [in] command          Pointer to the buffer to be transmitted
 * \param [in] command_length   Buffer size to be transmitted
 * \param [in] data             Pointer to the buffer to be transmitted
 * \param [in] data_length      Buffer size to be transmitted
 *
 * \retval status     Operation status
 */
sx1280_hal_status_t sx1280_hal_write( const void *context, const uint8_t *command,
                                      const uint16_t command_length,
                                      const uint8_t *data, const uint16_t data_length );

/*!
 * Radio data transfer - read
 *
 * \remark Must be implemented by the upper layer
 * \remark Do not forget to call \ref sx1280_hal_wakeup function at the very
 * beginning of the implementation.
 *
 * \param [in] context          Radio implementation parameters
 * \param [in] command          Pointer to the buffer to be transmitted
 * \param [in] command_length   Buffer size to be transmitted
 * \param [in] data             Pointer to the buffer to be received
 * \param [in] data_length      Buffer size to be received
 *
 * \retval status     Operation status
 */
sx1280_hal_status_t sx1280_hal_read( const void *context, const uint8_t *command,
                                     const uint16_t command_length,
                                     uint8_t *data, const uint16_t data_length );

/*!
 * Reset the radio
 *
 * \remark Must be implemented by the upper layer
 *
 * \param [in] context Radio implementation parameters
 */
void sx1280_hal_reset( const void *context );

/*!
 * Wake the radio up.
 *
 * \remark Must be implemented by the upper layer
 *
 * \param [in] context Radio implementation parameters

 * \retval status    Operation status
 */
sx1280_hal_status_t sx1280_hal_wakeup( const void *context );

/*!
 * Get radio operating mode
 *
 * \remark Must be implemented by the upper layer
 *
 * \param [in] context Radio implementation parameters
 * \retval     op_mode Radio current operating mode
 */
sx1280_hal_operating_mode_t sx1280_hal_get_operating_mode( const void *context );

/*!
 * Set radio operating mode
 *
 * \remark Must be implemented by the upper layer
 *
 * \param [in] context Radio implementation parameters
 * \param [in] op_mode Radio operating mode
 */
void sx1280_hal_set_operating_mode( const void *context,
                                    const sx1280_hal_operating_mode_t op_mode );

#ifdef __cplusplus
}
#endif

#endif  // __SX1280_HAL_H__

/* --- EOF ------------------------------------------------------------------ */
