# About

This is a manual test application for the [Microchip ATA8020E Sigfox module](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-9409-Smart-RF-ATA8520E_Datasheet.pdf).

# Usage

This application provides a `sigfox` shell command to interact with the device.
Use the subcommands to perform common Sigfox operations:
* `sigfox info` returns the Atmel and Sigfox versions of the module
* `sigfox keys` returns the ID and PAC keys used to activate the device in the
  Sigfox cloud backend at https://backend.sigfox.com/activate.
* `sigfox tx <payload>` sends a payload to the Sigfox backend. The payload can
  be either a frame (e.g a string with 12 characters max) or a bit (e.g 0 or 1)
* `sigfox tx_rx <payload>` is the same as the previous but after sending the
  payload, it waits for incoming message sent from the backend

# Examples:

* Read the internal keys of the module:
```
    > sigfox keys
    ID: 00000000
    PAC: 0000000000000000
```
* Send a bit value to the backend:
```
    > sigfox tx 1
    Bit sent with success
```
* Send a string message to the backend:
```
    > sigfox tx This\ is\ RIOT
    Message sent with success
```
