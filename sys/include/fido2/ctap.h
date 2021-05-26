/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap CTAP
 * @ingroup     fido2
 * @brief       FIDO2 CTAP
 *
 * The Client-to-Authenticator Protocol (CTAP) is an application layer protocol
 * for the communication between an authenticator and a host.
 *
 * @{
 *
 * @file
 * @brief       FIDO2 CTAP2 defines, structures and function declarations
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#ifndef FIDO2_CTAP_H
#define FIDO2_CTAP_H

#include <stdint.h>

#include "mutex.h"
#include "cbor.h"
#include "assert.h"
#include "hashes/sha256.h"
#include "crypto/modes/ccm.h"

#include "fido2/ctap/ctap_status.h"
#include "fido2/ctap/ctap_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CTAP max message size
 *
 * CTAP specification (version 20190130) section 6
 */
#define CTAP_MAX_MSG_SIZE                   0x400

/**
 * @name CTAP2 methods
 *
 * @{
 */
#define CTAP_MAKE_CREDENTIAL                0x01
#define CTAP_GET_ASSERTION                  0x02
#define CTAP_GET_INFO                       0x04
#define CTAP_CLIENT_PIN                     0x06
#define CTAP_RESET                          0x07
#define CTAP_GET_NEXT_ASSERTION             0x08
#define CTAP_VENDOR_FIRST                   0x40
#define CTAP_VENDOR_LAST                    0xBF
/** @} */

/**
 * @name CTAP authenticator data option flags
 *
 * @{
 */
#define CTAP_AUTH_DATA_FLAG_UP     (1 << 0)     /**< user present */
#define CTAP_AUTH_DATA_FLAG_UV     (1 << 2)     /**< user verified */
#define CTAP_AUTH_DATA_FLAG_AT     (1 << 6)     /**< attested credential data included */
#define CTAP_AUTH_DATA_FLAG_ED     (1 << 7)     /**< extension data included */
/** @} */

/**
 * @brief Max size of relying party name
 */
#define CTAP_RP_MAX_NAME_SIZE   32

/**
 * @brief Max size of username including null character
 */
#define CTAP_USER_MAX_NAME_SIZE 64 + 1

/**
 * @brief Max size of user id
 */
#define CTAP_USER_ID_MAX_SIZE 64

/**
 * @brief Max size of a domain name including null character
 */
#define CTAP_DOMAIN_NAME_MAX_SIZE 253 + 1

/**
 * @brief Max size of icon including null character
 */
#define CTAP_ICON_MAX_SIZE  128 + 1

/**
 * @brief PIN min size
 */
#define CTAP_PIN_MIN_SIZE   4

/**
 * @brief Encrypted newPin min size
 *
 * Encrypted PIN is padded with trailing 0x00 bytes to a minimum length
 * of 64 in order to prevent leak of PIN length.
 */
#define CTAP_PIN_ENC_MIN_SIZE 64

/**
 * @brief Encrypted newPin max size
 *
 */
#define CTAP_PIN_ENC_MAX_SIZE 256

/**
 * @brief PIN max size
 */
#define CTAP_PIN_MAX_SIZE   64

/**
 * @brief Max total consecutive incorrect PIN attempts
 */
#define CTAP_PIN_MAX_ATTS 8

/**
 * @brief Max consecutive incorrect PIN attempts for 1 boot cycle
 */
#define CTAP_PIN_MAX_ATTS_BOOT 3

/**
 * @brief PIN protocol version
 */
#define CTAP_PIN_PROT_VER 1

/**
 * @brief Size of pin token
 */
#define CTAP_PIN_TOKEN_SIZE 16

/**
 * @brief Size of pin auth
 */
#define CTAP_PIN_AUTH_SZ 16

/**
 * @brief Size of key used to encrypt credential
 *
 * Needed if authenticator is unable to store resident keys.
 * See webauthn specification (version 20190304) section 4 (Credential ID)
 * for details
 */
#define CTAP_CRED_KEY_LEN 16

/**
 * @brief AES_CCM_L parameter
 *
 * L has to be between 2 and 8. Value of 2 means that message has to be
 * in the range 0 <= l(m) < 2^(16) = 65536.
 * This should always be sufficient to send an encrypted resident key.
 */
#define CTAP_AES_CCM_L 2

/**
 * @brief AES CCM nonce size
 */
#define CTAP_AES_CCM_NONCE_SIZE (15 - CTAP_AES_CCM_L)

/**
 * @brief Total size of AES CCM credential id
 *
 * Size of encrypted resident key = resident key - cred id
 *                                        - has_nonce
 */
#define CTAP_CREDENTIAL_ID_ENC_SIZE (sizeof(struct ctap_resident_key) - \
                                     sizeof(((struct ctap_resident_key *)0)-> \
                                            cred_desc.cred_id) - \
                                     sizeof(((struct ctap_resident_key *)0)-> \
                                            cred_desc.has_nonce))
