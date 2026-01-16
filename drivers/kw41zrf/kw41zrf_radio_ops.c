/*
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 *
 * @file
 * @brief       Implementation of 802.15.4 Radio HAL interface for KW41ZRF
 *              transceivers.
 *
 * @author     Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 * @}
 */

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "kw41zrf.h"
#include "kw41zrf_netdev.h"
#include "kw41zrf_intern.h"
#include "kw41zrf_getset.h"
#include "vendor/MKW41Z4.h"

#include "bit.h"
#include "log.h"
#include "iolist.h"
#include "byteorder.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#ifdef MODULE_OD
#  include "od.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#define CHECKSUM_MASK               0xFFFF

/* Mask for all kinds of IEEE 802.15.4 frames supported by the radio */
#define ALL_FRAMES_TYPE_MASK        (ZLL_RX_FRAME_FILTER_BEACON_FT_MASK | \
                                     ZLL_RX_FRAME_FILTER_DATA_FT_MASK    | \
                                     ZLL_RX_FRAME_FILTER_CMD_FT_MASK     | \
                                     ZLL_RX_FRAME_FILTER_ACK_FT_MASK)

/* IRQ mask: RF (0-6), WAKE (8), TMR_IRQ (16-19), TMR_MSK (20-23) */
#define KW41ZRF_IRQS    (0x00ff017fUL)

static kw41zrf_t intern_dev;
static const ieee802154_radio_ops_t kw41zrf_ops;
static ieee802154_dev_t *_kw41zrf_hal_dev;

static int _request_on(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[kw41zrf] request_on\n");
    kw41zrf_set_power_mode(&intern_dev, KW41ZRF_POWER_IDLE);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[kw41zrf] confirm_on\n");
    if (kw41zrf_is_dsm()) {
        return -EAGAIN;
    }
    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[kw41zrf] off\n");
    if (!kw41zrf_is_dsm()) {
        kw41zrf_set_power_mode(&intern_dev, KW41ZRF_POWER_DSM);
    }
    return 0;
}

static int _write(ieee802154_dev_t *hal, const iolist_t *psdu)
{
    DEBUG("[kw41zrf] write\n");
    (void)hal;
    uint8_t len = 0;
    uint8_t *data = psdu->iol_base;

    /* Indicate if an ack was requested to handle auto ack */
    if (*data & IEEE802154_FCF_ACK_REQ) {
        bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RXACKRQD_SHIFT);
    }
    else {
        bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RXACKRQD_SHIFT);
    }

    /* load packet data into buffer */
    for (const iolist_t *iol = psdu; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if ((len + iol->iol_len) > (KW41ZRF_MAX_PKT_LENGTH - IEEE802154_FCS_LEN)) {
            return -EOVERFLOW;
        }
        memcpy(&((uint8_t *)ZLL->PKT_BUFFER_TX)[len + 1], iol->iol_base, iol->iol_len);
        len += iol->iol_len;
    }

    /*
     * First octet in the TX buffer contains the frame length.
     * Nbytes = FRAME_LEN - 2 -> FRAME_LEN = Nbytes + 2
     * MKW41Z ref. man. 44.6.2.6.3.1.3 Sequence T (Transmit), p. 2147
     */
    *((volatile uint8_t *)&ZLL->PKT_BUFFER_TX[0]) = len + IEEE802154_FCS_LEN;

#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump((const uint8_t *)ZLL->PKT_BUFFER_TX, len, OD_WIDTH_DEFAULT);
#endif

    return 0;
}

