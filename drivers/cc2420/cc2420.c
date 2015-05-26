/**
  * cc2420.c - Implementation of cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  * Copyright (C) 2014 Kévin Roussel <Kevin.Roussel@inria.fr>
  *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
  */

#include "panic.h"
#include "cc2420.h"
#include "cc2420_spi.h"
#include "cc2420_settings.h"
#include "cc2420_arch.h"
#include "hwtimer.h"
#include "transceiver.h"

#include "debug.h"


/* number of attempts to turn on transceiver at startup, before panicking */
#define CC2420_STARTUP_ATTEMPTS  3

/* startup timeout (2 ms) in 16MHz-clock cycles */
#define CC2420_STARTUP_TIMEOUT  32000U


/* implementation of driver's functions */

void cc2420_initialize(void)
{
    cc2420_spi_init();
    hwtimer_wait(CC2420_WAIT_TIME);
    cc2420_reset();

    bool ok = false;
    for (int i = 0; i < CC2420_STARTUP_ATTEMPTS; i++) {
        ok = cc2420_on();
        if (ok) break;
    }
    if (!ok) {
        core_panic(0x2420, "Could not start CC2420 radio transceiver");
    }

    hwtimer_wait(CC2420_WAIT_TIME);

    uint16_t reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);
    reg |= CC2420_ADR_DECODE;                     /* enable auto adress decoding */
    reg |= CC2420_AUTOACK;                        /* enable auto ACK */
    reg |= CC2420_AUTOCRC;                        /* enable auto CRC */
    reg &= ~(CC2420_RES_FRM_MODE);                /* disable reserved frames */
    cc2420_write_reg(CC2420_REG_MDMCTRL0, reg);

    /* Change default values as recomended in the data sheet, */
    /* RX bandpass filter = 1.3uA. */
    reg = cc2420_read_reg(CC2420_REG_RXCTRL1);
    reg |= CC2420_RXBPF_LOCUR;
    cc2420_write_reg(CC2420_REG_RXCTRL1, reg);

    /* Set the FIFOP threshold to maximum. */
    cc2420_write_reg(CC2420_REG_IOCFG0, 127);

    /* Turn off "Security enable" (page 32). */
    reg = cc2420_read_reg(CC2420_REG_SECCTRL0);
    reg &= ~CC2420_RXFIFO_PROTECTION;
    cc2420_write_reg(CC2420_REG_SECCTRL0, reg);

    cc2420_set_tx_power(0);
    cc2420_set_channel(CC2420_DEFAULT_CHANNR);
    cc2420_set_pan(0x1111);
    DEBUG("CC2420 initialized and set to channel %i and pan 0x1111\n", CC2420_DEFAULT_CHANNR);
    cc2420_init_interrupts();
    cc2420_switch_to_rx();
}

void cc2420_init(kernel_pid_t tpid)
{
    transceiver_pid = tpid;
    cc2420_initialize();
}

bool cc2420_on(void)
{
    /* enable transceiver's crystal oscillator */
    cc2420_strobe(CC2420_STROBE_XOSCON);
    /* wait for the oscillator to be stable */
    unsigned int delay_on = 0;
    do {
        delay_on++;
        if (delay_on >= CC2420_STARTUP_TIMEOUT) {
            /* could not start up radio transceiver! */
            return false;
        }
    } while ((cc2420_status_byte() & CC2420_STATUS_XOSC16M_STABLE) == 0);
    hwtimer_wait(CC2420_WAIT_TIME);
    /* discard any potential garbage in TX buffer */
    cc2420_strobe(CC2420_STROBE_FLUSHTX);
    /* switch to RX mode */
    cc2420_switch_to_rx();
    /* OK, radio is on */
    return true;
}

void cc2420_off(void)
{
    /* halt any pending communication... */
    cc2420_strobe(CC2420_STROBE_RFOFF);
    /* ... and put the transceiver in power-down mode */
    cc2420_strobe(CC2420_STROBE_XOSCOFF);
}

bool cc2420_is_on(void)
{
    return ((cc2420_status_byte() & CC2420_STATUS_XOSC16M_STABLE) != 0);
}

void cc2420_switch_to_rx(void)
{
    cc2420_strobe(CC2420_STROBE_RFOFF);
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
    cc2420_strobe(CC2420_STROBE_RXON);
}

void cc2420_rxoverflow_irq(void)
{
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
    /* CC2420 datasheet says to do this twice... */
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
}

void cc2420_rx_irq(void)
{
    cc2420_rx_handler();
}

void cc2420_set_monitor(bool mode)
{
    uint16_t reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);
    if (mode) {
        reg &= ~CC2420_ADR_DECODE;
    } else {
        reg |= CC2420_ADR_DECODE;
    }
    cc2420_write_reg(CC2420_REG_MDMCTRL0, reg);
}

bool cc2420_get_monitor(void)
{
    uint16_t reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);
    return ((reg & CC2420_ADR_DECODE) == 0);
}

