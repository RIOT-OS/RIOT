About
=====
This test application allows you to type `ntpdate <server>` and get the current
date + the offset in microseconds of the NTP timestamp (seconds from 1900-01-01)
from the current `xtimer_now_usec64()` back utilizing SNTP. Make sure that
`<server>` is reachable and has an NTP daemon installed.
