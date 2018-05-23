/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#ifdef MODULE_PERIPH_I2C
#include "periph/i2c.h"
#endif
#ifdef MODULE_PERIPH_SPI
#include "periph/spi.h"
#endif
#ifdef MODULE_PERIPH_RTC
#include "periph/rtc.h"
#endif
#ifdef MODULE_PERIPH_RTT
#include "periph/rtt.h"
#endif
#ifdef MODULE_PERIPH_HWRNG
#include "periph/hwrng.h"
#endif

void periph_init(void)
{
    /* initialize configured I2C devices */
#ifdef I2C_NUMOF
    for (unsigned i = 0; i < I2C_NUMOF; i++) {
        i2c_init(I2C_DEV(i));
    }
#endif

    /* initialize configured SPI devices */
#ifdef MODULE_PERIPH_SPI
    for (unsigned i = 0; i < SPI_NUMOF; i++) {
        spi_init(SPI_DEV(i));
    }
#endif

    /* Initialize RTC */
#ifdef MODULE_PERIPH_RTC
    rtc_init();
#endif

    /* Initialize RTT */
#ifdef MODULE_PERIPH_RTT
    rtt_init();
#endif

#ifdef MODULE_PERIPH_HWRNG
    hwrng_init();
#endif
}
