@defgroup    boards_nrf9160dk-ns nRF9160DK (non-secure)
@ingroup     boards
@brief       Support for the nRF9160DK board running as TrustZone-M non-secure image

### General information

This board target runs RIOT on the nRF9160DK as the ARM TrustZone-M *non-secure*
image. It is the non-secure counterpart of @ref boards_nrf9160dk: peripherals are
accessed through their non-secure aliases (`NRF_x_NS`), and cryptographic
operations are delegated to a secure-world image through the PSA Crypto API.

A secure image has to be present in flash; the non-secure application does not
boot on its own. The secure image also configures which flash, RAM and
peripherals are handed over to the non-secure world.

For general information about the hardware (LEDs, buttons, documentation), refer
to @ref boards_nrf9160dk.

### Links

- [nRF9160dk web page](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk)
- [documentation and hardware description](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrf91_dk%2FUG%2Fnrf91_DK%2Fintro.html&cp=2_0_4)

### Flash the board

The secure and the non-secure image are built, merged and flashed together using
the `tee-flash` target:

```shell
make BOARD=nrf9160dk-ns -C tests/sys/psa_riot_tee tee-flash
```

This requires `mergehex` (shipped with the Nordic command line tools) to be
installed and available in `PATH`.

### Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>

```shell
make BOARD=nrf9160dk-ns -C tests/sys/psa_riot_tee term
```
