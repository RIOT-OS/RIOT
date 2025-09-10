/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 Radio HAL implementation for the CC2538 RF driver
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "net/gnrc.h"

#include "cc2538_rf.h"
#include "cc2538_rf_internal.h"

#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const ieee802154_radio_ops_t cc2538_rf_ops;

static ieee802154_dev_t *cc2538_rf_hal;

typedef enum {
    CC2538_STATE_READY,             /**< The radio is ready to receive requests */
    CC2538_STATE_TRX_TRANSITION,    /**< There's a pending TRX state transition */
    CC2538_STATE_CONFIRM_TX,        /**< Transmission finished and waiting for confirm */
    CC2538_STATE_TX_BUSY,           /**< The radio is busy transmitting */
    CC2538_STATE_TX_ACK,            /**< The radio is currently transmitting an ACK frame */
    CC2538_STATE_CCA,               /**< The radio is doing CCA */
    CC2538_STATE_CONFIRM_CCA,       /**< CCA finished and waiting for confirm */
} cc2538_state_t;

static cc2538_state_t cc2538_state;
static uint8_t cc2538_min_be = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE;
static uint8_t cc2538_max_be = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE;
static int cc2538_csma_ca_retries = CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES;

static bool cc2538_cca_status;  /**< status of the last CCA request */

static void _enable_rx(void)
{
    RFCORE_XREG_FRMCTRL0 &= ~CC2538_FRMCTRL0_RX_MODE_DIS;
}

