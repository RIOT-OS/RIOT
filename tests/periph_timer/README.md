# Periph Timer Test

## About

This application will test all configured peripheral timers of the target
platform. For each timer, it will set each channel with an incrementing
timeout: CH0 is set to 5ms, CH1 to 10ms, CH2 to 15ms and so on.

## Expected Result

The output should show that every channel fired after an evenly distributed
amount of time, i.e. the diff values should be equal (with some jitter...).

## Note

This test does however **NOT** show whether the timeouts and diffs were correct
in relation to the expected real-time; use e.g. tests/xtimer_msg for this.
