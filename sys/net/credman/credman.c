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

#include <assert.h>
#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t _mutex = MUTEX_INIT;

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
