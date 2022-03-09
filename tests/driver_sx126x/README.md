sx126x/llcc68 LoRa driver
==================

This is a manual test application for the SX1261/2/8 and LLCC68 LoRa radio driver.

Usage
=====

This application adds a shell command to control basic features radio device:

```
main(): This is RIOT! (Version: 2021.04-devel)
Initialization successful - starting the shell now
> help
help
Command              Description
---------------------------------------
sx126x               Control the SX126X radio
reboot               Reboot the node
version              Prints current RIOT_VERSION
pm                   interact with layered PM subsystem
> sx126x
sx126x
Usage: sx126x <get|set|rx|tx>
```

The `get` and `set` subcommands allows for getting/setting the current
frequency channel (freq) and lora modulation parameters (bw, sf, cr).

To put the device in listen mode, use the `rx` subcommand:

```
> sx126x rx start
sx126x rx start
Listen mode started
```

To send a message, use the `tx` subcommand:

```
> sx126x tx "This is RIOT!"
sx126x tx "This is RIOT!"
sending "This is RIOT!" payload (14 bytes)
> Transmission completed
```
