/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Wrapper for sys/lock.h
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is a wrapper for sys/lock.h to ensure source code compatibility
 * even if retargetable locking is enabled in newlib.
 */

#ifndef SYS_LOCK_H
#define SYS_LOCK_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include_next <sys/lock.h>

#if defined(_RETARGETABLE_LOCKING)

/*
 * The structured lock type has to be large enough to hold a recursive mutex.
 * Since `rmutex.h` cannot be included in this stage of header inclusion,
 * `sizeof` cannot be used here and an array of sufficient size has to be
 * defined. This is ensured by a `static_assert` in `cpu/esp_common/syscalls.c`.
 */
typedef struct __lock {
    void *reserved[2];
} __lock_t;

#endif /* defined(_RETARGETABLE_LOCKING) */

/*
 * Declarations for source code compatibility. These functions were used
 * in earlier newlib versions. In newer newlib versions, either the
 * __lock_* or __retarget_* functions are used. They are still used in
 * ESP-IDF.
 */

typedef _LOCK_T _lock_t;

void _lock_init(_lock_t *lock);
void _lock_init_recursive(_lock_t *lock);
void _lock_close(_lock_t *lock);
void _lock_close_recursive(_lock_t *lock);
void _lock_acquire(_lock_t *lock);
void _lock_acquire_recursive(_lock_t *lock);
int  _lock_try_acquire(_lock_t *lock);
int  _lock_try_acquire_recursive(_lock_t *lock);
void _lock_release(_lock_t *lock);
void _lock_release_recursive(_lock_t *lock);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SYS_LOCK_H */
/** @} */
