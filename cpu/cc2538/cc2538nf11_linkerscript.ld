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
 * @file            cc2538nf11_linkerscript.ld
 * @brief           Linker script for the CC2538NF11 model MCU
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

/* Memory Space Definitions: */
MEMORY
{
    rom   (rx ) : ORIGIN = 0x00200000, LENGTH = 128K - 44
    cca   (rx ) : ORIGIN = 0x0027ffd4, LENGTH = 44
    sram1 (rwx) : ORIGIN = 0x20004000, LENGTH = 16K
    ram   (rwx) : ORIGIN = 0x20004000, LENGTH = 16K
}

INCLUDE cc2538_linkerscript.ld

/* @} */
