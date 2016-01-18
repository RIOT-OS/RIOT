Description
===========

This test measures the difference of two consecutive calls to xtimer_now64() 10k times.
Should the difference be larger then 1000us, the test fails, otherwise it succeeds.
