/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SX127X_H
#define SX127X_H

/**
 * @defgroup    drivers_sx127x Semtech SX1272 and SX1276 radios driver
 * @ingroup     drivers_netdev
 * @brief       Driver for Semtech SX1272 and SX1276 radios.
 *
 * This module contains the driver for radio devices of the Semtech SX127x
 * series (SX1272 and SX1276).
 * Only LoRa long range modem is supported at the moment.
 *
 * SX127x modules are designed to be used in the ISM radio frequency (RF) band.
 * This RF band depends on different regional regulations worldwide.
 * Be careful to configure the device to use a RF frequency allowed in your
 * region.
 * Here is the list of allowed frequencies for your region (see
 * [LoRaWAN regional parameters document available online](https://www.lora-alliance.org/for-developers)):
 * - Europe has 2 allowed bands (ETSI):
 *   - EU863-870
 *   - EU433 (from 433.175MHZ to 434.665MHZ exactly)
 * - US is US902-928
 * - China has 2 allowed bands:
 *   - CN779-787 (from 779.5MHz to 786.5MHz exactly)
 *   - CN470-510 (from 470.3MHz to 509.7MHz exactly)
 * - Australia is AU915-928
 * - South asia, AS923:
 *   - Bruneï [923-925 MHz]
 *   - Cambodia [923-925 MHz]
 *   - Hong Kong [920-925 MHz]
 *   - Indonesia [923-925 MHz]
 *   - Japan [920-928 MHz]
 *   - Laos [923-925 MHz]
 *   - New Zealand [915-928 MHz]
 *   - Singapore [920-925 MHz]
 *   - Taiwan [922-928 MHz]
 *   - Thailand [920-925 MHz]
 *   - Vietnam [920-925 MHz]
 * - South Korea: KR920-923 (from 920.9MHz to 923.3MHz exactly)
 *
 * For more information on Semtech SX1272 and SX1276 modules see:
 * - [SX1272/73 datasheet](https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a/440000001NCE/v_VBhk1IolDgxwwnOpcS_vTFxPfSEPQbuneK3mWsXlU)
 * - [SX1276/77/78/79 datasheet](https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a/2R0000001OKs/Bs97dmPXeatnbdoJNVMIDaKDlQz8q1N_gxDcgqi7g2o)
 *
 * @{
 *
 * @file
 * @brief       Public interface for SX127X driver
 * @author      Eugene P. <ep@unwds.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "timex.h"
#include "ztimer.h"
#include "net/netdev.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SX127X device default configuration
 * @{
 */
#define SX127X_MODEM_DEFAULT             (SX127X_MODEM_LORA)    /**< Use LoRa as default modem */
#define SX127X_CHANNEL_DEFAULT           (868300000UL)          /**< Default channel frequency, 868.3MHz (Europe) */
#define SX127X_HF_CHANNEL_DEFAULT        (868000000UL)          /**< Use to calibrate RX chain for LF and HF bands */
#define SX127X_RF_MID_BAND_THRESH        (525000000UL)          /**< Mid-band threshold */
#define SX127X_XTAL_FREQ                 (32000000UL)           /**< Internal oscillator frequency, 32MHz */
#define SX127X_RADIO_WAKEUP_TIME         (1U)                   /**< In milliseconds [ms] */

#define SX127X_TX_TIMEOUT_DEFAULT        (30 * MS_PER_SEC)      /**< TX timeout, 30s */
#define SX127X_RX_SINGLE                 (false)                /**< Single byte receive mode => continuous by default */
#define SX127X_RX_BUFFER_SIZE            (256)                  /**< RX buffer size */
#define SX127X_RADIO_TX_POWER            (14U)                  /**< Radio power in dBm */

#define SX127X_EVENT_HANDLER_STACK_SIZE  (2048U)                /**< Stack size event handler */
#define SX127X_IRQ_DIO0                  (1 << 0)               /**< DIO0 IRQ */
#define SX127X_IRQ_DIO1                  (1 << 1)               /**< DIO1 IRQ */
#define SX127X_IRQ_DIO2                  (1 << 2)               /**< DIO2 IRQ */
#define SX127X_IRQ_DIO3                  (1 << 3)               /**< DIO3 IRQ */
#define SX127X_IRQ_DIO4                  (1 << 4)               /**< DIO4 IRQ */
#define SX127X_IRQ_DIO5                  (1 << 5)               /**< DIO5 IRQ */
/** @} */

