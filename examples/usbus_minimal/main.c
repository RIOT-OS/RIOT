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
#include "shell.h"
#include "msg.h"

static char _stack[USBUS_STACKSIZE];

static usbus_t usbus;
/* TODO: remove as soon as we have decent auto_init */
#include "sam_usb.h"
static sam0_common_usb_t usbdev;

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT USB stack example application");
    
    /* TODO: remove as soon as we have decent auto_init */
    sam_usbdev_setup(&usbdev);
    /* start usb stack */
    usbus_init(&usbus, (usbdev_t*)&usbdev);
    usbus_create(_stack, sizeof(_stack), USBUS_PRIO, USBUS_TNAME, &usbus);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
