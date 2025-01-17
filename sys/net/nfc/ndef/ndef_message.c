#include "net/nfc/ndef/ndef.h"
#include "net/nfc/ndef/ndef_text_payload.h"
#include "log.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#define SHORT_RECORD_PAYLOAD_LENGTH 255
#define LONG_RECORD_PAYLOAD_LENGTH 65535

#define SHORT_RECORD_PAYLOAD_LENGTH_SIZE 1
#define LONG_RECORD_PAYLOAD_LENGTH_SIZE 4

/* Structure of the header byte */
#define RECORD_MB_MASK 0x80
#define RECORD_ME_MASK 0x40
#define RECORD_CF_MASK 0x20
#define RECORD_SR_MASK 0x10
#define RECORD_IL_MASK 0x08
#define RECORD_TNF_MASK 0x07

/**
 * @brief Converts the payload length field to a uint32_t.
 * 
 * @note The payload length can be either 1 or 4 bytes depending on the SR flag
 * in the NDEF record header. Analogously, the @ref convert_uint32_to_payload_length
 * does the opposite.
 * 
 * @param[in] bytes
 * @param[in] size
 * 
 * @return Converted uint32_t

static uint32_t convert_payload_length_to_uint32(uint8_t const *bytes, uint32_t size) {
    assert(size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE || size == NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE);
    
    if (size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE) {
        return (uint32_t) bytes[0];
    } else {
        uint32_t return_value = 0;
        return (return_value | bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    }
    return 0;
}


 * @brief Converts a uint32_t to the payload length field.
 * 
 * @note Does the opposite of @ref convert_payload_length_to_uint32.
 * 
 * @param[out] bytes
 * @param[in] size
 * @param[in] uint32

static void convert_uint32_to_payload_length(uint8_t* bytes, uint32_t size, uint32_t uint32) {
    assert(size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE || size == NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE);
    if (size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE) {
        bytes[0] = uint32;
    } else {
        bytes[0] = (uint32 >> 24) & 0xFF;
        bytes[1] = (uint32 >> 16) & 0xFF;
        bytes[2] = (uint32 >> 8) & 0xFF;
        bytes[3] = uint32 & 0xFF;
    }
}
 */

size_t ndef_get_size(ndef_t const *ndef) {
    return ndef->buffer.cursor;
}

void ndef_pretty_print(ndef_t const *ndef) {
    printf("----------------\n");
    printf("NDEF Printing\n");
    printf("NDEF message records: %lu\n", ndef->record_count);
    printf("\n");
    for (size_t i = 0; i < (size_t) ndef->record_count; ++i) {
        ndef_record_desc_t const *record = &ndef->records[i];
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
        } else {
            payload_length = record->payload_length[0] << 24 | record->payload_length[1] << 16 | record->payload_length[2] << 8 | record->payload_length[3]; 
        }
        
        printf("Payload length: %lu\n", payload_length);
        printf("Payload: ");
        for (size_t j = 0; j < (size_t) payload_length; ++j) {
            printf("0x%02x ", record->payload[j]);
        }
        printf("\n\n");
    }
    printf("----------------");
    printf("\n");
}

uint8_t* ndef_write_to_buffer(ndef_t* ndef, uint8_t const *data, uint32_t data_length) {
    uint8_t *before_write = &(ndef->buffer.memory[ndef->buffer.cursor]); 
    if (ndef->buffer.cursor + data_length > ndef->buffer.memory_size) {
        LOG_ERROR("NDEF buffer overflow\n");
        return NULL;
    }
    memcpy(&ndef->buffer.memory[ndef->buffer.cursor], data, data_length);
    ndef->buffer.cursor += data_length;
    return before_write;
}

void ndef_init(ndef_t *message, uint8_t *buffer, uint32_t buffer_size) {
    assert(message != NULL);
    assert(buffer != NULL);
    message->buffer.memory = buffer;
    message->buffer.memory_size = buffer_size;
    message->buffer.cursor = 0;
    message->record_count = 0;
}

