Test description
==========
The testsuite tests the GNRC TCP integration into the SOCK TCP interface.
The tests offer only basic verification of the SOCK TCP interface since GNRC TCP aims
follow the SOCK TCP interface as close as possible, detailed tests are under tests/gnrc_tcp

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
