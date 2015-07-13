/*
 * Copyright (C) 2003-2005 by Christopher R. Hertel
 *               2015 Freie Universität Berlin
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @defgroup    sys_hashes_md5 MD5
 * @ingroup     sys_hashes
 * @brief       Implementation of the MD5 hashing function
 *
 *  None of this will make any sense unless you're studying RFC 1321 as you
 *  read the code.
 *
 *  There are three primary motivations for this particular implementation.
 *  1) Programmer's pride.  I wanted to be able to say I'd done it, and I
 *     wanted to learn from the experience.
 *  2) Portability.  I wanted an implementation that I knew to be portable
 *     to a reasonable number of platforms.  In particular, the algorithm is
 *     designed with little-endian platforms in mind, but I wanted an
 *     endian-agnostic implementation.
 *  3) Compactness.  While not an overriding goal, I thought it worth-while
 *     to see if I could reduce the overall size of the result.  This is in
 *     keeping with my hopes that this library will be suitable for use in
 *     some embedded environments.
 *  Beyond that, cleanliness and clarity are always worth pursuing.
 *
 *  As mentioned above, the code really only makes sense if you are familiar
 *  with the MD5 algorithm or are using RFC 1321 as a guide.  This code is
 *  quirky, however, so you'll want to be reading carefully.
 *
 * @{
 *
 * @file
 * @brief       MD5 interface definition
 *
 * @author      Christopher R. Hertel <crh@ubiqx.mn.org>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HASHES_MD5_H
#define HASHES_MD5_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of MD5 digests in byte
 */
#define MD5_DIGEST_LENGTH           (16U)

/**
 * @brief   MD5 calculation context
 */
typedef struct
{
    uint32_t len;       /**< overall number of bytes processed */
    uint32_t abcd[4];   /**< virtual registers for hash calculation */
    int b_used;         /**< number of bytes used in the current block */
    uint8_t block[64];  /**< one block is calculated at a time */
} md5_ctx_t;

/**
 * @brief   Initialize the MD5 calculation context
 *
 * @param[out] ctx      Pointer to the context to be initialized
 *
 * The purpose of the context is to make it possible to generate an MD5 Message
 * Digest in stages, rather than having to pass a single large block to a single
 * MD5 function. The context structure keeps track of various bits of state
 * information.
 *
 * Once the context is initialized, the blocks of message data are passed to the
 * <md5_update()> function. Once the final bit of data has been handed to
 * <md5_update()> the context can be closed out by calling <md5_final()>, which
 * also calculates the final MD5 result. Don't forget to free an allocated
 * context structure when you've finished using it.
 */
void md5_init(md5_ctx_t *ctx);

/**
 * @brief   Build an MD5 Message Digest within the given context
 *
 * @param[in,out] ctx   Context of the current calculation
 * @param[in] data      Input data
 * @param[in] len       Length of @p data
 */
void md5_update(md5_ctx_t *ctx, const uint8_t *data, size_t len);

/**
 * @brief   Finish up the current MD5 hash calculation generate the final hash
 *
 * @param[in] ctx       Context of the current calculation
 * @param[out] dst      Result location, must be 16 byte
 */
void md5_final(md5_ctx_t *ctx, uint8_t *dst);

/**
 * @brief   Calculate a MD5 hash from the given data
 *
 * @param[out] dst      Result location, must be 16 byte
 * @param[in] src       Input data
 * @param[in] len       Length of @p src
 */
void md5(uint8_t *dst, const uint8_t *src, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* HASHES_MD5_H */
/** @} */