static int _len(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[kw41zrf] len\n");
    uint32_t len = ((ZLL->IRQSTS & ZLL_IRQSTS_RX_FRAME_LENGTH_MASK) >>
                    ZLL_IRQSTS_RX_FRAME_LENGTH_SHIFT) - IEEE802154_FCS_LEN;
    return (int)len;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    DEBUG("[kw41zrf] read\n");

    if (!buf) {
        return 0;
    }

    uint8_t pkt_len = _len(hal);
    pkt_len = size < pkt_len ? size : pkt_len;

#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump((const uint8_t *)ZLL->PKT_BUFFER_RX, pkt_len, OD_WIDTH_DEFAULT);
#endif

    /* Read packet buffer. */
    /* Don't use memcpy to work around a presumed compiler bug in
     * arm-none-eabi-gcc 7.3.1 2018-q2-6 */
    for (int i = 0; i < pkt_len; i++) {
        ((uint8_t *)buf)[i] = ((uint8_t *)ZLL->PKT_BUFFER_RX)[i];
    }

    if (info) {
        info->lqi = kw41zrf_get_lqi_value(&intern_dev);
        info->rssi = kw41zrf_get_rssi_value(&intern_dev);
    }

    return pkt_len;
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    (void)hal;
    int res = -ENOTSUP;
    uint8_t state = kw41zrf_get_sequence(&intern_dev);

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        DEBUG("[kw41zrf] request_op TX\n");
        if (state != XCVSEQ_IDLE) {
            return -EBUSY;
        }

        if (kw41zrf_ack_requested(&intern_dev)) {
            kw41zrf_set_sequence(&intern_dev, XCVSEQ_TX_RX | ZLL_PHY_CTRL_TC3TMOUT_MASK);
        }
        else {
            kw41zrf_set_sequence(&intern_dev, XCVSEQ_TRANSMIT);
        }
        return 0;
    case IEEE802154_HAL_OP_SET_RX:
        DEBUG("[kw41zrf] request_op RX\n");

        if (state == XCVSEQ_RECEIVE) {
            return 0;
        }

        if (state != XCVSEQ_IDLE) {
            return -EBUSY;
        }

        kw41zrf_set_sequence(&intern_dev, XCVSEQ_RECEIVE);
        return 0;
    case IEEE802154_HAL_OP_SET_IDLE:
        DEBUG("[kw41zrf] request_op IDLE\n");

        assert(ctx);
        bool force = *((bool *)ctx);

        if (state == XCVSEQ_IDLE) {
            return 0;
        }

        if (force) {
            /* aborts any sequence by setting idle sequence */
            kw41zrf_set_sequence(&intern_dev, XCVSEQ_IDLE);
            return 0;
        }

        if (!kw41zrf_can_switch_to_idle(&intern_dev)) {
            return -EBUSY;
        }

        kw41zrf_set_sequence(&intern_dev, XCVSEQ_IDLE);
        return 0;
    case IEEE802154_HAL_OP_CCA:
        DEBUG("[kw41zrf] request_op CCA\n");

        if (state != XCVSEQ_IDLE) {
            return -EBUSY;
        }

        kw41zrf_set_sequence(&intern_dev, XCVSEQ_CCA);
        return 0;
    }
    return res;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    (void)dev;
    ieee802154_tx_info_t *info = ctx;

    bool eagain = false;
    uint8_t state = kw41zrf_get_sequence(&intern_dev);

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        DEBUG("[kw41zrf] confirm_op TX\n");
        if (state != XCVSEQ_IDLE) {
            return -EAGAIN;
        }

        if (info) {
            if (intern_dev.cca_busy) {
                intern_dev.tx_status = TX_STATUS_MEDIUM_BUSY;
            }
            info->status = intern_dev.tx_status;
        }

        return 0;
    case IEEE802154_HAL_OP_SET_RX:
        DEBUG("[kw41zrf] confirm_op RX\n");
        eagain = state != XCVSEQ_RECEIVE;
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        DEBUG("[kw41zrf] confirm_op IDLE\n");
        eagain = state != XCVSEQ_IDLE;
        break;
    case IEEE802154_HAL_OP_CCA:
        DEBUG("[kw41zrf] confirm_op CCA\n");
        eagain = state != XCVSEQ_IDLE;
        assert(ctx);

        *((bool *)ctx) = intern_dev.cca_busy;
        break;
    default:
        assert(false);
        return -ENOTSUP;
    }

    if (eagain) {
        return -EAGAIN;
    }

    return 0;
}