/**
 * @brief Start page for storing resident keys
 */
#define CTAP_RK_START_PAGE 34U

/**
 * @brief Page for storing authenticator state information
 */
#define CTAP_STATE_PAGE CTAP_RK_START_PAGE - 1

/**
 * @brief Calculate padding needed to align struct size for saving to flash
 */
#define CTAP_FLASH_ALIGN_PAD(x) (sizeof(x) % FLASHPAGE_WRITE_BLOCK_ALIGNMENT == \
                                 0 ? \
                                 0 : FLASHPAGE_WRITE_BLOCK_ALIGNMENT - \
                                 sizeof(x) % FLASHPAGE_WRITE_BLOCK_ALIGNMENT)

/**
 * @brief Resident key size with alignment padding
 */
#define CTAP_PAD_RK_SZ (sizeof(struct ctap_resident_key) + \
                        CTAP_FLASH_ALIGN_PAD(struct ctap_resident_key))

/**
 * @brief State struct size with alignment padding
 */
#define CTAP_PAD_STATE_SZ (sizeof(ctap_state_t) + \
                           CTAP_FLASH_ALIGN_PAD(ctap_state_t))

/**
 * @brief Max amount of resident keys that can be stored
 */
#define CTAP_MAX_RK (((FLASHPAGE_NUMOF - CTAP_RK_START_PAGE) *FLASHPAGE_SIZE) /  \
                     CTAP_PAD_RK_SZ)

/**
 * @brief Timeout for user presence test
 */
#define CTAP_UP_TIMEOUT (15 * US_PER_SEC)

/**
 * @brief Max time between call to get_assertion or get_next_assertion until
 * error is returned
 */
#define CTAP_GET_NEXT_ASSERTION_TIMEOUT (30 * US_PER_SEC)

/**
 * @name CTAP version strings
 * @{
 */
#define CTAP_VERSION_STRING_FIDO_PRE "FIDO_2_1_PRE"
#define CTAP_VERSION_STRING_FIDO     "FIDO_2_0"
#define CTAP_VERSION_STRING_U2F_V2   "U2F_V2"
/** @} */

/**
 * @name CTAP version flags
 *
 * @{
 */
#define CTAP_VERSION_FLAG_FIDO_PRE  0x01
#define CTAP_VERSION_FLAG_FIDO      0x02
#define CTAP_VERSION_FLAG_U2F_V2    0x04
/** @} */

/**
 * @name CTAP get info response options map CBOR key values
 *
 * All options are in the form key-value pairs with string IDs and
 * boolean values
 * @{
 */
#define CTAP_GET_INFO_RESP_OPTIONS_ID_PLAT       "plat"
#define CTAP_GET_INFO_RESP_OPTIONS_ID_RK         "rk"
#define CTAP_GET_INFO_RESP_OPTIONS_ID_CLIENT_PIN "clientPin"
#define CTAP_GET_INFO_RESP_OPTIONS_ID_UP         "up"
#define CTAP_GET_INFO_RESP_OPTIONS_ID_UV         "uv"
/** @} */

/**
 * @name CTAP get info options flags
 *
 * @{
 */
