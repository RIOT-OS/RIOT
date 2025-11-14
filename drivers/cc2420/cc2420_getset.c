/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the cc2420 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "cc2420.h"
#include "cc2420_internal.h"
#include "cc2420_registers.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Translation from dBm to PA level
 *
 * Entry 0 in the array corresponds to -25dBm (min), entry 25 to 0dBm (max), so
 * `PA_level = power_dbm_to_pa[DBM + 25]`. We use only the 3 MSB of the 5-bit
 * level, leading to 8 distinct power settings (the 8 settings listed in the
 * datasheet in section 28, page 51).
 */
static const uint8_t power_dbm_to_pa[26] = {
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  7,  7,  7,
     7,  7, 11, 11, 11, 15, 15, 19, 19, 23, 23, 27, 31
};

/**
 * @brief   Translate PA level to dBm
 *
 * As we use only the 3 MSB of the PA level value, we have 8 distinct settings.
 * We get the dBm value with `DBM = power_pa_to_dbm(PA >> 2).
 */
static const int8_t power_pa_to_dbm[8] = {
    -25, -15, -10, -7, -5, -3, -1, 0
};

void cc2420_get_addr_short(cc2420_t *dev, uint8_t *addr)
{
    uint8_t tmp[2];

    cc2420_ram_read(dev, CC2420_RAM_SHORTADR, tmp, 2);

    addr[0] = tmp[1];
    addr[1] = tmp[0];
}

void cc2420_set_addr_short(cc2420_t *dev, const uint8_t *addr)
{
    uint8_t tmp[2];
    tmp[0] = addr[1];
    tmp[1] = addr[0];

    memcpy(dev->netdev.short_addr, addr, 2);

#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->netdev.short_addr[0] &= 0x7F;
#endif

    cc2420_ram_write(dev, CC2420_RAM_SHORTADR, tmp, 2);
}

void cc2420_get_addr_long(cc2420_t *dev, uint8_t *addr)
{
    cc2420_ram_read(dev, CC2420_RAM_IEEEADR, addr, 8);

    uint8_t *ap = (uint8_t *)(&addr);
    for (int i = 0; i < 8; i++) {
        ap[i] = dev->netdev.long_addr[i];
    }
}

void cc2420_set_addr_long(cc2420_t *dev, const uint8_t *addr)
{
    int i, j;
    uint8_t tmp[8];

    for (i = 0, j = 7; i < 8; i++, j--) {
        dev->netdev.long_addr[i] = addr[i];
        tmp[j] = addr[i];
    }

    cc2420_ram_write(dev, CC2420_RAM_IEEEADR, tmp, 8);
}

uint16_t cc2420_get_pan(cc2420_t *dev)
{
    le_uint16_t pan;

    cc2420_ram_read(dev, CC2420_RAM_PANID, pan.u8, 2);
    return pan.u16;
}

void cc2420_set_pan(cc2420_t *dev, uint16_t pan)
{
    cc2420_ram_write(dev, CC2420_RAM_PANID, (uint8_t *)&pan, 2);
}

uint16_t cc2420_get_chan(cc2420_t *dev)
{
    uint16_t chan;
    uint16_t freq = cc2420_reg_read(dev, CC2420_REG_FSCTRL);

    chan = (((freq & CC2420_FSCTRL_FREQ_MASK) - 357) / 5) + 11;
    return chan;
}

int cc2420_set_chan(cc2420_t *dev, uint16_t chan)
{
    if ((chan < CC2420_CHAN_MIN) || (chan > CC2420_CHAN_MAX)) {
        DEBUG("cc2420: set channel: given channel invalid\n");
        return -ENOTSUP;
    }

    /* calculation from http://www.ti.com/lit/ds/symlink/cc2420.pdf p.50 */
    uint16_t freq = cc2420_reg_read(dev, CC2420_REG_FSCTRL);
    freq &= ~CC2420_FSCTRL_FREQ_MASK;
    freq |= (357 + (5 * (chan - 11)));
    cc2420_reg_write(dev, CC2420_REG_FSCTRL, freq);

    dev->netdev.chan = chan;

    return CC2420_RET_CHAN_OK;
}

int16_t cc2420_get_txpower(cc2420_t *dev)
{
    uint16_t txctrl = cc2420_reg_read(dev, CC2420_REG_TXCTRL);
    return (int16_t)power_pa_to_dbm[(txctrl & CC2420_TXCTRL_PA_MASK) >> 2];
}

