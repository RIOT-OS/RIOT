@defgroup       sys_jelly Jelly
@ingroup        sys

@brief          Use the Jelly tool to speak Slipmux with RIOT
@author         Bennet Hattesen <bennet.hattesen@haw-hamburg.de>

## About

The [slipmux](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03)
protocol allows to speak stdio, CoAP and IP over a single UART.
RIOT implements it in the `slipmux` driver with the modules `stdio_slipdev`,
`slipdev_config` and `slipdev_net`.
On the host side, the [Jelly](https://github.com/Teufelchen1/jelly) tool
can be used to interface with a slipmux speaking device.

### Usage

Install Jelly on your computer by either grabbing it from
[crates.io](https://crates.io/crates/Jelly) via `cargo install Jelly` or
build if from the [source](https://github.com/Teufelchen1/jelly).

Add `USEMODULE += jelly` to your RIOT app makefile. Among other things,
this will enable `stdio` and CoAP through `slipmux`. The backend for CoAP is `unicoap`.
If you also want networking, you need to add `USEMODULE += slipdev_net` as well.

We can now build, flash and open the Jelly shell:
```shell
USEMODULE=jelly BOARD=nrf52840dk make flash term
```

If you want to use networking through Jelly, add the `slipdev_net` module
and tell Jelly which TUN interface to use:
```shell
TERMFLAGS=--network=slip0 USEMODULE="jelly slipdev_net" BOARD=nrf52840dk make flash term
```

Of course you can also manually run Jelly:
```shell
Jelly --help
```
