@defgroup    boards_i-nucleo-lrwan1 ST I-NUCLEO-LRWAN1 LoRa board
@ingroup     boards
@brief       Support for the ST I-NUCLEO-LRWAN1 LoRa board shield

## Description

<img src="https://www.st.com/content/ccc/fragment/product_related/rpn_information/board_photo/group0/d0/d8/be/59/bb/0f/42/d9/i-nucleo-lrwan1.jpg/files/i-nucleo-lrwan1.jpg/_jcr_content/translations/en.i-nucleo-lrwan1.jpg"
     alt="I-NUCLEO-LRWAN1" style="width:400px;"/>

The [I-NUCLEO-LRWAN1](https://www.st.com/en/evaluation-tools/i-nucleo-lrwan1.html)
board provides LoRa connectivity with an SX1272 radio connected via SPI
to an ultra-low power
[STM32L052T8](https://www.st.com/en/microcontrollers-microprocessors/stm32l052t8.html).
microcontroller.

For details, the
[schematics are available on GitHub](https://github.com/USIWP1Module/USI_I-NUCLEO-LRWAN1/blob/master/Schematics/USI%20LoRa%20Arduino%20shield_SCH_20161115-1.pdf).

_Note_: to use the on-board I2C sensors, the R19 and R20 shorts must be closed
(soldered).

## Flashing the board

To flash, one needs to use an external ST-Link programmer connected to SWD pins
of the JP6 connector.

By default, the flash on the microcontroller is write protected so before being
able to flash it, one needs to unlock using the following:
- Apply the following patch to the `openocd.sh` script:
```diff
diff --git a/dist/tools/openocd/openocd.sh b/dist/tools/openocd/openocd.sh
index c59a1939a2..0c359e438c 100755
--- a/dist/tools/openocd/openocd.sh
+++ b/dist/tools/openocd/openocd.sh
@@ -248,6 +248,11 @@ do_flash() {
             -c 'init' \
             -c 'targets' \
             -c 'reset halt' \
+            -c 'stm32lx unlock 0' \
+            -c 'reset halt' \
+            -c 'init' \
+            -c 'targets' \
+            -c 'reset halt' \
             ${OPENOCD_PRE_FLASH_CMDS} \
             -c 'flash write_image erase \"${IMAGE_FILE}\" ${IMAGE_OFFSET} ${IMAGE_TYPE}' \
             ${OPENOCD_PRE_VERIFY_CMDS} \
```
- Run make flash:
```sh
make BOARD=i-nucleo-lrwan1 -C examples/basic/hello-world flash
```
  The command will fail but after that the memory will be unlocked after a
  power cycle. The line added above in `openocd.sh` can also be removed.
- Unplug the board and replug-it: the board is now flashable normally.

Note that this unlock procedure only needs to be done once.

## Accessing STDIO

STDIO is available on pin 0 and 1 on CN9 so one needs an USB to UART to connect
to STDIO.

When flashing using an ST-Link, STDIO pins can be plugged directly to the RX/TX
pins on the programmer, STDIO is then accessible like on any Nucleo boards.
