/*
 * Copyright (C) 2019 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit
 * @{
 *
 * @file
 * @brief       SUIT handlers for the SUIT outer wrapper
 *
 * This file contains the handlers for the content of the SUIT outer wrapper.
 * This includes the authentication wrapper and the manifest itself.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <cose/sign.h>
#include <nanocbor/nanocbor.h>

#include "hashes/sha256.h"
#include "kernel_defines.h"
#include "log.h"
#include "public_key.h"
#include "suit/conditions.h"
#include "suit/handlers.h"
#include "suit.h"

static int _auth_handler(suit_manifest_t *manifest, int key,
                         nanocbor_value_t *it)
{
    (void)key;
    cose_sign_dec_t verify;
    const uint8_t *cose_buf;
    const uint8_t *cose_container;
    size_t container_len;
    size_t cose_len = 0;
    /* It is a list of cose signatures */
    int res = nanocbor_get_bstr(it, &cose_container, &container_len);
    if (res < 0) {
        LOG_INFO("Unable to get COSE signature\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    nanocbor_value_t _cont, arr;
    nanocbor_decoder_init(&_cont, cose_container, container_len);

    int rc = nanocbor_enter_array(&_cont, &arr);
    if (rc < 0) {
        LOG_INFO("Unable to enter COSE signatures\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    uint32_t tag;
    nanocbor_get_tag(&arr, &tag);
    arr.remaining++;
    res = nanocbor_get_subcbor(&arr, &cose_buf, &cose_len);
    if (res < 0) {
        LOG_INFO("Unable to get subcbor: %d\n", res);
    }

    res = cose_sign_decode(&verify, cose_buf, cose_len);
    if (res < 0) {
        LOG_INFO("Unable to parse COSE signature\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* Iterate over signatures, should only be a single signature */
    cose_signature_dec_t signature;

    cose_sign_signature_iter_init(&signature);
    if (!cose_sign_signature_iter(&verify, &signature)) {
        LOG_INFO("Unable to get signature iteration\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* Initialize key from hardcoded public key */
    cose_key_t pkey;
    cose_key_init(&pkey);
    cose_key_set_keys(&pkey, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
                      (uint8_t *)public_key, NULL, NULL);

    LOG_INFO("suit: verifying manifest signature\n");
    int verification = cose_sign_verify(&verify, &signature,
                                        &pkey, manifest->validation_buf,
                                        SUIT_COSE_BUF_SIZE);
    if (verification != 0) {
        LOG_INFO("Unable to validate signature: %d\n", verification);
        return SUIT_ERR_SIGNATURE;
    }

    manifest->cose_payload = verify.payload;
    manifest->cose_payload_len = verify.payload_len;
    manifest->state |= SUIT_STATE_COSE_AUTHENTICATED;

    return 0;
}

static int _manifest_handler(suit_manifest_t *manifest, int key,
                             nanocbor_value_t *it)
{
    (void)key;
    const uint8_t *manifest_buf;
    size_t manifest_len;

    if (!(manifest->state & SUIT_STATE_COSE_AUTHENTICATED)) {
        return SUIT_ERR_SIGNATURE;
    }

    nanocbor_value_t cbor_buf = *it;

    nanocbor_get_subcbor(&cbor_buf, &manifest_buf, &manifest_len);

    uint8_t digest_struct[4 + SHA256_DIGEST_LENGTH] =
        /* CBOR array of length 2, sha256 digest and a bytestring of SHA256
         * length
         */
    { 0x82, 0x02, 0x58, SHA256_DIGEST_LENGTH };
    sha256(manifest_buf, manifest_len, digest_struct + 4);

    /* The COSE payload and the sha256 of the manifest itself is public info and
     * verification does not depend on secret info. No need for cryptographic
     * memcmp here */
    if (memcmp(digest_struct, manifest->cose_payload,
               sizeof(digest_struct)) != 0) {
        LOG_ERROR("SUIT manifest digest and COSE digest mismatch\n");
        return SUIT_ERR_DIGEST_MISMATCH;
    }

    manifest->state |= SUIT_STATE_FULLY_AUTHENTICATED;

    LOG_DEBUG("Starting global sequence handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it,
                                               suit_global_handlers,
                                               suit_global_handlers_len);
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_container_handlers[] = {
    [SUIT_WRAPPER_AUTHENTICATION] = _auth_handler,
    [SUIT_WRAPPER_MANIFEST]       = _manifest_handler,
};
/* end{code-style-ignore} */

const size_t suit_container_handlers_len = ARRAY_SIZE(suit_container_handlers);
