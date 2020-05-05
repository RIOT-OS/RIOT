# README for xtimer_now_irq test

This test checks, if the timer returns the correct time (xtimer_now_usec() is called), when interrupts are disabled. Specifically tested is if the time is correct after a low-level timer overflow.

When the returned value is incorrect the test will print: "ERROR: wrong time with interrupts disabled".

This test must be run over at least one full timer period. Meaning the test only finishes in reasonable short time on platforms with small timers up to 24 bit.
This test does not work for 32 bit timers.
