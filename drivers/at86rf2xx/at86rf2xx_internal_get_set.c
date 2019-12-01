/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
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
 * @brief       Getter and setter functions for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <string.h>
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"
#include "at86rf2xx_internal.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev)
{
    /* if sleeping immediately return state */
    if (dev->base.state == AT86RF2XX_STATE_SLEEP) {
        return dev->base.state;
    }

    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
            & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS);
}

void at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state, uint8_t cmd)
{
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, cmd);

    /* To prevent a possible race condition when changing to
     * RX_AACK_ON state the state doesn't get read back in that
     * case. See discussion
     * in https://github.com/RIOT-OS/RIOT/pull/5244
     */
    if (state != AT86RF2XX_STATE_RX_AACK_ON) {
        while (at86rf2xx_get_status(dev) != state) {}
    }
    /* Although RX_AACK_ON state doesn't get read back,
     * at least make sure if state transition is in progress or not
     */
    else {
        while (at86rf2xx_get_status(dev) == AT86RF2XX_STATE_IN_PROGRESS) {}
    }

    dev->base.state = state;
}

void at86rf2xx_get_addr_short(const at86rf2xx_t *dev, network_uint16_t *addr)
{
    memcpy(addr, dev->base.netdev.short_addr, sizeof(*addr));
}

void at86rf2xx_set_addr_short(at86rf2xx_t *dev, const network_uint16_t *addr)
{
    memcpy(dev->base.netdev.short_addr, addr, sizeof(*addr));
#if IS_USED(MODULE_SIXLOWPAN)
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->base.netdev.short_addr[0] &= 0x7F;
#endif
    /* device use lsb first, not network byte order */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_0,
                        dev->base.netdev.short_addr[1]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_1,
                        dev->base.netdev.short_addr[0]);
}

void at86rf2xx_get_addr_long(const at86rf2xx_t *dev, eui64_t *addr)
{
    memcpy(addr, dev->base.netdev.long_addr, sizeof(*addr));
}

void at86rf2xx_set_addr_long(at86rf2xx_t *dev, const eui64_t *addr)
{
    memcpy(dev->base.netdev.long_addr, addr, sizeof(*addr));
    for (int i = 0; i < 8; i++) {
        /* device use lsb first, not network byte order */
        at86rf2xx_reg_write(dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                            dev->base.netdev.long_addr[
                                IEEE802154_LONG_ADDRESS_LEN - 1 - i]);
    }
}

void at86rf2xx_set_channel(at86rf2xx_t *dev, uint8_t channel)
{
    uint8_t phy_cc_cca = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);

    /* Clear previous configuration for channel number */
    phy_cc_cca &= ~(AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);

    /* Update the channel register */
    phy_cc_cca |= (channel & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca);

    dev->base.netdev.chan = channel;
}

void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan)
{
    le_uint16_t le_pan = byteorder_btols(byteorder_htons(pan));

    DEBUG("pan0: %u, pan1: %u\n", le_pan.u8[0], le_pan.u8[1]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_0, le_pan.u8[0]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_1, le_pan.u8[1]);
}

uint8_t at86rf2xx_get_frame_max_retries(const at86rf2xx_t *dev)
{
    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0) >> 4);
}

void at86rf2xx_set_frame_max_retries(const at86rf2xx_t *dev, uint8_t max)
{
    uint8_t xah_ctrl_0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);

    xah_ctrl_0 &= ~(AT86RF2XX_XAH_CTRL_0_MASK__MAX_FRAME_RETRIES);
    /* valid values: 0-7 */
    xah_ctrl_0 |= ((max > AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__7)
                  ? AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__7
                  : max) << 4;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, xah_ctrl_0);
}

uint8_t at86rf2xx_get_csma_max_retries(const at86rf2xx_t *dev)
{
    uint8_t xah_ctrl_0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);

    xah_ctrl_0 &= AT86RF2XX_XAH_CTRL_0_MASK__MAX_CSMA_RETRIES;
    xah_ctrl_0 >>= 1;
    return xah_ctrl_0;
}

void at86rf2xx_set_slotted_operation(const at86rf2xx_t *dev, bool slotted)
{
    uint8_t xah_ctrl_0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);

    xah_ctrl_0 = (slotted)
                 ? (xah_ctrl_0 | AT86RF2XX_XAH_CTRL_0_MASK__SLOTTED_OPERATION)
                 : (xah_ctrl_0 &
                    ~(AT86RF2XX_XAH_CTRL_0_MASK__SLOTTED_OPERATION));

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, xah_ctrl_0);
}

