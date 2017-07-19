This test app allows to unblock power modes using `pm_unblock` from `pm_layered`
module during a given amount of time.

The time is measured using RTC as many RTC can wake up cpu from deep power
saving modes.

The RTC alarm callback then block the power mode using `pm_block`.
