# xtimer_drift test application

Make note of the PC clock when starting this test. Let it run for a while, and
compare the printed time against the expected time from the PC clock. The
difference is the RIOT timer drift, this is likely caused by either:

- an inaccurate hardware timer, or
- bugs in the software (xtimer or periph/timer)

This test will run a periodic timer every `TEST_INTERVAL` microseconds (`TEST_HZ`).
The current time will be printed once per second, along with the difference
between the actual and expected `xtimer_now` value. The first output variable
`drift`, represents the total offset since start between `xtimer_now` and the
expected time. The second output variable `jitter`, represents the difference
in drift from the last printout. Two other threads are also running only to
cause CPU load with extra interrupts and context switches.
