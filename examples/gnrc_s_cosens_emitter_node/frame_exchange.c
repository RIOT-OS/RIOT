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
 * @brief       Demonstrating the sending of IEEE 802.15.4 frames using
 *              the S-CoSenS for leaf nodes MAC layer
 *              (i.e.: 'gnrc_s_cosens_node' module)
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "kernel.h"
#include "net/gnrc.h"
#include "net/gnrc/pktdump.h"
#include "timex.h"
#include "net/ieee802154.h"
#include "at86rf2xx.h"


#define NEW_PACKET_BUF_SIZE   (127)

extern at86rf2xx_t at86rf2xx_dev;


static gnrc_netreg_entry_t server = { /* set by gnrc_netreg_register() */
                                      NULL,
                                      /* there's no "speciality" like port
                                         number or whatever at the MAC level */
                                      GNRC_NETREG_DEMUX_CTX_ALL,
                                      /* set by start_server() function below */
                                      KERNEL_PID_UNDEF };


/**
 * @brief Command for sending IEEE 802.15.4 frames
 *        on radio medium using S-CoSenS-node MAC layer
 */
int s_cosens_send_cmd(int argc, char **argv)
{
    gnrc_netdev_t *dev = (gnrc_netdev_t *) &at86rf2xx_dev;
    /* check the number of arguments */
    if (argc < 3) {
        printf("usage: %s <addr> <data>\n", argv[0]);
        return 1;
    }

    /* get PAN and source address */
    uint16_t pan, src;
    int res = dev->driver->get(dev,
                               NETOPT_NID,
                               (void *) &pan,
                               sizeof(uint16_t));
    if (res <= 0) {
        puts("Error: Cannot get PAN ID from device");
        return 1;
    }
    res = dev->driver->get(dev,
                           NETOPT_ADDRESS,
                           (void *) &src,
                           sizeof(uint16_t));
    if (res <= 0) {
        puts("Error: Cannot get router address from device");
        return 1;
    }

    /* contents of the packet to send */
    uint8_t pkt_buf[NEW_PACKET_BUF_SIZE];

    /* destintion node address */
    uint16_t dest = (uint16_t) atoi(argv[1]);

    /* build the IEEE 802.15.4 header of the packet to send */
        /* build a standard kind of IEEE 802.15.4 data packet,
           without security, ACK request or "frame pending" signal,
           for intra-PAN communication */
    pkt_buf[0] = IEEE802154_FCF_TYPE_DATA |
                 IEEE802154_FCF_PAN_COMP;
        /* we use short (intra-PAN) addresses, and
           use the very first version of FCF */
    pkt_buf[1] = IEEE802154_FCF_DST_ADDR_SHORT |
                 IEEE802154_FCF_VERS_V0 |
                 IEEE802154_FCF_SRC_ADDR_SHORT;
        /* PAN ID */
    pkt_buf[2] = (pan & 0xff);
    pkt_buf[3] = (pan >> 8);
        /* Destination address ! broadcast */
    pkt_buf[4] = (dest & 0xff);
    pkt_buf[5] = (dest >> 8);
        /* Short source address, of this router */
    pkt_buf[6] = (src & 0xff);
    pkt_buf[7] = (src >> 8);

    int index = 8;
    char *data = argv[2];
    for (int n = 0; data[n] != 0; n++) {
        pkt_buf[index] = (uint8_t) (data[n]);
        index++;
    }

    /* allocate packet */
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL,
                                          pkt_buf,
                                          (size_t) index,
                                          GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return 1;
    }

    /* send the packet, via the 'gnrc_s_cosens_node' module */
    res = gnrc_netapi_dispatch_send(GNRC_NETTYPE_UNDEF,
                                    GNRC_NETREG_DEMUX_CTX_ALL,
                                    pkt);
    if (res <= 0) {
        puts("Error: Couldn't contact the packet sending module!");
        return 1;
    }

    /* if we arrive here, the packet has been sent */
    return 0;
}

/**
 * @brief Function for starting RX server for IEEE 802.15.4 frames
 *        (this server wil actualy be the gnrc_pktdump thread)
 */
void start_frame_rx_server(void)
{
    /* check if server is already running */
    if (server.pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running (thread #%" PRIkernel_pid ")\n",
               server.pid);
        return;
    }
    /* start server (which means registering pktdump for the chosen port) */
    server.pid = gnrc_pktdump_getpid();
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &server);
    printf("Success: started frame RX server (thread #%" PRIkernel_pid ")\n",
           server.pid);
}
