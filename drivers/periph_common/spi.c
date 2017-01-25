/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup drivers
 * @{
 *
 * @file
 * @brief       common SPI function fallback implementations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
#include <stddef.h>

#include "board.h"
#include "cpu.h"
#include "periph/spi.h"

#ifdef SPI_NUMOF

#ifdef PERIPH_SPI_NEEDS_INIT_CS
int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    if ((cs == SPI_CS_UNDEF) || (cs == GPIO_UNDEF)) {
        return SPI_NOCS;
    }

    gpio_init((gpio_t)cs, GPIO_OUT);
    gpio_set((gpio_t)cs);

    return SPI_OK;
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_BYTE
uint8_t spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont, uint8_t out)
{
    uint8_t in;
    spi_transfer_bytes(bus, cs, cont, &out, &in, 1);
    return in;
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_REG
uint8_t spi_transfer_reg(spi_t bus, spi_cs_t cs, uint8_t reg, uint8_t out)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1);
    return spi_transfer_byte(bus, cs, false, out);
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_REGS
void spi_transfer_regs(spi_t bus, spi_cs_t cs,
                       uint8_t reg, const void *out, void *in, size_t len)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1);
    spi_transfer_bytes(bus, cs, false, out, in, len);
}
#endif

#endif /* SPI_NUMOF */
