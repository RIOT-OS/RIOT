#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

typedef struct __attribute__((packed)){
    uint8_t protocol_id;
    uint8_t spi_size;
    uint16_t notify_type;
} ike_notify_payload_const_t;

int build_notify_payload(char *start, size_t max_len, size_t *new_len,
                         ike_payload_type_t next_payload, uint8_t proto_id, ike_notify_type_t type,
                         chunk_t spi,
                         chunk_t notify)
{
    size_t len = sizeof(ike_generic_payload_header_t)
                 + sizeof(ike_notify_payload_const_t) + spi.len + notify.len;

    if (max_len < len) {
        return -ENOMEM;
    }
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(len),
    };
    ike_notify_payload_const_t nh = {
        .protocol_id = spi.len ? proto_id : 0,
        .spi_size = spi.len,
        .notify_type = htons(type),
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &nh, sizeof(nh));
    memcpy(start + sizeof(h) + sizeof(nh), spi.ptr, spi.len);
    memcpy(start + sizeof(h) + sizeof(nh) + spi.len, notify.ptr, notify.len);
    *new_len = len;

    return 0;
}
