# nrf_sf_radio tests

## About

This directory contains an automated single-board test and a manual two-board
communication test for the `nrf_sf_radio` module.

The single-board test verifies:

- time conversion macros
- default RADIO initialization
- PHY mode, BLE channel, and transmit-power configuration
- the free-running timer and relative wait function
- scheduled transmission and READY, ADDRESS, and END timestamps
- scheduled receive timeout handling and buffer-pointer preservation

The manual test uses two nRF52 boards and consists of two applications:

- `receiver` continuously receives packets and verifies that each payload is
  `"hello, world"`.
- `transmitter` continuously schedules transmissions of `"hello, world"`.

Both applications report their status every 1000 successfully processed
packets so that the output remains visible when a serial terminal is attached
after the application has started.

The two-board test covers packet-header encoding, packet reception, CRC
validation, payload access, and verification of the received bytes.

## Single-board test

The automated test requires one supported nRF52 board. No other node using the
same radio configuration should transmit while this test is running, because
one check expects a scheduled reception to time out.

Build, flash, and run the test from the RIOT repository root:

```shell
make -C tests/sys/nrf_sf_radio flash test
```

The default board is `adafruit-feather-nrf52840-sense`. A different supported
board can be selected with `BOARD=<board>`.

A successful run ends with:

```text
[SUCCESS]
```

Each failed check is printed with a `[FAILED]` prefix, and the application
returns a nonzero status.

## Two-board test

### Hardware requirements

- Two boards based on a supported nRF52 MCU
- One USB connection and serial terminal for each board
- Both boards within radio range of each other

The default board is `adafruit-feather-nrf52840-sense`. A different supported
board can be selected with `BOARD=<board>`.

### Building

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

Both applications use BLE channel 20 and the test Access Address
`0x8367BED6`. Any configuration changes must be applied to both applications.

### Running the test

Connect both boards and identify their serial ports. The commands below use
`/dev/ttyACM0` for the transmitter and `/dev/ttyACM1` for the receiver.

1. In the first terminal, flash and start the transmitter:

   ```shell
   make -C tests/sys/nrf_sf_radio/transmitter \
       BOARD=adafruit-feather-nrf52840-sense \
       PORT=/dev/ttyACM0 flash term
   ```

2. Use a second terminal to flash and start the receiver:

   ```shell
   make -C tests/sys/nrf_sf_radio/receiver \
       BOARD=adafruit-feather-nrf52840-sense \
       PORT=/dev/ttyACM1 flash term
   ```

Adjust `PORT` for the device names assigned by the host. Keep the transmitter
running until the receiver reports `[SUCCESS]`. When multiple UF2 bootloader
volumes are mounted, also set `UF2_MOUNTPOINT` to the intended board's mount
point during `flash`.

### Expected result

After every 1000 successful transmissions, the transmitter prints:

```text
[OK] transmitter sending
```

After every 1000 received and validated payloads, the receiver prints:

```text
[OK] received expected payload
[SUCCESS]
```

The test succeeds when the receiver prints `[SUCCESS]`. A line starting with
`[FAILED]` indicates either a transmission failure or an unexpected payload.
Both applications continue running until their boards are reset or powered
off.

### Automation

The two-board test is manual because it requires two independently flashed
boards and two serial ports. Only the single-board test is executed through
the standard `make test` workflow.
