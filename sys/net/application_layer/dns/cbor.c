/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <martine.lenders@tu-dresden.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "net/dns/types.h"
#include "nanocbor/nanocbor.h"

#include "net/dns/cbor.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NAME_REF_TAG    7U
#define OPT_RR_TAG      141U

typedef struct {
    const uint8_t *comps[CONFIG_DNS_CBOR_NAME_COMPS];
    size_t lens[CONFIG_DNS_CBOR_NAME_COMPS];
    size_t numof;
} _name_ref_t;

ssize_t dns_cbor_compose_query(dns_cbor_status_t *status, void *buf)
{
    nanocbor_encoder_t encoder;
    const char *domain_name = status->question.domain_name;
    size_t name_len = strlen(domain_name);
    size_t len = (name_len > 0) ? 1U : 0U;
    uint16_t type = status->question.type;
    uint16_t class = status->question.class;

    assert(name_len < CONFIG_DNS_CBOR_NAME_LEN);
    assert(buf != NULL);
    assert(status != NULL);
    if (type != DNS_TYPE_AAAA) {
        len++;
    }
    if (class != DNS_CLASS_IN) {
        len++;
    }
    for (unsigned i = 0; i < name_len; i++) {
        if (domain_name[i] == '.') {
            len++;
        }
    }
    nanocbor_encoder_init(&encoder, buf, CONFIG_DNS_CBOR_LEN);
    /* initialize outer array */
    nanocbor_fmt_array(&encoder, 1U);
    nanocbor_fmt_array(&encoder, len);
    const char *ptr = domain_name;
    /* reuse len variable for component length */
    len = 0;
    for (unsigned i = 0; i < name_len; i++) {
        if ((domain_name[i] == '.') || ((i + 1) == name_len)) {
            if ((i + 1) == name_len) {
                len++;
            }
            nanocbor_put_tstrn(&encoder, ptr, len);
            ptr = &domain_name[i + 1];
            len = 0;
        }
        else {
            len++;
        }
    }
    if (type != DNS_TYPE_AAAA) {
        nanocbor_fmt_uint(&encoder, type);
    }
    if (class != DNS_CLASS_IN) {
        nanocbor_fmt_uint(&encoder, class);
    }
    /* reuse len variable for component length */
    len = nanocbor_encoded_len(&encoder);
    if (len > CONFIG_DNS_CBOR_LEN) {
        DEBUG_PUTS("dns_cbor: CBOR object too large for buffer");
        /* using feature of nanocbor: buffer would have overflowed with encoding above */
        return -ENOBUFS;
    }
    /* status->domain_name is already changed */
    status->question.type = type;
    status->question.class = class;
    return (ssize_t)len;
}

static int _extend_name(const char *in, size_t in_len, char *out, size_t *out_len)
{
    if (*out_len > 0) {
        if ((*out_len + 1) >= CONFIG_DNS_CBOR_NAME_LEN) {
            DEBUG("dns_cbor: Name component %.*s too long for name buffer\n",
                  in_len, in);
            return -ENOSPC;
        }
        out[*out_len] = '.';
        (*out_len)++;
    }

    if ((*out_len + in_len) >= CONFIG_DNS_CBOR_NAME_LEN) {
        DEBUG("dns_cbor: Name component %.*s too long for name buffer\n", in_len, in);
        return -ENOSPC;
    }
    strncpy(&out[*out_len], in, in_len);
    *out_len += in_len;
    out[*out_len] = '\0';
    return 0;
}

static int _parse_name(nanocbor_value_t *decoder, _name_ref_t *name_ref,
                       char *name, size_t *name_len, int idx);

