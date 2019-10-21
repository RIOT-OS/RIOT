# Linux 6lowpan autoconn

This Python script loads and enables 6lowpan Linux module and then starts scanning BLE devices which have IP-over-BLE capabilities in order to autoconnect to them. You can also choose (optionally) what name of the devices you want to filter.

It was inspired from [this](https://github.com/RIOT-OS/RIOT/pull/12361).

**KEEP IN MIND** that IPv6-over-BLE between RIOT and Linux is highly experimental at the moment and does suffer stability issues!

## Requirements

You need the [bluepy](https://github.com/IanHarvey/bluepy) Python library to execute this, so follow their [README's](https://github.com/IanHarvey/bluepy/blob/master/README.md) installation instructions first.

To understand how this works, it is recommended to follow [this](https://github.com/RIOT-OS/RIOT/blob/master/pkg/nimble/README.ipv6-over-ble.md) first.

## Start the script

This script automates the process of loading and enabling the 6lowpan Linux module, so you need to be logged in as root.

    sudo su

>  **Note:** Make sure your debugfs is correctly mounted. Anyway, this is already the case on many distributions.

    mount -t debugfs none /sys/kernel/debug

To execute the script you can just type:

    python3 6lowpan_autoconn.py

Or (optionally) add also the name filter:

    python3 6lowpan_autoconn.py -n NAME

Keep it running in the background. It will uninterruptedly search for devices and autoconnect to the correct ones.

To test the connection just try with `ping6` or `ifconfig` as stated in the final section [here](https://github.com/RIOT-OS/RIOT/blob/master/pkg/nimble/README.ipv6-over-ble.md).
