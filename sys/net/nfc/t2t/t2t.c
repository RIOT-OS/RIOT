#include "net/nfc/t2t/t2t.h"
#include <stdio.h>
#include <string.h>
#include "log.h"

/**
 * @brief Sets dynamic lock bits and decreases data area.
 *
 * Sets the dynamic lock bits to 0x0 at the end of the data area as defined
 * for the default behavior in Type 2 Tag Specification 2.2.2
 *
 * @param tag the type 2 tag to be altered
 * @return 0 in case of success, negative number in case of problems
 */
static int create_default_dynamic_memory_layout(nfc_t2t_t *tag, bool read_only){
// calculate size of dynamic lock bytes and amnt of lock bits
    LOG_DEBUG("Creating default lock bytes\n");
    if(tag->memory_size <= NFC_T2T_STATIC_MEMORY_SIZE){
        LOG_ERROR("Tag size too small to add dynamic memory layout\n");
        return -1;
    }
    uint32_t dyn_lock_bytes;
    uint32_t dyn_lock_bits; // only needed if setting to 1 needed
    if(((tag->data_area_size - NFC_T2T_SIZE_STATIC_DATA_AREA) % 8) > 0){
        dyn_lock_bytes= ((tag->data_area_size - 48) / 64) + 1;
        dyn_lock_bits= ((tag->data_area_size - 48) / 8) + 1;  
    }else{
        dyn_lock_bytes = (tag->data_area_size - 48) / 64;
        dyn_lock_bits = (tag->data_area_size - 48) / 8;
    }
    // calculate position of dynamic lock bits -> new end of user usable data area
    uint32_t start_lock_bits = tag->data_area_size - 1 - dyn_lock_bytes;
    // write lock bits to data area
    for(uint32_t i = 0; i < dyn_lock_bytes; i++){
        if(read_only){
            tag->data_area_start[start_lock_bits + i] = 0xFF; //initiate to read_only
        }else{
            tag->data_area_start[start_lock_bits + i] = 0x00; //initiate to read_write
        } 
    }
    // in case last byte is just partially filled with lock bits
    if(read_only && dyn_lock_bits % 8 != 0){
        // only as many bits shall be set to 1 as there are lock bits - remaining bits to 0x0
        // e.g. 70 Byte data area -> 22 byte dyn -> ceil(22/8) = 3bit -> 1110 0000 --> 1111 1111 << (8-3=5) 
        tag->data_area_start[start_lock_bits + dyn_lock_bytes] = (uint8_t) 0xFF << (8-(dyn_lock_bits %8));
    }
    tag->data_area_size = tag->data_area_size - dyn_lock_bytes; //I think this is necessary
    tag->cc->memory_size = (uint8_t) (tag->data_area_size / 8); //TODO what if data_area % 8 != 0?
    tag->extra.default_lock_bits_set = true;
    tag->extra.default_lock_bits = dyn_lock_bits;
    tag->extra.default_lock_bytes = dyn_lock_bytes;
    tag->extra.custom_lock_bytes = 0;
    tag->extra.custom_reserved_bytes = 0;

    LOG_DEBUG("Wrote %ld lock bits in %ld lock bytes\n", dyn_lock_bits, dyn_lock_bytes);
    
    return 0;
}

/**
 * @brief Returns amount of bytes between data_area_cursor and end of data_area
 * 
 * @param tag 
 * @return uint32_t 
 */
static uint32_t free_space_in_data_area(nfc_t2t_t *tag){
    uint32_t free_space = (uint32_t) (tag->data_area_start + tag->data_area_size) - (uint32_t) tag->data_area_cursor;
    return free_space;
}

bool t2t_is_writeable(nfc_t2t_t *tag){
    // hypothetically should check whether static & dynamic lock bits (if existent)
    // and CC lock is set - simply checking CC lock is what NDEF detect does
    return (tag->cc->read_write_access == (uint8_t) 0x00);
}

int t2t_set_writeable(nfc_t2t_t *tag){
    tag->cc->read_write_access = NFC_T2T_CC_READ_WRITE;
    return 0;
}


int t2t_set_read_only(nfc_t2t_t *tag){
    tag->cc->read_write_access = NFC_T2T_CC_READ_ONLY;
    return 0;
}

