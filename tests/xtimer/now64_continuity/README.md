Description
===========

This test measures the difference of two consecutive calls to xtimer_now64()
100k times. If the max or average difference is larger than 1000us the test
fails, otherwise it succeeds.
