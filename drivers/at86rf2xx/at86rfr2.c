#include "kernel_defines.h"
#if IS_USED(MODULE_AT86RFR2)
#include "xtimer.h"
#include "at86rf2xx.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication_periph.h"
#include "at86rfr2_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

extern const int16_t _a1_r2_tx_pow_to_dbm[16];
extern const uint8_t _a1_r2_dbm_to_tx_pow[22];
extern const int16_t _a1_r2_rx_sens_to_dbm[16];
extern const uint8_t _231_232_a1_r2_dbm_to_rx_sens[53];

static
void at86rfr2_sleep(at86rfr2_t *dev)
{
    if (dev->base.state != AT86RF2XX_STATE_SLEEP) {
        /* First go to TRX_OFF */
        if (dev->base.state != AT86RF2XX_STATE_TRX_OFF) {
            at86rfr2_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
        }
        /* Discard all IRQ flags, framebuffer is lost anyway */
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__IRQ_STATUS));
        /* reset interrupt states in device */
        dev->irq_status = 0;
        /* Setting SLPTR bit brings radio transceiver
           to sleep in TRX_OFF */
        *AT86RFR2_REG__TRXPR |= (AT86RF2XX_TRXPR_MASK__SLPTR);
        dev->base.state = AT86RF2XX_STATE_SLEEP;
    }
}

static
void at86rfr2_assert_awake(at86rfr2_t *dev)
{
    if (dev->base.state == AT86RF2XX_STATE_SLEEP) {
        *AT86RFR2_REG__TRXPR &= ~(AT86RF2XX_TRXPR_MASK__SLPTR);
        xtimer_usleep(AT86RFR2_WAKEUP_DELAY);

        /* update state: on some platforms, the timer behind xtimer
         * may be inaccurate or the radio itself may take longer
         * to wake up due to extra capacitance on the oscillator.
         * Spin until we are actually awake
         */
        do {
            dev->base.state =
                at86rf2xx_periph_reg_read(AT86RFR2_REG(
                                              AT86RF2XX_REG__TRX_STATUS))
                & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
        } while (dev->base.state != AT86RF2XX_TRX_STATUS__TRX_OFF);
    }
}

/**
 * @brief   Convert TX_PWR register value to actual transmission power in dbm
 *          for AT86RFR2 transceivers
 */
static
int16_t at86rfr2_tx_pow_to_dbm(const at86rfr2_t *dev, uint8_t txpower)
{
    (void)dev;
    assert(txpower < ARRAY_SIZE(_a1_r2_tx_pow_to_dbm));
    return _a1_r2_tx_pow_to_dbm[txpower];
}

/**
 * @brief   Convert transmission power in dbm to TX_PWR register value
 *          for AT86RFR2 transceivers
 */
static
uint8_t at86rfr2_dbm_to_tx_pow(const at86rfr2_t *dev, int16_t dbm)
{
    (void)dev;
    assert(dbm >= AT86RFR2_TXPOWER_DBM_MIN &&
           dbm <= AT86RFR2_TXPOWER_DBM_MAX);
    dbm += AT86RFR2_TXPOWER_OFF;
    return _a1_r2_dbm_to_tx_pow[dbm];
}

/**
 * @brief   Convert RX_PDT_LEVEL register value to actual receiver sensibility
 *          in dbm for AT86RFR2 transceivers
 */
static
int16_t at86rfr2_rx_sens_to_dbm(const at86rfr2_t *dev, uint8_t rxsens)
{
    (void)dev;
    assert(rxsens < ARRAY_SIZE(_a1_r2_rx_sens_to_dbm));
    return _a1_r2_rx_sens_to_dbm[rxsens];
}

/**
 * @brief   Convert receiver sensibility to ... register value
 *          for AT86RFR2 transceivers
 */
static
uint8_t at86rfr2_dbm_to_rxsens(const at86rfr2_t *dev, int16_t dbm)
{
    (void)dev;
    assert(dbm >= AT86RFR2_RX_SENSITIVITY_DBM_MIN &&
           dbm <= AT86RFR2_RX_SENSITIVITY_DBM_MAX);
    dbm -= AT86RFR2_RX_SENSITIVITY_DBM_MIN;
    return _231_232_a1_r2_dbm_to_rx_sens[dbm];
}

static
void at86rfr2_hardware_reset(at86rfr2_t *dev)
{
    *AT86RFR2_REG__TRXPR |= AT86RF2XX_TRXPR_MASK__TRXRST;
    xtimer_usleep(AT86RF2XX_RESET_DELAY);
    assert(at86rfr2_get_status(dev) == AT86RF2XX_STATE_TRX_OFF);
    dev->base.state = AT86RF2XX_STATE_TRX_OFF;
}

