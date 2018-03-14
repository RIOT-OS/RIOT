/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_firmware
 * @{
 *
 * @file
 * @brief       Firmware update cia CoAP implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "firmware_update.h"
#include "log.h"
#include "net/nanocoap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static firmware_update_t _state;

ssize_t ota_coap_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void)context;

    uint32_t result = COAP_CODE_204;
    int res;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);

    LOG_INFO("ota: received bytes %u-%u", \
             (unsigned)block1.offset, (unsigned)block1.offset + pkt->payload_len);

    if (_state.state == FIRMWARE_UPDATE_VERIFIED) {
        unsigned total = _state.metadata_buf.size + FIRMWARE_METADATA_SIZE;
        LOG_INFO(" of %u (left=%u)\n", total,
                 total - block1.offset - pkt->payload_len);
    }
    else {
        LOG_INFO("\n");
    }

    if (block1.offset == 0) {
        /* initialize firmware upgrade state struct */
        firmware_update_init(&_state, firmware_target_slot());
    }

    if (_state.state == FIRMWARE_UPDATE_IDLE) {
        res = -1;
    }
    else {
        if (block1.offset == _state.offset) {
            res = firmware_update_putbytes(&_state, block1.offset, pkt->payload, pkt->payload_len);
        }
        else {
            LOG_INFO("coap_ota_handler(): ignoring already received block\n");
            res = 0;
        }
    }

    if (res) {
        result = COAP_CODE_NOT_ACCEPTABLE;
        _state.state = FIRMWARE_UPDATE_IDLE;
        LOG_INFO("coap_ota_handler(): unexpected packet\n");
    }
    else if (block1.more == 1) {
        result = COAP_CODE_231;
    }

    if (!res && (!blockwise || !block1.more)) {
        if (firmware_update_finish(&_state) != 0) {
            result = COAP_CODE_BAD_OPTION;
        }
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    uint8_t *pkt_pos = (uint8_t *)pkt->hdr + reply_len;
    if (!res) {
        pkt_pos += coap_put_block1_ok(pkt_pos, &block1, 0);
    }

    return pkt_pos - (uint8_t *)pkt->hdr;
}
