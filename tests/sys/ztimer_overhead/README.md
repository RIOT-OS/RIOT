# Introduction

This test application sets up a ztimer_periph at 1MHz, then measures 1024
times how much overhead ztimer adds.

It uses the "ztimer_overhead_set()" function. See it's documentation for more
information.

It then sets `adjust_set` parameter and sleeps 1024 times and measure how
much overhead ztimer_sleep adds.

It uses the "ztimer_overhead_sleep()" function. See it's documentation for more
information.

At the end of the test `adjust_set` and `adjust_sleep` values are printed
that can be set for the target `BOARD` in `board.h`.

e.g for dwm1001:

```shell
ZTIMER_USEC adjust params for dwm1001:
    CONFIG_ZTIMER_USEC_ADJUST_SET    6
    CONFIG_ZTIMER_USEC_ADJUST_SLEEP  21
```

This test application also makes use of the `ztimer_auto_adjust` pseudomodule
and thereby allows for comparison of the offset values calculated during
auto-initialization and later during this test.
