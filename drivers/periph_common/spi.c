/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 *
 * @}
 */
#include <stddef.h>

#include "board.h"
#include "cpu.h"
#include "periph/spi.h"
#include "periph_cpu.h"

#ifdef PERIPH_SPI_NEEDS_TRANSFER_BYTES
void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        uint8_t *out, uint8_t *in, size_t len)
{
    if (out && in) {
        while (len--) {
            spi_transfer_byte(bus, cs, (cont || len > 0), *out++, in++);
        }
    }
    else if (out) {
        uint8_t tmp;
        while (len--) {
            spi_transfer_byte(bus, cs, (cont || len > 0), *out++, &tmp);
        }
    }
    else if (in) {
        while (len--) {
            spi_transfer_byte(bus, cs, (cont || len > 0), 0, in++);
        }
    }
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_REG
void spi_transfer_reg(spi_t bus, spi_cs_t cs,
                      uint8_t reg, uint8_t out, uint8_t *in)
{
    uint8_t tmp;
    spi_transfer_byte(bus, cs, true, reg, &tmp);
    spi_transfer_byte(bus, cs, false, out, &tmp);
    if (in)
        *in = tmp;
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_REGS
void spi_transfer_regs(spi_t bus, spi_cs_t cs,
                       uint8_t reg, uint8_t *out, uint8_t *in, size_t len)
{
    uint8_t tmp;
    spi_transfer_byte(bus, cs, true, reg, &tmp);
    spi_transfer_bytes(bus, cs, false, out, in, len);
}
#endif
