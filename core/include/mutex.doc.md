
@defgroup    core_sync_mutex Mutex
@ingroup     core_sync
@brief       Mutex for thread synchronization

@warning     By default, no mitigation against priority inversion is
             employed. If your application is subject to priority inversion
             and cannot tolerate the additional delay this can cause, use
             module `core_mutex_priority_inheritance` to employ
             priority inheritance as mitigation.

Mutex Implementation Basics
===========================

Data Structures and Encoding
----------------------------

A `mutex_t` contains basically a point, which can have one of the following
values:

1. `NULL`, in case it is unlocked
2. `MUTEX_LOCKED` in case it is locked, but no other thread is waiting on it
3. A pointer to the head of single linked list of threads (or more precisely
   their `thread_t` structures) blocked waiting for obtaining the mutex. This
   list is terminated by `NULL`, not by `MUTEX_LOCKED`

The same information graphically:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Unlocked mutex:
+-------+
| Mutex | --> NULL
+-------+

Locked mutex, no waiters:
+-------+
| Mutex | --> MUTEX_LOCKED
+-------+

Locked mutex, one waiter:
+-------+     +--------+
| Mutex | --> | Waiter | --> NULL
+-------+     +--------+

Locked mutex, 2 waiters:
+-------+     +--------+     +--------+
| Mutex | --> | Waiter | --> | Waiter | --> NULL
+-------+     +--------+     +--------+
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Obtaining a Mutex
-----------------

If a `mutex_lock()` is called, one of the following happens:

1. If the mutex was unlocked (value of `NULL`), its value is changed to
   `MUTEX_LOCKED` and the call to `mutex_lock()` returns right away without
   blocking.
2. If the mutex has a value of `MUTEX_LOCKED`, it will be changed to point to
   the `thread_t` of the running thread. The single item list is terminated
   by setting the `thread_t::rq_entry.next` of the running thread to `NULL`.
   The running thread blocks as described below.
3. Otherwise, the current thread is inserted into the list of waiting
   threads sorted by thread priority. The running thread blocks as described
   below.

In case 2) and 3), the running thread will mark itself as blocked (waiting
for a mutex) and yields. Once control is transferred back to this thread
(which is done in the call to `mutex_unlock()`), it has the mutex and the
function `mutex_lock()` returns.

Returning a Mutex
-----------------

If `mutex_unlock()` is called, one of the following happens:

1. If the mutex was already unlocked (value of `NULL`), the call returns
   without modifying the mutex.
2. If the mutex was locked without waiters (value of `MUTEX_LOCKED`), it is
   unlocked by setting its value to `NULL`.
3. Otherwise the first `thread_t` from the linked list of waiters is removed
   from the list.
    - This thread is the one with the highest priority, as the list is sorted
      by priority.
    - This thread's status is set to pending and its added to the appropriate
      run queue.
    - If that thread was the last item in the list, the mutex is set to
      `MUTEX_LOCK`.
    - The scheduler is run, so that if the unblocked waiting thread can
      run now, in case it has a higher priority than the running thread.

Debugging deadlocks
-------------------

The module `core_mutex_debug` can be used to print on whom `mutex_lock()`
is waiting. This information includes the thread ID of the owner and the
program counter (PC) from where `mutex_lock()` was called. Note that the
information is only valid if:

- The mutex was locked by a thread, and not e.g. by `MUTEX_INIT_LOCKED`
- The function `cpu_get_caller_pc()` is implemented for the target
  architecture. (The thread ID will remain valid, though.)
- The caller PC is briefly 0 when the current owner passes over ownership
  to the next thread, but that thread didn't get CPU time yet to write its
  PC into the data structure. Even worse, on architectures where an aligned
  function-pointer-sized write is not atomic, the value may briefly be
  bogus. Chances are close to zero this ever hits and since this only
  effects debug output, the ostrich algorithm was chosen here.
