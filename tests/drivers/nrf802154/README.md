# About
This is a manual test application for the NRF802154 radio driver.

# Usage
For testing the radio driver you can use the ifconfig and txtsnd shell commands
that are included in this application.

For testing beacon frame acceptance you can flash this test onto two nrf52 devices
and use the send command to send a beacon frame. The opponent should print the received frame details.