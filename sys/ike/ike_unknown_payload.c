#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>

int process_generic_payload_header(char *start, size_t max_len, size_t *cur_len,
                                   ike_payload_type_t *next_payload)
{
    if (max_len < sizeof(ike_generic_payload_header_t)) {
        return -EMSGSIZE;
    }
    ike_generic_payload_header_t *ike_gnrc_hdr = (ike_generic_payload_header_t *)start;

    *cur_len = ntohs(ike_gnrc_hdr->payload_length);
    *next_payload = ike_gnrc_hdr->next_payload;
    if (max_len < *cur_len || sizeof(ike_generic_payload_header_t) > *cur_len) {
        return -EMSGSIZE;
    }
    return 0;
}

int process_unknown_payload(char *start, size_t max_len, size_t *cur_len,
                            ike_payload_type_t *next_payload)
{
    return process_generic_payload_header(start, max_len, cur_len, next_payload);
}
