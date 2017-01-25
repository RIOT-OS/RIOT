/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_init
 * @{
 *
 * @file
 * @brief       Common static peripheral driver initialization implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph/spi.h"

void periph_init(void)
{
    /* initialize configured SPI devices */
#ifdef SPI_NUMOF
    for (unsigned i = 0; i < SPI_NUMOF; i++) {
        spi_init(SPI_DEV(i));
    }
#endif
}
