/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT USB stack
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "fmt.h"
#include "shell.h"
#include "shell_commands.h"
#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"
#include "usb/usbus/cdc/acm_uart.h"

static char _stack[USBUS_STACKSIZE];

static usbus_t usbus;

#define MIN(a,b)        (((a) < (b)) ? a : b)
/* Max number of USB to UART functions.                                    *
 * Reduced by 3 for the control endpoint and the two endpoints used by the *
 * STDIO CDC ACM module. Divided by two as each CDC ACM function requires  *
 * two endpoints in each direction                                         */
#define USB_ACM_MAX     ((USBDEV_NUM_ENDPOINTS - 1U)/2U)
/* Determine the number of USB to UART functions possible */
#define NUMOF_ACM       MIN(USB_ACM_MAX, UART_NUMOF)

static usbus_cdc_acm_uart_device_t acmuart[NUMOF_ACM];
static char _cdc_acm_uart_stack[NUMOF_ACM][THREAD_STACKSIZE_SMALL];
static char _cdc_acm_uart_name[NUMOF_ACM][16];

static void _init_usb(void)
{
    /* Get driver context */
    usbdev_t *usbdev = usbdev_get_ctx(0);
    assert(usbdev);

    /* Initialize basic usbus struct, don't start the thread yet */
    usbus_init(&usbus, usbdev);

    /* Instantiate the threads */
    for(unsigned i = 0; i < NUMOF_ACM; i++) {
        static const char basename[] = "uart: ";
        memcpy(_cdc_acm_uart_name[i], basename, sizeof(basename));
        fmt_u32_dec(&_cdc_acm_uart_name[i][strlen(basename)], i);
        usbus_cdc_acm_uart_init(&usbus, &acmuart[i], UART_DEV(i),
                                _cdc_acm_uart_stack[i],
                                sizeof(_cdc_acm_uart_stack[i]),
                                _cdc_acm_uart_name[i]);
    }

    /* And create and start the usbus stack */
    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
}

int main(void)
{
    _init_usb();
}
