This is basically the same tests as for the normal shell, but here we are
testing via blueooth instead of UART. You have to set up a virtual serial port
manually.

For instructions on how to open a virtual serial port to your bluetooth device
see `sys/stdio_nimble/README`.

**Note:** `make term` and `make test-with-config` will open two different types of terminals.
- When calling `make term` then a terminal will communicate with the board
via UART. Due to the nature of `stdio_nimble` the board won't respond to input
coming from here)
- When calling `make test-with-config` then picocom will communicate with the board via the
given virtual serial port

So a procedure to run this test could be:
0. Make sure that the current test application instance is fresh and no test was
run on it before. Otherwise your test might fail, because the test case
`check_control_d` only works once per run.
1. Execute `make flash term`
2. Open a virtual serial port with `ble-serial` and note the virtual serial port
that was created (search for `Slave created on /tmp/dev_riot_ble -> /dev/pts/25`)
3. Execute `PORT=/dev/pts/25 make test-with-config`