#define CTAP_INFO_OPTIONS_FLAG_PLAT         (1 << 0)
#define CTAP_INFO_OPTIONS_FLAG_RK           (1 << 1)
#define CTAP_INFO_OPTIONS_FLAG_CLIENT_PIN   (1 << 2)
#define CTAP_INFO_OPTIONS_FLAG_UP           (1 << 3)
#define CTAP_INFO_OPTIONS_FLAG_UV           (1 << 4)
/** @} */

/**
 * @name CTAP get info response CBOR key values
 *
 * @{
 */
#define CTAP_GET_INFO_RESP_VERSIONS         0x01
#define CTAP_GET_INFO_RESP_EXTENSIONS       0x02
#define CTAP_GET_INFO_RESP_AAGUID           0x03
#define CTAP_GET_INFO_RESP_OPTIONS          0x04
#define CTAP_GET_INFO_RESP_MAX_MSG_SIZE     0x05
#define CTAP_GET_INFO_RESP_PIN_PROTOCOLS    0x06
/** @} */

/**
 * @name CTAP make credential request CBOR key values
 *
 * @{
 */
#define CTAP_MC_REQ_CLIENT_DATA_HASH    0x01
#define CTAP_MC_REQ_RP                  0x02
#define CTAP_MC_REQ_USER                0x03
#define CTAP_MC_REQ_PUB_KEY_CRED_PARAMS 0x04
#define CTAP_MC_REQ_EXCLUDE_LIST        0x05
#define CTAP_MC_REQ_EXTENSIONS          0x06
#define CTAP_MC_REQ_OPTIONS             0x07
#define CTAP_MC_REQ_PIN_AUTH            0x08
#define CTAP_MC_REQ_PIN_PROTOCOL        0x09
/** @} */

/**
 * @name CTAP make credential response CBOR key values
 *
 * @{
 */
#define CTAP_MC_RESP_FMT                0x01
#define CTAP_MC_RESP_AUTH_DATA          0x02
#define CTAP_MC_RESP_ATT_STMT           0x03
/** @} */

/**
 * @name CTAP get assertion request CBOR key values
 *
 * @{
 */
#define CTAP_GA_REQ_RP_ID               0x01
#define CTAP_GA_REQ_CLIENT_DATA_HASH    0x02
#define CTAP_GA_REQ_ALLOW_LIST          0x03
#define CTAP_GA_REQ_EXTENSIONS          0x04
#define CTAP_GA_REQ_OPTIONS             0x05
#define CTAP_GA_REQ_PIN_AUTH            0x06
#define CTAP_GA_REQ_PIN_PROTOCOL        0x07
/** @} */

/**
 * @name CTAP get assertion response CBOR key values
 *
 * @{
 */
#define CTAP_GA_RESP_CREDENTIAL             0x01
#define CTAP_GA_RESP_AUTH_DATA              0x02
#define CTAP_GA_RESP_SIGNATURE              0x03
#define CTAP_GA_RESP_USER                   0x04
#define CTAP_GA_RESP_NUMBER_OF_CREDENTIALS  0x05
/** @} */

/**
 * @name CTAP Client PIN request CBOR key values
 *
 * @{
 */
#define CTAP_CP_REQ_PIN_PROTOCOL    0x01
#define CTAP_CP_REQ_SUB_COMMAND     0x02
#define CTAP_CP_REQ_KEY_AGREEMENT   0x03
#define CTAP_CP_REQ_PIN_AUTH        0x04
#define CTAP_CP_REQ_NEW_PIN_ENC     0x05
#define CTAP_CP_REQ_PIN_HASH_ENC    0x06
/** @} */

/**
 * @name CTAP Client PIN request subcommand CBOR key values
 *
 * @{
 */
#define CTAP_CP_REQ_SUB_COMMAND_GET_RETRIES         0x01
#define CTAP_CP_REQ_SUB_COMMAND_GET_KEY_AGREEMENT   0x02
#define CTAP_CP_REQ_SUB_COMMAND_SET_PIN             0x03
#define CTAP_CP_REQ_SUB_COMMAND_CHANGE_PIN          0x04
#define CTAP_CP_REQ_SUB_COMMAND_GET_PIN_TOKEN       0x05
/** @} */

