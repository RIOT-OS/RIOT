/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @internal
 * This is an informal explanation about the internal organization of peer devices that we share a cryptographic key with.
 * In the Security section of the IEEE 802.15.4 standard, there are some datatype structures defined, which are in some way
 * connected to make it possible to find the correct key for a frame that needs to be decrypted or encrypted, and to realize
 * replay protection per peer device. The datatype structures are called "descriptors". There are actually a lot of
 * them, which requires a lot of memory and lookup overhead. So if you wanted to implement the standard very complete, the
 * process becomes more complicated. Due to that reason, I made some simplifications and I hope I did understand the
 * procedures in the standard and did not violate the intended connections between the individual descriptors.
 *
 * Our implementation shall get along with the, for my understanding, most important descriptors, which are the so called
 * secKeyDescriptor, secDeviceDescriptor, and secKeyIdLookupDescriptor. The secKeyDescriptor represents the cryptographic key
 * material. The secDeviceDescriptor represents a peer device. The device descriptor must of course store the address
 * information of a device.
 * The secKeyIdLookupDescriptor stores information, necessary to find the correct key from data contained in a frame.
 * It stores a reference to a key and the frame counter how often this key has been used to encrypt a frame.
 * The key can be found implicitly from a device address in the frame header, or a frame must contain auxiliary information in
 * which case the key is identified explicitly.
 * The DeviceLookupDescriptor stores a reference to a key and device pair and tracks the incoming frame counter for replay protection.
 *
 * @verbatim
 *  +------------------+        +-----------------------------------------------------+
 *  | secKeyDescriptor +-       | secKeyIdLookupDescriptor (implicit)                 | <-- externally provisioned
 *  +------------------+ \      +-----------------------------------------------------+     device key
 *  | byte key[]       |  \     | byte key_mode = IEEE802154_SEC_SCF_KEYMODE_IMPLICIT |
 *  +------------------+   \    | byte dev_mode                                       |
 *  |                       \   | byte dev_pan_id[2]                                  |
 *  |                        \  | byte dev_addr[8]                                    |
 *  |                         --+ byte key                                            |
 *  |                           | u32 fc                                              |
 *  |                           +-----------------------------------------------------+
 *  |
 *  |   +------------------+        +-----------------------------------------------------------+
 *  |   | secKeyDescriptor +-       | secKeyIdLookupDescriptor (explicit)                       | <-- externally provisioned
 *  |   +------------------+ \      +-----------------------------------------------------------+     network key
 *  |   | byte key[]       |  \     | byte key_mode = IEEE802154_SEC_SCF_KEYMODE_INDEX or       |
 *  |   +------------------+   \    |                 IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX or |
 *  |   |                       \   |                 IEEE802154_SEC_SCF_KEYMODE_HW_INDEX       |
 *  |   |                        \  | byte key_index                                            |
 *  |   |                         \ | byte key_source[8]                                        |
 *  |    \                         -+ byte key                                                  |
 *  |     \                         | u32 fc                                                    |
 *  |      \                        +-----------------------------------------------------------+
 *  |       ---------------
 *  |                      \    +-------------------------+                  +---------------------+
 *  |                       \   | DeviceLookupDescriptor  |                  + secDeviceDescriptor | <-- externally provisioned
 *  |                        \  +-------------------------+                / +---------------------+     peer device for replay protection
 *  |                         --+ byte key                |               / || byte pan_id[2]      |
 *  |                           | byte dev                +---------------  || byte short_addr[2]  |
 *   \                          | u32 fc                  |                 || byte long_addr[8]   |
 *    \                         +-------------------------+                / +---------------------+
 *     \                                                                  /
 *      -------------------                                              /
 *                         \    +-------------------------+             /
 *                          \   | DeviceLookupDescriptor  |            /
 *                           \  +-------------------------+           /
 *                            --+ byte key                |          /
 *                              | byte dev                +----------
 *                              | u32 fc                  |
 *                              +-------------------------+
 * @endverbatim
 * Furthermore, the standard offers two ways where to store the frame counter. Depending on whether secFrameCounterPerKey is
 * true or false, you can have a global frame counter for an interface or you can store the outgoing frame counter per key.
 * As you can see from the picture above, this implementation stores a frame counter per key, because it is easier to update
 * only one link with a new key instead of updating all links with a new key, which could be error prone.
 *
 * @author Fabian Hüßler <fabian.huessler@mlpa.com>
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bitarithm.h"
#include "bitfield.h"
#include "byteorder.h"
#include "crypto/ciphers.h"
#include "crypto/modes/ecb.h"
#include "crypto/modes/cbc.h"
#include "macros/utils.h"
#include "net/ieee802154.h"
#include "net/ieee802154_security.h"
#include "string_utils.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DEBUG_SEC(s, ...)   DEBUG("[IEEE 802.15.4 SEC] function %s: " s, __func__, ## __VA_ARGS__)

#define BITMAP_FOR_EACH(set, element, bitfield, nbits, code)                    \
for (size_t i = 0; i < (nbits); i++) {                                          \
    if (bf_isset(bitfield, i)) {                                                \
        element = set[i];                                                       \
        code                                                                    \
    }                                                                           \
}
#define KEYLOOKUP_FOR_EACH(ctx, lookup, code) {                                 \
    BITMAP_FOR_EACH(&(ctx)->key_lookup_table.key_lookup,                        \
                    (lookup), (ctx)->key_lookup_table.mask,                     \
                    CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE, code)         \
}
#define PEERLOOKUP_FOR_EACH(ctx, dev, code) {                                   \
    BITMAP_FOR_EACH(&(ctx)->peer_lookup_table.peer_lookup,                      \
                    (dev), (ctx)->peer_lookup_table.mask,                       \
                    CONFIG_IEEE802154_SEC_DEFAULT_PEERLOOKUP_SIZE, code)        \
}
#define PEER_FOR_EACH(ctx, dev, code) {                                         \
    BITMAP_FOR_EACH(&(ctx)->devstore.peers,                                     \
                    (dev), (ctx)->devstore.mask,                                \
                    CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE, code)          \
}

#define CTX_LOCK(ctx)   { DEBUG_SEC("try lock context: %p\n", ctx); mutex_lock(&ctx->lock); }
#define CTX_UNLOCK(ctx) { mutex_unlock(&ctx->lock); DEBUG_SEC("unlocked context: %p\n", ctx); }

