#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

typedef struct __attribute__((packed)){
    uint16_t diffie_helman_group_num;
    uint16_t reserved;
} ike_key_exchange_payload_const_t;

int build_key_exchange_payload(char *start, size_t max_len, size_t *new_len,
                               ike_payload_type_t next_payload, ike_transform_dh_t dh,
                               chunk_t key_data)
{
    size_t len = sizeof(ike_generic_payload_header_t) + sizeof(ike_key_exchange_payload_const_t) +
                 key_data.len;

    if (max_len < len) {
        return -ENOMEM;
    }
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(len),
    };
    ike_key_exchange_payload_const_t kh = {
        .diffie_helman_group_num = htons(dh),
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &kh, sizeof(kh));
    memcpy(start + sizeof(h) + sizeof(kh), key_data.ptr, key_data.len);
    *new_len = len;

    return 0;
}

int process_key_exchange_payload(char *start, size_t max_len, size_t *cur_len,
                                 ike_payload_type_t *next_payload, ike_transform_dh_t *dh,
                                 chunk_t *key_data)
{
    int ret = process_generic_payload_header(start, max_len, cur_len, next_payload);

    if (ret < 0) {
        return ret;
    }
    if (*cur_len <
        sizeof(ike_generic_payload_header_t) + sizeof(ike_key_exchange_payload_const_t)) {
        return -EBADMSG;
    }
    ike_key_exchange_payload_const_t *kh = (ike_key_exchange_payload_const_t *)start +
                                           sizeof(ike_generic_payload_header_t);

    *dh = ntohs(kh->diffie_helman_group_num);
    size_t key_len = *cur_len - sizeof(ike_generic_payload_header_t) -
                     sizeof(ike_key_exchange_payload_const_t);

    free_chunk(key_data);
    *key_data = malloc_chunk(key_len);
    memcpy(key_data->ptr,
           start + sizeof(ike_generic_payload_header_t) + sizeof(ike_key_exchange_payload_const_t),
           key_len);
    return 0;
}
