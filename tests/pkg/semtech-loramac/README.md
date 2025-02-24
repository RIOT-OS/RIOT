# Semtech LoRaMAC package test application

## About

This is a test application for the Semtech LoRaMAC package. This package
provides the MAC primitives for sending and receiving data to/from a
LoRaWAN network.

See [LoRa Alliance](https://www.lora-alliance.org/) for more information on LoRa.
See [Semtech LoRamac-node repository](https://github.com/Lora-net/LoRaMac-node)
to have a look at the original package code.

This application can only be used with Semtech
[SX1272](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/440000001NCE/v_VBhk1IolDgxwwnOpcS_vTFxPfSEPQbuneK3mWsXlU) or
[SX1276](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R0000001OKs/Bs97dmPXeatnbdoJNVMIDaKDlQz8q1N_gxDcgqi7g2o) radio devices.

## Application configuration

Before building the application and joining a LoRaWAN network, you need an
account on a LoRaWAN backend provider. Then create a LoRaWAN application and
register your device.
Since this application has been heavily tested with the backend provided by
[TheThingsNetwork](https://www.thethingsnetwork.org/) (TTN), we recommend that
you use this one.

Once your application and device are created and registered, you'll have
several information (provided by the LoRaWAN provider):

* The type of join procedure: ABP (Activation by personnalization) or OTAA (
  Over The Air Activation)
* The device EUI: an 8 bytes array
* The application EUI: an 8 bytes array
* The application key: a 16 bytes array
* The device address: a 4 bytes array, only required with ABP join procedure
* The application session key: a 16 bytes array, only required with ABP join procedure
* The network session key: a 16 bytes array, only required with ABP join procedure

Once you have this information, either edit the `Makefile` accordingly or
use the `set`/`get` commands in test application shell.

## Building the application

The default parameters for the Semtech SX1261/SX1262/SX1272/SX1276 radios works as-is with
STM32 Nucleo-64 boards and MBED LoRa shields
([SX1261/SX1262](https://os.mbed.com/components/SX126xMB2xAS/),
[SX1276](https://os.mbed.com/components/SX1276MB1xAS/) or
[SX1272](https://os.mbed.com/components/SX1272MB2xAS/)). You can also use the
ST [b-l072z-lrwan1](http://www.st.com/en/evaluation-tools/b-l072z-lrwan1.html)
board.

Depending on the type of radio device, set the `LORA_DRIVER` variable accordingly:
For example:

    LORA_DRIVER=sx1272 make BOARD=nucleo-f411re -C tests/pkg/semtech-loramac flash term

will build the application for a nucleo-f411re with an SX1272 based mbed LoRa shield.

The SX1276 is the default value.

The other parameter that has to be set at build time is the geographic region:
`EU868`, `US915`, etc. See LoRaWAN regional parameters for more information.

    LORA_REGION=US915 LORA_DRIVER=sx1272 make BOARD=nucleo-f411re -C tests/pkg/semtech-loramac flash term

will build the application for a nucleo-f411re with an SX1272 based mbed LoRa shield
for US915 region.

The default region is `EU868`.

**For testing purpose**, it is possible to disable the duty-cycle restriction
implemented in the MAC layer with the `DISABLE_LORAMAC_DUTYCYCLE` macro:

      CFLAGS=-DDISABLE_LORAMAC_DUTYCYCLE LORA_REGION=US915 LORA_DRIVER=sx1272 make ...

## Using the shell

This application provides the `loramac` command for configuring the MAC,
joining a network and sending/receiving data to/from a LoRaWAN network.
`join` and `tx` subcommands are blocking until the MAC is done. Class A
is activated by default.

### Joining with Over The Air Activation

* Set your device EUI, application EUI, application key:

      > loramac set deveui AAAAAAAAAAAAAAAA
      > loramac set appeui BBBBBBBBBBBBBBBB
      > loramac set appkey CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

* Join a network using the OTAA procedure:

      > loramac join otaa
      Join procedure succeeded!

### Joining with Activation By Personalization

OTAA is always preferred in real world scenarios.
However, ABP can be practical for testing or workshops.

* Set your Device Address, Network Session Key , Application Session Key:

      > loramac set devaddr AAAAAAAA
      > loramac set nwkskey BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
      > loramac set appskey CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

* Join a network using the ABP procedure:

      > loramac join abp
      Join procedure succeeded!

Saving frame counters in flash memory is not (yet) supported.
Before sending data, it's necessary to clear frame counters in
Network Server! Otherwise uplink messages won't work.

**NOTE**:
If using TTN with ABP make sure to set the correct datarate for RX2.
Otherwise confirmed and downlink messages won't work.
The datarate for RX2 should be DR3 (SF9BW125) as seen in
[TTN LoRaWAN Frequencies Overview](https://www.thethingsnetwork.org/docs/lorawan/frequency-plans.html):

      > loramac set rx2_dr 3

### Sending and receiving data

* Send confirmable data on port 2 (cnf and port are optional):

      > loramac tx This\ is\ RIOT! cnf 2

* Send unconfirmable data on port 10:

      > loramac tx This\ is\ RIOT! uncnf 10

When using Class A (default mode) downlink messages will be received in
the next uplink:

      > loramac tx hello
        Data received: RIOT, port: 1

### Other shell commands

* Save the device LoRaWAN configuration (EUIs and keys) in EEPROM (if provided
  by the microcontroller):

      > loramac save

  On the next device reboot, these parameters will be automatically re-read
  from the EEPROM non-volatile storage and thus you can join directly the
  network without entering them again from the command line.

* Remove previously stored LoRaWAN configuration:

      > loramac erase

* Switch the default datarate index (from 0 to 16). 5 is for SF7, BW125:

      > loramac set dr 5

* Switch to adaptive data rate:

      > loramac set adr on

* Perform a Link Check command (will be triggered in the next transmission):

      > loramac link_check

The list of available commands:

      > help
      help
      Command              Description
      ---------------------------------------
      loramac              control the loramac stack
      reboot               Reboot the node


On the TTN web console, you can follow the activation and the data
sent/received to/from a node.

## Playing with MQTT to send/receive data to/from a LoRa node

TheThingsNetwork API also provide a MQTT broker to send/receive data.
See the
[online documentation](https://www.thethingsnetwork.org/docs/applications/mqtt/api.html)
for more information.

* Let's use [mosquitto](https://mosquitto.org/) clients. They can be installed
  on Ubuntu using:

      sudo apt install mosquitto-clients

* Subscribe to data raised by any node from any application:

      mosquitto_sub -h eu.thethings.network -p 1883 -u <your username> -P <your password> -t '+/devices/+/up'

* Publish some data to one of the node:

      mosquitto_pub -h eu.thethings.network -p 1883 -u <your username> -P <your password> -t '<application name>/devices/<device name>/down' -m '{"port":2, "payload_raw":"VGhpcyBpcyBSSU9UIQ=="}'

After sending some data from the node, the subscribed MQTT client will display:

    {"app_id":"<your application>","dev_id":"<your node>","hardware_serial":"XXXXXXXXXXXX","port":2,"counter":7,"confirmed":true,"payload_raw":"dGVzdA==","metadata":    {"time":"2017-12-14T09:47:24.84548586Z","frequency":868.1,"modulation":"LORA","data_rate":"SF12BW125","coding_rate":"4/5","gateways":[{"gtw_id":"eui-xxxxxxxx","timestamp":3910359076, "time":"2017-12-14T09:47:24.85112Z","channel":0,"rssi":-10,"snr":12.2,"rf_chain":1,"latitude":48.715027,"longitude":2.2059395,"altitude":157,"location_source":"registry"}]}}

The payload sent is in the `payload_raw` json field and is formatted in base64
(`dGVzdA==` in this example).

The node will also print the data received:

    > loramac tx test
    Data received: This is RIOT!

## Automatic test

The automatic test replicates 11-lorawan release specs tests:

- [11-lorawan](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md)
  - [Task #02 - OTAA join procedure](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md#task-02---otaa-join-procedure)
  - [Task #03 - ABP join procedure](https://github.com/RIOT-OS/Release-Specs/blob/ba236c4a1d1258ab63d21b0a860d0f5a5935bbd4/11-lorawan/11-lorawan.md#task-03---abp-join-procedure)
  - [Task #04 - LoRaWAN device parameters persistence](https://github.com/RIOT-OS/Release-Specs/blob/master/11-lorawan/11-lorawan.md#task-04---lorawan-device-parameters-persistence)

It is recommended to test using iotlab-nodes. The default configuration is already
set on the application Makefile.

### Requirements

- The tests assumes that there is a gateway in all DR distance to the device and the
device was flashed with the correct keys. The APPEUI is assumed to be the same for OTAA
and ABP.

- The DR duty cycling time-offs values are for EU863-870

- To use iotlab it is required to have a valid account for the FIT IoT-LAB
(registration there is open for everyone) and the [iot-lab/cli-tools](https://github.com/iot-lab/cli-tools) need to be installed.

- The frame counters must be reset on the LoRaWAN backend at the beginning of the
test.

- iotlab uses TTN lorawan gateways, to run the test you will need to create an
[account](https://account.thethingsnetwork.org/), add an [application](https://www.thethingsnetwork.org/docs/applications/add.html)
and [register](https://www.thethingsnetwork.org/docs/devices/registration.html)
a device. Two devices must be registered, one configured for OTA and another
for ABP. For this test you need to take note of the Device EUI, Application EUI
& Application Key of the device registered for OTA, as well as the Device EUI,
Device Address, Network and Application session keys of the device registered
for ABP. The test assumes that both devices have the same Application EUI.

### Usage

1. flash device with appropriate keys and test

    $ DEVEUI_OTA=<...> DEVEUI_ABP=<...> APPEUI=<...> APPKEY=<...> DEVADDR=<...> NWKSKEY=<...> APPSKEY=<...> RX2_DR=<...> make BOARD=b-l072z-lrwan1 -C tests/pkg/semtech-loramac test

#### With iotlab

1. setup the iotlab experiment:

    $ make -C tests/pkg/semtech-loramac iotlab-exp

2. flash device with the appropriate keys and test

    $ DEVEUI=<...> APPEUI=<...> APPKEY=<...> DEVADDR=<...> NWKSKEY=<...> APPSKEY=<...> RX2_DR=<...> IOTLAB_NODE=auto make -C tests/pkg/semtech-loramac flash test

3. stop the iotlab experiment:

    $ make -C examples/networking/misc/lorawan/ iotlab-stop

_note_: if you have multiple running experiments you will need to set `IOTLAB_EXP_ID`
        to the appropriate experiment, when using the `iotlab-exp` you will see a:
        `Waiting that experiment 175694 gets in state Running`. That number matches
        the experiment id you started.
