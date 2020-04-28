/*
 * Copyright (C) 2019 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    usbus_cdc_acm_uart USBUS CDC ACM - USBUS CDC ACM - UART
 * @ingroup     usbus_cdc_acm
 * @brief       USB CDC ACM to UART functionality
 *
 * @{
 *
 * @file
 * @brief       Definition for USB CDC ACM to UART functionality
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_USBUS_CDC_ACM_UART_H
#define USB_USBUS_CDC_ACM_UART_H

#include "periph/uart.h"
#include "isrpipe.h"
#include "usb/usbus/cdc/acm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of the local buffer for UART data
 */
#ifndef USBUS_CDC_ACM_UART_BUF_SIZE
#define USBUS_CDC_ACM_UART_BUF_SIZE 128
#endif

/**
 * @brief Default baud rate at which the uart interface is initialized
 */
#define USBUS_CDC_ACM_UART_DEFAULT_BAUD 115200

/**
 * @brief usbus CDC ACM to UART handler struct
 */
typedef struct {
    usbus_cdcacm_device_t cdcacm;                   /**< CDC ACM USB function */
    uint8_t buf[USBUS_CDC_ACM_UART_BUF_SIZE];       /**< uart receive buffer  */
    uint8_t acmbuf[USBUS_CDC_ACM_UART_BUF_SIZE];    /**< USB receive buffer   */
    isrpipe_t usb2uart;                             /**< isrpipe to uart      */
    uint32_t baud;                                  /**< Current baud rate    */
    uart_t uart;                                    /**< UART device handler  */
} usbus_cdc_acm_uart_device_t;

/**
 * @brief Initialize a USB UART handler device
 *
 * This function inializes a thread to handle and buffer data from the USB
 * thread to the UART device and the data received from UART to the USB
 * function. Can be called multiple times for instantiating multiple USB to
 * UART functions
 *
 * @param usbus     USBUS thread context
 * @param acmuart   CDC ACM to UART handler struct
 * @param uart      UART device to use
 * @param stack     Stack space for the thread
 * @param stacksize Size of the stack in bytes
 * @param name      Name of the thread
 */
void usbus_cdc_acm_uart_init(usbus_t *usbus,
                             usbus_cdc_acm_uart_device_t *acmuart, uart_t uart,
                             char *stack, size_t stacksize, char *name);

#ifdef __cplusplus
}
#endif

#endif /* USB_USBUS_CDC_ACM_UART_H */
/** @} */
