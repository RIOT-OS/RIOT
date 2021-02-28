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

6) 06-receive_data_closed_conn.py
    This test covers accessing received data after receiving a FIN packet. If the connection was closed
    by the peer, a call to gnrc_tcp_recv must return directly with all currently received data
    or zero if there is no data. The function must return immediately despite any given timeout.

7) 07-endpoint_construction.py
    This test ensures the correctness of the endpoint construction.

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
