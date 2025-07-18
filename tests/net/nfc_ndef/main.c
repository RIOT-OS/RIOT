#include "net/nfc/ndef/ndef.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

static void print_ndef_as_hex(const ndef_t *message)
{
    printf("NDEF message size: %" PRIu32 "\n", message->buffer.cursor);
    for (uint32_t i = 0; i < message->buffer.cursor; ++i) {
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
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
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

    ndef_add_uri_record(&message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);
    print_ndef_as_hex(&message);

    return true;
}

static bool test_ndef_mime_record(void)
{
    puts("\tNDEF MIME record test");
    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);

    ndef_add_mime_record(&message, "text/plain", 10, (uint8_t *)"Hello World", 11);
    ndef_pretty_print(&message);

    return true;
}

static bool test_two_ndef_records(void)
{
    puts("Two NDEF records test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_text_record(&message, "Hej Verden", 10, "da", 2, UTF8);
    ndef_pretty_print(&message);
    return true;
}

static bool test_ndef_remove(void)
{
    puts("NDEF remove test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_text_record(&message, "Hej Verden", 10, "da", 2, UTF8);
    puts("Before removal:");
    ndef_pretty_print(&message);
    ndef_remove_record(&message);

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
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    assert(message.buffer.cursor == ndef_calculate_text_record_size(11, 2));
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