t2t_sn_t t2t_create_default_sn(void){
    t2t_sn_t sn = NFC_T2T_4_BYTE_DEFAULT_UID;
    return sn;
}

static uint8_t * block_no_to_address(uint8_t block_no, nfc_t2t_t *tag){
    return (uint8_t*) &tag->memory[(tag->current_sector * NFC_T2T_BLOCKS_PER_SECTOR + block_no) * NFC_T2T_BLOCK_SIZE];
}

int t2t_create_uid(nfc_t2t_t *tag){
    t2t_sn_t *sn = &tag->sn;
    t2t_uid_t *uid = (t2t_uid_t*) &tag->memory[0];
    uid->uid[0] = sn->sn[0];
    uid->uid[1] = sn->sn[1];
    uid->uid[2] = sn->sn[2];
    if(sn->sn_length == NFC_ISO14443A_UID_SINGLE_SIZE){
        uid->uid[3] = sn->sn[3];
        uid->uid[4] = uid->uid[0] ^ uid->uid[1] ^ uid->uid[2] ^ uid->uid[3];
        uid->uid[5] = 0XFF;
        uid->uid[6] = 0XFF;
        uid->uid[7] = 0XFF;
        uid->uid[8] = 0XFF;
        uid->uid[9] = NFC_T2T_VERSION_1_1; //find a norm to verify this
        LOG_DEBUG("Wrote 4 Byte UID\n");
    }else if(sn->sn_length == NFC_ISO14443A_UID_DOUBLE_SIZE){
        uid->uid[3] = NFC_T2T_CASCADE_TAG_BYTE ^ uid->uid[0] ^ uid->uid[1] ^ uid->uid[2];
        uid->uid[4] = sn->sn[3];
        uid->uid[5] = sn->sn[4];
        uid->uid[6] = sn->sn[5];
        uid->uid[7] = sn->sn[6];
        uid->uid[8] = uid->uid[4]^ uid->uid[5] ^ uid->uid[6] ^ uid->uid[7];
        uid->uid[9] = NFC_T2T_VERSION_1_1; //find a norm to verify this
        LOG_DEBUG("Wrote 7 Byte UID\n");
    }else{
        uid->uid[3] = sn->sn[3];
        uid->uid[4] = sn->sn[4];
        uid->uid[5] = sn->sn[5];
        uid->uid[6] = sn->sn[6];
        uid->uid[7] = sn->sn[7];
        uid->uid[8] = sn->sn[8];
        uid->uid[9] = sn->sn[9];
        LOG_DEBUG("Wrote 10 Byte UID\n");
    }
    tag->uid = uid;
    return 0;
}


int t2t_create_static_lock_bytes(bool read_write, nfc_t2t_t *tag){
    t2t_static_lock_bytes_t *lb = (t2t_static_lock_bytes_t*) &tag->memory[NFC_T2T_SIZE_UID];
    if(read_write){
        lb->lock_byte1 = NFC_T2T_LOCK_BYTES_READ_WRITE;
        lb->lock_byte2 = NFC_T2T_LOCK_BYTES_READ_WRITE;
    }else{
        lb->lock_byte1 = NFC_T2T_LOCK_BYTES_READ_ONLY;
        lb->lock_byte2 = NFC_T2T_LOCK_BYTES_READ_ONLY;
    }
    tag->lb = lb;
    return 0;
}

int t2t_set_static_lock_bytes(t2t_static_lock_bytes_t * lock_bytes, nfc_t2t_t *tag){
    t2t_static_lock_bytes_t *lb = (t2t_static_lock_bytes_t*) &tag->memory[NFC_T2T_SIZE_UID];
    lb->lock_byte1 = lock_bytes->lock_byte1;
    lb->lock_byte2 = lock_bytes->lock_byte2;
    tag->lb = lb;
    return 0;
}

int t2t_create_cc(bool readable, bool writeable, uint32_t size_data_area, nfc_t2t_t *tag){
    t2t_cc_t *cc = (t2t_cc_t*) &tag->memory[NFC_T2T_SIZE_UID+NFC_T2T_SIZE_STATIC_LOCK_BYTES];
    cc->magic_number = NFC_T2T_CC_MAGIC_NUMBER;
    cc->version_number = NFC_T2T_VERSION_1_1;
    cc->memory_size = (uint8_t) size_data_area / 8; //TODO this doesn't always work
    if(readable && writeable){
        cc->read_write_access = (uint8_t) NFC_T2T_CC_READ_WRITE;
    }else{
        cc->read_write_access = (uint8_t) NFC_T2T_CC_READ_ONLY;
    }
    tag->cc = cc;
    return 0;
}

