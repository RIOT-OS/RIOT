# TinyDTLS example

This application demonstrates how to use TinyDTLS
(http://sourceforge.net/p/tinydtls/code/ci/master/tree/) as a package in RIOT.
The example allows to start a DTLS server, connect to a DTLS server, and
exchange encrypted messages between two peers. It provides three shell commands
to serve these purposes:
* `dtlsserver`      - To start a DTLS server
* `dtls <IP>`       - To connect to a DTLS server with IPv6 address <IP>
* `dtlssend <TEXT>` - To send encrypted <TEXT> to a connected remote host