void cc2420_set_txpower(cc2420_t *dev, int16_t txpower)
{
    if (txpower > CC2420_TXPOWER_MAX) {
        txpower = CC2420_TXPOWER_MAX;
    }
    else if (txpower < CC2420_TXPOWER_MIN) {
        txpower = CC2420_TXPOWER_MIN;
    }

    uint16_t txctrl = cc2420_reg_read(dev, CC2420_REG_TXCTRL);
    txctrl &= ~(CC2420_TXCTRL_PA_MASK);
    txctrl |= power_dbm_to_pa[txpower + 25];
    cc2420_reg_write(dev, CC2420_REG_TXCTRL, txctrl);
}

int cc2420_set_option(cc2420_t *dev, uint16_t option, bool state)
{
    uint16_t reg;

    /* set option field */
    if (state) {
        dev->options |= option;
        /* trigger option specific actions */
        switch (option) {
            case CC2420_OPT_AUTOACK:
                DEBUG("cc2420: set_opt: CC2420_OPT_AUTOACK\n");
                reg = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                reg |= CC2420_MDMCTRL0_AUTOACK;
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                break;

            case CC2420_OPT_CSMA:
                DEBUG("cc2420: set_opt: CC2420_OPT_CSMA\n");
                /* TODO: en/disable csma */
                break;

            case CC2420_OPT_PROMISCUOUS:
                DEBUG("cc2420: set_opt: CC2420_OPT_PROMISCUOUS\n");
                /* in promisc mode, AUTO ACKs are should be disabled */
                reg = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                reg &= ~(CC2420_MDMCTRL0_AUTOACK);
                reg &= ~(CC2420_MDMCTRL0_ADR_DECODE);
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                break;

            case CC2420_OPT_PRELOADING:
                DEBUG("cc2420: set_opt: CC2420_OPT_PRELOADING\n");
                break;

            default:
                return -ENOTSUP;
        }
    }
    else {
        dev->options &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case CC2420_OPT_AUTOACK:
                DEBUG("cc2420: clr_opt: CC2420_OPT_AUTOACK\n");
                reg = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                reg &= ~(CC2420_MDMCTRL0_AUTOACK);
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                break;

            case CC2420_OPT_CSMA:
                DEBUG("cc2420: clr_opt: CC2420_OPT_CSMA\n");
                /* TODO: en/disable csma */
                break;

            case CC2420_OPT_PROMISCUOUS:
                DEBUG("cc2420: clr_opt: CC2420_OPT_PROMISCUOUS\n");
                reg = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                reg |= CC2420_MDMCTRL0_ADR_DECODE;
                /* re-enable AUTOACK only if the option was set */
                if (dev->options & CC2420_OPT_AUTOACK) {
                    reg |= CC2420_MDMCTRL0_AUTOACK;
                }
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                break;

            case CC2420_OPT_PRELOADING:
                DEBUG("cc2420: clr_opt: CC2420_OPT_PRELOADING\n");
                break;

            default:
                return -ENOTSUP;
        }
    }
    return sizeof(netopt_enable_t);
}

int cc2420_set_state(cc2420_t *dev, netopt_state_t cmd)
{
    if ((cc2420_get_state(dev) == NETOPT_STATE_OFF) &&
        (cmd != NETOPT_STATE_OFF)) {
        cc2420_en_xosc(dev);
    }
    switch (cmd) {
        case NETOPT_STATE_OFF:
            cc2420_strobe(dev, CC2420_STROBE_XOSCOFF);
            while (cc2420_state(dev) != CC2420_STATE_PD) {}
            break;
        case NETOPT_STATE_SLEEP:
            cc2420_strobe(dev, CC2420_STROBE_RFOFF);
            while (cc2420_state(dev) != CC2420_STATE_IDLE) {}
            break;
        case NETOPT_STATE_IDLE:
            cc2420_strobe(dev, CC2420_STROBE_FLUSHRX);
            cc2420_strobe(dev, CC2420_STROBE_RXON);
            break;
        case NETOPT_STATE_TX:
            cc2420_tx_exec(dev);
            break;
        case NETOPT_STATE_RESET:
            cc2420_init(dev);
            break;
        case NETOPT_STATE_RX:
        default:
            DEBUG("cc2420: set_state: called with invalid target state\n");
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

netopt_state_t cc2420_get_state(cc2420_t *dev)
{
    uint8_t cur_state = cc2420_state(dev);

    if (cur_state == 0) {
        return NETOPT_STATE_OFF;
    }
    else if (cur_state == 1) {
        return NETOPT_STATE_SLEEP;
    }
    else if (((cur_state >= 32) && (cur_state <=39)) || (cur_state == 56)) {
        return NETOPT_STATE_TX;
    }
    else if ((cur_state >= 3) && (cur_state <= 6)) {
        return NETOPT_STATE_IDLE;
    }
    else {
        return NETOPT_STATE_RX;
    }
}
