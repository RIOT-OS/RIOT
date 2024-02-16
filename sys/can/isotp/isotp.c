/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_can_isotp
 * @{
 * @file
 * @brief       ISO TP high level interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "can/common.h"
#include "can/isotp.h"
#include "can/raw.h"
#include "can/router.h"
#include "macros/utils.h"
#include "mutex.h"
#include "net/gnrc/pktbuf.h"
#include "thread.h"
#include "timex.h"
#include "utlist.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CAN_ISOTP_MSG_QUEUE_SIZE
#define CAN_ISOTP_MSG_QUEUE_SIZE 64
#endif

#ifndef CAN_ISOTP_TIMEOUT_N_As
#define CAN_ISOTP_TIMEOUT_N_As (1 * US_PER_SEC)
#endif

#ifndef CAN_ISOTP_TIMEOUT_N_Bs
#define CAN_ISOTP_TIMEOUT_N_Bs (1 * US_PER_SEC)
#endif

#ifndef CAN_ISOTP_TIMEOUT_N_Ar
#define CAN_ISOTP_TIMEOUT_N_Ar (1 * US_PER_SEC)
#endif

#ifndef CAN_ISOTP_TIMEOUT_N_Cr
#define CAN_ISOTP_TIMEOUT_N_Cr (1 * US_PER_SEC)
#endif

enum {
    ISOTP_IDLE = 0,
    ISOTP_WAIT_FC,
    ISOTP_WAIT_CF,
    ISOTP_SENDING_SF,
    ISOTP_SENDING_FF,
    ISOTP_SENDING_CF,
    ISOTP_SENDING_FC,
    ISOTP_SENDING_NEXT_CF,
};

#define MAX_MSG_LENGTH 4095

/* N_PCI type values in bits 7-4 of N_PCI bytes */
#define N_PCI_SF 0x00 /* single frame */
#define N_PCI_FF 0x10 /* first frame */
#define N_PCI_CF 0x20 /* consecutive frame */
#define N_PCI_FC 0x30 /* flow control */

#define N_PCI_SZ 1  /* size of the PCI byte #1 */
#define SF_PCI_SZ 1 /* size of SingleFrame PCI including 4 bit SF_DL */
#define FF_PCI_SZ 2 /* size of FirstFrame PCI including 12 bit FF_DL */
#define FC_CONTENT_SZ 3 /* flow control content size in byte (FS/BS/STmin) */

/* Flow Status given in FC frame */
#define ISOTP_FC_CTS    0  /* clear to send */
#define ISOTP_FC_WT     1  /* wait */
#define ISOTP_FC_OVFLW  2  /* overflow */

static kernel_pid_t isotp_pid = KERNEL_PID_UNDEF;
static struct isotp *isotp_list = NULL;
static mutex_t lock = MUTEX_INIT;

static void _rx_timeout(void *arg);
static int _isotp_send_fc(struct isotp *isotp, int ae, uint8_t status);
static int _isotp_tx_send(struct isotp *isotp, can_frame_t *frame);

static int _send_msg(msg_t *msg, can_reg_entry_t *entry)
{
#ifdef MODULE_CAN_MBOX
    switch (entry->type) {
    case CAN_TYPE_DEFAULT:
        return msg_try_send(msg, entry->target.pid);
    case CAN_TYPE_MBOX:
        DEBUG("_send_msg: sending msg=%p to mbox=%p\n", (void *)msg, (void *)entry->target.mbox);
        return mbox_try_put(entry->target.mbox, msg);
    default:
        return -ENOTSUP;
    }
#else
    return msg_try_send(msg, entry->target.pid);
#endif
}

static int _isotp_dispatch_rx(struct isotp *isotp)
{
    msg_t msg;
    int ret = 0;
    can_rx_data_t *data;

    msg.type = CAN_MSG_RX_INDICATION;
    data = can_pkt_alloc_rx_data(isotp->rx.snip,
                                 isotp->rx.snip->size + sizeof(*isotp->rx.snip),
                                 isotp->arg);

    if (!data) {
        return -ENOMEM;
    }

    msg.content.ptr = data;
    if (_send_msg(&msg, &isotp->entry) < 1) {
        DEBUG("_isotp_dispatch_rx: msg lost, freeing rx buf\n");
        gnrc_pktbuf_release(((gnrc_pktsnip_t *)data->data.iov_base));
        can_pkt_free_rx_data(data);
        ret = -EOVERFLOW;
    }

    isotp->rx.snip = NULL;

    return ret;
}

static int _isotp_dispatch_tx(struct isotp *isotp, int err)
{
    msg_t msg;

    gnrc_pktbuf_release(isotp->tx.snip);
    isotp->tx.snip = NULL;

    if (isotp->opt.flags & CAN_ISOTP_TX_DONT_WAIT) {
        return 0;
    }

    if (!err) {
        msg.type = CAN_MSG_TX_CONFIRMATION;
    }
    else {
        msg.type = CAN_MSG_TX_ERROR;
    }

    msg.content.ptr = isotp->arg;

    if (_send_msg(&msg, &isotp->entry) < 1) {
        DEBUG("_isotp_dispatch_tx: msg lost\n");
        return -EOVERFLOW;
    }

    return 0;
}

static void _rx_timeout(void *arg)
{
    msg_t msg;

    DEBUG("_rx_timeout: arg=%p\n", arg);

    msg.type = CAN_MSG_ISOTP_RX_TIMEOUT;
    msg.content.ptr = arg;

    msg_send(&msg, isotp_pid);
}

static void _tx_timeout(void *arg)
{
    msg_t msg;

    DEBUG("_tx_timeout: arg=%p\n", arg);

    msg.type = CAN_MSG_ISOTP_TX_TIMEOUT;
    msg.content.ptr = arg;

    msg_send(&msg, isotp_pid);
}

static int _isotp_rcv_fc(struct isotp *isotp, can_frame_t *frame, int ae)
{
    if (isotp->tx.state != ISOTP_WAIT_FC) {
        return 0;
    }

    ztimer_remove(ZTIMER_USEC, &isotp->tx_timer);

    if (frame->len < ae + FC_CONTENT_SZ) {
        /* Invalid length */
        isotp->tx.state = ISOTP_IDLE;
        return 1;
    }

    isotp->txfc.bs = frame->data[ae + 1];
    isotp->txfc.stmin = frame->data[ae + 2];

    DEBUG("_isotp_rcv_fc: first FC: bs=0x%" PRIx8 ", stmin=0x%" PRIx8 "\n",
          isotp->txfc.bs, isotp->txfc.stmin);

    if ((isotp->txfc.stmin > 0x7F) &&
            ((isotp->txfc.stmin < 0xF1) || (isotp->txfc.stmin > 0xF9))) {
        /* according to ISO15765-2 8.5.5.6 */
        isotp->txfc.stmin = 0x7F;
    }
    /* ISO15765-2 8.5.5.5 */
    /* Range 0x0 - 0x7F -> 0 ms - 127 ms */
    if (isotp->txfc.stmin < 0x80) {
        isotp->tx_gap = isotp->txfc.stmin * US_PER_MS;
    }
    /* Range 0xF1 - 0xF9 -> 100 us - 900 us */
    else {
        isotp->tx_gap = (isotp->txfc.stmin - 0xF0) * 100;
    }

    switch (frame->data[ae] & 0xF) {
    case ISOTP_FC_CTS:
        isotp->tx_wft = 0;
        isotp->tx.bs = 0;
        isotp->tx.state = ISOTP_SENDING_NEXT_CF;
        ztimer_set(ZTIMER_USEC, &isotp->tx_timer, isotp->tx_gap);
        break;

    case ISOTP_FC_WT:
        if (isotp->tx_wft++ >= isotp->txfc.wftmax) {
            isotp->tx.state = ISOTP_IDLE;
            _isotp_dispatch_tx(isotp, ETIMEDOUT);
            return 1;
        }
        /* BS and STmin shall be ignored */
        ztimer_set(ZTIMER_USEC, &isotp->tx_timer, CAN_ISOTP_TIMEOUT_N_Bs);
        break;

    case ISOTP_FC_OVFLW:
        /* overflow on receiver side -> error */

    default:
        isotp->tx.state = ISOTP_IDLE;
        _isotp_dispatch_tx(isotp, EOVERFLOW);
        break;
    }

    return 0;
}

static int _isotp_rcv_sf(struct isotp *isotp, can_frame_t *frame, int ae)
{
    ztimer_remove(ZTIMER_USEC, &isotp->rx_timer);
    isotp->rx.state = ISOTP_IDLE;

    int len = (frame->data[ae] & 0x0F);
    if (len > frame->len - (SF_PCI_SZ + ae)) {
        return 1;
    }

    gnrc_pktsnip_t *snip = gnrc_pktbuf_add(NULL, NULL, len, GNRC_NETTYPE_UNDEF);
    if (!snip) {
        return 1;
    }
    isotp->rx.snip = snip;

    isotp->rx.idx = 0;
    for (size_t i = SF_PCI_SZ + ae; i < isotp->rx.snip->size + ae + SF_PCI_SZ; i++) {
        ((uint8_t *)isotp->rx.snip->data)[isotp->rx.idx++] = frame->data[i];
    }

    return _isotp_dispatch_rx(isotp);
}

static int _isotp_rcv_ff(struct isotp *isotp, can_frame_t *frame, int ae)
{
    isotp->rx.state = ISOTP_IDLE;

    int len = (frame->data[ae] & 0x0F) << 8;
    len += frame->data[ae + 1];

    if (isotp->rx.snip) {
        DEBUG("_isotp_rcv_ff: freeing previous rx buf\n");
        gnrc_pktbuf_release(isotp->rx.snip);
    }

    if (len > MAX_MSG_LENGTH) {
        if (!(isotp->opt.flags & CAN_ISOTP_LISTEN_MODE)) {
            _isotp_send_fc(isotp, ae, ISOTP_FC_OVFLW);
        }
        return 1;
    }

    gnrc_pktsnip_t *snip = gnrc_pktbuf_add(NULL, NULL, len, GNRC_NETTYPE_UNDEF);
    if (!snip) {
        if (!(isotp->opt.flags & CAN_ISOTP_LISTEN_MODE)) {
            _isotp_send_fc(isotp, ae, ISOTP_FC_OVFLW);
        }
        return 1;
    }
    isotp->rx.snip = snip;

    isotp->rx.idx = 0;
    for (int i = ae + FF_PCI_SZ; i < frame->len; i++) {
        ((uint8_t *)isotp->rx.snip->data)[isotp->rx.idx++] = frame->data[i];
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("_isotp_rcv_ff: rx.buf=");
        for (unsigned i = 0; i < isotp->rx.idx; i++) {
            DEBUG("%02hhx", ((uint8_t *)isotp->rx.snip->data)[i]);
        }
        DEBUG("\n");
    }

    isotp->rx.sn = 1;

    if (isotp->opt.flags & CAN_ISOTP_LISTEN_MODE) {
        isotp->rx.state = ISOTP_WAIT_CF;
        return 0;
    }

    isotp->rx.state = ISOTP_SENDING_FC;
    _isotp_send_fc(isotp, ae, ISOTP_FC_CTS);

    return 0;
}

static int _isotp_rcv_cf(struct isotp *isotp, can_frame_t *frame, int ae)
{
    DEBUG("_isotp_rcv_cf: state=%d\n", isotp->rx.state);

    if (isotp->rx.state != ISOTP_WAIT_CF) {
        return 1;
    }

    ztimer_remove(ZTIMER_USEC, &isotp->rx_timer);

    if ((frame->data[ae] & 0x0F) != isotp->rx.sn) {
        DEBUG("_isotp_rcv_cf: wrong seq number %d, expected %d\n", frame->data[ae] & 0x0F, isotp->rx.sn);
        isotp->rx.state = ISOTP_IDLE;
        gnrc_pktbuf_release(isotp->rx.snip);
        isotp->rx.snip = NULL;
        return 1;
    }
    isotp->rx.sn++;
    isotp->rx.sn %= 16;

    for (int i = ae + N_PCI_SZ; i < frame->len; i++) {
        ((uint8_t *)isotp->rx.snip->data)[isotp->rx.idx++] = frame->data[i];
        if (isotp->rx.idx >= isotp->rx.snip->size) {
            break;
        }
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("_isotp_rcv_cf: rx.buf=");
        for (unsigned i = 0; i < isotp->rx.idx; i++) {
            DEBUG("%02hhx", ((uint8_t *)isotp->rx.snip->data)[i]);
        }
        DEBUG("\n");
    }

    if (isotp->rx.idx >= isotp->rx.snip->size) {
        isotp->rx.state = ISOTP_IDLE;
        return _isotp_dispatch_rx(isotp);
    }

    if (isotp->opt.flags & CAN_ISOTP_LISTEN_MODE) {
        return 0;
    }

    DEBUG("_isotp_rcv_cf: rxfc.bs=%" PRIx8 " rx.bs=%" PRIx8 "\n", isotp->rxfc.bs, isotp->rx.bs);

    if (!isotp->rxfc.bs || (++isotp->rx.bs < isotp->rxfc.bs)) {
        ztimer_set(ZTIMER_USEC, &isotp->rx_timer, CAN_ISOTP_TIMEOUT_N_Cr);
        return 0;
    }

    return _isotp_send_fc(isotp, ae, ISOTP_FC_CTS);
}

static int _isotp_rcv(struct isotp *isotp, can_frame_t *frame)
{
    int ae = (isotp->opt.flags & CAN_ISOTP_EXTEND_ADDR) ? 1 : 0;
    uint8_t n_pci_type;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("_isotp_rcv: id=%" PRIx32 " data=", frame->can_id);
        for (int i = 0; i < frame->len; i++) {
            DEBUG("%02hhx", frame->data[i]);
        }
        DEBUG("\n");
    }

    if (ae && frame->data[0] != isotp->opt.rx_ext_address) {
        return 1;
    }

    n_pci_type = frame->data[ae] & 0xF0;

    switch (n_pci_type) {
    case N_PCI_FC:
        return _isotp_rcv_fc(isotp, frame, ae);

    case N_PCI_SF:
        return _isotp_rcv_sf(isotp, frame, ae);

    case N_PCI_FF:
        return _isotp_rcv_ff(isotp, frame, ae);

    case N_PCI_CF:
        return _isotp_rcv_cf(isotp, frame, ae);

    }

    return 1;
}

