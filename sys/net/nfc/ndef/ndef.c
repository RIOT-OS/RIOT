/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "fmt.h"
#include "log.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <byteorder.h>

#define NDEF_RECORD_PAYLOAD_LENGTH_MAXIMUM 65535

#define SHORT_RECORD_PAYLOAD_LENGTH_SIZE 1
#define LONG_RECORD_PAYLOAD_LENGTH_SIZE  4

/* Structure of the header byte */
#define RECORD_MB_MASK                   (1 << 7)
#define RECORD_ME_MASK                   (1 << 6)
#define RECORD_CF_MASK                   (1 << 5)
#define RECORD_SR_MASK                   (1 << 4)
#define RECORD_IL_MASK                   (1 << 3)
#define RECORD_TNF_MASK                  (0b00000111)

size_t ndef_get_size(const ndef_t *ndef)
{
    return (size_t) (ndef->buffer.cursor - ndef->buffer.memory);
}

size_t ndef_get_capacity(const ndef_t *ndef) {
    assert(ndef != NULL);

    return (size_t)(ndef->buffer.memory_end - ndef->buffer.memory + 1);
}

#define MAXIMUM_UINT_16_SIZE 6
/**
 * @brief Prints a 16-bit unsigned integer
 */
static inline void _print_u16_dec(uint16_t n) {
    char str[MAXIMUM_UINT_16_SIZE];
    fmt_u16_dec(str, n);
    print_str(str);
}

/**
 * @brief Prints an array of NDEF record descriptors
 *
 * @param[in] ndef_record_descriptors   Array of record descriptors
 * @param[in] record_count              Number of record descriptors in array
 */
static void ndef_record_descriptors_pretty_print(const ndef_record_desc_t *ndef_record_descriptors,
    size_t record_count)
{
    print_str("----------------\n\n");
    for (size_t i = 0; i < (size_t)record_count; ++i) {
        const ndef_record_desc_t *record = &ndef_record_descriptors[i];
        print_str("Record "); print_u32_dec(i); print_str("\n");
        print_str("----\n");
        print_str("MB: "); _print_u16_dec((record->header[0] & RECORD_MB_MASK) != 0); print_str("\n");
        print_str("ME: "); _print_u16_dec((record->header[0] & RECORD_ME_MASK) != 0); print_str("\n");
        print_str("CF: "); _print_u16_dec((record->header[0] & RECORD_CF_MASK) != 0); print_str("\n");
        print_str("SR: "); _print_u16_dec((record->header[0] & RECORD_SR_MASK) != 0); print_str("\n");
        print_str("IL: "); _print_u16_dec((record->header[0] & RECORD_IL_MASK) != 0); print_str("\n");
        print_str("TNF: "); _print_u16_dec(record->header[0] & RECORD_TNF_MASK); print_str("\n");
        print_str("Type length: "); _print_u16_dec(record->type_length[0]); print_str("\n");
        print_str("Type: "); print((char *) record->type, record->type_length[0]); print_str("\n");
        print_str("Payload length size: "); print_u32_dec(record->record_type); print_str("\n");
        uint32_t payload_length;
        if (record->record_type == NDEF_RECORD_SHORT) {
            payload_length = record->payload_length[0];
        } else {
            payload_length = ((uint32_t)record->payload_length[0]) << 24 |
                             ((uint32_t)record->payload_length[1]) << 16 |
                             ((uint32_t)record->payload_length[2]) << 8  |
                             ((uint32_t)record->payload_length[3]);
        }

        print_str("Payload length: "); print_u32_dec(payload_length); print_str("\n");
        print_str("Payload: "); print_bytes_hex(record->payload, payload_length); print_str("\n\n");
    }
    print_str("----------------\n");
}

void ndef_pretty_print(const ndef_t *ndef) {
    ndef_record_desc_t ndef_record_descriptors[CONFIG_NDEF_MAX_RECORD_COUNT];
    ndef_parse(ndef, ndef_record_descriptors, CONFIG_NDEF_MAX_RECORD_COUNT);
    ndef_record_descriptors_pretty_print(ndef_record_descriptors, ndef->record_count);
}

/**
 * @brief Writes the data buffer to the given NDEF message.
 *
 * @note This writes the data of the given buffer and
 * shifts the cursor of the NDEF buffer by the length of the data.
 *
 * @param[in,out]   ndef            The NDEF message that gets the data written to its memory
 * @param[in]       data            Data to write
 * @param[in]       data_length     Length of the data
 *
 * @retval Pointer to the memory cursor before the write
 * @retval NULL if the data is too long
 */
static uint8_t *ndef_put_into_buffer(ndef_t *ndef, const uint8_t *data, uint32_t data_length)
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