static void _disable_rx(void)
{
    RFCORE_XREG_FRMCTRL0 |= CC2538_FRMCTRL0_RX_MODE_DIS;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    (void) dev;
    int pkt_len = 0;
    RFCORE_SFR_RFST = ISFLUSHTX;
    rfcore_write_byte(0);

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len) {
            pkt_len += iol->iol_len;
            rfcore_write_fifo(iol->iol_base, iol->iol_len);
        }
    }

    /* Set first byte of TX FIFO to the packet length */
    rfcore_poke_tx_fifo(0, pkt_len + CC2538_AUTOCRC_LEN);
    return 0;
}
static int _request_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    (void) dev;
    int res = -EBUSY;
    cc2538_rf_disable_irq();
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        assert(cc2538_state != CC2538_STATE_TX_BUSY);
        cc2538_state = CC2538_STATE_TX_BUSY;
        if (cc2538_csma_ca_retries < 0) {
            RFCORE_SFR_RFST = ISTXON;
            /* The CPU Ctrl mask is used here to indicate whether the radio is being
             * controlled by the CPU or the CSP Strobe Processor.
             * We set this to 1 in order to indicate that the CSP is not used and
             * thus, that the @ref ieee802154_radio_confirm_transmit should
             * return 0 immediately after the TXDONE event
             */
            RFCORE_XREG_CSPCTRL |= CC2538_CSP_MCU_CTRL_MASK;
        }
        else {
            /* Disable RX Chain for CCA (see CC2538 RM, Section 29.9.5.3) */
            _disable_rx();
            RFCORE_SFR_RFST = ISRXON;
            /* Clear last program */
            RFCORE_SFR_RFST = ISCLEAR;

            /* If the RSSI is not yet valid, skip 0 instructions. This creates
             * a busy loop until the RSSI is valid. */
            RFCORE_SFR_RFST = SKIP_S_C
                              | CC2538_CSP_SKIP_N_MASK
                              | CC2538_CSP_SKIP_COND_RSSI;

            /* Set a label right before the backoff */
            RFCORE_SFR_RFST = LABEL;

            /* Load a random number with "register Y" LSBs into register X.
             * This is equivalent to choosing a random number between
             * (0, 2^(Y+1)).
             * Then, wait "register X" number of backoff units */
            RFCORE_SFR_RFST = RANDXY;
            RFCORE_SFR_RFST = WAITX;

            /* If CCA is not valid, skip the next stop instruction.  In such case
             * the CSP_STOP interrupt will trigger the transmission since the
             * channel is clear */
            RFCORE_SFR_RFST = SKIP_S_C
                              | (1 << CC2538_CSP_SKIP_INST_POS)
                              | CC2538_CSP_SKIP_N_MASK
                              | CC2538_CSP_SKIP_COND_CCA;

            RFCORE_SFR_RFST = STOP;

            /* If we are here, the channel was not clear. Decrement the register Z
             * (remaining attempts) */
            RFCORE_SFR_RFST = DECZ;

            /* Update the backoff exponent */
            RFCORE_SFR_RFST = INCMAXY | (cc2538_max_be & CC2538_CSP_INCMAXY_MAX_MASK);

            /* If the are CSMA-CA retries left, go back to the defined label */
            RFCORE_SFR_RFST = RPT_C
                              | CC2538_CSP_SKIP_N_MASK
                              | CC2538_CSP_SKIP_COND_CSPZ;

            /* Stop the program. The CSP_STOP interrupt will trigger the routine
             * to inform the upper layer that CSMA-CA failed. */
            RFCORE_SFR_RFST = STOP;

            RFCORE_XREG_CSPX = 0; /* Holds timer value */
            RFCORE_XREG_CSPY = cc2538_min_be; /* Holds MinBE */

            assert(cc2538_csma_ca_retries >= 0);
            RFCORE_XREG_CSPZ = cc2538_csma_ca_retries + 1; /* Holds CSMA-CA attempts (retries + 1) */

            RFCORE_XREG_CSPCTRL &= ~CC2538_CSP_MCU_CTRL_MASK;

            /* Execute the program */
            RFCORE_SFR_RFST = ISSTART;
        }
        cc2538_rf_enable_irq();
        break;
    case IEEE802154_HAL_OP_SET_RX:
        if (cc2538_state != CC2538_STATE_READY) {
            goto error;
        }
        cc2538_state = CC2538_STATE_TRX_TRANSITION;
        /* Enable RX Chain */
        _enable_rx();
        RFCORE_SFR_RFST = ISRXON;
        RFCORE_SFR_RFIRQF0 &= ~RXPKTDONE;
        RFCORE_SFR_RFIRQF0 &= ~SFD;
        /* We keep the interrupts disabled until the state transition is
         * finished */
        break;
    case IEEE802154_HAL_OP_SET_IDLE: {
        assert(ctx);
        bool force = *((bool*) ctx);

        if (!force && cc2538_state != CC2538_STATE_READY) {
            goto error;
        }

        cc2538_state = CC2538_STATE_TRX_TRANSITION;
        if (RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE != FSM_STATE_IDLE) {
            RFCORE_SFR_RFST = ISRFOFF;
        }
        RFCORE_SFR_RFIRQF0 &= ~RXPKTDONE;
        RFCORE_SFR_RFIRQF0 &= ~SFD;
        /* We keep the interrupts disabled until the state transition is
         * finished */
        break;
    }
    case IEEE802154_HAL_OP_CCA:
        /* Ignore baseband processing */
        _disable_rx();
        RFCORE_SFR_RFIRQF0 &= ~RXPKTDONE;
        RFCORE_SFR_RFIRQF0 &= ~SFD;
        RFCORE_SFR_RFST = ISRXON;
        RFCORE_SFR_RFST = ISCLEAR;
        RFCORE_SFR_RFST = STOP;
        RFCORE_XREG_CSPCTRL &= ~CC2538_CSP_MCU_CTRL_MASK;
        /* Execute the last program */
        RFCORE_SFR_RFST = ISSTART;
        cc2538_rf_enable_irq();
        break;
    }

    res = 0;
    return res;

