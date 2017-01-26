/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2016 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the MRF24J40 drivers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Neo Nenaco <neo@nenaco.de>
 *
 * @}
 */

#include "mrf24j40.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Values of RFCON3 - Address: 0x203
 * 0b00000000 -> 0dB	-> 0
 * 0b00001000 -> -0.5dB -> 0
 * 0b00010000 -> -1.2dB -> -1
 * 0b00011000 -> -1.9dB -> -2
 * 0b00100000 -> -2.8dB -> -3
 * 0b00101000 -> -3.7dB -> -4
 * 0b00110000 -> -4.9dB -> -5
 * 0b00111000 -> -6.3dB -> -6
 * 0b01000000 -> -10dB  -> -10
 * 0b01001000 -> -10.5dB-> -10
 * 0b01010000 -> -11.2dB-> -11
 * 0b01011000 -> -11.9dB-> -12
 * 0b01100000 -> -12.8dB-> -13
 * 0b01101000 -> -13.7dB-> -14
 * 0b01110000 -> -14.9dB-> -15
 * 0b01111000 -> -16.3dB-> -16
 * 0b10000000 -> -20dB  -> -20
 * 0b10001000 -> -20.5dB-> -20
 * 0b10010000 -> -21.2dB-> -21
 * 0b10011000 -> -21.9dB-> -22
 * 0b10100000 -> -22.8dB-> -23
 * 0b10101000 -> -23.7dB-> -24
 * 0b10110000 -> -24.9dB-> -25
 * 0b10111000 -> -26.3dB-> -26
 * 0b11000000 -> -30dB  -> -30
 * 0b11001000 -> -30.5dB-> -30
 * 0b11010000 -> -31.2dB-> -31
 * 0b11011000 -> -31.9dB-> -32
 * 0b11100000 -> -32.8dB-> -33
 * 0b11101000 -> -33.7dB-> -34
 * 0b11110000 -> -34.9dB-> -35
 * 0b11111000 -> -36.3dB-> -36
 */

static const int16_t tx_pow_to_dbm[] = { 0, 0, -1, -2, -3, -4, -5, -6,
                                         -10, -10, -11, -12, -13, -14, -15, -16,
                                         -20, -20, -21, -22, -23, -24, -25, -26,
                                         -30, -30, -31, -32, -33, -34, -35, -36 };

static const uint8_t dbm_to_tx_pow[] = { 0x00, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x38, 0x38, 0x40,
                                         0x40, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x78, 0x78, 0x80,
                                         0x80, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8, 0xb8, 0xb8, 0xc0,
                                         0xc0, 0xd0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8 };

/* take a look onto datasheet table 3-8 */
static const int8_t dBm_value[] = {  95, 89, 88, 88, 87, 87, 87, 87, \
                                     86, 86, 86, 86, 85, 85, 85, 85, \
                                     84, 84, 84, 84, 84, 84, 83, 83, \
                                     83, 83, 82, 82, 82, 82, 81, 81, \
                                     81, 81, 81, 80, 80, 80, 80, 80, \
                                     80, 79, 79, 79, 79, 79, 78, 78, \
                                     78, 78, 78, 77, 77, 77, 77, 77, \
                                     76, 76, 76, 76, 76, 75, 75, 75, \
                                     75, 75, 75, 74, 74, 74, 74, 73, \
                                     73, 73, 73, 73, 72, 72, 72, 72, \
                                     72, 71, 71, 71, 71, 71, 70, 70, \
                                     70, 70, 70, 70, 70, 69, 69, 69, \
                                     69, 69, 68, 68, 68, 68, 68, 68, \
                                     68, 67, 67, 67, 67, 66, 66, 66, \
                                     66, 66, 66, 65, 65, 65, 65, 65, \
                                     64, 64, 64, 64, 63, 63, 63, 63, \
                                     62, 62, 62, 62, 61, 61, 61, 61, \
                                     60, 60, 60, 60, 60, 59, 59, 59, \
                                     59, 59, 58, 58, 58, 58, 58, 57, \
                                     57, 57, 57, 57, 57, 56, 56, 56, \
                                     56, 56, 56, 56, 55, 55, 55, 55, \
                                     54, 54, 54, 54, 54, 54, 53, 53, \
                                     53, 53, 53, 53, 53, 52, 52, 52, \
                                     52, 52, 52, 51, 51, 51, 51, 51, \
                                     50, 50, 50, 50, 50, 49, 49, 49, \
                                     49, 49, 48, 48, 48, 48, 47, 47, \
                                     47, 47, 47, 46, 46, 46, 46, 45, \
                                     45, 45, 45, 44, 44, 44, 44, 44, \
                                     43, 43, 43, 42, 42, 42, 42, 41, \
                                     41, 41, 41, 41, 41, 40, 40, 40, \
                                     40, 40, 39, 39, 39, 39, 39, 38, \
                                     38, 38, 38, 37, 37, 37, 36, 30 };

