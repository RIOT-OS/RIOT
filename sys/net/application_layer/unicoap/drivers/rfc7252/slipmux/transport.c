/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-FileCopyrightText: 2024-2026 Bennet Hattesen
 * SPDX-FileCopyrightText: 2024-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_drivers_slipmux
 * @brief   Transport implementation of CoAP over Slipmux driver
 * @author  Bennet Hattesen
 */

#include <stdint.h>
#include <errno.h>
#include "architecture.h"
#include "net/unicoap/transport.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

/* Provides `unicoap_receiver_buffer` */
UNICOAP_DECL_RECEIVER_STORAGE_EXTERN;

extern int unicoap_messaging_process_rfc7252(const uint8_t *pdu, size_t size, bool truncated,
                                             unicoap_packet_t *packet);

void unicoap_slipdev_recv_handler(event_t *event)
{
    slipdev_t *dev = container_of(event, slipdev_t, rxevent);

    unicoap_endpoint_t remote = {
        .proto = UNICOAP_PROTO_SLIPMUX,
        .slipmux_ep = dev,
    };

    unicoap_packet_t packet = { .remote = &remote };

    while (1) {
        ssize_t len = slipdev_coap_recv(unicoap_receiver_buffer, sizeof(unicoap_receiver_buffer),
                                        dev);
        if (len <= 0) {
            if (len == -2) {
                DEBUG("Slipmux: received configuration frame with bad checksum on UART(%d)\n",
                      dev->config.uart);
            }
            if (len == -1) {
                DEBUG(
                    "Slipmux: received configuration frame that is too long or too short on UART(%d)\n",
                    dev->config.uart);
            }
            return;
        }
        unicoap_messaging_process_rfc7252(unicoap_receiver_buffer, len, false, &packet);
    }
}

int unicoap_transport_sendv_slipmux(iolist_t *iolist, const unicoap_endpoint_t *remote)
{
    DEBUG("Sending slipmux coap to UART(%d)\n", remote->slipmux_ep->config.uart);
    size_t len = iolist_to_buffer(iolist, unicoap_receiver_buffer, sizeof(unicoap_receiver_buffer));
    slipdev_coap_send(unicoap_receiver_buffer, len, remote->slipmux_ep);

    return 0;
}

int unicoap_init_slipmux(event_queue_t *queue)
{
    slipdev_coap_set_event_queue(queue);
    return 0;
}

int unicoap_deinit_slipmux(event_queue_t *queue)
{
    (void)queue;
    slipdev_coap_unset_event_queue();
    return 0;
}
