# libCSP test application

This application allows basic testing of the LibCSP package port. It requires
a baord with a CAN-BUS interface, this can be implemented by the `periph_can`
`FEATURE` (although often a CAN transceiver is required as well, e.g. `tja1042`)
or by a stand-alone CAN-controller, e.g. `mcp2515`.

By default this application will use the `periph_can` `FEATURE` for boards
providing and the `mcp2515` module for those that do not.

## Native Setup


## Usage

- interface information

```
> csp
can0       addr: 23:A5 netmask: 4 mtu: 119
           tx: 00000 rx: 00000 txe: 00000 rxe: 00000
           drop: 00000 autherr: 00000 frame: 00000
           txb: 0 (0B) rxb: 0 (0B)
```

this can be useful to get the device addr.

- sending

```
> csp send <addr> <port> <message>
```

- receiving

To receive start a server listening on a given port:

```
> csp server start <port>
```

## Auto test

The auto-test will on a single device send a string to itself. This requires
a device with a valid can interface, for native see the requirements above.

```
make -c tests/pkg_libcsp flash test-with-config
```