/**
 * @defgroup drivers_sx127x_config     Semtech SX1272 and SX1276 driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief   GPIO mode of DIOx Pins.
 */
#ifndef SX127X_DIO_PULL_MODE
#define SX127X_DIO_PULL_MODE             (GPIO_IN_PD)
#endif
/** @} */

/**
 * @brief   SX127X initialization result.
 */
enum {
    SX127X_INIT_OK = 0,                 /**< Initialization was successful */
    SX127X_ERR_SPI,                     /**< Failed to initialize SPI bus or CS line */
    SX127X_ERR_GPIOS,                   /**< Failed to initialize GPIOs */
    SX127X_ERR_NODEV                    /**< No valid device version found */
};

/**
 * @brief   Radio driver supported modems.
 */
enum {
    SX127X_MODEM_FSK = 0,               /**< FSK modem driver */
    SX127X_MODEM_LORA,                  /**< LoRa modem driver */
};

/**
 * @brief   Radio driver internal state machine states definition.
 */
enum {
    SX127X_RF_IDLE = 0,                 /**< Idle state */
    SX127X_RF_RX_RUNNING,               /**< Sending state */
    SX127X_RF_TX_RUNNING,               /**< Receiving state */
    SX127X_RF_CAD,                      /**< Channel activity detection state */
};

/**
 * @brief   Event types.
 */
enum {
    SX127X_RX_DONE = 0,                 /**< Receiving complete */
    SX127X_TX_DONE,                     /**< Sending complete*/
    SX127X_RX_TIMEOUT,                  /**< Receiving timeout */
    SX127X_TX_TIMEOUT,                  /**< Sending timeout */
    SX127X_RX_ERROR_CRC,                /**< Receiving CRC error */
    SX127X_FHSS_CHANGE_CHANNEL,         /**< Channel change */
    SX127X_CAD_DONE,                    /**< Channel activity detection complete */
};

/**
 * @brief Power amplifier modes
 *
 * Default value is SX127X_PA_RFO.
 *
 * The power amplifier mode depends on the module hardware configuration.
 */
enum {
    SX127X_PA_RFO = 0,                  /**< RFO HF or RFO LF */
    SX127X_PA_BOOST,                    /**< Power amplifier boost (high power) */
};

/**
 * @name    SX127X device descriptor boolean flags
 * @{
 */
#define SX127X_LOW_DATARATE_OPTIMIZE_FLAG       (1 << 0)
#define SX127X_ENABLE_FIXED_HEADER_LENGTH_FLAG  (1 << 1)
#define SX127X_ENABLE_CRC_FLAG                  (1 << 2)
#define SX127X_CHANNEL_HOPPING_FLAG             (1 << 3)
#define SX127X_IQ_INVERTED_FLAG                 (1 << 4)
#define SX127X_RX_CONTINUOUS_FLAG               (1 << 5)
/** @} */

/**
 * @brief   LoRa configuration structure.
 */
typedef struct {
    uint16_t preamble_len;              /**< Length of preamble header */
    int8_t power;                       /**< Signal power */
    uint8_t bandwidth;                  /**< Signal bandwidth */
    uint8_t datarate;                   /**< Spreading factor rate, e.g datarate */
    uint8_t coderate;                   /**< Error coding rate */
    uint8_t freq_hop_period;            /**< Frequency hop period */
    uint8_t flags;                      /**< Boolean flags */
    uint32_t rx_timeout;                /**< RX timeout in milliseconds */
    uint32_t tx_timeout;                /**< TX timeout in milliseconds */
} sx127x_lora_settings_t;

/**
 * @brief   Radio settings.
 */
