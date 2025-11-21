/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Simple shell application to test the USB board reset function
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"

#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"

#ifdef MODULE_USBUS_DFU
#include "usb/usbus/dfu.h"
static usbus_dfu_device_t _dfu;
#endif

static char line_buf[SHELL_DEFAULT_BUFSIZE];

static usbus_t _usbus;
static char _stack[USBUS_STACKSIZE];

static void _init(void)
{
    usbus_init(&_usbus, usbdev_get_ctx(0));

#ifdef MODULE_STDIO_CDC_ACM
    /* if stdio_cdc_acm is used, initialize it */
    void usb_cdc_acm_stdio_init(usbus_t *_usbus);
    usb_cdc_acm_stdio_init(&_usbus);
#else
    /* otherwise create a device CDC ACM and initialize it */
    static usbus_cdcacm_device_t _cdcacm;
    /* buffer is required in usbus_cdc_acm_init, use only a single character */
    static uint8_t _cdcacm_buf;
    usbus_cdc_acm_init(&_usbus, &_cdcacm, NULL, NULL, &_cdcacm_buf, 1);
#endif

#ifdef MODULE_USBUS_DFU
    usbus_dfu_init(&_usbus, &_dfu, USB_DFU_PROTOCOL_RUNTIME_MODE);
#endif

    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &_usbus);
}

int main(void)
{
    _init();

    shell_run(NULL, line_buf, ARRAY_SIZE(line_buf));

    return 0;
}
