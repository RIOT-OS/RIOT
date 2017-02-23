/*
 * copyright (c) 2016 unwired devices [info@unwds.com]
 * copyright (c) 2017 fundación inria chile
 *
 * this file is subject to the terms and conditions of the gnu lesser
 * general public license v2.1. see the file license in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx1276
 * @{
 * @file
 * @brief       implementation of internal functions for sx1276
 *
 * @author      eugene p. [ep@unwds.com]
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @}
 */
#include "sx1276.h"
#include "sx1276_registers.h"
#include "sx1276_internal.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "sx1276_params.h"

uint32_t sx1276_random(sx1276_t *dev)
{
    uint8_t i;
    uint32_t rnd = 0;

    sx1276_set_modem(dev, SX1276_MODEM_LORA); /* Set LoRa modem ON */

    /* Disable LoRa modem interrupts */
    sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGSMASK, SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT |
                     SX1276_RF_LORA_IRQFLAGS_RXDONE |
                     SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                     SX1276_RF_LORA_IRQFLAGS_VALIDHEADER |
                     SX1276_RF_LORA_IRQFLAGS_TXDONE |
                     SX1276_RF_LORA_IRQFLAGS_CADDONE |
                     SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                     SX1276_RF_LORA_IRQFLAGS_CADDETECTED);

    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_STANDBY);
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, SX1276_RNG_REG_MODEM_CONFIG1);
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, SX1276_RNG_REG_MODEM_CONFIG2);

    /* Set radio in continuous reception */
    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_RECEIVER);

    for (i = 0; i < 32; i++) {
        xtimer_usleep(1000); /* wait for the chaos */

        /* Non-filtered RSSI value reading. Only takes the LSB value */
        rnd |= ((uint32_t) sx1276_reg_read(dev, SX1276_REG_LR_RSSIWIDEBAND) & 0x01) << i;
    }

    sx1276_set_sleep(dev);

    return rnd;
}

void sx1276_reg_write(sx1276_t *dev, uint8_t addr, uint8_t data)
{
    sx1276_reg_write_burst(dev, addr, &data, 1);
}

uint8_t sx1276_reg_read(sx1276_t *dev, uint8_t addr)
{
    uint8_t data;

    sx1276_reg_read_burst(dev, addr, &data, 1);

    return data;
}

void sx1276_reg_write_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                            uint8_t size)
{
    unsigned int cpsr;

    spi_acquire(dev->params.spi, SPI_CS_UNDEF, SX1276_PARAM_SPI_MODE, SX1276_PARAM_SPI_SPEED);
    cpsr = irq_disable();

    gpio_clear(dev->params.nss_pin);
    spi_transfer_regs(dev->params.spi, SPI_CS_UNDEF, addr | 0x80, (char *) buffer, NULL, size);
    gpio_set(dev->params.nss_pin);

    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

void sx1276_reg_read_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                           uint8_t size)
{
    unsigned int cpsr;

    cpsr = irq_disable();

    spi_acquire(dev->params.spi, SPI_CS_UNDEF, SX1276_PARAM_SPI_MODE, SX1276_PARAM_SPI_SPEED);

    gpio_clear(dev->params.nss_pin);
    spi_transfer_regs(dev->params.spi, SPI_CS_UNDEF, addr & 0x7F, NULL, (char *) buffer, size);
    gpio_set(dev->params.nss_pin);

    spi_release(dev->params.spi);

    irq_restore(cpsr);
}

void sx1276_write_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size)
{
    sx1276_reg_write_burst(dev, 0, buffer, size);
}

void sx1276_read_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size)
{
    sx1276_reg_read_burst(dev, 0, buffer, size);
}
