Thread-Duel
============

This is a thread duel application to show RIOTs abilities to run multiple-threads
concurrently, even if they are neither cooperative nor dividable into different scheduler priorities, by using the optional round-robin scheduler module.

Every thread will do some work (busy waiting).
After the work is done it counts up by the amount of work it did and then it rests.
There are different resting strategies and these have a huge
influence on thread fairness and scheduling.

Resting strategies for the threads of this example are:
- `nice_wait`: does nice breaks giving other threads time to use the CPU
- `bad_wait`: takes breaks by busy waiting and therefore hogging the CPU
- `yield_wait`: takes no explicit breaks but yields (to higher or equal priority threads)
- `no_wait`: never takes a break

After completing a batch of work (and rest) a thread will print information on the work done.
(Printing is done in steps to avoid flooding)

If one thread (all are same priority) follows `bad_wait` or `no_wait` strategy,
scheduling without round robin will see all CPU time be hogged by that one thread
(or the first one to get it).

In this example Round Robin scheduling is enabled by default,
to disable compile with `RR=0`

Change the behaviour of the different threads by adding
`CFLAGS='-DTHREAD_1={<rest_strategy>,<work>}'`

e.g.:
```
CFLAGS='-DTHREAD_1={yield_wait,3} -DTHREAD_2={bad_wait,2}' RR=0 make
```
Will set:
- thread 1 to follow `yield_waiting` strategy and
to complete 3 works in one batch after that, it will yield.
- thread 2 will do 2 work and follow `bad_wait` (hog the cpu while waiting).
the Round Robin scheduling is not activated.

The result will be:
- CPU hogged by thread 2, which will only do work for 20% of the time (really bad)
- thread 1 will have done 3 work but will never even get the chance to print that
- thread 3 will never have done any work.