error:
    cc2538_rf_enable_irq();
    return res;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    int res = -EAGAIN;
    (void) dev;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        cc2538_rf_disable_irq();
        if (cc2538_state != CC2538_STATE_CONFIRM_TX) {
            goto error;
        }

        if (ctx) {
            ieee802154_tx_info_t *info = ctx;

            if (cc2538_csma_ca_retries >= 0 && RFCORE_XREG_CSPZ == 0) {
                info->status = TX_STATUS_MEDIUM_BUSY;
            }
            else {
                info->status = TX_STATUS_SUCCESS;
            }
        }
        break;
    case IEEE802154_HAL_OP_SET_RX:
    case IEEE802154_HAL_OP_SET_IDLE:
        /* IRQ is already disabled here */
        assert(cc2538_state == CC2538_STATE_TRX_TRANSITION);
        break;
    case IEEE802154_HAL_OP_CCA:
        assert(ctx);
        cc2538_rf_disable_irq();
        assert(cc2538_state == CC2538_STATE_CCA || cc2538_state == CC2538_STATE_CONFIRM_CCA);
        if (cc2538_state != CC2538_STATE_CONFIRM_CCA) {
            goto error;
        }

        _enable_rx();
        *((bool*) ctx) = cc2538_cca_status;
        break;
    }

    cc2538_state = CC2538_STATE_READY;
    res = 0;

error:
    cc2538_rf_enable_irq();
    return res;

}

static int _len(ieee802154_dev_t *dev)
{
    (void) dev;
    return rfcore_peek_rx_fifo(0) - IEEE802154_FCS_LEN;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    (void) dev;
    int res;
    size_t pkt_len;

    if (!buf) {
        res = 0;
        goto end;
    }

    /* The upper layer shouldn't call this function if the RX_DONE event was
     * not triggered */
    if (!(RFCORE_XREG_RXFIFOCNT > 0)) {
        assert(false);
    }

    pkt_len = rfcore_read_byte() - IEEE802154_FCS_LEN;
    if (pkt_len > size) {
        res = -ENOBUFS;
        goto end;
    }

    rfcore_read_fifo(buf, pkt_len);
    res = pkt_len;
    if (info != NULL) {
        uint8_t corr_val;
        int8_t rssi_val;
        rssi_val = rfcore_read_byte();

        /* The number of dB above maximum sensitivity detected for the
         * received packet */
        /* Make sure there is no overflow even if no signal with such
           low sensitivity should be detected */
        const int hw_rssi_min = IEEE802154_RADIO_RSSI_OFFSET -
                                CC2538_RSSI_OFFSET;
        int8_t hw_rssi = rssi_val > hw_rssi_min ?
            (CC2538_RSSI_OFFSET + rssi_val) : IEEE802154_RADIO_RSSI_OFFSET;
        info->rssi = hw_rssi - IEEE802154_RADIO_RSSI_OFFSET;

        corr_val = rfcore_read_byte() & CC2538_CORR_VAL_MASK;
        if (corr_val < CC2538_CORR_VAL_MIN) {
            corr_val = CC2538_CORR_VAL_MIN;
        }
        else if (corr_val > CC2538_CORR_VAL_MAX) {
            corr_val = CC2538_CORR_VAL_MAX;
        }

        /* Interpolate the correlation value between 0 - 255
         * to provide an LQI value */
        info->lqi = 255 * (corr_val - CC2538_CORR_VAL_MIN) /
                          (CC2538_CORR_VAL_MAX - CC2538_CORR_VAL_MIN);
    }

end:
    /* We don't need to enable RX chain here, since the upper layer already
     * made sure the transceiver is in IDLE. We simply flush the RX buffer */
    RFCORE_SFR_RFST = ISFLUSHRX;
    return res;
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void) dev;
    (void) threshold;

    if (threshold < CC2538_RF_SENSITIVITY) {
        return -EINVAL;
    }

    RFCORE_XREG_CCACTRL0 &= ~CC2538_CCA_THR_MASK;
    RFCORE_XREG_CCACTRL0 |= (threshold - CC2538_RSSI_OFFSET) & CC2538_CCA_THR_MASK;

    return 0;
}

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    (void) dev;
    int8_t pow = conf->pow;
    if (pow  < OUTPUT_POWER_MIN || pow > OUTPUT_POWER_MAX) {
        return -EINVAL;
    }

    uint8_t channel = conf->channel;
    cc2538_set_freq(IEEE802154_CHAN2FREQ(channel));
    cc2538_set_tx_power(pow);

    return 0;
}