int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void)dev;
    uint8_t cca_type = 0;

    DEBUG("[kw41zrf] set_cca_mode\n");

    switch (mode) {
    case IEEE802154_CCA_MODE_ED_THRESHOLD:
        cca_type = KW41ZRF_CCATYPE_MODE1;
        break;
    case IEEE802154_CCA_MODE_CARRIER_SENSING:
        cca_type = KW41ZRF_CCATYPE_MODE2;
        break;
    case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
        cca_type = KW41ZRF_CCATYPE_MODE3;
        kw41zrf_set_cca3_mode(0);
        break;
    case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
        cca_type = KW41ZRF_CCATYPE_MODE3;
        kw41zrf_set_cca3_mode(1);
        break;
    default:
        return -ENOTSUP;
    }
    kw41zrf_set_cca_type(&intern_dev, cca_type);
    return 0;
}

int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void)dev;
    DEBUG("[kw41zrf] set_cca_threshold\n");
    kw41zrf_set_cca_threshold(&intern_dev, threshold);
    return 0;
}

int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd, int8_t retries)
{
    (void)bd;
    (void)dev;

    DEBUG("[kw41zrf] set_csma_params\n");

    if (retries < 0) {
        bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CCABFRTX_SHIFT);
        return 0;
    }
    else if (retries == 0) {
        bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CCABFRTX_SHIFT);
        return 0;
    }

    return -EINVAL;
}

int _set_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t mode)
{
    (void)hal;

    DEBUG("[kw41zrf] set_frame_filter_mode\n");

    switch (mode) {
    case IEEE802154_FILTER_ACCEPT:
        bit_clear32(&ZLL->RX_FRAME_FILTER, ZLL_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS_SHIFT);
        bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CRC_MSK_SHIFT);

        ZLL->RX_FRAME_FILTER = (ZLL->RX_FRAME_FILTER & ~ALL_FRAMES_TYPE_MASK) |
                               ALL_FRAMES_TYPE_MASK;
        intern_dev.filter_mode = IEEE802154_FILTER_ACCEPT;
        break;

    case IEEE802154_FILTER_ACK_ONLY:
        bit_clear32(&ZLL->RX_FRAME_FILTER, ZLL_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS_SHIFT);
        bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CRC_MSK_SHIFT);

        ZLL->RX_FRAME_FILTER = (ZLL->RX_FRAME_FILTER & ~ALL_FRAMES_TYPE_MASK)
                               | ZLL_RX_FRAME_FILTER_ACK_FT_MASK;

        intern_dev.filter_mode = IEEE802154_FILTER_ACK_ONLY;
        break;

    case IEEE802154_FILTER_PROMISC:
        bit_set32(&ZLL->RX_FRAME_FILTER, ZLL_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS_SHIFT);

        intern_dev.filter_mode = IEEE802154_FILTER_PROMISC;
        break;

    case IEEE802154_FILTER_SNIFFER:
        bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_PROMISCUOUS_SHIFT);
        bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CRC_MSK_SHIFT);

        intern_dev.filter_mode = IEEE802154_FILTER_SNIFFER;
        break;

    default:
        return -ENOTSUP;
    }
    return 0;
}

static int _get_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t *mode)
{
    (void)hal;
    DEBUG("[kw41zrf] get_frame_filter_mode\n");
    *mode = intern_dev.filter_mode;
    return 0;
}

int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    (void)dev;

    DEBUG("[kw41zrf] config_addr_filter\n");

    switch (cmd) {
    case IEEE802154_AF_SHORT_ADDR:
        kw41zrf_set_addr_short(&intern_dev, value);
        break;
    case IEEE802154_AF_EXT_ADDR:
        eui64_t addr;
        addr.uint64.u64 = byteorder_swapll(*(uint64_t *)value);
        kw41zrf_set_addr_long(&intern_dev, &addr);
        break;
    case IEEE802154_AF_PANID:
        kw41zrf_set_pan(&intern_dev, *((const uint16_t *)value));
        break;
    case IEEE802154_AF_PAN_COORD:
        kw41zrf_set_pan_coord(*((const uint8_t *)value));
        break;
    }
    return 0;
}

