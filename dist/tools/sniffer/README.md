# RIOT Sniffer Application


## About

This sniffer script can be used to sniff network traffic using RIOT based
nodes. It is primarily designed for sniffing wireless data traffic, but can also
well be used for wired network traffic, as long as the used network devices
support promiscuous mode and output of raw data.

The sniffer is based on a RIOT node running the [sniffer application](https://github.com/RIOT-OS/applications/tree/master/sniffer) application located in [RIOTs application repository](https://github.com/RIOT-OS/applications).
This node outputs received network traffic via a serial port in the Wireshark
pcap format. This output is then parsed by the `sniffer.py` script included
in this folder run on a host computer.

The `sniffer.py` script is a modified version of [malvira's script](https://github.com/malvira/libmc1322x/blob/master/tools/rftestrx2pcap.py) for the Redbee Ecotag
(https://github.com/malvira/libmc1322x/wiki/wireshark).


## Dependencies

The `sniffer.py` script needs [pyserial](https://pypi.python.org/pypi/pyserial).

Installing the dependencies:

#### Debuntu
    apt-get install python-serial

#### PIP
    pip install pyserial


## Usage

General usage:

1. Flash an applicable RIOT node with the sniffer application from
(https://github.com/RIOT-OS/applications/tree/master/sniffer)

2. Run the `sniffer.py` script
```
$ ./sniffer.py <tty> <baudrate> <channel> [outfile]
```
The script has the following parameters:

- **tty:** The serial port the RIOT board is connected to. Under Linux, this is
           typically something like /dev/ttyUSB0 or /dev/ttyACM0. Under Windows,
           this is typically something like COM0 or COM1
- **baudrate:** The baudrate the serial port is configured to. The default in
                RIOT is 115200, though this is defined per board and some boards
                have some other value defined per default. NOTE: when sniffing
                networks where the on-air bitrate is > baudrate, it makes sense
                to increase the baudrate so no data is skipped when sniffing.
- **channel:** The radio channel to use when sniffing. Possible values vary and
               depend on the link-layer that is sniffed. This parameter is
               ignored when sniffing wired networks.
- **[outfile]:** When this parameter is specified, the sniffer output is saved
               into this file. See the examples below for alternatives to
               specifying this parameter. (optional)


### Examples

The following examples are made when using the sniffer application together with
an `iotlab_m3` node that is connected to /dev/ttyUSB1 (or COM1) and runs per
default with a baudrate of 500000.

#### Linux

Dump packets to a file:
```
$ ./sniffer.py /dev/ttyUSB1 500000 17 > foo.pcap
```

This .pcap can then be opened in wireshark.

Alternatively for live captures, you can pipe directly into wireshark with:
```
$ ./sniffer.py /dev/ttyUSB1 500000 17 | wireshark -k -i -
```

#### Windows

For windows you can use the optional third argument to output to a
.pcap:

```
$ ./sniffer.py COM1 500000 17 foo.pcap
```