void cc2538_irq_handler(void)
{
    uint_fast8_t flags_f0 = RFCORE_SFR_RFIRQF0;
    uint_fast8_t flags_f1 = RFCORE_SFR_RFIRQF1;

    uint8_t handled_f0 = 0;
    uint8_t handled_f1 = 0;

    assert(cc2538_state != CC2538_STATE_TRX_TRANSITION);

    if (flags_f1 & TXDONE) {
        handled_f1 |= TXDONE;
        /* TXDONE marks the end of the TX chain. The radio is not busy anymore */
        assert(cc2538_state == CC2538_STATE_TX_BUSY);
        cc2538_state = CC2538_STATE_CONFIRM_TX;
        cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }

    /* The RX chain is not busy anymore on TXACKDONE event */
    if (flags_f1 & TXACKDONE) {
        handled_f1 |= TXACKDONE;
        assert(cc2538_state == CC2538_STATE_TX_ACK);
        cc2538_state = CC2538_STATE_READY;
    }

    if ((flags_f0 & SFD)) {
        handled_f0 |= SFD;
        switch(cc2538_state) {
        case CC2538_STATE_READY:
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_RX_START);
            break;
        case CC2538_STATE_TX_BUSY:
            /* If the radio already transmitted, this SFD is the TX_START event */
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_TX_START);
            break;
        case CC2538_STATE_TX_ACK:
            /* The detected SFD comes from the transmitted ACK frame. Simply
             * ignore it */
            break;
        default:
            /* This should never happen */
            DEBUG("ERROR: cc2538_state: %i\n", cc2538_state);
            assert(false);
        }
    }

    if (flags_f0 & RXPKTDONE) {
        handled_f0 |= RXPKTDONE;
        /* CRC_OK bit is located in the last byte of the packet.
         * The radio masks the length byte before filling the FIFO with the
         * corresponding number of bytes. */
        uint8_t pkt_len = (rfcore_peek_rx_fifo(0) & CC2538_LENGTH_BYTE_MASK);
        if (rfcore_peek_rx_fifo(pkt_len) & CC2538_CRC_BIT_MASK) {
            /* Disable RX while the frame has not been processed */
            _disable_rx();
            /* If AUTOACK is disabled or the ACK request bit is not set */
            if (IS_ACTIVE(CONFIG_IEEE802154_AUTO_ACK_DISABLE) ||
                (!(rfcore_peek_rx_fifo(1) & IEEE802154_FCF_ACK_REQ))) {
                /* The radio won't send an ACK. Therefore the RX chain is not
                 * busy anymore
                 */
                cc2538_state = CC2538_STATE_READY;
            }
            else {
                cc2538_state = CC2538_STATE_TX_ACK;
            }
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        else {
            /* Disable RX while the frame has not been processed */
            /* CRC failed; discard packet. The RX chain is not busy anymore */
            cc2538_state = CC2538_STATE_READY;
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_CRC_ERROR);
        }
    }

    /* Check if the interrupt was triggered because the CSP finished its routine
     * (CSMA-CA or CCA request)
     */
    if (flags_f1 & CSP_STOP) {
        handled_f1 |= CSP_STOP;
        RFCORE_XREG_CSPCTRL |= CC2538_CSP_MCU_CTRL_MASK;
        switch (cc2538_state) {
        case CC2538_STATE_TX_BUSY:
            if (RFCORE_XREG_CSPZ > 0) {
                RFCORE_SFR_RFST = ISTXON;
            }
            else {
                /* In case of CCA failure the TX chain is not busy anymore */
                cc2538_state = CC2538_STATE_CONFIRM_TX;
                cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
            }
            break;
        case CC2538_STATE_CCA:
            cc2538_cca_status = BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA)
                                && RFCORE->XREG_RSSISTATbits.RSSI_VALID;
            cc2538_state = CC2538_STATE_CONFIRM_CCA;
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_CONFIRM_CCA);
            break;
        default:
            /* This should never happen */
            DEBUG("ERROR: cc2538_state: %i\n", cc2538_state);
            assert(false);
            break;
        }
    }

    RFCORE_SFR_RFIRQF0 &= ~handled_f0;
    RFCORE_SFR_RFIRQF1 &= ~handled_f1;
}