static int _decode_name_ref(nanocbor_value_t *decoder, _name_ref_t *name_ref,
                            char *name, size_t *name_len, unsigned idx)
{
    nanocbor_value_t name_decoder;
    int res = 0;

    if ((res = _extend_name((const char *)name_ref->comps[idx], name_ref->lens[idx],
                            name, name_len)) < 0) {
        return res;
    }
    nanocbor_decoder_init(&name_decoder, name_ref->comps[idx] + name_ref->lens[idx],
                          decoder->end - name_ref->comps[idx] - name_ref->lens[idx]);
    if (name_decoder.end > decoder->end) {
        DEBUG("dns_cbor: decoder corrupted: end %p > original end %p\n",
              (void *)name_decoder.end, (void *)decoder->end);
        /* this should not happen, but better safe than sorry */
        return -EINVAL;
    }
    return _parse_name(&name_decoder, name_ref, name, name_len, idx);
}

static int _parse_name(nanocbor_value_t *decoder, _name_ref_t *name_ref,
                       char *name, size_t *name_len, int idx)
{
    while (!nanocbor_at_end(decoder)) {
        uint32_t tag;
        const uint8_t *name_component;
        size_t name_component_len;
        const void *tmp = decoder->cur;

        if (nanocbor_get_type(decoder) == NANOCBOR_TYPE_TSTR) {
            int res;

            if (nanocbor_get_tstr(decoder, &name_component, &name_component_len) < 0) {
                DEBUG_PUTS("dns_cbor: unable to parse name component");
                return -EINVAL;
            }
            if ((idx < 0) && (name_ref->numof >= CONFIG_DNS_CBOR_NAME_COMPS)) {
                DEBUG("dns_cbor: no space left to store name component\n");
                return -ENOMEM;
            }
            if ((res = _extend_name((const char *)name_component, name_component_len,
                                    name, name_len)) < 0) {
                return res;
            }
            if (idx < 0) {
                name_ref->comps[name_ref->numof] = name_component;
                name_ref->lens[name_ref->numof] = name_component_len;
                name_ref->numof++;
            }
        }
        else if (nanocbor_get_tag(decoder, &tag) == NANOCBOR_OK) {
            if (tag == NAME_REF_TAG) {
                int res;
                uint8_t inner_idx;

                if (nanocbor_get_uint8(decoder, &inner_idx) < 0) {
                    DEBUG("dns_cbor: Unexpected major type %u or too large number in "
                          "name in response\n", nanocbor_get_type(decoder));
                    return -EINVAL;
                }
                if (inner_idx >= name_ref->numof) {
                    DEBUG("dns_cbor: Unexpected reference to name component #%u\n",
                          inner_idx);
                    return -EINVAL;
                }
                if ((idx >= 0) && (inner_idx >= idx)) {
                    DEBUG("dns_cbor: Unexpected reference to name component #%u\n", idx);
                    return -EINVAL;
                }
                if ((res = _decode_name_ref(decoder, name_ref,
                                            name, name_len, inner_idx)) < 0) {
                    return res;
                }
            }
            else {
                /* trace back */
                decoder->cur = tmp;
            }
            break;
        }
        else {
            break;
        }
    }
    return 0;
}

static int _get_name(nanocbor_value_t *decoder, _name_ref_t *name_ref,
                     char *name, size_t *name_len) {
    return _parse_name(decoder, name_ref, name, name_len, -1);
}

static int _name_to_rdata(const char *in, size_t in_len, void *out)
{
    if ((in_len + 2U) > CONFIG_DNS_CBOR_OUT_LEN) {
        DEBUG("dns_cbor: rdata too large to copy into out buffer\n");
        return -ENOBUFS;
    }
    uint8_t *size_ptr = out;
    uint8_t *write_ptr = size_ptr + 1;
    *size_ptr = 0U;

    if (in_len == 0) {
        /* name is root '.' */
        return 1U;
    }
    for (unsigned i = 0; i < in_len; i++, write_ptr++) {
        if (in[i] == '.') {
            *write_ptr = 0U;
            size_ptr = write_ptr;
        }
        else {
            *size_ptr += 1;
            *write_ptr = (uint8_t)in[i];
        }
    }
    *write_ptr = 0U;
    return (write_ptr - ((uint8_t *)out)) + 1;
}

