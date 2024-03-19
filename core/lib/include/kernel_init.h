/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       prototypes for kernel initialization
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef KERNEL_INIT_H
#define KERNEL_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup core_init_config Core initialization compile configuration
 * @ingroup config
 * @{
 */
#ifdef DOXYGEN

/**
 * @brief Enable this to disable printing a message on bootup.
 */
#define CONFIG_SKIP_BOOT_MSG

/**
 * @brief The message printed by RIOT before calling the main() function, when
 *        @ref CONFIG_SKIP_BOOT_MSG is not set.
 */
#define CONFIG_BOOT_MSG_STRING

#endif /* DOXYGEN */
/** @} */

/**
 * @brief   Initializes scheduler and creates main and idle task
 */
void kernel_init(void);

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

/**
 * @brief   Initialize debug LEDs and stdio
 */
#ifdef MODULE_CORE_INIT
void early_init(void);
#else
static inline void early_init(void) {}
#endif

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INIT_H */
/** @} */
