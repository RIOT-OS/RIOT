#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "luid.h"

#include "net/ieee802154.h"
#include "periph/timer.h"
#include "nrf802154.h"
#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
static uint8_t rxbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t txbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t ack[5];

#define ED_RSSISCALE        (4U)
#define ED_RSSIOFFS         (-92)

#define LIFS                (40U)
#define SIFS                (12U)
#define SIFS_MAXPKTSIZE     (18U)
#define TIMER_FREQ          (62500UL)

#define TX_POWER_MIN        (-40)
#define TX_POWER_MAX        (8)

#define DEFAULT_SHORTS      (RADIO_SHORTS_RXREADY_START_Msk | \
                             RADIO_SHORTS_TXREADY_START_Msk);

#define CCA_SHORTS          (RADIO_SHORTS_RXREADY_CCASTART_Msk | \
                             RADIO_SHORTS_CCAIDLE_STOP_Msk | \
                             RADIO_SHORTS_CCAIDLE_TXEN_Msk | \
                             RADIO_SHORTS_CCABUSY_DISABLE_Msk | \
                             RADIO_SHORTS_TXREADY_START_Msk);

typedef enum {
    STATE_IDLE,
    STATE_TX,
    STATE_ACK,
    STATE_RX,
    STATE_CCA_CLEAR,
    STATE_CCA_BUSY,
} nrf802154_state_t;

static volatile uint8_t _state;

static uint8_t nrf802154_short_addr[IEEE802154_SHORT_ADDRESS_LEN];
static uint8_t nrf802154_long_addr[IEEE802154_LONG_ADDRESS_LEN];
static uint16_t nrf802154_pan_id;

static bool ifs;
static bool cca_send = true;
static bool ack_filter = true;

static const ieee802154_radio_ops_t nrf802154_ops;

ieee802154_dev_t nrf802154_hal_dev = {
    .driver = &nrf802154_ops,
};

static bool _l2filter(uint8_t *mhr)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);

    /* filter PAN ID */
    /* Will only work on little endian platform (all?) */

    if ((memcmp(pan_bcast, dst_pan.u8, 2) != 0) &&
        (memcmp(&nrf802154_pan_id, dst_pan.u8, 2) != 0)) {
        return false;
    }

    /* check destination address */
    if (((addr_len == IEEE802154_SHORT_ADDRESS_LEN) &&
          (memcmp(nrf802154_short_addr, dst_addr, addr_len) == 0 ||
           memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0)) ||
        ((addr_len == IEEE802154_LONG_ADDRESS_LEN) &&
          (memcmp(nrf802154_long_addr, dst_addr, addr_len) == 0))) {
        return true;
    }

    return false;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    (void)dev;

    DEBUG("[nrf802154] Send a packet\n");

    assert(iolist);

    /* copy packet data into the transmit buffer */
    unsigned int len = 0;

    /* Load packet data into FIFO. Size checks are handled by higher
     * layers */
    for (; iolist; iolist = iolist->iol_next) {
        /* Check if there is data to copy, prevents undefined behaviour with
         * memcpy when iolist->iol_base == NULL */
        if (iolist->iol_len) {
            memcpy(&txbuf[len + 1], iolist->iol_base, iolist->iol_len);
            len += iolist->iol_len;
        }
    }

    DEBUG("[nrf802154] send: putting %i byte into the ether\n", len);

    /* specify the length of the package. */
    txbuf[0] = len + IEEE802154_FCS_LEN;

    return len;
}

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    (void) dev;
    if (_state != STATE_IDLE && _state != STATE_CCA_BUSY && NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        return -EAGAIN;
    }

    if (info) {
        info->status = (_state == STATE_CCA_BUSY) ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
    }

    _state = STATE_IDLE;
    NRF_RADIO->SHORTS = DEFAULT_SHORTS;

    return 0;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    (void) dev;
    if (ifs) {
        return -EBUSY;
    }

    _state = STATE_TX;
    if (cca_send) {
        NRF_RADIO->SHORTS = CCA_SHORTS;
        NRF_RADIO->TASKS_RXEN = 1;
    }
    else {
        NRF_RADIO->TASKS_TXEN = 1;
    }

    return 0;
}

/**
 * @brief   Convert from the internal representation to dBm, when the
 *          radio operates as a IEEE802.15.4 transceiver.
 */
static inline int8_t _hwval_to_ieee802154_dbm(uint8_t hwval)
{
    return (ED_RSSISCALE * hwval) + ED_RSSIOFFS;
}

