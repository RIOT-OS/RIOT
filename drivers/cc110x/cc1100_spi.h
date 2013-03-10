/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
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
 * @note		$Id: cc1100_spi.h 1775 2010-01-26 09:37:03Z hillebra $
 */

#ifndef CC1100_SPI_H_
#define CC1100_SPI_H_

int cc110x_get_gdo0(void);
int cc110x_get_gdo1(void);
int cc110x_get_gdo2(void);

void cc110x_spi_init(void);
void cc110x_spi_cs(void);
void cc110x_spi_select(void);
void cc110x_spi_unselect(void);

uint8_t cc1100_spi_writeburst_reg(uint8_t addr, char *buffer, uint8_t count);
void cc1100_spi_readburst_reg(uint8_t addr, char *buffer, uint8_t count);
void cc1100_spi_write_reg(uint8_t addr, uint8_t value);
uint8_t cc1100_spi_read_reg(uint8_t addr);
uint8_t cc1100_spi_read_status(uint8_t addr);
uint8_t cc1100_spi_strobe(uint8_t c);

/** @} */
#endif /* CC1100_SPI_H_ */
