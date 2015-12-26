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

#if SPI_NUMOF

#ifdef PERIPH_SPI_NEEDS_TRANSFER_BYTES
int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    int trans_ret;
    unsigned trans_bytes = 0;
    char in_temp;

    for (trans_bytes = 0; trans_bytes < length; trans_bytes++) {
        if (out != NULL) {
            trans_ret = spi_transfer_byte(dev, out[trans_bytes], &in_temp);
        }
        else {
            trans_ret = spi_transfer_byte(dev, 0, &in_temp);
        }
        if (trans_ret < 0) {
            return -1;
        }
        if (in != NULL) {
            in[trans_bytes] = in_temp;
        }
    }

    return trans_bytes;
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_REG
int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    int trans_ret;

    trans_ret = spi_transfer_byte(dev, reg, in);
    if (trans_ret < 0) {
        return -1;
    }
    trans_ret = spi_transfer_byte(dev, out, in);
    if (trans_ret < 0) {
        return -1;
    }

    return 1;
}
#endif

#ifdef PERIPH_SPI_NEEDS_TRANSFER_REGS
int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    int trans_ret;

    trans_ret = spi_transfer_byte(dev, reg, in);
    if (trans_ret < 0) {
        return -1;
    }
    trans_ret = spi_transfer_bytes(dev, out, in, length);
    if (trans_ret < 0) {
        return -1;
    }

    return trans_ret;
}
#endif

#endif /* SPI_NUMOF */
