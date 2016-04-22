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

#include "net/netdev2.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"

#include "common.h"

#define _STACKSIZE      (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#define MSG_TYPE_ISR    (0x3456)

static char stack[_STACKSIZE];
static kernel_pid_t _recv_pid;

at86rf2xx_t devs[AT86RF2XX_NUM];

static const shell_command_t shell_commands[] = {
    { "ifconfig", "Configure netdev2", ifconfig },
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { NULL, NULL, NULL }
};

static void _event_cb(netdev2_t *dev, netdev2_event_t event, void *data)
{
    (void) data;

    if (event == NETDEV2_EVENT_ISR) {
        msg_t msg;

        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = (void *) dev;

        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("gnrc_netdev2: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case NETDEV2_EVENT_RX_COMPLETE:
            {
                recv(dev);

                break;
            }
            default:
                puts("Unexpected event received");
                break;
        }
    }
}

void *_recv_thread(void *arg)
{
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev2_t *dev = (netdev2_t *)msg.content.ptr;
            dev->driver->isr(dev);
        }
        else {
            puts("unexpected message type");
        }
    }
}

int main(void)
{
    puts("AT86RF2xx device driver test");
    xtimer_init();

    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        const at86rf2xx_params_t *p = &at86rf2xx_params[i];
        netdev2_t *dev = (netdev2_t *)(&devs[i]);

        printf("Initializing AT86RF2xx radio at SPI_%d\n", p->spi);
        at86rf2xx_setup(&devs[i], (at86rf2xx_params_t*) p);
        dev->event_callback = _event_cb;
        dev->driver->init(dev);
    }

    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              THREAD_CREATE_STACKTEST, _recv_thread, NULL,
                              "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        puts("Creation of receiver thread failed");
        return 1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
