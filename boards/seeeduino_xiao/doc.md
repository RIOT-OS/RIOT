@defgroup   boards_seeeduino_xiao Seeeduino XIAO
@ingroup    boards
@brief      Support for the Seeeduino XIAO.

### General information

The Seeeduino XIAO is the smallest member of the Seeeduino family. It carries the powerful ATSAMD21G18A-MU which is a low-power microcontrollers. On the other hand, this little board has good performance in processing but needs less power. As a matter of fact, it is designed in a tiny size and can be used for wearable devices and small projects.

Seeeduino XIAO has 14 GPIO PINs, which can be used for 11 digital interfaces, 11 mock interfaces, 10 PWM interfaces (d1-d10), 1 DAC output pin D0, 1 SWD pad interface, 1 I2C interface, 1 SPI interface, 1 UART interface, Serial communication indicator (T/R), Blink light (L). The colors of LEDs(Power,L,RX,TX) are green, yellow, blue and blue. Moreover, Seeeduino XIAO has a Type-C interface which can supply power and download code. There are two reset button, you can short connect them to reset the board.

-- General description of the [wiki](https://wiki.seeedstudio.com/Seeeduino-XIAO/)

### Pinout

![XIAO pinout](https://files.seeedstudio.com/wiki/Seeeduino-XIAO/img/Seeeduino-XIAO-pinout.jpg)

### Flash the board

Use `BOARD=seeeduino_xiao` with the `make` command.<br/>

Example with `default` application:
```
     make BOARD=seeeduino_xiao -C examples/basic/default flash
```

RIOT will automatically trigger a reset to the bootloader, but this only works if RIOT is still
running on the board.
If your application has crashed or got erased, `make flash` will not be able to trigger a bootloader reset.

To manually enter the bootloader, short the RST pins with a short line or tweezers
You know you've successfully entered the bootloader when the orange LED flickers on and light up.

![Reset Sequence](https://files.seeedstudio.com/wiki/Seeeduino-XIAO/img/XIAO-reset.gif)

Sometimes flashing fails and the board gets stuck in the bootloader.
In this case, just run `make flash` again when the device is not busy anymore.
