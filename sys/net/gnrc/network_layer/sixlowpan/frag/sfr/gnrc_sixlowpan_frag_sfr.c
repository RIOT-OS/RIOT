/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stddef.h>

#include "irq.h"
#include "kernel_defines.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/ipv6/addr.h"
#endif
#ifdef MODULE_GNRC_IPV6
#include "net/ipv6/hdr.h"
#endif
#include "net/gnrc/neterr.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/config.h"
#include "net/gnrc/sixlowpan/frag/fb.h"
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "net/gnrc/sixlowpan/frag/vrb.h"
#include "net/sixlowpan/sfr.h"
#include "thread.h"
#include "unaligned.h"
#include "xtimer.h"

#include "net/gnrc/sixlowpan/frag/sfr.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define FRAG_DESCS_POOL_SIZE    (CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE * \
                                 CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE)
#define FRAME_QUEUE_POOL_SIZE   (FRAG_DESCS_POOL_SIZE + \
                                 CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE)

typedef struct {
    clist_node_t super;     /**< list parent instance */
    uint32_t last_sent;     /**< last time sent in microseconds */
    /**
     * @brief   Acknowledgment request flag, sequence number, and fragment size
     */
    uint16_t ar_seq_fs;
    uint16_t offset;        /**< offset of the fragment */
    uint8_t retries;        /**< how often the fragment was retried */
} _frag_desc_t;

typedef struct {
    clist_node_t super;     /**< list parent instance */
    gnrc_pktsnip_t *frame;  /**< frame in the queue */
    uint8_t datagram_tag;   /**< tag for identification */
    uint8_t page;           /**< parsing page context for the frame */
} _frame_queue_t;

typedef struct {
    enum {
        _UNDEF = 0,
        _RB,
        _VRB,
    } type;
    union {
        gnrc_sixlowpan_frag_rb_base_t *base;
        gnrc_sixlowpan_frag_rb_t *rb;
        gnrc_sixlowpan_frag_vrb_t *vrb;
    } entry;
} _generic_rb_entry_t;

#ifdef MODULE_GNRC_IPV6_NIB
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#else   /* MODULE_GNRC_IPV6_NIB */
static char addr_str[GNRC_NETIF_HDR_L2ADDR_PRINT_LEN];
#endif  /* MODULE_GNRC_IPV6_NIB */

static evtimer_msg_t _arq_timer;
static xtimer_t _if_gap_timer = { .offset = 0, .long_offset = 0 };
static msg_t _if_gap_msg = { .type = GNRC_SIXLOWPAN_FRAG_SFR_INTER_FRAG_GAP_MSG };
static uint32_t _last_frame_sent = 0U;

static _frag_desc_t _frag_descs_pool[FRAG_DESCS_POOL_SIZE];
static _frame_queue_t _frame_queue_pool[FRAME_QUEUE_POOL_SIZE];

static clist_node_t _frag_descs_free;
static clist_node_t _frame_queue_free;
static clist_node_t _frame_queue;

static const gnrc_sixlowpan_frag_sfr_bitmap_t _full_bitmap = { .u32 = UINT32_MAX };
static const gnrc_sixlowpan_frag_sfr_bitmap_t _null_bitmap = { .u32 = 0U };

static gnrc_sixlowpan_frag_sfr_stats_t _stats;

/**
 * @brief   Converts a @ref sys_bitmap based bitmap to a
 *          gnrc_sixlowpan_frag_sfr_bitmap_t
 *
 * @param[in] bitmap A @ref sys_bitmap
 *
 * @return  A gnrc_sixlowpan_frag_sfr_bitmap_t.
 */
static inline gnrc_sixlowpan_frag_sfr_bitmap_t *_get_bitmap(gnrc_sixlowpan_frag_rb_t *frag_rb);

/**
 * @brief   Checks if fragment represented by a fragment descriptor requested an
 *          ACK
 */
static inline bool _frag_ack_req(_frag_desc_t *frag);
/**
 * @brief   Returns sequence number for fragment represented by fragment
 *          descriptor
 */
static inline uint8_t _frag_seq(_frag_desc_t *frag);

/**
 * @brief   Returns fragment size for fragment represented by fragment
 *          descriptor
 */
static inline uint16_t _frag_size(_frag_desc_t *frag);

/**
 * @brief   Cleans up a fragmentation buffer entry and all state related to its
 *          datagram.
 *
 * @param[in] fbuf  A fragmentation buffer entry
 * @param[in] error An errno to provide to an upper layer as the reason for why
 *                  gnrc_sixlowpan_frag_fb_t::pkt of @p fbuf was released.
 */
static void _clean_up_fbuf(gnrc_sixlowpan_frag_fb_t *fbuf, int error);

/**
 * @brief   Send first fragment.
 *
 * @param[in] netif         Network interface to send fragment over
 * @param[in] fbuf          Fragmentation buffer for the datagram to fragment
 * @param[in] page          Current 6Lo dispatch parsing page.
 *
 * @return  Size of the fragment
 */
static uint16_t _send_1st_fragment(gnrc_netif_t *netif,
                                   gnrc_sixlowpan_frag_fb_t *fbuf,
                                   unsigned page);

/**
 * @brief   Send subsequent fragment.
 *
 * @param[in] netif         Network interface to send fragment over
 * @param[in] fbuf          Fragmentation buffer for the datagram to fragment
 * @param[in] page          Current 6Lo dispatch parsing page.
 *
 * @return  Size of the fragment
 */
static uint16_t _send_nth_fragment(gnrc_netif_t *netif,
                                   gnrc_sixlowpan_frag_fb_t *fbuf,
                                   unsigned page);

/**
 * @brief   Send a abort pseudo fragment for datagram identified by @p tag
 *
 * @param[in] pkt       Datagram that is to be aborted.
 * @param[in] tag       Tag for @p pkt.
 * @param[in] req_ack   Request ACK for pseudo fragment from receive
 * @param[in] page      Current 6Lo dispatch parsing page.
 *
 * @return  true, if abort pseudo fragment was sent.
 * @return  false, if abort pseudo fragment was unable to be sent.
 */
static bool _send_abort_frag(gnrc_pktsnip_t *pkt, uint8_t tag, bool req_ack,
                             unsigned page);

/**
 * @brief   Re-send a fragment
 *
 * @param[in] node          The fragment descriptor for the fragment to be
 *                          resend
 * @param[in] fbuf_ptr      Fragmentation buffer for the datagram to fragment
 *
 * Used as a `clist_foreach()` iterator function
 *
 * return true  when fragment was resent
 * return false on error
 */
static int _resend_frag(clist_node_t *node, void *fbuf_ptr);

/**
 * @brief   Retry to send the complete datagram
 *
 * @param[in] fbuf  Fragmentation buffer for the datagram
 */
static void _retry_datagram(gnrc_sixlowpan_frag_fb_t *fbuf);

/**
 * @brief   Cleans up state for the causing RFRAG and optionally also sends an
 *          abort ACK (NULL-bitmap ACK).
 *
 * @param[in] pkt       The packet causing the abort. Will be released
 *                      by this function. gnrc_pktsnip_t::data of @p pkt is
 *                      expected to point to an RFRAG packet.
 * @param[in] entry     (Virtual) reassembly buffer entry to abort.
 * @param[in] netif_hdr NETIF header of @p pkt.
 * @param[in] send_ack  Send an abort ACK.
 */
static void _abort_rb(gnrc_pktsnip_t *pkt, _generic_rb_entry_t *entry,
                      gnrc_netif_hdr_t *netif_hdr, bool send_ack);

/**
 * @brief   Sends an RFRAG-ACK
 *
 * @param[in] netif     Network interface to send ACK over
 * @param[in] dst       Destination address of ACK.
 * @param[in] dst_len   Length of @p dst.
 * @param[in] rfrag     The RFRAG to ACK
 * @param[in] bitmap    The bitmap for the ACK.
 */
static void _send_ack(gnrc_netif_t *netif, const uint8_t *dst, uint8_t dst_len,
                      const sixlowpan_sfr_t *rfrag, const uint8_t *bitmap);

/**
 * @brief   Schedule next frame (RFRAG or RFRAG-ACK) with
 *          @ref GNRC_SIXLOWPAN_FRAG_SFR_INTER_FRAG_GAP_MSG
 */
static void _sched_next_frame(void);

/**
 * @brief   Schedule ARQ timeout
 *
 * @param[in] fbuf      A fragmentation buffer holding the state of the datagram
 *                      and recoverable fragments.
 * @param[in] offset    Offset for the ARQ timeout in milliseconds.
 */
