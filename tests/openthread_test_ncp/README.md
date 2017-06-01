## OpenThread on RIOT

This test demonstrates how to use NEST's OpenThread stack from [OpenThread](https://github.com/openthread/openthread) on
RIOT. The [Command Line Interface](https://github.com/openthread/openthread/blob/master/examples/apps/cli/README.md) of
OpenThread was ported. Please check the [full
documentation](https://github.com/openthread/openthread/blob/master/src/cli/README.md) of the CLI for usage information.

This test show how to use a Network Co-Processor (NCP). A Network Co-Processor is used with wpantund software.
Wpantund is a user-space network interface driver/daemon that provides a native IPv6 network interface to a low-power
wireless Network Co-Processor. NCP and wpantund communicates by UART (UART(0) is used here)

## Quick usage

To test OpenThread NCP in RIOT, you can do the following:

1. Flash nodes with `make BOARD=<target> clean all flash`
2. Install wpantund: On Ubuntu 16.04, run
```
sudo apt-get -y install build-essential subversion libncurses5-dev libssl-dev zlib1g-dev gawk gcc-multilib flex git-core gettext gcc binutils bzip2 python perl make unzip libz-dev tftp git shtool autogen automake libtool autotools-dev libdbus-1-dev
git clone https://github.com/openthread/wpantund.git
cd wpantund
./bootstrap.sh
./configure
make
sudo make install
```

3) Start wpantund on your host.  Be sure to pass flags appropriate to connect wpantund to your NCP connection type.
You can also name the network interface at this time or let wpantund automatically assign a name (wpan0):

`sudo /usr/local/sbin/wpantund -s /dev/ttyACM0 -I wpan0 -o SyslogMask all`

4) Start wpanctl to access the command line management interface for controlling Thread features on your device.
This is similar to the Thread cli interface conceptually but with slightly different command syntax:

`sudo /usr/local/bin/wpanctl -I wpan0`

5) Confirm your network interface has started and is operational by running standard IP commands such `ifconfig` and `ping6`.


## OpenThread port on RIOT status

OpenThread port on RIOT is still under development. In case of any bug, please report via GitHub issue.


## Known issues

* When the network traffic is really high, sometimes some nodes crash silently.
* In some cases there are isolated leaders that are not able to recover to the main partition. These won't affect the
  rest of the network, but this node becomes unreachable.
