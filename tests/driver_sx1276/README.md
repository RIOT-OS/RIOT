# About
This is a manual test application for the SX1276 radio driver

For running this test, you need to connect/configure the following pins of your
radio device:
- SPI MISO
- SPI MOSI
- SPI CLK
- NSS (negative slave select)
- RESET
- DIO0
- DIO1
- DIO2
- DIO3
- DIO4 (not used)
- DIO5 (not used)

# Usage
To setup bandwidth parameters, use lora_setup
To change frequency, go to common.h
For testing the radio you can use the tx_test and rx_test commands
To work with registers values, use get/set commands
