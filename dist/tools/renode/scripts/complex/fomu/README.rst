Description
-----------

This is a demo showcasing hybrid emulation where part of the platform is emulated in Renode and part is run on an actual hardware.

Architecture
------------

The system architecture is as follows:

[Renode] <-TCP-> [litex_server] <-USB-> [FOMU]

(1) Renode runs the emulation of LiteX+VexRiscv with an additional "led" peripheral of EtherBoneBridge type.
(2) There is litex_server running on the host machine (outside the emulation).
(3) The actual LED peripheral is located on the FOMU board.

Renode communicates over sockets with litex_server using the EtherBone protocol.
litex_server talks over USB with FOMU programmed with a bitstream containing the ValentyUSB IPCore with a USB-wishbone bridge.

How to run it?
--------------

0. Clone LiteX and Migen (required to start litex_server)::

    git clone https://github.com/enjoy-digital/litex.git
    git clone https://github.com/m-labs/migen.git

    export PYTHONPATH=`pwd`/litex:`pwd`/migen

1. Attach FOMU to a USB port and verify in dmesg that it's recognized::

    [3265038.250957] usb 2-1: new full-speed USB device number 16 using xhci_hcd
    [3265038.409283] usb 2-1: New USB device found, idVendor=1209, idProduct=5bf0, bcdDevice= 1.01
    [3265038.409286] usb 2-1: New USB device strings: Mfr=1, Product=2, SerialNumber=0
    [3265038.409287] usb 2-1: Product: Fomu DFU Bootloader v1.7.2-3-g9013054
    [3265038.409288] usb 2-1: Manufacturer: Foosn

2. Start litex_server.py (shipped together with LiteX available at https://github.com/enjoy-digital/litex.git)::

    cd litex
    (sudo) python3 litex/tools/litex_server.py --usb --usb-vid 0x1209 --usb-pid 0x5bf0

   This will start EtherBone server on 127.0.0.1:1234 by default. If you want to change the port, use --bin-port switch.

   Note: To allow non-root users to access USB Fomu device, install an udev rule in your system::

    sudo cp 99-fomu.rules /etc/udev/rules.d
    sudo udevadm control --reload

3. Start Renode and run 'renode_etherbone_fomu.resc' script.

4. Once started use 'led_toggle' or 'led_breathe' commands in Zephyr's shell to control HW led from inside of the emulation!

