#include "net/nfc/ndef/ndef.h"

#include "errno.h"
#include "log.h"
#include <stdbool.h>
#include <assert.h>

int ndef_add_mime_record(ndef_t *message, const char *mime_type, uint32_t mime_type_length, const char *mime_payload, uint32_t mime_payload_length)
{
    assert(mime_payload_length <= 65535);

    /* the payload will be written later */
    ndef_add_record(message, (uint8_t *)mime_type, mime_type_length, NULL, 0, NULL, mime_payload_length, TNF_MEDIA_TYPE);

    ndef_write_to_buffer(message, (uint8_t *)mime_payload, mime_payload_length);

    return 0;
}

size_t ndef_calculate_mime_record_size(uint32_t mime_type_length, uint32_t mime_payload_length)
{
    return ndef_calculate_record_size(mime_type_length, 0, mime_payload_length);
}