const ieee802154_radio_cipher_ops_t ieee802154_radio_cipher_ops = {
    .set_key = NULL,
    .ecb = NULL,
    .cbc = NULL
};

static void _set_key(ieee802154_sec_context_t *ctx,
                     const uint8_t *key)
{
    if (ctx->dev.cipher_ops->set_key) {
        ctx->dev.cipher_ops->set_key(&ctx->dev, key, IEEE802154_SEC_BLOCK_SIZE);
    }
    memcpy(ctx->cipher.context.context, key, IEEE802154_SEC_KEY_LENGTH);
}

/**
 * @brief   Perform an ECB block cipher for IEEE 802.15.4 security layer.
 *
 * This is the fallback implementation for the case where the security device
 * does not provide an specific implementation.
 *
 * @param[in]  dev       Security device
 * @param[out] cipher    Output cipher blocks
 * @param[in]  plain     Input plain blocks
 * @param[in]  nblocks   Number of blocks
 */
static void _sec_ecb(const ieee802154_sec_dev_t *dev,
                     uint8_t *cipher,
                     const uint8_t *plain,
                     uint8_t nblocks)
{
    cipher_encrypt_ecb(&((ieee802154_sec_context_t *)dev->ctx)->cipher,
                       plain,
                       nblocks * IEEE802154_SEC_BLOCK_SIZE,
                       cipher);
}

/**
 * @brief   Perform a CBC block cipher for IEEE 802.15.4 security layer MIC
 *          computation.
 *
 * This is the fallback implementation for the case where the security device
 * does not provide an specific implementation.
 *
 * @param[in]  dev      Security device
 * @param[out] cipher   Output cipher blocks
 * @param[in]  iv       Initial vector
 * @param[in]  plain    Input plain blocks
 * @param[in]  nblocks  Number of blocks
 */
static void _sec_cbc(const ieee802154_sec_dev_t *dev,
                     uint8_t *cipher,
                     uint8_t *iv,
                     const uint8_t *plain,
                     uint8_t nblocks)
{
    cipher_encrypt_cbc(&((ieee802154_sec_context_t *)dev->ctx)->cipher,
                       iv,
                       plain,
                       nblocks * IEEE802154_SEC_BLOCK_SIZE,
                       cipher);
}

/**
 * @brief Flag field of CCM input block
 *
 * @verbatim
 *   Bit 7    Bit6       Bit 5 - Bit 3            Bit2 - Bit 0
 * +--------+-------+-----------------------+-----------------------+
 * | 0 (r)  | Adata |          M            |          L            |
 * +--------+-------+-----------------------+-----------------------+
 * @endverbatim
 *
 * r: value reserved
 * Adata: 0 if no MIC is present, 1 else
 * M: Number of octets in authentication field      (M-2)/2
 * L: Number of octets in length field              L-1
 *
 * L will actually always be 2 because the maximul message length is 127
 * which is expressed as two bytes.
 * Valid values for M are 0 (No MIC), 4 (32 bit MIC), 8 (64 bit MIC)
 * and 16 (128 bit MIC)
 */
static inline uint8_t _ccm_flag(uint8_t M, uint8_t L)
{
    assert(M == 0 || M == 4 || M == 8 || M == 16);
    assert(L == 2);
    return (M >= 4 ? ((1 << 6) | (((M) - 2) / 2)) : 0) | ((L) - 1);
}

static inline uint8_t _get_sec_level(uint8_t scf)
{
    return (scf & IEEE802154_SEC_SCF_SECLEVEL_MASK)
           >> IEEE802154_SEC_SCF_SECLEVEL_SHIFT;
}

static inline uint8_t _get_key_id_mode(uint8_t scf)
{
    return (scf & IEEE802154_SEC_SCF_KEYMODE_MASK)
           >> IEEE802154_SEC_SCF_KEYMODE_SHIFT;
}

static inline uint8_t _mac_size(uint8_t sec_level)
{
    switch (sec_level) {
        case IEEE802154_SEC_SCF_SECLEVEL_MIC32:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC32:
            return 4;
        case IEEE802154_SEC_SCF_SECLEVEL_MIC64:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC64:
            return 8;
        case IEEE802154_SEC_SCF_SECLEVEL_MIC128:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC128:
            return 16;
        default:
            return 0;
    }
}

/* frame is secured with signature */
static inline bool _req_mac(uint8_t sec_level)
{
    switch (sec_level) {
        case IEEE802154_SEC_SCF_SECLEVEL_MIC32:
        case IEEE802154_SEC_SCF_SECLEVEL_MIC64:
        case IEEE802154_SEC_SCF_SECLEVEL_MIC128:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC32:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC64:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC128:
            return true;
        default:
            return false;
    }
}

/* frame is encrypted */
static inline bool _req_encryption(uint8_t sec_level)
{
    switch (sec_level) {
        case IEEE802154_SEC_SCF_SECLEVEL_ENC:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC32:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC64:
        case IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC128:
            return true;
        default:
            return false;
    }
}

static inline uint8_t _key_source_size(uint8_t key_mode)
{
    switch (key_mode) {
        case IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX:
            return 4;
        case IEEE802154_SEC_SCF_KEYMODE_HW_INDEX:
            return 8;
        default:
            return 0;
    }
}

static inline uint8_t _scf(uint8_t sec_level, uint8_t key_mode)
{
    return (sec_level << IEEE802154_SEC_SCF_SECLEVEL_SHIFT) |
           (key_mode << IEEE802154_SEC_SCF_KEYMODE_SHIFT);
}

static inline uint8_t _get_aux_hdr_size(uint8_t security_level,
                                        uint8_t key_mode)
{
    if (security_level == IEEE802154_SEC_SCF_SECLEVEL_NONE) {
            return 0;
    }
    switch (key_mode) {
        case IEEE802154_SEC_SCF_KEYMODE_IMPLICIT:
            return 5;
        case IEEE802154_SEC_SCF_KEYMODE_INDEX:
            return 6;
        case IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX:
            return 10;
        case IEEE802154_SEC_SCF_KEYMODE_HW_INDEX:
            return 14;
        default:
            return 0;
    }
}

static inline uint8_t _get_aux_key_index(uint8_t key_mode, const void *identifier)
{
    switch (key_mode) {
        case IEEE802154_SEC_SCF_KEYMODE_INDEX:
            return ((const ieee802154_sec_aux_key_identifier_1_t *)identifier)->key_index;
        case IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX:
            return ((const ieee802154_sec_aux_key_identifier_5_t *)identifier)->key_index;
        case IEEE802154_SEC_SCF_KEYMODE_HW_INDEX:
            return ((const ieee802154_sec_aux_key_identifier_9_t *)identifier)->key_index;
        default:
            return 0;
    }
}

