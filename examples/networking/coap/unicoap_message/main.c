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
 * @brief   Demonstration of `unicoap` message, options, and parsing APIs
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "assert.h"
#include "msg.h"
#include "modules.h"
#include "architecture.h"

#include "net/unicoap.h"

static void _example_handle_message(const unicoap_message_t* message)
{
    /* Use unicoap_message_is_request, unicoap_message_is_response, unicoap_message_is_signal
     * to determine what class of message you are dealing with.
     * In this case, we expect a request. */
    assert(unicoap_message_is_request(message->code));

    /* The unicoap message type supports different typed views of the CoAP message code:
     * message->method, message->status, and message->signal.
     *
     * Now, we want to get a string from the method: GET, PUT, POST, etc.
     * unicoap_string_from_* methods generate a null-terminated constant C string
     * from typed values. */
    const char* method_name = unicoap_string_from_method(message->method);
    /* Also possible, more general: unicoap_string_from_code(uint8_t code). */

    printf("PDU is a CoAP %s request"
           " with payload=<%" PRIuSIZE " bytes>\n",
           method_name, message->payload_size);

    /* Options ----------------- */

    /* If you need to dump options quickly, use this function: */
    unicoap_options_dump_all(message->options);

    /* Content-Format is an option that can occur no more than once. */
    unicoap_content_format_t format = 0;
    if (unicoap_options_get_content_format(message->options, &format) < 0) {
        puts("Error: could not read Content-Format!");
        return;
    }

    if (format != UNICOAP_FORMAT_JSON) {
        puts("Error: was expecting JSON request!");
        return;
    }

    /* Uri-Query can occur more than once. Hence, unicoap provides multiple convenience
     * accessors. */
    const char* query = NULL;

    /* The first getter provides a view into the PDU buffer. The returned string
     * is thus not null-terminated. */
    ssize_t res = unicoap_options_get_first_uri_query(message->options, &query);
    if (res < 0) {
        /* The getter also fails in cases where no option was found */
        if (res == -ENOENT) {
            puts("Message has no Uri-Query option");
        }
        printf("Error: could read first Uri-Query option");
    }

    printf("First URI query: '%.*s'\n", (int)res, query);

    /* You can also get a query by name: */
    res = unicoap_options_get_first_uri_query_by_name(message->options, "color", &query);
    if (res < 0) {
        /* The getter also fails in cases where no option was found */
        if (res == -ENOENT) {
            puts("Message has no 'color' query");
        }
        printf("Error: could read first 'color' query");
    }

    /* If you do want to access all values of a repeatable option,
     * unicoap offers two ways:
     * a, unicoap can generate a contiguous string from repeatable options: ?a=1&b=2&c=3
     * b, you iterate over all options using the unicoap option iterator */
    char query_string[50] = { 0 };
    res = unicoap_options_copy_uri_queries(message->options, query_string, sizeof(query_string));
    if (res < 0) {
        puts("Error: could not generate URI query string");
    }

    /* - or - */

    unicoap_options_iterator_t iterator;
    unicoap_options_iterator_init(&iterator, message->options);

    while ((res = unicoap_options_get_next_uri_query(&iterator, &query)) >= 0) {
        printf("- URI query: '%.*s'\n", (int)res, query);
    }

    /* You can also use the option iterator to iterate over all options */
    unicoap_options_iterator_init(&iterator, message->options);
    unicoap_option_number_t number;
    const uint8_t* value = NULL;

    while ((res = unicoap_options_get_next(&iterator, &number, &value)) >= 0) {
        const char* name = unicoap_string_from_option_number(number);
        printf("- option %s nr=%i contains %" PRIuSIZE " bytes\n", name, number, res);
    }
}

