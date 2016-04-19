Expected result
===============
When successful, you should see 10 different threads printing their PID and
priority. The thread with the lowest priority will print its status first,
followed by the other threads in the order of their priority (highest next). The
output should look like the following:

```
main(): This is RIOT! (Version: xxx)
Mutex order test
Please refer to the README.md for more information

T3 (prio 6): locking mutex now
T4 (prio 0): locking mutex now
T5 (prio 4): locking mutex now
T6 (prio 2): locking mutex now
T7 (prio 1): locking mutex now
T3 (prio 6): unlocking mutex now
T4 (prio 0): unlocking mutex now
T7 (prio 1): unlocking mutex now
T6 (prio 2): unlocking mutex now
T5 (prio 4): unlocking mutex now

Test END, check the order of priorities above.
```

Background
==========
This test application stresses a mutex with a number of threads waiting on it.
