/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap_cbor
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include "fido2/ctap/ctap_cbor.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief CTAP CBOR entity types
 */
typedef enum {
    USER,
    RP
} entity_type_t;

/**
 * @brief Parse CBOR encoded PublicKeyCredentialRpEntity or PublicKeyCredentialUserEntity
 *        data structure into @ref ctap_rp_ent_t or @ref ctap_user_ent_t struct
 *        respectively.
 */
static int _parse_entity(CborValue *it, void *entity, entity_type_t type);

/**
 * @brief Parse CBOR encoded sequence of PublicKeyCredentialDescriptors into
 *        @ref ctap_cred_desc_alt_t struct
 */
static int _parse_exclude_list(CborValue *it, ctap_cred_desc_alt_t *exclude_list,
                               size_t *exclude_list_len);

/**
 * @brief Parse CBOR encoded sequence of PublicKeyCredentialDescriptors into
 *        @ref ctap_cred_desc_alt_t struct
 */
static int _parse_allow_list(CborValue *it, ctap_cred_desc_alt_t *allow_list,
                             uint8_t *allow_list_len);

/**
 * @brief Parse CBOR encoded sequence of PublicKeyCredentialType and cryptographic
 *        algorithm type pairs and check if the combination is supported
 */
static int _parse_pub_key_cred_params(CborValue *it,
                                      ctap_make_credential_req_t *req);

/**
 * @brief Parse CBOR encoded PublicKeyCredentialType and cryptographic
 *        algorithm type
 */
static int _parse_pub_key_cred_param(CborValue *it, uint8_t *cred_type,
                                     int32_t *alg_type);

/**
 * @brief Parse CBOR encoded map of authenticator options into @ref ctap_options_t
 *        struct
 */
static int _parse_options(CborValue *it, ctap_options_t *options);

/**
 * @brief Parse public key in COSE_KEY format into ctap_public_key_cose_t struct
 */
static int _parse_public_key_cose(CborValue *it, ctap_public_key_cose_t *cose_key);

/**
 * @brief Parse CBOR encoded fixed length array into dst
 */
static int _parse_fixed_len_byte_array(CborValue *map, uint8_t *dst,
                                       size_t *len);

/**
 * @brief Parse CBOR encoded unknown length array into dst
 */
static int _parse_byte_array(CborValue *it, uint8_t *dst, size_t *len);

/**
 * @brief Parse CBOR encoded unknown length array into dst
 */
static int _parse_byte_array_u8len(CborValue *it, uint8_t *dst, uint8_t *len);

/**
 * @brief Parse CBOR encoded string into dst
 */
static int _parse_text_string(CborValue *it, char *dst, size_t *len);
/**
 * @brief Parse CBOR encoded string into dst
 */
static int _parse_text_string_u8len(CborValue *it, char *dst, uint8_t *len);

/**
 * @brief Parse CBOR encoded int into num
 */
static int _parse_int(CborValue *it, int *num);

/**
 * @brief Parse credential description
 */
static int _fido2_ctap_cbor_parse_cred_desc(CborValue *arr, ctap_cred_desc_alt_t *cred);

/**
 * @brief Encode public key into COSE_KEY format
 *
 * See https://tools.ietf.org/html/rfc8152#page-34 Section 13.1.1 for details.
 */
static int _encode_public_key_cose(CborEncoder *cose_key, const ctap_public_key_cose_t *key);

/**
 * @brief Encode PublicKeyCredentialDescriptor into CBOR format
 */
static int _encode_credential(CborEncoder *encoder, const void *cred_ptr,
                              bool rk);

/**
 * @brief Encode PublicKeyCredentialUserEntity into CBOR format
 */
static int _encode_user_entity(CborEncoder *it, const ctap_resident_key_t *rk);

/**
 * @brief CBOR encoder
 */
CborEncoder _encoder;

size_t fido2_ctap_cbor_get_buffer_size(const uint8_t *buf)
{
    return cbor_encoder_get_buffer_size(&_encoder, buf);
}

