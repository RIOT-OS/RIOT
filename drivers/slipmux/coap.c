/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 */
#include "checksum/crc16_ccitt.h"
#include "net/nanocoap.h"
#include "slipdev.h"
#include "slipdev_internal.h"

/* The special init is the result of normal fcs init combined with slipmux config start (0xa9) */
#define SPECIAL_INIT_FCS (0x374cU)
#define COAP_STACKSIZE (1024)

static char coap_stack[COAP_STACKSIZE];

static inline void _slipdev_lock(void)
{
    mutex_lock(&slipdev_mutex);
}

static inline void _slipdev_unlock(void)
{
    mutex_unlock(&slipdev_mutex);
}

void *_coap_server_thread(void *arg)
{
    static uint8_t buf[512];
    slipdev_t *dev = arg;

    while (1) {
        thread_flags_wait_any(1);
        size_t len;
        while (crb_get_chunk_size(&dev->rb_config, &len)) {
            if (len > sizeof(buf)) {
                crb_consume_chunk(&dev->rb_config, NULL, len);
                continue;
            }
            crb_consume_chunk(&dev->rb_config, buf, len);

            /* Is the crc correct via residue(=0xF0B8) test */
            if (crc16_ccitt_fcs_update(SPECIAL_INIT_FCS, buf, len) != 0xF0B8) {
                break;
            }

            /* cut off the FCS checksum at the end */
            size_t pktlen = len - 2;

            coap_pkt_t pkt;
            sock_udp_ep_t remote;
            coap_request_ctx_t ctx = {
                .remote = &remote,
            };
            if (coap_parse(&pkt, buf, pktlen) < 0) {
                break;
            }
            unsigned int res = 0;
            if ((res = coap_handle_req(&pkt, buf, sizeof(buf), &ctx)) <= 0) {
                break;
            }

            uint16_t fcs_sum = crc16_ccitt_fcs_finish(SPECIAL_INIT_FCS, buf, res);

            _slipdev_lock();
            slipdev_write_byte(dev->config.uart, SLIPDEV_START_COAP);
            slipdev_write_bytes(dev->config.uart, buf, res);
            slipdev_write_bytes(dev->config.uart, (uint8_t *)&fcs_sum, 2);
            slipdev_write_byte(dev->config.uart, SLIPDEV_END);
            _slipdev_unlock();
        }
    }

    return NULL;
}

void slipdev_setup_coap(slipdev_t *dev) {
    crb_init(&dev->rb_config, dev->rxmem_config, sizeof(dev->rxmem_config));

    dev->coap_server_pid = thread_create(coap_stack, sizeof(coap_stack), THREAD_PRIORITY_MAIN - 1,
                                         THREAD_CREATE_STACKTEST, _coap_server_thread,
                                         (void *)dev, "Slipmux CoAP server");
}

/** @} */