static int _get_rr(nanocbor_value_t *rr, _name_ref_t *name_ref,
                   dns_cbor_status_t *status, void *out, uint32_t *ttl)
{
    char name[CONFIG_DNS_CBOR_NAME_LEN];
    const uint8_t *tmp;
    int res;
    size_t name_len = 0;
    size_t rdata_len = 0;
    uint16_t type = status->question.type;
    uint16_t class = status->question.class;

    if ((res = _get_name(rr, name_ref, name, &name_len)) < 0) {
        return res;
    }
    if (nanocbor_get_uint32(rr, ttl) < 0) {
        DEBUG("dns_cbor: Unexpected major type %u or too large number in response\n",
              nanocbor_get_type(rr));
        return -EINVAL;
    }
    if (nanocbor_get_type(rr) == NANOCBOR_TYPE_UINT) {
        nanocbor_get_uint16(rr, &type);
    }
    if (nanocbor_get_type(rr) == NANOCBOR_TYPE_UINT) {
        nanocbor_get_uint16(rr, &class);
    }
    const uint8_t *rdata;
    tmp = rr->cur;
    /* rdata is a name */
    if (nanocbor_at_end(rr) ||
        nanocbor_get_type(rr) == NANOCBOR_TYPE_TSTR ||
        nanocbor_get_type(rr) == NANOCBOR_TYPE_TAG) {
        uint32_t tag = 0;

        if ((nanocbor_get_tag(rr, &tag) == NANOCBOR_OK) && (tag != NAME_REF_TAG)) {
            DEBUG("dns_cbor: Record data with tag %" PRIu32 " not supported\n", tag);
            return -EINVAL;
        }
        else if (tag == NAME_REF_TAG) {
            /* reset cur pointer */
            rr->cur = tmp;
        }

        char rdata_name[CONFIG_DNS_CBOR_NAME_LEN];
        size_t rdata_name_len = 0;

        if ((res = _get_name(rr, name_ref, rdata_name, &rdata_name_len)) < 0) {
            return res;
        }
        if (!nanocbor_at_end(rr)) {
            DEBUG("dns_cbor: Unexpected major type %u in resource record\n",
                  nanocbor_get_type(rr));
            return -EINVAL;
        }
        if ((type == status->question.type) &&
            (class == status->question.class) &&
            ((name_len == 0) || (
                (strlen(status->question.domain_name) == name_len) &&
                (strncmp(status->question.domain_name, name, name_len) == 0)
            ))) {
            return _name_to_rdata(rdata_name, rdata_name_len, out);
        }
    }
    else if (nanocbor_get_bstr(rr, &rdata, &rdata_len) == NANOCBOR_OK) {
        /* copy only if it is the queried record data */
        if ((type == status->question.type) &&
            (class == status->question.class) &&
            ((name_len == 0) || (
                (strlen(status->question.domain_name) == name_len) &&
                (strncmp(status->question.domain_name, name, name_len) == 0)
            ))) {
            if (rdata_len > CONFIG_DNS_CBOR_OUT_LEN) {
                DEBUG("dns_cbor: rdata too large to copy into out buffer\n");
                return -ENOBUFS;
            }
            if (rdata_len > 0) {
                memcpy(out, rdata, rdata_len);
            }
            return rdata_len;
        }
        DEBUG("dns_cbor: Not searching for name=\"%.*s\", type=%u, class=%u\n",
              name_len, name, type, class);
    }
    else {
        DEBUG("dns_cbor: Invalid major type %u in resource record\n",
              nanocbor_get_type(rr));
        return -EINVAL;
    }
    return -EBADMSG;
}

