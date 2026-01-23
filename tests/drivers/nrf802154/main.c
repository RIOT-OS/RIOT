/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for NRF802154 IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "nrf802154.h"
#include "init_dev.h"
#include "net/netdev/ieee802154_submac.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"

static netdev_ieee802154_submac_t nrf802154;

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    netdev_t *netdev = &nrf802154.dev.netdev;

    puts("Initializing NRF802154 device");

    netdev_register(netdev, NETDEV_CC2538, 0);
    netdev_ieee802154_submac_init(&nrf802154);

    /* set the application-provided callback */
    netdev->event_callback = cb;

    /* setup and initialize the specific driver */
    nrf802154_hal_setup(&nrf802154.submac.dev);
    nrf802154_init();

    /* initialize the device driver */
    int res = netdev->driver->init(netdev);
    if (res != 0) {
        return -1;
    }

    return 0;
}

void send_beacon_usage(char *cmd_name)
{
    printf("usage: %s [<16 bit hex source PAN ID>] (uses device PAN ID by default)\n", cmd_name);
}

int cmd_send_beacon(int argc, char **argv)
{
    uint16_t pan_id;
    /* evaluating if argument is present */
    switch (argc) {
    case 1:
        pan_id = nrf802154.dev.pan;
        puts("using device PAN ID as source");
        break;
    case 2:
        if (strlen(argv[1]) != 4) {
            puts("send: Error parsing PAN ID");
            send_beacon_usage(argv[0]);
            return 1;
        }
        pan_id = strtoul(argv[1], NULL, 16);
        break;
    default:
        send_beacon_usage(argv[0]);
        return 1;
    }

    printf("PAN ID: %x\n", pan_id);

    /* preparing the mac header */
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    memset(mhr, 0, IEEE802154_MAX_HDR_LEN*sizeof(uint8_t));
    le_uint16_t src_pan = byteorder_btols(byteorder_htons(pan_id));
    le_uint16_t dst_pan = byteorder_htols(0);
    size_t src_len = 2, dst_len = 0;
    uint8_t *src = nrf802154.dev.short_addr, *dst = NULL;
    uint8_t flags = IEEE802154_FCF_TYPE_BEACON;

    int res = ieee802154_set_frame_hdr(mhr, src, src_len,
                                        dst, dst_len,
                                        src_pan, dst_pan,
                                        flags, nrf802154.dev.seq++);
    if (res < 0) {
        puts("send: Error preparing frame");
        send_beacon_usage(argv[0]);
        return 1;
    }
    /* preparing packet to send */
    iolist_t iol = {
        .iol_base = mhr,
        .iol_len = (size_t)res,
        .iol_next = NULL,
    };

    puts("Sending Beacon Frame");

    res = netdev_ieee802154_minimal_send(&nrf802154.dev.netdev, &iol);

    if (res < 0) {
        puts("send: Error on sending");
        send_beacon_usage(argv[0]);
        return 1;
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "beaconsend", "Send an IEEE 802.15.4 beacon frame", cmd_send_beacon},
    {NULL, NULL, NULL}
};

int main(void)
{
    puts("Test application for NRF802154 IEEE 802.15.4 device driver");

    int res = netdev_ieee802154_minimal_init();
    if (res) {
        puts("Error initializing devices");
        return 1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
