## Requirements

- currently, the host side only compiles on Linux

## Usage

To use, add

    #
    GNRC_NETIF_NUMOF := 2
    USEMODULE += ethos gnrc_netdev2
    CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO

to app Makefile, "make clean all flash", then run this tool so follows:
    # sudo ./ethos <tap-device> <serial>
