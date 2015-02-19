/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_pktbuf   Packet buffer
 * @ingroup     net
 * @brief       A global network packet buffer.
 *
 * @note    **WARNING!!** Do not store data structures that are not packed
 *          (defined with `__attribute__((packed))`) or enforce alignment in
 *          in any way in here if @ref NG_PKTBUF_SIZE > 0. On some RISC architectures
 *          this *will* lead to alignment problems and can potentially result
 *          in segmentation/hard faults and other unexpected behaviour.
 *
 * @{
 *
 * @file
 * @brief   Interface definition for the global network buffer. Network devices
 *          and layers can allocate space for packets here.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_PKTBUF_H_
#define NG_PKTBUF_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "atomic.h"
#include "cpu-conf.h"
#include "net/ng_pkt.h"
#include "net/ng_nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     NG_PKTBUF_SIZE
 * @brief   Maximum size of the static packet buffer.
 *
 * @details The rational here is to have at least space for 4 full-MTU IPv6
 *          packages (2 incoming, 2 outgoing; 2 * 2 * 1280 B = 5 KiB) +
 *          Meta-Data (roughly estimated to 1 KiB; might be smaller). If
 *          @ref NG_PKTBUF_SIZE is 0 the packet buffer will use dynamic memory
 *          management to allocate packets.
 */
#ifndef NG_PKTBUF_SIZE
#define NG_PKTBUF_SIZE  (6144)
#endif  /* NG_PKTBUF_SIZE */

/**
 * @brief   Prepends a new ng_pktsnip_t to a packet.
 *
 * @details It is ill-advised to add a ng_pktsnip_t simply by using
 *
 *     next = ng_pktsnip_add(NULL, NULL, size1, NG_NETTYPE_UNDEF);
 *     pkt = ng_pktsnip_add(NULL, NULL, size2, NG_NETTYPE_UNDEF);
 *
 *     pkt->next = next;
 *     next->data = next->data + size2;
 *
 * Since @p data can be in the range of the data allocated on
 * ng_pktsnip_t::data of @p next, it would be impossible to free
 * ng_pktsnip_t::data of @p next, after @p next was released and the
 * generated ng_pktsnip_t not or vice versa. This function ensures that this
 * can't happen.
 *
 * @param[in] next  The packet you want to add the ng_pktsnip_t to. If
 *                  ng_pktsnip_t::data field of @p next is equal to data it will
 *                  be set to `next->data + size`. If @p next is NULL the
 *                  ng_pktsnip_t::next field of the result will be also set to
 *                  NULL.
 * @param[in] data  Data of the new ng_pktsnip_t. If @p data is NULL no data
 *                  will be inserted into the result.
 * @param[in] size  Length of @p data. If @p size is 0, it will be assumed,
 *                  that @p data is NULL and no data will be inserted into the
 *                  result
 * @param[in] type  Protocol type of the ng_pktsnip_t.
 *
 * @return  Pointer to the packet part that represents the new ng_pktsnip_t.
 * @return  NULL, if no space is left in the packet buffer.
 */
ng_pktsnip_t *ng_pktbuf_add(ng_pktsnip_t *next, void *data, size_t size,
                            ng_nettype_t type);

/**
 * @brief   Reallocates ng_pktsnip_t::data of @p pkt in the packet buffer, without
 *          changing the content.
 *
 * @pre `pkt->users == 1 && pkt->next == NULL` and @p pkt must be in packet buffer
 *
 * @details If enough memory is available behind it or @p size is smaller than
 *          the original size the packet then ng_pktsnip_t::data of @p pkt will
 *          not be moved. Otherwise, it will be moved. If no space is available
 *          nothing happens.
 *
 * @param[in] pkt           A packet part.
 * @param[in] size          The size for @p pkt.
 *
 * @return  0, on success
 * @return  EINVAL, if precondition is not met
 * @return  ENOENT, if ng_pktsnip_t::data of @p pkt was not from the packet buffer.
 * @return  ENOMEM, if no space is left in the packet buffer or size was 0.
 */
int ng_pktbuf_realloc_data(ng_pktsnip_t *pkt, size_t size);

/**
 * @brief   Increases ng_pktsnip_t::users of @p pkt atomically.
 *
 * @param[in] pkt   A packet.
 * @param[in] num   Number you want to increment ng_pktsnip_t::users of @p pkt by.
 */
static inline void ng_pktbuf_hold(ng_pktsnip_t *pkt, unsigned int num)
{
    if (pkt != NULL) {
        atomic_set_return(&(pkt->users), pkt->users + num);
    }
}

/**
 * @brief   Decreases ng_pktsnip_t::users of @p pkt atomically and removes it if it
 *          reaches 0.
 *
 * @param[in] pkt   A packet.
 */
void ng_pktbuf_release(ng_pktsnip_t *pkt);

/**
 * @brief   Must be called once before there is a write operation in a thread.
 *
 * @details This function duplicates a packet in the packet buffer if
 *          ng_pktsnip_t::users of @p pkt > 1.
 *
 * @note    Do *not* call this function in a thread twice on the same packet.
 *
 * @param[in] pkt   The packet you want to write into.
 *
 * @return  The (new) pointer to the pkt.
 * @return  NULL, if ng_pktsnip_t::users of @p pkt > 1 and if there is not anough
 *          space in the packet buffer.
 */
ng_pktsnip_t *ng_pktbuf_start_write(ng_pktsnip_t *pkt);

#ifdef DEVELHELP
/**
 * @brief   Prints some statistics about the packet buffer to stdout.
 *
 * @note    Only available with DEVELHELP defined.
 *
 * @details Statistics include maximum number of reserved bytes.
 */
void ng_pktbuf_stats(void);
#endif

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  true, if packet buffer is empty
 * @return  false, if packet buffer is not empty
 */
bool ng_pktbuf_is_empty(void);

/**
 * @brief   Resets the whole packet buffer
 */
void ng_pktbuf_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NG_PKTBUF_H_ */
/** @} */
