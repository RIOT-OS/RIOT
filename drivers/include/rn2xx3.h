/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_rn2xx3 RN2483/RN2903 LoRa module driver
 * @ingroup     drivers_netdev
 * @brief       High-level driver for the RN2483/RN2903 LoRa modules
 * @{
 *
 * @file
 * @brief       High-level driver for the RN2483/RN2903 LoRa modules
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef RN2XX3_H
#define RN2XX3_H

#include <stdint.h>

#include "ztimer.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/netdev.h"

#include "net/loramac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum length of an exchanged messages with commands
 */
#define RN2XX3_MAX_BUF                  (40U)

/**
 * @brief   Maximum length of an RX message
 *
 *          LoRaMAC payload can be up to 250 bytes.
 */
#define RN2XX3_RX_MAX_BUF               (250U)

/**
 * @brief   Maximum delay in second to receive a reply from server.
 */
#define RN2XX3_REPLY_DELAY_TIMEOUT      (60U)

/**
 * @brief   Minimum sleep duration (in ms)
 */
#define RN2XX3_SLEEP_MIN                (100U)

/**
 * @defgroup drivers_rn2xx3_config     RN2483/RN2903 Radio driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief   Default sleep duration (in ms)
 */
#ifndef CONFIG_RN2XX3_DEFAULT_SLEEP
#define CONFIG_RN2XX3_DEFAULT_SLEEP            (5000U)
#endif

/**
 * @brief   Default automatic reply status
 *
 * Set to 1 to enable the automatic reply. The module will transmit a packet without a
 * payload immediately after a confirmed downlink is received, or when the Frame
 * Pending bit has been set by the server.
 *
 * @note If all enabled channels are busy due to duty cycle limitations, the
 * module will wait for the first channel that will become free to transmit. The
 * user will not be able to initiate uplink transmissions until the automatic
 * transmissions are done. Refer RN2483 LoRa™ Technology Module Command
 * Reference User’s Guide for more information.
 */
#ifdef DOXYGEN
#define CONFIG_RN2XX3_DEFAULT_AR
#endif
/** @} */

#if defined(MODULE_RN2903)
#define RN2XX3_FREQ_BAND                (915U)
#elif defined(MODULE_RN2483)
#define RN2XX3_FREQ_BAND                (868U)
#else
#error "Unsupported module type, use either RN2483 or RN2903"
#endif

/**
 * @brief   Command responses and server replies status codes
 */
enum {
    RN2XX3_OK,                         /**< Command is valid */
    RN2XX3_DATA,                       /**< Command returned data */
    RN2XX3_TIMEOUT,                    /**< Command timeout */
    /* all other error codes */
    RN2XX3_ERR_MAC_INIT,               /**< Device mac initialization failed */
    RN2XX3_ERR_INVALID_PARAM,          /**< Wrong command given */
    RN2XX3_ERR_NOT_JOINED,             /**< Network is not joined */
    RN2XX3_ERR_NO_FREE_CH,             /**< All channels are busy */
    RN2XX3_ERR_SILENT,                 /**< Device is in Silent Immediately state */
    RN2XX3_ERR_FR_CNT_REJOIN_NEEDED,   /**< Frame counter rolled over */
    RN2XX3_ERR_BUSY,                   /**< MAC is not in Idle state */
    RN2XX3_ERR_MAC_PAUSED,             /**< MAC was paused */
    RN2XX3_ERR_INVALID_DATA_LENGTH,    /**< Wrong payload given */
    RN2XX3_ERR_KEYS_NOT_INIT,          /**< Keys not configured ("mac join" command) */
    RN2XX3_ERR_SLEEP_MODE,             /**< Failure because device is in sleep mode */
    /* all other reply codes */
    RN2XX3_REPLY_TX_MAC_OK,            /**< MAC transmission successful */
    RN2XX3_REPLY_TX_MAC_ERR,           /**< MAC transmission failed */
    RN2XX3_REPLY_TX_INVALID_DATA_LEN,  /**< Application payload too large */
    RN2XX3_REPLY_TX_MAC_RX,            /**< Data received from server */
    RN2XX3_REPLY_JOIN_ACCEPTED,        /**< Join procedure successful */
    RN2XX3_REPLY_JOIN_DENIED,          /**< Join procedure failed */
    RN2XX3_REPLY_TIMEOUT,              /**< No MAC reply received from server */
    RN2XX3_REPLY_OTHER,                /**< Unknown reply */
};

/**
 * @brief   Internal states of the device
 */
enum {
    RN2XX3_INT_STATE_RESET,            /**< the device is being reset or initialized */
    RN2XX3_INT_STATE_CMD,              /**< waiting command response */
    RN2XX3_INT_STATE_IDLE,             /**< waiting for incoming commands */
    RN2XX3_INT_STATE_SLEEP,            /**< sleep mode */
    RN2XX3_INT_STATE_MAC_JOIN,         /**< waiting for mac join procedure reply */
    RN2XX3_INT_STATE_MAC_TX,           /**< waiting for mac tx reply */
    RN2XX3_INT_STATE_MAC_RX_PORT,      /**< waiting for mac rx port */
    RN2XX3_INT_STATE_MAC_RX_MESSAGE,   /**< waiting for mac rx message */
};

/**
 * @brief   LoRaMAC communication settings
 */
typedef struct {
    uint32_t rx2_freq;                 /**< Center frequency used for second receive window */
    loramac_dr_idx_t rx2_dr;           /**< Datarate used for second receive window */
    uint8_t tx_port;                   /**< Application TX port (between 1 and 223) */
    loramac_tx_mode_t tx_mode;         /**< TX mode, either confirmable of unconfirmable */
    uint8_t rx_port;                   /**< RX port (between 1 and 223) */
} loramac_settings_t;

/**
 * @brief   Configuration parameters for RN2483/RN2903 devices
 */
typedef struct {
    uart_t uart;                       /**< UART interfaced the device is connected to */
    uint32_t baudrate;                 /**< baudrate to use */
    gpio_t pin_reset;                  /**< GPIO pin that is connected to the STATUS pin
                                            set to GPIO_UNDEF if not used */
} rn2xx3_params_t;

/**
 * @brief   RN2483/RN2903 device descriptor
 */
typedef struct {
    netdev_t netdev;                   /**< Netdev parent struct */
    /* device driver specific fields */
    rn2xx3_params_t p;                 /**< configuration parameters */
    loramac_settings_t loramac;        /**< loramac communication settings */

    /* values for the UART TX state machine  */
    char cmd_buf[RN2XX3_MAX_BUF];      /**< command to send data buffer */
    mutex_t cmd_lock;                  /**< mutex to allow only one
                                        *   command at a time */
    uint8_t int_state;                 /**< current state of the device */

    /* buffer and synchronization for command responses */
    mutex_t resp_lock;                 /**< mutex for waiting for command
                                        *   response */
    char resp_buf[RN2XX3_MAX_BUF];     /**< command response data buffer */
    uint16_t resp_size;                /**< counter for received char in response */
    uint8_t resp_done;                 /**< check if response has completed */

    /* buffer for RX messages */
    char rx_tmp_buf[2];                /**< Temporary RX buffer used to convert
                                        *   2 hex characters in one byte on the
                                        *   fly. */
    uint8_t rx_buf[RN2XX3_RX_MAX_BUF]; /**< RX data buffer */
    uint16_t rx_size;                  /**< counter for received char in RX */

    /* timers */
    ztimer_t sleep_timer;              /**< Timer used to count module sleep time */
    uint32_t sleep;                    /**< module sleep duration */
} rn2xx3_t;

/**
 * @brief   Prepares the given RN2XX3 device
 *
 * @param[out] dev          RN2XX3 device to initialize
 * @param[in]  params       parameters for device initialization
 */
void rn2xx3_setup(rn2xx3_t *dev, const rn2xx3_params_t *params);

/**
 * @brief   Initializes the RN2XX3 device
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                 -ENXIO if UART initialization failed
 * @return                  RN2XX3_TIMEOUT if UART communication failed
 */
int rn2xx3_init(rn2xx3_t *dev);

/**
 * @brief   Restarts the RN2XX2 device
 *
 * After calling this function, dev->resp_buf contains the module
 * name and version string.
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                  RN2XX3_TIMEOUT if UART communication failed
 */
int rn2xx3_sys_reset(rn2xx3_t *dev);

/**
 * @brief   Performs a factory reset of the module
 *
 * The configuration data and user EEPPROM are reinitialized to factory default
 * values and the module reboots
 *
 * After calling this function, dev->resp_buf contains the module name and
 * version string.
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                  RN2XX3_TIMEOUT if UART communication failed
 */
int rn2xx3_sys_factory_reset(rn2xx3_t *dev);

/**
 * @brief   Puts the RN2XX2 device in sleep mode
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                  RN2XX3_TIMEOUT if UART communication failed
 */
 int rn2xx3_sys_sleep(rn2xx3_t *dev);

/**
 * @brief   Initializes the RN2XX3 device MAC layer
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                  RN2XX3_TIMEOUT if UART communication failed
 */
int rn2xx3_mac_init(rn2xx3_t *dev);

/**
 * @brief   Writes a command to the RN2XX3 device
 *
 * The module will immediately reply with a meaningful message if the command
 * is valid or not.
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                  RN2XX3_TIMEOUT if no response is received from the module
 * @return                  RN2XX3_ERR_INVALID_PARAM if command is invalid
 * @return                  RN2XX3_ERR_NOT_JOINED if network is not joined
 * @return                  RN2XX3_ERR_NO_FREE_CH if no free channel
 * @return                  RN2XX3_ERR_SILENT if device is in Silent state
 * @return                  RN2XX3_ERR_FR_CNT_REJOIN_NEEDED if frame counter rolled over
 * @return                  RN2XX3_ERR_BUSY if MAC is not in Idle state
 * @return                  RN2XX3_ERR_INVALID_DATA_LENGTH if payload is too large
 * @return                  RN2XX3_ERR_KEYS_NOT_INIT if keys are not configured
 */
int rn2xx3_write_cmd(rn2xx3_t *dev);

/**
 * @brief   Writes a command to the RN2XX3 device but don't wait for timeout or response
 *
 * The response can be checked in the `dev->resp_buf` buffer after a small delay
 * (for example 1ms).
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 */
int rn2xx3_write_cmd_no_wait(rn2xx3_t *dev);

/**
 * @brief   Waits for a response to a command from the device
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK on success
 * @return                  RN2XX3_TIMEOUT if no response is received from the module
 * @return                  RN2XX3_ERR_INVALID_PARAM if command is invalid
 * @return                  RN2XX3_ERR_NOT_JOINED if network is not joined
 * @return                  RN2XX3_ERR_NO_FREE_CH if no free channel
 * @return                  RN2XX3_ERR_SILENT if device is in Silent state
 * @return                  RN2XX3_ERR_FR_CNT_REJOIN_NEEDED if frame counter rolled over
 * @return                  RN2XX3_ERR_BUSY if MAC is not in Idle state
 * @return                  RN2XX3_ERR_INVALID_DATA_LENGTH if payload is too large
 * @return                  RN2XX3_ERR_KEYS_NOT_INIT if keys are not configured
 */
int rn2xx3_wait_response(rn2xx3_t *dev);

/**
 * @brief   Waits for a reply from the LoRaWAN network
 *
 * @param[in] dev           LoRaBee device descriptor
 * @param[in] timeout       Reply wait timeout in seconds
 *
 * @return                  RN2XX3_REPLY_TIMEOUT when no MAC reply is received from server
 * @return                  RN2XX3_REPLY_TX_MAC_OK when MAC transmission succeeded
 * @return                  RN2XX3_REPLY_TX_MAC_ERR when MAC transmission failed
 * @return                  RN2XX3_REPLY_TX_MAC_RX when received downlink data from server
 * @return                  RN2XX3_REPLY_TX_INVALID_DATA_LEN when Application payload is too large
 * @return                  RN2XX3_REPLY_JOIN_ACCEPTED when the join procedure succeeded
 * @return                  RN2XX3_REPLY_JOIN_DENIED when the join procedure failed
 * @return                  RN2XX3_REPLY_OTHER when an unknown reply is received
 */
int rn2xx3_wait_reply(rn2xx3_t *dev, uint8_t timeout);

/**
 * @brief   Sends data to LoRaWAN server
 *
 * @param[in] dev           RN2XX3 device descriptor
 * @param[in] payload       Payload to transmit
 * @param[in] payload_len   Payload length to transmit
 *
 * @return                  RN2XX3_ERR_KEYS_NOT_INIT if the loramac params are not configured
 * @return                  RN2XX3_ERR_NO_FREE_CH if channels are busy
 * @return                  RN2XX3_ERR_SILENT if device is in Silent state
 * @return                  RN2XX3_ERR_BUSY if MAC layer is in idle state
 * @return                  RN2XX3_ERR_MAC_PAUSED if MAC laid is paused
 * @return                  RN2XX3_REPLY_TX_INVALID_DATA_LEN if payload is too large
 * @return                  RN2XX3_REPLY_TX_MAC_ERR when MAC transmission failed
 * @return                  RN2XX3_REPLY_TX_MAC_RX when received downlink data from server
 * @return                  RN2XX3_REPLY_TX_MAC_OK when MAC transmission succeeded
 */
int rn2xx3_mac_tx(rn2xx3_t *dev, uint8_t *payload, uint8_t payload_len);

/**
 * @brief   Starts network activation procedure
 *
 * @param[in] dev           RN2XX3 device descriptor
 * @param[in] mode          Activation procedure type
 *
 * @return                  RN2XX3_ERR_KEYS_NOT_INIT if the loramac params are not configured
 * @return                  RN2XX3_ERR_NO_FREE_CH if channels are busy
 * @return                  RN2XX3_ERR_SILENT if device is in Silent state
 * @return                  RN2XX3_ERR_BUSY if MAC layer is in idle state
 * @return                  RN2XX3_ERR_MAC_PAUSED if MAC layer is paused
 * @return                  RN2XX3_REPLY_JOIN_ACCEPTED when the join procedure succeeded
 * @return                  RN2XX3_REPLY_JOIN_DENIED when the join procedure failed
 */
int rn2xx3_mac_join_network(rn2xx3_t *dev, loramac_join_mode_t mode);

/**
 * @brief   Saves current LoRaMAC configuration to internal EEPROM
 *
 * The configuration parameters saved are: frequency band, end device EUI,
 * application EUI, application key, network session key, application session
 * key, end device EUI and all channel parameters.
 *
 * @param[in] dev           RN2XX3 device descriptor
 *
 * @return                  RN2XX3_OK if all is ok
 * @return                  RN2XX3_TIMEOUT if the device didn't reply
 */
int rn2xx3_mac_save(rn2xx3_t *dev);

/* Get/Set functions definitions */

/**
 * @brief   Gets the rn2xx3 LoRaMAC device EUI
 *
 * The device EUI is an array of 8 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[out] eui                     The device EUI
 */
void rn2xx3_mac_get_deveui(rn2xx3_t *dev, uint8_t *eui);

/**
 * @brief   Sets the rn2xx3 LoRaMAC device EUI
 *
 * The device EUI is an array of 8 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] eui                      The device EUI
 */
void rn2xx3_mac_set_deveui(rn2xx3_t *dev, const uint8_t *eui);

/**
 * @brief   Gets the rn2xx3 LoRaMAC application EUI
 *
 * The application EUI is an array of 8 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[out] eui                     The application EUI
 */
void rn2xx3_mac_get_appeui(rn2xx3_t *dev, uint8_t *eui);

/**
 * @brief   Sets the rn2xx3 LoRaMAC application EUI
 *
 * The application key is an array of 8 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] eui                      The application EUI
 */
void rn2xx3_mac_set_appeui(rn2xx3_t *dev, const uint8_t *eui);

/**
 * @brief   Sets the rn2xx3 LoRaMAC application key
 *
 * The application key is an array of 16 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] key                      The application key
 */
void rn2xx3_mac_set_appkey(rn2xx3_t *dev, const uint8_t *key);

/**
 * @brief   Sets the rn2xx3 LoRaMAC application session key
 *
 * The application session key is an array of 16 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] key                      The application session key
 */
void rn2xx3_mac_set_appskey(rn2xx3_t *dev, const uint8_t *key);

/**
 * @brief   Sets the rn2xx3 LoRaMAC network session key
 *
 * The network session key is an array of 16 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] key                      The network session key
 */
void rn2xx3_mac_set_nwkskey(rn2xx3_t *dev, const uint8_t *key);

/**
 * @brief   Gets the rn2xx3 LoRaMAC device address
 *
 * The device address is an array of 4 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[out] addr                    The device address
 */
void rn2xx3_mac_get_devaddr(rn2xx3_t *dev, uint8_t *addr);

/**
 * @brief   Sets the rn2xx3 LoRaMAC device address
 *
 * The device address is an array of 4 bytes.
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] addr                     The device address
 */
void rn2xx3_mac_set_devaddr(rn2xx3_t *dev, const uint8_t *addr);

/**
 * @brief   Gets the rn2xx3 LoRaMAC TX radio power index
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The radio power index
 */
loramac_tx_pwr_idx_t rn2xx3_mac_get_tx_power(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC transmission power index
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] power                    The TX power index
 */
void rn2xx3_mac_set_tx_power(rn2xx3_t *dev, loramac_tx_pwr_idx_t power);

/**
 * @brief   Gets the rn2xx3 LoRaMAC datarate
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The datarate
 */
loramac_dr_idx_t rn2xx3_mac_get_dr(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC datarate
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] dr                       The datarate
 */
void rn2xx3_mac_set_dr(rn2xx3_t *dev, loramac_dr_idx_t dr);

/**
 * @brief   Gets the rn2xx3 LoRaMAC frequency band in operation
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The frequency band
 */
 uint16_t rn2xx3_mac_get_band(rn2xx3_t *dev);

/**
 * @brief   Checks if the rn2xx3 LoRaMAC adaptive datarate is enabled/disabled
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             true if adaptive datarate is set,
 *                                     false otherwise
 */
bool rn2xx3_mac_get_adr(rn2xx3_t *dev);

/**
 * @brief   Enables/disables the rn2xx3 LoRaMAC adaptive datarate
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] adr                      The adaptive datarate mode
 */
void rn2xx3_mac_set_adr(rn2xx3_t *dev, bool adr);

/**
 * @brief   Sets the rn2xx3 battery level measured by the end device
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] battery                  The battery level:
 *                                     - 0 means external power,
 *                                     - 1 means low level,
 *                                     - 254 high level,
 *                                     - 255 means the battery level couldn't be
 *                                       measured by the device.
 */
void rn2xx3_mac_set_battery(rn2xx3_t *dev, uint8_t battery);

/**
 * @brief   Gets the rn2xx3 LoRaMAC uplink retransmission retries number
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The number of uplink retransmission retries
 */
uint8_t rn2xx3_mac_get_retx(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC uplink retransmission retries number
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] retx                     The number of uplink retransmission retries
 */
void rn2xx3_mac_set_retx(rn2xx3_t *dev, uint8_t retx);

/**
 * @brief   Sets the rn2xx3 LoRaMAC link check interval (in seconds)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] linkchk                  The link check interval in seconds
 */
void rn2xx3_mac_set_linkchk_interval(rn2xx3_t *dev, uint16_t linkchk);

/**
 * @brief   Gets the rn2xx3 LoRaMAC interval delay before the first reception window (in ms)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The delay in ms
 */
uint16_t rn2xx3_mac_get_rx1_delay(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC interval delay before the first reception window (in ms)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] rx1                      The delay in ms
 */
void rn2xx3_mac_set_rx1_delay(rn2xx3_t *dev, uint16_t rx1);

/**
 * @brief   Gets the rn2xx3 LoRaMAC interval delay before the second reception window (in ms)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The delay in ms
 */
uint16_t rn2xx3_mac_get_rx2_delay(rn2xx3_t *dev);

/**
 * @brief   Checks the rn2xx3 LoRaMAC automatic reply state
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The automatic reply state
 */
bool rn2xx3_mac_get_ar(rn2xx3_t *dev);

/**
 * @brief   Enables/disables LoRaMAC rn2xx3 MAC automatic reply state
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] ar                       The automatic reply state
 */
void rn2xx3_mac_set_ar(rn2xx3_t *dev, bool ar);

/**
 * @brief   Gets the rn2xx3 LoRaMAC datarate used for second receive window
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The datarate during second receive window
 */
loramac_dr_idx_t rn2xx3_mac_get_rx2_dr(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC datarate used for second receive window
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] dr                       The datarate during second receive window
 */
void rn2xx3_mac_set_rx2_dr(rn2xx3_t *dev, loramac_dr_idx_t dr);

/**
 * @brief   Gets the rn2xx3 LoRaMAC frequency used during second receive window (in Hz)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The frequency during second receive window
 */
uint32_t rn2xx3_mac_get_rx2_freq(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC frequency used during second receive window (in Hz)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] freq                     The frequency during second receive window
 */
void rn2xx3_mac_set_rx2_freq(rn2xx3_t *dev, uint32_t freq);

/**
 * @brief   Gets the rn2xx3 LoRaMAC TX port
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The TX port
 */
uint8_t rn2xx3_mac_get_tx_port(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC TX port
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] port                     The TX port (from 1 to 223)
 */
void rn2xx3_mac_set_tx_port(rn2xx3_t *dev, uint8_t port);

/**
 * @brief   Gets the rn2xx3 LoRaMAC TX mode
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The TX mode, either confirmable or unconfirmable
 */
loramac_tx_mode_t rn2xx3_mac_get_tx_mode(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 LoRaMAC TX mode
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] mode                     The TX mode, either confirmable or unconfirmable
 */
void rn2xx3_mac_set_tx_mode(rn2xx3_t *dev, loramac_tx_mode_t mode);

/**
 * @brief   Parses the response buffer to get the LoRaWAN RX port
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 *
 * @return                             The RX port (between 1 and 223)
 */
uint8_t rn2xx3_mac_get_rx_port(rn2xx3_t *dev);

/**
 * @brief   Sets the rn2xx3 sleep mode duration (in ms)
 *
 * @param[in] dev                      The rn2xx3 device descriptor
 * @param[in] sleep                    The sleep mode duration (ms)
 */
void rn2xx3_sys_set_sleep_duration(rn2xx3_t *dev, uint32_t sleep);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* RN2XX3_H */