static int _isotp_send_fc(struct isotp *isotp, int ae, uint8_t status)
{
    can_frame_t fc;

    fc.can_id = isotp->opt.tx_id;

    if (isotp->opt.flags & CAN_ISOTP_TX_PADDING) {
        memset(fc.data, isotp->opt.txpad_content, CAN_MAX_DLEN);
        fc.len = CAN_MAX_DLEN;
    }
    else {
        fc.len = ae + FC_CONTENT_SZ;
    }

    fc.data[ae] = N_PCI_FC | status;
    fc.data[ae + 1] = isotp->rxfc.bs;
    fc.data[ae + 2] = isotp->rxfc.stmin;

    if (ae) {
        fc.data[0] = isotp->opt.ext_address;
    }

    isotp->rx.bs = 0;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("_isotp_send_fc: id=%" PRIx32 " data=", fc.can_id);
        for (int i = 0; i < fc.len; i++) {
            DEBUG("%02hhx", fc.data[i]);
        }
        DEBUG("\n");
    }

    ztimer_set(ZTIMER_USEC, &isotp->rx_timer, CAN_ISOTP_TIMEOUT_N_Ar);
    isotp->rx.tx_handle = raw_can_send(isotp->entry.ifnum, &fc, isotp_pid);

    if (isotp->rx.tx_handle >= 0) {
        return 0;
    }
    else {
        isotp->rx.state = ISOTP_IDLE;
        ztimer_remove(ZTIMER_USEC, &isotp->rx_timer);
        return isotp->rx.tx_handle;
    }
}

