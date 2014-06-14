/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup dtls
 * @{
 * @file
 * @brief   common functions for DTLS
 * @author  Jonatan Zint <j.zint@fu-berlin.com>
 * @author  Nico von Geyso <nico.geyso@fu-berlin.com>
 */

#ifndef DTLS_COMMON_H_
#define DTLS_COMMON_H_

/**
 * basic stdint types
 */

typedef struct __attribute__((packed)) {
    uint32_t uint32:24;
} uint24_t;

typedef struct __attribute__((packed)) {
    uint64_t uint64:48;
} uint48_t;

#endif
/**
 * @}
 */
