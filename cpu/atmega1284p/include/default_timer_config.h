/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_atmega1284p
 * @{
 *
 * @file
 * @brief           Default timer configuration
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "periph_cpu_common.h"
#include "periph_conf.h" /* <- Allow overwriting timer config from board */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIMER_NUMOF
#define TIMER_NUMOF         (2U)
#define TIMER_CHANNEL_NUMOF (2)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect

#define TIMER_1             MEGA_TIMER3
#define TIMER_1_MASK        &TIMSK3
#define TIMER_1_FLAG        &TIFR3
#define TIMER_1_ISRA        TIMER3_COMPA_vect
#define TIMER_1_ISRB        TIMER3_COMPB_vect
#endif /* TIMER_NUMOF */

#ifdef __cplusplus
}
#endif

/** @} */