uint16_t _calc_short_addr_checksum(uint16_t short_addr, uint16_t dest_pan_id)
{
    return (short_addr + dest_pan_id) & CHECKSUM_MASK;
}

uint16_t _calc_ext_addr_checksum(const eui64_t *ext_addr, uint16_t dest_pan_id)
{
    uint16_t checksum = 0;

    checksum = (ext_addr->uint16[0].u16 + dest_pan_id) & CHECKSUM_MASK;
    checksum = (checksum + ext_addr->uint16[1].u16) & CHECKSUM_MASK;
    checksum = (checksum + ext_addr->uint16[2].u16) & CHECKSUM_MASK;
    checksum = (checksum + ext_addr->uint16[3].u16) & CHECKSUM_MASK;
    return checksum;
}

void _update_next_free_idx(void)
{
    DEBUG("[kw41zrf] update_next_free_idx\n");
    bit_set32(&ZLL->SAM_TABLE, ZLL_SAM_TABLE_FIND_FREE_IDX_SHIFT);
    /* wait while hw updating next free index */
    while (ZLL->SAM_TABLE & ZLL_SAM_TABLE_SAM_BUSY_MASK) {}

}

int _add_to_sam_table(uint16_t checksum)
{
    uint8_t free_idx = kw41zrf_get_1st_free_idx_sap0();

    DEBUG("[kw41zrf] add_to_sam_table\n");

    if (kw41zrf_get_partition_start(ZLL_SAM_CTRL_SAA0_START_MASK,
                                    ZLL_SAM_CTRL_SAA0_START_SHIFT) >= free_idx) {
        return -ENOSPC;
    }

    uint32_t entry = (checksum << ZLL_SAM_TABLE_SAM_CHECKSUM_SHIFT)
                     | (free_idx << ZLL_SAM_TABLE_SAM_INDEX_SHIFT)
                     | (1 << ZLL_SAM_TABLE_SAM_INDEX_EN_SHIFT)
                     | (0 << ZLL_SAM_TABLE_SAM_INDEX_INV_SHIFT)
                     | (1 << ZLL_SAM_TABLE_SAM_INDEX_WR_SHIFT);

    ZLL->SAM_TABLE = entry;
    return 0;
}

int _remove_from_sam_table(uint16_t checksum)
{
    uint8_t end_idx = kw41zrf_get_partition_start(ZLL_SAM_CTRL_SAA0_START_MASK,
                                                  ZLL_SAM_CTRL_SAA0_START_SHIFT);

    DEBUG("[kw41zrf] remove_from_sam_table\n");

    for (uint8_t idx = 0; idx < end_idx; idx++) {

        ZLL->SAM_TABLE = (idx << ZLL_SAM_TABLE_SAM_INDEX_SHIFT) | (1 <<
                                                                   ZLL_SAM_TABLE_SAM_INDEX_WR_SHIFT);
        uint16_t entry_checksum = ZLL->SAM_TABLE & ZLL_SAM_TABLE_SAM_CHECKSUM_MASK;

        if (checksum == entry_checksum) {
            ZLL->SAM_TABLE = (idx << ZLL_SAM_TABLE_SAM_INDEX_SHIFT)
                             | (0 << ZLL_SAM_TABLE_SAM_INDEX_EN_SHIFT)
                             | (1 << ZLL_SAM_TABLE_SAM_INDEX_INV_SHIFT)
                             | (0 << ZLL_SAM_TABLE_SAM_INDEX_WR_SHIFT);

            _update_next_free_idx();
            return 0;
        }
    }

    return -ENOENT;
}

