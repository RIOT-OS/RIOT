/**
  * cc2420.c - Implementation of cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */

#include <cc2420.h>
#include <cc2420_spi.h>
#include <cc2420_settings.h>
#include <cc2420_arch.h>
#include <hwtimer.h>

//#define ENABLE_DEBUG
#include <debug.h>

static uint16_t radio_channel;
static uint16_t radio_address;
static uint64_t radio_address_long;
static uint16_t radio_pan;

/* Radio driver API */

int transceiver_pid;

void cc2420_init(int tpid)
{
    uint16_t reg;
    transceiver_pid = tpid;

    cc2420_spi_init();
    hwtimer_wait(CC2420_WAIT_TIME);
    cc2420_reset();

    cc2420_strobe(CC2420_STROBE_XOSCON);               //enable crystal

    while((cc2420_strobe(NOBYTE) & 0x40) == 0);        //wait for crystal to be stable
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
    DEBUG("CC2420 initialized and set to channel %i and pan %i\n", radio_channel, radio_pan);
    cc2420_init_interrupts();
    cc2420_switch_to_rx();

}

void cc2420_switch_to_rx(void) {
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
    if(mode) {
        reg &= ~CC2420_ADR_DECODE;
    } else {
        reg |= CC2420_ADR_DECODE;
    }
    cc2420_write_reg(CC2420_REG_MDMCTRL0, reg);
}

int16_t cc2420_set_channel(uint16_t chan)
{
    if(chan < 11 || chan > 26) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n",chan);
        return -1;
    }
    radio_channel = chan;
    chan = 357 + (5 * (radio_channel-11));              //calculation from p.50
    cc2420_write_reg(CC2420_REG_FSCTRL, chan);
    return radio_channel;
}

uint16_t cc2420_get_channel(void)
{
    return radio_channel;
}

uint16_t cc2420_set_address(uint16_t addr)
{
    uint8_t buf[2];
    radio_address = addr;
    buf[0] = (uint8_t)(addr & 0xFF);
    buf[1] = (uint8_t)(addr >> 8);
    cc2420_write_ram(CC2420_RAM_SHORTADR, buf, 2);
    cc2420_set_address_long(0x00FF & addr);
    return radio_address;
}

uint64_t cc2420_set_address_long(uint64_t addr)
{
    uint8_t buf[8];
    radio_address_long = addr;
    buf[0] = (uint8_t)(addr & 0xFF);
    buf[1] = (uint8_t)(addr >> 8);
    buf[2] = (uint8_t)(addr >> 16);
    buf[3] = (uint8_t)(addr >> 24);
    buf[4] = (uint8_t)(addr >> 32);
    buf[5] = (uint8_t)(addr >> 40);
    buf[6] = (uint8_t)(addr >> 48);
    buf[7] = (uint8_t)(addr >> 56);
    cc2420_write_ram(CC2420_RAM_IEEEADR, buf, 8);
    return radio_address_long;
}

uint16_t cc2420_get_address(void)
{
    return radio_address;
}

uint64_t cc2420_get_address_long(void)
{
    return radio_address_long;
}

uint16_t cc2420_set_pan(uint16_t pan)
{
    uint8_t buf[2];
    radio_pan = pan;
    buf[0] = (uint8_t)(pan & 0xFF);
    buf[1] = (uint8_t)(pan >> 8);
    cc2420_write_ram(CC2420_RAM_PANID, buf, 2);
    return radio_pan;
}

uint16_t cc2420_get_pan(void)
{
    return radio_pan;
}

void cc2420_swap_fcf_bytes(uint8_t *buf)
{
    uint8_t tmp;
    tmp = buf[0];
    buf[0] = buf[1];
    buf[1] = tmp;
}
