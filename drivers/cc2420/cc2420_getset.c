/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the CC2420 drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cc2420.h"
#include "cc2420_internal.h"
#include "cc2420_registers.h"
#include "periph/spi.h"

#include "panic.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static const uint8_t DBM_TO_LEVEL[32] = {
    31, 27, 25, 23, 21, 19, 17, 15, 13, 12, 11, 10, 9, 8, 7, 7,
    6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

static const int LEVEL_TO_DBM[32] = {
    -25, -25, -25, -24, -21, -19, -17, -15, -13, -12, -11, -10, -9, -8, -7, -7,
    -6, -6, -5, -5, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0, 0, 0
};

uint16_t cc2420_get_addr_short(cc2420_t *dev)
{
    return (dev->addr_short[0] << 8) | dev->addr_short[1];
}

void cc2420_set_addr_short(cc2420_t *dev, uint16_t addr)
{
    dev->addr_short[0] = addr >> 8;
    dev->addr_short[1] = addr & 0xff;
    cc2420_ram_write(dev, CC2420_RAM_SHORTADR, dev->addr_short, 2);
    cc2420_set_addr_long(dev, 0xffff & addr);
}

uint64_t cc2420_get_addr_long(cc2420_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);
    for (int i = 0; i < 8; i++) {
        ap[i] = dev->addr_long[7 - i];
    }
    return addr;
}

void cc2420_set_addr_long(cc2420_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++) {
        dev->addr_long[i] = (addr >> ((7 - i) * 8));
    }
    cc2420_ram_write(dev, CC2420_RAM_IEEEADR, dev->addr_long, 8);
}

uint8_t cc2420_get_chan(cc2420_t *dev)
{
    return dev->chan;
}

void cc2420_set_chan(cc2420_t *dev, uint8_t channel)
{
    if (channel < CC2420_MIN_CHANNEL
        || channel > CC2420_MAX_CHANNEL) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n", chan);
        return;
    }
    dev->chan = channel;
    /*
     * calculation from http://www.ti.com/lit/ds/symlink/cc2420.pdf p.50
     */
    uint16_t freq= cc2420_reg_read(dev, CC2420_REG_FSCTRL);
    freq &= ~CC2420_FREQ_MASK;
    freq |= (357 + (5 * (channel - 11)));
    cc2420_reg_write(dev, CC2420_REG_FSCTRL, freq);
}

uint16_t cc2420_get_pan(cc2420_t *dev)
{
    uint8_t buf[2];
    cc2420_ram_read(dev, CC2420_RAM_PANID, buf, 2);
    return (buf[0]<<8 | buf[1]);
}

void cc2420_set_pan(cc2420_t *dev, uint16_t pan)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(pan & 0xFF);
    buf[1] = (uint8_t)(pan >> 8);
    dev->pan = pan;
    DEBUG("pan0: %u, pan1: %u\n", (uint8_t)pan, pan >> 8);
    cc2420_ram_write(dev, CC2420_RAM_PANID, buf, 2);
}

int16_t cc2420_get_txpower(cc2420_t *dev)
{
    uint16_t txctrl_reg = cc2420_reg_read(dev, CC2420_REG_TXCTRL);
    /* PA_LEVEL is in the 5 least-significant bits of TXCTRL register */
    uint8_t level = txctrl_reg & 0x001F;
    /* determine output power in dBm from TX level */
    int pow = LEVEL_TO_DBM[level];
    return pow;
}

void cc2420_set_txpower(cc2420_t *dev, int16_t txpower)
{
    uint16_t txctrl_reg = cc2420_reg_read(dev, CC2420_REG_TXCTRL);
    /* reset PA_LEVEL bits */
    txctrl_reg &= 0xFFE0;
    /* constrain power in transceiver's acceptable set of values */
    if (txpower > 0) {
        txpower = 0;
    }
    if (txpower < -25) {
        txpower = -25;
    }
    /* determine TX level from power in dBm */
    uint8_t level = DBM_TO_LEVEL[-txpower];
    /* put wanted value in PA_LEVEL bits, and write back register */
    txctrl_reg |= level;
    cc2420_reg_write(dev, CC2420_REG_TXCTRL, txctrl_reg);
}

