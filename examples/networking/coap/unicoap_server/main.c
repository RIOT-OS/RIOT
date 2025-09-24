/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup examples
 * @brief   Sample `unicoap` server application
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include "net/unicoap.h"

/* This is needed for netifs_print_ipv6 in main below. */
#include "net/netif.h"

/* If you need CoAP over DTLS support, you need to include extra dependencies. What's more,
 * you'll also need to load a DTLS credential for message encryption and verification.
 * The `IS_USED(MODULE_UNICOAP_DRIVER_DTLS)` below checks if the @ref net_unicoap_drivers_dtls
 * has been imported via the USEMODULE variable in the application Makefile. */
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
# include "net/sock/dtls/creds.h"
# include "net/credman.h"
# include "net/dsm.h"
# include "unicoap_example_dtls.h"

/* Example credential tag for credman. Tag together with the credential type needs to be unique. */
# define EXAMPLE_DTLS_CREDENTIAL_TAG 42 /* This should answer your question. */

static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = EXAMPLE_DTLS_CREDENTIAL_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif /* IS_USED(MODULE_UNICOAP_DRIVER_DTLS) */

static int handle_hello_request(unicoap_message_t* message, const unicoap_aux_t* aux,
                                unicoap_request_context_t* ctx, void* arg) {
    (void)aux;
    (void)arg;

    printf("app: %s /, %" PRIuSIZE " bytes\n",
           unicoap_string_from_method(message->method),
           unicoap_message_payload_get_size(message));

    /* String literals in C are null-terminated. For null-terminated string, we can use the
     * _string message initializers. */
    unicoap_response_init_string(message, UNICOAP_STATUS_CONTENT, "Hello, World!");

    /* Respond. Note that you do not need to return the result of @ref unicoap_send_response. */
    return unicoap_send_response(message, ctx);
}

/*
 * Let's define a CoAP resource. To define a new resource statically (at compile-time), you
 * use the @ref UNICOAP_RESOURCE macro. Note that you will need to supply an identifier. Let's
 * call it `hello`. The identifier is required by the [implementation](@ref UNICOAP_RESOURCE),
 * but not used otherwise. You can give it any name you want, as long as it is a unique C
 * variable name.
 */

UNICOAP_RESOURCE(hello) {
    /* Each resource must be assigned a path. This will be the part that follows the host/domain
     * and port in the CoAP URI. */
    .path = "/",

    /* Since we are using CoAP over UDP (or CoAP over DTLS, which relies on UDP for that matter),
     * we instruct `unicoap` to send confirmable responses. To do this, we pass the
     * @ref UNICOAP_RESOURCE_FLAG_RELIABLE flag. Don't want to loose our greeting along the way. */
    .flags = UNICOAP_RESOURCE_FLAG_RELIABLE,

    /* You must declare what CoAP methods you want to allow for each resource. */
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_PUT, UNICOAP_METHOD_POST),

    /* Optionally, you can also restrict the resource to a set of transports. */
    .protocols = UNICOAP_PROTOCOLS(UNICOAP_PROTO_DTLS, UNICOAP_PROTO_UDP),

    /* Finally, pass the handler and an optional argument. */
    .handler = handle_hello_request,
    .handler_arg = NULL
};

/* Next, we create a more complex resource. */

static bool is_valid_name(const char* string, size_t length) {
    for (size_t i = 0; i < length; i += 1) {
        if (string[i] == 0) {
            return false;
        }
    }
    return true;
}