static int _off(ieee802154_dev_t *dev)
{
    (void) dev;
    return -ENOTSUP;
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    (void) dev;
    const network_uint16_t *short_addr = value;
    const eui64_t *ext_addr = value;
    const uint16_t *pan_id = value;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            RFCORE_FFSM_SHORT_ADDR0 = short_addr->u8[1];
            RFCORE_FFSM_SHORT_ADDR1 = short_addr->u8[0];
            break;
        case IEEE802154_AF_EXT_ADDR:
            RFCORE_FFSM_EXT_ADDR0 = ext_addr->uint8[7];
            RFCORE_FFSM_EXT_ADDR1 = ext_addr->uint8[6];
            RFCORE_FFSM_EXT_ADDR2 = ext_addr->uint8[5];
            RFCORE_FFSM_EXT_ADDR3 = ext_addr->uint8[4];
            RFCORE_FFSM_EXT_ADDR4 = ext_addr->uint8[3];
            RFCORE_FFSM_EXT_ADDR5 = ext_addr->uint8[2];
            RFCORE_FFSM_EXT_ADDR6 = ext_addr->uint8[1];
            RFCORE_FFSM_EXT_ADDR7 = ext_addr->uint8[0];
            break;
        case IEEE802154_AF_PANID:
            RFCORE_FFSM_PAN_ID0 = *pan_id;
            RFCORE_FFSM_PAN_ID1 = (*pan_id) >> 8;
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }

    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd, const void *value)
{
    (void) dev;
    switch(cmd) {
        case IEEE802154_SRC_MATCH_EN:
            RFCORE_XREG_FRMCTRL1 &= ~CC2538_FRMCTRL1_PENDING_OR_MASK;
            if (*((const bool*) value) == true) {
                RFCORE_XREG_FRMCTRL1 |= CC2538_FRMCTRL1_PENDING_OR_MASK;
            }
            break;
        default:
            return -ENOTSUP;
    }
    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void) dev;
    /* TODO */
    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    (void) dev;
    /* TODO */
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void) dev;
    uint8_t tmp = 0;
    switch (mode) {
        case IEEE802154_CCA_MODE_ED_THRESHOLD:
            tmp = 1;
            break;
        case IEEE802154_CCA_MODE_CARRIER_SENSING:
            tmp = 2;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
            tmp = 3;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
            return -ENOTSUP;
    }

    RFCORE_XREG_CCACTRL1 &= CC2538_CCA_MODE_MASK;
    RFCORE_XREG_CCACTRL1 |= (tmp << CC2538_CCA_MODE_POS);

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd, int8_t retries)
{
    (void) dev;
    if (bd) {
        cc2538_min_be = bd->min;
        cc2538_max_be = bd->max;
    }

    cc2538_csma_ca_retries = retries;

    return 0;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    (void) dev;

    uint8_t flags = 0;
    bool promisc = false;
    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            flags =   CC2538_ACCEPT_FT_0_BEACON
                    | CC2538_ACCEPT_FT_1_DATA
                    | CC2538_ACCEPT_FT_3_CMD;
            break;
        case IEEE802154_FILTER_PROMISC:
            promisc = true;
            break;
        case IEEE802154_FILTER_ACK_ONLY:
            flags = CC2538_ACCEPT_FT_2_ACK;
            break;
        default:
            return -ENOTSUP;
    }

    RFCORE_XREG_FRMFILT1 |= flags;
    cc2538_set_monitor(promisc);

    return 0;

}

void cc2538_rf_hal_setup(ieee802154_dev_t *hal)
{
    /* We don't set hal->priv because the context of this device is global */
    /* We need to store a reference to the HAL descriptor though for the ISR */
    hal->driver = &cc2538_rf_ops;
    cc2538_rf_hal = hal;
}

static const ieee802154_radio_ops_t cc2538_rf_ops = {
    .caps = IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_AUTO_ACK
          | IEEE802154_CAP_AUTO_CSMA
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_START
          | IEEE802154_CAP_PHY_OQPSK,

    .write = _write,
    .read = _read,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_csma_params = _set_csma_params,
    .set_frame_filter_mode = _set_frame_filter_mode,
};
