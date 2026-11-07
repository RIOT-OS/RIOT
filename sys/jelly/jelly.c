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

#include "net/unicoap.h"

static int _riot_version_handler(
    unicoap_message_t *message, const unicoap_aux_t *aux,
    unicoap_request_context_t *ctx, void *arg)
{
    (void)aux;
    (void)arg;
    unicoap_response_init_string(message, UNICOAP_STATUS_CONTENT, RIOT_VERSION);
    return unicoap_send_response(message, ctx);
}

static int _riot_board_handler(
    unicoap_message_t *message, const unicoap_aux_t *aux,
    unicoap_request_context_t *ctx, void *arg)
{
    (void)aux;
    (void)arg;
    unicoap_response_init_string(message, UNICOAP_STATUS_CONTENT, RIOT_BOARD);
    return unicoap_send_response(message, ctx);
}

UNICOAP_RESOURCE(riot_version)
{
    .path = UNICOAP_PATH("jelly", "ver"),
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET),
    .handler = _riot_version_handler,
    .protocols = UNICOAP_PROTOCOLS(UNICOAP_PROTO_SLIPMUX),
};

UNICOAP_RESOURCE(riot_board)
{
    .path = UNICOAP_PATH("jelly", "board"),
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET),
    .handler = _riot_board_handler,
    .protocols = UNICOAP_PROTOCOLS(UNICOAP_PROTO_SLIPMUX),
};

/** @} */
