/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include "firmware/manifest.h"

#include "cose.h"
#include "log.h"
#include "thread.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "cn-cbor/cn-cbor.h"
#include "fmt.h"
#include "mutex.h"
#include "msg.h"
#include "net/sock/udp.h"
#include "hashes/sha256.h"

#include <string.h>
#include "keys.h"

/* Large stacksize due to crypto requirements */
#define STACKSIZE           (5*THREAD_STACKSIZE_DEFAULT)
/* Low priority to keep crypto out of the way */
#define PRIO                (THREAD_PRIORITY_MAIN + 2)
#define TNAME               "fw_upgrade"
/* initialize nanocoap server instance */
static char _stack[STACKSIZE];
static uint8_t verification_buf[MANIFEST_BUF_SIZE];

/* COSE objects */
static cose_key_t key;
static cose_sign_t cose_in;

/* Updater state */
static firmware_manifest_t _fw_man;

static ssize_t fetch_block(firmware_manifest_t *state, unsigned num, sock_udp_ep_t *local);
static bool _check_timestamp(firmware_manifest_t *state);
static bool _check_digest(firmware_manifest_t *state, uint8_t *digest);
static void *_firmware_manifest_thread(void *arg);

static void *cose_calloc(size_t count, size_t size, void *context);
static void cose_free(void *ptr, void *context);

/* CN_CBOR block allocator context struct*/
static cn_cbor_context ct =
{
    .calloc_func = cose_calloc,
    .free_func = cose_free,
    .context = &_fw_man,
};

static void *cose_calloc(size_t count, size_t size, void *context)
{
    (void)count;
    firmware_manifest_t *state = (firmware_manifest_t*)context;
    void *block = memarray_alloc(&state->storage);
    if (block) {
        memset(block, 0, size);
    }
    return block;
}

static void cose_free(void *ptr, void *context)
{
    firmware_manifest_t *state = (firmware_manifest_t*)context;
    memarray_free(&state->storage, ptr);
}

static bool _check_timestamp(firmware_manifest_t *state)
{
    firmware_metadata_t *cur = firmware_get_metadata(firmware_current_slot());
    return cur->version <= state->manifest.timestamp;
}

static bool _check_digest(firmware_manifest_t *state, uint8_t *digest)
{
    uint8_t exp[SHA256_DIGEST_LENGTH];
    size_t len = sizeof(exp);
    if (!suit_payload_get_digest(&state->manifest, SUIT_DIGEST_TYPE_RAW, exp, &len)) {
        LOG_ERROR("ota: failure getting sha256\n");
        return false;
    }
    return (memcmp(exp, digest, len) == 0);
}

/* Add manifest bytes */
int firmware_manifest_putbytes(uint8_t *buf, size_t len, size_t offset, bool more)
{
    if (!mutex_trylock(&_fw_man.lock)) {
        return -1;
    }
    if (offset == 0) {
        _fw_man.mbuf_len = 0;
    }
    if (offset + len > MANIFEST_BUF_SIZE) {
        mutex_unlock(&_fw_man.lock);
        return -1;
    }
    else {
        memcpy(&_fw_man.mbuf[offset], buf, len);
        _fw_man.mbuf_len += len;
    }
    if (!more) {
        /* Signal event */
        msg_t msg = { .type = OTA_MANIFEST_MSG_MANIFEST };
        msg.content.value = offset + len;
        msg_try_send(&msg, _fw_man.pid);
    }
    mutex_unlock(&_fw_man.lock);
    return 0;
}

static int _manifest_verify(firmware_manifest_t *state)
{
    cose_sign_init(&cose_in, 0);
    /* Decode cose */
    int res = cose_sign_decode(&cose_in, state->mbuf,
            state->mbuf_len, &ct);
    if (res != 0) {
        LOG_WARNING("Could not decode cose struct: %d\n", res);
        goto out;
    }
    /* Verify cose */
    res = cose_sign_verify(&cose_in, &key, 0, verification_buf,
            sizeof(verification_buf), &ct);
    if (res != 0) {
        LOG_WARNING("Could not verify cose struct: %d\n", res);
        goto out;
    }
    /* Parse manifest */
    res = suit_parse(&state->manifest, cose_in.payload, cose_in.payload_len);
    if (res < 0) {
        goto out;
    }
    if (!_check_timestamp(state)) {
        LOG_WARNING("Timestamp failure, ignoring update\n");
        goto out;
    }
out:
    return res;
}

