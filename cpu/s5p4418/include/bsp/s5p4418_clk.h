/*
 * (C) Copyright 2009
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __S5P4418_CLK_H__
#define __S5P4418_CLK_H__

#include <stdint.h>
#include <stdbool.h>
#include <err.h>
#include "s5p4418_chip.h"


#ifdef __cplusplus
extern "C"
{
#endif

struct clk {
    unsigned long rate;
};

struct device {
    char *init_name;
};

#define	dev_name(d) (d->init_name)

struct clk *clk_get(struct device *dev, const char *id);
unsigned long clk_get_rate(struct clk *clk);
long clk_round_rate(struct clk *clk, unsigned long rate);
int clk_set_rate(struct clk *clk, unsigned long rate);
int clk_enable(struct clk *clk);
void clk_disable(struct clk *clk);
void s5p4418_clk_init(void);
void s5p4418_clk_print(void);



#ifdef __cplusplus
}
#endif


#endif // __S5P4418_CLK_H__
