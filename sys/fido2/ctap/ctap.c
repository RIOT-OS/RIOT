/*
 * Copyright (C) 2021 Nils Ollrogge
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
 * @author  Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "fmt.h"
#include "xtimer.h"
#include "byteorder.h"

#include "fido2/ctap/transport/ctap_transport.h"
#include "fido2/ctap.h"
#include "fido2/ctap/ctap_utils.h"
#include "fido2/ctap/ctap_cbor.h"
#include "fido2/ctap/ctap_mem.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/*** main CTAP2 functions ***/

/**
 * @brief MakeCredential method
 *
 * CTAP specification (version 20190130) section 5.1
 */
static uint8_t make_credential(CborEncoder *encoder, uint8_t *req_raw,
                               size_t size,
                               bool (*should_cancel)(void));
/**
 * @brief GetAssertion method
 *
 * CTAP specification (version 20190130) section 5.2
 */
static int get_assertion(CborEncoder *encoder, size_t size, uint8_t *req_raw,
                         bool (*should_cancel)(void));

/**
 * @brief GetNextAssertion method
 *
 * CTAP specification (version 20190130) section 5.3
 */
static int get_next_assertion(CborEncoder *encoder);

/**
 * @brief GetInfo method
 *
 * CTAP specification (version 20190130) section 5.4
 */
static uint8_t get_info(CborEncoder *encoder);

/**
 * @brief ClientPIN method
 *
 * CTAP specification (version 20190130) section 5.5
 */
static int client_pin(CborEncoder *encoder, size_t size, uint8_t *req_raw,
                      bool (*should_cancel)(void));

/*** CTAP2 clientPIN functions ***/

/**
 * @brief ClientPIN getRetries method
 *
 * CTAP specification (version 20190130) section 5.5.3
 */
static uint8_t get_retries(CborEncoder *encoder);

/**
 * @brief ClientPIN getKeyAgreement method
 *
 * CTAP specification (version 20190130) section 5.5.4
 */
static int get_key_agreement(CborEncoder *encoder);

/**
 * @brief ClientPIN setPIN method
 *
 * CTAP specification (version 20190130) section 5.5.5
 */
static int set_pin(ctap_client_pin_req_t *req, bool (*should_cancel)(void));

/**
 * @brief ClientPIN changePIN method
 *
 * CTAP specification (version 20190130) section 5.5.6
 */
static int change_pin(ctap_client_pin_req_t *req, bool (*should_cancel)(void));

/**
 * @brief ClientPIN getPINToken method
 *
 * CTAP specification (version 20190130) section 5.5.7
 */
static int get_pin_token(CborEncoder *encoder, ctap_client_pin_req_t *req,
                         bool (*should_cancel)(void));

/*** helper functions ***/

/**
 * @brief Initialize authData of attestation object
 */
static int make_auth_data_attest(ctap_make_credential_req_t *req,
                                 ctap_auth_data_t *auth_data,
                                 ctap_resident_key_t *k, bool uv, bool up,
                                 bool rk);
/**
 * @brief Initialize authData of assertion object
 */
static int make_auth_data_assert(uint8_t *rp_id, size_t rp_id_len,
                                 ctap_auth_data_header_t *auth_data, bool uv,
                                 bool up, uint32_t sign_count);
/**
 * @brief Initialize authData of assertion object
 */
static int make_auth_data_next_assert(uint8_t *rp_id_hash,
                                      ctap_auth_data_header_t *auth_data,
                                      bool uv, bool up, uint32_t sign_count);
/**
 * @brief Find most recent rk matching rp_id_hash and present in allow_list
 */
static uint8_t find_matching_rks(ctap_resident_key_t *rks, size_t rks_len,
                                 ctap_cred_desc_alt_t *allow_list,
                                 size_t allow_list_len,
                                 uint8_t *rp_id, size_t rp_id_len);
/**
 * @brief Check if any of the credentials in li belong to this authenticator
 */
static bool rks_exist(ctap_cred_desc_alt_t *li, size_t len, uint8_t *rp_id,
                      size_t rp_id_len);

/**
 * @brief Decrypt credential that is stored by relying party
 *
 * Webauthn specification (version 20190304) section 4 (Credential ID)
 */
static int ctap_decrypt_rk(ctap_resident_key_t *rk, ctap_cred_id_t *id);

static int save_rk(ctap_resident_key_t *rk);
static int save_pin(uint8_t *pin, size_t len);
static int save_state(const ctap_state_t *state);
static void load_state(ctap_state_t *state);
static inline bool pin_protocol_supported(uint8_t version);
static inline uint8_t get_remaining_pin_attempts(void);
static int decrement_pin_attempts(void);
static void reset_pin_attempts(void);
static void reset(void);
static int verify_pin_auth(uint8_t *auth, uint8_t *hash, size_t len);
static inline bool locked(void);
static inline bool boot_locked(void);
static inline uint16_t get_page_offset(uint16_t i);
static inline uint16_t get_offset_into_page(uint16_t t);

static ctap_state_t g_state;
static ctap_get_assertion_state_t g_assert_state;
static uint8_t g_pin_token[CTAP_PIN_TOKEN_SIZE];
/* remaining PIN attempts until authenticator is boot locked */
static int g_rem_pin_att_boot = CTAP_PIN_MAX_ATTS_BOOT;

int fido2_ctap_init(void)
{
    int ret;

    load_state(&g_state);

    ret = fido2_ctap_crypto_init();

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: initialization failed \n");
        return -1;
    }

    /* first startup up of the device */
    if (g_state.initialized != CTAP_INITIALIZED_MARKER) {
        reset();
    }

    /* initialize pin_token */
    fido2_ctap_crypto_prng(g_pin_token, sizeof(g_pin_token));

    DEBUG("fido2_ctap: initialization successful \n");

    return 0;
}

