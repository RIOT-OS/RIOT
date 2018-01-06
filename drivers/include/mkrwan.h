/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mkrwan MKRWAN LoRa radio transceiver
 * @ingroup     drivers_netdev
 * @brief       Device driver interface for the MKRWAN LoRa radio transceiver.
 *
 * @warning This driver was tested with version 1.1.9 of the firmware running on
 * the transceiver.
 *
 * To update the firmware of the transceiver, the easiest is to flash the
 * Arduino MKRWANFWUpdate_standalone example from https://github.com/arduino-libraries/MKRWAN/tree/master/examples/MKRWANFWUpdate_standalone
 * on the board microcontroller.
 * At startup, this firmware will automatically update the firmware running on
 * transceiver.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the MKRWAN LoRa radio transceiver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MKRWAN_H
#define MKRWAN_H

#include <inttypes.h>
#include "net/loramac.h"
#include "xtimer.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "at.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Size of temporary internal buffer
 */
#define MKRWAN_TMP_BUF_SIZE     (40U)

/**
 * @brief   Size of the internal buffer (must be a power of 2)
 */
#define MKRWAN_INT_BUF_SIZE     (256U)

/**
 * @brief   Command response timeout (2s)
 */
#define MKRWAN_TIMEOUT          (2 * US_PER_SEC)

/**
 * @brief   Command join timeout (60s)
 */
#define MKRWAN_JOIN_TIMEOUT     (60 * US_PER_SEC)

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uart_t uart;                            /**< UART device that is used */
    uint32_t baudrate;                      /**< UART baudrate */
    gpio_t reset_pin;                       /**< Module reset pin */
    gpio_t boot0_pin;                       /**< Module boot0 pin */
} mkrwan_params_t;

/**
 * @brief   Device descriptor for the STM32 AT LoRa radio
 */
typedef struct {
    mkrwan_params_t params;                 /**< Device initialization parameters */
    at_dev_t at_dev;                        /**< AT command device */
    char buf[MKRWAN_INT_BUF_SIZE];          /**< internal buffer */
} mkrwan_t;

/**
 * @brief   Status and error return codes
 */
enum {
    MKRWAN_OK = 0,                          /**< everything was fine */
    MKRWAN_ERR,                             /**< generic error */
};

/**
 * @brief   Setup the given device
 *
 * @param[out] dev          Initialized device descriptor of the device
 * @param[in]  params       Initialization parameters
 */
void mkrwan_setup(mkrwan_t *dev, const mkrwan_params_t *params);

/**
 * @brief   Setup the given device
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
int mkrwan_init(mkrwan_t *dev);

/**
 * @brief   Reset the given device
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
int mkrwan_reset(mkrwan_t *dev);

/**
 * @brief   Get the version of the device
 *
 * @param[in] dev           Device descriptor of the device
 * @param[out] version      String containing the version of the module
 */
void mkrwan_version(mkrwan_t *dev, char *version);

/**
 * @brief   Join a LoRaWAN network
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] mode          Join procedure mode (either OTAA or ABP)
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
int mkrwan_join(mkrwan_t *dev, uint8_t mode);

/**
 * @brief   Send data to a LoRaWAN network
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] mode          Send mode (either confirmed or unconfirmed)
 * @param[in] data          Data to send
 * @param[in] len           Length of data to send
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
int mkrwan_send(mkrwan_t *dev, uint8_t *data, uint8_t len, uint8_t mode);

/**
 * @brief   Set the device EUI
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] eui           The device EUI
 */
void mkrwan_set_deveui(mkrwan_t *dev, uint8_t *eui);

/**
 * @brief   Get the device EUI
 *
 * @param[in] dev           Device descriptor of the device
 * @param[out] eui          The device EUI
 */
void mkrwan_get_deveui(mkrwan_t *dev, uint8_t *eui);

/**
 * @brief   Set the application EUI
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] eui           The application EUI
 */
void mkrwan_set_appeui(mkrwan_t *dev, uint8_t *eui);

/**
 * @brief   Set the application EUI
 *
 * @param[in] dev           Device descriptor of the device
 * @param[out] eui          The application EUI
 */
void mkrwan_get_appeui(mkrwan_t *dev, uint8_t *eui);

/**
 * @brief   Set the application key
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] key           The application key
 */
void mkrwan_set_appkey(mkrwan_t *dev, uint8_t *key);

/**
 * @brief   Get the application key
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] key           The application key
 */
void mkrwan_get_appkey(mkrwan_t *dev, uint8_t *key);

/**
 * @brief   Set the device address
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] addr          The device address
 */
void mkrwan_set_devaddr(mkrwan_t *dev, uint8_t *addr);

/**
 * @brief   Get the device address
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] addr          The device address
 */
void mkrwan_get_devaddr(mkrwan_t *dev, uint8_t *addr);

/**
 * @brief   Set the application session key
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] skey          The application session key
 */
void mkrwan_set_appskey(mkrwan_t *dev, uint8_t *skey);

/**
 * @brief   Get the application session key
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] skey          The application session key
 */
void mkrwan_get_appskey(mkrwan_t *dev, uint8_t *skey);

/**
 * @brief   Set the network session key
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] skey          The network session key
 */
void mkrwan_set_nwkskey(mkrwan_t *dev, uint8_t *skey);

/**
 * @brief   Get the network session key
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] skey          The network session key
 */
void mkrwan_get_nwkskey(mkrwan_t *dev, uint8_t *skey);

/**
 * @brief   Set the device class
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] class         Device class
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
void mkrwan_set_class(mkrwan_t *dev, uint8_t class);

/**
 * @brief   Get the device class
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  the device class
 */
uint8_t mkrwan_get_class(mkrwan_t *dev);

/**
 * @brief   Enable/disable duty cycle
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] enable        duty cycle is enabled or not
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
void mkrwan_set_duty_cycle(mkrwan_t *dev, bool enable);

/**
 * @brief   Check if duty cycle is enabled/disabled
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  true duty cycle if is enabled, false otherwise
 */
bool mkrwan_get_duty_cycle(mkrwan_t *dev);

/**
 * @brief   Enable/disable the public network
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] enable        network is enabled or not
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
void mkrwan_set_public_network(mkrwan_t *dev, bool enable);

/**
 * @brief   Check if public network is enabled/disabled
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  true is public network is enabled, false otherwise
 */
bool mkrwan_get_public_network(mkrwan_t *dev);

/**
 * @brief   Set the datarate index
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] dr            The datarate index
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
void mkrwan_set_datarate(mkrwan_t *dev, loramac_dr_idx_t dr);

/**
 * @brief   Get the datarate index
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  the datarate index
 */
loramac_dr_idx_t mkrwan_get_datarate(mkrwan_t *dev);

/**
 * @brief   Set the adaptive datarate mode
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] enable        Enable adaptive datarate
 *
 * @return                  MKRWAN_OK on success
 * @return                 -MKRWAN_ERR on error
 */
void mkrwan_set_adr(mkrwan_t *dev, bool enable);

/**
 * @brief   Get the datarate index
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  the datarate index
 */
bool mkrwan_get_adr(mkrwan_t *dev);

/**
 * @brief   Sets the TX application port
 *
 * @param[in] dev           Device descriptor of the device
 * @param[in] port          The TX application port
 */
void mkrwan_set_tx_port(mkrwan_t *dev, uint8_t port);

/**
 * @brief   Gets the TX application port
 *
 * @param[in] dev           Device descriptor of the device
 *
 * @return                  The TX application port
 */
uint8_t mkrwan_get_tx_port(mkrwan_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MKRWAN_H */
/** @} */
