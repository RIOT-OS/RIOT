#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

typedef struct __attribute__((packed)){
    uint8_t id_type;
    uint8_t reserved[3];
} ike_identification_payload_const_t;

int build_identification_payload(char *start, size_t max_len, size_t *new_len,
                                 ike_payload_type_t next_payload, ike_id_type_t id_type, chunk_t id)
{
    size_t len = sizeof(ike_generic_payload_header_t) +
                 +sizeof(ike_identification_payload_const_t) + id.len;

    if (max_len < len) {
        return -ENOMEM;
    }
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(len),
    };
    ike_identification_payload_const_t ph = {
        .id_type = id_type,
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &ph, sizeof(ph));
    memcpy(start + sizeof(h) + sizeof(ph), id.ptr, id.len);
    *new_len = len;

    return 0;
}

int process_identification_payload(char *start, size_t max_len, size_t *cur_len,
                                   ike_payload_type_t *next_payload, ike_id_type_t *id_type,
                                   chunk_t *id)
{
    int ret = process_generic_payload_header(start, max_len, cur_len, next_payload);

    if (ret < 0) {
        return ret;
    }
    ike_identification_payload_const_t *ph;

    if (*cur_len <
        sizeof(ike_generic_payload_header_t) + sizeof(ike_identification_payload_const_t)) {
        return -ENOMEM;
    }
    size_t id_len = *cur_len - sizeof(ike_generic_payload_header_t) -
                    sizeof(ike_identification_payload_const_t);

    ph = (ike_identification_payload_const_t *)(start + sizeof(ike_identification_payload_const_t));
    *id_type = ph->id_type;
    free_chunk(id);
    *id = malloc_chunk(id_len);
    memcpy(id->ptr,
           start + sizeof(ike_generic_payload_header_t) + sizeof(ike_identification_payload_const_t),
           id_len);
    return 0;
}
