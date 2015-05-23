Creating a SLIP network interface
=================================

The module `ng_slip` (Serial line IP) enables the RIOT network stack to
communicate IP packets over the serial interface. This collection of tools
originally from Contiki [1] enables Linux to interpret this data. Though there
is a tool for such operations on Linux (`slattach`) it is only able to handle
IPv4 packages and is unnessarily complicated.

# Installation
Just install them using

``` {.sh}
make
sudo make install
```

By default they are installed to the `/usr/local/bin` directory, you can however
change that by setting the PREFIX environment variable

``` {.sh}
export PREFIX=${HOME}/.local
make
sudo make install
```

# Usage
`tapslip6` allows you to open a TAP interface (includes link-layer data) for
a serial interace handling IPv6 data,
`tunslip` allows you to open a TUN interface (includes only network-layer data)
for a serial interace handling IPv4 data, and
`tunslip6` allows you to open a TUN interface (includes only network-layer data)
for a serial interace handling IPv6 data.

For more information use the help feature of the tools

``` {.sh}
tapslip -h
tunslip -h
tunslip6 -h
```

[1] https://github.com/contiki-os/contiki/tree/a4206273a5a491949f9e565e343f31908173c998/tools
