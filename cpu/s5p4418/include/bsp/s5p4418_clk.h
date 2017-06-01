/*
 * Copyright (C) 2009 jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

/**
 * @brief clock structure
 */
struct clk {
    unsigned long rate; /**< Clock rate */
};

/**
 * @brief device structure
 */
struct device {
    char *init_name; /**< init name */
};

#define dev_name(d) (d->init_name)

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

#endif
