/*
 * board_init.c - redbee-econotag initialization code
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
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
