# Overview

This test application is a direct translation of ztimer_msg to the ztimer64API.
It is meant mostly as a means to do size comparisons, thus tries to be as close
as possible to the original.

One notable change is the option to choose a different ztimer clock.
By default, the test will use ZTIMER64_USEC, unless ZTIMER64_MSEC is compiled in,
which will be used in that case.
