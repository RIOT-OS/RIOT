/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        atomic.h
 * @brief       Atomic getter and setter functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "arch/atomic_arch.h"

/**
 * @brief Sets a new and returns the old value of a variable atomically
 *
 * @param[in] val   The variable to be set
 * @param[in] set   The value to be written
 *
 * @return The old value of *val*
 */
unsigned int atomic_set_return(unsigned int *val, unsigned int set);

#endif /* _ATOMIC_H */
/** @} */
