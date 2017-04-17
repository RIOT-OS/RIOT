/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_S5P4418
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Dang Minh Phuong <kamejoko80@yahoo.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_GPIO_T
#define GPIO_PIN(port,pin) ((gpio_t)(32*(port)+(pin)))
#define GPIO_UNDEF         ((gpio_t)(160))

typedef uint8_t gpio_t;


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
