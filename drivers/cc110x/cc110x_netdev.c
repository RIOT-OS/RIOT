/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Implementation of RIOT's netdev_driver API for the CC1100/CC1101
 *              transceiver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "assert.h"
#include "iolist.h"
#include "irq.h"
#include "mutex.h"
#include "net/eui64.h"
#include "net/netdev.h"
#include "xtimer.h"

#include "cc110x.h"
#include "cc110x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int cc110x_init(netdev_t *netdev);
static int cc110x_recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int cc110x_send(netdev_t *netdev, const iolist_t *iolist);
static int cc110x_get(netdev_t *netdev, netopt_t opt,
                      void *val, size_t max_len);
static int cc110x_set(netdev_t *netdev, netopt_t opt,
                      const void *val, size_t len);

/**
 * @brief   A lookup table to convert from dBm value to the best matching
 *          @ref cc110x_tx_power_t value
 */
static const int8_t tx_power_from_dbm[] = {
    [CC110X_TX_POWER_MINUS_30_DBM]  = -25,
    [CC110X_TX_POWER_MINUS_20_DBM]  = -17,
    [CC110X_TX_POWER_MINUS_15_DBM]  = -12,
    [CC110X_TX_POWER_MINUS_10_DBM]  = -5,
    [CC110X_TX_POWER_0_DBM]         = 3,
    [CC110X_TX_POWER_PLUS_5_DBM]    = 6,
    [CC110X_TX_POWER_PLUS_7_DBM]    = 9,
};

/**
 * @brief   A lookup table to convert an @ref cc110x_tx_power_t value to dBm
 */
static const int8_t dbm_from_tx_power[] = {
    [CC110X_TX_POWER_MINUS_30_DBM]  = -30,
    [CC110X_TX_POWER_MINUS_20_DBM]  = -20,
    [CC110X_TX_POWER_MINUS_15_DBM]  = -15,
    [CC110X_TX_POWER_MINUS_10_DBM]  = -10,
    [CC110X_TX_POWER_0_DBM]         = 0,
    [CC110X_TX_POWER_PLUS_5_DBM]    = 5,
    [CC110X_TX_POWER_PLUS_7_DBM]    = 7,
    [CC110X_TX_POWER_PLUS_10_DBM]   = 10,
};

const netdev_driver_t cc110x_driver = {
    .init = cc110x_init,
    .recv = cc110x_recv,
    .send = cc110x_send,
    .isr  = cc110x_isr,
    .get  = cc110x_get,
    .set  = cc110x_set,
};

void cc110x_on_gdo(void *_dev)
{
    cc110x_t *dev = _dev;
    if ((dev->state & 0x07) == CC110X_STATE_TX_MODE) {
        /* Unlock mutex to unblock netdev thread */
        mutex_unlock(&dev->isr_signal);
    }
    else {
        netdev_trigger_event_isr(&dev->netdev);
    }
}

static int identify_device(cc110x_t *dev)
{
    uint8_t partnum, version, status;
    int is_ready;
    cc110x_state_t state;

    cc110x_read(dev, CC110X_REG_VERSION, &version);
    /* Retrieving the status is reliable for non-transient status */
    status = cc110x_read(dev, CC110X_REG_PARTNUM, &partnum);
    state = cc110x_state_from_status(status);
    /* Most significant bit should be zero, otherwise chip is not ready */
    is_ready = cc110x_is_ready_from_status(status);

    DEBUG("[cc110x] PARTNUM = %i, VERSION = %i, STATUS = 0x%02x, READY = %i\n",
          (int)partnum, (int)version, (int)status, is_ready);

    if ((state != CC110X_STATE_IDLE) || (!is_ready)) {
        DEBUG("[cc110x] IC not ready or in invalid state\n");
        return -1;
    }

    /* Source: https://e2e.ti.com/support/wireless-connectivity/other-wireless/f/667/t/370643 */
    if (partnum != 0) {
        DEBUG("[cc110x] Device not a CC110x transceiver\n");
        return -1;
    }

    switch (version) {
    case 3:
        DEBUG("[cc110x] Detected CC1100 transceiver\n");
        /* RSSI offset is 78dBm @ 868MHz & 250kBaud.
         * Depends on the symbol rate and base band and ranges from
         * 74dBm to 79dBm.
         */
        dev->rssi_offset = 78;
        return 0;
    case 5:
        DEBUG("[cc110x] Detected CC1100E transceiver\n");
        /* RSSI offset is 79 dBm @ 250kbps & 250 kbps.
         * Depends on base band and symbol rate and ranges from
         * 75dBm to 79dBm
         */
        dev->rssi_offset = 79;
        return 0;
    case 4:
    /* falls through */
    case 14:
    /* falls through */
    case 20:
        /* RSSI offset for the CC1101 is independent of symbol rate and
         * base 74 dBm
         */
        dev->rssi_offset = 74;
        DEBUG("[cc110x] Detected CC1101 transceiver\n");
        return 0;
    default:
        DEBUG("[cc110x] Device not a CC110x transceiver\n");
        return -1;
    }
}

