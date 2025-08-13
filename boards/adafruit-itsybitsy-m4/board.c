/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_adafruit-itsybitsy-m4
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Adafruit ItsyBitsy M4
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "mtd_spi_nor.h"
#include "timex.h"
#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
#endif

#ifdef MODULE_MTD
/* GD25x16 */
static const mtd_spi_nor_params_t _samd51_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 15 * US_PER_SEC,
    .wait_32k_erase = 150 * US_PER_MS,
    .wait_64k_erase = 250 * US_PER_MS,
    .wait_sector_erase = 50 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(54),
    .flag = SPI_NOR_F_SECT_4K
          | SPI_NOR_F_SECT_32K
          | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(1),
    .mode = SPI_MODE_0,
    .cs   = SAM0_QSPI_PIN_CS,
    .wp   = SAM0_QSPI_PIN_DATA_2,
    .hold = SAM0_QSPI_PIN_DATA_3,
};

static mtd_spi_nor_t samd51_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_samd51_nor_params,
};

MTD_XFA_ADD(samd51_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(samd51_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif /* MODULE_MTD */

static inline void _toggle(unsigned n)
{
    n *= 2;
    while (--n) {
        /* This might break if the GPIO driver gets more optimized, but
         * with the current implementation the toggle is slow enough :)  */
        gpio_toggle(APA102_PARAM_CLK_PIN);
    }
}

void board_init(void)
{
    /* if the real driver is used, it will deal with the LED */
    if (IS_USED(MODULE_APA102)) {
        return;
    }

    /* bootloader leaves on an annoyingly bright LED - turn it off manually */
    gpio_init(APA102_PARAM_DATA_PIN, GPIO_OUT);
    gpio_init(APA102_PARAM_CLK_PIN, GPIO_OUT);

    /* start frame - 32 zero bits */
    gpio_clear(APA102_PARAM_DATA_PIN);
    _toggle(32);

    /* LED frame: 3 start bits (1), 5 alpha bits (0), 24 color bits (0) */
    gpio_set(APA102_PARAM_DATA_PIN);
    _toggle(3);
    gpio_clear(APA102_PARAM_DATA_PIN);
    _toggle(29); /* 5 alpha + 8 red + 8 green + 8 blue */

    /* end frame - 32 one bits */
    gpio_set(APA102_PARAM_DATA_PIN);
    _toggle(32);
}