static int handle_greeting_request(unicoap_message_t* message, const unicoap_aux_t* aux,
                                   unicoap_request_context_t* ctx, void* arg) {
    (void)aux;
    (void)arg;

    printf("app: %s /greeting, %" PRIuSIZE " bytes\n",
           unicoap_string_from_method(message->method),
           unicoap_message_payload_get_size(message));

    /* Our /greeting resource accepts a 'name' query parameter.
     * If you GET /greeting?name=RIOTeer, we want to respond with "Hello, RIOTeer!".
     * Besides, this URI is transmitted as two CoAP options:
     * - `Uri-Path`: "greeting"
     * - `Uri-Query`: "name=RIOTeer"
     */

    ssize_t res = 0;
    const char* name = NULL;

    if ((res = unicoap_options_get_first_uri_query_by_name(message->options, "name", &name)) < 0) {
        printf("error: could not get 'name' query: %" PRIdSIZE " (%s)\n", res, strerror(-res));
        /* Returning a status code here will result in a response being sent automatically. */
        return UNICOAP_STATUS_BAD_REQUEST;
    }

    /* Validate any input. Here, we apply a 30 UTF-8 character limit. */
    if (res > 30 || !is_valid_name(name, res)) {
        unicoap_response_init_string(message, UNICOAP_STATUS_BAD_REQUEST, "invalid 'name' query");
        return unicoap_send_response(message, ctx);
    }

    /* Now we can craft out response. Let's start with the part that can fail. */

    /* Allocate options on the stack. Provide the size of the buffer that will contain the
     * response options. */
    UNICOAP_OPTIONS_ALLOC(options, 2);

    /* Set Content-Format option to text/plain */
    if (unicoap_options_set_content_format(&options, UNICOAP_FORMAT_TEXT) < 0) {
        return UNICOAP_STATUS_INTERNAL_SERVER_ERROR;
    }

    message->options = &options;

    /* Now, we create our payload. In theory, we could write "Hello, ", then the name, and then
     * "! ..." into that buffer. Instead, we can use a vectored payload. */

#define static_strlen(string) sizeof(string) - 1

    /* These are going to be the first and last payload chunks. */
#define PREFIX "Hello, "
    iolist_t list = {
        .iol_base = PREFIX,
        .iol_len = static_strlen(PREFIX),
    };

#define SUFFIX "! Welcome to our itsy bitsy tiny CoAP server!"
    iolist_t suffix = {
        .iol_base = SUFFIX,
        .iol_len = static_strlen(SUFFIX)
    };

    iolist_t name_chunk = {
        .iol_next = &suffix,
        .iol_base = (void*)name,
        .iol_len = res
    };
    list.iol_next = &name_chunk;

    unicoap_message_payload_set_chunks(message, &list);
    unicoap_response_set_status(message, UNICOAP_STATUS_CONTENT);

    /* Respond. Note that you do not need to return the result of @ref unicoap_send_response. */
    return unicoap_send_response(message, ctx);
}

UNICOAP_RESOURCE(greeting) {
    .path = "/greeting",

    .flags = UNICOAP_RESOURCE_FLAG_RELIABLE,

    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET),

    .handler = handle_greeting_request,
    .handler_arg = NULL
};

int main(void) {
#  if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
    /* You can access the underlying transport handle. In the case of UDP,
     * this will be a UDP sock provided by the sock API. */
    sock_udp_t* udp_socket = unicoap_transport_udp_get_socket();
    assert(udp_socket);

    /* An endpoint abstracts the transport handle. */
    unicoap_endpoint_t udp_local = {
        .proto = UNICOAP_PROTO_UDP,
    };

    /* We can set the endpoint's UDP handle to the actual one. */
    sock_udp_get_local(udp_socket, &udp_local.udp_ep);

    printf("app: listening at ");
    /* Because we're in posession of an endpoint now, we can print
     * a debug description from it. */
    unicoap_print_endpoint(&udp_local);
    printf("\n");
#  endif

    /* If DTLS is enabled, the CoAP over DTLS driver needs to be told
     * at least one DTLS credential. */
#  if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    /* credman is a utility that manages DTLS credentials. */
    int res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* Here we ignore duplicate credentials. */
        printf("app: cannot add credential to system: %d\n", res);
        return 1;
    }

    /* Just like with UDP, we can access the DTLS transport handle. */
    sock_dtls_t* dtls_socket = unicoap_transport_dtls_get_socket();
    assert(dtls_socket);

    unicoap_endpoint_t dtls_local = {
        .proto = UNICOAP_PROTO_DTLS,
    };

    sock_udp_get_local(sock_dtls_get_udp_sock(dtls_socket), &dtls_local.dtls_ep);

    printf("app: listening at ");
    unicoap_print_endpoint(&dtls_local);
    printf("\n");

    /* Here we tell the DTLS socket to use the credential known by the specified
     * tag. Internally, the socket will retrieve the corresponding credential through
     * the credential manager. */
    if ((res = sock_dtls_add_credential(dtls_socket, EXAMPLE_DTLS_CREDENTIAL_TAG)) < 0) {
        printf("app: cannot add credential to DTLS sock: %d\n", res);
        return 1;
    }

    printf("app: using credential: "
           "type=PSK id=%s key=%s\n",
           (char*)credential.params.psk.id.s,
           (char*)credential.params.psk.key.s);
#  endif

    printf("app: IPv6 address: ");
    netifs_print_ipv6(", ");
    printf("\n");
}
