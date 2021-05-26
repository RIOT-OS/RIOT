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
                                         const ctap_cose_key_t *key);

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