static inline const void *_get_aux_key_source(uint8_t key_mode, const void *identifier)
{
    switch (key_mode) {
        case IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX:
            return ((const ieee802154_sec_aux_key_identifier_5_t *)identifier)->key_source;
        case IEEE802154_SEC_SCF_KEYMODE_HW_INDEX:
            return ((const ieee802154_sec_aux_key_identifier_9_t *)identifier)->key_source;
        default:
            return NULL;
    }
}

static uint8_t _set_aux_hdr(uint8_t sec_level,
                            uint8_t key_mode,
                            uint32_t frame_counter,
                            ieee802154_sec_aux_t *ahr,
                            ieee802154_sec_key_identifier_t *identifier)
{
    ahr->scf = _scf(sec_level, key_mode);
    /* If you look in the specification: Annex C,
       integers values are in little endian */
    ahr->fc = byteorder_htoll(frame_counter).u32;
    size_t len = 5;
    switch (key_mode) {
        case IEEE802154_SEC_SCF_KEYMODE_IMPLICIT:
            break;
        case IEEE802154_SEC_SCF_KEYMODE_INDEX:
            memcpy(ahr->key_id, &identifier->key_index, 1);
            len++;
            break;
        case IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX:
            memcpy(ahr->key_id, identifier->key_source, 4);
            memcpy(ahr->key_id + 4, &identifier->key_index, 1);
            len += 5;
            break;
        case IEEE802154_SEC_SCF_KEYMODE_HW_INDEX:
            memcpy(ahr->key_id, identifier->key_source, 8);
            memcpy(ahr->key_id + 8, &identifier->key_index, 1);
            len += 9;
            break;
        default:
            break;
    }
    return len;
}

/**
 * @brief   Construct the first block A0 for CTR
 */
static inline void _init_ctr_A0(ieee802154_sec_ccm_block_t *A0,
                                uint32_t frame_counter,
                                uint8_t security_level,
                                const uint8_t *src_address)
{
    A0->flags = _ccm_flag(0, 2);
    A0->nonce.frame_counter = htonl(frame_counter);
    A0->nonce.security_level = security_level;
    A0->counter = 0;
    memcpy(A0->nonce.src_addr, src_address, IEEE802154_LONG_ADDRESS_LEN);
}

/**
 * @brief   In CTR, the blocks Ai differ in a successive counter
 */
static inline void _advance_ctr_Ai(ieee802154_sec_ccm_block_t *Ai)
{
    Ai->counter = htons(ntohs(Ai->counter) + 1);
}

/**
 * @brief   Construct the first block B0 for CBC-MAC
 */
static inline void _init_cbc_B0(ieee802154_sec_ccm_block_t *B0,
                                uint32_t frame_counter,
                                uint8_t security_level,
                                uint16_t m_len,
                                uint8_t mic_size,
                                const uint8_t *src_address)
{
    B0->flags = _ccm_flag(mic_size, 2);
    B0->nonce.frame_counter = htonl(frame_counter);
    B0->nonce.security_level = security_level;
    B0->counter = htons(m_len);
    memcpy(B0->nonce.src_addr, src_address, IEEE802154_LONG_ADDRESS_LEN);
}

/**
 * @brief   Perform ECB on one block of data and and add padding if necessary
 */
static uint8_t _ecb(ieee802154_sec_context_t *ctx,
                    uint8_t *tmp1, uint8_t *tmp2, uint8_t *data,
                    const uint8_t *Ai, uint16_t size)
{
    uint16_t s = MIN(IEEE802154_SEC_BLOCK_SIZE, size);
    if (ctx->dev.cipher_ops->ecb) {
        ctx->dev.cipher_ops->ecb(&ctx->dev, tmp2, Ai, 1);
    }
    else {
        _sec_ecb(&ctx->dev, tmp2, Ai, 1);
    }
    memcpy(tmp1, data, s);
    memset(tmp1 + s, 0, IEEE802154_SEC_BLOCK_SIZE - s);
    memxor(tmp1, tmp2, IEEE802154_SEC_BLOCK_SIZE);
    memcpy(data, tmp1, s);
    return s;
}

/**
 * @brief   Perform CBC on one block of data and add padding if necessary
 */
static uint8_t _cbc_next(ieee802154_sec_context_t *ctx,
                         uint8_t *last, uint8_t *tmp,
                         const uint8_t *next, uint16_t size)
{
    uint16_t s = MIN(IEEE802154_SEC_BLOCK_SIZE, size);
    memcpy(tmp, next, s);
    memset(tmp + s, 0, IEEE802154_SEC_BLOCK_SIZE - s);
    if (ctx->dev.cipher_ops->cbc){
        ctx->dev.cipher_ops->cbc(&ctx->dev, last, last, tmp, 1);
    }
    else {
        _sec_cbc(&ctx->dev, last, last, tmp, 1);
    }
    return s;
}

static void _comp_mic(ieee802154_sec_context_t *ctx,
                      uint8_t mic[IEEE802154_SEC_MAX_MAC_SIZE],
                      ieee802154_sec_ccm_block_t *B0,
                      const void *a, uint16_t a_len,
                      const void *m, uint16_t m_len)
{
    uint8_t tmp[IEEE802154_SEC_BLOCK_SIZE] = { 0 };
    uint16_t off;
    memset(mic, 0, IEEE802154_SEC_MAX_MAC_SIZE);
    _cbc_next(ctx, mic, tmp, (uint8_t *)B0, sizeof(*B0));
    byteorder_htobebufs(tmp, a_len);
    off = MIN(sizeof(tmp) - sizeof(uint16_t), a_len);
    memcpy(tmp + sizeof(uint16_t), a, off);
    _cbc_next(ctx, mic, tmp, tmp, sizeof(uint16_t) + off);
    for (;off < a_len;) {
        off += _cbc_next(ctx, mic, tmp, &(((uint8_t *)a)[off]), a_len - off);
    }
    for (off = 0; off < m_len;) {
        off += _cbc_next(ctx, mic, tmp, &(((uint8_t *)m)[off]), m_len - off);
    }
}

