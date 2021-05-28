/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "fido2/ctap/ctap_cbor.h"

static int parse_rp(CborValue *it, ctap_rp_ent_t *rp);
static int parse_user(CborValue *it, ctap_user_ent_t *user);
static int parse_pub_key_cred_params(CborValue *it,
                                     ctap_make_credential_req_t *req);
static int parse_pub_key_cred_param(CborValue *it, uint8_t *cred_type,
                                    int32_t *alg_type);
static int parse_allow_list(CborValue *it, ctap_cred_desc_alt_t *allow_list,
                            size_t *allow_list_len);
static int parse_exclude_list(CborValue *it, ctap_cred_desc_alt_t *exclude_list,
                              size_t *exclude_list_len);
static int parse_options(CborValue *it, ctap_options_t *options);
static int parse_cose_key(CborValue *it, ctap_cose_key_t *cose_key);
static int encode_cose_key(CborEncoder *cose_key, const ctap_cose_key_t *key);
static int encode_credential(CborEncoder *encoder, const void *cred_ptr,
                             bool rk);
static int encode_user_entity(CborEncoder *it, const ctap_resident_key_t *rk);
static int parse_fixed_size_byte_array(CborValue *map, uint8_t *dst,
                                       size_t *len);
static int parse_byte_array(CborValue *it, uint8_t *dst, size_t *len);
static int parse_text_string(CborValue *it, char *dst, size_t *len);
static int parse_int(CborValue *it, int *num);

