/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-FileCopyrightText: 2020-2022 Université Grenoble Alpes
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test Application For SX1280 Driver
 *
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "architecture.h"
#include "msg.h"
#include "thread.h"
#include "shell.h"

#include "net/lora.h"
#include "net/netdev.h"
#include "net/netdev/lora.h"

#include "sx1280.h"
#include "sx1280_params.h"
#include "sx1280_netdev.h"

#define SX1280_MSG_QUEUE        (8U)
#define SX1280_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#define SX1280_MSG_TYPE_ISR     (0x3456)
#define SX1280_MAX_PAYLOAD_LEN  (128U)

static char stack[SX1280_STACKSIZE];
static kernel_pid_t _recv_pid;

static char message[SX1280_MAX_PAYLOAD_LEN];

static sx1280_t sx1280;

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;
        msg.type = SX1280_MSG_TYPE_ISR;
        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("sx1280_netdev: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
        case NETDEV_EVENT_RX_STARTED:
            puts("Data reception started");
            break;

        case NETDEV_EVENT_RX_COMPLETE:
        {
            size_t len = dev->driver->recv(dev, NULL, 0, 0);
            netdev_lora_rx_info_t packet_info;
            dev->driver->recv(dev, message, len, &packet_info);
            printf(
                "Received: \"%s\" (%" PRIuSIZE " bytes) - [RSSI: %i, SNR: %i]\n",
                message, len, packet_info.rssi, packet_info.snr);
        }
        break;

        case NETDEV_EVENT_TX_COMPLETE:
            puts("Transmission completed");
            break;

        case NETDEV_EVENT_TX_TIMEOUT:
            puts("Transmission timeout");
            break;

        default:
            printf("Unexpected netdev event received: %d\n", event);
            break;
        }
    }
}

void *_recv_thread(void *arg)
{
    netdev_t *netdev = arg;

    static msg_t _msg_queue[SX1280_MSG_QUEUE];

    msg_init_queue(_msg_queue, SX1280_MSG_QUEUE);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == SX1280_MSG_TYPE_ISR) {
            netdev->driver->isr(netdev);
        }
        else {
            puts("Unexpected msg type");
        }
    }
}

static void _get_usage(const char *cmd)
{
    printf("Usage: %s get <type|freq|bw|sf|cr>\n", cmd);
}

static void _usage_freq(void)
{
    printf("Usage: use freq between 2400000000 + (bw/2) and 2500000000 - (bw/2) (Hz) !\n");
}

static void _usage_bw(void)
{
    printf("Usage: use 200, 400, 800, 1600 (kHz)\n");
}

static void _usage_sf(void)
{
    printf("Usage: use SF between 5 and 12\n");
}

static void _usage_cr(void)
{
    printf(
        "Usage: use\n \
    LORA_CR_4_5 = 1\n \
    LORA_CR_4_6 = 2\n \
    LORA_CR_4_7 = 3\n \
    LORA_CR_4_8 = 4\n \
    LORA_CR_LI_4_5 = 5\n \
    LORA_CR_LI_4_6 = 6\n \
    LORA_CR_LI_4_8 = 7\n");
}

static int sx1280_get_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 2) {
        _get_usage(argv[0]);
        return -1;
    }

    if (!strcmp("type", argv[2])) {
        uint16_t type;
        netdev->driver->get(netdev, NETOPT_DEVICE_TYPE, &type, sizeof(uint16_t));
        printf("Device type: %s\n", (type == NETDEV_TYPE_LORA) ? "lora" : "fsk");
    }
    else if (!strcmp("freq", argv[2])) {
        uint32_t freq;
        netdev->driver->get(netdev, NETOPT_CHANNEL_FREQUENCY, &freq, sizeof(uint32_t));
        printf("Frequency: %" PRIu32 " Hz\n", freq);
    }
    else if (!strcmp("bw", argv[2])) {
        uint32_t bw_val = 0;
        netdev->driver->get(netdev, NETOPT_BANDWIDTH, &bw_val, sizeof(uint32_t));
        printf("Bandwidth: %" PRIu32 " kHz\n", bw_val / 1000);
    }
    else if (!strcmp("sf", argv[2])) {
        uint8_t sf;
        netdev->driver->get(netdev, NETOPT_SPREADING_FACTOR, &sf, sizeof(uint8_t));
        printf("Spreading factor: %d\n", sf);
    }
    else if (!strcmp("cr", argv[2])) {
        uint8_t cr;
        netdev->driver->get(netdev, NETOPT_CODING_RATE, &cr, sizeof(uint8_t));
        printf("Coding rate: %d\n", cr);
        _usage_cr();
    }
    else {
        _get_usage(argv[0]);
        return -1;
    }

    return 0;
}

