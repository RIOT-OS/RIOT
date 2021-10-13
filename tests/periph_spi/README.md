Expected result
===============
You should be presented with the RIOT shell, providing you with commands to initialize a board
as master or slave, and to send and receive data via SPI.

Background
==========
Test for the low-level SPI driver.

## Default SPI CS pin

To overwrite the optional default cs pin CFLAGS can be used:

`CFLAGS="-DDEFAULT_SPI_CS_PORT=<my_port_int> -DDEFAULT_SPI_CS_PIN=<my_pin_int>" BOARD=<my_board> make flash term`
