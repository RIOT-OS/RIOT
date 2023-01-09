/**
@defgroup   sys_malloc_ts           Thread-safe wrappers for malloc and friends
@ingroup    sys
@brief      This module provides wrappers for malloc, calloc, realloc and free
            that provide mutually exclusive access to those functions.
@warning    This module is automatically selected, if needed. Never add it
            manually.

# Background

Without support of the OS (or resorting to disabling IRQs), the standard C
library is unable to guarantee that at most one thread at a time accesses the
heap management data structures. Some C libraries provide hooks for locking
(e.g. picolibc and newlib do so optionally), others (e.g. AVR libc) don't.
By providing wrapper functions for `malloc()` and friends and instructing the
linker to link to those instead of their actual implementations, we can provide
thread safe access to the heap regardless of C libraries support.


# Usage

This module is intended to be use by platforms not providing the required
locking with other means automatically. Hence, application developers and users
should never select this module by hand.

 */