static void _set_usage(const char *cmd)
{
    printf("Usage: %s set <freq|bw|sf|cr|> <value>\n", cmd);
}

static int sx1280_set_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 3) {
        if (!strcmp("freq", argv[2])) {
            _usage_freq();
        }
        if (!strcmp("bw", argv[2])) {
            _usage_bw();
        }
        if (!strcmp("sf", argv[2])) {
            _usage_sf();
        }
        if (!strcmp("cr", argv[2])) {
            _usage_cr();
        }
    }
    if (argc != 4) {
        _set_usage(argv[0]);
        return -1;
    }

    int ret = 0;

    if (!strcmp("freq", argv[2])) {
        uint32_t freq = strtoul(argv[3], NULL, 10);
        ret = netdev->driver->set(netdev, NETOPT_CHANNEL_FREQUENCY, &freq, sizeof(uint32_t));
    }
    else if (!strcmp("bw", argv[2])) {
        uint16_t bw = atoi(argv[3]);
        ret = netdev->driver->set(netdev, NETOPT_BANDWIDTH, &bw, sizeof(uint16_t));
    }
    else if (!strcmp("sf", argv[2])) {
        uint8_t sf = atoi(argv[3]);
        ret = netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR, &sf, sizeof(uint8_t));
    }
    else if (!strcmp("cr", argv[2])) {
        uint8_t cr = atoi(argv[3]);
        ret = netdev->driver->set(netdev, NETOPT_CODING_RATE, &cr, sizeof(uint8_t));
    }
    else {
        _set_usage(argv[0]);
        return -1;
    }

    if (ret < 0) {
        printf("cannot set %s\n", argv[2]);
        return ret;
    }

    printf("%s set\n", argv[2]);
    return 0;
}

static void _rx_usage(const char *cmd)
{
    printf("Usage: %s rx <start|stop>\n", cmd);
}

static int sx1280_rx_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 2) {
        _rx_usage(argv[0]);
        return -1;
    }

    if (!strcmp("start", argv[2])) {
        /* Switch to RX (IDLE) state */
        netopt_state_t state = NETOPT_STATE_IDLE;
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
        printf("Listen mode started\n");
    }
    else if (!strcmp("stop", argv[2])) {
        /* Switch to STANDBY state */
        netopt_state_t state = NETOPT_STATE_STANDBY;
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
        printf("Listen mode stopped\n");
    }
    else {
        _rx_usage(argv[0]);
        return -1;
    }

    return 0;
}

static int sx1280_tx_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 2) {
        printf("Usage: %s tx <payload>\n", argv[0]);
        return -1;
    }

    printf("sending \"%s\" payload (%" PRIuSIZE " bytes)\n",
           argv[2], strlen(argv[2]) + 1);
    iolist_t iolist = {
        .iol_base = argv[2],
        .iol_len = (strlen(argv[2]) + 1)
    };

    if (netdev->driver->send(netdev, &iolist) == -ENOTSUP) {
        puts("Cannot send: radio is still transmitting");
        return -1;
    }

    return 0;
}

static int sx1280_reset_cmd(netdev_t *netdev, int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("resetting sx1280...");
    netopt_state_t state = NETOPT_STATE_RESET;

    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    return 0;

}
int sx1280_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <get|set|rx|tx|reset>\n", argv[0]);
        return -1;
    }

    netdev_t *netdev = &sx1280.netdev;

    if (!strcmp("get", argv[1])) {
        return sx1280_get_cmd(netdev, argc, argv);
    }
    else if (!strcmp("set", argv[1])) {
        return sx1280_set_cmd(netdev, argc, argv);
    }
    else if (!strcmp("rx", argv[1])) {
        return sx1280_rx_cmd(netdev, argc, argv);
    }
    else if (!strcmp("tx", argv[1])) {
        return sx1280_tx_cmd(netdev, argc, argv);
    }
    else if (!strcmp("reset", argv[1])) {
        return sx1280_reset_cmd(netdev, argc, argv);
    }
    else {
        printf("Unknown cmd %s\n", argv[1]);
        printf("Usage: %s <get|set|rx|tx|reset>\n", argv[0]);
        return -1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "sx1280", "Control the SX1280 radio",     sx1280_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    sx1280_setup(&sx1280, &sx1280_params[0], 0);

    netdev_t *netdev = &sx1280.netdev;

    netdev->driver = &sx1280_driver;

    netdev->event_callback = _event_cb;

    if (netdev->driver->init(netdev) < 0) {
        puts("Failed to initialize SX1280 device, exiting");
        return 1;
    }

    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              0, _recv_thread, netdev,
                              "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        puts("Creation of receiver thread failed");
        return 1;
    }

    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