static void _sched_arq_timeout(gnrc_sixlowpan_frag_fb_t *fbuf, uint32_t offset);

/**
 * @brief   Schedule ARQ timeout for an abort fragment pseudo fragment
 *
 * @param[in,out] fbuf  A fragmentation buffer. All state information will be
 *                      cleared when called, except for identifying the ACK.
 */
static void _sched_abort_timeout(gnrc_sixlowpan_frag_fb_t *fbuf);

/**
 * @brief   Handle a received RFRAG packet
 *
 * @param[in] netif_hdr NETIF header of @p pkt
 * @param[in] pkt       An RFRAG packet
 * @param[in] page      Current 6Lo dispatch parsing page.
 */
static void _handle_rfrag(gnrc_netif_hdr_t *netif_hdr,
                          gnrc_pktsnip_t *pkt, unsigned page);

/**
 * @brief   Handle a received RFRAG-ACK
 *
 * @param[in] netif_hdr NETIF header of @p pkt
 * @param[in] pkt       An RFRAG-ACK
 * @param[in] page      Current 6Lo dispatch parsing page.
 */
static void _handle_ack(gnrc_netif_hdr_t *netif_hdr,
                        gnrc_pktsnip_t *pkt, unsigned page);

/**
 * @brief   Forward a RFRAG
 *
 * @pre `entry->type == _VRB`
 *
 * @param[in] pkt       The RFRAG to forward (without NETIF header)
 * @param[in] entry     The VRB entry to determine the route
 * @param[in] offset    Offset (from the incoming RFRAG's field) of the RFRAG.
 *                      for offset > 0 this will be adapted for the offset
 *                      difference from the first fragment due to recompression
 * @param[in] page      Current 6Lo dispatch parsing page.
 *
 * @return  0 on success,
 * @return  -ENOMEM, when packet buffer is too full to prepare packet for
 *          forwarding. @p pkt is released in that case.
 */
static int _forward_rfrag(gnrc_pktsnip_t *pkt, _generic_rb_entry_t *entry,
                          uint16_t offset, unsigned page);

/* ====== PUBLIC FUNCTION DEFINITIONS ====== */
void gnrc_sixlowpan_frag_sfr_init(void)
{
    if (CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US > 0) {
        for (unsigned i = 0; i < FRAME_QUEUE_POOL_SIZE; i++) {
            clist_rpush(&_frame_queue_free, &_frame_queue_pool[i].super);
        }
    }
    for (unsigned i = 0; i < FRAG_DESCS_POOL_SIZE; i++) {
        clist_rpush(&_frag_descs_free, &_frag_descs_pool[i].super);
    }
}

void gnrc_sixlowpan_frag_sfr_send(gnrc_pktsnip_t *pkt, void *ctx,
                                  unsigned page)
{
    gnrc_sixlowpan_frag_fb_t *fbuf = ctx;
    gnrc_netif_t *netif;
    int error_no = GNRC_NETERR_SUCCESS;
    uint16_t res;

    assert((fbuf != NULL) && ((fbuf->pkt == pkt) || (pkt == NULL)));
    DEBUG("6lo sfr: (re-)sending fragmented datagram %u\n", fbuf->tag);
    pkt = fbuf->pkt;
    assert(pkt->type == GNRC_NETTYPE_NETIF);
    netif = gnrc_netif_hdr_get_netif(pkt->data);
    assert(netif != NULL);

    if (fbuf->offset == 0) {
        DEBUG("6lo sfr: sending first fragment\n");
        res = _send_1st_fragment(netif, fbuf, page);
        if (res == 0) {
            DEBUG("6lo sfr: error sending first fragment\n");
            /* _send_1st_fragment only returns 0 if there is a memory problem */
            error_no = ENOMEM;
            goto error;
        }
    }
    else if (fbuf->sfr.frags_sent >= fbuf->sfr.window_size) {
        DEBUG("6lo sfr: frags_sent >= fbuf->sfr.window_size: don't send more\n");
        return;
    }
    else if (fbuf->offset < fbuf->datagram_size) {
        DEBUG("6lo sfr: sending subsequent fragment\n");
        res = _send_nth_fragment(netif, fbuf, page);
        if (res == 0) {
            DEBUG("6lo sfr: error sending subsequent fragment (offset = %u)\n",
                  fbuf->offset);
            /* _send_nth_fragment only returns 0 if there is a memory problem */
            error_no = ENOMEM;
            goto error;
        }
    }
    else {
        /* offset is greater or equal to datagram size
         * => we are done sending fragments (not an error, but we can release
         * the fragmentation buffer now) */
        goto error;
    }
    fbuf->offset += res;

    if ((fbuf->sfr.frags_sent < fbuf->sfr.window_size) &&
        (fbuf->offset < fbuf->datagram_size) &&
        !gnrc_sixlowpan_frag_fb_send(fbuf)) {
        /* the queue of the 6LoWPAN thread is full */
        error_no = ENOMEM;
        /* go back offset to not send abort on first fragment */
        fbuf->offset -= res;
        goto error;
    }
    /* check if last fragment sent requested an ACK */
    _frag_desc_t *frag_desc = (_frag_desc_t *)clist_rpeek(&fbuf->sfr.window);
    DEBUG("6lo sfr: last sent fragment (tag: %u, X: %i, seq: %u, "
          "frag_size: %u, offset: %u)\n",
          (uint8_t)fbuf->tag, _frag_ack_req(frag_desc),
          _frag_seq(frag_desc), _frag_size(frag_desc),
          frag_desc->offset);
    if (_frag_ack_req(frag_desc)) {
        /* initialize _arq_timer if not yet done */
        if (_arq_timer.callback == NULL) {
            evtimer_init_msg(&_arq_timer);
        }
        _sched_arq_timeout(fbuf, fbuf->sfr.arq_timeout);
    }
    thread_yield();
    return;
error:
    /* don't send abort for first fragment, the network does not know about
     * the datagram */
    if ((fbuf->offset > 0) &&
        _send_abort_frag(fbuf->pkt, (uint8_t)fbuf->tag, true, 0)) {
        /* wait for ACK before fbuf is deleted */
        _sched_abort_timeout(fbuf);
    }
    else {
        _clean_up_fbuf(fbuf, error_no);
    }
}

void gnrc_sixlowpan_frag_sfr_recv(gnrc_pktsnip_t *pkt, void *ctx,
                                  unsigned page)
{
    sixlowpan_sfr_t *hdr;
    gnrc_netif_hdr_t *netif_hdr;

    (void)ctx;
    DEBUG("6lo sfr: received selective fragment forwarding message\n");
    assert(pkt != NULL);
    hdr = pkt->data;
    assert(pkt->next != NULL);
    netif_hdr = pkt->next->data;
    assert(netif_hdr != NULL);
    if (page != 0) {
        DEBUG("6lo sfr: Invalid page %u\n", page);
        gnrc_pktbuf_release(pkt);
    }
    else if (sixlowpan_sfr_rfrag_is(hdr)) {
        _handle_rfrag(netif_hdr, pkt, page);
    }
    else if (sixlowpan_sfr_ack_is(hdr)) {
        _handle_ack(netif_hdr, pkt, page);
    }
    else {
        DEBUG("6lo sfr: Unknown dispatch: %02x\n",
              hdr->disp_ecn & SIXLOWPAN_SFR_DISP_MASK);
        gnrc_pktbuf_release(pkt);
    }
}

int gnrc_sixlowpan_frag_sfr_forward(gnrc_pktsnip_t *pkt,
                                    sixlowpan_sfr_rfrag_t *rfrag,
                                    gnrc_sixlowpan_frag_vrb_t *vrbe,
                                    unsigned page)
{
    _generic_rb_entry_t entry = { .type = _VRB, .entry = { .vrb = vrbe } };
    gnrc_pktsnip_t *hdrsnip = gnrc_pktbuf_add(pkt, rfrag, sizeof(*rfrag),
                                              GNRC_NETTYPE_SIXLOWPAN);

    /* free all intervals associated to the VRB entry, as we don't need them
     * with SFR, so throw them out, to save this resource */
    while (vrbe->super.ints) {
        vrbe->super.ints->end = 0U;
        vrbe->super.ints = vrbe->super.ints->next;
    }
    if (hdrsnip == NULL) {
        DEBUG("6lo sfr: Unable to allocate new rfrag header\n");
        gnrc_pktbuf_release(pkt);
        return -ENOMEM;
    }
    DEBUG("6lo sfr: adapting old fragment size (%u) for forwarding to %u\n",
          sixlowpan_sfr_rfrag_get_frag_size(hdrsnip->data),
          (unsigned)gnrc_pkt_len(pkt));
    /* due to compression, packet length of the original fragment might have
     * changed */
    sixlowpan_sfr_rfrag_set_frag_size(hdrsnip->data, gnrc_pkt_len(pkt));
    /* offset is adapted in `_forward_rfrag()` */
    return _forward_rfrag(hdrsnip, &entry, sixlowpan_sfr_rfrag_get_offset(rfrag),
                          page);
}

