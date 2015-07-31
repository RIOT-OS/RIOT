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
#include "cpu_conf.h"
#include "net/ng_pkt.h"
#include "net/ng_nettype.h"
#include "utlist.h"

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
 * @brief   Initializes packet buffer module.
 */
void ng_pktbuf_init(void);

/**
 * @brief   Adds a new ng_pktsnip_t and its packet to the packet buffer.
 *
 * @warning **Do not** change the fields of the ng_pktsnip_t created by this
 *          function externally. This will most likely create memory leaks or
 *          not allowed memory access.
 *
 * @param[in] next      Next ng_pktsnip_t in the packet. Leave NULL if you
 *                      want to create a new packet.
 * @param[in] data      Data of the new ng_pktsnip_t. If @p data is NULL no data
 *                      will be inserted into `result`.
 * @param[in] size      Length of @p data. May not be 0.
 * @param[in] type      Protocol type of the ng_pktsnip_t.
 *
 * @return  Pointer to the packet part that represents the new ng_pktsnip_t.
 * @return  NULL, if no space is left in the packet buffer.
 * @return  NULL, if @p size == 0.
 */
ng_pktsnip_t *ng_pktbuf_add(ng_pktsnip_t *next, void *data, size_t size,
                            ng_nettype_t type);

/**
 * @brief   Marks the first @p size bytes in a received packet with a new
 *          packet snip that is appended to the packet.
 *
 *  Graphically this can be represented as follows:
 *  @code
 * Before                                    After
 * ======                                    =====
 *                                                       (next)
 *  pkt->data                                 result->data <== pkt->data
 *  v                                         v                v
 * +--------------------------------+        +----------------+---------------+
 * +--------------------------------+        +----------------+---------------+
 *  \__________pkt->size___________/          \_result->size_/ \__pkt->size__/
 *  @endcode
 *
 * @pre @p pkt != NULL && @p size != 0
 *
 * @param[in] pkt   A received packet.
 * @param[in] size  The size of the new packet snip.
 * @param[in] type  The type of the new packet snip.
 *
 * @return  The new packet snip in @p pkt on success.
 * @return  NULL, if pkt == NULL or size == 0 or size > pkt->size or pkt->data == NULL.
 * @return  NULL, if no space is left in the packet buffer.
 */
ng_pktsnip_t *ng_pktbuf_mark(ng_pktsnip_t *pkt, size_t size, ng_nettype_t type);

/**
 * @brief   Reallocates ng_pktsnip_t::data of @p pkt in the packet buffer, without
 *          changing the content.
 *
 * @pre ng_pktsnip_t::data of @p pkt is in the packet buffer.
 *
 * @details If enough memory is available behind it or @p size is smaller than
 *          the original size of the packet then ng_pktsnip_t::data of @p pkt will
 *          not be moved. Otherwise, it will be moved. If no space is available
 *          nothing happens.
 *
 * @param[in] pkt   A packet part.
 * @param[in] size  The size for @p pkt.
 *
 * @return  0, on success
 * @return  ENOMEM, if no space is left in the packet buffer or size was 0.
 */
int ng_pktbuf_realloc_data(ng_pktsnip_t *pkt, size_t size);

/**
 * @brief   Increases ng_pktsnip_t::users of @p pkt atomically.
 *
 * @param[in] pkt   A packet.
 * @param[in] num   Number you want to increment ng_pktsnip_t::users of @p pkt by.
 */
void ng_pktbuf_hold(ng_pktsnip_t *pkt, unsigned int num);

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
 * @return  NULL, if ng_pktsnip_t::users of @p pkt > 1 and if there is not enough
 *          space in the packet buffer.
 */
ng_pktsnip_t *ng_pktbuf_start_write(ng_pktsnip_t *pkt);

/**
 * @brief   Deletes a snip from a packet and the packet buffer.
 *
 * @param[in] pkt   A packet.
 * @param[in] snip  A snip in the packet.
 *
 * @return  The new reference to @p pkt.
 */
static inline ng_pktsnip_t *ng_pktbuf_remove_snip(ng_pktsnip_t *pkt,
        ng_pktsnip_t *snip)
{
    LL_DELETE(pkt, snip);
    snip->next = NULL;
    ng_pktbuf_release(snip);

    return pkt;
}

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
 * @brief   Checks if the implementation's internal invariants still uphold
 *
 * @return  true, the packet buffer is sane.
 * @return  false, the packet buffer is insane.
 */
bool ng_pktbuf_is_sane(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NG_PKTBUF_H_ */
/** @} */
