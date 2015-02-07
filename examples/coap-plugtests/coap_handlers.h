/*
 * Copyright (C) 2014 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef COAP_HANDLERS_H
#define COAP_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tcb.h"
#include "coap.h"

void index_handler(coap_context_t  *ctx, struct coap_resource_t *resource,
                   const coap_endpoint_t *local_interface,
                   coap_address_t *peer, coap_pdu_t *request, str *token,
                   coap_pdu_t *response);

void threads_handler(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_01(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_02(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_03(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_04(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_09(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_19(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_20(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_21(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_core_23(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_link_09(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response);

void td_coap_block_01(coap_context_t  *ctx, struct coap_resource_t *resource,
                      const coap_endpoint_t *local_interface,
                      coap_address_t *peer, coap_pdu_t *request, str *token,
                      coap_pdu_t *response);



void init_local_data(void);

#ifdef __cplusplus
}
#endif

#endif /* COAP_HANDLERS_H */
