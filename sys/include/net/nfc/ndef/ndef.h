#ifndef NDEF_H
#define NDEF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define NDEF_RECORD_HEADER_SIZE 1
#define NDEF_RECORD_TYPE_LENGTH_SIZE 1
#define NDEF_RECORD_ID_LENGTH_SIZE 1
#define NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE 1
#define NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE 4

#define MAX_NDEF_RECORD_COUNT 10
// #define NDEF_MEMORY_SIZE 1024



typedef struct {
	/* contains the NDEF memory */
	uint8_t *memory;

	/* size of the memory that can't be exceeded by the cursor */
	uint32_t memory_size;

	/* index to the last byte + 1 of @ref memory, so it basically is the length
	of data */
	uint32_t cursor;
} ndef_buffer_t;

typedef enum {
	NDEF_SHORT_RECORD = 1,
	NDEF_LONG_RECORD = 4
} ndef_record_type_t;

typedef struct {
	uint8_t *start;

    uint8_t *header;

    uint8_t *type_length;
    uint8_t *type;

    uint8_t *id_length;
    uint8_t *id;

	ndef_record_type_t record_type;
    uint8_t *payload_length;
    uint8_t *payload;
} ndef_record_desc_t;




typedef enum {
	/** The value indicates that there is no type or payload. */
	TNF_EMPTY         = 0x00,
	/** NFC Forum well-known type [NFC RTD]. */
	TNF_WELL_KNOWN    = 0x01,
	/** Media-type as defined in RFC 2046 [RFC 2046]. */
	TNF_MEDIA_TYPE    = 0x02,
	/** Absolute URI as defined in RFC 3986 [RFC 3986]. */
	TNF_ABSOLUTE_URI  = 0x03,
	/** NFC Forum external type [NFC RTD]. */
	TNF_EXTERNAL_TYPE = 0x04,
	/** The value indicates that there is no type associated with this
	 *  record.
	 */
	TNF_UNKNOWN_TYPE  = 0x05,
	/** The value is used for the record chunks used in chunked payload. */
	TNF_UNCHANGED     = 0x06,
	/** The value is reserved for future use. */
	TNF_RESERVED      = 0x07,
} ndef_record_tnf_t;

typedef enum {
	NDEF_FIRST_RECORD  = 0x80, /** First record. */
	NDEF_MIDDLE_RECORD = 0x00, /** Middle record. */
	NDEF_LAST_RECORD   = 0x40, /** Last record. */
	NDEF_LONE_RECORD   = 0xC0  /** Only one record in the message. */
} ndef_record_location_t;

typedef struct {
	ndef_buffer_t buffer;
    ndef_record_desc_t records[MAX_NDEF_RECORD_COUNT];
    uint32_t record_count;
} ndef_t;


/**
 * @brief Pretty prints the NDEF message in human readable format.
 *
 * @param[in] ndef NDEF message to print
 */
void ndef_pretty_print(ndef_t const *ndef);

/**
 * @brief Get the NDEF message size
 *
 * @param[in] ndef NDEF messgae
 * @return size_t
 */
size_t ndef_get_size(ndef_t const *ndef);

/**
 * @brief Writes the data buffer to the given NDEF message.
 *
 * @note This writes the data of the given buffer and
 * shifts the cursor of the NDEF buffer by the length of the data.
 *
 * @param[in,out] ndef The NDEF message that gets the data written to its memory
 * @param[in] data Data to write
 * @param[in] data_length Length of the data
 * @return The pointer to the memory cursor before the write operation, NULL if the data is too long
 * for the remaining buffer size.
 */
uint8_t* ndef_write_to_buffer(ndef_t* ndef, uint8_t const *data, uint32_t data_length);

/**
 * @brief Initializes the given NDEF message with the given buffer and buffer size.
 *
 * @note The buffer needs to be allocated by the user and has to stay valid for the entire lifetime
 * of the NDEF message struct.
 *
 * @param[out] message Message to initilaize
 * @param[in] buffer Empty buffer that is used by the NDEF message
 * @param[in] buffer_size Buffer size
 */
void ndef_init(ndef_t *message, uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief Adds an NDEF record to the end of the NDEF message
 *
 * @param[in,out] 	message
 * @param[in] 		type
 * @param[in] 		type_length
 * @param[in] 		id
 * @param[in] 		id_length
 * @param[in] 		payload
 * @param[in] 		payload_length
 * @param[in] 		tnf
 * @return int 0 if successful, error otherwise
 */
int ndef_add_record(ndef_t *message, const uint8_t *type, uint8_t type_length, const uint8_t *id, uint8_t id_length, uint8_t const *payload, uint32_t payload_length, ndef_record_tnf_t tnf);

/**
 * @brief Returns the size of the NDEF message
 *
 * @param[in] message NDEF message
 * @return size_t Size of the message
 */
size_t ndef_get_size(const ndef_t *message);

/**
 * @brief Removes the last record from the NDEF message
 *
 * @param message
 * @return 0 if successful, error otherwise
 */
int ndef_remove_record(ndef_t *message);

/**
 * @brief Removes all records from the NDEF message
 *
 * @param message Message to clear
 */
void ndef_clear(ndef_t *message);

#include "net/nfc/ndef/ndef_text_payload.h"
#include "net/nfc/ndef/ndef_uri_payload.h"

#endif