void at86rfr2_setup(at86rfr2_t *dev)
{
    dev->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RFR2;
    at86rf2xx_setup((at86rf2xx_t *)dev);
    at86rf2xx_periph_reg_write(AT86RFR2_REG(AT86RF2XX_REG__IRQ_MASK), 0x00);
}

int at86rfr2_validate(const at86rfr2_t *dev)
{
    (void)dev;
    uint8_t partn =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__PART_NUM));
    if (partn != AT86RFR2_PARTNUM) {
        DEBUG("[at86rfr2] error: unable to read correct part number\n");
        return -ENOTSUP;
    }
    DEBUG("AT86RFR2 0x%02X\n", partn);
    DEBUG("manufactorer: 0x%02X%02X\n",
          at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__MAN_ID_1)),
          at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__MAN_ID_0)));
    DEBUG("version: 0x%02x\n",
          at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__VERSION_NUM)));
    return 0;
}

uint8_t at86rfr2_set_state(at86rfr2_t *dev, uint8_t state)
{
    uint8_t old_state;
    uint8_t cmd = state;

    if (state == AT86RF2XX_STATE_FORCE_TRX_OFF) {
        state = AT86RF2XX_STATE_TRX_OFF;
    }
    /* make sure there is no ongoing transmission, or state transmission already
       in progress */
    do {
        old_state = at86rfr2_get_status(dev);
    } while (old_state == AT86RF2XX_STATE_BUSY_RX_AACK ||
             old_state == AT86RF2XX_STATE_BUSY_TX_ARET ||
             old_state == AT86RF2XX_STATE_IN_PROGRESS);

    if (state != old_state) {
        if (old_state == AT86RF2XX_STATE_SLEEP) {
            DEBUG("at86rfr2: waking up from sleep mode\n");
            at86rfr2_assert_awake(dev);
        }
        /* we need to go via PLL_ON if we are moving between RX_AACK_ON <-> TX_ARET_ON */
        else if ((old_state == AT86RF2XX_STATE_RX_AACK_ON &&
                  state == AT86RF2XX_STATE_TX_ARET_ON) ||
                 (old_state == AT86RF2XX_STATE_TX_ARET_ON &&
                  state == AT86RF2XX_STATE_RX_AACK_ON)) {
            at86rf2xx_set_state((at86rf2xx_t *)dev, AT86RF2XX_STATE_PLL_ON,
                                AT86RF2XX_STATE_PLL_ON);
            at86rf2xx_set_state((at86rf2xx_t *)dev, state, cmd);
        }
        /* check if we need to wake up from sleep mode */
        else if (state == AT86RF2XX_STATE_SLEEP) {
            at86rfr2_sleep(dev);
        }
        else if (state == AT86RF2XX_STATE_RESET) {
            at86rfr2_reset(dev);
        }
        else {
            at86rf2xx_set_state((at86rf2xx_t *)dev, state, cmd);
        }
    }
    else if (cmd == AT86RF2XX_STATE_FORCE_TRX_OFF) {
        at86rf2xx_set_state((at86rf2xx_t *)dev, AT86RF2XX_STATE_TRX_OFF, cmd);
    }

    /* check state */
    uint8_t trx_status;
    do {
        trx_status =
            at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__TRX_STATUS));
        trx_status &= AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    } while (trx_status == AT86RF2XX_STATE_IN_PROGRESS);
    DEBUG("input state: 0x%02X -- device state: 0x%02X -- trx_staus: 0x%02X\n",
          state, dev->base.state, trx_status);

    if (dev->base.state == AT86RF2XX_STATE_RX_ON) {
        assert((trx_status == AT86RF2XX_STATE_RX_ON) ||
               (trx_status == AT86RF2XX_STATE_BUSY_RX));
    }
    else if (dev->base.state == AT86RF2XX_STATE_RX_AACK_ON) {
        assert((trx_status == AT86RF2XX_STATE_RX_AACK_ON) ||
               (trx_status == AT86RF2XX_STATE_BUSY_RX_AACK));
    }
    else if (dev->base.state == AT86RF2XX_STATE_SLEEP) {
        assert(state == AT86RF2XX_STATE_SLEEP);
    }
    else {
        assert(trx_status == dev->base.state);
    }

    return old_state;
}

