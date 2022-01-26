# Benchmark for `periph/gpio_ll`

This application will generate a square wave on two output pins with a phase
difference of zero between them using both the `periph/gpio` API (as reference)
and the `periph/gpio_ll` API. You can use a logic analyzer or scope to verify
that the square waves are indeed simultaneous (no phase difference) and their
frequency. Note that with the pin based `periph/gpio` API a phase difference is
expected, but not for the port based `periph/gpio_ll` API.

In addition, a timer is used to measure the average frequency over 50,000
square wave periods. The overhead of the loop is estimated and a compensated
frequency (that would be achievable only by unrolling the loop) is calculated.
Both frequencies are printed, in addition to the number of CPU cycles per wave
period. The optimal value is 2 CPU cycles (signal is 1 cycle high and 1 cycle
low).

## Configuration

Configure in the `Makefile` or set via environment variables the number of
the GPIO port to use via the `PORT_OUT` variable. The `PIN_OUT_0` and
`PIN_OUT_1` variables select the pins to use within that GPIO port. If possible,
choose a GPIO port that is fully broken out to pins of your board but left
unconnected. That way you can connect a scope or a logic analyzer to verify
the output.

Note that the test using `gpio_ll_write()` might cause changes to unrelated pins
on the `PORT_OUT` GPIO port, by restoring their value to what it was at the
beginning of the benchmark.

## FAQ

Why are 4 functions calls used for `periph/gpio`, but only 2 for
`periph/gpio_ll`? This isn't fair!

Since in a port based APIs multiple pins can be accessed at once, only two
accesses are needed (one for the high and one for the low part of each square
wave period). In the pin based `periph/gpio` API, two accesses are needed per
pin. This unfair advantage in speed is one of the reasons we want a low level
port based API in RIOT - in addition to a more convenient to use and high level
pin based API.
