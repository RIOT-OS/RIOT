Round Robing Scheduling Test
========================

This application is a simple test case for round-robin scheduling.
Two threads are started with the same priority.
The first thread is a busy loop and is started first.
The second thread unlocks a mutex allowing the main thread to continue and exit.

Without Round Robin scheduling the busy loop thread would run indefinitely,
with round-robin in eventually getting de-scheduled allowing the main thread to run and exit.

Usage
=====

By default `sched_round_robin` is included:

`make tests/sys_sched_round_robin flash term`

```
...Board Initialisation...

Help: Press s to start test, r to print it is ready
s
START
main(): This is RIOT! (Version: ...)
starting threads
double locking mutex
mutex_thread yield
bad thread looping
unlock mutex
[SUCCESS]

```

It can be excluded from the build by setting the command-line argument `NORR=1`:


`NORR=1 make tests/sys_sched_round_robin flash term`

```
...Board Initialisation...

Help: Press s to start test, r to print it is ready
s
START
main(): This is RIOT! (Version: ...)
starting threads
double locking mutex
mutex_thread yield
bad thread looping
```

This will loop endlessly as the bad thread does not release the CPU,
`make test` will timeout in that case.
