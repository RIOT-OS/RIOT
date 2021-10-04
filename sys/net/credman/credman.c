/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_credman
 * @{
 *
 * @file
 * @brief       (D)TLS Credentials management module implementation
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include "net/credman.h"
#include "mutex.h"
#include "kernel_defines.h"

#include <assert.h>
#include <string.h>
#include <inttypes.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t _mutex = MUTEX_INIT;

#if IS_USED(MODULE_CREDMAN_LOAD)
#include "tiny-asn1.h"

/* Context-specific tag in DER encoding
 * (see section 8.1.2.2 of ITU-T X.690 https://www.itu.int/rec/T-REC-X.690-200811-S) */
#define ASN1_CONTEXT_TAG(v)     (0xA0 | (v & 0x1F))

/* ASN.1 representation of ecPublicKey - OID 1.2.840.10045.2.1
 * (see https://oidref.com/1.2.840.10045.2.1) */
static const uint8_t ecPublicKey[] = { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01 };

static int _parse_ecc_point(const asn1_tree *key, const void **x, const void **y);
#endif /* MODULE_CREDMAN_LOAD */

static credman_credential_t credentials[CONFIG_CREDMAN_MAX_CREDENTIALS];
static unsigned used = 0;

static int _find_credential_pos(credman_tag_t tag, credman_type_t type,
                                credman_credential_t **empty);

int credman_add(const credman_credential_t *credential)
{
    credman_credential_t *entry = NULL;
    assert(credential);
    mutex_lock(&_mutex);
    int pos = -1;
    int ret = CREDMAN_ERROR;

    if ((credential->type == CREDMAN_TYPE_EMPTY) ||
        (credential->tag == CREDMAN_TAG_EMPTY)) {
        DEBUG("credman: invalid credential type/tag\n");
        ret = CREDMAN_INVALID;
        goto end;
    }
    switch (credential->type) {
    case CREDMAN_TYPE_PSK:
        if ((credential->params.psk.key.s == NULL) ||
            (credential->params.psk.key.len == 0)) {
            DEBUG("credman: invalid PSK parameters\n");
            ret = CREDMAN_INVALID;
            goto end;
        }
        break;
    case CREDMAN_TYPE_ECDSA:
        if ((credential->params.ecdsa.private_key == NULL) ||
            (credential->params.ecdsa.public_key.x == NULL) ||
            (credential->params.ecdsa.public_key.y == NULL)) {
            DEBUG("credman: invalid ECDSA parameters\n");
            ret = CREDMAN_INVALID;
            goto end;
        }
        break;
    default:
        ret = CREDMAN_TYPE_UNKNOWN;
        goto end;
    }

    pos = _find_credential_pos(credential->tag, credential->type, &entry);
    if (pos >= 0) {
        DEBUG("credman: credential with tag %d and type %d already exist\n",
              credential->tag, credential->type);
        ret = CREDMAN_EXIST;
    }
    else if (entry == NULL) {
        DEBUG("credman: no space for new credential\n");
        ret = CREDMAN_NO_SPACE;
    }
    else {
        *entry = *credential;
        used++;
        ret = CREDMAN_OK;
    }
end:
    mutex_unlock(&_mutex);
    return ret;
}

#if IS_USED(MODULE_CREDMAN_LOAD)
int credman_load_public_key(const void *buf, size_t buf_len, ecdsa_public_key_t *out)
{
    asn1_tree objects[CONFIG_CREDMAN_MAX_ASN1_OBJ];
    asn1_tree pub_key;

    assert(buf);
    assert(out);

    int obj_count = der_object_count((uint8_t *)buf, buf_len);
    if (obj_count <= 0) {
        DEBUG("credman: could not calculate the number of elements within the key\n");
        return CREDMAN_INVALID;
    }

    if (obj_count > CONFIG_CREDMAN_MAX_ASN1_OBJ) {
        DEBUG("credman: not enough ASN.1 objects to decode key.\n");
        DEBUG("credman: current max is %d, and we need %d\n",
              CONFIG_CREDMAN_MAX_ASN1_OBJ, obj_count);
        return CREDMAN_INVALID;
    }

    int32_t res = der_decode((uint8_t *)buf, buf_len, &pub_key, objects, obj_count);
    if (res < 0) {
        DEBUG("credman: could not parse the key (%" PRId32 ")\n", res);
        return CREDMAN_INVALID;
    }

    /*
     * From https://tools.ietf.org/html/rfc5280#section-4.1
     *
     * SubjectPublicKeyInfo  ::=  SEQUENCE  {
     *      algorithm               AlgorithmIdentifier,
     *      subjectPublicKey        BIT STRING  }
     *
     * AlgorithmIdentifier   ::=  SEQUENCE  {
     *      algorithm               OBJECT IDENTIFIER,
     *      parameters              ANY DEFINED BY algorithm OPTIONAL  }
     */

    /* the outer container is a 'SubjectPublicKeyInfo', which should be a SEQUENCE */
    if (pub_key.type != ASN1_TYPE_SEQUENCE) {
        DEBUG("credman: the public key information should be contained in an ASN.1 SEQUENCE\n");
        return CREDMAN_INVALID;
    }

    asn1_tree *algorithm_id = pub_key.child;
    asn1_tree *algorithm = algorithm_id->child;

    /* for now only ECDSA is supported by credman */
    /* the algorithm should be Elliptic Curve Public Key (OID 1.2.840.10045.2.1) */
    if (sizeof(ecPublicKey) != algorithm->length) {
        DEBUG("credman: wrong OID length for algorithm\n");
        return CREDMAN_INVALID;
    }

    if (memcmp(ecPublicKey, algorithm->data, sizeof(ecPublicKey)) != 0) {
        DEBUG("credman: wrong OID for algorithm. Expected 1.2.840.10045.2.1\n");
        return CREDMAN_INVALID;
    }

    return _parse_ecc_point(algorithm_id->next, &out->x, &out->y);
}

int credman_load_private_key(const void *buf, size_t buf_len, credman_credential_t *cred)
{

    asn1_tree objects[CONFIG_CREDMAN_MAX_ASN1_OBJ];
    asn1_tree priv_key;

    assert(buf);
    assert(cred);

    int obj_count = der_object_count((uint8_t *)buf, buf_len);
    if (obj_count <= 0) {
        DEBUG("credman: could not calculate the number of elements within the key\n");
        return CREDMAN_INVALID;
    }

    if (obj_count > CONFIG_CREDMAN_MAX_ASN1_OBJ) {
        DEBUG("credman: not enough ASN.1 objects to decode key.\n");
        DEBUG("credman: current max is %d, and we need %d\n",
              CONFIG_CREDMAN_MAX_ASN1_OBJ, obj_count);
        return CREDMAN_INVALID;
    }

    if (der_decode((uint8_t *)buf, buf_len, &priv_key, objects, obj_count) < 0) {
        DEBUG("credman: could not parse the key\n");
        return CREDMAN_INVALID;
    }

    /*
     * From https://tools.ietf.org/html/rfc5958#section-2
     *
     * OneAsymmetricKey ::= SEQUENCE {
     *      version                   Version,
     *      privateKeyAlgorithm       PrivateKeyAlgorithmIdentifier,
     *      privateKey                PrivateKey,
     *      attributes            [0] Attributes OPTIONAL,
     *      ...,
     *      [[2: publicKey        [1] PublicKey OPTIONAL ]],
     *      ...
     * }
     *
     * PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
     *                                   { PUBLIC-KEY,
     *                                     { PrivateKeyAlgorithms } }
     *
     * AlgorithmIdentifier   ::=  SEQUENCE  {
     *      algorithm               OBJECT IDENTIFIER,
     *      parameters              ANY DEFINED BY algorithm OPTIONAL  }
     *
     * PrivateKey ::= OCTET STRING
     *                   -- Content varies based on type of key.  The
     *                   -- algorithm identifier dictates the format of
     *                   -- the key.
     */

    /* the outer container is a 'OneAsymmetricKey', which should be a SEQUENCE */
    if (priv_key.type != ASN1_TYPE_SEQUENCE) {
        DEBUG("credman: the private key information should be contained in an ASN.1 SEQUENCE\n");
        return CREDMAN_INVALID;
    }

    /* point to version */
    asn1_tree *node = priv_key.child;

    if (!node || node->type != ASN1_TYPE_INTEGER) {
        DEBUG("credman: invalid private key version\n");
        return CREDMAN_INVALID;
    }

    /* point to privateKeyAlgorithm */
    node = node->next;
    if (!node || node->type != ASN1_TYPE_SEQUENCE || !node->length) {
        DEBUG("credman: invalid private key algorithm identifier\n");
        return CREDMAN_INVALID;
    }

    /* for now only ECDSA is supported by credman */
    /* the algorithm should be Elliptic Curve Public Key (OID 1.2.840.10045.2.1) */
    asn1_tree *algorithm = node->child;
    if (sizeof(ecPublicKey) != algorithm->length) {
        DEBUG("credman: wrong private key algorithm, only ecPublicKey is supported\n");
        return CREDMAN_INVALID;
    }

    if (memcmp(ecPublicKey, algorithm->data, sizeof(ecPublicKey)) != 0) {
        DEBUG("credman: wrong OID for algorithm. Expected 1.2.840.10045.2.1\n");
        return CREDMAN_INVALID;
    }

    /* point to privateKey */
    node = node->next;
    if (!node || node->type != ASN1_TYPE_OCTET_STRING || !node->data || !node->length) {
        DEBUG("credman: no private key found\n");
        return CREDMAN_INVALID;
    }

    return credman_load_private_ecc_key(node->data, node->length, cred);

}

int credman_load_private_ecc_key(const void *buf, size_t buf_len, credman_credential_t *cred)
{
    asn1_tree objects[CONFIG_CREDMAN_MAX_ASN1_OBJ];
    asn1_tree priv_key;

    assert(buf);
    assert(cred);

    int obj_count = der_object_count((uint8_t *)buf, buf_len);
    if (obj_count <= 0) {
        DEBUG("credman: could not calculate the number of elements within the key\n");
        return CREDMAN_INVALID;
    }

    if (obj_count > CONFIG_CREDMAN_MAX_ASN1_OBJ) {
        DEBUG("credman: not enough ASN.1 objects to decode key.\n");
        DEBUG("credman: current max is %d, and we need %d\n",
              CONFIG_CREDMAN_MAX_ASN1_OBJ, obj_count);
        return CREDMAN_INVALID;
    }

    if (der_decode((uint8_t *)buf, buf_len, &priv_key, objects, obj_count) < 0) {
        DEBUG("credman: could not parse the key\n");
        return CREDMAN_INVALID;
    }

    /*
     * From https://tools.ietf.org/html/rfc5915#section-3
     *
     * ECPrivateKey ::= SEQUENCE {
     *      version        INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1),
     *      privateKey     OCTET STRING,
     *      parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
     *      publicKey  [1] BIT STRING OPTIONAL
     * }
     */

    /* point to version, it SHALL be 1 */
    asn1_tree *node = priv_key.child;
    if (!node || node->type != ASN1_TYPE_INTEGER || node->data[0] != 0x01) {
        DEBUG("credman: invalid private key version\n");
        return CREDMAN_INVALID;
    }

    /* point to privateKey */
    node = node->next;
    if (!node || node->type != ASN1_TYPE_OCTET_STRING || !node->data || !node->length) {
        DEBUG("credman: invalid private key\n");
        return CREDMAN_INVALID;
    }

    cred->type = CREDMAN_TYPE_ECDSA;
    cred->params.ecdsa.public_key.x = NULL;
    cred->params.ecdsa.public_key.y = NULL;

    cred->params.ecdsa.private_key = node->data;

    /* try to find a publicKey by tag */
    while (node && node->type != ASN1_CONTEXT_TAG(1)) {
        node = node->next;
    }

    if (!node) {
        return CREDMAN_OK;
    }

    return _parse_ecc_point(node->child, &cred->params.ecdsa.public_key.x,
                            &cred->params.ecdsa.public_key.y);
}

static int _parse_ecc_point(const asn1_tree *key, const void **x, const void **y)
{
    if (!key || key->type != ASN1_TYPE_BIT_STRING) {
        DEBUG("credman: the key should be an ASN.1 BIT STRING\n");
        return CREDMAN_INVALID;
    }

    if (!key->length) {
        DEBUG("credman: the key is missing\n");
        return CREDMAN_INVALID;
    }

    /* SEC 1: Elliptic Curve Cryptography - Section 2.3.4 (https://www.secg.org/sec1-v2.pdf) */
    /* check for uncompressed key format */
    if (key->data[1] != 0x04) {
        DEBUG("credman: only uncompressed format is supported\n");
        return CREDMAN_INVALID;
    }

    size_t coords_len = (key->length - 2) / 2;
    const uint8_t *_x = &key->data[2]; /* skip format specifier and unused bits */
    const uint8_t *_y = &_x[coords_len];

    *x = _x;
    *y = _y;

    return CREDMAN_OK;
}
#endif /* MODULE_CREDMAN_LOAD */

int credman_get(credman_credential_t *credential, credman_tag_t tag,
                credman_type_t type)
{
    assert(credential);
    mutex_lock(&_mutex);
    int ret = CREDMAN_ERROR;

    int pos = _find_credential_pos(tag, type, NULL);
    if (pos < 0) {
        DEBUG("credman: credential with tag %d and type %d not found\n",
              tag, type);
        ret = CREDMAN_NOT_FOUND;
    }
    else {
        memcpy(credential, &credentials[pos], sizeof(credman_credential_t));
        ret = CREDMAN_OK;
    }
    mutex_unlock(&_mutex);
    return ret;
}

void credman_delete(credman_tag_t tag, credman_type_t type)
{
    mutex_lock(&_mutex);
    int pos = _find_credential_pos(tag, type, NULL);
    if (pos >= 0) {
        memset(&credentials[pos], 0, sizeof(credman_credential_t));
        used--;
    }
    mutex_unlock(&_mutex);
}

int credman_get_used_count(void)
{
    return used;
}

static int _find_credential_pos(credman_tag_t tag, credman_type_t type,
                                credman_credential_t **empty)
{
    for (unsigned i = 0; i < CONFIG_CREDMAN_MAX_CREDENTIALS; i++) {
        credman_credential_t *c = &credentials[i];
        if ((c->tag == tag) && (c->type == type)) {
            return i;
        }
        /* only check until empty position found */
        if ((empty) && (*empty == NULL) &&
            (c->tag == CREDMAN_TAG_EMPTY) && (c->type == CREDMAN_TYPE_EMPTY)) {
            *empty = c;
        }
    }
    return -1;
}

#ifdef TEST_SUITES
void credman_reset(void)
{
    mutex_lock(&_mutex);
    memset(credentials, 0,
           sizeof(credman_credential_t) * CONFIG_CREDMAN_MAX_CREDENTIALS);
    used = 0;
    mutex_unlock(&_mutex);
}
#endif /* TEST_SUITES */
