/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @internal
 * This is an informal explanation about the internal organization of peer devices that we share a cryptographic key with.
 * In the Security section of the IEEE 802.15.4 standard, there are some datatype structures defined, which are in some way
 * connected to make it possible to find the correct key for a frame that needs to be decrypted or encrypted, and to realize
 * replay protection per peer device. The datatype structures are basically called "descriptors". There are actually a lot of
 * them, which requires a lot of memory and lookup overhead. So if you wanted to implement the standard very complete, the
 * process becomes complicated and error prone. Due to that reason, I made some simplifications and I hope I did undersand the
 * procedures in the standard and did not violate the intended connections between the individual descriptors.
 *
 * Our implementation shall get along with the, for my understanding, most important descriptors, which are the so called
 * secKeyDescriptor, secDeviceDescriptor, and secKeyIdLookupDescriptor. The secKeyDescriptor represents the cryptographic key
 * material and stores how often this key has been used to secure an outgoing frame. One key may have references to multiple
 * peer devices. The secDeviceDescriptor represents such a peer device. The device descriptor must of course store the address
 * information of a device, and additionally the minimum expected frame counter of a frame, decrypted with the key of that
 * device. The secKeyIdLookupDescriptor stores information, necessary to find the correct key from data contained in a frame.
 * The key can be found implicitly from a device address in the frmae header, or a frame must contain auxiliary information in
 * which case the key is identified explicitly. I understand secKeyIdLookupDescriptors as a thing which tells how some devices
 * and keys are related. Have a look at the following figure, illustration the internal relations.
 * @verbatim
 *  +------------------+        +-----------------------------------------------------+         +---------------------+
 *  | secKeyDescriptor +-       | secKeyIdLookupDescriptor (implicit)                 |      +--+ secDeviceDescriptor |
 *  +------------------+ \      +-----------------------------------------------------+     /  /+---------------------+
 *  | byte key[]       |  \     | byte key_mode = IEEE802154_SEC_SCF_KEYMODE_IMPLICIT |    /  / | byte pan_id[2]      |
 *  | u32 fc           |   \    | byte dev_mode                                       +---+  /  | byte short_addr[2]  |
 *  | bmap dev_fc_list |    \   | byte dev_pan_id[2]                                  +--+  /   | byte long_addr[8]   |
 *  +--------------001-+     \  | byte dev_addr[8]                                    +-+  /    +---------------------+
 *                   |        --+ byte key                                            |   /
 *                   |          +-----------------------------------------------------+  /
 *                   |                                                                  /
 *                   +-----------------------------------------------------------------+
 *
 *  +------------------+        +-----------------------------------------------------------+          +---------------------+
 *  | secKeyDescriptor +-       | secKeyIdLookupDescriptor (explicit)                       |          + secDeviceDescriptor |
 *  +------------------+ \      +-----------------------------------------------------------+          +---------------------+
 *  | byte key[]       |  \     | byte key_mode = IEEE802154_SEC_SCF_KEYMODE_INDEX or       |          | byte pan_id[2]      |
 *  | u32 fc           |   \    |                 IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX or +          | byte short_addr[2]  |
 *  | bmap dev_fc_list |    \   |                 IEEE802154_SEC_SCF_KEYMODE_HW_INDEX       +          | byte long_addr[8]   |
 *  +--------------110-+     \  | byte key_index                                            |          | u32 fc              |
 *                 ||         \ | byte key_source[8]                                        |          +---------------------+
 *                 ||          -+ byte key                                                  |
 *                 ||           +-----------------------------------------------------------+
 *                 ||
 *                 |+--------------------------------------------------------------------------------- +---------------------+
 *                 +--------------------------------------------------------------------------------+--+ secDeviceDescriptor |
 *                                                                                                     +---------------------+
 *                                                                                                     | byte pan_id[2]      |
 *                                                                                                     | byte short_addr[2]  |
 *                                                                                                     | byte long_addr[8]   |
 *                                                                                                     | u32 fc              |
 *                                                                                                     +---------------------+
 * @endverbatim
 * Furthermore, the standard offers two ways where to store the frame counter. Depending on whether secFrameCounterPerKey is
 * true or false, you can have a global frame counter for an interface or you can store the outgoing frame counter in the
 * secKeyDescriptor. As you can see from the picture above, this implementation does not distinguish between two kinds.
 * We store the outgoing frame counter in the key descriptor, because if we stored it globally, we would have to exchange keys
 * with all devices at the same time, which appears to be a complicated and error prone task.
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
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
#include "net/ieee802154_security.h"

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
#define DEVLOOKUP_FOR_EACH(ctx, dev, code) {                                    \
    BITMAP_FOR_EACH(&(ctx)->dev_lookup_table.dev_lookup,                        \
                    (dev), (ctx)->dev_lookup_table.mask,                        \
                    CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE, code)          \
}

