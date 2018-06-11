/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_lwip_arch_sys_arch Architecture depentent definitions
 * @ingroup     pkg_lwip
 * @brief       Semaphores and mailboxes.
 * @{
 *
 * @file
 * @brief   Semaphore and mailboxes definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef ARCH_SYS_ARCH_H
#define ARCH_SYS_ARCH_H

#include <stdbool.h>
#include <stdint.h>

#include "cib.h"
#include "kernel_types.h"
#include "mbox.h"
#include "mutex.h"
#include "random.h"
#include "sema.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LWIP_COMPAT_MUTEX   (0)
#define SYS_SEM_NULL        { 0, PRIORITY_QUEUE_INIT }
#define SYS_MBOX_SIZE       (8)

typedef struct {
    mbox_t mbox;
    msg_t msgs[SYS_MBOX_SIZE];
} sys_mbox_t;

typedef mutex_t sys_mutex_t;
typedef sema_t sys_sem_t;
typedef kernel_pid_t sys_thread_t;

static inline bool sys_mutex_valid(sys_mutex_t *mutex)
{
    return mutex != NULL;
}

static inline bool sys_sem_valid(sys_sem_t *sem)
{
    return sem != NULL;
}

static inline bool sys_mbox_valid(sys_mbox_t *mbox)
{
    return (mbox != NULL) && (mbox->mbox.cib.mask != 0);
}

static inline void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    if (mbox != NULL) {
        mbox->mbox.cib.mask = 0;
    }
}

#define sys_mutex_valid(mutex)          (sys_mutex_valid(mutex))
#define sys_mutex_set_invalid(mutex)
#define sys_sem_valid(sem)              (sys_sem_valid(sem))
#define sys_sem_set_invalid(sem)
#define sys_mbox_valid(mbox)            (sys_mbox_valid(mbox))
#define sys_mbox_set_invalid(mbox)      (sys_mbox_set_invalid(mbox))

#ifdef MODULE_RANDOM
#define LWIP_RAND()                     (random_uint32())
#endif

#ifdef __cplusplus
}
#endif

#endif /* ARCH_SYS_ARCH_H */
/** @} */
