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

#include "kernel_defines.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

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
#ifdef MODULE_PERIPH_INIT_VBAT
#include "periph/vbat.h"
#endif
#ifdef MODULE_PERIPH_INIT_PIO
#include "periph/pio.h"
#endif
#ifdef MODULE_PERIPH_INIT_SDMMC
#include "sdmmc/sdmmc.h"
#endif

#if IS_USED(MODULE_PERIPH_INIT_I2C)
#include "periph/gpio.h"
#include "busy_wait.h"

static void _i2c_sync_devices(i2c_t dev)
{
    /* Recover possible bus hangup by clocking peripheral
     * i2c device state machines into idle.
     * Hangup can occur if a transaction was interrupted
     * by a reset of the MCU.
     * Badly designed hardware can also cause a hangup
     * due to glitches on the bus during operation which
     * we do not aim to mitigate here.
     */
    gpio_init(i2c_config[dev].sda_pin, GPIO_IN);
    gpio_init(i2c_config[dev].scl_pin, GPIO_OUT);
    gpio_set(i2c_config[dev].scl_pin);
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (!gpio_read(i2c_config[dev].sda_pin)) {
            DEBUG("periph_init: i2c bus hangup detected for bus #%u,"
                " recovering...\n", dev);
        }
        else {
            DEBUG("periph_init: i2c bus #%u is ok\n", dev);
        }
    }
    int max_cycles = 10; /* 9 clock cycles should be enough for making
                          * any device that holds the SDA line in low
                          * release the line. Actual number of cyccles may
                          * vary depending on the device state.
                          * The 10th cycle was added in case the first cycle
                          * was not accepted by a device.
                          */
    while (gpio_read(i2c_config[dev].sda_pin) == 0 && max_cycles--) {
        gpio_clear(i2c_config[dev].scl_pin);
        busy_wait_us(100);
        gpio_set(i2c_config[dev].scl_pin);
        busy_wait_us(100);
    }
    if (IS_ACTIVE(ENABLE_DEBUG) && gpio_read(i2c_config[dev].sda_pin) == 0) {
        DEBUG("periph_init: i2c recovery failed for bus #%u\n", dev);
    }
}
#endif /* MODULE_PERIPH_INIT_I2C */

#endif /* MODULE_PERIPH_INIT */

void periph_init(void)
{
#ifdef MODULE_PERIPH_INIT
    /* initialize buttonss */
    if (IS_USED(MODULE_PERIPH_INIT_BUTTONS)) {
        extern void button_init(void);
        button_init();
    }
    /* initialize configured I2C devices */
#ifdef MODULE_PERIPH_INIT_I2C
    for (unsigned i = 0; i < I2C_NUMOF; i++) {
        _i2c_sync_devices(i);
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

#if defined(MODULE_PERIPH_INIT_WDT)
    if (WDT_HAS_INIT) {
        wdt_init();
    }

    if (IS_ACTIVE(MODULE_PERIPH_WDT_AUTO_START)) {
        wdt_setup_reboot(CONFIG_PERIPH_WDT_WIN_MIN_MS, CONFIG_PERIPH_WDT_WIN_MAX_MS);
        wdt_start();
    }
#endif

#if defined(MODULE_PERIPH_INIT_PTP)
    ptp_init();
#endif

#if defined(MODULE_PERIPH_INIT_FMC)
    extern void fmc_init(void);
    fmc_init();
#endif

#if defined(MODULE_PERIPH_INIT_VBAT)
    vbat_init();
#endif

#ifdef MODULE_PERIPH_INIT_PIO
    for (int i = 0; i < (int)PIO_NUMOF; i++) {
        pio_init(PIO_DEV(i));
    }
#if defined(MODULE_PERIPH_INIT_I2C) && defined(PIO_I2C_NUMOF)
    for (unsigned i = 0; i < PIO_I2C_NUMOF; i++) {
        i2c_init(I2C_DEV(I2C_NUMOF + i));
    }
#endif
    pio_start_programs();
#endif

#if defined(MODULE_PERIPH_INIT_SDMMC)
    for (unsigned i = 0; i < SDMMC_NUMOF; i++) {
        sdmmc_init(sdmmc_get_dev(i));
    }
#endif

#endif /* MODULE_PERIPH_INIT */
}
