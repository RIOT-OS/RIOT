# WINC1500 WiFi Module Driver

## How to use the test program

Compile the program by `make all` command. To build the program for a specific
board, you can set `BOARD` variable when calling `make` command. For example,
if you want to build it for Arduino mkr1000, you may run
`BOARD=arduino-mkr1000 make all` command.

Currently the test program works with the following boards without
modifying the code:

* Arduino mkr1000: `BOARD=arduino-mkr1000`
* SAM D21 Xplained Pro Evaluation Kit with ATWINC1500-XPRO module: `BOARD=samd21-xpro`
* SAM R21 Xplained Pro Evaluation Kit with ATWINC1500-XPRO module: `BOARD=samr21-xpro`

After building the program, you should flash the device with `make flash`
command. You can connect the device and use the interactive shell with
`make term` command.

The test program supports the following additional shell commands for the WiFi module:

* `init`: Initialize a WINC1500 WiFi module.
* `scan`: Scan for available access points and then list them.  
* `connect SSID [password]`: Connect to an access point. You should add `password` parameter if it is a WPA-secured access point.
* `disconnect`: Disconnect with the connected access point.
* `info`: Display information of the connected access point.

### How to test with other boards

You can test this program with other boards by adding your own `winc1500_params.h`
file or by changing WINC1500-related macros. The following example can be added
to `Makefile` for `stm32f4discovery` board:

```makefile
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  CFLAGS += -DWINC1500_SPI=SPI_DEV\(1\)
  CFLAGS += -DWINC1500_SPI_CS_PIN=GPIO_PIN\(PORT_E,14\)
  CFLAGS += -DWINC1500_INTN_PIN=GPIO_PIN\(PORT_E,8\)
  CFLAGS += -DWINC1500_RESET_PIN=GPIO_PIN\(PORT_E,12\)
  CFLAGS += -DWINC1500_CHIP_EN_PIN=GPIO_PIN\(PORT_E,13\)
  CFLAGS += -DWINC1500_WAKE_PIN=GPIO_PIN\(PORT_C,2\)
endif
```

### Debugging support

To get more debugging messages while developing you can enable debugging mode
by adding `CFLAGS += -DLOG_LEVEL=LOG_DEBUG` in `Makefile`. Also consider adding
`CFLAGS += -DDEVELHELP -g3 -ggdb3 -O0 -Og` in `Makefile`.

## WINC1500 Xplained Pro Pinmap

### ATWINC1500 Connection

| Pin | Function  |
|:---:|-----------|
| 5   | RESET_N   |
| 6   | WAKE      |
| 9   | IRQ_N     |
| 10  | CHIP_EN   |
| 15  | SPI_SSN   |
| 16  | SPI_MOSI  |
| 17  | SPI_MISO  |
| 18  | SPI_SCK   |
| 19  | GND       |
| 20  | VCC       |

### ATECC508A Crypto device Connection

| Pin | Function  |
|:---:|-----------|
| 11  | I2C_SDA   |
| 12  | I2C_SCL   |
