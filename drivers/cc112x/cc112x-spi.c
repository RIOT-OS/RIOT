/**
 * @ingroup     drivers_cc112x
 * @{
 *
 * @file
 * @brief       TI Chipcon CC120x spi driver
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
 * @}
 */

#include <stdio.h>

#include "cc112x.h"
#include "cc112x-spi.h"
#include "cc112x-internal.h"
#include "cc112x-defines.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "xtimer.h"
#include "irq.h"

/**********************************************************************
 *                      CC112x spi access
 **********************************************************************/

void cc112x_cs(cc112x_t *dev)
{
    volatile int retry_count = 0;
    /* Switch MISO/GDO1 to GPIO input mode */
#ifndef GPIO_READS_SPI_PINS
    gpio_init(dev->params.gpio1, GPIO_DIR_IN, GPIO_NOPULL);
#endif
    /* CS to low */
    gpio_clear(dev->params.cs);
    /* Wait for SO to go low (voltage regulator
     * has stabilized and the crystal is running) */
    while (gpio_read(dev->params.gpio1)) {
        /* Wait ~500us and try again */
        xtimer_usleep(CS_SO_WAIT_TIME);

        if (gpio_read(dev->params.gpio1)) {
            retry_count++;

            if (retry_count > CC112X_GDO1_LOW_RETRY) {
                puts("[CC112X spi] fatal error\n");
                break;
            }

            gpio_set(dev->params.cs);
            gpio_clear(dev->params.cs);
        }
    }
    /* Switch MISO/GDO1 to spi mode */
#ifndef GPIO_READS_SPI_PINS
    spi_conf_pins(dev->params.spi);
#endif
}

void cc112x_writeburst_reg(cc112x_t *dev, uint16_t addr, const char *src, uint8_t count)
{
	unsigned int cpsr;
	spi_acquire( dev->params.spi );
	cpsr = disableIRQ();
	cc112x_cs( dev );
    /* In writing and reading should be recognition between normal and
     * extended address list.
     */
	if(addr & 0xff00){
		/* extended address */
		spi_transfer_byte( dev->params.spi, 0x2F | CC112X_WRITE | CC112X_BURST, NULL);
		/* sending address */
		spi_transfer_byte( dev->params.spi, (uint8_t) addr&0xff, NULL );
	} else {
		spi_transfer_byte( dev->params.spi, (uint8_t)(addr&0xff) | CC112X_WRITE | CC112X_BURST, NULL );
	}
	/* sending following data */
	for( uint32_t a = 0 ; a < count ; ){
		spi_transfer_byte( dev->params.spi, src[a], NULL );
		++a;
	}

    gpio_set(dev->params.cs);
    restoreIRQ(cpsr);
    spi_release(dev->params.spi);
}

void cc112x_readburst_reg(cc112x_t *dev, uint16_t addr, char *buffer, uint8_t count)
{
    unsigned int cpsr;
    spi_acquire(dev->params.spi);
    cc112x_cs(dev);
    cpsr = disableIRQ();
    if(addr & 0xff00){
    	/* extended address */
    	spi_transfer_byte( dev->params.spi, 0x2f | CC112X_READ | CC112X_BURST, NULL );
    	spi_transfer_byte(dev->params.spi, (uint8_t) addr&0xff, NULL );
    } else {
    	spi_transfer_byte(dev->params.spi, (uint8_t)(addr&0xff) | CC112X_READ | CC112X_BURST, NULL );
    }
    for( uint32_t a = 0 ; a < count ;){
        spi_transfer_byte(dev->params.spi, CC112X_NOBYTE, &buffer[a]);
        ++a;
    }
    gpio_set(dev->params.cs);
    restoreIRQ(cpsr);
    spi_release(dev->params.spi);
}

int cnt = 0;

void cc112x_write_reg(cc112x_t *dev, uint16_t addr, uint8_t value)
{
	uint32_t cpsr;
    spi_acquire(dev->params.spi);
    cc112x_cs(dev);
    cpsr = disableIRQ();
    if(addr & 0xff00){
		/* extended address */
		spi_transfer_byte( dev->params.spi, 0x2f | CC112X_WRITE | CC112X_SINGLE, NULL );
		spi_transfer_byte( dev->params.spi, (uint8_t) addr&0xff, NULL );
	} else {
		spi_transfer_byte( dev->params.spi, (uint8_t)(addr&0xff) | CC112X_WRITE | CC112X_SINGLE, NULL );
	}
    spi_transfer_byte( dev->params.spi, value, NULL );

    gpio_set(dev->params.cs);
    restoreIRQ(cpsr);
    spi_release(dev->params.spi);
    return;
}

uint8_t cc112x_read_reg(cc112x_t *dev, uint16_t addr)
{
    char result;
    uint32_t cpsr;
    spi_acquire(dev->params.spi);
    cc112x_cs(dev);
    cpsr = disableIRQ();
    if(addr & 0xff00){
		/* extended address */
		spi_transfer_byte( dev->params.spi, 0x2f | CC112X_READ | CC112X_SINGLE, NULL );
		spi_transfer_byte( dev->params.spi, (uint8_t) addr&0xff, NULL );
	} else {
		spi_transfer_byte( dev->params.spi, (uint8_t)(addr&0xff) | CC112X_READ | CC112X_SINGLE, NULL );
	}
    spi_transfer_byte( dev->params.spi, CC112X_NOBYTE, &result );
    gpio_set(dev->params.cs);
    restoreIRQ(cpsr);
    spi_release(dev->params.spi);
    return (uint8_t) result;
}

uint8_t cc112x_read_status(cc112x_t *dev, uint16_t addr)
{
    char result;
    unsigned int cpsr;
    spi_acquire(dev->params.spi);
    cc112x_cs(dev);
    cpsr = disableIRQ();
    spi_transfer_reg(dev->params.spi, addr | CC112X_READ | CC112X_BURST, CC112X_NOBYTE, &result);
    gpio_set(dev->params.cs);
    restoreIRQ(cpsr);
    spi_release(dev->params.spi);
    return (uint8_t) result;
}

uint8_t cc112x_strobe(cc112x_t *dev, uint8_t c)
{
#ifdef CC112X_DONT_RESET
    if (c == CC112X_SRES) {
        return 0;
    }
#endif
    char result;
    unsigned int cpsr;
    spi_acquire(dev->params.spi);
    cc112x_cs(dev);
    cpsr = disableIRQ();
    spi_transfer_byte(dev->params.spi, c, &result);
    gpio_set(dev->params.cs);
    restoreIRQ(cpsr);
    spi_release(dev->params.spi);
    return (uint8_t) result;
}
