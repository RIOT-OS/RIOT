#include "net/nfc/ndef/ndef.h"
#include "net/nfc/ndef/ndef_text_payload.h"
#include "log.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#define SHORT_RECORD_PAYLOAD_LENGTH      255
#define LONG_RECORD_PAYLOAD_LENGTH       65535

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
    return ndef->buffer.cursor;
}

void ndef_pretty_print(const ndef_t *ndef)
{
    printf("----------------\n");
    printf("NDEF Printing\n");
    printf("NDEF message records: %" PRIu32 "\n", ndef->record_count);
    printf("\n");
    for (size_t i = 0; i < (size_t)ndef->record_count; ++i) {
        const ndef_record_desc_t *record = &ndef->records[i];
        printf("Record %d\n", i);
        printf("----\n");
        printf("MB: %d\n", record->header[0] & RECORD_MB_MASK ? true : false);
        printf("ME: %d\n", record->header[0] & RECORD_ME_MASK ? true : false);
        printf("CF: %d\n", record->header[0] & RECORD_CF_MASK ? true : false);
        printf("SR: %d\n", record->header[0] & RECORD_SR_MASK ? true : false);
        printf("IL: %d\n", record->header[0] & RECORD_IL_MASK ? true : false);
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
            payload_length = record->payload_length[0] << 24 | record->payload_length[1] << 16 | record->payload_length[2] << 8 | record->payload_length[3];
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

uint8_t *ndef_write_to_buffer(ndef_t *ndef, const uint8_t *data, uint32_t data_length)
{
    uint8_t *before_write = &(ndef->buffer.memory[ndef->buffer.cursor]);
    if (ndef->buffer.cursor + data_length > ndef->buffer.memory_size) {
        LOG_ERROR("NDEF buffer overflow\n");
        return NULL;
    }
    memcpy(&ndef->buffer.memory[ndef->buffer.cursor], data, data_length);
    ndef->buffer.cursor += data_length;
    return before_write;
}

void ndef_init(ndef_t *message, uint8_t *buffer, uint32_t buffer_size)
{
    assert(message != NULL);
    assert(buffer != NULL);
    message->buffer.memory = buffer;
    message->buffer.memory_size = buffer_size;
    message->buffer.cursor = 0;
    message->record_count = 0;
}

int ndef_add_record(ndef_t *message, const uint8_t *type, uint8_t type_length, const uint8_t *id, uint8_t id_length, const uint8_t *payload, uint32_t payload_length, ndef_record_tnf_t tnf)
{
    ndef_record_desc_t record;

    assert(message != NULL);
    assert(payload_length > 0);
    assert(payload_length <= LONG_RECORD_PAYLOAD_LENGTH);

    if (message->record_count >= MAX_NDEF_RECORD_COUNT) {
        LOG_ERROR("NDEF record count exceeds maximum\n");
        return -1;
    }

    bool mb, me, cf, sr, il;

    /** 
     *  The ME flag can only be set on the last record of the NDEF message.
     *  The second to last record should have the ME flag cleared.
     */
    if (message->record_count >= 1) {
        message->records[message->record_count - 1].header[0] &= ~RECORD_ME_MASK;
        mb = false;
    }
    else {
        mb = true;
    }

    /* message end has to be true because records are only added at the end */
    me = true;

    /* chunking is not supported */
    cf = false;

    /* if the payload is short, set sr to true */
    if (payload_length <= SHORT_RECORD_PAYLOAD_LENGTH) {
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

    uint8_t header_byte = 0;
    header_byte |= mb ? RECORD_MB_MASK : 0;
    header_byte |= me ? RECORD_ME_MASK : 0;
    header_byte |= cf ? RECORD_CF_MASK : 0;
    header_byte |= sr ? RECORD_SR_MASK : 0;
    header_byte |= il ? RECORD_IL_MASK : 0;
    header_byte |= tnf;

    if (sr) {
        record.record_type = NDEF_SHORT_RECORD;
    }
    else {
        record.record_type = NDEF_LONG_RECORD;
    }

    record.header = ndef_write_to_buffer(message, &header_byte, 1);
    record.start = record.header;
    record.type_length = ndef_write_to_buffer(message, &type_length, 1);

    if (record.record_type == NDEF_SHORT_RECORD) {
        uint8_t payload_length_single_byte = (uint8_t)payload_length & 0xFF;
        record.payload_length = ndef_write_to_buffer(message, (uint8_t *)&payload_length_single_byte, SHORT_RECORD_PAYLOAD_LENGTH_SIZE);
    }
    else {
        record.payload_length = ndef_write_to_buffer(message, (uint8_t *)&payload_length, LONG_RECORD_PAYLOAD_LENGTH_SIZE);
    }

    if (id) {
        record.id_length = ndef_write_to_buffer(message, &id_length, 1);
    }

    record.type = ndef_write_to_buffer(message, (uint8_t *)type, type_length);

    if (id) {
        record.id = ndef_write_to_buffer(message, id, id_length);
    }

    if (payload != NULL) {
        record.payload = ndef_write_to_buffer(message, payload, payload_length);
    }
    else {
        /** 
         *  this needs to be done so the payload points to the correct position 
         *  for further writes
         */
        record.payload = &message->buffer.memory[message->buffer.cursor];

        /* the payload has to be written by the calling function */
        (void)payload;
    }

    message->records[message->record_count] = record;
    message->record_count += 1;

    return 0;
}

int ndef_remove_record(ndef_t *message)
{
    assert(message != NULL);

    if (message->record_count == 0) {
        LOG_ERROR("No records to remove");
        return -1;
    }

    /* for only one record we simply set the count and cursor to 0 */
    if (message->record_count == 1) {
        message->record_count = 0;
        message->buffer.cursor = 0;
        return 0;
    }

    /**
     * the new cursor position can be found by looking at the start pointer
     * of the last record and setting the cursor to that location 
     */
    ndef_record_desc_t *last_record = &message->records[message->record_count - 1];

    /** 
     * the new cursor position is the difference between the start of the first record
     * record and the start of the last record as the cursor counts the bytes
     */
    uint32_t new_cursor = last_record->start - message->records[0].start;
    message->buffer.cursor = new_cursor;
    message->record_count -= 1;

    /* the last record must now have ME to 1 */
    message->records[message->record_count - 1].header[0] |= RECORD_ME_MASK;

    return 0;
}

void ndef_clear(ndef_t *message)
{
    assert(message != NULL);

    message->record_count = 0;
    message->buffer.cursor = 0;
}

size_t ndef_calculate_record_size(uint8_t type_length, uint8_t id_length, uint32_t payload_length)
{
    size_t size = NDEF_RECORD_HEADER_SIZE + NDEF_RECORD_TYPE_LENGTH_SIZE + type_length;

    if (id_length > 0) {
        size += NDEF_RECORD_ID_LENGTH_SIZE + id_length;
    }

    if (payload_length <= SHORT_RECORD_PAYLOAD_LENGTH) {
        size += SHORT_RECORD_PAYLOAD_LENGTH_SIZE;
    }
    else {
        size += LONG_RECORD_PAYLOAD_LENGTH_SIZE;
    }

    size += payload_length;

    return size;
}
