/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_server
 * @brief   Server implementation
 * @author  Carl <carl.seifert@tu-dresden.de>
 */

#include <string.h>

#include "thread.h"
#include "compiler_hints.h"
#include "modules.h"

#include "net/unicoap/client.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

static int _cancel(unicoap_client_memo_t* memo) {
    int res = 0;
    unicoap_message_t request = {
        .method = UNICOAP_METHOD_GET,
        .options = unicoap_client_memo_options(memo)
    };
    if ((res = unicoap_options_set_observe(request.options, UNICOAP_OBSERVE_OPTION_DEREGISTER)) < 0) {
        _CLIENT_DEBUG("could not set Observe option\n");
        goto error;
    }
    unicoap_packet_t packet = { .remote = &memo->super.endpoint,
                                .message = &request,
                                .properties = {
                                    .token = memo->token,
                                    .token_length = sizeof(memo->token),
                                } };
    /* TODO: OSCORE */
    /* Make sure we release memo when we get a response. Therefore, we remove OBSERVE flag. */
    memo->flags &= ~UNICOAP_CLIENT_FLAG_OBSERVE;
    if ((res = unicoap_client_send_request_part(&packet, memo, memo->flags)) < 0) {
        goto error;
    }
    return 0;

error:
    unicoap_client_memo_free(memo);
    return res;
}

int unicoap_client_cancel_observation(int refno) {
    if (IS_ACTIVE(CONFIG_UNICOAP_CLIENT_CANCELLABLE)) {
        if (refno <= 0) {
            return -EINVAL;
        }
        unicoap_client_memo_t* memo = unicoap_client_memo_find_refno(refno);
        if (!memo) {
            _CLIENT_DEBUG("no request with refno %i\n", refno);
            return -ENOENT;
        }
        if (!(memo->flags & UNICOAP_CLIENT_FLAG_OBSERVE)) {
            if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
                unicoap_assist(API_MISUSE("observation not enabled for request")
                                   FIXIT("use unicoap_client_cancel instead"));
            }
            return -EINVAL;
        }
        return _cancel(memo);
    } else {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
            unicoap_assist(API_MISUSE("request cancellation not supported")
                               FIXIT("enable CONFIG_UNICOAP_CLIENT_CANCELLABLE"));
        }
        return -ENOTSUP;
    }
}

int unicoap_client_cancel_observation_token(const unicoap_endpoint_t* endpoint,
                                            const uint8_t* token, size_t token_length) {
    unicoap_client_memo_t* memo = unicoap_client_memo_find_token(endpoint, token, token_length);
    if (!memo) {
        return -ENOENT;
    }
    return _cancel(memo);
}