/* take a look onto datasheet table 3-8 */
static const uint8_t RSSI_value[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
                                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
                                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
                                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfd, 0xfa, 0xf5, \
                                      0xef, 0xe9, 0xe4, 0xe1, 0xdd, 0xd8, 0xd4, 0xcf, 0xcb, 0xc6, \
                                      0xc1, 0xbc, 0xb7, 0xb0, 0xaa, 0xa5, 0x9f, 0x99, 0x94, 0x8f, \
                                      0x8a, 0x85, 0x81, 0x7d, 0x79, 0x75, 0x6f, 0x6b, 0x64, 0x5f, \
                                      0x59, 0x53, 0x4e, 0x49, 0x44, 0x3f, 0x3a, 0x35, 0x30, 0x2b, \
                                      0x25, 0x20, 0x1b, 0x17, 0x12, 0x0d, 0x09, 0x05, 0x02, 0x01, \
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


uint16_t mrf24j40_get_addr_short(mrf24j40_t *dev)
{
    return (dev->netdev.short_addr[0] << 8) | dev->netdev.short_addr[1];
}

void mrf24j40_set_addr_short(mrf24j40_t *dev, uint16_t addr)
{
#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->netdev.short_addr[0] &= 0x7F;
#endif
    dev->netdev.short_addr[0] = (uint8_t)(addr);
    dev->netdev.short_addr[1] = (uint8_t)(addr >> 8);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SADRL,
                             dev->netdev.short_addr[1]);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SADRH,
                             dev->netdev.short_addr[0]);
}

uint64_t mrf24j40_get_addr_long(mrf24j40_t *dev)
{
    uint64_t addr;

    uint8_t *ap = (uint8_t *)(&addr);

    for (int i = 0; i < 8; i++) {
        ap[i] = dev->netdev.long_addr[i];
    }
    return addr;
}

void mrf24j40_set_addr_long(mrf24j40_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++) {
        dev->netdev.long_addr[i] = (uint8_t)(addr >> (i * 8));
        mrf24j40_reg_write_short(dev, (MRF24J40_REG_EADR0 + i),
                                 (addr >> ((7 - i) * 8)));
    }
}

uint8_t mrf24j40_get_chan(mrf24j40_t *dev)
{
    return dev->netdev.chan;
}


void mrf24j40_set_chan(mrf24j40_t *dev, uint8_t channel)
{
    uint8_t channel_value;

    if ((channel < IEEE802154_CHANNEL_MIN) ||
        (channel > IEEE802154_CHANNEL_MAX)) {
        return;
    }

    dev->netdev.chan = channel;

    /* Channel settings
     * 11 -> Value = 0x03
     * 12 -> Value = 0x13
     * 13 -> Value = 0x23
     * 14 -> Value = 0x33
     * 15 -> Value = 0x43
     * 16 -> Value = 0x53
     * 17 -> Value = 0x63
     * 18 -> Value = 0x73
     * 19 -> Value = 0x83
     * 20 -> Value = 0x93
     * 21 -> Value = 0xA3
     * 22 -> Value = 0xB3
     * 23 -> Value = 0xC3
     * 24 -> Value = 0xD3
     * 25 -> Value = 0xE3
     * 26 -> Value = 0xF3
     */
    /* not using an array here because it's not starting at zero */
    switch (channel) {
        case 11:    channel_value = 0x03;
            break;
        case 12:    channel_value = 0x13;
            break;
        case 13:    channel_value = 0x23;
            break;
        case 14:    channel_value = 0x33;
            break;
        case 15:    channel_value = 0x43;
            break;
        case 16:    channel_value = 0x53;
            break;
        case 17:    channel_value = 0x63;
            break;
        case 18:    channel_value = 0x73;
            break;
        case 19:    channel_value = 0x83;
            break;
        case 20:    channel_value = 0x93;
            break;
        case 21:    channel_value = 0xa3;
            break;
        case 22:    channel_value = 0xb3;
            break;
        case 23:    channel_value = 0xc3;
            break;
        case 24:    channel_value = 0xd3;
            break;
        case 25:    channel_value = 0xe3;
            break;
        case 26:    channel_value = 0xf3;
            break;
        default:
            return;
    }

    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON0, channel_value);
    /*
     * Note:Perform an RF State Machine Reset (see Section 3.1 “Reset”)
     * after a channel frequency change. Then, delay at least 192 us after
     * the RF State Machine Reset, to allow the RF circuitry to calibrate.
     */
    mrf24j40_reset_state_machine(dev);
}

