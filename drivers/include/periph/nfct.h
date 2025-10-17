/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_nfct NFCT
 * @ingroup     drivers_periph
 * @brief       NFC Tag peripheral driver interface
 *
 * TODO Description
 *
 *
 * @{
 *
 * @file
 * @brief       NFC Tag peripheral driver interface definitions
 *
 * @author      Nico Behrens <nico.behrens@mailbox.tu-dresden.de>
 */

#ifndef PERIPH_NFCT_H
#define PERIPH_NFCT_H

#include <limits.h>
#include <stdint.h>

#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STATE_DISABLE,
    STATE_SENSE_FIELD,
    STATE_IDLERU,
    STATE_ACTIVE_A,
    STATE_RECEIVE,
    STATE_TRANSMIT,
} nfct_state_t;

static inline void activate(void);

static inline void disable(void);


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_NFCT_H */
/** @} */
