# Test Application for `periph_gpio` Without Stdio

## Usage

The test application will configure three pins as input - one floating, one with pull up, and one
with pull down enabled. It will also configure three pins as output. The outputs are driven high or
low to match the reading at the corresponding input pin.

It is suggested that you connect LEDs to the pins configured as output (and add current limiting
resistors to avoid damaging the LEDs or your board). The LED corresponding to the floating input
should flicker (try touching the pin with your finger), go fully of when connected to ground, and
go fully on when connected to high logic level. The LED corresponding to the input with pull up
enabled should be on, unless the input pin is connected to ground. The LED corresponding to the
input with pull down enabled should be off, unless the input pin is connected to high logic level.

If `periph_gpio_irq` is also supported, three more inputs and corresponding outputs are set up and,
by default, have the pull up resistor enabled. The first input is configured to trigger an IRQ on
rising edge, the second on falling edge, the third on both. On each IRQ, the corresponding output
pin is toggled. (Note: No debouncing is done, so LEDs connected to the corresponding outputs
might flicker quite a bit.)

## Motivation

When porting new boards to RIOT it can be useful to be able to test a new `periph_gpio` driver
while no `periph_uart` or `periph_usb` driver is in place. The existing test application relies on
a way to present the user a shell - which is convenient when possible, but not an option until an
transport of stdio is implemented. This test application is aimed to fill that gap.
