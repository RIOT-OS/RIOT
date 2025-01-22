#include "net/nfc/nfct/nfct.h"
#include "net/nfc/ndef/ndef.h"
#include "net/nfc/t2t/t2t.h"

#include "modules.h"
#include "log.h"
#include "string.h"

#if IS_USED(MODULE_NFC_T2T)
void nfct_create_type_2_tag(nfct_t2t_emulator_t* dev, void *tag, const nfct_tag_type_t tag_type) {
    if (tag_type == TYPE_2_TAG) {
        nfc_t2t_t *t2t = (nfc_t2t_t *) tag; 
        dev->init(t2t);
        dev->enable();
    }
}

void nfct_create_type_2_tag_with_ndef(nfct_t2t_emulator_t* dev, void *tag, const ndef_t* message, const nfct_tag_type_t tag_type) {
    if (tag_type == TYPE_2_TAG) {
        nfc_t2t_t *t2t = (nfc_t2t_t *) tag; 
        if (t2t_add_ndef_msg(t2t, message) != 0) {
            LOG_ERROR("Failed to add NDEF message to tag\n");
            return;
        }
        dev->init(t2t);
        dev->enable();
    }
}

/**
 * @brief Temporarily creates NDEF message to add text content to given tag
 */
static void add_text_message_to_tag(void *tag, 
const char *content, char* lang_code, ndef_text_encoding_t encoding){
    ndef_t message;
    size_t content_size = strlen(content);
    size_t lang_code_size = 0;
    if(lang_code != NULL){
        lang_code_size = strlen(lang_code);
    }else{
        lang_code = "en";
        lang_code_size = 2;
    }
     
    size_t message_size = content_size + lang_code_size + 7; // headers + large payload
    uint8_t buffer[message_size];

    ndef_init(&message, buffer, message_size);
    ndef_add_text_record(&message, content, content_size, lang_code, lang_code_size, encoding);
    t2t_add_ndef_msg(tag, &message);
    return;
}

void nfct_create_type_2_tag_with_text(nfct_t2t_emulator_t* dev, nfc_t2t_t *tag, 
uint8_t* tag_mem, size_t tag_mem_size, const char *content, char* lang_code, ndef_text_encoding_t encoding){
    if(content == NULL){
        return;
    }
    t2t_create_type_2_tag(tag, NULL, NULL, NULL,tag_mem_size, tag_mem);
    add_text_message_to_tag(tag, content, lang_code, encoding);
    dev->init(tag);
    dev->enable();
}

/**
 * @brief Temporarily creates NDEF message to add text URI to given tag
 */
static void add_uri_message_to_tag(void *tag,
ndef_uri_identifier_code_t identifier_code, const char *uri){
    ndef_t message;
    size_t uri_length = strlen(uri);
    size_t message_size = uri_length + 8; // headers + large payload + uri identfifier
    uint8_t buffer[message_size];

    ndef_init(&message, buffer, message_size);
    ndef_add_uri_record(&message, identifier_code, uri, uri_length);
    t2t_add_ndef_msg(tag, &message);
    return;
}

void nfct_create_type_2_tag_with_uri(nfct_t2t_emulator_t* dev, nfc_t2t_t *tag,
uint8_t* tag_mem, size_t tag_mem_size, ndef_uri_identifier_code_t identifier_code,
const char *uri){
    if(uri == NULL){
        return;
    }
    t2t_create_type_2_tag(tag, NULL, NULL, NULL,tag_mem_size, tag_mem);
    add_uri_message_to_tag(tag, identifier_code, uri);
    dev->init(tag);
    dev->enable();
}

void nfct_delete_type_2_tag(nfct_t2t_emulator_t* dev) {
    dev->disable();
    dev->uninit();
}
#endif