int t2t_set_cc(t2t_cc_t * cap_cont, nfc_t2t_t *tag){
    t2t_cc_t *cc = (t2t_cc_t*) &tag->memory[NFC_T2T_SIZE_UID+NFC_T2T_SIZE_STATIC_LOCK_BYTES];
    cc->magic_number = cap_cont->magic_number;
    cc->memory_size = cap_cont->memory_size;
    cc->read_write_access = cap_cont->read_write_access;
    cc->version_number = cap_cont->version_number;
    tag->cc = cc;
    return 0;
}

/**
 * @brief Gets bytes per page value for memory control and lock control TLVs
 * 
 * Formula for byte address is byte_address = page_address * 2^(bytes/page) + offset <br>
 * byte_address is <= size of tag memory, page_address, bytes_page and offset are
 * stored in a single nibble each, so they must all be <= 0xF.
 * bytes/page is calculated as bytes_page >= (memory_size - 15) / 15
 * @param mem_size total tag memory size
 * @return uint8_t exponent for bytes/page
 */
static uint8_t calculate_bytes_per_page(uint32_t mem_size){
    for(uint8_t i = 3; i <= 0xF; i++){
        if ((uint32_t) (2 << (i-1)) >= (uint32_t) ((mem_size-15) / 15)){
            return i;
        }
    }
    //should not happen
    return -1;
}


int t2t_create_type_2_tag(nfc_t2t_t *tag, t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                            uint32_t memory_size, uint8_t *memory)
{
    LOG_DEBUG("Creating type 2 tag\n");
    tag->memory = memory;
    tag->memory_size = memory_size;
    memset(tag->memory, 0x00, tag->memory_size);
    if(memory_size > 64){ 
        tag->dynamic_layout = true;
        tag->data_area_size = memory_size-(NFC_T2T_SIZE_UID+NFC_T2T_SIZE_STATIC_LOCK_BYTES+NFC_T2T_SIZE_CC);
        tag->extra.bytes_per_page = calculate_bytes_per_page(memory_size);
        LOG_DEBUG("Using dynamic memory layout\n");
    }else{
        tag->dynamic_layout = false;
        tag->data_area_size = NFC_T2T_SIZE_STATIC_DATA_AREA;
        LOG_DEBUG("Using static memory layout\n");
    }
    tag->data_area_start = memory + NFC_T2T_START_STATIC_DATA_AREA;
    tag->data_area_cursor = tag->data_area_start;
    //initialize to sector 0
    tag->current_sector = 0;
    
    //create/use sn
    if(!sn){
        tag->sn = t2t_create_default_sn();
    }else{
        tag->sn = *sn;
    }
    //create uid
    int error = 0;
    error = t2t_create_uid(tag);
    if(error) return error;
    //create lock_bytes
    if(!lb){
        error = t2t_create_static_lock_bytes(true, tag);
    }else{
        error = t2t_set_static_lock_bytes(lb, tag);
    }
    if(error) return error;
    //create cc
    if(!cc){
        error = t2t_create_cc(true, true, tag->data_area_size, tag);
    }else{
        error = t2t_set_cc(cc, tag);
    }
    if(error) return error;
    if(tag->dynamic_layout){
        create_default_dynamic_memory_layout(tag, false); //sets dynamic lock bits at end of data area and lowers data_area_size
    }

    LOG_DEBUG("Created tag with %ld bytes data area\n", tag->data_area_size);

    return 0;
}

int t2t_create_type_2_tag_with_ndef(nfc_t2t_t *tag, t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                                uint32_t memory_size, uint8_t *memory, ndef_t *msg){
    int error = 0;
    error = t2t_create_type_2_tag(tag, sn, cc, lb, memory_size, memory);
    if(error != 0) return error;
    error = t2t_add_ndef_msg(tag, msg);

    return error;

}

int t2t_create_empty_default_tag(nfc_t2t_t *tag, uint32_t memory_size, uint8_t *memory){
    int error = 0;
    error = t2t_create_type_2_tag(tag, NULL, NULL, NULL, memory_size, memory);
    if(error) return error;
    error = t2t_add_empty_ndef_tlv(tag);
    return error;
}

