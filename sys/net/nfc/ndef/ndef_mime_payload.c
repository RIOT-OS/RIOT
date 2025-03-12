#include "net/nfc/ndef/ndef.h"

#include "errno.h"
#include "log.h"
#include <stdbool.h>
#include <assert.h>

int ndef_record_add_mime(ndef_t *message, const char *mime_type, uint32_t mime_type_length,
                         const uint8_t *mime_payload, uint32_t mime_payload_length)
{
    assert(mime_payload_length <= 65535);

    /* the payload will be written later */
    ndef_add_record(message, (uint8_t *)mime_type, mime_type_length, NULL, 0, NULL,
                    mime_payload_length, NDEF_TNF_MEDIA_TYPE);

    ndef_write_to_buffer(message, (uint8_t *)mime_payload, mime_payload_length);

    return 0;
}

size_t ndef_record_calculate_mime_size(uint32_t mime_type_length, uint32_t mime_payload_length)
{
    return ndef_calculate_record_size(mime_type_length, 0, mime_payload_length);
}
