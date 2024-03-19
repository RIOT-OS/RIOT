/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp32_esp_can ESP32 CAN controller
 * @ingroup     cpu_esp32
 * @ingroup     drivers_can
 * @brief       CAN controller driver for ESP32 (esp_can)
 *
 * The ESP32 intregates a CAN controller which is compatible with the
 * NXP SJA1000 CAN controller. Thus, it is CAN 2.0B specification compliant
 * and supports two message formats:
 *
 * - Base frame format (11-bit ID)
 * - Extended frame format (29-bit ID)
 *
 * @note
 * - ESP32 CAN does not support CAN-FD and is not CAN-FD tolerant.
 * - ESP32 CAN does not support SJA1000's sleep mode and wake-up functionality.
 *
 * As with the SJA1000, the ESP32 CAN controller provides only the data link
 * layer and the physical layer signaling sublayer. Therefore, depending on
 * the physical layer requirements, an external transceiver module is required
 * which converts the `CAN-RX` and `CAN-TX` signals of the ESP32 into `CAN_H`
 * and `CAN_L` bus signals, e.g., the MCP2551 or SN65HVD23X transceiver for
 * compatibility with ISO 11898-2.
 *
 * The driver realizes a low-level CAN driver interface for the ESP32 CAN
 * controller and provides a CAN DLL device that can be used in RIOT's CAN
 * protocol stack.
 * It uses the ESP32 CAN controller in SJA1000 PeliCAN mode. Please refer the
 * [SJA1000 Datasheet](https://www.nxp.com/documents/data_sheet/SJA1000.pdf)
 * for detailed information about the CAN controller and its programming.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 * @file
 */

#ifndef CAN_ESP_H
#define CAN_ESP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "can/candev.h"
#include "periph/gpio.h"

/** Number of receiver acceptance filters */
#define ESP_CAN_MAX_RX_FILTERS  (16)

/** Number of frames in receiver buffer, must be a power of two */
#define ESP_CAN_MAX_RX_FRAMES   (8)

/**
 * Low level device structure for ESP32 CAN (extension of candev_t)
 */
typedef struct can {
    candev_t          candev;       /**< candev base structure */
    canopt_state_t    state;        /**< current state of device */

    struct can_frame *tx_frame;                           /**< frame in transmission  */
    struct can_frame  rx_frames[ESP_CAN_MAX_RX_FRAMES];   /**< frames received        */
    struct can_filter rx_filters[ESP_CAN_MAX_RX_FILTERS]; /**< acceptance filter list */

    uint32_t rx_frames_wptr;    /**< pointer to ring buffer for write */
    uint32_t rx_frames_rptr;    /**< pointer to ring buffer for read  */
    uint32_t rx_frames_num;     /**< number of frames in ring buffer  */
    uint32_t rx_filter_num;     /**< number of acceptance filters     */

    bool   powered_up;          /**< device is powered up */

    uint32_t events;            /**< events triggered by the last interrupt */
} can_t;

/** CAN device type can_t is redefined by ESP CAN */
#define HAVE_CAN_T

/**
 * ESP CAN device configuration
 */
typedef struct {
    uint32_t bitrate;           /**< Bitrate */
    gpio_t tx_pin;              /**< CAN transceiver TX pin */
    gpio_t rx_pin;              /**< CAN transceiver RX pin */
#ifdef CAN_CLK_OUT
    gpio_t clk_out_pin;         /**< optional CLK_OUT pin    */
#endif
#ifdef CAN_BUS_ON_OFF
    gpio_t bus_on_off_pin;      /**< optional BUS_ON_OFF pin */
#endif
} can_conf_t;

/** CAN device configuration type can_conf_t is redefined by ESP CAN */
#define HAVE_CAN_CONF_T

#ifdef __cplusplus
}
#endif

#endif /* CAN_ESP_H */
/** @} */
