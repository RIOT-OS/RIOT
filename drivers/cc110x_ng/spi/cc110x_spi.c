/******************************************************************************
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @ingroup		dev_cc110x
 * @{
 */

/**
 * @file
 * @internal
 * @brief		TI Chipcon CC1100 SPI driver
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author		Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 1775 $
 *
 * @note		$Id: cc110x_spi.c 1775 2010-01-26 09:37:03Z hillebra $
 */

#include <stdio.h>

#include "cc110x_ng.h"
#include "cc110x-arch.h"
#include "cc110x-internal.h"
#include "cc110x_spi.h"
#include "cc110x-reg.h"

#include "irq.h"

/*---------------------------------------------------------------------------*/
//					    CC1100 SPI access
/*---------------------------------------------------------------------------*/

#define NOBYTE 0xFF

uint8_t cc110x_writeburst_reg(uint8_t addr, char *src, uint8_t count)
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

void cc110x_readburst_reg(uint8_t addr, char *buffer, uint8_t count)
{
    int i = 0;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_READ_BURST);

    while (i < count) {
        buffer[i] = cc110x_txrx(NOBYTE);
        i++;
    }

    cc110x_spi_unselect();
    restoreIRQ(cpsr);
}

void cc110x_read_fifo(char *buffer, uint8_t count)
{
    cc110x_readburst_reg(CC1100_RXFIFO, buffer, count);
}

void cc110x_write_reg(uint8_t addr, uint8_t value)
{
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr);
    cc110x_txrx(value);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
}

uint8_t cc110x_read_reg(uint8_t addr)
{
    uint8_t result;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_READ_SINGLE);
    result = cc110x_txrx(NOBYTE);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

uint8_t cc110x_read_status(uint8_t addr)
{
    uint8_t result;
    unsigned int cpsr = disableIRQ();
    cc110x_spi_select();
    cc110x_txrx(addr | CC1100_READ_BURST);
    result = cc110x_txrx(NOBYTE);
    cc110x_spi_unselect();
    restoreIRQ(cpsr);
    return result;
}

uint8_t cc110x_strobe(uint8_t c)
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
