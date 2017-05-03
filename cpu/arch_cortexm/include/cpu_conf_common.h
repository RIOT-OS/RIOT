/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Common CPU definitione for Cortex-M family based MCUs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CPU_CONF_COMMON_H
#define CPU_CONF_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Configuration of default stack sizes
 *
 * As all members of the Cortex-M family behave identical in terms of stack
 * usage, we define the default stack size values here centrally for all CPU
 * implementations.
 *
 * If needed, you can overwrite these values the the `cpu_conf.h` file of the
 * specific CPU implementation.
 *
 * @todo Adjust values for Cortex-M4F with FPU?
 * @todo Configure second set if no newlib nano.specs are available?
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (512)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (1024)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE           (256)
#endif
/** @} */

/**
 * @brief   Stack size used for the exception (ISR) stack
 * @{
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                   (512U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_COMMON_H */
/** @} */
