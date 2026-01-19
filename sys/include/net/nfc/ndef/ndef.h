/**
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    ndef NDEF
 * @ingroup     net
 * @brief       Serialization for the NFC Data Exchange Format
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for the NFC Data Exchange Format
 * @see         [NFC Forum NFC Data Exchange Format (NDEF) Technical Specification 1.0]
 *              (https://archive.org/details/nfcforum-ts-ndef-1.0)
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Maximum payload length for a short NDEF record
 */
#define NDEF_RECORD_SHORT_PAYLOAD_LENGTH      255

/**
 * @brief Maximum payload length for a long NDEF record
 */
#define NDEF_RECORD_LONG_PAYLOAD_LENGTH       65535

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

#ifndef CONFIG_NDEF_MAX_RECORD_COUNT
/**
 * @brief Specifies how many NDEF records can be stored in one NDEF message
 */
#  define CONFIG_NDEF_MAX_RECORD_COUNT 16
#endif

/**
 * @brief NDEF buffer struct
 */
typedef struct {
    /**
     * @brief Pointer to the start of the memory containing the NDEF message
     */
    uint8_t *memory;

    /**
     * @brief Pointer to the end of the memory
     *
     * @note The end is the last byte of the memory buffer
     */
    uint8_t *memory_end;

    /**
     * @brief Pointer to the next free memory location
     */
    uint8_t *cursor;
} ndef_buffer_t;

/**
 * @brief NDEF record type
 */
typedef enum {
    NDEF_RECORD_SHORT = 1,
    NDEF_RECORD_LONG = 4
} ndef_record_type_t;

/**
 * @brief Describes an NDEF record
 *
 * @note This descriptor struct contains only pointers to the correct
 * positions in the NDEF message.
 */