static void _isotp_create_ff(struct isotp *isotp, can_frame_t *frame, int ae)
{

    frame->can_id = isotp->opt.tx_id;
    frame->len = CAN_MAX_DLEN;

    if (ae) {
        frame->data[0] = isotp->opt.ext_address;
    }

    frame->data[ae] = (uint8_t)(isotp->tx.snip->size >> 8) | N_PCI_FF;
    frame->data[ae + 1] = (uint8_t) isotp->tx.snip->size & 0xFFU;

    for (int i = ae + FF_PCI_SZ; i < CAN_MAX_DLEN; i++) {
        frame->data[i] = ((uint8_t *)isotp->tx.snip->data)[isotp->tx.idx++];
    }

    isotp->tx.sn = 1;
}

static void _isotp_fill_dataframe(struct isotp *isotp, can_frame_t *frame, int ae)
{
    size_t pci_len = N_PCI_SZ + ae;
    size_t space = CAN_MAX_DLEN - pci_len;
    size_t num_bytes = MIN(space, isotp->tx.snip->size - isotp->tx.idx);

    frame->can_id = isotp->opt.tx_id;
    frame->len = num_bytes + pci_len;

    DEBUG("_isotp_fill_dataframe: num_bytes=%" PRIuSIZE ", pci_len=%" PRIuSIZE "\n",
          num_bytes, pci_len);

    if (num_bytes < space) {
        if (isotp->opt.flags & CAN_ISOTP_TX_PADDING) {
            frame->len = CAN_MAX_DLEN;
            memset(frame->data, isotp->opt.txpad_content, frame->len);
        }
    }

    for (size_t i = 0; i < num_bytes; i++) {
        frame->data[pci_len + i] = ((uint8_t *)isotp->tx.snip->data)[isotp->tx.idx++];
    }

    if (ae) {
        frame->data[0] = isotp->opt.ext_address;
    }

}

