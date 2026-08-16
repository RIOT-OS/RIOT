# Testing rapidly received packets while sending replies

## Usage

This test is supposed to be used in tandem with [netdev_flood_flooder].

It is supposed to test a bug in network devices where an incoming packet is
overwitten by a sent packet before it is read by the upper layer.

Start the flooder first (see README there) and then flash this application and
run the test

```sh
make flash
make test
```

If there is no output for a while, the test was successful.

## Support for more devices
Currently the following devices are supported with this test:

- `at86rf2xx`

To extend the coverage of this test, just add the setup, sending, and reception
behavior for your device in the functions it is documented

```C
/**
 * @note    Please amend for more device support
 */
```

[netdev_flood_flooder]: ../netdev_flood_flooder/
