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
 * @author  Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "byteorder.h"
#include "errno.h"
#include "fmt.h"
#include "string_utils.h"
#include "ztimer.h"

#include "fido2/ctap/ctap.h"
#include "fido2/ctap/transport/ctap_transport.h"
#include "fido2/ctap.h"
#include "fido2/ctap/ctap_utils.h"
#include "fido2/ctap/ctap_cbor.h"
#include "fido2/ctap/ctap_mem.h"

#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
#include "fido2/ctap/transport/hid/ctap_hid.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief CTAP get_assertion state
 */
typedef struct {
    ctap_resident_key_t rks[CTAP_MAX_EXCLUDE_LIST_SIZE];    /**< eligible resident keys found */
    uint8_t count;                                          /**< number of rks found  */
    uint8_t cred_counter;                                   /**< amount of creds sent to host */
    uint32_t timer;                                         /**< time gap between get_next_assertion calls in milliseconds  */
    bool uv;                                                /**< indicate if user verified */
    bool up;                                                /**< indicate if user present */
    uint8_t client_data_hash[SHA256_DIGEST_LENGTH];         /**< SHA-256 hash of JSON serialized client data */
} ctap_get_assertion_state_t;

/*** CTAP methods ***/

/**
 * @brief MakeCredential method
 *
 * CTAP specification (version 20190130) section 5.1
 */
static int _make_credential(ctap_req_t *req_raw);

/**
 * @brief GetAssertion method
 *
 * CTAP specification (version 20190130) section 5.2
 */
static int _get_assertion(ctap_req_t *req_raw);

/**
 * @brief GetNextAssertion method
 *
 * CTAP specification (version 20190130) section 5.3
 */
static int _get_next_assertion(void);

/**
 * @brief GetInfo method
 *
 * CTAP specification (version 20190130) section 5.4
 */
static int _get_info(void);

/**
 * @brief ClientPIN method
 *
 * CTAP specification (version 20190130) section 5.5
 */
static int _client_pin(ctap_req_t *req_raw);

/**
 * @brief Reset method
 *
 * CTAP specification (version 20190130) section 5.6
 */
static int _reset(void);

/*** CTAP clientPIN functions ***/

/**
 * @brief ClientPIN getRetries method
 *
 * CTAP specification (version 20190130) section 5.5.3
 */
static int _get_retries(void);

/**
 * @brief ClientPIN getKeyAgreement method
 *
 * CTAP specification (version 20190130) section 5.5.4
 */
static int _get_key_agreement(void);

/**
 * @brief ClientPIN setPIN method
 *
 * CTAP specification (version 20190130) section 5.5.5
 */
static int _set_pin(ctap_client_pin_req_t *req);

/**
 * @brief ClientPIN changePIN method
 *
 * CTAP specification (version 20190130) section 5.5.6
 */
static int _change_pin(ctap_client_pin_req_t *req);

/**
 * @brief ClientPIN getPINToken method
 *
 * CTAP specification (version 20190130) section 5.5.7
 */
static int _get_pin_token(ctap_client_pin_req_t *req);

/*** helper functions ***/

/**
 * @brief Initialize authData of attestation object
 *
 * This function generates the public key pair used by the credential
 */
static int _make_auth_data_attest(ctap_make_credential_req_t *req,
                                  ctap_auth_data_t *auth_data,
                                  ctap_resident_key_t *k, bool uv, bool up,
                                  bool rk);
/**
 * @brief Initialize authData of assertion object
 */
static int _make_auth_data_assert(uint8_t *rp_id, size_t rp_id_len,
                                  ctap_auth_data_header_t *auth_data, bool uv,
                                  bool up, uint32_t sign_count);
/**
 * @brief Initialize authData of next assertion object
 */
static int _make_auth_data_next_assert(uint8_t *rp_id_hash,
                                       ctap_auth_data_header_t *auth_data,
                                       bool uv, bool up, uint32_t sign_count);
/**
 * @brief Find most recent rk matching rp_id_hash and present in allow_list
 */
static int _find_matching_rks(ctap_resident_key_t *rks, size_t rks_len,
                              ctap_cred_desc_alt_t *allow_list,
                              size_t allow_list_len,
                              uint8_t *rp_id, size_t rp_id_len);

/**
 * @brief Decrypt credential that is stored by relying party
 *
 * This function is used when the credential is stored by the
 * relying party in encrypted form.
 * See Webauthn specification (version 20190304) section 4 (Credential ID)
 * for more details.
 */
static int _ctap_decrypt_rk(ctap_resident_key_t *rk, ctap_cred_id_t *id);

/**
 * @brief Save PIN to authenticator state and write the updated state to flash
 */
static int _save_pin(uint8_t *pin, size_t len);

/**
 * @brief Check if PIN protocol version is supported
 */
static inline bool _pin_protocol_supported(uint8_t version);

/**
 * @brief Decrement PIN attempts and indicate if authenticator is locked
 *
 * Should the decrease of PIN attempts cross one of the two thresholds
 * ( @ref CTAP_PIN_MAX_ATTS_BOOT, @ref CTAP_PIN_MAX_ATTS ) this methods returns
 * an error code indicating the locking status.
 */
static int _decrement_pin_attempts(void);

/**
 * @brief Reset PIN attempts to its starting values
 */
static int _reset_pin_attempts(void);

/**
 * @brief Verify pinAuth sent by platform
 *
 * pinAuth is verified by comparing it to the first 16 bytes of
 * HMAC-SHA-256(pinToken, clientDataHash)
 */
static int _verify_pin_auth(uint8_t *auth, uint8_t *hash, size_t len);

/**
 * @brief Check if authenticator is boot locked
 *
 * Authenticator needs to be rebooted in order to be used again.
 */
static inline bool _is_boot_locked(void);

/**
 * @brief Check if authenticator is completely locked
 *
 * Authenticator needs to be reset, deleting all stored credentials, in order
 * to be used again.
 */
static inline bool _is_locked(void);

/**
 * @brief State of authenticator
 */
static ctap_state_t _state;

