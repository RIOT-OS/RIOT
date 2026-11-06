@defgroup    boards_reel Phytec reel board
@ingroup     boards
@brief       Support for the PHYTEC 'reel board'

## Overview

The 'reel board' is an IoT development platform based on Nordic's nRF52840 SoC.
It was developed by PHYTEC in cooperation with the Zephyr project.

@see https://www.phytec.eu/product-eu/internet-of-things/reelboard/
@see https://docs.zephyrproject.org/latest/boards/arm/reel_board/doc/reel_board.html

## Buttons

The board has a user button (S5) connected to P0.07 and exposed as `BTN0`.
The reset button (S4) is connected to P0.18 and resets the nRF52840.