#define LOCK(ctx)   { DEBUG_SEC("try lock context: %p\n", ctx); mutex_lock(&ctx->lock); }
#define UNLOCK(ctx) { mutex_unlock(&ctx->lock); DEBUG_SEC("unlocked context: %p\n", ctx); }

const ieee802154_radio_cipher_ops_t ieee802154_radio_cipher_ops = {
    .set_key = NULL,
    .ecb = NULL,
    .cbc = NULL
};

static inline uint16_t _min(uint16_t a, uint16_t b)
{
    return a < b ? a : b;
}

static inline const uint8_t * _get_src_pan(const uint8_t *hdr)
{
    uint16_t fcf = (((uint16_t)hdr[0]) << 8) | hdr[1];
    if (((fcf & 0x00ff) & IEEE802154_FCF_SRC_ADDR_MASK) == IEEE802154_FCF_SRC_ADDR_VOID) {
        /* SRC PAN and SRC address are not present,
           so the caller must use MAC PAN ID */
        return NULL;
    }
    uint8_t off = 3; /* skip fcf and seq. */
    if (!(((fcf & 0xff00) >> 8) & IEEE802154_FCF_PAN_COMP)) {
        switch (((fcf & 0x00ff) & IEEE802154_FCF_DST_ADDR_MASK)) {
            case IEEE802154_FCF_DST_ADDR_SHORT:
                off += 4; /* skip dst. PAN and short dst. address */
                break;
            case IEEE802154_FCF_DST_ADDR_LONG:
                off += 10; /* skip dst. PAN and long dst. address */
                break;
        }
    }
    return &hdr[off];
}

static inline const uint8_t * _get_dst_pan(const uint8_t *hdr)
{
    uint16_t fcf = (((uint16_t)hdr[0]) << 8) | hdr[1];
    if (((fcf & 0x00ff) & IEEE802154_FCF_DST_ADDR_MASK) == IEEE802154_FCF_DST_ADDR_VOID) {
        /* DST PAN and DST address are not present,
           so the caller must use MAC PAN ID */
        return NULL;
    }
    return &hdr[3]; /* skip fcf and seq. */
}

static inline const uint8_t * _get_src_address(const uint8_t *hdr)
{
    uint16_t fcf = (((uint16_t)hdr[0]) << 8) | hdr[1];
    if (((fcf & 0x00ff) & IEEE802154_FCF_SRC_ADDR_MASK) == IEEE802154_FCF_SRC_ADDR_VOID) {
        /* SRC PAN and SRC address are not present,
           so the caller must use MAC PAN coordinator address */
        return NULL;
    }
    uint8_t off = 3; /* skip fcf and seq */
    switch (((fcf & 0x00ff) & IEEE802154_FCF_DST_ADDR_MASK)) {
        case IEEE802154_FCF_DST_ADDR_SHORT:
            off += 4; /* skip dst. PAN and short dst. address */
            break;
        case IEEE802154_FCF_DST_ADDR_LONG:
            off += 10; /* skip dst. PAN and long dst. address */
            break;
    }
    if (!(((fcf & 0xff00) >> 8) & IEEE802154_FCF_PAN_COMP)) {
        off += 2; /* skip src. PAN */
    }
    return &hdr[off];
}

static inline uint8_t _get_src_address_size(const uint8_t *hdr)
{
    uint16_t fcf = (((uint16_t)hdr[0]) << 8) | hdr[1];
    switch (((fcf & 0x00ff) & IEEE802154_FCF_SRC_ADDR_MASK)) {
        case IEEE802154_FCF_SRC_ADDR_SHORT:
            return IEEE802154_SHORT_ADDRESS_LEN;
        case IEEE802154_FCF_SRC_ADDR_LONG:
            return IEEE802154_LONG_ADDRESS_LEN;
    }
    return 0;
}

static inline const uint8_t * _get_dst_address(const uint8_t *hdr)
{
    uint16_t fcf = (((uint16_t)hdr[0]) << 8) | hdr[1];
    if (((fcf & 0x00ff) & IEEE802154_FCF_DST_ADDR_MASK) == IEEE802154_FCF_DST_ADDR_VOID) {
        /* DST PAN and DST address are not present,
           so the caller must use MAC PAN coordinator address */
        return NULL;
    }
    return &hdr[5]; /* skip fcf, seq. and dst. PAN */
}

static inline uint8_t _get_dst_address_size(const uint8_t *hdr)
{
    uint16_t fcf = (((uint16_t)hdr[0]) << 8) | hdr[1];
    switch (((fcf & 0x00ff) & IEEE802154_FCF_DST_ADDR_MASK)) {
        case IEEE802154_FCF_DST_ADDR_SHORT:
            return IEEE802154_SHORT_ADDRESS_LEN;
        case IEEE802154_FCF_DST_ADDR_LONG:
            return IEEE802154_LONG_ADDRESS_LEN;
    }
    return 0;
}

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
 *   Bit 7    Bit6       Bit 5 - Bit 3            Bit2 - Bit 0
 * +--------+-------+-----------------------+-----------------------+
 * | 0 (r)  | Adata |          M            |          L            |
 * +--------+-------+-----------------------+-----------------------+
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

