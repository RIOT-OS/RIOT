## LoRaWAN

### Description

This application shows a basic LoRaWAN use-case with RIOT.

By using the real time clock and low-power capabilities of a board, this
application also shows how to program a LoRaWAN Class A device using RIOT.

By default, the application uses the Over-The-Air Activation (OTAA) procedure.

### Configuration

To join a LoRaWAN network using OTAA activation, edit the application
`Makefile` and set your device information:

    ACTIVATION_MODE ?= otaa
    DEVEUI ?= 0000000000000000
    APPEUI ?= 0000000000000000
    APPKEY ?= 00000000000000000000000000000000

To join a LoRaWAN network using ABP activation, edit the application
`Makefile` and set your device and LoRaWAN application information:

    ACTIVATION_MODE ?= abp
    DEVADDR ?= 00000000
    NWKSKEY ?= 00000000000000000000000000000000
    APPSKEY ?= 00000000000000000000000000000000
    RX2_FREQ ?= 869525000
    RX2_DR ?= 3

Note that rx2 frequency (`RX2_FREQ`) and datarate (`RX2_DR`) variables must be
set explicitly at compile time when using ABP activation because they are
supposed to be known in advance by the network and the device. In this example,
the values used are compatible with TheThingsNetwork provider network.
They might change depending on the network provider used.

Use the `BOARD`, `DRIVER` and `LORA_REGION` make variables to adapt the application
to your hardware setup and region of use:

- `BOARD` can be one of the nucleo-64 boards
- `DRIVER` can be either `sx1261`, `sx1262`, `sx1268`, `sx1276` or `sx1272`
- `LORA_REGION` can be `EU868`, `US915`, etc (see LoRaWAN regional parameters for
  details).

The `SEND_PERIOD_S` variable can also be adapted to change the time period (in
seconds) between each message sent by the device.

### Usage

Simply build and flash the application for a ST B-L072Z-LRWAN1 board:

    make flash term

ST Nucleo-64 can be used as-is with mbed LoRa shields: there's one based on
[the sx1276 radio](https://os.mbed.com/components/SX1276MB1xAS/) and one based
on the [the sx1272 radio](https://os.mbed.com/components/SX1272MB2xAS/).

## Automatic test

The automatic test replicates 11-lorawan release specs test:

- [11-lorawan](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md)
  - [Task #01 - LoRaWAN example](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md#task-01---lorawan-example)

It is recommended to test using iotlab-nodes. The default configuration is already
set on the application Makefile.

### Requirements

- The tests assumes that there is a gateway in all DR distance to the device and the
device was flashed with the correct keys.

- To use iotlab it is required to have a valid account for the FIT IoT-LAB
(registration there is open for everyone) and the [iot-lab/cli-tools](https://github.com/iot-lab/cli-tools) need to be installed.

- iotlab uses TTN lorawan gateways, to run the test you will need to create an
[account](https://account.thethingsnetwork.org/) add an [application](https://www.thethingsnetwork.org/docs/applications/add.html) and [register](https://www.thethingsnetwork.org/docs/devices/registration.html) a device. For this
test you need to take note of the Device EUI, Application EUI & Application Key
for that device.

### Usage

1. flash device with appropriate keys and test

    $ DEVEUI=<device eui> APPEUI=<application eui> APPKEY=<application key> make -C examples/lorawan/ flash test

#### With iotlab

1. setup the iotlab experiment:

    $ make -C examples/lorawan/ iotlab-exp

2. flash device, set appropriate keys and test

    $ DEVEUI=<device eui> APPEUI=<application eui> APPKEY=<application key> IOTLAB_NODE=auto make -C examples/lorawan/ flash test

3. stop the iotlab experiment:

    $ make -C examples/lorawan/ iotlab-stop

_note_: if you have multiple running experiments you will need to set `IOTLAB_EXP_ID`
        to the appropriate experiment, when using the `iotlab-exp` you will see a:
        `Waiting that experiment 175694 gets in state Running`. That number matches
        the experiment id you started.
