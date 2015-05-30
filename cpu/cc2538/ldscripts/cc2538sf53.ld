/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           Linker script for the CC2538SF53
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/* Memory Space Definitions: */
MEMORY
{
    rom   (rx ) : ORIGIN = 0x00200000, LENGTH = 512K - 44
    cca   (rx ) : ORIGIN = 0x0027ffd4, LENGTH = 44
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE cc2538.ld

/* @} */
