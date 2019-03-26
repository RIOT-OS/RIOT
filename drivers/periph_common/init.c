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

#include "cpu.h"

#ifndef HAVE_ARCH_STDIO_INIT
#include "stdio_base.h"
#endif
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

#ifndef HAVE_ARCH_STDIO_INIT
/**
 * @brief Initializes stdio
 *
 * @details This function can be overwritten by `#define`ing
 * `HAVE_ARCH_STDIO_INIT` in cpu.h and declaring a function with the same
 * signature there. This allows handling architecture specific stuff if needed.
 */
static inline void arch_stdio_init(void)
{
    stdio_init();
}
#endif /* HAVE_ARCH_STDIO_INIT */

void periph_init(void)
{
    /* initialize stdio first to allow DEBUG() during later stages */
#if defined(MODULE_STDIO_UART) || defined(MODULE_STDIO_RTT)
    arch_stdio_init();
#endif

    /* initialize configured I2C devices */
#ifdef MODULE_PERIPH_I2C
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
