#include "ike/ike_payloads.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>


typedef struct __attribute__((packed)) {
    uint8_t last;
    uint8_t reserved;
    uint16_t proposal_length;
    uint8_t proposal_num;
    uint8_t protocol_id;
    uint8_t spi_size;
    uint8_t num_transforms;
} ike_proposal_substructure_const_t;

typedef struct __attribute__((packed)) {
    uint8_t last;
    uint8_t reserved1;
    uint16_t transform_length;
    uint8_t transform_type;
    uint8_t reserved2;
    uint16_t transform_id;
} ike_transform_substructure_const_t;

int build_sa_payload(char *start, size_t max_len, size_t *new_len, ike_payload_type_t next_payload,
                     ike_protocol_id_t protocol,
                     ike_transform_encr_t encr, ike_transform_prf_t prf,
                     ike_transform_integ_t integ,
                     ike_transform_dh_t dh, ike_transform_esn_t esn, size_t key_size, chunk_t spi)
{
    uint16_t len = 0;
    uint16_t proposal_len = 0;
    uint8_t proposals_num = 0;

    /* What to build - 2 means it is last transform */
    int build_enc = 0;
    int build_prf = 0;
    int build_integ = 0;
    int build_dh = 0;
    int build_esn = 0;

    if (max_len - len <
        sizeof(ike_generic_payload_header_t) + sizeof(ike_proposal_substructure_const_t) +
        spi.len) {
        return -ENOMEM;
    }
    // Set headers when length is known
    len += sizeof(ike_generic_payload_header_t) + sizeof(ike_proposal_substructure_const_t) +
           spi.len;
    switch (protocol) {
    case (IKE_PROTO_IKE):
        build_enc = 1;
        build_prf = 1;
        build_integ = 1;
        build_dh = 2;
        break;
    case (IKE_PROTO_AH):
    //break;
    case (IKE_PROTO_ESP):
        build_enc = 1;
        build_integ = 1;
        build_esn = 2;
        break;
    default:
        return -EINVAL;
    }
    if (build_enc) {
        typedef struct __attribute__((packed)) {
            ike_transform_substructure_const_t h;
            ike_transform_attribute_t a;
        } enc_substr_t;
        enc_substr_t e = {
            .h = {
                .last = build_enc == 2 ? 0 : 3,
                .transform_length = htons(sizeof(enc_substr_t)),
                .transform_type = IKE_TRANSFORM_TYPE_ENCR,
                .transform_id = htons(encr),
            },
            .a = {
                .t_attribute_type = htons(1 << 15 | IKE_TRANSFORM_ATTRIBUTE_KEYLEN),
                .attribute_value = htons(key_size),
            },
        };
        if (max_len - len < sizeof(e)) {
            return -ENOMEM;
        }
        memcpy(start + len, &e, sizeof(e));
        len += sizeof(e);
        proposal_len += sizeof(e);
        proposals_num++;
    }
    if (build_prf) {
        typedef struct __attribute__((packed)) {
            ike_transform_substructure_const_t h;
        } prf_substr_t;
        prf_substr_t e = {
            .h = {
                .last = build_prf == 2 ? 0 : 3,
                .transform_length = htons(sizeof(prf_substr_t)),
                .transform_type = IKE_TRANSFORM_TYPE_PRF,
                .transform_id = htons(prf),
            },
        };
        if (max_len - len < sizeof(e)) {
            return -ENOMEM;
        }
        memcpy(start + len, &e, sizeof(e));
        len += sizeof(e);
        proposal_len += sizeof(e);
        proposals_num++;
    }
    if (build_integ) {
        typedef struct __attribute__((packed)) {
            ike_transform_substructure_const_t h;
        } integ_substr_t;
        integ_substr_t e = {
            .h = {
                .last = build_integ == 2 ? 0 : 3,
                .transform_length = htons(sizeof(integ_substr_t)),
                .transform_type = IKE_TRANSFORM_TYPE_INTEG,
                .transform_id = htons(integ),
            },
        };
        if (max_len - len < sizeof(e)) {
            return -ENOMEM;
        }
        memcpy(start + len, &e, sizeof(e));
        len += sizeof(e);
        proposal_len += sizeof(e);
        proposals_num++;
    }
    if (build_dh) {
        typedef struct __attribute__((packed)) {
            ike_transform_substructure_const_t h;
        } dh_substr_t;
        dh_substr_t e = {
            .h = {
                .last = build_dh == 2 ? 0 : 3,
                .transform_length = htons(sizeof(dh_substr_t)),
                .transform_type = IKE_TRANSFORM_TYPE_DH,
                .transform_id = htons(dh),
            },
        };
        if (max_len - len < sizeof(e)) {
            return -ENOMEM;
        }
        memcpy(start + len, &e, sizeof(e));
        len += sizeof(e);
        proposal_len += sizeof(e);
        proposals_num++;
    }
    if (build_esn) {
        typedef struct __attribute__((packed)) {
            ike_transform_substructure_const_t h;
        } esn_substr_t;
        esn_substr_t e = {
            .h = {
                .last = build_esn == 2 ? 0 : 3,
                .transform_length = htons(sizeof(esn_substr_t)),
                .transform_type = IKE_TRANSFORM_TYPE_ESN,
                .transform_id = htons(esn),
            },
        };
        if (max_len - len < sizeof(e)) {
            return -ENOMEM;
        }
        memcpy(start + len, &e, sizeof(e));
        len += sizeof(e);
        proposal_len += sizeof(e);
        proposals_num++;
    }
    *new_len = sizeof(ike_generic_payload_header_t) + sizeof(ike_proposal_substructure_const_t) +
               spi.len + proposal_len;
    ike_generic_payload_header_t h = {
        .next_payload = next_payload,
        .payload_length = htons(*new_len),
    };
    ike_proposal_substructure_const_t ph = {
        .proposal_length =
            htons(proposal_len + sizeof(ike_proposal_substructure_const_t) + spi.len),
        .proposal_num = 1,
        .protocol_id = protocol,
        .spi_size = spi.len,
        .num_transforms = proposals_num,
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), &ph, sizeof(ph));
    memcpy(start + sizeof(h) + sizeof(ph), spi.ptr, spi.len);

    return 0;
}

