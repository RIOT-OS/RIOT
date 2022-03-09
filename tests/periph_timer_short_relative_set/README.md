# Periph Timer Test for short timer_set() values

## About

This test exposes if timer_set() with very low values (down to zero) underflows.
For each value from 100 to 0, it'll try to set a timer with that length.
In human terms, that should trigger instantly (It depends on TEST_TIMER_FREQ.
The default configuration is 1MHz. Some boards configure 32768kHz, which is the
slowest configuration currently. So each line should trigger after at most
31us.).

See this example of a timer_set() implementation:

    int timer_set(tim_t dev, int channel, unsigned int timeout)
    {
        return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
    }

This will probably underflow if "timeout" is 0, or if an ISR interrupts
somewhere between the read and the timerSet_absolute() call.
Depending on the used frequency of the underlying timer and the time it
requires for the involved function calls, reading the timer register, doing the
addition and writing back to the register, this will also fail for higher
timeouts.

For example, as of this writing (30-Oct-19), samr21-xpro fails for values below
8, nrf52dk for values below 2.

## Expected Result

After 100 "interval N ok" messages the test should print "TEST SUCCEEDED".
On failure, the test will print "TEST FAILED".
