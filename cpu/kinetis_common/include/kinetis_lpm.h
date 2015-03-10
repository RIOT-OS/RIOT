/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_common_lpm Kinetis LPM
 * @ingroup     cpu_kinetis_common
 * @brief       LPM (LLWU) driver.
 *
 * @{
 * @file
 * @brief       Kinetis extensions to RIOT LPM driver.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef KINETIS_LPM_H
#define KINETIS_LPM_H

#include "atomic.h"

#ifdef __cplusplus
extern "C"
{
#endif


/** @brief LLWU wake up edge trigger modes for external pin wake-ups */
typedef enum kinetis_lpm_edge {
    /** Disable external pin wake up */
    KINETIS_LPM_EDGE_DISABLE = 0b00,
    /** Wake on rising edge */
    KINETIS_LPM_EDGE_RISING = 0b01,
    /** Wake on falling edge */
    KINETIS_LPM_EDGE_FALLING = 0b10,
    /** Wake on any logic change */
    KINETIS_LPM_EDGE_ANY = 0b11,
} kinetis_lpm_edge_t;

/** @brief Counting semaphore used for keeping track of the inhibitors for STOP */
extern atomic_int_t kinetis_lpm_inhibit_stop_sema;
/** @brief Counting semaphore used for keeping track of the inhibitors for VLPS */
extern atomic_int_t kinetis_lpm_inhibit_vlps_sema;
/** @brief Counting semaphore used for keeping track of the inhibitors for LLS */
extern atomic_int_t kinetis_lpm_inhibit_lls_sema;

/**
 * @brief Inhibit STOP, and deeper, power modes
 */
static inline void kinetis_lpm_inhibit_stop(void) {
    atomic_inc(&kinetis_lpm_inhibit_stop_sema);
}

/**
 * @brief Inhibit VLPS, and deeper, power modes
 */
static inline void kinetis_lpm_inhibit_vlps(void) {
    atomic_inc(&kinetis_lpm_inhibit_vlps_sema);
}

/**
 * @brief Inhibit LLS, and deeper, power modes
 */
static inline void kinetis_lpm_inhibit_lls(void) {
    atomic_inc(&kinetis_lpm_inhibit_lls_sema);
}

/**
 * @brief Uninhibit STOP, and deeper, power modes
 */
static inline void kinetis_lpm_uninhibit_stop(void) {
    atomic_dec(&kinetis_lpm_inhibit_stop_sema);
}

/**
 * @brief Uninhibit VLPS, and deeper, power modes
 */
static inline void kinetis_lpm_uninhibit_vlps(void) {
    atomic_dec(&kinetis_lpm_inhibit_vlps_sema);
}

/**
 * @brief Uninhibit LLS, and deeper, power modes
 */
static inline void kinetis_lpm_uninhibit_lls(void) {
    atomic_dec(&kinetis_lpm_inhibit_lls_sema);
}

/**
 * @brief Enable LLWU wake-ups from the given module.
 *
 * @param[in]  module  the module number, see the data sheet for more information.
 */
void kinetis_lpm_enable_wakeup_module(const uint8_t module);

/**
 * @brief Disable LLWU wake-ups from the given module.
 *
 * @param[in]  module  the module number, see the data sheet for more information.
 */
void kinetis_lpm_disable_wakeup_module(const uint8_t module);

/**
 * @brief Set external pin LLWU wake-up
 *
 * @param[in]  pin  the pin number, see the data sheet for more information.
 * @param[in]  edge which edge to wake on.
 */
void kinetis_lpm_set_wakeup_pin(const uint8_t pin, const kinetis_lpm_edge_t edge);

#ifdef __cplusplus
}
#endif

#endif /* KINETIS_LPM_H */
/** @} */
