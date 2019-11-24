# thread_priority_inversion test application

This application uses three threads for demonstrating the
priority inversion problem. In theory, the highest priority thread (**t_high**)
should be scheduled periodically and produce some output:
```
2017-07-17 17:00:29,337 - INFO # t_high: got resource.
...
2017-07-17 17:00:30,343 - INFO # t_high: freeing resource...
```
During this phase of 1s, **t_high** lockes the mutex **res_mtx** which
represents a shared resource. After unlocking the mutex, **t_high** waits 1s
before restaring its cycle again.

A low priority thread **t_low** is doing the same. Since both threads sharing
the same resource **res_mtx**, they have to wait for each other until the mutex
is unlocked. On startup, **t_low** starts immediately, while **t_high** waits
0.5s before so that **t_low** allocates the resource first. Together, the output
looks like this:
```
2017-07-17 17:00:28,339 - INFO # t_low: allocating resource...
2017-07-17 17:00:28,339 - INFO # t_low: got resource.
2017-07-17 17:00:28,340 - INFO # t_high: allocating resource...
2017-07-17 17:00:29,337 - INFO # t_low: freeing resource...
2017-07-17 17:00:29,337 - INFO # t_high: got resource.
2017-07-17 17:00:29,338 - INFO # t_low: freed resource.
2017-07-17 17:00:30,343 - INFO # t_high: freeing resource...
2017-07-17 17:00:30,344 - INFO # t_high: freed resource.
2017-07-17 17:00:30,345 - INFO # t_low: allocating resource...
2017-07-17 17:00:30,346 - INFO # t_low: got resource.
...
```

After 3s, a third thread with medium priority (**t_mid**) is started. This
thread does not touch **res_mtx**, but it runs an infinite loop without leaving
some CPU time to lower priority tasks. This prevents **t_low** from freeing the
resource and thus, **t_high** from running (**Priority Inversion**). In this
situation, the test program output stops with the following lines:
```
2017-07-17 17:00:31,335 - INFO # t_mid: doing some stupid stuff...
2017-07-17 17:00:31,340 - INFO # t_high: allocating resource...
```

If the scheduler contains a mechanism for handling this problem, the program
should continue with output from **t_high**.
