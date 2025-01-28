# About
This is a manual test application for the NRF802154 radio driver.
Supported boards must have an nRF52-series chip that has an IEEE 802.15.4 radio (e.g. nrf52840dk).

# Usage
For testing the radio driver you can use the ifconfig and txtsnd shell commands
that are included in this application.

For testing beacon frame acceptance you can flash this test onto two nRF52 devices
and use the beaconsend command to send an IEEE 802.15.4 beacon frame.
The opponent should print the received frame details.