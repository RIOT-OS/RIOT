/*
 * Copyright (C) 2020 Antonio Galea <antonio.galea@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_ata8510 ATA8510 driver
 *
 * This module contains a driver for the radio device Atmel ATA8510.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for ATA8510 driver
 *
 * @author      Antonio Galea <antonio.galea@gmail.com>
 * @author      Roberto Asquini <bobasquins@gmail.com>
 */

#ifndef ATA8510_H
#define ATA8510_H

#include <stdint.h>
#include <inttypes.h>
#include "xtimer.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "ata8510_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief return codes
 */
enum {
    ATA8510_OK = 0,                 /**< Everything is fine */
    ATA8510_ERR_SPI,                /**< An error occurred with SPI */
    ATA8510_ERR_MALLOC,             /**< An error occurred during malloc */
    ATA8510_ERR_SIGNATURE,          /**< Wrong device signature found */
    ATA8510_ERR_GPIO_INT,           /**< An error occurred with interrupt pin */
    ATA8510_ERR_GPIO_RESET,         /**< An error occurred with reset pin */
    ATA8510_ERR_GPIO_POWER,         /**< An error occurred with power pin */
    ATA8510_ERR_EVENT_TIMEOUT,      /**< An expected event never happened */
    ATA8510_ERR_BUFFER_OVERFLOW,    /**< Received too many bytes */
    ATA8510_ERR_RECV_THREAD,        /**< Cannot create the receiver thread */
};

/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct ata8510_params {
    spi_t spi;                              /**< SPI device */
    spi_clk_t spi_clk;                      /**< SPI clock speed */
    gpio_t cs_pin;                          /**< Chip select pin */
    gpio_t int_pin;                         /**< IRQ pin */
    gpio_t sleep_pin;                       /**< Sleep pin */
    gpio_t reset_pin;                       /**< Reset pin */
} ata8510_params_t;

/**
 * @brief   Packet handler callback
 *
 * NOTE: the callback is responsible for freeing buffer
 *       after using it.
 *
 * @param[in] buffer    The packet received
 * @param[in] len       Packet length
 * @param[in] dbm       Signal strength in dBm
 */
typedef void (*ata8510_recv_cb_t)(uint8_t *buffer, int len, int dbm);

/**
 * @brief device descriptor
 */
typedef struct {
    ata8510_params_t params;        /**< parameters for initialization */
    uint8_t internal_state;         /**< current state of the radio */
    mutex_t tx_lock;                /**< mutex for tx events */
    mutex_t rx_lock;                /**< mutex for rx events */
    ata8510_events_t events;        /**< radio events registers */
    ata8510_channel_t channel;      /**< radio channel for tx/rx */
    ata8510_service_t service;      /**< radio service for tx/rx */
    ata8510_rssi_t rssi;            /**< holds rssi measures */
    ata8510_recv_cb_t receive_cb;   /**< callback invoked on receive */
} ata8510_t;

/**
 * @brief Initialize the device
 *
 * This will also initialize the CS pin as a GPIO output, without pull resistors
 * and the interrupt pin handler to manage events.
 *
 * @param[out] dev          Pointer to device descriptor
 * @param[in]  params       Pointer to SPI settings
 *
 * @return  ATA8510_OK on success
 * @return -ATA8510_ERR_SPI on SPI initialization error
 * @return -ATA8510_ERR_SIGNATURE on wrong device signature
 * @return -ATA8510_ERR_GPIO_INT on interrupt pin initialization error
 * @return -ATA8510_ERR_GPIO_RESET on reset pin initialization error
 * @return -ATA8510_ERR_GPIO_POWER on power pin initialization error
 */
int ata8510_init(ata8510_t *dev, const ata8510_params_t *params);

/**
 * @brief Power on the device
 *
 * @param[in] dev           Pointer to device descriptor
 */
void ata8510_poweron(ata8510_t *dev);

/**
 * @brief Power off the device
 *
 * @param[in] dev           Pointer to device descriptor
 */
void ata8510_poweroff(ata8510_t *dev);

/**
 * @brief Perform a complete reset of the device
 *
 * @param[in] dev           Pointer to device descriptor
 */
void ata8510_system_reset(ata8510_t *dev);

/**
 * @brief Read the events bytes and write to dev->events
 *
 * @param[in] dev           Pointer to device descriptor
 */
void ata8510_get_event_bytes(ata8510_t *dev);

/**
 * @brief Set the radio mode
 *
 * @param[in] dev           Pointer to device descriptor
 * @param[in] opm           Operational mode
 *
 * @return previous mode
 */
ata8510_system_mode_config_opm_t ata8510_set_mode(
    ata8510_t *dev, ata8510_system_mode_config_opm_t opm);

/**
 * @brief Send a frame
 *
 * @param[in] dev           Pointer to device descriptor
 * @param[in] payload       The payload to send
 * @param[in] payload_len   The length of the payload to send
 */
void ata8510_send_frame(ata8510_t *dev, uint8_t *payload, uint8_t payload_len);

/**
 * @brief Get the power amplifier setting
 *
 * @param[in] dev           Pointer to device descriptor
 *
 * @return output power
 */
uint8_t ata8510_get_radio_power(ata8510_t *dev);

/**
 * @brief Set the power amplifier setting
 *
 * @param[in] dev           Pointer to device descriptor
 * @param[in] power         Output power
 */
void ata8510_set_radio_power(ata8510_t *dev, uint8_t power);

/**
 * @brief Get the radio service / channel
 *
 * @param[in]  dev          Pointer to device descriptor
 * @param[out] service      Service used for rx/tx
 * @param[out] channel      Channel used for rx/tx
 */
void ata8510_get_radio_service_channel(
    ata8510_t *dev, ata8510_service_t *service, ata8510_channel_t *channel);

/**
 * @brief Set the radio service / channel
 *
 * @param[in] dev          Pointer to device descriptor
 * @param[in] service      Service used for rx/tx
 * @param[in] channel      Channel used for rx/tx
 */
void ata8510_set_radio_service_channel(
    ata8510_t *dev, ata8510_service_t service, ata8510_channel_t channel);

#ifdef __cplusplus
}
#endif

#endif /* ATA8510_H */
/** @} */