void at86rfr2_reset(at86rfr2_t *dev)
{
    /* hardware reset */
    at86rfr2_hardware_reset(dev);

    /* soft reset */
    at86rf2xx_reset((at86rf2xx_t *)dev);

    /* reconfigure channel */
    at86rfr2_set_channel(dev, AT86RFR2_DEFAULT_CHANNEL);

    /* reconfigure TX power */
    at86rfr2_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);

    /* enable smart receive technology */
    at86rfr2_enable_smart_idle(dev);

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rfr2_set_frame_buffer_protection(dev, true);

    /* enable and clear interrupts */
    at86rf2xx_periph_reg_write(AT86RFR2_REG(AT86RF2XX_REG__IRQ_MASK),
                               AT86RF2XX_IRQ_MASK_MASK__TX_END |
                               AT86RF2XX_IRQ_MASK_MASK__RX_END);
    at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__IRQ_STATUS));

    /* State to return after receiving or transmitting */
    dev->base.idle_state = AT86RF2XX_STATE_RX_AACK_ON;

    DEBUG("[at86rfr2] reset complete.\n");
}

int16_t at86rfr2_get_txpower(const at86rfr2_t *dev)
{
    uint8_t phy_tx_pwr =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__PHY_TX_PWR));

    phy_tx_pwr &= AT86RFR2_PHY_TX_PWR_MASK__TX_PWR;
    DEBUG("[at86rfr2] txpower value: %x\n", phy_tx_pwr);
    return at86rfr2_tx_pow_to_dbm(dev, phy_tx_pwr);
}

void at86rfr2_set_txpower(at86rfr2_t *dev, int16_t dbm)
{
    if (dbm < AT86RFR2_TXPOWER_DBM_MIN) {
        dbm = AT86RFR2_TXPOWER_DBM_MIN;
    }
    else if (dbm > AT86RFR2_TXPOWER_DBM_MAX) {
        dbm = AT86RFR2_TXPOWER_DBM_MAX;
    }
    uint8_t txpower = at86rfr2_dbm_to_tx_pow(dev, dbm);
    DEBUG("[at86rfr2] txpower value: %x\n", txpower);
    uint8_t phy_tx_pwr =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__PHY_TX_PWR));
    phy_tx_pwr &= ~(AT86RFR2_PHY_TX_PWR_MASK__TX_PWR);
    phy_tx_pwr |= (txpower & AT86RFR2_PHY_TX_PWR_MASK__TX_PWR);
    at86rf2xx_periph_reg_write(AT86RFR2_REG(
                                   AT86RF2XX_REG__PHY_TX_PWR), phy_tx_pwr);
    dev->base.netdev.txpower = dbm;
}

int16_t at86rfr2_get_rxsensitivity(const at86rfr2_t *dev)
{
    uint8_t rx_syn =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__RX_SYN))
        & AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL;

    return at86rfr2_rx_sens_to_dbm(dev, rx_syn);
}

void at86rfr2_set_rxsensitivity(const at86rfr2_t *dev, int16_t dbm)
{
    if (dbm < AT86RFR2_RX_SENSITIVITY_DBM_MIN) {
        dbm = AT86RFR2_RX_SENSITIVITY_DBM_MIN;
    }
    else if (dbm > AT86RFR2_RX_SENSITIVITY_DBM_MAX) {
        dbm = AT86RFR2_RX_SENSITIVITY_DBM_MAX;
    }
    uint8_t rxsens = at86rfr2_dbm_to_rxsens(dev, dbm);
    DEBUG("[at86rfr2] rxsens value: %x\n", rxsens);
    uint8_t rx_syn =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__RX_SYN));
    rx_syn &= ~(AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL);
    rx_syn |= (rxsens & AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL);
    at86rf2xx_periph_reg_write(AT86RFR2_REG(AT86RF2XX_REG__RX_SYN), rx_syn);
}

bool at86rfr2_cca(at86rfr2_t *dev)
{
    uint8_t old_state = at86rfr2_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    /* Disable RX path */
    uint8_t rx_syn =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__RX_SYN));
    uint8_t tmp = rx_syn | AT86RF2XX_RX_SYN_MASK__RX_PDT_DIS;

    at86rf2xx_periph_reg_write(AT86RFR2_REG(AT86RF2XX_REG__RX_SYN), tmp);
    /* Manually triggered CCA is only possible in RX_ON (basic operating mode) */
    at86rfr2_set_state(dev, AT86RF2XX_STATE_RX_ON);
    bool cca =  at86rf2xx_cca((at86rf2xx_t *)dev);
    /* re-enable RX */
    at86rf2xx_periph_reg_write(AT86RFR2_REG(AT86RF2XX_REG__RX_SYN), rx_syn);
    /* Step back to the old state */
    at86rfr2_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    at86rfr2_set_state(dev, old_state);
    return cca;
}

