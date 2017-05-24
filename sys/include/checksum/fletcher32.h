/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum_fletcher32 Fletcher32
 * @ingroup     sys_checksum
 *
 * @brief       Fletcher32 checksum algorithm
 * @{
 *
 * @file
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CHECKSUM_FLETCHER32_H
#define CHECKSUM_FLETCHER32_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fletcher's 32 bit checksum
 *
 * found on
 * http://en.wikipedia.org/w/index.php?title=Fletcher%27s_checksum&oldid=661273016#Optimizations
 *
 * @note the returned sum is never 0
 * @note pay attention to alignment issues since this operates on an input
 *       buffer containing 16 bit words, not bytes.
 *
 * @param buf input buffer to hash
 * @param words length of buffer, in 16 bit words
 * @return 32 bit sized hash in the interval [1..2^32]
 */
uint32_t fletcher32(const uint16_t *buf, size_t words);

#ifdef __cplusplus
}
#endif

#endif /* CHECKSUM_FLETCHER32_H */

/** @} */
