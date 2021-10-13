/**
@defgroup    boards_sodaq-explorer SODAQ ExpLoRer
@ingroup     boards
@brief       Support for the SODAQ ExpLoRer board

### General information

General information about this board can be found on the
[SODAQ support](http://support.sodaq.com/sodaq-one/explorer/) website.

### Flash the board

Use `BOARD=sodaq-explorer` with the `make` command.<br/>
Example with `hello-world` application:
```
     make BOARD=sodaq-explorer -C examples/hello-world flash
```

@note     If the application crashes, automatic reflashing via USB, as explained
          above won't be possible. In this case, the board must be set in
          bootloader mode by double tapping the reset button before running the
          flash command.

### Accessing STDIO via UART

STDIO of RIOT is directly available over the USB port.

The `TERM_DELAY` environment variable can be used to add a delay (in second)
before opening the serial terminal. The default value is 2s which should be
enough in most of the situation.
 */
