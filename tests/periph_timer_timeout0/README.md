# Timer callback stack overflow regression test

This test is designed to catch an implementation bug where a timer callback is
called directly from inside timer_set if the given timeout=0, leading to a
stack overflow if timer_set is called from within the callback of the same
timer.

## Test algorithm

The test will attempt to initialize each timer in the system and set a non-zero
timeout at first. The callback function provided will then attempt to set a new
timeout=0 until we have called the callback TEST_ITERATIONS times (default 10000).
The expected behavior is that the timer will trigger again as soon as the timer
callback function returns. If the timer driver implementation is broken, then
the callback will be called again by timer_set, causing a stack overflow after a
number of iterations.

## Consequences of a failed test

If this test is failing (or crashing), it means that there may be a risk for
stack overflows if using the high level timer subsystems (xtimer, ztimer) in
certain scenarios.
