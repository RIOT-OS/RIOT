LittlevGL sample application
============================

This application shows a basic usage of LittlevGL with RIOT. It's simply an
adaption of one of the upstream examples: the
[sysmon example](https://github.com/lvgl/lv_apps/tree/master/src/lv_sysmon).

### Flashing the application

The application works without modification on the stm32f429i-disc1 board. To
build, flash and run the application for this board, just use:

```
make BOARD=stm32f429i-disc1 -C tests/pkg_lvgl flash
```

### Expected result

The application displays a basic system monitor with the CPU and memory
usage of the board.
Note that on the pinetime, due to a too small screen resolution, some
information is cropped.
