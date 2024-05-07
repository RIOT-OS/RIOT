/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_iotlab-m3
 * @{
 *
 * @file
 * @brief       MTD configuration for the iotlab-m3 board
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "periph/gpio.h"
#include "timex.h"

#ifdef MODULE_MTD
/* N25Q128A13E1240F */
static const mtd_spi_nor_params_t _mtd_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase   = 240 * US_PER_SEC,
    .wait_64k_erase    = 700 * US_PER_MS,
    .wait_sector_erase = 250 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(54),
    .flag = SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(1),
    .mode = SPI_MODE_0,
    .cs   = GPIO_PIN(PORT_A, 11),
    .wp   = GPIO_PIN(PORT_C, 6),
    .hold = GPIO_PIN(PORT_C, 9),
};

static mtd_spi_nor_t mtd_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_mtd_nor_params,
};

MTD_XFA_ADD(mtd_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(mtd_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif /* MODULE_MTD */