int ndef_record_header_add(ndef_t *ndef, const uint8_t *type, uint8_t type_length,
    const uint8_t *id, uint8_t id_length, uint32_t payload_length, ndef_record_tnf_t tnf)
{
    /* make sure that the message is valid */
    assert(ndef != NULL);

    /* a payload must be bigger than zero and smaller than the maximum NDEF payload length */
    assert(payload_length > 0);
    assert(payload_length <= NDEF_RECORD_LONG_PAYLOAD_LENGTH);

    /* the maximum is specified at compile time */
    if (ndef->record_count >= CONFIG_NDEF_MAX_RECORD_COUNT) {
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

    /**
     * Chunking is not supported and not needed for tag functionality. Furthermore, chunking
     * is not a mechanism for introducing multiplexing or data streaming into NDEF and it MUST NOT
     * be used for those purposes. It is a mechanism to reduce the need for outbound
     * buffering on the generating side. This is similar to the message chunking mechanism
     * defined in HTTP/1.1 [RFC 2616].
     */
    cf = false;

    /* if the payload is short, set sr (short record) to true */
    sr = (payload_length <= NDEF_RECORD_SHORT_PAYLOAD_LENGTH);

    /* if an ID is present, set il to true */
    il = (id != NULL);

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
                             &payload_length_single_byte,
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

    ndef->records[ndef->record_count] = start_pointer;
    ndef->record_count += 1;

    return 0;
}

int ndef_record_remove_last(ndef_t *ndef)
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

/**
 * @brief Calculates the expected size of an NDEF record
 *
 * @param[in] type_length       Length of type field
 * @param[in] id_length         Length of ID field
 * @param[in] payload_length    Length of payload field
 * @return size_t Size of the record
 */
static inline size_t ndef_record_calculate_size(uint8_t type_length, uint8_t id_length,
    uint32_t payload_length)
{
    size_t size = NDEF_RECORD_HEADER_SIZE + NDEF_RECORD_TYPE_LENGTH_SIZE + type_length;

    if (id_length > 0) {
        size += NDEF_RECORD_ID_LENGTH_SIZE + id_length;
    }

    if (payload_length <= NDEF_RECORD_SHORT_PAYLOAD_LENGTH) {
        size += NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE;
    }
    else {
        size += NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE;
    }

    size += payload_length;

    return size;
}

/**
 * @brief Parses a single NDEF record and returns its record descriptor
 *
 * @param[in]  ndef_record  the NDEF record
 * @param[out] record_desc  the NDEF record descriptor
 */
static void ndef_record_parse(const uint8_t *ndef_record, ndef_record_desc_t *record_desc)
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

    record_desc->record_type = (*(record_desc->header) & RECORD_SR_MASK)
        ? NDEF_RECORD_SHORT : NDEF_RECORD_LONG;

    record_desc->type_length = (uint8_t *)(record_desc->header + NDEF_RECORD_HEADER_SIZE);
    record_desc->payload_length = (uint8_t *)(record_desc->type_length +
                                              NDEF_RECORD_TYPE_LENGTH_SIZE);

    /* the pointer for any field after the payload_length depends on the type of the record */
    uint8_t *pointer_after_payload_length = (uint8_t *)(record_desc->payload_length +
                                                        ((record_desc->record_type ==
                                                          NDEF_RECORD_SHORT) ?
                                                             NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE :
                                                             NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE));

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
               size_t record_descriptors_size)
{
    assert(ndef != NULL);
    assert(record_descriptors != NULL);

    if (ndef->record_count == 0) {
        LOG_ERROR("No records to parse");
        return -1;
    }

    int ret;
    if (ndef->record_count > record_descriptors_size) {
        LOG_ERROR("Can only parse %zu of %u records", record_descriptors_size,
            ndef->record_count);
        ret = -2;
    } else {
        ret = 0;
    }

    for (size_t i = 0; i < record_descriptors_size; ++i) {
        ndef_record_desc_t *record_desc = &record_descriptors[i];
        ndef_record_parse(ndef->records[i], record_desc);
    }
    return ret;
}

