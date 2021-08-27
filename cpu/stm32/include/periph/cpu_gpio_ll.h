/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           GPIO LL CPU definitions for the STM32 family
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_CPU_GPIO_LL_H
#define PERIPH_CPU_GPIO_LL_H

#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Hide this from Doxygen to avoid merging implementation details into
 * public view on type */
#ifndef DOXYGEN

#define HAVE_GPIO_PULL_STRENGTH_T
typedef enum {
    GPIO_PULL_WEAKEST = 0,
    GPIO_PULL_WEAK = 0,
    GPIO_PULL_STRONG = 0,
    GPIO_PULL_STRONGEST = 0
} gpio_pull_strength_t;

#define HAVE_GPIO_DRIVE_STRENGTH_T
typedef enum {
    GPIO_DRIVE_WEAKEST = 0,
    GPIO_DRIVE_WEAK = 0,
    GPIO_DRIVE_STRONG = 0,
    GPIO_DRIVE_STRONGEST = 0
} gpio_drive_strength_t;

#define HAVE_GPIO_IRQ_TRIG_T
/*
 * Layout:
 *      7 6 5 4 3 2 1 0
 *      +-+-+-+-+-+-+-+-+
 *      |   RFU   |T|L|H|
 *      +-+-+-+-+-+-+-+-+
 *
 * RFU = Reserved for future use
 * T   = Trigger mode (1 = Level triggered, 0 = Edge triggered)
 * L   = Low (1 = low level / falling edge)
 * H   = High (1 = high level / rising edge)
 *
 * Note: This layout overlaps with gpio_flank_t by intent
 */
typedef enum {
    GPIO_TRIGGER_EDGE_RISING    = 0x1,
    GPIO_TRIGGER_EDGE_FALLING   = 0x2,
    GPIO_TRIGGER_EDGE_BOTH      = GPIO_TRIGGER_EDGE_RISING | GPIO_TRIGGER_EDGE_FALLING,
    GPIO_TRIGGER_LEVEL          = 0x4,
    GPIO_TRIGGER_LEVEL_HIGH     = GPIO_TRIGGER_LEVEL | GPIO_TRIGGER_EDGE_RISING,
    GPIO_TRIGGER_LEVEL_LOW      = GPIO_TRIGGER_LEVEL | GPIO_TRIGGER_EDGE_FALLING,
} gpio_irq_trig_t;

#endif /* ndef Doxygen */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_GPIO_LL_H */
/** @} */