static int _skip_opt_rr(nanocbor_value_t *section)
{
    nanocbor_value_t rr;
    nanocbor_value_t opts;
    uint64_t u64;
    uint8_t count = 0;

    if (nanocbor_enter_array(section, &rr) < 0) {
        DEBUG("dns_cbor: Unexpected major type %u in option of response\n",
              nanocbor_get_type(section));
        goto error;
    }
    if (nanocbor_at_end(&rr)) {
        nanocbor_leave_container(section, &rr);
        return 0;
    }
    nanocbor_get_uint64(&rr, &u64);
    if (nanocbor_enter_array(&rr, &opts) < 0) {
        DEBUG("dns_cbor: Unexpected major type %u in option of response\n",
              nanocbor_get_type(&rr));
        goto error;
    }
    while (!nanocbor_at_end(&opts)) {
        const uint8_t *opt_value;
        size_t opt_len;
        if (nanocbor_get_uint64(&opts, &u64) < 0) {
            DEBUG("dns_cbor: Unexpected major type %u in option of response\n",
                  nanocbor_get_type(&opts));
            goto error;
        }
        if (nanocbor_at_end(&opts)) {
            DEBUG("dns_cbor: Missing option value for option %u\n", (unsigned)u64);
            goto error;
        }
        if (nanocbor_get_bstr(&opts, &opt_value, &opt_len) < 0) {
            DEBUG("dns_cbor: Unexpected major type %u in option of response\n",
                  nanocbor_get_type(&opts));
            goto error;
        }
    }
    nanocbor_leave_container(&rr, &opts);
    while (!nanocbor_at_end(&rr)) {
        if (nanocbor_get_uint64(&rr, &u64) < 0) {
            DEBUG("dns_cbor: Unexpected major type %u in option of response\n",
                  nanocbor_get_type(&rr));
            return -EINVAL;
        }
        if (++count > 3) {
            DEBUG_PUTS("dns_cbor: too many uint fields in tail of option of response");
            goto error;
        }
    }
    nanocbor_leave_container(section, &rr);
    return 0;
error:
    return -EINVAL;
}

static int _get_section(nanocbor_value_t *section, _name_ref_t *name_ref,
                        dns_cbor_status_t *status, void *out, uint32_t *ttl)
{
    int res = -EBADMSG;
    int count = 0;
    while (!nanocbor_at_end(section)) {
        uint32_t tag = 0;

        count++;
        if (nanocbor_get_type(section) == NANOCBOR_TYPE_ARR) {
            nanocbor_value_t rr;

            nanocbor_enter_array(section, &rr);
            if ((res = _get_rr(&rr, name_ref, status, out, ttl)) != 0) {
                if (res != -EBADMSG) {
                    return res;
                }
            }
            nanocbor_leave_container(section, &rr);
        }
        else if (nanocbor_get_type(section) == NANOCBOR_TYPE_BSTR) {
            DEBUG_PUTS("dns_cbor: raw bstr RRs not supported");
            nanocbor_skip_simple(section);
        }
        else if (nanocbor_get_tag(section, &tag) == NANOCBOR_OK && (tag == OPT_RR_TAG)) {
            if ((res = _skip_opt_rr(section)) < 0) {
                return res;
            }
            res = -EBADMSG;     /* continue to assume bad message */
        }
        else {
            DEBUG("dns_cbor: Unexpected major type %u or tag %" PRIu32 " in response\n",
                  nanocbor_get_type(section), tag);
            return -EINVAL;
        }
    }
    return res;
}