static void _ctr(ieee802154_sec_context_t *ctx,
                 ieee802154_sec_ccm_block_t *A0,
                 const void *m, uint16_t m_len)
{
    uint8_t tmp1[IEEE802154_SEC_BLOCK_SIZE] = { 0 };
    uint8_t tmp2[IEEE802154_SEC_BLOCK_SIZE] = { 0 };

    for (uint16_t off = 0; off < m_len;) {
        _advance_ctr_Ai(A0);
        off += _ecb(ctx, tmp1, tmp2,
                    &(((uint8_t *)m)[off]), (uint8_t *)A0, m_len - off);
    }
}

static void _ctr_mic(ieee802154_sec_context_t *ctx,
                     ieee802154_sec_ccm_block_t *A0,
                     void *mic, uint8_t mic_size)
{
    uint8_t tmp1[IEEE802154_SEC_BLOCK_SIZE] = { 0 };
    uint8_t tmp2[IEEE802154_SEC_BLOCK_SIZE] = { 0 };

    _ecb(ctx, tmp1, tmp2, mic, (uint8_t *)A0, mic_size);
}

static void _init_peer(ieee802154_sec_peer_t *peer,
                       const uint8_t *pan,
                       const uint8_t *short_addr,
                       const uint8_t *long_addr)
{
    /* save pan in little endian and address reversed
       because that´s how it is ordered in the header */
    memcpy(peer->pan_id, pan, sizeof(peer->pan_id));
    if (short_addr) {
        memcpy_reversed(peer->short_addr, short_addr, sizeof(peer->short_addr));
    }
    else {
        uint8_t a[] = {0xff, 0xfe}; /* use only long address */
        memcpy_reversed(peer->short_addr, a, sizeof(peer->short_addr));
    }
    memcpy_reversed(peer->long_addr, long_addr, sizeof(peer->long_addr));
}

static void _init_key_lookup_implicit(ieee802154_sec_key_lookup_t *key_lookup,
                                      ieee802154_sec_dev_addrmode_t dev_mode,
                                      const uint8_t *dev_pan_id,
                                      const uint8_t *dev_addr)
{
    /* save pan and address reversed
       because that´s how it is ordered in the header */
    memset(key_lookup, 0, sizeof(*key_lookup));
    key_lookup->key_mode = IEEE802154_SEC_SCF_KEYMODE_IMPLICIT;
    key_lookup->key_lookup.implicit.dev_mode = dev_mode;
    if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_NONE) {
        /* We cannot deal with NONE addressing mode right now
           because we do not have a PAN coordinator implementation */
        return;
    }
    if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_SHORT) {
        memcpy(key_lookup->key_lookup.implicit.dev_pan_id, dev_pan_id,
               sizeof(key_lookup->key_lookup.implicit.dev_pan_id));
        memcpy_reversed(key_lookup->key_lookup.implicit.dev_addr, dev_addr,
                        IEEE802154_SHORT_ADDRESS_LEN);
    }
    else if(dev_mode == IEEE802154_SEC_DEV_ADDRMODE_LONG) {
        memcpy(key_lookup->key_lookup.implicit.dev_pan_id, dev_pan_id,
               sizeof(key_lookup->key_lookup.implicit.dev_pan_id));
        memcpy_reversed(key_lookup->key_lookup.implicit.dev_addr, dev_addr,
                        IEEE802154_LONG_ADDRESS_LEN);
    }
    key_lookup->key = IEEE802154_SEC_NO_IDENT;
    key_lookup->fc = 0;
}

static void _init_key_lookup_explicit(ieee802154_sec_key_lookup_t *key_lookup,
                                      ieee802154_sec_scf_keymode_t key_mode,
                                      uint8_t key_index,
                                      const void *key_source)
{
    key_lookup->key_mode = key_mode;
    key_lookup->key_lookup.explicit.key_index = key_index;
    memcpy(key_lookup->key_lookup.explicit.key_source, key_source,
           _key_source_size(key_mode));
    key_lookup->key = IEEE802154_SEC_NO_IDENT;
    key_lookup->fc = 0;
}

/* get key lookup descriptor vgl. 9.2.2 KeyDescriptor lookup procedure */
static ieee802154_sec_key_lookup_t *_get_key_lookup(ieee802154_sec_context_t *ctx,
                                                    const uint8_t *pan,
                                                    const uint8_t *addr,
                                                    uint8_t addr_len,
                                                    uint8_t key_mode,
                                                    ieee802154_sec_key_identifier_t *identifier)
{
    ieee802154_sec_key_lookup_t *l, *out = NULL;
    if (key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
        if (!pan || !addr || !addr_len) {
            /* PAN coordinator not implemented */
            return NULL;
        }
        ieee802154_sec_dev_addrmode_t dev_mode = IEEE802154_SEC_DEV_ADDRMODE_NONE;
        if (addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
            dev_mode = IEEE802154_SEC_DEV_ADDRMODE_SHORT;
        }
        else if (addr_len == IEEE802154_LONG_ADDRESS_LEN) {
            dev_mode = IEEE802154_SEC_DEV_ADDRMODE_LONG;
        }
        KEYLOOKUP_FOR_EACH(ctx, l, {
            if (l->key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
                if (l->key_lookup.implicit.dev_mode == dev_mode &&
                    !memcmp(pan, l->key_lookup.implicit.dev_pan_id,
                            sizeof(l->key_lookup.implicit.dev_pan_id)) &&
                    !memcmp(addr, l->key_lookup.implicit.dev_addr, addr_len)) {
                    out = l;
                    break;
                }
            }
        });
    }
    else {
        assert(key_mode == IEEE802154_SEC_SCF_KEYMODE_INDEX ||
               key_mode == IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX ||
               key_mode == IEEE802154_SEC_SCF_KEYMODE_HW_INDEX);
        assert(identifier);
        uint8_t source_size = _key_source_size(key_mode);
        KEYLOOKUP_FOR_EACH(ctx, l, {
            if (l->key_mode == key_mode &&
                (l->key_lookup.explicit.key_index == identifier->key_index &&
                !memcmp(l->key_lookup.explicit.key_source, identifier->key_source, source_size))) {
                out = l;
                break;
            }
        });
    }
    return out;
}