/**
 * @brief Assertion state
 *
 * The assertion state is needed to keep state between the GetAssertion and
 * GetNextAssertion methods.
 */
static ctap_get_assertion_state_t _assert_state;

/**
 * @brief pinToken
 *
 * The pinToken is used to reduce the cost of the PIN mechanism and increase
 * its security.
 *
 * See CTAP specification (version 20190130) section 5.5.2 for more details
 */
static uint8_t _pin_token[CTAP_PIN_TOKEN_SZ];

/**
 * @brief remaining PIN attempts until authenticator is boot locked
 */
static int _rem_pin_att_boot = CTAP_PIN_MAX_ATTS_BOOT;

ctap_status_code_t fido2_ctap_init(void)
{
    int ret;

    ret = fido2_ctap_mem_init();

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = fido2_ctap_mem_read_state_from_flash(&_state);
    if (ret != CTAP2_OK) {
        return ret;
    }

    /* first startup of the device */
    if (_state.initialized_marker != CTAP_INITIALIZED_MARKER) {
        ret = _reset();
        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
        ret = fido2_ctap_utils_init_gpio_pin(CTAP_UP_BUTTON, CTAP_UP_BUTTON_MODE,
                                             CTAP_UP_BUTTON_FLANK);
        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    ret = fido2_ctap_crypto_init();

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* initialize pin_token */
    ret = fido2_ctap_crypto_prng(_pin_token, sizeof(_pin_token));

    if (ret != CTAP2_OK) {
        return ret;
    }

    DEBUG("fido2_ctap: initialization successful \n");

    return CTAP2_OK;
}

ctap_status_code_t fido2_ctap_handle_request(ctap_req_t *req, ctap_resp_t *resp)
{
    assert(req);
    assert(resp);

    switch (req->method) {
    case CTAP_GET_INFO:
        DEBUG("fido2_ctap: get_info req \n");
        fido2_ctap_get_info(resp);
        break;
    case CTAP_MAKE_CREDENTIAL:
        DEBUG("fido2_ctap: make_credential req \n");
        fido2_ctap_make_credential(req, resp);
        break;
    case CTAP_GET_ASSERTION:
        DEBUG("fido2_ctap: get_assertion req \n");
        fido2_ctap_get_assertion(req, resp);
        break;
    case CTAP_GET_NEXT_ASSERTION:
        DEBUG("fido2_ctap: get_next_assertion req \n");
        fido2_ctap_get_next_assertion(resp);
        break;
    case CTAP_CLIENT_PIN:
        DEBUG("fido2_ctap: client_pin req \n");
        fido2_ctap_client_pin(req, resp);
        break;
    case CTAP_RESET:
        DEBUG("fido2_ctap: reset req \n");
        fido2_ctap_reset(resp);
        break;
    default:
        DEBUG("fido2_ctap: unknown req: %u \n", req->method);
        resp->status = CTAP1_ERR_INVALID_COMMAND;
        resp->len = 0x0;
        break;
    }

    DEBUG("Resp status %d \n", resp->status);

    return resp->status;
}

ctap_state_t *fido2_ctap_get_state(void)
{
    return &_state;
}

ctap_status_code_t fido2_ctap_get_info(ctap_resp_t *resp)
{
    assert(resp);

    fido2_ctap_cbor_init_encoder(resp->data, sizeof(resp->data));

    resp->status = _get_info();
    resp->len = fido2_ctap_cbor_get_buffer_size(resp->data);

    return resp->status;
}

ctap_status_code_t fido2_ctap_make_credential(ctap_req_t *req, ctap_resp_t *resp)
{
    assert(req);
    assert(resp);

    fido2_ctap_cbor_init_encoder(resp->data, sizeof(resp->data));

    resp->status = _make_credential(req);
    resp->len = fido2_ctap_cbor_get_buffer_size(resp->data);

    return resp->status;
}

ctap_status_code_t fido2_ctap_get_assertion(ctap_req_t *req, ctap_resp_t *resp)
{
    assert(req);
    assert(resp);

    fido2_ctap_cbor_init_encoder(resp->data, sizeof(resp->data));

    resp->status = _get_assertion(req);
    resp->len = fido2_ctap_cbor_get_buffer_size(resp->data);

    return resp->status;
}

ctap_status_code_t fido2_ctap_get_next_assertion(ctap_resp_t *resp)
{
    assert(resp);

    fido2_ctap_cbor_init_encoder(resp->data, sizeof(resp->data));

    resp->status = _get_next_assertion();
    resp->len = fido2_ctap_cbor_get_buffer_size(resp->data);

    return resp->status;
}

ctap_status_code_t fido2_ctap_client_pin(ctap_req_t *req, ctap_resp_t *resp)
{
    assert(req);
    assert(resp);

    fido2_ctap_cbor_init_encoder(resp->data, sizeof(resp->data));

    resp->status = _client_pin(req);
    resp->len = fido2_ctap_cbor_get_buffer_size(resp->data);

    return resp->status;
}

ctap_status_code_t fido2_ctap_reset(ctap_resp_t *resp)
{
    assert(resp);

    resp->status = _reset();
    resp->len = 0x0;

    return resp->status;
}

static uint32_t get_id(void)
{
    return _state.id_cnt++;
}

static int _reset(void)
{
    // If CTAP is initialized (marker is set), erase existing FIDO2 data from flash
    if (_state.initialized_marker == CTAP_INITIALIZED_MARKER) {
        int ret = fido2_ctap_mem_erase_flash(&_state);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    _state.initialized_marker = CTAP_INITIALIZED_MARKER;
    _state.rem_pin_att = CTAP_PIN_MAX_ATTS;
    _state.pin_is_set = false;
    _state.rk_amount_stored = 0;
    _state.id_cnt = 0;

    _rem_pin_att_boot = CTAP_PIN_MAX_ATTS_BOOT;

    /* invalidate AES CCM key */
    explicit_bzero(_state.cred_key, sizeof(_state.cred_key));
    _state.cred_key_is_initialized = false;

    _state.config.options |= CTAP_INFO_OPTIONS_FLAG_PLAT;
    _state.config.options |= CTAP_INFO_OPTIONS_FLAG_RK;
    _state.config.options |= CTAP_INFO_OPTIONS_FLAG_CLIENT_PIN;
    _state.config.options |= CTAP_INFO_OPTIONS_FLAG_UP;

    uint8_t aaguid[CTAP_AAGUID_SIZE];

    fmt_hex_bytes(aaguid, CTAP_AAGUID);

    memcpy(_state.config.aaguid, aaguid, sizeof(_state.config.aaguid));

    return fido2_ctap_mem_write_state_to_flash(&_state);
}

static int _make_credential(ctap_req_t *req_raw)
{
    int ret;
    bool uv = false;
    bool up = false;
    bool rk = false;
    ctap_make_credential_req_t req = { 0 };
    ctap_auth_data_t auth_data = { 0 };
    ctap_resident_key_t k = { 0 };

    if (_is_locked()) {
        ret = CTAP2_ERR_PIN_BLOCKED;
        goto done;
    }

    if (_is_boot_locked()) {
        ret = CTAP2_ERR_PIN_AUTH_BLOCKED;
        goto done;
    }

    /* set default values for options */
    req.options.rk = false;
    req.options.uv = false;
    req.options.up = -1;

    ret = fido2_ctap_cbor_parse_make_credential_req(&req, req_raw->buf, req_raw->len);

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* true => authenticator is instructed to store credential on device */
    rk = req.options.rk;

    if (req.exclude_list_len > 0) {
        if (_find_matching_rks(_assert_state.rks, CTAP_MAX_EXCLUDE_LIST_SIZE,
                               req.exclude_list, req.exclude_list_len, req.rp.id,
                               req.rp.id_len) > 0x0) {
            if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
                fido2_ctap_utils_user_presence_test();
            }

            ret = CTAP2_ERR_CREDENTIAL_EXCLUDED;
            goto done;
        }
    }

    /**
     * The user presence (up) check is mandatory for the MakeCredential method.
     * For the MakeCredential method the up key of the options dictionary,
     * which is part of the MakeCredential request, is not defined.
     * Therefore setting it is invalid for this method.
     */
    if (req.options.up != -1) {
        ret = CTAP2_ERR_INVALID_OPTION;
        goto done;
    }

    if (fido2_ctap_pin_is_set() && req.pin_auth_present) {
        /* CTAP specification (version 20190130) section 5.5.8.1 */
        if (req.pin_auth_len == 0) {
            if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
                fido2_ctap_utils_user_presence_test();
            }

            ret = CTAP2_ERR_PIN_INVALID;
            goto done;
        }

        ret = _verify_pin_auth(req.pin_auth, req.client_data_hash,
                               sizeof(req.client_data_hash));

        if (ret != CTAP2_OK) {
            goto done;
        }

        uv = true;
    }
    /* CTAP specification (version 20190130) section 5.5.8.1 */
    else if (!fido2_ctap_pin_is_set() && req.pin_auth_present && req.pin_auth_len == 0) {
        if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
            fido2_ctap_utils_user_presence_test();
        }

        ret = CTAP2_ERR_PIN_NOT_SET;
        goto done;
    }
    else if (fido2_ctap_pin_is_set() && !req.pin_auth_present) {
        ret = CTAP2_ERR_PIN_REQUIRED;
        goto done;
    }

    if (req.pin_auth_present && !_pin_protocol_supported(req.pin_protocol)) {
        ret = CTAP2_ERR_PIN_AUTH_INVALID;
        goto done;
    }

    /* last moment where transaction can be cancelled */
