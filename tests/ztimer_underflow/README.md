# Introduction

This test application underflows ztimer clocks by setting a time offset of zero.
The tests succeeds if the board running the test does not get stuck.


## ZTIMER_USEC

Set the environment variable `TEST_ZTIMER_CLOCK=ZTIMER_USEC`. If the test gets
stuck, increase `CONFIG_ZTIMER_USEC_MIN`.


## ZTIMER_MSEC

Set the environment variable `TEST_ZTIMER_CLOCK=ZTIMER_MSEC`. If the test gets
stuck, increase `RTT_MIN_OFFSET`.
