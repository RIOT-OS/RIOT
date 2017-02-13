Expected result
===============
When successful, you should see 5 different threads printing their PID and
priority. The thread with the lowest priority should be able to lock (and
unlock) the mutex first, followed by the other threads in the order of their
priority (highest next). The output should look like the following:

```
main(): This is RIOT! (Version: xxx)
Mutex order test
Please refer to the README.md for more information

T3 (prio 6): trying to lock mutex now
T4 (prio 4): trying to lock mutex now
T5 (prio 0): trying to lock mutex now
T6 (prio 2): trying to lock mutex now
T7 (prio 1): trying to lock mutex now
T5 (prio 0): unlocking mutex now
T7 (prio 1): unlocking mutex now
T6 (prio 2): unlocking mutex now
T4 (prio 4): unlocking mutex now
T3 (prio 6): unlocking mutex now

Test END, check the order of priorities above.
```

Background
==========
This test application stresses a mutex with a number of threads waiting on it.