/* get device descriptor vgl. 9.2.5 DeviceDescriptor lookup procedure */
static ieee802154_sec_peer_t *_get_peer(ieee802154_sec_context_t *ctx,
                                        const uint8_t *dev_pan_id,
                                        const uint8_t *dev_addr,
                                        uint8_t addr_len)
{
    ieee802154_sec_peer_t *d, *out = NULL;
    if (!addr_len || !dev_pan_id || !dev_addr) {
        /* PAN coordinator not implemented */
        return NULL;
    }
    if (addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        PEER_FOR_EACH(ctx, d, {
            if (!memcmp(dev_pan_id, d->pan_id, sizeof(d->pan_id)) &&
                !memcmp(dev_addr, d->short_addr, sizeof(d->short_addr))) {
                out = d;
                break;
            }
        });
    }
    else if (addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        PEER_FOR_EACH(ctx, d, {
            if (!memcmp(dev_pan_id, d->pan_id, sizeof(d->pan_id)) &&
                !memcmp(dev_addr, d->long_addr, sizeof(d->long_addr))) {
                out = d;
                break;
            }
        });
    }
    return out;
}

#if IS_USED(MODULE_IEEE802154_SECURITY_REPLAY_PROTECTION)
static void _init_peer_lookup(ieee802154_sec_peer_lookup_t *peer_lookup,
                              ieee802154_sec_peer_descriptor_t peer,
                              ieee802154_sec_key_descriptor_t key)
{
    peer_lookup->peer = peer;
    peer_lookup->key = key;
    peer_lookup->fc = 0;
}

static ieee802154_sec_peer_lookup_t *_get_peer_lookup(ieee802154_sec_context_t *ctx,
                                                      ieee802154_sec_peer_descriptor_t peer,
                                                      ieee802154_sec_key_descriptor_t key)
{
    ieee802154_sec_peer_lookup_t *l, *out = NULL;
    PEERLOOKUP_FOR_EACH(ctx, l, {
        if (l->peer == peer && (l->key == key || key == IEEE802154_SEC_NO_IDENT)) {
            out = l;
            break;
        }
    });
    return out;
}

/* this is internal because peers are added dynamically and not from outside */
int _peer_lookup(ieee802154_sec_context_t *ctx,
                 ieee802154_sec_peer_descriptor_t d,
                 ieee802154_sec_key_descriptor_t k, bool add)
{
    /* check_for duplicate */
    ieee802154_sec_peer_lookup_t tmp_lookup;
    _init_peer_lookup(&tmp_lookup, d, k);
    ieee802154_sec_peer_lookup_t *tmp = _get_peer_lookup(ctx, d, k);
    if (tmp) {
        if (!add) {
            /* remove */
            bf_unset(ctx->peer_lookup_table.mask, tmp - ctx->peer_lookup_table.peer_lookup);
            memset(tmp, 0, sizeof(*tmp));
        }
        return IEEE802154_SEC_OK;
    }
    /* allocate a lookup descriptor */
    int l = bf_get_unset(ctx->peer_lookup_table.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_PEERLOOKUP_SIZE);
    if (l < 0) {
        return IEEE802154_SEC_NO_DEV;
    }
    ctx->peer_lookup_table.peer_lookup[l] = tmp_lookup;
    return IEEE802154_SEC_OK;
}
#endif

static ieee802154_sec_key_descriptor_t _add_key(ieee802154_sec_context_t *ctx,
                                                const uint8_t *key)
{
    ieee802154_sec_key_t tmp_key;
    memset(&tmp_key, 0, sizeof(tmp_key));
    memcpy(&tmp_key.key, key, sizeof(tmp_key.key));
    /* do not check for duplicate key material */
    int k = bf_get_unset(ctx->keystore.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE);
    if (k < 0) {
        return IEEE802154_SEC_NO_IDENT;
    }
    ctx->keystore.keys[k] = tmp_key;
    return k;
}

static void _remove_key(ieee802154_sec_context_t *ctx, ieee802154_sec_key_descriptor_t k)
{
    assert(k < CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE);
    bf_unset(ctx->keystore.mask, k);
    memset(&ctx->keystore.keys[k], 0, sizeof(ctx->keystore.keys[k]));
}

void ieee802154_sec_init(ieee802154_sec_context_t *ctx,
                         ieee802154_sec_scf_seclevel_t sec_level,
                         ieee802154_sec_scf_keymode_t sec_key_mode,
                         uint8_t sec_key_index,
                         const void *sec_key_source)
{
    /* device driver can override this */
    ctx->dev.cipher_ops = &ieee802154_radio_cipher_ops;
    /* device driver can override this */
    ctx->dev.ctx = ctx;
    ctx->security_level = sec_level;
    ctx->key_id_mode = sec_key_mode;
    ctx->key_index = sec_key_index;
    memcpy(ctx->key_source, sec_key_source, _key_source_size(sec_key_mode));
    uint8_t key[] = CONFIG_IEEE802154_SEC_DEFAULT_KEY;
    assert(sizeof(key) >= IEEE802154_SEC_KEY_LENGTH);
    assert(CIPHER_MAX_CONTEXT_SIZE >= IEEE802154_SEC_KEY_LENGTH);
    cipher_init(&ctx->cipher, CIPHER_AES, key, IEEE802154_SEC_KEY_LENGTH);
    mutex_init(&ctx->lock);

    if (0 /* TODO: initialize from persistent storage */) {
    }
    else {
        /* no persistent storage or failure */
        CTX_LOCK(ctx);
        memset(&ctx->key_lookup_table, 0, sizeof(ctx->key_lookup_table));
        memset(&ctx->keystore, 0, sizeof(ctx->keystore));
#if IS_USED(MODULE_IEEE802154_SECURITY_REPLAY_PROTECTION)
        memset(&ctx->peer_lookup_table, 0, sizeof(ctx->peer_lookup_table));
#endif
        memset(&ctx->devstore, 0, sizeof(ctx->devstore));
        CTX_UNLOCK(ctx);
        /* add an explicit default key */
        /* implicit key cannot be added without a known peer address */
        /* default key mode and security level are independent of that */
        if (sec_key_index > 0) {
            ieee802154_sec_key_lookup_explicit(ctx, IEEE802154_SEC_SCF_KEYMODE_INDEX,
                                               sec_key_index, ctx->key_source, key, true);
        }
    }
}

