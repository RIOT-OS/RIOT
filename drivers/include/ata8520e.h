/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_ata8520e Microchip ATA8520E transceiver
 * @ingroup     drivers_netdev
 * @brief       Device driver for Microchip ATA8520E transceiver (Sigfox)
 *
 * For more information, please refer to the datasheet
 * http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-9409-Smart-RF-ATA8520E_Datasheet.pdf
 *
 * @{
 *
 * @file
 * @brief       Device driver for Microchip ATA8520E transceiver (Sigfox)
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef ATA8520E_H
#define ATA8520E_H

#include <stdint.h>
#include <inttypes.h>
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SIGFOX constants
 * @{
 */
#define SIGFOX_PAC_LENGTH    (16U)  /**< Length of Sigfox PAC */
#define SIGFOX_ID_LENGTH     (8U)   /**< Length of Sigfox ID */
#define SIGFOX_MAX_TX_LENGTH (12U)  /**< Maximum length of a TX payload */
#define SIGFOX_RX_LENGTH     (8U)   /**< RX payload length is always 8 */
/** @} */

/**
 * @brief return codes
 */
enum {
    ATA8520E_OK = 0,                /**< Everything is fine */
    ATA8520E_ERR_SPI,               /**< An error occurred with SPI */
    ATA8520E_ERR_GPIO_INT,          /**< An error occurred with interrupt pin */
    ATA8520E_ERR_GPIO_RESET,        /**< An error occurred with reset pin */
    ATA8520E_ERR_GPIO_POWER,        /**< An error occurred with power pin */
    ATA8520E_ERR_EVENT_TIMEOUT,     /**< An expected event never happened */
};

/**
 * @brief device internal states
 */
enum {
    ATA8520E_STATE_IDLE,            /**< Device is in idle state */
    ATA8520E_STATE_INIT,            /**< Device is in initialization phase */
    ATA8520E_STATE_TX,              /**< Device is in TX mode */
    ATA8520E_STATE_RX,              /**< Device is in RX mode, just after TX */
};

/**
 * @brief device module status
 */
typedef enum {
    ATA8520E_SSM,                   /**< SoC System Management status */
    ATA8520E_ATMEL,                 /**< Atmel status */
    ATA8520E_SIGFOX                 /**< Sigfox status */
} ata8520e_status_type_t;

/**
 * @brief Sigfox statuses
 *
 * See ATA8520E datasheet, section 2.1.2.10, page 12
 */
typedef enum {
    ATA8520E_ATMEL_OK,                      /**< No error */
    ATA8520E_ATMEL_COMMAND_ERROR,           /**< Command error / not supported */
    ATA8520E_ATMEL_GENERIC_ERROR,           /**< Generic error */
    ATA8520E_ATMEL_FREQUENCY_ERROR,         /**< Frequency error */
    ATA8520E_ATMEL_USAGE_ERROR,             /**< Usage error */
    ATA8520E_ATMEL_OPENING_ERROR,           /**< Opening error */
    ATA8520E_ATMEL_CLOSING_ERROR,           /**< Closing error */
    ATA8520E_ATMEL_SEND_ERROR               /**< Send error */
} ata8520e_atmel_status_t;

/**
 * @brief Sigfox statuses.
 *
 * See ATA8520E datasheet, section 2.1.2.10, page 12
 */
typedef enum {
    ATA8520E_SIGFOX_OK,                      /**< No error */
    ATA8520E_SIGFOX_MANUFACTURER_ERROR,      /**< Manufacturer error */
    ATA8520E_SIGFOX_ID_OR_KEY_ERROR,         /**< ID or key error */
    ATA8520E_SIGFOX_STATE_MACHINE_ERROR,     /**< State machine error */
    ATA8520E_SIGFOX_FRAME_SIZE_ERROR,        /**< Frame size error */
    ATA8520E_SIGFOX_MANUFACTURER_SEND_ERROR, /**< Manufacturer send error */
    ATA8520E_SIGFOX_GET_VOLTAGE_TEMP_ERROR,  /**< Get voltage/temperature error */
    ATA8520E_SIGFOX_CLOSE_ERROR,             /**< Close issues encountered */
    ATA8520E_SIGFOX_API_ERROR,               /**< API error indication */
    ATA8520E_SIGFOX_GET_PN9_ERROR,           /**< Error getting PN9 */
    ATA8520E_SIGFOX_GET_FREQUENCY_ERROR,     /**< Error getting frequency */
    ATA8520E_SIGFOX_BUILDING_FRAME_ERROR,    /**< Error building frame */
    ATA8520E_SIGFOX_DELAY_ROUTINE_ERROR,     /**< Error in delay routine */
    ATA8520E_SIGFOX_CALLBACK_ERROR,          /**< Callback causes error */
    ATA8520E_SIGFOX_TIMING_ERROR,            /**< Timing error */
    ATA8520E_SIGFOX_FREQUENCY_ERROR          /**< Frequency error */
} ata8520e_sigfox_status_t;

/**
 * @brief Initialization parameters
 */
typedef struct {
    spi_t spi;                              /**< SPI device */
    spi_clk_t spi_clk;                      /**< SPI clock speed */
    gpio_t cs_pin;                          /**< Chip select pin */
    gpio_t int_pin;                         /**< IRQ pin */
    gpio_t power_pin;                       /**< Power pin */
    gpio_t reset_pin;                       /**< Reset pin */
} ata8520e_params_t;

/**
 * @brief device descriptor
 */
typedef struct {
    ata8520e_params_t params;               /**< Device parameters */
    uint8_t internal_state;                 /**< current state of the device */
    uint8_t event_received;                 /**< check if an event was received */
    mutex_t event_lock;                     /**< mutex for waiting for event */
} ata8520e_t;

/**
 * @brief Initialize the device
 *
 * This will also initialize the CS pin as a GPIO output, without pull resistors
 * and the interrupt pin handler to manage events.
 *
 * @param[out] dev          Pointer to device descriptor
 * @param[in]  params       Pointer to SPI settings
 *
 * @return  ATA8520E_OK on success
 * @return -ATA8520E_ERR_SPI on SPI initialization error
 * @return -ATA8520E_ERR_GPIO_INT on interrupt pin initialization error
 * @return -ATA8520E_ERR_GPIO_RESET on reset pin initialization error
 * @return -ATA8520E_ERR_GPIO_POWER on power pin initialization error
 */
int ata8520e_init(ata8520e_t *dev, const ata8520e_params_t *params);

/**
 * @brief Perform a complete reset of the device
 *
 * @param[in] dev           Pointer to device descriptor
 */
void ata8520e_system_reset(const ata8520e_t *dev);

/**
 * @brief Return the Atmel version of the device
 *
 * @param[in]  dev          Pointer to device descriptor
 * @param[out] version      Pointer to the string containing the version, must
 *                          be able to hold 2 characters
 */
void ata8520e_read_atmel_version(const ata8520e_t *dev, uint8_t *version);

/**
 * @brief Return the Atmel version of the device
 *
 * @param[in]  dev          Pointer to device descriptor
 * @param[out] version      String containing the version, must be able to hold
 *                          11 characters
 */
void ata8520e_read_sigfox_version(const ata8520e_t *dev, char *version);

/**
 * @brief Return the porting authorization code (PAC) available in the device
 *
 * @param[in]  dev          Pointer to device descriptor
 * @param[out] pac          String containing the pac, must be able to hold 17
 *                          characters
 */
void ata8520e_read_pac(const ata8520e_t *dev, char *pac);

/**
 * @brief Return the ID of the device
 *
 * @param[in]  dev          Pointer to device descriptor
 * @param[out] id           String containing the ID, must be able to hold 9
 *                          characters
 */
void ata8520e_read_id(const ata8520e_t *dev, char *id);

/**
 * @brief Send a frame
 *
 * @param[in] dev           Pointer to device descriptor
 * @param[in] payload       The payload to send
 * @param[in] payload_len   The length of the payload to send
 *
 * @return  ATA8520E_OK if the send was successful
 * @return -ATA8520E_ERR_EVENT_TIMEOUT if the send failed
 */
int ata8520e_send_frame(ata8520e_t *dev, uint8_t *payload, uint8_t payload_len);

/**
 * @brief Send a frame and wait for a RX packet
 *
 * @param[in]  dev          Pointer to device descriptor
 * @param[in]  payload      The payload to send
 * @param[in]  payload_len  The length of the payload to send
 * @param[out] rx_payload   The payload received
 *
 * @return  ATA8520E_OK if the send was successful
 * @return -ATA8520E_ERR_EVENT_TIMEOUT if the send failed
 */
int ata8520e_send_receive_frame(ata8520e_t *dev, uint8_t *payload,
                                uint8_t payload_len, uint8_t *rx_payload);

/**
 * @brief Send a bit
 *
 * @param[in] dev           Pointer to device descriptor
 * @param[in] bit           The bit to send
 *
 * @return  ATA8520E_OK if the send was successful
 * @return -ATA8520E_ERR_EVENT_TIMEOUT if the send failed
 */
int ata8520e_send_bit(ata8520e_t *dev, bool bit);

#ifdef __cplusplus
}
#endif

#endif /* ATA8520E_H */
/** @} */
