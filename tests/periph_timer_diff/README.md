# About

This test compares the difference of two timer values t0 and t1 using timer_diff
function and a simple subtraction, i.e., `t1 - t0`. Depending on the platform
and timer configuration both values can differ, for instance when running a
timer in 16-Bit mode/width on a 32-Bit platform.

The test is run for all enabled timers, but only for the first channel.