#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    if (fido2_ctap_transport_hid_should_cancel()) {
        ret = CTAP2_ERR_KEEPALIVE_CANCEL;
        goto done;
    }
#endif

    /* user presence test to create a new credential */
    if (IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
        up = true;
    }
    else {
        if (fido2_ctap_utils_user_presence_test() == CTAP2_OK) {
            up = true;
        }
    }

    ret = _make_auth_data_attest(&req, &auth_data, &k, uv, up, rk);

    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_cbor_encode_attestation_object(&auth_data, req.client_data_hash, &k);

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* if created credential is a resident credential, save it to flash */
    if (rk) {
        ret = fido2_ctap_mem_write_rk_to_flash(&k);
        if (ret != CTAP2_OK) {
            goto done;
        }
    }

    ret = CTAP2_OK;

done:
    /* clear rk to remove private key from memory */
    explicit_bzero(&k, sizeof(k));
    return ret;
}

static int _get_assertion(ctap_req_t *req_raw)
{
    int ret;
    bool uv = false;
    bool up = false;
    ctap_get_assertion_req_t req = { 0 };
    ctap_auth_data_header_t auth_data = { 0 };
    ctap_resident_key_t *rk = NULL;

    if (_is_locked()) {
        ret = CTAP2_ERR_PIN_BLOCKED;
        goto done;
    }

    if (_is_boot_locked()) {
        ret = CTAP2_ERR_PIN_AUTH_BLOCKED;
        goto done;
    }

    memset(&_assert_state, 0, sizeof(ctap_get_assertion_state_t));

    /* set default values for options */
    req.options.up = true;
    req.options.uv = false;

    ret = fido2_ctap_cbor_parse_get_assertion_req(&req, req_raw->buf, req_raw->len);

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* find eligible credentials */
    _assert_state.count = _find_matching_rks(_assert_state.rks,
                                             CTAP_MAX_EXCLUDE_LIST_SIZE,
                                             req.allow_list,
                                             req.allow_list_len, req.rp_id,
                                             req.rp_id_len);

    if (fido2_ctap_pin_is_set() && req.pin_auth_present) {
        /* CTAP specification (version 20190130) section 5.5.8.2 */
        if (req.pin_auth_len == 0) {
            if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
                fido2_ctap_utils_user_presence_test();
            }

            ret = CTAP2_ERR_PIN_INVALID;
            goto done;
        }
        ret = _verify_pin_auth(req.pin_auth, req.client_data_hash,
                               sizeof(req.client_data_hash));

        if (ret != CTAP2_OK) {
            goto done;
        }

        uv = true;
        _assert_state.uv = true;
    }
    /* CTAP specification (version 20190130) section 5.5.8.2 */
    else if (!fido2_ctap_pin_is_set() && req.pin_auth_present && req.pin_auth_len == 0) {
        if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
            fido2_ctap_utils_user_presence_test();
        }

        ret = CTAP2_ERR_PIN_NOT_SET;
        goto done;
    }
    else if (fido2_ctap_pin_is_set() && !req.pin_auth_present) {
        uv = false;
        _assert_state.uv = false;
    }

    if (req.pin_auth_present && !_pin_protocol_supported(req.pin_protocol)) {
        ret = CTAP2_ERR_PIN_AUTH_INVALID;
        goto done;
    }

    if (req.options.up) {
        if (IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)) {
            up = true;
            _assert_state.up = true;
        }
        else {
            if (fido2_ctap_utils_user_presence_test() == CTAP2_OK) {
                up = true;
                _assert_state.up = true;
            }
            else {
                ret = CTAP2_ERR_OPERATION_DENIED;
                goto done;
            }
        }
    }

    if (req.options.uv) {
        ret = CTAP2_ERR_UNSUPPORTED_OPTION;
        goto done;
    }

    if (_assert_state.count == 0) {
        ret = CTAP2_ERR_NO_CREDENTIALS;
        goto done;
    }

    memcpy(_assert_state.client_data_hash, req.client_data_hash,
           SHA256_DIGEST_LENGTH);

    /* most recently created eligible rk found */
    rk = &_assert_state.rks[_assert_state.cred_counter++];

    /* last moment where transaction can be cancelled */