int ieee802154_sec_encrypt_frame(ieee802154_sec_context_t *ctx,
                                 const uint8_t *header, uint8_t *header_size,
                                 uint8_t *payload, uint16_t payload_size,
                                 uint8_t *mic, uint8_t *mic_size,
                                 const uint8_t *src_address)
{
    /* For non data frames (MAC commands, beacons) a and a_len would be larger.
       ACKs are not encrypted. */
    assert((*((uint8_t *)header)) & IEEE802154_FCF_TYPE_DATA);

    uint32_t fc;
    ieee802154_sec_scf_keymode_t key_mode;
    ieee802154_sec_scf_seclevel_t sec_level;
    ieee802154_sec_key_identifier_t identifier;
    {
        CTX_LOCK(ctx);
        key_mode = ctx->key_id_mode;
        sec_level = ctx->security_level;
        if (sec_level == IEEE802154_SEC_SCF_SECLEVEL_NONE) {
            *mic_size = 0;
            CTX_UNLOCK(ctx);
            return IEEE802154_SEC_OK;
        }
        identifier.key_index = ctx->key_index;
        memcpy(identifier.key_source, ctx->key_source,
               _key_source_size(ctx->key_id_mode));
        /* attempt to find the encryption key */
        ieee802154_sec_key_lookup_t *key = _get_key_lookup(
            ctx,
            ieee802154_get_dst_pan_ptr(header),
            ieee802154_get_dst_ptr(header),
            ieee802154_get_dst_len(header),
            key_mode,
            &identifier);
        if (!key) {
            CTX_UNLOCK(ctx);
            DEBUG_SEC("key not found\n");
            return -IEEE802154_SEC_NO_KEY;
        }
        ieee802154_sec_key_t *k = &ctx->keystore.keys[key->key];
        fc = key->fc;
        if (fc == 0xFFFFFFFF) {
            /* Letting the frame counter overflow is explicitly prohibited by the specification.
            (see 9.4.2) */
            CTX_UNLOCK(ctx);
            DEBUG_SEC("Frame counter would overflow\n");
            return -IEEE802154_SEC_FRAME_COUNTER_OVERFLOW;
        }
        key->fc++;
        key_mode = key->key_mode;
        _set_key(ctx, k->key);
        CTX_UNLOCK(ctx);
    }
    /* write the auxiliary header */
    ieee802154_sec_aux_t *aux = (ieee802154_sec_aux_t *)(header + *header_size);
    uint8_t aux_size = _get_aux_hdr_size(sec_level, key_mode);
    _set_aux_hdr(sec_level, key_mode, fc, aux, &identifier);

    *mic_size = _mac_size(sec_level);
    const uint8_t *a = header;
    uint8_t *m = payload;
    uint16_t a_len = *header_size + aux_size;
    uint16_t m_len = payload_size;
    ieee802154_sec_ccm_block_t ccm; /* Ai or Bi */

    /* compute MIC */
    if (_req_mac(sec_level)) {
        _init_cbc_B0(&ccm, fc, sec_level, m_len, *mic_size, src_address);
        _comp_mic(ctx, mic, &ccm, a, a_len, m, m_len);

        /* encrypt MIC */
        _init_ctr_A0(&ccm, fc, sec_level, src_address);
        _ctr_mic(ctx, &ccm, mic, *mic_size);
    }
    /* encrypt payload */
    if (_req_encryption(sec_level)) {
        _init_ctr_A0(&ccm, fc, sec_level, src_address);
        _ctr(ctx, &ccm, m, m_len);
    }
    *header_size += aux_size;
    return IEEE802154_SEC_OK;
}

int ieee802154_sec_decrypt_frame(ieee802154_sec_context_t *ctx,
                                 uint16_t frame_size,
                                 uint8_t *header, uint8_t *header_size,
                                 uint8_t **payload, uint16_t *payload_size,
                                 uint8_t **mic, uint8_t *mic_size)
{
    /* For non data frames (MAC commands, beacons) a and a_len would be larger.
       ACKs are not encrypted. */
    assert(*header & IEEE802154_FCF_TYPE_DATA);

    /* read the fields of the auxiliary header */
    ieee802154_sec_aux_t *aux = (ieee802154_sec_aux_t *)(header + *header_size);
    uint8_t security_level = _get_sec_level(aux->scf);
    uint8_t key_mode = _get_key_id_mode(aux->scf);
    uint8_t aux_size = _get_aux_hdr_size(security_level, key_mode);
    uint8_t mac_size = _mac_size(security_level);
    /* remember that the frame counter was stored in little endian */
    uint32_t frame_counter = byteorder_ltohl((le_uint32_t){aux->fc});
    uint8_t src_address[IEEE802154_LONG_ADDRESS_LEN];

    if (security_level == IEEE802154_SEC_SCF_SECLEVEL_NONE) {
        *payload = header + *header_size;
        *payload_size = frame_size - *header_size;
        *mic = NULL;
        *mic_size = 0;
        return IEEE802154_SEC_OK;
    }

    *payload_size = frame_size - *header_size - aux_size - mac_size;
    *payload = header + *header_size + aux_size;
    *mic_size = mac_size;
    *mic = header + frame_size - mac_size;
    {
        ieee802154_sec_key_identifier_t identifier;
        if (key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
            identifier.key_index = _get_aux_key_index(key_mode, aux->key_id);
            memcpy(identifier.key_source,
                   _get_aux_key_source(key_mode, aux->key_id),
                   _key_source_size(key_mode));
        }
        CTX_LOCK(ctx);
        /* attempt to find key descriptor */
        ieee802154_sec_key_lookup_t *key = _get_key_lookup(ctx,
            ieee802154_get_src_pan_ptr(header),
            ieee802154_get_src_ptr(header),
            ieee802154_get_src_len(header),
            key_mode,
            &identifier);
        if (!key) {
            CTX_UNLOCK(ctx);
            DEBUG_SEC("key not found\n");
            return -IEEE802154_SEC_NO_KEY;
        }
        ieee802154_sec_peer_t *dev = _get_peer(ctx,
                ieee802154_get_src_pan_ptr(header),
                ieee802154_get_src_ptr(header),
                ieee802154_get_src_len(header));
#if IS_USED(MODULE_IEEE802154_SECURITY_REPLAY_PROTECTION)
        if (!dev) {
            /* The device has to exist and must be known by long address. */
            /* If it would not have to exist, an attacker could replay any frame by creating an arbitrary device. */
            /* If it exists, it is trusted and a lookup descriptor is created for tracking key usage and frame counter. */
            CTX_UNLOCK(ctx);
            DEBUG_SEC("device not found\n");
            return -IEEE802154_SEC_NO_DEV;
        }
        /* get or create peer lookup descriptor to track key usage and frame counter */
        ieee802154_sec_peer_lookup_t *dev_lookup =  _get_peer_lookup(
            ctx,
            dev - ctx->devstore.peers,
            key->key);
        if (!dev_lookup) {
            if (_peer_lookup(ctx, dev - ctx->devstore.peers, key->key, true) != IEEE802154_SEC_OK) {
                CTX_UNLOCK(ctx);
                DEBUG_SEC("peer lookup table full\n");
                return -IEEE802154_SEC_NO_DEV;
            }
            dev_lookup = _get_peer_lookup(
                ctx,
                dev - ctx->devstore.peers,
                key->key);
            assert(dev_lookup); /* must exist as it was just created */
        }
        /* check frame counter to fend replay attacks */
        if (frame_counter < dev_lookup->fc) {
            CTX_UNLOCK(ctx);
            DEBUG_SEC("Frame counter indicates replay\n");
            return -IEEE802154_SEC_FRAME_COUNTER_REPLAY;
        }
        /* store the next minimum expected frame counter */
        dev_lookup->fc = frame_counter + 1;
        memcpy_reversed(src_address, dev->long_addr, sizeof(dev->long_addr));
#else
        if (dev) {
            memcpy_reversed(src_address, dev->long_addr, sizeof(dev->long_addr));
        }
        else if (ieee802154_get_src_len(header) == IEEE802154_LONG_ADDRESS_LEN) {
            memcpy_reversed(src_address, ieee802154_get_src_ptr(header), IEEE802154_LONG_ADDRESS_LEN);
        }
        else {
            CTX_UNLOCK(ctx);
            DEBUG_SEC("No long source address\n");
            return -IEEE802154_SEC_NO_DEV;
        }
#endif
        _set_key(ctx, ctx->keystore.keys[key->key].key);
        CTX_UNLOCK(ctx);
    }
    const uint8_t *a = header;
    uint8_t *c = *payload;
    uint16_t a_len = *header_size + aux_size;
    uint16_t c_len = *payload_size;
    uint8_t *mac = *mic;
    ieee802154_sec_ccm_block_t ccm; /* Ai or Bi */

    /* decrypt MIC */
    if (mac_size) {
        _init_ctr_A0(&ccm, frame_counter, security_level, src_address);
        _ctr_mic(ctx, &ccm, mac, mac_size);
    }
    /* decrypt cipher */
    if (_req_encryption(security_level)) {
        _init_ctr_A0(&ccm, frame_counter, security_level, src_address);
        _ctr(ctx, &ccm, c, c_len);
    }
    /* check MIC */
    if (_req_mac(security_level)) {
        uint8_t tmp_mic[IEEE802154_SEC_MAX_MAC_SIZE];
        _init_cbc_B0(&ccm, frame_counter, security_level, c_len, mac_size, src_address);
        _comp_mic(ctx, tmp_mic, &ccm, a, a_len, c, c_len);
        if (memcmp(tmp_mic, *mic, mac_size)) {
            DEBUG_SEC("invalid MIC\n");
            return -IEEE802154_SEC_MAC_CHECK_FAILURE;
        }
    }
    *header_size += aux_size;
    return IEEE802154_SEC_OK;
}

