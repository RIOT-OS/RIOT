/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften
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
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 * @author      <neo@nenaco.de>
 *
 * @}
 */

#include "mrf24j40.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "periph/spi.h"
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

static const uint8_t dbm_to_tx_pow[] = { 0xf8, 0xf0, 0xe8, 0xe0, 0xd8, 0xd0, 0xc8, 0xc0,
                                         0xb8, 0xb0, 0xa8, 0xa8, 0x98, 0x90, 0x88, 0x80,
                                         0x78, 0x70, 0x68, 0x60, 0x58, 0x50, 0x48, 0x40,
                                         0x38, 0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00 };

uint16_t mrf24j40_get_addr_short(mrf24j40_t *dev)
{
    return (dev->netdev.short_addr[0] << 8) | dev->netdev.short_addr[1];
}

void mrf24j40_set_addr_short(mrf24j40_t *dev, uint16_t addr)
{
    dev->netdev.short_addr[0] = (uint8_t)(addr);
    dev->netdev.short_addr[1] = (uint8_t)(addr >> 8);
#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->netdev.short_addr[0] &= 0x7F;
#endif
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SADRL, dev->netdev.short_addr[1]);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SADRH, dev->netdev.short_addr[0]);
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
    uint8_t tmp;

    if ((channel < MRF24J40_MIN_CHANNEL) ||
        (channel > MRF24J40_MAX_CHANNEL) ||
        (dev->netdev.chan == channel)) {
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
        default:    channel_value = 0x3;
    }

    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON0, channel_value);
    /*
     * Note:Perform an RF State Machine Reset (see Section 3.1 “Reset”)
     * after a channel frequency change. Then, delay at least 192 us after
     * the RF State Machine Reset, to allow the RF circuitry to calibrate.
     */
    tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RFCTL);
    tmp |= 0b00000100;
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, tmp);
    tmp &= 0b11111011;
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, tmp);
    xtimer_usleep(200);             /* Delay at least 192us */
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

    txpower = mrf24j40_reg_read_long(dev, MRF24J40_REG_RFCON3);
    return tx_pow_to_dbm[txpower];
}

void mrf24j40_set_txpower(mrf24j40_t *dev, int16_t txpower)
{
    uint8_t txpower_reg_value;

    txpower_reg_value = dbm_to_tx_pow[txpower];

    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON3, txpower_reg_value);

}


uint8_t mrf24j40_get_max_retries(mrf24j40_t *dev)
{
    return (mrf24j40_reg_read_short(dev, MRF24J40_REG_TXSTAT) >> 6);
}

void mrf24j40_set_max_retries(mrf24j40_t *dev, uint8_t max)
{
    max = (max > 3) ? 3 : max;
    uint8_t tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXSTAT);
    tmp &= ~(MRF24J40_TXSTAT__MAX_FRAME_RETRIES);
    tmp |= (max << 6);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXSTAT, tmp);
}

uint8_t mrf24j40_get_csma_max_retries(mrf24j40_t *dev)
{
    uint8_t tmp;

    tmp  = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp  &= 0x7;
    return tmp;
}

void mrf24j40_set_csma_max_retries(mrf24j40_t *dev, int8_t retries)
{
    uint8_t tmp;

    retries = (retries > 5) ? 5 : retries; /* valid values: 0-5 */
    tmp  = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    if (retries < 0) {
        tmp |= MRF24J40_TXMCR__NOCSMA;
        mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp); /* max < 0 => disable CSMA (set to 7) */
    }
    DEBUG("[mrf24j40] opt: Set CSMA retries to %u\n", retries);

    tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp &= ~(MRF24J40_TXMCR__MAX_BACKOFF);
    tmp |= (retries);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
}

void mrf24j40_set_csma_backoff_exp(mrf24j40_t *dev, uint8_t min, uint8_t max)
{
    uint8_t tmp;

    max = (max > 3) ? 3 : max;
    min = (min > max) ? max : min;
    DEBUG("[mrf24j40] opt: Set min BE=%u, max BE=%u\n", min, max);

    tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp &= ~MRF24J40_TXMCR__CSMA_MACMAXBE;
    tmp |= (min << 3);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
}

int8_t mrf24j40_get_cca_threshold(mrf24j40_t *dev)
{
    /* take a look onto datasheet table 3-8 */
    int8_t tmp;
    int8_t dBm_value[] = {  95, 89, 88, 88, 87, 87, 87, 87, \
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


    tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_CCAEDTH);   /* Energy detection threshold */

    return(dBm_value[tmp]);                                     /* in dBm */
}