#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    if (fido2_ctap_transport_hid_should_cancel()) {
        ret = CTAP2_ERR_KEEPALIVE_CANCEL;
        goto done;
    }
#endif

    ret = _make_auth_data_assert(req.rp_id, req.rp_id_len, &auth_data, uv,
                                 up,
                                 rk->sign_count);

    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_cbor_encode_assertion_object(&auth_data,
                                                  req.client_data_hash, rk,
                                                  _assert_state.count);

    if (ret != CTAP2_OK) {
        goto done;
    }

    rk->sign_count++;

    /**
     * if has_nonce is false, the credential is a resident credential and
     * therefore needs to be saved on the device.
     */
    if (!rk->cred_desc.has_nonce) {
        ret = fido2_ctap_mem_write_rk_to_flash(rk);

        if (ret != CTAP2_OK) {
            goto done;
        }
    }

    /* save current time for get_next_assertion timeout */
    _assert_state.timer = ztimer_now(ZTIMER_MSEC);

    ret = CTAP2_OK;

done:
    /* clear rk to remove private key from memory */
    if (rk) {
        explicit_bzero(rk, sizeof(*rk));
    }
    return ret;
}

static int _get_next_assertion(void)
{
    int ret;
    uint32_t now;
    ctap_resident_key_t *rk = NULL;
    ctap_auth_data_header_t auth_data = { 0 };

    if (_is_locked()) {
        ret = CTAP2_ERR_PIN_BLOCKED;
        goto done;
    }

    if (_is_boot_locked()) {
        ret = CTAP2_ERR_PIN_AUTH_BLOCKED;
        goto done;
    }

    /* no current valid assertion req pending */
    if (_assert_state.timer == 0) {
        ret = CTAP2_ERR_NOT_ALLOWED;
        goto done;
    }

    if (_assert_state.cred_counter >= _assert_state.count) {
        ret = CTAP2_ERR_NOT_ALLOWED;
        goto done;
    }

    now = ztimer_now(ZTIMER_MSEC);
    if (now - _assert_state.timer > CTAP_GET_NEXT_ASSERTION_TIMEOUT) {
        memset(&_assert_state, 0, sizeof(_assert_state));
        ret = CTAP2_ERR_NOT_ALLOWED;
        goto done;
    }

    /* next eligible rk */
    rk = &_assert_state.rks[_assert_state.cred_counter];
    _assert_state.cred_counter++;

    ret = _make_auth_data_next_assert(rk->rp_id_hash, &auth_data,
                                      _assert_state.uv, _assert_state.up,
                                      rk->sign_count);

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* cred count set to 0 because omitted when get_next_assertion */
    ret = fido2_ctap_cbor_encode_assertion_object(&auth_data,
                                                  _assert_state.client_data_hash, rk,
                                                  0);

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* restart timer */
    _assert_state.timer = ztimer_now(ZTIMER_MSEC);

    rk->sign_count++;

    /**
     * if has_nonce is false, the credential is a resident credential and
     * therefore needs to be saved on the device.
     */
    if (!rk->cred_desc.has_nonce) {
        ret = fido2_ctap_mem_write_rk_to_flash(rk);

        if (ret != CTAP2_OK) {
            goto done;
        }
    }

    ret = CTAP2_OK;

done:
    /* clear rk to remove private key from memory */
    if (rk) {
        explicit_bzero(rk, sizeof(*rk));
    }
    return ret;
}

static int _get_info(void)
{
    ctap_info_t info = { 0 };

    info.versions |= CTAP_VERSION_FLAG_FIDO;
    info.options = _state.config.options;
    info.max_msg_size = CTAP_MAX_MSG_SIZE;
    info.pin_protocol = CTAP_PIN_PROT_VER;
    info.pin_is_set = fido2_ctap_pin_is_set();
    memcpy(info.aaguid, _state.config.aaguid, CTAP_AAGUID_SIZE);

    return fido2_ctap_cbor_encode_info(&info);
}

