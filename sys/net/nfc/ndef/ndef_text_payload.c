/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     ndef
 * @{
 *
 * @file
 * @brief       Implementation of the NDEF text payload
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 * @}
 */

#include "net/nfc/ndef/ndef.h"

#include "errno.h"
#include "log.h"
#include <stdbool.h>
#include <assert.h>

#define ENCODING_POSITION            7
#define ENCODING_MASK                (1 << ENCODING_POSITION)

#define STATUS_SIZE                  1

#define LENGTH_OF_LANG_CODE_POSITION 0
#define LENGTH_OF_LANG_CODE_MASK     0x3F /* 0011 1111 */

#define MAXIMUM_LANG_CODE_LENGTH     LENGTH_OF_LANG_CODE_MASK

const uint8_t ndef_text_record_type[] = { 'T' };

int ndef_record_add_text(ndef_t *message, const char *text, uint32_t text_length,
                         const char *lang_code,
                         uint8_t lang_code_length, ndef_text_encoding_t encoding)
{
    if (lang_code_length > MAXIMUM_LANG_CODE_LENGTH) {
        LOG_ERROR("NDEF text record language code is too long");
        return -1;
    }

    uint32_t payload_length = STATUS_SIZE + lang_code_length + text_length;
    assert(payload_length <= NDEF_LONG_RECORD_PAYLOAD_LENGTH);

    uint8_t status_byte = 0;

    /* this is how the status byte is constructed S0LL LLLL */
    status_byte |= (lang_code_length << LENGTH_OF_LANG_CODE_POSITION);
    status_byte |= encoding << ENCODING_POSITION;

    /* the payload will be written later */
    int res = ndef_add_record(message, ndef_text_record_type, sizeof(ndef_text_record_type), NULL, 0, NULL,
                              payload_length, NDEF_TNF_WELL_KNOWN);
    if (res < 0) {
        return res;
    }

    ndef_put_into_buffer(message, &status_byte, 1);
    ndef_put_into_buffer(message, (uint8_t *)lang_code, lang_code_length);
    ndef_put_into_buffer(message, (uint8_t *)text, text_length);

    return 0;
}

size_t ndef_record_calculate_text_size(uint32_t text_length, uint8_t lang_code_length)
{
    size_t payload_size = STATUS_SIZE + lang_code_length + text_length;
    return ndef_calculate_record_size(sizeof(ndef_text_record_type), 0, payload_size);
}
