#include <stdint.h>

#define NFC_TLV_TYPE_NULL_TLV 0x00
#define NFC_TLV_TYPE_LOCK_CTRL 0x01
#define NFC_TLV_TYPE_MEMORY_CTRL 0x02
#define NFC_TLV_TYPE_NDEF_MSG 0x03
#define NFC_TLV_TYPE_PROPRIETARY 0x04
#define NFC_TLV_TYPE_TERMINATOR 0x05

#define NFC_TLV_SIZE_TYPE_NULL_TLV 0
#define NFC_TLV_SIZE_TYPE_TERMINATOR_TLV 0
#define NFC_TLV_SIZE_TYPE_LOCK_CTRL_TLV 3
#define NFC_TLV_SIZE_TYPE_MEMORY_CTRL_TLV 3

typedef struct {
    uint8_t type;
    uint16_t length;
    uint8_t * address_in_tag;
    uint8_t data[];
} nfc_tlv_t;