void gnrc_sixlowpan_frag_sfr_arq_timeout(gnrc_sixlowpan_frag_fb_t *fbuf)
{
    uint32_t now = xtimer_now_usec();
    _frag_desc_t * const head = (_frag_desc_t *)fbuf->sfr.window.next;
    _frag_desc_t *frag_desc = head;
    uint32_t next_arq_offset = fbuf->sfr.arq_timeout;
    bool reschedule_arq_timeout = false;
    int error_no = ETIMEDOUT;   /* assume time out for fbuf->pkt */

    DEBUG("6lo sfr: ARQ timeout for datagram %u\n", fbuf->tag);
    fbuf->sfr.arq_timeout_event.msg.content.ptr = NULL;
    /* copying clist_foreach because we can't work just in function context */
    if (frag_desc) {
        do {
            uint32_t diff;

            frag_desc = (_frag_desc_t *)frag_desc->super.next;
            diff = now - frag_desc->last_sent;
            if (diff < (fbuf->sfr.arq_timeout * US_PER_MS)) {
                /* this fragment's last was last sent < fbuf->sfr.arq_timeout
                 * ago */
                uint32_t offset = fbuf->sfr.arq_timeout - (diff / US_PER_MS);

                DEBUG("6lo sfr: wait for fragment %u in next reschedule\n",
                      _frag_seq(frag_desc));
                if (offset < next_arq_offset) {
                    /* wait for this fragments ACK next */
                    next_arq_offset = offset;
                    DEBUG("         (next ARQ timeout in %lu)\n",
                          (long unsigned)next_arq_offset);
                }
                /* this fragment is still waiting for its ACK,
                 * reschedule the next ACK timeout to the difference
                 * of the ACK timeout and the time of its last send */
                reschedule_arq_timeout = true;
            }
            else if (_frag_ack_req(frag_desc)) {
                /* for this fragment we requested an ACK which was not received
                 * yet. Try to resend it */
                if ((frag_desc->retries++) < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES) {
                    /* we have retries left for this fragment */
                    DEBUG("6lo sfr: %u retries left for fragment (tag: %u, "
                          "X: %i, seq: %u, frag_size: %u, offset: %u)\n",
                          CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES -
                          (frag_desc->retries - 1), (uint8_t)fbuf->tag,
                          _frag_ack_req(frag_desc), _frag_seq(frag_desc),
                          _frag_size(frag_desc), frag_desc->offset);
                    if (_resend_frag(&frag_desc->super, fbuf) != 0) {
                        /* _resend_frag failed due to a memory resource
                         * problem */
                        error_no = ENOMEM;
                        goto error;
                    }
                    else if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
                        /* fragment was resent successfully, note this done in
                         * the statistics */
                        _stats.fragment_resends.by_timeout++;
                    }
                    /* fragment was resent successfully, schedule next ACK
                     * timeout */
                    reschedule_arq_timeout = true;
                }
                else {
                    /* out of retries */
                    DEBUG("6lo sfr: no retries left for fragment "
                          "(tag: %u, X: %i, seq: %u, frag_size: %u, "
                          "offset: %u)\n",
                          (uint8_t)fbuf->tag, _frag_ack_req(frag_desc),
                          _frag_seq(frag_desc), _frag_size(frag_desc),
                          frag_desc->offset);
                    /* we are out of retries on the fragment level, but we
                     * might be able to retry the datagram if retries for the
                     * datagram are configured. */
                    _retry_datagram(fbuf);
                    return;
                }
            }
            else {
                /* Do not resend fragments that were not explicitly asking for
                 * an ACK from the reassembling endpoint on ACK timeout.
                 * If this is true for all fragments remaining in the fragment
                 * buffer, the datagram is to be considered timed out, so
                 * error_no should remain ETIMEDOUT */
                DEBUG("6lo sfr: nothing to do for fragment %u\n",
                      _frag_seq(frag_desc));
            }
        } while (frag_desc != head);
    }
    else {
        /* No fragments to resend, we can assume the packet was delivered
         * successfully */
        error_no = GNRC_NETERR_SUCCESS;
    }
    assert(fbuf->sfr.frags_sent == clist_count(&fbuf->sfr.window));
    if (reschedule_arq_timeout) {
        _sched_arq_timeout(fbuf, next_arq_offset);
        return;
    }
error:
    /* don't check return value, as we don't want to wait for an ACK again ;-) */
    _send_abort_frag(fbuf->pkt, (uint8_t)fbuf->tag, false, 0);
    _clean_up_fbuf(fbuf, error_no);
}

void gnrc_sixlowpan_frag_sfr_inter_frame_gap(void)
{
    if (CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US > 0) {
        _frame_queue_t *node = (_frame_queue_t *)clist_lpop(&_frame_queue);

        if (node != NULL) {
            _last_frame_sent = xtimer_now_usec();
            gnrc_sixlowpan_dispatch_send(node->frame, NULL, node->page);
            /* unset packet just to be safe */
            node->frame = NULL;
            clist_rpush(&_frame_queue_free, &node->super);
        }
        if (clist_lpeek(&_frame_queue) != NULL) {
            _sched_next_frame();
        }
    }
}

void gnrc_sixlowpan_frag_sfr_stats_get(gnrc_sixlowpan_frag_sfr_stats_t *stats)
{
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
        *stats = _stats;
    }
}

/* ====== INTERNAL FUNCTION DEFINITIONS ====== */
static inline uint16_t _min(uint16_t a, size_t b)
{
    return (a < b) ? a : (uint16_t)b;
}

static inline kernel_pid_t _getpid(void)
{
    /* in production, only the 6LoWPAN thread is supposed to call the API
     * functions, so just get the current thread's PID for sending messages.
     * When testing, those functions might however be called by the testing
     * thread (usually the main thread), so indirect over the 6LoWPAN thread in
     * that case */
    return IS_ACTIVE(TEST_SUITES) ? gnrc_sixlowpan_get_pid() : thread_getpid();
}

/*
 * @brief   Returns the datagram in @p fbuf to its original state
 *
 * This function can be both used to clean up the fragmentation buffer on
 * failure without releasing @p fbuf's gnrc_sixlowpan_frag_fb_t::pkt and to
 * reset a datagram for a datagram retry.
 *
 * @param[in]   fbuf    The fragmentation buffer entry to clean up
 */
static void _clean_slate_datagram(gnrc_sixlowpan_frag_fb_t *fbuf)
{
    clist_node_t new_queue = { .next = NULL };

    fbuf->sfr.arq_timeout_event.msg.content.ptr = NULL;
    /* remove potentially scheduled timers for this datagram */
    evtimer_del((evtimer_t *)(&_arq_timer),
                &fbuf->sfr.arq_timeout_event.event);
    fbuf->sfr.arq_timeout_event.event.next = NULL;
    if (CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US > 0) {
        for (clist_node_t *node = clist_lpop(&_frame_queue);
             node != NULL; node = clist_lpop(&_frame_queue)) {
            _frame_queue_t *entry = (_frame_queue_t *)node;
            /* remove frames of this datagram from frame queue */
            if (entry->datagram_tag == fbuf->tag) {
                gnrc_pktbuf_release(entry->frame);
                /* unset packet just to be safe */
                entry->frame = NULL;
                clist_rpush(&_frag_descs_free, node);
            }
            else {
                clist_rpush(&new_queue, node);
            }
        }
        /* reset frame queue with remaining frames */
        _frame_queue = new_queue;
    }
    fbuf->offset = 0U;
    fbuf->sfr.cur_seq = 0U;
    fbuf->sfr.frags_sent = 0U;
    fbuf->sfr.window_size = CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE;
    for (clist_node_t *node = clist_lpop(&fbuf->sfr.window);
         node != NULL; node = clist_lpop(&fbuf->sfr.window)) {
        clist_rpush(&_frag_descs_free, node);
    }
}

