/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-grand-central-m4-express
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Adafruit Grand Central M4 Express
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "timex.h"
#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
#endif

#ifdef MODULE_MTD_SPI_NOR

#include "mtd_spi_nor.h"

/* GD25Q64 */
static const mtd_spi_nor_params_t _samd51_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 25 * US_PER_SEC,
    .wait_32k_erase = 150 * US_PER_MS,
    .wait_64k_erase = 200 * US_PER_MS,
    .wait_sector_erase = 50 * US_PER_MS,
    .wait_chip_wake_up = 10 * US_PER_MS,
    .clk  = MHZ(54),
    .flag = SPI_NOR_F_SECT_4K
          | SPI_NOR_F_SECT_32K
          | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(2),
    .mode = SPI_MODE_0,
    .cs   = SAM0_QSPI_PIN_CS,
    .wp   = SAM0_QSPI_PIN_DATA_2,
    .hold = SAM0_QSPI_PIN_DATA_3,
};

static mtd_spi_nor_t samd51_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .sector_count = 2048,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_samd51_nor_params,
};

MTD_XFA_ADD(samd51_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(samd51_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif

#endif /* MODULE_MTD_SPI_NOR */