int ndef_add_record(ndef_t *message, uint8_t const *type, uint8_t type_length, uint8_t const *id, uint8_t id_length, uint8_t const *payload, uint32_t payload_length, ndef_record_tnf_t tnf) {
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
    } else {
        mb = true;
    }

    /* message end has to be true because records are only added at the end */
    me = true;
    
    /* chunking is not supported */
    cf = false;

    /* if the payload is short, set sr to true */
    if (payload_length <= SHORT_RECORD_PAYLOAD_LENGTH) {
        sr = true;
    } else {
        sr = false;
    }

    /* if an ID is present, set il to true */
    if (id) {
        il = true;
    } else {
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
    } else {
        record.record_type = NDEF_LONG_RECORD;
    }

    record.header = ndef_write_to_buffer(message, &header_byte, 1);
    record.start = record.header;
    record.type_length = ndef_write_to_buffer(message, &type_length, 1);

    if (record.record_type == NDEF_SHORT_RECORD) {
        uint8_t payload_length_single_byte = (uint8_t) payload_length & 0xFF;
        record.payload_length = ndef_write_to_buffer(message, (uint8_t*) &payload_length_single_byte, SHORT_RECORD_PAYLOAD_LENGTH_SIZE);
    } else {
        record.payload_length = ndef_write_to_buffer(message, (uint8_t*) &payload_length, LONG_RECORD_PAYLOAD_LENGTH_SIZE);
    }


    if (id) {
        record.id_length = ndef_write_to_buffer(message, &id_length, 1);
    }

    record.type = ndef_write_to_buffer(message, (uint8_t*) type, type_length);

    if (id) {
        record.id = ndef_write_to_buffer(message, id, id_length);
    }

    if (payload != NULL) {
        record.payload = ndef_write_to_buffer(message, payload, payload_length);
    } else {
        /** 
         *  this needs to be done so the payload points to the correct position 
         *  for further writes
         */
        record.payload = &message->buffer.memory[message->buffer.cursor];

        /* the payload has to be written by the calling function */
        (void) payload;
    }

    message->records[message->record_count] = record;
    message->record_count += 1;

    return 0;
}

int ndef_remove_record(ndef_t *message) {
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
    ndef_record_desc_t *last_record = &message->records[message->record_count-1];

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

void ndef_clear(ndef_t *message) {
    assert(message != NULL);

    message->record_count = 0;
    message->buffer.cursor = 0;
}

/*
int calculate_ndef_record_size(ndef_record_t const *record) {
    // we assume no ID and no chunking
    int record_size = 1; // TNF and flags

    record_size += 1; // Type length
    record_size += record->type_length; // Type

    if (record->payload_length <= SHORT_RECORD_PAYLOAD_LENGTH && record->payload_length > 0) {
        record_size += 1; // Payload length
    } else if (record->payload_length <= LONG_RECORD_PAYLOAD_LENGTH && record->payload_length > 0) {
        record_size += 4; // Payload length
    } else {
        record_size += 0;
    }
    record_size += record->payload_length; // Payload

    record_size += 0; // ID length
    record_size += 0; // ID

    return record_size;
}
*/

/*
int calculate_ndef_size(ndef_t const *message) {
    int ndef_size = 0;
    for (int i = 0; i < message->record_count; ++i) {
        ndef_record_t const *record = &message->records[i];
        int record_size = calculate_ndef_record_size(record);
        ndef_size += record_size;
    }
    return ndef_size;
}
*/

/*
int encode_ndef_record(ndef_record_t const *record, ndef_record_location_t record_location, uint8_t *buffer, uint32_t len) {
    uint8_t header_byte = 0;
    header_byte |= record->tnf;

    if (record->type_length <= 255) {
        header_byte |= RECORD_SR_MASK;
    }

    header_byte |= record_location;

    buffer[0] = header_byte;
    buffer[1] = record->type_length;
    int byte_position = 2;
    if (record->payload_length <= 255) {
        buffer[2] = record->payload_length;
        byte_position += 1;  
    } else {
        // MSB first
        buffer[byte_position]   = record->payload_length >> 24;
        buffer[byte_position+1] = record->payload_length >> 16;
        buffer[byte_position+2] = record->payload_length >> 8;
        buffer[byte_position+3] = record->payload_length;
        byte_position += 4; 
    }

    memcpy(&buffer[byte_position], record->type, record->type_length);

    return 0;
}
*/

/*
int encode_ndef_message(ndef_t const *message, uint8_t *buffer, uint32_t len) {
    int free_buffer_space = len;
    for (uint32_t i = 0; i < message->record_count; ++i) {
        ndef_record_location_t record_location;
        if (i == 0 && i == message->record_count - 1) {
            record_location = NDEF_LONE_RECORD;
        } else if (i == 0) {
            record_location = NDEF_FIRST_RECORD;
        } else if (i == message->record_count - 1) {
            record_location = NDEF_LAST_RECORD;
        } else {
            record_location = NDEF_MIDDLE_RECORD;
        }

        ndef_record_t *record = message->records[i];
        encode_ndef_record(record, record_location, buffer, len);
    }
    return 0;
}
*/

/*
int create_ndef_message_with_text(ndef_t *message, char const *text, uint32_t text_length) {
    ndef_t msg = {
        .record_count = 1,
        .records = {
            {
                .tnf = TNF_WELL_KNOWN,
                .type_length = 1,
                .type = {0x54}, // T
                .id_length = 0,
                .id = 0,
                .payload_length = text_length,
                .payload = text
            }
        }
    };
}
*/