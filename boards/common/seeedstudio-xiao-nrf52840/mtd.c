/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840
 * @ingroup     boards_seeedstudio-xiao-nrf52840-sense
 * @{
 *
 * @file
 * @brief       MTD configuration for the XIAO nRF52840
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#ifdef MODULE_MTD

#include "board.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "periph_conf.h"
#include "timex.h"

static const mtd_spi_nor_params_t _xiao_nrf52840_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    /* datasheet reports 20ms, but experiments showed ~135ms*/
    .wait_chip_erase = 200LU * US_PER_MS,
    .wait_32k_erase = 20LU * US_PER_MS,
    .wait_sector_erase = 20LU * US_PER_MS,
    .wait_chip_wake_up = 35LU * US_PER_MS,
    .clk = XIAO_NRF52840_NOR_SPI_CLK,
    .flag = XIAO_NRF52840_NOR_FLAGS,
    .spi = XIAO_NRF52840_NOR_SPI_DEV,
    .mode = XIAO_NRF52840_NOR_SPI_MODE,
    .cs = XIAO_NRF52840_NOR_SPI_CS,
    .wp = XIAO_NRF52840_NOR_SPI_WP,
    .hold = XIAO_NRF52840_NOR_SPI_HOLD,
};

static mtd_spi_nor_t xiao_nrf52840_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = XIAO_NRF52840_NOR_PAGE_SIZE,
        .pages_per_sector = XIAO_NRF52840_NOR_PAGES_PER_SECTOR,
        .sector_count = XIAO_NRF52840_NOR_SECTOR_COUNT,
    },
    .params = &_xiao_nrf52840_nor_params,
};

MTD_XFA_ADD(xiao_nrf52840_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(xiao_nrf52840_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif
