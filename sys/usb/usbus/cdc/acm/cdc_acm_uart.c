/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_acm Virtual Serial Port UART implementation
 * @{
 *
 * @file
 * @brief CDC ACM UART implementation
 *
 * This file implements a USB CDC ACM callback and read functions from an UART
 * peripheral.
 * @}
 */

#include "periph/uart.h"
#include "usb/cdc.h"
#include "usb/usbus/cdc/acm_uart.h"

#ifdef PERIPH_UART_MODECFG
static int _acm2uart_bits(uint8_t bits, uart_data_bits_t *conv)
{
    switch (bits) {
        case 5:
            *conv = UART_DATA_BITS_5;
            break;
        case 6:
            *conv = UART_DATA_BITS_6;
            break;
        case 7:
            *conv = UART_DATA_BITS_7;
            break;
        case 8:
            *conv = UART_DATA_BITS_8;
            break;
        default:
            return -1;
    }
    return 0;
}

static int _acm2uart_parity(uint8_t parity, uart_parity_t *conv)
{
    switch (parity) {
        case USB_CDC_ACM_CODING_PARITY_NONE:
            *conv = UART_PARITY_NONE;
            break;
        case USB_CDC_ACM_CODING_PARITY_EVEN:
            *conv = UART_PARITY_EVEN;
            break;
        case USB_CDC_ACM_CODING_PARITY_ODD:
            *conv = UART_PARITY_ODD;
            break;
        case USB_CDC_ACM_CODING_PARITY_MARK:
            *conv = UART_PARITY_MARK;
            break;
        case USB_CDC_ACM_CODING_PARITY_SPACE:
            *conv = UART_PARITY_SPACE;
            break;
        default:
            return -1;
    }
    return 0;
}

static int _acm2uart_stop(uint8_t stop, uart_stop_bits_t *conv)
{
    switch (stop) {
        case USB_CDC_ACM_CODING_STOP_BITS_1:
            *conv = UART_STOP_BITS_1;
            break;
        case USB_CDC_ACM_CODING_STOP_BITS_2:
            *conv = UART_STOP_BITS_2;
            break;
        default:
            return -1;
    }
    return 0;
}
#endif

static void _rx_cb(void *arg, uint8_t data)
{
    usbus_cdc_acm_uart_device_t *acmuart = (usbus_cdc_acm_uart_device_t *)arg;

    usbus_cdc_acm_submit(&acmuart->cdcacm, &data, 1);
    if (data == '\n') {
        usbus_cdc_acm_flush(&acmuart->cdcacm);
    }
}

static int _coding_cb(usbus_cdcacm_device_t *cdcacm, uint32_t baud,
                      uint8_t bits, uint8_t parity, uint8_t stop)
{
    usbus_cdc_acm_uart_device_t *acmuart =
        container_of(cdcacm, usbus_cdc_acm_uart_device_t, cdcacm);

#ifdef PERIPH_UART_MODECFG
    uart_data_bits_t ubits = UART_DATA_BITS_8;
    uart_parity_t uparity = UART_PARITY_NONE;
    uart_stop_bits_t ustop = UART_STOP_BITS_1;
    if (_acm2uart_bits(bits, &ubits) < 0 ||
        _acm2uart_parity(parity, &uparity) < 0 ||
        _acm2uart_stop(stop, &ustop) < 0) {
        return -1;
    }
#else
    if (bits != 8 ||
        parity != USB_CDC_ACM_CODING_PARITY_NONE ||
        stop != USB_CDC_ACM_CODING_STOP_BITS_1) {
        return -1;
    }
#endif

    if (baud != acmuart->baud) {
        uart_init(acmuart->uart, baud, _rx_cb, acmuart);
        acmuart->baud = baud;
    }
#ifdef PERIPH_UART_MODECFG
    if (uart_mode(acmuart->uart, ubits, uparity, ustop) == UART_OK) {
        return 0;
    }
    return -1;
#endif
    return 0;
}

void _submit_pipe(usbus_cdcacm_device_t *cdcacm,
                  uint8_t *data, size_t len)
{
    usbus_cdc_acm_uart_device_t *acmuart =
        container_of(cdcacm, usbus_cdc_acm_uart_device_t, cdcacm);

    for (size_t i = 0; i < len; i++) {
        isrpipe_write_one(&acmuart->usb2uart, data[i]);
    }
}

void *_thread(void *arg)
{
    usbus_cdc_acm_uart_device_t *acmuart = (usbus_cdc_acm_uart_device_t *)arg;

    while (1) {
        uint8_t uartbuf[16];
        size_t bytes = isrpipe_read(&acmuart->usb2uart,
                                    uartbuf, sizeof(uartbuf));
        uart_write(acmuart->uart, uartbuf, bytes);
    }
    return NULL;
}

void usbus_cdc_acm_uart_init(usbus_t *usbus,
                             usbus_cdc_acm_uart_device_t *acmuart, uart_t uart,
                             char *stack, size_t stacksize, char *name)
{
    acmuart->uart = uart;
    isrpipe_init(&acmuart->usb2uart, acmuart->buf,
                 USBUS_CDC_ACM_UART_BUF_SIZE);
    uart_init(acmuart->uart, USBUS_CDC_ACM_UART_DEFAULT_BAUD, _rx_cb, acmuart);
    usbus_cdc_acm_init(usbus, &acmuart->cdcacm, _submit_pipe, _coding_cb,
                       acmuart->acmbuf, USBUS_CDC_ACM_UART_BUF_SIZE);

    thread_create(stack, stacksize, USBUS_PRIO + 1, THREAD_CREATE_STACKTEST,
                  _thread, (void *)acmuart, name);
}
