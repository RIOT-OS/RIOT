/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @file        seqnum.h
 * @brief       aodvv2 sequence number
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_SEQNUM_H_
#define AODVV2_SEQNUM_H_

#include <stdint.h>

#include "aodvv2/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize sequence number.
 */
void seqnum_init(void);

/**
 * @brief Get sequence number.
 *
 * @return sequence number
 */
aodvv2_seqnum_t seqnum_get(void);

/**
 * @brief Increment the sequence number by 1.
 */
void seqnum_inc(void);

/**
 * @brief Compare 2 sequence numbers.
 * @param[in] s1  first sequence number
 * @param[in] s2  second sequence number
 * @return        -1 when s1 is smaller, 0 if equal, 1 if s1 is bigger.
 */
static inline int seqnum_cmp(aodvv2_seqnum_t s1, aodvv2_seqnum_t s2)
{
    return s1 == s2 ? 0 : (s1 > s2 ? +1 : -1);
}

#ifdef  __cplusplus
}
#endif

#endif /* AODVV2_SEQNUM_H_ */
