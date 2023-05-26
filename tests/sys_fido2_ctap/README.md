# Overview

This test application tests FIDO2 CTAP2 functionality without a transport layer being used.

To execute the test run e.g.

```
BOARD = nrf52840dk make flash test
```

To generate a new test case array run:
* Note: This requires the [python-fido lib](https://github.com/Yubico/python-fido2)

```
python3 gen_test_case.py
```