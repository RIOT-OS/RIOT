/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

/* static port mapping */
static NRF_SPI_Type *const spi[] = {
#if SPI_0_EN
    SPI_0_DEV,
#endif
#if SPI_1_EN
    SPI_1_DEV
#endif
};

/**
 * @brief   array holding one pre-initialized mutex for each SPI device
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
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    spi_poweron(dev);

    /* disable the device -> nRF51822 reference 3.0 26.1.1 and 27.1*/
    spi[dev]->ENABLE = 0;

    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            /* disable TWI Interface */
            NRF_TWI0->ENABLE = 0;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            /* disable SPI Slave */
            NRF_SPIS1->ENABLE = 0;
            /* disable TWI Interface */
            NRF_TWI1->ENABLE = 0;
            break;
#endif
        default:
            return -1;
    }

    /* configure direction of used pins */
    spi_conf_pins(dev);
    /* configure SPI mode */
    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi[dev]->CONFIG = (SPI_CONFIG_CPOL_ActiveHigh << 2) | (SPI_CONFIG_CPHA_Leading << 1);
            break;
        case SPI_CONF_SECOND_RISING:
            spi[dev]->CONFIG = (SPI_CONFIG_CPOL_ActiveHigh << 2) | (SPI_CONFIG_CPHA_Trailing << 1);
            break;
        case SPI_CONF_FIRST_FALLING:
            spi[dev]->CONFIG = (SPI_CONFIG_CPOL_ActiveLow << 2) | (SPI_CONFIG_CPHA_Leading << 1);
            break;
        case SPI_CONF_SECOND_FALLING:
            spi[dev]->CONFIG = (SPI_CONFIG_CPOL_ActiveLow << 2) | (SPI_CONFIG_CPHA_Trailing << 1);
            break;
    }

    /* select bus speed */
    switch (speed) {
        case SPI_SPEED_100KHZ:          /* 125 KHz for this device */
            spi[dev]->FREQUENCY = SPI_FREQUENCY_FREQUENCY_K125;
            break;
        case SPI_SPEED_400KHZ:          /* 500 KHz for this device */
            spi[dev]->FREQUENCY = SPI_FREQUENCY_FREQUENCY_K500;
            break;
        case SPI_SPEED_1MHZ:            /* 1 MHz for this device */
            spi[dev]->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M1;
            break;
        case SPI_SPEED_5MHZ:            /* 4 MHz for this device */
            spi[dev]->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M4;
            break;
        case SPI_SPEED_10MHZ:           /* 8 MHz for this device */
            spi[dev]->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M8;
            break;
    }

    /* finally enable the device */
    spi[dev]->ENABLE = 1;
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    /* This API is incompatible with nRF51 SPIS */
    return -1;
}

int spi_conf_pins(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            /* set pin direction */
            NRF_GPIO->DIRSET = (1 << SPI_0_PIN_MOSI) | (1 << SPI_0_PIN_SCK);
            NRF_GPIO->DIRCLR = (1 << SPI_0_PIN_MISO);
            /* select pins to be used by SPI */
            spi[dev]->PSELMOSI = SPI_0_PIN_MOSI;
            spi[dev]->PSELMISO = SPI_0_PIN_MISO;
            spi[dev]->PSELSCK = SPI_0_PIN_SCK;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            /* set pin direction */
            NRF_GPIO->DIRSET = (1 << SPI_1_PIN_MOSI) | (1 << SPI_1_PIN_SCK);
            NRF_GPIO->DIRCLR = (1 << SPI_1_PIN_MISO);
            /* select pins to be used by SPI */
            spi[dev]->PSELMOSI = SPI_1_PIN_MOSI;
            spi[dev]->PSELMISO = SPI_1_PIN_MISO;
            spi[dev]->PSELSCK = SPI_1_PIN_SCK;
            break;
#endif
    }
    return 0;
}

int spi_acquire(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    return spi_transfer_bytes(dev, &out, in, 1);
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    for (int i = 0; i < length; i++) {
        char tmp = (out) ? out[i] : 0;
        spi[dev]->EVENTS_READY = 0;
        spi[dev]->TXD = (uint8_t)tmp;
        while (spi[dev]->EVENTS_READY != 1);
        tmp = (char)spi[dev]->RXD;
        if (in) {
            in[i] = tmp;
        }
    }

    return length;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_byte(dev, out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_bytes(dev, out, in, length);
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* spi slave is not implemented */
}

void spi_poweron(spi_t dev)
{
    if (dev < SPI_NUMOF) {
        spi[dev]->POWER = 1;
    }
}

void spi_poweroff(spi_t dev)
{
    if (dev < SPI_NUMOF) {
        spi[dev]->POWER = 0;
    }
}

#endif /* SPI_NUMOF */
