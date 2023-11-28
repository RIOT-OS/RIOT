# Peripheral Test Battery using the Peripheral Selftest Shield

@warning Before you power your board, make sure the shield's VCC voltage
         selector is matching the logic level of your board. E.g. having the
         VCC selector at 5 V and testing on a 3.3 V logic board with pins that
         are not 5 V tolerant, you might damage your board permanently.

## Quick Start

0. For the MCU family you want to test, find a compatible board
    - the board needs to be mechanically and electrically compatible with
      Arduino UNO shields
    - The Arduino GPIO pin mapping and ideally mapping of SPI, I2C, ADC, and
      PWM needs to be provided
1. Unplug your board (so that it is not powered)
2. Connect the shield
3. Move the VCC selector to match the logic level of the board.
    - If unsure, go for 3.3 V
    - The nRF52840DK works fine with 3.3 V, despite the logic level being only
      3.0 V
4. Close the loops for the peripherals you want to test be moving the DIP
   switches into the `ON` position, leave the untested open by moving it in the
   `OFF` position
    - Start by enabling all but UART
    - If the UART test is actually run and fails, also enable the UART switch
    - (Background: If the UART at D0 and D1 is used for stdio, it cannot be
       looped and tested)
5. Flash and run the test
    - In this directory, run `make BOARD=<YOUR_BOARD> flash test-with-config`

## Details

This test application does a full self test of any of the following peripheral
driver, if supported by the board and Arduino I/O mapping is available:

- `periph_adc` (1)
- `periph_gpio`
- `periph_gpio_irq`
- `periph_i2c` (2)
- `periph_pwm`
- `periph_spi` (3)
- `periph_uart` (4)

Notes:
1. The ADC input is generated via a 4-bit R-2R resistor ladder and the I2C
   attached GPIO extender and/or PWM. If neither is available, no tests are
   performed. If both are available, up to three ADC channels are tested:
   Those muxed to pins A0, A1, and A2.
2. I2C is indirectly tested by operating the PCF8574 I2C GPIO extender and
   observing the result using internal GPIOs connected to the external ones.
   This test is only enabled if both `periph_i2c` and `periph_gpio` is
   available
3. Correctness of the bit order and the clock phase is not checked due to the
   limitations of the loopback mode testing approach. The clock polarity is
   validated (at idle level), the correctness of the clock frequency is roughly
   tested when `periph_timer` is implemented.
4.  UART self testing is skipped when the D0/D1 UART interface is used for
    stdio. Correctness of the symbol rate (often incorrectly referred to as
    "baudrate") is only checked for plausibility when `periph_timer` is
    available. Checking different bits per character, number of stop bits,
    parity bit etc. is possible using `periph_timer`, but not yet implemented
    here.

## Debugging Failures

### Increase Verbosity

Compile with `make DETAILED_OUTPUT=1` to increase the verbosity of the output
at the cost of increased ROM. When you start seeing link failures due to the
ROM cost, you could disable some of the succeeding tests by blacklisting the
peripheral features unaffected. However, be aware some peripherals are tested
with the help of others; e.g. disabling the ADC will prevent testing the
duty cycle of PWM outputs for correctness.

### Early Critical Failure

If the application fails before performing any tests, e.g. the output looks
similar to this:

```
START
main(): This is RIOT! (Version: 2023.10-devel-348-gf1c68-peripheral-selftest)
self-testing peripheral drivers
===============================
CRITICAL FAILURE in tests/periph/selftest_shield/main.c:<line-number>
```

This will indicate a failure to initialize the GPIO extender. Possible reasons
are:

1. Wrong I2C configuration
    - Is the I2C bus (typically in the `periph_conf.h` or in a file included by
      `periph_conf.h`) correctly configured? Is it connected to the Arduino UNO
      pins D14 / D15?
    - Is the `ARDUINO_I2C_UNO` macro correctly defined to the index of the
      I2C bus connected Arduino UNO pins D14 / D15
2. Missing pull-up resistor
    - The shield has no pull up resistors on the I2C bus
    - All RIOT I2C drivers will enable the internal MCU pull up resistors for
      the I2C buses by default. But some MCUs do not have internal pull up
      resistors (on some pins) or may not be able to enable them when used in
      I2C mode.
    - If possible, the I2C driver / I2C board configuration in `periph_conf.h`
      should be configured so that robust operation works out of the box without
      having to add external pull ups.
    - If the board depends on external pulls ups (no suitable internal pull ups
      can be used and the board has no external pull ups on D15 / D15), pull
      ups need to be added to the board. The easiest is to just plug in an
      I2C sensor/EEPROM breakout into the I2C pin socket at J2, as those
      breakout boards typically have external pull up resistors integrated.
3. Bug in the I2C driver
    - A bug in the I2C driver could cause this
    - A logic analyzer connected to the I2C Header (J3), the I2C Socket (J2),
      or the I2C Groove Connector (J1) will come in handy
4. Hardware issue
    - This is the least likely cause :wink:

### Repeated Instances of the Same Error

Many tests are performed in a loop. E.g. some tests are repeated for
`flaky_test_repetitions` (by default `100`), as some tests may pass by chance.
Some tests also iterate over different configurations (such as clock speeds)
and repeat the same tests for each configuration.

To reduce the noise, you can pass `STOP_ON_FAILURE=1` as environment
variable or as parameter to `make` on compilation to abort the test run on the
first failing test.

## Configuration

The test requires no configuration and will by default test all of the above
peripheral drivers for which Arduino I/O mappings exists. To disable a subset
of the tests (e.g. to safe RAM/ROM to trim down the test to a low end board), it
is possible to disable features (`FEATURES_BLACKLIST += periph_<foobar>`).