/**
 * @name CTAP Client PIN resp CBOR key values
 *
 * @{
 */
#define CTAP_CP_RESP_KEY_AGREEMENT  0x01
#define CTAP_CP_RESP_PIN_TOKEN      0x02
#define CTAP_CP_RESP_RETRIES        0x03
/** @} */

/**
 * @brief CTAP size of authenticator AAGUID in bytes
 */
#define CTAP_AAGUID_SIZE 16

/**
 * 128 bit identifier of authenticator
 */
#ifdef CONFIG_FIDO2_CTAP_DEVICE_AAGUID
#define CTAP_AAGUID CONFIG_FIDO2_CTAP_DEVICE_AAGUID
#else
/* randomly generated fallback value */
#define CTAP_AAGUID "9c295865fa2c36b705a42320af9c8f16"
#endif

/**
 * @name CTAP credential types
 *
 * @{
 */
#define CTAP_PUB_KEY_CRED_PUB_KEY 0x01
#define CTAP_PUB_KEY_CRED_UNKNOWN 0x02
/** @} */

/**
 * @name CTAP COSE key CBOR map key values
 *
 * @{
 */
#define CTAP_COSE_KEY_LABEL_KTY      1
#define CTAP_COSE_KEY_LABEL_ALG      3
#define CTAP_COSE_KEY_LABEL_CRV      -1
#define CTAP_COSE_KEY_LABEL_X        -2
#define CTAP_COSE_KEY_LABEL_Y        -3
#define CTAP_COSE_KEY_KTY_EC2        2
#define CTAP_COSE_KEY_CRV_P256       1
/** @} */

/**
 * @brief CTAP COSE Algorithms registry identifier for ES256
 */
#define CTAP_COSE_ALG_ES256           -7

/**
 * @brief CTAP COSE Algorithms registry identifier for ECDH ES HKDF 256
 */
#define CTAP_COSE_ALG_ECDH_ES_HKDF_256 -25

/**
 * @brief CTAP size of credential id
 *
 */
#define CTAP_CREDENTIAL_ID_SIZE 16U

/**
 * @brief CTAP state initialized marker
 *
 * Used to check if authenticator state has already been initialized when
 * reading data from flash.
 */
#define CTAP_INITIALIZED_MARKER 0x4e

/**
 * @brief Max size of allow list
 */
#define CTAP_MAX_ALLOW_LIST_SIZE 0x10

/**
 * @brief Max size of allow list
 */
#define CTAP_MAX_EXCLUDE_LIST_SIZE 0x10

/**
 * @brief CTAP resp struct forward declaration
 */
typedef struct ctap_resp ctap_resp_t;

/**
 * @brief CTAP cred struct forward declaration
 */
typedef struct ctap_cred_desc ctap_cred_desc_t;

/**
 * @brief Alternative CTAP cred struct forward declaration
 */
typedef struct ctap_cred_desc_alt ctap_cred_desc_alt_t;

/**
 * @brief CTAP resident key credential forward declaration
 */
typedef struct ctap_resident_key ctap_resident_key_t;

/**
 * @brief CTAP user entity struct
 */
typedef struct {
    uint8_t id[CTAP_USER_ID_MAX_SIZE];                  /**< RP-specific user account id */
    uint8_t id_len;                                     /**< actual length of user id */
    uint8_t name[CTAP_USER_MAX_NAME_SIZE];              /**< user name */
    uint8_t display_name[CTAP_USER_MAX_NAME_SIZE];      /**< user display name */
    uint8_t icon[CTAP_DOMAIN_NAME_MAX_SIZE];            /**< URL referencing user icon image */
} ctap_user_ent_t;

/**
 * @brief CTAP response struct
 */
struct ctap_resp {
    uint8_t status;                     /**< response status */
    uint8_t data[CTAP_MAX_MSG_SIZE];    /**< response data */
};

/**
 * @brief CTAP options struct
 *
 */
typedef struct {
    int rk; /**< resident key */
    int uv; /**< user verification */
    int up; /**< user presence */
} ctap_options_t;

