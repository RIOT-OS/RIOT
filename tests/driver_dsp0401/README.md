## About

This is a test application for the [DSP0401](https://www.embeddedadventures.com/datasheets/DSP-0401B_hw_v4.pdf),
a 4 digits with 16 segments alphanumeric display.

## Connecting the device

The display can be controlled from the JP1 connection jumper as follows:
* Connect `SIN` to the board `DSP0401_PARAM_SDI_PIN` (see your board_params.h file)
* Connect `CLK` to the board `DSP0401_PARAM_CLK_PIN`
* Connect `LAT` to the board `DSP0401_PARAM_LAT_PIN`
* Connect `PWM` to the board `DSP0401_PARAM_PWM_DEV` pwm device. See the board periph_conf.h
to find a valid pin and configuration.
* Connect `VCC` to one of the board VCC pin (3.3V or 5V depending on your board.
  Example: use 3.3V with STM32 Nucleo-64)
* Connect `GND` to one of the board GND.

Since there are potentially a lot of LEDs to poweron, it's preferable to use an
external power supply to power the DSP0401 module. This is what JP2 is meant for.
See the [datasheet](https://www.embeddedadventures.com/datasheets/DSP-0401B_hw_v4.pdf)
section `Power` for more information on power configuration.

## Chaining devices

Since DSP0401 contains a shift register, it's possible to chain devices by connecting
the JP4 and JP5 connection jumpers of the device N to respectively JP2 and JP1 of the
device N+1.
Attention: ensure your power supply is able to provide enough current for the number of devices
you plan to use.
