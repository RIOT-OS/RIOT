
#include <stdio.h>
#include <string.h>

#include "bme680_hal.h"

#ifdef MODULE_PERIPH_I2C
#include "periph/i2c.h"
#endif
#ifdef MODULE_PERIPH_SPI
#include "periph/spi.h"
#endif

#include "xtimer.h"

#ifndef BME680_SPI_SPEED
#define BME680_SPI_SPEED    (SPI_CLK_1MHZ)
#endif /* BME680_SPI_SPEED */

#ifndef BME6800_SPI_MODE
#define BME680_SPI_MODE     (SPI_MODE_0)
#endif /* BME680_SPI_MODE */

#ifndef BME680_NSS_PIN
#define BME680_NSS_PIN  GPIO_PIN(PA, 5)
#endif  /* BME680_NSS_PIN */

#ifndef BME680_I2C_ADDR
#define BME680_I2C_ADDR (0x76)
#endif /* BME_I2C_ADDR */

void ms_sleep(uint32_t msleep)
{
    xtimer_usleep(msleep * US_PER_MS);
}

#ifdef MODULE_PERIPH_I2C
int8_t i2c_read_hal(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    uint8_t ret;
    i2c_acquire(I2C_DEV(dev_id));
    ret = i2c_read_regs(I2C_DEV(dev_id), BME680_I2C_ADDR, reg_addr, data, len, 0);
    i2c_release(I2C_DEV(dev_id));
    return ret;
}

int8_t i2c_write_hal(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    uint8_t ret;
    i2c_acquire(I2C_DEV(dev_id));
    ret = i2c_write_regs(I2C_DEV(dev_id), BME680_I2C_ADDR, reg_addr,data,len,0);
    i2c_release(I2C_DEV(dev_id));
    return ret;
}
#endif /* MODULE_PERIPH_I2C */

#ifdef MODULE_PERIPH_SPI
int8_t spi_read_hal(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    unsigned int cpsr;
    cpsr = irq_disable();

    gpio_clear(BME680_NSS_PIN);
    spi_acquire(SPI_DEV(dev_id), SPI_CS_UNDEF, BME680_SPI_MODE, BME680_SPI_SPEED);
    spi_transfer_regs(SPI_DEV(dev_id), SPI_CS_UNDEF, reg_addr, NULL, data, len);
    gpio_set(BME680_NSS_PIN);

    irq_restore(cpsr);
    spi_release(SPI_DEV(dev_id));
    return 0;
}

int8_t spi_write_hal(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    unsigned int cpsr;
    cpsr = irq_disable();

    gpio_clear(BME680_NSS_PIN);
    spi_acquire(SPI_DEV(dev_id), SPI_CS_UNDEF, BME680_SPI_MODE, BME680_SPI_SPEED);
    spi_transfer_regs(SPI_DEV(dev_id), SPI_CS_UNDEF, reg_addr, data, NULL, len);
    gpio_set(BME680_NSS_PIN);

    irq_restore(cpsr);
    spi_release(SPI_DEV(dev_id));
    return 0;
}
#endif /* MODULE_PERIPH_SPI */
