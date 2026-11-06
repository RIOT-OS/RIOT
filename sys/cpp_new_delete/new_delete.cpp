/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 Arduino.  All right reserved.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <stdlib.h>

#include "compiler_hints.h"

#ifndef __EXCEPTIONS
#  include "panic.h"
#else
#  include <new>
#endif

namespace
{
void *_new_helper(size_t size)
{
    if (size == 0) {
        /* C++ requires new to return unique ptrs. If we allocate at least 1
         * byte, we can do so */
        size = 1;
    }
    void *result = malloc(size);
    if (unlikely(result == NULL)) {
#ifdef __EXCEPTIONS
        throw std::bad_alloc();
#else
        core_panic(PANIC_MEM_MANAGE, "C++ new alloc failure");
#endif
    }

    return result;
}
} /* end anonymous namespace */

__attribute__((weak))
void *operator new(size_t size)
{
    return _new_helper(size);
}

__attribute__((weak))
void *operator new[](size_t size)
{
    return _new_helper(size);
}

__attribute__((weak))
void operator delete(void *ptr) noexcept
{
    free(ptr);
}

__attribute__((weak))
void operator delete(void *ptr, size_t unused) noexcept
{
    (void)unused;
    free(ptr);
}

__attribute__((weak))
void operator delete[](void *ptr) noexcept
{
    free(ptr);
}

__attribute__((weak))
void operator delete[](void *ptr, size_t unused) noexcept
{
    (void)unused;
    free(ptr);
}
