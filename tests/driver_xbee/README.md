# About
This is a manual test application for testing the Xbee S1 network device driver.

For running this test, you need to connect the following pins of a Xbee S1
module to your board:
- UART RX
- UART TX
- VCC (3V3)
- GND

NOTE: when you use an Arduino Xbee shield, the Xbee module is connected to the
same UART as RIOTs standard out. In this case you must redefine UART_STDIO_DEV to
another UART interface in the board.h and connect a UART-to-USB adapter to that
UART.

# Usage
For testing the Xbee driver you can use the netif shell commands that are
included in this application.
