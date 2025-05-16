/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @defgroup    usbus_cdc_acm_stdio  STDIO over CDC ACM (usbus)
 * @ingroup     sys_stdio
 * @brief       Standard input/output backend using usbus CDC ACM.
 *
 * This will provide STDIO via a virtual COM port over USB.
 * It can be enabled with
 *
 *    USEMODULE += stdio_cdc_acm
 *
 * @see         usbus_cdc_acm
 */

/**
 * @defgroup    usbus_cdc_acm USBUS CDC ACM - USBUS CDC abstract control model
 * @ingroup     usb
 * @brief       USBUS CDC ACM interface module
 *
 * @{
 *
 * @file
 * @brief       Interface and definitions for USB CDC ACM type interfaces in
 *              USBUS.
 *
 *              The functionality provided here only implements the USB
 *              specific handling. A different module is required to provide
 *              functional handling of the data e.g. UART or STDIO integration.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Koen Zandberg <koen@bergzand.net>
 */


#include <stdint.h>
#include "usb/cdc.h"
#include "usb/usbus.h"
#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup usbus_cdc_acm_conf USBUS CDC ACM compile time configurations
 * @ingroup usb_conf
 * @{
 */
/**
 * @brief Buffer size for STDIN and STDOUT data to and from USB when using
 *        the USBUS_CDC_ACM_STDIO module
 */
#ifdef CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE_EXP
#define CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE (1<<CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE_EXP)
#endif
#ifndef CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE
#define CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE (128)
#endif

/**
 * @brief USB CDC ACM bulk endpoint size
 */
#if IS_ACTIVE(CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE_8)
#define CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE     (8)
#elif IS_ACTIVE(CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE_16)
#define CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE    (16)
#elif IS_ACTIVE(CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE_32)
#define CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE    (32)
#elif IS_ACTIVE(CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE_64)
#define CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE    (64)
#endif

#ifndef CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE
#define CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE    (64)
#endif
/** @} */

/**
 * @brief USBUS CDC ACM interrupt endpoint size.
 */
#define USBUS_CDC_ACM_INT_EP_SIZE      (8)

/**
 * @brief Number of IN EPs required for the CDC ACM interface
 */
#define USBUS_CDC_ACM_EP_IN_REQUIRED_NUMOF   2

/**
 * @brief Number of Out EPs required for the CDC ACM interface
 */
#define USBUS_CDC_ACM_EP_OUT_REQUIRED_NUMOF  1

/**
 * @brief CDC ACM line state as reported by the host computer
 */
typedef enum {
    /**
     * @brief No DTE connected
     */
    USBUS_CDC_ACM_LINE_STATE_DISCONNECTED,

    /**
     * @brief DTE (e.g. a personal computer) is present and connected
     */
    USBUS_CDC_ACM_LINE_STATE_DTE
} usbus_cdcacm_line_state_t;

/**
 * @brief USBUS CDC ACM context struct forward declaration
 */
typedef struct usbus_cdcacm_device usbus_cdcacm_device_t;

/**
 * @brief CDC ACM data callback.
 *
 * Callback for received data from the USB host
 *
 * @param[in]   cdcacm  CDC ACM handler context
 * @param[in]   data    ptr to the data
 * @param[in]   len     Length of the received data
 */
typedef void (*usbus_cdcacm_cb_t)(usbus_cdcacm_device_t *cdcacm,
                                  uint8_t *data, size_t len);

/**
 * @brief CDC ACM line coding callback.
 *
 * Callback for received line coding request from the USB host
 *
 * @param[in]   cdcacm  CDC ACM handler context
 * @param[in]   baud    requested baud rate
 * @param[in]   bits    requested number of data bits
 * @param[in]   parity  requested parity
 * @param[in]   stop    requested number of stop bits
 *
 * @return              0 when the mode is available
 * @return              negative if the mode is not available
 */
typedef int (*usbus_cdcacm_coding_cb_t)(usbus_cdcacm_device_t *cdcacm,
                                        uint32_t baud, uint8_t bits,
                                        uint8_t parity, uint8_t stop);

/**
 * @brief USBUS CDC ACM context struct
 */
struct usbus_cdcacm_device {
    usbus_handler_t handler_ctrl;       /**< control handler                 */
    usbus_interface_t iface_ctrl;       /**< CDC control interface           */
    usbus_interface_t iface_data;       /**< CDC data interface              */
    usbus_descr_gen_t cdcacm_descr;     /**< CDC descriptor generator        */
    usbus_cdcacm_cb_t cb;               /**< Callback for data handlers      */
    usbus_cdcacm_coding_cb_t coding_cb; /**< Callback for ACM coding changes */
    tsrb_t tsrb;                        /**< TSRB for data to the host       */
    usbus_t *usbus;                     /**< USBUS reference                 */
    size_t occupied;                    /**< Number of bytes for the host    */
    usbus_cdcacm_line_state_t state;    /**< Current line state              */
    event_t flush;                      /**< device2host forced flush event  */
    usb_req_cdcacm_coding_t coding;     /**< Current coding configuration    */

    /**
     * @brief Host to device data buffer
     */
    usbdev_ep_buf_t out_buf[CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE];

    /**
     * @brief Device to host data buffer
     */
    usbdev_ep_buf_t in_buf[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
};

/**
 * @brief Initialize an USBUS CDC ACM interface
 *
 * @param[in]   usbus       USBUS context to register with
 * @param[in]   cdcacm      USBUS CDC ACM handler
 * @param[in]   cb          Callback for data from the USB interface
 * @param[in]   coding_cb   Callback for control settings
 * @param[in]   buf         Buffer for data to the USB interface
 * @param[in]   len         Size in bytes of the buffer
 */
void usbus_cdc_acm_init(usbus_t *usbus, usbus_cdcacm_device_t *cdcacm,
                        usbus_cdcacm_cb_t cb,
                        usbus_cdcacm_coding_cb_t coding_cb,
                        uint8_t *buf, size_t len);

/**
 * @brief Submit bytes to the CDC ACM handler
 *
 * @param[in]   cdcacm      USBUS CDC ACM handler context
 * @param[in]   buf         buffer to submit
 * @param[in]   len         length of the submitted buffer
 *
 * @return                  Number of bytes added to the CDC ACM ring buffer
 */
size_t usbus_cdc_acm_submit(usbus_cdcacm_device_t *cdcacm,
                           const uint8_t *buf, size_t len);

/**
 * @brief Flush the buffer to the USB host
 *
 * @param[in]   cdcacm      USBUS CDC ACM handler context
 */
void usbus_cdc_acm_flush(usbus_cdcacm_device_t *cdcacm);

/**
 * @brief Set the callback for control settings
 *
 * Interrupts are disabled during update to ensure thread safety
 *
 * @param[in]   cdcacm      USBUS CDC ACM handler context
 * @param[in]   coding_cb   Callback for control settings
 */
void usbus_cdc_acm_set_coding_cb(usbus_cdcacm_device_t *cdcacm,
                                 usbus_cdcacm_coding_cb_t coding_cb);

#ifdef __cplusplus
}
#endif

/** @} */
