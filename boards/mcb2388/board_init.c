/*
 * Copyright (C) 2019 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "stdio_base.h"

#ifdef MODULE_MTD_MCI
extern const mtd_desc_t mtd_mci_driver;
static mtd_dev_t _mtd_mci = { .driver = &mtd_mci_driver };
mtd_dev_t *mtd0 = &_mtd_mci;
#endif

void board_init(void)
{
    /* LEDS */
    FIO2DIR |= LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK \
            |  LED4_MASK | LED5_MASK | LED6_MASK | LED7_MASK;
}
