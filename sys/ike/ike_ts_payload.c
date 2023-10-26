#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

typedef enum {
    TS_IPV4_ADDR_RANGE  = 7,
    TS_IPV6_ADDR_RANGE  = 8,
} ike_ts_type_t;

typedef struct __attribute__((packed)){
    uint8_t ts_num;
    uint8_t reserved[3];
} ike_ts_payload_const_t;

typedef struct __attribute__((packed)){
    uint8_t ts_type;
    uint8_t ip_proto_id;
    uint16_t length;
    uint16_t start_port;
    uint16_t end_port;
    uint8_t start_address[16];
    uint8_t end_address[16];
} ike_ts_structure_const_t;

/* ANY TS */
int build_ts_payload(char *start, size_t max_len, size_t *new_len, ike_payload_type_t next_payload)
{
    size_t len = sizeof(ike_generic_payload_header_t) + sizeof(ike_ts_payload_const_t) +
                 sizeof(ike_ts_structure_const_t);

    if (max_len < len) {
        return -ENOMEM;
    }
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(len),
    };
    ike_ts_payload_const_t ph = {
        .ts_num = 1,
    };
    ike_ts_structure_const_t ts = {
        .ts_type = TS_IPV6_ADDR_RANGE,
        .ip_proto_id = 0,
        .length = htons(sizeof(ike_ts_structure_const_t)),
        .start_port = 0,
        .end_port = 0xFFFF,
        .start_address =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00 },
        .end_address =
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF },
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &ph, sizeof(ph));
    memcpy(start + sizeof(h) + sizeof(ph), &ts, sizeof(ts));
    *new_len = len;

    return 0;
}

int process_ts_payload(char *start, size_t max_len, size_t *cur_len,
                       ike_payload_type_t *next_payload, ipv6_addr_t *ts_start, ipv6_addr_t *ts_end)
{
    int ret = process_generic_payload_header(start, max_len, cur_len, next_payload);

    if (ret < 0) {
        return ret;
    }
    ike_ts_payload_const_t *ph;
    ike_ts_structure_const_t *ts;
    ipv6_addr_t *tmp_addr;

    if (*cur_len !=
        sizeof(ike_generic_payload_header_t) + sizeof(ike_ts_payload_const_t) +
        sizeof(ike_ts_structure_const_t)) {
        return -ENOMEM;
    }
    ph = (ike_ts_payload_const_t *)(start + sizeof(ike_generic_payload_header_t));
    if (ph->ts_num != 1) {
        return -1;
    }
    ts =
        (ike_ts_structure_const_t *)(start + sizeof(ike_generic_payload_header_t) +
                                     sizeof(ike_ts_payload_const_t));
    if (ntohs(ts->length) != sizeof(ike_ts_structure_const_t)) {
        return -1;
    }
    tmp_addr = (ipv6_addr_t *)ts->start_address;
    *ts_start = *tmp_addr;
    tmp_addr = (ipv6_addr_t *)ts->end_address;
    *ts_end = *tmp_addr;
    return 0;
}