int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd, const void *value)
{
    (void)dev;
    uint16_t pan_id = kw41zrf_get_pan(&intern_dev);
    uint16_t checksum = 0;
    eui64_t *ext_addr = 0;
    network_uint16_t *short_addr = 0;

    DEBUG("[kw41zrf] config_src_addr_match\n");

    switch (cmd) {
    case IEEE802154_SRC_MATCH_EN:
        const bool enable = *((const bool *)value);
        if (enable) {
            bit_set32(&ZLL->SAM_TABLE, ZLL_SAM_TABLE_ACK_FRM_PND_SHIFT);
        }
        else {
            bit_clear32(&ZLL->SAM_TABLE, ZLL_SAM_TABLE_ACK_FRM_PND_SHIFT);
        }
        break;
    case IEEE802154_SRC_MATCH_SHORT_ADD:
        short_addr = (network_uint16_t *)value;
        checksum = _calc_short_addr_checksum(short_addr->u16, pan_id);
        return _add_to_sam_table(checksum);
    case IEEE802154_SRC_MATCH_SHORT_CLEAR:
        short_addr = (network_uint16_t *)value;
        checksum = _calc_short_addr_checksum(short_addr->u16, pan_id);
        return _remove_from_sam_table(checksum);
    case IEEE802154_SRC_MATCH_EXT_ADD:
        ext_addr = (eui64_t *)value;
        checksum = _calc_ext_addr_checksum(ext_addr, pan_id);
        return _add_to_sam_table(checksum);
    case IEEE802154_SRC_MATCH_EXT_CLEAR:
        ext_addr = (eui64_t *)value;
        checksum = _calc_ext_addr_checksum(ext_addr, pan_id);
        return _remove_from_sam_table(checksum);
    }
    return 0;
}
/* Common CCA check handler code for sequences Transmit and Transmit/Receive */
static uint32_t _isr_event_seq_t_ccairq(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;

    DEBUG("[kw41zrf] _isr_event_seq_t_ccairq\n");

    if (irqsts & ZLL_IRQSTS_CCAIRQ_MASK) {
        /* CCA before TX has completed */
        handled_irqs |= ZLL_IRQSTS_CCAIRQ_MASK;
        dev->cca_busy = (irqsts & ZLL_IRQSTS_CCA_MASK) > 0;
    }
    return handled_irqs;
}

/* Handler for standalone CCA */
static uint32_t _isr_event_seq_cca(kw41zrf_t *dev, uint32_t irqsts,
                                   bool *indicate_hal_event,
                                   ieee802154_trx_ev_t *hal_event)
{
    uint32_t handled_irqs = 0;

    DEBUG("[kw41zrf] _isr_event_seq_cca\n");

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        /* Finished CCA sequence */
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        dev->cca_busy = (irqsts & ZLL_IRQSTS_CCA_MASK) > 0;
        kw41zrf_abort_sequence(&intern_dev);

        if (_kw41zrf_hal_dev->cb) {
            *hal_event = IEEE802154_RADIO_CONFIRM_CCA;
            *indicate_hal_event = true;
        }
    }
    return handled_irqs;
}

/* Handler for Receive sequence */
static uint32_t _isr_event_seq_r(kw41zrf_t *dev, uint32_t irqsts,
                                 bool *indicate_hal_event,
                                 ieee802154_trx_ev_t *hal_event)
{
    (void)dev;
    uint32_t handled_irqs = 0;

    DEBUG("[kw41zrf] _isr_event_seq_r\n");

    if (irqsts & ZLL_IRQSTS_RXWTRMRKIRQ_MASK) {
        handled_irqs |= ZLL_IRQSTS_RXWTRMRKIRQ_MASK;
        if (_kw41zrf_hal_dev->cb) {
            *hal_event = IEEE802154_RADIO_INDICATION_RX_START;
            *indicate_hal_event = true;
            return handled_irqs; /* don't process further events on RX_START */
        }
    }

    if (irqsts & ZLL_IRQSTS_FILTERFAIL_IRQ_MASK) {
        KW41ZRF_LED_RX_OFF;
        handled_irqs |= ZLL_IRQSTS_FILTERFAIL_IRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_RXIRQ_MASK) {
        KW41ZRF_LED_RX_ON;
        handled_irqs |= ZLL_IRQSTS_RXIRQ_MASK;
        if (ZLL->PHY_CTRL & ZLL_PHY_CTRL_AUTOACK_MASK) {
            KW41ZRF_LED_TX_ON;
        }
    }

    if (irqsts & ZLL_IRQSTS_TXIRQ_MASK) {
        KW41ZRF_LED_TX_OFF;
        handled_irqs |= ZLL_IRQSTS_TXIRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        uint32_t seq_ctrl_sts = ZLL->SEQ_CTRL_STS;

        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        KW41ZRF_LED_TX_OFF;
        kw41zrf_abort_sequence(&intern_dev);

        if (!(irqsts & ZLL_IRQSTS_CRCVALID_MASK)) {
            if (_kw41zrf_hal_dev->cb) {
                *hal_event = IEEE802154_RADIO_INDICATION_CRC_ERROR;
                *indicate_hal_event = true;
            }
        }
        else if ((seq_ctrl_sts & (ZLL_SEQ_CTRL_STS_TC3_ABORTED_MASK |
                                  ZLL_SEQ_CTRL_STS_PLL_ABORTED_MASK |
                                  ZLL_SEQ_CTRL_STS_SW_ABORTED_MASK)) > 0) {
            assert(1);
        }
        /* No error reported */
        else {
            if (_kw41zrf_hal_dev->cb) {
                *hal_event = IEEE802154_RADIO_INDICATION_RX_DONE;
                *indicate_hal_event = true;
            }
        }
    }
    return handled_irqs;
}

