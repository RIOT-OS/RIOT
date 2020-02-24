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

#include "net/netdev.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"
#include "common.h"
#include "at86rf2xx_params.h"

#define _STACKSIZE          (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#define MSG_TYPE_ISR        (0x3456)
#define RX_MSG_QUEUE_SIZE   (1 << 1)

static char stack[_STACKSIZE];
static kernel_pid_t _recv_pid;
static msg_t _msg_queue[RX_MSG_QUEUE_SIZE];

static const shell_command_t shell_commands[] = {
    { "ifconfig", "Configure netdev", ifconfig },
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { NULL, NULL, NULL }
};

const at86rf2xx_t *at86rf2xx_dev_ptrs[AT86RF2XX_NUM] = {0};
unsigned at86rf2xx_num_dev_ptrs = 0;

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = dev;
        int msg_send_result = msg_send(&msg, _recv_pid);
        if (msg_send_result <= 0) {
            printf("gnrc_netdev: possibly lost interrupt. (%d)\n",
                                                 msg_send_result);
        }
    }
    else {
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE:
            {
                recv(dev);

                break;
            }
            default:
                printf("Unexpected event received: %d\n", event);
                break;
        }
    }
}

void *_recv_thread(void *arg)
{
    (void)arg;
    msg_init_queue(_msg_queue, RX_MSG_QUEUE_SIZE);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev_t *dev = msg.content.ptr;
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
    netopt_enable_t en = NETOPT_ENABLE;
    netdev_t *netdev;

#if IS_USED(MODULE_AT86RF212B)
    at86rf212b_t at86rf212b_devs[AT86RF212B_NUM_OF];
    at86rf212b_setup(at86rf212b_devs, at86rf212b_params, AT86RF212B_NUM_OF);
    for (unsigned i = 0; i < AT86RF212B_NUM_OF; i++) {
        netdev = (netdev_t *)&((at86rf2xx_t *)&at86rf212b_devs[i])->base.netdev;
        netdev->event_callback = _event_cb;
        if (netdev->driver->init(netdev) < 0) {
            printf("at86rf212b radio #%u: initialization failed\n", i);
            continue;
        }
        printf("at86rf212b radio #%u: initialization successful\n", i);
        at86rf2xx_dev_ptrs[at86rf2xx_num_dev_ptrs++] =
            (at86rf2xx_t *)&at86rf212b_devs[i];
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }
#endif
#if IS_USED(MODULE_AT86RF231)
    at86rf231_t at86rf231_devs[AT86RF231_NUM_OF];
    at86rf231_setup(at86rf231_devs, at86rf231_params, AT86RF231_NUM_OF);
    for (unsigned i = 0; i < AT86RF231_NUM_OF; i++) {
        netdev = (netdev_t *)&((at86rf2xx_t *)&at86rf231_devs[i])->base.netdev;
        netdev->event_callback = _event_cb;
        if (netdev->driver->init(netdev) < 0) {
            printf("at86rf231 radio #%u: initialization failed\n", i);
            continue;
        }
        printf("at86rf231 radio #%u: initialization successful\n", i);
        at86rf2xx_dev_ptrs[at86rf2xx_num_dev_ptrs++] =
            (at86rf2xx_t *)&at86rf231_devs[i];
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }
#endif
#if IS_USED(MODULE_AT86RF232)
    at86rf232_t at86rf232_devs[AT86RF232_NUM_OF];
    at86rf232_setup(at86rf232_devs, at86rf232_params, AT86RF232_NUM_OF);
    for (unsigned i = 0; i < AT86RF232_NUM_OF; i++) {
        netdev = (netdev_t *)&((at86rf2xx_t *)&at86rf232_devs[i])->base.netdev;
        netdev->event_callback = _event_cb;
        if (netdev->driver->init(netdev) < 0) {
            printf("at86rf232 radio #%u: initialization failed\n", i);
            continue;
        }
        printf("at86rf232 radio #%u: initialization successful\n", i);
        at86rf2xx_dev_ptrs[at86rf2xx_num_dev_ptrs++] =
            (at86rf2xx_t *)&at86rf232_devs[i];
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }
#endif
#if IS_USED(MODULE_AT86RF233)
    at86rf233_t at86rf233_devs[AT86RF233_NUM_OF];
    at86rf233_setup(at86rf233_devs, at86rf233_params, AT86RF233_NUM_OF);
    for (unsigned i = 0; i < AT86RF233_NUM_OF; i++) {
        netdev = (netdev_t *)&((at86rf2xx_t *)&at86rf233_devs[i])->base.netdev;
        netdev->event_callback = _event_cb;
        if (netdev->driver->init(netdev) < 0) {
            printf("at86rf233 radio #%u: initialization failed\n", i);
            continue;
        }
        printf("at86rf233 radio #%u: initialization successful\n", i);
        at86rf2xx_dev_ptrs[at86rf2xx_num_dev_ptrs++] =
            (at86rf2xx_t *)&at86rf233_devs[i];
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }
#endif
#if IS_USED(MODULE_AT86RFA1)
    at86rfa1_t at86rfa1_dev;
    at86rfa1_setup(&at86rfa1_dev);
    netdev = (netdev_t *)&((at86rf2xx_t *)&at86rfa1_dev)->base.netdev;
    netdev->event_callback = _event_cb;
    if (netdev->driver->init(netdev) < 0) {
        printf("at86rfa1 initialization failed\n");
    }
    else {
        printf("at86rfa1 initialization successful\n");
        at86rf2xx_dev_ptrs[at86rf2xx_num_dev_ptrs++] =
            (at86rf2xx_t *)&at86rfa1_dev;
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }
#elif IS_USED(MODULE_AT86RFR2)
    at86rfr2_t at86rfr2_dev;
    at86rfr2_setup(&at86rfr2_dev);
    netdev = (netdev_t *)&((at86rf2xx_t *)&at86rfr2_dev)->base.netdev;
    netdev->event_callback = _event_cb;
    if (netdev->driver->init(netdev) < 0) {
        printf("at86rfr2 initialization failed\n");
    }
    else {
        printf("at86rfr2 initialization successful\n");
        at86rf2xx_dev_ptrs[at86rf2xx_num_dev_ptrs++] =
            (at86rf2xx_t *)&at86rfr2_dev;
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }
#endif

    if (!at86rf2xx_num_dev_ptrs) {
        puts("No device could be initialized");
        return 1;
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
