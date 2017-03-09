## Requirements

- currently, the host side only compiles on Linux

## Usage

To use, add

    #
    GNRC_NETIF_NUMOF := 2
    USEMODULE += ethos ethos_stdio gnrc_netdev2
    CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200

to app Makefile, "make clean all flash", then run this tool so follows:
    # sudo ./ethos <tap-device> <serial>
