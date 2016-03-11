/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file
 * @brief       prototypes for kernel intitialization
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
 * @brief   Initializes scheduler and creates main and idle task
 */
void kernel_init(void);

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INIT_H */
/** @} */
