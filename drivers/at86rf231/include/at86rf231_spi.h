#ifndef AT86RF231_SPI_H_
#define AT86RF231_SPI_H_

#include <stdint.h>

#include "board.h"

uint8_t at86rf231_reg_read(uint8_t addr);
void at86rf231_reg_write(uint8_t addr, uint8_t value);

void at86rf231_read_fifo(uint8_t *data, radio_packet_length_t length);
void at86rf231_write_fifo(const uint8_t *data, radio_packet_length_t length);
#endif
