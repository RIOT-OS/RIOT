/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       Functions for atomic handling of variables
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Integer variable for use in atomic counters.
 */
typedef struct atomic_int {
    volatile int value;
} atomic_int_t;

/**
 * @brief Initializer for atomic variables
 *
 * @param[in] val  initial value for the atomic integer
 */
#define ATOMIC_INIT(val) {(val)}

/**
 * @brief Atomic Compare and Swap
 *
 * Updates the value in var iff the current value of var is equal to old.
 *
 * @param[inout] var  Atomic variable to update
 * @param[in]    old  The old value to compare against
 * @param[in]    now  The new value to write to var
 *
 * @return 1 if the write completed successfully
 * @return 0 if the write failed.
 */
int atomic_cas(atomic_int_t *var, int old, int now);

/**
 * @brief Increment a counter variable by one atomically and return the old value.
 *
 * @param[inout]  var   Pointer to a counter variable.
 *
 * @return The value of *val* before the increment.
 */
static inline int atomic_inc(atomic_int_t *var)
{
    int old;

    do {
        old = var->value;
    } while (!atomic_cas(var, old, old + 1));

    return old;
}

/**
 * @brief Decrement a counter variable by one atomically and return the old value.
 *
 * @param[inout]  var   Pointer to a counter variable.
 *
 * @return The value of *val* before the decrement.
 */
static inline int atomic_dec(atomic_int_t *var)
{
    int old;

    do {
        old = var->value;
    } while (!atomic_cas(var, old, old - 1));

    return old;
}

/**
 * @brief Set an atomic variable to 1.
 *
 * @param[inout]  var   Pointer to an atomic variable to update
 *
 * @return 1 if the old value was 0 and the variable was successfully updated
 * @return 0 if the variable was already set
 */
static inline int atomic_set_to_one(atomic_int_t *var)
{
    do {
        if (var->value != 0) {
            return 0;
        }
    } while (!atomic_cas(var, 0, 1));

    return 1;
}

/**
 * @brief Set an atomic variable to 0.
 *
 * @param[inout]  var   Pointer to an atomic variable to update
 *
 * @return 1 if the old value was not 0 and the variable was successfully updated
 * @return 0 if the variable was already cleared
 */
static inline int atomic_set_to_zero(atomic_int_t *var)
{
    int old;

    do {
        old = var->value;

        if (old == 0) {
            return 0;
        }
    } while (!atomic_cas(var, old, 0));

    return 1;
}

/**
 * @brief Get the value of an atomic int
 *
 * @param[in]  var   Atomic variable
 *
 * @return the value of the atomic integer
 *
 * @note This can be used for non-thread-safe assignment of the atomic integer
 */
#define ATOMIC_VALUE(var) ((var).value)

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_H_ */
/** @} */