void at86rf2xx_set_csma_max_retries(const at86rf2xx_t *dev, int8_t retries)
{
    /* valid values: 0-5 */
    retries = (retries > AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__5)
              ? AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__5
              : retries;
    /* max < 0 => disable CSMA (set to 7) */
    retries = (retries < 0)
              ? AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__NO_CSMA
              : retries;
    DEBUG("[at86rf2xx] opt: Set CSMA retries to %u\n", retries);

    uint8_t xah_ctrl_0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    xah_ctrl_0 &= ~(AT86RF2XX_XAH_CTRL_0_MASK__MAX_CSMA_RETRIES);
    xah_ctrl_0 |= (retries << 1);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, xah_ctrl_0);
}

uint8_t at86rf2xx_get_frame_retries(const at86rf2xx_t *dev)
{
    uint8_t xah_ctrl_2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_2);

    return (xah_ctrl_2 & AT86RF2XX_XAH_CTRL_2_MASK__ARET_FRAME_RETRIES) >> 4;
}

uint8_t at86rf2xx_get_csma_retries(const at86rf2xx_t *dev)
{
    uint8_t xah_ctrl_2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_2);

    return (xah_ctrl_2 & AT86RF2XX_XAH_CTRL_2_MASK__ARET_CSMA_RETRIES) >> 1;
}

void at86rf2xx_set_csma_backoff_exp(const at86rf2xx_t *dev,
                                    uint8_t min, uint8_t max)
{
    max = (max > AT86RF2XX_CSMA_BE_MAX_BE__MAX)
          ? AT86RF2XX_CSMA_BE_MAX_BE__MAX
          : max;
    min = (min > max) ? max : min;
    DEBUG("[at86rf2xx] opt: Set min BE=%u, max BE=%u\n", min, max);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
}

void at86rf2xx_set_csma_seed(const at86rf2xx_t *dev, const uint8_t entropy[2])
{
    if (entropy == NULL) {
        DEBUG("[at86rf2xx] opt: CSMA seed entropy is nullpointer\n");
        return;
    }
    DEBUG("[at86rf2xx] opt: Set CSMA seed to 0x%x 0x%x\n", entropy[0],
          entropy[1]);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_0, entropy[0]);

    uint8_t csma_seed_1 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    csma_seed_1 &= ~(AT86RF2XX_CSMA_SEED_1_MASK__CSMA_SEED_1);
    csma_seed_1 |= entropy[1] & AT86RF2XX_CSMA_SEED_1_MASK__CSMA_SEED_1;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, csma_seed_1);
}

int8_t at86rf2xx_get_cca_threshold(const at86rf2xx_t *dev)
{
    int8_t cca_thres = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CCA_THRES);

    return cca_thres & AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
}

void at86rf2xx_set_cca_threshold(const at86rf2xx_t *dev, uint8_t thresh)
{
    uint8_t cca_thres = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CCA_THRES);

    cca_thres &= ~AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    cca_thres = cca_thres | thresh;
    cca_thres |= AT86RF2XX_CCA_THRES_MASK__RSVD_HI_NIBBLE; /* What is this? */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CCA_THRES, cca_thres);
}

void at86rf2xx_set_clock_output(const at86rf2xx_t *dev, bool immediately,
                                char mode)
{
    uint8_t trx_ctrl_0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);

    trx_ctrl_0 = (immediately)
                 ? (trx_ctrl_0 & ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL))
                 : (trx_ctrl_0 | AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);

    /* set clock output mode over pin 17 CLKM */
    trx_ctrl_0 &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    trx_ctrl_0 |= (mode & AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, trx_ctrl_0);
}

void at86rf2xx_set_irq_mode(const at86rf2xx_t *dev, bool polling, bool polarity)
{
    uint8_t trx_ctrl_1 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);

    /* enable/disable polling */
    trx_ctrl_1 = (polling)
                 ? (trx_ctrl_1 | AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE)
                 : (trx_ctrl_1 & ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE));

    /* set polarity */
    trx_ctrl_1 = (polarity)
                 ? (trx_ctrl_1 | AT86RF2XX_TRX_CTRL_1_MASK__IRQ_POLARITY)
                 : (trx_ctrl_1 & ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_POLARITY));

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, trx_ctrl_1);
}

void at86rf2xx_set_frame_buffer_protection(const at86rf2xx_t *dev, bool protect)
{
    uint8_t trx_ctrl_2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);

    trx_ctrl_2 = (protect)
                 ? (trx_ctrl_2 | AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE)
                 : (trx_ctrl_2 & ~(AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE));

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, trx_ctrl_2);
}

