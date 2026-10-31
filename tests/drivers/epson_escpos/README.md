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

## Printer profiles

The driver uses a printer profile to know the capabilities of the printer,
e.g. the number of characters per line or the supported cut modes. By default,
the profile of the TM-T88IV is used, another profile can be selected with
`EPSON_ESCPOS_PARAM_PROFILE`:

```
CFLAGS += '-DEPSON_ESCPOS_PARAM_PROFILE=(&epson_escpos_profile_tm_t88iii)'
```

The profiles are taken from the
[ESC/POS printer database](https://github.com/receipt-print-hq/escpos-printer-db),
which is also used by python-escpos. Only the profiles of printers that were
tested with the driver are included:

| Profile                          | Printer         |
|----------------------------------|-----------------|
| `epson_escpos_profile_tm_t88iii` | Epson TM-T88III |
| `epson_escpos_profile_tm_t88iv`  | Epson TM-T88IV  |

### Adding a profile

1. Copy the profile of the printer from `data/profile/` of the printer
   database to `drivers/epson_escpos/contrib/`. Keep the file unchanged and
   only add the SPDX header and the source URL (with the commit of the
   database) at the top, like in the existing files. If the profile inherits
   from another profile (`inherits:`), that profile has to be copied as well.
   `default.yml` is the base of most profiles and defines the features, it is
   only used for inheritance and is not available as a profile itself.
2. Run `./gen_profiles.py` in `drivers/epson_escpos/` (requires PyYAML). It
   resolves the inheritance in the same way as the printer database and
   regenerates `epson_escpos_profiles.c` and `epson_escpos_profiles.h` in
   `drivers/epson_escpos/generated/`.
   Never edit these two files by hand.
3. Commit the YAML file together with the generated files. The new profile is
   available as `epson_escpos_profile_<name>`, where `<name>` is the profile
   name in lower case with all other characters replaced by `_`
   (e.g. `TM-T88IV` becomes `tm_t88iv`).

If the printer is not in the database, a profile can be written in the same
format. Consider adding it to the printer database as well, see
[How to get your printer included in the database](https://github.com/receipt-print-hq/escpos-printer-db/blob/master/doc/add-your-printer.md).

The profiles are licensed under
[CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) by the contributors
of the printer database.

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
