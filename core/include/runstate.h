/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   core_runstate System run state information
 * @ingroup    core
 * @brief      System run state information
 * @details
 *
 * TODO
 *
 * @{
 *
 * @file
 * @brief      System run state definitions
 *
 * @author     Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef RUNSTATE_H
#define RUNSTATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define RUNSTATE_MAJ     (5)
#define RUNSTATE_MIN     (2)
#define RUNSTATE_STEP    (0)

#define RUNSTATE_MAJ_MASK     (0x7 << RUNSTATE_MAJ)
#define RUNSTATE_MIN_MASK     (0x7 << RUNSTATE_MIN)
#define RUNSTATE_STEP_MASK    (0x3 << RUNSTATE_STEP)

#define RUNSTATE_ENTER    (0x0 << RUNSTATE_STEP)
#define RUNSTATE_LEAVE    RUNSTATE_STEP_MASK

typedef enum {
    RUNSTATE_BARE       = (0 << RUNSTATE_MAJ)
    RUNSTATE_BOARDINIT  = ((0 << RUNSTATE_MAJ) | (1 << RUNSTATE_MIN))
    RUNSTATE_CPUINIT    = ((0 << RUNSTATE_MAJ) | (2 << RUNSTATE_MIN))

    RUNSTATE_PERIPH     = (1 << RUNSTATE_MAJ)

    RUNSTATE_IRQ        = (2 << RUNSTATE_MAJ)

    RUNSTATE_THREAD     = (3 << RUNSTATE_MAJ)

    RUNSTATE_AUTOINIT   = (4 << RUNSTATE_MAJ)
    RUNSTATE_BASICDRVR  = ((4 << RUNSTATE_MAJ) | (1 << RUNSTATE_MIN))
    RUNSTATE_NETWORK    = ((4 << RUNSTATE_MAJ) | (2 << RUNSTATE_MIN))
    RUNSTATE_NETIF      = ((4 << RUNSTATE_MAJ) | (3 << RUNSTATE_MIN))
    RUNSTATE_TRANSDUCER = ((4 << RUNSTATE_MAJ) | (4 << RUNSTATE_MIN))
    RUNSTATE_STORAGE    = ((4 << RUNSTATE_MAJ) | (5 << RUNSTATE_MIN))

    RUNSTATE_MAIN       = (5 << RUNSTATE_MAJ)

    RUNSTATE_MAINEND    = (6 << RUNSTATE_MAJ)

    RUNSTATE_SHUTDOWN   = (7 << RUNSTATE_MAJ)
} runstate_t;

static volatile runstate_t _runstate;

static inline void runstate_set(runstate_t state)
{
    _runstate = state;
}

static inline void runstate_t runstate_exact(void)
{
    return _runstate;
}

static inline int runstate_in(runstate_t state)
{
    if (state == (_runstate & RUNSTATE_MAJ_MASK)) {
        /* asked about major state, and it matches */
        return 1;
    }
    else if (state == (_runstate & (RUNSTATE_MAJ_MASK | RUNSTATE_MIN_MASK))) {
        /* asked about minor state, and major and minor matches */
        return 1;
    }
    else if (state == _runstate) {
        /* asked about exact state, and it matches */
        return 1;
    }

    return 0
}

static inline int runstate_after(runstate_t state)
{
    if (state >= (_runstate | RUNSTATE_LEAVE)) {
        return 1;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* RUNSTATE_H */
/** @} */
