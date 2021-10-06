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

static const ieee802154_radio_ops_t cc2538_rf_ops;

static ieee802154_dev_t *cc2538_rf_hal;

static bool cc2538_tx_busy;     /**< used to indicate TX chain is busy */
static bool cc2538_rx_busy;     /**< used to indicate RX chain is busy */

static uint8_t cc2538_min_be = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE;
static uint8_t cc2538_max_be = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE;
static int cc2538_csma_ca_retries = CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES;

static bool cc2538_cca_status;  /**< status of the last CCA request */
static bool cc2538_cca;         /**< used to check whether the last CCA result
                                     corresponds to a CCA request or send with
                                     CSMA-CA */

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

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    (void) dev;

    if (cc2538_tx_busy) {
        return -EAGAIN;
    }

    if (info) {
        if (cc2538_csma_ca_retries >= 0 && RFCORE_XREG_CSPZ == 0) {
            info->status = TX_STATUS_MEDIUM_BUSY;
        }
        else {
            info->status = TX_STATUS_SUCCESS;
        }
    }

    return 0;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    (void) dev;

    cc2538_tx_busy = true;
    if (cc2538_csma_ca_retries < 0) {
        RFCORE_SFR_RFST = ISTXON;
        /* The CPU Ctrl mask is used here to indicate whether the radio is being
         * controlled by the CPU or the CSP Strobe Processor.
         * We set this to 1 in order to indicate that the CSP is not used and
         * thus, that the @ref ieee802154_radio_ops::confirm_transmit should
         * return 0 immediately after the TXDONE event
         */
        RFCORE_XREG_CSPCTRL |= CC2538_CSP_MCU_CTRL_MASK;
    }
    else {
        cc2538_cca = false;

        /* Disable RX Chain for CCA (see CC2538 RM, Section 29.9.5.3) */
        RFCORE_XREG_FRMCTRL0 |= CC2538_FRMCTRL0_RX_MODE_DIS;
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
    return 0;
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
    /* Enable RX Chain */
    RFCORE_XREG_FRMCTRL0 &= ~CC2538_FRMCTRL0_RX_MODE_DIS;
    RFCORE_SFR_RFST = ISFLUSHRX;
    return res;
}

static int _confirm_cca(ieee802154_dev_t *dev)
{
    (void) dev;

    RFCORE_XREG_RFIRQM0 |= RXPKTDONE;

    return cc2538_cca_status;
}

static int _request_cca(ieee802154_dev_t *dev)
{
    (void) dev;
    if (!RFCORE->XREG_FSMSTAT1bits.RX_ACTIVE) {
        return -EINVAL;
    }

    /* Ignore baseband processing */
    RFCORE_XREG_RFIRQM0 &= ~RXPKTDONE;

    cc2538_cca = true;
    RFCORE_SFR_RFST = ISCLEAR;
    RFCORE_SFR_RFST = STOP;
    RFCORE_XREG_CSPCTRL &= ~CC2538_CSP_MCU_CTRL_MASK;

    /* Execute the last program */
    RFCORE_SFR_RFST = ISSTART;

    return 0;
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

static int _confirm_set_trx_state(ieee802154_dev_t *dev)
{
    (void) dev;
    if (RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE == FSM_STATE_RX_CALIBRATION) {
        return -EAGAIN;
    }
    return 0;
}

static int _request_set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state)
{

    (void) dev;
    int irq = irq_disable();
    if (cc2538_tx_busy || cc2538_rx_busy) {
        irq_restore(irq);
        return -EBUSY;
    }

    switch (state) {
        case IEEE802154_TRX_STATE_TRX_OFF:
        case IEEE802154_TRX_STATE_TX_ON:
            if (RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE != FSM_STATE_IDLE) {
                RFCORE_SFR_RFST = ISRFOFF;
            }
            cc2538_rx_busy = false;
            break;
        case IEEE802154_TRX_STATE_RX_ON:
            RFCORE_XREG_RFIRQM0 |= RXPKTDONE;
            /* Enable RX Chain */
            RFCORE_XREG_FRMCTRL0 &= ~CC2538_FRMCTRL0_RX_MODE_DIS;
            RFCORE_SFR_RFST = ISRXON;
            break;
    }

    RFCORE_SFR_RFIRQF0 = 0;
    RFCORE_SFR_RFIRQF1 = 0;

    irq_restore(irq);
    return 0;
}

