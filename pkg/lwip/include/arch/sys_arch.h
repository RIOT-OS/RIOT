/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_lwip_sys    Porting layer
 * @ingroup     pkg_lwip
 * @brief       System abstraction layer
 * @{
 *
 * @file
 * @brief   OS abstraction layer
 * @see     https://www.nongnu.org/lwip/2_1_x/group__sys__os.html
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "cib.h"
#include "sched.h"
#include "mbox.h"
#include "mutex.h"
#include "random.h"
#include "sema.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System configuration
 */
/* prefer mutexes rather than binary semaphores */
#define LWIP_COMPAT_MUTEX   (0)

/**
 * @name    Critical sections protection definitions
 * @see     https://www.nongnu.org/lwip/2_1_x/group__sys__prot.html
 * @{
 */
#define SYS_ARCH_PROTECT(x)         mutex_lock(&x)
#define SYS_ARCH_UNPROTECT(x)       mutex_unlock(&x)
#define SYS_ARCH_DECL_PROTECT(x)    mutex_t x = MUTEX_INIT
/** @} */

/**
 * @name    Semaphores definitions
 * @see     https://www.nongnu.org/lwip/2_1_x/group__sys__sem.html
 * @{
 */
typedef sema_t sys_sem_t;               /**< Platform specific semaphore type */

static inline bool sys_sem_valid(sys_sem_t *sem)
{
    return sem != NULL;
}

#define sys_sem_valid(sem)              (sys_sem_valid(sem))

#define sys_sem_set_invalid(sem)
/** @} */

/**
 * @name    Mutexes definitions
 * @see     https://www.nongnu.org/lwip/2_1_x/group__sys__mutex.html
 * @{
 */
typedef mutex_t sys_mutex_t;            /**< Platform specific mutex type */

static inline bool sys_mutex_valid(sys_mutex_t *mutex)
{
    return mutex != NULL;
}

#define sys_mutex_valid(mutex)          (sys_mutex_valid(mutex))
#define sys_mutex_set_invalid(mutex)
/** @} */

/**
 * @name    Mailboxes OS abstraction layer definitions
 * @see     https://www.nongnu.org/lwip/2_1_x/group__sys__mbox.html
 * @{
 */
#define SYS_MBOX_SIZE               (8)

/**
 * @brief   Platform specific mailbox type
 */
typedef struct {
    mbox_t mbox;                    /**< RIOT mbox */
    msg_t msgs[SYS_MBOX_SIZE];      /**< queue for the mbox */
} sys_mbox_t;

static inline bool sys_mbox_valid(sys_mbox_t *mbox)
{
    return (mbox != NULL) && (mbox_size(&mbox->mbox) != 0);
}

static inline void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    if (mbox != NULL) {
        mbox_unset(&mbox->mbox);
    }
}

#define sys_mbox_valid(mbox)            (sys_mbox_valid(mbox))
#define sys_mbox_set_invalid(mbox)      (sys_mbox_set_invalid(mbox))
/** @} */

typedef kernel_pid_t sys_thread_t;      /**< Platform specific thread type */

extern kernel_pid_t lwip_tcpip_thread;  /**< PID of the lwIP TCP/IP thread */

/**
 * @name    Functions for locking/unlocking core to assure thread safety.
 * @{
 */
void sys_lock_tcpip_core(void);
#define LOCK_TCPIP_CORE()          sys_lock_tcpip_core()
void sys_unlock_tcpip_core(void);
#define UNLOCK_TCPIP_CORE()        sys_unlock_tcpip_core()
/** @} */

#ifdef MODULE_RANDOM
/**
 * @brief   Use `random_uint32()` to generate random numbers, if available
 */
#define LWIP_RAND()                     (random_uint32())
#endif

#ifdef __cplusplus
}
#endif

/** @} */
