#ifndef AT86RF231_ARCH_H_
#define AT86RF231_ARCH_H_

#include <stdint.h>

void at86rf231_gpio_spi_interrupts_init(void);

void at86rf231_reset(void);
uint8_t at86rf231_get_status(void);

void at86rf231_switch_to_rx(void);

void at86rf231_spi_select(void);
void at86rf231_spi_unselect(void);

uint8_t at86rf231_spi_transfer_byte(uint8_t byte);

void at86rf231_init_interrupts(void);
void at86rf231_enable_interrupts(void);
void at86rf231_disable_interrupts(void);
#endif
