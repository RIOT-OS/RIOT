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
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "crypto/ciphers.h"
#include "crypto/modes/ecb.h"
#include "crypto/modes/cbc.h"
#include "net/ieee802154_security.h"

const ieee802154_radio_cipher_ops_t ieee802154_radio_cipher_ops = {
    .set_key = NULL,
    .ecb = NULL,
    .cbc = NULL
};

static inline uint16_t _min(uint16_t a, uint16_t b)
{
    return a < b ? a : b;
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
    return (scf & IEEE802154_SCF_SECLEVEL_MASK)
           >> IEEE802154_SCF_SECLEVEL_SHIFT;
}

static inline uint8_t _get_key_id_mode(uint8_t scf)
{
    return (scf & IEEE802154_SCF_KEYMODE_MASK)
           >> IEEE802154_SCF_KEYMODE_SHIFT;
}

static inline uint8_t _mac_size(uint8_t sec_level)
{
    switch (sec_level) {
        case IEEE802154_SCF_SECLEVEL_MIC32:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC32:
            return 4;
        case IEEE802154_SCF_SECLEVEL_MIC64:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC64:
            return 8;
        case IEEE802154_SCF_SECLEVEL_MIC128:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC128:
            return 16;
        default:
            return 0;
    }
}

/* frame is secured with signature */
static inline bool _req_mac(uint8_t sec_level)
{
    switch (sec_level) {
        case IEEE802154_SCF_SECLEVEL_MIC32:
        case IEEE802154_SCF_SECLEVEL_MIC64:
        case IEEE802154_SCF_SECLEVEL_MIC128:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC32:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC64:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC128:
            return true;
        default:
            return false;
    }
}

/* frame is encrypted */
static inline bool _req_encryption(uint8_t sec_level)
{
    switch (sec_level) {
        case IEEE802154_SCF_SECLEVEL_ENC:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC32:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC64:
        case IEEE802154_SCF_SECLEVEL_ENC_MIC128:
            return true;
        default:
            return false;
    }
}

static inline void _memxor(void *dst, const void* src, size_t size)
{
    while (size--) {
        ((uint8_t *)dst)[size] ^= ((uint8_t *)src)[size];
    }
}

static inline uint8_t _scf(uint8_t sec_level, uint8_t key_mode)
{
    return (sec_level << IEEE802154_SCF_SECLEVEL_SHIFT) |
           (key_mode << IEEE802154_SCF_KEYMODE_SHIFT);
}

static inline uint8_t _get_aux_hdr_size(uint8_t security_level,
                                        uint8_t key_mode)
{
    if (security_level == IEEE802154_SCF_SECLEVEL_NONE) {
            return 0;
    }
    switch (key_mode) {
        case IEEE802154_SCF_KEYMODE_IMPLICIT:
            return 5;
        case IEEE802154_SCF_KEYMODE_INDEX:
            return 6;
        case IEEE802154_SCF_KEYMODE_SHORT_INDEX:
            return 10;
        case IEEE802154_SCF_KEYMODE_HW_INDEX:
            return 14;
        default:
            return 0;
    }
}