static int _indication_rx(ieee802154_dev_t *dev, void *buf, size_t max_size, ieee802154_rx_info_t *info)
{
    (void) dev;
    size_t pktlen = (size_t)rxbuf[0] - IEEE802154_FCS_LEN;
    int res = -ENOBUFS;

    if (max_size < pktlen) {
        DEBUG("[nrf802154] recv: buffer is to small\n");
        return res;
    }
    else {
        DEBUG("[nrf802154] recv: reading packet of length %i\n", pktlen);
        if (info != NULL) {
            ieee802154_rx_info_t *radio_info = info;
            /* Hardware link quality indicator */
            uint8_t hwlqi = rxbuf[pktlen + 1];
            /* Convert to 802.15.4 LQI (page 319 of product spec v1.1) */
            radio_info->lqi = (uint8_t)(hwlqi > UINT8_MAX/ED_RSSISCALE
                                       ? UINT8_MAX
                                       : hwlqi * ED_RSSISCALE);
            /* Calculate RSSI by subtracting the offset from the datasheet.
             * Intentionally using a different calculation than the one from
             * figure 122 of the v1.1 product specification. This appears to
             * match real world performance better */
            radio_info->rssi = _hwval_to_ieee802154_dbm(hwlqi) + IEEE802154_RADIO_RSSI_OFFSET;
        }
        memcpy(buf, &rxbuf[1], pktlen);
    }

    NRF_RADIO->TASKS_START = 1;

    return pktlen;
}

static int _confirm_cca(ieee802154_dev_t *dev)
{
    (void) dev;
    int res;
    switch (_state) {
        case STATE_CCA_CLEAR:
            res = true;
            break;
        case STATE_CCA_BUSY:
            res = false;
            break;
        default:
            res = -EAGAIN;
    }

    _state = STATE_RX;
    return res;
}

static int _request_cca(ieee802154_dev_t *dev)
{
    (void) dev;

    if (_state != STATE_RX) {
        return -EBUSY;
    }

    /* Go back to RxIdle state and start CCA */
    NRF_RADIO->TASKS_STOP = 1;
    NRF_RADIO->TASKS_CCASTART = 1;
    return 0;
}

/**
 * @brief   Set CCA threshold value in internal represetion
 */
static void _set_cca_thresh(uint8_t thresh)
{
    NRF_RADIO->CCACTRL &= ~RADIO_CCACTRL_CCAEDTHRES_Msk;
    NRF_RADIO->CCACTRL |= thresh << RADIO_CCACTRL_CCAEDTHRES_Pos;
}

/**
 * @brief   Convert from dBm to the internal representation, when the
 *          radio operates as a IEEE802.15.4 transceiver.
 */
static inline uint8_t _dbm_to_ieee802154_hwval(int8_t dbm)
{
    return ((dbm - ED_RSSIOFFS) / ED_RSSISCALE);
}

static int set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void) dev;
    _set_cca_thresh(_dbm_to_ieee802154_hwval(threshold));
    return 0;
}

static void _set_txpower(int16_t txpower)
{
    if (txpower > 8) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos8dBm;
    }
    else if (txpower > 1) {
        NRF_RADIO->TXPOWER = (uint32_t)txpower;
    }
    else if (txpower > -1) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    }
    else if (txpower > -5) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    }
    else if (txpower > -9) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    }
    else if (txpower > -13) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    }
    else if (txpower > -17) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    }
    else if (txpower > -21) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    }
    else {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg40dBm;
    }
}

static void _disable(void)
{
    /* set device into DISABLED state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        NRF_RADIO->EVENTS_DISABLED = 0;
        NRF_RADIO->TASKS_DISABLE = 1;
        DEBUG("[nrf802154] Device state: DISABLED\n");
    }
}

static void _set_ifs_timer(bool lifs)
{
    uint8_t timeout;
    ifs = true;
    if (lifs) {
        timeout = LIFS;
    }
    else {
        timeout = SIFS;
    }

    timer_set(NRF802154_TIMER, 1, timeout);
    timer_start(NRF802154_TIMER);
}

static int _confirm_set_trx_state(ieee802154_dev_t *dev)
{
    (void) dev;
    int radio_state = NRF_RADIO->STATE;
    if (radio_state == RADIO_STATE_STATE_TxRu || radio_state == RADIO_STATE_STATE_RxRu ||
        radio_state == RADIO_STATE_STATE_TxDisable || radio_state == RADIO_STATE_STATE_RxDisable) {
        return -EAGAIN;
    }
    return 0;
}

static int _request_set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state)
{
    (void) dev;

    if (_state != STATE_IDLE && _state != STATE_RX) {
        return -EBUSY;
    }

    _disable();

    /* This will take in worst case 21 us */
    while (NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled);

    switch(state) {
        case IEEE802154_TRX_STATE_TRX_OFF:
            _state = STATE_IDLE;
            break;
        case IEEE802154_TRX_STATE_RX_ON:
            NRF_RADIO->PACKETPTR = (uint32_t)rxbuf;
            NRF_RADIO->TASKS_RXEN = 1;
            _state = STATE_RX;
            break;
        case IEEE802154_TRX_STATE_TX_ON:
            NRF_RADIO->PACKETPTR = (uint32_t)txbuf;
            _state = STATE_IDLE;
            break;
    }

    return 0;
}

