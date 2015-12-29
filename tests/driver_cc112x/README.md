# About
This is a manual test application for the AT86RF2xx radio driver

For running this test, you need to connect/configure the following pins of your
radio device:
- SPI MISO
- SPI MOSI
- SPI CLK
- CS (ship select)
- RESET
- SLEEP
- INT (external interrupt)

# Usage
For testing the radio driver you can use the netif and txtsnd shell commands
that are included in this application.
