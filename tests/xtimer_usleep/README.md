# xtimer_usleep test application

This test tests `xtimer_usleep()` against the timings of `xtimer_now_usec()`
and by comparing the incoming values with the test hosts time.

The sleep times can be probed with a oscilloscope at a pin if `SLEEP_PIN` is set
and the respective gpio `SLEEP_PORT` is defined in the makefile, the port
information can be found in the enum in `cpu/include/periph_cpu.h`.

```
FEATURES_REQUIRED += periph_gpio
CFLAGS += -DSLEEP_PIN=7
CFLAGS += -DSLEEP_PORT=PORT_F
```

## Usage
Executed from the project directory
```
make BOARD=<Board Name> flash test
```

### Expected result running
```
XXX-XX-XX XX:XX:XX,XXX - INFO # Connect to serial port /dev/ttyACM0
Welcome to pyterm!
Type '/exit' to exit.
XXXX-XX-XX XX:XX:XX,XXX - INFO # main(): This is RIOT! (Version: XXX )
XXXX-XX-XX XX:XX:XX,XXX - INFO # Running test 5 times with 7 distinct sleep times
XXXX-XX-XX XX:XX:XX,XXX - INFO # Please hit any key and then ENTER to continue
a
a
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10232 us (expected: 10000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 50232 us (expected: 50000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10464 us (expected: 10234 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 57008 us (expected: 56780 us) Offset: 228 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 12352 us (expected: 12122 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 98992 us (expected: 98765 us) Offset: 227 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 75232 us (expected: 75000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10232 us (expected: 10000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 50224 us (expected: 50000 us) Offset: 224 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10464 us (expected: 10234 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 57008 us (expected: 56780 us) Offset: 228 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 12352 us (expected: 12122 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 98992 us (expected: 98765 us) Offset: 227 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 75232 us (expected: 75000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10232 us (expected: 10000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 50232 us (expected: 50000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10456 us (expected: 10234 us) Offset: 222 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 57008 us (expected: 56780 us) Offset: 228 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 12352 us (expected: 12122 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 98992 us (expected: 98765 us) Offset: 227 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 75232 us (expected: 75000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10232 us (expected: 10000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 50232 us (expected: 50000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10456 us (expected: 10234 us) Offset: 222 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 57008 us (expected: 56780 us) Offset: 228 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 12352 us (expected: 12122 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 98992 us (expected: 98765 us) Offset: 227 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 75232 us (expected: 75000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10232 us (expected: 10000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 50232 us (expected: 50000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10464 us (expected: 10234 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 57008 us (expected: 56780 us) Offset: 228 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 12352 us (expected: 12122 us) Offset: 230 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 98992 us (expected: 98765 us) Offset: 227 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 75224 us (expected: 75000 us) Offset: 224 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Test ran for 2056976 us
```

### On Error with pyterm
```
XXX-XX-XX XX:XX:XX,XXX - INFO # Connect to serial port /dev/ttyACM0
Welcome to pyterm!
Type '/exit' to exit.
XXXX-XX-XX XX:XX:XX,XXX - INFO # main(): This is RIOT! (XXX)
XXXX-XX-XX XX:XX:XX,XXX - INFO # Running test 5 times with 7 distinct sleep times
XXXX-XX-XX XX:XX:XX,XXX - INFO # Please hit any key and then ENTER to continue
a
a
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 10224 us (expected: 10000 us) Offset: 224 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 50232 us (expected: 50000 us) Offset: 232 us
XXXX-XX-XX XX:XX:XX,XXX - INFO # Slept for 1464 us (expected: 1234 us) Offset: 230 us
Invalid timeout 1464 ,expected 1172 < 1234 < 1295
Host max error  61
error           291
```