void nrf802154_on(void)
{
    NRF_RADIO->POWER = 1;
    /* make sure the radio is disabled/stopped */
    _disable();
    /* we configure it to run in IEEE802.15.4 mode */
    NRF_RADIO->MODE = RADIO_MODE_MODE_Ieee802154_250Kbit;
    /* and set some fitting configuration */
    NRF_RADIO->PCNF0 = ((8 << RADIO_PCNF0_LFLEN_Pos) |
                        (RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos) |
                        (RADIO_PCNF0_CRCINC_Include << RADIO_PCNF0_CRCINC_Pos));
    NRF_RADIO->PCNF1 = IEEE802154_FRAME_LEN_MAX;
    /* set start frame delimiter */
    NRF_RADIO->SFD = IEEE802154_SFD;
    /* set MHR filters */
    NRF_RADIO->MHRMATCHCONF = 0;              /* Search Pattern Configuration */
    NRF_RADIO->MHRMATCHMAS = 0xff0007ff;      /* Pattern mask */
    /* configure CRC conform to IEEE802154 */
    NRF_RADIO->CRCCNF = ((RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos) |
                         (RADIO_CRCCNF_SKIPADDR_Ieee802154 << RADIO_CRCCNF_SKIPADDR_Pos));
    NRF_RADIO->CRCPOLY = 0x011021;
    NRF_RADIO->CRCINIT = 0;

    /* Disable the hardware IFS handling  */
    NRF_RADIO->MODECNF0 |= RADIO_MODECNF0_RU_Msk;

    NRF_RADIO->SHORTS = DEFAULT_SHORTS;

    _set_cca_thresh(CONFIG_NRF802154_CCA_THRESH_DEFAULT);

    /* enable interrupts */
    NVIC_EnableIRQ(RADIO_IRQn);
    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk |
                          RADIO_INTENSET_CCAIDLE_Msk |
                          RADIO_INTENSET_CCABUSY_Msk;

}

static void _timer_cb(void *arg, int chan)
{
    (void)arg;
    ieee802154_dev_t *dev = &nrf802154_hal_dev;

    if (chan == 0) {
        ack[3] = rxbuf[3];
        NRF_RADIO->PACKETPTR = (uint32_t) &ack;
        NRF_RADIO->TASKS_TXEN = 1;
        dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
    }
    else if (chan == 1) {
        ifs = false;
    }

    timer_stop(NRF802154_TIMER);
}
/**
 * @brief   Set radio into DISABLED state
 */
int nrf802154_init(void)
{
    /* reset buffer */
    rxbuf[0] = 0;
    txbuf[0] = 0;

    ack[0] = 5;
    ack[1] = 0x02;
    ack[2] = 0;

    int result = timer_init(NRF802154_TIMER, TIMER_FREQ, _timer_cb, NULL);
    assert(result >= 0);
    (void)result;
    timer_stop(NRF802154_TIMER);

    _state = STATE_IDLE;
    /* power off peripheral */
    NRF_RADIO->POWER = 0;

    return 0;
}

