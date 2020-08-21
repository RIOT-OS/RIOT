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
 * @brief       This module module privides round robin scheduling within
 *              for all runnable threads within each not masked priority.
 *              Priority 0 is masked by default.
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
 * @brief masks off priorities that should not be scheduled default 0
 */
#define SCHED_RR_MASK 1<<0
#endif

/**
 * @brief the round robin scheduler
 */
void schedule_rr(void *);

/**
 * @brief stop the scheduler
 */
void stop_schedule_rr(void);

/**
 * @brief start the scheduler
 */
void start_schedule_rr(void);
