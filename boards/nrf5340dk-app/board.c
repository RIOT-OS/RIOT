/*
 * SPDX-FileCopyrightText: 2023 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_nrf5340dk-app
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Nordic nRF5340DK board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
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
/* MX25R64 */
static const mtd_spi_nor_params_t _nrf5340_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 240 * US_PER_SEC,
    .wait_64k_erase = 800 * US_PER_MS,
    .wait_sector_erase = 240 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(54),
    .flag = SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(0),
    .mode = SPI_MODE_0,
    .cs   = BOARD_QSPI_PIN_CS,
    .wp   = BOARD_QSPI_PIN_WP,
    .hold = BOARD_QSPI_PIN_HOLD,
};

static mtd_spi_nor_t nrf5340_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_nrf5340_nor_params,
};
MTD_XFA_ADD(nrf5340_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(nrf5340_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif /* MODULE_MTD_SPI_NOR */
