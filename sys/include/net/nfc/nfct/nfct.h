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
    #include "nfct_t2t_emulator.h"

    void nfct_create_type_2_tag(nfct_t2t_emulator_t* dev, void* tag, const ndef_t* message, nfct_tag_type_t tag_type);

    void nfct_delete_type_2_tag(nfct_t2t_emulator_t* dev);
#endif

#endif