/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys/net/nfc
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for NFC Forum Type 2 Tags.
 *
 * Typedefs and function definitions for NFC Forum Type 2 Tags.
 *
 * @author      Max Stolze <max_karl.stolze@mailbox.tu-dresden.de>
 *
 */

#ifndef NET_NFC_TYPE_2_TAG_H
#define NET_NFC_TYPE_2_TAG_H

#include <stdint.h>
#include <stdbool.h>
#include "tlv.h"
#include "net/nfc/ndef/ndef.h"


// ISO-1443 specific defines
#define NFC_ISO14443A_UID_SINGLE_SIZE 4
#define	NFC_ISO14443A_UID_DOUBLE_SIZE 7
#define NFC_ISO14443A_UID_TRIPLE_SIZE 10
#define NFC_ISO14443A_UID_MAX_LEN NFC_ISO14443A_UID_TRIPLE_SIZE

// NFC specific constants
#define NFC_T2T_CASCADE_TAG_BYTE 0x88
#define NFC_T2T_VERSION_1_1 0x11
#define NFC_T2T_CC_MAGIC_NUMBER 0xE1
#define NFC_T2T_CC_READ_WRITE 0x00
#define NFC_T2T_CC_READ_ONLY 0x0F
#define NFC_T2T_LOCK_BYTES_READ_ONLY 0xFF
#define NFC_T2T_LOCK_BYTES_READ_WRITE 0x00

//sizes
#define NFC_T2T_BLOCK_SIZE 4
#define NFC_T2T_BLOCKS_PER_SECTOR 256
#define NFC_T2T_STATIC_MEMORY_SIZE 64
#define NFC_T2T_STATIC_MEMORY_DATA_AREA_SIZE 64
#define NFC_T2T_DEFAULT_MEM_SIZE NFC_T2T_STATIC_MEMORY_SIZE
#define NFC_T2T_SIZE_UID 10
#define NFC_T2T_SIZE_STATIC_LOCK_BYTES 2
#define NFC_T2T_SIZE_STATIC_DATA_AREA 48
#define NFC_T2T_START_STATIC_DATA_AREA (NFC_T2T_SIZE_UID + NFC_T2T_SIZE_STATIC_LOCK_BYTES + NFC_T2T_SIZE_CC)
#define NFC_T2T_SIZE_CC 4
#define NFC_T2T_READ_RETURN_BYTES 16
#define NFC_T2T_MEM_SIZE_LARGEST_POSSIBLE_TAG 2088 //0xff data area + 32 lock bytes + reserved
#define NFC_T2T_SIZE_RESERVED_AREA 16 //Size of the first four blocks

//selfmade defaults - TODO check how a reader interprets that
#define NFC_T2T_4_BYTE_DEFAULT_UID {NFC_ISO14443A_UID_SINGLE_SIZE, {0x09, 0x01, 0x02, 0x03}} //ISO-14443-3 6.4.4 Table 10
#define NFC_T2T_10_BYTE_DEFAULT_UID {NFC_ISO14443A_UID_TRIPLE_SIZE, {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09}} //ISO-14443-3 6.4.4 Table 10

// typedefs

/**
 * @brief Struct for UID in memory
 * 
 * UIDs and meaning defined in ISO-14443-3 6.4.4
 */
typedef struct {
    uint8_t uid[NFC_ISO14443A_UID_MAX_LEN];
} t2t_uid_t;

/**
 * @brief Struct for serial number
 * 
 * Serial number is not a part of the tag structure. This is used to hand
 * over the UID and length for it.
 * 
 */
typedef struct {
    uint8_t sn_length;
    uint8_t sn[NFC_ISO14443A_UID_MAX_LEN];
} t2t_sn_t;

/**
 * @brief Struct for Lock Bytes
 * 
 * T2T Lock bytes for static memory layout.
 */
typedef struct {
    uint8_t lock_byte1;
    uint8_t lock_byte2;
} t2t_static_lock_bytes_t;

/**
 * @brief Capability Container struct.
 * 
 * Capability Container struct holds four uint8_t for each byte of the CC
 * as defined in T2T Op 6.1. Magic number, T2T version, data area size 
 * divided by 8 and read_write settings.
 */
typedef struct {
    uint8_t magic_number; //EH1 for NDEF
    uint8_t version_number; // msb bit major, lsb bit minor -> 0x11
    uint8_t memory_size; // data area size / 8
    uint8_t read_write_access; // 4 bit read, 4 bit write,
} t2t_cc_t;

