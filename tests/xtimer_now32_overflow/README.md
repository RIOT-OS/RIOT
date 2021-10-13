Description
===========

This is a regression test for when `xtimer_now64()` becomes greater than
`UINT32_MAX` and the internal state machine fails to update the timers state
correctly.