int fido2_ctap_cbor_encode_info(CborEncoder *encoder, const ctap_info_t *info)
{
    int ret;
    size_t sz = 0;
    CborEncoder map;
    CborEncoder map2;
    CborEncoder array;
    CborEncoder array2;

    ret = cbor_encoder_create_map(encoder, &map, 5);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (info->versions & CTAP_VERSION_FLAG_FIDO) {
        sz++;
    }
    if (info->versions & CTAP_VERSION_FLAG_FIDO_PRE) {
        sz++;
    }

    /* encode versions */
    ret = cbor_encode_uint(&map, CTAP_GET_INFO_RESP_VERSIONS);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_create_array(&map, &array, sz);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    if (info->versions & CTAP_VERSION_FLAG_FIDO) {
        ret = cbor_encode_text_stringz(&array, CTAP_VERSION_STRING_FIDO);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->versions & CTAP_VERSION_FLAG_FIDO_PRE) {
        ret = cbor_encode_text_stringz(&array, CTAP_VERSION_STRING_FIDO_PRE);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    ret = cbor_encoder_close_container(&map, &array);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* todo: encode supported extensions once implemented */

    /* encode aaguid */
    ret = cbor_encode_uint(&map, CTAP_GET_INFO_RESP_AAGUID);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, info->aaguid, sizeof(info->aaguid));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    sz = 0;

    if (info->options & CTAP_INFO_OPTIONS_FLAG_PLAT) {
        sz++;
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_RK) {
        sz++;
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_CLIENT_PIN) {
        sz++;
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_UP) {
        sz++;
    }

    /* encode options */
    /* order of the items is important. needs to be canonical CBOR */
    ret = cbor_encode_uint(&map, CTAP_GET_INFO_RESP_OPTIONS);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_create_map(&map, &map2, sz);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (info->options & CTAP_INFO_OPTIONS_FLAG_RK) {
        ret =
            cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_RK, 2);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, 1);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_UP) {
        ret =
            cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_UP, 2);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, 1);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    /* default for up is true so need to set false explicitly if not supported */
    else {
        ret =
            cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_UP, 2);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, 0);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_PLAT) {
        ret = cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_PLAT,
                                      4);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, 1);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_CLIENT_PIN) {
        ret = cbor_encode_text_string(&map2,
                                      CTAP_GET_INFO_RESP_OPTIONS_ID_CLIENT_PIN,
                                      9);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        if (info->pin_is_set) {
            ret = cbor_encode_boolean(&map2, 1);
            if (ret != CborNoError) {
                return CTAP2_ERR_CBOR_PARSING;
            }
        }
        else {
            ret = cbor_encode_boolean(&map2, 0);
            if (ret != CborNoError) {
                return CTAP2_ERR_CBOR_PARSING;
            }
        }
    }

    ret = cbor_encoder_close_container(&map, &map2);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* encode maxMsgSize */
    ret = cbor_encode_uint(&map, CTAP_GET_INFO_RESP_MAX_MSG_SIZE);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_uint(&map, info->max_msg_size);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* encode pinProtocols */
    ret = cbor_encode_uint(&map, CTAP_GET_INFO_RESP_PIN_PROTOCOLS);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_create_array(&map, &array2, 1);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_int(&array2, info->pin_protocol);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_close_container(&map, &array2);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_assertion_object(CborEncoder *encoder,
                                            const ctap_auth_data_header_t *auth_data,
                                            const uint8_t *client_data_hash,
                                            ctap_resident_key_t *rk,
                                            uint8_t valid_cred_count)
{
    int ret;
    CborEncoder map;
    uint8_t sig_buf[CTAP_CRYPTO_ES256_DER_MAX_SIZE];
    size_t sig_buf_len = sizeof(sig_buf);
    ctap_cred_id_t id;
    ctap_cred_desc_t *cred_desc = &rk->cred_desc;
    bool is_resident = !cred_desc->has_nonce;

    /* map contains at least credential descriptor, authData and signature */
    uint8_t map_len = 3;

    if (valid_cred_count > 1) {
        map_len++;
    }

    if (is_resident) {
        map_len++;
    }

    ret = cbor_encoder_create_map(encoder, &map, map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_GA_RESP_CREDENTIAL);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* encode credential */
    /* if not a resident key, encrypt key and store it in credential id */
    if (!is_resident) {
        ret = fido2_ctap_encrypt_rk(rk, rk->cred_desc.nonce,
                                    sizeof(rk->cred_desc.nonce), &id);

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = encode_credential(&map, (void *)&id, false);
    }
    else {
        ret = encode_credential(&map, (void *)&rk->cred_desc, true);
    }

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* encode auth data */
    ret = cbor_encode_int(&map, CTAP_GA_RESP_AUTH_DATA);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret =
        cbor_encode_byte_string(&map, (uint8_t *)auth_data, sizeof(*auth_data));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* get signature for assertion */
    ret = fido2_ctap_get_sig((uint8_t *)auth_data, sizeof(*auth_data),
                             client_data_hash, rk, sig_buf,
                             &sig_buf_len);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* encode signature */
    ret = cbor_encode_int(&map, CTAP_GA_RESP_SIGNATURE);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, sig_buf, sig_buf_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* user_id mandatory if resident credential */
    if (is_resident) {
        ret = cbor_encode_int(&map, CTAP_GA_RESP_USER);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = encode_user_entity(&map, rk);
        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    /* if more than 1 valid credential found, encode amount of eligible creds found */
    if (valid_cred_count > 1) {
        ret = cbor_encode_int(&map, CTAP_GA_RESP_NUMBER_OF_CREDENTIALS);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_int(&map, valid_cred_count);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_attestation_object(CborEncoder *encoder,
                                              const ctap_auth_data_t *auth_data,
                                              const uint8_t *client_data_hash,
                                              ctap_resident_key_t *rk)
{
    int ret;
    uint16_t cred_id_sz;
    uint16_t cred_header_sz;
    size_t offset = 0;
    uint8_t *cose_key_buf;
    uint8_t sig_buf[CTAP_CRYPTO_ES256_DER_MAX_SIZE];
    size_t sig_buf_len = sizeof(sig_buf);
    uint8_t auth_data_buf[512] = { 0 };
    const ctap_attested_cred_data_header_t *cred_header;
    CborEncoder map;
    CborEncoder cose_key;
    CborEncoder attest_stmt_map;

    cred_header = &auth_data->attested_cred_data.header;
    cred_id_sz = (cred_header->cred_len_h << 8) | cred_header->cred_len_l;

    /* size varies depending on if cred_id is the encrypted rk or 16 rand bytes */
    cred_header_sz = cred_id_sz + sizeof(cred_header->aaguid) + \
                     sizeof(cred_header->cred_len_h) + \
                     sizeof(cred_header->cred_len_h);

    ret = cbor_encoder_create_map(encoder, &map, 3);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* packed attestation format (webauthn specification (version 20190304) section 8.2) */
    ret = cbor_encode_int(&map, CTAP_MC_RESP_FMT);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_text_stringz(&map, "packed");
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* move rp id hash, flash and counter into authenticator data buffer */
    memmove(auth_data_buf, (void *)&auth_data->header,
            sizeof(ctap_auth_data_header_t));
    offset += sizeof(ctap_auth_data_header_t);

    /* move attested credential data header into authenticator data buffer  */
    memmove(auth_data_buf + offset, (void *)cred_header, cred_header_sz);
    offset += cred_header_sz;

    cose_key_buf = auth_data_buf + offset;

    cbor_encoder_init(&cose_key, cose_key_buf, sizeof(auth_data_buf) - offset,
                      0);

    /* encode credential public key into COSE format */
    ret = encode_cose_key(&cose_key, &auth_data->attested_cred_data.key);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    offset += cbor_encoder_get_buffer_size(&cose_key, cose_key_buf);

    /* encode the authenticator data */
    ret = cbor_encode_int(&map, CTAP_MC_RESP_AUTH_DATA);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, auth_data_buf, offset);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* sign authenticator data */
    ret = fido2_ctap_get_sig(auth_data_buf, offset, client_data_hash, rk,
                             sig_buf, &sig_buf_len);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* encode attestation statement */
    ret = cbor_encode_int(&map, CTAP_MC_RESP_ATT_STMT);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_create_map(&map, &attest_stmt_map, 2);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_stringz(&attest_stmt_map, "alg");
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_int(&attest_stmt_map, CTAP_COSE_ALG_ES256);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_stringz(&attest_stmt_map, "sig");
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&attest_stmt_map, sig_buf, sig_buf_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(&map, &attest_stmt_map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* todo: extensions once implemented */

    return CTAP2_OK;
}

static int encode_credential(CborEncoder *encoder, const void *cred_ptr,
                             bool rk)
{
    CborEncoder desc;
    int ret;

    ret = cbor_encoder_create_map(encoder, &desc, 2);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&desc, "id", 2);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (rk) {
        ctap_cred_desc_t *cred_desc = (ctap_cred_desc_t *)cred_ptr;
        ret = cbor_encode_byte_string(&desc, cred_desc->cred_id,
                                      sizeof(cred_desc->cred_id));
    }
    else {
        ctap_cred_id_t *id = (ctap_cred_id_t *)cred_ptr;
        ret = cbor_encode_byte_string(&desc, (uint8_t *)id,
                                      sizeof(*id));
    }

    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&desc, "type", 4);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&desc, "public-key", 10);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &desc);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_key_agreement(CborEncoder *encoder,
                                         const ctap_cose_key_t *key)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(encoder, &map, 1);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CP_RESP_KEY_AGREEMENT);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = encode_cose_key(&map, key);

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_retries(CborEncoder *encoder, uint8_t tries_left)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(encoder, &map, 1);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CP_RESP_RETRIES);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, (int)tries_left);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_pin_token(CborEncoder *encoder, uint8_t *token,
                                     size_t size)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(encoder, &map, 1);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CP_RESP_PIN_TOKEN);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_byte_string(&map, token, size);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

