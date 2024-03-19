# Semihosting STDIO test

This test aims to test ARM semihosting based STDIO on RIOT. The test is manual,
starting a debugger on the CI for automated test is not supported.

## Usage

1. Flash the test on a board with an ARM-based MCU.
2. Use `make term RIOT_TERMINAL=semihosting` to start a debug session with
   semihosting enabled.
3. Restart the microcontroller via the debug session.

The shell prompt should be available in the GDB session.