int t2t_create_default_tag_with_ndef(nfc_t2t_t *tag, uint32_t memory_size, uint8_t *memory, ndef_t *msg){
    return t2t_create_type_2_tag_with_ndef(tag, NULL, NULL, NULL, memory_size, memory, msg);
}

int t2t_create_tag_from_given_memory(nfc_t2t_t *tag, uint32_t memory_size, uint8_t *memory, uint8_t uid_size){
    LOG_DEBUG("Creating type 2 tag from given memory portion\n");
    int error = 0;
    if (memory == NULL){
        LOG_ERROR("Invalid pointer to memory region\n");
        return -1;
    }
    if(memory_size < NFC_T2T_STATIC_MEMORY_SIZE){
        LOG_ERROR("Smallest possible memory size is 64 byte, given is %ld\n", memory_size);
        return -1; //debatable - technically small NDEF messages could fit in less than 64 byte
    }
    tag->memory = memory;
    tag->memory_size = memory_size;
    if(memory_size > NFC_T2T_STATIC_MEMORY_SIZE){ 
        tag->dynamic_layout = true;
    }else{
        tag->dynamic_layout = false;
    }
    tag->current_sector = 0;
    tag->data_area_start = memory + NFC_T2T_START_STATIC_DATA_AREA;
    tag->data_area_cursor = tag->data_area_start;
    tag->data_area_size = tag->memory_size - (NFC_ISO14443A_UID_TRIPLE_SIZE + NFC_T2T_SIZE_STATIC_LOCK_BYTES + NFC_T2T_SIZE_CC);
    tag->uid = (t2t_uid_t*) (tag->memory);
    tag->lb = (t2t_static_lock_bytes_t*) (tag->memory + NFC_T2T_SIZE_UID);
    tag->cc = (t2t_cc_t*) (tag->memory + NFC_T2T_SIZE_UID + NFC_T2T_SIZE_STATIC_LOCK_BYTES);
    if(uid_size == NFC_ISO14443A_UID_SINGLE_SIZE
            || uid_size == NFC_ISO14443A_UID_DOUBLE_SIZE 
            || uid_size == NFC_ISO14443A_UID_TRIPLE_SIZE){
        tag->sn.sn_length = uid_size;
    }else{
        LOG_ERROR("Invalid uid size given, defaulting to 10 byte UID\n");
        tag->sn.sn_length = NFC_ISO14443A_UID_TRIPLE_SIZE; //default to maximum
    }
    memcpy(tag->sn.sn, tag->uid, tag->sn.sn_length);

    LOG_DEBUG("Created type 2 tag from memory portion\n");
    
    return error;
}

int t2t_handle_read(nfc_t2t_t *tag, uint8_t block_no, uint8_t *buf){
    uint8_t *block_address = block_no_to_address(block_no, tag);
    if((uint32_t) block_address >= ((uint32_t) tag->memory + tag->memory_size)){
        LOG_DEBUG("Block number too large, outside of valid memory region\n");
        return -1;
    }else if(((uint32_t) block_address + 16) > ((uint32_t) tag->memory + tag->memory_size)){
        LOG_DEBUG("End of region outside of tag memory, filling up with %#x\n", NFC_TLV_TYPE_TERMINATOR);
        memset(buf, NFC_TLV_TYPE_TERMINATOR, NFC_T2T_READ_RETURN_BYTES);
        memcpy(buf, block_address, (((uint32_t) tag->memory + tag->memory_size) - (uint32_t) block_address));

    }else{
        LOG_DEBUG("Reading 4 blocks beginning at block %d\n", block_no);
        memcpy(buf, block_address, NFC_T2T_READ_RETURN_BYTES);
    }
    
    return 0;
}