static void _isotp_tx_timeout_task(struct isotp *isotp)
{
    int ae = (isotp->opt.flags & CAN_ISOTP_EXTEND_ADDR) ? 1 : 0;
    can_frame_t frame;

    DEBUG("_isotp_tx_timeout_task: state=%d\n", isotp->tx.state);

    switch (isotp->tx.state) {
    case ISOTP_WAIT_FC:
        DEBUG("_isotp_tx_timeout_task: FC not received on time\n");
        isotp->tx.state = ISOTP_IDLE;
        _isotp_dispatch_tx(isotp, ETIMEDOUT);
        break;

    case ISOTP_SENDING_NEXT_CF:
        DEBUG("_isotp_tx_timeout_task: sending next CF\n");
        _isotp_fill_dataframe(isotp, &frame, ae);
        frame.data[ae] = N_PCI_CF | isotp->tx.sn++;
        isotp->tx.sn %= 16;
        isotp->tx.bs++;

        isotp->tx.state = ISOTP_SENDING_CF;
        _isotp_tx_send(isotp, &frame);
        break;

    case ISOTP_SENDING_CF:
    case ISOTP_SENDING_FF:
    case ISOTP_SENDING_SF:
        DEBUG("_isotp_tx_timeout_task: timeout on DLL\n");
        isotp->tx.state = ISOTP_IDLE;
        raw_can_abort(isotp->entry.ifnum, isotp->tx.tx_handle);
        _isotp_dispatch_tx(isotp, ETIMEDOUT);
        break;
    }
}