int cc2420_set_channel(unsigned int chan)
{
    if (chan < 11 || chan > 26) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n", chan);
        return -1;
    }
    /*
     * calculation from http://www.ti.com/lit/ds/symlink/cc2420.pdf p.50
     */
    uint16_t freq = 357 + (5 * (chan - 11));
    cc2420_write_reg(CC2420_REG_FSCTRL, freq);
    return ((unsigned int) chan);
}

unsigned int cc2420_get_channel(void)
{
    /* undo calculation from cc2420_set_channel() */
    return ((cc2420_read_reg(CC2420_REG_FSCTRL) - 357) / 5) + 11;
}

uint16_t cc2420_set_address(uint16_t addr)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(addr & 0xFF);
    buf[1] = (uint8_t)(addr >> 8);
    cc2420_write_ram(CC2420_RAM_SHORTADR, buf, 2);
    cc2420_set_address_long(0xFFFF & addr);
    return addr;
}

uint64_t cc2420_set_address_long(uint64_t addr)
{
    uint8_t buf[8];
    buf[0] = (uint8_t)(addr & 0xFF);
    buf[1] = (uint8_t)(addr >> 8);
    buf[2] = (uint8_t)(addr >> 16);
    buf[3] = (uint8_t)(addr >> 24);
    buf[4] = (uint8_t)(addr >> 32);
    buf[5] = (uint8_t)(addr >> 40);
    buf[6] = (uint8_t)(addr >> 48);
    buf[7] = (uint8_t)(addr >> 56);
    cc2420_write_ram(CC2420_RAM_IEEEADR, buf, 8);
    return addr;
}

uint16_t cc2420_get_address(void)
{
    uint16_t addr;
    cc2420_read_ram(CC2420_RAM_SHORTADR, (uint8_t *)&addr, sizeof(addr));
    return addr;
}

uint64_t cc2420_get_address_long(void)
{
    uint64_t addr;
    cc2420_read_ram(CC2420_RAM_IEEEADR, (uint8_t *)&addr, sizeof(addr));
    return addr;
}

uint16_t cc2420_set_pan(uint16_t pan)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(pan & 0xFF);
    buf[1] = (uint8_t)(pan >> 8);
    cc2420_write_ram(CC2420_RAM_PANID, buf, 2);
    return pan;
}

uint16_t cc2420_get_pan(void)
{
    uint16_t pan;
    cc2420_read_ram(CC2420_RAM_PANID, (uint8_t *)&pan, sizeof(pan));
    return pan;
}

static const uint8_t DBM_TO_LEVEL[32] = {
    31, 27, 25, 23, 21, 19, 17, 15, 13, 12, 11, 10, 9, 8, 7, 7,
    6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

int cc2420_set_tx_power(int pow)
{
    uint16_t txctrl_reg = cc2420_read_reg(CC2420_REG_TXCTRL);
    /* reset PA_LEVEL bits */
    txctrl_reg &= 0xFFE0;
    /* constrain power in transceiver's acceptable set of values */
    if (pow > 0) pow = 0;
    if (pow < -25) pow = -25;
    /* determine TX level from power in dBm */
    uint8_t level = DBM_TO_LEVEL[-pow];
    /* put wanted value in PA_LEVEL bits, and write back register */
    txctrl_reg |= level;
    cc2420_write_reg(CC2420_REG_TXCTRL, txctrl_reg);
    return pow;
}

static const int LEVEL_TO_DBM[32] = {
    -25, -25, -25, -24, -21, -19, -17, -15, -13, -12, -11, -10, -9, -8, -7, -7,
    -6, -6, -5, -5, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0, 0, 0
};

int cc2420_get_tx_power(void)
{
    uint16_t txctrl_reg = cc2420_read_reg(CC2420_REG_TXCTRL);
    /* PA_LEVEL is in the 5 least-significant bits of TXCTRL register */
    uint8_t level = txctrl_reg & 0x001F;
    /* determine output power in dBm from TX level */
    int pow = LEVEL_TO_DBM[level];
    return pow;
}

inline bool cc2420_channel_clear(void)
{
    return (cc2420_get_cca() != 0);
}

/* CC2420 low-level radio driver definition */
const ieee802154_radio_driver_t cc2420_radio_driver = {
    .init = cc2420_initialize,
    .on = cc2420_on,
    .off = cc2420_off,
    .is_on = cc2420_is_on,
    .load_tx = cc2420_load_tx_buf,
    .transmit = cc2420_transmit_tx_buf,
    .send = cc2420_do_send,
    .set_receive_callback = cc2420_set_recv_callback,
    .switch_to_rx = cc2420_switch_to_rx,
    .set_channel = do_set_channel,
    .get_channel = cc2420_get_channel,
    .set_address = do_set_address,
    .get_address = cc2420_get_address,
    .set_long_address = do_set_long_address,
    .get_long_address = cc2420_get_address_long,
    .set_pan_id = do_set_pan_id,
    .get_pan_id = cc2420_get_pan,
    .set_tx_power = do_set_tx_power,
    .get_tx_power = cc2420_get_tx_power,
    .channel_is_clear = cc2420_channel_clear,
    .set_promiscuous_mode = cc2420_set_monitor,
    .in_promiscuous_mode = cc2420_get_monitor
};
