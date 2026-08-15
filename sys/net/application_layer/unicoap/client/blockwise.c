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
