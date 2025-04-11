/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "net/nfc/ndef/ndef.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "embUnit.h"

// static uint8_t ndef_with_two_records = {};

static void
print_ndef_as_hex(const ndef_t *message)
{
    size_t size = message->buffer.cursor - message->buffer.memory;
    printf("NDEF message size: %" PRIu16 "\n", size);
    for (uint8_t i = 0; i < size; ++i) {
        if (i % 4 == 0 && i != 0) {
            printf("\n");
        }
        printf("0x%02x ", message->buffer.memory[i]);
    }
    printf("\n\n");
}

static bool test_ndef_text_record(void)
{
    puts("NDEF text record test");
    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    print_ndef_as_hex(&message);
    ndef_pretty_print(&message);
    return true;
}

static bool test_ndef_uri_record(void)
{
    puts("NDEF URI record test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);

    ndef_record_add_uri(&message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);
    print_ndef_as_hex(&message);

    return true;
}

static bool test_ndef_mime_record(void)
{
    puts("\tNDEF MIME record test");
    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);

    ndef_record_add_mime(&message, "text/plain", 10, (uint8_t *)"Hello World", 11);
    ndef_pretty_print(&message);

    return true;
}

static bool test_two_ndef_records(void)
{
    puts("Two NDEF records test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_record_add_text(&message, "Hej Verden", 10, "da", 2, UTF8);
    ndef_pretty_print(&message);
    return true;
}

static bool test_ndef_remove(void)
{
    puts("NDEF remove test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_record_add_text(&message, "Hej Verden", 10, "da", 2, UTF8);
    puts("Before removal:");
    ndef_pretty_print(&message);
    ndef_remove_last_record(&message);

    puts("After removal:");
    ndef_pretty_print(&message);

    return true;
}

static bool test_ndef_calculate_size(void)
{
    puts("NDEF calculate size test");
    uint8_t buffer[1024];
    ndef_t message;
    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    assert((uint32_t)(message.buffer.cursor - message.buffer.memory) == ndef_record_calculate_text_size(11, 2));
    return true;
}

int main(void)
{
    puts("Starting NDEF test");
    test_ndef_text_record();
    test_ndef_uri_record();
    test_ndef_mime_record();
    test_two_ndef_records();
    test_ndef_calculate_size();
    test_ndef_remove();
    puts("Ending NDEF test");
    return 0;
}