static void _isotp_tx_tx_conf(struct isotp *isotp)
{
    ztimer_remove(ZTIMER_USEC, &isotp->tx_timer);
    isotp->tx.tx_handle = 0;

    DEBUG("_isotp_tx_tx_conf: state=%d\n", isotp->tx.state);

    switch (isotp->tx.state) {
    case ISOTP_SENDING_SF:
        isotp->tx.state = ISOTP_IDLE;
        _isotp_dispatch_tx(isotp, 0);
        break;

    case ISOTP_SENDING_FF:
        isotp->tx.state = ISOTP_WAIT_FC;
        ztimer_set(ZTIMER_USEC, &isotp->tx_timer, CAN_ISOTP_TIMEOUT_N_Bs);
        break;

    case ISOTP_SENDING_CF:
        if (isotp->tx.idx >= isotp->tx.snip->size) {
            /* Finished */
            isotp->tx.state = ISOTP_IDLE;
            _isotp_dispatch_tx(isotp, 0);
            break;
        }

        if (isotp->txfc.bs && (isotp->tx.bs >= isotp->txfc.bs)) {
            /* wait for FC */
            isotp->tx.state = ISOTP_WAIT_FC;
            ztimer_set(ZTIMER_USEC, &isotp->tx_timer, CAN_ISOTP_TIMEOUT_N_Bs);
            break;
        }

        isotp->tx.state = ISOTP_SENDING_NEXT_CF;
        ztimer_set(ZTIMER_USEC, &isotp->tx_timer, isotp->tx_gap);
        break;
    }
}