/**
 * @brief CTAP relying party entity struct
 *
 */
typedef struct {
    uint8_t id[CTAP_DOMAIN_NAME_MAX_SIZE + 1];      /**< relying party identifier */
    uint8_t id_len;                                 /**< actual length of
                                                       relying party identifier */
    uint8_t name[CTAP_RP_MAX_NAME_SIZE + 1];        /**< human friendly relying
                                                         party name */
    uint8_t icon[CTAP_DOMAIN_NAME_MAX_SIZE + 1];    /**< URL referencing relying
                                                         party icon image */
} ctap_rp_ent_t;

/**
 * @brief CTAP cose key struct
 *
 * https://www.iana.org/assignments/cose/cose.xhtml
 */
typedef struct {
    ctap_crypto_pub_key_t pubkey;   /**< public key */
    int kty;                        /**< identification of key type */
    int crv;                        /**< EC identifier */
    int32_t alg_type;               /**< COSEAlgorithmIdentifier */
    uint8_t cred_type;              /**< type of credential */
} ctap_cose_key_t;

/**
 * @brief CTAP credential description struct
 *
 * Webauthn specification (version 20190304) section 5.8.3
 */
struct ctap_cred_desc {
    uint8_t cred_type;                              /**< type of credential */
    union {
        uint8_t cred_id[CTAP_CREDENTIAL_ID_SIZE];   /**< credential identifier */
        uint8_t nonce[CTAP_AES_CCM_NONCE_SIZE];     /**< CTAP AES CCM nonce */
    };
    bool has_nonce;                                 /**< Indicate if nonce or
                                                         cred_id  */
};

/**
 * @brief CTAP resident key struct
 *
 * A resident key is a fido2 credential that is being stored on the
 * authenticator.
 */
struct __attribute__((packed)) ctap_resident_key {
    uint8_t rp_id_hash[SHA256_DIGEST_LENGTH];   /**< hash of rp domain string */
    uint8_t user_id[CTAP_USER_ID_MAX_SIZE];     /**< id of user */
    uint8_t user_id_len;                        /**< length of the user id */
    uint8_t priv_key[CTAP_CRYPTO_KEY_SIZE];     /**< private key */
    uint32_t sign_count;                        /**< signature counter.
                                                See webauthn specification
                                                (version 20190304) section 6.1.1
                                                for details. */
    ctap_cred_desc_t cred_desc;                 /**< credential descriptor */
};

/**
 * @brief CTAP credential ID
 *
 * Credential ID can either be 16 random bytes or the encrypted resident
 * key. (AES CCM cipher + mac + nonce used)
 */
typedef struct __attribute__((packed)) {
    uint8_t id[CTAP_CREDENTIAL_ID_ENC_SIZE];    /**< id */
    uint8_t mac[CCM_MAC_MAX_LEN];               /**< AES CCM MAC */
    uint8_t nonce[CTAP_AES_CCM_NONCE_SIZE];     /**< AES CCM nonce */
} ctap_cred_id_t;

/**
 * @brief CTAP credential description alternative struct
 *
 * This struct is used when parsing an allow or exclude list.
 */
struct ctap_cred_desc_alt {
    uint8_t cred_type;          /**< type of credential */
    ctap_cred_id_t cred_id;     /**< credential id */
};

/**
 * @brief CTAP make credential request struct
 */
typedef struct {
    uint8_t client_data_hash[SHA256_DIGEST_LENGTH];     /**< SHA-256 hash of JSON serialized client data */
    ctap_rp_ent_t rp;                                   /**< relying party */
    ctap_user_ent_t user;                               /**< user */
    ctap_options_t options;                             /**< parameters to influence authenticator operation */
    CborValue exclude_list;                             /**< CBOR array holding exclude list */
    size_t exclude_list_len;                            /**< length of CBOR exclude list array */
    uint8_t pin_auth[CTAP_PIN_AUTH_SZ];                 /**< pin_auth if PIN is set */
    size_t pin_auth_len;                                /**< pin_auth len */
    bool pin_auth_present;                              /**< pin_auth present */
    uint8_t pin_protocol;                               /**< PIN protocol version */
    uint8_t cred_type;                                  /**< type of credential */
    int32_t alg_type;                                   /**< cryptographic algorithm identifier */
} ctap_make_credential_req_t;

