/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup examples
 * @brief   Sample `unicoap` client application
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "net/unicoap.h"
#include "shell.h"
#include "od.h"

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

static int _print_usage(char** argv) {
    printf("usage: %s <get|post|put|delete|fetch|path|ipatch> [-r] <URI> [<UTF-8 payload>]\n", argv[0]);
    printf("          cancel <refno>\n");
    printf("Options: (default: unreliable)\n");
    printf("    -r   send reliably (send CON instead of NON over RFC7252 over UDP/DTLS)\n");
    printf("    -t   print response as UTF-8 if Content-Format is absent\n");
    return 1;
}

#define _SHELL_FLAG_TEXT (1)

static int _on_response(
    const unicoap_message_t* response,
    const unicoap_aux_t* aux, int error, void* arg
) {
    (void)aux;
    (void)arg;
    if (error) {
        printf("request failed (%i, %s)\n", error, strerror(-error));
        return error;
    }

    char status[4] = {0};
    unicoap_print_code(response->code, status);
    printf("response %s (%" PRIuSIZE " bytes)\n", status, response->payload_size);
    unicoap_content_format_t format;
    if (((error = unicoap_options_get_content_format(response->options, &format)) >= 0
         && format == UNICOAP_FORMAT_TEXT)
        || (error == -ENOENT && (((uint8_t)(uintptr_t)arg) & _SHELL_FLAG_TEXT))) {
        printf("text response: '%.*s'\n", (int)response->payload_size, (char*)response->payload);
    } else if (response->payload_size > 0) {
        od_hex_dump(response->payload, response->payload_size, 16);
    }
    return 0;
}

static int _set_flags(char* opt, unicoap_client_flags_t* flags, uint8_t* shell_flags) {
    if (strcmp(opt, "-r") == 0) {
        *flags |= UNICOAP_CLIENT_FLAG_RELIABLE;
    } else if (strcmp(opt, "-t") == 0) {
        *shell_flags |= _SHELL_FLAG_TEXT;
    } else {
        printf("error: invalid option '%s'\n", opt);
        return -1;
    }
    return 0;
}

static int _cli(int argc, char** argv) {
    /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
    char* methods[] = {
        "get", "post", "put", "delete", "fetch", "patch", "ipatch"
    };

    unicoap_client_flags_t flags = 0;
    uint8_t shell_flags = 0;
    int res = 0;

    if (argc == 1) {
        printf("error: missing request method\n");
        goto help;
    }

    if (strcmp(argv[1], "cancel") == 0) {
        if (argc != 3) {
            printf("error: invalid arguments\n");
            goto help;
        }
        if ((res = unicoap_client_cancel(atoi(argv[2]))) < 0) {
            printf("cancelling request failed (%i, %s)\n", res, strerror(-res));
        }
        return res;
    }

    int method = -1;
    for (size_t i = 0; i < ARRAY_SIZE(methods) && method == -1; i++) {
        if (strcmp(argv[1], methods[i]) == 0) {
            method = ((int)i + 1);
        }
    }

    if (method == -1) {
        printf("error: invalid request method\n");
        goto help;
    }

    assert(method != 0);

    /* parse options */
    int k = 2;
    while (k < argc && strncmp(argv[k], "-", 1) == 0) {
        if (_set_flags(argv[k], &flags, &shell_flags) < 0) {
            goto help;
        }
        k += 1;
    }

    if (k < argc) {
        const char* uri = argv[k];
        k += 1;

        char* payload = NULL;
        UNICOAP_OPTIONS_ALLOC_STATIC(options, 60);
        unicoap_options_clear(&options);

        if (k < argc) {
            payload = argv[k];
            k += 1;

            if ((res = unicoap_options_set_content_format(&options, UNICOAP_FORMAT_TEXT)) < 0) {
                printf("setting Content-Format failed (%i, %s)\n", res, strerror(-res));
                return res;
            }

            if ((res = unicoap_options_set_accept(&options, UNICOAP_FORMAT_TEXT)) < 0) {
                printf("setting Accept failed (%i, %s)\n", res, strerror(-res));
                return res;
            }
        }

        printf("requesting resource\n");

        unicoap_message_t request;
        unicoap_request_init_string_with_options(&request, (unicoap_method_t)method, payload, &options);
        unicoap_destination_t destination = unicoap_destination_uri_string(uri);

        if ((res = unicoap_send_request_async(&request, &destination, _on_response, (void*)(uintptr_t)shell_flags, flags)) < 0) {
            printf("sending request failed (error %i, %s)\n", res, strerror(-res));
            return res;
        }
        printf("request refno=%i\n", res);
        return 0;
    }

    printf("error: missing URI\n");
help:
    return _print_usage(argv);
}

SHELL_COMMAND(coap, "unicoap client", _cli) 

int main(void) {
    /* By default, unicoap_init() is automatically called for you before main().
     * This is because auto_init_unicoap is part of the DEFAULT_MODULE makefile variable.
     * You can opt out of this default behavior by setting DISABLE_MODULE += auto_init_unicoap.
     * However, then you need to call unicoap_init() yourself. */
#if !IS_USED(MODULE_AUTO_INIT_UNICOAP)
    if (unicoap_init() < 0) {
        printf("app: failed to initialize unicoap\n");
    }
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
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
    /* Because we're in possession of an endpoint now, we can print
     * a debug description of it. */
    unicoap_print_endpoint(&udp_local);
    printf("\n");
#endif

    /* If DTLS is enabled, the CoAP over DTLS driver needs to be told
     * at least one DTLS credential. */
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
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

    printf("app: using credential: type=PSK id=%s key=%s\n",
           (char*)credential.params.psk.id.s,
           (char*)credential.params.psk.key.s);
#endif

    printf("app: interfaces have ipv6=[ ");
    netifs_print_ipv6(", ");
    printf(" ]\n");

    /* By default, unicoap_init() will create a background thread. If you do not want that,
     * set CONFIG_UNICOAP_CREATE_THREAD to 0 and run the processing loop on a thread of your choice.
     * Note that this function is not available when CONFIG_UNICOAP_CREATE_THREAD is 1. Multiple
     * unicoap instances are not allowed. Note too that you can add multiple ports instead. */
#if !CONFIG_UNICOAP_CREATE_THREAD
#   error Cannot run unicoap loop on main thread in unicoap client example as shell already runs on main there
    /* unicoap_loop_run(); */
#endif
    /* start shell */
    puts("All up, running unicoap client shell, command is 'coap'");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
