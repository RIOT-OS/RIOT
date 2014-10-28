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
 * @note        $Id: cc1100_spi.h 1775 2010-01-26 09:37:03Z hillebra $
 */

#ifndef CC1100_SPI_H_
#define CC1100_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC1100_SPI_H_ */