/**
 * @brief CTAP get assertion request struct
 */
typedef struct {
    uint8_t rp_id[CTAP_DOMAIN_NAME_MAX_SIZE + 1];       /**< Relying Party Identifier */
    uint8_t rp_id_len;                                  /**< Actual Length of Relying Party Identifier */
    uint8_t client_data_hash[SHA256_DIGEST_LENGTH];     /**< SHA-256 hash of JSON serialized client data */
    ctap_options_t options;                             /**< parameters to influence authenticator operation */
    CborValue allow_list;                               /**< cbor array holding allow list */
    uint8_t allow_list_len;                             /**< length of CBOR allow list array */
    uint8_t pin_auth[CTAP_PIN_AUTH_SZ];                 /**< pin_auth if PIN is set */
    size_t pin_auth_len;                                /**< pin_auth length */
    bool pin_auth_present;                              /**< indicate if pin_auth present */
    uint8_t pin_protocol;                               /**< PIN protocol version */
} ctap_get_assertion_req_t;

/**
 * @brief CTAP client pin request struct
 */
typedef struct {
    uint8_t pin_protocol;                                       /**< PIN protocol version chosen by the client */
    uint8_t sub_command;                                        /**< authenticator Client PIN sub command */
    ctap_cose_key_t key_agreement;                              /**< public key of platform_key_agreement_key*/
    bool key_agreement_present;                                 /**< indicate if key_agreement present */
    uint8_t pin_auth[CTAP_PIN_AUTH_SZ];                         /**< first 16 bytes of HMAC-SHA-256 of encrypted contents  */
    bool pin_auth_present;                                      /**< indicate if pin_auth present */
    uint8_t new_pin_enc[CTAP_PIN_ENC_MAX_SIZE];                 /**< Encrypted new PIN using sharedSecret. */
    uint16_t new_pin_enc_size;                                  /**< size of encrypted new pin */
    uint8_t pin_hash_enc[SHA256_DIGEST_LENGTH / 2];             /**< Encrypted first 16 bytes of SHA-256 of PIN using sharedSecret. */
    bool pin_hash_enc_present;                                  /**< indicate pin_hash_enc is present */
} ctap_client_pin_req_t;

/**
 * @brief CTAP get_assertion state
 */
typedef struct {
    ctap_resident_key_t rks[CTAP_MAX_ALLOW_LIST_SIZE];  /**< eligible resident keys found */
    uint8_t count;                                      /**< number of rks found  */
    uint8_t cred_counter;                               /**< amount of creds sent to host */
    uint32_t timer;                                     /**< time gap between get_next_assertion calls  */
    bool uv;                                            /**< indicate if user verified */
    bool up;                                            /**< indicate if user present */
    uint8_t client_data_hash[SHA256_DIGEST_LENGTH];     /**< SHA-256 hash of JSON serialized client data */
} ctap_get_assertion_state_t;

/**
 * @brief CTAP attested credential data header struct
 *
 * Defined for easier serialization
 */
typedef struct __attribute__((packed)){
    uint8_t aaguid[CTAP_AAGUID_SIZE];   /**< authenticator aaguid */
    uint8_t cred_len_h;                 /**< higher byte of credential length */
    uint8_t cred_len_l;                 /**< lower byte of credential length */
    ctap_cred_id_t cred_id;             /**< credential id */
} ctap_attested_cred_data_header_t;

/**
 * @brief CTAP attested credential data struct
 */
typedef struct {
    ctap_attested_cred_data_header_t header;    /**< attested credential data header */
    ctap_cose_key_t key;                        /**< cose key */
} ctap_attested_cred_data_t;

/**
 * @brief CTAP authenticator data header struct
 *
 * Defined for easier serialization
 */
typedef struct __attribute__((packed)){
    uint8_t rp_id_hash[SHA256_DIGEST_LENGTH];   /**< hash of relying party id */
    uint8_t flags;                              /**< flags indicating result of user verification */
    uint32_t sign_count;                        /**< sign count of credential */
} ctap_auth_data_header_t;

