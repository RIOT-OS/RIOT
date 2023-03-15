Expected result
===============

When successful, you should see 5 different threads printing their
PID, priority and recursion depth. The thread with the lowest priority
should be able to lock (and unlock) the mutex first, followed by the
other threads in the order of their priority (highest next). If two
threads have the same priority the thread who comes first in the run queue
(in this test this is the one with the lower thread id) should acquire the
lock.

Background
==========
This test application stresses a mutex with a number of threads waiting on it.
