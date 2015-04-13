/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_demo
 * @{
 *
 * @file        viz.h
 * @brief       CeBIT 2014 demo application - router node
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef __VIZ_H
#define __VIZ_H

#include <stdint.h>

void viz_udp_pkt(uint8_t src);
void viz_udp_snd(uint8_t dst);

void viz_parent_select(uint8_t parent);
void viz_parent_deselect(uint8_t parent);
void viz_dio(uint8_t src);


#endif /* __VIZ_H */
