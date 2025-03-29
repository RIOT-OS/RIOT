@defgroup boards_common_nucleo_flashing Flashing the Nucleo
@ingroup boards_common_nucleo32
@ingroup boards_common_nucleo64
@ingroup boards_common_nucleo144
@brief Flashing ST Nucleo Development Boards

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command (replace the `xxxxx` with your Nucleo variant):

```
make BOARD=nucleo-xxxx PROGRAMMER=cpy2remed flash
```

@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).


## Flashing the Board Using OpenOCD

The ST Nucleo64 board includes an on-board ST-LINK programmer and can be
flashed using OpenOCD (use version 0.11.0 at least).

To flash this board, just use the following command (replace the `xxxxx`
with your Nucleo variant):

```
make BOARD=nucleo-xxxxx flash -C examples/basic/hello-world
```