/**
 * @brief Information about dynamic memory parts.
 * 
 * t2t_dynamic_t holds information specific for dynamic memory layout.
 * Lock and Memory bytes information is used by t2t library to calculate 
 * positions in tag memory.
 * 
 */
typedef struct{
    bool default_lock_bits_set;
    uint8_t *start_default_lock_bits;
    uint32_t default_lock_bits;
    uint32_t default_lock_bytes;
    uint32_t custom_lock_bytes;
    uint32_t custom_reserved_bytes;
    uint8_t bytes_per_page;
}t2t_dynamic_t;

/**
 * @brief Type 2 Tag container.
 * 
 * nfc_t2t_t holds meta information about the type 2 tag memory.
 * 
 */
typedef struct{
    uint8_t *memory;
    uint32_t memory_size;
    size_t usable_memory;
    bool dynamic_layout;
    uint8_t current_sector;
    t2t_sn_t sn; //this is not part of the t2t mem layout - reflected as uid
    t2t_uid_t *uid;
    t2t_static_lock_bytes_t *lb;
    t2t_cc_t *cc;
    uint32_t data_area_size;
    uint8_t *data_area_start;
    uint8_t *data_area_cursor;
    t2t_dynamic_t extra;
} nfc_t2t_t;

//functions
/** 
 * @brief Creates an empty tag using given memory and creates an empty ndef message
*/
int t2t_create_empty_default_tag(nfc_t2t_t *tag, uint32_t memory_size, uint8_t *memory);
//creates an empty tag with requested size and an empty ndef message

/**
 * @brief Creates tag using given memory and ndef message
 * 
 * @param tag 
 * @param memory_size 
 * @param memory 
 * @param msg 
 * @return int negative in case of error
 */
int t2t_create_default_tag_with_ndef(nfc_t2t_t *tag, uint32_t memory_size, uint8_t *memory, ndef_t *msg);

/**
 * @brief Create a Type 2 Tag object with given parameters
 * 
 * @param tag 
 * @param sn 
 * @param cc 
 * @param lb 
 * @param memory_size 
 * @param memory 
 * @return int 
 */
int t2t_create_type_2_tag(nfc_t2t_t *tag, t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                                uint32_t memory_size, uint8_t *memory);

/**
 * @brief Create a Type 2 Tag with given ndef message
 * 
 * @param tag 
 * @param sn 
 * @param cc 
 * @param lb 
 * @param memory_size 
 * @param memory 
 * @param msg 
 * @return int 
 */
int t2t_create_type_2_tag_with_ndef(nfc_t2t_t *tag, t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                                uint32_t memory_size, uint8_t *memory, ndef_t *msg);

/**
 * @brief Creates a tag with the given memory block without adding or removing any data.
 * 
 * This function takes the given memory as is and does not modify its content. 
 * The content is not parsed and cannot be extended using t2t functions other then
 * t2t_handle_write(). The uid_size is needed to determine how many of the 10 possible bytes
 * are the actual uid. If set to a value other then NFC_ISO14443A_UID_SINGLE, 
 * DOUBLE or TRIPLE_SIZE it will default to NFC_ISO14443A_UID_TRIPLE_SIZE.
 * 
 * @param tag 
 * @param memory_size 
 * @param memory 
 * @param uid_size
 * @return int 
 */
int t2t_create_tag_from_given_memory(nfc_t2t_t *tag, uint32_t memory_size, uint8_t *memory, uint8_t uid_size);

//operations
/**
 * @brief Handles NDEF Write command
 * 
 * Overwrites data in block (4 Bytes) identified by block_no with data from buf.
 * For CC block existing data is bitwise or'd with given data.
 * For lockbytes data is also or'd and remainder of block is not overwritten.
 * @param tag 
 * @param block_no block to write to
 * @param buf buffer with data to write in block
 * @return int negative in case of error
 */
int t2t_handle_write(nfc_t2t_t *tag, uint8_t block_no, uint8_t const *buf);

/**
 * @brief Handles NDEF Read command
 * 
 * Reads 16 byte (4 blocks) beginning from block address of block_no and
 * stores values in buf
 * 
 * @param tag 
 * @param block_no block to start reading from
 * @param buf buffer to write 16 bytes to
 * @return int negative in case of error
 */
int t2t_handle_read(nfc_t2t_t *tag, uint8_t block_no, uint8_t *buf);
//int t2t_handle_sector_select(nfc_t2t_t *tag, uint8_t *buf); //TODO - this way or as above?
/**
 * @brief Handles NDEF Sector Select Command
 * 
 * @param tag 
 * @param sector 
 * @return int 
 */
int t2t_handle_sector_select(nfc_t2t_t *tag, uint8_t sector);

