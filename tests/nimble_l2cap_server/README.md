# About

This application acts as a L2CAP echo server for testing and benchmarking raw
L2CAP connection oriented channel (COC) connections using NimBLE on RIOT. This
server application works in conjunction with the `nimble_l2cap` client test
application in `tests/nimble_l2cap`.

Next to the actual L2CAP server implementation, this folder also holds some
shared configuration for both the server and the client test applications.


# Usage

To run the L2CAP test suite, you need to have two BLE (NimBLE) capable boards in
radio range of each other available. Simple flash the server application on one
board and the test application on the other one. The applications are build in a
way, that they will automatically discover each other and open a dedicated
L2CAP channel connection that is used for all subsequent test.

The server will print something like
```
2019-03-21 21:27:12,012 - INFO # main(): This is RIOT! (Version: XX)
2019-03-21 21:27:12,012 - INFO # NimBLE L2CAP test server
2019-03-21 21:27:12,013 - INFO # # now advertising
[pausing here until the client is detected]
2019-03-21 21:27:16,928 - INFO # # GAP event 0
2019-03-21 21:27:16,981 - INFO # # L2CAP: CONNECTED
2019-03-21 21:27:16,983 - INFO # # MTUs: our 250, remote 250
```

The clients corresponding output will look similar to
```
2019-03-21 21:27:16,837 - INFO # main(): This is RIOT! (Version: XX)
2019-03-21 21:27:16,839 - INFO # NimBLE L2CAP test application
2019-03-21 21:27:16,840 - INFO # # Scanning now
[pausing here until server is detected]
2019-03-21 21:27:16,930 - INFO # # Found Server, connecting now# GAP event: 0
2019-03-21 21:27:17,030 - INFO # # L2CAP: CONNECTED
2019-03-21 21:27:17,035 - INFO # # Connection established, running test suite now
2019-03-21 21:27:17,037 - INFO # # MTUs: our 250, remote 250
2019-03-21 21:27:17,039 - INFO # # Shell is now available
```

Once the connection between the client and server is established, you can use
the shell commands provided by the client application to stress the opened
L2CAP channel.

E.g. do something like:
```
flood 5000 10
```
to send 10 packets of 5000 bytes. The clients output should look like this:
```
flood 5000 10
2019-03-21 21:30:57,151 - INFO #  flood 5000 10
2019-03-21 21:30:57,154 - INFO # # Sending: size  5000 seq     1
2019-03-21 21:30:57,159 - INFO # # Sending: size  5000 seq     2
2019-03-21 21:30:57,688 - INFO # # Sending: size  5000 seq     3
2019-03-21 21:30:58,688 - INFO # # Sending: size  5000 seq     4
2019-03-21 21:30:59,688 - INFO # # Sending: size  5000 seq     5
2019-03-21 21:31:00,688 - INFO # # Sending: size  5000 seq     6
2019-03-21 21:31:01,687 - INFO # # Sending: size  5000 seq     7
2019-03-21 21:31:02,688 - INFO # # Sending: size  5000 seq     8
2019-03-21 21:31:03,688 - INFO # # Sending: size  5000 seq     9
2019-03-21 21:31:04,688 - INFO # # Sending: size  5000 seq    10
2019-03-21 21:31:07,187 - INFO # # TEST COMPLETE
2019-03-21 21:31:07,189 - INFO # -> runtime: 10082ms
2019-03-21 21:31:07,190 - INFO # -> ~ 5455 bytes/s
```
