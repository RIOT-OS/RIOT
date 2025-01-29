#include "net/nfc/t2t/t2t.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// static mem example from T2T Operation Document
static uint8_t predefined_t2t[] = {
    0x00, 0x01, 0x02, 0x03, // UID 1
    0x04, 0x05, 0x06, 0x07, // UID 2
    0x08, 0x09, 0x00, 0x00, // UID 3, LB
    0xE1, 0x10, 0x06, 0x00, // CC
    0x03, 0x00, 0xFE, 0x00, // Empty NDEF message and terminator
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
#define T2T_STAT_MEM_SIZE NFC_T2T_STATIC_MEMORY_SIZE
#define T2T_DYN_MEM_SIZE 1200
#define NDEF_BUF_SIZE 100
#define LARGE_NDEF_BUF_SIZE 300 //to enforce two byte tlv size
#define LARGE_MESSAGE_BUF_SIZE 0xFF
static uint8_t t2t_static_mem[T2T_STAT_MEM_SIZE];
static uint8_t t2t_dynamic_mem[T2T_DYN_MEM_SIZE];
static uint8_t ndef_buf[NDEF_BUF_SIZE];
static uint8_t large_ndef_buf[LARGE_NDEF_BUF_SIZE];
static uint8_t large_message_buf[LARGE_MESSAGE_BUF_SIZE];

/**
 * @brief Tests type 2 tag with static memory layout
 * 
 * @return int 
 */
static int test_t2t_static_mem(void){
    printf("\nStart t2t static memory test\n");
    nfc_t2t_t tag;
    int error = 0;
    error = t2t_create_type_2_tag(&tag, NULL, NULL, NULL, T2T_STAT_MEM_SIZE, t2t_static_mem);
    if(error){
        printf("FAIL: Error while creating the tag\n");
        return error;
    }
    t2t_create_null_tlv(&tag); //1 byte
    t2t_create_null_tlv(&tag); //1 byte

    ndef_t ndef_msg;
    ndef_init(&ndef_msg, ndef_buf, NDEF_BUF_SIZE);
    ndef_add_text_record(&ndef_msg, "Hola", 4, "de", 2, UTF8); //13 byte
    int add_attempts = 0;

    while(error == 0){
        error = t2t_add_ndef_msg(&tag, &ndef_msg);
        add_attempts++;    
    }
    if(add_attempts != 4){
        printf("FAIL: Message should have been attempted to be added four times");
        return error;
    }
    printf("Added 13 byte NDEF message %d times. Tag contains %d / %d bytes\n", 
        add_attempts-1, (tag.data_area_cursor-tag.memory), T2T_STAT_MEM_SIZE);

    uint8_t read_buf[16];
    uint8_t write_buf[] = {'T', 'E', 'S', 'T'};
    
    t2t_handle_read(&tag, 0, read_buf);
    for(int i=0;i<16; i++){
        if(read_buf[i] != t2t_static_mem[i]){
            puts("FAIL: read data differs from memory content");
            printf("Read buf %d: %#4x \n", i, read_buf[i]);
            printf("Memory %d: %#4x \n", i, t2t_static_mem[i]);
            return -1;
        }
    }
    
    t2t_handle_read(&tag, 15, read_buf);
    for(int i=0;i<4; i++){
        int address = (15*4) + i;
        if(read_buf[i] != t2t_static_mem[address]){
            puts("FAIL: read data differs from memory content");
            printf("Read buf %d: %#4x \n", i, read_buf[i]);
            printf("Memory %d: %#4x \n", i, t2t_static_mem[address]);
        }
    }
    for(int i=4;i<16; i++){
        if(read_buf[i] != NFC_TLV_TYPE_TERMINATOR){
            puts("FAIL: read data ain't terminator byte");
            printf("Read buf %d: %#4x \n", i, read_buf[i]);
        }
    }

    error = t2t_handle_read(&tag, 16, read_buf);
    if(error){
        printf("SUCCESS: Can't read out of bounds tag\n");
    }
    
    t2t_handle_write(&tag, 15, write_buf);
    error = t2t_handle_write(&tag, 16, write_buf);
    if(error){
        printf("SUCCESS: Can't write to out of bounds block\n");
    }
    
    t2t_dump_tag_memory(&tag);
    return 0;

}

/**
 * @brief Tests type 2 tag with dynamic memory layout
 * 
 * @return int 
 */

static int test_t2t_dynamic_mem(void){
    printf("\nStart t2t dynamic memory test\n");
    nfc_t2t_t tag;
    int error = 0;
    error = t2t_create_type_2_tag(&tag, NULL, NULL, NULL, T2T_DYN_MEM_SIZE, t2t_dynamic_mem);
    if(error){
        printf("FAIL: Error while creating the tag\n");
        return error;
    }
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    uint8_t reserved_buf[] = {'R', 'E', 'S', 'E', 'R', 'V', 'E', 'D'};
    error = t2t_create_memory_control_tlv(&tag, reserved_buf, sizeof(reserved_buf));
    if(error){
        printf("FAIL: Could not create memory control block with reserved data\n");
        return error;
    }
    
    ndef_t ndef_msg;
    ndef_init(&ndef_msg, ndef_buf, NDEF_BUF_SIZE);
    error = ndef_add_text_record(&ndef_msg, "Lorem ipsum dolor sit amet, consectetur adipiscing elit", 55, "de", 2, UTF8);
    if(error){
        puts("FAIL: Could not add small NDEF text record");
    }
    error = t2t_add_ndef_msg(&tag, &ndef_msg);
    if(error){
        printf("FAIL: Could not add small ndef message to tag\n");
        return error;
    }

    ndef_init(&ndef_msg, large_ndef_buf, LARGE_NDEF_BUF_SIZE);
    memset(large_message_buf, 'A', 0xFF); 
    error = ndef_add_text_record(&ndef_msg, (char*)large_message_buf, 0xFF, "de", 2, UTF8);
    if(error){
        puts("FAIL: Could not add large NDEF text record");
    }
    error = t2t_add_ndef_msg(&tag, &ndef_msg);
    if(error){
        printf("FAIL: Could not add large ndef message to tag\n");
        return error;
    }
    
    uint8_t write_buffer[] = {'S', 'E', 'C', '1'};
    error = t2t_handle_write(&tag,0xFF,write_buffer);
    if(error){
        printf("FAIL: Writing to end of sector 1 failed\n");
        return error;
    }
    error = t2t_handle_sector_select(&tag, 0x1);
    if(error){
        printf("FAIL: Sector select failed\n");
        return error;
    }
    write_buffer[3] = '2';
    error = t2t_handle_write(&tag,0x00,write_buffer);
    if(error){
        printf("FAIL: Write to sector 2 failed\n");
        return error;
    }
    
    t2t_dump_tag_memory(&tag);
    return 0;

}

/**
 * @brief Tests use of predefined memory
 * 
 * @return int 
 */
static int test_t2t_with_predefined_mem(void){
    printf("\nStart t2t predefined mem test\n");
    int error = 0;
    nfc_t2t_t tag;
    error = t2t_create_tag_from_given_memory(&tag, NFC_T2T_DEFAULT_MEM_SIZE, predefined_t2t, 4);
    if(error){
        printf("FAIL: Could not create tag from given memory\n");
        return error;
    }
    uint8_t read_buf[16];
    error = t2t_handle_read(&tag, 0, read_buf);
    if(error){
        printf("Could not read from tag\n");
        return error;
    }
    
    for(int i=0;i<16; i++){
        if(read_buf[i] != tag.memory[i]){
            puts("FAIL: read data differs from memory content");
            printf("Read buf %d: %#4x \n", i, read_buf[i]);
            printf("Memory %d: %#4x \n", i, tag.memory[i]);
            return -1;
        }
    }
    read_buf[0] = 'T';
    read_buf[1] = 'E';
    read_buf[2] = 'S';
    read_buf[3] = 'T';
    t2t_handle_write(&tag, 5, read_buf);
    for(int i=0;i<4; i++){
        int address = (5*4) + i;
        if(read_buf[i] != tag.memory[address]){
            puts("FAIL: written data differs from memory content");
            printf("Read buf %d: %#4x \n", i, read_buf[i]);
            printf("Memory %d: %#4x \n", i, tag.memory[address]);
            return -1;
        }
    }

    t2t_dump_tag_memory(&tag);

    return 0;

}
/**
 * @brief Run all T2T tests, abort if one fails.
 * 
 * @return int 
 */
int main(void){
    int num_tests = 3;
    int (*p[]) (void) = {test_t2t_static_mem, test_t2t_dynamic_mem, test_t2t_with_predefined_mem};
    puts("Starting NFC tests");
    int error = 0;
    for(int i=0; i<num_tests;i++){
        error = p[i]();
        if(error){
            puts("FAILED!");
            return -1;
        }
    }
    puts("Ending NFC tests");
    return 0;
}
