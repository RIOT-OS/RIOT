#include "net/nfc/ndef/ndef.h"

#include "errno.h"
#include "log.h"
#include <stdbool.h>
#include <assert.h>

#define ENCODING_POSITION 7
#define ENCODING_MASK (1 << ENCODING_POSITION)

#define STATUS_SIZE 1

#define LENGTH_OF_LANG_CODE_POSITION 0
#define LENGTH_OF_LANG_CODE_MASK 0x3F // 0011 1111

#define MAXIMUM_LANG_CODE_LENGTH LENGTH_OF_LANG_CODE_MASK

uint8_t const ndef_text_record_type[] = {'T'};

int ndef_add_text_record(ndef_t *message, char const *text, uint32_t text_length, 
char const *lang_code, uint8_t lang_code_length, ndef_text_encoding_t encoding) {
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
    ndef_write_to_buffer(message, (uint8_t*) lang_code, lang_code_length);
    ndef_write_to_buffer(message, (uint8_t*) text, text_length);

    return 0;
}

/*
int encode_ndef_text_payload(ndef_text_payload_t *payload, uint8_t *buffer, uint32_t len) {
    if ((!payload->language.lang_code_length) || (!payload->language.lang_code) || (!payload->text_length) || (!payload->text) || (!len)) {
        return -EINVAL;
    }

    if (payload->language.lang_code_length > MAXIMUM_LANG_CODE_LENGTH) {
        return -EINVAL;
    }


    uint32_t payload_size = 1 + payload->language.lang_code_length + payload->text_length;

    if (buffer) {
        if (payload_size > len) {
            return -ENOSR;
        }

        *buffer = (payload->language.lang_code_length || (payload->encoding << ENCODING_POSITION));
        buffer += 1;

        memcpy(buffer, payload->language.lang_code, payload->language.lang_code_length);
        buffer += payload->language.lang_code_length;

        memcpy(buffer, payload->text, payload->text_length);
    }
    return 0;
}
*/