static inline void _memxor(void *restrict dst, const void* restrict src, size_t size)
{
    while (size--) {
        ((uint8_t *)dst)[size] ^= ((const uint8_t *)src)[size];
    }
}

static inline void _memrvrscpy(void *restrict dst, const void *restrict src, size_t size)
{
    size_t s = 0;
    while (size--) {
        ((uint8_t *)dst)[s++] = ((const uint8_t *)src)[size];
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

static inline const void*_get_aux_key_source(uint8_t key_mode, const void *identifier)
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

static uint8_t _set_aux_hdr(const ieee802154_sec_context_t *ctx,
                            uint32_t frame_counter,
                            ieee802154_sec_aux_t *ahr)
{
    ahr->scf = _scf(ctx->security_level, ctx->key_id_mode);
    /* If you look in the specification: Annex C,
       integers values are in little endian */
    ahr->fc = byteorder_htoll(frame_counter).u32;
    size_t len = 5;
    switch (ctx->key_id_mode) {
        case IEEE802154_SEC_SCF_KEYMODE_IMPLICIT:
            break;
        case IEEE802154_SEC_SCF_KEYMODE_INDEX:
            memcpy(ahr->key_id, &ctx->key_index, 1);
            len++;
            break;
        case IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX:
            memcpy(ahr->key_id, ctx->key_source, 4);
            memcpy(ahr->key_id + 4, &ctx->key_index, 1);
            len += 5;
            break;
        case IEEE802154_SEC_SCF_KEYMODE_HW_INDEX:
            memcpy(ahr->key_id, ctx->key_source, 8);
            memcpy(ahr->key_id + 8, &ctx->key_index, 1);
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
    B0->nonce.frame_counter = htonl(frame_counter),
    B0->nonce.security_level = security_level,
    B0->counter = htons(m_len);
    memcpy(B0->nonce.src_addr, src_address, IEEE802154_LONG_ADDRESS_LEN);
}

/* get key lookup descriptor vgl. 9.2.2 KeyDescriptor lookup procedure */
static ieee802154_sec_key_lookup_t *_get_key(ieee802154_sec_context_t *ctx,
                                             const uint8_t *pan,
                                             const uint8_t *addr,
                                             uint8_t addr_len,
                                             uint8_t key_mode,
                                             uint8_t key_index,
                                             const void *key_source)
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
        uint8_t source_size = _key_source_size(key_mode);
        KEYLOOKUP_FOR_EACH(ctx, l, {
            if (l->key_mode == key_mode &&
                l->key_lookup.explicit.key_index == key_index &&
                !memcmp(l->key_lookup.explicit.key_source, key_source, source_size)) {
                out = l;
                break;
            }
        });
    }
    return out;
}

/* get device descriptor vgl. 9.2.5 DeviceDescriptor lookup procedure */
static ieee802154_sec_dev_lookup_t *_get_dev(ieee802154_sec_context_t *ctx,
                                             const uint8_t *dev_pan_id,
                                             const uint8_t *dev_addr,
                                             uint8_t addr_len)
{
    ieee802154_sec_dev_lookup_t *d, *out = NULL;
    if (!addr_len || !dev_pan_id || !dev_addr) {
        /* PAN coordinator not implemented */
        return NULL;
    }
    else if (addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        DEVLOOKUP_FOR_EACH(ctx, d, {
            if (!memcmp(dev_pan_id, d->pan_id, sizeof(d->pan_id)) &&
                !memcmp(dev_addr, d->short_addr, sizeof(d->short_addr))) {
                out = d;
                break;
            }
        });
    }
    else if (addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        DEVLOOKUP_FOR_EACH(ctx, d, {
            if (!memcmp(dev_pan_id, d->pan_id, sizeof(d->pan_id)) &&
                !memcmp(dev_addr, d->long_addr, sizeof(d->long_addr))) {
                out = d;
                break;
            }
        });
    }
    return out;
}

/**
 * @brief   Perform ECB on one block of data and and add padding if necessary
 */
static uint8_t _ecb(ieee802154_sec_context_t *ctx,
                    uint8_t *tmp1, uint8_t *tmp2, uint8_t *data,
                    const uint8_t *Ai, uint16_t size)
{
    uint16_t s = _min(IEEE802154_SEC_BLOCK_SIZE, size);
    if (ctx->dev.cipher_ops->ecb) {
        ctx->dev.cipher_ops->ecb(&ctx->dev, tmp2, Ai, 1);
    }
    else {
        _sec_ecb(&ctx->dev, tmp2, Ai, 1);
    }
    memcpy(tmp1, data, s);
    memset(tmp1 + s, 0, IEEE802154_SEC_BLOCK_SIZE - s);
    _memxor(tmp1, tmp2, IEEE802154_SEC_BLOCK_SIZE);
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
    uint16_t s = _min(IEEE802154_SEC_BLOCK_SIZE, size);
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
    off = _min(sizeof(tmp) - sizeof(uint16_t), a_len);
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

void _init_dev(ieee802154_sec_dev_lookup_t *dev,
               const uint8_t *pan,
               const uint8_t *short_addr,
               const uint8_t *long_addr)
{
    /* save pan and address reversed
       because that´s how it is ordered in the header */
    _memrvrscpy(dev->pan_id, pan, sizeof(dev->pan_id));
    if (short_addr) {
        _memrvrscpy(dev->short_addr, short_addr, sizeof(dev->short_addr));
    }
    else {
        uint8_t a[] = {0xff, 0xfe}; /* use only long address */
        _memrvrscpy(dev->short_addr, a, sizeof(dev->short_addr));
    }
    _memrvrscpy(dev->long_addr, long_addr, sizeof(dev->long_addr));
    dev->fc = 0;
}

static void _init_key_lookup_implicit(ieee802154_sec_key_lookup_t *key_lookup,
                                      ieee802154_sec_dev_addrmode_t dev_mode,
                                      const uint8_t *dev_pan_id,
                                      const uint8_t *dev_addr,
                                      ieee802154_sec_key_descriptor_t key)
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
    else if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_SHORT) {
        _memrvrscpy(key_lookup->key_lookup.implicit.dev_pan_id, dev_pan_id,
                    sizeof(key_lookup->key_lookup.implicit.dev_pan_id));
        _memrvrscpy(key_lookup->key_lookup.implicit.dev_addr, dev_addr,
                    IEEE802154_SHORT_ADDRESS_LEN);
    }
    else if(dev_mode == IEEE802154_SEC_DEV_ADDRMODE_LONG) {
        _memrvrscpy(key_lookup->key_lookup.implicit.dev_pan_id, &dev_pan_id,
                    sizeof(key_lookup->key_lookup.implicit.dev_pan_id));
        _memrvrscpy(key_lookup->key_lookup.implicit.dev_addr, dev_addr,
                    IEEE802154_LONG_ADDRESS_LEN);
    }
    key_lookup->key = key;
}

