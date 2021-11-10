Testing for Absence of Interactions between Floating Point Calculations and Context Switches
============================================================================================

This tests launches three threads, t1, t2 and t3 that will perform a long and costly series of
floating point calculations with different input data, while a software timer triggers context
switches. The threads t1 and t3 will print the results. All threads will do this in an endless
loop.

This allows for testing the following:

1. When using the pseudo module `printf_float`, floating point numbers can be correctly printed
2. `THREAD_STACKSIZE_MAIN` is large enough to print floats without stack overflows.
3. Context switches while the (soft) FPU is busy does not result in precision loss.
    - This could happen if the FPU state is not properly saved / restored on context switch. This
      could be needed if the FPU internally uses a higher resolution that `float` / `double`
      (e.g. the x86 FPU uses 80 bits internally, instead of 64 bits for double)
