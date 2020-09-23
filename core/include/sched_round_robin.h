/*
 * Copyright (C) 2020 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    Sched_RR
 * @ingroup     sys
 * @brief       This module module provides round robin scheduling within
 *              for all runnable threads within each not masked priority.
 *              Priority 0 is masked by default.
 *              There is no fainess involved in this scheuler. It schedules,
 *              when the timer ticks. Keep it as simple as possible.
 *
 * @note        start using start_schedule_rr()
 *
 * @{
 *
 * @file
 * @brief       Round Robin Scheduler
 *
 * @author      Karl Fessel <karl.fessel@ovgu.de>
 *
 */

#if !defined(SCHED_RR_TIMER) || defined(DOXYGEN)
/**
 * @brief Time between round robin calls in µs
 *
 * @details Defaults to 1ms
 */
#define SCHED_RR_TIMER 1000
#endif

#if !defined(SCHED_RR_MASK) || defined(DOXYGEN)
/**
 * @brief masks off priorities that should not be scheduled default: 0 is masked
 */
#define SCHED_RR_MASK (1 << 0)
#endif

/**
 * @brief setup round robin for priority if possible an sensible
 */
void sched_round_robin_check_set(uint8_t prio);

/**
 * @brief remove round robin if active for prio and setup for active priotrity
 */
void sched_round_robin_check_remove_set(uint8_t prio);
