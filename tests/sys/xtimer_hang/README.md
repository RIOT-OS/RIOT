# xTimer hang test

This is a runtime test for the RIOT xtimer implementation it prints text
`Testing... (<percentage>)` (approximately) every `TEST_INTERVAL_MS` ms
(milliseconds) for an overall test duration of `TEST_TIME_S` seconds.

The test will print `[SUCCESS]` on completion, if it stops or hangs before,
the test has failed.

Please note (again), this is a runtime test to check if xtimer runs into a
deadlock, it is not about clock stability nor accuracy of timing intervals.

When debug pins are used and observed the expected output is as follows:
The MAIN_THREAD_PIN is on for ca. 100ms and that in a regular interval. If this interval is not regular or has gaps this
is an error. The WORKER_THREAD_PIN should toggle after 1ms and 1.1ms. (As this might fall in the XTIMER_ISR_BACKOFF the
first pin toggle is delayed until the second is ready. Thus the time interval can be longer.)
If the Timer fall in the same interrupt there might be a interrupt before the second worker thread timer is set.
This leads to a separation of the timer interrupts until they again fall in the same interrupt.
It might happen that from the separation of the interrupts till the merge there is only a uneven count of
WORKER_THREAD_PIN toggles, which means a loss of one worker time 2, which is expected as it has a lower priority then
worker timer 1. And thus in the moment when the hardware interrupt for the 2 worker is executet it has to wait for the
1 worker timer because of the XTIMER_ISR_BACKOFF and so the first timer is executed first as it has the higher priority.