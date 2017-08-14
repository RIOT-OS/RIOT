# test application for xtimer_msg_receive_timeout()

This test will sequentially start TEST_COUNT xtimers to send a IPC msg,
alternating with an interval of TEST_PERIOD +/- 10%. Every time a timer
was set, it will wait for a message for at most TEST_PERIOD microseconds.
This should succeed with a message or fail with timeout in an alternating
manner.

Default values are TEST_COUNT = 10, and TEST_PERIOD = 100ms = 100000us.