static void _isotp_rx_timeout_task(struct isotp *isotp)
{
    switch (isotp->rx.state) {
    case ISOTP_SENDING_FC:
        DEBUG("_isotp_rx_timeout_task: FC tx conf timeout\n");
        raw_can_abort(isotp->entry.ifnum, isotp->rx.tx_handle);
        /* Fall through */
    case ISOTP_WAIT_CF:
        DEBUG("_isotp_rx_timeout_task: free rx buf\n");
        gnrc_pktbuf_release(isotp->rx.snip);
        isotp->rx.snip = NULL;
        isotp->rx.state = ISOTP_IDLE;
        /* TODO dispatch rx error ? */
        break;
    }
}

static void _isotp_rx_tx_conf(struct isotp *isotp)
{
    ztimer_remove(ZTIMER_USEC, &isotp->rx_timer);
    isotp->rx.tx_handle = 0;

    DEBUG("_isotp_rx_tx_conf: state=%d\n", isotp->rx.state);

    switch (isotp->rx.state) {
    case ISOTP_SENDING_FC:
        isotp->rx.state = ISOTP_WAIT_CF;
        ztimer_set(ZTIMER_USEC, &isotp->rx_timer, CAN_ISOTP_TIMEOUT_N_Cr);
        break;
    }
}

static int _isotp_tx_send(struct isotp *isotp, can_frame_t *frame)
{
    ztimer_set(ZTIMER_USEC, &isotp->tx_timer, CAN_ISOTP_TIMEOUT_N_As);
    isotp->tx.tx_handle = raw_can_send(isotp->entry.ifnum, frame, isotp_pid);
    DEBUG("isotp_send: FF/SF/CF sent handle=%d\n", isotp->tx.tx_handle);
    if (isotp->tx.tx_handle < 0) {
        ztimer_remove(ZTIMER_USEC, &isotp->tx_timer);
        isotp->tx.state = ISOTP_IDLE;
        return _isotp_dispatch_tx(isotp, isotp->tx.tx_handle);
    }

    return 0;
}

static int _isotp_send_sf_ff(struct isotp *isotp)
{
    can_frame_t frame;
    unsigned ae = (isotp->opt.flags & CAN_ISOTP_EXTEND_ADDR) ? 1 : 0;

    if (isotp->tx.snip->size <= CAN_MAX_DLEN - SF_PCI_SZ - ae) {
        /* Fits into a single frame */
        _isotp_fill_dataframe(isotp, &frame, ae);

        frame.data[ae] = N_PCI_SF;
        frame.data[ae] |= isotp->tx.snip->size;

        isotp->tx.state = ISOTP_SENDING_SF;
    }
    else {
        isotp->tx.state = ISOTP_SENDING_FF;
        /* Must send a First frame */
        _isotp_create_ff(isotp, &frame, ae);
    }

    return _isotp_tx_send(isotp, &frame);
}