typedef struct {
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

/**
 * @brief Enum for the Type Name Format (TNF) of an NDEF record
 *
 * @note The TNF specifies the structure of the type field of the record.
 */
typedef enum {
    /** The value indicates that there is no type or payload. */
    NDEF_TNF_EMPTY = 0x00,
    /** NFC Forum well-known type [NFC RTD]. */
    NDEF_TNF_WELL_KNOWN = 0x01,
    /** Media-type as defined in RFC 2046 [RFC 2046]. */
    NDEF_TNF_MEDIA_TYPE = 0x02,
    /** Absolute URI as defined in RFC 3986 [RFC 3986]. */
    NDEF_TNF_ABSOLUTE_URI = 0x03,
    /** NFC Forum external type [NFC RTD]. */
    NDEF_TNF_EXTERNAL_TYPE = 0x04,
    /** The value indicates that there is no type associated with this
     *  record.
     */
    NDEF_TNF_UNKNOWN_TYPE = 0x05,
    /** The value is used for the record chunks used in chunked payload. */
    NDEF_TNF_UNCHANGED = 0x06,
    /** The value is reserved for future use. */
    NDEF_TNF_RESERVED = 0x07,
} ndef_record_tnf_t;

/**
 * @brief NDEF struct
 *
 * @note It can contain up to @ref CONFIG_NDEF_MAX_RECORD_COUNT records.
 */
typedef struct {
    /**
     * @brief Buffer containing the data
     */
    ndef_buffer_t buffer;

    /**
     * @brief Array of NDEF record descriptors
     */
    uint8_t *records[CONFIG_NDEF_MAX_RECORD_COUNT];

    /**
     * @brief Number of records in the NDEF message
     */
    uint32_t record_count;
} ndef_t;

/**
 * @brief Pretty prints the NDEF message descriptors in human-readable format.
 *
 * @param[in] ndef_record_descriptors   Pointer to the array of record descriptors
 * @param[in] record_count 				Number of records in the array
 */
void ndef_pretty_print(const ndef_record_desc_t *ndef_record_descriptors, size_t record_count);

/**
 * @brief Initializes the given NDEF message with the given buffer and buffer size.
 *
 * @note The buffer needs to be allocated by the user and has to stay valid for the entire lifetime
 * of the NDEF message struct.
 *
 * @param[out] 	ndef 		    Message to initialize
 * @param[in] 	buffer 			Empty buffer that is used by the NDEF message
 * @param[in] 	buffer_size 	Buffer size
 */
void ndef_init(ndef_t *ndef, uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief Adds an NDEF record header to the end of the NDEF message
 *
 * @param[in,out] 	ndef     		NDEF message
 * @param[in] 		type 			Type of the record
 * @param[in] 		type_length 	Length of the type
 * @param[in] 		id 				ID of the record
 * @param[in] 		id_length 		Length of the ID
 * @param[in] 		payload_length 	Length of the payload
 * @param[in] 		tnf 			Type Name Format of the record
 * @return 			0 if successful
 * @return 			error otherwise
 */
int ndef_record_header_add(ndef_t *ndef, const uint8_t *type, uint8_t type_length,
    const uint8_t *id, uint8_t id_length, uint32_t payload_length, ndef_record_tnf_t tnf);

/**
 * @brief Returns the current size of the NDEF message
 *
 * @param[in] ndef NDEF message
 * @return size_t Size of the message
 */
static inline size_t ndef_get_size(const ndef_t *ndef);

/**
 * @brief Removes the last record from the NDEF message
 *
 * @param[in,out] ndef Message to remove the last record from
 * @return 0 if successful, error otherwise
 */
int ndef_record_remove_last(ndef_t *ndef);

/**
 * @brief Removes all records from the NDEF message
 *
 * @param[in,out] ndef Message to clear
 */
void ndef_clear(ndef_t *ndef);

/**
 * @brief Parses the NDEF message and returns the record descriptors
 *
 * @note The array must be large enough to hold all record descriptors.
 * @param[in]   ndef                    NDEF message to parse
 * @param[out]  record_descriptors      Array of record descriptors
 * @param[in]   record_descriptors_size Size of the array
 *
 * @retval 0    on success
 * @retval -1   if the message can't be parsed
 */
int ndef_parse(const ndef_t *ndef, ndef_record_desc_t *record_descriptors,
               size_t record_descriptors_size);

/**
 * @brief Parses a single NDEF record
 *
 * @param[in]   ndef_record         NDEF record to parse
 * @param[out]  record_descriptor   Record descriptor to fill
 *
 */
void ndef_record_parse(const uint8_t *ndef_record, ndef_record_desc_t *record_descriptor);

/**
 * @brief NDEF text encoding
 */
typedef enum {
    UTF8 = 0,
    UTF16 = 1
} ndef_text_encoding_t;

/**
 * @brief Adds an NDEF text record to the NDEF message
 *
 * @note The text record is added to the end of the NDEF message.
 *
 * @param[in,out]   ndef                NDEF message
 * @param[in]       text                Text to add
 * @param[in]       text_length         Length of the text
 * @param[in]       lang_code           Language code
 * @param[in]       lang_code_length    Length of the language code
 * @param[in]       encoding            Encoding of the text
 * @retval 0                            on success
 * @retval -1                           if writing the payload failed
 */
int ndef_record_add_text(ndef_t *ndef, const char *text, uint32_t text_length,
                         const char *lang_code, uint8_t lang_code_length,
                         ndef_text_encoding_t encoding);

/**
 * @brief Calculates the size of an NDEF text record
 *
 * @param[in] text_length       Length of the text
 * @param[in] lang_code_length  Length of the language code
 * @return Size of the text record
 */
size_t ndef_record_calculate_text_size(uint32_t text_length, uint8_t lang_code_length);

/**
 * @brief NDEF URI types
 */
typedef enum {
    NDEF_URI_NA = 0x00,
    NDEF_URI_HTTP_WWW = 0x01,
    NDEF_URI_HTTPS_WWW = 0x02,
    NDEF_URI_HTTP = 0x03,
    NDEF_URI_HTTPS = 0x04,
    NDEF_URI_TEL = 0x05,
    NDEF_URI_MAILTO = 0x06,
    NDEF_URI_FTP_ANONYMOUS = 0x07,
    NDEF_URI_FTP_FTP = 0x08,
    NDEF_URI_FTPS = 0x09,
    NDEF_URI_SFTP = 0x0A,
    NDEF_URI_SMB = 0x0B,
    NDEF_URI_NFS = 0x0C,
    NDEF_URI_FTP = 0x0D,
    NDEF_URI_DAV = 0x0E,
    NDEF_URI_NEWS = 0x0F,
    NDEF_URI_TELNET = 0x10,
    NDEF_URI_IMAP = 0x11,
    NDEF_URI_RTSP = 0x12,
    NDEF_URI_URN = 0x13,
    NDEF_URI_POP = 0x14,
    NDEF_URI_SIP = 0x15,
    NDEF_URI_SIPS = 0x16,
    NDEF_URI_TFTP = 0x17,
    NDEF_URI_BTSPP = 0x18,
    NDEF_URI_BTL2CAP = 0x19,
    NDEF_URI_BTGOEP = 0x1A,
    NDEF_URI_TCPOBEX = 0x1B,
    NDEF_URI_IRDAOBEX = 0x1C,
    NDEF_URI_FILE = 0x1D,
    NDEF_URI_URN_EPC_ID = 0x1E,
    NDEF_URI_URN_EPC_TAG = 0x1F,
    NDEF_URI_URN_EPC_PAT = 0x20,
    NDEF_URI_URN_EPC_RAW = 0x21,
    NDEF_URI_URN_EPC = 0x22,
    NDEF_URI_URN_NFC = 0x23
} ndef_uri_identifier_code_t;

/**
 * @brief Adds an NDEF URI record to the NDEF message
 *
 * @param[in,out]   ndef                NDEF message
 * @param[in]       identifier_code     URI identifier code
 * @param[in]       uri                 URI
 * @param[in]       uri_length          Length of the URI
 * @retval 0                            on success
 * @retval -1                           if writing the payload failed
 */
int ndef_record_uri_add(ndef_t *ndef, ndef_uri_identifier_code_t identifier_code,
                        const char *uri, uint32_t uri_length);

/**
 * @brief
 *
 * @param ndef                  NDEF message
 * @param mime_type             MIME type
 * @param mime_type_length      Length of the MIME type
 * @param mime_payload          MIME payload
 * @param mime_payload_length   Length of the MIME payload
 * @retval 0                    on success
 * @retval -1                   if writing the payload failed
 */
int ndef_record_mime_add(ndef_t *ndef, const char *mime_type, uint32_t mime_type_length,
                         const uint8_t *mime_payload, uint32_t mime_payload_length);

/**
 * @brief Calculates the size of an NDEF mime record
 *
 * @param[in] mime_type_length      Length of the MIME type
 * @param[in] mime_payload_length   Length of the MIME payload
 * @return Size of the MIME record
 */
size_t ndef_record_calculate_mime_size(uint32_t mime_type_length, uint32_t mime_payload_length);

#ifdef __cplusplus
}
#endif

/** @} */
