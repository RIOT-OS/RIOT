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
 * @brief       Implementation of the NDEF message format
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 * @}
 */

#include "net/nfc/ndef/ndef.h"
#include "log.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <byteorder.h>

#define SHORT_RECORD_PAYLOAD_LENGTH_SIZE 1
#define LONG_RECORD_PAYLOAD_LENGTH_SIZE  4

/* Structure of the header byte */
#define RECORD_MB_MASK                   0x80
#define RECORD_ME_MASK                   0x40
#define RECORD_CF_MASK                   0x20
#define RECORD_SR_MASK                   0x10
#define RECORD_IL_MASK                   0x08
#define RECORD_TNF_MASK                  0x07

size_t ndef_get_size(const ndef_t *ndef)
{
    return ndef->buffer.cursor - ndef->buffer.memory;
}

void ndef_pretty_print(const ndef_record_desc_t *ndef_record_descriptors, size_t record_count)
{
    printf("----------------\n");
    printf("\n");
    for (size_t i = 0; i < (size_t)record_count; ++i) {
        const ndef_record_desc_t *record = &ndef_record_descriptors[i];
        printf("Record %d\n", i);
        printf("----\n");
        printf("MB: %d\n", (record->header[0] & RECORD_MB_MASK) != 0);
        printf("ME: %d\n", (record->header[0] & RECORD_ME_MASK) != 0);
        printf("CF: %d\n", (record->header[0] & RECORD_CF_MASK) != 0);
        printf("SR: %d\n", (record->header[0] & RECORD_SR_MASK) != 0);
        printf("IL: %d\n", (record->header[0] & RECORD_IL_MASK) != 0);
        printf("TNF: %d\n", record->header[0] & RECORD_TNF_MASK);
        printf("Type length: %d\n", record->type_length[0]);
        printf("Type: ");
        for (int j = 0; j < record->type_length[0]; ++j) {
            printf("%c", record->type[j]);
        }
        printf("\n");

        printf("Payload length size: %u\n", record->record_type);
        uint32_t payload_length;
        if (record->record_type == NDEF_SHORT_RECORD) {
            payload_length = record->payload_length[0];
        }
        else {
            payload_length = ((uint32_t) record->payload_length[0]) << 24 |
                             ((uint32_t) record->payload_length[1]) << 16 |
                             ((uint32_t) record->payload_length[2]) <<  8 |
                             ((uint32_t) record->payload_length[3]);
        }

        printf("Payload length: %" PRIu32 "\n", payload_length);
        printf("Payload: ");
        for (size_t j = 0; j < (size_t)payload_length; ++j) {
            printf("0x%02x ", record->payload[j]);
        }
        printf("\n\n");
    }
    printf("----------------");
    printf("\n");
}

uint8_t *ndef_put_into_buffer(ndef_t *ndef, const uint8_t *data, uint32_t data_length)
{
    uint8_t *before_write = ndef->buffer.cursor;

    if (ndef->buffer.cursor + data_length > ndef->buffer.memory_end) {
        LOG_ERROR("NDEF buffer overflow\n");
        return NULL;
    }

    memcpy(ndef->buffer.cursor, data, data_length);
    ndef->buffer.cursor += data_length;

    return before_write;
}

void ndef_init(ndef_t *ndef, uint8_t *buffer, uint32_t buffer_size)
{
    /* make sure that all pointers are valid */
    assert(ndef != NULL);
    assert(buffer != NULL);

    ndef->buffer.memory = buffer;

    /* the end of memory is the last valid byte */
    ndef->buffer.memory_end = ndef->buffer.memory + buffer_size - 1;
    ndef->buffer.cursor = buffer;
    ndef->record_count = 0;
}

