/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x_ng
 * @{
 *
 * @file
 * @brief       TI Chipcon CC1100 SPI driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC1100_SPI_H_ */