int8_t at86rfr2_get_cca_threshold(const at86rfr2_t *dev)
{
    uint8_t thresh = at86rf2xx_get_cca_threshold((const at86rf2xx_t *)dev);

    /* multiply by 2 because precesion is 2 dbm */
    return AT86RFR2_RSSI_BASE_VAL + (thresh << 1);
}

void at86rfr2_set_cca_threshold(const at86rfr2_t *dev, int8_t thresh)
{
    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (thresh > 0) {
        thresh = -thresh;
    }
    /* transform the dBm value in the form
       that will fit in the AT86RF2XX_REG__CCA_THRES register */
    thresh -= AT86RFR2_RSSI_BASE_VAL;
    thresh >>= 1; /* divide by 2 because precesion is 2 dbm */
    thresh &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;

    return at86rf2xx_set_cca_threshold((const at86rf2xx_t *)dev,
                                       (uint8_t)thresh);
}

int8_t at86rfr2_get_ed_level(const at86rfr2_t *dev)
{
    (void)dev;
    uint8_t phy_ed_level =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__PHY_ED_LEVEL));
    return (int8_t)(phy_ed_level + AT86RFR2_RSSI_BASE_VAL);
}

void at86rfr2_set_channel(at86rfr2_t *dev, uint8_t channel)
{
    if (channel < AT86RFR2_CHANNEL_MIN) {
        channel = AT86RFR2_CHANNEL_MIN;
    }
    else if (channel > AT86RFR2_CHANNEL_MAX) {
        channel = AT86RFR2_CHANNEL_MAX;
    }

    uint8_t prev_state = at86rfr2_set_state(dev, AT86RF2XX_STATE_TRX_OFF);

    at86rf2xx_set_channel((at86rf2xx_t *)dev, channel);

    /* Return to the state we had before reconfiguring */
    at86rfr2_set_state(dev, prev_state);
}

void at86rfr2_configure_phy(const at86rfr2_t *dev, at86rfr2_phy_mode_t mode)
{
    (void)dev;
    uint8_t trx_ctrl_2 =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__TRX_CTRL_2));

    uint8_t mode_mask = AT86RFR2_TRX_CTRL_2_MASK__OQPSK_DATA_RATE;

    trx_ctrl_2 &= ~(mode_mask);
    trx_ctrl_2 |= (mode & mode_mask);

    at86rf2xx_periph_reg_write(AT86RFR2_REG(
                                   AT86RF2XX_REG__TRX_CTRL_2), trx_ctrl_2);
}

void at86rfr2_enable_smart_idle(at86rfr2_t *dev)
{
    uint8_t tmp =
        at86rf2xx_periph_reg_read(AT86RFR2_REG(AT86RF2XX_REG__TRX_RPC));

    tmp |= (AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__XAH_RPC_EN);
    at86rf2xx_periph_reg_write(AT86RFR2_REG(AT86RF2XX_REG__TRX_RPC), tmp);
    at86rfr2_set_rxsensitivity(dev, AT86RFR2_RSSI_BASE_VAL);
}

uint8_t at86rfr2_fb_read_phr(const at86rfr2_t *dev)
{
    (void)dev;
    return TST_RX_LENGTH;
}

ssize_t at86rfr2_fb_read(const at86rfr2_t *dev, at86rfr2_fb_t *fb,
                         void *buf, size_t buf_size)
{
    (void)dev;
    fb->phr = TST_RX_LENGTH;
    int payload_length = (fb->phr & 0x7F) - IEEE802154_FCS_LEN;
    if (payload_length > 0) {
        if ((size_t)payload_length > buf_size) {
            return -ENOBUFS;
        }
        at86rf2xx_periph_fb_read(buf, AT86RFR2_REG__TRXFBST,
                                 (size_t)payload_length);
        fb->payload = buf;
        at86rf2xx_periph_fb_read(&fb->fcs,
                                 AT86RFR2_REG__TRXFBST + payload_length, 2);
        at86rf2xx_periph_fb_read(&fb->lqi,
                                 AT86RFR2_REG__TRXFBST + payload_length + 2, 1);
    }
    return payload_length;
}
#endif /* IS_USED(MODULE_AT86RFR2) */