static uint8_t _set_aux_hdr(const ieee802154_sec_context_t *ctx,
                            ieee802154_aux_sec_t *ahr)
{
    ahr->scf = _scf(ctx->security_level, ctx->key_id_mode);
    /* If you look in the specification: Annex C,
       integers values are in little endian */
    ahr->fc = byteorder_htoll(ctx->frame_counter).u32;
    size_t len = 5;
    switch (ctx->key_id_mode) {
        case IEEE802154_SCF_KEYMODE_IMPLICIT:
            break;
        case IEEE802154_SCF_KEYMODE_INDEX:
            memcpy(ahr->key_id, &ctx->key_index, 1);
            len++;
            break;
        case IEEE802154_SCF_KEYMODE_SHORT_INDEX:
            memcpy(ahr->key_id, ctx->key_source, 4);
            memcpy(ahr->key_id + 4, &ctx->key_index, 1);
            len += 5;
            break;
        case IEEE802154_SCF_KEYMODE_HW_INDEX:
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
static inline void _init_ctr_A0(ieee802154_ccm_block_t *A0,
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
static inline void _advance_ctr_Ai(ieee802154_ccm_block_t *Ai)
{
    Ai->counter = htons(ntohs(Ai->counter) + 1);
}

/**
 * @brief   Construct the first block B0 for CBC-MAC
 */
static inline void _init_cbc_B0(ieee802154_ccm_block_t *B0,
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

static const uint8_t *_get_encryption_key(const ieee802154_sec_context_t *ctx,
                                          const uint8_t *mhr, uint8_t mhr_len,
                                          const ieee802154_aux_sec_t *ahr)
{
    (void)mhr;
    (void)mhr_len;
    (void)ahr;
    /* For simplicity, assume that everyone has the same key */
    /* Else you´d have to look up the key based on the destination address */
    return ctx->cipher.context.context;
}

static const uint8_t *_get_decryption_key(const ieee802154_sec_context_t *ctx,
                                          const uint8_t *mhr, uint8_t mhr_len,
                                          const ieee802154_aux_sec_t *ahr)
{
    (void)mhr;
    (void)mhr_len;
    (void)ahr;
    /* For simplicity, assume that everyone has the same key */
    /* Else you´d have to look up the key based on the source address */
    return ctx->cipher.context.context;
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
                      uint8_t mic[IEEE802154_MAC_SIZE],
                      ieee802154_ccm_block_t *B0,
                      const void *a, uint16_t a_len,
                      const void *m, uint16_t m_len)
{
    uint8_t tmp[IEEE802154_SEC_BLOCK_SIZE] = { 0 };
    uint16_t off;
    memset(mic, 0, IEEE802154_MAC_SIZE);
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
                 ieee802154_ccm_block_t *A0,
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
                     ieee802154_ccm_block_t *A0,
                     void *mic, uint8_t mic_size)
{
    uint8_t tmp1[IEEE802154_SEC_BLOCK_SIZE] = { 0 };
    uint8_t tmp2[IEEE802154_SEC_BLOCK_SIZE] = { 0 };

    _ecb(ctx, tmp1, tmp2, mic, (uint8_t *)A0, mic_size);
}

void ieee802154_sec_init(ieee802154_sec_context_t *ctx)
{
    /* device driver can override this */
    ctx->dev.cipher_ops = &ieee802154_radio_cipher_ops;
    /* device driver can override this */
    ctx->dev.ctx = ctx;
    /* MIC64 is the only mandatory security mode */
    ctx->security_level = IEEE802154_SCF_SECLEVEL_ENC_MIC64;
    ctx->key_id_mode = IEEE802154_SCF_KEYMODE_IMPLICIT;
    memset(ctx->key_source, 0, sizeof(ctx->key_source));
    ctx->key_index = 0;
    ctx->frame_counter = 0;
    uint8_t key[] = IEEE802154_DEFAULT_KEY;

    assert(CIPHER_MAX_CONTEXT_SIZE >= IEEE802154_SEC_KEY_LENGTH);
    cipher_init(&ctx->cipher, CIPHER_AES_128, key, IEEE802154_SEC_KEY_LENGTH);
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

    if (ctx->security_level == IEEE802154_SCF_SECLEVEL_NONE) {
        *mic_size = 0;
        return IEEE802154_SEC_OK;
    }
    if (ctx->frame_counter == 0xFFFFFFFF) {
        /* Letting the frame counter overflow is explicitly prohibited by the specification.
           (see 9.4.2) */
        return -IEEE802154_SEC_FRAME_COUNTER_OVERFLOW;
    }

    /* write the auxiliary header */
    ieee802154_aux_sec_t *aux = (ieee802154_aux_sec_t *)(header + *header_size);
    uint8_t aux_size = _get_aux_hdr_size(ctx->security_level, ctx->key_id_mode);
    _set_aux_hdr(ctx, aux);

    /* attempt to find the encrypton key */
    const uint8_t *key;
    if (!(key = _get_encryption_key(ctx, header, *header_size, aux))) {
        return -IEEE802154_SEC_NO_KEY;
    }
    _set_key(ctx, key);

    *mic_size = _mac_size(ctx->security_level);
    const uint8_t *a = header;
    uint8_t *m = payload;
    uint16_t a_len = *header_size + aux_size;
    uint16_t m_len = payload_size;
    ieee802154_ccm_block_t ccm; /* Ai or Bi */

    /* compute MIC */
    if (_req_mac(ctx->security_level)) {
        _init_cbc_B0(&ccm, ctx->frame_counter, ctx->security_level, m_len, *mic_size, src_address);
        _comp_mic(ctx, mic, &ccm, a, a_len, m, m_len);

        /* encrypt MIC */
        _init_ctr_A0(&ccm, ctx->frame_counter, ctx->security_level, src_address);
        _ctr_mic(ctx, &ccm, mic, *mic_size);
    }
    /* encrypt payload */
    if (_req_encryption(ctx->security_level)) {
        _init_ctr_A0(&ccm, ctx->frame_counter, ctx->security_level, src_address);
        _ctr(ctx, &ccm, m, m_len);
    }
    *header_size += aux_size;
    ctx->frame_counter++;
    return IEEE802154_SEC_OK;
}

int ieee802154_sec_decrypt_frame(ieee802154_sec_context_t *ctx,
                                 uint16_t frame_size,
                                 uint8_t *header, uint8_t *header_size,
                                 uint8_t **payload, uint16_t *payload_size,
                                 uint8_t **mic, uint8_t *mic_size,
                                 const uint8_t *src_address)
{
    /* For non data frames (MAC commands, beacons) a and a_len would be larger.
       ACKs are not encrypted. */
    assert(*header & IEEE802154_FCF_TYPE_DATA);

    /* read the fields of the auxiliary header */
    ieee802154_aux_sec_t *aux = (ieee802154_aux_sec_t *)(header + *header_size);
    uint8_t security_level = _get_sec_level(aux->scf);
    uint8_t key_mode = _get_key_id_mode(aux->scf);
    uint8_t aux_size = _get_aux_hdr_size(security_level, key_mode);
    uint8_t mac_size = _mac_size(security_level);
    /* remember that the frame counter was stored in little endian */
    uint32_t frame_counter = byteorder_ltohl((le_uint32_t){aux->fc});

    if (security_level == IEEE802154_SCF_SECLEVEL_NONE) {
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

    /* attempt to find the decryption key */
    const uint8_t *key;
    if (!(key = _get_decryption_key(ctx, header, *header_size, aux))) {
        return -IEEE802154_SEC_NO_KEY;
    }
    _set_key(ctx, key);

    const uint8_t *a = header;
    uint8_t *c = *payload;
    uint16_t a_len = *header_size + aux_size;
    uint16_t c_len = *payload_size;
    uint8_t *mac = *mic;
    ieee802154_ccm_block_t ccm; /* Ai or Bi */

    /* TODO:
       A better implementation would check if the received frame counter is
       greater then the frame counter that has previously been received from
       the other endpoint. This is done to protect against replay attacks.
       But we do not store this information because we also do not have
       a proper key store, to avoid complexity on embedded devices. */

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
        uint8_t tmp_mic[IEEE802154_MAC_SIZE];
        _init_cbc_B0(&ccm, frame_counter, security_level, c_len, mac_size, src_address);
        _comp_mic(ctx, tmp_mic, &ccm, a, a_len, c, c_len);
        if (memcmp(tmp_mic, *mic, mac_size)) {
            return -IEEE802154_SEC_MAC_CHECK_FAILURE;
        }
    }
    *header_size += aux_size;
    return IEEE802154_SEC_OK;
}
