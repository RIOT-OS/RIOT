Test description
==========
The GNRC TCP test test all phases of a tcp connections lifecycle as a server or a client
as well as TCP behavior on incoming malformed packets.

Setup
==========
The test requires a tap-device setup. This can be achieved by running 'dist/tools/tapsetup/tapsetup'
or by executing the following commands:

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up

Usage
==========
    make BOARD=<BOARD_NAME> all flash
    sudo make BOARD=<BOARD_NAME> test-as-root

'sudo' is required due to ethos and raw socket usage.
