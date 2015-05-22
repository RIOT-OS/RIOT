/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file
 * @brief       Architecture dependent interface for an atomic set operation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ATOMIC_ARCH_H
#define ATOMIC_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Define mappings between arch and internal interfaces
 *
 * This mapping is done for compatibility of existing platforms,
 * new platforms should always use the *_arch_* interfaces.
 * @{
 */
#ifdef COREIF_NG
#define atomic_set_return       atomic_arch_set_return
#endif
/** @} */

/**
 * @brief Set a value atomically without interruption from interrupts etc.
 *
 * @param[out]  to_set      variable to set
 * @param[in]   value       value to set to_set to
 *
 * @return                  the value that was set
 */
unsigned int atomic_arch_set_return(unsigned int *to_set, unsigned int value);

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_ARCH_H */
/** @} */