/**
 * @brief CTAP authenticator data struct
 */
typedef struct {
    ctap_auth_data_header_t header;                 /**< auth data header */
    ctap_attested_cred_data_t attested_cred_data;   /**< attested credential data */
} ctap_auth_data_t;

/**
 * @brief CTAP authenticator config struct
 */
typedef struct {
    uint8_t options;                    /**< options */
    uint8_t aaguid[CTAP_AAGUID_SIZE];   /**< AAGUID of device */
} ctap_config_t;

/**
 * @brief CTAP state struct
 *
 * state of authenticator. Stored in flash memory
 */
typedef struct {
    uint8_t initialized;                        /**< CTAP is initialized or not */
    bool pin_is_set;                            /**< PIN is set or not */
    int rem_pin_att;                            /**< remaining PIN tries */
    uint8_t pin_hash[SHA256_DIGEST_LENGTH / 2]; /**< LEFT(SHA-256(pin), 16) */
    ctap_config_t config;                       /**< configuration of authenticator */
    uint16_t rk_amount_stored;                  /**< total number of resident keys stored on device */
    uint8_t cred_key[CTAP_CRED_KEY_LEN];        /**< AES CCM encryption key for cred */
} ctap_state_t;

/**
 * @brief CTAP info struct
 */
typedef struct {
    uint8_t versions;                   /**< supported versions of FIDO */
    uint8_t aaguid[CTAP_AAGUID_SIZE];   /**< AAGUID */
    uint8_t options;                    /**< supported options */
    uint16_t max_msg_size;              /**< max message size */
    uint8_t pin_protocol;               /**< supported PIN protocol versions */
    bool pin_is_set;                    /**< PIN is set or not */
} ctap_info_t;

/**
 * @brief Initialize ctap
 *
 * @return 0 for success, -1 for failure
 */
int fido2_ctap_init(void);

/**
 * @brief Handle CBOR encoded ctap request.
 *
 * @param[in] req_raw           raw request from host
 * @param[in] size              size of request in bytes
 * @param[in] resp              response struct
 * @param[in] should_cancel     callback to cancel transaction if CTAPHID_CANCEL was received.
 *
 * @return size of CBOR encoded response data
 */
size_t fido2_ctap_handle_request(uint8_t *req, size_t size, ctap_resp_t *resp,
                                 bool (*should_cancel)(void));

/**
 * @brief Create signature from authenticator data
 *
 * Used for attestation and assertion statement
 *
 * @param[in] auth_data         authenticator data
 * @param[in] auth_data_len     length of authenticator data
 * @param[in] client_data_hash  hash of client data sent by relying party in request
 * @param[in] rk                resident key used to sign the data
 * @param[in] sig               signature buffer
 * @param[in] sig_len           length of signature buffer
 *
 * @return CTAP status code
 */
int fido2_ctap_get_sig(const uint8_t *auth_data, size_t auth_data_len,
                              const uint8_t *client_data_hash,
                              const ctap_resident_key_t *rk,
                              uint8_t *sig, size_t *sig_len);

/**
 * @brief Check if requested algorithm is supported
 *
 * @param[in] cred_type    type of credential
 * @param[in] alg_type     cryptographic algorithm identifier
 *
 */
bool fido2_ctap_cred_params_supported(uint8_t cred_type, int32_t alg_type);

/**
 * @brief Encrypt resident key with AES CCM
 *
 * @param[in] rk                type of credential
 * @param[in] nonce             CCM nonce
 * @param[in] nonce_size        size of nonce buffer
 * @param[in] id                credential id struct storing encrypted resident key
 *
 * @return CTAP status code
 */
int fido2_ctap_encrypt_rk(ctap_resident_key_t *rk, uint8_t *nonce,
                          size_t nonce_size, ctap_cred_id_t *id);

/**
 * @brief Return if PIN has been set
 *
 * @return true if it has been set, false if not
 */
bool fido2_ctap_pin_is_set(void);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_H */
/** @} */
