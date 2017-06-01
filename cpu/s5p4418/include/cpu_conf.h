/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef S5P4418_CPU_CONF_H
#define S5P4418_CPU_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (4096)
#endif

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF (4096)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE (4096)
#endif

/**
 * @brief   Stack size used for the exception (IRQ) stack
 * @{
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                   (0x1000U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif
