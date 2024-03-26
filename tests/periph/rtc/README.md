Expected result
===============

When running the test application execute the following commands and verify the
output. If `periph_rtc_mem` is not supported on the particular board go straight
to step 8.

1. `rtc_clear` returns `RTC mem cleared`
2. `rtc_verify` returns `RTC mem content does not match`
3. `rtc_write` returns `RTC mem set`
4. `rtc_verify` returns `RTC mem OK`
5. `rtc_reboot` reboots device
6. `rtc_verify` returns `RTC mem OK`
7. `rtc_clear` returns `RTC mem cleared`. This is to clear RTC for the future.
8. `rtc_test_alarms` returns

```
This test will display 'Alarm!' every 2 seconds for 4 times
Clock value is now   2024-02-20 15:41:43.176
  Setting clock to   2020-02-28 23:59:57
Clock value is now   2020-02-28 23:59:57.176
  Setting alarm to   2020-02-28 23:59:59
   Alarm is set to   2020-02-28 23:59:59
  Alarm cleared at   2020-02-28 23:59:57.176
       No alarm at   2020-02-28 23:59:59.176
  Setting alarm to   2020-02-28 23:59:61

Alarm!
Alarm!
Alarm!
Alarm!
```

This procedure is also included in the `tests/01-run.py` so `make test` on a
board that supports the `periph_rtc_mem` feature will execute the above steps.
On a board without `periph_rtc_mem` support, it will just execute the
`rtc_test_alarms`.


Background
==========
Test for the low-level RTC driver.
