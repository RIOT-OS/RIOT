/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file        atomic_arch.h
 * @brief       Architecture dependent interface for an atomic set operation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __ATOMIC_ARCH_H
#define __ATOMIC_ARCH_H

/**
 * @brief Define mappings between arch and internal interfaces
 * @{
 */
#define atomic_set_return       atomic_arch_set_return
/** @} */

/**
 * @brief Set a value atomically without interruption from interrupts etc.
 *
 * @param[out]  to_set      Variable to set
 * @param[in]   value       Value to set to_set to
 * @return The value that was set
 */
unsigned int atomic_arch_set_return(unsigned int *to_set, unsigned int value);

/** @} */
#endif /* __ATOMIC_ARCH_H */