int ieee802154_sec_key_lookup_implicit(ieee802154_sec_context_t *ctx,
                                       ieee802154_sec_dev_addrmode_t dev_mode,
                                       uint16_t dev_pan_id,
                                       const uint8_t *dev_addr,
                                       const uint8_t *key_mat,
                                       bool add)
{
    /* Check if the key exists */
    uint8_t addr_len = 0;
    if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_SHORT) {
        addr_len = IEEE802154_SHORT_ADDRESS_LEN;
    }
    else if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_LONG) {
        addr_len = IEEE802154_LONG_ADDRESS_LEN;
    }
    else if (dev_mode != IEEE802154_SEC_DEV_ADDRMODE_NONE) {
        return IEEE802154_SEC_NO_IDENT;
    }
    ieee802154_sec_key_lookup_t tmp_lookup;
    dev_pan_id = byteorder_htols(dev_pan_id).u16; /* save in little endian */
    _init_key_lookup_implicit(&tmp_lookup, dev_mode, (uint8_t *)&dev_pan_id, dev_addr);
    CTX_LOCK(ctx);
    /* check for duplicate */
    ieee802154_sec_key_lookup_t *tmp =  _get_key_lookup(
        ctx,
        tmp_lookup.key_lookup.implicit.dev_pan_id,
        tmp_lookup.key_lookup.implicit.dev_addr,
        addr_len,
        tmp_lookup.key_mode,
        NULL);
    if (tmp) {
        if (!add) {
            /* remove */
            _remove_key(ctx, tmp->key);
            bf_unset(ctx->key_lookup_table.mask, tmp - ctx->key_lookup_table.key_lookup);
            memset(tmp, 0, sizeof(*tmp));
        }
        CTX_UNLOCK(ctx);
        return IEEE802154_SEC_OK;
    }
    /* allocate a key */
    ieee802154_sec_key_descriptor_t k = _add_key(ctx, key_mat);
    if (k == IEEE802154_SEC_NO_IDENT) {
        CTX_UNLOCK(ctx);
        DEBUG_SEC("keystore full\n");
        return -IEEE802154_SEC_NO_KEY;
    }
    tmp_lookup.key = k;
    /* allocate a lookup descriptor */
    int l = bf_get_unset(ctx->key_lookup_table.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);
    if (l < 0) {
        _remove_key(ctx, k);
        CTX_UNLOCK(ctx);
        DEBUG_SEC("key lookup table full\n");
        return -IEEE802154_SEC_NO_KEY;
    }
    ctx->key_lookup_table.key_lookup[l] = tmp_lookup;
    CTX_UNLOCK(ctx);
    return IEEE802154_SEC_OK;
}

