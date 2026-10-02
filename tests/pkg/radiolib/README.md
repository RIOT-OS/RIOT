<!--
SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
SPDX-License-Identifier: LGPL-2.1-only
-->
## RadioLib PingPong

This test application verifies that the `radiolib` package works: the radio is
brought up by `auto_init_radiolib` and used through the global `radiolib_radio`
object

Two boards are needed, one flashed as Ping (initiator), one as Pong
(responder). Ping transmits `Ping`, waits 5 s for an answer and repeats every
second; Pong waits for a packet and replies `Pong`.

## Usage

Board A (initiator):

```
make BOARD=<board> PING=1 -C tests/pkg/radiolib all flash term
```

Board B (responder):

```
make BOARD=<board> -C tests/pkg/radiolib all flash term
```

The default board is `b-l072z-lrwan1`, which carries an SX1276.

Add `CFLAGS+=-DENABLE_DEBUG=1` to also print the TX/RX state, RSSI and SNR of
each packet.

## Configuration

| Variable | Default | Purpose |
| --- | --- | --- |
| `RADIOLIB_DRIVER` | `radiolib_sx127x` | transceiver driver module |
| `PING` | unset | build as Ping sender instead of Pong sender |

Pins, SPI bus and carrier frequency come from the `CONFIG_RADIOLIB_*` macros
in `pkg/radiolib/include/radiolib_riotos_params.h`, which default to the
board's `SX127X_PARAM_*` definitions. Override them from the board header or
via `CFLAGS` if your wiring differs:

```
make BOARD=<board> CFLAGS='-DCONFIG_RADIOLIB_FREQ=433.0' ...
```

## Expected results

Both boards print `[radiolib] radio ready` on startup. Once the second board
is running, Ping shows:

```
[radiolib] radio ready
[radiolib] Received: Pong
[radiolib] Received: Pong
```

and Pong shows:

```
[radiolib] radio ready
[radiolib] Received: Ping
[radiolib] Received: Ping
```

If `[radiolib] radio auto-init failed` is printed instead, the
SPI or pin configuration does not match the wiring. You can see
debug traces by setting `#define ENABLE_DEBUG=1` in `main.cpp`.
