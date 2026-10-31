# About

This is a manual test application for the Epson ESC/POS receipt printer
driver. It provides shell commands to print text, change the text style, cut
the paper and query the printer status.

# Usage

The transport is selected with `ESCPOS_TRANSPORT` (`tcp`, `uart` or `dummy`,
default: `tcp`).

## TCP

Epson network printers only support IPv4, so the application uses `lwip_ipv4`.
The address of the printer is set with `ESCPOS_HOST` and can also be changed at
runtime with `open <host>[:<port>]`.

By default a static IPv4 configuration is applied at startup
(`ESCPOS_IPV4_ADDR`, `ESCPOS_IPV4_NETMASK`, `ESCPOS_IPV4_GW`). With
`ESCPOS_DHCP=1` the address is obtained via DHCP instead.

### native

On `native`, the printer is reached through a TAP interface. The host has to
forward and NAT the traffic of the TAP interface to its uplink (`eth0` here):

```
sudo ip tuntap add dev tap0 mode tap user $USER
sudo ip addr add 10.42.0.1/24 dev tap0
sudo ip link set tap0 up
sudo sysctl -w net.ipv4.ip_forward=1
sudo iptables -t nat -A POSTROUTING -s 10.42.0.0/24 -o eth0 -j MASQUERADE
sudo iptables -I FORWARD -i tap0 -o eth0 -j ACCEPT
sudo iptables -I FORWARD -i eth0 -o tap0 -m state --state RELATED,ESTABLISHED -j ACCEPT
```

Then run the application with the default static configuration
(10.42.0.2/24, gateway 10.42.0.1):

```
make BOARD=native ESCPOS_HOST=192.168.178.59 all term
```

## UART

The UART device and baud rate are set with `EPSON_ESCPOS_PARAM_UART_DEV` and
`EPSON_ESCPOS_PARAM_UART_BAUDRATE`. If the BUSY line of the printer is
connected to a GPIO, set `EPSON_ESCPOS_PARAM_UART_BUSY_PIN` to enable flow
control.

```
make BOARD=<board> ESCPOS_TRANSPORT=uart flash term
```

## Example

```
> status
printer is online, paper: ok
> print Hello RIOT!
> style size 2x
> print BIG
> style reset
> cut
> demo
```
