/**
  * cc2420.c - Implementation of cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */

#include "cc2420.h"
#include "cc2420_spi.h"
#include "cc2420_settings.h"
#include "cc2420_arch.h"
#include "hwtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Radio driver API */

void cc2420_init(int tpid)
{
    uint16_t reg;
    transceiver_pid = tpid;

    cc2420_spi_init();
    hwtimer_wait(CC2420_WAIT_TIME);
    cc2420_reset();

    cc2420_strobe(CC2420_STROBE_XOSCON);               //enable crystal

    while ((cc2420_strobe(NOBYTE) & 0x40) == 0);       //wait for crystal to be stable

    hwtimer_wait(CC2420_WAIT_TIME);

    reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);
    reg |= CC2420_ADR_DECODE;                          //enable adr decode
    reg |= CC2420_AUTOACK;                             //enable auto ack
    reg |= CC2420_AUTOCRC;                             //enable auto crc
    reg &= ~(CC2420_RES_FRM_MODE);                     //disable reserved frames
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

    /* set output power to 0dbm */
    cc2420_write_reg(CC2420_REG_TXCTRL, 0xA0FF);

    cc2420_set_channel(CC2420_DEFAULT_CHANNR);
    cc2420_set_pan(0x1111);
    DEBUG("CC2420 initialized and set to channel %i and pan 0x1111\n", CC2420_DEFAULT_CHANNR);
    cc2420_init_interrupts();
    cc2420_switch_to_rx();

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
    //Datasheets says do this twice...
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
}

void cc2420_rx_irq(void)
{
    cc2420_rx_handler();
}

void cc2420_set_monitor(uint8_t mode)
{
    uint16_t reg;
    reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);

    if (mode) {
        reg &= ~CC2420_ADR_DECODE;
    }
    else {
        reg |= CC2420_ADR_DECODE;
    }

    cc2420_write_reg(CC2420_REG_MDMCTRL0, reg);
}

int16_t cc2420_set_channel(uint16_t chan)
{
    uint16_t freq;

    if (chan < 11 || chan > 26) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n", chan);
        return -1;
    }

    /*
     * calculation from http://www.ti.com/lit/ds/symlink/cc2420.pdf p.50
     */
    freq = 357 + (5 * (chan - 11));
    cc2420_write_reg(CC2420_REG_FSCTRL, freq);
    return (int32_t)chan;
}

uint16_t cc2420_get_channel(void)
{
    /* undo calculation from cc2420_set_channel() */
    return ((cc2420_read_reg(CC2420_REG_FSCTRL) - 357) / 5) + 11;
}

radio_address_t cc2420_set_address(radio_address_t addr)
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

radio_address_t cc2420_get_address(void)
{
    radio_address_t addr;
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

inline bool channel_clear(void)
{
    return (cc2420_get_cca() != 0);
}
