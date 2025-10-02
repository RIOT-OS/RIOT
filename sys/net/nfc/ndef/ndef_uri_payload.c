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
 * @brief       Implementation of the NDEF URI payload
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

#define IDENTIFIER_CODE_LENGTH 1

const uint8_t ndef_uri_record_type[] = { 'U' };

int ndef_record_add_uri(ndef_t *ndef, ndef_uri_identifier_code_t identifier_code, const char *uri, uint32_t uri_length)
{
    uint32_t payload_length = IDENTIFIER_CODE_LENGTH + uri_length;
    assert(payload_length <= 65535);

    /* the payload will be written later */
    int res = ndef_add_record(ndef, ndef_uri_record_type, sizeof(ndef_uri_record_type),
                              NULL, 0, NULL, payload_length, NDEF_TNF_WELL_KNOWN);
    if (res < 0) {
        return res;
    }

    /* write the payload here */
    ndef_put_into_buffer(ndef, (uint8_t *)&identifier_code, IDENTIFIER_CODE_LENGTH);
    ndef_put_into_buffer(ndef, (uint8_t *)uri, uri_length);

    return 0;
}

size_t ndef_record_calculate_uri_size(uint32_t uri_length)
{
    size_t payload_length = IDENTIFIER_CODE_LENGTH + uri_length;
    return ndef_calculate_record_size(sizeof(ndef_uri_record_type), 0, payload_length);
}
