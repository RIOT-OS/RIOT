Tests for stm32 Ethernet periph driver
======================================

This tests the stm32 Ethernet driver using lwIP and IPv4. It implements a very
simple UDP echo server at the configured address (by default 172.16.19.5) and 
port `12345`.

To interface with the board configure the local IP address of the available 
interface (e.g. eth1) with the gateway address (by default 172.16.19.1)
and execute a tool such as netcat to send UPD messages to the stm32.

```sh
# Configure IP on the available interface
ifconfig eth1 172.16.19.1/24

# Start netcat
nc -u 172.16.19.5 12345
```

The board will reply all the messages sent to it (with a maximum length of 128
bytes). 