static gnrc_pktsnip_t *_build_rfrag(uint8_t tag, bool ack_req, uint16_t size,
                                    uint8_t seq)
{
    sixlowpan_sfr_rfrag_t *hdr;
    gnrc_pktsnip_t *frag = gnrc_pktbuf_add(NULL, NULL,
                                           sizeof(sixlowpan_sfr_rfrag_t) +
                                           size, GNRC_NETTYPE_SIXLOWPAN);

    if (frag == NULL) {
        return NULL;
    }
    sixlowpan_sfr_rfrag_set_disp(frag->data);

    hdr = frag->data;
    hdr->base.tag = tag;
    if (ack_req) {
        sixlowpan_sfr_rfrag_set_ack_req(hdr);
    }
    else {
        sixlowpan_sfr_rfrag_clear_ack_req(hdr);
    }
    sixlowpan_sfr_rfrag_set_frag_size(hdr, size);
    sixlowpan_sfr_rfrag_set_seq(hdr, seq);
    /* set offset / datagram_size in callers */
    return frag;
}

static gnrc_pktsnip_t *_build_frag_pkt(gnrc_netif_hdr_t *old_netif_hdr,
                                       uint8_t tag, bool ack_req, uint16_t size,
                                       uint8_t seq)
{
    gnrc_netif_hdr_t *new_netif_hdr;
    gnrc_pktsnip_t *netif, *res;

    DEBUG("6lo sfr: building fragment (tag: %u, X: %i, seq: %u, frag_size: %u)\n",
           tag, ack_req, seq, size);
    netif = gnrc_netif_hdr_build(gnrc_netif_hdr_get_src_addr(old_netif_hdr),
                                 old_netif_hdr->src_l2addr_len,
                                 gnrc_netif_hdr_get_dst_addr(old_netif_hdr),
                                 old_netif_hdr->dst_l2addr_len);
    if (netif == NULL) {
        return NULL;
    }

    new_netif_hdr = netif->data;
    *new_netif_hdr = *old_netif_hdr;
    res = _build_rfrag(tag, ack_req, size, seq);
    if (res == NULL) {
        gnrc_pktbuf_release(netif);
        return NULL;
    }
    return gnrc_pkt_prepend(res, netif);
}

static gnrc_pktsnip_t *_build_frag_from_fbuf(gnrc_pktsnip_t *pkt,
                                             gnrc_sixlowpan_frag_fb_t *fbuf,
                                             uint16_t frag_size)
{
    return _build_frag_pkt(pkt->data, (uint8_t)fbuf->tag,
                           ((frag_size + fbuf->offset) >= fbuf->datagram_size) ||
                           ((fbuf->sfr.frags_sent + 1) >= fbuf->sfr.window_size),
                           frag_size, fbuf->sfr.cur_seq);
}

static uint16_t _copy_pkt_to_frag(uint8_t *data, const gnrc_pktsnip_t *pkt,
                                  uint16_t frag_size, uint16_t init_offset)
{
    uint16_t offset = init_offset;

    while ((pkt != NULL) && (offset < frag_size)) {
        uint16_t len = _min(frag_size - offset, pkt->size);

        memcpy(data + offset, pkt->data, len);

        offset += len;
        pkt = pkt->next;
    }
    return offset;
}

static uint16_t _find_offset_and_copy_rest(uint8_t *data, gnrc_pktsnip_t **pkt,
                                           uint16_t frag_size,
                                           uint16_t offset)
{
    uint16_t offset_count = 0, cur_frag_size = 0;
    while ((*pkt != NULL) && (offset_count != offset)) {     /* go to offset */
        uint16_t pkt_size = (uint16_t)(*pkt)->size;
        offset_count += pkt_size;

        if (offset_count > offset) {                        /* we overshot */
            /* => copy rest of partly send packet snip */
            uint16_t pkt_offset = offset - (offset_count - pkt_size);
            size_t clen = _min(frag_size, pkt_size - pkt_offset);

            memcpy(data, ((uint8_t *)(*pkt)->data) + pkt_offset, clen);
            cur_frag_size = clen;
            *pkt = (*pkt)->next;
            break;
        }
        *pkt = (*pkt)->next;
    }
    return cur_frag_size;
}

static bool _send_frame(gnrc_pktsnip_t *frame, void *ctx, unsigned page)
{
    uint32_t now = xtimer_now_usec();

    if ((CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US == 0) ||
        ((now - _last_frame_sent) > CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US)) {
        DEBUG("6lo sfr: dispatch frame to network interface\n");
        _last_frame_sent = now;
        gnrc_sixlowpan_dispatch_send(frame, ctx, page);
        return true;
    }
    else {
        _frame_queue_t *node = (_frame_queue_t *)clist_lpop(&_frame_queue_free);

        if (node != NULL) {
            sixlowpan_sfr_t *hdr = frame->next->data;

            assert(sixlowpan_sfr_is(hdr));
            node->frame = frame;
            node->datagram_tag = hdr->tag;
            node->page = page;
            clist_rpush(&_frame_queue, &node->super);
            _sched_next_frame();
        }
        return (node != NULL);
    }
}

static bool _send_fragment(gnrc_pktsnip_t *frag, gnrc_sixlowpan_frag_fb_t *fbuf,
                           unsigned page, uint16_t offset)
{
    sixlowpan_sfr_rfrag_t *hdr = frag->next->data;
    _frag_desc_t *frag_desc = (_frag_desc_t *)clist_lpop(&_frag_descs_free);
    bool res;

    if (frag_desc == NULL) {
        DEBUG("6lo sfr: could not remember fragment to send\n");
        gnrc_pktbuf_release(frag);
        return false;
    }
    frag_desc->ar_seq_fs = byteorder_ntohs(hdr->ar_seq_fs);
    frag_desc->offset = offset;
    frag_desc->retries = 0;
    clist_rpush(&fbuf->sfr.window, &frag_desc->super);
    if ((res = _send_frame(frag, NULL, page))) {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
            _stats.fragments_sent.usual++;
        }
        frag_desc->last_sent = _last_frame_sent;
        fbuf->sfr.cur_seq++;
        fbuf->sfr.frags_sent++;
    }
    return res;
}

static gnrc_pktsnip_t *_build_ack(gnrc_netif_t *netif,
                                  const uint8_t *dst, uint8_t dst_len,
                                  const sixlowpan_sfr_t *hdr,
                                  const uint8_t *bitmap)
{
    gnrc_pktsnip_t *ack_snip, *ack_netif;
    sixlowpan_sfr_ack_t *ack;

    ack_netif = gnrc_netif_hdr_build(NULL, 0, dst, dst_len);
    if (ack_netif == NULL) {
        DEBUG("6lo sfr: can't allocate netif header for ACK for (%s, %02x).\n",
              gnrc_netif_addr_to_str(dst, dst_len, addr_str), hdr->tag);
        return NULL;
    }
    gnrc_netif_hdr_set_netif(ack_netif->data, netif);

    ack_snip = gnrc_pktbuf_add(NULL, NULL, sizeof(sixlowpan_sfr_ack_t),
                               GNRC_NETTYPE_SIXLOWPAN);

    if (ack_snip == NULL) {
        DEBUG("6lo sfr: can't allocate ACK for (%s, %02x).\n",
              gnrc_netif_addr_to_str(dst, dst_len, addr_str), hdr->tag);
        gnrc_pktbuf_release(ack_netif);
        return NULL;
    }
    ack = ack_snip->data;
    /* https://tools.ietf.org/html/rfc8931#section-6:
     * The Datagram_Tag in the RFRAG_ACK is unique to the reassembling endpoint
     * and is enough information for an intermediate hop to locate the VRB that
     * contains the Datagram_Tag used by the previous hop and the Layer-2
     * information associated with it (interface and Link-Layer address)..
     * [...] The reassembling endpoint of a fragment with the 'E' (ECN) flag set
     * MUST echo that information at most once by setting the 'E' (ECN) flag in
     * the next RFRAG_ACK.
     *
     * => base except dispatch are the same as ack'd RFRAG.
     */
    ack->base = *hdr;
    sixlowpan_sfr_ack_set_disp(&ack->base);
    memcpy(ack->bitmap, bitmap, sizeof(ack->bitmap));
    ack_netif->next = ack_snip;
    return ack_netif;
}

static void _clean_up_rb_entry(_generic_rb_entry_t *entry)
{
    if (entry != NULL) {
        switch (entry->type) {
            case _RB:
                gnrc_pktbuf_release(entry->entry.rb->pkt);
                gnrc_sixlowpan_frag_rb_remove(entry->entry.rb);
                break;
            case _VRB:
                gnrc_sixlowpan_frag_vrb_rm(entry->entry.vrb);
                break;
            default:
                break;
        }
    }
}

