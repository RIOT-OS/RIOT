# Semtech LoRaMAC package test application

## About

This is a test application for the Semtech LoRaMAC package. This package
provides the MAC primitives for sending and receiving data to/from a
LoRaWAN network.

See [LoRa Alliance](https://www.lora-alliance.org/) for more information on LoRa.
See [Semtech LoRamac-node repository](https://github.com/Lora-net/LoRaMac-node)
to have a look at the original package code.

This application can only be used with Semtech
[SX1272](http://www.semtech.com/images/datasheet/sx1272.pdf) or
[SX1276](http://www.semtech.com/images/datasheet/sx1276.pdf) radio devices.

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

The default parameters for the Semtech SX1272/SX1276 radios works as-is with
STM32 Nucleo-64 boards and MBED LoRa shields
([SX1276](https://os.mbed.com/components/SX1276MB1xAS/) or
[SX1272](https://os.mbed.com/components/SX1272MB2xAS/)). You can also use the
ST [b-l072z-lrwan1](http://www.st.com/en/evaluation-tools/b-l072z-lrwan1.html)
board.

Depending on the type of radio device, set the `LORA_DRIVER` variable accordingly:
For example:

    LORA_DRIVER=sx1272 make BOARD=nucleo-f411re -C tests/pkg_semtech-loramac flash term

will build the application for a nucleo-f411re with an SX1272 based mbed LoRa shield.

The SX1276 is the default value.

The other parameter that has to be set at build time is the geographic region:
`EU868`, `US915`, etc. See LoRaWAN regional parameters for more information.

    LORA_REGION=US915 LORA_DRIVER=sx1272 make BOARD=nucleo-f411re -C tests/pkg_semtech-loramac flash term

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