static int encode_user_entity(CborEncoder *encoder,
                              const ctap_resident_key_t *rk)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(encoder, &map, 1);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&map, "id", 2);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, rk->user_id, rk->user_id_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

/* https://tools.ietf.org/html/rfc8152#page-34 Section 13.1.1 */
static int encode_cose_key(CborEncoder *cose_key, const ctap_cose_key_t *key)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(cose_key, &map, 5);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_COSE_KEY_LABEL_KTY);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_int(&map, key->kty);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_COSE_KEY_LABEL_ALG);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_int(&map, key->alg_type);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_COSE_KEY_LABEL_CRV);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_int(&map, key->crv);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_COSE_KEY_LABEL_X);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, key->pubkey.x, sizeof(key->pubkey.x));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_COSE_KEY_LABEL_Y);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, key->pubkey.y, sizeof(key->pubkey.y));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(cose_key, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_parse_get_assertion_req(ctap_get_assertion_req_t *req,
                                            const uint8_t *req_raw, size_t size)
{
    uint8_t required_parsed = 0;
    int ret;
    int key;
    int temp;
    size_t len;
    size_t map_len;
    CborParser parser;
    CborValue it;
    CborValue map;
    CborType type;

    ret = cbor_parser_init(req_raw, size, CborValidateCanonicalFormat, &parser,
                           &it);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&it);
    if (type != CborMapType) {
        return CTAP2_ERR_CBOR_UNEXPECTED_TYPE;
    }

    ret = cbor_value_enter_container(&it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(&it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* loop over CBOR GetAssertion map */
    for (size_t i = 0; i < map_len; i++) {
        type = cbor_value_get_type(&map);
        if (type != CborIntegerType) {
            return CTAP2_ERR_CBOR_UNEXPECTED_TYPE;
        }

        ret = cbor_value_get_int_checked(&map, &key);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        switch (key) {
            case CTAP_GA_REQ_RP_ID:
                DEBUG("ctap_cbor: parse rp_id \n");
                req->rp_id_len = CTAP_DOMAIN_NAME_MAX_SIZE;
                ret = parse_text_string(&map, (char *)req->rp_id,
                                        (size_t *)&req->rp_id_len);
                required_parsed++;
                break;
            case CTAP_GA_REQ_CLIENT_DATA_HASH:
                DEBUG("ctap_cbor: parse client_data_hash \n");
                len = SHA256_DIGEST_LENGTH;
                ret =
                    parse_fixed_size_byte_array(&map, req->client_data_hash, &len);
                required_parsed++;
                break;
            case CTAP_GA_REQ_ALLOW_LIST:
                DEBUG("ctap_cbor: parse allow_list \n");
                ret = parse_allow_list(&map, req->allow_list,
                                       (size_t *)&req->allow_list_len);
                break;
            case CTAP_GA_REQ_EXTENSIONS:
                /* todo: implement once extensions are supported */
                DEBUG("ctap_cbor: parse extensions \n");
                break;
            case CTAP_GA_REQ_OPTIONS:
                DEBUG("ctap_cbor: parse options \n");
                ret = parse_options(&map, &req->options);
                break;
            case CTAP_GA_REQ_PIN_AUTH:
                DEBUG("ctap_cbor: parse pin_auth \n");
                len = 16;
                ret = parse_fixed_size_byte_array(&map, req->pin_auth, &len);
                /* CTAP specification (version 20190130) section 5.5.8.1 */
                if (ret == CTAP1_ERR_INVALID_LENGTH && len == 0) {
                    ret = CTAP2_OK;
                }
                req->pin_auth_len = len;
                req->pin_auth_present = true;
                break;
            case CTAP_GA_REQ_PIN_PROTOCOL:
                DEBUG("ctap_cbor: parse pin_protocol \n");
                ret = parse_int(&map, &temp);
                req->pin_protocol = (uint8_t)temp;
                break;
            default:
                DEBUG("ctap_cbor: unknown GetAssertion key: %d \n", key);
                break;
        }

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    /* rp_id and client_data_hash are required */
    if (required_parsed != 2) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_parse_client_pin_req(ctap_client_pin_req_t *req,
                                         const uint8_t *req_raw, size_t size)
{
    uint8_t required_parsed = 0;
    int ret;
    int key;
    int type;
    int temp;
    size_t map_len;
    size_t len;
    CborParser parser;
    CborValue it;
    CborValue map;

    ret = cbor_parser_init(req_raw, size, CborValidateCanonicalFormat, &parser,
                           &it);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&it);
    if (type != CborMapType) {
        return CTAP2_ERR_CBOR_UNEXPECTED_TYPE;
    }

    ret = cbor_value_enter_container(&it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(&it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* loop over CBOR ClientPIN map */
    for (size_t i = 0; i < map_len; i++) {
        type = cbor_value_get_type(&map);
        if (type != CborIntegerType) {
            return CTAP2_ERR_CBOR_UNEXPECTED_TYPE;
        }

        ret = cbor_value_get_int_checked(&map, &key);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        switch (key) {
            case CTAP_CP_REQ_PIN_PROTOCOL:
                DEBUG("ctap_cbor: parse pinProtocol \n");
                ret = parse_int(&map, &temp);
                req->pin_protocol = (uint8_t)temp;
                required_parsed++;
                break;
            case CTAP_CP_REQ_SUB_COMMAND:
                DEBUG("ctap_cbor: parse subCommand \n");
                ret = parse_int(&map, &temp);
                req->sub_command = (uint8_t)temp;
                required_parsed++;
                break;
            case CTAP_CP_REQ_KEY_AGREEMENT:
                DEBUG("ctap_cbor: parse keyAgreement \n");
                ret = parse_cose_key(&map, &req->key_agreement);
                req->key_agreement_present = true;
                break;
            case CTAP_CP_REQ_PIN_AUTH:
                DEBUG("ctap_cbor: parse pinAuth \n");
                len = sizeof(req->pin_auth);
                ret = parse_fixed_size_byte_array(&map, req->pin_auth, &len);
                req->pin_auth_present = true;
                break;
            case CTAP_CP_REQ_NEW_PIN_ENC:
                DEBUG("ctap_cbor: parse newPinEnc \n");
                len = sizeof(req->new_pin_enc);
                ret = parse_byte_array(&map, req->new_pin_enc, &len);
                req->new_pin_enc_size = len;
                break;
            case CTAP_CP_REQ_PIN_HASH_ENC:
                DEBUG("ctap_cbor: parse pinHashEnc \n");
                len = sizeof(req->pin_hash_enc);
                ret = parse_fixed_size_byte_array(&map, req->pin_hash_enc, &len);
                req->pin_hash_enc_present = true;
                break;
            default:
                DEBUG("parse_client_pin unknown key: %d \n", key);
                break;
        }
        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    if (required_parsed != 2) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_parse_make_credential_req(ctap_make_credential_req_t *req,
                                              const uint8_t *req_raw,
                                              size_t size)
{
    uint8_t required_parsed = 0;
    int ret;
    int key;
    int temp;
    size_t len;
    size_t map_len;
    CborParser parser;
    CborValue it;
    CborValue map;
    CborType type;

    ret = cbor_parser_init(req_raw, size, CborValidateCanonicalFormat, &parser,
                           &it);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&it);
    if (type != CborMapType) {
        return CTAP2_ERR_CBOR_UNEXPECTED_TYPE;
    }

    ret = cbor_value_enter_container(&it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(&it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    for (size_t i = 0; i < map_len; i++) {
        type = cbor_value_get_type(&map);
        if (type != CborIntegerType) {
            return CTAP2_ERR_CBOR_UNEXPECTED_TYPE;
        }

        ret = cbor_value_get_int_checked(&map, &key);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        switch (key) {
            case CTAP_MC_REQ_CLIENT_DATA_HASH:
                DEBUG("ctap_cbor: parse clientDataHash \n");
                len = SHA256_DIGEST_LENGTH;
                ret =
                    parse_fixed_size_byte_array(&map, req->client_data_hash, &len);
                required_parsed++;
                break;
            case CTAP_MC_REQ_RP:
                DEBUG("ctap_cbor: parse rp \n");
                ret = parse_rp(&map, &req->rp);
                required_parsed++;
                break;
            case CTAP_MC_REQ_USER:
                DEBUG("ctap_cbor: parse user \n");
                ret = parse_user(&map, &req->user);
                required_parsed++;
                break;
            case CTAP_MC_REQ_PUB_KEY_CRED_PARAMS:
                DEBUG("ctap_cbor: parse key_cred params \n");
                ret = parse_pub_key_cred_params(&map, req);
                required_parsed++;
                break;
            case CTAP_MC_REQ_EXCLUDE_LIST:
                DEBUG("ctap_cbor: parse excludeList \n");
                ret = parse_exclude_list(&map, req->exclude_list,
                                         &req->exclude_list_len);
                break;
            case CTAP_MC_REQ_EXTENSIONS:
                DEBUG("ctap_cbor: parse exclude_list \n");
                ret = CTAP2_ERR_UNSUPPORTED_EXTENSION;
                break;
            case CTAP_MC_REQ_OPTIONS:
                DEBUG("ctap_cbor: parse options \n");
                ret = parse_options(&map, &req->options);
                break;
            case CTAP_MC_REQ_PIN_AUTH:
                DEBUG("ctap_cbor: parse pin_auth \n");
                len = 16;
                ret = parse_fixed_size_byte_array(&map, req->pin_auth, &len);
                /* CTAP specification (version 20190130) section 5.5.8.1 (pinAuth) */
                if (ret == CTAP1_ERR_INVALID_LENGTH && len == 0) {
                    ret = CTAP2_OK;
                }
                req->pin_auth_len = len;
                req->pin_auth_present = true;
                break;
            case CTAP_MC_REQ_PIN_PROTOCOL:
                DEBUG("ctap_cbor: parse pin_protocol \n");
                ret = parse_int(&map, &temp);
                req->pin_protocol = (uint8_t)temp;
                break;
            default:
                DEBUG("ctap_cbor: unknown MakeCredential key: %d \n", key);
                break;
        }

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    if (required_parsed != 4) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

static int parse_cose_key(CborValue *it, ctap_cose_key_t *cose_key)
{
    int ret;
    int type;
    int key;
    int temp;
    size_t map_len;
    size_t len;
    uint8_t required_parsed = 0;
    CborValue map;

    type = cbor_value_get_type(it);
    if (type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_enter_container(it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    for (size_t i = 0; i < map_len; i++) {

        ret = parse_int(&map, &key);

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        switch (key) {
            case CTAP_COSE_KEY_LABEL_KTY:
                ret = parse_int(&map, &cose_key->kty);
                required_parsed++;
                break;
            case CTAP_COSE_KEY_LABEL_ALG:
                ret = parse_int(&map, &temp);
                cose_key->alg_type = (int32_t)temp;
                required_parsed++;
                break;
            case CTAP_COSE_KEY_LABEL_CRV:
                ret = parse_int(&map, &cose_key->crv);
                required_parsed++;
                break;
            case CTAP_COSE_KEY_LABEL_X:
                len = sizeof(cose_key->pubkey.x);
                ret = parse_fixed_size_byte_array(&map, cose_key->pubkey.x, &len);
                required_parsed++;
                break;
            case CTAP_COSE_KEY_LABEL_Y:
                len = sizeof(cose_key->pubkey.y);
                ret = parse_fixed_size_byte_array(&map, cose_key->pubkey.y, &len);
                required_parsed++;
                break;
            default:
                DEBUG("Parse cose key unknown key: %d \n", key);
        }

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    if (required_parsed != 5) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

/* parse PublicKeyCredentialRpEntity dictionary */
static int parse_rp(CborValue *it, ctap_rp_ent_t *rp)
{
    int ret;
    int type;
    CborValue map;
    size_t map_len;
    size_t key_len;
    size_t len;
    char key[8];
    uint8_t required_parsed = 0;

    type = cbor_value_get_type(it);
    if (type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_enter_container(it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    for (size_t i = 0; i < map_len; i++) {
        type = cbor_value_get_type(&map);
        if (type != CborTextStringType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        key_len = sizeof(key);
        ret = cbor_value_copy_text_string(&map, key, &key_len, NULL);
        if (ret == CborErrorOutOfMemory) {
            return CTAP2_ERR_LIMIT_EXCEEDED;
        }

        key[sizeof(key) - 1] = 0;

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        type = cbor_value_get_type(&map);
        if (type != CborTextStringType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        if (strcmp(key, "id") == 0) {
            rp->id_len = CTAP_DOMAIN_NAME_MAX_SIZE;
            ret =
                parse_text_string(&map, (char *)rp->id, (size_t *)&rp->id_len);
            if (ret != CborNoError) {
                return ret;
            }

            required_parsed++;
        }
        else if (strcmp(key, "name") == 0) {
            len = CTAP_RP_MAX_NAME_SIZE;
            ret = parse_text_string(&map, (char *)rp->name, &len);
            if (ret != CborNoError) {
                return ret;
            }
        }
        else if (strcmp(key, "icon") == 0) {
            len = CTAP_DOMAIN_NAME_MAX_SIZE;
            ret = parse_text_string(&map, (char *)rp->icon, &len);
            if (ret != 0) {
                return ret;
            }
        }
        else {
            DEBUG("CTAP_parse_rp: ignoring unknown key: %s \n", key);
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    /* id is mandatory */
    if (required_parsed != 1) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

/* parse PublicKeyCredentialUserEntity dictionary */
static int parse_user(CborValue *it, ctap_user_ent_t *user)
{
    uint8_t id_parsed = 0;
    char key[24];
    size_t key_len;
    size_t map_len;
    size_t len;
    int type;
    int ret;
    CborValue map;

    type = cbor_value_get_type(it);
    if (type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_enter_container(it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    for (size_t i = 0; i < map_len; i++) {
        type = cbor_value_get_type(&map);
        if (type != CborTextStringType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        key_len = sizeof(key);
        ret = cbor_value_copy_text_string(&map, key, &key_len, NULL);
        if (ret == CborErrorOutOfMemory) {
            return CTAP2_ERR_LIMIT_EXCEEDED;
        }

        key[sizeof(key) - 1] = 0;

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        if (strcmp(key, "id") == 0) {
            user->id_len = CTAP_USER_ID_MAX_SIZE;
            ret = parse_byte_array(&map, user->id, (size_t *)&user->id_len);
            if (ret != CTAP2_OK) {
                return ret;
            }

            id_parsed = 1;
        }
        else if (strcmp(key, "name") == 0) {
            len = CTAP_USER_MAX_NAME_SIZE;
            ret = parse_text_string(&map, (char *)user->name, &len);
            if (ret != CTAP2_OK) {
                return ret;
            }
        }
        else if (strcmp(key, "displayName") == 0) {
            len = CTAP_USER_MAX_NAME_SIZE;
            ret = parse_text_string(&map, (char *)user->display_name, &len);
            if (ret != CTAP2_OK) {
                return ret;
            }
        }
        else if (strcmp(key, "icon") == 0) {
            len = CTAP_DOMAIN_NAME_MAX_SIZE;
            ret = parse_text_string(&map, (char *)user->icon, &len);
            if (ret != CTAP2_OK) {
                return ret;
            }
        }
        else {
            DEBUG("CTAP_parse_rp: ignoring unknown key: %s \n", key);
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    /* user id is mandatory */
    if (!id_parsed) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

/* parse pubKeyCredParams array */
static int parse_pub_key_cred_params(CborValue *it,
                                     ctap_make_credential_req_t *req)
{
    int type;
    int ret;
    CborValue arr;
    size_t arr_len;
    uint8_t cred_type;
    int32_t alg_type;

    type = cbor_value_get_type(it);
    if (type != CborArrayType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_enter_container(it, &arr);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_array_length(it, &arr_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* params ordered from most preferred (by the RP) to least */
    for (size_t i = 0; i < arr_len; i++) {
        ret = parse_pub_key_cred_param(&arr, &cred_type, &alg_type);
        if (ret != CTAP2_OK) {
            return ret;
        }

        /* check if algorithm is supported */
        if (fido2_ctap_cred_params_supported(cred_type, alg_type)) {
            req->cred_type = cred_type;
            req->alg_type = alg_type;
            return CTAP2_OK;
        }

        ret = cbor_value_advance(&arr);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    return CTAP2_ERR_UNSUPPORTED_ALGORITHM;
}

/* parse pubKeyCredParam dictionary */
static int parse_pub_key_cred_param(CborValue *it, uint8_t *cred_type,
                                    int32_t *alg_type)
{
    int ret;
    int type;
    char type_str[16];
    CborValue cred, alg;
    size_t type_str_len = sizeof(type_str);

    type = cbor_value_get_type(it);
    if (type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_map_find_value(it, "type", &cred);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&cred);
    if (type != CborTextStringType) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    ret = cbor_value_map_find_value(it, "alg", &alg);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&alg);
    if (type != CborIntegerType) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    ret = cbor_value_copy_text_string(&cred, type_str, &type_str_len, NULL);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type_str[sizeof(type_str) - 1] = 0;

    if (strncmp(type_str, "public-key", 11) == 0) {
        *cred_type = CTAP_PUB_KEY_CRED_PUB_KEY;
    }
    else {
        *cred_type = CTAP_PUB_KEY_CRED_UNKNOWN;
    }

    ret = cbor_value_get_int_checked(&alg, (int *)alg_type);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return 0;
}

/* parse options dictionary */
static int parse_options(CborValue *it, ctap_options_t *options)
{
    int ret;
    int type;
    char key[8];
    size_t key_len = sizeof(key), map_len;
    bool b;
    CborValue map;

    type = cbor_value_get_type(it);
    if (type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_enter_container(it, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_get_map_length(it, &map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    for (size_t i = 0; i < map_len; i++) {
        type = cbor_value_get_type(&map);
        if (type != CborTextStringType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        ret = cbor_value_copy_text_string(&map, key, &key_len, NULL);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        key[sizeof(key) - 1] = 0;

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        type = cbor_value_get_type(&map);
        if (type != CborBooleanType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        ret = cbor_value_get_boolean(&map, &b);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        if (strncmp(key, "rk", 2) == 0) {
            options->rk = b;
        }
        else if (strncmp(key, "uv", 2) == 0) {
            options->uv = b;
        }
        else if (strncmp(key, "up", 2) == 0) {
            options->up = b;
        }
        else {
            /* ignore unknown options */
            DEBUG("Ctap parse options, unknown uption: %s \n", key);
        }

        cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    return CTAP2_OK;
}

static int parse_allow_list(CborValue *it, ctap_cred_desc_alt_t *allow_list,
                            size_t *allow_list_len)
{
    return parse_exclude_list(it, allow_list, allow_list_len);
}

static int parse_exclude_list(CborValue *it, ctap_cred_desc_alt_t *exclude_list,
                              size_t *exclude_list_len)
{
    int ret;
    int type;
    CborValue arr;

    type = cbor_value_get_type(it);
    if (type != CborArrayType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_get_array_length(it, exclude_list_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (*exclude_list_len > CTAP_MAX_EXCLUDE_LIST_SIZE) {
        *exclude_list_len = CTAP_MAX_EXCLUDE_LIST_SIZE;
    }

    ret = cbor_value_enter_container(it, &arr);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    for (uint8_t i = 0; i < *exclude_list_len; i++) {
        /**
         * parse the CBOR encoded PublicKeyCredentialDescriptors of the
         * exclude list sent by the host.
         */
        ret = fido2_ctap_cbor_parse_cred_desc(&arr, exclude_list);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_parse_cred_desc(CborValue *arr, ctap_cred_desc_alt_t *cred)
{
    int ret;
    int type;
    CborValue val;
    char type_str[16];
    size_t buf_len;

    type = cbor_value_get_type(arr);
    if (type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_map_find_value(arr, "type", &val);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&val);
    if (type != CborTextStringType) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    buf_len = sizeof(type_str);

    ret = cbor_value_copy_text_string(&val, type_str, &buf_len, NULL);

    /* CborErrorOutOfMemory == unknown type */
    if (ret != CborNoError && ret != CborErrorOutOfMemory) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    type_str[sizeof(type_str) - 1] = 0;

    if (strncmp(type_str, "public-key", 11) == 0) {
        cred->cred_type = CTAP_PUB_KEY_CRED_PUB_KEY;
    }
    else {
        cred->cred_type = CTAP_PUB_KEY_CRED_UNKNOWN;
    }

    ret = cbor_value_map_find_value(arr, "id", &val);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    type = cbor_value_get_type(&val);
    if (type != CborByteStringType) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    buf_len = sizeof(cred->cred_id);
    ret = cbor_value_copy_byte_string(&val, (uint8_t *)&cred->cred_id, &buf_len,
                                      NULL);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (buf_len < CTAP_CREDENTIAL_ID_SIZE) {
        return CTAP1_ERR_INVALID_LENGTH;
    }

    ret = cbor_value_advance(arr);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

static int parse_fixed_size_byte_array(CborValue *it, uint8_t *dst, size_t *len)
{
    int ret;
    int type;
    size_t temp_len = *len;

    type = cbor_value_get_type(it);
    if (type != CborByteStringType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_copy_byte_string(it, dst, len, NULL);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (temp_len != *len) {
        return CTAP1_ERR_INVALID_LENGTH;
    }

    return CTAP2_OK;
}

static int parse_byte_array(CborValue *it, uint8_t *dst, size_t *len)
{
    int type;
    int ret;

    type = cbor_value_get_type(it);
    if (type != CborByteStringType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_copy_byte_string(it, dst, len, NULL);
    if (ret == CborErrorOutOfMemory) {
        return CTAP2_ERR_LIMIT_EXCEEDED;
    }

    return CTAP2_OK;
}

static int parse_text_string(CborValue *it, char *dst, size_t *len)
{
    int type;
    int ret;

    type = cbor_value_get_type(it);
    if (type != CborTextStringType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_copy_text_string(it, dst, len, NULL);
    if (ret == CborErrorOutOfMemory) {
        return CTAP2_ERR_LIMIT_EXCEEDED;
    }

    dst[*len] = 0;

    return CTAP2_OK;
}

static int parse_int(CborValue *it, int *num)
{
    int type;
    int ret;

    type = cbor_value_get_type(it);
    if (type != CborIntegerType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_get_int_checked(it, num);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}
