/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    debug_irq_disable IRQ Disable Debug helper
 * @ingroup     sys
 * @brief       Debug time spent with IRQ disabled
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef DEBUG_IRQ_DISABLE_H
#define DEBUG_IRQ_DISABLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Threshold (in CPU ticks) below which periods with IRQs
 *        disabled are not printed.
 *
 *        Use this to prevent *a lot* of output when debugging.
 */
#ifndef CONFIG_DEBUG_IRQ_DISABLE_THRESHOLD
#define CONFIG_DEBUG_IRQ_DISABLE_THRESHOLD  (1)
#endif

/**
 * @brief   Print time spent with IRQ disabled
 * @internal
 *
 * @param[in] file  file where irq_restore() was called
 * @param[in] line  line where irq_restore() was called
 * @param[in] ticks CPU ticks spent with IRQ disabled
 */
void debug_irq_disable_print(const char *file, unsigned line, uint32_t ticks);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DEBUG_IRQ_DISABLE_H */
