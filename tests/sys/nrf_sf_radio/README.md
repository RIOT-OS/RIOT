# nrf_sf_radio two-board test

## About

This test verifies basic communication through the `nrf_sf_radio` module. It
uses two nRF52 boards and consists of two applications:

- `receiver` starts the radio, waits for a valid packet, prints its payload,
  and exits.
- `transmitter` repeatedly schedules the transmission of
  `"hello, world"` and reports whether each transmission completed.

The test covers radio initialization, scheduled transmission, packet-header
encoding, packet reception, CRC validation, and access to the received
application payload.

## Hardware requirements

- Two boards based on a supported nRF52 MCU
- One USB connection and serial terminal for each board
- Both boards within radio range of each other

The default board is `adafruit-feather-nrf52840-sense`. A different supported
board can be selected with `BOARD=<board>`.

## Building

Run the following commands from the RIOT repository root:

```shell
make -C tests/sys/nrf_sf_radio/receiver clean all
make -C tests/sys/nrf_sf_radio/transmitter clean all
```

To build for another board, pass the same board to both applications:

```shell
make -C tests/sys/nrf_sf_radio/receiver \
    BOARD=nrf52840dk clean all
make -C tests/sys/nrf_sf_radio/transmitter \
    BOARD=nrf52840dk clean all
```

Both applications must use the same radio configuration.

## Running the test

1. Flash the receiver application onto the first board:

   ```shell
   make -C tests/sys/nrf_sf_radio/receiver \
       BOARD=adafruit-feather-nrf52840-sense flash
   ```

2. Open a terminal for the receiver board. Set `PORT` if the board is not
   selected automatically:

   ```shell
   make -C tests/sys/nrf_sf_radio/receiver \
       BOARD=adafruit-feather-nrf52840-sense \
       PORT=/dev/ttyACM0 term
   ```

3. Flash the transmitter application onto the second board:

   ```shell
   make -C tests/sys/nrf_sf_radio/transmitter \
       BOARD=adafruit-feather-nrf52840-sense flash
   ```

4. Open another terminal for the transmitter board:

   ```shell
   make -C tests/sys/nrf_sf_radio/transmitter \
       BOARD=adafruit-feather-nrf52840-sense \
       PORT=/dev/ttyACM1 term
   ```

## Expected result

The transmitter repeatedly prints:

```text
TX SUCCESSFUL
```

The receiver prints the received payload once:

```text
hello, world
```

The test succeeds when the receiver prints the expected payload and the
transmitter reports a successful transmission. `TX FAILED` indicates that the
transmission did not complete before its configured deadline.

## Automation

This is a manual hardware test. It requires two independently flashed boards
and two serial ports, so it cannot be executed by the standard single-board
`make test` workflow.