uint16_t mrf24j40_get_pan(mrf24j40_t *dev)
{
    return dev->netdev.pan;
}

void mrf24j40_set_pan(mrf24j40_t *dev, uint16_t pan)
{
    le_uint16_t le_pan = byteorder_btols(byteorder_htons(pan));

    dev->netdev.pan = pan;
    DEBUG("pan0: %u, pan1: %u\n", le_pan.u8[0], le_pan.u8[1]);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PANIDL, le_pan.u8[0]);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PANIDH, le_pan.u8[1]);
}


int16_t mrf24j40_get_txpower(mrf24j40_t *dev)
{
    uint8_t txpower;

    txpower = (mrf24j40_reg_read_long(dev, MRF24J40_REG_RFCON3) >> 3) & 0x1F;
    return tx_pow_to_dbm[txpower];
}

void mrf24j40_set_txpower(mrf24j40_t *dev, int16_t txpower)
{
    uint8_t txpower_reg_value;

    /* positive values are better with a conversion array */
    txpower = (txpower < 0) ? -1 * txpower : txpower;
    txpower = (txpower > 36) ? 36 : txpower;
    txpower_reg_value = dbm_to_tx_pow[txpower];

    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON3, txpower_reg_value);

}

uint8_t mrf24j40_get_csma_max_retries(mrf24j40_t *dev)
{
    uint8_t tmp;

    tmp  = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp  &= MRF24J40_TXMCR_CSMA_BACKOFF_MASK;
    return tmp;
}

void mrf24j40_set_csma_max_retries(mrf24j40_t *dev, int8_t retries)
{
    uint8_t tmp;

    /* get current register settings */
    tmp  = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    /* clear csma bits */
    tmp  &= ~(MRF24J40_TXMCR_CSMA_BACKOFF_MASK);
    /* apply new settings */
    tmp  |= (retries & MRF24J40_TXMCR_CSMA_BACKOFF_MASK);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
}

int8_t mrf24j40_get_cca_threshold(mrf24j40_t *dev)
{
    int8_t tmp;

    tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_CCAEDTH);   /* Energy detection threshold */

    return(dBm_value[tmp]);                                     /* in dBm */
}

void mrf24j40_set_cca_threshold(mrf24j40_t *dev, int8_t value)
{
    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (value < 0) {
        value = -value;
    }

    mrf24j40_reg_write_short(dev, MRF24J40_REG_CCAEDTH, RSSI_value[value]);
}

