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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_RETARGETABLE_LOCKING)

typedef void * _lock_t;

#define _LOCK_T             _lock_t
#define _LOCK_RECURSIVE_T   _lock_t

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

#else /* !defined(_RETARGETABLE_LOCKING) */

#include_next <sys/lock.h>

#endif /* !defined(_RETARGETABLE_LOCKING) */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SYS_LOCK_H */
/** @} */