int ndef_add_record(ndef_t *ndef, const uint8_t *type, uint8_t type_length, const uint8_t *id,
                    uint8_t id_length, const uint8_t *payload,
                    uint32_t payload_length, ndef_record_tnf_t tnf)
{
    /* make sure that the message is valid */
    assert(ndef != NULL);

    /* a payload must be bigger than zero and smaller than the maximum NDEF payload length */
    assert(payload_length > 0);
    assert(payload_length <= NDEF_LONG_RECORD_PAYLOAD_LENGTH);



    /* the maximum is specified at compile time */
    if (ndef->record_count >= MAX_NDEF_RECORD_COUNT) {
        LOG_ERROR("NDEF record count exceeds maximum\n");
        return -1;
    }

    uint8_t *start_pointer = ndef->buffer.cursor;

    bool mb, me, cf, sr, il;

    /**
     *  The ME flag can only be set on the last record of the NDEF message.
     *  The second to last record should have the ME flag cleared.
     */
    if (ndef->record_count >= 1) {
        *(ndef->records[ndef->record_count - 1]) &= ~RECORD_ME_MASK;
        mb = false;
    }
    else {
        mb = true;
    }

    /* message end has to be true because records are only added at the end */
    me = true;

    /* chunking is not supported */
    cf = false;

    /* if the payload is short, set sr (short record) to true */
    if (payload_length <= NDEF_SHORT_RECORD_PAYLOAD_LENGTH) {
        sr = true;
    }
    else {
        sr = false;
    }

    /* if an ID is present, set il to true */
    if (id) {
        il = true;
    }
    else {
        il = false;
    }

    /* the header byte consists of the different flags and the Type Name Format (TNF) */
    uint8_t header_byte = 0;
    header_byte |= mb ? RECORD_MB_MASK : 0;
    header_byte |= me ? RECORD_ME_MASK : 0;
    header_byte |= cf ? RECORD_CF_MASK : 0;
    header_byte |= sr ? RECORD_SR_MASK : 0;
    header_byte |= il ? RECORD_IL_MASK : 0;
    header_byte |= tnf;

    ndef_put_into_buffer(ndef, &header_byte, NDEF_RECORD_HEADER_SIZE);
    ndef_put_into_buffer(ndef, &type_length, NDEF_RECORD_TYPE_LENGTH_SIZE);

    if (sr) {
        uint8_t payload_length_single_byte = (uint8_t)payload_length & 0xFF;
        ndef_put_into_buffer(ndef,
                             (uint8_t *)&payload_length_single_byte,
                             SHORT_RECORD_PAYLOAD_LENGTH_SIZE);
    }
    else {
        /* payload length shall be MSB first or Big Endian */
        uint32_t payload_length_multi_byte = htonl(payload_length);
        ndef_put_into_buffer(ndef,
                             (uint8_t *)&payload_length_multi_byte,
                             LONG_RECORD_PAYLOAD_LENGTH_SIZE);
    }

    if (id) {
        ndef_put_into_buffer(ndef, &id_length, NDEF_RECORD_ID_LENGTH_SIZE);
    }

    ndef_put_into_buffer(ndef, (uint8_t *)type, type_length);

    if (id) {
        ndef_put_into_buffer(ndef, id, id_length);
    }

    if (payload != NULL) {
        ndef_put_into_buffer(ndef, payload, payload_length);
    }
    else {
        /**
         *  this needs to be done so the payload points to the correct position
         *  for further writes
         */

        /* the payload has to be written by the calling function */
        (void)payload;
    }

    ndef->records[ndef->record_count] = start_pointer;
    ndef->record_count += 1;

    return 0;
}

int ndef_remove_last_record(ndef_t *ndef)
{
    assert(ndef != NULL);

    if (ndef->record_count == 0) {
        LOG_ERROR("No records to remove");
        return -1;
    }

    /* for only one record we simply set the count and cursor to 0 */
    if (ndef->record_count == 1) {
        ndef_clear(ndef);
        return 0;
    }

    /**
     * the new cursor position can be found by looking at the start pointer
     * of the last record and setting the cursor to that location
     */
    uint8_t *last_record = ndef->records[ndef->record_count - 1];
    ndef->buffer.cursor = last_record;
    ndef->record_count -= 1;

    /* the last record must now have ME to 1 */
    uint8_t *header = ndef->records[ndef->record_count - 1];
    *header |= RECORD_ME_MASK;

    return 0;
}

void ndef_clear(ndef_t *ndef)
{
    assert(ndef != NULL);

    ndef->record_count = 0;
    ndef->buffer.cursor = ndef->buffer.memory;
}

