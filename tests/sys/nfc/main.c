#include "net/nfc/nfct/nfct.h"
#include "net/nfc/ndef/ndef.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "log.h"
#include "ztimer.h"


#define BUFFER_SIZE 1024

static uint8_t ndef_mem[BUFFER_SIZE];
static uint8_t t2t_mem[BUFFER_SIZE];

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

static void print_ndef_as_hex(ndef_t const *message) {
    printf("NDEF message size: %lu\n", message->buffer.cursor);
    for (uint32_t i = 0; i < message->buffer.cursor; ++i) {
        if (i % 4 == 0 && i != 0) {
            printf("\n");
        }
        printf("0x%02x ", message->buffer.memory[i]);
    }
    printf("\n\n");
}

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

static bool test_ndef_text_record(void) {
    puts("NDEF text record test");
    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    print_ndef_as_hex(&message);
    ndef_pretty_print(&message);
    return true;
}

static bool test_ndef_uri_record(void) {
    puts("NDEF URI record test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);

    ndef_add_uri_record(&message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);
    print_ndef_as_hex(&message);

    return true;
}

static bool test_two_ndef_records(void) {
    puts("Two NDEF records test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_text_record(&message, "Hej Verden", 10, "da", 2, UTF8);
    ndef_pretty_print(&message);
    return true;
}

static bool test_ndef_remove(void) {
    puts("NDEF remove test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_text_record(&message, "Hej Verden", 10, "da", 2, UTF8);
    puts("Before removal:");
    ndef_pretty_print(&message);
    ndef_remove_record(&message);

    puts("After removal:");
    ndef_pretty_print(&message);

    return true;
}


static bool test_nfct(void) {
    puts("Starting NFC T2T test");
    ndef_t ndef_message;

    ndef_init(&ndef_message, ndef_mem, 1024);
    ndef_add_text_record(&ndef_message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_uri_record(&ndef_message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);

    nfc_t2t_t t2t;
    t2t_create_type_2_tag(&t2t, NULL, NULL, NULL, NFC_T2T_STATIC_MEMORY_SIZE, t2t_mem);
    nfct_create_type_2_tag(&DEFAULT_T2T_EMULATOR_DEV, &t2t, &ndef_message, TYPE_2_TAG);
    /* sleep for 10 seconds, then disable the tag */
    ztimer_sleep(ZTIMER_SEC, 10);
    nfct_delete_type_2_tag(&DEFAULT_T2T_EMULATOR_DEV);
    return true;
}

int main(void){
    puts("Starting NFC tests");
    test_t2t();
    test_ndef_text_record();
    test_ndef_uri_record();

    test_two_ndef_records();
    test_ndef_remove();
    test_nfct();
    puts("Ending NFC tests");
    return 0;
}
