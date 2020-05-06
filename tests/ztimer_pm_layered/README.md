# Introduction

This test application runs an endless loop of ztimer_sleep() based on the clocks
ZTIMER_USEC and ZTIMER_MSEC alternating.

If the board running this application has configured
CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE, it should consume less power when sleeping
on the ZTIMER_MSEC clock.
