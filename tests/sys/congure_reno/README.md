Tests for the CongURE TCP Reno implementation
=============================================

This test tests the `congure_reno` implementation.

Usage
-----

The test requires an up-to-date version of `riotctrl` with `rapidjson` support:

```console
$ pip install --upgrade riotctrl[rapidjson]
```

Then simply run the application using:

```console
$ BOARD="<board>" make flash test
```

It can also executed with pytest:

```console
$ pytest tests/01-run.py
```

Expected result
---------------

The application's test script passes without error code.
