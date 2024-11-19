IO-Mapping and Shields                                                {#iomaps}
======================

[TOC]

Introduction                                                    {#iomaps-intro}
============

Arduino has popularized the concept of adding an abstraction layer over the
MCU GPIOs and ADC multiplexer as "Digital Pins" and "Analog Pins". For historic
reasons as well as to express appreciation for the introduction of concept, we
prefix features and macros that map board pins to MCU pins with `ARDUINO`.

@note   This means, we also explicitly use the Arduino feature for boards that
        are neither official Arduino products nor compatible.

I/O Mappings                                                  {#iomaps-mapping}
============

All I/O mappings are provided by the `arduino_iomap.h` header file. This is
provided either by the board directly (e.g. in
`<RIOT git repo>/board/<board>/include/arduino_iomap.h`) or by the board
family (e.g. in `board/common/<board family>/include/arduino_iomap.h`).

To add support to new boards, start by copy-pasting the `arduino_iomap.h` from
an existing board and adapt/extend the macros as needed. Finally, add the
features to the `Makefile.features` and `KConfig` files of the board that
indicate the presence of the added mappings.

Digital Pins                                             {#iomaps-mapping-gpio}
------------

The feature `arduino_pins` is provided by boards that do have a mapping to
digital pins. The GPIO for e.g. `D3` is provided as `ARDUINO_PIN_3`. The
number of the highest digital pin is provided as `ARDUINO_PIN_LAST`. E.g. if
`ARDUINO_PIN_LAST` is 42, digital pins `D0` to `D42` are typically provided.

@warning    Some boards do have "gaps" in the pin mapping. It could be that
            e.g. `ARDUINO_PIN_5` is defined, but `ARDUINO_PIN_4` is not.

@note       Often aliases are provided for analog pins in digital mode. E.g.
            to use the pin `A3` as digital pin, `ARDUINO_PIN_A3` can be used
            to avoid looking up what digital number that pin has.

Analog Pins                                               {#iomaps-mapping-adc}
-----------

The feature `arduino_analog` is provided by boards that do have a mapping of
ADC lines to analog pins. E.g. the ADC line corresponding to pin `A5` would
be `ARDUINO_A5`. The number of the highest analog pin is provided as
`ARDUINO_ANALOG_PIN_LAST`. The macro `ARDUINO_ANALOG_PIN_LAST` is defined if
and only if a mapping is provided (so it can be used to test for this feature).

@warning    Some boards do have "gaps" in the analog pin mapping. It could be
            that e.g. `ARDUINO_A3` is defined, but `ARDUINO_A2` is not.

DAC Pins                                                  {#iomaps-mapping-dac}
--------

The feature `arduino_dac` is provided by boards that do have a mapping of
DAC pins to DAC lines. E.g. the DAC line for the pin DAC1 would be
`ARDUINO_DAC1`. The number of the highest DAC pin is provided as
`ARDUINO_DAC_PIN_LAST`. The macro `ARDUINO_DAC_PIN_LAST` is defined if
and only if a mapping is provided (so it can be used to test for this feature).

@warning    Some boards do have "gaps" in the analog pin mapping. It could be
            that e.g. `ARDUINO_DAC4` is defined, but `ARDUINO_DAC3` is not.

PWM Pins                                                  {#iomaps-mapping-pwm}
--------

The feature `arduino_pwm` is provided by boards that do have a mapping of
digital pins to PWM settings. E.g. the PWM device connected to the digital pin
`D11` would be `ARDUINO_PIN_11_PWM_DEV` and the channel would be
`ARDUINO_PIN_11_PWM_CHAN`. A PWM frequency for all PWM pins is defined as
`ARDUINO_PWM_FREQU`.

@warning    Typically only few digital pins support PWM. For pins without PWM
            output no `ARDUINO_PIN_<NUM>_PWM_DEV` macro and no
            `ARDUINO_PIN_<NUM>_PWM_DEV` is defined.

UART Device                                              {#iomaps-mapping-uart}
-----------

The feature `arduino_uart` is provided by boards that do provide an UART device
mapping. For the official Arduino boards and compatible boards, the
`ARDUINO_UART_D0D1` macro refers to the UART device that uses the digital pins
`D0` and `D1`.

Please extend the table below to keep naming among boards of the same form
factor consistent:

| Form Factor      | Macro Name            | Description                       |
|:---------------- |:--------------------- |:--------------------------------- |
| Arduino Nano     | `ARDUINO_UART_D0D1`   | UART on D0 (RXD) and D1 (TXD)     |
| Arduino Uno      | `ARDUINO_UART_D0D1`   | UART on D0 (RXD) and D1 (TXD)     |
| Arduino Mega     | `ARDUINO_UART_D0D1`   | UART on D0 (RXD) and D1 (TXD)     |
| Seeedstudio XIAO | `ARDUINO_UART_DEV`    | UART on D7 (RXD) and D6 (TXD)     |

I²C Buses                                                 {#iomaps-mapping-i2c}
---------

The feature `arduino_i2c` is provided by boards that do provide an I²C bus
mapping.

| Form Factor      | Macro Name            | Description                       |
|:---------------- |:--------------------- |:--------------------------------- |
| Arduino Nano     | `ARDUINO_I2C_NANO`    | D18 (SDA) / D19 (SCL)             |
| Arduino Uno      | `ARDUINO_I2C_UNO`     | D18 (SDA) / D19 (SCL)             |
| Arduino Mega     | `ARDUINO_I2C_UNO`     | D20 (SDA) / D21 (SCL)             |
| Arduino Zero     | `ARDUINO_I2C_UNO`     | D20 (SDA) / D21 (SCL)             |
| Arduino Due      | `ARDUINO_I2C_UNO`     | D70 (SDA) / D71 (SCL)             |
| Seeedstudio XIAO | `ARDUINO_I2C_DEV`     | D4 (SDA) / D5 (SCL)               |

The `ARDUINO_I2C_UNO` refers to the I²C bus next to the AREF pin (the topmost
pins on header on the top right) of an Arduino Uno compatible board, e.g.
such as the Arduino Mega2560. Even though the Arduino UNO, the Arduino MEGA2560,
the Arduino Zero and the Arduino Zero all having the I²C bus at the exact same
mechanical positions, the digital pin number of the I²C bus next to the AREF
differs between the versions.

SPI Buses                                                 {#iomaps-mapping-spi}
---------

The feature `arduino_spi` is provided by boards that do provide an SPI bus
mapping.

| Form Factor      | Macro Name                | Description                           |
|:---------------- |:------------------------- |:------------------------------------- |
| Arduino Nano     | `ARDUINO_SPI_ISP`         | The SPI on the ISP header             |
| Arduino Nano     | `ARDUINO_SPI_D11D12D13`   | D11 (MOSI) / D12 (MISO) / D13 (SCK)   |
| Arduino Uno      | `ARDUINO_SPI_ISP`         | The SPI on the ISP header             |
| Arduino Uno      | `ARDUINO_SPI_D11D12D13`   | D11 (MOSI) / D12 (MISO) / D13 (SCK)   |
| Arduino Mega     | `ARDUINO_SPI_ISP`         | The SPI on the ISP header             |
| Arduino Mega     | `ARDUINO_SPI_D11D12D13`   | D11 (MOSI) / D12 (MISO) / D13 (SCK)   |
| Seeedstudio XIAO | `ARDUINO_SPI_DEV`         | D10 (MOSI) / D9 (MISO) / D8 (SCK)     |

@note   The original AVR based Arduinos only have a single hard SPI bus which
        is only provided via the ISP header. Many modern Arduino compatible
        boards do not provide the ISP header and only have SPI on D11/D12/D13
        provided.

Mechanical and Electrical Compatibility                       {#iomaps-shields}
=======================================

Modules implementing drivers for extension boards, a.k.a. shields, can express
their mechanical and electrical requirements by depending on `arduino_shield_...`
features. The following list of features currently exists:

| Feature Name          | Compatibility Claim                                           |
|:--------------------- |:------------------------------------------------------------- |
| `arduino_shield_nano` | Board has side headers compatible with the Arduino Nano       |
| `arduino_shield_uno`  | Board has side headers compatible with the Arduino UNO        |
| `arduino_shield_mega` | Board has side headers compatible with the Arduino MEGA       |
| `xiao_shield`         | Board has headers compatible with the Seeedstudio XIAO boards |

@note   A board providing `arduino_shield_mega` **MUST** also provide
        `arduino_shield_uno`, as Arduino MEGA boards are backward compatible to
        shields for the Arduino UNO.

E.g. a module that implements the W5100 based Ethernet shield would depend on
both `arduino_shield_uno` and `arduino_shield_isp` for electrical compatibility
as well as on `arduino_spi` for SPI bus mapping and `arduino_pins` for the CS
pin mapping. This module could reuse the existing `w5100` driver and just
supply the correct `w5100_params_t` using the I/O mappings.