int t2t_handle_write(nfc_t2t_t *tag, uint8_t block_no, uint8_t const *buf){
    if(!t2t_is_writeable(tag)){
        LOG_ERROR("Tag is in read only state, aborting write\n");
        return -1;
    }
    uint8_t *block_address = block_no_to_address(block_no, tag);
    printf("address: %p\n",block_address);
    if((uint32_t) block_address >= ((uint32_t) tag->memory + tag->memory_size)){
        LOG_ERROR("Block number too high, outside of valid memory region, aborting write\n");
        return -1;
    }
    if(tag->current_sector == 0x0){
        if(block_no == 3){ //write to CC block causes bitwise OR instead of overwriting
            tag->cc->magic_number   |= buf[0];
            tag->cc->version_number |= buf[1];
            tag->cc->memory_size    |= buf[2];
            tag->cc->read_write_access |= buf[3];
        }else if(block_no == 2){//same should go for lock bytes, also no overwrite of UID
            tag->lb->lock_byte1 |= buf[3];
            tag->lb->lock_byte2 |= buf[4];
        }else if(block_no < 2){ //5.2 does not list uid to be overwritten
            LOG_ERROR("Can't overwrite UID, aborting write\n");
            return -1; //debatable if error or just ignored ok
        }else{
            LOG_ERROR("Writing content to tag memory\n");
            memcpy(block_address, buf, NFC_T2T_BLOCK_SIZE);
        }
    }else{
        LOG_ERROR("Writing content to tag memory\n");
        memcpy(block_address, buf, NFC_T2T_BLOCK_SIZE);
    }
    
    return 0;
    
}

int t2t_handle_sector_select(nfc_t2t_t *tag, uint8_t sector){
    if(sector > (tag->memory_size /NFC_T2T_BLOCKS_PER_SECTOR)){
        LOG_ERROR("Sector number too high\n");
        return -1;
    }
    tag->current_sector = sector;
    return 0;
}

int t2t_create_null_tlv(nfc_t2t_t *tag){
    if((uint32_t)tag->data_area_cursor < ((uint32_t)tag->data_area_start+tag->data_area_size)){
        tag->data_area_cursor[0] = NFC_TLV_TYPE_NULL_TLV;
        tag->data_area_cursor++;
        return 0;
    }else{
        LOG_ERROR("Not enough space in tag to create NULL TLV\n");
        return -1;
    }
    
}

int t2t_create_terminator_tlv(nfc_t2t_t *tag){
    if((uint32_t)tag->data_area_cursor < ((uint32_t)tag->data_area_start+tag->data_area_size)){
        tag->data_area_cursor[0] = NFC_TLV_TYPE_TERMINATOR;
        tag->data_area_cursor++;
        return 0;
    }else{
        LOG_ERROR("Not enough space in tag to create Terminator TLV\n");
        return -1;
    }
}

int t2t_create_memory_control_tlv(nfc_t2t_t *tag, uint8_t * data, uint32_t size){
    LOG_DEBUG("Creating memory control TLV\n");
    if(size > tag->data_area_size){
        LOG_ERROR("Size of reserved memory too large for tag area, aborting\n");
        return -1;
    }
    if(tag->dynamic_layout == false){
        LOG_ERROR("Tag has no dynamic memory layout, aborting\n");
        return -2;
    }
    tag->data_area_cursor[0] = NFC_TLV_TYPE_MEMORY_CTRL;
    tag->data_area_cursor[1] = NFC_TLV_SIZE_TYPE_MEMORY_CTRL_TLV;
    uint8_t *start_address;
    if(tag->extra.custom_lock_bytes != 0){
        start_address = (tag->memory + tag->memory_size) - tag->extra.custom_reserved_bytes - tag->extra.custom_lock_bytes - size;
    }else{
        start_address = (tag->memory + tag->memory_size) - tag->extra.custom_reserved_bytes - tag->extra.default_lock_bytes - size;
    }
    uint32_t byte_address = (uint32_t) start_address - (uint32_t)tag->memory;
    uint8_t offset = byte_address % (2 << (tag->extra.bytes_per_page-1));
    uint8_t page_address = byte_address / (2 << (tag->extra.bytes_per_page-1));
    //printf("start_address: %d, byte address: %d, page_address: %d, offset: %d, bytes/page: %d\n"
    //    ,start_address, byte_address, page_address, offset, tag->extra.bytes_per_page);
    uint8_t b1 = 0x00 | page_address;
    b1 = (b1 << 4) | offset; 
    uint8_t b2 = size;
    uint8_t b3 = 0x00 | tag->extra.bytes_per_page;
    //printf("%#4x %#4x %#4x\n",b1, b2, b3);
    tag->data_area_cursor[2] = b1;
    tag->data_area_cursor[3] = b2;
    tag->data_area_cursor[4] = b3;
    tag->data_area_cursor += 5;
    tag->data_area_size -= size;
    tag->cc->memory_size = (uint8_t) (tag->data_area_size / 8); //TODO not necessarily dividable by 8
    tag->extra.custom_reserved_bytes += size;
    memcpy(start_address, data, size);

    LOG_DEBUG("Wrote %ld byte of reserved memory information to end of tag memory\n", size);

    return 0;
}

