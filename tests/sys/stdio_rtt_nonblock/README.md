# stdio_rtt non-blocking output test

## Introduction

By default, `stdio_rtt` writes stdout without blocking: when no RTT host is
reading the up buffer, output that does not fit is dropped. This test checks
that a caller that loops until all of its output is written still makes
progress in that case. Such loops are used, for example, by `write_all()` in
`pkg/mpaland-printf`.

The main thread writes a numbered line every 100 ms with such a loop. A second
thread of the same priority toggles LED0 every 250 ms.

## Running the test

Flash the application to a board with a J-Link (or another RTT-capable debug
probe) and a LED, and do **not** start an RTT reader:

    make BOARD=<board> JLINK_SERIAL=<serial> flash

## Expected result

LED0 keeps blinking with no RTT host attached. After starting an RTT reader,
for example with `make BOARD=<board> JLINK_SERIAL=<serial> term`, numbered
lines appear; lines written while no reader was attached are missing.

If `stdio_write()` returns 0 when the up buffer is full, the main thread spins
in its write loop once the buffer has filled, and LED0 stops blinking after
about half a second.
