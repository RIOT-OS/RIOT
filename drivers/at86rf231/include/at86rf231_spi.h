#ifndef AT86RF231_SPI_H_
#define AT86RF231_SPI_H_

#include <stdint.h>

uint8_t at86rf231_reg_read(uint8_t addr);
void at86rf231_reg_write(uint8_t addr, uint8_t value);

#endif
