/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/* Section Definitions */
SECTIONS
{
    .upper.text :
    {
        . = ALIGN(4);
        *(.upper.text.* .upper.text)
    } > HIROM

    .upper.rodata :
    {
        . = ALIGN(4);
        *(.upper.rodata.* .upper.rodata)
    } > HIROM

    .infoC     : {} > INFOC
    .infoD     : {} > INFOD
}
