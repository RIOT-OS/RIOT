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
#include "usb/usbus.h"

static char _stack[USBUS_STACKSIZE];

static usbus_t usbus;
/* TODO: remove as soon as we have decent auto_init */
#include "periph_conf.h"
#include "sam_usb.h"

int main(void)
{
    puts("RIOT USB stack example application");

    /* TODO: remove as soon as we have decent auto_init */
    usbdev_t *usbdev_ctx = usbdev_get_ctx(0);
    /* start usb stack */
    usbus_init(&usbus, usbdev_ctx);
    usbus_create(_stack, sizeof(_stack), USBUS_PRIO, USBUS_TNAME, &usbus);

    /* start shell */
    puts("Started USB stack!");
    return 0;
}
