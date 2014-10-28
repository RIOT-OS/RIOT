/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     dev_cc110x_legacy_csma
 * @{
 */

/**
 * @file
 * @internal
 * @brief       TI Chipcon CC1100 SPI driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 1775 $
 *
 * @note        $Id: cc1100_spi.c 1775 2010-01-26 09:37:03Z hillebra $
 */

#include <stdio.h>

#include "irq.h"
#include "arch_cc1100.h"
#include "cc1100.h"
#include "cc1100_spi.h"
#include "cc1100-internal.h"

/*---------------------------------------------------------------------------*/
//                      CC1100 SPI access
/*---------------------------------------------------------------------------*/

uint8_t
cc1100_spi_writeburst_reg(uint8_t addr, char *src, uint8_t count)
{
    int i = 0;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_WRITE_BURST);

    while (i < count) {
        cc110x_txrx(src[i]);
        i++;
    }

    cc110x_spi_unselect();
    restoreIRQ(cpsr);
    return count;
}

void
cc1100_spi_readburst_reg(uint8_t addr, char *buffer, uint8_t count)
{
    int i = 0;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_READ_BURST);

    while (i < count) {
        buffer[i] = cc110x_txrx(CC1100_NOBYTE);
        i++;
    }

    cc110x_spi_unselect();
    restoreIRQ(cpsr);
}

void
cc1100_spi_write_reg(uint8_t addr, uint8_t value)
{
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr);
    cc110x_txrx(value);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
}

uint8_t cc1100_spi_read_reg(uint8_t addr)
{
    uint8_t result;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_READ_SINGLE);
    result = cc110x_txrx(CC1100_NOBYTE);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

uint8_t cc1100_spi_read_status(uint8_t addr)
{
    uint8_t result;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_READ_BURST);
    result = cc110x_txrx(CC1100_NOBYTE);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

uint8_t cc1100_spi_strobe(uint8_t c)
{
    uint8_t result;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    result = cc110x_txrx(c);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

/** @} */