static void _init_key_lookup_explicit(ieee802154_sec_key_lookup_t *key_lookup,
                                      ieee802154_sec_scf_keymode_t key_mode,
                                      uint8_t key_index,
                                      const void *key_source,
                                      ieee802154_sec_key_descriptor_t key)
{
    key_lookup->key_mode = key_mode;
    key_lookup->key_lookup.explicit.key_index = key_index;
    memcpy(key_lookup->key_lookup.explicit.key_source, key_source,
           _key_source_size(key_mode));
    key_lookup->key = key;
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
        LOCK(ctx);
        memset(&ctx->key_lookup_table, 0, sizeof(ctx->key_lookup_table));
        memset(&ctx->dev_lookup_table, 0, sizeof(ctx->dev_lookup_table));
        memset(&ctx->keystore, 0, sizeof(ctx->keystore));
        UNLOCK(ctx);
        /* our default key */
        ieee802154_sec_key_descriptor_t k = ieee802154_sec_add_key(ctx, key);
        if (sec_key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
            ieee802154_sec_add_key_lookup_explicit(ctx, sec_key_mode,
                                                   sec_key_index,
                                                   sec_key_source, k);
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

    if (ctx->security_level == IEEE802154_SEC_SCF_SECLEVEL_NONE) {
        *mic_size = 0;
        return IEEE802154_SEC_OK;
    }
    uint32_t fc;
    {
        LOCK(ctx);
        /* attempt to find the encrypton key */
        const ieee802154_sec_key_lookup_t *key;
        if (!(key = _get_key(ctx,
                            _get_dst_pan(header),
                            _get_dst_address(header),
                            _get_dst_address_size(header),
                            ctx->key_id_mode,
                            ctx->key_index,
                            ctx->key_source))) {
            UNLOCK(ctx);
            DEBUG_SEC("key not found\n");
            return -IEEE802154_SEC_NO_KEY;
        }
        ieee802154_sec_key_t *k = &ctx->keystore.keys[key->key];
        if ((fc = k->fc) == 0xFFFFFFFF) {
            /* Letting the frame counter overflow is explicitly prohibited by the specification.
            (see 9.4.2) */
            UNLOCK(ctx);
            DEBUG_SEC("Frame counter would overflow\n");
            return -IEEE802154_SEC_FRAME_COUNTER_OVERFLOW;
        }
        k->fc++;
        _set_key(ctx, k->key);
        UNLOCK(ctx);
    }
    /* write the auxiliary header */
    ieee802154_sec_aux_t *aux = (ieee802154_sec_aux_t *)(header + *header_size);
    uint8_t aux_size = _get_aux_hdr_size(ctx->security_level, ctx->key_id_mode);
    _set_aux_hdr(ctx, fc, aux);

    *mic_size = _mac_size(ctx->security_level);
    const uint8_t *a = header;
    uint8_t *m = payload;
    uint16_t a_len = *header_size + aux_size;
    uint16_t m_len = payload_size;
    ieee802154_sec_ccm_block_t ccm; /* Ai or Bi */

    /* compute MIC */
    if (_req_mac(ctx->security_level)) {
        _init_cbc_B0(&ccm, fc, ctx->security_level, m_len, *mic_size, src_address);
        _comp_mic(ctx, mic, &ccm, a, a_len, m, m_len);

        /* encrypt MIC */
        _init_ctr_A0(&ccm, fc, ctx->security_level, src_address);
        _ctr_mic(ctx, &ccm, mic, *mic_size);
    }
    /* encrypt payload */
    if (_req_encryption(ctx->security_level)) {
        _init_ctr_A0(&ccm, fc, ctx->security_level, src_address);
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
        LOCK(ctx);
        ieee802154_sec_key_lookup_t *key;
        /* attempt to find key descriptor */
        if (!(key = _get_key(ctx,
                            _get_src_pan(header),
                            _get_src_address(header),
                            _get_src_address_size(header),
                            key_mode,
                            _get_aux_key_index(key_mode, aux->key_id),
                            _get_aux_key_source(key_mode, aux->key_id)))) {
            UNLOCK(ctx);
            DEBUG_SEC("key not found\n");
            return -IEEE802154_SEC_NO_KEY;
        }
#if IS_ACTIVE(CONFIG_IEEE802154_SEC_DEFAULT_REPLAY_PROTECTION)
        /* attempt to find device descriptor */
        ieee802154_sec_dev_lookup_t *dev;
        if (!(dev = _get_dev(ctx,
                             _get_src_pan(header),
                             _get_src_address(header),
                             _get_src_address_size(header)))) {
            UNLOCK(ctx);
            DEBUG_SEC("device not found\n");
            return -IEEE802154_SEC_NO_DEV;
        }
        if (!bf_isset(ctx->keystore.keys[key->key].dev_fc_list, dev - ctx->dev_lookup_table.dev_lookup)) {
            UNLOCK(ctx);
            DEBUG_SEC("device not peered\n");
            return -IEEE802154_SEC_NO_DEV;
        }
        /* check frame counter to fend replay attacks */
        if (frame_counter < dev->fc) {
            UNLOCK(ctx);
            DEBUG_SEC("Frame counter indicates replay\n");
            return -IEEE802154_SEC_FRAME_COUNTER_ERROR;
        }
        dev->fc = frame_counter + 1;
        _memrvrscpy(src_address, dev->long_addr, sizeof(dev->long_addr));
#else
        if (_get_src_address_size(header) != IEEE802154_LONG_ADDRESS_LEN) {
            DEBUG_SEC("No long source address\n");
            return -IEEE802154_SEC_NO_DEV;
        }
        _memrvrscpy(src_address, _get_src_address(header), IEEE802154_LONG_ADDRESS_LEN);
#endif
        _set_key(ctx, ctx->keystore.keys[key->key].key);
        UNLOCK(ctx);
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

ieee802154_sec_dev_descriptor_t ieee802154_sec_add_dev(ieee802154_sec_context_t *ctx,
                                                       uint16_t pan_id,
                                                       const uint8_t *short_addr,
                                                       const uint8_t *long_addr)
{
    pan_id = byteorder_htons(pan_id).u16;
    ieee802154_sec_dev_lookup_t tmp_dev;
    _init_dev(&tmp_dev, (uint8_t *)&pan_id, short_addr, long_addr);
    LOCK(ctx);
    /* check for duplicate */
    ieee802154_sec_dev_lookup_t *tmp = NULL;
    DEVLOOKUP_FOR_EACH(ctx, tmp, {
        if (!memcmp(&tmp->pan_id, &tmp_dev.pan_id, sizeof(tmp_dev.pan_id)) &&
            !memcmp(&tmp->short_addr, &tmp_dev.short_addr, sizeof(tmp_dev.short_addr)) &&
            !memcmp(&tmp->long_addr, &tmp_dev.long_addr, sizeof(tmp_dev.long_addr))) {
            UNLOCK(ctx);
            return tmp - ctx->dev_lookup_table.dev_lookup;
        }
    })
    int d = bf_get_unset(ctx->dev_lookup_table.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE);
    if (d < 0) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    ctx->dev_lookup_table.dev_lookup[d] = tmp_dev;
    UNLOCK(ctx);
    return d;
}

ieee802154_sec_key_descriptor_t ieee802154_sec_add_key(ieee802154_sec_context_t *ctx,
                                                       const uint8_t *key)
{
    ieee802154_sec_key_t tmp_key;
    memset(&tmp_key, 0, sizeof(tmp_key));
    memcpy(&tmp_key.key, key, sizeof(tmp_key.key));
    LOCK(ctx);
    int k = bf_get_unset(ctx->keystore.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE);
    if (k < 0) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    ctx->keystore.keys[k] = tmp_key;
    UNLOCK(ctx);
    return k;
}

ieee802154_sec_key_lookup_descriptor_t
ieee802154_sec_add_key_lookup_implicit(ieee802154_sec_context_t *ctx,
                                       ieee802154_sec_dev_addrmode_t dev_mode,
                                       uint16_t dev_pan_id,
                                       const uint8_t *dev_addr,
                                       ieee802154_sec_key_descriptor_t k)
{
    /* Check if the key exists */
    if (dev_mode != IEEE802154_SEC_DEV_ADDRMODE_NONE &&
        dev_mode != IEEE802154_SEC_DEV_ADDRMODE_SHORT &&
        dev_mode != IEEE802154_SEC_DEV_ADDRMODE_LONG) {
        return IEEE802154_SEC_NO_IDENT;
    }
    ieee802154_sec_key_lookup_t tmp_lookup;
    _init_key_lookup_implicit(&tmp_lookup, dev_mode, (uint8_t *)&dev_pan_id, dev_addr, k);
    LOCK(ctx);
    /* check for duplicate */
    ieee802154_sec_key_lookup_t *tmp;
    KEYLOOKUP_FOR_EACH(ctx, tmp, {
        if (tmp->key_mode == tmp_lookup.key_mode &&
            !memcmp(&tmp->key_lookup, &tmp_lookup.key_lookup, sizeof(tmp_lookup.key_lookup))) {
            UNLOCK(ctx);
            return tmp - ctx->key_lookup_table.key_lookup;
        }
    })
    if (!bf_isset(ctx->keystore.mask, k)) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    /* Check if the device exists */
    ieee802154_sec_dev_lookup_t *dev;
    dev_pan_id = byteorder_htons(dev_pan_id).u16;
    uint16_t pan = ((dev_pan_id & 0x00ff) << 8) | ((dev_pan_id & 0xff00) >> 8);
    if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_NONE) {
        dev = _get_dev(ctx, (uint8_t *)&pan, NULL, 0);
    }
    else if (dev_mode == IEEE802154_SEC_DEV_ADDRMODE_SHORT) {
        uint8_t addr[IEEE802154_SHORT_ADDRESS_LEN];
        _memrvrscpy(addr, dev_addr, sizeof(addr));
        dev = _get_dev(ctx, (uint8_t *)&pan, addr, sizeof(addr));
    }
    else {
        uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];
        _memrvrscpy(addr, dev_addr, sizeof(addr));
        dev = _get_dev(ctx, (uint8_t *)&pan, addr, sizeof(addr));
    }
    if (!dev) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    /* allocate a lookup descriptor */
    int l = bf_get_unset(ctx->key_lookup_table.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);
    if (l < 0) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    ctx->key_lookup_table.key_lookup[l] = tmp_lookup;
    /* device bit in key descriptor should be set during pairings phase,
       see ieee802154_sec_peer_dev() */
    UNLOCK(ctx);
    return l;
}

ieee802154_sec_key_lookup_descriptor_t
ieee802154_sec_add_key_lookup_explicit(ieee802154_sec_context_t *ctx,
                                       ieee802154_sec_scf_keymode_t key_mode,
                                       uint8_t key_index,
                                       const void *key_source,
                                       ieee802154_sec_key_descriptor_t k)
{
    if (key_mode != IEEE802154_SEC_SCF_KEYMODE_INDEX &&
        key_mode != IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX &&
        key_mode != IEEE802154_SEC_SCF_KEYMODE_HW_INDEX) {
        return IEEE802154_SEC_NO_IDENT;
    }
    if (key_index == 0x00) {
        return IEEE802154_SEC_NO_IDENT; /* not allowed */
    }
    ieee802154_sec_key_lookup_t tmp_lookup;
    _init_key_lookup_explicit(&tmp_lookup, key_mode, key_index, key_source, k);
    LOCK(ctx);
    /* check for duplicate */
    ieee802154_sec_key_lookup_t *tmp;
    KEYLOOKUP_FOR_EACH(ctx, tmp, {
        if (tmp->key_mode == tmp_lookup.key_mode &&
            !memcmp(&tmp->key_lookup, &tmp_lookup.key_lookup, sizeof(tmp_lookup.key_lookup))) {
            UNLOCK(ctx);
            return tmp - ctx->key_lookup_table.key_lookup;
        }
    })
    /* Check if the key exists */
    if (!bf_isset(ctx->keystore.mask, k)) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    /* allocate a lookup descriptor */
    int l = bf_get_unset(ctx->key_lookup_table.mask,
                         CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);
    if (l < 0) {
        UNLOCK(ctx);
        return IEEE802154_SEC_NO_IDENT;
    }
    ctx->key_lookup_table.key_lookup[l] = tmp_lookup;
    /* device bit in key descriptor should be set during pairings phase,
       see ieee802154_sec_peer_dev() */
    UNLOCK(ctx);
    return l;
}

void ieee802154_sec_remove_dev(ieee802154_sec_context_t *ctx,
                               ieee802154_sec_dev_descriptor_t d)
{
    assert(d < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE);
    LOCK(ctx);
    /* delete lookup references */
    ieee802154_sec_key_lookup_t *lookup;
    KEYLOOKUP_FOR_EACH(ctx, lookup,
        /* if an association between a key and the device exists */
        if (bf_isset(ctx->keystore.keys[lookup->key].dev_fc_list, d)) {
            /* delete association */
            bf_unset(ctx->keystore.keys[lookup->key].dev_fc_list, d);
            /* release the lookup descriptor */
            if (lookup->key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
                bf_unset(ctx->dev_lookup_table.mask, lookup - ctx->key_lookup_table.key_lookup);
                memset(lookup, 0, sizeof(*lookup));
            }
        }
    )
    /* release device descriptor */
    bf_unset(ctx->dev_lookup_table.mask, d);
    memset(&ctx->dev_lookup_table.dev_lookup[d], 0,
           sizeof(ctx->dev_lookup_table.dev_lookup[d]));
    UNLOCK(ctx);
}

void ieee802154_sec_remove_key(ieee802154_sec_context_t *ctx,
                               ieee802154_sec_key_descriptor_t k)
{
    assert(k < CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE);
    LOCK(ctx);
    /* delete lookup references */
    ieee802154_sec_key_lookup_t *lookup;
    KEYLOOKUP_FOR_EACH(ctx, lookup,
        if (lookup->key == k) {
            /* reset device frame counters which reference the key */
            for (unsigned d = 0; d < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE; d++) {
                if (bf_isset(ctx->keystore.keys[k].dev_fc_list, d)) {
                    ctx->dev_lookup_table.dev_lookup[d].fc = 0;
                }
            }
            bf_unset(ctx->dev_lookup_table.mask, lookup - ctx->key_lookup_table.key_lookup);
            memset(lookup, 0, sizeof(*lookup));
        }
    )
    /* release key descriptor */
    bf_unset(ctx->keystore.mask, k);
    memset(&ctx->keystore.keys[k], 0, sizeof(ctx->keystore.keys[k]));
    UNLOCK(ctx);
}

void ieee802154_sec_remove_key_lookup(ieee802154_sec_context_t *ctx,
                                      ieee802154_sec_key_lookup_descriptor_t l)
{
    assert(l < CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);
    LOCK(ctx);
    bf_unset(ctx->dev_lookup_table.mask, l);
    memset(&ctx->dev_lookup_table.dev_lookup[l], 0,
           sizeof(ctx->dev_lookup_table.dev_lookup[l]));
    UNLOCK(ctx);
}

int ieee802154_sec_update_key(ieee802154_sec_context_t *ctx,
                               ieee802154_sec_key_descriptor_t k,
                               const uint8_t *new)
{
    assert(k < CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE);
    ieee802154_sec_key_t *key = &ctx->keystore.keys[k];
    LOCK(ctx);
    if (!bf_isset(ctx->keystore.mask, k)) {
        UNLOCK(ctx);
        return -IEEE802154_SEC_NO_KEY;
    }
    if (memcmp(new, key->key, sizeof(key->key))) {
        key->fc = 0;
        memcpy(key->key, new, sizeof(key->key));
        for (unsigned d = 0; d < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE; d++) {
            if (bf_isset(key->dev_fc_list, d)) {
                ctx->dev_lookup_table.dev_lookup[d].fc =  0;
                /* TODO: pairing with device */
            }
        }
    }
    UNLOCK(ctx);
    return IEEE802154_SEC_OK;
}

int ieee802154_sec_peer_dev(ieee802154_sec_context_t *ctx,
                             ieee802154_sec_key_lookup_descriptor_t l,
                             uint16_t pan_id,
                             const uint8_t *short_addr, const uint8_t *long_addr)
{
    assert(l < CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);
    ieee802154_sec_key_lookup_t *lookup = &ctx->key_lookup_table.key_lookup[l];
    ieee802154_sec_dev_lookup_t *dev = NULL;
    pan_id = byteorder_htons(pan_id).u16;
    uint16_t pan = ((pan_id & 0x00ff) << 8) | ((pan_id & 0xff00) >> 8);
    LOCK(ctx);
    if (!bf_isset(ctx->key_lookup_table.mask, l)) {
        UNLOCK(ctx);
        return -IEEE802154_SEC_NO_KEY;
    }
    /* find device */
    if (lookup->key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
        if (lookup->key_lookup.implicit.dev_mode == IEEE802154_SEC_DEV_ADDRMODE_NONE) {
            dev = _get_dev(ctx, (uint8_t *)&pan, NULL, 0);
        }
        else if (lookup->key_lookup.implicit.dev_mode == IEEE802154_SEC_DEV_ADDRMODE_SHORT) {
            uint8_t addr[IEEE802154_SHORT_ADDRESS_LEN];
            _memrvrscpy(addr, short_addr, sizeof(addr));
            dev = _get_dev(ctx, (uint8_t *)&pan, addr, sizeof(addr));
        }
        else if (lookup->key_lookup.implicit.dev_mode == IEEE802154_SEC_DEV_ADDRMODE_LONG) {
            uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];
            _memrvrscpy(addr, long_addr, sizeof(addr));
            dev = _get_dev(ctx, (uint8_t *)&pan, addr, sizeof(addr));
        }
    }
    else {
        uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];
        _memrvrscpy(addr, long_addr, sizeof(addr));
        dev = _get_dev(ctx, (uint8_t *)&pan, addr, sizeof(addr));
    }
    if (!dev) {
        UNLOCK(ctx);
        return -IEEE802154_SEC_NO_DEV;
    }
    /* remove previous pairings */
    ieee802154_sec_key_lookup_t *tmp_lookup = NULL;
    KEYLOOKUP_FOR_EACH(ctx, tmp_lookup,
        if (bf_isset(ctx->keystore.keys[tmp_lookup->key].dev_fc_list,
                        dev - ctx->dev_lookup_table.dev_lookup)) {
            bf_unset(ctx->keystore.keys[tmp_lookup->key].dev_fc_list,
                        dev - ctx->dev_lookup_table.dev_lookup);
            if (tmp_lookup->key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
                bf_unset(ctx->dev_lookup_table.mask,
                         tmp_lookup - ctx->key_lookup_table.key_lookup);
                memset(tmp_lookup, 0, sizeof(*tmp_lookup));
            }
        }
    )
    /* TODO: Send pairings request. For now, assume pairings succeeds unconditionally. */
    bf_set(ctx->keystore.keys[lookup->key].dev_fc_list,
           dev - ctx->dev_lookup_table.dev_lookup);
    dev->fc = 0;
    UNLOCK(ctx);
    return IEEE802154_SEC_OK;
}

