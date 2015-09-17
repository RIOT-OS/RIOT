/*
 * Copyright (C) 2015 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  usbdev_stack
 * @{
 *
 * @file
 * @brief       USB device driver interface definitions.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef USBDEV_DRIVER_H
#define USBDEV_DRIVER_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include "thread.h"
#include "usbdev_config.h"
#include "usb_datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USBDEV_CTRL_EP              0

typedef enum {
    USBDEV_STRING_IDX_MFR = 1,
    USBDEV_STRING_IDX_PRODUCT,
    USBDEV_STRING_IDX_SN,
    USBDEV_STRING_IDX_CIF,
    USBDEV_STRING_IDX_DIF,
} usbdev_string_idx_t;

typedef enum {
    USBDEV_STAGE_IDLE = 0,
    USBDEV_STAGE_STATUS_IN,
    USBDEV_STAGE_DATA_IN,
    USBDEV_STAGE_DATA_OUT,
    USBDEV_STAGE_STALL,
} usbdev_stage_t;

typedef uint32_t usb_ep_t;

typedef enum {
    USBDEV_EVENT_SETUP = 0,
    USBDEV_EVENT_OUT,
    USBDEV_EVENT_IN,
    USBDEV_EVENT_REQUEST_CLASS_SETUP,
    USBDEV_EVENT_REQUEST_CLASS_OUT,
    USBDEV_EVENT_STALL,
    USBDEV_EVENT_RESET,
    USBDEV_EVENT_WAKEUP,
    USBDEV_EVENT_SUSPEND,
    USBDEV_EVENT_RESUME,
    USBDEV_EVENT_SOF,
    USBDEV_EVENT_ERROR,
} usbdev_event_t;

/**
 * @brief   Definition of basic usb device options.
 */
typedef enum {
    USBDEV_SET_CONNECT,             /**< Connect the USB device */
    USBDEV_SET_DISCONNECT,          /**< Disconnect the USB device */
    USBDEV_SET_ADDRESS,             /**< Set USB device address */
    USBDEV_OPT_EP_RESET,            /**< Reset USB device endpoint */
    USBDEV_SET_EP_ENABLE,           /**< Enable an endpoint */
    USBDEV_SET_EP_DISABLE,          /**< Disable an endpoint */
    USBDEV_SET_EP_STALL,            /**< Set STALL for an endpoint */
    USBDEV_SET_EP_CLR_STALL,        /**< Clear STALL for an endpoint */
} usbdev_cmd_t;

/**
 * @brief   Definition of the usb device type
 *
 * @see     struct usbdev_t
 *
 * @note    Forward definition to use in @ref usbdev_ops_t
 */
typedef struct usbdev_t usbdev_t;

/**
 * @brief   usb device API definition.
 *
 * @details This is a set of functions that must be implemented by any driver
 *           for a usb device.
 */
typedef struct {
    usbdev_t *dev;
    /**
     * @brief Initialize a usb device.
     *
     * @return  0 on success
     */
    int (*init)(void);

    /**
     * @brief Configure a given usb device endpoint
     *
     * @param[in] ep                the usb device endpoint number
     * @param[in] size              the enpoint packet size
     * @return  a error
     */
    int (*configure_ep)(usb_ep_t ep, size_t size);

    /**
     * @brief Set Toggle Bit
     *
     * @param[in] ep                the usb device endpoint number
     * @return  a error
     */
    int (*set_ep_toggle_bit)(usb_ep_t ep);

    /**
     * @brief Clear Toggle Bit
     *
     * @param[in] ep                the usb device endpoint number
     * @return  a error
     */
    int (*clr_ep_toggle_bit)(usb_ep_t ep);

    /**
     * @brief Write data to a given usb device endpoint buffer
     *
     * @param[in] ep                the usb endpoint
     * @param[in] data              the data to write
     * @param[in] data_len          the length of *data* in byte
     * @return  a error
     */
    int (*write_ep)(usb_ep_t ep, uint8_t * data, size_t data_len);

    /**
     * @brief Read data from a given usb device endpoint buffer
     *
     * @param[in] ep                the usb endpoint
     * @param[in] data              the data to read
     * @param[in] data_len          the length of *data* in byte
     * @return  a error
     */
    int (*read_ep)(usb_ep_t ep, uint8_t *data, size_t size);

    /**
     * @brief   Set an option value for a given usb device.
     *
     * @param[in] opt           the option type
     * @param[in] value         the value to set
     * @return  a error
     */
    int (*ioctl)(usbdev_cmd_t cmd, uint32_t arg);

} usbdev_ops_t;

typedef int (* usbdev_iface_init_t)(usbdev_t *dev);
typedef usbdev_ops_t *(* usbdev_driver_init_t)(usbdev_t *dev);

/**
 * @brief   Definition of the usb device type
 *
 */
struct usbdev_t {
    usbdev_ops_t *driver;      /**< The driver for this device */
    void (*irq_ep_event)(usbdev_t *dev, usb_ep_t ep, uint16_t event_type);
    void (*irq_bc_event)(usbdev_t *dev, uint16_t event_type);

    uint8_t *string_descriptor;
    usb_device_descriptor_t *device_descriptor;

    uint8_t *sdescr_tab[USBDEV_DESCRIPTOR_IDX_MAX];
    uint8_t *cfgdescr_tab[USBDEV_DESCRIPTOR_IDX_MAX];

    kernel_pid_t ep_in_pid[USBDEV_MAX_ENDPOINT_IN];
    kernel_pid_t ep_out_pid[USBDEV_MAX_ENDPOINT_OUT];

    usb_device_request_t setup_pkt;

    uint8_t *ep0_data_ptr;
    uint16_t ep0_data_cnt_tx;
    uint16_t ep0_data_cnt_rx;
    uint8_t ep0_buf[USBDEV_EP0_MAX_PACKET];
    uint8_t zero_pkt;

    uint8_t address;
    uint16_t status;
    uint8_t configuration;
    uint32_t ep_configured;
    uint32_t ep_halted;
    uint32_t ep_stalled;
    uint8_t n_interfaces;

    uint8_t alt_setting[USBDEV_MAX_INTERFACES];

    usbdev_stage_t stage;
};

int usbdev_register_iface(usbdev_iface_init_t init);
int usbdev_register_driver(usbdev_driver_init_t init);
int usbdev_add_string_descriptor(usbdev_t *dev, uint8_t index, char *string);
int usbdev_add_cfg_descriptor(usbdev_t *dev, uint8_t *descriptor);
int usbdev_remove(usbdev_t *dev);
uint32_t ep_configured(usbdev_t *dev, usb_ep_t ep);

#ifdef __cplusplus
}
#endif

#endif /* USBDEV_DRIVER_H */
/**
 * @}
 */