int ieee802154_sec_key_lookup_explicit(ieee802154_sec_context_t *ctx,
                                       ieee802154_sec_scf_keymode_t key_mode,
                                       uint8_t key_index,
                                       const void *key_source,
                                       const uint8_t *key_mat,
                                       bool add)
{
    if (key_mode != IEEE802154_SEC_SCF_KEYMODE_INDEX &&
        key_mode != IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX &&
        key_mode != IEEE802154_SEC_SCF_KEYMODE_HW_INDEX) {
        return -IEEE802154_SEC_UNSUPPORTED;
    }
    if (key_index == 0x00) {
        return -IEEE802154_SEC_UNSUPPORTED; /* not allowed */
    }
    ieee802154_sec_key_lookup_t tmp_lookup;
    _init_key_lookup_explicit(&tmp_lookup, key_mode, key_index, key_source);
    ieee802154_sec_key_identifier_t identifier;
    identifier.key_index = tmp_lookup.key_lookup.explicit.key_index;
    memcpy(identifier.key_source, tmp_lookup.key_lookup.explicit.key_source,
           _key_source_size(tmp_lookup.key_mode));
    CTX_LOCK(ctx);
    /* check for duplicate */
    ieee802154_sec_key_lookup_t *tmp = _get_key_lookup(
        ctx,
        0, NULL, 0,
        tmp_lookup.key_mode,
        &identifier);
    if (tmp) {
        if (!add) {
            /* remove */
            _remove_key(ctx, tmp->key);
            bf_unset(ctx->key_lookup_table.mask, tmp - ctx->key_lookup_table.key_lookup);
            memset(tmp, 0, sizeof(*tmp));
        }
        CTX_UNLOCK(ctx);
        return IEEE802154_SEC_OK;
    }
    /* allocate a key */
    ieee802154_sec_key_descriptor_t k = _add_key(ctx, key_mat);
    if (k == IEEE802154_SEC_NO_IDENT) {
        CTX_UNLOCK(ctx);
        DEBUG_SEC("keystore full\n");
        return -IEEE802154_SEC_NO_KEY;
    }
    tmp_lookup.key = k;
    /* allocate a lookup descriptor */
    int l = bf_get_unset(ctx->key_lookup_table.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);
    if (l < 0) {
        _remove_key(ctx, k);
        CTX_UNLOCK(ctx);
        DEBUG_SEC("key lookup table full\n");
        return -IEEE802154_SEC_NO_KEY;
    }
    ctx->key_lookup_table.key_lookup[l] = tmp_lookup;
    /* device bit in key descriptor should be set during pairings phase,
       see ieee802154_sec_peer_dev() */
    CTX_UNLOCK(ctx);
    return IEEE802154_SEC_OK;
}

static void _remove_peer(ieee802154_sec_context_t *ctx, ieee802154_sec_peer_descriptor_t d)
{
    assert(d < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE);
    bf_unset(ctx->devstore.mask, d);
    memset(&ctx->devstore.peers[d], 0, sizeof(ctx->devstore.peers[d]));
}

int ieee802154_sec_peer(ieee802154_sec_context_t *ctx,
                         uint16_t pan_id,
                         const uint8_t *short_addr,
                         const uint8_t *long_addr,
                         bool add)
{
    assert(long_addr); /* must know the long address */
    CTX_LOCK(ctx);
    pan_id = byteorder_htols(pan_id).u16; /* save in little endian */
    ieee802154_sec_peer_t tmp_dev;
    _init_peer(&tmp_dev, (uint8_t *)&pan_id, short_addr, long_addr);
    /* check for duplicate */
    ieee802154_sec_peer_t *tmp = _get_peer(ctx, tmp_dev.pan_id,
                                           tmp_dev.long_addr, IEEE802154_LONG_ADDRESS_LEN);
    if (tmp) {
        if (!add) {
#if IS_USED(MODULE_IEEE802154_SECURITY_REPLAY_PROTECTION)
            while (_get_peer_lookup(ctx, tmp - ctx->devstore.peers, IEEE802154_SEC_NO_IDENT)) {
                _peer_lookup(ctx, tmp - ctx->devstore.peers, IEEE802154_SEC_NO_IDENT, false);
            };
#endif
            _remove_peer(ctx, tmp - ctx->devstore.peers);
        }
        *tmp = tmp_dev; /* update pan and short address */
        CTX_UNLOCK(ctx);
        return IEEE802154_SEC_OK;
    }
    int d = bf_get_unset(ctx->devstore.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE);
    if (d < 0) {
        CTX_UNLOCK(ctx);
        DEBUG_SEC("device store full\n");
        return -IEEE802154_SEC_NO_DEV;
    }
    ctx->devstore.peers[d] = tmp_dev;
    CTX_UNLOCK(ctx);
    return IEEE802154_SEC_OK;
}

int ieee802154_sec_update(ieee802154_sec_context_t *ctx,
                          ieee802154_sec_scf_seclevel_t sec_level,
                          ieee802154_sec_scf_keymode_t sec_key_mode,
                          uint8_t sec_key_index,
                          const void *sec_key_source,
                          const uint8_t *new)
{
    if (sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT &&
        sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_INDEX &&
        sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX &&
        sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_HW_INDEX) {
        DEBUG_SEC("unsupported key mode\n");
        return -IEEE802154_SEC_UNSUPPORTED;
    }
    if (sec_level != IEEE802154_SEC_SCF_SECLEVEL_NONE &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_MIC32 &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_MIC64 &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_MIC128 &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_ENC &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC32 &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC64 &&
        sec_level != IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC128) {
        DEBUG_SEC("unsupported security level\n");
        return -IEEE802154_SEC_UNSUPPORTED;
    }
    if (sec_key_index == 0 && sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
        DEBUG_SEC("unsupported key index\n");
        return -IEEE802154_SEC_UNSUPPORTED; /* not allowed */
    }
    if (sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT &&
        sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_INDEX &&
        !sec_key_source) {
        DEBUG_SEC("unsupported key source\n");
        return -IEEE802154_SEC_UNSUPPORTED; /* key source must be given for explicit keys */
    }
    if (sec_level != IEEE802154_SEC_SCF_SECLEVEL_NONE && !new) {
        DEBUG_SEC("missing key material\n");
        return -IEEE802154_SEC_UNSUPPORTED; /* new key material must be given if security level is not NONE */
    }
    ieee802154_sec_key_lookup_explicit(ctx, ctx->key_id_mode,
                                       ctx->key_index, ctx->key_source,
                                       NULL, false);
    if (sec_key_index) {
        if (ieee802154_sec_key_lookup_explicit(ctx, sec_key_mode,
                                               sec_key_index, sec_key_source,
                                               new, true) != IEEE802154_SEC_OK) {
            DEBUG_SEC("failed to update key\n");
            return -IEEE802154_SEC_NO_KEY;
        }

    }
    CTX_LOCK(ctx);
    if (sec_key_index) {
        ctx->key_index = sec_key_index;
        memcpy(ctx->key_source, sec_key_source, _key_source_size(sec_key_mode));
    }
    /* update on success */
    ctx->security_level = sec_level;
    ctx->key_id_mode = sec_key_mode;
    CTX_UNLOCK(ctx);
    return IEEE802154_SEC_OK;
}
