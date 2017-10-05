# xTimer hang test

This is a runtime test for the RIOT xtimer implementation it prints text
`Testing... (<percentage>)` (approximately) every `TEST_INTERVAL_MS` ms
(milliseconds) for an overall test duration of `TEST_TIME_S` seconds.

The test will print `[SUCCESS]` on completion, if it stops or hangs before,
the test has failed.

Please note (again), this is a runtime test to check if xtimer runs into a
deadlock, it is not about clock stability nor accuracy of timing intervals.
