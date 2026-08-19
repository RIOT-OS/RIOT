/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_server
 * @brief   Server automatic block-wise transfers
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <string.h>
#include <errno.h>

#include "ztimer.h"
#include "mutex.h"
#include "compiler_hints.h"

#if IS_USED(MODULE_DNS)
#  include "net/dns.h"
#endif

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

/* MARK: - Internals */

int unicoap_server_prepare_response_blockwise(unicoap_packet_t* packet, 
                                              unicoap_server_memo_t* memo,
                                              unicoap_blockwise_flags_t blockwise_flags) {
    unicoap_blockwise_transfer_t* transfer = unicoap_memo_blockwise_transfer_get(&memo->super);
    int res = 0;

    unicoap_block_option_t block1 = UNICOAP_BLOCK_OPTION_NONE;
    if (transfer) {
        /* Exists, so check for Block1 option from ended Block1 transfer / collect stage. */
        unicoap_options_get_block1(&transfer->options, &block1);
        if ((res = unicoap_blockwise_transfer_setup(packet->message, transfer, blockwise_flags,
                                                    packet->remote->proto, false)) < 0) {
            return res;
        }
    }
    else {
        if (!(transfer = unicoap_blockwise_transfer_create(&memo->super, blockwise_flags))) {
            return -ENOBUFS;
        }
        transfer->stage = UNICOAP_BLOCKWISE_STAGE_SLICE;
        if ((res = unicoap_blockwise_transfer_setup(packet->message, transfer, blockwise_flags,
                                                    packet->remote->proto, true)) < 0) {
            return res;
        }
    }

    if ((res = unicoap_blockwise_slice_block2_produce(&transfer->iterator,
                                                      &packet->message->payload)) < 0) {
        return res;
    }

    packet->message->payload_size = res;

    if (block1 != UNICOAP_BLOCK_OPTION_NONE) {
        if ((res = unicoap_options_set_block1(packet->message->options, block1)) < 0) {
            return res;
        }
    }
    if ((res = unicoap_options_set_block2(packet->message->options,
        unicoap_blockwise_iterator_current_option(&transfer->iterator))) < 0) {
        return res;
    }

    /* Size2 is optional anyways, ignore errors (no buffer space) */
    unicoap_options_set_size2(packet->message->options,
                              (uint32_t)transfer->iterator.body_size);
    return 0;
}

int unicoap_server_process_request_blockwise(unicoap_packet_t* packet, unicoap_server_memo_t* memo,
                                             const unicoap_resource_t* resource) {
    unicoap_blockwise_transfer_t* transfer = unicoap_memo_blockwise_transfer_get(&memo->super);
    unicoap_message_t* message = packet->message;
    ssize_t res = 0;
    unicoap_block_option_t block = UNICOAP_BLOCK_OPTION_NONE;

    switch (transfer->stage) {
    case UNICOAP_BLOCKWISE_STAGE_COLLECT: {
        _SERVER_BLOCKWISE_DEBUG("received request in COLLECT stage\n");

        res = unicoap_options_get_block1(message->options, &block);
        if (res < 0 && res != -ENOENT) {
            return res;
        }

        int collect_res = 0;
        if ((collect_res = unicoap_blockwise_collect_block1(
                 &transfer->iterator, block, message->payload, message->payload_size)) < 0) {
            return collect_res;
        }
        if ((res = unicoap_blockwise_transfer_collector_validate(message, transfer)) < 0) {
            return res;
        }
        if (collect_res > 0) {
            _SERVER_BLOCKWISE_DEBUG("reassembled block-wise, asking for more.\n");
            /* send the same request again with a Block1 option */
            unicoap_response_init_with_options(message, UNICOAP_STATUS_CONTINUE, NULL, 0,
                                               &transfer->options);

            if ((res = unicoap_options_set_block1(message->options,
                unicoap_blockwise_iterator_current_option(&transfer->iterator))) < 0) {
                return res;
            }

            if ((res = unicoap_server_send_response_part(packet, memo, resource->flags)) < 0) {
                return res;
            }

            return 1; /* > 0 ... not done yet */
        }

        message->payload = transfer->iterator.body;
        message->payload_size = transfer->iterator.body_size;
        _SERVER_BLOCKWISE_DEBUG("reassembled block-wise (total=%" PRIuSIZE "B), done.\n",
                                message->payload_size);

        /* check if we're about to slice the response */
        if (resource->flags & UNICOAP_RESOURCE_FLAG_SLICE) {
            transfer->stage = UNICOAP_BLOCKWISE_STAGE_SLICE;
            unicoap_blockwise_iterator_init(&transfer->iterator, CONFIG_UNICOAP_BLOCK_SZX, NULL, 0);

            if (block) {
                /* Remember Block1 option to set in last Block1 response = first Block2 response */
                unicoap_options_set_block1(&transfer->options, block);
            }

            block = UNICOAP_BLOCK_OPTION_NONE;
            res = unicoap_options_get_block2(message->options, &block);
            if (res < 0 && res != -ENOENT) {
                return res;
            }

            if ((res = unicoap_blockwise_slice_block2_process(&transfer->iterator, block)) < 0) {
                return res;
            }
        }
        else {
            /* We could keep the block-wise transfer until the exchange ends,
             * but that would mean unnecessarily keeping the transfer. */
            _SERVER_BLOCKWISE_DEBUG("block-wise transfer ended, freeing transfer\n");
            unicoap_blockwise_transfer_free(&memo->super);
        }

        /* let the caller invoke the resource handler */
        return 0; /* 0 ... done */
    }

    case UNICOAP_BLOCKWISE_STAGE_SLICE: {
        _SERVER_BLOCKWISE_DEBUG("received request in SLICE stage\n");

        uint8_t* chunk = NULL;
        block = UNICOAP_BLOCK_OPTION_NONE;
        res = unicoap_options_get_block2(message->options, &block);
        if (res < 0 && res != -ENOENT) {
            return res;
        }

        if ((res = unicoap_blockwise_slice_block2(&transfer->iterator, block, &chunk)) < 0) {
            return res;
        }

        if (res > 0) {
            _SERVER_BLOCKWISE_DEBUG("slicing block-wise, sending more.\n");
            // send block-wise response
            message->code = transfer->code;
            message->options = &transfer->options;
            message->payload = chunk;
            message->payload_size = res;

            if ((res = unicoap_options_set_block2(message->options,
                unicoap_blockwise_iterator_current_option(&transfer->iterator))) < 0) {
                return res;
            }

            if ((res = unicoap_server_send_response_part(packet, memo, resource->flags)) <
                0) {
                return res;
            }
            return 1; /* > 0 ... not done yet */
        }

        _SERVER_BLOCKWISE_DEBUG("sliced block-wise, done.\n");
        return 0; /* 0 ... done */
    }

    default:
        UNREACHABLE();
        return -1;
    }
    return 0;
}
