/*
 * board_init.c - redbee-econotag initialization code
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

void board_init(void)
{
    asm("nop");
}

void bl_init_clks(void)
{
    // dummy to compile
}

void bl_init_ports(void)
{
    // dummy to compile
}
