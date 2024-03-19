# Overview

This test application is a direct translation of xtimer_msg to the ztimer API.
It is meant mostly as a means to do size comparisons, thus tries to be as close
as possible to the original.

One notable change is the option to choose a different ztimer clock.
By default, the test will use ZTIMER_USEC, unless ZTIMER_MSEC is compiled in,
which will be used in that case.
