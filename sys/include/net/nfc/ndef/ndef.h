/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef NDEF_H
#define NDEF_H

/**
 * @ingroup     sys/net/nfc
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for the NFC Data Exchange Format
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief The size of the NDEF record header
 */
#define NDEF_RECORD_HEADER_SIZE               1

/**
 * @brief The size of the type length field
 */
#define NDEF_RECORD_TYPE_LENGTH_SIZE          1

/**
 * @brief The size of the id length field
 */
#define NDEF_RECORD_ID_LENGTH_SIZE            1

/**
 * @brief The size of the payload length field for short records
 */
#define NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE 1

/**
 * @brief The size of the payload length field for long records
 *
 * @note In the specification this is just referred to as normal length.
 */
#define NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE  4

#ifndef MAX_NDEF_RECORD_COUNT
/**
 * @brief Specifies how many NDEF records can be stored in one NDEF message
 */
#  define MAX_NDEF_RECORD_COUNT 16
#endif

/**
 * @brief NDEF buffer struct
 */
typedef struct {
    /**
     * @brief Stores the data of the NDEF message
     */
    uint8_t *memory;

    /**
     * @brief Size of the memory buffer that can't be exceeded
     */
    uint32_t memory_size;

    /**
     * @brief Index to the last byte + 1 of the memory buffer
	 *
	 * @note It is identical to the length of the data
     */
    uint32_t cursor;
} ndef_buffer_t;

/**
 * @brief NDEF record type
 */
typedef enum {
    NDEF_SHORT_RECORD = 1,
    NDEF_LONG_RECORD = 4
} ndef_record_type_t;

/**
 * @brief Describes a NDEF record
 *
 * @note This descritor struct contains only pointers to the correct
 * positions in the NDEF message.
 */
typedef struct {
    /**
	 * @brief Points to the start of the record in the NDEF message
	 */
    uint8_t *start;

    /**
	 * @brief Points to the header of the record
	 */
    uint8_t *header;

    /**
	 * @brief Points to the type length field
	 */
    uint8_t *type_length;

    /**
	 * @brief Points to the type field
	 */
    uint8_t *type;

    /**
	 * @brief Points to the ID length field
	 */
    uint8_t *id_length;

    /**
	 * @brief Points to the ID field
	 */
    uint8_t *id;

    /**
	 * @brief Record type of this record
	 */
    ndef_record_type_t record_type;

    /**
	 * @brief Points to the payload length field
	 */
    uint8_t *payload_length;

    /**
	 * @brief Points to the payload field
	 */
    uint8_t *payload;
} ndef_record_desc_t;

typedef enum {
    /** The value indicates that there is no type or payload. */
    TNF_EMPTY = 0x00,
    /** NFC Forum well-known type [NFC RTD]. */
    TNF_WELL_KNOWN = 0x01,
    /** Media-type as defined in RFC 2046 [RFC 2046]. */
    TNF_MEDIA_TYPE = 0x02,
    /** Absolute URI as defined in RFC 3986 [RFC 3986]. */
    TNF_ABSOLUTE_URI = 0x03,
    /** NFC Forum external type [NFC RTD]. */
    TNF_EXTERNAL_TYPE = 0x04,
    /** The value indicates that there is no type associated with this
	 *  record.
	 */
    TNF_UNKNOWN_TYPE = 0x05,
    /** The value is used for the record chunks used in chunked payload. */
    TNF_UNCHANGED = 0x06,
    /** The value is reserved for future use. */
    TNF_RESERVED = 0x07,
} ndef_record_tnf_t;

typedef enum {
    NDEF_FIRST_RECORD = 0x80,  /** First record. */
    NDEF_MIDDLE_RECORD = 0x00, /** Middle record. */
    NDEF_LAST_RECORD = 0x40,   /** Last record. */
    NDEF_LONE_RECORD = 0xC0    /** Only one record in the message. */
} ndef_record_location_t;

/**
 * @brief NDEF struct
 *
 * @note It can contain up to @ref MAX_NDEF_RECORD_COUNT records.
 */
typedef struct {
    /**
	 * @brief Buffer containing the data
	 */
    ndef_buffer_t buffer;

    /**
	 * @brief Array of NDEF record descriptors
	 */
    ndef_record_desc_t records[MAX_NDEF_RECORD_COUNT];

    /**
	 * @brief Number of records in the NDEF message
	 */
    uint32_t record_count;
} ndef_t;

/**
 * @brief Pretty prints the NDEF message in human readable format.
 *
 * @param[in] ndef NDEF message to print
 */
void ndef_pretty_print(const ndef_t *ndef);

/**
 * @brief Writes the data buffer to the given NDEF message.
 *
 * @note This writes the data of the given buffer and
 * shifts the cursor of the NDEF buffer by the length of the data.
 *
 * @param[in,out]   ndef 		    The NDEF message that gets the data written to its memory
 * @param[in]       data 			Data to write
 * @param[in]       data_length 	Length of the data
 * @retval Pointer to the memory cursor before the write
 * @retval NULL if the data is too long
 */
uint8_t *ndef_write_to_buffer(ndef_t *ndef, const uint8_t *data, uint32_t data_length);

/**
 * @brief Initializes the given NDEF message with the given buffer and buffer size.
 *
 * @note The buffer needs to be allocated by the user and has to stay valid for the entire lifetime
 * of the NDEF message struct.
 *
 * @param[out] 	message 		Message to initilaize
 * @param[in] 	buffer 			Empty buffer that is used by the NDEF message
 * @param[in] 	buffer_size 	Buffer size
 */
void ndef_init(ndef_t *message, uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief Adds an NDEF record to the end of the NDEF message
 *
 * @param[in,out] 	message 		NDEF message
 * @param[in] 		type 			Type of the record
 * @param[in] 		type_length 	Length of the type
 * @param[in] 		id 				ID of the record
 * @param[in] 		id_length 		Length of the ID
 * @param[in] 		payload 		Payload of the record
 * @param[in] 		payload_length 	Length of the payload
 * @param[in] 		tnf 			Type Name Format of the record
 * @return 			0 if successful
 * @return 			error otherwise
 */
int ndef_add_record(ndef_t *message, const uint8_t *type, uint8_t type_length, const uint8_t *id, uint8_t id_length, const uint8_t *payload, uint32_t payload_length, ndef_record_tnf_t tnf);

/**
 * @brief Calculates the expected size of an NDEF record
 * 
 * @param[in] type_length       Length of type field
 * @param[in] id_length         Length of ID field
 * @param[in] payload_length    Length of payload field
 * @return size_t Size of the record
 */
size_t ndef_calculate_record_size(uint8_t type_length, uint8_t id_length, uint32_t payload_length);

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
 * @param[in,out] message Message to remove the record from
 * @return 0 if successful, error otherwise
 */
int ndef_remove_record(ndef_t *message);

/**
 * @brief Removes all records from the NDEF message
 *
 * @param[in,out] message Message to clear
 */
void ndef_clear(ndef_t *message);

#include "net/nfc/ndef/ndef_text_payload.h"
#include "net/nfc/ndef/ndef_uri_payload.h"

/** }@ */
#endif
