Test description
==========
The entire GNRC TCP test consists of several test cases. Each test is ran via its own python script
in the tests directory.

1) 01-conn_lifecycle_as_client.py
    This test covers TCP connection establishment and teardown with GNRC_TCP acting as tcp client.

2) 02-conn_lifecycle_as_server.py
    This test covers TCP connection establishment and teardown with GNRC_TCP acting as tcp server.

3) 03-send_data.py
    This test covers sending of a byte stream from GNRC_TCP to the host system.
    The amount of data to send is large enough to force GNRC_TCP to split the given stream into
    multiple packets.

4) 04-receive_data.py
    This test covers receiving of a byte stream from the host system. The received data is
    causing window opening and closing as well as data transmission over multiple packets.

5) 05-garbage-pkts.py
    This test mostly is a regression test for issues that were found through fuzzing. It uses
    `scapy` to interact with the node.

Setup
==========
The test requires a tap-device setup. This can be achieved by running 'dist/tools/tapsetup/tapsetup'
or by executing the following commands:

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up

Usage
==========
    make BOARD=<BOARD_NAME> all flash
    sudo make BOARD=<BOARD_NAME> test

'sudo' is required due to ethos and raw socket usage.
