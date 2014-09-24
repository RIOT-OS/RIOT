# Test for nrf24l01p lowlevel functions 

## About
This is a test application to check the nrf24l01+ lowlevel functions via netdev interface. 

## Usage
You should be presented with the RIOT shell, providing you with commands to initialize the transceiver (command: `it`), send data (command: `data`), read out and print all registers of the transceiver as binary values (command: `prgs`) and also print the values of a static `static_rx_buf` (command: `prxbuf`).

### Procedure
* take two boards and connect a transceiver to each
  (it should be also possible to use one board with different SPI-ports)
* dependent on your board, you'll maybe also need to connect a UART/tty converter 
* build and flash the test-program to each
* open a terminal (e.g. pyterm) for each 
* if possible, reset the board by reset-button. you'll see "_Welcome to RIOT_" etc.
* type `help` to see the description of the commands
* initialize both with `it`
* with one, send data with `send`
* on the other, type `prxbuf` and see the received values 


## Expected Results
After initialization (`it`) you should see the following output (neglecting the timestamps):
```
2014-10-13 14:51:26,281 - INFO # > it
2014-10-13 14:51:26,281 - INFO # 
2014-10-13 14:51:26,283 - INFO # Initialize Transceiver
2014-10-13 14:51:26,283 - INFO # 
2014-10-13 14:51:26,289 - INFO # State is: NETDEV_STATE_POWER_SLEEP
2014-10-13 14:51:26,292 - INFO # Channel was set sucessful to: 5
2014-10-13 14:51:26,296 - INFO # TX address was set sucessful to: e9e9e9e9e9
2014-10-13 14:51:26,300 - INFO # RX address was set sucessful to: e9e9e9e9e9
2014-10-13 14:51:26,301 - INFO # Disabled PIPE 1
2014-10-13 14:51:26,304 - INFO # Power was set sucessful to: 0 dB
2014-10-13 14:51:26,306 - INFO # Max. packet size is: 32 Byte
2014-10-13 14:51:26,310 - INFO # Protocol type is: NETDEV_PROTO_PROPRIETARY
2014-10-13 14:51:26,315 - INFO # State is: NETDEV_STATE_RX_MODE
2014-10-13 14:51:26,318 - INFO # Registrated callback function
2014-10-13 14:51:26,321 - INFO # Registering nrf24l01p_rx_handler thread...
2014-10-13 14:51:26,323 - INFO # ID was set sucessful to: 3
```
After the data has been sent by the transmitter (`send`), you can check the received data at the receiver (`prxbuf`). This should be (neglecting the timestamps):

```
2014-10-13 14:51:34,928 - INFO # > prxbuf
2014-10-13 14:51:34,928 - INFO # 
2014-10-13 14:51:34,929 - INFO # 0 1 2 3 
2014-10-13 14:51:34,929 - INFO # 4 5 6 7 
2014-10-13 14:51:34,930 - INFO # 8 9 10 11 
2014-10-13 14:51:34,932 - INFO # 12 13 14 15 
2014-10-13 14:51:34,933 - INFO # 16 17 18 19 
2014-10-13 14:51:34,934 - INFO # 20 21 22 23 
2014-10-13 14:51:34,935 - INFO # 24 25 26 27 
2014-10-13 14:51:34,936 - INFO # 28 29 30 31 
2014-10-13 14:51:34,937 - INFO # 32 33 34 35 
2014-10-13 14:51:34,938 - INFO # 36 37 38 39 
2014-10-13 14:51:34,940 - INFO # 40 41 42 43 
2014-10-13 14:51:34,941 - INFO # 44 45 46 47 
2014-10-13 14:51:34,942 - INFO # 48 49 50 51 
2014-10-13 14:51:34,943 - INFO # 52 53 54 55 
2014-10-13 14:51:34,944 - INFO # 56 57 58 59 
2014-10-13 14:51:34,945 - INFO # 60 61 62 63 
2014-10-13 14:51:34,946 - INFO # 0 0 0 0 
2014-10-13 14:51:34,947 - INFO # 0 0 0 0 

etc..
```

## Notes

* the static receive buffer (`static_rx_buf`) here is just 128 bytes long
* the receive procedure in this example is done in the ISR and gives the received data to the application code via callback. When in callback, an LED ist toggled (`LD3_TOGGLE`). Note that this may not be defined for all boards
* the ISR sends a message to a thread wich tells that new data was received. All the thread does in this example, is toggling an LED (`LD4_TOGGLE`). Note that this may not be defined for all boards
* the max. data length for one packet is 32 bytes (`NRF24L01P_MAX_DATA_LENGTH`). When a longer buffer has to be transferred, the function `_nrf24l01p_send_data` in _nrf24l01p.c_ splits this buffer into 32 byte packets. To "make shure" the receiver has received one packet, a relatively long delay has been inserted betwen the transmission of two packets, to simply test this functionality 

