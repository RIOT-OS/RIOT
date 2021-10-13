# About
This application demonstrates the usage of the `NimBLE` BLE stack as a scanner.
It makes use of the RIOT specific `nimble_scanner` adaption module, abstracting
`NimBLE`s APIs for easier access.

# Usage
This example provides a `scan` shell command. Issue `scan help` for more
information on its usage.

Example output:
```
> 2019-03-26 14:59:33,158 - INFO #  main(): This is RIOT! (Version: X)
2019-03-26 14:59:33,161 - INFO # NimBLE Scanner Example Application
2019-03-26 14:59:33,164 - INFO # Type `scan help` for more information
> scan help
2019-03-26 15:00:29,214 - INFO #  scan help
2019-03-26 15:00:29,216 - INFO # usage: scan [timeout in ms]
> scan 50
2019-03-26 14:59:41,289 - INFO #  scan 50
2019-03-26 14:59:41,343 - INFO # Scanning for 50ms now ... done
2019-03-26 14:59:41,343 - INFO #
2019-03-26 14:59:41,344 - INFO # Results:
2019-03-26 14:59:41,351 - INFO # [ 0] 3c:8a:28:86:40:90 (RANDOM)  "undefined", adv_msg_cnt: 1, adv_int: 0us, last_rssi: -59
2019-03-26 14:59:41,359 - INFO # [ 1] 5c:8a:fd:28:9f:c5 (RANDOM)  "undefined", adv_msg_cnt: 1, adv_int: 0us, last_rssi: -87
2019-03-26 14:59:41,367 - INFO # [ 2] 22:e6:6e:3a:a8:74 (RANDOM)  "undefined", adv_msg_cnt: 1, adv_int: 0us, last_rssi: -84
2019-03-26 14:59:41,375 - INFO # [ 3] 15:03:93:bb:8b:98 (RANDOM)  "undefined", adv_msg_cnt: 1, adv_int: 0us, last_rssi: -85
2019-03-26 14:59:41,383 - INFO # [ 4] 61:3a:2e:73:9d:74 (RANDOM)  "undefined", adv_msg_cnt: 1, adv_int: 0us, last_rssi: -63
2019-03-26 14:59:41,384 - INFO #
```
