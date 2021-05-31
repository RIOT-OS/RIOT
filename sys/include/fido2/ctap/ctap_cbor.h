/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP CBOR helper
 *
 * @{
 *
 * @file
 * @brief       CTAP2 CBOR helper function declarations
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#ifndef FIDO2_CTAP_CTAP_CBOR_H
#define FIDO2_CTAP_CTAP_CBOR_H

#include "fido2/ctap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CBOR map size of encoded getInfo response
 */
#define CTAP_CBOR_INFO_MAP_SZ 0x06

/**
 * @brief CBOR map size of encoded attestation
 */
#define CTAP_CBOR_ATTESTATION_MAP_SZ 0x03

/**
 * @brief CBOR map size of encoded attestation statement
 */
#define CTAP_CBOR_ATTESTATION_STMT_MAP_SZ 0x02

/**
 * @brief CBOR map size of encoded credential description
 */
#define CTAP_CBOR_CRED_DESC_MAP_SZ 0x02

/**
 * @brief CBOR map size of encoded pinToken
 */
#define CTAP_CBOR_PIN_TOKEN_MAP_SZ 0x01

/**
 * @brief CBOR map size of encoded user entity
 */
#define CTAP_CBOR_USER_ENTITY_MAP_SZ 0x01

/**
 * @brief CBOR map size of public key encoded in COSE format
 */
#define CTAP_CBOR_COSE_KEY_MAP_SZ 0x05

/**
 * @brief CBOR map size of encoded clientPIN keyAgreement
 */
#define CTAP_CBOR_KEY_AGREEMENT_MAP_SZ 0x01

/**
 * @brief CBOR map size of encoded clientPIN retries
 */
#define CTAP_CBOR_RETRIES_MAP_SZ 0x01

/**
 * @brief Attestation statement data buffer size
 */
#define CTAP_CBOR_ATT_STMT_AUTH_DATA_SZ 0x12c

/**
 * @brief Max length of string key in CBOR map
 */
#define CTAP_CBOR_MAP_MAX_KEY_LEN 0x10

/**
 * @brief Max length of PublicKeyCredentialType string
 */
#define CTAP_CBOR_MAX_CREDENTIAL_TYPE_LEN 0x10

/**
 * @name CTAP CBOR map key string values
 * @{
 */
#define CTAP_CBOR_STR_PACKED "packed"
#define CTAP_CBOR_STR_ALG "alg"
#define CTAP_CBOR_STR_SIG "sig"
#define CTAP_CBOR_STR_ID "id"
#define CTAP_CBOR_STR_TYPE "type"
#define CTAP_CBOR_STR_PUBLIC_KEY "public-key"
#define CTAP_CBOR_STR_USER_VERIFIED "uv"
#define CTAP_CBOR_STR_USER_PRESENT "up"
#define CTAP_CBOR_STR_RESIDENT_KEY "rk"
#define CTAP_CBOR_STR_NAME "name"
#define CTAP_CBOR_STR_ICON "icon"
#define CTAP_CBOR_DISPLAY_NAME "displayName"
/** @} */

/**
 * @name CTAP Client PIN response CBOR map key values
 *
 * @{
 */
#define CTAP_CBOR_CP_RESP_KEY_AGREEMENT  0x01
#define CTAP_CBOR_CP_PIN_TOKEN_RESP      0x02
#define CTAP_CBOR_CP_RETRIES_RESP        0x03
/** @} */

/**
 * @name CTAP make credential request CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_MC_REQ_CLIENT_DATA_HASH    0x01
#define CTAP_CBOR_MC_REQ_RP                  0x02
#define CTAP_CBOR_MC_REQ_USER                0x03
#define CTAP_CBOR_MC_REQ_PUB_KEY_CRED_PARAMS 0x04
#define CTAP_CBOR_MC_REQ_EXCLUDE_LIST        0x05
#define CTAP_CBOR_MC_REQ_EXTENSIONS          0x06
#define CTAP_CBOR_MC_REQ_OPTIONS             0x07
#define CTAP_CBOR_MC_REQ_PIN_AUTH            0x08
#define CTAP_CBOR_MC_REQ_PIN_PROTOCOL        0x09
/** @} */

/**
 * @name CTAP get info response CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_GET_INFO_RESP_VERSIONS         0x01
#define CTAP_CBOR_GET_INFO_RESP_EXTENSIONS       0x02
#define CTAP_CBOR_GET_INFO_RESP_AAGUID           0x03
#define CTAP_CBOR_GET_INFO_RESP_OPTIONS          0x04
#define CTAP_CBOR_GET_INFO_RESP_MAX_MSG_SIZE     0x05
#define CTAP_CBOR_GET_INFO_RESP_PIN_PROTOCOLS    0x06
/** @} */

/**
 * @name CTAP version strings
 * @{
 */
#define CTAP_CBOR_VERSION_STRING_FIDO_PRE "FIDO_2_1_PRE"
#define CTAP_CBOR_VERSION_STRING_FIDO     "FIDO_2_0"
#define CTAP_CBOR_VERSION_STRING_U2F_V2   "U2F_V2"
/** @} */

/**
 * @name CTAP make credential response CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_MC_RESP_FMT                0x01
#define CTAP_CBOR_MC_RESP_AUTH_DATA          0x02
#define CTAP_CBOR_MC_RESP_ATT_STMT           0x03
/** @} */

/**
 * @name CTAP get assertion request CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_GA_REQ_RP_ID               0x01
#define CTAP_CBOR_GA_REQ_CLIENT_DATA_HASH    0x02
#define CTAP_CBOR_GA_REQ_ALLOW_LIST          0x03
#define CTAP_CBOR_GA_REQ_EXTENSIONS          0x04
#define CTAP_CBOR_GA_REQ_OPTIONS             0x05
#define CTAP_CBOR_GA_REQ_PIN_AUTH            0x06
#define CTAP_CBOR_GA_REQ_PIN_PROTOCOL        0x07
/** @} */

/**
 * @name CTAP get assertion response CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_GA_RESP_CREDENTIAL             0x01
#define CTAP_CBOR_GA_RESP_AUTH_DATA              0x02
#define CTAP_CBOR_GA_RESP_SIGNATURE              0x03
#define CTAP_CBOR_GA_RESP_USER                   0x04
#define CTAP_CBOR_GA_RESP_NUMBER_OF_CREDENTIALS  0x05
/** @} */

/**
 * @name CTAP Client PIN request CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_CP_REQ_PIN_PROTOCOL    0x01
#define CTAP_CBOR_CP_REQ_SUB_COMMAND     0x02
#define CTAP_CBOR_CP_REQ_KEY_AGREEMENT   0x03
#define CTAP_CBOR_CP_REQ_PIN_AUTH        0x04
#define CTAP_CBOR_CP_REQ_NEW_PIN_ENC     0x05
#define CTAP_CBOR_CP_REQ_PIN_HASH_ENC    0x06
/** @} */

/**
 * @brief Parse MakeCredential method
 *
 * CTAP specification (version 20190130) section 5.1
 *
 * @param[in] req       struct to parse into
 * @param[in] req_raw   raw request
 * @param[in] size      size of raw request
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_parse_make_credential_req(ctap_make_credential_req_t *req,
                                              const uint8_t *req_raw, size_t size);

/**
 * @brief Parse GetAssertion method
 *
 * CTAP specification (version 20190130) section 5.2
 *
 * @param[in] req       struct to parse into
 * @param[in] req_raw   raw request
 * @param[in] size      size of raw request
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_parse_get_assertion_req(ctap_get_assertion_req_t *req,
                                            const uint8_t *req_raw, size_t size);

/**
 * @brief Encode CBOR info map
 *
 * CTAP specification (version 20190130) section 5.4
 *
 * @param[in] encoder   CBOR encoder
 * @param[in] info      information about capabilities
 *
 * @return CTAP status code
 */

int fido2_ctap_cbor_encode_info(CborEncoder *encoder, const ctap_info_t *info);
/**
 * @brief Parse ClientPIN method
 *
 * CTAP specification (version 20190130) section 5.5
 *
 * @param[in] req       struct to parse into
 * @param[in] req_raw   raw request
 * @param[in] size      size of raw request
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_parse_client_pin_req(ctap_client_pin_req_t *req,
                                         const uint8_t *req_raw, size_t size);
/**
 * @brief Encode attestation object
 *
 * Webauthn specification (version 20190304) section 6.5
 *
 * @param[in] encoder           CBOR encoder
 * @param[in] auth_data         authenticator data
 * @param[in] client_data_hash  SHA-256 hash of JSON serialized client data
 * @param[in] rk                resident key
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_encode_attestation_object(CborEncoder *encoder,
                                              const ctap_auth_data_t *auth_data,
                                              const uint8_t *client_data_hash,
                                              ctap_resident_key_t *rk);

/**
 * @brief Encode assertion object
 *
 * CTAP specification (version 20190130) section 5.2
 *
 * @param[in] encoder           CBOR encoder
 * @param[in] auth_data         authenticator data header
 * @param[in] client_data_hash  SHA-256 hash of JSON serialized client data
 * @param[in] rk                resident key
 * @param[in] valid_cred_count  amount of valid credentials found in allow list
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_encode_assertion_object(CborEncoder *encoder,
                                            const ctap_auth_data_header_t *auth_data,
                                            const uint8_t *client_data_hash,
                                            ctap_resident_key_t *rk,
                                            uint8_t valid_cred_count);
/**
 * @brief Encode key agreement
 *
 * @param[in] encoder           CBOR encoder
 * @param[in] key               Pub key in COSE format
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_encode_key_agreement(CborEncoder *encoder,
                                         const ctap_public_key_cose_t *key);

/**
 * @brief Encode encrypted pin token
 *
 * @param[in] encoder           CBOR encoder
 * @param[in] token             encrypted pin token
 * @param[in] size              size of encrypted pin token
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_encode_pin_token(CborEncoder *encoder, uint8_t *token, size_t size);

/**
 * @brief Encode PIN tries left
 *
 * @param[in] encoder            CBOR encoder
 * @param[in] tries_left         amount of tries left
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_encode_retries(CborEncoder *encoder, uint8_t tries_left);

/**
 * @brief Parse credential description
 *
 * @param[in] arr   CBOR array
 * @param[in] cred  struct to parse into
 *
 * @return CTAP status code
 */
int fido2_ctap_cbor_parse_cred_desc(CborValue *arr, ctap_cred_desc_alt_t *cred);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_CBOR_H */
/** @} */
