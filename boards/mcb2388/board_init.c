/*
 * SPDX-FileCopyrightText: 2019 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_mcb2388
 * @{
 */

/**
 * @file
 * @brief       MCB2388 board initialization
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "mtd.h"
#include "periph/init.h"

#ifdef MODULE_MTD_MCI
extern const mtd_desc_t mtd_mci_driver;
static mtd_dev_t _mtd_mci = { .driver = &mtd_mci_driver };
MTD_XFA_ADD(_mtd_mci, 0);
#endif

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
VFS_AUTO_MOUNT(fatfs, { .dev = &_mtd_mci }, VFS_DEFAULT_SD(0), 0);
#endif

void led_init(void)
{
    /* LEDs */
    FIO2DIR |= LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK \
            |  LED4_MASK | LED5_MASK | LED6_MASK | LED7_MASK;
}
