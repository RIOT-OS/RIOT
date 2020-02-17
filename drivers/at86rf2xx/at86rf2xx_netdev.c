/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Netdev adaption for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "iolist.h"

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "at86rf2xx.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"
#include "at86rf2xx_internal_common.h"
#include "at86rf2xx_internal.h"

#if IS_USED(MODULE_AT86RF212B)
#include "at86rf212b_internal.h"
#endif
#if IS_USED(MODULE_AT86RF231)
#include "at86rf231_internal.h"
#endif
#if IS_USED(MODULE_AT86RF232)
#include "at86rf232_internal.h"
#endif
#if IS_USED(MODULE_AT86RF233)
#include "at86rf233_internal.h"
#endif
#if IS_USED(MODULE_AT86RFA1)
#include "at86rfa1_internal.h"
#endif
#if IS_USED(MODULE_AT86RFR2)
#include "at86rfr2_internal.h"
#endif

#include "at86rf2xx_netdev.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len);

const netdev_driver_t at86rf2xx_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

#if IS_USED(MODULE_AT86RF2XX_PERIPH)
/* SOC has radio interrupts, store reference to netdev */
static netdev_t *at86rfmega_dev;
#endif

#if IS_USED(MODULE_AT86RF2XX_SPI)
static
void _irq_handler(void *arg)
{
    netdev_t *dev = (netdev_t *)arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV_EVENT_ISR);
    }
}
#endif

