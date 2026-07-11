# nrf_sf_radio

## Overview

`nrf_sf_radio` provides low-level access to the nRF52 RADIO peripheral for time-slotted protocols such as synchronous flooding. It uses PPI to trigger RADIO startup at absolute timer deadlines and records hardware timestamps for the READY, ADDRESS, and END events.

The module also provides a lightweight BLE-like link layer. It prepends an S0 field, a LENGTH field, and a six-byte advertising address to the application payload, and then uses the nRF52 BLE 1 Mbit/s radio format with data whitening and a three-byte CRC.

This module is not a BLE stack. It does not provide advertising procedures, connections, channel hopping, retransmission policies, or protocol state management. The application is responsible for defining slots, selecting deadlines, handling timeouts, and interpreting payloads. The module is intended for experiments that require direct and predictable TIMER/PPI control and is not a replacement for NimBLE or the RIOT nrfble network driver.

The implementation is limited to nRF52 devices equipped with the RADIO, TIMER2, TIMER3, and PPI peripherals.

## Enabling the module

Add the module to the application Makefile:

```make
USEMODULE += nrf_sf_radio
```

Include the physical-layer and link-layer interfaces as needed:

```c
#include "nrf_sf_radio/link_radio.h"
#include "nrf_sf_radio/radio_driver.h"
```

Call `nrf_sf_radio_start()` once before using any other module function:

```c
nrf_sf_radio_start();
```

Initialization powers the RADIO peripheral, requests the high-frequency
crystal oscillator, starts TIMER3, and configures TIMER2 and the required PPI
channels. Runtime radio configuration functions must be called after
`nrf_sf_radio_start()`, because initialization installs the default values.

## Basic usage

### Scheduled transmission

`nrf_sf_radio_tx_start()` takes an absolute tick for starting TX ramp-up and an
absolute deadline by which the END event is expected:

```c
uint8_t payload[] = "hello, world";
uint32_t now = nrf_sf_radio_now_ticks();
uint32_t txen_ticks = now + NRF_SF_RADIO_US_TO_TIMER_TICKS(100U);
uint32_t end_ticks = now + NRF_SF_RADIO_US_TO_TIMER_TICKS(600U);

bool sent = nrf_sf_radio_tx_start(payload, txen_ticks, end_ticks,
                                  sizeof(payload));
```

The END deadline must include the scheduled delay, radio ramp-up, complete
packet airtime, and a suitable timing margin.

### Scheduled reception

`nrf_sf_radio_rx_start()` changes the supplied pointer to point past the
link-layer header when a valid packet is received:

```c
uint8_t packet[64] = { 0 };
uint8_t *payload = packet;

uint8_t status = nrf_sf_radio_rx_start(&payload, rxen_ticks,
                                       address_deadline_ticks,
                                       end_deadline_ticks);
if (status == 0U) {
    /* payload now points to the application payload in packet. */
}
```

The pointer passed to the function must initially point to the beginning of
the receive buffer. Reset it before every new reception:

```c
payload = packet;
```

The status values are:

| Value | Meaning |
|------:|---------|
| `0` | Valid packet received |
| `1` | ADDRESS event timeout |
| `2` | END event timeout |
| `3` | CRC check failed |

### Immediate reception

`nrf_sf_radio_rx_listen_until_packet()` starts reception immediately. Its END
timeout is specified in timer ticks, while its ADDRESS timeout is specified in
microseconds:

```c
uint8_t packet[64] = { 0 };
uint8_t *payload = packet;

uint32_t address_ticks = nrf_sf_radio_rx_listen_until_packet(
    &payload,
    NRF_SF_RADIO_US_TO_TIMER_TICKS(500U),
    1000U);
```

On success, the function returns the corrected transmitter-side ADDRESS timestamp and updates the `payload` pointer. It returns zero on timeout or CRC failure.

## Packet format

The link layer places the following fields in the RADIO packet buffer:

| Field | Length |
|-------|-------:|
| S0 | 1 byte |
| LENGTH | 1 byte |
| Advertising address | 6 bytes |
| Application payload | 0 to 245 bytes |

`NRF_SF_RADIO_HDR_LEN` is therefore eight bytes. The nRF52 packet
configuration permits at most 251 bytes after the LENGTH field. Six of those
bytes are used by the advertising address, leaving at most 245 application
payload bytes. A receive buffer for the maximum packet requires 253 bytes:
one S0 byte, one LENGTH byte, and 251 following bytes.

