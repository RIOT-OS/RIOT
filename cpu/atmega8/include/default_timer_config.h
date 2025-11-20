/*
 * SPDX-FileCopyrightText: 2015 HAW Hamburg
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2023 Hugues Larrive
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_atmega8
 * @{
 *
 * @file
 * @brief           Default timer configuration
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Hugues Larrive <hugues.larrive@pm.me>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIMER_NUMOF
#define TIMER_NUMOF         (1U)
#define TIMER_CHANNEL_NUMOF (2)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK
#define TIMER_0_FLAG        &TIFR
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
#endif /* TIMER_NUMOF */

#ifdef __cplusplus
}
#endif

/** @} */