static int _client_pin(ctap_req_t *req_raw)
{
    int ret;
    ctap_client_pin_req_t req = { 0 };

    ret = fido2_ctap_cbor_parse_client_pin_req(&req, req_raw->buf, req_raw->len);

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: client_pin - error parsing request: %d \n", ret);
        return ret;
    }

    /* common error handling */
    if (req.sub_command != CTAP_PIN_GET_RETRIES) {
        if (_is_locked()) {
            return CTAP2_ERR_PIN_BLOCKED;
        }

        if (_is_boot_locked()) {
            return CTAP2_ERR_PIN_AUTH_BLOCKED;
        }
    }

    if (req.pin_protocol != CTAP_PIN_PROT_VER) {
        return CTAP1_ERR_OTHER;
    }

    switch (req.sub_command) {
    case CTAP_PIN_GET_RETRIES:
        ret = _get_retries();
        break;
    case CTAP_PIN_GET_KEY_AGREEMENT:
        ret = _get_key_agreement();
        break;
    case CTAP_PIN_SET_PIN:
        ret = _set_pin(&req);
        break;
    case CTAP_PIN_CHANGE_PIN:
        ret = _change_pin(&req);
        break;
    case CTAP_PIN_GET_PIN_TOKEN:
        ret = _get_pin_token(&req);
        break;
    default:
        ret = CTAP1_ERR_OTHER;
        DEBUG("fido2_crap: clientpin - subcommand unknown %u \n",
              req.sub_command);
    }

    return ret;
}

static int _get_retries(void)
{
    return fido2_ctap_cbor_encode_retries(_state.rem_pin_att);
}

static int _get_key_agreement(void)
{
    int ret;
    ctap_public_key_cose_t key = { 0 };

    /* generate key agreement key */
    ret =
        fido2_ctap_crypto_gen_keypair(&_state.ag_key.pub, _state.ag_key.priv,
                                      sizeof(_state.ag_key.priv));

    if (ret != CTAP2_OK) {
        return ret;
    }

    memcpy(key.pubkey.x, _state.ag_key.pub.x, CTAP_CRYPTO_KEY_SIZE);
    memcpy(key.pubkey.y, _state.ag_key.pub.y, CTAP_CRYPTO_KEY_SIZE);

    key.alg_type = CTAP_COSE_ALG_ECDH_ES_HKDF_256;
    key.cred_type = CTAP_PUB_KEY_CRED_PUB_KEY;
    key.crv = CTAP_COSE_KEY_CRV_P256;
    key.kty = CTAP_COSE_KEY_KTY_EC2;

    return fido2_ctap_cbor_encode_key_agreement(&key);
}

