/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_same54-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Microchip SAM E54 Xplained
 *              Pro board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "mtd_spi_nor.h"
#include "timex.h"

#ifdef MODULE_MTD
/* N25Q256A */
static const mtd_spi_nor_params_t _same54_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 240 * US_PER_SEC,
    .wait_64k_erase = 700 * US_PER_MS,
    .wait_sector_erase = 250 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(54),
    .flag = SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(2),
    .mode = SPI_MODE_0,
    .cs   = SAM0_QSPI_PIN_CS,
    .wp   = SAM0_QSPI_PIN_DATA_2,
    .hold = SAM0_QSPI_PIN_DATA_3,
    .addr_width = 4,
};

static mtd_spi_nor_t same54_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_same54_nor_params,
};
mtd_dev_t *mtd0 = (mtd_dev_t *)&same54_nor_dev;

#include "mtd_at24cxxx.h"
#include "at24cxxx_params.h"
static at24cxxx_t at24cxxx_dev;
static mtd_at24cxxx_t at24mac_dev = {
    .base = {
        .driver = &mtd_at24cxxx_driver,
    },
    .at24cxxx_eeprom = &at24cxxx_dev,
    .params = at24cxxx_params,
};
mtd_dev_t *mtd1 = (mtd_dev_t *)&at24mac_dev;
#endif /* MODULE_MTD */

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;

    /* initialize the on-board button */
    gpio_init(BTN0_PIN, BTN0_MODE);

    /* initialize the CPU */
    cpu_init();
}