static int check_config(cc110x_t *dev)
{
    char buf[CC110X_CONF_SIZE];

    /* Verify content of main config registers */
    cc110x_burst_read(dev, CC110X_CONF_START, buf, sizeof(buf));
    if (memcmp(buf, cc110x_conf, sizeof(buf))) {
        DEBUG("[cc110x] ERROR: Verification of main config registers failed\n"
              "         Check SPI wiring or reduce SPI clock\n");
        return -1;
    }

    /* Verify content of "magic number" config registers */
    cc110x_burst_read(dev, CC110X_REG_TEST2, buf,
                      sizeof(cc110x_magic_registers));
    if (memcmp(buf, cc110x_magic_registers, sizeof(cc110x_magic_registers))) {
        DEBUG("[cc110x] ERROR: Verification of \"magic\" registers failed\n"
              "         Check SPI wiring or reduce SPI clock\n");
        return -1;
    }

    /* Verify content of PA_TABLE */
    cc110x_burst_read(dev, CC110X_MULTIREG_PATABLE, buf, CC110X_PATABLE_LEN);
    if (memcmp(buf, dev->params.patable->data, CC110X_PATABLE_LEN)) {
        DEBUG("[cc110x] ERROR: Verification of PA_TABLE failed\n"
              "         Check SPI wiring or reduce SPI clock\n");
        return -1;
    }

    DEBUG("[cc110x] Content of configuration registers verified\n");
    return 0;
}

static int check_gdo_pins(cc110x_t *dev)
{
    /* GPIOs connected to GDOs are not yet configured, so we do this now.
     * This configuration is just temporarily for testing, as gpio_init_int()
     * will reconfigure them later again. We do not want to set up the
     * interrupts before validating the transceiver, so we effectively configure
     * the GPIOs twice.
     */
    if (gpio_init(dev->params.gdo2, GPIO_IN)) {
        DEBUG("[cc110x] Configuring GDO2 failed");
        return -1;
    }

    if (gpio_init(dev->params.gdo0, GPIO_IN)) {
        DEBUG("[cc110x] Configuring GDO0 failed");
        return -1;
    }

    /* Validate that GDO2 responds to configuration updates */
    cc110x_write(dev, CC110X_REG_IOCFG2, CC110X_GDO_CONSTANT_HIGH);
    if (!gpio_read(dev->params.gdo2)) {
        DEBUG("[cc110x] GDO2 does not respond (check wiring!)\n");
        return -1;
    }

    cc110x_write(dev, CC110X_REG_IOCFG2, CC110X_GDO_CONSTANT_LOW);
    if (gpio_read(dev->params.gdo2)) {
        DEBUG("[cc110x] GDO2 does not respond (check wiring!)\n");
        return -1;
    }

    /* Validate that GDO0 responds to configuration updates */
    cc110x_write(dev, CC110X_REG_IOCFG0, CC110X_GDO_CONSTANT_HIGH);
    if (!gpio_read(dev->params.gdo0)) {
        DEBUG("[cc110x] GDO0 does not respond (check wiring!)\n");
        return -1;
    }

    cc110x_write(dev, CC110X_REG_IOCFG0, CC110X_GDO_CONSTANT_LOW);
    if (gpio_read(dev->params.gdo0)) {
        DEBUG("[cc110x] GDO0 does not respond (check wiring!)\n");
        return -1;
    }

    /* Restore default GDO2 & GDO0 config */
    cc110x_write(dev, CC110X_REG_IOCFG2, cc110x_conf[CC110X_REG_IOCFG2]);
    cc110x_write(dev, CC110X_REG_IOCFG0, cc110x_conf[CC110X_REG_IOCFG0]);

    return 0;
}

