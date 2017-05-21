/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 *
 * @file            cpu-conf.h
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT  (4096)
#define THREAD_EXTRA_STACKSIZE_PRINTF        (2048)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (1024)
#endif

#define THREAD_STACKSIZE_IDLE  (1024)

#ifdef __cplusplus
}
#endif

#endif
