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

## Expected Failures

Implementations are allowed to not electrically disconnect GPIO pins in state
`GPIO_DISCONNECT`. The test will however test for pins to be electrically
disconnected. For every MCU supported by GPIO LL so far at least some pins can
be electrically disconnected. You might need to change the GPIO pins tested
if the test for disconnected GPIOs being also electrically disconnected.

When features such as `periph_gpio_ll_pull_up` or `periph_gpio_ll_open_drain`
are provided, this test expect those to be available on the tested pins. If
those features are not available on all pins, suitable pins need to be
selected for the test to pass.