static int cc110x_init(netdev_t *netdev)
{
    cc110x_t *dev = (cc110x_t *)netdev;
    /* Use locked mutex to block thread on TX and un-block from ISR */
    mutex_init(&dev->isr_signal);
    mutex_lock(&dev->isr_signal);

    /* Make sure the crystal is stable and the chip ready. This is needed as
     * the reset is done via an SPI command, but the SPI interface must not be
     * used unless the chip is ready according to the data sheet. After the
     * reset, a second call to cc110x_power_on_and_acquire() is needed to finally have
     * the transceiver in a known state and ready for SPI communication.
     */
    if (cc110x_power_on_and_acquire(dev)) {
        DEBUG("[cc110x] netdev_driver_t::init(): Failed to pull CS pin low\n");
        return -EIO;
    }

    /* Performing a reset of the transceiver to get it in a known state */
    cc110x_cmd(dev, CC110X_STROBE_RESET);
    cc110x_release(dev);

    /* Again, make sure the crystal is stable and the chip ready */
    if (cc110x_power_on_and_acquire(dev)) {
        DEBUG("[cc110x] netdev_driver_t::init(): Failed to pull CS pin low "
              "after reset\n");
        return -EIO;
    }

    if (identify_device(dev)) {
        DEBUG("[cc110x] netdev_driver_t::init(): Device identification failed\n");
        cc110x_release(dev);
        return -ENOTSUP;
    }

    /* Upload the main configuration */
    cc110x_burst_write(dev, CC110X_CONF_START, cc110x_conf, CC110X_CONF_SIZE);
    /* Set TX power to match uploaded configuration */
    dev->tx_power = CC110X_TX_POWER_0_DBM;

    /* Upload the poorly documented magic numbers obtained via SmartRF Studio */
    cc110x_burst_write(dev, CC110X_REG_TEST2, cc110x_magic_registers,
                       sizeof(cc110x_magic_registers));

    /* Setup the selected PA_TABLE */
    cc110x_burst_write(dev, CC110X_MULTIREG_PATABLE,
                       dev->params.patable->data, CC110X_PATABLE_LEN);

    /* Verify main config, magic numbers and PA_TABLE correctly uploaded */
    if (check_config(dev)) {
        cc110x_release(dev);
        return -EIO;
    }

    /* Verify that pins GDO2 and GDO0 are correctly connected */
    if (check_gdo_pins(dev)) {
        cc110x_release(dev);
        return -EIO;
    }

    /* Setup the layer 2 address, but do not accept CC1XXX_BCAST_ADDR (which
     * has the value 0x00 and is used for broadcast)
     */
    cc1xxx_eui_get(&dev->netdev, &dev->addr);
    assert(dev->addr != CC1XXX_BCAST_ADDR);
    cc110x_write(dev, CC110X_REG_ADDR, dev->addr);

    /* Setup interrupt on GDO0  */
    if (gpio_init_int(dev->params.gdo0, GPIO_IN, GPIO_BOTH,
                      cc110x_on_gdo, dev)) {
        cc110x_release(dev);
        DEBUG("[cc110x] netdev_driver_t::init(): Failed to setup interrupt on "
              "GDO0 pin\n");
        return -EIO;
    }

    /* Setup interrupt on GDO2 */
    if (gpio_init_int(dev->params.gdo2, GPIO_IN, GPIO_BOTH,
                      cc110x_on_gdo, dev)) {
        gpio_irq_disable(dev->params.gdo0);
        cc110x_release(dev);
        DEBUG("[cc110x] netdev_driver_t::init(): Failed to setup interrupt on "
              "GDO2 pin\n");
        return -EIO;
    }

    /* Update the state of the driver/transceiver */
    dev->state = CC110X_STATE_IDLE;
    cc110x_release(dev);

    int retval; /**< Store return value to be able to pass through error code */
    /* Apply configuration (if non-NULL) and channel map, which also calls
     * cc110x_full_calibration
     */
    retval = cc110x_apply_config(dev, dev->params.config, dev->params.channels,
                                 CONFIG_CC110X_DEFAULT_CHANNEL);
    if (retval) {
        gpio_irq_disable(dev->params.gdo0);
        gpio_irq_disable(dev->params.gdo2);
        DEBUG("[cc110x] netdev_driver_t::init(): cc110x_apply_config() "
              "failed\n");
        /* Pass through received error code */
        return retval;
    }

    DEBUG("[cc110x] netdev_driver_t::init(): Success\n");

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int cc110x_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    cc110x_t *dev = (cc110x_t *)netdev;

    /* Call to cc110x_enter_rx_mode() will clear dev->buf.len, so back up it first */
    int size = dev->buf.len;

    cc110x_acquire(dev);

    /* Copy RX info on last frame (if requested) */
    if (info != NULL) {
        *((cc1xxx_rx_info_t *)info) = dev->rx_info;
    }

    if (!buf) {
        /* Get the size of the frame; if len > 0 then also drop the frame */
        if (len > 0) {
            /* Drop frame requested */
            cc110x_enter_rx_mode(dev);
        }
        cc110x_release(dev);
        return size;
    }

    if (len < (size_t)size) {
        /* Drop frame and return -ENOBUFS */
        cc110x_enter_rx_mode(dev);
        cc110x_release(dev);
        return -ENOBUFS;
    }

    memcpy(buf, dev->buf.data, (size_t)size);

    cc110x_enter_rx_mode(dev);
    cc110x_release(dev);
    return size;
}

