## About

This is a manual test application for the SX127X radio driver.

This test application uses the default pin configuration provided by the
driver implementation and that matches the STM32 Nucleo-64 layout.
It is best to use [SX1272](https://developer.mbed.org/components/SX1272MB2xAS/)
or [SX1276](https://developer.mbed.org/components/SX1276MB1xAS/) mbed modules
with nucleo boards or the all-in-one
[ST P-NUCLEO-LRWAN1 LoRa kit](http://www.st.com/en/evaluation-tools/p-nucleo-lrwan1.html).

If you have other hardware (boards, Semtech based LoRa module), you can adapt
the configuration to your needs by copying an adapted version of
`drivers/sx127x/include/sx127x_params.h` file to your application directory.

By default the application builds the SX1276 version of the driver. If you
want to use this application with a SX1272 module, set the variable `DRIVER` in
the application [Makefile](Makefile):
```
DRIVER = sx1272
```
instead of
```
DRIVER = sx1276
```
You can also pass `DRIVER` when building the application:
```
$ make BOARD=nucleo-l073rz DRIVER=sx1272 -C tests/drivers_sx127x flash term
```

## Usage

This test application provides low level shell commands to interact with the
SX1272/SX1276 modules.

Once the board is flashed and you are connected via serial to the shell, use the `help`
command to display the available commands:
```
> help
help
Command              Description
---------------------------------------
setup                Initialize LoRa modulation settings
random               Get random number from sx127x
channel              Get/Set channel frequency (in Hz)
register             Get/Set value(s) of registers of sx127x
send                 Send raw payload string
listen               Start raw payload listener
reboot               Reboot the node
ps                   Prints information about running threads.
```

Once the board is booted, use `setup` to configure the basic LoRa settings:
* Bandwidth: 125kHz, 250kHz or 500kHz
* Spreading factor: between 7 and 12
* Code rate: between 5 and 8

Example:
```
> setup 125 12 5
setup: setting 125KHz bandwidth
[Info] setup: configuration set with success
```

All values are supported by both SX1272 and SX1276.

The `random ` command use the Semtech to generate a random integer value.

Example:
```
> random
random: number from sx127x: 2339536315
> random
random: number from sx127x: 863363442
```

The `channel` command allows to change/query the RF frequency channel.
The default is 868MHz for Europe, change to 915MHz for America. The frequency
is given/returned in Hz.

Example:
```
> channel set 868000000
New channel set
> channel get
Channel: 868000000
```

The `register` command allows to get/set the content of the module registers.
Example:
```
> register get all
- listing all registers -
Reg   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
0x00 00 80 1A 0B 00 52 D9 00 00 0F 08 2B 00 4D 80 00
0x10 00 FF 00 00 00 00 00 00 00 00 00 00 00 72 C4 0A
0x20 00 08 01 FF 00 00 08 00 00 00 00 00 00 50 14 40
0x30 00 03 05 67 1C 0A 00 0A 42 12 64 19 01 A1 00 00
0x40 00 10 22 13 0E 5B DB 24 0E 81 3A 2E 00 03 00 00
0x50 00 00 04 23 01 24 3F B0 09 05 84 0B D0 0B D0 32
0x60 2B 14 00 00 10 00 00 00 0F E0 00 0C F6 10 1D 07
0x70 00 5C 00 00 00 00 00 00 00 00 00 00 00 00 00 00
- done -
```

Use the `send` and `receive` commands in order to exchange messages between several modules.
You need first to ensure that all modules are configured the same: use `setup` and
`channel` commands to configure them correctly.

Assuming you have 2 modules, one listening and one sending messages, do the following:
* On listening module:
```
> setup 125 12 5
setup: setting 125KHz bandwidth
[Info] setup: configuration set with success
> channel set 868000000
New channel set
> listen
Listen mode set
```
* On sending module:
```
> setup 125 12 5
setup: setting 125KHz bandwidth
[Info] setup: configuration set with success
> channel set 868000000
New channel set
> send This\ is\ RIOT!
```

On the listening module, the message is captured:
```
{Payload: "This is RIOT!" (13 bytes), RSSI: 103, SNR: 240}
```
