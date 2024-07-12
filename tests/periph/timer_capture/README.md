Test for `periph_timer_capture` implementations
===============================================

This test app depends on a jumper wire that connects a GPIO output (the
"trigger pin") to a GPIO that can be used as an external trigger for a timer
capture channel. The test will configure the capture in all possible modes and
cause trigger events. It will then check if the time is captured, the registered
interrupt handler is executed, etc.

Check the `Makefile` for the variables to set. You can save a per-board
configuration in a `Makefile.<BOARDNAME>` that will be included, if existing.