void at86rf2xx_set_external_timestamping(const at86rf2xx_t *dev,
                                         bool timestamping)
{
    uint8_t trx_ctrl_1 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);

    /* enable/disable timestamping over pin 10 (DIG2) */
    trx_ctrl_1 = (timestamping)
                 ? (trx_ctrl_1 | AT86RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN)
                 : (trx_ctrl_1 & ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN));

    /* pin 10 (DIG2) also provides a differential control signal for
       antenna diversity, so it must be disabled it timestamping shall
       be used*/
    uint8_t ant_div = at86rf2xx_reg_read(dev, AT86RF2XX_REG__ANT_DIV);
    ant_div &= ~(AT86RF2XX_ANT_DIV_MASK__ANT_EXT_SW_EN);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__ANT_DIV, ant_div);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, trx_ctrl_1);
}

void at86rf2xx_set_frame_type_filter(const at86rf2xx_t *dev,
                                     bool reserved_ft, bool filter_ft)
{
    uint8_t xah_ctrl_1 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);

    xah_ctrl_1 = (reserved_ft)
                 ? (xah_ctrl_1 | AT86RF2XX_XAH_CTRL_1_MASK__AACK_UPLD_RES_FT)
                 : (xah_ctrl_1 &
                    ~(AT86RF2XX_XAH_CTRL_1_MASK__AACK_UPLD_RES_FT));

    xah_ctrl_1 = (filter_ft)
                 ? (xah_ctrl_1 | AT86RF2XX_XAH_CTRL_1_MASK__AACK_FLTR_RES_FT)
                 : (xah_ctrl_1 &
                    ~(AT86RF2XX_XAH_CTRL_1_MASK__AACK_FLTR_RES_FT));

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, xah_ctrl_1);
}

void at86rf2xx_get_random(const at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    for (size_t byteCount = 0; byteCount < len; ++byteCount) {
        uint8_t rnd = 0;
        for (uint8_t i = 0; i < 4; ++i) {
            /* bit 5 and 6 of the AT86RF2XX_REG__PHY_RSSI register contain the RND_VALUE */
            uint8_t phy_rssi = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_RSSI)
                               & AT86RF2XX_PHY_RSSI_MASK__RND_VALUE;
            /* shift the two random bits first to the right and then to the correct position of the return byte */
            phy_rssi = phy_rssi >> 5;
            phy_rssi = phy_rssi << 2 * i;
            rnd |= phy_rssi;
        }
        data[byteCount] = rnd;
    }
}

void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    dev->base.flags = (state) ? (dev->base.flags |  option)
                              : (dev->base.flags & ~option);
    /* trigger option specific actions */
    switch (option) {
        case AT86RF2XX_OPT_CSMA:
            if (state) {
                DEBUG("[at86rf2xx] opt: enabling CSMA mode" \
                      "(4 retries, min BE: 3 max BE: 5)\n");
                /* Initialize CSMA seed with hardware address */
                at86rf2xx_set_csma_seed(dev, dev->base.netdev.long_addr);
                at86rf2xx_set_csma_max_retries(dev, 4);
                at86rf2xx_set_csma_backoff_exp(dev, 3, 5);
            }
            else {
                DEBUG("[at86rf2xx] opt: disabling CSMA mode\n");
                /* setting retries to -1 means CSMA disabled */
                at86rf2xx_set_csma_max_retries(dev, -1);
            }
            break;
        case AT86RF2XX_OPT_PROMISCUOUS:
            DEBUG("[at86rf2xx] opt: %s PROMISCUOUS mode\n",
                  (state ? "enable" : "disable"));
            /* disable/enable auto ACKs in promiscuous mode */
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp |  AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                          : (tmp & ~AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            /* enable/disable promiscuous mode */
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
            tmp = (state) ? (tmp |  AT86RF2XX_XAH_CTRL_1_MASK__AACK_PROM_MODE)
                          : (tmp & ~AT86RF2XX_XAH_CTRL_1_MASK__AACK_PROM_MODE);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
            break;
        case AT86RF2XX_OPT_AUTOACK:
            DEBUG("[at86rf2xx] opt: %s auto ACKs\n",
                  (state ? "enable" : "disable"));
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp & ~AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                          : (tmp |  AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            break;
        case AT86RF2XX_OPT_TELL_RX_START:
            DEBUG("[at86rf2xx] opt: %s SFD IRQ\n",
                  (state ? "enable" : "disable"));
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
            tmp = (state) ? (tmp |  AT86RF2XX_IRQ_STATUS_MASK__RX_START)
                          : (tmp & ~AT86RF2XX_IRQ_STATUS_MASK__RX_START);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);
            break;
        case AT86RF2XX_OPT_ACK_PENDING:
            DEBUG("[at86rf2xx] opt: enabling pending ACKs\n");
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp |  AT86RF2XX_CSMA_SEED_1_MASK__AACK_SET_PD)
                          : (tmp & ~AT86RF2XX_CSMA_SEED_1_MASK__AACK_SET_PD);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            break;
        default:
            /* do nothing */
            break;
    }
}
