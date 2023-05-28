# About

This test measures the number of times one thread can set (and wakeup) another
thread using thread_flags(). The result amounts to the number of times the
thread flag was set, which is half the number of context switches incurred in
that time.

This test application intentionally duplicates code with some similar benchmark
applications in order to be able to compare code sizes.
