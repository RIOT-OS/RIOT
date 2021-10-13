/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* This file is just a mapper for source code compatibility with ESP-IDF */

#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/* the order of these includes is important */
#include "periph_conf.h"
#include "gpio_arch.h"

#define GPIO_NUM_MAX        (40)

#define gpio_num_t          gpio_t
#define gpio_pull_mode_t    uint32_t
#define gpio_drive_cap_t    gpio_drive_strength_t

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* DRIVER_GPIO_H */
