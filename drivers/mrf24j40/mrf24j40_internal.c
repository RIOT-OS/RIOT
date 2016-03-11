/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */
#include "periph/spi.h"
#include "periph/gpio.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "xtimer.h"

void mrf24j40_reg_write_short(const mrf24j40_t *dev, const uint8_t addr, const uint8_t value)
{
	spi_acquire(dev->spi);
	gpio_clear(dev->cs_pin);
	spi_transfer_reg(dev->spi, MRF24J40_SHORT_ADDR_TRANS | (addr<<MRF24J40_ADDR_OFFSET) | MRF24J40_ACCESS_WRITE ,value, 0);
	gpio_set(dev->cs_pin);
	spi_release(dev->spi);
}

uint8_t mrf24j40_reg_read_short(const mrf24j40_t *dev, const uint8_t addr)
{
	char value;
	spi_acquire(dev->spi);
	gpio_clear(dev->cs_pin);
	spi_transfer_reg(dev->spi, MRF24J40_SHORT_ADDR_TRANS | (addr<<MRF24J40_ADDR_OFFSET) | MRF24J40_ACCESS_READ , 0, &value);
	gpio_set(dev->cs_pin);
	spi_release(dev->spi);
	
	return (uint8_t)value;
}

/**
 *bei spi müsste ne uint16_t addr eingeführt werden sonst müll
 */
void mrf24j40_reg_write_long(const mrf24j40_t *dev, const uint16_t addr, const uint8_t value)
{
	uint8_t reg1,reg2;
	reg1 = MRF24J40_LONG_ADDR_TRANS | (addr>>3);
	reg2 = (addr<<5) | MRF24J40_ACCESS_WRITE;
	spi_acquire(dev->spi);
	gpio_clear(dev->cs_pin);
	spi_transfer_byte(dev->spi, reg1, 0);
	spi_transfer_byte(dev->spi, reg2, 0);
	spi_transfer_byte(dev->spi, value, 0);
	gpio_set(dev->cs_pin);
	spi_release(dev->spi);
}

uint8_t mrf24j40_reg_read_long(const mrf24j40_t *dev, const uint16_t addr)
{   
	uint8_t reg1,reg2;
	reg1 = MRF24J40_LONG_ADDR_TRANS | (addr>>3);
	reg2 = (addr<<5) | MRF24J40_ACCESS_READ;
	char value;
	spi_acquire(dev->spi);
	gpio_clear(dev->cs_pin);
	spi_transfer_byte(dev->spi, reg1, 0);
	spi_transfer_byte(dev->spi, reg2, &value);
	gpio_set(dev->cs_pin);
	spi_release(dev->spi);
	
	return (uint8_t)value;
}

uint8_t mrf24j40_get_status(const mrf24j40_t *dev)
{
    /* if sleeping immediately return state */
    if(dev->state == MRF24J40_STATE_SLEEP)
        return dev->state;

    return mrf24j40_reg_read(dev, MRF24J40_REG_RFSTATE)
                & MRF24J40_RFSTATE_MASK_RFSTATE;
}
