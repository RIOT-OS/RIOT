Thread-Duel
============

This is a thread duel application to show RIOTs multi-threading abilities

Every thread will do some work (busy waiting) after the work is done
it counts up by the amount of work it did and then it rests.
There are different resting strategies and these have a huge
influence on thread fairness and scheduling.

resting strategies for the threads of this example are:
- `nice_wait`: does nice breaks giving other threads time to use the CPU
- `bad_wait`: take breaks by busy waiting and therefore hogging the CPU
- `yield_wait`: take no explicit breaks but yields (to higher or equal priority threads)
- `no_wait`: never take a break

After completing a batch of work (and rest) a thread will print information on the work done.
(Printing is done in steps to avoid flooding)

If one thread (all are same priority) does rests `bad_wait`ing or `no_wait`ing at all,
scheduling without round robin will see all CPU time be hogged by that one thread
(or the first one to get it).

In this example Round Robin scheduling is enabled by default,
to disable compile with `RR=0`

change the behaviour of the different thread by adding `CFLAGS='-DTHREAD_1={no_wait,5}'`

e.g.:
```
CFLAGS='-DTHREAD_1={yield_wait,3} -DTHREAD_2={bad_wait,2}' RR=0 make
```
will set thread 1 to be yield_waiting and to 3 works in one batch after that it will yield.
thread 2 will do 2 work bad_wait (hog the cpu while waiting).
the Round Robin scheduling is not activated.
the result will be a CPU hogged by thread 2 which will only do work for 20% of the time (really bad)
(thread 1 will have done 3 work but will never even get the chance to print that)
thread 3 will never have done work.
