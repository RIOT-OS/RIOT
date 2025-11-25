/*
 * SPDX-FileCopyrightText: 2019 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_usbdev_mock USBdev mockup device
 * @ingroup     drivers_misc
 * @brief       USBdev mockup device for testing
 * @{
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of IN and OUT endpoints available in the mock usbdev device
 */
#define USBDEV_MOCK_NUM_EP      (1)

/**
 * @name usbdev mock device endpoint states
 * @{
 */
typedef enum {
    EP_STATE_READY,             /**< Endpoint is ready/idle */
    EP_STATE_DATA_AVAILABLE,    /**< Endpoint has data      */
} usbdev_mock_ep_state_t;
/** @} */

/**
 * @brief usbdev mock device endpoint
 */
typedef struct {
    usbdev_ep_t ep;                 /**< Generic endpoint struct                    */
    usbdev_mock_ep_state_t state;   /**< Endpoint state                             */
    size_t available;               /**< Bytes available in the buffer              */
    uint8_t *target_buf;            /**< Buffer as passed by @ref usbdev_ep_xmit    */
    uint8_t *buf;                   /**< Internal mock buffer pointer */
} usbdev_mock_ep_t;

/**
 * @brief usbdev mock device forward declaration
 */
typedef struct usbdev_mock usbdev_mock_t;

/**
 * @brief usbdev mock device callback for esr event checking
 *
 * @param dev   usbdev mock device descriptor
 */
typedef void (*usbdev_mock_esr_cb_t)(usbdev_mock_t *dev);

/**
 * @brief usbdev mock device callback for endpoint event checking
 *
 * @param dev   usbdev mock device descriptor
 * @param ep    usbdev mock device endpoint
 */
typedef void (*usbdev_mock_ep_esr_cb_t)(usbdev_mock_t *dev,
                                        usbdev_mock_ep_t *ep);

/**
 * @brief usbdev mock device callback for endpoint ready call checking
 *
 * @param dev   usbdev mock device descriptor
 * @param ep    usbdev mock device endpoint
 * @param len   length supplied to the ready call
 */
typedef void (*usbdev_mock_ready_cb_t)(usbdev_mock_t *dev,
                                       usbdev_mock_ep_t *ep,
                                       size_t len);

/**
 * @brief usbdev mock device
 */
struct usbdev_mock {
    usbdev_t usbdev;                          /**< Generic usbdev device      */
    usbdev_mock_ep_t in[USBDEV_MOCK_NUM_EP];  /**< IN endpoints               */
    usbdev_mock_ep_t out[USBDEV_MOCK_NUM_EP]; /**< OUT endpoints              */
    uint8_t config_addr;                      /**< Configured address         */
    size_t req_len;                           /**< Length of the current
                                                *  request                    */
    usbdev_mock_esr_cb_t esr_cb;              /**< ESR callback for
                                                *  validation                 */
    usbdev_mock_ep_esr_cb_t ep_esr_cb;        /**< Endpoint ESR cb for
                                                *  validation                 */
    usbdev_mock_ready_cb_t ready_cb;          /**< Endpoint ready cb for
                                                *  validation                 */
};

/**
 * @brief usbdev mock device setup function
 *
 * @param esr_cb    ESR callback
 * @param ep_esr_cb Endpoint ESR callback
 * @param ready_cb  Endpoint ready callback
 */
void usbdev_mock_setup(usbdev_mock_esr_cb_t esr_cb,
                       usbdev_mock_ep_esr_cb_t ep_esr_cb,
                       usbdev_mock_ready_cb_t ready_cb);

#ifdef __cplusplus
}
#endif

/** @} */