static void *_isotp_thread(void *args)
{
    (void)args;
    msg_t msg, msg_queue[CAN_ISOTP_MSG_QUEUE_SIZE];
    struct can_rx_data *rx_frame;
    struct isotp *isotp;

    /* setup the device layers message queue */
    msg_init_queue(msg_queue, CAN_ISOTP_MSG_QUEUE_SIZE);

    isotp_pid = thread_getpid();

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case CAN_MSG_SEND_FRAME:
            _isotp_send_sf_ff(msg.content.ptr);
            break;
        case CAN_MSG_RX_INDICATION:
            rx_frame = msg.content.ptr;
            if (!rx_frame) {
                DEBUG("_isotp_thread: CAN_MSG_RX_INDICATION with NULL ptr\n");
                break;
            }
            DEBUG("_isotp_thread: CAN_MSG_RX_INDICATION, frame=%p, data=%p\n",
                  (void *)rx_frame->data.iov_base, rx_frame->arg);
            _isotp_rcv((struct isotp *)rx_frame->arg, rx_frame->data.iov_base);
            raw_can_free_frame(rx_frame);
            break;
        case CAN_MSG_TX_CONFIRMATION:
            DEBUG("_isotp_thread: CAN_MSG_TX_CONFIRMATION, handle=%d\n", (int)msg.content.value);
            mutex_lock(&lock);
            LL_FOREACH(isotp_list, isotp) {
                if (isotp->tx.tx_handle == (int)msg.content.value) {
                    mutex_unlock(&lock);
                    _isotp_tx_tx_conf(isotp);
                    break;
                }
                else if (isotp->rx.tx_handle == (int)msg.content.value) {
                    mutex_unlock(&lock);
                    _isotp_rx_tx_conf(isotp);
                    break;
                }
            }
            if (isotp == NULL) {
                mutex_unlock(&lock);
            }
            break;
        case CAN_MSG_ISOTP_RX_TIMEOUT:
            isotp = msg.content.ptr;
            DEBUG("_isotp_thread: RX TIMEOUT arg=%p\n", (void *)isotp);
            _isotp_rx_timeout_task(isotp);
            break;
        case CAN_MSG_ISOTP_TX_TIMEOUT:
            isotp = msg.content.ptr;
            DEBUG("_isotp_thread: TX_TIMEOUT arg=%p\n", (void *)isotp);
            _isotp_tx_timeout_task(isotp);
            break;
        }
    }

    return NULL;
}

kernel_pid_t isotp_init(char *stack, int stacksize, char priority, const char *name)
{
    kernel_pid_t res;

    DEBUG("isotp_init\n");

    /* create new can device thread */
    res = thread_create(stack, stacksize, priority, 0,
                         _isotp_thread, NULL, name);
    if (res <= 0) {
        return -EINVAL;
    }

    return res;
}

int isotp_send(struct isotp *isotp, const void *buf, int len, int flags)
{
    assert(isotp != NULL);
#ifdef MODULE_CAN_MBOX
    assert((isotp->entry.type == CAN_TYPE_DEFAULT && pid_is_valid(isotp->entry.target.pid)) ||
           (isotp->entry.type == CAN_TYPE_MBOX && isotp->entry.target.mbox != NULL));
#else
    assert(isotp->entry.target.pid != KERNEL_PID_UNDEF);
#endif
    assert (len && len <= MAX_MSG_LENGTH);

    if (isotp->tx.state != ISOTP_IDLE) {
        return -EBUSY;
    }

    if (flags) {
        isotp->opt.flags &= CAN_ISOTP_RX_FLAGS_MASK;
        isotp->opt.flags |= (flags & CAN_ISOTP_TX_FLAGS_MASK);
    }

    gnrc_pktsnip_t *snip = gnrc_pktbuf_add(NULL, NULL, len, GNRC_NETTYPE_UNDEF);
    if (!snip) {
        return -ENOMEM;
    }
    isotp->tx.snip = snip;

    memcpy(isotp->tx.snip->data, buf, len);

    isotp->tx.idx = 0;

    isotp->tx_wft = 0;

    msg_t msg;
    msg.type = CAN_MSG_SEND_FRAME;
    msg.content.ptr = isotp;
    msg_send(&msg, isotp_pid);

    return len;
}

int isotp_bind(struct isotp *isotp, can_reg_entry_t *entry, void *arg,
               struct isotp_fc_options *fc_options)
{
    int ret;

    assert(isotp != NULL);
#ifdef MODULE_CAN_MBOX
    assert((entry->type == CAN_TYPE_DEFAULT && pid_is_valid(entry->target.pid)) ||
           (entry->type == CAN_TYPE_MBOX && entry->target.mbox != NULL));
#else
    assert(pid_is_valid(entry->target.pid));
#endif
    assert(isotp->opt.tx_id != isotp->opt.rx_id);
    assert(!((isotp->opt.tx_id | isotp->opt.rx_id) & (CAN_RTR_FLAG | CAN_ERR_FLAG)));
    assert(entry->ifnum < CAN_DLL_NUMOF);