static void _example_parse_pdu(void)
{
    ssize_t res = 0;

    /*
     {
       "type": "Confirmable",
       "code": "POST",
       "id": 65201,
       "token": 0,
       "options": [
         "Uri-Path: actuators",
         "Uri-Path: leds",
         "Content-Format: application/json",
         "Uri-Query: color=g",
         "Accept: application/json"
       ]
     }
     */
    const uint8_t pdu[] = {
        0x40, 0x02, 0xfe, 0xb1, 0xb9, 0x61, 0x63, 0x74, 0x75, 0x61, 0x74, 0x6f, 0x72, 0x73, 0x04,
        0x6c, 0x65, 0x64, 0x73, 0x11, 0x32, 0x37, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x3d, 0x67, 0x21,
        0x32, 0xff, 0x6d, 0x6f, 0x64, 0x65, 0x3d, 0x6f, 0x6e
    };

    /* Allocate a helper structure */
    unicoap_parser_result_t parsed = { 0 };

    /* Parse using helper function, _result version initializes options structure for us. */
    if ((res = unicoap_pdu_parse_rfc7252_result((uint8_t*)pdu, sizeof(pdu), &parsed)) < 0) {
        puts("Error: parsing failed");
        return;
    }

    /* To read the header, use the message properties: */
    printf("CoAP message has token=<%i bytes>\n", parsed.properties.token_length);

    /* If the message was sent over CoAP over UDP or DTLS, it has the
     * RFC 7252 CoAP header. This is how you access these properties: */
    printf("CoAP over UDP/DTLS has id=%i type=%s\n",
           parsed.properties.rfc7252.id,
           unicoap_string_from_rfc7252_type(parsed.properties.rfc7252.type));

    _example_handle_message(&parsed.message);
}

static void _example_serialize_message(const unicoap_message_t* message)
{
    /* Well, first we need a buffer. */
    uint8_t pdu[200];

    /* The header format varies depending on the transport. Let's use
     * CoAP over UDP or CoAP over DTLS, i.e., the RFC 7252 format.
     * Also, let's not use a token (very constrained nodes may only handle a request at a time
     * and thus use an empty token value). */
    unicoap_message_properties_t properties = {
        .token = NULL,
        .token_length = 0,
        .rfc7252 = {
            .id = 0xABCD,
            .type = UNICOAP_TYPE_NON
        }
    };

    ssize_t res = unicoap_pdu_build_rfc7252(pdu, sizeof(pdu), message, &properties);
    if (res < 0) {
        if (res == -ENOBUFS) {
            puts("Error: PDU buffer too small");
        }
        puts("Error: could not serialize message");
        return;
    }

    printf("The final PDU has a size of %" PRIuSIZE " bytes.\n", res);
}

static void _example_create_message(void)
{
    /* If you want options, you need to allocate an options object and provide
     * a capacity for the options buffer. The object is stack-allocated. The
     * macro just saves you the boilerplate of allocating a buffer and initializing
     * the options struct. */
    UNICOAP_OPTIONS_ALLOC(options, 100);

    const char payload[] = "Dear thermostat, please adjust your target temperature to 22.5 °C";
    unicoap_message_t message;
    unicoap_request_init_string_with_options(&message, UNICOAP_METHOD_POST, payload, &options);

    /* Options ----------------- */

    /* To set the path, use the following method. There are two versions for null-terminated
     * strings and for ones that aren't. */
    int res = unicoap_options_add_uri_path_string(&options, "/thermostat/temperature");
    if (res < 0) {
        if (res == -ENOBUFS) {
            puts("Error: options buffer too small");
        }
        puts("Error: could not add URI path");
    }

    /* Repeatable options like Uri-Path can also be added individually.
     * This results in /thermostat/temperature/target */
    res = unicoap_options_add_uri_path_component_string(&options, "target");
    if (res < 0) {
        puts("Error: could not add path component");
    }

    /* Uri-Query is a repeatable option, and can thus also be added individually. */
    res = unicoap_options_add_uri_queries_string(&options, "unit=C&cool=yes");
    if (res < 0) {
        puts("Error: could not add URI query");
    }

    /* Simple options like Content-Format can be set as follows: */
    res = unicoap_options_set_content_format(&options, UNICOAP_FORMAT_TEXT);
    if (res < 0) {
        puts("Error: could not set Content-Format");
    }

    /* Let's see if everything has been added: */
    unicoap_options_dump_all(&options);

    _example_serialize_message(&message);
}

int main(void)
{
    puts("Parsing a sample message =============");
    _example_parse_pdu();
    puts("Creating a sample message ============");
    _example_create_message();
}