ssize_t dns_cbor_parse_response(const void *buf, size_t len, dns_cbor_status_t *status,
                                void *out, uint32_t *ttl)
{
    nanocbor_value_t decoder, response, section;
    _name_ref_t name_ref = { .numof = 0 };
    ssize_t res = 0;
    uint16_t flags;

    assert(status != NULL);
    assert(out != NULL);
    assert(ttl != NULL);
    nanocbor_decoder_init(&decoder, buf, len);
    if (nanocbor_get_type(&decoder) != NANOCBOR_TYPE_ARR) {
        DEBUG_PUTS("dns_cbor: response is not an array");
        return -EINVAL;
    }
    nanocbor_enter_array(&decoder, &response);
    if (nanocbor_get_type(&response) == NANOCBOR_TYPE_UINT) {
        if (nanocbor_get_uint16(&response, &flags) < 0) {
            DEBUG_PUTS("dns_cbor: flags too large");
            return -EINVAL;
        }
    }
    else {
        flags = 0x8000;
    }
    if (!(flags & 0x8000)) {
        DEBUG_PUTS("dns_cbor: message to parse is not a response");
        return -EINVAL;
    }
    /* try to get question or answer */
    if (nanocbor_get_type(&response) != NANOCBOR_TYPE_ARR) {
        DEBUG("dns_cbor: unexpected major type %d in response array\n",
              nanocbor_get_type(&response));
        return -EINVAL;
    }
    nanocbor_enter_array(&response, &section);
    if (nanocbor_at_end(&section)) {
        nanocbor_value_t tmp_value;

        /* backup response to check if nothing comes after the section */
        memcpy(&tmp_value, &response, sizeof(tmp_value));
        nanocbor_leave_container(&response, &section);
        if (nanocbor_at_end(&response)) {
            /* only one empty list => answer section which is empty */
            DEBUG_PUTS("dns_cbor: answer section is empty");
            return -EBADMSG;
        }
        /* restore response and section */
        memcpy(&response, &tmp_value, sizeof(response));
        nanocbor_enter_array(&response, &section);
    }
    /* reuse flags to store type */
    flags = nanocbor_get_type(&section);
    /* flags could be legally NANOCBOR_TYPE_TAG (NAME_REF_TAG) but it would be
     * non-sensical at this point since we found no name component yet it could refer
     * to. So rather: use it to detect OPT_RR_TAG.
     */
    if ((flags != NANOCBOR_TYPE_ARR) && (flags != NANOCBOR_TYPE_BSTR) &&
        (flags != NANOCBOR_TYPE_TAG)) {
        /* decode question section */
        char name[CONFIG_DNS_CBOR_NAME_LEN];
        size_t name_len = 0;
        uint16_t type = DNS_TYPE_AAAA;
        uint16_t class = DNS_CLASS_IN;

        if ((res = _get_name(&section, &name_ref, name, &name_len)) < 0) {
            return res;
        }
        if (!nanocbor_at_end(&section)) {
            if (nanocbor_get_uint16(&section, &type) < 0) {
                DEBUG("dns_cbor: Unexpected major type %u or too large number in "
                      "question section of response\n", nanocbor_get_type(&section));
                return -EINVAL;
            }
            if (!nanocbor_at_end(&section)) {
                if (nanocbor_get_uint16(&section, &class) < 0) {
                    DEBUG("dns_cbor: Unexpected major type %u or too large number in "
                          "question section of response\n", nanocbor_get_type(&section));
                    return -EINVAL;
                }
            }
        }
        if (name_len > 0) {
            strncpy(status->question.domain_name, name, name_len);
        }
        else {
            status->question.domain_name[0] = '\0';
        }
        status->question.type = type;
        status->question.class = class;
        nanocbor_leave_container(&response, &section);
        if (nanocbor_enter_array(&response, &section) < 0) {
            DEBUG("dns_cbor: Unexpected major type %u after question\n",
                  nanocbor_get_type(&response));
            return -EINVAL;
        }
    }
    /* handle answer potentially after question section */
    if ((res = _get_section(&section, &name_ref, status, out, ttl)) < 0) {
        if (ENABLE_DEBUG && (res == -EBADMSG)) {
            DEBUG_PUTS("dns_cbor: Unable to find queried data in answer section\n");
        }
        return res;
    }
    nanocbor_leave_container(&response, &section);
    return res;
}

/** @} */
