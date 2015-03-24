/*
 * Copyright 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef LTC4150_H
#define LTC4150_H

#include "ltc4150_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

void ltc4150_init(void);
void ltc4150_start(void);
void ltc4150_stop(void);

double ltc4150_get_current_mA(void);
double ltc4150_get_total_mAh(void);
double ltc4150_get_total_Joule(void);
double ltc4150_get_avg_mA(void);
int ltc4150_get_interval(void);
long ltc4150_get_intcount(void);

#ifdef __cplusplus
}
#endif

#endif /* LTC4150_H */
