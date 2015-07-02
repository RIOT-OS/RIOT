/*
 * Copyright (C) 2015 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_ng_pktbuf
 * @{
 *
 * @file
 * @brief   Internal definitions for the packet buffer
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef XPKTBUF_INTERNAL_H_
#define XPKTBUF_INTERNAL_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal alloc on packet buffer
 *
 * @internal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html">
 *          malloc()
 *      </a>
 */
void *_pktbuf_internal_alloc(size_t size);

/**
 * @brief   Internal realloc on static packet buffer
 *
 * @internal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html">
 *          realloc()
 *      </a>
 */
void *_pktbuf_internal_realloc(void *ptr, size_t size);

/**
 * @brief   Adds packet that uses @p ptr for its data part
 *
 * @internal
 */
bool _pktbuf_internal_add_pkt(void *ptr);

/**
 * @brief   Internal free on static packet buffer
 *
 * @internal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html">
 *          free()
 *      </a> but @p ptr can be anywhere in alloced space.
 */
void _pktbuf_internal_free(void *ptr);

/**
 * @brief   Checks if a pointer is part of the static packet buffer.
 *
 * @param[in] ptr   A pointer.
 *
 * @return true, if @p ptr is part of the static packet buffer.
 * @return false, if @p ptr is not part of the static packet buffer.
 */
bool _pktbuf_internal_contains(const void *ptr);

#ifdef DEVELHELP
/**
 * @brief   Prints some statistics about the packet buffer to stdout.
 *
 * @details Statistics include maximum number of reserved bytes.
 */
void _pktbuf_internal_stats(void);
#endif

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  1, if packet buffer is empty
 * @return  0, if packet buffer is not empty
 */
bool _pktbuf_internal_is_empty(void);

/**
 * @brief   Sets the whole packet buffer to 0
 */
void _pktbuf_internal_reset(void);
#endif  /* TEST_SUITES */

#ifdef __cplusplus
}
#endif

#endif /* XPKTBUF_INTERNAL_H_ */
/** @} */