static void _try_reassembly(gnrc_netif_hdr_t *netif_hdr,
                            gnrc_pktsnip_t *rfrag, unsigned offset,
                            _generic_rb_entry_t *entry,
                            unsigned page)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    const gnrc_sixlowpan_frag_sfr_bitmap_t *bitmap;
    sixlowpan_sfr_rfrag_t *hdr = rfrag->data;
    gnrc_pktsnip_t *netif_snip = rfrag->next;
    /* copy base for ACK */
    sixlowpan_sfr_t base = hdr->base;
    int8_t ack_req = sixlowpan_sfr_rfrag_ack_req(hdr);
    uint8_t seq = sixlowpan_sfr_rfrag_get_seq(hdr);

    assert(netif_snip->data == netif_hdr);
    gnrc_pktbuf_hold(netif_snip, 1);    /* hold netif header to use it with
                                         * dispatch_when_complete()
                                         * (rb_add() releases `pkt`) */
    entry->entry.rb = gnrc_sixlowpan_frag_rb_add(netif_hdr, rfrag,
                                                 offset, page);
    /* check if VRB entry was created */
    vrbe = gnrc_sixlowpan_frag_vrb_get(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                       netif_hdr->src_l2addr_len, base.tag);
    if ((entry->entry.rb == NULL) && (vrbe == NULL)) {
        DEBUG("6lo sfr: can't allocate reassembly buffer or forward compressed "
              "fragment\n");
        /* send abort */
        bitmap = &_null_bitmap;
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
            _stats.acks.aborts++;
        }
    }
    else if (vrbe != NULL) {
        DEBUG("6lo sfr: packet was forwarded\n");
        goto end;
    }
    else {
        int res;

        DEBUG("6lo sfr: reassembling datagram (%s, %u)\n",
              gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                     netif_hdr->src_l2addr_len, addr_str),
              base.tag);
        entry->type = _RB;
        bf_set(entry->entry.rb->received, seq);
        if ((res = gnrc_sixlowpan_frag_rb_dispatch_when_complete(entry->entry.rb,
                                                                 netif_hdr)) < 0) {
            DEBUG("6lo sfr: can not dispatch datagram to upper layer\n");
            _clean_up_rb_entry(entry);
            /* send abort */
            bitmap = &_null_bitmap;
            if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
                _stats.acks.aborts++;
            }
        }
        else {
            if (res) {
                DEBUG("6lo sfr: dispatched datagram to upper layer\n");
                bitmap = &_full_bitmap;
                if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
                    _stats.acks.full++;
                }
            }
            else if (ack_req) {
                DEBUG("6lo sfr: ACKing received fragments %02X%02X%02X%02X "
                      "(%u of %u bytes received)\n",
                      entry->entry.rb->received[0],
                      entry->entry.rb->received[1],
                      entry->entry.rb->received[2],
                      entry->entry.rb->received[3],
                      entry->entry.base->current_size,
                      entry->entry.base->datagram_size);
                bitmap = _get_bitmap(entry->entry.rb);
                if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
                    _stats.acks.partly++;
                }
            }
            else {
                /* no ACK was requested and no error was causing an abort ACK*/
                DEBUG("6lo sfr: no ACK requested by received fragment %u "
                      "(bitmap so far: %02X%02X%02X%02X)\n", seq,
                      entry->entry.rb->received[0],
                      entry->entry.rb->received[1],
                      entry->entry.rb->received[2],
                      entry->entry.rb->received[3]);
                goto end;
            }
        }
    }
    _send_ack(gnrc_netif_hdr_get_netif(netif_hdr),
              gnrc_netif_hdr_get_src_addr(netif_hdr),
              netif_hdr->src_l2addr_len, &base, bitmap->bf);
end:
    gnrc_pktbuf_release(netif_snip);    /* release hold */
}

static void _forward_uncomp(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                            unsigned page, _generic_rb_entry_t *entry,
                            void *payload)
{
    sixlowpan_sfr_rfrag_t *hdr = pkt->data;
    gnrc_sixlowpan_frag_rb_base_t vrb_base = {
        .src_len = netif_hdr->src_l2addr_len,
        .tag = hdr->base.tag,
        .datagram_size = sixlowpan_sfr_rfrag_get_offset(hdr),
    };
    gnrc_pktsnip_t tmp = {
        .data = payload,
        .size = pkt->size - sizeof(sixlowpan_sfr_rfrag_t) - 1,
        .users = 1,
    };

    switch (page) {
#if defined(MODULE_GNRC_IPV6)
        case 0: {
            ipv6_hdr_t *ipv6_hdr = tmp.data;

            if (ipv6_hdr->hl <= 1) {
                DEBUG("6lo sfr: minimal hop-limit reached\n");
                /* try to reassemble to hand to IPv6 module for error
                 * handling */
                _try_reassembly(netif_hdr, pkt, 0, entry, page);
                return;
            }
            tmp.type = GNRC_NETTYPE_IPV6;
            break;
        }
#endif
        default:
            tmp.type = GNRC_NETTYPE_UNDEF;
            break;
    }
    vrb_base.arrival = xtimer_now_usec();
    memcpy(vrb_base.src, gnrc_netif_hdr_get_src_addr(netif_hdr),
           vrb_base.src_len);
    entry->entry.vrb = gnrc_sixlowpan_frag_vrb_from_route(&vrb_base,
                                                          NULL, &tmp);
    if (entry->entry.vrb == NULL) {
        DEBUG("6lo sfr: no route found or no VRB space left, "
              "trying reassembly\n");
        _try_reassembly(netif_hdr, pkt, 0, entry, page);
        return;
    }
    /* only decrement hop-limit after check in case we reassemble */
    switch (page) {
#if defined(MODULE_GNRC_IPV6)
        case 0: {
            ipv6_hdr_t *ipv6_hdr = tmp.data;

            ipv6_hdr->hl--;
            break;
        }
#endif
        default:
            break;
    }
    entry->type = _VRB;
    entry->entry.vrb->in_netif = gnrc_netif_hdr_get_netif(netif_hdr);
    entry->entry.vrb->offset_diff = 0; /* packet is uncompressed so offset
                                        * does not change */
    _forward_rfrag(pkt, entry, sixlowpan_sfr_rfrag_get_offset(hdr), page);
}

static void _handle_1st_rfrag(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                              unsigned page, _generic_rb_entry_t *entry)
{
    sixlowpan_sfr_rfrag_t *hdr = pkt->data;
    /* https://tools.ietf.org/html/rfc8931#section-5.1:
     *   +  For a first fragment (i.e. with a Sequence of 0), this field
     *      indicates the datagram_size of the compressed datagram, [...] */
    uint16_t datagram_size = sixlowpan_sfr_rfrag_get_offset(hdr);
    uint8_t fragment_size = sixlowpan_sfr_rfrag_get_frag_size(hdr);
    uint8_t *payload;

    if ((datagram_size == 0) && (fragment_size == 0)) {
        /* the received fragment is a pseudo-fragment that signals an abort
         * condition by the fragmenting end-point, release state on the
         * datagram */
        bool release_pkt = true;

        DEBUG("6lo sfr: Abort for datagram (%s, %u) received\n",
              gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                     netif_hdr->src_l2addr_len, addr_str),
              hdr->base.tag);
        if ((entry->entry.vrb = gnrc_sixlowpan_frag_vrb_get(
                gnrc_netif_hdr_get_src_addr(netif_hdr),
                netif_hdr->src_l2addr_len, hdr->base.tag)) != NULL) {
            /* we have a VRB on the aborted datagram. Release it */
            entry->type = _VRB;
            _forward_rfrag(pkt, entry, 0, page);
            gnrc_sixlowpan_frag_vrb_rm(entry->entry.vrb);
            release_pkt = false;
        }
        if ((entry->entry.rb = gnrc_sixlowpan_frag_rb_get_by_datagram(
                netif_hdr, hdr->base.tag)) != NULL) {
            /* we have a reassembly buffer entry on the aborted datagram.
             * Release it */
            entry->type = _RB;
            _abort_rb(pkt, entry, netif_hdr, sixlowpan_sfr_rfrag_ack_req(hdr));
            release_pkt = false;
        }
        if (release_pkt) {
            DEBUG("6lo sfr: received abort for unknown datagram\n");
            /* neither VRB or RB exists so we don't have any state on the
             * aborted datagram left. Just release the abort pseudo fragment */
            gnrc_pktbuf_release(pkt);
        }
        return;
    }
    DEBUG("6lo sfr: First fragment (%s, %u) received\n",
          gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                 netif_hdr->src_l2addr_len, addr_str),
          hdr->base.tag);

    payload = (uint8_t *)(hdr + 1);
    if (payload[0] == SIXLOWPAN_UNCOMP) {
        _forward_uncomp(netif_hdr, pkt, page, entry, payload + 1);
    }
    else {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_IPHC) &&
            sixlowpan_iphc_is(payload)) {
            _try_reassembly(netif_hdr, pkt, 0, entry, page);
            return;
        }
        DEBUG("6lo sfr: unable to parse next dispatch for forwarding "
              "information. Abort\n");
        _abort_rb(pkt, entry, netif_hdr, false);
        return;
    }
}

