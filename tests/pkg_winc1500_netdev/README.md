# WINC1500 Wifi Module Driver

## How to use the test program

### Debugging support

To get more debugging messages while developing you can enable debugging mode
by uncommenting `#CFLAGS += -DLOG_LEVEL=LOG_DEBUG` in `Makefile`.

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