void fido2_ctap_cbor_init_encoder(uint8_t *buf, size_t len)
{
    cbor_encoder_init(&_encoder, buf, len, 0);
}

int fido2_ctap_cbor_encode_info(const ctap_info_t *info)
{
    int ret;
    size_t sz = 0;
    CborEncoder map;
    CborEncoder map2;
    CborEncoder array;
    CborEncoder array2;

    /* CTAP_CBOR_INFO_MAP_SZ - 1 due to no extensions being supported atm */
    ret = cbor_encoder_create_map(&_encoder, &map, CTAP_CBOR_INFO_MAP_SZ - 1);
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
    ret = cbor_encode_uint(&map, CTAP_CBOR_GET_INFO_RESP_VERSIONS);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_create_array(&map, &array, sz);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    if (info->versions & CTAP_VERSION_FLAG_FIDO) {
        ret = cbor_encode_text_stringz(&array, CTAP_CBOR_VERSION_STRING_FIDO);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->versions & CTAP_VERSION_FLAG_FIDO_PRE) {
        ret = cbor_encode_text_stringz(&array, CTAP_CBOR_VERSION_STRING_FIDO_PRE);
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
    ret = cbor_encode_uint(&map, CTAP_CBOR_GET_INFO_RESP_AAGUID);
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
    ret = cbor_encode_uint(&map, CTAP_CBOR_GET_INFO_RESP_OPTIONS);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_create_map(&map, &map2, sz);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    if (info->options & CTAP_INFO_OPTIONS_FLAG_RK) {
        ret =
            cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_RK,
                                    strlen(CTAP_GET_INFO_RESP_OPTIONS_ID_RK));
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, true);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_UP) {
        ret =
            cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_UP,
                                    strlen(CTAP_GET_INFO_RESP_OPTIONS_ID_UP));
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, true);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    /* default for up is true so need to set false explicitly if not supported */
    else {
        ret =
            cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_UP,
                                    strlen(CTAP_GET_INFO_RESP_OPTIONS_ID_UP));
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, false);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_PLAT) {
        ret = cbor_encode_text_string(&map2, CTAP_GET_INFO_RESP_OPTIONS_ID_PLAT,
                                      strlen(CTAP_GET_INFO_RESP_OPTIONS_ID_PLAT));
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_boolean(&map2, true);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }
    if (info->options & CTAP_INFO_OPTIONS_FLAG_CLIENT_PIN) {
        ret = cbor_encode_text_string(&map2,
                                      CTAP_GET_INFO_RESP_OPTIONS_ID_CLIENT_PIN,
                                      strlen(CTAP_GET_INFO_RESP_OPTIONS_ID_CLIENT_PIN));
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        if (info->pin_is_set) {
            ret = cbor_encode_boolean(&map2, true);
            if (ret != CborNoError) {
                return CTAP2_ERR_CBOR_PARSING;
            }
        }
        else {
            ret = cbor_encode_boolean(&map2, false);
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
    ret = cbor_encode_uint(&map, CTAP_CBOR_GET_INFO_RESP_MAX_MSG_SIZE);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_uint(&map, info->max_msg_size);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* encode pinProtocols */
    ret = cbor_encode_uint(&map, CTAP_CBOR_GET_INFO_RESP_PIN_PROTOCOLS);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encoder_create_array(&map, &array2, CTAP_AMT_SUP_PIN_VER);
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

    ret = cbor_encoder_close_container(&_encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_assertion_object(const ctap_auth_data_header_t *auth_data,
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

    ret = cbor_encoder_create_map(&_encoder, &map, map_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CBOR_GA_RESP_CREDENTIAL);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /**
     * encode credential
     * if not a resident key encrypt key and store it in credential id
     */
    if (!is_resident) {
        ret = fido2_ctap_encrypt_rk(rk, rk->cred_desc.nonce,
                                    sizeof(rk->cred_desc.nonce), &id);

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = _encode_credential(&map, (void *)&id, false);
    }
    else {
        ret = _encode_credential(&map, (void *)&rk->cred_desc, true);
    }

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* encode auth data */
    ret = cbor_encode_int(&map, CTAP_CBOR_GA_RESP_AUTH_DATA);
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
    ret = cbor_encode_int(&map, CTAP_CBOR_GA_RESP_SIGNATURE);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_byte_string(&map, sig_buf, sig_buf_len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* user_id mandatory if resident credential */
    if (is_resident) {
        ret = cbor_encode_int(&map, CTAP_CBOR_GA_RESP_USER);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = _encode_user_entity(&map, rk);
        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    /* if more than 1 valid credential found, encode amount of eligible creds found */
    if (valid_cred_count > 1) {
        ret = cbor_encode_int(&map, CTAP_CBOR_GA_RESP_NUMBER_OF_CREDENTIALS);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
        ret = cbor_encode_int(&map, valid_cred_count);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    ret = cbor_encoder_close_container(&_encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_attestation_object(const ctap_auth_data_t *auth_data,
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
    uint8_t auth_data_buf[CTAP_CBOR_ATT_STMT_AUTH_DATA_SZ] = { 0 };
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

    ret = cbor_encoder_create_map(&_encoder, &map, CTAP_CBOR_ATTESTATION_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* packed attestation format (webauthn specification (version 20190304) section 8.2) */
    ret = cbor_encode_int(&map, CTAP_CBOR_MC_RESP_FMT);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_text_string(&map, CTAP_CBOR_STR_PACKED, strlen(CTAP_CBOR_STR_PACKED));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* move rp id hash, flash and counter into authenticator data buffer */
    memcpy(auth_data_buf, (void *)&auth_data->header,
           sizeof(ctap_auth_data_header_t));
    offset += sizeof(ctap_auth_data_header_t);

    /* move attested credential data header into authenticator data buffer  */
    memcpy(auth_data_buf + offset, (void *)cred_header, cred_header_sz);
    offset += cred_header_sz;

    cose_key_buf = auth_data_buf + offset;

    cbor_encoder_init(&cose_key, cose_key_buf, sizeof(auth_data_buf) - offset,
                      0);

    /* encode credential public key into COSE format */
    ret = _encode_public_key_cose(&cose_key, &auth_data->attested_cred_data.key);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    offset += cbor_encoder_get_buffer_size(&cose_key, cose_key_buf);

    /* encode the authenticator data */
    ret = cbor_encode_int(&map, CTAP_CBOR_MC_RESP_AUTH_DATA);
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
    ret = cbor_encode_int(&map, CTAP_CBOR_MC_RESP_ATT_STMT);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_create_map(&map, &attest_stmt_map, CTAP_CBOR_ATTESTATION_STMT_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&attest_stmt_map, CTAP_CBOR_STR_ALG, strlen(CTAP_CBOR_STR_ALG));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }
    ret = cbor_encode_int(&attest_stmt_map, CTAP_COSE_ALG_ES256);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&attest_stmt_map, CTAP_CBOR_STR_SIG, strlen(CTAP_CBOR_STR_SIG));
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

    ret = cbor_encoder_close_container(&_encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    /* todo: extensions once implemented */

    return CTAP2_OK;
}

static int _encode_credential(CborEncoder *encoder, const void *cred_ptr,
                              bool rk)
{
    CborEncoder desc;
    int ret;

    ret = cbor_encoder_create_map(encoder, &desc, CTAP_CBOR_CRED_DESC_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&desc, CTAP_CBOR_STR_ID, strlen(CTAP_CBOR_STR_ID));
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

    ret = cbor_encode_text_string(&desc, CTAP_CBOR_STR_TYPE, strlen(CTAP_CBOR_STR_TYPE));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret =
        cbor_encode_text_string(&desc, CTAP_CBOR_STR_PUBLIC_KEY, strlen(CTAP_CBOR_STR_PUBLIC_KEY));
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(encoder, &desc);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_key_agreement(const ctap_public_key_cose_t *key)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(&_encoder, &map, CTAP_CBOR_KEY_AGREEMENT_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CBOR_CP_RESP_KEY_AGREEMENT);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = _encode_public_key_cose(&map, key);

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = cbor_encoder_close_container(&_encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_retries(uint8_t tries_left)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(&_encoder, &map, CTAP_CBOR_RETRIES_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CBOR_CP_RETRIES_RESP);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, (int)tries_left);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(&_encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_encode_pin_token(uint8_t *token, size_t len)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(&_encoder, &map, CTAP_CBOR_PIN_TOKEN_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_int(&map, CTAP_CBOR_CP_PIN_TOKEN_RESP);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_byte_string(&map, token, len);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encoder_close_container(&_encoder, &map);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

static int _encode_user_entity(CborEncoder *encoder,
                               const ctap_resident_key_t *rk)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(encoder, &map, CTAP_CBOR_USER_ENTITY_MAP_SZ);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_encode_text_string(&map, CTAP_CBOR_STR_ID, strlen(CTAP_CBOR_STR_ID));
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

static int _encode_public_key_cose(CborEncoder *cose_key, const ctap_public_key_cose_t *key)
{
    int ret;
    CborEncoder map;

    ret = cbor_encoder_create_map(cose_key, &map, CTAP_CBOR_COSE_KEY_MAP_SZ);
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
                                            const uint8_t *req_raw, size_t len)
{
    uint8_t required_parsed = 0;
    int ret;
    int key;
    int tmp;
    size_t map_len;
    CborParser parser;
    CborValue it;
    CborValue map;
    CborType type;

    ret = cbor_parser_init(req_raw, len, 0, &parser, &it);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_validate(&it, CborValidateCanonicalFormat);
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
        case CTAP_CBOR_GA_REQ_RP_ID:
            DEBUG("ctap_cbor: parse rp_id \n");
            req->rp_id_len = CTAP_DOMAIN_NAME_MAX_SIZE;
            ret = _parse_text_string_u8len(&map, (char *)req->rp_id,
                                           &req->rp_id_len);
            required_parsed++;
            break;
        case CTAP_CBOR_GA_REQ_CLIENT_DATA_HASH:
            DEBUG("ctap_cbor: parse client_data_hash \n");
            len = SHA256_DIGEST_LENGTH;
            ret =
                _parse_fixed_len_byte_array(&map, req->client_data_hash, &len);
            required_parsed++;
            break;
        case CTAP_CBOR_GA_REQ_ALLOW_LIST:
            DEBUG("ctap_cbor: parse allow_list \n");
            ret = _parse_allow_list(&map, req->allow_list,
                                    &req->allow_list_len);
            break;
        case CTAP_CBOR_GA_REQ_EXTENSIONS:
            /* todo: implement once extensions are supported */
            DEBUG("ctap_cbor: parse extensions \n");
            break;
        case CTAP_CBOR_GA_REQ_OPTIONS:
            DEBUG("ctap_cbor: parse options \n");
            ret = _parse_options(&map, &req->options);
            break;
        case CTAP_CBOR_GA_REQ_PIN_AUTH:
            DEBUG("ctap_cbor: parse pin_auth \n");
            len = 16;
            ret = _parse_fixed_len_byte_array(&map, req->pin_auth, &len);
            /* CTAP specification (version 20190130) section 5.5.8.1 */
            if (ret == CTAP1_ERR_INVALID_LENGTH && len == 0) {
                ret = CTAP2_OK;
            }
            req->pin_auth_len = len;
            req->pin_auth_present = true;
            break;
        case CTAP_CBOR_GA_REQ_PIN_PROTOCOL:
            DEBUG("ctap_cbor: parse pin_protocol \n");
            ret = _parse_int(&map, &tmp);
            req->pin_protocol = (uint8_t)tmp;
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

    /* rpId and clientDataHash are required */
    if (required_parsed != 2) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_parse_client_pin_req(ctap_client_pin_req_t *req,
                                         const uint8_t *req_raw, size_t len)
{
    uint8_t required_parsed = 0;
    int ret;
    int key;
    int cbor_type;
    int tmp;
    size_t map_len;
    CborParser parser;
    CborValue it;
    CborValue map;

    ret = cbor_parser_init(req_raw, len, 0, &parser, &it);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_validate(&it, CborValidateCanonicalFormat);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    cbor_type = cbor_value_get_type(&it);
    if (cbor_type != CborMapType) {
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
        cbor_type = cbor_value_get_type(&map);
        if (cbor_type != CborIntegerType) {
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
        case CTAP_CBOR_CP_REQ_PIN_PROTOCOL:
            DEBUG("ctap_cbor: parse pinProtocol \n");
            ret = _parse_int(&map, &tmp);
            req->pin_protocol = (uint8_t)tmp;
            required_parsed++;
            break;
        case CTAP_CBOR_CP_REQ_SUB_COMMAND:
            DEBUG("ctap_cbor: parse subCommand \n");
            ret = _parse_int(&map, &tmp);
            req->sub_command = (uint8_t)tmp;
            required_parsed++;
            break;
        case CTAP_CBOR_CP_REQ_KEY_AGREEMENT:
            DEBUG("ctap_cbor: parse keyAgreement \n");
            ret = _parse_public_key_cose(&map, &req->key_agreement);
            req->key_agreement_present = true;
            break;
        case CTAP_CBOR_CP_REQ_PIN_AUTH:
            DEBUG("ctap_cbor: parse pinAuth \n");
            len = sizeof(req->pin_auth);
            ret = _parse_fixed_len_byte_array(&map, req->pin_auth, &len);
            req->pin_auth_present = true;
            break;
        case CTAP_CBOR_CP_REQ_NEW_PIN_ENC:
            DEBUG("ctap_cbor: parse newPinEnc \n");
            len = sizeof(req->new_pin_enc);
            ret = _parse_byte_array(&map, req->new_pin_enc, &len);
            req->new_pin_enc_size = len;
            break;
        case CTAP_CBOR_CP_REQ_PIN_HASH_ENC:
            DEBUG("ctap_cbor: parse pinHashEnc \n");
            len = sizeof(req->pin_hash_enc);
            ret = _parse_fixed_len_byte_array(&map, req->pin_hash_enc, &len);
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

    /* pinProtocol and subCommand are required */
    if (required_parsed != 2) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

int fido2_ctap_cbor_parse_make_credential_req(ctap_make_credential_req_t *req,
                                              const uint8_t *buf,
                                              size_t size)
{
    uint8_t required_parsed = 0;
    int ret;
    int key;
    int tmp;
    size_t len;
    size_t map_len;
    CborParser parser;
    CborValue it;
    CborValue map;
    CborType type;

    ret = cbor_parser_init(buf, size, 0, &parser, &it);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    ret = cbor_value_validate(&it, CborValidateCanonicalFormat);
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
        case CTAP_CBOR_MC_REQ_CLIENT_DATA_HASH:
            DEBUG("ctap_cbor: parse clientDataHash \n");
            len = SHA256_DIGEST_LENGTH;
            ret =
                _parse_fixed_len_byte_array(&map, req->client_data_hash, &len);
            required_parsed++;
            break;
        case CTAP_CBOR_MC_REQ_RP:
            DEBUG("ctap_cbor: parse rp \n");
            ret = _parse_entity(&map, &req->rp, RP);
            required_parsed++;
            break;
        case CTAP_CBOR_MC_REQ_USER:
            DEBUG("ctap_cbor: parse user \n");
            ret = _parse_entity(&map, &req->user, USER);
            required_parsed++;
            break;
        case CTAP_CBOR_MC_REQ_PUB_KEY_CRED_PARAMS:
            DEBUG("ctap_cbor: parse key_cred params \n");
            ret = _parse_pub_key_cred_params(&map, req);
            required_parsed++;
            break;
        case CTAP_CBOR_MC_REQ_EXCLUDE_LIST:
            DEBUG("ctap_cbor: parse excludeList \n");
            ret = _parse_exclude_list(&map, req->exclude_list,
                                      &req->exclude_list_len);
            break;
        case CTAP_CBOR_MC_REQ_EXTENSIONS:
            DEBUG("ctap_cbor: parse exclude_list \n");
            ret = CTAP2_ERR_UNSUPPORTED_EXTENSION;
            break;
        case CTAP_CBOR_MC_REQ_OPTIONS:
            DEBUG("ctap_cbor: parse options \n");
            ret = _parse_options(&map, &req->options);
            break;
        case CTAP_CBOR_MC_REQ_PIN_AUTH:
            DEBUG("ctap_cbor: parse pin_auth \n");
            len = 16;
            ret = _parse_fixed_len_byte_array(&map, req->pin_auth, &len);
            /* CTAP specification (version 20190130) section 5.5.8.1 (pinAuth) */
            if (ret == CTAP1_ERR_INVALID_LENGTH && len == 0) {
                ret = CTAP2_OK;
            }
            req->pin_auth_len = len;
            req->pin_auth_present = true;
            break;
        case CTAP_CBOR_MC_REQ_PIN_PROTOCOL:
            DEBUG("ctap_cbor: parse pin_protocol \n");
            ret = _parse_int(&map, &tmp);
            req->pin_protocol = (uint8_t)tmp;
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

    /* clientDataHash, rp, user and pubKeyCredParams are required */
    if (required_parsed != 4) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

static int _parse_public_key_cose(CborValue *it, ctap_public_key_cose_t *cose_key)
{
    int ret;
    int type;
    int key;
    int tmp;
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

        ret = _parse_int(&map, &key);

        if (ret != CTAP2_OK) {
            return ret;
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        switch (key) {
        case CTAP_COSE_KEY_LABEL_KTY:
            ret = _parse_int(&map, &cose_key->kty);
            required_parsed++;
            break;
        case CTAP_COSE_KEY_LABEL_ALG:
            ret = _parse_int(&map, &tmp);
            cose_key->alg_type = (int32_t)tmp;
            required_parsed++;
            break;
        case CTAP_COSE_KEY_LABEL_CRV:
            ret = _parse_int(&map, &cose_key->crv);
            required_parsed++;
            break;
        case CTAP_COSE_KEY_LABEL_X:
            len = sizeof(cose_key->pubkey.x);
            ret = _parse_fixed_len_byte_array(&map, cose_key->pubkey.x, &len);
            required_parsed++;
            break;
        case CTAP_COSE_KEY_LABEL_Y:
            len = sizeof(cose_key->pubkey.y);
            ret = _parse_fixed_len_byte_array(&map, cose_key->pubkey.y, &len);
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

static int _parse_entity(CborValue *it, void *entity, entity_type_t type)
{
    int ret;
    int cbor_type;
    size_t map_len;
    size_t key_len;
    size_t len;
    CborValue map;
    char key[CTAP_CBOR_MAP_MAX_KEY_LEN];

    uint8_t required_parsed = 0;

    cbor_type = cbor_value_get_type(it);
    if (cbor_type != CborMapType) {
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
        cbor_type = cbor_value_get_type(&map);
        if (cbor_type != CborTextStringType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        key_len = sizeof(key);
        ret = cbor_value_copy_text_string(&map, key, &key_len, NULL);
        if (ret == CborErrorOutOfMemory) {
            return CTAP2_ERR_LIMIT_EXCEEDED;
        }

        key[sizeof(key) - 1] = '\0';

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        if (strncmp(key, CTAP_CBOR_STR_ID, strlen(CTAP_CBOR_STR_ID)) == 0) {
            if (type == USER) {
                ctap_user_ent_t *user = (ctap_user_ent_t *)entity;

                user->id_len = CTAP_USER_ID_MAX_SIZE;
                ret = _parse_byte_array_u8len(&map, user->id, &user->id_len);
            }
            else {
                ctap_rp_ent_t *rp = (ctap_rp_ent_t *)entity;

                rp->id_len = CTAP_DOMAIN_NAME_MAX_SIZE;
                ret = _parse_text_string_u8len(&map, (char *)rp->id, &rp->id_len);
            }

            if (ret != CTAP2_OK) {
                return ret;
            }
            required_parsed++;
        }
        else if (strncmp(key, CTAP_CBOR_STR_NAME, strlen(CTAP_CBOR_STR_NAME)) == 0) {
            if (type == USER) {
                ctap_user_ent_t *user = (ctap_user_ent_t *)entity;

                len = CTAP_USER_MAX_NAME_SIZE;
                ret = _parse_text_string(&map, (char *)user->name, &len);
            }
            else {
                ctap_rp_ent_t *rp = (ctap_rp_ent_t *)entity;

                len = CTAP_RP_MAX_NAME_SIZE;
                ret = _parse_text_string(&map, (char *)rp->name, &len);
            }

            if (ret != CTAP2_OK) {
                return ret;
            }
        }
        else if (strncmp(key, CTAP_CBOR_STR_ICON, strlen(CTAP_CBOR_STR_ICON)) == 0) {
            len = CTAP_DOMAIN_NAME_MAX_SIZE;

            if (type == USER) {
                ctap_user_ent_t *user = (ctap_user_ent_t *)entity;

                ret = _parse_text_string(&map, (char *)user->icon, &len);
            }
            else {
                ctap_rp_ent_t *rp = (ctap_rp_ent_t *)entity;

                ret = _parse_text_string(&map, (char *)rp->icon, &len);
            }

            if (ret != CTAP2_OK) {
                return ret;
            }
        }
        else if (strncmp(key, CTAP_CBOR_DISPLAY_NAME, strlen(CTAP_CBOR_DISPLAY_NAME)) == 0) {
            if (type == USER) {
                ctap_user_ent_t *user = (ctap_user_ent_t *)entity;

                len = CTAP_USER_MAX_NAME_SIZE;
                ret = _parse_text_string(&map, (char *)user->display_name, &len);
                if (ret != CTAP2_OK) {
                    return ret;
                }
            }
        }
        else {
            DEBUG("parse entity: ignoring unknown key: %s \n", key);
        }

        ret = cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    /* userId / rpId is required */
    if (required_parsed != 1) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    return CTAP2_OK;
}

static int _parse_pub_key_cred_params(CborValue *it,
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
        ret = _parse_pub_key_cred_param(&arr, &cred_type, &alg_type);
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

static int _parse_pub_key_cred_param(CborValue *it, uint8_t *cred_type,
                                     int32_t *alg_type)
{
    int ret;
    int cbor_type;
    char cred_type_str[CTAP_CBOR_MAX_CREDENTIAL_TYPE_LEN];
    size_t cred_type_str_len = sizeof(cred_type_str);
    CborValue cred, alg;

    cbor_type = cbor_value_get_type(it);
    if (cbor_type != CborMapType) {
        return CTAP2_ERR_INVALID_CBOR_TYPE;
    }

    ret = cbor_value_map_find_value(it, CTAP_CBOR_STR_TYPE, &cred);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    cbor_type = cbor_value_get_type(&cred);
    if (cbor_type != CborTextStringType) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    ret = cbor_value_map_find_value(it, CTAP_CBOR_STR_ALG, &alg);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    cbor_type = cbor_value_get_type(&alg);
    if (cbor_type != CborIntegerType) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    ret = cbor_value_copy_text_string(&cred, cred_type_str, &cred_type_str_len, NULL);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    cred_type_str[sizeof(cred_type_str) - 1] = '\0';

    if (strncmp(cred_type_str, CTAP_CBOR_STR_PUBLIC_KEY, strlen(CTAP_CBOR_STR_PUBLIC_KEY)) == 0) {
        *cred_type = CTAP_PUB_KEY_CRED_PUB_KEY;
    }
    else {
        *cred_type = CTAP_PUB_KEY_CRED_UNKNOWN;
    }

    ret = cbor_value_get_int_checked(&alg, (int *)alg_type);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

static int _parse_options(CborValue *it, ctap_options_t *options)
{
    int ret;
    int cbor_type;
    char key[CTAP_CBOR_MAP_MAX_KEY_LEN];
    size_t key_len = sizeof(key);
    size_t map_len;
    bool option_value;
    CborValue map;

    cbor_type = cbor_value_get_type(it);
    if (cbor_type != CborMapType) {
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
        cbor_type = cbor_value_get_type(&map);
        if (cbor_type != CborTextStringType) {
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

        cbor_type = cbor_value_get_type(&map);
        if (cbor_type != CborBooleanType) {
            return CTAP2_ERR_INVALID_CBOR_TYPE;
        }

        /* get boolean value of options parameter */
        ret = cbor_value_get_boolean(&map, &option_value);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }

        if (strncmp(key, CTAP_CBOR_STR_RESIDENT_KEY, strlen(CTAP_CBOR_STR_RESIDENT_KEY)) == 0) {
            options->rk = option_value;
        }
        else if (strncmp(key, CTAP_CBOR_STR_USER_VERIFIED,
                         strlen(CTAP_CBOR_STR_USER_VERIFIED)) == 0) {
            options->uv = option_value;
        }
        else if (strncmp(key, CTAP_CBOR_STR_USER_PRESENT,
                         strlen(CTAP_CBOR_STR_USER_PRESENT)) == 0) {
            options->up = option_value;
        }
        else {
            /* ignore unknown options */
            DEBUG("Ctap parse options, unknown option: %s \n", key);
        }

        cbor_value_advance(&map);
        if (ret != CborNoError) {
            return CTAP2_ERR_CBOR_PARSING;
        }
    }

    return CTAP2_OK;
}

static int _parse_allow_list(CborValue *it, ctap_cred_desc_alt_t *allow_list,
                             uint8_t *allow_list_len)
{
    size_t len2 = *allow_list_len;
    int retval = _parse_exclude_list(it, allow_list, &len2);

    *allow_list_len = (uint8_t)len2;
    return retval;
}

static int _parse_exclude_list(CborValue *it, ctap_cred_desc_alt_t *exclude_list,
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
        ret = _fido2_ctap_cbor_parse_cred_desc(&arr, &exclude_list[i]);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    return CTAP2_OK;
}

static int _fido2_ctap_cbor_parse_cred_desc(CborValue *arr, ctap_cred_desc_alt_t *cred)
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

    ret = cbor_value_map_find_value(arr, CTAP_CBOR_STR_TYPE, &val);
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

    if (strncmp(type_str, CTAP_CBOR_STR_PUBLIC_KEY, strlen(CTAP_CBOR_STR_PUBLIC_KEY)) == 0) {
        cred->cred_type = CTAP_PUB_KEY_CRED_PUB_KEY;
    }
    else {
        cred->cred_type = CTAP_PUB_KEY_CRED_UNKNOWN;
    }

    ret = cbor_value_map_find_value(arr, CTAP_CBOR_STR_ID, &val);
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

    ret = cbor_value_advance(arr);
    if (ret != CborNoError) {
        return CTAP2_ERR_CBOR_PARSING;
    }

    return CTAP2_OK;
}

static int _parse_fixed_len_byte_array(CborValue *it, uint8_t *dst, size_t *len)
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

static int _parse_byte_array(CborValue *it, uint8_t *dst, size_t *len)
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

static int _parse_byte_array_u8len(CborValue *it, uint8_t *dst, uint8_t *len)
{
    size_t len2 = *len;
    int retval = _parse_byte_array(it, dst, &len2);

    *len = (uint8_t)len2;
    return retval;
}

static int _parse_text_string(CborValue *it, char *dst, size_t *len)
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

static int _parse_text_string_u8len(CborValue *it, char *dst, uint8_t *len)
{
    size_t len2 = *len;
    int retval = _parse_text_string(it, dst, &len2);

    *len = (uint8_t)len2;
    return retval;
}

static int _parse_int(CborValue *it, int *num)
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