static void _handle_nth_rfrag(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                              unsigned page, _generic_rb_entry_t *entry)
{
    sixlowpan_sfr_rfrag_t *hdr = pkt->data;
    uint16_t offset = sixlowpan_sfr_rfrag_get_offset(hdr);

    DEBUG("6lo sfr: Subsequent fragment (%s, %u) received\n",
          gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                 netif_hdr->src_l2addr_len, addr_str),
          hdr->base.tag);
    if (gnrc_sixlowpan_frag_rb_exists(netif_hdr, hdr->base.tag)) {
        DEBUG("6lo sfr: I am destination endpoint => adding to reassembly "
              "buffer\n");
        _try_reassembly(netif_hdr, pkt, offset, entry, page);
    }
    else if ((entry->entry.vrb = gnrc_sixlowpan_frag_vrb_get(
            gnrc_netif_hdr_get_src_addr(netif_hdr),
            netif_hdr->src_l2addr_len, hdr->base.tag)) != NULL) {
        entry->type = _VRB;
        entry->entry.base->arrival = xtimer_now_usec();
        _forward_rfrag(pkt, entry, offset, page);
    }
    else {
        DEBUG("6lo sfr: neither VRB nor RB found\n");
        /* always send abort ACK:
         * https://tools.ietf.org/html/rfc8931#section-6.1.2 */
        _abort_rb(pkt, entry, netif_hdr, true);
    }
}

static int _resend_failed_frag(clist_node_t *node, void *fbuf_ptr)
{
    int res;

    if (((res = _resend_frag(node, fbuf_ptr)) == 0) &&
        IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
        _stats.fragment_resends.by_nack++;
    }
    return res;
}

static void _check_failed_frags(sixlowpan_sfr_ack_t *ack,
                                gnrc_sixlowpan_frag_fb_t *fbuf)
{
    _frag_desc_t *frag_desc;
    clist_node_t not_received = { .next = NULL };

    DEBUG("6lo sfr: checking which fragments to resend for datagram %u\n",
          fbuf->tag);
    for (frag_desc = (_frag_desc_t *)clist_lpop(&fbuf->sfr.window);
         frag_desc != NULL;
         frag_desc = (_frag_desc_t *)clist_lpop(&fbuf->sfr.window)) {
        uint8_t seq;

        seq = _frag_seq(frag_desc);
        if (bf_isset(ack->bitmap, seq)) {
            DEBUG("6lo sfr: fragment %u (offset: %u, frag_size: %u) "
                  "for datagram %u was received\n", seq,
                  frag_desc->offset, _frag_size(frag_desc), fbuf->tag);
            fbuf->sfr.frags_sent--;
            clist_rpush(&_frag_descs_free, &frag_desc->super);
        }
        else {
            DEBUG("6lo sfr: fragment %u (offset: %u, frag_size: %u) "
                  "for datagram %u was not received\n", seq,
                  frag_desc->offset, _frag_size(frag_desc), fbuf->tag);
            if ((frag_desc->retries++) < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES) {
                DEBUG("6lo sfr: %u retries left\n",
                      CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES -
                      (frag_desc->retries - 1));
                /* put fragment in "not received" list */
                clist_rpush(&not_received, &frag_desc->super);
                frag_desc->ar_seq_fs &= ~(SIXLOWPAN_SFR_ACK_REQ << 8U);
            }
            else {
                DEBUG("6lo sfr: no more retries for fragment %u\n", seq);
                clist_rpush(&_frag_descs_free, &frag_desc->super);
                /* retry to resend whole datagram */
                _retry_datagram(fbuf);
                return;
            }
        }
    }
    /* all fragments were received of the current window were received and
     * the datagram was transmitted completely */
    if ((clist_lpeek(&not_received) == NULL) &&
        (fbuf->offset == fbuf->datagram_size)) {
        /* release fragmentation buffer */
        _clean_up_fbuf(fbuf, GNRC_NETERR_SUCCESS);
    }
    /* at least one fragment was not received */
    else {
        fbuf->sfr.window = not_received;
        assert(fbuf->sfr.frags_sent == clist_count(&fbuf->sfr.window));
        /* use _resend_failed_frag here instead of loop above, so
         * _resend_frag can know if the fragment is the last in the window by
         * using clist_rpeek() on fbuf->sfr.window */
        if (clist_foreach(&fbuf->sfr.window, _resend_failed_frag, fbuf) != NULL) {
            /* XXX: it is unlikely that allocating an abort RFRAG will be
             * successful since the resources missing to cause the abort are
             * still in use, but we should at least try */
            if (_send_abort_frag(fbuf->pkt, (uint8_t)fbuf->tag, true, 0)) {
                /* wait for ACK before fbuf is deleted */
                _sched_abort_timeout(fbuf);
            }
            else {
                /* we have no memory resources left to send neither the
                 * resent fragment nor the abort ACK to signalize that fact to
                 * the reassembling endpoint */
                _clean_up_fbuf(fbuf, ENOMEM);
            }
        }
        if ((fbuf->sfr.frags_sent < fbuf->sfr.window_size) &&
            (fbuf->offset < fbuf->datagram_size)) {
            DEBUG("6lo sfr: trigger send of further fragments of datagram %u\n",
                  fbuf->tag);
            gnrc_sixlowpan_frag_fb_send(fbuf);
        }
    }
}

/* ====== INTERNAL FUNCTIONS USED BY PUBLIC FUNCTIONS ======
 * ====== AND TO MANIPULATE INTERNAL DATA STRUCTURES  ====== */
static inline gnrc_sixlowpan_frag_sfr_bitmap_t *_get_bitmap(gnrc_sixlowpan_frag_rb_t *frag_rb)
{
    DECLARE_CONSTANT(is_aligned,
        HAS_ALIGNMENT_OF(offsetof(gnrc_sixlowpan_frag_rb_t, received),
                         alignof(gnrc_sixlowpan_frag_sfr_bitmap_t)))
    static_assert(is_aligned,
                  "gnrc_sixlowpan_frag_rb_t::received must be suitably aligned");
    return (gnrc_sixlowpan_frag_sfr_bitmap_t *)(uintptr_t)frag_rb->received;
}

static inline bool _frag_ack_req(_frag_desc_t *frag)
{

    return (frag->ar_seq_fs & (SIXLOWPAN_SFR_ACK_REQ << 8U));
}

static inline uint8_t _frag_seq(_frag_desc_t *frag)
{
    return (frag->ar_seq_fs & (SIXLOWPAN_SFR_SEQ_MASK << 8U)) >>
           (SIXLOWPAN_SFR_SEQ_POS + 8U);
}

static inline uint16_t _frag_size(_frag_desc_t *frag)
{
    return (frag->ar_seq_fs & SIXLOWPAN_SFR_FRAG_SIZE_MASK);
}

static void _clean_up_fbuf(gnrc_sixlowpan_frag_fb_t *fbuf, int error)
{
    DEBUG("6lo sfr: removing fragmentation buffer entry for datagram %u\n",
          fbuf->tag);
    _clean_slate_datagram(fbuf);
    gnrc_pktbuf_release_error(fbuf->pkt, error);
    fbuf->pkt = NULL;
}

