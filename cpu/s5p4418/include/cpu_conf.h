/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_S5P4418 Jellyfish
 * @brief           S5P4418 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Dang Minh Phuong <kamejoko80@yahoo.com>
*/

#ifndef S5P4418_CPU_CONF_H
#define S5P4418_CPU_CONF_H

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (4096)
#endif

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF (4096)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE (4096)
#endif

#ifdef __cplusplus
}
#endif

#endif /* S5P4418_CPU_CONF_H */
/** @} */
