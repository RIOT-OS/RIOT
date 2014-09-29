/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       Register access function definitions for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef AT86RF231_SPI_H_
#define AT86RF231_SPI_H_

#include <stdint.h>

#include "board.h"

uint8_t at86rf231_reg_read(uint8_t addr);
void at86rf231_reg_write(uint8_t addr, uint8_t value);

void at86rf231_read_fifo(uint8_t *data, radio_packet_length_t length);
void at86rf231_write_fifo(const uint8_t *data, radio_packet_length_t length);

uint8_t at86rf231_get_status(void);

#endif /* AT86RF231_SPI_H_ */
/** @} */