static void reset(void)
{
    g_state.initialized = CTAP_INITIALIZED_MARKER;
    g_state.rem_pin_att = CTAP_PIN_MAX_ATTS;
    g_state.pin_is_set = false;
    g_state.rk_amount_stored = 0;
    g_state.sign_count = 0;

    g_rem_pin_att_boot = CTAP_PIN_MAX_ATTS_BOOT;

    fido2_ctap_crypto_prng(g_state.cred_key, sizeof(g_state.cred_key));

    g_state.config.options |= CTAP_INFO_OPTIONS_FLAG_PLAT;
    g_state.config.options |= CTAP_INFO_OPTIONS_FLAG_RK;
    g_state.config.options |= CTAP_INFO_OPTIONS_FLAG_CLIENT_PIN;
    g_state.config.options |= CTAP_INFO_OPTIONS_FLAG_UP;

    uint8_t aaguid[CTAP_AAGUID_SIZE];

    fmt_hex_bytes(aaguid, CTAP_AAGUID);

    memmove(g_state.config.aaguid, aaguid, sizeof(g_state.config.aaguid));
    save_state(&g_state);
}

size_t fido2_ctap_handle_request(uint8_t *req, size_t size, ctap_resp_t *resp,
                                 bool (*should_cancel)(void))
{
    CborEncoder encoder = { 0 };
    uint8_t cmd = *req;
    uint8_t *buf = resp->data;

    req++;
    size--;

    cbor_encoder_init(&encoder, buf, CTAP_MAX_MSG_SIZE, 0);

    /* handle common error case */
    switch (cmd) {
    case CTAP_MAKE_CREDENTIAL:
    case CTAP_GET_ASSERTION:
    case CTAP_GET_NEXT_ASSERTION: {
        if (locked()) {
            return CTAP2_ERR_PIN_BLOCKED;
        }

        if (boot_locked()) {
            return CTAP2_ERR_PIN_AUTH_BLOCKED;
        }
    }
    }

    switch (cmd) {
    case CTAP_GET_INFO:
        DEBUG("fido2_ctap: get_info req \n");
        resp->status = get_info(&encoder);
        DEBUG("fido2_ctap: get_info resp: status: %u, size: %u \n",
              resp->status, cbor_encoder_get_buffer_size(&encoder, buf));
        return cbor_encoder_get_buffer_size(&encoder, buf);
        break;
    case CTAP_MAKE_CREDENTIAL:
        DEBUG("fido2_ctap: make_credential req \n");
        resp->status = make_credential(&encoder, req, size, should_cancel);
        DEBUG("fido2_ctap: make_credential resp: status: %u, size: %u \n",
              resp->status, cbor_encoder_get_buffer_size(&encoder, buf));
        return cbor_encoder_get_buffer_size(&encoder, buf);
        break;
    case CTAP_GET_ASSERTION:
        DEBUG("fido2_ctap: get_assertion req \n");
        resp->status = get_assertion(&encoder, size, req, should_cancel);
        DEBUG("fido2_ctap: get_assertion resp: status: %u, size: %u \n",
              resp->status, cbor_encoder_get_buffer_size(&encoder, buf));
        return cbor_encoder_get_buffer_size(&encoder, buf);
        break;
    case CTAP_GET_NEXT_ASSERTION:
        DEBUG("fido2_ctap: get_next_assertion req \n");
        resp->status = get_next_assertion(&encoder);
        DEBUG("fido2_ctap: get_next_assertion resp: status: %u, size: %u \n",
              resp->status, cbor_encoder_get_buffer_size(&encoder, buf));
        return cbor_encoder_get_buffer_size(&encoder, buf);
        break;
    case CTAP_CLIENT_PIN:
        DEBUG("fido2_ctap: client_pin req \n");
        resp->status = client_pin(&encoder, size, req, should_cancel);
        DEBUG("fido2_ctap: client_pin resp: status: %u, size: %u \n",
              resp->status, cbor_encoder_get_buffer_size(&encoder, buf));
        return cbor_encoder_get_buffer_size(&encoder, buf);
        break;
    case CTAP_RESET:
        DEBUG("fido2_ctap: reset req \n");
        reset();
        break;
    default:
        DEBUG("fido2_ctap: unknown req: %u \n", cmd);
        resp->status = CTAP1_ERR_INVALID_COMMAND;
        return 0;
        break;
    }

    return 0;
}

