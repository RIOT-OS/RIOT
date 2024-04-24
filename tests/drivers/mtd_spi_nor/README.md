# Test Program for the MTD SPI NOR Flash Driver

This program performs a number of tests with the MTD SPI NOR Flash driver,
including reading, writing and erasing of the chip.

This test will destroy the data present on the chip!

# Usage

## nRF52840DK
The test is designed to work with the built-in MX25F6435F flash of the
Nordic Semiconductor nRF52840DK development board.
Currently the nRF52840DK Flash definitions do not have the security options enabled.

To run the test you simply have to compile and run the test:
```
make flash term
```
The tests should conclude with "OK (3 tests)".

To enable the security features of the built-in flash, the "MTD_SPI_NOR_MX_SECURITY"
pseudomodule can be added to the make call:
```
USEMODULE+=mtd_spi_nor_mx_security make flash term
```

Likewise, the tests should conclude with "OK (3 tests)".

## nRF52840DK with different flash chips
The default SPI Device for using different Flash chips is SPI(1), which is present on
the Arduino Uno style headers. The default pin for Chip Select is P1.5. The SPI Device and
Chip Select pin can be changed with the CFLAGS FLASH_SPI_DEV and FLASH_SPI_CS:
```
CFLAGS+="-DFLASH_SPI_DEV=1 -DFLASH_SPI_CS='GPIO_PIN(1,5)'" ...
```

The Device under Test can be changed with CFLAGS as well:
```
... CFLAGS+="-DTEST_IS25LE01G" ...
```

If the Flash supports the security features, they can be enabled with the usage of a
pseudomodule as well:
```
... USEMODULE+=mtd_spi_nor_issi_security ...
```

A full test call would therefore be for example:
```
CFLAGS+="-DFLASH_SPI_DEV=1 -DFLASH_SPI_CS='GPIO_PIN(1,5)' -DTEST_IS25LE01G" \
USEMODULE+=mtd_spi_nor_issi_security make flash term
```

# Tested/Supported Chips

## ISSI

- IS25LP128 (no security features)
- IS25LE01G

## Macronix

- M25F6435F (built-in on nRF52840DK)
- M25F12873F
