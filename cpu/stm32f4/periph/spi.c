/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   The STM32F4 only has one hardware chip select line
 */
#define HWCS_LINE           (0)

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
#if SPI_2_EN
    [SPI_2] = MUTEX_INIT
#endif
};

static inline SPI_TypeDef *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline void clk_en(spi_t bus)
{
    if (spi_config[bus].abpbus == BUS_APB1) {
        RCC->APB1ENR |= (spi_config[bus].rccmask);
    }
    else {
        RCC->APB2ENR |= (spi_config[bus].rccmask);
    }
}

static inline void clk_dis(spi_t bus)
{
    if (spi_config[bus].abpbus == BUS_APB1) {
        RCC->APB1ENR &= ~(spi_config[bus].rccmask);
    }
    else {
        RCC->APB2ENR &= ~(spi_config[bus].rccmask);
    }
}

int spi_init_pins(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return -1;
    }

    gpio_init(spi_config[bus].mosi_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(spi_config[bus].miso_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init(spi_config[bus].sclk_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init_af(spi_config[bus].mosi_pin, spi_config[bus].af);
    gpio_init_af(spi_config[bus].mosi_pin, spi_config[bus].af);
    gpio_init_af(spi_config[bus].sclk_pin, spi_config[bus].af);

    if (cs == HWCS_LINE) {
        gpio_init((gpio_t)cs, GPIO_DIR_OUT, GPIO_NOPULL);
        gpio_init_af((gpio_t)cs, spi_config[bus].af);
    }
    else {
        gpio_init((gpio_t)cs, GPIO_DIR_OUT, GPIO_NOPULL);
        gpio_set((gpio_t)cs);
    }

    return 0;
}

int spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk, spi_cs_t cs)
{
    /* check device and clock speed for validity */
    if (bus >= SPI_NUMOF || clk > 0x0f) {
        return -1;
    }

    /* lock bus */
    mutex_lock(&locks[bus]);
    /* enable SPI device clock */
    clk_en(bus);
    /* enable device */
    dev(bus)->CR1 = (((clk + spi_config[bus].abpbus) << 3) | mode | SPI_CR1_MSTR);
    if (cs != HWCS_LINE) {
        dev(bus)->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    }
    return 0;
}

void spi_release(spi_t bus)
{
    /* disable device */
    dev(bus)->CR1 = 0;
    clk_dis(bus);
    mutex_unlock(&locks[bus]);
}

void spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont,
                       uint8_t out, uint8_t *in)
{
    spi_transfer_bytes(bus, cs, cont, &out, in, 1);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        uint8_t *out, uint8_t *in, size_t len)
{
    dev(bus)->CR1 |= (SPI_CR1_SPE);     /* this pulls the HW CS line low */
    if (cs != HWCS_LINE) {
        gpio_clear((gpio_t)cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out) ? out[i] : 0;
        while (!(dev(bus)->SR & SPI_SR_TXE));
        dev(bus)->DR = tmp;
        while (!(dev(bus)->SR & SPI_SR_RXNE));
        tmp = dev(bus)->DR;
        if (in) {
            in[i] = tmp;
        }
    }

    if (!cont) {
        if (cs != HWCS_LINE) {
            gpio_set((gpio_t)cs);
        }
        else {
            dev(bus)->CR1 &= ~(SPI_CR1_SPE);    /* pull HW CS line high */
        }
    }
}

void spi_transfer_reg(spi_t bus, spi_cs_t cs,
                      uint8_t reg, uint8_t out, uint8_t *in)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1);
    spi_transfer_bytes(bus, cs, false, &out, in, 1);
}

void spi_transfer_regs(spi_t bus, spi_cs_t cs,
                       uint8_t reg, uint8_t *out, uint8_t *in, size_t len)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1);
    spi_transfer_bytes(bus, cs, false, out, in, len);
}