static int _set_pin(ctap_client_pin_req_t *req)
{
    uint8_t shared_key[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t shared_secret[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t hmac[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t new_pin_dec[CTAP_PIN_MAX_SIZE] = { 0 };
    size_t sz;
    int ret;

    if (!req->new_pin_enc_size || !req->pin_auth_present ||
        !req->key_agreement_present) {
        ret = CTAP2_ERR_MISSING_PARAMETER;
        goto done;
    }

    if (fido2_ctap_pin_is_set()) {
        ret = CTAP2_ERR_PIN_AUTH_INVALID;
        goto done;
    }

    if (req->new_pin_enc_size < CTAP_PIN_ENC_MIN_SIZE) {
        ret = CTAP1_ERR_OTHER;
        goto done;
    }

    ret = fido2_ctap_crypto_ecdh(shared_secret, sizeof(shared_secret),
                                 &req->key_agreement.pubkey, _state.ag_key.priv,
                                 sizeof(_state.ag_key.priv));

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* sha256 of shared secret ((abG).x) to obtain shared key */
    ret = fido2_ctap_crypto_sha256(shared_secret, sizeof(shared_secret), shared_key);

    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_crypto_hmac_sha256(shared_key, sizeof(shared_key), req->new_pin_enc,
                                        req->new_pin_enc_size, hmac);

    if (ret != CTAP2_OK) {
        goto done;
    }

    if (memcmp(hmac, req->pin_auth, CTAP_PIN_AUTH_SZ) != 0) {
        DEBUG("fido2_ctap: set pin - hmac and pin_auth differ \n");
        ret = CTAP2_ERR_PIN_AUTH_INVALID;
        goto done;
    }

    sz = sizeof(new_pin_dec);
    ret = fido2_ctap_crypto_aes_dec(new_pin_dec, &sz, req->new_pin_enc,
                                    req->new_pin_enc_size, shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: set pin - error while decrypting PIN \n");
        goto done;
    }

    /* last moment where transaction can be cancelled */
#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    if (fido2_ctap_transport_hid_should_cancel()) {
        ret = CTAP2_ERR_KEEPALIVE_CANCEL;
        goto done;
    }
#endif

    sz = fmt_strnlen((char *)new_pin_dec, CTAP_PIN_MAX_SIZE + 1);
    if (sz < CTAP_PIN_MIN_SIZE || sz > CTAP_PIN_MAX_SIZE) {
        ret = CTAP2_ERR_PIN_POLICY_VIOLATION;
        goto done;
    }

    ret = _save_pin(new_pin_dec, sz);
    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = _reset_pin_attempts();
    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = CTAP2_OK;

done:
    /* clear key agreement key */
    explicit_bzero(&_state.ag_key, sizeof(_state.ag_key));
    return ret;
}

static int _change_pin(ctap_client_pin_req_t *req)
{
    int ret;
    size_t sz;
    hmac_context_t ctx;
    uint8_t shared_key[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t shared_secret[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t pin_hash_dec[CTAP_PIN_TOKEN_SZ] = { 0 };
    uint8_t hmac[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t new_pin_dec[CTAP_PIN_MAX_SIZE] = { 0 };

    if (!req->pin_auth_present || !req->key_agreement_present ||
        !req->pin_hash_enc_present) {
        ret = CTAP2_ERR_MISSING_PARAMETER;
        goto done;
    }

    if (!fido2_ctap_pin_is_set()) {
        ret = CTAP2_ERR_PIN_NOT_SET;
        goto done;
    }

    if (req->new_pin_enc_size < CTAP_PIN_ENC_MIN_SIZE) {
        ret = CTAP1_ERR_OTHER;
        goto done;
    }

    /* derive shared secret */
    ret = fido2_ctap_crypto_ecdh(shared_secret, sizeof(shared_secret),
                                 &req->key_agreement.pubkey, _state.ag_key.priv,
                                 sizeof(_state.ag_key.priv));
    if (ret != CTAP2_OK) {
        goto done;
    }

    /* sha256 of shared secret ((abG).x) to obtain shared key */
    ret = fido2_ctap_crypto_sha256(shared_secret, sizeof(shared_secret), shared_key);
    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_crypto_hmac_sha256_init(&ctx, shared_key, sizeof(shared_key));
    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_crypto_hmac_sha256_update(&ctx, req->new_pin_enc, req->new_pin_enc_size);
    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_crypto_hmac_sha256_update(&ctx, req->pin_hash_enc, sizeof(req->pin_hash_enc));
    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = fido2_ctap_crypto_hmac_sha256_final(&ctx, hmac);
    if (ret != CTAP2_OK) {
        goto done;
    }

    /* verify pinAuth by comparing first 16 bytes of HMAC-SHA-256*/
    if (memcmp(hmac, req->pin_auth, CTAP_PIN_AUTH_SZ) != 0) {
        DEBUG("fido2_ctap: pin hmac and pin_auth differ \n");
        ret = CTAP2_ERR_PIN_AUTH_INVALID;
        goto done;
    }

    sz = sizeof(pin_hash_dec);
    /* decrypt pinHashEnc */
    ret = fido2_ctap_crypto_aes_dec(pin_hash_dec, &sz, req->pin_hash_enc,
                                    sizeof(req->pin_hash_enc), shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: set pin - error while decrypting pin hash \n");
        goto done;
    }

    /* last moment where transaction can be cancelled */
#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    if (fido2_ctap_transport_hid_should_cancel()) {
        ret = CTAP2_ERR_KEEPALIVE_CANCEL;
        goto done;
    }
#endif
    /* verify decrypted pinHash against LEFT(SHA-256(curPin), 16) */
    if (memcmp(pin_hash_dec, _state.pin_hash, CTAP_PIN_TOKEN_SZ) != 0) {
        DEBUG("fido2_ctap: _get_pin_token - invalid pin \n");

        /* reset key agreement key */
        ret =
            fido2_ctap_crypto_gen_keypair(&_state.ag_key.pub, _state.ag_key.priv,
                                          sizeof(_state.ag_key.priv));

        if (ret != CTAP2_OK) {
            goto done;
        }

        ret = fido2_ctap_mem_write_state_to_flash(&_state);
        if (ret != CTAP2_OK) {
            goto done;
        }

        ret = _decrement_pin_attempts();

        if (ret != CTAP2_OK) {
            goto done;
        }

        ret = CTAP2_ERR_PIN_INVALID;
        goto done;
    }

    ret = _reset_pin_attempts();
    if (ret != CTAP2_OK) {
        goto done;
    }

    sz = sizeof(new_pin_dec);
    /* decrypt newPinEnc to obtain newPin */
    ret = fido2_ctap_crypto_aes_dec(new_pin_dec, &sz, req->new_pin_enc,
                                    req->new_pin_enc_size, shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("fido2_ctap: set pin - error while decrypting new PIN \n");
        goto done;
    }

    sz = fmt_strnlen((char *)new_pin_dec, CTAP_PIN_MAX_SIZE + 1);
    if (sz < CTAP_PIN_MIN_SIZE || sz > CTAP_PIN_MAX_SIZE) {
        ret = CTAP2_ERR_PIN_POLICY_VIOLATION;
        goto done;
    }

    ret = _save_pin(new_pin_dec, sz);
    if (ret != CTAP2_OK) {
        goto done;
    }

    ret = CTAP2_OK;

done:
    /* clear key agreement key */
    explicit_bzero(&_state.ag_key, sizeof(_state.ag_key));
    return ret;
}

static int _get_pin_token(ctap_client_pin_req_t *req)
{
    uint8_t shared_key[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t shared_secret[CTAP_CRYPTO_KEY_SIZE] = { 0 };
    uint8_t pin_hash_dec[CTAP_PIN_TOKEN_SZ] = { 0 };
    uint8_t pin_token_enc[CTAP_PIN_TOKEN_SZ] = { 0 };
    int ret;
    size_t sz;

    if (!fido2_ctap_pin_is_set()) {
        ret = CTAP2_ERR_PIN_NOT_SET;
        goto done;
    }

    if (!req->key_agreement_present || !req->pin_hash_enc_present) {
        ret = CTAP2_ERR_MISSING_PARAMETER;
        goto done;
    }

    ret = fido2_ctap_crypto_ecdh(shared_secret, sizeof(shared_secret),
                                 &req->key_agreement.pubkey, _state.ag_key.priv,
                                 sizeof(_state.ag_key.priv));

    if (ret != CTAP2_OK) {
        goto done;
    }

    /* last moment where transaction can be cancelled */
#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    if (fido2_ctap_transport_hid_should_cancel()) {
        ret = CTAP2_ERR_KEEPALIVE_CANCEL;
        goto done;
    }
#endif
    /* sha256 of shared secret ((abG).x) to obtain shared key */
    ret = fido2_ctap_crypto_sha256(shared_secret, sizeof(shared_secret), shared_key);

    if (ret != CTAP2_OK) {
        goto done;
    }

    sz = sizeof(pin_hash_dec);
    ret = fido2_ctap_crypto_aes_dec(pin_hash_dec, &sz, req->pin_hash_enc,
                                    sizeof(req->pin_hash_enc), shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("set pin: error while decrypting pin hash \n");
        goto done;
    }

    if (memcmp(pin_hash_dec, _state.pin_hash, sizeof(_state.pin_hash)) != 0) {
        DEBUG("fido2_ctap: _get_pin_token - invalid pin \n");

        /* reset key agreement key */
        ret =
            fido2_ctap_crypto_gen_keypair(&_state.ag_key.pub, _state.ag_key.priv,
                                          sizeof(_state.ag_key.priv));

        if (ret != CTAP2_OK) {
            goto done;
        }

        ret = _decrement_pin_attempts();

        if (ret != CTAP2_OK) {
            goto done;
        }

        ret = CTAP2_ERR_PIN_INVALID;
        goto done;
    }

    ret = _reset_pin_attempts();
    if (ret != CTAP2_OK) {
        goto done;
    }

    sz = sizeof(pin_token_enc);
    ret = fido2_ctap_crypto_aes_enc(pin_token_enc, &sz, _pin_token,
                                    sizeof(_pin_token), shared_key,
                                    sizeof(shared_key));

    if (ret != CTAP2_OK) {
        DEBUG("get pin token: error encrypting pin token \n");
        goto done;
    }

    ret = fido2_ctap_cbor_encode_pin_token(pin_token_enc,
                                           sizeof(pin_token_enc));

done:
    /* clear key agreement key */
    explicit_bzero(&_state.ag_key, sizeof(_state.ag_key));
    return ret;
}

static int _save_pin(uint8_t *pin, size_t len)
{
    uint8_t buf[SHA256_DIGEST_LENGTH] = { 0 };
    int ret;

    /* store LEFT(SHA-256(newPin), 16) */
    ret = fido2_ctap_crypto_sha256(pin, len, buf);

    if (ret != CTAP2_OK) {
        return ret;
    }

    memcpy(_state.pin_hash, buf, sizeof(_state.pin_hash));
    _state.pin_is_set = true;

    return fido2_ctap_mem_write_state_to_flash(&_state);
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

static inline bool _pin_protocol_supported(uint8_t version)
{
    return version == CTAP_PIN_PROT_VER;
}

bool fido2_ctap_pin_is_set(void)
{
    return _state.pin_is_set;
}

static inline bool _is_locked(void)
{
    return _state.rem_pin_att <= 0;
}

static inline bool _is_boot_locked(void)
{
    return _rem_pin_att_boot <= 0;
}

static int _decrement_pin_attempts(void)
{
    int ret;

    if (_state.rem_pin_att > 0) {
        _state.rem_pin_att--;
    }

    if (_rem_pin_att_boot > 0) {
        _rem_pin_att_boot--;
    }

    ret = fido2_ctap_mem_write_state_to_flash(&_state);
    if (ret != CTAP2_OK) {
        return ret;
    }

    if (_is_locked()) {
        return CTAP2_ERR_PIN_BLOCKED;
    }

    if (_is_boot_locked()) {
        return CTAP2_ERR_PIN_AUTH_BLOCKED;
    }

    return CTAP2_OK;
}

static int _reset_pin_attempts(void)
{
    _state.rem_pin_att = CTAP_PIN_MAX_ATTS;
    _rem_pin_att_boot = CTAP_PIN_MAX_ATTS_BOOT;

    return fido2_ctap_mem_write_state_to_flash(&_state);
}

static int _verify_pin_auth(uint8_t *auth, uint8_t *hash, size_t len)
{
    int ret;
    uint8_t hmac[SHA256_DIGEST_LENGTH] = { 0 };

    ret = fido2_ctap_crypto_hmac_sha256(_pin_token, sizeof(_pin_token), hash, len, hmac);

    if (ret != CTAP2_OK) {
        return ret;
    }

    if (memcmp(auth, hmac, CTAP_PIN_AUTH_SZ) != 0) {
        return CTAP2_ERR_PIN_AUTH_INVALID;
    }

    return CTAP2_OK;
}

static int _find_matching_rks(ctap_resident_key_t *rks, size_t rks_len,
                              ctap_cred_desc_alt_t *allow_list,
                              size_t allow_list_len, uint8_t *rp_id,
                              size_t rp_id_len)
{
    uint8_t index = 0;
    uint8_t rp_id_hash[SHA256_DIGEST_LENGTH] = { 0 };
    int ret;

    ret = fido2_ctap_crypto_sha256(rp_id, rp_id_len, rp_id_hash);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* no rks stored, try decrypt only */
    if (_state.rk_amount_stored == 0) {
        for (uint16_t i = 0; i < allow_list_len; i++) {
            ret = _ctap_decrypt_rk(&rks[index], &allow_list[i].cred_id);
            if (ret == CTAP2_OK) {
                if (memcmp(rks[index].rp_id_hash, rp_id_hash,
                           SHA256_DIGEST_LENGTH) == 0) {
                    index++;
                }
            }
        }
    }

    ctap_resident_key_t rk = { 0 };
    void *state = NULL;

    while (fido2_ctap_mem_rk_iter(&rk, &state) == CTAP2_OK) {
        if (memcmp(rk.rp_id_hash, rp_id_hash, sizeof(rp_id_hash)) != 0) {
            continue;
        }
        if (allow_list_len == 0) {
            memcpy(&rks[index], &rk, sizeof(rk));
            index++;
        }

        for (size_t i = 0; i < allow_list_len; i++) {
            /* if allow list is present, check that cred_id is in list */
            if (memcmp(allow_list[i].cred_id.id, rk.cred_desc.cred_id,
                       sizeof(rk.cred_desc.cred_id)) == 0) {
                memcpy(&rks[index], &rk, sizeof(rk));
                index++;
                break;
            }
            else {
                /* no match with stored key, try to decrypt */
                ret = _ctap_decrypt_rk(&rks[index],
                                       &allow_list[i].cred_id);
                if (ret == CTAP2_OK) {
                    if (memcmp(rks[index].rp_id_hash, rk.rp_id_hash,
                               SHA256_DIGEST_LENGTH) == 0) {
                        index++;
                        break;
                    }
                }
            }
        }

        if (index >= rks_len) {
            break;
        }
    }

    /**
     * Sort in descending order based on id. Credential with the
     * highest (most recent) id will be first in list.
     */
    if (index > 1) {
        qsort(rks, index, sizeof(ctap_resident_key_t), fido2_ctap_utils_cred_cmp);
    }

    return index;
}

static int _make_auth_data_assert(uint8_t *rp_id, size_t rp_id_len,
                                  ctap_auth_data_header_t *auth_data, bool uv,
                                  bool up, uint32_t sign_count)
{
    int ret;

    ret = fido2_ctap_crypto_sha256(rp_id, rp_id_len, auth_data->rp_id_hash);

    if (ret != CTAP2_OK) {
        return ret;
    }

    auth_data->sign_count = htonl(sign_count);

    if (up) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UP;
    }

    if (uv) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UV;
    }

    return CTAP2_OK;
}

static int _make_auth_data_next_assert(uint8_t *rp_id_hash,
                                       ctap_auth_data_header_t *auth_data,
                                       bool uv, bool up, uint32_t sign_count)
{
    memcpy(auth_data->rp_id_hash, rp_id_hash, sizeof(auth_data->rp_id_hash));

    auth_data->sign_count = htonl(sign_count);

    if (up) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UP;
    }

    if (uv) {
        auth_data->flags |= CTAP_AUTH_DATA_FLAG_UV;
    }

    return CTAP2_OK;
}

static int _make_auth_data_attest(ctap_make_credential_req_t *req,
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

    ret = fido2_ctap_crypto_sha256(rp->id, rp->id_len, auth_header->rp_id_hash);

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* set flag indicating that attested credential data included */
    auth_header->flags |= CTAP_AUTH_DATA_FLAG_AT;

    if (up) {
        auth_header->flags |= CTAP_AUTH_DATA_FLAG_UP;
    }

    if (uv) {
        auth_header->flags |= CTAP_AUTH_DATA_FLAG_UV;
    }

    auth_header->sign_count = 0;

    memcpy(cred_header->aaguid, aaguid, sizeof(cred_header->aaguid));

    ret =
        fido2_ctap_crypto_gen_keypair(&cred_data->key.pubkey, k->priv_key,
                                      sizeof(_state.ag_key.priv));

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
    k->id = get_id();

    memcpy(k->user_id, user->id, user->id_len);
    memcpy(k->rp_id_hash, auth_header->rp_id_hash, SHA256_DIGEST_LENGTH);

    if (rk) {
        /* generate credential id as 16 random bytes */
        ret = fido2_ctap_crypto_prng(cred_header->cred_id.id,
                                     CTAP_CREDENTIAL_ID_SIZE);

        if (ret != CTAP2_OK) {
            return ret;
        }

        memcpy(k->cred_desc.cred_id, cred_header->cred_id.id,
               sizeof(k->cred_desc.cred_id));

        cred_header->cred_len_h = (CTAP_CREDENTIAL_ID_SIZE & 0xff00) >> 8;
        cred_header->cred_len_l = CTAP_CREDENTIAL_ID_SIZE & 0x00ff;
    }
    else {
        /* generate credential id by encrypting resident key */
        uint8_t nonce[CTAP_AES_CCM_NONCE_SIZE];
        ret = fido2_ctap_crypto_prng(nonce, sizeof(nonce));

        if (ret != CTAP2_OK) {
            return ret;
        }

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
                          size_t nonce_len, ctap_cred_id_t *id)
{
    assert(rk);
    assert(nonce);
    assert(id);

    int ret;

    /**
     * If not initialized, create a new AES_CCM key to be able to encrypt a
     * credential when it is not a resident credential and therefore
     * will be stored by the relying party.
     */
    if (!_state.cred_key_is_initialized) {
        ret = fido2_ctap_crypto_prng(_state.cred_key, sizeof(_state.cred_key));

        if (ret != CTAP2_OK) {
            return ret;
        }

        _state.cred_key_is_initialized = true;

        ret = fido2_ctap_mem_write_state_to_flash(&_state);
        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    ret = fido2_ctap_crypto_aes_ccm_enc((uint8_t *)id, sizeof(id),
                                        (uint8_t *)rk,
                                        CTAP_CREDENTIAL_ID_ENC_SIZE,
                                        NULL, 0,
                                        CCM_MAC_MAX_LEN, CTAP_AES_CCM_L,
                                        nonce, nonce_len,
                                        _state.cred_key,
                                        sizeof(_state.cred_key));

    if (ret != CTAP2_OK) {
        return ret;
    }

    memcpy(id->nonce, nonce, CTAP_AES_CCM_NONCE_SIZE);

    return CTAP2_OK;
}

static int _ctap_decrypt_rk(ctap_resident_key_t *rk, ctap_cred_id_t *id)
{
    int ret;

    ret = fido2_ctap_crypto_aes_ccm_dec((uint8_t *)rk, sizeof(*rk),
                                        (uint8_t *)id,
                                        sizeof(id->id) + sizeof(id->mac),
                                        NULL, 0,
                                        CCM_MAC_MAX_LEN, CTAP_AES_CCM_L,
                                        id->nonce, sizeof(id->nonce),
                                        _state.cred_key,
                                        sizeof(_state.cred_key));

    if (ret != CTAP2_OK) {
        return ret;
    }

    /* store nonce in key to be able to later encrypt again */
    memcpy(rk->cred_desc.nonce, id->nonce, CTAP_AES_CCM_NONCE_SIZE);
    rk->cred_desc.has_nonce = true;

    return CTAP2_OK;
}

int fido2_ctap_get_sig(const uint8_t *auth_data, size_t auth_data_len,
                       const uint8_t *client_data_hash,
                       const ctap_resident_key_t *rk,
                       uint8_t *sig, size_t *sig_len)
{
    assert(auth_data);
    assert(client_data_hash);
    assert(rk);
    assert(sig);
    assert(sig_len);

    int ret;
    sha256_context_t ctx;
    uint8_t hash[SHA256_DIGEST_LENGTH];

    ret = fido2_ctap_crypto_sha256_init(&ctx);

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = fido2_ctap_crypto_sha256_update(&ctx, auth_data, auth_data_len);

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = fido2_ctap_crypto_sha256_update(&ctx, client_data_hash, SHA256_DIGEST_LENGTH);

    if (ret != CTAP2_OK) {
        return ret;
    }

    ret = fido2_ctap_crypto_sha256_final(&ctx, hash);

    if (ret != CTAP2_OK) {
        return ret;
    }

    return fido2_ctap_crypto_get_sig(hash, sizeof(hash), sig, sig_len,
                                     rk->priv_key,
                                     sizeof(rk->priv_key));
}
