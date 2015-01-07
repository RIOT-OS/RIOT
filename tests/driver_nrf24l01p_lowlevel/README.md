# Test for nrf24l01p lowlevel functions 

## About
This is a small test application to see how the lowlevel-driver functions of the proprietary nrf24l01p-transceiver work. These functions consist of general SPI and GPIO commands, which abstract the driver-functions from the used board. In order to build this application, you need to add the board to the Makefile's `WHITELIST` first and define a pin mapping.

## Predefined pin mapping
Please compare the `tests/driver_nrf24l01p_lowlevel/Makefile` for predefined pin-mappings on different boards. (In addition, you also need to connect to 3V and GND)

## Usage
You should be presented with the RIOT shell, providing you with commands to initialize the transceiver (command: `it`), sending one packet (command: `send`) or read out and print all registers of the transceiver as binary values (command: `prgs`). 

### Procedure
* take two boards and connect a transceiver to each
  (it should be also possible to use one board with different SPI-ports)
* depending on your board, you'll maybe also need to connect a UART/tty converter 
* build and flash the test-program to each
* open a terminal (e.g. pyterm) for each 
* if possible, reset the board by using the reset-button. You'll see "_Welcome to RIOT_" etc.
* type `help` to see the description of the commands
* initialize both with `it`
* with one board, send a packet by typing `send`
* in the next step you can also use `send` to send data in the other direction
* now you can use send on both boards/transceivers to send messages between them


## Expected Results
After you did all steps described above, you should see that a 32 Byte sequence (numbers from 32...1) has been transferred from one device to the other. This sequence is printed out from the receiver after the receive interrupt occurred and the receive-procedure has been made.

After initialization (`it`) you should see the following output:

```
 > it
 
Init Transceiver

Registering nrf24l01p_rx_handler thread...
################## Print Registers ###################
REG_CONFIG: 
0x0 returned: 00111111

REG_EN_AA: 
0x1 returned: 00000001

REG_EN_RXADDR: 
0x2 returned: 00000011

REG_SETUP_AW: 
0x3 returned: 00000011

REG_SETUP_RETR: 
0x4 returned: 00101111

REG_RF_CH: 
0x5 returned: 00000101

REG_RF_SETUP: 
0x6 returned: 00100111

REG_STATUS: 
0x7 returned: 00001110

REG_OBSERVE_TX: 
0x8 returned: 00000000

REG_RPD: 
0x9 returned: 00000000

REG_RX_ADDR_P0: 
0xa returned: e7 e7 e7 e7 e7 

REG_TX_ADDR: 
0x10 returned: e7 e7 e7 e7 e7 

REG_RX_PW_P0: 
0x11 returned: 00100000

REG_FIFO_STATUS: 
0x17 returned: 00010001

REG_DYNPD: 
0x1c returned: 00000000

REG_FEATURE: 
0x1d returned: 00000000

```

After the data has been sent (`send`), you should see the following output on the receiver terminal:
```
In HW cb
nrf24l01p_rx_handler got a message: Received packet.
32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 

```