static uint8_t make_credential(CborEncoder *encoder, uint8_t *req_raw,
                               size_t size,
                               bool (*should_cancel)(void))
{
    int ret;
    bool uv = false;
    bool up = false;
    bool rk = false;
    ctap_make_credential_req_t req = { 0 };
    ctap_auth_data_t auth_data = { 0 };
    ctap_resident_key_t k = { 0 };
    ctap_cred_desc_alt_t exclude_list[CTAP_MAX_EXCLUDE_LIST_SIZE] = { 0 };

    /* set default values for options */
    req.options.rk = false;
    req.options.uv = false;
    req.options.up = -1;

    ret = fido2_ctap_cbor_parse_make_credential_req(&req, req_raw, size);

    if (ret != CTAP2_OK) {
        return ret;
    }

    if (req.exclude_list_len > 0) {

        if (req.exclude_list_len > CTAP_MAX_EXCLUDE_LIST_SIZE) {
            req.exclude_list_len = CTAP_MAX_EXCLUDE_LIST_SIZE;
        }

        for (uint8_t i = 0; i < req.exclude_list_len; i++) {
            ret = fido2_ctap_cbor_parse_cred_desc(&req.exclude_list,
                                                  &exclude_list[i]);

            if (ret != CTAP2_OK) {
                return ret;
            }
        }

        if (rks_exist(exclude_list, req.exclude_list_len, req.rp.id,
                      req.rp.id_len)) {
#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
            fido2_ctap_utils_user_presence_test();
#endif
            return CTAP2_ERR_CREDENTIAL_EXCLUDED;
        }
    }

    /* setting user presence (up) is not valid for this command */
    if (req.options.up != -1) {
        return CTAP2_ERR_INVALID_OPTION;
    }

    if (fido2_ctap_pin_is_set() && req.pin_auth_present) {
        /* CTAP specification (version 20190130) section 5.5.8.1 */
        if (req.pin_auth_len == 0) {
#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
            fido2_ctap_utils_user_presence_test();
#endif
            return CTAP2_ERR_PIN_INVALID;
        }

        ret = verify_pin_auth(req.pin_auth, req.client_data_hash,
                              sizeof(req.client_data_hash));

        if (ret != CTAP2_OK) {
            return ret;
        }

        uv = true;
    }
    /* CTAP specification (version 20190130) section 5.5.8.1 */
    else if (!fido2_ctap_pin_is_set() && req.pin_auth_present
             && req.pin_auth_len == 0) {
#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
        fido2_ctap_utils_user_presence_test();
#endif
        return CTAP2_ERR_PIN_NOT_SET;
    }
    else if (fido2_ctap_pin_is_set() && !req.pin_auth_present) {
        return CTAP2_ERR_PIN_REQUIRED;
    }

    if (req.pin_auth_present && !pin_protocol_supported(req.pin_protocol)) {
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    /* last moment where transaction can be cancelled */
    if (should_cancel()) {
        return CTAP2_ERR_KEEPALIVE_CANCEL;
    }

    /* ask for permission to create a new credential */
#if IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
    up = true;
#else
    if (fido2_ctap_utils_user_presence_test() == CTAP2_OK) {
        up = true;
    }
#endif

    /* resident credential or not */
    rk = req.options.rk;

    ret = make_auth_data_attest(&req, &auth_data, &k, uv, up, rk);

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = fido2_ctap_cbor_encode_attestation_object(encoder, &auth_data,
                                                    req.client_data_hash, &k);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* if created credential is a resident key, save it to flash */
    if (rk) {
        ret = save_rk(&k);
        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    return CTAP2_OK;
}

static int get_assertion(CborEncoder *encoder, size_t size, uint8_t *req_raw,
                         bool (*should_cancel)(void))
{
    int ret;
    bool uv = false;
    bool up = false;
    ctap_get_assertion_req_t req = { 0 };
    ctap_auth_data_header_t auth_data = { 0 };
    ctap_cred_desc_alt_t allow_list[CTAP_MAX_ALLOW_LIST_SIZE] = { 0 };
    ctap_resident_key_t *rk;

    memset(&g_assert_state, 0, sizeof(g_assert_state));

    /* set default values for options */
    req.options.up = true;
    req.options.uv = false;

    ret = fido2_ctap_cbor_parse_get_assertion_req(&req, req_raw, size);

    if (ret != CTAP2_OK) {
        return ret;
    }

    if (req.allow_list_len > CTAP_MAX_ALLOW_LIST_SIZE) {
        req.allow_list_len = CTAP_MAX_ALLOW_LIST_SIZE;
    }

    for (uint8_t i = 0; i < req.allow_list_len; i++) {
        ret = fido2_ctap_cbor_parse_cred_desc(&req.allow_list, &allow_list[i]);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    /* find eligble credentials */
    g_assert_state.count = find_matching_rks(g_assert_state.rks,
                                             sizeof(g_assert_state.rks),
                                             allow_list,
                                             req.allow_list_len, req.rp_id,
                                             req.rp_id_len);

    if (fido2_ctap_pin_is_set() && req.pin_auth_present) {
        /* CTAP specification (version 20190130) section 5.5.8.2 */
        if (req.pin_auth_len == 0) {
#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
            fido2_ctap_utils_user_presence_test();
#endif
            return CTAP2_ERR_PIN_INVALID;
        }
        ret = verify_pin_auth(req.pin_auth, req.client_data_hash,
                              sizeof(req.client_data_hash));

        if (ret != CTAP2_OK) {
            return ret;
        }

        uv = true;
        g_assert_state.uv = true;
    }
    /* CTAP specification (version 20190130) section 5.5.8.2 */
    else if (!fido2_ctap_pin_is_set() && req.pin_auth_present
             && req.pin_auth_len == 0) {
#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
        fido2_ctap_utils_user_presence_test();
#endif
        return CTAP2_ERR_PIN_NOT_SET;
    }
    else if (fido2_ctap_pin_is_set() && !req.pin_auth_present) {
        uv = false;
        g_assert_state.uv = false;
    }

    if (req.pin_auth_present && !pin_protocol_supported(req.pin_protocol)) {
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    if (req.options.up) {
#if IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
        up = true;
        g_assert_state.up = true;
#else
        if (fido2_ctap_utils_user_presence_test() == CTAP2_OK) {
            up = true;
            g_assert_state.up = true;
        }
        else {
            return CTAP2_ERR_OPERATION_DENIED;
        }
#endif
    }

    if (req.options.uv) {
        return CTAP2_ERR_UNSUPPORTED_OPTION;
    }

    if (g_assert_state.count == 0) {
        return CTAP2_ERR_NO_CREDENTIALS;
    }

    memmove(g_assert_state.client_data_hash, req.client_data_hash,
            sizeof(g_assert_state.client_data_hash));

    /* most recent (based on sign count) eligble rk found */
    rk = &g_assert_state.rks[0];

    g_assert_state.cred_counter++;

    /* last moment where transaction can be cancelled */
    if (should_cancel()) {
        return CTAP2_ERR_KEEPALIVE_CANCEL;
    }

    /* use global sign counter because has_nonce == true => not a resident key */
    if (rk->cred_desc.has_nonce) {
        ret = make_auth_data_assert(req.rp_id, req.rp_id_len, &auth_data, uv,
                                    up,
                                    g_state.sign_count);
    }
    else {
        ret = make_auth_data_assert(req.rp_id, req.rp_id_len, &auth_data, uv,
                                    up,
                                    rk->sign_count);
    }

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = fido2_ctap_cbor_encode_assertion_object(encoder, &auth_data,
                                                  req.client_data_hash, rk,
                                                  g_assert_state.count);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* use global sign_count if not resident key, per key sign count
     * if resident key.
     * webauthn specification (version 20190304) section 6.1.1
     */
    if (!rk->cred_desc.has_nonce) {
        rk->sign_count++;
        ret = save_rk(rk);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    g_state.sign_count++;
    ret = save_state(&g_state);
    if (ret != CTAP2_OK) {
        return ret;
    }

    /* start timer for get_next_assertion timeout */
    g_assert_state.timer = xtimer_now_usec();

    return CTAP2_OK;
}

static int get_next_assertion(CborEncoder *encoder)
{
    int ret;
    uint32_t now;
    ctap_resident_key_t *rk;
    ctap_auth_data_header_t auth_data = { 0 };

    /* no current valid assertion req pending */
    if (g_assert_state.timer == 0) {
        return CTAP2_ERR_NOT_ALLOWED;
    }

    if (g_assert_state.cred_counter >= g_assert_state.count) {
        return CTAP2_ERR_NOT_ALLOWED;
    }

    now = xtimer_now_usec();
    if (now - g_assert_state.timer > CTAP_GET_NEXT_ASSERTION_TIMEOUT) {
        memset(&g_assert_state, 0, sizeof(g_assert_state));
        return CTAP2_ERR_NOT_ALLOWED;
    }

    /* next eligble rk */
    rk = &g_assert_state.rks[g_assert_state.cred_counter];
    g_assert_state.cred_counter++;

    /* has_nonce == true => not a resident key */
    if (rk->cred_desc.has_nonce) {
        ret = make_auth_data_next_assert(rk->rp_id_hash, &auth_data,
                                         g_assert_state.uv, g_assert_state.up,
                                         g_state.sign_count);
    }
    else {
        ret = make_auth_data_next_assert(rk->rp_id_hash, &auth_data,
                                         g_assert_state.uv, g_assert_state.up,
                                         rk->sign_count);
    }

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* cred count set to 0 because omitted when get_next_assertion */
    ret = fido2_ctap_cbor_encode_assertion_object(encoder, &auth_data,
                                                  g_assert_state.client_data_hash, rk,
                                                  0);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* restart timer */
    g_assert_state.timer = xtimer_now_usec();

    /* use global sign_count of not resident key, per key sign count
     * if resident key.
     * webauthn specification (version 20190304) section 6.1.1
     */
    if (!rk->cred_desc.has_nonce) {
        rk->sign_count++;
        ret = save_rk(rk);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    g_state.sign_count++;
    ret = save_state(&g_state);
    if (ret != CTAP2_OK) {
        return ret;
    }

    return CTAP2_OK;
}

static uint8_t get_info(CborEncoder *encoder)
{
    ctap_info_t info = { 0 };

    info.versions |= CTAP_VERSION_FLAG_FIDO;
    memmove(info.aaguid, g_state.config.aaguid, sizeof(info.aaguid));
    info.options = g_state.config.options;
    info.max_msg_size = CTAP_MAX_MSG_SIZE;
    info.pin_protocol = CTAP_PIN_PROT_VER;
    info.pin_is_set = fido2_ctap_pin_is_set();

    return fido2_ctap_cbor_encode_info(encoder, &info);
}

static int client_pin(CborEncoder *encoder, size_t size, uint8_t *req_raw,
                      bool (*should_cancel)(void))
{
    int ret;
    ctap_client_pin_req_t req = { 0 };

    ret = fido2_ctap_cbor_parse_client_pin_req(&req, req_raw, size);

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: client_pin - error parsing request: %d \n", ret);
        return ret;
    }

    /* common error handling */
    if (req.sub_command != CTAP_CP_REQ_SUB_COMMAND_GET_RETRIES) {
        if (locked()) {
            return CTAP2_ERR_PIN_BLOCKED;
        }

        if (boot_locked()) {
            return CTAP2_ERR_PIN_AUTH_BLOCKED;
        }
    }

    if (req.pin_protocol != CTAP_PIN_PROT_VER) {
        return CTAP1_ERR_OTHER;
    }

    switch (req.sub_command) {
    case CTAP_CP_REQ_SUB_COMMAND_GET_RETRIES:
        ret = get_retries(encoder);
        break;
    case CTAP_CP_REQ_SUB_COMMAND_GET_KEY_AGREEMENT:
        ret = get_key_agreement(encoder);
        break;
    case CTAP_CP_REQ_SUB_COMMAND_SET_PIN:
        ret = set_pin(&req, should_cancel);
        break;
    case CTAP_CP_REQ_SUB_COMMAND_CHANGE_PIN:
        ret = change_pin(&req, should_cancel);
        break;
    case CTAP_CP_REQ_SUB_COMMAND_GET_PIN_TOKEN:
        ret = get_pin_token(encoder, &req, should_cancel);
        break;
    default:
        ret = CTAP1_ERR_OTHER;
        DEBUG("fido2_crap: clientpin - subcommand unknown %u \n",
              req.sub_command);
    }

    return ret;
}

static uint8_t get_retries(CborEncoder *encoder)
{
    return fido2_ctap_cbor_encode_retries(encoder,
                                          get_remaining_pin_attempts());
}

static int get_key_agreement(CborEncoder *encoder)
{
    ctap_cose_key_t key = { 0 };

    fido2_ctap_crypto_get_key_agreement(&key.pubkey);

    key.alg_type = CTAP_COSE_ALG_ECDH_ES_HKDF_256;
    key.cred_type = CTAP_PUB_KEY_CRED_PUB_KEY;
    key.crv = CTAP_COSE_KEY_CRV_P256;
    key.kty = CTAP_COSE_KEY_KTY_EC2;

    return fido2_ctap_cbor_encode_key_agreement(encoder, &key);
}

static int set_pin(ctap_client_pin_req_t *req, bool (*should_cancel)(void))
{
    uint8_t shared_key[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t shared_secret[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t hmac[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t new_pin_dec[CTAP_PIN_MAX_SIZE] = { 0 };
    size_t sz;
    int ret;

    if (!req->new_pin_enc_size || !req->pin_auth_present ||
        !req->key_agreement_present) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    if (fido2_ctap_pin_is_set()) {
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    if (req->new_pin_enc_size < CTAP_PIN_ENC_MIN_SIZE) {
        return CTAP1_ERR_OTHER;
    }

    ret = fido2_ctap_crypto_ecdh(shared_secret, sizeof(shared_secret),
                                 &req->key_agreement.pubkey);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* sha256 of shared secret ((abG).x) to obtain shared key */
    sha256(shared_secret, sizeof(shared_secret), shared_key);

    hmac_sha256(shared_key, sizeof(shared_key), req->new_pin_enc,
                req->new_pin_enc_size, hmac);

    if (memcmp(hmac, req->pin_auth, CTAP_PIN_AUTH_SZ) != 0) {
        DEBUG("fido2_ctap: set pin - hmac and pin_auth differ \n");
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    sz = sizeof(new_pin_dec);
    ret = fido2_ctap_crypto_aes_dec(new_pin_dec, &sz, req->new_pin_enc,
                                    req->new_pin_enc_size, shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: set pin - error while decrypting PIN \n");
        return ret;
    }

    /* last moment where transaction can be cancelled */
    if (should_cancel()) {
        return CTAP2_ERR_KEEPALIVE_CANCEL;
    }

    sz = fmt_strnlen((char *)new_pin_dec, CTAP_PIN_MAX_SIZE + 1);
    if (sz < CTAP_PIN_MIN_SIZE || sz > CTAP_PIN_MAX_SIZE) {
        return CTAP2_ERR_PIN_POLICY_VIOLATION;
    }

    save_pin(new_pin_dec, sz);

    reset_pin_attempts();

    return CTAP2_OK;
}

static int change_pin(ctap_client_pin_req_t *req, bool (*should_cancel)(void))
{
    int ret;
    size_t sz;
    hmac_context_t ctx;
    uint8_t shared_key[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t shared_secret[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t pin_hash_dec[CTAP_PIN_TOKEN_SIZE] = { 0 };
    uint8_t hmac[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t new_pin_dec[CTAP_PIN_MAX_SIZE] = { 0 };

    if (!req->pin_auth_present || !req->key_agreement_present ||
        !req->pin_hash_enc_present) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    if (!fido2_ctap_pin_is_set()) {
        return CTAP2_ERR_PIN_NOT_SET;
    }

    if (req->new_pin_enc_size < CTAP_PIN_ENC_MIN_SIZE) {
        return CTAP1_ERR_OTHER;
    }

    ret = fido2_ctap_crypto_ecdh(shared_secret, sizeof(shared_secret),
                                 &req->key_agreement.pubkey);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* sha256 of shared secret ((abG).x) to obtain shared key */
    sha256(shared_secret, sizeof(shared_secret), shared_key);

    hmac_sha256_init(&ctx, shared_key, sizeof(shared_key));
    hmac_sha256_update(&ctx, req->new_pin_enc, req->new_pin_enc_size);
    hmac_sha256_update(&ctx, req->pin_hash_enc, sizeof(req->pin_hash_enc));
    hmac_sha256_final(&ctx, hmac);

    if (memcmp(hmac, req->pin_auth, 16) != 0) {
        DEBUG("fido2_ctap: pin hmac and pin_auth differ \n");
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    sz = sizeof(pin_hash_dec);
    ret = fido2_ctap_crypto_aes_dec(pin_hash_dec, &sz, req->pin_hash_enc,
                                    sizeof(req->pin_hash_enc), shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: set pin - error while decrypting pin hash \n");
        return ret;
    }

    /* last moment where transaction can be cancelled */
    if (should_cancel()) {
        return CTAP2_ERR_KEEPALIVE_CANCEL;
    }

    if (memcmp(pin_hash_dec, g_state.pin_hash, 16) != 0) {
        DEBUG("fido2_ctap: get_pin_token - invalid pin \n");
        fido2_ctap_crypto_reset_key_agreement();
        save_state(&g_state);

        ret = decrement_pin_attempts();

        if (ret != CTAP2_OK) {
            return ret;
        }

        return CTAP2_ERR_PIN_INVALID;
    }

    reset_pin_attempts();

    sz = sizeof(new_pin_dec);
    ret = fido2_ctap_crypto_aes_dec(new_pin_dec, &sz, req->new_pin_enc,
                                    req->new_pin_enc_size, shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: set pin - error while decrypting new PIN \n");
        return ret;
    }

    sz = fmt_strnlen((char *)new_pin_dec, CTAP_PIN_MAX_SIZE + 1);
    if (sz < CTAP_PIN_MIN_SIZE || sz > CTAP_PIN_MAX_SIZE) {
        return CTAP2_ERR_PIN_POLICY_VIOLATION;
    }

    save_pin(new_pin_dec, sz);

    return CTAP2_OK;
}

static int get_pin_token(CborEncoder *encoder, ctap_client_pin_req_t *req,
                         bool (*should_cancel)(void))
{
    uint8_t shared_key[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t shared_secret[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t pin_hash_dec[CTAP_PIN_TOKEN_SIZE] = { 0 };
    uint8_t pin_token_enc[CTAP_PIN_TOKEN_SIZE] = { 0 };
    int ret;
    size_t sz;

    if (!fido2_ctap_pin_is_set()) {
        return CTAP2_ERR_PIN_NOT_SET;
    }

    if (!req->key_agreement_present || !req->pin_hash_enc_present) {
        return CTAP2_ERR_MISSING_PARAMETER;
    }

    ret = fido2_ctap_crypto_ecdh(shared_secret, sizeof(shared_secret),
                                 &req->key_agreement.pubkey);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* last moment where transaction can be cancelled */
    if (should_cancel()) {
        return CTAP2_ERR_KEEPALIVE_CANCEL;
    }

    /* sha256 of shared secret ((abG).x) to obtain shared key */
    sha256(shared_secret, sizeof(shared_secret), shared_key);

    sz = sizeof(pin_hash_dec);
    ret = fido2_ctap_crypto_aes_dec(pin_hash_dec, &sz, req->pin_hash_enc,
                                    sizeof(req->pin_hash_enc), shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("set pin: error while decrypting pin hash \n");
        return ret;
    }

    if (memcmp(pin_hash_dec, g_state.pin_hash, sizeof(g_state.pin_hash)) != 0) {
        DEBUG("fido2_ctap: get_pin_token - invalid pin \n");
        fido2_ctap_crypto_reset_key_agreement();

        ret = decrement_pin_attempts();

        if (ret != CTAP2_OK) {
            return ret;
        }

        return CTAP2_ERR_PIN_INVALID;
    }

    reset_pin_attempts();

    sz = sizeof(pin_token_enc);
    ret = fido2_ctap_crypto_aes_enc(pin_token_enc, &sz, g_pin_token,
                                    sizeof(g_pin_token), shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("get pin token: error encrypting pin token \n");
        return ret;
    }

    return fido2_ctap_cbor_encode_pin_token(encoder, pin_token_enc,
                                            sizeof(pin_token_enc));
}

static int save_pin(uint8_t *pin, size_t len)
{
    uint8_t buf[SHA256_DIGEST_LENGTH] = { 0 };

    sha256(pin, len, buf);
    memmove(g_state.pin_hash, buf, sizeof(g_state.pin_hash));
    g_state.pin_is_set = true;

    save_state(&g_state);

    return CTAP2_OK;
}

bool fido2_ctap_cred_params_supported(uint8_t cred_type, int32_t alg_type)
{
    if (cred_type == CTAP_PUB_KEY_CRED_PUB_KEY) {
        if (alg_type == CTAP_COSE_ALG_ES256) {
            return true;
        }
    }

    return false;
}

static inline bool pin_protocol_supported(uint8_t version)
{
    return version == CTAP_PIN_PROT_VER;
}

bool fido2_ctap_pin_is_set(void)
{
    return g_state.pin_is_set;
}

static inline bool locked(void)
{
    return g_state.rem_pin_att <= 0;
}

static inline bool boot_locked(void)
{
    return g_rem_pin_att_boot <= 0;
}

static int decrement_pin_attempts(void)
{
    g_state.rem_pin_att--;
    g_rem_pin_att_boot--;

    save_state(&g_state);

    if (g_state.rem_pin_att <= 0) {
        return CTAP2_ERR_PIN_BLOCKED;
    }

    if (g_rem_pin_att_boot <= 0) {
        return CTAP2_ERR_PIN_AUTH_BLOCKED;
    }

    DEBUG("rem pin att boot: %d \n", g_rem_pin_att_boot);

    return CTAP2_OK;
}

static inline uint8_t get_remaining_pin_attempts(void)
{
    return g_state.rem_pin_att;
}

static void reset_pin_attempts(void)
{
    g_state.rem_pin_att = CTAP_PIN_MAX_ATTS;
    g_rem_pin_att_boot = CTAP_PIN_MAX_ATTS_BOOT;

    save_state(&g_state);
}

static int verify_pin_auth(uint8_t *auth, uint8_t *hash, size_t len)
{
    uint8_t hmac[SHA256_DIGEST_LENGTH] = { 0 };

    hmac_sha256(g_pin_token, sizeof(g_pin_token), hash, len, hmac);

    if (memcmp(auth, hmac, 16) != 0) {
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    return CTAP2_OK;
}

static inline uint16_t get_page_offset(uint16_t i)
{
    return i / (FLASHPAGE_SIZE / CTAP_PAD_RK_SZ);
}

static inline uint16_t get_offset_into_page(uint16_t i)
{
    return CTAP_PAD_RK_SZ * (i % (FLASHPAGE_SIZE / CTAP_PAD_RK_SZ));
}

static bool rks_exist(ctap_cred_desc_alt_t *li, size_t len, uint8_t *rp_id,
                      size_t rp_id_len)
{
    uint8_t rp_id_hash[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t page[FLASHPAGE_SIZE] = { 0 };
    uint16_t page_offset = 0, page_offset_into_page = 0;
    ctap_resident_key_t rk;
    int ret;

    sha256(rp_id, rp_id_len, rp_id_hash);

    /* no rks stored, try decrypt only */
    if (g_state.rk_amount_stored == 0) {
        for (uint16_t i = 0; i < len; i++) {
            ret = ctap_decrypt_rk(&rk, &li[i].cred_id);
            if (ret == CTAP2_OK) {
                if (memcmp(rk.rp_id_hash, rp_id_hash, SHA256_DIGEST_LENGTH)
                    == 0) {
                    return true;
                }
            }
        }
    }

    for (uint16_t i = 0; i < g_state.rk_amount_stored; i++) {
        page_offset = get_page_offset(i);
        page_offset_into_page = get_offset_into_page(i);

        if (page_offset_into_page == 0) {
            fido2_ctap_mem_read(CTAP_RK_START_PAGE + page_offset, page);
        }

        memmove(&rk, page + page_offset_into_page, sizeof(rk));

        if (memcmp(rk.rp_id_hash, rp_id_hash, SHA256_DIGEST_LENGTH) == 0) {
            for (size_t j = 0; j < len; j++) {
                if (memcmp(li[j].cred_id.id, rk.cred_desc.cred_id,
                           CTAP_CREDENTIAL_ID_SIZE) == 0) {
                    return true;
                }
                else {
                    /* no match with stored key, try to decrypt */
                    ret = ctap_decrypt_rk(&rk, &li[i].cred_id);
                    if (ret == CTAP2_OK) {
                        if (memcmp(rk.rp_id_hash, rp_id_hash,
                                   SHA256_DIGEST_LENGTH) == 0) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

static uint8_t find_matching_rks(ctap_resident_key_t *rks, size_t rks_len,
                                 ctap_cred_desc_alt_t *allow_list,
                                 size_t allow_list_len, uint8_t *rp_id,
                                 size_t rp_id_len)
{
    uint8_t index = 0;
    uint8_t rp_id_hash[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t page[FLASHPAGE_SIZE] = { 0 };
    ctap_resident_key_t rk;
    uint16_t page_offset = 0;
    uint16_t page_offset_into_page = 0;
    int ret;

    sha256(rp_id, rp_id_len, rp_id_hash);

    /* no rks stored, try decrypt only */
    if (g_state.rk_amount_stored == 0) {
        for (uint16_t i = 0; i < allow_list_len; i++) {
            ret = ctap_decrypt_rk(&rks[index], &allow_list[i].cred_id);
            if (ret == CTAP2_OK) {
                if (memcmp(rks[index].rp_id_hash, rp_id_hash,
                           SHA256_DIGEST_LENGTH) == 0) {
                    index++;
                }
            }
        }
    }

    for (uint16_t i = 0; i < g_state.rk_amount_stored; i++) {
        if (index >= rks_len) {
            break;
        }
        page_offset = get_page_offset(i);
        page_offset_into_page = get_offset_into_page(i);

        if (page_offset_into_page == 0) {
            fido2_ctap_mem_read(CTAP_RK_START_PAGE + page_offset, page);
        }

        memmove(&rk, page + page_offset_into_page, sizeof(rk));

        /* search for rk's matching rp_id_hash */
        if (memcmp(rk.rp_id_hash, rp_id_hash, SHA256_DIGEST_LENGTH) == 0) {
            /* if allow list, also check that cred_id is in list */
            if (allow_list_len > 0) {
                for (size_t j = 0; j < allow_list_len; j++) {
                    if (memcmp(allow_list[j].cred_id.id, rk.cred_desc.cred_id,
                               sizeof(rk.cred_desc.cred_id)) == 0) {
                        memmove(&rks[index], &rk, sizeof(rks[index]));
                        index++;
                        break;
                    }
                    else {
                        /* no match with stored key, try to decrypt */
                        ret = ctap_decrypt_rk(&rks[index],
                                              &allow_list[j].cred_id);
                        if (ret == CTAP2_OK) {
                            if (memcmp(rks[index].rp_id_hash, rk.rp_id_hash,
                                       SHA256_DIGEST_LENGTH) == 0) {
                                index++;
                                break;
                            }
                        }
                    }
                }
            }
            else {
                /* no allow list so rp_id_hash match is enough */
                memmove(&rks[index], &rk, sizeof(rks[index]));
                index++;
            }
        }
    }

    /* sort ascending order based on sign count */
    if (g_state.rk_amount_stored > 0 && index > 0) {
        qsort(rks, index, sizeof(ctap_resident_key_t), fido2_ctap_utils_cred_cmp);
    }

    return index;
}

/**
 * overwrite existing key if equal, else find free space.
 *
 * The current official CTAP2 spec does not have credential management yet
 * so rk's can't be deleted, only overwritten => we can be sure that there are
 * no holes when reading keys from flash memory
 */
static int save_rk(ctap_resident_key_t *rk)
{
    bool equal = false;
    int ret;
    uint16_t page_offset = 0, page_offset_into_page = 0;
    uint8_t page[FLASHPAGE_SIZE] = { 0 };
    uint8_t buf[CTAP_PAD_RK_SZ] = { 0 };
    ctap_resident_key_t rk_temp = { 0 };

    if (g_state.rk_amount_stored >= CTAP_MAX_RK) {
        return CTAP2_ERR_KEY_STORE_FULL;
    }

    if (g_state.rk_amount_stored > 0) {
        for (uint16_t i = 0; i <= g_state.rk_amount_stored; i++) {
            page_offset = get_page_offset(i);
            page_offset_into_page = get_offset_into_page(i);

            if (i == g_state.rk_amount_stored) {
                break;
            }

            /* beginning of a new page, read from flash */
            if (page_offset_into_page == 0) {
                fido2_ctap_mem_read(CTAP_RK_START_PAGE + page_offset, page);
            }

            memmove(&rk_temp, page + page_offset_into_page, sizeof(rk_temp));

            /* if equal overwrite */
            if (fido2_ctap_utils_ks_equal(&rk_temp, rk)) {
                equal = true;
                break;
            }
        }
    }

    memmove(buf, rk, sizeof(*rk));

    if (!equal) {
        ret = fido2_ctap_mem_write_and_verify(CTAP_RK_START_PAGE + page_offset,
                                              page_offset_into_page, buf,
                                              sizeof(buf));

        if (ret != FLASHPAGE_OK) {
            return CTAP1_ERR_OTHER;
        }

        g_state.rk_amount_stored++;
        ret = save_state(&g_state);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }
    else {
        /* update page by moving new rk value into page */
        memmove(page + page_offset_into_page, buf, sizeof(buf));
        ret = fido2_ctap_mem_write_and_verify(CTAP_RK_START_PAGE + page_offset,
                                              0, page, sizeof(page));

        if (ret != FLASHPAGE_OK) {
            return CTAP1_ERR_OTHER;
        }
    }

    return CTAP2_OK;
}

static int make_auth_data_assert(uint8_t *rp_id, size_t rp_id_len,
                                 ctap_auth_data_header_t *auth_data, bool uv,
                                 bool up, uint32_t sign_count)
{
    sha256(rp_id, rp_id_len, auth_data->rp_id_hash);

    auth_data->sign_count = htonl(sign_count);

    if (up) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UP;
    }

    if (uv) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UV;
    }

    return CTAP2_OK;
}

static int make_auth_data_next_assert(uint8_t *rp_id_hash,
                                      ctap_auth_data_header_t *auth_data,
                                      bool uv, bool up, uint32_t sign_count)
{
    memmove(auth_data->rp_id_hash, rp_id_hash, sizeof(auth_data->rp_id_hash));

    auth_data->sign_count = htonl(sign_count);

    if (up) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UP;
    }

    if (uv) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UV;
    }

    return CTAP2_OK;
}

static int make_auth_data_attest(ctap_make_credential_req_t *req,
                                 ctap_auth_data_t *auth_data,
                                 ctap_resident_key_t *k,
                                 bool uv, bool up, bool rk)
{
    int ret;
    /* device aaguid */
    uint8_t aaguid[] = { CTAP_AAGUID };
    ctap_auth_data_header_t *auth_header = &auth_data->header;
    ctap_attested_cred_data_t *cred_data = &auth_data->attested_cred_data;
    ctap_attested_cred_data_header_t *cred_header = &cred_data->header;
    ctap_rp_ent_t *rp = &req->rp;
    ctap_user_ent_t *user = &req->user;

    memset(k, 0, sizeof(*k));
    memset(auth_data, 0, sizeof(*auth_data));

    sha256(rp->id, rp->id_len, auth_header->rp_id_hash);

    /* set flag indicating that attested credential data included */
    auth_header->flags |= CTAP_AUTH_DATA_FLAG_AT;

    if (up) {
        auth_header->flags |= CTAP_AUTH_DATA_FLAG_UP;
    }

    if (uv) {
        auth_header->flags |= CTAP_AUTH_DATA_FLAG_UV;
    }

    auth_header->sign_count = 0;

    memmove(cred_header->aaguid, aaguid, sizeof(cred_header->aaguid));

    ret = fido2_ctap_crypto_gen_keypair(&cred_data->key.pubkey, k->priv_key);

    if (ret != CTAP2_OK) {
        return ret;
    }

    cred_data->key.alg_type = req->alg_type;
    cred_data->key.cred_type = req->cred_type;
    cred_data->key.crv = CTAP_COSE_KEY_CRV_P256;
    cred_data->key.kty = CTAP_COSE_KEY_KTY_EC2;

    /* init key */
    k->cred_desc.cred_type = req->cred_type;
    k->user_id_len = user->id_len;

    memmove(k->user_id, user->id, user->id_len);
    memmove(k->rp_id_hash, auth_header->rp_id_hash, sizeof(k->rp_id_hash));

    if (rk) {
        /* generate credential id as 16 random bytes */
        fido2_ctap_crypto_prng(cred_header->cred_id.id,
                               CTAP_CREDENTIAL_ID_SIZE);
        memmove(k->cred_desc.cred_id, cred_header->cred_id.id,
                sizeof(k->cred_desc.cred_id));

        cred_header->cred_len_h = (CTAP_CREDENTIAL_ID_SIZE & 0xff00) >> 8;
        cred_header->cred_len_l = CTAP_CREDENTIAL_ID_SIZE & 0x00ff;
    }
    else {
        /* generate credential id by encrypting resident key */
        uint8_t nonce[CTAP_AES_CCM_NONCE_SIZE];
        fido2_ctap_crypto_prng(nonce, sizeof(nonce));

        ret = fido2_ctap_encrypt_rk(k, nonce, sizeof(nonce),
                                    &cred_header->cred_id);

        if (ret != CTAP2_OK) {
            return ret;
        }

        cred_header->cred_len_h = (sizeof(cred_header->cred_id) & 0xff00) >> 8;
        cred_header->cred_len_l = sizeof(cred_header->cred_id) & 0x00ff;
    }

    return CTAP2_OK;
}

int fido2_ctap_encrypt_rk(ctap_resident_key_t *rk, uint8_t *nonce,
                          size_t nonce_size, ctap_cred_id_t *id)
{
    int ret;

    ret = fido2_ctap_crypto_aes_ccm_enc((uint8_t *)id, sizeof(id),
                                        (uint8_t *)rk,
                                        CTAP_CREDENTIAL_ID_ENC_SIZE,
                                        NULL, 0,
                                        CCM_MAC_MAX_LEN, CTAP_AES_CCM_L,
                                        nonce, nonce_size,
                                        g_state.cred_key,
                                        sizeof(g_state.cred_key));

    if (ret != CTAP2_OK) {
        return ret;
    }

    memmove(id->nonce, nonce, CTAP_AES_CCM_NONCE_SIZE);

    return CTAP2_OK;
}

static int ctap_decrypt_rk(ctap_resident_key_t *rk, ctap_cred_id_t *id)
{
    int ret;

    ret = fido2_ctap_crypto_aes_ccm_dec((uint8_t *)rk, sizeof(*rk),
                                        (uint8_t *)id,
                                        sizeof(id->id) + sizeof(id->mac),
                                        NULL, 0,
                                        CCM_MAC_MAX_LEN, CTAP_AES_CCM_L,
                                        id->nonce, sizeof(id->nonce),
                                        g_state.cred_key,
                                        sizeof(g_state.cred_key));

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* store nonce in key to be able to later encrypt again */
    memmove(rk->cred_desc.nonce, id->nonce, CTAP_AES_CCM_NONCE_SIZE);
    rk->cred_desc.has_nonce = true;

    return CTAP2_OK;
}

static int save_state(const ctap_state_t *state)
{
    uint8_t page[CTAP_PAD_STATE_SZ] = { 0 };
    int ret;

    memmove(page, state, sizeof(*state));

    ret =
        fido2_ctap_mem_write_and_verify(CTAP_STATE_PAGE, 0, page, sizeof(page));

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

static void load_state(ctap_state_t *state)
{
    uint8_t page[FLASHPAGE_SIZE] = { 0 };

    fido2_ctap_mem_read(CTAP_STATE_PAGE, page);
    memmove(state, page, sizeof(*state));
}

int fido2_ctap_get_sig(const uint8_t *auth_data, size_t auth_data_len,
                       const uint8_t *client_data_hash,
                       const ctap_resident_key_t *rk,
                       uint8_t *sig, size_t *sig_len)
{
    sha256_context_t ctx;
    uint8_t hash[SHA256_DIGEST_LENGTH];

    sha256_init(&ctx);
    sha256_update(&ctx, auth_data, auth_data_len);
    sha256_update(&ctx, client_data_hash, SHA256_DIGEST_LENGTH);
    sha256_final(&ctx, hash);

    return fido2_ctap_crypto_get_sig(hash, sizeof(hash), sig, sig_len,
                                     rk->priv_key,
                                     sizeof(rk->priv_key));
}
