#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

typedef struct __attribute__((packed)){
    uint8_t auth_method;
    uint8_t reserved[3];
} ike_auth_payload_const_t;

int build_auth_payload(char *start, size_t max_len, size_t *new_len,
                       ike_payload_type_t next_payload, ike_auth_method_t auth_method,
                       chunk_t auth_data)
{
    size_t len = sizeof(ike_generic_payload_header_t) + +sizeof(ike_auth_payload_const_t) +
                 auth_data.len;

    if (max_len < len) {
        return -ENOMEM;
    }
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(len),
    };
    ike_auth_payload_const_t ph = {
        .auth_method = auth_method,
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &ph, sizeof(ph));
    memcpy(start + sizeof(h) + sizeof(ph), auth_data.ptr, auth_data.len);
    *new_len = len;

    return 0;
}

int process_auth_payload(char *start, size_t max_len, size_t *cur_len,
                         ike_payload_type_t *next_payload, ike_auth_method_t *auth_method,
                         chunk_t *auth_data)
{
    int ret = process_generic_payload_header(start, max_len, cur_len, next_payload);
    size_t auth_len = *cur_len - sizeof(ike_generic_payload_header_t) -
                      sizeof(ike_auth_payload_const_t);
    ike_auth_payload_const_t *ph;

    if (ret < 0) {
        return ret;
    }
    ph = (ike_auth_payload_const_t *)(start + sizeof(ike_generic_payload_header_t));
    if (auth_data->len != auth_len) {
        return -1;
    }
    memcpy(auth_data->ptr,
           start + sizeof(ike_generic_payload_header_t) + sizeof(ike_auth_payload_const_t),
           auth_len);
    *auth_method = ph->auth_method;
    return 0;
}
