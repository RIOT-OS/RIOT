# bench_xtimer_load test application

This benchmark will run a periodic timer every `TEST_INTERVAL` microseconds
(`TEST_HZ`). Two other threads are also running only to cause CPU load with
extra interrupts and context switches.

This benchmark is heavily based on tests/xtimer_drift. It basically minimizes
the per-second printout and adds statistic gathering / printing.

With low TEST_HZ (`<64`), this test can expose accuracy issues in the timer
configuration.
With high TEST_HZ (depending on the platform, `64 - 8192`), it exposes issues
with both high timer isr load issues in periph_timer and concurrency
/ race condition issues in xtimer. If it finishes, the resulting values can
give an indication of the timer implementation efficiency.

The difference between the actual and expected `xtimer_now()` value ("drift")
will be calculated, along with the drift change since the last iteration ("jitter").
Every second, a dot (".") will be printed.

After TEST_TIME seconds (defaults to 10), the minimum and  maximum values for
both drift and jitter will be printed, along with the final drift and average
jitter (sum(abs(jitter)) / num_samples).

The default TEST_HZ is quite low (16Hz), which should result in close to zero
drift and jitter.

With higher TEST_HZ, timer isr collisions are more likely, causing increased
jitter.

As long as the CPU can handle the load, the final drift should stay low.
If the CPU can't handle the load, drift will increase with every iteration.


# Notes

This benchmark will currently get stuck due to xtimer bugs on at least native
and every board where xtimer uses a `<32bit` hardware timer.
