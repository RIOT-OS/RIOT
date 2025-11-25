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
#include "timex.h"
#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
#endif

#ifdef MODULE_MTD_SPI_NOR
#include "mtd_spi_nor.h"
/* N25Q256A or SST26VF064B */
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
};

static mtd_spi_nor_t same54_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_same54_nor_params,
};
MTD_XFA_ADD(same54_nor_dev, 0);

#ifdef MODULE_VFS_DEFAULT
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(same54_nor_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif /* MODULE_MTD_SPI_NOR */

#ifdef MODULE_MTD_AT24CXXX
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
MTD_XFA_ADD(at24mac_dev, 1);

#endif /* MODULE_MTD_AT24CXXX */

#ifdef MODULE_SAM0_SDHC
#include "mtd_sam0_sdhc.h"
static mtd_sam0_sdhc_t sdhc_dev = {
        .base = {
            .driver = &mtd_sam0_sdhc_driver,
        },
        .state = {
            .dev = SDHC1,
            .cd  = GPIO_PIN(PD, 20),
            .wp  = GPIO_PIN(PD, 21),
        },
    };
MTD_XFA_ADD(sdhc_dev, 2);

#ifdef MODULE_VFS_DEFAULT
/* default to FAT */
#if defined(MODULE_FATFS_VFS)
VFS_AUTO_MOUNT(fatfs, VFS_MTD(sdhc_dev), VFS_DEFAULT_SD(0), 1);
/* but also support ext2/3/4 */
#elif defined(MODULE_LWEXT4)
VFS_AUTO_MOUNT(lwext4, VFS_MTD(sdhc_dev), VFS_DEFAULT_SD(0), 1);
#endif
#endif /* MODULE_VFS_DEFAULT */
#endif /* MODULE_SAM0_SDHC */