/* Handler for Transmit sequence */
static uint32_t _isr_event_seq_t(kw41zrf_t *dev, uint32_t irqsts,
                                 bool *indicate_hal_event,
                                 ieee802154_trx_ev_t *hal_event)
{
    (void)dev;
    uint32_t handled_irqs = 0;

    DEBUG("[kw41zrf] _isr_event_seq_t\n");

    if (irqsts & ZLL_IRQSTS_TXIRQ_MASK) {
        handled_irqs |= ZLL_IRQSTS_TXIRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;

        kw41zrf_abort_sequence(&intern_dev);
        intern_dev.tx_status = TX_STATUS_SUCCESS;
        KW41ZRF_LED_TX_OFF;

        if (_kw41zrf_hal_dev->cb) {
            *hal_event = IEEE802154_RADIO_CONFIRM_TX_DONE;
            *indicate_hal_event = true;
        }
    }
    return handled_irqs;
}


/* Handler for Transmit/Receive sequence */
static uint32_t _isr_event_seq_tr(kw41zrf_t *dev, uint32_t irqsts,
                                  bool *indicate_hal_event,
                                  ieee802154_trx_ev_t *hal_event)
{
    uint32_t handled_irqs = 0;

    DEBUG("[kw41zrf] _isr_event_seq_tr\n");

    if (irqsts & ZLL_IRQSTS_TXIRQ_MASK) {
        KW41ZRF_LED_RX_ON;
        handled_irqs |= ZLL_IRQSTS_TXIRQ_MASK;
        /* TX done, now waiting for ACK - START timer NOW */
        if (kw41zrf_ack_requested(dev)) {
            /* Set timeout from current time */
            kw41zrf_timer_set(dev, &ZLL->T3CMP, IEEE802154_ACK_TIMEOUT_SYMS);
            bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_TMR3CMP_EN_SHIFT);

            DEBUG("[kw41zrf] wait for RX ACK (timeout set)\n");
        }
    }

    if (irqsts & ZLL_IRQSTS_RXIRQ_MASK) {
        KW41ZRF_LED_RX_OFF;
        handled_irqs |= ZLL_IRQSTS_RXIRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_FILTERFAIL_IRQ_MASK) {
        handled_irqs |= ZLL_IRQSTS_FILTERFAIL_IRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        uint32_t seq_ctrl_sts = ZLL->SEQ_CTRL_STS;
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        KW41ZRF_LED_TX_OFF;
        KW41ZRF_LED_RX_OFF;

        /* Disable RX Timeout */
        bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_TC3TMOUT_SHIFT);
        bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_TMR3CMP_EN_SHIFT);

            kw41zrf_abort_sequence(&intern_dev);
        if (irqsts & ZLL_IRQSTS_TMR3IRQ_MASK) {
            handled_irqs |= ZLL_IRQSTS_TMR3IRQ_MASK;
        }

        if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_PLL_ABORTED_MASK) {
            /* if this does happen in development, it's worth checking why */
            assert(false);
        }
        else if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_TC3_ABORTED_MASK) {
            intern_dev.tx_status = TX_STATUS_NO_ACK;
            if (_kw41zrf_hal_dev->cb) {
                *hal_event = IEEE802154_RADIO_CONFIRM_TX_DONE;
                *indicate_hal_event = true;
            }
        }
        else if ((seq_ctrl_sts & ZLL_SEQ_CTRL_STS_SW_ABORTED_MASK)) {
            /* Software aborted, no callback needed */
            assert(false);
        }
        else {
            /* No error reported */
            intern_dev.tx_status = intern_dev.cca_busy ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
            if (_kw41zrf_hal_dev->cb) {
                *hal_event = IEEE802154_RADIO_CONFIRM_TX_DONE;
                *indicate_hal_event = true;
            }
        }
    }
    return handled_irqs;
}

