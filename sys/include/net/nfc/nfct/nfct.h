#ifndef NFCT_H
#define NFCT_H

#include "net/nfc/ndef/ndef.h"
#include "net/nfc/t2t/t2t.h"
#include "modules.h"

/**
 * @brief NFCT tag type
 */
typedef enum {
    TYPE_2_TAG,
    TYPE_4_TAG
} nfct_tag_type_t;

#if IS_USED(MODULE_NFCT_CUSTOM_EMULATOR)
    /* use your own emulator, otherwise a fitting emulator header is included */
#elif IS_USED(MODULE_NRFX_NFCT)
    #include "nrfx_nfct_t2t_emulator.h"
#endif


#if IS_USED(MODULE_NFC_T2T)
#endif
    #include "nfct_t2t_emulator.h"
    /**
     * @brief Starts emulation of the given type 2 tag using the emulator
     * 
     * @param dev Type 2 tag emulator
     * @param tag Type 2 tag
     * @param tag_type NFC tag type
     */
    void nfct_create_type_2_tag(nfct_t2t_emulator_t* dev, void *tag, const nfct_tag_type_t tag_type);

    /**
     * @brief Adds the given NDEF message to the given type 2 tag and emulates 
     * the tag using the emulator
     * 
     * @param dev Type 2 tag emulator
     * @param tag Type 2 tag
     * @param message NDEF message 
     * @param tag_type NFC tag type
     */
    void nfct_create_type_2_tag_with_ndef(nfct_t2t_emulator_t* dev, void* tag, 
    const ndef_t* message, nfct_tag_type_t tag_type);
    
    //convenience
    /**
     * @brief Initializes the type 2 tag struct, adds the given text content 
     * and emulates it.
     * 
     * @param dev Type 2 tag emulator
     * @param tag Uninitialized type 2 tag struct
     * @param tag_mem Empty tag memory
     * @param tag_mem_size Size of tag_mem
     * @param content Text String to put in tag
     * @param lang_code Language code for Text (defaults to "en" if NULL)
     * @param encoding Text Encoding
     */
    void nfct_create_type_2_tag_with_text(nfct_t2t_emulator_t* dev, nfc_t2t_t *tag,
    uint8_t* tag_mem, size_t tag_mem_size, const char *content, char* lang_code,
    ndef_text_encoding_t encoding);

    /**
     * @brief Initializes the type 2 tag struct, adds the given URI and emulates it.
     * 
     * @param dev Type 2 tag emulator
     * @param tag Uninitialized type 2 tag struct
     * @param tag_mem Empty tag memory
     * @param tag_mem_size Size of tag_mem
     * @param identifier_code NDEF URI identifier
     * @param uri URI
     */
    void nfct_create_type_2_tag_with_uri(nfct_t2t_emulator_t* dev, nfc_t2t_t *tag, 
    uint8_t* tag_mem, size_t tag_mem_size, ndef_uri_identifier_code_t identifier_code, 
    const char *uri);

    /**
     * @brief Stops the type 2 tag emulation of the given emulator.
     * 
     * @param dev Type 2 tag emulator
     */
    void nfct_delete_type_2_tag(nfct_t2t_emulator_t* dev);
#endif

//#endif
