#include <errno.h>
#include <stdio.h>

#include "net/gnrc.h"

#include "cc2538_rf.h"
#include "cc2538_rf_internal.h"

#include "net/ieee802154/radio.h"

#define CC2538_ACCEPT_FT_2_ACK     (1 << 5)

#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
static const ieee802154_radio_ops_t cc2538_rf_ops;

ieee802154_dev_t cc2538_rf_dev = {
    .driver = &cc2538_rf_ops,
};

static uint8_t cc2538_min_be = 3;
static uint8_t cc2538_max_be = 5;
static int cc2538_retries = 4;

static bool cc2538_cca_status;
static bool cc2538_cca;

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

    if (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE != 0 || !(RFCORE_XREG_CSPCTRL & CC2538_CSP_MCU_CTRL_MASK)) {
        return -EAGAIN;
    }

    if (info) {
        if (cc2538_retries >= 0 && RFCORE_XREG_CSPZ == 0) {
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

    if (cc2538_retries < 0) {
        RFCORE_SFR_RFST = ISTXON;
    }
    else {
        cc2538_cca = false;

        RFCORE_SFR_RFST = ISRXON;
        /* Clear last program */
        RFCORE_SFR_RFST = ISCLEAR;

        /*  If the RSSI is not yet valid, busy loop */
        RFCORE_SFR_RFST = SKIP_S_C |
                          CC2538_CSP_SKIP_N_MASK |
                          CC2538_CSP_SKIP_COND_RSSI;

        RFCORE_SFR_RFST = LABEL;

        RFCORE_SFR_RFST = RANDXY;
        RFCORE_SFR_RFST = WAITX;

        /* If CCA is valid, stop */
        RFCORE_SFR_RFST = SKIP_S_C |
                          (1 << CC2538_CSP_SKIP_INST_POS) |
                          CC2538_CSP_SKIP_N_MASK |
                          CC2538_CSP_SKIP_COND_CCA;

        RFCORE_SFR_RFST = STOP;

        RFCORE_SFR_RFST = DECZ;
        RFCORE_SFR_RFST = INCMAXY | (cc2538_max_be & CC2538_CSP_INCMAXY_MAX_MASK);

        RFCORE_SFR_RFST = RPT_C |
                          CC2538_CSP_SKIP_N_MASK |
                          CC2538_CSP_SKIP_COND_CSPZ;

        /* Stop the program */
        RFCORE_SFR_RFST = STOP;

        RFCORE_XREG_CSPX = 0; /* Holds timer value */
        RFCORE_XREG_CSPY = cc2538_min_be; /* Holds MinBE */

        assert(cc2538_retries >= 0);
        RFCORE_XREG_CSPZ = cc2538_retries + 1; /* Holds attempts (retries + 1) */

        RFCORE_XREG_CSPCTRL &= ~CC2538_CSP_MCU_CTRL_MASK;

        /* Execute the last program */
        RFCORE_SFR_RFST = ISSTART;
    }
    return 0;
}

static int _len(ieee802154_dev_t *dev)
{
    (void) dev;
   return rfcore_peek_rx_fifo(0) - IEEE802154_FCS_LEN;
}

static int _indication_rx(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    (void) dev;
    int res;
    size_t pkt_len = rfcore_read_byte();

    pkt_len -= IEEE802154_FCS_LEN;

    if (pkt_len > size) {
        RFCORE_SFR_RFST = ISFLUSHRX;
        return -ENOBUFS;
    }

    if (buf != NULL) {
        rfcore_read_fifo(buf, pkt_len);
        res = pkt_len;
        if (info != NULL) {
            uint8_t corr_val;
            int8_t rssi_val;
            rssi_val = rfcore_read_byte();

            /* The number of dB above maximum sensitivity detected for the
             * received packet */
            info->rssi = -CC2538_RSSI_OFFSET + rssi_val + IEEE802154_RADIO_RSSI_OFFSET;

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
    }
    else {
        res = 0;
    }

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

static int _config_phy(ieee802154_dev_t *dev, ieee802154_phy_conf_t *conf)
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
    bool wait_sfd = RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE > 0x2 && RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE < 0x7;
    if ((RFCORE->XREG_FSMSTAT1bits.RX_ACTIVE && !wait_sfd) ||
         RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        return -EBUSY;
    }

    switch(state) {
        case IEEE802154_TRX_STATE_TRX_OFF:
        case IEEE802154_TRX_STATE_TX_ON:
            if (RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE != FSM_STATE_IDLE) {
                RFCORE_SFR_RFST = ISRFOFF;
            }
            break;
        case IEEE802154_TRX_STATE_RX_ON:
            RFCORE_XREG_RFIRQM0 |= RXPKTDONE;
            RFCORE_SFR_RFST = ISRXON;
            break;
    }

    return 0;
}

void _irq_handler(void)
{
    uint_fast8_t flags_f0 = RFCORE_SFR_RFIRQF0;
    uint_fast8_t flags_f1 = RFCORE_SFR_RFIRQF1;

    RFCORE_SFR_RFIRQF0 = 0;
    RFCORE_SFR_RFIRQF1 = 0;

    if (flags_f1 & TXDONE) {
        cc2538_rf_dev.cb(&cc2538_rf_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }
     
    if (flags_f0 & RXPKTDONE) {
        /* CRC check */
        uint8_t pkt_len = rfcore_peek_rx_fifo(0);
        if (rfcore_peek_rx_fifo(pkt_len) & CC2538_CRC_BIT_MASK) {
            cc2538_rf_dev.cb(&cc2538_rf_dev, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        else {
            /* CRC failed; discard packet */
            RFCORE_SFR_RFST = ISFLUSHRX;
        }

    }

    if (flags_f1 & CSP_STOP) {
        RFCORE_XREG_CSPCTRL |= CC2538_CSP_MCU_CTRL_MASK;
        if (!cc2538_cca) {
            if (RFCORE_XREG_CSPZ > 0) {
                RFCORE_XREG_RXMASKCLR = CC2538_RXENABLE_RXON_MASK;
                RFCORE_SFR_RFST = ISTXON;
            }
            else {
                cc2538_rf_dev.cb(&cc2538_rf_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
            }
        }
        else {
            cc2538_cca_status = BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA) && RFCORE->XREG_RSSISTATbits.RSSI_VALID;
            cc2538_rf_dev.cb(&cc2538_rf_dev, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }
}

static int _off(ieee802154_dev_t *dev)
{
    (void) dev;
    return -ENOTSUP;
}

static bool _get_cap(ieee802154_dev_t *dev, ieee802154_rf_caps_t cap)
{
    (void) dev;
    switch(cap) {
        case IEEE802154_CAP_24_GHZ:
        case IEEE802154_CAP_IRQ_TX_DONE:
        case IEEE802154_CAP_IRQ_CCA_DONE:
        case IEEE802154_CAP_AUTO_CSMA:
            return true;
        default:
            return false;
    }
}

static int _set_hw_addr_filter(ieee802154_dev_t *dev, const network_uint16_t *short_addr,
                               const eui64_t *ext_addr, uint16_t pan_id)
{
    (void) dev;
    if (short_addr) {
        RFCORE_FFSM_SHORT_ADDR0 = short_addr->u8[1];
        RFCORE_FFSM_SHORT_ADDR1 = short_addr->u8[0];
    }

    if (ext_addr) {
        RFCORE_FFSM_EXT_ADDR0 = ext_addr->uint8[7];
        RFCORE_FFSM_EXT_ADDR1 = ext_addr->uint8[6];
        RFCORE_FFSM_EXT_ADDR2 = ext_addr->uint8[5];
        RFCORE_FFSM_EXT_ADDR3 = ext_addr->uint8[4];
        RFCORE_FFSM_EXT_ADDR4 = ext_addr->uint8[3];
        RFCORE_FFSM_EXT_ADDR5 = ext_addr->uint8[2];
        RFCORE_FFSM_EXT_ADDR6 = ext_addr->uint8[1];
        RFCORE_FFSM_EXT_ADDR7 = ext_addr->uint8[0];
    }

    RFCORE_FFSM_PAN_ID0 = pan_id;
    RFCORE_FFSM_PAN_ID1 = pan_id >> 8;
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

static int _set_rx_mode(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode)
{
    (void) dev;

    bool promisc = false;
    bool ack_filter = true;
    switch (mode) {
        case IEEE802154_RX_AACK_DISABLED:
            RFCORE->XREG_FRMCTRL0bits.AUTOACK = 0;
            break;
        case IEEE802154_RX_AACK_ENABLED:
            RFCORE->XREG_FRMCTRL0bits.AUTOACK = 1;
            RFCORE_XREG_FRMCTRL1 &= ~0x4;
            break;
        case IEEE802154_RX_AACK_FRAME_PENDING:
            RFCORE->XREG_FRMCTRL0bits.AUTOACK = 1;
            RFCORE_XREG_FRMCTRL1 |= 0x4;
            break;
        case IEEE802154_RX_PROMISC:
            promisc = true;
            break;
        case IEEE802154_RX_WAIT_FOR_ACK:
            ack_filter = false;
            break;
    }

    if (ack_filter) {
        RFCORE_XREG_FRMFILT1 &= ~CC2538_ACCEPT_FT_2_ACK;
    }
    else {
        RFCORE_XREG_FRMFILT1 |= CC2538_ACCEPT_FT_2_ACK;
    }

    cc2538_set_monitor(promisc);
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, ieee802154_csma_be_t *bd, int8_t retries)
{
    (void) dev;
    if (bd) {
        cc2538_min_be = bd->min;
        cc2538_max_be = bd->max;
    }

    cc2538_retries = retries;

    return 0;
}

static const ieee802154_radio_ops_t cc2538_rf_ops = {
    .write = _write,
    .request_transmit = _request_transmit,
    .confirm_transmit = _confirm_transmit,
    .len = _len,
    .indication_rx = _indication_rx,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_set_trx_state = _request_set_trx_state,
    .confirm_set_trx_state = _confirm_set_trx_state,
    .request_cca = _request_cca,
    .confirm_cca = _confirm_cca,
    .get_cap = _get_cap,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_hw_addr_filter = _set_hw_addr_filter,
    .set_csma_params = _set_csma_params,
    .set_rx_mode = _set_rx_mode,
};
#else
int dont_be_pedantic;
#endif