static void kw41zrf_radio_hal_irq_handler(void *dev)
{
    (void)dev;
    DEBUG("[kw41zrf] kw41zrf_radio_hal_irq_handler\n");

    kw41zrf_mask_irqs();

    /* ZLL register access requires that the transceiver is not in deep sleep mode */
    if (kw41zrf_is_dsm()) {
        /* Transceiver is sleeping, the IRQ must have occurred before entering
         * sleep, discard the call */
        kw41zrf_unmask_irqs();
        return;
    }

    uint32_t irqsts = ZLL->IRQSTS;
    /* Clear all IRQ flags now */
    ZLL->IRQSTS = irqsts;

    uint32_t handled_irqs = 0;
    bool indicate_hal_event = false;
    ieee802154_trx_ev_t hal_event;

    uint8_t seq = kw41zrf_get_sequence(&intern_dev);

    switch (seq) {
    case XCVSEQ_RECEIVE:
        handled_irqs |= _isr_event_seq_r(&intern_dev, irqsts, &indicate_hal_event, &hal_event);
        break;

    case XCVSEQ_IDLE:
        if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
            handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        }
        break;

    case XCVSEQ_TRANSMIT:
        /* First check CCA flags */
        handled_irqs |= _isr_event_seq_t_ccairq(&intern_dev, irqsts);
        /* Then TX flags */
        handled_irqs |= _isr_event_seq_t(&intern_dev, irqsts & ~handled_irqs, &indicate_hal_event,
                                         &hal_event);
        break;

    case XCVSEQ_CCA:
        handled_irqs |= _isr_event_seq_cca(&intern_dev, irqsts, &indicate_hal_event, &hal_event);
        break;

    case XCVSEQ_TX_RX:
        /* First check CCA flags */
        handled_irqs |= _isr_event_seq_t_ccairq(&intern_dev, irqsts);
        /* Then TX/RX flags */
        handled_irqs |= _isr_event_seq_tr(&intern_dev, irqsts & ~handled_irqs, &indicate_hal_event,
                                          &hal_event);
        break;

    case XCVSEQ_CONTINUOUS_CCA:
        handled_irqs |= _isr_event_seq_cca(&intern_dev, irqsts, &indicate_hal_event, &hal_event);
        break;

    default:
        assert(0);
        break;
    }

    irqsts &= ~handled_irqs;
    /* doesn't need handling; just prevent outputting an error below */
    irqsts &= ~ZLL_IRQSTS_RXWTRMRKIRQ_MASK;

    /* Enable IRQs first */
    kw41zrf_unmask_irqs();

    /* Then call the callback if needed */
    if (indicate_hal_event && _kw41zrf_hal_dev->cb) {
        _kw41zrf_hal_dev->cb(_kw41zrf_hal_dev, hal_event);
    }
}