static int cc110x_send(netdev_t *netdev, const iolist_t *iolist)
{
    cc110x_t *dev = (cc110x_t *)netdev;

    /* assert that cc110x_send was called with valid parameters */
    assert(netdev && iolist && (iolist->iol_len == sizeof(cc1xxx_l2hdr_t)));

    cc110x_acquire(dev);

    switch (dev->state) {
    case CC110X_STATE_FSTXON:
        /* falls through */
    case CC110X_STATE_RX_MODE:
        break;
    case CC110X_STATE_RECEIVING:
        cc110x_release(dev);
        DEBUG("[cc110x] netdev_driver_t::send(): Refusing to send while "
              "receiving a frame\n");
        return -EBUSY;
    case CC110X_STATE_OFF:
        cc110x_release(dev);
        return -ENOTSUP;
    default:
        cc110x_release(dev);
        DEBUG("[cc110x] netdev_driver_t::send(): Driver state %i prevents "
              "sending\n", (int)dev->state);
        return -1;
    }

    /* Copy data to send into frame buffer */
    size_t size = sizeof(cc1xxx_l2hdr_t);
    memcpy(dev->buf.data, iolist->iol_base, sizeof(cc1xxx_l2hdr_t));

    for (const iolist_t *iol = iolist->iol_next; iol; iol = iol->iol_next) {
        if (iol->iol_len) {
            if (size + iol->iol_len > CC110X_MAX_FRAME_SIZE) {
                cc110x_release(dev);
                DEBUG("[cc110x] netdev_driver_t::send(): Frame size of %"
                      PRIuSIZE "B exceeds maximum supported size of %uB\n",
                      size + iol->iol_len, (unsigned)CC110X_MAX_FRAME_SIZE);
                return -1;
            }
            memcpy(dev->buf.data + size, iol->iol_base, iol->iol_len);
            size += iol->iol_len;
        }
    }

    dev->buf.len = (uint8_t)size;

    /* Disable IRQs, as GDO configuration will be changed now */
    gpio_irq_disable(dev->params.gdo0);
    gpio_irq_disable(dev->params.gdo2);

    /* Fill the TX FIFO: First write the length, then the frame */
    dev->buf.pos = (size > CC110X_FIFO_SIZE - 1) ? CC110X_FIFO_SIZE - 1 : size;
    /* cc110x_framebuf_t has the same memory layout as the device expects */
    cc110x_burst_write(dev, CC110X_MULTIREG_FIFO,
                       &dev->buf, dev->buf.pos + 1);

    /* Go to TX */
    cc110x_cmd(dev, CC110X_STROBE_TX);

    /* Configure GDO2 and update state */
    if (dev->buf.pos < dev->buf.len) {
        /* We need to keep feeding TX FIFO */
        cc110x_write(dev, CC110X_REG_IOCFG2, CC110X_GDO_ON_TX_DATA);
        dev->state = CC110X_STATE_TX_MODE;
    }
    else {
        /* All data in TX FIFO, just waiting for transceiver to finish */
        cc110x_write(dev, CC110X_REG_IOCFG2, CC110X_GDO_CONSTANT_LOW);
        dev->state = CC110X_STATE_TX_COMPLETING;
    }

    cc110x_release(dev);

    /* Restore IRQs */
    gpio_irq_enable(dev->params.gdo0);
    gpio_irq_enable(dev->params.gdo2);

    while ((dev->state & 0x07) == CC110X_STATE_TX_MODE) {
        uint64_t timeout = (dev->state != CC110X_STATE_TX_COMPLETING) ?
                           2048 : 1024;
        /* Block until mutex is unlocked from ISR, or a timeout occurs. The
         * timeout prevents deadlocks when IRQs are lost. If the TX FIFO
         * still needs to be filled, the timeout is 1024 µs - or after 32 Byte
         * (= 50%) of the FIFO have been transmitted at 250 kbps. If
         * no additional data needs to be fed into the FIFO, a timeout of
         * 2048 µs is used instead to allow the frame to be completely drained
         * before the timeout triggers. The ISR handler is prepared to be
         * called prematurely, so we don't need to worry about extra calls
         * to cc110x_isr() introduced by accident.
         */
        xtimer_mutex_lock_timeout(&dev->isr_signal, timeout);
        cc110x_isr(&dev->netdev);
    }

    return (int)size;
}

