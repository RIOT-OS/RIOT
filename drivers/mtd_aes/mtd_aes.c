/*
 * Copyright (C) 2020 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd_aes
 * @{
 *
 * @file
 * @brief       Driver for flash memory encryption
 *
 * @author      Juergen Fitschen <jfi@ssv-embedded.de>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>

#include "kernel_defines.h"
#include "mtd.h"
#include "mtd_aes.h"
#include "mutex.h"
#include "byteorder.h"
#include "crypto/modes/ctr.h"
#include "od.h"

static inline bool _add_overflow(uint64_t a, uint64_t b, uint64_t *res)
{
#ifdef BUILTIN_ADD_OVERFLOW_EXIST
    return __builtin_add_overflow(a, b, res);
#else
    *res = a + b;
    return (a > UINT64_MAX - b);
#endif
}

static inline void _store_iv(mtd_aes_t *aes, const uint8_t iv[AES_BLOCK_SIZE])
{
    /* network_uint64_t is big endian */
    const network_uint64_t * iv_be = (const network_uint64_t*) iv;
    for (size_t i = 0; i < AES_BLOCK_SIZE / sizeof(network_uint64_t); i++) {
        /* store the iv with the CPU's endianess */
        aes->iv[i] = byteorder_ntohll(iv_be[i]);
    }
}

static inline void _get_ctr(const mtd_aes_t *aes, uint64_t offset, uint8_t ctr[AES_BLOCK_SIZE])
{
    network_uint64_t * ctr_be = (network_uint64_t*) ctr;
    for (ssize_t i = 1; i >= 0; i--) {
        uint64_t sum;

        if (offset) {
            bool overflow = _add_overflow(aes->iv[i], offset, &sum);
            offset = overflow ? 1 : 0;
        } else {
            sum = aes->iv[i];
        }

        ctr_be[i] = byteorder_htonll(sum);
    }
}

static inline size_t _min(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

static int _init(mtd_dev_t *mtd)
{
    mtd_aes_t *aes = container_of(mtd, mtd_aes_t, mtd);
    return mtd_init(aes->parent);
}

static int _write_page(mtd_dev_t *mtd, const void *src, uint32_t page, uint32_t offset, uint32_t count)
{
    int rc;
    mtd_aes_t *aes = container_of(mtd, mtd_aes_t, mtd);
    uint8_t ctr[AES_BLOCK_SIZE];
    uint8_t buf[aes->mtd.page_size];
    uint32_t written = 0;

    assert(offset < aes->mtd.page_size);

    /* Craft nonce from page number */
    _get_ctr(aes, page * mtd->page_size / AES_BLOCK_SIZE, ctr);

    /* Go thru src page by page */
    while (count) {
        const size_t to_copy = _min(aes->mtd.page_size - offset, count);

        /* Copy src into buf with correct alignment */
        memcpy(buf + offset, src, to_copy);

        /* Crypt page buffer */
        rc = cipher_encrypt_ctr(&aes->cipher, ctr, 0, buf, sizeof(buf), buf);
        if (rc < 0) {
            return -EIO;
        }

        /* Write crypted buffer into parent device */
        rc = mtd_write_page(aes->parent, buf + offset, page, offset, to_copy);
        if (rc < 0) {
            return rc;
        }

        offset = 0;
        count -= to_copy;
        written += to_copy;
        page += 1;
    }

    return written;
}

static int _read_page(mtd_dev_t *mtd, void *dest, uint32_t page, uint32_t offset, uint32_t count)
{
    int rc;
    mtd_aes_t *aes = container_of(mtd, mtd_aes_t, mtd);
    uint8_t ctr[AES_BLOCK_SIZE];
    uint8_t buf[aes->mtd.page_size];
    uint32_t read = 0;

    assert(offset < aes->mtd.page_size);

    /* Craft nonce from page number */
    _get_ctr(aes, page * mtd->page_size / AES_BLOCK_SIZE, ctr);

    /* Read from parent page by page */
    while (count) {
        const size_t to_copy = _min(aes->mtd.page_size - offset, count);

        /* Copy src into buf with correct alignment */
        rc = mtd_read_page(aes->parent, buf + offset, page, offset, to_copy);
        if (rc < 0) {
            return rc;
        }

        /* Crypt page buffer */
        rc = cipher_encrypt_ctr(&aes->cipher, ctr, 0, buf, sizeof(buf), buf);
        if (rc < 0) {
            return -EIO;
        }

        /* Copy crypted buffer to dest */
        memcpy(dest, buf + offset, to_copy);

        offset = 0;
        count -= to_copy;
        read += to_copy;
        page += 1;
    }

    return read;
}

static int _erase_sector(mtd_dev_t *mtd, uint32_t sector, uint32_t count)
{
    mtd_aes_t *aes = container_of(mtd, mtd_aes_t, mtd);
    return mtd_erase_sector(aes->parent, sector, count);
}

static int _power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    mtd_aes_t *aes = container_of(mtd, mtd_aes_t, mtd);
    return mtd_power(aes->parent, power);
}

int mtd_aes_init(mtd_aes_t * aes, mtd_dev_t * parent,
                 const uint8_t key[AES_KEY_SIZE], const uint8_t iv[AES_BLOCK_SIZE])
{
    aes->parent = parent;
    aes->mtd.driver = &mtd_aes_driver;
    aes->mtd.page_size = parent->page_size;
    aes->mtd.pages_per_sector = parent->pages_per_sector;
    aes->mtd.sector_count = parent->sector_count;
    _store_iv(aes, iv);
    return cipher_init(&aes->cipher, CIPHER_AES_128, key, AES_KEY_SIZE);
}

const mtd_desc_t mtd_aes_driver = {
    .init = _init,
    .read_page = _read_page,
    .write_page = _write_page,
    .erase_sector = _erase_sector,
    .power = _power,
};
