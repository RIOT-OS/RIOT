# candev test application

## About

This application is a test for using the candev abstraction directly.
Use this if you want to use a single CAN driver and thus don't need the CAN-DLL layer.

The CAN_DRIVER variable is used to select the default CAN_DRIVER, supported
alternatives are:

- MCP2515 to use `mcp2515` stand-alone CAN controller
- PERIPH_CAN to use `periph_can` controller, usually requires a CAN transceiver
  as well: e.g. `tja1042` or `ncv7356` (except for native)

## Usage

### Sending

Messages can be sent over the CAN-bus through the `send` command. Optionally, up to 8 bytes can be passed as arguments (in decimal form). If no arguments are passed it will default to sending AB CD EF (hex).

```shell
> send <bytes>
# e.g.: send AA BB CC
> send 170 187 204
send 170 187 204
```

### Receiving

The test-app is always listening for incoming CAN messages. They will be stored asynchronously in a buffer and can be requested by means of the `receive` command. Optionally, an argument n can be passed to receive n messages in a row.

```shell
> receive <n>
# e.g.:
> receive 2
Reading from Rxbuf...
id: 1 dlc: 3 Data:
0xAA 0xBB 0xCC
Reading from Rxbuf...
id: 1 dlc: 3 Data:
0xAA 0xBB 0xCC
```

## Native Setup

Refer to [README.native.can.md](README.native.can.md).
