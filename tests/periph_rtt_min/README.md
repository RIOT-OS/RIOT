## About

This applications is meant to determine RTT_MIN_OFFSET for a specific BOARD.

The application will iteratively set an alarm starting from 0 ticks offset
until the alarm actually triggers. Every time the alarm underflows the
alarm will not be triggered within the expected time, so the application
will set another one with a larger offset until the alarm successfully triggers.

The rtt might advance between the call to rtt_get_counter() and
rtt_set_alarm(). If that happens with val=1 then the alarm would be set
to the current time and underflow. The test is ran over multiple samples
to make this more likely to happen. Nonetheless its always possible
that now sample will underflow so a conservatory value would be to
set RTT_MIN_OFFSET to the value found out with this test + 1tick.

## Usage

Run `BOARD=<board> make -C tests/periph_rtt_min/ flash test` the value will
be printed as:

```
Evaluate RTT_MIN_OFFSET over 1024 samples
........................................................................
........................................................................
........................................................................
........................................................................
........................................................................
RTT_MIN_OFFSET for <board>: 2
```
