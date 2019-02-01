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
#include "msg.h"

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    puts("RIOT USB stack example application");

    /* start usb stack */
    usbus_init();


    /* should be never reached */
    return 0;
}
