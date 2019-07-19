/*
 * Copyright (C) 2019 Wlad Meixner
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* Memory Space Definitions: */
MEMORY
{
    rom (rx)    : ORIGIN = 0x20004000, LENGTH = 16K
    ram (w!rx)  : ORIGIN = 0x20008000, LENGTH = 240K - 16K
}

INCLUDE cortexm_base.ld