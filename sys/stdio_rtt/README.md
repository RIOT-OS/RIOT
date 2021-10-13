# RTT STDIO

This module will allow communication using SEGGER's Real Time Terminal protocol.
Briefly, it replaces UART stdio with a set of ringbuffers that are manipulated
over JTAG. There are several advantages to this system. The biggest is that
writing to stdout is extremely fast (as you are just copying to memory). This
is useful if you are adding print statements in timing-sensitive code as part
of debugging. The other advantage is that it frees your UART for other use
and enables stdio on platforms that do not have a UART.

To use this module, add

```
USEMODULE += stdio_rtt
```

to your makefile. By default the module will drop bytes written to stdout if the
buffer is full. If you know for certain that the debugger is attached, you
can obtain lossless stdout by adding

```
CFLAGS += -DSTDIO_RTT_ENABLE_BLOCKING_STDOUT
```

to your makefile. Note well that if you do NOT plug in the debugger and run
the SEGGER RTT software (or compatible software) this will then lock up the
system as it waits forever. Typically you would only define this during
development on the lab bench.

If you are printing significant data out (pages a second), you can increase
your stdout bandwidth by lowering the poll interval. The default is 50ms.
A choice of 5ms is good during printf-heavy debugging:

```
CFLAGS += -DSTDIO_POLL_INTERVAL=5000U
```

SEGGER RTT supports stdin as well, and this is enabled by default. It requires
polling the stdin ringbuffer, however, which on low duty cycle systems
can increase the number of unnecessary wakeups from sleep. To disable stdin,
add this to your makefile:

```
CFLAGS += -DSTDIO_RTT_DISABLE_STDIN
```
