#ifndef AT86RF231_ARCH_H_
#define AT86RF231_ARCH_H_

#include <stdint.h>

#include "vtimer.h"

void at86rf231_gpio_spi_interrupts_init(void);

void at86rf231_reset(void);
uint8_t at86rf231_get_status(void);

void at86rf231_spi_select(void);
void at86rf231_spi_unselect(void);

void at86rf231_spi_transfer(const uint8_t *data_out, uint8_t *data_in, radio_packet_length_t length);
uint8_t at86rf231_spi_transfer_byte(uint8_t byte);

void at86rf231_init_interrupts(void);
void at86rf231_enable_interrupts(void);
void at86rf231_disable_interrupts(void);
#endif