static uint16_t _send_1st_fragment(gnrc_netif_t *netif,
                                   gnrc_sixlowpan_frag_fb_t *fbuf,
                                   unsigned page)
{
    gnrc_pktsnip_t *frag, *pkt = fbuf->pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    uint8_t *data;
    size_t comp_form_size = gnrc_pkt_len(pkt->next);
    uint16_t frag_size = (uint16_t)netif->sixlo.max_frag_size -
                         sizeof(sixlowpan_sfr_rfrag_t);

    assert((fbuf->sfr.cur_seq == 0) && (fbuf->sfr.frags_sent == 0));
    assert(fbuf->sfr.window.next == NULL);
    assert(comp_form_size <= UINT16_MAX);
    /* restrict tag to value space of SFR, so that later RFRAG ACK can find
     * it in reverse look-up */
    fbuf->tag &= UINT8_MAX;
    DEBUG("6lo sfr: determined frag_size = %u\n", frag_size);

    /* packet was compressed */
    if (fbuf->datagram_size > comp_form_size) {
        /* add slack to first fragment */
        frag_size -= (fbuf->datagram_size - comp_form_size);
        /* use compressed form */
        fbuf->datagram_size = (uint16_t)gnrc_pkt_len(pkt->next);
    }
    else {
        /* Add uncompressed datagram dispatch to "compressed form"
         * datagram_size */
        fbuf->datagram_size++;
    }
    fbuf->sfr.arq_timeout = CONFIG_GNRC_SIXLOWPAN_SFR_OPT_ARQ_TIMEOUT_MS;
    fbuf->sfr.window_size = CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE;

    frag = _build_frag_from_fbuf(pkt, fbuf, frag_size);
    if (frag == NULL) {
        DEBUG("6lo sfr: error allocating first fragment\n");
        return 0;
    }
    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);
    sixlowpan_sfr_rfrag_set_offset(hdr, fbuf->datagram_size);
    /* don't copy netif header of pkt => pkt->next */
    frag_size = _copy_pkt_to_frag(data, pkt->next, frag_size, 0);

    DEBUG("6lo sfr: send first fragment (tag: %u, X: %i, seq: %u, "
          "frag_size: %u, datagram_size: %u)\n",
          hdr->base.tag, sixlowpan_sfr_rfrag_ack_req(hdr),
          sixlowpan_sfr_rfrag_get_seq(hdr),
          sixlowpan_sfr_rfrag_get_frag_size(hdr),
          sixlowpan_sfr_rfrag_get_offset(hdr));
    if (!_send_fragment(frag, fbuf, page, 0)) {
        frag_size = 0;
    }
    return frag_size;
}

static uint16_t _send_nth_fragment(gnrc_netif_t *netif,
                                   gnrc_sixlowpan_frag_fb_t *fbuf,
                                   unsigned page)
{
    gnrc_pktsnip_t *frag, *pkt = fbuf->pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    uint8_t *data;
    uint16_t frag_size = (uint16_t)netif->sixlo.max_frag_size -
                         sizeof(sixlowpan_sfr_rfrag_t);
    uint16_t local_offset;

    assert((fbuf->sfr.cur_seq > 0) &&
           (fbuf->sfr.cur_seq <= SIXLOWPAN_SFR_SEQ_MAX));
    assert((fbuf->sfr.frags_sent == 0) || (fbuf->sfr.window.next != NULL));
    assert(fbuf->tag <= UINT8_MAX);

    DEBUG("6lo sfr: determined frag_size = %u\n", frag_size);
    frag = _build_frag_from_fbuf(pkt, fbuf,
                                 _min(frag_size,
                                      fbuf->datagram_size - fbuf->offset));
    if (frag == NULL) {
        DEBUG("6lo sfr: error allocating subsequent fragment\n");
        return 0;
    }
    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);
    sixlowpan_sfr_rfrag_set_offset(hdr, fbuf->offset);
    pkt = pkt->next;    /* don't copy netif header */
    local_offset = _find_offset_and_copy_rest(data, &pkt, frag_size,
                                              fbuf->offset);
    /* copy remaining packet snips */
    local_offset = _copy_pkt_to_frag(data, pkt, frag_size, local_offset);
    DEBUG("6lo sfr: send subsequent fragment (tag: %u, X: %i, seq: %u, "
          "frag_size: %u, offset: %u)\n",
          hdr->base.tag, sixlowpan_sfr_rfrag_ack_req(hdr),
          sixlowpan_sfr_rfrag_get_seq(hdr),
          sixlowpan_sfr_rfrag_get_frag_size(hdr),
          sixlowpan_sfr_rfrag_get_offset(hdr));
    if (!_send_fragment(frag, fbuf, page, fbuf->offset)) {
        local_offset = 0;
    }
    return local_offset;
}

static bool _send_abort_frag(gnrc_pktsnip_t *pkt, uint8_t tag, bool req_ack,
                             unsigned page)
{
    gnrc_pktsnip_t *frag;

    frag = _build_frag_pkt(pkt->data, tag, req_ack, 0, 0);
    if (frag != NULL) {
        sixlowpan_sfr_rfrag_set_offset(frag->next->data, 0);
        _send_frame(frag, NULL, page);
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
            _stats.fragments_sent.aborts++;
        }
        return true;
    }
    return false;
}

static int _resend_frag(clist_node_t *node, void *fbuf_ptr)
{
    _frag_desc_t *frag_desc = (_frag_desc_t *)node;
    gnrc_sixlowpan_frag_fb_t *fbuf = fbuf_ptr;
    gnrc_pktsnip_t *frag, *pkt = fbuf->pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    uint8_t *data;
    uint16_t frag_size = _frag_size(frag_desc), cur_frag_size;

    frag = _build_frag_pkt(pkt->data, (uint8_t)fbuf->tag, false,
                           frag_size, 0);
    if (frag == NULL) {
        DEBUG("6lo sfr: error allocating fragment to resend\n");
        return 1;
    }
    hdr = frag->next->data;
    /* is last fragment in window */
    if (((fbuf->sfr.frags_sent >= fbuf->sfr.window_size) ||
         (fbuf->offset >= fbuf->datagram_size)) &&
        (clist_node_t *)frag_desc == clist_rpeek(&fbuf->sfr.window)) {
        frag_desc->ar_seq_fs |= (SIXLOWPAN_SFR_ACK_REQ << 8U);
        _sched_arq_timeout(fbuf, fbuf->sfr.arq_timeout);
    }
    hdr->ar_seq_fs = byteorder_htons(frag_desc->ar_seq_fs);
    if (frag_desc->offset > 0) {
        sixlowpan_sfr_rfrag_set_offset(hdr, frag_desc->offset);
    }
    else {
        sixlowpan_sfr_rfrag_set_offset(hdr, fbuf->datagram_size);
    }

    data = (uint8_t *)(hdr + 1);
    pkt = pkt->next;    /* don't copy netif header */
    cur_frag_size = _find_offset_and_copy_rest(data, &pkt, frag_size,
                                               frag_desc->offset);
    /* copy remaining packet snips */
    _copy_pkt_to_frag(data, pkt, frag_size, cur_frag_size);
    DEBUG("6lo sfr: resending fragment (retry: %u, tag: %u, X: %i, seq: %u, "
          "frag_size: %u, %s: %u)\n", frag_desc->retries,
          hdr->base.tag, sixlowpan_sfr_rfrag_ack_req(hdr),
          sixlowpan_sfr_rfrag_get_seq(hdr),
          sixlowpan_sfr_rfrag_get_frag_size(hdr),
          (sixlowpan_sfr_rfrag_get_seq(hdr)) ? "offset" : "datagram_size",
          sixlowpan_sfr_rfrag_get_offset(hdr));
    if (_send_frame(frag, NULL, 0)) {
        frag_desc->last_sent = _last_frame_sent;
        return 0;
    }
    else {
        return 1;
    }
}

static void _retry_datagram(gnrc_sixlowpan_frag_fb_t *fbuf)
{
    if ((CONFIG_GNRC_SIXLOWPAN_SFR_DG_RETRIES == 0) ||
        (fbuf->sfr.retrans == 0)) {
        DEBUG("6lo sfr: giving up to send datagram %u\n",
              fbuf->tag);
        _clean_up_fbuf(fbuf, ETIMEDOUT);
    }
    else {
        DEBUG("6lo sfr: Retrying to send datagram %u completely\n", fbuf->tag);
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
            _stats.datagram_resends++;
        }
        fbuf->sfr.retrans--;
        /* return fragmentation buffer to its original state to resend the whole
         * datagram again */
        _clean_slate_datagram(fbuf);
        gnrc_sixlowpan_frag_sfr_send(fbuf->pkt, fbuf, 0);
    }
}

static void _abort_rb(gnrc_pktsnip_t *pkt, _generic_rb_entry_t *entry,
                      gnrc_netif_hdr_t *netif_hdr, bool send_ack)
{
    sixlowpan_sfr_rfrag_t *hdr = pkt->data;

    DEBUG("6lo sfr: Aborting datagram (%s, %02x)\n",
          gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                 netif_hdr->src_l2addr_len, addr_str),
          hdr->base.tag);
    if (send_ack) {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
            _stats.acks.aborts++;
        }
        _send_ack(gnrc_netif_hdr_get_netif(netif_hdr),
                  gnrc_netif_hdr_get_src_addr(netif_hdr),
                  netif_hdr->src_l2addr_len,
                  &hdr->base, _null_bitmap.bf);
    }
    _clean_up_rb_entry(entry);
    gnrc_pktbuf_release(pkt);
}