static int t2t_test_and_remove_terminator_tlv(nfc_t2t_t *tag){
    if(tag->data_area_cursor <= tag->data_area_start){
        LOG_DEBUG("No terminator TLV found\n");
        return 0;
    }
    uint8_t * before = tag->data_area_cursor -1;
    if (before[0] == NFC_TLV_TYPE_TERMINATOR){
        before[0] = 0x00;
        tag->data_area_cursor--;
    }
    return 0;
}

int write_tlv_length(uint8_t *buf, uint16_t length){
    if(length > 0xFF){ //==> length 0xFF and to be interpreted as 2 bytes
        LOG_DEBUG("Writing three byte NDEF length TLV\n");
        buf[0] = 0xFF;
        buf[1] = (length >> 8) & 0x00FF; //most significant Byte - left 8 bit
        buf[2] = (length & 0x00FF); //lsB - right 8 bit
        return 3;
    }else{
        LOG_DEBUG("Writing one byte NDEF length TLV\n");
        buf[0] = (uint8_t) length;
        return 1;
    }
}

int t2t_create_ndef_tlv(nfc_t2t_t *tag, uint16_t length){
    uint8_t pos = 0;
    tag->data_area_cursor[pos] = NFC_TLV_TYPE_NDEF_MSG;
    pos++;
    pos += write_tlv_length(&tag->data_area_cursor[pos], length);
    tag->data_area_cursor += pos;
    return pos; //either 2 or 4
}


int t2t_add_ndef_msg(nfc_t2t_t *tag, ndef_t const *msg){

    uint32_t msg_length = msg->buffer.cursor;
    uint32_t tlv_header_size = 1;
    if(msg_length >= 0xFF){
        tlv_header_size = 3;
    }
    if(free_space_in_data_area(tag) < (msg_length + tlv_header_size)){
        LOG_ERROR("Not enough space in data area for NDEF message\n");
        return -1;
    }
    // check for already existing terminator tlv
    t2t_test_and_remove_terminator_tlv(tag);
    //write msg into tag mem
    t2t_create_ndef_tlv(tag, msg_length);
    memcpy(tag->data_area_cursor, msg->buffer.memory,msg_length);
    tag->data_area_cursor += msg_length;
    //add terminator
    t2t_create_terminator_tlv(tag);
    return 0;
}

int t2t_add_empty_ndef_tlv(nfc_t2t_t *tag){
    int error = 0;
    t2t_test_and_remove_terminator_tlv(tag);
    error = t2t_create_ndef_tlv(tag, 0);
    if(error) return error;
    error = t2t_create_terminator_tlv(tag);
    if(error) return error;
    return 0;
}

void t2t_dump_tag_memory(nfc_t2t_t *tag){
    for(uint32_t i=0; i < tag->memory_size; i++){
        if(tag->memory[i] >= 65 && tag->memory[i] <= 122){
            printf("%c\t", tag->memory[i]);
        }else{
            printf("%#04x\t", tag->memory[i]);
        }        
        if(((i+1)%4) == 0){
            printf("\n");
        }
        
    }
    
}

int t2t_clear_data_area(nfc_t2t_t *tag){
    LOG_DEBUG("Setting data area of type 2 tag to 0x00\n");
    memset(tag->data_area_start, 0x00, tag->data_area_size);
    tag->data_area_cursor = tag->data_area_start;
    return 0;
}

int t2t_clear_mem(nfc_t2t_t *tag){
    LOG_DEBUG("Setting complete memory of type 2 tag to 0x00\n");
    tag->uid = NULL;
    tag->lb = NULL;
    tag->cc = NULL;
    tag->data_area_start = NULL;
    tag->data_area_cursor = NULL;
    tag->data_area_size = 0;
    memset(tag->memory, 0x00, tag->memory_size);
    return 0;
}

uint32_t t2t_get_size(nfc_t2t_t *tag){
    return tag->memory_size;
}
