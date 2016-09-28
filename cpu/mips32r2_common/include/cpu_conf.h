/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _CPU_CONF_H_
#define _CPU_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1024)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (4096)
#endif
#ifndef THREAD_STACKSIZE_IDLE

/*
 * EXTRA PRINTF needed when debug logging turned on on code which
 * runs on this thread eg timer debug
 */
#define THREAD_STACKSIZE_IDLE           (1024 + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifdef __cplusplus
}
#endif

#endif
