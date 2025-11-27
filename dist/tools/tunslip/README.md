# Creating a SLIP network interface

The module `slipmux_net` (Serial line IP Multiplexing) enables the RIOT network
stack to send and receive IP packets over a serial interface. This collection
of tools originally from Contiki [1] enables Linux to interpret this data.
Though there is a tool for such operations on Linux (`slattach`) it is only
able to handle IPv4 packages and is unnecessarily complicated.

## Installation

Just install them using

``` {.sh}
make
sudo make install
```

By default they are installed to the `/usr/local/bin` directory, you
can however change that by setting the `PREFIX` environment variable:

``` {.sh}
export PREFIX=${HOME}/.local
make
sudo make install
```

## Usage

- `tapslip6` opens a TAP interface (includes link-layer data) for
  a serial interface handling IPv6 packets,
- `tunslip` opens a TUN interface (includes network-layer data)
  for a serial interface handling IPv4 packets, and
- `tunslip6` opens a TUN interface (includes network-layer data)
  for a serial interface handling IPv6 packets.

For more information use the help feature of the tools:

``` {.sh}
tapslip -h
tunslip -h
tunslip6 -h
```

[1] https://github.com/contiki-os/contiki/tree/a4206273a5a491949f9e565e343f31908173c998/tools
