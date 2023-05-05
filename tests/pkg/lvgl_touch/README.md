LittlevGL sample application
============================

This application shows a usage of LittlevGL with touch capabilities.

### Flashing the application

The application works without modification on the stm32f429i-disc1 board. To
build, flash and run the application for this board, just use:

```
make BOARD=stm32f429i-disc1 -C tests/pkg_lvgl_touch flash
```

### Expected result

The application simply displays a button that can be clicked. Each time the
button is clicked, the message "Button clicked!" is displayed in the terminal.