/**
 * @brief   Checks if the CC110x's address filter is disabled
 * @param   dev     Transceiver to check if in promiscuous mode
 * @param   dest    Store the result here
 *
 * @return  Returns the size of @ref netopt_enable_t to confirm with the API
 *          in @ref netdev_driver_t::get
 */
static int cc110x_get_promiscuous_mode(cc110x_t *dev, netopt_enable_t *dest)
{
    cc110x_acquire(dev);

    uint8_t pktctrl1;
    cc110x_read(dev, CC110X_REG_PKTCTRL1, &pktctrl1);
    *dest = ((pktctrl1 & CC110X_PKTCTRL1_GET_ADDR_MODE) == CC110X_PKTCTRL1_ADDR_ALL);
    cc110x_release(dev);
    return sizeof(netopt_enable_t);
}

static int cc110x_get(netdev_t *netdev, netopt_t opt,
                      void *val, size_t max_len)
{
    cc110x_t *dev = (cc110x_t *)netdev;

    (void)max_len;  /* only used in assert() */
    switch (opt) {
    case NETOPT_DEVICE_TYPE:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)val) = NETDEV_TYPE_CC110X;
        return sizeof(uint16_t);
    case NETOPT_PROTO:
        assert(max_len == sizeof(gnrc_nettype_t));
        *((gnrc_nettype_t *)val) = CC110X_DEFAULT_PROTOCOL;
        return sizeof(gnrc_nettype_t);
    case NETOPT_MAX_PDU_SIZE:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)val) = CC110X_MAX_FRAME_SIZE - sizeof(cc1xxx_l2hdr_t);
        return sizeof(uint16_t);
    case NETOPT_ADDR_LEN:
    /* falls through */
    case NETOPT_SRC_LEN:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)val) = CC1XXX_ADDR_SIZE;
        return sizeof(uint16_t);
    case NETOPT_ADDRESS:
        assert(max_len >= CC1XXX_ADDR_SIZE);
        *((uint8_t *)val) = dev->addr;
        return CC1XXX_ADDR_SIZE;
    case NETOPT_CHANNEL:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)val) = dev->channel;
        return sizeof(uint16_t);
    case NETOPT_TX_POWER:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)val) = dbm_from_tx_power[dev->tx_power];
        return sizeof(uint16_t);
    case NETOPT_PROMISCUOUSMODE:
        assert(max_len == sizeof(netopt_enable_t));
        return cc110x_get_promiscuous_mode(dev, val);
    case NETOPT_STATE:
        assert(max_len == sizeof(netopt_state_t));
        {
            netopt_state_t *state = val;
            switch (dev->state) {
            case CC110X_STATE_RECEIVING:
            case CC110X_STATE_FRAME_READY:
            case CC110X_STATE_RXFIFO_OVERFLOW:
                *state = NETOPT_STATE_RX;
                break;
            case CC110X_STATE_IDLE:
                *state = NETOPT_STATE_STANDBY;
                break;
            case CC110X_STATE_OFF:
                *state = NETOPT_STATE_SLEEP;
                break;
            case CC110X_STATE_TX_MODE:
            case CC110X_STATE_TX_COMPLETING:
            case CC110X_STATE_TXFIFO_UNDERFLOW:
                *state = NETOPT_STATE_TX;
                break;
            case CC110X_STATE_RX_MODE:
                *state = NETOPT_STATE_IDLE;
                break;
            default:
                *state = NETOPT_STATE_RESET;
                break;
            }
        }
        break;
    default:
        break;
    }
    return -ENOTSUP;
}

