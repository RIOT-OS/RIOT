/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#ifndef BOARD_INFO_H
#define BOARD_INFO_H

#include <stdint.h>
#include <string.h>

#include "board.h"
#include "periph_conf.h"
#include "timex.h"

#if defined __has_include
#  if __has_include ("openwsn_defs.h")
#    include "openwsn_defs.h"
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    OpenWSN interrupt handling wrappers
 * @{
 */
#define INTERRUPT_DECLARATION()             unsigned irq_state;
#define DISABLE_INTERRUPTS()                irq_state = irq_disable();
#define ENABLE_INTERRUPTS()                 irq_restore(irq_state);
#define SCHEDULER_WAKEUP()                  /* unused by RIOT */
#define SCHEDULER_ENABLE_INTERRUPT()        /* unused by RIOT */
/** @} */


/**
 * @name    OpenWSN platform dependent definitions
 * @{
 */
/* Always 32bit when using ztimer */
#if RTT_MAX_VALUE == UINT32_MAX || !((RTT_MAX_VALUE + 1) & (RTT_MAX_VALUE)) || \
    IS_USED(MODULE_OPENWSN_SCTIMER_ZTIMER)
#define PORT_TIMER_WIDTH                    uint32_t
#define PORT_RADIOTIMER_WIDTH               uint32_t
#else
#error "RTT_MAX_VALUE not supported"
#endif

#if __SIZEOF_POINTER__ == 2
#define PORT_SIGNED_INT_WIDTH               int16_t
#else
#define PORT_SIGNED_INT_WIDTH               int32_t
#endif

#define SCTIMER_FREQUENCY                   (32768U)

/* 32 ticks @32768Hz */
#define PORT_TICS_PER_MS                    (SCTIMER_FREQUENCY / MS_PER_SEC)
/* 30 us per tick @32768Hz */
#define PORT_US_PER_TICK                    (US_PER_SEC / SCTIMER_FREQUENCY)
/** @} */

/**
 * @name    OpenWSN IEEE802154E timings
 * @{
 *
 * @note These parameters are BOARD and CPU specific.
   Values can't be taken directly from OpenWSN since they do not necessarily use
   the same BSP configuration (timers, clock speed, etc.)
   For precise synchronization these values should be measured and tuned for
   every BOARD.
   To understand the relationship between these values and OpenWSN state machine
   as well as how they can be measured refer to:
   - https://openwsn.atlassian.net/wiki/spaces/OW/pages/688251/State+Machine
   - https://openwsn.atlassian.net/wiki/spaces/OW/pages/688255/Timing+Constants
 */
/* standard slot duration is 10ms but code execution time for most OpenWSN
   supported BOARDS takes longer than 10ms, so use the default 20ms upstream
   slot */
#ifndef SLOTDURATION
#define SLOTDURATION        20        /* in milliseconds */
#endif

#if SLOTDURATION == 20
#ifndef PORT_TsSlotDuration     /* 655 ticks at @32768Hz */
#define PORT_TsSlotDuration     ((SCTIMER_FREQUENCY * SLOTDURATION) / MS_PER_SEC)
#endif

/* Execution speed related parameters */
#ifndef PORT_maxTxDataPrepare
#define PORT_maxTxDataPrepare   (3355 / PORT_US_PER_TICK )  /* ~110 ticks at @32768Hz */
#endif
#ifndef PORT_maxRxAckPrepare
#define PORT_maxRxAckPrepare    (610 / PORT_US_PER_TICK )   /* ~20 ticks at @32768Hz */
#endif
#ifndef PORT_maxRxDataPrepare
#define PORT_maxRxDataPrepare   (1000 / PORT_US_PER_TICK )  /* ~33 ticks at@32768Hz */
#endif
#ifndef PORT_maxTxAckPrepare
#define PORT_maxTxAckPrepare    (1525 / PORT_US_PER_TICK )  /* ~50 ticks at@32768Hz */
#endif

/* Radio speed related parameters */
#ifndef PORT_delayTx
#define PORT_delayTx            (300 / PORT_US_PER_TICK )   /* ~10 ticks at@32768Hz */
#endif
#ifndef PORT_delayRx
#define PORT_delayRx            (0 / PORT_US_PER_TICK )     /* ~0 ticks at@32768Hz */
#endif
#else
#error "Only 20ms slot duration is currently supported"
#endif
/** @} */ /* SLOTDURATION == 20 */

/**
 * @name    Adaptive sync accuracy
 *
 *          Used for synchronization in heterogeneous networks (different BOARDs)
 *          Not supported yet, dummy value needs to be provided.
 * @{
 */
#define SYNC_ACCURACY   (1)    /* ticks */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_INFO_H */
