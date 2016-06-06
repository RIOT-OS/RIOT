/*
 * Copyright (C) 2015 INRIA
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
 * @brief       Example application for demonstrating the S-CoSenS MAC "leaf
 *              node variant" layer for RIOT "gnrc" network stack.
 *              Right now, this example only works for AT86RF2XX radios...
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "board.h"
#include "msg.h"
#include "panic.h"

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "net/gnrc/s_cosens_node.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int s_cosens_send_cmd(int argc, char **argv);
extern void start_frame_rx_server(void);


at86rf2xx_t at86rf2xx_dev;
static kernel_pid_t s_cosens_mac_tid;

static char mac_thread_stack[THREAD_STACKSIZE_DEFAULT];

static const shell_command_t shell_commands[] = {
    { "send_frame", "send frame (S-CoSenS for leaf nodes)", s_cosens_send_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT S-CoSenS (node) MAC layer example application.");

    /* I have to start all the network machinery here, since the
       'auto_init_' modules all sytematicaly start "nomac" */
    DEBUG("Initializing AT86RF2xx radio at SPI_0\n");
    const at86rf2xx_params_t *p = &at86rf2xx_params[0];
    int res = at86rf2xx_init(&at86rf2xx_dev,
                             p->spi,
                             p->spi_speed,
                             p->cs_pin,
                             p->int_pin,
                             p->sleep_pin,
                             p->reset_pin);
    if (res < 0) {
        core_panic(PANIC_ASSERT_FAIL,
                   "Error initializing AT86RF2xx radio device!");
    }
    else {
        /* start the 'gnrc_s_cosens_node' module */
        s_cosens_mac_tid = gnrc_s_cosens_node_init(mac_thread_stack,
                                                   THREAD_STACKSIZE_DEFAULT,
                                                   THREAD_PRIORITY_MAIN - 4,
                                                   "s_cosens_node_mac",
                                                   (gnrc_netdev_t *)&at86rf2xx_dev);
    }

    /* Start IEEE 802.15.4 frame RX server */
    start_frame_rx_server();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