    isotp->rx_timer.callback = _rx_timeout;
    isotp->rx_timer.arg = isotp;

    isotp->tx_timer.callback = _tx_timeout;
    isotp->tx_timer.arg = isotp;

    memset(&isotp->rx, 0, sizeof(struct tpcon));
    memset(&isotp->tx, 0, sizeof(struct tpcon));

    isotp->rxfc.bs = fc_options ? fc_options->bs : CAN_ISOTP_BS;
    isotp->rxfc.stmin = fc_options ? fc_options->stmin : CAN_ISOTP_STMIN;
    isotp->rxfc.wftmax = 0;

    isotp->txfc.bs = 0;
    isotp->txfc.stmin = 0;
    isotp->txfc.wftmax = fc_options ? fc_options->wftmax : CAN_ISOTP_WFTMAX;

    isotp->entry.ifnum = entry->ifnum;
#ifdef MODULE_CAN_MBOX
    isotp->entry.type = entry->type;
    isotp->entry.target.mbox = entry->target.mbox;
#else
    isotp->entry.target.pid = entry->target.pid;
#endif
    isotp->arg = arg;
    isotp->next = NULL;

    DEBUG("isotp_bind: ifnum=%d, txid=%" PRIx32 ", rxid=%" PRIx32 ", flags=0x%" PRIx16 "\n",
          isotp->entry.ifnum, isotp->opt.tx_id, isotp->opt.rx_id, isotp->opt.flags);
    DEBUG("isotp_bind: pid=%" PRIkernel_pid "\n", entry->target.pid);

    struct can_filter filter = {
        .can_id = isotp->opt.rx_id,
        .can_mask = 0xFFFFFFFF,
    };
    ret = raw_can_subscribe_rx(isotp->entry.ifnum, &filter, isotp_pid, isotp);
    if (ret < 0) {
        return ret;
    }

    mutex_lock(&lock);
    LL_APPEND(isotp_list, isotp);
    mutex_unlock(&lock);

    return 0;
}

void isotp_free_rx(can_rx_data_t *rx)
{
    DEBUG("isotp_free_rx: rx=%p\n", (void *)rx);
    gnrc_pktbuf_release(rx->data.iov_base);
    can_pkt_free_rx_data(rx);
}

int isotp_release(struct isotp *isotp)
{
    assert(isotp != NULL);
#ifdef MODULE_CAN_MBOX
    assert((isotp->entry.type == CAN_TYPE_DEFAULT && pid_is_valid(isotp->entry.target.pid)) ||
           (isotp->entry.type == CAN_TYPE_MBOX && isotp->entry.target.mbox != NULL));
#else
    assert(isotp->entry.target.pid != KERNEL_PID_UNDEF);
#endif

    DEBUG("isotp_release: isotp=%p\n", (void *)isotp);

    struct can_filter filter = {
        .can_id = isotp->opt.rx_id,
        .can_mask = 0xFFFFFFFF,
    };
    raw_can_unsubscribe_rx(isotp->entry.ifnum, &filter, isotp_pid, isotp);
    ztimer_remove(ZTIMER_USEC, &isotp->rx_timer);

    if (isotp->rx.snip) {
        DEBUG("isotp_release: freeing rx buf\n");
        gnrc_pktbuf_release(isotp->rx.snip);
        isotp->rx.snip = NULL;
    }
    isotp->rx.state = ISOTP_IDLE;
    isotp->entry.target.pid = KERNEL_PID_UNDEF;

    ztimer_remove(ZTIMER_USEC, &isotp->tx_timer);

    mutex_lock(&lock);
    LL_DELETE(isotp_list, isotp);
    mutex_unlock(&lock);

    if (isotp->tx.snip) {
        DEBUG("isotp_release: freeing rx buf\n");
        gnrc_pktbuf_release(isotp->tx.snip);
        isotp->tx.snip = NULL;
    }
    isotp->tx.state = ISOTP_IDLE;

    return 0;
}
