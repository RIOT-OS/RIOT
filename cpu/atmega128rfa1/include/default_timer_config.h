/*
 * SPDX-FileCopyrightText: 2017 Josua Arndt, Steffen Robertz, RWTH Aachen
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_atmega128rfa1
 * @{
 *
 * @file
 * @brief           Default timer configuration
 *
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author          Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIMER_NUMOF
#define TIMER_NUMOF         (3U)
#define TIMER_CHANNEL_NUMOF (3)

#define TIMER_0             MEGA_TIMER4
#define TIMER_0_MASK        &TIMSK4
#define TIMER_0_FLAG        &TIFR4
#define TIMER_0_ISRA        TIMER4_COMPA_vect
#define TIMER_0_ISRB        TIMER4_COMPB_vect
#define TIMER_0_ISRC        TIMER4_COMPC_vect

#define TIMER_1             MEGA_TIMER5
#define TIMER_1_MASK        &TIMSK5
#define TIMER_1_FLAG        &TIFR5
#define TIMER_1_ISRA        TIMER5_COMPA_vect
#define TIMER_1_ISRB        TIMER5_COMPB_vect
#define TIMER_1_ISRC        TIMER5_COMPC_vect

#define TIMER_2             MEGA_TIMER1
#define TIMER_2_MASK        &TIMSK1
#define TIMER_2_FLAG        &TIFR1
#define TIMER_2_ISRA        TIMER1_COMPA_vect
#define TIMER_2_ISRB        TIMER1_COMPB_vect
#define TIMER_2_ISRC        TIMER1_COMPC_vect
#endif /* TIMER_NUMOF */

#ifdef __cplusplus
}
#endif

/** @} */
