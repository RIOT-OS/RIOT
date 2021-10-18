/*
 * Copyright (C) 2014-2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_mulle
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Mulle board
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stddef.h> /* for NULL */
#include <stdio.h>
#include "board.h"
#include "cpu.h"
#include "mcg.h"
#include "periph/gpio.h"
#include "periph/rtt.h"
#include "periph/spi.h"
#include "nvram-spi.h"
#include "nvram.h"
#include "xtimer.h"
#include "vfs.h"
#include "fs/devfs.h"
#include "mtd_spi_nor.h"

static nvram_t mulle_nvram_dev;
nvram_t *mulle_nvram = &mulle_nvram_dev;
static nvram_spi_params_t nvram_spi_params = {
        .spi = MULLE_NVRAM_SPI_DEV,
        .clk = MULLE_NVRAM_SPI_CLK,
        .cs = MULLE_NVRAM_SPI_CS,
        .address_count = MULLE_NVRAM_SPI_ADDRESS_COUNT,
};

static devfs_t mulle_nvram_devfs = {
    .path = "/fram0",
    .f_op = &nvram_vfs_ops,
    .private_data = &mulle_nvram_dev,
};

static const mtd_spi_nor_params_t mulle_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 16LU * US_PER_SEC,
    .wait_sector_erase = 10LU * US_PER_MS,
    .wait_32k_erase = 20LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .spi = MULLE_NOR_SPI_DEV,
    .addr_width = 3,
    .mode = SPI_MODE_3,
    .cs = MULLE_NOR_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
    .clk = SPI_CLK_10MHZ,
};

static mtd_spi_nor_t mulle_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 256,
        .sector_count = 32,
    },
    .params = &mulle_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&mulle_nor_dev;

static devfs_t mulle_nor_devfs = {
    .path = "/mtd0",
    .f_op = &mtd_vfs_ops,
    .private_data = &mulle_nor_dev,
};

/** @brief Initialize the GPIO pins controlling the power switches. */
static inline void power_pins_init(void);

static void increase_boot_count(void);
static int mulle_nvram_init(void);

int mulle_nor_init(void);

void board_init(void)
{
    int status;

    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* Initialize power control pins */
    power_pins_init();

    /* Turn on Vperiph for peripherals */
    /*
     * By turning on Vperiph first, and before waiting for the clocks to
     * stabilize, we will have used enough time to have let the FRAM start up
     * properly when we want to access it later without having to add any extra
     * delays.
     */
    gpio_set(MULLE_POWER_VPERIPH);

    /* Turn on AVDD for reading voltages */
    gpio_set(MULLE_POWER_AVDD);


    /* NVRAM requires xtimer for timing */
    xtimer_init();

    /* Initialize NVRAM */
    status = mulle_nvram_init();
    if (status == 0) {
        /* Increment boot counter */
        increase_boot_count();
    }

    /* Initialize NOR flash */
    mulle_nor_init();
}

static inline void power_pins_init(void)
{
    gpio_init(MULLE_POWER_AVDD, GPIO_OUT);
    gpio_init(MULLE_POWER_VPERIPH, GPIO_OUT);
    gpio_init(MULLE_POWER_VSEC, GPIO_OUT);
    gpio_clear(MULLE_POWER_AVDD);
    gpio_clear(MULLE_POWER_VPERIPH);
    gpio_clear(MULLE_POWER_VSEC);
}

static int mulle_nvram_init(void)
{
    union {
        uint32_t u32;
        uint8_t  u8[sizeof(uint32_t)];
    } rec;
    rec.u32 = 0;

    if (nvram_spi_init(mulle_nvram, &nvram_spi_params, MULLE_NVRAM_CAPACITY) != 0) {
        return -2;
    }

    if (mulle_nvram->read(mulle_nvram, &rec.u8[0], MULLE_NVRAM_MAGIC, sizeof(rec.u32)) != sizeof(rec.u32)) {
        return -3;
    }

    if (rec.u32 != MULLE_NVRAM_MAGIC_EXPECTED) {
        int i;
        union {
            uint64_t u64;
            uint8_t  u8[sizeof(uint64_t)];
        } zero;
        zero.u64 = 0;
        for (i = 0; i < MULLE_NVRAM_CAPACITY; i += sizeof(zero)) {
            if (mulle_nvram->write(mulle_nvram, &zero.u8[0], i, sizeof(zero.u64)) != sizeof(zero.u64)) {
                return -4;
            }
        }
        rec.u32 = MULLE_NVRAM_MAGIC_EXPECTED;
        if (mulle_nvram->write(mulle_nvram, &rec.u8[0], MULLE_NVRAM_MAGIC, sizeof(rec.u32)) != sizeof(rec.u32)) {
            return -5;
        }
    }

    /* Register DevFS node */
    return devfs_register(&mulle_nvram_devfs);
}

static void increase_boot_count(void)
{
    union {
        uint32_t u32;
        uint8_t  u8[sizeof(uint32_t)];
    } rec;
    rec.u32 = 0;
    if (mulle_nvram->read(mulle_nvram, &rec.u8[0], MULLE_NVRAM_BOOT_COUNT, sizeof(rec.u32)) != sizeof(rec.u32)) {
        return;
    }
    ++rec.u32;
    mulle_nvram->write(mulle_nvram, &rec.u8[0], MULLE_NVRAM_BOOT_COUNT, sizeof(rec.u32));
}

int mulle_nor_init(void)
{
    int res = mtd_init(mtd0);

    if (res >= 0) {
        /* Register DevFS node */
        res = devfs_register(&mulle_nor_devfs);
    }

    return res;
}
