/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum_fletcher16 Fletcher16
 * @ingroup     sys_checksum
 *
 * @brief       Fletcher16 checksum algorithm
 * @{
 *
 * @file
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef FLETCHER16_H
#define FLETCHER16_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fletcher's 16 bit checksum
 *
 * found on
 * http://en.wikipedia.org/w/index.php?title=Fletcher%27s_checksum&oldid=661273016#Optimizations
 *
 * @note the returned sum is never 0
 *
 * @param buf input buffer to hash
 * @param bytes length of buffer, in bytes
 * @return 16 bit sized hash in the interval [1..65535]
 */
uint16_t fletcher16(const uint8_t *buf, size_t bytes);


#ifdef __cplusplus
}
#endif

#endif /* FLETCHER16_H */

/** @} */
