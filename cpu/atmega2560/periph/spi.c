/*
* Copyright (C) 2015 Daniel Amkaer Sorensen
*
* This file is subject to the terms and conditions of the GNU Lesser General
* Public License v2.1. See the file LICENSE in the top level directory for more
* details.
*/

/**
 * @ingroup     cpu_atmega2560
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Daniel Amkaer Sorensen <daniel.amkaer@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "sched.h"
#include "thread.h"

#include "periph/spi.h"
#include "periph_conf.h"

#if SPI_NUMOF

/**
 * @brief Each SPI device has to store one callback.
 */
typedef struct {
    char (*rx_cb)(char data);
} spi_state_t;

static spi_state_t config[SPI_NUMOF];

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
};

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    uint8_t _spcr = (1<<SPI_0_MSTR)|(1<<SPI_0_SPE);

    /* Assume F_CPU = 16.000.000 Hz */
    switch (speed) {
        case SPI_SPEED_100KHZ:
            /* Actual rate = 125.000 Hz */
            _spcr |= (1<<SPI_0_SPR1)|(1<<SPI_0_SPR0);
            break;
        case SPI_SPEED_400KHZ:
            /* Actual rate = 250.000 Hz */
            _spcr |= (1<<SPI_0_SPR1);
            break;
        case SPI_SPEED_1MHZ:
            /* Actual rate = 1.000.000 Hz */
            _spcr |= (1<<SPI_0_SPR0);
            break;
        case SPI_SPEED_10MHZ:
            /* Actual rate = 8.000.000 Hz */
            _spcr |= (1<<SPI_0_SPI2X);
            break;
        default:
            /* Default rate is 4 MHz (SPR0, SPR1, SPI2X = 0) */
            break;
    }

    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            /* Do nothing for CPOL=0, CPHA=0 */
            break;
        case SPI_CONF_SECOND_RISING:
            _spcr |= (1<<SPI_0_CPHA);
            break;
        case SPI_CONF_FIRST_FALLING:
            _spcr |= (1<<SPI_0_CPOL);
            break;
        case SPI_CONF_SECOND_FALLING:
            _spcr |= (1<<SPI_0_CPOL)|(1<<SPI_0_CPHA);
            break;
    }

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_DDR |= (1<<SPI_0_MOSI)|(1<<SPI_0_SCK);
            SPI_0_DDR &= ~((1<<SPI_0_MISO));
            SPI_0_CR = _spcr;
            return 0;
            break;
#endif
    }
    return -2;

}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data)) {
    uint8_t _spcr = (1<<SPI_0_SPE);

    switch(conf) {
        case SPI_CONF_FIRST_RISING:
            /* Do nothing for CPOL=0, CPHA=0 */
            break;
        case SPI_CONF_SECOND_RISING:
            _spcr |= (1<<SPI_0_CPHA);
            break;
        case SPI_CONF_FIRST_FALLING:
            _spcr |= (1<<SPI_0_CPOL);
            break;
        case SPI_CONF_SECOND_FALLING:
            _spcr |= (1<<SPI_0_CPOL)|(1<<SPI_0_CPHA);
            break;
    }

    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_DDR |= (1<<SPI_0_MISO);
            SPI_0_DDR &= ~((1<<SPI_0_MOSI)|(SPI_0_SCK));
            SPI_0_CR = _spcr;
            config[dev].rx_cb = cb;
            return 0;
            break;
#endif
    }

    return -1;
}

int spi_conf_pins(spi_t dev) {
    /* Done by init functions */
    return 0;
}

int spi_acquire(spi_t dev) {
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}
int spi_release(spi_t dev) {
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in) {
    switch (dev) {
#if SPI_0_EN
    case SPI_0:
        SPI_0_DR = out;
        while (!(SPI_0_SR & (1<<SPI_0_SPIF)));
        if (in != NULL) {
            *in = SPI_0_DR;
        } else {
            (void)SPI_0_DR;
        }
        return 1;
        break;
#endif
    }
    return -1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length) {
    unsigned int i = 0;
    for (i=0; i < length; i++) {
        int res = spi_transfer_byte(dev, ((out != NULL) ? out[i] : 0),
                ((in != NULL) ? &(in[i]) : NULL)
            );

        if (res < 0) return res;
    }
    return i;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in) {
    int res = spi_transfer_byte(dev,reg,NULL);
    if (res < 0) {
        return res;
    }
    res = spi_transfer_byte(dev,out,in);
    return res;
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in,
        unsigned int length) {

    int res = spi_transfer_byte(dev,reg,NULL);
    if (res < 0) return res;
    unsigned int i;
    for (i = 0; i < length; i++) {
        res = spi_transfer_byte(dev,((out != NULL) ? out[i] : 0),
                ((in != NULL) ? &(in[i]) : NULL)
            );
        if (res < 0) {
            return res;
        }
    }
    return i;
}

void spi_transmission_begin(spi_t dev, char reset_val) {
    (void)dev;
    (void)reset_val;
    /* Not supported */
}

void spi_poweron(spi_t dev) {
    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CR |= (1<<SPI_0_SPE);
            break;
#endif
    }
}
void spi_poweroff(spi_t dev) {
    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CR &= ~(1<<SPI_0_SPE);
            break;
#endif
    }
}
#endif