void mrf24j40_set_option(mrf24j40_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->netdev.flags |= option;
        /* trigger option specific actions */
        switch (option) {
            case MRF24J40_OPT_CSMA:
                DEBUG("[mrf24j40] opt: enabling CSMA mode (4 retries, macMinBE: 3 max BE: 5)\n");
                /* Initialize CSMA seed with hardware address */
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
                tmp &= ~MRF24J40_TXMCR_NOCSMA;
                mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
                break;
            case MRF24J40_OPT_PROMISCUOUS:
                DEBUG("[mrf24j40] opt: enabling PROMISCUOUS mode\n");
                /* disable auto ACKs in promiscuous mode */
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp |= MRF24J40_RXMCR_NOACKRSP;
                /* enable promiscuous mode */
                tmp |= MRF24J40_RXMCR_PROMI;
                tmp &= ~MRF24J40_RXMCR_ERRPKT;
                mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                break;
            case NETDEV2_IEEE802154_ACK_REQ:
                DEBUG("[mrf24j40] opt: enabling auto ACKs\n");
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp &= ~MRF24J40_RXMCR_NOACKRSP;
                mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
    /* clear option field */
    else {
        dev->netdev.flags &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case MRF24J40_OPT_CSMA:
                DEBUG("[mrf24j40] opt: disabling CSMA mode\n");
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
                tmp |= MRF24J40_TXMCR_NOCSMA;
                /* MACMINBE<1:0>: The minimum value of the backoff exponent
                 * in the CSMA-CA algorithm. Note that if this value is set
                 * to ‘0’, collision avoidance is disabled. */
                mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
                break;
            case MRF24J40_OPT_PROMISCUOUS:
                DEBUG("[mrf24j40] opt: disabling PROMISCUOUS mode\n");
                /* disable promiscuous mode */
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp &= ~MRF24J40_RXMCR_PROMI;
                tmp &= ~MRF24J40_RXMCR_ERRPKT;
                /* re-enable AUTOACK only if the option is set */
                if (dev->netdev.flags & NETDEV2_IEEE802154_ACK_REQ) {
                    tmp &= ~(MRF24J40_RXMCR_NOACKRSP);
                    mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                }
                break;
            case NETDEV2_IEEE802154_ACK_REQ:
                DEBUG("[mrf24j40] opt: disabling auto ACKs\n");
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp |= MRF24J40_RXMCR_NOACKRSP;
                mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
}


void mrf24j40_set_state(mrf24j40_t *dev, uint8_t state)
{
    uint8_t old_state;

    old_state = dev->state;


    if (state == old_state) {
        return;
    }
    /* check if asked to wake up from sleep mode */
    if (old_state == MRF24J40_PSEUDO_STATE_SLEEP) {
        mrf24j40_assert_awake(dev);
    }
    if (state == MRF24J40_PSEUDO_STATE_SLEEP) {
        mrf24j40_sleep(dev);
    }
    if (state == MRF24J40_PSEUDO_STATE_IDLE) {
        dev->state = state;
    }
    dev->idle_state = state;
}

void mrf24j40_sleep(mrf24j40_t *dev)
{
    DEBUG("[mrf24j40] Putting into sleep mode\n");
    /* Datasheet chapter 3.15.2 IMMEDIATE SLEEP AND WAKE-UP MODE */
    /* First force a Power Management Reset */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SOFTRST, MRF24J40_SOFTRST_RSTPWR);
    /* Go to SLEEP mode */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SLPACK, MRF24J40_SLPACK_SLPACK);
    dev->state = MRF24J40_PSEUDO_STATE_SLEEP;
}

void mrf24j40_assert_sleep(mrf24j40_t *dev)
{
    if (dev->idle_state == MRF24J40_PSEUDO_STATE_SLEEP) {
        mrf24j40_sleep(dev);
    }
}

void mrf24j40_assert_awake(mrf24j40_t *dev)
{
    if (dev->state == MRF24J40_PSEUDO_STATE_SLEEP) {
        DEBUG("[mrf24j40] Waking up from sleep mode\n");
        /* Wake mrf up */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_IMMWAKE | MRF24J40_WAKECON_REGWAKE);
        /* undocumented delay, needed for stable wakeup */
        xtimer_usleep(MRF24J40_DELAY_SLEEP_TOGGLE);
        mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_IMMWAKE);
        /* reset state machine */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, MRF24J40_RFCTL_RFRST);
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x00);
        /* After wake-up, delay at least 2 ms to allow 20 MHz main
         * oscillator time to stabilize before transmitting or receiving.
         */
        xtimer_usleep(MRF24J40_WAKEUP_DELAY);
        /* reset interrupts */
        mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT);
        dev->state = MRF24J40_PSEUDO_STATE_IDLE;
    }
}

void mrf24j40_reset_state_machine(mrf24j40_t *dev)
{
    uint8_t rfstate;

    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, MRF24J40_RFCTL_RFRST);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x00);
    xtimer_usleep(MRF24J40_STATE_RESET_DELAY);             /* Delay at least 192us */
    do {
        rfstate = mrf24j40_reg_read_long(dev, MRF24J40_REG_RFSTATE);
    } while ((rfstate & MRF24J40_RFSTATE_MASK) != MRF24J40_RFSTATE_RX);
}

void mrf24j40_software_reset(mrf24j40_t *dev)
{
    uint8_t softrst;

    mrf24j40_reg_write_short(dev, MRF24J40_REG_SOFTRST, MRF24J40_SOFTRST_RSTPWR |
                                                        MRF24J40_SOFTRST_RSTBB  |
                                                        MRF24J40_SOFTRST_RSTMAC );
    do {
        softrst = mrf24j40_reg_read_short(dev, MRF24J40_REG_SOFTRST);
    } while (softrst != 0);        /* wait until soft-reset has finished */
}
