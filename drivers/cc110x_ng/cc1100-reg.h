#ifndef CC110X_REG_H
#define CC110X_REG_H
#include <stdint.h>

uint8_t cc1100_writeburst_reg(uint8_t addr, char *buffer, uint8_t count);
void cc1100_readburst_reg(uint8_t addr, char *buffer, uint8_t count);
void cc1100_write_reg(uint8_t addr, uint8_t value);
uint8_t cc1100_read_reg(uint8_t addr);
uint8_t cc1100_read_status(uint8_t addr);
uint8_t cc1100_strobe(uint8_t c);

#endif
