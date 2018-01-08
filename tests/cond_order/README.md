Expected result
===============
When successful, you should see 5 different threads printing their PID and
priority. The thread with the lowest priority should be able to signaled first,
followed by the other threads in the order of their priority (highest next). If
the main thread holds the lock, however, none of the other threads should be
able to make progress. The output should look like the following:

```
main(): This is RIOT! (Version: 2018.01-devel-1120-g811de-starbeam-feature-condition-variable)
Condition variable order test
Please refer to the README.md for more information

T3 (prio 6): waiting on condition variable now
T4 (prio 4): waiting on condition variable now
T5 (prio 0): waiting on condition variable now
T6 (prio 2): waiting on condition variable now
T7 (prio 1): waiting on condition variable now
First batch was signaled
T5 (prio 0): condition variable was signaled now
T7 (prio 1): condition variable was signaled now
T6 (prio 2): condition variable was signaled now
First batch has woken up
Second batch was signaled
T4 (prio 4): condition variable was signaled now
T3 (prio 6): condition variable was signaled now
Second batch has woken up

Test END, check the order of priorities above.
```

Background
==========
This test application stresses a condition variable with a number of threads
waiting on it. The threads are signaled (awakened) in two batches.