static
int _init(netdev_t *netdev)
{
    static int _count = 0;
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;

    DEBUG("[at86rf2xx] init. device #%d at %p\n", _count++, dev);
    switch (dev->base.dev_type) {
        default:
#if IS_USED(MODULE_AT86RF2XX_SPI)
            /* initialize GPIOs */
            gpio_init(dev->params.sleep_pin, GPIO_OUT);
            gpio_clear(dev->params.sleep_pin);
            gpio_init(dev->params.reset_pin, GPIO_OUT);
            gpio_set(dev->params.reset_pin);
            DEBUG("[at86rf2xx] initialize SPI based transceiver\n");
            spi_init_cs(dev->params.spi, dev->params.cs_pin);
            /* Intentionally check if bus can be acquired,
               since getbus() drops the return value */
            if (spi_acquire(dev->params.spi, dev->params.cs_pin,
                            SPI_MODE_0, dev->params.spi_clk) < 0) {
                DEBUG("[at86rf2xx] error: unable to acquire SPI bus\n");
                return -EIO;
            }
            spi_release(dev->params.spi);
            gpio_init_int(dev->params.int_pin, GPIO_IN,
                          GPIO_RISING, _irq_handler, dev);
#endif
            break;
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            at86rfmega_dev = netdev;
            break;
#endif
    }
    int valid = -ENOTSUP;
    /* test if the device is responding and
       reset device to default values and
       put it into idle state */
    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            at86rf212b_hardware_reset((at86rf212b_t *)dev);
            if ((valid = at86rf212b_validate((at86rf212b_t *)dev)) != 0) {
                return valid;
            }
            if (at86rf212b_get_status((at86rf212b_t *)dev) ==
                AT86RF2XX_STATE_P_ON) {
                at86rf212b_set_state((at86rf212b_t *)dev,
                                     AT86RF2XX_STATE_FORCE_TRX_OFF);
            }
            at86rf212b_reset((at86rf212b_t *)dev);
            at86rf212b_set_state((at86rf212b_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            at86rf231_hardware_reset((at86rf231_t *)dev);
            if ((valid = at86rf231_validate((at86rf231_t *)dev)) != 0) {
                return valid;
            }
            if (at86rf231_get_status((at86rf231_t *)dev) ==
                AT86RF2XX_STATE_P_ON) {
                at86rf231_set_state((at86rf231_t *)dev,
                                    AT86RF2XX_STATE_FORCE_TRX_OFF);
            }
            at86rf231_reset((at86rf231_t *)dev);
            at86rf231_set_state((at86rf231_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            at86rf232_hardware_reset((at86rf232_t *)dev);
            if ((valid = at86rf232_validate((at86rf232_t *)dev)) != 0) {
                return valid;
            }
            if (at86rf232_get_status((at86rf232_t *)dev) ==
                AT86RF2XX_STATE_P_ON) {
                at86rf232_set_state((at86rf232_t *)dev,
                                    AT86RF2XX_STATE_FORCE_TRX_OFF);
            }
            at86rf232_reset((at86rf232_t *)dev);
            at86rf232_set_state((at86rf232_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            at86rf233_hardware_reset((at86rf233_t *)dev);
            if ((valid = at86rf233_validate((at86rf233_t *)dev)) != 0) {
                return valid;
            }
            if (at86rf233_get_status((at86rf233_t *)dev) ==
                AT86RF2XX_STATE_P_ON) {
                at86rf233_set_state((at86rf233_t *)dev,
                                    AT86RF2XX_STATE_FORCE_TRX_OFF);
            }
            at86rf233_reset((at86rf233_t *)dev);
            at86rf233_set_state((at86rf233_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rfa1_hardware_reset((at86rfa1_t *)dev);
            if ((valid = at86rfa1_validate((at86rfa1_t *)dev)) != 0) {
                return valid;
            }
            if (at86rfa1_get_status((at86rfa1_t *)dev) ==
                AT86RF2XX_STATE_P_ON) {
                at86rfa1_set_state((at86rfa1_t *)dev,
                                   AT86RF2XX_STATE_FORCE_TRX_OFF);
            }
            at86rfa1_reset((at86rfa1_t *)dev);
            at86rfa1_set_state((at86rfa1_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rfr2_hardware_reset((at86rfr2_t *)dev);
            if ((valid = at86rfr2_validate((at86rfr2_t *)dev)) != 0) {
                return valid;
            }
            if (at86rfr2_get_status((at86rfr2_t *)dev) ==
                AT86RF2XX_STATE_P_ON) {
                at86rfr2_set_state((at86rfr2_t *)dev,
                                   AT86RF2XX_STATE_FORCE_TRX_OFF);
            }
            at86rfr2_reset((at86rfr2_t *)dev);
            at86rfr2_set_state((at86rfr2_t *)dev, dev->base.idle_state);
            break;
        }
#endif
    }

    return valid < 0 ? valid : 0;
}

static
void _set_state(at86rf2xx_t *dev, uint8_t state)
{
    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B:
            state = at86rf212b_set_state((at86rf212b_t *)dev, state);
            break;
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231:
            state = at86rf231_set_state((at86rf231_t *)dev, state);
            break;
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232:
            state = at86rf232_set_state((at86rf232_t *)dev, state);
            break;
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233:
            state = at86rf233_set_state((at86rf233_t *)dev, state);
            break;
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
            state = at86rfa1_set_state((at86rfa1_t *)dev, state);
            break;
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            state = at86rfr2_set_state((at86rfr2_t *)dev, state);
            break;
#endif
    }
}

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in,out] dev        device to prepare for sending
 */
static
void at86rf2xx_tx_prepare(at86rf2xx_t *dev)
{
    uint8_t state = at86rf2xx_get_status(dev);

    dev->base.pending_tx++;
    _set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
    /**
     * Check what state is going to becomes the idle state
     * (the state we return to, after sending).
     * When sending rapidly:
     * Assume the transceiver is going to send some data.
     * So it changes in TX_ARET_ON, then in BUSY_TX_ARET.
     * Assume this function gets called, while the trancseiver
     * is still busy. Then the idle state must not
     * become any BUSY_* state, because switching to that
     * idle state would request an impossible
     * state change, after the send completes.
     */
    if (state == AT86RF2XX_STATE_RX_AACK_ON ||
        state == AT86RF2XX_STATE_RX_ON      ||
        state == AT86RF2XX_STATE_TRX_OFF    ||
        state == AT86RF2XX_STATE_SLEEP) {
        dev->base.idle_state = state;
    }
    dev->base.tx_frame_len = 0;
}

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in,out] dev       device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 * @param[in] offset        offset used when writing data to internal buffer
 *
 * @return                  offset + number of bytes written
 */
static
size_t at86rf2xx_tx_load(at86rf2xx_t *dev, const uint8_t *data,
                         size_t len, size_t offset)
{
    /* offset + 1 because first byte is the frame length (PHR) */
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    dev->base.tx_frame_len += (uint8_t)len;
    return offset + len;
}

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
static
void at86rf2xx_tx_exec(at86rf2xx_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    dev->base.tx_frame_len += IEEE802154_FCS_LEN;
    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->base.tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
    if (netdev->event_callback &&
        (dev->base.flags & AT86RF2XX_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    }
}

static
int _send(netdev_t *netdev, const iolist_t *iolist)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;

    at86rf2xx_tx_prepare(dev);

    /* load packet data into FIFO */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if (dev->base.tx_frame_len
            + iol->iol_len
            + IEEE802154_FCS_LEN > AT86RF2XX_MAX_PKT_LENGTH) {
            DEBUG("[at86rf2xx] error: \
                  frame too large (loaded: %u byte) to be send\n",
                  (unsigned)dev->base.tx_frame_len + 2);
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            at86rf2xx_tx_load(dev, iol->iol_base, iol->iol_len,
                              dev->base.tx_frame_len);
        }
    }

    /* send data out directly if pre-loading id disabled */
    if (!(dev->base.flags & AT86RF2XX_OPT_PRELOADING)) {
        at86rf2xx_tx_exec(dev);
    }
    /* return the number of bytes that were actually loaded into the frame
     * buffer/send out */
    return (int)dev->base.tx_frame_len;
}

static
int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;
    size_t frame_len = 0;

    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            /* frame buffer protection will be unlocked as soon as
             * at86rf2xx_fb_stop() is called.
             * Set receiver to PLL_ON state to be able to free the
             * SPI bus and avoid losing data. */
            at86rf212b_set_state((at86rf212b_t *)dev, AT86RF2XX_STATE_PLL_ON);
            /* ignore MSB (refer p.80) and subtract length of FCS field */
            frame_len = (at86rf212b_fb_read_phr((at86rf212b_t *)dev) & 0x7F)
                        - IEEE802154_FCS_LEN;
            /* return length when buf == NULL */
            if (!buf) {
                /* drop packet, continue receiving */
                if (len) {
                    /* set device back in operation state which was used
                     * before last transmission.
                     * This state is saved in
                     * at86rf2xx.c/at86rf2xx_tx_prepare() e.g RX_AACK_ON */
                    at86rf212b_set_state((at86rf212b_t *)dev,
                                         dev->base.idle_state);
                }
                return frame_len;
            }
            /* not enough space in buf */
            if (frame_len > len) {
                /* set device back in operation state which was used
                 * before last transmission.
                 * This state is saved in
                 * at86rf2xx.c/at86rf2xx_tx_prepare() e.g RX_AACK_ON */
                at86rf212b_set_state((at86rf212b_t *)dev, dev->base.idle_state);
                return -ENOBUFS;
            }
            at86rf212b_fb_t fb;
            at86rf212b_fb_read((at86rf212b_t *)dev, &fb, buf, len);
            if (info) {
                *((netdev_ieee802154_rx_info_t *)info) =
                    (netdev_ieee802154_rx_info_t) {
                    .rssi = AT86RF212B_RSSI_BASE_VAL + fb.ed,
                    .lqi = fb.lqi
                };
                DEBUG("[at86rf212b]\n\
                      LQI: %d high is good,\n\
                      RSSI: %d high is either good or too much interference.\n",
                      ((netdev_ieee802154_rx_info_t *)info)->lqi,
                      ((netdev_ieee802154_rx_info_t *)info)->rssi);
            }
            /* set device back in operation state which was used before last transmission.
             * This state is saved in at86rf2xx.c/at86rf2xx_tx_prepare() e.g RX_AACK_ON */
            at86rf212b_set_state((at86rf212b_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            at86rf231_set_state((at86rf231_t *)dev, AT86RF2XX_STATE_PLL_ON);
            frame_len = (at86rf231_fb_read_phr((at86rf231_t *)dev) & 0x7F)
                        - IEEE802154_FCS_LEN;
            if (!buf) {
                if (len) {
                    at86rf231_set_state((at86rf231_t *)dev,
                                        dev->base.idle_state);
                }
                return frame_len;
            }
            if (frame_len > len) {
                at86rf231_set_state((at86rf231_t *)dev, dev->base.idle_state);
                return -ENOBUFS;
            }
            at86rf231_fb_t fb;
            at86rf231_fb_read((at86rf231_t *)dev, &fb, buf, len);
            if (info) {
                *((netdev_ieee802154_rx_info_t *)info) =
                    (netdev_ieee802154_rx_info_t) {
                    .rssi = AT86RF231_RSSI_BASE_VAL +
                            at86rf2xx_spi_reg_read(dev,
                                                   AT86RF2XX_REG__PHY_ED_LEVEL),
                    .lqi = fb.lqi
                };
                DEBUG("[at86rf231]\n\
                      LQI: %d high is good,\n\
                      RSSI: %d high is either good or too much interference.\n",
                      ((netdev_ieee802154_rx_info_t *)info)->lqi,
                      ((netdev_ieee802154_rx_info_t *)info)->rssi);
            }
            at86rf231_set_state((at86rf231_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            at86rf232_set_state((at86rf232_t *)dev, AT86RF2XX_STATE_PLL_ON);
            frame_len = (at86rf232_fb_read_phr((at86rf232_t *)dev) & 0x7F)
                        - IEEE802154_FCS_LEN;
            if (!buf) {
                if (len) {
                    at86rf232_set_state((at86rf232_t *)dev,
                                        dev->base.idle_state);
                }
                return frame_len;
            }
            if (frame_len > len) {
                at86rf232_set_state((at86rf232_t *)dev, dev->base.idle_state);
                return -ENOBUFS;
            }
            at86rf232_fb_t fb;
            at86rf232_fb_read((at86rf232_t *)dev, &fb, buf, len);
            if (info) {
                *((netdev_ieee802154_rx_info_t *)info) =
                    (netdev_ieee802154_rx_info_t) {
                    .rssi = AT86RF232_RSSI_BASE_VAL + fb.ed,
                    .lqi = fb.lqi
                };
                DEBUG("[at86rf232]\n\
                      LQI: %d high is good\n\
                      RSSI: %d high is either good or too much interference.\n",
                      ((netdev_ieee802154_rx_info_t *)info)->lqi,
                      ((netdev_ieee802154_rx_info_t *)info)->rssi);
            }
            at86rf232_set_state((at86rf232_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            at86rf233_set_state((at86rf233_t *)dev, AT86RF2XX_STATE_PLL_ON);
            frame_len = (at86rf233_fb_read_phr((at86rf233_t *)dev) & 0x7F)
                        - IEEE802154_FCS_LEN;
            if (!buf) {
                if (len) {
                    at86rf233_set_state((at86rf233_t *)dev,
                                        dev->base.idle_state);
                }
                return frame_len;
            }
            if (frame_len > len) {
                at86rf233_set_state((at86rf233_t *)dev, dev->base.idle_state);
                return -ENOBUFS;
            }
            at86rf233_fb_t fb;
            at86rf233_fb_read((at86rf233_t *)dev, &fb, buf, len);
            if (info) {
                *((netdev_ieee802154_rx_info_t *)info) =
                    (netdev_ieee802154_rx_info_t) {
                    .rssi = AT86RF233_RSSI_BASE_VAL + fb.ed,
                    .lqi = fb.lqi
                };
                DEBUG("[at86rf233]\n\
                      LQI: %d high is good\n\
                      RSSI: %d high is either good or too much interference.\n",
                      ((netdev_ieee802154_rx_info_t *)info)->lqi,
                      ((netdev_ieee802154_rx_info_t *)info)->rssi);
            }
            at86rf233_set_state((at86rf233_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rfa1_set_state((at86rfa1_t *)dev, AT86RF2XX_STATE_PLL_ON);
            frame_len = (at86rfa1_fb_read_phr((at86rfa1_t *)dev) & 0x7F)
                        - IEEE802154_FCS_LEN;
            if (!buf) {
                if (len) {
                    at86rfa1_set_state((at86rfa1_t *)dev, dev->base.idle_state);
                }
                return frame_len;
            }
            if (frame_len > len) {
                at86rfa1_set_state((at86rfa1_t *)dev, dev->base.idle_state);
                return -ENOBUFS;
            }
            at86rfa1_fb_t fb;
            at86rfa1_fb_read((at86rfa1_t *)dev, &fb, buf, len);
            if (info) {
                *((netdev_ieee802154_rx_info_t *)info) =
                    (netdev_ieee802154_rx_info_t) {
                    .rssi = AT86RFA1_RSSI_BASE_VAL +
                            at86rf2xx_periph_reg_read(AT86RFA1_REG(
                                                          AT86RF2XX_REG__PHY_ED_LEVEL)),
                    .lqi = fb.lqi
                };
                DEBUG("[at86rfa1]\n\
                      LQI: %d high is good\n\
                      RSSI: %d high is either good or too much interference.\n",
                      ((netdev_ieee802154_rx_info_t *)info)->lqi,
                      ((netdev_ieee802154_rx_info_t *)info)->rssi);
            }
            at86rfa1_set_state((at86rfa1_t *)dev, dev->base.idle_state);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rfr2_set_state((at86rfr2_t *)dev, AT86RF2XX_STATE_PLL_ON);
            frame_len = (at86rfr2_fb_read_phr((at86rfr2_t *)dev) & 0x7F)
                        - IEEE802154_FCS_LEN;
            if (!buf) {
                if (len) {
                    at86rfr2_set_state((at86rfr2_t *)dev, dev->base.idle_state);
                }
                return frame_len;
            }
            if (frame_len > len) {
                at86rfr2_set_state((at86rfr2_t *)dev, dev->base.idle_state);
                return -ENOBUFS;
            }
            at86rfr2_fb_t fb;
            at86rfr2_fb_read((at86rfr2_t *)dev, &fb, buf, len);
            if (info) {
                *((netdev_ieee802154_rx_info_t *)info) =
                    (netdev_ieee802154_rx_info_t) {
                    .rssi = AT86RFR2_RSSI_BASE_VAL +
                            at86rf2xx_periph_reg_read(AT86RFR2_REG(
                                                          AT86RF2XX_REG__PHY_ED_LEVEL)),
                    .lqi = fb.lqi
                };
                DEBUG("[at86rfr2]\n\
                      LQI: %d high is good\n\
                      RSSI: %d high is either good or too much interference.\n",
                      ((netdev_ieee802154_rx_info_t *)info)->lqi,
                      ((netdev_ieee802154_rx_info_t *)info)->rssi);
            }
            at86rfr2_set_state((at86rfr2_t *)dev, dev->base.idle_state);
            break;
        }
#endif
    }

    return frame_len;
}

static
int _set_state_netopt(at86rf2xx_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_STANDBY:
            _set_state(dev, AT86RF2XX_STATE_TRX_OFF);
            break;
        case NETOPT_STATE_SLEEP:
            _set_state(dev, AT86RF2XX_STATE_SLEEP);
            break;
        case NETOPT_STATE_IDLE:
            _set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);
            break;
        case NETOPT_STATE_TX:
            if (dev->base.flags & AT86RF2XX_OPT_PRELOADING) {
                /* The netdev driver ISR switches the transceiver back to the
                 * previous idle state after a completed TX. If the user tries
                 * to initiate another transmission (retransmitting the same data)
                 * without first going to TX_ARET_ON, the command to start TX
                 * would be ignored, leading to a deadlock in this netdev driver
                 * thread.
                 * Additionally, avoids driver thread deadlock when PRELOADING
                 * is set and the user tries to initiate TX without first calling
                 * send() to write some frame data.
                 */
                if (dev->base.pending_tx == 0) {
                    /* retransmission of old data, at86rf2xx_tx_prepare normally
                     * increments this and the ISR for TX_END decrements it, to
                     * know when to switch back to the idle state. */
                    ++dev->base.pending_tx;
                }
                _set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
                at86rf2xx_tx_exec(dev);
            }
            break;
        case NETOPT_STATE_RESET:
            _set_state(dev, AT86RF2XX_STATE_RESET);
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static
netopt_state_t _get_state(at86rf2xx_t *dev)
{
    switch (at86rf2xx_get_status(dev)) {
        case AT86RF2XX_STATE_SLEEP:
            return NETOPT_STATE_SLEEP;
        case AT86RF2XX_STATE_TRX_OFF:
            return NETOPT_STATE_STANDBY;
        case AT86RF2XX_STATE_BUSY_RX_AACK:
            return NETOPT_STATE_RX;
        case AT86RF2XX_STATE_BUSY_TX_ARET:
        case AT86RF2XX_STATE_TX_ARET_ON:
            return NETOPT_STATE_TX;
        case AT86RF2XX_STATE_RX_AACK_ON:
        default:
            return NETOPT_STATE_IDLE;
    }
}

static
int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    /* getting these options doesn't require the transceiver to be responsive */
    switch (opt) {
        case NETOPT_CHANNEL_PAGE:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = dev->base.netdev.page;
            return sizeof(uint16_t);

        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_PRELOADING:
            if (dev->base.flags & AT86RF2XX_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_PROMISCUOUSMODE:
            if (dev->base.flags & AT86RF2XX_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->base.flags & AT86RF2XX_OPT_TELL_RX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->base.flags & AT86RF2XX_OPT_TELL_RX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->base.flags & AT86RF2XX_OPT_TELL_TX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->base.flags & AT86RF2XX_OPT_TELL_TX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA:
            *((netopt_enable_t *)val) =
                !!(dev->base.flags & AT86RF2XX_OPT_CSMA);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_RETRIES_NEEDED:
            assert(max_len >= sizeof(uint8_t));
            switch (dev->base.dev_type) {
                default:
                    return -ENOTSUP;
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    *((uint8_t *)val) =
                        at86rf232_get_tx_retries((at86rf232_t *)dev);
                    return sizeof(uint8_t);
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    *((uint8_t *)val) =
                        at86rf233_get_tx_retries((at86rf233_t *)dev);
                    return sizeof(uint8_t);
#endif
            }
        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val,
                                      max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    uint8_t old_state = at86rf2xx_get_status(dev);

    /* temporarily wake up if sleeping */
    if (old_state == AT86RF2XX_STATE_SLEEP) {
        _set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    }
    /* these options require the transceiver to be not sleeping*/
    switch (opt) {
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    *((uint16_t *)val) = at86rf212b_get_txpower(
                        (at86rf212b_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    *((uint16_t *)val) = at86rf231_get_txpower(
                        (at86rf231_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    *((uint16_t *)val) = at86rf232_get_txpower(
                        (at86rf232_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    *((uint16_t *)val) = at86rf233_get_txpower(
                        (at86rf233_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    *((uint16_t *)val) =
                        at86rfa1_get_txpower((at86rfa1_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2:
                    *((uint16_t *)val) =
                        at86rfr2_get_txpower((at86rfr2_t *)dev);
                    break;
#endif
            }
            res = sizeof(uint16_t);
            break;

        case NETOPT_RETRANS:
            assert(max_len == sizeof(uint8_t));
            *((uint8_t *)val) = at86rf2xx_get_frame_max_retries(dev);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CSMA_RETRIES:
            assert(max_len == sizeof(uint8_t));
            *((uint8_t *)val) = at86rf2xx_get_csma_max_retries(dev);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(max_len == sizeof(int8_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    *((int8_t *)val) =
                        at86rf212b_get_cca_threshold((at86rf212b_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    *((int8_t *)val) =
                        at86rf231_get_cca_threshold((at86rf231_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    *((int8_t *)val) =
                        at86rf232_get_cca_threshold((at86rf232_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    *((int8_t *)val) =
                        at86rf233_get_cca_threshold((at86rf233_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    *((int8_t *)val) = at86rfa1_get_cca_threshold(
                        (at86rfa1_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2:
                    *((int8_t *)val) = at86rfr2_get_cca_threshold(
                        (at86rfr2_t *)dev);
                    break;
#endif
            }
            res = sizeof(int8_t);
            break;

        case NETOPT_IS_CHANNEL_CLR:
            assert(max_len == sizeof(netopt_enable_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    *((netopt_enable_t *)val) = at86rf212b_cca(
                        (at86rf212b_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    *((netopt_enable_t *)val) =
                        at86rf231_cca((at86rf231_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    *((netopt_enable_t *)val) =
                        at86rf232_cca((at86rf232_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    *((netopt_enable_t *)val) =
                        at86rf233_cca((at86rf233_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    *((netopt_enable_t *)val) = at86rfa1_cca((at86rfa1_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2:
                    *((netopt_enable_t *)val) = at86rfr2_cca((at86rfr2_t *)dev);
                    break;
#endif
            }
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_LAST_ED_LEVEL:
            assert(max_len >= sizeof(int8_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    *((int8_t *)val) = at86rf212b_get_ed_level(
                        (at86rf212b_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    *((int8_t *)val) =
                        at86rf231_get_ed_level((at86rf231_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    *((int8_t *)val) =
                        at86rf232_get_ed_level((at86rf232_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    *((int8_t *)val) =
                        at86rf233_get_ed_level((at86rf233_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    *((int8_t *)val) = at86rfa1_get_ed_level((at86rfa1_t *)dev);
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2:
                    *((int8_t *)val) = at86rfr2_get_ed_level((at86rfr2_t *)dev);
                    break;
#endif
            }
            res = sizeof(int8_t);
            break;

        case NETOPT_AUTOACK:
            assert(max_len == sizeof(netopt_enable_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B) || \
    IS_USED(MODULE_AT86RF231)  || \
    IS_USED(MODULE_AT86RF232)  || \
    IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                case AT86RF2XX_DEV_TYPE_AT86RF233: {
                    uint8_t csma_seed_1 =
                        at86rf2xx_spi_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
                    *((netopt_enable_t *)val) =
                        (csma_seed_1 & AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                            ? false
                            : true;
                    break;
                }
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1: {
                    uint8_t csma_seed_1 =
                        at86rf2xx_periph_reg_read(AT86RFA1_REG(
                                                      AT86RF2XX_REG__CSMA_SEED_1));
                    *((netopt_enable_t *)val) =
                        (csma_seed_1 & AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                            ? false
                            : true;
                    break;
                }
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2: {
                    uint8_t csma_seed_1 =
                        at86rf2xx_periph_reg_read(AT86RFR2_REG(
                                                      AT86RF2XX_REG__CSMA_SEED_1));
                    *((netopt_enable_t *)val) =
                        (csma_seed_1 & AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                            ? false
                            : true;
                    break;
                }
#endif
            }
            res = sizeof(netopt_enable_t);
            break;

        default:
            res = -ENOTSUP;
            break;
    }

    /* go back to sleep if were sleeping */
    if (old_state == AT86RF2XX_STATE_SLEEP) {
        _set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    return res;
}

static
int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;
    uint8_t old_state = at86rf2xx_get_status(dev);
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* temporarily wake up if sleeping and opt != NETOPT_STATE.
     * opt != NETOPT_STATE check prevents redundant wake-up.
     * when opt == NETOPT_STATE, at86rf2xx_set_state() will wake up the
     * radio if needed. */
    if ((old_state == AT86RF2XX_STATE_SLEEP) && (opt != NETOPT_STATE)) {
        _set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len == sizeof(network_uint16_t));
            at86rf2xx_set_addr_short(dev, val);
            res = sizeof(network_uint16_t);
            break;
        case NETOPT_ADDRESS_LONG:
            assert(len == sizeof(eui64_t));
            at86rf2xx_set_addr_long(dev, val);
            res = sizeof(eui64_t);
            break;
        case NETOPT_NID:
            assert(len == sizeof(uint16_t));
            at86rf2xx_set_pan(dev, *((const uint16_t *)val));
            res = sizeof(uint16_t);
            break;
        case NETOPT_CHANNEL:
            assert(len == sizeof(uint16_t));
            uint8_t chan = (uint8_t)(*((const uint16_t *)val));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    at86rf212b_set_channel((at86rf212b_t *)dev,
                                           dev->base.netdev.page,
                                           chan);
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    at86rf231_set_channel((at86rf231_t *)dev, chan);
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    at86rf232_set_channel((at86rf232_t *)dev, chan);
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    at86rf233_set_channel((at86rf233_t *)dev, chan);
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    at86rfa1_set_channel((at86rfa1_t *)dev, chan);
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2:
                    at86rfr2_set_channel((at86rfr2_t *)dev, chan);
                    break;
#endif
            }
            res = sizeof(uint16_t);
            break;

        case NETOPT_CHANNEL_PAGE:
            assert(len == sizeof(uint16_t));
            uint8_t page = (uint8_t)(*((const uint16_t *)val));
            switch (dev->base.dev_type) {
                default:
                    (void)page;
                    dev->base.netdev.page = 0;
                    break;
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    at86rf212b_set_channel((at86rf212b_t *)dev,
                                           page,
                                           dev->base.netdev.chan);
                    break;
#endif
            }
            res = sizeof(uint16_t);
            break;

        case NETOPT_TX_POWER:
            assert(len == sizeof(int16_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    at86rf212b_set_txpower((at86rf212b_t *)dev,
                                           *((const int16_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    at86rf231_set_txpower((at86rf231_t *)dev,
                                          *((const int16_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    at86rf232_set_txpower((at86rf232_t *)dev,
                                          *((const int16_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    at86rf233_set_txpower((at86rf233_t *)dev,
                                          *((const int16_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    at86rfa1_set_txpower((at86rfa1_t *)dev,
                                         *((const int16_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2:
                    at86rfr2_set_txpower((at86rfr2_t *)dev,
                                         *((const int16_t *)val));
                    break;
#endif
            }
            res = sizeof(uint16_t);
            break;

        case NETOPT_STATE:
            assert(len == sizeof(netopt_state_t));
            res = _set_state_netopt(dev, *((const netopt_state_t *)val));
            break;

        case NETOPT_AUTOACK:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_ACK_PENDING:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_ACK_PENDING,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RETRANS:
            assert(len == sizeof(uint8_t));
            at86rf2xx_set_frame_max_retries(dev, *((const uint8_t *)val));
            res = sizeof(uint8_t);
            break;

        case NETOPT_PRELOADING:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PRELOADING,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PROMISCUOUS,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_START_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_END_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_START_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_START,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_END_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_END,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            assert(len == sizeof(uint8_t));
            if (!(dev->base.flags & AT86RF2XX_OPT_CSMA)) {
                /* If CSMA is disabled, don't allow setting retries */
                res = -EINVAL;
            }
            else {
                at86rf2xx_set_csma_max_retries(dev, *((const uint8_t *)val));
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(len == sizeof(int8_t));
            switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
                case AT86RF2XX_DEV_TYPE_AT86RF212B:
                    at86rf212b_set_cca_threshold((at86rf212b_t *)dev,
                                                 *((const int8_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RF231)
                case AT86RF2XX_DEV_TYPE_AT86RF231:
                    at86rf231_set_cca_threshold((at86rf231_t *)dev,
                                                *((const int8_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RF232)
                case AT86RF2XX_DEV_TYPE_AT86RF232:
                    at86rf232_set_cca_threshold((at86rf232_t *)dev,
                                                *((const int8_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RF233)
                case AT86RF2XX_DEV_TYPE_AT86RF233:
                    at86rf233_set_cca_threshold((at86rf233_t *)dev,
                                                *((const int8_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    at86rfa1_set_cca_threshold((at86rfa1_t *)dev,
                                               *((const int8_t *)val));
                    break;
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFA1:
                    at86rfr2_set_cca_threshold((at86rfr2_t *)dev,
                                               *((const int8_t *)val));
                    break;
#endif
            }
            res = sizeof(int8_t);
            break;

        default:
            break;
    }

    /* go back to sleep if were sleeping and state hasn't been changed */
    if ((old_state == AT86RF2XX_STATE_SLEEP)
        && (opt != NETOPT_STATE)) {
        _set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    if (res == -ENOTSUP) {
        res =
            netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt, val, len);
    }

    return res;
}

static
inline uint8_t _get_irq_status(at86rf2xx_t *dev)
{
    uint8_t irq_mask;

    switch (dev->base.dev_type) {
        default:
            irq_mask = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
            irq_mask = at86rfa1_get_irq_status((at86rfa1_t *)dev);
            ((at86rfa1_t *)dev)->irq_status = 0;
            break;
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            irq_mask = at86rfr2_get_irq_status((at86rfr2_t *)dev);
            ((at86rfr2_t *)dev)->irq_status = 0;
            break;
#endif
    }
    return irq_mask;
}

static
void _isr_send_complete(at86rf2xx_t *dev, uint8_t trac_status) {
    netdev_t *netdev = &dev->base.netdev.netdev;
    switch (dev->base.dev_type) {
        default:
            break;
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232:
            ((at86rf232_t *)dev)->tx_retries =
                at86rf232_get_frame_retries((at86rf232_t *)dev);
            break;
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233:
            ((at86rf233_t *)dev)->tx_retries =
                at86rf233_get_frame_retries((at86rf233_t *)dev);
            break;
#endif
    }

    DEBUG("[at86rf2xx] EVT - TX_END\n");

    switch (trac_status) {
#if IS_USED(MODULE_OPENTHREAD)
        case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
            if (netdev->event_callback &&
                (dev->base.flags & AT86RF2XX_OPT_TELL_TX_END)) {
                netdev->event_callback(netdev,
                                        NETDEV_EVENT_TX_COMPLETE);
            }
            DEBUG("[at86rf2xx] TX SUCCESS\n");
            break;
        case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
            if (netdev->event_callback &&
                (dev->base.flags & AT86RF2XX_OPT_TELL_TX_END)) {
                netdev->event_callback(netdev,
                                        NETDEV_EVENT_TX_COMPLETE_DATA_PENDING);
            }
            DEBUG("[at86rf2xx] TX SUCCESS DATA PENDING\n");
            break;
#else
        case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
        case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
            if (netdev->event_callback &&
                (dev->base.flags & AT86RF2XX_OPT_TELL_TX_END)) {
                netdev->event_callback(netdev,
                                        NETDEV_EVENT_TX_COMPLETE);
            }
            DEBUG("[at86rf2xx] TX SUCCESS\n");
            break;
#endif
        case AT86RF2XX_TRX_STATE__TRAC_NO_ACK:
            if (netdev->event_callback &&
                (dev->base.flags & AT86RF2XX_OPT_TELL_TX_END)) {
                netdev->event_callback(netdev, NETDEV_EVENT_TX_NOACK);
            }
            DEBUG("[at86rf2xx] TX NO_ACK\n");
            break;
        case AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
            if (netdev->event_callback &&
                (dev->base.flags & AT86RF2XX_OPT_TELL_TX_END)) {
                netdev->event_callback(netdev,
                                        NETDEV_EVENT_TX_MEDIUM_BUSY);
            }
            DEBUG("[at86rf2xx] TX_CHANNEL_ACCESS_FAILURE\n");
            break;
        default:
            DEBUG("[at86rf2xx] Unhandled TRAC_STATUS: %d\n",
                    trac_status >> 5);
    }
}

static
void _isr(netdev_t *netdev)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;
    uint8_t state;
    uint8_t irq_mask;
    uint8_t trac_status;

    /* If transceiver is sleeping register access is impossible and frames are
     * lost anyway, so return immediately.
     */
    state = at86rf2xx_get_status(dev);
    if (state == AT86RF2XX_STATE_SLEEP) {
        return;
    }

    /* read (consume) device status */
    irq_mask = _get_irq_status(dev);

    trac_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATE)
                  & AT86RF2XX_TRX_STATE_MASK__TRAC_STATUS;

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        if (dev->base.flags & AT86RF2XX_OPT_TELL_RX_START) {
            netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
        }
        DEBUG("[at86rf2xx] EVT - RX_START\n");
    }

    uint8_t en_irq_mask;
    switch (dev->base.dev_type) {
        default:
            en_irq_mask = AT86RF2XX_IRQ_STATUS_MASK__TRX_END;
            break;
#if IS_USED(MODULE_AT86RFA1) || \
    IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            en_irq_mask = AT86RF2XX_IRQ_STATUS_MASK__TX_END |
                          AT86RF2XX_IRQ_STATUS_MASK__RX_END;
            break;
#endif
    }

    if (irq_mask & en_irq_mask) {
        if ((state == AT86RF2XX_STATE_RX_AACK_ON)
            || (state == AT86RF2XX_STATE_BUSY_RX_AACK)) {
            DEBUG("[at86rf2xx] EVT - RX_END\n");
            if (!(dev->base.flags & AT86RF2XX_OPT_TELL_RX_END)) {
                return;
            }
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
        else if (state == AT86RF2XX_STATE_TX_ARET_ON) {
            /* check for more pending TX calls and return to idle state if
             * there are none */
            assert(dev->base.pending_tx != 0);
            /* Radio is idle, any TX transaction is done */
            dev->base.pending_tx = 0;
            _set_state(dev, dev->base.idle_state);
            DEBUG("[at86rf2xx] return to idle state 0x%x\n", dev->base.idle_state);
            _isr_send_complete(dev, trac_status);
        }
        /* Only the case when an interrupt was received and the radio is busy
         * with a next PDU transmission when _isr is called.
         * dev->pending == 1 means a receive and immediately a send happened.
         * The receive is discarded as the send already overwrote the internal
         * buffer.
         * dev->pending == 2 means two transmits occurred and this is the isr for
         * the first.
         */
        else if (state == AT86RF2XX_STATE_BUSY_TX_ARET) {
            if (dev->base.pending_tx > 1) {
                dev->base.pending_tx--;
                _isr_send_complete(dev, trac_status);
            }
        }
    }
}

#if IS_USED(MODULE_AT86RF2XX_PERIPH)

/**
 * @brief ISR for transceiver's receive end interrupt
 *
 *  Is triggered when valid data is received. FCS check passed.
 *  Save IRQ status and inform upper layer of data reception.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_RX_END_vect, ISR_BLOCK)
{
    atmega_enter_isr();

#if IS_USED(MODULE_AT86RFA1)
    uint8_t status = (*AT86RFA1_REG(AT86RF2XX_REG__TRX_STATE))
                     & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_END 0x%x\n", status);
    ((at86rfa1_t *)at86rfmega_dev)->irq_status
        |= AT86RF2XX_IRQ_STATUS_MASK__RX_END;
#elif IS_USED(MODULE_AT86RFR2)
    uint8_t status = (*AT86RFR2_REG(AT86RF2XX_REG__TRX_STATE))
                     & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_END 0x%x\n", status);
    ((at86rfr2_t *)at86rfmega_dev)->irq_status
        |= AT86RF2XX_IRQ_STATUS_MASK__RX_END;
#endif
    /* Call upper layer to process received data */
    at86rfmega_dev->event_callback(at86rfmega_dev, NETDEV_EVENT_ISR);

    atmega_exit_isr();
}

/**
 * @brief  Transceiver Frame Address Match, indicates incoming frame
 *
 *  Is triggered when Frame with valid Address is received.
 *  Can be used to wake up MCU from sleep, etc.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_XAH_AMI_vect, ISR_BLOCK)
{
    atmega_enter_isr();

    DEBUG("TRX24_XAH_AMI\n");

#if IS_USED(MODULE_AT86RFA1)
    ((at86rfa1_t *)at86rfmega_dev)->irq_status
        |= AT86RF2XX_IRQ_STATUS_MASK__AMI;
#elif IS_USED(MODULE_AT86RFR2)
    ((at86rfr2_t *)at86rfmega_dev)->irq_status
        |= AT86RF2XX_IRQ_STATUS_MASK__AMI;
#endif

    atmega_exit_isr();
}

/**
 * @brief ISR for transceiver's transmit end interrupt
 *
 *  Is triggered when data or when acknowledge frames where send.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_TX_END_vect, ISR_BLOCK)
{
    atmega_enter_isr();

    /* only inform upper layer when a transmission was done,
     * not for sending acknowledge frames if data was received. */
#if IS_USED(MODULE_AT86RFA1)
    uint8_t status = (*AT86RFA1_REG(AT86RF2XX_REG__TRX_STATE))
                     & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_TX_END 0x%x\n", status);
    if (status != AT86RF2XX_STATE_RX_AACK_ON) {
        ((at86rfa1_t *)at86rfmega_dev)->irq_status
            |= AT86RF2XX_IRQ_STATUS_MASK__TX_END;
    }
#elif IS_USED(MODULE_AT86RFR2)
    uint8_t status = (*AT86RFR2_REG(AT86RF2XX_REG__TRX_STATE))
                     & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_TX_END 0x%x\n", status);
    if (status != AT86RF2XX_STATE_RX_AACK_ON) {
        ((at86rfr2_t *)at86rfmega_dev)->irq_status
            |= AT86RF2XX_IRQ_STATUS_MASK__TX_END;
    }
#endif
    /* Call upper layer to process if data was send successful */
    at86rfmega_dev->event_callback(at86rfmega_dev, NETDEV_EVENT_ISR);

    atmega_exit_isr();
}

#endif /* IS_USED(MODULE_AT86RF2XX_PERIPH) */
