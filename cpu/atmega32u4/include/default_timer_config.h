/*
 * Copyright (C) 2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_atmega32u4
 * @{
 *
 * @file
 * @brief           Default ATmega32U4 Timer Config
 *
 * @author          Thomas Perrot <thomas.perrot@tupi.fr>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIMER_NUMOF
#define TIMER_NUMOF         (2U)
#define TIMER_CHANNEL_NUMOF (3)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
#define TIMER_0_ISRC        TIMER1_COMPC_vect

#define TIMER_1             MEGA_TIMER3
#define TIMER_1_MASK        &TIMSK3
#define TIMER_1_FLAG        &TIFR3
#define TIMER_1_ISRA        TIMER3_COMPA_vect
#define TIMER_1_ISRB        TIMER3_COMPB_vect
#define TIMER_1_ISRC        TIMER3_COMPC_vect
#endif /* TIMER_NUMOF */

#ifdef __cplusplus
}
#endif

/** @} */
