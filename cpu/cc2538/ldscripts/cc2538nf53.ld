/*
 * Copyright (C) 2014 Loci Controls Inc.
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
 * @brief           Linker script for the CC2538NF53 and CC2538SF53 model MCUs
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

/* Memory Space Definitions: */
MEMORY
{
    rom (rx)    : ORIGIN = 0x00200000, LENGTH = 512K - 44
    cca         : ORIGIN = 0x0027ffd4, LENGTH = 44
    sram0       : ORIGIN = 0x20000000, LENGTH = 16K /* Lost in PM2 and PM3 */
    sram1       : ORIGIN = 0x20004000, LENGTH = 16K
    ram (w!rx)  : ORIGIN = 0x20000000, LENGTH = 32K
}

/* MCU Specific Section Definitions */
SECTIONS
{
    .flashcca :
    {
        KEEP(*(.flashcca))
    } > cca
}

INCLUDE cortexm_base.ld

/* @} */
