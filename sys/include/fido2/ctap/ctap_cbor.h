/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap_cbor FIDO2 CTAP CBOR
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP CBOR helper
 *
 * @{
 *
 * @file
 * @brief       CTAP CBOR helper function declarations
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#ifndef FIDO2_CTAP_CTAP_CBOR_H
#define FIDO2_CTAP_CTAP_CBOR_H

#include "fido2/ctap/ctap.h"

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
#define CTAP_CBOR_ATT_STMT_AUTH_DATA_SZ 0x134

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
#define CTAP_CBOR_STR_PACKED "packed"           /**< packed key string */
#define CTAP_CBOR_STR_ALG "alg"                 /**< algorithm key string */
#define CTAP_CBOR_STR_SIG "sig"                 /**< signature key string */
#define CTAP_CBOR_STR_ID "id"                   /**< id key string */
#define CTAP_CBOR_STR_TYPE "type"               /**< type key string */
#define CTAP_CBOR_STR_PUBLIC_KEY "public-key"   /**< public-key key string */
#define CTAP_CBOR_STR_USER_VERIFIED "uv"        /**< user verification key string */
#define CTAP_CBOR_STR_USER_PRESENT "up"         /**< user presence key string */
#define CTAP_CBOR_STR_RESIDENT_KEY "rk"         /**< resident key key string */
#define CTAP_CBOR_STR_NAME "name"               /**< name key string */
#define CTAP_CBOR_STR_ICON "icon"               /**< icon key string */
#define CTAP_CBOR_DISPLAY_NAME "displayName"    /**< displayName key string */
/** @} */

/**
 * @name CTAP Client PIN response CBOR map key values
 *
 * @{
 */
#define CTAP_CBOR_CP_RESP_KEY_AGREEMENT  0x01   /**< KeyAgreement key value */
#define CTAP_CBOR_CP_PIN_TOKEN_RESP      0x02   /**< pinToken key value */
#define CTAP_CBOR_CP_RETRIES_RESP        0x03   /**< retries key value */
/** @} */

/**
 * @name CTAP make credential request CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_MC_REQ_CLIENT_DATA_HASH    0x01   /**< clientDataHash key value */
#define CTAP_CBOR_MC_REQ_RP                  0x02   /**< relying party key value */
#define CTAP_CBOR_MC_REQ_USER                0x03   /**< user key value */
#define CTAP_CBOR_MC_REQ_PUB_KEY_CRED_PARAMS 0x04   /**< pubKeyCredParams key value */
#define CTAP_CBOR_MC_REQ_EXCLUDE_LIST        0x05   /**< excludeList key value */
#define CTAP_CBOR_MC_REQ_EXTENSIONS          0x06   /**< extensions key value */
#define CTAP_CBOR_MC_REQ_OPTIONS             0x07   /**< options key value */
#define CTAP_CBOR_MC_REQ_PIN_AUTH            0x08   /**< pinAuth key value */
#define CTAP_CBOR_MC_REQ_PIN_PROTOCOL        0x09   /**< pinProtocol key value */
/** @} */

/**
 * @name CTAP get info response CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_GET_INFO_RESP_VERSIONS         0x01   /**< versions key value */
#define CTAP_CBOR_GET_INFO_RESP_EXTENSIONS       0x02   /**< extensions key value */
#define CTAP_CBOR_GET_INFO_RESP_AAGUID           0x03   /**< AAGUID key value */
#define CTAP_CBOR_GET_INFO_RESP_OPTIONS          0x04   /**< options key value */
#define CTAP_CBOR_GET_INFO_RESP_MAX_MSG_SIZE     0x05   /**< maxMsgSize key value */
#define CTAP_CBOR_GET_INFO_RESP_PIN_PROTOCOLS    0x06   /**< pinProtocol key value */
/** @} */

/**
 * @name CTAP version strings
 * @{
 */
#define CTAP_CBOR_VERSION_STRING_FIDO_PRE "FIDO_2_1_PRE"    /**< FIDO 2.1 flag */
#define CTAP_CBOR_VERSION_STRING_FIDO     "FIDO_2_0"        /**< FIDO 2 flag */
#define CTAP_CBOR_VERSION_STRING_U2F_V2   "U2F_V2"          /**< U2F V2 flag */
/** @} */

/**
 * @name CTAP make credential response CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_MC_RESP_FMT                0x01   /**< attestation statement format identifier key value */
#define CTAP_CBOR_MC_RESP_AUTH_DATA          0x02   /**< authData key value */
#define CTAP_CBOR_MC_RESP_ATT_STMT           0x03   /**< attestation statement key value */
/** @} */

/**
 * @name CTAP get assertion request CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_GA_REQ_RP_ID               0x01   /**< relying party identifier key value */
#define CTAP_CBOR_GA_REQ_CLIENT_DATA_HASH    0x02   /**< clientDataHash key value */
#define CTAP_CBOR_GA_REQ_ALLOW_LIST          0x03   /**< allowList key value */
#define CTAP_CBOR_GA_REQ_EXTENSIONS          0x04   /**< extensions key value */
#define CTAP_CBOR_GA_REQ_OPTIONS             0x05   /**< options key value */
#define CTAP_CBOR_GA_REQ_PIN_AUTH            0x06   /**< pinAuth key value */
#define CTAP_CBOR_GA_REQ_PIN_PROTOCOL        0x07   /**< pinProtocol key value */
/** @} */

/**
 * @name CTAP get assertion response CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_GA_RESP_CREDENTIAL             0x01   /**< credential key value */
#define CTAP_CBOR_GA_RESP_AUTH_DATA              0x02   /**< authData key value */
#define CTAP_CBOR_GA_RESP_SIGNATURE              0x03   /**< signature key value */
#define CTAP_CBOR_GA_RESP_USER                   0x04   /**< user key value */
#define CTAP_CBOR_GA_RESP_NUMBER_OF_CREDENTIALS  0x05   /**< numberOfCredentials key value */
/** @} */

/**
 * @name CTAP Client PIN request CBOR key values
 *
 * @{
 */
#define CTAP_CBOR_CP_REQ_PIN_PROTOCOL    0x01   /**< pinProtocol key value */
#define CTAP_CBOR_CP_REQ_SUB_COMMAND     0x02   /**< subCommand key value */
#define CTAP_CBOR_CP_REQ_KEY_AGREEMENT   0x03   /**< keyAgreement key value */
#define CTAP_CBOR_CP_REQ_PIN_AUTH        0x04   /**< pinAuth key value */
#define CTAP_CBOR_CP_REQ_NEW_PIN_ENC     0x05   /**< newPinEnc key value */
#define CTAP_CBOR_CP_REQ_PIN_HASH_ENC    0x06   /**< pinHashEnc key value */
/** @} */

/**
 * @brief Parse MakeCredential method
 *
 * CTAP specification (version 20190130) section 5.1
 *
 * @param[in] req       struct to parse into
 * @param[in] req_raw   raw request
 * @param[in] len       length of @p req_raw
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_parse_make_credential_req(ctap_make_credential_req_t *req,
                                              const uint8_t *req_raw, size_t len);

/**
 * @brief Parse GetAssertion method
 *
 * CTAP specification (version 20190130) section 5.2
 *
 * @param[in] req       struct to parse into
 * @param[in] req_raw   raw request
 * @param[in] len       length of @p req_raw
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_parse_get_assertion_req(ctap_get_assertion_req_t *req,
                                            const uint8_t *req_raw, size_t len);

/**
 * @brief Encode CBOR info map
 *
 * CTAP specification (version 20190130) section 5.4
 *
 * @param[in] info      information about capabilities
 *
 * @return @ref ctap_status_code_t
 */

ctap_status_code_t fido2_ctap_cbor_encode_info(const ctap_info_t *info);
/**
 * @brief Parse ClientPIN method
 *
 * CTAP specification (version 20190130) section 5.5
 *
 * @param[in] req       struct to parse into
 * @param[in] req_raw   raw request
 * @param[in] len       length of @p req_raw
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_parse_client_pin_req(ctap_client_pin_req_t *req,
                                         const uint8_t *req_raw, size_t len);
/**
 * @brief Encode attestation object
 *
 * Webauthn specification (version 20190304) section 6.5
 *
 * @param[in] auth_data         authenticator data
 * @param[in] client_data_hash  SHA-256 hash of JSON serialized client data
 * @param[in] rk                resident key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_encode_attestation_object(const ctap_auth_data_t *auth_data,
                                              const uint8_t *client_data_hash,
                                              ctap_resident_key_t *rk);

/**
 * @brief Encode assertion object
 *
 * CTAP specification (version 20190130) section 5.2
 *
 * @param[in] auth_data         authenticator data header
 * @param[in] client_data_hash  SHA-256 hash of JSON serialized client data
 * @param[in] rk                resident key
 * @param[in] valid_cred_count  amount of valid credentials found in allow list
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_encode_assertion_object(const ctap_auth_data_header_t *auth_data,
                                            const uint8_t *client_data_hash,
                                            ctap_resident_key_t *rk,
                                            uint8_t valid_cred_count);
/**
 * @brief Encode key agreement
 *
 * @param[in] key               Public key in COSE format
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_encode_key_agreement(const ctap_public_key_cose_t *key);

/**
 * @brief Encode encrypted pin token
 *
 * @param[in] token             encrypted pin token
 * @param[in] len               length of @p token
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_encode_pin_token(uint8_t *token, size_t len);

/**
 * @brief Encode PIN tries left
 *
 * @param[in] tries_left         amount of tries left
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_cbor_encode_retries(uint8_t tries_left);

/**
 * @brief Get size of CBOR encoded data
 *
 * @param[in] buf  Buffer holding the data
 *
 * @return size of CBOR encoded data
 */
size_t fido2_ctap_cbor_get_buffer_size(const uint8_t *buf);

/**
 * @brief Initialize CBOR encoder
 *
 * @param[in] buf  Buffer to hold CBOR encoded data
 * @param[in] len  Length of @p buf
 */
void fido2_ctap_cbor_init_encoder(uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_CBOR_H */
/** @} */
