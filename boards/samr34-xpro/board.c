/*
 * SPDX-FileCopyrightText: 2019 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_samr34-xpro
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Microchip
 *              SAM R34 Xplained Pro board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#endif

#ifdef MODULE_MTD_SPI_NOR
#include "timex.h"
#include "mtd_spi_nor.h"
/* AT25DF041B */
static const mtd_spi_nor_params_t _mtd_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 3600 * US_PER_MS,
    .wait_64k_erase = 450 * US_PER_MS,
    .wait_32k_erase = 250 * US_PER_MS,
    .wait_sector_erase = 35 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(16),
    .flag = SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(1),
    .mode = SPI_MODE_0,
    .cs   = GPIO_PIN(PA, 22),
    .wp   = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t _nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_mtd_nor_params,
};
MTD_XFA_ADD(_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif /* MODULE_MTD_SPI_NOR */

void board_init(void)
{
    /* initialize board specific pins for LoRa */
#ifdef MODULE_SX127X
    gpio_init(TCXO_PWR_PIN, GPIO_OUT);
    gpio_set(TCXO_PWR_PIN);
    gpio_init(TX_OUTPUT_SEL_PIN, GPIO_OUT);
    gpio_write(TX_OUTPUT_SEL_PIN, !SX127X_PARAM_PASELECT);
#endif /* USEMODULE_SX127X */
}
