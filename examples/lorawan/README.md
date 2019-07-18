LoRaWAN
=======

Description
-----------

This application shows a simple use case of LoRaWAN with RIOT.

By using the real time clock and low-power capabilities of a board, this
application shows how to program a LoRaWAN Class A device using RIOT.

This application is using the Over-The-Air Activation procedure.

Usage
-----

Simply build and flash the application for a ST B-L072Z-LRWAN1 board:

    make flash term

Use the `BOARD`, `DRIVER` and `LORA_REGION` make variables to adapt the application
to your hardware setup and region of use:

- `BOARD` can be one of the nucleo-64 boards
- `DRIVER` can be either `sx1276` or `sx1272`
- `LORA_REGION` can be `EU868`, `US915`, etc (see LoRaWAN regional parameters for
  details).

ST Nucleo-64 can be used with mbed LoRa shields: there's one based on
[the sx1276 radio](https://os.mbed.com/components/SX1276MB1xAS/) and one based
on the [the sx1272 radio](https://os.mbed.com/components/SX1272MB2xAS/).

Finally, to join a LoRaWAN network using OTAA activation, edit the application
`Makefile` and set your device information:

    DEVEUI ?= 0000000000000000
    APPEUI ?= 0000000000000000
    APPKEY ?= 00000000000000000000000000000000

## Automatic test

The automatic test replicates 11-lorawan release specs test:

- [11-lorawan](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md)
  - [Task #01 - LoRaWAN example](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md#task-01---lorawan-example)


### Assumptions

- The tests assumes that there is a gateway in all DR distance to the device and the
device was flashed with the correct keys.

### Usage

On a board with a lora radio device run:

    $ make BOARD=b-l072z-lrwan1 -C examples/lorawan test