void isr_radio(void)
{
    ieee802154_dev_t *dev = &nrf802154_hal_dev;

    if (NRF_RADIO->EVENTS_CCAIDLE) {
        NRF_RADIO->EVENTS_CCAIDLE = 0;
        if (_state != STATE_TX) {
            _state = STATE_CCA_CLEAR;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }

    if (NRF_RADIO->EVENTS_CCABUSY) {
        NRF_RADIO->EVENTS_CCABUSY = 0;
        if (_state == STATE_TX) { 
            /* TODO */
            _state = STATE_CCA_BUSY;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
        else {
            _state = STATE_CCA_BUSY;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }

    if (NRF_RADIO->EVENTS_END) {
        NRF_RADIO->EVENTS_END = 0;

        switch (_state) {
            case STATE_TX:
                DEBUG("[nrf802154] TX state: %x\n", (uint8_t)NRF_RADIO->STATE);

                _set_ifs_timer(txbuf[0] > SIFS_MAXPKTSIZE);
                _state = STATE_IDLE;
                dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
                break;
            case STATE_RX:
                if (NRF_RADIO->CRCSTATUS) {
                    if (_l2filter(rxbuf+1)) {
                        /* If frame has ACK_REQ bit and AACK is enabled */
                        if ((rxbuf[1] & IEEE802154_FCF_ACK_REQ) && ack[1]) {
                            timer_set(NRF802154_TIMER, 0, 12U);
                            timer_start(NRF802154_TIMER);
                            _disable();
                            _state = STATE_ACK;
                        }
                        else {
                            dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                        }
                    }
                    else if (ack_filter == false && rxbuf[1] & IEEE802154_FCF_TYPE_ACK) {
                        NRF_RADIO->TASKS_START = 1;
                        dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                    }
                    else {
                        NRF_RADIO->TASKS_START = 1;
                    }
                }
                else {
                    NRF_RADIO->TASKS_START = 1;
                }
                break;
            case STATE_ACK:
                _state = STATE_RX;
                NRF_RADIO->PACKETPTR = (uint32_t) rxbuf;
                _disable();
                /* This will take around 0.5 us */
                while(NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled);
                NRF_RADIO->TASKS_RXEN = 1;
                _set_ifs_timer(false);
                break;
            default:
                assert(false);
        }
    }

    cortexm_isr_end();
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void) dev;
    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    (void) dev;
    nrf802154_on();
    return 0;
}

static int _config_phy(ieee802154_dev_t *dev, ieee802154_phy_conf_t *conf)
{
    (void) dev;
    _disable();
    int8_t pow = conf->pow;

    if (pow < TX_POWER_MIN || pow > TX_POWER_MAX) {
        return -EINVAL;
    }

    NRF_RADIO->FREQUENCY = (((uint8_t) conf->channel) - 10) * 5;
    _set_txpower(pow);
    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    /* TODO: implement */
    (void) dev;
    return 0;
}

static bool _get_cap(ieee802154_dev_t *dev, ieee802154_rf_caps_t cap)
{
    (void) dev;
    switch (cap) {
        case IEEE802154_CAP_24_GHZ:
        case IEEE802154_CAP_IRQ_TX_DONE:
        case IEEE802154_CAP_IRQ_CCA_DONE:
            return true;
        default:
            return false;
    }
}

int _len(ieee802154_dev_t *dev)
{
    (void) dev;
    return (size_t)rxbuf[0] - IEEE802154_FCS_LEN;
}

int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void) dev;

    NRF_RADIO->CCACTRL &= RADIO_CCACTRL_CCAMODE_Msk;
    uint8_t tmp = 0;

    switch (mode) {
        case IEEE802154_CCA_MODE_ED_THRESHOLD:
            tmp = RADIO_CCACTRL_CCAMODE_EdMode;
            break;
        case IEEE802154_CCA_MODE_CARRIER_SENSING:
            tmp = RADIO_CCACTRL_CCAMODE_CarrierOrEdMode;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
            tmp = RADIO_CCACTRL_CCAMODE_CarrierAndEdMode;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
            tmp = RADIO_CCACTRL_CCAMODE_CarrierOrEdMode;
            break;
    }

    NRF_RADIO->CCACTRL |= tmp;

    return 0;
}

static int _set_hw_addr_filter(ieee802154_dev_t *dev, const network_uint16_t *short_addr,
                              const eui64_t *ext_addr, const uint16_t *pan_id)
{
    (void) dev;
    if (short_addr) {
        memcpy(nrf802154_short_addr, short_addr, IEEE802154_SHORT_ADDRESS_LEN);
    }

    if (ext_addr) {
        memcpy(nrf802154_long_addr, ext_addr, IEEE802154_LONG_ADDRESS_LEN);
    }

    if (pan_id) {
        nrf802154_pan_id = *pan_id;
    }

    return 0;
}

static int _set_rx_mode(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode)
{
    (void) dev;
    bool ackf = true;
    switch (mode) {
        case IEEE802154_RX_AACK_DISABLED:
            ack[1] = 0;
            break;
        case IEEE802154_RX_AACK_ENABLED:
            ack[1] = IEEE802154_FCF_TYPE_ACK;
            break;
        case IEEE802154_RX_AACK_FRAME_PENDING:
            ack[1] = IEEE802154_FCF_TYPE_ACK | IEEE802154_FCF_FRAME_PEND;
            break;
        case IEEE802154_RX_PROMISC:
            /* TODO */
            break;
        case IEEE802154_RX_WAIT_FOR_ACK:
            ackf = false;
            break;
    }

    ack_filter = ackf;

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, ieee802154_csma_be_t *bd, int8_t retries)
{
    (void) dev;
    (void) bd;

    if (retries > 0) {
        return -ENOTSUP;
    }

    cca_send = (retries == 0);

    return 0;
}

static const ieee802154_radio_ops_t nrf802154_ops = {
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
    .set_cca_threshold = set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_hw_addr_filter = _set_hw_addr_filter,
    .set_csma_params = _set_csma_params,
    .set_rx_mode = _set_rx_mode,
};
#else
int dont_be_pedantic;
#endif