typedef struct {
    uint32_t channel;                   /**< Radio channel */
    uint8_t state;                      /**< Radio state */
    uint8_t modem;                      /**< Driver model (FSK or LoRa) */
    sx127x_lora_settings_t lora;        /**< LoRa settings */
} sx127x_radio_settings_t;

/**
 * @brief   SX127X internal data.
 */
typedef struct {
    /* Data that will be passed to events handler in application */
    ztimer_t tx_timeout_timer;          /**< TX operation timeout timer */
    ztimer_t rx_timeout_timer;          /**< RX operation timeout timer */
    uint32_t last_channel;              /**< Last channel in frequency hopping sequence */
    bool is_last_cad_success;           /**< Sign of success of last CAD operation (activity detected) */
} sx127x_internal_t;

/**
 * @brief   SX127X hardware and global parameters.
 */
typedef struct {
    spi_t spi;                          /**< SPI device */
    gpio_t nss_pin;                     /**< SPI NSS pin */
    gpio_t reset_pin;                   /**< Reset pin */
    gpio_t dio0_pin;                    /**< Interrupt line DIO0 (Tx done) */
    gpio_t dio1_pin;                    /**< Interrupt line DIO1 (Rx timeout) */
    gpio_t dio2_pin;                    /**< Interrupt line DIO2 (FHSS channel change) */
    gpio_t dio3_pin;                    /**< Interrupt line DIO3 (CAD done) */
    gpio_t dio4_pin;                    /**< Interrupt line DIO4 (not used) */
    gpio_t dio5_pin;                    /**< Interrupt line DIO5 (not used) */
#if defined(SX127X_USE_TX_SWITCH) || defined(SX127X_USE_RX_SWITCH)
    gpio_t rx_switch_pin;               /**< Rx antenna switch */
    gpio_t tx_switch_pin;               /**< Tx antenna switch */
#endif
    uint8_t paselect;                   /**< Power amplifier mode (RFO or PABOOST) */
} sx127x_params_t;

/**
 * @brief   SX127X IRQ flags.
 */
typedef uint8_t sx127x_flags_t;

/**
 * @brief   SX127X device descriptor.
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                    /**< Netdev parent struct */
    sx127x_radio_settings_t settings;   /**< Radio settings */
    sx127x_params_t params;             /**< Device driver parameters */
    sx127x_internal_t _internal;        /**< Internal sx127x data used within the driver */
    sx127x_flags_t irq;                 /**< Device IRQ flags */
} sx127x_t;

/**
 * @brief   Hardware IO IRQ callback function definition.
 */
typedef void (sx127x_dio_irq_handler_t)(sx127x_t *dev);

/**
 * @brief   Setup the SX127X
 *
 * @param[in] dev                      Device descriptor
 * @param[in] params                   Parameters for device initialization
 * @param[in] index                    Index of @p params in a global parameter struct array.
 *                                     If initialized manually, pass a unique identifier instead.
 */
void sx127x_setup(sx127x_t *dev, const sx127x_params_t *params, uint8_t index);

/**
 * @brief   Resets the SX127X
 *
 * @param[in] dev                      The sx127x device descriptor
 */
int sx127x_reset(const sx127x_t *dev);

/**
 * @brief   Initializes the transceiver.
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return result of initialization
 */
int sx127x_init(sx127x_t *dev);

/**
 * @brief   Initialize radio settings with default values
 *
 * @param[in] dev                      The sx127x device pointer
 */
void sx127x_init_radio_settings(sx127x_t *dev);

/**
 * @brief   Generates 32 bits random value based on the RSSI readings
 *
 * @attention This function sets the radio in LoRa mode and disables all
 *            interrupts from it. After calling this function either
 *            sx127x_set_rx_config or sx127x_set_tx_config functions must
 *            be called.
 *
 * @param[in] dev                      The sx127x device structure pointer
 *
 * @return random 32 bits value
 */
uint32_t sx127x_random(sx127x_t *dev);

/**
 * @brief   Start a channel activity detection.
 *
 * @param[in] dev                      The sx127x device descriptor
 */
void sx127x_start_cad(sx127x_t *dev);

