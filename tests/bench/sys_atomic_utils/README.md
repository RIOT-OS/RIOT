# Benchmark for `sys/atomic_utils`

This application will perform 100.000 repetitions (or 1.000.000 on
Cortex-M7 and ESP32) for each atomic operation and will print the total time it
took in a table. For comparison, the speed of C11 atomics and plain `volatile`
accesses are also printed.

## Expectations

Lower is better!

Plain `volatile` accesses are not atomic, and therefore should perform faster
than actual atomic operations. If atomic operations turn out to be faster
(by more than rounding errors and noise), something is odd.

The `atomic_utils` aim to be at least as fast as C11 atomics and often faster.
If the `atomic_utils` implementation performs slower than C11 atomics, you have
found potential for further optimization.
