#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

typedef struct __attribute__((packed)){
    uint8_t protocol_id;
    uint8_t spi_size;
    uint16_t num_of_spis;
} ike_notify_payload_const_t;

int build_delete_payload(char *start, size_t max_len, size_t *new_len,
                         ike_payload_type_t next_payload)
{
    size_t len = sizeof(ike_generic_payload_header_t)
                 + sizeof(ike_notify_payload_const_t);

    if (max_len < len) {
        return -ENOMEM;
    }
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(len),
    };
    ike_notify_payload_const_t dh = {
        .protocol_id = IKE_PROTO_IKE,
        .spi_size = 0,
        .num_of_spis = htons(0),
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &dh, sizeof(dh));
    *new_len = len;

    return 0;
}