/**
 * @brief   Checks that channel is free with specified RSSI threshold.
 *
 * @param[in] dev                      The sx127x device structure pointer
 * @param[in] freq                     channel RF frequency
 * @param[in] rssi_threshold           RSSI threshold
 *
 * @return true if channel is free, false otherwise
 */
bool sx127x_is_channel_free(sx127x_t *dev, uint32_t freq, int16_t rssi_threshold);

/**
 * @brief   Reads the current RSSI value.
 *
 * @param[in] dev                      The sx127x device structure pointer
 *
 * @return the current value of RSSI (in dBm)
 */
int16_t sx127x_read_rssi(const sx127x_t *dev);

/**
 * @brief   Gets current state of transceiver.
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return radio state [RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
uint8_t sx127x_get_state(const sx127x_t *dev);

/**
 * @brief   Sets current state of transceiver.
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] state                    The new radio state
 */
void sx127x_set_state(sx127x_t *dev, uint8_t state);

/**
 * @brief   Configures the radio with the given modem.
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] modem                    Modem to be used [0: FSK, 1: LoRa]
 */
void sx127x_set_modem(sx127x_t *dev, uint8_t modem);

/**
 * @brief   Gets the synchronization word.
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return The synchronization word
 */
uint8_t sx127x_get_syncword(const sx127x_t *dev);

/**
 * @brief   Sets the synchronization word.
 *
 * @param[in] dev                     The sx127x device descriptor
 * @param[in] syncword                The synchronization word
 */
void sx127x_set_syncword(sx127x_t *dev, uint8_t syncword);

/**
 * @brief   Gets the channel RF frequency.
 *
 * @param[in]  dev                     The sx127x device descriptor
 *
 * @return The channel frequency
 */
uint32_t sx127x_get_channel(const sx127x_t *dev);

/**
 * @brief   Sets the channel RF frequency.
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] freq                     Channel RF frequency
 */
void sx127x_set_channel(sx127x_t *dev, uint32_t freq);

/**
 * @brief   Computes the packet time on air in milliseconds.
 *
 * @pre     Can only be called if sx127x_init_radio_settings has already
 *          been called.
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] pkt_len                  The received packet payload length
 *
 * @return computed air time (ms) for the given packet payload length
 */
uint32_t sx127x_get_time_on_air(const sx127x_t *dev, uint8_t pkt_len);

/**
 * @brief   Sets the radio in sleep mode
 *
 * @param[in] dev                      The sx127x device descriptor
 */
void sx127x_set_sleep(sx127x_t *dev);

/**
 * @brief   Sets the radio in stand-by mode
 *
 * @param[in] dev                      The sx127x device descriptor
 */
void sx127x_set_standby(sx127x_t *dev);

/**
 * @brief   Sets the radio in reception mode.
 *
 * @param[in] dev                      The sx127x device descriptor
 */
void sx127x_set_rx(sx127x_t *dev);

/**
 * @brief   Sets the radio in transmission mode.
 *
 * @param[in] dev                      The sx127x device descriptor
 */
void sx127x_set_tx(sx127x_t *dev);

/**
 * @brief   Gets the maximum payload length.
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return The maximum payload length
 */
uint8_t sx127x_get_max_payload_len(const sx127x_t *dev);

/**
 * @brief   Sets the maximum payload length.
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] maxlen                   Maximum payload length in bytes
 */
void sx127x_set_max_payload_len(const sx127x_t *dev, uint8_t maxlen);

/**
 * @brief   Gets the SX127X operating mode
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return The actual operating mode
 */
uint8_t sx127x_get_op_mode(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X operating mode
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] op_mode                  The new operating mode
 */
void sx127x_set_op_mode(const sx127x_t *dev, uint8_t op_mode);

/**
 * @brief   Gets the SX127X bandwidth
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the bandwidth
 */
uint8_t sx127x_get_bandwidth(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X bandwidth
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] bandwidth                The new bandwidth
 */
void sx127x_set_bandwidth(sx127x_t *dev, uint8_t bandwidth);

/**
 * @brief   Gets the SX127X LoRa spreading factor
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the spreading factor
 */