void ieee802154_sec_iterator_acquire(ieee802154_sec_context_t *ctx)
{
    LOCK(ctx);
}

void ieee802154_sec_iterator_release(ieee802154_sec_context_t *ctx)
{
    UNLOCK(ctx);
}

ieee802154_sec_key_descriptor_t
ieee802154_sec_key_iterator(ieee802154_sec_context_t *ctx,
                            ieee802154_sec_key_descriptor_t prev,
                            ieee802154_sec_key_t *key)
{
    if (prev >= CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE) {
        prev = 0;
    }
    else if (++prev == CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE) {
        return IEEE802154_SEC_NO_IDENT;
    }
    while (prev < CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE &&
           !bf_isset(ctx->keystore.mask, prev)) {
        prev++;
    }
    if (key && prev < CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE) {
        *key = ctx->keystore.keys[prev];
    }
    return prev < CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE
           ? prev : IEEE802154_SEC_NO_IDENT;
}

ieee802154_sec_dev_descriptor_t
ieee802154_sec_dev_iterator(ieee802154_sec_context_t *ctx,
                            ieee802154_sec_dev_descriptor_t prev,
                            ieee802154_sec_dev_lookup_t *dev)
{
    if (prev >= CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE) {
        prev = 0;
    }
    else if (++prev == CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE) {
        return IEEE802154_SEC_NO_IDENT;
    }
    while (prev < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE &&
           !bf_isset(ctx->dev_lookup_table.mask, prev)) {
        prev++;
    }
    if (dev && prev < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE) {
        *dev = ctx->dev_lookup_table.dev_lookup[prev];
    }
    return prev < CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE
           ? prev : IEEE802154_SEC_NO_IDENT;
}

ieee802154_sec_key_lookup_descriptor_t
ieee802154_sec_key_lookup_iterator(ieee802154_sec_context_t *ctx,
                                   ieee802154_sec_key_lookup_descriptor_t prev,
                                   ieee802154_sec_key_lookup_t *lookup)
{
    if (prev >= CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE) {
        prev = 0;
    }
    else if (++prev == CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE) {
        return IEEE802154_SEC_NO_IDENT;
    }
    while (prev < CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE &&
           !bf_isset(ctx->key_lookup_table.mask, prev)) {
        prev++;
    }
    if (lookup && prev < CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE) {
        *lookup = ctx->key_lookup_table.key_lookup[prev];
    }
    return prev < CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE
           ? prev : IEEE802154_SEC_NO_IDENT;
}
