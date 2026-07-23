/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup sys_shell_commands
 * @brief   `coap` client shell command
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "net/unicoap.h"
#include "shell.h"
#include "od.h"

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

    printf("response ");
    unicoap_print_code(response->code);
    printf(" (%" PRIuSIZE " bytes)\n", response->payload_size);

    unicoap_content_format_t format;
    if (((error = unicoap_options_get_content_format(response->options, &format)) >= 0
         && unicoap_content_format_is_human_readable(format))
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
        if ((res = unicoap_cancel_request(atoi(argv[2]))) < 0) {
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