int process_sa_payload(char *start, size_t max_len, size_t *cur_len,
                       ike_payload_type_t *next_payload, ike_protocol_id_t *protocol,
                       ike_transform_encr_t *encr, ike_transform_prf_t *prf,
                       ike_transform_integ_t *integ,
                       ike_transform_dh_t *dh, ike_transform_esn_t *esn, size_t *key_size,
                       chunk_t *spi)
{
    (void)protocol; /* Unused parameter */
    (void)encr;     /* Unused parameter */
    (void)prf;      /* Unused parameter */
    (void)integ;    /* Unused parameter */
    (void)dh;       /* Unused parameter */
    (void)esn;      /* Unused parameter */
    (void)key_size; /* Unused parameter */

    ike_proposal_substructure_const_t *ph;
    size_t remaining_len;
    uint8_t *spi_pld;

    int ret = process_generic_payload_header(start, max_len, cur_len, next_payload);

    if (ret < 0) {
        return ret;
    }
    remaining_len = *cur_len - sizeof(ike_generic_payload_header_t);

    if (remaining_len < sizeof(ike_proposal_substructure_const_t)) {
        return -1;
    }
    ph = (ike_proposal_substructure_const_t *)(start + sizeof(ike_generic_payload_header_t));
    remaining_len -= sizeof(ike_proposal_substructure_const_t);
    if (ph->spi_size > remaining_len || ph->spi_size > spi->len) {
        return -1;
    }
    spi_pld = (uint8_t *)ph + sizeof(ike_proposal_substructure_const_t);
    memcpy(spi->ptr, spi_pld, ph->spi_size);

    // TODO
    return 0;
}