static int suit_finish_update(firmware_manifest_t* state)
{
    uint8_t metadata_buf[FIRMWARE_METADATA_SIZE];
    uint8_t digest[SHA256_DIGEST_LENGTH];

    firmware_metadata_t *metadata = (firmware_metadata_t*)metadata_buf;
    memcpy(&metadata->magic_number, "RIOT", 4);
    metadata->start_addr = ((uint32_t)firmware_get_metadata(firmware_target_slot()) + FIRMWARE_METADATA_SIZE);
    sha256((uint32_t*)metadata->start_addr, state->manifest.size, digest);
    if (!_check_digest(state, digest)) {
        LOG_WARNING("firmware_manifest: Digest check failure\n");
        return -1;
    }

    metadata->version = state->manifest.timestamp;
    metadata->chksum = firmware_metadata_checksum(metadata);

    return firmware_flashwrite_finish(&state->writer, metadata, sizeof(firmware_metadata_t));
}

static ssize_t fetch_block(firmware_manifest_t *state, unsigned num, sock_udp_ep_t *local)
{
    (void)state;
    uint8_t buf[128];
    coap_pkt_t pkt;
    uint8_t *pktpos = buf;
    sock_udp_ep_t remote = {
        .family    = AF_INET6,
        .netif     = SOCK_ADDR_ANY_NETIF,
        .port      = COAP_PORT,
    };
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6,
                           "fe80::845d:8bff:fe1c:c24f");

    char path[32];
    memcpy(path, "/fw/", 4);
    fmt_u16_dec(path+4, firmware_target_slot());
    path[5] = '\0';

    pkt.hdr = (coap_hdr_t*)buf;
    pktpos += coap_build_hdr(pkt.hdr, COAP_REQ, NULL, 0, COAP_METHOD_GET, num);
    pktpos += coap_put_option_uri(pktpos, 0, path, COAP_OPT_URI_PATH);
    pktpos += coap_put_option_block2(pktpos, COAP_OPT_URI_PATH, num, 2, 0);
    pkt.payload_len = pktpos - buf;

    int res = nanocoap_request(&pkt, local, &remote, sizeof(buf) );
    if (res > 0)
    {
        coap_block1_t block2;
        coap_get_block2(&pkt, &block2);

        if (block2.offset == 0) {
            firmware_flashwrite_init(&_fw_man.writer, firmware_target_slot(), FIRMWARE_METADATA_SIZE);
            _fw_man.writer.flashpage++;
        }
        if (block2.offset + pkt.payload_len > _fw_man.manifest.size) {
            LOG_ERROR("ota: received firmware larger than expected: %u, aborting\n", block2.offset + pkt.payload_len);
            return -1;
        }
        firmware_flashwrite_putbytes(&_fw_man.writer, pkt.payload,
                pkt.payload_len, block2.more);
        if (block2.more) {
            msg_t msg = { .type = OTA_MANIFEST_MSG_NEXT };
            msg.content.value = block2.blknum;
            msg_try_send(&msg, _fw_man.pid);
        }
        else {
            if (block2.offset + pkt.payload_len == _fw_man.manifest.size) {
                msg_t msg = { .type = OTA_MANIFEST_MSG_FINAL };
                msg.content.value = block2.blknum;
                msg_try_send(&msg, _fw_man.pid);
            }
            else {
                LOG_ERROR("ota: incorrect size received, aborting\n");
            }
        }
        return block2.more;
    }
    return -1;
}

static int _get_update(void)
{
    mutex_lock(&_fw_man.lock);
    int res = _manifest_verify(&_fw_man);
    if (res < 0) {
        LOG_ERROR("Manifest verify result %i\n", res);
        return res;
    }
    int more = 1;
    unsigned blknum = 0;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = 12345;
    while (more) {
        more = fetch_block(&_fw_man, blknum, &local);
        if (more < 0) {
            LOG_ERROR("ota: error fetching block: %d\n", more);
            res = more;
            goto out;
        }
        if (more == 0) {
            break;
        }
        blknum++;
    }
    if (suit_finish_update(&_fw_man) < 0) {
        LOG_WARNING("ota: Error finalizing update\n");
    }
    else {
        LOG_INFO("ota: Succesfully updated firmware\n");
    }
out:
    mutex_unlock(&_fw_man.lock);
    return res;
}

static void *_firmware_manifest_thread(void* arg)
{
    (void)arg;
    msg_t msg, msg_queue[4];
    msg_init_queue(msg_queue, 4);
    _fw_man.pid = sched_active_pid;
    mutex_init(&_fw_man.lock);
    memarray_init(&_fw_man.storage, _fw_man.blocks, sizeof(cn_cbor),
                  OTA_MANIFEST_BLOCKS);
    while(1) {
        msg_receive(&msg);
        switch (msg.type) {
            case OTA_MANIFEST_MSG_MANIFEST:
            {
               _get_update();
               break;
            }
            default:
                break;
        }
    }
    return NULL;
}

void firmware_manifest_run(void)
{
    cose_key_init(&key);
    cose_key_set_keys(&key, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
            pubkey, NULL, NULL);
    thread_create(_stack, STACKSIZE, PRIO, THREAD_CREATE_STACKTEST,
                        _firmware_manifest_thread, NULL, TNAME);
}