int _config_phy(ieee802154_dev_t *hal, const ieee802154_phy_conf_t *conf)
{
    (void)hal;
    int8_t pow = conf->pow;
    uint8_t channel = conf->channel;

    DEBUG("[kw41zrf] config_phy\n");

    if (pow < KW41ZRF_OUTPUT_POWER_MIN || pow > KW41ZRF_OUTPUT_POWER_MAX) {
        return -EINVAL;
    }
    kw41zrf_set_tx_power(&intern_dev, pow);
    kw41zrf_set_channel(&intern_dev, channel);
    return 0;
}

int kw41zrf_reset(void)
{
    DEBUG("[kw41zrf] reset\n");
    kw41zrf_mask_irqs();

    /* Sometimes (maybe 1 in 30 reboots) there is a failure in the vendor
     * routines in kw41zrf_rx_bba_dcoc_dac_trim_DCest() that can be worked
     * around by retrying. Clearly this is not ideal.
     */
    for (int retries = 0; ; retries++) {
        int res = kw41zrf_reset_hardware(&intern_dev);
        if (!res) {
            break;
        }
        if (retries == KW41ZRF_MAX_BOOT_RETRIES) {
            kw41zrf_unmask_irqs();
            return res;
        }
    }

    kw41zrf_set_power_mode(&intern_dev, KW41ZRF_POWER_IDLE);
    kw41zrf_set_tx_power(&intern_dev, KW41ZRF_DEFAULT_TX_POWER);
    kw41zrf_set_channel(&intern_dev, KW41ZRF_DEFAULT_CHANNEL);
    kw41zrf_set_cca_type(&intern_dev, KW41ZRF_CCATYPE_MODE1);
    kw41zrf_set_rx_watermark(&intern_dev, 1);
    kw41zrf_set_option(&intern_dev, KW41ZRF_OPT_AUTOACK, true);
    kw41zrf_set_option(&intern_dev, KW41ZRF_OPT_CCA_BEFORE_TX, true);
    kw41zrf_abort_sequence(&intern_dev);
    kw41zrf_unmask_irqs();

    return 0;
}

int kw41zrf_init(void)
{
    DEBUG("[kw41zrf] init\n");

    /* Save a copy of the RF_OSC_EN setting to use when the radio is in deep sleep */
    intern_dev.rf_osc_en_idle = RSIM->CONTROL & RSIM_CONTROL_RF_OSC_EN_MASK;
    intern_dev.pm_blocked = 0;
    kw41zrf_mask_irqs();
    kw41zrf_set_irq_callback(kw41zrf_radio_hal_irq_handler, &intern_dev);

    /* Perform clean reset of the radio modules. */
    int res = kw41zrf_reset();
    if (res < 0) {
        /* Restore saved RF_OSC_EN setting */
        RSIM->CONTROL = (RSIM->CONTROL & ~RSIM_CONTROL_RF_OSC_EN_MASK) | intern_dev.rf_osc_en_idle;
        return res;
    }
    /* Radio is now on and idle */

    /* Allow radio interrupts */
    kw41zrf_unmask_irqs();

    bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RX_WMRK_MSK_SHIFT);
    bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_TXMSK_SHIFT);
    return 0;
}


void kw41zrf_hal_setup(ieee802154_dev_t *hal)
{
    DEBUG("[kw41zrf] hal_setup\n");
    /* We don't set &intern_dev because the context of this device is global */
    /* We need to store a reference to the HAL descriptor though for the ISR */
    hal->driver = &kw41zrf_ops;
    _kw41zrf_hal_dev = hal;
}

static const ieee802154_radio_ops_t kw41zrf_ops = {
    .caps = IEEE802154_CAP_24_GHZ
            | IEEE802154_CAP_PHY_OQPSK
            | IEEE802154_CAP_IRQ_CRC_ERROR
            | IEEE802154_CAP_IRQ_TX_DONE
            | IEEE802154_CAP_IRQ_RX_START
            | IEEE802154_CAP_IRQ_CCA_DONE
            | IEEE802154_CAP_IRQ_ACK_TIMEOUT
            | IEEE802154_CAP_AUTO_ACK
            | IEEE802154_CAP_SRC_ADDR_MATCH,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
    .get_frame_filter_mode = _get_frame_filter_mode,
};
