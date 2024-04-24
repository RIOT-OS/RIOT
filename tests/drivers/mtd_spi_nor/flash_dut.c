/*
 * Copyright (C) 2024 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 *
 * @file
 * @brief       Definitions for different Flash chips that can be tested
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#include "timex.h"
#include "mtd_spi_nor.h"

/* The default CS pin and SPI device is for the built-in flash of the nRF52840DK */
#ifndef FLASH_SPI_CS
#define FLASH_SPI_CS    GPIO_PIN(0, 17)
#endif

#ifndef FLASH_SPI_DEV
#define FLASH_SPI_DEV   0
#endif

#ifdef TEST_IS25LP128

#define IS25LP128_PAGE_SIZE         (256)
#define IS25LP128_PAGES_PER_SECTOR  (16)
#define IS25LP128_SECTOR_COUNT      (4096)
#define IS25LP128_FLAGS             (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | \
                                     SPI_NOR_F_SECT_64K)
#define IS25LP128_SPI_CLK           SPI_CLK_100KHZ
#define IS25LP128_SPI_MODE          SPI_MODE_0

static const mtd_spi_nor_params_t _is25lp128_flash_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 30LU * US_PER_SEC,
    .wait_32k_erase = 100LU *US_PER_MS,
    .wait_sector_erase = 70LU * US_PER_MS,
    .wait_chip_wake_up = 100LU * US_PER_MS,
    .clk = IS25LP128_SPI_CLK,
    .flag = IS25LP128_FLAGS,
    .spi = SPI_DEV(FLASH_SPI_DEV),
    .mode = IS25LP128_SPI_MODE,
    .cs = FLASH_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t _is25lp128_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = IS25LP128_PAGE_SIZE,
        .pages_per_sector = IS25LP128_PAGES_PER_SECTOR,
        .sector_count = IS25LP128_SECTOR_COUNT,
    },
    .params = &_is25lp128_flash_nor_params,
};

MTD_XFA_ADD(_is25lp128_nor_dev, 10);

#elif defined TEST_IS25LE01G

#define IS25LE01G_PAGE_SIZE         (256)
#define IS25LE01G_PAGES_PER_SECTOR  (16)
#define IS25LE01G_SECTOR_COUNT      (32768)
#define IS25LE01G_FLAGS             (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | \
                                     SPI_NOR_F_SECT_64K)
#define IS25LE01G_SPI_CLK           SPI_CLK_10MHZ
#define IS25LE01G_SPI_MODE          SPI_MODE_0

static const mtd_spi_nor_params_t _is25le01g_flash_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 90LU * US_PER_SEC,
    .wait_32k_erase = 140LU * US_PER_MS,
    .wait_sector_erase = 100LU * US_PER_MS,
    .wait_chip_wake_up = 35LU * US_PER_MS,
    .clk = IS25LE01G_SPI_CLK,
    .flag = IS25LE01G_FLAGS,
    .spi = SPI_DEV(FLASH_SPI_DEV),
    .mode = IS25LE01G_SPI_MODE,
    .cs = FLASH_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t _is25le01g_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = IS25LE01G_PAGE_SIZE,
        .pages_per_sector = IS25LE01G_PAGES_PER_SECTOR,
        .sector_count = IS25LE01G_SECTOR_COUNT,
    },
    .params = &_is25le01g_flash_nor_params,
};

MTD_XFA_ADD(_is25le01g_nor_dev, 10);

#elif defined TEST_MX25L12873F

#define MX25L12873F_PAGE_SIZE           (256)
#define MX25L12873F_PAGES_PER_SECTOR    (16)
#define MX25L12873F_SECTOR_COUNT        (4096)
#define MX25L12873F_FLAGS               (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | \
                                         SPI_NOR_F_SECT_64K)
#define MX25L12873F_SPI_CLK             SPI_CLK_10MHZ
#define MX25L12873F_SPI_MODE            SPI_MODE_0

static const mtd_spi_nor_params_t _mx25l12873f_flash_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 50LU * US_PER_SEC,
    .wait_32k_erase = 150LU *US_PER_MS,
    .wait_sector_erase = 40LU * US_PER_MS,
    .wait_chip_wake_up = 35LU * US_PER_MS,
    .clk = MX25L12873F_SPI_CLK,
    .flag = MX25L12873F_FLAGS,
    .spi = SPI_DEV(FLASH_SPI_DEV),
    .mode = MX25L12873F_SPI_MODE,
    .cs = FLASH_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t _mx25l12873f_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = MX25L12873F_PAGE_SIZE,
        .pages_per_sector = MX25L12873F_PAGES_PER_SECTOR,
        .sector_count = MX25L12873F_SECTOR_COUNT,
    },
    .params = &_mx25l12873f_flash_nor_params,
};

MTD_XFA_ADD(_mx25l12873f_nor_dev, 10);

#endif /* TEST_MX25L12873F */
/** @} */
