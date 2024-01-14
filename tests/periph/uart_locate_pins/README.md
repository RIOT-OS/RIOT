# Utility to locate GPIO pins

This application bit-bangs the name of every GPIO pin via `soft_uart` on each
GPIO. Connect an UART adapter to one pin at a time configured at symbol rate
of 9600 Bd and check the output to know which pin you are connected to.
