/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_client
 * @brief   Client automatic block-wise transfers
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

/* MARK: - Public client API */

int unicoap_send_request_blockwise_async(unicoap_message_t* request,
                                         unicoap_destination_t* destination,
                                         unicoap_blockwise_callback_t callback, 
                                         unicoap_request_parameters_t* parameters,
                                         unicoap_request_flags_t flags) {
    assert(callback);
    return unicoap_open_request(request, destination, 
                                (unicoap_callback_t) { .block = callback }, 
                                parameters,
                                flags | (uint)UNICOAP_CLIENT_FLAG_BLOCK_CALLBACK);
}


typedef struct {
    unicoap_blockwise_callback_t callback;
    void* caller_arg;
    mutex_t roadblock;
    int res;
} _blockwise_sync_args_t;

static int _sync_blockwise_callback(const unicoap_message_t* response, 
                                    const unicoap_aux_t *aux,
                                    int error, 
                                    const unicoap_block_info_t* block, 
                                    void* args
) {
    _blockwise_sync_args_t* a = (_blockwise_sync_args_t*)args;
    a->res = a->callback(response, aux, error, block, a->caller_arg);

    if (a->res < 0 || block == NULL || !block->more_available) {
        mutex_unlock(&a->roadblock);
    }
    return 0;
}

int unicoap_send_request_blockwise_sync(unicoap_message_t* request,
                                        unicoap_destination_t* destination,
                                        unicoap_blockwise_callback_t callback, 
                                        unicoap_request_parameters_t* parameters,
                                        unicoap_request_flags_t flags)
{
    assert(callback);
    _blockwise_sync_args_t args = (_blockwise_sync_args_t){ 
        .roadblock = MUTEX_INIT_LOCKED,
        .callback = callback
    };

    unicoap_request_parameters_t blockwise_sync_parameters = {};
    if (parameters) {
        args.caller_arg = parameters->callback_arg;
        blockwise_sync_parameters = *parameters;
    }
    blockwise_sync_parameters.callback_arg = &args;

    /* if auto-slice flag is present, prevent copies. request will stay valid, as we we lock thread */
    int res = unicoap_open_request(request, destination, 
                                   (unicoap_callback_t) { .block = _sync_blockwise_callback },
                                   &blockwise_sync_parameters, flags | 
                                   UNICOAP_CLIENT_FLAG_BLOCK_CALLBACK | 
                                   UNICOAP_CLIENT_FLAG_DURABLE_MESSAGE);
    if (res < 0) {
        return res;
    }
    /* Block until callback calls unlock. */
    mutex_lock(&args.roadblock);
    return args.res;
}

/* MARK: - Internals */

int unicoap_client_prepare_request_blockwise(const unicoap_packet_t* packet,
                                             unicoap_blockwise_transfer_t* transfer,
                                             unicoap_request_flags_t client_flags) {
    int res = 0;
    unicoap_message_t* request = packet->message;

    if ((res = unicoap_blockwise_transfer_setup(request, transfer, 
                                                _blockwise_flags_client(client_flags),
                                                packet->remote->proto, true)) < 0) {
        return res;
    }

    switch (transfer->stage) {
    case UNICOAP_BLOCKWISE_STAGE_SLICE:
        if ((res = unicoap_blockwise_slice_block1(&transfer->iterator, UNICOAP_BLOCK_OPTION_NONE,
                                                  &request->payload)) < 0) {
            return res;
        }
        request->payload_size = res;
        if ((res = unicoap_options_set_block1(request->options, 
            unicoap_blockwise_iterator_current_option(&transfer->iterator))) < 0) {
            return res;
        }

        /* Size1 is optional anyway, options API is atomic, so ignore errors.
         * If there's no space for Size1, then we'll silently accept our fate. */
        unicoap_options_set_size1(request->options,
                                  (uint32_t)transfer->iterator.body_size);
        break;

    case UNICOAP_BLOCKWISE_STAGE_COLLECT:
        if ((res = unicoap_blockwise_collect_block2(&transfer->iterator, UNICOAP_BLOCK_OPTION_NONE,
                                                    NULL, 0)) < 0) {
            return res;
        }

        if ((res = unicoap_options_set_block2(request->options, 
            unicoap_blockwise_iterator_current_option(&transfer->iterator)
        )) < 0) {
            return res;
        }
        break;

    default:
        UNREACHABLE();
        return -1;
    }
    return 0;
}