void mrf24j40_set_cca_threshold(mrf24j40_t *dev, int8_t value)
{
    /* take a look onto datasheet table 3-8 */
    uint8_t RSSI_value[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
                             0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
                             0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
                             0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfd, 0xfa, 0xf5, \
                             0xef, 0xe9, 0xe4, 0xe1, 0xdd, 0xd8, 0xd4, 0xcf, 0xcb, 0xc6, \
                             0xc1, 0xbc, 0xb7, 0xb0, 0xaa, 0xa5, 0x9f, 0x99, 0x94, 0x8f, \
                             0x8a, 0x85, 0x81, 0x7d, 0x79, 0x75, 0x6f, 0x6b, 0x64, 0x5f, \
                             0x59, 0x53, 0x4e, 0x49, 0x44, 0x3f, 0x3a, 0x35, 0x30, 0x2b, \
                             0x25, 0x20, 0x1b, 0x17, 0x12, 0x0d, 0x09, 0x05, 0x02, 0x01, \
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (value < 0) {
        value = -value;
    }

    printf("\nmrf24j40_set_cca_threshold : register-value to set = %x\n", (int)RSSI_value[value]);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_CCAEDTH, RSSI_value[value]);
}

void mrf24j40_set_option(mrf24j40_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);
    printf("\nset option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->netdev.flags |= option;
        /* trigger option specific actions */
        switch (option) {
            case MRF24J40_OPT_CSMA:
                DEBUG("[mrf24j40] opt: enabling CSMA mode (4 retries, macMinBE: 3 max BE: 5)\n");

                /* Initialize CSMA seed with hardware address */
                mrf24j40_set_csma_max_retries(dev, 4);
                mrf24j40_set_csma_backoff_exp(dev, 3, 5);
                break;
            case MRF24J40_OPT_PROMISCUOUS:
                DEBUG("[mrf24j40] opt: enabling PROMISCUOUS mode\n");
                /* disable auto ACKs in promiscuous mode */
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp |= MRF24J40_RXMCR__NOACKRSP;
                /* enable promiscuous mode */
                tmp |= MRF24J40_RXMCR__PROMI;
                tmp &= ~MRF24J40_RXMCR__ERRPKT;
                mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                break;
            case MRF24J40_OPT_AUTOACK:
                DEBUG("[mrf24j40] opt: enabling auto ACKs\n");
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp &= ~(MRF24J40_RXMCR__NOACKRSP);
                mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->netdev.flags &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case MRF24J40_OPT_CSMA:
                DEBUG("[mrf24j40] opt: disabling CSMA mode\n");

                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
                tmp |= MRF24J40_TXMCR_MASK__NOCSMA;
                mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);

                break;
            case MRF24J40_OPT_PROMISCUOUS:
                DEBUG("[mrf24j40] opt: disabling PROMISCUOUS mode\n");
                /* disable promiscuous mode */
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp &= ~MRF24J40_RXMCR__PROMI;
                tmp &= ~MRF24J40_RXMCR__ERRPKT;
                /* re-enable AUTOACK only if the option is set */
                if (dev->netdev.flags & MRF24J40_OPT_AUTOACK) {
                    tmp &= ~(MRF24J40_RXMCR__NOACKRSP);
                    mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp);
                }
                break;
            case MRF24J40_OPT_AUTOACK:
                DEBUG("[mrf24j40] opt: disabling auto ACKs\n");
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
                tmp |= MRF24J40_RXMCR__NOACKRSP;
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

    old_state = mrf24j40_get_status(dev);

    if (state == old_state) {
        printf("mrf24j40_set_state : Keine State-Aenderung -> return");
        return;
    }


    if (state == MRF24J40_PSEUDO_STATE_SLEEP) {     /* Datasheet chapter 3.15.2 IMMEDIATE SLEEP AND WAKE-UP MODE */

        /* set sleep/wake-pin on uController to low */
        gpio_clear(dev->params.sleep_pin);

        /* set sleep/wake pin polarity high on radio chip to high and enable it */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RXFLUSH, 0x60);

        mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_MASK__IMMWAKE);

        /* First force a Power Management Reset */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_SOFTRST, MRF24J40_SOFTRST_MASK__RSTPWR);

        /* Discard all IRQ flags, disable IRQ */
        mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT);         /* clearing IRQ flags */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, 0xff);   /* disable IRQs */

        /* Go to SLEEP mode */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_SLPACK, MRF24J40_SLPACK_MASK__SLPACK);
        dev->state = state;
    }
    else if (state == MRF24J40_PSEUDO_STATE_BUSY_TX_ARET_OFF) {
    }
    else {
//        _set_state(dev, state);
    }
}


void mrf24j40_reset_state_machine(mrf24j40_t *dev)
{
    uint8_t rfstate;

    mrf24j40_assert_awake(dev);

    /* Wait for any state transitions to complete before forcing TRX_OFF */
//    do {
//        old_state = mrf24j40_get_status(dev);
//    } while (old_state == MRF24J40_STATE_IN_PROGRESS);
//    at86rf2xx_force_trx_off(dev);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x04);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x00);
    xtimer_usleep(200);             /* Delay at least 192us */
}

void mrf24j40_software_reset(mrf24j40_t *dev)
{
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SOFTRST, 0x7);
    while (mrf24j40_reg_read_short(dev, MRF24J40_REG_SOFTRST) & 0x7) {
        /* wait for soft reset to finish */
    }
}


