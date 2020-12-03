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

#define USB_H_USER_IS_RIOT_INTERNAL

#include "periph_cpu.h"

#ifdef MODULE_PERIPH_INIT
#ifdef MODULE_PERIPH_INIT_I2C
#include "periph/i2c.h"
#endif
#ifdef MODULE_PERIPH_INIT_SPI
#include "periph/spi.h"
#endif
#ifdef MODULE_PERIPH_INIT_RTC
#include "periph/rtc.h"
#endif
#ifdef MODULE_PERIPH_INIT_RTT
#include "periph/rtt.h"
#endif
#ifdef MODULE_PERIPH_INIT_HWRNG
#include "periph/hwrng.h"
#endif
#ifdef MODULE_PERIPH_INIT_USBDEV
#include "periph/usbdev.h"
#endif
#ifdef MODULE_PERIPH_INIT_WDT
#include "periph/wdt.h"
#endif
#ifdef MODULE_PERIPH_INIT_PTP
#include "periph/ptp.h"
#endif
#endif /* MODULE_PERIPH_INIT */

void periph_init(void)
{
#ifdef MODULE_PERIPH_INIT
    /* initialize configured I2C devices */
#ifdef MODULE_PERIPH_INIT_I2C
    for (unsigned i = 0; i < I2C_NUMOF; i++) {
        i2c_init(I2C_DEV(i));
    }
#endif

    /* initialize configured SPI devices */
#ifdef MODULE_PERIPH_INIT_SPI
    for (unsigned i = 0; i < SPI_NUMOF; i++) {
        spi_init(SPI_DEV(i));
    }
#endif

    /* Initialize RTT before RTC to allow for RTT based RTC implementations */
#ifdef MODULE_PERIPH_INIT_RTT
    rtt_init();
#endif

    /* Initialize RTC */
#ifdef MODULE_PERIPH_INIT_RTC
    rtc_init();
#endif

    /* Initialize Tamper Detection */
#ifdef MODULE_PERIPH_INIT_GPIO_TAMPER_WAKE
    rtc_tamper_init();
#endif

#ifdef MODULE_PERIPH_INIT_HWRNG
    hwrng_init();
#endif

#ifdef MODULE_PERIPH_INIT_USBDEV
    usbdev_init_lowlevel();
#endif

#if defined(MODULE_PERIPH_INIT_WDT) && WDT_HAS_INIT
    wdt_init();
#endif

#if defined(MODULE_PERIPH_INIT_PTP)
    ptp_init();
#endif

#endif /* MODULE_PERIPH_INIT */
}
