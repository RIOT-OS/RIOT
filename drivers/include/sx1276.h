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

#ifndef SX1276_H
#define SX1276_H

#define SX1276_RADIO_WAKEUP_TIME                           1000        /**< [us] */
#define SX1276_RX_BUFFER_SIZE                              256
#define SX1276_RF_MID_BAND_THRESH                          525000000
#define SX1276_CHANNEL_HF                                  868000000

#define SX1276_EVENT_HANDLER_STACK_SIZE 2048

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
    sx1276_rx_packet_t last_packet;         /**< Last packet that was received */
    uint32_t last_channel;                  /**< Last channel in frequency hopping sequence */
    bool is_last_cad_success;               /**< Sign of success of last CAD operation (activity detected) */

    /* DIO lines events handler outside ISR context */
    kernel_pid_t dio_polling_thread_pid;            /**< SX1276 DIO interrupt lines handler thread PID */
    /**< SX1276 DIO interrupt lines events handler stack */
    uint8_t dio_polling_thread_stack[SX1276_EVENT_HANDLER_STACK_SIZE];

    /* Timers */
    xtimer_t tx_timeout_timer;              /**< TX operation timeout timer */
    xtimer_t rx_timeout_timer;              /**< RX operation timeout timer */
} sx1276_internal_t;

/**
 * SX1276 hardware and global parameters.
 */
typedef struct sx1276_s {
    spi_t spi;                                                          /**< SPI */
    gpio_t nss_pin;                                                     /**< SPI NSS pin */

    gpio_t reset_pin;                                                   /**< Reset pin */
    gpio_t dio0_pin;                                                    /**< Interrupt line DIO0 */
    gpio_t dio1_pin;                                                    /**< Interrupt line DIO1 */
    gpio_t dio2_pin;                                                    /**< Interrupt line DIO2 */
    gpio_t dio3_pin;                                                    /**< Interrupt line DIO3 */
    gpio_t dio4_pin;                                                    /**< Interrupt line DIO4 (not used) */
    gpio_t dio5_pin;                                                    /**< Interrupt line DIO5 (not used) */

    sx1276_settings_t settings;                                         /**< Transceiver settings */

    void (*sx1276_event_cb)(void *dev, sx1276_event_type_t event_type); /**< Event callback */
    void *callback_arg;                                                 /**< User-defined callback argument */

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
 * @brief generates 32 bits random value based on the RSSI readings
 * This function sets the radio in LoRa mode and disables all interrupts from it.
 * After calling this function either sx1276_set_rx_config or sx1276_set_tx_config
 * functions must be called.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @return random 32 bits value
 */
uint32_t sx1276_random(sx1276_t *dev);

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
uint32_t sx1276_get_time_on_air(sx1276_t *dev, sx1276_radio_modems_t modem, uint8_t pkt_len);

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
void sx1276_set_rx(sx1276_t *dev, uint32_t timeout);

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
 * @brief Writes the radio register at specified address.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr Register address
 *
 * @param	[IN]	data New register value
 */
void sx1276_reg_write(sx1276_t *dev, uint8_t addr, uint8_t data);

/**
 * @brief Reads the radio register at specified address.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr	Register address
 *
 * @return	Register value
 */
uint8_t sx1276_reg_read(sx1276_t *dev, uint8_t addr);

/**
 * @brief Writes multiple radio registers starting at address (burst-mode).
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr	First radio register address
 *
 * @param	[IN]	buffer	buffer containing the new register's values
 *
 * @param	[IN]	size	Number of registers to be written
 */
void sx1276_reg_write_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                            uint8_t size);

/**
 * @brief Reads multiple radio registers starting at address.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr	First radio register address
 *
 * @param	[OUT]	bufer	Buffer where to copy registers data
 *
 * @param	[IN]	size	Number of registers to be read
 */
void sx1276_reg_read_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                           uint8_t size);

/**
 * @brief Sets the maximum payload length.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	modem	Modem to use
 *
 * @param	[IN]	maxlen	Maximum payload length in bytes
 */
void sx1276_set_max_payload_len(sx1276_t *dev, sx1276_radio_modems_t modem, uint8_t maxlen);

/**
 * @brief sx1276 state machine hanlder thread body.
 *
 * @param	[IN]	arg	an sx1276 device instance
 */
void *dio_polling_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* SX1276_H */

/** @} */