size_t ndef_calculate_record_size(uint8_t type_length, uint8_t id_length, uint32_t payload_length)
{
    size_t size = NDEF_RECORD_HEADER_SIZE + NDEF_RECORD_TYPE_LENGTH_SIZE + type_length;

    if (id_length > 0) {
        size += NDEF_RECORD_ID_LENGTH_SIZE + id_length;
    }

    if (payload_length <= NDEF_SHORT_RECORD_PAYLOAD_LENGTH) {
        size += SHORT_RECORD_PAYLOAD_LENGTH_SIZE;
    }
    else {
        size += LONG_RECORD_PAYLOAD_LENGTH_SIZE;
    }

    size += payload_length;

    return size;
}

void ndef_parse_record(const uint8_t *ndef_record, ndef_record_desc_t *record_desc)
{
    assert(ndef_record != NULL);
    assert(record_desc != NULL);

    /* the header byte is the first byte of the record */
    record_desc->header = (uint8_t *)ndef_record;

    /* check if the TNF is set to EMPTY */
    if ((*(record_desc->header) & RECORD_TNF_MASK) == NDEF_TNF_EMPTY) {
        /* type length and payload length are 0, the respective data fields are omitted */
        record_desc->type_length = (record_desc->header + NDEF_RECORD_HEADER_SIZE);
        record_desc->payload_length = (record_desc->type_length + NDEF_RECORD_TYPE_LENGTH_SIZE);
        record_desc->type = NULL;
        record_desc->payload = NULL;
        if (*(record_desc->header) & RECORD_IL_MASK) {
            record_desc->id_length = (uint8_t *)(record_desc->payload_length +
                                                 SHORT_RECORD_PAYLOAD_LENGTH_SIZE);
            record_desc->id = record_desc->id_length + NDEF_RECORD_ID_LENGTH_SIZE;
        }
        else {
            record_desc->id_length = NULL;
            record_desc->id = NULL;
        }
        return;
    }

    record_desc->record_type = (*(record_desc->header) & RECORD_SR_MASK) ? NDEF_SHORT_RECORD
                                                                         : NDEF_LONG_RECORD;

    record_desc->type_length = (uint8_t *)(record_desc->header + NDEF_RECORD_HEADER_SIZE);
    record_desc->payload_length = (uint8_t *)(record_desc->type_length +
                                              NDEF_RECORD_TYPE_LENGTH_SIZE);

    /* the pointer for any field after the payload_length depends on the type of the record */
    uint8_t *pointer_after_payload_length = (uint8_t *)(record_desc->payload_length +
                                                        ((record_desc->record_type ==
                                                        NDEF_SHORT_RECORD) ?
                                                        SHORT_RECORD_PAYLOAD_LENGTH_SIZE :
                                                        LONG_RECORD_PAYLOAD_LENGTH_SIZE));


    if (*(record_desc->header) & RECORD_IL_MASK) {
        record_desc->id_length = pointer_after_payload_length;
        record_desc->type = (uint8_t *)(record_desc->id_length + NDEF_RECORD_ID_LENGTH_SIZE);
        record_desc->id = (uint8_t *)(record_desc->type + *(record_desc->type_length));
        record_desc->payload = (uint8_t *)(record_desc->id + *(record_desc->id_length));
    }
    else {
        record_desc->id_length = NULL;
        record_desc->type = pointer_after_payload_length;
        record_desc->id = NULL;
        record_desc->payload = (uint8_t *)(record_desc->type + *(record_desc->type_length));
    }

    return;
}

int ndef_parse(const ndef_t *ndef, ndef_record_desc_t *record_descriptors,
                            size_t record_descriptors_size) {
    assert(ndef != NULL);
    assert(record_descriptors != NULL);

    if (ndef->record_count > record_descriptors_size) {
        LOG_ERROR("Too many records to parse");
        return -1;
    }

    if (ndef->record_count == 0) {
        LOG_ERROR("No records to parse");
        return -1;
    }

    for (size_t i = 0; i < ndef->record_count; ++i) {
        ndef_record_desc_t *record_desc = &record_descriptors[i];
        ndef_parse_record(ndef->records[i], record_desc);
    }
    return 0;
}
