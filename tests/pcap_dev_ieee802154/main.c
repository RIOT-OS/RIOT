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
#include "pcap_dev/sd_file.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"

#include "common.h"

#define _STACKSIZE      (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#define MSG_TYPE_ISR    (0x3456)
#define DRAIN_NAME      "drain.pcap"
#define SINK_NAME       "sink.pcap"
#define SINK_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

static char stack[_STACKSIZE];
static uint8_t _drain_buf[127 + sizeof(pcaprec_hdr_t)];
static pcap_dev_sd_file_state_t _sd_state;
static kernel_pid_t _recv_pid;

pcap_dev_t dev;

static const shell_command_t shell_commands[] = {
    { "ifconfig", "Configure netdev2", ifconfig },
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { NULL, NULL, NULL }
};

static void _event_cb(netdev2_t *netdev, netdev2_event_t event, void *data)
{
    (void) data;

    if (event == NETDEV2_EVENT_ISR) {
        msg_t msg;

        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = (void *) netdev;

        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("gnrc_netdev2: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case NETDEV2_EVENT_RX_COMPLETE:
            {
                recv(netdev);

                break;
            }
            case NETDEV2_EVENT_RX_STARTED:
                break;
            default:
                puts("Unexpected event received");
                break;
        }
    }
}

void *_recv_thread(void *arg)
{
    (void)arg;
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev2_t *netdev = (netdev2_t *)msg.content.ptr;
            netdev->driver->isr(netdev);
        }
        else {
            puts("unexpected message type");
        }
    }
    return NULL;
}

int main(void)
{
    netdev2_t *netdev = (netdev2_t *)(&dev);

    puts("PCAP device driver test");
    xtimer_init();

    _sd_state.sink_fname = SINK_NAME;
    _sd_state.sink_mode = SINK_MODE;
    _sd_state.drain_fname = DRAIN_NAME;
    pcap_dev_setup_ieee802154(&dev, &pcap_dev_sd_file, &_sd_state, 1000000,
                              _drain_buf, sizeof(_drain_buf));
    netdev->event_callback = _event_cb;
    netdev->driver->init(netdev);

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
