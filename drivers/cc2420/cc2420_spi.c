/**
  * cc2420_spi.c - Implementation of SPI cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
  */

#include "cc2420_spi.h"
#include "cc2420_arch.h"

#include "cc2420_settings.h"
#include "irq.h"

/* reg */
void cc2420_write_reg(uint8_t addr, uint16_t value) {
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    cc2420_txrx(addr | CC2420_WRITE_ACCESS);
    cc2420_txrx((uint8_t) (value >> 8));
    cc2420_txrx((uint8_t) (value & 0xFF));
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
}

uint16_t cc2420_read_reg(uint8_t addr) {
    uint16_t result;
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    cc2420_txrx(addr | CC2420_READ_ACCESS);
    result = cc2420_txrx(NOBYTE);
    result <<= 8;
    result |= cc2420_txrx(NOBYTE);
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

uint8_t cc2420_strobe(uint8_t c) {
    uint8_t result;
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    result = cc2420_txrx(c);
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

/* ram */
uint16_t cc2420_read_ram(uint16_t addr, uint8_t* buffer, uint16_t len) {
    uint16_t i;
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    cc2420_txrx(CC2420_RAM_ACCESS | (addr & 0x7F));
    cc2420_txrx(((addr >> 1) & 0xC0) | CC2420_RAM_READ_ACCESS);
    for (i = 0; i < len; i++) {
        buffer[i] = cc2420_txrx(NOBYTE);
    }
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
    return i;
}

uint16_t cc2420_write_ram(uint16_t addr, uint8_t* buffer, uint16_t len) {
    uint16_t i;
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    cc2420_txrx(CC2420_RAM_ACCESS | (addr & 0x7F));
    cc2420_txrx(((addr >> 1) & 0xC0) | CC2420_RAM_WRITE_ACCESS);
    for (i = 0; i < len; i++) {
        cc2420_txrx(buffer[i]);
    }
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
    return i;
}

/* fifo */

uint16_t cc2420_write_fifo(uint8_t* data, uint16_t data_length) {
    uint16_t i;
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    cc2420_txrx(CC2420_REG_TXFIFO | CC2420_WRITE_ACCESS);
    for (i = 0; i < data_length; i++) {
        cc2420_txrx(data[i]);
    }
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
    return i;
}

uint16_t cc2420_read_fifo(uint8_t* data, uint16_t data_length) {
    uint16_t i;
    unsigned int cpsr = disableIRQ();
    cc2420_spi_select();
    cc2420_txrx(CC2420_REG_RXFIFO | CC2420_READ_ACCESS);
    for (i = 0; i < data_length; i++) {
        data[i] = cc2420_txrx(NOBYTE);
    }
    cc2420_spi_unselect();
    restoreIRQ(cpsr);
    return i;
}
