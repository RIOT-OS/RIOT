/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "kernel_defines.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#if MODULE_PERIPH_INIT
#if MODULE_PERIPH_INIT_I2C
#include "periph/i2c.h"
#endif
#if MODULE_PERIPH_INIT_SPI
#include "periph/spi.h"
#endif
#if MODULE_PERIPH_INIT_RTC
#include "periph/rtc.h"
#endif
#if MODULE_PERIPH_INIT_RTT
#include "periph/rtt.h"
#endif
#if MODULE_PERIPH_INIT_HWRNG
#include "periph/hwrng.h"
#endif
#if MODULE_PERIPH_INIT_USBDEV
#include "periph/usbdev.h"
#endif
#if MODULE_PERIPH_INIT_WDT
#include "periph/wdt.h"
#endif
#if MODULE_PERIPH_INIT_PTP
#include "periph/ptp.h"
#endif
#if MODULE_PERIPH_INIT_VBAT
#include "periph/vbat.h"
#endif
#if MODULE_PERIPH_INIT_PIO
#include "periph/pio.h"
#endif
#if MODULE_PERIPH_INIT_SDMMC
#include "sdmmc/sdmmc.h"
#endif
#endif /* MODULE_PERIPH_INIT */

void periph_init(void)
{
#if MODULE_PERIPH_INIT_WDT
    if (WDT_HAS_INIT) {
        wdt_init();
    }

    if (IS_ACTIVE(MODULE_PERIPH_WDT_AUTO_START)) {
        wdt_setup_reboot(CONFIG_PERIPH_WDT_WIN_MIN_MS, CONFIG_PERIPH_WDT_WIN_MAX_MS);
        wdt_start();
    }
#endif

#if MODULE_PERIPH_INIT
    /* initialize buttonss */
    if (MODULE_PERIPH_INIT_BUTTONS) {
        extern void button_init(void);
        button_init();
    }
    /* initialize configured I2C devices */
#if MODULE_PERIPH_INIT_I2C
    for (unsigned i = 0; i < I2C_NUMOF; i++) {
        i2c_init(I2C_DEV(i));
    }
#endif

    /* initialize configured SPI devices */
#if MODULE_PERIPH_INIT_SPI
    for (unsigned i = 0; i < SPI_NUMOF; i++) {
        spi_init(SPI_DEV(i));
    }
#endif

    /* Initialize RTT before RTC to allow for RTT based RTC implementations */
#if MODULE_PERIPH_INIT_RTT
    rtt_init();
#endif

    /* Initialize RTC */
#if MODULE_PERIPH_INIT_RTC
    rtc_init();
#endif

    /* Initialize Tamper Detection */
#if MODULE_PERIPH_INIT_GPIO_TAMPER_WAKE
    rtc_tamper_init();
#endif

#if MODULE_PERIPH_INIT_HWRNG
    hwrng_init();
#endif

#if MODULE_PERIPH_INIT_USBDEV
    usbdev_init_lowlevel();
#endif

#if MODULE_PERIPH_INIT_PTP
    ptp_init();
#endif

#if MODULE_PERIPH_INIT_FMC
    extern void fmc_init(void);
    fmc_init();
#endif

#if MODULE_PERIPH_INIT_VBAT
    vbat_init();
#endif

#if MODULE_PERIPH_INIT_PIO
    for (int i = 0; i < (int)PIO_NUMOF; i++) {
        pio_init(PIO_DEV(i));
    }
#if MODULE_PERIPH_INIT_I2C && defined(PIO_I2C_NUMOF)
    for (unsigned i = 0; i < PIO_I2C_NUMOF; i++) {
        i2c_init(I2C_DEV(I2C_NUMOF + i));
    }
#endif
    pio_start_programs();
#endif

#if MODULE_PERIPH_INIT_SDMMC
    for (unsigned i = 0; i < SDMMC_NUMOF; i++) {
        sdmmc_init(sdmmc_get_dev(i));
    }
#endif

#endif /* MODULE_PERIPH_INIT */
}