uint8_t sx127x_get_spreading_factor(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X LoRa spreading factor
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] sf                       The spreading factor
 */
void sx127x_set_spreading_factor(sx127x_t *dev, uint8_t sf);

/**
 * @brief   Gets the SX127X LoRa coding rate
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the current LoRa coding rate
 */
uint8_t sx127x_get_coding_rate(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X LoRa coding rate
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] coderate                 The LoRa coding rate
 */
void sx127x_set_coding_rate(sx127x_t *dev, uint8_t coderate);

/**
 * @brief   Checks if the SX127X LoRa RX single mode is enabled/disabled
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the LoRa single mode
 */
bool sx127x_get_rx_single(const sx127x_t *dev);

/**
 * @brief   Enable/disable the SX127X LoRa RX single mode
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] single                   The LoRa RX single mode
 */
void sx127x_set_rx_single(sx127x_t *dev, bool single);

/**
 * @brief   Checks if the SX127X CRC verification mode is enabled
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the LoRa single mode
 */
bool sx127x_get_crc(const sx127x_t *dev);

/**
 * @brief   Enable/Disable the SX127X CRC verification mode
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] crc                      The CRC check mode
 */
void sx127x_set_crc(sx127x_t *dev, bool crc);

/**
 * @brief   Gets the SX127X frequency hopping period
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the frequency hopping period
 */
uint8_t sx127x_get_hop_period(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X frequency hopping period
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] hop_period               The frequency hopping period
 */
void sx127x_set_hop_period(sx127x_t *dev, uint8_t hop_period);

/**
 * @brief   Gets the SX127X LoRa fixed header length mode
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the LoRa implicit mode
 */
bool sx127x_get_fixed_header_len_mode(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X to fixed header length mode (explicit mode)
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] mode                     The header mode
 */
void sx127x_set_fixed_header_len_mode(sx127x_t *dev, bool mode);

/**
 * @brief   Gets the SX127X payload length
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the payload length
 */
uint8_t sx127x_get_payload_length(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X payload length
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] len                      The payload len
 */
void sx127x_set_payload_length(sx127x_t *dev, uint8_t len);

/**
 * @brief   Gets the SX127X TX radio power
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the radio power
 */
uint8_t sx127x_get_tx_power(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X transmission power
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] power                    The TX power
 */
void sx127x_set_tx_power(sx127x_t *dev, int8_t power);

/**
 * @brief   Gets the SX127X preamble length
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the preamble length
 */
uint16_t sx127x_get_preamble_length(const sx127x_t *dev);

/**
 * @brief   Sets the SX127X LoRa preamble length
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] preamble                 The LoRa preamble length
 */
void sx127x_set_preamble_length(sx127x_t *dev, uint16_t preamble);

/**
 * @brief   Sets the SX127X LoRa symbol timeout
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] timeout                  The LoRa symbol timeout
 */
void sx127x_set_symbol_timeout(sx127x_t *dev, uint16_t timeout);

/**
 * @brief   Sets the SX127X RX timeout
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] timeout                  The RX timeout
 */
void sx127x_set_rx_timeout(sx127x_t *dev, uint32_t timeout);

/**
 * @brief   Sets the SX127X TX timeout
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] timeout                  The TX timeout
 */
void sx127x_set_tx_timeout(sx127x_t *dev, uint32_t timeout);

/**
 * @brief   Checks if the SX127X LoRa inverted IQ mode is enabled/disabled
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return the LoRa IQ inverted mode
 */
bool sx127x_get_iq_invert(const sx127x_t *dev);

/**
 * @brief   Enable/disable the SX127X LoRa IQ inverted mode
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] iq_invert                The LoRa IQ inverted mode
 */
void sx127x_set_iq_invert(sx127x_t *dev, bool iq_invert);

/**
 * @brief   Sets the SX127X LoRa frequency hopping mode
 *
 * @param[in] dev                      The sx127x device descriptor
 * @param[in] freq_hop_on              The LoRa frequency hopping mode
 */
void sx127x_set_freq_hop(sx127x_t *dev, bool freq_hop_on);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SX127X_H */