int ndef_from_buffer(ndef_t *ndef, uint8_t *buffer, size_t buffer_size)
{
    assert(ndef != NULL);

    ndef_init(ndef, buffer, buffer_size);
    uint8_t *current_pointer = ndef->buffer.memory;
    while (current_pointer < ndef->buffer.cursor) {
        ndef_record_desc_t record_desc;
        ndef_record_parse(current_pointer, &record_desc);

        ndef->records[ndef->record_count] = current_pointer;
        ndef->record_count += 1;

        if (record_desc.record_type == NDEF_RECORD_SHORT) {
            current_pointer += NDEF_RECORD_HEADER_SIZE + NDEF_RECORD_TYPE_LENGTH_SIZE +
                               SHORT_RECORD_PAYLOAD_LENGTH_SIZE + *(record_desc.type_length);

            if (record_desc.id_length) {
                current_pointer += NDEF_RECORD_ID_LENGTH_SIZE + *(record_desc.id_length);
            }

            current_pointer += *(record_desc.payload_length);
        }
        else {
            current_pointer += NDEF_RECORD_HEADER_SIZE + NDEF_RECORD_TYPE_LENGTH_SIZE +
                               LONG_RECORD_PAYLOAD_LENGTH_SIZE + *(record_desc.type_length);

            if (record_desc.id_length) {
                current_pointer += NDEF_RECORD_ID_LENGTH_SIZE + *(record_desc.id_length);
            }

            uint32_t payload_length = ((uint32_t)record_desc.payload_length[0]) << 24 |
                                     ((uint32_t)record_desc.payload_length[1]) << 16 |
                                     ((uint32_t)record_desc.payload_length[2]) << 8 |
                                     ((uint32_t)record_desc.payload_length[3]);
            current_pointer += payload_length;
        }
    }

    if (ndef->record_count > CONFIG_NDEF_MAX_RECORD_COUNT) {
        LOG_ERROR("NDEF record count exceeds maximum\n");
        return -1;
    }

    return 0;
}

/* MARK: MIME */
int ndef_record_mime_add(ndef_t *ndef, const char *mime_type, uint32_t mime_type_length,
                         const uint8_t *mime_payload, uint32_t mime_payload_length)
{
    assert(mime_payload_length <= NDEF_RECORD_LONG_PAYLOAD_LENGTH);

    ndef_record_header_add(ndef, (uint8_t *)mime_type, mime_type_length, NULL, 0,
                    mime_payload_length, NDEF_TNF_MEDIA_TYPE);

    ndef_put_into_buffer(ndef, (uint8_t *)mime_payload, mime_payload_length);

    return 0;
}

/* MARK: Text */
#define ENCODING_POSITION            7
#define ENCODING_MASK                (1 << ENCODING_POSITION)

#define STATUS_SIZE                  1

#define LENGTH_OF_LANG_CODE_POSITION 0
#define LENGTH_OF_LANG_CODE_MASK     0x3F /* 0011 1111 */

#define MAXIMUM_LANG_CODE_LENGTH     LENGTH_OF_LANG_CODE_MASK

const uint8_t ndef_text_record_type[] = { 'T' };

int ndef_record_text_add(ndef_t *ndef, const char *text, uint32_t text_length,
                         const char *lang_code, uint8_t lang_code_length,
                         ndef_text_encoding_t encoding)
{
    if (lang_code_length > MAXIMUM_LANG_CODE_LENGTH) {
        LOG_ERROR("NDEF text record language code is too long");
        return -1;
    }

    uint32_t payload_length = STATUS_SIZE + lang_code_length + text_length;
    assert(payload_length <= NDEF_RECORD_LONG_PAYLOAD_LENGTH);

    uint8_t status_byte = 0;

    /* this is how the status byte is constructed S0LL LLLL */
    status_byte |= (lang_code_length << LENGTH_OF_LANG_CODE_POSITION);
    status_byte |= encoding << ENCODING_POSITION;

    int res = ndef_record_header_add(ndef, ndef_text_record_type, sizeof(ndef_text_record_type),
            NULL, 0, payload_length, NDEF_TNF_WELL_KNOWN);
    if (res < 0) {
        return res;
    }

    /* writes the payload */
    ndef_put_into_buffer(ndef, &status_byte, 1);
    ndef_put_into_buffer(ndef, (uint8_t *)lang_code, lang_code_length);
    ndef_put_into_buffer(ndef, (uint8_t *)text, text_length);

    return 0;
}

/* MARK: URI */
#define IDENTIFIER_CODE_LENGTH 1

const uint8_t ndef_uri_record_type[] = { 'U' };

/**
 * @brief Calculates the size of an NDEF URI record
 *
 * @param[in] uri_length Length of the URI
 * @return Size of the URI record
 */
int ndef_record_uri_add(ndef_t *ndef, ndef_uri_identifier_code_t identifier_code,
    const char *uri, uint32_t uri_length)
{
    uint32_t payload_length = IDENTIFIER_CODE_LENGTH + uri_length;
    assert(payload_length <= NDEF_RECORD_PAYLOAD_LENGTH_MAXIMUM);

    int res = ndef_record_header_add(ndef, ndef_uri_record_type, sizeof(ndef_uri_record_type),
                                     NULL, 0, payload_length, NDEF_TNF_WELL_KNOWN);
    if (res < 0) {
        return res;
    }

    /* writes the payload */
    ndef_put_into_buffer(ndef, (uint8_t *)&identifier_code, IDENTIFIER_CODE_LENGTH);
    ndef_put_into_buffer(ndef, (uint8_t *)uri, uri_length);

    return 0;
}