void cc2538_irq_handler(void)
{
    uint_fast8_t flags_f0 = RFCORE_SFR_RFIRQF0;
    uint_fast8_t flags_f1 = RFCORE_SFR_RFIRQF1;

    RFCORE_SFR_RFIRQF0 = 0;
    RFCORE_SFR_RFIRQF1 = 0;

    if ((flags_f0 & SFD)) {
        /* If the radio already transmitted, this SFD is the TX_START event */
        if (cc2538_tx_busy) {
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_TX_START);
        }
        /* If the RX chain was not busy, the detected SFD corresponds to a new
         * incoming frame. Note the automatic ACK frame also triggers this event.
         * Therefore, we use this variable to distinguish them. */
        else if (!cc2538_rx_busy){
            cc2538_rx_busy = true;
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_RX_START);
        }
    }

    if (flags_f1 & TXDONE) {
        /* TXDONE marks the end of the TX chain. The radio is not busy anymore */
        cc2538_tx_busy = false;
        cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }

    if (flags_f0 & RXPKTDONE) {
        /* CRC check */
        uint8_t pkt_len = rfcore_peek_rx_fifo(0);
        if (rfcore_peek_rx_fifo(pkt_len) & CC2538_CRC_BIT_MASK) {
            /* Disable RX while the frame has not been processed */
            RFCORE_XREG_FRMCTRL0 |= CC2538_FRMCTRL0_RX_MODE_DIS;
            /* If AUTOACK is disabled or the ACK request bit is not set */
            if (IS_ACTIVE(CONFIG_IEEE802154_AUTO_ACK_DISABLE) ||
                (!(rfcore_peek_rx_fifo(1) & IEEE802154_FCF_ACK_REQ))) {
                /* The radio won't send an ACK. Therefore the RX chain is not
                 * busy anymore
                 */
                cc2538_rx_busy = false;
            }
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        else {
            /* Disable RX while the frame has not been processed */
            /* CRC failed; discard packet. The RX chain is not busy anymore */
            cc2538_rx_busy = false;
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_INDICATION_CRC_ERROR);
        }
    }

    /* The RX chain is not busy anymore on TXACKDONE event */
    if (flags_f1 & TXACKDONE) {
        cc2538_rx_busy = false;
    }

    /* Check if the interrupt was triggered because the CSP finished its routine
     * (CSMA-CA or CCA request)
     */
    if (flags_f1 & CSP_STOP) {
        RFCORE_XREG_CSPCTRL |= CC2538_CSP_MCU_CTRL_MASK;
        if (!cc2538_cca) {
            if (RFCORE_XREG_CSPZ > 0) {
                RFCORE_SFR_RFST = ISTXON;
            }
            else {
                /* In case of CCA failure the TX chain is not busy anymore */
                cc2538_tx_busy = false;
                cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
            }
        }
        else {
            cc2538_cca_status = BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA)
                                && RFCORE->XREG_RSSISTATbits.RSSI_VALID;
            cc2538_rf_hal->cb(cc2538_rf_hal, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }
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
          | IEEE802154_CAP_AUTO_CSMA
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_START
          | IEEE802154_CAP_PHY_OQPSK
          | IEEE802154_CAP_RX_CONTINUOUS,

    .write = _write,
    .read = _read,
    .request_transmit = _request_transmit,
    .confirm_transmit = _confirm_transmit,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_set_trx_state = _request_set_trx_state,
    .confirm_set_trx_state = _confirm_set_trx_state,
    .request_cca = _request_cca,
    .confirm_cca = _confirm_cca,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_csma_params = _set_csma_params,
    .set_frame_filter_mode = _set_frame_filter_mode,
};
