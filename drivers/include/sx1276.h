/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sx1276 SX1276
 * @brief       Semtech SX1276
 * @{
 * @file
 * @brief       Public interface for SX1276 driver
 * @author      Eugene P. [ep@unwds.com]
 */

#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "net/netdev2.h"
#include "net/netdev2/lorawan.h"

#ifndef SX1276_H
#define SX1276_H

#define SX1276_RADIO_WAKEUP_TIME                           1000        /**< [us] */
#define SX1276_RX_BUFFER_SIZE                              256
#define SX1276_RF_MID_BAND_THRESH                          525000000
#define SX1276_CHANNEL_HF                                  868000000

#define SX1276_EVENT_HANDLER_STACK_SIZE 2048

#define SX1276_IRQ_DIO0 (1<<0)
#define SX1276_IRQ_DIO1 (1<<1)
#define SX1276_IRQ_DIO2 (1<<2)
#define SX1276_IRQ_DIO3 (1<<3)
#define SX1276_IRQ_DIO4 (1<<4)
#define SX1276_IRQ_DIO5 (1<<5)

/**
 * @name SX1276 configuration
 * @{
 */
#define RF_FREQUENCY                                915000000   // Hz, 868.9MHz

#define TX_OUTPUT_POWER                             10          // dBm

#define LORA_PREAMBLE_LENGTH                        8           // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         10          // Symbols

#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION                           false

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Radio driver supported modems.
 */
typedef enum {
    SX1276_MODEM_FSK = 0, 
    SX1276_MODEM_LORA,
} sx1276_radio_modems_t;

/**
 * LoRa modulation bandwidth.
 */
typedef enum {
    SX1276_BW_125_KHZ = 7,
    SX1276_BW_250_KHZ,
    SX1276_BW_500_KHZ
} sx1276_lora_bandwidth_t;

/**
 * LoRa modulation spreading factor.
 */
typedef enum {
    SX1276_SF7 = 7,
    SX1276_SF8,
    SX1276_SF9,
    SX1276_SF10,
    SX1276_SF11,
    SX1276_SF12
} sx1276_lora_spreading_factor_t;

/**
 * LoRa modulation coding rate.
 */
typedef enum {
    SX1276_CR_4_5 = 1,
    SX1276_CR_4_6,
    SX1276_CR_4_7,
    SX1276_CR_4_8
} sx1276_lora_coding_rate_t;

/**
 * LoRa configuration structure.
 */
typedef struct {
    uint8_t power;
    sx1276_lora_bandwidth_t bandwidth;
    sx1276_lora_spreading_factor_t datarate;
    bool low_datarate_optimize;
    sx1276_lora_coding_rate_t coderate;
    uint16_t preamble_len;
    bool implicit_header;
    uint8_t payload_len;
    bool crc_on;
    bool freq_hop_on;
    uint8_t hop_period;
    bool iq_inverted;
    bool rx_continuous;
    uint32_t tx_timeout;
    uint32_t rx_timeout; /**< RX timeout in symbols */
} sx1276_lora_settings_t;

/**
 * LoRa received packet.
 */
typedef struct {
    uint8_t snr_value;      /**< Packet's signal-to-noise rate (SNR) */
    int16_t rssi_value;     /**< Packet's RSSI */
    uint8_t size;           /**< Packet's actual size in bytes */

    uint8_t content[256];   /**< Packet's content */
} sx1276_rx_packet_t;

/**
 * Radio driver internal state machine states definition.
 */
typedef enum {
    SX1276_RF_IDLE = 0,
    
    SX1276_RF_RX_RUNNING,
    SX1276_RF_TX_RUNNING,
    
    SX1276_RF_CAD,
} sx1276_radio_state_t;

/**
 * Radio settings.
 */
typedef struct {
    sx1276_radio_state_t state;
    uint32_t channel;
    sx1276_lora_settings_t lora;
    sx1276_radio_modems_t modem;
    uint32_t window_timeout;
    uint8_t time_on_air_pkt_len;
} sx1276_settings_t;

typedef enum {
    SX1276_RX_DONE = 0,
    SX1276_TX_DONE,

    SX1276_RX_TIMEOUT,
    SX1276_TX_TIMEOUT,

    SX1276_RX_ERROR_CRC,

    SX1276_FHSS_CHANGE_CHANNEL,
    SX1276_CAD_DONE,

} sx1276_event_type_t;

/***
 * SX1276 internal data.
 */
typedef struct {
    /* Data that will be passed to events handler in application */
    uint32_t last_channel;                  /**< Last channel in frequency hopping sequence */
    bool is_last_cad_success;               /**< Sign of success of last CAD operation (activity detected) */
    xtimer_t tx_timeout_timer;              /**< TX operation timeout timer */
    xtimer_t rx_timeout_timer;              /**< RX operation timeout timer */
} sx1276_internal_t;

/**
 * SX1276 hardware and global parameters.
 */
typedef struct {
    spi_t spi;                                                          /**< SPI */
    gpio_t nss_pin;                                                     /**< SPI NSS pin */

    gpio_t reset_pin;                                                   /**< Reset pin */
    gpio_t dio0_pin;                                                    /**< Interrupt line DIO0 */
    gpio_t dio1_pin;                                                    /**< Interrupt line DIO1 */
    gpio_t dio2_pin;                                                    /**< Interrupt line DIO2 */
    gpio_t dio3_pin;                                                    /**< Interrupt line DIO3 */
    gpio_t dio4_pin;                                                    /**< Interrupt line DIO4 (not used) */
    gpio_t dio5_pin;                                                    /**< Interrupt line DIO5 (not used) */

} sx1276_params_t;

typedef uint8_t sx1276_flags_t;

typedef struct sx1276_s {
    netdev2_lorawan_t netdev;
    sx1276_settings_t settings;                                         /**< Transceiver settings */
    sx1276_params_t params;
    sx1276_flags_t irq;

    sx1276_internal_t _internal;                                        /**< Internal sx1276 data used within the driver */
} sx1276_t;

/**
 * SX1276 initialization result.
 */
typedef enum {
    SX1276_INIT_OK = 0,         /**< Initialization was successful */
    SX1276_ERR_SPI,             /**< Failed to initialize SPI bus or CS line */
    SX1276_ERR_TEST_FAILED,     /**< SX1276 testing failed during initialization (check chip) */
    SX1276_ERR_THREAD           /**< Unable to create DIO handling thread (check amount of free memory) */
} sx1276_init_result_t;

/**
 * Hardware IO IRQ callback function definition.
 */
typedef void (sx1276_dio_irq_handler_t)(sx1276_t *dev);

/**
 * SX1276 definitions.
 */
#define SX1276_XTAL_FREQ       32000000 /**< 32MHz */
#define SX1276_FREQ_STEP       61.03515625

/**
 * Public functions prototypes.
 */

/**
 * @brief Tests the transceiver.
 *
 * @param	[IN]	dev	The sx1276 device structure pointer
 * @return true if test passed, false otherwise
 */
bool sx1276_test(sx1276_t *dev);

/**
 * @brief Initializes the transceiver.
 *
 * @param	[IN]	dev					The sx1276 device structure pointer
 *
 * @return result of initialization (see @sx1276_init_result_t struct)
 */

sx1276_init_result_t sx1276_init(sx1276_t *dev);

/**
 * @brief Gets current status of transceiver.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @return radio status [RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
sx1276_radio_state_t sx1276_get_status(sx1276_t *dev);

/**
 * @brief Configures the radio with the given modem.
 *
 * @param [IN] modem Modem to be used [0: FSK, 1: LoRa]
 */
void sx1276_set_modem(sx1276_t *dev, sx1276_radio_modems_t modem);

/**
 * @brief Gets the channel frequency.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 */

uint8_t sx1276_get_syncword(sx1276_t *dev);

void sx1276_set_syncword(sx1276_t *dev, uint8_t syncword);
uint32_t sx1276_get_channel(sx1276_t *dev);
/**
 * @brief Sets the channel frequency.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 */
void sx1276_set_channel(sx1276_t *dev, uint32_t freq);

/**
 * @brief Checks that channel is free with specified RSSI threshold.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 * @param	[IN]	freq channel RF frequency
 * @param	[IN]	rssi_thresh RSSI treshold
 *
 * @return channel is free or not [true: channel is free, false: channel is not free]
 */
bool sx1276_is_channel_free(sx1276_t *dev, uint32_t freq, int16_t rssi_thresh);

/**
 * @brief Sets up the LoRa modem configuration.
 *
 * @param	[IN]	dev			The sx1276 device pointer
 * @param	[IN]	settings	The LoRa modem settings structure
 */
void sx1276_configure_lora(sx1276_t *dev, sx1276_lora_settings_t *settings);

/**
 * @brief Sets up the LoRa modem bandwidth settings.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 * @param	[IN]	bw		LoRa modulation bandwidth
 */
void sx1276_configure_lora_bw(sx1276_t *dev, sx1276_lora_bandwidth_t bw);

/**
 * @brief Sets up the LoRa modem spreading factor settings.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 * @param	[IN]	sf		LoRa modulation spreading factor
 */
void sx1276_configure_lora_sf(sx1276_t *dev, sx1276_lora_spreading_factor_t sf);

/**
 * @brief Sets up the LoRa modem coding rate settings.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 * @param	[IN]	cr		LoRa modulation bandwidth
 */
void sx1276_configure_lora_cr(sx1276_t *dev, sx1276_lora_coding_rate_t cr);

/**
 * @brief Computes the packet time on air in us for the given payload.
 * Can only be called once sx1276_configure_lora have been called
 *
 * @param	[IN]	dev			The sx1276 device structure pointer
 *
 * @param	[IN]	modem		Modem to use
 *
 * @param	[IN]	pk_len		Packet payload length
 *
 * @return computed air time (us) for the given packet payload length
 */
uint32_t sx1276_get_time_on_air(sx1276_t *dev);

/**
 * @brief Sends the buffer of size. Prepares the packet to be sent and sets
 *        the radio in transmission
 *
 * @param	[IN]	dev			The sx1276 device structure pointer
 *
 * @param	[IN]	buffer		Buffer pointer
 *
 * @param	[IN]	size		Buffer size
 */
void sx1276_send(sx1276_t *dev, uint8_t *buffer, uint8_t size);

/**
 * @brief Sets the radio in sleep mode
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 */
void sx1276_set_sleep(sx1276_t *dev);

/**
 * @brief Sets the radio in stand-by mode
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 */
void sx1276_set_standby(sx1276_t *dev);

/**
 * @brief Sets the radio in reception mode for given amount of time.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	timeout	reception timeout [us] [0: continuous, others: timeout]
 */
void sx1276_set_rx(sx1276_t *dev);

/**
 * @brief Sets the radio in transmission mode for given amount of time.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	timeout	reception timeout [us] [0: continuous, others: timeout]
 */
void sx1276_set_tx(sx1276_t *dev, uint32_t timeout);

/**
 * @brief Start a channel activity detection.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 */

void sx1276_start_cad(sx1276_t *dev);

/**
 * @brief Reads the current RSSI value.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @return current value of RSSI in [dBm]
 */
int16_t sx1276_read_rssi(sx1276_t *dev);

/**
 * @brief Sets the maximum payload length.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	modem	Modem to use
 *
 * @param	[IN]	maxlen	Maximum payload length in bytes
 */
void sx1276_set_max_payload_len(sx1276_t *dev, uint8_t maxlen);

/**
 * @brief sx1276 state machine hanlder thread body.
 *
 * @param	[IN]	arg	an sx1276 device instance
 */
void *dio_polling_thread(void *arg);

void sx1276_on_dio0(void *arg);
void sx1276_on_dio1(void *arg);
void sx1276_on_dio2(void *arg);
void sx1276_on_dio3(void *arg);
void sx1276_on_dio4(void *arg);
void sx1276_on_dio5(void *arg);
void sx1276_on_dio6(void *arg);

void sx1276_set_status(sx1276_t *dev, sx1276_radio_state_t state);

/**
 * @brief Sets the SX1276 operating mode
 *
 * @param [IN] op_mode New operating mode
 */
void sx1276_set_op_mode(sx1276_t *dev, uint8_t op_mode);

/**
 * @brief Gets the SX1276 operating mode
 */
uint8_t sx1276_get_op_mode(sx1276_t *dev);


/**
 * @brief Resets the SX1276
 */
void sx1276_reset(sx1276_t *dev);
uint32_t sx1276_random(sx1276_t *dev);

void init_configs(sx1276_t *dev);

sx1276_lora_bandwidth_t sx1276_get_bandwidth(sx1276_t *dev);

sx1276_lora_spreading_factor_t sx1276_get_spreading_factor(sx1276_t *dev);

sx1276_lora_coding_rate_t sx1276_get_coding_rate(sx1276_t *dev);

void sx1276_set_rx_single(sx1276_t *dev, uint8_t single);
uint8_t sx1276_get_rx_single(sx1276_t *dev);


void sx1276_set_crc(sx1276_t *dev, bool crc);
bool sx1276_get_crc(sx1276_t *dev);

uint8_t sx1276_get_hop_period(sx1276_t *dev);
void sx1276_set_hop_period(sx1276_t *dev, uint8_t hop_period);
void sx1276_set_implicit_mode(sx1276_t *dev, bool implicit);
bool  sx12376_get_implicit_mode(sx1276_t *dev);
void sx1276_set_payload_length(sx1276_t *dev, uint8_t len);
uint8_t sx1276_get_payload_length(sx1276_t *dev);
void sx1276_set_power(sx1276_t *dev, uint8_t power);
uint8_t sx1276_get_power(sx1276_t *dev);
void sx1276_set_preamble_length(sx1276_t *dev, uint16_t preamble);
uint16_t sx1276_get_preamble_length(sx1276_t *dev);
void sx1276_set_bandwidth(sx1276_t *dev, sx1276_lora_bandwidth_t bandwidth);
void sx1276_set_coding_rate(sx1276_t *dev, sx1276_lora_coding_rate_t coderate);
void sx1276_set_spreading_factor(sx1276_t *dev, sx1276_lora_spreading_factor_t sf);
void sx1276_set_symbol_timeout(sx1276_t *dev, uint16_t timeout);
void sx1276_set_rx_timeout(sx1276_t *dev, uint32_t timeout);
void sx1276_set_tx_timeout(sx1276_t *dev, uint32_t timeout);
void sx1276_set_iq_invert(sx1276_t *dev, bool iq_invert);
void sx1276_set_freq_hop(sx1276_t *dev, bool freq_hop_on);

void sx1276_setup(sx1276_t *dev, const sx1276_params_t *params);
#ifdef __cplusplus
}
#endif

#endif /* SX1276_H */

/** @} */