/**
 * @brief   Set the given address as the device's layer 2 address
 *
 * @param   dev     Device descriptor of the transceiver
 * @param   addr    Address to set
 */
static int cc110x_set_addr(cc110x_t *dev, uint8_t addr)
{
    cc110x_acquire(dev);

    dev->addr = addr;
    cc110x_write(dev, CC110X_REG_ADDR, addr);
    cc110x_release(dev);
    return 1;
}

/**
 * @brief   Enables/disables the CC110x's address filter
 * @param   dev     Transceiver to turn promiscuous mode on/off
 * @param   enable  Whether to enable or disable promiscuous mode
 *
 * @return  Returns the size of @ref netopt_enable_t to confirm with the API
 *          in @ref netdev_driver_t::set
 */
static int cc110x_set_promiscuous_mode(cc110x_t *dev, netopt_enable_t enable)
{
    cc110x_acquire(dev);

    uint8_t pktctrl1 = CC110X_PKTCTRL1_VALUE;
    if (enable == NETOPT_ENABLE) {
        pktctrl1 |= CC110X_PKTCTRL1_ADDR_ALL;
    }
    else {
        pktctrl1 |= CC110X_PKTCTRL1_ADDR_MATCH;
    }
    cc110x_write(dev, CC110X_REG_PKTCTRL1, pktctrl1);
    cc110x_release(dev);
    return sizeof(netopt_enable_t);
}

static int cc110x_set(netdev_t *netdev, netopt_t opt,
                      const void *val, size_t len)
{
    (void)len;
    cc110x_t *dev = (cc110x_t *)netdev;

    switch (opt) {
    case NETOPT_ADDRESS:
        assert(len == CC1XXX_ADDR_SIZE);
        return cc110x_set_addr(dev, *((uint8_t *)val));
    case NETOPT_CHANNEL:
    {
        assert(len == sizeof(uint16_t));
        int retval;
        uint16_t channel = *((uint16_t *)val);
        if (channel >= CC110X_MAX_CHANNELS) {
            return -EINVAL;
        }
        if ((retval = cc110x_set_channel(dev, (uint8_t)channel))) {
            return retval;
        }
    }
        return sizeof(uint16_t);
    case NETOPT_TX_POWER:
    {
        assert(len == sizeof(int16_t));
        int16_t dbm = *((int16_t *)val);
        cc110x_tx_power_t power = CC110X_TX_POWER_MINUS_30_DBM;
        for ( ; power < CC110X_TX_POWER_PLUS_10_DBM; power++) {
            if ((int16_t)tx_power_from_dbm[power] >= dbm) {
                break;
            }
        }
        if (cc110x_set_tx_power(dev, power)) {
            return -EINVAL;
        }
    }
        return sizeof(uint16_t);
    case NETOPT_PROMISCUOUSMODE:
        assert(len == sizeof(netopt_enable_t));
        return cc110x_set_promiscuous_mode(dev, *((const netopt_enable_t *)val));
    case NETOPT_STATE:
        assert(len == sizeof(netopt_state_t));
        switch (*((netopt_state_t *)val)) {
        case NETOPT_STATE_RESET:
        case NETOPT_STATE_IDLE:
            cc110x_wakeup(dev);
            return sizeof(netopt_state_t);
        case NETOPT_STATE_OFF:
        case NETOPT_STATE_SLEEP:
            cc110x_sleep(dev);
            return sizeof(netopt_state_t);
        default:
            return -ENOTSUP;
        }
        break;
    default:
        return -ENOTSUP;
    }
}