int unicoap_client_process_response_blockwise(unicoap_packet_t* packet, unicoap_client_memo_t* memo) {
    if (unicoap_code_class(packet->message->code) != UNICOAP_CODE_CLASS_RESPONSE_SUCCESS) {
        _CLIENT_BLOCKWISE_DEBUG("received error response, aborting block-wise transfer\n");

        unicoap_client_callback_success(memo, packet, UNICOAP_BLOCK_OPTION_NONE);
        return 0; /* 0 ... done, nothing we can do anymore */
    }

    unicoap_request_flags_t client_flags = memo->flags;

    switch (unicoap_memo_blockwise_transfer_get(&memo->super)->stage) {
    case UNICOAP_BLOCKWISE_STAGE_SLICE: {
        _CLIENT_BLOCKWISE_DEBUG("received response in SLICE transfer stage\n");

        unicoap_blockwise_transfer_t* transfer = unicoap_memo_blockwise_transfer_get(&memo->super);
        unicoap_message_t* message = packet->message;
        int res = 0;
        unicoap_block_option_t block = UNICOAP_BLOCK_OPTION_NONE;
        uint8_t* chunk = NULL;

        res = unicoap_options_get_block1(message->options, &block);
        if (res < 0 && res != -ENOENT) {
            return res;
        }

        if ((res = unicoap_blockwise_slice_block1(&transfer->iterator, block, &chunk)) < 0) {
            return res;
        }

        if (res > 0) {
            if (unicoap_options_contains(message->options, UNICOAP_OPTION_OBSERVE)) {
                _CLIENT_BLOCKWISE_DEBUG("server sent Observe option in Block1 stage, "
                                        "not interested\n");
            }

            _CLIENT_BLOCKWISE_DEBUG("slicing block-wise, sending more.\n");
            /* send another block-wise request with Block1 options */
            unicoap_request_init_with_options(message, transfer->code, chunk, res, 
                                              &transfer->options);

            if ((res = unicoap_options_set_block1(message->options,
                unicoap_blockwise_iterator_current_option(&transfer->iterator))) < 0) {
                return res;
            }

            if (!unicoap_block_get_more(
                unicoap_blockwise_iterator_current_option(&transfer->iterator))) {
                /* final Block1 request is first Block2 request */
                if (client_flags &
                    (UNICOAP_CLIENT_FLAG_REASSEMBLE | UNICOAP_CLIENT_FLAG_BLOCK_CALLBACK)) {
                    transfer->stage = UNICOAP_BLOCKWISE_STAGE_COLLECT;
                    if ((res = unicoap_client_prepare_request_blockwise(
                        packet, transfer, client_flags)) < 0) {
                        return res;
                    }
                }
            }

            if ((res = unicoap_client_send_request_part(packet, memo, client_flags)) < 0) {
                return res;
            }

            /* remove Block1 and Size1 option for potential Block2 transfer */
            unicoap_options_remove_block1(&transfer->options);
            unicoap_options_remove_size1(&transfer->options);
            return 1; /* > 0 ... not done yet */
        }
        else {
            _CLIENT_BLOCKWISE_DEBUG("sliced block-wise, done.\n");
            if ((res = unicoap_client_callback_success(memo, packet, block)) < 0) {
                return res;
            }
            return 0; /* 0 ... done */
        }
    }

    case UNICOAP_BLOCKWISE_STAGE_COLLECT: {
        _CLIENT_BLOCKWISE_DEBUG("received response in COLLECT transfer state\n");
        unicoap_blockwise_transfer_t* transfer = unicoap_memo_blockwise_transfer_get(&memo->super);
        unicoap_message_t* message = packet->message;
        int res = 0;
        unicoap_block_option_t block = UNICOAP_BLOCK_OPTION_NONE;

        res = unicoap_options_get_block2(message->options, &block);
        if (res < 0 && res != -ENOENT) {
            return res;
        }

        int collect_res = 0;
        if ((collect_res = unicoap_blockwise_collect_block2(
                 &transfer->iterator, block, message->payload, message->payload_size)) < 0) {
            return collect_res;
        }

        if ((res = unicoap_blockwise_transfer_collector_validate(message, transfer)) < 0) {
            return res;
        }

        if (collect_res > 0) {
            _CLIENT_BLOCKWISE_DEBUG("collecting block-wise (collected=%" PRIuSIZE "B), "
                "asking for more.\n", transfer->iterator.offset);
            /* send the same request again with a Block2 option */
            unicoap_request_init_with_options(message, transfer->code, NULL, 0, &transfer->options);

            if ((res = unicoap_options_set_block2(message->options,
                unicoap_blockwise_iterator_current_option(&transfer->iterator))) < 0) {
                return res;
            }

            if (client_flags & UNICOAP_CLIENT_FLAG_BLOCK_CALLBACK &&
                (res = unicoap_client_callback_success(memo, packet, block)) < 0) {
                _CLIENT_BLOCKWISE_DEBUG("block-wise callback returned error, cancelling\n");
                return res;
            }

            if ((res = unicoap_client_send_request_part(packet, memo, client_flags)) < 0) {
                return res;
            }
            return 1; /* > 0 ... not done yet */
        }

        _CLIENT_BLOCKWISE_DEBUG("collected block-wise (total=%" PRIuSIZE "B), done.\n",
                     transfer->iterator.offset);
        if (client_flags & UNICOAP_CLIENT_FLAG_REASSEMBLE) {
            message->payload = transfer->iterator.body;
            message->payload_size = transfer->iterator.body_size;
        }

        if ((res = unicoap_client_callback_success(memo, packet, block)) < 0) {
            return res;
        }
        return 0; /* 0 ... done */
    }

    default:
        UNREACHABLE();
        return -1;
    }
}
