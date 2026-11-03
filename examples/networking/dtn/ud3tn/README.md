# uD3TN RIOT example

This example runs the RIOT port of
[uD3TN](https://gitlab.com/d3tn/ud3tn), a Bundle Protocol implementation for
delay-tolerant networking.

The example configures a TCPCLv3 contact and starts a small chat agent. The
node configuration is selected with `NODE_A`, `NODE_B`, or `NODE_C` in the
Makefile. Before using multiple nodes, adapt the IPv6 addresses and ports in
`main.c` to the addresses assigned to the connected interfaces.

Before using the example, the IPv6 addresses in `main.c` must be adapted to the
actual addresses of the connected nodes. Build and flash each node first, start
the RIOT shell, and run `ifconfig` to determine the current IPv6 address of its
network interface. Then update the corresponding `LOCAL_IP`, `PEER_IP`, and,
where applicable, `PEER_IP_C` definitions in `main.c`. Rebuild and flash the
nodes after changing these values. The addresses must be checked again whenever
the network interface or its EUI-64 identifier changes.

## Native

Create TAP interfaces from the RIOT root directory:

```shell
sudo ./dist/tools/tapsetup/tapsetup -c 3
```

Build and start a node:

```shell
make BOARD=native PORT=tap0
make BOARD=native PORT=tap0 term
```

For multiple nodes, use a separate `PORT` and `BINDIRBASE` for each node and
select a different node definition.
'''shell
PORT=tap0 CFLAGS="-DNODE_A" BINDIRBASE=/tmp/ud3tn-node-a  make BOARD=native
'''
The RIOT package downloads the pinned uD3TN revision and applies its patch
automatically.

## ESP32

The example supports `esp32-wroom-32`. Set `ESP_WIFI_SSID` and
`ESP_WIFI_PASS` through a local build override or adapt the example Makefile.

1. Installing the ESP32 toolchain `https://api.riot-os.org/group__cpu__esp32.html#esp32_riot`

2. Building for esp32-wroom-32

```
CFLAGS="-DNODE_A_ESP32" BINDIRBASE=/tmp/ud3tn-node-a make BOARD=esp32-wroom-32
```

3. Flashing
```
PORT=/dev/ttyUSB0 CFLAGS="-DNODE_A_ESP32" BINDIRBASE=/tmp/ud3tn-node-a make BOARD=esp32-wroom-32 flash term
```
- To find out the port, you can run `make list-ttys`. All connected USB devices should be listed here.
- If you're using WSL 2, you can use `usbipd` to forward the USB devices to the VM. Instructions are linked in the references.