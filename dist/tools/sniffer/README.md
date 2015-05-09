Using Wireshark to sniff 802.15.4/Zigbee/6lowpan traffic:
=========================================================

Software
--------

In order to sniff any radio packets use the sniffer application from the in the
RIOT applications repository (https://github.com/RIOT-OS/applications/).

Generating a .pcap
------------------

The next step is to run the control python script, rftestrx2pcap.py,
(part of RIOT). This script will set the channel on the node and then create a
.pcap with the packets received. The script is a modified version of [malvira's
script](https://github.com/malvira/libmc1322x/blob/master/tools/rftestrx2pcap.py)
for the Redbee Ecotag (https://github.com/malvira/libmc1322x/wiki/wireshark).

### Download rftestrx2pcap.py

[rftestrx2pcap.py](rftestrx2pcap.py)

### Install dependencies

Needs [pyserial](https://pypi.python.org/pypi/pyserial)

#### Debuntu
    apt-get install python-serial

#### PIP
    pip install pyserial

### Usage

    ./rftestrx2pcap.py tty chan [outfile]

rftestrx2pcap.py takes the tty/com port as the first argument and the radio
channel as the second (from 0 through 24). The third argument is an optional
output file.

#### Linux

Dump packets to a file:

    $ ./rftestrx2pcap.py /dev/ttyUSB1 10 > foo.pcap

This .pcap can then be opened in wireshark.Alternatively for live
captures, you can pipe directly into wireshark with:

    $ ./rftestrx2pcap.py /dev/ttyUSB1 10 | wireshark -k -i -

#### Windows

For windows you can use the optional third argument to output to a
.pcap:

    $ ./rftestrx2pcap.py /dev/ttyUSB1 26 foo.pcap
