/*
 * Copyright (C) 2015 Dinh Nguyen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200
 * @{
 *
 * @file
 * @brief           cc3200 GPIO control
 *
 * @author          Dinh Nguyen <nqdinhddt@gmail.com>
 */

#ifndef __CC3200_GPIO_H__
#define __CC3200_GPIO_H__

#include "cpu.h"

void gpio_set(uint32_t gpio, uint32_t value);

#endif