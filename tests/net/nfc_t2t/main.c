#include "net/nfc/t2t/t2t.h"

#include <stdint.h>
#include <stdio.h>

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

static int test_t2t_static_mem(void){
    printf("Start t2t static memory test\n");
    uint8_t tag_mem[NFC_T2T_STATIC_MEMORY_SIZE];
    nfc_t2t_t tag;
    int error = 0;
    error = t2t_create_type_2_tag(&tag, NULL, NULL, NULL, sizeof(tag_mem), tag_mem);
    if(error){
        printf("Error while creating the tag\n");
        return error;
    }else{
        printf("Created Tag with static memory layout\n");
    }
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);

    uint8_t ndef_mem[100];
    ndef_t ndef_msg;
    ndef_init(&ndef_msg, ndef_mem, sizeof(ndef_mem));
    ndef_add_text_record(&ndef_msg, "Hola", 4, "de", 2, UTF8);
    t2t_add_ndef_msg(&tag, &ndef_msg);
    t2t_add_ndef_msg(&tag, &ndef_msg);
    t2t_add_ndef_msg(&tag, &ndef_msg);
    t2t_add_ndef_msg(&tag, &ndef_msg);
    error = t2t_add_ndef_msg(&tag, &ndef_msg);
    if(error){
        printf("Last message successfully not written\n");
    }
    uint8_t read_buf[16];
    uint8_t write_buf[] = {'T', 'E', 'S', 'T'};
    
    t2t_handle_read(&tag, 0, read_buf);
    for(int i=0;i<16; i++){
        printf("%#4x ", read_buf[i]);
    }
    printf("\n");
    
    t2t_handle_read(&tag, 15, read_buf);
    for(int i=0;i<16; i++){
        printf("%#4x ", read_buf[i]);
    }
    printf("\n");

    error = t2t_handle_read(&tag, 16, read_buf);
    if(error){
        printf("Can't read out of bounds tag\n");
    }
    
    t2t_handle_write(&tag, 15, write_buf);
    error = t2t_handle_write(&tag, 16, write_buf);
    if(error){
        printf("Last write successfully aborted\n");
    }
    
    
    t2t_dump_tag_memory(&tag);
    return 0;

}

static int test_t2t_dynamic_mem(void){
    printf("Start t2t dynamic memory test\n");
    size_t tag_size = 1200; //2 sectors
    uint8_t tag_mem[tag_size];
    nfc_t2t_t tag;
    int error = 0;
    error = t2t_create_type_2_tag(&tag, NULL, NULL, NULL, sizeof(tag_mem), tag_mem);
    if(error){
        printf("Error while creating the tag\n");
        return error;
    }else{
        printf("Created Tag with dynamic memory layout\n");
    }
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    uint8_t reserved_buf[] = {'R', 'E', 'S', 'E', 'R', 'V', 'E', 'D'};
    t2t_create_memory_control_tlv(&tag, reserved_buf, sizeof(reserved_buf));
    
    uint8_t ndef_mem[100];
    ndef_t ndef_msg;
    ndef_init(&ndef_msg, ndef_mem, sizeof(ndef_mem));
    ndef_add_text_record(&ndef_msg, "Lorem ipsum dolor sit amet, consectetur adipiscing elit", 55, "de", 2, UTF8);
    t2t_add_ndef_msg(&tag, &ndef_msg);
    
    uint8_t write_buffer[] = {'S', 'E', 'C', '1'};
    t2t_handle_write(&tag,0xFF,write_buffer);
    t2t_handle_sector_select(&tag, 0x1);
    write_buffer[3] = '2';
    t2t_handle_write(&tag,0x00,write_buffer);
    
    t2t_dump_tag_memory(&tag);
    return 0;

}

static int test_t2t_with_predefined_mem(void){
    printf("Start t2t predefined mem test\n");
    nfc_t2t_t tag;
    t2t_create_tag_from_given_memory(&tag, NFC_T2T_DEFAULT_MEM_SIZE, predefined_t2t, 4);
    uint8_t buffer[16];
    t2t_handle_read(&tag, 0, buffer);
    
    for (uint8_t i=0; i<16; i++){
       printf("%#02x ", buffer[i]);
       if((i+1) % 4 == 0){
        printf("\n");
       }
    }
    buffer[0] = 'T';
    buffer[1] = 'E';
    buffer[2] = 'S';
    buffer[3] = 'T';
    t2t_handle_write(&tag, 5, buffer);

    t2t_dump_tag_memory(&tag);

    return 0;

}


static bool test_t2t(void) {
    test_t2t_static_mem();
    test_t2t_dynamic_mem();
    test_t2t_with_predefined_mem();
    return true;
}

int main(void){
    puts("Starting NFC tests");
    test_t2t();
    puts("Ending NFC tests");
    return 0;
}
