# Test for `periph/gpio_ll`

This application will use two output and two input GPIOs, which have to be
connected via a jumper wire. The test will change the value of the two pins
output pins and use the input pins to read the value back in. If the value read
doesn't match the expected result, the test aborts and fails.

If IRQ support is provided, the test will additionally walk through every IRQ
configuration for the first GPIO pin given and iterate over any trigger
condition possible. It will check that edge trigger and (if supported) level
trigger IRQs trigger exactly on the configured triggers.

## Configuration

Configure in the `Makefile` or set via environment variables the number of
the output GPIO port to use via the `PORT_OUT` variable. The `PIN_OUT_0` and
`PIN_OUT_1` variables select the pins to use within that GPIO port. The input
GPIO port is set via `PORT_IN`. Both `PORT_IN == PORT_OUT` and
`PORT_IN != PORT_OUT` is valid. The input pin number within `PORT_IN` are set
via `PIN_IN_0` and `PIN_IN_1`. `PIN_IN_0` has to be wired to `PIN_OUT_0`, and
`PIN_IN_1` to `PIN_OUT_1`.
