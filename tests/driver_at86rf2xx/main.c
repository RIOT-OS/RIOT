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
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "kernel.h"
#include "shell.h"
#include "shell_commands.h"
#include "posix_io.h"
#include "board_uart0.h"
#include "ng_at86rf2xx.h"
#include "net/ng_netbase.h"
#include "net/ng_nomac.h"
#include "net/ng_pktdump.h"

/* make sure the SPI port and the needed GPIO pins are defined */
#ifndef ATRF_SPI
#error "SPI not defined"
#endif
#ifndef ATRF_CS
#error "Chip select pin not defined"
#endif
#ifndef ATRF_INT
#error "Interrupt pin not defined"
#endif
#ifndef ATRF_SLEEP
#error "Sleep pin not defined"
#endif
#ifndef ATRF_RESET
#error "Reset pin not defined"
#endif
#ifndef ATRF_SPI_SPEED
#define ATRF_SPI_SPEED          (SPI_SPEED_5MHZ)
#endif

/**
 * @brief   Buffer size used by the shell
 */
#define SHELL_BUFSIZE           (64U)

/**
 * @brief   Allocate the AT86RF2xx device descriptor
 */
static ng_at86rf2xx_t dev;

/**
 * @brief   Stack for the nomac thread
 */
static char nomac_stack[KERNEL_CONF_STACKSIZE_MAIN];

/**
 * @brief   Read chars from STDIO
 */
int shell_read(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

/**
 * @brief   Write chars to STDIO
 */
void shell_put(int c)
{
    putchar((char)c);
}

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    kernel_pid_t iface;
    int res;
    shell_t shell;
    ng_netreg_entry_t dump;

    puts("AT86RF2xx device driver test");
    printf("Initializing the radio at SPI_%i... \n", ATRF_SPI);

    /* register the pktdump thread */
    puts("Register the packet dump thread for NG_NETTYPE_UNDEF packets");
    dump.pid = ng_pktdump_getpid();
    dump.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    ng_netreg_register(NG_NETTYPE_UNDEF, &dump);

    /* initialize the AT86RF2xx device */
    puts("Initialize the AT86RF2xx radio device");
    res = ng_at86rf2xx_init(&dev, ATRF_SPI, ATRF_SPI_SPEED,
                            ATRF_CS, ATRF_INT,
                            ATRF_SLEEP, ATRF_RESET);
    if (res < 0) {
        puts("Error initializing AT86RF2xx radio device");
        return -1;
    }

    /* start MAC layer */
    puts("Starting the NOMAC layer on top of the driver");
    iface = ng_nomac_init(nomac_stack, sizeof(nomac_stack), PRIORITY_MAIN - 3,
                          "at86rf2xx", (ng_netdev_t *)&dev);
    if (iface <= KERNEL_PID_UNDEF) {
        puts("Error initializing MAC layer");
        return -1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, shell_read, shell_put);
    shell_run(&shell);

    return 0;
}
