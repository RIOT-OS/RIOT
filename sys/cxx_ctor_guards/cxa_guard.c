/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_cxx_ctor_guards
 * @{
 *
 * @file
 * @brief       Implementation of the C++ ctor guards
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "test_utils/expect.h"
#include "rmutex.h"

#ifdef CXX_CTOR_GUARDS_CUSTOM_TYPE
/* Some architectures (such as ARM) have custom types for __guard in their
 * ABI defined. To support such cases, a custom header can be provided and
 * `CXX_CTOR_GUARDS_CUSTOM_TYPE` can be defined */
#include "cxx_ctor_guards_arch.h"
#else
/* The ABI for"generic" CPUs defines __guard as follows:
 * (see "libstdc++-v3/config/cpu/generic/cxxabi_tweaks.h" in GCC source) */
__extension__ typedef int __guard __attribute__((mode(__DI__)));
/* Above is just a fancy version of `typedef int64_t __guard`. But let's stick
 * with the official type definition */
#endif

#define GUARD_DONE      0x01
#define GUARD_PENDING   0x02

/* A recursive mutex is needed, as the initialization of a static class could
 * require calling ctors of members of that class. With a regular mutex, this
 * would result in a deadlock.
 */
static rmutex_t cxa_rmutex = RMUTEX_INIT;

int __cxa_guard_acquire(__guard *g)
{
    uint8_t *guard = (uint8_t *)g;
    /* Optimize for the case that the instance is already initialized by
     * doing a lock-free check. */
    if (__atomic_load_1(guard, __ATOMIC_SEQ_CST) & GUARD_DONE) {
        return 0;
    }

    rmutex_lock(&cxa_rmutex);
    /* Check again (this time protected by the rmutex) for the instance being
     * already initialized */
    if (*guard & GUARD_DONE) {
        rmutex_unlock(&cxa_rmutex);
        return 0;
    }

    if (*guard & GUARD_PENDING) {
        /* Recursive initialization of the *same* instance --> bug */
        expect(0);
    }

    *guard = GUARD_PENDING;
    return 1;
}

void __cxa_guard_release(__guard *g)
{
    uint8_t *guard = (uint8_t *)g;
    *guard = GUARD_DONE;
    rmutex_unlock(&cxa_rmutex);
}

void __cxa_guard_abort(__guard *g)
{
    uint8_t *guard = (uint8_t *)g;
    *guard = 0;
    rmutex_unlock(&cxa_rmutex);
}
