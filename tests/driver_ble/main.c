/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the NRF51822 BLE minimal radio driver (blemin)
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "posix_io.h"
#include "board_uart0.h"
#include "net/gnrc.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc/pktdump.h"
#include "net/ble_ll.h"
#include "nrf51_ble.h"

#define SHELL_BUFSIZE       (UART0_BUFSIZE)

static char nomac_stack[THREAD_STACKSIZE_DEFAULT];

int main(void)
{
    shell_t shell;
    gnrc_netdev_t dev;

    puts("\nManual test for the minimal NRF51822 BLE radio driver\n");
    puts("Use the 'ifconfig' and 'blesnd' shell commands to verify the driver");

    /* initialize network device */
    nrf51_ble_init(&dev);
    ble_ll_init(nomac_stack, sizeof(nomac_stack), 5, "ble_ll", &dev);

    /* initialize and run the shell */
    board_uart0_init();
    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}