/**
 * @brief Writes UID to correct position in memory pointed to by tag.
 * 
 * @param tag 
 * @return int 
 */
int t2t_create_uid(nfc_t2t_t *tag);

/**
 * @brief Creates and returns a default serial number for the tag of length 4.
 * 
 * @return t2t_sn_t 
 */
t2t_sn_t t2t_create_default_sn(void); //TODO - only accept predefined? offer to roll one?

/**
 * @brief Creates capacity container bytes in memory pointed to by tag.
 * 
 * Creates default values for capacity container given the arguments. Magic value is
 * set to NDEF message content.
 * @param readable 
 * @param writeable 
 * @param size_data_area 
 * @param tag 
 * @return int 
 */
int t2t_create_cc(bool readable, bool writeable, uint32_t size_data_area, nfc_t2t_t *tag);
/**
 * @brief Sets capacity container bytes in memory pointed to by tag to values taken 
 * from cap_cont.
 * 
 * Does not create any defaults but simply copies values from given cap_cont.
 * 
 * @param cap_cont 
 * @param tag 
 * @return int 
 */
int t2t_set_cc(t2t_cc_t * cap_cont, nfc_t2t_t *tag);

/**
 * @brief Sets Lock Bytes in memory pointed to by tag.
 * 
 * Sets all lock bytes to read only or read write setting depending on read_write
 * argument.
 * 
 * @param read_write 
 * @param tag 
 * @return int 
 */
int t2t_create_static_lock_bytes(bool read_write, nfc_t2t_t *tag);

/**
 * @brief Sets static lock bytes in memory pointed to by tag to values from lock_bytes.
 * 
 * Does not create any defaults but just sets the values found in lock_bytes.
 * 
 * @param lock_bytes 
 * @param tag 
 * @return int 
 */
int t2t_set_static_lock_bytes(t2t_static_lock_bytes_t * lock_bytes, nfc_t2t_t *tag);


//content 
/**
 * @brief Adds the ndef message to the current end of the tags data area
 * 
 * @param tag 
 * @param msg 
 * @return int negative in case of error
 */
int t2t_add_ndef_msg(nfc_t2t_t *tag, ndef_t const *msg);
/**
 * @brief Adds an empty NDEF TLV to the current end of the tags data area.
 * 
 * @param tag 
 * @return int 
 */
int t2t_add_empty_ndef_tlv(nfc_t2t_t *tag);

/**
 * @brief Creates a memory control tlv to a dynamic memory layout tag and adds the
 * given data to the reserved bytes.
 * 
 * The TLV block is created at the current end of the data area. 
 * The data for the reserved bytes is read from data and written to the end of
 * the memory area. The data area in the CC block is shrunk trough this operation.
 * @param tag 
 * @param data 
 * @param size 
 * @return int 
 */
int t2t_create_memory_control_tlv(nfc_t2t_t *tag, uint8_t * data, uint32_t size);

/**
 * @brief Adds a NULL TLV to the current end of the data area.
 * 
 * @param tag 
 * @return int 
 */
int t2t_create_null_tlv(nfc_t2t_t *tag);

/**
 * @brief Adds a Terminator TLV to the current end of the data area.
 * 
 * @param tag 
 * @return int 
 */
int t2t_create_terminator_tlv(nfc_t2t_t *tag);

/**
 * @brief Reserves the requested amount of space for a ndef message
 * 
 * This method can be used if a NDEF message shall be written directly
 * into the tag memory. It will create an TLV header for a message of the 
 * requested size and move the cursor behind the expected message 
 * and will create a terminator TLV.
 * A pointer to the beginning of the reserved area (i.e. the V section
 * of the TLV block) will be returned.
 * 
 * @param tag the type 2 tag in which the message shall be written
 * @param msg_size the size of the ndef message
 * @return uint8_t* pointer to the memory or NULL if not enough space
 */
uint8_t* t2t_reserve_ndef_space(nfc_t2t_t *tag, size_t msg_size);

//misc
uint32_t t2t_get_size(nfc_t2t_t *tag); //returns size of tag
bool t2t_is_writeable(nfc_t2t_t *tag); // true if tag is writeable
int t2t_set_writeable(nfc_t2t_t *tag); // sets tag writable in CC
int t2t_set_read_only(nfc_t2t_t *tag); // sets tag read only in CC
int t2t_clear_mem(nfc_t2t_t *tag); // clears complete tag memory
int t2t_clear_data_area(nfc_t2t_t *tag); //clears data area of tag
void t2t_dump_tag_memory(nfc_t2t_t *tag); //dumps the tags memory to stdout

#endif