void cc2420_set_option(cc2420_t *dev, uint16_t option, bool state)
{
    uint16_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->options |= option;
        /* trigger option specific actions */
        switch (option) {
            case CC2420_OPT_CSMA:
                DEBUG("[cc2420] opt: enabling CSMA mode (NOT IMPLEMENTED)\n");
                /* TODO: en/disable csma */
                break;
            case CC2420_OPT_PROMISCUOUS:
                DEBUG("[cc2420] opt: enabling PROMISCUOUS mode\n");
                tmp = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                /* disable auto ACKs in promiscuous mode */
                tmp &= ~CC2420_AUTOACK;
                /* enable promiscuous mode */
                tmp &= ~CC2420_ADR_DECODE;
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, tmp);
                dev->options &= ~(CC2420_OPT_AUTOACK);
                break;
            case CC2420_OPT_AUTOACK:
                DEBUG("[cc2420] opt: enabling auto ACKs\n");
                tmp = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                tmp |= CC2420_AUTOACK;
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, tmp);
                break;
            case CC2420_OPT_TELL_RX_START:
                DEBUG("[cc2420] opt: enabling SFD IRQ (NOT IMPLEMENTED)\n");
                /* TODO */
                break;
            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->options &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case CC2420_OPT_CSMA:
                DEBUG("[cc2420] opt: disabling CSMA mode (NOT IMPLEMENTED)\n");
                /* TODO: en/disable csma */
                break;
            case CC2420_OPT_PROMISCUOUS:
                DEBUG("[cc2420] opt: disabling PROMISCUOUS mode\n");
                /* disable promiscuous mode */
                tmp = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                tmp |= CC2420_ADR_DECODE;
                /* re-enable AUTOACK only if the option was set */
                if (dev->options & CC2420_OPT_AUTOACK) {
                    tmp |= CC2420_AUTOACK;
                }
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, tmp);
                break;
            case CC2420_OPT_AUTOACK:
                DEBUG("[cc2420] opt: disabling auto ACKs\n");
                tmp = cc2420_reg_read(dev, CC2420_REG_MDMCTRL0);
                tmp &= ~CC2420_AUTOACK;
                cc2420_reg_write(dev, CC2420_REG_MDMCTRL0, tmp);
                break;
            case CC2420_OPT_TELL_RX_START:
                DEBUG("[cc2420] opt: disabling SFD IRQ (NOT IMPLEMENTED)\n");
                /* TODO */
                break;
            default:
                /* do nothing */
                break;
        }
    }
}

inline uint8_t cc2420_get_state(cc2420_t *dev)
{
    cc2420_reg_read(dev, CC2420_REG_FSMSTATE);
    return dev->state;
}

void cc2420_set_state(cc2420_t *dev, uint8_t state)
{
    switch (state) {
        case CC2420_STATE_IDLE:
            cc2420_strobe(dev, CC2420_STROBE_RFOFF);
            break;
        case CC2420_STATE_P_DOWN:
            cc2420_strobe(dev, CC2420_STROBE_XOSCOFF);
            break;
        case CC2420_STATE_RX_ON:
            cc2420_strobe(dev, CC2420_STROBE_RXON);
            break;
        case CC2420_STATE_TX_BUSY:
            cc2420_strobe(dev, CC2420_STROBE_TXON);
            break;
        case CC2420_STATE_TX_PREP:
        case CC2420_STATE_RX_BUSY:
        case CC2420_STATE_IN_PROGRESS:
            /* driver internal state, don't directly match FSM's states */
            break;
        default:
            core_panic(0x2420, "Unknown state passed to state machine!");
            break;
    }
    dev->state = state;
}