static void _send_ack(gnrc_netif_t *netif, const uint8_t *dst, uint8_t dst_len,
                      const sixlowpan_sfr_t *hdr, const uint8_t *bitmap)
{
    gnrc_pktsnip_t *ack = _build_ack(netif, dst, dst_len, hdr, bitmap);

    DEBUG("6lo sfr: Sending ACK for (%s, %02x): %02X%02X%02X%02X\n",
          gnrc_netif_addr_to_str(dst, dst_len, addr_str),
          hdr->tag, bitmap[0], bitmap[1], bitmap[2], bitmap[3]);
    if (ack != NULL) {
        _send_frame(ack, NULL, 0);
    }
    else {
        DEBUG("6lo sfr: unable to build ACK for sending\n");
    }
}

static void _sched_next_frame(void)
{
    if (CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US > 0) {
        int state = irq_disable();  /* make timer check atomic */
        bool already_set = (_if_gap_timer.offset ||
                            _if_gap_timer.long_offset);

        irq_restore(state);
        if (!already_set) {
            uint32_t last_sent_since = (_last_frame_sent - xtimer_now_usec());

            if (last_sent_since <= CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US) {
                uint32_t offset = CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US -
                                  last_sent_since;
                DEBUG("6lo sfr: arming inter-frame timer in %" PRIu32 " us\n",
                      last_sent_since);
                xtimer_set_msg(&_if_gap_timer, offset, &_if_gap_msg, _getpid());
            }
            else {
                DEBUG("6lo sfr: send frame immediately\n");
                gnrc_sixlowpan_frag_sfr_inter_frame_gap();
            }
        }
        else {
            DEBUG("6lo sfr: inter-frame timer was already set\n");
        }
    }
}

static void _sched_arq_timeout(gnrc_sixlowpan_frag_fb_t *fbuf, uint32_t offset)
{
    if (fbuf->sfr.arq_timeout_event.msg.content.ptr != NULL) {
        DEBUG("6lo sfr: ARQ timeout for datagram %u already scheduled\n",
              (uint8_t)fbuf->tag);
        return;
    }
    DEBUG("6lo sfr: arming ACK timeout in %lums for datagram %u\n",
          (long unsigned)offset, fbuf->tag);
    fbuf->sfr.arq_timeout_event.event.offset = offset;
    fbuf->sfr.arq_timeout_event.msg.content.ptr = fbuf;
    fbuf->sfr.arq_timeout_event.msg.type = GNRC_SIXLOWPAN_FRAG_SFR_ARQ_TIMEOUT_MSG;
    evtimer_add_msg(&_arq_timer, &fbuf->sfr.arq_timeout_event,
                    _getpid());
}

static void _sched_abort_timeout(gnrc_sixlowpan_frag_fb_t *fbuf)
{
    /* no fragments to wait for anymore as we aborted fragmentation and just
     * wait for an ACK by the reassembling end point that they know. As such,
     * clean-out the fragmentation buffer. */
    _clean_slate_datagram(fbuf);
    fbuf->sfr.retrans = 0;
    _sched_arq_timeout(fbuf, fbuf->sfr.arq_timeout);
}

static void _handle_rfrag(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                          unsigned page)
{
    _generic_rb_entry_t entry = { .type = _UNDEF };

    if (sixlowpan_sfr_rfrag_get_seq(pkt->data) == 0U) {
        _handle_1st_rfrag(netif_hdr, pkt, page, &entry);
    }
    else {
        _handle_nth_rfrag(netif_hdr, pkt, page, &entry);
    }
}

static void _handle_ack(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                        unsigned page)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    sixlowpan_sfr_ack_t *hdr = pkt->data;

    (void)page;
    DEBUG("6lo sfr: received ACK for datagram (%s, %02x): %02X%02X%02X%02X\n",
          gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                 netif_hdr->src_l2addr_len,
                                 addr_str), hdr->base.tag,
          hdr->bitmap[0], hdr->bitmap[1], hdr->bitmap[2], hdr->bitmap[3]);
    if ((vrbe = gnrc_sixlowpan_frag_vrb_reverse(
            gnrc_netif_hdr_get_netif(netif_hdr),
            gnrc_netif_hdr_get_src_addr(netif_hdr),
            netif_hdr->src_l2addr_len, hdr->base.tag)) != NULL) {
        /* we found a VRB entry by reverse lookup, forward ACK further down. */
        sixlowpan_sfr_t mock_base = { .disp_ecn = hdr->base.disp_ecn,
                                      .tag = vrbe->super.tag };
        DEBUG("6lo sfr: forward ACK to (%s, %02x)\n",
              gnrc_netif_addr_to_str(vrbe->super.src, vrbe->super.src_len,
                                     addr_str), vrbe->super.tag);
        _send_ack(vrbe->in_netif, vrbe->super.src, vrbe->super.src_len,
                  &mock_base, hdr->bitmap);
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
            _stats.acks.forwarded++;
        }
        if ((unaligned_get_u32(hdr->bitmap) == _full_bitmap.u32) ||
            (unaligned_get_u32(hdr->bitmap) == _null_bitmap.u32)) {
            if (CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER > 0) {
                /* garbage-collect entry after CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER
                 * microseconds */
                vrbe->super.arrival = xtimer_now_usec() -
                                      (CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US -
                                       CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER);
            }
            else {
                gnrc_sixlowpan_frag_vrb_rm(vrbe);
            }
        }
        else {
            vrbe->super.arrival = xtimer_now_usec();
        }
    }
    else {
        gnrc_sixlowpan_frag_fb_t *fbuf;

        if ((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(hdr->base.tag)) != NULL) {
            /* ACK for pending ACK timeout received. removing ACK timeout */
            DEBUG("6lo sfr: cancelling ARQ timeout\n");
            evtimer_del((evtimer_t *)(&_arq_timer),
                        &fbuf->sfr.arq_timeout_event.event);
            fbuf->sfr.arq_timeout_event.msg.content.ptr = NULL;
            if ((unaligned_get_u32(hdr->bitmap) == _null_bitmap.u32)) {
                /* ACK indicates the reassembling endpoint canceled reassembly
                 */
                DEBUG("6lo sfr: fragmentation canceled\n");
                /* Retry to send whole datagram if configured, otherwise
                 * cancel fragmentation */
                _retry_datagram(fbuf);
            }
            else {
                /* Check and resent failed fragments within the current window
                 */
               _check_failed_frags(hdr, fbuf);
            }
        }
        else {
            DEBUG("6lo sfr: no VRB or fragmentation buffer found\n");
        }
    }
    gnrc_pktbuf_release(pkt);
}

static int _forward_rfrag(gnrc_pktsnip_t *pkt, _generic_rb_entry_t *entry,
                          uint16_t offset, unsigned page)
{
    gnrc_pktsnip_t *old, *new = gnrc_netif_hdr_build(
            NULL, 0,
            entry->entry.base->dst, entry->entry.base->dst_len
        );
    sixlowpan_sfr_rfrag_t *hdr;

    assert(entry->type == _VRB);
    /* restrict out_tag to value space of SFR, so that later RFRAG ACK can find
     * it in reverse look-up */
    entry->entry.vrb->out_tag &= UINT8_MAX;
    DEBUG("6lo sfr: Forwarding to (%s, %u)\n",
          gnrc_netif_addr_to_str(entry->entry.base->dst,
                                 entry->entry.base->dst_len, addr_str),
          entry->entry.vrb->out_tag);
    if (new == NULL) {
        DEBUG("6lo sfr: Unable to forward fragment, "
              "packet buffer full\n");
        gnrc_pktbuf_release(pkt);
        return -ENOMEM;
    }

    hdr = pkt->data;
    old = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    if (old != NULL) {
        /* remove original netif header */
        gnrc_pktbuf_remove_snip(pkt, old);
    }
    if (offset > 0) {
        offset += entry->entry.vrb->offset_diff;
    }
    sixlowpan_sfr_rfrag_set_offset(hdr, offset);
    hdr->base.tag = entry->entry.vrb->out_tag;
    gnrc_netif_hdr_set_netif(new->data, entry->entry.vrb->out_netif);
    new->next = pkt;
    _send_frame(new, NULL, page);
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)) {
        _stats.fragments_sent.forwarded++;
    }
    return 0;
}

/** @} */
