Program the reset pin for nRF52x CPUs
=====================================

Simply compile, flash, and run this tool on your nRF52x-based board. It will
program the given `RESET_PIN` into the NRF_UICR->PSELRESET registers, hence
allowing for hardware resets using a button connected to that pin.

> [!WARNING]
> This tool only works with early nRF52 chips with build codes before Fxx.
> Later build codes automatically erase the UICR register on each flashing
> operation.

Context
=======
For nRF52x CPUs, the reset pin is programmable. The reset pin configuration is
store in two persistent registers, that are programmed in the same way as the
CPUs flash memory.

In most cases, these values should be readily programmed when you get your
board, and thus do not need to be touched. However, we have seen however a
number of nrf52xxxdk boards, where this was not the case and the on-board reset
button did have no effect. Running this tool on those boards solves the issue.