The preamble, four-byte radio access address, and three-byte CRC are handled by
the RADIO peripheral and are not part of `NRF_SF_RADIO_HDR_LEN`.

## Configuration

### Compile-time configuration

The following macros can be defined through an application's `CFLAGS` before
building the module:

| Macro | Default | Description |
|-------|---------|-------------|
| `NRF_SF_RADIO_ACCESS_ADDRESS` | `0x8E89BED6U` | Four-byte RADIO access address |
| `NRF_SF_RADIO_FAST_RAMPUP` | `1` | Select fast (`1`) or default (`0`) ramp-up |

For example, an application Makefile can select a custom access address and
the default 140 us ramp-up mode as follows:

```make
CFLAGS += -DNRF_SF_RADIO_ACCESS_ADDRESS=0x8367BED6U
CFLAGS += -DNRF_SF_RADIO_FAST_RAMPUP=0
```

Fast ramp-up uses 40 us and default ramp-up uses 140 us. Values other than
`0` and `1` for `NRF_SF_RADIO_FAST_RAMPUP` cause a compile-time error.

All communicating nodes must use the same access address, channel, PHY mode,
and timing configuration. The default access address is the standard BLE
advertising access address. A custom value is recommended when unrelated BLE
advertising traffic must not be accepted by the RADIO hardware.

### Runtime configuration

The following functions change the RADIO configuration after initialization:

| Function | Description |
|----------|-------------|
| `nrf_sf_radio_set_mode()` | Sets the raw nRF52 RADIO MODE register value |
| `nrf_sf_radio_set_ble_channel()` | Selects BLE channel 0 through 39 |
| `nrf_sf_radio_set_power()` | Selects transmit power from a value in dBm |

The default runtime configuration is BLE 1 Mbit/s mode, BLE advertising
channel 37, and 0 dBm transmit power.

`nrf_sf_radio_set_ble_channel()` does not perform bounds checking. The caller
must pass a channel in the range 0 through 39. 

Transmit power is quantized by `nrf_sf_radio_set_power()` to one of
+4, 0, -4, -8, -12, -16, -20, or -30 dBm.

Changing the radio mode does not automatically update packet timing, preamble
configuration, whitening, or CRC settings. Applications using a mode other
than BLE 1 Mbit/s must ensure that all associated configuration and timing
assumptions remain valid.

## Hardware resource usage

The module directly configures and exclusively uses two hardware timers:

| Resource | Usage |
|----------|-------|
| TIMER3 | Free-running 16 MHz, 32-bit absolute time base and radio scheduler |
| TIMER2 | 16 MHz, 32-bit relative ramp-up timer that triggers RADIO START |

TIMER3 compare/capture registers are allocated as follows:

| Register | Usage |
|----------|-------|
| CC[0] | Absolute TXEN or RXEN scheduling deadline |
| CC[1] | ADDRESS event timestamp |
| CC[2] | Software-triggered current-time capture |
| CC[3] | END event timestamp |
| CC[4] | READY event timestamp |

The following PPI channels are configured directly:

| PPI channel | Event to task connection |
|------------:|--------------------------|
| 9 | RADIO READY to TIMER3 CAPTURE[4] |
| 10 | RADIO END to TIMER3 CAPTURE[3] |
| 11 | TIMER2 COMPARE[0] to RADIO START |
| 12 | TIMER3 COMPARE[0] to TIMER2 START |
| 13 | TIMER3 COMPARE[0] to RADIO RXEN |
| 14 | RADIO ADDRESS to TIMER3 CAPTURE[1] |
| 15 | TIMER3 COMPARE[0] to RADIO TXEN |

The module also takes direct ownership of the RADIO peripheral and requests
the HFXO through `clock_hfxo_request()`. It currently provides no deinitialize
function, so TIMER2, TIMER3, PPI channels 9 through 15, RADIO, and the HFXO
request remain allocated for the lifetime of the application.

Applications must not use this module together with another component that
configures the same timers, PPI channels, or RADIO peripheral. In particular,
it cannot concurrently share the RADIO with NimBLE or `nrfble`. Check the
board, timer, and peripheral configuration of every other enabled module for
resource conflicts.

## Timing and concurrency restrictions

- All timing uses a 16 MHz timer, giving 16 ticks per microsecond.
- TX/RX helper functions busy-wait for radio events or deadlines.
- Only one transmit or receive operation may be active at a time.
- The module directly programs PPI channels instead of using a resource
  allocator.

## Test

The manual two-board communication test is documented in
`tests/sys/nrf_sf_radio/README.md`.
