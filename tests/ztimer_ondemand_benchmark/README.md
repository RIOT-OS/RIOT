ztimer_ondemand_benchmark
=========================

This application measures latencies introduced by `ztimer_acquire()` and
`ztimer_release()`. Since it is directly interacting with the underlying
peripheral, you need at least two `periph_timer` instances if you're
benchmarking a ztimer clock based on `ztimer_periph_timer`.

The application tries to select a unused timer automatically. If this selection
process fails, users may specify device and frequency in this test's Makefile.
