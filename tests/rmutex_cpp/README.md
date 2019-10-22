Expected result
===============

When successful, you should see 5 different threads printing their
PID, priority and recursion depth. The thread with the lowest priority
should be able to lock (and unlock) the mutex first, followed by the
other threads in the order of their priority (highest next). If two
threads have the same priority the lower thread id should acquire the
lock. The output should look like the following:

```
main(): This is RIOT! (Version: xxx)
Recursive Mutex test
Please refer to the README.md for more information

Recursive Mutex test
Please refer to the README.md for more information

T3 (prio 6, depth 0): trying to lock rmutex now
T4 (prio 4, depth 0): trying to lock rmutex now
T5 (prio 5, depth 0): trying to lock rmutex now
T6 (prio 2, depth 0): trying to lock rmutex now
T7 (prio 3, depth 0): trying to lock rmutex now
main: unlocking recursive mutex
T6 (prio 2, depth 0): locked rmutex now
T6 (prio 2, depth 1): trying to lock rmutex now
T6 (prio 2, depth 1): locked rmutex now
T6 (prio 2, depth 2): trying to lock rmutex now
T6 (prio 2, depth 2): locked rmutex now
T6 (prio 2, depth 3): trying to lock rmutex now
T6 (prio 2, depth 3): locked rmutex now
T6 (prio 2, depth 3): unlocked rmutex
T6 (prio 2, depth 2): unlocked rmutex
T6 (prio 2, depth 1): unlocked rmutex
T6 (prio 2, depth 0): unlocked rmutex
T7 (prio 3, depth 0): locked rmutex now
T7 (prio 3, depth 1): trying to lock rmutex now
T7 (prio 3, depth 1): locked rmutex now
T7 (prio 3, depth 2): trying to lock rmutex now
T7 (prio 3, depth 2): locked rmutex now
T7 (prio 3, depth 3): trying to lock rmutex now
T7 (prio 3, depth 3): locked rmutex now
T7 (prio 3, depth 4): trying to lock rmutex now
T7 (prio 3, depth 4): locked rmutex now
T7 (prio 3, depth 4): unlocked rmutex
T7 (prio 3, depth 3): unlocked rmutex
T7 (prio 3, depth 2): unlocked rmutex
T7 (prio 3, depth 1): unlocked rmutex
T7 (prio 3, depth 0): unlocked rmutex
T4 (prio 4, depth 0): locked rmutex now
T4 (prio 4, depth 1): trying to lock rmutex now
T4 (prio 4, depth 1): locked rmutex now
T4 (prio 4, depth 2): trying to lock rmutex now
T4 (prio 4, depth 2): locked rmutex now
T4 (prio 4, depth 2): unlocked rmutex
T4 (prio 4, depth 1): unlocked rmutex
T4 (prio 4, depth 0): unlocked rmutex
T5 (prio 5, depth 0): locked rmutex now
T5 (prio 5, depth 1): trying to lock rmutex now
T5 (prio 5, depth 1): locked rmutex now
T5 (prio 5, depth 2): trying to lock rmutex now
T5 (prio 5, depth 2): locked rmutex now
T5 (prio 5, depth 2): unlocked rmutex
T5 (prio 5, depth 1): unlocked rmutex
T5 (prio 5, depth 0): unlocked rmutex
T3 (prio 6, depth 0): locked rmutex now
T3 (prio 6, depth 1): trying to lock rmutex now
T3 (prio 6, depth 1): locked rmutex now
T3 (prio 6, depth 2): trying to lock rmutex now
T3 (prio 6, depth 2): locked rmutex now
T3 (prio 6, depth 3): trying to lock rmutex now
T3 (prio 6, depth 3): locked rmutex now
T3 (prio 6, depth 4): trying to lock rmutex now
T3 (prio 6, depth 4): locked rmutex now
T3 (prio 6, depth 4): unlocked rmutex
T3 (prio 6, depth 3): unlocked rmutex
T3 (prio 6, depth 2): unlocked rmutex
T3 (prio 6, depth 1): unlocked rmutex
T3 (prio 6, depth 0): unlocked rmutex

Test END, check the order of priorities above.
```

Background
==========
This test application stresses a mutex with a number of threads waiting on it.
