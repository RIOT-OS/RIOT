#include "net/nfc/ndef/ndef.h"

#include "errno.h"
#include "log.h"
#include <stdbool.h>
#include <assert.h>

#define ENCODING_POSITION            7
#define ENCODING_MASK                (1 << ENCODING_POSITION)

#define STATUS_SIZE                  1

#define LENGTH_OF_LANG_CODE_POSITION 0
#define LENGTH_OF_LANG_CODE_MASK     0x3F // 0011 1111

#define MAXIMUM_LANG_CODE_LENGTH     LENGTH_OF_LANG_CODE_MASK

const uint8_t ndef_text_record_type[] = { 'T' };

int ndef_add_text_record(ndef_t *message, const char *text, uint32_t text_length,
                         const char *lang_code,
                         uint8_t lang_code_length, ndef_text_encoding_t encoding)
{
    uint32_t payload_length = STATUS_SIZE + lang_code_length + text_length;
    assert(payload_length <= 65535);

    uint8_t status_byte = 0;

    if (lang_code_length > MAXIMUM_LANG_CODE_LENGTH) {
        LOG_ERROR("NDEF text record language code is too long");
        return -EINVAL;
    }

    // S-LL LLLL
    status_byte |= (lang_code_length << LENGTH_OF_LANG_CODE_POSITION);
    status_byte |= encoding << ENCODING_POSITION;

    /*
    bool mb;
    if (message->record_count == 0) {
        mb = true;
    } else {
        mb = false;
    }

    bool sr;
    if (payload_length <= 255) {
        sr = true;
    } else {
        sr = false;
    }
    */

    /* the payload will be written later */
    ndef_add_record(message, ndef_text_record_type, sizeof(ndef_text_record_type), NULL, 0, NULL,
                    payload_length, TNF_WELL_KNOWN);

    ndef_write_to_buffer(message, &status_byte, 1);
    ndef_write_to_buffer(message, (uint8_t *)lang_code, lang_code_length);
    ndef_write_to_buffer(message, (uint8_t *)text, text_length);

    return 0;
}

size_t ndef_calculate_text_record_size(uint32_t text_length, uint8_t lang_code_length)
{
    size_t payload_size = STATUS_SIZE + lang_code_length + text_length;
    return ndef_calculate_record_size(sizeof(ndef_text_record_type), 0, payload_size);
}