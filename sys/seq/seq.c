/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_seq
 * @{
 *
 * @file
 * @brief       Serial Number Arithmetic (RFC 1982)
 *
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * @}
 */

#include <assert.h>
#include "seq.h"

seq8_t seq8_adds(seq8_t s, uint8_t n, uint8_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (n > SEQ_LIMIT(space)) {
        return s;
    }

    return (space == UINT8_MAX) ? (s + n) : ((s + n) & space);
}

int seq8_compares(seq8_t s1, seq8_t s2, uint8_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (s1 == s2) {
        return 0;
    }

    if (((s1 < s2) && ((uint8_t)(s2 - s1) < (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint8_t)(s1 - s2) > (SEQ_LIMIT(space) + 1)))) {
        return -1;
    }

    if (((s1 < s2) && ((uint8_t)(s2 - s1) > (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint8_t)(s1 - s2) < (SEQ_LIMIT(space) + 1)))) {
        return 1;
    }

    return -EINVAL;
}

seq16_t seq16_adds(seq16_t s, uint16_t n, uint16_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (n > SEQ_LIMIT(space)) {
        return s;
    }

    return (space == UINT16_MAX) ? (s + n) : ((s + n) & space);
}

int seq16_compares(seq16_t s1, seq16_t s2, uint16_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (s1 == s2) {
        return 0;
    }

    if (((s1 < s2) && ((uint16_t)(s2 - s1) < (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint16_t)(s1 - s2) > (SEQ_LIMIT(space) + 1)))) {
        return -1;
    }

    if (((s1 < s2) && ((uint16_t)(s2 - s1) > (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint16_t)(s1 - s2) < (SEQ_LIMIT(space) + 1)))) {
        return 1;
    }

    return -EINVAL;
}

seq32_t seq32_adds(seq32_t s, uint32_t n, uint32_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (n > SEQ_LIMIT(space)) {
        return s;
    }

    return (space == UINT32_MAX) ? (s + n) : ((s + n) & space);
}

int seq32_compares(seq32_t s1, seq32_t s2, uint32_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (s1 == s2) {
        return 0;
    }

    if (((s1 < s2) && ((uint32_t)(s2 - s1) < (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint32_t)(s1 - s2) > (SEQ_LIMIT(space) + 1)))) {
        return -1;
    }

    if (((s1 < s2) && ((uint32_t)(s2 - s1) > (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint32_t)(s1 - s2) < (SEQ_LIMIT(space) + 1)))) {
        return 1;
    }

    return -EINVAL;
}

seq64_t seq64_adds(seq64_t s, uint64_t n, uint64_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (n > SEQ_LIMIT(space)) {
        return s;
    }

    return (space == UINT64_MAX) ? (s + n) : ((s + n) & space);
}

int seq64_compares(seq64_t s1, seq64_t s2, uint64_t space)
{
    /* check if space is a power of 2 minus 1 */
    assert((space != 0) && ((space & (space + 1)) == 0));

    if (s1 == s2) {
        return 0;
    }

    if (((s1 < s2) && ((uint64_t)(s2 - s1) < (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint64_t)(s1 - s2) > (SEQ_LIMIT(space) + 1)))) {
        return -1;
    }

    if (((s1 < s2) && ((uint64_t)(s2 - s1) > (SEQ_LIMIT(space) + 1))) ||
            ((s1 > s2) && ((uint64_t)(s1 - s2) < (SEQ_LIMIT(space) + 1)))) {
        return 1;
    }

    return -EINVAL;
}
